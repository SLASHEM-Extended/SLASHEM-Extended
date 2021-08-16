/*	SCCS Id: @(#)hack.c	3.4	2003/04/30	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "qtext.h"

STATIC_DCL int moverock(void);
STATIC_DCL int still_chewing(XCHAR_P,XCHAR_P);
STATIC_DCL void dosinkfall(void);
STATIC_DCL boolean findtravelpath(BOOLEAN_P);
STATIC_DCL boolean monstinroom(struct permonst *,int);
STATIC_DCL boolean anymonstinroom(int);

STATIC_DCL void move_update(BOOLEAN_P);
STATIC_PTR void set_litX(int,int,void *);

static boolean door_opened;	/* set to true if door was opened during test_move */


#define IS_SHOP(x)	(rooms[x].rtype >= SHOPBASE)

#ifdef OVL2

#ifdef DUNGEON_GROWTH
void
rndmappos(x,y) /* guaranteed to return a valid coord */
xchar *x;
xchar *y;
{
   if (*x >= COLNO) *x = COLNO;
   else if (*x == -1) *x = rn2(COLNO-1)+1;
   else if (*x < 1) *x = 1;
   
   if (*y >= ROWNO) *y = ROWNO;
   else if (*y == -1) *y = rn2(ROWNO);
   else if (*y < 0) *y = 0;
}

#define HERB_GROWTH_LIMIT    3 /* to limit excessive farming */

static const struct herb_info {
   int herb;
   boolean in_water;
} herb_info[] = { 
   { SPRIG_OF_WOLFSBANE, FALSE },
   { CLOVE_OF_GARLIC,    FALSE }, 
   { CARROT,             FALSE },
   { KELP_FROND,         TRUE  }
};

long
count_herbs_at(x,y, watery)
xchar x,y;
boolean watery;
{
   register int dd;
   register long count = 0;
   
   if (isok(x,y)) {
      for (dd = 0; dd < SIZE(herb_info); dd++) {
	 if (watery == herb_info[dd].in_water) {
	    register struct obj *otmp = sobj_at(herb_info[dd].herb, x,y);
	    if (otmp)
	      count += otmp->quan;
	 }
      }
   }
   return count;
}

/* returns TRUE if a herb can grow at (x,y) */
boolean
herb_can_grow_at(x,y, watery)
xchar x,y;
boolean watery;
{
  register struct rm *lev = &levl[x][y];
  if (inside_shop(x,y)) return FALSE;
  if (watery) 
     return (IS_POOL(lev->typ) && 
	     ((count_herbs_at(x,y, watery)) < HERB_GROWTH_LIMIT));
   return (lev->lit && (lev->typ == ROOM || lev->typ == CORR ||
			(IS_DOOR(lev->typ) && 
			 ((lev->doormask == D_NODOOR) ||
			 (lev->doormask == D_ISOPEN) ||
			 (lev->doormask == D_BROKEN)))) &&
	   (count_herbs_at(x,y, watery) < HERB_GROWTH_LIMIT));
}

/* grow herbs in water. return true if did something. */
boolean
grow_water_herbs(herb, x,y)
int herb;
xchar x,y;
{
   struct obj *otmp;
   
   rndmappos(&x, &y);
   otmp = sobj_at(herb, x, y);
   if (otmp && herb_can_grow_at(x,y, TRUE)) {
      otmp->quan++;
      otmp->owt = weight(otmp);
      return TRUE;
      /* There's no need to start growing these on the neighboring
       * mapgrids, as they move around (see water_current())
       */
   }
   return FALSE;
}

/* grow herb on ground at (x,y), or maybe spread out.
   return true if did something. */
boolean
grow_herbs(herb, x,y, showmsg, update)
int herb;
xchar x,y;
boolean showmsg, update;
{
   struct obj *otmp;
   struct rm *lev;
   
   rndmappos(&x, &y);
   lev = &levl[x][y];
   otmp = sobj_at(herb, x, y);
   if (otmp && herb_can_grow_at(x,y, FALSE)) {
      if (otmp->quan <= rn2(HERB_GROWTH_LIMIT)) {
	 otmp->quan++;
	 otmp->owt = weight(otmp);
	 return TRUE;
      } else {
	 int dd, dofs = rn2(8);
	 /* check surroundings, maybe grow there? */
	 for (dd = 0; dd < 8; dd++) {
	    coord pos;
	    
	    dtoxy(&pos, (dd+dofs) % 8);
	    pos.x += x;
	    pos.y += y;
	    if (isok(pos.x,pos.y) && herb_can_grow_at(pos.x,pos.y, FALSE)) {
	       lev = &levl[pos.x][pos.y];
	       otmp = sobj_at(herb, pos.x, pos.y);
	       if (otmp) {
		  if (otmp->quan <= rn2(HERB_GROWTH_LIMIT)) {
		     otmp->quan++;
		     otmp->owt = weight(otmp);
		     return TRUE;
		  }
	       } else {
		  otmp = mksobj(herb, TRUE, FALSE, FALSE);
		  if (otmp) {
			  otmp->quan = 1;
			  otmp->owt = weight(otmp); 
			  place_object(otmp, pos.x, pos.y);
			  if (update) newsym(pos.x,pos.y);
			  if (cansee(pos.x,pos.y)) {
			     if (showmsg && flags.verbose) {
				const char *what;
				if (herb == CLOVE_OF_GARLIC)
				  what = "some garlic";
				else 
				  what = an(xname(otmp));
				Norep("Suddenly you notice %s growing on the %s.",
				      what, surface(pos.x,pos.y));
			     }
			  }
		  }
		  return TRUE;
	       } 
	    }
	 }
      } 
   }
   return FALSE;
}

/* moves topmost object in water at (x,y) to dir. 
   return true if did something. */
boolean
water_current(x,y,dir,waterforce, showmsg, update)
xchar x,y;
int dir;
unsigned waterforce;  /* strength of the water current */
boolean showmsg, update;
{
   struct obj *otmp;
   coord pos;

   rndmappos(&x,&y);
   dtoxy(&pos, dir);
   pos.x += x;
   pos.y += y;
   if (isok(pos.x,pos.y) && IS_POOL(levl[x][y].typ) && 
       IS_POOL(levl[pos.x][pos.y].typ)) {
      otmp = level.objects[x][y];
      if (otmp && otmp->where == OBJ_FLOOR) {
	 if (otmp->quan > 1) 
	   otmp = splitobj(otmp, otmp->quan - 1);
	 if (otmp->owt <= waterforce) {
	    if (showmsg && Underwater && 
		(cansee(pos.x,pos.y) || cansee(x,y))) {
	       Norep("%s floats%s in%s the murky water.",
		     An(xname(otmp)),
		     (cansee(x,y) && cansee(pos.x,pos.y)) ? "" :
		     (cansee(x,y) ? " away from you" : " towards you"),
		     flags.verbose ? " the currents of" : "");
	    }
	    obj_extract_self(otmp);
	    place_object(otmp, pos.x,pos.y);
	    stackobj(otmp);
	    if (update) {
	       newsym(x,y);
	       newsym(pos.x,pos.y);
	    }
	    return TRUE;
	 } else  /* the object didn't move, put it back */
	   stackobj(otmp);
      }
   }
   return FALSE;
}

/* a tree at (x,y) spontaneously drops a ripe fruit */
boolean
drop_ripe_treefruit(x,y,showmsg, update)
xchar x,y;
boolean showmsg, update;
{
   register struct rm *lev;
   
   rndmappos(&x,&y);
   lev = &levl[x][y];
   if (IS_TREE(lev->typ) && !(lev->looted & TREE_LOOTED) && may_dig(x,y)) {
      coord pos;
      int dir, dofs = rn2(8);

	/* Amy edit: this shouldn't be a 100% chance, after all kicking isn't either */
	if (rn2(3)) {
		levl[x][y].looted |= TREE_LOOTED;
		return FALSE;
	}

      for (dir = 0; dir < 8; dir++) {
	 dtoxy(&pos, (dir + dofs) % 8);
	 pos.x += x;
	 pos.y += y;
	 if (!isok(pos.x, pos.y)) return FALSE;
	 lev = &levl[pos.x][pos.y];
	 if (SPACE_POS(lev->typ) || IS_POOL(lev->typ)) {
	    struct obj *otmp;
	    otmp = rnd_treefruit_at(pos.x,pos.y);
	    if (otmp) {
	       otmp->quan = 1;
	       otmp->owt = weight(otmp);
	       obj_extract_self(otmp);
	       if (showmsg) {
		  if ((cansee(pos.x,pos.y) || cansee(x,y))) {
		     Norep("%s falls from %s%s.",
			   cansee(pos.x,pos.y) ? An(xname(otmp)) : Something,
			   cansee(x,y) ? "the tree" : "somewhere",
			   (cansee(x,y) && IS_POOL(lev->typ)) ? 
			   " into the water" : "");
		  } else if (distu(pos.x,pos.y) < 9 && 
			     otmp->otyp != EUCALYPTUS_LEAF) {
		     /* a leaf is too light to cause any sound */
		     You_hear("a %s!",
			      (IS_POOL(lev->typ) || IS_FOUNTAIN(lev->typ)) ? 
			      "plop" : "splut"); /* rainforesty sounds */
		  }
	       }
	       place_object(otmp, pos.x,pos.y);
	       stackobj(otmp);
	       if (rn2(6)) levl[x][y].looted |= TREE_LOOTED;
	       if (update) newsym(pos.x,pos.y);
	       return TRUE;
	    }
	 }
      }
   }
   return FALSE;
}

/* Tree at (x,y) seeds. returns TRUE if a new tree was created.
 * Creates a kind of forest, with (hopefully) most places available.
 */
boolean
seed_tree(x,y)
xchar x,y;
{
   coord pos, pos2;
   struct rm *lev;
   
   rndmappos(&x,&y);
   if (IS_TREE(levl[x][y].typ) && may_dig(x,y)) {
      int dir = rn2(8);
      dtoxy(&pos, dir);
      pos.x += x;
      pos.y += y;
      if (!rn2(3)) {
	 dtoxy(&pos2, (dir+rn2(2)) % 8);
	 pos.x += pos2.x;
	 pos.y += pos2.y;
      }
      if (!isok(pos.x,pos.y)) return FALSE;
      lev = &levl[pos.x][pos.y];
      if (lev->lit && !cansee(pos.x,pos.y) && !inside_shop(pos.x,pos.y) &&
	  (lev->typ == ROOM || lev->typ == CORR) &&
	  !(u.ux == pos.x && u.uy == pos.y) && !m_at(pos.x,pos.y) && 
	  !t_at(pos.x,pos.y) && !OBJ_AT(pos.x,pos.y)) {
	 int nogrow = 0;
	 int dx,dy;
	 for (dx = pos.x-1; dx <= pos.x+1; dx++) {
	    for (dy = pos.y-1; dy <= pos.y+1; dy++) {
	       if (!isok(dx,dy) || 
		   (isok(dx,dy) && !SPACE_POS(levl[dx][dy].typ)))
		 nogrow++;
	    }
	 }
	 if (nogrow < 3) {
	    lev->typ = TREE;
	    lev->looted &= ~TREE_LOOTED;
	    block_point(pos.x,pos.y);
	    return TRUE;
	 }
      }
   } 
   return FALSE;
}

void
dgn_growths(showmsg, update, treesnstuff)
boolean showmsg; /* show messages */
boolean update;  /* do newsym() */
boolean treesnstuff;
{
	int herbnum = rn2(SIZE(herb_info));
	int randomx, randomy;
	int i, j, count, randchance=0;
	boolean secretcorr = TRUE;
	/*register struct monst *mtmp;*/

	/* note by Amy: disabled herb growth and water currents. GDB says that the dreaded savegame error is happening
	 * in this function, and since Paliculo had the savegame error happen in SLASH'EM 0.08, chances are it's something
	 * that was introduced pre-SLEX. That said, the newsym() function was erroring too, and since the dgn_growths
	 * function is being called during saving for every single level that exists, I wouldn't be at all surprised if
	 * that's somehow related. At the very least I'm taking out the minliquid() call below, and that means we don't need
	 * the mtmp anymore either. If it's still somehow crashing then we'll need to look into the GDB output which will
	 * hopefully tell us where exactly in newsym() it's choking...
	 * another note: on levels that the character isn't currently on, only regrow walls, don't drop fruit */

	/* update: *sigh* apparently the newsym() is really the culprit and I could just have used the update variable! */

	if (!rn2(100)) (void) seed_tree(-1,-1);
	/*if (herb_info[herbnum].in_water)
		(void) grow_water_herbs(herb_info[herbnum].herb, -1,-1);
		else
		(void) grow_herbs(herb_info[herbnum].herb, -1,-1, showmsg, update);*/

	if (treesnstuff && !rn2(isfriday ? 100 : 30)) (void) drop_ripe_treefruit(-1,-1, showmsg, update);

	/*(void) water_current(-1,-1, rn2(8), Is_waterlevel(&u.uz) ? 200 : 25, showmsg, update);*/

trap_of_walls:

	/* evil patch idea by Amy: occasionally, corridors and room squares will "grow" back into solid rock or walls.
	 * Depending on the # of surrounding squares that are blocked, give a higher or lower chance to place a new wall.
	 * If 6 out of 8 surrounding squares are blocked it most probably means that it's a corridor, which would
	 * completely block progress if it were made into a wall, and since players don't always have a pick-axe, let that
	 * only happen rarely. On the other hand, if all 8 surrounding squares are blocked, there probably isn't much
	 * harm done in closing it.
	 * Why am I such a filthy bitch who even thinks up such bullshit?
	 * Relax! There's a simple reason - after a while, umber hulks and similar monsters might dig out entire levels,
	 * and in vanilla there's absolutely no way to restore them to their previous condition. Not so here,
	 * where the dungeon will gradually "repair" itself, so to speak. Scrolls of lockout can further that repair.*/
   if (!rn2(iswarper ? 5 : 10) ) {
	randomx = rn1(COLNO-3,2);
	randomy = rn2(ROWNO);

	if ((!rn2(3) || (!In_sokoban(&u.uz)) ) && isok(randomx, randomy) && ((levl[randomx][randomy].wall_info & W_NONDIGGABLE) == 0) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR || (levl[randomx][randomy].typ == DOOR && levl[randomx][randomy].doormask == D_NODOOR) ) ) {
		count = 0;
		for (i= -1; i<=1; i++) for(j= -1; j<=1; j++) {
			if (!i && !j) continue;
			if (!isok(randomx+i, randomy+j) || IS_WATERTUNNEL(levl[randomx+i][randomy+j].typ) || IS_STWALL(levl[randomx+i][randomy+j].typ) )
				count++;
		}
		switch (count) {
	
			case 8:
				randchance = (levl[randomx][randomy].wall_info & W_EASYGROWTH) ? 1 : 10;
				break;
			case 7:
				randchance = (levl[randomx][randomy].wall_info & W_EASYGROWTH) ? 1 : 20;
				break;
			case 6:
				randchance = (levl[randomx][randomy].wall_info & W_EASYGROWTH) ? 10 : 10000;
				break;
			case 5:
				randchance = (levl[randomx][randomy].wall_info & W_EASYGROWTH) ? 5 : 2000;
				break;
			case 4:
				randchance = (levl[randomx][randomy].wall_info & W_EASYGROWTH) ? 2 : 500;
				break;
			case 3:
				randchance = (levl[randomx][randomy].wall_info & W_EASYGROWTH) ? 2 : 100;
				break;
			case 2:
				randchance = (levl[randomx][randomy].wall_info & W_EASYGROWTH) ? 1 : 30;
				break;
			case 1:
				randchance = (levl[randomx][randomy].wall_info & W_EASYGROWTH) ? 1 : 20;
				break;
			case 0:
			default: /* e.g. if it's 9 */
				randchance = (levl[randomx][randomy].wall_info & W_EASYGROWTH) ? 1 : 10;
				break;
	
		}
		/*pline("coord %d,%d, count %d, chance %d",randomx, randomy, count, randchance);*/

		/* In Soviet Russia, digging out an entire level should be permanent, like Moscow's Scorched Earth strategy
		 * in World War II. You should have no way of restoring any area to its previous condition, no matter what.
		 * And of course there are no pick-axes either, or any other methods of removing newly created walls. --Amy */

		if (issoviet) randchance *= 100;

		if (!rn2(randchance) && (!In_sokoban(&u.uz) || !sobj_at(BOULDER, randomx, randomy) ) ) {

			if (rn2(3)) {
				/* Sigh again. I had already given up (see below), and now of course this line was also causing
				 * the savegame error again. One day I will fucking eradicate newsym() entirely. BULLSHIT! */

				doorlockX(randomx, randomy, update); /* let's hope this "update" will FINALLY fix things ARGH */
				if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
			}
			else if ((levl[randomx][randomy].wall_info & W_NONDIGGABLE) == 0) {
				if (levl[randomx][randomy].typ != DOOR) {
					if (secretcorr && !rn2(10)) levl[randomx][randomy].typ = SCORR;
					else levl[randomx][randomy].typ = ROCKWALL;
				}
				else {
					if (secretcorr && !rn2(10)) levl[randomx][randomy].typ = SDOOR;
					else levl[randomx][randomy].typ = CROSSWALL;
				}
				if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
				blockorunblock_point(randomx,randomy);
				del_engr_at(randomx, randomy);

				/*if ((mtmp = m_at(randomx, randomy)) != 0) {
					(void) minliquid(mtmp);
				} else {*/
					if (update) newsym(randomx,randomy);
	/* this line, without the update variable check, is probably the monument of stupidity that caused savegame errors */
				/*}*/

			}

		}
	}

   }

	/* yet another update by Amy: I give up. No fucking idea why the line below is causing the savegame error,
	 * but it is. Apparently, calling this function during saving fucks up the "uwep" or "uswapwep" structures,
	 * even though the safety checks should make sure that it works right. Oh well, have to make the function get called
	 * only during regular play then, even though that is really stupid. */

	if (update) {

		if ((u.uprops[WALL_TRAP_EFFECT].extrinsic || WallTrapping || have_wallstone() || (uarmg && uarmg->oartifact == ART_STOUT_IMMURRING) || (uarmc && uarmc->oartifact == ART_MOST_CHARISMATIC_PRESIDENT) || (uwep && uwep->oartifact == ART_CUDGEL_OF_CUTHBERT) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_CUDGEL_OF_CUTHBERT) || (uwep && uwep->oartifact == ART_ONE_THROUGH_FOUR_SCEPTER) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_ONE_THROUGH_FOUR_SCEPTER) ) && rn2(100)) {
			secretcorr = FALSE;
			goto trap_of_walls;
		}

	}

}

/* catch up with growths when returning to a previously visited level */
void
catchup_dgn_growths(mvs)
int mvs;
{
   if (mvs < 0) mvs = 0;
   else if (mvs > LARGEST_INT) mvs = LARGEST_INT;
   while (mvs-- > 0)
     dgn_growths(FALSE, FALSE, FALSE);
}
#endif /* DUNGEON_GROWTH */

boolean
revive_nasty(x, y, msg)
int x,y;
const char *msg;
{
    register struct obj *otmp, *otmp2;
    struct monst *mtmp;
    coord cc;
    boolean revived = FALSE;

    for(otmp = level.objects[x][y]; otmp; otmp = otmp2) {
	otmp2 = otmp->nexthere;
	if (otmp->otyp == CORPSE &&
	    (is_rider(&mons[otmp->corpsenm]) || is_deadlysin(&mons[otmp->corpsenm]) ||
	     otmp->corpsenm == PM_WIZARD_OF_YENDOR)) {
	    /* move any living monster already at that location */
	    if((mtmp = m_at(x,y)) && enexto(&cc, x, y, mtmp->data))
		rloc_to(mtmp, cc.x, cc.y);
	    if(msg) Norep("%s", msg);
	    revived = revive_corpse(otmp, FALSE);
	}
    }

    /* this location might not be safe, if not, move revived monster */
    if (revived) {
	mtmp = m_at(x,y);
	if (mtmp && !goodpos(x, y, mtmp, 0) &&
	    enexto(&cc, x, y, mtmp->data)) {
	    rloc_to(mtmp, cc.x, cc.y);
	}
	/* else impossible? */
    }

    return (revived);
}

STATIC_OVL int
moverock()
{
    register xchar rx, ry, sx, sy;
    register struct obj *otmp;
    register struct trap *ttmp;
    register struct monst *mtmp;

    sx = u.ux + u.dx,  sy = u.uy + u.dy;	/* boulder starting position */
    while ((otmp = sobj_at(BOULDER, sx, sy)) != 0) {
	/* make sure that this boulder is visible as the top object */
	if (otmp != level.objects[sx][sy]) movobj(otmp, sx, sy);

	rx = u.ux + 2 * u.dx;	/* boulder destination position */
	ry = u.uy + 2 * u.dy;
	nomul(0, 0, FALSE);
	/* if you combine levitator and sokosolver the game shouldn't be unwinnable --Amy */
	if ((Levitation || Is_airlevel(&u.uz)) && !Race_if(PM_LEVITATOR) ) {
		if (Blind) feel_location(sx,sy);
	    You("don't have enough leverage to push %s.", the(xname(otmp)));
	    /* Give them a chance to climb over it? */
	    return -1;
	}
	if (verysmall(youmonst.data) && !Race_if(PM_TRANSFORMER) && !u.usteed ) {
		if (Blind) feel_location(sx,sy);
	    pline("You're too small to push that %s.", xname(otmp));
	    goto cannot_push;
	}
	if (isok(rx,ry) && !IS_ROCK(levl[rx][ry].typ) &&
	    levl[rx][ry].typ != IRONBARS &&
	    (!IS_DOOR(levl[rx][ry].typ) || !(u.dx && u.dy) || (
#ifdef REINCARNATION
		!Is_rogue_level(&u.uz) &&
#endif
		(levl[rx][ry].doormask & ~D_BROKEN) == D_NODOOR)) &&
	    !sobj_at(BOULDER, rx, ry)) {
	    ttmp = t_at(rx, ry);
	    mtmp = m_at(rx, ry);

		/* KMH -- Sokoban doesn't let you push boulders diagonally */
	    if (In_sokoban(&u.uz) && u.dx && u.dy) {
	    	if (Blind) feel_location(sx,sy);
	    	pline("%s won't roll diagonally on this %s.",
	        		The(xname(otmp)), surface(sx, sy));
	    	goto cannot_push;
	    }

	    if (revive_nasty(rx, ry, "You sense movement on the other side.")) {
		pline("The boulder vanishes!");
		delobj(otmp); /* prevent easy Death farming --Amy */
		return (-1);
		}

	    if (mtmp && !noncorporeal(mtmp->data) &&
		    (!mtmp->mtrapped ||
			 !(ttmp && ((ttmp->ttyp == PIT) || (ttmp->ttyp == SHIT_PIT) || (ttmp->ttyp == MANA_PIT)
			|| (ttmp->ttyp == ANOXIC_PIT) || (ttmp->ttyp == ACID_PIT) || (ttmp->ttyp == SPIKED_PIT) || (ttmp->ttyp == GIANT_CHASM))))) {

		if (Blind) feel_location(sx,sy);
		if (canspotmon(mtmp)) {
		    boolean by_name = (mtmp->data->geno & G_UNIQ ||
				       mtmp->isshk || mtmp->mnamelth);
		    if (by_name && !Hallucination)
			pline("%s is on the other side.", Monnam(mtmp));
		    else
			pline("There's %s on the other side.", a_monnam(mtmp));
		} else {
		    You_hear("a monster behind %s.", the(xname(otmp)));
		    map_invisible(rx, ry);
		}
		if (flags.verbose)
		    pline("Perhaps that's why %s cannot move it.", u.usteed ? y_monnam(u.usteed) : "you");
		goto cannot_push;
	    }

	    if (ttmp)
		switch(ttmp->ttyp) {
		case LANDMINE:
		    if (rn2(10)) {
			obj_extract_self(otmp);
			place_object(otmp, rx, ry);
			unblock_point(sx, sy);
			newsym(sx, sy);
			pline("KAABLAMM!!!  %s %s land mine.",
			      Tobjnam(otmp, "trigger"),
			      ttmp->madeby_u ? "your" : "a");
			blow_up_landmine(ttmp);
			/* if the boulder remains, it should fill the pit */
			fill_pit(u.ux, u.uy);
			if (cansee(rx,ry)) newsym(rx,ry);
			continue;
		    }
		    break;
		case SPIKED_PIT:
		case SHIT_PIT:
		case MANA_PIT:
		case ANOXIC_PIT:
		case ACID_PIT:
		case PIT:
		case GIANT_CHASM:
		    obj_extract_self(otmp);
		    /* vision kludge to get messages right;
		       the pit will temporarily be seen even
		       if this is one among multiple boulders */
		    if (!Blind) viz_array[ry][rx] |= IN_SIGHT;
		    if (!flooreffects(otmp, rx, ry, "fall")) {
			place_object(otmp, rx, ry);
		    }
		    if (mtmp && !Blind) newsym(rx, ry);
		    continue;
		case HOLE:
		case TRAPDOOR:
		case SHAFT_TRAP:
		case CURRENT_SHAFT:
		    if (Blind)
			pline("Kerplunk!  You no longer feel %s.",
				the(xname(otmp)));
		    else
			pline("%s%s and %s a %s in the %s!",
			  Tobjnam(otmp,
			   (ttmp->ttyp == TRAPDOOR) ? "trigger" : "fall"),
			  (ttmp->ttyp == TRAPDOOR) ? nul : " into",
			  otense(otmp, "plug"),
			  (ttmp->ttyp == TRAPDOOR) ? "trap door" : "hole",
			  surface(rx, ry));
		    if (PlayerHearsSoundEffects) pline(issoviet ? "Takim obrazom, vy deystvitel'no dumayete, chto vy dostatochno umny, chtoby reshit' golovolomku bloka. Ya ser'yezno somnevayus' v etom." : "Tchueb!");
		    deltrap(ttmp);
		    delobj(otmp);
		    bury_objs(rx, ry);
		    if (cansee(rx,ry)) newsym(rx,ry);
		    continue;
		case LEVEL_TELEP:
		case LEVEL_BEAMER:
		case TELEP_TRAP:
		case BEAMER_TRAP:
		    if (u.usteed)
			pline("%s pushes %s and suddenly it disappears!",
			      upstart(y_monnam(u.usteed)), the(xname(otmp)));
		    else
		    You("push %s and suddenly it disappears!",
			the(xname(otmp)));
		    if (ttmp->ttyp == TELEP_TRAP)
			rloco(otmp);
		    else if (ttmp->ttyp == BEAMER_TRAP)
			rloco(otmp);
		    else {
			int newlev = random_teleport_level();
			d_level dest;

			if (newlev == depth(&u.uz) || In_endgame(&u.uz))
			    continue;
			obj_extract_self(otmp);
			add_to_migration(otmp);
			get_level(&dest, newlev);
			otmp->ox = dest.dnum;
			otmp->oy = dest.dlevel;
			otmp->owornmask = (long)MIGR_RANDOM;
		    }
		    seetrap(ttmp);
		    continue;
		}
	    if (closed_door(rx, ry))
		goto nopushmsg;
	    if (boulder_hits_pool(otmp, rx, ry, TRUE))
		continue;
	    /*
	     * Re-link at top of fobj chain so that pile order is preserved
	     * when level is restored.
	     */
	    if (otmp != fobj) {
		remove_object(otmp);
		place_object(otmp, otmp->ox, otmp->oy);
	    }

	    {
#ifdef LINT /* static long lastmovetime; */
		long lastmovetime;
		lastmovetime = 0;
#else
		/* note: reset to zero after save/restore cycle */
		static NEARDATA long lastmovetime;
#endif
		if (!u.usteed) {
		  if (moves > lastmovetime+2 || moves < lastmovetime)
		    pline("With %s effort you move %s.",
			  throws_rocks(youmonst.data) ? "little" : "great",
			  the(xname(otmp)));
		  if (!rn2(50)) exercise(A_STR, TRUE);
		} else 
		    pline("%s moves %s.",
			  upstart(y_monnam(u.usteed)), the(xname(otmp)));
		lastmovetime = moves;

		if (otmp && otmp->oartifact == ART_WENDYHOLE) {
			pline("Wendy produces %s farting noises with her sexy butt.", !rn2(3) ? "loud" : !rn2(2) ? "disgusting" : "erogenous");
			u.cnd_fartingcount++;
			if (Role_if(PM_CLIMACTERIAL)) climtrainsqueaking(1);
			if (Role_if(PM_BUTT_LOVER) && !rn2(20)) buttlovertrigger();
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
			if (!extralongsqueak()) badeffect();
		}
	    }

	    /* Move the boulder *after* the message. */
	    if (memory_is_invisible(rx, ry))
		unmap_object(rx, ry);
	    movobj(otmp, rx, ry);	/* does newsym(rx,ry) */
	    if (Blind) {
		feel_location(rx,ry);
                feel_location(sx,sy);
	    } else {
                newsym(sx,sy);
	    }
	} else {
	nopushmsg:
	  if (u.usteed)
	    pline("%s tries to move %s, but cannot.",
		  upstart(y_monnam(u.usteed)), the(xname(otmp)));
	  else
	    You("try to move %s, but in vain.", the(xname(otmp)));
		 if (Blind) feel_location(sx,sy);
	cannot_push:
	    if (throws_rocks(youmonst.data)) {
		if (u.usteed && !(uwep && uwep->oartifact == ART_SORTIE_A_GAUCHE) && !(powerfulimplants() && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) && !(bmwride(ART_DEEPER_LAID_BMW)) && (PlayerCannotUseSkills || P_SKILL(P_RIDING) < P_BASIC) ) {
		    You("aren't skilled enough to %s %s from %s.",
			(flags.pickup && !In_sokoban(&u.uz))
			    ? "pick up" : "push aside",
			the(xname(otmp)), y_monnam(u.usteed));
		} else
		{
		    pline("However, you can easily %s.",
			(flags.pickup && !In_sokoban(&u.uz))
			    ? "pick it up" : "push it aside");
		if (yn("Do it?") != 'y')
		return (-1);

		    if (In_sokoban(&u.uz))
			{change_luck(-1);
			pline("You cheater!");
			if (evilfriday) u.ugangr++;
			}
			/* Sokoban guilt */
		    break;
		}
		break;
	    }

	    if (!u.usteed && (((!invent || inv_weight() <= -1500) &&
		 (!u.dx || !u.dy || (IS_ROCK(levl[u.ux][sy].typ)
				     && IS_ROCK(levl[sx][u.uy].typ))))
		|| verysmall(youmonst.data))) {

		if (yn("However, you can squeeze yourself into a small opening. Do it?") != 'y')
		return (-1);
		else {
		if (In_sokoban(&u.uz))
			{change_luck(-1);
			pline("You cheater!");
			if (evilfriday) u.ugangr++;
			}
			/* Sokoban guilt */
		break;
		}

	    } else
		return (-1);
	}
    }
    return (0);
}

