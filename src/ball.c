/*	SCCS Id: @(#)ball.c	3.4	2003/02/03	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* Ball & Chain =============================================================*/

#include "hack.h"

STATIC_DCL int bc_order(void);
STATIC_DCL void litter(void);

#ifdef DISPLAY_LAYERS
#define memory_object(x, y)	(levl[x][y].mem_obj)
#else
#define memory_object(x, y)	(levl[x][y].glyph)
#endif

void
ballfall()
{
	boolean gets_hit;

	gets_hit = (((uball->ox != u.ux) || (uball->oy != u.uy)) &&
		    ((uwep == uball)? FALSE : (boolean)rn2(5)));
	if (carried(uball)) {
		pline("Startled, you drop the iron ball.");
		if (uwep == uball)
			setuwep((struct obj *)0, FALSE, TRUE);
		if (uswapwep == uball)
			setuswapwep((struct obj *)0, FALSE);
		if (uquiver == uball)
			setuqwep((struct obj *)0);;
		if (uwep != uball)
			freeinv(uball);
	}
	if(gets_hit){
		int dmg = rn1(7,25);
		pline_The("iron ball falls on your %s.",
			body_part(HEAD));
		if (uarmh) {
		    if(is_hardmaterial(uarmh)) {
			pline("Fortunately, you are wearing a hard helmet.");
			dmg = 3;
		    } else if (flags.verbose)
			Your("%s does not protect you.", xname(uarmh));
		}
		losehp(dmg, "crunched in the head by an iron ball",
			NO_KILLER_PREFIX);
	}
}

/*
 *  To make this work, we have to mess with the hero's mind.  The rules for
 *  ball&chain are:
 *
 *	1. If the hero can see them, fine.
 *	2. If the hero can't see either, it isn't seen.
 *	3. If either is felt it is seen.
 *	4. If either is felt and moved, it disappears.
 *
 *  If the hero can see, then when a move is done, the ball and chain are
 *  first picked up, the positions under them are corrected, then they
 *  are moved after the hero moves.  Not too bad.
 *
 *  If the hero is blind, then she can "feel" the ball and/or chain at any
 *  time.  However, when the hero moves, the felt ball and/or chain become
 *  unfelt and whatever was felt "under" the ball&chain appears.  Pretty
 *  nifty, but it requires that the ball&chain "remember" what was under
 *  them --- i.e. they pick-up glyphs when they are felt and drop them when
 *  moved (and felt).  When swallowed, the ball&chain are pulled completely
 *  off of the dungeon, but are still on the object chain.  They are placed
 *  under the hero when she is expelled.
 */

/*
 * from you.h
 *	int u.bglyph		glyph under the ball
 *	int u.cglyph		glyph under the chain
 *	int u.bc_felt		mask for ball/chain being felt
 *	#define BC_BALL  0x01	bit mask in u.bc_felt for ball
 *	#define BC_CHAIN 0x02	bit mask in u.bc_felt for chain
 *	int u.bc_order		ball & chain order
 *
 * u.bc_felt is also manipulated in display.c and read.c, the others only
 * in this file.  None of these variables are valid unless the player is
 * Blind.
 */

/* values for u.bc_order */
#define BCPOS_DIFFER	0	/* ball & chain at different positions */
#define BCPOS_CHAIN	1	/* chain on top of ball */
#define BCPOS_BALL	2	/* ball on top of chain */



/*
 *  Place the ball & chain under the hero.  Make sure that the ball & chain
 *  variables are set (actually only needed when blind, but what the heck).
 *  It is assumed that when this is called, the ball and chain are NOT
 *  attached to the object list.
 *
 *  Should not be called while swallowed.
 */
