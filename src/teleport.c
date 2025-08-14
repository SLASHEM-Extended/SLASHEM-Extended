/*	SCCS Id: @(#)teleport.c	3.4	2003/08/11	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

STATIC_DCL boolean tele_jump_ok(int,int,int,int);
STATIC_DCL boolean teleokX(int,int,BOOLEAN_P);
STATIC_DCL boolean teleokXconfused(int,int,BOOLEAN_P);
STATIC_DCL void vault_tele(void);
STATIC_DCL boolean rloc_pos_ok(int,int,struct monst *);
STATIC_DCL void mvault_tele(struct monst *);

/*
 * Is (x, y) a bad position of mtmp?  If mtmp is NULL, then is (x, y) bad
 * for an object?
 *
 * Caller is responsible for checking (x, y) with isok() if required.
 *
 * Returns: -1: Inaccessible, 0: Good pos, 1: Temporally inacessible
 */
static int
badpos(x, y, mtmp, gpflags)
int x, y;
struct monst *mtmp;
unsigned gpflags;
{
	int is_badpos = 0, pool, crystalpool;
	struct permonst *mdat = NULL;
	boolean ignorewater = ((gpflags & MM_IGNOREWATER) != 0);
	boolean crystalornot = ((gpflags & MM_CRYSTALORNOT) != 0);
	struct monst *mtmp2;

	/* in many cases, we're trying to create a new monster, which
	 * can't go on top of the player or any existing monster.
	 * however, occasionally we are relocating engravings or objects,
	 * which could be co-located and thus get restricted a bit too much.
	 * oh well.
	 */
	if (mtmp != &youmonst && x == u.ux && y == u.uy && (!u.usteed || mtmp != u.usteed) )
	    is_badpos = 1;

	if (mtmp) {
	    mtmp2 = m_at(x,y);

	    /* Be careful with long worms.  A monster may be placed back in
	     * its own location.  Normally, if m_at() returns the same monster
	     * that we're trying to place, the monster is being placed in its
	     * own location.  However, that is not correct for worm segments,
	     * because all the segments of the worm return the same m_at().
	     * Actually we overdo the check a little bit--a worm can't be placed
	     * in its own location, period.  If we just checked for mtmp->mx
	     * != x || mtmp->my != y, we'd miss the case where we're called
	     * to place the worm segment and the worm's head is at x,y.
	     */
	    if (mtmp2 && (mtmp2 != mtmp || mtmp->wormno))
		is_badpos = 1;

	    mdat = mtmp->data;
	    pool = (is_waterypool(x,y) && !is_crystalwater(x,y));

	    if (crystalornot) pool = (is_drowningpool(x,y) && !is_crystalwater(x,y));

	    crystalpool = is_crystalwater(x,y);
	    if (mdat->mlet == S_EEL && !pool && rn2(13) && !ignorewater)
		is_badpos = 1;

		if ((mtmp == &youmonst) && (Flying || Levitation) && crystalpool) return -1;

	    if (pool && !ignorewater && !(crystalornot && (Flying || Levitation || Wwalking || Race_if(PM_KORONST)) ) ) {

		if (mtmp == &youmonst)
			return (HLevitation || Flying || Wwalking || Race_if(PM_KORONST) ||
				    Swimming || Amphibious) ? is_badpos : -1;
		else	return (is_flyer(mdat) || is_swimmer(mdat) ||
				    is_clinger(mdat)) ? is_badpos : -1;
		/* note by Amy: the egotype check causes eternal phantom bugs in this function. According to FIQ, mtmp is
		 * never initialized correctly due to being a filler monster, instead of the actual one teleported. */

	    } else if (is_lava(x,y)) {
		if (mtmp == &youmonst)
		    return HLevitation ? is_badpos : -1;
		else
		    return (is_flyer(mdat) || likes_lava(mdat)) ?
			    is_badpos : -1;
	    }
	    if (passes_walls(mdat) && may_passwall(x,y)) return is_badpos;
	    if (u.roommatehack) {
			return is_badpos;
	    }
	}
	if (!ACCESSIBLE(levl[x][y].typ) ) {
		if (!is_farmland(x,y) && !(is_waterypool(x,y) && !u.roommatehack && (ignorewater || crystalornot))) return -1;
	}

	if (closed_door(x, y) && (!mdat || !amorphous(mdat)))
	    return mdat && (nohands(mdat) || verysmall(mdat)) ? -1 : 1;
	if (sobj_at(BOULDER, x, y) && (!mdat || !throws_rocks(mdat)))
	    return mdat ? -1 : 1;
	if (is_raincloud(x,y) && !crystalornot) return -1;

	return is_badpos;
}

/*
 * Is (x,y) a good position of mtmp?  If mtmp is NULL, then is (x,y) good
 * for an object?
 *
 * This function will only look at mtmp->mdat, so makemon, mplayer, etc can
 * call it to generate new monster positions with fake monster structures.
 */
boolean
goodpos(x, y, mtmp, gpflags)
int x,y;
struct monst *mtmp;
unsigned gpflags;
{
    if (!isok(x, y)) return FALSE;

    return !badpos(x, y, mtmp, gpflags);
}

/*
 * "entity next to"
 *
 * Attempt to find a good place for the given monster type in the closest
 * position to (xx,yy).  Do so in successive square rings around (xx,yy).
 * If there is more than one valid positon in the ring, choose one randomly.
 * Return TRUE and the position chosen when successful, FALSE otherwise.
 */
boolean
enexto(cc, xx, yy, mdat)
coord *cc;
register xchar xx, yy;
struct permonst *mdat;
{
	return enexto_core(cc, xx, yy, mdat, 0);
}

boolean
enexto_core(cc, xx, yy, mdat, entflags)
coord *cc;
register xchar xx, yy;
struct permonst *mdat;
unsigned entflags;
{
#define MAX_GOOD 15
    coord good[MAX_GOOD], *good_ptr;
    int x, y, range, i;
    int xmin, xmax, ymin, ymax;
    struct monst fakemon;	/* dummy monster */

    if (!mdat) {
#ifdef DEBUG
	pline("enexto() called with mdat==0");
#endif
	/* default to player's original monster type */
	mdat = &mons[u.umonster];
    }
    fakemon.data = mdat;	/* set up for goodpos */
    good_ptr = good;
    range = 1;
    /*
     * Walk around the border of the square with center (xx,yy) and
     * radius range.  Stop when we find at least one valid position.
     */
    do {
	xmin = max(1, xx-range);
	xmax = min(COLNO-1, xx+range);
	ymin = max(0, yy-range);
	ymax = min(ROWNO-1, yy+range);

	for (x = xmin; x <= xmax; x++)
	    if (goodpos(x, ymin, &fakemon, entflags)) {
		good_ptr->x = x;
		good_ptr->y = ymin ;
		/* beware of accessing beyond segment boundaries.. */
		if (good_ptr++ == &good[MAX_GOOD-1]) goto full;
	    }
	for (x = xmin; x <= xmax; x++)
	    if (goodpos(x, ymax, &fakemon, entflags)) {
		good_ptr->x = x;
		good_ptr->y = ymax ;
		/* beware of accessing beyond segment boundaries.. */
		if (good_ptr++ == &good[MAX_GOOD-1]) goto full;
	    }
	for (y = ymin+1; y < ymax; y++)
	    if (goodpos(xmin, y, &fakemon, entflags)) {
		good_ptr->x = xmin;
		good_ptr-> y = y ;
		/* beware of accessing beyond segment boundaries.. */
		if (good_ptr++ == &good[MAX_GOOD-1]) goto full;
	    }
	for (y = ymin+1; y < ymax; y++)
	    if (goodpos(xmax, y, &fakemon, entflags)) {
		good_ptr->x = xmax;
		good_ptr->y = y ;
		/* beware of accessing beyond segment boundaries.. */
		if (good_ptr++ == &good[MAX_GOOD-1]) goto full;
	    }
	range++;

	/* return if we've grown too big (nothing is valid) */
	if (range > ROWNO && range > COLNO) return FALSE;
    } while (good_ptr == good);

full:
    i = rn2((int)(good_ptr - good));
    cc->x = good[i].x;
    cc->y = good[i].y;
    return TRUE;
}

/*
 * "entity path to"
 *
 * Attempt to find nc good places for the given monster type with the shortest
 * path to (xx,yy).  Where there is more than one valid set of positions, one
 * will be chosen at random.  Return the number of positions found.
 * Warning:  This routine is much slower than enexto and should be used
 * with caution.
 */

#define EPATHTO_UNSEEN		0x0
#define EPATHTO_INACCESSIBLE	0x1
#define EPATHTO_DONE		0x2
#define EPATHTO_TAIL(n)		(0x3 + ((n) & 1))

#define EPATHTO_XY(x,y)		(((y) + 1) * COLNO + (x))
#define EPATHTO_Y(xy)		((xy) / COLNO - 1)
#define EPATHTO_X(xy)		((xy) % COLNO)

#ifdef DEBUG
coord epathto_debug_cc[100];
#endif

