/*	SCCS Id: @(#)mkmaze.c	3.4	2002/04/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "sp_lev.h"
#include "lev.h"	/* save & restore info */

/* from sp_lev.c, for fixup_special() */
extern char *lev_message;
extern lev_region *lregions;
extern int num_lregions;

STATIC_DCL boolean iswall(int,int);
STATIC_DCL boolean iswall_or_stone(int,int);
STATIC_DCL boolean is_solid(int,int);
STATIC_DCL int extend_spine(int [3][3], int, int, int);
STATIC_DCL boolean okay(int,int,int);
STATIC_DCL void maze0xy(coord *);
STATIC_DCL boolean put_lregion_here(XCHAR_P,XCHAR_P,XCHAR_P,
	XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,BOOLEAN_P,d_level *);
STATIC_DCL boolean really_put_lregion_here(XCHAR_P,XCHAR_P,XCHAR_P,
	XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,BOOLEAN_P,d_level *);
STATIC_DCL void fixup_special(void);
STATIC_DCL void move(int *,int *,int);
STATIC_DCL void setup_waterlevel(void);
STATIC_DCL void unsetup_waterlevel(void);

STATIC_OVL boolean
iswall(x,y)
int x,y;
{
    register int type;

    if (!isok(x,y)) return FALSE;
    type = levl[x][y].typ;
    return (IS_WALL(type) || IS_DOOR(type) ||
	    type == SDOOR || type == IRONBARS);
}

STATIC_OVL boolean
iswall_or_stone(x,y)
    int x,y;
{
    register int type;

    /* out of bounds = stone */
    if (!isok(x,y)) return TRUE;

    type = levl[x][y].typ;
    return (type == STONE || IS_WALL(type) || IS_DOOR(type) ||
	    type == SDOOR || type == IRONBARS);
}

/* return TRUE if out of bounds, wall or rock */
STATIC_OVL boolean
is_solid(x,y)
    int x, y;
{
    return (!isok(x,y) || IS_STWALL(levl[x][y].typ));
}


/*
 * Return 1 (not TRUE - we're doing bit vectors here) if we want to extend
 * a wall spine in the (dx,dy) direction.  Return 0 otherwise.
 *
 * To extend a wall spine in that direction, first there must be a wall there.
 * Then, extend a spine unless the current position is surrounded by walls
 * in the direction given by (dx,dy).  E.g. if 'x' is our location, 'W'
 * a wall, '.' a room, 'a' anything (we don't care), and our direction is
 * (0,1) - South or down - then:
 *
 *		a a a
 *		W x W		This would not extend a spine from x down
 *		W W W		(a corridor of walls is formed).
 *
 *		a a a
 *		W x W		This would extend a spine from x down.
 *		. W W
 */
STATIC_OVL int
extend_spine(locale, wall_there, dx, dy)
    int locale[3][3];
    int wall_there, dx, dy;
{
    int spine, nx, ny;

    nx = 1 + dx;
    ny = 1 + dy;

    if (wall_there) {	/* wall in that direction */
	if (dx) {
	    if (locale[ 1][0] && locale[ 1][2] && /* EW are wall/stone */
		locale[nx][0] && locale[nx][2]) { /* diag are wall/stone */
		spine = 0;
	    } else {
		spine = 1;
	    }
	} else {	/* dy */
	    if (locale[0][ 1] && locale[2][ 1] && /* NS are wall/stone */
		locale[0][ny] && locale[2][ny]) { /* diag are wall/stone */
		spine = 0;
	    } else {
		spine = 1;
	    }
	}
    } else {
	spine = 0;
    }

    return spine;
}


/*
 * Walls to surprise jaded Gehennom-haters :)
 *
 * Wall cleanup. This function turns all wall squares into 'floortype' squares.
 */
STATIC_OVL
void
wallify_special(x1, y1, x2, y2, floortype)
int x1, y1, x2, y2;
int floortype;		/* The 'wall' floortype */
{
	uchar type;
	register int x,y;
	struct rm *lev;

	int density = 3;
	if (!rn2(5)) density += 1;
	if (!rn2(10)) density += rnd(3);
	if (!rn2(25)) density += rnd(5);
	if (!rn2(125)) density += rnd(10);
	if (!rn2(750)) density += rnd(20);

	/* sanity check on incoming variables */
	if (x1<0 || x2>=COLNO || x1>x2 || y1<0 || y2>=ROWNO || y1>y2)
	    panic("wallify_fire: bad bounds (%d,%d) to (%d,%d)",x1,y1,x2,y2);

	/* Translate the maze... */
	for(x = x1; x <= x2; x++)
	    for(y = y1; y <= y2; y++) {
		lev = &levl[x][y];
		type = lev->typ;
		if ( IS_WALL(type) && type != GRAVEWALL && (rnd(density) < 4))
		    lev->typ = (floortype == CROSSWALL) ? randomwalltype() : floortype;
		else if IS_WALL(type)
		    lev->typ = STONE;
		/* Doors become room squares. Does this make sense? */
		else if (IS_DOOR(type))
		    lev->typ = ROOM;
		else if (type == SDOOR)
		    lev->typ = ROOM;
		else if (type == SCORR)
		    lev->typ = ROOM;
	    }

	return;
}

/*
 * Wall cleanup.  This function has two purposes: (1) remove walls that
 * are totally surrounded by stone - they are redundant.  (2) correct
 * the types so that they extend and connect to each other.
 */
STATIC_OVL
void
wallify_stone(x1, y1, x2, y2)	/* [Lethe] Classic stone walls */
int x1, y1, x2, y2;
{
	uchar type;
	register int x,y;
	struct rm *lev;
	int bits;
	int locale[3][3];	/* rock or wall status surrounding positions */
	/*
	 * Value 0 represents a free-standing wall.  It could be anything,
	 * so even though this table says VWALL, we actually leave whatever
	 * typ was there alone.
	 */
	static xchar spine_array[16] = {
	    VWALL,	HWALL,		HWALL,		HWALL,
	    VWALL,	TRCORNER,	TLCORNER,	TDWALL,
	    VWALL,	BRCORNER,	BLCORNER,	TUWALL,
	    VWALL,	TLWALL,		TRWALL,		CROSSWALL
	};

	/* sanity check on incoming variables */
	if (x1<0 || x2>=COLNO || x1>x2 || y1<0 || y2>=ROWNO || y1>y2)
	    panic("wallify_stone: bad bounds (%d,%d) to (%d,%d)",x1,y1,x2,y2);

	/* Step 1: change walls surrounded by rock to rock. */
	for(x = x1; x <= x2; x++)
	    for(y = y1; y <= y2; y++) {
		lev = &levl[x][y];
		type = lev->typ;
		if (IS_WALL(type) && type != DBWALL && type != ROCKWALL && type != TUNNELWALL && type != GRAVEWALL) {
		    if (is_solid(x-1,y-1) &&
			is_solid(x-1,y  ) &&
			is_solid(x-1,y+1) &&
			is_solid(x,  y-1) &&
			is_solid(x,  y+1) &&
			is_solid(x+1,y-1) &&
			is_solid(x+1,y  ) &&
			is_solid(x+1,y+1))
		    lev->typ = STONE;
		}
	    }

	/*
	 * Step 2: set the correct wall type.  We can't combine steps
	 * 1 and 2 into a single sweep because we depend on knowing if
	 * the surrounding positions are stone.
	 */
	for(x = x1; x <= x2; x++)
	    for(y = y1; y <= y2; y++) {
		lev = &levl[x][y];
		type = lev->typ;
		if ( !(IS_WALL(type) && type != DBWALL && type != ROCKWALL && type != TUNNELWALL && type != GRAVEWALL)) continue;

		/* set the locations TRUE if rock or wall or out of bounds */
		locale[0][0] = iswall_or_stone(x-1,y-1);
		locale[1][0] = iswall_or_stone(  x,y-1);
		locale[2][0] = iswall_or_stone(x+1,y-1);

		locale[0][1] = iswall_or_stone(x-1,  y);
		locale[2][1] = iswall_or_stone(x+1,  y);

		locale[0][2] = iswall_or_stone(x-1,y+1);
		locale[1][2] = iswall_or_stone(  x,y+1);
		locale[2][2] = iswall_or_stone(x+1,y+1);

		/* determine if wall should extend to each direction NSEW */
		bits =    (extend_spine(locale, iswall(x,y-1),  0, -1) << 3)
			| (extend_spine(locale, iswall(x,y+1),  0,  1) << 2)
			| (extend_spine(locale, iswall(x+1,y),  1,  0) << 1)
			|  extend_spine(locale, iswall(x-1,y), -1,  0);

		/* don't change typ if wall is free-standing */
		if (bits) lev->typ = spine_array[bits];
	    }
}

/*
 * Wall cleanup.  This selects an appropriate function to sort out the 
 * dungeon walls.                                                    
 */
