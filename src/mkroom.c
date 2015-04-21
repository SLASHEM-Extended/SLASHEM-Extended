/*	SCCS Id: @(#)mkroom.c	3.4	2001/09/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * Entry points:
 *	mkroom() -- make and stock a room of a given type
 *	nexttodoor() -- return TRUE if adjacent to a door
 *	has_dnstairs() -- return TRUE if given room has a down staircase
 *	has_upstairs() -- return TRUE if given room has an up staircase
 *	courtmon() -- generate a court monster
 *	douglas_adams_mon() -- create a Douglas-Adams_Monster
 *	save_rooms() -- save rooms into file fd
 *	rest_rooms() -- restore rooms from file fd
 */

#include "hack.h"

#ifdef OVLB
STATIC_DCL boolean FDECL(isbig, (struct mkroom *));
STATIC_DCL struct mkroom * FDECL(pick_room,(BOOLEAN_P));
STATIC_DCL void NDECL(mkshop), FDECL(mkzoo,(int)), NDECL(mkswamp);
STATIC_DCL void NDECL(mktemple);
STATIC_DCL coord * FDECL(shrine_pos, (int));
STATIC_DCL struct permonst * NDECL(morguemon);
STATIC_DCL struct permonst * NDECL(douglas_adams_mon);
STATIC_DCL struct permonst * NDECL(tenshallmon);
STATIC_DCL struct permonst * NDECL(tenshallmonB);
STATIC_DCL struct permonst * NDECL(squadmon);
STATIC_DCL struct permonst * NDECL(fungus);

/*STATIC_DCL struct permonst * FDECL(colormon, (int));*/

STATIC_DCL void NDECL(mktraproom);
STATIC_DCL void NDECL(mkpoolroom);
STATIC_DCL void NDECL(mkstatueroom);
STATIC_DCL void NDECL(mkinsideroom);
STATIC_DCL void NDECL(mkriverroom);
STATIC_DCL void FDECL(save_room, (int,struct mkroom *));
STATIC_DCL void FDECL(rest_room, (int,struct mkroom *));
#endif /* OVLB */

#define sq(x) ((x)*(x))

extern const struct shclass shtypes[];	/* defined in shknam.c */

#ifdef OVLB

STATIC_OVL boolean
isbig(sroom)
register struct mkroom *sroom;
{
	register int area = (sroom->hx - sroom->lx + 1)
			   * (sroom->hy - sroom->ly + 1);
	return((boolean)( area > 20 ));
}

void
mkroom(roomtype)
/* make and stock a room of a given type */
int	roomtype;
{
    if (roomtype >= SHOPBASE)
	mkshop();	/* someday, we should be able to specify shop type */
    else switch(roomtype) {
	case COURT:	mkzoo(COURT); break;
	case ZOO:	mkzoo(ZOO); break;
	case BEEHIVE:	mkzoo(BEEHIVE); break;
	case MORGUE:	mkzoo(MORGUE); break;
	case BARRACKS:	mkzoo(BARRACKS); break;
	case REALZOO:   mkzoo(REALZOO); break;
	case BADFOODSHOP: mkzoo(BADFOODSHOP); break;
	case DOUGROOM:	mkzoo(DOUGROOM); break;
	case DRAGONLAIR: mkzoo(DRAGONLAIR); break;
	case GIANTCOURT: mkzoo(GIANTCOURT); break;
	case SWAMP:	mkswamp(); break;
	case TEMPLE:	mktemple(); break;
	case LEPREHALL:	mkzoo(LEPREHALL); break;
	case COCKNEST:	mkzoo(COCKNEST); break;
	case ANTHOLE:	mkzoo(ANTHOLE); break;
	case LEMUREPIT: mkzoo(LEMUREPIT); break;
	case MIGOHIVE:  mkzoo(MIGOHIVE); break;
	case FUNGUSFARM: mkzoo(FUNGUSFARM); break;
	case CLINIC: mkzoo(CLINIC); break;
	case TERRORHALL: mkzoo(TERRORHALL); break;
	case TENSHALL: mkzoo(TENSHALL); break;
	case ELEMHALL: mkzoo(ELEMHALL); break;
	case ANGELHALL: mkzoo(ANGELHALL); break;
	case MIMICHALL: mkzoo(MIMICHALL); break;
	case NYMPHHALL: mkzoo(NYMPHHALL); break;
	case TROLLHALL: mkzoo(TROLLHALL); break;
	case HUMANHALL: mkzoo(HUMANHALL); break;
	case GOLEMHALL: mkzoo(GOLEMHALL); break;
	case SPIDERHALL: mkzoo(SPIDERHALL); break;
	case COINHALL: mkzoo(COINHALL); break;
	case GRUEROOM: mkzoo(GRUEROOM); break;
	case TRAPROOM:  mktraproom(); break;
	case POOLROOM:  mkpoolroom(); break;
	case STATUEROOM:  mkstatueroom(); break;
	case INSIDEROOM: mkinsideroom(); break;
	case RIVERROOM: mkriverroom(); break;
	case ARMORY: mkzoo(ARMORY); break;

	case RANDOMROOM: {

		switch (rnd(36)) {

			case 1: mkzoo(COURT); break;
			case 2: mkswamp(); break;
			case 3: mkzoo(BEEHIVE); break;
			case 4: mkzoo(MORGUE); break;
			case 5: mkzoo(BARRACKS); break;
			case 6: mkzoo(ZOO); break;
			case 7: mkzoo(REALZOO); break;
			case 8: mkzoo(GIANTCOURT); break;
			case 9: mkzoo(LEPREHALL); break;
			case 10: mkzoo(DRAGONLAIR); break;
			case 11: mkzoo(BADFOODSHOP); break;
			case 12: mkzoo(COCKNEST); break;
			case 13: mkzoo(ANTHOLE); break;
			case 14: mkzoo(LEMUREPIT); break;
			case 15: mkzoo(MIGOHIVE); break;
			case 16: mkzoo(FUNGUSFARM); break;
			case 17: mkzoo(CLINIC); break;
			case 18: mkzoo(TERRORHALL); break;
			case 19: mkzoo(ELEMHALL); break;
			case 20: mkzoo(ANGELHALL); break;
			case 21: mkzoo(MIMICHALL); break;
			case 22: mkzoo(NYMPHHALL); break;
			case 23: mkzoo(SPIDERHALL); break;
			case 24: mkzoo(TROLLHALL); break;
			case 25: mkzoo(HUMANHALL); break;
			case 26: mkzoo(GOLEMHALL); break;
			case 27: mkzoo(COINHALL); break;
			case 28: mkzoo(DOUGROOM); break;
			case 29: mkzoo(ARMORY); break;
			case 30: mkzoo(TENSHALL); break;
			case 31: mktraproom(); break;
			case 32: mkpoolroom(); break;
			case 33: mkstatueroom(); break;
			case 34: mkinsideroom(); break;
			case 35: mkriverroom(); break;
			case 36: mktemple(); break;

		}
		break;

	}

	default:	impossible("Tried to make a room of type %d.", roomtype);
    }
}