int
epathto(cc, nc, xx, yy, mdat)
coord *cc;
int nc;
register xchar xx, yy;
struct permonst *mdat;
{
    int i, j, dir, ndirs, xy, x, y, r;
    int path_len, postype;
    int first_col, last_col;
    int nd, n;
    unsigned char *map;
    static const int dirs[8] =
      /* N, S, E, W, NW, NE, SE, SW */
      { -COLNO, COLNO, 1, -1, -COLNO-1, -COLNO+1, COLNO+1, COLNO-1};
    struct monst fakemon;	/* dummy monster */
    fakemon.data = mdat;	/* set up for badpos */
    map = (unsigned char *)alloc(COLNO * (ROWNO + 2));
    (void) memset((void *)map, EPATHTO_INACCESSIBLE, COLNO * (ROWNO + 2));
    for(i = 1; i < COLNO; i++)
	for(j = 0; j < ROWNO; j++)
	    map[EPATHTO_XY(i, j)] = EPATHTO_UNSEEN;
    map[EPATHTO_XY(xx, yy)] = EPATHTO_TAIL(0);
    if (badpos(xx, yy, &fakemon, 0) == 0) {
	cc[0].x = xx;
	cc[0].y = yy;
	nd = n = 1;
    }
    else
	nd = n = 0;
    for(path_len = 0; nd < nc; path_len++)
    {
	first_col = max(1, xx - path_len);
	last_col = min(COLNO - 1, xx + path_len);
	for(j = max(0, yy - path_len); j <= min(ROWNO - 1, yy + path_len); j++)
	    for(i = first_col; i <= last_col; i++)
		if (map[EPATHTO_XY(i, j)] == EPATHTO_TAIL(path_len)) {
		    map[EPATHTO_XY(i, j)] = EPATHTO_DONE;
		    ndirs = (isgridbug(mdat)) ? 4 : 8;
		    for(dir = 0; dir < ndirs; dir++) {
			xy = EPATHTO_XY(i, j) + dirs[dir];
			if (map[xy] == EPATHTO_UNSEEN) {
			    x = EPATHTO_X(xy);
			    y = EPATHTO_Y(xy);
			    postype = badpos(x, y, &fakemon, 0);
			    map[xy] = postype < 0 ? EPATHTO_INACCESSIBLE :
				    EPATHTO_TAIL(path_len + 1);
			    if (postype == 0) {
				if (n < nc)
				{
				    cc[n].x = x;
				    cc[n].y = y;
				}
				else if (rn2(n - nd + 1) < nc - nd)
				{
				    r = rn2(nc - nd) + nd;
				    cc[r].x = x;
				    cc[r].y = y;
				}
				++n;
			    }
			}
		    }
		}
	if (nd == n)
	    break;	/* No more positions */
	else
	    nd = n;
    }
    if (nd > nc)
	nd = nc;
#ifdef DEBUG
    if (cc == epathto_debug_cc)
    {
	winid win;
	int glyph;
	char row[COLNO+1];

	win = create_nhwindow(NHW_TEXT);
	putstr(win, 0, "");
	for (y = 0; y < ROWNO; y++) {
	    for (x = 1; x < COLNO; x++) {
		xy = EPATHTO_XY(x, y);
		if (map[xy] == EPATHTO_INACCESSIBLE) {
		    glyph = back_to_glyph(x, y);
		    row[x] = showsyms[glyph_to_cmap(glyph)];
		}
		else
		    row[x] = ' ';
	    }
	    for (i = 0; i < nd; i++)
		if (cc[i].y == y)
		    row[cc[i].x] = i < 10 ? '0' + i :
			i < 36 ? 'a' + i - 10 :
			i < 62 ? 'A' + i - 36 :
			'?';
	    /* remove trailing spaces */
	    for (x = COLNO-1; x >= 1; x--)
		if (row[x] != ' ') break;
	    row[x+1] = '\0';

	    putstr(win, 0, &row[1]);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
    }
#endif

    free((void *)map);
    return nd;
}

/*
 * func should return 1 if the location should be counted as inaccessible
 * (path won't continue through this point) or 0 if it is accessible.
 */

void
xpathto(r, xx, yy, func, data)
int r;
register xchar xx, yy;
int (*func)(void *, int, int);
void * data;
{
    int i, j, dir, xy, x, y;
    int path_len, postype;
    int first_col, last_col;
    int nd, n;
    unsigned char *map;
    static const int dirs[8] =
      /* N, S, E, W, NW, NE, SE, SW */
      { -COLNO, COLNO, 1, -1, -COLNO-1, -COLNO+1, COLNO+1, COLNO-1};
    map = (unsigned char *)alloc(COLNO * (ROWNO + 2));
    (void) memset((void *)map, EPATHTO_INACCESSIBLE, COLNO * (ROWNO + 2));
    for(i = 1; i < COLNO; i++)
	for(j = 0; j < ROWNO; j++)
	    map[EPATHTO_XY(i, j)] = EPATHTO_UNSEEN;
    map[EPATHTO_XY(xx, yy)] = EPATHTO_TAIL(0);
    if (func(data, xx, yy) == 0)
	nd = n = 1;
    else
	nd = n = 0;
    for(path_len = 0; path_len < r; path_len++)
    {
	first_col = max(1, xx - path_len);
	last_col = min(COLNO - 1, xx + path_len);
	for(j = max(0, yy - path_len); j <= min(ROWNO - 1, yy + path_len); j++)
	    for(i = first_col; i <= last_col; i++)
		if (map[EPATHTO_XY(i, j)] == EPATHTO_TAIL(path_len)) {
		    map[EPATHTO_XY(i, j)] = EPATHTO_DONE;
		    for(dir = 0; dir < 8; dir++) {
			xy = EPATHTO_XY(i, j) + dirs[dir];
			if (map[xy] == EPATHTO_UNSEEN) {
			    x = EPATHTO_X(xy);
			    y = EPATHTO_Y(xy);
			    postype = func(data, x, y);
			    map[xy] = postype ? EPATHTO_INACCESSIBLE :
				    EPATHTO_TAIL(path_len + 1);
			    if (postype == 0)
				++n;
			}
		    }
		}
	if (nd == n)
	    break;	/* No more positions */
	else
	    nd = n;
    }
    free((void *)map);
}

#ifdef DEBUG
void
wiz_debug_cmd() /* in this case, run epathto on arbitary monster & goal */
{
    struct permonst *ptr;
    int mndx, i;
    coord cc;
    char buf[BUFSIZ];
    for(i = 0; ; i++) {
	if(i >= 5) {
	    pline(thats_enough_tries);
	    return;
	}
	getlin("What monster do you want to test? [type the name]", buf);

	mndx = name_to_mon(buf);
	if (mndx == NON_PM) {
	    pline("Such creatures do not exist in this world.");
	    continue;
	}
	ptr = &mons[mndx];
	pline("Which position do you want to aim for?");
	cc.x = u.ux;
	cc.y = u.uy;
	if (getpos(&cc, TRUE, "the goal position") < 0)
	    return;	/* abort */
	epathto(epathto_debug_cc, SIZE(epathto_debug_cc), cc.x, cc.y, ptr);
	break;
    }
}
#endif	/* DEBUG */

/*
 * Check for restricted areas present in some special levels.  (This might
 * need to be augmented to allow deliberate passage in wizard mode, but
 * only for explicitly chosen destinations.)
 */
STATIC_OVL boolean
tele_jump_ok(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
	if (dndest.nlx > 0) {
	    /* if inside a restricted region, can't teleport outside */
	    if (within_bounded_area(x1, y1, dndest.nlx, dndest.nly,
						dndest.nhx, dndest.nhy) &&
		!within_bounded_area(x2, y2, dndest.nlx, dndest.nly,
						dndest.nhx, dndest.nhy))
# ifdef WIZARD
                if (!wizard)
# endif /* WIZARD */
		return FALSE;
	    /* and if outside, can't teleport inside */
	    if (!within_bounded_area(x1, y1, dndest.nlx, dndest.nly,
						dndest.nhx, dndest.nhy) &&
		within_bounded_area(x2, y2, dndest.nlx, dndest.nly,
						dndest.nhx, dndest.nhy))
# ifdef WIZARD
                if (!wizard)
# endif /* WIZARD */
		return FALSE;
	}
	if (updest.nlx > 0) {		/* ditto */
	    if (within_bounded_area(x1, y1, updest.nlx, updest.nly,
						updest.nhx, updest.nhy) &&
		!within_bounded_area(x2, y2, updest.nlx, updest.nly,
						updest.nhx, updest.nhy))
# ifdef WIZARD
                if (!wizard)
# endif /* WIZARD */
		return FALSE;
	    if (!within_bounded_area(x1, y1, updest.nlx, updest.nly,
						updest.nhx, updest.nhy) &&
		within_bounded_area(x2, y2, updest.nlx, updest.nly,
						updest.nhx, updest.nhy))
# ifdef WIZARD
                if (!wizard)
# endif /* WIZARD */
		return FALSE;
	}
	return TRUE;
}

boolean
teleok(x, y, trapok)
register int x, y;
boolean trapok;
{
	if (!trapok && t_at(x, y)) return FALSE;
	if (!goodpos(x, y, &youmonst, 0)) return FALSE;

	/* In Soviet Russia, water is considered safe as long as you can swim, because hehehe. --Amy */

	if (is_waterypool(x, y) && !(HLevitation || Flying || Wwalking || Race_if(PM_KORONST) || (issoviet && (Swimming || Amphibious) ) )) return FALSE;
	if (is_watertunnel(x,y) && (Levitation || Flying) && !Passes_walls) return FALSE;
	if (is_watertunnel(x,y) && !(Levitation || Flying || (issoviet && (Swimming || Amphibious) ))) return FALSE;
	if (is_farmland(x,y) && !Levitation && !Flying && !(uarmf && uarmf->oartifact == ART_PURPLE_JUNGLE) && !(uarmf && uarmf->oartifact == ART_UTE_S_GREENCHANGE) && !(ublindf && ublindf->oartifact == ART_FREEBOUND) && !(uarm && uarm->oartifact == ART_TILLING_FIELDS) && !(uwep && uwep->oartifact == ART_GARY_S_RIVALRY) && !(uwep && uwep->oartifact == ART_REAL_WALKING) && !(u.usteed && u.usteed->data->mlet == S_QUADRUPED) && !(Upolyd && youmonst.data->mlet == S_QUADRUPED)) return FALSE;

	if (!tele_jump_ok(u.ux, u.uy, x, y)) return FALSE;
	if (!in_out_region(x, y)) return FALSE;
	return TRUE;
}

boolean
teleok_normalterrain(x, y, trapok)
register int x, y;
boolean trapok;
{
	if (!trapok && t_at(x, y)) return FALSE;
	if (!goodpos(x, y, &youmonst, 0)) return FALSE;

	/* In Soviet Russia, water is considered safe as long as you can swim, because hehehe. --Amy */

	if (is_waterypool(x, y)) return FALSE;
	if (is_watertunnel(x,y)) return FALSE;
	if (!ACCESSIBLE(levl[x][y].typ) ) return FALSE;

	if (closed_door(x, y)) return FALSE;
	if (is_raincloud(x,y)) return FALSE;

	if (!tele_jump_ok(u.ux, u.uy, x, y)) return FALSE;
	if (!in_out_region(x, y)) return FALSE;
	return TRUE;
}

STATIC_OVL boolean
teleokX(x, y, trapok)
register int x, y;
boolean trapok;
{
	int udist = distu(x,y);

	if (!trapok && t_at(x, y)) return FALSE;
	if (!goodpos(x, y, &youmonst, 0)) return FALSE;

	if (is_waterypool(x, y) && !(HLevitation || Flying || Wwalking || Race_if(PM_KORONST))) return FALSE;
	if (is_watertunnel(x,y) && (Levitation || Flying) && !Passes_walls) return FALSE;
	if (is_watertunnel(x,y) && !(Levitation || Flying)) return FALSE;
	if (is_farmland(x,y) && !Levitation && !Flying && !(uarmf && uarmf->oartifact == ART_PURPLE_JUNGLE) && !(uarmf && uarmf->oartifact == ART_UTE_S_GREENCHANGE) && !(ublindf && ublindf->oartifact == ART_FREEBOUND) && !(uarm && uarm->oartifact == ART_TILLING_FIELDS) && !(uwep && uwep->oartifact == ART_GARY_S_RIVALRY) && !(uwep && uwep->oartifact == ART_REAL_WALKING) && !(u.usteed && u.usteed->data->mlet == S_QUADRUPED) && !(Upolyd && youmonst.data->mlet == S_QUADRUPED)) return FALSE;

	if (!tele_jump_ok(u.ux, u.uy, x, y)) return FALSE;
	if (!in_out_region(x, y)) return FALSE;
	if (udist < 3) return FALSE;
	if (udist > 100) return FALSE;

	return TRUE;
}

STATIC_OVL boolean
teleokXconfused(x, y, trapok)
register int x, y;
boolean trapok;
{
	int udist = distu(x,y);

	if (!trapok && t_at(x, y)) return FALSE;
	if (!goodpos(x, y, &youmonst, 0)) return FALSE;

	if (is_waterypool(x, y) && !(HLevitation || Flying || Wwalking || Race_if(PM_KORONST))) return FALSE;
	if (is_watertunnel(x,y) && (Levitation || Flying) && !Passes_walls) return FALSE;
	if (is_watertunnel(x,y) && !(Levitation || Flying)) return FALSE;
	if (is_farmland(x,y) && !Levitation && !Flying && !(uarmf && uarmf->oartifact == ART_PURPLE_JUNGLE) && !(uarmf && uarmf->oartifact == ART_UTE_S_GREENCHANGE) && !(ublindf && ublindf->oartifact == ART_FREEBOUND) && !(uarm && uarm->oartifact == ART_TILLING_FIELDS) && !(uwep && uwep->oartifact == ART_GARY_S_RIVALRY) && !(uwep && uwep->oartifact == ART_REAL_WALKING) && !(u.usteed && u.usteed->data->mlet == S_QUADRUPED) && !(Upolyd && youmonst.data->mlet == S_QUADRUPED)) return FALSE;

	if (!tele_jump_ok(u.ux, u.uy, x, y)) return FALSE;
	if (!in_out_region(x, y)) return FALSE;
	if (udist < 3) return FALSE;
	if (udist > 200) return FALSE;

	return TRUE;
}

void
teleds(nux, nuy, allow_drag)
register int nux,nuy;
boolean allow_drag;
{
	boolean ball_active = (Punished && uball->where != OBJ_FREE),
		ball_still_in_range = FALSE;

	/* If they have to move the ball, then drag if allow_drag is true;
	 * otherwise they are teleporting, so unplacebc().
	 * If they don't have to move the ball, then always "drag" whether or
	 * not allow_drag is true, because we are calling that function, not
	 * to drag, but to move the chain.  *However* there are some dumb
	 * special cases:
	 *    0                          0
	 *   _X  move east       ----->  X_
	 *    @                           @
	 * These are permissible if teleporting, but not if dragging.  As a
	 * result, drag_ball() needs to know about allow_drag and might end
	 * up dragging the ball anyway.  Also, drag_ball() might find that
	 * dragging the ball is completely impossible (ball in range but there's
	 * rock in the way), in which case it teleports the ball on its own.
	 */
	if (ball_active) {
	    if (!carried(uball) && distmin(nux, nuy, uball->ox, uball->oy) <= 2)
		ball_still_in_range = TRUE; /* don't have to move the ball */
	    else {
		/* have to move the ball */
		if (!allow_drag || distmin(u.ux, u.uy, nux, nuy) > 1) {
		    /* we should not have dist > 1 and allow_drag at the same
		     * time, but just in case, we must then revert to teleport.
		     */
		    allow_drag = FALSE;
		    unplacebc();
		}
	    }
	}
	u.utrap = 0;
	setustuck(0);
	u.ux0 = u.ux;
	u.uy0 = u.uy;

	if (hides_under(youmonst.data) || (uarmh && itemhasappearance(uarmh, APP_SECRET_HELMET) ) || (uarmc && uarmc->oartifact == ART_UUU_LOST_TURN) || (uarmf && uarmf->oartifact == ART_WHO_IS_HIDING_THERE_) || (!night() && uarmg && uarmg->oartifact == ART_NIGHTLY_HIGHWAY) || (uarmc && uarmc->oartifact == ART_JANA_S_EXTREME_HIDE_AND_SE))
		u.uundetected = OBJ_AT(nux, nuy);
	else if (is_wagon(nux, nuy))
	    u.uundetected = TRUE;
	else if (youmonst.data->mlet == S_EEL)
		u.uundetected = is_waterypool(nux, nuy);
	else {
		u.uundetected = 0;
		/* mimics stop being unnoticed */
		if (youmonst.data->mlet == S_MIMIC)
		    youmonst.m_ap_type = M_AP_NOTHING;
	}

	if (u.uswallow) {
		u.uswldtim = u.uswallow = 0;
		if (Punished && !ball_active) {
		    /* ensure ball placement, like unstuck */
		    ball_active = TRUE;
		    allow_drag = FALSE;
		}
		docrt();
	}
	if (ball_active) {
	    if (ball_still_in_range || allow_drag) {
		int bc_control;
		xchar ballx, bally, chainx, chainy;
		boolean cause_delay;

		if (drag_ball(nux, nuy, &bc_control, &ballx, &bally,
				    &chainx, &chainy, &cause_delay, allow_drag))
		    move_bc(0, bc_control, ballx, bally, chainx, chainy);
	    }
	}
	/* must set u.ux, u.uy after drag_ball(), which may need to know
	   the old position if allow_drag is true... */
	u.ux = nux;
	u.uy = nuy;
	fill_pit(u.ux0, u.uy0);
	if (ball_active) {
	    if (!ball_still_in_range && !allow_drag)
		placebc();
	}
	initrack(); /* teleports mess up tracking monsters without this */
	update_player_regions();
	/* Move your steed, too */
	if (u.usteed) {
		u.usteed->mx = nux;
		u.usteed->my = nuy;
	}

	/*
	 *  Make sure the hero disappears from the old location.  This will
	 *  not happen if she is teleported within sight of her previous
	 *  location.  Force a full vision recalculation because the hero
	 *  is now in a new location.
	 */
	newsym(u.ux0,u.uy0);
	see_monsters();
	vision_full_recalc = 1;
	nomul(0, 0, FALSE);
	vision_recalc(0);	/* vision before effects */
	spoteffects(TRUE);
	invocation_message();
}

boolean
safe_teleds(allow_drag)
boolean allow_drag;
{
	register int nux, nuy, tcnt = 0;

	do {
		nux = rnd(COLNO-1);
		nuy = rn2(ROWNO);
	} while (!teleok(nux, nuy, (boolean)(tcnt > 200)) && ++tcnt <= 400);

	if (tcnt <= 400) {
		teleds(nux, nuy, allow_drag);
		return TRUE;
	} else
		return FALSE;
}

/* safer teleportation that always ignores stuff like phaseable walls, water and so on --Amy
 * used for e.g. automatic relocation when entering forging chambers, where we really don't want players to end up
 * outside of the actual playing field with sucky odds of getting back (e.g. sokoban level clones) */
boolean
safe_teleds_normalterrain(allow_drag)
boolean allow_drag;
{
	register int nux, nuy, tcnt = 0;

	do {
		nux = rnd(COLNO-1);
		nuy = rn2(ROWNO);
	} while (!teleok_normalterrain(nux, nuy, (boolean)(tcnt > 200)) && ++tcnt <= 4000);

	if (tcnt <= 4000) {
		teleds(nux, nuy, allow_drag);
		return TRUE;
	} else /* still didn't find a place? grudgingly use the regular method then */
		{
			do {
				nux = rnd(COLNO-1);
				nuy = rn2(ROWNO);
			} while (!teleok(nux, nuy, (boolean)(tcnt > 200)) && ++tcnt <= 400);

			if (tcnt <= 400) {
				teleds(nux, nuy, allow_drag);
				return TRUE;
			}

		}

		return FALSE;
}

boolean
safe_teledsPD(confused)
boolean confused;
{
	register int nux, nuy, tcnt, udist, goodspot = 0;
	tcnt = 0;

	if (confused) do {
		nux = rnd(COLNO-1);
		nuy = rn2(ROWNO);
	} while (!teleokXconfused(nux, nuy, (boolean)(tcnt > 200)) && ++tcnt <= 2000 );

	if (!confused) do {
		nux = rnd(COLNO-1);
		nuy = rn2(ROWNO);
	} while (!teleokX(nux, nuy, (boolean)(tcnt > 200)) && ++tcnt <= 2000 );

	if (tcnt <= 2000) {
		/*pline("Phase door destination: %d,%d", nux, nuy);*/
		/* this debug information is no longer needed now --Amy */

		teleds(nux, nuy, 0);
		return TRUE;
	} else {
		/*pline("Phase door failed. Destination would have been: %d,%d", nux, nuy);*/

		return FALSE;
	}
}

boolean
safe_teledsNOTRAP(allow_drag)
boolean allow_drag;
{
	register int nux, nuy, tcnt = 0;

	do {
		nux = rnd(COLNO-1);
		nuy = rn2(ROWNO);
	} while ((!teleok(nux, nuy, (boolean)(tcnt > 200)) || (t_at(nux, nuy)) ) && ++tcnt <= 400);

	if (tcnt <= 400) {
		teleds(nux, nuy, allow_drag);
		return TRUE;
	} else
		return FALSE;
}

STATIC_OVL void
vault_tele()
{
	register struct mkroom *croom = search_special(VAULT);
	coord c;

	if (croom && somexy(croom, &c) && teleok(c.x,c.y,FALSE)) {
		teleds(c.x,c.y,FALSE);
		return;
	}
	tele();
}

boolean
teleport_pet(mtmp, force_it)
register struct monst *mtmp;
boolean force_it;
{
	register struct obj *otmp;

	if (mtmp == u.usteed)
		return (FALSE);

	if (mtmp->mleashed) {
	    otmp = get_mleash(mtmp);
	    if (!otmp) {
		impossible("%s is leashed, without a leash.", Monnam(mtmp));
		goto release_it;
	    }
	    if (otmp->cursed && !force_it) {
		yelp(mtmp);
		return FALSE;
	    } else {
		Your("leash goes slack.");
 release_it:
		m_unleash(mtmp, FALSE);
		return TRUE;
	    }
	}
	return TRUE;
}

void
tele()
{
	coord cc;

	/* Disable teleportation in stronghold && Vlad's Tower */
	if ((level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout) && !Race_if(PM_RODNEYAN) ) {
#ifdef WIZARD
		if (!wizard) {
#endif
		    pline("A mysterious force prevents you from teleporting!");
		    return;
#ifdef WIZARD
		}
#endif
	}

	/* don't show trap if "Sorry..." */
	if (!Blinded) make_blinded(0L,FALSE);

	if
#ifdef WIZARD
        (
#endif
        ((u.uhave.amulet && !u.freeplaymode && (u.amuletcompletelyimbued || !rn2(3))) || CannotTeleport || On_W_tower_level(&u.uz)
	|| (u.usteed && mon_has_amulet(u.usteed))
	)
#ifdef WIZARD
        && (!wizard) )
#endif
	{
	    You_feel("disoriented for a moment.");
	    return;
	}

	/* at this point, you will definitely be teleported... unless something prevents it */

	{
		boolean teletrapactive = FALSE;
		struct monst *teletrapmon;
		for (teletrapmon = fmon; teletrapmon; teletrapmon = teletrapmon->nmon) {
		    if (DEADMONSTER(teletrapmon)) continue;
		    if (teletrapmon->mnum == PM_TELEPORT_TRAP) {
				teletrapactive = TRUE;
				break;
		    }
		}

		if (teletrapactive) {

			int nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
			if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
			int blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
			if (!blackngdur ) blackngdur = 500; /* fail safe */
			randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

			badeffect();
			return;
		}

	}

	if ((Teleport_control && !Stunned && (!level.flags.has_insideroom || !rn2(5)) && rn2(StrongTeleport_control ? 20 : 4)) /* low chance for tele control to fail --Amy */
#ifdef WIZARD
			    || (wizard && yn_function("Invoke wizard-mode teleport control?", ynchars, 'y') == 'y')
#endif
					) {
	    if (unconscious()) {
		pline("Being unconscious, you cannot control your teleport.");
	    } else {
		    char buf[BUFSZ];
		    if (u.usteed) sprintf(buf," and %s", mon_nam(u.usteed));
		    pline("To what position do you%s want to be teleported?", u.usteed ? buf : "");
		    cc.x = u.ux;
		    cc.y = u.uy;
		    if (getpos(&cc, TRUE, "the desired position") < 0)
			return;	/* abort */
		    /* possible extensions: introduce a small error if
		       magic power is low; allow transfer to solid rock */

		    /* Amy edit: teleporting on a trap will trigger it, instead of being invalid. I've specifically made it
		     * so that weird-looking rooms make teleport control likely to fail, but if you spam-spam-spam Ctrl-T to
		     * skip past that and teleport on the pentagram anyway, and there's a trap on it, well... :P */
		    if (teleok(cc.x, cc.y, TRUE)) {
			teleds(cc.x, cc.y, FALSE);
			return;
		    }
		    pline("Sorry...");
		}
	}

	u.cnd_teleportcount++;
	if (uleft && uleft->oartifact == ART_MEGOHEAL) healup(20, 0, 0, 0);
	if (uright && uright->oartifact == ART_MEGOHEAL) healup(20, 0, 0, 0);

	(void) safe_teleds(FALSE);
}

void
teleX()
{
	/* Disable teleportation in stronghold && Vlad's Tower */
	if ((level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout) && !Race_if(PM_RODNEYAN) ) {
		    pline("A mysterious force prevents you from teleporting!");
		    return;
	}

	/* don't show trap if "Sorry..." */
	if (!Blinded) make_blinded(0L,FALSE);

	if
        ((u.uhave.amulet && !u.freeplaymode && (u.amuletcompletelyimbued || !rn2(3))) || CannotTeleport || On_W_tower_level(&u.uz)
	|| (u.usteed && mon_has_amulet(u.usteed))
	)
	{
	    You_feel("disoriented for a moment.");
	    return;
	}
	u.cnd_teleportcount++;
	(void) safe_teleds(FALSE);
}

void
phase_door(confused)
boolean confused;
{
	/* Disable teleportation in stronghold && Vlad's Tower */
	if ((level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout) && !Race_if(PM_RODNEYAN) ) {
		    pline("A mysterious force prevents you from phasing!");
		    return;
	}

	if
        ((u.uhave.amulet && !u.freeplaymode && (u.amuletcompletelyimbued || !rn2(3))) || CannotTeleport || On_W_tower_level(&u.uz)
	|| (u.usteed && mon_has_amulet(u.usteed))
	)
	{
	    You_feel("disoriented for a moment.");
	    return;
	}
#ifdef PUBLIC_SERVER
	pline("Your body is transported to another location!"); /* for debug purposes --Amy */
#endif
	u.cnd_phasedoorcount++;
	(void) safe_teledsPD(confused);
}

/* the player has used the Ctrl-T or "dotele" command, as opposed to automatically trying to teleport after falling into
 * a pool of water or something. This is a very powerful ability that only costs 30 Pw, so there needs to be a downside */
int
dotele()
{
	return dotele_post(TRUE);
}

int
dotele_post(costly)
boolean costly;
{
	struct trap *trap;
	boolean reallycostly = FALSE;

	trap = t_at(u.ux, u.uy);
	if (trap && (!trap->tseen || trap->ttyp != TELEP_TRAP))
		trap = 0;

	if (trap) {
		if (trap->once) {
			pline("This is a vault teleport, usable once only.");
			if (yn("Jump in?") == 'n')
				trap = 0;
			else {
				deltrap(trap);
				newsym(u.ux, u.uy);
			}
		}
		if (trap)
			You("%s onto the teleportation trap.",
			    locomotion(youmonst.data, "jump"));
	}
	if (!trap) {
	    boolean castit = FALSE;
	    register int sp_no = 0, energy = 0;

	    if ((!Teleportation || (u.ulevel < (Race_if(PM_LICH_WARRIOR) ? 1 : Race_if(PM_RODNEYAN) ? 1 : Role_if(PM_WIZARD) ? 8 : 12) && !can_teleport(youmonst.data))) && !(uarmf && uarmf->oartifact == ART_HAWAIIAN_KAMEHAMEHA) && !(uarmh && uarmh->oartifact == ART_TRIP_TERRAIN)) {
		/* Try to use teleport away spell. */
		if (objects[SPE_TELEPORT_AWAY].oc_name_known && !Confusion && !costly) {
		    for (sp_no = 0; sp_no < MAXSPELL; sp_no++) {
			if (spl_book[sp_no].sp_id == SPE_TELEPORT_AWAY) {
				castit = TRUE;
				break;
			}
			if (spl_book[sp_no].sp_id == SPE_TELEPORT_SELF) {
				castit = TRUE;
				break;
			}
		    }
		}
#ifdef WIZARD
		if (!wizard) {
#endif
		    if (!castit) {
			if (!Teleportation)
			    You("don't know that spell.");
			else You("are not able to teleport at will.");
			return(0);
		    }
#ifdef WIZARD
		}
#endif
	    }
	    else if (!wizard) {
		reallycostly = TRUE; /* no trap, and using teleportitis at will */
	    }

	    if ( (u.uhunger <= (costly ? 100 : 30)) || ACURR(A_STR) < 6) {
#ifdef WIZARD
		if (!wizard) {
#endif
			You("lack the strength %s.",
			    castit ? "for a teleport spell" : "to teleport");
			return /*1*/0;
#ifdef WIZARD
		}
#endif
	    }

	    energy = objects[SPE_TELEPORT_AWAY].oc_level * 5; /* even if it was teleport self (arbitrary) --Amy */
	    if (costly) energy = 100;
	    if (powerfulimplants() && uimplant && uimplant->oartifact == ART_KATRIN_S_SUDDEN_APPEARANCE) energy /= 2;
	    if (uarmh && uarmh->oartifact == ART_TRIP_TERRAIN) energy /= 3;
	    if (uarmc && uarmc->oartifact == ART_HOP_QUICKER) energy /= 3;
	    if (u.uen < energy) {
#ifdef WIZARD
		if (wizard)
			energy = u.uen;
		else
#endif
		{
			You("lack the energy %s. You need at least %d.",
			    castit ? "for a teleport spell" : "to teleport", energy);
			return /*1*/0;
		}
	    }

	    /* unless you have double teleportitis, which will be the case very rarely, you'll be unable to control it */
	    if (reallycostly && !StrongTeleportation) u.uprops[DEAC_TELEPORT_CONTROL].intrinsic++;

	    if (check_capacity(
			"Your concentration falters from carrying so much."))
		return 1;

	    if (castit) {
		exercise(A_WIS, TRUE);
		if (spelleffects(sp_no, TRUE))
			return(1);
		else
#ifdef WIZARD
		    if (!wizard)
#endif
			return(0);
	    } else {
		u.uen -= energy;
		flags.botl = 1;
	    }
	}

	if (next_to_u()) {
		if (trap && trap->once) vault_tele();
		else {
			tele();
			/* using teleportitis at will now stuns you, to reduce the player's ability to abuse it --Amy */
			if (reallycostly) make_stunned(HStun + rn1(3,3), TRUE);
		}
		(void) next_to_u();
	} else {
		You("%s", shudder_for_moment);
		return(0);
	}
	if (!trap) morehungry(costly ? 100 : 30);
	return(1);
}

void
level_tele()
{
	register int newlev;
	d_level newlevel;
	const char *escape_by_flying = 0;	/* when surviving dest of -N */
	char buf[BUFSZ];
	boolean force_dest = FALSE;
	if (iflags.debug_fuzzer) goto random_levtport;

	if (((u.uhave.amulet && !u.freeplaymode) || CannotTeleport || In_endgame(&u.uz) || In_sokoban_real(&u.uz) || (Role_if(PM_CAMPERSTRIKER) && In_quest(&u.uz)) || (u.usteed && mon_has_amulet(u.usteed)) )
#ifdef WIZARD
						&& !wizard
#endif
							) {
	    You_feel("very disoriented for a moment.");

		if (uarmh && itemhasappearance(uarmh, APP_WEEPING_HELMET) ) {
			pline("Your helmet does not like the idea of blocked level teleportation!");
			losexp("weeping helmet", TRUE, FALSE);
		    /* This ignores level-drain resistance (not a bug). --Amy */

		}

		if (RngeWeeping) {

			You_hear("faint weeping...");
			losexp("weeping", TRUE, FALSE);

		}

	    return;
	}
	/* Skipping the quest via teleport control is lame. --Amy */
	if ((Teleport_control && !(In_quest(&u.uz)) && !(In_minotaurmaze(&u.uz)) && !Stunned && (!level.flags.has_insideroom || !rn2(5)) && rn2(StrongTeleport_control ? 10 : 3)) /* Teleport control might not always work. --Amy */
#ifdef WIZARD
	   || (wizard && yn_function("Invoke wizard-mode teleport control?", ynchars, 'y') == 'y')
#endif
		) {
	    char qbuf[BUFSZ];
	    int trycnt = 0;

	    strcpy(qbuf, "To what level do you want to teleport?");
	    do {
		if (++trycnt == 2) {
#ifdef WIZARD
			if (wizard) strcat(qbuf, " [type a number or ? for a menu]");
			else
#endif
			strcat(qbuf, " [type a number]");
		}
		getlin(qbuf, buf);
		if (!strcmp(buf,"\033")) {	/* cancelled */
		    if (Confusion && rnl(5)) {
			pline("Uh-oh..."); /* don't make player jump out of their seat with a fake panic message! --Amy */
			goto random_levtport;
		    }
		    return;
		} else if (!strcmp(buf,"*")) {
		    goto random_levtport;
		} else if (Confusion && rnl(5)) {
		    pline("Uh-oh...");
		    goto random_levtport;
		}
#ifdef WIZARD
		if (wizard && !strcmp(buf,"?")) {
		    schar destlev = 0;
		    xchar destdnum = 0;

		    if ((newlev = (int)print_dungeon(TRUE, &destlev, &destdnum))) {
			newlevel.dnum = destdnum;
			newlevel.dlevel = destlev;
			if (In_endgame(&newlevel) && !In_endgame(&u.uz)) {
				sprintf(buf,
				    "Destination is earth level");
				if (!u.uhave.amulet) {
					struct obj *obj;
					obj = mksobj(AMULET_OF_YENDOR, TRUE, FALSE, FALSE);
					if (obj) {
						obj = addinv(obj);
						strcat(buf, " with the amulet");
					}
				}
				assign_level(&newlevel, &earth_level);
				pline("%s.", buf);
			}
			force_dest = TRUE;
		    } else return;
		} else
#endif
		if ((newlev = lev_by_name(buf)) == 0) {
#ifdef WIZARD
		    s_level *slev;

		    /* if you're using wizard mode, you shouldn't really need
		     * the game to interpret things like `mine town level' */

		    if (wizard && (slev = find_level(buf))) {

			schedule_goto(&slev->dlevel, FALSE, FALSE, 0,
				      (char *)0, (char *)0);
			return;
		    }
#endif
		    newlev = atoi(buf);
                }
	    } while (!newlev && !digit(buf[0]) &&
		     (buf[0] != '-' || !digit(buf[1])) &&
		     trycnt < 10);

	    /* no dungeon escape via this route */
	    if (newlev == 0) {
		if (trycnt >= 10)
		    goto random_levtport;
		if (ynq("Go to Nowhere.  Are you sure?") != 'y') return;
		u.youaredead = 1;
		You("%s in agony as your body begins to warp...",
		    is_silent(youmonst.data) ? "writhe" : "scream");
		display_nhwindow(WIN_MESSAGE, FALSE);
		You("cease to exist.");
		if (invent) Your("possessions land on the %s with a thud.",
				surface(u.ux, u.uy));
		killer_format = NO_KILLER_PREFIX;
		killer = "committed suicide";
		done(DIED);
		u.youaredead = 0;
		pline("An energized cloud of dust begins to coalesce.");
		Your("body rematerializes%s.", invent ?
			", and you gather up all your possessions" : "");
		return;
	    }

	    /* if in Knox and the requested level > 0, stay put.
	     * we let negative values requests fall into the "heaven" loop.
	     * [ALI] Add other single level dungeons entered via portal.
	     */
	    if ((Is_knox(&u.uz) ||
		    Is_blackmarket(&u.uz) ||
		    Is_aligned_quest(&u.uz)) && newlev > 0) {
		You("%s", shudder_for_moment);
		return;
	    }
	    /* if in Quest, the player sees "Home 1", etc., on the status
	     * line, instead of the logical depth of the level.  controlled
	     * level teleport request is likely to be relativized to the
	     * status line, and consequently it should be incremented to
	     * the value of the logical depth of the target level.
	     *
	     * we let negative values requests fall into the "heaven" loop.
	     */
	    if (In_quest(&u.uz) && newlev > 0)
		newlev = newlev + dungeons[u.uz.dnum].depth_start - 1;
	} else { /* involuntary level tele */
 random_levtport:
	    newlev = random_teleport_level();
	    if (newlev == depth(&u.uz)) {
		You("%s", shudder_for_moment);
		return;
	    }
	}

	if (!next_to_u()) {
		You("%s", shudder_for_moment);
		return;
	}
#ifdef WIZARD
	if (In_endgame(&u.uz)) {	/* must already be wizard */
	    int llimit = dunlevs_in_dungeon(&u.uz);

	    if (newlev >= 0 || newlev <= -llimit) {
		You_cant("get there from here.");
		return;
	    }
	    newlevel.dnum = u.uz.dnum;
	    newlevel.dlevel = llimit + newlev;
	    schedule_goto(&newlevel, FALSE, FALSE, 0, (char *)0, (char *)0);
	    return;
	}
#endif
	killer = 0;		/* still alive, so far... */

	if (iflags.debug_fuzzer && newlev < 0) goto random_levtport;

	if (newlev < 0 && !force_dest) {
		if (*u.ushops0) {
		    /* take unpaid inventory items off of shop bills */
		    in_mklev = TRUE;	/* suppress map update */
		    u_left_shop(u.ushops0, TRUE);
		    /* you're now effectively out of the shop */
		    *u.ushops0 = *u.ushops = '\0';
		    in_mklev = FALSE;
		}
		if (newlev <= -10) {
			You("arrive in heaven.");
			verbalize("Thou art early, but we'll admit thee.");
			killer_format = NO_KILLER_PREFIX;
			killer = "went to heaven prematurely";
		} else if (newlev == -9) {
			You_feel("deliriously happy. ");
			pline("(In fact, you're on Cloud 9!) ");
			display_nhwindow(WIN_MESSAGE, FALSE);
		} else
			You("are now high above the clouds...");

		if (killer) {
		    ;		/* arrival in heaven is pending */
		} else if (Levitation) {
		    escape_by_flying = "float gently down to earth";
		} else if (Flying) {
		    escape_by_flying = "fly down to the ground";
		} else {
		    pline("Unfortunately, you don't know how to fly.");
		    You("plummet a few thousand feet to your death.");
		    sprintf(buf,
			  "teleported out of the dungeon and fell to %s death",
			    uhis());
		    killer = buf;
		    killer_format = NO_KILLER_PREFIX;
		}
	}

	if (killer) {	/* the chosen destination was not survivable */
	    d_level lsav;

	    /* set specific death location; this also suppresses bones */
	    lsav = u.uz;	/* save current level, see below */
	    u.uz.dnum = 0;	/* main dungeon */
	    u.uz.dlevel = (newlev <= -10) ? -10 : 0;	/* heaven or surface */
	    done(DIED);
	    /* can only get here via life-saving (or declining to die in
	       explore|debug mode); the hero has now left the dungeon... */
	    escape_by_flying = "find yourself back on the surface";
	    u.uz = lsav;	/* restore u.uz so escape code works */
	}

	/* calls done(ESCAPED) if newlevel==0 */
	if (escape_by_flying) {
	    You("%s.", escape_by_flying);
	    newlevel.dnum = 0;		/* specify main dungeon */
	    newlevel.dlevel = 0;	/* escape the dungeon */
	    /* [dlevel used to be set to 1, but it doesn't make sense to
		teleport out of the dungeon and float or fly down to the
		surface but then actually arrive back inside the dungeon] */
	} else if (u.uz.dnum == medusa_level.dnum &&
	    newlev >= dungeons[u.uz.dnum].depth_start +
						dunlevs_in_dungeon(&u.uz)) {
#ifdef WIZARD
	    if (!(wizard && force_dest))
#endif
	    find_hell(&newlevel);
	} else {
	    /* if invocation did not yet occur, teleporting into
	     * the last level of Gehennom is forbidden.
	     */
#ifdef WIZARD
		if (!wizard)
#endif
	    if (In_gehennom(&u.uz) && !u.uevent.invoked &&
			newlev >= (dungeons[u.uz.dnum].depth_start +
					dunlevs_in_dungeon(&u.uz) - 1)) {
# ifdef WIZARD
                if (!wizard)
                {
# endif /* WIZARD */
		newlev = dungeons[u.uz.dnum].depth_start +
					dunlevs_in_dungeon(&u.uz) - 2;
		pline("Sorry...");
# ifdef WIZARD
                }
# endif /* WIZARD */

	    }
	    /* no teleporting out of quest dungeon */
            if
# ifdef WIZARD
                ((!wizard) &&
# endif /* WIZARD */
                (In_quest(&u.uz) && newlev < depth(&qstart_level))
# ifdef WIZARD
                )
# endif /* WIZARD */
		newlev = depth(&qstart_level);
	    /* the player thinks of levels purely in logical terms, so
	     * we must translate newlev to a number relative to the
	     * current dungeon.
	     */
#ifdef WIZARD
	    if (!(wizard && force_dest))
#endif
	    get_level(&newlevel, newlev);
	}
	u.cnd_telelevelcount++;

	schedule_goto(&newlevel, FALSE, FALSE, 0, (char *)0, (char *)0);
	/* in case player just read a scroll and is about to be asked to
	   call it something, we can't defer until the end of the turn */
	if (u.utotype && !flags.mon_moving) deferred_goto();
}

void
domagicportal(ttmp)
register struct trap *ttmp;
{
	struct d_level target_level;

	if (!next_to_u()) {
		You("%s", shudder_for_moment);
		return;
	}

	/* if landed from another portal, do nothing */
	/* problem: level teleport landing escapes the check */
	if (!on_level(&u.uz, &u.uz0)) return;

	You("activated a magic portal!");

	if (!rn2(isfriday ? 3 : 5)) u.stairscumslowing += rn1(5,5);

	/* prevent the poor shnook, whose amulet was stolen while in
	 * the endgame, from accidently triggering the portal to the
	 * next level, and thus losing the game
	 */
	if (In_endgame(&u.uz) && !u.uhave.amulet && !u.freeplaymode) {
	    You_feel("dizzy for a moment, but nothing happens...");
	    return;
	}

	target_level = ttmp->dst;

	/* a slashem bug: it always panics if you're punished on water... should be fixed now --Amy */
	/*if (In_endgame(&u.uz) && Punished && Is_firelevel(&u.uz) ) {
	    You_feel("the iron ball preventing you from proceeding...");
	    return;
	}*/

	schedule_goto(&target_level, FALSE, FALSE, 1,
		      "You feel dizzy for a moment, but the sensation passes.",
		      (char *)0);

}

void
tele_trap(trap)
struct trap *trap;
{
	if (In_endgame(&u.uz) || Antimagic) {
		if (Antimagic)
			shieldeff(u.ux, u.uy);
		You_feel("a wrenching sensation.");
	} else if (!next_to_u()) {
		You("%s", shudder_for_moment);
	} else if (trap->once) {
		You("%s onto a vault teleporter!",
		      Levitation ? (const char *)"float" :
				  locomotion(youmonst.data, "step"));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		deltrap(trap);
		newsym(u.ux,u.uy);	/* get rid of trap symbol */
		vault_tele();
	} else {
		You("%s onto a teleport trap!",
		      Levitation ? (const char *)"float" :
				  locomotion(youmonst.data, "step"));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		tele();
		}
}

/* beamer trap - ignores magic resistance --Amy */
void
tele_trapX(trap)
struct trap *trap;
{
	if (In_endgame(&u.uz)) {
		You_feel("a wrenching sensation.");
	} else if (!next_to_u()) {
		You("%s", shudder_for_moment);
	} else if (trap->once) {
		You("%s onto a vault teleporter!",
		      Levitation ? (const char *)"float" :
				  locomotion(youmonst.data, "step"));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		deltrap(trap);
		newsym(u.ux,u.uy);	/* get rid of trap symbol */
		vault_tele();
	} else {
		You("%s onto a teleport trap!",
		      Levitation ? (const char *)"float" :
				  locomotion(youmonst.data, "step"));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		tele();
		}
}

void
level_tele_trap(trap)
struct trap *trap;
{
	You("%s onto a level teleport trap!",
		      Levitation ? (const char *)"float" :
				  locomotion(youmonst.data, "step"));
	if (Antimagic) {
	    shieldeff(u.ux, u.uy);
	}
	if (Antimagic || In_endgame(&u.uz)) {
	    You_feel("a wrenching sensation.");
	    return;
	}
	if (!Blind) {
	    You("are momentarily blinded by a flash of light.");
		if (evilfriday) make_blinded(Blinded+rnz(100),FALSE);
	} else
	    You("are momentarily disoriented.");
	deltrap(trap);
	newsym(u.ux,u.uy);	/* get rid of trap symbol */
      if (!playerlevelportdisabled()) level_tele();
	else pline("The trap doesn't seem to have any effect on you.");
}

/* level beamer: ignores magic resistance --Amy */
void
level_tele_trapX(trap)
struct trap *trap;
{
	You("%s onto a level teleport trap!",
		      Levitation ? (const char *)"float" :
				  locomotion(youmonst.data, "step"));
	if (In_endgame(&u.uz)) {
	    You_feel("a wrenching sensation.");
	    return;
	}
	if (!Blind) {
	    You("are momentarily blinded by a flash of light.");
		if (evilfriday) make_blinded(Blinded+rnz(100),FALSE);
	} else
	    You("are momentarily disoriented.");
	deltrap(trap);
	newsym(u.ux,u.uy);	/* get rid of trap symbol */
      if (!playerlevelportdisabled()) level_tele();
	else pline("The trap doesn't seem to have any effect on you.");
}

/* check whether monster can arrive at location <x,y> via Tport (or fall) */
STATIC_OVL boolean
rloc_pos_ok(x, y, mtmp)
register int x, y;		/* coordinates of candidate location */
struct monst *mtmp;
{
	register int xx, yy;