void
wallification(x1, y1, x2, y2, initial)
int x1, y1, x2, y2;
boolean initial;
{
	/* Wallify normally unless creating a full maze level */
	if (!initial) {
		wallify_stone(x1, y1, x2, y2);
		return;
	}

	/* Put in the walls... */
	{
	    int wallchoice = rn2(250);

	    if (wallchoice < 217)
		wallify_stone(x1, y1, x2, y2);
	    else if (wallchoice < 218)
		wallify_special(x1, y1, x2, y2, CROSSWALL);
	    else if (wallchoice < 219)
		wallify_special(x1, y1, x2, y2, CLOUD);
	    else if (wallchoice < 221)
		wallify_special(x1, y1, x2, y2, MOAT);
	    else if (wallchoice < 223)
		wallify_special(x1, y1, x2, y2, ICE);
	    else if (wallchoice < 225)
		wallify_special(x1, y1, x2, y2, LAVAPOOL);
	    else if (wallchoice < 226)
		wallify_special(x1, y1, x2, y2, TREE);
	    else if (wallchoice < 227)
		wallify_special(x1, y1, x2, y2, GRAVEWALL);
	    else if (wallchoice < 228)
		wallify_special(x1, y1, x2, y2, TUNNELWALL);
	    else if (wallchoice < 229)
		wallify_special(x1, y1, x2, y2, FARMLAND);
	    else if (wallchoice < 230)
		wallify_special(x1, y1, x2, y2, MOUNTAIN);
	    else if (wallchoice < 231)
		wallify_special(x1, y1, x2, y2, WATERTUNNEL);
	    else if (wallchoice < 232)
		wallify_special(x1, y1, x2, y2, CRYSTALWATER);
	    else if (wallchoice < 233)
		wallify_special(x1, y1, x2, y2, MOORLAND);
	    else if (wallchoice < 234)
		wallify_special(x1, y1, x2, y2, URINELAKE);
	    else if (wallchoice < 235)
		wallify_special(x1, y1, x2, y2, SHIFTINGSAND);
	    else if (wallchoice < 236)
		wallify_special(x1, y1, x2, y2, STYXRIVER);
	    else if (wallchoice < 237)
		wallify_special(x1, y1, x2, y2, SNOW);
	    else if (wallchoice < 238)
		wallify_special(x1, y1, x2, y2, ASH);
	    else if (wallchoice < 239)
		wallify_special(x1, y1, x2, y2, SAND);
	    else if (wallchoice < 240)
		wallify_special(x1, y1, x2, y2, PAVEDFLOOR);
	    else if (wallchoice < 241)
		wallify_special(x1, y1, x2, y2, HIGHWAY);
	    else if (wallchoice < 242)
		wallify_special(x1, y1, x2, y2, GRASSLAND);
	    else if (wallchoice < 243)
		wallify_special(x1, y1, x2, y2, NETHERMIST);
	    else if (wallchoice < 244)
		wallify_special(x1, y1, x2, y2, STALACTITE);
	    else if (wallchoice < 245)
		wallify_special(x1, y1, x2, y2, CRYPTFLOOR);
	    else if (wallchoice < 246)
		wallify_special(x1, y1, x2, y2, BUBBLES);
	    else if (wallchoice < 247)
		wallify_special(x1, y1, x2, y2, RAINCLOUD);
	    else
		wallify_special(x1, y1, x2, y2, IRONBARS);
	}
	return;
}

STATIC_OVL boolean
okay(x,y,dir)
int x,y;
register int dir;
{
	move(&x,&y,dir);
	move(&x,&y,dir);
	if(x<3 || y<3 || x>x_maze_max || y>y_maze_max || levl[x][y].typ != 0)
		return(FALSE);
	return(TRUE);
}

STATIC_OVL void
maze0xy(cc)	/* find random starting point for maze generation */
	coord	*cc;
{
	cc->x = 3 + 2*rn2((x_maze_max>>1) - 1);
	cc->y = 3 + 2*rn2((y_maze_max>>1) - 1);
	return;
}

/*
 * Bad if:
 *	pos is occupied OR
 *	pos is inside restricted region (lx,ly,hx,hy) OR
 *	NOT (pos is corridor and a maze level OR pos is a room OR pos is air)
 */
boolean
bad_location(x, y, lx, ly, hx, hy)
    xchar x, y;
    xchar lx, ly, hx, hy;
{
    return((boolean)(occupied(x, y) ||
	   within_bounded_area(x,y, lx,ly, hx,hy) ||
	   !((levl[x][y].typ == CORR /*&& level.flags.is_maze_lev*/) ||
	       levl[x][y].typ == ROOM || levl[x][y].typ == AIR || levl[x][y].typ == CLOUD || levl[x][y].typ == ICE || levl[x][y].typ == SNOW || levl[x][y].typ == ASH || levl[x][y].typ == SAND || levl[x][y].typ == PAVEDFLOOR || levl[x][y].typ == HIGHWAY || levl[x][y].typ == GRASSLAND || levl[x][y].typ == NETHERMIST || levl[x][y].typ == STALACTITE || levl[x][y].typ == CRYPTFLOOR || levl[x][y].typ == BUBBLES || levl[x][y].typ == RAINCLOUD)));
}

boolean
really_bad_location(x, y, lx, ly, hx, hy)
    xchar x, y;
    xchar lx, ly, hx, hy;
{
    return((boolean) within_bounded_area(x,y, lx,ly, hx,hy) );
}

/* pick a location in area (lx, ly, hx, hy) but not in (nlx, nly, nhx, nhy) */
/* and place something (based on rtype) in that region */
void
place_lregion(lx, ly, hx, hy, nlx, nly, nhx, nhy, rtype, lev)
    xchar	lx, ly, hx, hy;
    xchar	nlx, nly, nhx, nhy;
    xchar	rtype;
    d_level	*lev;
{
    int trycnt;
    boolean oneshot;
    xchar x, y;

    if(!lx) { /* default to whole level */
	/*
	 * if there are rooms and this a branch, let place_branch choose
	 * the branch location (to avoid putting branches in corridors).
	 * Amy edit: but only if it's a plot-critical branch that you must find in order to win the game, and even then
	 * only if you're not playing the evil variant. Otherwise, let them be randomly placed.
	 * Note that it's not a bug that the sanctum-to-yendortower portal is exempt - otherwise, we'd always place it
	 * either in the temple or the graveyard, which we definitely don't want!
	 * Also, the flipside of those branches is free to be placed completely randomly, because you (normally) don't
	 * have to look for it - to any would-be slex modders reading this, I only want to avoid the player having to search
	 * the entirety of every level for branches that absolutely MUST be done to win the game.
	 * Stairseeker hybrid race should also allow them all to be placed randomly, like evilvariant mode
	 */
	if(rtype == LR_BRANCH && nroom) {

		if (!evilfriday && !isstairseeker && (at_dgn_entrance("Green Cross") || at_dgn_entrance("The Subquest") || at_dgn_entrance("The Quest") || at_dgn_entrance("Lawful Quest") || at_dgn_entrance("Neutral Quest") || at_dgn_entrance("Chaotic Quest") || at_dgn_entrance("The Elemental Planes") || at_dgn_entrance("Sheol") || at_dgn_entrance("Bell Caves") || at_dgn_entrance("Vlad's Tower") || at_dgn_entrance("Forging Chamber") || at_dgn_entrance("Dead Grounds") || at_dgn_entrance("Ordered Chaos") ) ) {

			place_branch(Is_branchlev(&u.uz), 0, 0);
			return;

		}

	}

	lx = 1; hx = COLNO-1;
	ly = 1; hy = ROWNO-1;
    }

    /* first a probabilistic approach */

    oneshot = (lx == hx && ly == hy);
    for (trycnt = 0; trycnt < 200; trycnt++) {
	x = rn1((hx - lx) + 1, lx);
	y = rn1((hy - ly) + 1, ly);
	if (put_lregion_here(x,y,nlx,nly,nhx,nhy,rtype,oneshot,lev))
	    return;
    }

    /* then a deterministic one */

    oneshot = TRUE;
    for (x = lx; x <= hx; x++)
	for (y = ly; y <= hy; y++)
	    if (put_lregion_here(x,y,nlx,nly,nhx,nhy,rtype,oneshot,lev))
		return;

    /* and finally, brute force the shit out of it because we're making a motherfucking BRANCH and the game might be
     * rendered unwinnable if it cannot be placed! --Amy */

    if (wizard) {
	pline("Trying to force a location for lregion type %d", rtype);
    }

    oneshot = TRUE;
    for (x = lx; x <= hx; x++)
	for (y = ly; y <= hy; y++)
	    if (really_put_lregion_here(x,y,nlx,nly,nhx,nhy,rtype,oneshot,lev))
		return;

    impossible("Couldn't place lregion type %d!", rtype);
}

STATIC_OVL boolean
put_lregion_here(x,y,nlx,nly,nhx,nhy,rtype,oneshot,lev)
xchar x, y;
xchar nlx, nly, nhx, nhy;
xchar rtype;
boolean oneshot;
d_level *lev;
{
    if (bad_location(x, y, nlx, nly, nhx, nhy)) {
	if (!oneshot) {
	    return FALSE;		/* caller should try again */
	} else {
	    /* Must make do with the only location possible;
	       avoid failure due to a misplaced trap.
	       It might still fail if there's a dungeon feature here. */
	    struct trap *t = t_at(x,y);

	    if (t && t->ttyp != MAGIC_PORTAL) deltrap(t);
	    if (bad_location(x, y, nlx, nly, nhx, nhy)) return FALSE;
	}
    }
    switch (rtype) {
    case LR_TELE:
    case LR_UPTELE:
    case LR_DOWNTELE:
	/* "something" means the player in this case */
	if(MON_AT(x, y)) {
	    /* move the monster if no choice, or just try again */
	    if(oneshot) (void) rloc(m_at(x,y), FALSE);
	    else return(FALSE);
	}
	u_on_newpos(x, y);
	break;
    case LR_PORTAL:
	mkportal(x, y, lev->dnum, lev->dlevel);
	break;
    case LR_DOWNSTAIR:
    case LR_UPSTAIR:
	mkstairs(x, y, (char)rtype, (struct mkroom *)0);
	break;
    case LR_BRANCH:
	place_branch(Is_branchlev(&u.uz), x, y);
	break;
    }
    return(TRUE);
}

STATIC_OVL boolean
really_put_lregion_here(x,y,nlx,nly,nhx,nhy,rtype,oneshot,lev)
xchar x, y;
xchar nlx, nly, nhx, nhy;
xchar rtype;
boolean oneshot;
d_level *lev;
{
    if (really_bad_location(x, y, nlx, nly, nhx, nhy)) {
	if (!oneshot) {
	    return FALSE;		/* caller should try again */
	} else {
	    /* Must make do with the only location possible;
	       avoid failure due to a misplaced trap.
	       It may not fail, no matter what happens. Missing ladder + lost soul mode == unwinnable game! --Amy */
	    struct trap *t = t_at(x,y);

	    if (t && t->ttyp != MAGIC_PORTAL) deltrap(t);
	    if (really_bad_location(x, y, nlx, nly, nhx, nhy)) return FALSE;
	}
    }
    switch (rtype) {
    case LR_TELE:
    case LR_UPTELE:
    case LR_DOWNTELE:
	/* "something" means the player in this case */
	if(MON_AT(x, y)) {
	    /* move the monster if no choice, or just try again */
	    if(oneshot) (void) rloc(m_at(x,y), FALSE);
	    else return(FALSE);
	}
	u_on_newpos(x, y);
	break;
    case LR_PORTAL:
	mkportal(x, y, lev->dnum, lev->dlevel);
	break;
    case LR_DOWNSTAIR:
    case LR_UPSTAIR:
	mkstairs(x, y, (char)rtype, (struct mkroom *)0);
	break;
    case LR_BRANCH:
	place_branch(Is_branchlev(&u.uz), x, y);
	break;
    }
    return(TRUE);
}