STATIC_OVL void
mkshop()
{
	register struct mkroom *sroom;
	int i = -1, j;
#ifdef WIZARD
	char *ep = (char *)0;	/* (init == lint suppression) */

	/* first determine shoptype */
	if(wizard){
#ifndef MAC
		ep = nh_getenv("SHOPTYPE");
		if(ep){
			if(*ep == 'z' || *ep == 'Z'){
				mkzoo(ZOO);
				return;
			}
			if(*ep == 'm' || *ep == 'M'){
				mkzoo(MORGUE);
				return;
			}
			if(*ep == 'b' || *ep == 'B'){
				mkzoo(BEEHIVE);
				return;
			}
			if (*ep == 'p' || *ep == 'P') {
				mkzoo(LEMUREPIT);
				return;
			}
			if (*ep == 'i' || *ep == 'I') {
				mkzoo(MIGOHIVE);
				return;
			}
			if (*ep == 'f' || *ep == 'F') {
				mkzoo(FUNGUSFARM);
				return;
			}
			if(*ep == 't' || *ep == 'T' || *ep == '\\'){
				mkzoo(COURT);
				return;
			}
			if(*ep == 's' || *ep == 'S'){
				mkzoo(BARRACKS);
				return;
			}
			if(*ep == 'a' || *ep == 'A'){
				mkzoo(ANTHOLE);
				return;
			}
			if(*ep == 'c' || *ep == 'C'){
				mkzoo(COCKNEST);
				return;
			}
			if(*ep == 'r' || *ep == 'R'){
				mkzoo(ARMORY);
				return;
			}
			if(*ep == 'l' || *ep == 'L'){
				mkzoo(LEPREHALL);
				return;
			}
			if(*ep == '_'){
				mktemple();
				return;
			}
			if(*ep == '}'){
				mkswamp();
				return;
			}
			if (*ep == 'd' || *ep == 'D') {
				mkzoo(DOUGROOM);
				return;
			}
			j = -1;
			for(i=0; shtypes[i].name; i++)
				if(*ep == def_oc_syms[(int)shtypes[i].symb]) {
					if (j < 0) j = i;
					if (!strcmp(ep + 1, shtypes[i].name))
						break;
				}
			if(*ep == 'g' || *ep == 'G')
				i = 0;
			else
				i = j;
		}
#endif
	}
#endif
	for(sroom = &rooms[0]; ; sroom++){
		if(sroom->hx < 0) return;
		if(sroom - rooms >= nroom) {
			pline("rooms not closed by -1?");
			return;
		}
		if(sroom->rtype != OROOM) continue;
		if(has_dnstairs(sroom) || has_upstairs(sroom))
			continue;
		if(
#ifdef WIZARD
		   (wizard && ep && sroom->doorct != 0) ||
#endif
			sroom->doorct == 1) break;
	}
	if (!sroom->rlit) {
		int x, y;

		for(x = sroom->lx - 1; x <= sroom->hx + 1; x++)
		for(y = sroom->ly - 1; y <= sroom->hy + 1; y++)
			levl[x][y].lit = 1;
		sroom->rlit = 1;
	}

	if(i < 0) {			/* shoptype not yet determined */
	    /* pick a shop type at random */
	    for (j = rnd(100), i = 0; (j -= shtypes[i].prob) > 0; i++)
		continue;

	    /* big rooms cannot be wand or book shops,
	     * - so make them general stores
	     */
	    /*if(isbig(sroom) && (shtypes[i].symb == WAND_CLASS
				|| shtypes[i].symb == SPBOOK_CLASS)) i = 0;*/
	}
	sroom->rtype = SHOPBASE + i;

	/* set room bits before stocking the shop */
#ifdef SPECIALIZATION
	topologize(sroom, FALSE); /* doesn't matter - this is a special room */
#else
	topologize(sroom);
#endif

	/* stock the room with a shopkeeper and artifacts */
	stock_room(i, sroom);
}

STATIC_OVL struct mkroom *
pick_room(strict)
register boolean strict;
/* pick an unused room, preferably with only one door */
{
	register struct mkroom *sroom;
	register int i = nroom;

	for(sroom = &rooms[rn2(nroom)]; i--; sroom++) {
		if(sroom == &rooms[nroom])
			sroom = &rooms[0];
		if(sroom->hx < 0)
			return (struct mkroom *)0;
		if(sroom->rtype != OROOM)	continue;
		if(!strict) {
		    if( (has_upstairs(sroom) && rn2(iswarper ? 10 : 100)) || (has_dnstairs(sroom) && rn2(3)))
			continue;
		} else if(has_upstairs(sroom) || has_dnstairs(sroom))
			continue;
		if(sroom->doorct == 1 || (!rn2(5) && !strict)
#ifdef WIZARD
						|| (wizard && !strict)
#endif
							)
			return sroom;
	}
	return (struct mkroom *)0;
}

STATIC_OVL void
mkzoo(type)
int type;
{
	register struct mkroom *sroom;

	/*if (type == BADFOODSHOP) {
	   if ((sroom = pick_room(TRUE)) != 0) {
		sroom->rtype = type;
		fill_zoo(sroom);
	   }
	}
	else */if ((sroom = pick_room(FALSE)) != 0) {
		sroom->rtype = type;
		fill_zoo(sroom);
	}
}

void
fill_zoo(sroom)
struct mkroom *sroom;
{
	struct monst *mon;
	struct monst *randomon;
	register struct obj *otmp;

	register int sx,sy,i;
	int sh, tx, ty, goldlim, type = sroom->rtype;
	int rmno = (sroom - rooms) + ROOMOFFSET;
	coord mm;
	int gravetries;

	int moreorless;

#ifdef GCC_WARN
	tx = ty = goldlim = 0;
#endif

	sh = sroom->fdoor;
	switch(type) {
	    case COURT:
	    case GIANTCOURT:
		if(level.flags.is_maze_lev) {
		    for(tx = sroom->lx; tx <= sroom->hx; tx++)
			for(ty = sroom->ly; ty <= sroom->hy; ty++)
			    if(IS_THRONE(levl[tx][ty].typ))
				goto throne_placed;
		}

		i = 100;
		do {	/* don't place throne on top of stairs */
			(void) somexy(sroom, &mm);
			tx = mm.x; ty = mm.y;
		} while (occupied((xchar)tx, (xchar)ty) && --i > 0);
	    throne_placed:
		/* TODO: try to ensure the enthroned monster is an M2_PRINCE */
		break;
	    case BEEHIVE:
	    case MIGOHIVE:
		tx = sroom->lx + (sroom->hx - sroom->lx + 1)/2;
		ty = sroom->ly + (sroom->hy - sroom->ly + 1)/2;
		if(sroom->irregular) {
		    /* center might not be valid, so put queen elsewhere */
		    if ((int) levl[tx][ty].roomno != rmno ||
			    levl[tx][ty].edge) {
			(void) somexy(sroom, &mm);
			tx = mm.x; ty = mm.y;
		    }
		}
		break;
	    case ZOO:
	    case LEPREHALL:
		case COINHALL:
		goldlim = 500 * level_difficulty();
		break;
		case CLINIC:
		case MIMICHALL:
		case ELEMHALL:
		case TERRORHALL:
		case ANGELHALL:
		case SPIDERHALL:
		case HUMANHALL:
		case GOLEMHALL:
		case TROLLHALL:
		case NYMPHHALL:
		case GRUEROOM:
		    break;
		case TENSHALL:
			u.tensionmonster = (rn2(187) + 1);
			u.tensionmonsteX = (rn2(100) + 1);
			u.tensionmonsterB = 0;
			u.tensionmonsterspec = 0;
			u.tensionmonsterspecB = 0;
			u.colormonster = 0;
			u.colormonsterB = 0;
			if (!rn2(10)) {u.colormonster = rnd(15);
				if (!rn2(4)) u.colormonsterB = rnd(15);

				if (u.colormonster == CLR_BLUE) { u.colormonster = 0; u.colormonsterB = 0;}
				if (u.colormonsterB == CLR_BLUE) u.colormonsterB = 0;
			}

			if (!rn2(4)) u.tensionmonsterB = (rn2(187) + 1);
			if (!rn2(10)) {u.tensionmonsterspec = rndmonst();
				if (!rn2(4)) u.tensionmonsterspecB = rndmonst();
			}
			break;
	    case DRAGONLAIR:
		goldlim = 1500 * level_difficulty();
		break;
	}

	moreorless = (rnd((ishaxor && !issuxxor) ? 20 : (issuxxor && !ishaxor) ? 5 : 10) + 1);

	if (sroom->ly == 20 && sroom->hy == 19) sroom->ly = sroom->hy = 20;
	if (sroom->ly == 1 && sroom->hy == 0) sroom->ly = sroom->hy = 0;