	if (!goodpos(x, y, mtmp, 0)) return FALSE;
	/*
	 * Check for restricted areas present in some special levels.
	 *
	 * `xx' is current column; if 0, then `yy' will contain flag bits
	 * rather than row:  bit #0 set => moving upwards; bit #1 set =>
	 * inside the Wizard's tower.
	 */
	xx = mtmp->mx;
	yy = mtmp->my;
	if (!xx) {
	    /* no current location (migrating monster arrival) */
	    if (dndest.nlx && On_W_tower_level(&u.uz))
		return ((yy & 2) != 0) ^	/* inside xor not within */
		       !within_bounded_area(x, y, dndest.nlx, dndest.nly,
						  dndest.nhx, dndest.nhy);
	    if (updest.lx && (yy & 1) != 0)	/* moving up */
		return (within_bounded_area(x, y, updest.lx, updest.ly,
						  updest.hx, updest.hy) &&
		       (!updest.nlx ||
			!within_bounded_area(x, y, updest.nlx, updest.nly,
						   updest.nhx, updest.nhy)));
	    if (dndest.lx && (yy & 1) == 0)	/* moving down */
		return (within_bounded_area(x, y, dndest.lx, dndest.ly,
						  dndest.hx, dndest.hy) &&
		       (!dndest.nlx ||
			!within_bounded_area(x, y, dndest.nlx, dndest.nly,
						   dndest.nhx, dndest.nhy)));
	} else {
	    /* current location is <xx,yy> */
	    if (!tele_jump_ok(xx, yy, x, y)) return FALSE;
	}
	/* <x,y> is ok */
	return TRUE;
}

/*
 * rloc_to()
 *
 * Pulls a monster from its current position and places a monster at
 * a new x and y.  If oldx is 0, then the monster was not in the levels.monsters
 * array.  However, if oldx is 0, oldy may still have a value because mtmp is a
 * migrating_mon.  Worm tails are always placed randomly around the head of
 * the worm.
 */
void
rloc_to(mtmp, x, y)
struct monst *mtmp;
register int x, y;
{
	register int oldx = mtmp->mx, oldy = mtmp->my;
	boolean resident_shk = mtmp->isshk && inhishop(mtmp);

	if (x == mtmp->mx && y == mtmp->my)	/* that was easy */
		return;

	if (oldx) {				/* "pick up" monster */
	    if (mtmp->wormno)
		remove_worm(mtmp);
	    else {
		remove_monster(oldx, oldy);
		newsym(oldx, oldy);		/* update old location */
	    }
	}

	place_monster(mtmp, x, y);		/* put monster down */
	update_monster_region(mtmp);

	if (mtmp->wormno)			/* now put down tail */
		place_worm_tail_randomly(mtmp, x, y);

	if (u.ustuck == mtmp) {
		if (u.uswallow) {
			u.ux = x;
			u.uy = y;
			docrt();
		} else	setustuck(0);
	}

	newsym(x, y);				/* update new location */
	set_apparxy(mtmp);			/* orient monster */

	/* shopkeepers will only teleport if you zap them with a wand of
	   teleportation or if they've been transformed into a jumpy monster;
	   the latter only happens if you've attacked them with polymorph
	   Amy edit: that is no longer the case, so this shouldn't make them hostile anymore! */
	if (resident_shk && issoviet && !inhishop(mtmp)) {
		make_angry_shk(mtmp, oldx, oldy);
		pline("VA-KHA-KHA-KHA, lavochnik ub'yet tebya.");
	}
}

/* place a monster at a random location, typically due to teleport */
/* return TRUE if successful, FALSE if not */
boolean
rloc(mtmp, suppress_impossible)
struct monst *mtmp;	/* mx==0 implies migrating monster arrival */
boolean suppress_impossible;
{
	register int x, y, trycount;

	if (mtmp == u.usteed) {
	    tele();
	    return TRUE;
	}

	if (mtmp->iswiz && mtmp->mx) {	/* Wizard, not just arriving */
	    if (!In_W_tower(u.ux, u.uy, &u.uz))
		x = xupstair,  y = yupstair;
	    else if (!xdnladder)	/* bottom level of tower */
		x = xupladder,  y = yupladder;
	    else
		x = xdnladder,  y = ydnladder;
	    /* if the wiz teleports away to heal, try the up staircase,
	       to block the player's escaping before he's healed
	       (deliberately use `goodpos' rather than `rloc_pos_ok' here) */
	    if (goodpos(x, y, mtmp, 0))
		goto found_xy;
	}

	trycount = 0;
	do {
	    x = rn1(COLNO-3,2);
	    y = rn2(ROWNO);
	    if ((trycount < 500) ? rloc_pos_ok(x, y, mtmp)
				 : goodpos(x, y, mtmp, 0))
		goto found_xy;
	} while (++trycount < 1000);

	/* last ditch attempt to find a good place */
	for (x = 2; x < COLNO - 1; x++)
	    for (y = 0; y < ROWNO; y++)
		if (goodpos(x, y, mtmp, 0))
		    goto found_xy;

	/* level either full of monsters or somehow faulty */
	if (!suppress_impossible)
		impossible("rloc(): couldn't relocate monster");
	return FALSE;

 found_xy:
	rloc_to(mtmp, x, y);
	return TRUE;
}

STATIC_OVL void
mvault_tele(mtmp)
struct monst *mtmp;
{
	register struct mkroom *croom = search_special(VAULT);
	coord c;

