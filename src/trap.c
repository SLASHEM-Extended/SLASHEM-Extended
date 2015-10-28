/*	SCCS Id: @(#)trap.c	3.4	2003/10/20	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

extern const char * const destroy_strings[];	/* from zap.c */

STATIC_DCL void FDECL(rocks_fall, (int, int));

STATIC_DCL void FDECL(dofiretrap, (struct obj *));
STATIC_DCL void FDECL(doplasmatrap, (struct obj *));
STATIC_DCL void FDECL(doicetrap, (struct obj *));
STATIC_DCL void FDECL(doshocktrap, (struct obj *));
STATIC_DCL void FDECL(dovolttrap, (struct obj *));
STATIC_DCL void FDECL(doshittrap, (struct obj *));
STATIC_DCL void NDECL(domagictrap);
STATIC_DCL boolean FDECL(emergency_disrobe,(boolean *));
STATIC_DCL int FDECL(untrap_prob, (struct trap *ttmp));
STATIC_DCL void FDECL(cnv_trap_obj, (int, int, struct trap *));
STATIC_DCL void FDECL(move_into_trap, (struct trap *));
STATIC_DCL int FDECL(try_disarm, (struct trap *,BOOLEAN_P));
STATIC_DCL void FDECL(reward_untrap, (struct trap *, struct monst *));
/*
STATIC_DCL int FDECL(disarm_holdingtrap, (struct trap *));
STATIC_DCL int FDECL(disarm_unknowntrap, (struct trap *));
STATIC_DCL int FDECL(disarm_fartingtrap, (struct trap *));
STATIC_DCL int FDECL(disarm_active_superscroller, (struct trap *));
STATIC_DCL int FDECL(disarm_trap_percents, (struct trap *));
STATIC_DCL int FDECL(disarm_rust_trap, (struct trap *));
STATIC_DCL int FDECL(disarm_acid_trap, (struct trap *));
STATIC_DCL int FDECL(disarm_water_trap, (struct trap *));
STATIC_DCL int FDECL(disarm_heel_trap, (struct trap *));
STATIC_DCL int FDECL(disarm_glyph_trap, (struct trap *), int);
STATIC_DCL int FDECL(disarm_blade_trap, (struct trap *));
STATIC_DCL int FDECL(disarm_fire_trap, (struct trap *));
STATIC_DCL int FDECL(disarm_landmine, (struct trap *));
STATIC_DCL int FDECL(disarm_squeaky_board, (struct trap *));
STATIC_DCL int FDECL(disarm_shooting_trap, (struct trap *, int));
*/
STATIC_DCL int FDECL(try_lift, (struct monst *, struct trap *, int, BOOLEAN_P));
STATIC_DCL int FDECL(help_monster_out, (struct monst *, struct trap *));
STATIC_DCL boolean FDECL(thitm, (int,struct monst *,struct obj *,int,BOOLEAN_P));
STATIC_DCL int FDECL(mkroll_launch,
			(struct trap *,XCHAR_P,XCHAR_P,SHORT_P,long));
STATIC_DCL boolean FDECL(isclearpath,(coord *, int, SCHAR_P, SCHAR_P));
#ifdef STEED
STATIC_OVL int FDECL(steedintrap, (struct trap *, struct obj *));
STATIC_OVL boolean FDECL(keep_saddle_with_steedcorpse,
			(unsigned, struct obj *, struct obj *));
#endif

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

static const char * const farttrapnames[] = {
/* always 5 names per line, because we'll need the index numbers later --Amy */

/* quiet */
"Nadja", "Mailie", "Elif", "Solvejg", "Sueschen", 
"Jessica", "Yvonne", "Patricia", "Jennifer", "Inge", 
"Sarah", /* 0-10 */

/* normal */
"Kati", "Maurah", "Eveline", "Larissa", "Sandra", 
"Meltem", "Kerstin", "Karin", "Ina", "Lou", 
"Lisa", "Miriam", "Elena", "Katharina", "Simone", 
"Jasieen", "Marike", "Sue Lyn", /* 11-28 */

/* loud */
"Sunali", "Thai", "Klara", "Ludgera", "Johanetta", 
"Antje", "Ruea", "Mariya", "Wendy", "Katia", /* 29-38 */

};

#ifdef OVLB

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

/* called when you're hit by fire (dofiretrap,buzz,zapyourself,explode) */
boolean			/* returns TRUE if hit on torso */
burnarmor(victim)
struct monst *victim;
{
    struct obj *item;
    char buf[BUFSZ];
    int mat_idx;



	if ((victim == &youmonst) && !rn2(2) ) make_burned(HBurned + rnd(20 + (monster_difficulty() * 5) ),TRUE);
    