void
placebc()
{
    if (!uchain || !uball) {
	impossible("Where are your ball and chain?");

	if (uchain) obj_extract_self(uchain);

	setworn((struct obj *)0, W_CHAIN);
	setworn((struct obj *)0, W_BALL);
	return;
    }

    (void) flooreffects(uchain, u.ux, u.uy, "");	/* chain might rust */

    if (!uchain || !uball) { /* can apparently happen due to erosion destroying them --Amy */
	You("no longer have a ball and chain.");

	if (uchain) obj_extract_self(uchain);

	setworn((struct obj *)0, W_CHAIN);
	setworn((struct obj *)0, W_BALL);
	return;

    }

    if (carried(uball))		/* the ball is carried */
	u.bc_order = BCPOS_DIFFER;
    else {
	/* ball might rust -- already checked when carried */
	(void) flooreffects(uball, u.ux, u.uy, "");

	if (!uchain || !uball) { /* can apparently happen due to erosion destroying them --Amy */
		You("no longer have a ball and chain.");

		if (uchain) obj_extract_self(uchain);

		setworn((struct obj *)0, W_CHAIN);
		setworn((struct obj *)0, W_BALL);
		return;

	}

	place_object(uball, u.ux, u.uy);
	u.bc_order = BCPOS_CHAIN;
    }

    /*if (!Is_waterlevel(&u.uz))*/ place_object(uchain, u.ux, u.uy);

    u.bglyph = u.cglyph = memory_object(u.ux, u.uy);   /* pick up glyph */

    newsym(u.ux,u.uy);
}

void
unplacebc()
{
    if (u.uswallow) return;	/* ball&chain not placed while swallowed */

    if (!carried(uball)) {
	obj_extract_self(uball);
	if (Blind && (u.bc_felt & BC_BALL))		/* drop glyph */
	    memory_object(uball->ox, uball->oy) = u.bglyph;

	newsym(uball->ox,uball->oy);
    }
    obj_extract_self(uchain);
    if (Blind && (u.bc_felt & BC_CHAIN))		/* drop glyph */
	memory_object(uchain->ox, uchain->oy) = u.cglyph;

    newsym(uchain->ox,uchain->oy);
    u.bc_felt = 0;					/* feel nothing */
}


/*
 *  Return the stacking of the hero's ball & chain.  This assumes that the
 *  hero is being punished.
 */
STATIC_OVL int
bc_order()
{
    struct obj *obj;

    if (uchain->ox != uball->ox || uchain->oy != uball->oy || carried(uball)
		|| u.uswallow)
	return BCPOS_DIFFER;

    for (obj = level.objects[uball->ox][uball->oy]; obj; obj = obj->nexthere) {
	if (obj == uchain) return BCPOS_CHAIN;
	if (obj == uball) return BCPOS_BALL;
    }
    impossible("bc_order:  ball&chain not in same location!");
    return BCPOS_DIFFER;
}

/*
 *  set_bc()
 *
 *  The hero is either about to go blind or already blind and just punished.
 *  Set up the ball and chain variables so that the ball and chain are "felt".
 */
void
set_bc(already_blind)
int already_blind;
{
    int ball_on_floor = !carried(uball);

    u.bc_order = bc_order();				/* get the order */
    u.bc_felt = ball_on_floor ? BC_BALL|BC_CHAIN : BC_CHAIN;	/* felt */

    if (already_blind || u.uswallow) {
	u.cglyph = u.bglyph = memory_object(u.ux, u.uy);
	return;
    }

    /*
     *  Since we can still see, remove the ball&chain and get the glyph that
     *  would be beneath them.  Then put the ball&chain back.  This is pretty
     *  disgusting, but it will work.
     */
    remove_object(uchain);
    if (ball_on_floor) remove_object(uball);

    newsym(uchain->ox, uchain->oy);
    u.cglyph = memory_object(uchain->ox, uchain->oy);

    if (u.bc_order == BCPOS_DIFFER) {		/* different locations */
	place_object(uchain, uchain->ox, uchain->oy);
	newsym(uchain->ox, uchain->oy);
	if (ball_on_floor) {
	    newsym(uball->ox, uball->oy);		/* see under ball */
	    u.bglyph = memory_object(uball->ox, uball->oy);
	    place_object(uball,  uball->ox, uball->oy);
	    newsym(uball->ox, uball->oy);		/* restore ball */
	}
    } else {
	u.bglyph = u.cglyph;
	if (u.bc_order == BCPOS_CHAIN) {
	    place_object(uball,  uball->ox, uball->oy);
	    place_object(uchain, uchain->ox, uchain->oy);
	} else {
	    place_object(uchain, uchain->ox, uchain->oy);
	    place_object(uball,  uball->ox, uball->oy);
	}
	newsym(uball->ox, uball->oy);
    }
}