	for(sx = sroom->lx; sx <= sroom->hx; sx++)
	    for(sy = sroom->ly; sy <= sroom->hy; sy++) {
		if(sroom->irregular) {
		    if ((int) levl[sx][sy].roomno != rmno ||
			  (levl[sx][sy].edge) /*||
			  (sroom->doorct &&
			   distmin(sx, sy, doors[sh].x, doors[sh].y) <= 1)*/)
			continue;
		} else if(!SPACE_POS(levl[sx][sy].typ) /*||
			  (sroom->doorct &&
			   ((sx == sroom->lx && doors[sh].x == sx-1) ||
			    (sx == sroom->hx && doors[sh].x == sx+1) ||
			    (sy == sroom->ly && doors[sh].y == sy-1) ||
			    (sy == sroom->hy && doors[sh].y == sy+1)))*/)
		    continue;
		/* don't place monster on explicitly placed throne */
		if(type == COURT && IS_THRONE(levl[sx][sy].typ))
		    continue;
               /* armories don't contain as many monsters */
		if ( (type != ARMORY && rn2(moreorless) ) || rn2(2)) mon = makemon(
		    (type == COURT) ? (rn2(5) ? courtmon() : mkclass(S_ORC,0) ) :
		    (type == BARRACKS) ? squadmon() :
			(type == CLINIC) ? &mons[PM_NURSE] :
			(type == TERRORHALL) ? mkclass(S_UMBER,0) :
			(type == TENSHALL) ? (u.tensionmonsterspecB ? (rn2(2) ? u.tensionmonsterspecB : u.tensionmonsterspec ) : u.tensionmonsterspec ? u.tensionmonsterspec : u.colormonsterB ? (rn2(2) ? colormon(u.colormonsterB) : colormon(u.colormonster) ) : u.colormonster ? colormon(u.colormonster) : u.tensionmonsterB ? (rn2(2) ? tenshallmon() : tenshallmonB() ) : tenshallmon()) :
			(type == ELEMHALL) ? mkclass(S_ELEMENTAL,0) :
			(type == ANGELHALL) ? mkclass(S_ANGEL,0) :
			(type == MIMICHALL) ? mkclass(S_MIMIC,0) :
			(type == NYMPHHALL) ? mkclass(S_NYMPH,0) :
			(type == TROLLHALL) ? mkclass(S_TROLL,0) :
			(type == SPIDERHALL) ? mkclass(S_SPIDER,0) :
			(type == HUMANHALL) ? mkclass(S_HUMAN,0) :
			(type == GOLEMHALL) ? mkclass(S_GOLEM,0) :
			(type == COINHALL) ? mkclass(S_BAD_COINS,0) :
			(type == GRUEROOM) ? mkclass(S_GRUE,0) :
		    (type == MORGUE) ? morguemon() :
		    (type == FUNGUSFARM) ? (rn2(2) ? fungus() : mkclass(S_FUNGUS,0)) :
		    (type == BEEHIVE) ?
			(sx == tx && sy == ty ? &mons[PM_QUEEN_BEE] :
			 &mons[PM_KILLER_BEE]) :
		    (type == DOUGROOM) ? douglas_adams_mon() : 
		    (type == LEPREHALL) ? /*&mons[PM_LEPRECHAUN]*/mkclass(S_LEPRECHAUN,0) :
		    (type == COCKNEST) ? 
		    	/*(rn2(4) ? &mons[PM_COCKATRICE] :
		    	 &mons[PM_CHICKATRICE])*/mkclass(S_COCKATRICE,0) :
                   (type == ARMORY) ? (rn2(10) ? mkclass(S_RUSTMONST,0) :
			mkclass(S_PUDDING,0) ) :
		    (type == ANTHOLE) ? /*antholemon()*/mkclass(S_ANT,0) :
		    (type == DRAGONLAIR) ? mkclass(S_DRAGON,0) :
		    (type == LEMUREPIT)? 
		    	(!rn2(20)? &mons[PM_HORNED_DEVIL] : !rn2(20) ? mkclass(S_DEMON,0) : rn2(2) ? mkclass(S_IMP,0) :
			           &mons[PM_LEMURE]) :
		    (type == MIGOHIVE)?
		      (sx == tx && sy == ty? &mons[PM_MIGO_QUEEN] :
	              (rn2(2)? &mons[PM_MIGO_DRONE] : &mons[PM_MIGO_WARRIOR])) :
		    (type == BADFOODSHOP) ? mkclass(S_BAD_FOOD,0) :
		    (type == REALZOO) ? (rn2(5) ? realzoomon() : mkclass(S_QUADRUPED,0) ) :
		    (type == GIANTCOURT) ? mkclass(S_GIANT,0) :
		    (struct permonst *) 0,
		   sx, sy, NO_MM_FLAGS);
               else mon = ((struct monst *)0);
/* some rooms can spawn new monster variants now --Amy */
		if(mon) {
			if (rn2(10)) mon->msleeping = 1; /*random chance of them not being asleep --Amy*/
			if (/*type==COURT && */mon->mpeaceful) { /*enemies in these rooms will always be hostile now --Amy*/
				mon->mpeaceful = 0;
				set_malign(mon);
			}
		}
		switch(type) {
		    case ZOO:
		    case DRAGONLAIR:
		    case LEPREHALL:
			if(sroom->doorct)
			{
			    int distval = dist2(sx,sy,doors[sh].x,doors[sh].y);
			    i = sq(distval);
			}
			else
			    i = goldlim;
			if(i >= goldlim) i = 5*level_difficulty();
			goldlim -= i;
			(void) mkgold((long) rn1(i, 10), sx, sy);
			break;
		    case MORGUE:
			if(!rn2(5))
			    (void) mk_tt_object(CORPSE, sx, sy);
			if(!rn2(5) && (level_difficulty() > 10+rnd(200) )) { /* real player ghosts --Amy */
				coord mmm;
				mmm.x = sx;   
				mmm.y = sy;
			    (void) tt_mname(&mmm, FALSE, 0);
				}
			if(ishaxor && !rn2(5) && (level_difficulty() > 10+rnd(200) )) { /* real player ghosts --Amy */
				coord mmm;
				mmm.x = sx;   
				mmm.y = sy;
			    (void) tt_mname(&mmm, FALSE, 0);
				}
			if(!rn2(ishaxor ? 5 : 10))	/* lots of treasure buried with dead */
			    (void) mksobj_at((rn2(3)) ? LARGE_BOX : CHEST,
					     sx, sy, TRUE, FALSE);
			if (!rn2(5)) {
			    make_grave(sx, sy, (char *)0);

				if (!rn2(3)) (void) mkgold(0L, sx, sy);
				for (gravetries = rn2(5); gravetries; gravetries--) {
				    otmp = mkobj(RANDOM_CLASS, TRUE);
				    if (!otmp) return;
				    curse(otmp);
				    otmp->ox = sx;
				    otmp->oy = sy;
				    add_to_buried(otmp);
				}

			}
			break;
		    case BEEHIVE:
			if(!rn2(3))
			    (void) mksobj_at(LUMP_OF_ROYAL_JELLY,
					     sx, sy, TRUE, FALSE);
			break;
		    case FUNGUSFARM:
			if (!rn2(3))
			    (void) mksobj_at(SLIME_MOLD, sx, sy, TRUE, FALSE);
			break;
		    case MIGOHIVE:
			switch (rn2(10)) {
			    case 9:
				mksobj_at(DIAMOND, sx, sy, TRUE, FALSE);
				break;
			    case 8:
				mksobj_at(RUBY, sx, sy, TRUE, FALSE);
				break;
			    case 7:
			    case 6:
				mksobj_at(AGATE, sx, sy, TRUE, FALSE);
				break;
			    case 5:
			    case 4:
				mksobj_at(FLUORITE, sx, sy, TRUE, FALSE);
				break;
			    default:
				break;
			}
			break;
		    case BARRACKS:
			if(!rn2(ishaxor ? 10 : 20))	/* the payroll and some loot */
			    (void) mksobj_at((rn2(3)) ? LARGE_BOX : CHEST,
					     sx, sy, TRUE, FALSE);
			if (!rn2(5)) {
			    make_grave(sx, sy, (char *)0);

				if (!rn2(3)) (void) mkgold(0L, sx, sy);
				for (gravetries = rn2(5); gravetries; gravetries--) {
				    otmp = mkobj(RANDOM_CLASS, TRUE);
				    if (!otmp) return;
				    curse(otmp);
				    otmp->ox = sx;
				    otmp->oy = sy;
				    add_to_buried(otmp);
				}

			}
			break;

		    case CLINIC:
			if(!rn2(10))
			    (void) mksobj_at(ICE_BOX,sx,sy,TRUE,FALSE);
			break;
		    case GOLEMHALL:
			if(!rn2(ishaxor ? 10 : 20))
			    (void) mkobj_at(CHAIN_CLASS, sx, sy, FALSE);
			break;
		    case SPIDERHALL:
			if(!rn2(3))
			    (void) mksobj_at(EGG,sx,sy,TRUE,FALSE);
			break;
		    case COCKNEST:
			if(!rn2(3)) {
			    struct obj *sobj = mk_tt_object(STATUE, sx, sy);

			    if (sobj) {
				for (i = rn2(5); i; i--)
				    (void) add_to_container(sobj,
						mkobj(RANDOM_CLASS, FALSE));
				sobj->owt = weight(sobj);
			    }
			}
			break;
		    case ARMORY:
			{
				struct obj *otmp;
				if (!rn2(5)) { /* sorry Patrick, but the quantity of those items needs to be lower. --Amy */
					if (rn2(2))
						otmp = mkobj_at(WEAPON_CLASS, sx, sy, FALSE);
					else
						otmp = mkobj_at(ARMOR_CLASS, sx, sy, FALSE);
					otmp->spe = 0;
					if (is_rustprone(otmp)) otmp->oeroded = rn2(4);
					else if (is_rottable(otmp)) otmp->oeroded2 = rn2(4);
				}
			}
			break;
		    case ANTHOLE:
			if(!rn2(3))
			    (void) mkobj_at(FOOD_CLASS, sx, sy, FALSE);
			break;
		    case ANGELHALL:
			if(!rn2(10))
			    (void) mkobj_at(GEM_CLASS, sx, sy, FALSE);
			break;
		    case MIMICHALL:
		    case HUMANHALL:
			if(!rn2(3))
			    (void) mkobj_at(RANDOM_CLASS, sx, sy, FALSE);
			break;
		}
	    }
	switch (type) {
	      case COURT:
	      case GIANTCOURT:
		{
		  struct obj *chest;
		  levl[tx][ty].typ = THRONE;
		  (void) somexy(sroom, &mm);
		  (void) mkgold((long) rn1(50 * level_difficulty(),10), mm.x, mm.y);
		  /* the royal coffers */
		  chest = mksobj_at(CHEST, mm.x, mm.y, TRUE, FALSE);
		  chest->spe = 2; /* so it can be found later */
		  level.flags.has_court = 1;
		  break;
		}
	      case BARRACKS:
		  level.flags.has_barracks = 1;
		  break;
	      case REALZOO:              
	      case ZOO:
		  level.flags.has_zoo = 1;
		  break;
	      case MORGUE:
		  level.flags.has_morgue = 1;
		  break;
	      case SWAMP:
		  level.flags.has_swamp = 1;
		  break;
	      case BEEHIVE:
		  level.flags.has_beehive = 1;
		  break;
	      case DOUGROOM:
		  level.flags.has_zoo = 1;
		  break;
	      case LEMUREPIT:
		  level.flags.has_lemurepit = 1;
		  break;
	      case MIGOHIVE:
		  level.flags.has_migohive = 1;
		  break;
	      case FUNGUSFARM:
		  level.flags.has_fungusfarm = 1;
		  break;
            case CLINIC:
              level.flags.has_clinic = 1;
              break;
            case TERRORHALL:
              level.flags.has_terrorhall = 1;
              break;
            case INSIDEROOM:
              level.flags.has_insideroom = 1;
              break;
            case RIVERROOM:
              level.flags.has_riverroom = 1;
              break;
            case TENSHALL:
              level.flags.has_tenshall = 1;
              break;
            case ELEMHALL:
              level.flags.has_elemhall = 1;
              break;
            case ANGELHALL:
              level.flags.has_angelhall = 1;
              break;
            case MIMICHALL:
              level.flags.has_mimichall = 1;
              break;
            case NYMPHHALL:
              level.flags.has_nymphhall = 1;
              break;
            case SPIDERHALL:
              level.flags.has_spiderhall = 1;
              break;
            case TROLLHALL:
              level.flags.has_trollhall = 1;
              break;
            case COINHALL:
              level.flags.has_coinhall = 1;
              break;
            case HUMANHALL:
              level.flags.has_humanhall = 1;
              break;
            case GOLEMHALL:
              level.flags.has_golemhall = 1;
              break;
            case TRAPROOM:
              level.flags.has_traproom = 1;
              break;
            case GRUEROOM:
              level.flags.has_grueroom = 1;
              break;
            case POOLROOM:
              level.flags.has_poolroom = 1;
              break;
            case STATUEROOM:
              level.flags.has_statueroom = 1;
              break;
	}
}

