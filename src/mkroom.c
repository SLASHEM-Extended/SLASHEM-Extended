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
STATIC_DCL boolean isbig(struct mkroom *);
STATIC_DCL struct mkroom * pick_room(BOOLEAN_P);
STATIC_DCL void mkshop(void), mkzoo(int), mkswamp(void);
STATIC_DCL void mktemple(void);
STATIC_DCL coord * shrine_pos(int);
STATIC_DCL struct permonst * morguemon(void);
STATIC_DCL struct permonst * douglas_adams_mon(void);
STATIC_DCL struct permonst * tenshallmon(void);
STATIC_DCL struct permonst * tenshallmonB(void);
STATIC_DCL struct permonst * squadmon(void);
STATIC_DCL struct permonst * doomsquadmon(void);
STATIC_DCL struct permonst * fungus(void);
STATIC_DCL struct permonst * beehivemon(void);
STATIC_DCL struct permonst * migohivemon(void);

/*STATIC_DCL struct permonst * colormon(int);*/

STATIC_DCL void mktraproom(void);
STATIC_DCL void mkpoolroom(void);
STATIC_DCL void mkbossroom(void);
STATIC_DCL void mknastycentral(void);
STATIC_DCL void mkemptydesert(void);
STATIC_DCL void mkstatueroom(void);
STATIC_DCL void mkinsideroom(void);
STATIC_DCL void mkriverroom(void);
STATIC_DCL void save_room(int,struct mkroom *);
STATIC_DCL void rest_room(int,struct mkroom *);
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
	case EMPTYNEST: mkzoo(EMPTYNEST); break;

	case CRYPTROOM: mkzoo(CRYPTROOM); break;
	case TROUBLEZONE: mkzoo(TROUBLEZONE); break;
	case WEAPONCHAMBER: mkzoo(WEAPONCHAMBER); break;
	case HELLPIT: mkzoo(HELLPIT); break;
	case FEMINISMROOM: mkzoo(FEMINISMROOM); break;
	case MEADOWROOM: mkzoo(MEADOWROOM); break;
	case COOLINGCHAMBER: mkzoo(COOLINGCHAMBER); break;
	case VOIDROOM: mkzoo(VOIDROOM); break;
	case HAMLETROOM: mkzoo(HAMLETROOM); break;
	case KOPSTATION: mkzoo(KOPSTATION); break;
	case BOSSROOM: mkbossroom(); break;
	case RNGCENTER: mkzoo(RNGCENTER); break;
	case WIZARDSDORM: mkzoo(WIZARDSDORM); break;
	case DOOMEDBARRACKS: mkzoo(DOOMEDBARRACKS); break;
	case SLEEPINGROOM: mkzoo(SLEEPINGROOM); break;
	case DIVERPARADISE: mkzoo(DIVERPARADISE); break;
	case MENAGERIE: mkzoo(MENAGERIE); break;
	case NASTYCENTRAL: mknastycentral(); break;
	case EMPTYDESERT: mkemptydesert(); break;
	case RARITYROOM: mkzoo(RARITYROOM); break;
	case EXHIBITROOM: mkzoo(EXHIBITROOM); break;
	case PRISONCHAMBER: mkzoo(PRISONCHAMBER); break;
	case NUCLEARCHAMBER: mkzoo(NUCLEARCHAMBER); break;
	case LEVELSEVENTYROOM: mkzoo(LEVELSEVENTYROOM); break;
	case VARIANTROOM: mkzoo(VARIANTROOM); break;


	case RANDOMROOM: {

retryrandtype:
		switch (rnd(63)) {

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
			case 37: mkzoo(EMPTYNEST); break;
			case 38: mkzoo(GRUEROOM); break;
			case 39: mkzoo(CRYPTROOM); break;
			case 40: mkzoo(TROUBLEZONE); break;
			case 41: mkzoo(WEAPONCHAMBER); break;
			case 42: mkzoo(HELLPIT); break;
			case 43: mkzoo(FEMINISMROOM); break;
			case 44: mkzoo(MEADOWROOM); break;
			case 45: mkzoo(COOLINGCHAMBER); break;
			case 46: mkzoo(VOIDROOM); break;
			case 47: mkzoo(HAMLETROOM); break;
			case 48: mkzoo(KOPSTATION); break;
			case 49: mkbossroom(); break;
			case 50: mkzoo(RNGCENTER); break;
			case 51: mkzoo(WIZARDSDORM); break;
			case 52: mkzoo(DOOMEDBARRACKS); break;
			case 53: mkzoo(SLEEPINGROOM); break;
			case 54: mkzoo(DIVERPARADISE); break;
			case 55: mkzoo(MENAGERIE); break;
			case 56: 
				if (!rn2(20)) {
					mknastycentral(); break;
				}
				else goto retryrandtype;
			case 57: mkemptydesert(); break;
			case 58: mkzoo(RARITYROOM); break;
			case 59: mkzoo(EXHIBITROOM); break;
			case 60: mkzoo(PRISONCHAMBER); break;
			case 61: mkzoo(NUCLEARCHAMBER); break;
			case 62: mkzoo(LEVELSEVENTYROOM); break;
			case 63: mkzoo(VARIANTROOM); break;

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
	boolean specialzoo = 0;
	int specialzoochance = 50;
	int specialzootype = 0;
	if (!rn2(10)) {
		specialzoo = 1; /* extra items! */
		specialzoochance = 50 + rnd(50);
		if (!rn2(5)) specialzoochance /= (1 + rnd(4));
		if (rn2(5)) specialzootype = 1 + rnd(16);
	}

	int sleepchance = 10;
	if (!rn2(10)) sleepchance -= rnd(10);

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
	    case PRISONCHAMBER:
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
		case VARIANTROOM:
			u.specialtensionmonster = !rn2(5) ? 341 : !rn2(4) ? 324 : !rn2(3) ? 325 : !rn2(2) ? 326 : 330;
			/* evil variant, angband, animeband, steamband or dnethack; one day elona shall be added */
		    break;
		case TENSHALL:
			u.tensionmonster = (rn2(187) + 1);
			u.tensionmonsteX = (rn2(100) + 1);
			u.tensionmonsterB = 0;
			u.tensionmonsterspec = 0;
			u.tensionmonsterspecB = 0;
			u.colormonster = 0;
			u.colormonsterB = 0;
			u.specialtensionmonster = 0;
			u.specialtensionmonsterB = 0;
			if (!rn2(8)) {u.colormonster = rnd(15);
				if (!rn2(4)) u.colormonsterB = rnd(15);

				if (u.colormonster == CLR_BLUE) { u.colormonster = 0; u.colormonsterB = 0;}
				if (u.colormonsterB == CLR_BLUE) u.colormonsterB = 0;
			}

			if (!rn2(4)) u.tensionmonsterB = (rn2(187) + 1);
			if (!rn2(10)) {u.tensionmonsterspec = rndmonst();
				if (!rn2(4)) u.tensionmonsterspecB = rndmonst();
			}
			if (!rn2(10)) {u.specialtensionmonster = rnd(341);
				if (!rn2(4)) u.specialtensionmonsterB = rnd(341);
			}
			break;
	    case DRAGONLAIR:
		goldlim = 1500 * level_difficulty();
		break;
	}

	moreorless = 100;
	if (rn2(10)) moreorless -= rnd(20);
	if (!rn2(3)) {
		moreorless -= rnd(30);
		if (!rn2(5)) moreorless -= rnd(40);
	}

	/* armories don't contain as many monsters; also, some other rooms and settings change the values */

	if (ishaxor && rn2(2)) moreorless = 100;
	if (type == VOIDROOM) {
		if (rn2(2)) moreorless /= 2;
		else moreorless /= 3;
	}
	if (type == ARMORY) moreorless /= 2;
	if (type == DIVERPARADISE) moreorless /= 5;
	if (type == LEVELSEVENTYROOM) moreorless /= 2;
	if (type == NUCLEARCHAMBER) moreorless /= 2;
	if (type == HAMLETROOM && moreorless > 5) moreorless = 5;
	if (issuxxor) moreorless /= 2;
	if (moreorless < 0) moreorless = 0;
	if (moreorless > 100) moreorless = 100;

	if (sroom->ly == 20 && sroom->hy == 19) sroom->ly = sroom->hy = 20;
	if (sroom->ly == 1 && sroom->hy == 0) sroom->ly = sroom->hy = 0;

	/* evil patch idea by Amy: 2% chance that special rooms are populated with much higher-leveled monsters */
	if (!rn2(50) || (type == VOIDROOM) ) {
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
	}

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
		if ( (rnd(100) <= moreorless) && (type != EMPTYNEST) ) mon = makemon(
		    (type == COURT) ? (rn2(5) ? courtmon() : mkclass(S_ORC,0) ) :

		    (type == INSIDEROOM) ? (/*!*/rn2(Role_if(PM_CAMPERSTRIKER) ? 20 : 40) ? insidemon() : (struct permonst *) 0 ) :

		    (type == BARRACKS) ? squadmon() :
		    (type == DOOMEDBARRACKS) ? doomsquadmon() :
			(type == CLINIC) ? specialtensmon(218) /* AD_HEAL */ :
			(type == TERRORHALL) ? mkclass(S_UMBER,0) :
			(type == VARIANTROOM) ? specialtensmon(u.specialtensionmonster) :
			(type == TENSHALL) ? (u.specialtensionmonsterB ? (rn2(2) ? specialtensmon(u.specialtensionmonsterB) : specialtensmon(u.specialtensionmonster) ) : u.specialtensionmonster ? specialtensmon(u.specialtensionmonster) : u.tensionmonsterspecB ? (rn2(2) ? u.tensionmonsterspecB : u.tensionmonsterspec ) : u.tensionmonsterspec ? u.tensionmonsterspec : u.colormonsterB ? (rn2(2) ? colormon(u.colormonsterB) : colormon(u.colormonster) ) : u.colormonster ? colormon(u.colormonster) : u.tensionmonsterB ? (rn2(2) ? tenshallmon() : tenshallmonB() ) : tenshallmon()) :
			(type == ELEMHALL) ? mkclass(S_ELEMENTAL,0) :
			(type == ANGELHALL) ? mkclass(S_ANGEL,0) :
			(type == FEMINISMROOM) ? (!rn2(20) ? specialtensmon(333) /* MS_STENCH */ : !rn2(3) ? specialtensmon(38) : !rn2(2) ? specialtensmon(39) : specialtensmon(40) ) /* MS_FART_foo */ :
			(type == MIMICHALL) ? mkclass(S_MIMIC,0) :
			(type == MEADOWROOM) ? (!rn2(10) ? mkclass(S_NYMPH,0) : !rn2(3) ? mkclass(S_ANT,0) : !rn2(2) ? mkclass(S_QUADRUPED,0) : mkclass(S_XAN,0)) :
			(type == SLEEPINGROOM) ? (!rn2(10) ? mkclass(S_OGRE,0) : !rn2(3) ? mkclass(S_KOBOLD,0) : !rn2(2) ? mkclass(S_RODENT,0) : mkclass(S_ORC,0)) :
			(type == HELLPIT) ? (!rn2(10) ? mkclass(S_ARCHFIEND,0) : (rnd(9) > 2) ? mkclass(S_DEMON,0) : specialtensmon(109) /* M2_DEMON */ ) :
			(type == MENAGERIE) ? (!rn2(8) ? mkclass(S_DOG,0) : !rn2(7) ? mkclass(S_FELINE,0) : !rn2(6) ? mkclass(S_RODENT,0) : !rn2(5) ? mkclass(S_UNICORN,0) : !rn2(4) ? mkclass(S_ZOUTHERN,0) : !rn2(3) ? mkclass(S_BAT,0) : !rn2(2) ? mkclass(S_SNAKE,0) : mkclass(S_YETI,0)) :
			(type == EXHIBITROOM) ? (!rn2(7) ? mkclass(S_EYE,0) : !rn2(6) ? mkclass(S_GREMLIN,0) : !rn2(5) ? mkclass(S_PIERCER,0) : !rn2(4) ? mkclass(S_TRAPPER,0) : !rn2(3) ? mkclass(S_WORM,0) : !rn2(2) ? mkclass(S_LIGHT,0) : mkclass(S_NAGA,0)) :
			(type == RARITYROOM) ? (!rn2(5) ? mkclass(S_QUANTMECH,0) : !rn2(4) ? mkclass(S_JABBERWOCK,0) : !rn2(3) ? mkclass(S_RUBMONST,0) : !rn2(2) ? mkclass(S_XORN,0) : mkclass(S_WALLMONST,0)) :
			(type == NYMPHHALL) ? mkclass(S_NYMPH,0) :
			(type == DIVERPARADISE) ? mkclass(S_EEL,0) :
			(type == WEAPONCHAMBER) ? specialtensmon(190) /* AT_WEAP */ :
			(type == TROUBLEZONE) ? (!rn2(10) ? mkclass(S_KOP,0) : !rn2(4) ? mkclass(S_TROLL,0) : !rn2(3) ? mkclass(S_UMBER,0) : !rn2(2) ? mkclass(S_DEMON,0) : mkclass(S_VORTEX,0)) :
			(type == TROLLHALL) ? mkclass(S_TROLL,0) :
			(type == SPIDERHALL) ? mkclass(S_SPIDER,0) :
			(type == COOLINGCHAMBER) ? (!rn2(3) ? specialtensmon(195) : !rn2(2) ? specialtensmon(243) : specialtensmon(299) ) /* AD_COLD, AD_FRZE and AD_ICEB */ :
			(type == HUMANHALL) ? mkclass(S_HUMAN,0) :
			(type == VOIDROOM) ? mkclass(S_GHOST,0) :
			(type == WIZARDSDORM) ? (!rn2(4) ? mkclass(S_DRAGON,0) : !rn2(3) ? specialtensmon(180) : !rn2(2) ? specialtensmon(236) : specialtensmon(313)) /* AT_SPIT, AD_TCKL, AD_CAST */ :
			(type == KOPSTATION) ? mkclass(S_KOP,0) :
			(type == RNGCENTER) ? (!rn2(2) ? specialtensmon(312) : specialtensmon(331) ) /* AD_RNG, M5_RANDOMIZED */ :
			(type == HAMLETROOM) ? mkclass(S_GIANT,0) :
			(type == GOLEMHALL) ? mkclass(S_GOLEM,0) :
			(type == CRYPTROOM) ? specialtensmon(95) /* M2_UNDEAD */ :
			(type == NUCLEARCHAMBER) ? specialtensmon(337) /* AD_CONT */ :
			(type == LEVELSEVENTYROOM) ? specialtensmon(0) /* any random monster */ :
			(type == COINHALL) ? mkclass(S_BAD_COINS,0) :
			(type == GRUEROOM) ? mkclass(S_GRUE,0) :
		    (type == MORGUE) ? morguemon() :
		    (type == FUNGUSFARM) ? (!rn2(4) ? mkclass(S_BLOB,0) : !rn2(3) ? mkclass(S_PUDDING,0) : !rn2(2) ? mkclass(S_JELLY,0) : mkclass(S_FUNGUS,0)) :
		    (type == BEEHIVE) ? (sx == tx && sy == ty ? &mons[PM_QUEEN_BEE] : beehivemon()) :
		    (type == PRISONCHAMBER) ? (sx == tx && sy == ty ? &mons[PM_PRISONER] : mkclass(S_OGRE,0) ) :
		    (type == DOUGROOM) ? douglas_adams_mon() : 
		    (type == LEPREHALL) ? mkclass(S_LEPRECHAUN,0) :
		    (type == COCKNEST) ? mkclass(S_COCKATRICE,0) :
                (type == ARMORY) ? (rn2(10) ? mkclass(S_RUSTMONST,0) : mkclass(S_PUDDING,0) ) :
		    (type == ANTHOLE) ? mkclass(S_ANT,0) :
		    (type == DRAGONLAIR) ? mkclass(S_DRAGON,0) :
		    (type == LEMUREPIT)? 
		    	(!rn2(20) ? &mons[PM_HORNED_DEVIL] : !rn2(20) ? mkclass(S_DEMON,0) : !rn2(50) ? &mons[ndemon(A_NONE)] : rn2(2) ? mkclass(S_IMP,0) : &mons[PM_LEMURE]) :
		    (type == MIGOHIVE) ? (sx == tx && sy == ty ? &mons[PM_MIGO_QUEEN] : migohivemon()) :
		    (type == BADFOODSHOP) ? mkclass(S_BAD_FOOD,0) :
		    (type == REALZOO) ? (rn2(5) ? realzoomon() : rn2(3) ? mkclass(S_QUADRUPED,0) : rn2(3) ? mkclass(S_FELINE,0) : rn2(3) ? mkclass(S_YETI,0) : mkclass(S_SNAKE,0) ) :
		    (type == GIANTCOURT) ? mkclass(S_GIANT,0) :
		    (struct permonst *) 0,
		   sx, sy, /*NO_MM_FLAGS*/MM_ADJACENTOK);
               else mon = ((struct monst *)0);
/* some rooms can spawn new monster variants now --Amy */
		if(mon) {
			if ((sleepchance > 1) && !issoviet && !(uarmf && uarmf->oartifact == ART_VERY_NICE_PERSON) && rn2(sleepchance)) mon->msleeping = 1; /*random chance of them not being asleep --Amy*/
		/* In Soviet Russia, monsters are always awake harharharharhar harharhar harhar! --Amy */
			if (/*type==COURT && */mon->mpeaceful) { /*enemies in these rooms will always be hostile now --Amy*/
				mon->mpeaceful = 0;
				set_malign(mon);
			}
		}
		switch(type) {

		    case CRYPTROOM:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(10)) {
				levl[sx][sy].typ = CRYPTFLOOR;
			}
			break;

		    case COOLINGCHAMBER:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(10)) {
				levl[sx][sy].typ = rn2(2) ? SNOW : ICE;
			}
			if(!rn2(10))
			    (void) mksobj_at(ICE_BOX,sx,sy,TRUE,FALSE);
			break;

		    case VOIDROOM:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(10)) {
				levl[sx][sy].typ = NETHERMIST;
			}
			if(!rn2(4) && !t_at(sx, sy))
				(void) maketrap(sx, sy, GIANT_CHASM, 100);
			break;

		    case RARITYROOM:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !rn2(5)) {
				levl[sx][sy].typ = randomwalltype();
			}
			break;

		    case DIVERPARADISE:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(10)) {
				levl[sx][sy].typ = rn2(4) ? WATERTUNNEL : MOAT;
			}
			if (!rn2(5)) (void) mkobj_at(RANDOM_CLASS, sx, sy, FALSE);
			break;

		    case WIZARDSDORM:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !rn2(4)) {
				levl[sx][sy].typ = rn2(2) ? WOODENTABLE : CARVEDBED;
			}
			break;

		    case SLEEPINGROOM:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !rn2(3)) {
				levl[sx][sy].typ = STRAWMATTRESS;
			}
			break;

		    case LEVELSEVENTYROOM:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(10)) {
				levl[sx][sy].typ = GRAVEWALL;
			}
			break;

		    case NUCLEARCHAMBER:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(5)) {
				levl[sx][sy].typ = STYXRIVER;
			}
			if(!rn2(2) && !t_at(sx, sy))
				(void) maketrap(sx, sy, CONTAMINATION_TRAP, 100);
			break;

		    case PRISONCHAMBER:
			if (sx == tx && sy == ty) {
				levl[sx][sy].typ = ALTAR;
				levl[sx][sy].altarmask = Align2amask( A_NONE );
			}
			break;

		    case KOPSTATION:
			if(!rn2(5) && !t_at(sx, sy))
				(void) maketrap(sx, sy, KOP_CUBE, 100);
			break;

		    case BOSSROOM:
			if(!rn2(5) && !t_at(sx, sy))
				(void) maketrap(sx, sy, BOSS_SPAWNER, 100);
			break;

		    case HAMLETROOM:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(10)) {
				levl[sx][sy].typ = rn2(10) ? GRASSLAND : !rn2(4) ? BURNINGWAGON : !rn2(3) ? WELL : rn2(2) ? POISONEDWELL : WAGON ;
			}
			if(!rn2(5) && !t_at(sx, sy)) {
				if (rn2(10)) (void) maketrap(sx, sy, STATUE_TRAP, 100);
				else if (rn2(20)) (void) maketrap(sx, sy, MONSTER_CUBE, 100);
				else (void) maketrap(sx, sy, WARP_ZONE, 100);
			}
			break;

		    case MEADOWROOM:
			if(rn2(5) && (levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR)) {
				levl[sx][sy].typ = GRASSLAND;
			}
			if(!rn2(5))
			    (void) mkobj_at(FOOD_CLASS, sx, sy, FALSE);

			break;

		    case HELLPIT:
			if(!rn2(2) && (levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR)) {
				levl[sx][sy].typ = LAVAPOOL;
			}
			if (!rn2(10) && !(t_at(sx,sy))) {
				(void) maketrap(sx, sy, FIRE_TRAP, 100);
			}
			break;

		    case ZOO:
			if (specialzoo) {
				if (specialzoochance > rnd(100)) {
				    (void) mkobj_at(specialzootype, sx, sy, TRUE);
				}
			}
			/* fall through */
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
				for (gravetries = rn2(2 + rn2(4)); gravetries; gravetries--) {
				    otmp = mkobj(rn2(3) ? COIN_CLASS : RANDOM_CLASS, TRUE);
				    if (!otmp) return;
				    curse(otmp);
				    otmp->ox = sx;
				    otmp->oy = sy;
				    add_to_buried(otmp);
				}

			}
			break;
		    case BEEHIVE:
			if(!rn2(5)) /* slightly lowered chance --Amy */
			    (void) mksobj_at(LUMP_OF_ROYAL_JELLY,
					     sx, sy, TRUE, FALSE);
			break;
		    case FUNGUSFARM:
			if (!rn2(25)) /* lowered chance --Amy */
			    (void) mksobj_at(SLIME_MOLD, sx, sy, TRUE, FALSE);
			break;
		    case MIGOHIVE:
			switch (rn2(30)) { /* greatly lowered chance --Amy */
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
				for (gravetries = rn2(2 + rn2(4)); gravetries; gravetries--) {
				    otmp = mkobj(rn2(3) ? COIN_CLASS : RANDOM_CLASS, TRUE);
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
		    case EMPTYNEST:
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
					if (otmp) {
						if (is_rustprone(otmp)) otmp->oeroded = rn2(4);
						else if (is_rottable(otmp)) otmp->oeroded2 = rn2(4);
					}
				}
			}
			break;
		    case WEAPONCHAMBER:
			{
				if (!rn2(2)) {
					if (rn2(10))
						(void) mkobj_at(WEAPON_CLASS, sx, sy, FALSE);
					else
						(void) mkobj_at(ARMOR_CLASS, sx, sy, FALSE);
				}
			}
			break;
		    case ANTHOLE:
			if(!rn2(15))
			    (void) mkobj_at(FOOD_CLASS, sx, sy, FALSE);
			break;
		    case ANGELHALL:
			if(!rn2(10))
			    (void) mkobj_at(GEM_CLASS, sx, sy, FALSE);
			break;
		    case MIMICHALL: /* lower overall amount of items --Amy */
			if(!rn2(10))
			    (void) mkobj_at(rn2(5) ? COIN_CLASS : RANDOM_CLASS, sx, sy, FALSE);
			break;
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
		  if (somexy(sroom, &mm)) {
			  (void) mkgold((long) rn1(50 * level_difficulty(),10), mm.x, mm.y);
			  /* the royal coffers */
			  chest = mksobj_at(CHEST, mm.x, mm.y, TRUE, FALSE);
			  if (chest) {
				  chest->spe = 2; /* so it can be found later */
			  }
		}

		  if (!rn2(20)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);
		  }

		  level.flags.has_court = 1;
		  break;
		}
	      case ARMORY:

		  if (!rn2(10)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);
		  }

		  break;

	      case BARRACKS:

		  if (!rn2(50)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);
		  }

		  level.flags.has_barracks = 1;
		  break;
	      case ZOO:
		  if (!rn2(50)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);
		  }
	      case REALZOO:              
		  level.flags.has_zoo = 1;
		  break;
	      case MORGUE:
		  if (!rn2(50)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);
		  }

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

		  if (!rn2(10)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);

			while (!rn2(2)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);
			}
		  }

              level.flags.has_insideroom = 1;
              break;
            case RIVERROOM:
		  if (!rn2(30)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);
		  }
              level.flags.has_riverroom = 1;
              break;
            case TENSHALL:
		  if (!rn2(50)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);
		  }
              level.flags.has_tenshall = 1;
              break;
            case ELEMHALL:
		  if (!rn2(50)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);
		  }
              level.flags.has_elemhall = 1;
              break;
            case ANGELHALL:
		  if (!rn2(20)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);
		  }
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
		  if (!rn2(50)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);
		  }
              level.flags.has_trollhall = 1;
              break;
            case COINHALL:
              level.flags.has_coinhall = 1;
              break;
            case HUMANHALL:
		  if (!rn2(30)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);
		  }
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

	    case CRYPTROOM:
		level.flags.has_cryptroom = 1;
		break;
	    case TROUBLEZONE:
		  if (!rn2(3)) {
			  if (somexy(sroom, &mm)) {
				if(levl[mm.x][mm.x].typ == ROOM || levl[mm.x][mm.x].typ == CORR) {
					levl[mm.x][mm.x].typ = PENTAGRAM;
				}
			}
		  }
		level.flags.has_troublezone = 1;
		break;
	    case WEAPONCHAMBER:
		level.flags.has_weaponchamber = 1;
		break;
	    case HELLPIT:
		  if (!rn2(10)) {
			  if (somexy(sroom, &mm)) {
				if(levl[mm.x][mm.x].typ == ROOM || levl[mm.x][mm.x].typ == CORR) {
					levl[mm.x][mm.x].typ = PENTAGRAM;
				}
			}
		  }
		level.flags.has_hellpit = 1;
		break;
	    case FEMINISMROOM:
		level.flags.has_feminismroom = 1;
		break;
	    case MEADOWROOM:
		level.flags.has_meadowroom = 1;
		break;
	    case COOLINGCHAMBER:
		level.flags.has_coolingchamber = 1;
		break;
	    case VOIDROOM:
		level.flags.has_voidroom = 1;
		break;
	    case HAMLETROOM:
		level.flags.has_hamletroom = 1;
		break;
	    case KOPSTATION:
		level.flags.has_kopstation = 1;
		break;
	    case BOSSROOM:
		level.flags.has_bossroom = 1;
		break;
	    case RNGCENTER:
		level.flags.has_rngcenter = 1;
		break;
	    case WIZARDSDORM:
		level.flags.has_wizardsdorm = 1;
		break;
	    case DOOMEDBARRACKS:
		level.flags.has_doomedbarracks = 1;
		break;
	    case SLEEPINGROOM:
		level.flags.has_sleepingroom = 1;
		break;
	    case DIVERPARADISE:
		level.flags.has_diverparadise = 1;
		break;
	    case MENAGERIE:
		level.flags.has_menagerie = 1;
		break;
	    case NASTYCENTRAL:
		level.flags.has_nastycentral = 1;
		break;
	    case EMPTYDESERT:
		level.flags.has_emptydesert = 1;
		break;
	    case RARITYROOM:
		level.flags.has_rarityroom = 1;
		break;
	    case EXHIBITROOM:
		level.flags.has_exhibitroom = 1;
		break;
	    case PRISONCHAMBER:
		level.flags.has_prisonchamber = 1;
		break;
	    case NUCLEARCHAMBER:
		level.flags.has_nuclearchamber = 1;
		break;
	    case LEVELSEVENTYROOM:
		level.flags.has_levelseventyroom = 1;
		break;
	    case VARIANTROOM:
		level.flags.has_variantroom = 1;
		break;

            case POOLROOM:
              level.flags.has_poolroom = 1;
              break;
            case STATUEROOM:
              level.flags.has_statueroom = 1;
              break;
	}

	u.aggravation = 0;

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

	if (Aggravate_monster) {
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
	}

	while (cnt--) {
	    mdat = morguemon();
	    if (enexto(&cc, mm->x, mm->y, mdat) &&
		    (!revive_corpses ||
		     !(otmp = sobj_at(CORPSE, cc.x, cc.y)) ||
		     !revive(otmp)))
		(void) makemon(mdat, cc.x, cc.y, mm_flags);
	}

	u.aggravation = 0;

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

	if (Aggravate_monster) {
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
	}

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

	u.aggravation = 0;
}