/*
 *  move_bc()
 *
 *  Move the ball and chain.  This is called twice for every move.  The first
 *  time to pick up the ball and chain before the move, the second time to
 *  place the ball and chain after the move.  If the ball is carried, this
 *  function should never have BC_BALL as part of its control.
 *
 *  Should not be called while swallowed.
 */
void
move_bc(before, control, ballx, bally, chainx, chainy)
int   before, control;
xchar ballx, bally, chainx, chainy;	/* only matter !before */
{
    if (Blind) {
	/*
	 *  The hero is blind.  Time to work hard.  The ball and chain that
	 *  are attached to the hero are very special.  The hero knows that
	 *  they are attached, so when they move, the hero knows that they
	 *  aren't at the last position remembered.  This is complicated
	 *  by the fact that the hero can "feel" the surrounding locations
	 *  at any time, hence, making one or both of them show up again.
	 *  So, we have to keep track of which is felt at any one time and
	 *  act accordingly.
	 */
	if (!before) {
	    if ((control & BC_CHAIN) && (control & BC_BALL)) {
		/*
		 *  Both ball and chain moved.  If felt, drop glyph.
		 */
		if (u.bc_felt & BC_BALL)
		    memory_object(uball->ox, uball->oy) = u.bglyph;
		if (u.bc_felt & BC_CHAIN)
		    memory_object(uchain->ox, uchain->oy) = u.cglyph;
		u.bc_felt = 0;

		/* Pick up glyph at new location. */
		u.bglyph = memory_object(ballx, bally);
		u.cglyph = memory_object(chainx, chainy);

		movobj(uball,ballx,bally);
		movobj(uchain,chainx,chainy);
	    } else if (control & BC_BALL) {
		if (u.bc_felt & BC_BALL) {
		    if (u.bc_order == BCPOS_DIFFER) {	/* ball by itself */
			memory_object(uball->ox, uball->oy) = u.bglyph;
		    } else if (u.bc_order == BCPOS_BALL) {
			if (u.bc_felt & BC_CHAIN) {   /* know chain is there */
			    map_object(uchain, 0);
			} else {
			    memory_object(uball->ox, uball->oy) = u.bglyph;
			}
		    }
		    u.bc_felt &= ~BC_BALL;	/* no longer feel the ball */
		}

		/* Pick up glyph at new position. */
		u.bglyph = (ballx != chainx || bally != chainy) ?
					memory_object(ballx, bally) : u.cglyph;

		movobj(uball,ballx,bally);
	    } else if (control & BC_CHAIN) {
		if (u.bc_felt & BC_CHAIN) {
		    if (u.bc_order == BCPOS_DIFFER) {
			memory_object(uchain->ox, uchain->oy) = u.cglyph;
		    } else if (u.bc_order == BCPOS_CHAIN) {
			if (u.bc_felt & BC_BALL) {
			    map_object(uball, 0);
			} else {
			    memory_object(uchain->ox, uchain->oy) = u.cglyph;
			}
		    }
		    u.bc_felt &= ~BC_CHAIN;
		}
		/* Pick up glyph at new position. */
		u.cglyph = (ballx != chainx || bally != chainy) ?
				memory_object(chainx, chainy) : u.bglyph;

		movobj(uchain,chainx,chainy);
	    }

	    u.bc_order = bc_order();	/* reset the order */
	}

    } else {
	/*
	 *  The hero is not blind.  To make this work correctly, we need to
	 *  pick up the ball and chain before the hero moves, then put them
	 *  in their new positions after the hero moves.
	 */
	if (before) {
	    if (!control) {
		/*
		 * Neither ball nor chain is moving, so remember which was
		 * on top until !before.  Use the variable u.bc_order
		 * since it is only valid when blind.
		 */
		u.bc_order = bc_order();
	    }

	    remove_object(uchain);
	    newsym(uchain->ox, uchain->oy);
	    if (!carried(uball)) {
		remove_object(uball);
		newsym(uball->ox,  uball->oy);
	    }
	} else {
	    int on_floor = !carried(uball);

	    if ((control & BC_CHAIN) ||
				(!control && u.bc_order == BCPOS_CHAIN)) {
		/* If the chain moved or nothing moved & chain on top. */
		if (on_floor) place_object(uball,  ballx, bally);
		place_object(uchain, chainx, chainy);	/* chain on top */
	    } else {
		place_object(uchain, chainx, chainy);
		if (on_floor) place_object(uball,  ballx, bally);
							    /* ball on top */
	    }
	    newsym(chainx, chainy);
	    if (on_floor) newsym(ballx, bally);
	}
    }
}