/* make a swarm of undead around mm */
void
mkundead(mm, revive_corpses, mm_flags)
coord *mm;
boolean revive_corpses;
int mm_flags;
{
	int cnt = 1;
	if (!rn2(2)) cnt = (level_difficulty() + 1)/10;
	if (!rn2(5)) cnt += rnz(5);
	if (cnt < 1) cnt = 1;
	struct permonst *mdat;
	struct obj *otmp;
	coord cc;

	while (cnt--) {
	    mdat = morguemon();
	    if (enexto(&cc, mm->x, mm->y, mdat) &&
		    (!revive_corpses ||
		     !(otmp = sobj_at(CORPSE, cc.x, cc.y)) ||
		     !revive(otmp)))
		(void) makemon(mdat, cc.x, cc.y, mm_flags);
	}
	level.flags.graveyard = TRUE;	/* reduced chance for undead corpse */
}

/*void
mkundeadboo(mm, revive_corpses, mm_flags)
coord *mm;
boolean revive_corpses;
int mm_flags;
{
	struct monst *nmonst;

	int cnt = 1;
	if (!rn2(2)) cnt = (level_difficulty() + 1)/10;
	if (!rn2(5)) cnt += rnz(5);
	if (cnt < 1) cnt = 1;
	int mdat;
	struct obj *otmp;
	coord cc;

	while (cnt--) {
	    mdat = PM_UNDEAD_ARCHEOLOGIST + rn2(PM_UNDEAD_WIZARD - PM_UNDEAD_ARCHEOLOGIST + 1);
	    if (enexto(&cc, mm->x, mm->y, youmonst.data) &&
		    (!revive_corpses ||
		     !(otmp = sobj_at(CORPSE, cc.x, cc.y)) ||
		     !revive(otmp)))

		nmonst = makemon(&mons[mdat], cc.x, cc.y, mm_flags);
		tt_mname(nmonst);
	}
}*/

/* make a swarm of undead around mm but less, for zap.c WAN_SUMMON_UNDEAD */
void
mkundeadX(mm, revive_corpses, mm_flags)
coord *mm;
boolean revive_corpses;
int mm_flags;
{
	int cnt = 1;
	struct permonst *mdat;
	struct obj *otmp;
	coord cc;

	if (!rn2(10)) cnt += rnz(2);

	while (cnt--) {
	    mdat = morguemon();
	    if (enexto(&cc, mm->x, mm->y, mdat) &&
		    (!revive_corpses ||
		     !(otmp = sobj_at(CORPSE, cc.x, cc.y)) ||
		     !revive(otmp)))
		(void) makemon(mdat, cc.x, cc.y, mm_flags);
	}
	level.flags.graveyard = TRUE;	/* reduced chance for undead corpse */
}

STATIC_OVL struct permonst *
morguemon()
{
	register int i = rn2(100), hd = rn2(level_difficulty());

	if(hd > 10 && i < 10)
		return((Inhell || In_endgame(&u.uz)) ? mkclass(S_DEMON,0) :
						       &mons[ndemon(A_NONE)]);
	if(hd > 8 && i > 90)
		return(mkclass(S_VAMPIRE,0));

	return((i < 25) ? &mons[PM_GHOST]
			: (i < 30) ? mkclass(S_GHOST,0) : (i < 40) ? mkclass(S_WRAITH,0) : (i < 70) ? mkclass(S_MUMMY,0) : (i < 71) ? mkclass(S_LICH,0) : mkclass(S_ZOMBIE,0));
} /* added mummies, enabled all of S_wraith type monsters --Amy */

struct permonst *
antholemon()
{
	int mtyp;

	/* Same monsters within a level, different ones between levels */
	switch ((level_difficulty() + ((long)u.ubirthday)) % 4) {
	default:	mtyp = PM_GIANT_ANT; break;
	case 0:		mtyp = PM_SOLDIER_ANT; break;
	case 1:		mtyp = PM_FIRE_ANT; break;
	case 2:		mtyp = PM_SNOW_ANT; break;
	}
	return ((mvitals[mtyp].mvflags & G_GONE) ?
			(struct permonst *)0 : &mons[mtyp]);
}