	if (croom && somexy(croom, &c) &&
				goodpos(c.x, c.y, mtmp, 0)) {
		rloc_to(mtmp, c.x, c.y);
		return;
	}
	(void) rloc(mtmp, FALSE);
}

boolean
tele_restrict(mon)
struct monst *mon;
{
	if (level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout) {
		if (canseemon(mon))
		    pline("A mysterious force prevents %s from teleporting!",
			mon_nam(mon));
		return TRUE;
	}
	return FALSE;
}

void
mtele_trap(mtmp, trap, in_sight)
struct monst *mtmp;
struct trap *trap;
int in_sight;
{
	char *monname;

	if (tele_restrict(mtmp)) return;
	if (teleport_pet(mtmp, FALSE)) {
	    /* save name with pre-movement visibility */
	    monname = Monnam(mtmp);

	    /* Note: don't remove the trap if a vault.  Other-
	     * wise the monster will be stuck there, since
	     * the guard isn't going to come for it...
	     */
	    if (trap->once) mvault_tele(mtmp);
	    else (void) rloc(mtmp, FALSE);

	    if (in_sight) {
		if (canseemon(mtmp))
		    pline("%s seems disoriented.", monname);
		else
		    pline("%s suddenly disappears!", monname);
		seetrap(trap);
	    }
	}
}

/* return 0 if still on level, 3 if not */
int
mlevel_tele_trap(mtmp, trap, force_it, in_sight)
struct monst *mtmp;
struct trap *trap;
boolean force_it;
int in_sight;
{
	int tt = trap->ttyp;
	struct permonst *mptr = mtmp->data;

	if (mtmp == u.ustuck)	/* probably a vortex */
	    return 0;		/* temporary? kludge */
	if (teleport_pet(mtmp, force_it)) {
	    d_level tolevel;
	    int migrate_typ = MIGR_RANDOM;

	    if ((tt == HOLE || tt == TRAPDOOR || tt == SHAFT_TRAP || tt == CURRENT_SHAFT)) {
		if (Is_stronghold(&u.uz)) {
		    assign_level(&tolevel, &valley_level);
		} else if (Is_botlevel(&u.uz)) {
		    if (in_sight && trap->tseen)
			pline("%s avoids the %s.", Monnam(mtmp),
			(tt == HOLE) ? "hole" : "trap");
		    return 0;
		} else {
		    get_level(&tolevel, depth(&u.uz) + 1);
		}
	    } else if (tt == MAGIC_PORTAL) {
		if (In_endgame(&u.uz) &&
		    (mon_has_amulet(mtmp) || is_home_elemental(mptr))) {
		    if (in_sight && mptr->mlet != S_ELEMENTAL) {
			pline("%s seems to shimmer for a moment.",
							Monnam(mtmp));
			seetrap(trap);
		    }
		    return 0;
		}
	      	else if (mtmp->mtame &&
			(Is_blackmarket(&trap->dst) || Is_blackmarket(&u.uz))) {
	          if (in_sight) {
		     pline("%s seems to shimmer for a moment.",
		     Monnam(mtmp));
		     seetrap(trap);
	          }
	          return 0;
	        }
		  else {
		    assign_level(&tolevel, &trap->dst);
		    migrate_typ = MIGR_PORTAL;
		}
	    } else { /* (tt == LEVEL_TELEP) */
		int nlev;

		if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
		    if (in_sight)
			pline("%s seems very disoriented for a moment.",
				Monnam(mtmp));
		    return 0;
		}
		nlev = random_teleport_level();
		if (nlev == depth(&u.uz)) {
		    if (in_sight)
			pline("%s shudders for a moment.", Monnam(mtmp));
		    return 0;
		}
		get_level(&tolevel, nlev);
	    }

	    if (in_sight) {
		pline("Suddenly, %s disappears out of sight.", mon_nam(mtmp));
		seetrap(trap);
	    }
	    migrate_to_level(mtmp, ledger_no(&tolevel),
			     migrate_typ, (coord *)0);
	    return 3;	/* no longer on this level */
	}
	return 0;
}