    if (!victim) return 0;
#define burn_dmg(obj,descr) rust_dmg(obj, descr, 0, FALSE, victim)
    while (1) {
	switch (rn2(5)) {
	case 0:
	    item = (victim == &youmonst) ? uarmh : which_armor(victim, W_ARMH);
	    if (item) {
		mat_idx = objects[item->otyp].oc_material;
	    	Sprintf(buf,"%s helmet", materialnm[mat_idx] );
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
#ifdef TOURIST
	    item = (victim == &youmonst) ? uarmu : which_armor(victim, W_ARMU);
	    if (item)
		(void) burn_dmg(item, "shirt");
#endif
	    return TRUE;
	case 2:
	    item = (victim == &youmonst) ? uarms : which_armor(victim, W_ARMS);
	    if (!burn_dmg(item, "wooden shield")) continue;
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
genericptr_t poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;

	if (/*nexttodoor(x, y) || */(rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
do_floode(x, y, poolcnt)
int x, y;
genericptr_t poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;

	if (/*nexttodoor(x, y) || */(rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
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
	boolean grprot = FALSE;
	boolean is_primary = TRUE;
	boolean vismon = (victim != &youmonst) && canseemon(victim);
	int erosion;

	if (!otmp) return(FALSE);
	if (stack_too_big(otmp)) return (FALSE);
	switch(type) {
		case 0: vulnerable = is_flammable(otmp);
			break;
		case 1: vulnerable = is_rustprone(otmp);
			grprot = TRUE;
			break;
		case 2: vulnerable = is_rottable(otmp);
			is_primary = FALSE;
			break;
		case 3: vulnerable = is_corrodeable(otmp);
			grprot = TRUE;
			is_primary = FALSE;
			break;
	}
	erosion = is_primary ? otmp->oeroded : otmp->oeroded2;

	if (!print && (!vulnerable || otmp->oerodeproof /* || erosion == MAX_ERODE*/ ))
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
	    if (grprot && otmp->greased) {
		grease_protect(otmp,ostr,victim);
	    } else if (otmp->oerodeproof || (otmp->blessed && !rnl(4))) {
		if (flags.verbose) {
		    if (victim == &youmonst)
			pline("Somehow, your %s %s not affected.",
			      ostr, vtense(ostr, "are"));
		    else if (vismon)
			pline("Somehow, %s's %s %s not affected.",
			      mon_nam(victim), ostr, vtense(ostr, "are"));
		}
	    } else {
		if (victim == &youmonst)
		    Your("%s %s%s!", ostr,
			 vtense(ostr, action[type]),
			 erosion+1 == MAX_ERODE ? " completely" :
			    erosion ? " further" : "");
		else if (vismon)
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
	} else if (!otmp->oartifact) {
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
	boolean grprot = FALSE;
	boolean is_primary = TRUE;
	boolean vismon = (victim != &youmonst) && canseemon(victim);
	int erosion;

	if (!otmp) return(FALSE);
	if (stack_too_big(otmp)) return (FALSE);
	switch(type) {
		case 0: vulnerable = TRUE;
			break;
		case 1: vulnerable = TRUE;
			grprot = TRUE;
			break;
		case 2: vulnerable = TRUE;
			is_primary = FALSE;
			break;
		case 3: vulnerable = TRUE;
			grprot = TRUE;
			is_primary = FALSE;
			break;
	}
	erosion = is_primary ? otmp->oeroded : otmp->oeroded2;

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
	    if (grprot && otmp->greased) {
		grease_protect(otmp,ostr,victim);
	    } else if (otmp->blessed && !rnl(4)) {
		if (flags.verbose) {
		    if (victim == &youmonst)
			pline("Somehow, your %s %s not affected.",
			      ostr, vtense(ostr, "are"));
		    else if (vismon)
			pline("Somehow, %s's %s %s not affected.",
			      mon_nam(victim), ostr, vtense(ostr, "are"));
		}
	    } else {
		if (victim == &youmonst)
		    Your("%s %s%s!", ostr,
			 vtense(ostr, action[type]),
			 erosion+1 == MAX_ERODE ? " completely" :
			    erosion ? " further" : "");
		else if (vismon)
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
	} else if (!otmp->oartifact) {
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
	if (!rn2(2) && !stack_too_big(otmp) ) {
	    otmp->greased = 0;
	    if (carried(otmp)) {
		pline_The("grease dissolves.");
		update_inventory();
	    }
	}
}

void
makerandomtrap_at(x,y)
register int x, y;
{

	int rtrap;
	rtrap = randomtrap();
	maketrap(x,y,rtrap);
	return;

}

struct trap *
maketrap(x,y,typ)
register int x, y, typ;
{
	register struct trap *ttmp;
	register struct rm *lev;
	register boolean oldplace;

	if ((ttmp = t_at(x,y)) != 0) {
	    if (ttmp->ttyp == MAGIC_PORTAL) return (struct trap *)0;
	    oldplace = TRUE;
	    if (u.utrap && (x == u.ux) && (y == u.uy) &&
	      ((u.utraptype == TT_BEARTRAP && typ != BEAR_TRAP) ||
	      (u.utraptype == TT_WEB && typ != WEB) ||
	      (u.utraptype == TT_PIT && typ != PIT && typ != SPIKED_PIT && typ != GIANT_CHASM && typ != SHIT_PIT)))
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
	switch(typ) {
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
		    (void) add_to_container(statue, otmp);
		}
		statue->owt = weight(statue);
		if (!rn2(5)) statue->oinvis = 1; /* player needs see invisible to detect this thing --Amy */
		if (!rn2(500)) {statue->oinvis = 1; statue->oinvisreal = 1;} /* not detectable at all */

	/* "Stop Fungi/Objects from hiding statue traps. It's a bit of a balance issue and created clutter, and had a good chance of spawning out of depth monsters." In Soviet Russia, people want to be able to see that statue of a slimy vortex in advance. They don't like games being difficult, and they don't want any nasty surprises, so they make sure that statue traps are never obscured by anything. I wonder if the next thing they'll do is to remove all the other traps because you can't see them in advance, or even make sure all of them are visible before you trigger them, too? Completely missing the point of traps, but oh well... --Amy */

		else if (!rn2(10) && !issoviet) (void) mkobj_at(COIN_CLASS, x, y, TRUE); /* hidden underneath a zorkmid */
		else if (!rn2(10) && !issoviet) (void) mksobj_at(BOULDER, x, y, TRUE, FALSE); /* hidden underneath a boulder */
		else if (!rn2(10) && !issoviet) (void) mkobj_at(0, x, y, TRUE); /* hidden underneath a random item */
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

	    case HEEL_TRAP:
		{

		ttmp->launch_otyp = rnd(15);

		break;
	      }

	    case FART_TRAP:
		{

		ttmp->launch_otyp = rn2(SIZE(farttrapnames));

		break;
	      }

	    case MAGIC_BEAM_TRAP:
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

				switch (rnd(96)) {

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
	    case SHAFT_TRAP:
	    case TRAPDOOR:
		lev = &levl[x][y];
		if (*in_rooms(x, y, SHOPBASE) &&
			((typ == HOLE || typ == TRAPDOOR || typ == SHAFT_TRAP) ||
			 IS_DOOR(lev->typ) || IS_WALL(lev->typ)))
		    add_damage(x, y,		/* schedule repair */
			       ((IS_DOOR(lev->typ) || IS_WALL(lev->typ))
				&& !flags.mon_moving) ? 200L : 0L);
		lev->doormask = 0;	/* subsumes altarmask, icedpool... */
		if (IS_ROOM(lev->typ)) /* && !IS_AIR(lev->typ) */
		    lev->typ = ROOM;

		/*
		 * some cases which can happen when digging
		 * down while phazing thru solid areas
		 */
		else if (lev->typ == STONE || lev->typ == SCORR)
		    lev->typ = CORR;
		else if (IS_WALL(lev->typ) || lev->typ == SDOOR)
		    lev->typ = level.flags.is_maze_lev ? ROOM :
			       level.flags.is_cavernous_lev ? CORR : DOOR;

		unearth_objs(x, y);
		break;
	}
	ttmp->hiddentrap = 0;
	if (!rn2(!(u.monstertimefinish % 13336) ? 3 : !(u.monstertimefinish % 1336) ? 10 : !(u.monstertimefinish % 136) ? 30 : 100)) ttmp->hiddentrap = 1;

	if (ttmp->ttyp == u.invisotrap) ttmp->hiddentrap = 1;

	if (ttmp->ttyp == HOLE && !In_sokoban(&u.uz) && !ttmp->hiddentrap ) ttmp->tseen = 1;  /* You can't hide a hole */
	else ttmp->tseen = 0;
	ttmp->once = 0;
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
	if(Blind && Levitation && !In_sokoban(&u.uz)) return;

	do {
	    newlevel++;
	} while(!rn2(4) && newlevel < dunlevs_in_dungeon(&u.uz));

	if(td) {
	    struct trap *t=t_at(u.ux,u.uy);
	    seetrap(t);
	    if (!In_sokoban(&u.uz)) {
		if (t->ttyp == TRAPDOOR)
			pline("A trap door opens up under you!");
		else if (t->ttyp == SHAFT_TRAP)
			pline("A shaft opens up under you!");
		else 
			pline("There's a gaping hole under you!");
	    }
	} else pline_The("%s opens up under you!", surface(u.ux,u.uy));

	if (In_sokoban(&u.uz) && Can_fall_thru(&u.uz))
	    ;	/* KMH -- You can't escape the Sokoban level traps */
	else if(Levitation || u.ustuck || !Can_fall_thru(&u.uz)
	   || Flying || is_clinger(youmonst.data)
	   || (Role_if(PM_ARCHEOLOGIST) && uwep && uwep->otyp == BULLWHIP)
	   || (Inhell && !u.uevent.invoked &&
					newlevel == dunlevs_in_dungeon(&u.uz))
		) {
		if (Role_if(PM_ARCHEOLOGIST) && uwep && uwep->otyp == BULLWHIP)            
		pline("But thanks to your trusty whip ...");
	    dont_fall = "don't fall in.";
	} else if (youmonst.data->msize >= MZ_HUGE) {
	    dont_fall = "don't fit through.";
	} else if (!next_to_u()) {
	    dont_fall = "are jerked back by your pet!";
	}
	if (dont_fall) {
	    You(dont_fall);
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
	if (!td)
	    Sprintf(msgbuf, "The hole in the %s above you closes up.",
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
	if(Blind && Levitation && !In_sokoban(&u.uz)) return;

	do {
	    newlevel++;
	} while(rn2(4) && newlevel < dunlevs_in_dungeon(&u.uz));

	if(td) {
	    struct trap *t=t_at(u.ux,u.uy);
	    seetrap(t);
	    if (!In_sokoban(&u.uz)) {
		if (t->ttyp == TRAPDOOR)
			pline("A trap door opens up under you!");
		else if (t->ttyp == SHAFT_TRAP)
			pline("A shaft opens up under you!");
		else 
			pline("There's a gaping hole under you!");
	    }
	} else pline_The("%s opens up under you!", surface(u.ux,u.uy));

	if (In_sokoban(&u.uz) && Can_fall_thru(&u.uz))
	    ;	/* KMH -- You can't escape the Sokoban level traps */
	else if(Levitation || u.ustuck || !Can_fall_thru(&u.uz)
	   || Flying || is_clinger(youmonst.data)
	   || (Role_if(PM_ARCHEOLOGIST) && uwep && uwep->otyp == BULLWHIP)
	   || (Inhell && !u.uevent.invoked &&
					newlevel == dunlevs_in_dungeon(&u.uz))
		) {
		if (Role_if(PM_ARCHEOLOGIST) && uwep && uwep->otyp == BULLWHIP)            
		pline("But thanks to your trusty whip ...");
	    dont_fall = "don't fall in.";
	} else if (youmonst.data->msize >= MZ_HUGE) {
	    dont_fall = "don't fit through.";
	} else if (!next_to_u()) {
	    dont_fall = "are jerked back by your pet!";
	}
	if (dont_fall) {
	    You(dont_fall);
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
	if (!td)
	    Sprintf(msgbuf, "The hole in the %s above you closes up.",
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

	Strcpy(statuename,the(xname(statue)));

	if (statue->oxlth && statue->oattached == OATTACHED_MONST) {
	    cc.x = x,  cc.y = y;
	    mon = montraits(statue, &cc);
	    if (mon && mon->mtame && !mon->isminion)
		wary_dog(mon, TRUE);
	} else {
	    /* statue of any golem hit with stone-to-flesh becomes flesh golem */
	    if (is_golem(&mons[statue->corpsenm]) && cause == ANIMATE_SPELL)
	    	mptr = &mons[PM_FLESH_GOLEM];
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
		Upolyd && hides_under(youmonst.data) && !OBJ_AT(x, y))
	    u.uundetected = 0;

	if (fail_reason) *fail_reason = AS_OK;
	return mon;
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

#ifdef STEED
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
#endif /*STEED*/

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

	int monstcnt; /* for animation trap */
	int ptmp;	/* for poison gas trap */
	int nastytrapdur;	/* for nasty traps */
	int blackngdur;	/* timeout for black ng wall trap after which you die */

	int randsp;
	int randmnst;
	struct permonst *randmonstforspawn;
	int monstercolor;
	int randmnsx;
	int i;

	int randomamount = 0;
	int randomx, randomy;

	struct obj *otmpi, *otmpii;

	nastytrapdur = (Role_if(PM_GRADUATE) ? 12 : Role_if(PM_GEEK) ? 25 : 50);
	if (!nastytrapdur) nastytrapdur = 50; /* fail safe */

	blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
	if (!blackngdur ) blackngdur = 500; /* fail safe */

	/* Traps are 50% more likely to fail for a pickpocket */
	if (!In_sokoban(&u.uz) && Role_if(PM_PICKPOCKET) && rn2(2)) return;

	nomul(0, 0);

	/* KMH -- You can't escape the Sokoban level traps */
	if (In_sokoban(&u.uz) &&
			(ttype == PIT || ttype == SPIKED_PIT || ttype == GIANT_CHASM || ttype == SHIT_PIT || ttype == SHAFT_TRAP || ttype == HOLE ||
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
	    if ((Levitation || Flying) &&
		    (ttype == PIT || ttype == SPIKED_PIT || ttype == GIANT_CHASM || ttype == HOLE ||
		    ttype == BEAR_TRAP)) {
		You("%s over %s %s.",
		    Levitation ? "float" : "fly",
		    a_your[trap->madeby_u],
		    defsyms[trap_to_defsym(ttype)].explanation);
		return;
	    }
	    if(!Fumbling && ttype != MAGIC_PORTAL && ttype != RMB_LOSS_TRAP && ttype != AUTOMATIC_SWITCHER && ttype != MENU_TRAP && ttype != SPEED_TRAP && ttype != DISPLAY_TRAP && ttype != SPELL_LOSS_TRAP && ttype != YELLOW_SPELL_TRAP && ttype != AUTO_DESTRUCT_TRAP && ttype != MEMORY_TRAP && ttype != INVENTORY_TRAP && ttype != SUPERSCROLLER_TRAP && ttype != NUPESELL_TRAP && ttype != ACTIVE_SUPERSCROLLER_TRAP && ttype != BLACK_NG_WALL_TRAP && ttype != FREE_HAND_TRAP && ttype != UNIDENTIFY_TRAP && ttype != THIRST_TRAP && ttype != LUCK_TRAP && ttype != SHADES_OF_GREY_TRAP && ttype != FAINT_TRAP && ttype != CURSE_TRAP && ttype != DIFFICULTY_TRAP && ttype != SOUND_TRAP && ttype != DROP_TRAP && ttype != CASTER_TRAP && ttype != WEAKNESS_TRAP && ttype != ROT_THIRTEEN_TRAP && ttype != ALIGNMENT_TRAP && ttype != BISHOP_TRAP && ttype != STAIRS_TRAP && ttype != DSTW_TRAP && ttype != STATUS_TRAP && ttype != UNINFORMATION_TRAP && ttype != CONFUSION_TRAP && ttype != INTRINSIC_LOSS_TRAP && ttype != BLOOD_LOSS_TRAP && ttype != BAD_EFFECT_TRAP && ttype != MULTIPLY_TRAP && ttype != AUTO_VULN_TRAP && ttype != TELE_ITEMS_TRAP && ttype != NASTINESS_TRAP && ttype != FARLOOK_TRAP && ttype != CAPTCHA_TRAP && ttype != RESPAWN_TRAP && ttype != RECURRING_AMNESIA_TRAP && ttype != BIGSCRIPT_TRAP && ttype != BANK_TRAP && ttype != ONLY_TRAP && ttype != MAP_TRAP && ttype != TECH_TRAP && ttype != DISENCHANT_TRAP && ttype != VERISIERT && ttype != CHAOS_TRAP && ttype != MUTENESS_TRAP && ttype != NTLL_TRAP && ttype != ENGRAVING_TRAP && ttype != MAGIC_DEVICE_TRAP && ttype != BOOK_TRAP && ttype != LEVEL_TRAP && ttype != QUIZ_TRAP && ttype != LOUDSPEAKER && ttype != LASER_TRAP &&
		ttype != ANTI_MAGIC && ttype != OUT_OF_MAGIC_TRAP && !forcebungle &&
		(!rn2(5) ||
	    ((ttype == PIT || ttype == SPIKED_PIT || ttype == GIANT_CHASM || ttype == SHIT_PIT) && is_clinger(youmonst.data)))) {
		You("escape %s %s.",
		    (ttype == ARROW_TRAP && !trap->madeby_u) ? "an" :
			a_your[trap->madeby_u],
		    defsyms[trap_to_defsym(ttype)].explanation);
		if (ttype == FART_TRAP) {
			pline("But you're longing for a sexy girl so much, so...");
		} else
			return;
	    }
	}

#ifdef STEED
	if (u.usteed) u.usteed->mtrapseen |= (1 << (ttype-1));
#endif

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
		otmp = mksobj(ARROW, TRUE, FALSE);
		otmp->quan = 1L;
		otmp->owt = weight(otmp);
		otmp->opoisoned = 0;
#ifdef STEED
		if (u.usteed && !rn2(2) && steedintrap(trap, otmp)) /* nothing */;
		else
#endif
		if (thitu(8 + rnd((monster_difficulty() / 2) + 1), dmgval(otmp, &youmonst) + rnd((monster_difficulty() / 2) + 1) , otmp, "arrow")) {
		    obfree(otmp, (struct obj *)0);
		} else {
		    place_object(otmp, u.ux, u.uy);
		    if (!Blind) otmp->dknown = 1;
		    stackobj(otmp);
		    newsym(u.ux, u.uy);
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
		otmp = mksobj(CROSSBOW_BOLT, TRUE, FALSE);
		otmp->quan = 1L;
		otmp->owt = weight(otmp);
		otmp->opoisoned = 0;
#ifdef STEED
		if (u.usteed && !rn2(2) && steedintrap(trap, otmp)) /* nothing */;
		else
#endif
		if (thitu(8 + rnd((monster_difficulty() * 2 / 3) + 1), dmgval(otmp, &youmonst) + rnd((monster_difficulty() * 2 / 3) + 1) , otmp, "bolt")) {
		    obfree(otmp, (struct obj *)0);
		} else {
		    place_object(otmp, u.ux, u.uy);
		    if (!Blind) otmp->dknown = 1;
		    stackobj(otmp);
		    newsym(u.ux, u.uy);
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
		otmp = mksobj(BULLET, TRUE, FALSE);
		otmp->quan = 1L;
		otmp->owt = weight(otmp);
		otmp->opoisoned = 0;
#ifdef STEED
		if (u.usteed && !rn2(2) && steedintrap(trap, otmp)) /* nothing */;
		else
#endif
		if (thitu(8 + rnd(monster_difficulty() + 1), dmgval(otmp, &youmonst) + rnd((monster_difficulty() * 2 / 3) + 1) , otmp, "bullet")) {
		    obfree(otmp, (struct obj *)0);
		} else {
		    obfree(otmp, (struct obj *)0);
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
		otmp = mksobj(DROVEN_ARROW, TRUE, FALSE);
		otmp->quan = 1L;
		otmp->owt = weight(otmp);
		otmp->opoisoned = 0;
#ifdef STEED
		if (u.usteed && !rn2(2) && steedintrap(trap, otmp)) /* nothing */;
		else
#endif
		if (thitu(8 + rnd((monster_difficulty() / 2) + 1), dmgval(otmp, &youmonst) + rnd((monster_difficulty() / 2) + 1) , otmp, "glass arrow")) {
		    obfree(otmp, (struct obj *)0);
		} else {
		    place_object(otmp, u.ux, u.uy);
		    if (!Blind) otmp->dknown = 1;
		    stackobj(otmp);
		    newsym(u.ux, u.uy);
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
		otmp = mksobj(DROVEN_BOLT, TRUE, FALSE);
		otmp->quan = 1L;
		otmp->owt = weight(otmp);
		otmp->opoisoned = 0;
#ifdef STEED
		if (u.usteed && !rn2(2) && steedintrap(trap, otmp)) /* nothing */;
		else
#endif
		if (thitu(8 + rnd((monster_difficulty() * 2 / 3) + 1), dmgval(otmp, &youmonst) + rnd((monster_difficulty() * 2 / 3) + 1) , otmp, "glass bolt")) {
		    obfree(otmp, (struct obj *)0);
		} else {
		    place_object(otmp, u.ux, u.uy);
		    if (!Blind) otmp->dknown = 1;
		    stackobj(otmp);
		    newsym(u.ux, u.uy);
		}
		break;

	    case FART_TRAP:

		if (!trap->tseen) pline("You discover a construction, behind which there is a sexy girl waiting... but you can only see her butt. It seems that her name is %s.", farttrapnames[trap->launch_otyp]);
		else pline("You can't resist the temptation to caress %s's sexy butt.", farttrapnames[trap->launch_otyp]);

		seetrap(trap);

		if (trap->launch_otyp < 11) pline("%s produces %s farting noises with her sexy butt.", farttrapnames[trap->launch_otyp], rn2(2) ? "tender" : "soft");
		else if (trap->launch_otyp < 29) pline("%s produces %s farting noises with her sexy butt.", farttrapnames[trap->launch_otyp], rn2(2) ? "beautiful" : "squeaky");
		else pline("%s produces %s farting noises with her sexy butt.", farttrapnames[trap->launch_otyp], rn2(2) ? "disgusting" : "loud");
		badeffect();

		break;

	    case MAGIC_BEAM_TRAP:
		You_hear("a soft click.");
		seetrap(trap);
		if (isok(trap->launch.x,trap->launch.y) && IS_STWALL(levl[trap->launch.x][trap->launch.y].typ)) {
		    buzz(trap->launch_otyp, 7 + rnd((monster_difficulty() / 10) + 1) ,
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
		otmp = mksobj(DART, TRUE, FALSE);
		otmp->quan = 1L;
		otmp->owt = weight(otmp);
		if (!rn2(6)) otmp->opoisoned = 1;
#ifdef STEED
		if (u.usteed && !rn2(2) && steedintrap(trap, otmp)) /* nothing */;
		else
#endif
		if (thitu(7 + rnd((monster_difficulty() / 3) + 1), dmgval(otmp, &youmonst) + rnd((monster_difficulty() / 3) + 1), otmp, "little dart")) {
		    if (otmp->opoisoned)
			poisoned("dart", A_CON, "little dart", -10);
		    obfree(otmp, (struct obj *)0);
		} else {
		    place_object(otmp, u.ux, u.uy);
		    if (!Blind) otmp->dknown = 1;
		    stackobj(otmp);
		    newsym(u.ux, u.uy);
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
		otmp = mksobj(SHURIKEN, TRUE, FALSE);
		otmp->quan = 1L;
		otmp->owt = weight(otmp);
		if (!rn2(6)) otmp->opoisoned = 1;
#ifdef STEED
		if (u.usteed && !rn2(2) && steedintrap(trap, otmp)) /* nothing */;
		else
#endif
		if (thitu(8 + rnd((monster_difficulty() / 2) + 1), dmgval(otmp, &youmonst) + rnd((monster_difficulty() / 2) + 1), otmp, "shuriken")) {
		    if (otmp->opoisoned)
			poisoned("shuriken", A_CON, "shuriken", -10);
		    obfree(otmp, (struct obj *)0);
		} else {
		    place_object(otmp, u.ux, u.uy);
		    if (!Blind) otmp->dknown = 1;
		    stackobj(otmp);
		    newsym(u.ux, u.uy);
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

		    trap->once = 1;
		    seetrap(trap);
		    otmp = mksobj_at(ROCK, u.ux, u.uy, TRUE, FALSE);
		    otmp->quan = 1L;
		    otmp->owt = weight(otmp);

		    pline("A trap door in %s opens and %s falls on your %s!",
			  the(ceiling(u.ux,u.uy)),
			  an(xname(otmp)),
			  body_part(HEAD));

		    if (uarmh) {
			if(is_metallic(uarmh)) {
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
		break;

	    case FALLING_BOULDER_TRAP:
		if (trap->once && !rn2(15)) {
		    pline("A trap door in %s opens, but nothing falls out!",
			  the(ceiling(u.ux,u.uy)));
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		} else {
		    int dmg = d(6,6) + rnd(monster_difficulty() + 1); /* should be std BOULDER dmg? */

		    trap->once = 1;
		    seetrap(trap);
		    otmp = mksobj_at(BOULDER, u.ux, u.uy, TRUE, FALSE);
		    otmp->quan = 1L;
		    otmp->owt = weight(otmp);

		    pline("A trap door in %s opens and %s falls on your %s!",
			  the(ceiling(u.ux,u.uy)),
			  an(xname(otmp)),
			  body_part(HEAD));

		    if (uarmh) {
			if(is_metallic(uarmh)) {
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
		break;

	    case SQKY_BOARD:	    /* stepped on a squeaky board */
		if (Levitation || Flying) {
		    if (!Blind) {
			seetrap(trap);
			if (Hallucination)
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
			    if (!DEADMONSTER(mtmp3)) {
				mtmp3->msleeping = 0;
			    }
			}
			pline("Your ears are blasted by hellish noise!");
			int dmg = rnd(16) + rnd( (monster_difficulty() * 2 / 3) + 1);
			if (Deafness) dmg /= 2;
			make_stunned(HStun + dmg, TRUE);
			if (!rn2(5)) (void)destroy_item(POTION_CLASS, AD_COLD);
	      	if (dmg) losehp(dmg, "noise trap", KILLED_BY_AN);

		    seetrap(trap);
		}
		break;

	    case BEAR_TRAP:
		if(Levitation || Flying) break;
		seetrap(trap);
		if(amorphous(youmonst.data) || is_whirly(youmonst.data) ||
						    unsolid(youmonst.data)) {
		    pline("%s bear trap closes harmlessly through you.",
			    A_Your[trap->madeby_u]);
		    break;
		}
		if(
#ifdef STEED
		   !u.usteed &&
#endif
		   youmonst.data->msize <= MZ_SMALL) {
		    pline("%s bear trap closes harmlessly over you.",
			    A_Your[trap->madeby_u]);
		    break;
		}
		u.utrap = rn1(4, 4);
		u.utraptype = TT_BEARTRAP;
#ifdef STEED
		if (u.usteed) {
		    pline("%s bear trap closes on %s %s!",
			A_Your[trap->madeby_u], s_suffix(mon_nam(u.usteed)),
			mbodypart(u.usteed, FOOT));
		} else
#endif
		{
		    pline("%s bear trap closes on your %s!",
			    A_Your[trap->madeby_u], body_part(FOOT));
		    if(u.umonnum == PM_OWLBEAR || u.umonnum == PM_BUGBEAR)
			You("howl in anger!");
		}
		exercise(A_DEX, FALSE);
		break;

	    case SLP_GAS_TRAP:
		seetrap(trap);
		if(Sleep_resistance || breathless(youmonst.data)) {
		    You("are enveloped in a cloud of gas!");
		    break;
		}
		pline("A cloud of gas puts you to sleep!");
		fall_asleep(-rnd(25 + rnd((monster_difficulty() / 4) + 1) ), TRUE);
#ifdef STEED
		(void) steedintrap(trap, (struct obj *)0);
#endif
		break;

	    case PARALYSIS_TRAP:
	      pline("You stepped on a trigger!");
		seetrap(trap);
		if (!Free_action) {
		    pline("You are frozen in place!");
		    nomul(-rnz(10), "frozen by a paralysis trap");
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
		} else You("stiffen momentarily.");
#ifdef STEED
		(void) steedintrap(trap, (struct obj *)0);
#endif
		break;

	    case POISON_GAS_TRAP:
		seetrap(trap);
		if(breathless(youmonst.data)) {
		    pline("A cloud of gas surrounds you!");
		    break;
		}
		pline("A cloud of gas surrounds you!");
		ptmp = rn2(6); /* A_STR ... A_CHA */
		    poisoned("gas", ptmp, "poison gas trap", 30);
#ifdef STEED
		(void) steedintrap(trap, (struct obj *)0);
#endif

		break;

	    case SLOW_GAS_TRAP:
		seetrap(trap);
		if(breathless(youmonst.data)) {
		    pline("A cloud of foggy gas shoots out at you!");
		    break;
		}
		pline("A cloud of foggy gas shoots out at you!");
		u_slow_down();

		if (!rn2(3)) make_frozen(HFrozen + ( (monster_difficulty() + 1) * 5), TRUE); /* randomly freeze the player --Amy */

#ifdef STEED
		(void) steedintrap(trap, (struct obj *)0);
#endif
		break;

	    case ACID_POOL:

		if (Levitation || Flying) break; /* this trap is ground-based */

		seetrap(trap);

	        int num = 0;
		struct obj *target;
	        num = d(4, 4) + rnd((monster_difficulty() / 2) + 1);

	        pline("You are sprayed with acid!");
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

		if (Levitation || Flying) break; /* this trap is ground-based */

		seetrap(trap);

			if (Levitation) {
				pline("You float over a deep pool of water.");
				break;
			}
			if (Flying) {
				pline("You fly over a deep pool of water.");
				break;
			}
			if (Wwalking) {
				pline("There is a deep pool of water right below your feet.");
				break;
			}

	        int numX = 0;
	        numX = d(3, 3) + rnd((monster_difficulty() / 3) + 1);

		  if (Swimming) {pline("You fell into a pool of water, but thankfully you can swim."); numX = 0;}
		  else if (Amphibious) {pline("You fell into a pool of water, but you aren't drowning."); numX = 0;}
	        else pline("You fell into a pool of water!");

			if (!rn2(20)) { /* A higher chance would be incredibly annoying. --Amy */
			water_damage(invent, FALSE, FALSE);
			if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
			}

	        if (numX) losehp(numX, "waterpool", KILLED_BY_AN);

		break;

	    case RUST_TRAP:
		seetrap(trap);
		if (u.umonnum == PM_IRON_GOLEM) {
		    int dam = u.mhmax;

		    pline("%s you!", A_gush_of_water_hits);
		    You("are covered with rust!");
		    if (Half_physical_damage && rn2(2) ) dam = (dam+1) / 2;
		    losehp(dam, "rusting away", KILLED_BY);
		    break;
		} else if (u.umonnum == PM_FLAMING_SPHERE) {
		    int dam = u.mhmax;

		    pline("%s you!", A_gush_of_water_hits);
		    You("are extinguished!");
		    if (Half_physical_damage && rn2(2) ) dam = (dam+1) / 2;
		    losehp(dam, "drenching", KILLED_BY);
		    break;
		} else if (u.umonnum == PM_GREMLIN && rn2(3)) {
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
glovecheck:		(void) rust_dmg(uarmg, "gauntlets", 1, TRUE, &youmonst);
			/* Not "metal gauntlets" since it gets called
			 * even if it's leather for the message
			 */
			break;
		    case 2:
			pline("%s your right %s!", A_gush_of_water_hits,
				    body_part(ARM));
			if (uwep && !snuff_lit(uwep))
			    erode_obj(uwep, FALSE, TRUE);
			goto glovecheck;
		    default:
			pline("%s you!", A_gush_of_water_hits);
			for (otmp=invent; otmp; otmp = otmp->nobj)
				    (void) snuff_lit(otmp);
			if (uarmc)
			    (void) rust_dmg(uarmc, cloak_simple_name(uarmc),
						1, TRUE, &youmonst);
			else if (uarm)
			    (void) rust_dmg(uarm, "armor", 1, TRUE, &youmonst);
#ifdef TOURIST
			else if (uarmu)
			    (void) rust_dmg(uarmu, "shirt", 1, TRUE, &youmonst);
#endif
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
		losehp(monster_difficulty(),"laser trap",KILLED_BY_AN);
		break;

	    case ESCALATING_TRAP:
		seetrap(trap);
		pline("You stepped on a trigger!");
		pline("Something hits your %s.", body_part(rn2(19) ) );
		losehp(1 + u.chokhmahdamage + rnd(u.ualign.sins + 1),"escalating trap",KILLED_BY_AN);
		u.chokhmahdamage++;
		break;

	    case ANIMATION_TRAP:

		monstcnt = 1;
		if (!rn2(5)) monstcnt += 1;
		if (!rn2(25)) monstcnt += 2;
		if (!rn2(125)) monstcnt += 3;
		if (!rn2(725)) monstcnt += rnd(20);

	      while(--monstcnt >= 0) {
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		}

	pline("es come to life!"); /* garbled string from Castle of the Winds. This trap summons random monsters. --Amy */

		deltrap(trap); /* only triggers once */
		break;

	    case HORDE_TRAP:

		monstcnt = 6 + rnd(12);
		if (!rn2(5)) monstcnt += rnd(5);
		if (!rn2(25)) monstcnt += rnd(10);
		if (!rn2(125)) monstcnt += rnd(20);
		if (!rn2(725)) monstcnt += rnd(50);

	      while(--monstcnt >= 0) {
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		}

		pline("e Pale Wraith with a lightning stroke.");

		deltrap(trap); /* only triggers once */
		break;

	    case IMMOBILITY_TRAP:

		monstcnt = 8;
		while (rn2(10)) monstcnt++;

		{
		int sessileattempts;
		int sessilemnum;

	      while(--monstcnt >= 0) {
			for (sessileattempts = 0; sessileattempts < 100; sessileattempts++) {
				sessilemnum = rndmonnum();
				if (sessilemnum != -1 && (mons[sessilemnum].mlet != S_TROVE) && is_nonmoving(&mons[sessilemnum]) ) sessileattempts = 100;
			}
			if (sessilemnum != -1) (void) makemon( &mons[sessilemnum], u.ux, u.uy, NO_MM_FLAGS);
		}

		}

		pline("vailable, exit anyway?");

		deltrap(trap); /* only triggers once */
		break;

	    case BOSS_TRAP:

		{
			int attempts = 0;
			register struct permonst *ptrZ;
newboss:
			do {

				ptrZ = rndmonst();
				attempts++;

			} while ( (!ptrZ || (ptrZ && !(ptrZ->geno & G_UNIQ))) && attempts < 50000);

			if (ptrZ && ptrZ->geno & G_UNIQ) {
				if (wizard) pline("monster generation: %s", ptrZ->mname);
				(void) makemon(ptrZ, u.ux, u.uy, NO_MM_FLAGS);
			}
			else if (ptrZ && rn2(10)) {
				attempts = 0;
				goto newboss;
			}

		}

		pline("A boss monster suddenly appears!");

		deltrap(trap); /* only triggers once */
		break;

	    case WISHING_TRAP:

		deltrap(trap); /* only triggers once, and before giving the wish to make sure you can't hangup cheat :P */
		pline("You stepped on a trap of wishing!");
		if (Luck+rn2(5) < 0) {
			pline("Unfortunately, nothing happens.");
			break;
		}
		makewish();
		break;

	    case GROWING_TRAP:

		pline("Uh-oh, should have watched your step...");
		{
			int madepoolQ = 0;

			do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_treefloodd, (genericptr_t)&madepoolQ);
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

			do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_icefloodd, (genericptr_t)&madepoolQ);
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

			do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_barfloodd, (genericptr_t)&madepoolQ);
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

			do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_lockfloodd, (genericptr_t)&madepoolQ);
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

			do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_cloudfloodd, (genericptr_t)&madepoolQ);
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

			do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_terrainfloodd, (genericptr_t)&madepoolQ);
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

	      while(--monstcnt >= 0) {
		(void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, NO_MM_FLAGS);
		}

		pline(Hallucination ? "And she's buying a stairway to heaven... er, hell." : "From the dark stairway to hell, demons appear to surround you!");

		deltrap(trap); /* only triggers once */
		break;

	    case LAVA_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("The thin crust of rock gives way, revealing lava underneath!");

		deltrap(trap); /* only triggers once */

		int madepool = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_lavafloode, (genericptr_t)&madepool);

		if (levl[u.ux][u.uy].typ == ROOM || levl[u.ux][u.uy].typ == CORR || levl[u.ux][u.uy].typ == ICE)
			levl[u.ux][u.uy].typ = LAVAPOOL; /* don't fall in, that would be too evil. --Amy */

		break;

	    case FLOOD_TRAP:

		pline("Uh-oh, should have watched your step...");
		pline("The thin crust of rock gives way, revealing water underneath!");

		deltrap(trap); /* only triggers once */

		int madepoolX = 0;

		do_clear_areaX(u.ux, u.uy, 5, do_floode, (genericptr_t)&madepoolX);

		if (levl[u.ux][u.uy].typ == ROOM || levl[u.ux][u.uy].typ == CORR || levl[u.ux][u.uy].typ == ICE) {
			levl[u.ux][u.uy].typ = POOL;
			if (!Wwalking && !Flying && !Levitation) drown();
		}
		break;

	    case QUICKSAND_TRAP:

		if (Levitation || Flying) {

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

		losehp(quicksanddamage,"being pulled into quicksand",KILLED_BY_AN);

		pline("Wheeeew! At last you managed to pull yourself out of the quicksand.");

		break;

	    case ITEM_TELEP_TRAP:
		seetrap(trap);
		pline("A vivid purple glow surrounds you...");

		if (invent) {
		    for (otmpi = invent; otmpi; otmpi = otmpii) {
		      otmpii = otmpi->nobj;

			if (!rn2(5) && !stack_too_big(otmpi) ) {
				dropx(otmpi);
			      if (otmpi->where == OBJ_FLOOR) rloco(otmpi);
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
			pseudo = mksobj(SPE_STONE_TO_FLESH, FALSE, FALSE);
			pseudo->blessed = pseudo->cursed = 0;
			pseudo->quan = 20L;			/* do not let useup get it */

		    if (u.umonnum == PM_STONE_GOLEM)
			(void) polymon(PM_FLESH_GOLEM);
		    if (Stoned) fix_petrification();	/* saved! */
		    /* but at a cost.. */
		    for (otemp = invent; otemp; otemp = onext) {
			onext = otemp->nobj;
			(void) bhito(otemp, pseudo);
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

	    case DRAIN_TRAP:

		seetrap(trap);
		pline("CLICK! You have triggered a trap!");

		if (!Drain_resistance || !rn2(20) ) {
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

			if (!rn2(20) && !Antimagic ) {
				killer_format = KILLED_BY_AN;
				killer = "instadeath trap";
				done(DIED);}
			else {

		      int dmgnum = 0;
		      dmgnum = d(2, 6) + rnd((monster_difficulty() * 2) + 1);
			if (Antimagic || (Half_spell_damage && rn2(2)) ) dmgnum /= 2;
			You("feel drained...");
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

		if (Sick_resistance) {
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
		pline("CLICK! You have triggered a trap!");

		{
			struct obj *otmpD;
			otmpD = some_armor(&youmonst);
			if(otmpD) {
			    if(!destroy_arm(otmpD)) {
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
				pline("You feel less protected!");
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
				pline("You feel a little apathetic...");

				switch(rn2(7)) {
				    case 0: /* destroy certain things */
					lethe_damage(invent, FALSE, FALSE);
					break;
				    case 1: /* sleep */
					if (multi >= 0) {
					    if (Sleep_resistance && rn2(20)) {pline("You yawn."); break;}
					    fall_asleep(-rnd(10), TRUE);
					    You("are put to sleep!");
					}
					break;
				    case 2: /* paralyse */
					if (multi >= 0) {
					    if (Free_action) {
						You("momentarily stiffen.");            
					    } else {
						You("are frozen!");
						nomovemsg = 0;	/* default: "you can move again" */
						nomul(-rnd(10), "paralyzed by a sin trap");
						exercise(A_DEX, FALSE);
					    }
					}
					break;
				    case 3: /* slow */
					if(HFast)  u_slow_down();
					else You("pause momentarily.");
					break;
				    case 4: /* drain Dex */
					adjattrib(A_DEX, -rn1(1,1), 0);
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
				    for (otmpi = invent; otmpi; otmpi = otmpii) {
				      otmpii = otmpi->nobj;
		
					if (!rn2(5) && !stack_too_big(otmpi) ) {
						dropx(otmpi);
					      if (otmpi->where == OBJ_FLOOR) rloco(otmpi);
					}
				    }
				}
				break;
			case 6: /* envy */
				pline("You hear a chuckling laughter.");
			      attrcurse();
			      attrcurse();
				break;
			case 7: /* pride */
			      pline("The RNG determines to take you down a peg or two...");
				if (!rn2(3)) {
				    poisoned("air", rn2(A_MAX), "sin trap", 30);
				}
				if (!rn2(4)) {
					pline("You feel drained...");
					u.uhpmax -= rn1(10,10);
					if (u.uhpmax < 0) u.uhpmax = 0;
					if(u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				}
				if (!rn2(4)) {
					You_feel("less energised!");
					u.uenmax -= rn1(10,10);
					if (u.uenmax < 0) u.uenmax = 0;
					if(u.uen > u.uenmax) u.uen = u.uenmax;
				}
				if (!rn2(4)) {
					if(!Drain_resistance || !rn2(20) )
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
				newman();
				break;
			    case 3: case 4:
				if(!rn2(4) && u.ulycn == NON_PM &&
					!Protection_from_shape_changers &&
					!is_were(youmonst.data) &&
					!defends(AD_WERE,uwep)) {
				    You_feel("feverish.");
				    exercise(A_CON, FALSE);
				    u.ulycn = PM_WERECOW;
				} else {
					if (multi >= 0) {
					    if (Sleep_resistance && rn2(20)) break;
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
				water_damage(invent, FALSE, FALSE);
				break;
			    case 13:
				if (multi >= 0) {
				    if (Free_action) {
					You("momentarily stiffen.");            
				    } else {
					You("are frozen!");
					nomovemsg = 0;	/* default: "you can move again" */
					nomul(-rnd(10), "paralyzed by a sin trap");
					exercise(A_DEX, FALSE);
				    }
				}
				break;
			    case 14:
				if (Hallucination)
					pline("What a groovy feeling!");
				else
					You(Blind ? "%s and get dizzy..." :
						 "%s and your vision blurs...",
						    stagger(youmonst.data, "stagger"));
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
	
				    if (drain_item(objD)) {
					Your("%s less effective.", aobjnam(objD, "seem"));
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

		if (!Disint_resistance || !rn2(100) ) {
			pline("You feel like you're falling apart!");

			if (uarms) {
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
			} else done(DIED);

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

			switch (dmgtype) {

			case 1: /* ordinary physical damage */
			if (dmg) {losehp(dmg, "hidden glyph", KILLED_BY_AN); }
				break;
			case 2: /* shock */
			if(Shock_resistance) dmg = 0;
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
			if (dmg) {pline("You are hit by lightning!"); losehp(dmg, "lightning", KILLED_BY); }
			break;
			case 3: /* ice */
			if(Cold_resistance) dmg = 0;
			if (!rn2(10)) make_frozen(HFrozen + (dmg * 5), TRUE); /* randomly freeze the player --Amy */
		    if (!rn2(33)) /* new calculations --Amy */  destroy_item(POTION_CLASS, AD_COLD);
			if (dmg) {pline("You are hit by cold!"); losehp(dmg, "cold", KILLED_BY); }
			break;
			case 4: /* fire */
			if(Fire_resistance) dmg = 0;
			if ( /* burnarmor(&youmonst) || */ !rn2(33)) /* new calculation -- Amy */ {
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

			switch (dmgtype) {

			case 1: /* ordinary physical damage */
			if (dmg) {losehp(dmg, "hidden glyph", KILLED_BY_AN); }
				break;
			case 2: /* shock */
			if(Shock_resistance) dmg = 0;
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
			if (dmg) {pline("You are hit by lightning!"); losehp(dmg, "lightning", KILLED_BY); }
			break;
			case 3: /* ice */
			if(Cold_resistance) dmg = 0;
			if (!rn2(10)) make_frozen(HFrozen + (dmg * 5), TRUE); /* randomly freeze the player --Amy */
		    if (!rn2(33)) /* new calculations --Amy */  destroy_item(POTION_CLASS, AD_COLD);
			if (dmg) {pline("You are hit by cold!"); losehp(dmg, "cold", KILLED_BY); }
			break;
			case 4: /* fire */
			if(Fire_resistance) dmg = 0;
			if ( /* burnarmor(&youmonst) || */ !rn2(33)) /* new calculation -- Amy */ {
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

			switch (dmgtype) {

			case 1: /* ordinary physical damage */
			if (dmg) {losehp(dmg, "hidden glyph", KILLED_BY_AN); }
				break;
			case 2: /* shock */
			if(Shock_resistance) dmg = 0;
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
			if (dmg) {pline("You are hit by lightning!"); losehp(dmg, "lightning", KILLED_BY); }
			break;
			case 3: /* ice */
			if(Cold_resistance) dmg = 0;
			if (!rn2(10)) make_frozen(HFrozen + (dmg * 5), TRUE); /* randomly freeze the player --Amy */
		    if (!rn2(33)) /* new calculations --Amy */  destroy_item(POTION_CLASS, AD_COLD);
			if (dmg) {pline("You are hit by cold!"); losehp(dmg, "cold", KILLED_BY); }
			break;
			case 4: /* fire */
			if(Fire_resistance) dmg = 0;
			if ( /* burnarmor(&youmonst) || */ !rn2(33)) /* new calculation -- Amy */ {
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

	      while(--monstcnt >= 0) {
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		}

		pline("e Pale Wraith with a lightning stroke.");

		if (!rn2(10)) deltrap(trap);
		break;

	    case GREEN_GLYPH:
			{
			pline("You trigger a hidden glyph!");
		    seetrap(trap);

		    int dmg = (d(4, 4) + rnd(monster_difficulty() + 1));
			int dmgtype = rnd(4);

			switch (dmgtype) {

			case 1: /* ordinary physical damage */
			if (dmg) {losehp(dmg, "hidden glyph", KILLED_BY_AN); }
				break;
			case 2: /* shock */
			if(Shock_resistance) dmg = 0;
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
			if (dmg) {pline("You are hit by lightning!"); losehp(dmg, "lightning", KILLED_BY); }
			break;
			case 3: /* ice */
			if(Cold_resistance) dmg = 0;
			if (!rn2(10)) make_frozen(HFrozen + (dmg * 5), TRUE); /* randomly freeze the player --Amy */
		    if (!rn2(33)) /* new calculations --Amy */  destroy_item(POTION_CLASS, AD_COLD);
			if (dmg) {pline("You are hit by cold!"); losehp(dmg, "cold", KILLED_BY); }
			break;
			case 4: /* fire */
			if(Fire_resistance) dmg = 0;
			if ( /* burnarmor(&youmonst) || */ !rn2(33)) /* new calculation -- Amy */ {
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

			switch (dmgtype) {

			case 1: /* ordinary physical damage */
			if (dmg) {losehp(dmg, "hidden glyph", KILLED_BY_AN); }
				break;
			case 2: /* shock */
			if(Shock_resistance) dmg = 0;
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
			if (dmg) {pline("You are hit by lightning!"); losehp(dmg, "lightning", KILLED_BY); }
			break;
			case 3: /* ice */
			if(Cold_resistance) dmg = 0;
			if (!rn2(10)) make_frozen(HFrozen + (dmg * 5), TRUE); /* randomly freeze the player --Amy */
		    if (!rn2(33)) /* new calculations --Amy */  destroy_item(POTION_CLASS, AD_COLD);
			if (dmg) {pline("You are hit by cold!"); losehp(dmg, "cold", KILLED_BY); }
			break;
			case 4: /* fire */
			if(Fire_resistance) dmg = 0;
			if ( /* burnarmor(&youmonst) || */ !rn2(33)) /* new calculation -- Amy */ {
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

	      while(--monstcnt >= 0) {
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		}

		pline("es come to life!");

		if (!rn2(10)) deltrap(trap);
		break;

	    case BLUE_GLYPH:
			{
			pline("You trigger a hidden glyph!");
		    seetrap(trap);

		    int dmg = (d(4, 4) + rnd((monster_difficulty() / 4) + 1));
			int dmgtype = rnd(4);

			switch (dmgtype) {

			case 1: /* ordinary physical damage */
			if (dmg) {losehp(dmg, "hidden glyph", KILLED_BY_AN); }
				break;
			case 2: /* shock */
			if(Shock_resistance) dmg = 0;
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
			if (dmg) {pline("You are hit by lightning!"); losehp(dmg, "lightning", KILLED_BY); }
			break;
			case 3: /* ice */
			if(Cold_resistance) dmg = 0;
			if (!rn2(10)) make_frozen(HFrozen + (dmg * 5), TRUE); /* randomly freeze the player --Amy */
		    if (!rn2(33)) /* new calculations --Amy */  destroy_item(POTION_CLASS, AD_COLD);
			if (dmg) {pline("You are hit by cold!"); losehp(dmg, "cold", KILLED_BY); }
			break;
			case 4: /* fire */
			if(Fire_resistance) dmg = 0;
			if ( /* burnarmor(&youmonst) || */ !rn2(33)) /* new calculation -- Amy */ {
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

		if (Levitation || Flying) { /* ground-based trap, obviously */
		    if (!already_seen && rn2(3)) break;
		    seetrap(trap);
		    pline("%s %s on the ground below you.",
			    already_seen ? "There is" : "You discover",
			    "a stinking heap of shit");
			break;
		} 

		seetrap(trap);
		doshittrap((struct obj *)0);
		if (!rn2(50)) deltrap(trap);
		break;

	    case PIT:
	    case SPIKED_PIT:
	    case GIANT_CHASM:
	    case SHIT_PIT:
		/* KMH -- You can't escape the Sokoban level traps */
		if (!In_sokoban(&u.uz) && (Levitation || Flying)) break;
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
#ifdef STEED
		    if (u.usteed) {
		    	if ((trflags & RECURSIVETRAP) != 0)
			    Sprintf(verbbuf, "and %s fall",
				x_monnam(u.usteed,
				    u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				    (char *)0, SUPPRESS_SADDLE, FALSE));
			else
			    Sprintf(verbbuf,"lead %s",
				x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 	 "poor", SUPPRESS_SADDLE, FALSE));
		    } else
#endif
		    Strcpy(verbbuf,"fall");
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
		if (ttype == SPIKED_PIT) {
		    const char *predicament = "on a set of sharp iron spikes";
#ifdef STEED
		    if (u.usteed) {
			pline("%s lands %s!",
				upstart(x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
					 "poor", SUPPRESS_SADDLE, FALSE)),
			      predicament);
		    } else
#endif
		    You("land %s!", predicament);
		}
		if (!Passes_walls)
		    u.utrap = rn1(6,2);
		    if (ttype == SHIT_PIT) u.utrap += rnd(7);
		    if (ttype == GIANT_CHASM) u.utrap += rnd(75);
		u.utraptype = TT_PIT;
#ifdef STEED
		if (!steedintrap(trap, (struct obj *)0)) {
#endif
		if (ttype == SPIKED_PIT) {
		    losehp(rnd(10) + rnd((monster_difficulty() / 2) + 1) ,"fell into a pit of iron spikes",
			NO_KILLER_PREFIX);
		    if (!rn2(6))
			poisoned("spikes", A_STR, "fall onto poison spikes", 8);
		} else if (ttype == GIANT_CHASM) {
		    losehp(rnd(25) + rnd(monster_difficulty() + 1) ,"fell into a giant chasm", NO_KILLER_PREFIX);
		} else {
		    losehp(rnd(6) + rnd((monster_difficulty() / 3) + 1) ,"fell into a pit", NO_KILLER_PREFIX);
		}

		if (ttype == SHIT_PIT) {
			doshittrap((struct obj *)0);
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
#ifdef STEED
		}
#endif
		break;
	    case HOLE:
	    case TRAPDOOR:
	    case SHAFT_TRAP:
		if (!Can_fall_thru(&u.uz)) {
		    seetrap(trap);	/* normally done in fall_through */
		    impossible("dotrap: %ss cannot exist on this level.",
			       defsyms[trap_to_defsym(ttype)].explanation);
		    break;		/* don't activate it after all */
		}
		if (ttype != SHAFT_TRAP) fall_through(TRUE);
		else fall_throughX(TRUE);
		break;

	    case TELEP_TRAP:
		seetrap(trap);
		tele_trap(trap);
		break;

	    case RELOCATION_TRAP:
		seetrap(trap);
		pline("You feel yourself yanked in a direction you didn't know existed!");
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	      (void) safe_teleds(FALSE);
		deltrap(trap);
		break;

	    case WARP_ZONE:
		deltrap(trap);

		if (u.uevent.udemigod || u.uhave.amulet || (u.usteed && mon_has_amulet(u.usteed))) { pline("You shudder for a moment."); (void) safe_teleds(FALSE); break;}

		if (flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) { 
			pline("For some reason you resist the banishment!"); break;}

		make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

		if (rn2(2)) {(void) safe_teleds(FALSE); goto_level(&medusa_level, TRUE, FALSE, FALSE); }
		else {(void) safe_teleds(FALSE); goto_level(&portal_level, TRUE, FALSE, FALSE); }

		register int newlev = rnd(71);
		d_level newlevel;
		get_level(&newlevel, newlev);
		goto_level(&newlevel, TRUE, FALSE, FALSE);
		pline("Welcome to warp zone!");

		break;

	    case LEVEL_TELEP:
		seetrap(trap);
		level_tele_trap(trap);
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
			if(Shock_resistance)  {
			    shieldeff(u.ux, u.uy);
			    You("don't seem to be affected.");
			    break;
			} else
			    losehp(d(4, 4) + rnd((monster_difficulty() / 2) + 1), "electric shock", KILLED_BY_AN);
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
			break;
		      }
		case 5:
		case 4:
		case 3:
			if (!Free_action) {
			pline("Suddenly you are frozen in place!");
			nomul(-d(5, 6), "frozen by a lock trap");
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
			}
			make_stunned(HStun + rn1(7, 16) + rnd((monster_difficulty() / 2) + 1),FALSE);
			(void) make_hallucinated(HHallucination + rn1(5, 16) + rnd((monster_difficulty() / 2) + 1),FALSE,0L);
			break;
		default: impossible("bad lock trap");
			break;
		}
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
		switch (rnd(10)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				pline("You feel life has clocked back.");
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
		if (dmg) losehp(dmg, "a time trap", KILLED_BY);
		}
		break;

	    case NEGATIVE_TRAP:
		pline("You stepped on a trigger!");
		seetrap(trap);
		switch (rnd(10)) {
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
				(void) adjattrib(rn2(A_MAX), -1, FALSE);
				break;
			case 10:
				make_sick(rn1(25,25), "negative food poisoning", TRUE, SICK_VOMITABLE);
				break;
		}
		break;

	    case PETRIFICATION_TRAP:
		pline("CLICK! You have triggered a trap!");
		seetrap(trap);
		    if (!Stone_resistance &&
			!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
			if (!Stoned) { Stoned = 7;
				pline("You start turning to stone!");
			}
			Sprintf(killer_buf, "petrification trap");
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
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slime trap";
		}
		break;

	    case LYCANTHROPY_TRAP:
		pline("CLICK! You have triggered a trap!");
		seetrap(trap);
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREWOLF;
			pline("You feel feverish.");
		}
		break;

	    case EARTHQUAKE_TRAP:
		pline("CLICK! You have triggered a trap!");
		deltrap(trap);
		pline_The("entire dungeon is shaking around you!");
		do_earthquake((u.ulevel - 1) / 3 + 1);
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

			mtmp2->mtame = mtmp2->mpeaceful = 0;

		}

		pline("It seems a little more dangerous here now...");
		doredraw();

		}

		break;

	    case MIND_WIPE_TRAP:
		pline("CLICK! You have triggered a trap!");
		seetrap(trap);

		forget(3);
		{
		if (!strncmpi(plname, "Maud", 4))
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
		pline("Eek - you stepped into a heap of sticky glue!");
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
		if (webmaker(youmonst.data) || Race_if(PM_SPIDERMAN) ) {
		    if (webmsgok)
		    	pline(trap->madeby_u ? "You take a walk on your web."
					 : "There is a spider web here.");
		    break;
		}
		if (webmsgok) {
		   char verbbuf[BUFSZ];
		   verbbuf[0] = '\0';
#ifdef STEED
		   if (u.usteed)
		   	Sprintf(verbbuf,"lead %s",
				x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 	 "poor", SUPPRESS_SADDLE, FALSE));
		   else
#endif
			
		    Sprintf(verbbuf, "%s", Levitation ? (const char *)"float" :
		      		locomotion(youmonst.data, "stumble"));
		    You("%s into %s spider web!",
			verbbuf, a_your[trap->madeby_u]);
		}
		u.utraptype = TT_WEB;

		/* Time stuck in the web depends on your/steed strength. */
		{
		    register int str = ACURR(A_STR);

#ifdef STEED
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
#endif
		    if (str <= 3) u.utrap = rn1(6,6);
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
#ifdef STEED
		(void) steedintrap(trap, (struct obj *)0);
#endif
		break;

	    case ANTI_MAGIC:
		seetrap(trap);
		if(Antimagic) {
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
		if(Antimagic && rn2(3) ) {
		    shieldeff(u.ux, u.uy);
		    You_feel("momentarily lethargic.");
		} else if (u.uen > ((u.uenmax / 9) + 1)) {
			u.uen = 0;
			pline("You feel drained of energy!");
		}
		else {
			u.uen = 0;
			pline("You feel drained of energy!");
			drain_en(rnd(u.ulevel) + 1 + rnd(monster_difficulty() + 1));
		}
		break;

	    case POLY_TRAP: {
	        char verbbuf[BUFSZ];
		seetrap(trap);
#ifdef STEED
		if (u.usteed)
			Sprintf(verbbuf, "lead %s",
				x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 	 (char *)0, SUPPRESS_SADDLE, FALSE));
		else
#endif
		 Sprintf(verbbuf,"%s",
		    Levitation ? (const char *)"float" :
		    locomotion(youmonst.data, "step"));
		You("%s onto a polymorph trap!", verbbuf);
		if(Antimagic || Unchanging) {
		    shieldeff(u.ux, u.uy);
		    You_feel("momentarily different.");
		    /* Trap did nothing; don't remove it --KAA */
		} else {
#ifdef STEED
		    (void) steedintrap(trap, (struct obj *)0);
#endif
		    deltrap(trap);	/* delete trap before polymorph */
		    newsym(u.ux,u.uy);	/* get rid of trap symbol */
		    You_feel("a change coming over you.");
		    polyself(FALSE);
		}
		break;
	    }

	    case GENETIC_TRAP: {
	        char verbbuf[BUFSZ];
		seetrap(trap);
		 Sprintf(verbbuf,"%s",
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
			u.wormpolymorph = rn2(NUMMONS);
		    polyself(FALSE);
		}

		break;
		}

	    case MISSINGNO_TRAP: {
	        char verbbuf[BUFSZ];
		seetrap(trap);
		 Sprintf(verbbuf,"%s",
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
			u.wormpolymorph = (NUMMONS + rnz(rnd(5000)));
			u.ughmemory = 1;
		    polyself(FALSE);
		}

		break;
		}

	    case LANDMINE: {
#ifdef STEED
		unsigned steed_mid = 0;
		struct obj *saddle = 0;
#endif
		if (Levitation || Flying) {
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
#ifdef STEED
		    /* prevent landmine from killing steed, throwing you to
		     * the ground, and you being affected again by the same
		     * mine because it hasn't been deleted yet
		     */
		    static boolean recursive_mine = FALSE;

		    if (recursive_mine) break;
#endif
		    seetrap(trap);
		    pline("KAABLAMM!!!  You triggered %s land mine!",
					    a_your[trap->madeby_u]);
#ifdef STEED
		    if (u.usteed) steed_mid = u.usteed->m_id;
		    recursive_mine = TRUE;
		    (void) steedintrap(trap, (struct obj *)0);
		    recursive_mine = FALSE;
		    saddle = sobj_at(SADDLE,u.ux, u.uy);
#endif
		    set_wounded_legs(LEFT_SIDE, HWounded_legs + rn1(35, 41));
		    set_wounded_legs(RIGHT_SIDE, HWounded_legs + rn1(35, 41));
		    exercise(A_DEX, FALSE);
		}
		blow_up_landmine(trap);
#ifdef STEED
		if (steed_mid && saddle && !u.usteed)
			(void)keep_saddle_with_steedcorpse(steed_mid, fobj, saddle);
#endif
		newsym(u.ux,u.uy);		/* update trap symbol */
		losehp(rnd(16) + rnd(monster_difficulty() + 1), "land mine", KILLED_BY_AN);
		/* fall recursively into the pit... */
		if ((trap = t_at(u.ux, u.uy)) != 0) dotrap(trap, RECURSIVETRAP);
		fill_pit(u.ux, u.uy);
		break;
	    }

	    case BOMB_TRAP: {

		    int i, j, bd = (1 + (monster_difficulty() / 10) );
		    pline("KAABLAMM!!!  You triggered a bomb!");
		newsym(u.ux,u.uy);		/* update trap symbol */
		losehp(rnd(36) + rnd( (monster_difficulty() * 3) + 1), "hidden bomb", KILLED_BY_AN);
		deltrap(trap);
		/* fall recursively into the pit... */

		    for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if (levl[u.ux + i][u.uy + j].typ != ROOM && levl[u.ux + i][u.uy + j].typ != CORR) continue;					if (t_at(u.ux + i, u.uy + j)) continue;
			maketrap(u.ux + i, u.uy + j, PIT);
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
		seetrap(trap);
#if defined(BLACKMARKET) && defined(STEED)
		if (u.usteed &&
			(Is_blackmarket(&trap->dst) || Is_blackmarket(&u.uz)))
		    pline("%s seems to shimmer for a moment.",
			  Monnam(u.usteed));
		else
#endif
		domagicportal(trap);
		break;

		 case SPEAR_TRAP:
		seetrap(trap);
		pline("A spear stabs up from a hole in the ground at you!");
		if (thick_skinned(youmonst.data)) {
			pline("But it breaks off against your body.");
			deltrap(trap);
		} else if (Levitation) {
			pline("The spear isn't long enough to reach you.");
		} else if (unsolid(youmonst.data)) {
			pline("It passes right through you!");
		} else {
			pline("Ouch! That hurts!");
			losehp(rnd(10)+ rnd(monster_difficulty() + 1),"sharpened bamboo stick",KILLED_BY_AN);
		}
		break;

		 case SCYTHING_BLADE:
		seetrap(trap);

		if (unsolid(youmonst.data)) {
			pline("A blade swings through your body.");
		} else if (!rn2(4)) {
			pline("You are barely missed by a scything blade!");
		} else {
			pline("You are hit by a scything blade!");
			losehp(rnd(9)+ rnd( (monster_difficulty() / 2) + 1 + (has_head(youmonst.data) && !Role_if(PM_COURIER)) ? 10 : 0),"scything blade",KILLED_BY_AN);
		}
		break;

		 case GUILLOTINE_TRAP:
		seetrap(trap);

		if (!rn2(20) && has_head(youmonst.data) && !Role_if(PM_COURIER) ) {
			pline("You are decapitated by a guillotine!");
			losehp(2 * (Upolyd ? u.mh : u.uhp) + 200, "guillotine trap",KILLED_BY_AN);
		} else {
			pline("You are hit by a guillotine!");
			losehp(rnd(20)+ rnd( (monster_difficulty() * 2) + 1 + (has_head(youmonst.data) && !Role_if(PM_COURIER)) ? 40 : 0),"guillotine trap",KILLED_BY_AN);
		}
		break;

		 case BISECTION_TRAP:
		seetrap(trap);

		if (!rn2(20) && !bigmonst(youmonst.data) && !Invulnerable && !(Stoned_chiller && Stoned) ) {
			pline("You are bisected by a razor-sharp blade!");
			losehp(2 * (Upolyd ? u.mh : u.uhp) + 200, "bisection trap",KILLED_BY);
		} else {
			pline("You are hit by a razor-sharp blade!");
			losehp(rnd(30)+ rnd( (monster_difficulty() * 3) + 1),"bisection trap",KILLED_BY_AN);
		}
		break;

		 case COLLAPSE_TRAP:
			pline("Rocks fall all around you!");
			deltrap(trap);
			newsym(u.ux,u.uy);
			rocks_fall(u.ux,u.uy);
		 break;

		 case UNKNOWN_TRAP: /* randomly transforms into some other trap */

			deltrap(trap);

			int rtrap;
		      rtrap = randomtrap();

			(void) maketrap(u.ux, u.uy, rtrap);

			if ((trap = t_at(u.ux, u.uy)) != 0) dotrap(trap, RECURSIVETRAP);

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
			(void) maketrap(u.ux, u.uy, ROCKTRAP); /* no recursive trap here */
			}

		 break;

		 case LOUDSPEAKER: /* fake message */
			pline(fauxmessage());

		 break;

		 case RMB_LOSS_TRAP:

			if (RMBLoss) break;

			RMBLoss = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DROP_TRAP:

			if (NoDropProblem) break;

			NoDropProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DSTW_TRAP:

			if (DSTWProblem) break;

			DSTWProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case STATUS_TRAP:

			if (StatusTrapProblem) break;

			StatusTrapProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

			if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
			if (HStun) set_itimeout(&HeavyStunned, HStun);
			if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
			if (HFeared) set_itimeout(&HeavyFeared, HFeared);
			if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
			if (HBurned) set_itimeout(&HeavyBurned, HBurned);
			if (Blinded) set_itimeout(&HeavyBlind, Blinded);
			if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);

		 break;

		 case NUPESELL_TRAP: /* supposed to be impossible */

		 break;

		 case ACTIVE_SUPERSCROLLER_TRAP:

			if (Superscroller) break;

			else {
			deltrap(trap);
			(void) maketrap(u.ux, u.uy, SUPERSCROLLER_TRAP);
			/* no recursive trap - just give the nasty effect here */
			Superscroller = rnz(nastytrapdur * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) * (monster_difficulty() + 1));
			(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
			}

		 break;

		 case SUPERSCROLLER_TRAP:

			if (Superscroller) break;

			Superscroller = rnz(nastytrapdur * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) * (monster_difficulty() + 1));
			(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);

		 break;

		 case MENU_TRAP:

			if (MenuBug) break;

			MenuBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case FREE_HAND_TRAP:

			if (FreeHandLoss) break;

			FreeHandLoss = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case UNIDENTIFY_TRAP:

			if (Unidentify) break;

			Unidentify = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case RECURRING_AMNESIA_TRAP:

			if (RecurringAmnesia) break;

			RecurringAmnesia = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BIGSCRIPT_TRAP:

			if (BigscriptEffect) break;

			BigscriptEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BANK_TRAP:

			if (BankTrapEffect) break;

			BankTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

			if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));
			u.bankcashamount += u.ugold;
			u.ugold = 0;

		 break;

		 case MAP_TRAP:

			if (MapTrapEffect) break;

			MapTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case TECH_TRAP:

			if (TechTrapEffect) break;

			TechTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DISENCHANT_TRAP:

			if (RecurringDisenchant) break;

			RecurringDisenchant = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case VERISIERT:

			if (verisiertEffect) break;

			verisiertEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CHAOS_TRAP:

			if (ChaosTerrain) break;

			ChaosTerrain = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MUTENESS_TRAP:

			if (Muteness) break;

			Muteness = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case ENGRAVING_TRAP:

			if (EngravingDoesntWork) break;

			EngravingDoesntWork = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MAGIC_DEVICE_TRAP:

			if (MagicDeviceEffect) break;

			MagicDeviceEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BOOK_TRAP:

			if (BookTrapEffect) break;

			BookTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case LEVEL_TRAP:

			if (LevelTrapEffect) break;

			LevelTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case QUIZ_TRAP:

			if (QuizTrapEffect) break;

			QuizTrapEffect = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case NTLL_TRAP:

		 {
			char qbuf[QBUFSZ];
			char c;
			Strcpy(qbuf,"nt|| - Not a valid save file");
			if ((c = yn_function(qbuf, ynqchars, 'y')) != 'n') {
			    nomul(-rnd(127), "frozen by a ntll trap");
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

			Thirst = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case LUCK_TRAP:

			if (LuckLoss) break;

			LuckLoss = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SHADES_OF_GREY_TRAP:

			if (ShadesOfGrey) break;

			ShadesOfGrey = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case FAINT_TRAP:

			if (FaintActive) break;

			FaintActive = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CURSE_TRAP:

			if (Itemcursing) break;

			Itemcursing = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case DIFFICULTY_TRAP:

			if (DifficultyIncreased) break;

			DifficultyIncreased = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SOUND_TRAP:

			if (Deafness) break;

			Deafness = rnz(nastytrapdur * (monster_difficulty() + 1));
			flags.soundok = 0;

		 break;

		 case CASTER_TRAP:

			if (CasterProblem) break;

			CasterProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case WEAKNESS_TRAP:

			if (WeaknessProblem) break;

			WeaknessProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case ROT_THIRTEEN_TRAP:

			if (RotThirteen) break;

			RotThirteen = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BISHOP_TRAP:

			if (BishopGridbug) break;

			BishopGridbug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case UNINFORMATION_TRAP:

			if (UninformationProblem) break;

			UninformationProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case FARLOOK_TRAP:

			if (FarlookProblem) break;

			FarlookProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case RESPAWN_TRAP:

			if (RespawnProblem) break;

			RespawnProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CAPTCHA_TRAP:

			if (CaptchaProblem) break;

			CaptchaProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		case RECURSION_TRAP:
			pline("CLICK! You have triggered a trap!");
			deltrap(trap);
			recursioneffect();
		break;

		 case INTRINSIC_LOSS_TRAP:

			if (!rn2(5)) {
				You_hear("maniacal laughter!");
				attrcurse();
			}

			if (IntrinsicLossProblem) break;

			IntrinsicLossProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BLOOD_LOSS_TRAP:

			if (BloodLossProblem) break;

			BloodLossProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BAD_EFFECT_TRAP:

			if (!rn2(5)) badeffect();

			if (BadEffectProblem) break;

			BadEffectProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MULTIPLY_TRAP:


			{
				int rtrap;
			    int i, j, bd = 1;
	
			      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
					if (!isok(u.ux + i, u.uy + j)) continue;
					if ((levl[u.ux + i][u.uy + j].typ != ROOM && levl[u.ux + i][u.uy + j].typ != CORR) || MON_AT(u.ux + i, u.uy + j)) continue;
					if (t_at(u.ux + i, u.uy + j)) continue;
	
				      rtrap = randomtrap();
	
					(void) maketrap(u.ux + i, u.uy + j, rtrap);
				}
			}

			makerandomtrap();

			if (TrapCreationProblem) break;

			TrapCreationProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case AUTO_VULN_TRAP:

			if (!rn2(5)) {

				 switch (rnd(111)) {

					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
						u.uprops[DEAC_FIRE_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having fire resistance!");
						break;
					case 6:
					case 7:
					case 8:
					case 9:
					case 10:
						u.uprops[DEAC_COLD_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having cold resistance!");
						break;
					case 11:
					case 12:
					case 13:
					case 14:
					case 15:
						u.uprops[DEAC_SLEEP_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having sleep resistance!");
						break;
					case 16:
					case 17:
						u.uprops[DEAC_DISINT_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having disintegration resistance!");
						break;
					case 18:
					case 19:
					case 20:
					case 21:
					case 22:
						u.uprops[DEAC_SHOCK_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having shock resistance!");
						break;
					case 23:
					case 24:
					case 25:
					case 26:
					case 27:
						u.uprops[DEAC_POISON_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having poison resistance!");
						break;
					case 28:
					case 29:
					case 30:
						u.uprops[DEAC_DRAIN_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having drain resistance!");
						break;
					case 31:
					case 32:
						u.uprops[DEAC_SICK_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having sickness resistance!");
						break;
					case 33:
					case 34:
						u.uprops[DEAC_ANTIMAGIC].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having magic resistance!");
						break;
					case 35:
					case 36:
					case 37:
					case 38:
						u.uprops[DEAC_ACID_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having acid resistance!");
						break;
					case 39:
					case 40:
						u.uprops[DEAC_STONE_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having petrification resistance!");
						break;
					case 41:
						u.uprops[DEAC_FEAR_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having fear resistance!");
						break;
					case 42:
					case 43:
					case 44:
						u.uprops[DEAC_SEE_INVIS].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having see invisible!");
						break;
					case 45:
					case 46:
					case 47:
						u.uprops[DEAC_TELEPAT].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having telepathy!");
						break;
					case 48:
					case 49:
					case 50:
						u.uprops[DEAC_WARNING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having warning!");
						break;
					case 51:
					case 52:
					case 53:
						u.uprops[DEAC_SEARCHING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having automatic searching!");
						break;
					case 54:
						u.uprops[DEAC_CLAIRVOYANT].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having clairvoyance!");
						break;
					case 55:
					case 56:
					case 57:
					case 58:
					case 59:
						u.uprops[DEAC_INFRAVISION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having infravision!");
						break;
					case 60:
						u.uprops[DEAC_DETECT_MONSTERS].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having detect monsters!");
						break;
					case 61:
					case 62:
					case 63:
						u.uprops[DEAC_INVIS].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having invisibility!");
						break;
					case 64:
						u.uprops[DEAC_DISPLACED].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having displacement!");
						break;
					case 65:
					case 66:
					case 67:
						u.uprops[DEAC_STEALTH].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having stealth!");
						break;
					case 68:
						u.uprops[DEAC_JUMPING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having jumping!");
						break;
					case 69:
					case 70:
					case 71:
						u.uprops[DEAC_TELEPORT_CONTROL].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having teleport control!");
						break;
					case 72:
						u.uprops[DEAC_FLYING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having flying!");
						break;
					case 73:
						u.uprops[DEAC_MAGICAL_BREATHING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having magical breathing!");
						break;
					case 74:
						u.uprops[DEAC_PASSES_WALLS].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having phasing!");
						break;
					case 75:
					case 76:
						u.uprops[DEAC_SLOW_DIGESTION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having slow digestion!");
						break;
					case 77:
						u.uprops[DEAC_HALF_SPDAM].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having half spell damage!");
						break;
					case 78:
						u.uprops[DEAC_HALF_PHDAM].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having half physical damage!");
						break;
					case 79:
					case 80:
					case 81:
					case 82:
					case 83:
						u.uprops[DEAC_REGENERATION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having regeneration!");
						break;
					case 84:
					case 85:
						u.uprops[DEAC_ENERGY_REGENERATION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having mana regeneration!");
						break;
					case 86:
					case 87:
					case 88:
						u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having polymorph control!");
						break;
					case 89:
					case 90:
					case 91:
					case 92:
					case 93:
						u.uprops[DEAC_FAST].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having speed!");
						break;
					case 94:
					case 95:
					case 96:
						u.uprops[DEAC_REFLECTING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having reflection!");
						break;
					case 97:
					case 98:
					case 99:
						u.uprops[DEAC_FREE_ACTION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having free action!");
						break;
					case 100:
						u.uprops[DEAC_HALLU_PARTY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from hallu partying!");
						break;
					case 101:
						u.uprops[DEAC_DRUNKEN_BOXING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from drunken boxing!");
						break;
					case 102:
						u.uprops[DEAC_STUNNOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having stunnopathy!");
						break;
					case 103:
						u.uprops[DEAC_NUMBOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having numbopathy!");
						break;
					case 104:
						u.uprops[DEAC_FREEZOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having freezopathy!");
						break;
					case 105:
						u.uprops[DEAC_STONED_CHILLER].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from being a stoned chiller!");
						break;
					case 106:
						u.uprops[DEAC_CORROSIVITY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having corrosivity!");
						break;
					case 107:
						u.uprops[DEAC_FEAR_FACTOR].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having an increased fear factor!");
						break;
					case 108:
						u.uprops[DEAC_BURNOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having burnopathy!");
						break;
					case 109:
						u.uprops[DEAC_SICKOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having sickopathy!");
						break;
					case 110:
						u.uprops[DEAC_KEEN_MEMORY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from having keen memory!");
						break;
					case 111:
						u.uprops[DEAC_THE_FORCE].intrinsic += rnz( (monster_difficulty() * 10) + 1);
						pline("You are prevented from using the force like a real jedi!");
						break;
				}

			}

			if (AutomaticVulnerabilitiy) break;

			AutomaticVulnerabilitiy = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case TELE_ITEMS_TRAP:

			if (TeleportingItems) break;

			TeleportingItems = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case NASTINESS_TRAP:

			switch (rnd(55)) {

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

			}

			if (NastinessProblem) break;

			NastinessProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case STAIRS_TRAP:

			if (StairsProblem) break;

			StairsProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case ALIGNMENT_TRAP:

			if (AlignmentProblem) break;

			AlignmentProblem = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case CONFUSION_TRAP:

			if (ConfusionProblem) break;

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

		 switch (rnd(111)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				u.uprops[DEAC_FIRE_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having fire resistance!");
				break;
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				u.uprops[DEAC_COLD_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having cold resistance!");
				break;
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				u.uprops[DEAC_SLEEP_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having sleep resistance!");
				break;
			case 16:
			case 17:
				u.uprops[DEAC_DISINT_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having disintegration resistance!");
				break;
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
				u.uprops[DEAC_SHOCK_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having shock resistance!");
				break;
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
				u.uprops[DEAC_POISON_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having poison resistance!");
				break;
			case 28:
			case 29:
			case 30:
				u.uprops[DEAC_DRAIN_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having drain resistance!");
				break;
			case 31:
			case 32:
				u.uprops[DEAC_SICK_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having sickness resistance!");
				break;
			case 33:
			case 34:
				u.uprops[DEAC_ANTIMAGIC].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having magic resistance!");
				break;
			case 35:
			case 36:
			case 37:
			case 38:
				u.uprops[DEAC_ACID_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having acid resistance!");
				break;
			case 39:
			case 40:
				u.uprops[DEAC_STONE_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having petrification resistance!");
				break;
			case 41:
				u.uprops[DEAC_FEAR_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having fear resistance!");
				break;
			case 42:
			case 43:
			case 44:
				u.uprops[DEAC_SEE_INVIS].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having see invisible!");
				break;
			case 45:
			case 46:
			case 47:
				u.uprops[DEAC_TELEPAT].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having telepathy!");
				break;
			case 48:
			case 49:
			case 50:
				u.uprops[DEAC_WARNING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having warning!");
				break;
			case 51:
			case 52:
			case 53:
				u.uprops[DEAC_SEARCHING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having automatic searching!");
				break;
			case 54:
				u.uprops[DEAC_CLAIRVOYANT].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having clairvoyance!");
				break;
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
				u.uprops[DEAC_INFRAVISION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having infravision!");
				break;
			case 60:
				u.uprops[DEAC_DETECT_MONSTERS].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having detect monsters!");
				break;
			case 61:
			case 62:
			case 63:
				u.uprops[DEAC_INVIS].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having invisibility!");
				break;
			case 64:
				u.uprops[DEAC_DISPLACED].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having displacement!");
				break;
			case 65:
			case 66:
			case 67:
				u.uprops[DEAC_STEALTH].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having stealth!");
				break;
			case 68:
				u.uprops[DEAC_JUMPING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having jumping!");
				break;
			case 69:
			case 70:
			case 71:
				u.uprops[DEAC_TELEPORT_CONTROL].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having teleport control!");
				break;
			case 72:
				u.uprops[DEAC_FLYING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having flying!");
				break;
			case 73:
				u.uprops[DEAC_MAGICAL_BREATHING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having magical breathing!");
				break;
			case 74:
				u.uprops[DEAC_PASSES_WALLS].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having phasing!");
				break;
			case 75:
			case 76:
				u.uprops[DEAC_SLOW_DIGESTION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having slow digestion!");
				break;
			case 77:
				u.uprops[DEAC_HALF_SPDAM].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having half spell damage!");
				break;
			case 78:
				u.uprops[DEAC_HALF_PHDAM].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having half physical damage!");
				break;
			case 79:
			case 80:
			case 81:
			case 82:
			case 83:
				u.uprops[DEAC_REGENERATION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having regeneration!");
				break;
			case 84:
			case 85:
				u.uprops[DEAC_ENERGY_REGENERATION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having mana regeneration!");
				break;
			case 86:
			case 87:
			case 88:
				u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having polymorph control!");
				break;
			case 89:
			case 90:
			case 91:
			case 92:
			case 93:
				u.uprops[DEAC_FAST].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having speed!");
				break;
			case 94:
			case 95:
			case 96:
				u.uprops[DEAC_REFLECTING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having reflection!");
				break;
			case 97:
			case 98:
			case 99:
				u.uprops[DEAC_FREE_ACTION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having free action!");
				break;
			case 100:
				u.uprops[DEAC_HALLU_PARTY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from hallu partying!");
				break;
			case 101:
				u.uprops[DEAC_DRUNKEN_BOXING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from drunken boxing!");
				break;
			case 102:
				u.uprops[DEAC_STUNNOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having stunnopathy!");
				break;
			case 103:
				u.uprops[DEAC_NUMBOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having numbopathy!");
				break;
			case 104:
				u.uprops[DEAC_FREEZOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having freezopathy!");
				break;
			case 105:
				u.uprops[DEAC_STONED_CHILLER].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from being a stoned chiller!");
				break;
			case 106:
				u.uprops[DEAC_CORROSIVITY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having corrosivity!");
				break;
			case 107:
				u.uprops[DEAC_FEAR_FACTOR].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having an increased fear factor!");
				break;
			case 108:
				u.uprops[DEAC_BURNOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having burnopathy!");
				break;
			case 109:
				u.uprops[DEAC_SICKOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having sickopathy!");
				break;
			case 110:
				u.uprops[DEAC_KEEN_MEMORY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from having keen memory!");
				break;
			case 111:
				u.uprops[DEAC_THE_FORCE].intrinsic += rnz( (monster_difficulty() * 10) + 1);
				pline("You are prevented from using the force like a real jedi!");
				break;
			}

		 break;

		 case HEEL_TRAP:
			seetrap(trap);

			pline("You stepped on a trigger!");

			switch (trap->launch_otyp) {

				case 1:

				pline("A wedged sandal suddenly slams against your shins! It hurts!");
				losehp(rnd(10)+ rnd(monster_difficulty() + 1),"wedged sandal",KILLED_BY_AN);
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
				losehp(rnd(10)+ rnd(monster_difficulty() + 1),"soft sneaker",KILLED_BY_AN);
				break;

				case 5:

				pline("A leather peep-toe slams against your shins!");
				losehp(rnd(10)+ rnd(monster_difficulty() + 1),"leather peep-toe",KILLED_BY_AN);

				if (multi >= 0 && !rn2(2)) {
				    if (flags.female) {
					pline("It hurts a bit, but not too badly." );
				    } else if (Free_action) {
					pline("It hurts like hell, but you bear it like a man.");            
				    } else {
					pline("It hurts like hell! You pass out from the intense pain.");            
					nomovemsg = "You finally manage to get up again.";
					nomul(-(rnd(monster_difficulty() + 1)), "knocked out by a peep-toe trap" );
					exercise(A_DEX, FALSE);
				    }
				}

				break;

				case 6:

				pline("A sexy leather pump suddenly scratches up and down your %ss!", body_part(LEG) );

				losehp(rnd(10)+ rnd( (monster_difficulty() * 2) + 1),"sexy leather pump",KILLED_BY_AN);

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

				losehp(rnd(10)+ rnd( (monster_difficulty() * 4) + 1),"stiletto boot",KILLED_BY_AN);
				if (!rn2(250)) pushplayer();

				break;

				case 8:

				pline("Your %s is suddenly hit painfully by a high-heeled sandal!", body_part(HEAD) );

				losehp(rnd(12)+ rnd( monster_difficulty() + 1),"heeled sandal to the head",KILLED_BY_AN);

				break;

				case 9:

				pline("Klock! A sexy leather pump suddenly slams on your %s, producing a beautiful sound.", body_part(HEAD) );

				losehp(rnd(20)+ rnd( monster_difficulty() + 1),"sexy leather pump to the head",KILLED_BY_AN);

				break;

				case 10:

				pline("Suddenly, a spiked battle boot squeezes and stings your skin!" );

				losehp(rnd(10)+ rnd( monster_difficulty() + 1),"battle boot with spikes",KILLED_BY_AN);
				    if (!rn2(6))
					poisoned("spikes", A_STR, "poisoned battle boot", 8);

				break;

				case 11:

				pline("A wedged little-girl sandal painfully thunders on your %s!", body_part(HEAD) );

				losehp(rnd(4)+ rnd( monster_difficulty() + 1),"wedged girl sandal to the head",KILLED_BY_AN);

				break;

				case 12:

				pline("A girl's plateau boot unyieldingly bonks your %s!", body_part(HEAD) );

				losehp(rnd(10)+ rnd( monster_difficulty() + 1),"female plateau boot to the head",KILLED_BY_AN);

				break;

				case 13:

				pline("Your %s is hit hard by a thick hugging boot!", body_part(HEAD) );

				losehp(rnd(12)+ rnd( monster_difficulty() + 1),"female winter boot to the head",KILLED_BY_AN);
				if (Upolyd) u.mhmax--; /* lose one hit point */
				else u.uhpmax--; /* lose one hit point */
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				if (u.mh > u.mhmax) u.mh = u.mhmax;

				break;

				case 14:

				pline("An unyielding wooden sandal painfully lands on your %s!", body_part(HEAD) );

				losehp(rnd(15)+ rnd( monster_difficulty() + 1),"wooden geta to the head",KILLED_BY_AN);

					if (multi >= 0 && !rn2(2)) {
					    if (Free_action) {
						pline("You struggle to stay on your %s.", makeplural(body_part(FOOT)));
					    } else {
						pline("You're knocked out and helplessly drop to the floor.");
						nomovemsg = 0;	/* default: "you can move again" */
						nomul(-rnd(5), "knocked out by a geta trap");
						exercise(A_DEX, FALSE);
						    }
						}

				break;

				case 15:

					if (Role_if(PM_COURIER)) pline("A lacquered dancing shoe appears and harmlessly scratches you.");
					else {pline("A lacquered dancing shoe appears and scratches your %s!", body_part(HEAD));

						if (!uarmh || uarmh->otyp != DUNCE_CAP) {

					    /* No such thing as mindless players... */
					    if (ABASE(A_INT) <= ATTRMIN(A_INT)) {
						int lifesaved = 0;
						struct obj *wore_amulet = uamul;
			
						while(1) {
						    /* avoid looping on "die(y/n)?" */
						    if (lifesaved && (discover || wizard)) {
							if (wore_amulet && !uamul) {
							    /* used up AMULET_OF_LIFE_SAVING; still
							       subject to dying from brainlessness */
							    wore_amulet = 0;
							} else {
							    /* explicitly chose not to die;
							       arbitrarily boost intelligence */
							    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
							    You_feel("like a scarecrow.");
							    break;
							}
						    }

						    if (lifesaved)
							pline("Unfortunately your brain is still gone.");
						    else
							Your("last thought fades away.");
						    killer = "brainlessness";
						    killer_format = KILLED_BY;
						    done(DIED);
						    lifesaved++;
						}
					    }
					}
					/* adjattrib gives dunce cap message when appropriate */
					if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE);
					else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE);
					if (!rn2(3)) forget_levels(5);	/* lose memory of 25% of levels */
					if (!rn2(10)) forget_objects(5);	/* lose memory of 25% of objects */
					exercise(A_WIS, FALSE);

						}

				break;

			}

		 break;

		 case AUTOMATIC_SWITCHER:

			if (RMBLoss || Superscroller || DisplayLoss || SpellLoss || YellowSpells || AutoDestruct || MemoryLoss || InventoryLoss || BlackNgWalls || MenuBug || SpeedBug || FreeHandLoss || Unidentify || Thirst || LuckLoss || ShadesOfGrey || FaintActive || Itemcursing || DifficultyIncreased || Deafness || CasterProblem || WeaknessProblem || NoDropProblem || RotThirteen || BishopGridbug || ConfusionProblem || DSTWProblem || StatusTrapProblem || AlignmentProblem || StairsProblem || UninformationProblem || IntrinsicLossProblem || BloodLossProblem || BadEffectProblem || TrapCreationProblem ||AutomaticVulnerabilitiy || TeleportingItems || NastinessProblem || CaptchaProblem || RespawnProblem || FarlookProblem || RecurringAmnesia || BigscriptEffect || BankTrapEffect || MapTrapEffect || TechTrapEffect || RecurringDisenchant || verisiertEffect || ChaosTerrain || Muteness || EngravingDoesntWork || MagicDeviceEffect || BookTrapEffect || LevelTrapEffect || QuizTrapEffect ) {

			RMBLoss = 0L;
			DisplayLoss = 0L;
			SpellLoss = 0L;
			YellowSpells = 0L;
			AutoDestruct = 0L;
			MemoryLoss = 0L;
			InventoryLoss = 0L;
			BlackNgWalls = 0L;
			MenuBug = 0L;
			SpeedBug = 0L;
			Superscroller = 0L;
			FreeHandLoss = 0L;
			Unidentify = 0L;
			Thirst = 0L;
			LuckLoss = 0L;
			ShadesOfGrey = 0L;
			FaintActive = 0L;
			Itemcursing = 0L;
			DifficultyIncreased = 0L;
			Deafness = 0L;
			CasterProblem = 0L;
			WeaknessProblem = 0L;
			RotThirteen = 0L;
			BishopGridbug = 0L;
			ConfusionProblem = 0L;
			NoDropProblem = 0L;
			DSTWProblem = 0L;
			StatusTrapProblem = 0L;
			AlignmentProblem = 0L;
			StairsProblem = 0L;
			UninformationProblem = 0L;
			IntrinsicLossProblem = 0L;
			BloodLossProblem = 0L;
			BadEffectProblem = 0L;
			TrapCreationProblem = 0L;
			AutomaticVulnerabilitiy = 0L;
			TeleportingItems = 0L;
			NastinessProblem = 0L;
			CaptchaProblem = 0L;
			FarlookProblem = 0L;
			RespawnProblem = 0L;
			RecurringAmnesia = 0L;
			BigscriptEffect = 0L;
			BankTrapEffect = 0L;
			MapTrapEffect = 0L;
			TechTrapEffect = 0L;
			RecurringDisenchant = 0L;
			verisiertEffect = 0L;
			ChaosTerrain = 0L;
			Muteness = 0L;
			EngravingDoesntWork = 0L;
			MagicDeviceEffect = 0L;
			BookTrapEffect = 0L;
			LevelTrapEffect = 0L;
			QuizTrapEffect = 0L;
			deltrap(trap); /* used up if anything was cured */

			}

		 break;

		 case SPEED_TRAP:

			if (SpeedBug) break;

			SpeedBug = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SWARM_TRAP:

			deltrap(trap); /* triggers only once */

			    coord cc;

			if (!rn2(3)) {

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

			else if (!rn2(2)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			randmonstforspawn = rndmonst();

			if (wizard || !rn2(10)) pline("You feel the arrival of monsters!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(randmonstforspawn, 0, 0, NO_MM_FLAGS);
			}

			}

			else {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			monstercolor = rnd(15);
			do { monstercolor = rnd(15); } while (monstercolor == CLR_BLUE);

			if (wizard || !rn2(10)) pline("You feel a colorful sensation!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(colormon(monstercolor), 0, 0, NO_MM_FLAGS);
			}

			}

		 break;

		 case CYANIDE_TRAP:
			pline("CLICK! You have triggered a trap!");
			seetrap(trap);
			pline("Cyanide gas blows in your %s!", body_part(FACE) ); /* unbreathing doesn't help --Amy */
			if (Upolyd) {
				if (u.mh > 1) u.mh /= 2;
				else losehp(100, "cyanide trap", KILLED_BY_AN);
			} else {
				if (u.uhp > 1) u.uhp /= 2;
				else losehp(100, "cyanide trap", KILLED_BY_AN);
			}
			pline("You feel very poisoned...");
		      poisoned("gas", rn2(A_MAX), "cyanide trap", 5);

		 break;

		 case NEST_TRAP:

			deltrap(trap); /* triggers only once */

			    coord dd;
			    int cx,cy;

		      cx = rn2(COLNO);
		      cy = rn2(ROWNO);

			if (!rn2(3)) {

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

			else if (!rn2(2)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			randmonstforspawn = rndmonst();

			if (wizard || !rn2(10)) pline("You feel the arrival of monsters!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(randmonstforspawn, cx, cy, MM_ADJACENTOK);
			}

			}

			else {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			monstercolor = rnd(15);
			do { monstercolor = rnd(15); } while (monstercolor == CLR_BLUE);

			if (wizard || !rn2(10)) pline("You feel a colorful sensation!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(colormon(monstercolor), cx, cy, MM_ADJACENTOK);
			}

			}

		 break;

		 case DISPLAY_TRAP:

			if (DisplayLoss) break;

			DisplayLoss = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case SPELL_LOSS_TRAP:

			if (SpellLoss) break;

			SpellLoss = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case YELLOW_SPELL_TRAP:

			if (YellowSpells) break;

			YellowSpells = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case AUTO_DESTRUCT_TRAP:

			if (AutoDestruct) break;

			AutoDestruct = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case MEMORY_TRAP:

			if (MemoryLoss) break;

			MemoryLoss = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case INVENTORY_TRAP:

			if (InventoryLoss) break;

			InventoryLoss = rnz(nastytrapdur * (monster_difficulty() + 1));

		 break;

		 case BLACK_NG_WALL_TRAP:

			if (BlackNgWalls) break;

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

	for (rocks = rnd(100)+rnd(100);rocks > 0;rocks--) {
		rx = zx + rn2(3)-1; 
		ry = zy + rn2(3)-1;
		if (levl[rx][ry].typ == ROOM || levl[rx][ry].typ == CORR) {
			if (rn2(50)) {
				otmp = mksobj(ROCK,FALSE,FALSE);
			} else {
				otmp = mksobj(BOULDER,FALSE,FALSE);
			}

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
			} else if (mtmp = m_at(rx,ry)) {
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
			if (is_metallic(uarmh)) {
				pline("Your hard helmet protects you somewhat.");
				dmg /= 2;
			} else if (flags.verbose) {
				Your("%s does not protect you.", xname(uarmh));
			}
		}
	}
	losehp(dmg + rnd((monster_difficulty()*2) + 1 ),"shower of rocks",KILLED_BY_AN);

}



#ifdef STEED
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
		case ARROW_TRAP:
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
#endif /*STEED*/

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
	    (void) memset((genericptr_t)&digging, 0, sizeof digging);

	dist = distmin(x1,y1,x2,y2);
	bhitpos.x = x1;
	bhitpos.y = y1;
	dx = sgn(x2 - x1);
	dy = sgn(y2 - y1);
	switch (style) {
	    case ROLL|LAUNCH_UNSEEN:
			if (otyp == BOULDER) {
			    You_hear(Hallucination ?
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
			if (multi) nomul(0, 0);
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
			case TELEP_TRAP:
			    if (cansee(bhitpos.x, bhitpos.y))
			    	pline("Suddenly the rolling boulder disappears!");
			    else
			    	You_hear("a rumbling stop abruptly.");
			    singleobj->otrapped = 0;
			    if (t->ttyp == TELEP_TRAP)
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
			case SHAFT_TRAP:
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
				&& is_pool(x+distance*dx,y+distance*dy))
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
		otmp = mksobj(otyp, TRUE, FALSE);
		otmp->quan = ocount;
		otmp->owt = weight(otmp);
		place_object(otmp, cc.x, cc.y);
		stackobj(otmp);
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
		(trap->ttyp == SPIKED_PIT || trap->ttyp == GIANT_CHASM || trap->ttyp == SHIT_PIT || trap->ttyp == BEAR_TRAP ||
		 trap->ttyp == HOLE || trap->ttyp == PIT ||
		 trap->ttyp == WEB)) {
		/* If you come upon an obviously trapped monster, then
		 * you must be able to see the trap it's in too.
		 */
		seetrap(trap);
	    }
		
	    if (!rn2(40)) {
		if (sobj_at(BOULDER, mtmp->mx, mtmp->my) &&
			(trap->ttyp == PIT || trap->ttyp == SPIKED_PIT || trap->ttyp == GIANT_CHASM || trap->ttyp == SHIT_PIT)) {
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
	    }
	} else {
	    register int tt = trap->ttyp;
	    boolean in_sight, tear_web, see_it,
		    inescapable = ((tt == HOLE || tt == PIT) &&
				   In_sokoban(&u.uz) && !trap->madeby_u);
	    const char *fallverb;

#ifdef STEED
	    /* true when called from dotrap, inescapable is not an option */
	    if (mtmp == u.usteed) inescapable = TRUE;
#endif
	    if (!inescapable &&
		    ((mtmp->mtrapseen & (1 << (tt-1))) != 0 ||
			(tt == HOLE && !mindless(mtmp->data)))) {
		/* it has been in such a trap - perhaps it escapes */
		if(rn2(4)) return(0);
	    } else {
		mtmp->mtrapseen |= (1 << (tt-1));
	    }
	    /* Monster is aggravated by being trapped by you.
	       Recognizing who made the trap isn't completely
	       unreasonable; everybody has their own style. */
	    if (trap->madeby_u && rnl(5)) setmangry(mtmp);

	    in_sight = canseemon(mtmp);
	    see_it = cansee(mtmp->mx, mtmp->my);
#ifdef STEED
	    /* assume hero can tell what's going on for the steed */
	    if (mtmp == u.usteed) in_sight = TRUE;
#endif
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
			otmp = mksobj(ARROW, TRUE, FALSE);
			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = 0;
			if (in_sight) seetrap(trap);
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
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
			otmp = mksobj(CROSSBOW_BOLT, TRUE, FALSE);
			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = 0;
			if (in_sight) seetrap(trap);
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
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
			otmp = mksobj(BULLET, TRUE, FALSE);
			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = 0;
			if (in_sight) seetrap(trap);
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
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
			otmp = mksobj(DROVEN_ARROW, TRUE, FALSE);
			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = 0;
			if (in_sight) seetrap(trap);
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
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
			otmp = mksobj(DROVEN_BOLT, TRUE, FALSE);
			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = 0;
			if (in_sight) seetrap(trap);
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
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
			otmp = mksobj(DART, TRUE, FALSE);
			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			if (!rn2(6)) otmp->opoisoned = 1;
			if (in_sight) seetrap(trap);
			if (thitm(7, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
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
			otmp = mksobj(SHURIKEN, TRUE, FALSE);
			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			if (!rn2(6)) otmp->opoisoned = 1;
			if (in_sight) seetrap(trap);
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
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
			otmp = mksobj(ROCK, TRUE, FALSE);
			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = 0;
			if (in_sight) seetrap(trap);
			if (thitm(0, mtmp, otmp, d(2, 6), FALSE))
			    trapkilled = TRUE;
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
			otmp = mksobj(BOULDER, TRUE, FALSE);
			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = 0;
			if (in_sight) seetrap(trap);
			if (thitm(0, mtmp, otmp, d(6, 6), FALSE))
			    trapkilled = TRUE;
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
				    if (!DEADMONSTER(mtmp3)) {
					mtmp3->msleeping = 0;
				    }
				}

			}
			break;

		case BEAR_TRAP:
			if(mptr->msize > MZ_SMALL &&
				!amorphous(mptr) && !is_flyer(mptr) && (!mtmp->egotype_flying) &&
				!is_whirly(mptr) && !unsolid(mptr)) {
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
		    if (!resists_sleep(mtmp) && !breathless(mptr) && (!mtmp->egotype_undead) &&
				!mtmp->msleeping && mtmp->mcanmove) {
			    mtmp->mcanmove = 0;
			    mtmp->mfrozen = rnd(25);
			    if (in_sight) {
				pline("%s suddenly falls asleep!",
				      Monnam(mtmp));
				seetrap(trap);
			    }
			if (!rn2(50)) deltrap(trap);
			}
			break;

		case PARALYSIS_TRAP:
		      {
			    mtmp->mcanmove = 0;
			    mtmp->mfrozen = rnd(10);
			    if (in_sight) {
				pline("%s is suddenly paralyzed!", Monnam(mtmp));
				seetrap(trap);
			    }
			if (!rn2(50)) deltrap(trap);
			}
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

			if (!rn2(50)) deltrap(trap);
			}
			break;

		case SLOW_GAS_TRAP:
		    if (!breathless(mptr) && (!mtmp->egotype_undead) ) {

			    if (in_sight) {
				pline("%s inhales a cloud of foggy gas!", Monnam(mtmp));
				seetrap(trap);
			    }
			mon_adjust_speed(mtmp, -1, (struct obj *)0);
			if (!rn2(50)) deltrap(trap);
			}
			break;

		case RUST_TRAP:
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
			    (void) rust_dmg(target, "gauntlets", 1, TRUE, mtmp);
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
#ifdef TOURIST
				else {
				    target = which_armor(mtmp, W_ARMU);
				    (void) rust_dmg(target, "shirt", 1, TRUE, mtmp);
				}
#endif
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
			} else if (mptr == &mons[PM_FLAMING_SPHERE]) {
				if (in_sight)
				    pline("%s is extinguished!", Monnam(mtmp));
				mondied(mtmp);
				if (mtmp->mhp <= 0)
					trapkilled = TRUE;
			} else if (mptr == &mons[PM_GREMLIN] && rn2(3)) {
				(void)split_mon(mtmp, (struct monst *)0);
			}
			if (!rn2(50)) deltrap(trap);
			break;
		    }
		case FIRE_TRAP:
 mfiretrap:
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
			if (!rn2(50)) deltrap(trap);
			break;

		case PLASMA_TRAP:
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
			if (!rn2(50)) deltrap(trap);
			break;

           case ICE_TRAP:
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
               else if (!rn2(33))
                (void) destroy_mitem(mtmp, POTION_CLASS, AD_COLD);
               }
           if (see_it) seetrap(trap);
		if (!rn2(50)) deltrap(trap);
           break;

           case SHOCK_TRAP:
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
		if (!rn2(50)) deltrap(trap);
           break;

           case ACID_POOL:

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
		if (!rn2(50)) deltrap(trap);
           break;

           case DEATH_TRAP:

		if (nonliving(mtmp->data) || is_demon(mtmp->data) || resists_death(mtmp) || mtmp->data->msound == MS_NEMESIS || resists_magm(mtmp)) break;

		else {

               if (in_sight) pline("%s is lit by a black glow...", Monnam(mtmp));

               int num = d(5,4);
               if (thitm(0, mtmp, (struct obj *)0, num, FALSE))
                 trapkilled = TRUE;

		}

		if (see_it) seetrap(trap);
		if (!rn2(50)) deltrap(trap);

		break;

           case DISINTEGRATION_TRAP:

		if (resists_disint(mtmp)) break;

		else {

               if (in_sight) pline("%s is shaken heavily!", Monnam(mtmp));

               int num = d(4,4);
               if (thitm(0, mtmp, (struct obj *)0, num, FALSE))
                 trapkilled = TRUE;

		}

		if (see_it) seetrap(trap);
		if (!rn2(50)) deltrap(trap);

		break;

           case DRAIN_TRAP:

		if (resists_drli(mtmp)) break;

		else {

               if (in_sight) pline("%s looks less powerful!", Monnam(mtmp));

               int num = d(3,4);
               if (thitm(0, mtmp, (struct obj *)0, num, FALSE))
                 trapkilled = TRUE;

		}

		if (see_it) seetrap(trap);
		if (!rn2(50)) deltrap(trap);

		break;

           case WATER_POOL:

		if (is_floater(mtmp->data) || is_flyer(mtmp->data) || mtmp->egotype_flying || mtmp->egotype_watersplasher || is_swimmer(mtmp->data) || amphibious(mtmp->data) || breathless(mtmp->data) || mtmp->egotype_undead ) break;

           if (in_sight)
             pline("%s falls into a pool of water!", Monnam(mtmp));
           else if (see_it)
             You("see a splash of water!");

               int num = d(2,2);
               if (thitm(0, mtmp, (struct obj *)0, num, FALSE))
                 trapkilled = TRUE;

           if (see_it) seetrap(trap);
		if (!rn2(50)) deltrap(trap);
           break;

           case SHIT_TRAP:

		if( (is_flyer(mptr) || mtmp->egotype_flying) && (mptr != &mons[PM_ARABELLA]) && (mptr != &mons[PM_ARABELLA_THE_TUFTED_ASIAN_GIRL]) && (mptr != &mons[PM_ANASTASIA_THE_SWEET_BLONDE]) && (mptr != &mons[PM_HENRIETTA_THE_BURLY_WOMAN]) && (mptr != &mons[PM_KATRIN_THE_VIOLET_BEAUTY]) && (mptr != &mons[PM_JANA_THE_SOFT_WENCH]) && (mptr != &mons[PM_ANASTASIA]) && (mptr != &mons[PM_HENRIETTA]) && (mptr != &mons[PM_KATRIN]) && (mptr != &mons[PM_JANA]) & (mptr != &mons[PM_TUFTED_ASIAN_GIRL]) && (mptr != &mons[PM_SWEET_BLONDE]) && (mptr != &mons[PM_BURLY_WOMAN]) && (mptr != &mons[PM_VIOLET_BEAUTY]) && (mptr != &mons[PM_SOFT_WENCH]) && (mptr != &mons[PM_ARABELLA_THE_MONEY_THIEF]) ) break; /* since this is a ground-based trap */
		/* The spacewars fighter nemeses somehow have affinity to this type of trap. */

           if (in_sight)
             pline("%s steps into a heap of shit!", mon_nam(mtmp));
           else if (see_it)
             You("see %s stepping into a heap of shit!", surface(mtmp->mx, mtmp->my));

			mon_adjust_speed(mtmp, 1, (struct obj *)0);

           if (see_it) seetrap(trap);
		if (!rn2(20)) deltrap(trap);
           break;

		case PIT:
		case SPIKED_PIT:
		case GIANT_CHASM:
		case SHIT_PIT:
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
			if (!Can_fall_thru(&u.uz)) {
			 impossible("mintrap: %ss cannot exist on this level.",
				    defsyms[trap_to_defsym(tt)].explanation);
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
			/* Fall through */
		case LEVEL_TELEP:
			{
			    int mlev_res;
			    mlev_res = mlevel_tele_trap(mtmp, trap,
							inescapable, in_sight);
			    if (mlev_res) return(mlev_res);
			}
			break;

		case MAGIC_PORTAL: /* no longer allows players to simply get rid of them --Amy */
		case RELOCATION_TRAP:
			rloc(mtmp, FALSE);
			break;

		case TELEP_TRAP:
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
		case INTRINSIC_LOSS_TRAP:
		case BLOOD_LOSS_TRAP:
		case BAD_EFFECT_TRAP:
		case MULTIPLY_TRAP:
		case AUTO_VULN_TRAP:
		case TELE_ITEMS_TRAP:
		case NASTINESS_TRAP:

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

			break;

		case LOUDSPEAKER:
			pline(fauxmessage());
			break;
		case MAGIC_TRAP:
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
			if (!rn2(50)) deltrap(trap);
			break;
		case LANDMINE:
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
			if (thitm(0, mtmp, (struct obj *)0, rnd(16), FALSE))
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

		case POLY_TRAP:
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
			if (!rn2(5)) deltrap(trap); /* this should tone down abuse potential --Amy */
		    break;

		case SPEAR_TRAP:
			if (in_sight) {
				seetrap(trap);
				pline("A spear stabs up from a hole in the ground!");
			}
			if (!rn2(5)) { /* Sorry, no infinite conga lines of death. --Amy */
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
			if (in_sight) {
				seetrap(trap);
			}
			if (!rn2(15)) { /* Sorry, no infinite conga lines of death. --Amy */
				if (in_sight) {
					pline("You see %s smash a scything blade apart!",mon_nam(mtmp));
				}
				deltrap(trap);
			} else if (unsolid(mptr)) {
				if (in_sight) {
					pline("A scything blade passes right through %s!",mon_nam(mtmp));
				}
			} else {
				if (thitm(0, mtmp, (struct obj *)0, rnd(9 + has_head(mtmp->data) ? 3 : 0), FALSE)) {
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
		if (!rn2(7)) deltrap(trap); /* making sure monsters don't get killed over and over --Amy */
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
	if (Stone_resistance) return;
	if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
	    return;
	/* You("turn to stone...");
	killer_format = KILLED_BY;
	killer = str;
	done(STONING); */
	if (!Stoned) Stoned = 7;
	delayed_killer = "coming into contact with a petrifying object";
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
	char kbuf[BUFSZ];

	if(uwep && uwep->otyp == CORPSE && touch_petrifies(&mons[uwep->corpsenm])
			&& !Stone_resistance) {
		pline("%s touch the %s corpse.", arg,
		        mons[uwep->corpsenm].mname);
		Sprintf(kbuf, "%s corpse", an(mons[uwep->corpsenm].mname));
		instapetrify(kbuf);
	}
	/* Or your secondary weapon, if wielded */
	if(u.twoweap && uswapwep && uswapwep->otyp == CORPSE &&
			touch_petrifies(&mons[uswapwep->corpsenm]) && !Stone_resistance){
		pline("%s touch the %s corpse.", arg,
		        mons[uswapwep->corpsenm].mname);
		Sprintf(kbuf, "%s corpse", an(mons[uswapwep->corpsenm].mname));
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
		if (!Hallucination)
			You_feel("limber!");
	    else
		pline("What a pity - you just ruined a future piece of %sart!",
		      ACURR(A_CHA) > 15 ? "fine " : "");
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
	else if (Hallucination)
		pline("Up, up, and awaaaay!  You're walking on air!");
	else if(Is_airlevel(&u.uz))
		You("gain control over your movements.");
	else
		You("start to float in the air!");
#ifdef STEED
	if (u.usteed && !is_floater(u.usteed->data) &&
						!is_flyer(u.usteed->data) && (!u.usteed->egotype_flying) ) {
	    if (Lev_at_will)
	    	pline("%s magically floats up!", Monnam(u.usteed));
	    else {
	    	You("cannot stay on %s.", mon_nam(u.usteed));
	    	dismount_steed(DISMOUNT_GENERIC);
	    }
	}
#endif
	return;
}

void
fill_pit(x, y)
int x, y;
{
	struct obj *otmp;
	struct trap *t;

	if ((t = t_at(x, y)) &&
	    ((t->ttyp == PIT) || (t->ttyp == SPIKED_PIT) || (t->ttyp == GIANT_CHASM) || (t->ttyp == SHIT_PIT)) &&
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
	    (is_pool(uball->ox, uball->oy) ||
	     ((trap = t_at(uball->ox, uball->oy)) &&
	      ((trap->ttyp == PIT) || (trap->ttyp == SPIKED_PIT) || (trap->ttyp == GIANT_CHASM) || (trap->ttyp == SHIT_PIT) || (trap->ttyp == SHAFT_TRAP) ||
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
	if(!Flying) {
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
		if(is_pool(u.ux,u.uy) && !Wwalking && !Swimming && !u.uinwater)
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
#ifdef STEED
		if (!(emask & W_SADDLE))
#endif
		{
		    boolean sokoban_trap = (In_sokoban(&u.uz) && trap);
		    if (Hallucination)
			pline("Bummer!  You've %s.",
			      is_pool(u.ux,u.uy) ?
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
#ifdef STEED
			    if (u.usteed) dismount_steed(DISMOUNT_FELL);
#endif
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
			break;
		case HOLE:
		case TRAPDOOR:
		case SHAFT_TRAP:
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
        if (box) {
                impossible("doicetrap() called with non-null box.");
                return;
        }

        pline("A freezing cloud shoots from %s!", surface(u.ux, u.uy));
        if (Cold_resistance) {
                shieldeff(u.ux, u.uy);
                num = d(2, 2) + rnd((monster_difficulty() / 5) + 1);
        }

        if (!num)
                You("are uninjured.");
        else
                losehp(num, "freezing cloud", KILLED_BY_AN);
			if (!rn2(10)) make_frozen(HFrozen + (num * 5), TRUE); /* randomly freeze the player --Amy */
		    if (!rn2(33)) /* new calculations --Amy */        destroy_item(POTION_CLASS, AD_COLD);
}

STATIC_OVL void
doshocktrap(box)
struct obj *box;        /* at the moment only for floor traps */
{
        int num = 0;
        num = d(4, 4) + rnd((monster_difficulty() / 3) + 1);
        if (box) {
                impossible("doshocktrap() called with non-null box.");
                return;
        }

        pline("A lightning bolt hits you!");
        if (Shock_resistance) {
                shieldeff(u.ux, u.uy);
                num = d(2, 2) + rnd((monster_difficulty() / 5) + 1);
        }

        if (!num)
                You("are uninjured.");
        else
                losehp(num, "lightning trap", KILLED_BY_AN);
			if (!rn2(3)) make_numbed(HNumbed + (num * 5), TRUE); /* PLAYER NUMBED CAN'T DO --Amy */
		    if (!rn2(33)) destroy_item(WAND_CLASS, AD_ELEC);
		    if (!rn2(33)) destroy_item(RING_CLASS, AD_ELEC);
}

STATIC_OVL void
dovolttrap(box)
struct obj *box;        /* at the moment only for floor traps */
{
        int num = 0;
        num = d(8, 4) + rnd(monster_difficulty() + 1);
        if (box) {
                impossible("doshocktrap() called with non-null box.");
                return;
        }

        pline("A massive electric shock surges through your body!");
        if (Shock_resistance) {
                shieldeff(u.ux, u.uy);
                num = d(6, 3) + rnd((monster_difficulty() / 2) + 1);
        }

        if (!num)
                You("are uninjured.");
        else
                losehp(num, "volt trap", KILLED_BY_AN);
		    make_numbed(HNumbed + (num * 5), TRUE); /* PLAYER NUMBED CAN'T DO --Amy */
		    if (!rn2(10)) destroy_item(WAND_CLASS, AD_ELEC);
		    if (!rn2(10)) destroy_item(RING_CLASS, AD_ELEC);
			if (!rn2(3)) {
				if (!Free_action) {
				    pline("You are frozen in place!");
				    nomul(-rnz(10), "frozen by a volt trap");
				    nomovemsg = You_can_move_again;
				    exercise(A_DEX, FALSE);
				} else You("stiffen momentarily.");

			}

}

STATIC_OVL void
doshittrap(box)
struct obj *box;        /* at the moment only for floor traps */
{
        int num = 0;
        num = d(4, 4) + rnd((monster_difficulty() / 2) + 1);
        if (box) {
                impossible("doshittrap() called with non-null box.");
                return;
        }

        pline("You stepped into a heap of shit!");
        if (Acid_resistance) { /* let's just assume the stuff is acidic or corrosive --Amy */
                shieldeff(u.ux, u.uy);
                num = d(2, 2)+ rnd((monster_difficulty() / 4) + 1);
        }

		if (Stoned) fix_petrification();

	    if (uarmf && !rn2(5)) (void)rust_dmg(uarmf, xname(uarmf), 0, TRUE, &youmonst);
	    if (uarmf && !rn2(5)) (void)rust_dmg(uarmf, xname(uarmf), 1, TRUE, &youmonst);
	    if (uarmf && !rn2(5)) (void)rust_dmg(uarmf, xname(uarmf), 2, TRUE, &youmonst);
	    if (uarmf && !rn2(5)) (void)rust_dmg(uarmf, xname(uarmf), 3, TRUE, &youmonst);
		/* Dog shit is extremely aggressive to footwear. Let's give it a chance to do withering damage. --Amy */
	    if (uarmf && !rn2(25)) (void)wither_dmg(uarmf, xname(uarmf), 0, TRUE, &youmonst);
	    if (uarmf && !rn2(25)) (void)wither_dmg(uarmf, xname(uarmf), 1, TRUE, &youmonst);
	    if (uarmf && !rn2(25)) (void)wither_dmg(uarmf, xname(uarmf), 2, TRUE, &youmonst);
	    if (uarmf && !rn2(25)) (void)wither_dmg(uarmf, xname(uarmf), 3, TRUE, &youmonst);

		if (!uarmf) {
			pline("You slip on the shit with your bare %s.", makeplural(body_part(FOOT)));
			num *= 2;
		}

		if (!rn2(20)) u_slow_down();

		if ( !rn2(100) || (!Free_action && !rn2(10)))	{
			You("inhale the intense smell of shit! The world spins and goes dark.");
			nomovemsg = "You are conscious again.";	/* default: "you can move again" */
			nomul(-rnd(10), "unconscious from smelling dog shit");
			exercise(A_DEX, FALSE);
		}

        if (num) losehp(num, "heap of shit", KILLED_BY_AN);

}

STATIC_OVL void
dofiretrap(box)
struct obj *box;	/* null for floor trap */
{
	boolean see_it = !Blind;
	int num, alt;

/* Bug: for box case, the equivalent of burn_floor_paper() ought
 * to be done upon its contents.
 */

	if ((box && !carried(box)) ? is_pool(box->ox, box->oy) : Underwater) {
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
	    if (u.uhpmax > u.ulevel)
		u.uhpmax -= rn2(min(u.uhpmax,5)), flags.botl = 1;
	}
	if (!num)
	    You("are uninjured.");
	else
	    losehp(num + rnd((monster_difficulty() / 3) + 1), tower_of_flame, KILLED_BY_AN);
	burn_away_slime();

	/*if ( burnarmor(&youmonst) || !rn2(33))*/ /* new calculation -- Amy */ {
	    if (!rn2(33)) destroy_item(SCROLL_CLASS, AD_FIRE);
	    if (!rn2(33)) destroy_item(SPBOOK_CLASS, AD_FIRE);
	    if (!rn2(33)) destroy_item(POTION_CLASS, AD_FIRE);
		if (!rn2(33)) burnarmor(&youmonst);
	}
	if (!box && burn_floor_paper(u.ux, u.uy, see_it, TRUE) && !see_it)
	    You("smell paper burning.");
	if (is_ice(u.ux, u.uy))
	    melt_ice(u.ux, u.uy);
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

	if ((box && !carried(box)) ? is_pool(box->ox, box->oy) : Underwater) {
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
	    num = rnd(12);
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
	}
	if (!num)
	    You("are uninjured.");
	else {
	    losehp(num + rnd(monster_difficulty() + 1), "plasma trap", KILLED_BY_AN);
		make_stunned(HStun + num + rnd(monster_difficulty() + 1), TRUE);
	}

	burn_away_slime();

	    if (!rn2(5)) destroy_item(SCROLL_CLASS, AD_FIRE);
	    if (!rn2(5)) destroy_item(SPBOOK_CLASS, AD_FIRE);
	    if (!rn2(5)) destroy_item(POTION_CLASS, AD_FIRE);
		if (!rn2(5)) burnarmor(&youmonst);

	if (!box && burn_floor_paper(u.ux, u.uy, see_it, TRUE) && !see_it)
	    You("smell paper burning.");
	if (is_ice(u.ux, u.uy))
	    melt_ice(u.ux, u.uy);
}

STATIC_OVL void
domagictrap()
{
	register int fate = rnd(20);

	/* What happened to the poor sucker? */

	if (fate < 10) {
	  /* Most of the time, it creates some monsters. */
	  register int cnt = rnd(4);

	  if (!resists_blnd(&youmonst)) {
		You("are momentarily blinded by a flash of light!");
		make_blinded((long)rn1(5,10),FALSE);
		if (!Blind) Your(vision_clears);
	  } else if (!Blind) {
		You("see a flash of light!");
	  }  else
		You_hear("a deafening roar!");
	  while(cnt--)
		(void) makemon((struct permonst *) 0, u.ux, u.uy, NO_MM_FLAGS);
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
	     case 13:	pline("A shiver runs up and down your %s!",
			      body_part(SPINE));
			break;
	     case 14:	You_hear(Hallucination ?
				"the moon howling at you." :
				"distant howling.");
			break;
	     case 15:	if (on_level(&u.uz, &qstart_level))
			    You_feel("%slike the prodigal son.",
			      (flags.female || (Upolyd && is_neuter(youmonst.data))) ?
				     "oddly " : "");
			else
			    You("suddenly yearn for %s.",
				Hallucination ? "Cleveland" :
			    (In_quest(&u.uz) || at_dgn_entrance("The Quest")) ?
						"your nearby homeland" :
						"your distant homeland");
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
	     case 17:	You(Hallucination ?
				"smell hamburgers." :
				"smell charred flesh.");
			break;
	     case 18:	You_feel("tired.");
			break;

	     /* very occasionally something nice happens. */

	     case 19:
		    /* tame nearby monsters */
		   {   register int i,j;
		       register struct monst *mtmp;

		       (void) adjattrib(A_CHA,1,FALSE);
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

	if (Is_container(obj)) {
	    switch (obj->otyp) {
	    case ICE_BOX:
		continue;		/* Immune */
		/*NOTREACHED*/
		break;
	    case CHEST:
		chance = 60;
		break;
	    case LARGE_BOX:
		chance = 55;
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
	    if (obj->otyp == SCR_FIRE || obj->otyp == SPE_FIREBALL || obj->oartifact)
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
	} else if (is_flammable(obj) && obj->oeroded < MAX_ERODE &&
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

			if ( (!rn2(50) || force ) && (!obj->blessed || !rn2(4) ) && !stack_too_big(obj) && obj->otyp != SPE_BOOK_OF_THE_DEAD && obj->otyp != AMULET_OF_YENDOR && obj->otyp != CANDELABRUM_OF_INVOCATION && obj->otyp != BELL_OF_OPENING && obj->oartifact != ART_KEY_OF_LAW && obj->oartifact != ART_KEY_OF_CHAOS && obj->oartifact != ART_KEY_OF_NEUTRALITY   ) { /* 2% chance for each item to be affected, blessed ones are only affected with 0.5% chance --Amy */

				if (rn2(2)) {

					if (obj->oeroded < MAX_ERODE && !((obj->blessed && !rnl(4)))) obj->oeroded++;
					else if (obj->oeroded == MAX_ERODE)
					{
				    
					pline("One of your objects withered away!");
					delobj(obj);
					update_inventory();
			    
					}
				}
				else {

					if (obj->oeroded2 < MAX_ERODE && !((obj->blessed && !rnl(4)))) obj->oeroded2++;
					else if (obj->oeroded2 == MAX_ERODE)
					{
				    
					pline("One of your objects withered away!");
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

		(void) snuff_lit(obj);

		if (stack_too_big(obj)) continue;

		else if(obj->otyp == CAN_OF_GREASE && obj->spe > 0) {
			continue;
		} else if(obj->greased) {
			if (force || !rn2(2)) obj->greased = 0;
		} else if(Is_container(obj) && !Is_box(obj) &&
			(obj->otyp != OILSKIN_SACK || (obj->cursed && !rn2(3)))) {
			water_damage(obj->cobj, force, FALSE);
		} else if (!force && (Luck - luckpenalty + 5 + rnd(20) ) > rn2(20)) {
			/*  chance per item of sustaining damage:
			 *	max luck (full moon):	 5%
			 *	max luck (elsewhen):	10%
			 *	avg luck (Luck==0):	75%
			 *	awful luck (Luck<-4):  100%
			 *  If this is the Lethe, things are much worse.
			 *  Amy edit: extra rnd(20) boost for the chance, so a single misstep won't necessarily blank everything
			 */
			continue;
		} else if (obj->blessed && rn2(4) ) { /* blessed items get an extra saving throw --Amy */
			continue;
		/* An oil skin cloak protects your body armor  */
		} else if( obj->oclass == ARMOR_CLASS
			&& obj == uarm
			&& uarmc
			&& uarmc->otyp == OILSKIN_CLOAK
			&& (!uarmc->cursed || rn2(3))) {
		    continue;
		} else {
		    /* The Lethe strips blessed and cursed status... */
		    if (level.flags.lethe) {
			uncurse(obj);
			unbless(obj);
		    }

		    switch (obj->oclass) {
		    case SCROLL_CLASS:
			if  (obj->otyp != SCR_HEALING && obj->otyp != SCR_STANDARD_ID && obj->otyp != SCR_MANA && obj->otyp != SCR_CURE && obj->otyp != SCR_PHASE_DOOR
#ifdef MAIL
		    && obj->otyp != SCR_MAIL
#endif
			)
		    {
			    /* The Lethe sometimes does a little rewrite */
			    obj->otyp = (level.flags.lethe && !rn2(10)) ?
					SCR_AMNESIA : SCR_BLANK_PAPER;
			obj->spe = 0;
		    }
			break;
		    case SPBOOK_CLASS:
			/* Spell books get blanked... */
			if (obj->otyp == SPE_BOOK_OF_THE_DEAD)
				pline("Steam rises from %s.", the(xname(obj)));
			else obj->otyp = SPE_BLANK_PAPER;
			break;
		    case POTION_CLASS:
			if (obj->otyp == POT_ACID) {
				/* damage player/monster? */
				pline("A potion explodes!");
				delobj(obj);
				continue;
			} else
			/* Potions turn to water or amnesia... */
			if (level.flags.lethe) {
			    if (obj->otyp == POT_WATER)
				obj->otyp = POT_AMNESIA;
			    else if (obj->otyp != POT_AMNESIA) {
				obj->otyp = POT_WATER;
				obj->odiluted = 0;
			    }
			} else if (obj->odiluted || obj->otyp == POT_AMNESIA) {
				obj->otyp = POT_WATER;
				obj->blessed = obj->cursed = obj->hvycurse = obj->prmcurse = 0;
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
					|| obj->otyp == STONE_OF_MAGIC_RESISTANCE
					|| obj->otyp == TOUCHSTONE))
			    obj->otyp = FLINT;
			break;
		    case TOOL_CLASS:
			if (level.flags.lethe) {
			    switch (obj->otyp) {
			    case MAGIC_LAMP:
				obj->otyp = OIL_LAMP;
				break;
			    case MAGIC_CANDLE:
				obj->otyp = rn2(2)? WAX_CANDLE : TALLOW_CANDLE;
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
			if (is_rustprone(obj) && obj->oeroded < MAX_ERODE &&
					!(obj->oerodeproof || 
					 (obj->blessed && !rnl(4))))
				obj->oeroded++;
			else if (is_rustprone(obj) && obj->oeroded == MAX_ERODE &&
					!(obj->oerodeproof ))
			{
			    
				pline("One of your objects was destroyed by rust!");
				if (obj == uball) unpunish;
				if (obj == uchain) unpunish;
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

		/* important quest items are immune */
		if (obj->otyp == SPE_BOOK_OF_THE_DEAD || obj->otyp == AMULET_OF_YENDOR || obj->otyp == CANDELABRUM_OF_INVOCATION || obj->otyp == BELL_OF_OPENING || obj->oartifact == ART_KEY_OF_LAW || obj->oartifact == ART_KEY_OF_NEUTRALITY || obj->oartifact == ART_KEY_OF_CHAOS) continue;

			if (obj->oeroded < MAX_ERODE && !( (obj->blessed && !rnl(4))))
				obj->oeroded++;
			else if (obj->oeroded == MAX_ERODE)
			{
			    
				pline("One of your objects withered away!");
				delobj(obj);
				update_inventory();
			    
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
		     * for obvious reasons.
		     */
		    if (!(( (obj->otyp == LOADSTONE || obj->otyp == LUCKSTONE || obj->otyp == HEALTHSTONE || obj->otyp == MANASTONE || obj->otyp == SLEEPSTONE || obj->otyp == LOADBOULDER || obj->otyp == STONE_OF_MAGIC_RESISTANCE || is_nastygraystone(obj) ) && obj->cursed) ||
			  obj == uamul || obj == uleft || obj == uright ||
			  obj == ublindf || obj == uarm || obj == uarmc ||
			  obj == uarmg || obj == uarmf ||
#ifdef TOURIST
			  obj == uarmu ||
#endif
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

	/* happily wading in the same contiguous pool */
	if (u.uinwater && is_pool(u.ux-u.dx,u.uy-u.dy) &&
	    (Swimming || Amphibious)) {
		/* water effects on objects every now and then */
		if (!rn2(5)) inpool_ok = TRUE;
		else return(FALSE);
	}

	if (!u.uinwater) {
	    You("%s into the %swater%c",
		Is_waterlevel(&u.uz) ? "plunge" : "fall",
		sparkle,
		Amphibious || Swimming ? '.' : '!');
	    if (!Swimming && !Is_waterlevel(&u.uz))
		    You("sink like %s.",
			Hallucination ? "the Titanic" : "a rock");
	}

	if (level.flags.lethe) {
	    /* Bad idea */
	    You_feel("the sparkling waters of the Lethe sweep away your "
			    "cares!");
	    forget(5); /* used to be 25 --Amy */
	}

	water_damage(invent, FALSE, FALSE);
	if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
	if (Burned) make_burned(0L, TRUE);

	if (u.umonnum == PM_GREMLIN && rn2(3))
	    (void)split_mon(&youmonst, (struct monst *)0);
	else if (u.umonnum == PM_IRON_GOLEM) {
	    You("rust!");
	    i = d(2,6);
	    if (u.mhmax > i) u.mhmax -= i;
	    losehp(i, "rusting away", KILLED_BY);
	}
	if (inpool_ok) return(FALSE);

	if ((i = number_leashed()) > 0) {
		pline_The("leash%s slip%s loose.",
			(i > 1) ? "es" : "",
			(i > 1) ? "" : "s");
		unleash_all();
	}

	if (Amphibious || Swimming) {
		if (Amphibious) {
			if (flags.verbose)
				pline("But you aren't drowning.");
			if (!Is_waterlevel(&u.uz)) {
				if (Hallucination)
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
		under_water(1);
		vision_full_recalc = 1;
		return(FALSE);
	}
	else if (Swimming && !Is_waterlevel(&u.uz)) {
		if (Punished) {
			unplacebc();
			placebc();
		}
		u.uinwater = 1;
		under_water(1);
		vision_full_recalc = 1;
		return(FALSE);
	}
	if ((Teleportation || can_teleport(youmonst.data)) &&
		    !u.usleep && (Teleport_control || rn2(3) < Luck+2)) {
		You("attempt a teleport spell.");	/* utcsri!carroll */
		if (!level.flags.noteleport) {
			(void) dotele();
			if(!is_pool(u.ux,u.uy))
				return(TRUE);
		} else pline_The("attempted teleport spell fails.");
	}
#ifdef STEED
	if (u.usteed) {
		dismount_steed(DISMOUNT_GENERIC);
		if(!is_pool(u.ux,u.uy))
			return(TRUE);
	}
#endif
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
		if (goodpos(x, y, &youmonst, 0)) {
			crawl_ok = TRUE;
			goto crawl;
		}
	}
	/* one more scan */
	for (x = u.ux - 1; x <= u.ux + 1; x++)
		for (y = u.uy - 1; y <= u.uy + 1; y++)
			if (goodpos(x, y, &youmonst, 0)) {
				crawl_ok = TRUE;
				goto crawl;
			}
 crawl:
	if (crawl_ok) {
		boolean lost = FALSE;
		/* time to do some strip-tease... */
		boolean succ = Is_waterlevel(&u.uz) ? TRUE :
				emergency_disrobe(&lost);

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
	You("drown.");
	/* [ALI] Vampires return to vampiric form on drowning.
	 */
	if (Upolyd && !Unchanging && Race_if(PM_VAMPIRE)) {
		rehumanize();
		u.uinwater = 0;
		You("fly up out of the water!");
		return (TRUE);
	}
	killer_format = KILLED_BY_AN;
	killer = (levl[u.ux][u.uy].typ == POOL || Is_medusa_level(&u.uz)) ?
	    "pool of water" : "moat";
	done(DROWNING);
	/* oops, we're still alive.  better get out of the water. */
	while (!safe_teleds(TRUE)) {
		pline("You're still drowning.");
		done(DROWNING);
	}
	if (u.uinwater) {
	u.uinwater = 0;
	You("find yourself back %s.", Is_waterlevel(&u.uz) ?
		"in an air bubble" : "on land");
	}
	return(TRUE);
	
}

void
drain_en(n)
register int n;
{
	int maxenloss;

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
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    return 0;
	} else if (u.ustuck && sticks(youmonst.data)) {
	    pline("You'll have to let go of %s first.", mon_nam(u.ustuck));
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    return 0;
	}
	if (u.ustuck || (welded(uwep) && bimanual(uwep))) {
	    Your("%s seem to be too busy for that.",
		 makeplural(body_part(HAND)));
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    return 0;
	}
	return untrap(FALSE);
}
#endif /* OVLB */
#ifdef OVL2

/* Probability of disabling a trap.  Helge Hafting */
STATIC_OVL int
untrap_prob(ttmp)
struct trap *ttmp;
{
	int chance = 3;

	/* Only spiders know how to deal with webs reliably */
	if (ttmp->ttyp == WEB && !webmaker(youmonst.data) && !Race_if(PM_SPIDERMAN) )
	 	chance = 30;
	if (ttmp->ttyp == ACTIVE_SUPERSCROLLER_TRAP) chance = 20;
	if (ttmp->ttyp == HEEL_TRAP) chance = 10;
	if (ttmp->ttyp == GLYPH_OF_WARDING) chance = 5;
	if (ttmp->ttyp == UNKNOWN_TRAP) chance = 5;
	if (ttmp->ttyp == SCYTHING_BLADE) chance = 4;

	if (ttmp->ttyp == FART_TRAP) chance = (ttmp->launch_otyp < 11) ? 5 : (ttmp->launch_otyp < 29) ? 10 : 20;

	if (Confusion || Hallucination) chance++;
	if (Blind) chance++;
	if (Numbed) chance++;
	if (Stunned) chance += 2;
	if (Feared) chance += 2;
	if (Fumbling) chance *= 2;
	/* Your own traps are better known than others. */
	if (ttmp && ttmp->madeby_u) chance--;
	if (Role_if(PM_ROGUE)) {
	    if (rn2(2 * MAXULEV) < u.ulevel) chance--;
	    if (u.uhave.questart && chance > 1) chance--;
	} else if (Role_if(PM_RANGER) && chance > 1) chance--;
	return rn2(chance);
}

/* Replace trap with object(s).  Helge Hafting */
STATIC_OVL void
cnv_trap_obj(otyp, cnt, ttmp)
int otyp;
int cnt;
struct trap *ttmp;
{
	struct obj *otmp = mksobj(otyp, TRUE, FALSE);
	/* [ALI] Only dart traps are capable of being poisonous */
	if (otyp != DART)
	    otmp->opoisoned = 0;
	otmp->quan=cnt;
	otmp->owt = weight(otmp);
	/* Only dart traps are capable of being poisonous */
	if (otyp != DART)
	    otmp->opoisoned = 0;
	place_object(otmp, ttmp->tx, ttmp->ty);
	/* Sell your own traps only... */
	if (ttmp->madeby_u) sellobj(otmp, ttmp->tx, ttmp->ty);
	stackobj(otmp);
	newsym(ttmp->tx, ttmp->ty);
	deltrap(ttmp);
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
try_disarm(ttmp, force_failure)
struct trap *ttmp;
boolean force_failure;
{
	struct monst *mtmp = m_at(ttmp->tx,ttmp->ty);
	int ttype = ttmp->ttyp;
	boolean under_u = (!u.dx && !u.dy);
	boolean holdingtrap = (ttype == BEAR_TRAP || ttype == WEB);

#ifdef JEDI
	if (tech_inuse(T_TELEKINESIS) && !force_failure)
		return 2;
#endif
	
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
#ifdef STEED
		if (u.usteed && P_SKILL(P_RIDING) < P_BASIC)
			You("aren't skilled enough to reach from %s.",
				mon_nam(u.usteed));
		else
#endif
		You("are unable to reach the %s!",
			defsyms[trap_to_defsym(ttype)].explanation);
		return 0;
	}

	/* Will our hero succeed? */
	if (force_failure || untrap_prob(ttmp)) {
		if (rnl(5)) {
		    pline("Whoops...");
		    if (mtmp) {		/* must be a trap that holds monsters */
			if (ttype == BEAR_TRAP) {
			    if (mtmp->mtame) abuse_dog(mtmp);
			    if ((mtmp->mhp -= rnd(4)) <= 0) killed(mtmp);
			} else if (ttype == WEB) {
			    if (!webmaker(youmonst.data)) {
				struct trap *ttmp2 = maketrap(u.ux, u.uy, WEB);
				if (ttmp2) {
				    pline_The("webbing sticks to you. You're caught too!");
				    dotrap(ttmp2, NOWEBMSG);
#ifdef STEED
				    if (u.usteed && u.utrap) {
					/* you, not steed, are trapped */
					dismount_steed(DISMOUNT_FELL);
				    }
#endif
				}
			    } else
				pline("%s remains entangled.", Monnam(mtmp));
			}
		    } else if (under_u) {
			dotrap(ttmp, 0);
		    } else {
			move_into_trap(ttmp);
		    }
		} else {
		    pline("%s %s is difficult to %s.",
			  ttmp->madeby_u ? "Your" : under_u ? "This" : "That",
			  defsyms[trap_to_defsym(ttype)].explanation,
			  (ttype == WEB) ? "remove" : "disarm");
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
		mtmp->mpeaceful = 1;
		set_malign(mtmp);	/* reset alignment */
		pline("%s is grateful.", Monnam(mtmp));
	    }
	    /* Helping someone out of a trap is a nice thing to do,
	     * A lawful may be rewarded, but not too often.  */
	    if (!rn2(3) && !rnl(8) && u.ualign.type == A_LAWFUL) {
		adjalign(1);
		You_feel("that you did the right thing.");
	    }
	}
}

int
disarm_holdingtrap(ttmp) /* Helge Hafting */
struct trap *ttmp;
{
	struct monst *mtmp;
	int fails = try_disarm(ttmp, FALSE);

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
	} else {
		if (ttmp->ttyp == BEAR_TRAP) {
			You("disarm %s bear trap.", the_your[ttmp->madeby_u]);
			cnv_trap_obj(BEARTRAP, 1, ttmp);
		} else /* if (ttmp->ttyp == WEB) */ {
			You("succeed in removing %s web.", the_your[ttmp->madeby_u]);
			deltrap(ttmp);
		}
	}
	newsym(u.ux + u.dx, u.uy + u.dy);
	return 1;
}

int
disarm_landmine(ttmp) /* Helge Hafting */
struct trap *ttmp;
{
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;
	You("disarm %s land mine.", the_your[ttmp->madeby_u]);
	cnv_trap_obj(LAND_MINE, 1, ttmp);
	return 1;
}

int
disarm_rust_trap(ttmp) /* Paul Sonier */
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;
	You("disarm the water trap!");
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
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	    more_experienced(exper,0);
		    newexplevel();
	deltrap(ttmp);
	newsym(trapx, trapy);
	return 1;
}

int
disarm_blade_trap(ttmp)
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	    more_experienced(3,0);
		    newexplevel();
	deltrap(ttmp);
	newsym(trapx, trapy);
	return 1;
}

int
disarm_acid_trap(ttmp)
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	    more_experienced(3,0);
		    newexplevel();
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
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	    more_experienced(1,0);
		    newexplevel();
	deltrap(ttmp);
	newsym(trapx, trapy);
	return 1;
}

int
disarm_active_superscroller(ttmp)
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;
	You("removed the superscroller!");
	    more_experienced(1,0);
		    newexplevel();
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
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	    more_experienced(5,0);
		    newexplevel();

	unkrwrd = mkobj(RANDOM_CLASS, FALSE); dropy(unkrwrd );

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
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;

	pline("You bash %s's sexy butt.", farttrapnames[ttmp->launch_otyp]);
	diceroll = rnd(30);

	if (ttmp->launch_otyp < 11) diceroll -= rnd(diceroll);
	else if (!rn2(2) && ttmp->launch_otyp < 29) diceroll -= rnd(diceroll);

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
			pline("You hurt %s so badly that she retreats her sexy butt, and decides to set up her high heels as a trap instead!", farttrapnames[ttmp->launch_otyp]);
			deltrap(ttmp);
			ttmp = maketrap(trapx, trapy, HEEL_TRAP);
			if (ttmp && !ttmp->hiddentrap ) ttmp->tseen = 1;
			newsym(trapx, trapy);
			return 1;

	}

}

int
disarm_water_trap(ttmp)
struct trap *ttmp;
{
	xchar trapx = ttmp->tx, trapy = ttmp->ty;
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	    more_experienced(3,0);
		    newexplevel();
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
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;
	You("disarm the trap!");
	    more_experienced(20,0);
		    newexplevel();

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
	fails = try_disarm(ttmp, bad_tool);
	if (fails < 2) return fails;

	useup(obj);
	makeknown(POT_WATER);
	You("manage to extinguish the pilot light!");

	/* Lower chance of getting oil. The deltrap function may NOT be called if cnv_trap_obj is also called
	 * or the game will segfault due to dereferencing the trap twice! --Amy */

	if (!rn2(3)) cnv_trap_obj(POT_OIL, 4 - rnl(4), ttmp);
	else deltrap(ttmp);
	more_experienced(1, 5);
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
			 (obj->otyp != CAN_OF_GREASE || !obj->spe)));

	fails = try_disarm(ttmp, bad_tool);
	if (fails < 2) return fails;

	/* successfully used oil or grease to fix squeaky board */
	if (obj->otyp == CAN_OF_GREASE) {
	    consume_obj_charge(obj, TRUE);
	} else {
	    useup(obj);	/* oil */
	    makeknown(POT_OIL);
	}
	You("repair the squeaky board.");	/* no madeby_u */
	deltrap(ttmp);
	newsym(trapx, trapy);
	more_experienced(1, 5);
		    newexplevel();
	return 1;
}

/* removes traps that shoot arrows, darts, etc. */
int
disarm_shooting_trap(ttmp, otyp)
struct trap *ttmp;
int otyp;
{
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;
	You("disarm %s trap.", the_your[ttmp->madeby_u]);
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
		mtmp->mpeaceful = 1;
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
	if (touch_petrifies(mtmp->data) && !uarmg && !Stone_resistance) {
		You("grab the trapped %s using your bare %s.",
				mtmp->data->mname, makeplural(body_part(HAND)));

		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
			display_nhwindow(WIN_MESSAGE, FALSE);
		else {
			char kbuf[BUFSZ];

			Sprintf(kbuf, "trying to help %s out of a pit",
					an(mtmp->data->mname));
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
		Strcpy(the_trap, the(defsyms[trap_to_defsym(ttmp->ttyp)].explanation));
		if (box_here) {
			if (ttmp->ttyp == PIT || ttmp->ttyp == SPIKED_PIT || ttmp->ttyp == GIANT_CHASM || ttmp->ttyp == SHIT_PIT) {
			    You_cant("do much about %s%s.",
					the_trap, u.utrap ?
					" that you're stuck in" :
					" while standing on the edge of it");
			    trap_skipped = TRUE;
			    deal_with_floor_trap = FALSE;
			} else {
			    Sprintf(qbuf, "There %s and %s here. %s %s?",
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
				return disarm_holdingtrap(ttmp);
			case LANDMINE:
				return disarm_landmine(ttmp);
			case SQKY_BOARD:
				return disarm_squeaky_board(ttmp);
			case DART_TRAP:
				return disarm_shooting_trap(ttmp, DART);
			case THROWING_STAR_TRAP:
				return disarm_shooting_trap(ttmp, SHURIKEN);
			case HEEL_TRAP:
				return disarm_heel_trap(ttmp);
			case ARROW_TRAP:
				return disarm_shooting_trap(ttmp, ARROW);
			case BOLT_TRAP:
				return disarm_shooting_trap(ttmp, CROSSBOW_BOLT);
			case BULLET_TRAP:
				return disarm_shooting_trap(ttmp, BULLET);
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
			case FIRE_TRAP:
				return disarm_fire_trap(ttmp);
			case PIT:
			case SPIKED_PIT:
			case GIANT_CHASM:
			case SHIT_PIT:
				if (!u.dx && !u.dy) {
				    You("are already on the edge of the pit.");
				    return 0;
				}
				if (!(mtmp = m_at(x,y))) {
				    pline("Try filling the pit instead.");
				    return 0;
				}
				return help_monster_out(mtmp, ttmp);
			default:
				You("cannot disable %s trap.", (u.dx || u.dy) ? "that" : "this");
				return 0;
		    }
		}
	} /* end if */

	if(!u.dx && !u.dy) {
	    for(otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
		if(Is_box(otmp)) {
		    Sprintf(qbuf, "There is %s here. Check it for traps?",
			safe_qbuf("", sizeof("There is  here. Check it for traps?"),
				doname(otmp), an(simple_typename(otmp->otyp)), "a box"));
		    switch (ynq(qbuf)) {
			case 'q': return(0);
			case 'n': continue;
		    }
#ifdef STEED
		    if (u.usteed && P_SKILL(P_RIDING) < P_BASIC) {
			You("aren't skilled enough to reach from %s.",
				mon_nam(u.usteed));
			return(0);
		    }
#endif
		    if((otmp->otrapped && (force || (!confused
				&& rn2(MAXULEV + 1 - u.ulevel) < 10)))
		       || (!force && confused && !rn2(3))) {
			You("find a trap on %s!", the(xname(otmp)));
			if (!confused) exercise(A_WIS, TRUE);

			switch (ynq("Disarm it?")) {
			    case 'q': return(1);
			    case 'n': trap_skipped = TRUE;  continue;
			}

			if(otmp->otrapped) {
			    exercise(A_DEX, TRUE);
			    ch = ACURR(A_DEX) + u.ulevel;
			    if (Role_if(PM_ROGUE)) ch *= 2;
			    if(!force && (confused || Fumbling ||
				rnd(75+level_difficulty()/2) > ch)) {
				(void) chest_trap(otmp, FINGER, TRUE);
			    } else {
				You("disarm it!");
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
	char	buf[80];
	const char *msg;
	coord cc;

	if (get_obj_location(obj, &cc.x, &cc.y, 0))	/* might be carried */
	    obj->ox = cc.x,  obj->oy = cc.y;

	otmp->otrapped = 0;	/* trap is one-shot; clear flag first in case
				   chest kills you and ends up in bones file */
	You(disarm ? "set it off!" : "trigger a trap!");
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
			  Sprintf(buf, "exploding %s", xname(obj));

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
			  if (Punished && !carried(uball) &&
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
			if(Shock_resistance)  {
			    shieldeff(u.ux, u.uy);
			    You("don't seem to be affected.");
			    dmg = 0;
			} else
			    dmg = d(4, 4);
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (!rn2(33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
			if (dmg) losehp(dmg, "electric shock", KILLED_BY_AN);
			break;
		      }
		case 5:
		case 4:
		case 3:
			if (!Free_action) {                        
			if (!Free_action) {                        
			pline("Suddenly you are frozen in place!");
			nomul(-d(5, 6), "frozen by a container trap");
			exercise(A_DEX, FALSE);
			nomovemsg = You_can_move_again;
			} else You("momentarily stiffen.");
			} else You("momentarily stiffen.");                        
			break;
		case 2:
		case 1:
		case 0:
			pline("A cloud of %s gas billows from %s.",
				Blind ? blindgas[rn2(SIZE(blindgas))] :
				rndcolor(), the(xname(obj)));
			if(!Stunned) {
			    if (Hallucination)
				pline("What a groovy feeling!");
			    else if (Blind)
				You("%s and get dizzy...",
				    stagger(youmonst.data, "stagger"));
			    else
				You("%s and your vision blurs...",
				    stagger(youmonst.data, "stagger"));
			}
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
		     (ttmp->ttyp == SHIT_PIT) ||
		     (ttmp->ttyp == TELEP_TRAP) ||
		     (ttmp->ttyp == LEVEL_TELEP) ||
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
	int dmg = rnd(5 + (lvl < 5 ? lvl : 2+lvl/2));

	pline("KABOOM!!  %s was booby-trapped!", The(item));
	wake_nearby();
	losehp(dmg, "explosion", KILLED_BY_AN);
	exercise(A_STR, FALSE);
	if (bodypart) exercise(A_CON, FALSE);
	make_stunned(HStun + dmg, TRUE);
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
	if (obj && (!strike || d_override)) {
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

boolean
lava_effects()
{
    register struct obj *obj, *obj2;
    int dmg;
    boolean usurvive;

    burn_away_slime();
    if (likes_lava(youmonst.data)) return FALSE;


    if (Slimed) {
	pline("The slime boils away!");
	Slimed = 0;
    }

    if (Frozen) {
	pline("The ice thaws!");
	make_frozen(0L, FALSE);
    }

    if (!Fire_resistance) {

	if(Wwalking) {
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
	    if(is_organic(obj) && !obj->oerodeproof && !stack_too_big(obj)) {
		if(obj->owornmask) {
		    if (usurvive)
			Your("%s into flame!", aobjnam(obj, "burst"));

		    if(obj == uarm) (void) Armor_gone();
		    else if(obj == uarmc) (void) Cloak_off();
		    else if(obj == uarmh) (void) Helmet_off();
		    else if(obj == uarms) (void) Shield_off();
		    else if(obj == uarmg) (void) Gloves_off();
		    else if(obj == uarmf) (void) Boots_off();
#ifdef TOURIST
		    else if(obj == uarmu) setnotworn(obj);
#endif
		    else if(obj == uleft) Ring_gone(obj);
		    else if(obj == uright) Ring_gone(obj);
		    else if(obj == ublindf) Blindf_off(obj);
		    else if(obj == uamul) Amulet_off();
		    else if(obj == uwep) uwepgone();
		    else if (obj == uquiver) uqwepgone();
		    else if (obj == uswapwep) uswapwepgone();
		}
		useupall(obj);
	    }
	}

	/* s/he died... */
	u.uhp = -1;
	killer_format = KILLED_BY;
	killer = lava_killer;
	You("burn to a crisp...");
	done(BURNING);
	while (!safe_teleds(TRUE)) {
		pline("You're still burning.");
		done(BURNING);
	}
	You("find yourself back on solid %s.", surface(u.ux, u.uy));
	return(TRUE);
    }

    if (!Wwalking) {
	u.utrap = rn1(4, 4) + (rn1(4, 12) << 8);
	u.utraptype = TT_LAVA;
	You("sink into the lava, but it only burns slightly!");
	if (u.uhp > 1)
	    losehp(1, lava_killer, KILLED_BY);
    }
    /* just want to burn boots, not all armor; destroy_item doesn't work on
       armor anyway */
burn_stuff:
    if(uarmf && !uarmf->oerodeproof && is_organic(uarmf)) {
	/* save uarmf value because Boots_off() sets uarmf to null */
	obj = uarmf;
	Your("%s bursts into flame!", xname(obj));
	(void) Boots_off();
	useup(obj);
    }
		    if (!rn2(15)) /* new calculations --Amy */    destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (!rn2(15)) /* new calculations --Amy */    destroy_item(SPBOOK_CLASS, AD_FIRE);
		    if (!rn2(15)) /* new calculations --Amy */    destroy_item(POTION_CLASS, AD_FIRE);
    return(FALSE);
}

#endif /* OVLB */

/*trap.c*/