STATIC_OVL struct permonst *
fungus()
{
	register int i, hd = level_difficulty(), mtyp = 0;

	i = rn2(hd > 20 ? 17 : hd > 12 ? 14 : 12);

	switch (i) {
	case 0:
	case 1: mtyp = PM_LICHEN; 		break;	
	case 2: mtyp = PM_BROWN_MOLD;		break;
	case 3: mtyp = PM_YELLOW_MOLD;		break;
	case 4: mtyp = PM_GREEN_MOLD;		break;
	case 5: mtyp = PM_RED_MOLD;		break;
	case 6: mtyp = PM_SHRIEKER;		break;
	case 7: mtyp = PM_VIOLET_FUNGUS;	break;
	case 8: mtyp = PM_BLUE_JELLY;		break;
	case 9: mtyp = PM_DISGUSTING_MOLD;	break;
	case 10: mtyp = PM_BLACK_MOLD;		break;
	case 11: mtyp = PM_GRAY_OOZE;		break;
	/* Following only after level 12... */
	case 12: mtyp = PM_SPOTTED_JELLY;	break;
	case 13: mtyp = PM_BROWN_PUDDING;	break;
	/* Following only after level 20... */
	case 14: mtyp = PM_GREEN_SLIME;		break;
	case 15: mtyp = PM_BLACK_PUDDING;	break;
	case 16: mtyp = PM_OCHRE_JELLY;		break;
	}

	return ((mvitals[mtyp].mvflags & G_GONE) ?
			(struct permonst *)0 : &mons[mtyp]);
}

STATIC_OVL void
mkswamp()	/* Michiel Huisjes & Fred de Wilde */
{
	register struct mkroom *sroom;
	register int sx,sy,i,eelct = 0;

	for(i=0; i<5; i++) {		/* turn up to 5 rooms swampy */
		sroom = &rooms[rn2(nroom)];
		if(sroom->hx < 0 || sroom->rtype != OROOM || (has_upstairs(sroom) && rn2(iswarper ? 10 : 100)) )
			continue;

		if (!rn2(3)) continue; /* allow some randomness --Amy */

		/* satisfied; make a swamp */
		sroom->rtype = SWAMP;

		if (sroom->ly == 20 && sroom->hy == 19) sroom->ly = sroom->hy = 20;
		if (sroom->ly == 1 && sroom->hy == 0) sroom->ly = sroom->hy = 0;

		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) /*&& !nexttodoor(sx,sy)*/) {
		    if((sx+sy)%2) {
			levl[sx][sy].typ = POOL;
			if(!eelct || !rn2(4)) {
			    /* mkclass() won't do, as we might get kraken */
/* comment by Amy - low-level players shouldn't move close to water anyway, so I will totally spawn everything here! */
			    (void) makemon(rn2(3) ? mkclass(S_EEL,0)
						  : rn2(5) ? &mons[PM_GIANT_EEL]
						  : rn2(2) ? &mons[PM_PIRANHA]
						  : &mons[PM_ELECTRIC_EEL],
						sx, sy, NO_MM_FLAGS);
			    eelct++;
			}
		    } else
			if(!rn2(4))	/* swamps tend to be moldy */
			    (void) makemon(mkclass(S_FUNGUS,0),
						sx, sy, NO_MM_FLAGS);
		}
		level.flags.has_swamp = 1;
	}
}

STATIC_OVL coord *
shrine_pos(roomno)
int roomno;
{
	static coord buf;
	struct mkroom *troom = &rooms[roomno - ROOMOFFSET];

	buf.x = troom->lx + ((troom->hx - troom->lx) / 2);
	buf.y = troom->ly + ((troom->hy - troom->ly) / 2);
	return(&buf);
}

STATIC_OVL void
mktemple()
{
	register struct mkroom *sroom;
	coord *shrine_spot;
	register struct rm *lev;

	if(!(sroom = pick_room(TRUE))) return;

	/* set up Priest and shrine */
	sroom->rtype = TEMPLE;
	/*
	 * In temples, shrines are blessed altars
	 * located in the center of the room
	 */
	shrine_spot = shrine_pos((sroom - rooms) + ROOMOFFSET);
	lev = &levl[shrine_spot->x][shrine_spot->y];
	lev->typ = ALTAR;
	lev->altarmask = induced_align(80);
	priestini(&u.uz, sroom, shrine_spot->x, shrine_spot->y, FALSE);
	lev->altarmask |= AM_SHRINE;
	level.flags.has_temple = 1;
}

boolean
nexttodoor(sx,sy)
register int sx, sy;
{
	register int dx, dy;
	register struct rm *lev;
	for(dx = -1; dx <= 1; dx++) for(dy = -1; dy <= 1; dy++) {
		if(!isok(sx+dx, sy+dy)) continue;
		if(IS_DOOR((lev = &levl[sx+dx][sy+dy])->typ) ||
		    lev->typ == SDOOR)
			return(TRUE);
	}
	return(FALSE);
}

boolean
has_dnstairs(sroom)
register struct mkroom *sroom;
{
	if (sroom == dnstairs_room)
		return TRUE;
	if (sstairs.sx && !sstairs.up)
		return((boolean)(sroom == sstairs_room));
	return FALSE;
}

boolean
has_upstairs(sroom)
register struct mkroom *sroom;
{
	if (sroom == upstairs_room)
		return TRUE;
	if (sstairs.sx && sstairs.up)
		return((boolean)(sroom == sstairs_room));
	return FALSE;
}

#endif /* OVLB */
#ifdef OVL0

int
somex(croom)
register struct mkroom *croom;
{
	return rn2(croom->hx-croom->lx+1) + croom->lx;
}

int
somey(croom)
register struct mkroom *croom;
{
	return rn2(croom->hy-croom->ly+1) + croom->ly;
}

boolean
inside_room(croom, x, y)
struct mkroom *croom;
xchar x, y;
{
	return((boolean)(x >= croom->lx-1 && x <= croom->hx+1 &&
		y >= croom->ly-1 && y <= croom->hy+1));
}

boolean
somexy(croom, c)
struct mkroom *croom;
coord *c;
{
	int try_cnt = 0;
	int i;

	if (croom->irregular) {
	    i = (croom - rooms) + ROOMOFFSET;

	    while(try_cnt++ < 100) {
		c->x = somex(croom);
		c->y = somey(croom);
		if (!levl[c->x][c->y].edge &&
			(int) levl[c->x][c->y].roomno == i)
		    return TRUE;
	    }
	    /* try harder; exhaustively search until one is found */
	    for(c->x = croom->lx; c->x <= croom->hx; c->x++)
		for(c->y = croom->ly; c->y <= croom->hy; c->y++)
		    if (!levl[c->x][c->y].edge &&
			    (int) levl[c->x][c->y].roomno == i)
			return TRUE;
	    return FALSE;
	}

	if (!croom->nsubrooms) {
		c->x = somex(croom);
		c->y = somey(croom);
		return TRUE;
	}

	/* Check that coords doesn't fall into a subroom or into a wall */

	while(try_cnt++ < 100) {
		c->x = somex(croom);
		c->y = somey(croom);
		if (IS_WALL(levl[c->x][c->y].typ))
		    continue;
		for(i=0 ; i<croom->nsubrooms;i++)
		    if(inside_room(croom->sbrooms[i], c->x, c->y))
			goto you_lose;
		break;
you_lose:	;
	}
	if (try_cnt >= 100)
	    return FALSE;
	return TRUE;
}

/*
 * Search for a special room given its type (zoo, court, etc...)
 *	Special values :
 *		- ANY_SHOP
 *		- ANY_TYPE
 */

struct mkroom *
search_special(type)
schar type;
{
	register struct mkroom *croom;

	for(croom = &rooms[0]; croom->hx >= 0; croom++)
	    if((type == ANY_TYPE && croom->rtype != OROOM) ||
	       (type == ANY_SHOP && croom->rtype >= SHOPBASE) ||
	       croom->rtype == type)
		return croom;
	for(croom = &subrooms[0]; croom->hx >= 0; croom++)
	    if((type == ANY_TYPE && croom->rtype != OROOM) ||
	       (type == ANY_SHOP && croom->rtype >= SHOPBASE) ||
	       croom->rtype == type)
		return croom;
	return (struct mkroom *) 0;
}