void
rloco(obj)
register struct obj *obj;
{
	register xchar tx, ty, otx, oty;
	boolean restricted_fall;
	int try_limit = 4000;

	if (obj->dynamitekaboom) return;
	if (obj->mstartinventX) return;

	if (obj->otyp == CORPSE && is_rider(&mons[obj->corpsenm])) {
	    if (revive_corpse(obj, FALSE)) return;
	}
	if (obj->otyp == CORPSE && is_deadlysin(&mons[obj->corpsenm])) {
	    if (revive_corpse(obj, FALSE)) return;
	}

	obj_extract_self(obj);
	otx = obj->ox;
	oty = obj->oy;
	restricted_fall = (otx == 0 && dndest.lx);
	do {
	    tx = rn1(COLNO-3,2);
	    ty = rn2(ROWNO);
	    if (!--try_limit) break;
	} while (!goodpos(tx, ty, (struct monst *)0, 0) ||
		/* bug: this lacks provision for handling the Wizard's tower */
		 (restricted_fall &&
		  (!within_bounded_area(tx, ty, dndest.lx, dndest.ly,
						dndest.hx, dndest.hy) ||
		   (dndest.nlx &&
		    within_bounded_area(tx, ty, dndest.nlx, dndest.nly,
						dndest.nhx, dndest.nhy)))));

	if (flooreffects(obj, tx, ty, "fall")) {
	    return;
	} else if (otx == 0 && oty == 0) {
	    ;	/* fell through a trap door; no update of old loc needed */
	} else {
	    if (costly_spot(otx, oty)
	      && (!costly_spot(tx, ty) ||
		  !index(in_rooms(tx, ty, 0), *in_rooms(otx, oty, 0)))) {
		if (costly_spot(u.ux, u.uy) &&
			    index(u.urooms, *in_rooms(otx, oty, 0)))
		    addtobill(obj, FALSE, FALSE, FALSE);
		else (void)stolen_value(obj, otx, oty, FALSE, FALSE, FALSE);
	    }
	    newsym(otx, oty);	/* update old location */
	}
	place_object(obj, tx, ty);
	newsym(tx, ty);
}

/* Returns an absolute depth */
int
random_teleport_level()
{
	int nlev, max_depth, min_depth,
	    cur_depth = (int)depth(&u.uz);

	if (!rn2(5) || Is_knox(&u.uz) ||
		Is_blackmarket(&u.uz) ||
		Is_aligned_quest(&u.uz))
	    return cur_depth;

	/* What I really want to do is as follows:
	 * -- If in a dungeon that goes down, the new level is to be restricted
	 *    to [top of parent, bottom of current dungeon]
	 * -- If in a dungeon that goes up, the new level is to be restricted
	 *    to [top of current dungeon, bottom of parent]
	 * -- If in a quest dungeon or similar dungeon entered by portals,
	 *    the new level is to be restricted to [top of current dungeon,
	 *    bottom of current dungeon]
	 * The current behavior is not as sophisticated as that ideal, but is
	 * still better what we used to do, which was like this for players
	 * but different for monsters for no obvious reason.  Currently, we
	 * must explicitly check for special dungeons.  We check for Knox
	 * above; endgame is handled in the caller due to its different
	 * message ("disoriented").
	 * --KAA
	 * [ALI] Also check for Sam's blackmarket and the three aligned quests
	 * above.
	 * 3.4.2: explicitly handle quest here too, to fix the problem of
	 * monsters sometimes level teleporting out of it into main dungeon.
	 * Also prevent monsters reaching the Sanctum prior to invocation.
	 */
	min_depth = In_quest(&u.uz) ? dungeons[u.uz.dnum].depth_start : 1;
	max_depth = dunlevs_in_dungeon(&u.uz) +
		(dungeons[u.uz.dnum].depth_start - 1);

	/* can't reach the Sanctum if the invocation hasn't been performed */
	if (In_gehennom(&u.uz) && !u.uevent.invoked) max_depth -= 1;

	/* Get a random value relative to the current dungeon */
	/* Range is 1 to current+3, current not counting */
	/* Amy edit: somehow this is just stupid. Why not make it so that your deepest level counts? */
	if (In_quest(&u.uz)) nlev = dungeons[u.uz.dnum].depth_start + rnd(6);
	else { nlev = rn2(/*cur_depth*/deepest_lev_reached(TRUE) + 3 - min_depth) + min_depth;
	if (nlev >= cur_depth) nlev++;
	}

	if (nlev > max_depth) {
	    nlev = max_depth;
	    /* teleport up if already on bottom */
	    if (Is_botlevel(&u.uz)) nlev -= rnd(3);
	}
	if (nlev < min_depth) {
	    nlev = min_depth;
	    if (nlev == cur_depth) {
		nlev += rnd(3);
		if (nlev > max_depth)
		    nlev = max_depth;
	    }
	}
	return nlev;
}