/*
 *  still_chewing()
 *
 *  Chew on a wall, door, or boulder.  Returns TRUE if still eating, FALSE
 *  when done.
 */
STATIC_OVL int
still_chewing(x,y)
    xchar x, y;
{
    struct rm *lev = &levl[x][y];
    struct obj *boulder = sobj_at(BOULDER,x,y);
    const char *digtxt = (char *)0, *dmgtxt = (char *)0;

    if (digging.down)		/* not continuing previous dig (w/ pick-axe) */
	(void) memset((void *)&digging, 0, sizeof digging);

    if (!boulder && IS_ROCK(lev->typ) && !may_dig(x,y)) {
	You("hurt your teeth on the %s.",
	    IS_TREE(lev->typ) ? "tree" : "hard stone");
	nomul(0, 0, FALSE);
	return 1;
    } else if (digging.pos.x != x || digging.pos.y != y ||
		!on_level(&digging.level, &u.uz)) {
	digging.down = FALSE;
	digging.chew = TRUE;
	digging.warned = FALSE;
	digging.pos.x = x;
	digging.pos.y = y;
	assign_level(&digging.level, &u.uz);
	/* solid rock takes more work & time to dig through */
	digging.effort =
	    (IS_ROCK(lev->typ) && !IS_TREE(lev->typ) ? 30 : 60) + u.udaminc;
	You("start chewing %s %s.",
	    (boulder || IS_TREE(lev->typ)) ? "on a" : "a hole in the",
	    boulder ? "boulder" :
	    IS_TREE(lev->typ) ? "tree" : IS_WATERTUNNEL(lev->typ) ? "rock above the water" : IS_ROCK(lev->typ) ? "rock" : "door");
	watch_dig((struct monst *)0, x, y, FALSE);
	return 1;
    } else if ((digging.effort += (30 + u.udaminc)) <= 100)  {
	if (flags.verbose)
	    You("%s chewing on the %s.",
		digging.chew ? "continue" : "begin",
		boulder ? "boulder" :
		IS_TREE(lev->typ) ? "tree" :
		IS_ROCK(lev->typ) ? "rock" : "door");
	digging.chew = TRUE;
	watch_dig((struct monst *)0, x, y, FALSE);
	return 1;
    }

    /* Okay, you've chewed through something */
    u.uconduct.food++;
    u.uhunger += rnd(20);

    if (boulder) {
	delobj(boulder);		/* boulder goes bye-bye */
	You("eat the boulder.");	/* yum */

	/*
	 *  The location could still block because of
	 *	1. More than one boulder
	 *	2. Boulder stuck in a wall/stone/door.
	 *
	 *  [perhaps use does_block() below (from vision.c)]
	 */
	if (IS_ROCK(lev->typ) || closed_door(x,y) || sobj_at(BOULDER,x,y)) {
	    block_point(x,y);	/* delobj will unblock the point */
	    /* reset dig state */
	    (void) memset((void *)&digging, 0, sizeof digging);
	    return 1;
	}

    } else if (IS_WATERTUNNEL(lev->typ)) {
	digtxt = "chew away the rock above the water.";
	lev->typ = MOAT;
    } else if (IS_WALL(lev->typ)) {
	if (*in_rooms(x, y, SHOPBASE)) {
	    add_damage(x, y, 10L * ACURRSTR);
	    dmgtxt = "damage";
	}
	digtxt = "chew a hole in the wall.";
	if (level.flags.is_maze_lev) {
	    lev->typ = ROOM;
		if (u.geolysis && !rn2(4)) {
			lev->typ = !rn2(3) ? WATER : !rn2(2) ? ICE : CLOUD;
		}
	} else if (level.flags.is_cavernous_lev && !in_town(x, y)) {
	    lev->typ = CORR;
		if (u.geolysis && !rn2(4)) {
			lev->typ = !rn2(3) ? WATER : !rn2(2) ? ICE : CLOUD;
		}
	} else {
	    lev->typ = DOOR;
	    lev->doormask = D_NODOOR;
	}
    } else if (IS_TREE(lev->typ)) {
	digtxt = "chew through the tree.";
	lev->typ = ROOM;
    } else if (lev->typ == SDOOR) {
	if (lev->doormask & D_TRAPPED) {
	    lev->doormask = D_NODOOR;
	    b_trapped("secret door", 0);
	} else {
	    digtxt = "chew through the secret door.";
	    lev->doormask = D_BROKEN;
	}
	lev->typ = DOOR;

    } else if (IS_DOOR(lev->typ)) {
	if (*in_rooms(x, y, SHOPBASE)) {
	    add_damage(x, y, 400L);
	    dmgtxt = "break";
	}
	if (lev->doormask & D_TRAPPED) {
	    lev->doormask = D_NODOOR;
	    b_trapped("door", 0);
	} else {
	    digtxt = "chew through the door.";
	    lev->doormask = D_BROKEN;
	}

    } else { /* STONE or SCORR */
	digtxt = "chew a passage through the rock.";
	lev->typ = CORR;
	if (u.geolysis && !rn2(4)) {
		lev->typ = !rn2(3) ? WATER : !rn2(2) ? ICE : CLOUD;
	}
    }

    unblock_point(x, y);	/* vision */
	if (!(levl[x][y].wall_info & W_HARDGROWTH)) levl[x][y].wall_info |= W_EASYGROWTH;
    newsym(x, y);
    if (digtxt) You("%s", digtxt);	/* after newsym */
    if (dmgtxt) pay_for_damage(dmgtxt, FALSE);
    (void) memset((void *)&digging, 0, sizeof digging);
    return 0;
}

#endif /* OVL2 */
#ifdef OVLB

void
movobj(obj, ox, oy)
register struct obj *obj;
register xchar ox, oy;
{
	/* optimize by leaving on the fobj chain? */
	remove_object(obj);
	newsym(obj->ox, obj->oy);
	place_object(obj, ox, oy);
	newsym(ox, oy);
}

static NEARDATA const char fell_on_sink[] = "fell onto a sink";

STATIC_OVL void
dosinkfall()
{
	register struct obj *obj;

	if (is_floater(youmonst.data) || (HLevitation & FROMOUTSIDE)) {
	    You("wobble unsteadily for a moment.");
	} else {
	    long save_ELev = ELevitation, save_HLev = HLevitation;

	    /* fake removal of levitation in advance so that final
	       disclosure will be right in case this turns out to
	       be fatal; fortunately the fact that rings and boots
	       are really still worn has no effect on bones data */
	    ELevitation = HLevitation = 0L;
	    You("crash to the floor!");
	    losehp(rn1(8, 25 - (int)ACURR(A_CON)),
		   fell_on_sink, NO_KILLER_PREFIX);
	    exercise(A_DEX, FALSE);
	    selftouch("Falling, you");
	    for (obj = level.objects[u.ux][u.uy]; obj; obj = obj->nexthere)
		if (obj->oclass == WEAPON_CLASS || is_weptool(obj)) {
		    You("fell on %s.", doname(obj));
		    losehp(rnd(3), fell_on_sink, NO_KILLER_PREFIX);
		    exercise(A_CON, FALSE);
		}
	    ELevitation = save_ELev;
	    HLevitation = save_HLev;
	}

	ELevitation &= ~W_ARTI;
	HLevitation &= ~(I_SPECIAL|TIMEOUT);
	HLevitation++;
	if(uleft && uleft->otyp == RIN_LEVITATION) {
	    obj = uleft;
	    Ring_off(obj);
	    off_msg(obj);
	}
	if(uright && uright->otyp == RIN_LEVITATION) {
	    obj = uright;
	    Ring_off(obj);
	    off_msg(obj);
	}
	if(uarmf && uarmf->otyp == LEVITATION_BOOTS) {
	    obj = uarmf;
	    (void)Boots_off();
	    off_msg(obj);
	}
	HLevitation--;
}

boolean
may_dig(x,y)
register xchar x,y;
/* intended to be called only on ROCKs */
{
    return (boolean)(!(IS_STWALL(levl[x][y].typ) && !(IS_DIGGABLEWALL(levl[x][y].typ)) &&
			(levl[x][y].wall_info & W_NONDIGGABLE)) && !(IS_FARMLAND(levl[x][y].typ)) && !(IS_GRAVEWALL(levl[x][y].typ)) && !(IS_MOUNTAIN(levl[x][y].typ)) );
}

boolean
may_passwall(x,y)
register xchar x,y;
{
   return (boolean)(!(IS_STWALL(levl[x][y].typ) && !(IS_DIGGABLEWALL(levl[x][y].typ)) &&
			(levl[x][y].wall_info & W_NONPASSWALL)));
}

#endif /* OVLB */
#ifdef OVL1

/* [ALI] Changed to take monst * as argument to support passwall property */
boolean
bad_rock(mon,x,y)
struct monst *mon;
register xchar x,y;
{
	struct permonst *mdat = mon->data;
	boolean passwall = mon == &youmonst ? Passes_walls : ( passes_walls(mdat) || (mon->egotype_wallwalk) );
	return((boolean) ((In_sokoban(&u.uz) && sobj_at(BOULDER,x,y)) ||
	       (IS_ROCK(levl[x][y].typ)
		    && (!tunnels(mdat) || needspick(mdat) || !may_dig(x,y))
		    && !(passwall && may_passwall(x,y)))));
}

boolean
invocation_pos(x, y)
xchar x, y;
{
	return((boolean)(Invocation_lev(&u.uz) && x == inv_pos.x && y == inv_pos.y));
}

#endif /* OVL1 */
#ifdef OVL3
/* For my clever ending messages... */
int Instant_Death = 0;
int Quick_Death = 0;
int Nibble_Death = 0;
int last_hit = 0;
int second_last_hit = 0;
int third_last_hit = 0;

/* For those tough guys who get carried away... */
int repeat_hit = 0;

/* return TRUE if (dx,dy) is an OK place to move
 * mode is one of DO_MOVE, TEST_MOVE or TEST_TRAV
 */
boolean 
test_move(ux, uy, dx, dy, mode)
int ux, uy, dx, dy;
int mode;
{
    int x = ux+dx;
    int y = uy+dy;
    register struct rm *tmpr = &levl[x][y];
    register struct rm *ust;
    boolean opentry = 0;

    /*
     *  Check for physical obstacles.  First, the place we are going.
     */
    if (IS_ROCK(tmpr->typ) || tmpr->typ == IRONBARS || (SpellColorPlatinum && (glyph_to_cmap(glyph_at(x,y)) == S_bars) ) || tmpr->typ == WOODENTABLE || tmpr->typ == WATERTUNNEL) {
	if (Blind && mode == DO_MOVE) feel_location(x,y);
	if (tmpr->typ == IRONBARS || (SpellColorPlatinum && (glyph_to_cmap(glyph_at(x,y)) == S_bars) ) ) {
	    if (!(Passes_walls || passes_bars(youmonst.data) || (powerfulimplants() && uimplant && uimplant && uimplant->oartifact == ART_SIGNIFICANT_RNG_JITTER) )) {
		if (mode == DO_MOVE) {
			if (WallsAreHyperBlue) {
				You("crash into a set of iron bars! Ouch!");

				losehp(rnd(10), "walking into iron bars", KILLED_BY);
				if (!rn2(Role_if(PM_COURIER) ? 1000 : uarmh ? 50 : 10)) {
					if (rn2(50)) {
						adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
						if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
					} else {
						You_feel("dizzy!");
						forget(1 + rn2(5));
					}
				}

			} else pline("There is a set of iron bars in the way!");
		}
		return FALSE;
		}
	    else if (In_sokoban(&u.uz)) {
		if (mode == DO_MOVE)
		    pline_The("Sokoban bars resist your ability.");
		return FALSE;
	    }
	} else if (Passes_walls && may_passwall(x,y)) {
	    ;	/* do nothing */
	} else if (Race_if(PM_HUMANOID_DRYAD) && tmpr->typ == TREE) {
	    ;	/* dryad can walk thru trees --Amy */
	} else if (uarmg && uarmg->oartifact == ART_GREEN_THUMB) {
	    ;
	} else if (uarmf && uarmf->oartifact == ART_EVERYTHING_IS_GREEN && tmpr->typ == TREE) {
	    ;	/* special effect of that artifact --Amy */
	} else if (uwep && uwep->oartifact == ART_GIFT_TO_NATURE && tmpr->typ == TREE) {
	    ;	/* special effect of that artifact --Amy */

	} else if (tmpr->typ == WOODENTABLE) {

		if ( (near_capacity() > UNENCUMBERED) && !Passes_walls) {
			if (mode != DO_MOVE) return FALSE;
			if (mode == DO_MOVE) {
				pline("Climbing the table does not work while you're burdened.");
				return FALSE;
			}
		}

	} else if (tmpr->typ == MOUNTAIN) {
		if (mode != DO_MOVE) return FALSE;
		if (mode == DO_MOVE && !Passes_walls && !(powerfulimplants() && uimplant && uimplant && uimplant->oartifact == ART_SIGNIFICANT_RNG_JITTER)) {
			int climbingchance = 100;
			if (uamul && uamul->otyp == AMULET_OF_CLIMBING) climbingchance = ((levl[u.ux][u.uy].typ == MOUNTAIN) ? 3 : 10);
			if (ublindf && ublindf->otyp == CLIMBING_SET) climbingchance = ((levl[u.ux][u.uy].typ == MOUNTAIN) ? 3 : 10);
			if (uwep && uwep->otyp == CLIMBING_STICK) climbingchance = ((levl[u.ux][u.uy].typ == MOUNTAIN) ? 3 : 10);
			if (uarmf && itemhasappearance(uarmf, APP_CLIMBING_BOOTS)) climbingchance = ((levl[u.ux][u.uy].typ == MOUNTAIN) ? 3 : 10);

			if (!(u.usteed) && rn2(climbingchance)) {
				TimerunBug += 1; /* ugly hack --Amy */
				Norep("You try to scale the mountain. This may take many attempts to succeed.");
				/* Note that it is not a bug that you cannot easily walk over the next mountain tile
				 * even if you're already on one, since they're considered to have different heights :D --Amy
				 * however, with climbing gear it will indeed help */
				return(FALSE);
			}
			/* success! */
			if (!(u.usteed)) {
				You("successfully climb the mountain.");
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			}
		}

	} else if (tmpr->typ == FARMLAND) {
		if (mode != DO_MOVE && !Levitation && !Flying && !(ublindf && ublindf->oartifact == ART_FREEBOUND) && !(uwep && uwep->oartifact == ART_GARY_S_RIVALRY) && !(uwep && uwep->oartifact == ART_REAL_WALKING) && !(u.usteed && u.usteed->data->mlet == S_QUADRUPED) && !(Upolyd && youmonst.data->mlet == S_QUADRUPED)) return FALSE;

		if (mode == DO_MOVE && !Levitation && !Flying && !(ublindf && ublindf->oartifact == ART_FREEBOUND) && !(uwep && uwep->oartifact == ART_GARY_S_RIVALRY) && !(uwep && uwep->oartifact == ART_REAL_WALKING) && !(u.usteed && u.usteed->data->mlet == S_QUADRUPED) && !(Upolyd && youmonst.data->mlet == S_QUADRUPED) && !(powerfulimplants() && uimplant && uimplant && uimplant->oartifact == ART_SIGNIFICANT_RNG_JITTER) ) {

			if (WallsAreHyperBlue) {
				You("crash into a farmland! Ouch!");

				losehp(rnd(10), "walking into a farmland", KILLED_BY);
				if (!rn2(Role_if(PM_COURIER) ? 1000 : uarmh ? 50 : 10)) {
					if (rn2(50)) {
						adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
						if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
					} else {
						You_feel("dizzy!");
						forget(1 + rn2(5));
					}
				}
			} else {
				You("cannot cross the farmland!");
				if (FunnyHallu) pline("Nature preservation and all that.");
				/* Even passwall does not help here, this is intentional. --Amy */
			}

			return FALSE;
		}

	} else if (tmpr->typ == TUNNELWALL) {
		if (mode != DO_MOVE && !Passes_walls && (Flying || Levitation)) return FALSE;
		if (mode == DO_MOVE && !Passes_walls && (Flying || Levitation)) {

			if (WallsAreHyperBlue) {
				You("crash into a tunnel! Ouch!");

				losehp(rnd(10), "walking into a tunnel", KILLED_BY);
				if (!rn2(Role_if(PM_COURIER) ? 1000 : uarmh ? 50 : 10)) {
					if (rn2(50)) {
						adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
						if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
					} else {
						You_feel("dizzy!");
						forget(1 + rn2(5));
					}
				}
			} else {
				if (Levitation) pline("While levitating, you cannot enter the tunnel.");
				else pline("While flying, you cannot enter the tunnel.");
			}

			return FALSE;

		}

	} else if (tmpr->typ == GRAVEWALL) {
		/* Once again, passwall intentionally does not help --Amy */

		if (u.walscholarpass) goto walscholardone; /* can pass through */

		if (mode != DO_MOVE) return FALSE;

		if (Role_if(PM_WALSCHOLAR) && (yn("Do you really want to dig into the grave wall? Doing so would be sinful for a Walscholar.") != 'y') ) return FALSE;

		if (rn2(5) && !(uwep && uwep->oartifact == ART_CERULEAN_SMASH) ) {
			Norep("You dig into the grave wall.");
			TimerunBug += 1; /* ugly hack --Amy */
			return FALSE;
		} else {
			You("dig out the grave wall.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			u.cnd_gravewallamount++;

			if (Role_if(PM_WALSCHOLAR)) {
				You_feel("like a miserably hussy.");
				if (FunnyHallu) pline("Maybe you should buy a bottle of drum stint reluctance perfume.");
				u.ualign.sins++;
				u.alignlim--;
				adjalign(-10);
			}

			tmpr->typ = CORR;
			blockorunblock_point(ux+dx,uy+dy);
			if (!rn2(20) && isok(ux+dx, uy+dy)) {
				maketrap(ux+dx, uy+dy, randomtrap(), 100, TRUE);
			} else if (!rn2(20) && isok(ux+dx, uy+dy)) {
				pline("There was a monster hidden underneath the wall!");
				makemon((struct permonst *)0, ux+dx, uy+dy, MM_ADJACENTOK);
				return FALSE;
			}

			if (Role_if(PM_HUSSY)) {
				You("feel like a proper hussy.");
				adjalign(rnd(5));
				if (!rn2(10)) {
					pline("There was some gold hidden in the grave wall!");
					u.ugold += rnz(10);
				}
				if (!rn2(1000) && isok(ux+dx, uy+dy)) {
					(void) mksobj_at(DIAMOND, ux+dx, uy+dy, TRUE, TRUE, FALSE);
					pline("Wow, this was one of the special grave walls where Hans Walt had hidden a diamond!");
				}
			}

			if (!Role_if(PM_WALSCHOLAR)) {
				more_experienced(Role_if(PM_HUSSY) ? 50 : 5, 0);
				newexplevel();
			}

		}

walscholardone:
		;

	} else if (tmpr->typ == WATERTUNNEL) {
		if (mode != DO_MOVE) return FALSE;

		if ( ( ( (tunnels(youmonst.data) && !needspick(youmonst.data)) || (uarmf && uarmf->oartifact == ART_STONEWALL_CHECKERBOARD_DIS) || (Race_if(PM_SCURRIER) && !Upolyd) || u.geolysis) ) && flags.eatingwalls && (Flying || Levitation) ) {
		/* can eat the stupid things */
			if (mode == DO_MOVE && still_chewing(x,y)) return FALSE;
		}

		/* if you just ate it, you shouldn't crash into a no-longer-existing rock part above the tunnel */
		if (mode == DO_MOVE && !Passes_walls && (Flying || Levitation) && (levl[x][y].typ == WATERTUNNEL) ) {

			if (WallsAreHyperBlue) {
				You("crash into a water tunnel! Ouch!");

				losehp(rnd(10), "walking into a water tunnel", KILLED_BY);
				if (!rn2(Role_if(PM_COURIER) ? 1000 : uarmh ? 50 : 10)) {
					if (rn2(50)) {
						adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
						if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
					} else {
						You_feel("dizzy!");
						forget(1 + rn2(5));
					}
				}
			} else {
				if (Levitation) pline("While levitating, you cannot enter the water tunnel.");
				else pline("While flying, you cannot enter the water tunnel.");
			}

			return FALSE;

		}

	} else if ( ( ( (tunnels(youmonst.data) && !needspick(youmonst.data)) || (uarmf && uarmf->oartifact == ART_STONEWALL_CHECKERBOARD_DIS) || (Race_if(PM_SCURRIER) && !Upolyd) || u.geolysis) ) && flags.eatingwalls ) {
	    /* Eat the rock. */
	    if (mode == DO_MOVE && still_chewing(x,y)) return FALSE;

	/* autodig: note by Amy, this needs to interact with all nasty traps that would fire when you apply something.
	 * For simplicity of coding, I decided to make autodig do nothing if you have such a trap active :P */
	} else if (flags.autodig && !(u.powerfailure || CurseAsYouUse || InterruptEffect || u.uprops[INTERRUPT_EFFECT].extrinsic || have_interruptionstone() || (isselfhybrid && (moves % 3 == 0 && moves % 11 != 0) ) ) && !(WallsAreHyperBlue) && !flags.run && !flags.nopick &&
		   uwep && is_pick(uwep)) {
	/* MRKR: Automatic digging when wielding the appropriate tool */
	    if (mode == DO_MOVE) {
		if (!touch_artifact(uwep, &youmonst)) return FALSE;
		(void) use_pick_axe2(uwep);
	    }
	    return FALSE;
	} else {
	    if (mode == DO_MOVE) {
		if (Is_stronghold(&u.uz) && is_db_wall(x,y))
		    pline_The("drawbridge is up!");
		if (Passes_walls && !may_passwall(x,y) && In_sokoban(&u.uz))
		    pline_The("Sokoban walls resist your ability.");

		if (!(Is_stronghold(&u.uz) && is_db_wall(x,y)) && !(Passes_walls && !may_passwall(x,y) && In_sokoban(&u.uz))) {
			if (tmpr->typ == TREE && mode == DO_MOVE) {

				if (WallsAreHyperBlue) {
					You("crash into a tree! Ouch!");

					losehp(rnd(10), "walking into a tree", KILLED_BY);
					if (!rn2(Role_if(PM_COURIER) ? 1000 : uarmh ? 50 : 10)) {
						if (rn2(50)) {
							adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
							if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
						} else {
							You_feel("dizzy!");
							forget(1 + rn2(5));
						}
					}
				} else pline("There is a tree in the way!");

			} else if (mode == DO_MOVE) {

				if (WallsAreHyperBlue) {
					You("crash into a wall! Ouch!");

					losehp(rnd(10), "walking into a wall", KILLED_BY);
					if (!rn2(Role_if(PM_COURIER) ? 1000 : uarmh ? 50 : 10)) {
						if (rn2(50)) {
						adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
						if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
						} else {
							You_feel("dizzy!");
							forget(1 + rn2(5));
						}
					}
				} else pline("There is a wall in the way!");
			}
		}
	    }
	    return FALSE;
	}
    } else if (IS_DOOR(tmpr->typ)) {
	if (closed_door(x,y)) {

	    if (Blind && mode == DO_MOVE) feel_location(x,y);
	    /* ALI - artifact doors */
	    if (artifact_door(x, y)) {
		if (mode == DO_MOVE) {
		    if (amorphous(youmonst.data))
			You("try to ooze under the door, but the gap is too small.");
		    else if ((tunnels(youmonst.data) && !needspick(youmonst.data)) || (uarmf && uarmf->oartifact == ART_STONEWALL_CHECKERBOARD_DIS) || (Race_if(PM_SCURRIER) && !Upolyd))
			You("hurt your teeth on the re-enforced door.");
		    else if (x == u.ux || y == u.uy) {
			if (Blind || Stunned || Numbed || ACURR(A_DEX) < 10 || Fumbling) {                            pline("Ouch!  You bump into a heavy door.");
			    exercise(A_DEX, FALSE);
			} else pline("That door is closed.");
		    }
		}
		return FALSE;
	    } else
	    if (Passes_walls)
		;	/* do nothing */
	    else if (can_ooze(&youmonst) || (uwep && uwep->oartifact == ART_DOORS_ARE_NO_OBSTACLES)) {
		if (mode == DO_MOVE) You("ooze under the door.");
	    } else if (((tunnels(youmonst.data) && !needspick(youmonst.data)) || (uarmf && uarmf->oartifact == ART_STONEWALL_CHECKERBOARD_DIS) || (Race_if(PM_SCURRIER) && !Upolyd)) && flags.eatingdoors ) {
		/* Eat the door. */
		if (mode == DO_MOVE && still_chewing(x,y)) return FALSE;
	    } else {
		if (mode == DO_MOVE) {
		    if (amorphous(youmonst.data))
			You("try to ooze under the door, but can't squeeze your possessions through.");
		    else if (iflags.autoopen && !Confusion && !Stunned && !Fumbling && levl[ux][uy].seenv && !(RMBLoss || u.uprops[RMB_LOST].extrinsic || (uarmh && uarmh->oartifact == ART_NO_RMB_VACATION) || (uamul && uamul->oartifact == ART_BUEING) || (uimplant && uimplant->oartifact == ART_ARABELLA_S_SEXY_CHARM) || (uarmh && uarmh->oartifact == ART_WOLF_KING) || (uamul && uamul->oartifact == ART_YOU_HAVE_UGH_MEMORY) || have_rmbstone() || u.totter || (uarms && uarms->oartifact == ART_DOLORES__VIRGINITY) || (uarms && uarms->oartifact == ART_BLUE_SHIRT_OF_DEATH) || u.uprops[TOTTER_EFFECT].extrinsic || TotterTrapEffect || have_directionswapstone() || (uimplant && uimplant->oartifact == ART_CORTEX_COPROCESSOR) || ClockwiseSpinBug || u.uprops[CLOCKWISE_SPIN_BUG].extrinsic || have_clockwisestone() || CounterclockwiseSpin || u.uprops[COUNTERCLOCKWISE_SPIN_BUG].extrinsic || have_counterclockwisestone() || InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone() || QuasarVision || u.uprops[QUASAR_BUG].extrinsic || have_quasarstone() || GrayoutBug || u.uprops[GRAYOUT_BUG].extrinsic || have_grayoutstone() || GrayCenterBug || u.uprops[GRAY_CENTER_BUG].extrinsic || have_graycenterstone() || Quaversal || u.uprops[QUAVERSAL].extrinsic || have_quaversalstone() || (uwep && uwep->oartifact == ART_OMGHAXERETH) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_OMGHAXERETH) || (SpellColorSilver && !u.seesilverspell) || CheckerboardBug || u.uprops[CHECKERBOARD_BUG].extrinsic || have_checkerboardstone() || WallsAreHyperBlue ) ) {
			    door_opened = flags.move = doopen_indir(x, y);
			    opentry = 1;
		    }
		    else if (x == ux || y == uy) {

			if (WallsAreHyperBlue) {
				You("crash into a door! Ouch!");

				losehp(rnd(10), "walking into a door", KILLED_BY);
				if (!rn2(Role_if(PM_COURIER) ? 1000 : uarmh ? 50 : 10)) {
					if (rn2(50)) {
						adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
						if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
					} else {
						You_feel("dizzy!");
						forget(1 + rn2(5));
					}
				}
			}
			/* It is not a mistake that the next message will still be displayed,
			 * since it contains the check for dexterity abuse and hyperbluewalls should not disable that :D --Amy */

			if (Blind || Stunned || Numbed || ACURR(A_DEX) < 10 || Fumbling) {
			    if (u.usteed) {
				You_cant("lead %s through that closed door.",
				      y_monnam(u.usteed));
		 	    } else
			    {
			        pline("Ouch!  You bump into a door.");
			        exercise(A_DEX, FALSE);
			    }
			} else pline("That door is closed.");

		    return FALSE;
		    }
		} else if (mode == TEST_TRAV) goto testdiag;
		if (mode == DO_MOVE) {

				if (WallsAreHyperBlue) {
					You("crash into a door! Ouch!");

					losehp(rnd(10), "walking into a door", KILLED_BY);
					if (!rn2(Role_if(PM_COURIER) ? 1000 : uarmh ? 50 : 10)) {
						if (rn2(50)) {
							adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
							if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
						} else {
							You_feel("dizzy!");
							forget(1 + rn2(5));
						}
					}
				}

				/* only print "there is a door in the way" if autoopen didn't try to open it --Amy */

				else if (dx && dy && !opentry && !Passes_walls && ((tmpr->doormask & ~D_BROKEN)
#ifdef REINCARNATION
		    || Is_rogue_level(&u.uz)
#endif
		    || block_door(x,y))) {

					pline("There is a door in the way!");
				}

		}
		return FALSE;
	    }
	} else {
	testdiag:
	    if ((dx && dy && !Passes_walls
		&& ((tmpr->doormask & ~D_BROKEN)
#ifdef REINCARNATION
		    || Is_rogue_level(&u.uz)
#endif
		    || block_door(x,y))) && !can_ooze(&youmonst) && !(uwep && uwep->oartifact == ART_DOORS_ARE_NO_OBSTACLES)) {
		/* Diagonal moves into a door are not allowed. */
		if (Blind && mode == DO_MOVE)
		    feel_location(x,y);
		if (mode == DO_MOVE) pline("You cannot diagonally move through a door!");
		return FALSE;
	    }
	}
    }
    if (dx && dy
	    && bad_rock(&youmonst,ux,y) && bad_rock(&youmonst,x,uy)) {
	/* Move at a diagonal. */
	if (In_sokoban(&u.uz)) {
	    if (mode == DO_MOVE)
		You("cannot pass that way.");
	    return FALSE;
	}
	if ( (bigmonst(youmonst.data) && !Race_if(PM_TRANSFORMER) ) || (!Upolyd && Race_if(PM_HUMANOID_CENTAUR) ) || (!Upolyd && Race_if(PM_ETHEREALOID) ) || (!Upolyd && Race_if(PM_INCORPOREALOID) ) || (!Upolyd && Race_if(PM_PLAYER_CERBERUS) ) || (!Upolyd && Race_if(PM_CHIROPTERAN) ) || (!Upolyd && Race_if(PM_THUNDERLORD) ) || (uarmf && uarmf->oartifact == ART_ANTJE_S_POWERSTRIDE) || (!Upolyd && Race_if(PM_PLAYER_JABBERWOCK) ) ) {
	    if (mode == DO_MOVE)
		Your("body is too large to fit through.");
	    return FALSE;
	}
	if (invent && ((inv_weight() + weight_cap()) > 5000)) {
	    if (mode == DO_MOVE)
        if (!Passes_walls)
		You("are carrying too much to get through.");
	    return FALSE;
	}
    }
    /* Pick travel path that does not require crossing a trap.
     * Avoid water and lava using the usual running rules.
     * (but not u.ux/u.uy because findtravelpath walks toward u.ux/u.uy) */
    if (flags.run == 8 && mode != DO_MOVE && (x != u.ux || y != u.uy)) {
	struct trap* t = t_at(x, y);

	if ((t && t->tseen) ||
	    (!Levitation && !Flying &&
	     !is_clinger(youmonst.data) &&
	     (is_waterypool(x, y) || is_watertunnel(x,y) || is_moorland(x,y) || is_styxriver(x,y) || is_shiftingsand(x,y) || is_urinelake(x,y) || is_lava(x, y)) && levl[x][y].seenv))
	    return FALSE;
    }

    ust = &levl[ux][uy];

    /* Now see if other things block our way . . */
    if (dx && dy && !Passes_walls && !can_ooze(&youmonst) && !(uwep && uwep->oartifact == ART_DOORS_ARE_NO_OBSTACLES)
		     && (IS_DOOR(ust->typ) && ((ust->doormask & ~D_BROKEN)
#ifdef REINCARNATION
			     || Is_rogue_level(&u.uz)
#endif
			     || block_entry(x, y))
			 )) {
	/* Can't move at a diagonal out of a doorway with door. */
	if (mode == DO_MOVE) pline("You cannot diagonally move out of a door!");
	return FALSE;
    }

    if (sobj_at(BOULDER,x,y) && (In_sokoban(&u.uz) || !Passes_walls)) {
	if (!(Blind || Hallucination) && (flags.run >= 2) && mode != TEST_TRAV)
	    return FALSE;
	if (mode == DO_MOVE) {
	    /* tunneling monsters will chew before pushing */
	    if ( (( (tunnels(youmonst.data) && !needspick(youmonst.data)) || (uarmf && uarmf->oartifact == ART_STONEWALL_CHECKERBOARD_DIS) || (Race_if(PM_SCURRIER) && !Upolyd) || u.geolysis) &&
		!In_sokoban(&u.uz)) && flags.eatingboulders ) {
		if (still_chewing(x,y)) return FALSE;
	    } else
		if (moverock() < 0) return FALSE;
	} else if (mode == TEST_TRAV) {
	    struct obj* obj;

	    /* don't pick two boulders in a row, unless there's a way thru */
	    if (sobj_at(BOULDER,ux,uy) && !In_sokoban(&u.uz)) {
		if (!Passes_walls &&
		    !(tunnels(youmonst.data) && !needspick(youmonst.data)) &&
		    !(Race_if(PM_SCURRIER) && !Upolyd) &&
		    !(uarmf && uarmf->oartifact == ART_STONEWALL_CHECKERBOARD_DIS) &&
		    !carrying(PICK_AXE) && !carrying(CONGLOMERATE_PICK) && !carrying(CONUNDRUM_PICK) && !carrying(MYSTERY_PICK) &&
		    !carrying(BRONZE_PICK) && !carrying(NANO_PICK) && !carrying(BRICK_PICK) && !carrying(DWARVISH_MATTOCK) &&
		    !((obj = carrying(WAN_DIGGING)) &&
		      !objects[obj->otyp].oc_name_known))
		    return FALSE;
	    }
	}
	/* assume you'll be able to push it when you get there... */
    }

    /* OK, it is a legal place to move. */
    return TRUE;
}