#endif /* OVL0 */
#ifdef OVLB

struct permonst *
courtmon()
{
	int     i = rnz(60) + rnz(3*level_difficulty());
	if (i > 200)            return(mkclass(S_DRAGON,0));
	else if (i > 130)       return(mkclass(S_GIANT,0));
	else if (i > 85)	return(mkclass(S_TROLL,0));
	else if (i > 75)	return(mkclass(S_CENTAUR,0));
	else if (i > 60)	return(mkclass(S_ORC,0));
	else if (i > 45)	return(&mons[PM_BUGBEAR]);
	else if (i > 30)	return(&mons[PM_HOBGOBLIN]);
	else if (i > 15)	return(mkclass(S_GNOME,0));
	else			return(mkclass(S_KOBOLD,0));
}

struct permonst *
colormon(color)
{

	register struct permonst *ptr;
	register int ct = 0;

	if (color == NO_COLOR) color = CLR_BLACK;

	ptr = rndmonst();

	do {

		ptr = rndmonst();
		ct++;

	} while (ptr->mcolor != color && ct < 200);

	return ptr;

}

struct permonst *
tenshallmon()
{
	if (u.tensionmonster < 6) return (mkclass(S_ANT,0));
	else if (u.tensionmonster < 9) return (mkclass(S_BLOB,0));
	else if (u.tensionmonster < 11) return (mkclass(S_COCKATRICE,0));
	else if (u.tensionmonster < 15) return (mkclass(S_DOG,0));
	else if (u.tensionmonster < 18) return (mkclass(S_EYE,0));
	else if (u.tensionmonster < 22) return (mkclass(S_FELINE,0));
	else if (u.tensionmonster < 24) return (mkclass(S_GREMLIN,0));
	else if (u.tensionmonster < 29) return (mkclass(S_HUMANOID,0));
	else if (u.tensionmonster < 33) return (mkclass(S_IMP,0));
	else if (u.tensionmonster < 36) return (mkclass(S_JELLY,0));
	else if (u.tensionmonster < 41) return (mkclass(S_KOBOLD,0));
	else if (u.tensionmonster < 44) return (mkclass(S_LEPRECHAUN,0));
	else if (u.tensionmonster < 47) return (mkclass(S_MIMIC,0));
	else if (u.tensionmonster < 50) return (mkclass(S_NYMPH,0));
	else if (u.tensionmonster < 54) return (mkclass(S_ORC,0));
	else if (u.tensionmonster < 55) return (mkclass(S_PIERCER,0));
	else if (u.tensionmonster < 58) return (mkclass(S_QUADRUPED,0));
	else if (u.tensionmonster < 62) return (mkclass(S_RODENT,0));
	else if (u.tensionmonster < 65) return (mkclass(S_SPIDER,0));
	else if (u.tensionmonster < 66) return (mkclass(S_TRAPPER,0));
	else if (u.tensionmonster < 69) return (mkclass(S_UNICORN,0));
	else if (u.tensionmonster < 71) return (mkclass(S_VORTEX,0));
	else if (u.tensionmonster < 73) return (mkclass(S_WORM,0));
	else if (u.tensionmonster < 75) return (mkclass(S_XAN,0));
	else if (u.tensionmonster < 76) return (mkclass(S_LIGHT,0));
	else if (u.tensionmonster < 77) return (mkclass(S_ZOUTHERN,0));
	else if (u.tensionmonster < 78) return (mkclass(S_ANGEL,0));
	else if (u.tensionmonster < 81) return (mkclass(S_BAT,0));
	else if (u.tensionmonster < 83) return (mkclass(S_CENTAUR,0));
	else if (u.tensionmonster < 86) return (mkclass(S_DRAGON,0));
	else if (u.tensionmonster < 89) return (mkclass(S_ELEMENTAL,0));
	else if (u.tensionmonster < 94) return (mkclass(S_FUNGUS,0));
	else if (u.tensionmonster < 99) return (mkclass(S_GNOME,0));
	else if (u.tensionmonster < 102) return (mkclass(S_GIANT,0));
	else if (u.tensionmonster < 103) return (mkclass(S_JABBERWOCK,0));
	else if (u.tensionmonster < 104) return (mkclass(S_KOP,0));
	else if (u.tensionmonster < 105) return (mkclass(S_LICH,0));
	else if (u.tensionmonster < 108) return (mkclass(S_MUMMY,0));
	else if (u.tensionmonster < 110) return (mkclass(S_NAGA,0));
	else if (u.tensionmonster < 113) return (mkclass(S_OGRE,0));
	else if (u.tensionmonster < 115) return (mkclass(S_PUDDING,0));
	else if (u.tensionmonster < 116) return (mkclass(S_QUANTMECH,0));
	else if (u.tensionmonster < 118) return (mkclass(S_RUSTMONST,0));
	else if (u.tensionmonster < 121) return (mkclass(S_SNAKE,0));
	else if (u.tensionmonster < 123) return (mkclass(S_TROLL,0));
	else if (u.tensionmonster < 124) return (mkclass(S_UMBER,0));
	else if (u.tensionmonster < 125) return (mkclass(S_VAMPIRE,0));
	else if (u.tensionmonster < 127) return (mkclass(S_WRAITH,0));
	else if (u.tensionmonster < 128) return (mkclass(S_XORN,0));
	else if (u.tensionmonster < 130) return (mkclass(S_YETI,0));
	else if (u.tensionmonster < 135) return (mkclass(S_ZOMBIE,0));
	else if (u.tensionmonster < 145) return (mkclass(S_HUMAN,0));
	else if (u.tensionmonster < 147) return (mkclass(S_GHOST,0));
	else if (u.tensionmonster < 149) return (mkclass(S_GOLEM,0));
	else if (u.tensionmonster < 152) return (mkclass(S_DEMON,0));
	else if (u.tensionmonster < 155) return (mkclass(S_EEL,0));
	else if (u.tensionmonster < 160) return (mkclass(S_LIZARD,0));
	else if (u.tensionmonster < 162) return (mkclass(S_BAD_FOOD,0));
	else if (u.tensionmonster < 165) return (mkclass(S_BAD_COINS,0));
	else if (u.tensionmonster < 166) {if (u.tensionmonsteX < 95) return (mkclass(S_HUMAN,0));
		else return (mkclass(S_NEMESE,0));
	}
	else if (u.tensionmonster < 171) return (mkclass(S_GRUE,0));
	else if (u.tensionmonster < 176) return (mkclass(S_WALLMONST,0));
	else if (u.tensionmonster < 180) return (mkclass(S_RUBMONST,0));
	else if (u.tensionmonster < 181) {if (u.tensionmonsteX < 99) return (mkclass(S_HUMAN,0));
		else return (mkclass(S_ARCHFIEND,0));
	}
	else if (u.tensionmonster < 186) return (mkclass(S_TURRET,0));
	else if (u.tensionmonster < 187) return (mkclass(S_FLYFISH,0));
	else return ((struct permonst*)0);
}