/* return TRUE if the caller needs to place the ball and chain down again
 *
 *  Should not be called while swallowed.  Should be called before movement,
 *  because we might want to move the ball or chain to the hero's old position.
 *
 * It is called if we are moving.  It is also called if we are teleporting
 * *if* the ball doesn't move and we thus must drag the chain.  It is not
 * called for ordinary teleportation.
 *
 * allow_drag is only used in the ugly special case where teleporting must
 * drag the chain, while an identical-looking movement must drag both the ball
 * and chain.
 */
boolean
drag_ball(x, y, bc_control, ballx, bally, chainx, chainy, cause_delay,
    allow_drag)
xchar x, y;
int *bc_control;
xchar *ballx, *bally, *chainx, *chainy;
boolean *cause_delay;
boolean allow_drag;
{
	struct trap *t = (struct trap *)0;
	boolean already_in_rock;

	*ballx  = uball->ox;
	*bally  = uball->oy;
	*chainx = uchain->ox;
	*chainy = uchain->oy;
	*bc_control = 0;
	*cause_delay = FALSE;

	if (dist2(x, y, uchain->ox, uchain->oy) <= 2) {	/* nothing moved */
	    move_bc(1, *bc_control, *ballx, *bally, *chainx, *chainy);
	    return TRUE;
	}

	/* only need to move the chain? */
	if (carried(uball) || distmin(x, y, uball->ox, uball->oy) <= 2) {
	    xchar oldchainx = uchain->ox, oldchainy = uchain->oy;
	    *bc_control = BC_CHAIN;
	    move_bc(1, *bc_control, *ballx, *bally, *chainx, *chainy);
	    if (carried(uball)) {
		/* move chain only if necessary */
		if (distmin(x, y, uchain->ox, uchain->oy) > 1) {
		    *chainx = u.ux;
		    *chainy = u.uy;
		}
		return TRUE;
	    }
#define CHAIN_IN_MIDDLE(chx, chy) \
(distmin(x, y, chx, chy) <= 1 && distmin(chx, chy, uball->ox, uball->oy) <= 1)
#define IS_CHAIN_ROCK(x,y) \
(IS_ROCKWFL(levl[x][y].typ) || (IS_DOOR(levl[x][y].typ) && \
      (levl[x][y].doormask & (D_CLOSED|D_LOCKED))))
/* Don't ever move the chain into solid rock.  If we have to, then instead
 * undo the move_bc() and jump to the drag ball code.  Note that this also
 * means the "cannot carry and drag" message will not appear, since unless we
 * moved at least two squares there is no possibility of the chain position
 * being in solid rock.
 */
#define SKIP_TO_DRAG { *chainx = oldchainx; *chainy = oldchainy; \
    move_bc(0, *bc_control, *ballx, *bally, *chainx, *chainy); \
    goto drag; } 
	    if (IS_CHAIN_ROCK(u.ux, u.uy) || IS_CHAIN_ROCK(*chainx, *chainy)
			|| IS_CHAIN_ROCK(uball->ox, uball->oy))
		already_in_rock = TRUE;
	    else
		already_in_rock = FALSE;

	    switch(dist2(x, y, uball->ox, uball->oy)) {
		/* two spaces diagonal from ball, move chain inbetween */
		case 8:
		    *chainx = (uball->ox + x)/2;
		    *chainy = (uball->oy + y)/2;
		    if (IS_CHAIN_ROCK(*chainx, *chainy) && !already_in_rock)
			SKIP_TO_DRAG;
		    break;

		/* player is distance 2/1 from ball; move chain to one of the
		 * two spaces between
		 *   @
		 *   __
		 *    0
		 */
		case 5: {
		    xchar tempx, tempy, tempx2, tempy2;

		    /* find position closest to current position of chain */
		    /* no effect if current position is already OK */
		    if (abs(x - uball->ox) == 1) {
			tempx = x;
			tempx2 = uball->ox;
			tempy = tempy2 = (uball->oy + y)/2;
		    } else {
			tempx = tempx2 = (uball->ox + x)/2;
			tempy = y;
			tempy2 = uball->oy;
		    }
		    if (IS_CHAIN_ROCK(tempx, tempy) &&
				!IS_CHAIN_ROCK(tempx2, tempy2) &&
				!already_in_rock) {
			if (allow_drag) {
			    /* Avoid pathological case *if* not teleporting:
			     *   0			    0_
			     *   _X  move northeast  ----->  X@
			     *    @
			     */
			    if (dist2(u.ux, u.uy, uball->ox, uball->oy) == 5 &&
				  dist2(x, y, tempx, tempy) == 1)
				SKIP_TO_DRAG;
			    /* Avoid pathological case *if* not teleporting:
			     *    0			     0
			     *   _X  move east       ----->  X_
			     *    @			      @
			     */
			    if (dist2(u.ux, u.uy, uball->ox, uball->oy) == 4 &&
				  dist2(x, y, tempx, tempy) == 2)
				SKIP_TO_DRAG;
			}
			*chainx = tempx2;
			*chainy = tempy2;
		    } else if (!IS_CHAIN_ROCK(tempx, tempy) &&
				IS_CHAIN_ROCK(tempx2, tempy2) &&
				!already_in_rock) {
			if (allow_drag) {
			    if (dist2(u.ux, u.uy, uball->ox, uball->oy) == 5 &&
				    dist2(x, y, tempx2, tempy2) == 1)
				SKIP_TO_DRAG;
			    if (dist2(u.ux, u.uy, uball->ox, uball->oy) == 4 &&
				  dist2(x, y, tempx2, tempy2) == 2)
				SKIP_TO_DRAG;
			}
			*chainx = tempx;
			*chainy = tempy;
		    } else if (IS_CHAIN_ROCK(tempx, tempy) &&
				IS_CHAIN_ROCK(tempx2, tempy2) &&
				!already_in_rock) {
			SKIP_TO_DRAG;
		    } else if (dist2(tempx, tempy, uchain->ox, uchain->oy) <
			 dist2(tempx2, tempy2, uchain->ox, uchain->oy) ||
		       ((dist2(tempx, tempy, uchain->ox, uchain->oy) ==
			 dist2(tempx2, tempy2, uchain->ox, uchain->oy)) && rn2(2))) {
			*chainx = tempx;
			*chainy = tempy;
		    } else {
			*chainx = tempx2;
			*chainy = tempy2;
		    }
		    break;
		}

		/* ball is two spaces horizontal or vertical from player; move*/
		/* chain inbetween *unless* current chain position is OK */
		case 4:
		    if (CHAIN_IN_MIDDLE(uchain->ox, uchain->oy))
			break;
		    *chainx = (x + uball->ox)/2;
		    *chainy = (y + uball->oy)/2;
		    if (IS_CHAIN_ROCK(*chainx, *chainy) && !already_in_rock)
			SKIP_TO_DRAG;
		    break;
		
		/* ball is one space diagonal from player.  Check for the
		 * following special case:
		 *   @
		 *    _    moving southwest becomes  @_
		 *   0                                0
		 * (This will also catch teleporting that happens to resemble
		 * this case, but oh well.)  Otherwise fall through.
		 */
		case 2:
		    if (dist2(x, y, uball->ox, uball->oy) == 2 &&
			    dist2(x, y, uchain->ox, uchain->oy) == 4) {
			if (uchain->oy == y)
			    *chainx = uball->ox;
			else
			    *chainy = uball->oy;
			if (IS_CHAIN_ROCK(*chainx, *chainy) && !already_in_rock)
			    SKIP_TO_DRAG;
			break;
		    }
		    /* fall through */
		case 1:
		case 0:
		    /* do nothing if possible */
		    if (CHAIN_IN_MIDDLE(uchain->ox, uchain->oy))
			break;
		    /* otherwise try to drag chain to player's old position */
		    if (CHAIN_IN_MIDDLE(u.ux, u.uy)) {
			*chainx = u.ux;
			*chainy = u.uy;
			break;
		    }
		    /* otherwise use player's new position (they must have
		       teleported, for this to happen) */
		    *chainx = x;
		    *chainy = y;
		    break;
		
		default: impossible("bad chain movement");
		    break;
	    }
#undef SKIP_TO_DRAG
#undef IS_CHAIN_ROCK
#undef CHAIN_IN_MIDDLE
	    return TRUE;
	}