/* teleport player to a random branch --Amy */
void
randombranchtele()
{

	d_level dtmp;

	/* make sure you can't be branchported if you're supposed to be immune --Amy */
	if (((u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) {
		return;
	}

	if (playerlevelportdisabled()) {
		return;
	}

	dtmp = random_branchport_level();

	schedule_goto(&dtmp, FALSE, FALSE, 0, (char *)0, (char *)0);
	u.cnd_branchportcount++;

}

/* Random branchport level decision */
d_level
random_branchport_level()
{

	d_level dtmp;
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

	return dtmp;

}

/* Levelport level decision */
int
random_banish_level()
{
	int nlev, max_depth, min_depth, cur_depth = (int)depth(&u.uz);
;

	if (Is_knox(&u.uz) ||
		Is_blackmarket(&u.uz) ||
		Is_aligned_quest(&u.uz))
	    return cur_depth;

	min_depth = In_quest(&u.uz) ? dungeons[u.uz.dnum].depth_start : 1;
	max_depth = dunlevs_in_dungeon(&u.uz) +
		(dungeons[u.uz.dnum].depth_start - 1);
	/* can't reach the Sanctum, no matter if invocation or not */
	if (In_gehennom(&u.uz)) max_depth -= 1;

	/* Get a random value relative to the current dungeon */

	nlev = rn2(max_depth - min_depth) + min_depth;

	if (nlev > max_depth) nlev = max_depth;

	if (nlev < min_depth) nlev = min_depth;

	return nlev;
}

/* Random branchport level decision */
d_level
random_banishment_level()
{

	d_level dtmp;
	boolean minusworld = FALSE;

	dtmp.dnum = dname_to_dnum("The Dungeons of Doom"); /* fail safe in case something goes wrong */
	dtmp.dlevel = 1; /* ditto */

	if (!rn2(100)) {
		dtmp.dnum = dname_to_dnum("Minus World");
		dtmp.dlevel = dunlevs_in_dungeon(&dtmp);
		minusworld = TRUE;
	} else if (rn2(2)) {
		dtmp.dnum = dname_to_dnum("The Dungeons of Doom");
	} else if (rn2(2)) {
		dtmp.dnum = dname_to_dnum("Gehennom");
	} else if (rn2(4)) {
upperdungeonagain:
		switch (rnd(32)) {
			case 1:
				dtmp.dnum = dname_to_dnum("The Gnomish Mines");
				break;
			case 2:
				dtmp.dnum = dname_to_dnum("The Quest");
				break;
			case 3:
				if (!u.silverbellget && !u.prematuresubquest) goto upperdungeonagain;
				dtmp.dnum = dname_to_dnum("The Subquest");
				break;
			case 4:
				if (!u.silverbellget && !!u.prematuresubquest) goto upperdungeonagain;
				dtmp.dnum = dname_to_dnum("Bell Caves");
				break;
			case 5:
				dtmp.dnum = dname_to_dnum("Lawful Quest");
				break;
			case 6:
				dtmp.dnum = dname_to_dnum("Neutral Quest");
				break;
			case 7:
				dtmp.dnum = dname_to_dnum("Chaotic Quest");
				break;
			case 8:
				dtmp.dnum = dname_to_dnum("Sokoban");
				break;
			case 9:
				dtmp.dnum = dname_to_dnum("Town");
				break;
			case 10:
				dtmp.dnum = dname_to_dnum("Grund's Stronghold");
				break;
			case 11:
				if (!u.havebeeninludios) goto upperdungeonagain;
				dtmp.dnum = dname_to_dnum("Fort Ludios");
				break;
			case 12:
				dtmp.dnum = dname_to_dnum("The Wyrm Caves");
				break;
			case 13:
				dtmp.dnum = dname_to_dnum("One-eyed Sam's Market");
				break;
			case 14:
				dtmp.dnum = dname_to_dnum("The Lost Tomb");
				break;
			case 15:
				dtmp.dnum = dname_to_dnum("The Spider Caves");
				break;
			case 16:
				dtmp.dnum = dname_to_dnum("The Sunless Sea");
				break;
			case 17:
				dtmp.dnum = dname_to_dnum("The Temple of Moloch");
				break;
			case 18:
				dtmp.dnum = dname_to_dnum("Grue Challenge");
				break;
			case 19:
				dtmp.dnum = dname_to_dnum("Joust Challenge");
				break;
			case 20:
				dtmp.dnum = dname_to_dnum("Pacman Challenge");
				break;
			case 21:
				dtmp.dnum = dname_to_dnum("Pool Challenge");
				break;
			case 22:
				dtmp.dnum = dname_to_dnum("Digdug Challenge");
				break;
			case 23:
				dtmp.dnum = dname_to_dnum("Illusory Castle");
				break;
			case 24:
				dtmp.dnum = dname_to_dnum("Deep Mines");
				break;
			case 25:
				dtmp.dnum = dname_to_dnum("Space Base");
				break;
			case 26: /* not a bug that the space base subdungeons are always accessible --Amy */
				dtmp.dnum = dname_to_dnum("Sewer Plant");
				break;
			case 27:
				dtmp.dnum = dname_to_dnum("Gamma Caves");
				break;
			case 28:
				dtmp.dnum = dname_to_dnum("Mainframe");
				break;
			case 29:
				dtmp.dnum = dname_to_dnum("Minotaur Maze");
				break;
			case 30:
				if (!u.greencrossopen && !Role_if(PM_PREVERSIONER) && !Role_if(PM_SPACEWARS_FIGHTER) && !Role_if(PM_CAMPERSTRIKER) && !(Role_if(PM_GANG_SCHOLAR) && u.greencrosschance < 5) && !(Role_if(PM_WALSCHOLAR) && u.greencrosschance < 5) && !(u.greencrosschance < 2) ) goto upperdungeonagain;
				dtmp.dnum = dname_to_dnum("Green Cross");
				break;
			case 31:
				dtmp.dnum = dname_to_dnum("The Giant Caverns");
				break;
			case 32:
				dtmp.dnum = dname_to_dnum("The Ice Queen's Realm");
				break;
		}
	} else {
lowerdungeonagain:
		switch (rnd(33)) {
			case 1:
				if (!u.silverbellget) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("Rival Quest");
				break;
			case 2:
				dtmp.dnum = dname_to_dnum("Void");
				break;
			case 3:
				dtmp.dnum = dname_to_dnum("Nether Realm");
				break;
			case 4:
				dtmp.dnum = dname_to_dnum("Angmar");
				break;
			case 5:
				dtmp.dnum = dname_to_dnum("Emyn Luin");
				break;
			case 6:
				dtmp.dnum = dname_to_dnum("Swimming Pool");
				break;
			case 7:
				dtmp.dnum = dname_to_dnum("Hell's Bathroom");
				break;
			case 8:
				dtmp.dnum = dname_to_dnum("Frankenstein's Lab");
				break;
			case 9:
				dtmp.dnum = dname_to_dnum("Sheol");
				break;
			case 10:
				dtmp.dnum = dname_to_dnum("Vlad's Tower");
				break;
			case 11:
				if (!achieve.get_amulet && !u.prematureyendortower) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("Yendorian Tower");
				break;
			case 12:
				if (!achieve.get_amulet && !u.prematureyendortower) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("Forging Chamber");
				break;
			case 13:
				if (!achieve.get_amulet && !u.prematureyendortower) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("Dead Grounds");
				break;
			case 14:
				if (!achieve.get_amulet && !u.prematureyendortower) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("Ordered Chaos");
				break;
			case 15:
				dtmp.dnum = dname_to_dnum("Resting Zone GA");
				break;
			case 16:
				dtmp.dnum = dname_to_dnum("Resting Zone GB");
				break;
			case 17:
				dtmp.dnum = dname_to_dnum("Resting Zone GC");
				break;
			case 18:
				dtmp.dnum = dname_to_dnum("Resting Zone GD");
				break;
			case 19:
				dtmp.dnum = dname_to_dnum("Resting Zone GE");
				break;
			case 20:
				if (!achieve.get_amulet && !u.prematureyendortower) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("Resting Zone TA");
				break;
			case 21:
				if (!achieve.get_amulet && !u.prematureyendortower) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("Resting Zone TB");
				break;
			case 22:
				if (!achieve.get_amulet && !u.prematureyendortower) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("Resting Zone TC");
				break;
			case 23:
				if (!achieve.get_amulet && !u.prematureyendortower) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("Resting Zone TD");
				break;
			case 24:
				if (!achieve.get_amulet && !u.prematureyendortower) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("Resting Zone TE");
				break;
			case 25:
				if (!achieve.get_amulet && !u.prematureyendortower) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("Resting Zone TF");
				break;
			case 26:
				if (!achieve.get_amulet && !u.prematureyendortower) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("Resting Zone TG");
				break;
			case 27:
				if (!achieve.get_amulet && !u.prematureyendortower) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("Resting Zone TH");
				break;
			case 28:
				if (!achieve.get_amulet && !u.prematureyendortower) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("Resting Zone TI");
				break;
			case 29:
				if (!achieve.get_amulet && !u.prematureyendortower) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("Resting Zone TJ");
				break;
			case 30:
				dtmp.dnum = dname_to_dnum("Resting Zone A");
				break;
			case 31:
				dtmp.dnum = dname_to_dnum("Resting Zone S");
				break;
			case 32:
				if (!u.freeplaymode || !u.freeplayplanes) goto lowerdungeonagain;
				dtmp.dnum = dname_to_dnum("The Elemental Planes");
				break;
			case 33:
				dtmp.dnum = dname_to_dnum("Resting Zone E");
				break;
		}
	}

	if (!minusworld) {
		dtmp.dlevel = rnd(dunlevs_in_dungeon(&dtmp));
		if (!strcmp(dungeons[dtmp.dnum].dname, "Gehennom") && !u.uevent.invoked) dtmp.dlevel = rnd((dunlevs_in_dungeon(&dtmp)) - 1);
	}

	return dtmp;
}

/* the player is being banished to a random level. Usually it'll be in the upper dungeons, sometimes in Gehennom,
 * but occasionally also some branch, which should be accessible so as to prevent sequence breaking.
 * 1% chance to end up in Minus World, which is initially the only way to get there. --Amy */
void
banishplayer()
{
	d_level dtmp;

	/* make sure you can't be banished if you're supposed to be immune --Amy */
	if (((u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) {
		return;
	}

	if (playerlevelportdisabled()) {
		return;
	}

	dtmp = random_banishment_level();

	if (multi >= 0) {
		nomovemsg = "You feel a little dizzy.";
		nomul(-1, "being banished", FALSE); /* because it's not called until you get another turn... */
	}

	schedule_goto(&dtmp, FALSE, FALSE, 0, (char *)0, (char *)0);
	u.cnd_banishmentcount++;

}

/* you teleport a monster (via wand, spell, or poly'd q.mechanic attack);
   return false iff the attempt fails */
boolean
u_teleport_mon(mtmp, give_feedback)
struct monst *mtmp;
boolean give_feedback;
{
	coord cc;

	if (evilfriday && (level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout)) {
	    if (give_feedback)
		pline("Ha ha ha, the wand destruction patch made it so that your wand of teleportation does jack diddly on a no-teleport level. You just wasted a charge, sucker!");
		return FALSE;
	} else if (mtmp->ispriest && *in_rooms(mtmp->mx, mtmp->my, TEMPLE)) {
	    if (give_feedback)
		pline("%s resists your magic!", Monnam(mtmp));
	    return FALSE;
	} else if ((level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout) && u.uswallow && mtmp == u.ustuck) {
	    if (give_feedback)
		You("are no longer inside %s!", mon_nam(mtmp));
	    unstuck(mtmp);
	    rloc(mtmp, FALSE);
	} else if (is_rider(mtmp->data) && rn2(13) &&
		   enexto(&cc, u.ux, u.uy, mtmp->data))
	    rloc_to(mtmp, cc.x, cc.y);
	else if (is_deadlysin(mtmp->data) && rn2(13) &&
		   enexto(&cc, u.ux, u.uy, mtmp->data))
	    rloc_to(mtmp, cc.x, cc.y);
	else {
#ifdef WIZARD
	    if (wizard && Teleport_control)
	    {
		/*
		 * [ALI] This code will only allow monsters to be
		 * teleported to positions acceptable to rloc_pos_ok().
		 * We could use goodpos() which would allow more
		 * locations but, in my view, is less informative.
		 */
		xchar omx, omy;
		pline("To what position do you want %s to be teleported?",
		       mon_nam(mtmp));
		cc.x = omx = mtmp->mx;
		cc.y = omy = mtmp->my;
		if (getpos(&cc, TRUE, "the desired position") < 0)
		    rloc(mtmp, FALSE);	/* abort */
		else if (rloc_pos_ok(cc.x, cc.y, mtmp)) {
		    rloc_to(mtmp, cc.x, cc.y);
		    /* As rloc() ... */
		    if (mtmp->isshk && !inhishop(mtmp) && issoviet) {
			make_angry_shk(mtmp, omx, omy);
			pline("VA-KHA-KHA-KHA, lavochnik ub'yet tebya.");
		    }
		} else {
		    pline("Sorry...");
		    rloc(mtmp, FALSE);
		}
	    } else
#endif /* WIZARD */
	    rloc(mtmp, FALSE);
	}
	return TRUE;
}

/* monster is levelported */
boolean
u_teleport_monB(mtmp, give_feedback)
struct monst *mtmp;
boolean give_feedback;
{
	boolean ball_active = (Punished && uball->where != OBJ_FREE);

	if (mtmp->data == &mons[PM_BAN_EVADING_TROLL]) return FALSE;

	if (mtmp->isshk && issoviet) {
		make_angry_shk(mtmp, 0, 0);
		pline("VA-KHA-KHA-KHA, lavochnik ub'yet tebya.");
	}

			int nlev;
			d_level flev;

			if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
				if (give_feedback) pline("%s seems very disoriented for a moment.", Monnam(mtmp));
				return 2;
			}
			nlev = random_banish_level();
			if (nlev == depth(&u.uz)) {
				if (give_feedback) pline("%s shudders for a moment.", Monnam(mtmp));
				return 2;
			}

			if (u.uswallow) {
				u.uswldtim = u.uswallow = 0;
				if (Punished && !ball_active) {
				    /* ensure ball placement, like unstuck */
				    ball_active = TRUE;
				}
				docrt();
			}

			get_level(&flev, nlev);
			migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM, (coord *)0);

	return TRUE;
}

/* monster is branchported */
boolean
u_teleport_monC(mtmp, give_feedback)
struct monst *mtmp;
boolean give_feedback;
{
	boolean ball_active = (Punished && uball->where != OBJ_FREE);

	if (mtmp->data == &mons[PM_BAN_EVADING_TROLL]) return FALSE;

	if (mtmp->isshk && issoviet) {
		make_angry_shk(mtmp, 0, 0);
		pline("VA-KHA-KHA-KHA, lavochnik ub'yet tebya.");
	}

			int nlev;
			d_level flev;

			if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
				if (give_feedback) pline("%s seems very disoriented for a moment.", Monnam(mtmp));
				return 2;
			}

			flev = random_branchport_level();
			migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM, (coord *)0);

	return TRUE;
}