struct permonst *
tenshallmonB()
{
	if (u.tensionmonsterB < 6) return (mkclass(S_ANT,0));
	else if (u.tensionmonsterB < 9) return (mkclass(S_BLOB,0));
	else if (u.tensionmonsterB < 11) return (mkclass(S_COCKATRICE,0));
	else if (u.tensionmonsterB < 15) return (mkclass(S_DOG,0));
	else if (u.tensionmonsterB < 18) return (mkclass(S_EYE,0));
	else if (u.tensionmonsterB < 22) return (mkclass(S_FELINE,0));
	else if (u.tensionmonsterB < 24) return (mkclass(S_GREMLIN,0));
	else if (u.tensionmonsterB < 29) return (mkclass(S_HUMANOID,0));
	else if (u.tensionmonsterB < 33) return (mkclass(S_IMP,0));
	else if (u.tensionmonsterB < 36) return (mkclass(S_JELLY,0));
	else if (u.tensionmonsterB < 41) return (mkclass(S_KOBOLD,0));
	else if (u.tensionmonsterB < 44) return (mkclass(S_LEPRECHAUN,0));
	else if (u.tensionmonsterB < 47) return (mkclass(S_MIMIC,0));
	else if (u.tensionmonsterB < 50) return (mkclass(S_NYMPH,0));
	else if (u.tensionmonsterB < 54) return (mkclass(S_ORC,0));
	else if (u.tensionmonsterB < 55) return (mkclass(S_PIERCER,0));
	else if (u.tensionmonsterB < 58) return (mkclass(S_QUADRUPED,0));
	else if (u.tensionmonsterB < 62) return (mkclass(S_RODENT,0));
	else if (u.tensionmonsterB < 65) return (mkclass(S_SPIDER,0));
	else if (u.tensionmonsterB < 66) return (mkclass(S_TRAPPER,0));
	else if (u.tensionmonsterB < 69) return (mkclass(S_UNICORN,0));
	else if (u.tensionmonsterB < 71) return (mkclass(S_VORTEX,0));
	else if (u.tensionmonsterB < 73) return (mkclass(S_WORM,0));
	else if (u.tensionmonsterB < 75) return (mkclass(S_XAN,0));
	else if (u.tensionmonsterB < 76) return (mkclass(S_LIGHT,0));
	else if (u.tensionmonsterB < 77) return (mkclass(S_ZOUTHERN,0));
	else if (u.tensionmonsterB < 78) return (mkclass(S_ANGEL,0));
	else if (u.tensionmonsterB < 81) return (mkclass(S_BAT,0));
	else if (u.tensionmonsterB < 83) return (mkclass(S_CENTAUR,0));
	else if (u.tensionmonsterB < 86) return (mkclass(S_DRAGON,0));
	else if (u.tensionmonsterB < 89) return (mkclass(S_ELEMENTAL,0));
	else if (u.tensionmonsterB < 94) return (mkclass(S_FUNGUS,0));
	else if (u.tensionmonsterB < 99) return (mkclass(S_GNOME,0));
	else if (u.tensionmonsterB < 102) return (mkclass(S_GIANT,0));
	else if (u.tensionmonsterB < 103) return (mkclass(S_JABBERWOCK,0));
	else if (u.tensionmonsterB < 104) return (mkclass(S_KOP,0));
	else if (u.tensionmonsterB < 105) return (mkclass(S_LICH,0));
	else if (u.tensionmonsterB < 108) return (mkclass(S_MUMMY,0));
	else if (u.tensionmonsterB < 110) return (mkclass(S_NAGA,0));
	else if (u.tensionmonsterB < 113) return (mkclass(S_OGRE,0));
	else if (u.tensionmonsterB < 115) return (mkclass(S_PUDDING,0));
	else if (u.tensionmonsterB < 116) return (mkclass(S_QUANTMECH,0));
	else if (u.tensionmonsterB < 118) return (mkclass(S_RUSTMONST,0));
	else if (u.tensionmonsterB < 121) return (mkclass(S_SNAKE,0));
	else if (u.tensionmonsterB < 123) return (mkclass(S_TROLL,0));
	else if (u.tensionmonsterB < 124) return (mkclass(S_UMBER,0));
	else if (u.tensionmonsterB < 125) return (mkclass(S_VAMPIRE,0));
	else if (u.tensionmonsterB < 127) return (mkclass(S_WRAITH,0));
	else if (u.tensionmonsterB < 128) return (mkclass(S_XORN,0));
	else if (u.tensionmonsterB < 130) return (mkclass(S_YETI,0));
	else if (u.tensionmonsterB < 135) return (mkclass(S_ZOMBIE,0));
	else if (u.tensionmonsterB < 145) return (mkclass(S_HUMAN,0));
	else if (u.tensionmonsterB < 147) return (mkclass(S_GHOST,0));
	else if (u.tensionmonsterB < 149) return (mkclass(S_GOLEM,0));
	else if (u.tensionmonsterB < 152) return (mkclass(S_DEMON,0));
	else if (u.tensionmonsterB < 155) return (mkclass(S_EEL,0));
	else if (u.tensionmonsterB < 160) return (mkclass(S_LIZARD,0));
	else if (u.tensionmonsterB < 162) return (mkclass(S_BAD_FOOD,0));
	else if (u.tensionmonsterB < 165) return (mkclass(S_BAD_COINS,0));
	else if (u.tensionmonsterB < 166) {if (u.tensionmonsteX < 95) return (mkclass(S_HUMAN,0));
		else return (mkclass(S_NEMESE,0));
	}
	else if (u.tensionmonsterB < 171) return (mkclass(S_GRUE,0));
	else if (u.tensionmonsterB < 176) return (mkclass(S_WALLMONST,0));
	else if (u.tensionmonsterB < 180) return (mkclass(S_RUBMONST,0));
	else if (u.tensionmonsterB < 181) {if (u.tensionmonsteX < 99) return (mkclass(S_HUMAN,0));
		else return (mkclass(S_ARCHFIEND,0));
	}
	else if (u.tensionmonsterB < 186) return (mkclass(S_TURRET,0));
	else if (u.tensionmonsterB < 187) return (mkclass(S_FLYFISH,0));
	else return ((struct permonst*)0);
}

struct permonst *
douglas_adams_mon()
{
	int     i = rn2(60);
	if (i > 55) return(&mons[PM_RAVENOUS_BUGBLATTER_BEAST_OF_TRAAL]);
	else if (i > 54)        return(&mons[PM_MARVIN]);
	else if (i > 46)        return(&mons[PM_CREEPING___]);
	else if (i > 26)        return(&mons[PM_MICROSCOPIC_SPACE_FLEET]);
	else if (i > 20)        return(&mons[PM_VOGON]);
	else if (i > 19)        return(&mons[PM_VOGON_LORD]);
	else if (i > 2)        return(&mons[PM_BABELFISH]);
	else                    return(&mons[PM_ALGOLIAN_SUNTIGER]);
}

struct permonst *
realzoomon()
{
	int     i = rn2(60) + rn2(3*level_difficulty());
	if (i > 175)    return(&mons[PM_JUMBO_THE_ELEPHANT]);
	else if (i > 115)       return(&mons[PM_MASTODON]);
	else if (i > 85)        return(&mons[PM_PYTHON]);
	else if (i > 70)        return(&mons[PM_MUMAK]);
	else if (i > 55)        return(&mons[PM_TIGER]);
	else if (i > 45)        return(&mons[PM_PANTHER]);
	else if (i > 25)        return(&mons[PM_JAGUAR]);
	else if (i > 15)        return(&mons[PM_APE]);
	else                    return(&mons[PM_MONKEY]);
}

#define NSTYPES (PM_CAPTAIN - PM_SOLDIER + 1)

static struct {
    unsigned	pm;
    unsigned	prob;
} squadprob[NSTYPES] = {
    {PM_SOLDIER, 80}, {PM_SERGEANT, 15}, {PM_LIEUTENANT, 4}, {PM_CAPTAIN, 1}
};

STATIC_OVL struct permonst *
squadmon()		/* return soldier types. */
{
	int sel_prob, i, cpro, mndx;

	sel_prob = rnd(80+level_difficulty());

	cpro = 0;
	for (i = 0; i < NSTYPES; i++) {
	    cpro += squadprob[i].prob;
	    if (cpro > sel_prob) {
		mndx = squadprob[i].pm;
		goto gotone;
	    }
	}
	mndx = squadprob[rn2(NSTYPES)].pm;
gotone:
	if (!(mvitals[mndx].mvflags & G_GONE)) return(&mons[mndx]);
	else			    return((struct permonst *) 0);
}

/*
 * save_room : A recursive function that saves a room and its subrooms
 * (if any).
 */

STATIC_OVL void
save_room(fd, r)
int	fd;
struct mkroom *r;
{
	short i;
	/*
	 * Well, I really should write only useful information instead
	 * of writing the whole structure. That is I should not write
	 * the subrooms pointers, but who cares ?
	 */
	bwrite(fd, (genericptr_t) r, sizeof(struct mkroom));
	for(i=0; i<r->nsubrooms; i++)
	    save_room(fd, r->sbrooms[i]);
}

/*
 * save_rooms : Save all the rooms on disk!
 */