STATIC_OVL struct permonst *
morguemon()
{
	register int i = rn2(100), hd = rn2(level_difficulty());

	if(hd > 10 && i < 10)
		return(&mons[ndemon(A_NONE)]);
	if(hd > 8 && i > 90)
		return(mkclass(S_VAMPIRE,0));

	return((i < 30) ? mkclass(S_GHOST,0) : (i < 40) ? mkclass(S_WRAITH,0) : (i < 70) ? mkclass(S_MUMMY,0) : (i < 71) ? mkclass(S_LICH,0) : mkclass(S_ZOMBIE,0));
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

	/* There should be variation in the amount. --Amy */
	int ammount = rnd(5);

	for(i=0; i<ammount; i++) {		/* turn up to 5 rooms swampy */
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
			    (void) makemon(rn2(5) ? mkclass(S_EEL,0)
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
	if (i > 200)      return(mkclass(S_DRAGON,0));
	else if (i > 130) return(mkclass(S_GIANT,0));
	else if (i > 85)	return(mkclass(S_TROLL,0));
	else if (i > 75)	return(mkclass(S_JABBERWOCK,0));
	else if (i > 60)	return(mkclass(S_CENTAUR,0));
	else if (i > 45)	return(mkclass(S_ORC,0));
	else if (i > 30)	return(mkclass(S_HUMANOID,0));
	else if (i > 15)	return(mkclass(S_GNOME,0));
	else			return(mkclass(S_KOBOLD,0));
}

struct permonst *
insidemon()
{
	int     i = rnd(100);
	if (i > 99)       return(rn2(1000) ? &mons[PM_SUPERTHIEF] : (level_difficulty() < 40) ? &mons[PM_SUPERTHIEF] : &mons[PM__S_____NIX]);
	else if (i > 98)  return(rn2(200) ? &mons[PM_SUPERTHIEF] : (level_difficulty() < 20) ? &mons[PM_SUPERTHIEF] : &mons[PM_NIX]);
	else if (i > 96)	return((level_difficulty() < 5) ? &mons[PM_SUPERTHIEF] : &mons[PM_AK_THIEF_IS_DEAD_]);
	else if (i > 94)	return((level_difficulty() < 5) ? &mons[PM_SUPERTHIEF] : &mons[PM_UN_IN_PROTECT_MODE]);
	else if (i > 84)	return((level_difficulty() < 20) ? &mons[PM_SPACEWARS_FIGHTER] : &mons[PM_UNDEAD_SPACEWARS_FIGHTER]);
	else if (i > 74)	return(&mons[PM_CAR_DRIVING_SUPERTHIEF]);
	else if (i > 64)	return(&mons[PM_SUPERJEDI]);
	else if (i > 54)	return(&mons[PM_DIVISION_THIEF]);
	else if (i > 45)	return(&mons[PM_DIVISION_JEDI]);
	else if (i > 36)	return(&mons[PM_CRITICALLY_INJURED_THIEF]);
	else if (i > 27)	return(&mons[PM_CRITICALLY_INJURED_JEDI]);
	else if (i > 18)	return(&mons[PM_HUGE_OGRE_THIEF]);
	else if (i > 9)	return(&mons[PM_GUNNHILD_S_GENERAL_STORE]);
	else			return(&mons[PM_SUPERTHIEF]);
}

struct permonst *
colormon(color)
int color;
{

	register struct permonst *ptr;
	register int ct = 0;

	if (color == NO_COLOR) color = CLR_BLACK;

	ptr = rndmonst();

	do {

		ptr = rndmonst();
		ct++;

	} while (!ptr || (ptr->mcolor != color && ct < 200) );

	return ptr;

}

struct permonst *
specialtensmon(type) /* for monstercolor function and some others */
int type;
{

	register struct permonst *ptr;
	register int ct = 0;

	ptr = rndmonst();

	do {

		ptr = rndmonst();
		ct++;
		if (!rn2(2000)) reset_rndmonst(NON_PM);

	} while ( !ptr || (( (type == 1 && !(ptr->msound == MS_SILENT)) || (type == 2 && !(ptr->msound == MS_BARK)) || (type == 3 && !(ptr->msound == MS_MEW)) || (type == 4 && !(ptr->msound == MS_ROAR)) || (type == 5 && !(ptr->msound == MS_GROWL)) || (type == 6 && !(ptr->msound == MS_SQEEK)) || (type == 7 && !(ptr->msound == MS_SQAWK)) || (type == 8 && !(ptr->msound == MS_HISS)) || (type == 9 && !(ptr->msound == MS_BUZZ)) || (type == 10 && !(ptr->msound == MS_GRUNT)) || (type == 11 && !(ptr->msound == MS_NEIGH)) || (type == 12 && !(ptr->msound == MS_WAIL)) || (type == 13 && !(ptr->msound == MS_GURGLE)) || (type == 14 && !(ptr->msound == MS_BURBLE)) || (type == 15 && !(ptr->msound == MS_SHRIEK)) || (type == 16 && !(ptr->msound == MS_BONES)) || (type == 17 && !(ptr->msound == MS_LAUGH)) || (type == 18 && !(ptr->msound == MS_MUMBLE)) || (type == 19 && !(ptr->msound == MS_IMITATE)) || (type == 20 && !(ptr->msound == MS_ORC)) || (type == 21 && !(ptr->msound == MS_HUMANOID)) || (type == 22 && !(ptr->msound == MS_ARREST)) || (type == 23 && !(ptr->msound == MS_SOLDIER)) || (type == 24 && !(ptr->msound == MS_DJINNI)) || (type == 25 && !(ptr->msound == MS_NURSE)) || (type == 26 && !(ptr->msound == MS_SEDUCE)) || (type == 27 && !(ptr->msound == MS_VAMPIRE)) || (type == 28 && !(ptr->msound == MS_CUSS)) || (type == 29 && !(ptr->msound == MS_NEMESIS)) || (type == 30 && !(ptr->msound == MS_SPELL)) || (type == 31 && !(ptr->msound == MS_WERE)) || (type == 32 && !(ptr->msound == MS_BOAST)) || (type == 33 && !(ptr->msound == MS_SHEEP)) || (type == 34 && !(ptr->msound == MS_CHICKEN)) || (type == 35 && !(ptr->msound == MS_COW)) || (type == 36 && !(ptr->msound == MS_PARROT)) || (type == 37 && !(ptr->msound == MS_WHORE)) || (type == 38 && !(ptr->msound == MS_FART_QUIET)) || (type == 39 && !(ptr->msound == MS_FART_NORMAL)) || (type == 40 && !(ptr->msound == MS_FART_LOUD)) || (type == 41 && !(ptr->msound == MS_BOSS)) || (type == 42 && !(ptr->msound == MS_SOUND)) || (type == 43 && !(ptr->mresists & MR_FIRE)) || (type == 44 && !(ptr->mresists & MR_COLD)) || (type == 45 && !(ptr->mresists & MR_SLEEP)) || (type == 46 && !(ptr->mresists & MR_DISINT)) || (type == 47 && !(ptr->mresists & MR_ELEC)) || (type == 48 && !(ptr->mresists & MR_POISON)) || (type == 49 && !(ptr->mresists & MR_ACID)) || (type == 50 && !(ptr->mresists & MR_STONE)) || (type == 51 && !(ptr->mresists & MR_DEATH)) || (type == 52 && !(ptr->mresists & MR_DRAIN)) || (type == 53 && !(ptr->mresists & MR_PLUSONE)) || (type == 54 && !(ptr->mresists & MR_PLUSTWO)) || (type == 55 && !(ptr->mresists & MR_PLUSTHREE)) || (type == 56 && !(ptr->mresists & MR_PLUSFOUR)) || (type == 57 && !(ptr->mresists & MR_HITASONE)) || (type == 58 && !(ptr->mresists & MR_HITASTWO)) || (type == 59 && !(ptr->mresists & MR_HITASTHREE)) || (type == 60 && !(ptr->mresists & MR_HITASFOUR)) || (type == 61 && !(ptr->mflags1 & M1_FLY)) || (type == 62 && !(ptr->mflags1 & M1_SWIM)) || (type == 63 && !(ptr->mflags1 & M1_AMORPHOUS)) || (type == 64 && !(ptr->mflags1 & M1_WALLWALK)) || (type == 65 && !(ptr->mflags1 & M1_CLING)) || (type == 66 && !(ptr->mflags1 & M1_TUNNEL)) || (type == 67 && !(ptr->mflags1 & M1_NEEDPICK)) || (type == 68 && !(ptr->mflags1 & M1_CONCEAL)) || (type == 69 && !(ptr->mflags1 & M1_HIDE)) || (type == 70 && !(ptr->mflags1 & M1_AMPHIBIOUS)) || (type == 71 && !(ptr->mflags1 & M1_BREATHLESS)) || (type == 72 && !(ptr->mflags1 & M1_NOTAKE)) || (type == 73 && !(ptr->mflags1 & M1_NOEYES)) || (type == 74 && !(ptr->mflags1 & M1_NOHANDS)) || (type == 75 && !(ptr->mflags1 & M1_NOLIMBS)) || (type == 76 && !(ptr->mflags1 & M1_NOHEAD)) || (type == 77 && !(ptr->mflags1 & M1_MINDLESS)) || (type == 78 && !(ptr->mflags1 & M1_HUMANOID)) || (type == 79 && !(ptr->mflags1 & M1_ANIMAL)) || (type == 80 && !(ptr->mflags1 & M1_SLITHY)) || (type == 81 && !(ptr->mflags1 & M1_UNSOLID)) || (type == 82 && !(ptr->mflags1 & M1_THICK_HIDE)) || (type == 83 && !(ptr->mflags1 & M1_OVIPAROUS)) || (type == 84 && !(ptr->mflags1 & M1_REGEN)) || (type == 85 && !(ptr->mflags1 & M1_SEE_INVIS)) || (type == 86 && !(ptr->mflags1 & M1_TPORT)) || (type == 87 && !(ptr->mflags1 & M1_TPORT_CNTRL)) || (type == 88 && !(ptr->mflags1 & M1_ACID)) || (type == 89 && !(ptr->mflags1 & M1_POIS)) || (type == 90 && !(ptr->mflags1 & M1_CARNIVORE)) || (type == 91 && !(ptr->mflags1 & M1_HERBIVORE)) || (type == 92 && !(ptr->mflags1 & M1_OMNIVORE)) || (type == 93 && !(ptr->mflags1 & M1_METALLIVORE)) || (type == 94 && !(ptr->mflags2 & M2_NOPOLY)) || (type == 95 && !(ptr->mflags2 & M2_UNDEAD)) || (type == 96 && !(ptr->mflags2 & M2_MERC)) || (type == 97 && !(ptr->mflags2 & M2_HUMAN)) || (type == 98 && !(ptr->mflags2 & M2_ELF)) || (type == 99 && !(ptr->mflags2 & M2_DWARF)) || (type == 100 && !(ptr->mflags2 & M2_GNOME)) || (type == 101 && !(ptr->mflags2 & M2_ORC)) || (type == 102 && !(ptr->mflags2 & M2_HOBBIT)) || (type == 103 && !(ptr->mflags2 & M2_WERE)) || (type == 104 && !(ptr->mflags2 & M2_VAMPIRE)) || (type == 105 && !(ptr->mflags2 & M2_LORD)) || (type == 106 && !(ptr->mflags2 & M2_PRINCE)) || (type == 107 && !(ptr->mflags2 & M2_MINION)) || (type == 108 && !(ptr->mflags2 & M2_GIANT)) || (type == 109 && !(ptr->mflags2 & M2_DEMON)) || (type == 110 && !(ptr->mflags2 & M2_MALE)) || (type == 111 && !(ptr->mflags2 & M2_FEMALE)) || (type == 112 && !(ptr->mflags2 & M2_NEUTER)) || (type == 113 && !(ptr->mflags2 & M2_PNAME)) || (type == 114 && !(ptr->mflags2 & M2_HOSTILE)) || (type == 115 && !(ptr->mflags2 & M2_PEACEFUL)) || (type == 116 && !(ptr->mflags2 & M2_DOMESTIC)) || (type == 117 && !(ptr->mflags2 & M2_WANDER)) || (type == 118 && !(ptr->mflags2 & M2_STALK)) || (type == 119 && !(ptr->mflags2 & M2_NASTY)) || (type == 120 && !(ptr->mflags2 & M2_STRONG)) || (type == 121 && !(ptr->mflags2 & M2_ROCKTHROW)) || (type == 122 && !(ptr->mflags2 & M2_GREEDY))  || (type == 123 && !(ptr->mflags2 & M2_JEWELS)) || (type == 124 && !(ptr->mflags2 & M2_COLLECT)) || (type == 125 && !(ptr->mflags2 & M2_MAGIC)) || (type == 126 && !(ptr->mflags3 & M3_WANTSAMUL)) || (type == 127 && !(ptr->mflags3 & M3_WANTSBELL)) || (type == 128 && !(ptr->mflags3 & M3_WANTSBOOK)) || (type == 129 && !(ptr->mflags3 & M3_WANTSCAND)) || (type == 130 && !(ptr->mflags3 & M3_WANTSARTI)) || (type == 131 && !(ptr->mflags3 & M3_WAITFORU)) || (type == 132 && !(ptr->mflags3 & M3_CLOSE)) || (type == 133 && !(ptr->mflags3 & M3_INFRAVISION)) || (type == 134 && !(ptr->mflags3 & M3_INFRAVISIBLE)) || (type == 135 && !(ptr->mflags3 & M3_TRAITOR)) || (type == 136 && !(ptr->mflags3 & M3_NOTAME)) || (type == 137 && !(ptr->mflags3 & M3_AVOIDER)) || (type == 138 && !(ptr->mflags3 & M3_LITHIVORE)) || (type == 139 && !(ptr->mflags3 & M3_PETTY)) || (type == 140 && !(ptr->mflags3 & M3_POKEMON)) || (type == 141 && !(ptr->mflags3 & M3_NOPLRPOLY)) || (type == 142 && !(ptr->mflags3 & M3_NONMOVING)) || (type == 143 && !(ptr->mflags3 & M3_EGOTYPE)) || (type == 144 && !(ptr->mflags3 & M3_TELEPATHIC)) || (type == 145 && !(ptr->mflags3 & M3_SPIDER)) || (type == 146 && !(ptr->mflags3 & M3_PETRIFIES)) || (type == 147 && !(ptr->mflags3 & M3_IS_MIND_FLAYER)) || (type == 148 && !(ptr->mflags3 & M3_NO_DECAY)) || (type == 149 && !(ptr->mflags3 & M3_MIMIC)) || (type == 150 && !(ptr->mflags3 & M3_PERMAMIMIC)) || (type == 151 && !(ptr->mflags3 & M3_SLIME)) || (type == 152 && !(ptr->mflags3 & M3_FREQ_UNCOMMON2)) || (type == 153 && !(ptr->mflags3 & M3_FREQ_UNCOMMON3)) || (type == 154 && !(ptr->mflags3 & M3_FREQ_UNCOMMON5)) || (type == 155 && !(ptr->mflags3 & M3_FREQ_UNCOMMON7)) || (type == 156 && !(ptr->mflags3 & M3_FREQ_UNCOMMON10)) || (type == 157 && !(ptr->mflags3 & M3_MIND_FLAYER)) || (type == 158 && !(ptr->msize == MZ_TINY)) || (type == 159 && !(ptr->msize == MZ_SMALL)) || (type == 160 && !(ptr->msize == MZ_MEDIUM)) || (type == 161 && !(ptr->msize == MZ_LARGE)) || (type == 162 && !(ptr->msize == MZ_HUGE)) || (type == 163 && !(ptr->msize == MZ_GIGANTIC)) || (type == 164 && !(ptr->geno & G_VLGROUP)) || (type == 165 && !(ptr->geno & G_UNIQ)) || (type == 166 && !(ptr->geno & G_SGROUP)) || (type == 167 && !(ptr->geno & G_LGROUP)) || (type == 168 && !(ptr->geno & G_GENO)) || (type == 169 && !(ptr->geno & G_NOCORPSE)) || (type == 170 && !attacktype(ptr, AT_NONE)) || (type == 171 && !attacktype(ptr, AT_CLAW)) || (type == 172 && !attacktype(ptr, AT_BITE)) || (type == 173 && !attacktype(ptr, AT_KICK)) || (type == 174 && !attacktype(ptr, AT_BUTT)) || (type == 175 && !attacktype(ptr, AT_TUCH)) || (type == 176 && !attacktype(ptr, AT_STNG)) || (type == 177 && !attacktype(ptr, AT_HUGS)) || (type == 178 && !attacktype(ptr, AT_SCRA)) || (type == 179 && !attacktype(ptr, AT_LASH)) || (type == 180 && !attacktype(ptr, AT_SPIT)) || (type == 181 && !attacktype(ptr, AT_ENGL)) || (type == 182 && !attacktype(ptr, AT_BREA)) || (type == 183 && !attacktype(ptr, AT_EXPL)) || (type == 184 && !attacktype(ptr, AT_BOOM)) || (type == 185 && !attacktype(ptr, AT_GAZE)) || (type == 186 && !attacktype(ptr, AT_TENT)) || (type == 187 && !attacktype(ptr, AT_TRAM)) || (type == 188 && !attacktype(ptr, AT_BEAM)) || (type == 189 && !attacktype(ptr, AT_MULTIPLY)) || (type == 190 && !attacktype(ptr, AT_WEAP)) || (type == 191 && !attacktype(ptr, AT_MAGC)) || (type == 192 && !dmgtype(ptr, AD_PHYS)) || (type == 193 && !dmgtype(ptr, AD_MAGM)) || (type == 194 && !dmgtype(ptr, AD_FIRE)) || (type == 195 && !dmgtype(ptr, AD_COLD)) || (type == 196 && !dmgtype(ptr, AD_SLEE)) || (type == 197 && !dmgtype(ptr, AD_DISN)) || (type == 198 && !dmgtype(ptr, AD_ELEC)) || (type == 199 && !dmgtype(ptr, AD_DRST)) || (type == 200 && !dmgtype(ptr, AD_ACID)) || (type == 201 && !dmgtype(ptr, AD_LITE)) || (type == 202 && !dmgtype(ptr, AD_BLND)) || (type == 203 && !dmgtype(ptr, AD_STUN)) || (type == 204 && !dmgtype(ptr, AD_SLOW)) || (type == 205 && !dmgtype(ptr, AD_PLYS)) || (type == 206 && !dmgtype(ptr, AD_DRLI)) || (type == 207 && !dmgtype(ptr, AD_DREN)) || (type == 208 && !dmgtype(ptr, AD_LEGS)) || (type == 209 && !dmgtype(ptr, AD_STON)) || (type == 210 && !dmgtype(ptr, AD_STCK)) || (type == 211 && !dmgtype(ptr, AD_SGLD)) || (type == 212 && !dmgtype(ptr, AD_SITM)) || (type == 213 && !dmgtype(ptr, AD_SEDU)) || (type == 214 && !dmgtype(ptr, AD_TLPT)) || (type == 215 && !dmgtype(ptr, AD_RUST)) || (type == 216 && !dmgtype(ptr, AD_CONF)) || (type == 217 && !dmgtype(ptr, AD_DGST)) || (type == 218 && !dmgtype(ptr, AD_HEAL)) || (type == 219 && !dmgtype(ptr, AD_WRAP)) || (type == 220 && !dmgtype(ptr, AD_WERE)) || (type == 221 && !dmgtype(ptr, AD_DRDX)) || (type == 222 && !dmgtype(ptr, AD_DRCO)) || (type == 223 && !dmgtype(ptr, AD_DRIN)) || (type == 224 && !dmgtype(ptr, AD_DISE)) || (type == 225 && !dmgtype(ptr, AD_DCAY)) || (type == 226 && !dmgtype(ptr, AD_SSEX)) || (type == 227 && !dmgtype(ptr, AD_HALU)) || (type == 228 && !dmgtype(ptr, AD_DETH)) || (type == 229 && !dmgtype(ptr, AD_PEST)) || (type == 230 && !dmgtype(ptr, AD_FAMN)) || (type == 231 && !dmgtype(ptr, AD_SLIM)) || (type == 232 && !dmgtype(ptr, AD_CALM)) || (type == 233 && !dmgtype(ptr, AD_ENCH)) || (type == 234 && !dmgtype(ptr, AD_POLY)) || (type == 235 && !dmgtype(ptr, AD_CORR)) || (type == 236 && !dmgtype(ptr, AD_TCKL)) || (type == 237 && !dmgtype(ptr, AD_NGRA)) || (type == 238 && !dmgtype(ptr, AD_GLIB)) || (type == 239 && !dmgtype(ptr, AD_DARK)) || (type == 240 && !dmgtype(ptr, AD_WTHR)) || (type == 241 && !dmgtype(ptr, AD_LUCK)) || (type == 242 && !dmgtype(ptr, AD_NUMB)) || (type == 243 && !dmgtype(ptr, AD_FRZE)) || (type == 244 && !dmgtype(ptr, AD_DISP)) || (type == 245 && !dmgtype(ptr, AD_BURN)) || (type == 246 && !dmgtype(ptr, AD_FEAR)) || (type == 247 && !dmgtype(ptr, AD_NPRO)) || (type == 248 && !dmgtype(ptr, AD_POIS)) || (type == 249 && !dmgtype(ptr, AD_THIR)) || (type == 250 && !dmgtype(ptr, AD_LAVA)) || (type == 251 && !dmgtype(ptr, AD_FAKE)) || (type == 252 && !dmgtype(ptr, AD_LETH)) || (type == 253 && !dmgtype(ptr, AD_CNCL)) || (type == 254 && !dmgtype(ptr, AD_BANI)) || (type == 255 && !dmgtype(ptr, AD_WISD)) || (type == 256 && !dmgtype(ptr, AD_SHRD)) || (type == 257 && !dmgtype(ptr, AD_WET)) || (type == 258 && !dmgtype(ptr, AD_SUCK)) || (type == 259 && !dmgtype(ptr, AD_MALK)) || (type == 260 && !dmgtype(ptr, AD_UVUU)) || (type == 261 && !dmgtype(ptr, AD_ABDC)) || (type == 262 && !dmgtype(ptr, AD_AXUS)) || (type == 263 && !dmgtype(ptr, AD_CHKH)) || (type == 264 && !dmgtype(ptr, AD_HODS)) || (type == 265 && !dmgtype(ptr, AD_CHRN)) || (type == 266 && !dmgtype(ptr, AD_WEEP)) || (type == 267 && !dmgtype(ptr, AD_VAMP)) || (type == 268 && !dmgtype(ptr, AD_WEBS)) || (type == 269 && !dmgtype(ptr, AD_STTP)) || (type == 270 && !dmgtype(ptr, AD_DEPR)) || (type == 271 && !dmgtype(ptr, AD_WRAT)) || (type == 272 && !dmgtype(ptr, AD_LAZY)) || (type == 273 && !dmgtype(ptr, AD_DRCH)) || (type == 274 && !dmgtype(ptr, AD_DFOO)) || (type == 275 && !dmgtype(ptr, AD_NEXU)) || (type == 276 && !dmgtype(ptr, AD_SOUN)) || (type == 277 && !dmgtype(ptr, AD_GRAV)) || (type == 278 && !dmgtype(ptr, AD_INER)) || (type == 279 && !dmgtype(ptr, AD_TIME)) || (type == 280 && !dmgtype(ptr, AD_MANA)) || (type == 281 && !dmgtype(ptr, AD_PLAS)) || (type == 282 && !dmgtype(ptr, AD_SKIL)) || (type == 283 && !dmgtype(ptr, AD_CLRC)) || (type == 284 && !dmgtype(ptr, AD_SPEL)) || (type == 285 && !dmgtype(ptr, AD_RBRE)) || (type == 286 && !dmgtype(ptr, AD_SAMU)) || (type == 287 && !dmgtype(ptr, AD_CURS)) || (type == 288 && !dmgtype(ptr, AD_SPC2)) || (type == 289 && !dmgtype(ptr, AD_VENO)) || (type == 290 && !dmgtype(ptr, AD_DREA)) || (type == 291 && !dmgtype(ptr, AD_NAST)) || (type == 292 && !dmgtype(ptr, AD_BADE)) || (type == 293 && !dmgtype(ptr, AD_SLUD)) || (type == 294 && !dmgtype(ptr, AD_ICUR)) || (type == 295 && !dmgtype(ptr, AD_VULN)) || (type == 296 && !dmgtype(ptr, AD_FUMB)) || (type == 297 && !dmgtype(ptr, AD_DIMN)) || (type == 298 && !dmgtype(ptr, AD_AMNE)) || (type == 299 && !dmgtype(ptr, AD_ICEB)) || (type == 300 && !dmgtype(ptr, AD_VAPO)) || (type == 301 && !dmgtype(ptr, AD_EDGE)) || (type == 302 && !dmgtype(ptr, AD_VOMT)) || (type == 303 && !dmgtype(ptr, AD_LITT)) || (type == 304 && !dmgtype(ptr, AD_FREN)) || (type == 305 && !dmgtype(ptr, AD_NGEN)) || (type == 306 && !dmgtype(ptr, AD_CHAO)) || (type == 307 && !dmgtype(ptr, AD_INSA)) || (type == 308 && !dmgtype(ptr, AD_TRAP)) || (type == 309 && !dmgtype(ptr, AD_WGHT)) || (type == 310 && !dmgtype(ptr, AD_NTHR)) || (type == 311 && !dmgtype(ptr, AD_MIDI)) || (type == 312 && !dmgtype(ptr, AD_RNG)) || (type == 313 && !dmgtype(ptr, AD_CAST)) || (type == 314 && !(ptr->mflags4 & M4_BAT)) || (type == 315 && !(ptr->mflags4 & M4_REVIVE)) || (type == 316 && !(ptr->mflags4 & M4_RAT)) || (type == 317 && !(ptr->mflags4 & M4_SHADE)) || (type == 318 && !(ptr->mflags4 & M4_REFLECT)) || (type == 319 && !(ptr->mflags4 & M4_MULTIHUED)) || (type == 320 && !(ptr->mflags4 & M4_TAME)) || (type == 321 && !(ptr->mflags4 & M4_ORGANIVORE)) || (type == 322 && !(ptr->mflags5 & M5_SPACEWARS)) || (type == 323 && !(ptr->mflags5 & M5_JOKE)) || (type == 324 && !(ptr->mflags5 & M5_ANGBAND)) || (type == 325 && !(ptr->mflags5 & M5_STEAMBAND)) || (type == 326 && !(ptr->mflags5 & M5_ANIMEBAND)) || (type == 327 && !(ptr->mflags5 & M5_DIABLO)) || (type == 328 && !(ptr->mflags5 & M5_DLORDS)) || (type == 329 && !(ptr->mflags5 & M5_VANILLA)) || (type == 330 && !(ptr->mflags5 & M5_DNETHACK)) || (type == 331 && !(ptr->mflags5 & M5_RANDOMIZED)) || (type == 332 && !(ptr->msound == MS_SHOE)) || (type == 333 && !(ptr->msound == MS_STENCH)) || (type == 334 && !dmgtype(ptr, AD_ALIN)) || (type == 335 && !dmgtype(ptr, AD_SIN)) || (type == 336 && !dmgtype(ptr, AD_MINA)) || (type == 337 && !dmgtype(ptr, AD_CONT)) || (type == 338 && !dmgtype(ptr, AD_AGGR)) || (type == 339 && !(ptr->mflags5 & M5_JONADAB)) || (type == 340 && !dmgtype(ptr, AD_DATA)) || (type == 341 && !(ptr->mflags5 & M5_EVIL)) ) && (ct < 250000) ) );

	return ptr;

}

struct permonst *
speedymon() /* returns a monster whose speed is at least 18 */
{

	register struct permonst *ptr;
	register int ct = 0;

	ptr = rndmonst();

	do {

		ptr = rndmonst();
		ct++;
		if (!rn2(2000)) reset_rndmonst(NON_PM);

	} while ( !ptr || ((ptr->mmove < 18) && (ct < 250000) ));

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
beehivemon()
{
	int     i = rn2(78);

	if (i > 76) return((level_difficulty() > 15) ? &mons[PM_VORACIOUS_FORCE_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 74) return((level_difficulty() > 15) ? &mons[PM_VORACIOUS_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 73) return((level_difficulty() > 6) ? &mons[PM_ZOMBEE] : &mons[PM_KILLER_BEE]);
	else if (i > 72) return((level_difficulty() > 6) ? &mons[PM_FUMBLEBEE] : &mons[PM_KILLER_BEE]);
	else if (i > 64) return((level_difficulty() > 6) ? &mons[PM_WING_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 56) return((level_difficulty() > 6) ? &mons[PM_TWIN_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 55) return((level_difficulty() > 6) ? &mons[PM_WEREKILLERBEE] : &mons[PM_KILLER_BEE]);
	else if (i > 54) return((level_difficulty() > 5) ? &mons[PM_GIANT_JELLY_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 44) return((level_difficulty() > 5) ? &mons[PM_GIANT_KILLER_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 34) return(&mons[PM_HUNTER_BEE]);
	else if (i > 33) return(&mons[PM_WEREBEE]);
	else if (i > 32) return(&mons[PM_HONEY_BEE]);
	else if (i > 31) return(&mons[PM_WAX_BEE]);
	else if (i > 30) return(&mons[PM_JELLY_BEE]);
	else return(&mons[PM_KILLER_BEE]);

}

struct permonst *
migohivemon()
{
	if (rn2(8)) return (rn2(2) ? &mons[PM_MIGO_WARRIOR] : &mons[PM_MIGO_DRONE]);
	else if (!rn2(4)) return (rn2(2) ? &mons[PM_MIGO_FORCE_DRONE] : &mons[PM_ARMED_MIGO_DRONE]);
	else switch (rnd(14)) {

		case 1:
			return &mons[PM_MIGO_DRONE];
		case 2:
			return &mons[PM_MIGO_FORCE_DRONE];
		case 3:
			return &mons[PM_PETTY_MIGO_DRONE];
		case 4:
			return &mons[PM_ARMED_MIGO_DRONE];
		case 5:
			return &mons[PM_MIGO_WARRIOR];
		case 6:
			return &mons[PM_ARMED_MIGO_WARRIOR];
		case 7:
			if (level_difficulty() > 14) return &mons[PM_MIGO_QUEEN];
			else return &mons[PM_MIGO_WARRIOR];
		case 8:
			if (level_difficulty() > 14) return &mons[PM_ARMED_MIGO_QUEEN];
			else return &mons[PM_MIGO_WARRIOR];
		case 9:
			if (level_difficulty() > 25) return &mons[PM_MIGO_EMPRESS];
			else return &mons[PM_MIGO_WARRIOR];
		case 10:
			return &mons[PM_MI_GO__THE_FUNGI_FROM_YUGGOTH];
		case 11:
			return &mons[PM_GOMI_WORKER];
		case 12:
			return &mons[PM_GOMI_SOLDIER];
		case 13:
			return &mons[PM_GOMI_PHILOSOPHER];
		case 14:
			if (level_difficulty() > 14) return &mons[PM_GOMI_QUEEN];
			else return &mons[PM_MIGO_WARRIOR];

	}
}

struct permonst *
realzoomon()
{
	int     i = rn2(60) + rn2(3*level_difficulty());
	if (i > 175)    return(&mons[PM_JUMBO_THE_ELEPHANT]);
	else if (i > 115)       return(&mons[PM_MASTODON]);
	else if (i > 85)        return(&mons[PM_PYTHON]);
	else if (i > 70)        return(&mons[PM_MUMAK]);
	else if (i > 60)        return(&mons[PM_TIGER]);
	else if (i > 55)        return(&mons[PM_LYNX]);
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

STATIC_OVL struct permonst *
doomsquadmon()
{
	int     i = rn2(60) + rn2(3*level_difficulty());

	if (rn2(4)) return(&mons[PM_ZOMBIEMAN]);
	else if (rn2(3)) return(&mons[PM_FORMER_SERGEANT]);
	else if (rn2(2) && i > 90) return(&mons[PM_WOLFENSTEINER]);
	else if (rn2(2) && i > 115) return(&mons[PM_ARACHNOTRON]);
	else if (rn2(2) && i > 150) return(&mons[PM_MANCUBUS]);
	else return(&mons[PM_ZOMBIEMAN]);
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
	bwrite(fd, (void *) r, sizeof(struct mkroom));
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
		if((levl[sx][sy].typ > DBWALL) && !t_at(sx,sy) /*&& !nexttodoor(sx,sy)*/) {
		    if(rn2(5)) 
				(void) maketrap(sx, sy, rtrap, 100);
			if (randomnes == 1) rtrap = randomtrap();
		}

}

void
mkbossroom()
{
    struct mkroom *sroom;

	register int sx,sy = 0;
	int rtrap;
	int randomnes = 0;

    if(!(sroom = pick_room(FALSE))) return;

    sroom->rtype = BOSSROOM;

	for(sx = sroom->lx; sx <= sroom->hx; sx++)
	for(sy = sroom->ly; sy <= sroom->hy; sy++) {

		if(!rn2(5) && !t_at(sx, sy))
			(void) maketrap(sx, sy, BOSS_SPAWNER, 100);
	}

	level.flags.has_bossroom = 1;

}

void
mknastycentral()
{
    struct mkroom *sroom;
	register int sx,sy = 0;

    if (!(sroom = pick_room(FALSE))) return;

	if(sroom->rtype != OROOM || (has_upstairs(sroom) && rn2(iswarper ? 10 : 100)) ) return;

    sroom->rtype = NASTYCENTRAL;

	for(sx = sroom->lx; sx <= sroom->hx; sx++)
	for(sy = sroom->ly; sy <= sroom->hy; sy++) {
		(void) mksobj_at(rnd_class(RIGHT_MOUSE_BUTTON_STONE, NASTY_STONE), sx, sy, TRUE, FALSE);
	}

	level.flags.has_nastycentral = 1;

}

void
mkemptydesert()
{
    struct mkroom *sroom;
	register int sx,sy = 0;

    if (!(sroom = pick_room(FALSE))) return;

	if(sroom->rtype != OROOM || (has_upstairs(sroom) && rn2(iswarper ? 10 : 100)) ) return;

    sroom->rtype = EMPTYDESERT;

	for(sx = sroom->lx; sx <= sroom->hx; sx++)
	for(sy = sroom->ly; sy <= sroom->hy; sy++) {
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(10)) {
			levl[sx][sy].typ = rn2(5) ? SAND : SHIFTINGSAND;
		}
	}

	level.flags.has_emptydesert = 1;

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

    typ = !rn2(6) ? POOL : !rn2(5) ? MOAT : !rn2(8) ? ICE : !rn2(14) ? GRASSLAND : !rn2(25) ? SNOW : !rn2(30) ? ASH : !rn2(35) ? SAND : !rn2(35) ? PAVEDFLOOR : !rn2(50) ? HIGHWAY : !rn2(45) ? GRAVEWALL : !rn2(20) ? CLOUD : !rn2(32) ? AIR : !rn2(40) ? ROCKWALL : !rn2(40) ? TREE : !rn2(50) ? FARMLAND : !rn2(75) ? NETHERMIST : !rn2(100) ? STALACTITE : !rn2(100) ? MOUNTAIN : !rn2(60) ? IRONBARS : !rn2(70) ? CRYSTALWATER : !rn2(100) ? TUNNELWALL : !rn2(150) ? WATERTUNNEL : !rn2(100) ? MOORLAND : !rn2(150) ? URINELAKE : !rn2(200) ? SHIFTINGSAND : !rn2(50) ? STYXRIVER : !rn2(400) ? WOODENTABLE : !rn2(800) ? CARVEDBED : !rn2(300) ? STRAWMATTRESS : !rn2(800) ? WELL : !rn2(800) ? POISONEDWELL : !rn2(800) ? WAGON : !rn2(800) ? BURNINGWAGON : !rn2(480) ? FOUNTAIN : !rn2(60) ? CRYPTFLOOR : !rn2(100) ? BUBBLES : !rn2(80) ? RAINCLOUD : !rn2(1000) ? PENTAGRAM : !rn2(1000) ? THRONE : !rn2(240) ? SINK : !rn2(160) ? TOILET : !rn2(80) ? GRAVE : !rn2(1000) ? ALTAR : LAVAPOOL;

		if (sroom->ly == 20 && sroom->hy == 19) sroom->ly = sroom->hy = 20;
		if (sroom->ly == 1 && sroom->hy == 0) sroom->ly = sroom->hy = 0;

		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) /*&& !nexttodoor(sx,sy)*/) {
		    if(rn2(5)) {
			levl[sx][sy].typ = typ;
			if (typ == FOUNTAIN) 	level.flags.nfountains++;
			if (typ == SINK) 	level.flags.nsinks++;

			}
		}

	level.flags.has_poolroom = 1;

}

void
mkinsideroom()
{
    struct mkroom *sroom;
    int typ, typ2;
	register int sx,sy = 0;
	coord mm;

	register int tryct = 0;
	register struct obj *otmp;

    if (!(sroom = pick_room(FALSE))) return;

	if(sroom->rtype != OROOM || (has_upstairs(sroom) && rn2(iswarper ? 10 : 100)) ) return;

    sroom->rtype = INSIDEROOM;

		if (sroom->ly == 20 && sroom->hy == 19) sroom->ly = sroom->hy = 20;
		if (sroom->ly == 1 && sroom->hy == 0) sroom->ly = sroom->hy = 0;

		  if (!rn2(10)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);

			while (!rn2(2)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);
			}
		  }

		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) /*&& !nexttodoor(sx,sy)*/) {

    typ = !rn2(8) ? POOL : !rn2(10) ? MOAT : !rn2(9) ? ICE : !rn2(20) ? GRASSLAND : !rn2(50) ? SNOW : !rn2(75) ? ASH : !rn2(100) ? SAND : !rn2(35) ? PAVEDFLOOR : !rn2(150) ? HIGHWAY : !rn2(25) ? GRAVEWALL : !rn2(15) ? CLOUD : !rn2(16) ? AIR : !rn2(16) ? ROCKWALL : !rn2(16) ? TREE : !rn2(30) ? FARMLAND : !rn2(100) ? NETHERMIST : !rn2(200) ? STALACTITE : !rn2(20) ? MOUNTAIN : !rn2(20) ? IRONBARS : !rn2(50) ? CRYSTALWATER : !rn2(16) ? TUNNELWALL : !rn2(150) ? WATERTUNNEL : !rn2(100) ? MOORLAND : !rn2(200) ? URINELAKE : !rn2(300) ? SHIFTINGSAND : !rn2(100) ? STYXRIVER : !rn2(200) ? WOODENTABLE : !rn2(300) ? CARVEDBED : !rn2(100) ? STRAWMATTRESS : !rn2(400) ? WELL : !rn2(200) ? POISONEDWELL : !rn2(400) ? WAGON : !rn2(200) ? BURNINGWAGON : !rn2(40) ? FOUNTAIN : !rn2(60) ? CRYPTFLOOR : !rn2(100) ? BUBBLES : !rn2(80) ? RAINCLOUD : !rn2(100) ? PENTAGRAM : !rn2(100) ? THRONE : !rn2(32) ? SINK : !rn2(24) ? TOILET : !rn2(12) ? GRAVE : !rn2(200) ? ALTAR : LAVAPOOL;

		typ2 = randominsidetrap();

		    if(rn2(3)) {
			levl[sx][sy].typ = typ;

			if (typ == FOUNTAIN) 	level.flags.nfountains++;
			if (typ == SINK) 	level.flags.nsinks++;

			if (typ == GRAVE) {

					make_grave(sx, sy, (char *) 0);
					/* Possibly fill it with objects */
					if (!rn2(3)) (void) mkgold(0L, sx, sy);
					for (tryct = rn2(2 + rn2(4)); tryct; tryct--) {
					    otmp = mkobj(rn2(3) ? COIN_CLASS : RANDOM_CLASS, TRUE);
					    if (!otmp) return;
					    curse(otmp);
					    otmp->ox = sx;
					    otmp->oy = sy;
					    add_to_buried(otmp);
					}

				}
			}
			/*else*/ if (!rn2(Role_if(PM_CAMPERSTRIKER) ? 5 : 10)) (void) maketrap(sx, sy, typ2, 100);

			if (!rn2(1000)) 	(void) mksobj_at(SWITCHER, sx, sy, TRUE, FALSE);
			if (!rn2(Role_if(PM_CAMPERSTRIKER) ? 25 : 100)) 	(void) mksobj_at(UGH_MEMORY_TO_CREATE_INVENTORY, sx, sy, TRUE, FALSE);

			if (!rn2(Role_if(PM_CAMPERSTRIKER) ? 20 : 40)) 	(void) makemon(insidemon(), sx, sy, MM_ADJACENTOK);

		}

	level.flags.has_insideroom = 1;

}

void
mkriverroom()
{
    struct mkroom *sroom;
    schar typ;
	register int sx,sy = 0;
	coord mm;

    if (!(sroom = pick_room(FALSE))) return;

	if(sroom->rtype != OROOM || (has_upstairs(sroom) && rn2(iswarper ? 10 : 100)) ) return;

    sroom->rtype = RIVERROOM;

		if (sroom->ly == 20 && sroom->hy == 19) sroom->ly = sroom->hy = 20;
		if (sroom->ly == 1 && sroom->hy == 0) sroom->ly = sroom->hy = 0;

		  if (!rn2(30)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE);
		  }

		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) /*&& !nexttodoor(sx,sy)*/) {

	    typ = !rn2(5) ? POOL : !rn2(6) ? MOAT : !rn2(20) ? ICE : !rn2(20) ? GRASSLAND : !rn2(80) ? HIGHWAY : !rn2(20) ? FOUNTAIN : !rn2(30) ? FARMLAND : !rn2(35) ? MOUNTAIN : !rn2(50) ? CRYSTALWATER : !rn2(100) ? TUNNELWALL : !rn2(150) ? WATERTUNNEL : !rn2(6) ? ROCKWALL : !rn2(16) ? TREE : ROOM;

		levl[sx][sy].typ = typ;
		if (typ == FOUNTAIN) 	level.flags.nfountains++;
		if (typ == SINK) 	level.flags.nsinks++;
		if(Role_if(PM_CAMPERSTRIKER) && !rn2(50)) (void) maketrap(sx, sy, randomtrap(), 100);
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
				(void) maketrap(sx, sy, (rn2(10) ? STATUE_TRAP : ANIMATION_TRAP), 100 );
		}

		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		    if(rn2(2)) 
			{
			    struct obj *sobj = mksobj_at(STATUE, sx, sy, TRUE, FALSE);

			    if (sobj && !rn2(3) ) {
				for (i = rn2(2 + rn2(4)); i; i--)
				    (void) add_to_container(sobj,
						mkobj(RANDOM_CLASS, FALSE));
				sobj->owt = weight(sobj);
			    }
			    if (sobj) sobj->owt = weight(sobj);
			}

	level.flags.has_statueroom = 1;

}

void
save_rooms(fd)
int fd;
{
	short i;

	/* First, write the number of rooms */
	bwrite(fd, (void *) &nroom, sizeof(nroom));
	for(i=0; i<nroom; i++)
	    save_room(fd, &rooms[i]);
}

STATIC_OVL void
rest_room(fd, r)
int fd;
struct mkroom *r;
{
	short i;

	mread(fd, (void *) r, sizeof(struct mkroom));
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

	mread(fd, (void *) &nroom, sizeof(nroom));
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