static boolean was_waterlevel; /* ugh... this shouldn't be needed */

/* this is special stuff that the level compiler cannot (yet) handle */
STATIC_OVL void
fixup_special()
{
    register lev_region *r = lregions;
    struct d_level lev;
    register int x, y;
    struct mkroom *croom;
    boolean added_branch = FALSE;

    if (was_waterlevel) {
	was_waterlevel = FALSE;
	u.uinwater = 0;
	unsetup_waterlevel();
    } else if (Is_waterlevel(&u.uz)) {
	level.flags.hero_memory = 0;
	was_waterlevel = TRUE;
	/* water level is an odd beast - it has to be set up
	   before calling place_lregions etc. */
	setup_waterlevel();
    }
    for(x = 0; x < num_lregions; x++, r++) {
	switch(r->rtype) {
	case LR_BRANCH:
	    added_branch = TRUE;
	    goto place_it;

	case LR_PORTAL:
	    if(*r->rname.str >= '0' && *r->rname.str <= '9') {
		/* "chutes and ladders" */
		lev = u.uz;
		lev.dlevel = atoi(r->rname.str);
	    } else {
		s_level *sp = find_level(r->rname.str);
		lev = sp->dlevel;
	    }
	    /* fall into... */

	case LR_UPSTAIR:
	case LR_DOWNSTAIR:
	place_it:
	    place_lregion(r->inarea.x1, r->inarea.y1,
			  r->inarea.x2, r->inarea.y2,
			  r->delarea.x1, r->delarea.y1,
			  r->delarea.x2, r->delarea.y2,
			  r->rtype, &lev);
	    break;

	case LR_TELE:
	case LR_UPTELE:
	case LR_DOWNTELE:
	    /* save the region outlines for goto_level() */
	    if(r->rtype == LR_TELE || r->rtype == LR_UPTELE) {
		    updest.lx = r->inarea.x1; updest.ly = r->inarea.y1;
		    updest.hx = r->inarea.x2; updest.hy = r->inarea.y2;
		    updest.nlx = r->delarea.x1; updest.nly = r->delarea.y1;
		    updest.nhx = r->delarea.x2; updest.nhy = r->delarea.y2;
	    }
	    if(r->rtype == LR_TELE || r->rtype == LR_DOWNTELE) {
		    dndest.lx = r->inarea.x1; dndest.ly = r->inarea.y1;
		    dndest.hx = r->inarea.x2; dndest.hy = r->inarea.y2;
		    dndest.nlx = r->delarea.x1; dndest.nly = r->delarea.y1;
		    dndest.nhx = r->delarea.x2; dndest.nhy = r->delarea.y2;
	    }
	    /* place_lregion gets called from goto_level() */
	    break;
	}

	if (r->rname.str) free((void *) r->rname.str),  r->rname.str = 0;
    }

    /* place dungeon branch if not placed above */
    if (!added_branch && Is_branchlev(&u.uz)) {
	place_lregion(0,0,0,0,0,0,0,0,LR_BRANCH,(d_level *)0);
    }

	/* KMH -- arboreal levels */
/*	if (level.flags.arboreal)
		for(x = 2; x < x_maze_max; x++)
			for(y = 2; y < y_maze_max; y++)
				if (levl[x][y].typ == STONE)
					levl[x][y].typ = TREE;*/

	/* KMH -- Sokoban levels */
	if(In_sokoban(&u.uz))
		sokoban_detect();

    /* Still need to add some stuff to level file */
    if (Is_medusa_level(&u.uz)) {
	struct obj *otmp;
	int tryct;

	croom = &rooms[0]; /* only one room on the medusa level */
	for (tryct = rnd(4); tryct; tryct--) {
	    x = somex(croom); y = somey(croom);
	    if (goodpos(x, y, (struct monst *)0, 0)) {
		otmp = mk_tt_object(STATUE, x, y);
		while (otmp && (poly_when_stoned(&mons[otmp->corpsenm]) ||
				pm_resistance(&mons[otmp->corpsenm],MR_STONE))) {
		    otmp->corpsenm = rndmonnum();
		    otmp->owt = weight(otmp);
		}
	    }
	}

	if (rn2(2))
	    otmp = mk_tt_object(STATUE, somex(croom), somey(croom));
	else /* Medusa statues don't contain books */
	    otmp = mkcorpstat(STATUE, (struct monst *)0, (struct permonst *)0,
			      somex(croom), somey(croom), FALSE);
	if (otmp) {
	    while (pm_resistance(&mons[otmp->corpsenm],MR_STONE)
		   || poly_when_stoned(&mons[otmp->corpsenm])) {
		otmp->corpsenm = rndmonnum();
		otmp->owt = weight(otmp);
	    }
	}
    } else if(Is_wiz1_level(&u.uz)) {
	croom = search_special(MORGUE);

	create_secret_door(croom, W_SOUTH|W_EAST|W_WEST);
    } else if(Is_knox(&u.uz)) {
	/* using an unfilled morgue for rm id */
	croom = search_special(MORGUE);
	/* avoid inappropriate morgue-related messages */
	level.flags.graveyard = level.flags.has_morgue = 0;
	croom->rtype = OROOM;	/* perhaps it should be set to VAULT? */
	/* stock the main vault */
	for(x = croom->lx; x <= croom->hx; x++)
	    for(y = croom->ly; y <= croom->hy; y++) {
		(void) mkgold((long) rn1(300, 600), x, y);
		if (!rn2(3) && !is_waterypool(x,y))
		    (void)maketrap(x, y, rn2(3) ? LANDMINE : SPIKED_PIT, 5, TRUE);
	    }
    } else if (Role_if(PM_PRIEST) && In_quest(&u.uz)) {
	/* less chance for undead corpses (lured from lower morgues) */
	level.flags.graveyard = 1;
    } else if (Is_stronghold(&u.uz)) {
	level.flags.graveyard = 1;
    } else if(Is_sanctum(&u.uz)) {
	croom = search_special(TEMPLE);

	create_secret_door(croom, W_ANY);
    } else if(on_level(&u.uz, &orcus_level)) {
	   register struct monst *mtmp, *mtmp2;

	   /* it's a ghost town, get rid of shopkeepers */
	    for(mtmp = fmon; mtmp; mtmp = mtmp2) {
		    mtmp2 = mtmp->nmon;
		    if(mtmp->isshk) mongone(mtmp);
	    }
    }

    if(lev_message) {
	char *str, *nl;
	for(str = lev_message; (nl = index(str, '\n')) != 0; str = nl+1) {
	    *nl = '\0';
	    pline("%s", str);
	}
	if(*str)
	    pline("%s", str);
	free((void *)lev_message);
	lev_message = 0;
    }

    if (lregions)
	free((void *) lregions),  lregions = 0;
    num_lregions = 0;
}

/*
 * Select a random coordinate in the maze.
 *
 * We want a place not 'touched' by the loader.  That is, a place in
 * the maze outside every part of the special level.
 */