void
mktraproom()
{
    struct mkroom *sroom;

	register int sx,sy = 0;
	int rtrap;
	int randomnes = 0;

    if(!(sroom = pick_room(FALSE))) return;

    sroom->rtype = TRAPROOM;
	rtrap = randomtrap();

	if (!rn2(4)) randomnes = 1;

		if (sroom->ly == 20 && sroom->hy == 19) sroom->ly = sroom->hy = 20;
		if (sroom->ly == 1 && sroom->hy == 0) sroom->ly = sroom->hy = 0;

		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) /*&& !nexttodoor(sx,sy)*/) {
		    if(rn2(5)) 
				(void) maketrap(sx, sy, rtrap);
			if (randomnes == 1) rtrap = randomtrap();
		}

}

void
mkpoolroom()
{
    struct mkroom *sroom;
    schar typ;
	register int sx,sy = 0;

    if (!(sroom = pick_room(FALSE))) return;

	if(sroom->rtype != OROOM || (has_upstairs(sroom) && rn2(iswarper ? 10 : 100)) ) return;

    sroom->rtype = POOLROOM;
    typ = !rn2(3) ? POOL : !rn2(4) ? ICE : !rn2(5) ? CLOUD : !rn2(8) ? AIR : !rn2(10) ? STONE : !rn2(10) ? TREE : !rn2(15) ? IRONBARS : !rn2(120) ? FOUNTAIN : !rn2(250) ? THRONE : !rn2(60) ? SINK : !rn2(40) ? TOILET : !rn2(20) ? GRAVE : !rn2(500) ? ALTAR : LAVAPOOL;

		if (sroom->ly == 20 && sroom->hy == 19) sroom->ly = sroom->hy = 20;
		if (sroom->ly == 1 && sroom->hy == 0) sroom->ly = sroom->hy = 0;

		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) /*&& !nexttodoor(sx,sy)*/) {
		    if(rn2(5)) 
			levl[sx][sy].typ = typ;
		}

	level.flags.has_poolroom = 1;

}

void
mkinsideroom()
{
    struct mkroom *sroom;
    schar typ, typ2;
	register int sx,sy = 0;

    if (!(sroom = pick_room(FALSE))) return;

	if(sroom->rtype != OROOM || (has_upstairs(sroom) && rn2(iswarper ? 10 : 100)) ) return;

    sroom->rtype = INSIDEROOM;

		if (sroom->ly == 20 && sroom->hy == 19) sroom->ly = sroom->hy = 20;
		if (sroom->ly == 1 && sroom->hy == 0) sroom->ly = sroom->hy = 0;

		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) /*&& !nexttodoor(sx,sy)*/) {

    typ = !rn2(5) ? POOL : !rn2(5) ? ICE : !rn2(7) ? CLOUD : !rn2(8) ? AIR : !rn2(8) ? STONE : !rn2(8) ? TREE : !rn2(10) ? IRONBARS : !rn2(20) ? FOUNTAIN : !rn2(50) ? THRONE : !rn2(16) ? SINK : !rn2(12) ? TOILET : !rn2(6) ? GRAVE : !rn2(100) ? ALTAR : LAVAPOOL;

	typ2 = !rn2(7) ? TRAP_PERCENTS : !rn2(9) ? UNKNOWN_TRAP : !rn2(12) ? RMB_LOSS_TRAP : !rn2(12) ? WEAKNESS_TRAP : !rn2(10) ? DISPLAY_TRAP : !rn2(10) ? SPELL_LOSS_TRAP : !rn2(10) ? NUPESELL_TRAP : !rn2(9) ? CASTER_TRAP : !rn2(9) ? YELLOW_SPELL_TRAP : !rn2(9) ? SOUND_TRAP : !rn2(9) ? DSTW_TRAP : !rn2(9) ? STATUS_TRAP : !rn2(8) ? MENU_TRAP : !rn2(7) ? AUTO_DESTRUCT_TRAP : !rn2(7) ? DIFFICULTY_TRAP : !rn2(7) ? FREE_HAND_TRAP : !rn2(6) ? ROT_THIRTEEN_TRAP : !rn2(6) ? DROP_TRAP : !rn2(6) ? MEMORY_TRAP : !rn2(6) ? UNIDENTIFY_TRAP : !rn2(5) ? INVENTORY_TRAP : !rn2(5) ? THIRST_TRAP : !rn2(5) ? FAINT_TRAP : !rn2(4) ? CURSE_TRAP : !rn2(4) ? SHADES_OF_GREY_TRAP : !rn2(4) ? BISHOP_TRAP : !rn2(3) ? SPEED_TRAP : !rn2(3) ? CONFUSION_TRAP : !rn2(3) ? LUCK_TRAP : !rn2(2) ? BLACK_NG_WALL_TRAP : rn2(50) ? SUPERSCROLLER_TRAP : AUTOMATIC_SWITCHER;


		    if(rn2(3)) 
			levl[sx][sy].typ = typ;
			/*else*/ if (!rn2(10))			(void) maketrap(sx, sy, typ2);

			if (!rn2(1000)) 	(void) mksobj_at(SWITCHER, sx, sy, TRUE, FALSE);
		}

	level.flags.has_insideroom = 1;

}

void
mkriverroom()
{
    struct mkroom *sroom;
    schar typ;
	register int sx,sy = 0;

    if (!(sroom = pick_room(FALSE))) return;

	if(sroom->rtype != OROOM || (has_upstairs(sroom) && rn2(iswarper ? 10 : 100)) ) return;

    sroom->rtype = RIVERROOM;

		if (sroom->ly == 20 && sroom->hy == 19) sroom->ly = sroom->hy = 20;
		if (sroom->ly == 1 && sroom->hy == 0) sroom->ly = sroom->hy = 0;

		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) /*&& !nexttodoor(sx,sy)*/) {

	    typ = !rn2(3) ? POOL : !rn2(10) ? ICE : !rn2(10) ? FOUNTAIN : !rn2(3) ? STONE : !rn2(8) ? TREE : ROOM;

		levl[sx][sy].typ = typ;
		}

	level.flags.has_riverroom = 1;

}

void
mkstatueroom()
{
    struct mkroom *sroom;
    schar typ;
	register int sx,sy,i = 0;

    if (!(sroom = pick_room(FALSE))) return;

	if(sroom->rtype != OROOM || (has_upstairs(sroom) && rn2(iswarper ? 10 : 100)) ) return;

    sroom->rtype = STATUEROOM;

		if (sroom->ly == 20 && sroom->hy == 19) sroom->ly = sroom->hy = 20;
		if (sroom->ly == 1 && sroom->hy == 0) sroom->ly = sroom->hy = 0;

		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) /*&& !nexttodoor(sx,sy)*/) {
		    if(rn2(2)) 
				(void) maketrap(sx, sy, (rn2(10) ? STATUE_TRAP : ANIMATION_TRAP) );
		}

		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		    if(rn2(2)) 
			{
			    struct obj *sobj = mksobj_at(STATUE, sx, sy, TRUE, FALSE);

			    if (sobj) {
				for (i = rn2(5); i; i--)
				    (void) add_to_container(sobj,
						mkobj(RANDOM_CLASS, FALSE));
				sobj->owt = weight(sobj);
			    }
			}

	level.flags.has_statueroom = 1;

}

void
save_rooms(fd)
int fd;
{
	short i;

	/* First, write the number of rooms */
	bwrite(fd, (genericptr_t) &nroom, sizeof(nroom));
	for(i=0; i<nroom; i++)
	    save_room(fd, &rooms[i]);
}

STATIC_OVL void
rest_room(fd, r)
int fd;
struct mkroom *r;
{
	short i;

	mread(fd, (genericptr_t) r, sizeof(struct mkroom));
	for(i=0; i<r->nsubrooms; i++) {
		r->sbrooms[i] = &subrooms[nsubroom];
		rest_room(fd, &subrooms[nsubroom]);
		subrooms[nsubroom++].resident = (struct monst *)0;
	}
}

/*
 * rest_rooms : That's for restoring rooms. Read the rooms structure from
 * the disk.
 */

void
rest_rooms(fd)
int	fd;
{
	short i;

	mread(fd, (genericptr_t) &nroom, sizeof(nroom));
	nsubroom = 0;
	for(i = 0; i<nroom; i++) {
	    rest_room(fd, &rooms[i]);
	    rooms[i].resident = (struct monst *)0;
	}
	rooms[nroom].hx = -1;		/* restore ending flags */
	subrooms[nsubroom].hx = -1;
}
#endif /* OVLB */

/*mkroom.c*/
