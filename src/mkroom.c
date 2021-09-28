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
STATIC_DCL struct permonst * prisonermon(void);
STATIC_DCL struct permonst * doomsquadmon(void);
STATIC_DCL struct permonst * evilroommon(void);
STATIC_DCL struct permonst * machineroommon(void);
STATIC_DCL struct permonst * fungus(void);
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
STATIC_DCL void mkchaosroom(void);
STATIC_DCL void mkmixedpool(void);
STATIC_DCL void mkshowerroom(void);
STATIC_DCL void mkcentraltedium(void);
STATIC_DCL void mkrampageroom(void);
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
mkroommateroom(variety)
int variety;
/* variety: 0 = usually make floor but not always, 1 = only very rarely make floor; might add more later */
{
	int xlou, xreal, ylou, yreal;

	boolean wallremoving = 0;
	if (variety == 0 && rn2(5)) wallremoving = 1;
	if (variety == 1 && !rn2(5)) wallremoving = 1;

	u.roommatehack = FALSE;
	if (wallremoving == 0 && rn2(3)) u.roommatehack = TRUE; /* allow monsters to be placed inside walls sometimes */
	if (wallremoving == 1 && !rn2(4)) u.roommatehack = TRUE; /* allow monsters to be placed inside walls sometimes */

	xlou = ylou = 1;
	xreal = 2 + rn2(10);
	yreal = 2 + rn2(5);
#ifdef BIGSLEX
	if (!rn2(100)) xreal += rnd(80);
	if (!rn2(100)) yreal += rnd(24);
	if (!rn2(10)) xreal += rnd(40);
	if (!rn2(10)) yreal += rnd(12);
	if (xreal > 80) xreal = 80;
	if (yreal > 20) yreal = 20;
#else
	if (!rn2(100)) xreal += rnd(40);
	if (!rn2(100)) yreal += rnd(12);
	if (!rn2(10)) xreal += rnd(20);
	if (!rn2(10)) yreal += rnd(6);
	if (xreal > 50) xreal = 50;
	if (yreal > 10) yreal = 10;
#endif

	xlou += rn2(COLNO - (xreal + 1));
	ylou += rn2(ROWNO - (yreal + 1));

	while ((xlou + xreal) > (COLNO - 1)) {
		impossible("xlou + xreal too big");
		xreal--;
	}
	while ((ylou + yreal) > (ROWNO - 1)) {
		impossible("ylou + yreal too big");
		yreal--;
	}

	add_room(xlou, ylou, (xlou + xreal), (ylou + yreal), rn2(2), RANDOMROOM, FALSE, FALSE, wallremoving ? 1 : 2);
	fill_room(&rooms[nroom - 1], FALSE);

	u.roommatehack = FALSE;

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
	case ROBBERCAVE: mkzoo(ROBBERCAVE); break;
	case SANITATIONCENTRAL: mkzoo(SANITATIONCENTRAL); break;
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
	case PLAYERCENTRAL: mkzoo(PLAYERCENTRAL); break;
	case VARIANTROOM: mkzoo(VARIANTROOM); break;

	case EVILROOM: mkzoo(EVILROOM); break;
	case RELIGIONCENTER: mkzoo(RELIGIONCENTER); break;
	case CHAOSROOM: mkchaosroom(); break;
	case CURSEDMUMMYROOM: mkzoo(CURSEDMUMMYROOM); break;
	case MIXEDPOOL: mkmixedpool(); break;
	case ARDUOUSMOUNTAIN: mkzoo(ARDUOUSMOUNTAIN); break;
	case LEVELFFROOM: mkzoo(LEVELFFROOM); break;
	case VERMINROOM: mkzoo(VERMINROOM); break;
	case MIRASPA: mkzoo(MIRASPA); break;
	case MACHINEROOM: mkzoo(MACHINEROOM); break;
	case SHOWERROOM: mkshowerroom(); break;
	case GREENCROSSROOM: mkzoo(GREENCROSSROOM); break;
	case CENTRALTEDIUM: mkcentraltedium(); break;
	case RUINEDCHURCH: mkzoo(RUINEDCHURCH); break;
	case RAMPAGEROOM: mkrampageroom(); break;
	case GAMECORNER: mkzoo(GAMECORNER); break;
	case ILLUSIONROOM: mkzoo(ILLUSIONROOM); break;


	case RANDOMROOM: {

retryrandtype:
		switch (rnd(83)) {

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
			case 64: mkzoo(EVILROOM); break;
			case 65: mkzoo(RELIGIONCENTER); break;
			case 66: mkchaosroom(); break;
			case 67: mkzoo(CURSEDMUMMYROOM); break;
			case 68: mkmixedpool(); break;
			case 69: mkzoo(ARDUOUSMOUNTAIN); break;
			case 70: mkzoo(LEVELFFROOM); break;
			case 71: mkzoo(VERMINROOM); break;
			case 72: mkzoo(MIRASPA); break;
			case 73: mkzoo(MACHINEROOM); break;
			case 74: mkshowerroom(); break;
			case 75: mkzoo(GREENCROSSROOM); break;
			case 76: mkcentraltedium(); break;
			case 77: mkzoo(RUINEDCHURCH); break;
			case 78: mkrampageroom(); break;
			case 79: mkzoo(GAMECORNER); break;
			case 80: mkzoo(ILLUSIONROOM); break;
			case 81: mkzoo(ROBBERCAVE); break;
			case 82: mkzoo(SANITATIONCENTRAL); break;
			case 83: mkzoo(PLAYERCENTRAL); break;

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

	int depthuz;

	if (iszapem && In_ZAPM(&u.uz) && !(u.zapemescape)) {

		d_level zapemlevel;
		int zapemdepth;
		zapemlevel.dnum = dname_to_dnum("Space Base");
		zapemlevel.dlevel = dungeons[zapemlevel.dnum].entry_lev;
		zapemdepth = depth(&zapemlevel);

		depthuz = (1 + depth(&u.uz) - zapemdepth);
		if (depthuz < 1) depthuz = 1; /* fail safe */

	} else if (u.preversionmode && !u.preversionescape && In_greencross(&u.uz)) {

		d_level preverlevel;
		int preverdepth;
		preverlevel.dnum = dname_to_dnum("Green Cross");
		preverlevel.dlevel = dungeons[preverlevel.dnum].entry_lev;
		preverdepth = depth(&preverlevel);

		depthuz = (1 + depth(&u.uz) - preverdepth);
		if (depthuz < 1) depthuz = 1; /* fail safe */

	} else {
		depthuz = depth(&u.uz);
	}

	boolean aggravatemonsteron = FALSE;
	boolean uglynastyhack = FALSE;

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
	    case CURSEDMUMMYROOM:
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
			/* evil variant, angband, animeband, steamband or dnethack */
		    break;
		case GAMECORNER:
			u.specialtensionmonster = !rn2(5) ? 361 : !rn2(4) ? 322 : !rn2(3) ? 323 : !rn2(2) ? 327 : 328;
			/* elona, cow, joke, diablo, dlords, one day aoe and elderscrolls should be added */
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
			if (!rn2(10)) {u.specialtensionmonster = rnd(379); /* monstercolor function! */
				if (!rn2(4)) u.specialtensionmonsterB = rnd(379);
			}
			break;
	    case DRAGONLAIR:
		goldlim = 1500 * level_difficulty();
		break;
	}

	moreorless = 100;
	if (rn2(10)) moreorless -= 10;
	if (rn2(10)) moreorless -= rnd(20);
	if (!rn2(3)) {
		moreorless -= rnd(30);
		if (!rn2(5)) moreorless -= rnd(40);
	}
	if (moreorless < 10) moreorless = 10;

	/* armories don't contain as many monsters; also, some other rooms and settings change the values */

	if (ishaxor && rn2(2)) moreorless = 100;
	if (type == VOIDROOM) {
		if (rn2(2)) moreorless /= 2;
		else moreorless /= 3;
	}
	if (type == ARMORY) moreorless /= 2;
	if (type == ROBBERCAVE) moreorless /= 2;
	if (type == DIVERPARADISE) moreorless /= 5;
	if (type == SANITATIONCENTRAL) moreorless /= 3;
	if (type == LEVELFFROOM) moreorless /= 3;
	if (type == MACHINEROOM) moreorless /= 3;
	if (type == ARDUOUSMOUNTAIN) moreorless /= 5;
	if (type == RUINEDCHURCH) moreorless /= 5;
	if (type == GREENCROSSROOM) moreorless /= 10;
	if (type == MIRASPA) moreorless /= 5;
	if (type == PLAYERCENTRAL) moreorless /= 5;
	if (type == LEVELSEVENTYROOM) moreorless /= 2;
	if (type == NUCLEARCHAMBER) moreorless /= 2;
	if (type == HAMLETROOM && moreorless > 5) moreorless = 5;
	if (issuxxor) moreorless /= 2;
	moreorless /= rnd(5); /* generally tone it down --Amy */
	if (moreorless < 1) moreorless = 1;
	if (moreorless > 100) moreorless = 100;

	if (sroom->ly == 20 && sroom->hy == 19) sroom->ly = sroom->hy = 20;
	if (sroom->ly == 1 && sroom->hy == 0) sroom->ly = sroom->hy = 0;

	/* evil patch idea by Amy: 2% chance that special rooms are populated with much higher-leveled monsters */
	if (!rn2(50) || (type == VOIDROOM) ) {
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
		aggravatemonsteron = TRUE;
	}

	if (type == VOIDROOM) {
		u.heavyaggravation = 1;
	}

	for(sx = sroom->lx; sx <= sroom->hx; sx++)
	    for(sy = sroom->ly; sy <= sroom->hy; sy++) {
		if(sroom->irregular) {
		    if ((int) levl[sx][sy].roomno != rmno ||
			  (levl[sx][sy].edge) /*||
			  (sroom->doorct &&
			   distmin(sx, sy, doors[sh].x, doors[sh].y) <= 1)*/)
			continue;
		} else if(!u.roommatehack && !SPACE_POS(levl[sx][sy].typ) /*||
			  (sroom->doorct &&
			   ((sx == sroom->lx && doors[sh].x == sx-1) ||
			    (sx == sroom->hx && doors[sh].x == sx+1) ||
			    (sy == sroom->ly && doors[sh].y == sy-1) ||
			    (sy == sroom->hy && doors[sh].y == sy+1)))*/)
		    continue;
		/* don't place monster on explicitly placed throne */
		if(type == COURT && IS_THRONE(levl[sx][sy].typ))
		    continue;

		if (type == CURSEDMUMMYROOM) { /* ugly hack to make the lich into an OOD monster but not the remaining stuff */
			if (sx == tx && sy == ty) {
				u.aggravation = 1;
				u.heavyaggravation = 1;
				uglynastyhack = 1;
				DifficultyIncreased += 1;
				HighlevelStatus += 1;
				reset_rndmonst(NON_PM);
			} else if (!aggravatemonsteron) {
				u.aggravation = 0;
				u.heavyaggravation = 0;
			} else u.heavyaggravation = 0;

			if (!(sx == tx && sy == ty) && uglynastyhack) {
				uglynastyhack = FALSE;
				if (DifficultyIncreased > 0) DifficultyIncreased -= 1;
				if (HighlevelStatus > 0) HighlevelStatus -= 1;
			}
		}

		if ( ((rnd(100) <= moreorless) || (rn2(5) && sx == tx && sy == ty)) && (type != EMPTYNEST) ) mon = makemon(
		    (type == COURT) ? (rn2(5) ? courtmon() : mkclass(S_ORC,0) ) :

		    (type == INSIDEROOM) ? (rn2(Role_if(PM_CAMPERSTRIKER) ? 20 : 40) ? insidemon() : (struct permonst *) 0 ) :

		    (type == BARRACKS) ? squadmon() :
		    (type == DOOMEDBARRACKS) ? doomsquadmon() :
		    (type == EVILROOM) ? evilroommon() :
		    (type == RUINEDCHURCH) ? mkclass(S_GHOST,0) :
		    (type == GREENCROSSROOM) ? (rn2(5) ? mkclass(S_HUMAN,0) : specialtensmon(332) /* MS_SHOE */ ) :
		    (type == MACHINEROOM) ? machineroommon() :
		    (type == MIRASPA) ? (!rn2(4) ? specialtensmon(49) /* MR_ACID */ : !rn2(3) ? specialtensmon(200) /* AD_ACID */ : !rn2(2) ? specialtensmon(88) /* M1_ACID */ : specialtensmon(235) /* AD_CORR */ ) :
		    (type == VERMINROOM) ? (!rn2(3) ? mkclass(S_RODENT,0) : !rn2(2) ? mkclass(S_SNAKE,0) : mkclass(S_WORM,0) ) :
		    (type == LEVELFFROOM) ? (!rn2(100) ? &mons[PM_SHOPKEEPER] : specialtensmon(323) /* M5_JOKE */ ) :
		    (type == ARDUOUSMOUNTAIN) ? (rn2(2) ? specialtensmon(156) /* UNCOMMON10 */ : rn2(2) ? specialtensmon(155) /* UNCOMMON7 */ : rn2(2) ? specialtensmon(154) /* UNCOMMON5 */ : rn2(2) ? specialtensmon(153) /* UNCOMMON3 */ : specialtensmon(152) /* UNCOMMON2 */ ) :
		    (type == CURSEDMUMMYROOM) ? (sx == tx && sy == ty ? mkclass(S_LICH,0) : mkclass(S_MUMMY,0) ) :
		    (type == RELIGIONCENTER) ? (rn2(5) ? specialtensmon(347) /* MS_CONVERT */ : specialtensmon(348) /* MS_ALIEN */ ) :
			(type == CLINIC) ? specialtensmon(218) /* AD_HEAL */ :
			(type == TERRORHALL) ? mkclass(S_UMBER,0) :
			(type == ROBBERCAVE) ? (!rn2(20) ? specialtensmon(286) /* AD_SAMU */ : !rn2(4) ? specialtensmon(357) /* AD_THIE */ : !rn2(3) ? specialtensmon(212) /* AD_SITM */ : !rn2(2) ? specialtensmon(213) /* AD_SEDU */ : specialtensmon(211) /* AD_SGLD */ ) :
			(type == SANITATIONCENTRAL) ? specialtensmon(363) /* AD_SANI */ :
			(type == VARIANTROOM) ? specialtensmon(u.specialtensionmonster) :
			(type == ILLUSIONROOM) ? illusionmon() :
			(type == GAMECORNER) ? specialtensmon(u.specialtensionmonster) :
			(type == TENSHALL) ? (u.specialtensionmonsterB ? (rn2(2) ? specialtensmon(u.specialtensionmonsterB) : specialtensmon(u.specialtensionmonster) ) : u.specialtensionmonster ? specialtensmon(u.specialtensionmonster) : u.tensionmonsterspecB ? (rn2(2) ? u.tensionmonsterspecB : u.tensionmonsterspec ) : u.tensionmonsterspec ? u.tensionmonsterspec : u.colormonsterB ? (rn2(2) ? colormon(u.colormonsterB) : colormon(u.colormonster) ) : u.colormonster ? colormon(u.colormonster) : u.tensionmonsterB ? (rn2(2) ? tenshallmon() : tenshallmonB() ) : tenshallmon()) :
			(type == ELEMHALL) ? mkclass(S_ELEMENTAL,0) :
			(type == ANGELHALL) ? mkclass(S_ANGEL,0) :
			(type == FEMINISMROOM) ? (!rn2(50) ? specialtensmon(369) /* AD_FEMI */ : !rn2(20) ? specialtensmon(333) /* MS_STENCH */ : !rn2(3) ? specialtensmon(38) : !rn2(2) ? specialtensmon(39) : specialtensmon(40) ) /* MS_FART_foo */ :
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
			(type == PLAYERCENTRAL) ? (&mons[PM_ARCHEOLOGIST + rn2(PM_WIZARD - PM_ARCHEOLOGIST + 1)]) :
			(type == COINHALL) ? mkclass(S_BAD_COINS,0) :
			(type == GRUEROOM) ? mkclass(S_GRUE,0) :
		    (type == MORGUE) ? morguemon() :
		    (type == FUNGUSFARM) ? (!rn2(4) ? mkclass(S_BLOB,0) : !rn2(3) ? mkclass(S_PUDDING,0) : !rn2(2) ? mkclass(S_JELLY,0) : mkclass(S_FUNGUS,0)) :
		    (type == BEEHIVE) ? (sx == tx && sy == ty ? (((depthuz < 5) && !In_sokoban_real(&u.uz) && !In_mainframe(&u.uz) && (level_difficulty() < (3 + rn2(3)))) ? &mons[PM_BIG_BEE] : &mons[PM_QUEEN_BEE]) : beehivemon()) :
		    (type == PRISONCHAMBER) ? (sx == tx && sy == ty ? prisonermon() : mkclass(S_OGRE,0) ) :
		    (type == DOUGROOM) ? douglas_adams_mon() : 
		    (type == LEPREHALL) ? mkclass(S_LEPRECHAUN,0) :
		    (type == COCKNEST) ? mkclass(S_COCKATRICE,0) :
                (type == ARMORY) ? (rn2(10) ? mkclass(S_RUSTMONST,0) : mkclass(S_PUDDING,0) ) :
		    (type == ANTHOLE) ? mkclass(S_ANT,0) :
		    (type == DRAGONLAIR) ? mkclass(S_DRAGON,0) :
		    (type == LEMUREPIT)? 
		    	(!rn2(20) ? &mons[PM_HORNED_DEVIL] : !rn2(20) ? mkclass(S_DEMON,0) : !rn2(50) ? &mons[ndemon(A_NONE)] : rn2(2) ? mkclass(S_IMP,0) : &mons[PM_LEMURE]) :
		    (type == MIGOHIVE) ? (sx == tx && sy == ty ? (((depthuz < 10) && !In_sokoban_real(&u.uz) && !In_mainframe(&u.uz) && (level_difficulty() < (5 + rn2(5)))) ? &mons[PM_SUDO_MIGO] : &mons[PM_MIGO_QUEEN]) : migohivemon()) :
		    (type == BADFOODSHOP) ? mkclass(S_BAD_FOOD,0) :
		    (type == REALZOO) ? (rn2(3) ? realzoomon() : rn2(3) ? mkclass(S_QUADRUPED,0) : rn2(3) ? mkclass(S_FELINE,0) : rn2(3) ? mkclass(S_YETI,0) : mkclass(S_SNAKE,0) ) :
		    (type == GIANTCOURT) ? mkclass(S_GIANT,0) :
		    (struct permonst *) 0,
		   sx, sy, /*NO_MM_FLAGS*/MM_ADJACENTOK);
               else mon = ((struct monst *)0);
/* some rooms can spawn new monster variants now --Amy */
		if(mon) {
			if ((sleepchance > 1) && !(In_netherrealm(&u.uz)) && !issoviet && !(uarmf && uarmf->oartifact == ART_VERY_NICE_PERSON) && rn2(sleepchance)) mon->msleeping = 1; /*random chance of them not being asleep --Amy*/
		/* In Soviet Russia, monsters are always awake harharharharhar harharhar harhar! --Amy */

			/*enemies in these rooms will almost always be hostile now --Amy*/
			if (mon->mpeaceful && rn2(20)) {
				mon->mpeaceful = 0;
				set_malign(mon);
			}

			/* green cross rooms are supposed to be very peaceful if you behave well --Amy */
			if (type == GREENCROSSROOM && (rnd(20) < u.ualign.record)) {
				mon->mpeaceful = 1;
				set_malign(mon);
			}

		}
		switch(type) {

		    case CRYPTROOM:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(10)) {
				levl[sx][sy].typ = CRYPTFLOOR;
			}
			break;

		    case GREENCROSSROOM:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(10)) {
				levl[sx][sy].typ = rn2(3) ? PAVEDFLOOR : GRASSLAND;
			}
			break;

		    case MIRASPA:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(10)) {
				levl[sx][sy].typ = URINELAKE;
			}
			break;

		    case MACHINEROOM:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(4)) {
				levl[sx][sy].typ = !rn2(3) ? TUNNELWALL : !rn2(2) ? STALACTITE : CRYSTALWATER;
			}
			break;

		    case ARDUOUSMOUNTAIN:
			if(levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) {

				if (rn2(2)) levl[sx][sy].typ = MOUNTAIN;
				else if (!rn2(5)) levl[sx][sy].typ = TUNNELWALL;
			}
			break;

		    case ROBBERCAVE:

			if(levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) {

				if (!rn2(5)) levl[sx][sy].typ = ROCKWALL;
			}

			if(!rn2(25) && !t_at(sx, sy)) {
				(void) maketrap(sx, sy, MONSTER_CUBE, 100, FALSE);
			}

			break;

		    case SANITATIONCENTRAL:

			if(levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) {

				if (!rn2(5)) levl[sx][sy].typ = ROCKWALL;
			}

			break;

		    case LEVELFFROOM:
			if(levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) {

				if (rn2(2)) levl[sx][sy].typ = FARMLAND;
				else if (!rn2(3)) levl[sx][sy].typ = HIGHWAY;
				else if (!rn2(4)) levl[sx][sy].typ = GRASSLAND;
			}
			break;

		    case COOLINGCHAMBER:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(10)) {
				levl[sx][sy].typ = rn2(2) ? SNOW : ICE;
			}
			if(!rn2(10))
			    (void) mksobj_at(ICE_BOX,sx,sy,TRUE,FALSE, FALSE);
			break;

		    case VOIDROOM:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(10)) {
				levl[sx][sy].typ = NETHERMIST;
			}
			if(!rn2(4) && !t_at(sx, sy))
				(void) maketrap(sx, sy, GIANT_CHASM, 100, TRUE);
			break;

		    case RARITYROOM:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !rn2(5)) {
				levl[sx][sy].typ = randomwalltype();
			}
			break;

		    case DIVERPARADISE:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(3)) {
				levl[sx][sy].typ = rn2(4) ? WATERTUNNEL : MOAT;
			}
			if (!rn2(5)) (void) mkobj_at(RANDOM_CLASS, sx, sy, FALSE, FALSE);
			break;

		    case WIZARDSDORM:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !rn2(4)) {
				levl[sx][sy].typ = rn2(2) ? WOODENTABLE : CARVEDBED;
			}

			if(!rn2(50))
			    (void) mksobj_at(CHARGER,sx,sy,TRUE,FALSE, FALSE);

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
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(2)) {
				levl[sx][sy].typ = STYXRIVER;
			}
			if(!rn2(5) && !t_at(sx, sy))
				(void) maketrap(sx, sy, CONTAMINATION_TRAP, 100, TRUE);

			if(!rn2(20))
			    (void) mksobj_at(rn2(10) ? CHARGER : SYMBIOTE,sx,sy,TRUE,FALSE, FALSE);

			break;

		    case PRISONCHAMBER:
			if (sx == tx && sy == ty && levl[sx][sy].typ != STAIRS && levl[sx][sy].typ != LADDER) {
				levl[sx][sy].typ = ALTAR;
				levl[sx][sy].altarmask = Align2amask( A_NONE );
			}
			break;

		    case KOPSTATION:
			if(!rn2(25) && !t_at(sx, sy))
				(void) maketrap(sx, sy, KOP_CUBE, 100, FALSE);
			break;

		    case BOSSROOM:
			if(!rn2(5) && !t_at(sx, sy))
				(void) maketrap(sx, sy, BOSS_SPAWNER, 100, FALSE);
			break;

		    case HAMLETROOM:
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(10)) {
				levl[sx][sy].typ = rn2(10) ? GRASSLAND : !rn2(4) ? BURNINGWAGON : !rn2(3) ? WELL : rn2(2) ? POISONEDWELL : WAGON ;
			}
			if(!rn2(5) && !t_at(sx, sy)) {
				if (rn2(10)) (void) maketrap(sx, sy, STATUE_TRAP, 100, TRUE);
				else if (rn2(20)) (void) maketrap(sx, sy, MONSTER_CUBE, 100, FALSE);
				else (void) maketrap(sx, sy, WARP_ZONE, 100, TRUE);
			}
			break;

		    case MEADOWROOM:
			if(rn2(5) && (levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR)) {
				levl[sx][sy].typ = GRASSLAND;
			}
			if(!rn2(5))
			    (void) mkobj_at(FOOD_CLASS, sx, sy, FALSE, FALSE);

			break;

		    case HELLPIT:
			if(!rn2(2) && (levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR)) {
				levl[sx][sy].typ = LAVAPOOL;
			}
			if (!rn2(10) && !(t_at(sx,sy))) {
				(void) maketrap(sx, sy, FIRE_TRAP, 100, TRUE);
			}
			break;

		    case ZOO:
			if (specialzoo) {
				if (specialzoochance > rnd(100)) {
				    (void) mkobj_at(specialzootype, sx, sy, TRUE, FALSE);
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
			if (rn2(4)) (void) mkgold(1, sx, sy); /* don't spawn endless amounts of gold --Amy */
			else (void) mkgold((long) rn1(i, 10), sx, sy);
			break;
		    case MORGUE:
			if(!rn2(5))
			    (void) mk_tt_object(CORPSE, sx, sy);
			if(!rn2(50) && (level_difficulty() > 10+rnd(200) )) { /* real player ghosts --Amy */
				coord mmm;
				mmm.x = sx;   
				mmm.y = sy;
			    (void) tt_mname(&mmm, FALSE, 0);
				}
			if(ishaxor && !rn2(50) && (level_difficulty() > 10+rnd(200) )) { /* real player ghosts --Amy */
				coord mmm;
				mmm.x = sx;   
				mmm.y = sy;
			    (void) tt_mname(&mmm, FALSE, 0);
				}
			if(!rn2(ishaxor ? 10 : 20))	/* lots of treasure buried with dead */
			    (void) mksobj_at((rn2(3)) ? LARGE_BOX : CHEST, sx, sy, TRUE, FALSE, FALSE);
			if (!rn2(10)) {
			    make_grave(sx, sy, (char *)0);

				if (!rn2(5)) (void) mkgold(0L, sx, sy);
				for (gravetries = rn2(2 + rn2(4)); gravetries; gravetries--) {
					if (timebasedlowerchance()) {
					    otmp = mkobj(rn2(3) ? COIN_CLASS : RANDOM_CLASS, TRUE, FALSE);
					    if (!otmp) return;
					    curse(otmp);
					    otmp->ox = sx;
					    otmp->oy = sy;
					    add_to_buried(otmp);
					}
				}

			}
			break;
		    case BEEHIVE:
			if(!rn2(5)) /* slightly lowered chance --Amy */
			    (void) mksobj_at(LUMP_OF_ROYAL_JELLY, sx, sy, TRUE, FALSE, FALSE);
			break;
		    case FUNGUSFARM:
			if (!rn2(25)) /* lowered chance --Amy */
			    (void) mksobj_at(SLIME_MOLD, sx, sy, TRUE, FALSE, FALSE);
			if (!rn2(100))
			    (void) mksobj_at(SYMBIOTE, sx, sy, TRUE, FALSE, FALSE);
			break;
		    case MIGOHIVE:
			switch (rn2(100)) { /* greatly lowered chance --Amy */
			    case 9:
				mksobj_at(DIAMOND, sx, sy, TRUE, FALSE, FALSE);
				break;
			    case 8:
				mksobj_at(RUBY, sx, sy, TRUE, FALSE, FALSE);
				break;
			    case 7:
			    case 6:
				mksobj_at(AGATE, sx, sy, TRUE, FALSE, FALSE);
				break;
			    case 5:
			    case 4:
				mksobj_at(FLUORITE, sx, sy, TRUE, FALSE, FALSE);
				break;
			    default:
				break;
			}
			break;
		    case BARRACKS:
			if(!rn2(ishaxor ? 25 : 50))	/* the payroll and some loot */
			    (void) mksobj_at((rn2(3)) ? LARGE_BOX : CHEST, sx, sy, TRUE, FALSE, FALSE);
			if (!rn2(25)) {
			    make_grave(sx, sy, (char *)0);

				if (!rn2(5)) (void) mkgold(0L, sx, sy);
				for (gravetries = rn2(2 + rn2(4)); gravetries; gravetries--) {
					if (timebasedlowerchance()) {
					    otmp = mkobj(rn2(3) ? COIN_CLASS : RANDOM_CLASS, TRUE, FALSE);
					    if (!otmp) return;
					    curse(otmp);
					    otmp->ox = sx;
					    otmp->oy = sy;
					    add_to_buried(otmp);
					}
				}

			}
			break;

		    case CLINIC:
			if(!rn2(10))
			    (void) mksobj_at(ICE_BOX,sx,sy,TRUE,FALSE, FALSE);
			if (!rn2(100))
			    (void) mksobj_at(SYMBIOTE, sx, sy, TRUE, FALSE, FALSE);
			break;
		    case GOLEMHALL:
			if(!rn2(ishaxor ? 10 : 20))
			    (void) mkobj_at(CHAIN_CLASS, sx, sy, FALSE, FALSE);
			break;
		    case SPIDERHALL:
			if(!rn2(3))
			    (void) mksobj_at(EGG,sx,sy,TRUE,FALSE, FALSE);
			break;
		    case EMPTYNEST:
		      (void) mksobj_at(EGG,sx,sy,TRUE,FALSE, FALSE);
			break;
		    case COCKNEST:
			if(!rn2(7)) {
			    struct obj *sobj = mk_tt_object(STATUE, sx, sy);

			    if (sobj) {
				for (i = rn2(5); i; i--)
					if (timebasedlowerchance()) {
					    (void) add_to_container(sobj, mkobj(RANDOM_CLASS, FALSE, FALSE));
					}
				sobj->owt = weight(sobj);
			    }
			}
			break;
		    case ARMORY:
			{
				struct obj *otmp;
				if (!rn2(5)) { /* sorry Patrick, but the quantity of those items needs to be lower. --Amy */
					if (rn2(2))
						otmp = mkobj_at(WEAPON_CLASS, sx, sy, FALSE, FALSE);
					else
						otmp = mkobj_at(ARMOR_CLASS, sx, sy, FALSE, FALSE);
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
						(void) mkobj_at(WEAPON_CLASS, sx, sy, FALSE, FALSE);
					else
						(void) mkobj_at(ARMOR_CLASS, sx, sy, FALSE, FALSE);
				}
			}
			break;
		    case ANTHOLE:
			if(!rn2(15))
			    (void) mkobj_at(FOOD_CLASS, sx, sy, FALSE, FALSE);
			break;
		    case ANGELHALL:
			if(!rn2(10))
			    (void) mkobj_at(GEM_CLASS, sx, sy, FALSE, FALSE);
			break;

		    case CURSEDMUMMYROOM:
			if(!rn2(10))
			    (void) mksobj_at(STATUE, sx, sy, TRUE, FALSE, FALSE);

			if(!rn2(10) && !t_at(sx, sy))
				(void) maketrap(sx, sy, rn2(50) ? STATUE_TRAP : SATATUE_TRAP, 100, TRUE);

			if(!rn2(10)) {
				struct obj *enchantedgear;
				enchantedgear = mkobj_at(rn2(2) ? WEAPON_CLASS : ARMOR_CLASS, sx, sy, FALSE, FALSE);

				if (enchantedgear && enchantedgear->spe == 0) {
					enchantedgear->spe = rne(Race_if(PM_LISTENER) ? 3 : 2);
				}

				/* the items are supposed to be cursed... --Amy */
				if (enchantedgear) {
					curse(enchantedgear);
					curse(enchantedgear);
					if (enchantedgear->hvycurse) { /* ... with a good chance of nastier curses */
						curse(enchantedgear);
						curse(enchantedgear);
					}
				}
			}

			break;

		    case RUINEDCHURCH:

			if(!rn2(5))
			    (void) mksobj_at(rnd_class(JADE+1, LUCKSTONE-1), sx, sy, TRUE, FALSE, FALSE); /* worthless glass */
			if(!rn2(20))
			    (void) mkobj_at(SCROLL_CLASS, sx, sy, FALSE, FALSE);
			break;
		    case MIMICHALL: /* lower overall amount of items --Amy */
			if(!rn2(10))
			    (void) mkobj_at(rn2(5) ? COIN_CLASS : RANDOM_CLASS, sx, sy, FALSE, FALSE);
			break;
		    case HUMANHALL:
			if(!rn2(3))
			    (void) mkobj_at(RANDOM_CLASS, sx, sy, FALSE, FALSE);
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
			  chest = mksobj_at(CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
			  if (chest) {
				  chest->spe = 2; /* so it can be found later */
			  }
		}

		  if (!rn2(20)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
		  }

		  level.flags.has_court = 1;
		  break;
		}
	      case ARMORY:

		  level.flags.has_armory = 1;

		  if (!rn2(10)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
		  }

		  break;

	      case RUINEDCHURCH:

		  if (somexy(sroom, &mm)) {
			levl[mm.x][mm.y].typ = ALTAR;
			switch (rnd(4)) {
	
				case 1: levl[mm.x][mm.y].altarmask = Align2amask( A_LAWFUL ); break;
				case 2: levl[mm.x][mm.y].altarmask = Align2amask( A_NEUTRAL ); break;
				case 3: levl[mm.x][mm.y].altarmask = Align2amask( A_CHAOTIC ); break;
				case 4: levl[mm.x][mm.y].altarmask = Align2amask( A_NONE ); break;

			}

			(void) mkobj_at(SPBOOK_CLASS, mm.x, mm.y, FALSE, FALSE);

		  }
              level.flags.has_ruinedchurch = 1;

		  break;

	      case BARRACKS:

		  if (!rn2(50)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
		  }

		  level.flags.has_barracks = 1;
		  break;
	      case ZOO:
		  if (!rn2(50)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
		  }
	      case REALZOO:
		  level.flags.has_zoo = 1;
		  if (!rn2(10)) {
			if (somexy(sroom, &mm)) {
				(void) mksobj_at(SACK, mm.x, mm.y, TRUE, FALSE, FALSE);
			}
		  }
		  break;
	      case MORGUE:
		  if (!rn2(50)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
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
		  if (!rn2(5)) {
			if (somexy(sroom, &mm)) {
				(void) mksobj_at(SACK, mm.x, mm.y, TRUE, FALSE, FALSE);
			}
		  }
		  level.flags.has_zoo = 1;
		  break;
	      case LEMUREPIT:
		  level.flags.has_lemurepit = 1;
		  break;
	      case MIGOHIVE:
		  level.flags.has_migohive = 1;
		  break;
	      case FUNGUSFARM:
		  if (!rn2(5)) {
			  if (somexy(sroom, &mm)) {
				(void) mksobj_at(SYMBIOTE, mm.x, mm.y, TRUE, FALSE, FALSE);
			  }
		  }
		  level.flags.has_fungusfarm = 1;
		  break;
            case CLINIC:
		  if (!rn2(10)) {
			  if (somexy(sroom, &mm)) {
				(void) mksobj_at(SYMBIOTE, mm.x, mm.y, TRUE, FALSE, FALSE);
			  }
		  }
              level.flags.has_clinic = 1;
              break;
            case TERRORHALL:
              level.flags.has_terrorhall = 1;
              break;
            case INSIDEROOM:

		  if (!rn2(10)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);

			while (!rn2(2)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
			}
		  }

              level.flags.has_insideroom = 1;
              break;
            case RIVERROOM:
		  if (!rn2(30)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
		  }
              level.flags.has_riverroom = 1;
              break;
            case TENSHALL:
		  if (!rn2(50)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
		  }
              level.flags.has_tenshall = 1;
              break;
            case EVILROOM:
              level.flags.has_evilroom = 1;
              break;
            case RELIGIONCENTER:
		  if (!rn2(10)) {
			if (somexy(sroom, &mm)) {
				(void) mksobj_at(SACK, mm.x, mm.y, TRUE, FALSE, FALSE);
			}
		  }
              level.flags.has_religioncenter = 1;
              break;
            case CURSEDMUMMYROOM:
              level.flags.has_cursedmummyroom = 1;
              break;
            case ARDUOUSMOUNTAIN:
		  if (!rn2(50)) {
			if (somexy(sroom, &mm)) {
				(void) mksobj_at(SACK, mm.x, mm.y, TRUE, FALSE, FALSE);
			}
		  }
		  if (!rn2(3)) {
			  if (somexy(sroom, &mm))
				  (void) (void) mkobj_at(IMPLANT_CLASS, mm.x, mm.y, FALSE, FALSE);
		  }
              level.flags.has_arduousmountain = 1;
              break;
            case LEVELFFROOM:
              level.flags.has_levelffroom = 1;
              break;
            case VERMINROOM:
		  if (!rn2(20)) {
			if (somexy(sroom, &mm)) {
				(void) mksobj_at(SACK, mm.x, mm.y, TRUE, FALSE, FALSE);
			}
		  }
              level.flags.has_verminroom = 1;
              break;
            case MIRASPA:
              level.flags.has_miraspa = 1;
              break;
            case MACHINEROOM:
		  if (!rn2(5)) {
			  if (somexy(sroom, &mm))
				  (void) (void) mkobj_at(IMPLANT_CLASS, mm.x, mm.y, FALSE, FALSE);
		  }
              level.flags.has_machineroom = 1;
              break;
            case SHOWERROOM:
              level.flags.has_showerroom = 1;
              break;
            case GREENCROSSROOM:
              level.flags.has_greencrossroom = 1;
              break;
            case GAMECORNER:
		  if (!rn2(10)) {
			if (somexy(sroom, &mm)) {
				(void) mksobj_at(SACK, mm.x, mm.y, TRUE, FALSE, FALSE);
			}
		  }
              level.flags.has_gamecorner = 1;
              break;
            case ILLUSIONROOM:
              level.flags.has_illusionroom = 1;
              break;
            case ELEMHALL:
		  if (!rn2(50)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
		  }
              level.flags.has_elemhall = 1;
              break;
            case ANGELHALL:
		  if (!rn2(20)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
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
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
		  }
              level.flags.has_trollhall = 1;
              break;
            case COINHALL:
              level.flags.has_coinhall = 1;
              break;
            case HUMANHALL:
		  if (!rn2(30)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
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
		if (somexy(sroom, &mm)) {
			(void) mksobj_at(CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
		}
		break;
	    case TROUBLEZONE:
		  if (!rn2(3)) {
			  if (somexy(sroom, &mm)) {
				if(levl[mm.x][mm.x].typ == ROOM || levl[mm.x][mm.x].typ == CORR) {
					levl[mm.x][mm.x].typ = PENTAGRAM;
				}
			}
		  }
		if (somexy(sroom, &mm)) {
			(void) mksobj_at(CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
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

		if (somexy(sroom, &mm)) {
			(void) mksobj_at(HANDYBAG, mm.x, mm.y, TRUE, FALSE, FALSE);
		}
		if (!rn2(10)) {
			if (somexy(sroom, &mm)) {
				(void) mksobj_at(rnd_class(ELIF_S_JEWEL, DORA_S_JEWEL), mm.x, mm.y, TRUE, FALSE, FALSE);
			}
		}

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
		  if (!rn2(10)) {
			if (somexy(sroom, &mm)) {
				(void) mksobj_at(SACK, mm.x, mm.y, TRUE, FALSE, FALSE);
			}
		  }
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
		  if (!rn2(10)) {
			if (somexy(sroom, &mm)) {
				(void) mksobj_at(SACK, mm.x, mm.y, TRUE, FALSE, FALSE);
			}
		  }
		level.flags.has_wizardsdorm = 1;
		break;
	    case DOOMEDBARRACKS:
		level.flags.has_doomedbarracks = 1;
		break;
	    case SLEEPINGROOM:
		  if (!rn2(30)) {
			if (somexy(sroom, &mm)) {
				(void) mksobj_at(SACK, mm.x, mm.y, TRUE, FALSE, FALSE);
			}
		  }
		level.flags.has_sleepingroom = 1;
		break;
	    case DIVERPARADISE:
		level.flags.has_diverparadise = 1;
		if (!rn2(5)) {
			if (somexy(sroom, &mm)) {
				(void) mksobj_at(MATERIAL_KIT, mm.x, mm.y, TRUE, FALSE, FALSE);
			}
		}
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
		  if (!rn2(20)) {
			if (somexy(sroom, &mm)) {
				(void) mksobj_at(SACK, mm.x, mm.y, TRUE, FALSE, FALSE);
			}
		  }
		level.flags.has_prisonchamber = 1;
		break;
	    case ROBBERCAVE:
		  if (!rn2(20)) {
			if (somexy(sroom, &mm)) {
				(void) mksobj_at(SACK, mm.x, mm.y, TRUE, FALSE, FALSE);
			}
		  }
		level.flags.has_robbercave = 1;
		break;
	    case SANITATIONCENTRAL:
		level.flags.has_sanitationcentral = 1;
		break;
	    case NUCLEARCHAMBER:
		level.flags.has_nuclearchamber = 1;
		break;
	    case LEVELSEVENTYROOM:
		level.flags.has_levelseventyroom = 1;
		break;
	    case PLAYERCENTRAL:
		level.flags.has_playercentral = 1;
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
	u.heavyaggravation = 0;

}

/* make a swarm of undead around mm */
void
mkundead(mm, revive_corpses, mm_flags, hostility)
coord *mm;
boolean revive_corpses;
int mm_flags;
boolean hostility;
{
	int cnt = 1;
	if (!rn2(2)) cnt = (level_difficulty() + 1)/10;
	if (!rn2(5)) cnt += rnz(5);
	if (cnt < 1) cnt = 1;
	struct permonst *mdat;
	struct obj *otmp;
	coord cc;

	register struct monst *mtmp;

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
		mtmp = makemon(mdat, cc.x, cc.y, mm_flags);
		if (mtmp && hostility) {
			mtmp->mpeaceful = 0;
			mtmp->mfrenzied = 1;
		}
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
			if(!eelct || !rn2(10)) {
			    /* mkclass() won't do, as we might get kraken */
/* comment by Amy - low-level players shouldn't move close to water anyway, so I will totally spawn everything here! */
			    (void) makemon(mkclass(S_EEL,0), sx, sy, NO_MM_FLAGS);
			    eelct++;
			}
		    } else
			if(!rn2(10))	/* swamps tend to be moldy */
			    (void) makemon(mkclass(S_FUNGUS,0), sx, sy, NO_MM_FLAGS);
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
	int     i = rnd(105);
	if (i > 100)	return(mkclass(S_WORM_TAIL,0));
	if (i > 99)       return(rn2(1000) ? &mons[PM_SUPERTHIEF] : (level_difficulty() < 40) ? &mons[PM_SUPERTHIEF] : &mons[PM_PERCENTS_____NIX]);
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

	} while ( !ptr || (( (type == 1 && !(ptr->msound == MS_SILENT)) || (type == 2 && !(ptr->msound == MS_BARK)) || (type == 3 && !(ptr->msound == MS_MEW)) || (type == 4 && !(ptr->msound == MS_ROAR)) || (type == 5 && !(ptr->msound == MS_GROWL)) || (type == 6 && !(ptr->msound == MS_SQEEK)) || (type == 7 && !(ptr->msound == MS_SQAWK)) || (type == 8 && !(ptr->msound == MS_HISS)) || (type == 9 && !(ptr->msound == MS_BUZZ)) || (type == 10 && !(ptr->msound == MS_GRUNT)) || (type == 11 && !(ptr->msound == MS_NEIGH)) || (type == 12 && !(ptr->msound == MS_WAIL)) || (type == 13 && !(ptr->msound == MS_GURGLE)) || (type == 14 && !(ptr->msound == MS_BURBLE)) || (type == 15 && !(ptr->msound == MS_SHRIEK)) || (type == 16 && !(ptr->msound == MS_BONES)) || (type == 17 && !(ptr->msound == MS_LAUGH)) || (type == 18 && !(ptr->msound == MS_MUMBLE)) || (type == 19 && !(ptr->msound == MS_IMITATE)) || (type == 20 && !(ptr->msound == MS_ORC)) || (type == 21 && !(ptr->msound == MS_HUMANOID)) || (type == 22 && !(ptr->msound == MS_ARREST)) || (type == 23 && !(ptr->msound == MS_SOLDIER)) || (type == 24 && !(ptr->msound == MS_DJINNI)) || (type == 25 && !(ptr->msound == MS_NURSE)) || (type == 26 && !(ptr->msound == MS_SEDUCE)) || (type == 27 && !(ptr->msound == MS_VAMPIRE)) || (type == 28 && !(ptr->msound == MS_CUSS)) || (type == 29 && !(ptr->msound == MS_NEMESIS)) || (type == 30 && !(ptr->msound == MS_SPELL)) || (type == 31 && !(ptr->msound == MS_WERE)) || (type == 32 && !(ptr->msound == MS_BOAST)) || (type == 33 && !(ptr->msound == MS_SHEEP)) || (type == 34 && !(ptr->msound == MS_CHICKEN)) || (type == 35 && !(ptr->msound == MS_COW)) || (type == 36 && !(ptr->msound == MS_PARROT)) || (type == 37 && !(ptr->msound == MS_WHORE)) || (type == 38 && !(ptr->msound == MS_FART_QUIET)) || (type == 39 && !(ptr->msound == MS_FART_NORMAL)) || (type == 40 && !(ptr->msound == MS_FART_LOUD)) || (type == 41 && !(ptr->msound == MS_BOSS)) || (type == 42 && !(ptr->msound == MS_SOUND)) || (type == 43 && !(ptr->mresists & MR_FIRE)) || (type == 44 && !(ptr->mresists & MR_COLD)) || (type == 45 && !(ptr->mresists & MR_SLEEP)) || (type == 46 && !(ptr->mresists & MR_DISINT)) || (type == 47 && !(ptr->mresists & MR_ELEC)) || (type == 48 && !(ptr->mresists & MR_POISON)) || (type == 49 && !(ptr->mresists & MR_ACID)) || (type == 50 && !(ptr->mresists & MR_STONE)) || (type == 51 && !(ptr->mresists & MR_DEATH)) || (type == 52 && !(ptr->mresists & MR_DRAIN)) || (type == 53 && !(ptr->mresists & MR_PLUSONE)) || (type == 54 && !(ptr->mresists & MR_PLUSTWO)) || (type == 55 && !(ptr->mresists & MR_PLUSTHREE)) || (type == 56 && !(ptr->mresists & MR_PLUSFOUR)) || (type == 57 && !(ptr->mresists & MR_HITASONE)) || (type == 58 && !(ptr->mresists & MR_HITASTWO)) || (type == 59 && !(ptr->mresists & MR_HITASTHREE)) || (type == 60 && !(ptr->mresists & MR_HITASFOUR)) || (type == 61 && !(ptr->mflags1 & M1_FLY)) || (type == 62 && !(ptr->mflags1 & M1_SWIM)) || (type == 63 && !(ptr->mflags1 & M1_AMORPHOUS)) || (type == 64 && !(ptr->mflags1 & M1_WALLWALK)) || (type == 65 && !(ptr->mflags1 & M1_CLING)) || (type == 66 && !(ptr->mflags1 & M1_TUNNEL)) || (type == 67 && !(ptr->mflags1 & M1_NEEDPICK)) || (type == 68 && !(ptr->mflags1 & M1_CONCEAL)) || (type == 69 && !(ptr->mflags1 & M1_HIDE)) || (type == 70 && !(ptr->mflags1 & M1_AMPHIBIOUS)) || (type == 71 && !(ptr->mflags1 & M1_BREATHLESS)) || (type == 72 && !(ptr->mflags1 & M1_NOTAKE)) || (type == 73 && !(ptr->mflags1 & M1_NOEYES)) || (type == 74 && !(ptr->mflags1 & M1_NOHANDS)) || (type == 75 && !(ptr->mflags1 & M1_NOLIMBS)) || (type == 76 && !(ptr->mflags1 & M1_NOHEAD)) || (type == 77 && !(ptr->mflags1 & M1_MINDLESS)) || (type == 78 && !(ptr->mflags1 & M1_HUMANOID)) || (type == 79 && !(ptr->mflags1 & M1_ANIMAL)) || (type == 80 && !(ptr->mflags1 & M1_SLITHY)) || (type == 81 && !(ptr->mflags1 & M1_UNSOLID)) || (type == 82 && !(ptr->mflags1 & M1_THICK_HIDE)) || (type == 83 && !(ptr->mflags1 & M1_OVIPAROUS)) || (type == 84 && !(ptr->mflags1 & M1_REGEN)) || (type == 85 && !(ptr->mflags1 & M1_SEE_INVIS)) || (type == 86 && !(ptr->mflags1 & M1_TPORT)) || (type == 87 && !(ptr->mflags1 & M1_TPORT_CNTRL)) || (type == 88 && !(ptr->mflags1 & M1_ACID)) || (type == 89 && !(ptr->mflags1 & M1_POIS)) || (type == 90 && !(ptr->mflags1 & M1_CARNIVORE)) || (type == 91 && !(ptr->mflags1 & M1_HERBIVORE)) || (type == 92 && !(ptr->mflags1 & M1_OMNIVORE)) || (type == 93 && !(ptr->mflags1 & M1_METALLIVORE)) || (type == 94 && !(ptr->mflags2 & M2_NOPOLY)) || (type == 95 && !(ptr->mflags2 & M2_UNDEAD)) || (type == 96 && !(ptr->mflags2 & M2_MERC)) || (type == 97 && !(ptr->mflags2 & M2_HUMAN)) || (type == 98 && !(ptr->mflags2 & M2_ELF)) || (type == 99 && !(ptr->mflags2 & M2_DWARF)) || (type == 100 && !(ptr->mflags2 & M2_GNOME)) || (type == 101 && !(ptr->mflags2 & M2_ORC)) || (type == 102 && !(ptr->mflags2 & M2_HOBBIT)) || (type == 103 && !(ptr->mflags2 & M2_WERE)) || (type == 104 && !(ptr->mflags2 & M2_VAMPIRE)) || (type == 105 && !(ptr->mflags2 & M2_LORD)) || (type == 106 && !(ptr->mflags2 & M2_PRINCE)) || (type == 107 && !(ptr->mflags2 & M2_MINION)) || (type == 108 && !(ptr->mflags2 & M2_GIANT)) || (type == 109 && !(ptr->mflags2 & M2_DEMON)) || (type == 110 && !(ptr->mflags2 & M2_MALE)) || (type == 111 && !(ptr->mflags2 & M2_FEMALE)) || (type == 112 && !(ptr->mflags2 & M2_NEUTER)) || (type == 113 && !(ptr->mflags2 & M2_PNAME)) || (type == 114 && !(ptr->mflags2 & M2_HOSTILE)) || (type == 115 && !(ptr->mflags2 & M2_PEACEFUL)) || (type == 116 && !(ptr->mflags2 & M2_DOMESTIC)) || (type == 117 && !(ptr->mflags2 & M2_WANDER)) || (type == 118 && !(ptr->mflags2 & M2_STALK)) || (type == 119 && !(ptr->mflags2 & M2_NASTY)) || (type == 120 && !(ptr->mflags2 & M2_STRONG)) || (type == 121 && !(ptr->mflags2 & M2_ROCKTHROW)) || (type == 122 && !(ptr->mflags2 & M2_GREEDY))  || (type == 123 && !(ptr->mflags2 & M2_JEWELS)) || (type == 124 && !(ptr->mflags2 & M2_COLLECT)) || (type == 125 && !(ptr->mflags2 & M2_MAGIC)) || (type == 126 && !(ptr->mflags3 & M3_WANTSAMUL)) || (type == 127 && !(ptr->mflags3 & M3_WANTSBELL)) || (type == 128 && !(ptr->mflags3 & M3_WANTSBOOK)) || (type == 129 && !(ptr->mflags3 & M3_WANTSCAND)) || (type == 130 && !(ptr->mflags3 & M3_WANTSARTI)) || (type == 131 && !(ptr->mflags3 & M3_WAITFORU)) || (type == 132 && !(ptr->mflags3 & M3_CLOSE)) || (type == 133 && !(ptr->mflags3 & M3_INFRAVISION)) || (type == 134 && !(ptr->mflags3 & M3_INFRAVISIBLE)) || (type == 135 && !(ptr->mflags3 & M3_TRAITOR)) || (type == 136 && !(ptr->mflags3 & M3_NOTAME)) || (type == 137 && !(ptr->mflags3 & M3_AVOIDER)) || (type == 138 && !(ptr->mflags3 & M3_LITHIVORE)) || (type == 139 && !(ptr->mflags3 & M3_PETTY)) || (type == 140 && !(ptr->mflags3 & M3_POKEMON)) || (type == 141 && !(ptr->mflags3 & M3_NOPLRPOLY)) || (type == 142 && !(ptr->mflags3 & M3_NONMOVING)) || (type == 143 && !(ptr->mflags3 & M3_EGOTYPE)) || (type == 144 && !(ptr->mflags3 & M3_TELEPATHIC)) || (type == 145 && !(ptr->mflags3 & M3_SPIDER)) || (type == 146 && !(ptr->mflags3 & M3_PETRIFIES)) || (type == 147 && !(ptr->mflags3 & M3_IS_MIND_FLAYER)) || (type == 148 && !(ptr->mflags3 & M3_NO_DECAY)) || (type == 149 && !(ptr->mflags3 & M3_MIMIC)) || (type == 150 && !(ptr->mflags3 & M3_PERMAMIMIC)) || (type == 151 && !(ptr->mflags3 & M3_SLIME)) || (type == 152 && !(ptr->mflags3 & M3_FREQ_UNCOMMON2)) || (type == 153 && !(ptr->mflags3 & M3_FREQ_UNCOMMON3)) || (type == 154 && !(ptr->mflags3 & M3_FREQ_UNCOMMON5)) || (type == 155 && !(ptr->mflags3 & M3_FREQ_UNCOMMON7)) || (type == 156 && !(ptr->mflags3 & M3_FREQ_UNCOMMON10)) || (type == 157 && !(ptr->mflags3 & M3_MIND_FLAYER)) || (type == 158 && !(ptr->msize == MZ_TINY)) || (type == 159 && !(ptr->msize == MZ_SMALL)) || (type == 160 && !(ptr->msize == MZ_MEDIUM)) || (type == 161 && !(ptr->msize == MZ_LARGE)) || (type == 162 && !(ptr->msize == MZ_HUGE)) || (type == 163 && !(ptr->msize == MZ_GIGANTIC)) || (type == 164 && !(ptr->geno & G_VLGROUP)) || (type == 165 && !(ptr->geno & G_UNIQ)) || (type == 166 && !(ptr->geno & G_SGROUP)) || (type == 167 && !(ptr->geno & G_LGROUP)) || (type == 168 && !(ptr->geno & G_GENO)) || (type == 169 && !(ptr->geno & G_NOCORPSE)) || (type == 170 && !haspassive(ptr)) || (type == 171 && !attacktype(ptr, AT_CLAW)) || (type == 172 && !attacktype(ptr, AT_BITE)) || (type == 173 && !attacktype(ptr, AT_KICK)) || (type == 174 && !attacktype(ptr, AT_BUTT)) || (type == 175 && !attacktype(ptr, AT_TUCH)) || (type == 176 && !attacktype(ptr, AT_STNG)) || (type == 177 && !attacktype(ptr, AT_HUGS)) || (type == 178 && !attacktype(ptr, AT_SCRA)) || (type == 179 && !attacktype(ptr, AT_LASH)) || (type == 180 && !attacktype(ptr, AT_SPIT)) || (type == 181 && !attacktype(ptr, AT_ENGL)) || (type == 182 && !attacktype(ptr, AT_BREA)) || (type == 183 && !attacktype(ptr, AT_EXPL)) || (type == 184 && !attacktype(ptr, AT_BOOM)) || (type == 185 && !attacktype(ptr, AT_GAZE)) || (type == 186 && !attacktype(ptr, AT_TENT)) || (type == 187 && !attacktype(ptr, AT_TRAM)) || (type == 188 && !attacktype(ptr, AT_BEAM)) || (type == 189 && !attacktype(ptr, AT_MULTIPLY)) || (type == 190 && !attacktype(ptr, AT_WEAP)) || (type == 191 && !attacktype(ptr, AT_MAGC)) || (type == 192 && !dmgtype(ptr, AD_PHYS)) || (type == 193 && !dmgtype(ptr, AD_MAGM)) || (type == 194 && !dmgtype(ptr, AD_FIRE)) || (type == 195 && !dmgtype(ptr, AD_COLD)) || (type == 196 && !dmgtype(ptr, AD_SLEE)) || (type == 197 && !dmgtype(ptr, AD_DISN)) || (type == 198 && !dmgtype(ptr, AD_ELEC)) || (type == 199 && !dmgtype(ptr, AD_DRST)) || (type == 200 && !dmgtype(ptr, AD_ACID)) || (type == 201 && !dmgtype(ptr, AD_LITE)) || (type == 202 && !dmgtype(ptr, AD_BLND)) || (type == 203 && !dmgtype(ptr, AD_STUN)) || (type == 204 && !dmgtype(ptr, AD_SLOW)) || (type == 205 && !dmgtype(ptr, AD_PLYS)) || (type == 206 && !dmgtype(ptr, AD_DRLI)) || (type == 207 && !dmgtype(ptr, AD_DREN)) || (type == 208 && !dmgtype(ptr, AD_LEGS)) || (type == 209 && !dmgtype(ptr, AD_STON)) || (type == 210 && !dmgtype(ptr, AD_STCK)) || (type == 211 && !dmgtype(ptr, AD_SGLD)) || (type == 212 && !dmgtype(ptr, AD_SITM)) || (type == 213 && !dmgtype(ptr, AD_SEDU)) || (type == 214 && !dmgtype(ptr, AD_TLPT)) || (type == 215 && !dmgtype(ptr, AD_RUST)) || (type == 216 && !dmgtype(ptr, AD_CONF)) || (type == 217 && !dmgtype(ptr, AD_DGST)) || (type == 218 && !dmgtype(ptr, AD_HEAL)) || (type == 219 && !dmgtype(ptr, AD_WRAP)) || (type == 220 && !dmgtype(ptr, AD_WERE)) || (type == 221 && !dmgtype(ptr, AD_DRDX)) || (type == 222 && !dmgtype(ptr, AD_DRCO)) || (type == 223 && !dmgtype(ptr, AD_DRIN)) || (type == 224 && !dmgtype(ptr, AD_DISE)) || (type == 225 && !dmgtype(ptr, AD_DCAY)) || (type == 226 && !dmgtype(ptr, AD_SSEX)) || (type == 227 && !dmgtype(ptr, AD_HALU)) || (type == 228 && !dmgtype(ptr, AD_DETH)) || (type == 229 && !dmgtype(ptr, AD_PEST)) || (type == 230 && !dmgtype(ptr, AD_FAMN)) || (type == 231 && !dmgtype(ptr, AD_SLIM)) || (type == 232 && !dmgtype(ptr, AD_CALM)) || (type == 233 && !dmgtype(ptr, AD_ENCH)) || (type == 234 && !dmgtype(ptr, AD_POLY)) || (type == 235 && !dmgtype(ptr, AD_CORR)) || (type == 236 && !dmgtype(ptr, AD_TCKL)) || (type == 237 && !dmgtype(ptr, AD_NGRA)) || (type == 238 && !dmgtype(ptr, AD_GLIB)) || (type == 239 && !dmgtype(ptr, AD_DARK)) || (type == 240 && !dmgtype(ptr, AD_WTHR)) || (type == 241 && !dmgtype(ptr, AD_LUCK)) || (type == 242 && !dmgtype(ptr, AD_NUMB)) || (type == 243 && !dmgtype(ptr, AD_FRZE)) || (type == 244 && !dmgtype(ptr, AD_DISP)) || (type == 245 && !dmgtype(ptr, AD_BURN)) || (type == 246 && !dmgtype(ptr, AD_FEAR)) || (type == 247 && !dmgtype(ptr, AD_NPRO)) || (type == 248 && !dmgtype(ptr, AD_POIS)) || (type == 249 && !dmgtype(ptr, AD_THIR)) || (type == 250 && !dmgtype(ptr, AD_LAVA)) || (type == 251 && !dmgtype(ptr, AD_FAKE)) || (type == 252 && !dmgtype(ptr, AD_LETH)) || (type == 253 && !dmgtype(ptr, AD_CNCL)) || (type == 254 && !dmgtype(ptr, AD_BANI)) || (type == 255 && !dmgtype(ptr, AD_WISD)) || (type == 256 && !dmgtype(ptr, AD_SHRD)) || (type == 257 && !dmgtype(ptr, AD_WET)) || (type == 258 && !dmgtype(ptr, AD_SUCK)) || (type == 259 && !dmgtype(ptr, AD_MALK)) || (type == 260 && !dmgtype(ptr, AD_UVUU)) || (type == 261 && !dmgtype(ptr, AD_ABDC)) || (type == 262 && !dmgtype(ptr, AD_AXUS)) || (type == 263 && !dmgtype(ptr, AD_CHKH)) || (type == 264 && !dmgtype(ptr, AD_HODS)) || (type == 265 && !dmgtype(ptr, AD_CHRN)) || (type == 266 && !dmgtype(ptr, AD_WEEP)) || (type == 267 && !dmgtype(ptr, AD_VAMP)) || (type == 268 && !dmgtype(ptr, AD_WEBS)) || (type == 269 && !dmgtype(ptr, AD_STTP)) || (type == 270 && !dmgtype(ptr, AD_DEPR)) || (type == 271 && !dmgtype(ptr, AD_WRAT)) || (type == 272 && !dmgtype(ptr, AD_LAZY)) || (type == 273 && !dmgtype(ptr, AD_DRCH)) || (type == 274 && !dmgtype(ptr, AD_DFOO)) || (type == 275 && !dmgtype(ptr, AD_NEXU)) || (type == 276 && !dmgtype(ptr, AD_SOUN)) || (type == 277 && !dmgtype(ptr, AD_GRAV)) || (type == 278 && !dmgtype(ptr, AD_INER)) || (type == 279 && !dmgtype(ptr, AD_TIME)) || (type == 280 && !dmgtype(ptr, AD_MANA)) || (type == 281 && !dmgtype(ptr, AD_PLAS)) || (type == 282 && !dmgtype(ptr, AD_SKIL)) || (type == 283 && !dmgtype(ptr, AD_CLRC)) || (type == 284 && !dmgtype(ptr, AD_SPEL)) || (type == 285 && !dmgtype(ptr, AD_RBRE)) || (type == 286 && !dmgtype(ptr, AD_SAMU)) || (type == 287 && !dmgtype(ptr, AD_CURS)) || (type == 288 && !dmgtype(ptr, AD_SPC2)) || (type == 289 && !dmgtype(ptr, AD_VENO)) || (type == 290 && !dmgtype(ptr, AD_DREA)) || (type == 291 && !dmgtype(ptr, AD_NAST)) || (type == 292 && !dmgtype(ptr, AD_BADE)) || (type == 293 && !dmgtype(ptr, AD_SLUD)) || (type == 294 && !dmgtype(ptr, AD_ICUR)) || (type == 295 && !dmgtype(ptr, AD_VULN)) || (type == 296 && !dmgtype(ptr, AD_FUMB)) || (type == 297 && !dmgtype(ptr, AD_DIMN)) || (type == 298 && !dmgtype(ptr, AD_AMNE)) || (type == 299 && !dmgtype(ptr, AD_ICEB)) || (type == 300 && !dmgtype(ptr, AD_VAPO)) || (type == 301 && !dmgtype(ptr, AD_EDGE)) || (type == 302 && !dmgtype(ptr, AD_VOMT)) || (type == 303 && !dmgtype(ptr, AD_LITT)) || (type == 304 && !dmgtype(ptr, AD_FREN)) || (type == 305 && !dmgtype(ptr, AD_NGEN)) || (type == 306 && !dmgtype(ptr, AD_CHAO)) || (type == 307 && !dmgtype(ptr, AD_INSA)) || (type == 308 && !dmgtype(ptr, AD_TRAP)) || (type == 309 && !dmgtype(ptr, AD_WGHT)) || (type == 310 && !dmgtype(ptr, AD_NTHR)) || (type == 311 && !dmgtype(ptr, AD_MIDI)) || (type == 312 && !dmgtype(ptr, AD_RNG)) || (type == 313 && !dmgtype(ptr, AD_CAST)) || (type == 314 && !(ptr->mflags4 & M4_BAT)) || (type == 315 && !(ptr->mflags4 & M4_REVIVE)) || (type == 316 && !(ptr->mflags4 & M4_RAT)) || (type == 317 && !(ptr->mflags4 & M4_SHADE)) || (type == 318 && !(ptr->mflags4 & M4_REFLECT)) || (type == 319 && !(ptr->mflags4 & M4_MULTIHUED)) || (type == 320 && !(ptr->mflags4 & M4_TAME)) || (type == 321 && !(ptr->mflags4 & M4_ORGANIVORE)) || (type == 322 && !(ptr->mflags5 & M5_SPACEWARS)) || (type == 323 && !(ptr->mflags5 & M5_JOKE)) || (type == 324 && !(ptr->mflags5 & M5_ANGBAND)) || (type == 325 && !(ptr->mflags5 & M5_STEAMBAND)) || (type == 326 && !(ptr->mflags5 & M5_ANIMEBAND)) || (type == 327 && !(ptr->mflags5 & M5_DIABLO)) || (type == 328 && !(ptr->mflags5 & M5_DLORDS)) || (type == 329 && !(ptr->mflags5 & M5_VANILLA)) || (type == 330 && !(ptr->mflags5 & M5_DNETHACK)) || (type == 331 && !(ptr->mflags5 & M5_RANDOMIZED)) || (type == 332 && !(ptr->msound == MS_SHOE)) || (type == 333 && !(ptr->msound == MS_STENCH)) || (type == 334 && !dmgtype(ptr, AD_ALIN)) || (type == 335 && !dmgtype(ptr, AD_SIN)) || (type == 336 && !dmgtype(ptr, AD_MINA)) || (type == 337 && !dmgtype(ptr, AD_CONT)) || (type == 338 && !dmgtype(ptr, AD_AGGR)) || (type == 339 && !(ptr->mflags5 & M5_JONADAB)) || (type == 340 && !dmgtype(ptr, AD_DATA)) || (type == 341 && !(ptr->mflags5 & M5_EVIL)) || (type == 342 && !(ptr->mflags4 & M4_SHAPESHIFT)) || (type == 343 && !(ptr->mflags4 & M4_GRIDBUG)) || (type == 344 && !dmgtype(ptr, AD_DEST)) || (type == 345 && !dmgtype(ptr, AD_TREM)) || (type == 346 && !dmgtype(ptr, AD_RAGN)) || (type == 347 && !(ptr->msound == MS_CONVERT)) || (type == 348 && !(ptr->msound == MS_HCALIEN)) || (type == 349 && !dmgtype(ptr, AD_IDAM)) || (type == 350 && !dmgtype(ptr, AD_ANTI)) || (type == 351 && !dmgtype(ptr, AD_PAIN)) || (type == 352 && !dmgtype(ptr, AD_TECH)) || (type == 353 && !dmgtype(ptr, AD_MEMO)) || (type == 354 && !dmgtype(ptr, AD_TRAI)) || (type == 355 && !dmgtype(ptr, AD_STAT)) || (type == 356 && !dmgtype(ptr, AD_DAMA)) || (type == 357 && !dmgtype(ptr, AD_THIE)) || (type == 358 && !dmgtype(ptr, AD_PART)) || (type == 359 && !dmgtype(ptr, AD_RUNS)) || (type == 360 && !attacktype(ptr, AT_RATH)) || (type == 361 && !(ptr->mflags5 & M5_ELONA)) || (type == 362 && !dmgtype(ptr, AD_NACU)) || (type == 363 && !dmgtype(ptr, AD_SANI)) || (type == 364 && !dmgtype(ptr, AD_RBAD)) || (type == 365 && !dmgtype(ptr, AD_BLEE)) || (type == 366 && !dmgtype(ptr, AD_SHAN)) || (type == 367 && !dmgtype(ptr, AD_SCOR)) || (type == 368 && !dmgtype(ptr, AD_TERR)) || (type == 369 && !dmgtype(ptr, AD_FEMI)) || (type == 370 && !dmgtype(ptr, AD_LEVI)) || (type == 371 && !dmgtype(ptr, AD_ILLU)) || (type == 372 && !dmgtype(ptr, AD_MCRE)) || (type == 373 && !dmgtype(ptr, AD_FLAM)) || (type == 374 && !dmgtype(ptr, AD_DEBU)) || (type == 375 && !dmgtype(ptr, AD_UNPR)) || (type == 376 && !dmgtype(ptr, AD_NIVE)) || (type == 377 && !dmgtype(ptr, AD_TDRA)) || (type == 378 && !dmgtype(ptr, AD_BLAS)) || (type == 379 && !dmgtype(ptr, AD_DROP)) ) && (ct < 250000) ) );

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
	int depthuz;
	int maxdougdiff = 2000; /* arbitrary */

	if (iszapem && In_ZAPM(&u.uz) && !(u.zapemescape)) {

		d_level zapemlevel;
		int zapemdepth;
		zapemlevel.dnum = dname_to_dnum("Space Base");
		zapemlevel.dlevel = dungeons[zapemlevel.dnum].entry_lev;
		zapemdepth = depth(&zapemlevel);

		depthuz = (1 + depth(&u.uz) - zapemdepth);
		if (depthuz < 1) depthuz = 1; /* fail safe */

	} else if (u.preversionmode && !u.preversionescape && In_greencross(&u.uz)) {

		d_level preverlevel;
		int preverdepth;
		preverlevel.dnum = dname_to_dnum("Green Cross");
		preverlevel.dlevel = dungeons[preverlevel.dnum].entry_lev;
		preverdepth = depth(&preverlevel);

		depthuz = (1 + depth(&u.uz) - preverdepth);
		if (depthuz < 1) depthuz = 1; /* fail safe */
	} else {
		depthuz = depth(&u.uz);
	}

	if ((depthuz < 10) && !In_sokoban_real(&u.uz) && !In_mainframe(&u.uz) && (level_difficulty() < (8 + rn2(3)))) {
		maxdougdiff = depthuz;
	}

	int     i = rn2(60);
	if (i > 55) return((maxdougdiff < 12) ? &mons[PM_TRAIL_BEAST] : &mons[PM_RAVENOUS_BUGBLATTER_BEAST_OF_TRAAL]);
	else if (i > 54 && !rn2(10))        return((maxdougdiff < 9) ? &mons[PM_MRIVAN] : &mons[PM_MARVIN]);
	else if (i > 46)        return(&mons[PM_CREEPING___]);
	else if (i > 26)        return(&mons[PM_MICROSCOPIC_SPACE_FLEET]);
	else if (i > 20)        return((maxdougdiff < 4) ? &mons[PM_BIRDON] : !rn2(5) ? &mons[PM_STUNOGON] : &mons[PM_VOGON]);
	else if (i > 19)        return((maxdougdiff < 14) ? &mons[PM_BIRDON] : &mons[PM_VOGON_LORD]);
	else if (i > 2)        return(&mons[PM_BABELFISH]);
	else                    return((maxdougdiff < 8) ? &mons[PM_MOONTIGER] : &mons[PM_ALGOLIAN_SUNTIGER]);
}

struct permonst *
beehivemon()
{
	int     i = rn2(108);

	if (i > 106) return((level_difficulty() > 40) ? &mons[PM_NEUROBEE] : &mons[PM_KILLER_BEE]);
	else if (i > 105) return((level_difficulty() > 15) ? &mons[PM_VORACIOUS_FORCE_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 104) return((level_difficulty() > 15) ? &mons[PM_THWARTER_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 102) return((level_difficulty() > 15) ? &mons[PM_VORACIOUS_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 101) return((level_difficulty() > 9) ? &mons[PM_HIGHBEE] : &mons[PM_KILLER_BEE]);
	else if (i > 100) return(((level_difficulty() > 9) && !rn2(50)) ? &mons[PM_BAN_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 99) return((level_difficulty() > 6) ? (rn2(5) ? &mons[PM_ZOMBEE] : &mons[PM_SPIKE_ZOMBEE]) : &mons[PM_KILLER_BEE]);
	else if (i > 98) return((level_difficulty() > 6) ? &mons[PM_FUMBLEBEE] : &mons[PM_KILLER_BEE]);
	else if (i > 90) return((level_difficulty() > 6) ? &mons[PM_WING_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 82) return((level_difficulty() > 6) ? &mons[PM_TWIN_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 81) return((level_difficulty() > 6) ? &mons[PM_WEREKILLERBEE] : &mons[PM_KILLER_BEE]);
	else if (i > 80) return((level_difficulty() > 5) ? &mons[PM_GIANT_JELLY_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 70) return((level_difficulty() > 5) ? &mons[PM_GIANT_KILLER_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 69) return((level_difficulty() > 3) ? &mons[PM_SHARP_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 67) return((level_difficulty() > 3) ? &mons[PM_STUNNING_BEE] : &mons[PM_KILLER_BEE]);
	else if (i > 57) return(&mons[PM_THORN_BEE]);
	else if (i > 56) return(&mons[PM_BOTULISM_BEE]);
	else if (i > 55) return(&mons[PM_INJECTING_BEE]);
	else if (i > 54) return(&mons[PM_HUNTER_BEE]);
	else if (i > 53) return(&mons[PM_BUMBLEBEE]);
	else if (i > 52) return(&mons[PM_WEREBEE]);
	else if (i > 51) return(&mons[PM_HONEY_BEE]);
	else if (i > 50) return(&mons[PM_WAX_BEE]);
	else if (i > 49) return(&mons[PM_JELLY_BEE]);
	else return(&mons[PM_KILLER_BEE]);

}

struct permonst *
migohivemon()
{
	int depthuz;

	if (iszapem && In_ZAPM(&u.uz) && !(u.zapemescape)) {

		d_level zapemlevel;
		int zapemdepth;
		zapemlevel.dnum = dname_to_dnum("Space Base");
		zapemlevel.dlevel = dungeons[zapemlevel.dnum].entry_lev;
		zapemdepth = depth(&zapemlevel);

		depthuz = (1 + depth(&u.uz) - zapemdepth);
		if (depthuz < 1) depthuz = 1; /* fail safe */

	} else if (u.preversionmode && !u.preversionescape && In_greencross(&u.uz)) {

		d_level preverlevel;
		int preverdepth;
		preverlevel.dnum = dname_to_dnum("Green Cross");
		preverlevel.dlevel = dungeons[preverlevel.dnum].entry_lev;
		preverdepth = depth(&preverlevel);

		depthuz = (1 + depth(&u.uz) - preverdepth);
		if (depthuz < 1) depthuz = 1; /* fail safe */
	} else {
		depthuz = depth(&u.uz);
	}

	if ((depthuz < 10) && !In_sokoban_real(&u.uz) && !In_mainframe(&u.uz) && (level_difficulty() < (5 + rn2(5)))) return (&mons[PM_LITTLE_MIGO]);

	if (rn2(8)) return (rn2(2) ? &mons[PM_MIGO_WARRIOR] : &mons[PM_MIGO_DRONE]);
	else if (!rn2(4)) return (rn2(2) ? &mons[PM_MIGO_FORCE_DRONE] : &mons[PM_ARMED_MIGO_DRONE]);
	else switch (rnd(15)) {

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
		case 15:
			return &mons[PM_MI_GO_OPERATOR];
		default:
			return &mons[PM_MIGO_DRONE];

	}
}

struct permonst *
realzoomon()
{
	int depthuz;

	if (iszapem && In_ZAPM(&u.uz) && !(u.zapemescape)) {

		d_level zapemlevel;
		int zapemdepth;
		zapemlevel.dnum = dname_to_dnum("Space Base");
		zapemlevel.dlevel = dungeons[zapemlevel.dnum].entry_lev;
		zapemdepth = depth(&zapemlevel);

		depthuz = (1 + depth(&u.uz) - zapemdepth);
		if (depthuz < 1) depthuz = 1; /* fail safe */

	} else if (u.preversionmode && !u.preversionescape && In_greencross(&u.uz)) {

		d_level preverlevel;
		int preverdepth;
		preverlevel.dnum = dname_to_dnum("Green Cross");
		preverlevel.dlevel = dungeons[preverlevel.dnum].entry_lev;
		preverdepth = depth(&preverlevel);

		depthuz = (1 + depth(&u.uz) - preverdepth);
		if (depthuz < 1) depthuz = 1; /* fail safe */
	} else {
		depthuz = depth(&u.uz);
	}

	if ((depthuz < 6) && !In_sokoban_real(&u.uz) && !In_mainframe(&u.uz) && (level_difficulty() < (5 + rn2(2)))) {
		switch (rnd(3)) {
			case 1: return(&mons[PM_MONKEY]);
			case 2: return(&mons[PM_ROTHE]);
			case 3: return(&mons[PM_SMALL_LYNX]);
		}
	}

	int     i = rn2(60) + rn2(3*level_difficulty());
	if (i > 175 && !rn2(50))    return(&mons[PM_JUMBO_THE_ELEPHANT]);
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

STATIC_OVL struct permonst *
prisonermon()	/* return random prisoner type --Amy */
{
	if (rn2(5)) return (&mons[PM_PRISONER]);
	else switch (rnd(7)) {
		case 1: return (&mons[PM_CASTLE_PRISONER]);
		case 2: return (&mons[PM_OCCASIONAL_FRIEND]);
		case 3: return (&mons[PM_GIRL_OUTSIDE_GANG]);
		case 4: return (&mons[PM_YOUR_BROTHER]);
		case 5: return (&mons[PM_YOUR_SISTER]);
		case 6: return (&mons[PM_GRAVITY_STRIKER]);
		case 7: return (&mons[PM_POEZ_PRESIDENT]);
	}
}

STATIC_OVL struct permonst *
squadmon()		/* return soldier types. */
{
	int sel_prob, i, mndx;

	sel_prob = rnd(80+level_difficulty());

	int depthuz;

	if (iszapem && In_ZAPM(&u.uz) && !(u.zapemescape)) {

		d_level zapemlevel;
		int zapemdepth;
		zapemlevel.dnum = dname_to_dnum("Space Base");
		zapemlevel.dlevel = dungeons[zapemlevel.dnum].entry_lev;
		zapemdepth = depth(&zapemlevel);

		depthuz = (1 + depth(&u.uz) - zapemdepth);
		if (depthuz < 1) depthuz = 1; /* fail safe */

	} else if (u.preversionmode && !u.preversionescape && In_greencross(&u.uz)) {

		d_level preverlevel;
		int preverdepth;
		preverlevel.dnum = dname_to_dnum("Green Cross");
		preverlevel.dlevel = dungeons[preverlevel.dnum].entry_lev;
		preverdepth = depth(&preverlevel);

		depthuz = (1 + depth(&u.uz) - preverdepth);
		if (depthuz < 1) depthuz = 1; /* fail safe */
	} else {
		depthuz = depth(&u.uz);
	}

	if ((depthuz < 8) && !In_sokoban_real(&u.uz) && !In_mainframe(&u.uz) && (level_difficulty() < (4 + rn2(2)))) {
		switch (rnd(5)) {
			case 1: mndx = PM_UNARMORED_SOLDIER; break;
			case 2: mndx = PM_WEAKISH_SOLDIER; break;
			case 3: mndx = PM_NEWBIE_SOLDIER; break;
			case 4: mndx = PM_LUSH_SOLDIER; break;
			case 5: mndx = PM_RECRUIT_SOLDIER; break;
		}
		goto gotone;
	}

	if (sel_prob < 81) {
		switch (rnd(18)) {
			case 1: mndx = PM_SOLDIER; break;
			case 2: mndx = PM_TEUTON_SOLDIER; break;
			case 3: mndx = PM_FRANKISH_SOLDIER; break;
			case 4: mndx = PM_BRITISH_SOLDIER; break;
			case 5: mndx = PM_AMERICAN_SOLDIER; break;
			case 6: mndx = PM_ARAB_SOLDIER; break;
			case 7: mndx = PM_ASIAN_SOLDIER; break;
			case 8: mndx = PM_SEAFARING_SOLDIER; break;
			case 9: mndx = PM_BYZANTINE_SOLDIER; break;
			case 10: mndx = PM_CELTIC_SOLDIER; break;
			case 11: mndx = PM_VANILLA_SOLDIER; break;
			case 12: mndx = PM_VIKING_SOLDIER; break;
			case 13: mndx = PM_SWAMP_SOLDIER; break;
			case 14: mndx = PM_JAVA_SOLDIER; break;
			case 15: mndx = PM_IBERIAN_SOLDIER; break;
			case 16: mndx = PM_ROHIRRIM_SOLDIER; break;
			case 17: mndx = PM_GAUCHE_SOLDIER; break;
			case 18: mndx = PM_PAD_SOLDIER; break;
		}
		goto gotone;
	}
	if (sel_prob < 96) {
		switch (rnd(7)) {
			case 1: mndx = PM_SERGEANT; break;
			case 2: mndx = PM_EXTRATERRESTRIAL_SERGEANT; break;
			case 3: mndx = PM_MINOAN_SERGEANT; break;
			case 4: mndx = PM_HUN_SERGEANT; break;
			case 5: mndx = PM_MONGOL_SERGEANT; break;
			case 6: mndx = PM_PERSIAN_SERGEANT; break;
			case 7: mndx = PM_TWOWEAP_SERGEANT; break;
		}
		goto gotone;
	}
	if (sel_prob < 100) {
		switch (rnd(4)) {
			case 1: mndx = PM_LIEUTENANT; break;
			case 2: mndx = PM_YAMATO_LIEUTENANT; break;
			case 3: mndx = PM_CARTHAGE_LIEUTENANT; break;
			case 4: mndx = PM_ROMAN_LIEUTENANT; break;
		}
		goto gotone;
	}
	if (sel_prob > 120 && !rn2(3)) {
		mndx = PM_GENERAL;
		goto gotone;
	}
	mndx = rn2(2) ? PM_CAPTAIN : PM_GOTHIC_CAPTAIN;
	goto gotone;

gotone:
	if (!(mvitals[mndx].mvflags & G_GONE)) return(&mons[mndx]);
	else			    return((struct permonst *) 0);
}

STATIC_OVL struct permonst *
doomsquadmon()
{
	int     i = rn2(60) + rn2(3*level_difficulty());

	int depthuz;

	if (iszapem && In_ZAPM(&u.uz) && !(u.zapemescape)) {

		d_level zapemlevel;
		int zapemdepth;
		zapemlevel.dnum = dname_to_dnum("Space Base");
		zapemlevel.dlevel = dungeons[zapemlevel.dnum].entry_lev;
		zapemdepth = depth(&zapemlevel);

		depthuz = (1 + depth(&u.uz) - zapemdepth);
		if (depthuz < 1) depthuz = 1; /* fail safe */

	} else if (u.preversionmode && !u.preversionescape && In_greencross(&u.uz)) {

		d_level preverlevel;
		int preverdepth;
		preverlevel.dnum = dname_to_dnum("Green Cross");
		preverlevel.dlevel = dungeons[preverlevel.dnum].entry_lev;
		preverdepth = depth(&preverlevel);

		depthuz = (1 + depth(&u.uz) - preverdepth);
		if (depthuz < 1) depthuz = 1; /* fail safe */
	} else {
		depthuz = depth(&u.uz);
	}

	if ((depthuz < 8) && !In_sokoban_real(&u.uz) && !In_mainframe(&u.uz) && (level_difficulty() < (4 + rn2(3)))) return(&mons[PM_STUNTED_ZOMBIEMAN]);

	if (rn2(4)) return(&mons[PM_ZOMBIEMAN]);
	else if (rn2(3)) return(&mons[PM_FORMER_SERGEANT]);
	else if (rn2(2) && i > 90) return(&mons[PM_WOLFENSTEINER]);
	else if (rn2(2) && i > 115) return(&mons[PM_ARACHNOTRON]);
	else if (rn2(2) && i > 150) return(&mons[PM_MANCUBUS]);
	else return(&mons[PM_ZOMBIEMAN]);
}

struct permonst *
illusionmon()
{

	switch (rnd(33)) {
		case 1:
			return specialtensmon(288); break; /* AD_SPC2 */
		case 2:
			return specialtensmon(203); break; /* AD_STUN */
		case 3:
			return specialtensmon(223); break; /* AD_DRIN */
		case 4:
			return specialtensmon(227); break; /* AD_HALU */
		case 5:
			return specialtensmon(239); break; /* AD_DARK */
		case 6:
			return specialtensmon(246); break; /* AD_FEAR */
		case 7:
			return specialtensmon(251); break; /* AD_FAKE */
		case 8:
			return specialtensmon(252); break; /* AD_LETH */
		case 9:
			return specialtensmon(253); break; /* AD_CNCL */
		case 10:
			return specialtensmon(254); break; /* AD_BANI */
		case 11:
			return specialtensmon(255); break; /* AD_WISD */
		case 12:
			return specialtensmon(264); break; /* AD_HODS */
		case 13:
			return specialtensmon(265); break; /* AD_CHRN */
		case 14:
			return specialtensmon(266); break; /* AD_WEEP */
		case 15:
			return specialtensmon(269); break; /* AD_STTP */
		case 16:
			return specialtensmon(270); break; /* AD_DEPR */
		case 17:
			return specialtensmon(272); break; /* AD_LAZY */
		case 18:
			return specialtensmon(278); break; /* AD_INER */
		case 19:
			return specialtensmon(295); break; /* AD_VULN */
		case 20:
			return specialtensmon(298); break; /* AD_AMNE */
		case 21:
			return specialtensmon(307); break; /* AD_INSA */
		case 22:
			return specialtensmon(308); break; /* AD_TRAP */
		case 23:
			return specialtensmon(342); break; /* M4_SHAPESHIFT */
		case 24:
			return specialtensmon(363); break; /* AD_SANI */
		case 25:
			return specialtensmon(366); break; /* AD_SHAN */
		case 26:
			return specialtensmon(368); break; /* AD_TERR */
		case 27:
			return specialtensmon(369); break; /* AD_FEMI */
		case 28:
			return specialtensmon(370); break; /* AD_LEVI */
		case 29:
			return specialtensmon(371); break; /* AD_ILLU */
		case 30:
			return specialtensmon(372); break; /* AD_MCRE */
		case 31:
			return specialtensmon(374); break; /* AD_DEBU */
		case 32:
			return specialtensmon(375); break; /* AD_UNPR */
		case 33:
			return specialtensmon(378); break; /* AD_BLAS */
		default:
			return specialtensmon(288); break; /* AD_SPC2 */
	}

}

STATIC_OVL struct permonst *
evilroommon()
{
	register struct permonst *ptr;
	register int ct = 0;

	ptr = rndmonst();

	do {

		ptr = rndmonst();
		ct++;
		if (!rn2(2000)) reset_rndmonst(NON_PM);

	} while ( !ptr || ( (!dmgtype(ptr, AD_DISN) && !dmgtype(ptr, AD_STON) && !dmgtype(ptr, AD_DETH) && !dmgtype(ptr, AD_PEST) && !dmgtype(ptr, AD_FAMN) && !dmgtype(ptr, AD_SLIM) && !dmgtype(ptr, AD_WTHR) && !dmgtype(ptr, AD_NPRO) && !dmgtype(ptr, AD_LAVA) && !dmgtype(ptr, AD_LETH) && !dmgtype(ptr, AD_CNCL) && !dmgtype(ptr, AD_BANI) && !dmgtype(ptr, AD_SHRD) && !dmgtype(ptr, AD_WET) && !dmgtype(ptr, AD_SUCK) && !dmgtype(ptr, AD_UVUU) && !dmgtype(ptr, AD_STTP) && !dmgtype(ptr, AD_DEPR) && !dmgtype(ptr, AD_WRAT) && !dmgtype(ptr, AD_DFOO) && !dmgtype(ptr, AD_TIME) && !dmgtype(ptr, AD_VENO) && !dmgtype(ptr, AD_VAPO) && !dmgtype(ptr, AD_EDGE) && !dmgtype(ptr, AD_LITT) && !dmgtype(ptr, AD_FREN) && !dmgtype(ptr, AD_NGEN) && !dmgtype(ptr, AD_CHAO) && !dmgtype(ptr, AD_DEST) && !dmgtype(ptr, AD_ANTI) && !dmgtype(ptr, AD_STAT) && !dmgtype(ptr, AD_RUNS) && !dmgtype(ptr, AD_DAMA) && !dmgtype(ptr, AD_THIE) && !dmgtype(ptr, AD_RAGN) && !dmgtype(ptr, AD_DATA) && !dmgtype(ptr, AD_MINA) && !dmgtype(ptr, AD_RBAD) ) && (ct < 250000) ));

	return ptr;

}

STATIC_OVL struct permonst *
machineroommon()
{
	register struct permonst *ptr;
	register int ct = 0;

	ptr = rndmonst();

	do {

		ptr = rndmonst();
		ct++;
		if (!rn2(2000)) reset_rndmonst(NON_PM);

	} while ( !ptr || (!nonliving(ptr) && (ct < 250000) ));

	return ptr;

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
				(void) maketrap(sx, sy, rtrap, 100, TRUE);
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
			(void) maketrap(sx, sy, BOSS_SPAWNER, 100, FALSE);
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
		(void) mksobj_at(rnd_class(RIGHT_MOUSE_BUTTON_STONE, NASTY_STONE), sx, sy, TRUE, FALSE, FALSE);
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
mkchaosroom()
{

    struct mkroom *sroom;
	register int sx,sy = 0;

    if (!(sroom = pick_room(FALSE))) return;

	if(sroom->rtype != OROOM || (has_upstairs(sroom) && rn2(iswarper ? 10 : 100)) ) return;

    sroom->rtype = CHAOSROOM;

	for(sx = sroom->lx; sx <= sroom->hx; sx++)
	for(sy = sroom->ly; sy <= sroom->hy; sy++) {
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(10)) {
			levl[sx][sy].typ = randomwalltype();
		}
	}

}

void
mkmixedpool()
{

    struct mkroom *sroom;
	register int sx,sy = 0;
	coord mm;

    if (!(sroom = pick_room(FALSE))) return;

	if(sroom->rtype != OROOM || (has_upstairs(sroom) && rn2(iswarper ? 10 : 100)) ) return;

    sroom->rtype = MIXEDPOOL;

	for(sx = sroom->lx; sx <= sroom->hx; sx++)
	for(sy = sroom->ly; sy <= sroom->hy; sy++) {
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(4)) {
			levl[sx][sy].typ = rn2(2) ? MOAT : CRYSTALWATER;
		}
	}

	if (somexy(sroom, &mm)) {
		  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
	}

}

void
mkshowerroom()
{

    struct mkroom *sroom;
	register int sx,sy = 0;

    if (!(sroom = pick_room(FALSE))) return;

	if(sroom->rtype != OROOM || (has_upstairs(sroom) && rn2(iswarper ? 10 : 100)) ) return;

    sroom->rtype = SHOWERROOM;

	level.flags.has_showerroom = 1;

	for(sx = sroom->lx; sx <= sroom->hx; sx++)
	for(sy = sroom->ly; sy <= sroom->hy; sy++) {
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(3)) {
			levl[sx][sy].typ = rn2(2) ? BUBBLES : RAINCLOUD;
		}
	}

}

void
mkcentraltedium()
{

    struct mkroom *sroom;
	register int sx,sy = 0;

    if (!(sroom = pick_room(FALSE))) return;

	if(sroom->rtype != OROOM || (has_upstairs(sroom) && rn2(iswarper ? 10 : 100)) ) return;

    sroom->rtype = CENTRALTEDIUM;

	for(sx = sroom->lx; sx <= sroom->hx; sx++)
	for(sy = sroom->ly; sy <= sroom->hy; sy++) {
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) ) {
			switch (rnd(10)) {
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
					levl[sx][sy].typ = HIGHWAY;
					break;
				case 8:
				case 9:
					levl[sx][sy].typ = GRASSLAND;
					break;
				case 10:
					break;
			}
		}

		if((levl[sx][sy].typ > DBWALL) && !t_at(sx,sy) ) {
			if(!rn2(10)) {
				if(!rn2(10)) {
					if(!rn2(10)) {
						(void) maketrap(sx, sy, randominsidetrap(), 100, TRUE);
					}
					else (void) maketrap(sx, sy, LEVEL_TELEP, 100, TRUE);
				}
				else (void) maketrap(sx, sy, TELEP_TRAP, 100, TRUE);
			}

		}

	}

}

void
mkrampageroom()
{

    struct mkroom *sroom;
	register int sx,sy = 0;

    if (!(sroom = pick_room(FALSE))) return;

	if(sroom->rtype != OROOM || (has_upstairs(sroom) && rn2(iswarper ? 10 : 100)) ) return;

    sroom->rtype = RAMPAGEROOM;

	for(sx = sroom->lx; sx <= sroom->hx; sx++)
	for(sy = sroom->ly; sy <= sroom->hy; sy++) {
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !rn2(10)) {
			levl[sx][sy].typ = ROCKWALL;
		}

		if (!rn2(3)) (void) mksobj_at(BOULDER, sx, sy, TRUE, FALSE, FALSE);
		if (!rn2(3)) (void) maketrap(sx, sy, randomtrap(), 100, TRUE);
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
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);

			while (!rn2(2)) {
			  if (somexy(sroom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
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
					if (!rn2(5)) (void) mkgold(0L, sx, sy);
					for (tryct = rn2(2 + rn2(4)); tryct; tryct--) {
						if (timebasedlowerchance()) {
						    otmp = mkobj(rn2(3) ? COIN_CLASS : RANDOM_CLASS, TRUE, FALSE);
						    if (!otmp) return;
						    curse(otmp);
						    otmp->ox = sx;
						    otmp->oy = sy;
						    add_to_buried(otmp);
						}
					}

				}
			}
			/*else*/ if (!rn2(Role_if(PM_CAMPERSTRIKER) ? 5 : 10)) (void) maketrap(sx, sy, typ2, 100, TRUE);

			if (!rn2(1000)) 	(void) mksobj_at(SWITCHER, sx, sy, TRUE, FALSE, FALSE);
			if (!rn2(Role_if(PM_CAMPERSTRIKER) ? 25 : 100)) 	(void) mksobj_at(UGH_MEMORY_TO_CREATE_INVENTORY, sx, sy, TRUE, FALSE, FALSE);

			if (!rn2(Role_if(PM_CAMPERSTRIKER) ? 20 : 40)) 	(void) makemon(insidemon(), sx, sy, MM_ADJACENTOK|MM_ANGRY);

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
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
		  }

		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) /*&& !nexttodoor(sx,sy)*/) {

	    typ = !rn2(5) ? POOL : !rn2(6) ? MOAT : !rn2(20) ? ICE : !rn2(20) ? GRASSLAND : !rn2(80) ? HIGHWAY : !rn2(20) ? FOUNTAIN : !rn2(30) ? FARMLAND : !rn2(35) ? MOUNTAIN : !rn2(50) ? CRYSTALWATER : !rn2(100) ? TUNNELWALL : !rn2(150) ? WATERTUNNEL : !rn2(6) ? ROCKWALL : !rn2(16) ? TREE : ROOM;

		levl[sx][sy].typ = typ;
		if (typ == FOUNTAIN) 	level.flags.nfountains++;
		if (typ == SINK) 	level.flags.nsinks++;
		if(Role_if(PM_CAMPERSTRIKER) && !rn2(50)) (void) maketrap(sx, sy, randomtrap(), 100, TRUE);
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
				(void) maketrap(sx, sy, (rn2(10) ? STATUE_TRAP : ANIMATION_TRAP), 100, TRUE);
		}

		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		    if(rn2(2)) 
			{
			    struct obj *sobj = mksobj_at(STATUE, sx, sy, TRUE, FALSE, FALSE);

			    if (sobj && !rn2(3) ) {
				for (i = rn2(2 + rn2(4)); i; i--)
					if (timebasedlowerchance()) {
					    (void) add_to_container(sobj, mkobj(RANDOM_CLASS, FALSE, FALSE));
					}
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