/*
 * Find a path from the destination (u.tx,u.ty) back to (u.ux,u.uy).
 * A shortest path is returned.  If guess is TRUE, consider various
 * inaccessible locations as valid intermediate path points.
 * Returns TRUE if a path was found.
 */
static boolean
findtravelpath(guess)
boolean guess;
{
    /* if travel to adjacent, reachable location, use normal movement rules */
    if (!guess && iflags.travel1 && distmin(u.ux, u.uy, u.tx, u.ty) == 1 && !(u.ux != u.tx && u.uy != u.ty && (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || (uarmg && uarmg->oartifact == ART_LINE_CAN_PLAY_BY_YOURSELF) || (uwep && uwep->oartifact == ART_KILLER_PIANO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_KILLER_PIANO) || isgridbug(youmonst.data) || (Race_if(PM_WEAPON_BUG) && !Upolyd)) ) ) {
	flags.run = 0;
	if (test_move(u.ux, u.uy, u.tx-u.ux, u.ty-u.uy, TEST_MOVE)) {
	    u.dx = u.tx-u.ux;
	    u.dy = u.ty-u.uy;
	    forcenomul(0, 0);
	    iflags.travelcc.x = iflags.travelcc.y = -1;
	    return TRUE;
	}
	flags.run = 8;
    }
    if (u.tx != u.ux || u.ty != u.uy) {
	xchar travel[COLNO][ROWNO];
	xchar travelstepx[2][COLNO*ROWNO];
	xchar travelstepy[2][COLNO*ROWNO];
	xchar tx, ty, ux, uy;
	int n = 1;			/* max offset in travelsteps */
	int set = 0;			/* two sets current and previous */
	int radius = 1;			/* search radius */
	int i;

	/* If guessing, first find an "obvious" goal location.  The obvious
	 * goal is the position the player knows of, or might figure out
	 * (couldsee) that is closest to the target on a straight path.
	 */
	if (guess) {
	    tx = u.ux; ty = u.uy; ux = u.tx; uy = u.ty;
	} else {
	    tx = u.tx; ty = u.ty; ux = u.ux; uy = u.uy;
	}

    noguess:
	(void) memset((void *)travel, 0, sizeof(travel));
	travelstepx[0][0] = tx;
	travelstepy[0][0] = ty;

	while (n != 0) {
	    int nn = 0;

	    for (i = 0; i < n; i++) {
		int dir;
		int x = travelstepx[set][i];
		int y = travelstepy[set][i];
		static int ordered[] = { 0, 2, 4, 6, 1, 3, 5, 7 };
		/* no diagonal movement for grid bugs */
		int dirmax = (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || (uarmg && uarmg->oartifact == ART_LINE_CAN_PLAY_BY_YOURSELF) || (uwep && uwep->oartifact == ART_KILLER_PIANO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_KILLER_PIANO) || isgridbug(youmonst.data) || (Race_if(PM_WEAPON_BUG) && !Upolyd))? 4 : 8;

		for (dir = 0; dir < dirmax; ++dir) {
		    int nx = x+xdir[ordered[dir]];
		    int ny = y+ydir[ordered[dir]];

		    if (!isok(nx, ny)) continue;
		    if ((!Passes_walls && !can_ooze(&youmonst) && !(uwep && uwep->oartifact == ART_DOORS_ARE_NO_OBSTACLES) &&
			closed_door(x, y)) || sobj_at(BOULDER, x, y)) {
			/* closed doors and boulders usually
			 * cause a delay, so prefer another path */
			if (travel[x][y] > radius-3) {
			    travelstepx[1-set][nn] = x;
			    travelstepy[1-set][nn] = y;
			    /* don't change travel matrix! */
			    nn++;
			    continue;
			}
		    }
		    if (test_move(x, y, nx-x, ny-y, TEST_TRAV) &&
			(levl[nx][ny].seenv || (!Blind && couldsee(nx, ny)))) {
			if (nx == ux && ny == uy) {
			    if (!guess) {
				u.dx = x-ux;
				u.dy = y-uy;
				if (x == u.tx && y == u.ty) {
				    forcenomul(0, 0);
				    /* reset run so domove run checks work */
				    flags.run = 8;
				    iflags.travelcc.x = iflags.travelcc.y = -1;
				}
				return TRUE;
			    }
			} else if (!travel[nx][ny]) {
			    travelstepx[1-set][nn] = nx;
			    travelstepy[1-set][nn] = ny;
			    travel[nx][ny] = radius;
			    nn++;
			}
		    }
		}
	    }
	    
	    n = nn;
	    set = 1-set;
	    radius++;
	}

	/* if guessing, find best location in travel matrix and go there */
	if (guess) {
	    int px = tx, py = ty;	/* pick location */
	    int dist, nxtdist, d2, nd2;

	    dist = distmin(ux, uy, tx, ty);
	    d2 = dist2(ux, uy, tx, ty);
	    for (tx = 1; tx < COLNO; ++tx)
		for (ty = 0; ty < ROWNO; ++ty)
		    if (travel[tx][ty]) {
			nxtdist = distmin(ux, uy, tx, ty);
			if (nxtdist == dist && couldsee(tx, ty)) {
			    nd2 = dist2(ux, uy, tx, ty);
			    if (nd2 < d2) {
				/* prefer non-zigzag path */
				px = tx; py = ty;
				d2 = nd2;
			    }
			} else if (nxtdist < dist && couldsee(tx, ty)) {
			    px = tx; py = ty;
			    dist = nxtdist;
			    d2 = dist2(ux, uy, tx, ty);
			}
		    }

	    if (px == u.ux && py == u.uy) {
		/* no guesses, just go in the general direction */
		u.dx = sgn(u.tx - u.ux);
		u.dy = sgn(u.ty - u.uy);
		if (test_move(u.ux, u.uy, u.dx, u.dy, TEST_MOVE))
		    return TRUE;
		goto found;
	    }
	    tx = px;
	    ty = py;
	    ux = u.ux;
	    uy = u.uy;
	    set = 0;
	    n = radius = 1;
	    guess = FALSE;
	    goto noguess;
	}
	return FALSE;
    }

found:
    u.dx = 0;
    u.dy = 0;
    forcenomul(0, 0);
    return FALSE;
}

