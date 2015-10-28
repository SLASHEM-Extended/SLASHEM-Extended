/*	SCCS Id: @(#)hack.c	3.4	2003/04/30	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

/*#ifdef OVL1
STATIC_DCL void NDECL(maybe_wail);
#endif*/ /*OVL1*/
STATIC_DCL int NDECL(moverock);
STATIC_DCL int FDECL(still_chewing,(XCHAR_P,XCHAR_P));
#ifdef SINKS
STATIC_DCL void NDECL(dosinkfall);
#endif
STATIC_DCL boolean FDECL(findtravelpath, (BOOLEAN_P));
STATIC_DCL boolean FDECL(monstinroom, (struct permonst *,int));

STATIC_DCL void FDECL(move_update, (BOOLEAN_P));
STATIC_PTR void FDECL(set_litX, (int,int,genericptr_t));

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
		  otmp = mksobj(herb, TRUE, FALSE);
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
dgn_growths(showmsg, update)
boolean showmsg; /* show messages */
boolean update;  /* do newsym() */
{
   int herbnum = rn2(SIZE(herb_info));
   int randomx, randomy;
   int i, j, count, randchance=0;
	register struct monst *mtmp;

   if (!rn2(100)) (void) seed_tree(-1,-1);
   if (herb_info[herbnum].in_water)
     (void) grow_water_herbs(herb_info[herbnum].herb, -1,-1);
   else
     (void) grow_herbs(herb_info[herbnum].herb, -1,-1, showmsg, update);
   if (!rn2(30))
     (void) drop_ripe_treefruit(-1,-1, showmsg, update);
   (void) water_current(-1,-1, rn2(8), 
			Is_waterlevel(&u.uz) ? 200 : 25, showmsg, update);

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
   if (!rn2(iswarper ? 5 : 10) || (!(u.monstertimefinish % 517) && !rn2(iswarper ? 5 : 10) ) || (!(u.monstertimefinish % 3517) && !rn2(iswarper ? 2 : 5) ) || !(u.monstertimefinish % 23517) ) {
	randomx = rn1(COLNO-3,2);
	randomy = rn2(ROWNO);

	if (randomx && randomy && isok(randomx, randomy) && ((levl[randomx][randomy].wall_info & W_NONDIGGABLE) == 0) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR || (levl[randomx][randomy].typ == DOOR && levl[randomx][randomy].doormask == D_NODOOR) ) ) {
		count = 0;
		for (i= -1; i<=1; i++) for(j= -1; j<=1; j++) {
			if (!i && !j) continue;
			if (!isok(randomx+i, randomy+j) || IS_STWALL(levl[randomx+i][randomy+j].typ) )
				count++;
		}
		switch (count) {
	
			case 8:
				randchance = 1;
				break;
			case 7:
				randchance = 2;
				break;
			case 6:
				randchance = 1000;
				break;
			case 5:
				randchance = 200;
				break;
			case 4:
				randchance = 50;
				break;
			case 3:
				randchance = 10;
				break;
			case 2:
				randchance = 3;
				break;
			case 1:
				randchance = 2;
				break;
			case 0:
			default: /* e.g. if it's 9 */
				randchance = 1;
				break;
	
		}
		/*pline("coord %d,%d, count %d, chance %d",randomx, randomy, count, randchance);*/

		if (!rn2(randchance)) {


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
     dgn_growths(FALSE, FALSE);
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
	nomul(0, 0);
	if (Levitation || Is_airlevel(&u.uz)) {
		if (Blind) feel_location(sx,sy);
	    You("don't have enough leverage to push %s.", the(xname(otmp)));
	    /* Give them a chance to climb over it? */
	    return -1;
	}
	if (verysmall(youmonst.data) && !Race_if(PM_TRANSFORMER) 
#ifdef STEED
		 && !u.usteed
#endif
				    ) {
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
			 !(ttmp && ((ttmp->ttyp == PIT) || (ttmp->ttyp == SHIT_PIT) ||
				    (ttmp->ttyp == SPIKED_PIT) || (ttmp->ttyp == GIANT_CHASM))))) {

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
		    pline("Perhaps that's why %s cannot move it.",
#ifdef STEED
				u.usteed ? y_monnam(u.usteed) :
#endif
				"you");
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
		    deltrap(ttmp);
		    delobj(otmp);
		    bury_objs(rx, ry);
		    if (cansee(rx,ry)) newsym(rx,ry);
		    continue;
		case LEVEL_TELEP:
		case TELEP_TRAP:
#ifdef STEED
		    if (u.usteed)
			pline("%s pushes %s and suddenly it disappears!",
			      upstart(y_monnam(u.usteed)), the(xname(otmp)));
		    else
#endif
		    You("push %s and suddenly it disappears!",
			the(xname(otmp)));
		    if (ttmp->ttyp == TELEP_TRAP)
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
#ifdef STEED
		if (!u.usteed) {
#endif
		  if (moves > lastmovetime+2 || moves < lastmovetime)
		    pline("With %s effort you move %s.",
			  throws_rocks(youmonst.data) ? "little" : "great",
			  the(xname(otmp)));
		  if (!rn2(50)) exercise(A_STR, TRUE);
#ifdef STEED
		} else 
		    pline("%s moves %s.",
			  upstart(y_monnam(u.usteed)), the(xname(otmp)));
#endif
		lastmovetime = moves;
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
#ifdef STEED
	  if (u.usteed)
	    pline("%s tries to move %s, but cannot.",
		  upstart(y_monnam(u.usteed)), the(xname(otmp)));
	  else
#endif
	    You("try to move %s, but in vain.", the(xname(otmp)));
		 if (Blind) feel_location(sx,sy);
	cannot_push:
	    if (throws_rocks(youmonst.data)) {
#ifdef STEED
		if (u.usteed && P_SKILL(P_RIDING) < P_BASIC) {
		    You("aren't skilled enough to %s %s from %s.",
			(flags.pickup && !In_sokoban(&u.uz))
			    ? "pick up" : "push aside",
			the(xname(otmp)), y_monnam(u.usteed));
		} else
#endif
		{
		    pline("However, you can easily %s.",
			(flags.pickup && !In_sokoban(&u.uz))
			    ? "pick it up" : "push it aside");
		if (yn("Do it?") != 'y')
		return (-1);

		    if (In_sokoban(&u.uz))
			{change_luck(-1);
			pline("You cheater!");
			}
			/* Sokoban guilt */
		    break;
		}
		break;
	    }

	    if (
#ifdef STEED
		!u.usteed &&
#endif	    
		(((!invent || inv_weight() <= -1500) &&
		 (!u.dx || !u.dy || (IS_ROCK(levl[u.ux][sy].typ)
				     && IS_ROCK(levl[sx][u.uy].typ))))
		|| verysmall(youmonst.data))) {

		if (yn("However, you can squeeze yourself into a small opening. Do it?") != 'y')
		return (-1);
		else {
		if (In_sokoban(&u.uz))
			{change_luck(-1);
			pline("You cheater!");
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
	(void) memset((genericptr_t)&digging, 0, sizeof digging);

    if (!boulder && IS_ROCK(lev->typ) && !may_dig(x,y)) {
	You("hurt your teeth on the %s.",
	    IS_TREE(lev->typ) ? "tree" : "hard stone");
	nomul(0, 0);
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
	    IS_TREE(lev->typ) ? "tree" : IS_ROCK(lev->typ) ? "rock" : "door");
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
	    (void) memset((genericptr_t)&digging, 0, sizeof digging);
	    return 1;
	}

    } else if (IS_WALL(lev->typ)) {
	if (*in_rooms(x, y, SHOPBASE)) {
	    add_damage(x, y, 10L * ACURRSTR);
	    dmgtxt = "damage";
	}
	digtxt = "chew a hole in the wall.";
	if (level.flags.is_maze_lev) {
	    lev->typ = ROOM;
	} else if (level.flags.is_cavernous_lev && !in_town(x, y)) {
	    lev->typ = CORR;
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
    }

    unblock_point(x, y);	/* vision */
    newsym(x, y);
    if (digtxt) You(digtxt);	/* after newsym */
    if (dmgtxt) pay_for_damage(dmgtxt, FALSE);
    (void) memset((genericptr_t)&digging, 0, sizeof digging);
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

#ifdef SINKS
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
#endif

boolean
may_dig(x,y)
register xchar x,y;
/* intended to be called only on ROCKs */
{
    return (boolean)(!(IS_STWALL(levl[x][y].typ) &&
			(levl[x][y].wall_info & W_NONDIGGABLE)));
}

boolean
may_passwall(x,y)
register xchar x,y;
{
   return (boolean)(!(IS_STWALL(levl[x][y].typ) &&
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

    /*
     *  Check for physical obstacles.  First, the place we are going.
     */
    if (IS_ROCK(tmpr->typ) || tmpr->typ == IRONBARS) {
	if (Blind && mode == DO_MOVE) feel_location(x,y);
	if (tmpr->typ == IRONBARS) {
	    if (!(Passes_walls || passes_bars(youmonst.data) ))
		return FALSE;
	    else if (In_sokoban(&u.uz)) {
		if (mode == DO_MOVE)
		    pline_The("Sokoban bars resist your ability.");
		return FALSE;
	    }
	} else if (Passes_walls && may_passwall(x,y)) {
	    ;	/* do nothing */
	} else if (Race_if(PM_HUMANOID_DRYAD) && tmpr->typ == TREE) {
	    ;	/* dryad can walk thru trees --Amy */
	} else if (tunnels(youmonst.data) && !needspick(youmonst.data)) {
	    /* Eat the rock. */
	    if (mode == DO_MOVE && still_chewing(x,y)) return FALSE;
	} else if (flags.autodig && !flags.run && !flags.nopick &&
		   uwep && is_pick(uwep)) {
	/* MRKR: Automatic digging when wielding the appropriate tool */
	    if (mode == DO_MOVE)
		(void) use_pick_axe2(uwep);
	    return FALSE;
	} else {
	    if (mode == DO_MOVE) {
		if (Is_stronghold(&u.uz) && is_db_wall(x,y))
		    pline_The("drawbridge is up!");
		if (Passes_walls && !may_passwall(x,y) && In_sokoban(&u.uz))
		    pline_The("Sokoban walls resist your ability.");
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
		    else if (tunnels(youmonst.data) && !needspick(youmonst.data))
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
	    else if (can_ooze(&youmonst)) {
		if (mode == DO_MOVE) You("ooze under the door.");
	    } else if (tunnels(youmonst.data) && !needspick(youmonst.data)) {
		/* Eat the door. */
		if (mode == DO_MOVE && still_chewing(x,y)) return FALSE;
	    } else {
		if (mode == DO_MOVE) {
		    if (amorphous(youmonst.data))
			You("try to ooze under the door, but can't squeeze your possessions through.");
		    else if (x == ux || y == uy) {
			if (Blind || Stunned || Numbed || ACURR(A_DEX) < 10 || Fumbling) {
#ifdef STEED
			    if (u.usteed) {
				You_cant("lead %s through that closed door.",
				      y_monnam(u.usteed));
		 	    } else
#endif
			    {
			        pline("Ouch!  You bump into a door.");
			        exercise(A_DEX, FALSE);
			    }
			} else pline("That door is closed.");
		    }
		} else if (mode == TEST_TRAV) goto testdiag;
		return FALSE;
	    }
	} else {
	testdiag:
	    if (dx && dy && !Passes_walls
		&& ((tmpr->doormask & ~D_BROKEN)
#ifdef REINCARNATION
		    || Is_rogue_level(&u.uz)
#endif
		    || block_door(x,y))) {
		/* Diagonal moves into a door are not allowed. */
		if (Blind && mode == DO_MOVE)
		    feel_location(x,y);
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
	if ( (bigmonst(youmonst.data) && !Race_if(PM_TRANSFORMER) ) || (!Upolyd && Race_if(PM_HUMANOID_CENTAUR) ) ) {
	    if (mode == DO_MOVE)
		Your("body is too large to fit through.");
	    return FALSE;
	}
	if (invent && (inv_weight() + weight_cap() > 5000)) {
	    if (mode == DO_MOVE)
#ifdef CONVICT
        if (!Passes_walls)
#endif /* CONVICT */
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
	     (is_pool(x, y) || is_lava(x, y)) && levl[x][y].seenv))
	    return FALSE;
    }

    ust = &levl[ux][uy];

    /* Now see if other things block our way . . */
    if (dx && dy && !Passes_walls
		     && (IS_DOOR(ust->typ) && ((ust->doormask & ~D_BROKEN)
#ifdef REINCARNATION
			     || Is_rogue_level(&u.uz)
#endif
			     || block_entry(x, y))
			 )) {
	/* Can't move at a diagonal out of a doorway with door. */
	return FALSE;
    }

    if (sobj_at(BOULDER,x,y) && (In_sokoban(&u.uz) || !Passes_walls)) {
	if (!(Blind || Hallucination) && (flags.run >= 2) && mode != TEST_TRAV)
	    return FALSE;
	if (mode == DO_MOVE) {
	    /* tunneling monsters will chew before pushing */
	    if (tunnels(youmonst.data) && !needspick(youmonst.data) &&
		!In_sokoban(&u.uz)) {
		if (still_chewing(x,y)) return FALSE;
	    } else
		if (moverock() < 0) return FALSE;
	} else if (mode == TEST_TRAV) {
	    struct obj* obj;

	    /* don't pick two boulders in a row, unless there's a way thru */
	    if (sobj_at(BOULDER,ux,uy) && !In_sokoban(&u.uz)) {
		if (!Passes_walls &&
		    !(tunnels(youmonst.data) && !needspick(youmonst.data)) &&
		    !carrying(PICK_AXE) && !carrying(DWARVISH_MATTOCK) &&
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
    if (!guess && iflags.travel1 && distmin(u.ux, u.uy, u.tx, u.ty) == 1) {
	flags.run = 0;
	if (test_move(u.ux, u.uy, u.tx-u.ux, u.ty-u.uy, TEST_MOVE)) {
	    u.dx = u.tx-u.ux;
	    u.dy = u.ty-u.uy;
	    nomul(0, 0);
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
	(void) memset((genericptr_t)travel, 0, sizeof(travel));
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
		int dirmax = (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || u.umonnum == PM_GRID_BUG || u.umonnum == PM_WEREGRIDBUG || u.umonnum == PM_GRID_XORN || u.umonnum == PM_STONE_BUG || u.umonnum == PM_NATURAL_BUG || u.umonnum == PM_WEAPON_BUG || u.umonnum == PM_MELEE_BUG || (Race_if(PM_WEAPON_BUG) && !Upolyd))? 4 : 8;

		for (dir = 0; dir < dirmax; ++dir) {
		    int nx = x+xdir[ordered[dir]];
		    int ny = y+ydir[ordered[dir]];

		    if (!isok(nx, ny)) continue;
		    if ((!Passes_walls && !can_ooze(&youmonst) &&
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
				    nomul(0, 0);
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
    nomul(0, 0);
    return FALSE;
}

boolean
ask_about_trap(int x, int y)
{

	struct trap *traphere = t_at(x, y);
	if (/* is_pool(x, y) || is_lava(x, y) || */ (traphere && traphere->tseen) && !Confusion && !Stunned && !Hallucination)  {

		/* who the heck included this? Maybe the player doesn't really want to use the portal at all! --Amy */
		/*if (traphere->ttyp == MAGIC_PORTAL) {
			return FALSE;
		}*/

		if (Role_if(PM_TOPMODEL) && Is_qlocate(&u.uz)) /* traps on this level do wind damage */
			return TRUE;
		if (Role_if(PM_FAILED_EXISTENCE) && Is_qlocate(&u.uz))
			return TRUE;

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
		}
		return TRUE;
	}
	return FALSE;
}

boolean
ask_about_water(int x, int y)
{
	if (is_pool(u.ux, u.uy)) return FALSE;

	if (is_pool(x, y) && !Levitation && !Flying && !Confusion && !Stunned && levl[x][y].seenv) return TRUE; 

	if (is_pool(x, y) && !Confusion && !Stunned && levl[x][y].seenv && Role_if(PM_TOPMODEL) && Is_qlocate(&u.uz) ) return TRUE; 
	if (is_pool(x, y) && !Confusion && !Stunned && levl[x][y].seenv && Role_if(PM_FAILED_EXISTENCE) && Is_qlocate(&u.uz) ) return TRUE; 

	return FALSE;
}

boolean
ask_about_lava(int x, int y)
{
	if (is_lava(u.ux, u.uy)) return FALSE;

	if (is_lava(x, y) && !Levitation && !Flying && !Confusion && !Stunned && levl[x][y].seenv) return TRUE; 
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
	    nomul(0, 0);
	    return;
	}
	if(u.uswallow) {
		u.dx = u.dy = 0;
		u.ux = x = u.ustuck->mx;
		u.uy = y = u.ustuck->my;
		mtmp = u.ustuck;
	} else {
		if (Is_airlevel(&u.uz) && rn2(2) && /* was rn2(4) - let's make it a bit easier --Amy */
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
		    static int skates = 0;
		    if (!skates) skates = find_skates();
		    if ((uarmf && uarmf->otyp == skates)
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
		if (!on_ice && (HFumbling & FROMOUTSIDE))
		    HFumbling &= ~FROMOUTSIDE;

		x = u.ux + u.dx;
		y = u.uy + u.dy;
#ifdef STEED
		/* Check if your steed can move */
		if (u.usteed && (!u.usteed->mcanmove || u.usteed->msleeping)) {
		    Your("steed doesn't respond!");
		    nomul(0, 0);
		    return;
		}
#endif
		if( (Stunned && !rn2(2)) || (Confusion && !rn2(8)) /* toned down so it's less crippling --Amy */
#ifdef STEED
			|| (u.usteed && u.usteed->mconf)
#endif	
		   ) {
			register int tries = 0;

			do {
				if(tries++ > 50) {
					nomul(0, 0);
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
				nomul(0, 0);
				return;
			}
			x = u.ux + u.dx;
			y = u.uy + u.dy;
		}
		if(!isok(x, y)) {
			nomul(0, 0);
			return;
		}
		if (((trap = t_at(x, y)) && trap->tseen) ||
		    (Blind && !Levitation && !Flying &&
		     !is_clinger(youmonst.data) &&
		     (is_pool(x, y) || is_lava(x, y)) && levl[x][y].seenv)) {
			if(flags.run >= 2) {
				nomul(0, 0);
				flags.move = 0;
				return;
			} else
				nomul(0, 0);
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
			    nomul(0, 0);
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
				nomul(0, 0);
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
	    nomul(0, 0);
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
	     * different message and makes the player remember the monster.		     */
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
	    if(flags.forcefight || !mtmp->mundetected || sensemon(mtmp) ||
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
		if ( (mtmp->data == &mons[PM_DISPLACER_BEAST] || mtmp->data == &mons[PM_JUMPING_CHAMPION] || mtmp->data->mlet == S_GRUE || mtmp->data == &mons[PM_QUANTUM_MOLD] || mtmp->data == &mons[PM_QUANTUM_GROWTH] || mtmp->data == &mons[PM_QUANTUM_FUNGUS] || mtmp->data == &mons[PM_QUANTUM_PATCH] || mtmp->data == &mons[PM_QUANTUM_STALK] || mtmp->data == &mons[PM_QUANTUM_MUSHROOM] || mtmp->data == &mons[PM_QUANTUM_SPORE] || mtmp->data == &mons[PM_QUANTUM_COLONY] || mtmp->data == &mons[PM_QUANTUM_FORCE_FUNGUS] || mtmp->data == &mons[PM_QUANTUM_FORCE_PATCH] || mtmp->data == &mons[PM_QUANTUM_WARP_FUNGUS] || mtmp->data == &mons[PM_QUANTUM_WARP_PATCH]) && !rn2(2))
		    displacer = TRUE; /* grues can also displace the player to make them more annoying --Amy */
		else
		/* try to attack; note that it might evade */
		/* also, we don't attack tame when _safepet_ */
		if(attack(mtmp)) return;
	    }
	}
	/* specifying 'F' with no monster wastes a turn */
	if (flags.forcefight ||
	    /* remembered an 'I' && didn't use a move command */
	    (memory_is_invisible(x, y) && !flags.nopick)) {
		boolean expl = (Upolyd && attacktype(youmonst.data, AT_EXPL));
	    	char buf[BUFSZ];
		Sprintf(buf,"a vacant spot on the %s", surface(x,y));
		You("%s %s.",
		    expl ? "explode at" : "attack",
		    !Underwater ? "thin air" :
		    is_pool(x,y) ? "empty water" : buf);
		unmap_object(x, y); /* known empty -- remove 'I' if present */
		newsym(x, y);
		nomul(0, 0);
		if (expl) {
		    u.mh = -1;		/* dead in the current form */
		    rehumanize();
		}
		return;
	}
	if (memory_is_invisible(x, y)) {
	    unmap_object(x, y);
	    newsym(x, y);
	}
	/* not attacking an animal, so we try to move */
	if (!displacer) {

#ifdef STEED
	if (u.usteed && !u.usteed->mcanmove && (u.dx || u.dy)) {
		pline("%s won't move!", upstart(y_monnam(u.usteed)));
		nomul(0, 0);
		return;
	} else
#endif
	if( is_nonmoving(youmonst.data) && !Race_if(PM_MISSINGNO) ) {
	/* This catches the moveamt code in hack.c, preventing you from moving as a red mold
	 * even if you do get some movement points. It's mainly meant to prevent you from being unable to do anything
	 * until you get knocked out of red mold form, so you can at least sit around and maybe throw some daggers. --Amy */
		You("are rooted %s.",
		    Levitation || Is_airlevel(&u.uz) || Is_waterlevel(&u.uz) ?
		    "in place" : "to the ground");
		nomul(0, 0);
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
#ifdef STEED
				u.usteed ? "ride" :
#endif
				"crawl");
			fill_pit(u.ux, u.uy);
			vision_full_recalc = 1;	/* vision limits change */
		    } else if (flags.verbose) {
#ifdef STEED
			if (u.usteed)
			    Norep("%s is still in a pit.",
				  upstart(y_monnam(u.usteed)));
			else
#endif
			Norep( (Hallucination && !rn2(5)) ?
				"You've fallen, and you can't get up." :
				"You are still in a pit." );
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

		} else if (u.utraptype == TT_LAVA) {
		    if(flags.verbose) {
			predicament = "stuck in the lava";
#ifdef STEED
			if (u.usteed)
			    Norep("%s is %s.", upstart(y_monnam(u.usteed)),
				  predicament);
			else
#endif
			Norep("You are %s.", predicament);
		    }
		    if(!is_lava(x,y)) {
			u.utrap--;
			if((u.utrap & 0xff) == 0) {
#ifdef STEED
			    if (u.usteed)
				You("lead %s to the edge of the lava.",
				    y_monnam(u.usteed));
			    else
#endif
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
		    if(--u.utrap) {
			if(flags.verbose) {
			    predicament = "stuck to the web";
#ifdef STEED
			    if (u.usteed)
				Norep("%s is %s.", upstart(y_monnam(u.usteed)),
				      predicament);
			    else
#endif
			    Norep("You are %s.", predicament);
			}
		    } else {
#ifdef STEED
			if (u.usteed)
			    pline("%s breaks out of the web.",
				  upstart(y_monnam(u.usteed)));
			else
#endif
			You("disentangle yourself.");
		    }
		} else if (u.utraptype == TT_GLUE) {
		    if(--u.utrap) {
			if(flags.verbose) {
			    predicament = "held in place by the glue";
#ifdef STEED
			    if (u.usteed)
				Norep("%s is %s.", upstart(y_monnam(u.usteed)),
				      predicament);
			    else
#endif
			    Norep("You are %s.", predicament);
			}
		    } else {
#ifdef STEED
			if (u.usteed)
			    pline("%s breaks out of the glue.",
				  upstart(y_monnam(u.usteed)));
			else
#endif
			You("finally get the sticky glue off.");
		    }
		} else if (u.utraptype == TT_INFLOOR) {
		    if(--u.utrap) {
			if(flags.verbose) {
			    predicament = "stuck in the";
#ifdef STEED
			    if (u.usteed)
				Norep("%s is %s %s.",
				      upstart(y_monnam(u.usteed)),
				      predicament, surface(u.ux, u.uy));
			    else
#endif
			    Norep("You are %s %s.", predicament,
				  surface(u.ux, u.uy));
			}
		    } else {
#ifdef STEED
			if (u.usteed)
			    pline("%s finally wiggles free.",
				  upstart(y_monnam(u.usteed)));
			else
#endif
			You("finally wiggle free.");
		    }
		} else {
		    if(flags.verbose) {
			predicament = "caught in a bear trap";
#ifdef STEED
			if (u.usteed)
			    Norep("%s is %s.", upstart(y_monnam(u.usteed)),
				  predicament);
			else
#endif
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
	    flags.move = 0;
	    nomul(0, 0);
	    return;
	}

	if (t_at(x,y)) {

	    traphere = t_at(x,y);
	    if (traphere && traphere->ttyp == NUPESELL_TRAP) {
		flags.move = 0;
		nomul(0, 0);
		return;
	    }

	}

	/* warn player before walking into known traps */
	if (ask_about_trap(x, y)) {
		char qbuf[BUFSZ];
		trap = t_at(x, y);
		Sprintf(qbuf,"Do you really want to %s into that %s?", 
				locomotion(youmonst.data, "step"),
				defsyms[trap_to_defsym(trap->ttyp)].explanation);
		if (yn(qbuf) != 'y') {
			nomul(0, 0);
			flags.move = 0;
			return;
		}
	}

	if (ask_about_water(x, y)) {

		if (Role_if(PM_TOPMODEL) && Is_qlocate(&u.uz) ) {
		if (yn("There is a strong wind above the water. It seems dangerous. Really step there?") != 'y') {
			nomul(0, 0);
			flags.move = 0;
			return;
			}
		}
		if (Role_if(PM_FAILED_EXISTENCE) && Is_qlocate(&u.uz) ) {
		if (yn("There is a strong wind above the water. It seems dangerous. Really step there?") != 'y') {
			nomul(0, 0);
			flags.move = 0;
			return;
			}
		}

		else {
		if (yn("This is a water tile that can cause you to drown. Really step on it?") != 'y') {
			nomul(0, 0);
			flags.move = 0;
			return;
			}
		}

	}

	if (ask_about_lava(x, y)) {

		if (yn("This is a lava tile that can burn you to a crisp. Really step on it?") != 'y') {
			nomul(0, 0);
			flags.move = 0;
			return;
		}
		char bufX[BUFSZ];
		getlin ("Are you really sure [yes/no]?",bufX);
		if (strcmp (bufX, "yes")) {
			nomul(0, 0);
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
		nomul(0, 0);
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
#ifdef STEED
	/* Move your steed, too */
	if (u.usteed) {
		u.usteed->mx = u.ux;
		u.usteed->my = u.uy;
		exercise_steed();
	}
#endif

	if (displacer) {
	    char pnambuf[BUFSZ];

	    u.utrap = 0;			/* A lucky escape */
	    /* save its current description in case of polymorph */
	    Strcpy(pnambuf, mon_nam(mtmp));
	    remove_monster(x, y);
	    place_monster(mtmp, u.ux0, u.uy0);
	    /* check for displacing it into pools and traps */
	    switch (minliquid(mtmp) ? 2 : mintrap(mtmp)) {
		case 0:
		    You("displaced %s.", pnambuf);
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
	 */
	if (is_safepet(mtmp) && !( (is_hider(mtmp->data) || mtmp->egotype_hide || mtmp->egotype_mimic) && mtmp->mundetected)) {
	    /* if trapped, there's a chance the pet goes wild */
	    if (mtmp->mtrapped) {
		if (!rn2(mtmp->mtame)) {
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
		    (trap->ttyp == PIT || trap->ttyp == SPIKED_PIT || trap->ttyp == GIANT_CHASM || trap->ttyp == SHIT_PIT) &&
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
		Strcpy(pnambuf, y_monnam(mtmp));
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
		    nomul(0, 0);
	}

	if (hides_under(youmonst.data))
	    u.uundetected = OBJ_AT(u.ux, u.uy);
	else if (youmonst.data->mlet == S_EEL)
	    u.uundetected = is_pool(u.ux, u.uy) && !Is_waterlevel(&u.uz);
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
	    nomul(-2, "dragging an iron ball");
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

	    nomul(0, 0);		/* stop running or travelling */
	    if (Hallucination)
		pline("You're picking up good vibrations!");
	    else {
#ifdef STEED
	    if (u.usteed) Sprintf(buf, "beneath %s", y_monnam(u.usteed));
	    else
#endif
	    if (Levitation || Flying) Strcpy(buf, "beneath you");
	    else Sprintf(buf, "under your %s", makeplural(body_part(FOOT)));

	    You_feel("a strange vibration %s.", buf);
	    }
	    if (otmp && otmp->spe == 7 && otmp->lamplit)
		pline("%s %s!", The(xname(otmp)),
		    Blind ? "throbs palpably" : "glows with a strange light");
	}
}

/* Wounds patch gotten from 5lo's SLEX mod --Amy */

static const char * const hallu_adverb[] = { /* 5lo: Wounds patch by L */ 
    "mildly", "mostly", "somewhat", "slightly", "probably", "massively", "extremely", 
    "flagrantly", "flamboyantly", "supremely", "excessively", "truly", "terribly", 
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
		 || (!Race_if(PM_HERBALIST) && !Role_if(PM_HEALER) && !Role_if(PM_SCIENTIST) && !Race_if(PM_ALCHEMIST) && !Role_if(PM_NECROMANCER) && !Role_if(PM_UNDERTAKER) ) ) 
		/* 5lo: Expanded for more roles */ 
	    return (char *)0; 
	if (mon->mhp == mon->mhpmax || mon->mhp < 1) 
	    return (char *)0; 
	if (!Hallucination && mon->mhp <= mon->mhpmax / 6) { 
	    Sprintf(buf,"almost "); 
	    strcat(buf, nonliving(mon->data) ? "destroyed" : "dead"); 
	} else { 
	    if (Hallucination) { 
		Sprintf(buf,hallu_adverb[rn2(SIZE(hallu_adverb))]); 
		strcat(buf," "); 
	    } 
	    else if (mon->mhp <= mon->mhpmax / 4) 
	        Sprintf(buf,"horribly "); 
	    else if (mon->mhp <= mon->mhpmax / 3) 
	        Sprintf(buf,"heavily "); 
	    else if (mon->mhp <= 3 * mon->mhpmax / 4) 
	        Sprintf(buf,"moderately "); 
	    else 
		Sprintf(buf,"lightly "); 
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

		pline(Hallucination ? "This whirl is like the eye of a hurricane, but it tickles!" : "There are scathing winds here! Your skin is scraped off!");
		losehp(rnz(u.legscratching), "scathing winds", KILLED_BY);

		You("tumble...");
		make_stunned(HStun + rnz(u.legscratching), FALSE);

		u.legscratching++;
	}

	if (t_at(u.ux, u.uy) && (Role_if(PM_TOPMODEL) || Role_if(PM_FAILED_EXISTENCE)) && Is_qlocate(&u.uz) ) {

	/* every trap on the Grand Canyon level also has a lesser wind effect. --Amy */

		pline(Hallucination ? "A twister... or is that a tornado?" : "You are enclosed in a whirlwind!");
		losehp(rnd(u.legscratching + 2), "whirlwinds", KILLED_BY);

	}

	if(u.uinwater) {
		int was_underwater;

		if (!is_pool(u.ux,u.uy)) {
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
		else if (Levitation)
			You("pop out of the water like a cork!");
		/* KMH, balance patch -- new intrinsic */
		else if (Flying)
			You("fly out of the water.");
		else if (Wwalking)
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
	if (!Levitation && !u.ustuck && !Flying) {
	    /* limit recursive calls through teleds() */
	    if (is_pool(u.ux, u.uy) || is_lava(u.ux, u.uy)) {
#ifdef STEED
		if (u.usteed && !is_flyer(u.usteed->data) && (!u.usteed->egotype_flying) &&
			!is_floater(u.usteed->data) &&
			!is_clinger(u.usteed->data)) {
		    dismount_steed(Underwater ?
			    DISMOUNT_FELL : DISMOUNT_GENERIC);
		    /* dismount_steed() -> float_down() -> pickup() */
		    if (!Is_airlevel(&u.uz) && !Is_waterlevel(&u.uz))
			pick = FALSE;
		} else
#endif
		if (is_lava(u.ux, u.uy)) {
		    if (lava_effects()) return;
		} else if (!Wwalking && drown())
		    return;
	    }
	}
	check_special_room(FALSE);
#ifdef SINKS
	if(IS_SINK(levl[u.ux][u.uy].typ) && Levitation)
		dosinkfall();
#endif
	if (!in_steed_dismounting) { /* if dismounting, we'll check again later */
		struct trap *trap = t_at(u.ux, u.uy);
		boolean pit;
		pit = (trap && (trap->ttyp == PIT || trap->ttyp == SPIKED_PIT || trap->ttyp == GIANT_CHASM || trap->ttyp == SHIT_PIT));
		if (trap && pit)
			dotrap(trap, 0);	/* fall into pit */
		if (pick) (void) pickup(1);
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
			else if(uarmh && is_metallic(uarmh))
			    pline("Its blow glances off your helmet.");
			else if (u.uac + 3 <= rnd(20) && rn2(3) ) /* ac no longer gives complete protection --Amy */
			    You("are almost hit by %s!",
				x_monnam(mtmp, ARTICLE_A, "falling", 0, TRUE));
			else {
			    int dmg;
			    You("are hit by %s!",
				x_monnam(mtmp, ARTICLE_A, "falling", 0, TRUE));
			    dmg = d(4,6);
			    if(Half_physical_damage && rn2(2) ) dmg = (dmg+1) / 2;
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
			} else
			    pline("%s attacks you by surprise!",
					Amonnam(mtmp));
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

	Strcpy(u.urooms0, u.urooms);
	Strcpy(u.ushops0, u.ushops);
	if (newlev) {
		u.urooms[0] = '\0';
		u.uentered[0] = '\0';
		u.ushops[0] = '\0';
		u.ushops_entered[0] = '\0';
		Strcpy(u.ushops_left, u.ushops0);
		return;
	}
	Strcpy(u.urooms, in_rooms(u.ux, u.uy, 0));

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

		/* "Special Room monster change. Entering a special room will no longer awaken all monsters inside of the room, making the behavior the same as Vanilla Nethack and Slash'EM. Removed wake_nearby, gets really annoying" In Soviet Russia, players want to be able to mindlessly slaughter all the denizens of special rooms, because they're somehow unable to handle a game posing an actual challenge. They don't view it as too easy if they can hack up one monster after the other without the remaining ones even reacting or doing anything. But of course we all know that this is not the way it's supposed to be, so for all the other races the monsters will wake up. --Amy */

		case ZOO:
		    pline(Hallucination ? "Welcome to our Theme Park!" : "Welcome to David's treasure zoo!");
		    if (!issoviet) wake_nearby();
		    break;
		case SWAMP:
		    pline("It %s rather %s down here.",
			  Blind ? "feels" : "looks",
			  Blind ? "humid" : "muddy");
		    if (!issoviet) wake_nearby();
		    break;
		case COURT:
		    You(Hallucination ? "enter the Queen's chambers!" : "enter an opulent throne room!");
		    if (!issoviet) wake_nearby();
		    break;
		case REALZOO:
		    You(Hallucination ? "feel that some extinct species might still live here!" : "enter a smelly zoo!");
		    if (!issoviet) wake_nearby();
		    break;
		case GIANTCOURT:
		    You(Hallucination ? "enter a real huge hall!" : "enter a giant throne room!");
		    if (!issoviet) wake_nearby();
		    break;
		case DRAGONLAIR:
		    You(Hallucination ? "enter a fairy lair!" : "enter a dragon lair...");
		    if (!issoviet) wake_nearby();
		    break;
		case BADFOODSHOP:
		    You(Hallucination ? "enter some sort of market! Perhaps you can buy some weed here?" : "enter an abandoned store...");
		    if (!issoviet) wake_nearby();
		    break;
		case LEPREHALL:
		    You(Hallucination ? "encounter a Stonehenge replica!" : "enter a leprechaun hall!");
		    if (!issoviet) wake_nearby();
		    break;
		case MORGUE:
		    if(midnight()) {
			const char *run = locomotion(youmonst.data, "Run");
			pline("%s away!  %s away!", run, run);
		    } else
			{ You("have an uncanny feeling..."); }
		    if (!issoviet) wake_nearby();
		    break;
		case BEEHIVE:
		    You(Hallucination ? "enter a tracker jacker nest! RUN AWAY!!!" : "enter a giant beehive!");
		    if (!issoviet) wake_nearby();
		    break;
		case LEMUREPIT:
		    You(Hallucination ? "enter the Devil's Lair!" : "enter a pit of screaming lemures!");
		    if (!issoviet) wake_nearby();
		    break;
		case MIGOHIVE:
		    You(Hallucination ? "enter some futuristic alien structure!" : "enter a strange hive!");
		    if (!issoviet) wake_nearby();
		    break;
		case FUNGUSFARM:
		    You(Hallucination ? "enter a sticky, slimy room..." : "enter a room full of fungi!");
		    if (!issoviet) wake_nearby();
		    break;
		case COCKNEST:
		    You(Hallucination ? "sense the well-known smell of weed as you enter this room!" : "enter a disgusting nest!");
		    if (!issoviet) wake_nearby();
		    break;
		case ANTHOLE:
		    You(Hallucination ? "enter a room filled with bugs!" : "enter an anthole!");
		    if (!issoviet) wake_nearby();
		    break;
            case CLINIC:
                You(Hallucination ? "feel reminded of 'Emergency Room' as you enter this area!" : "enter a modern hospital.");
		    if (!issoviet) wake_nearby();
                break;
            case ANGELHALL:
                You(Hallucination ? "see the gods as you enter! WOW! So that's what they look like..." : "enter a radiating hall of Angels!");
		    if (!issoviet) wake_nearby();
                break;
	      case TERRORHALL:
                You(Hallucination ? "feel like you just got detected by a tripwire!" : "enter a terrifying hall.");
		    if (!issoviet) wake_nearby();
                break;
	      case TENSHALL:
                You(Hallucination ? "have died. Do you want your possessions identified? [ynq] (n) _" : "enter a killer room! This is actually an ADOM tension room.");
		    if (!issoviet) wake_nearby();
                break;
	      case ELEMHALL:
                You(Hallucination ? "get a vision of Mother Nature as you enter!" : "enter a room full of elementals!");
		    if (!issoviet) wake_nearby();
                break;
	      case NYMPHHALL:
                You(Hallucination ? "see a picture on the wall of this room. It shows the most beautiful woman you ever saw..." : "enter a beautiful garden!");
		    if (!issoviet) wake_nearby();
                break;
            case ARMORY:
                You(Hallucination ? "enter some old weapon storage chamber! Let's see what weapons are left!" : "enter a dilapidated armory.");
		    if (!issoviet) wake_nearby();
                break;
	      case COINHALL:
                You(Hallucination ? "enter a room full of treasure, and it's all going to be yours! YEAH!" : "enter a room filled with money!");
		    if (!issoviet) wake_nearby();
                break;
	      case TROLLHALL:
                You(Hallucination ? "feel that this room smells like a public toilet!" : "enter a room full of stinking trolls...");
		    if (!issoviet) wake_nearby();
                break;
	      case HUMANHALL:
                You(Hallucination ? "encounter a party room! Let's invite some hot girls!" : "encounter a living room!");
		    if (!issoviet) wake_nearby();
                break;
	      case SPIDERHALL:
                You(Hallucination ? "notice spiders of all forms and sizes in this room sitting everywhere!" : "stumble into a nest of spiders...");
		    if (!issoviet) wake_nearby();
                break;
	      case GOLEMHALL:
                You(Hallucination ? "encounter some warmeches!" : "enter a room full of golems!");
		    if (!issoviet) wake_nearby();
                break;
	      case TRAPROOM:
                if (wizard) You("enter a trapped room!");
                break;
	      case POOLROOM:
                if (wizard) You("enter a pool room!");
                break;
	      case STATUEROOM:
                if (wizard) You("enter a statue room!");
                break;
	      case INSIDEROOM:
                You(Hallucination ? "enter a normal-looking room." : "enter a weird-looking room...");
		    if (!issoviet) wake_nearby();
                break;
	      case RIVERROOM:
                You(Hallucination ? "encounter an underground mountain! Wait, what? This makes no sense!" : "encounter an underground river!");
		    if (!issoviet) wake_nearby();
                break;
	      case GRUEROOM:
                pline(Hallucination ? "It is radiant bright. You are likely to be eaten by the sun." : "It is pitch black. You are likely to be eaten by a grue.");

	    do_clear_areaX(u.ux,u.uy,		/* extra darkness --Amy */
		15, set_litX, (genericptr_t)((char *)0));
		/* IMHO grue rooms may remove light every time you enter them. --Amy */
		    if (!issoviet) wake_nearby();
                break;
		case BARRACKS:
		    if(monstinroom(&mons[PM_SOLDIER], roomno) ||
			monstinroom(&mons[PM_SERGEANT], roomno) ||
			monstinroom(&mons[PM_LIEUTENANT], roomno) ||
			monstinroom(&mons[PM_CAPTAIN], roomno) ||
			monstinroom(&mons[PM_GENERAL], roomno))
			You("enter a military barracks!");
		    else
			{ You("enter an abandoned barracks."); }
		    if (!issoviet) wake_nearby();
		    break;
		case DELPHI:
		    if(monstinroom(&mons[PM_ORACLE], roomno))
			verbalize("%s, %s, welcome to Delphi!",
					Hello((struct monst *) 0), plname);
		    if (!issoviet) wake_nearby();
		    break;
		case DOUGROOM:
		    You("feel 42.");
		    if (!issoviet) wake_nearby();
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
	if(is_pool(u.ux, u.uy)) {
	    if (Wwalking || is_floater(youmonst.data) || is_clinger(youmonst.data)
			|| (Flying && !Breathless)) {
		You("cannot dive into the water to pick things up.");
		return(0);
	    } else if (!Underwater) {
		You_cant("even see the bottom, let alone pick up %s.",
				something);
		return(0);
	    }
	}
	if (is_lava(u.ux, u.uy)) {
	    if (Wwalking || is_floater(youmonst.data) || is_clinger(youmonst.data)
			|| (Flying && !Breathless)) {
		You_cant("reach the bottom to pick things up.");
		return(0);
	    } else if (!likes_lava(youmonst.data)) {
		You("would burn to a crisp trying to pick things up.");
		return(0);
	    }
	}
	if(!OBJ_AT(u.ux, u.uy)) {
		There("is nothing here to pick up.");
		return(0);
	}
	if (!can_reach_floor()) {
#ifdef STEED
		if (u.usteed && P_SKILL(P_RIDING) < P_BASIC)
		    You("aren't skilled enough to reach from %s.",
			y_monnam(u.usteed));
		else
#endif
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
		if ((traphere->ttyp == PIT || traphere->ttyp == SPIKED_PIT || traphere->ttyp == GIANT_CHASM || traphere->ttyp == SHIT_PIT) &&
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
    if ((BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || u.umonnum == PM_GRID_BUG || u.umonnum == PM_WEREGRIDBUG || u.umonnum == PM_GRID_XORN || u.umonnum == PM_STONE_BUG || u.umonnum == PM_NATURAL_BUG || u.umonnum == PM_MELEE_BUG || u.umonnum == PM_WEAPON_BUG || (Race_if(PM_WEAPON_BUG) && !Upolyd))&& u.dx && u.dy) {
	nomul(0, 0);
	return;
    }

    if(Blind || flags.run == 0) return;
    for(x = u.ux-1; x <= u.ux+1; x++) for(y = u.uy-1; y <= u.uy+1; y++) {
	if(!isok(x,y)) continue;

	if((BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || u.umonnum == PM_GRID_BUG || u.umonnum == PM_WEREGRIDBUG || u.umonnum == PM_GRID_XORN || u.umonnum == PM_STONE_BUG || u.umonnum == PM_NATURAL_BUG || u.umonnum == PM_MELEE_BUG || u.umonnum == PM_WEAPON_BUG || (Race_if(PM_WEAPON_BUG) && !Upolyd)) && x != u.ux && y != u.uy) continue;

	if(x == u.ux && y == u.uy) continue;

	if((mtmp = m_at(x,y)) &&
		    mtmp->m_ap_type != M_AP_FURNITURE &&
		    mtmp->m_ap_type != M_AP_OBJECT &&
		    (!mtmp->minvis || See_invisible) && !mtmp->minvisreal && !mtmp->mundetected) {
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
	} else if (is_pool(x,y) || is_lava(x,y)) {
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
	nomul(0, 0);
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

/*STATIC_OVL*/ void
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
    }
}

void
nomul(nval, txt)
	register int nval;
	const char *txt;
{
	if(multi < nval) return;	/* This is a bug fix by ab@unido */
	u.uinvulnerable = FALSE;	/* Kludge to avoid ctrl-C bug -dlc */
	u.usleep = 0;
	multi = nval;
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
	if (*nomovemsg) pline(nomovemsg);
	nomovemsg = 0;
	u.usleep = 0;
	if (afternmv) (*afternmv)();
	afternmv = 0;
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
		if(u.ulevel >= lev)
			pline("(%d pts.)", n);
    }
	return;
}
#endif


void
losehp(n, knam, k_format)
register int n;
register const char *knam;
int k_format; /* WAC k_format is an int */
{
	if (flags.iwbtg) {
		killer_format = k_format;
		killer = knam;		/* the thing that killed you */
		You("explode in a fountain of red pixels!");
		pline("GAME OVER - press R to try again");
		done(DIED);

	}

	/* let's allow the player to deflect some damage if he's lucky (higher chance with good constitution). --Amy */
	if (rn2(ABASE(A_CON))) {
	if (!rn2(3) && n >= 1) {n = n / 2; if (n < 1) n = 1;}
	if (!rn2(10) && n >= 1 && u.ulevel >= 10) {n = n / 3; if (n < 1) n = 1;}
	if (!rn2(20) && n >= 1 && u.ulevel >= 20) {n = n / 5; if (n < 1) n = 1;}
	if (!rn2(50) && n >= 1 && u.ulevel >= 30) {n = n / 10; if (n < 1) n = 1;}
	}

	if (Role_if(PM_BLEEDER)) n = n * 2; /* bleeders are harder than hard mode */
	if (have_cursedmagicresstone()) n = n * 2;

	/* [max] Invulnerable no dmg */
	if (Invulnerable || (Stoned_chiller && Stoned)) {
		n = 0;
		pline("You are unharmed!");
		/* NOTE: DO NOT RETURN - losehp is also called to check for death 
		 * via u.uhp < 1
		 */
	}        



	if (Upolyd) {
		u.mh -= n;
		if (u.mhmax < u.mh) u.mhmax = u.mh;
		if (u.mh < 1)
		    rehumanize();
		else if (n > 0 && u.mh*10 < u.mhmax && Unchanging)
		    maybe_wail();
		return;
	} else {
	u.uhplast = u.uhp;
	u.uhp -= n;
	if(u.uhp > u.uhpmax)
		u.uhpmax = u.uhp;	/* perhaps n was negative */
	}

	flags.botl = 1; /* Update status bar */

	if(u.uhp < 1) {
		killer_format = k_format;
		killer = knam;		/* the thing that killed you */
		You(isangbander ? "have died." : "die...");
		done(DIED);
	} else if (n > 0 && u.uhp*10 < u.uhpmax) {
		maybe_wail();
	}

#ifdef SHOW_DMG                
	if (flags.showdmg && !DisplayLoss && !u.uprops[DISPLAY_LOST].extrinsic && !have_displaystone() && n > 0) { 

		pline("[-%d -> %d]", n, (Upolyd ? (u.mh) : (u.uhp) ) );  /* WAC see damage */
		if (!Upolyd && (( (u.uhp) * 5) < u.uhpmax)) pline(isangbander ? "***LOW HITPOINT WARNING***" : "Warning: HP low!");


	}
#endif

}

int
weight_cap()
{
	register long carrcap;

	carrcap = 50*(ACURRSTR + ACURR(A_CON)) + 50 + 50*(u.ulevel);
	if (Upolyd) {
		/* consistent with can_carry() in mon.c */
		if (youmonst.data->mlet == S_NYMPH)
			carrcap = MAX_CARR_CAP;
		else if (!youmonst.data->cwt)
			carrcap = ((carrcap * (long)youmonst.data->msize) / MZ_HUMAN) + 50*(u.ulevel);
		else if (!strongmonst(youmonst.data)
			|| (strongmonst(youmonst.data) && (youmonst.data->cwt > WT_HUMAN)))
			carrcap = ((carrcap * (long)youmonst.data->cwt / WT_HUMAN)) + 50*(u.ulevel);
	if (carrcap < 500) carrcap = 500;
	}

	if (Race_if(PM_HAXOR)) carrcap *= 2;
	if (Race_if(PM_HUMANOID_CENTAUR)) carrcap /= 2;

	if (Levitation || Is_airlevel(&u.uz)    /* pugh@cornell */
#ifdef STEED
			|| (u.usteed && strongmonst(u.usteed->data))
#endif
	)
		carrcap = MAX_CARR_CAP;
	else {
		if(carrcap > MAX_CARR_CAP) carrcap = MAX_CARR_CAP;
		if (!Flying) {
			if(EWounded_legs & LEFT_SIDE) carrcap -= 250;
			if(EWounded_legs & RIGHT_SIDE) carrcap -= 250;
		}
		if (carrcap < 0) carrcap = 0;
	}
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
		(int)((u.ugold + 50L) / /*100L*/10000L);
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
	    pline(str);
	else
	    You_cant("do that while carrying so much stuff.");
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
genericptr_t val;
{
	if (val)
	    levl[x][y].lit = 1;
	else {
	    levl[x][y].lit = 0;
	    snuff_light_source(x, y);
	}
}