void
makemaz(s)
register const char *s;
{
	int x,y;
	char protofile[20];
	s_level	*sp = Is_special(&u.uz);
	coord mm;
	struct monst *mtmp;

	if(*s) {
	    if(sp && sp->rndlevs) sprintf(protofile, "%s-%d", s,
						rnd((int) sp->rndlevs));
	    else		 strcpy(protofile, s);
	} else if(*(dungeons[u.uz.dnum].proto)) {
	    if(dunlevs_in_dungeon(&u.uz) > 1) {
		if(sp && sp->rndlevs)
		     sprintf(protofile, "%s%d-%d", dungeons[u.uz.dnum].proto,
						dunlev(&u.uz),
						rnd((int) sp->rndlevs));
		else sprintf(protofile, "%s%d", dungeons[u.uz.dnum].proto,
						dunlev(&u.uz));
	    } else if(sp && sp->rndlevs) {
		     sprintf(protofile, "%s-%d", dungeons[u.uz.dnum].proto,
						rnd((int) sp->rndlevs));
	    } else strcpy(protofile, dungeons[u.uz.dnum].proto);

	} else strcpy(protofile, "");

#ifdef WIZARD
	/* SPLEVTYPE format is "level-choice,level-choice"... */
	if (wizard && *protofile && sp && sp->rndlevs) {
	    char *ep = getenv("SPLEVTYPE");	/* not nh_getenv */
	    if (ep) {
		/* rindex always succeeds due to code in prior block */
		int len = (rindex(protofile, '-') - protofile) + 1;

		while (ep && *ep) {
		    if (!strncmp(ep, protofile, len)) {
			int pick = atoi(ep + len);
			/* use choice only if valid */
			if (pick > 0 && pick <= (int) sp->rndlevs)
			    sprintf(protofile + len, "%d", pick);
			break;
		    } else {
			ep = index(ep, ',');
			if (ep) ++ep;
		    }
		}
	    }
	}
#endif

	if(*protofile) {
	    strcat(protofile, LEV_EXT);
	    if(load_special(protofile)) {
		fixup_special();
		/* some levels can end up with monsters
		   on dead mon list, including light source monsters */
		dmonsfree();
		return;	/* no mazification right now */
	    }
	    /* impossible("Couldn't load \"%s\" - making a maze.", protofile); */
	    if (strncmpi(protofile, "makemase.lev", 13) && strncmpi(protofile, "makemais.lev", 13) ) pline("Couldn't load \"%s\" - making a maze.", protofile);
	}

	if (rn2(2)) level.flags.is_maze_lev = TRUE;

#ifndef WALLIFIED_MAZE
	for(x = 2; x < x_maze_max; x++)
		for(y = 2; y < y_maze_max; y++)
			levl[x][y].typ = STONE;
#else
	for(x = 2; x <= x_maze_max; x++)
		for(y = 2; y <= y_maze_max; y++)
			levl[x][y].typ = ((x % 2) && (y % 2)) ? STONE : HWALL;
#endif

	maze0xy(&mm);
	walkfrom((int) mm.x, (int) mm.y);
	/* put a boulder at the maze center */
	(void) mksobj_at(BOULDER, (int) mm.x, (int) mm.y, TRUE, FALSE, FALSE);

#ifdef WALLIFIED_MAZE
	wallification(2, 2, x_maze_max, y_maze_max, TRUE);
#endif
	mazexy_all(&mm);
	mkstairs(mm.x, mm.y, 1, (struct mkroom *)0);		/* up */
	if (!Invocation_lev(&u.uz)) {
	    mazexy_all(&mm);
	    mkstairs(mm.x, mm.y, 0, (struct mkroom *)0);	/* down */
	} else {	/* choose "vibrating square" location */
#define x_maze_min 2
#define y_maze_min 2
	    /*
	     * Pick a position where the stairs down to Moloch's Sanctum
	     * level will ultimately be created.  At that time, an area
	     * will be altered:  walls removed, moat and traps generated,
	     * boulders destroyed.  The position picked here must ensure
	     * that that invocation area won't extend off the map.
	     *
	     * We actually allow up to 2 squares around the usual edge of
	     * the area to get truncated; see mkinvokearea(mklev.c).
	     */
#define INVPOS_X_MARGIN (6 - 2)
#define INVPOS_Y_MARGIN (5 - 2)
#define INVPOS_DISTANCE 11
	    int x_range = x_maze_max - x_maze_min - 2*INVPOS_X_MARGIN - 1,
		y_range = y_maze_max - y_maze_min - 2*INVPOS_Y_MARGIN - 1;

#ifdef DEBUG
	    if (x_range <= INVPOS_X_MARGIN || y_range <= INVPOS_Y_MARGIN ||
		   (x_range * y_range) <= (INVPOS_DISTANCE * INVPOS_DISTANCE))
		panic("inv_pos: maze is too small! (%d x %d)",
		      x_maze_max, y_maze_max);
#endif
	    inv_pos.x = inv_pos.y = 0; /*{occupied() => invocation_pos()}*/
	    do {
		x = rn1(x_range, x_maze_min + INVPOS_X_MARGIN + 1);
		y = rn1(y_range, y_maze_min + INVPOS_Y_MARGIN + 1);
		/* we don't want it to be too near the stairs, nor
		   to be on a spot that's already in use (wall|trap) */
	    } while (x == xupstair || y == yupstair ||	/*(direct line)*/
		     abs(x - xupstair) == abs(y - yupstair) ||
		     distmin(x, y, xupstair, yupstair) <= INVPOS_DISTANCE ||
		     !SPACE_POS(levl[x][y].typ) || occupied(x, y));
	    inv_pos.x = x;
	    inv_pos.y = y;

		/* spice this boring maze up - seven deadly sins :D --Amy */
	    mtmp = makemon(&mons[PM_LUST], 0, 0, NO_MM_FLAGS);
		if (mtmp) (void) maketrap(mtmp->mx, mtmp->my, SIN_TRAP, 0, FALSE);
	    mtmp = makemon(&mons[PM_GLUTTONY], 0, 0, NO_MM_FLAGS);
		if (mtmp) (void) maketrap(mtmp->mx, mtmp->my, SIN_TRAP, 0, FALSE);
	    mtmp = makemon(&mons[PM_ENVY], 0, 0, NO_MM_FLAGS);
		if (mtmp) (void) maketrap(mtmp->mx, mtmp->my, SIN_TRAP, 0, FALSE);
	    mtmp = makemon(&mons[PM_PRIDE], 0, 0, NO_MM_FLAGS);
		if (mtmp) (void) maketrap(mtmp->mx, mtmp->my, SIN_TRAP, 0, FALSE);
	    mtmp = makemon(&mons[PM_GREED], 0, 0, NO_MM_FLAGS);
		if (mtmp) (void) maketrap(mtmp->mx, mtmp->my, SIN_TRAP, 0, FALSE);
	    mtmp = makemon(&mons[PM_SLOTH], 0, 0, NO_MM_FLAGS);
		if (mtmp) (void) maketrap(mtmp->mx, mtmp->my, SIN_TRAP, 0, FALSE);
	    mtmp = makemon(&mons[PM_WRATH], 0, 0, NO_MM_FLAGS);
		if (mtmp) (void) maketrap(mtmp->mx, mtmp->my, SIN_TRAP, 0, FALSE);


#undef INVPOS_X_MARGIN
#undef INVPOS_Y_MARGIN
#undef INVPOS_DISTANCE
#undef x_maze_min
#undef y_maze_min
	}

	/* place branch stair or portal */
	place_branch(Is_branchlev(&u.uz), 0, 0);

#ifdef BIGSLEX
	for(x = (rn2(4) ? rn1(12,24) : rn1(24, 48)); x; x--) {
#else
	for(x = (rn2(4) ? rn1(8,16) : rn1(16, 32)); x; x--) {
#endif
		if (timebasedlowerchance()) {
			mazexy_all(&mm);
			(void) mkobj_at(!rn2(10) ? GEM_CLASS : 0, mm.x, mm.y, TRUE, FALSE);
		}
	}

	if (moves == 1 && !rn2(2)) { /* some earlygame help... --Amy */
#ifdef BIGSLEX
		for(x = (rn2(4) ? rn1(12,24) : rn1(24, 48)); x; x--) {
#else
		for(x = (rn2(4) ? rn1(8,16) : rn1(16, 32)); x; x--) {
#endif
			if (timebasedlowerchance()) {
				mazexy_all(&mm);
				(void) mkobj_at(!rn2(10) ? GEM_CLASS : 0, mm.x, mm.y, TRUE, FALSE);
			}
		}
	}

#ifdef BIGSLEX
	for (x = rn1(4,20); x; x--) {
#else
	for (x = rn1(2,10); x; x--) {
#endif
		mazexy_all(&mm);
			    char buf[BUFSZ];
				const char *mesg = random_engraving(buf);
			    make_engr_at(mm.x, mm.y, mesg, 0L, (xchar)0);
			}
	for(x = rn1(10,2); x; x--) {
		mazexy_all(&mm);
		(void) mksobj_at(BOULDER, mm.x, mm.y, TRUE, FALSE, FALSE);
	}
	if (!(iszapem && !(u.zapemescape)) && !(u.preversionmode && !u.preversionescape) && (depth(&u.uz) > depth(&medusa_level))) {
	for (x = rn2(3); x; x--) {
		mazexy_all(&mm);
		if (!ishomicider) (void) makemon(&mons[PM_MINOTAUR], mm.x, mm.y, MM_MAYSLEEP);
		else makerandomtrap_at(mm.x, mm.y, TRUE);
		}
	}	 /* cause they would be outta depth when mazes are generated at a shallow level --Amy */
#ifdef BIGSLEX
	for(x = (rn2(2) ? rn1(40, 56) : rn2(4) ? rn1(10,14) : rn1(20, 28)); x; x--) {
#else
	for(x = (rn2(4) ? rn1(5,7) : rn1(10, 14)); x; x--) {
#endif
		mazexy_all(&mm);
		if (!ishomicider) (void) makemon((struct permonst *) 0, mm.x, mm.y, MM_MAYSLEEP);
		else makerandomtrap_at(mm.x, mm.y, TRUE);
	}
	for(x = rn1(6,7); x; x--) {
		mazexy_all(&mm);
		(void) mkgold(0L,mm.x,mm.y);
	}
#ifdef BIGSLEX
	for(x = (rn2(2) ? rn1(48, 56) : rn2(4) ? rn1(12,14) : rn1(24, 28)); x; x--) {
#else
	for(x = (rn2(4) ? rn1(6,7) : rn1(12, 14)); x; x--) {
#endif
		if (!(depth(&u.uz) == 1 && In_dod(&u.uz) && rn2(3)) && !(depth(&u.uz) == 2 && In_dod(&u.uz) && rn2(2)) ) {
			mktrap(0,1,(struct mkroom *) 0, (coord*) 0, TRUE);
		}
	}

	if (ishaxor) {
#ifdef BIGSLEX
	for(x = (rn2(4) ? rn1(12,24) : rn1(24, 48)); x; x--) {
#else
	for(x = (rn2(4) ? rn1(8,16) : rn1(16, 32)); x; x--) {
#endif
		if (timebasedlowerchance()) {
			mazexy_all(&mm);
			(void) mkobj_at(!rn2(10) ? GEM_CLASS : 0, mm.x, mm.y, TRUE, FALSE);
		}
	}
#ifdef BIGSLEX
	for (x = rn1(4,20); x; x--) {
#else
	for (x = rn1(2,10); x; x--) {
#endif
		mazexy_all(&mm);
			    char buf[BUFSZ];
				const char *mesg = random_engraving(buf);
			    make_engr_at(mm.x, mm.y, mesg, 0L, (xchar)0);
			}
	for(x = rn1(10,2); x; x--) {
		mazexy_all(&mm);
		(void) mksobj_at(BOULDER, mm.x, mm.y, TRUE, FALSE, FALSE);
	}
	if (!(iszapem && !(u.zapemescape)) && !(u.preversionmode && !u.preversionescape) && (depth(&u.uz) > depth(&medusa_level))) {
	for (x = rn2(3); x; x--) {
		mazexy_all(&mm);
		if (!ishomicider) (void) makemon(&mons[PM_MINOTAUR], mm.x, mm.y, MM_MAYSLEEP);
		else makerandomtrap_at(mm.x, mm.y, TRUE);
		}
	}	 /* cause they would be outta depth when mazes are generated at a shallow level --Amy */
#ifdef BIGSLEX
	for(x = (rn2(2) ? rn1(40, 56) : rn2(4) ? rn1(10,14) : rn1(20, 28)); x; x--) {
#else
	for(x = (rn2(4) ? rn1(5,7) : rn1(10, 14)); x; x--) {
#endif
		mazexy_all(&mm);
		if (!ishomicider) (void) makemon((struct permonst *) 0, mm.x, mm.y, MM_MAYSLEEP);
		else makerandomtrap_at(mm.x, mm.y, TRUE);
	}
	for(x = rn1(6,7); x; x--) {
		mazexy_all(&mm);
		(void) mkgold(0L,mm.x,mm.y);
	}
#ifdef BIGSLEX
	for(x = (rn2(2) ? rn1(48, 56) : rn2(4) ? rn1(12,14) : rn1(24, 28)); x; x--) {
#else
	for(x = (rn2(4) ? rn1(6,7) : rn1(12, 14)); x; x--) {
#endif
		if (!(depth(&u.uz) == 1 && In_dod(&u.uz) && rn2(3)) && !(depth(&u.uz) == 2 && In_dod(&u.uz) && rn2(2)) ) {
			mktrap(0,1,(struct mkroom *) 0, (coord*) 0, TRUE);
		}

	}

	} /* haxor check */

	/* chance to create Ludios portal, by Amy */
	if (In_dod(&u.uz)) {
		mazexy_all(&mm);
		if (isok(mm.x, mm.y)) mk_knox_portal(mm.x, mm.y);
	}

}

#ifdef MICRO
/* Make the mazewalk iterative by faking a stack.  This is needed to
 * ensure the mazewalk is successful in the limited stack space of
 * the program.  This iterative version uses the minimum amount of stack
 * that is totally safe.
 */
void
walkfrom(x,y)
int x,y;
{
#define CELLS (ROWNO * COLNO) / 4		/* a maze cell is 4 squares */
	char mazex[CELLS + 1], mazey[CELLS + 1];	/* char's are OK */
	int q, a, dir, pos;
	int dirs[4];

	register int tryct = 0;
	register struct obj *otmpX;
	const char *str;

	int specialcorridor = 0;
	if (!rn2(iswarper ? 50 : 500)) specialcorridor = rnd(2);

	pos = 1;
	mazex[pos] = (char) x;
	mazey[pos] = (char) y;
	while (pos) {
		x = (int) mazex[pos];
		y = (int) mazey[pos];
		if(!IS_DOOR(levl[x][y].typ) && !specialcorridor) {
		    /* might still be on edge of MAP, so don't overwrite */
#ifndef WALLIFIED_MAZE
		    levl[x][y].typ = CORR;
#else
		    levl[x][y].typ = ROOM;
#endif
		    levl[x][y].flags = 0;

			if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = THRONE;
			else if (!rn2(ishaxor ? 50000 : 100000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = PENTAGRAM;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WELL;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = POISONEDWELL;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WAGON;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = BURNINGWAGON;
			else if (!rn2(ishaxor ? 20000 : 40000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WOODENTABLE;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CARVEDBED;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = STRAWMATTRESS;
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				}
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = SINK;
				level.flags.nsinks++;
				}
			else if (!rn2(ishaxor ? 5000 : 10000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TOILET;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = GRAVE;
				str = random_epitaph();
				del_engr_at(x, y);
				make_engr_at(x, y, str, 0L, HEADSTONE);

				if (!rn2(3)) (void) mkgold(0L, x, y);
				for (tryct = rn2(5); tryct; tryct--) {
					if (timebasedlowerchance()) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE, FALSE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = x;
					    otmpX->oy = y;
					    add_to_buried(otmpX);
					    }
					}
				}
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = ALTAR;
				if (rn2(10)) levl[x][y].altarmask = Align2amask( A_NONE );
				else switch (rnd(3)) {
	
				case 1: levl[x][y].altarmask = Align2amask( A_LAWFUL ); break;
				case 2: levl[x][y].altarmask = Align2amask( A_NEUTRAL ); break;
				case 3: levl[x][y].altarmask = Align2amask( A_CHAOTIC ); break;
	
				}
			}
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TREE;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOAT;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = LAVAPOOL;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ICE;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CLOUD;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = GRAVEWALL;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = SNOW;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ASH;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = SAND;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = PAVEDFLOOR;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = HIGHWAY;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = GRASSLAND;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = NETHERMIST;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = STALACTITE;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CRYPTFLOOR;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = BUBBLES;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = RAINCLOUD;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TUNNELWALL;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = FARMLAND;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOUNTAIN;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WATERTUNNEL;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CRYSTALWATER;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOORLAND;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = URINELAKE;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = SHIFTINGSAND;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = STYXRIVER;

		}
		if(!IS_DOOR(levl[x][y].typ) && specialcorridor) {

		    levl[x][y].typ = walkableterrain();
		    levl[x][y].flags = 0;
		}

		q = 0;
		for (a = 0; a < 4; a++)
			if(okay(x, y, a)) dirs[q++]= a;
		if (!q)
			pos--;
		else {
			dir = dirs[rn2(q)];
			move(&x, &y, dir);

			if (!specialcorridor) {
#ifndef WALLIFIED_MAZE
			levl[x][y].typ = CORR;
#else
			levl[x][y].typ = ROOM;
#endif

			if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = THRONE;
			else if (!rn2(ishaxor ? 50000 : 100000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = PENTAGRAM;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WELL;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = POISONEDWELL;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WAGON;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = BURNINGWAGON;
			else if (!rn2(ishaxor ? 20000 : 40000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WOODENTABLE;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CARVEDBED;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = STRAWMATTRESS;
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))){
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				}
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = SINK;
				level.flags.nsinks++;
				}
			else if (!rn2(ishaxor ? 5000 : 10000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TOILET;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = GRAVE;
				str = random_epitaph();
				del_engr_at(x, y);
				make_engr_at(x, y, str, 0L, HEADSTONE);
	
				if (!rn2(3)) (void) mkgold(0L, x, y);
				for (tryct = rn2(5); tryct; tryct--) {
					if (timebasedlowerchance()) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE, FALSE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = x;
					    otmpX->oy = y;
					    add_to_buried(otmpX);
					    }
					}
				}
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = ALTAR;
				if (rn2(10)) levl[x][y].altarmask = Align2amask( A_NONE );
				else switch (rnd(3)) {
	
				case 1: levl[x][y].altarmask = Align2amask( A_LAWFUL ); break;
				case 2: levl[x][y].altarmask = Align2amask( A_NEUTRAL ); break;
				case 3: levl[x][y].altarmask = Align2amask( A_CHAOTIC ); break;
	
				}
			}
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TREE;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOAT;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = LAVAPOOL;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ICE;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CLOUD;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = GRAVEWALL;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = SNOW;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ASH;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = SAND;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = PAVEDFLOOR;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = HIGHWAY;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = GRASSLAND;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = NETHERMIST;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = STALACTITE;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CRYPTFLOOR;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = BUBBLES;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = RAINCLOUD;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TUNNELWALL;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = FARMLAND;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOUNTAIN;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WATERTUNNEL;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CRYSTALWATER;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOORLAND;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = URINELAKE;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = SHIFTINGSAND;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = STYXRIVER;

			}
			else levl[x][y].typ = walkableterrain();

			move(&x, &y, dir);
			pos++;
			if (pos > CELLS)
				panic("Overflow in walkfrom");
			mazex[pos] = (char) x;
			mazey[pos] = (char) y;
		}
	}
}
#else

void
walkfrom(x,y)
int x,y;
{
	register int q,a,dir;
	int dirs[4];

	register int tryct = 0;
	register struct obj *otmpX;
	const char *str;

	int specialcorridor = 0;
	if (!rn2(iswarper ? 50 : 500)) specialcorridor = rnd(2);

	if(!IS_DOOR(levl[x][y].typ) && !specialcorridor) {
	    /* might still be on edge of MAP, so don't overwrite */
#ifndef WALLIFIED_MAZE
	    levl[x][y].typ = CORR;
#else
	    levl[x][y].typ = ROOM;
#endif
	    levl[x][y].flags = 0;

			if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = THRONE;
			else if (!rn2(ishaxor ? 50000 : 100000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = PENTAGRAM;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WELL;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = POISONEDWELL;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WAGON;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = BURNINGWAGON;
			else if (!rn2(ishaxor ? 20000 : 40000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WOODENTABLE;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CARVEDBED;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = STRAWMATTRESS;
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				}
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = SINK;
				level.flags.nsinks++;
				}
			else if (!rn2(ishaxor ? 5000 : 10000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TOILET;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = GRAVE;
				str = random_epitaph();
				del_engr_at(x, y);
				make_engr_at(x, y, str, 0L, HEADSTONE);
	
				if (!rn2(3)) (void) mkgold(0L, x, y);
				for (tryct = rn2(5); tryct; tryct--) {
					if (timebasedlowerchance()) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE, FALSE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = x;
					    otmpX->oy = y;
					    add_to_buried(otmpX);
					    }
					}
				}
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = ALTAR;
				if (rn2(10)) levl[x][y].altarmask = Align2amask( A_NONE );
				else switch (rnd(3)) {
	
				case 1: levl[x][y].altarmask = Align2amask( A_LAWFUL ); break;
				case 2: levl[x][y].altarmask = Align2amask( A_NEUTRAL ); break;
				case 3: levl[x][y].altarmask = Align2amask( A_CHAOTIC ); break;
	
				}
			}
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TREE;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOAT;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = LAVAPOOL;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ICE;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CLOUD;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = GRAVEWALL;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = SNOW;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ASH;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = SAND;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = PAVEDFLOOR;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = HIGHWAY;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = GRASSLAND;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = NETHERMIST;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = STALACTITE;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CRYPTFLOOR;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = BUBBLES;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = RAINCLOUD;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TUNNELWALL;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = FARMLAND;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOUNTAIN;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WATERTUNNEL;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CRYSTALWATER;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOORLAND;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = URINELAKE;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = SHIFTINGSAND;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = STYXRIVER;

	}
	if(!IS_DOOR(levl[x][y].typ) && specialcorridor) {

	    levl[x][y].typ = walkableterrain();
	    levl[x][y].flags = 0;
	}

	while(1) {
		q = 0;
		for(a = 0; a < 4; a++)
			if(okay(x,y,a)) dirs[q++]= a;
		if(!q) return;
		dir = dirs[rn2(q)];
		move(&x,&y,dir);
		if (!specialcorridor) {
#ifndef WALLIFIED_MAZE
		levl[x][y].typ = CORR;
#else
		levl[x][y].typ = ROOM;
#endif
			if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = THRONE;
			else if (!rn2(ishaxor ? 50000 : 100000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = PENTAGRAM;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WELL;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = POISONEDWELL;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WAGON;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = BURNINGWAGON;
			else if (!rn2(ishaxor ? 20000 : 40000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WOODENTABLE;
			else if (!rn2(ishaxor ? 25000 : 50000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CARVEDBED;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = STRAWMATTRESS;
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				}
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = SINK;
				level.flags.nsinks++;
				}
			else if (!rn2(ishaxor ? 5000 : 10000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TOILET;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = GRAVE;
				str = random_epitaph();
				del_engr_at(x, y);
				make_engr_at(x, y, str, 0L, HEADSTONE);
	
				if (!rn2(3)) (void) mkgold(0L, x, y);
				for (tryct = rn2(5); tryct; tryct--) {
					if (timebasedlowerchance()) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE, FALSE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = x;
					    otmpX->oy = y;
					    add_to_buried(otmpX);
					    }
					}
				}
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = ALTAR;
				if (rn2(10)) levl[x][y].altarmask = Align2amask( A_NONE );
				else switch (rnd(3)) {
	
				case 1: levl[x][y].altarmask = Align2amask( A_LAWFUL ); break;
				case 2: levl[x][y].altarmask = Align2amask( A_NEUTRAL ); break;
				case 3: levl[x][y].altarmask = Align2amask( A_CHAOTIC ); break;
	
				}
			}
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TREE;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOAT;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = LAVAPOOL;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ICE;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CLOUD;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = GRAVEWALL;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = SNOW;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ASH;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = SAND;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = PAVEDFLOOR;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = HIGHWAY;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = GRASSLAND;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = NETHERMIST;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = STALACTITE;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CRYPTFLOOR;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = BUBBLES;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = RAINCLOUD;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TUNNELWALL;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = FARMLAND;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOUNTAIN;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = WATERTUNNEL;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CRYSTALWATER;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOORLAND;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = URINELAKE;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = SHIFTINGSAND;
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = STYXRIVER;

		}
		else levl[x][y].typ = walkableterrain();
		move(&x,&y,dir);
		walkfrom(x,y);
	}
}
#endif /* MICRO */

STATIC_OVL void
move(x,y,dir)
register int *x, *y;
register int dir;
{
	switch(dir){
		case 0: --(*y); break;
		case 1: (*x)++; break;
		case 2: (*y)++; break;
		case 3: --(*x); break;
		default: panic("move: bad direction");
	}
}

void
mazexy(cc)	/* find random point in generated corridors,
		   so we don't create items in moats, bunkers, or walls */
	coord	*cc;
{
	int cpt=0;

	do {
	    cc->x = 3 + 2*rn2((x_maze_max>>1) - 1);
	    cc->y = 3 + 2*rn2((y_maze_max>>1) - 1);
	    cpt++;
	} while (cpt < 100 && levl[cc->x][cc->y].typ !=
#ifdef WALLIFIED_MAZE
		 ROOM
#else
		 CORR
#endif
		);
	if (cpt >= 100) {
		register int x, y;
		/* last try */
		for (x = 0; x < (x_maze_max>>1) - 1; x++)
		    for (y = 0; y < (y_maze_max>>1) - 1; y++) {
			cc->x = 3 + 2 * x;
			cc->y = 3 + 2 * y;
			if (levl[cc->x][cc->y].typ ==
#ifdef WALLIFIED_MAZE
			    ROOM
#else
			    CORR
#endif
			   ) return;
		    }

		for (x = 0; x < (x_maze_max>>1) - 1; x++)
		    for (y = 0; y < (y_maze_max>>1) - 1; y++) {
			cc->x = 3 + 2 * x;
			cc->y = 3 + 2 * y;
			if ((levl[cc->x][cc->y].typ == ROOM) || (levl[cc->x][cc->y].typ == CORR) || (levl[cc->x][cc->y].typ == CLOUD) || (levl[cc->x][cc->y].typ == ICE) || (levl[cc->x][cc->y].typ == SNOW) || (levl[cc->x][cc->y].typ == ASH) || (levl[cc->x][cc->y].typ == SAND) || (levl[cc->x][cc->y].typ == PAVEDFLOOR) || (levl[cc->x][cc->y].typ == HIGHWAY) || (levl[cc->x][cc->y].typ == GRASSLAND) || (levl[cc->x][cc->y].typ == NETHERMIST) || (levl[cc->x][cc->y].typ == STALACTITE) || (levl[cc->x][cc->y].typ == CRYPTFLOOR) || (levl[cc->x][cc->y].typ == BUBBLES) || (levl[cc->x][cc->y].typ == RAINCLOUD)) return;
		    }

		for (x = 0; x < (x_maze_max) - 1; x++)
		    for (y = 0; y < (y_maze_max) - 1; y++) {
			cc->x = 3 + x;
			cc->y = 3 + y;
			if ((levl[cc->x][cc->y].typ >= ROCKWALL && levl[cc->x][cc->y].typ <= URINELAKE) || (levl[cc->x][cc->y].typ >= SHIFTINGSAND && levl[cc->x][cc->y].typ <= ROOM) || (levl[cc->x][cc->y].typ >= ICE && levl[cc->x][cc->y].typ <= CRYPTFLOOR) || (levl[cc->x][cc->y].typ >= AIR && levl[cc->x][cc->y].typ <= RAINCLOUD) ) return;
		    }

		panic("mazexy: can't find a place!");
	}
	return;
}

void
mazexy_all(cc)	/* mazexy() only returns "even-numbered" squares... --Amy */
	coord	*cc;
{
	int cpt=0;

	do {
	    cc->x = 3 + rn2((x_maze_max) - 1);
	    cc->y = 3 + rn2((y_maze_max) - 1);
	    cpt++;
	} while (cpt < 100 && levl[cc->x][cc->y].typ !=
#ifdef WALLIFIED_MAZE
		 ROOM
#else
		 CORR
#endif
		);
	if (cpt >= 100) {
		register int x, y;
		/* last try */
		for (x = 0; x < (x_maze_max) - 1; x++)
		    for (y = 0; y < (y_maze_max) - 1; y++) {
			cc->x = 3 + x;
			cc->y = 3 + y;
			if (levl[cc->x][cc->y].typ ==
#ifdef WALLIFIED_MAZE
			    ROOM
#else
			    CORR
#endif
			   ) return;
		    }

		for (x = 0; x < (x_maze_max) - 1; x++)
		    for (y = 0; y < (y_maze_max) - 1; y++) {
			cc->x = 3 + x;
			cc->y = 3 + y;
			if ((levl[cc->x][cc->y].typ == ROOM) || (levl[cc->x][cc->y].typ == CORR) || (levl[cc->x][cc->y].typ == CLOUD) || (levl[cc->x][cc->y].typ == ICE) || (levl[cc->x][cc->y].typ == SNOW) || (levl[cc->x][cc->y].typ == ASH) || (levl[cc->x][cc->y].typ == SAND) || (levl[cc->x][cc->y].typ == PAVEDFLOOR) || (levl[cc->x][cc->y].typ == HIGHWAY) || (levl[cc->x][cc->y].typ == GRASSLAND) || (levl[cc->x][cc->y].typ == NETHERMIST) || (levl[cc->x][cc->y].typ == STALACTITE) || (levl[cc->x][cc->y].typ == CRYPTFLOOR) || (levl[cc->x][cc->y].typ == BUBBLES) || (levl[cc->x][cc->y].typ == RAINCLOUD)) return;
		    }

		for (x = 0; x < (x_maze_max) - 1; x++)
		    for (y = 0; y < (y_maze_max) - 1; y++) {
			cc->x = 3 + x;
			cc->y = 3 + y;
			if ((levl[cc->x][cc->y].typ >= ROCKWALL && levl[cc->x][cc->y].typ <= URINELAKE) || (levl[cc->x][cc->y].typ >= SHIFTINGSAND && levl[cc->x][cc->y].typ <= ROOM) || (levl[cc->x][cc->y].typ >= ICE && levl[cc->x][cc->y].typ <= CRYPTFLOOR) || (levl[cc->x][cc->y].typ >= AIR && levl[cc->x][cc->y].typ <= RAINCLOUD) ) return;
		    }

		panic("mazexy_all: can't find a place!");
	}
	return;
}

void
bound_digging()
/* put a non-diggable boundary around the initial portion of a level map.
 * assumes that no level will initially put things beyond the isok() range.
 *
 * we can't bound unconditionally on the last line with something in it,
 * because that something might be a niche which was already reachable,
 * so the boundary would be breached
 *
 * we can't bound unconditionally on one beyond the last line, because
 * that provides a window of abuse for WALLIFIED_MAZE special levels
 */
{
	register int x,y;
	register int quasarchance;
	register unsigned typ;
	register struct rm *lev;
	boolean found, nonwall;
	int xmin,xmax,ymin,ymax;

	quasarchance = 3 + rn2(5);

	for (x = 0; x < COLNO; x++)
	  for (y = 0; y < ROWNO; y++) {
		if (!rn2(quasarchance)) levl[x][y].wall_info |= W_QUASAROK;
	}

	if(Is_earthlevel(&u.uz)) return; /* everything diggable here */

	found = nonwall = FALSE;
	for(xmin=0; !found; xmin++) {
		lev = &levl[xmin][0];
		for(y=0; y<=ROWNO-1; y++, lev++) {
			typ = lev->typ;
			if(typ != STONE) {
				found = TRUE;
				if(!IS_WALL(typ)) nonwall = TRUE;
			}
		}
	}
	xmin -= (nonwall || !level.flags.is_maze_lev) ? 2 : 1;
	if (xmin < 0) xmin = 0;

	found = nonwall = FALSE;
	for(xmax=COLNO-1; !found; xmax--) {
		lev = &levl[xmax][0];
		for(y=0; y<=ROWNO-1; y++, lev++) {
			typ = lev->typ;
			if(typ != STONE) {
				found = TRUE;
				if(!IS_WALL(typ)) nonwall = TRUE;
			}
		}
	}
	xmax += (nonwall || !level.flags.is_maze_lev) ? 2 : 1;
	if (xmax >= COLNO) xmax = COLNO-1;

	found = nonwall = FALSE;
	for(ymin=0; !found; ymin++) {
		lev = &levl[xmin][ymin];
		for(x=xmin; x<=xmax; x++, lev += ROWNO) {
			typ = lev->typ;
			if(typ != STONE) {
				found = TRUE;
				if(!IS_WALL(typ)) nonwall = TRUE;
			}
		}
	}
	ymin -= (nonwall || !level.flags.is_maze_lev) ? 2 : 1;

	found = nonwall = FALSE;
	for(ymax=ROWNO-1; !found; ymax--) {
		lev = &levl[xmin][ymax];
		for(x=xmin; x<=xmax; x++, lev += ROWNO) {
			typ = lev->typ;
			if(typ != STONE) {
				found = TRUE;
				if(!IS_WALL(typ)) nonwall = TRUE;
			}
		}
	}
	ymax += (nonwall || !level.flags.is_maze_lev) ? 2 : 1;

	for (x = 0; x < COLNO; x++)
	  for (y = 0; y < ROWNO; y++)
	    if (y <= ymin || y >= ymax || x <= xmin || x >= xmax) {
#ifdef DCC30_BUG
		lev = &levl[x][y];
		lev->wall_info |= W_NONDIGGABLE;
#else
		levl[x][y].wall_info |= W_NONDIGGABLE;
#endif
	    }
}

void
mkportal(x, y, todnum, todlevel)
register xchar x, y, todnum, todlevel;
{
	/* a portal "trap" must be matched by a */
	/* portal in the destination dungeon/dlevel */
	register struct trap *ttmp = maketrap(x, y, MAGIC_PORTAL, 0, FALSE);

	if (!ttmp) {
		impossible("portal on top of portal??");
		return;
	}
#ifdef DEBUG
	pline("mkportal: at (%d,%d), to %s, level %d",
		x, y, dungeons[todnum].dname, todlevel);
#endif
	ttmp->dst.dnum = todnum;
	ttmp->dst.dlevel = todlevel;
	return;
}

/*
 * Special waterlevel stuff in endgame (TH).
 *
 * Some of these functions would probably logically belong to some
 * other source files, but they are all so nicely encapsulated here.
 */

/* to ease the work of debuggers at this stage */
#define register

#define CONS_OBJ   0
#define CONS_MON   1
#define CONS_HERO  2
#define CONS_TRAP  3

static struct bubble *bbubbles, *ebubbles;

static struct trap *wportal;
static int xmin, ymin, xmax, ymax;	/* level boundaries */
/* bubble movement boundaries */
#define bxmin (xmin + 1)
#define bymin (ymin + 1)
#define bxmax (xmax - 1)
#define bymax (ymax - 1)

STATIC_DCL void set_wportal(void);
STATIC_DCL void mk_bubble(int,int,int);
STATIC_DCL void mv_bubble(struct bubble *,int,int,BOOLEAN_P);

void
movebubbles()
{
	static boolean up;
	register struct bubble *b;
	register int x, y, i, j;
	struct trap *btrap;
	static const struct rm water_pos =
#ifdef DISPLAY_LAYERS
		{ S_water, 0, 0, 0, 0, 0,
#else
		{ cmap_to_glyph(S_water),
#endif
		WATER, 0, 0, 0, 0, 0, 0, 0 };

	/* set up the portal the first time bubbles are moved */
	if (!wportal) set_wportal();

	vision_recalc(2);
	/* fix from 3.4.3: keep attached ball&chain separate from bubble objects, otherwise panic! */
	if (Punished) unplacebc();

	/*
	 * Pick up everything inside of a bubble then fill all bubble
	 * locations.
	 */

	for (b = up ? bbubbles : ebubbles; b; b = up ? b->next : b->prev) {
	    if (b->cons) panic("movebubbles: cons != null");
	    for (i = 0, x = b->x; i < (int) b->bm[0]; i++, x++)
		for (j = 0, y = b->y; j < (int) b->bm[1]; j++, y++)
		    if (b->bm[j + 2] & (1 << i)) {
			if (!isok(x,y)) {
			    impossible("movebubbles: bad pos (%d,%d)", x,y);
			    continue;
			}

			/* pick up objects, monsters, hero, and traps */
			if (OBJ_AT(x,y)) {
			    struct obj *olist = (struct obj *) 0, *otmp;
			    struct container *cons = (struct container *)
				alloc(sizeof(struct container));

			    while ((otmp = level.objects[x][y]) != 0) {
				remove_object(otmp);
				otmp->ox = otmp->oy = 0;
				otmp->nexthere = olist;
				olist = otmp;
			    }

			    cons->x = x;
			    cons->y = y;
			    cons->what = CONS_OBJ;
			    cons->list = (void *) olist;
			    cons->next = b->cons;
			    b->cons = cons;
			}
			if (MON_AT(x,y)) {
			    struct monst *mon = m_at(x,y);
			    struct container *cons = (struct container *)
				alloc(sizeof(struct container));

			    cons->x = x;
			    cons->y = y;
			    cons->what = CONS_MON;
			    cons->list = (void *) mon;

			    cons->next = b->cons;
			    b->cons = cons;

			    if(mon->wormno)
				remove_worm(mon);
			    else
				remove_monster(x, y);

			    newsym(x,y);	/* clean up old position */
			    mon->mx = mon->my = 0;
			}
			if (!u.uswallow && x == u.ux && y == u.uy) {
			    struct container *cons = (struct container *)
				alloc(sizeof(struct container));

			    cons->x = x;
			    cons->y = y;
			    cons->what = CONS_HERO;
			    cons->list = (void *) 0;

			    cons->next = b->cons;
			    b->cons = cons;
			}
			if ((btrap = t_at(x,y)) != 0) {
			    struct container *cons = (struct container *)
				alloc(sizeof(struct container));

			    cons->x = x;
			    cons->y = y;
			    cons->what = CONS_TRAP;
			    cons->list = (void *) btrap;

			    cons->next = b->cons;
			    b->cons = cons;
			}

			levl[x][y] = water_pos;
			block_point(x,y);
		    }
	}

	/*
	 * Every second time traverse down.  This is because otherwise
	 * all the junk that changes owners when bubbles overlap
	 * would eventually end up in the last bubble in the chain.
	 */

	up = !up;
	for (b = up ? bbubbles : ebubbles; b; b = up ? b->next : b->prev) {
		register int rx = rn2(3), ry = rn2(3);

		mv_bubble(b,b->dx + 1 - (!b->dx ? rx : (rx ? 1 : 0)),
			    b->dy + 1 - (!b->dy ? ry : (ry ? 1 : 0)),
			    FALSE);
	}

	/* put attached ball&chain back */
	if (Punished) placebc();
	vision_full_recalc = 1;
}

/* when moving in water, possibly (1 in 3) alter the intended destination */
void
water_friction()
{
	register int x, y, dx, dy;
	register boolean eff = FALSE;

	if (Race_if(PM_SEA_ELF)) return; /* idea by Elronnd: they are at home in the sea, and therefore unaffected */
	if (tech_inuse(T_SILENT_OCEAN)) return; /* if the ocean is silent, it means there are no currents */

	if (Swimming && rn2(StrongSwimming ? 20 : 4))
		return;		/* natural swimmers have advantage */

	if (u.dx && !rn2(!u.dy ? 3 : 6)) {	/* 1/3 chance or half that */
		/* cancel delta x and choose an arbitrary delta y value */
		x = u.ux;
		do {
		    dy = rn2(3) - 1;		/* -1, 0, 1 */
		    y = u.uy + dy;
		} while (dy && (!isok(x,y) || !is_waterypool(x,y)));
		u.dx = 0;
		u.dy = dy;
		eff = TRUE;
	} else if (u.dy && !rn2(!u.dx ? 3 : 5)) {	/* 1/3 or 1/5*(5/6) */
		/* cancel delta y and choose an arbitrary delta x value */
		y = u.uy;
		do {
		    dx = rn2(3) - 1;		/* -1 .. 1 */
		    x = u.ux + dx;
		} while (dx && (!isok(x,y) || !is_waterypool(x,y)));
		u.dy = 0;
		u.dx = dx;
		eff = TRUE;
	}
	if (eff) pline("Water turbulence affects your movements.");
}

void
save_waterlevel(fd, mode)
int fd, mode;
{
	register struct bubble *b;

	if (!Is_waterlevel(&u.uz)) return;

	if (perform_bwrite(mode)) {
	    int n = 0;
	    for (b = bbubbles; b; b = b->next) ++n;
	    bwrite(fd, (void *)&n, sizeof (int));
	    bwrite(fd, (void *)&xmin, sizeof (int));
	    bwrite(fd, (void *)&ymin, sizeof (int));
	    bwrite(fd, (void *)&xmax, sizeof (int));
	    bwrite(fd, (void *)&ymax, sizeof (int));
	    for (b = bbubbles; b; b = b->next)
		bwrite(fd, (void *)b, sizeof (struct bubble));
	}
	if (release_data(mode))
	    unsetup_waterlevel();
}

void
restore_waterlevel(fd)
register int fd;
{
	register struct bubble *b = (struct bubble *)0, *btmp;
	register int i;
	int n;

	if (!Is_waterlevel(&u.uz)) return;

	set_wportal();
	mread(fd,(void *)&n,sizeof(int));
	mread(fd,(void *)&xmin,sizeof(int));
	mread(fd,(void *)&ymin,sizeof(int));
	mread(fd,(void *)&xmax,sizeof(int));
	mread(fd,(void *)&ymax,sizeof(int));

	if (n > 0) {
		for (i = 0; i < n; i++) {
			btmp = b;
			b = (struct bubble *)alloc(sizeof(struct bubble));
			mread(fd,(void *)b,sizeof(struct bubble));
			if (bbubbles) {
				btmp->next = b;
				b->prev = btmp;
			} else {
				bbubbles = b;
				b->prev = (struct bubble *)0;
			}
			mv_bubble(b,0,0,TRUE);
		}
		ebubbles = b;
		b->next = (struct bubble *)0;
	}
	was_waterlevel = TRUE;
}

const char *waterbody_name(x, y)
xchar x,y;
{
	register struct rm *lev;
	schar ltyp;

	if (!isok(x,y))
		return "drink";		/* should never happen */
	lev = &levl[x][y];
	ltyp = lev->typ;

	if (is_lava(x,y))
		return "lava";
	else if (ltyp == ICE ||
		 (ltyp == DRAWBRIDGE_UP &&
		  (levl[x][y].drawbridgemask & DB_UNDER) == DB_ICE))
		return "ice";
	else if (((ltyp != POOL) && (ltyp != WATER) &&
	  !Is_medusa_level(&u.uz) && !Is_waterlevel(&u.uz) && !Is_juiblex_level(&u.uz)) ||
	   (ltyp == DRAWBRIDGE_UP && (levl[x][y].drawbridgemask & DB_UNDER) == DB_MOAT))
		return "moat";
	else if ((ltyp != POOL) && (ltyp != WATER) && Is_juiblex_level(&u.uz))
		return "swamp";
	else if (ltyp == POOL)
		return "pool of water";
	else if (ltyp == MOORLAND)
		return "moorland";
	else if (ltyp == URINELAKE)
		return "urine lake";
	else if (ltyp == STYXRIVER)
		return "styx river";
	else return "water";
}

STATIC_OVL void
set_wportal()
{
	/* there better be only one magic portal on water level... */
	for (wportal = ftrap; wportal; wportal = wportal->ntrap)
		if (wportal->ttyp == MAGIC_PORTAL) return;
	impossible("set_wportal(): no portal!");
}

STATIC_OVL void
setup_waterlevel()
{
	register int x, y;
	register int xskip, yskip;

	/* ouch, hardcoded... */

#ifdef BIGSLEX
	xmin = 26;
	ymin = 13;
	xmax = 101;
	ymax = 32;
#else
	xmin = 3;
	ymin = 1;
	xmax = 78;
	ymax = 20;
#endif

	/* set hero's memory to water */

	for (x = xmin; x <= xmax; x++)
		for (y = ymin; y <= ymax; y++)
			clear_memory_glyph(x, y, S_water);

	/* make bubbles */

	xskip = 10 + rn2(10);
	yskip = 4 + rn2(4);
	for (x = bxmin; x <= bxmax; x += xskip)
		for (y = bymin; y <= bymax; y += yskip)
			mk_bubble(x,y,rn2(7));
}

STATIC_OVL void
unsetup_waterlevel()
{
	register struct bubble *b, *bb;

	/* free bubbles */

	for (b = bbubbles; b; b = bb) {
		bb = b->next;
		free((void *)b);
	}
	bbubbles = ebubbles = (struct bubble *)0;
}

STATIC_OVL void
mk_bubble(x,y,n)
register int x, y, n;
{
	/*
	 * These bit masks make visually pleasing bubbles on a normal aspect
	 * 25x80 terminal, which naturally results in them being mathematically
	 * anything but symmetric.  For this reason they cannot be computed
	 * in situ, either.  The first two elements tell the dimensions of
	 * the bubble's bounding box.
	 */
	static uchar
		bm2[] = {2,1,0x3},
		bm3[] = {3,2,0x7,0x7},
		bm4[] = {4,3,0x6,0xf,0x6},
		bm5[] = {5,3,0xe,0x1f,0xe},
		bm6[] = {6,4,0x1e,0x3f,0x3f,0x1e},
		bm7[] = {7,4,0x3e,0x7f,0x7f,0x3e},
		bm8[] = {8,4,0x7e,0xff,0xff,0x7e},
		*bmask[] = {bm2,bm3,bm4,bm5,bm6,bm7,bm8};

	register struct bubble *b;

	if (x >= bxmax || y >= bymax) return;
	if (n >= SIZE(bmask)) {
		impossible("n too large (mk_bubble)");
		n = SIZE(bmask) - 1;
	}
	b = (struct bubble *)alloc(sizeof(struct bubble));
	if ((x + (int) bmask[n][0] - 1) > bxmax) x = bxmax - bmask[n][0] + 1;
	if ((y + (int) bmask[n][1] - 1) > bymax) y = bymax - bmask[n][1] + 1;
	b->x = x;
	b->y = y;
	b->dx = 1 - rn2(3);
	b->dy = 1 - rn2(3);
	b->bm = bmask[n];
	b->cons = 0;
	if (!bbubbles) bbubbles = b;
	if (ebubbles) {
		ebubbles->next = b;
		b->prev = ebubbles;
	}
	else
		b->prev = (struct bubble *)0;
	b->next =  (struct bubble *)0;
	ebubbles = b;
	mv_bubble(b,0,0,TRUE);
}

/*
 * The player, the portal and all other objects and monsters
 * float along with their associated bubbles.  Bubbles may overlap
 * freely, and the contents may get associated with other bubbles in
 * the process.  Bubbles are "sticky", meaning that if the player is
 * in the immediate neighborhood of one, he/she may get sucked inside.
 * This property also makes leaving a bubble slightly difficult.
 */
STATIC_OVL void
mv_bubble(b,dx,dy,ini)
register struct bubble *b;
register int dx, dy;
register boolean ini;
{
	register int x, y, i, j, colli = 0;
	struct container *cons, *ctemp;

	/* move bubble */
	if (dx < -1 || dx > 1 || dy < -1 || dy > 1) {
	    /* pline("mv_bubble: dx = %d, dy = %d", dx, dy); */
	    dx = sgn(dx);
	    dy = sgn(dy);
	}

	/*
	 * collision with level borders?
	 *	1 = horizontal border, 2 = vertical, 3 = corner
	 */
	if (b->x <= bxmin) colli |= 2;
	if (b->y <= bymin) colli |= 1;
	if ((int) (b->x + b->bm[0] - 1) >= bxmax) colli |= 2;
	if ((int) (b->y + b->bm[1] - 1) >= bymax) colli |= 1;

	if (b->x < bxmin) {
	    pline("bubble xmin: x = %d, xmin = %d", b->x, bxmin);
	    b->x = bxmin;
	}
	if (b->y < bymin) {
	    pline("bubble ymin: y = %d, ymin = %d", b->y, bymin);
	    b->y = bymin;
	}
	if ((int) (b->x + b->bm[0] - 1) > bxmax) {
	    pline("bubble xmax: x = %d, xmax = %d",
			b->x + b->bm[0] - 1, bxmax);
	    b->x = bxmax - b->bm[0] + 1;
	}
	if ((int) (b->y + b->bm[1] - 1) > bymax) {
	    pline("bubble ymax: y = %d, ymax = %d",
			b->y + b->bm[1] - 1, bymax);
	    b->y = bymax - b->bm[1] + 1;
	}

	/* bounce if we're trying to move off the border */
	if (b->x == bxmin && dx < 0) dx = -dx;
	if (b->x + b->bm[0] - 1 == bxmax && dx > 0) dx = -dx;
	if (b->y == bymin && dy < 0) dy = -dy;
	if (b->y + b->bm[1] - 1 == bymax && dy > 0) dy = -dy;

	b->x += dx;
	b->y += dy;

	/* void positions inside bubble */

	for (i = 0, x = b->x; i < (int) b->bm[0]; i++, x++)
	    for (j = 0, y = b->y; j < (int) b->bm[1]; j++, y++)
		if (b->bm[j + 2] & (1 << i)) {
		    levl[x][y].typ = AIR;
		    levl[x][y].lit = 1;
		    unblock_point(x,y);
		}

	/* replace contents of bubble */
	for (cons = b->cons; cons; cons = ctemp) {
	    ctemp = cons->next;
	    cons->x += dx;
	    cons->y += dy;

	    switch(cons->what) {
		case CONS_OBJ: {
		    struct obj *olist, *otmp;

		    for (olist=(struct obj *)cons->list; olist; olist=otmp) {
			otmp = olist->nexthere;
			place_object(olist, cons->x, cons->y);
		    }
		    break;
		}

		case CONS_MON: {
		    struct monst *mon = (struct monst *) cons->list;
		    (void) mnearto(mon, cons->x, cons->y, TRUE);
		    break;
		}

		case CONS_HERO: {
		    int ux0 = u.ux, uy0 = u.uy;

		    /* change u.ux0 and u.uy0? */
		    u.ux = cons->x;
		    u.uy = cons->y;
		    newsym(ux0, uy0);	/* clean up old position */

		    if (MON_AT(cons->x, cons->y)) {
				mnexto(m_at(cons->x,cons->y));
			}
                        /* WAC removed this.  The ball and chain is moved
                         * as a CONS_OBJECT by the bubble
                         */
#if 0
                    if (Punished) placebc();    /* do this for now */
#endif
		    break;
		}

		case CONS_TRAP: {
		    struct trap *btrap = (struct trap *) cons->list;
		    btrap->tx = cons->x;
		    btrap->ty = cons->y;
		    break;
		}

		default:
		    impossible("mv_bubble: unknown bubble contents");
		    break;
	    }
	    free((void *)cons);
	}
	b->cons = 0;

	/* boing? */

	switch (colli) {
	    case 1: b->dy = -b->dy;	break;
	    case 3: b->dy = -b->dy;	/* fall through */
	    case 2: b->dx = -b->dx;	break;
	    default:
		/* sometimes alter direction for fun anyway
		   (higher probability for stationary bubbles) */
		if (!ini && ((b->dx || b->dy) ? !rn2(20) : !rn2(5))) {
			b->dx = 1 - rn2(3);
			b->dy = 1 - rn2(3);
		}
	}
}

/*mkmaze.c*/