drag:

	if (near_capacity() > SLT_ENCUMBER && dist2(x, y, u.ux, u.uy) <= 2) {
	    You("cannot %sdrag the heavy iron ball.",
			    invent ? "carry all that and also " : "");
	    nomul(0, 0, FALSE);
	    return FALSE;
	}

	if ((is_pool(uchain->ox, uchain->oy) &&
			/* water not mere continuation of previous water */
			(levl[uchain->ox][uchain->oy].typ == POOL ||
			 !is_pool(uball->ox, uball->oy) ||
			 levl[uball->ox][uball->oy].typ == POOL))
	    || ((t = t_at(uchain->ox, uchain->oy)) &&
			(t->ttyp == PIT ||
			 t->ttyp == SPIKED_PIT ||
			 t->ttyp == GIANT_CHASM ||
			 t->ttyp == SHIT_PIT ||
			 t->ttyp == MANA_PIT ||
			 t->ttyp == ANOXIC_PIT ||
			 t->ttyp == HYPOXIC_PIT ||
			 t->ttyp == ACID_PIT ||
			 t->ttyp == HOLE ||
			 t->ttyp == SHAFT_TRAP ||
			 t->ttyp == CURRENT_SHAFT ||
			 t->ttyp == TRAPDOOR)) ) {

	    if (Levitation) {
		You_feel("a tug from the iron ball.");
		if (t && !t->hiddentrap) t->tseen = 1;
	    } else {
		struct monst *victim;

		You("are jerked back by the iron ball!");
		if ((victim = m_at(uchain->ox, uchain->oy)) != 0) {
		    int tmp;
			int dieroll = rnd(20);

		    tmp = -2 + Luck + find_mac(victim);
		    tmp += omon_adj(victim, uball, TRUE);
		    if (tmp >= rnd(dieroll))
			(void) hmon(victim,uball,3,dieroll);
		    else
			miss(xname(uball), victim);

		}		/* now check again in case mon died */
		if (!m_at(uchain->ox, uchain->oy)) {
		    u.ux = uchain->ox;
		    u.uy = uchain->oy;
		    newsym(u.ux0, u.uy0);
		}
		nomul(0, 0, FALSE);

		*bc_control = BC_BALL;
		move_bc(1, *bc_control, *ballx, *bally, *chainx, *chainy);
		*ballx = uchain->ox;
		*bally = uchain->oy;
		move_bc(0, *bc_control, *ballx, *bally, *chainx, *chainy);
		spoteffects(TRUE);
		return FALSE;
	    }
	}

	*bc_control = BC_BALL|BC_CHAIN;

	move_bc(1, *bc_control, *ballx, *bally, *chainx, *chainy);
	if (dist2(x, y, u.ux, u.uy) > 2) {
	    /* Awful case: we're still in range of the ball, so we thought we
	     * could only move the chain, but it turned out that the target
	     * square for the chain was rock, so we had to drag it instead.
	     * But we can't drag it either, because we teleported and are more
	     * than one square from our old position.  Revert to the teleport
	     * behavior.
	     */
	    *ballx = *chainx = x;
	    *bally = *chainy = y;
	} else {
	    *ballx  = uchain->ox;
	    *bally  = uchain->oy;
	    *chainx = u.ux;
	    *chainy = u.uy;
	}
	*cause_delay = TRUE;
	return TRUE;
}