/* monster is banished */
boolean
u_teleport_monD(mtmp, give_feedback)
struct monst *mtmp;
boolean give_feedback;
{
	boolean ball_active = (Punished && uball->where != OBJ_FREE);

	if (mtmp->data == &mons[PM_BAN_EVADING_TROLL]) return FALSE;

	if (mtmp->isshk && issoviet) {
		make_angry_shk(mtmp, 0, 0);
		pline("VA-KHA-KHA-KHA, lavochnik ub'yet tebya.");
	}

			int nlev;
			d_level flev;

			if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
				if (give_feedback) pline("%s seems very disoriented for a moment.", Monnam(mtmp));
				return 2;
			}

			flev = random_banishment_level();
			migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM, (coord *)0);

	return TRUE;
}

/* scroll/spell/whatever of return: lets the player choose a previously visited dungeon level to return to --Amy */
void
setupreturn(returntype)
int returntype; /* 0 = let player choose, 1 = let RNG choose, 2 = destination is specifically level 62 of Gehennom */
{
	int possiblechoices = 1;

	/* have the amulet? then sorry, pal, it won't work at all --Amy */
	if ((u.uhave.amulet && !u.freeplaymode) || (u.usteed && mon_has_amulet(u.usteed))) {
		pline_The("charge is absorbed by the amulet!");
		return;
	}
	/* otherwise, set up return; if you're prevented from levelporting at the time it triggers, it fizzles */

	u.returntimer = rn1(50, 50);
	u.returndestination = 1; /* fail safe: Dungeons of Doom, level 1 */

	if (returntype == 2) {
		u.returndestination = 99999; /* Gehennom, level 62, eevn if you've not been there yet */
		You_feel("the air change around you..."); /* intentionally says "change", not "charge" */
		return;
	}

	if (returntype == 0) pline("Pick a destination to return to. The prompt will loop until you actually make a choice.");

	if (returntype == 0) {
returnagain:
		if (yn("Dungeons of Doom, level 1?") == 'y') {
			u.returndestination = 1;
			goto madeachoice;
		} else if (u.returndest_dod14 && yn("Dungeons of Doom, level 14?") == 'y') {
			u.returndestination = 2;
			goto madeachoice;
		} else if (u.returndest_dod29 && yn("Dungeons of Doom, level 29?") == 'y') {
			u.returndestination = 3;
			goto madeachoice;
		} else if (u.returndest_dod44 && yn("Dungeons of Doom, level 44?") == 'y') {
			u.returndestination = 4;
			goto madeachoice;
		} else if (u.returndest_geh53 && yn("Gehennom, level 53?") == 'y') {
			u.returndestination = 5;
			goto madeachoice;
		} else if (u.returndest_geh74 && yn("Gehennom, level 74?") == 'y') {
			u.returndestination = 6;
			goto madeachoice;
		} else if (u.returndest_geh95 && yn("Gehennom, level 95?") == 'y') {
			u.returndestination = 7;
			goto madeachoice;
		} else if (u.returndest_min7 && yn("Gnomish Mines, level 7?") == 'y') {
			u.returndestination = 8;
			goto madeachoice;
		} else if (u.returndest_ill11 && yn("Illusory Castle, level 11?") == 'y') {
			u.returndestination = 9;
			goto madeachoice;
		} else if (u.returndest_ill22 && yn("Illusory Castle, level 22?") == 'y') {
			u.returndestination = 10;
			goto madeachoice;
		} else if (u.returndest_dee11 && yn("Deep Mines, level 11?") == 'y') {
			u.returndestination = 11;
			goto madeachoice;
		} else if (u.returndest_dee22 && yn("Deep Mines, level 22?") == 'y') {
			u.returndestination = 12;
			goto madeachoice;
		} else if (u.returndest_spa6 && yn("Space Base, level 6?") == 'y') {
			u.returndestination = 13;
			goto madeachoice;
		} else if (u.returndest_ang7 && yn("Angmar, level 7?") == 'y') {
			u.returndestination = 14;
			goto madeachoice;
		} else if (u.returndest_emy10 && yn("Emyn Luin, level 10?") == 'y') {
			u.returndestination = 15;
			goto madeachoice;
		} else if (u.returndest_swi8 && yn("Swimming Pools, level 8?") == 'y') {
			u.returndestination = 16;
			goto madeachoice;
		} else if (u.returndest_gre13 && yn("Green Cross, level 13?") == 'y') {
			u.returndestination = 17;
			goto madeachoice;
		} else if (u.returndest_gre28 && yn("Green Cross, level 28?") == 'y') {
			u.returndestination = 18;
			goto madeachoice;
		} else if (u.returndest_gre43 && yn("Green Cross, level 43?") == 'y') {
			u.returndestination = 19;
			goto madeachoice;
		} else if (u.returndest_gre58 && yn("Green Cross, level 58?") == 'y') {
			u.returndestination = 20;
			goto madeachoice;
		} else if (u.returndest_maz11 && yn("Minotaur Maze, level 11?") == 'y') {
			u.returndestination = 21;
			goto madeachoice;
		} else if (u.returndest_maz22 && yn("Minotaur Maze, level 22?") == 'y') {
			u.returndestination = 22;
			goto madeachoice;
		} else if (u.returndest_she5 && yn("Sheol, level 5?") == 'y') {
			u.returndestination = 23;
			goto madeachoice;
		} else if (u.returndest_she20 && yn("Sheol, level 20?") == 'y') {
			u.returndestination = 24;
			goto madeachoice;
		} else if (u.returndest_yen4 && yn("Yendorian Tower, level 4?") == 'y') {
			u.returndestination = 25;
			goto madeachoice;
		} else if (u.returndest_yen19 && yn("Yendorian Tower, level 19?") == 'y') {
			u.returndestination = 26;
			goto madeachoice;
		} else if (u.returndest_yen34 && yn("Yendorian Tower, level 34?") == 'y') {
			u.returndestination = 27;
			goto madeachoice;
		} else if (u.returndest_yen49 && yn("Yendorian Tower, level 49?") == 'y') {
			u.returndestination = 28;
			goto madeachoice;
		} else if (u.returndest_yen64 && yn("Yendorian Tower, level 64?") == 'y') {
			u.returndestination = 29;
			goto madeachoice;
		} else if (u.returndest_yen79 && yn("Yendorian Tower, level 79?") == 'y') {
			u.returndestination = 30;
			goto madeachoice;
		} else if (u.returndest_yen94 && yn("Yendorian Tower, level 94?") == 'y') {
			u.returndestination = 31;
			goto madeachoice;
		} else goto returnagain;
	}

	if (returntype == 1) {
		u.returndestination = 1;
		possiblechoices = 1;
		if (u.returndest_dod14) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 2;
		}
		if (u.returndest_dod29) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 3;
		}
		if (u.returndest_dod44) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 4;
		}
		if (u.returndest_geh53) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 5;
		}
		if (u.returndest_geh74) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 6;
		}
		if (u.returndest_geh95) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 7;
		}
		if (u.returndest_min7) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 8;
		}
		if (u.returndest_ill11) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 9;
		}
		if (u.returndest_ill22) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 10;
		}
		if (u.returndest_dee11) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 11;
		}
		if (u.returndest_dee22) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 12;
		}
		if (u.returndest_spa6) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 13;
		}
		if (u.returndest_ang7) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 14;
		}
		if (u.returndest_emy10) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 15;
		}
		if (u.returndest_swi8) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 16;
		}
		if (u.returndest_gre13) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 17;
		}
		if (u.returndest_gre28) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 18;
		}
		if (u.returndest_gre43) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 19;
		}
		if (u.returndest_gre58) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 20;
		}
		if (u.returndest_maz11) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 21;
		}
		if (u.returndest_maz22) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 22;
		}
		if (u.returndest_she5) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 23;
		}
		if (u.returndest_she20) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 24;
		}
		if (u.returndest_yen4) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 25;
		}
		if (u.returndest_yen19) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 26;
		}
		if (u.returndest_yen34) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 27;
		}
		if (u.returndest_yen49) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 28;
		}
		if (u.returndest_yen64) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 29;
		}
		if (u.returndest_yen79) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 30;
		}
		if (u.returndest_yen94) {
			possiblechoices++;
			if (!rn2(possiblechoices)) u.returndestination = 31;
		}
	}

madeachoice:
	You_feel("the air charge around you..."); /* here it says "charge", this isn't a mistake */

	return; /* todo */
}

/* A function that pushes the player around, mainly to be used by ranged attackers so they can get a shot. --Amy
 * "allowtrap" should be FALSE if it's the result of a monster attack, because otherwise we could get segfaults
 * and bus errors when the trap moves you off the level before the monster's attack routine is finished! */
void
pushplayer(allowtrap)
boolean allowtrap;
{
		coord ccc;
		int direction, pushwidth, trycnt;
		register struct obj *otmp;
		trycnt = 0;

		if (uwep && uwep->oartifact == ART_IMHULLU && rn2(10)) return;
		if (uarm && uarm->oartifact == ART_SAIL_IN_THE_WIND && rn2(2)) return;
		if (uarmf && uarmf->oartifact == ART_STAND_TALL_IN_THE_STORM) return;

newtry:
		direction = rnd(8);
		pushwidth = rnd(2);
		if (!rn2(2)) pushwidth += rnd(2);
		if (uarmf && itemhasappearance(uarmf, APP_ZERO_DROP_SHOES)) {
			pushwidth = rnd(3);
			if (!rn2(2)) pushwidth += rnd(3);
		}
		ccc.x = u.ux;
		ccc.y = u.uy;

		while (pushwidth--) {
		if (direction == 1 || direction == 5) ccc.x += 1; 
		else if (direction == 2 || direction == 6) ccc.x -= 1; 
		else if (direction == 3 || direction == 7) ccc.y += 1; 
		else if (direction == 4 || direction == 8) ccc.y -= 1; 

		if (direction == 5) ccc.y += 1;
		else if (direction == 6) ccc.y -= 1;
		else if (direction == 7) ccc.x -= 1;
		else if (direction == 8) ccc.x += 1;

		if (!isok(ccc.x, ccc.y)) break; /* otherwise the game could segfault! */

		if ((levl[ccc.x][ccc.y].typ != ROOM && levl[ccc.x][ccc.y].typ != AIR && levl[ccc.x][ccc.y].typ != STAIRS && levl[ccc.x][ccc.y].typ != LADDER && levl[ccc.x][ccc.y].typ != FOUNTAIN && levl[ccc.x][ccc.y].typ != THRONE && levl[ccc.x][ccc.y].typ != SINK && levl[ccc.x][ccc.y].typ != TOILET && levl[ccc.x][ccc.y].typ != GRAVE && levl[ccc.x][ccc.y].typ != ALTAR && levl[ccc.x][ccc.y].typ != ICE && levl[ccc.x][ccc.y].typ != CLOUD && levl[ccc.x][ccc.y].typ != SNOW && levl[ccc.x][ccc.y].typ != ASH && levl[ccc.x][ccc.y].typ != SAND && levl[ccc.x][ccc.y].typ != PAVEDFLOOR && levl[ccc.x][ccc.y].typ != HIGHWAY && levl[ccc.x][ccc.y].typ != GRASSLAND && levl[ccc.x][ccc.y].typ != NETHERMIST && levl[ccc.x][ccc.y].typ != STALACTITE && levl[ccc.x][ccc.y].typ != CRYPTFLOOR && levl[ccc.x][ccc.y].typ != BUBBLES && levl[ccc.x][ccc.y].typ != RAINCLOUD &&
			 levl[ccc.x][ccc.y].typ != CORR) || MON_AT(ccc.x, ccc.y) || (otmp = sobj_at(BOULDER, ccc.x, ccc.y)) != 0 ) break;
		}

		if ((levl[ccc.x][ccc.y].typ != ROOM && levl[ccc.x][ccc.y].typ != AIR && levl[ccc.x][ccc.y].typ != STAIRS && levl[ccc.x][ccc.y].typ != LADDER && levl[ccc.x][ccc.y].typ != FOUNTAIN && levl[ccc.x][ccc.y].typ != THRONE && levl[ccc.x][ccc.y].typ != SINK && levl[ccc.x][ccc.y].typ != TOILET && levl[ccc.x][ccc.y].typ != GRAVE && levl[ccc.x][ccc.y].typ != ALTAR && levl[ccc.x][ccc.y].typ != ICE && levl[ccc.x][ccc.y].typ != CLOUD && levl[ccc.x][ccc.y].typ != SNOW && levl[ccc.x][ccc.y].typ != ASH && levl[ccc.x][ccc.y].typ != SAND && levl[ccc.x][ccc.y].typ != PAVEDFLOOR && levl[ccc.x][ccc.y].typ != HIGHWAY && levl[ccc.x][ccc.y].typ != GRASSLAND && levl[ccc.x][ccc.y].typ != NETHERMIST && levl[ccc.x][ccc.y].typ != STALACTITE && levl[ccc.x][ccc.y].typ != CRYPTFLOOR && levl[ccc.x][ccc.y].typ != BUBBLES && levl[ccc.x][ccc.y].typ != RAINCLOUD &&
			 levl[ccc.x][ccc.y].typ != CORR) || MON_AT(ccc.x, ccc.y) || (t_at(ccc.x, ccc.y) && !allowtrap) || (otmp = sobj_at(BOULDER, ccc.x, ccc.y)) != 0) {
		if (trycnt < 50) {trycnt++; goto newtry;}
		return; /* more than 50 tries */
		}

		if (!isok(ccc.x, ccc.y)) return; /* otherwise the game could segfault! */

		pline("You're pushed back!");

		teleds(ccc.x, ccc.y, allowtrap);

		if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) doredraw();
		return;
}