boolean
ask_about_trap(int x, int y)
{

	struct trap *traphere = t_at(x, y);

	if (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) return FALSE;

	if (KnowledgeBug || u.uprops[KNOWLEDGE_BUG].extrinsic || have_trapknowledgestone()) return FALSE;

	/* hallu means you cannot tell traps apart, and we used to make it so that you never get any confirmation;
	 * it's much more fair if you always get one though --Amy
	 * what we definitely cannot do is make it give the confirmation depending on what trap it is, because that would
	 * be leaking info */
	if (Hallucination && (traphere && traphere->tseen) && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist)) return TRUE;

	if (/* is_pool(x, y) || is_lava(x, y) || */ (traphere && traphere->tseen) && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && !Hallucination)  {

		/* who the heck included this? Maybe the player doesn't really want to use the portal at all! --Amy */
		/*if (traphere->ttyp == MAGIC_PORTAL) {
			return FALSE;
		}*/

		if (SpellColorBrown && traphere->ttyp == SHIT_TRAP) return FALSE;

		if (Role_if(PM_TOPMODEL) && Is_qlocate(&u.uz)) /* traps on this level do wind damage */
			return TRUE;
		if (Role_if(PM_FAILED_EXISTENCE) && Is_qlocate(&u.uz))
			return TRUE;

		if (traphere->ttyp == S_PRESSING_TRAP) {
			return FALSE;
		}

		if (Levitation || Flying) {
			if (!In_sokoban(&u.uz) && traphere->ttyp == PIT) {
				return FALSE;
			}
			if (!In_sokoban(&u.uz) && traphere->ttyp == SPIKED_PIT) {
				return FALSE;
			}
			if (!In_sokoban(&u.uz) && traphere->ttyp == GIANT_CHASM) {
				return FALSE;
			}
			if (!In_sokoban(&u.uz) && traphere->ttyp == SHIT_PIT) {
				return FALSE;
			}
			if (!In_sokoban(&u.uz) && traphere->ttyp == MANA_PIT) {
				return FALSE;
			}
			if (!In_sokoban(&u.uz) && traphere->ttyp == ANOXIC_PIT) {
				return FALSE;
			}
			if (!In_sokoban(&u.uz) && traphere->ttyp == ACID_PIT) {
				return FALSE;
			}
			if (!In_sokoban(&u.uz) && traphere->ttyp == HOLE) {
				return FALSE;
			}
			if (!In_sokoban(&u.uz) && traphere->ttyp == SHAFT_TRAP) {
				return FALSE;
			}
			if (!In_sokoban(&u.uz) && traphere->ttyp == TRAPDOOR) {
				return FALSE;
			}
			if (traphere->ttyp == BEAR_TRAP) {
				return FALSE;
			}
			if (traphere->ttyp == ACID_POOL) {
				return FALSE;
			}
			if (traphere->ttyp == WATER_POOL) {
				return FALSE;
			}
			if (traphere->ttyp == SQKY_BOARD) {
				return FALSE;
			}
			if (traphere->ttyp == SHIT_TRAP && !(uarmf && itemhasappearance(uarmf, APP_HUGGING_BOOTS) ) ) {
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}

boolean
ask_about_water(int x, int y)
{
	if (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) return FALSE;

	if (is_pool(u.ux, u.uy)) return FALSE;

	if (is_pool(x, y) && !Levitation && !Flying && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && levl[x][y].seenv) return TRUE; 

	if (is_pool(x, y) && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && levl[x][y].seenv && Role_if(PM_TOPMODEL) && Is_qlocate(&u.uz) ) return TRUE; 
	if (is_pool(x, y) && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && levl[x][y].seenv && Role_if(PM_FAILED_EXISTENCE) && Is_qlocate(&u.uz) ) return TRUE; 

	return FALSE;
}

boolean
ask_about_lava(int x, int y)
{
	if (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) return FALSE;

	if (is_lava(u.ux, u.uy)) return FALSE;

	if (is_lava(x, y) && !Levitation && !Flying && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && levl[x][y].seenv) return TRUE; 
	return FALSE;
}

boolean
ask_about_watertunnel(int x, int y)
{
	if (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) return FALSE;

	if (is_watertunnel(u.ux, u.uy)) return FALSE;

	if (is_watertunnel(x, y) && !Levitation && !Flying && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && levl[x][y].seenv) return TRUE; 
	return FALSE;
}

boolean
ask_about_crystalwater(int x, int y)
{
	if (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) return FALSE;

	if (is_crystalwater(u.ux, u.uy)) return FALSE;

	if (is_crystalwater(x, y) && (Levitation || Flying) && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && levl[x][y].seenv) return TRUE; 
	return FALSE;
}

boolean
ask_about_moorland(int x, int y)
{
	if (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) return FALSE;

	if (is_moorland(u.ux, u.uy)) return FALSE;

	if (is_moorland(x, y) && !Levitation && !Flying && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && levl[x][y].seenv) return TRUE; 
	return FALSE;
}

boolean
ask_about_urinelake(int x, int y)
{
	if (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) return FALSE;

	if (is_urinelake(u.ux, u.uy)) return FALSE;

	if (is_urinelake(x, y) && !Levitation && !Flying && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && levl[x][y].seenv) return TRUE; 
	return FALSE;
}

boolean
ask_about_shiftingsand(int x, int y)
{
	if (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) return FALSE;

	if (is_shiftingsand(u.ux, u.uy)) return FALSE;

	if (is_shiftingsand(x, y) && !Levitation && !Flying && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && levl[x][y].seenv) return TRUE; 
	return FALSE;
}

boolean
ask_about_styxriver(int x, int y)
{
	if (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) return FALSE;

	if (is_styxriver(u.ux, u.uy)) return FALSE;

	if (is_styxriver(x, y) && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && levl[x][y].seenv) return TRUE; 
	return FALSE;
}

boolean
ask_about_burningwagon(int x, int y)
{
	if (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) return FALSE;

	if (is_burningwagon(u.ux, u.uy)) return FALSE;

	if (is_burningwagon(x, y) && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && levl[x][y].seenv) return TRUE; 
	return FALSE;
}

boolean
ask_about_nethermist(int x, int y)
{
	if (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) return FALSE;

	if (is_nethermist(u.ux, u.uy)) return FALSE;

	if (is_nethermist(x, y) && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && levl[x][y].seenv) return TRUE; 
	return FALSE;
}

boolean
ask_about_stalactite(int x, int y)
{
	if (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) return FALSE;

	if (is_stalactite(u.ux, u.uy)) return FALSE;

	if (is_stalactite(x, y) && (Flying || Levitation) && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && levl[x][y].seenv) return TRUE; 
	return FALSE;
}

boolean
ask_about_raincloud(int x, int y)
{
	if (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) return FALSE;

	if (is_raincloud(u.ux, u.uy)) return FALSE;

	if (is_raincloud(x, y) && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && levl[x][y].seenv) return TRUE; 
	return FALSE;
}

boolean
ask_about_bubble(int x, int y)
{
	if (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) return FALSE;

	if (is_bubble(u.ux, u.uy)) return FALSE;

	if (is_bubble(x, y) && !(Confusion && !Conf_resist) && !(Stunned && !Stun_resist) && levl[x][y].seenv) return TRUE; 
	return FALSE;
}

void
domove()
{
	register struct monst *mtmp;
	register struct rm *tmpr;
	register xchar x,y;
	struct trap *trap;
	struct trap *traphere = t_at(x, y);
	int wtcap;
	boolean on_ice;
	xchar chainx, chainy, ballx, bally;	/* ball&chain new positions */
	int bc_control;				/* control for ball&chain */
	boolean cause_delay = FALSE;	/* dragging ball will skip a move */
	const char *predicament;
	boolean displacer = FALSE;	/* defender attempts to displace you */
	boolean peacedisplacer = FALSE;
	boolean canexistonsquare = TRUE;

	u_wipe_engr(rnd(5));

	if (flags.travel) {
	    if (!findtravelpath(FALSE))
		(void) findtravelpath(TRUE);
	    iflags.travel1 = 0;
	}

	if(((wtcap = near_capacity()) >= OVERLOADED
	    || (wtcap > SLT_ENCUMBER &&
		(Upolyd ? (u.mh < 5 && u.mh != u.mhmax)
			: (u.uhp < 10 && u.uhp != u.uhpmax))))
	   && !Is_airlevel(&u.uz)) {
	    if(wtcap < OVERLOADED) {
		You("don't have enough stamina to move.");
		exercise(A_CON, FALSE);
	    } else
		You("collapse under your load.");
	    forcenomul(0, 0);
	    return;
	}
	if (Race_if(PM_ELONA_SNAIL) && !flags.forcefight && uarmf && !PlayerInHighHeels) {
	    pline("In order to move, snails need to be bare-footed or wearing high heels!");
	    forcenomul(0, 0);
	    return;
	}

	if(u.uswallow) {
		u.dx = u.dy = 0;
		u.ux = x = u.ustuck->mx;
		u.uy = y = u.ustuck->my;
		mtmp = u.ustuck;
	} else {
		if (Is_airlevel(&u.uz) && !flags.forcefight && rn2(2) && /* was rn2(4) - let's make it a bit easier --Amy */
			!Levitation && !Flying) {
		    switch(rn2(3)) {
		    case 0:
			You("tumble in place.");
			exercise(A_DEX, FALSE);
			break;
		    case 1:
			You_cant("control your movements very well."); break;
		    case 2:
			pline("It's hard to walk in thin air.");
			exercise(A_DEX, TRUE);
			break;
		    }
		    return;
		}

		/* check slippery ice */
		on_ice = !Levitation && is_ice(u.ux, u.uy);
		if (on_ice) {

			if (uarmf && uarmf->oartifact == ART_MERLOT_FUTURE && !(HFast)) {
				incr_itimeout(&HFast, rnd(500));
				pline("Vrooooom, your ski heels speed up thanks to walking on snow!");
			}

		    static int skates = 0;
		    if (!skates) skates = find_skates();
		    static int skates2 = 0;
		    if (!skates2) skates2 = find_skates2();
		    static int skates3 = 0;
		    if (!skates3) skates3 = find_skates3();
		    static int skates4 = 0;
		    if (!skates4) skates4 = find_skates4();
		    static int skates5 = 0;
		    if (!skates5) skates5 = find_cyan_sneakers();
		    if ((uarmf && uarmf->otyp == skates)
			    || (powerfulimplants() && uimplant && uimplant->oartifact == ART_WHITE_WHALE_HATH_COME)
			    || (uarmf && uarmf->otyp == skates2)
			    || (uarmf && uarmf->otyp == skates3)
			    || (uarmf && uarmf->otyp == skates4)
			    || (uarmf && uarmf->otyp == skates5)
			    || (uwep && uwep->oartifact == ART_GLACIERDALE)
			    || (uarmf && uarmf->oartifact == ART_BRIDGE_SHITTE)
			    || (uarmf && uarmf->oartifact == ART_MERLOT_FUTURE)
			    || (uarmf && uarmf->oartifact == ART_IMPOSSIBLE_CATWALK)
			    || (uwep && uwep->oartifact == ART_DAMN_SKI_WEDGE && uarmf)
			    || resists_cold(&youmonst) || Flying
			    || is_floater(youmonst.data) || is_clinger(youmonst.data)
			    || is_whirly(youmonst.data))
			on_ice = FALSE;
		    else if (!rn2(Cold_resistance ? 3 : 2)) {
			HFumbling |= FROMOUTSIDE;
			HFumbling &= ~TIMEOUT;
			HFumbling += 1;  /* slip on next move */
		    }
		}
		if (!on_ice && !u.fumbleduration && (HFumbling & FROMOUTSIDE))
		    HFumbling &= ~FROMOUTSIDE;

		x = u.ux + u.dx;
		y = u.uy + u.dy;
		/* Check if your steed can move */
		if (u.usteed && !flags.forcefight && (!u.usteed->mcanmove || u.usteed->msleeping)) {
		    Your("steed doesn't respond!");
		    forcenomul(0, 0);
		    return;
		}

	/* In Soviet Russia, stunning is a crippling status effect that will fuck you up. You're not supposed to stand
	 * any chance while stunned, because seriously, players having a chance? That's a no-go! --Amy */

		if ((Stunned && !rn2(issoviet ? 1 : StrongStun_resist ? 20 : Stun_resist ? 8 : 2)) || (Confusion && !rn2(issoviet ? 2 : StrongConf_resist ? 200 : Conf_resist ? 40 : 8) || ((uarmh && itemhasappearance(uarmh, APP_THINKING_HELMET)) && !rn2(8) ) )
			/* toned down so it's less crippling --Amy
			 * nerf for extremely fast steeds: they cause you to sometimes walk randomly */
			|| (u.usteed && ((u.usteed->mconf && confsteeddir()) || (u.usteed->data->mmove > 36 && rnd(u.usteed->data->mmove) > 36) ) )
		   ) {
			register int tries = 0;

			do {
				if(tries++ > 50) {
					forcenomul(0, 0);
					return;
				}
				confdir();
				x = u.ux + u.dx;
				y = u.uy + u.dy;
			} while(!isok(x, y) || bad_rock(&youmonst, x, y));
		}
		/* turbulence might alter your actual destination */
		if (u.uinwater) {
			water_friction();
			if (!u.dx && !u.dy) {
				forcenomul(0, 0);
				return;
			}
			x = u.ux + u.dx;
			y = u.uy + u.dy;
		}
		if(!isok(x, y)) {
			forcenomul(0, 0);
			return;
		}
		if (((trap = t_at(x, y)) && trap->tseen) ||
		    (Blind && !Levitation && !Flying &&
		     !is_clinger(youmonst.data) &&
		     (is_waterypool(x, y) || is_watertunnel(x,y) || is_shiftingsand(x,y) || is_moorland(x,y) || is_urinelake(x,y) || is_lava(x, y)) && levl[x][y].seenv)) {
			if(flags.run >= 2) {
				forcenomul(0, 0);
				flags.move = 0;
				return;
			} else
				forcenomul(0, 0);
		}

		if (u.ustuck && (x != u.ustuck->mx || y != u.ustuck->my)) {
		    if (distu(u.ustuck->mx, u.ustuck->my) > 2) {
			/* perhaps it fled (or was teleported or ... ) */
			setustuck(0);
		    } else if (sticks(youmonst.data)) {
			/* When polymorphed into a sticking monster,
			 * u.ustuck means it's stuck to you, not you to it.
			 */
			You("release %s.", mon_nam(u.ustuck));
			setustuck(0);
		    } else {
			/* If holder is asleep or paralyzed:
			 *	37.5% chance of getting away,
			 *	12.5% chance of waking/releasing it;
			 * otherwise:
			 *	 7.5% chance of getting away.
			 * [strength ought to be a factor]
			 * If holder is tame and there is no conflict,
			 * guaranteed escape.
			 */
			switch (rn2(!u.ustuck->mcanmove ? 8 : 40)) {
			case 0: case 1: case 2:
			pull_free:
			    You("pull free from %s.", mon_nam(u.ustuck));
			    setustuck(0);
			    break;
			case 3:
			    if (!u.ustuck->mcanmove) {
				/* it's free to move on next turn */
				u.ustuck->mfrozen = 1;
				u.ustuck->msleeping = 0;
			    }
			    /*FALLTHRU*/
			default:
			    if (u.ustuck->mtame &&
				!Conflict && !u.ustuck->mconf)
				goto pull_free;
			    You("cannot escape from %s!", mon_nam(u.ustuck));
			    nomul(0, 0, FALSE);
			    return;
			}
		    }
		}

		mtmp = m_at(x,y);
		if (mtmp) {
			/* Don't attack if you're running, and can see it */
			/* We should never get here if forcefight */
			if (flags.run &&
			    ((!Blind && mon_visible(mtmp) &&
			      ((mtmp->m_ap_type != M_AP_FURNITURE &&
				mtmp->m_ap_type != M_AP_OBJECT) ||
			       Protection_from_shape_changers)) ||
			     sensemon(mtmp))) {
				forcenomul(0, 0);
				flags.move = 0;
				return;
			}
		}
	}
	u.ux0 = u.ux;
	u.uy0 = u.uy;
	bhitpos.x = x;
	bhitpos.y = y;
	tmpr = &levl[x][y];
	/* attack monster */

	if(mtmp) {

	    forcenomul(0, 0);
	    /* only attack if we know it's there */
	    /* or if we used the 'F' command to fight blindly */
	    /* or if it hides_under, in which case we call attack() to print
	     * the Wait! message.
	     * This is different from ceiling hiders, who aren't handled in
	     * attack().
	     */

	    /* If they used a 'm' command, trying to move onto a monster
	     * prints the below message and wastes a turn.  The exception is
	     * if the monster is unseen and the player doesn't remember an
	     * invisible monster--then, we fall through to attack() and
	     * attack_check(), which still wastes a turn, but prints a
	     * different message and makes the player remember the monster. */
	    if(flags.nopick &&
		  (canspotmon(mtmp) || memory_is_invisible(x, y))){
		if(mtmp->m_ap_type && !Protection_from_shape_changers
						    && !sensemon(mtmp))
		    stumble_onto_mimic(mtmp);
		else if (mtmp->mpeaceful && !Hallucination)
		    pline("Pardon me, %s.", m_monnam(mtmp));
		else
		    You("move right into %s.", mon_nam(mtmp));
		return;
	    }

	    /* If there is ANYthing that might be a remembered monster,
	     * the player should goddamn ATTACK IT and NOT waste a turn. GRRRR. --Amy */
	    if(flags.forcefight || memory_is_invisible(x, y) || mon_warning(mtmp) || !mtmp->mundetected || sensemon(mtmp) ||
		    ((hides_under(mtmp->data) || mtmp->data->mlet == S_EEL) &&
			!is_safepet(mtmp))){
		gethungry();
		if(wtcap >= HVY_ENCUMBER && moves%3) {
		    if (Upolyd && u.mh > 1) {
			u.mh--;
		    } else if (!Upolyd && u.uhp > 1) {
			u.uhp--;
		    } else {
			You("pass out from exertion!");
			exercise(A_CON, FALSE);
			fall_asleep(-10, FALSE);
		    }
		}
		if(multi < 0) return;	/* we just fainted */
		/* new displacer beast thingie -- by [Tom] */
		/* sometimes, instead of attacking, you displace it. */
		/* Good joke, huh? */
		if ( (mtmp->data == &mons[PM_DISPLACER_BEAST] || mtmp->data == &mons[PM_SARTAN_TANNIN] || mtmp->data == &mons[PM_POLYMORPH_CODE] || mtmp->data == &mons[PM_FIRST_WRAITHWORM] || mtmp->data == &mons[PM_WRAITHWORM] || mtmp->data == &mons[PM_LILAC_FEMMY] || mtmp->data == &mons[PM_SHARAB_KAMEREL] || mtmp->data == &mons[PM_WUXTINA] || mtmp->data == &mons[PM_IVEL_WUXTINA] || mtmp->data == &mons[PM_FLUTTERBUG] || mtmp->data == &mons[PM_ORTHOS] || mtmp->data == &mons[PM_SHIMMERING_DRACONIAN] || mtmp->data == &mons[PM_JUMPING_CHAMPION] || mtmp->data->mlet == S_GRUE || mtmp->data == &mons[PM_QUANTUM_MOLD] || mtmp->data == &mons[PM_QUANTUM_GROWTH] || mtmp->data == &mons[PM_QUANTUM_FUNGUS] || mtmp->data == &mons[PM_QUANTUM_PATCH] || mtmp->data == &mons[PM_QUANTUM_STALK] || mtmp->data == &mons[PM_QUANTUM_MUSHROOM] || mtmp->data == &mons[PM_QUANTUM_SPORE] || mtmp->data == &mons[PM_QUANTUM_COLONY] || mtmp->data == &mons[PM_QUANTUM_FORCE_FUNGUS] || mtmp->data == &mons[PM_QUANTUM_WORT] || mtmp->data == &mons[PM_QUANTUM_FORCE_PATCH] || mtmp->data == &mons[PM_QUANTUM_WARP_FUNGUS] || mtmp->data == &mons[PM_QUANTUM_WARP_PATCH] || mtmp->egotype_displacer) && !rn2(2))
		    displacer = TRUE; /* grues can also displace the player to make them more annoying --Amy */
		else if (tech_inuse(T_EDDY_WIND)) peacedisplacer = TRUE;
		else if (u.swappositioncount) peacedisplacer = TRUE;
		else if (uwep && uwep->oartifact == ART_DIZZY_METAL_STORM) peacedisplacer = TRUE;
		/* Displacement allows the player to displace peaceful things --Amy */
		else if (Displaced && !mtmp->isshk && !mtmp->ispriest && mtmp->mpeaceful) peacedisplacer = TRUE;
		else
		/* try to attack; note that it might evade */
		/* also, we don't attack tame when _safepet_ */

		if(attack(mtmp)) return;

		if (tech_inuse(T_EDDY_WIND) && flags.forcefight) {
			if(attack(mtmp)) return;
		}
		if (u.swappositioncount && flags.forcefight) {
			if(attack(mtmp)) return;
		}
		if (uwep && uwep->oartifact == ART_DIZZY_METAL_STORM && flags.forcefight) {
			if(attack(mtmp)) return;
		}
	    }
	}

	/* fucked up bug where we can reach here even when engulfed and waste a turn... --Amy
	 * This could crash the game if the player is punished, which we'll prevent by just aborting the player's move */
	if(u.uswallow) {
		pline("Caution: You seem to have encountered the bug where moving into an engulfer wastes a turn. In order to attack the monster, prefix your movement with F (shift-f), please. Sorry for the inconvenience. --Amy");
		return;
	}

	/* Amy feature: safety question when displacing pets into liquids, impossible to do it to non-pets (too cheesy) */
	if (mtmp && (displacer || peacedisplacer || mtmp->mtame)) {
		boolean inpool, inlava;

		inpool = (is_waterypool(u.ux, u.uy) || is_watertunnel(u.ux, u.uy)) && !is_flyer(mtmp->data) && !(is_urinelake(u.ux, u.uy)) && !(is_moorland(u.ux, u.uy)) && !(is_crystalwater(u.ux, u.uy)) && (!mtmp->egotype_flying) && !is_floater(mtmp->data);
		inlava = is_lava(u.ux, u.uy) && !is_flyer(mtmp->data) && (!mtmp->egotype_flying) && !is_floater(mtmp->data);

		if (inpool || inlava) canexistonsquare = FALSE;
	}

	if (!canexistonsquare) {
		if (mtmp && !mtmp->mtame) {
			You("cannot displace that monster into a harmful liquid!");
			return;
		} else if (mtmp && mtmp->mtame) {
			if (!(ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone())) {
				if (yn("Really displace the pet into a harmful liquid?") != 'y') {
					return;
				}
			}
		}
	}

	/* specifying 'F' with no monster wastes a turn */
	if (flags.forcefight ||
	    /* remembered an 'I' && didn't use a move command */
	    (memory_is_invisible(x, y) && !flags.nopick)) {
		boolean expl = (Upolyd && attacktype(youmonst.data, AT_EXPL));
	    	char buf[BUFSZ];
		sprintf(buf,"a vacant spot on the %s", surface(x,y));
		You("%s %s.",
		    expl ? "explode at" : "attack",
		    !Underwater ? "thin air" :
		    is_waterypool(x,y) ? "empty water" : buf);
		unmap_object(x, y); /* known empty -- remove 'I' if present */
		newsym(x, y);
		forcenomul(0, 0);
		if (expl) {
	    	    if (!Race_if(PM_UNGENOMOLD)) {
		      u.mh = -1;		/* dead in the current form */
			rehumanize();
		    } else polyself(FALSE);
		}
		return;
	}
	if (memory_is_invisible(x, y)) {
	    unmap_object(x, y);
	    newsym(x, y);
	}
	/* not attacking an animal, so we try to move */
	if (!displacer) {

	if (peacedisplacer) goto peacedisplace;

	if (u.usteed && !u.usteed->mcanmove && (u.dx || u.dy)) {
		pline("%s won't move!", upstart(y_monnam(u.usteed)));
		forcenomul(0, 0);
		return;
	} else
	if( is_nonmoving(youmonst.data) && !Race_if(PM_MISSINGNO) && !(uactivesymbiosis && Upolyd && (u.umonnum == u.usymbiote.mnum)) ) {
	/* This catches the moveamt code in hack.c, preventing you from moving as a red mold
	 * even if you do get some movement points. It's mainly meant to prevent you from being unable to do anything
	 * until you get knocked out of red mold form, so you can at least sit around and maybe throw some daggers. --Amy */
		You("are rooted %s.",
		    Levitation || Is_airlevel(&u.uz) || Is_waterlevel(&u.uz) ?
		    "in place" : "to the ground");
		forcenomul(0, 0);
		return;
	}
	if(u.utrap) {
		if(u.utraptype == TT_PIT) {
		    if (!rn2(2) && sobj_at(BOULDER, u.ux, u.uy)) {
			Your("%s gets stuck in a crevice.", body_part(LEG));
			display_nhwindow(WIN_MESSAGE, FALSE);
			clear_nhwindow(WIN_MESSAGE);
			You("free your %s.", body_part(LEG));
		    } else if (Flying && !In_sokoban(&u.uz)) {
			/* eg fell in pit, poly'd to a flying monster */
			You("fly from the pit.");
			u.utrap = 0;
			fill_pit(u.ux, u.uy);
			vision_full_recalc = 1;	/* vision limits change */
		    } else if (!(--u.utrap)) {
			You("%s to the edge of the pit.",
				(In_sokoban(&u.uz) && Levitation) ?
				"struggle against the air currents and float" :
				u.usteed ? "ride" : "crawl");
			fill_pit(u.ux, u.uy);
			vision_full_recalc = 1;	/* vision limits change */
		    } else if (flags.verbose) {
			if (u.usteed)
			    Norep("%s is still in a pit.",
				  upstart(y_monnam(u.usteed)));
			else
			Norep( (FunnyHallu && !rn2(5)) ?
				"You've fallen, and you can't get up." :
				"You are still in a pit." );
		    }

			if (FemtrapActiveNatalje) {
				u.nataljetrapturns = moves;
				u.nataljetrapx = u.ux;
				u.nataljetrapy = u.uy;
			}

		      traphere = t_at(u.ux,u.uy);
		      if (u.utrap && traphere && traphere->ttyp == SHIT_PIT) {
				pline("You splotch into a heap of dog shit!");
			      int num = 0;
			      num = d(2, 2) + rnd((monster_difficulty() / 3) + 1);
			      if (Acid_resistance) { /* let's just assume the stuff is acidic or corrosive --Amy */
			      	shieldeff(u.ux, u.uy);
			           num = d(1, 2) + rnd((monster_difficulty() / 7) + 1);
			      }
				if (Stoned) fix_petrification();

				if (!rn2(10) || !(uarmf && itemhasappearance(uarmf, APP_PROFILED_BOOTS) ) ) {

				if (uarmf && !rn2(5)) (void)rust_dmg(uarmf, xname(uarmf), 0, TRUE, &youmonst);
				if (uarmf && !rn2(5)) (void)rust_dmg(uarmf, xname(uarmf), 1, TRUE, &youmonst);
				if (uarmf && !rn2(5)) (void)rust_dmg(uarmf, xname(uarmf), 2, TRUE, &youmonst);
				if (uarmf && !rn2(5)) (void)rust_dmg(uarmf, xname(uarmf), 3, TRUE, &youmonst);
			/* Dog shit is extremely aggressive to footwear. Let's give it a chance to do withering damage. --Amy */
				if (uarmf && !rn2(25)) (void)wither_dmg(uarmf, xname(uarmf), 0, TRUE, &youmonst);
				if (uarmf && !rn2(25)) (void)wither_dmg(uarmf, xname(uarmf), 1, TRUE, &youmonst);
				if (uarmf && !rn2(25)) (void)wither_dmg(uarmf, xname(uarmf), 2, TRUE, &youmonst);
				if (uarmf && !rn2(25)) (void)wither_dmg(uarmf, xname(uarmf), 3, TRUE, &youmonst);

				}

				if (!uarmf) {
					pline("You slip on the shit with your bare %s.", makeplural(body_part(FOOT)));
					num *= 2;
				}

				if (uarmf ? !rn2(20) : !rn2(15)) {
					HFumbling = FROMOUTSIDE | rnd(5);
					incr_itimeout(&HFumbling, rnd(2));
					u.fumbleduration += rnz(uarmf ? 30 : 20);

				}

				if (uarmf && itemhasappearance(uarmf, APP_PROFILED_BOOTS) ) {
				    if (!(HFast & INTRINSIC)) {
					if (!Fast)
					    You("speed up.");
					else
					    Your("quickness feels more natural.");
					exercise(A_DEX, TRUE);
				    }
				    HFast |= FROMOUTSIDE;

				} else if (!rn2(20)) u_slow_down();

				if ( !rn2(StrongFree_action ? 1000 : 100) || (!Free_action && !rn2(10)))	{
					You("inhale the intense smell of shit! The world spins and goes dark.");
					nomovemsg = "You are conscious again.";	/* default: "you can move again" */
					nomul(-rnd(10), "unconscious from smelling dog shit", TRUE);
					exercise(A_DEX, FALSE);
				}

			      if (uarmf && itemhasappearance(uarmf, APP_PROFILED_BOOTS) ) num /= 4;
			      if (num) losehp(num, "heap of shit", KILLED_BY_AN);

			}

		      if (u.utrap && traphere && traphere->ttyp == MANA_PIT) {
			    drain_en(rnz(monster_difficulty() + 1));
			}

		} else if (u.utraptype == TT_LAVA) { /* WHO THE HELL MADE THIS INTO A NOREP GAAAAAAH --Amy */

			if (FemtrapActiveNatalje) {
				u.nataljetrapturns = moves;
				u.nataljetrapx = u.ux;
				u.nataljetrapy = u.uy;
			}

		    if(flags.verbose) {
			predicament = "stuck in the lava";
			if (u.usteed)
			    pline("%s is %s.", upstart(y_monnam(u.usteed)),
				  predicament);
			else
			pline("You are %s.", predicament);
		    }
		    if(!is_lava(x,y)) {
			u.utrap--;
			if((u.utrap & 0xff) == 0) {
			    if (u.usteed)
				You("lead %s to the edge of the lava.",
				    y_monnam(u.usteed));
			    else
			     You("pull yourself to the edge of the lava.");
			    u.utrap = 0;
			}
		    }
		    u.umoved = TRUE;
		} else if (u.utraptype == TT_WEB) {
		    if(uwep && uwep->oartifact == ART_STING) {
			u.utrap = 0;
			pline("Sting cuts through the web!");
			return;
		    }

			if (FemtrapActiveNatalje) {
				u.nataljetrapturns = moves;
				u.nataljetrapx = u.ux;
				u.nataljetrapy = u.uy;
			}

		    if(--u.utrap) {
			if(flags.verbose) {
			    predicament = "stuck to the web";
			    if (u.usteed)
				Norep("%s is %s.", upstart(y_monnam(u.usteed)),
				      predicament);
			    else
			    Norep("You are %s.", predicament);
			}
		    } else {
			if (u.usteed)
			    pline("%s breaks out of the web.",
				  upstart(y_monnam(u.usteed)));
			else
			You("disentangle yourself.");
		    }
		} else if (u.utraptype == TT_GLUE) {

			if (FemtrapActiveNatalje) {
				u.nataljetrapturns = moves;
				u.nataljetrapx = u.ux;
				u.nataljetrapy = u.uy;
			}

		    if(--u.utrap) {
			if(flags.verbose) {
			    predicament = "held in place by the glue";
			    if (u.usteed)
				Norep("%s is %s.", upstart(y_monnam(u.usteed)),
				      predicament);
			    else
			    Norep("You are %s.", predicament);
			}
		    } else {
			if (u.usteed)
			    pline("%s breaks out of the glue.",
				  upstart(y_monnam(u.usteed)));
			else
			You("finally get the sticky glue off.");
		    }
		} else if (u.utraptype == TT_INFLOOR) {

			if (FemtrapActiveNatalje) {
				u.nataljetrapturns = moves;
				u.nataljetrapx = u.ux;
				u.nataljetrapy = u.uy;
			}

		    if(--u.utrap) {
			if(flags.verbose) {
			    predicament = "stuck in the";
			    if (u.usteed)
				Norep("%s is %s %s.",
				      upstart(y_monnam(u.usteed)),
				      predicament, surface(u.ux, u.uy));
			    else
			    Norep("You are %s %s.", predicament,
				  surface(u.ux, u.uy));
			}
		    } else {
			if (u.usteed)
			    pline("%s finally wiggles free.",
				  upstart(y_monnam(u.usteed)));
			else
			You("finally wiggle free.");
		    }
		} else {

			if (FemtrapActiveNatalje) {
				u.nataljetrapturns = moves;
				u.nataljetrapx = u.ux;
				u.nataljetrapy = u.uy;
			}

		    if(flags.verbose) {
			predicament = "caught in a bear trap";
			if (u.usteed)
			    Norep("%s is %s.", upstart(y_monnam(u.usteed)),
				  predicament);
			else
			Norep("You are %s.", predicament);
		    }
		    if((u.dx && u.dy) || !rn2(5)) {
				u.utrap--;
				 if (u.utrap == 0) pline("You pull free from the trap."); 
				else pline("You try to get out of the trap.");}
		}
		return;
	}

	if (!test_move(u.ux, u.uy, x-u.ux, y-u.uy, DO_MOVE)) {
            if (!door_opened) {
	        flags.move = 0;
	        forcenomul(0, 0);
            } else {
                door_opened = 0;
            }
		return;
	}

	if (t_at(x,y)) {

	    traphere = t_at(x,y);
	    if (traphere && traphere->ttyp == NUPESELL_TRAP) {
		flags.move = 0;
		forcenomul(0, 0);
		return;
	    }

	}

peacedisplace:

	/* warn player before walking into known traps */
	if (ask_about_trap(x, y)) {
		char qbuf[BUFSZ];
		trap = t_at(x, y);
		sprintf(qbuf,"Do you really want to %s into that %s?", 
				locomotion(youmonst.data, "step"),
				Hallucination ? "trap" : defsyms[trap_to_defsym(trap->ttyp)].explanation);
		if (yn(qbuf) != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
		}
	}

	if (ask_about_water(x, y)) {

		if (Role_if(PM_TOPMODEL) && Is_qlocate(&u.uz) ) {
		if (yn("There is a strong wind above the water. It seems dangerous. Really step there?") != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
			}
		}
		if (Role_if(PM_FAILED_EXISTENCE) && Is_qlocate(&u.uz) ) {
		if (yn("There is a strong wind above the water. It seems dangerous. Really step there?") != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
			}
		}

		else {
		if (yn("This is a water tile that can cause you to drown. Really step on it?") != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
			}
		}

	}

	if (ask_about_lava(x, y)) {

		if (yn("This is a lava tile that can burn you to a crisp. Really step on it?") != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
		}
		char bufX[BUFSZ];
		getlin ("Are you really sure [yes/no]?",bufX);
		if (strcmp (bufX, "yes")) {
			forcenomul(0, 0);
			flags.move = 0;
			return;
		}
	}

	if (ask_about_watertunnel(x, y)) {

		if (yn("This is a water tunnel that can cause you to drown. Really dive into it?") != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
		}
	}

	if (ask_about_crystalwater(x, y)) {

		if (yn("This is a crystal water tile that can cause you to drown. Really fly into it?") != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
		}
	}

	if (ask_about_moorland(x, y)) {

		if (yn("This is moorland; swimming in it will continuously hurt you. Really do it?") != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
		}
	}

	if (ask_about_urinelake(x, y)) {

		if (yn("This is a urine lake, which can be detrimental to swim in. Really do it?") != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
		}
	}

	if (ask_about_shiftingsand(x, y)) {

		if (yn("This is a shifting sand tile, which will quickly kill you by suffocation. Really step on it?") != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
		}
		char bufX[BUFSZ];
		getlin ("Are you really sure [yes/no]?",bufX);
		if (strcmp (bufX, "yes")) {
			forcenomul(0, 0);
			flags.move = 0;
			return;
		}
	}

	if (ask_about_styxriver(x, y)) {

		if (yn("This is a styx river, which will continuously contaminate you. Really step on it?") != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
		}
	}

	if (ask_about_burningwagon(x, y)) {

		if (yn("This is a burning wagon, which will burn you. Really step on it?") != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
		}
	}

	if (ask_about_nethermist(x, y)) {

		if (yn("This is a nether mist tile, which can drain your experience. Really step on it?") != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
		}
	}

	if (ask_about_stalactite(x, y)) {

		if (yn("This is a stalactite, which will hurt you if you fly into it. Really do it?") != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
		}
	}

	if (ask_about_raincloud(x, y)) {

		if (yn("This is a rain cloud, which will make you and your entire inventory wet. Really step into it?") != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
		}
	}

	if (ask_about_bubble(x, y)) {

		if (yn("This is a bubble, which causes stunning. Really step into it?") != 'y') {
			forcenomul(0, 0);
			flags.move = 0;
			return;
		}
	}

	} else if (!test_move(u.ux, u.uy, x-u.ux, y-u.uy, TEST_MOVE)) {
	    /*
	     * If a monster attempted to displace us but failed
	     * then we are entitled to our normal attack.
	     */
	    if (!attack(mtmp)) {
		flags.move = 0;
		forcenomul(0, 0);
	    }
	    return;
	}

	/* Move ball and chain.  */
	if (Punished)
	    if (!drag_ball(x,y, &bc_control, &ballx, &bally, &chainx, &chainy,
			&cause_delay, TRUE))
		return;

	/* Check regions entering/leaving */
	if (!in_out_region(x,y)) {
#if 0
	    /* [ALI] This can't happen at present, but if it did we would
	     * also need to worry about the call to drag_ball above.
	     */
	    if (displacer) (void)attack(mtmp);
#endif
	    return;
	}

 	/* now move the hero */
	mtmp = m_at(x, y);

	u.ux += u.dx;
	u.uy += u.dy;

	if (TronEffect || u.uprops[TRON_EFFECT].extrinsic || have_tronstone()) {
		if (u.dx == 1 && !u.dy) u.trontrapdirection = 1;
		else if (u.dx == 1 && u.dy == 1) u.trontrapdirection = 2;
		else if (!u.dx && u.dy == -1) u.trontrapdirection = 3;
		else if (u.dx == -1 && u.dy == -1) u.trontrapdirection = 4;
		else if (u.dx == -1 && !u.dy) u.trontrapdirection = 5;
		else if (u.dx == -1 && u.dy == 1) u.trontrapdirection = 6;
		else if (!u.dx && u.dy == 1) u.trontrapdirection = 7;
		else if (u.dx == 1 && u.dy == -1) u.trontrapdirection = 8;
		else u.trontrapdirection = -1;

		u.trontrapturn = moves;

	}

	/* Move your steed, too */
	if (u.usteed) {
		u.usteed->mx = u.ux;
		u.usteed->my = u.uy;
		exercise_steed();
	}

	/* exercising high heel skill takes a while... */
	if (PlayerInHighHeels && (u.uhighheelturns++ >= 50)) {
	    u.uhighheelturns = 0;
	    use_skill(P_HIGH_HEELS, 1);
	}

	/* exercising sexy flats does too, especially if you're male */
	if (PlayerInSexyFlats && (flags.female || !rn2(5)) && (u.usexyflatturns++ >= 50)) {
	    u.usexyflatturns = 0;
	    use_skill(P_SEXY_FLATS, 1);
	}

	if (displacer || peacedisplacer) {
	    char pnambuf[BUFSZ];

	    u.utrap = 0;			/* A lucky escape */
	    /* save its current description in case of polymorph */
	    strcpy(pnambuf, mon_nam(mtmp));
	    remove_monster(x, y);
	    place_monster(mtmp, u.ux0, u.uy0);

	    if (u.swappositioncount) u.swappositioncount = 0;

	    /* check for displacing it into pools and traps */
	    switch (minliquid(mtmp) ? 2 : mintrap(mtmp)) {
		case 0:
		    if (tech_inuse(T_EDDY_WIND)) You("whirl past %s.", pnambuf);
		    else You("displaced %s.", pnambuf);
		    break;
		case 1:
		case 3:
		    break;
		case 2:
		    u.uconduct.killer++;
		    break;
	    }
	}

	/*
	 * if safepet at destination then move the pet to the hero's
	 * previous location using the same conditions as in attack().
	 * there are special extenuating circumstances:
	 * (1) if the pet dies then your god angers,
	 * (2) if the pet gets trapped then your god may disapprove,
	 * (3) if the pet was already trapped and you attempt to free it
	 * not only do you encounter the trap but you may frighten your
	 * pet causing it to go wild!  moral: don't abuse this privilege.
	 *
	 * Ceiling-hiding pets are skipped by this section of code, to
	 * be caught by the normal falling-monster code.
	 * Amy addition: displacement already allows you to displace monsters; don't call the displacing code twice
	 */
	if (is_safepet(mtmp) && !Displaced && !( (is_hider(mtmp->data) || mtmp->egotype_hide || mtmp->egotype_mimic) && mtmp->mundetected)) {
	    /* if trapped, there's a chance the pet goes wild */
	    if (mtmp->mtrapped) {
		if (!rn2(mtmp->mtame) && !((rnd(30 - ACURR(A_CHA))) < 4) ) {
		    mtmp->mtame = mtmp->mpeaceful = mtmp->msleeping = 0;
		    if (mtmp->mleashed) m_unleash(mtmp, TRUE);
		    growl(mtmp);
		} else {
		    yelp(mtmp);
		}
	    }
	    mtmp->mundetected = 0;
	    if (mtmp->m_ap_type) seemimic(mtmp);
	    else if (!mtmp->mtame) newsym(mtmp->mx, mtmp->my);

	    if (mtmp->mtrapped &&
		    (trap = t_at(mtmp->mx, mtmp->my)) != 0 &&
		    (trap->ttyp == PIT || trap->ttyp == SPIKED_PIT || trap->ttyp == GIANT_CHASM || trap->ttyp == SHIT_PIT || trap->ttyp == MANA_PIT || trap->ttyp == ANOXIC_PIT || trap->ttyp == ACID_PIT) &&
		    sobj_at(BOULDER, trap->tx, trap->ty)) {
		/* can't swap places with pet pinned in a pit by a boulder */
		u.ux = u.ux0,  u.uy = u.uy0;	/* didn't move after all */
	    } else if (u.ux0 != x && u.uy0 != y &&
		       bad_rock(mtmp, x, u.uy0) &&
		       bad_rock(mtmp, u.ux0, y) &&
		       (bigmonst(mtmp->data) || (curr_mon_load(mtmp) > 5000))) {
		/* can't swap places when pet won't fit thru the opening */
		u.ux = u.ux0,  u.uy = u.uy0;	/* didn't move after all */
		You("stop.  %s won't fit through.", upstart(y_monnam(mtmp)));
	    } else {
		char pnambuf[BUFSZ];

		/* save its current description in case of polymorph */
		strcpy(pnambuf, y_monnam(mtmp));
		mtmp->mtrapped = 0;
		remove_monster(x, y);
		place_monster(mtmp, u.ux0, u.uy0);

		/* check for displacing it into pools and traps */
		switch (minliquid(mtmp) ? 2 : mintrap(mtmp)) {
		case 0:
		    You("%s %s.", mtmp->mtame ? "displaced" : "frightened",
			pnambuf);
		    break;
		case 1:		/* trapped */
		case 3:		/* changed levels */
		    /* there's already been a trap message, reinforce it */
		    abuse_dog(mtmp);
		    adjalign(-10);
		    break;
		case 2:
		    /* it may have drowned or died.  that's no way to
		     * treat a pet!  your god gets angry.
		     */
		    if (rn2(4)) {
			You_feel("guilty about losing your pet like this.");
			u.ugangr++;
			adjalign(-50);
		    }

		    /* you killed your pet by direct action.
		     * minliquid and mintrap don't know to do this
		     */
		    u.uconduct.killer++;
		    break;
		default:
		    pline("that's strange, unknown mintrap result!");
		    break;
		}
	    }
	}

	reset_occupations();
	if (flags.run) {
	    if ( flags.run < 8 )
		if (IS_DOOR(tmpr->typ) || IS_ROCK(tmpr->typ) ||
			IS_FURNITURE(tmpr->typ))
		    forcenomul(0, 0);
	}

	if (hides_under(youmonst.data) || (uarmh && itemhasappearance(uarmh, APP_SECRET_HELMET) ) || (uarmc && uarmc->oartifact == ART_JANA_S_EXTREME_HIDE_AND_SE) )
	    u.uundetected = OBJ_AT(u.ux, u.uy);
	else if (youmonst.data->mlet == S_EEL)
	    u.uundetected = is_waterypool(u.ux, u.uy) && !Is_waterlevel(&u.uz);
	else if (is_wagon(u.ux, u.uy))
	    u.uundetected = TRUE;
	else if (u.dx || u.dy)
	    u.uundetected = 0;

	/*
	 * Mimics (or whatever) become noticeable if they move and are
	 * imitating something that doesn't move.  We could extend this
	 * to non-moving monsters...
	 */
	if ((u.dx || u.dy) && (youmonst.m_ap_type == M_AP_OBJECT
				|| youmonst.m_ap_type == M_AP_FURNITURE))
	    youmonst.m_ap_type = M_AP_NOTHING;

	check_leash(u.ux0,u.uy0);

	if(u.ux0 != u.ux || u.uy0 != u.uy) {
	    u.umoved = TRUE;

	    /* Clean old position -- vision_recalc() will print our new one. */
	    newsym(u.ux0,u.uy0);
	    /* Since the hero has moved, adjust what can be seen/unseen. */
	    vision_recalc(1);	/* Do the work now in the recover time. */
	    invocation_message();
	}

	if (Punished)				/* put back ball and chain */
	    move_bc(0,bc_control,ballx,bally,chainx,chainy);

	spoteffects(TRUE);

	/* delay next move because of ball dragging */
	/* must come after we finished picking up, in spoteffects() */
	if (cause_delay) {
	    nomul(-2, "dragging an iron ball", TRUE);
	    nomovemsg = "";
	}

	if (flags.run && iflags.runmode != RUN_TPORT) {
	    /* display every step or every 7th step depending upon mode */
	    if (iflags.runmode != RUN_LEAP || !(moves % 7L)) {
		if (flags.time) flags.botl = 1;
		curs_on_u();
		delay_output();
		if (iflags.runmode == RUN_CRAWL) {
		    delay_output();
		    delay_output();
		    delay_output();
		    delay_output();
		}
	    }
	}
}

void
invocation_message()
{
	/* a special clue-msg when on the Invocation position */
	if(invocation_pos(u.ux, u.uy) && !On_stairs(u.ux, u.uy)) {
	    char buf[BUFSZ];
	    struct obj *otmp = carrying(CANDELABRUM_OF_INVOCATION);

	    nomul(0, 0, FALSE);		/* stop running or travelling */
	    if (FunnyHallu) {
		pline("You're picking up good vibrations!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    } else {
	    if (u.usteed) sprintf(buf, "beneath %s", y_monnam(u.usteed));
	    else
	    if (Levitation || Flying) strcpy(buf, "beneath you");
	    else sprintf(buf, "under your %s", makeplural(body_part(FOOT)));

	    You_feel("a strange vibration %s.", buf);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    }
	    if (otmp && otmp->spe == 7 && otmp->lamplit)
		pline("%s %s!", The(xname(otmp)),
		    Blind ? "throbs palpably" : "glows with a strange light");

	    litroomlite(TRUE);

		if (!u.vibratingsquarefind) {
			u.vibratingsquarefind = 1;
			qt_pager(QT_VIBRATINGSQUARE);
		}

	}
}

/* Wounds patch gotten from 5lo's SLEX mod --Amy */

static const char * const hallu_adverb[] = { /* 5lo: Wounds patch by L */ 
    "mildly", "mostly", "somewhat", "slightly", "probably", "massively", "extremely", 
    "flagrantly", "flamboyantly", "supremely", "excessively", "truly", "terribly", 
    "not at all", "barely", "heavily", "critically", "normally", "nonchalantly", "strangely",
    "fleecily", "painfully", "hurtingly", "almost", "nearly", "now", "no longer",
    "apparently", "frighteningly", "totally", "awfully", "with difficulty", "secretly", 
    "immediately", "fatally", "magically", "dreadfully", "superficially", "openly",
    "sharply", "in some way", "maybe", "seemingly", "really", "not really", "unremarkably", 
    "happily", "randomly", "stupidly", "intelligently", "strongly", "weakly", "wisely",
    "always", "never", "more", "less", "certainly", "irredeemably", "questionably",
    "perhaps", "confused and", "stunned and", "dead or", "hallucinating but not", "more or less",
    "incredibly", "unbelievably", "obscenely", "insanely", "amazingly", "absolutely" 
}; 
 
void 
wounds_message(mon) 
struct monst *mon; 
{ 
    if (mon_wounds(mon)) 
	pline("%s is %s.", Monnam(mon), mon_wounds(mon)); 
} 
 
char * 
mon_wounds(mon) 
struct monst *mon; 
{ 
	static char buf[BUFSZ]; 
	boolean wounded = ((!nonliving(mon->data) ||  
			/* Zombies and mummies (but not skeletons) have flesh */ 
			((mon->data->mlet == S_ZOMBIE && mon->data != &mons[PM_SKELETON]) 
			  || mon->data->mlet == S_MUMMY || mon->data->mlet == S_VAMPIRE 
			  || mon->data == &mons[PM_FLESH_GOLEM])) 
			&& !vegetarian(mon->data)); /* :TODO?: Check and see if some Extended monsters fit the above */ 
 
	/* Able to detect wounds? */ 
	if (!(canseemon(mon) || (u.ustuck == mon && u.uswallow && !Blind)) 
		 || (!Race_if(PM_HERBALIST) && !Role_if(PM_HEALER) && !Role_if(PM_SCIENTIST) && !Role_if(PM_EMPATH) && !Race_if(PM_ALCHEMIST) && !Role_if(PM_NECROMANCER) && !Role_if(PM_UNDERTAKER) ) ) 
		/* 5lo: Expanded for more roles */ 
	    return (char *)0; 
	if (mon->mhp == mon->mhpmax || mon->mhp < 1) 
	    return (char *)0; 
	if (!Hallucination && mon->mhp <= mon->mhpmax / 6) { 
	    sprintf(buf,"almost "); 
	    strcat(buf, nonliving(mon->data) ? "destroyed" : "dead"); 
	} else {
	    if (Hallucination) { 
		sprintf(buf, "%s", hallu_adverb[rn2(SIZE(hallu_adverb))]); 
		strcat(buf," "); 
	    } 
	    else if (mon->mhp <= mon->mhpmax / 4) 
	        sprintf(buf,"horribly "); 
	    else if (mon->mhp <= mon->mhpmax / 3) 
	        sprintf(buf,"heavily "); 
	    else if (mon->mhp <= 3 * mon->mhpmax / 4) 
	        sprintf(buf,"moderately "); 
	    else 
		sprintf(buf,"lightly "); 
	    strcat(buf, wounded || (Hallucination && rn2(2)) ? "wounded" : "damaged"); 
	} 
	return buf; 
} 


#endif /* OVL3 */
#ifdef OVL2

void
spoteffects(pick)
boolean pick;
{
	register struct monst *mtmp;

	if  (is_pool(u.ux, u.uy) && (Role_if(PM_TOPMODEL) || Role_if(PM_FAILED_EXISTENCE)) && Is_qlocate(&u.uz) ) {

	/* strong winds over the Grand Canyon. Please don't ask me how they can continue working underwater. :-) --Amy */

		pline(FunnyHallu ? "This whirl is like the eye of a hurricane, but it tickles!" : "There are scathing winds here! Your skin is scraped off!");
		losehp(rnz(u.legscratching), "scathing winds", KILLED_BY);

		You("tumble...");
		make_stunned(HStun + rnz(u.legscratching), FALSE);

		u.legscratching++;
	}

	if (t_at(u.ux, u.uy) && (Role_if(PM_TOPMODEL) || Role_if(PM_FAILED_EXISTENCE)) && Is_qlocate(&u.uz) ) {

	/* every trap on the Grand Canyon level also has a lesser wind effect. --Amy */

		pline(FunnyHallu ? "A twister... or is that a tornado?" : "You are enclosed in a whirlwind!");
		losehp(rnd(u.legscratching + 2), "whirlwinds", KILLED_BY);

	}

	if (is_crystalwater(u.ux,u.uy) && (Flying || Levitation)) {
		crystaldrown();
	}

	if(u.uinwater) {
		int was_underwater;

		if (!is_drowningpool(u.ux,u.uy)) {
			if (Is_waterlevel(&u.uz))
				You("pop into an air bubble.");
			else if (is_lava(u.ux, u.uy))
				You("leave the water...");	/* oops! */
			else
				You("are on solid %s again.",
				    is_ice(u.ux, u.uy) ? "ice" : "land");
		}
		else if (Is_waterlevel(&u.uz))
			goto stillinwater;
		else if (Levitation && !is_crystalwater(u.ux,u.uy))
			You("pop out of the water like a cork!");
		/* KMH, balance patch -- new intrinsic */
		else if (Flying && !is_crystalwater(u.ux,u.uy))
			You("fly out of the water.");
		else if (uarmc && itemhasappearance(uarmc, APP_FLIER_CLOAK) && !is_crystalwater(u.ux,u.uy))
			You("fly out of the water.");
		else if (uarmf && uarmf->oartifact == ART_RUBBER_LOVE && !is_crystalwater(u.ux,u.uy))
			You("float on top of the water.");
		else if ((Wwalking || Race_if(PM_KORONST)) && !is_crystalwater(u.ux,u.uy))
			You("slowly rise above the surface.");
/*              else if (Swimming)
			You("paddle up to the surface.");*/
		else
			goto stillinwater;
		was_underwater = Underwater && !Is_waterlevel(&u.uz);
		u.uinwater = 0;		/* leave the water */
		if (was_underwater) {	/* restore vision */
			docrt();
			vision_full_recalc = 1;
		}
	}
stillinwater:;
	if (!Levitation && !u.ustuck && !Flying && !(uarmf && uarmf->oartifact == ART_RUBBER_LOVE) && !(uarmc && itemhasappearance(uarmc, APP_FLIER_CLOAK)) ) {
	    /* limit recursive calls through teleds() */
	    if ((is_drowningpool(u.ux, u.uy) && !(is_crystalwater(u.ux,u.uy))) || is_lava(u.ux, u.uy)) {
		if (u.usteed && !is_flyer(u.usteed->data) && (!u.usteed->egotype_flying) &&
			!is_floater(u.usteed->data) &&
			!is_clinger(u.usteed->data)) {
		    dismount_steed(Underwater ?
			    DISMOUNT_FELL : DISMOUNT_GENERIC);
		    /* dismount_steed() -> float_down() -> pickup() */
		    if (!Is_airlevel(&u.uz) && !Is_waterlevel(&u.uz))
			pick = FALSE;
		} else
		if (is_lava(u.ux, u.uy)) {
		    if (lava_effects()) return;
		} else if (!Wwalking && !Race_if(PM_KORONST) && drown())
		    return;
	    }
	}
	check_special_room(FALSE);
	if(IS_SINK(levl[u.ux][u.uy].typ) && Levitation)
		dosinkfall();
	if (!in_steed_dismounting) { /* if dismounting, we'll check again later */
		struct trap *trap = t_at(u.ux, u.uy);
		boolean pit;
		pit = (trap && (trap->ttyp == PIT || trap->ttyp == SPIKED_PIT || trap->ttyp == GIANT_CHASM || trap->ttyp == SHIT_PIT || trap->ttyp == MANA_PIT || trap->ttyp == ANOXIC_PIT || trap->ttyp == ACID_PIT));
		if (trap && pit)
			dotrap(trap, 0);	/* fall into pit */
		/* somehow, being engulfed can sometimes result in "you can't take out blablabla" messages when you very
		 * obviously just wanted to attack the engulfer, but I can't seem to be able to reproduce it... yet it's
		 * incredibly annoying whenever it happens; I hope this is the correct line of code to change --Amy */
		if (pick && !u.uswallow) (void) pickup(1);
		if (trap && !pit)
			dotrap(trap, 0);	/* fall into arrow trap, etc. */
	}
	if((mtmp = m_at(u.ux, u.uy)) && !u.uswallow) {
		mtmp->mundetected = mtmp->msleeping = 0;
		switch(mtmp->data->mlet) {
		    case S_PIERCER:
			pline("%s suddenly drops from the %s!",
			      Amonnam(mtmp), ceiling(u.ux,u.uy));
			if(mtmp->mtame) /* jumps to greet you, not attack */
			    ;
			else if(uarmh && is_metallic(uarmh) && !is_etheritem(uarmh))
			    pline("Its blow glances off your helmet.");
			else if (u.uac + 3 <= rnd(20) && rn2(3) ) /* ac no longer gives complete protection --Amy */
			    You("are almost hit by %s!",
				x_monnam(mtmp, ARTICLE_A, "falling", 0, TRUE));
			else {
			    int dmg;
			    int molev;
			    You("are hit by %s!",
				x_monnam(mtmp, ARTICLE_A, "falling", 0, TRUE));
			    /* Amy edit: make it depend on the monster's level */
			    molev = mtmp->data->mlevel;
			    if (molev > 5) molev -= ((molev - 4) * 2 / 3);
			    if (molev < 1) molev = 1;
			    dmg = d(molev,6);
			    if(Half_physical_damage && rn2(2) ) dmg = (dmg+1) / 2;
			    if(StrongHalf_physical_damage && rn2(2) ) dmg = (dmg+1) / 2;
			    mdamageu(mtmp, dmg);
			}
			break;
		    default:	/* monster surprises you. */
			if(mtmp->mtame)
			    pline("%s jumps near you from the %s.",
					Amonnam(mtmp), ceiling(u.ux,u.uy));
			else if(mtmp->mpeaceful) {
				You("surprise %s!",
				    Blind && !sensemon(mtmp) ?
				    something : a_monnam(mtmp));
				mtmp->mpeaceful = 0;
			} else if (mtmp->data == &mons[PM_DROPCLONK_BEAR] || mtmp->data == &mons[PM_DROPTREE_BEAR]) {
				int dmg;
				You("are hit by %s!",
				x_monnam(mtmp, ARTICLE_A, "falling", 0, TRUE));
				dmg = d(10, 10);
				if(Half_physical_damage && rn2(2) ) dmg = (dmg+1) / 2;
				if(StrongHalf_physical_damage && rn2(2) ) dmg = (dmg+1) / 2;
				if (uarmh) dmg = (dmg+1) / 2;
				mdamageu(mtmp, dmg);

			} else {
				pline("%s attacks you by surprise!", Amonnam(mtmp));
			}
			break;
		}
		mnexto(mtmp); /* have to move the monster */
	}
}

STATIC_OVL boolean
monstinroom(mdat,roomno)
struct permonst *mdat;
int roomno;
{
	register struct monst *mtmp;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
		if(!DEADMONSTER(mtmp) && mtmp->data == mdat &&
		   index(in_rooms(mtmp->mx, mtmp->my, 0), roomno + ROOMOFFSET))
			return(TRUE);
	return(FALSE);
}

STATIC_OVL boolean
anymonstinroom(roomno)
int roomno;
{
	register struct monst *mtmp;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
		if(!DEADMONSTER(mtmp) && index(in_rooms(mtmp->mx, mtmp->my, 0), roomno + ROOMOFFSET))
			return(TRUE);
	return(FALSE);
}

char *
in_rooms(x, y, typewanted)
register xchar x, y;
register int typewanted;
{
	static char buf[5];
	char rno, *ptr = &buf[4];
	int typefound, min_x, min_y, max_x, max_y_offset, step;
	register struct rm *lev;

#define goodtype(rno) (!typewanted || \
	     ((typefound = rooms[rno - ROOMOFFSET].rtype) == typewanted) || \
	     ((typewanted == SHOPBASE) && (typefound > SHOPBASE))) \

	switch (rno = levl[x][y].roomno) {
		case NO_ROOM:
			return(ptr);
		case SHARED:
			step = 2;
			break;
		case SHARED_PLUS:
			step = 1;
			break;
		default:			/* i.e. a regular room # */
			if (goodtype(rno))
				*(--ptr) = rno;
			return(ptr);
	}

	min_x = x - 1;
	max_x = x + 1;
	if (x < 1)
		min_x += step;
	else
	if (x >= COLNO)
		max_x -= step;

	min_y = y - 1;
	max_y_offset = 2;
	if (min_y < 0) {
		min_y += step;
		max_y_offset -= step;
	} else
	if ((min_y + max_y_offset) >= ROWNO)
		max_y_offset -= step;

	for (x = min_x; x <= max_x; x += step) {
		lev = &levl[x][min_y];
		y = 0;
		if (((rno = lev[y].roomno) >= ROOMOFFSET) &&
		    !index(ptr, rno) && goodtype(rno))
			*(--ptr) = rno;
		y += step;
		if (y > max_y_offset)
			continue;
		if (((rno = lev[y].roomno) >= ROOMOFFSET) &&
		    !index(ptr, rno) && goodtype(rno))
			*(--ptr) = rno;
		y += step;
		if (y > max_y_offset)
			continue;
		if (((rno = lev[y].roomno) >= ROOMOFFSET) &&
		    !index(ptr, rno) && goodtype(rno))
			*(--ptr) = rno;
	}
	return(ptr);
}

char *
in_roomscolouur(x, y, typewanted)
register xchar x, y;
register int typewanted;
{
	static char buf[5];
	char rno, *ptr = &buf[4];
	int typefound, min_x, min_y, max_x, max_y_offset, step;
	register struct rm *lev;

#define goodtypeX(rno) (!typewanted || \
	     ((typefound = rooms[rno - ROOMOFFSET].colouur) == typewanted)) \

	switch (rno = levl[x][y].roomno) {
		case NO_ROOM:
			return(ptr);
		case SHARED:
			step = 2;
			break;
		case SHARED_PLUS:
			step = 1;
			break;
		default:			/* i.e. a regular room # */
			if (goodtypeX(rno))
				*(--ptr) = rno;
			return(ptr);
	}

	min_x = x - 1;
	max_x = x + 1;
	if (x < 1)
		min_x += step;
	else
	if (x >= COLNO)
		max_x -= step;

	min_y = y - 1;
	max_y_offset = 2;
	if (min_y < 0) {
		min_y += step;
		max_y_offset -= step;
	} else
	if ((min_y + max_y_offset) >= ROWNO)
		max_y_offset -= step;

	for (x = min_x; x <= max_x; x += step) {
		lev = &levl[x][min_y];
		y = 0;
		if (((rno = lev[y].roomno) >= ROOMOFFSET) &&
		    !index(ptr, rno) && goodtypeX(rno))
			*(--ptr) = rno;
		y += step;
		if (y > max_y_offset)
			continue;
		if (((rno = lev[y].roomno) >= ROOMOFFSET) &&
		    !index(ptr, rno) && goodtypeX(rno))
			*(--ptr) = rno;
		y += step;
		if (y > max_y_offset)
			continue;
		if (((rno = lev[y].roomno) >= ROOMOFFSET) &&
		    !index(ptr, rno) && goodtypeX(rno))
			*(--ptr) = rno;
	}
	return(ptr);

}

/* is (x,y) in a town? */
boolean
in_town(x, y)
register int x, y;
{
	s_level *slev = Is_special(&u.uz);
	register struct mkroom *sroom;
	boolean has_subrooms = FALSE;

	if (!slev || !slev->flags.town) return FALSE;

	/*
	 * See if (x,y) is in a room with subrooms, if so, assume it's the
	 * town.  If there are no subrooms, the whole level is in town.
	 */
	for (sroom = &rooms[0]; sroom->hx > 0; sroom++) {
	    if (sroom->nsubrooms > 0) {
		has_subrooms = TRUE;
		if (inside_room(sroom, x, y)) return TRUE;
	    }
	}

	return !has_subrooms;
}

STATIC_OVL void
move_update(newlev)
register boolean newlev;
{
	char *ptr1, *ptr2, *ptr3, *ptr4;

	strcpy(u.urooms0, u.urooms);
	strcpy(u.ushops0, u.ushops);
	if (newlev) {
		u.urooms[0] = '\0';
		u.uentered[0] = '\0';
		u.ushops[0] = '\0';
		u.ushops_entered[0] = '\0';
		strcpy(u.ushops_left, u.ushops0);
		return;
	}
	strcpy(u.urooms, in_rooms(u.ux, u.uy, 0));

	for (ptr1 = &u.urooms[0],
	     ptr2 = &u.uentered[0],
	     ptr3 = &u.ushops[0],
	     ptr4 = &u.ushops_entered[0];
	     *ptr1; ptr1++) {
		if (!index(u.urooms0, *ptr1))
			*(ptr2++) = *ptr1;
		if (IS_SHOP(*ptr1 - ROOMOFFSET)) {
			*(ptr3++) = *ptr1;
			if (!index(u.ushops0, *ptr1))
				*(ptr4++) = *ptr1;
		}
	}
	*ptr2 = '\0';
	*ptr3 = '\0';
	*ptr4 = '\0';

	/* filter u.ushops0 -> u.ushops_left */
	for (ptr1 = &u.ushops0[0], ptr2 = &u.ushops_left[0]; *ptr1; ptr1++)
		if (!index(u.ushops, *ptr1))
			*(ptr2++) = *ptr1;
	*ptr2 = '\0';
}

void
check_special_room(newlev)
register boolean newlev;
{
	register struct monst *mtmp;
	char *ptr;

	move_update(newlev);

	if (*u.ushops0)
	    u_left_shop(u.ushops_left, newlev);

	if (!*u.uentered && !*u.ushops_entered)		/* implied by newlev */
	    return;		/* no entrance messages necessary */

	/* Did we just enter a shop? */
	if (*u.ushops_entered)
	    u_entered_shop(u.ushops_entered);

	for (ptr = &u.uentered[0]; *ptr; ptr++) {
	    register int roomno = *ptr - ROOMOFFSET, rt = rooms[roomno].rtype;

	    /* Did we just enter some other special room? */
	    /* vault.c insists that a vault remain a VAULT,
	     * and temples should remain TEMPLEs,
	     * but everything else gives a message only the first time */
		/* edit by Amy - removed that stupidity. Why the heck would a swamp turn into an ordinary room??? 
		 * Besides, I _like_ ambient level messages! They can be anything from soothing to terrifying!
		 * There's no reason to get rid of them if you enter a room, and it's OK to get a message every time, too. */
	    switch (rt) {

		/* "Special Room monster change. Entering a special room will no longer awaken all monsters inside of the room, making the behavior the same as Vanilla Nethack and Slash'EM. Removed wake_nearby, gets really annoying" In Soviet Russia, players want to be able to mindlessly slaughter all the denizens of special rooms, because they're somehow unable to handle a game posing an actual challenge. They don't view it as too easy if they can hack up one monster after the other without the remaining ones even reacting or doing anything. But of course we all know that this is not the way it's supposed to be, so for all the other races the monsters will wake up. --Amy
		 * edit: do the wake_nearby only if there are monsters in the room, because they're the reason why we're
		 * doing it at all: to make sure that you can't simply kill the sleeping monsters. If there's no monsters
		 * in the room anyway, we have no need to wake up monsters somewhere else on the level! */

		case ZOO:
		    pline(FunnyHallu ? "Welcome to our Theme Park!" : "Welcome to David's treasure zoo!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case SWAMP:
		    pline("It %s rather %s down here.",
			  Blind ? "feels" : "looks",
			  Blind ? "humid" : "muddy");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case COURT:
		    You(FunnyHallu ? "enter the Queen's chambers!" : "enter an opulent throne room!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case REALZOO:
		    You(FunnyHallu ? "feel that some extinct species might still live here!" : "enter a smelly zoo!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case GIANTCOURT:
		    You(FunnyHallu ? "enter a real huge hall!" : "enter a giant throne room!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case DRAGONLAIR:
		    You(FunnyHallu ? "enter a fairy lair!" : "enter a dragon lair...");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case BADFOODSHOP:
		    You(FunnyHallu ? "enter some sort of market! Perhaps you can buy some weed here?" : "enter an abandoned store...");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case LEPREHALL:
		    You(FunnyHallu ? "encounter a Stonehenge replica!" : "enter a leprechaun hall!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case MORGUE:
		    if(midnight()) {
			const char *run = locomotion(youmonst.data, "Run");
			pline("%s away!  %s away!", run, run);
		    } else
			{ You("have an uncanny feeling..."); }
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case BEEHIVE:
		    You(FunnyHallu ? "enter a tracker jacker nest! RUN AWAY!!!" : "enter a giant beehive!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case LEMUREPIT:
		    You(FunnyHallu ? "enter the Devil's Lair!" : "enter a pit of screaming lemures!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case MIGOHIVE:
		    You(FunnyHallu ? "enter some futuristic alien structure!" : "enter a strange hive!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case FUNGUSFARM:
		    You(FunnyHallu ? "enter a sticky, slimy room..." : "enter a room full of fungi!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case COCKNEST:
		    You(FunnyHallu ? "sense the well-known smell of weed as you enter this room!" : "enter a disgusting nest!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case ANTHOLE:
		    You(FunnyHallu ? "enter a room filled with bugs!" : "enter an anthole!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
            case CLINIC:
                You(FunnyHallu ? "feel reminded of 'Emergency Room' as you enter this area!" : "enter a modern hospital.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
            case ANGELHALL:
                You(FunnyHallu ? "see the gods as you enter! WOW! So that's what they look like..." : "enter a radiating hall of Angels!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case TERRORHALL:
                You(FunnyHallu ? "feel like you just got detected by a tripwire!" : "enter a terrifying hall.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case TENSHALL:
                You(FunnyHallu ? "have died. Do you want your possessions identified? [ynq] (n) _" : "enter a killer room! This is actually an ADOM tension room.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case ELEMHALL:
                You(FunnyHallu ? "get a vision of Mother Nature as you enter!" : "enter a room full of elementals!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case NYMPHHALL:
                You(FunnyHallu ? "see a picture on the wall of this room. It shows the most beautiful woman you ever saw..." : "enter a beautiful garden!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
            case ARMORY:
                You(FunnyHallu ? "enter some old weapon storage chamber! Let's see what weapons are left!" : "enter a dilapidated armory.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case COINHALL:
                You(FunnyHallu ? "enter a room full of treasure, and it's all going to be yours! YEAH!" : "enter a room filled with money!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case TROLLHALL:
                You(FunnyHallu ? "feel that this room smells like a public toilet!" : "enter a room full of stinking trolls...");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case HUMANHALL:
                You(FunnyHallu ? "encounter a party room! Let's invite some hot girls!" : "encounter a living room!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case SPIDERHALL:
                You(FunnyHallu ? "notice spiders of all forms and sizes in this room sitting everywhere!" : "stumble into a nest of spiders...");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case GOLEMHALL:
                You(FunnyHallu ? "encounter some warmeches!" : "enter a room full of golems!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case TRAPROOM:
                if (wizard) You("enter a trapped room!");
                break;
	      case EMPTYNEST:
                if (wizard) You("enter an empty nest!");
                break;
	      case POOLROOM:
                if (wizard) You("enter a pool room!");
                break;
	      case STATUEROOM:
                if (wizard) You("enter a statue room!");
                break;
	      case NASTYCENTRAL:
                if (wizard) You("enter a nasty central!");
                break;
	      case INSIDEROOM:
                You(FunnyHallu ? "enter a normal-looking room." : "enter a weird-looking room...");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case RIVERROOM:
                You(FunnyHallu ? "encounter an underground mountain! Wait, what? This makes no sense!" : "encounter an underground river!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case GRUEROOM:
                pline(FunnyHallu ? "It is radiant bright. You are likely to be eaten by the sun." : "It is pitch black. You are likely to be eaten by a grue.");
	    do_clear_areaX(u.ux,u.uy,		/* extra darkness --Amy */
		15, set_litX, (void *)((char *)0));
		/* IMHO grue rooms may remove light every time you enter them. --Amy */
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case CRYPTROOM:
                You(FunnyHallu ? "forgot to bring your light source and can't see anything in this room." : "enter the dark crypts!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case TROUBLEZONE:
                You(FunnyHallu ? "got tons of trouble, baby!" : "enter the trouble zone!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case WEAPONCHAMBER:
                You(FunnyHallu ? "see people with long pointy sticks who want to impale you!" : "enter a weapons chamber!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case HELLPIT:
                pline(FunnyHallu ? "Gee, this looks exactly like the realms of Oblivion!" : "You enter the fiery pits of Hell!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case FEMINISMROOM:
                You(FunnyHallu ? "enter a room full of girls in sexy bikinis and high-heeled leather boots! WOW!" : "enter a feminist meeting room!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case MEADOWROOM:
                You(FunnyHallu ? "stumble into a ranch! Where's the cowboys and horses?" : "encounter a cattle meadow!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case COOLINGCHAMBER:
                You(FunnyHallu ? "entered the radiator area!" : "freeze as you enter the cooling chamber.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case VOIDROOM:
                pline(FunnyHallu ? "Your body warps strangely and you cease to exist... Do you want your possessions identified? [ynq] (n) _" : "You entered the Void!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case HAMLETROOM:
                pline(FunnyHallu ? "This room looks not dangerous at all." : "You've stumbled over a tiny hamlet!");
			if (Role_if(PM_SPACEWARS_FIGHTER) || Role_if(PM_CAMPERSTRIKER)) {
	            	pline(FunnyHallu ? "You feel that you've seen this before... it reminds you of the Woodstock Festival!" : "It looks familiar... didn't your adventures usually start in a similar place?");

			}
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case KOPSTATION:
                pline(FunnyHallu ? "As you enter the heavily guarded army base, bullets start flying in your direction! TAKE COVER!" : "You've entered the local police station!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case BOSSROOM:
                You(FunnyHallu ? "were fated to die here. DIE!" : "feel that you will meet your fate here.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case RNGCENTER:
                You(FunnyHallu ? "enter a room that looks like it was designed by God Himself!" : "enter the Random Number Generator central!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case WIZARDSDORM:
                You(FunnyHallu ? "disturbed some old wizened fool that lives here, and now you have to kill that stupid git!" : "entered a wizard's dormitory!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case DOOMEDBARRACKS:
                You(FunnyHallu ? "realize that the game has turned into DoomRL! Quick, ready your kalashnikov and BFG!" : "enter an alien barracks!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case SLEEPINGROOM:
                pline(FunnyHallu ? "I heard you were sleeping there." : "You stumble into a sleeping room!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case DIVERPARADISE:
                You(FunnyHallu ? "enter a huge swimming pool, and the entrance is free! YEAH! Now you can splash around in the water and have many hours of pure FUN!" : "encounter the diver's paradise!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case MENAGERIE:
                You(FunnyHallu ? "enter a zoo filled with prehistoric animals! Err... I really hope they're peaceful!" : "enter a menagerie!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case EMPTYDESERT:
                pline(FunnyHallu ? "This looks like the Shifting Sand Land from Super Mario 64!" : "The air in this room is hot and arid.");
                break;
	      case RARITYROOM:
                pline(FunnyHallu ? "It's the dungeon master's rarity collection! Quick, steal it before he turns up!" : "You enter a room filled with rare creatures!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case EXHIBITROOM:
                pline(FunnyHallu ? "Oh, look at all the zoo exhibits! Can I feed them? Can I pet them? Mind if I incinerate them?" : "You encounter an exhibit of strange creatures!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case PRISONCHAMBER:
                pline(FunnyHallu ? "Go directly to jail. Do not pass go. Do not collect 200 zorkmids." : "You enter a prison!");
			if (FunnyHallu) pline("Do you want your possessions identified? [ynq] (n) _");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case NUCLEARCHAMBER:
                pline(FunnyHallu ? "It's where the government is researching weaponized uranium! If you can steal their technology, you can nuke the entire dungeon and ascend prematurely!" : "You encounter a nuclear power plant!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case PLAYERCENTRAL:
                pline(FunnyHallu ? "Are the people in here celebrating a corona party? Call the police! They're spreading the virus!" : "Apparently a rival gang of adventurers has set up camp here!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case LEVELSEVENTYROOM: /* no message but still wake_nearby --Amy */
                if (wizard) You("enter a level 70 room!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case VARIANTROOM:
                pline(FunnyHallu ? "The game suddenly turned into dnethack. The elder priest tentacles to tentacle you! Your cloak of magic resistance disintegrates!" : "You encounter a room from another variant!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case ROBBERCAVE:
                pline(FunnyHallu ? "Oh my god, Amy's fanfics have come true!" : "You enter the robbers' hideout!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;
	      case SANITATIONCENTRAL:
                pline(FunnyHallu ? "Muahahahahaha, you feel like focusing your gaze on a great race of yith because who needs sanity anyway?" : "Something seems to focus on your mind as you enter this room.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
                break;

		case BARRACKS:
		    if(anymonstinroom(roomno)) {
			You("enter a military barracks!");
		    } else {
			You("enter an abandoned barracks.");
		    }
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case DELPHI:
		    if(monstinroom(&mons[PM_ORACLE], roomno))
			verbalize("%s, %s, welcome to Delphi!", Hello((struct monst *) 0), playeraliasname);
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case DOUGROOM:
		    You_feel("42.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case EVILROOM:
			pline(FunnyHallu ? "Eek, you've stumbled into a SJW meeting!" : "The feel of this room is giving you the creeps.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case RELIGIONCENTER:
			pline(FunnyHallu ? "Ugh, an overwhelming cancerous stench floods your nostrils as you enter this room!" : "This room smells like rotten holy water.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case CURSEDMUMMYROOM:
			pline(FunnyHallu ? "You entered the Pharao's chambers!" : "Things that should be dead are walking this room.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case ARDUOUSMOUNTAIN:
			pline(FunnyHallu ? "You encounter an underground mountain. Wait, why the hell is there a mountain in the dungeon???" : "You encounter an underground mountain.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case LEVELFFROOM:
			pline(FunnyHallu ? "Hey, this room looks just like your own living room!" : "This room looks familiar, but somehow different too.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case VERMINROOM:
			pline(FunnyHallu ? "Oh great, you entered a room full of shit." : "The air in this room is rank with mildew.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case CHAOSROOM:
			if (wizard) pline("You enter a chaos room!");
		    break;
		case RAMPAGEROOM:
			if (wizard) pline("You enter a rampage room!");
		    break;
		case MIXEDPOOL:
			if (wizard) pline("You enter a mixed pool!");
		    break;
		case MIRASPA:
			pline(FunnyHallu ? "Whoa, this room totally smells of ammonia!" : "As you enter the room, you can hear Mira inviting you for a swim.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case MACHINEROOM:
			pline(FunnyHallu ? "It's the inside of the Space Shuttle!" : "You enter a machinery room.");
			if (uarmf && itemhasappearance(uarmf, APP_MACHINERY_BOOTS) && !u.uspellprot) {
				u.uspellprot = 4;
				u.uspmtime = 10;
				find_ac();
				flags.botl = TRUE;
				You_feel("at home here, thanks to your machinery boots!");
			}
			if (uarmf && uarmf->oartifact == ART_PERMINANT_INCREASE && u.ublessed < 4) {
				if (!(HProtection & INTRINSIC)) HProtection |= FROMOUTSIDE;
				u.ublessed = 4;
				find_ac();
				flags.botl = TRUE;
				pline("A holy aura of protection surrounds you!");
			}
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case SHOWERROOM:
			pline(FunnyHallu ? "Wow! You seem to have found the Niagara Falls!" : "You enter the shower.");
		    break;
		case GREENCROSSROOM:
			pline(FunnyHallu ? "Entering this room feels like being put in headlock by your wonderful fleecy roommate!" : "This room has a very peaceful atmosphere.");
		    break;
		case RUINEDCHURCH:
			pline(FunnyHallu ? "You enter Satan's chamber! Quick, ask him what is inferior to SLEX!" : "You enter a desecrated church.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case GAMECORNER:
			pline(FunnyHallu ? "You've entered the local Game Stop store! The storeclerk says: 'Hello sir or miss, what can I do for you? I have GTA 5, the newest Call of Duty and of course also the latest Pokemon generation games available!'" : "You encounter a game corner!");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case ILLUSIONROOM:
			pline(FunnyHallu ? "This room is an illusion and a trap devisut by Satan. Go ahead dauntlessly! Make rapid progres!" : "Somehow, this room doesn't seem to be what it looks like.");
		    if (!issoviet && anymonstinroom(roomno)) wake_nearby();
		    break;
		case CENTRALTEDIUM:
			pline(FunnyHallu ? "It's the Straight Road! In order to get through this room, you need to stay on the road at all times and be fast or the Straight Road will weaken and ultimately be destroyed!" : "You encounter a highway to the left.");
		    break;
		case TEMPLE:
		    intemple(roomno + ROOMOFFSET);
		    /* fall through */
		default:
		    rt = 0;
	    }

	    if (rt != 0) {
		/*rooms[roomno].rtype = OROOM;*/
		/*if (!search_special(rt)) {*/
			/* No more room of that type */
			/*switch(rt) {
			    case COURT:
			    case GIANTCOURT:
				level.flags.has_court = 0;
				break;
			    case SWAMP:
				level.flags.has_swamp = 0;
				break;
			    case MORGUE:
				level.flags.has_morgue = 0;
				break;
			    case ZOO:
			    case REALZOO:
				level.flags.has_zoo = 0;
				break;
			    case BARRACKS:
				level.flags.has_barracks = 0;
				break;
                      case CLINIC:
                        level.flags.has_clinic = 0;
                        break;
                      case MIMICHALL:
                        level.flags.has_mimichall = 0;
                        break;
                      case ANGELHALL:
                        level.flags.has_angelhall = 0;
                        break;
                      case TERRORHALL:
                        level.flags.has_terrorhall = 0;
                        break;
                      case TENSHALL:
                        level.flags.has_tenshall = 0;
                        break;
                      case ELEMHALL:
                        level.flags.has_elemhall = 0;
                        break;
                      case NYMPHHALL:
                        level.flags.has_nymphhall = 0;
                        break;
                      case GOLEMHALL:
                        level.flags.has_golemhall = 0;
                        break;
                      case TRAPROOM:
                        level.flags.has_traproom = 0;
                        break;
                      case GRUEROOM:
                        level.flags.has_grueroom = 0;
                        break;
                      case CRYPTROOM:
                        level.flags.has_cryptroom = 0;
                        break;
                      case TROUBLEZONE:
                        level.flags.has_troublezone = 0;
                        break;
                      case WEAPONCHAMBER:
                        level.flags.has_weaponchamber = 0;
                        break;
                      case HELLPIT:
                        level.flags.has_hellpit = 0;
                        break;
                      case ROBBERCAVE:
                        level.flags.has_robbercave = 0;
                        break;
                      case SANITATIONCENTRAL:
                        level.flags.has_sanitationcentral = 0;
                        break;
                      case FEMINISMROOM:
                        level.flags.has_feminismroom = 0;
                        break;
                      case MEADOWROOM:
                        level.flags.has_meadowroom = 0;
                        break;
                      case COOLINGCHAMBER:
                        level.flags.has_coolingchamber = 0;
                        break;
                      case VOIDROOM:
                        level.flags.has_voidroom = 0;
                        break;
                      case ARMORY:
                        level.flags.has_armory = 0;
                        break;
                      case HAMLETROOM:
                        level.flags.has_hamletroom = 0;
                        break;
                      case KOPSTATION:
                        level.flags.has_kopstation = 0;
                        break;
                      case BOSSROOM:
                        level.flags.has_bossroom = 0;
                        break;
                      case RNGCENTER:
                        level.flags.has_rngcenter = 0;
                        break;
                      case WIZARDSDORM:
                        level.flags.has_wizardsdorm = 0;
                        break;
                      case DOOMEDBARRACKS:
                        level.flags.has_doomedbarracks = 0;
                        break;
                      case SLEEPINGROOM:
                        level.flags.has_sleepingroom = 0;
                        break;
                      case DIVERPARADISE:
                        level.flags.has_diverparadise = 0;
                        break;
                      case MENAGERIE:
                        level.flags.has_menagerie = 0;
                        break;
                      case NASTYCENTRAL:
                        level.flags.has_nastycentral = 0;
                        break;
                      case EMPTYDESERT:
                        level.flags.has_emptydesert = 0;
                        break;
                      case RARITYROOM:
                        level.flags.has_rarityroom = 0;
                        break;
                      case EXHIBITROOM:
                        level.flags.has_exhibitroom = 0;
                        break;
                      case PRISONCHAMBER:
                        level.flags.has_prisonchamber = 0;
                        break;
                      case NUCLEARCHAMBER:
                        level.flags.has_nuclearchamber = 0;
                        break;
                      case LEVELSEVENTYROOM:
                        level.flags.has_levelseventyroom = 0;
                        break;
                      case PLAYERCENTRAL:
                        level.flags.has_playercentral = 0;
                        break;
                      case VARIANTROOM:
                        level.flags.has_variantroom = 0;
                        break;
                      case POOLROOM:
                        level.flags.has_poolroom = 0;
                        break;
                      case STATUEROOM:
                        level.flags.has_statueroom = 0;
                        break;
                      case INSIDEROOM:
                        level.flags.has_insideroom = 0;
                        break;
                      case RIVERROOM:
                        level.flags.has_riverroom = 0;
                        break;
                      case HUMANHALL:
                        level.flags.has_humanhall = 0;
                        break;
                      case COINHALL:
                        level.flags.has_coinhall = 0;
                        break;
                      case TROLLHALL:
                        level.flags.has_trollhall = 0;
                        break;
                      case SPIDERHALL:
                        level.flags.has_spiderhall = 0;
                        break;
			    case TEMPLE:
				level.flags.has_temple = 0;
				break;
			    case BEEHIVE:
				level.flags.has_beehive = 0;
				break;
			    case LEMUREPIT:
				level.flags.has_lemurepit = 0;
				break;
			    case MIGOHIVE:
				level.flags.has_migohive = 0;
				break;
			    case FUNGUSFARM:
				level.flags.has_fungusfarm = 0;
				break;
			    case EVILROOM:
				level.flags.has_evilroom = 0;
				break;
			    case RELIGIONCENTER:
				level.flags.has_religioncenter = 0;
				break;
			    case CURSEDMUMMYROOM:
				level.flags.has_cursedmummyroom = 0;
				break;
			    case ARDUOUSMOUNTAIN:
				level.flags.has_arduousmountain = 0;
				break;
			    case LEVELFFROOM:
				level.flags.has_levelffroom = 0;
				break;
			    case VERMINROOM:
				level.flags.has_verminroom = 0;
				break;
			    case MIRASPA:
				level.flags.has_miraspa = 0;
				break;
			    case MACHINEROOM:
				level.flags.has_machineroom = 0;
				break;
			    case SHOWERROOM:
				level.flags.has_showerroom = 0;
				break;
			    case GREENCROSSROOM:
				level.flags.has_greencrossroom = 0;
				break;
			    case RUINEDCHURCH:
				level.flags.has_ruinedchurch = 0;
				break;
			    case GAMECORNER:
				level.flags.has_gamecorner = 0;
				break;
			    case ILLUSIONROOM:
				level.flags.has_illusionroom = 0;
				break;
			}
		}*/
		if(rt==COURT || rt==SWAMP || rt==MORGUE || rt==ZOO || (rt && !rn2(10)) )
		    for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
			if (!DEADMONSTER(mtmp) && (!Stealth || Aggravate_monster) && !rn2(3)) mtmp->msleeping = 0;
	    }
	}

	return;
}

#endif /* OVL2 */
#ifdef OVLB

int
dopickup()
{
	int count;
	struct trap *traphere = t_at(u.ux, u.uy);
 	/* awful kludge to work around parse()'s pre-decrement */
	count = (multi || (save_cm && *save_cm == ',')) ? multi + 1 : 0;
	multi = 0;	/* always reset */
	/* uswallow case added by GAN 01/29/87 */
	if(u.uswallow) {
	    if (!u.ustuck->minvent) {
		if (is_animal(u.ustuck->data)) {
		    You("pick up %s tongue.",
				    s_suffix(mon_nam(u.ustuck)));
		    pline("But it's kind of slimy, so you drop it.");
		} else
		    You("don't %s anything in here to pick up.",
			  Blind ? "feel" : "see");
		return(0);
	    } else {
	    	int tmpcount = -count;
		return loot_mon(u.ustuck, &tmpcount, (boolean *)0);
	    }
	}
	if((is_waterypool(u.ux, u.uy) || is_watertunnel(u.ux, u.uy) || is_moorland(u.ux, u.uy) || is_urinelake(u.ux, u.uy)) && !(is_crystalwater(u.ux, u.uy)) ) {
	    if (Wwalking || Race_if(PM_KORONST) || is_floater(youmonst.data) || is_clinger(youmonst.data)
			|| (Flying && !(uarmf && uarmf->oartifact == ART_DIP_DIVE) && !StrongFlying && !Breathless && !Swimming)) {
		You("cannot dive into the water to pick things up.");
		return(0);
	    } else if (!Underwater && !(uarmf && uarmf->oartifact == ART_DIP_DIVE) && !Swimming && !StrongFlying) {
		You_cant("even see the bottom, let alone pick up %s.",
				something);
		return(0);
	    }
	}
	if (is_lava(u.ux, u.uy)) {
	    if (Wwalking || Race_if(PM_KORONST) || is_floater(youmonst.data) || is_clinger(youmonst.data)
			|| (Flying && !(uarmf && uarmf->oartifact == ART_DIP_DIVE) && !StrongFlying && !Breathless)) {
		You_cant("reach the bottom to pick things up.");
		return(0);
	    } else if (!likes_lava(youmonst.data) && !Race_if(PM_HYPOTHERMIC) && !(uarmf && itemhasappearance(uarmf, APP_HOT_BOOTS) ) && !(uwep && uwep->oartifact == ART_EVERYTHING_MUST_BURN) && !(uamul && uamul->otyp == AMULET_OF_D_TYPE_EQUIPMENT) && !(uarmf && uarmf->oartifact == ART_DIP_DIVE) && !Race_if(PM_PLAYER_SALAMANDER) && !(uwep && uwep->oartifact == ART_MANUELA_S_PRACTICANT_TERRO) && !(powerfulimplants() && uimplant && uimplant->oartifact == ART_RUBBER_SHOALS) && !(uarm && uarm->oartifact == ART_LAURA_CROFT_S_BATTLEWEAR) && !(uarmc && uarmc->oartifact == ART_SCOOBA_COOBA) && !(uarm && uarm->oartifact == ART_D_TYPE_EQUIPMENT) && !(uarmf && uarmf->oartifact == ART_JOHANNA_S_RED_CHARM) ) {
		You("would burn to a crisp trying to pick things up.");
		return(0);
	    }
	}
	if(!OBJ_AT(u.ux, u.uy)) {
		There("is nothing here to pick up.");
		return(0);
	}
	if (!can_reach_floor()) {
		if (u.usteed && !(uwep && uwep->oartifact == ART_SORTIE_A_GAUCHE) && !(powerfulimplants() && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) && !(bmwride(ART_DEEPER_LAID_BMW)) && (PlayerCannotUseSkills || P_SKILL(P_RIDING) < P_BASIC) )
		    You("aren't skilled enough to reach from %s.",
			y_monnam(u.usteed));
		else
		You("cannot reach the %s.", surface(u.ux,u.uy));
		return(0);
	}

 	if (traphere && traphere->tseen) {
		/* Allow pickup from holes and trap doors that you escaped from
		 * because that stuff is teetering on the edge just like you, but
		 * not pits, because there is an elevation discrepancy with stuff
		 * in pits.
		 */
		/* [BarclayII] phasing or flying players can phase/fly into the pit */
		if ((traphere->ttyp == PIT || traphere->ttyp == SPIKED_PIT || traphere->ttyp == GIANT_CHASM || traphere->ttyp == SHIT_PIT || traphere->ttyp == MANA_PIT || traphere->ttyp == ANOXIC_PIT || traphere->ttyp == ACID_PIT) &&
		     (!u.utrap || (u.utrap && u.utraptype != TT_PIT)) && !Passes_walls && !Flying) {
			You("cannot reach the bottom of the pit.");
			return(0);
		}
	}

	return (pickup(-count));
}

#endif /* OVLB */
#ifdef OVL2

/* stop running if we see something interesting */
/* turn around a corner if that is the only way we can proceed */
/* do not turn left or right twice */
void
lookaround()
{
    register int x, y, i, x0 = 0, y0 = 0, m0 = 1, i0 = 9;
    register int corrct = 0, noturn = 0;
    register struct monst *mtmp;
    register struct trap *trap;

    /* Grid bugs stop if trying to move diagonal, even if blind.  Maybe */
    /* they polymorphed while in the middle of a long move. */
    if ((BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || (uarmg && uarmg->oartifact == ART_LINE_CAN_PLAY_BY_YOURSELF) || (uwep && uwep->oartifact == ART_KILLER_PIANO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_KILLER_PIANO) || isgridbug(youmonst.data) || (Race_if(PM_WEAPON_BUG) && !Upolyd))&& u.dx && u.dy) {
	forcenomul(0, 0);
	return;
    }

    if(Blind || flags.run == 0) return;
    for(x = u.ux-1; x <= u.ux+1; x++) for(y = u.uy-1; y <= u.uy+1; y++) {
	if(!isok(x,y)) continue;

	if((BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || (uarmg && uarmg->oartifact == ART_LINE_CAN_PLAY_BY_YOURSELF) || (uwep && uwep->oartifact == ART_KILLER_PIANO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_KILLER_PIANO) || isgridbug(youmonst.data) || (Race_if(PM_WEAPON_BUG) && !Upolyd)) && x != u.ux && y != u.uy) continue;

	if(x == u.ux && y == u.uy) continue;

	if((mtmp = m_at(x,y)) &&
		    mtmp->m_ap_type != M_AP_FURNITURE &&
		    mtmp->m_ap_type != M_AP_OBJECT &&
		    (!mtmp->minvis || (See_invisible && (StrongSee_invisible || mtmp->seeinvisble) ) ) && !mtmp->minvisreal && !mtmp->mundetected) {
	    if((flags.run != 1 && !mtmp->mtame)
					|| (x == u.ux+u.dx && y == u.uy+u.dy))
		goto stop;
	}

	if (levl[x][y].typ == STONE) continue;
	if (x == u.ux-u.dx && y == u.uy-u.dy) continue;

	if (IS_ROCK(levl[x][y].typ) || (levl[x][y].typ == ROOM) ||
	    IS_AIR(levl[x][y].typ))
	    continue;
	else if (closed_door(x,y) ||
		 (mtmp && mtmp->m_ap_type == M_AP_FURNITURE &&
		  (mtmp->mappearance == S_hcdoor ||
		   mtmp->mappearance == S_vcdoor))) {
	    if(x != u.ux && y != u.uy) continue;
	    if(flags.run != 1) goto stop;
	    goto bcorr;
	} else if (levl[x][y].typ == CORR) {
bcorr:
	    if(levl[u.ux][u.uy].typ != ROOM) {
		if(flags.run == 1 || flags.run == 3 || flags.run == 8) {
		    i = dist2(x,y,u.ux+u.dx,u.uy+u.dy);
		    if(i > 2) continue;
		    if(corrct == 1 && dist2(x,y,x0,y0) != 1)
			noturn = 1;
		    if(i < i0) {
			i0 = i;
			x0 = x;
			y0 = y;
			m0 = mtmp ? 1 : 0;
		    }
		}
		corrct++;
	    }
	    continue;
	} else if ((trap = t_at(x,y)) && trap->tseen) {
	    if(flags.run == 1) goto bcorr;	/* if you must */
	    if(x == u.ux+u.dx && y == u.uy+u.dy) goto stop;
	    continue;
	} else if (is_waterypool(x,y) || is_watertunnel(x,y) || is_urinelake(x,y) || is_shiftingsand(x,y) || is_moorland(x,y) || is_lava(x,y)) {
	    /* water and lava only stop you if directly in front, and stop
	     * you even if you are running
	     */
		/* KMH, balance patch -- new intrinsic */
	    if(!Levitation && !Flying && !is_clinger(youmonst.data) &&
				x == u.ux+u.dx && y == u.uy+u.dy)
			/* No Wwalking check; otherwise they'd be able
			 * to test boots by trying to SHIFT-direction
			 * into a pool and seeing if the game allowed it
			 */
			goto stop;
	    continue;
	} else {		/* e.g. objects or trap or stairs */
	    if(flags.run == 1) goto bcorr;
	    if(flags.run == 8) continue;
	    if(mtmp) continue;		/* d */
	    if(((x == u.ux - u.dx) && (y != u.uy + u.dy)) ||
	       ((y == u.uy - u.dy) && (x != u.ux + u.dx)))
	       continue;
	}
stop:
	nomul(0, 0, FALSE);
	return;
    } /* end for loops */

    if(corrct > 1 && flags.run == 2) goto stop;
    if((flags.run == 1 || flags.run == 3 || flags.run == 8) &&
	!noturn && !m0 && i0 && (corrct == 1 || (corrct == 2 && i0 == 1)))
    {
	/* make sure that we do not turn too far */
	if(i0 == 2) {
	    if(u.dx == y0-u.uy && u.dy == u.ux-x0)
		i = 2;		/* straight turn right */
	    else
		i = -2;		/* straight turn left */
	} else if(u.dx && u.dy) {
	    if((u.dx == u.dy && y0 == u.uy) || (u.dx != u.dy && y0 != u.uy))
		i = -1;		/* half turn left */
	    else
		i = 1;		/* half turn right */
	} else {
	    if((x0-u.ux == y0-u.uy && !u.dy) || (x0-u.ux != y0-u.uy && u.dy))
		i = 1;		/* half turn right */
	    else
		i = -1;		/* half turn left */
	}

	i += u.last_str_turn;
	if(i <= 2 && i >= -2) {
	    u.last_str_turn = i;
	    u.dx = x0-u.ux;
	    u.dy = y0-u.uy;
	}
    }
}

/* something like lookaround, but we are not running */
/* react only to monsters that might hit us */
int
monster_nearby()
{
	register int x,y;
	register struct monst *mtmp;

	/* Also see the similar check in dochugw() in monmove.c */
	for(x = u.ux-1; x <= u.ux+1; x++)
	    for(y = u.uy-1; y <= u.uy+1; y++) {
		if(!isok(x,y)) continue;
		if(x == u.ux && y == u.uy) continue;
		if((mtmp = m_at(x,y)) &&
		   mtmp->m_ap_type != M_AP_FURNITURE &&
		   mtmp->m_ap_type != M_AP_OBJECT &&
		   (!mtmp->mpeaceful || Hallucination) &&
		   ( (!is_hider(mtmp->data) && (!mtmp->egotype_hide) && (!mtmp->egotype_mimic) ) || !mtmp->mundetected) &&
		   !noattacks(mtmp->data) &&
		   mtmp->mcanmove && !mtmp->msleeping &&  /* aplvax!jcn */
		   !onscary(u.ux, u.uy, mtmp) &&
		   canspotmon(mtmp))
			return(1);
	}
	return(0);
}

void
maybe_wail()
{
    static short powers[] = { TELEPORT, SEE_INVIS, POISON_RES, COLD_RES,
			      SHOCK_RES, FIRE_RES, SLEEP_RES, DISINT_RES,
			      TELEPORT_CONTROL, STEALTH, FAST, INVIS };

    if (moves <= wailmsg + 50) return;

    wailmsg = moves;
    if (Role_if(PM_WIZARD) || Role_if(PM_CONVICT) || Role_if(PM_COURIER) || Role_if(PM_CAMPERSTRIKER) || Role_if(PM_SPACEWARS_FIGHTER) || Role_if(PM_WARRIOR) || Role_if(PM_ELPH) || Role_if(PM_TWELPH) || Race_if(PM_ELF) || Race_if(PM_ASGARDIAN) || Race_if(PM_RODNEYAN) || ishaxor || isheretic || Race_if(PM_ALBAE) || Role_if(PM_VALKYRIE)) {
	const char *who;
	int i, powercnt;

	who = (!rn2(10)) ? urace.noun :(Role_if(PM_WIZARD) || Role_if(PM_CONVICT) || Role_if(PM_COURIER) || Role_if(PM_CAMPERSTRIKER) || Role_if(PM_SPACEWARS_FIGHTER) || Role_if(PM_WARRIOR) || Role_if(PM_ELPH) || Role_if(PM_TWELPH) || Race_if(PM_ASGARDIAN) || ishaxor || isheretic || Race_if(PM_ALBAE) || Role_if(PM_VALKYRIE)) ?
		urole.name.m : Race_if(PM_RODNEYAN) ? "Wizard of Yendor" : "Elf";
	if (u.uhp == 1) {
	    pline("%s is about to die.", who);
	} else {
	    for (i = 0, powercnt = 0; i < SIZE(powers); ++i)
		if (u.uprops[powers[i]].intrinsic & INTRINSIC) ++powercnt;

	    pline(powercnt >= 4 ? "%s, all your powers will be lost..."
				: "%s, your life force is running out.", who);
	}
    } else {
	You_hear(u.uhp == 1 ? "the wailing of the Banshee..."
			    : "the howling of the CwnAnnwn...");
	if (u.uhp == 1 && (PlayerHearsSoundEffects)) pline(issoviet ? "Da! Umri, pozhaluysta! Ya nenavizhu tebya! Nadeyus', vy nastol'ko glupy, chtoby pozvolit' vse, chto chudovishche privelo vas k blizkoy smerti, chtoby snova udarit' tebya, a potom eto igra zakonchena GA GA GA!" : "SKRIIIIE-IIIIE-IIIIE-IIIIE-IIIIE!");
	else if (PlayerHearsSoundEffects) pline(issoviet ? "Nadeyus', vy prodolzhat' deystvovat' tak glupo, potomu chto togda vy budete umirat' i pridetsya svernut' novyy kharakter. Eto budet sluzhit' vam pravil'no, vy plokhoy igrok!" : "Wueueueue-oooooooh!");
    }

    if (u.uhp == 1) u.cnd_bansheecount++;
    else u.cnd_cwnannwncount++;

    if (practicantterror && ((u.cnd_bansheecount + u.cnd_cwnannwncount) >= 5)) {
	if (!u.pract_cwnannwn) {
		u.pract_cwnannwn = TRUE;
		pline("%s rings out: 'It seems that you have a habit of getting into trouble. This is not how things run in this lab, maggot. Stop it or there'll be sanctions.'", noroelaname());
	} else {
		int cwnannwnfine = (u.cnd_bansheecount + u.cnd_cwnannwncount) * 100;
		pline("%s thunders: 'I told you to stay away from trouble! That costs %d zorkmids.'", noroelaname(), cwnannwnfine);
		fineforpracticant(cwnannwnfine, 0, 0);

	}
    }

}

void
nomul(nval, txt, discountpossible)
	register int nval;
	const char *txt;
	boolean discountpossible;
{
	if (uarmc && uarmc->oartifact == ART_LIGHTSPEED_TRAVEL && nval == 0) return;

	if (uarmf && nval == 0 && itemhasappearance(uarmf, APP_TURBO_BOOTS) ) return;

	if (u.katitrapocc && nval == 0) {
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
	if (u.singtrapocc && nval == 0) {
		pline("Something tries to interrupt your attempt to clean the female shoes! If you stop now, the sexy girl will hate you!");
		if (yn("Really stop cleaning them?") == 'y') {
		      register struct monst *mtmp2;

			for (mtmp2 = fmon; mtmp2; mtmp2 = mtmp2->nmon) {

				if (!mtmp2->mtame) {
					mtmp2->mpeaceful = 0;
					mtmp2->mfrenzied = 1;
					mtmp2->mhp = mtmp2->mhpmax;
				}
			}
			pline("The beautiful girl in the sexy female shoes is very sad that you didn't finish cleaning her lovely footwear, and urges everyone in her vicinity to bludgeon you.");

		} else return;

	}

	if(multi < nval) return;	/* This is a bug fix by ab@unido */
	u.uinvulnerable = FALSE;	/* Kludge to avoid ctrl-C bug -dlc */
	u.usleep = 0;

	if (PlayerInWedgeHeels && discountpossible && (nval < -1)) {
		register int dmgreductor = 90;
		if (!(PlayerCannotUseSkills)) switch (P_SKILL(P_HIGH_HEELS)) {
			case P_BASIC: dmgreductor = 88; break;
			case P_SKILLED: dmgreductor = 86; break;
			case P_EXPERT: dmgreductor = 84; break;
			case P_MASTER: dmgreductor = 82; break;
			case P_GRAND_MASTER: dmgreductor = 80; break;
			case P_SUPREME_MASTER: dmgreductor = 78; break;
		}
		nval *= dmgreductor;
		nval /= 100;
	}

	if (discountpossible && (nval < -2) && !rn2(10) && uarmf && itemhasappearance(uarmf, APP_PLOF_HEELS) ) {
		nval /= 5;
		if (nval > -2) nval = -2;
	}

	/* Discount action will halve paralysis duration, but some paralysis sources ignore it --Amy */
	if (Discount_action && discountpossible && (nval < -1)) nval /= 2;
	if (StrongDiscount_action && discountpossible && (nval < -1)) nval /= 2;
	multi = nval;
	if (multi < 0) flags.botl = 1;
	if (txt && txt[0])
	  (void) strncpy(multi_txt, txt, BUFSZ);
	else
	  (void) memset(multi_txt, 0, BUFSZ);
	flags.travel = iflags.travel1 = flags.mv = flags.run = 0;
}

void
forcenomul(nval, txt)
	register int nval;
	const char *txt;
{
	if(multi < nval) return;	/* This is a bug fix by ab@unido */
	u.uinvulnerable = FALSE;	/* Kludge to avoid ctrl-C bug -dlc */
	u.usleep = 0;
	multi = nval;
	if (multi < 0) flags.botl = 1;
	if (txt && txt[0])
	  (void) strncpy(multi_txt, txt, BUFSZ);
	else
	  (void) memset(multi_txt, 0, BUFSZ);
	flags.travel = iflags.travel1 = flags.mv = flags.run = 0;
}

/* called when a non-movement, multi-turn action has completed */
void
unmul(msg_override)
const char *msg_override;
{
	multi = 0;	/* caller will usually have done this already */
	(void) memset(multi_txt, 0, BUFSZ);
	if (msg_override) nomovemsg = msg_override;
	else if (!nomovemsg) nomovemsg = You_can_move_again;
	if (*nomovemsg) pline("%s", nomovemsg);
	nomovemsg = 0;
	u.usleep = 0;
	if (afternmv) (*afternmv)();
	afternmv = 0;
	flags.botl = 1;
}

#endif /* OVL2 */
#ifdef OVL1


#ifdef SHOW_DMG                
/* Print the amount of damage inflicted */
/* KMH -- Centralized to one function */
void
showdmg(n)
	register int n;
{
	int lev;

	if (DamageMeterBug || u.uprops[DAMAGE_METER_BUG].extrinsic || have_damagemeterstone()) return;
	if (Role_if(PM_NOOB_MODE_BARB)) return; /* sorry --Amy */

	if (flags.showdmg && n > 1) {
		switch (Role_switch) {
			case PM_BARBARIAN: case PM_MONK: lev = 10; break;
			case PM_CAVEMAN: case PM_VALKYRIE: lev = 12; break;
			case PM_SAMURAI: case PM_KNIGHT: case PM_CHEVALIER: case PM_GOLDMINER: lev = 14; break;
			default: lev = 17; break;
	}
		switch (Race_switch) {
			case PM_GNOME: if (lev > 14) lev = 14; break;
			case PM_VEELA: if (lev > 1) lev = 1; break;
    }
#ifdef WIZARD
		if (wizard) lev = 1;
#endif
		if(GushLevel >= lev)
			pline("(%d pts.)", n);
    }
	return;
}
#endif

/* dnethack weeping angel contamination. Unlike dnethack, we have many sources of contamination, which is why I
 * don't call it "weeping angel" contamination; it's more like nuclear radiation or something. There will be certain
 * weeping angel types that use it as an attack, but the effect is just a generic "contamination". --Amy */
void
contaminate(amount, tellplayer)
register int amount;
boolean tellplayer;
{
	int precheckamount;

	if (flags.female) {
		while (!rn2(5)) amount *= 2;
	}
	/* Yes I totally know this will call the SJWs and they will whine and bitch like mad, but there is an actual reason
	 * for this: female characters are much more resistant to most item-stealers (since almost all nymphs are female),
	 * and mhitu.c also specifically makes females more resistant to certain shoes when they are used by monsters.
	 * Some players have even asserted that there is basically no advantage to playing a male character, and so I
	 * decided to make females a little bit more susceptible to contamination.
	 * THIS DOES NOT MEAN I HATE WOMEN, SO YOU SJWS CAN JUST BUGGER OFF KTHX --Amy */

	if (Race_if(PM_HUMANOID_ANGEL)) amount *= 2;
	/* It is also widely known that the angel race is teh uber pwnz0r and has almost no downsides, so I added one here */

	/* Contamination resistance divides incoming contamination by 5, but not below 1 */
	if (Cont_resist && amount > 1) {
		amount /= 5;
		if (amount < 1) amount = 1;
	}
	if (StrongCont_resist) {
		amount /= 2;
		if (amount < 1 && rn2(2)) amount = 1;
		if (amount < 1) {
			if (tellplayer) pline("Somehow, the contamination doesn't affect you.");
			return;
		}
	}

	if (isfriday && !rn2(5)) amount *= 2;

	/* Platinum is supposed to be a material that shields you against contamination --Amy */
	if (uwep && objects[uwep->otyp].oc_material == MT_PLATINUM && !rn2(10)) {
		if (tellplayer) pline("Your platinum weapon prevents you from being contaminated!");
		return;
	}
	if (u.twoweap && uswapwep && objects[uswapwep->otyp].oc_material == MT_PLATINUM && !rn2(10)) {
		if (tellplayer) pline("Your platinum off-hand weapon prevents you from being contaminated!");
		return;
	}
	if (uarm && objects[uarm->otyp].oc_material == MT_PLATINUM && !rn2(10)) {
		if (tellplayer) pline("Your platinum armor prevents you from being contaminated!");
		return;
	}
	if (uarmc && objects[uarmc->otyp].oc_material == MT_PLATINUM && !rn2(10)) {
		if (tellplayer) pline("Your platinum cloak prevents you from being contaminated!");
		return;
	}
	if (uarmh && objects[uarmh->otyp].oc_material == MT_PLATINUM && !rn2(10)) {
		if (tellplayer) pline("Your platinum helmet prevents you from being contaminated!");
		return;
	}
	if (uarms && objects[uarms->otyp].oc_material == MT_PLATINUM && !rn2(10)) {
		if (tellplayer) pline("Your platinum shield prevents you from being contaminated!");
		return;
	}
	if (uarmg && objects[uarmg->otyp].oc_material == MT_PLATINUM && !rn2(10)) {
		if (tellplayer) pline("Your platinum pair of gauntlets prevents you from being contaminated!");
		return;
	}
	if (uarmf && objects[uarmf->otyp].oc_material == MT_PLATINUM && !rn2(10)) {
		if (tellplayer) pline("Your platinum pair of boots prevents you from being contaminated!");
		return;
	}
	if (uarmu && objects[uarmu->otyp].oc_material == MT_PLATINUM && !rn2(10)) {
		if (tellplayer) pline("Your platinum shirt prevents you from being contaminated!");
		return;
	}
	if (uamul && objects[uamul->otyp].oc_material == MT_PLATINUM && !rn2(10)) {
		if (tellplayer) pline("Your platinum amulet prevents you from being contaminated!");
		return;
	}
	if (uimplant && objects[uimplant->otyp].oc_material == MT_PLATINUM && !rn2(10)) {
		if (tellplayer) pline("Your platinum implant prevents you from being contaminated!");
		return;
	}
	if (uleft && objects[uleft->otyp].oc_material == MT_PLATINUM && !rn2(10)) {
		if (tellplayer) pline("Your platinum left ring prevents you from being contaminated!");
		return;
	}
	if (uright && objects[uright->otyp].oc_material == MT_PLATINUM && !rn2(10)) {
		if (tellplayer) pline("Your platinum right ring prevents you from being contaminated!");
		return;
	}
	if (ublindf && objects[ublindf->otyp].oc_material == MT_PLATINUM && !rn2(10)) {
		if (tellplayer) pline("Your platinum blindfold prevents you from being contaminated!");
		return;
	}

	/* lead shields against contamination even more --Amy */
	if (uwep && objects[uwep->otyp].oc_material == MT_LEAD && !rn2(5)) {
		if (tellplayer) pline("Your lead weapon prevents you from being contaminated!");
		return;
	}
	if (u.twoweap && uswapwep && objects[uswapwep->otyp].oc_material == MT_LEAD && !rn2(5)) {
		if (tellplayer) pline("Your lead off-hand weapon prevents you from being contaminated!");
		return;
	}
	if (uarm && objects[uarm->otyp].oc_material == MT_LEAD && !rn2(5)) {
		if (tellplayer) pline("Your lead armor prevents you from being contaminated!");
		return;
	}
	if (uarmc && objects[uarmc->otyp].oc_material == MT_LEAD && !rn2(5)) {
		if (tellplayer) pline("Your lead cloak prevents you from being contaminated!");
		return;
	}
	if (uarmh && objects[uarmh->otyp].oc_material == MT_LEAD && !rn2(5)) {
		if (tellplayer) pline("Your lead helmet prevents you from being contaminated!");
		return;
	}
	if (uarms && objects[uarms->otyp].oc_material == MT_LEAD && !rn2(5)) {
		if (tellplayer) pline("Your lead shield prevents you from being contaminated!");
		return;
	}
	if (uarmg && objects[uarmg->otyp].oc_material == MT_LEAD && !rn2(5)) {
		if (tellplayer) pline("Your lead pair of gauntlets prevents you from being contaminated!");
		return;
	}
	if (uarmf && objects[uarmf->otyp].oc_material == MT_LEAD && !rn2(5)) {
		if (tellplayer) pline("Your lead pair of boots prevents you from being contaminated!");
		return;
	}
	if (uarmu && objects[uarmu->otyp].oc_material == MT_LEAD && !rn2(5)) {
		if (tellplayer) pline("Your lead shirt prevents you from being contaminated!");
		return;
	}
	if (uamul && objects[uamul->otyp].oc_material == MT_LEAD && !rn2(5)) {
		if (tellplayer) pline("Your lead amulet prevents you from being contaminated!");
		return;
	}
	if (uimplant && objects[uimplant->otyp].oc_material == MT_LEAD && !rn2(5)) {
		if (tellplayer) pline("Your lead implant prevents you from being contaminated!");
		return;
	}
	if (uleft && objects[uleft->otyp].oc_material == MT_LEAD && !rn2(5)) {
		if (tellplayer) pline("Your lead left ring prevents you from being contaminated!");
		return;
	}
	if (uright && objects[uright->otyp].oc_material == MT_LEAD && !rn2(5)) {
		if (tellplayer) pline("Your lead right ring prevents you from being contaminated!");
		return;
	}
	if (ublindf && objects[ublindf->otyp].oc_material == MT_LEAD && !rn2(5)) {
		if (tellplayer) pline("Your lead blindfold prevents you from being contaminated!");
		return;
	}

	precheckamount = u.contamination;

	u.contamination += amount;

	if (u.contamination >= 100 && u.contamination < 200 && precheckamount < 100) pline(FunnyHallu ? "Your body itches comfortably." : "You are now afflicted with minor contamination.");
	if (u.contamination >= 200 && u.contamination < 400 && precheckamount < 200) pline(FunnyHallu ? "The itching on your body increases." : "You are now afflicted with light contamination.");
	if (u.contamination >= 400 && u.contamination < 600 && precheckamount < 400) pline(FunnyHallu ? "You seem to be developing ulcers." : "You are now afflicted with contamination.");
	if (u.contamination >= 600 && u.contamination < 800 && precheckamount < 600) pline(FunnyHallu ? "You feel like your digestive tract started to digest itself." : "You are now afflicted with severe contamination.");
	if (u.contamination >= 800 && u.contamination < 1000 && precheckamount < 800) pline(FunnyHallu ? "You feel that your body is consuming itself from within." : "You are now afflicted with lethal contamination.");
	if (u.contamination >= 1000 && precheckamount < 1000) pline(FunnyHallu ? "You feel terminally ill. Something tells you that you only have three days to live." : "You are now afflicted with fatal contamination. Seek medical attention immediately.");

	/* if you got told that you were contaminated, and wisdom is low, give a warning --Amy */
	if (tellplayer) {
		if (ABASE(A_WIS) < 4) {
			pline("DANGER!!! Your wisdom is critically low and you're very likely to die from the contamination! You can cure it by using a scroll or wand of remove curse, or by successfully praying on a coaligned altar. Amnesia may also help in a pinch, or you may buy a decontamination service from a nurse.");
		} else if (ABASE(A_WIS) < 6) {
			pline("Warning! Your wisdom is low and if the contamination causes it to fall below 3, you die! You can cure it by using a scroll or wand of remove curse, or by successfully praying on a coaligned altar. Amnesia may also help in a pinch, or you may buy a decontamination service from a nurse.");
		}
	}

	/* Actual contamination effects are handled in attrib.c */
}

void
decontaminate(amount)
register int amount;
{
	int precheckamount;

	precheckamount = u.contamination;

	u.contamination -= amount;
	if (u.contamination < 0) u.contamination = 0;

	if (u.contamination == 0 && precheckamount >= 1) pline(FunnyHallu ? "Your body feels completely normal again." : "Your contamination has faded away completely.");
	if (u.contamination < 100 && u.contamination >= 1 && precheckamount >= 100) pline(FunnyHallu ? "Your body no longer itches." : "You are only very slightly contaminated now.");
	if (u.contamination < 200 && u.contamination >= 100 && precheckamount >= 200) pline(FunnyHallu ? "The itching on your body decreases." : "Your contamination decreased to a low level.");
	if (u.contamination < 400 && u.contamination >= 200 && precheckamount >= 400) pline(FunnyHallu ? "Your ulcers disappear." : "You are only lightly contaminated now.");
	if (u.contamination < 600 && u.contamination >= 400 && precheckamount >= 600) pline(FunnyHallu ? "Your digestive tract seems okay now." : "Your contamination is no longer severe.");
	if (u.contamination < 800 && u.contamination >= 600 && precheckamount >= 800) pline(FunnyHallu ? "Your body no longer tries to consume itself." : "You are only severely contaminated now.");
	if (u.contamination < 1000 && u.contamination >= 800 && precheckamount >= 1000) pline(FunnyHallu ? "Your terminal illness has passed and you may actually survive." : "You are only lethally contaminated now.");

}

void
reducesanity(amount)
register int amount;
{
	if (!(u.usanity)) return;

	u.usanity -= amount;
	if (u.usanity < 0) u.usanity = 0;
	if (u.usanity == 0) pline("Your sanity was cured completely.");
	else if (amount < 10) pline("Your sanity was reduced.");
	else if (amount < 100) pline("Your sanity was reduced greatly.");
	else if (amount < 1000) pline("Your sanity was reduced enormously.");
	else pline("Your sanity was reduced by a really huge amount but is still not cured.");
}

void
losehp(n, knam, k_format)
register int n;
register const char *knam;
int k_format; /* WAC k_format is an int */
{
	if (flags.iwbtg) {
		u.youaredead = 1;
		killer_format = k_format;
		killer = knam;		/* the thing that killed you */
		You("explode in a fountain of red pixels!");
		pline("GAME OVER - press R to try again");
		done(DIED);
		u.youaredead = 0;

	}

	/* let's allow the player to deflect some damage if he's lucky (higher chance with good constitution). --Amy */
	if (rn2(ABASE(A_CON))) {
	if (!rn2(3) && n >= 1) {n++; n = n / 2; if (n < 1) n = 1;}
	if (!rn2(10) && rn2(ABASE(A_CON)) && n >= 1 && GushLevel >= 10) {n++; n = n / 3; if (n < 1) n = 1;}
	if (!rn2(15) && rn2(ABASE(A_CON)) && rn2(ABASE(A_CON)) && n >= 1 && GushLevel >= 14) {n++; n = n / 4; if (n < 1) n = 1;}
	if (!rn2(20) && rn2(ABASE(A_CON)) && rn2(ABASE(A_CON)) && rn2(ABASE(A_CON)) && n >= 1 && GushLevel >= 20) {n++; n = n / 5; if (n < 1) n = 1;}
	if (!rn2(50) && rn2(ABASE(A_CON)) && rn2(ABASE(A_CON)) && rn2(ABASE(A_CON)) && rn2(ABASE(A_CON)) && n >= 1 && GushLevel >= 30) {n++; n = n / 10; if (n < 1) n = 1;}
	}

	if (PlayerInConeHeels && n > 0) {
		register int dmgreductor = 95;
		if (!(PlayerCannotUseSkills)) switch (P_SKILL(P_HIGH_HEELS)) {
			case P_BASIC: dmgreductor = 92; break;
			case P_SKILLED: dmgreductor = 89; break;
			case P_EXPERT: dmgreductor = 86; break;
			case P_MASTER: dmgreductor = 83; break;
			case P_GRAND_MASTER: dmgreductor = 80; break;
			case P_SUPREME_MASTER: dmgreductor = 77; break;
		}
		n++;
		n *= dmgreductor;
		n /= 100;
		if (n < 1) n = 1;
	}

	if (n > 0 && StrongDetect_monsters) {
		n++;
		n *= 9;
		n /= 10;
		if (n < 1) n = 1;
	}

	if (Race_if(PM_ITAQUE) && n > 0) {
		n++;
		n *= (100 - u.ulevel);
		n /= 100;
		if (n < 1) n = 1;
	}

	if (Race_if(PM_CARTHAGE) && u.usteed && (mcalcmove(u.usteed) < 12) && n > 0) {
		n++;
		n *= 4;
		n /= 5;
		if (n < 1) n = 1;
	}

	if (is_sand(u.ux,u.uy) && n > 0) {
		n++;
		n *= 4;
		n /= 5;
		if (n < 1) n = 1;
	}

	if (Race_if(PM_VIKING) && n > 0) {
		n *= 5;
		n /= 4;
	}

	if (Race_if(PM_SPARD) && n > 0) {
		n *= 5;
		n /= 4;
	}

	if (Race_if(PM_MAYMES) && uwep && weapon_type(uwep) == P_FIREARM && n > 0) {
		n++;
		n *= 4;
		n /= 5;
		if (n < 1) n = 1;
	}

	if (n > 0 && uarmf && itemhasappearance(uarmf, APP_MARJI_SHOES) ) {
		n++;
		n *= 9;
		n /= 10;
		if (n < 1) n = 1;
	}

	/* very early on, low-level characters should be more survivable
	 * this can certainly be exploited in some way; if players start exploiting it I'll have to fix it
	 * but it should fix the annoying problem where you often instadie to a trap while your max HP are bad --Amy */
	if (depth(&u.uz) == 1 && u.ulevel == 1 && moves < 1000 && In_dod(&u.uz) && n > 1) { n /= 2; }
	if (depth(&u.uz) == 1 && u.ulevel == 2 && moves < 1000 && In_dod(&u.uz) && n > 1) { n *= 2; n /= 3; }

	if (n && Race_if(PM_YUKI_PLAYA)) n += rnd(5);
	if (Role_if(PM_BLEEDER)) n = n * 2; /* bleeders are harder than hard mode */
	if (have_cursedmagicresstone()) n = n * 2;
	if (Role_if(PM_DANCER) && !rn2(3)) n = n * 2;
	if (Race_if(PM_METAL)) n *= rnd(10);
	if (HardModeEffect || u.uprops[HARD_MODE_EFFECT].extrinsic || have_hardmodestone() || (uleft && uleft->oartifact == ART_RING_OF_FAST_LIVING) || (uright && uright->oartifact == ART_RING_OF_FAST_LIVING) || (uimplant && uimplant->oartifact == ART_IME_SPEW)) n = n * 2;
	if (uamul && uamul->otyp == AMULET_OF_VULNERABILITY) n *= rnd(4);
	if (RngeFrailness) n = n * 2;

	if (Race_if(PM_SHELL) && !Upolyd && n > 1) n /= 2;

	if (isfriday && !rn2(50)) n += rnd(n);

	/* [max] Invulnerable no dmg */
	if (Invulnerable || (Stoned_chiller && Stoned)) {
		n = 0;
		pline("You are unharmed!");
		/* NOTE: DO NOT RETURN - losehp is also called to check for death 
		 * via u.uhp < 1
		 */
	}        
	else if (u.metalguard) {
		    u.metalguard = 0;
		    n = 0;
		    Your("metal guard prevents the damage!");
	}

	if (uactivesymbiosis && !u.symbiotedmghack && (rn2(100) < u.symbioteaggressivity) && !(u.usymbiote.mhpmax >= 5 && u.usymbiote.mhp <= (u.usymbiote.mhpmax / 5) && rn2(5))) {
		if (tech_inuse(T_POWERBIOSIS) && n > 1) n /= 2;
		if (tech_inuse(T_IMPLANTED_SYMBIOSIS) && uimplant && objects[uimplant->otyp].oc_charged && uimplant->spe > 0) {
			int imbiophases = uimplant->spe;
			while ((imbiophases > 0) && n > 1) {
				imbiophases--;
				n *= 10;
				n /= 11;
			}
		}
		u.usymbiote.mhp -= n;
		Your("%s symbiote takes the damage for you.", mons[u.usymbiote.mnum].mname);
		if (u.usymbiote.mhp <= 0) {
			u.usymbiote.active = 0;
			u.usymbiote.mnum = PM_PLAYERMON;
			u.usymbiote.mhp = 0;
			u.usymbiote.mhpmax = 0;
			u.usymbiote.cursed = u.usymbiote.hvycurse = u.usymbiote.prmcurse = u.usymbiote.bbcurse = u.usymbiote.morgcurse = u.usymbiote.evilcurse = u.usymbiote.stckcurse = 0;
			u.cnd_symbiotesdied++;
			if (FunnyHallu) pline("Ack! You feel like you quaffed aqua pura by mistake, and feel like something inside you has been flushed away!");
			else Your("symbiote dies from protecting you, and you feel very sad...");
		}
		if (flags.showsymbiotehp) flags.botl = TRUE;
	} else if (u.disruptionshield && u.uen >= n) {
		u.uen -= n;
		pline("Your mana shield takes the damage for you!");
		flags.botl = 1;

	} else if (Upolyd) {
		u.mh -= n;
		if (u.mhmax < u.mh) u.mhmax = u.mh;
		if (u.mh < 1) {

	    	    rehumanize();

		} else if (n > 0 && u.mh*10 < u.mhmax && Unchanging)
		    maybe_wail();

#ifdef SHOW_DMG                
		if (flags.showdmg && !(DamageMeterBug || u.uprops[DAMAGE_METER_BUG].extrinsic || have_damagemeterstone()) && !DisplayDoesNotGoAtAll && n > 0) { 
			pline("[-%d -> %d]", n, (Upolyd ? (u.mh) : (u.uhp) ) );  /* WAC see damage */
		}
#endif
		return;
	} else {
	u.uhplast = u.uhp;
	u.uhp -= n;
	if(u.uhp > u.uhpmax)
		u.uhpmax = u.uhp;	/* perhaps n was negative */
	}

	flags.botl = 1; /* Update status bar */

	if(u.uhp < 1) {
		u.youaredead = 1;
		killer_format = k_format;
		killer = knam;		/* the thing that killed you */
		You(isangbander ? "have died." : "die...");
		done(DIED);
		u.youaredead = 0;
	} else if (n > 0 && u.uhp*10 < u.uhpmax) {
		maybe_wail();
	}

#ifdef SHOW_DMG                
	if (flags.showdmg && !(DamageMeterBug || u.uprops[DAMAGE_METER_BUG].extrinsic || have_damagemeterstone()) && !DisplayDoesNotGoAtAll && n > 0) { 

		pline("[-%d -> %d]", n, (Upolyd ? (u.mh) : (u.uhp) ) );  /* WAC see damage */
		if (!Upolyd && (( (u.uhp) * 5) < u.uhpmax)) pline(isangbander ? "***LOW HITPOINT WARNING***" : "Warning: HP low!");
		if (isangbander && (!Upolyd && (( (u.uhp) * 5) < u.uhpmax)) && (PlayerHearsSoundEffects)) pline(issoviet ? "Umeret' glupyy igrok ublyudka!" : "TSCHINGTSCHINGTSCHINGTSCHING!");

	}
#endif

	if (u.uprops[TURNLIMITATION].extrinsic || (uarmf && uarmf->oartifact == ART_OUT_OF_TIME) || (uarmu && uarmu->oartifact == ART_THERMAL_BATH) || TurnLimitation || have_limitationstone() ) {
		if (n > 0) u.ascensiontimelimit -= n;
		if (u.ascensiontimelimit < 1) u.ascensiontimelimit = 1;
	}

	if (Race_if(PM_CELTIC) && !rn2(100)) {
		if (u.berserktime) {
			if (!obsidianprotection()) switch (rn2(11)) {
			case 0:
				make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20), "celtic sickness", TRUE, SICK_NONVOMITABLE);
				break;
			case 1: make_blinded(Blinded + 25, TRUE);
				break;
			case 2: if (!Confusion)
				You("suddenly feel %s.", FunnyHallu ? "trippy" : "confused");
				make_confused(HConfusion + 25, TRUE);
				break;
			case 3: make_stunned(HStun + 25, TRUE);
				break;
			case 4: make_numbed(HNumbed + 25, TRUE);
				break;
			case 5: make_frozen(HFrozen + 25, TRUE);
				break;
			case 6: make_burned(HBurned + 25, TRUE);
				break;
			case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
				break;
			case 8: (void) make_hallucinated(HHallucination + 25, TRUE, 0L);
				break;
			case 9: make_feared(HFeared + 25, TRUE);
				break;
			case 10: make_dimmed(HDimmed + 25, TRUE);
				break;
			}

		} else u.berserktime = 25;
	}

}

int
max_carr_cap()
{
	int maxcarrcap = 5000;

	if (tech_inuse(T_LIGHTER_BALLS)) {
		if (uwep && uwep->oclass == BALL_CLASS) maxcarrcap += (uwep->owt / 2);
		if (u.twoweap && uswapwep && uswapwep->oclass == BALL_CLASS) maxcarrcap += (uswapwep->owt / 2);
	}

	return maxcarrcap;
}

int
weight_cap()
{
	register long carrcap;

	carrcap = 50*(ACURRSTR + ACURR(A_CON)) + 50 + 50*(u.ulevel);
	if (Upolyd) {
		/* consistent with can_carry() in mon.c */
		if (youmonst.data->mlet == S_NYMPH || youmonst.data == &mons[PM_GOLDEN_KNIGHT] || youmonst.data == &mons[PM_URCAGUARY])
			carrcap = max_carr_cap();
		else if (!youmonst.data->cwt)
			carrcap = ((carrcap * (long)youmonst.data->msize) / MZ_HUMAN) + 50*(u.ulevel);
		else if (!strongmonst(youmonst.data)
			|| (strongmonst(youmonst.data) && (youmonst.data->cwt > WT_HUMAN)))
			carrcap = ((carrcap * (long)youmonst.data->cwt / WT_HUMAN)) + 50*(u.ulevel);
	if (carrcap < 500) carrcap = 500;
	}

	/* Intelligence increases carrying capacity, because a smarter character is able to organize and balance their pack
	 * more evenly. (I don't remember who had this idea, maybe aosdict? --Amy) */
	carrcap += 10*(ACURR(A_INT));

	if (u.menoraget) carrcap += 200;
	if (u.bookofthedeadget) carrcap += 250;
	if (u.silverbellget) carrcap += 150;
	if (u.chaoskeyget) carrcap += 50;
	if (u.neutralkeyget) carrcap += 50;
	if (u.lawfulkeyget) carrcap += 50;
	if (u.medusaremoved) carrcap += 150;
	if (u.luckstoneget) carrcap += 50;
	if (u.sokobanfinished) carrcap += 50;
	if (u.deepminefinished) carrcap += 50;
	if (uarm && uarm->oartifact == ART_DRAGON_PLATE) carrcap -= 250;
	if (uarmg && uarmg->oartifact == ART_HANDBOXED) carrcap += 1000;
	if (uwep && uwep->oartifact == ART_GIRLFUL_BONKING) carrcap -= 500;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_GIRLFUL_BONKING) carrcap -= 500;
	if (uleft && uleft->oartifact == ART_CORGON_S_RING) carrcap += 100;
	if (uright && uright->oartifact == ART_CORGON_S_RING) carrcap += 100;

	if (!PlayerCannotUseSkills && uarm && (uarm->otyp >= ROBE && uarm->otyp <= ROBE_OF_WEAKNESS)) {

		switch (P_SKILL(P_SORESU)) {
			case P_BASIC: carrcap += 200; break;
			case P_SKILLED: carrcap += 400; break;
			case P_EXPERT: carrcap += 600; break;
			case P_MASTER: carrcap += 800; break;
			case P_GRAND_MASTER: carrcap += 1000; break;
			case P_SUPREME_MASTER: carrcap += 1200; break;

		}

	}

	if (Race_if(PM_HAXOR)) carrcap *= 2;
	if (Race_if(PM_HUMANOID_CENTAUR)) carrcap /= 2;
	if (Race_if(PM_CHIROPTERAN)) carrcap += 2000;
	if (Race_if(PM_PLAYER_SHEEP) && u.ulevel >= 20) carrcap += 2000;

	if (tech_inuse(T_LIGHTER_BALLS)) {
		if (uwep && uwep->oclass == BALL_CLASS) carrcap += (uwep->owt / 2);
		if (u.twoweap && uswapwep && uswapwep->oclass == BALL_CLASS) carrcap += (uswapwep->owt / 2);
	}

	if (carrcap < 500) carrcap = 500;

	if (Levitation || Is_airlevel(&u.uz)    /* pugh@cornell */
			|| (u.usteed && strongmonst(u.usteed->data))
	) {
		carrcap += 2000;
		if(carrcap > max_carr_cap()) carrcap = max_carr_cap();
	}
	else {
		if(carrcap > max_carr_cap()) carrcap = max_carr_cap();
		if (!Flying) {
			if(EWounded_legs & LEFT_SIDE) carrcap -= 250;
			if(EWounded_legs & RIGHT_SIDE) carrcap -= 250;
		}
		if (carrcap < 0) carrcap = 0;
	}
	if (Upolyd && !PlayerCannotUseSkills) {
		switch (P_SKILL(P_POLYMORPHING)) {
			case P_BASIC: carrcap *= 6; carrcap /= 5; break;
			case P_SKILLED: carrcap *= 7; carrcap /= 5; break;
			case P_EXPERT: carrcap *= 8; carrcap /= 5; break;
			case P_MASTER: carrcap *= 9; carrcap /= 5; break;
			case P_GRAND_MASTER: carrcap *= 10; carrcap /= 5; break;
			case P_SUPREME_MASTER: carrcap *= 11; carrcap /= 5; break;
		}
	}

	if (uarmf && uarmf->oartifact == ART_A_SPOONFUL_OF_FO_U_RK) return 5000;

	return((int) carrcap);
}

static int wc;	/* current weight_cap(); valid after call to inv_weight() */

/* returns how far beyond the normal capacity the player is currently. */
/* inv_weight() is negative if the player is below normal capacity. */
int
inv_weight()
{
	register struct obj *otmp = invent;
	register int wt = 0;

#ifndef GOLDOBJ
	/* when putting stuff into containers, gold is inserted at the head
	   of invent for easier manipulation by askchain & co, but it's also
	   retained in u.ugold in order to keep the status line accurate; we
	   mustn't add its weight in twice under that circumstance */
	/* tried to make gold lighter --Amy */
	wt = (otmp && otmp->oclass == COIN_CLASS) ? 0 :
		(int)((u.ugold + 50L) / /*100L*/1000000L);
#endif
	while (otmp) {
#ifndef GOLDOBJ
		if ( (otmp->otyp != BOULDER && otmp->otyp != LOADBOULDER) || !throws_rocks(youmonst.data))
#else
		if (otmp->oclass == COIN_CLASS)
			wt += (int)(((long)otmp->quan + 50L) / /*100L*/10000L);
		else if (otmp->otyp != BOULDER || !throws_rocks(youmonst.data))
#endif
			wt += otmp->owt;
		otmp = otmp->nobj;
	}
	if (IncreasedGravity) wt += IncreasedGravity;
	if (u.graundweight) wt += u.graundweight;
	if (uarmh && itemhasappearance(uarmh, APP_LEAD_HELMET)) wt += 50;
	if (uarmf && itemhasappearance(uarmf, APP_LEAD_BOOTS)) wt += 100;
	if (uarmf && itemhasappearance(uarmf, APP_WEIGHT_ATTACHMENT_BOOTS)) wt += 500;

	/* Symbiotes can have a weight. If you're a symbiant, the weight is considerably lower; everyone else will get
	 * a skill-dependant amount of weight added. At master and above, the weight no longer gets a multiplier --Amy
	 * climacterial role has zero symbiote weight starting at XL5 */
	if (uinsymbiosis && !(Role_if(PM_CLIMACTERIAL) && u.ulevel >= 5)) {

		if (Role_if(PM_SYMBIANT)) {
			if (mons[u.usymbiote.mnum].msize >= MZ_GIGANTIC) wt += 100;
			else if (mons[u.usymbiote.mnum].msize >= MZ_HUGE) wt += 30;
			else if (mons[u.usymbiote.mnum].msize >= MZ_LARGE) wt += 10;
			if ((int) mons[u.usymbiote.mnum].cwt > 99) wt += ((int) mons[u.usymbiote.mnum].cwt / 100);
		} else {

			int symweight = 50; /* base weight even for weightless one */

			if (mons[u.usymbiote.mnum].msize >= MZ_GIGANTIC) symweight += 1000;
			else if (mons[u.usymbiote.mnum].msize >= MZ_HUGE) symweight += 300;
			else if (mons[u.usymbiote.mnum].msize >= MZ_LARGE) symweight += 100;
			if ((int) mons[u.usymbiote.mnum].cwt > 9) symweight += ((int) mons[u.usymbiote.mnum].cwt / 10);

			/* goauld is special: they don't get a multiplier */
			if (Race_if(PM_GOAULD)) ;
			else if (!(PlayerCannotUseSkills)) {
				switch (P_SKILL(P_SYMBIOSIS)) {
					default: symweight *= 5; break;
					case P_BASIC: symweight *= 4; break;
					case P_SKILLED: symweight *= 3; break;
					case P_EXPERT: symweight *= 2; break;
					case P_MASTER: break;
					case P_GRAND_MASTER: break;
					case P_SUPREME_MASTER: break;
				}
			} else symweight *= 5;

			if (symweight < 0) symweight = 0; /* fail safe */
			wt += symweight;

		}
	}

	wc = weight_cap();
	return (wt - wc);
}

/*
 * Returns 0 if below normal capacity, or the number of "capacity units"
 * over the normal capacity the player is loaded.  Max is 5.
 */
int
calc_capacity(xtra_wt)
int xtra_wt;
{
    int cap, wt = inv_weight() + xtra_wt;

    if (wt <= 0) return UNENCUMBERED;
    if (wc <= 1) return OVERLOADED;
    cap = (wt*2 / wc) + 1;
    return min(cap, OVERLOADED);
}

int
near_capacity()
{
    return calc_capacity(0);
}

int
max_capacity()
{
    int wt = inv_weight();

    return (wt - (2 * wc));
}

boolean
check_capacity(str)
const char *str;
{
    if(near_capacity() >= EXT_ENCUMBER) {
	if(str)
	    pline("%s", str);
	else
	    You_cant("do that while carrying so much stuff.");

	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

	return 1;
    }
    return 0;
}

#endif /* OVL1 */
#ifdef OVLB

int
inv_cnt()
{
	register struct obj *otmp = invent;
	register int ct = 0;

	while(otmp){
		ct++;
		otmp = otmp->nobj;
	}
	return(ct);
}

#ifdef GOLDOBJ
/* Counts the money in an object chain. */
/* Intended use is for your or some monsters inventory, */
/* now that u.gold/m.gold is gone.*/
/* Counting money in a container might be possible too. */
long
money_cnt(otmp)
struct obj *otmp;
{
        while(otmp) {
	        /* Must change when silver & copper is implemented: */
 	        if (otmp->oclass == COIN_CLASS) return otmp->quan;
  	        otmp = otmp->nobj;
	}
	return 0;
}
#endif
#endif /* OVLB */

/*hack.c*/

STATIC_PTR void
set_litX(x,y,val)
int x, y;
void * val;
{
	if (val)
	    levl[x][y].lit = 1;
	else {
	    levl[x][y].lit = 0;
	    snuff_light_source(x, y);
	}
}

/* will zorkmids be renamed to buckazoids? */
boolean
zapmrename()
{
	if (In_ZAPM(&u.uz) || Role_if(PM_SOFTWARE_ENGINEER) || Role_if(PM_CRACKER) || Role_if(PM_JANITOR) || Role_if(PM_SPACE_MARINE) || Role_if(PM_STORMBOY) || Role_if(PM_YAUTJA) || Role_if(PM_QUARTERBACK) || Role_if(PM_PSYKER) || Role_if(PM_EMPATH) || Role_if(PM_MASTERMIND) || Role_if(PM_WEIRDBOY) || Role_if(PM_ASTRONAUT) || Role_if(PM_CYBERNINJA) || Role_if(PM_DISSIDENT) || Race_if(PM_RETICULAN) || Race_if(PM_OUTSIDER) || Role_if(PM_XELNAGA)) return TRUE;

	return FALSE;
}