/*
 *  drop_ball()
 *
 *  The punished hero drops or throws her iron ball.  If the hero is
 *  blind, we must reset the order and glyph.  Check for side effects.
 *  This routine expects the ball to be already placed.
 *
 *  Should not be called while swallowed.
 */
void
drop_ball(x, y)
xchar x, y;
{
    if (Blind) {
	u.bc_order = bc_order();			/* get the order */
							/* pick up glyph */
	u.bglyph = (u.bc_order) ? u.cglyph : memory_object(x, y);
    }

    if (x != u.ux || y != u.uy) {
	struct trap *t;
	const char *pullmsg = "The ball pulls you out of the %s!";

	if (u.utrap && u.utraptype != TT_INFLOOR) {
	    switch(u.utraptype) {
	    case TT_PIT:
		pline(pullmsg, "pit");
		break;
	    case TT_GLUE:
		pline(pullmsg, "glue");
		break;
	    case TT_WEB:
		pline(pullmsg, "web");
		pline_The("web is destroyed!");
		deltrap(t_at(u.ux,u.uy));
		break;
	    case TT_LAVA:
		pline(pullmsg, "lava");
		break;
	    case TT_BEARTRAP: {
		register long side = rn2(3) ? LEFT_SIDE : RIGHT_SIDE;
		pline(pullmsg, "bear trap");
		set_wounded_legs(side, HWounded_legs + rn1(1000, 500));
		if (!u.usteed)
		{
		    Your("%s %s is severely damaged.",
					(side == LEFT_SIDE) ? "left" : "right",
					body_part(LEG));
		    losehp(2, "leg damage from being pulled out of a bear trap",
					KILLED_BY);
		}
		break;
	      }
	    }
	    u.utrap = 0;
	    fill_pit(u.ux, u.uy);
	}

	u.ux0 = u.ux;
	u.uy0 = u.uy;
	if (!Levitation && !MON_AT(x, y) && !u.utrap &&
			    (is_pool(x, y) ||
			     ((t = t_at(x, y)) &&
			      (t->ttyp == PIT || t->ttyp == ANOXIC_PIT || t->ttyp == HYPOXIC_PIT || t->ttyp == ACID_PIT || t->ttyp == SPIKED_PIT || t->ttyp == GIANT_CHASM || t->ttyp == SHIT_PIT || t->ttyp == MANA_PIT || t->ttyp == SHAFT_TRAP || t->ttyp == CURRENT_SHAFT ||
			       t->ttyp == TRAPDOOR || t->ttyp == HOLE)))) {
	    u.ux = x;
	    u.uy = y;
	} else {
	    u.ux = x - u.dx;
	    u.uy = y - u.dy;
	}
	vision_full_recalc = 1;	/* hero has moved, recalculate vision later */

	if (Blind) {
	    /* drop glyph under the chain */
	    if (u.bc_felt & BC_CHAIN)
		memory_object(uchain->ox, uchain->oy) = u.cglyph;
	    u.bc_felt  = 0;		/* feel nothing */
	    /* pick up new glyph */
	    u.cglyph = (u.bc_order) ? u.bglyph : memory_object(u.ux, u.uy);
	}
	movobj(uchain,u.ux,u.uy);	/* has a newsym */
	if (Blind) {
	    u.bc_order = bc_order();
	}
	newsym(u.ux0,u.uy0);		/* clean up old position */
	if (u.ux0 != u.ux || u.uy0 != u.uy) {
	    spoteffects(TRUE);
	    /* This used to give sokoban penalties but you can't actually bypass anything so the penalty is removed --Amy */
	    /* Soviet Russia comment is in apply.c */

		if (issoviet && In_sokoban(&u.uz)) {
			change_luck(-1);
			pline("Teper' vy teryayete ochko udachi KHAR KHAR. Eto deystviye ne pomoglo vam reshit' golovolomki, no my takiye elitnyye.");
			if (evilfriday) u.ugangr++;
		}

	}
    }
}