void
pushplayerfar(allowtrap, distance)
boolean allowtrap;
int distance;
{
		coord ccc;
		int direction, pushwidth, trycnt;
		register struct obj *otmp;
		trycnt = 0;

		if (uwep && uwep->oartifact == ART_IMHULLU && rn2(10)) return;
		if (uarm && uarm->oartifact == ART_SAIL_IN_THE_WIND && rn2(2)) return;
		if (uarmf && uarmf->oartifact == ART_STAND_TALL_IN_THE_STORM) return;

newtry:
		direction = rnd(8);
		pushwidth = rnd(distance);
		ccc.x = u.ux;
		ccc.y = u.uy;

		while (pushwidth--) {
		if (direction == 1 || direction == 5) ccc.x += 1; 
		else if (direction == 2 || direction == 6) ccc.x -= 1; 
		else if (direction == 3 || direction == 7) ccc.y += 1; 
		else if (direction == 4 || direction == 8) ccc.y -= 1; 

		if (direction == 5) ccc.y += 1;
		else if (direction == 6) ccc.y -= 1;
		else if (direction == 7) ccc.x -= 1;
		else if (direction == 8) ccc.x += 1;

		if (!isok(ccc.x, ccc.y)) break; /* otherwise the game could segfault! */

		if ((levl[ccc.x][ccc.y].typ != ROOM && levl[ccc.x][ccc.y].typ != AIR && levl[ccc.x][ccc.y].typ != STAIRS && levl[ccc.x][ccc.y].typ != LADDER && levl[ccc.x][ccc.y].typ != FOUNTAIN && levl[ccc.x][ccc.y].typ != THRONE && levl[ccc.x][ccc.y].typ != SINK && levl[ccc.x][ccc.y].typ != TOILET && levl[ccc.x][ccc.y].typ != GRAVE && levl[ccc.x][ccc.y].typ != ALTAR && levl[ccc.x][ccc.y].typ != ICE && levl[ccc.x][ccc.y].typ != CLOUD && levl[ccc.x][ccc.y].typ != SNOW && levl[ccc.x][ccc.y].typ != ASH && levl[ccc.x][ccc.y].typ != SAND && levl[ccc.x][ccc.y].typ != PAVEDFLOOR && levl[ccc.x][ccc.y].typ != HIGHWAY && levl[ccc.x][ccc.y].typ != GRASSLAND && levl[ccc.x][ccc.y].typ != NETHERMIST && levl[ccc.x][ccc.y].typ != STALACTITE && levl[ccc.x][ccc.y].typ != CRYPTFLOOR && levl[ccc.x][ccc.y].typ != BUBBLES && levl[ccc.x][ccc.y].typ != RAINCLOUD &&
			 levl[ccc.x][ccc.y].typ != CORR) || MON_AT(ccc.x, ccc.y) || (otmp = sobj_at(BOULDER, ccc.x, ccc.y)) != 0 ) break;
		}

		if ((levl[ccc.x][ccc.y].typ != ROOM && levl[ccc.x][ccc.y].typ != AIR && levl[ccc.x][ccc.y].typ != STAIRS && levl[ccc.x][ccc.y].typ != LADDER && levl[ccc.x][ccc.y].typ != FOUNTAIN && levl[ccc.x][ccc.y].typ != THRONE && levl[ccc.x][ccc.y].typ != SINK && levl[ccc.x][ccc.y].typ != TOILET && levl[ccc.x][ccc.y].typ != GRAVE && levl[ccc.x][ccc.y].typ != ALTAR && levl[ccc.x][ccc.y].typ != ICE && levl[ccc.x][ccc.y].typ != CLOUD && levl[ccc.x][ccc.y].typ != SNOW && levl[ccc.x][ccc.y].typ != ASH && levl[ccc.x][ccc.y].typ != SAND && levl[ccc.x][ccc.y].typ != PAVEDFLOOR && levl[ccc.x][ccc.y].typ != HIGHWAY && levl[ccc.x][ccc.y].typ != GRASSLAND && levl[ccc.x][ccc.y].typ != NETHERMIST && levl[ccc.x][ccc.y].typ != STALACTITE && levl[ccc.x][ccc.y].typ != CRYPTFLOOR && levl[ccc.x][ccc.y].typ != BUBBLES && levl[ccc.x][ccc.y].typ != RAINCLOUD &&
			 levl[ccc.x][ccc.y].typ != CORR) || MON_AT(ccc.x, ccc.y) || (t_at(ccc.x, ccc.y) && !allowtrap) || (otmp = sobj_at(BOULDER, ccc.x, ccc.y)) != 0) {
		if (trycnt < 5000) {trycnt++; goto newtry;}
		return; /* more than 5000 tries */
		}

		if (!isok(ccc.x, ccc.y)) return; /* otherwise the game could segfault! */

		pline("You're pushed back!");

		teleds(ccc.x, ccc.y, allowtrap);

		if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) doredraw();
		return;
}

void
pushplayersilently(allowtrap)
boolean allowtrap;
{
		coord ccc;
		int direction, pushwidth, trycnt;
		register struct obj *otmp;
		trycnt = 0;

		if (uwep && uwep->oartifact == ART_IMHULLU && rn2(10)) return;
		if (uarm && uarm->oartifact == ART_SAIL_IN_THE_WIND && rn2(2)) return;
		if (uarmf && uarmf->oartifact == ART_STAND_TALL_IN_THE_STORM) return;

newtry:
		direction = rnd(8);
		pushwidth = rnd(2);
		if (!rn2(2)) pushwidth += rnd(2);
		if (uarmf && itemhasappearance(uarmf, APP_ZERO_DROP_SHOES)) {
			pushwidth = rnd(3);
			if (!rn2(2)) pushwidth += rnd(3);
		}
		ccc.x = u.ux;
		ccc.y = u.uy;

		while (pushwidth--) {
		if (direction == 1 || direction == 5) ccc.x += 1; 
		else if (direction == 2 || direction == 6) ccc.x -= 1; 
		else if (direction == 3 || direction == 7) ccc.y += 1; 
		else if (direction == 4 || direction == 8) ccc.y -= 1; 

		if (direction == 5) ccc.y += 1;
		else if (direction == 6) ccc.y -= 1;
		else if (direction == 7) ccc.x -= 1;
		else if (direction == 8) ccc.x += 1;

		if (!isok(ccc.x, ccc.y)) break; /* otherwise the game could segfault! */

		if ((levl[ccc.x][ccc.y].typ != ROOM && levl[ccc.x][ccc.y].typ != AIR && levl[ccc.x][ccc.y].typ != STAIRS && levl[ccc.x][ccc.y].typ != LADDER && levl[ccc.x][ccc.y].typ != FOUNTAIN && levl[ccc.x][ccc.y].typ != THRONE && levl[ccc.x][ccc.y].typ != SINK && levl[ccc.x][ccc.y].typ != TOILET && levl[ccc.x][ccc.y].typ != GRAVE && levl[ccc.x][ccc.y].typ != ALTAR && levl[ccc.x][ccc.y].typ != ICE && levl[ccc.x][ccc.y].typ != CLOUD && levl[ccc.x][ccc.y].typ != SNOW && levl[ccc.x][ccc.y].typ != ASH && levl[ccc.x][ccc.y].typ != SAND && levl[ccc.x][ccc.y].typ != PAVEDFLOOR && levl[ccc.x][ccc.y].typ != HIGHWAY && levl[ccc.x][ccc.y].typ != GRASSLAND && levl[ccc.x][ccc.y].typ != NETHERMIST && levl[ccc.x][ccc.y].typ != STALACTITE && levl[ccc.x][ccc.y].typ != CRYPTFLOOR && levl[ccc.x][ccc.y].typ != BUBBLES && levl[ccc.x][ccc.y].typ != RAINCLOUD &&
			 levl[ccc.x][ccc.y].typ != CORR) || MON_AT(ccc.x, ccc.y) || (otmp = sobj_at(BOULDER, ccc.x, ccc.y)) != 0 ) break;
		}

		if ((levl[ccc.x][ccc.y].typ != ROOM && levl[ccc.x][ccc.y].typ != AIR && levl[ccc.x][ccc.y].typ != STAIRS && levl[ccc.x][ccc.y].typ != LADDER && levl[ccc.x][ccc.y].typ != FOUNTAIN && levl[ccc.x][ccc.y].typ != THRONE && levl[ccc.x][ccc.y].typ != SINK && levl[ccc.x][ccc.y].typ != TOILET && levl[ccc.x][ccc.y].typ != GRAVE && levl[ccc.x][ccc.y].typ != ALTAR && levl[ccc.x][ccc.y].typ != ICE && levl[ccc.x][ccc.y].typ != CLOUD && levl[ccc.x][ccc.y].typ != SNOW && levl[ccc.x][ccc.y].typ != ASH && levl[ccc.x][ccc.y].typ != SAND && levl[ccc.x][ccc.y].typ != PAVEDFLOOR && levl[ccc.x][ccc.y].typ != HIGHWAY && levl[ccc.x][ccc.y].typ != GRASSLAND && levl[ccc.x][ccc.y].typ != NETHERMIST && levl[ccc.x][ccc.y].typ != STALACTITE && levl[ccc.x][ccc.y].typ != CRYPTFLOOR && levl[ccc.x][ccc.y].typ != BUBBLES && levl[ccc.x][ccc.y].typ != RAINCLOUD &&
			 levl[ccc.x][ccc.y].typ != CORR) || MON_AT(ccc.x, ccc.y) || (t_at(ccc.x, ccc.y) && !allowtrap) || (otmp = sobj_at(BOULDER, ccc.x, ccc.y)) != 0) {
		if (trycnt < 50) {trycnt++; goto newtry;}
		return; /* more than 50 tries */
		}

		if (!isok(ccc.x, ccc.y)) return; /* otherwise the game could segfault! */

		teleds(ccc.x, ccc.y, allowtrap);

		if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) doredraw();
		return;
}

boolean
pushmonster(mtmp)
struct monst *mtmp;
{
		coord ccc;
		int direction, pushwidth, trycnt;
		register struct obj *otmp;
		trycnt = 0;

		if (!mtmp) {
			impossible("pushmonster() called with no monster.");
			return 0;
		}

		if (!isok(mtmp->mx, mtmp->my)) {
			impossible("monster coordinates for pushmonster() out of range: %d, %d", mtmp->mx, mtmp->my);
			return 0;
		}

newtry:
		direction = rnd(8);
		pushwidth = rnd(2);
		if (!rn2(2)) pushwidth += rnd(2);
		ccc.x = mtmp->mx;
		ccc.y = mtmp->my;

		while (pushwidth--) {
		if (direction == 1 || direction == 5) ccc.x += 1; 
		else if (direction == 2 || direction == 6) ccc.x -= 1; 
		else if (direction == 3 || direction == 7) ccc.y += 1; 
		else if (direction == 4 || direction == 8) ccc.y -= 1; 

		if (direction == 5) ccc.y += 1;
		else if (direction == 6) ccc.y -= 1;
		else if (direction == 7) ccc.x -= 1;
		else if (direction == 8) ccc.x += 1;

		if (!isok(ccc.x, ccc.y)) break; /* otherwise the game could segfault! */

		if ((levl[ccc.x][ccc.y].typ != ROOM && levl[ccc.x][ccc.y].typ != AIR && levl[ccc.x][ccc.y].typ != STAIRS && levl[ccc.x][ccc.y].typ != LADDER && levl[ccc.x][ccc.y].typ != FOUNTAIN && levl[ccc.x][ccc.y].typ != THRONE && levl[ccc.x][ccc.y].typ != SINK && levl[ccc.x][ccc.y].typ != TOILET && levl[ccc.x][ccc.y].typ != GRAVE && levl[ccc.x][ccc.y].typ != ALTAR && levl[ccc.x][ccc.y].typ != ICE && levl[ccc.x][ccc.y].typ != CLOUD && levl[ccc.x][ccc.y].typ != SNOW && levl[ccc.x][ccc.y].typ != ASH && levl[ccc.x][ccc.y].typ != SAND && levl[ccc.x][ccc.y].typ != PAVEDFLOOR && levl[ccc.x][ccc.y].typ != HIGHWAY && levl[ccc.x][ccc.y].typ != GRASSLAND && levl[ccc.x][ccc.y].typ != NETHERMIST && levl[ccc.x][ccc.y].typ != STALACTITE && levl[ccc.x][ccc.y].typ != CRYPTFLOOR && levl[ccc.x][ccc.y].typ != BUBBLES && levl[ccc.x][ccc.y].typ != RAINCLOUD &&
			 levl[ccc.x][ccc.y].typ != CORR) || MON_AT(ccc.x, ccc.y) || (otmp = sobj_at(BOULDER, ccc.x, ccc.y)) != 0 ) break;
		}

		if ((levl[ccc.x][ccc.y].typ != ROOM && levl[ccc.x][ccc.y].typ != AIR && levl[ccc.x][ccc.y].typ != STAIRS && levl[ccc.x][ccc.y].typ != LADDER && levl[ccc.x][ccc.y].typ != FOUNTAIN && levl[ccc.x][ccc.y].typ != THRONE && levl[ccc.x][ccc.y].typ != SINK && levl[ccc.x][ccc.y].typ != TOILET && levl[ccc.x][ccc.y].typ != GRAVE && levl[ccc.x][ccc.y].typ != ALTAR && levl[ccc.x][ccc.y].typ != ICE && levl[ccc.x][ccc.y].typ != CLOUD && levl[ccc.x][ccc.y].typ != SNOW && levl[ccc.x][ccc.y].typ != ASH && levl[ccc.x][ccc.y].typ != SAND && levl[ccc.x][ccc.y].typ != PAVEDFLOOR && levl[ccc.x][ccc.y].typ != HIGHWAY && levl[ccc.x][ccc.y].typ != GRASSLAND && levl[ccc.x][ccc.y].typ != NETHERMIST && levl[ccc.x][ccc.y].typ != STALACTITE && levl[ccc.x][ccc.y].typ != CRYPTFLOOR && levl[ccc.x][ccc.y].typ != BUBBLES && levl[ccc.x][ccc.y].typ != RAINCLOUD &&
			 levl[ccc.x][ccc.y].typ != CORR) || MON_AT(ccc.x, ccc.y) || (otmp = sobj_at(BOULDER, ccc.x, ccc.y)) != 0) {
		if (trycnt < 50) {trycnt++; goto newtry;}
		return 0; /* more than 50 tries */
		}

		if (!isok(ccc.x, ccc.y)) return 0; /* otherwise the game could segfault! */

		remove_monster(mtmp->mx, mtmp->my);
		place_monster(mtmp, ccc.x, ccc.y);

		return 1;
}

/*teleport.c*/