STATIC_OVL void
litter()
{
	struct obj *otmp = invent, *nextobj;
	int capacity = weight_cap();

	while (otmp) {
		nextobj = otmp->nobj;
		if ((otmp != uball) && (rnd(capacity) <= (int)otmp->owt)) {
			if (canletgo(otmp, "")) {
				Your("%s you down the stairs.",
				     aobjnam(otmp, "follow"));
				dropx(otmp);
			}
		}
		otmp = nextobj;
	}
}

void
drag_down()
{
	boolean forward;
	uchar dragchance = 3;

	/*
	 *	Assume that the ball falls forward if:
	 *
	 *	a) the character is wielding it, or
	 *	b) the character has both hands available to hold it (i.e. is
	 *	   not wielding any weapon), or
	 *	c) (perhaps) it falls forward out of his non-weapon hand
	 */

	forward = carried(uball) && (uwep == uball || !uwep || !rn2(3));

	if (carried(uball))
		You("lose your grip on the iron ball.");

	if (forward) {
		if(rn2(6)) {
			pline_The("iron ball drags you downstairs!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vy s udovol'stviyem yeshche peretashchiv etu duratskuyu myach vmeste?" : "Rlalalalalaaaaar!");
			losehp(rnd(6), "dragged downstairs by an iron ball",
				NO_KILLER_PREFIX);
			litter();
		}
	} else {
		if(rn2(2)) {
			pline_The("iron ball smacks into you!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Kha-kha-kha, tip bloka l'da smeyetsya yego zadnitsu smotret' vy imeyete delo s etoy duratskoy myach." : "Tschackschwack!");
			losehp(rnd(20), "iron ball collision", KILLED_BY_AN);
			exercise(A_STR, FALSE);
			dragchance -= 2;
		}
		if( (int) dragchance >= rnd(6)) {
			pline_The("iron ball drags you downstairs!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vy s udovol'stviyem yeshche peretashchiv etu duratskuyu myach vmeste?" : "Rlalalalalaaaaar!");
			losehp(rnd(3), "dragged downstairs by an iron ball",
				NO_KILLER_PREFIX);
			exercise(A_STR, FALSE);
			litter();
		}
	}
}

/*ball.c*/
