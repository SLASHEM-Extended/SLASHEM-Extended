/*	SCCS Id: @(#)mklev.c	3.4	2001/11/29	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
/* #define DEBUG */	/* uncomment to enable code debugging */

#ifdef DEBUG
# ifdef WIZARD
#define debugpline	if (wizard) pline
# else
#define debugpline	pline
# endif
#endif

/* for UNIX, Rand #def'd to (long)lrand48() or (long)random() */
/* croom->lx etc are schar (width <= int), so % arith ensures that */
/* conversion of result to int is reasonable */


STATIC_DCL void mkfount(int,struct mkroom *);
STATIC_DCL void mksink(struct mkroom *);
STATIC_DCL void mktoilet(struct mkroom *);
STATIC_DCL void mkaltar(struct mkroom *);
STATIC_DCL void mkgrave(struct mkroom *);
STATIC_DCL void mkthrone(struct mkroom *);
STATIC_DCL void mkpentagram(struct mkroom *);
STATIC_DCL void mkwell(struct mkroom *);
STATIC_DCL void mkpoisonedwell(struct mkroom *);
STATIC_DCL void mkwagon(struct mkroom *);
STATIC_DCL void mkburningwagon(struct mkroom *);
STATIC_DCL void mkwoodentable(struct mkroom *);
STATIC_DCL void mkcarvedbed(struct mkroom *);
STATIC_DCL void mkstrawmattress(struct mkroom *);
STATIC_DCL void makevtele(void);
STATIC_DCL void clear_level_structures(void);
STATIC_DCL void makelevel(void);
STATIC_DCL void mineralize(void);
STATIC_DCL boolean bydoor(XCHAR_P,XCHAR_P);
STATIC_DCL struct mkroom *find_branch_room(coord *);
STATIC_DCL struct mkroom *pos_to_room(XCHAR_P, XCHAR_P);
STATIC_DCL boolean place_niche(struct mkroom *,int*,int*,int*);
STATIC_DCL void makeniche(int);
STATIC_DCL void make_niches(void);
STATIC_DCL struct permonst * morguemonX(void);

STATIC_DCL void mkstone(struct mkroom *);
STATIC_DCL void mktree(struct mkroom *);
STATIC_DCL void mkpool(struct mkroom *);
STATIC_DCL void mklavapool(struct mkroom *);
STATIC_DCL void mkironbars(struct mkroom *);
STATIC_DCL void mkdoor(struct mkroom *);
STATIC_DCL void mkice(struct mkroom *);
STATIC_DCL void mkcloud(struct mkroom *);
STATIC_DCL void mkgravewall(struct mkroom *);
STATIC_DCL void mktunnelwall(struct mkroom *);
STATIC_DCL void mkfarmland(struct mkroom *);
STATIC_DCL void mkmountain(struct mkroom *);
STATIC_DCL void mkwatertunnel(struct mkroom *);
STATIC_DCL void mkcrystalwater(struct mkroom *);
STATIC_DCL void mkmoorland(struct mkroom *);
STATIC_DCL void mkurinelake(struct mkroom *);
STATIC_DCL void mkshiftingsand(struct mkroom *);
STATIC_DCL void mkstyxriver(struct mkroom *);
STATIC_DCL void mksnow(struct mkroom *);
STATIC_DCL void mkash(struct mkroom *);
STATIC_DCL void mksand(struct mkroom *);
STATIC_DCL void mkpavedfloor(struct mkroom *);
STATIC_DCL void mkhighway(struct mkroom *);
STATIC_DCL void mkgrassland(struct mkroom *);
STATIC_DCL void mknethermist(struct mkroom *);
STATIC_DCL void mkstalactite(struct mkroom *);
STATIC_DCL void mkcryptfloor(struct mkroom *);
STATIC_DCL void mkbubbles(struct mkroom *);
STATIC_DCL void mkraincloud(struct mkroom *);

STATIC_DCL void mkstoneX(int,struct mkroom *);
STATIC_DCL void mktreeX(int,struct mkroom *);
STATIC_DCL void mkpoolX(int,struct mkroom *);
STATIC_DCL void mklavapoolX(int,struct mkroom *);
STATIC_DCL void mkironbarsX(int,struct mkroom *);
STATIC_DCL void mkiceX(int,struct mkroom *);
STATIC_DCL void mkcloudX(int,struct mkroom *);
STATIC_DCL void mkgravewallX(int,struct mkroom *);
STATIC_DCL void mktunnelwallX(int,struct mkroom *);
STATIC_DCL void mkfarmlandX(int,struct mkroom *);
STATIC_DCL void mkmountainX(int,struct mkroom *);
STATIC_DCL void mkwatertunnelX(int,struct mkroom *);
STATIC_DCL void mkcrystalwaterX(int,struct mkroom *);
STATIC_DCL void mkmoorlandX(int,struct mkroom *);
STATIC_DCL void mkurinelakeX(int,struct mkroom *);
STATIC_DCL void mkshiftingsandX(int,struct mkroom *);
STATIC_DCL void mkstyxriverX(int,struct mkroom *);
STATIC_DCL void mksnowX(int,struct mkroom *);
STATIC_DCL void mkashX(int,struct mkroom *);
STATIC_DCL void mksandX(int,struct mkroom *);
STATIC_DCL void mkpavedfloorX(int,struct mkroom *);
STATIC_DCL void mkhighwayX(int,struct mkroom *);
STATIC_DCL void mkgrasslandX(int,struct mkroom *);
STATIC_DCL void mknethermistX(int,struct mkroom *);
STATIC_DCL void mkstalactiteX(int,struct mkroom *);
STATIC_DCL void mkcryptfloorX(int,struct mkroom *);
STATIC_DCL void mkbubblesX(int,struct mkroom *);
STATIC_DCL void mkraincloudX(int,struct mkroom *);

STATIC_DCL int findrandtype(void);
/*STATIC_DCL int randomwalltype(void);*/

STATIC_PTR int  CFDECLSPEC do_comp(const void *,const void *);

STATIC_DCL void dosdoor(XCHAR_P,XCHAR_P,struct mkroom *,int);
STATIC_DCL void join(int,int,BOOLEAN_P);
STATIC_DCL void do_room_or_subroom(struct mkroom *,int,int,int,int,
				       BOOLEAN_P,SCHAR_P,BOOLEAN_P,BOOLEAN_P,BOOLEAN_P,int);
STATIC_DCL void makerooms(void);
STATIC_DCL void finddpos(coord *,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P);
STATIC_DCL void mkinvpos(XCHAR_P,XCHAR_P,int);

#define create_vault()	create_room(-1, -1, 2, 2, -1, -1, VAULT, TRUE, FALSE, FALSE)
#define init_vault()	vault_x = -1
#define do_vault()	(vault_x != -1)
static xchar		vault_x, vault_y;
boolean goldseen;
static boolean made_branch;	/* used only during level creation */


/* Args must be (const void *) so that qsort will always be happy. */

STATIC_PTR int CFDECLSPEC
do_comp(vx,vy)
const void * vx;
const void * vy;
{
#ifdef LINT
/* lint complains about possible pointer alignment problems, but we know
   that vx and vy are always properly aligned. Hence, the following
   bogus definition:
*/
	return (vx == vy) ? 0 : -1;
#else
	register const struct mkroom *x, *y;

	x = (const struct mkroom *)vx;
	y = (const struct mkroom *)vy;
	if(x->lx < y->lx) return(-1);
	return(x->lx > y->lx);
#endif /* LINT */
}

STATIC_OVL int
findrandtype()
{
retryrandtype:
	switch (rnd(89)) {
		case 1: return COURT;
		case 2: return SWAMP;
		case 3: return BEEHIVE;
		case 4: return MORGUE;
		case 5: return BARRACKS;
		case 6: return ZOO;
		case 7: return REALZOO;
		case 8: return GIANTCOURT;
		case 9: return LEPREHALL;
		case 10: return DRAGONLAIR;
		case 11: return BADFOODSHOP;
		case 12: return COCKNEST;
		case 13: return ANTHOLE;
		case 14: return LEMUREPIT;
		case 15: return MIGOHIVE;
		case 16: return FUNGUSFARM;
		case 17: return CLINIC;
		case 18: return TERRORHALL;
		case 19: return ELEMHALL;
		case 20: return ANGELHALL;
		case 21: return MIMICHALL;
		case 22: return NYMPHHALL;
		case 23: return SPIDERHALL;
		case 24: return TROLLHALL;
		case 25: return HUMANHALL;
		case 26: return GOLEMHALL;
		case 27: return COINHALL;
		case 28: return DOUGROOM;
		case 29: return ARMORY;
		case 30: return TENSHALL;
		case 31: return TRAPROOM;
		case 32: return POOLROOM;
		case 33: return STATUEROOM;
		case 34: return INSIDEROOM;
		case 35: return RIVERROOM;
		case 36: return TEMPLE;
		case 37: return EMPTYNEST;
		case 38: return GRUEROOM;
		case 39: return CRYPTROOM;
		case 40: return TROUBLEZONE;
		case 41: return WEAPONCHAMBER;
		case 42: return HELLPIT;
		case 43: return FEMINISMROOM;
		case 44: return MEADOWROOM;
		case 45: return COOLINGCHAMBER;
		case 46: return VOIDROOM;
		case 47: return HAMLETROOM;
		case 48: return KOPSTATION;
		case 49: return BOSSROOM;
		case 50: return RNGCENTER;
		case 51: return WIZARDSDORM;
		case 52: return DOOMEDBARRACKS;
		case 53: return SLEEPINGROOM;
		case 54: return DIVERPARADISE;
		case 55: return MENAGERIE;
		case 56: 
			if (!rn2(20)) return NASTYCENTRAL;
			else goto retryrandtype;
		case 57: return EMPTYDESERT;
		case 58: return RARITYROOM;
		case 59: return EXHIBITROOM;
		case 60: return PRISONCHAMBER;
		case 61: return NUCLEARCHAMBER;
		case 62: return LEVELSEVENTYROOM;
		case 63: return VARIANTROOM;

		case 64: return EVILROOM;
		case 65: return RELIGIONCENTER;
		case 66: return CHAOSROOM;
		case 67: return CURSEDMUMMYROOM;
		case 68: return MIXEDPOOL;
		case 69: return ARDUOUSMOUNTAIN;
		case 70: return LEVELFFROOM;
		case 71: return VERMINROOM;
		case 72: return MIRASPA;
		case 73: return MACHINEROOM;
		case 74: return SHOWERROOM;
		case 75: return GREENCROSSROOM;
		case 76: return CENTRALTEDIUM;
		case 77: return RUINEDCHURCH;
		case 78: return RAMPAGEROOM;
		case 79: return GAMECORNER;
		case 80: return ILLUSIONROOM;
		case 81: return ROBBERCAVE;
		case 82: return SANITATIONCENTRAL;
		case 83: return PLAYERCENTRAL;
		case 84: return CASINOROOM;
		case 85: return FULLROOM;
		case 86: return LETTERSALADROOM;
		case 87: return THE_AREA_ROOM;
		case 88: return CHANGINGROOM;
		case 89: return QUESTORROOM;
	}

	return EMPTYNEST;
}

int
walkableterrain()
{
	switch (rnd(14)) {
		case 1: return ICE;
		case 2: return CLOUD;
		case 3: return GRAVEWALL;
		case 4: return SNOW;
		case 5: return ASH;
		case 6: return SAND;
		case 7: return PAVEDFLOOR;
		case 8: return HIGHWAY;
		case 9: return GRASSLAND;
		case 10: return NETHERMIST;
		case 11: return STALACTITE;
		case 12: return CRYPTFLOOR;
		case 13: return BUBBLES;
		case 14: return RAINCLOUD;
	}

	/* compiler doesn't like it if control can reach end of a non-void function --Amy */
	return ICE;

}

int
randomwalltype()
{
	switch (rnd(29)) {
		case 1: return TREE;
		case 2: return MOAT;
		case 3: return LAVAPOOL;
		case 4: return IRONBARS;
		case 5: return CORR;
		case 6: return ICE;
		case 7: return CLOUD;
		case 8: return ROCKWALL;
		case 9: return GRAVEWALL;
		case 10: return TUNNELWALL;
		case 11: return FARMLAND;
		case 12: return MOUNTAIN;
		case 13: return WATERTUNNEL;
		case 14: return CRYSTALWATER;
		case 15: return MOORLAND;
		case 16: return URINELAKE;
		case 17: return SHIFTINGSAND;
		case 18: return STYXRIVER;
		case 19: return SNOW;
		case 20: return ASH;
		case 21: return SAND;
		case 22: return PAVEDFLOOR;
		case 23: return HIGHWAY;
		case 24: return GRASSLAND;
		case 25: return NETHERMIST;
		case 26: return STALACTITE;
		case 27: return CRYPTFLOOR;
		case 28: return BUBBLES;
		case 29: return RAINCLOUD;
	}


	// fall-through to appease compiler
	return ROCKWALL;
}

STATIC_OVL void
finddpos(cc, xl,yl,xh,yh)
coord *cc;
xchar xl,yl,xh,yh;
{
	register xchar x, y;

	x = (xl == xh) ? xl : (xl + rn2(xh-xl+1));
	y = (yl == yh) ? yl : (yl + rn2(yh-yl+1));
	if(okdoor(x, y))
		goto gotit;

	for(x = xl; x <= xh; x++) for(y = yl; y <= yh; y++)
		if(okdoor(x, y))
			goto gotit;

	for(x = xl; x <= xh; x++) for(y = yl; y <= yh; y++)
		if(IS_DOOR(levl[x][y].typ) || levl[x][y].typ == SDOOR)
			goto gotit;
	/* cannot find something reasonable -- strange */
	x = xl;
	y = yh;
gotit:
	cc->x = x;
	cc->y = y;
	return;
}

void
sort_rooms()
{
#if defined(SYSV) || defined(DGUX)
	qsort((void *) rooms, (unsigned)nroom, sizeof(struct mkroom), do_comp);
#else
	qsort((void *) rooms, nroom, sizeof(struct mkroom), do_comp);
#endif
}

STATIC_OVL void
do_room_or_subroom(croom, lowx, lowy, hix, hiy, lit, rtype, special, is_room, canbeshaped, roommate)
    register struct mkroom *croom;
    int lowx, lowy;
    register int hix, hiy;
    boolean lit;
    schar rtype;
    boolean special;
    boolean is_room;
    boolean canbeshaped;
    int roommate;
{
	register int x, y;
	struct rm *lev;

	int wallifytype = STONE;
	boolean wallifyxtra = 0;
	if ( !((moves + u.monstertimefinish) % 9357 ) || (!rn2(iswarper ? 100 : 5000))) {

		switch (rnd(28)) {

			case 1:
				wallifytype = TREE; break;
			case 2:
				wallifytype = MOAT; break;
			case 3:
				wallifytype = LAVAPOOL; break;
			case 4:
				wallifytype = IRONBARS; break;
			case 5:
				wallifytype = CORR; break;
			case 6:
				wallifytype = ICE; break;
			case 7:
				wallifytype = CLOUD; break;
			case 8:
				wallifytype = RAINCLOUD; break;
			case 9:
				wallifytype = GRAVEWALL; break;
			case 10:
				wallifytype = TUNNELWALL; break;
			case 11:
				wallifytype = FARMLAND; break;
			case 12:
				wallifytype = MOUNTAIN; break;
			case 13:
				wallifytype = WATERTUNNEL; break;
			case 14:
				wallifytype = CRYSTALWATER; break;
			case 15:
				wallifytype = MOORLAND; break;
			case 16:
				wallifytype = URINELAKE; break;
			case 17:
				wallifytype = SHIFTINGSAND; break;
			case 18:
				wallifytype = STYXRIVER; break;
			case 19:
				wallifytype = SNOW; break;
			case 20:
				wallifytype = ASH; break;
			case 21:
				wallifytype = SAND; break;
			case 22:
				wallifytype = PAVEDFLOOR; break;
			case 23:
				wallifytype = HIGHWAY; break;
			case 24:
				wallifytype = GRASSLAND; break;
			case 25:
				wallifytype = NETHERMIST; break;
			case 26:
				wallifytype = STALACTITE; break;
			case 27:
				wallifytype = CRYPTFLOOR; break;
			case 28:
				wallifytype = BUBBLES; break;
			}

		if (!rn2(50)) wallifyxtra = 1;

	}

	int wallifytypeB = STONE;
	boolean wallifyBxtra = 0;
	if ( !((moves + u.monstertimefinish) % 8357 ) || (!rn2(iswarper ? 200 : 5000))) {

		switch (rnd(28)) {

			case 1:
				wallifytypeB = TREE; break;
			case 2:
				wallifytypeB = MOAT; break;
			case 3:
				wallifytypeB = LAVAPOOL; break;
			case 4:
				wallifytypeB = IRONBARS; break;
			case 5:
				wallifytypeB = CORR; break;
			case 6:
				wallifytypeB = ICE; break;
			case 7:
				wallifytypeB = CLOUD; break;
			case 8:
				wallifytypeB = RAINCLOUD; break;
			case 9:
				wallifytypeB = GRAVEWALL; break;
			case 10:
				wallifytypeB = TUNNELWALL; break;
			case 11:
				wallifytypeB = FARMLAND; break;
			case 12:
				wallifytypeB = MOUNTAIN; break;
			case 13:
				wallifytypeB = WATERTUNNEL; break;
			case 14:
				wallifytypeB = CRYSTALWATER; break;
			case 15:
				wallifytypeB = MOORLAND; break;
			case 16:
				wallifytypeB = URINELAKE; break;
			case 17:
				wallifytypeB = SHIFTINGSAND; break;
			case 18:
				wallifytypeB = STYXRIVER; break;
			case 19:
				wallifytypeB = SNOW; break;
			case 20:
				wallifytypeB = ASH; break;
			case 21:
				wallifytypeB = SAND; break;
			case 22:
				wallifytypeB = PAVEDFLOOR; break;
			case 23:
				wallifytypeB = HIGHWAY; break;
			case 24:
				wallifytypeB = GRASSLAND; break;
			case 25:
				wallifytypeB = NETHERMIST; break;
			case 26:
				wallifytypeB = STALACTITE; break;
			case 27:
				wallifytypeB = CRYPTFLOOR; break;
			case 28:
				wallifytypeB = BUBBLES; break;
		}

		if (!rn2(50)) wallifyBxtra = 1;

	}

	croom->colouur = 0;
	if (/*!special && */rtype == OROOM) croom->colouur = (!rn2(20) ? 20 : rnd(15) );
	if (/*!special && */rtype >= SHOPBASE) croom->colouur = (!rn2(20) ? 20 : rnd(15) );
	if (/*!special && */rtype == BEEHIVE) croom->colouur = CLR_YELLOW;
	if (/*!special && */rtype == COURT) croom->colouur = CLR_MAGENTA;
	if (/*!special && */rtype == SWAMP) croom->colouur = CLR_GREEN;
	if (/*!special && */rtype == VAULT) croom->colouur = CLR_YELLOW;
	if (/*!special && */rtype == MORGUE) croom->colouur = CLR_BLACK;
	if (/*!special && */rtype == BARRACKS) croom->colouur = CLR_RED;
	if (/*!special && */rtype == ZOO) croom->colouur = CLR_BROWN;
	if (/*!special && */rtype == REALZOO) croom->colouur = CLR_BROWN;
	if (/*!special && */rtype == DELPHI) croom->colouur = CLR_BRIGHT_BLUE;
	if (/*!special && */rtype == TEMPLE) croom->colouur = CLR_CYAN;
	if (/*!special && */rtype == GIANTCOURT) croom->colouur = CLR_CYAN;
	if (/*!special && */rtype == LEPREHALL) croom->colouur = CLR_GREEN;
	if (/*!special && */rtype == DRAGONLAIR) croom->colouur = CLR_BRIGHT_MAGENTA;
	if (/*!special && */rtype == BADFOODSHOP) croom->colouur = CLR_RED;
	if (/*!special && */rtype == COCKNEST) croom->colouur = CLR_YELLOW;
	if (/*!special && */rtype == ANTHOLE) croom->colouur = CLR_BROWN;
	if (/*!special && */rtype == LEMUREPIT) croom->colouur = CLR_BLACK;
	if (/*!special && */rtype == MIGOHIVE) croom->colouur = CLR_BRIGHT_GREEN;
	if (/*!special && */rtype == FUNGUSFARM) croom->colouur = CLR_BRIGHT_GREEN;
	if (/*!special && */rtype == CLINIC) croom->colouur = CLR_ORANGE;
	if (/*!special && */rtype == TERRORHALL) croom->colouur = CLR_BRIGHT_CYAN;
	if (/*!special && */rtype == RIVERROOM) croom->colouur = CLR_BRIGHT_BLUE;
	if (/*!special && */rtype == ELEMHALL) croom->colouur = CLR_GRAY;
	if (/*!special && */rtype == ANGELHALL) croom->colouur = CLR_WHITE;
	if (/*!special && */rtype == NYMPHHALL) croom->colouur = CLR_GREEN;
	if (/*!special && */rtype == SPIDERHALL) croom->colouur = CLR_GRAY;
	if (/*!special && */rtype == TROLLHALL) croom->colouur = CLR_BROWN;
	if (/*!special && */rtype == HUMANHALL) croom->colouur = CLR_BRIGHT_BLUE;
	if (/*!special && */rtype == GOLEMHALL) croom->colouur = CLR_GRAY;
	if (/*!special && */rtype == COINHALL) croom->colouur = CLR_YELLOW;
	if (/*!special && */rtype == DOUGROOM) croom->colouur = CLR_BRIGHT_CYAN;
	if (/*!special && */rtype == ARMORY) croom->colouur = CLR_CYAN;
	if (/*!special && */rtype == TENSHALL) croom->colouur = 20;
	if (/*!special && */rtype == INSIDEROOM) croom->colouur = 20;
	if (/*!special && */rtype == POOLROOM) croom->colouur = CLR_BRIGHT_BLUE;
	if (/*!special && */rtype == EMPTYNEST) croom->colouur = (!rn2(20) ? 20 : rnd(15) );
	if (/*!special && */rtype == CRYPTROOM) croom->colouur = CLR_BRIGHT_BLUE;
	if (/*!special && */rtype == TROUBLEZONE) croom->colouur = CLR_MAGENTA;
	if (/*!special && */rtype == WEAPONCHAMBER) croom->colouur = CLR_BROWN;
	if (/*!special && */rtype == HELLPIT) croom->colouur = CLR_ORANGE;
	if (/*!special && */rtype == ROBBERCAVE) croom->colouur = CLR_GRAY;
	if (/*!special && */rtype == SANITATIONCENTRAL) croom->colouur = CLR_CYAN;
	if (/*!special && */rtype == GRUEROOM) croom->colouur = (!rn2(20) ? 20 : rnd(15) );
	if (/*!special && */rtype == FEMINISMROOM) croom->colouur = CLR_BRIGHT_MAGENTA;
	if (/*!special && */rtype == MEADOWROOM) croom->colouur = CLR_BRIGHT_GREEN;
	if (/*!special && */rtype == COOLINGCHAMBER) croom->colouur = CLR_BRIGHT_CYAN;
	if (/*!special && */rtype == VOIDROOM) croom->colouur = CLR_MAGENTA;
	if (/*!special && */rtype == HAMLETROOM) croom->colouur = CLR_GREEN;
	if (/*!special && */rtype == KOPSTATION) croom->colouur = CLR_MAGENTA;
	if (/*!special && */rtype == BOSSROOM) croom->colouur = 20;
	if (/*!special && */rtype == RNGCENTER) croom->colouur = 20;
	if (/*!special && */rtype == WIZARDSDORM) croom->colouur = CLR_CYAN;
	if (/*!special && */rtype == DOOMEDBARRACKS) croom->colouur = CLR_BLACK;
	if (/*!special && */rtype == SLEEPINGROOM) croom->colouur = CLR_GRAY;
	if (/*!special && */rtype == DIVERPARADISE) croom->colouur = CLR_BRIGHT_BLUE;
	if (/*!special && */rtype == MENAGERIE) croom->colouur = CLR_YELLOW;
	if (/*!special && */rtype == NASTYCENTRAL) croom->colouur = (!rn2(20) ? 20 : rnd(15) );
	if (/*!special && */rtype == EMPTYDESERT) croom->colouur = CLR_BROWN;
	if (/*!special && */rtype == RARITYROOM) croom->colouur = CLR_YELLOW;
	if (/*!special && */rtype == EXHIBITROOM) croom->colouur = CLR_RED;
	if (/*!special && */rtype == PRISONCHAMBER) croom->colouur = CLR_WHITE;
	if (/*!special && */rtype == NUCLEARCHAMBER) croom->colouur = CLR_ORANGE;
	if (/*!special && */rtype == LEVELSEVENTYROOM) croom->colouur = CLR_BLACK;
	if (/*!special && */rtype == PLAYERCENTRAL) croom->colouur = CLR_MAGENTA;
	if (/*!special && */rtype == VARIANTROOM) croom->colouur = 20;
	if (/*!special && */rtype == EVILROOM) croom->colouur = 21; /* magenta or bright magenta */
	if (/*!special && */rtype == RELIGIONCENTER) croom->colouur = CLR_BLACK;
	if (/*!special && */rtype == CHAOSROOM) croom->colouur = 20;
	if (/*!special && */rtype == CURSEDMUMMYROOM) croom->colouur = CLR_CYAN;
	if (/*!special && */rtype == MIXEDPOOL) croom->colouur = 22; /* bright blue or bright cyan */
	if (/*!special && */rtype == ARDUOUSMOUNTAIN) croom->colouur = CLR_WHITE;
	if (/*!special && */rtype == CHANGINGROOM) croom->colouur = CLR_BRIGHT_MAGENTA;
	if (/*!special && */rtype == QUESTORROOM) croom->colouur = CLR_MAGENTA;
	if (/*!special && */rtype == LEVELFFROOM) croom->colouur = CLR_GRAY;
	if (/*!special && */rtype == VERMINROOM) croom->colouur = CLR_BROWN;
	if (/*!special && */rtype == MIRASPA) croom->colouur = CLR_YELLOW;
	if (/*!special && */rtype == MACHINEROOM) croom->colouur = CLR_GRAY;
	if (/*!special && */rtype == SHOWERROOM) croom->colouur = CLR_BRIGHT_BLUE;
	if (/*!special && */rtype == GREENCROSSROOM) croom->colouur = CLR_BRIGHT_GREEN;
	if (/*!special && */rtype == CENTRALTEDIUM) croom->colouur = CLR_GREEN;
	if (/*!special && */rtype == RUINEDCHURCH) croom->colouur = CLR_BLACK;
	if (/*!special && */rtype == RAMPAGEROOM) croom->colouur = CLR_GRAY;
	if (/*!special && */rtype == GAMECORNER) croom->colouur = CLR_BRIGHT_BLUE;
	if (/*!special && */rtype == ILLUSIONROOM) croom->colouur = 23; /* white, gray or cyan */

	/* locations might bump level edges in wall-less rooms */
	/* add/subtract 1 to allow for edge locations */
	if(!lowx) lowx++;
	if(!lowy) lowy++;
	if(hix >= COLNO-1) hix = COLNO-2;
	if(hiy >= ROWNO-1) hiy = ROWNO-2;

	if(lit) {
		for(x = lowx-1; x <= hix+1; x++) {
			lev = &levl[x][max(lowy-1,0)];
			for(y = lowy-1; y <= hiy+1; y++)
				lev++->lit = 1;
		}
		croom->rlit = 1;
	} else
		croom->rlit = 0;

	croom->lx = lowx;
	croom->hx = hix;
	croom->ly = lowy;
	croom->hy = hiy;
	croom->rtype = rtype;
	croom->doorct = 0;
	/* if we're not making a vault, doorindex will still be 0
	 * if we are, we'll have problems adding niches to the previous room
	 * unless fdoor is at least doorindex
	 */
	croom->fdoor = doorindex;
	croom->irregular = FALSE;

	croom->nsubrooms = 0;
	croom->sbrooms[0] = (struct mkroom *) 0;
	if (!special) {

	    for(x = lowx-1; x <= hix+1; x++)
		for(y = lowy-1; y <= hiy+1; y += (hiy-lowy+2)) {
		    if (roommate) continue;
		    levl[x][y].typ = (wallifytypeB ? (wallifyBxtra ? randomwalltype() : wallifytypeB) : HWALL);
		    levl[x][y].horizontal = 1;	/* For open/secret doors. */
		}
	    for(x = lowx-1; x <= hix+1; x += (hix-lowx+2))
		for(y = lowy; y <= hiy; y++) {
		    if (roommate) continue;
		    levl[x][y].typ = (wallifytypeB ? (wallifyBxtra ? randomwalltype() : wallifytypeB) : VWALL);
		    levl[x][y].horizontal = 0;	/* For open/secret doors. */
		}
	    for(x = lowx; x <= hix; x++) {
		lev = &levl[x][lowy];
		for(y = lowy; y <= hiy; y++) {

		    if (roommate && (lev->typ > ROCKWALL)) {
			lev++;
			continue;
		    }
		    if (roommate == 2) continue;

		    lev++->typ = ROOM;

		}
	    }
	    if (is_room && !roommate) {
		levl[lowx-1][lowy-1].typ = (wallifytypeB ? (wallifyBxtra ? randomwalltype() : wallifytypeB) : TLCORNER);
		levl[hix+1][lowy-1].typ = (wallifytypeB ? (wallifyBxtra ? randomwalltype() : wallifytypeB) : TRCORNER);
		levl[lowx-1][hiy+1].typ = (wallifytypeB ? (wallifyBxtra ? randomwalltype() : wallifytypeB) : BLCORNER);
		levl[hix+1][hiy+1].typ = (wallifytypeB ? (wallifyBxtra ? randomwalltype() : wallifytypeB) : BRCORNER);
	    }

        if (canbeshaped && (hix - lowx > 3) && (hiy - lowy > 3) && ( !((moves + u.monstertimefinish) % 327 ) || (rnd(u.shaperoomchance) < 5 ) || (isnullrace && (rnd(u.shaperoomchance) < 5 ) ) ) )  {  
            int xcmax = 0, ycmax = 0, xcut = 0, ycut = 0;  
            boolean dotl = FALSE, dotr = FALSE, dobl = FALSE, dobr = FALSE, docenter = FALSE;  
            switch (rnd(9)) {
            case 1:
            case 2:
            case 3:
                /* L-shaped */  
                xcmax = (hix - lowx) * 2 / 3;
                ycmax = (hiy - lowy) * 2 / 3;

                switch(rn2(4)) {  
                case 1:  
                    dotr = TRUE;  
                    break;  
                case 2:  
                    dobr = TRUE;  
                    break;  
                case 3:  
                    dotl = TRUE;  
                    break;  
                default:  
                    dobl = TRUE;  
                    break;  
                }  
                break;  
            case 4:
            case 5:
                /* T-shaped */  
                xcmax = (hix - lowx) / 2;
                ycmax = (hiy - lowy) / 2;

                switch(rn2(4)) {  
                case 1:  
                    dotr = TRUE;  
                    dotl = TRUE;  
                case 2:  
                    dobr = TRUE;  
                    dobl = TRUE;  
                case 3:  
                    dotr = TRUE;  
                    dobr = TRUE;  
                default:  
                    dotl = TRUE;  
                    dobl = TRUE;  
                }  
                break;  
            case 6:
                /* S/Z shaped ("Tetris Piece") */  
                xcmax = (hix - lowx) / 2;
                ycmax = (hiy - lowy) / 2;

                switch(rn2(2)) {  
                case 1:  
                    dotr = TRUE;  
                    dobl = TRUE;  
                default:  
                    dotl = TRUE;  
                    dobr = TRUE;  
                }  
                break;  
            case 7:
                /* Plus Shaped */  
                xcmax = (hix - lowx) * 3 / 7;
                ycmax = (hiy - lowy) * 3 / 7;

                dotr = TRUE;  
                dotl = TRUE;  
                dobr = TRUE;  
                dobl = TRUE;  

			if (!rn2(5)) {
				switch (rnd(4)) {
					case 1:
						dotr = FALSE;
						break;
					case 2:
						dotl = FALSE;
						break;
					case 3:
						dobr = FALSE;
						break;
					case 4:
						dobl = FALSE;
						break;
				}
			}

                break;  
            case 8:
                /* square-O shaped (pillar cut out of middle) */  
                xcmax = (hix - lowx) / 2;
                ycmax = (hiy - lowy) / 2;

                docenter = TRUE;  
                break;  
            case 9:
                /* X-shaped */  
                xcmax = (hix - lowx) / 4;
                ycmax = (hiy - lowy) / 4;

                dotr = TRUE;  
                dotl = TRUE;  
                dobr = TRUE;  
                dobl = TRUE;  
                docenter = TRUE;  

			if (!rn2(5)) {
				switch (rnd(4)) {
					case 1:
						dotr = FALSE;
						break;
					case 2:
						dotl = FALSE;
						break;
					case 3:
						dobr = FALSE;
						break;
					case 4:
						dobl = FALSE;
						break;
				}
			}

                break;  
                /* TODO: oval */  
            default:  
                /* Rectangular -- nothing to do */  
                break;  
            }  
            if (dotr || dotl || dobr || dobl || docenter) {  
                xcut = 1 + rn2(xcmax);  
                ycut = 1 + rn2(ycmax);  
                /* Sometimes, instead of a small cut, do a max cut.  
                   This improves the probability of a larger cut,  
                   without removing the possibility for small ones. */  
                if ((xcut < (xcmax / 2)) && !rn2(3))  
                    xcut = xcmax;  
                if ((ycut < (ycmax / 2)) && !rn2(3))  
                    ycut = ycmax;  

            }  
            /* Now do the actual cuts. */  
            if (dotr) {  
                /* top-right cutout */  
                for (y = 0; y < ycut; y++) {  
                    for (x = 0; x < xcut; x++) {  
				levl[hix + 1 - x][lowy + y - 1].typ = (wallifyxtra ? randomwalltype() : wallifytype);
                    }  
                    levl[hix + 1 - xcut][lowy + y - 1].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : VWALL);
                }  
                for (x = 0; x < xcut; x++)  
                    levl[hix + 1 - x][lowy + ycut - 1].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : HWALL);
                levl[hix + 1 - xcut][lowy + ycut - 1].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : BLCORNER);
                levl[hix + 1][lowy + ycut - 1].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : TRCORNER);
                levl[hix + 1 - xcut][lowy - 1].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : TRCORNER);
            }  
            if (dobr) {  
                /* bottom-right cutout */  
                for (y = 0; y < ycut; y++) {  
                    for (x = 0; x < xcut; x++) {  
                        levl[hix + 1 - x][hiy + 1 - y].typ = (wallifyxtra ? randomwalltype() : wallifytype);
                    }  
                    levl[hix + 1 - xcut][hiy + 1 - y].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : VWALL);
                }  
                for (x = 0; x < xcut; x++)  
                    levl[hix + 1 - x][hiy + 1 - ycut].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : HWALL);
                levl[hix + 1 - xcut][hiy + 1 - ycut].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : TLCORNER);
                levl[hix + 1][hiy + 1 - ycut].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : BRCORNER);
                levl[hix + 1 - xcut][hiy + 1].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : BRCORNER);
            }  
            if (dotl) {  
                /* top-left cutout */  
                for (y = 0; y < ycut; y++) {  
                    for (x = 0; x < xcut; x++) {  
                        levl[lowx + x - 1][lowy + y - 1].typ = (wallifyxtra ? randomwalltype() : wallifytype);
                    }  
                    levl[lowx + xcut - 1][lowy + y - 1].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : VWALL);
                }  
                for (x = 0; x < xcut; x++)  
                    levl[lowx + x - 1][lowy + ycut - 1].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : HWALL);
                levl[lowx + xcut - 1][lowy + ycut - 1].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : BRCORNER);
                levl[lowx - 1][lowy + ycut - 1].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : TLCORNER);
                levl[lowx + xcut - 1][lowy - 1].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : TLCORNER);
            }  
            if (dobl) {  
                /* bottom-left cutout */  
                for (y = 0; y < ycut; y++) {  
                    for (x = 0; x < xcut; x++) {  
                        levl[lowx + x - 1][hiy + 1 - y].typ = (wallifyxtra ? randomwalltype() : wallifytype);
                    }  
                    levl[lowx + xcut - 1][hiy + 1 - y].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : VWALL);
                }  
                for (x = 0; x < xcut; x++)  
                    levl[lowx + x - 1][hiy + 1 - ycut].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : HWALL);
                levl[lowx + xcut - 1][hiy + 1 - ycut].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : TRCORNER);
                levl[lowx - 1][hiy + 1 - ycut].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : BLCORNER);
                levl[lowx + xcut - 1][hiy + 1].typ = (wallifytype ? (wallifyxtra ? randomwalltype() : wallifytype) : BLCORNER);
            }  
            if (docenter) {  
                /* pillar in the middle */  
                int xcenter = lowx + ((hix - lowx) / 2);  
                int ycenter = lowy + ((hiy - lowy) / 2);  
                int xparity = ((hix - lowx) % 2) ? 1 : 0;  
                int yparity = ((hiy - lowy) % 2) ? 1 : 0;  
                int xradius = (xcut + 1) / 2;  
                int yradius = (ycut + 1) / 2;  
                int vcorrmin = xcenter - xradius + 1;  
                int vcorrmax = xcenter + xradius + xparity - 1;  
                int hcorrmin = ycenter - yradius + 1;  
                int hcorrmax = ycenter + yradius + yparity - 1;  
                for (x = xcenter - xradius; x <= xcenter + xradius + xparity; x++) {  
                    for (y = ycenter - yradius; y <= ycenter + yradius + yparity; y++) {  

				if (wallifytype) levl[x][y].typ = wallifyxtra ? randomwalltype() : wallifytype;
				else levl[x][y].typ =  
                            ((x == xcenter - xradius) &&  
                             (y == ycenter - yradius)) ? TLCORNER :  
                            ((x == xcenter - xradius) &&  
                             (y == ycenter + yradius + yparity)) ? BLCORNER :  
                            ((x == xcenter + xradius + xparity) &&  
                             (y == ycenter - yradius)) ? TRCORNER :  
                            ((x == xcenter + xradius + xparity) &&  
                             (y == ycenter + yradius + yparity)) ? BRCORNER :  
                            ((x == xcenter - xradius) ||  
                             (x == xcenter + xradius + xparity)) ? VWALL :  
                            ((y == ycenter - yradius) ||  
                             (y == ycenter + yradius + yparity)) ? HWALL : STONE;  
                    }  
                }  
                if ((vcorrmax - vcorrmin) > 1 && rn2(3)) {  
                    x = vcorrmin + rn2(vcorrmax - vcorrmin);  
                    for (y = ycenter - yradius; y <= ycenter + yradius + yparity; y++) {  
                        levl[x][y].typ =  
                            ((y == ycenter - yradius) ||  
                             (y == ycenter + yradius + yparity)) ? SDOOR : SCORR;  
                        if (levl[x][y].typ == SDOOR) {  
                            levl[x][y].horizontal = 1;  
                        }  
                    }  
                }  
                if ((hcorrmax - hcorrmin) > 1 && rn2(3)) {
                    y = hcorrmin + rn2(hcorrmax - hcorrmin);  
                    for (x = xcenter - xradius; x <= xcenter + xradius + xparity; x++) {  
                        levl[x][y].typ =  
                            ((x == xcenter - xradius) ||  
                             (x == xcenter + xradius + xparity)) ? SDOOR : SCORR;  
                    }  
                }  
		}
          }  
	    if (!is_room) {	/* a subroom */
		wallification(lowx-1, lowy-1, hix+1, hiy+1, rn2(iswarper ? 10 : 200) ? FALSE : TRUE);
	    }
	}
}


void
add_room(lowx, lowy, hix, hiy, lit, rtype, special, canbeshaped, roommate)
register int lowx, lowy, hix, hiy;
boolean lit;
schar rtype;
boolean special;
boolean canbeshaped;
int roommate;
{
	register struct mkroom *croom;

	croom = &rooms[nroom];
	do_room_or_subroom(croom, lowx, lowy, hix, hiy, lit,
					    rtype, special, (boolean) TRUE, canbeshaped, roommate);
	croom++;
	croom->hx = -1;
	nroom++;
}

void
add_subroom(proom, lowx, lowy, hix, hiy, lit, rtype, special)
struct mkroom *proom;
register int lowx, lowy, hix, hiy;
boolean lit;
schar rtype;
boolean special;
{
	register struct mkroom *croom;

	croom = &subrooms[nsubroom];
	do_room_or_subroom(croom, lowx, lowy, hix, hiy, lit,
					    rtype, special, (boolean) FALSE, (boolean) FALSE, FALSE);
	proom->sbrooms[proom->nsubrooms++] = croom;
	croom++;
	croom->hx = -1;
	nsubroom++;
}

STATIC_OVL void
makerooms()
{
	boolean tried_vault = FALSE;

	/* make rooms until satisfied */
	/* rnd_rect() will returns 0 if no more rects are available... */
	while(nroom < MAXNROFROOMS && rnd_rect()) {

		if(nroom >=
#ifdef BIGSLEX
		21
#else
		7
#endif
		&& rn2(2) && !tried_vault) {
			tried_vault = TRUE;
			if (create_vault()) {
				vault_x = rooms[nroom].lx;
				vault_y = rooms[nroom].ly;
				rooms[nroom].hx = -1;
			}
		} else
		    if (!create_room(-1, -1, -1, -1, -1, -1, OROOM, -1, TRUE, FALSE) && !rn2(10) )
			return;
	}
	return;
}

STATIC_OVL void
join(a,b,nxcor)
register int a, b;
boolean nxcor;
{
	coord cc,tt, org, dest;
	register xchar tx, ty, xx, yy;
	register struct mkroom *croom, *troom;
	register int dx, dy;

	croom = &rooms[a];
	troom = &rooms[b];

	boolean specialcorridor = 0;
	if (!rn2(iswarper ? 50 : 500)) specialcorridor = 1;

	if (!((moves + u.monstertimefinish) % 5277 )) specialcorridor = 1;

	/* find positions cc and tt for doors in croom and troom
	   and direction for a corridor between them */

	if(troom->hx < 0 || croom->hx < 0 || doorindex >= DOORMAX) return;
	if(troom->lx > croom->hx) {
		dx = 1;
		dy = 0;
		xx = croom->hx+1;
		tx = troom->lx-1;
		finddpos(&cc, xx, croom->ly, xx, croom->hy);
		finddpos(&tt, tx, troom->ly, tx, troom->hy);
	} else if(troom->hy < croom->ly) {
		dy = -1;
		dx = 0;
		yy = croom->ly-1;
		finddpos(&cc, croom->lx, yy, croom->hx, yy);
		ty = troom->hy+1;
		finddpos(&tt, troom->lx, ty, troom->hx, ty);
	} else if(troom->hx < croom->lx) {
		dx = -1;
		dy = 0;
		xx = croom->lx-1;
		tx = troom->hx+1;
		finddpos(&cc, xx, croom->ly, xx, croom->hy);
		finddpos(&tt, tx, troom->ly, tx, troom->hy);
	} else {
		dy = 1;
		dx = 0;
		yy = croom->hy+1;
		ty = troom->ly-1;
		finddpos(&cc, croom->lx, yy, croom->hx, yy);
		finddpos(&tt, troom->lx, ty, troom->hx, ty);
	}
	xx = cc.x;
	yy = cc.y;
	tx = tt.x - dx;
	ty = tt.y - dy;
	if(nxcor && levl[xx+dx][yy+dy].typ)
		return;
	if (okdoor(xx,yy) || !nxcor)
	    dodoor(xx,yy,croom);

	org.x  = xx+dx; org.y  = yy+dy;
	dest.x = tx; dest.y = ty;

	/* KMH -- Support for arboreal levels */

	if (!specialcorridor) {
		if (!dig_corridor(&org, &dest, nxcor,
				level.flags.arboreal ? ROOM : CORR, STONE))
		    return;
	} else {
		if (!dig_corridor(&org, &dest, nxcor, walkableterrain(), STONE))
		    return;
	}

	/* we succeeded in digging the corridor */
	if (okdoor(tt.x, tt.y) || !nxcor)
	    dodoor(tt.x, tt.y, troom);

	if(smeq[a] < smeq[b])
		smeq[b] = smeq[a];
	else
		smeq[a] = smeq[b];
}

void
makecorridors()
{
	/*pline("makecorridors");*/

	int a, b, i;
	boolean any = TRUE;

	int style = 0;
	if (!rn2(5)) style = rnd(6);
#ifdef BIGSLEX
	/* bigslex levels have many more rooms, and the default mechanism is annoying because it constantly makes
	 * large vertical pathways... make it more random --Amy */
	style = rnd(6);
#endif

	/* sporkhack code to have different join mechanisms */
	switch (style) {
	default: /* case 4, vanilla style */

	for(a = 0; a < nroom-1; a++) {
		join(a, a+1, FALSE);
		if(!rn2(50)) break; /* allow some randomness */
	}
	for(a = 0; a < nroom-2; a++)
	    if(smeq[a] != smeq[a+2])
		join(a, a+2, FALSE);
	for(a = 0; any && a < nroom; a++) {
	    any = FALSE;
	    for(b = 0; b < nroom; b++)
		if(smeq[a] != smeq[b]) {
		    join(a, b, FALSE);
		    any = TRUE;
		}
	}
	if(nroom > 2)
	    for(i = rn2(nroom) + 4; i; i--) {
		a = rn2(nroom);
		b = rn2(nroom-2);
		if(b >= a) b += 2;
		join(a, b, TRUE);
	    }

	break;
	case 1: /* at least one corridor leaves from each room and goes to random room */
	    if (nroom > 1) {
		int cnt = 0;
		for (a = 0; a < nroom; a++) {
		    do {
			b = rn2(nroom-1);
		    } while (((a == b) || (rooms[b].doorct)) && cnt++ < 100);
		    if (cnt >= 100) {
			for (b = 0; b < nroom-1; b++)
			    if (!rooms[b].doorct && (a != b)) break;
		    }
		    if (a == b) b++;
		    join(a, b, FALSE);
		}
	    }
	    break;
	case 2: /* circular path: room1 -> room2 -> room3 -> ... -> room1  */
	    if (nroom > 1) {
		for (a = 0; a < nroom; a++) {
		    b = (a + 1) % nroom;
		    join(a, b, FALSE);
		}
	    }
	    break;
	case 3: /* all roads lead to rome. or to the first room. Amy edit: one random room, not always the leftmost */
        if (nroom > 1) {
            b = rn2(nroom);
            for (a = 0; a < nroom; a++) {
                if (b != a) join(a, b, FALSE);
            }
	  }
	    break;
    case 5: /* by Amy - join random rooms, then make sure that every room has at least one door */
        if (nroom > 1) {
            int rmcnt = nroom;
            rmcnt += rnd(rmcnt);
            while (rmcnt > 1) {
                rmcnt--;
                a = rn2(nroom);
                b = rn2(nroom);
                while (b == a) b = rn2(nroom);
                join(a, b, FALSE);
            }
        }
        for (a = 0; a < nroom; a++) {
            if (!rooms[a].doorct) {
                b = rn2(nroom);
                while (b == a) b = rn2(nroom);
                join(a, b, FALSE);
            }
        }
    
        break;
    case 6:
        if (nroom > 1) {
            for (a = 0; a < nroom; a++) {
                b = rn2(nroom);
                while (b == a) b = rn2(nroom);
                join(a, b, FALSE);
            }
            for (a = 0; a < nroom; a++) {
                b = rn2(nroom);
                while (b == a) b = rn2(nroom);
                join(a, b, FALSE);
            }
        }
        break;

	}

}

/* ALI - Artifact doors: Track doors in maze levels as well */

int
add_door(x,y,aroom)
register int x, y;
register struct mkroom *aroom;
{
	register struct mkroom *broom;
	register int tmp;

	if (doorindex == DOORMAX)
	    return -1;

	if (aroom) {
	    aroom->doorct++;
	    broom = aroom+1;
	} else
	    /* ALI
	     * Roomless doors must go right at the beginning of the list
	     */
	    broom = &rooms[0];
	if(broom->hx < 0)
	    tmp = doorindex;
	else
	    for(tmp = doorindex; tmp > broom->fdoor; tmp--)
		doors[tmp] = doors[tmp-1];
	doorindex++;
	doors[tmp].x = x;
	doors[tmp].y = y;
	for( ; broom->hx >= 0; broom++) broom->fdoor++;
	doors[tmp].arti_key = 0;
	return tmp;
}

STATIC_OVL void
dosdoor(x,y,aroom,type)
register xchar x, y;
register struct mkroom *aroom;
register int type;
{
	boolean shdoor = ((*in_rooms(x, y, SHOPBASE))? TRUE : FALSE);

	if(!IS_WALL(levl[x][y].typ)) /* avoid SDOORs on already made doors */
		type = DOOR;
	levl[x][y].typ = type;
	if(type == DOOR) {
	    if(!rn2(3)) {      /* is it a locked door, closed, or a doorway? */
		if(!rn2(5))
		    levl[x][y].doormask = D_ISOPEN;
		else if(!rn2(6))
		    levl[x][y].doormask = D_LOCKED;
		else
		    levl[x][y].doormask = D_CLOSED;

		if (levl[x][y].doormask != D_ISOPEN && !shdoor &&
		    level_difficulty() >= 5 && !rn2(25))
		    levl[x][y].doormask |= D_TRAPPED;
	    } else
#ifdef STUPID
		if (shdoor)
			levl[x][y].doormask = D_ISOPEN;
		else
			levl[x][y].doormask = D_NODOOR;
#else
		levl[x][y].doormask = (shdoor ? D_ISOPEN : D_NODOOR);
#endif
	    if(levl[x][y].doormask & D_TRAPPED) {
		struct monst *mtmp;

	/* "Mimics shouldn't appear outside of shops so early." In Soviet Russia, players can't handle mimics on dungeon level 5 for some reason. But mimics can appear as early as dungeon level 2, because that's where shops are generated for the first time, and those mimics everyone has to be able to handle... --Amy */

		if (level_difficulty() >= (issoviet ? 9 : 5) && !rn2(5) ) {
		    /* make a mimic instead */
		    levl[x][y].doormask = D_NODOOR;
		    mtmp = makemon(mkclass(S_MIMIC,0), x, y, MM_MAYSLEEP);
		    if (mtmp)
			set_mimic_sym(mtmp);
		}
	    }
	    /* newsym(x,y); */
	} else { /* SDOOR */
		if(shdoor || !rn2(5))	levl[x][y].doormask = D_LOCKED;
		else			levl[x][y].doormask = D_CLOSED;

		if(!shdoor && level_difficulty() >= 4 && !rn2(20))
		    levl[x][y].doormask |= D_TRAPPED;
	}

	add_door(x,y,aroom);
}

STATIC_OVL boolean
place_niche(aroom,dy,xx,yy)
register struct mkroom *aroom;
int *dy, *xx, *yy;
{
	coord dd;

	if(rn2(2)) {
	    *dy = 1;
	    finddpos(&dd, aroom->lx, aroom->hy+1, aroom->hx, aroom->hy+1);
	} else {
	    *dy = -1;
	    finddpos(&dd, aroom->lx, aroom->ly-1, aroom->hx, aroom->ly-1);
	}
	*xx = dd.x;
	*yy = dd.y;
	return((boolean)((isok(*xx,*yy+*dy) && levl[*xx][*yy+*dy].typ == STONE)
	    && (isok(*xx,*yy-*dy) && !IS_POOL(levl[*xx][*yy-*dy].typ)
				  && !IS_FURNITURE(levl[*xx][*yy-*dy].typ))));
}

/* there should be one of these per trap, in the same order as trap.h */
static NEARDATA const char *trap_engravings[TRAPNUM] = {
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0,
			/* 14..16: trap door, teleport, level-teleport */
			"Vlad was here", "ad aerarium", "ad aerarium",
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
};

STATIC_OVL void
makeniche(trap_type)
int trap_type;
{
	register struct mkroom *aroom;
	register struct rm *rm;
	register int vct = 8;
	int dy, xx, yy;
	register struct trap *ttmp;

	if(doorindex < DOORMAX)
	  while(vct--) {
	    aroom = &rooms[rn2(nroom)];
	    if(aroom->rtype != OROOM) continue;	/* not an ordinary room */
	    if(aroom->doorct == 1 && rn2(5)) continue;
	    if(!place_niche(aroom,&dy,&xx,&yy)) continue;

	    rm = &levl[xx][yy+dy];
	    if(trap_type || !rn2(4)) {

		rm->typ = SCORR;
		if(trap_type) {
		    if((trap_type == HOLE || trap_type == TRAPDOOR || trap_type == SHAFT_TRAP || trap_type == CURRENT_SHAFT)
			&& !Can_fall_thru(&u.uz))
			trap_type = ROCKTRAP;
		    ttmp = maketrap(xx, yy+dy, trap_type, 100, TRUE);
		    if (ttmp) {
			if (trap_type != ROCKTRAP) ttmp->once = 1;
			if (trap_engravings[trap_type]) {
				if (level.flags.vault_is_aquarium) {
					make_engr_at(xx, yy-dy,"ad aquarium",0L, DUST);
				} else if (level.flags.vault_is_cursed) {
					make_engr_at(xx, yy-dy,"iter vehemens ad necem",0L, DUST);
				} else {
			    make_engr_at(xx, yy-dy, trap_engravings[trap_type], 0L, DUST);
				}
			    wipe_engr_at(xx, yy-dy, 5); /* age it a little */
			}
		    }
		}
		dosdoor(xx, yy, aroom, SDOOR);
	    } else {
		rm->typ = CORR;
		if(rn2(35))
		    dosdoor(xx, yy, aroom, rn2(5) ? SDOOR : DOOR);
		else {

		    if (!rn2(2) && IS_WALL(levl[xx][yy].typ)) levl[xx][yy].typ = IRONBARS;

		    if (!level.flags.noteleport && !Race_if(PM_STABILISATOR))
			(void) mksobj_at(SCR_TELEPORTATION, xx, yy+dy, TRUE, FALSE, FALSE);
		    if (!rn2(3) && timebasedlowerchance()) (void) mkobj_at(0, xx, yy+dy, TRUE, FALSE);
		}
	    }
	    return;
	}
}

void
make_ironbarwalls(chance)
     int chance;
{
    xchar x,y;

    if (chance < 1) return;

    for (x = 1; x < COLNO-1; x++) {
	for(y = 1; y < ROWNO-1; y++) {
	    schar typ = levl[x][y].typ;
	    if (typ == HWALL) {
		if ((IS_WALL(levl[x-1][y].typ) || levl[x-1][y].typ == IRONBARS) &&
		    (IS_WALL(levl[x+1][y].typ) || levl[x+1][y].typ == IRONBARS) &&
		    SPACE_POS(levl[x][y-1].typ) && SPACE_POS(levl[x][y+1].typ) &&
		    rn2(100) < chance)
		    levl[x][y].typ = IRONBARS;
	    } else if (typ == VWALL) {
		if ((IS_WALL(levl[x][y-1].typ) || levl[x][y-1].typ == IRONBARS) &&
		    (IS_WALL(levl[x][y+1].typ) || levl[x][y+1].typ == IRONBARS) &&
		    SPACE_POS(levl[x-1][y].typ) && SPACE_POS(levl[x+1][y].typ) &&
		    rn2(100) < chance)
		    levl[x][y].typ = IRONBARS;
	    }
	}
    }
}

STATIC_OVL void
make_niches()
{
	register int ct = rnd((nroom>>1) + 1), dep = depth(&u.uz);

	boolean	ltptr = (!level.flags.noteleport && !Race_if(PM_STABILISATOR) && dep > 15),
		vamp = (dep > 5 && dep < 25);

	if (!rn2(2)) return; /* don't make so many --Amy */
	if ((ct > 1) && rn2(10)) ct = rnd(ct);

	while(ct--) {
		if (ltptr && !rn2(6)) {
			ltptr = FALSE;
			makeniche(LEVEL_TELEP);
		} else if (vamp && !rn2(6)) {
			vamp = FALSE;
			makeniche(TRAPDOOR);
		} else	makeniche(NO_TRAP);
	}
}

STATIC_OVL void
makevtele()
{
	makeniche(TELEP_TRAP);
}

/* specdungeoninit by Amy: function that modifies terrain and other stuff when a new level in certain subdungeons is loaded */
void
specdungeoninit()
{
	register int x,y;

	for (x = 0; x < COLNO; x++)
	  for (y = 0; y < ROWNO; y++) {

		if (isok(x,y) && In_illusorycastle(&u.uz) && (IS_STWALL(levl[x][y].typ) || levl[x][y].typ == ROOM || levl[x][y].typ == CORR) && !rn2(5)) {
			levl[x][y].typ = randomwalltype();

		}

		if (isok(x,y) && (In_voiddungeon(&u.uz) && !rn2(10)) && (levl[x][y].typ == ROOM || levl[x][y].typ == CORR) && !rn2(5)) {
			levl[x][y].typ = NETHERMIST;
			maketrap(x, y, GIANT_CHASM, 100, TRUE);
		}

		if (isok(x,y) && In_netherrealm(&u.uz) && (levl[x][y].typ == ROOM || levl[x][y].typ == CORR || (IS_STWALL(levl[x][y].typ) && !rn2(5)) ) && !rn2(5)) {
			levl[x][y].typ = NETHERMIST;
			if (!rn2(5)) maketrap(x, y, GIANT_CHASM, 100, TRUE);
		}

		if (isok(x,y) && In_angmar(&u.uz) && (IS_STWALL(levl[x][y].typ) && !rn2(5))) {
			levl[x][y].typ = rn2(7) ? SAND : SHIFTINGSAND;

		}

		if (isok(x,y) && In_angmar(&u.uz) && ((levl[x][y].typ == ROOM || levl[x][y].typ == CORR) && rn2(3))) {
			levl[x][y].typ = rn2(7) ? SAND : SHIFTINGSAND;

		}

		if (isok(x,y) && In_swimmingpool(&u.uz) && (IS_STWALL(levl[x][y].typ) && rn2(10))) {
			levl[x][y].typ = WATERTUNNEL;

		}

		if (isok(x,y) && In_swimmingpool(&u.uz) && ((levl[x][y].typ == ROOM || levl[x][y].typ == CORR) && !rn2(10))) {
			levl[x][y].typ = WATERTUNNEL;

		}

		if (isok(x,y) && In_hellbathroom(&u.uz) && (IS_STWALL(levl[x][y].typ) || levl[x][y].typ == ROOM || levl[x][y].typ == CORR) && rn2(2)) {
			levl[x][y].typ = rn2(100) ? URINELAKE : TOILET;

		}

		if (isok(x,y) && In_sewerplant(&u.uz) && (IS_STWALL(levl[x][y].typ) || (levl[x][y].typ == ROOM || levl[x][y].typ == CORR)) && !rn2(3)) {
			levl[x][y].typ = rn2(5) ? MOORLAND : MOAT;

		}

	}

	/* make rivers if possible --Amy */
	if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrivers();
	if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrivers();

	if (ishaxor) {
		if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrivers();
		if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrivers();
	}

	if (isaquarian && (!rn2(100) || depth(&u.uz) > 1) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrivers();
	if (RngeRivers && (!rn2(100) || depth(&u.uz) > 1) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrivers();

	if (Race_if(PM_HUNKAJUNK) && !rn2(25) && depth(&u.uz) > 1 && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) hunkajunkriver();
	if (Race_if(PM_HUNKAJUNK) && depth(&u.uz) > 1 && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) randhunkrivers();

	if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandrivers();
	if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandrivers();

	if (ishaxor) {
		if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandrivers();
		if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandrivers();
	}

	if ((isroommate || !rn2(100)) && ((depth(&u.uz) > 1 && !(u.preversionmode && In_greencross(&u.uz) && (dunlev(&u.uz) == 1)) && !(iszapem && In_spacebase(&u.uz) && (dunlev(&u.uz) == 1))) || !rn2(10)) && !In_endgame(&u.uz)) {

		mkroommateroom(0);
		if (!rn2(5)) {
			mkroommateroom(0);
			while (!rn2(3)) mkroommateroom(0);

		}

	}

	if (isaquarian && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandrivers();
	if (RngeRivers && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandrivers();


}

STATIC_OVL void
makeriver(x1,y1,x2,y2,lava,rndom)
int x1,y1,x2,y2;
boolean lava,rndom;
{
    int cx,cy;
    int dx, dy;
    int chance;
    int count = 0;
    int trynmbr = 0;
    int rndomizat = 0;
    const char *str;
    if (rndom) rndomizat = (rn2(5) ? 0 : 1);
    if (rndom) trynmbr = rnd(20);

	register struct obj *otmpX;
	register int tryct = 0;

    cx = x1;
    cy = y1;

    while (count++ < 2000) {
	int rnum = levl[cx][cy].roomno - ROOMOFFSET;
	chance = 0;
	/*if (rnum >= 0 && rooms[rnum].rtype != OROOM) chance = 0;
	else */if (levl[cx][cy].typ == CORR) chance = 15;
	else if (levl[cx][cy].typ == ROOM) chance = 30;
	else if (IS_ROCK(levl[cx][cy].typ)) chance = 100;
	if (rndomizat) trynmbr = (rn2(5) ? rnd(80) : rnd(28));

	if (rn2(100) < chance && !t_at(cx,cy)) {
	    if (lava) {
		if (rndom) { 

			if (trynmbr == 1) levl[cx][cy].typ = POOL;
			else if (trynmbr == 2) levl[cx][cy].typ = TREE;
			else if (trynmbr == 3) levl[cx][cy].typ = IRONBARS;
			else if (trynmbr == 4) levl[cx][cy].typ = ICE;
			else if (trynmbr == 5) levl[cx][cy].typ = CLOUD;
			else if (trynmbr == 6) levl[cx][cy].typ = CORR;
			else if (trynmbr == 7) levl[cx][cy].typ = GRAVEWALL;
			else if (trynmbr == 8) levl[cx][cy].typ = TUNNELWALL;
			else if (trynmbr == 9) levl[cx][cy].typ = FARMLAND;
			else if (trynmbr == 10) levl[cx][cy].typ = MOUNTAIN;
			else if (trynmbr == 11) levl[cx][cy].typ = WATERTUNNEL;
			else if (trynmbr == 12) levl[cx][cy].typ = CRYSTALWATER;
			else if (trynmbr == 13) levl[cx][cy].typ = MOORLAND;
			else if (trynmbr == 14) levl[cx][cy].typ = URINELAKE;
			else if (trynmbr == 15) levl[cx][cy].typ = SHIFTINGSAND;
			else if (trynmbr == 16) levl[cx][cy].typ = STYXRIVER;
			else if (trynmbr == 17) levl[cx][cy].typ = SNOW;
			else if (trynmbr == 18) levl[cx][cy].typ = ASH;
			else if (trynmbr == 19) levl[cx][cy].typ = SAND;
			else if (trynmbr == 20) levl[cx][cy].typ = PAVEDFLOOR;
			else if (trynmbr == 21) levl[cx][cy].typ = HIGHWAY;
			else if (trynmbr == 22) levl[cx][cy].typ = GRASSLAND;
			else if (trynmbr == 23) levl[cx][cy].typ = NETHERMIST;
			else if (trynmbr == 24) levl[cx][cy].typ = STALACTITE;
			else if (trynmbr == 25) levl[cx][cy].typ = CRYPTFLOOR;
			else if (trynmbr == 26) levl[cx][cy].typ = BUBBLES;
			else if (trynmbr == 27) levl[cx][cy].typ = RAINCLOUD;
			else levl[cx][cy].typ = LAVAPOOL;
		}
		else {levl[cx][cy].typ = LAVAPOOL;
		levl[cx][cy].lit = 1;
		}
	    } else	if (rndom) { 

			if (trynmbr == 1) levl[cx][cy].typ = LAVAPOOL;
			else if (trynmbr == 2) levl[cx][cy].typ = TREE;
			else if (trynmbr == 3) levl[cx][cy].typ = IRONBARS;
			else if (trynmbr == 4) levl[cx][cy].typ = ICE;
			else if (trynmbr == 5) levl[cx][cy].typ = CLOUD;
			else if (trynmbr == 6) levl[cx][cy].typ = CORR;
			else if (trynmbr == 7) levl[cx][cy].typ = GRAVEWALL;
			else if (trynmbr == 8) levl[cx][cy].typ = TUNNELWALL;
			else if (trynmbr == 9) levl[cx][cy].typ = FARMLAND;
			else if (trynmbr == 10) levl[cx][cy].typ = MOUNTAIN;
			else if (trynmbr == 11) levl[cx][cy].typ = WATERTUNNEL;
			else if (trynmbr == 12) levl[cx][cy].typ = CRYSTALWATER;
			else if (trynmbr == 13) levl[cx][cy].typ = MOORLAND;
			else if (trynmbr == 14) levl[cx][cy].typ = URINELAKE;
			else if (trynmbr == 15) levl[cx][cy].typ = SHIFTINGSAND;
			else if (trynmbr == 16) levl[cx][cy].typ = STYXRIVER;
			else if (trynmbr == 17) levl[cx][cy].typ = SNOW;
			else if (trynmbr == 18) levl[cx][cy].typ = ASH;
			else if (trynmbr == 19) levl[cx][cy].typ = SAND;
			else if (trynmbr == 20) levl[cx][cy].typ = PAVEDFLOOR;
			else if (trynmbr == 21) levl[cx][cy].typ = HIGHWAY;
			else if (trynmbr == 22) levl[cx][cy].typ = GRASSLAND;
			else if (trynmbr == 23) levl[cx][cy].typ = NETHERMIST;
			else if (trynmbr == 24) levl[cx][cy].typ = STALACTITE;
			else if (trynmbr == 25) levl[cx][cy].typ = CRYPTFLOOR;
			else if (trynmbr == 26) levl[cx][cy].typ = BUBBLES;
			else if (trynmbr == 27) levl[cx][cy].typ = RAINCLOUD;
			else levl[cx][cy].typ = POOL;
		}
		else 
		levl[cx][cy].typ = !rn2(3) ? POOL : MOAT;

		if (!rn2(ishaxor ? 10000 : 20000))
			levl[cx][cy].typ = THRONE;
		else if (!rn2(ishaxor ? 50000 : 100000))
			levl[cx][cy].typ = PENTAGRAM;
		else if (!rn2(ishaxor ? 25000 : 50000))
			levl[cx][cy].typ = WELL;
		else if (!rn2(ishaxor ? 25000 : 50000))
			levl[cx][cy].typ = POISONEDWELL;
		else if (!rn2(ishaxor ? 25000 : 50000))
			levl[cx][cy].typ = WAGON;
		else if (!rn2(ishaxor ? 25000 : 50000))
			levl[cx][cy].typ = BURNINGWAGON;
		else if (!rn2(ishaxor ? 20000 : 40000))
			levl[cx][cy].typ = WOODENTABLE;
		else if (!rn2(ishaxor ? 25000 : 50000))
			levl[cx][cy].typ = CARVEDBED;
		else if (!rn2(ishaxor ? 10000 : 20000))
			levl[cx][cy].typ = STRAWMATTRESS;
		else if (!rn2(ishaxor ? 2500 : 5000)) {
			levl[cx][cy].typ = FOUNTAIN;
			level.flags.nfountains++;
			}
		else if (!rn2(ishaxor ? 2500 : 5000)) {
			levl[cx][cy].typ = SINK;
			level.flags.nsinks++;
			}
		else if (!rn2(ishaxor ? 5000 : 10000))
			levl[cx][cy].typ = TOILET;
		else if (!rn2(ishaxor ? 1000 : 2000)) {
			levl[cx][cy].typ = GRAVE;
			str = random_epitaph();
			del_engr_at(cx, cy);
			make_engr_at(cx, cy, str, 0L, HEADSTONE);
	
			if (!rn2(3)) (void) mkgold(0L, cx, cy);
			for (tryct = rn2(5); tryct; tryct--) {
					if (timebasedlowerchance()) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE, FALSE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = cx;
					    otmpX->oy = cy;
					    add_to_buried(otmpX);
					}
				}
			}
		else if (!rn2(ishaxor ? 10000 : 20000)) {
			levl[cx][cy].typ = ALTAR;
			if (rn2(10)) levl[cx][cy].altarmask = Align2amask( A_NONE );
			else switch (rnd(3)) {
	
			case 1: levl[cx][cy].altarmask = Align2amask( A_LAWFUL ); break;
			case 2: levl[cx][cy].altarmask = Align2amask( A_NEUTRAL ); break;
			case 3: levl[cx][cy].altarmask = Align2amask( A_CHAOTIC ); break;
	
			}
		}

	}

	if (cx == x2 && cy == y2) break;

	if (cx < x2 && !rn2(3)) dx = 1;
	else if (cx > x2 && !rn2(3)) dx = -1;
	else dx = 0;

	if (cy < y2 && !rn2(3)) dy = 1;
	else if (cy > y2 && !rn2(3)) dy = -1;
	else dy = 0;

	switch (rn2(16)) {
	default: break;
	case 1: dx--; dy--; break;
	case 2: dx++; dy--; break;
	case 3: dx--; dy++; break;
	case 4: dx++; dy++; break;
	case 5: dy--; break;
	case 6: dy++; break;
	case 7: dx--; break;
	case 8: dx++; break;
	}

	if (dx < -1) dx = -1;
	else if (dx > 1) dx = 1;
	if (dy < -1) dy = -1;
	else if (dy > 1) dy = 1;

	cx += dx;
	cy += dy;

	if (cx < 0) cx = 0;
	else if (cx >= COLNO) cx = COLNO-1;
	if (cy < 0) cy = 0;
	else if (cy >= ROWNO) cy = ROWNO-1;

    }
}

STATIC_OVL void
makehunkriver(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
    int cx,cy;
    int dx, dy;
    int chance;
    int count = 0;
    const char *str;

    cx = x1;
    cy = y1;

    while (count++ < 2000) {
	int rnum = levl[cx][cy].roomno - ROOMOFFSET;
	chance = 0;
	if (levl[cx][cy].typ == CORR) chance = 75;
	else if (levl[cx][cy].typ == ROOM) chance = 90;
	else if (levl[cx][cy].typ == ICE) chance = 80;
	else if (levl[cx][cy].typ == SNOW) chance = 60;
	else if (levl[cx][cy].typ == ASH) chance = 75;
	else if (levl[cx][cy].typ == SAND) chance = 95;
	else if (levl[cx][cy].typ == PAVEDFLOOR) chance = 90;
	else if (levl[cx][cy].typ == HIGHWAY) chance = 95;
	else if (levl[cx][cy].typ == GRASSLAND) chance = 70;
	else if (levl[cx][cy].typ == NETHERMIST) chance = 50;
	else if (levl[cx][cy].typ == STALACTITE) chance = 30;
	else if (levl[cx][cy].typ == CRYPTFLOOR) chance = 70;
	else if (levl[cx][cy].typ == AIR) chance = 60;
	else if (levl[cx][cy].typ == CLOUD) chance = 75;
	else if (levl[cx][cy].typ == BUBBLES) chance = 50;
	else if (levl[cx][cy].typ == RAINCLOUD) chance = 30;

	if (rn2(100) < chance) {
		levl[cx][cy].typ = ROCKWALL;
		blockorunblock_point(cx,cy);
	}

	if (cx == x2 && cy == y2) break;

	if (cx < x2 && !rn2(3)) dx = 1;
	else if (cx > x2 && !rn2(3)) dx = -1;
	else dx = 0;

	if (cy < y2 && !rn2(3)) dy = 1;
	else if (cy > y2 && !rn2(3)) dy = -1;
	else dy = 0;

	switch (rn2(16)) {
	default: break;
	case 1: dx--; dy--; break;
	case 2: dx++; dy--; break;
	case 3: dx--; dy++; break;
	case 4: dx++; dy++; break;
	case 5: dy--; break;
	case 6: dy++; break;
	case 7: dx--; break;
	case 8: dx++; break;
	}

	if (dx < -1) dx = -1;
	else if (dx > 1) dx = 1;
	if (dy < -1) dy = -1;
	else if (dy > 1) dy = 1;

	cx += dx;
	cy += dy;

	if (cx < 0) cx = 0;
	else if (cx >= COLNO) cx = COLNO-1;
	if (cy < 0) cy = 0;
	else if (cy >= ROWNO) cy = ROWNO-1;

    }
}

STATIC_OVL void
makerandriver(lava,rndom)
boolean lava,rndom;

{
    int cx,cy;
    int chance;
    int count = 0;
    int ammount = rnz(10 + rnd(40) + rnz(5) + (rn2(5) ? 0 : 50) + (rn2(25) ? 0 : 200) );
    int trynmbr = 0;
    int rndomizat = 0;
    const char *str;
    if (rndom) rndomizat = (rn2(3) ? 0 : 1);
    if (rndom) trynmbr = rnd(12);

	register int tryct = 0;
	register struct obj *otmpX;

    while (count++ < ammount) {

      cx = rn2(COLNO);
      cy = rn2(ROWNO);

	chance = 0;
	if (levl[cx][cy].typ == CORR) chance = 15;
	else if (levl[cx][cy].typ == ROOM) chance = 30;
	else if (IS_ROCK(levl[cx][cy].typ)) chance = 100;
	if (rndomizat) trynmbr = (rn2(5) ? rnd(50) : rnd(28));

	if (rn2(100) < chance && !t_at(cx,cy)) {
	    if (lava) {
		if (rndom) { 

			if (trynmbr == 1) levl[cx][cy].typ = POOL;
			else if (trynmbr == 2) levl[cx][cy].typ = TREE;
			else if (trynmbr == 3) levl[cx][cy].typ = IRONBARS;
			else if (trynmbr == 4) levl[cx][cy].typ = ICE;
			else if (trynmbr == 5) levl[cx][cy].typ = CLOUD;
			else if (trynmbr == 6) levl[cx][cy].typ = CORR;
			else if (trynmbr == 7) levl[cx][cy].typ = GRAVEWALL;
			else if (trynmbr == 8) levl[cx][cy].typ = TUNNELWALL;
			else if (trynmbr == 9) levl[cx][cy].typ = FARMLAND;
			else if (trynmbr == 10) levl[cx][cy].typ = MOUNTAIN;
			else if (trynmbr == 11) levl[cx][cy].typ = WATERTUNNEL;
			else if (trynmbr == 12) levl[cx][cy].typ = CRYSTALWATER;
			else if (trynmbr == 13) levl[cx][cy].typ = MOORLAND;
			else if (trynmbr == 14) levl[cx][cy].typ = URINELAKE;
			else if (trynmbr == 15) levl[cx][cy].typ = SHIFTINGSAND;
			else if (trynmbr == 16) levl[cx][cy].typ = STYXRIVER;
			else if (trynmbr == 17) levl[cx][cy].typ = SNOW;
			else if (trynmbr == 18) levl[cx][cy].typ = ASH;
			else if (trynmbr == 19) levl[cx][cy].typ = SAND;
			else if (trynmbr == 20) levl[cx][cy].typ = PAVEDFLOOR;
			else if (trynmbr == 21) levl[cx][cy].typ = HIGHWAY;
			else if (trynmbr == 22) levl[cx][cy].typ = GRASSLAND;
			else if (trynmbr == 23) levl[cx][cy].typ = NETHERMIST;
			else if (trynmbr == 24) levl[cx][cy].typ = STALACTITE;
			else if (trynmbr == 25) levl[cx][cy].typ = CRYPTFLOOR;
			else if (trynmbr == 26) levl[cx][cy].typ = BUBBLES;
			else if (trynmbr == 27) levl[cx][cy].typ = RAINCLOUD;
			else levl[cx][cy].typ = LAVAPOOL;
		}
		else {levl[cx][cy].typ = LAVAPOOL;
		levl[cx][cy].lit = 1;
		}
	    } else	if (rndom) { 

			if (trynmbr == 1) levl[cx][cy].typ = LAVAPOOL;
			else if (trynmbr == 2) levl[cx][cy].typ = TREE;
			else if (trynmbr == 3) levl[cx][cy].typ = IRONBARS;
			else if (trynmbr == 4) levl[cx][cy].typ = ICE;
			else if (trynmbr == 5) levl[cx][cy].typ = CLOUD;
			else if (trynmbr == 6) levl[cx][cy].typ = CORR;
			else if (trynmbr == 7) levl[cx][cy].typ = GRAVEWALL;
			else if (trynmbr == 8) levl[cx][cy].typ = TUNNELWALL;
			else if (trynmbr == 9) levl[cx][cy].typ = FARMLAND;
			else if (trynmbr == 10) levl[cx][cy].typ = MOUNTAIN;
			else if (trynmbr == 11) levl[cx][cy].typ = WATERTUNNEL;
			else if (trynmbr == 12) levl[cx][cy].typ = CRYSTALWATER;
			else if (trynmbr == 13) levl[cx][cy].typ = MOORLAND;
			else if (trynmbr == 14) levl[cx][cy].typ = URINELAKE;
			else if (trynmbr == 15) levl[cx][cy].typ = SHIFTINGSAND;
			else if (trynmbr == 16) levl[cx][cy].typ = STYXRIVER;
			else if (trynmbr == 17) levl[cx][cy].typ = SNOW;
			else if (trynmbr == 18) levl[cx][cy].typ = ASH;
			else if (trynmbr == 19) levl[cx][cy].typ = SAND;
			else if (trynmbr == 20) levl[cx][cy].typ = PAVEDFLOOR;
			else if (trynmbr == 21) levl[cx][cy].typ = HIGHWAY;
			else if (trynmbr == 22) levl[cx][cy].typ = GRASSLAND;
			else if (trynmbr == 23) levl[cx][cy].typ = NETHERMIST;
			else if (trynmbr == 24) levl[cx][cy].typ = STALACTITE;
			else if (trynmbr == 25) levl[cx][cy].typ = CRYPTFLOOR;
			else if (trynmbr == 26) levl[cx][cy].typ = BUBBLES;
			else if (trynmbr == 27) levl[cx][cy].typ = RAINCLOUD;
			else levl[cx][cy].typ = POOL;
		}
		else 
		levl[cx][cy].typ = !rn2(3) ? POOL : MOAT;

		if (!rn2(ishaxor ? 10000 : 20000))
			levl[cx][cy].typ = THRONE;
		else if (!rn2(ishaxor ? 50000 : 100000))
			levl[cx][cy].typ = PENTAGRAM;
		else if (!rn2(ishaxor ? 25000 : 50000))
			levl[cx][cy].typ = WELL;
		else if (!rn2(ishaxor ? 25000 : 50000))
			levl[cx][cy].typ = POISONEDWELL;
		else if (!rn2(ishaxor ? 25000 : 50000))
			levl[cx][cy].typ = WAGON;
		else if (!rn2(ishaxor ? 25000 : 50000))
			levl[cx][cy].typ = BURNINGWAGON;
		else if (!rn2(ishaxor ? 20000 : 40000))
			levl[cx][cy].typ = WOODENTABLE;
		else if (!rn2(ishaxor ? 25000 : 50000))
			levl[cx][cy].typ = CARVEDBED;
		else if (!rn2(ishaxor ? 10000 : 20000))
			levl[cx][cy].typ = STRAWMATTRESS;
		else if (!rn2(ishaxor ? 2500 : 5000)) {
			levl[cx][cy].typ = FOUNTAIN;
			level.flags.nfountains++;
			}
		else if (!rn2(ishaxor ? 2500 : 5000)) {
			levl[cx][cy].typ = SINK;
			level.flags.nsinks++;
			}
		else if (!rn2(ishaxor ? 5000 : 10000))
			levl[cx][cy].typ = TOILET;
		else if (!rn2(ishaxor ? 1000 : 2000)) {
			levl[cx][cy].typ = GRAVE;
			str = random_epitaph();
			del_engr_at(cx, cy);
			make_engr_at(cx, cy, str, 0L, HEADSTONE);
	
			if (!rn2(3)) (void) mkgold(0L, cx, cy);
			for (tryct = rn2(5); tryct; tryct--) {
					if (timebasedlowerchance()) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE, FALSE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = cx;
					    otmpX->oy = cy;
					    add_to_buried(otmpX);
					}
				}
			}
		else if (!rn2(ishaxor ? 10000 : 20000)) {
			levl[cx][cy].typ = ALTAR;
			if (rn2(10)) levl[cx][cy].altarmask = Align2amask( A_NONE );
			else switch (rnd(3)) {
	
			case 1: levl[cx][cy].altarmask = Align2amask( A_LAWFUL ); break;
			case 2: levl[cx][cy].altarmask = Align2amask( A_NEUTRAL ); break;
			case 3: levl[cx][cy].altarmask = Align2amask( A_CHAOTIC ); break;
	
			}
		}
	}

	}
}

STATIC_OVL void
makerandhunkriver(rndom)
boolean rndom;

{
    int cx,cy;
    int chance;
    int count = 0;
    int ammount = rnz(10 + rnd(40) + rnz(5) + (rn2(5) ? 0 : 50) + (rn2(25) ? 0 : 200) );
    int trynmbr = 0;
    const char *str;
    if (rndom) trynmbr = rnd(12);

	register int tryct = 0;
	register struct obj *otmpX;

    while (count++ < ammount) {

      cx = rn2(COLNO);
      cy = rn2(ROWNO);

	chance = 0;
	if (levl[cx][cy].typ == CORR) chance = 75;
	else if (levl[cx][cy].typ == ROOM) chance = 90;
	else if (levl[cx][cy].typ == ICE) chance = 80;
	else if (levl[cx][cy].typ == SNOW) chance = 60;
	else if (levl[cx][cy].typ == ASH) chance = 75;
	else if (levl[cx][cy].typ == SAND) chance = 95;
	else if (levl[cx][cy].typ == PAVEDFLOOR) chance = 90;
	else if (levl[cx][cy].typ == HIGHWAY) chance = 95;
	else if (levl[cx][cy].typ == GRASSLAND) chance = 70;
	else if (levl[cx][cy].typ == NETHERMIST) chance = 50;
	else if (levl[cx][cy].typ == STALACTITE) chance = 30;
	else if (levl[cx][cy].typ == CRYPTFLOOR) chance = 70;
	else if (levl[cx][cy].typ == AIR) chance = 60;
	else if (levl[cx][cy].typ == CLOUD) chance = 75;
	else if (levl[cx][cy].typ == BUBBLES) chance = 50;
	else if (levl[cx][cy].typ == RAINCLOUD) chance = 30;

	if (rn2(100) < chance) {
		if (rndom || !rn2(10)) { 

			if (trynmbr == 1) levl[cx][cy].typ = POOL;
			else if (trynmbr == 2) levl[cx][cy].typ = TREE;
			else if (trynmbr == 3) levl[cx][cy].typ = IRONBARS;
			else if (trynmbr == 4) levl[cx][cy].typ = ICE;
			else if (trynmbr == 5) levl[cx][cy].typ = CLOUD;
			else if (trynmbr == 6) levl[cx][cy].typ = CORR;
			else if (trynmbr == 7) levl[cx][cy].typ = GRAVEWALL;
			else if (trynmbr == 8) levl[cx][cy].typ = TUNNELWALL;
			else if (trynmbr == 9) levl[cx][cy].typ = FARMLAND;
			else if (trynmbr == 10) levl[cx][cy].typ = MOUNTAIN;
			else if (trynmbr == 11) levl[cx][cy].typ = WATERTUNNEL;
			else if (trynmbr == 12) levl[cx][cy].typ = CRYSTALWATER;
			else if (trynmbr == 13) levl[cx][cy].typ = MOORLAND;
			else if (trynmbr == 14) levl[cx][cy].typ = URINELAKE;
			else if (trynmbr == 15) levl[cx][cy].typ = SHIFTINGSAND;
			else if (trynmbr == 16) levl[cx][cy].typ = STYXRIVER;
			else if (trynmbr == 17) levl[cx][cy].typ = SNOW;
			else if (trynmbr == 18) levl[cx][cy].typ = ASH;
			else if (trynmbr == 19) levl[cx][cy].typ = SAND;
			else if (trynmbr == 20) levl[cx][cy].typ = PAVEDFLOOR;
			else if (trynmbr == 21) levl[cx][cy].typ = HIGHWAY;
			else if (trynmbr == 22) levl[cx][cy].typ = GRASSLAND;
			else if (trynmbr == 23) levl[cx][cy].typ = NETHERMIST;
			else if (trynmbr == 24) levl[cx][cy].typ = STALACTITE;
			else if (trynmbr == 25) levl[cx][cy].typ = CRYPTFLOOR;
			else if (trynmbr == 26) levl[cx][cy].typ = BUBBLES;
			else if (trynmbr == 27) levl[cx][cy].typ = RAINCLOUD;
			else levl[cx][cy].typ = ROCKWALL;
		} else {
			levl[cx][cy].typ = ROCKWALL;
		}
		blockorunblock_point(cx,cy);

	}

	}
}

void
mkrivers()
{
    boolean lava;
    boolean rndom;
    int nriv = rn2(3) + 1;
    if (!rn2(10)) nriv += rnd(3);
    if (!rn2(100)) nriv += rnd(5);
    if (!rn2(500)) nriv += rnd(7);
    if (!rn2(2000)) nriv += rnd(10);
    if (!rn2(10000)) nriv += rnd(15);
    while (nriv--) {
      lava = rn2(100) < depth(&u.uz);
      rndom = (rn2(4) ? 0 : 1);
	if (rn2(2)) makeriver(0, rn2(ROWNO), COLNO-1, rn2(ROWNO), lava, rndom);
	else makeriver(rn2(COLNO), 0, rn2(COLNO), ROWNO-1, lava, rndom);
    }
}

void
hunkajunkriver()
{
    int nriv = rn2(3) + 1;
    if (!rn2(10)) nriv += rnd(3);
    if (!rn2(100)) nriv += rnd(5);
    if (!rn2(500)) nriv += rnd(7);
    if (!rn2(2000)) nriv += rnd(10);
    if (!rn2(10000)) nriv += rnd(15);
    while (nriv--) {
	if (rn2(2)) makehunkriver(0, rn2(ROWNO), COLNO-1, rn2(ROWNO));
	else makehunkriver(rn2(COLNO), 0, rn2(COLNO), ROWNO-1);
    }
}

void
mkrandrivers()
{
    boolean lava;
    boolean rndom;
    int nriv = 1;
    if (!rn2(10)) nriv += rnd(2);
    if (!rn2(100)) nriv += rnd(3);
    if (!rn2(500)) nriv += rnd(5);
    if (!rn2(2000)) nriv += rnd(7);
    if (!rn2(10000)) nriv += rnd(10);

    while (nriv--) {
      lava = rn2(100) < depth(&u.uz);
      rndom = (rn2(3) ? 0 : 1);
	if (rn2(2)) makerandriver(lava, rndom);
	else makerandriver(lava, rndom);
    }
}

void
randhunkrivers()
{
    boolean rndom;
    int nriv = 1;
    if (!rn2(10)) nriv += rnd(2);
    if (!rn2(100)) nriv += rnd(3);
    if (!rn2(500)) nriv += rnd(5);
    if (!rn2(2000)) nriv += rnd(7);
    if (!rn2(10000)) nriv += rnd(10);

    while (nriv--) {
      rndom = (rn2(10) ? 0 : 1);
	makerandhunkriver(rndom);
    }
}


/* clear out various globals that keep information on the current level.
 * some of this is only necessary for some types of levels (maze, normal,
 * special) but it's easier to put it all in one place than make sure
 * each type initializes what it needs to separately.
 */
STATIC_OVL void
clear_level_structures()
{
#ifdef DISPLAY_LAYERS
	static struct rm zerorm = { S_stone, 0, 0, 0, 0, 0,
#else
	static struct rm zerorm = { cmap_to_glyph(S_stone),
#endif
						0, 0, 0, 0, 0, 0, 0, 0 };
	register int x,y;
	register struct rm *lev;

	for(x=0; x<COLNO; x++) {
	    lev = &levl[x][0];
	    for(y=0; y<ROWNO; y++) {
		*lev++ = zerorm;
#ifdef MICROPORT_BUG
		level.objects[x][y] = (struct obj *)0;
		level.monsters[x][y] = (struct monst *)0;
#endif
	    }
	}
#ifndef MICROPORT_BUG
	(void) memset((void *)level.objects, 0, sizeof(level.objects));
	(void) memset((void *)level.monsters, 0, sizeof(level.monsters));
#endif
	level.objlist = (struct obj *)0;
	level.buriedobjlist = (struct obj *)0;
	level.monlist = (struct monst *)0;
	level.damagelist = (struct damage *)0;

	level.flags.nfountains = 0;
	level.flags.nsinks = 0;
	level.flags.has_shop = 0;
	level.flags.has_vault = 0;
	level.flags.vault_is_aquarium = 0;
	level.flags.vault_is_cursed = 0;
	level.flags.has_zoo = 0;
	level.flags.has_court = 0;
	level.flags.has_morgue = level.flags.graveyard = 0;
	level.flags.has_beehive = 0;
	level.flags.has_barracks = 0;
	level.flags.has_lemurepit = 0;
	level.flags.has_migohive = 0;
	level.flags.has_fungusfarm = 0;
	level.flags.has_clinic = 0;
	level.flags.has_terrorhall = 0;
	level.flags.has_tenshall = 0;
	level.flags.has_elemhall = 0;
	level.flags.has_angelhall = 0;
	level.flags.has_mimichall = 0;
	level.flags.has_nymphhall = 0;
	level.flags.has_golemhall = 0;
	level.flags.has_humanhall = 0;
	level.flags.has_spiderhall = 0;
	level.flags.has_trollhall = 0;
	level.flags.has_coinhall = 0;

	level.flags.has_grueroom = 0;
	level.flags.has_poolroom = 0;
	level.flags.has_traproom = 0;

	level.flags.has_cryptroom = 0;
	level.flags.has_troublezone = 0;
	level.flags.has_weaponchamber = 0;
	level.flags.has_hellpit = 0;
	level.flags.has_robbercave = 0;
	level.flags.has_casinoroom = 0;
	level.flags.has_sanitationcentral = 0;
	level.flags.has_feminismroom = 0;
	level.flags.has_meadowroom = 0;
	level.flags.has_coolingchamber = 0;
	level.flags.has_voidroom = 0;
	level.flags.has_armory = 0;
	level.flags.has_hamletroom = 0;
	level.flags.has_kopstation = 0;
	level.flags.has_bossroom = 0;
	level.flags.has_rngcenter = 0;
	level.flags.has_wizardsdorm = 0;
	level.flags.has_doomedbarracks = 0;
	level.flags.has_sleepingroom = 0;
	level.flags.has_diverparadise = 0;
	level.flags.has_menagerie = 0;
	level.flags.has_nastycentral = 0;
	level.flags.has_emptydesert = 0;
	level.flags.has_rarityroom = 0;
	level.flags.has_exhibitroom = 0;
	level.flags.has_prisonchamber = 0;
	level.flags.has_nuclearchamber = 0;
	level.flags.has_levelseventyroom = 0;
	level.flags.has_playercentral = 0;
	level.flags.has_variantroom = 0;

	level.flags.has_evilroom = 0;
	level.flags.has_religioncenter = 0;
	level.flags.has_cursedmummyroom = 0;
	level.flags.has_arduousmountain = 0;
	level.flags.has_changingroom = 0;
	level.flags.has_questorroom = 0;
	level.flags.has_levelffroom = 0;
	level.flags.has_verminroom = 0;
	level.flags.has_miraspa = 0;
	level.flags.has_machineroom = 0;
	level.flags.has_showerroom = 0;
	level.flags.has_greencrossroom = 0;
	level.flags.has_ruinedchurch = 0;
	level.flags.has_gamecorner = 0;
	level.flags.has_illusionroom = 0;

	level.flags.has_statueroom = 0;
	level.flags.has_insideroom = 0;
	level.flags.has_riverroom = 0;

	level.flags.has_temple = 0;
	level.flags.has_swamp = 0;
	level.flags.noteleport = 0;
	level.flags.hardfloor = 0;
	level.flags.nommap = 0;
	level.flags.hero_memory = 1;
	level.flags.shortsighted = 0;
	level.flags.arboreal = 0;
	level.flags.is_maze_lev = 0;
	level.flags.is_cavernous_lev = 0;
	/* KMH -- more level properties */
	level.flags.arboreal = 0;
	level.flags.spooky = 0;

	/* [DS] - Michael Clarke's Lethe flag */
	level.flags.lethe = 0;

	nroom = 0;
	rooms[0].hx = -1;
	nsubroom = 0;
	subrooms[0].hx = -1;
	doorindex = 0;
	init_rect();
	init_vault();
	xdnstair = ydnstair = xupstair = yupstair = 0;
	sstairs.sx = sstairs.sy = 0;
	xdnladder = ydnladder = xupladder = yupladder = 0;
	made_branch = FALSE;
	clear_regions();
}

STATIC_OVL void
makelevel()
{
	register struct mkroom *croom, *troom;
	register int tryct;
	register int x, y;
	struct monst *tmonst;	/* always put a web with a spider */
	branch *branchp;
	int room_threshold;
	schar randrmtyp;
	int specialraceflag = 0;	/* for mazewalker etc. */

	if(wiz1_level.dlevel == 0) init_dungeons();
	oinit();	/* assign level dependent obj probabilities */
	clear_level_structures();

	{
	    register s_level *slev = Is_special(&u.uz);

	    /* check for special levels */
#ifdef REINCARNATION
	    if (slev && !Is_rogue_level(&u.uz))
#else
	    if (slev)
#endif
	    {
		    makemaz(slev->proto);
		    return;
	    } else if (In_V_tower(&u.uz)) {
		    makemaz("");
		    return;

	    } else if (In_minotaurmaze(&u.uz)) { /* has only "interesting" mazes --Amy */

			switch (rnd(69)) {

			case 1: makemaz("mazes-1"); return;
			case 2: makemaz("mazes-2"); return;
			case 3: makemaz("mazes-3"); return;
			case 4: makemaz("mazes-4"); return;
			case 5: makemaz("mazes-5"); return;
			case 6: makemaz("mazes-6"); return;
			case 7: makemaz("mazes-7"); return;
			case 8: makemaz("mazes-8"); return;
			case 9: makemaz("mazes-9"); return;
			case 10: makemaz("mazes-10"); return;
			case 11: makemaz("mazes-11"); return;
			case 12: makemaz("mazes-12"); return;
			case 13: makemaz("mazes-13"); return;
			case 14: makemaz("mazes-14"); return;
			case 15: makemaz("mazes-15"); return;
			case 16: makemaz("mazes-16"); return;
			case 17: makemaz("mazes-17"); return;
			case 18: makemaz("mazes-18"); return;
			case 19: makemaz("mazes-19"); return;
			case 20: makemaz("mazes-20"); return;
			case 21: makemaz("mazes-21"); return;
			case 22: makemaz("mazes-22"); return;
			case 23: makemaz("mazes-23"); return;
			case 24: makemaz("mazes-24"); return;
			case 25: makemaz("mazes-25"); return;
			case 26: makemaz("mazes-26"); return;
			case 27: makemaz("mazes-27"); return;
			case 28: makemaz("mazes-28"); return;
			case 29: makemaz("mazes-29"); return;
			case 30: makemaz("mazes-30"); return;
			case 31: makemaz("mazes-31"); return;
			case 32: makemaz("mazes-32"); return;
			case 33: makemaz("mazes-33"); return;
			case 34: makemaz("mazes-34"); return;
			case 35: makemaz("mazes-35"); return;
			case 36: makemaz("mazes-36"); return;
			case 37: makemaz("mazes-37"); return;
			case 38: makemaz("mazes-38"); return;
			case 39: makemaz("mazes-39"); return;
			case 40: makemaz("mazes-40"); return;
			case 41: makemaz("mazes-41"); return;
			case 42: makemaz("mazes-42"); return;
			case 43: makemaz("mazes-43"); return;
			case 44: makemaz("mazes-44"); return;
			case 45: makemaz("mazes-45"); return;
			case 46: makemaz("mazes-46"); return;
			case 47: makemaz("mazes-47"); return;
			case 48: makemaz("mazes-48"); return;
			case 49: makemaz("mazes-49"); return;
			case 50: makemaz("mazes-50"); return;
			case 51: makemaz("mazes-51"); return;
			case 52: makemaz("mazes-52"); return;
			case 53: makemaz("mazes-53"); return;
			case 54: makemaz("mazes-54"); return;
			case 55: makemaz("mazes-55"); return;
			case 56: makemaz("mazes-56"); return;
			case 57: makemaz("mazes-57"); return;
			case 58: makemaz("mazes-58"); return;
			case 59: makemaz("mazes-59"); return;
			case 60: makemaz("mazes-60"); return;
			case 61: makemaz("mazes-61"); return;
			case 62: makemaz("mazes-62"); return;
			case 63: makemaz("mazes-63"); return;
			case 64: makemaz("mazes-64"); return;
			case 65: makemaz("mazes-65"); return;
			case 66: makemaz("mazes-66"); return;
			case 67: makemaz("mazes-67"); return;
			case 68: makemaz("mazes-68"); return;
			case 69: makemaz("mazes-69"); return;

			}

	    } else if ((!rn2(u.randomquestlevels) && !rn2(3))
				&& !In_V_tower(&u.uz) && !Invocation_lev(&u.uz)
				&& (!rn2(10) || depth(&u.uz) > 1)
				&& (In_dod(&u.uz) || In_mines(&u.uz) || In_Devnull(&u.uz) || In_greencross(&u.uz) || In_illusorycastle(&u.uz) || In_deepmines(&u.uz) || In_ZAPM(&u.uz) || In_sokoban(&u.uz) || In_towndungeon(&u.uz) || !strcmp(dungeons[u.uz.dnum].dname, "Grund's Stronghold") || !strcmp(dungeons[u.uz.dnum].dname, "The Ice Queen's Realm") || !strcmp(dungeons[u.uz.dnum].dname, "The Temple of Moloch") || !strcmp(dungeons[u.uz.dnum].dname, "The Giant Caverns") || !strcmp(dungeons[u.uz.dnum].dname, "The Sunless Sea") || !strcmp(dungeons[u.uz.dnum].dname, "The Spider Caves") || !strcmp(dungeons[u.uz.dnum].dname, "The Lost Tomb") || !strcmp(dungeons[u.uz.dnum].dname, "Yendorian Tower") || !strcmp(dungeons[u.uz.dnum].dname, "Bell Caves") || !strcmp(dungeons[u.uz.dnum].dname, "Forging Chamber") || !strcmp(dungeons[u.uz.dnum].dname, "Dead Grounds") || !strcmp(dungeons[u.uz.dnum].dname, "Ordered Chaos") || !strcmp(dungeons[u.uz.dnum].dname, "The Wyrm Caves") || !strcmp(dungeons[u.uz.dnum].dname, "One-eyed Sam's Market") || !strcmp(dungeons[u.uz.dnum].dname, "Fort Ludios") || In_restingzone(&u.uz) ) ) {

ghnhom1:
		{
		    char fillname[16];
		    switch (rnd(5)) {

			  case 1:
		        sprintf(fillname, "%s-1", urole.filecode); break;
			  case 2:
		        sprintf(fillname, "%s-2", urole.filecode); break;
			  case 3:
		        sprintf(fillname, "%s-3", urole.filecode); break;
			  case 4:
		        sprintf(fillname, "%s-4", urole.filecode); break;
			  case 5:
		        sprintf(fillname, "%s-5", urole.filecode); break;

		    }
		    makemaz(fillname);
		    return;
		}

	    } else if ((!rn2(u.randomquestlevels) && !rn2(3)) && !In_V_tower(&u.uz) && !Invocation_lev(&u.uz) && (In_gehennom(&u.uz) || In_sheol(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz) || In_angmar(&u.uz) || In_emynluin(&u.uz) || In_swimmingpool(&u.uz) || In_hellbathroom(&u.uz) || !strcmp(dungeons[u.uz.dnum].dname, "Frankenstein's Lab") ) ) {

			if (rn2(3)) goto ghnhom1;

		    char fillname[16];
		    switch (rnd(5)) {

			  case 1:
		        sprintf(fillname, "%s-6", urole.filecode); break;
			  case 2:
		        sprintf(fillname, "%s-7", urole.filecode); break;
			  case 3:
		        sprintf(fillname, "%s-8", urole.filecode); break;
			  case 4:
		        sprintf(fillname, "%s-9", urole.filecode); break;
			  case 5:
		        sprintf(fillname, "%s-0", urole.filecode); break;

		    }
		    makemaz(fillname);
		    return;

	    } else if (dungeons[u.uz.dnum].proto[0] && !In_V_tower(&u.uz) && !In_angmar(&u.uz) && !In_emynluin(&u.uz) && !In_hellbathroom(&u.uz) && !Invocation_lev(&u.uz) && (rn2(2)) ) {

		    if (rn2(3)) {

			switch (rnd(69)) {

			case 1: makemaz("mazes-1"); return;
			case 2: makemaz("mazes-2"); return;
			case 3: makemaz("mazes-3"); return;
			case 4: makemaz("mazes-4"); return;
			case 5: makemaz("mazes-5"); return;
			case 6: makemaz("mazes-6"); return;
			case 7: makemaz("mazes-7"); return;
			case 8: makemaz("mazes-8"); return;
			case 9: makemaz("mazes-9"); return;
			case 10: makemaz("mazes-10"); return;
			case 11: makemaz("mazes-11"); return;
			case 12: makemaz("mazes-12"); return;
			case 13: makemaz("mazes-13"); return;
			case 14: makemaz("mazes-14"); return;
			case 15: makemaz("mazes-15"); return;
			case 16: makemaz("mazes-16"); return;
			case 17: makemaz("mazes-17"); return;
			case 18: makemaz("mazes-18"); return;
			case 19: makemaz("mazes-19"); return;
			case 20: makemaz("mazes-20"); return;
			case 21: makemaz("mazes-21"); return;
			case 22: makemaz("mazes-22"); return;
			case 23: makemaz("mazes-23"); return;
			case 24: makemaz("mazes-24"); return;
			case 25: makemaz("mazes-25"); return;
			case 26: makemaz("mazes-26"); return;
			case 27: makemaz("mazes-27"); return;
			case 28: makemaz("mazes-28"); return;
			case 29: makemaz("mazes-29"); return;
			case 30: makemaz("mazes-30"); return;
			case 31: makemaz("mazes-31"); return;
			case 32: makemaz("mazes-32"); return;
			case 33: makemaz("mazes-33"); return;
			case 34: makemaz("mazes-34"); return;
			case 35: makemaz("mazes-35"); return;
			case 36: makemaz("mazes-36"); return;
			case 37: makemaz("mazes-37"); return;
			case 38: makemaz("mazes-38"); return;
			case 39: makemaz("mazes-39"); return;
			case 40: makemaz("mazes-40"); return;
			case 41: makemaz("mazes-41"); return;
			case 42: makemaz("mazes-42"); return;
			case 43: makemaz("mazes-43"); return;
			case 44: makemaz("mazes-44"); return;
			case 45: makemaz("mazes-45"); return;
			case 46: makemaz("mazes-46"); return;
			case 47: makemaz("mazes-47"); return;
			case 48: makemaz("mazes-48"); return;
			case 49: makemaz("mazes-49"); return;
			case 50: makemaz("mazes-50"); return;
			case 51: makemaz("mazes-51"); return;
			case 52: makemaz("mazes-52"); return;
			case 53: makemaz("mazes-53"); return;
			case 54: makemaz("mazes-54"); return;
			case 55: makemaz("mazes-55"); return;
			case 56: makemaz("mazes-56"); return;
			case 57: makemaz("mazes-57"); return;
			case 58: makemaz("mazes-58"); return;
			case 59: makemaz("mazes-59"); return;
			case 60: makemaz("mazes-60"); return;
			case 61: makemaz("mazes-61"); return;
			case 62: makemaz("mazes-62"); return;
			case 63: makemaz("mazes-63"); return;
			case 64: makemaz("mazes-64"); return;
			case 65: makemaz("mazes-65"); return;
			case 66: makemaz("mazes-66"); return;
			case 67: makemaz("mazes-67"); return;
			case 68: makemaz("mazes-68"); return;
			case 69: makemaz("mazes-69"); return;

			}

		    } else makemaz("");
		    return;

	    } else if (In_mines(&u.uz) && rn2((iswarper ? 5 : 50) )) {
		    if (rn2(10)) makemaz("minefill");
		    else switch (rnd(7)) {
			case 1: makemaz("minefila"); break;
			case 2: makemaz("minefilb"); break;
			case 3: makemaz("minefilc"); break;
			case 4: makemaz("minefild"); break;
			case 5: makemaz("minefile"); break;
			case 6: makemaz("minefilf"); break;
			case 7: makemaz("minefilg"); break;
		    }
		    return;
	    } else if (In_emynluin(&u.uz) && rn2(2)) {
			makemaz("rndmazea"); return;
	    } else if (In_deepmines(&u.uz) && rn2((iswarper ? 5 : 50) )) {
		    switch (rnd(15)) {
			case 1: makemaz("deepfila"); break;
			case 2: makemaz("deepfilb"); break;
			case 3: makemaz("deepfilc"); break;
			case 4: makemaz("deepfild"); break;
			case 5: makemaz("deepfile"); break;
			case 6: makemaz("deepfilf"); break;
			case 7: makemaz("deepfilg"); break;
			case 8: makemaz("deepfilh"); break;
			case 9: makemaz("deepfili"); break;
			case 10: makemaz("deepfilj"); break;
			case 11: makemaz("deepfilk"); break;
			case 12: makemaz("deepfill"); break;
			case 13: makemaz("deepfilm"); break;
			case 14: makemaz("deepfiln"); break;
			case 15: makemaz("deepfilo"); break;
		    }
		    return;

	    } else if (In_quest(&u.uz)) {
		    char        fillname[16];
		    s_level	*loc_lev;

			/* This must be the player's starting role, even if recursion was triggered, to prevent a segfault. */

		    sprintf(fillname, "%s-loca", ustartrole.filecode);
		    loc_lev = find_level(fillname);

		    sprintf(fillname, "%s-fil", ustartrole.filecode);
		    strcat(fillname,
			   (u.uz.dlevel < loc_lev->dlevel.dlevel) ? "a" : "b");
		    makemaz(fillname);
		    return;

	    } else if( /*u.uz.dnum == wiz1_level.dnum || u.uz.dnum == (wiz1_level.dnum + 1) || u.uz.dnum == (wiz1_level.dnum + 2) ||*/ (In_V_tower(&u.uz)) || Invocation_lev(&u.uz) || (In_hell(&u.uz) && issoviet) || 

	/* "Revert change allowing Room/Corridors in Gehennom. Gehennom should only have mazes and the specified special levels again." In Soviet Russia, people actually like the endless boring mazes of the Gehennom for some inexplicable reason. Why don't they just turn Nethack into an 100-level dungeon that has only mazes??? --Amy */

		 (In_hell(&u.uz) && !In_angmar(&u.uz) && !In_emynluin(&u.uz) && !In_hellbathroom(&u.uz) && (!rn2(2) && (In_sheol(&u.uz) ? rn2(iswarper ? 2 : 5) : rn2(iswarper ? 3 : 10) ) )  ) || /* allowing random rooms-and-corridors in Gehennom --Amy */
		  (rn2(5) && u.uz.dnum == medusa_level.dnum
			  && depth(&u.uz) > depth(&medusa_level)) || (In_mainframe(&u.uz) && rn2(10) ) ) {

		    if (rn2(3) && !In_V_tower(&u.uz) && !Invocation_lev(&u.uz) ) {

			switch (rnd(69)) {

			case 1: makemaz("mazes-1"); return;
			case 2: makemaz("mazes-2"); return;
			case 3: makemaz("mazes-3"); return;
			case 4: makemaz("mazes-4"); return;
			case 5: makemaz("mazes-5"); return;
			case 6: makemaz("mazes-6"); return;
			case 7: makemaz("mazes-7"); return;
			case 8: makemaz("mazes-8"); return;
			case 9: makemaz("mazes-9"); return;
			case 10: makemaz("mazes-10"); return;
			case 11: makemaz("mazes-11"); return;
			case 12: makemaz("mazes-12"); return;
			case 13: makemaz("mazes-13"); return;
			case 14: makemaz("mazes-14"); return;
			case 15: makemaz("mazes-15"); return;
			case 16: makemaz("mazes-16"); return;
			case 17: makemaz("mazes-17"); return;
			case 18: makemaz("mazes-18"); return;
			case 19: makemaz("mazes-19"); return;
			case 20: makemaz("mazes-20"); return;
			case 21: makemaz("mazes-21"); return;
			case 22: makemaz("mazes-22"); return;
			case 23: makemaz("mazes-23"); return;
			case 24: makemaz("mazes-24"); return;
			case 25: makemaz("mazes-25"); return;
			case 26: makemaz("mazes-26"); return;
			case 27: makemaz("mazes-27"); return;
			case 28: makemaz("mazes-28"); return;
			case 29: makemaz("mazes-29"); return;
			case 30: makemaz("mazes-30"); return;
			case 31: makemaz("mazes-31"); return;
			case 32: makemaz("mazes-32"); return;
			case 33: makemaz("mazes-33"); return;
			case 34: makemaz("mazes-34"); return;
			case 35: makemaz("mazes-35"); return;
			case 36: makemaz("mazes-36"); return;
			case 37: makemaz("mazes-37"); return;
			case 38: makemaz("mazes-38"); return;
			case 39: makemaz("mazes-39"); return;
			case 40: makemaz("mazes-40"); return;
			case 41: makemaz("mazes-41"); return;
			case 42: makemaz("mazes-42"); return;
			case 43: makemaz("mazes-43"); return;
			case 44: makemaz("mazes-44"); return;
			case 45: makemaz("mazes-45"); return;
			case 46: makemaz("mazes-46"); return;
			case 47: makemaz("mazes-47"); return;
			case 48: makemaz("mazes-48"); return;
			case 49: makemaz("mazes-49"); return;
			case 50: makemaz("mazes-50"); return;
			case 51: makemaz("mazes-51"); return;
			case 52: makemaz("mazes-52"); return;
			case 53: makemaz("mazes-53"); return;
			case 54: makemaz("mazes-54"); return;
			case 55: makemaz("mazes-55"); return;
			case 56: makemaz("mazes-56"); return;
			case 57: makemaz("mazes-57"); return;
			case 58: makemaz("mazes-58"); return;
			case 59: makemaz("mazes-59"); return;
			case 60: makemaz("mazes-60"); return;
			case 61: makemaz("mazes-61"); return;
			case 62: makemaz("mazes-62"); return;
			case 63: makemaz("mazes-63"); return;
			case 64: makemaz("mazes-64"); return;
			case 65: makemaz("mazes-65"); return;
			case 66: makemaz("mazes-66"); return;
			case 67: makemaz("mazes-67"); return;
			case 68: makemaz("mazes-68"); return;
			case 69: makemaz("mazes-69"); return;

			}

		    } else makemaz("");
		    return;

	    }
	}

	/* very random levels --Amy */

	if ( 
		(In_dod(&u.uz) && (!rn2(100) || depth(&u.uz) > 1) && !rn2(iswarper ? 10 : 100))
		|| (In_mines(&u.uz) && rn2(1000) /* check moved upwards */ ) || (In_sokoban(&u.uz) && !issokosolver && rn2(iswarper ? 5 : 2)) || (In_towndungeon(&u.uz) && !rn2(iswarper ? 3 : 20)) || (In_deepmines(&u.uz) && rn2(2)) || (In_illusorycastle(&u.uz) && !rn2(iswarper ? 3 : 20)) || (In_sewerplant(&u.uz) && !rn2(iswarper ? 20 : 50)) || (In_spacebase(&u.uz) && !rn2(iswarper ? 4 : 30)) || (In_gammacaves(&u.uz) && !rn2(iswarper ? 20 : 50)) || (In_greencross(&u.uz) && !rn2(iswarper ? 10 : 100)) || (In_mainframe(&u.uz) && !rn2(iswarper ? 3 : 20)) || (rn2(5) && (!strcmp(dungeons[u.uz.dnum].dname, "Grund's Stronghold") || !strcmp(dungeons[u.uz.dnum].dname, "The Ice Queen's Realm") || In_Devnull(&u.uz) || !strcmp(dungeons[u.uz.dnum].dname, "The Temple of Moloch") || !strcmp(dungeons[u.uz.dnum].dname, "The Giant Caverns") || !strcmp(dungeons[u.uz.dnum].dname, "The Sunless Sea") || !strcmp(dungeons[u.uz.dnum].dname, "The Spider Caves") || !strcmp(dungeons[u.uz.dnum].dname, "Yendorian Tower") || !strcmp(dungeons[u.uz.dnum].dname, "Bell Caves") || !strcmp(dungeons[u.uz.dnum].dname, "Forging Chamber") || !strcmp(dungeons[u.uz.dnum].dname, "Dead Grounds") || !strcmp(dungeons[u.uz.dnum].dname, "Ordered Chaos") || !strcmp(dungeons[u.uz.dnum].dname, "The Lost Tomb") || !strcmp(dungeons[u.uz.dnum].dname, "The Wyrm Caves") || !strcmp(dungeons[u.uz.dnum].dname, "One-eyed Sam's Market") || !strcmp(dungeons[u.uz.dnum].dname, "Fort Ludios") || In_restingzone(&u.uz) ) ) ) {

ghnhom2:
	    switch (rnd(117)) {

	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5:
	    case 6:
	    case 7:
	    case 8:
	    case 9:
	    case 10:

		switch (rnd(60)) {

			case 1: makemaz("bigrm-1"); return;
			case 2: makemaz("bigrm-2"); return;
			case 3: makemaz("bigrm-3"); return;
			case 4: makemaz("bigrm-4"); return;
			case 5: makemaz("bigrm-5"); return;
			case 6: makemaz("bigrm-6"); return;
			case 7: makemaz("bigrm-7"); return;
			case 8: makemaz("bigrm-8"); return;
			case 9: makemaz("bigrm-9"); return;
			case 10: makemaz("bigrm-10"); return;
			case 11: makemaz("bigrm-11"); return;
			case 12: makemaz("bigrm-12"); return;
			case 13: makemaz("bigrm-13"); return;
			case 14: makemaz("bigrm-14"); return;
			case 15: makemaz("bigrm-15"); return;
			case 16: makemaz("bigrm-16"); return;
			case 17: makemaz("bigrm-17"); return;
			case 18: makemaz("bigrm-18"); return;
			case 19: makemaz("bigrm-19"); return;
			case 20: makemaz("bigrm-20"); return;
			case 21: makemaz("bigrm-21"); return;
			case 22: makemaz("bigrm-22"); return;
			case 23: makemaz("bigrm-23"); return;
			case 24: makemaz("bigrm-24"); return;
			case 25: makemaz("bigrm-25"); return;
			case 26: makemaz("bigrm-26"); return;
			case 27: makemaz("bigrm-27"); return;
			case 28: makemaz("bigrm-28"); return;
			case 29: makemaz("bigrm-29"); return;
			case 30: makemaz("bigrm-30"); return;
			case 31: makemaz("bigrm-31"); return;
			case 32: makemaz("bigrm-32"); return;
			case 33: makemaz("bigrm-33"); return;
			case 34: makemaz("bigrm-34"); return;
			case 35: makemaz("bigrm-35"); return;
			case 36: makemaz("bigrm-36"); return;
			case 37: makemaz("bigrm-37"); return;
			case 38: makemaz("bigrm-38"); return;
			case 39: makemaz("bigrm-39"); return;
			case 40: makemaz("bigrm-40"); return;
			case 41: makemaz("bigrm-41"); return;
			case 42: makemaz("bigrm-42"); return;
			case 43: makemaz("bigrm-43"); return;
			case 44: makemaz("bigrm-44"); return;
			case 45: makemaz("bigrm-45"); return;
			case 46: makemaz("bigrm-46"); return;
			case 47: makemaz("bigrm-47"); return;
			case 48: makemaz("bigrm-48"); return;
			case 49: makemaz("bigrm-49"); return;
			case 50: makemaz("bigrm-50"); return;
			case 51: makemaz("bigrm-51"); return;
			case 52: makemaz("bigrm-52"); return;
			case 53: makemaz("bigrm-53"); return;
			case 54: makemaz("bigrm-54"); return;
			case 55: makemaz("bigrm-55"); return;
			case 56: makemaz("bigrm-56"); return;
			case 57: makemaz("bigrm-57"); return;
			case 58: makemaz("bigrm-58"); return;
			case 59: makemaz("bigrm-59"); return;
			case 60: makemaz("bigrm-60"); return;

		}
		break;

	    case 11:
	    case 12:
	    case 13:
	    case 14:
	    case 15:
	    case 16:
	    case 17:
	    case 18:
	    case 19:
	    case 20:

		switch (rnd(97)) {

			case 1: makemaz("unhck-1"); return;
			case 2: makemaz("unhck-2"); return;
			case 3: makemaz("unhck-3"); return;
			case 4: makemaz("unhck-4"); return;
			case 5: makemaz("unhck-5"); return;
			case 6: makemaz("unhck-6"); return;
			case 7: makemaz("unhck-7"); return;
			case 8: makemaz("unhck-8"); return;
			case 9: makemaz("unhck-9"); return;
			case 10: makemaz("unhck-10"); return;
			case 11: makemaz("unhck-11"); return;
			case 12: makemaz("unhck-12"); return;
			case 13: makemaz("unhck-13"); return;
			case 14: makemaz("unhck-14"); return;
			case 15: makemaz("unhck-15"); return;
			case 16: makemaz("unhck-16"); return;
			case 17: makemaz("unhck-17"); return;
			case 18: makemaz("unhck-18"); return;
			case 19: makemaz("unhck-19"); return;
			case 20: makemaz("unhck-20"); return;
			case 21: makemaz("unhck-21"); return;
			case 22: makemaz("unhck-22"); return;
			case 23: makemaz("unhck-23"); return;
			case 24: makemaz("unhck-24"); return;
			case 25: makemaz("unhck-25"); return;
			case 26: makemaz("unhck-26"); return;
			case 27: makemaz("unhck-27"); return;
			case 28: makemaz("unhck-28"); return;
			case 29: makemaz("unhck-29"); return;
			case 30: makemaz("unhck-30"); return;
			case 31: makemaz("unhck-31"); return;
			case 32: makemaz("unhck-32"); return;
			case 33: makemaz("unhck-33"); return;
			case 34: makemaz("unhck-34"); return;
			case 35: makemaz("unhck-35"); return;
			case 36: makemaz("unhck-36"); return;
			case 37: makemaz("unhck-37"); return;
			case 38: makemaz("unhck-38"); return;
			case 39: makemaz("unhck-39"); return;
			case 40: makemaz("unhck-40"); return;
			case 41: makemaz("unhck-41"); return;
			case 42: makemaz("unhck-42"); return;
			case 43: makemaz("unhck-43"); return;
			case 44: makemaz("unhck-44"); return;
			case 45: makemaz("unhck-45"); return;
			case 46: makemaz("unhck-46"); return;
			case 47: makemaz("unhck-47"); return;
			case 48: makemaz("unhck-48"); return;
			case 49: makemaz("unhck-49"); return;
			case 50: makemaz("unhck-50"); return;
			case 51: makemaz("unhck-51"); return;
			case 52: makemaz("unhck-52"); return;
			case 53: makemaz("unhck-53"); return;
			case 54: makemaz("unhck-54"); return;
			case 55: makemaz("unhck-55"); return;
			case 56: makemaz("unhck-56"); return;
			case 57: makemaz("unhck-57"); return;
			case 58: makemaz("unhck-58"); return;
			case 59: makemaz("unhck-59"); return;
			case 60: makemaz("unhck-60"); return;
			case 61: makemaz("unhck-61"); return;
			case 62: makemaz("unhck-62"); return;
			case 63: makemaz("unhck-63"); return;
			case 64: makemaz("unhck-64"); return;
			case 65: makemaz("unhck-65"); return;
			case 66: makemaz("unhck-66"); return;
			case 67: makemaz("unhck-67"); return;
			case 68: makemaz("unhck-68"); return;
			case 69: makemaz("unhck-69"); return;
			case 70: makemaz("unhck-70"); return;
			case 71: makemaz("unhck-71"); return;
			case 72: makemaz("unhck-72"); return;
			case 73: makemaz("unhck-73"); return;
			case 74: makemaz("unhck-74"); return;
			case 75: makemaz("unhck-75"); return;
			case 76: makemaz("unhck-76"); return;
			case 77: makemaz("unhck-77"); return;
			case 78: makemaz("unhck-78"); return;
			case 79: makemaz("unhck-79"); return;
			case 80: makemaz("unhck-80"); return;
			case 81: makemaz("unhck-81"); return;
			case 82: makemaz("unhck-82"); return;
			case 83: makemaz("unhck-83"); return;
			case 84: makemaz("unhck-84"); return;
			case 85: makemaz("unhck-85"); return;
			case 86: makemaz("unhck-86"); return;
			case 87: makemaz("unhck-87"); return;
			case 88: makemaz("unhck-88"); return;
			case 89: makemaz("unhck-89"); return;
			case 90: makemaz("unhck-90"); return;
			case 91: makemaz("unhck-91"); return;
			case 92: makemaz("unhck-92"); return;
			case 93: makemaz("unhck-93"); return;
			case 94: makemaz("unhck-94"); return;
			case 95: makemaz("unhck-95"); return;
			case 96: makemaz("unhck-96"); return;
			case 97: makemaz("minusw"); return;

		}
		break;

	    case 21:

		switch (rnd(5)) {

			case 1: makemaz("intpla-1"); return;
			case 2: makemaz("intpla-2"); return;
			case 3: makemaz("intpla-3"); return;
			case 4: makemaz("intpla-4"); return;
			case 5: makemaz("intpla-5"); return;

		}
		break;

	    case 22:
	    case 23:
	    case 24:

		switch (rnd(14)) {

			case 1: makemaz("minefill"); return;
			case 2: makemaz("minefila"); return;
			case 3: makemaz("minefilb"); return;
			case 4: makemaz("minefilc"); return;
			case 5: makemaz("minefild"); return;
			case 6: makemaz("minefile"); return;
			case 7: makemaz("minefilf"); return;
			case 8: makemaz("minefilg"); return;
			case 9: makemaz("minefill"); return;
			case 10: makemaz("minefill"); return;
			case 11: makemaz("minefill"); return;
			case 12: makemaz("minefill"); return;
			case 13: makemaz("minefill"); return;
			case 14: makemaz("minefill"); return;

		}
		break;

	    case 25:
	    case 26:
	    case 27:
	    case 28:
	    case 29:

		switch (rnd(21)) {

			case 1: makemaz("mintn-1"); return;
			case 2: makemaz("mintn-2"); return;
			case 3: makemaz("mintn-3"); return;
			case 4: makemaz("mintn-4"); return;
			case 5: makemaz("mintn-5"); return;
			case 6: makemaz("mintn-6"); return;
			case 7: makemaz("mintn-7"); return;
			case 8: makemaz("mintn-8"); return;
			case 9: makemaz("mintn-9"); return;
			case 10: makemaz("mintn-10"); return;
			case 11: makemaz("mintn-11"); return;
			case 12: makemaz("mintn-12"); return;
			case 13: makemaz("mintn-13"); return;
			case 14: makemaz("mintn-14"); return;
			case 15: makemaz("mintn-15"); return;
			case 16: makemaz("mintn-16"); return;
			case 17: makemaz("mintn-17"); return;
			case 18: makemaz("mintn-18"); return;
			case 19: makemaz("mintn-19"); return;
			case 20: makemaz("mintn-20"); return;
			case 21: makemaz("mintn-21"); return;

		}
		break;

	    case 30:
	    case 31:

		switch (rnd(10)) {

			case 1: makemaz("mnend-1"); return;
			case 2: makemaz("mnend-2"); return;
			case 3: makemaz("mnend-3"); return;
			case 4: makemaz("mnend-4"); return;
			case 5: makemaz("mnend-5"); return;
			case 6: makemaz("mnend-6"); return;
			case 7: makemaz("mnend-7"); return;
			case 8: makemaz("mnend-8"); return;
			case 9: makemaz("mnend-9"); return;
			case 10: makemaz("mnend-10"); return;

		}
		break;

	    case 32:
	    case 33:

		switch (rnd(18)) {

			case 1: makemaz("eking-1"); return;
			case 2: makemaz("eking-2"); return;
			case 3: makemaz("eking-3"); return;
			case 4: makemaz("eking-4"); return;
			case 5: makemaz("eking-5"); return;
			case 6: makemaz("eking-6"); return;
			case 7: makemaz("eking-7"); return;
			case 8: makemaz("eking-8"); return;
			case 9: makemaz("eking-9"); return;
			case 10: makemaz("eking-10"); return;
			case 11: makemaz("eking-11"); return;
			case 12: makemaz("eking-12"); return;
			case 13: makemaz("eking-13"); return;
			case 14: makemaz("eking-14"); return;
			case 15: makemaz("eking-15"); return;
			case 16: makemaz("eking-16"); return;
			case 17: makemaz("eking-17"); return;
			case 18: makemaz("eking-18"); return;

		}
		break;

	    case 34:
	    case 35:
	    case 36:
	    case 37:
	    case 38:
	    case 39:
	    case 40:
	    case 41:
	    case 42:
	    case 43:

		switch (rnd(154)) {

			case 1: makemaz("soko2-1"); return;
			case 2: makemaz("soko2-2"); return;
			case 3: makemaz("soko2-3"); return;
			case 4: makemaz("soko2-4"); return;
			case 5: makemaz("soko2-5"); return;
			case 6: makemaz("soko2-6"); return;
			case 7: makemaz("soko2-7"); return;
			case 8: makemaz("soko2-8"); return;
			case 9: makemaz("soko2-9"); return;
			case 10: makemaz("soko2-10"); return;
			case 11: makemaz("soko2-11"); return;
			case 12: makemaz("soko2-12"); return;
			case 13: makemaz("soko2-13"); return;
			case 14: makemaz("soko2-14"); return;
			case 15: makemaz("soko2-15"); return;
			case 16: makemaz("soko2-16"); return;
			case 17: makemaz("soko2-17"); return;
			case 18: makemaz("soko2-18"); return;
			case 19: makemaz("soko2-19"); return;
			case 20: makemaz("soko2-20"); return;
			case 21: makemaz("soko2-21"); return;
			case 22: makemaz("soko2-22"); return;
			case 23: makemaz("soko2-23"); return;
			case 24: makemaz("soko2-24"); return;
			case 25: makemaz("soko2-25"); return;
			case 26: makemaz("soko2-26"); return;
			case 27: makemaz("soko2-27"); return;
			case 28: makemaz("soko2-28"); return;
			case 29: makemaz("soko2-29"); return;
			case 30: makemaz("soko2-30"); return;
			case 31: makemaz("soko2-31"); return;
			case 32: makemaz("soko2-32"); return;
			case 33: makemaz("soko2-33"); return;
			case 34: makemaz("soko2-34"); return;
			case 35: makemaz("soko2-35"); return;
			case 36: makemaz("soko2-36"); return;
			case 37: makemaz("soko2-37"); return;
			case 38: makemaz("soko2-38"); return;
			case 39: makemaz("soko2-39"); return;
			case 40: makemaz("soko2-40"); return;
			case 41: makemaz("soko2-41"); return;
			case 42: makemaz("soko2-42"); return;
			case 43: makemaz("soko2-43"); return;
			case 44: makemaz("soko2-44"); return;

			case 45: makemaz("soko3-1"); return;
			case 46: makemaz("soko3-2"); return;
			case 47: makemaz("soko3-3"); return;
			case 48: makemaz("soko3-4"); return;
			case 49: makemaz("soko3-5"); return;
			case 50: makemaz("soko3-6"); return;
			case 51: makemaz("soko3-7"); return;
			case 52: makemaz("soko3-8"); return;
			case 53: makemaz("soko3-9"); return;
			case 54: makemaz("soko3-10"); return;
			case 55: makemaz("soko3-11"); return;
			case 56: makemaz("soko3-12"); return;
			case 57: makemaz("soko3-13"); return;
			case 58: makemaz("soko3-14"); return;
			case 59: makemaz("soko3-15"); return;
			case 60: makemaz("soko3-16"); return;
			case 61: makemaz("soko3-17"); return;
			case 62: makemaz("soko3-18"); return;
			case 63: makemaz("soko3-19"); return;
			case 64: makemaz("soko3-20"); return;
			case 65: makemaz("soko3-21"); return;
			case 66: makemaz("soko3-22"); return;
			case 67: makemaz("soko3-23"); return;
			case 68: makemaz("soko3-24"); return;
			case 69: makemaz("soko3-25"); return;
			case 70: makemaz("soko3-26"); return;
			case 71: makemaz("soko3-27"); return;
			case 72: makemaz("soko3-28"); return;
			case 73: makemaz("soko3-29"); return;

			case 74: makemaz("soko5-1"); return;
			case 75: makemaz("soko5-2"); return;
			case 76: makemaz("soko5-3"); return;
			case 77: makemaz("soko5-4"); return;
			case 78: makemaz("soko5-5"); return;
			case 79: makemaz("soko5-6"); return;
			case 80: makemaz("soko5-7"); return;
			case 81: makemaz("soko5-8"); return;
			case 82: makemaz("soko5-9"); return;
			case 83: makemaz("soko5-10"); return;
			case 84: makemaz("soko5-11"); return;
			case 85: makemaz("soko5-12"); return;
			case 86: makemaz("soko5-13"); return;
			case 87: makemaz("soko5-14"); return;
			case 88: makemaz("soko5-15"); return;
			case 89: makemaz("soko5-16"); return;
			case 90: makemaz("soko5-17"); return;
			case 91: makemaz("soko5-18"); return;
			case 92: makemaz("soko5-19"); return;
			case 93: makemaz("soko5-20"); return;
			case 94: makemaz("soko5-21"); return;
			case 95: makemaz("soko5-22"); return;
			case 96: makemaz("soko5-23"); return;
			case 97: makemaz("soko5-24"); return;
			case 98: makemaz("soko5-25"); return;
			case 99: makemaz("soko5-26"); return;
			case 100: makemaz("soko5-27"); return;
			case 101: makemaz("soko5-28"); return;

			case 102: makemaz("soko6-1"); return;
			case 103: makemaz("soko6-2"); return;
			case 104: makemaz("soko6-3"); return;
			case 105: makemaz("soko6-4"); return;
			case 106: makemaz("soko6-5"); return;
			case 107: makemaz("soko6-6"); return;
			case 108: makemaz("soko6-7"); return;
			case 109: makemaz("soko6-8"); return;
			case 110: makemaz("soko6-9"); return;
			case 111: makemaz("soko6-10"); return;
			case 112: makemaz("soko6-11"); return;
			case 113: makemaz("soko6-12"); return;
			case 114: makemaz("soko6-13"); return;
			case 115: makemaz("soko6-14"); return;
			case 116: makemaz("soko6-15"); return;
			case 117: makemaz("soko6-16"); return;
			case 118: makemaz("soko6-17"); return;
			case 119: makemaz("soko6-18"); return;
			case 120: makemaz("soko6-19"); return;
			case 121: makemaz("soko6-20"); return;
			case 122: makemaz("soko6-21"); return;
			case 123: makemaz("soko6-22"); return;
			case 124: makemaz("soko6-23"); return;
			case 125: makemaz("soko6-24"); return;
			case 126: makemaz("soko6-25"); return;
			case 127: makemaz("soko6-26"); return;
			case 128: makemaz("soko6-27"); return;
			case 129: makemaz("soko6-28"); return;
			case 130: makemaz("soko6-29"); return;
			case 131: makemaz("soko6-30"); return;
			case 132: makemaz("soko6-31"); return;
			case 133: makemaz("soko6-32"); return;
			case 134: makemaz("soko6-33"); return;
			case 135: makemaz("soko6-34"); return;
			case 136: makemaz("soko6-35"); return;
			case 137: makemaz("soko6-36"); return;
			case 138: makemaz("soko6-37"); return;
			case 139: makemaz("soko6-38"); return;
			case 140: makemaz("soko6-39"); return;
			case 141: makemaz("soko6-40"); return;
			case 142: makemaz("soko6-41"); return;
			case 143: makemaz("soko6-42"); return;
			case 144: makemaz("soko6-43"); return;
			case 145: makemaz("soko6-44"); return;
			case 146: makemaz("soko6-45"); return;
			case 147: makemaz("soko6-46"); return;
			case 148: makemaz("soko6-47"); return;
			case 149: makemaz("soko6-48"); return;
			case 150: makemaz("soko6-49"); return;
			case 151: makemaz("soko6-50"); return;
			case 152: makemaz("soko6-51"); return;

			case 153: makemaz("soko2-45"); return;

			case 154: makemaz("soko6-52"); return;

		}
		break;

	    case 44:
	    case 45:

		switch (rnd(15)) {

			case 1: makemaz("mall-1"); return;
			case 2: makemaz("mall-2"); return;
			case 3: makemaz("mall-3"); return;
			case 4: makemaz("mall-4"); return;
			case 5: makemaz("mall-5"); return;
			case 6: makemaz("mall-6"); return;
			case 7: makemaz("exmall-1"); return;
			case 8: makemaz("exmall-2"); return;
			case 9: makemaz("exmall-3"); return;
			case 10: makemaz("exmall-4"); return;
			case 11: makemaz("exmall-5"); return;
			case 12: makemaz("exmall-6"); return;
			case 13: makemaz("exmall-7"); return;
			case 14: makemaz("exmall-8"); return;
			case 15: makemaz("exmall-9"); return;

		}
		break;

	    case 46:

		makemaz("oracleX"); return;
		break;

	    case 47:

		switch (rnd(4)) {

			case 1: makemaz("erats-1"); return;
			case 2: makemaz("erats-2"); return;
			case 3: makemaz("erats-3"); return;
			case 4: makemaz("erats-4"); return;

		}
		break;

	    case 48:

		switch (rnd(6)) {

			case 1: makemaz("ekobol-1"); return;
			case 2: makemaz("ekobol-2"); return;
			case 3: makemaz("ekobol-3"); return;
			case 4: makemaz("ekobol-4"); return;
			case 5: makemaz("ekobol-5"); return;
			case 6: makemaz("ekobol-6"); return;

		}
		break;

	    case 49:

		switch (rnd(5)) {

			case 1: makemaz("enymp-1"); return;
			case 2: makemaz("enymp-2"); return;
			case 3: makemaz("enymp-3"); return;
			case 4: makemaz("enymp-4"); return;
			case 5: makemaz("enymp-5"); return;

		}
		break;

	    case 50:

		switch (rnd(9)) {

			case 1: makemaz("stor-1"); return;
			case 2: makemaz("stor-2"); return;
			case 3: makemaz("stor-3"); return;
			case 4: makemaz("stor-4"); return;
			case 5: makemaz("stor-5"); return;
			case 6: makemaz("stor-6"); return;
			case 7: makemaz("stor-7"); return;
			case 8: makemaz("stor-8"); return;
			case 9: makemaz("stor-9"); return;

		}
		break;

	    case 51:

		switch (rnd(8)) {

			case 1: makemaz("guild-1"); return;
			case 2: makemaz("guild-2"); return;
			case 3: makemaz("guild-3"); return;
			case 4: makemaz("guild-4"); return;
			case 5: makemaz("guild-5"); return;
			case 6: makemaz("guild-6"); return;
			case 7: makemaz("guild-7"); return;
			case 8: makemaz("guild-8"); return;

		}
		break;

	    case 52:

		switch (rnd(9)) {

			case 1: makemaz("forge"); return;
			case 2: makemaz("hitchE"); return;
			case 3: makemaz("compuE"); return;
			case 4: makemaz("keyE"); return;
			case 5: makemaz("robtn"); return;
			case 6: makemaz("rabhoE"); return;
			case 7: makemaz("machnE"); return;
			case 8: makemaz("orcbaE"); return;
			case 9: makemaz("sewpl"); return;

		}
		break;

	    case 53:
	    case 54:

		switch (rnd(12)) {

			case 1: makemaz("emedu-1"); return;
			case 2: makemaz("emedu-2"); return;
			case 3: makemaz("emedu-3"); return;
			case 4: makemaz("emedu-4"); return;
			case 5: makemaz("emedu-5"); return;
			case 6: makemaz("emedu-6"); return;
			case 7: makemaz("emedu-7"); return;
			case 8: makemaz("emedu-8"); return;
			case 9: makemaz("emedu-9"); return;
			case 10: makemaz("emedu-10"); return;
			case 11: makemaz("emedu-11"); return;
			case 12: makemaz("emedu-12"); return;

		}
		break;

	    case 55:
	    case 56:

		switch (rnd(15)) {

			case 1: makemaz("ecstl-1"); return;
			case 2: makemaz("ecstl-2"); return;
			case 3: makemaz("ecstl-3"); return;
			case 4: makemaz("ecstl-4"); return;
			case 5: makemaz("ecstl-5"); return;
			case 6: makemaz("ecstl-6"); return;
			case 7: makemaz("ecstl-7"); return;
			case 8: makemaz("ecstl-8"); return;
			case 9: makemaz("ecstl-9"); return;
			case 10: makemaz("ecstl-10"); return;
			case 11: makemaz("ecstl-11"); return;
			case 12: makemaz("ecstl-12"); return;
			case 13: makemaz("ecstl-13"); return;
			case 14: makemaz("ecstl-14"); return;
			case 15: makemaz("ecstl-15"); return;

		}
		break;

	    case 57:
	    case 58:
	    case 59:
	    case 60:
	    case 61:

		switch (rnd(88)) {

			case 1: makemaz("egehn-1"); return;
			case 2: makemaz("egehn-2"); return;
			case 3: makemaz("egehn-3"); return;
			case 4: makemaz("egehn-4"); return;
			case 5: makemaz("egehn-5"); return;
			case 6: makemaz("egehn-6"); return;
			case 7: makemaz("egehn-7"); return;
			case 8: makemaz("egehn-8"); return;
			case 9: makemaz("egehn-9"); return;
			case 10: makemaz("egehn-10"); return;
			case 11: makemaz("egehn-11"); return;
			case 12: makemaz("egehn-12"); return;
			case 13: makemaz("egehn-13"); return;
			case 14: makemaz("egehn-14"); return;
			case 15: makemaz("egehn-15"); return;
			case 16: makemaz("egehn-16"); return;
			case 17: makemaz("egehn-17"); return;
			case 18: makemaz("egehn-18"); return;
			case 19: makemaz("egehn-19"); return;
			case 20: makemaz("egehn-20"); return;
			case 21: makemaz("egehn-21"); return;
			case 22: makemaz("egehn-22"); return;
			case 23: makemaz("egehn-23"); return;
			case 24: makemaz("egehn-24"); return;
			case 25: makemaz("egehn-25"); return;
			case 26: makemaz("egehn-26"); return;
			case 27: makemaz("egehn-27"); return;
			case 28: makemaz("egehn-28"); return;
			case 29: makemaz("egehn-29"); return;
			case 30: makemaz("egehn-30"); return;
			case 31: makemaz("egehn-31"); return;
			case 32: makemaz("egehn-32"); return;
			case 33: makemaz("egehn-33"); return;
			case 34: makemaz("egehn-34"); return;
			case 35: makemaz("egehn-35"); return;
			case 36: makemaz("egehn-36"); return;
			case 37: makemaz("egehn-37"); return;
			case 38: makemaz("egehn-38"); return;
			case 39: makemaz("egehn-39"); return;
			case 40: makemaz("egehn-40"); return;
			case 41: makemaz("egehn-41"); return;
			case 42: makemaz("egehn-42"); return;
			case 43: makemaz("egehn-43"); return;
			case 44: makemaz("egehn-44"); return;
			case 45: makemaz("egehn-45"); return;
			case 46: makemaz("egehn-46"); return;
			case 47: makemaz("egehn-47"); return;
			case 48: makemaz("egehn-48"); return;
			case 49: makemaz("egehn-49"); return;
			case 50: makemaz("egehn-50"); return;
			case 51: makemaz("egehn-51"); return;
			case 52: makemaz("egehn-52"); return;
			case 53: makemaz("egehn-53"); return;
			case 54: makemaz("egehn-54"); return;
			case 55: makemaz("egehn-55"); return;
			case 56: makemaz("egehn-56"); return;
			case 57: makemaz("egehn-57"); return;
			case 58: makemaz("egehn-58"); return;
			case 59: makemaz("egehn-59"); return;
			case 60: makemaz("egehn-60"); return;
			case 61: makemaz("egehn-61"); return;
			case 62: makemaz("egehn-62"); return;
			case 63: makemaz("egehn-63"); return;
			case 64: makemaz("egehn-64"); return;
			case 65: makemaz("egehn-65"); return;
			case 66: makemaz("egehn-66"); return;
			case 67: makemaz("egehn-67"); return;
			case 68: makemaz("egehn-68"); return;
			case 69: makemaz("egehn-69"); return;
			case 70: makemaz("egehn-70"); return;
			case 71: makemaz("egehn-71"); return;
			case 72: makemaz("egehn-72"); return;
			case 73: makemaz("egehn-73"); return;
			case 74: makemaz("egehn-74"); return;
			case 75: makemaz("egehn-75"); return;
			case 76: makemaz("egehn-76"); return;
			case 77: makemaz("egehn-77"); return;
			case 78: makemaz("egehn-78"); return;
			case 79: makemaz("egehn-79"); return;
			case 80: makemaz("egehn-80"); return;
			case 81: makemaz("egehn-81"); return;
			case 82: makemaz("egehn-82"); return;
			case 83: makemaz("egehn-83"); return;
			case 84: makemaz("egehn-84"); return;
			case 85: makemaz("egehn-85"); return;
			case 86: makemaz("egehn-86"); return;
			case 87: makemaz("egehn-87"); return;
			case 88: makemaz("egehn-88"); return;

		}
		break;

	    case 62:

		makemaz("schoolX"); return;
		break;

	    case 63:

		switch (rnd(4)) {

			case 1: makemaz("etown-1"); return;
			case 2: makemaz("etown-2"); return;
			case 3: makemaz("etown-3"); return;
			case 4: makemaz("etown-4"); return;

		}
		break;

	    case 64:

		switch (rnd(3)) {

			case 1: makemaz("egrund-1"); return;
			case 2: makemaz("egrund-2"); return;
			case 3: makemaz("egrund-3"); return;

		}
		break;

	    case 65:

		switch (rnd(3)) {

			case 1: makemaz("eknox-1"); return;
			case 2: makemaz("eknox-2"); return;
			case 3: makemaz("eknox-3"); return;

		}

		break;

	    case 66:

		makemaz("dragons"); return;
		break;

	    case 67:

		makemaz("etomb"); return;
		break;

	    case 68:

		makemaz("espiders"); return;
		break;

	    case 69:

		makemaz("esea"); return;
		break;

	    case 70:

		makemaz("emtemple"); return;
		break;

	    case 71:
	    case 72:
	    case 73:

		switch (rnd(22)) {

			case 1: makemaz("esheo-1"); return;
			case 2: makemaz("esheo-2"); return;
			case 3: makemaz("esheo-3"); return;
			case 4: makemaz("esheo-4"); return;
			case 5: makemaz("esheo-5"); return;
			case 6: makemaz("esheo-6"); return;
			case 7: makemaz("esheo-7"); return;
			case 8: makemaz("esheo-8"); return;
			case 9: makemaz("esheo-9"); return;
			case 10: makemaz("esheo-10"); return;
			case 11: makemaz("esheo-11"); return;
			case 12: makemaz("esheo-12"); return;
			case 13: makemaz("esheo-13"); return;
			case 14: makemaz("esheo-14"); return;
			case 15: makemaz("esheo-15"); return;
			case 16: makemaz("esheo-16"); return;
			case 17: makemaz("esheo-17"); return;
			case 18: makemaz("esheo-18"); return;
			case 19: makemaz("esheo-19"); return;
			case 20: makemaz("esheo-20"); return;
			case 21: makemaz("esheo-21"); return;
			case 22: makemaz("esheo-22"); return;

		}
		break;

	    case 74:
	    case 75:
	    case 76:
	    case 77:
	    case 78:
	    case 79:
	    case 80:
	    case 81:
	    case 82:
	    case 83:
	    case 84:
	    case 85:
	    case 86:
	    case 87:
	    case 88:
	    case 89:
	    case 90:
	    case 91:
	    case 92:
	    case 93:

		switch (rnd(855)) {

			case 1: makemaz("Aci-1"); return;
			case 2: makemaz("Aci-2"); return;
			case 3: makemaz("Aci-3"); return;
			case 4: makemaz("Aci-4"); return;
			case 5: makemaz("Aci-5"); return;
			case 6: makemaz("Act-1"); return;
			case 7: makemaz("Act-2"); return;
			case 8: makemaz("Act-3"); return;
			case 9: makemaz("Act-4"); return;
			case 10: makemaz("Act-5"); return;
			case 11: makemaz("Alt-1"); return;
			case 12: makemaz("Alt-2"); return;
			case 13: makemaz("Alt-3"); return;
			case 14: makemaz("Alt-4"); return;
			case 15: makemaz("Alt-5"); return;
			case 16: makemaz("Ama-1"); return;
			case 17: makemaz("Ama-2"); return;
			case 18: makemaz("Ama-3"); return;
			case 19: makemaz("Ama-4"); return;
			case 20: makemaz("Ama-5"); return;
			case 21: makemaz("Arc-1"); return;
			case 22: makemaz("Arc-2"); return;
			case 23: makemaz("Arc-3"); return;
			case 24: makemaz("Arc-4"); return;
			case 25: makemaz("Arc-5"); return;
			case 26: makemaz("Art-1"); return;
			case 27: makemaz("Art-2"); return;
			case 28: makemaz("Art-3"); return;
			case 29: makemaz("Art-4"); return;
			case 30: makemaz("Art-5"); return;
			case 31: makemaz("Ass-1"); return;
			case 32: makemaz("Ass-2"); return;
			case 33: makemaz("Ass-3"); return;
			case 34: makemaz("Ass-4"); return;
			case 35: makemaz("Ass-5"); return;
			case 36: makemaz("Aug-1"); return;
			case 37: makemaz("Aug-2"); return;
			case 38: makemaz("Aug-3"); return;
			case 39: makemaz("Aug-4"); return;
			case 40: makemaz("Aug-5"); return;
			case 41: makemaz("Bar-1"); return;
			case 42: makemaz("Bar-2"); return;
			case 43: makemaz("Bar-3"); return;
			case 44: makemaz("Bar-4"); return;
			case 45: makemaz("Bar-5"); return;
			case 46: makemaz("Brd-1"); return;
			case 47: makemaz("Brd-2"); return;
			case 48: makemaz("Brd-3"); return;
			case 49: makemaz("Brd-4"); return;
			case 50: makemaz("Brd-5"); return;
			case 51: makemaz("Bin-1"); return;
			case 52: makemaz("Bin-2"); return;
			case 53: makemaz("Bin-3"); return;
			case 54: makemaz("Bin-4"); return;
			case 55: makemaz("Bin-5"); return;
			case 56: makemaz("Ble-1"); return;
			case 57: makemaz("Ble-2"); return;
			case 58: makemaz("Ble-3"); return;
			case 59: makemaz("Ble-4"); return;
			case 60: makemaz("Ble-5"); return;
			case 61: makemaz("Blo-1"); return;
			case 62: makemaz("Blo-2"); return;
			case 63: makemaz("Blo-3"); return;
			case 64: makemaz("Blo-4"); return;
			case 65: makemaz("Blo-5"); return;
			case 66: makemaz("Bos-1"); return;
			case 67: makemaz("Bos-2"); return;
			case 68: makemaz("Bos-3"); return;
			case 69: makemaz("Bos-4"); return;
			case 70: makemaz("Bos-5"); return;
			case 71: makemaz("Bul-1"); return;
			case 72: makemaz("Bul-2"); return;
			case 73: makemaz("Bul-3"); return;
			case 74: makemaz("Bul-4"); return;
			case 75: makemaz("Bul-5"); return;
			case 76: makemaz("Cav-1"); return;
			case 77: makemaz("Cav-2"); return;
			case 78: makemaz("Cav-3"); return;
			case 79: makemaz("Cav-4"); return;
			case 80: makemaz("Cav-5"); return;
			case 81: makemaz("Che-1"); return;
			case 82: makemaz("Che-2"); return;
			case 83: makemaz("Che-3"); return;
			case 84: makemaz("Che-4"); return;
			case 85: makemaz("Che-5"); return;
			case 86: makemaz("Con-1"); return;
			case 87: makemaz("Con-2"); return;
			case 88: makemaz("Con-3"); return;
			case 89: makemaz("Con-4"); return;
			case 90: makemaz("Con-5"); return;
			case 91: makemaz("Coo-1"); return;
			case 92: makemaz("Coo-2"); return;
			case 93: makemaz("Coo-3"); return;
			case 94: makemaz("Coo-4"); return;
			case 95: makemaz("Coo-5"); return;
			case 96: makemaz("Cou-1"); return;
			case 97: makemaz("Cou-2"); return;
			case 98: makemaz("Cou-3"); return;
			case 99: makemaz("Cou-4"); return;
			case 100: makemaz("Cou-5"); return;
			case 101: makemaz("Abu-1"); return;
			case 102: makemaz("Abu-2"); return;
			case 103: makemaz("Abu-3"); return;
			case 104: makemaz("Abu-4"); return;
			case 105: makemaz("Abu-5"); return;
			case 106: makemaz("Dea-1"); return;
			case 107: makemaz("Dea-2"); return;
			case 108: makemaz("Dea-3"); return;
			case 109: makemaz("Dea-4"); return;
			case 110: makemaz("Dea-5"); return;
			case 111: makemaz("Div-1"); return;
			case 112: makemaz("Div-2"); return;
			case 113: makemaz("Div-3"); return;
			case 114: makemaz("Div-4"); return;
			case 115: makemaz("Div-5"); return;
			case 116: makemaz("Dol-1"); return;
			case 117: makemaz("Dol-2"); return;
			case 118: makemaz("Dol-3"); return;
			case 119: makemaz("Dol-4"); return;
			case 120: makemaz("Dol-5"); return;
			case 121: makemaz("Dru-1"); return;
			case 122: makemaz("Dru-2"); return;
			case 123: makemaz("Dru-3"); return;
			case 124: makemaz("Dru-4"); return;
			case 125: makemaz("Dru-5"); return;
			case 126: makemaz("Dun-1"); return;
			case 127: makemaz("Dun-2"); return;
			case 128: makemaz("Dun-3"); return;
			case 129: makemaz("Dun-4"); return;
			case 130: makemaz("Dun-5"); return;
			case 131: makemaz("Ele-1"); return;
			case 132: makemaz("Ele-2"); return;
			case 133: makemaz("Ele-3"); return;
			case 134: makemaz("Ele-4"); return;
			case 135: makemaz("Ele-5"); return;
			case 136: makemaz("Elp-1"); return;
			case 137: makemaz("Elp-2"); return;
			case 138: makemaz("Elp-3"); return;
			case 139: makemaz("Elp-4"); return;
			case 140: makemaz("Elp-5"); return;
			case 141: makemaz("Stu-1"); return;
			case 142: makemaz("Stu-2"); return;
			case 143: makemaz("Stu-3"); return;
			case 144: makemaz("Stu-4"); return;
			case 145: makemaz("Stu-5"); return;
			case 146: makemaz("Fir-1"); return;
			case 147: makemaz("Fir-2"); return;
			case 148: makemaz("Fir-3"); return;
			case 149: makemaz("Fir-4"); return;
			case 150: makemaz("Fir-5"); return;
			case 151: makemaz("Fla-1"); return;
			case 152: makemaz("Fla-2"); return;
			case 153: makemaz("Fla-3"); return;
			case 154: makemaz("Fla-4"); return;
			case 155: makemaz("Fla-5"); return;
			case 156: makemaz("Fox-1"); return;
			case 157: makemaz("Fox-2"); return;
			case 158: makemaz("Fox-3"); return;
			case 159: makemaz("Fox-4"); return;
			case 160: makemaz("Fox-5"); return;
			case 161: makemaz("Gam-1"); return;
			case 162: makemaz("Gam-2"); return;
			case 163: makemaz("Gam-3"); return;
			case 164: makemaz("Gam-4"); return;
			case 165: makemaz("Gam-5"); return;
			case 166: makemaz("Gan-1"); return;
			case 167: makemaz("Gan-2"); return;
			case 168: makemaz("Gan-3"); return;
			case 169: makemaz("Gan-4"); return;
			case 170: makemaz("Gan-5"); return;
			case 171: makemaz("Gee-1"); return;
			case 172: makemaz("Gee-2"); return;
			case 173: makemaz("Gee-3"); return;
			case 174: makemaz("Gee-4"); return;
			case 175: makemaz("Gee-5"); return;
			case 176: makemaz("Gla-1"); return;
			case 177: makemaz("Gla-2"); return;
			case 178: makemaz("Gla-3"); return;
			case 179: makemaz("Gla-4"); return;
			case 180: makemaz("Gla-5"); return;
			case 181: makemaz("Gof-1"); return;
			case 182: makemaz("Gof-2"); return;
			case 183: makemaz("Gof-3"); return;
			case 184: makemaz("Gof-4"); return;
			case 185: makemaz("Gof-5"); return;
			case 186: makemaz("Gra-1"); return;
			case 187: makemaz("Gra-2"); return;
			case 188: makemaz("Gra-3"); return;
			case 189: makemaz("Gra-4"); return;
			case 190: makemaz("Gra-5"); return;
			case 191: makemaz("Gun-1"); return;
			case 192: makemaz("Gun-2"); return;
			case 193: makemaz("Gun-3"); return;
			case 194: makemaz("Gun-4"); return;
			case 195: makemaz("Gun-5"); return;
			case 196: makemaz("Hea-1"); return;
			case 197: makemaz("Hea-2"); return;
			case 198: makemaz("Hea-3"); return;
			case 199: makemaz("Hea-4"); return;
			case 200: makemaz("Hea-5"); return;
			case 201: makemaz("Ice-1"); return;
			case 202: makemaz("Ice-2"); return;
			case 203: makemaz("Ice-3"); return;
			case 204: makemaz("Ice-4"); return;
			case 205: makemaz("Ice-5"); return;
			case 206: makemaz("Scr-1"); return;
			case 207: makemaz("Scr-2"); return;
			case 208: makemaz("Scr-3"); return;
			case 209: makemaz("Scr-4"); return;
			case 210: makemaz("Scr-5"); return;
			case 211: makemaz("Jed-1"); return;
			case 212: makemaz("Jed-2"); return;
			case 213: makemaz("Jed-3"); return;
			case 214: makemaz("Jed-4"); return;
			case 215: makemaz("Jed-5"); return;
			case 216: makemaz("Jes-1"); return;
			case 217: makemaz("Jes-2"); return;
			case 218: makemaz("Jes-3"); return;
			case 219: makemaz("Jes-4"); return;
			case 220: makemaz("Jes-5"); return;
			case 221: makemaz("Kni-1"); return;
			case 222: makemaz("Kni-2"); return;
			case 223: makemaz("Kni-3"); return;
			case 224: makemaz("Kni-4"); return;
			case 225: makemaz("Kni-5"); return;
			case 226: makemaz("Kor-1"); return;
			case 227: makemaz("Kor-2"); return;
			case 228: makemaz("Kor-3"); return;
			case 229: makemaz("Kor-4"); return;
			case 230: makemaz("Kor-5"); return;
			case 231: makemaz("Lad-1"); return;
			case 232: makemaz("Lad-2"); return;
			case 233: makemaz("Lad-3"); return;
			case 234: makemaz("Lad-4"); return;
			case 235: makemaz("Lad-5"); return;
			case 236: makemaz("Lib-1"); return;
			case 237: makemaz("Lib-2"); return;
			case 238: makemaz("Lib-3"); return;
			case 239: makemaz("Lib-4"); return;
			case 240: makemaz("Lib-5"); return;
			case 241: makemaz("Loc-1"); return;
			case 242: makemaz("Loc-2"); return;
			case 243: makemaz("Loc-3"); return;
			case 244: makemaz("Loc-4"); return;
			case 245: makemaz("Loc-5"); return;
			case 246: makemaz("Lun-1"); return;
			case 247: makemaz("Lun-2"); return;
			case 248: makemaz("Lun-3"); return;
			case 249: makemaz("Lun-4"); return;
			case 250: makemaz("Lun-5"); return;
			case 251: makemaz("Mah-1"); return;
			case 252: makemaz("Mah-2"); return;
			case 253: makemaz("Mah-3"); return;
			case 254: makemaz("Mah-4"); return;
			case 255: makemaz("Mah-5"); return;
			case 256: makemaz("Mon-1"); return;
			case 257: makemaz("Mon-2"); return;
			case 258: makemaz("Mon-3"); return;
			case 259: makemaz("Mon-4"); return;
			case 260: makemaz("Mon-5"); return;
			case 261: makemaz("Mus-1"); return;
			case 262: makemaz("Mus-2"); return;
			case 263: makemaz("Mus-3"); return;
			case 264: makemaz("Mus-4"); return;
			case 265: makemaz("Mus-5"); return;
			case 266: makemaz("Mys-1"); return;
			case 267: makemaz("Mys-2"); return;
			case 268: makemaz("Mys-3"); return;
			case 269: makemaz("Mys-4"); return;
			case 270: makemaz("Mys-5"); return;
			case 271: makemaz("Nec-1"); return;
			case 272: makemaz("Nec-2"); return;
			case 273: makemaz("Nec-3"); return;
			case 274: makemaz("Nec-4"); return;
			case 275: makemaz("Nec-5"); return;
			case 276: makemaz("Nin-1"); return;
			case 277: makemaz("Nin-2"); return;
			case 278: makemaz("Nin-3"); return;
			case 279: makemaz("Nin-4"); return;
			case 280: makemaz("Nin-5"); return;
			case 281: makemaz("Nob-1"); return;
			case 282: makemaz("Nob-2"); return;
			case 283: makemaz("Nob-3"); return;
			case 284: makemaz("Nob-4"); return;
			case 285: makemaz("Nob-5"); return;
			case 286: makemaz("Off-1"); return;
			case 287: makemaz("Off-2"); return;
			case 288: makemaz("Off-3"); return;
			case 289: makemaz("Off-4"); return;
			case 290: makemaz("Off-5"); return;
			case 291: makemaz("Ord-1"); return;
			case 292: makemaz("Ord-2"); return;
			case 293: makemaz("Ord-3"); return;
			case 294: makemaz("Ord-4"); return;
			case 295: makemaz("Ord-5"); return;
			case 296: makemaz("Ota-1"); return;
			case 297: makemaz("Ota-2"); return;
			case 298: makemaz("Ota-3"); return;
			case 299: makemaz("Ota-4"); return;
			case 300: makemaz("Ota-5"); return;
			case 301: makemaz("Pal-1"); return;
			case 302: makemaz("Pal-2"); return;
			case 303: makemaz("Pal-3"); return;
			case 304: makemaz("Pal-4"); return;
			case 305: makemaz("Pal-5"); return;
			case 306: makemaz("Pic-1"); return;
			case 307: makemaz("Pic-2"); return;
			case 308: makemaz("Pic-3"); return;
			case 309: makemaz("Pic-4"); return;
			case 310: makemaz("Pic-5"); return;
			case 311: makemaz("Pir-1"); return;
			case 312: makemaz("Pir-2"); return;
			case 313: makemaz("Pir-3"); return;
			case 314: makemaz("Pir-4"); return;
			case 315: makemaz("Pir-5"); return;
			case 316: makemaz("Pok-1"); return;
			case 317: makemaz("Pok-2"); return;
			case 318: makemaz("Pok-3"); return;
			case 319: makemaz("Pok-4"); return;
			case 320: makemaz("Pok-5"); return;
			case 321: makemaz("Pol-1"); return;
			case 322: makemaz("Pol-2"); return;
			case 323: makemaz("Pol-3"); return;
			case 324: makemaz("Pol-4"); return;
			case 325: makemaz("Pol-5"); return;
			case 326: makemaz("Pri-1"); return;
			case 327: makemaz("Pri-2"); return;
			case 328: makemaz("Pri-3"); return;
			case 329: makemaz("Pri-4"); return;
			case 330: makemaz("Pri-5"); return;
			case 331: makemaz("Psi-1"); return;
			case 332: makemaz("Psi-2"); return;
			case 333: makemaz("Psi-3"); return;
			case 334: makemaz("Psi-4"); return;
			case 335: makemaz("Psi-5"); return;
			case 336: makemaz("Ran-1"); return;
			case 337: makemaz("Ran-2"); return;
			case 338: makemaz("Ran-3"); return;
			case 339: makemaz("Ran-4"); return;
			case 340: makemaz("Ran-5"); return;
			case 341: makemaz("Roc-1"); return;
			case 342: makemaz("Roc-2"); return;
			case 343: makemaz("Roc-3"); return;
			case 344: makemaz("Roc-4"); return;
			case 345: makemaz("Roc-5"); return;
			case 346: makemaz("Rog-1"); return;
			case 347: makemaz("Rog-2"); return;
			case 348: makemaz("Rog-3"); return;
			case 349: makemaz("Rog-4"); return;
			case 350: makemaz("Rog-5"); return;
			case 351: makemaz("Sag-1"); return;
			case 352: makemaz("Sag-2"); return;
			case 353: makemaz("Sag-3"); return;
			case 354: makemaz("Sag-4"); return;
			case 355: makemaz("Sag-5"); return;
			case 356: makemaz("Sai-1"); return;
			case 357: makemaz("Sai-2"); return;
			case 358: makemaz("Sai-3"); return;
			case 359: makemaz("Sai-4"); return;
			case 360: makemaz("Sai-5"); return;
			case 361: makemaz("Sam-1"); return;
			case 362: makemaz("Sam-2"); return;
			case 363: makemaz("Sam-3"); return;
			case 364: makemaz("Sam-4"); return;
			case 365: makemaz("Sam-5"); return;
			case 366: makemaz("Sci-1"); return;
			case 367: makemaz("Sci-2"); return;
			case 368: makemaz("Sci-3"); return;
			case 369: makemaz("Sci-4"); return;
			case 370: makemaz("Sci-5"); return;
			case 371: makemaz("Sla-1"); return;
			case 372: makemaz("Sla-2"); return;
			case 373: makemaz("Sla-3"); return;
			case 374: makemaz("Sla-4"); return;
			case 375: makemaz("Sla-5"); return;
			case 376: makemaz("Spa-1"); return;
			case 377: makemaz("Spa-2"); return;
			case 378: makemaz("Spa-3"); return;
			case 379: makemaz("Spa-4"); return;
			case 380: makemaz("Spa-5"); return;
			case 381: makemaz("Sup-1"); return;
			case 382: makemaz("Sup-2"); return;
			case 383: makemaz("Sup-3"); return;
			case 384: makemaz("Sup-4"); return;
			case 385: makemaz("Sup-5"); return;
			case 386: makemaz("Tha-1"); return;
			case 387: makemaz("Tha-2"); return;
			case 388: makemaz("Tha-3"); return;
			case 389: makemaz("Tha-4"); return;
			case 390: makemaz("Tha-5"); return;
			case 391: makemaz("Top-1"); return;
			case 392: makemaz("Top-2"); return;
			case 393: makemaz("Top-3"); return;
			case 394: makemaz("Top-4"); return;
			case 395: makemaz("Top-5"); return;
			case 396: makemaz("Tou-1"); return;
			case 397: makemaz("Tou-2"); return;
			case 398: makemaz("Tou-3"); return;
			case 399: makemaz("Tou-4"); return;
			case 400: makemaz("Tou-5"); return;
			case 401: makemaz("Tra-1"); return;
			case 402: makemaz("Tra-2"); return;
			case 403: makemaz("Tra-3"); return;
			case 404: makemaz("Tra-4"); return;
			case 405: makemaz("Tra-5"); return;
			case 406: makemaz("Und-1"); return;
			case 407: makemaz("Und-2"); return;
			case 408: makemaz("Und-3"); return;
			case 409: makemaz("Und-4"); return;
			case 410: makemaz("Und-5"); return;
			case 411: makemaz("Unt-1"); return;
			case 412: makemaz("Unt-2"); return;
			case 413: makemaz("Unt-3"); return;
			case 414: makemaz("Unt-4"); return;
			case 415: makemaz("Unt-5"); return;
			case 416: makemaz("Val-1"); return;
			case 417: makemaz("Val-2"); return;
			case 418: makemaz("Val-3"); return;
			case 419: makemaz("Val-4"); return;
			case 420: makemaz("Val-5"); return;
			case 421: makemaz("Wan-1"); return;
			case 422: makemaz("Wan-2"); return;
			case 423: makemaz("Wan-3"); return;
			case 424: makemaz("Wan-4"); return;
			case 425: makemaz("Wan-5"); return;
			case 426: makemaz("War-1"); return;
			case 427: makemaz("War-2"); return;
			case 428: makemaz("War-3"); return;
			case 429: makemaz("War-4"); return;
			case 430: makemaz("War-5"); return;
			case 431: makemaz("Wiz-1"); return;
			case 432: makemaz("Wiz-2"); return;
			case 433: makemaz("Wiz-3"); return;
			case 434: makemaz("Wiz-4"); return;
			case 435: makemaz("Wiz-5"); return;
			case 436: makemaz("Yeo-1"); return;
			case 437: makemaz("Yeo-2"); return;
			case 438: makemaz("Yeo-3"); return;
			case 439: makemaz("Yeo-4"); return;
			case 440: makemaz("Yeo-5"); return;
			case 441: makemaz("Zoo-1"); return;
			case 442: makemaz("Zoo-2"); return;
			case 443: makemaz("Zoo-3"); return;
			case 444: makemaz("Zoo-4"); return;
			case 445: makemaz("Zoo-5"); return;
			case 446: makemaz("Zyb-1"); return;
			case 447: makemaz("Zyb-2"); return;
			case 448: makemaz("Zyb-3"); return;
			case 449: makemaz("Zyb-4"); return;
			case 450: makemaz("Zyb-5"); return;
			case 451: makemaz("Ana-1"); return;
			case 452: makemaz("Ana-2"); return;
			case 453: makemaz("Ana-3"); return;
			case 454: makemaz("Ana-4"); return;
			case 455: makemaz("Ana-5"); return;
			case 456: makemaz("Cam-1"); return;
			case 457: makemaz("Cam-2"); return;
			case 458: makemaz("Cam-3"); return;
			case 459: makemaz("Cam-4"); return;
			case 460: makemaz("Cam-5"); return;
			case 461: makemaz("Mar-1"); return;
			case 462: makemaz("Mar-2"); return;
			case 463: makemaz("Mar-3"); return;
			case 464: makemaz("Mar-4"); return;
			case 465: makemaz("Mar-5"); return;
			case 466: makemaz("Sli-1"); return;
			case 467: makemaz("Sli-2"); return;
			case 468: makemaz("Sli-3"); return;
			case 469: makemaz("Sli-4"); return;
			case 470: makemaz("Sli-5"); return;
			case 471: makemaz("Drd-1"); return;
			case 472: makemaz("Drd-2"); return;
			case 473: makemaz("Drd-3"); return;
			case 474: makemaz("Drd-4"); return;
			case 475: makemaz("Drd-5"); return;
			case 476: makemaz("Erd-1"); return;
			case 477: makemaz("Erd-2"); return;
			case 478: makemaz("Erd-3"); return;
			case 479: makemaz("Erd-4"); return;
			case 480: makemaz("Erd-5"); return;
			case 481: makemaz("Fai-1"); return;
			case 482: makemaz("Fai-2"); return;
			case 483: makemaz("Fai-3"); return;
			case 484: makemaz("Fai-4"); return;
			case 485: makemaz("Fai-5"); return;
			case 486: makemaz("Fen-1"); return;
			case 487: makemaz("Fen-2"); return;
			case 488: makemaz("Fen-3"); return;
			case 489: makemaz("Fen-4"); return;
			case 490: makemaz("Fen-5"); return;
			case 491: makemaz("Fig-1"); return;
			case 492: makemaz("Fig-2"); return;
			case 493: makemaz("Fig-3"); return;
			case 494: makemaz("Fig-4"); return;
			case 495: makemaz("Fig-5"); return;
			case 496: makemaz("Gol-1"); return;
			case 497: makemaz("Gol-2"); return;
			case 498: makemaz("Gol-3"); return;
			case 499: makemaz("Gol-4"); return;
			case 500: makemaz("Gol-5"); return;
			case 501: makemaz("Jus-1"); return;
			case 502: makemaz("Jus-2"); return;
			case 503: makemaz("Jus-3"); return;
			case 504: makemaz("Jus-4"); return;
			case 505: makemaz("Jus-5"); return;
			case 506: makemaz("Med-1"); return;
			case 507: makemaz("Med-2"); return;
			case 508: makemaz("Med-3"); return;
			case 509: makemaz("Med-4"); return;
			case 510: makemaz("Med-5"); return;
			case 511: makemaz("Mid-1"); return;
			case 512: makemaz("Mid-2"); return;
			case 513: makemaz("Mid-3"); return;
			case 514: makemaz("Mid-4"); return;
			case 515: makemaz("Mid-5"); return;
			case 516: makemaz("Mur-1"); return;
			case 517: makemaz("Mur-2"); return;
			case 518: makemaz("Mur-3"); return;
			case 519: makemaz("Mur-4"); return;
			case 520: makemaz("Mur-5"); return;
			case 521: makemaz("Poi-1"); return;
			case 522: makemaz("Poi-2"); return;
			case 523: makemaz("Poi-3"); return;
			case 524: makemaz("Poi-4"); return;
			case 525: makemaz("Poi-5"); return;
			case 526: makemaz("Rin-1"); return;
			case 527: makemaz("Rin-2"); return;
			case 528: makemaz("Rin-3"); return;
			case 529: makemaz("Rin-4"); return;
			case 530: makemaz("Rin-5"); return;
			case 531: makemaz("Sha-1"); return;
			case 532: makemaz("Sha-2"); return;
			case 533: makemaz("Sha-3"); return;
			case 534: makemaz("Sha-4"); return;
			case 535: makemaz("Sha-5"); return;
			case 536: makemaz("Twe-1"); return;
			case 537: makemaz("Twe-2"); return;
			case 538: makemaz("Twe-3"); return;
			case 539: makemaz("Twe-4"); return;
			case 540: makemaz("Twe-5"); return;
			case 541: makemaz("Use-1"); return;
			case 542: makemaz("Use-2"); return;
			case 543: makemaz("Use-3"); return;
			case 544: makemaz("Use-4"); return;
			case 545: makemaz("Use-5"); return;
			case 546: makemaz("Sex-1"); return;
			case 547: makemaz("Sex-2"); return;
			case 548: makemaz("Sex-3"); return;
			case 549: makemaz("Sex-4"); return;
			case 550: makemaz("Sex-5"); return;
			case 551: makemaz("Unb-1"); return;
			case 552: makemaz("Unb-2"); return;
			case 553: makemaz("Unb-3"); return;
			case 554: makemaz("Unb-4"); return;
			case 555: makemaz("Unb-5"); return;
			case 556: makemaz("Trs-1"); return;
			case 557: makemaz("Trs-2"); return;
			case 558: makemaz("Trs-3"); return;
			case 559: makemaz("Trs-4"); return;
			case 560: makemaz("Trs-5"); return;
			case 561: makemaz("Cha-1"); return;
			case 562: makemaz("Cha-2"); return;
			case 563: makemaz("Cha-3"); return;
			case 564: makemaz("Cha-4"); return;
			case 565: makemaz("Cha-5"); return;
			case 566: makemaz("Elm-1"); return;
			case 567: makemaz("Elm-2"); return;
			case 568: makemaz("Elm-3"); return;
			case 569: makemaz("Elm-4"); return;
			case 570: makemaz("Elm-5"); return;
			case 571: makemaz("Wil-1"); return;
			case 572: makemaz("Wil-2"); return;
			case 573: makemaz("Wil-3"); return;
			case 574: makemaz("Wil-4"); return;
			case 575: makemaz("Wil-5"); return;
			case 576: makemaz("Occ-1"); return;
			case 577: makemaz("Occ-2"); return;
			case 578: makemaz("Occ-3"); return;
			case 579: makemaz("Occ-4"); return;
			case 580: makemaz("Occ-5"); return;
			case 581: makemaz("Pro-1"); return;
			case 582: makemaz("Pro-2"); return;
			case 583: makemaz("Pro-3"); return;
			case 584: makemaz("Pro-4"); return;
			case 585: makemaz("Pro-5"); return;
			case 586: makemaz("Kur-1"); return;
			case 587: makemaz("Kur-2"); return;
			case 588: makemaz("Kur-3"); return;
			case 589: makemaz("Kur-4"); return;
			case 590: makemaz("Kur-5"); return;
			case 591: makemaz("For-1"); return;
			case 592: makemaz("For-2"); return;
			case 593: makemaz("For-3"); return;
			case 594: makemaz("For-4"); return;
			case 595: makemaz("For-5"); return;
			case 596: makemaz("Trc-1"); return;
			case 597: makemaz("Trc-2"); return;
			case 598: makemaz("Trc-3"); return;
			case 599: makemaz("Trc-4"); return;
			case 600: makemaz("Trc-5"); return;
			case 601: makemaz("Nuc-1"); return;
			case 602: makemaz("Nuc-2"); return;
			case 603: makemaz("Nuc-3"); return;
			case 604: makemaz("Nuc-4"); return;
			case 605: makemaz("Nuc-5"); return;
			case 606: makemaz("Sco-1"); return;
			case 607: makemaz("Sco-2"); return;
			case 608: makemaz("Sco-3"); return;
			case 609: makemaz("Sco-4"); return;
			case 610: makemaz("Sco-5"); return;
			case 611: makemaz("Fem-1"); return;
			case 612: makemaz("Fem-2"); return;
			case 613: makemaz("Fem-3"); return;
			case 614: makemaz("Fem-4"); return;
			case 615: makemaz("Fem-5"); return;
			case 616: makemaz("Hus-1"); return;
			case 617: makemaz("Hus-2"); return;
			case 618: makemaz("Hus-3"); return;
			case 619: makemaz("Hus-4"); return;
			case 620: makemaz("Hus-5"); return;
			case 621: makemaz("Acu-1"); return;
			case 622: makemaz("Acu-2"); return;
			case 623: makemaz("Acu-3"); return;
			case 624: makemaz("Acu-4"); return;
			case 625: makemaz("Acu-5"); return;
			case 626: makemaz("Mas-1"); return;
			case 627: makemaz("Mas-2"); return;
			case 628: makemaz("Mas-3"); return;
			case 629: makemaz("Mas-4"); return;
			case 630: makemaz("Mas-5"); return;
			case 631: makemaz("Gre-1"); return;
			case 632: makemaz("Gre-2"); return;
			case 633: makemaz("Gre-3"); return;
			case 634: makemaz("Gre-4"); return;
			case 635: makemaz("Gre-5"); return;
			case 636: makemaz("Cel-1"); return;
			case 637: makemaz("Cel-2"); return;
			case 638: makemaz("Cel-3"); return;
			case 639: makemaz("Cel-4"); return;
			case 640: makemaz("Cel-5"); return;
			case 641: makemaz("Wal-1"); return;
			case 642: makemaz("Wal-2"); return;
			case 643: makemaz("Wal-3"); return;
			case 644: makemaz("Wal-4"); return;
			case 645: makemaz("Wal-5"); return;
			case 646: makemaz("Soc-1"); return;
			case 647: makemaz("Soc-2"); return;
			case 648: makemaz("Soc-3"); return;
			case 649: makemaz("Soc-4"); return;
			case 650: makemaz("Soc-5"); return;
			case 651: makemaz("Dem-1"); return;
			case 652: makemaz("Dem-2"); return;
			case 653: makemaz("Dem-3"); return;
			case 654: makemaz("Dem-4"); return;
			case 655: makemaz("Dem-5"); return;
			case 656: makemaz("Dis-1"); return;
			case 657: makemaz("Dis-2"); return;
			case 658: makemaz("Dis-3"); return;
			case 659: makemaz("Dis-4"); return;
			case 660: makemaz("Dis-5"); return;
			case 661: makemaz("Sto-1"); return;
			case 662: makemaz("Sto-2"); return;
			case 663: makemaz("Sto-3"); return;
			case 664: makemaz("Sto-4"); return;
			case 665: makemaz("Sto-5"); return;
			case 666: makemaz("Mam-1"); return;
			case 667: makemaz("Mam-2"); return;
			case 668: makemaz("Mam-3"); return;
			case 669: makemaz("Mam-4"); return;
			case 670: makemaz("Mam-5"); return;
			case 671: makemaz("Jan-1"); return;
			case 672: makemaz("Jan-2"); return;
			case 673: makemaz("Jan-3"); return;
			case 674: makemaz("Jan-4"); return;
			case 675: makemaz("Jan-5"); return;
			case 676: makemaz("Emp-1"); return;
			case 677: makemaz("Emp-2"); return;
			case 678: makemaz("Emp-3"); return;
			case 679: makemaz("Emp-4"); return;
			case 680: makemaz("Emp-5"); return;
			case 681: makemaz("Psy-1"); return;
			case 682: makemaz("Psy-2"); return;
			case 683: makemaz("Psy-3"); return;
			case 684: makemaz("Psy-4"); return;
			case 685: makemaz("Psy-5"); return;
			case 686: makemaz("Qua-1"); return;
			case 687: makemaz("Qua-2"); return;
			case 688: makemaz("Qua-3"); return;
			case 689: makemaz("Qua-4"); return;
			case 690: makemaz("Qua-5"); return;
			case 691: makemaz("Cra-1"); return;
			case 692: makemaz("Cra-2"); return;
			case 693: makemaz("Cra-3"); return;
			case 694: makemaz("Cra-4"); return;
			case 695: makemaz("Cra-5"); return;
			case 696: makemaz("Wei-1"); return;
			case 697: makemaz("Wei-2"); return;
			case 698: makemaz("Wei-3"); return;
			case 699: makemaz("Wei-4"); return;
			case 700: makemaz("Wei-5"); return;
			case 701: makemaz("Xel-1"); return;
			case 702: makemaz("Xel-2"); return;
			case 703: makemaz("Xel-3"); return;
			case 704: makemaz("Xel-4"); return;
			case 705: makemaz("Xel-5"); return;
			case 706: makemaz("Yau-1"); return;
			case 707: makemaz("Yau-2"); return;
			case 708: makemaz("Yau-3"); return;
			case 709: makemaz("Yau-4"); return;
			case 710: makemaz("Yau-5"); return;
			case 711: makemaz("Sof-1"); return;
			case 712: makemaz("Sof-2"); return;
			case 713: makemaz("Sof-3"); return;
			case 714: makemaz("Sof-4"); return;
			case 715: makemaz("Sof-5"); return;
			case 716: makemaz("Ast-1"); return;
			case 717: makemaz("Ast-2"); return;
			case 718: makemaz("Ast-3"); return;
			case 719: makemaz("Ast-4"); return;
			case 720: makemaz("Ast-5"); return;
			case 721: makemaz("Sma-1"); return;
			case 722: makemaz("Sma-2"); return;
			case 723: makemaz("Sma-3"); return;
			case 724: makemaz("Sma-4"); return;
			case 725: makemaz("Sma-5"); return;
			case 726: makemaz("Cyb-1"); return;
			case 727: makemaz("Cyb-2"); return;
			case 728: makemaz("Cyb-3"); return;
			case 729: makemaz("Cyb-4"); return;
			case 730: makemaz("Cyb-5"); return;
			case 731: makemaz("Tos-1"); return;
			case 732: makemaz("Tos-2"); return;
			case 733: makemaz("Tos-3"); return;
			case 734: makemaz("Tos-4"); return;
			case 735: makemaz("Tos-5"); return;
			case 736: makemaz("Sym-1"); return;
			case 737: makemaz("Sym-2"); return;
			case 738: makemaz("Sym-3"); return;
			case 739: makemaz("Sym-4"); return;
			case 740: makemaz("Sym-5"); return;
			case 741: makemaz("Pra-1"); return;
			case 742: makemaz("Pra-2"); return;
			case 743: makemaz("Pra-3"); return;
			case 744: makemaz("Pra-4"); return;
			case 745: makemaz("Pra-5"); return;
			case 746: makemaz("Mil-1"); return;
			case 747: makemaz("Mil-2"); return;
			case 748: makemaz("Mil-3"); return;
			case 749: makemaz("Mil-4"); return;
			case 750: makemaz("Mil-5"); return;
			case 751: makemaz("Gen-1"); return;
			case 752: makemaz("Gen-2"); return;
			case 753: makemaz("Gen-3"); return;
			case 754: makemaz("Gen-4"); return;
			case 755: makemaz("Gen-5"); return;
			case 756: makemaz("Fjo-1"); return;
			case 757: makemaz("Fjo-2"); return;
			case 758: makemaz("Fjo-3"); return;
			case 759: makemaz("Fjo-4"); return;
			case 760: makemaz("Fjo-5"); return;
			case 761: makemaz("Eme-1"); return;
			case 762: makemaz("Eme-2"); return;
			case 763: makemaz("Eme-3"); return;
			case 764: makemaz("Eme-4"); return;
			case 765: makemaz("Eme-5"); return;
			case 766: makemaz("Com-1"); return;
			case 767: makemaz("Com-2"); return;
			case 768: makemaz("Com-3"); return;
			case 769: makemaz("Com-4"); return;
			case 770: makemaz("Com-5"); return;
			case 771: makemaz("Akl-1"); return;
			case 772: makemaz("Akl-2"); return;
			case 773: makemaz("Akl-3"); return;
			case 774: makemaz("Akl-4"); return;
			case 775: makemaz("Akl-5"); return;
			case 776: makemaz("Dra-1"); return;
			case 777: makemaz("Dra-2"); return;
			case 778: makemaz("Dra-3"); return;
			case 779: makemaz("Dra-4"); return;
			case 780: makemaz("Dra-5"); return;
			case 781: makemaz("Car-1"); return;
			case 782: makemaz("Car-2"); return;
			case 783: makemaz("Car-3"); return;
			case 784: makemaz("Car-4"); return;
			case 785: makemaz("Car-5"); return;
			case 786: makemaz("But-1"); return;
			case 787: makemaz("But-2"); return;
			case 788: makemaz("But-3"); return;
			case 789: makemaz("But-4"); return;
			case 790: makemaz("But-5"); return;
			case 791: makemaz("Dan-1"); return;
			case 792: makemaz("Dan-2"); return;
			case 793: makemaz("Dan-3"); return;
			case 794: makemaz("Dan-4"); return;
			case 795: makemaz("Dan-5"); return;
			case 796: makemaz("Dia-1"); return;
			case 797: makemaz("Dia-2"); return;
			case 798: makemaz("Dia-3"); return;
			case 799: makemaz("Dia-4"); return;
			case 800: makemaz("Dia-5"); return;
			case 801: makemaz("Pre-1"); return;
			case 802: makemaz("Pre-2"); return;
			case 803: makemaz("Pre-3"); return;
			case 804: makemaz("Pre-4"); return;
			case 805: makemaz("Pre-5"); return;
			case 806: makemaz("Sec-1"); return;
			case 807: makemaz("Sec-2"); return;
			case 808: makemaz("Sec-3"); return;
			case 809: makemaz("Sec-4"); return;
			case 810: makemaz("Sec-5"); return;
			case 811: makemaz("Sho-1"); return;
			case 812: makemaz("Sho-2"); return;
			case 813: makemaz("Sho-3"); return;
			case 814: makemaz("Sho-4"); return;
			case 815: makemaz("Sho-5"); return;
			case 816: makemaz("Hal-1"); return;
			case 817: makemaz("Hal-2"); return;
			case 818: makemaz("Hal-3"); return;
			case 819: makemaz("Hal-4"); return;
			case 820: makemaz("Hal-5"); return;
			case 821: makemaz("Sin-1"); return;
			case 822: makemaz("Sin-2"); return;
			case 823: makemaz("Sin-3"); return;
			case 824: makemaz("Sin-4"); return;
			case 825: makemaz("Sin-5"); return;
			case 826: makemaz("Hed-1"); return;
			case 827: makemaz("Hed-2"); return;
			case 828: makemaz("Hed-3"); return;
			case 829: makemaz("Hed-4"); return;
			case 830: makemaz("Hed-5"); return;
			case 831: makemaz("Van-1"); return;
			case 832: makemaz("Van-2"); return;
			case 833: makemaz("Van-3"); return;
			case 834: makemaz("Van-4"); return;
			case 835: makemaz("Van-5"); return;
			case 836: makemaz("Sdw-1"); return;
			case 837: makemaz("Sdw-2"); return;
			case 838: makemaz("Sdw-3"); return;
			case 839: makemaz("Sdw-4"); return;
			case 840: makemaz("Sdw-5"); return;
			case 841: makemaz("Cli-1"); return;
			case 842: makemaz("Cli-2"); return;
			case 843: makemaz("Cli-3"); return;
			case 844: makemaz("Cli-4"); return;
			case 845: makemaz("Cli-5"); return;
			case 846: makemaz("Wom-1"); return;
			case 847: makemaz("Wom-2"); return;
			case 848: makemaz("Wom-3"); return;
			case 849: makemaz("Wom-4"); return;
			case 850: makemaz("Wom-5"); return;
			case 851: makemaz("Noo-1"); return;
			case 852: makemaz("Noo-2"); return;
			case 853: makemaz("Noo-3"); return;
			case 854: makemaz("Noo-4"); return;
			case 855: makemaz("Noo-5"); return;

		}
		break;

		case 94:
		case 95:
		case 96:
		case 97:
		case 98:
		case 99:
		case 100:

		switch (rnd(69)) {

			case 1: makemaz("mazes-1"); return;
			case 2: makemaz("mazes-2"); return;
			case 3: makemaz("mazes-3"); return;
			case 4: makemaz("mazes-4"); return;
			case 5: makemaz("mazes-5"); return;
			case 6: makemaz("mazes-6"); return;
			case 7: makemaz("mazes-7"); return;
			case 8: makemaz("mazes-8"); return;
			case 9: makemaz("mazes-9"); return;
			case 10: makemaz("mazes-10"); return;
			case 11: makemaz("mazes-11"); return;
			case 12: makemaz("mazes-12"); return;
			case 13: makemaz("mazes-13"); return;
			case 14: makemaz("mazes-14"); return;
			case 15: makemaz("mazes-15"); return;
			case 16: makemaz("mazes-16"); return;
			case 17: makemaz("mazes-17"); return;
			case 18: makemaz("mazes-18"); return;
			case 19: makemaz("mazes-19"); return;
			case 20: makemaz("mazes-20"); return;
			case 21: makemaz("mazes-21"); return;
			case 22: makemaz("mazes-22"); return;
			case 23: makemaz("mazes-23"); return;
			case 24: makemaz("mazes-24"); return;
			case 25: makemaz("mazes-25"); return;
			case 26: makemaz("mazes-26"); return;
			case 27: makemaz("mazes-27"); return;
			case 28: makemaz("mazes-28"); return;
			case 29: makemaz("mazes-29"); return;
			case 30: makemaz("mazes-30"); return;
			case 31: makemaz("mazes-31"); return;
			case 32: makemaz("mazes-32"); return;
			case 33: makemaz("mazes-33"); return;
			case 34: makemaz("mazes-34"); return;
			case 35: makemaz("mazes-35"); return;
			case 36: makemaz("mazes-36"); return;
			case 37: makemaz("mazes-37"); return;
			case 38: makemaz("mazes-38"); return;
			case 39: makemaz("mazes-39"); return;
			case 40: makemaz("mazes-40"); return;
			case 41: makemaz("mazes-41"); return;
			case 42: makemaz("mazes-42"); return;
			case 43: makemaz("mazes-43"); return;
			case 44: makemaz("mazes-44"); return;
			case 45: makemaz("mazes-45"); return;
			case 46: makemaz("mazes-46"); return;
			case 47: makemaz("mazes-47"); return;
			case 48: makemaz("mazes-48"); return;
			case 49: makemaz("mazes-49"); return;
			case 50: makemaz("mazes-50"); return;
			case 51: makemaz("mazes-51"); return;
			case 52: makemaz("mazes-52"); return;
			case 53: makemaz("mazes-53"); return;
			case 54: makemaz("mazes-54"); return;
			case 55: makemaz("mazes-55"); return;
			case 56: makemaz("mazes-56"); return;
			case 57: makemaz("mazes-57"); return;
			case 58: makemaz("mazes-58"); return;
			case 59: makemaz("mazes-59"); return;
			case 60: makemaz("mazes-60"); return;
			case 61: makemaz("mazes-61"); return;
			case 62: makemaz("mazes-62"); return;
			case 63: makemaz("mazes-63"); return;
			case 64: makemaz("mazes-64"); return;
			case 65: makemaz("mazes-65"); return;
			case 66: makemaz("mazes-66"); return;
			case 67: makemaz("mazes-67"); return;
			case 68: makemaz("mazes-68"); return;
			case 69: makemaz("mazes-69"); return;

		}
		break;

		case 101:
		case 102:
		case 103:
		case 104:
		case 105:
		switch (rnd(95)) {

			case 1: makemaz("levgn-1"); return;
			case 2: makemaz("levgn-2"); return;
			case 3: makemaz("levgn-3"); return;
			case 4: makemaz("levgn-4"); return;
			case 5: makemaz("levgn-5"); return;
			case 6: makemaz("levgn-6"); return;
			case 7: makemaz("levgn-7"); return;
			case 8: makemaz("levgn-8"); return;
			case 9: makemaz("levgn-9"); return;
			case 10: makemaz("levgn-10"); return;
			case 11: makemaz("levgn-11"); return;
			case 12: makemaz("levgn-12"); return;
			case 13: makemaz("levgn-13"); return;
			case 14: makemaz("levgn-14"); return;
			case 15: makemaz("levgn-15"); return;
			case 16: makemaz("levgn-16"); return;
			case 17: makemaz("levgn-17"); return;
			case 18: makemaz("levgn-18"); return;
			case 19: makemaz("levgn-19"); return;
			case 20: makemaz("levgn-20"); return;
			case 21: makemaz("levgn-21"); return;
			case 22: makemaz("levgn-22"); return;
			case 23: makemaz("levgn-23"); return;
			case 24: makemaz("levgn-24"); return;
			case 25: makemaz("levgn-25"); return;
			case 26: makemaz("levgn-26"); return;
			case 27: makemaz("levgn-27"); return;
			case 28: makemaz("levgn-28"); return;
			case 29: makemaz("levgn-29"); return;
			case 30: makemaz("levgn-30"); return;
			case 31: makemaz("levgn-31"); return;
			case 32: makemaz("levgn-32"); return;
			case 33: makemaz("levgn-33"); return;
			case 34: makemaz("levgn-34"); return;
			case 35: makemaz("levgn-35"); return;
			case 36: makemaz("levgn-36"); return;
			case 37: makemaz("levgn-37"); return;
			case 38: makemaz("levgn-38"); return;
			case 39: makemaz("levgn-39"); return;
			case 40: makemaz("levgn-40"); return;
			case 41: makemaz("levgn-41"); return;
			case 42: makemaz("levgn-42"); return;
			case 43: makemaz("levgn-43"); return;
			case 44: makemaz("levgn-44"); return;
			case 45: makemaz("levgn-45"); return;
			case 46: makemaz("levgn-46"); return;
			case 47: makemaz("levgn-47"); return;
			case 48: makemaz("levgn-48"); return;
			case 49: makemaz("levgn-49"); return;
			case 50: makemaz("levgn-50"); return;
			case 51: makemaz("levgn-51"); return;
			case 52: makemaz("levgn-52"); return;
			case 53: makemaz("levgn-53"); return;
			case 54: makemaz("levgn-54"); return;
			case 55: makemaz("levgn-55"); return;
			case 56: makemaz("levgn-56"); return;
			case 57: makemaz("levgn-57"); return;
			case 58: makemaz("levgn-58"); return;
			case 59: makemaz("levgn-59"); return;
			case 60: makemaz("levgn-60"); return;
			case 61: makemaz("levgn-61"); return;
			case 62: makemaz("levgn-62"); return;
			case 63: makemaz("levgn-63"); return;
			case 64: makemaz("levgn-64"); return;
			case 65: makemaz("levgn-65"); return;
			case 66: makemaz("levgn-66"); return;
			case 67: makemaz("levgn-67"); return;
			case 68: makemaz("levgn-68"); return;
			case 69: makemaz("levgn-69"); return;
			case 70: makemaz("levgn-70"); return;
			case 71: makemaz("levgn-71"); return;
			case 72: makemaz("levgn-72"); return;
			case 73: makemaz("levgn-73"); return;
			case 74: makemaz("levgn-74"); return;
			case 75: makemaz("levgn-75"); return;
			case 76: makemaz("levgn-76"); return;
			case 77: makemaz("levgn-77"); return;
			case 78: makemaz("levgn-78"); return;
			case 79: makemaz("levgn-79"); return;
			case 80: makemaz("levgn-80"); return;
			case 81: makemaz("levgn-81"); return;
			case 82: makemaz("levgn-82"); return;
			case 83: makemaz("levgn-83"); return;
			case 84: makemaz("levgn-84"); return;
			case 85: makemaz("levgn-85"); return;
			case 86: makemaz("levgn-86"); return;
			case 87: makemaz("levgn-87"); return;
			case 88: makemaz("levgn-88"); return;
			case 89: makemaz("levgn-89"); return;
			case 90: makemaz("levgn-90"); return;
			case 91: makemaz("levgn-91"); return;
			case 92: makemaz("levgn-92"); return;
			case 93: makemaz("levgn-93"); return;
			case 94: makemaz("levgn-94"); return;
			case 95: makemaz("levgn-95"); return;

		}

	    case 106:

		makemaz("ecav2"); return;
		break;

	    case 107:

		makemaz("efrnk"); return;
		break;

	    case 108:
	    case 109:
	    case 110:

		switch (rnd(8)) {

			case 1: {
				switch (rnd(5)) {

					case 1: makemaz("cowla-1"); return;
					case 2: makemaz("cowla-2"); return;
					case 3: makemaz("cowla-3"); return;
					case 4: makemaz("cowla-4"); return;
					case 5: makemaz("cowla-5"); return;

				}
				break;
			}
			case 2: makemaz("cowlb"); return;
			case 3: makemaz("cowlc"); return;
			case 4: makemaz("cowld"); return;
			case 5: makemaz("cowle"); return;
			case 6: makemaz("cowlf"); return;
			case 7: makemaz("cowlg"); return;
			case 8: makemaz("cowlh"); return;

		}

		break;

	    case 111:
		switch (rnd(15)) {
			case 1: makemaz("deepfila"); return;
			case 2: makemaz("deepfilb"); return;
			case 3: makemaz("deepfilc"); return;
			case 4: makemaz("deepfild"); return;
			case 5: makemaz("deepfile"); return;
			case 6: makemaz("deepfilf"); return;
			case 7: makemaz("deepfilg"); return;
			case 8: makemaz("deepfilh"); return;
			case 9: makemaz("deepfili"); return;
			case 10: makemaz("deepfilj"); return;
			case 11: makemaz("deepfilk"); return;
			case 12: makemaz("deepfill"); return;
			case 13: makemaz("deepfilm"); return;
			case 14: makemaz("deepfiln"); return;
			case 15: makemaz("deepfilo"); return;

		}

		break;

	    case 112:
		switch (rnd(10)) {
			case 1: makemaz("deptn-1"); return;
			case 2: makemaz("deptn-2"); return;
			case 3: makemaz("deptn-3"); return;
			case 4: makemaz("deptn-4"); return;
			case 5: makemaz("deptn-5"); return;
			case 6: makemaz("deptn-6"); return;
			case 7: makemaz("deptn-7"); return;
			case 8: makemaz("deptn-8"); return;
			case 9: makemaz("deptn-9"); return;
			case 10: makemaz("deptn-10"); return;

		}

		break;

	    case 113:
		switch (rnd(6)) {
			case 1: makemaz("depeE-1"); return;
			case 2: makemaz("depeE-2"); return;
			case 3: makemaz("depeE-3"); return;
			case 4: makemaz("depeE-4"); return;
			case 5: makemaz("depeE-5"); return;
			case 6: makemaz("depeE-6"); return;

		}

		break;

	    case 114:
		switch (rnd(5)) {
			case 1: makemaz("gruelaiX"); return;
			case 2: makemaz("joustX"); return;
			case 3: makemaz("pmazeX"); return;
			case 4: makemaz("poolhalX"); return;
			case 5: makemaz("dmazeX"); return;

		}

		break;

	    case 115:
	    case 116:

		switch (rnd(8)) {

			case 1: makemaz("grcra"); return;
			case 2: makemaz("grcrb"); return;
			case 3: makemaz("grcrc"); return;
			case 4: makemaz("grcrd"); return;
			case 5: makemaz("grcre"); return;
			case 6: makemaz("grcrf"); return;
			case 7: makemaz("grcrg"); return;
			case 8: makemaz("grcrh"); return;

		}
		break;

	    case 117:
		switch (rnd(3)) {
			case 1: makemaz("eiceqa"); return;
			case 2: makemaz("eiceqb"); return;
			case 3: makemaz("eiceqc"); return;

		}

		break;

		break;

          }

	}

	if ( 
		(In_dod(&u.uz) && (!rn2(100) || depth(&u.uz) > 1) && !rn2((iswarper ? 10000 : 100000)))
		|| (In_mines(&u.uz) /* check moved upwards */ ) || (In_sokoban(&u.uz) && !issokosolver && rn2(iswarper ? 5000 : 2000)) || (In_towndungeon(&u.uz) && !rn2(iswarper ? 3000 : 20000)) ||  (In_deepmines(&u.uz)) || (In_illusorycastle(&u.uz) && !rn2(iswarper ? 3 : 20)) || (In_sewerplant(&u.uz) && !rn2(iswarper ? 20 : 50)) || (In_spacebase(&u.uz) && !rn2(iswarper ? 4 : 30)) || (In_gammacaves(&u.uz) && !rn2(iswarper ? 20 : 50)) || (In_mainframe(&u.uz) && !rn2(iswarper ? 3 : 20)) || (In_gehennom(&u.uz) && !rn2(iswarper ? 3 : 10)) || (In_voiddungeon(&u.uz) && !rn2(iswarper ? 3 : 10)) || (In_netherrealm(&u.uz) && !rn2(iswarper ? 3 : 10)) || (In_swimmingpool(&u.uz) && !rn2(iswarper ? 3 : 10)) || (In_hellbathroom(&u.uz) && !rn2(iswarper ? 20 : 50)) || (In_angmar(&u.uz) && !rn2(iswarper ? 5 : 20)) || (In_emynluin(&u.uz) && !rn2(iswarper ? 20 : 50)) || (rn2(5) && !strcmp(dungeons[u.uz.dnum].dname, "Frankenstein's Lab") ) || (!rn2(1000) && (!strcmp(dungeons[u.uz.dnum].dname, "Grund's Stronghold") || !strcmp(dungeons[u.uz.dnum].dname, "The Ice Queen's Realm") || In_Devnull(&u.uz) || In_greencross(&u.uz) || !strcmp(dungeons[u.uz.dnum].dname, "The Temple of Moloch") || !strcmp(dungeons[u.uz.dnum].dname, "The Giant Caverns") || !strcmp(dungeons[u.uz.dnum].dname, "The Sunless Sea") || !strcmp(dungeons[u.uz.dnum].dname, "The Spider Caves") || !strcmp(dungeons[u.uz.dnum].dname, "Yendorian Tower") || !strcmp(dungeons[u.uz.dnum].dname, "Bell Caves") || !strcmp(dungeons[u.uz.dnum].dname, "Forging Chamber") || !strcmp(dungeons[u.uz.dnum].dname, "Dead Grounds") || !strcmp(dungeons[u.uz.dnum].dname, "Ordered Chaos") || !strcmp(dungeons[u.uz.dnum].dname, "The Lost Tomb") || !strcmp(dungeons[u.uz.dnum].dname, "The Wyrm Caves") || !strcmp(dungeons[u.uz.dnum].dname, "One-eyed Sam's Market") || !strcmp(dungeons[u.uz.dnum].dname, "Fort Ludios") || In_restingzone(&u.uz) ) ) || (In_sheol(&u.uz) && (!rn2(iswarper ? 2 : 5) ) ) ) {

		if (rn2(3)) goto ghnhom2;

	    switch (rnd(117)) {

	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5:
	    case 6:
	    case 7:
	    case 8:
	    case 9:
	    case 10:

		switch (rnd(60)) {

			case 1: makemaz("ghbgr-1"); return;
			case 2: makemaz("ghbgr-2"); return;
			case 3: makemaz("ghbgr-3"); return;
			case 4: makemaz("ghbgr-4"); return;
			case 5: makemaz("ghbgr-5"); return;
			case 6: makemaz("ghbgr-6"); return;
			case 7: makemaz("ghbgr-7"); return;
			case 8: makemaz("ghbgr-8"); return;
			case 9: makemaz("ghbgr-9"); return;
			case 10: makemaz("ghbgr-10"); return;
			case 11: makemaz("ghbgr-11"); return;
			case 12: makemaz("ghbgr-12"); return;
			case 13: makemaz("ghbgr-13"); return;
			case 14: makemaz("ghbgr-14"); return;
			case 15: makemaz("ghbgr-15"); return;
			case 16: makemaz("ghbgr-16"); return;
			case 17: makemaz("ghbgr-17"); return;
			case 18: makemaz("ghbgr-18"); return;
			case 19: makemaz("ghbgr-19"); return;
			case 20: makemaz("ghbgr-20"); return;
			case 21: makemaz("ghbgr-21"); return;
			case 22: makemaz("ghbgr-22"); return;
			case 23: makemaz("ghbgr-23"); return;
			case 24: makemaz("ghbgr-24"); return;
			case 25: makemaz("ghbgr-25"); return;
			case 26: makemaz("ghbgr-26"); return;
			case 27: makemaz("ghbgr-27"); return;
			case 28: makemaz("ghbgr-28"); return;
			case 29: makemaz("ghbgr-29"); return;
			case 30: makemaz("ghbgr-30"); return;
			case 31: makemaz("ghbgr-31"); return;
			case 32: makemaz("ghbgr-32"); return;
			case 33: makemaz("ghbgr-33"); return;
			case 34: makemaz("ghbgr-34"); return;
			case 35: makemaz("ghbgr-35"); return;
			case 36: makemaz("ghbgr-36"); return;
			case 37: makemaz("ghbgr-37"); return;
			case 38: makemaz("ghbgr-38"); return;
			case 39: makemaz("ghbgr-39"); return;
			case 40: makemaz("ghbgr-40"); return;
			case 41: makemaz("ghbgr-41"); return;
			case 42: makemaz("ghbgr-42"); return;
			case 43: makemaz("ghbgr-43"); return;
			case 44: makemaz("ghbgr-44"); return;
			case 45: makemaz("ghbgr-45"); return;
			case 46: makemaz("ghbgr-46"); return;
			case 47: makemaz("ghbgr-47"); return;
			case 48: makemaz("ghbgr-48"); return;
			case 49: makemaz("ghbgr-49"); return;
			case 50: makemaz("ghbgr-50"); return;
			case 51: makemaz("ghbgr-51"); return;
			case 52: makemaz("ghbgr-52"); return;
			case 53: makemaz("ghbgr-53"); return;
			case 54: makemaz("ghbgr-54"); return;
			case 55: makemaz("ghbgr-55"); return;
			case 56: makemaz("ghbgr-56"); return;
			case 57: makemaz("ghbgr-57"); return;
			case 58: makemaz("ghbgr-58"); return;
			case 59: makemaz("ghbgr-59"); return;
			case 60: makemaz("ghbgr-60"); return;

		}
		break;

	    case 11:
	    case 12:
	    case 13:
	    case 14:
	    case 15:
	    case 16:
	    case 17:
	    case 18:
	    case 19:
	    case 20:

		switch (rnd(97)) {

			case 1: makemaz("heck-1"); return;
			case 2: makemaz("heck-2"); return;
			case 3: makemaz("heck-3"); return;
			case 4: makemaz("heck-4"); return;
			case 5: makemaz("heck-5"); return;
			case 6: makemaz("heck-6"); return;
			case 7: makemaz("heck-7"); return;
			case 8: makemaz("heck-8"); return;
			case 9: makemaz("heck-9"); return;
			case 10: makemaz("heck-10"); return;
			case 11: makemaz("heck-11"); return;
			case 12: makemaz("heck-12"); return;
			case 13: makemaz("heck-13"); return;
			case 14: makemaz("heck-14"); return;
			case 15: makemaz("heck-15"); return;
			case 16: makemaz("heck-16"); return;
			case 17: makemaz("heck-17"); return;
			case 18: makemaz("heck-18"); return;
			case 19: makemaz("heck-19"); return;
			case 20: makemaz("heck-20"); return;
			case 21: makemaz("heck-21"); return;
			case 22: makemaz("heck-22"); return;
			case 23: makemaz("heck-23"); return;
			case 24: makemaz("heck-24"); return;
			case 25: makemaz("heck-25"); return;
			case 26: makemaz("heck-26"); return;
			case 27: makemaz("heck-27"); return;
			case 28: makemaz("heck-28"); return;
			case 29: makemaz("heck-29"); return;
			case 30: makemaz("heck-30"); return;
			case 31: makemaz("heck-31"); return;
			case 32: makemaz("heck-32"); return;
			case 33: makemaz("heck-33"); return;
			case 34: makemaz("heck-34"); return;
			case 35: makemaz("heck-35"); return;
			case 36: makemaz("heck-36"); return;
			case 37: makemaz("heck-37"); return;
			case 38: makemaz("heck-38"); return;
			case 39: makemaz("heck-39"); return;
			case 40: makemaz("heck-40"); return;
			case 41: makemaz("heck-41"); return;
			case 42: makemaz("heck-42"); return;
			case 43: makemaz("heck-43"); return;
			case 44: makemaz("heck-44"); return;
			case 45: makemaz("heck-45"); return;
			case 46: makemaz("heck-46"); return;
			case 47: makemaz("heck-47"); return;
			case 48: makemaz("heck-48"); return;
			case 49: makemaz("heck-49"); return;
			case 50: makemaz("heck-50"); return;
			case 51: makemaz("heck-51"); return;
			case 52: makemaz("heck-52"); return;
			case 53: makemaz("heck-53"); return;
			case 54: makemaz("heck-54"); return;
			case 55: makemaz("heck-55"); return;
			case 56: makemaz("heck-56"); return;
			case 57: makemaz("heck-57"); return;
			case 58: makemaz("heck-58"); return;
			case 59: makemaz("heck-59"); return;
			case 60: makemaz("heck-60"); return;
			case 61: makemaz("heck-61"); return;
			case 62: makemaz("heck-62"); return;
			case 63: makemaz("heck-63"); return;
			case 64: makemaz("heck-64"); return;
			case 65: makemaz("heck-65"); return;
			case 66: makemaz("heck-66"); return;
			case 67: makemaz("heck-67"); return;
			case 68: makemaz("heck-68"); return;
			case 69: makemaz("heck-69"); return;
			case 70: makemaz("heck-70"); return;
			case 71: makemaz("heck-71"); return;
			case 72: makemaz("heck-72"); return;
			case 73: makemaz("heck-73"); return;
			case 74: makemaz("heck-74"); return;
			case 75: makemaz("heck-75"); return;
			case 76: makemaz("heck-76"); return;
			case 77: makemaz("heck-77"); return;
			case 78: makemaz("heck-78"); return;
			case 79: makemaz("heck-79"); return;
			case 80: makemaz("heck-80"); return;
			case 81: makemaz("heck-81"); return;
			case 82: makemaz("heck-82"); return;
			case 83: makemaz("heck-83"); return;
			case 84: makemaz("heck-84"); return;
			case 85: makemaz("heck-85"); return;
			case 86: makemaz("heck-86"); return;
			case 87: makemaz("heck-87"); return;
			case 88: makemaz("heck-88"); return;
			case 89: makemaz("heck-89"); return;
			case 90: makemaz("heck-90"); return;
			case 91: makemaz("heck-91"); return;
			case 92: makemaz("heck-92"); return;
			case 93: makemaz("heck-93"); return;
			case 94: makemaz("heck-94"); return;
			case 95: makemaz("heck-95"); return;
			case 96: makemaz("heck-96"); return;
			case 97: makemaz("minusw"); return;

		}
		break;

	    case 21:

		switch (rnd(5)) {

			case 1: makemaz("intplx-1"); return;
			case 2: makemaz("intplx-2"); return;
			case 3: makemaz("intplx-3"); return;
			case 4: makemaz("intplx-4"); return;
			case 5: makemaz("intplx-5"); return;

		}
		break;

	    case 22:
	    case 23:
	    case 24:

		switch (rnd(14)) {

			case 1: makemaz("hellfill"); return;
			case 2: makemaz("hellfila"); return;
			case 3: makemaz("hellfilb"); return;
			case 4: makemaz("hellfilc"); return;
			case 5: makemaz("hellfild"); return;
			case 6: makemaz("hellfile"); return;
			case 7: makemaz("hellfilf"); return;
			case 8: makemaz("hellfilg"); return;
			case 9: makemaz("hellfill"); return;
			case 10: makemaz("hellfill"); return;
			case 11: makemaz("hellfill"); return;
			case 12: makemaz("hellfill"); return;
			case 13: makemaz("hellfill"); return;
			case 14: makemaz("hellfill"); return;

		}
		break;

	    case 25:
	    case 26:
	    case 27:
	    case 28:
	    case 29:

		switch (rnd(21)) {

			case 1: makemaz("hmint-1"); return;
			case 2: makemaz("hmint-2"); return;
			case 3: makemaz("hmint-3"); return;
			case 4: makemaz("hmint-4"); return;
			case 5: makemaz("hmint-5"); return;
			case 6: makemaz("hmint-6"); return;
			case 7: makemaz("hmint-7"); return;
			case 8: makemaz("hmint-8"); return;
			case 9: makemaz("hmint-9"); return;
			case 10: makemaz("hmint-10"); return;
			case 11: makemaz("hmint-11"); return;
			case 12: makemaz("hmint-12"); return;
			case 13: makemaz("hmint-13"); return;
			case 14: makemaz("hmint-14"); return;
			case 15: makemaz("hmint-15"); return;
			case 16: makemaz("hmint-16"); return;
			case 17: makemaz("hmint-17"); return;
			case 18: makemaz("hmint-18"); return;
			case 19: makemaz("hmint-19"); return;
			case 20: makemaz("hmint-20"); return;
			case 21: makemaz("hmint-21"); return;

		}
		break;

	    case 30:
	    case 31:

		switch (rnd(10)) {

			case 1: makemaz("hmnen-1"); return;
			case 2: makemaz("hmnen-2"); return;
			case 3: makemaz("hmnen-3"); return;
			case 4: makemaz("hmnen-4"); return;
			case 5: makemaz("hmnen-5"); return;
			case 6: makemaz("hmnen-6"); return;
			case 7: makemaz("hmnen-7"); return;
			case 8: makemaz("hmnen-8"); return;
			case 9: makemaz("hmnen-9"); return;
			case 10: makemaz("hmnen-10"); return;

		}
		break;

	    case 32:
	    case 33:

		switch (rnd(18)) {

			case 1: makemaz("hking-1"); return;
			case 2: makemaz("hking-2"); return;
			case 3: makemaz("hking-3"); return;
			case 4: makemaz("hking-4"); return;
			case 5: makemaz("hking-5"); return;
			case 6: makemaz("hking-6"); return;
			case 7: makemaz("hking-7"); return;
			case 8: makemaz("hking-8"); return;
			case 9: makemaz("hking-9"); return;
			case 10: makemaz("hking-10"); return;
			case 11: makemaz("hking-11"); return;
			case 12: makemaz("hking-12"); return;
			case 13: makemaz("hking-13"); return;
			case 14: makemaz("hking-14"); return;
			case 15: makemaz("hking-15"); return;
			case 16: makemaz("hking-16"); return;
			case 17: makemaz("hking-17"); return;
			case 18: makemaz("hking-18"); return;

		}
		break;

	    case 34:
	    case 35:
	    case 36:
	    case 37:
	    case 38:
	    case 39:
	    case 40:
	    case 41:
	    case 42:
	    case 43:

		switch (rnd(154)) {

			case 1: makemaz("soko8-1"); return;
			case 2: makemaz("soko8-2"); return;
			case 3: makemaz("soko8-3"); return;
			case 4: makemaz("soko8-4"); return;
			case 5: makemaz("soko8-5"); return;
			case 6: makemaz("soko8-6"); return;
			case 7: makemaz("soko8-7"); return;
			case 8: makemaz("soko8-8"); return;
			case 9: makemaz("soko8-9"); return;
			case 10: makemaz("soko8-10"); return;
			case 11: makemaz("soko8-11"); return;
			case 12: makemaz("soko8-12"); return;
			case 13: makemaz("soko8-13"); return;
			case 14: makemaz("soko8-14"); return;
			case 15: makemaz("soko8-15"); return;
			case 16: makemaz("soko8-16"); return;
			case 17: makemaz("soko8-17"); return;
			case 18: makemaz("soko8-18"); return;
			case 19: makemaz("soko8-19"); return;
			case 20: makemaz("soko8-20"); return;
			case 21: makemaz("soko8-21"); return;
			case 22: makemaz("soko8-22"); return;
			case 23: makemaz("soko8-23"); return;
			case 24: makemaz("soko8-24"); return;
			case 25: makemaz("soko8-25"); return;
			case 26: makemaz("soko8-26"); return;
			case 27: makemaz("soko8-27"); return;
			case 28: makemaz("soko8-28"); return;
			case 29: makemaz("soko8-29"); return;
			case 30: makemaz("soko8-30"); return;
			case 31: makemaz("soko8-31"); return;
			case 32: makemaz("soko8-32"); return;
			case 33: makemaz("soko8-33"); return;
			case 34: makemaz("soko8-34"); return;
			case 35: makemaz("soko8-35"); return;
			case 36: makemaz("soko8-36"); return;
			case 37: makemaz("soko8-37"); return;
			case 38: makemaz("soko8-38"); return;
			case 39: makemaz("soko8-39"); return;
			case 40: makemaz("soko8-40"); return;
			case 41: makemaz("soko8-41"); return;
			case 42: makemaz("soko8-42"); return;
			case 43: makemaz("soko8-43"); return;
			case 44: makemaz("soko8-44"); return;

			case 45: makemaz("soko7-1"); return;
			case 46: makemaz("soko7-2"); return;
			case 47: makemaz("soko7-3"); return;
			case 48: makemaz("soko7-4"); return;
			case 49: makemaz("soko7-5"); return;
			case 50: makemaz("soko7-6"); return;
			case 51: makemaz("soko7-7"); return;
			case 52: makemaz("soko7-8"); return;
			case 53: makemaz("soko7-9"); return;
			case 54: makemaz("soko7-10"); return;
			case 55: makemaz("soko7-11"); return;
			case 56: makemaz("soko7-12"); return;
			case 57: makemaz("soko7-13"); return;
			case 58: makemaz("soko7-14"); return;
			case 59: makemaz("soko7-15"); return;
			case 60: makemaz("soko7-16"); return;
			case 61: makemaz("soko7-17"); return;
			case 62: makemaz("soko7-18"); return;
			case 63: makemaz("soko7-19"); return;
			case 64: makemaz("soko7-20"); return;
			case 65: makemaz("soko7-21"); return;
			case 66: makemaz("soko7-22"); return;
			case 67: makemaz("soko7-23"); return;
			case 68: makemaz("soko7-24"); return;
			case 69: makemaz("soko7-25"); return;
			case 70: makemaz("soko7-26"); return;
			case 71: makemaz("soko7-27"); return;
			case 72: makemaz("soko7-28"); return;
			case 73: makemaz("soko7-29"); return;

			case 74: makemaz("soko9-1"); return;
			case 75: makemaz("soko9-2"); return;
			case 76: makemaz("soko9-3"); return;
			case 77: makemaz("soko9-4"); return;
			case 78: makemaz("soko9-5"); return;
			case 79: makemaz("soko9-6"); return;
			case 80: makemaz("soko9-7"); return;
			case 81: makemaz("soko9-8"); return;
			case 82: makemaz("soko9-9"); return;
			case 83: makemaz("soko9-10"); return;
			case 84: makemaz("soko9-11"); return;
			case 85: makemaz("soko9-12"); return;
			case 86: makemaz("soko9-13"); return;
			case 87: makemaz("soko9-14"); return;
			case 88: makemaz("soko9-15"); return;
			case 89: makemaz("soko9-16"); return;
			case 90: makemaz("soko9-17"); return;
			case 91: makemaz("soko9-18"); return;
			case 92: makemaz("soko9-19"); return;
			case 93: makemaz("soko9-20"); return;
			case 94: makemaz("soko9-21"); return;
			case 95: makemaz("soko9-22"); return;
			case 96: makemaz("soko9-23"); return;
			case 97: makemaz("soko9-24"); return;
			case 98: makemaz("soko9-25"); return;
			case 99: makemaz("soko9-26"); return;
			case 100: makemaz("soko9-27"); return;
			case 101: makemaz("soko9-28"); return;

			case 102: makemaz("soko0-1"); return;
			case 103: makemaz("soko0-2"); return;
			case 104: makemaz("soko0-3"); return;
			case 105: makemaz("soko0-4"); return;
			case 106: makemaz("soko0-5"); return;
			case 107: makemaz("soko0-6"); return;
			case 108: makemaz("soko0-7"); return;
			case 109: makemaz("soko0-8"); return;
			case 110: makemaz("soko0-9"); return;
			case 111: makemaz("soko0-10"); return;
			case 112: makemaz("soko0-11"); return;
			case 113: makemaz("soko0-12"); return;
			case 114: makemaz("soko0-13"); return;
			case 115: makemaz("soko0-14"); return;
			case 116: makemaz("soko0-15"); return;
			case 117: makemaz("soko0-16"); return;
			case 118: makemaz("soko0-17"); return;
			case 119: makemaz("soko0-18"); return;
			case 120: makemaz("soko0-19"); return;
			case 121: makemaz("soko0-20"); return;
			case 122: makemaz("soko0-21"); return;
			case 123: makemaz("soko0-22"); return;
			case 124: makemaz("soko0-23"); return;
			case 125: makemaz("soko0-24"); return;
			case 126: makemaz("soko0-25"); return;
			case 127: makemaz("soko0-26"); return;
			case 128: makemaz("soko0-27"); return;
			case 129: makemaz("soko0-28"); return;
			case 130: makemaz("soko0-29"); return;
			case 131: makemaz("soko0-30"); return;
			case 132: makemaz("soko0-31"); return;
			case 133: makemaz("soko0-32"); return;
			case 134: makemaz("soko0-33"); return;
			case 135: makemaz("soko0-34"); return;
			case 136: makemaz("soko0-35"); return;
			case 137: makemaz("soko0-36"); return;
			case 138: makemaz("soko0-37"); return;
			case 139: makemaz("soko0-38"); return;
			case 140: makemaz("soko0-39"); return;
			case 141: makemaz("soko0-40"); return;
			case 142: makemaz("soko0-41"); return;
			case 143: makemaz("soko0-42"); return;
			case 144: makemaz("soko0-43"); return;
			case 145: makemaz("soko0-44"); return;
			case 146: makemaz("soko0-45"); return;
			case 147: makemaz("soko0-46"); return;
			case 148: makemaz("soko0-47"); return;
			case 149: makemaz("soko0-48"); return;
			case 150: makemaz("soko0-49"); return;
			case 151: makemaz("soko0-50"); return;
			case 152: makemaz("soko0-51"); return;

			case 153: makemaz("soko8-45"); return;

			case 154: makemaz("soko0-52"); return;

		}
		break;

	    case 44:
	    case 45:

		switch (rnd(10)) {

			case 1: makemaz("hllma-1"); return;
			case 2: makemaz("hllma-2"); return;
			case 3: makemaz("hllma-3"); return;
			case 4: makemaz("hllma-4"); return;
			case 5: makemaz("hllma-5"); return;
			case 6: makemaz("hllma-6"); return;
			case 7: makemaz("hllma-7"); return;
			case 8: makemaz("hllma-8"); return;
			case 9: makemaz("hllma-9"); return;
			case 10: makemaz("hllma-10"); return;

		}
		break;

	    case 46:

		makemaz("oracleX"); return;
		break;

	    case 47:

		switch (rnd(4)) {

			case 1: makemaz("hrats-1"); return;
			case 2: makemaz("hrats-2"); return;
			case 3: makemaz("hrats-3"); return;
			case 4: makemaz("hrats-4"); return;

		}
		break;

	    case 48:

		switch (rnd(6)) {

			case 1: makemaz("hkobol-1"); return;
			case 2: makemaz("hkobol-2"); return;
			case 3: makemaz("hkobol-3"); return;
			case 4: makemaz("hkobol-4"); return;
			case 5: makemaz("hkobol-5"); return;
			case 6: makemaz("hkobol-6"); return;

		}
		break;

	    case 49:

		switch (rnd(5)) {

			case 1: makemaz("hnymp-1"); return;
			case 2: makemaz("hnymp-2"); return;
			case 3: makemaz("hnymp-3"); return;
			case 4: makemaz("hnymp-4"); return;
			case 5: makemaz("hnymp-5"); return;

		}
		break;

	    case 50:

		switch (rnd(9)) {

			case 1: makemaz("hstor-1"); return;
			case 2: makemaz("hstor-2"); return;
			case 3: makemaz("hstor-3"); return;
			case 4: makemaz("hstor-4"); return;
			case 5: makemaz("hstor-5"); return;
			case 6: makemaz("hstor-6"); return;
			case 7: makemaz("hstor-7"); return;
			case 8: makemaz("hstor-8"); return;
			case 9: makemaz("hstor-9"); return;

		}
		break;

	    case 51:

		switch (rnd(8)) {

			case 1: makemaz("guild-1"); return;
			case 2: makemaz("guild-2"); return;
			case 3: makemaz("guild-3"); return;
			case 4: makemaz("guild-4"); return;
			case 5: makemaz("guild-5"); return;
			case 6: makemaz("guild-6"); return;
			case 7: makemaz("guild-7"); return;
			case 8: makemaz("guild-8"); return;

		}
		break;

	    case 52:

		switch (rnd(9)) {

			case 1: makemaz("forgeX"); return;
			case 2: makemaz("hitchX"); return;
			case 3: makemaz("compuX"); return;
			case 4: makemaz("keyX"); return;
			case 5: makemaz("robtnH"); return;
			case 6: makemaz("rabhoH"); return;
			case 7: makemaz("machnH"); return;
			case 8: makemaz("orcbaH"); return;
			case 9: makemaz("sewplH"); return;

		}
		break;

	    case 53:
	    case 54:

		switch (rnd(12)) {

			case 1: makemaz("hmedu-1"); return;
			case 2: makemaz("hmedu-2"); return;
			case 3: makemaz("hmedu-3"); return;
			case 4: makemaz("hmedu-4"); return;
			case 5: makemaz("hmedu-5"); return;
			case 6: makemaz("hmedu-6"); return;
			case 7: makemaz("hmedu-7"); return;
			case 8: makemaz("hmedu-8"); return;
			case 9: makemaz("hmedu-9"); return;
			case 10: makemaz("hmedu-10"); return;
			case 11: makemaz("hmedu-11"); return;
			case 12: makemaz("hmedu-12"); return;

		}
		break;

	    case 55:
	    case 56:

		switch (rnd(15)) {

			case 1: makemaz("hcstl-1"); return;
			case 2: makemaz("hcstl-2"); return;
			case 3: makemaz("hcstl-3"); return;
			case 4: makemaz("hcstl-4"); return;
			case 5: makemaz("hcstl-5"); return;
			case 6: makemaz("hcstl-6"); return;
			case 7: makemaz("hcstl-7"); return;
			case 8: makemaz("hcstl-8"); return;
			case 9: makemaz("hcstl-9"); return;
			case 10: makemaz("hcstl-10"); return;
			case 11: makemaz("hcstl-11"); return;
			case 12: makemaz("hcstl-12"); return;
			case 13: makemaz("hcstl-13"); return;
			case 14: makemaz("hcstl-14"); return;
			case 15: makemaz("hcstl-15"); return;

		}
		break;

	    case 57:
	    case 58:
	    case 59:
	    case 60:
	    case 61:

		switch (rnd(88)) {

			case 1: makemaz("hgehn-1"); return;
			case 2: makemaz("hgehn-2"); return;
			case 3: makemaz("hgehn-3"); return;
			case 4: makemaz("hgehn-4"); return;
			case 5: makemaz("hgehn-5"); return;
			case 6: makemaz("hgehn-6"); return;
			case 7: makemaz("hgehn-7"); return;
			case 8: makemaz("hgehn-8"); return;
			case 9: makemaz("hgehn-9"); return;
			case 10: makemaz("hgehn-10"); return;
			case 11: makemaz("hgehn-11"); return;
			case 12: makemaz("hgehn-12"); return;
			case 13: makemaz("hgehn-13"); return;
			case 14: makemaz("hgehn-14"); return;
			case 15: makemaz("hgehn-15"); return;
			case 16: makemaz("hgehn-16"); return;
			case 17: makemaz("hgehn-17"); return;
			case 18: makemaz("hgehn-18"); return;
			case 19: makemaz("hgehn-19"); return;
			case 20: makemaz("hgehn-20"); return;
			case 21: makemaz("hgehn-21"); return;
			case 22: makemaz("hgehn-22"); return;
			case 23: makemaz("hgehn-23"); return;
			case 24: makemaz("hgehn-24"); return;
			case 25: makemaz("hgehn-25"); return;
			case 26: makemaz("hgehn-26"); return;
			case 27: makemaz("hgehn-27"); return;
			case 28: makemaz("hgehn-28"); return;
			case 29: makemaz("hgehn-29"); return;
			case 30: makemaz("hgehn-30"); return;
			case 31: makemaz("hgehn-31"); return;
			case 32: makemaz("hgehn-32"); return;
			case 33: makemaz("hgehn-33"); return;
			case 34: makemaz("hgehn-34"); return;
			case 35: makemaz("hgehn-35"); return;
			case 36: makemaz("hgehn-36"); return;
			case 37: makemaz("hgehn-37"); return;
			case 38: makemaz("hgehn-38"); return;
			case 39: makemaz("hgehn-39"); return;
			case 40: makemaz("hgehn-40"); return;
			case 41: makemaz("hgehn-41"); return;
			case 42: makemaz("hgehn-42"); return;
			case 43: makemaz("hgehn-43"); return;
			case 44: makemaz("hgehn-44"); return;
			case 45: makemaz("hgehn-45"); return;
			case 46: makemaz("hgehn-46"); return;
			case 47: makemaz("hgehn-47"); return;
			case 48: makemaz("hgehn-48"); return;
			case 49: makemaz("hgehn-49"); return;
			case 50: makemaz("hgehn-50"); return;
			case 51: makemaz("hgehn-51"); return;
			case 52: makemaz("hgehn-52"); return;
			case 53: makemaz("hgehn-53"); return;
			case 54: makemaz("hgehn-54"); return;
			case 55: makemaz("hgehn-55"); return;
			case 56: makemaz("hgehn-56"); return;
			case 57: makemaz("hgehn-57"); return;
			case 58: makemaz("hgehn-58"); return;
			case 59: makemaz("hgehn-59"); return;
			case 60: makemaz("hgehn-60"); return;
			case 61: makemaz("hgehn-61"); return;
			case 62: makemaz("hgehn-62"); return;
			case 63: makemaz("hgehn-63"); return;
			case 64: makemaz("hgehn-64"); return;
			case 65: makemaz("hgehn-65"); return;
			case 66: makemaz("hgehn-66"); return;
			case 67: makemaz("hgehn-67"); return;
			case 68: makemaz("hgehn-68"); return;
			case 69: makemaz("hgehn-69"); return;
			case 70: makemaz("hgehn-70"); return;
			case 71: makemaz("hgehn-71"); return;
			case 72: makemaz("hgehn-72"); return;
			case 73: makemaz("hgehn-73"); return;
			case 74: makemaz("hgehn-74"); return;
			case 75: makemaz("hgehn-75"); return;
			case 76: makemaz("hgehn-76"); return;
			case 77: makemaz("hgehn-77"); return;
			case 78: makemaz("hgehn-78"); return;
			case 79: makemaz("hgehn-79"); return;
			case 80: makemaz("hgehn-80"); return;
			case 81: makemaz("hgehn-81"); return;
			case 82: makemaz("hgehn-82"); return;
			case 83: makemaz("hgehn-83"); return;
			case 84: makemaz("hgehn-84"); return;
			case 85: makemaz("hgehn-85"); return;
			case 86: makemaz("hgehn-86"); return;
			case 87: makemaz("hgehn-87"); return;
			case 88: makemaz("hgehn-88"); return;

		}
		break;

	    case 62:

		makemaz("schoolX"); return;
		break;

	    case 63:

		switch (rnd(4)) {

			case 1: makemaz("htown-1"); return;
			case 2: makemaz("htown-2"); return;
			case 3: makemaz("htown-3"); return;
			case 4: makemaz("htown-4"); return;

		}
		break;

	    case 64:

		switch (rnd(3)) {

			case 1: makemaz("hgrund-1"); return;
			case 2: makemaz("hgrund-2"); return;
			case 3: makemaz("hgrund-3"); return;

		}
		break;

	    case 65:

		switch (rnd(3)) {

			case 1: makemaz("hknox-1"); return;
			case 2: makemaz("hknox-2"); return;
			case 3: makemaz("hknox-3"); return;

		}
		break;

	    case 66:

		makemaz("hdragons"); return;
		break;

	    case 67:

		makemaz("htomb"); return;
		break;

	    case 68:

		makemaz("hspiders"); return;
		break;

	    case 69:

		makemaz("hsea"); return;
		break;

	    case 70:

		makemaz("hmtemple"); return;
		break;

	    case 71:
	    case 72:
	    case 73:

		switch (rnd(22)) {

			case 1: makemaz("hsheo-1"); return;
			case 2: makemaz("hsheo-2"); return;
			case 3: makemaz("hsheo-3"); return;
			case 4: makemaz("hsheo-4"); return;
			case 5: makemaz("hsheo-5"); return;
			case 6: makemaz("hsheo-6"); return;
			case 7: makemaz("hsheo-7"); return;
			case 8: makemaz("hsheo-8"); return;
			case 9: makemaz("hsheo-9"); return;
			case 10: makemaz("hsheo-10"); return;
			case 11: makemaz("hsheo-11"); return;
			case 12: makemaz("hsheo-12"); return;
			case 13: makemaz("hsheo-13"); return;
			case 14: makemaz("hsheo-14"); return;
			case 15: makemaz("hsheo-15"); return;
			case 16: makemaz("hsheo-16"); return;
			case 17: makemaz("hsheo-17"); return;
			case 18: makemaz("hsheo-18"); return;
			case 19: makemaz("hsheo-19"); return;
			case 20: makemaz("hsheo-20"); return;
			case 21: makemaz("hsheo-21"); return;
			case 22: makemaz("hsheo-22"); return;

		}
		break;

	    case 74:
	    case 75:
	    case 76:
	    case 77:
	    case 78:
	    case 79:
	    case 80:
	    case 81:
	    case 82:
	    case 83:
	    case 84:
	    case 85:
	    case 86:
	    case 87:
	    case 88:
	    case 89:
	    case 90:
	    case 91:
	    case 92:
	    case 93:

		switch (rnd(855)) {

			case 1: makemaz("Aci-6"); return;
			case 2: makemaz("Aci-7"); return;
			case 3: makemaz("Aci-8"); return;
			case 4: makemaz("Aci-9"); return;
			case 5: makemaz("Aci-0"); return;
			case 6: makemaz("Act-6"); return;
			case 7: makemaz("Act-7"); return;
			case 8: makemaz("Act-8"); return;
			case 9: makemaz("Act-9"); return;
			case 10: makemaz("Act-0"); return;
			case 11: makemaz("Alt-6"); return;
			case 12: makemaz("Alt-7"); return;
			case 13: makemaz("Alt-8"); return;
			case 14: makemaz("Alt-9"); return;
			case 15: makemaz("Alt-0"); return;
			case 16: makemaz("Ama-6"); return;
			case 17: makemaz("Ama-7"); return;
			case 18: makemaz("Ama-8"); return;
			case 19: makemaz("Ama-9"); return;
			case 20: makemaz("Ama-0"); return;
			case 21: makemaz("Arc-6"); return;
			case 22: makemaz("Arc-7"); return;
			case 23: makemaz("Arc-8"); return;
			case 24: makemaz("Arc-9"); return;
			case 25: makemaz("Arc-0"); return;
			case 26: makemaz("Art-6"); return;
			case 27: makemaz("Art-7"); return;
			case 28: makemaz("Art-8"); return;
			case 29: makemaz("Art-9"); return;
			case 30: makemaz("Art-0"); return;
			case 31: makemaz("Ass-6"); return;
			case 32: makemaz("Ass-7"); return;
			case 33: makemaz("Ass-8"); return;
			case 34: makemaz("Ass-9"); return;
			case 35: makemaz("Ass-0"); return;
			case 36: makemaz("Aug-6"); return;
			case 37: makemaz("Aug-7"); return;
			case 38: makemaz("Aug-8"); return;
			case 39: makemaz("Aug-9"); return;
			case 40: makemaz("Aug-0"); return;
			case 41: makemaz("Bar-6"); return;
			case 42: makemaz("Bar-7"); return;
			case 43: makemaz("Bar-8"); return;
			case 44: makemaz("Bar-9"); return;
			case 45: makemaz("Bar-0"); return;
			case 46: makemaz("Brd-6"); return;
			case 47: makemaz("Brd-7"); return;
			case 48: makemaz("Brd-8"); return;
			case 49: makemaz("Brd-9"); return;
			case 50: makemaz("Brd-0"); return;
			case 51: makemaz("Bin-6"); return;
			case 52: makemaz("Bin-7"); return;
			case 53: makemaz("Bin-8"); return;
			case 54: makemaz("Bin-9"); return;
			case 55: makemaz("Bin-0"); return;
			case 56: makemaz("Ble-6"); return;
			case 57: makemaz("Ble-7"); return;
			case 58: makemaz("Ble-8"); return;
			case 59: makemaz("Ble-9"); return;
			case 60: makemaz("Ble-0"); return;
			case 61: makemaz("Blo-6"); return;
			case 62: makemaz("Blo-7"); return;
			case 63: makemaz("Blo-8"); return;
			case 64: makemaz("Blo-9"); return;
			case 65: makemaz("Blo-0"); return;
			case 66: makemaz("Bos-6"); return;
			case 67: makemaz("Bos-7"); return;
			case 68: makemaz("Bos-8"); return;
			case 69: makemaz("Bos-9"); return;
			case 70: makemaz("Bos-0"); return;
			case 71: makemaz("Bul-6"); return;
			case 72: makemaz("Bul-7"); return;
			case 73: makemaz("Bul-8"); return;
			case 74: makemaz("Bul-9"); return;
			case 75: makemaz("Bul-0"); return;
			case 76: makemaz("Cav-6"); return;
			case 77: makemaz("Cav-7"); return;
			case 78: makemaz("Cav-8"); return;
			case 79: makemaz("Cav-9"); return;
			case 80: makemaz("Cav-0"); return;
			case 81: makemaz("Che-6"); return;
			case 82: makemaz("Che-7"); return;
			case 83: makemaz("Che-8"); return;
			case 84: makemaz("Che-9"); return;
			case 85: makemaz("Che-0"); return;
			case 86: makemaz("Con-6"); return;
			case 87: makemaz("Con-7"); return;
			case 88: makemaz("Con-8"); return;
			case 89: makemaz("Con-9"); return;
			case 90: makemaz("Con-0"); return;
			case 91: makemaz("Coo-6"); return;
			case 92: makemaz("Coo-7"); return;
			case 93: makemaz("Coo-8"); return;
			case 94: makemaz("Coo-9"); return;
			case 95: makemaz("Coo-0"); return;
			case 96: makemaz("Cou-6"); return;
			case 97: makemaz("Cou-7"); return;
			case 98: makemaz("Cou-8"); return;
			case 99: makemaz("Cou-9"); return;
			case 100: makemaz("Cou-0"); return;
			case 101: makemaz("Abu-6"); return;
			case 102: makemaz("Abu-7"); return;
			case 103: makemaz("Abu-8"); return;
			case 104: makemaz("Abu-9"); return;
			case 105: makemaz("Abu-0"); return;
			case 106: makemaz("Dea-6"); return;
			case 107: makemaz("Dea-7"); return;
			case 108: makemaz("Dea-8"); return;
			case 109: makemaz("Dea-9"); return;
			case 110: makemaz("Dea-0"); return;
			case 111: makemaz("Div-6"); return;
			case 112: makemaz("Div-7"); return;
			case 113: makemaz("Div-8"); return;
			case 114: makemaz("Div-9"); return;
			case 115: makemaz("Div-0"); return;
			case 116: makemaz("Dol-6"); return;
			case 117: makemaz("Dol-7"); return;
			case 118: makemaz("Dol-8"); return;
			case 119: makemaz("Dol-9"); return;
			case 120: makemaz("Dol-0"); return;
			case 121: makemaz("Dru-6"); return;
			case 122: makemaz("Dru-7"); return;
			case 123: makemaz("Dru-8"); return;
			case 124: makemaz("Dru-9"); return;
			case 125: makemaz("Dru-0"); return;
			case 126: makemaz("Dun-6"); return;
			case 127: makemaz("Dun-7"); return;
			case 128: makemaz("Dun-8"); return;
			case 129: makemaz("Dun-9"); return;
			case 130: makemaz("Dun-0"); return;
			case 131: makemaz("Ele-6"); return;
			case 132: makemaz("Ele-7"); return;
			case 133: makemaz("Ele-8"); return;
			case 134: makemaz("Ele-9"); return;
			case 135: makemaz("Ele-0"); return;
			case 136: makemaz("Elp-6"); return;
			case 137: makemaz("Elp-7"); return;
			case 138: makemaz("Elp-8"); return;
			case 139: makemaz("Elp-9"); return;
			case 140: makemaz("Elp-0"); return;
			case 141: makemaz("Stu-6"); return;
			case 142: makemaz("Stu-7"); return;
			case 143: makemaz("Stu-8"); return;
			case 144: makemaz("Stu-9"); return;
			case 145: makemaz("Stu-0"); return;
			case 146: makemaz("Fir-6"); return;
			case 147: makemaz("Fir-7"); return;
			case 148: makemaz("Fir-8"); return;
			case 149: makemaz("Fir-9"); return;
			case 150: makemaz("Fir-0"); return;
			case 151: makemaz("Fla-6"); return;
			case 152: makemaz("Fla-7"); return;
			case 153: makemaz("Fla-8"); return;
			case 154: makemaz("Fla-9"); return;
			case 155: makemaz("Fla-0"); return;
			case 156: makemaz("Fox-6"); return;
			case 157: makemaz("Fox-7"); return;
			case 158: makemaz("Fox-8"); return;
			case 159: makemaz("Fox-9"); return;
			case 160: makemaz("Fox-0"); return;
			case 161: makemaz("Gam-6"); return;
			case 162: makemaz("Gam-7"); return;
			case 163: makemaz("Gam-8"); return;
			case 164: makemaz("Gam-9"); return;
			case 165: makemaz("Gam-0"); return;
			case 166: makemaz("Gan-6"); return;
			case 167: makemaz("Gan-7"); return;
			case 168: makemaz("Gan-8"); return;
			case 169: makemaz("Gan-9"); return;
			case 170: makemaz("Gan-0"); return;
			case 171: makemaz("Gee-6"); return;
			case 172: makemaz("Gee-7"); return;
			case 173: makemaz("Gee-8"); return;
			case 174: makemaz("Gee-9"); return;
			case 175: makemaz("Gee-0"); return;
			case 176: makemaz("Gla-6"); return;
			case 177: makemaz("Gla-7"); return;
			case 178: makemaz("Gla-8"); return;
			case 179: makemaz("Gla-9"); return;
			case 180: makemaz("Gla-0"); return;
			case 181: makemaz("Gof-6"); return;
			case 182: makemaz("Gof-7"); return;
			case 183: makemaz("Gof-8"); return;
			case 184: makemaz("Gof-9"); return;
			case 185: makemaz("Gof-0"); return;
			case 186: makemaz("Gra-6"); return;
			case 187: makemaz("Gra-7"); return;
			case 188: makemaz("Gra-8"); return;
			case 189: makemaz("Gra-9"); return;
			case 190: makemaz("Gra-0"); return;
			case 191: makemaz("Gun-6"); return;
			case 192: makemaz("Gun-7"); return;
			case 193: makemaz("Gun-8"); return;
			case 194: makemaz("Gun-9"); return;
			case 195: makemaz("Gun-0"); return;
			case 196: makemaz("Hea-6"); return;
			case 197: makemaz("Hea-7"); return;
			case 198: makemaz("Hea-8"); return;
			case 199: makemaz("Hea-9"); return;
			case 200: makemaz("Hea-0"); return;
			case 201: makemaz("Ice-6"); return;
			case 202: makemaz("Ice-7"); return;
			case 203: makemaz("Ice-8"); return;
			case 204: makemaz("Ice-9"); return;
			case 205: makemaz("Ice-0"); return;
			case 206: makemaz("Scr-6"); return;
			case 207: makemaz("Scr-7"); return;
			case 208: makemaz("Scr-8"); return;
			case 209: makemaz("Scr-9"); return;
			case 210: makemaz("Scr-0"); return;
			case 211: makemaz("Jed-6"); return;
			case 212: makemaz("Jed-7"); return;
			case 213: makemaz("Jed-8"); return;
			case 214: makemaz("Jed-9"); return;
			case 215: makemaz("Jed-0"); return;
			case 216: makemaz("Jes-6"); return;
			case 217: makemaz("Jes-7"); return;
			case 218: makemaz("Jes-8"); return;
			case 219: makemaz("Jes-9"); return;
			case 220: makemaz("Jes-0"); return;
			case 221: makemaz("Kni-6"); return;
			case 222: makemaz("Kni-7"); return;
			case 223: makemaz("Kni-8"); return;
			case 224: makemaz("Kni-9"); return;
			case 225: makemaz("Kni-0"); return;
			case 226: makemaz("Kor-6"); return;
			case 227: makemaz("Kor-7"); return;
			case 228: makemaz("Kor-8"); return;
			case 229: makemaz("Kor-9"); return;
			case 230: makemaz("Kor-0"); return;
			case 231: makemaz("Lad-6"); return;
			case 232: makemaz("Lad-7"); return;
			case 233: makemaz("Lad-8"); return;
			case 234: makemaz("Lad-9"); return;
			case 235: makemaz("Lad-0"); return;
			case 236: makemaz("Lib-6"); return;
			case 237: makemaz("Lib-7"); return;
			case 238: makemaz("Lib-8"); return;
			case 239: makemaz("Lib-9"); return;
			case 240: makemaz("Lib-0"); return;
			case 241: makemaz("Loc-6"); return;
			case 242: makemaz("Loc-7"); return;
			case 243: makemaz("Loc-8"); return;
			case 244: makemaz("Loc-9"); return;
			case 245: makemaz("Loc-0"); return;
			case 246: makemaz("Lun-6"); return;
			case 247: makemaz("Lun-7"); return;
			case 248: makemaz("Lun-8"); return;
			case 249: makemaz("Lun-9"); return;
			case 250: makemaz("Lun-0"); return;
			case 251: makemaz("Mah-6"); return;
			case 252: makemaz("Mah-7"); return;
			case 253: makemaz("Mah-8"); return;
			case 254: makemaz("Mah-9"); return;
			case 255: makemaz("Mah-0"); return;
			case 256: makemaz("Mon-6"); return;
			case 257: makemaz("Mon-7"); return;
			case 258: makemaz("Mon-8"); return;
			case 259: makemaz("Mon-9"); return;
			case 260: makemaz("Mon-0"); return;
			case 261: makemaz("Mus-6"); return;
			case 262: makemaz("Mus-7"); return;
			case 263: makemaz("Mus-8"); return;
			case 264: makemaz("Mus-9"); return;
			case 265: makemaz("Mus-0"); return;
			case 266: makemaz("Mys-6"); return;
			case 267: makemaz("Mys-7"); return;
			case 268: makemaz("Mys-8"); return;
			case 269: makemaz("Mys-9"); return;
			case 270: makemaz("Mys-0"); return;
			case 271: makemaz("Nec-6"); return;
			case 272: makemaz("Nec-7"); return;
			case 273: makemaz("Nec-8"); return;
			case 274: makemaz("Nec-9"); return;
			case 275: makemaz("Nec-0"); return;
			case 276: makemaz("Nin-6"); return;
			case 277: makemaz("Nin-7"); return;
			case 278: makemaz("Nin-8"); return;
			case 279: makemaz("Nin-9"); return;
			case 280: makemaz("Nin-0"); return;
			case 281: makemaz("Nob-6"); return;
			case 282: makemaz("Nob-7"); return;
			case 283: makemaz("Nob-8"); return;
			case 284: makemaz("Nob-9"); return;
			case 285: makemaz("Nob-0"); return;
			case 286: makemaz("Off-6"); return;
			case 287: makemaz("Off-7"); return;
			case 288: makemaz("Off-8"); return;
			case 289: makemaz("Off-9"); return;
			case 290: makemaz("Off-0"); return;
			case 291: makemaz("Ord-6"); return;
			case 292: makemaz("Ord-7"); return;
			case 293: makemaz("Ord-8"); return;
			case 294: makemaz("Ord-9"); return;
			case 295: makemaz("Ord-0"); return;
			case 296: makemaz("Ota-6"); return;
			case 297: makemaz("Ota-7"); return;
			case 298: makemaz("Ota-8"); return;
			case 299: makemaz("Ota-9"); return;
			case 300: makemaz("Ota-0"); return;
			case 301: makemaz("Pal-6"); return;
			case 302: makemaz("Pal-7"); return;
			case 303: makemaz("Pal-8"); return;
			case 304: makemaz("Pal-9"); return;
			case 305: makemaz("Pal-0"); return;
			case 306: makemaz("Pic-6"); return;
			case 307: makemaz("Pic-7"); return;
			case 308: makemaz("Pic-8"); return;
			case 309: makemaz("Pic-9"); return;
			case 310: makemaz("Pic-0"); return;
			case 311: makemaz("Pir-6"); return;
			case 312: makemaz("Pir-7"); return;
			case 313: makemaz("Pir-8"); return;
			case 314: makemaz("Pir-9"); return;
			case 315: makemaz("Pir-0"); return;
			case 316: makemaz("Pok-6"); return;
			case 317: makemaz("Pok-7"); return;
			case 318: makemaz("Pok-8"); return;
			case 319: makemaz("Pok-9"); return;
			case 320: makemaz("Pok-0"); return;
			case 321: makemaz("Pol-6"); return;
			case 322: makemaz("Pol-7"); return;
			case 323: makemaz("Pol-8"); return;
			case 324: makemaz("Pol-9"); return;
			case 325: makemaz("Pol-0"); return;
			case 326: makemaz("Pri-6"); return;
			case 327: makemaz("Pri-7"); return;
			case 328: makemaz("Pri-8"); return;
			case 329: makemaz("Pri-9"); return;
			case 330: makemaz("Pri-0"); return;
			case 331: makemaz("Psi-6"); return;
			case 332: makemaz("Psi-7"); return;
			case 333: makemaz("Psi-8"); return;
			case 334: makemaz("Psi-9"); return;
			case 335: makemaz("Psi-0"); return;
			case 336: makemaz("Ran-6"); return;
			case 337: makemaz("Ran-7"); return;
			case 338: makemaz("Ran-8"); return;
			case 339: makemaz("Ran-9"); return;
			case 340: makemaz("Ran-0"); return;
			case 341: makemaz("Roc-6"); return;
			case 342: makemaz("Roc-7"); return;
			case 343: makemaz("Roc-8"); return;
			case 344: makemaz("Roc-9"); return;
			case 345: makemaz("Roc-0"); return;
			case 346: makemaz("Rog-6"); return;
			case 347: makemaz("Rog-7"); return;
			case 348: makemaz("Rog-8"); return;
			case 349: makemaz("Rog-9"); return;
			case 350: makemaz("Rog-0"); return;
			case 351: makemaz("Sag-6"); return;
			case 352: makemaz("Sag-7"); return;
			case 353: makemaz("Sag-8"); return;
			case 354: makemaz("Sag-9"); return;
			case 355: makemaz("Sag-0"); return;
			case 356: makemaz("Sai-6"); return;
			case 357: makemaz("Sai-7"); return;
			case 358: makemaz("Sai-8"); return;
			case 359: makemaz("Sai-9"); return;
			case 360: makemaz("Sai-0"); return;
			case 361: makemaz("Sam-6"); return;
			case 362: makemaz("Sam-7"); return;
			case 363: makemaz("Sam-8"); return;
			case 364: makemaz("Sam-9"); return;
			case 365: makemaz("Sam-0"); return;
			case 366: makemaz("Sci-6"); return;
			case 367: makemaz("Sci-7"); return;
			case 368: makemaz("Sci-8"); return;
			case 369: makemaz("Sci-9"); return;
			case 370: makemaz("Sci-0"); return;
			case 371: makemaz("Sla-6"); return;
			case 372: makemaz("Sla-7"); return;
			case 373: makemaz("Sla-8"); return;
			case 374: makemaz("Sla-9"); return;
			case 375: makemaz("Sla-0"); return;
			case 376: makemaz("Spa-6"); return;
			case 377: makemaz("Spa-7"); return;
			case 378: makemaz("Spa-8"); return;
			case 379: makemaz("Spa-9"); return;
			case 380: makemaz("Spa-0"); return;
			case 381: makemaz("Sup-6"); return;
			case 382: makemaz("Sup-7"); return;
			case 383: makemaz("Sup-8"); return;
			case 384: makemaz("Sup-9"); return;
			case 385: makemaz("Sup-0"); return;
			case 386: makemaz("Tha-6"); return;
			case 387: makemaz("Tha-7"); return;
			case 388: makemaz("Tha-8"); return;
			case 389: makemaz("Tha-9"); return;
			case 390: makemaz("Tha-0"); return;
			case 391: makemaz("Top-6"); return;
			case 392: makemaz("Top-7"); return;
			case 393: makemaz("Top-8"); return;
			case 394: makemaz("Top-9"); return;
			case 395: makemaz("Top-0"); return;
			case 396: makemaz("Tou-6"); return;
			case 397: makemaz("Tou-7"); return;
			case 398: makemaz("Tou-8"); return;
			case 399: makemaz("Tou-9"); return;
			case 400: makemaz("Tou-0"); return;
			case 401: makemaz("Tra-6"); return;
			case 402: makemaz("Tra-7"); return;
			case 403: makemaz("Tra-8"); return;
			case 404: makemaz("Tra-9"); return;
			case 405: makemaz("Tra-0"); return;
			case 406: makemaz("Und-6"); return;
			case 407: makemaz("Und-7"); return;
			case 408: makemaz("Und-8"); return;
			case 409: makemaz("Und-9"); return;
			case 410: makemaz("Und-0"); return;
			case 411: makemaz("Unt-6"); return;
			case 412: makemaz("Unt-7"); return;
			case 413: makemaz("Unt-8"); return;
			case 414: makemaz("Unt-9"); return;
			case 415: makemaz("Unt-0"); return;
			case 416: makemaz("Val-6"); return;
			case 417: makemaz("Val-7"); return;
			case 418: makemaz("Val-8"); return;
			case 419: makemaz("Val-9"); return;
			case 420: makemaz("Val-0"); return;
			case 421: makemaz("Wan-6"); return;
			case 422: makemaz("Wan-7"); return;
			case 423: makemaz("Wan-8"); return;
			case 424: makemaz("Wan-9"); return;
			case 425: makemaz("Wan-0"); return;
			case 426: makemaz("War-6"); return;
			case 427: makemaz("War-7"); return;
			case 428: makemaz("War-8"); return;
			case 429: makemaz("War-9"); return;
			case 430: makemaz("War-0"); return;
			case 431: makemaz("Wiz-6"); return;
			case 432: makemaz("Wiz-7"); return;
			case 433: makemaz("Wiz-8"); return;
			case 434: makemaz("Wiz-9"); return;
			case 435: makemaz("Wiz-0"); return;
			case 436: makemaz("Yeo-6"); return;
			case 437: makemaz("Yeo-7"); return;
			case 438: makemaz("Yeo-8"); return;
			case 439: makemaz("Yeo-9"); return;
			case 440: makemaz("Yeo-0"); return;
			case 441: makemaz("Zoo-6"); return;
			case 442: makemaz("Zoo-7"); return;
			case 443: makemaz("Zoo-8"); return;
			case 444: makemaz("Zoo-9"); return;
			case 445: makemaz("Zoo-0"); return;
			case 446: makemaz("Zyb-6"); return;
			case 447: makemaz("Zyb-7"); return;
			case 448: makemaz("Zyb-8"); return;
			case 449: makemaz("Zyb-9"); return;
			case 450: makemaz("Zyb-0"); return;
			case 451: makemaz("Ana-6"); return;
			case 452: makemaz("Ana-7"); return;
			case 453: makemaz("Ana-8"); return;
			case 454: makemaz("Ana-9"); return;
			case 455: makemaz("Ana-0"); return;
			case 456: makemaz("Cam-6"); return;
			case 457: makemaz("Cam-7"); return;
			case 458: makemaz("Cam-8"); return;
			case 459: makemaz("Cam-9"); return;
			case 460: makemaz("Cam-0"); return;
			case 461: makemaz("Mar-6"); return;
			case 462: makemaz("Mar-7"); return;
			case 463: makemaz("Mar-8"); return;
			case 464: makemaz("Mar-9"); return;
			case 465: makemaz("Mar-0"); return;
			case 466: makemaz("Sli-6"); return;
			case 467: makemaz("Sli-7"); return;
			case 468: makemaz("Sli-8"); return;
			case 469: makemaz("Sli-9"); return;
			case 470: makemaz("Sli-0"); return;
			case 471: makemaz("Drd-6"); return;
			case 472: makemaz("Drd-7"); return;
			case 473: makemaz("Drd-8"); return;
			case 474: makemaz("Drd-9"); return;
			case 475: makemaz("Drd-0"); return;
			case 476: makemaz("Erd-6"); return;
			case 477: makemaz("Erd-7"); return;
			case 478: makemaz("Erd-8"); return;
			case 479: makemaz("Erd-9"); return;
			case 480: makemaz("Erd-0"); return;
			case 481: makemaz("Fai-6"); return;
			case 482: makemaz("Fai-7"); return;
			case 483: makemaz("Fai-8"); return;
			case 484: makemaz("Fai-9"); return;
			case 485: makemaz("Fai-0"); return;
			case 486: makemaz("Fen-6"); return;
			case 487: makemaz("Fen-7"); return;
			case 488: makemaz("Fen-8"); return;
			case 489: makemaz("Fen-9"); return;
			case 490: makemaz("Fen-0"); return;
			case 491: makemaz("Fig-6"); return;
			case 492: makemaz("Fig-7"); return;
			case 493: makemaz("Fig-8"); return;
			case 494: makemaz("Fig-9"); return;
			case 495: makemaz("Fig-0"); return;
			case 496: makemaz("Gol-6"); return;
			case 497: makemaz("Gol-7"); return;
			case 498: makemaz("Gol-8"); return;
			case 499: makemaz("Gol-9"); return;
			case 500: makemaz("Gol-0"); return;
			case 501: makemaz("Jus-6"); return;
			case 502: makemaz("Jus-7"); return;
			case 503: makemaz("Jus-8"); return;
			case 504: makemaz("Jus-9"); return;
			case 505: makemaz("Jus-0"); return;
			case 506: makemaz("Med-6"); return;
			case 507: makemaz("Med-7"); return;
			case 508: makemaz("Med-8"); return;
			case 509: makemaz("Med-9"); return;
			case 510: makemaz("Med-0"); return;
			case 511: makemaz("Mid-6"); return;
			case 512: makemaz("Mid-7"); return;
			case 513: makemaz("Mid-8"); return;
			case 514: makemaz("Mid-9"); return;
			case 515: makemaz("Mid-0"); return;
			case 516: makemaz("Mur-6"); return;
			case 517: makemaz("Mur-7"); return;
			case 518: makemaz("Mur-8"); return;
			case 519: makemaz("Mur-9"); return;
			case 520: makemaz("Mur-0"); return;
			case 521: makemaz("Poi-6"); return;
			case 522: makemaz("Poi-7"); return;
			case 523: makemaz("Poi-8"); return;
			case 524: makemaz("Poi-9"); return;
			case 525: makemaz("Poi-0"); return;
			case 526: makemaz("Rin-6"); return;
			case 527: makemaz("Rin-7"); return;
			case 528: makemaz("Rin-8"); return;
			case 529: makemaz("Rin-9"); return;
			case 530: makemaz("Rin-0"); return;
			case 531: makemaz("Sha-6"); return;
			case 532: makemaz("Sha-7"); return;
			case 533: makemaz("Sha-8"); return;
			case 534: makemaz("Sha-9"); return;
			case 535: makemaz("Sha-0"); return;
			case 536: makemaz("Twe-6"); return;
			case 537: makemaz("Twe-7"); return;
			case 538: makemaz("Twe-8"); return;
			case 539: makemaz("Twe-9"); return;
			case 540: makemaz("Twe-0"); return;
			case 541: makemaz("Use-6"); return;
			case 542: makemaz("Use-7"); return;
			case 543: makemaz("Use-8"); return;
			case 544: makemaz("Use-9"); return;
			case 545: makemaz("Use-0"); return;
			case 546: makemaz("Sex-6"); return;
			case 547: makemaz("Sex-7"); return;
			case 548: makemaz("Sex-8"); return;
			case 549: makemaz("Sex-9"); return;
			case 550: makemaz("Sex-0"); return;
			case 551: makemaz("Unb-6"); return;
			case 552: makemaz("Unb-7"); return;
			case 553: makemaz("Unb-8"); return;
			case 554: makemaz("Unb-9"); return;
			case 555: makemaz("Unb-0"); return;
			case 556: makemaz("Trs-6"); return;
			case 557: makemaz("Trs-7"); return;
			case 558: makemaz("Trs-8"); return;
			case 559: makemaz("Trs-9"); return;
			case 560: makemaz("Trs-0"); return;
			case 561: makemaz("Cha-6"); return;
			case 562: makemaz("Cha-7"); return;
			case 563: makemaz("Cha-8"); return;
			case 564: makemaz("Cha-9"); return;
			case 565: makemaz("Cha-0"); return;
			case 566: makemaz("Elm-6"); return;
			case 567: makemaz("Elm-7"); return;
			case 568: makemaz("Elm-8"); return;
			case 569: makemaz("Elm-9"); return;
			case 570: makemaz("Elm-0"); return;
			case 571: makemaz("Wil-6"); return;
			case 572: makemaz("Wil-7"); return;
			case 573: makemaz("Wil-8"); return;
			case 574: makemaz("Wil-9"); return;
			case 575: makemaz("Wil-0"); return;
			case 576: makemaz("Occ-6"); return;
			case 577: makemaz("Occ-7"); return;
			case 578: makemaz("Occ-8"); return;
			case 579: makemaz("Occ-9"); return;
			case 580: makemaz("Occ-0"); return;
			case 581: makemaz("Pro-6"); return;
			case 582: makemaz("Pro-7"); return;
			case 583: makemaz("Pro-8"); return;
			case 584: makemaz("Pro-9"); return;
			case 585: makemaz("Pro-0"); return;
			case 586: makemaz("Kur-6"); return;
			case 587: makemaz("Kur-7"); return;
			case 588: makemaz("Kur-8"); return;
			case 589: makemaz("Kur-9"); return;
			case 590: makemaz("Kur-0"); return;
			case 591: makemaz("For-6"); return;
			case 592: makemaz("For-7"); return;
			case 593: makemaz("For-8"); return;
			case 594: makemaz("For-9"); return;
			case 595: makemaz("For-0"); return;
			case 596: makemaz("Trc-6"); return;
			case 597: makemaz("Trc-7"); return;
			case 598: makemaz("Trc-8"); return;
			case 599: makemaz("Trc-9"); return;
			case 600: makemaz("Trc-0"); return;
			case 601: makemaz("Nuc-6"); return;
			case 602: makemaz("Nuc-7"); return;
			case 603: makemaz("Nuc-8"); return;
			case 604: makemaz("Nuc-9"); return;
			case 605: makemaz("Nuc-0"); return;
			case 606: makemaz("Sco-6"); return;
			case 607: makemaz("Sco-7"); return;
			case 608: makemaz("Sco-8"); return;
			case 609: makemaz("Sco-9"); return;
			case 610: makemaz("Sco-0"); return;
			case 611: makemaz("Fem-6"); return;
			case 612: makemaz("Fem-7"); return;
			case 613: makemaz("Fem-8"); return;
			case 614: makemaz("Fem-9"); return;
			case 615: makemaz("Fem-0"); return;
			case 616: makemaz("Hus-6"); return;
			case 617: makemaz("Hus-7"); return;
			case 618: makemaz("Hus-8"); return;
			case 619: makemaz("Hus-9"); return;
			case 620: makemaz("Hus-0"); return;
			case 621: makemaz("Acu-6"); return;
			case 622: makemaz("Acu-7"); return;
			case 623: makemaz("Acu-8"); return;
			case 624: makemaz("Acu-9"); return;
			case 625: makemaz("Acu-0"); return;
			case 626: makemaz("Mas-6"); return;
			case 627: makemaz("Mas-7"); return;
			case 628: makemaz("Mas-8"); return;
			case 629: makemaz("Mas-9"); return;
			case 630: makemaz("Mas-0"); return;
			case 631: makemaz("Gre-6"); return;
			case 632: makemaz("Gre-7"); return;
			case 633: makemaz("Gre-8"); return;
			case 634: makemaz("Gre-9"); return;
			case 635: makemaz("Gre-0"); return;
			case 636: makemaz("Cel-6"); return;
			case 637: makemaz("Cel-7"); return;
			case 638: makemaz("Cel-8"); return;
			case 639: makemaz("Cel-9"); return;
			case 640: makemaz("Cel-0"); return;
			case 641: makemaz("Wal-6"); return;
			case 642: makemaz("Wal-7"); return;
			case 643: makemaz("Wal-8"); return;
			case 644: makemaz("Wal-9"); return;
			case 645: makemaz("Wal-0"); return;
			case 646: makemaz("Soc-6"); return;
			case 647: makemaz("Soc-7"); return;
			case 648: makemaz("Soc-8"); return;
			case 649: makemaz("Soc-9"); return;
			case 650: makemaz("Soc-0"); return;
			case 651: makemaz("Dem-6"); return;
			case 652: makemaz("Dem-7"); return;
			case 653: makemaz("Dem-8"); return;
			case 654: makemaz("Dem-9"); return;
			case 655: makemaz("Dem-0"); return;
			case 656: makemaz("Dis-6"); return;
			case 657: makemaz("Dis-7"); return;
			case 658: makemaz("Dis-8"); return;
			case 659: makemaz("Dis-9"); return;
			case 660: makemaz("Dis-0"); return;
			case 661: makemaz("Sto-6"); return;
			case 662: makemaz("Sto-7"); return;
			case 663: makemaz("Sto-8"); return;
			case 664: makemaz("Sto-9"); return;
			case 665: makemaz("Sto-0"); return;
			case 666: makemaz("Mam-6"); return;
			case 667: makemaz("Mam-7"); return;
			case 668: makemaz("Mam-8"); return;
			case 669: makemaz("Mam-9"); return;
			case 670: makemaz("Mam-0"); return;
			case 671: makemaz("Jan-6"); return;
			case 672: makemaz("Jan-7"); return;
			case 673: makemaz("Jan-8"); return;
			case 674: makemaz("Jan-9"); return;
			case 675: makemaz("Jan-0"); return;
			case 676: makemaz("Emp-6"); return;
			case 677: makemaz("Emp-7"); return;
			case 678: makemaz("Emp-8"); return;
			case 679: makemaz("Emp-9"); return;
			case 680: makemaz("Emp-0"); return;
			case 681: makemaz("Psy-6"); return;
			case 682: makemaz("Psy-7"); return;
			case 683: makemaz("Psy-8"); return;
			case 684: makemaz("Psy-9"); return;
			case 685: makemaz("Psy-0"); return;
			case 686: makemaz("Qua-6"); return;
			case 687: makemaz("Qua-7"); return;
			case 688: makemaz("Qua-8"); return;
			case 689: makemaz("Qua-9"); return;
			case 690: makemaz("Qua-0"); return;
			case 691: makemaz("Cra-6"); return;
			case 692: makemaz("Cra-7"); return;
			case 693: makemaz("Cra-8"); return;
			case 694: makemaz("Cra-9"); return;
			case 695: makemaz("Cra-0"); return;
			case 696: makemaz("Wei-6"); return;
			case 697: makemaz("Wei-7"); return;
			case 698: makemaz("Wei-8"); return;
			case 699: makemaz("Wei-9"); return;
			case 700: makemaz("Wei-0"); return;
			case 701: makemaz("Xel-6"); return;
			case 702: makemaz("Xel-7"); return;
			case 703: makemaz("Xel-8"); return;
			case 704: makemaz("Xel-9"); return;
			case 705: makemaz("Xel-0"); return;
			case 706: makemaz("Yau-6"); return;
			case 707: makemaz("Yau-7"); return;
			case 708: makemaz("Yau-8"); return;
			case 709: makemaz("Yau-9"); return;
			case 710: makemaz("Yau-0"); return;
			case 711: makemaz("Sof-6"); return;
			case 712: makemaz("Sof-7"); return;
			case 713: makemaz("Sof-8"); return;
			case 714: makemaz("Sof-9"); return;
			case 715: makemaz("Sof-0"); return;
			case 716: makemaz("Ast-6"); return;
			case 717: makemaz("Ast-7"); return;
			case 718: makemaz("Ast-8"); return;
			case 719: makemaz("Ast-9"); return;
			case 720: makemaz("Ast-0"); return;
			case 721: makemaz("Sma-6"); return;
			case 722: makemaz("Sma-7"); return;
			case 723: makemaz("Sma-8"); return;
			case 724: makemaz("Sma-9"); return;
			case 725: makemaz("Sma-0"); return;
			case 726: makemaz("Cyb-6"); return;
			case 727: makemaz("Cyb-7"); return;
			case 728: makemaz("Cyb-8"); return;
			case 729: makemaz("Cyb-9"); return;
			case 730: makemaz("Cyb-0"); return;
			case 731: makemaz("Tos-6"); return;
			case 732: makemaz("Tos-7"); return;
			case 733: makemaz("Tos-8"); return;
			case 734: makemaz("Tos-9"); return;
			case 735: makemaz("Tos-0"); return;
			case 736: makemaz("Sym-6"); return;
			case 737: makemaz("Sym-7"); return;
			case 738: makemaz("Sym-8"); return;
			case 739: makemaz("Sym-9"); return;
			case 740: makemaz("Sym-0"); return;
			case 741: makemaz("Pra-6"); return;
			case 742: makemaz("Pra-7"); return;
			case 743: makemaz("Pra-8"); return;
			case 744: makemaz("Pra-9"); return;
			case 745: makemaz("Pra-0"); return;
			case 746: makemaz("Mil-6"); return;
			case 747: makemaz("Mil-7"); return;
			case 748: makemaz("Mil-8"); return;
			case 749: makemaz("Mil-9"); return;
			case 750: makemaz("Mil-0"); return;
			case 751: makemaz("Gen-6"); return;
			case 752: makemaz("Gen-7"); return;
			case 753: makemaz("Gen-8"); return;
			case 754: makemaz("Gen-9"); return;
			case 755: makemaz("Gen-0"); return;
			case 756: makemaz("Fjo-6"); return;
			case 757: makemaz("Fjo-7"); return;
			case 758: makemaz("Fjo-8"); return;
			case 759: makemaz("Fjo-9"); return;
			case 760: makemaz("Fjo-0"); return;
			case 761: makemaz("Eme-6"); return;
			case 762: makemaz("Eme-7"); return;
			case 763: makemaz("Eme-8"); return;
			case 764: makemaz("Eme-9"); return;
			case 765: makemaz("Eme-0"); return;
			case 766: makemaz("Com-6"); return;
			case 767: makemaz("Com-7"); return;
			case 768: makemaz("Com-8"); return;
			case 769: makemaz("Com-9"); return;
			case 770: makemaz("Com-0"); return;
			case 771: makemaz("Akl-6"); return;
			case 772: makemaz("Akl-7"); return;
			case 773: makemaz("Akl-8"); return;
			case 774: makemaz("Akl-9"); return;
			case 775: makemaz("Akl-0"); return;
			case 776: makemaz("Dra-6"); return;
			case 777: makemaz("Dra-7"); return;
			case 778: makemaz("Dra-8"); return;
			case 779: makemaz("Dra-9"); return;
			case 780: makemaz("Dra-0"); return;
			case 781: makemaz("Car-6"); return;
			case 782: makemaz("Car-7"); return;
			case 783: makemaz("Car-8"); return;
			case 784: makemaz("Car-9"); return;
			case 785: makemaz("Car-0"); return;
			case 786: makemaz("But-6"); return;
			case 787: makemaz("But-7"); return;
			case 788: makemaz("But-8"); return;
			case 789: makemaz("But-9"); return;
			case 790: makemaz("But-0"); return;
			case 791: makemaz("Dan-6"); return;
			case 792: makemaz("Dan-7"); return;
			case 793: makemaz("Dan-8"); return;
			case 794: makemaz("Dan-9"); return;
			case 795: makemaz("Dan-0"); return;
			case 796: makemaz("Dia-6"); return;
			case 797: makemaz("Dia-7"); return;
			case 798: makemaz("Dia-8"); return;
			case 799: makemaz("Dia-9"); return;
			case 800: makemaz("Dia-0"); return;
			case 801: makemaz("Pre-6"); return;
			case 802: makemaz("Pre-7"); return;
			case 803: makemaz("Pre-8"); return;
			case 804: makemaz("Pre-9"); return;
			case 805: makemaz("Pre-0"); return;
			case 806: makemaz("Sec-6"); return;
			case 807: makemaz("Sec-7"); return;
			case 808: makemaz("Sec-8"); return;
			case 809: makemaz("Sec-9"); return;
			case 810: makemaz("Sec-0"); return;
			case 811: makemaz("Sho-6"); return;
			case 812: makemaz("Sho-7"); return;
			case 813: makemaz("Sho-8"); return;
			case 814: makemaz("Sho-9"); return;
			case 815: makemaz("Sho-0"); return;
			case 816: makemaz("Hal-6"); return;
			case 817: makemaz("Hal-7"); return;
			case 818: makemaz("Hal-8"); return;
			case 819: makemaz("Hal-9"); return;
			case 820: makemaz("Hal-0"); return;
			case 821: makemaz("Sin-6"); return;
			case 822: makemaz("Sin-7"); return;
			case 823: makemaz("Sin-8"); return;
			case 824: makemaz("Sin-9"); return;
			case 825: makemaz("Sin-0"); return;
			case 826: makemaz("Hed-6"); return;
			case 827: makemaz("Hed-7"); return;
			case 828: makemaz("Hed-8"); return;
			case 829: makemaz("Hed-9"); return;
			case 830: makemaz("Hed-0"); return;
			case 831: makemaz("Van-6"); return;
			case 832: makemaz("Van-7"); return;
			case 833: makemaz("Van-8"); return;
			case 834: makemaz("Van-9"); return;
			case 835: makemaz("Van-0"); return;
			case 836: makemaz("Sdw-6"); return;
			case 837: makemaz("Sdw-7"); return;
			case 838: makemaz("Sdw-8"); return;
			case 839: makemaz("Sdw-9"); return;
			case 840: makemaz("Sdw-0"); return;
			case 841: makemaz("Cli-6"); return;
			case 842: makemaz("Cli-7"); return;
			case 843: makemaz("Cli-8"); return;
			case 844: makemaz("Cli-9"); return;
			case 845: makemaz("Cli-0"); return;
			case 846: makemaz("Wom-6"); return;
			case 847: makemaz("Wom-7"); return;
			case 848: makemaz("Wom-8"); return;
			case 849: makemaz("Wom-9"); return;
			case 850: makemaz("Wom-0"); return;
			case 851: makemaz("Noo-6"); return;
			case 852: makemaz("Noo-7"); return;
			case 853: makemaz("Noo-8"); return;
			case 854: makemaz("Noo-9"); return;
			case 855: makemaz("Noo-0"); return;
		}
		break;

		case 94:
		case 95:
		case 96:
		case 97:
		case 98:
		case 99:
		case 100:

		switch (rnd(69)) {

			case 1: makemaz("mazes-1"); return;
			case 2: makemaz("mazes-2"); return;
			case 3: makemaz("mazes-3"); return;
			case 4: makemaz("mazes-4"); return;
			case 5: makemaz("mazes-5"); return;
			case 6: makemaz("mazes-6"); return;
			case 7: makemaz("mazes-7"); return;
			case 8: makemaz("mazes-8"); return;
			case 9: makemaz("mazes-9"); return;
			case 10: makemaz("mazes-10"); return;
			case 11: makemaz("mazes-11"); return;
			case 12: makemaz("mazes-12"); return;
			case 13: makemaz("mazes-13"); return;
			case 14: makemaz("mazes-14"); return;
			case 15: makemaz("mazes-15"); return;
			case 16: makemaz("mazes-16"); return;
			case 17: makemaz("mazes-17"); return;
			case 18: makemaz("mazes-18"); return;
			case 19: makemaz("mazes-19"); return;
			case 20: makemaz("mazes-20"); return;
			case 21: makemaz("mazes-21"); return;
			case 22: makemaz("mazes-22"); return;
			case 23: makemaz("mazes-23"); return;
			case 24: makemaz("mazes-24"); return;
			case 25: makemaz("mazes-25"); return;
			case 26: makemaz("mazes-26"); return;
			case 27: makemaz("mazes-27"); return;
			case 28: makemaz("mazes-28"); return;
			case 29: makemaz("mazes-29"); return;
			case 30: makemaz("mazes-30"); return;
			case 31: makemaz("mazes-31"); return;
			case 32: makemaz("mazes-32"); return;
			case 33: makemaz("mazes-33"); return;
			case 34: makemaz("mazes-34"); return;
			case 35: makemaz("mazes-35"); return;
			case 36: makemaz("mazes-36"); return;
			case 37: makemaz("mazes-37"); return;
			case 38: makemaz("mazes-38"); return;
			case 39: makemaz("mazes-39"); return;
			case 40: makemaz("mazes-40"); return;
			case 41: makemaz("mazes-41"); return;
			case 42: makemaz("mazes-42"); return;
			case 43: makemaz("mazes-43"); return;
			case 44: makemaz("mazes-44"); return;
			case 45: makemaz("mazes-45"); return;
			case 46: makemaz("mazes-46"); return;
			case 47: makemaz("mazes-47"); return;
			case 48: makemaz("mazes-48"); return;
			case 49: makemaz("mazes-49"); return;
			case 50: makemaz("mazes-50"); return;
			case 51: makemaz("mazes-51"); return;
			case 52: makemaz("mazes-52"); return;
			case 53: makemaz("mazes-53"); return;
			case 54: makemaz("mazes-54"); return;
			case 55: makemaz("mazes-55"); return;
			case 56: makemaz("mazes-56"); return;
			case 57: makemaz("mazes-57"); return;
			case 58: makemaz("mazes-58"); return;
			case 59: makemaz("mazes-59"); return;
			case 60: makemaz("mazes-60"); return;
			case 61: makemaz("mazes-61"); return;
			case 62: makemaz("mazes-62"); return;
			case 63: makemaz("mazes-63"); return;
			case 64: makemaz("mazes-64"); return;
			case 65: makemaz("mazes-65"); return;
			case 66: makemaz("mazes-66"); return;
			case 67: makemaz("mazes-67"); return;
			case 68: makemaz("mazes-68"); return;
			case 69: makemaz("mazes-69"); return;

		}
		break;

		case 101:
		case 102:
		case 103:
		case 104:
		case 105:

		switch (rnd(95)) {

			case 1: makemaz("levgx-1"); return;
			case 2: makemaz("levgx-2"); return;
			case 3: makemaz("levgx-3"); return;
			case 4: makemaz("levgx-4"); return;
			case 5: makemaz("levgx-5"); return;
			case 6: makemaz("levgx-6"); return;
			case 7: makemaz("levgx-7"); return;
			case 8: makemaz("levgx-8"); return;
			case 9: makemaz("levgx-9"); return;
			case 10: makemaz("levgx-10"); return;
			case 11: makemaz("levgx-11"); return;
			case 12: makemaz("levgx-12"); return;
			case 13: makemaz("levgx-13"); return;
			case 14: makemaz("levgx-14"); return;
			case 15: makemaz("levgx-15"); return;
			case 16: makemaz("levgx-16"); return;
			case 17: makemaz("levgx-17"); return;
			case 18: makemaz("levgx-18"); return;
			case 19: makemaz("levgx-19"); return;
			case 20: makemaz("levgx-20"); return;
			case 21: makemaz("levgx-21"); return;
			case 22: makemaz("levgx-22"); return;
			case 23: makemaz("levgx-23"); return;
			case 24: makemaz("levgx-24"); return;
			case 25: makemaz("levgx-25"); return;
			case 26: makemaz("levgx-26"); return;
			case 27: makemaz("levgx-27"); return;
			case 28: makemaz("levgx-28"); return;
			case 29: makemaz("levgx-29"); return;
			case 30: makemaz("levgx-30"); return;
			case 31: makemaz("levgx-31"); return;
			case 32: makemaz("levgx-32"); return;
			case 33: makemaz("levgx-33"); return;
			case 34: makemaz("levgx-34"); return;
			case 35: makemaz("levgx-35"); return;
			case 36: makemaz("levgx-36"); return;
			case 37: makemaz("levgx-37"); return;
			case 38: makemaz("levgx-38"); return;
			case 39: makemaz("levgx-39"); return;
			case 40: makemaz("levgx-40"); return;
			case 41: makemaz("levgx-41"); return;
			case 42: makemaz("levgx-42"); return;
			case 43: makemaz("levgx-43"); return;
			case 44: makemaz("levgx-44"); return;
			case 45: makemaz("levgx-45"); return;
			case 46: makemaz("levgx-46"); return;
			case 47: makemaz("levgx-47"); return;
			case 48: makemaz("levgx-48"); return;
			case 49: makemaz("levgx-49"); return;
			case 50: makemaz("levgx-50"); return;
			case 51: makemaz("levgx-51"); return;
			case 52: makemaz("levgx-52"); return;
			case 53: makemaz("levgx-53"); return;
			case 54: makemaz("levgx-54"); return;
			case 55: makemaz("levgx-55"); return;
			case 56: makemaz("levgx-56"); return;
			case 57: makemaz("levgx-57"); return;
			case 58: makemaz("levgx-58"); return;
			case 59: makemaz("levgx-59"); return;
			case 60: makemaz("levgx-60"); return;
			case 61: makemaz("levgx-61"); return;
			case 62: makemaz("levgx-62"); return;
			case 63: makemaz("levgx-63"); return;
			case 64: makemaz("levgx-64"); return;
			case 65: makemaz("levgx-65"); return;
			case 66: makemaz("levgx-66"); return;
			case 67: makemaz("levgx-67"); return;
			case 68: makemaz("levgx-68"); return;
			case 69: makemaz("levgx-69"); return;
			case 70: makemaz("levgx-70"); return;
			case 71: makemaz("levgx-71"); return;
			case 72: makemaz("levgx-72"); return;
			case 73: makemaz("levgx-73"); return;
			case 74: makemaz("levgx-74"); return;
			case 75: makemaz("levgx-75"); return;
			case 76: makemaz("levgx-76"); return;
			case 77: makemaz("levgx-77"); return;
			case 78: makemaz("levgx-78"); return;
			case 79: makemaz("levgx-79"); return;
			case 80: makemaz("levgx-80"); return;
			case 81: makemaz("levgx-81"); return;
			case 82: makemaz("levgx-82"); return;
			case 83: makemaz("levgx-83"); return;
			case 84: makemaz("levgx-84"); return;
			case 85: makemaz("levgx-85"); return;
			case 86: makemaz("levgx-86"); return;
			case 87: makemaz("levgx-87"); return;
			case 88: makemaz("levgx-88"); return;
			case 89: makemaz("levgx-89"); return;
			case 90: makemaz("levgx-90"); return;
			case 91: makemaz("levgx-91"); return;
			case 92: makemaz("levgx-92"); return;
			case 93: makemaz("levgx-93"); return;
			case 94: makemaz("levgx-94"); return;
			case 95: makemaz("levgx-95"); return;
		}
	    case 106:

		makemaz("hcav2"); return;
		break;

	    case 107:

		makemaz("hfrnk"); return;
		break;

	    case 108:
	    case 109:
	    case 110:

		switch (rnd(8)) {

			case 1: {
				switch (rnd(5)) {

					case 1: makemaz("cowla-1"); return;
					case 2: makemaz("cowla-2"); return;
					case 3: makemaz("cowla-3"); return;
					case 4: makemaz("cowla-4"); return;
					case 5: makemaz("cowla-5"); return;

				}
				break;
			}
			case 2: makemaz("cowlb"); return;
			case 3: makemaz("cowlc"); return;
			case 4: makemaz("cowld"); return;
			case 5: makemaz("cowle"); return;
			case 6: makemaz("cowlf"); return;
			case 7: makemaz("cowlg"); return;
			case 8: makemaz("cowlh"); return;

		}

		break;

	    case 111:
		switch (rnd(15)) {
			case 1: makemaz("deehfila"); return;
			case 2: makemaz("deehfilb"); return;
			case 3: makemaz("deehfilc"); return;
			case 4: makemaz("deehfild"); return;
			case 5: makemaz("deehfile"); return;
			case 6: makemaz("deehfilf"); return;
			case 7: makemaz("deehfilg"); return;
			case 8: makemaz("deehfilh"); return;
			case 9: makemaz("deehfili"); return;
			case 10: makemaz("deehfilj"); return;
			case 11: makemaz("deehfilk"); return;
			case 12: makemaz("deehfill"); return;
			case 13: makemaz("deehfilm"); return;
			case 14: makemaz("deehfiln"); return;
			case 15: makemaz("deehfilo"); return;

		}

		break;

	    case 112:
		switch (rnd(10)) {
			case 1: makemaz("deptX-1"); return;
			case 2: makemaz("deptX-2"); return;
			case 3: makemaz("deptX-3"); return;
			case 4: makemaz("deptX-4"); return;
			case 5: makemaz("deptX-5"); return;
			case 6: makemaz("deptX-6"); return;
			case 7: makemaz("deptX-7"); return;
			case 8: makemaz("deptX-8"); return;
			case 9: makemaz("deptX-9"); return;
			case 10: makemaz("deptX-10"); return;

		}

		break;

	    case 113:
		switch (rnd(6)) {
			case 1: makemaz("depeX-1"); return;
			case 2: makemaz("depeX-2"); return;
			case 3: makemaz("depeX-3"); return;
			case 4: makemaz("depeX-4"); return;
			case 5: makemaz("depeX-5"); return;
			case 6: makemaz("depeX-6"); return;

		}

		break;

	    case 114:
		switch (rnd(5)) {
			case 1: makemaz("gruelaiX"); return;
			case 2: makemaz("joustX"); return;
			case 3: makemaz("pmazeX"); return;
			case 4: makemaz("poolhalX"); return;
			case 5: makemaz("dmazeX"); return;

		}

		break;

	    case 115:
	    case 116:

		switch (rnd(8)) {

			case 1: makemaz("grcra"); return;
			case 2: makemaz("grcrb"); return;
			case 3: makemaz("grcrc"); return;
			case 4: makemaz("grcrd"); return;
			case 5: makemaz("grcre"); return;
			case 6: makemaz("grcrf"); return;
			case 7: makemaz("grcrg"); return;
			case 8: makemaz("grcrh"); return;

		}
		break;

	    case 117:
		switch (rnd(3)) {
			case 1: makemaz("hiceqa"); return;
			case 2: makemaz("hiceqb"); return;
			case 3: makemaz("hiceqc"); return;

		}


		break;

          }

	}

	/* mazewalker only gets mazes (evil patch idea by jonadab) */
	/* special race flag: 1 = mazewalker, 2 = sokosolver, 3 = specialist */

	if (ismazewalker && issokosolver && isspecialist) specialraceflag = rnd(3);
	else if (ismazewalker && issokosolver) specialraceflag = rn2(2) ? 1 : 2;
	else if (ismazewalker && isspecialist) specialraceflag = rn2(2) ? 1 : 3;
	else if (issokosolver && isspecialist) specialraceflag = rn2(2) ? 2 : 3;
	else if (ismazewalker) specialraceflag = 1;
	else if (issokosolver) specialraceflag = 2;
	else if (isspecialist) specialraceflag = 3;
	else specialraceflag = 0; /* fail safe */

	if ((specialraceflag == 1) && (!rn2(100) || depth(&u.uz) > 1) ) { /* mazewalker */

	    if (rn2(3)) {

		switch (rnd(69)) {

		case 1: makemaz("mazes-1"); return;
		case 2: makemaz("mazes-2"); return;
		case 3: makemaz("mazes-3"); return;
		case 4: makemaz("mazes-4"); return;
		case 5: makemaz("mazes-5"); return;
		case 6: makemaz("mazes-6"); return;
		case 7: makemaz("mazes-7"); return;
		case 8: makemaz("mazes-8"); return;
		case 9: makemaz("mazes-9"); return;
		case 10: makemaz("mazes-10"); return;
		case 11: makemaz("mazes-11"); return;
		case 12: makemaz("mazes-12"); return;
		case 13: makemaz("mazes-13"); return;
		case 14: makemaz("mazes-14"); return;
		case 15: makemaz("mazes-15"); return;
		case 16: makemaz("mazes-16"); return;
		case 17: makemaz("mazes-17"); return;
		case 18: makemaz("mazes-18"); return;
		case 19: makemaz("mazes-19"); return;
		case 20: makemaz("mazes-20"); return;
		case 21: makemaz("mazes-21"); return;
		case 22: makemaz("mazes-22"); return;
		case 23: makemaz("mazes-23"); return;
		case 24: makemaz("mazes-24"); return;
		case 25: makemaz("mazes-25"); return;
		case 26: makemaz("mazes-26"); return;
		case 27: makemaz("mazes-27"); return;
		case 28: makemaz("mazes-28"); return;
		case 29: makemaz("mazes-29"); return;
		case 30: makemaz("mazes-30"); return;
		case 31: makemaz("mazes-31"); return;
		case 32: makemaz("mazes-32"); return;
		case 33: makemaz("mazes-33"); return;
		case 34: makemaz("mazes-34"); return;
		case 35: makemaz("mazes-35"); return;
		case 36: makemaz("mazes-36"); return;
		case 37: makemaz("mazes-37"); return;
		case 38: makemaz("mazes-38"); return;
		case 39: makemaz("mazes-39"); return;
		case 40: makemaz("mazes-40"); return;
		case 41: makemaz("mazes-41"); return;
		case 42: makemaz("mazes-42"); return;
		case 43: makemaz("mazes-43"); return;
		case 44: makemaz("mazes-44"); return;
		case 45: makemaz("mazes-45"); return;
		case 46: makemaz("mazes-46"); return;
		case 47: makemaz("mazes-47"); return;
		case 48: makemaz("mazes-48"); return;
		case 49: makemaz("mazes-49"); return;
		case 50: makemaz("mazes-50"); return;
		case 51: makemaz("mazes-51"); return;
		case 52: makemaz("mazes-52"); return;
		case 53: makemaz("mazes-53"); return;
		case 54: makemaz("mazes-54"); return;
		case 55: makemaz("mazes-55"); return;
		case 56: makemaz("mazes-56"); return;
		case 57: makemaz("mazes-57"); return;
		case 58: makemaz("mazes-58"); return;
		case 59: makemaz("mazes-59"); return;
		case 60: makemaz("mazes-60"); return;
		case 61: makemaz("mazes-61"); return;
		case 62: makemaz("mazes-62"); return;
		case 63: makemaz("mazes-63"); return;
		case 64: makemaz("mazes-64"); return;
		case 65: makemaz("mazes-65"); return;
		case 66: makemaz("mazes-66"); return;
		case 67: makemaz("mazes-67"); return;
		case 68: makemaz("mazes-68"); return;
		case 69: makemaz("mazes-69"); return;

		}

	    } else makemaz("");
	    return;

	}

	if ((specialraceflag == 2) && (!rn2(100) || depth(&u.uz) > 1) ) { /* sokosolver */

		if (In_dod(&u.uz) || In_mines(&u.uz) || In_sokoban(&u.uz) || In_towndungeon(&u.uz) || In_illusorycastle(&u.uz) || In_deepmines(&u.uz) || In_ZAPM(&u.uz) || In_Devnull(&u.uz) || In_greencross(&u.uz) || !strcmp(dungeons[u.uz.dnum].dname, "Grund's Stronghold") || !strcmp(dungeons[u.uz.dnum].dname, "The Ice Queen's Realm") || !strcmp(dungeons[u.uz.dnum].dname, "The Temple of Moloch") || !strcmp(dungeons[u.uz.dnum].dname, "The Giant Caverns") || !strcmp(dungeons[u.uz.dnum].dname, "The Sunless Sea") || !strcmp(dungeons[u.uz.dnum].dname, "The Spider Caves") || !strcmp(dungeons[u.uz.dnum].dname, "The Lost Tomb") || !strcmp(dungeons[u.uz.dnum].dname, "Yendorian Tower") || !strcmp(dungeons[u.uz.dnum].dname, "Bell Caves") || !strcmp(dungeons[u.uz.dnum].dname, "Forging Chamber") || !strcmp(dungeons[u.uz.dnum].dname, "Dead Grounds") || !strcmp(dungeons[u.uz.dnum].dname, "Ordered Chaos") || !strcmp(dungeons[u.uz.dnum].dname, "The Wyrm Caves") || !strcmp(dungeons[u.uz.dnum].dname, "One-eyed Sam's Market") || !strcmp(dungeons[u.uz.dnum].dname, "Fort Ludios") || In_restingzone(&u.uz) ) {

ghnhom3:
		switch (rnd(154)) {

			case 1: makemaz("soko2-1"); return;
			case 2: makemaz("soko2-2"); return;
			case 3: makemaz("soko2-3"); return;
			case 4: makemaz("soko2-4"); return;
			case 5: makemaz("soko2-5"); return;
			case 6: makemaz("soko2-6"); return;
			case 7: makemaz("soko2-7"); return;
			case 8: makemaz("soko2-8"); return;
			case 9: makemaz("soko2-9"); return;
			case 10: makemaz("soko2-10"); return;
			case 11: makemaz("soko2-11"); return;
			case 12: makemaz("soko2-12"); return;
			case 13: makemaz("soko2-13"); return;
			case 14: makemaz("soko2-14"); return;
			case 15: makemaz("soko2-15"); return;
			case 16: makemaz("soko2-16"); return;
			case 17: makemaz("soko2-17"); return;
			case 18: makemaz("soko2-18"); return;
			case 19: makemaz("soko2-19"); return;
			case 20: makemaz("soko2-20"); return;
			case 21: makemaz("soko2-21"); return;
			case 22: makemaz("soko2-22"); return;
			case 23: makemaz("soko2-23"); return;
			case 24: makemaz("soko2-24"); return;
			case 25: makemaz("soko2-25"); return;
			case 26: makemaz("soko2-26"); return;
			case 27: makemaz("soko2-27"); return;
			case 28: makemaz("soko2-28"); return;
			case 29: makemaz("soko2-29"); return;
			case 30: makemaz("soko2-30"); return;
			case 31: makemaz("soko2-31"); return;
			case 32: makemaz("soko2-32"); return;
			case 33: makemaz("soko2-33"); return;
			case 34: makemaz("soko2-34"); return;
			case 35: makemaz("soko2-35"); return;
			case 36: makemaz("soko2-36"); return;
			case 37: makemaz("soko2-37"); return;
			case 38: makemaz("soko2-38"); return;
			case 39: makemaz("soko2-39"); return;
			case 40: makemaz("soko2-40"); return;
			case 41: makemaz("soko2-41"); return;
			case 42: makemaz("soko2-42"); return;
			case 43: makemaz("soko2-43"); return;
			case 44: makemaz("soko2-44"); return;

			case 45: makemaz("soko3-1"); return;
			case 46: makemaz("soko3-2"); return;
			case 47: makemaz("soko3-3"); return;
			case 48: makemaz("soko3-4"); return;
			case 49: makemaz("soko3-5"); return;
			case 50: makemaz("soko3-6"); return;
			case 51: makemaz("soko3-7"); return;
			case 52: makemaz("soko3-8"); return;
			case 53: makemaz("soko3-9"); return;
			case 54: makemaz("soko3-10"); return;
			case 55: makemaz("soko3-11"); return;
			case 56: makemaz("soko3-12"); return;
			case 57: makemaz("soko3-13"); return;
			case 58: makemaz("soko3-14"); return;
			case 59: makemaz("soko3-15"); return;
			case 60: makemaz("soko3-16"); return;
			case 61: makemaz("soko3-17"); return;
			case 62: makemaz("soko3-18"); return;
			case 63: makemaz("soko3-19"); return;
			case 64: makemaz("soko3-20"); return;
			case 65: makemaz("soko3-21"); return;
			case 66: makemaz("soko3-22"); return;
			case 67: makemaz("soko3-23"); return;
			case 68: makemaz("soko3-24"); return;
			case 69: makemaz("soko3-25"); return;
			case 70: makemaz("soko3-26"); return;
			case 71: makemaz("soko3-27"); return;
			case 72: makemaz("soko3-28"); return;
			case 73: makemaz("soko3-29"); return;

			case 74: makemaz("soko5-1"); return;
			case 75: makemaz("soko5-2"); return;
			case 76: makemaz("soko5-3"); return;
			case 77: makemaz("soko5-4"); return;
			case 78: makemaz("soko5-5"); return;
			case 79: makemaz("soko5-6"); return;
			case 80: makemaz("soko5-7"); return;
			case 81: makemaz("soko5-8"); return;
			case 82: makemaz("soko5-9"); return;
			case 83: makemaz("soko5-10"); return;
			case 84: makemaz("soko5-11"); return;
			case 85: makemaz("soko5-12"); return;
			case 86: makemaz("soko5-13"); return;
			case 87: makemaz("soko5-14"); return;
			case 88: makemaz("soko5-15"); return;
			case 89: makemaz("soko5-16"); return;
			case 90: makemaz("soko5-17"); return;
			case 91: makemaz("soko5-18"); return;
			case 92: makemaz("soko5-19"); return;
			case 93: makemaz("soko5-20"); return;
			case 94: makemaz("soko5-21"); return;
			case 95: makemaz("soko5-22"); return;
			case 96: makemaz("soko5-23"); return;
			case 97: makemaz("soko5-24"); return;
			case 98: makemaz("soko5-25"); return;
			case 99: makemaz("soko5-26"); return;
			case 100: makemaz("soko5-27"); return;
			case 101: makemaz("soko5-28"); return;

			case 102: makemaz("soko6-1"); return;
			case 103: makemaz("soko6-2"); return;
			case 104: makemaz("soko6-3"); return;
			case 105: makemaz("soko6-4"); return;
			case 106: makemaz("soko6-5"); return;
			case 107: makemaz("soko6-6"); return;
			case 108: makemaz("soko6-7"); return;
			case 109: makemaz("soko6-8"); return;
			case 110: makemaz("soko6-9"); return;
			case 111: makemaz("soko6-10"); return;
			case 112: makemaz("soko6-11"); return;
			case 113: makemaz("soko6-12"); return;
			case 114: makemaz("soko6-13"); return;
			case 115: makemaz("soko6-14"); return;
			case 116: makemaz("soko6-15"); return;
			case 117: makemaz("soko6-16"); return;
			case 118: makemaz("soko6-17"); return;
			case 119: makemaz("soko6-18"); return;
			case 120: makemaz("soko6-19"); return;
			case 121: makemaz("soko6-20"); return;
			case 122: makemaz("soko6-21"); return;
			case 123: makemaz("soko6-22"); return;
			case 124: makemaz("soko6-23"); return;
			case 125: makemaz("soko6-24"); return;
			case 126: makemaz("soko6-25"); return;
			case 127: makemaz("soko6-26"); return;
			case 128: makemaz("soko6-27"); return;
			case 129: makemaz("soko6-28"); return;
			case 130: makemaz("soko6-29"); return;
			case 131: makemaz("soko6-30"); return;
			case 132: makemaz("soko6-31"); return;
			case 133: makemaz("soko6-32"); return;
			case 134: makemaz("soko6-33"); return;
			case 135: makemaz("soko6-34"); return;
			case 136: makemaz("soko6-35"); return;
			case 137: makemaz("soko6-36"); return;
			case 138: makemaz("soko6-37"); return;
			case 139: makemaz("soko6-38"); return;
			case 140: makemaz("soko6-39"); return;
			case 141: makemaz("soko6-40"); return;
			case 142: makemaz("soko6-41"); return;
			case 143: makemaz("soko6-42"); return;
			case 144: makemaz("soko6-43"); return;
			case 145: makemaz("soko6-44"); return;
			case 146: makemaz("soko6-45"); return;
			case 147: makemaz("soko6-46"); return;
			case 148: makemaz("soko6-47"); return;
			case 149: makemaz("soko6-48"); return;
			case 150: makemaz("soko6-49"); return;
			case 151: makemaz("soko6-50"); return;
			case 152: makemaz("soko6-51"); return;

			case 153: makemaz("soko2-45"); return;

			case 154: makemaz("soko6-52"); return;

		}

		} else if (In_gehennom(&u.uz) || In_sheol(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz) || In_angmar(&u.uz) || In_emynluin(&u.uz) || In_swimmingpool(&u.uz) || In_hellbathroom(&u.uz) || !strcmp(dungeons[u.uz.dnum].dname, "Frankenstein's Lab") ) {

			if (rn2(3)) goto ghnhom3;

		switch (rnd(154)) {

			case 1: makemaz("soko8-1"); return;
			case 2: makemaz("soko8-2"); return;
			case 3: makemaz("soko8-3"); return;
			case 4: makemaz("soko8-4"); return;
			case 5: makemaz("soko8-5"); return;
			case 6: makemaz("soko8-6"); return;
			case 7: makemaz("soko8-7"); return;
			case 8: makemaz("soko8-8"); return;
			case 9: makemaz("soko8-9"); return;
			case 10: makemaz("soko8-10"); return;
			case 11: makemaz("soko8-11"); return;
			case 12: makemaz("soko8-12"); return;
			case 13: makemaz("soko8-13"); return;
			case 14: makemaz("soko8-14"); return;
			case 15: makemaz("soko8-15"); return;
			case 16: makemaz("soko8-16"); return;
			case 17: makemaz("soko8-17"); return;
			case 18: makemaz("soko8-18"); return;
			case 19: makemaz("soko8-19"); return;
			case 20: makemaz("soko8-20"); return;
			case 21: makemaz("soko8-21"); return;
			case 22: makemaz("soko8-22"); return;
			case 23: makemaz("soko8-23"); return;
			case 24: makemaz("soko8-24"); return;
			case 25: makemaz("soko8-25"); return;
			case 26: makemaz("soko8-26"); return;
			case 27: makemaz("soko8-27"); return;
			case 28: makemaz("soko8-28"); return;
			case 29: makemaz("soko8-29"); return;
			case 30: makemaz("soko8-30"); return;
			case 31: makemaz("soko8-31"); return;
			case 32: makemaz("soko8-32"); return;
			case 33: makemaz("soko8-33"); return;
			case 34: makemaz("soko8-34"); return;
			case 35: makemaz("soko8-35"); return;
			case 36: makemaz("soko8-36"); return;
			case 37: makemaz("soko8-37"); return;
			case 38: makemaz("soko8-38"); return;
			case 39: makemaz("soko8-39"); return;
			case 40: makemaz("soko8-40"); return;
			case 41: makemaz("soko8-41"); return;
			case 42: makemaz("soko8-42"); return;
			case 43: makemaz("soko8-43"); return;
			case 44: makemaz("soko8-44"); return;

			case 45: makemaz("soko7-1"); return;
			case 46: makemaz("soko7-2"); return;
			case 47: makemaz("soko7-3"); return;
			case 48: makemaz("soko7-4"); return;
			case 49: makemaz("soko7-5"); return;
			case 50: makemaz("soko7-6"); return;
			case 51: makemaz("soko7-7"); return;
			case 52: makemaz("soko7-8"); return;
			case 53: makemaz("soko7-9"); return;
			case 54: makemaz("soko7-10"); return;
			case 55: makemaz("soko7-11"); return;
			case 56: makemaz("soko7-12"); return;
			case 57: makemaz("soko7-13"); return;
			case 58: makemaz("soko7-14"); return;
			case 59: makemaz("soko7-15"); return;
			case 60: makemaz("soko7-16"); return;
			case 61: makemaz("soko7-17"); return;
			case 62: makemaz("soko7-18"); return;
			case 63: makemaz("soko7-19"); return;
			case 64: makemaz("soko7-20"); return;
			case 65: makemaz("soko7-21"); return;
			case 66: makemaz("soko7-22"); return;
			case 67: makemaz("soko7-23"); return;
			case 68: makemaz("soko7-24"); return;
			case 69: makemaz("soko7-25"); return;
			case 70: makemaz("soko7-26"); return;
			case 71: makemaz("soko7-27"); return;
			case 72: makemaz("soko7-28"); return;
			case 73: makemaz("soko7-29"); return;

			case 74: makemaz("soko9-1"); return;
			case 75: makemaz("soko9-2"); return;
			case 76: makemaz("soko9-3"); return;
			case 77: makemaz("soko9-4"); return;
			case 78: makemaz("soko9-5"); return;
			case 79: makemaz("soko9-6"); return;
			case 80: makemaz("soko9-7"); return;
			case 81: makemaz("soko9-8"); return;
			case 82: makemaz("soko9-9"); return;
			case 83: makemaz("soko9-10"); return;
			case 84: makemaz("soko9-11"); return;
			case 85: makemaz("soko9-12"); return;
			case 86: makemaz("soko9-13"); return;
			case 87: makemaz("soko9-14"); return;
			case 88: makemaz("soko9-15"); return;
			case 89: makemaz("soko9-16"); return;
			case 90: makemaz("soko9-17"); return;
			case 91: makemaz("soko9-18"); return;
			case 92: makemaz("soko9-19"); return;
			case 93: makemaz("soko9-20"); return;
			case 94: makemaz("soko9-21"); return;
			case 95: makemaz("soko9-22"); return;
			case 96: makemaz("soko9-23"); return;
			case 97: makemaz("soko9-24"); return;
			case 98: makemaz("soko9-25"); return;
			case 99: makemaz("soko9-26"); return;
			case 100: makemaz("soko9-27"); return;
			case 101: makemaz("soko9-28"); return;

			case 102: makemaz("soko0-1"); return;
			case 103: makemaz("soko0-2"); return;
			case 104: makemaz("soko0-3"); return;
			case 105: makemaz("soko0-4"); return;
			case 106: makemaz("soko0-5"); return;
			case 107: makemaz("soko0-6"); return;
			case 108: makemaz("soko0-7"); return;
			case 109: makemaz("soko0-8"); return;
			case 110: makemaz("soko0-9"); return;
			case 111: makemaz("soko0-10"); return;
			case 112: makemaz("soko0-11"); return;
			case 113: makemaz("soko0-12"); return;
			case 114: makemaz("soko0-13"); return;
			case 115: makemaz("soko0-14"); return;
			case 116: makemaz("soko0-15"); return;
			case 117: makemaz("soko0-16"); return;
			case 118: makemaz("soko0-17"); return;
			case 119: makemaz("soko0-18"); return;
			case 120: makemaz("soko0-19"); return;
			case 121: makemaz("soko0-20"); return;
			case 122: makemaz("soko0-21"); return;
			case 123: makemaz("soko0-22"); return;
			case 124: makemaz("soko0-23"); return;
			case 125: makemaz("soko0-24"); return;
			case 126: makemaz("soko0-25"); return;
			case 127: makemaz("soko0-26"); return;
			case 128: makemaz("soko0-27"); return;
			case 129: makemaz("soko0-28"); return;
			case 130: makemaz("soko0-29"); return;
			case 131: makemaz("soko0-30"); return;
			case 132: makemaz("soko0-31"); return;
			case 133: makemaz("soko0-32"); return;
			case 134: makemaz("soko0-33"); return;
			case 135: makemaz("soko0-34"); return;
			case 136: makemaz("soko0-35"); return;
			case 137: makemaz("soko0-36"); return;
			case 138: makemaz("soko0-37"); return;
			case 139: makemaz("soko0-38"); return;
			case 140: makemaz("soko0-39"); return;
			case 141: makemaz("soko0-40"); return;
			case 142: makemaz("soko0-41"); return;
			case 143: makemaz("soko0-42"); return;
			case 144: makemaz("soko0-43"); return;
			case 145: makemaz("soko0-44"); return;
			case 146: makemaz("soko0-45"); return;
			case 147: makemaz("soko0-46"); return;
			case 148: makemaz("soko0-47"); return;
			case 149: makemaz("soko0-48"); return;
			case 150: makemaz("soko0-49"); return;
			case 151: makemaz("soko0-50"); return;
			case 152: makemaz("soko0-51"); return;

			case 153: makemaz("soko8-45"); return;

			case 154: makemaz("soko0-52"); return;

		}

		}

	    return;

	}

	if ((specialraceflag == 3) && (!rn2(100) || depth(&u.uz) > 1) ) { /* specialist */

		if (In_dod(&u.uz) || In_mines(&u.uz) || In_sokoban(&u.uz) || In_towndungeon(&u.uz) || In_illusorycastle(&u.uz) || In_deepmines(&u.uz) || In_ZAPM(&u.uz) || In_Devnull(&u.uz) || In_greencross(&u.uz) || !strcmp(dungeons[u.uz.dnum].dname, "Grund's Stronghold") || !strcmp(dungeons[u.uz.dnum].dname, "The Ice Queen's Realm") || !strcmp(dungeons[u.uz.dnum].dname, "The Temple of Moloch") || !strcmp(dungeons[u.uz.dnum].dname, "The Giant Caverns") || !strcmp(dungeons[u.uz.dnum].dname, "The Sunless Sea") || !strcmp(dungeons[u.uz.dnum].dname, "The Spider Caves") || !strcmp(dungeons[u.uz.dnum].dname, "The Lost Tomb") || !strcmp(dungeons[u.uz.dnum].dname, "Yendorian Tower") || !strcmp(dungeons[u.uz.dnum].dname, "Bell Caves") || !strcmp(dungeons[u.uz.dnum].dname, "Forging Chamber") || !strcmp(dungeons[u.uz.dnum].dname, "Dead Grounds") || !strcmp(dungeons[u.uz.dnum].dname, "Ordered Chaos") || !strcmp(dungeons[u.uz.dnum].dname, "The Wyrm Caves") || !strcmp(dungeons[u.uz.dnum].dname, "One-eyed Sam's Market") || !strcmp(dungeons[u.uz.dnum].dname, "Fort Ludios") || In_restingzone(&u.uz)) {

ghnhom4:
	    switch (rnd(117)) {

	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5:
	    case 6:
	    case 7:
	    case 8:
	    case 9:
	    case 10:

		switch (rnd(60)) {

			case 1: makemaz("bigrm-1"); return;
			case 2: makemaz("bigrm-2"); return;
			case 3: makemaz("bigrm-3"); return;
			case 4: makemaz("bigrm-4"); return;
			case 5: makemaz("bigrm-5"); return;
			case 6: makemaz("bigrm-6"); return;
			case 7: makemaz("bigrm-7"); return;
			case 8: makemaz("bigrm-8"); return;
			case 9: makemaz("bigrm-9"); return;
			case 10: makemaz("bigrm-10"); return;
			case 11: makemaz("bigrm-11"); return;
			case 12: makemaz("bigrm-12"); return;
			case 13: makemaz("bigrm-13"); return;
			case 14: makemaz("bigrm-14"); return;
			case 15: makemaz("bigrm-15"); return;
			case 16: makemaz("bigrm-16"); return;
			case 17: makemaz("bigrm-17"); return;
			case 18: makemaz("bigrm-18"); return;
			case 19: makemaz("bigrm-19"); return;
			case 20: makemaz("bigrm-20"); return;
			case 21: makemaz("bigrm-21"); return;
			case 22: makemaz("bigrm-22"); return;
			case 23: makemaz("bigrm-23"); return;
			case 24: makemaz("bigrm-24"); return;
			case 25: makemaz("bigrm-25"); return;
			case 26: makemaz("bigrm-26"); return;
			case 27: makemaz("bigrm-27"); return;
			case 28: makemaz("bigrm-28"); return;
			case 29: makemaz("bigrm-29"); return;
			case 30: makemaz("bigrm-30"); return;
			case 31: makemaz("bigrm-31"); return;
			case 32: makemaz("bigrm-32"); return;
			case 33: makemaz("bigrm-33"); return;
			case 34: makemaz("bigrm-34"); return;
			case 35: makemaz("bigrm-35"); return;
			case 36: makemaz("bigrm-36"); return;
			case 37: makemaz("bigrm-37"); return;
			case 38: makemaz("bigrm-38"); return;
			case 39: makemaz("bigrm-39"); return;
			case 40: makemaz("bigrm-40"); return;
			case 41: makemaz("bigrm-41"); return;
			case 42: makemaz("bigrm-42"); return;
			case 43: makemaz("bigrm-43"); return;
			case 44: makemaz("bigrm-44"); return;
			case 45: makemaz("bigrm-45"); return;
			case 46: makemaz("bigrm-46"); return;
			case 47: makemaz("bigrm-47"); return;
			case 48: makemaz("bigrm-48"); return;
			case 49: makemaz("bigrm-49"); return;
			case 50: makemaz("bigrm-50"); return;
			case 51: makemaz("bigrm-51"); return;
			case 52: makemaz("bigrm-52"); return;
			case 53: makemaz("bigrm-53"); return;
			case 54: makemaz("bigrm-54"); return;
			case 55: makemaz("bigrm-55"); return;
			case 56: makemaz("bigrm-56"); return;
			case 57: makemaz("bigrm-57"); return;
			case 58: makemaz("bigrm-58"); return;
			case 59: makemaz("bigrm-59"); return;
			case 60: makemaz("bigrm-60"); return;

		}
		break;

	    case 11:
	    case 12:
	    case 13:
	    case 14:
	    case 15:
	    case 16:
	    case 17:
	    case 18:
	    case 19:
	    case 20:

		switch (rnd(97)) {

			case 1: makemaz("unhck-1"); return;
			case 2: makemaz("unhck-2"); return;
			case 3: makemaz("unhck-3"); return;
			case 4: makemaz("unhck-4"); return;
			case 5: makemaz("unhck-5"); return;
			case 6: makemaz("unhck-6"); return;
			case 7: makemaz("unhck-7"); return;
			case 8: makemaz("unhck-8"); return;
			case 9: makemaz("unhck-9"); return;
			case 10: makemaz("unhck-10"); return;
			case 11: makemaz("unhck-11"); return;
			case 12: makemaz("unhck-12"); return;
			case 13: makemaz("unhck-13"); return;
			case 14: makemaz("unhck-14"); return;
			case 15: makemaz("unhck-15"); return;
			case 16: makemaz("unhck-16"); return;
			case 17: makemaz("unhck-17"); return;
			case 18: makemaz("unhck-18"); return;
			case 19: makemaz("unhck-19"); return;
			case 20: makemaz("unhck-20"); return;
			case 21: makemaz("unhck-21"); return;
			case 22: makemaz("unhck-22"); return;
			case 23: makemaz("unhck-23"); return;
			case 24: makemaz("unhck-24"); return;
			case 25: makemaz("unhck-25"); return;
			case 26: makemaz("unhck-26"); return;
			case 27: makemaz("unhck-27"); return;
			case 28: makemaz("unhck-28"); return;
			case 29: makemaz("unhck-29"); return;
			case 30: makemaz("unhck-30"); return;
			case 31: makemaz("unhck-31"); return;
			case 32: makemaz("unhck-32"); return;
			case 33: makemaz("unhck-33"); return;
			case 34: makemaz("unhck-34"); return;
			case 35: makemaz("unhck-35"); return;
			case 36: makemaz("unhck-36"); return;
			case 37: makemaz("unhck-37"); return;
			case 38: makemaz("unhck-38"); return;
			case 39: makemaz("unhck-39"); return;
			case 40: makemaz("unhck-40"); return;
			case 41: makemaz("unhck-41"); return;
			case 42: makemaz("unhck-42"); return;
			case 43: makemaz("unhck-43"); return;
			case 44: makemaz("unhck-44"); return;
			case 45: makemaz("unhck-45"); return;
			case 46: makemaz("unhck-46"); return;
			case 47: makemaz("unhck-47"); return;
			case 48: makemaz("unhck-48"); return;
			case 49: makemaz("unhck-49"); return;
			case 50: makemaz("unhck-50"); return;
			case 51: makemaz("unhck-51"); return;
			case 52: makemaz("unhck-52"); return;
			case 53: makemaz("unhck-53"); return;
			case 54: makemaz("unhck-54"); return;
			case 55: makemaz("unhck-55"); return;
			case 56: makemaz("unhck-56"); return;
			case 57: makemaz("unhck-57"); return;
			case 58: makemaz("unhck-58"); return;
			case 59: makemaz("unhck-59"); return;
			case 60: makemaz("unhck-60"); return;
			case 61: makemaz("unhck-61"); return;
			case 62: makemaz("unhck-62"); return;
			case 63: makemaz("unhck-63"); return;
			case 64: makemaz("unhck-64"); return;
			case 65: makemaz("unhck-65"); return;
			case 66: makemaz("unhck-66"); return;
			case 67: makemaz("unhck-67"); return;
			case 68: makemaz("unhck-68"); return;
			case 69: makemaz("unhck-69"); return;
			case 70: makemaz("unhck-70"); return;
			case 71: makemaz("unhck-71"); return;
			case 72: makemaz("unhck-72"); return;
			case 73: makemaz("unhck-73"); return;
			case 74: makemaz("unhck-74"); return;
			case 75: makemaz("unhck-75"); return;
			case 76: makemaz("unhck-76"); return;
			case 77: makemaz("unhck-77"); return;
			case 78: makemaz("unhck-78"); return;
			case 79: makemaz("unhck-79"); return;
			case 80: makemaz("unhck-80"); return;
			case 81: makemaz("unhck-81"); return;
			case 82: makemaz("unhck-82"); return;
			case 83: makemaz("unhck-83"); return;
			case 84: makemaz("unhck-84"); return;
			case 85: makemaz("unhck-85"); return;
			case 86: makemaz("unhck-86"); return;
			case 87: makemaz("unhck-87"); return;
			case 88: makemaz("unhck-88"); return;
			case 89: makemaz("unhck-89"); return;
			case 90: makemaz("unhck-90"); return;
			case 91: makemaz("unhck-91"); return;
			case 92: makemaz("unhck-92"); return;
			case 93: makemaz("unhck-93"); return;
			case 94: makemaz("unhck-94"); return;
			case 95: makemaz("unhck-95"); return;
			case 96: makemaz("unhck-96"); return;
			case 97: makemaz("minusw"); return;

		}
		break;

	    case 21:

		switch (rnd(5)) {

			case 1: makemaz("intpla-1"); return;
			case 2: makemaz("intpla-2"); return;
			case 3: makemaz("intpla-3"); return;
			case 4: makemaz("intpla-4"); return;
			case 5: makemaz("intpla-5"); return;

		}
		break;

	    case 22:
	    case 23:
	    case 24:

		switch (rnd(14)) {

			case 1: makemaz("minefill"); return;
			case 2: makemaz("minefila"); return;
			case 3: makemaz("minefilb"); return;
			case 4: makemaz("minefilc"); return;
			case 5: makemaz("minefild"); return;
			case 6: makemaz("minefile"); return;
			case 7: makemaz("minefilf"); return;
			case 8: makemaz("minefilg"); return;
			case 9: makemaz("minefill"); return;
			case 10: makemaz("minefill"); return;
			case 11: makemaz("minefill"); return;
			case 12: makemaz("minefill"); return;
			case 13: makemaz("minefill"); return;
			case 14: makemaz("minefill"); return;

		}
		break;

	    case 25:
	    case 26:
	    case 27:
	    case 28:
	    case 29:

		switch (rnd(21)) {

			case 1: makemaz("mintn-1"); return;
			case 2: makemaz("mintn-2"); return;
			case 3: makemaz("mintn-3"); return;
			case 4: makemaz("mintn-4"); return;
			case 5: makemaz("mintn-5"); return;
			case 6: makemaz("mintn-6"); return;
			case 7: makemaz("mintn-7"); return;
			case 8: makemaz("mintn-8"); return;
			case 9: makemaz("mintn-9"); return;
			case 10: makemaz("mintn-10"); return;
			case 11: makemaz("mintn-11"); return;
			case 12: makemaz("mintn-12"); return;
			case 13: makemaz("mintn-13"); return;
			case 14: makemaz("mintn-14"); return;
			case 15: makemaz("mintn-15"); return;
			case 16: makemaz("mintn-16"); return;
			case 17: makemaz("mintn-17"); return;
			case 18: makemaz("mintn-18"); return;
			case 19: makemaz("mintn-19"); return;
			case 20: makemaz("mintn-20"); return;
			case 21: makemaz("mintn-21"); return;

		}
		break;

	    case 30:
	    case 31:

		switch (rnd(10)) {

			case 1: makemaz("mnend-1"); return;
			case 2: makemaz("mnend-2"); return;
			case 3: makemaz("mnend-3"); return;
			case 4: makemaz("mnend-4"); return;
			case 5: makemaz("mnend-5"); return;
			case 6: makemaz("mnend-6"); return;
			case 7: makemaz("mnend-7"); return;
			case 8: makemaz("mnend-8"); return;
			case 9: makemaz("mnend-9"); return;
			case 10: makemaz("mnend-10"); return;

		}
		break;

	    case 32:
	    case 33:

		switch (rnd(18)) {

			case 1: makemaz("eking-1"); return;
			case 2: makemaz("eking-2"); return;
			case 3: makemaz("eking-3"); return;
			case 4: makemaz("eking-4"); return;
			case 5: makemaz("eking-5"); return;
			case 6: makemaz("eking-6"); return;
			case 7: makemaz("eking-7"); return;
			case 8: makemaz("eking-8"); return;
			case 9: makemaz("eking-9"); return;
			case 10: makemaz("eking-10"); return;
			case 11: makemaz("eking-11"); return;
			case 12: makemaz("eking-12"); return;
			case 13: makemaz("eking-13"); return;
			case 14: makemaz("eking-14"); return;
			case 15: makemaz("eking-15"); return;
			case 16: makemaz("eking-16"); return;
			case 17: makemaz("eking-17"); return;
			case 18: makemaz("eking-18"); return;

		}
		break;

	    case 34:
	    case 35:
	    case 36:
	    case 37:
	    case 38:
	    case 39:
	    case 40:
	    case 41:
	    case 42:
	    case 43:

		switch (rnd(154)) {

			case 1: makemaz("soko2-1"); return;
			case 2: makemaz("soko2-2"); return;
			case 3: makemaz("soko2-3"); return;
			case 4: makemaz("soko2-4"); return;
			case 5: makemaz("soko2-5"); return;
			case 6: makemaz("soko2-6"); return;
			case 7: makemaz("soko2-7"); return;
			case 8: makemaz("soko2-8"); return;
			case 9: makemaz("soko2-9"); return;
			case 10: makemaz("soko2-10"); return;
			case 11: makemaz("soko2-11"); return;
			case 12: makemaz("soko2-12"); return;
			case 13: makemaz("soko2-13"); return;
			case 14: makemaz("soko2-14"); return;
			case 15: makemaz("soko2-15"); return;
			case 16: makemaz("soko2-16"); return;
			case 17: makemaz("soko2-17"); return;
			case 18: makemaz("soko2-18"); return;
			case 19: makemaz("soko2-19"); return;
			case 20: makemaz("soko2-20"); return;
			case 21: makemaz("soko2-21"); return;
			case 22: makemaz("soko2-22"); return;
			case 23: makemaz("soko2-23"); return;
			case 24: makemaz("soko2-24"); return;
			case 25: makemaz("soko2-25"); return;
			case 26: makemaz("soko2-26"); return;
			case 27: makemaz("soko2-27"); return;
			case 28: makemaz("soko2-28"); return;
			case 29: makemaz("soko2-29"); return;
			case 30: makemaz("soko2-30"); return;
			case 31: makemaz("soko2-31"); return;
			case 32: makemaz("soko2-32"); return;
			case 33: makemaz("soko2-33"); return;
			case 34: makemaz("soko2-34"); return;
			case 35: makemaz("soko2-35"); return;
			case 36: makemaz("soko2-36"); return;
			case 37: makemaz("soko2-37"); return;
			case 38: makemaz("soko2-38"); return;
			case 39: makemaz("soko2-39"); return;
			case 40: makemaz("soko2-40"); return;
			case 41: makemaz("soko2-41"); return;
			case 42: makemaz("soko2-42"); return;
			case 43: makemaz("soko2-43"); return;
			case 44: makemaz("soko2-44"); return;

			case 45: makemaz("soko3-1"); return;
			case 46: makemaz("soko3-2"); return;
			case 47: makemaz("soko3-3"); return;
			case 48: makemaz("soko3-4"); return;
			case 49: makemaz("soko3-5"); return;
			case 50: makemaz("soko3-6"); return;
			case 51: makemaz("soko3-7"); return;
			case 52: makemaz("soko3-8"); return;
			case 53: makemaz("soko3-9"); return;
			case 54: makemaz("soko3-10"); return;
			case 55: makemaz("soko3-11"); return;
			case 56: makemaz("soko3-12"); return;
			case 57: makemaz("soko3-13"); return;
			case 58: makemaz("soko3-14"); return;
			case 59: makemaz("soko3-15"); return;
			case 60: makemaz("soko3-16"); return;
			case 61: makemaz("soko3-17"); return;
			case 62: makemaz("soko3-18"); return;
			case 63: makemaz("soko3-19"); return;
			case 64: makemaz("soko3-20"); return;
			case 65: makemaz("soko3-21"); return;
			case 66: makemaz("soko3-22"); return;
			case 67: makemaz("soko3-23"); return;
			case 68: makemaz("soko3-24"); return;
			case 69: makemaz("soko3-25"); return;
			case 70: makemaz("soko3-26"); return;
			case 71: makemaz("soko3-27"); return;
			case 72: makemaz("soko3-28"); return;
			case 73: makemaz("soko3-29"); return;

			case 74: makemaz("soko5-1"); return;
			case 75: makemaz("soko5-2"); return;
			case 76: makemaz("soko5-3"); return;
			case 77: makemaz("soko5-4"); return;
			case 78: makemaz("soko5-5"); return;
			case 79: makemaz("soko5-6"); return;
			case 80: makemaz("soko5-7"); return;
			case 81: makemaz("soko5-8"); return;
			case 82: makemaz("soko5-9"); return;
			case 83: makemaz("soko5-10"); return;
			case 84: makemaz("soko5-11"); return;
			case 85: makemaz("soko5-12"); return;
			case 86: makemaz("soko5-13"); return;
			case 87: makemaz("soko5-14"); return;
			case 88: makemaz("soko5-15"); return;
			case 89: makemaz("soko5-16"); return;
			case 90: makemaz("soko5-17"); return;
			case 91: makemaz("soko5-18"); return;
			case 92: makemaz("soko5-19"); return;
			case 93: makemaz("soko5-20"); return;
			case 94: makemaz("soko5-21"); return;
			case 95: makemaz("soko5-22"); return;
			case 96: makemaz("soko5-23"); return;
			case 97: makemaz("soko5-24"); return;
			case 98: makemaz("soko5-25"); return;
			case 99: makemaz("soko5-26"); return;
			case 100: makemaz("soko5-27"); return;
			case 101: makemaz("soko5-28"); return;

			case 102: makemaz("soko6-1"); return;
			case 103: makemaz("soko6-2"); return;
			case 104: makemaz("soko6-3"); return;
			case 105: makemaz("soko6-4"); return;
			case 106: makemaz("soko6-5"); return;
			case 107: makemaz("soko6-6"); return;
			case 108: makemaz("soko6-7"); return;
			case 109: makemaz("soko6-8"); return;
			case 110: makemaz("soko6-9"); return;
			case 111: makemaz("soko6-10"); return;
			case 112: makemaz("soko6-11"); return;
			case 113: makemaz("soko6-12"); return;
			case 114: makemaz("soko6-13"); return;
			case 115: makemaz("soko6-14"); return;
			case 116: makemaz("soko6-15"); return;
			case 117: makemaz("soko6-16"); return;
			case 118: makemaz("soko6-17"); return;
			case 119: makemaz("soko6-18"); return;
			case 120: makemaz("soko6-19"); return;
			case 121: makemaz("soko6-20"); return;
			case 122: makemaz("soko6-21"); return;
			case 123: makemaz("soko6-22"); return;
			case 124: makemaz("soko6-23"); return;
			case 125: makemaz("soko6-24"); return;
			case 126: makemaz("soko6-25"); return;
			case 127: makemaz("soko6-26"); return;
			case 128: makemaz("soko6-27"); return;
			case 129: makemaz("soko6-28"); return;
			case 130: makemaz("soko6-29"); return;
			case 131: makemaz("soko6-30"); return;
			case 132: makemaz("soko6-31"); return;
			case 133: makemaz("soko6-32"); return;
			case 134: makemaz("soko6-33"); return;
			case 135: makemaz("soko6-34"); return;
			case 136: makemaz("soko6-35"); return;
			case 137: makemaz("soko6-36"); return;
			case 138: makemaz("soko6-37"); return;
			case 139: makemaz("soko6-38"); return;
			case 140: makemaz("soko6-39"); return;
			case 141: makemaz("soko6-40"); return;
			case 142: makemaz("soko6-41"); return;
			case 143: makemaz("soko6-42"); return;
			case 144: makemaz("soko6-43"); return;
			case 145: makemaz("soko6-44"); return;
			case 146: makemaz("soko6-45"); return;
			case 147: makemaz("soko6-46"); return;
			case 148: makemaz("soko6-47"); return;
			case 149: makemaz("soko6-48"); return;
			case 150: makemaz("soko6-49"); return;
			case 151: makemaz("soko6-50"); return;
			case 152: makemaz("soko6-51"); return;

			case 153: makemaz("soko2-45"); return;

			case 154: makemaz("soko6-52"); return;

		}
		break;

	    case 44:
	    case 45:

		switch (rnd(15)) {

			case 1: makemaz("mall-1"); return;
			case 2: makemaz("mall-2"); return;
			case 3: makemaz("mall-3"); return;
			case 4: makemaz("mall-4"); return;
			case 5: makemaz("mall-5"); return;
			case 6: makemaz("mall-6"); return;
			case 7: makemaz("exmall-1"); return;
			case 8: makemaz("exmall-2"); return;
			case 9: makemaz("exmall-3"); return;
			case 10: makemaz("exmall-4"); return;
			case 11: makemaz("exmall-5"); return;
			case 12: makemaz("exmall-6"); return;
			case 13: makemaz("exmall-7"); return;
			case 14: makemaz("exmall-8"); return;
			case 15: makemaz("exmall-9"); return;

		}
		break;

	    case 46:

		makemaz("oracleX"); return;
		break;

	    case 47:

		switch (rnd(4)) {

			case 1: makemaz("erats-1"); return;
			case 2: makemaz("erats-2"); return;
			case 3: makemaz("erats-3"); return;
			case 4: makemaz("erats-4"); return;

		}
		break;

	    case 48:

		switch (rnd(6)) {

			case 1: makemaz("ekobol-1"); return;
			case 2: makemaz("ekobol-2"); return;
			case 3: makemaz("ekobol-3"); return;
			case 4: makemaz("ekobol-4"); return;
			case 5: makemaz("ekobol-5"); return;
			case 6: makemaz("ekobol-6"); return;

		}
		break;

	    case 49:

		switch (rnd(5)) {

			case 1: makemaz("enymp-1"); return;
			case 2: makemaz("enymp-2"); return;
			case 3: makemaz("enymp-3"); return;
			case 4: makemaz("enymp-4"); return;
			case 5: makemaz("enymp-5"); return;

		}
		break;

	    case 50:

		switch (rnd(9)) {

			case 1: makemaz("stor-1"); return;
			case 2: makemaz("stor-2"); return;
			case 3: makemaz("stor-3"); return;
			case 4: makemaz("stor-4"); return;
			case 5: makemaz("stor-5"); return;
			case 6: makemaz("stor-6"); return;
			case 7: makemaz("stor-7"); return;
			case 8: makemaz("stor-8"); return;
			case 9: makemaz("stor-9"); return;

		}
		break;

	    case 51:

		switch (rnd(8)) {

			case 1: makemaz("guild-1"); return;
			case 2: makemaz("guild-2"); return;
			case 3: makemaz("guild-3"); return;
			case 4: makemaz("guild-4"); return;
			case 5: makemaz("guild-5"); return;
			case 6: makemaz("guild-6"); return;
			case 7: makemaz("guild-7"); return;
			case 8: makemaz("guild-8"); return;

		}
		break;

	    case 52:

		switch (rnd(9)) {

			case 1: makemaz("forge"); return;
			case 2: makemaz("hitchE"); return;
			case 3: makemaz("compuE"); return;
			case 4: makemaz("keyE"); return;
			case 5: makemaz("robtn"); return;
			case 6: makemaz("rabhoE"); return;
			case 7: makemaz("machnE"); return;
			case 8: makemaz("orcbaE"); return;
			case 9: makemaz("sewpl"); return;

		}
		break;

	    case 53:
	    case 54:

		switch (rnd(12)) {

			case 1: makemaz("emedu-1"); return;
			case 2: makemaz("emedu-2"); return;
			case 3: makemaz("emedu-3"); return;
			case 4: makemaz("emedu-4"); return;
			case 5: makemaz("emedu-5"); return;
			case 6: makemaz("emedu-6"); return;
			case 7: makemaz("emedu-7"); return;
			case 8: makemaz("emedu-8"); return;
			case 9: makemaz("emedu-9"); return;
			case 10: makemaz("emedu-10"); return;
			case 11: makemaz("emedu-11"); return;
			case 12: makemaz("emedu-12"); return;

		}
		break;

	    case 55:
	    case 56:

		switch (rnd(15)) {

			case 1: makemaz("ecstl-1"); return;
			case 2: makemaz("ecstl-2"); return;
			case 3: makemaz("ecstl-3"); return;
			case 4: makemaz("ecstl-4"); return;
			case 5: makemaz("ecstl-5"); return;
			case 6: makemaz("ecstl-6"); return;
			case 7: makemaz("ecstl-7"); return;
			case 8: makemaz("ecstl-8"); return;
			case 9: makemaz("ecstl-9"); return;
			case 10: makemaz("ecstl-10"); return;
			case 11: makemaz("ecstl-11"); return;
			case 12: makemaz("ecstl-12"); return;
			case 13: makemaz("ecstl-13"); return;
			case 14: makemaz("ecstl-14"); return;
			case 15: makemaz("ecstl-15"); return;

		}
		break;

	    case 57:
	    case 58:
	    case 59:
	    case 60:
	    case 61:

		switch (rnd(88)) {

			case 1: makemaz("egehn-1"); return;
			case 2: makemaz("egehn-2"); return;
			case 3: makemaz("egehn-3"); return;
			case 4: makemaz("egehn-4"); return;
			case 5: makemaz("egehn-5"); return;
			case 6: makemaz("egehn-6"); return;
			case 7: makemaz("egehn-7"); return;
			case 8: makemaz("egehn-8"); return;
			case 9: makemaz("egehn-9"); return;
			case 10: makemaz("egehn-10"); return;
			case 11: makemaz("egehn-11"); return;
			case 12: makemaz("egehn-12"); return;
			case 13: makemaz("egehn-13"); return;
			case 14: makemaz("egehn-14"); return;
			case 15: makemaz("egehn-15"); return;
			case 16: makemaz("egehn-16"); return;
			case 17: makemaz("egehn-17"); return;
			case 18: makemaz("egehn-18"); return;
			case 19: makemaz("egehn-19"); return;
			case 20: makemaz("egehn-20"); return;
			case 21: makemaz("egehn-21"); return;
			case 22: makemaz("egehn-22"); return;
			case 23: makemaz("egehn-23"); return;
			case 24: makemaz("egehn-24"); return;
			case 25: makemaz("egehn-25"); return;
			case 26: makemaz("egehn-26"); return;
			case 27: makemaz("egehn-27"); return;
			case 28: makemaz("egehn-28"); return;
			case 29: makemaz("egehn-29"); return;
			case 30: makemaz("egehn-30"); return;
			case 31: makemaz("egehn-31"); return;
			case 32: makemaz("egehn-32"); return;
			case 33: makemaz("egehn-33"); return;
			case 34: makemaz("egehn-34"); return;
			case 35: makemaz("egehn-35"); return;
			case 36: makemaz("egehn-36"); return;
			case 37: makemaz("egehn-37"); return;
			case 38: makemaz("egehn-38"); return;
			case 39: makemaz("egehn-39"); return;
			case 40: makemaz("egehn-40"); return;
			case 41: makemaz("egehn-41"); return;
			case 42: makemaz("egehn-42"); return;
			case 43: makemaz("egehn-43"); return;
			case 44: makemaz("egehn-44"); return;
			case 45: makemaz("egehn-45"); return;
			case 46: makemaz("egehn-46"); return;
			case 47: makemaz("egehn-47"); return;
			case 48: makemaz("egehn-48"); return;
			case 49: makemaz("egehn-49"); return;
			case 50: makemaz("egehn-50"); return;
			case 51: makemaz("egehn-51"); return;
			case 52: makemaz("egehn-52"); return;
			case 53: makemaz("egehn-53"); return;
			case 54: makemaz("egehn-54"); return;
			case 55: makemaz("egehn-55"); return;
			case 56: makemaz("egehn-56"); return;
			case 57: makemaz("egehn-57"); return;
			case 58: makemaz("egehn-58"); return;
			case 59: makemaz("egehn-59"); return;
			case 60: makemaz("egehn-60"); return;
			case 61: makemaz("egehn-61"); return;
			case 62: makemaz("egehn-62"); return;
			case 63: makemaz("egehn-63"); return;
			case 64: makemaz("egehn-64"); return;
			case 65: makemaz("egehn-65"); return;
			case 66: makemaz("egehn-66"); return;
			case 67: makemaz("egehn-67"); return;
			case 68: makemaz("egehn-68"); return;
			case 69: makemaz("egehn-69"); return;
			case 70: makemaz("egehn-70"); return;
			case 71: makemaz("egehn-71"); return;
			case 72: makemaz("egehn-72"); return;
			case 73: makemaz("egehn-73"); return;
			case 74: makemaz("egehn-74"); return;
			case 75: makemaz("egehn-75"); return;
			case 76: makemaz("egehn-76"); return;
			case 77: makemaz("egehn-77"); return;
			case 78: makemaz("egehn-78"); return;
			case 79: makemaz("egehn-79"); return;
			case 80: makemaz("egehn-80"); return;
			case 81: makemaz("egehn-81"); return;
			case 82: makemaz("egehn-82"); return;
			case 83: makemaz("egehn-83"); return;
			case 84: makemaz("egehn-84"); return;
			case 85: makemaz("egehn-85"); return;
			case 86: makemaz("egehn-86"); return;
			case 87: makemaz("egehn-87"); return;
			case 88: makemaz("egehn-88"); return;

		}
		break;

	    case 62:

		makemaz("schoolX"); return;
		break;

	    case 63:

		switch (rnd(4)) {

			case 1: makemaz("etown-1"); return;
			case 2: makemaz("etown-2"); return;
			case 3: makemaz("etown-3"); return;
			case 4: makemaz("etown-4"); return;

		}
		break;

	    case 64:

		switch (rnd(3)) {

			case 1: makemaz("egrund-1"); return;
			case 2: makemaz("egrund-2"); return;
			case 3: makemaz("egrund-3"); return;

		}
		break;

	    case 65:

		switch (rnd(3)) {

			case 1: makemaz("eknox-1"); return;
			case 2: makemaz("eknox-2"); return;
			case 3: makemaz("eknox-3"); return;

		}
		break;

	    case 66:

		makemaz("dragons"); return;
		break;

	    case 67:

		makemaz("etomb"); return;
		break;

	    case 68:

		makemaz("espiders"); return;
		break;

	    case 69:

		makemaz("esea"); return;
		break;

	    case 70:

		makemaz("emtemple"); return;
		break;

	    case 71:
	    case 72:
	    case 73:

		switch (rnd(22)) {

			case 1: makemaz("esheo-1"); return;
			case 2: makemaz("esheo-2"); return;
			case 3: makemaz("esheo-3"); return;
			case 4: makemaz("esheo-4"); return;
			case 5: makemaz("esheo-5"); return;
			case 6: makemaz("esheo-6"); return;
			case 7: makemaz("esheo-7"); return;
			case 8: makemaz("esheo-8"); return;
			case 9: makemaz("esheo-9"); return;
			case 10: makemaz("esheo-10"); return;
			case 11: makemaz("esheo-11"); return;
			case 12: makemaz("esheo-12"); return;
			case 13: makemaz("esheo-13"); return;
			case 14: makemaz("esheo-14"); return;
			case 15: makemaz("esheo-15"); return;
			case 16: makemaz("esheo-16"); return;
			case 17: makemaz("esheo-17"); return;
			case 18: makemaz("esheo-18"); return;
			case 19: makemaz("esheo-19"); return;
			case 20: makemaz("esheo-20"); return;
			case 21: makemaz("esheo-21"); return;
			case 22: makemaz("esheo-22"); return;

		}
		break;

	    case 74:
	    case 75:
	    case 76:
	    case 77:
	    case 78:
	    case 79:
	    case 80:
	    case 81:
	    case 82:
	    case 83:
	    case 84:
	    case 85:
	    case 86:
	    case 87:
	    case 88:
	    case 89:
	    case 90:
	    case 91:
	    case 92:
	    case 93:

		switch (rnd(855)) {

			case 1: makemaz("Aci-1"); return;
			case 2: makemaz("Aci-2"); return;
			case 3: makemaz("Aci-3"); return;
			case 4: makemaz("Aci-4"); return;
			case 5: makemaz("Aci-5"); return;
			case 6: makemaz("Act-1"); return;
			case 7: makemaz("Act-2"); return;
			case 8: makemaz("Act-3"); return;
			case 9: makemaz("Act-4"); return;
			case 10: makemaz("Act-5"); return;
			case 11: makemaz("Alt-1"); return;
			case 12: makemaz("Alt-2"); return;
			case 13: makemaz("Alt-3"); return;
			case 14: makemaz("Alt-4"); return;
			case 15: makemaz("Alt-5"); return;
			case 16: makemaz("Ama-1"); return;
			case 17: makemaz("Ama-2"); return;
			case 18: makemaz("Ama-3"); return;
			case 19: makemaz("Ama-4"); return;
			case 20: makemaz("Ama-5"); return;
			case 21: makemaz("Arc-1"); return;
			case 22: makemaz("Arc-2"); return;
			case 23: makemaz("Arc-3"); return;
			case 24: makemaz("Arc-4"); return;
			case 25: makemaz("Arc-5"); return;
			case 26: makemaz("Art-1"); return;
			case 27: makemaz("Art-2"); return;
			case 28: makemaz("Art-3"); return;
			case 29: makemaz("Art-4"); return;
			case 30: makemaz("Art-5"); return;
			case 31: makemaz("Ass-1"); return;
			case 32: makemaz("Ass-2"); return;
			case 33: makemaz("Ass-3"); return;
			case 34: makemaz("Ass-4"); return;
			case 35: makemaz("Ass-5"); return;
			case 36: makemaz("Aug-1"); return;
			case 37: makemaz("Aug-2"); return;
			case 38: makemaz("Aug-3"); return;
			case 39: makemaz("Aug-4"); return;
			case 40: makemaz("Aug-5"); return;
			case 41: makemaz("Bar-1"); return;
			case 42: makemaz("Bar-2"); return;
			case 43: makemaz("Bar-3"); return;
			case 44: makemaz("Bar-4"); return;
			case 45: makemaz("Bar-5"); return;
			case 46: makemaz("Brd-1"); return;
			case 47: makemaz("Brd-2"); return;
			case 48: makemaz("Brd-3"); return;
			case 49: makemaz("Brd-4"); return;
			case 50: makemaz("Brd-5"); return;
			case 51: makemaz("Bin-1"); return;
			case 52: makemaz("Bin-2"); return;
			case 53: makemaz("Bin-3"); return;
			case 54: makemaz("Bin-4"); return;
			case 55: makemaz("Bin-5"); return;
			case 56: makemaz("Ble-1"); return;
			case 57: makemaz("Ble-2"); return;
			case 58: makemaz("Ble-3"); return;
			case 59: makemaz("Ble-4"); return;
			case 60: makemaz("Ble-5"); return;
			case 61: makemaz("Blo-1"); return;
			case 62: makemaz("Blo-2"); return;
			case 63: makemaz("Blo-3"); return;
			case 64: makemaz("Blo-4"); return;
			case 65: makemaz("Blo-5"); return;
			case 66: makemaz("Bos-1"); return;
			case 67: makemaz("Bos-2"); return;
			case 68: makemaz("Bos-3"); return;
			case 69: makemaz("Bos-4"); return;
			case 70: makemaz("Bos-5"); return;
			case 71: makemaz("Bul-1"); return;
			case 72: makemaz("Bul-2"); return;
			case 73: makemaz("Bul-3"); return;
			case 74: makemaz("Bul-4"); return;
			case 75: makemaz("Bul-5"); return;
			case 76: makemaz("Cav-1"); return;
			case 77: makemaz("Cav-2"); return;
			case 78: makemaz("Cav-3"); return;
			case 79: makemaz("Cav-4"); return;
			case 80: makemaz("Cav-5"); return;
			case 81: makemaz("Che-1"); return;
			case 82: makemaz("Che-2"); return;
			case 83: makemaz("Che-3"); return;
			case 84: makemaz("Che-4"); return;
			case 85: makemaz("Che-5"); return;
			case 86: makemaz("Con-1"); return;
			case 87: makemaz("Con-2"); return;
			case 88: makemaz("Con-3"); return;
			case 89: makemaz("Con-4"); return;
			case 90: makemaz("Con-5"); return;
			case 91: makemaz("Coo-1"); return;
			case 92: makemaz("Coo-2"); return;
			case 93: makemaz("Coo-3"); return;
			case 94: makemaz("Coo-4"); return;
			case 95: makemaz("Coo-5"); return;
			case 96: makemaz("Cou-1"); return;
			case 97: makemaz("Cou-2"); return;
			case 98: makemaz("Cou-3"); return;
			case 99: makemaz("Cou-4"); return;
			case 100: makemaz("Cou-5"); return;
			case 101: makemaz("Abu-1"); return;
			case 102: makemaz("Abu-2"); return;
			case 103: makemaz("Abu-3"); return;
			case 104: makemaz("Abu-4"); return;
			case 105: makemaz("Abu-5"); return;
			case 106: makemaz("Dea-1"); return;
			case 107: makemaz("Dea-2"); return;
			case 108: makemaz("Dea-3"); return;
			case 109: makemaz("Dea-4"); return;
			case 110: makemaz("Dea-5"); return;
			case 111: makemaz("Div-1"); return;
			case 112: makemaz("Div-2"); return;
			case 113: makemaz("Div-3"); return;
			case 114: makemaz("Div-4"); return;
			case 115: makemaz("Div-5"); return;
			case 116: makemaz("Dol-1"); return;
			case 117: makemaz("Dol-2"); return;
			case 118: makemaz("Dol-3"); return;
			case 119: makemaz("Dol-4"); return;
			case 120: makemaz("Dol-5"); return;
			case 121: makemaz("Dru-1"); return;
			case 122: makemaz("Dru-2"); return;
			case 123: makemaz("Dru-3"); return;
			case 124: makemaz("Dru-4"); return;
			case 125: makemaz("Dru-5"); return;
			case 126: makemaz("Dun-1"); return;
			case 127: makemaz("Dun-2"); return;
			case 128: makemaz("Dun-3"); return;
			case 129: makemaz("Dun-4"); return;
			case 130: makemaz("Dun-5"); return;
			case 131: makemaz("Ele-1"); return;
			case 132: makemaz("Ele-2"); return;
			case 133: makemaz("Ele-3"); return;
			case 134: makemaz("Ele-4"); return;
			case 135: makemaz("Ele-5"); return;
			case 136: makemaz("Elp-1"); return;
			case 137: makemaz("Elp-2"); return;
			case 138: makemaz("Elp-3"); return;
			case 139: makemaz("Elp-4"); return;
			case 140: makemaz("Elp-5"); return;
			case 141: makemaz("Stu-1"); return;
			case 142: makemaz("Stu-2"); return;
			case 143: makemaz("Stu-3"); return;
			case 144: makemaz("Stu-4"); return;
			case 145: makemaz("Stu-5"); return;
			case 146: makemaz("Fir-1"); return;
			case 147: makemaz("Fir-2"); return;
			case 148: makemaz("Fir-3"); return;
			case 149: makemaz("Fir-4"); return;
			case 150: makemaz("Fir-5"); return;
			case 151: makemaz("Fla-1"); return;
			case 152: makemaz("Fla-2"); return;
			case 153: makemaz("Fla-3"); return;
			case 154: makemaz("Fla-4"); return;
			case 155: makemaz("Fla-5"); return;
			case 156: makemaz("Fox-1"); return;
			case 157: makemaz("Fox-2"); return;
			case 158: makemaz("Fox-3"); return;
			case 159: makemaz("Fox-4"); return;
			case 160: makemaz("Fox-5"); return;
			case 161: makemaz("Gam-1"); return;
			case 162: makemaz("Gam-2"); return;
			case 163: makemaz("Gam-3"); return;
			case 164: makemaz("Gam-4"); return;
			case 165: makemaz("Gam-5"); return;
			case 166: makemaz("Gan-1"); return;
			case 167: makemaz("Gan-2"); return;
			case 168: makemaz("Gan-3"); return;
			case 169: makemaz("Gan-4"); return;
			case 170: makemaz("Gan-5"); return;
			case 171: makemaz("Gee-1"); return;
			case 172: makemaz("Gee-2"); return;
			case 173: makemaz("Gee-3"); return;
			case 174: makemaz("Gee-4"); return;
			case 175: makemaz("Gee-5"); return;
			case 176: makemaz("Gla-1"); return;
			case 177: makemaz("Gla-2"); return;
			case 178: makemaz("Gla-3"); return;
			case 179: makemaz("Gla-4"); return;
			case 180: makemaz("Gla-5"); return;
			case 181: makemaz("Gof-1"); return;
			case 182: makemaz("Gof-2"); return;
			case 183: makemaz("Gof-3"); return;
			case 184: makemaz("Gof-4"); return;
			case 185: makemaz("Gof-5"); return;
			case 186: makemaz("Gra-1"); return;
			case 187: makemaz("Gra-2"); return;
			case 188: makemaz("Gra-3"); return;
			case 189: makemaz("Gra-4"); return;
			case 190: makemaz("Gra-5"); return;
			case 191: makemaz("Gun-1"); return;
			case 192: makemaz("Gun-2"); return;
			case 193: makemaz("Gun-3"); return;
			case 194: makemaz("Gun-4"); return;
			case 195: makemaz("Gun-5"); return;
			case 196: makemaz("Hea-1"); return;
			case 197: makemaz("Hea-2"); return;
			case 198: makemaz("Hea-3"); return;
			case 199: makemaz("Hea-4"); return;
			case 200: makemaz("Hea-5"); return;
			case 201: makemaz("Ice-1"); return;
			case 202: makemaz("Ice-2"); return;
			case 203: makemaz("Ice-3"); return;
			case 204: makemaz("Ice-4"); return;
			case 205: makemaz("Ice-5"); return;
			case 206: makemaz("Scr-1"); return;
			case 207: makemaz("Scr-2"); return;
			case 208: makemaz("Scr-3"); return;
			case 209: makemaz("Scr-4"); return;
			case 210: makemaz("Scr-5"); return;
			case 211: makemaz("Jed-1"); return;
			case 212: makemaz("Jed-2"); return;
			case 213: makemaz("Jed-3"); return;
			case 214: makemaz("Jed-4"); return;
			case 215: makemaz("Jed-5"); return;
			case 216: makemaz("Jes-1"); return;
			case 217: makemaz("Jes-2"); return;
			case 218: makemaz("Jes-3"); return;
			case 219: makemaz("Jes-4"); return;
			case 220: makemaz("Jes-5"); return;
			case 221: makemaz("Kni-1"); return;
			case 222: makemaz("Kni-2"); return;
			case 223: makemaz("Kni-3"); return;
			case 224: makemaz("Kni-4"); return;
			case 225: makemaz("Kni-5"); return;
			case 226: makemaz("Kor-1"); return;
			case 227: makemaz("Kor-2"); return;
			case 228: makemaz("Kor-3"); return;
			case 229: makemaz("Kor-4"); return;
			case 230: makemaz("Kor-5"); return;
			case 231: makemaz("Lad-1"); return;
			case 232: makemaz("Lad-2"); return;
			case 233: makemaz("Lad-3"); return;
			case 234: makemaz("Lad-4"); return;
			case 235: makemaz("Lad-5"); return;
			case 236: makemaz("Lib-1"); return;
			case 237: makemaz("Lib-2"); return;
			case 238: makemaz("Lib-3"); return;
			case 239: makemaz("Lib-4"); return;
			case 240: makemaz("Lib-5"); return;
			case 241: makemaz("Loc-1"); return;
			case 242: makemaz("Loc-2"); return;
			case 243: makemaz("Loc-3"); return;
			case 244: makemaz("Loc-4"); return;
			case 245: makemaz("Loc-5"); return;
			case 246: makemaz("Lun-1"); return;
			case 247: makemaz("Lun-2"); return;
			case 248: makemaz("Lun-3"); return;
			case 249: makemaz("Lun-4"); return;
			case 250: makemaz("Lun-5"); return;
			case 251: makemaz("Mah-1"); return;
			case 252: makemaz("Mah-2"); return;
			case 253: makemaz("Mah-3"); return;
			case 254: makemaz("Mah-4"); return;
			case 255: makemaz("Mah-5"); return;
			case 256: makemaz("Mon-1"); return;
			case 257: makemaz("Mon-2"); return;
			case 258: makemaz("Mon-3"); return;
			case 259: makemaz("Mon-4"); return;
			case 260: makemaz("Mon-5"); return;
			case 261: makemaz("Mus-1"); return;
			case 262: makemaz("Mus-2"); return;
			case 263: makemaz("Mus-3"); return;
			case 264: makemaz("Mus-4"); return;
			case 265: makemaz("Mus-5"); return;
			case 266: makemaz("Mys-1"); return;
			case 267: makemaz("Mys-2"); return;
			case 268: makemaz("Mys-3"); return;
			case 269: makemaz("Mys-4"); return;
			case 270: makemaz("Mys-5"); return;
			case 271: makemaz("Nec-1"); return;
			case 272: makemaz("Nec-2"); return;
			case 273: makemaz("Nec-3"); return;
			case 274: makemaz("Nec-4"); return;
			case 275: makemaz("Nec-5"); return;
			case 276: makemaz("Nin-1"); return;
			case 277: makemaz("Nin-2"); return;
			case 278: makemaz("Nin-3"); return;
			case 279: makemaz("Nin-4"); return;
			case 280: makemaz("Nin-5"); return;
			case 281: makemaz("Nob-1"); return;
			case 282: makemaz("Nob-2"); return;
			case 283: makemaz("Nob-3"); return;
			case 284: makemaz("Nob-4"); return;
			case 285: makemaz("Nob-5"); return;
			case 286: makemaz("Off-1"); return;
			case 287: makemaz("Off-2"); return;
			case 288: makemaz("Off-3"); return;
			case 289: makemaz("Off-4"); return;
			case 290: makemaz("Off-5"); return;
			case 291: makemaz("Ord-1"); return;
			case 292: makemaz("Ord-2"); return;
			case 293: makemaz("Ord-3"); return;
			case 294: makemaz("Ord-4"); return;
			case 295: makemaz("Ord-5"); return;
			case 296: makemaz("Ota-1"); return;
			case 297: makemaz("Ota-2"); return;
			case 298: makemaz("Ota-3"); return;
			case 299: makemaz("Ota-4"); return;
			case 300: makemaz("Ota-5"); return;
			case 301: makemaz("Pal-1"); return;
			case 302: makemaz("Pal-2"); return;
			case 303: makemaz("Pal-3"); return;
			case 304: makemaz("Pal-4"); return;
			case 305: makemaz("Pal-5"); return;
			case 306: makemaz("Pic-1"); return;
			case 307: makemaz("Pic-2"); return;
			case 308: makemaz("Pic-3"); return;
			case 309: makemaz("Pic-4"); return;
			case 310: makemaz("Pic-5"); return;
			case 311: makemaz("Pir-1"); return;
			case 312: makemaz("Pir-2"); return;
			case 313: makemaz("Pir-3"); return;
			case 314: makemaz("Pir-4"); return;
			case 315: makemaz("Pir-5"); return;
			case 316: makemaz("Pok-1"); return;
			case 317: makemaz("Pok-2"); return;
			case 318: makemaz("Pok-3"); return;
			case 319: makemaz("Pok-4"); return;
			case 320: makemaz("Pok-5"); return;
			case 321: makemaz("Pol-1"); return;
			case 322: makemaz("Pol-2"); return;
			case 323: makemaz("Pol-3"); return;
			case 324: makemaz("Pol-4"); return;
			case 325: makemaz("Pol-5"); return;
			case 326: makemaz("Pri-1"); return;
			case 327: makemaz("Pri-2"); return;
			case 328: makemaz("Pri-3"); return;
			case 329: makemaz("Pri-4"); return;
			case 330: makemaz("Pri-5"); return;
			case 331: makemaz("Psi-1"); return;
			case 332: makemaz("Psi-2"); return;
			case 333: makemaz("Psi-3"); return;
			case 334: makemaz("Psi-4"); return;
			case 335: makemaz("Psi-5"); return;
			case 336: makemaz("Ran-1"); return;
			case 337: makemaz("Ran-2"); return;
			case 338: makemaz("Ran-3"); return;
			case 339: makemaz("Ran-4"); return;
			case 340: makemaz("Ran-5"); return;
			case 341: makemaz("Roc-1"); return;
			case 342: makemaz("Roc-2"); return;
			case 343: makemaz("Roc-3"); return;
			case 344: makemaz("Roc-4"); return;
			case 345: makemaz("Roc-5"); return;
			case 346: makemaz("Rog-1"); return;
			case 347: makemaz("Rog-2"); return;
			case 348: makemaz("Rog-3"); return;
			case 349: makemaz("Rog-4"); return;
			case 350: makemaz("Rog-5"); return;
			case 351: makemaz("Sag-1"); return;
			case 352: makemaz("Sag-2"); return;
			case 353: makemaz("Sag-3"); return;
			case 354: makemaz("Sag-4"); return;
			case 355: makemaz("Sag-5"); return;
			case 356: makemaz("Sai-1"); return;
			case 357: makemaz("Sai-2"); return;
			case 358: makemaz("Sai-3"); return;
			case 359: makemaz("Sai-4"); return;
			case 360: makemaz("Sai-5"); return;
			case 361: makemaz("Sam-1"); return;
			case 362: makemaz("Sam-2"); return;
			case 363: makemaz("Sam-3"); return;
			case 364: makemaz("Sam-4"); return;
			case 365: makemaz("Sam-5"); return;
			case 366: makemaz("Sci-1"); return;
			case 367: makemaz("Sci-2"); return;
			case 368: makemaz("Sci-3"); return;
			case 369: makemaz("Sci-4"); return;
			case 370: makemaz("Sci-5"); return;
			case 371: makemaz("Sla-1"); return;
			case 372: makemaz("Sla-2"); return;
			case 373: makemaz("Sla-3"); return;
			case 374: makemaz("Sla-4"); return;
			case 375: makemaz("Sla-5"); return;
			case 376: makemaz("Spa-1"); return;
			case 377: makemaz("Spa-2"); return;
			case 378: makemaz("Spa-3"); return;
			case 379: makemaz("Spa-4"); return;
			case 380: makemaz("Spa-5"); return;
			case 381: makemaz("Sup-1"); return;
			case 382: makemaz("Sup-2"); return;
			case 383: makemaz("Sup-3"); return;
			case 384: makemaz("Sup-4"); return;
			case 385: makemaz("Sup-5"); return;
			case 386: makemaz("Tha-1"); return;
			case 387: makemaz("Tha-2"); return;
			case 388: makemaz("Tha-3"); return;
			case 389: makemaz("Tha-4"); return;
			case 390: makemaz("Tha-5"); return;
			case 391: makemaz("Top-1"); return;
			case 392: makemaz("Top-2"); return;
			case 393: makemaz("Top-3"); return;
			case 394: makemaz("Top-4"); return;
			case 395: makemaz("Top-5"); return;
			case 396: makemaz("Tou-1"); return;
			case 397: makemaz("Tou-2"); return;
			case 398: makemaz("Tou-3"); return;
			case 399: makemaz("Tou-4"); return;
			case 400: makemaz("Tou-5"); return;
			case 401: makemaz("Tra-1"); return;
			case 402: makemaz("Tra-2"); return;
			case 403: makemaz("Tra-3"); return;
			case 404: makemaz("Tra-4"); return;
			case 405: makemaz("Tra-5"); return;
			case 406: makemaz("Und-1"); return;
			case 407: makemaz("Und-2"); return;
			case 408: makemaz("Und-3"); return;
			case 409: makemaz("Und-4"); return;
			case 410: makemaz("Und-5"); return;
			case 411: makemaz("Unt-1"); return;
			case 412: makemaz("Unt-2"); return;
			case 413: makemaz("Unt-3"); return;
			case 414: makemaz("Unt-4"); return;
			case 415: makemaz("Unt-5"); return;
			case 416: makemaz("Val-1"); return;
			case 417: makemaz("Val-2"); return;
			case 418: makemaz("Val-3"); return;
			case 419: makemaz("Val-4"); return;
			case 420: makemaz("Val-5"); return;
			case 421: makemaz("Wan-1"); return;
			case 422: makemaz("Wan-2"); return;
			case 423: makemaz("Wan-3"); return;
			case 424: makemaz("Wan-4"); return;
			case 425: makemaz("Wan-5"); return;
			case 426: makemaz("War-1"); return;
			case 427: makemaz("War-2"); return;
			case 428: makemaz("War-3"); return;
			case 429: makemaz("War-4"); return;
			case 430: makemaz("War-5"); return;
			case 431: makemaz("Wiz-1"); return;
			case 432: makemaz("Wiz-2"); return;
			case 433: makemaz("Wiz-3"); return;
			case 434: makemaz("Wiz-4"); return;
			case 435: makemaz("Wiz-5"); return;
			case 436: makemaz("Yeo-1"); return;
			case 437: makemaz("Yeo-2"); return;
			case 438: makemaz("Yeo-3"); return;
			case 439: makemaz("Yeo-4"); return;
			case 440: makemaz("Yeo-5"); return;
			case 441: makemaz("Zoo-1"); return;
			case 442: makemaz("Zoo-2"); return;
			case 443: makemaz("Zoo-3"); return;
			case 444: makemaz("Zoo-4"); return;
			case 445: makemaz("Zoo-5"); return;
			case 446: makemaz("Zyb-1"); return;
			case 447: makemaz("Zyb-2"); return;
			case 448: makemaz("Zyb-3"); return;
			case 449: makemaz("Zyb-4"); return;
			case 450: makemaz("Zyb-5"); return;
			case 451: makemaz("Ana-1"); return;
			case 452: makemaz("Ana-2"); return;
			case 453: makemaz("Ana-3"); return;
			case 454: makemaz("Ana-4"); return;
			case 455: makemaz("Ana-5"); return;
			case 456: makemaz("Cam-1"); return;
			case 457: makemaz("Cam-2"); return;
			case 458: makemaz("Cam-3"); return;
			case 459: makemaz("Cam-4"); return;
			case 460: makemaz("Cam-5"); return;
			case 461: makemaz("Mar-1"); return;
			case 462: makemaz("Mar-2"); return;
			case 463: makemaz("Mar-3"); return;
			case 464: makemaz("Mar-4"); return;
			case 465: makemaz("Mar-5"); return;
			case 466: makemaz("Sli-1"); return;
			case 467: makemaz("Sli-2"); return;
			case 468: makemaz("Sli-3"); return;
			case 469: makemaz("Sli-4"); return;
			case 470: makemaz("Sli-5"); return;
			case 471: makemaz("Drd-1"); return;
			case 472: makemaz("Drd-2"); return;
			case 473: makemaz("Drd-3"); return;
			case 474: makemaz("Drd-4"); return;
			case 475: makemaz("Drd-5"); return;
			case 476: makemaz("Erd-1"); return;
			case 477: makemaz("Erd-2"); return;
			case 478: makemaz("Erd-3"); return;
			case 479: makemaz("Erd-4"); return;
			case 480: makemaz("Erd-5"); return;
			case 481: makemaz("Fai-1"); return;
			case 482: makemaz("Fai-2"); return;
			case 483: makemaz("Fai-3"); return;
			case 484: makemaz("Fai-4"); return;
			case 485: makemaz("Fai-5"); return;
			case 486: makemaz("Fen-1"); return;
			case 487: makemaz("Fen-2"); return;
			case 488: makemaz("Fen-3"); return;
			case 489: makemaz("Fen-4"); return;
			case 490: makemaz("Fen-5"); return;
			case 491: makemaz("Fig-1"); return;
			case 492: makemaz("Fig-2"); return;
			case 493: makemaz("Fig-3"); return;
			case 494: makemaz("Fig-4"); return;
			case 495: makemaz("Fig-5"); return;
			case 496: makemaz("Gol-1"); return;
			case 497: makemaz("Gol-2"); return;
			case 498: makemaz("Gol-3"); return;
			case 499: makemaz("Gol-4"); return;
			case 500: makemaz("Gol-5"); return;
			case 501: makemaz("Jus-1"); return;
			case 502: makemaz("Jus-2"); return;
			case 503: makemaz("Jus-3"); return;
			case 504: makemaz("Jus-4"); return;
			case 505: makemaz("Jus-5"); return;
			case 506: makemaz("Med-1"); return;
			case 507: makemaz("Med-2"); return;
			case 508: makemaz("Med-3"); return;
			case 509: makemaz("Med-4"); return;
			case 510: makemaz("Med-5"); return;
			case 511: makemaz("Mid-1"); return;
			case 512: makemaz("Mid-2"); return;
			case 513: makemaz("Mid-3"); return;
			case 514: makemaz("Mid-4"); return;
			case 515: makemaz("Mid-5"); return;
			case 516: makemaz("Mur-1"); return;
			case 517: makemaz("Mur-2"); return;
			case 518: makemaz("Mur-3"); return;
			case 519: makemaz("Mur-4"); return;
			case 520: makemaz("Mur-5"); return;
			case 521: makemaz("Poi-1"); return;
			case 522: makemaz("Poi-2"); return;
			case 523: makemaz("Poi-3"); return;
			case 524: makemaz("Poi-4"); return;
			case 525: makemaz("Poi-5"); return;
			case 526: makemaz("Rin-1"); return;
			case 527: makemaz("Rin-2"); return;
			case 528: makemaz("Rin-3"); return;
			case 529: makemaz("Rin-4"); return;
			case 530: makemaz("Rin-5"); return;
			case 531: makemaz("Sha-1"); return;
			case 532: makemaz("Sha-2"); return;
			case 533: makemaz("Sha-3"); return;
			case 534: makemaz("Sha-4"); return;
			case 535: makemaz("Sha-5"); return;
			case 536: makemaz("Twe-1"); return;
			case 537: makemaz("Twe-2"); return;
			case 538: makemaz("Twe-3"); return;
			case 539: makemaz("Twe-4"); return;
			case 540: makemaz("Twe-5"); return;
			case 541: makemaz("Use-1"); return;
			case 542: makemaz("Use-2"); return;
			case 543: makemaz("Use-3"); return;
			case 544: makemaz("Use-4"); return;
			case 545: makemaz("Use-5"); return;
			case 546: makemaz("Sex-1"); return;
			case 547: makemaz("Sex-2"); return;
			case 548: makemaz("Sex-3"); return;
			case 549: makemaz("Sex-4"); return;
			case 550: makemaz("Sex-5"); return;
			case 551: makemaz("Unb-1"); return;
			case 552: makemaz("Unb-2"); return;
			case 553: makemaz("Unb-3"); return;
			case 554: makemaz("Unb-4"); return;
			case 555: makemaz("Unb-5"); return;
			case 556: makemaz("Trs-1"); return;
			case 557: makemaz("Trs-2"); return;
			case 558: makemaz("Trs-3"); return;
			case 559: makemaz("Trs-4"); return;
			case 560: makemaz("Trs-5"); return;
			case 561: makemaz("Cha-1"); return;
			case 562: makemaz("Cha-2"); return;
			case 563: makemaz("Cha-3"); return;
			case 564: makemaz("Cha-4"); return;
			case 565: makemaz("Cha-5"); return;
			case 566: makemaz("Elm-1"); return;
			case 567: makemaz("Elm-2"); return;
			case 568: makemaz("Elm-3"); return;
			case 569: makemaz("Elm-4"); return;
			case 570: makemaz("Elm-5"); return;
			case 571: makemaz("Wil-1"); return;
			case 572: makemaz("Wil-2"); return;
			case 573: makemaz("Wil-3"); return;
			case 574: makemaz("Wil-4"); return;
			case 575: makemaz("Wil-5"); return;
			case 576: makemaz("Occ-1"); return;
			case 577: makemaz("Occ-2"); return;
			case 578: makemaz("Occ-3"); return;
			case 579: makemaz("Occ-4"); return;
			case 580: makemaz("Occ-5"); return;
			case 581: makemaz("Pro-1"); return;
			case 582: makemaz("Pro-2"); return;
			case 583: makemaz("Pro-3"); return;
			case 584: makemaz("Pro-4"); return;
			case 585: makemaz("Pro-5"); return;
			case 586: makemaz("Kur-1"); return;
			case 587: makemaz("Kur-2"); return;
			case 588: makemaz("Kur-3"); return;
			case 589: makemaz("Kur-4"); return;
			case 590: makemaz("Kur-5"); return;
			case 591: makemaz("For-1"); return;
			case 592: makemaz("For-2"); return;
			case 593: makemaz("For-3"); return;
			case 594: makemaz("For-4"); return;
			case 595: makemaz("For-5"); return;
			case 596: makemaz("Trc-1"); return;
			case 597: makemaz("Trc-2"); return;
			case 598: makemaz("Trc-3"); return;
			case 599: makemaz("Trc-4"); return;
			case 600: makemaz("Trc-5"); return;
			case 601: makemaz("Nuc-1"); return;
			case 602: makemaz("Nuc-2"); return;
			case 603: makemaz("Nuc-3"); return;
			case 604: makemaz("Nuc-4"); return;
			case 605: makemaz("Nuc-5"); return;
			case 606: makemaz("Sco-1"); return;
			case 607: makemaz("Sco-2"); return;
			case 608: makemaz("Sco-3"); return;
			case 609: makemaz("Sco-4"); return;
			case 610: makemaz("Sco-5"); return;
			case 611: makemaz("Fem-1"); return;
			case 612: makemaz("Fem-2"); return;
			case 613: makemaz("Fem-3"); return;
			case 614: makemaz("Fem-4"); return;
			case 615: makemaz("Fem-5"); return;
			case 616: makemaz("Hus-1"); return;
			case 617: makemaz("Hus-2"); return;
			case 618: makemaz("Hus-3"); return;
			case 619: makemaz("Hus-4"); return;
			case 620: makemaz("Hus-5"); return;
			case 621: makemaz("Acu-1"); return;
			case 622: makemaz("Acu-2"); return;
			case 623: makemaz("Acu-3"); return;
			case 624: makemaz("Acu-4"); return;
			case 625: makemaz("Acu-5"); return;
			case 626: makemaz("Mas-1"); return;
			case 627: makemaz("Mas-2"); return;
			case 628: makemaz("Mas-3"); return;
			case 629: makemaz("Mas-4"); return;
			case 630: makemaz("Mas-5"); return;
			case 631: makemaz("Gre-1"); return;
			case 632: makemaz("Gre-2"); return;
			case 633: makemaz("Gre-3"); return;
			case 634: makemaz("Gre-4"); return;
			case 635: makemaz("Gre-5"); return;
			case 636: makemaz("Cel-1"); return;
			case 637: makemaz("Cel-2"); return;
			case 638: makemaz("Cel-3"); return;
			case 639: makemaz("Cel-4"); return;
			case 640: makemaz("Cel-5"); return;
			case 641: makemaz("Wal-1"); return;
			case 642: makemaz("Wal-2"); return;
			case 643: makemaz("Wal-3"); return;
			case 644: makemaz("Wal-4"); return;
			case 645: makemaz("Wal-5"); return;
			case 646: makemaz("Soc-1"); return;
			case 647: makemaz("Soc-2"); return;
			case 648: makemaz("Soc-3"); return;
			case 649: makemaz("Soc-4"); return;
			case 650: makemaz("Soc-5"); return;
			case 651: makemaz("Dem-1"); return;
			case 652: makemaz("Dem-2"); return;
			case 653: makemaz("Dem-3"); return;
			case 654: makemaz("Dem-4"); return;
			case 655: makemaz("Dem-5"); return;
			case 656: makemaz("Dis-1"); return;
			case 657: makemaz("Dis-2"); return;
			case 658: makemaz("Dis-3"); return;
			case 659: makemaz("Dis-4"); return;
			case 660: makemaz("Dis-5"); return;
			case 661: makemaz("Sto-1"); return;
			case 662: makemaz("Sto-2"); return;
			case 663: makemaz("Sto-3"); return;
			case 664: makemaz("Sto-4"); return;
			case 665: makemaz("Sto-5"); return;
			case 666: makemaz("Mam-1"); return;
			case 667: makemaz("Mam-2"); return;
			case 668: makemaz("Mam-3"); return;
			case 669: makemaz("Mam-4"); return;
			case 670: makemaz("Mam-5"); return;
			case 671: makemaz("Jan-1"); return;
			case 672: makemaz("Jan-2"); return;
			case 673: makemaz("Jan-3"); return;
			case 674: makemaz("Jan-4"); return;
			case 675: makemaz("Jan-5"); return;
			case 676: makemaz("Emp-1"); return;
			case 677: makemaz("Emp-2"); return;
			case 678: makemaz("Emp-3"); return;
			case 679: makemaz("Emp-4"); return;
			case 680: makemaz("Emp-5"); return;
			case 681: makemaz("Psy-1"); return;
			case 682: makemaz("Psy-2"); return;
			case 683: makemaz("Psy-3"); return;
			case 684: makemaz("Psy-4"); return;
			case 685: makemaz("Psy-5"); return;
			case 686: makemaz("Qua-1"); return;
			case 687: makemaz("Qua-2"); return;
			case 688: makemaz("Qua-3"); return;
			case 689: makemaz("Qua-4"); return;
			case 690: makemaz("Qua-5"); return;
			case 691: makemaz("Cra-1"); return;
			case 692: makemaz("Cra-2"); return;
			case 693: makemaz("Cra-3"); return;
			case 694: makemaz("Cra-4"); return;
			case 695: makemaz("Cra-5"); return;
			case 696: makemaz("Wei-1"); return;
			case 697: makemaz("Wei-2"); return;
			case 698: makemaz("Wei-3"); return;
			case 699: makemaz("Wei-4"); return;
			case 700: makemaz("Wei-5"); return;
			case 701: makemaz("Xel-1"); return;
			case 702: makemaz("Xel-2"); return;
			case 703: makemaz("Xel-3"); return;
			case 704: makemaz("Xel-4"); return;
			case 705: makemaz("Xel-5"); return;
			case 706: makemaz("Yau-1"); return;
			case 707: makemaz("Yau-2"); return;
			case 708: makemaz("Yau-3"); return;
			case 709: makemaz("Yau-4"); return;
			case 710: makemaz("Yau-5"); return;
			case 711: makemaz("Sof-1"); return;
			case 712: makemaz("Sof-2"); return;
			case 713: makemaz("Sof-3"); return;
			case 714: makemaz("Sof-4"); return;
			case 715: makemaz("Sof-5"); return;
			case 716: makemaz("Ast-1"); return;
			case 717: makemaz("Ast-2"); return;
			case 718: makemaz("Ast-3"); return;
			case 719: makemaz("Ast-4"); return;
			case 720: makemaz("Ast-5"); return;
			case 721: makemaz("Sma-1"); return;
			case 722: makemaz("Sma-2"); return;
			case 723: makemaz("Sma-3"); return;
			case 724: makemaz("Sma-4"); return;
			case 725: makemaz("Sma-5"); return;
			case 726: makemaz("Cyb-1"); return;
			case 727: makemaz("Cyb-2"); return;
			case 728: makemaz("Cyb-3"); return;
			case 729: makemaz("Cyb-4"); return;
			case 730: makemaz("Cyb-5"); return;
			case 731: makemaz("Tos-1"); return;
			case 732: makemaz("Tos-2"); return;
			case 733: makemaz("Tos-3"); return;
			case 734: makemaz("Tos-4"); return;
			case 735: makemaz("Tos-5"); return;
			case 736: makemaz("Sym-1"); return;
			case 737: makemaz("Sym-2"); return;
			case 738: makemaz("Sym-3"); return;
			case 739: makemaz("Sym-4"); return;
			case 740: makemaz("Sym-5"); return;
			case 741: makemaz("Pra-1"); return;
			case 742: makemaz("Pra-2"); return;
			case 743: makemaz("Pra-3"); return;
			case 744: makemaz("Pra-4"); return;
			case 745: makemaz("Pra-5"); return;
			case 746: makemaz("Mil-1"); return;
			case 747: makemaz("Mil-2"); return;
			case 748: makemaz("Mil-3"); return;
			case 749: makemaz("Mil-4"); return;
			case 750: makemaz("Mil-5"); return;
			case 751: makemaz("Gen-1"); return;
			case 752: makemaz("Gen-2"); return;
			case 753: makemaz("Gen-3"); return;
			case 754: makemaz("Gen-4"); return;
			case 755: makemaz("Gen-5"); return;
			case 756: makemaz("Fjo-1"); return;
			case 757: makemaz("Fjo-2"); return;
			case 758: makemaz("Fjo-3"); return;
			case 759: makemaz("Fjo-4"); return;
			case 760: makemaz("Fjo-5"); return;
			case 761: makemaz("Eme-1"); return;
			case 762: makemaz("Eme-2"); return;
			case 763: makemaz("Eme-3"); return;
			case 764: makemaz("Eme-4"); return;
			case 765: makemaz("Eme-5"); return;
			case 766: makemaz("Com-1"); return;
			case 767: makemaz("Com-2"); return;
			case 768: makemaz("Com-3"); return;
			case 769: makemaz("Com-4"); return;
			case 770: makemaz("Com-5"); return;
			case 771: makemaz("Akl-1"); return;
			case 772: makemaz("Akl-2"); return;
			case 773: makemaz("Akl-3"); return;
			case 774: makemaz("Akl-4"); return;
			case 775: makemaz("Akl-5"); return;
			case 776: makemaz("Dra-1"); return;
			case 777: makemaz("Dra-2"); return;
			case 778: makemaz("Dra-3"); return;
			case 779: makemaz("Dra-4"); return;
			case 780: makemaz("Dra-5"); return;
			case 781: makemaz("Car-1"); return;
			case 782: makemaz("Car-2"); return;
			case 783: makemaz("Car-3"); return;
			case 784: makemaz("Car-4"); return;
			case 785: makemaz("Car-5"); return;
			case 786: makemaz("But-1"); return;
			case 787: makemaz("But-2"); return;
			case 788: makemaz("But-3"); return;
			case 789: makemaz("But-4"); return;
			case 790: makemaz("But-5"); return;
			case 791: makemaz("Dan-1"); return;
			case 792: makemaz("Dan-2"); return;
			case 793: makemaz("Dan-3"); return;
			case 794: makemaz("Dan-4"); return;
			case 795: makemaz("Dan-5"); return;
			case 796: makemaz("Dia-1"); return;
			case 797: makemaz("Dia-2"); return;
			case 798: makemaz("Dia-3"); return;
			case 799: makemaz("Dia-4"); return;
			case 800: makemaz("Dia-5"); return;
			case 801: makemaz("Pre-1"); return;
			case 802: makemaz("Pre-2"); return;
			case 803: makemaz("Pre-3"); return;
			case 804: makemaz("Pre-4"); return;
			case 805: makemaz("Pre-5"); return;
			case 806: makemaz("Sec-1"); return;
			case 807: makemaz("Sec-2"); return;
			case 808: makemaz("Sec-3"); return;
			case 809: makemaz("Sec-4"); return;
			case 810: makemaz("Sec-5"); return;
			case 811: makemaz("Sho-1"); return;
			case 812: makemaz("Sho-2"); return;
			case 813: makemaz("Sho-3"); return;
			case 814: makemaz("Sho-4"); return;
			case 815: makemaz("Sho-5"); return;
			case 816: makemaz("Hal-1"); return;
			case 817: makemaz("Hal-2"); return;
			case 818: makemaz("Hal-3"); return;
			case 819: makemaz("Hal-4"); return;
			case 820: makemaz("Hal-5"); return;
			case 821: makemaz("Sin-1"); return;
			case 822: makemaz("Sin-2"); return;
			case 823: makemaz("Sin-3"); return;
			case 824: makemaz("Sin-4"); return;
			case 825: makemaz("Sin-5"); return;
			case 826: makemaz("Hed-1"); return;
			case 827: makemaz("Hed-2"); return;
			case 828: makemaz("Hed-3"); return;
			case 829: makemaz("Hed-4"); return;
			case 830: makemaz("Hed-5"); return;
			case 831: makemaz("Van-1"); return;
			case 832: makemaz("Van-2"); return;
			case 833: makemaz("Van-3"); return;
			case 834: makemaz("Van-4"); return;
			case 835: makemaz("Van-5"); return;
			case 836: makemaz("Sdw-1"); return;
			case 837: makemaz("Sdw-2"); return;
			case 838: makemaz("Sdw-3"); return;
			case 839: makemaz("Sdw-4"); return;
			case 840: makemaz("Sdw-5"); return;
			case 841: makemaz("Cli-1"); return;
			case 842: makemaz("Cli-2"); return;
			case 843: makemaz("Cli-3"); return;
			case 844: makemaz("Cli-4"); return;
			case 845: makemaz("Cli-5"); return;
			case 846: makemaz("Wom-1"); return;
			case 847: makemaz("Wom-2"); return;
			case 848: makemaz("Wom-3"); return;
			case 849: makemaz("Wom-4"); return;
			case 850: makemaz("Wom-5"); return;
			case 851: makemaz("Noo-1"); return;
			case 852: makemaz("Noo-2"); return;
			case 853: makemaz("Noo-3"); return;
			case 854: makemaz("Noo-4"); return;
			case 855: makemaz("Noo-5"); return;
		}
		break;

		case 94:
		case 95:
		case 96:
		case 97:
		case 98:
		case 99:
		case 100:

		switch (rnd(69)) {

			case 1: makemaz("mazes-1"); return;
			case 2: makemaz("mazes-2"); return;
			case 3: makemaz("mazes-3"); return;
			case 4: makemaz("mazes-4"); return;
			case 5: makemaz("mazes-5"); return;
			case 6: makemaz("mazes-6"); return;
			case 7: makemaz("mazes-7"); return;
			case 8: makemaz("mazes-8"); return;
			case 9: makemaz("mazes-9"); return;
			case 10: makemaz("mazes-10"); return;
			case 11: makemaz("mazes-11"); return;
			case 12: makemaz("mazes-12"); return;
			case 13: makemaz("mazes-13"); return;
			case 14: makemaz("mazes-14"); return;
			case 15: makemaz("mazes-15"); return;
			case 16: makemaz("mazes-16"); return;
			case 17: makemaz("mazes-17"); return;
			case 18: makemaz("mazes-18"); return;
			case 19: makemaz("mazes-19"); return;
			case 20: makemaz("mazes-20"); return;
			case 21: makemaz("mazes-21"); return;
			case 22: makemaz("mazes-22"); return;
			case 23: makemaz("mazes-23"); return;
			case 24: makemaz("mazes-24"); return;
			case 25: makemaz("mazes-25"); return;
			case 26: makemaz("mazes-26"); return;
			case 27: makemaz("mazes-27"); return;
			case 28: makemaz("mazes-28"); return;
			case 29: makemaz("mazes-29"); return;
			case 30: makemaz("mazes-30"); return;
			case 31: makemaz("mazes-31"); return;
			case 32: makemaz("mazes-32"); return;
			case 33: makemaz("mazes-33"); return;
			case 34: makemaz("mazes-34"); return;
			case 35: makemaz("mazes-35"); return;
			case 36: makemaz("mazes-36"); return;
			case 37: makemaz("mazes-37"); return;
			case 38: makemaz("mazes-38"); return;
			case 39: makemaz("mazes-39"); return;
			case 40: makemaz("mazes-40"); return;
			case 41: makemaz("mazes-41"); return;
			case 42: makemaz("mazes-42"); return;
			case 43: makemaz("mazes-43"); return;
			case 44: makemaz("mazes-44"); return;
			case 45: makemaz("mazes-45"); return;
			case 46: makemaz("mazes-46"); return;
			case 47: makemaz("mazes-47"); return;
			case 48: makemaz("mazes-48"); return;
			case 49: makemaz("mazes-49"); return;
			case 50: makemaz("mazes-50"); return;
			case 51: makemaz("mazes-51"); return;
			case 52: makemaz("mazes-52"); return;
			case 53: makemaz("mazes-53"); return;
			case 54: makemaz("mazes-54"); return;
			case 55: makemaz("mazes-55"); return;
			case 56: makemaz("mazes-56"); return;
			case 57: makemaz("mazes-57"); return;
			case 58: makemaz("mazes-58"); return;
			case 59: makemaz("mazes-59"); return;
			case 60: makemaz("mazes-60"); return;
			case 61: makemaz("mazes-61"); return;
			case 62: makemaz("mazes-62"); return;
			case 63: makemaz("mazes-63"); return;
			case 64: makemaz("mazes-64"); return;
			case 65: makemaz("mazes-65"); return;
			case 66: makemaz("mazes-66"); return;
			case 67: makemaz("mazes-67"); return;
			case 68: makemaz("mazes-68"); return;
			case 69: makemaz("mazes-69"); return;

		}
		break;

		case 101:
		case 102:
		case 103:
		case 104:
		case 105:
		switch (rnd(95)) {

			case 1: makemaz("levgn-1"); return;
			case 2: makemaz("levgn-2"); return;
			case 3: makemaz("levgn-3"); return;
			case 4: makemaz("levgn-4"); return;
			case 5: makemaz("levgn-5"); return;
			case 6: makemaz("levgn-6"); return;
			case 7: makemaz("levgn-7"); return;
			case 8: makemaz("levgn-8"); return;
			case 9: makemaz("levgn-9"); return;
			case 10: makemaz("levgn-10"); return;
			case 11: makemaz("levgn-11"); return;
			case 12: makemaz("levgn-12"); return;
			case 13: makemaz("levgn-13"); return;
			case 14: makemaz("levgn-14"); return;
			case 15: makemaz("levgn-15"); return;
			case 16: makemaz("levgn-16"); return;
			case 17: makemaz("levgn-17"); return;
			case 18: makemaz("levgn-18"); return;
			case 19: makemaz("levgn-19"); return;
			case 20: makemaz("levgn-20"); return;
			case 21: makemaz("levgn-21"); return;
			case 22: makemaz("levgn-22"); return;
			case 23: makemaz("levgn-23"); return;
			case 24: makemaz("levgn-24"); return;
			case 25: makemaz("levgn-25"); return;
			case 26: makemaz("levgn-26"); return;
			case 27: makemaz("levgn-27"); return;
			case 28: makemaz("levgn-28"); return;
			case 29: makemaz("levgn-29"); return;
			case 30: makemaz("levgn-30"); return;
			case 31: makemaz("levgn-31"); return;
			case 32: makemaz("levgn-32"); return;
			case 33: makemaz("levgn-33"); return;
			case 34: makemaz("levgn-34"); return;
			case 35: makemaz("levgn-35"); return;
			case 36: makemaz("levgn-36"); return;
			case 37: makemaz("levgn-37"); return;
			case 38: makemaz("levgn-38"); return;
			case 39: makemaz("levgn-39"); return;
			case 40: makemaz("levgn-40"); return;
			case 41: makemaz("levgn-41"); return;
			case 42: makemaz("levgn-42"); return;
			case 43: makemaz("levgn-43"); return;
			case 44: makemaz("levgn-44"); return;
			case 45: makemaz("levgn-45"); return;
			case 46: makemaz("levgn-46"); return;
			case 47: makemaz("levgn-47"); return;
			case 48: makemaz("levgn-48"); return;
			case 49: makemaz("levgn-49"); return;
			case 50: makemaz("levgn-50"); return;
			case 51: makemaz("levgn-51"); return;
			case 52: makemaz("levgn-52"); return;
			case 53: makemaz("levgn-53"); return;
			case 54: makemaz("levgn-54"); return;
			case 55: makemaz("levgn-55"); return;
			case 56: makemaz("levgn-56"); return;
			case 57: makemaz("levgn-57"); return;
			case 58: makemaz("levgn-58"); return;
			case 59: makemaz("levgn-59"); return;
			case 60: makemaz("levgn-60"); return;
			case 61: makemaz("levgn-61"); return;
			case 62: makemaz("levgn-62"); return;
			case 63: makemaz("levgn-63"); return;
			case 64: makemaz("levgn-64"); return;
			case 65: makemaz("levgn-65"); return;
			case 66: makemaz("levgn-66"); return;
			case 67: makemaz("levgn-67"); return;
			case 68: makemaz("levgn-68"); return;
			case 69: makemaz("levgn-69"); return;
			case 70: makemaz("levgn-70"); return;
			case 71: makemaz("levgn-71"); return;
			case 72: makemaz("levgn-72"); return;
			case 73: makemaz("levgn-73"); return;
			case 74: makemaz("levgn-74"); return;
			case 75: makemaz("levgn-75"); return;
			case 76: makemaz("levgn-76"); return;
			case 77: makemaz("levgn-77"); return;
			case 78: makemaz("levgn-78"); return;
			case 79: makemaz("levgn-79"); return;
			case 80: makemaz("levgn-80"); return;
			case 81: makemaz("levgn-81"); return;
			case 82: makemaz("levgn-82"); return;
			case 83: makemaz("levgn-83"); return;
			case 84: makemaz("levgn-84"); return;
			case 85: makemaz("levgn-85"); return;
			case 86: makemaz("levgn-86"); return;
			case 87: makemaz("levgn-87"); return;
			case 88: makemaz("levgn-88"); return;
			case 89: makemaz("levgn-89"); return;
			case 90: makemaz("levgn-90"); return;
			case 91: makemaz("levgn-91"); return;
			case 92: makemaz("levgn-92"); return;
			case 93: makemaz("levgn-93"); return;
			case 94: makemaz("levgn-94"); return;
			case 95: makemaz("levgn-95"); return;

		}
	    case 106:

		makemaz("ecav2"); return;
		break;

	    case 107:

		makemaz("efrnk"); return;
		break;

	    case 108:
	    case 109:
	    case 110:

		switch (rnd(8)) {

			case 1: {
				switch (rnd(5)) {

					case 1: makemaz("cowla-1"); return;
					case 2: makemaz("cowla-2"); return;
					case 3: makemaz("cowla-3"); return;
					case 4: makemaz("cowla-4"); return;
					case 5: makemaz("cowla-5"); return;

				}
				break;
			}
			case 2: makemaz("cowlb"); return;
			case 3: makemaz("cowlc"); return;
			case 4: makemaz("cowld"); return;
			case 5: makemaz("cowle"); return;
			case 6: makemaz("cowlf"); return;
			case 7: makemaz("cowlg"); return;
			case 8: makemaz("cowlh"); return;

		}

		break;

	    case 111:
		switch (rnd(15)) {
			case 1: makemaz("deepfila"); return;
			case 2: makemaz("deepfilb"); return;
			case 3: makemaz("deepfilc"); return;
			case 4: makemaz("deepfild"); return;
			case 5: makemaz("deepfile"); return;
			case 6: makemaz("deepfilf"); return;
			case 7: makemaz("deepfilg"); return;
			case 8: makemaz("deepfilh"); return;
			case 9: makemaz("deepfili"); return;
			case 10: makemaz("deepfilj"); return;
			case 11: makemaz("deepfilk"); return;
			case 12: makemaz("deepfill"); return;
			case 13: makemaz("deepfilm"); return;
			case 14: makemaz("deepfiln"); return;
			case 15: makemaz("deepfilo"); return;

		}

		break;

	    case 112:
		switch (rnd(10)) {
			case 1: makemaz("deptn-1"); return;
			case 2: makemaz("deptn-2"); return;
			case 3: makemaz("deptn-3"); return;
			case 4: makemaz("deptn-4"); return;
			case 5: makemaz("deptn-5"); return;
			case 6: makemaz("deptn-6"); return;
			case 7: makemaz("deptn-7"); return;
			case 8: makemaz("deptn-8"); return;
			case 9: makemaz("deptn-9"); return;
			case 10: makemaz("deptn-10"); return;

		}

		break;

	    case 113:
		switch (rnd(6)) {
			case 1: makemaz("depeE-1"); return;
			case 2: makemaz("depeE-2"); return;
			case 3: makemaz("depeE-3"); return;
			case 4: makemaz("depeE-4"); return;
			case 5: makemaz("depeE-5"); return;
			case 6: makemaz("depeE-6"); return;

		}

		break;

	    case 114:
		switch (rnd(5)) {
			case 1: makemaz("gruelaiX"); return;
			case 2: makemaz("joustX"); return;
			case 3: makemaz("pmazeX"); return;
			case 4: makemaz("poolhalX"); return;
			case 5: makemaz("dmazeX"); return;

		}

		break;

	    case 115:
	    case 116:

		switch (rnd(8)) {

			case 1: makemaz("grcra"); return;
			case 2: makemaz("grcrb"); return;
			case 3: makemaz("grcrc"); return;
			case 4: makemaz("grcrd"); return;
			case 5: makemaz("grcre"); return;
			case 6: makemaz("grcrf"); return;
			case 7: makemaz("grcrg"); return;
			case 8: makemaz("grcrh"); return;

		}
		break;

	    case 117:
		switch (rnd(3)) {
			case 1: makemaz("eiceqa"); return;
			case 2: makemaz("eiceqb"); return;
			case 3: makemaz("eiceqc"); return;

		}

		break;
	
          }

		} else if (In_gehennom(&u.uz) || In_sheol(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz) || In_angmar(&u.uz) || In_emynluin(&u.uz) || In_swimmingpool(&u.uz) || In_hellbathroom(&u.uz) || !strcmp(dungeons[u.uz.dnum].dname, "Frankenstein's Lab")) {

		if (rn2(3)) goto ghnhom4;

	    switch (rnd(117)) {

	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5:
	    case 6:
	    case 7:
	    case 8:
	    case 9:
	    case 10:

		switch (rnd(60)) {

			case 1: makemaz("ghbgr-1"); return;
			case 2: makemaz("ghbgr-2"); return;
			case 3: makemaz("ghbgr-3"); return;
			case 4: makemaz("ghbgr-4"); return;
			case 5: makemaz("ghbgr-5"); return;
			case 6: makemaz("ghbgr-6"); return;
			case 7: makemaz("ghbgr-7"); return;
			case 8: makemaz("ghbgr-8"); return;
			case 9: makemaz("ghbgr-9"); return;
			case 10: makemaz("ghbgr-10"); return;
			case 11: makemaz("ghbgr-11"); return;
			case 12: makemaz("ghbgr-12"); return;
			case 13: makemaz("ghbgr-13"); return;
			case 14: makemaz("ghbgr-14"); return;
			case 15: makemaz("ghbgr-15"); return;
			case 16: makemaz("ghbgr-16"); return;
			case 17: makemaz("ghbgr-17"); return;
			case 18: makemaz("ghbgr-18"); return;
			case 19: makemaz("ghbgr-19"); return;
			case 20: makemaz("ghbgr-20"); return;
			case 21: makemaz("ghbgr-21"); return;
			case 22: makemaz("ghbgr-22"); return;
			case 23: makemaz("ghbgr-23"); return;
			case 24: makemaz("ghbgr-24"); return;
			case 25: makemaz("ghbgr-25"); return;
			case 26: makemaz("ghbgr-26"); return;
			case 27: makemaz("ghbgr-27"); return;
			case 28: makemaz("ghbgr-28"); return;
			case 29: makemaz("ghbgr-29"); return;
			case 30: makemaz("ghbgr-30"); return;
			case 31: makemaz("ghbgr-31"); return;
			case 32: makemaz("ghbgr-32"); return;
			case 33: makemaz("ghbgr-33"); return;
			case 34: makemaz("ghbgr-34"); return;
			case 35: makemaz("ghbgr-35"); return;
			case 36: makemaz("ghbgr-36"); return;
			case 37: makemaz("ghbgr-37"); return;
			case 38: makemaz("ghbgr-38"); return;
			case 39: makemaz("ghbgr-39"); return;
			case 40: makemaz("ghbgr-40"); return;
			case 41: makemaz("ghbgr-41"); return;
			case 42: makemaz("ghbgr-42"); return;
			case 43: makemaz("ghbgr-43"); return;
			case 44: makemaz("ghbgr-44"); return;
			case 45: makemaz("ghbgr-45"); return;
			case 46: makemaz("ghbgr-46"); return;
			case 47: makemaz("ghbgr-47"); return;
			case 48: makemaz("ghbgr-48"); return;
			case 49: makemaz("ghbgr-49"); return;
			case 50: makemaz("ghbgr-50"); return;
			case 51: makemaz("ghbgr-51"); return;
			case 52: makemaz("ghbgr-52"); return;
			case 53: makemaz("ghbgr-53"); return;
			case 54: makemaz("ghbgr-54"); return;
			case 55: makemaz("ghbgr-55"); return;
			case 56: makemaz("ghbgr-56"); return;
			case 57: makemaz("ghbgr-57"); return;
			case 58: makemaz("ghbgr-58"); return;
			case 59: makemaz("ghbgr-59"); return;
			case 60: makemaz("ghbgr-60"); return;

		}
		break;

	    case 11:
	    case 12:
	    case 13:
	    case 14:
	    case 15:
	    case 16:
	    case 17:
	    case 18:
	    case 19:
	    case 20:

		switch (rnd(97)) {

			case 1: makemaz("heck-1"); return;
			case 2: makemaz("heck-2"); return;
			case 3: makemaz("heck-3"); return;
			case 4: makemaz("heck-4"); return;
			case 5: makemaz("heck-5"); return;
			case 6: makemaz("heck-6"); return;
			case 7: makemaz("heck-7"); return;
			case 8: makemaz("heck-8"); return;
			case 9: makemaz("heck-9"); return;
			case 10: makemaz("heck-10"); return;
			case 11: makemaz("heck-11"); return;
			case 12: makemaz("heck-12"); return;
			case 13: makemaz("heck-13"); return;
			case 14: makemaz("heck-14"); return;
			case 15: makemaz("heck-15"); return;
			case 16: makemaz("heck-16"); return;
			case 17: makemaz("heck-17"); return;
			case 18: makemaz("heck-18"); return;
			case 19: makemaz("heck-19"); return;
			case 20: makemaz("heck-20"); return;
			case 21: makemaz("heck-21"); return;
			case 22: makemaz("heck-22"); return;
			case 23: makemaz("heck-23"); return;
			case 24: makemaz("heck-24"); return;
			case 25: makemaz("heck-25"); return;
			case 26: makemaz("heck-26"); return;
			case 27: makemaz("heck-27"); return;
			case 28: makemaz("heck-28"); return;
			case 29: makemaz("heck-29"); return;
			case 30: makemaz("heck-30"); return;
			case 31: makemaz("heck-31"); return;
			case 32: makemaz("heck-32"); return;
			case 33: makemaz("heck-33"); return;
			case 34: makemaz("heck-34"); return;
			case 35: makemaz("heck-35"); return;
			case 36: makemaz("heck-36"); return;
			case 37: makemaz("heck-37"); return;
			case 38: makemaz("heck-38"); return;
			case 39: makemaz("heck-39"); return;
			case 40: makemaz("heck-40"); return;
			case 41: makemaz("heck-41"); return;
			case 42: makemaz("heck-42"); return;
			case 43: makemaz("heck-43"); return;
			case 44: makemaz("heck-44"); return;
			case 45: makemaz("heck-45"); return;
			case 46: makemaz("heck-46"); return;
			case 47: makemaz("heck-47"); return;
			case 48: makemaz("heck-48"); return;
			case 49: makemaz("heck-49"); return;
			case 50: makemaz("heck-50"); return;
			case 51: makemaz("heck-51"); return;
			case 52: makemaz("heck-52"); return;
			case 53: makemaz("heck-53"); return;
			case 54: makemaz("heck-54"); return;
			case 55: makemaz("heck-55"); return;
			case 56: makemaz("heck-56"); return;
			case 57: makemaz("heck-57"); return;
			case 58: makemaz("heck-58"); return;
			case 59: makemaz("heck-59"); return;
			case 60: makemaz("heck-60"); return;
			case 61: makemaz("heck-61"); return;
			case 62: makemaz("heck-62"); return;
			case 63: makemaz("heck-63"); return;
			case 64: makemaz("heck-64"); return;
			case 65: makemaz("heck-65"); return;
			case 66: makemaz("heck-66"); return;
			case 67: makemaz("heck-67"); return;
			case 68: makemaz("heck-68"); return;
			case 69: makemaz("heck-69"); return;
			case 70: makemaz("heck-70"); return;
			case 71: makemaz("heck-71"); return;
			case 72: makemaz("heck-72"); return;
			case 73: makemaz("heck-73"); return;
			case 74: makemaz("heck-74"); return;
			case 75: makemaz("heck-75"); return;
			case 76: makemaz("heck-76"); return;
			case 77: makemaz("heck-77"); return;
			case 78: makemaz("heck-78"); return;
			case 79: makemaz("heck-79"); return;
			case 80: makemaz("heck-80"); return;
			case 81: makemaz("heck-81"); return;
			case 82: makemaz("heck-82"); return;
			case 83: makemaz("heck-83"); return;
			case 84: makemaz("heck-84"); return;
			case 85: makemaz("heck-85"); return;
			case 86: makemaz("heck-86"); return;
			case 87: makemaz("heck-87"); return;
			case 88: makemaz("heck-88"); return;
			case 89: makemaz("heck-89"); return;
			case 90: makemaz("heck-90"); return;
			case 91: makemaz("heck-91"); return;
			case 92: makemaz("heck-92"); return;
			case 93: makemaz("heck-93"); return;
			case 94: makemaz("heck-94"); return;
			case 95: makemaz("heck-95"); return;
			case 96: makemaz("heck-96"); return;
			case 97: makemaz("minusw"); return;

		}
		break;

	    case 21:

		switch (rnd(5)) {

			case 1: makemaz("intplx-1"); return;
			case 2: makemaz("intplx-2"); return;
			case 3: makemaz("intplx-3"); return;
			case 4: makemaz("intplx-4"); return;
			case 5: makemaz("intplx-5"); return;

		}
		break;

	    case 22:
	    case 23:
	    case 24:

		switch (rnd(14)) {

			case 1: makemaz("hellfill"); return;
			case 2: makemaz("hellfila"); return;
			case 3: makemaz("hellfilb"); return;
			case 4: makemaz("hellfilc"); return;
			case 5: makemaz("hellfild"); return;
			case 6: makemaz("hellfile"); return;
			case 7: makemaz("hellfilf"); return;
			case 8: makemaz("hellfilg"); return;
			case 9: makemaz("hellfill"); return;
			case 10: makemaz("hellfill"); return;
			case 11: makemaz("hellfill"); return;
			case 12: makemaz("hellfill"); return;
			case 13: makemaz("hellfill"); return;
			case 14: makemaz("hellfill"); return;

		}
		break;

	    case 25:
	    case 26:
	    case 27:
	    case 28:
	    case 29:

		switch (rnd(21)) {

			case 1: makemaz("hmint-1"); return;
			case 2: makemaz("hmint-2"); return;
			case 3: makemaz("hmint-3"); return;
			case 4: makemaz("hmint-4"); return;
			case 5: makemaz("hmint-5"); return;
			case 6: makemaz("hmint-6"); return;
			case 7: makemaz("hmint-7"); return;
			case 8: makemaz("hmint-8"); return;
			case 9: makemaz("hmint-9"); return;
			case 10: makemaz("hmint-10"); return;
			case 11: makemaz("hmint-11"); return;
			case 12: makemaz("hmint-12"); return;
			case 13: makemaz("hmint-13"); return;
			case 14: makemaz("hmint-14"); return;
			case 15: makemaz("hmint-15"); return;
			case 16: makemaz("hmint-16"); return;
			case 17: makemaz("hmint-17"); return;
			case 18: makemaz("hmint-18"); return;
			case 19: makemaz("hmint-19"); return;
			case 20: makemaz("hmint-20"); return;
			case 21: makemaz("hmint-21"); return;

		}
		break;

	    case 30:
	    case 31:

		switch (rnd(10)) {

			case 1: makemaz("hmnen-1"); return;
			case 2: makemaz("hmnen-2"); return;
			case 3: makemaz("hmnen-3"); return;
			case 4: makemaz("hmnen-4"); return;
			case 5: makemaz("hmnen-5"); return;
			case 6: makemaz("hmnen-6"); return;
			case 7: makemaz("hmnen-7"); return;
			case 8: makemaz("hmnen-8"); return;
			case 9: makemaz("hmnen-9"); return;
			case 10: makemaz("hmnen-10"); return;

		}
		break;

	    case 32:
	    case 33:

		switch (rnd(18)) {

			case 1: makemaz("hking-1"); return;
			case 2: makemaz("hking-2"); return;
			case 3: makemaz("hking-3"); return;
			case 4: makemaz("hking-4"); return;
			case 5: makemaz("hking-5"); return;
			case 6: makemaz("hking-6"); return;
			case 7: makemaz("hking-7"); return;
			case 8: makemaz("hking-8"); return;
			case 9: makemaz("hking-9"); return;
			case 10: makemaz("hking-10"); return;
			case 11: makemaz("hking-11"); return;
			case 12: makemaz("hking-12"); return;
			case 13: makemaz("hking-13"); return;
			case 14: makemaz("hking-14"); return;
			case 15: makemaz("hking-15"); return;
			case 16: makemaz("hking-16"); return;
			case 17: makemaz("hking-17"); return;
			case 18: makemaz("hking-18"); return;

		}
		break;

	    case 34:
	    case 35:
	    case 36:
	    case 37:
	    case 38:
	    case 39:
	    case 40:
	    case 41:
	    case 42:
	    case 43:

		switch (rnd(154)) {

			case 1: makemaz("soko8-1"); return;
			case 2: makemaz("soko8-2"); return;
			case 3: makemaz("soko8-3"); return;
			case 4: makemaz("soko8-4"); return;
			case 5: makemaz("soko8-5"); return;
			case 6: makemaz("soko8-6"); return;
			case 7: makemaz("soko8-7"); return;
			case 8: makemaz("soko8-8"); return;
			case 9: makemaz("soko8-9"); return;
			case 10: makemaz("soko8-10"); return;
			case 11: makemaz("soko8-11"); return;
			case 12: makemaz("soko8-12"); return;
			case 13: makemaz("soko8-13"); return;
			case 14: makemaz("soko8-14"); return;
			case 15: makemaz("soko8-15"); return;
			case 16: makemaz("soko8-16"); return;
			case 17: makemaz("soko8-17"); return;
			case 18: makemaz("soko8-18"); return;
			case 19: makemaz("soko8-19"); return;
			case 20: makemaz("soko8-20"); return;
			case 21: makemaz("soko8-21"); return;
			case 22: makemaz("soko8-22"); return;
			case 23: makemaz("soko8-23"); return;
			case 24: makemaz("soko8-24"); return;
			case 25: makemaz("soko8-25"); return;
			case 26: makemaz("soko8-26"); return;
			case 27: makemaz("soko8-27"); return;
			case 28: makemaz("soko8-28"); return;
			case 29: makemaz("soko8-29"); return;
			case 30: makemaz("soko8-30"); return;
			case 31: makemaz("soko8-31"); return;
			case 32: makemaz("soko8-32"); return;
			case 33: makemaz("soko8-33"); return;
			case 34: makemaz("soko8-34"); return;
			case 35: makemaz("soko8-35"); return;
			case 36: makemaz("soko8-36"); return;
			case 37: makemaz("soko8-37"); return;
			case 38: makemaz("soko8-38"); return;
			case 39: makemaz("soko8-39"); return;
			case 40: makemaz("soko8-40"); return;
			case 41: makemaz("soko8-41"); return;
			case 42: makemaz("soko8-42"); return;
			case 43: makemaz("soko8-43"); return;
			case 44: makemaz("soko8-44"); return;

			case 45: makemaz("soko7-1"); return;
			case 46: makemaz("soko7-2"); return;
			case 47: makemaz("soko7-3"); return;
			case 48: makemaz("soko7-4"); return;
			case 49: makemaz("soko7-5"); return;
			case 50: makemaz("soko7-6"); return;
			case 51: makemaz("soko7-7"); return;
			case 52: makemaz("soko7-8"); return;
			case 53: makemaz("soko7-9"); return;
			case 54: makemaz("soko7-10"); return;
			case 55: makemaz("soko7-11"); return;
			case 56: makemaz("soko7-12"); return;
			case 57: makemaz("soko7-13"); return;
			case 58: makemaz("soko7-14"); return;
			case 59: makemaz("soko7-15"); return;
			case 60: makemaz("soko7-16"); return;
			case 61: makemaz("soko7-17"); return;
			case 62: makemaz("soko7-18"); return;
			case 63: makemaz("soko7-19"); return;
			case 64: makemaz("soko7-20"); return;
			case 65: makemaz("soko7-21"); return;
			case 66: makemaz("soko7-22"); return;
			case 67: makemaz("soko7-23"); return;
			case 68: makemaz("soko7-24"); return;
			case 69: makemaz("soko7-25"); return;
			case 70: makemaz("soko7-26"); return;
			case 71: makemaz("soko7-27"); return;
			case 72: makemaz("soko7-28"); return;
			case 73: makemaz("soko7-29"); return;

			case 74: makemaz("soko9-1"); return;
			case 75: makemaz("soko9-2"); return;
			case 76: makemaz("soko9-3"); return;
			case 77: makemaz("soko9-4"); return;
			case 78: makemaz("soko9-5"); return;
			case 79: makemaz("soko9-6"); return;
			case 80: makemaz("soko9-7"); return;
			case 81: makemaz("soko9-8"); return;
			case 82: makemaz("soko9-9"); return;
			case 83: makemaz("soko9-10"); return;
			case 84: makemaz("soko9-11"); return;
			case 85: makemaz("soko9-12"); return;
			case 86: makemaz("soko9-13"); return;
			case 87: makemaz("soko9-14"); return;
			case 88: makemaz("soko9-15"); return;
			case 89: makemaz("soko9-16"); return;
			case 90: makemaz("soko9-17"); return;
			case 91: makemaz("soko9-18"); return;
			case 92: makemaz("soko9-19"); return;
			case 93: makemaz("soko9-20"); return;
			case 94: makemaz("soko9-21"); return;
			case 95: makemaz("soko9-22"); return;
			case 96: makemaz("soko9-23"); return;
			case 97: makemaz("soko9-24"); return;
			case 98: makemaz("soko9-25"); return;
			case 99: makemaz("soko9-26"); return;
			case 100: makemaz("soko9-27"); return;
			case 101: makemaz("soko9-28"); return;

			case 102: makemaz("soko0-1"); return;
			case 103: makemaz("soko0-2"); return;
			case 104: makemaz("soko0-3"); return;
			case 105: makemaz("soko0-4"); return;
			case 106: makemaz("soko0-5"); return;
			case 107: makemaz("soko0-6"); return;
			case 108: makemaz("soko0-7"); return;
			case 109: makemaz("soko0-8"); return;
			case 110: makemaz("soko0-9"); return;
			case 111: makemaz("soko0-10"); return;
			case 112: makemaz("soko0-11"); return;
			case 113: makemaz("soko0-12"); return;
			case 114: makemaz("soko0-13"); return;
			case 115: makemaz("soko0-14"); return;
			case 116: makemaz("soko0-15"); return;
			case 117: makemaz("soko0-16"); return;
			case 118: makemaz("soko0-17"); return;
			case 119: makemaz("soko0-18"); return;
			case 120: makemaz("soko0-19"); return;
			case 121: makemaz("soko0-20"); return;
			case 122: makemaz("soko0-21"); return;
			case 123: makemaz("soko0-22"); return;
			case 124: makemaz("soko0-23"); return;
			case 125: makemaz("soko0-24"); return;
			case 126: makemaz("soko0-25"); return;
			case 127: makemaz("soko0-26"); return;
			case 128: makemaz("soko0-27"); return;
			case 129: makemaz("soko0-28"); return;
			case 130: makemaz("soko0-29"); return;
			case 131: makemaz("soko0-30"); return;
			case 132: makemaz("soko0-31"); return;
			case 133: makemaz("soko0-32"); return;
			case 134: makemaz("soko0-33"); return;
			case 135: makemaz("soko0-34"); return;
			case 136: makemaz("soko0-35"); return;
			case 137: makemaz("soko0-36"); return;
			case 138: makemaz("soko0-37"); return;
			case 139: makemaz("soko0-38"); return;
			case 140: makemaz("soko0-39"); return;
			case 141: makemaz("soko0-40"); return;
			case 142: makemaz("soko0-41"); return;
			case 143: makemaz("soko0-42"); return;
			case 144: makemaz("soko0-43"); return;
			case 145: makemaz("soko0-44"); return;
			case 146: makemaz("soko0-45"); return;
			case 147: makemaz("soko0-46"); return;
			case 148: makemaz("soko0-47"); return;
			case 149: makemaz("soko0-48"); return;
			case 150: makemaz("soko0-49"); return;
			case 151: makemaz("soko0-50"); return;
			case 152: makemaz("soko0-51"); return;

			case 153: makemaz("soko8-45"); return;

			case 154: makemaz("soko0-52"); return;

		}
		break;

	    case 44:
	    case 45:

		switch (rnd(10)) {

			case 1: makemaz("hllma-1"); return;
			case 2: makemaz("hllma-2"); return;
			case 3: makemaz("hllma-3"); return;
			case 4: makemaz("hllma-4"); return;
			case 5: makemaz("hllma-5"); return;
			case 6: makemaz("hllma-6"); return;
			case 7: makemaz("hllma-7"); return;
			case 8: makemaz("hllma-8"); return;
			case 9: makemaz("hllma-9"); return;
			case 10: makemaz("hllma-10"); return;

		}
		break;

	    case 46:

		makemaz("oracleX"); return;
		break;

	    case 47:

		switch (rnd(4)) {

			case 1: makemaz("hrats-1"); return;
			case 2: makemaz("hrats-2"); return;
			case 3: makemaz("hrats-3"); return;
			case 4: makemaz("hrats-4"); return;

		}
		break;

	    case 48:

		switch (rnd(6)) {

			case 1: makemaz("hkobol-1"); return;
			case 2: makemaz("hkobol-2"); return;
			case 3: makemaz("hkobol-3"); return;
			case 4: makemaz("hkobol-4"); return;
			case 5: makemaz("hkobol-5"); return;
			case 6: makemaz("hkobol-6"); return;

		}
		break;

	    case 49:

		switch (rnd(5)) {

			case 1: makemaz("hnymp-1"); return;
			case 2: makemaz("hnymp-2"); return;
			case 3: makemaz("hnymp-3"); return;
			case 4: makemaz("hnymp-4"); return;
			case 5: makemaz("hnymp-5"); return;

		}
		break;

	    case 50:

		switch (rnd(9)) {

			case 1: makemaz("hstor-1"); return;
			case 2: makemaz("hstor-2"); return;
			case 3: makemaz("hstor-3"); return;
			case 4: makemaz("hstor-4"); return;
			case 5: makemaz("hstor-5"); return;
			case 6: makemaz("hstor-6"); return;
			case 7: makemaz("hstor-7"); return;
			case 8: makemaz("hstor-8"); return;
			case 9: makemaz("hstor-9"); return;

		}
		break;

	    case 51:

		switch (rnd(8)) {

			case 1: makemaz("guild-1"); return;
			case 2: makemaz("guild-2"); return;
			case 3: makemaz("guild-3"); return;
			case 4: makemaz("guild-4"); return;
			case 5: makemaz("guild-5"); return;
			case 6: makemaz("guild-6"); return;
			case 7: makemaz("guild-7"); return;
			case 8: makemaz("guild-8"); return;

		}
		break;

	    case 52:

		switch (rnd(9)) {

			case 1: makemaz("forgeX"); return;
			case 2: makemaz("hitchX"); return;
			case 3: makemaz("compuX"); return;
			case 4: makemaz("keyX"); return;
			case 5: makemaz("robtnH"); return;
			case 6: makemaz("rabhoH"); return;
			case 7: makemaz("machnH"); return;
			case 8: makemaz("orcbaH"); return;
			case 9: makemaz("sewplH"); return;

		}
		break;

	    case 53:
	    case 54:

		switch (rnd(12)) {

			case 1: makemaz("hmedu-1"); return;
			case 2: makemaz("hmedu-2"); return;
			case 3: makemaz("hmedu-3"); return;
			case 4: makemaz("hmedu-4"); return;
			case 5: makemaz("hmedu-5"); return;
			case 6: makemaz("hmedu-6"); return;
			case 7: makemaz("hmedu-7"); return;
			case 8: makemaz("hmedu-8"); return;
			case 9: makemaz("hmedu-9"); return;
			case 10: makemaz("hmedu-10"); return;
			case 11: makemaz("hmedu-11"); return;
			case 12: makemaz("hmedu-12"); return;

		}
		break;

	    case 55:
	    case 56:

		switch (rnd(15)) {

			case 1: makemaz("hcstl-1"); return;
			case 2: makemaz("hcstl-2"); return;
			case 3: makemaz("hcstl-3"); return;
			case 4: makemaz("hcstl-4"); return;
			case 5: makemaz("hcstl-5"); return;
			case 6: makemaz("hcstl-6"); return;
			case 7: makemaz("hcstl-7"); return;
			case 8: makemaz("hcstl-8"); return;
			case 9: makemaz("hcstl-9"); return;
			case 10: makemaz("hcstl-10"); return;
			case 11: makemaz("hcstl-11"); return;
			case 12: makemaz("hcstl-12"); return;
			case 13: makemaz("hcstl-13"); return;
			case 14: makemaz("hcstl-14"); return;
			case 15: makemaz("hcstl-15"); return;

		}
		break;

	    case 57:
	    case 58:
	    case 59:
	    case 60:
	    case 61:

		switch (rnd(88)) {

			case 1: makemaz("hgehn-1"); return;
			case 2: makemaz("hgehn-2"); return;
			case 3: makemaz("hgehn-3"); return;
			case 4: makemaz("hgehn-4"); return;
			case 5: makemaz("hgehn-5"); return;
			case 6: makemaz("hgehn-6"); return;
			case 7: makemaz("hgehn-7"); return;
			case 8: makemaz("hgehn-8"); return;
			case 9: makemaz("hgehn-9"); return;
			case 10: makemaz("hgehn-10"); return;
			case 11: makemaz("hgehn-11"); return;
			case 12: makemaz("hgehn-12"); return;
			case 13: makemaz("hgehn-13"); return;
			case 14: makemaz("hgehn-14"); return;
			case 15: makemaz("hgehn-15"); return;
			case 16: makemaz("hgehn-16"); return;
			case 17: makemaz("hgehn-17"); return;
			case 18: makemaz("hgehn-18"); return;
			case 19: makemaz("hgehn-19"); return;
			case 20: makemaz("hgehn-20"); return;
			case 21: makemaz("hgehn-21"); return;
			case 22: makemaz("hgehn-22"); return;
			case 23: makemaz("hgehn-23"); return;
			case 24: makemaz("hgehn-24"); return;
			case 25: makemaz("hgehn-25"); return;
			case 26: makemaz("hgehn-26"); return;
			case 27: makemaz("hgehn-27"); return;
			case 28: makemaz("hgehn-28"); return;
			case 29: makemaz("hgehn-29"); return;
			case 30: makemaz("hgehn-30"); return;
			case 31: makemaz("hgehn-31"); return;
			case 32: makemaz("hgehn-32"); return;
			case 33: makemaz("hgehn-33"); return;
			case 34: makemaz("hgehn-34"); return;
			case 35: makemaz("hgehn-35"); return;
			case 36: makemaz("hgehn-36"); return;
			case 37: makemaz("hgehn-37"); return;
			case 38: makemaz("hgehn-38"); return;
			case 39: makemaz("hgehn-39"); return;
			case 40: makemaz("hgehn-40"); return;
			case 41: makemaz("hgehn-41"); return;
			case 42: makemaz("hgehn-42"); return;
			case 43: makemaz("hgehn-43"); return;
			case 44: makemaz("hgehn-44"); return;
			case 45: makemaz("hgehn-45"); return;
			case 46: makemaz("hgehn-46"); return;
			case 47: makemaz("hgehn-47"); return;
			case 48: makemaz("hgehn-48"); return;
			case 49: makemaz("hgehn-49"); return;
			case 50: makemaz("hgehn-50"); return;
			case 51: makemaz("hgehn-51"); return;
			case 52: makemaz("hgehn-52"); return;
			case 53: makemaz("hgehn-53"); return;
			case 54: makemaz("hgehn-54"); return;
			case 55: makemaz("hgehn-55"); return;
			case 56: makemaz("hgehn-56"); return;
			case 57: makemaz("hgehn-57"); return;
			case 58: makemaz("hgehn-58"); return;
			case 59: makemaz("hgehn-59"); return;
			case 60: makemaz("hgehn-60"); return;
			case 61: makemaz("hgehn-61"); return;
			case 62: makemaz("hgehn-62"); return;
			case 63: makemaz("hgehn-63"); return;
			case 64: makemaz("hgehn-64"); return;
			case 65: makemaz("hgehn-65"); return;
			case 66: makemaz("hgehn-66"); return;
			case 67: makemaz("hgehn-67"); return;
			case 68: makemaz("hgehn-68"); return;
			case 69: makemaz("hgehn-69"); return;
			case 70: makemaz("hgehn-70"); return;
			case 71: makemaz("hgehn-71"); return;
			case 72: makemaz("hgehn-72"); return;
			case 73: makemaz("hgehn-73"); return;
			case 74: makemaz("hgehn-74"); return;
			case 75: makemaz("hgehn-75"); return;
			case 76: makemaz("hgehn-76"); return;
			case 77: makemaz("hgehn-77"); return;
			case 78: makemaz("hgehn-78"); return;
			case 79: makemaz("hgehn-79"); return;
			case 80: makemaz("hgehn-80"); return;
			case 81: makemaz("hgehn-81"); return;
			case 82: makemaz("hgehn-82"); return;
			case 83: makemaz("hgehn-83"); return;
			case 84: makemaz("hgehn-84"); return;
			case 85: makemaz("hgehn-85"); return;
			case 86: makemaz("hgehn-86"); return;
			case 87: makemaz("hgehn-87"); return;
			case 88: makemaz("hgehn-88"); return;

		}
		break;

	    case 62:

		makemaz("schoolX"); return;
		break;

	    case 63:

		switch (rnd(4)) {

			case 1: makemaz("htown-1"); return;
			case 2: makemaz("htown-2"); return;
			case 3: makemaz("htown-3"); return;
			case 4: makemaz("htown-4"); return;

		}
		break;

	    case 64:

		switch (rnd(3)) {

			case 1: makemaz("hgrund-1"); return;
			case 2: makemaz("hgrund-2"); return;
			case 3: makemaz("hgrund-3"); return;

		}
		break;

	    case 65:

		switch (rnd(3)) {

			case 1: makemaz("hknox-1"); return;
			case 2: makemaz("hknox-2"); return;
			case 3: makemaz("hknox-3"); return;

		}
		break;

	    case 66:

		makemaz("hdragons"); return;
		break;

	    case 67:

		makemaz("htomb"); return;
		break;

	    case 68:

		makemaz("hspiders"); return;
		break;

	    case 69:

		makemaz("hsea"); return;
		break;

	    case 70:

		makemaz("hmtemple"); return;
		break;

	    case 71:
	    case 72:
	    case 73:

		switch (rnd(22)) {

			case 1: makemaz("hsheo-1"); return;
			case 2: makemaz("hsheo-2"); return;
			case 3: makemaz("hsheo-3"); return;
			case 4: makemaz("hsheo-4"); return;
			case 5: makemaz("hsheo-5"); return;
			case 6: makemaz("hsheo-6"); return;
			case 7: makemaz("hsheo-7"); return;
			case 8: makemaz("hsheo-8"); return;
			case 9: makemaz("hsheo-9"); return;
			case 10: makemaz("hsheo-10"); return;
			case 11: makemaz("hsheo-11"); return;
			case 12: makemaz("hsheo-12"); return;
			case 13: makemaz("hsheo-13"); return;
			case 14: makemaz("hsheo-14"); return;
			case 15: makemaz("hsheo-15"); return;
			case 16: makemaz("hsheo-16"); return;
			case 17: makemaz("hsheo-17"); return;
			case 18: makemaz("hsheo-18"); return;
			case 19: makemaz("hsheo-19"); return;
			case 20: makemaz("hsheo-20"); return;
			case 21: makemaz("hsheo-21"); return;
			case 22: makemaz("hsheo-22"); return;

		}
		break;

	    case 74:
	    case 75:
	    case 76:
	    case 77:
	    case 78:
	    case 79:
	    case 80:
	    case 81:
	    case 82:
	    case 83:
	    case 84:
	    case 85:
	    case 86:
	    case 87:
	    case 88:
	    case 89:
	    case 90:
	    case 91:
	    case 92:
	    case 93:

		switch (rnd(855)) {

			case 1: makemaz("Aci-6"); return;
			case 2: makemaz("Aci-7"); return;
			case 3: makemaz("Aci-8"); return;
			case 4: makemaz("Aci-9"); return;
			case 5: makemaz("Aci-0"); return;
			case 6: makemaz("Act-6"); return;
			case 7: makemaz("Act-7"); return;
			case 8: makemaz("Act-8"); return;
			case 9: makemaz("Act-9"); return;
			case 10: makemaz("Act-0"); return;
			case 11: makemaz("Alt-6"); return;
			case 12: makemaz("Alt-7"); return;
			case 13: makemaz("Alt-8"); return;
			case 14: makemaz("Alt-9"); return;
			case 15: makemaz("Alt-0"); return;
			case 16: makemaz("Ama-6"); return;
			case 17: makemaz("Ama-7"); return;
			case 18: makemaz("Ama-8"); return;
			case 19: makemaz("Ama-9"); return;
			case 20: makemaz("Ama-0"); return;
			case 21: makemaz("Arc-6"); return;
			case 22: makemaz("Arc-7"); return;
			case 23: makemaz("Arc-8"); return;
			case 24: makemaz("Arc-9"); return;
			case 25: makemaz("Arc-0"); return;
			case 26: makemaz("Art-6"); return;
			case 27: makemaz("Art-7"); return;
			case 28: makemaz("Art-8"); return;
			case 29: makemaz("Art-9"); return;
			case 30: makemaz("Art-0"); return;
			case 31: makemaz("Ass-6"); return;
			case 32: makemaz("Ass-7"); return;
			case 33: makemaz("Ass-8"); return;
			case 34: makemaz("Ass-9"); return;
			case 35: makemaz("Ass-0"); return;
			case 36: makemaz("Aug-6"); return;
			case 37: makemaz("Aug-7"); return;
			case 38: makemaz("Aug-8"); return;
			case 39: makemaz("Aug-9"); return;
			case 40: makemaz("Aug-0"); return;
			case 41: makemaz("Bar-6"); return;
			case 42: makemaz("Bar-7"); return;
			case 43: makemaz("Bar-8"); return;
			case 44: makemaz("Bar-9"); return;
			case 45: makemaz("Bar-0"); return;
			case 46: makemaz("Brd-6"); return;
			case 47: makemaz("Brd-7"); return;
			case 48: makemaz("Brd-8"); return;
			case 49: makemaz("Brd-9"); return;
			case 50: makemaz("Brd-0"); return;
			case 51: makemaz("Bin-6"); return;
			case 52: makemaz("Bin-7"); return;
			case 53: makemaz("Bin-8"); return;
			case 54: makemaz("Bin-9"); return;
			case 55: makemaz("Bin-0"); return;
			case 56: makemaz("Ble-6"); return;
			case 57: makemaz("Ble-7"); return;
			case 58: makemaz("Ble-8"); return;
			case 59: makemaz("Ble-9"); return;
			case 60: makemaz("Ble-0"); return;
			case 61: makemaz("Blo-6"); return;
			case 62: makemaz("Blo-7"); return;
			case 63: makemaz("Blo-8"); return;
			case 64: makemaz("Blo-9"); return;
			case 65: makemaz("Blo-0"); return;
			case 66: makemaz("Bos-6"); return;
			case 67: makemaz("Bos-7"); return;
			case 68: makemaz("Bos-8"); return;
			case 69: makemaz("Bos-9"); return;
			case 70: makemaz("Bos-0"); return;
			case 71: makemaz("Bul-6"); return;
			case 72: makemaz("Bul-7"); return;
			case 73: makemaz("Bul-8"); return;
			case 74: makemaz("Bul-9"); return;
			case 75: makemaz("Bul-0"); return;
			case 76: makemaz("Cav-6"); return;
			case 77: makemaz("Cav-7"); return;
			case 78: makemaz("Cav-8"); return;
			case 79: makemaz("Cav-9"); return;
			case 80: makemaz("Cav-0"); return;
			case 81: makemaz("Che-6"); return;
			case 82: makemaz("Che-7"); return;
			case 83: makemaz("Che-8"); return;
			case 84: makemaz("Che-9"); return;
			case 85: makemaz("Che-0"); return;
			case 86: makemaz("Con-6"); return;
			case 87: makemaz("Con-7"); return;
			case 88: makemaz("Con-8"); return;
			case 89: makemaz("Con-9"); return;
			case 90: makemaz("Con-0"); return;
			case 91: makemaz("Coo-6"); return;
			case 92: makemaz("Coo-7"); return;
			case 93: makemaz("Coo-8"); return;
			case 94: makemaz("Coo-9"); return;
			case 95: makemaz("Coo-0"); return;
			case 96: makemaz("Cou-6"); return;
			case 97: makemaz("Cou-7"); return;
			case 98: makemaz("Cou-8"); return;
			case 99: makemaz("Cou-9"); return;
			case 100: makemaz("Cou-0"); return;
			case 101: makemaz("Abu-6"); return;
			case 102: makemaz("Abu-7"); return;
			case 103: makemaz("Abu-8"); return;
			case 104: makemaz("Abu-9"); return;
			case 105: makemaz("Abu-0"); return;
			case 106: makemaz("Dea-6"); return;
			case 107: makemaz("Dea-7"); return;
			case 108: makemaz("Dea-8"); return;
			case 109: makemaz("Dea-9"); return;
			case 110: makemaz("Dea-0"); return;
			case 111: makemaz("Div-6"); return;
			case 112: makemaz("Div-7"); return;
			case 113: makemaz("Div-8"); return;
			case 114: makemaz("Div-9"); return;
			case 115: makemaz("Div-0"); return;
			case 116: makemaz("Dol-6"); return;
			case 117: makemaz("Dol-7"); return;
			case 118: makemaz("Dol-8"); return;
			case 119: makemaz("Dol-9"); return;
			case 120: makemaz("Dol-0"); return;
			case 121: makemaz("Dru-6"); return;
			case 122: makemaz("Dru-7"); return;
			case 123: makemaz("Dru-8"); return;
			case 124: makemaz("Dru-9"); return;
			case 125: makemaz("Dru-0"); return;
			case 126: makemaz("Dun-6"); return;
			case 127: makemaz("Dun-7"); return;
			case 128: makemaz("Dun-8"); return;
			case 129: makemaz("Dun-9"); return;
			case 130: makemaz("Dun-0"); return;
			case 131: makemaz("Ele-6"); return;
			case 132: makemaz("Ele-7"); return;
			case 133: makemaz("Ele-8"); return;
			case 134: makemaz("Ele-9"); return;
			case 135: makemaz("Ele-0"); return;
			case 136: makemaz("Elp-6"); return;
			case 137: makemaz("Elp-7"); return;
			case 138: makemaz("Elp-8"); return;
			case 139: makemaz("Elp-9"); return;
			case 140: makemaz("Elp-0"); return;
			case 141: makemaz("Stu-6"); return;
			case 142: makemaz("Stu-7"); return;
			case 143: makemaz("Stu-8"); return;
			case 144: makemaz("Stu-9"); return;
			case 145: makemaz("Stu-0"); return;
			case 146: makemaz("Fir-6"); return;
			case 147: makemaz("Fir-7"); return;
			case 148: makemaz("Fir-8"); return;
			case 149: makemaz("Fir-9"); return;
			case 150: makemaz("Fir-0"); return;
			case 151: makemaz("Fla-6"); return;
			case 152: makemaz("Fla-7"); return;
			case 153: makemaz("Fla-8"); return;
			case 154: makemaz("Fla-9"); return;
			case 155: makemaz("Fla-0"); return;
			case 156: makemaz("Fox-6"); return;
			case 157: makemaz("Fox-7"); return;
			case 158: makemaz("Fox-8"); return;
			case 159: makemaz("Fox-9"); return;
			case 160: makemaz("Fox-0"); return;
			case 161: makemaz("Gam-6"); return;
			case 162: makemaz("Gam-7"); return;
			case 163: makemaz("Gam-8"); return;
			case 164: makemaz("Gam-9"); return;
			case 165: makemaz("Gam-0"); return;
			case 166: makemaz("Gan-6"); return;
			case 167: makemaz("Gan-7"); return;
			case 168: makemaz("Gan-8"); return;
			case 169: makemaz("Gan-9"); return;
			case 170: makemaz("Gan-0"); return;
			case 171: makemaz("Gee-6"); return;
			case 172: makemaz("Gee-7"); return;
			case 173: makemaz("Gee-8"); return;
			case 174: makemaz("Gee-9"); return;
			case 175: makemaz("Gee-0"); return;
			case 176: makemaz("Gla-6"); return;
			case 177: makemaz("Gla-7"); return;
			case 178: makemaz("Gla-8"); return;
			case 179: makemaz("Gla-9"); return;
			case 180: makemaz("Gla-0"); return;
			case 181: makemaz("Gof-6"); return;
			case 182: makemaz("Gof-7"); return;
			case 183: makemaz("Gof-8"); return;
			case 184: makemaz("Gof-9"); return;
			case 185: makemaz("Gof-0"); return;
			case 186: makemaz("Gra-6"); return;
			case 187: makemaz("Gra-7"); return;
			case 188: makemaz("Gra-8"); return;
			case 189: makemaz("Gra-9"); return;
			case 190: makemaz("Gra-0"); return;
			case 191: makemaz("Gun-6"); return;
			case 192: makemaz("Gun-7"); return;
			case 193: makemaz("Gun-8"); return;
			case 194: makemaz("Gun-9"); return;
			case 195: makemaz("Gun-0"); return;
			case 196: makemaz("Hea-6"); return;
			case 197: makemaz("Hea-7"); return;
			case 198: makemaz("Hea-8"); return;
			case 199: makemaz("Hea-9"); return;
			case 200: makemaz("Hea-0"); return;
			case 201: makemaz("Ice-6"); return;
			case 202: makemaz("Ice-7"); return;
			case 203: makemaz("Ice-8"); return;
			case 204: makemaz("Ice-9"); return;
			case 205: makemaz("Ice-0"); return;
			case 206: makemaz("Scr-6"); return;
			case 207: makemaz("Scr-7"); return;
			case 208: makemaz("Scr-8"); return;
			case 209: makemaz("Scr-9"); return;
			case 210: makemaz("Scr-0"); return;
			case 211: makemaz("Jed-6"); return;
			case 212: makemaz("Jed-7"); return;
			case 213: makemaz("Jed-8"); return;
			case 214: makemaz("Jed-9"); return;
			case 215: makemaz("Jed-0"); return;
			case 216: makemaz("Jes-6"); return;
			case 217: makemaz("Jes-7"); return;
			case 218: makemaz("Jes-8"); return;
			case 219: makemaz("Jes-9"); return;
			case 220: makemaz("Jes-0"); return;
			case 221: makemaz("Kni-6"); return;
			case 222: makemaz("Kni-7"); return;
			case 223: makemaz("Kni-8"); return;
			case 224: makemaz("Kni-9"); return;
			case 225: makemaz("Kni-0"); return;
			case 226: makemaz("Kor-6"); return;
			case 227: makemaz("Kor-7"); return;
			case 228: makemaz("Kor-8"); return;
			case 229: makemaz("Kor-9"); return;
			case 230: makemaz("Kor-0"); return;
			case 231: makemaz("Lad-6"); return;
			case 232: makemaz("Lad-7"); return;
			case 233: makemaz("Lad-8"); return;
			case 234: makemaz("Lad-9"); return;
			case 235: makemaz("Lad-0"); return;
			case 236: makemaz("Lib-6"); return;
			case 237: makemaz("Lib-7"); return;
			case 238: makemaz("Lib-8"); return;
			case 239: makemaz("Lib-9"); return;
			case 240: makemaz("Lib-0"); return;
			case 241: makemaz("Loc-6"); return;
			case 242: makemaz("Loc-7"); return;
			case 243: makemaz("Loc-8"); return;
			case 244: makemaz("Loc-9"); return;
			case 245: makemaz("Loc-0"); return;
			case 246: makemaz("Lun-6"); return;
			case 247: makemaz("Lun-7"); return;
			case 248: makemaz("Lun-8"); return;
			case 249: makemaz("Lun-9"); return;
			case 250: makemaz("Lun-0"); return;
			case 251: makemaz("Mah-6"); return;
			case 252: makemaz("Mah-7"); return;
			case 253: makemaz("Mah-8"); return;
			case 254: makemaz("Mah-9"); return;
			case 255: makemaz("Mah-0"); return;
			case 256: makemaz("Mon-6"); return;
			case 257: makemaz("Mon-7"); return;
			case 258: makemaz("Mon-8"); return;
			case 259: makemaz("Mon-9"); return;
			case 260: makemaz("Mon-0"); return;
			case 261: makemaz("Mus-6"); return;
			case 262: makemaz("Mus-7"); return;
			case 263: makemaz("Mus-8"); return;
			case 264: makemaz("Mus-9"); return;
			case 265: makemaz("Mus-0"); return;
			case 266: makemaz("Mys-6"); return;
			case 267: makemaz("Mys-7"); return;
			case 268: makemaz("Mys-8"); return;
			case 269: makemaz("Mys-9"); return;
			case 270: makemaz("Mys-0"); return;
			case 271: makemaz("Nec-6"); return;
			case 272: makemaz("Nec-7"); return;
			case 273: makemaz("Nec-8"); return;
			case 274: makemaz("Nec-9"); return;
			case 275: makemaz("Nec-0"); return;
			case 276: makemaz("Nin-6"); return;
			case 277: makemaz("Nin-7"); return;
			case 278: makemaz("Nin-8"); return;
			case 279: makemaz("Nin-9"); return;
			case 280: makemaz("Nin-0"); return;
			case 281: makemaz("Nob-6"); return;
			case 282: makemaz("Nob-7"); return;
			case 283: makemaz("Nob-8"); return;
			case 284: makemaz("Nob-9"); return;
			case 285: makemaz("Nob-0"); return;
			case 286: makemaz("Off-6"); return;
			case 287: makemaz("Off-7"); return;
			case 288: makemaz("Off-8"); return;
			case 289: makemaz("Off-9"); return;
			case 290: makemaz("Off-0"); return;
			case 291: makemaz("Ord-6"); return;
			case 292: makemaz("Ord-7"); return;
			case 293: makemaz("Ord-8"); return;
			case 294: makemaz("Ord-9"); return;
			case 295: makemaz("Ord-0"); return;
			case 296: makemaz("Ota-6"); return;
			case 297: makemaz("Ota-7"); return;
			case 298: makemaz("Ota-8"); return;
			case 299: makemaz("Ota-9"); return;
			case 300: makemaz("Ota-0"); return;
			case 301: makemaz("Pal-6"); return;
			case 302: makemaz("Pal-7"); return;
			case 303: makemaz("Pal-8"); return;
			case 304: makemaz("Pal-9"); return;
			case 305: makemaz("Pal-0"); return;
			case 306: makemaz("Pic-6"); return;
			case 307: makemaz("Pic-7"); return;
			case 308: makemaz("Pic-8"); return;
			case 309: makemaz("Pic-9"); return;
			case 310: makemaz("Pic-0"); return;
			case 311: makemaz("Pir-6"); return;
			case 312: makemaz("Pir-7"); return;
			case 313: makemaz("Pir-8"); return;
			case 314: makemaz("Pir-9"); return;
			case 315: makemaz("Pir-0"); return;
			case 316: makemaz("Pok-6"); return;
			case 317: makemaz("Pok-7"); return;
			case 318: makemaz("Pok-8"); return;
			case 319: makemaz("Pok-9"); return;
			case 320: makemaz("Pok-0"); return;
			case 321: makemaz("Pol-6"); return;
			case 322: makemaz("Pol-7"); return;
			case 323: makemaz("Pol-8"); return;
			case 324: makemaz("Pol-9"); return;
			case 325: makemaz("Pol-0"); return;
			case 326: makemaz("Pri-6"); return;
			case 327: makemaz("Pri-7"); return;
			case 328: makemaz("Pri-8"); return;
			case 329: makemaz("Pri-9"); return;
			case 330: makemaz("Pri-0"); return;
			case 331: makemaz("Psi-6"); return;
			case 332: makemaz("Psi-7"); return;
			case 333: makemaz("Psi-8"); return;
			case 334: makemaz("Psi-9"); return;
			case 335: makemaz("Psi-0"); return;
			case 336: makemaz("Ran-6"); return;
			case 337: makemaz("Ran-7"); return;
			case 338: makemaz("Ran-8"); return;
			case 339: makemaz("Ran-9"); return;
			case 340: makemaz("Ran-0"); return;
			case 341: makemaz("Roc-6"); return;
			case 342: makemaz("Roc-7"); return;
			case 343: makemaz("Roc-8"); return;
			case 344: makemaz("Roc-9"); return;
			case 345: makemaz("Roc-0"); return;
			case 346: makemaz("Rog-6"); return;
			case 347: makemaz("Rog-7"); return;
			case 348: makemaz("Rog-8"); return;
			case 349: makemaz("Rog-9"); return;
			case 350: makemaz("Rog-0"); return;
			case 351: makemaz("Sag-6"); return;
			case 352: makemaz("Sag-7"); return;
			case 353: makemaz("Sag-8"); return;
			case 354: makemaz("Sag-9"); return;
			case 355: makemaz("Sag-0"); return;
			case 356: makemaz("Sai-6"); return;
			case 357: makemaz("Sai-7"); return;
			case 358: makemaz("Sai-8"); return;
			case 359: makemaz("Sai-9"); return;
			case 360: makemaz("Sai-0"); return;
			case 361: makemaz("Sam-6"); return;
			case 362: makemaz("Sam-7"); return;
			case 363: makemaz("Sam-8"); return;
			case 364: makemaz("Sam-9"); return;
			case 365: makemaz("Sam-0"); return;
			case 366: makemaz("Sci-6"); return;
			case 367: makemaz("Sci-7"); return;
			case 368: makemaz("Sci-8"); return;
			case 369: makemaz("Sci-9"); return;
			case 370: makemaz("Sci-0"); return;
			case 371: makemaz("Sla-6"); return;
			case 372: makemaz("Sla-7"); return;
			case 373: makemaz("Sla-8"); return;
			case 374: makemaz("Sla-9"); return;
			case 375: makemaz("Sla-0"); return;
			case 376: makemaz("Spa-6"); return;
			case 377: makemaz("Spa-7"); return;
			case 378: makemaz("Spa-8"); return;
			case 379: makemaz("Spa-9"); return;
			case 380: makemaz("Spa-0"); return;
			case 381: makemaz("Sup-6"); return;
			case 382: makemaz("Sup-7"); return;
			case 383: makemaz("Sup-8"); return;
			case 384: makemaz("Sup-9"); return;
			case 385: makemaz("Sup-0"); return;
			case 386: makemaz("Tha-6"); return;
			case 387: makemaz("Tha-7"); return;
			case 388: makemaz("Tha-8"); return;
			case 389: makemaz("Tha-9"); return;
			case 390: makemaz("Tha-0"); return;
			case 391: makemaz("Top-6"); return;
			case 392: makemaz("Top-7"); return;
			case 393: makemaz("Top-8"); return;
			case 394: makemaz("Top-9"); return;
			case 395: makemaz("Top-0"); return;
			case 396: makemaz("Tou-6"); return;
			case 397: makemaz("Tou-7"); return;
			case 398: makemaz("Tou-8"); return;
			case 399: makemaz("Tou-9"); return;
			case 400: makemaz("Tou-0"); return;
			case 401: makemaz("Tra-6"); return;
			case 402: makemaz("Tra-7"); return;
			case 403: makemaz("Tra-8"); return;
			case 404: makemaz("Tra-9"); return;
			case 405: makemaz("Tra-0"); return;
			case 406: makemaz("Und-6"); return;
			case 407: makemaz("Und-7"); return;
			case 408: makemaz("Und-8"); return;
			case 409: makemaz("Und-9"); return;
			case 410: makemaz("Und-0"); return;
			case 411: makemaz("Unt-6"); return;
			case 412: makemaz("Unt-7"); return;
			case 413: makemaz("Unt-8"); return;
			case 414: makemaz("Unt-9"); return;
			case 415: makemaz("Unt-0"); return;
			case 416: makemaz("Val-6"); return;
			case 417: makemaz("Val-7"); return;
			case 418: makemaz("Val-8"); return;
			case 419: makemaz("Val-9"); return;
			case 420: makemaz("Val-0"); return;
			case 421: makemaz("Wan-6"); return;
			case 422: makemaz("Wan-7"); return;
			case 423: makemaz("Wan-8"); return;
			case 424: makemaz("Wan-9"); return;
			case 425: makemaz("Wan-0"); return;
			case 426: makemaz("War-6"); return;
			case 427: makemaz("War-7"); return;
			case 428: makemaz("War-8"); return;
			case 429: makemaz("War-9"); return;
			case 430: makemaz("War-0"); return;
			case 431: makemaz("Wiz-6"); return;
			case 432: makemaz("Wiz-7"); return;
			case 433: makemaz("Wiz-8"); return;
			case 434: makemaz("Wiz-9"); return;
			case 435: makemaz("Wiz-0"); return;
			case 436: makemaz("Yeo-6"); return;
			case 437: makemaz("Yeo-7"); return;
			case 438: makemaz("Yeo-8"); return;
			case 439: makemaz("Yeo-9"); return;
			case 440: makemaz("Yeo-0"); return;
			case 441: makemaz("Zoo-6"); return;
			case 442: makemaz("Zoo-7"); return;
			case 443: makemaz("Zoo-8"); return;
			case 444: makemaz("Zoo-9"); return;
			case 445: makemaz("Zoo-0"); return;
			case 446: makemaz("Zyb-6"); return;
			case 447: makemaz("Zyb-7"); return;
			case 448: makemaz("Zyb-8"); return;
			case 449: makemaz("Zyb-9"); return;
			case 450: makemaz("Zyb-0"); return;
			case 451: makemaz("Ana-6"); return;
			case 452: makemaz("Ana-7"); return;
			case 453: makemaz("Ana-8"); return;
			case 454: makemaz("Ana-9"); return;
			case 455: makemaz("Ana-0"); return;
			case 456: makemaz("Cam-6"); return;
			case 457: makemaz("Cam-7"); return;
			case 458: makemaz("Cam-8"); return;
			case 459: makemaz("Cam-9"); return;
			case 460: makemaz("Cam-0"); return;
			case 461: makemaz("Mar-6"); return;
			case 462: makemaz("Mar-7"); return;
			case 463: makemaz("Mar-8"); return;
			case 464: makemaz("Mar-9"); return;
			case 465: makemaz("Mar-0"); return;
			case 466: makemaz("Sli-6"); return;
			case 467: makemaz("Sli-7"); return;
			case 468: makemaz("Sli-8"); return;
			case 469: makemaz("Sli-9"); return;
			case 470: makemaz("Sli-0"); return;
			case 471: makemaz("Drd-6"); return;
			case 472: makemaz("Drd-7"); return;
			case 473: makemaz("Drd-8"); return;
			case 474: makemaz("Drd-9"); return;
			case 475: makemaz("Drd-0"); return;
			case 476: makemaz("Erd-6"); return;
			case 477: makemaz("Erd-7"); return;
			case 478: makemaz("Erd-8"); return;
			case 479: makemaz("Erd-9"); return;
			case 480: makemaz("Erd-0"); return;
			case 481: makemaz("Fai-6"); return;
			case 482: makemaz("Fai-7"); return;
			case 483: makemaz("Fai-8"); return;
			case 484: makemaz("Fai-9"); return;
			case 485: makemaz("Fai-0"); return;
			case 486: makemaz("Fen-6"); return;
			case 487: makemaz("Fen-7"); return;
			case 488: makemaz("Fen-8"); return;
			case 489: makemaz("Fen-9"); return;
			case 490: makemaz("Fen-0"); return;
			case 491: makemaz("Fig-6"); return;
			case 492: makemaz("Fig-7"); return;
			case 493: makemaz("Fig-8"); return;
			case 494: makemaz("Fig-9"); return;
			case 495: makemaz("Fig-0"); return;
			case 496: makemaz("Gol-6"); return;
			case 497: makemaz("Gol-7"); return;
			case 498: makemaz("Gol-8"); return;
			case 499: makemaz("Gol-9"); return;
			case 500: makemaz("Gol-0"); return;
			case 501: makemaz("Jus-6"); return;
			case 502: makemaz("Jus-7"); return;
			case 503: makemaz("Jus-8"); return;
			case 504: makemaz("Jus-9"); return;
			case 505: makemaz("Jus-0"); return;
			case 506: makemaz("Med-6"); return;
			case 507: makemaz("Med-7"); return;
			case 508: makemaz("Med-8"); return;
			case 509: makemaz("Med-9"); return;
			case 510: makemaz("Med-0"); return;
			case 511: makemaz("Mid-6"); return;
			case 512: makemaz("Mid-7"); return;
			case 513: makemaz("Mid-8"); return;
			case 514: makemaz("Mid-9"); return;
			case 515: makemaz("Mid-0"); return;
			case 516: makemaz("Mur-6"); return;
			case 517: makemaz("Mur-7"); return;
			case 518: makemaz("Mur-8"); return;
			case 519: makemaz("Mur-9"); return;
			case 520: makemaz("Mur-0"); return;
			case 521: makemaz("Poi-6"); return;
			case 522: makemaz("Poi-7"); return;
			case 523: makemaz("Poi-8"); return;
			case 524: makemaz("Poi-9"); return;
			case 525: makemaz("Poi-0"); return;
			case 526: makemaz("Rin-6"); return;
			case 527: makemaz("Rin-7"); return;
			case 528: makemaz("Rin-8"); return;
			case 529: makemaz("Rin-9"); return;
			case 530: makemaz("Rin-0"); return;
			case 531: makemaz("Sha-6"); return;
			case 532: makemaz("Sha-7"); return;
			case 533: makemaz("Sha-8"); return;
			case 534: makemaz("Sha-9"); return;
			case 535: makemaz("Sha-0"); return;
			case 536: makemaz("Twe-6"); return;
			case 537: makemaz("Twe-7"); return;
			case 538: makemaz("Twe-8"); return;
			case 539: makemaz("Twe-9"); return;
			case 540: makemaz("Twe-0"); return;
			case 541: makemaz("Use-6"); return;
			case 542: makemaz("Use-7"); return;
			case 543: makemaz("Use-8"); return;
			case 544: makemaz("Use-9"); return;
			case 545: makemaz("Use-0"); return;
			case 546: makemaz("Sex-6"); return;
			case 547: makemaz("Sex-7"); return;
			case 548: makemaz("Sex-8"); return;
			case 549: makemaz("Sex-9"); return;
			case 550: makemaz("Sex-0"); return;
			case 551: makemaz("Unb-6"); return;
			case 552: makemaz("Unb-7"); return;
			case 553: makemaz("Unb-8"); return;
			case 554: makemaz("Unb-9"); return;
			case 555: makemaz("Unb-0"); return;
			case 556: makemaz("Trs-6"); return;
			case 557: makemaz("Trs-7"); return;
			case 558: makemaz("Trs-8"); return;
			case 559: makemaz("Trs-9"); return;
			case 560: makemaz("Trs-0"); return;
			case 561: makemaz("Cha-6"); return;
			case 562: makemaz("Cha-7"); return;
			case 563: makemaz("Cha-8"); return;
			case 564: makemaz("Cha-9"); return;
			case 565: makemaz("Cha-0"); return;
			case 566: makemaz("Elm-6"); return;
			case 567: makemaz("Elm-7"); return;
			case 568: makemaz("Elm-8"); return;
			case 569: makemaz("Elm-9"); return;
			case 570: makemaz("Elm-0"); return;
			case 571: makemaz("Wil-6"); return;
			case 572: makemaz("Wil-7"); return;
			case 573: makemaz("Wil-8"); return;
			case 574: makemaz("Wil-9"); return;
			case 575: makemaz("Wil-0"); return;
			case 576: makemaz("Occ-6"); return;
			case 577: makemaz("Occ-7"); return;
			case 578: makemaz("Occ-8"); return;
			case 579: makemaz("Occ-9"); return;
			case 580: makemaz("Occ-0"); return;
			case 581: makemaz("Pro-6"); return;
			case 582: makemaz("Pro-7"); return;
			case 583: makemaz("Pro-8"); return;
			case 584: makemaz("Pro-9"); return;
			case 585: makemaz("Pro-0"); return;
			case 586: makemaz("Kur-6"); return;
			case 587: makemaz("Kur-7"); return;
			case 588: makemaz("Kur-8"); return;
			case 589: makemaz("Kur-9"); return;
			case 590: makemaz("Kur-0"); return;
			case 591: makemaz("For-6"); return;
			case 592: makemaz("For-7"); return;
			case 593: makemaz("For-8"); return;
			case 594: makemaz("For-9"); return;
			case 595: makemaz("For-0"); return;
			case 596: makemaz("Trc-6"); return;
			case 597: makemaz("Trc-7"); return;
			case 598: makemaz("Trc-8"); return;
			case 599: makemaz("Trc-9"); return;
			case 600: makemaz("Trc-0"); return;
			case 601: makemaz("Nuc-6"); return;
			case 602: makemaz("Nuc-7"); return;
			case 603: makemaz("Nuc-8"); return;
			case 604: makemaz("Nuc-9"); return;
			case 605: makemaz("Nuc-0"); return;
			case 606: makemaz("Sco-6"); return;
			case 607: makemaz("Sco-7"); return;
			case 608: makemaz("Sco-8"); return;
			case 609: makemaz("Sco-9"); return;
			case 610: makemaz("Sco-0"); return;
			case 611: makemaz("Fem-6"); return;
			case 612: makemaz("Fem-7"); return;
			case 613: makemaz("Fem-8"); return;
			case 614: makemaz("Fem-9"); return;
			case 615: makemaz("Fem-0"); return;
			case 616: makemaz("Hus-6"); return;
			case 617: makemaz("Hus-7"); return;
			case 618: makemaz("Hus-8"); return;
			case 619: makemaz("Hus-9"); return;
			case 620: makemaz("Hus-0"); return;
			case 621: makemaz("Acu-6"); return;
			case 622: makemaz("Acu-7"); return;
			case 623: makemaz("Acu-8"); return;
			case 624: makemaz("Acu-9"); return;
			case 625: makemaz("Acu-0"); return;
			case 626: makemaz("Mas-6"); return;
			case 627: makemaz("Mas-7"); return;
			case 628: makemaz("Mas-8"); return;
			case 629: makemaz("Mas-9"); return;
			case 630: makemaz("Mas-0"); return;
			case 631: makemaz("Gre-6"); return;
			case 632: makemaz("Gre-7"); return;
			case 633: makemaz("Gre-8"); return;
			case 634: makemaz("Gre-9"); return;
			case 635: makemaz("Gre-0"); return;
			case 636: makemaz("Cel-6"); return;
			case 637: makemaz("Cel-7"); return;
			case 638: makemaz("Cel-8"); return;
			case 639: makemaz("Cel-9"); return;
			case 640: makemaz("Cel-0"); return;
			case 641: makemaz("Wal-6"); return;
			case 642: makemaz("Wal-7"); return;
			case 643: makemaz("Wal-8"); return;
			case 644: makemaz("Wal-9"); return;
			case 645: makemaz("Wal-0"); return;
			case 646: makemaz("Soc-6"); return;
			case 647: makemaz("Soc-7"); return;
			case 648: makemaz("Soc-8"); return;
			case 649: makemaz("Soc-9"); return;
			case 650: makemaz("Soc-0"); return;
			case 651: makemaz("Dem-6"); return;
			case 652: makemaz("Dem-7"); return;
			case 653: makemaz("Dem-8"); return;
			case 654: makemaz("Dem-9"); return;
			case 655: makemaz("Dem-0"); return;
			case 656: makemaz("Dis-6"); return;
			case 657: makemaz("Dis-7"); return;
			case 658: makemaz("Dis-8"); return;
			case 659: makemaz("Dis-9"); return;
			case 660: makemaz("Dis-0"); return;
			case 661: makemaz("Sto-6"); return;
			case 662: makemaz("Sto-7"); return;
			case 663: makemaz("Sto-8"); return;
			case 664: makemaz("Sto-9"); return;
			case 665: makemaz("Sto-0"); return;
			case 666: makemaz("Mam-6"); return;
			case 667: makemaz("Mam-7"); return;
			case 668: makemaz("Mam-8"); return;
			case 669: makemaz("Mam-9"); return;
			case 670: makemaz("Mam-0"); return;
			case 671: makemaz("Jan-6"); return;
			case 672: makemaz("Jan-7"); return;
			case 673: makemaz("Jan-8"); return;
			case 674: makemaz("Jan-9"); return;
			case 675: makemaz("Jan-0"); return;
			case 676: makemaz("Emp-6"); return;
			case 677: makemaz("Emp-7"); return;
			case 678: makemaz("Emp-8"); return;
			case 679: makemaz("Emp-9"); return;
			case 680: makemaz("Emp-0"); return;
			case 681: makemaz("Psy-6"); return;
			case 682: makemaz("Psy-7"); return;
			case 683: makemaz("Psy-8"); return;
			case 684: makemaz("Psy-9"); return;
			case 685: makemaz("Psy-0"); return;
			case 686: makemaz("Qua-6"); return;
			case 687: makemaz("Qua-7"); return;
			case 688: makemaz("Qua-8"); return;
			case 689: makemaz("Qua-9"); return;
			case 690: makemaz("Qua-0"); return;
			case 691: makemaz("Cra-6"); return;
			case 692: makemaz("Cra-7"); return;
			case 693: makemaz("Cra-8"); return;
			case 694: makemaz("Cra-9"); return;
			case 695: makemaz("Cra-0"); return;
			case 696: makemaz("Wei-6"); return;
			case 697: makemaz("Wei-7"); return;
			case 698: makemaz("Wei-8"); return;
			case 699: makemaz("Wei-9"); return;
			case 700: makemaz("Wei-0"); return;
			case 701: makemaz("Xel-6"); return;
			case 702: makemaz("Xel-7"); return;
			case 703: makemaz("Xel-8"); return;
			case 704: makemaz("Xel-9"); return;
			case 705: makemaz("Xel-0"); return;
			case 706: makemaz("Yau-6"); return;
			case 707: makemaz("Yau-7"); return;
			case 708: makemaz("Yau-8"); return;
			case 709: makemaz("Yau-9"); return;
			case 710: makemaz("Yau-0"); return;
			case 711: makemaz("Sof-6"); return;
			case 712: makemaz("Sof-7"); return;
			case 713: makemaz("Sof-8"); return;
			case 714: makemaz("Sof-9"); return;
			case 715: makemaz("Sof-0"); return;
			case 716: makemaz("Ast-6"); return;
			case 717: makemaz("Ast-7"); return;
			case 718: makemaz("Ast-8"); return;
			case 719: makemaz("Ast-9"); return;
			case 720: makemaz("Ast-0"); return;
			case 721: makemaz("Sma-6"); return;
			case 722: makemaz("Sma-7"); return;
			case 723: makemaz("Sma-8"); return;
			case 724: makemaz("Sma-9"); return;
			case 725: makemaz("Sma-0"); return;
			case 726: makemaz("Cyb-6"); return;
			case 727: makemaz("Cyb-7"); return;
			case 728: makemaz("Cyb-8"); return;
			case 729: makemaz("Cyb-9"); return;
			case 730: makemaz("Cyb-0"); return;
			case 731: makemaz("Tos-6"); return;
			case 732: makemaz("Tos-7"); return;
			case 733: makemaz("Tos-8"); return;
			case 734: makemaz("Tos-9"); return;
			case 735: makemaz("Tos-0"); return;
			case 736: makemaz("Sym-6"); return;
			case 737: makemaz("Sym-7"); return;
			case 738: makemaz("Sym-8"); return;
			case 739: makemaz("Sym-9"); return;
			case 740: makemaz("Sym-0"); return;
			case 741: makemaz("Pra-6"); return;
			case 742: makemaz("Pra-7"); return;
			case 743: makemaz("Pra-8"); return;
			case 744: makemaz("Pra-9"); return;
			case 745: makemaz("Pra-0"); return;
			case 746: makemaz("Mil-6"); return;
			case 747: makemaz("Mil-7"); return;
			case 748: makemaz("Mil-8"); return;
			case 749: makemaz("Mil-9"); return;
			case 750: makemaz("Mil-0"); return;
			case 751: makemaz("Gen-6"); return;
			case 752: makemaz("Gen-7"); return;
			case 753: makemaz("Gen-8"); return;
			case 754: makemaz("Gen-9"); return;
			case 755: makemaz("Gen-0"); return;
			case 756: makemaz("Fjo-6"); return;
			case 757: makemaz("Fjo-7"); return;
			case 758: makemaz("Fjo-8"); return;
			case 759: makemaz("Fjo-9"); return;
			case 760: makemaz("Fjo-0"); return;
			case 761: makemaz("Eme-6"); return;
			case 762: makemaz("Eme-7"); return;
			case 763: makemaz("Eme-8"); return;
			case 764: makemaz("Eme-9"); return;
			case 765: makemaz("Eme-0"); return;
			case 766: makemaz("Com-6"); return;
			case 767: makemaz("Com-7"); return;
			case 768: makemaz("Com-8"); return;
			case 769: makemaz("Com-9"); return;
			case 770: makemaz("Com-0"); return;
			case 771: makemaz("Akl-6"); return;
			case 772: makemaz("Akl-7"); return;
			case 773: makemaz("Akl-8"); return;
			case 774: makemaz("Akl-9"); return;
			case 775: makemaz("Akl-0"); return;
			case 776: makemaz("Dra-6"); return;
			case 777: makemaz("Dra-7"); return;
			case 778: makemaz("Dra-8"); return;
			case 779: makemaz("Dra-9"); return;
			case 780: makemaz("Dra-0"); return;
			case 781: makemaz("Car-6"); return;
			case 782: makemaz("Car-7"); return;
			case 783: makemaz("Car-8"); return;
			case 784: makemaz("Car-9"); return;
			case 785: makemaz("Car-0"); return;
			case 786: makemaz("But-6"); return;
			case 787: makemaz("But-7"); return;
			case 788: makemaz("But-8"); return;
			case 789: makemaz("But-9"); return;
			case 790: makemaz("But-0"); return;
			case 791: makemaz("Dan-6"); return;
			case 792: makemaz("Dan-7"); return;
			case 793: makemaz("Dan-8"); return;
			case 794: makemaz("Dan-9"); return;
			case 795: makemaz("Dan-0"); return;
			case 796: makemaz("Dia-6"); return;
			case 797: makemaz("Dia-7"); return;
			case 798: makemaz("Dia-8"); return;
			case 799: makemaz("Dia-9"); return;
			case 800: makemaz("Dia-0"); return;
			case 801: makemaz("Pre-6"); return;
			case 802: makemaz("Pre-7"); return;
			case 803: makemaz("Pre-8"); return;
			case 804: makemaz("Pre-9"); return;
			case 805: makemaz("Pre-0"); return;
			case 806: makemaz("Sec-6"); return;
			case 807: makemaz("Sec-7"); return;
			case 808: makemaz("Sec-8"); return;
			case 809: makemaz("Sec-9"); return;
			case 810: makemaz("Sec-0"); return;
			case 811: makemaz("Sho-6"); return;
			case 812: makemaz("Sho-7"); return;
			case 813: makemaz("Sho-8"); return;
			case 814: makemaz("Sho-9"); return;
			case 815: makemaz("Sho-0"); return;
			case 816: makemaz("Hal-6"); return;
			case 817: makemaz("Hal-7"); return;
			case 818: makemaz("Hal-8"); return;
			case 819: makemaz("Hal-9"); return;
			case 820: makemaz("Hal-0"); return;
			case 821: makemaz("Sin-6"); return;
			case 822: makemaz("Sin-7"); return;
			case 823: makemaz("Sin-8"); return;
			case 824: makemaz("Sin-9"); return;
			case 825: makemaz("Sin-0"); return;
			case 826: makemaz("Hed-6"); return;
			case 827: makemaz("Hed-7"); return;
			case 828: makemaz("Hed-8"); return;
			case 829: makemaz("Hed-9"); return;
			case 830: makemaz("Hed-0"); return;
			case 831: makemaz("Van-6"); return;
			case 832: makemaz("Van-7"); return;
			case 833: makemaz("Van-8"); return;
			case 834: makemaz("Van-9"); return;
			case 835: makemaz("Van-0"); return;
			case 836: makemaz("Sdw-6"); return;
			case 837: makemaz("Sdw-7"); return;
			case 838: makemaz("Sdw-8"); return;
			case 839: makemaz("Sdw-9"); return;
			case 840: makemaz("Sdw-0"); return;
			case 841: makemaz("Cli-6"); return;
			case 842: makemaz("Cli-7"); return;
			case 843: makemaz("Cli-8"); return;
			case 844: makemaz("Cli-9"); return;
			case 845: makemaz("Cli-0"); return;
			case 846: makemaz("Wom-6"); return;
			case 847: makemaz("Wom-7"); return;
			case 848: makemaz("Wom-8"); return;
			case 849: makemaz("Wom-9"); return;
			case 850: makemaz("Wom-0"); return;
			case 851: makemaz("Noo-6"); return;
			case 852: makemaz("Noo-7"); return;
			case 853: makemaz("Noo-8"); return;
			case 854: makemaz("Noo-9"); return;
			case 855: makemaz("Noo-0"); return;
		}
		break;

		case 94:
		case 95:
		case 96:
		case 97:
		case 98:
		case 99:
		case 100:

		switch (rnd(69)) {

			case 1: makemaz("mazes-1"); return;
			case 2: makemaz("mazes-2"); return;
			case 3: makemaz("mazes-3"); return;
			case 4: makemaz("mazes-4"); return;
			case 5: makemaz("mazes-5"); return;
			case 6: makemaz("mazes-6"); return;
			case 7: makemaz("mazes-7"); return;
			case 8: makemaz("mazes-8"); return;
			case 9: makemaz("mazes-9"); return;
			case 10: makemaz("mazes-10"); return;
			case 11: makemaz("mazes-11"); return;
			case 12: makemaz("mazes-12"); return;
			case 13: makemaz("mazes-13"); return;
			case 14: makemaz("mazes-14"); return;
			case 15: makemaz("mazes-15"); return;
			case 16: makemaz("mazes-16"); return;
			case 17: makemaz("mazes-17"); return;
			case 18: makemaz("mazes-18"); return;
			case 19: makemaz("mazes-19"); return;
			case 20: makemaz("mazes-20"); return;
			case 21: makemaz("mazes-21"); return;
			case 22: makemaz("mazes-22"); return;
			case 23: makemaz("mazes-23"); return;
			case 24: makemaz("mazes-24"); return;
			case 25: makemaz("mazes-25"); return;
			case 26: makemaz("mazes-26"); return;
			case 27: makemaz("mazes-27"); return;
			case 28: makemaz("mazes-28"); return;
			case 29: makemaz("mazes-29"); return;
			case 30: makemaz("mazes-30"); return;
			case 31: makemaz("mazes-31"); return;
			case 32: makemaz("mazes-32"); return;
			case 33: makemaz("mazes-33"); return;
			case 34: makemaz("mazes-34"); return;
			case 35: makemaz("mazes-35"); return;
			case 36: makemaz("mazes-36"); return;
			case 37: makemaz("mazes-37"); return;
			case 38: makemaz("mazes-38"); return;
			case 39: makemaz("mazes-39"); return;
			case 40: makemaz("mazes-40"); return;
			case 41: makemaz("mazes-41"); return;
			case 42: makemaz("mazes-42"); return;
			case 43: makemaz("mazes-43"); return;
			case 44: makemaz("mazes-44"); return;
			case 45: makemaz("mazes-45"); return;
			case 46: makemaz("mazes-46"); return;
			case 47: makemaz("mazes-47"); return;
			case 48: makemaz("mazes-48"); return;
			case 49: makemaz("mazes-49"); return;
			case 50: makemaz("mazes-50"); return;
			case 51: makemaz("mazes-51"); return;
			case 52: makemaz("mazes-52"); return;
			case 53: makemaz("mazes-53"); return;
			case 54: makemaz("mazes-54"); return;
			case 55: makemaz("mazes-55"); return;
			case 56: makemaz("mazes-56"); return;
			case 57: makemaz("mazes-57"); return;
			case 58: makemaz("mazes-58"); return;
			case 59: makemaz("mazes-59"); return;
			case 60: makemaz("mazes-60"); return;
			case 61: makemaz("mazes-61"); return;
			case 62: makemaz("mazes-62"); return;
			case 63: makemaz("mazes-63"); return;
			case 64: makemaz("mazes-64"); return;
			case 65: makemaz("mazes-65"); return;
			case 66: makemaz("mazes-66"); return;
			case 67: makemaz("mazes-67"); return;
			case 68: makemaz("mazes-68"); return;
			case 69: makemaz("mazes-69"); return;

		}
		break;

		case 101:
		case 102:
		case 103:
		case 104:
		case 105:

		switch (rnd(95)) {

			case 1: makemaz("levgx-1"); return;
			case 2: makemaz("levgx-2"); return;
			case 3: makemaz("levgx-3"); return;
			case 4: makemaz("levgx-4"); return;
			case 5: makemaz("levgx-5"); return;
			case 6: makemaz("levgx-6"); return;
			case 7: makemaz("levgx-7"); return;
			case 8: makemaz("levgx-8"); return;
			case 9: makemaz("levgx-9"); return;
			case 10: makemaz("levgx-10"); return;
			case 11: makemaz("levgx-11"); return;
			case 12: makemaz("levgx-12"); return;
			case 13: makemaz("levgx-13"); return;
			case 14: makemaz("levgx-14"); return;
			case 15: makemaz("levgx-15"); return;
			case 16: makemaz("levgx-16"); return;
			case 17: makemaz("levgx-17"); return;
			case 18: makemaz("levgx-18"); return;
			case 19: makemaz("levgx-19"); return;
			case 20: makemaz("levgx-20"); return;
			case 21: makemaz("levgx-21"); return;
			case 22: makemaz("levgx-22"); return;
			case 23: makemaz("levgx-23"); return;
			case 24: makemaz("levgx-24"); return;
			case 25: makemaz("levgx-25"); return;
			case 26: makemaz("levgx-26"); return;
			case 27: makemaz("levgx-27"); return;
			case 28: makemaz("levgx-28"); return;
			case 29: makemaz("levgx-29"); return;
			case 30: makemaz("levgx-30"); return;
			case 31: makemaz("levgx-31"); return;
			case 32: makemaz("levgx-32"); return;
			case 33: makemaz("levgx-33"); return;
			case 34: makemaz("levgx-34"); return;
			case 35: makemaz("levgx-35"); return;
			case 36: makemaz("levgx-36"); return;
			case 37: makemaz("levgx-37"); return;
			case 38: makemaz("levgx-38"); return;
			case 39: makemaz("levgx-39"); return;
			case 40: makemaz("levgx-40"); return;
			case 41: makemaz("levgx-41"); return;
			case 42: makemaz("levgx-42"); return;
			case 43: makemaz("levgx-43"); return;
			case 44: makemaz("levgx-44"); return;
			case 45: makemaz("levgx-45"); return;
			case 46: makemaz("levgx-46"); return;
			case 47: makemaz("levgx-47"); return;
			case 48: makemaz("levgx-48"); return;
			case 49: makemaz("levgx-49"); return;
			case 50: makemaz("levgx-50"); return;
			case 51: makemaz("levgx-51"); return;
			case 52: makemaz("levgx-52"); return;
			case 53: makemaz("levgx-53"); return;
			case 54: makemaz("levgx-54"); return;
			case 55: makemaz("levgx-55"); return;
			case 56: makemaz("levgx-56"); return;
			case 57: makemaz("levgx-57"); return;
			case 58: makemaz("levgx-58"); return;
			case 59: makemaz("levgx-59"); return;
			case 60: makemaz("levgx-60"); return;
			case 61: makemaz("levgx-61"); return;
			case 62: makemaz("levgx-62"); return;
			case 63: makemaz("levgx-63"); return;
			case 64: makemaz("levgx-64"); return;
			case 65: makemaz("levgx-65"); return;
			case 66: makemaz("levgx-66"); return;
			case 67: makemaz("levgx-67"); return;
			case 68: makemaz("levgx-68"); return;
			case 69: makemaz("levgx-69"); return;
			case 70: makemaz("levgx-70"); return;
			case 71: makemaz("levgx-71"); return;
			case 72: makemaz("levgx-72"); return;
			case 73: makemaz("levgx-73"); return;
			case 74: makemaz("levgx-74"); return;
			case 75: makemaz("levgx-75"); return;
			case 76: makemaz("levgx-76"); return;
			case 77: makemaz("levgx-77"); return;
			case 78: makemaz("levgx-78"); return;
			case 79: makemaz("levgx-79"); return;
			case 80: makemaz("levgx-80"); return;
			case 81: makemaz("levgx-81"); return;
			case 82: makemaz("levgx-82"); return;
			case 83: makemaz("levgx-83"); return;
			case 84: makemaz("levgx-84"); return;
			case 85: makemaz("levgx-85"); return;
			case 86: makemaz("levgx-86"); return;
			case 87: makemaz("levgx-87"); return;
			case 88: makemaz("levgx-88"); return;
			case 89: makemaz("levgx-89"); return;
			case 90: makemaz("levgx-90"); return;
			case 91: makemaz("levgx-91"); return;
			case 92: makemaz("levgx-92"); return;
			case 93: makemaz("levgx-93"); return;
			case 94: makemaz("levgx-94"); return;
			case 95: makemaz("levgx-95"); return;
		}
	    case 106:

		makemaz("hcav2"); return;
		break;

	    case 107:

		makemaz("hfrnk"); return;
		break;

	    case 108:
	    case 109:
	    case 110:

		switch (rnd(8)) {

			case 1: {
				switch (rnd(5)) {

					case 1: makemaz("cowla-1"); return;
					case 2: makemaz("cowla-2"); return;
					case 3: makemaz("cowla-3"); return;
					case 4: makemaz("cowla-4"); return;
					case 5: makemaz("cowla-5"); return;

				}
				break;
			}
			case 2: makemaz("cowlb"); return;
			case 3: makemaz("cowlc"); return;
			case 4: makemaz("cowld"); return;
			case 5: makemaz("cowle"); return;
			case 6: makemaz("cowlf"); return;
			case 7: makemaz("cowlg"); return;
			case 8: makemaz("cowlh"); return;

		}

		break;

	    case 111:
		switch (rnd(15)) {
			case 1: makemaz("deehfila"); return;
			case 2: makemaz("deehfilb"); return;
			case 3: makemaz("deehfilc"); return;
			case 4: makemaz("deehfild"); return;
			case 5: makemaz("deehfile"); return;
			case 6: makemaz("deehfilf"); return;
			case 7: makemaz("deehfilg"); return;
			case 8: makemaz("deehfilh"); return;
			case 9: makemaz("deehfili"); return;
			case 10: makemaz("deehfilj"); return;
			case 11: makemaz("deehfilk"); return;
			case 12: makemaz("deehfill"); return;
			case 13: makemaz("deehfilm"); return;
			case 14: makemaz("deehfiln"); return;
			case 15: makemaz("deehfilo"); return;

		}

		break;

	    case 112:
		switch (rnd(10)) {
			case 1: makemaz("deptX-1"); return;
			case 2: makemaz("deptX-2"); return;
			case 3: makemaz("deptX-3"); return;
			case 4: makemaz("deptX-4"); return;
			case 5: makemaz("deptX-5"); return;
			case 6: makemaz("deptX-6"); return;
			case 7: makemaz("deptX-7"); return;
			case 8: makemaz("deptX-8"); return;
			case 9: makemaz("deptX-9"); return;
			case 10: makemaz("deptX-10"); return;

		}

		break;

	    case 113:
		switch (rnd(6)) {
			case 1: makemaz("depeX-1"); return;
			case 2: makemaz("depeX-2"); return;
			case 3: makemaz("depeX-3"); return;
			case 4: makemaz("depeX-4"); return;
			case 5: makemaz("depeX-5"); return;
			case 6: makemaz("depeX-6"); return;

		}

		break;

	    case 114:
		switch (rnd(5)) {
			case 1: makemaz("gruelaiX"); return;
			case 2: makemaz("joustX"); return;
			case 3: makemaz("pmazeX"); return;
			case 4: makemaz("poolhalX"); return;
			case 5: makemaz("dmazeX"); return;

		}

		break;

	    case 115:
	    case 116:

		switch (rnd(8)) {

			case 1: makemaz("grcra"); return;
			case 2: makemaz("grcrb"); return;
			case 3: makemaz("grcrc"); return;
			case 4: makemaz("grcrd"); return;
			case 5: makemaz("grcre"); return;
			case 6: makemaz("grcrf"); return;
			case 7: makemaz("grcrg"); return;
			case 8: makemaz("grcrh"); return;

		}
		break;

	    case 117:
		switch (rnd(3)) {
			case 1: makemaz("hiceqa"); return;
			case 2: makemaz("hiceqb"); return;
			case 3: makemaz("hiceqc"); return;

		}


		break;
	
          }

		}

	    return;

	}

	/* otherwise, fall through - it's a "regular" level. */

#ifdef REINCARNATION
	if (Is_rogue_level(&u.uz)) {
		makeroguerooms();
		makerogueghost();
	} else
#endif
		makerooms();
	sort_rooms();

	/* construct stairs (up and down in different rooms if possible); stairseeker needs to make changes here
	 * see below */
	croom = &rooms[rn2(nroom)];
	if (!Is_botlevel(&u.uz) && !isstairseeker) {
		mkstairs(somex(croom), somey(croom), 0, croom);	/* down */
	}
	if (nroom > 1) {
	    troom = croom;
	    croom = &rooms[rn2(nroom-1)];
	    if (croom == troom) croom++;
	}

	if (u.uz.dlevel != 1) {
	    xchar sx, sy;
	    register int stairattempts = 0; /* see reallyoccupied function --Amy */
	    do {
		sx = somex(croom);
		sy = somey(croom);
		stairattempts++;
	    } while((stairattempts > 50000) ? reallyoccupied(sx, sy) : occupied(sx, sy));
	    if (!isstairseeker) mkstairs(sx, sy, 1, croom);	/* up */
	}

	branchp = Is_branchlev(&u.uz);	/* possible dungeon branch */
	room_threshold = branchp ? 4 : 3; /* minimum number of rooms needed
					     to allow a random special room */
#ifdef REINCARNATION
	if (Is_rogue_level(&u.uz)) goto skip0;
#endif
	makecorridors();
	make_niches();

	/* stairseeker mode: try hard to find an OK location, but fall back to the regular stair creation method
	 * if we fail to find a good location after 50k tries. --Amy
	 * we need to do that here, after corridors have already been made, to ensure that the stair can be in them */
skip0:
	if (isstairseeker) {
		int steetries = 0;

		if (!Is_botlevel(&u.uz)) {
			while (steetries < 50000) {
				steetries++;
				x = rnd(COLNO-1);
				y = rn2(ROWNO);
				if (ACCESSIBLE(levl[x][y].typ) && !(t_at(x,y) && ((t_at(x,y))->ttyp == MAGIC_PORTAL)) && (levl[x][y].typ == CORR || levl[x][y].typ == ROOM) && !On_stairs(x, y)) break; /* we got a good location! */
			}
			if (!ACCESSIBLE(levl[x][y].typ) || (t_at(x,y) && ((t_at(x,y))->ttyp == MAGIC_PORTAL)) || On_stairs(x, y)) {
				croom = &rooms[rn2(nroom)];
				mkstairs(somex(croom), somey(croom), 0, croom);	/* down */
			} else {
				mkstairs(x, y, 0, (struct mkroom *)0);	/* down */
			}
		}

		if (u.uz.dlevel != 1) {
			steetries = 0;
			while (steetries < 50000) {
				steetries++;
				x = rnd(COLNO-1);
				y = rn2(ROWNO);
				if (ACCESSIBLE(levl[x][y].typ) && !(t_at(x,y) && ((t_at(x,y))->ttyp == MAGIC_PORTAL)) && (levl[x][y].typ == CORR || levl[x][y].typ == ROOM) && !On_stairs(x, y)) break; /* we got a good location! */
			}
			if (!ACCESSIBLE(levl[x][y].typ) || (t_at(x,y) && ((t_at(x,y))->ttyp == MAGIC_PORTAL)) || On_stairs(x, y)) {
				xchar sx, sy;
				register int stairattempts = 0; /* see reallyoccupied function --Amy */
				do {
					sx = somex(croom);
					sy = somey(croom);
					stairattempts++;
				} while((stairattempts > 50000) ? reallyoccupied(sx, sy) : occupied(sx, sy));
				mkstairs(sx, sy, 1, croom);	/* up */
			} else {
				mkstairs(x, y, 1, (struct mkroom *)0);	/* up */
			}
		}

	} /* end stairseeker code */

#ifdef REINCARNATION
	if (Is_rogue_level(&u.uz)) goto skip1;
#endif

	if (!rn2(5)) make_ironbarwalls(rn2(20) ? rn2(20) : rn2(50));

	/* make a secret treasure vault, not connected to the rest */
	if(do_vault()) {
		xchar w,h;
#ifdef DEBUG
		debugpline("trying to make a vault...");
#endif
		w = 1;
		h = 1;
		if (check_room(&vault_x, &w, &vault_y, &h, TRUE, FALSE)) {
		    fill_vault:
			add_room(vault_x, vault_y, vault_x+w,
				 vault_y+h, TRUE, VAULT, FALSE, FALSE, FALSE);
			level.flags.has_vault = 1;
			++room_threshold;
			fill_room(&rooms[nroom - 1], FALSE);
			mk_knox_portal(vault_x+w, vault_y+h);
			if(!level.flags.noteleport && !Race_if(PM_STABILISATOR) && !rn2(3)) makevtele();
		} else if(rnd_rect() && create_vault()) {
			vault_x = rooms[nroom].lx;
			vault_y = rooms[nroom].ly;
			if (check_room(&vault_x, &w, &vault_y, &h, TRUE, FALSE))
				goto fill_vault;
			else
				rooms[nroom].hx = -1;
		}
	}

    {
	register int u_depth = depth(&u.uz);
	boolean gehxtra = FALSE;
	boolean gehxtra2 = FALSE;

#ifdef WIZARD
	if(wizard && nh_getenv("SHOPTYPE")) mkroom(SHOPBASE); else
#endif
	if (u_depth > 1 /*&&
	    u_depth < depth(&medusa_level)*/ &&
	    nroom >= room_threshold && /* shops were too uncommon; change by Amy */
	    ((rn2(u_depth) < 3) || (rn2(u_depth) < 2)) ) mkroom(SHOPBASE);
 
	if (!rn2(20)) mkroom(SHOPBASE);

	/* [Tom] totally reorganized this into categories... used
	   to be only one special room on a level... now allows
	   one of each major type */
	/*else {*/

	if (at_dgn_entrance("Green Cross")) mkroom(GREENCROSSROOM);

	/* "Make special rooms show up later. Reverted existant special room chances to their original values, new rooms have values that I believe work.  If not, they will be adjusted accordingly." The existing values do work, too; some players just don't seem to know what to do if there's a room filled with trolls on dlvl2: stay FAR away from there until you can handle it! But sure, in Soviet Russia they start appearing later. All the other races will have to contend with my intended values though. --Amy */

gehennomxtra:

	    /* courtrooms & barracks */

	if (rn2(4)) {

	    if(depth(&u.uz) > (issoviet ? 4 : 3) && (ishaxor ? !rn2(6) : !rn2(12))) mkroom(COURT);
		else if (depth(&u.uz) > (issoviet ? 5 : 4) && (ishaxor ? !rn2(5) : !rn2(9))) mkroom(LEPREHALL);
	    else if(depth(&u.uz) > (issoviet ? 14 : 10) && (ishaxor ? !rn2(15) : !rn2(30))) mkroom(GIANTCOURT);
	    else if(depth(&u.uz) > (issoviet ? 14 : 7) && (ishaxor ? !rn2(20) : !rn2(40))) mkroom(WIZARDSDORM);
	    else if(depth(&u.uz) > (issoviet ? 16 : 10) && (ishaxor ? !rn2(13) : !rn2(26))) mkroom(BARRACKS);
	    else if(depth(&u.uz) > (issoviet ? 9 : 3) && (ishaxor ? !rn2(18) : !rn2(36))) mkroom(VERMINROOM);
	    else if(depth(&u.uz) > (issoviet ? 14 : 10) && (ishaxor ? !rn2(18) : !rn2(36))) mkroom(DOOMEDBARRACKS);
	else if (u_depth > (issoviet ? 12 : 3) && (ishaxor ? !rn2(24) : !rn2(48))) mkroom(HUMANHALL);
	else if (u_depth > (issoviet ? 5 : 1) && (ishaxor ? !rn2(30) : !rn2(60))) mkroom(FULLROOM);
	else if (u_depth > (issoviet ? 20 : 5) && (ishaxor ? !rn2(45) : !rn2(90))) mkroom(RUINEDCHURCH);
	else if (u_depth > (issoviet ? 30 : 20) && (ishaxor ? !rn2(50) : !rn2(100))) mkroom(QUESTORROOM);
	else if (u_depth > (issoviet ? 12 : 3) && (ishaxor ? !rn2(15) : !rn2(30))) mkroom(WEAPONCHAMBER);
	else if (u_depth > (issoviet ? 5 : 1) && (ishaxor ? !rn2(29) : !rn2(58))) mkroom(TRAPROOM);
	else if (u_depth > (issoviet ? 24 : 5) && (ishaxor ? !rn2(29) : !rn2(58))) mkroom(ILLUSIONROOM);
	else if (u_depth > (issoviet ? 12 : 5) && (ishaxor ? !rn2(100) : !rn2(200))) mkroom(ARDUOUSMOUNTAIN);
	else if (u_depth > (issoviet ? 8 : 1) && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(TENSHALL);
	else if (u_depth > (issoviet ? 25 : 1) && (Role_if(PM_CAMPERSTRIKER) ? !rn2(10) : (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(INSIDEROOM);
	else if (u_depth > (issoviet ? 15 : 1) && (Role_if(PM_CAMPERSTRIKER) ? !rn2(10) : Role_if(PM_GANG_SCHOLAR) ? !rn2(10) : (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(RIVERROOM);

	}

	    /* hives */

	if (rn2(4)) {

	    if(depth(&u.uz) > (issoviet ? 9 : 5) && (ishaxor ? !rn2(11) : !rn2(22))) mkroom(BEEHIVE);
	    else if(depth(&u.uz) > (issoviet ? 12 : 4) && (ishaxor ? !rn2(19) : !rn2(37))) mkroom(PRISONCHAMBER);
	else if (u_depth > (issoviet ? 12 : 8) && (ishaxor ? !rn2(6) : !rn2(12))) mkroom(ANTHOLE);
	else if (u_depth > (issoviet ? 10 : 4) && (ishaxor ? !rn2(12) : !rn2(24))) mkroom(SLEEPINGROOM);
	else if (u_depth > (issoviet ? 12 : 3) && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(NYMPHHALL);
	else if (u_depth > (issoviet ? 20 : 1) && (ishaxor ? !rn2(33) : !rn2(66))) mkroom(VARIANTROOM);
	else if (u_depth > (issoviet ? 15 : 1) && (ishaxor ? !rn2(33) : !rn2(66))) mkroom(GAMECORNER);
	else if (u_depth > (issoviet ? 12 : 3) && (ishaxor ? !rn2(25) : !rn2(50))) mkroom(MEADOWROOM);
	else if (u_depth > (issoviet ? 8 : 2) && (ishaxor ? !rn2(25) : !rn2(50))) mkroom(CENTRALTEDIUM);
	else if (u_depth > (issoviet ? 7 : 2) && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(COINHALL);
	else if (u_depth > (issoviet ? 7 : 2) && (ishaxor ? !rn2(20) : !rn2(40))) mkroom(HAMLETROOM);
	else if (u_depth > (issoviet ? 7 : 2) && (ishaxor ? !rn2(20) : !rn2(40))) mkroom(GREENCROSSROOM);
	else if (u_depth > (issoviet ? 16 : 7) && (ishaxor ? !rn2(10) : !rn2(20))) mkroom(ARMORY);
	else if (u_depth > (issoviet ? 15 : 1) && (ishaxor ? !rn2(17) : !rn2(34))) mkroom(POOLROOM);
	else if (u_depth > (issoviet ? 25 : 3) && (ishaxor ? !rn2(34) : !rn2(68))) mkroom(MIXEDPOOL);
	else if (u_depth > (issoviet ? 8 : 1) && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(TENSHALL);
	else if (u_depth > (issoviet ? 25 : 1) && (Role_if(PM_CAMPERSTRIKER) ? !rn2(10) : (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(INSIDEROOM);
	else if (u_depth > (issoviet ? 15 : 1) && (Role_if(PM_CAMPERSTRIKER) ? !rn2(10) : Role_if(PM_GANG_SCHOLAR) ? !rn2(10) : (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(RIVERROOM);

	}

	    /* zoos */

	if (rn2(4)) {

	    if(depth(&u.uz) > (issoviet ? 6 : 5) && (ishaxor ? !rn2(6) : !rn2(12))) mkroom(ZOO);
	    /* fungus farms are rare... */
	    else if (u_depth > (issoviet ? 7 : 3) && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(FUNGUSFARM);
	    else if(depth(&u.uz) > (issoviet ? 9 : 7) && (ishaxor ? !rn2(15) : !rn2(30))) mkroom(REALZOO);
	    else if(depth(&u.uz) > (issoviet ? 9 : 4) && (ishaxor ? !rn2(20) : !rn2(40))) mkroom(MENAGERIE);
	    else if(depth(&u.uz) > (issoviet ? 17 : 9) && (ishaxor ? !rn2(15) : !rn2(30))) mkroom(ELEMHALL);
	    else if(depth(&u.uz) > (issoviet ? 25 : 12) && (ishaxor ? !rn2(25) : !rn2(50))) mkroom(ANGELHALL);
	    else if(depth(&u.uz) > (issoviet ? 8 : 1) && (ishaxor ? !rn2(18) : !rn2(36))) mkroom(KOPSTATION);
	    else if(depth(&u.uz) > (issoviet ? 9 : 2) && (ishaxor ? !rn2(13) : !rn2(26))) mkroom(MIMICHALL);
	    else if(depth(&u.uz) > (issoviet ? 20 : 2) && (ishaxor ? !rn2(38) : !rn2(76))) mkroom(SANITATIONCENTRAL);
	    else if(depth(&u.uz) > (issoviet ? 20 : 2) && (ishaxor ? !rn2(38) : !rn2(76))) mkroom(NUCLEARCHAMBER);
	    else if(depth(&u.uz) > (issoviet ? 22 : 14) && (ishaxor ? !rn2(40) : !rn2(80))) mkroom(LETTERSALADROOM);
	else if (u_depth > (issoviet ? 7 : 3) && (ishaxor ? !rn2(12) : !rn2(24))) mkroom(SPIDERHALL);
	else if (u_depth > (issoviet ? 7 : 3) && (ishaxor ? !rn2(17) : !rn2(33))) mkroom(EXHIBITROOM);
	else if (u_depth > (issoviet ? 5 : 1) && (ishaxor ? !rn2(50) : !rn2(100))) mkroom(BOSSROOM);
	else if (u_depth > (issoviet ? 5 : 1) && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(STATUEROOM);
	else if (u_depth > (issoviet ? 8 : 1) && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(TENSHALL);
	else if (u_depth > (issoviet ? 25 : 1) && (Role_if(PM_CAMPERSTRIKER) ? !rn2(10) : (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(INSIDEROOM);
	else if (u_depth > (issoviet ? 15 : 1) && (Role_if(PM_CAMPERSTRIKER) ? !rn2(10) : Role_if(PM_GANG_SCHOLAR) ? !rn2(10) : (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(RIVERROOM);

	}

	    /* neat rooms */

	if (rn2(4)) {

	    if(depth(&u.uz) > (issoviet ? 8 : 1) && (ishaxor ? !rn2(5) : !rn2(10))) mkroom(TEMPLE);
	    else if(depth(&u.uz) > (issoviet ? 10 : 5) && (ishaxor ? !rn2(25) : !rn2(50))) mkroom(RELIGIONCENTER);
	    else if(depth(&u.uz) > (issoviet ? 8 : 5) && (ishaxor ? !rn2(8) : !rn2(16))) mkroom(ROBBERCAVE);
	    else if(depth(&u.uz) > (issoviet ? 11 : 4) && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(MORGUE);
	    else if(depth(&u.uz) > (issoviet ? 17 : 6) && (ishaxor ? !rn2(28) : !rn2(56))) mkroom(CRYPTROOM);
	    else if(depth(&u.uz) > (issoviet ? 24 : 7) && (ishaxor ? !rn2(28) : !rn2(56))) mkroom(CURSEDMUMMYROOM);
	    else if(depth(&u.uz) > (issoviet ? 13 : 1) && (ishaxor ? !rn2(8) : !rn2(15))) mkroom(BADFOODSHOP);
	    else if(depth(&u.uz) > (issoviet ? 18 : 1) && (ishaxor ? !rn2(18) : !rn2(36))) mkroom(SWAMP);
	    else if(depth(&u.uz) > (issoviet ? 26 : 5) && (ishaxor ? !rn2(75) : !rn2(150))) mkroom(MIRASPA);
          else if(depth(&u.uz) > (issoviet ? 15 : 1) && (ishaxor ? !rn2(90) : !rn2(150))) mkroom(CASINOROOM);
	    else if(depth(&u.uz) > (issoviet ? 18 : 1) && (ishaxor ? !rn2(90) : !rn2(150))) mkroom(DIVERPARADISE);
	    else if(depth(&u.uz) > (issoviet ? 18 : 1) && (ishaxor ? !rn2(60) : !rn2(120))) mkroom(SHOWERROOM);
        else if (depth(&u.uz) > (issoviet ? 20 : 10) && (ishaxor ? !rn2(30) : !rn2(60))) mkroom(CLINIC); /*supposed to be very rare --Amy*/
        else if (depth(&u.uz) > (issoviet ? 20 : 10) && (ishaxor ? !rn2(30) : !rn2(60))) mkroom(THE_AREA_ROOM);
        else if (depth(&u.uz) > (issoviet ? 13 : 5) && (ishaxor ? !rn2(20) : !rn2(40))) mkroom(COOLINGCHAMBER);
        else if (depth(&u.uz) > (issoviet ? 5 : 1) && (ishaxor ? !rn2(30) : !rn2(60))) mkroom(EMPTYNEST);
        else if (depth(&u.uz) > (issoviet ? 5 : 1) && (ishaxor ? !rn2(58) : !rn2(116))) mkroom(EMPTYDESERT);
        else if (depth(&u.uz) > (issoviet ? 10 : 1) && (ishaxor ? !rn2(58) : !rn2(116))) mkroom(RAMPAGEROOM);
        else if (depth(&u.uz) > (issoviet ? 30 : 3) && (ishaxor ? !rn2(28) : !rn2(48))) mkroom(TERRORHALL);
        else if (depth(&u.uz) > (issoviet ? 30 : 4) && (ishaxor ? !rn2(50) : !rn2(100))) mkroom(TROUBLEZONE);
        else if (depth(&u.uz) > (issoviet ? 20 : 10) && (ishaxor ? !rn2(50) : !rn2(100))) mkroom(EVILROOM);
        else if (depth(&u.uz) > (issoviet ? 22 : 10) && (ishaxor ? !rn2(20) : !rn2(40))) mkroom(TROLLHALL);
        else if (depth(&u.uz) > (issoviet ? 27 : 1) && (ishaxor ? !rn2(48) : !rn2(96))) mkroom(LEVELFFROOM);
        else if (depth(&u.uz) > (issoviet ? 14 : 5) && (ishaxor ? !rn2(33) : !rn2(66))) mkroom(RNGCENTER);
 	else if(depth(&u.uz) > (issoviet ? 20 : 10) && (ishaxor ? !rn2(8) : !rn2(16))) mkroom(DOUGROOM);
	else if (u_depth > (issoviet ? 8 : 1) && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(TENSHALL);
	else if (u_depth > (issoviet ? 25 : 1) && (Role_if(PM_CAMPERSTRIKER) ? !rn2(10) : (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(INSIDEROOM);
	else if (u_depth > (issoviet ? 15 : 1) && (Role_if(PM_CAMPERSTRIKER) ? !rn2(10) : Role_if(PM_GANG_SCHOLAR) ? !rn2(10) : (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(RIVERROOM);

	}

	    /* dangerous ones */

	if (rn2(4)) {

	    if (u_depth > (issoviet ? 16 : 10) && (ishaxor ? !rn2(17) : !rn2(33))) mkroom(COCKNEST);
	    else if(depth(&u.uz) > (issoviet ? 20 : 2) && (ishaxor ? !rn2(18) : !rn2(36))) mkroom(DRAGONLAIR);
	    else if(depth(&u.uz) > (issoviet ? 10 : 1) && (ishaxor ? !rn2(24) : !rn2(47))) mkroom(LEVELSEVENTYROOM);
	    else if (u_depth > (issoviet ? 25 : 12) && (ishaxor ? !rn2(25) : !rn2(50))) mkroom(MIGOHIVE);
	    else if (u_depth > (issoviet ? 14 : 5) && (ishaxor ? !rn2(32) : !rn2(64))) mkroom(RARITYROOM);
	    /* [DS] the restriction of lemure pits to Gehennom means they're
	     *      never going to show up randomly (no random room+corridor
	     *      levels in Gehennom). Perhaps this should be removed? */
	    else if (depth(&u.uz) > (issoviet ? 15 : 5) && (ishaxor ? !rn2(14) : !rn2(28))) mkroom(LEMUREPIT);
	    else if (depth(&u.uz) > (issoviet ? 24 : 7) && (ishaxor ? !rn2(23) : !rn2(45))) mkroom(HELLPIT);
	else if (u_depth > (issoviet ? 12 : 3) && (ishaxor ? !rn2(25) : !rn2(50))) mkroom(GOLEMHALL);
	else if (u_depth > (issoviet ? 16 : 3) && (ishaxor ? !rn2(40) : !rn2(80))) mkroom(MACHINEROOM);
	else if (u_depth > (issoviet ? 26 : 8) && (ishaxor ? !rn2(35) : !rn2(70))) mkroom(VOIDROOM);
	else if (u_depth > (issoviet ? 20 : 10) && (ishaxor ? !rn2(40) : !rn2(80))) mkroom(PLAYERCENTRAL);
	else if (u_depth > (issoviet ? 8 : 1) && (ishaxor ? !rn2(35) : !rn2(70))) mkroom(GRUEROOM);
	else if (u_depth > (issoviet ? 19 : 4) && (ishaxor ? !rn2(35) : !rn2(70))) mkroom(CHAOSROOM);
	else if (u_depth > (issoviet ? 20 : 4) && (ishaxor ? !rn2(36) : !rn2(64))) mkroom(FEMINISMROOM);
	else if (u_depth > (issoviet ? 24 : 8) && (ishaxor ? !rn2(50) : !rn2(100))) mkroom(CHANGINGROOM);
	    else if (u_depth > (issoviet ? 5 : 1) && (ishaxor ? !rn2(100) : !rn2(200))) mkroom(NASTYCENTRAL);
	else if (u_depth > (issoviet ? 8 : 1) && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(TENSHALL);
	else if (u_depth > (issoviet ? 25 : 1) && (Role_if(PM_CAMPERSTRIKER) ? !rn2(10) : (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(INSIDEROOM);
	else if (u_depth > (issoviet ? 15 : 1) && (Role_if(PM_CAMPERSTRIKER) ? !rn2(10) : Role_if(PM_GANG_SCHOLAR) ? !rn2(10) : (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(RIVERROOM);

	}

		if (In_voiddungeon(&u.uz)) {
			mkroom(rn2(10) ? VOIDROOM : HELLPIT);
			mkroom(rn2(10) ? VOIDROOM : HELLPIT);
			mkroom(rn2(10) ? VOIDROOM : HELLPIT);
			mkroom(rn2(10) ? VOIDROOM : HELLPIT);
			mkroom(rn2(10) ? VOIDROOM : HELLPIT);
			mkroom(rn2(10) ? VOIDROOM : HELLPIT);
			mkroom(rn2(10) ? VOIDROOM : HELLPIT);
			mkroom(rn2(10) ? VOIDROOM : HELLPIT);
			mkroom(rn2(10) ? VOIDROOM : HELLPIT);
			mkroom(rn2(10) ? VOIDROOM : HELLPIT);
		}

		if (In_gammacaves(&u.uz) && rn2(2)) {
			mkroom(NUCLEARCHAMBER);
		}

		if (In_illusorycastle(&u.uz)) mkroom(ILLUSIONROOM);

		if (In_swimmingpool(&u.uz)) {
			mkroom(!rn2(3) ? MIXEDPOOL : rn2(10) ? DIVERPARADISE : SWAMP);
			mkroom(!rn2(3) ? MIXEDPOOL : rn2(10) ? DIVERPARADISE : SWAMP);
			mkroom(!rn2(3) ? MIXEDPOOL : rn2(10) ? DIVERPARADISE : SWAMP);
			mkroom(!rn2(3) ? MIXEDPOOL : rn2(10) ? DIVERPARADISE : SWAMP);
			mkroom(!rn2(3) ? MIXEDPOOL : rn2(10) ? DIVERPARADISE : SWAMP);
			mkroom(!rn2(3) ? MIXEDPOOL : rn2(10) ? DIVERPARADISE : SWAMP);
			mkroom(!rn2(3) ? MIXEDPOOL : rn2(10) ? DIVERPARADISE : SWAMP);
			mkroom(!rn2(3) ? MIXEDPOOL : rn2(10) ? DIVERPARADISE : SWAMP);
			mkroom(!rn2(3) ? MIXEDPOOL : rn2(10) ? DIVERPARADISE : SWAMP);
			mkroom(!rn2(3) ? MIXEDPOOL : rn2(10) ? DIVERPARADISE : SWAMP);
		}

		/* random rooms, which means a chance of getting several of the same type of room --Amy */
		while ((u_depth > 10 || (rn2(u_depth) && !rn2(20 - u_depth) ) ) && !rn2(ishaxor ? 7 : 15)) mkroom(RANDOMROOM);

	/* If we make a rooms-and-corridors level in Gehennom, or generally anywhere with a depth greater than castle,
	 * we'll have a chance of more special rooms because this part of the game is supposed to be harder --Amy */
	if (!rn2(3) && u_depth > 50 && !gehxtra) {
		gehxtra = TRUE;
		goto gehennomxtra;
	}
#ifdef BIGSLEX
	/* big dungeon levels need more special rooms on average or they'll get boring quickly... */
	if (!rn2(3) && !gehxtra2) {
		gehxtra2 = TRUE;
		goto gehennomxtra;
	}
#endif

		if ((isironman || RngeIronmanMode || In_netherrealm(&u.uz)) && (!rn2(10) || (u_depth > 1 && !(u.preversionmode && In_greencross(&u.uz) && (dunlev(&u.uz) == 1)) && !(iszapem && In_spacebase(&u.uz) && (dunlev(&u.uz) == 1)) ) ) ) {
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);

		}


	/*}*/

#ifdef REINCARNATION
skip1:
#endif
	/* Place multi-dungeon branch. */
	place_branch(branchp, 0, 0);

	/* for each room: put things inside */
	for(croom = rooms; croom->hx > 0; croom++) {
		/*if(croom->rtype != OROOM) continue;*/

		/* put a sleeping monster inside */
		/* Note: monster may be on the stairs. This cannot be
		   avoided: maybe the player fell through a trap door
		   while a monster was on the stairs. Conclusion:
		   we have to check for monsters on the stairs anyway. */
		if((u.uhave.amulet && !u.freeplaymode) || !rn2(3)) {
		    x = somex(croom); y = somey(croom);
		    if (!ishomicider) { tmonst = makemon((struct permonst *) 0, x, y, MM_MAYSLEEP);
		    if (tmonst && webmaker(tmonst->data) /*== &mons[PM_GIANT_SPIDER]*/ &&
			    !occupied(x, y))
			(void) maketrap(x, y, WEB, 25, TRUE);
		    }
		    if (ishomicider) (void) makerandomtrap_at(x, y, TRUE);
		}
		if(ishaxor && ((u.uhave.amulet && !u.freeplaymode) || !rn2(3)) ) {
		    x = somex(croom); y = somey(croom);
		    if (!ishomicider) { tmonst = makemon((struct permonst *) 0, x, y, MM_MAYSLEEP);
		    if (tmonst && webmaker(tmonst->data) /*== &mons[PM_GIANT_SPIDER]*/ &&
			    !occupied(x, y))
			(void) maketrap(x, y, WEB, 25, TRUE);
		    }
		    if (ishomicider) (void) makerandomtrap_at(x, y, TRUE);
		}
#ifdef BIGSLEX /* bigslex rooms are much larger, so we want a proportionally higher concentration of monsters --Amy */
		if(!rn2(3)) {
		    x = somex(croom); y = somey(croom);
		    if (!ishomicider) { tmonst = makemon((struct permonst *) 0, x, y, MM_MAYSLEEP);
		    if (tmonst && webmaker(tmonst->data) /*== &mons[PM_GIANT_SPIDER]*/ &&
			    !occupied(x, y))
			(void) maketrap(x, y, WEB, 25, TRUE);
		    }
		    if (ishomicider) (void) makerandomtrap_at(x, y, TRUE);
		}
		if(ishaxor && !rn2(3)) {
		    x = somex(croom); y = somey(croom);
		    if (!ishomicider) { tmonst = makemon((struct permonst *) 0, x, y, MM_MAYSLEEP);
		    if (tmonst && webmaker(tmonst->data) /*== &mons[PM_GIANT_SPIDER]*/ &&
			    !occupied(x, y))
			(void) maketrap(x, y, WEB, 25, TRUE);
		    }
		    if (ishomicider) (void) makerandomtrap_at(x, y, TRUE);
		}
		if(!rn2(3)) {
		    x = somex(croom); y = somey(croom);
		    if (!ishomicider) { tmonst = makemon((struct permonst *) 0, x, y, MM_MAYSLEEP);
		    if (tmonst && webmaker(tmonst->data) /*== &mons[PM_GIANT_SPIDER]*/ &&
			    !occupied(x, y))
			(void) maketrap(x, y, WEB, 25, TRUE);
		    }
		    if (ishomicider) (void) makerandomtrap_at(x, y, TRUE);
		}
		if(ishaxor && !rn2(3)) {
		    x = somex(croom); y = somey(croom);
		    if (!ishomicider) { tmonst = makemon((struct permonst *) 0, x, y, MM_MAYSLEEP);
		    if (tmonst && webmaker(tmonst->data) /*== &mons[PM_GIANT_SPIDER]*/ &&
			    !occupied(x, y))
			(void) maketrap(x, y, WEB, 25, TRUE);
		    }
		    if (ishomicider) (void) makerandomtrap_at(x, y, TRUE);
		}

#endif
		/* put traps and mimics inside */
		goldseen = FALSE;
		x = (rn1(5,10) - (level_difficulty()/10));
		if ((depth(&u.uz) > 2 || !In_dod(&u.uz)) && !rn2(3)) x /= 2;
		if ((depth(&u.uz) > 2 || !In_dod(&u.uz)) && !rn2(10)) x /= 5;
		if (x <= 1) x = 2;

		/* less traps in the very early game --Amy */
		if (!(depth(&u.uz) == 1 && In_dod(&u.uz) && rn2(3)) && !(depth(&u.uz) == 2 && In_dod(&u.uz) && rn2(2)) ) {
			while (!rn2(x))
				mktrap(0,0,croom,(coord*)0,TRUE);

			if(ishaxor) {
				while (!rn2(x))
					mktrap(0,0,croom,(coord*)0,TRUE);
			}
		}
#ifdef BIGSLEX /* also more traps for bigslex, but not more items; you already get more of those anyway --Amy */
		if (!(depth(&u.uz) == 1 && In_dod(&u.uz) && rn2(3)) && !(depth(&u.uz) == 2 && In_dod(&u.uz) && rn2(2)) ) {
			while (!rn2(x))
				mktrap(0,0,croom,(coord*)0,TRUE);

			if(ishaxor) {
				while (!rn2(x))
					mktrap(0,0,croom,(coord*)0,TRUE);
			}
		}
		if (!(depth(&u.uz) == 1 && In_dod(&u.uz) && rn2(3)) && !(depth(&u.uz) == 2 && In_dod(&u.uz) && rn2(2)) ) {
			while (!rn2(x))
				mktrap(0,0,croom,(coord*)0,TRUE);

			if(ishaxor) {
				while (!rn2(x))
					mktrap(0,0,croom,(coord*)0,TRUE);
			}
		}
#endif

		if (!goldseen && !rn2(3))
		    (void) mkgold(0L, somex(croom), somey(croom));
#ifdef REINCARNATION
		x = 400 - (depth(&u.uz) * 2);
		if (x < 5) x = 5;
		if(!rn2(x)) mkgrave(croom);

		if(ishaxor && !rn2(x)) mkgrave(croom);

		if(Is_rogue_level(&u.uz)) goto skip_nonrogue;
#endif
		if(!rn2(10)) mkfount(0,croom);
		if(ishaxor && !rn2(10)) mkfount(0,croom);

		if(!rn2(500)) mkthrone(croom); /* rare cause they may give wishes --Amy */
		if(ishaxor && !rn2(500)) mkthrone(croom); /* rare cause they may give wishes --Amy */

		if(!rn2(500)) mkpentagram(croom);
		if(ishaxor && !rn2(500)) mkpentagram(croom);

		if(!rn2(250)) mkwell(croom);
		if(ishaxor && !rn2(250)) mkwell(croom);

		if(!rn2(250)) mkpoisonedwell(croom);
		if(ishaxor && !rn2(250)) mkpoisonedwell(croom);

		if(!rn2(250)) mkwagon(croom);
		if(ishaxor && !rn2(250)) mkwagon(croom);

		if(!rn2(250)) mkburningwagon(croom);
		if(ishaxor && !rn2(250)) mkburningwagon(croom);

		if(!rn2(150)) mkwoodentable(croom);
		if(ishaxor && !rn2(150)) mkwoodentable(croom);

		if(!rn2(300)) mkcarvedbed(croom);
		if(ishaxor && !rn2(300)) mkcarvedbed(croom);

		if(!rn2(75)) mkstrawmattress(croom);
		if(ishaxor && !rn2(75)) mkstrawmattress(croom);

		if(!rn2(60)) {
		    mksink(croom);
		    if(!rn2(3)) mktoilet(croom);
		}

		if(ishaxor && !rn2(60)) {
		    mksink(croom);
		    if(!rn2(3)) mktoilet(croom);
		}
		if(!rn2(50)) {
		    mkstone(croom);
		    if(!rn2(3)) mkstone(croom);
		    if(!rn2(6)) mkstone(croom);
		    if(!rn2(10)) mkstone(croom);
		    if(!rn2(15)) mkstone(croom);
		    if(!rn2(20)) mkstone(croom);
		    if(!rn2(25)) mkstone(croom);
		} 

		if(ishaxor && !rn2(50)) {
		    mkstone(croom);
		    if(!rn2(3)) mkstone(croom);
		    if(!rn2(6)) mkstone(croom);
		    if(!rn2(10)) mkstone(croom);
		    if(!rn2(15)) mkstone(croom);
		    if(!rn2(20)) mkstone(croom);
		    if(!rn2(25)) mkstone(croom);
		} 

		if(!rn2(50)) mkstoneX(rnz(10),croom);
		if(!rn2(250)) mkstoneX(rnz(25),croom);

		if(ishaxor && !rn2(50)) mkstoneX(rnz(10),croom);
		if(ishaxor && !rn2(250)) mkstoneX(rnz(25),croom);

		if(!rn2(50)) {
		    mkice(croom);
		    if(!rn2(3)) mkice(croom);
		    if(!rn2(6)) mkice(croom);
		    if(!rn2(10)) mkice(croom);
		    if(!rn2(15)) mkice(croom);
		    if(!rn2(20)) mkice(croom);
		    if(!rn2(25)) mkice(croom);
		} 

		if(ishaxor && !rn2(50)) {
		    mkice(croom);
		    if(!rn2(3)) mkice(croom);
		    if(!rn2(6)) mkice(croom);
		    if(!rn2(10)) mkice(croom);
		    if(!rn2(15)) mkice(croom);
		    if(!rn2(20)) mkice(croom);
		    if(!rn2(25)) mkice(croom);
		} 

		if(!rn2(50)) mkiceX(rnz(10),croom);
		if(!rn2(250)) mkiceX(rnz(25),croom);

		if(ishaxor && !rn2(50)) mkiceX(rnz(10),croom);
		if(ishaxor && !rn2(250)) mkiceX(rnz(25),croom);

/*		if(!rn2(50)) {
		    mkdoor(croom);
		    if(!rn2(3)) mkdoor(croom);
		    if(!rn2(6)) mkdoor(croom);
		    if(!rn2(10)) mkdoor(croom);
		    if(!rn2(15)) mkdoor(croom);
		    if(!rn2(20)) mkdoor(croom);
		    if(!rn2(25)) mkdoor(croom);
		}*/

		if(!rn2(100)) {
		    mktree(croom);
		    if(!rn2(2)) mktree(croom);
		    if(!rn2(4)) mktree(croom);
		    if(!rn2(5)) mktree(croom);
		    if(!rn2(7)) mktree(croom);
		    if(!rn2(10)) mktree(croom);
		    if(!rn2(12)) mktree(croom);
		    if(!rn2(15)) mktree(croom);
		    if(!rn2(20)) mktree(croom);
		}

		if(ishaxor && !rn2(100)) {
		    mktree(croom);
		    if(!rn2(2)) mktree(croom);
		    if(!rn2(4)) mktree(croom);
		    if(!rn2(5)) mktree(croom);
		    if(!rn2(7)) mktree(croom);
		    if(!rn2(10)) mktree(croom);
		    if(!rn2(12)) mktree(croom);
		    if(!rn2(15)) mktree(croom);
		    if(!rn2(20)) mktree(croom);
		}

		if(!rn2(100)) mktreeX(rnz(10),croom);
		if(!rn2(500)) mktreeX(rnz(25),croom);

		if(ishaxor && !rn2(100)) mktreeX(rnz(10),croom);
		if(ishaxor && !rn2(500)) mktreeX(rnz(25),croom);

		if(!rn2(200)) {
		    mkpool(croom);
		    if(!rn2(2)) mkpool(croom);
		    if(!rn2(4)) mkpool(croom);
		    if(!rn2(8)) mkpool(croom);
		    if(!rn2(16)) mkpool(croom);
		    if(!rn2(32)) mkpool(croom);
		    if(!rn2(64)) mkpool(croom);
		    if(!rn2(128)) mkpool(croom);
		}

		if(ishaxor && !rn2(200)) {
		    mkpool(croom);
		    if(!rn2(2)) mkpool(croom);
		    if(!rn2(4)) mkpool(croom);
		    if(!rn2(8)) mkpool(croom);
		    if(!rn2(16)) mkpool(croom);
		    if(!rn2(32)) mkpool(croom);
		    if(!rn2(64)) mkpool(croom);
		    if(!rn2(128)) mkpool(croom);
		}

		if(!rn2(200)) mkpoolX(rnz(10),croom);
		if(!rn2(1000)) mkpoolX(rnz(25),croom);

		if(ishaxor && !rn2(200)) mkpoolX(rnz(10),croom);
		if(ishaxor && !rn2(1000)) mkpoolX(rnz(25),croom);

		if(!rn2(200)) {
		    mkcloud(croom);
		    if(!rn2(2)) mkcloud(croom);
		    if(!rn2(4)) mkcloud(croom);
		    if(!rn2(8)) mkcloud(croom);
		    if(!rn2(16)) mkcloud(croom);
		    if(!rn2(32)) mkcloud(croom);
		    if(!rn2(64)) mkcloud(croom);
		    if(!rn2(128)) mkcloud(croom);
		}

		if(ishaxor && !rn2(200)) {
		    mkcloud(croom);
		    if(!rn2(2)) mkcloud(croom);
		    if(!rn2(4)) mkcloud(croom);
		    if(!rn2(8)) mkcloud(croom);
		    if(!rn2(16)) mkcloud(croom);
		    if(!rn2(32)) mkcloud(croom);
		    if(!rn2(64)) mkcloud(croom);
		    if(!rn2(128)) mkcloud(croom);
		}

		if(!rn2(200)) mkcloudX(rnz(10),croom);
		if(!rn2(1000)) mkcloudX(rnz(25),croom);

		if(ishaxor && !rn2(200)) mkcloudX(rnz(10),croom);
		if(ishaxor && !rn2(1000)) mkcloudX(rnz(25),croom);

		if(!rn2(500)) {
		    mklavapool(croom);
		    if(!rn2(2)) mklavapool(croom);
		    if(!rn2(4)) mklavapool(croom);
		    if(!rn2(8)) mklavapool(croom);
		    if(!rn2(16)) mklavapool(croom);
		    if(!rn2(32)) mklavapool(croom);
		    if(!rn2(64)) mklavapool(croom);
		    if(!rn2(128)) mklavapool(croom);
		}

		if(ishaxor && !rn2(500)) {
		    mklavapool(croom);
		    if(!rn2(2)) mklavapool(croom);
		    if(!rn2(4)) mklavapool(croom);
		    if(!rn2(8)) mklavapool(croom);
		    if(!rn2(16)) mklavapool(croom);
		    if(!rn2(32)) mklavapool(croom);
		    if(!rn2(64)) mklavapool(croom);
		    if(!rn2(128)) mklavapool(croom);
		}

		if(!rn2(500)) mklavapoolX(rnz(10),croom);
		if(!rn2(2500)) mklavapoolX(rnz(25),croom);

		if(ishaxor && !rn2(500)) mklavapoolX(rnz(10),croom);
		if(ishaxor && !rn2(2500)) mklavapoolX(rnz(25),croom);

		if(!rn2(1000)) {
		    mkironbars(croom);
		    if(!rn2(3)) mkironbars(croom);
		    if(!rn2(9)) mkironbars(croom);
		    if(!rn2(27)) mkironbars(croom);
		    if(!rn2(81)) mkironbars(croom);
		}

		if(ishaxor && !rn2(1000)) {
		    mkironbars(croom);
		    if(!rn2(3)) mkironbars(croom);
		    if(!rn2(9)) mkironbars(croom);
		    if(!rn2(27)) mkironbars(croom);
		    if(!rn2(81)) mkironbars(croom);
		}

		if(!rn2(1000)) mkironbarsX(rnz(10),croom);
		if(!rn2(5000)) mkironbarsX(rnz(25),croom);

		if(ishaxor && !rn2(1000)) mkironbarsX(rnz(10),croom);
		if(ishaxor && !rn2(5000)) mkironbarsX(rnz(25),croom);

		if(!rn2(1000)) {
		    mkgravewall(croom);
		    if(!rn2(3)) mkgravewall(croom);
		    if(!rn2(9)) mkgravewall(croom);
		    if(!rn2(27)) mkgravewall(croom);
		    if(!rn2(81)) mkgravewall(croom);
		}

		if(ishaxor && !rn2(1000)) {
		    mkgravewall(croom);
		    if(!rn2(3)) mkgravewall(croom);
		    if(!rn2(9)) mkgravewall(croom);
		    if(!rn2(27)) mkgravewall(croom);
		    if(!rn2(81)) mkgravewall(croom);
		}

		if(!rn2(1000)) mkgravewallX(rnz(10),croom);
		if(!rn2(5000)) mkgravewallX(rnz(25),croom);

		if(ishaxor && !rn2(1000)) mkgravewallX(rnz(10),croom);
		if(ishaxor && !rn2(5000)) mkgravewallX(rnz(25),croom);

		if(!rn2(1000)) {
		    mksnow(croom);
		    if(!rn2(3)) mksnow(croom);
		    if(!rn2(9)) mksnow(croom);
		    if(!rn2(27)) mksnow(croom);
		    if(!rn2(81)) mksnow(croom);
		}

		if(ishaxor && !rn2(1000)) {
		    mksnow(croom);
		    if(!rn2(3)) mksnow(croom);
		    if(!rn2(9)) mksnow(croom);
		    if(!rn2(27)) mksnow(croom);
		    if(!rn2(81)) mksnow(croom);
		}

		if(!rn2(1000)) mksnowX(rnz(10),croom);
		if(!rn2(5000)) mksnowX(rnz(25),croom);

		if(ishaxor && !rn2(1000)) mksnowX(rnz(10),croom);
		if(ishaxor && !rn2(5000)) mksnowX(rnz(25),croom);

		if(!rn2(1000)) {
		    mkash(croom);
		    if(!rn2(3)) mkash(croom);
		    if(!rn2(9)) mkash(croom);
		    if(!rn2(27)) mkash(croom);
		    if(!rn2(81)) mkash(croom);
		}

		if(ishaxor && !rn2(1000)) {
		    mkash(croom);
		    if(!rn2(3)) mkash(croom);
		    if(!rn2(9)) mkash(croom);
		    if(!rn2(27)) mkash(croom);
		    if(!rn2(81)) mkash(croom);
		}

		if(!rn2(1000)) mkashX(rnz(10),croom);
		if(!rn2(5000)) mkashX(rnz(25),croom);

		if(ishaxor && !rn2(1000)) mkashX(rnz(10),croom);
		if(ishaxor && !rn2(5000)) mkashX(rnz(25),croom);

		if(!rn2(1000)) {
		    mksand(croom);
		    if(!rn2(3)) mksand(croom);
		    if(!rn2(9)) mksand(croom);
		    if(!rn2(27)) mksand(croom);
		    if(!rn2(81)) mksand(croom);
		}

		if(ishaxor && !rn2(1000)) {
		    mksand(croom);
		    if(!rn2(3)) mksand(croom);
		    if(!rn2(9)) mksand(croom);
		    if(!rn2(27)) mksand(croom);
		    if(!rn2(81)) mksand(croom);
		}

		if(!rn2(1000)) mksandX(rnz(10),croom);
		if(!rn2(5000)) mksandX(rnz(25),croom);

		if(ishaxor && !rn2(1000)) mksandX(rnz(10),croom);
		if(ishaxor && !rn2(5000)) mksandX(rnz(25),croom);

		if(!rn2(1000)) {
		    mkpavedfloor(croom);
		    if(!rn2(3)) mkpavedfloor(croom);
		    if(!rn2(9)) mkpavedfloor(croom);
		    if(!rn2(27)) mkpavedfloor(croom);
		    if(!rn2(81)) mkpavedfloor(croom);
		}

		if(ishaxor && !rn2(1000)) {
		    mkpavedfloor(croom);
		    if(!rn2(3)) mkpavedfloor(croom);
		    if(!rn2(9)) mkpavedfloor(croom);
		    if(!rn2(27)) mkpavedfloor(croom);
		    if(!rn2(81)) mkpavedfloor(croom);
		}

		if(!rn2(1000)) mkpavedfloorX(rnz(10),croom);
		if(!rn2(5000)) mkpavedfloorX(rnz(25),croom);

		if(ishaxor && !rn2(1000)) mkpavedfloorX(rnz(10),croom);
		if(ishaxor && !rn2(5000)) mkpavedfloorX(rnz(25),croom);

		if(!rn2(1000)) {
		    mkhighway(croom);
		    if(!rn2(3)) mkhighway(croom);
		    if(!rn2(9)) mkhighway(croom);
		    if(!rn2(27)) mkhighway(croom);
		    if(!rn2(81)) mkhighway(croom);
		}

		if(ishaxor && !rn2(1000)) {
		    mkhighway(croom);
		    if(!rn2(3)) mkhighway(croom);
		    if(!rn2(9)) mkhighway(croom);
		    if(!rn2(27)) mkhighway(croom);
		    if(!rn2(81)) mkhighway(croom);
		}

		if(!rn2(1000)) mkhighwayX(rnz(10),croom);
		if(!rn2(5000)) mkhighwayX(rnz(25),croom);

		if(ishaxor && !rn2(1000)) mkhighwayX(rnz(10),croom);
		if(ishaxor && !rn2(5000)) mkhighwayX(rnz(25),croom);

		if(!rn2(1000)) {
		    mkgrassland(croom);
		    if(!rn2(3)) mkgrassland(croom);
		    if(!rn2(9)) mkgrassland(croom);
		    if(!rn2(27)) mkgrassland(croom);
		    if(!rn2(81)) mkgrassland(croom);
		}

		if(ishaxor && !rn2(1000)) {
		    mkgrassland(croom);
		    if(!rn2(3)) mkgrassland(croom);
		    if(!rn2(9)) mkgrassland(croom);
		    if(!rn2(27)) mkgrassland(croom);
		    if(!rn2(81)) mkgrassland(croom);
		}

		if(!rn2(1000)) mkgrasslandX(rnz(10),croom);
		if(!rn2(5000)) mkgrasslandX(rnz(25),croom);

		if(ishaxor && !rn2(1000)) mkgrasslandX(rnz(10),croom);
		if(ishaxor && !rn2(5000)) mkgrasslandX(rnz(25),croom);

		if(!rn2(1000)) {
		    mknethermist(croom);
		    if(!rn2(3)) mknethermist(croom);
		    if(!rn2(9)) mknethermist(croom);
		    if(!rn2(27)) mknethermist(croom);
		    if(!rn2(81)) mknethermist(croom);
		}

		if(ishaxor && !rn2(1000)) {
		    mknethermist(croom);
		    if(!rn2(3)) mknethermist(croom);
		    if(!rn2(9)) mknethermist(croom);
		    if(!rn2(27)) mknethermist(croom);
		    if(!rn2(81)) mknethermist(croom);
		}

		if(!rn2(1000)) mknethermistX(rnz(10),croom);
		if(!rn2(5000)) mknethermistX(rnz(25),croom);

		if(ishaxor && !rn2(1000)) mknethermistX(rnz(10),croom);
		if(ishaxor && !rn2(5000)) mknethermistX(rnz(25),croom);

		if(!rn2(1000)) {
		    mkstalactite(croom);
		    if(!rn2(3)) mkstalactite(croom);
		    if(!rn2(9)) mkstalactite(croom);
		    if(!rn2(27)) mkstalactite(croom);
		    if(!rn2(81)) mkstalactite(croom);
		}

		if(ishaxor && !rn2(1000)) {
		    mkstalactite(croom);
		    if(!rn2(3)) mkstalactite(croom);
		    if(!rn2(9)) mkstalactite(croom);
		    if(!rn2(27)) mkstalactite(croom);
		    if(!rn2(81)) mkstalactite(croom);
		}

		if(!rn2(1000)) mkstalactiteX(rnz(10),croom);
		if(!rn2(5000)) mkstalactiteX(rnz(25),croom);

		if(ishaxor && !rn2(1000)) mkstalactiteX(rnz(10),croom);
		if(ishaxor && !rn2(5000)) mkstalactiteX(rnz(25),croom);

		if(!rn2(1000)) {
		    mkcryptfloor(croom);
		    if(!rn2(3)) mkcryptfloor(croom);
		    if(!rn2(9)) mkcryptfloor(croom);
		    if(!rn2(27)) mkcryptfloor(croom);
		    if(!rn2(81)) mkcryptfloor(croom);
		}

		if(ishaxor && !rn2(1000)) {
		    mkcryptfloor(croom);
		    if(!rn2(3)) mkcryptfloor(croom);
		    if(!rn2(9)) mkcryptfloor(croom);
		    if(!rn2(27)) mkcryptfloor(croom);
		    if(!rn2(81)) mkcryptfloor(croom);
		}

		if(!rn2(1000)) mkcryptfloorX(rnz(10),croom);
		if(!rn2(5000)) mkcryptfloorX(rnz(25),croom);

		if(ishaxor && !rn2(1000)) mkcryptfloorX(rnz(10),croom);
		if(ishaxor && !rn2(5000)) mkcryptfloorX(rnz(25),croom);

		if(!rn2(1000)) {
		    mkbubbles(croom);
		    if(!rn2(3)) mkbubbles(croom);
		    if(!rn2(9)) mkbubbles(croom);
		    if(!rn2(27)) mkbubbles(croom);
		    if(!rn2(81)) mkbubbles(croom);
		}

		if(ishaxor && !rn2(1000)) {
		    mkbubbles(croom);
		    if(!rn2(3)) mkbubbles(croom);
		    if(!rn2(9)) mkbubbles(croom);
		    if(!rn2(27)) mkbubbles(croom);
		    if(!rn2(81)) mkbubbles(croom);
		}

		if(!rn2(1000)) mkbubblesX(rnz(10),croom);
		if(!rn2(5000)) mkbubblesX(rnz(25),croom);

		if(ishaxor && !rn2(1000)) mkbubblesX(rnz(10),croom);
		if(ishaxor && !rn2(5000)) mkbubblesX(rnz(25),croom);

		if(!rn2(1000)) {
		    mkraincloud(croom);
		    if(!rn2(3)) mkraincloud(croom);
		    if(!rn2(9)) mkraincloud(croom);
		    if(!rn2(27)) mkraincloud(croom);
		    if(!rn2(81)) mkraincloud(croom);
		}

		if(ishaxor && !rn2(1000)) {
		    mkraincloud(croom);
		    if(!rn2(3)) mkraincloud(croom);
		    if(!rn2(9)) mkraincloud(croom);
		    if(!rn2(27)) mkraincloud(croom);
		    if(!rn2(81)) mkraincloud(croom);
		}

		if(!rn2(1000)) mkraincloudX(rnz(10),croom);
		if(!rn2(5000)) mkraincloudX(rnz(25),croom);

		if(ishaxor && !rn2(1000)) mkraincloudX(rnz(10),croom);
		if(ishaxor && !rn2(5000)) mkraincloudX(rnz(25),croom);

		if(!rn2(2000)) {
		    mkcrystalwater(croom);
		    if(!rn2(3)) mkcrystalwater(croom);
		    if(!rn2(9)) mkcrystalwater(croom);
		    if(!rn2(27)) mkcrystalwater(croom);
		    if(!rn2(81)) mkcrystalwater(croom);
		}

		if(ishaxor && !rn2(2000)) {
		    mkcrystalwater(croom);
		    if(!rn2(3)) mkcrystalwater(croom);
		    if(!rn2(9)) mkcrystalwater(croom);
		    if(!rn2(27)) mkcrystalwater(croom);
		    if(!rn2(81)) mkcrystalwater(croom);
		}

		if(!rn2(2000)) mkcrystalwaterX(rnz(10),croom);
		if(!rn2(10000)) mkcrystalwaterX(rnz(25),croom);

		if(ishaxor && !rn2(2000)) mkcrystalwaterX(rnz(10),croom);
		if(ishaxor && !rn2(10000)) mkcrystalwaterX(rnz(25),croom);

		if(!rn2(2000)) {
		    mkmoorland(croom);
		    if(!rn2(3)) mkmoorland(croom);
		    if(!rn2(9)) mkmoorland(croom);
		    if(!rn2(27)) mkmoorland(croom);
		    if(!rn2(81)) mkmoorland(croom);
		}

		if(ishaxor && !rn2(2000)) {
		    mkmoorland(croom);
		    if(!rn2(3)) mkmoorland(croom);
		    if(!rn2(9)) mkmoorland(croom);
		    if(!rn2(27)) mkmoorland(croom);
		    if(!rn2(81)) mkmoorland(croom);
		}

		if(!rn2(2000)) mkmoorlandX(rnz(10),croom);
		if(!rn2(10000)) mkmoorlandX(rnz(25),croom);

		if(ishaxor && !rn2(2000)) mkmoorlandX(rnz(10),croom);
		if(ishaxor && !rn2(10000)) mkmoorlandX(rnz(25),croom);

		if(!rn2(2000)) {
		    mkurinelake(croom);
		    if(!rn2(3)) mkurinelake(croom);
		    if(!rn2(9)) mkurinelake(croom);
		    if(!rn2(27)) mkurinelake(croom);
		    if(!rn2(81)) mkurinelake(croom);
		}

		if(ishaxor && !rn2(2000)) {
		    mkurinelake(croom);
		    if(!rn2(3)) mkurinelake(croom);
		    if(!rn2(9)) mkurinelake(croom);
		    if(!rn2(27)) mkurinelake(croom);
		    if(!rn2(81)) mkurinelake(croom);
		}

		if(!rn2(2000)) mkurinelakeX(rnz(10),croom);
		if(!rn2(10000)) mkurinelakeX(rnz(25),croom);

		if(ishaxor && !rn2(2000)) mkurinelakeX(rnz(10),croom);
		if(ishaxor && !rn2(10000)) mkurinelakeX(rnz(25),croom);

		if(!rn2(2000)) {
		    mkshiftingsand(croom);
		    if(!rn2(3)) mkshiftingsand(croom);
		    if(!rn2(9)) mkshiftingsand(croom);
		    if(!rn2(27)) mkshiftingsand(croom);
		    if(!rn2(81)) mkshiftingsand(croom);
		}

		if(ishaxor && !rn2(2000)) {
		    mkshiftingsand(croom);
		    if(!rn2(3)) mkshiftingsand(croom);
		    if(!rn2(9)) mkshiftingsand(croom);
		    if(!rn2(27)) mkshiftingsand(croom);
		    if(!rn2(81)) mkshiftingsand(croom);
		}

		if(!rn2(2000)) mkshiftingsandX(rnz(10),croom);
		if(!rn2(10000)) mkshiftingsandX(rnz(25),croom);

		if(ishaxor && !rn2(2000)) mkshiftingsandX(rnz(10),croom);
		if(ishaxor && !rn2(10000)) mkshiftingsandX(rnz(25),croom);

		if(!rn2(2000)) {
		    mkstyxriver(croom);
		    if(!rn2(3)) mkstyxriver(croom);
		    if(!rn2(9)) mkstyxriver(croom);
		    if(!rn2(27)) mkstyxriver(croom);
		    if(!rn2(81)) mkstyxriver(croom);
		}

		if(ishaxor && !rn2(2000)) {
		    mkstyxriver(croom);
		    if(!rn2(3)) mkstyxriver(croom);
		    if(!rn2(9)) mkstyxriver(croom);
		    if(!rn2(27)) mkstyxriver(croom);
		    if(!rn2(81)) mkstyxriver(croom);
		}

		if(!rn2(2000)) mkstyxriverX(rnz(10),croom);
		if(!rn2(10000)) mkstyxriverX(rnz(25),croom);

		if(ishaxor && !rn2(2000)) mkstyxriverX(rnz(10),croom);
		if(ishaxor && !rn2(10000)) mkstyxriverX(rnz(25),croom);

		if(!rn2(5000)) {
		    mktunnelwall(croom);
		    if(!rn2(3)) mktunnelwall(croom);
		    if(!rn2(9)) mktunnelwall(croom);
		    if(!rn2(27)) mktunnelwall(croom);
		    if(!rn2(81)) mktunnelwall(croom);
		}

		if(ishaxor && !rn2(5000)) {
		    mktunnelwall(croom);
		    if(!rn2(3)) mktunnelwall(croom);
		    if(!rn2(9)) mktunnelwall(croom);
		    if(!rn2(27)) mktunnelwall(croom);
		    if(!rn2(81)) mktunnelwall(croom);
		}

		if(!rn2(5000)) mktunnelwallX(rnz(10),croom);
		if(!rn2(20000)) mktunnelwallX(rnz(25),croom);

		if(ishaxor && !rn2(5000)) mktunnelwallX(rnz(10),croom);
		if(ishaxor && !rn2(20000)) mktunnelwallX(rnz(25),croom);

		if(!rn2(5000)) {
		    mkfarmland(croom);
		    if(!rn2(3)) mkfarmland(croom);
		    if(!rn2(9)) mkfarmland(croom);
		    if(!rn2(27)) mkfarmland(croom);
		    if(!rn2(81)) mkfarmland(croom);
		}

		if(ishaxor && !rn2(5000)) {
		    mkfarmland(croom);
		    if(!rn2(3)) mkfarmland(croom);
		    if(!rn2(9)) mkfarmland(croom);
		    if(!rn2(27)) mkfarmland(croom);
		    if(!rn2(81)) mkfarmland(croom);
		}

		if(!rn2(5000)) mkfarmlandX(rnz(10),croom);
		if(!rn2(20000)) mkfarmlandX(rnz(25),croom);

		if(ishaxor && !rn2(5000)) mkfarmlandX(rnz(10),croom);
		if(ishaxor && !rn2(20000)) mkfarmlandX(rnz(25),croom);

		if(!rn2(5000)) {
		    mkmountain(croom);
		    if(!rn2(3)) mkmountain(croom);
		    if(!rn2(9)) mkmountain(croom);
		    if(!rn2(27)) mkmountain(croom);
		    if(!rn2(81)) mkmountain(croom);
		}

		if(ishaxor && !rn2(5000)) {
		    mkmountain(croom);
		    if(!rn2(3)) mkmountain(croom);
		    if(!rn2(9)) mkmountain(croom);
		    if(!rn2(27)) mkmountain(croom);
		    if(!rn2(81)) mkmountain(croom);
		}

		if(!rn2(5000)) mkmountainX(rnz(10),croom);
		if(!rn2(20000)) mkmountainX(rnz(25),croom);

		if(ishaxor && !rn2(5000)) mkmountainX(rnz(10),croom);
		if(ishaxor && !rn2(20000)) mkmountainX(rnz(25),croom);

		if(!rn2(5000)) {
		    mkwatertunnel(croom);
		    if(!rn2(3)) mkwatertunnel(croom);
		    if(!rn2(9)) mkwatertunnel(croom);
		    if(!rn2(27)) mkwatertunnel(croom);
		    if(!rn2(81)) mkwatertunnel(croom);
		}

		if(ishaxor && !rn2(5000)) {
		    mkwatertunnel(croom);
		    if(!rn2(3)) mkwatertunnel(croom);
		    if(!rn2(9)) mkwatertunnel(croom);
		    if(!rn2(27)) mkwatertunnel(croom);
		    if(!rn2(81)) mkwatertunnel(croom);
		}

		if(!rn2(5000)) mkwatertunnelX(rnz(10),croom);
		if(!rn2(20000)) mkwatertunnelX(rnz(25),croom);

		if(ishaxor && !rn2(5000)) mkwatertunnelX(rnz(10),croom);
		if(ishaxor && !rn2(20000)) mkwatertunnelX(rnz(25),croom);

		x = 80 - (depth(&u.uz) * 2);
		if (x < 2) x = 2;
		if(!rn2(x)) mkgrave(croom);

		if(ishaxor && !rn2(x)) mkgrave(croom);

		if(!rn2(60)) mkaltar(croom);

		if(ishaxor && !rn2(60)) mkaltar(croom);

		/* put statues inside */
		if(!rn2(20))
		    (void) mkcorpstat(STATUE, (struct monst *)0,
				      (struct permonst *)0,
				      somex(croom), somey(croom), TRUE);
		/* put box/chest inside;
		 *  40% chance for at least 1 box, regardless of number
		 *  of rooms; about 5 - 7.5% for 2 boxes, least likely
		 *  when few rooms; chance for 3 or more is neglible.
		 */
		if(!rn2(nroom * 5 / 2))
		    (void) mksobj_at((rn2(3)) ? LARGE_BOX : CHEST, somex(croom), somey(croom), TRUE, FALSE, FALSE);

		/* maybe make some graffiti */
		if(!rn2(3 + 3 * abs(depth(&u.uz)))) {
		    char buf[BUFSZ];
		    const char *mesg = random_engraving(buf);
		    if (mesg) {
			do {
			    x = somex(croom);  y = somey(croom);
			} while(levl[x][y].typ != ROOM && !rn2(40));
			if (!(IS_POOL(levl[x][y].typ) ||
			      IS_FURNITURE(levl[x][y].typ)))
			    make_engr_at(x, y, mesg, 0L, (xchar)0);
		    }
		}

#ifdef REINCARNATION
	skip_nonrogue:
#endif

/* STEPHEN WHITE'S NEW CODE */
		if(!rn2(3) && timebasedlowerchance()) {
		    (void) mkobj_at(0, somex(croom), somey(croom), TRUE, FALSE);
		    tryct = 0;
		    while(!rn2(3)) {
			if(++tryct > 100) {
			    impossible("tryct overflow4");
			    break;
			}
			(void) mkobj_at(0, somex(croom), somey(croom), TRUE, FALSE);
		    }
		}

		if(ishaxor && timebasedlowerchance() && !rn2(3)) {
		    (void) mkobj_at(0, somex(croom), somey(croom), TRUE, FALSE);
		    tryct = 0;
		    while(!rn2(3)) {
			if(++tryct > 100) {
			    impossible("tryct overflow4");
			    break;
			}
			(void) mkobj_at(0, somex(croom), somey(croom), TRUE, FALSE);
		    }
		}

		/* some earlygame help... --Amy */
		if (moves == 1 && timebasedlowerchance() && !rn2(2)) {
		    (void) mkobj_at(0, somex(croom), somey(croom), TRUE, FALSE);
		    tryct = 0;
		    while(!rn2(3)) {
			if(++tryct > 100) {
			    impossible("tryct overflow4");
			    break;
			}
			(void) mkobj_at(0, somex(croom), somey(croom), TRUE, FALSE);
		    }
		}

		/* place additional items, monsters and traps, amount is random but higher average for bigger rooms --Amy */
		if (croom->rtype == OROOM) { /* only for regular rooms, please */
			int roomsize = (croom->hx - croom->lx + 1) * (croom->hy - croom->ly + 1);
			int helpcounter = roomsize;
			int helpvar = 0;
			int seedparameter;

			/* items */
			seedparameter = (75 + rn2(176));
			if (!rn2(2)) seedparameter *= rnd(10);
			if (seedparameter < 1) seedparameter = 1; /* fail safe */

			helpcounter = roomsize;
			helpvar = 0;

			while (helpcounter > 0) {
				if (helpcounter > seedparameter) {
					helpcounter -= seedparameter;
					helpvar++;
				} else {
					if (helpcounter > rn2(seedparameter)) helpvar++;
					helpcounter = 0;
				}
			}
			if (ishaxor) helpvar *= 2;
			if (helpvar > 0) helpvar = rn3(helpvar); /* rn3 for items because they can be useful, rest uses rn2 */
			if (!rn2(3)) helpvar = 0;
			while (helpvar > 0) {
				(void) mkobj_at(0, somex(croom), somey(croom), TRUE, FALSE);
				helpvar--;
			}

			/* monsters */
			seedparameter = (25 + rn2(51));
			if (!rn2(3)) seedparameter *= rnd(10);
			if (seedparameter < 1) seedparameter = 1; /* fail safe */

			helpcounter = roomsize;
			helpvar = 0;

			while (helpcounter > 0) {
				if (helpcounter > seedparameter) {
					helpcounter -= seedparameter;
					helpvar++;
				} else {
					if (helpcounter > rn2(seedparameter)) helpvar++;
					helpcounter = 0;
				}
			}
			if (ishaxor) helpvar *= 2;
			if (helpvar > 0) helpvar = rn2(helpvar);
			if (!rn2(10)) helpvar = 0;
			if (depth(&u.uz) == 1 && In_dod(&u.uz) && (helpvar > 0)) helpvar = rn2(helpvar);
			if (depth(&u.uz) == 2 && In_dod(&u.uz) && rn2(2) && (helpvar > 0)) helpvar = rn2(helpvar);

			while (helpvar > 0) {
				if (!ishomicider) (void) makemon((struct permonst *) 0, somex(croom), somey(croom), MM_MAYSLEEP);
				else (void) makerandomtrap_at(somex(croom), somey(croom), TRUE);
				helpvar--;
			}

			/* traps */
			seedparameter = (16 + rn2(61));
			if (!rn2(5)) seedparameter *= rnd(10);
			if (seedparameter < 1) seedparameter = 1; /* fail safe */

			helpcounter = roomsize;
			helpvar = 0;

			while (helpcounter > 0) {
				if (helpcounter > seedparameter) {
					helpcounter -= seedparameter;
					helpvar++;
				} else {
					if (helpcounter > rn2(seedparameter)) helpvar++;
					helpcounter = 0;
				}
			}
			if (ishaxor) helpvar *= 2;
			if (helpvar > 0) helpvar = rn2(helpvar);
			if (!rn2(5)) helpvar = 0;

			if (depth(&u.uz) == 1 && In_dod(&u.uz) && (helpvar > 0)) helpvar = rn2(helpvar);
			if (depth(&u.uz) == 2 && In_dod(&u.uz) && rn2(2) && (helpvar > 0)) helpvar = rn2(helpvar);

			while (helpvar > 0) {
				(void) makerandomtrap_at(somex(croom), somey(croom), TRUE);
				helpvar--;
			}
		}

	}

   }
}

/*
 *	Place deposits of minerals (gold and misc gems) in the stone
 *	surrounding the rooms on the map.
 *	Also place kelp in water.
 */

STATIC_OVL void
mineralize()
{
	s_level *sp;
	struct obj *otmp;
	int goldprob, goldamount, gemprob, objprob, x, y, cnt;
      const char *str;

	int density = 3;
	if (!rn2(5)) density += 1;
	if (!rn2(10)) density += rnd(3);
	if (!rn2(25)) density += rnd(5);
	if (!rn2(125)) density += rnd(10);
	if (!rn2(750)) density += rnd(20);

	int trapdensity = rnd(100);
	if (rn2(3)) trapdensity /= 10;
	if (!rn2(5)) trapdensity = 0;

	register struct obj *otmpX;
	register int tryct = 0;

	int otherwalltype = STONE;
	boolean otherwallxtra = 0;
	if (!rn2(iswarper ? 100 : 500)) {

		switch (rnd(28)) {

			case 1:
				otherwalltype = TREE; break;
			case 2:
				otherwalltype = MOAT; break;
			case 3:
				otherwalltype = LAVAPOOL; break;
			case 4:
				otherwalltype = IRONBARS; break;
			case 5:
				otherwalltype = CORR; break;
			case 6:
				otherwalltype = ICE; break;
			case 7:
				otherwalltype = CLOUD; break;
			case 8:
				otherwalltype = RAINCLOUD; break;
			case 9:
				otherwalltype = GRAVEWALL; break;
			case 10:
				otherwalltype = TUNNELWALL; break;
			case 11:
				otherwalltype = FARMLAND; break;
			case 12:
				otherwalltype = MOUNTAIN; break;
			case 13:
				otherwalltype = WATERTUNNEL; break;
			case 14:
				otherwalltype = CRYSTALWATER; break;
			case 15:
				otherwalltype = MOORLAND; break;
			case 16:
				otherwalltype = URINELAKE; break;
			case 17:
				otherwalltype = SHIFTINGSAND; break;
			case 18:
				otherwalltype = STYXRIVER; break;
			case 19:
				otherwalltype = SNOW; break;
			case 20:
				otherwalltype = ASH; break;
			case 21:
				otherwalltype = SAND; break;
			case 22:
				otherwalltype = PAVEDFLOOR; break;
			case 23:
				otherwalltype = HIGHWAY; break;
			case 24:
				otherwalltype = GRASSLAND; break;
			case 25:
				otherwalltype = NETHERMIST; break;
			case 26:
				otherwalltype = STALACTITE; break;
			case 27:
				otherwalltype = CRYPTFLOOR; break;
			case 28:
				otherwalltype = BUBBLES; break;
		}

		if (!rn2(50)) otherwallxtra = 1;

	}

	/* Place kelp, except on the plane of water */
	if (In_endgame(&u.uz)) return;
	for (x = 1; x < (COLNO); x++)
	    for (y = 0; y < (ROWNO); y++) {

		if ((levl[x][y].typ == STONE) && otherwalltype && (rnd(density) < 4) ) {

			if (otherwallxtra) {
				switch (rnd(29)) {

					case 1:
						otherwalltype = TREE; break;
					case 2:
						otherwalltype = MOAT; break;
					case 3:
						otherwalltype = LAVAPOOL; break;
					case 4:
						otherwalltype = IRONBARS; break;
					case 5:
						otherwalltype = CORR; break;
					case 6:
						otherwalltype = ICE; break;
					case 7:
						otherwalltype = CLOUD; break;
					case 8:
						otherwalltype = RAINCLOUD; break;
					case 9:
						otherwalltype = GRAVEWALL; break;
					case 10:
						otherwalltype = TUNNELWALL; break;
					case 11:
						otherwalltype = FARMLAND; break;
					case 12:
						otherwalltype = MOUNTAIN; break;
					case 13:
						otherwalltype = WATERTUNNEL; break;
					case 14:
						otherwalltype = CRYSTALWATER; break;
					case 15:
						otherwalltype = MOORLAND; break;
					case 16:
						otherwalltype = URINELAKE; break;
					case 17:
						otherwalltype = SHIFTINGSAND; break;
					case 18:
						otherwalltype = STYXRIVER; break;
					case 19:
						otherwalltype = SNOW; break;
					case 20:
						otherwalltype = ASH; break;
					case 21:
						otherwalltype = SAND; break;
					case 22:
						otherwalltype = PAVEDFLOOR; break;
					case 23:
						otherwalltype = HIGHWAY; break;
					case 24:
						otherwalltype = GRASSLAND; break;
					case 25:
						otherwalltype = NETHERMIST; break;
					case 26:
						otherwalltype = STALACTITE; break;
					case 27:
						otherwalltype = CRYPTFLOOR; break;
					case 28:
						otherwalltype = BUBBLES; break;
					case 29:
						otherwalltype = ROCKWALL; break;
				}
			}

			levl[x][y].typ = otherwalltype;

			if (!rn2(ishaxor ? 10000 : 20000))
				levl[x][y].typ = THRONE;
			else if (!rn2(ishaxor ? 50000 : 100000))
				levl[x][y].typ = PENTAGRAM;
			else if (!rn2(ishaxor ? 25000 : 50000))
				levl[x][y].typ = WELL;
			else if (!rn2(ishaxor ? 25000 : 50000))
				levl[x][y].typ = POISONEDWELL;
			else if (!rn2(ishaxor ? 25000 : 50000))
				levl[x][y].typ = WAGON;
			else if (!rn2(ishaxor ? 25000 : 50000))
				levl[x][y].typ = BURNINGWAGON;
			else if (!rn2(ishaxor ? 20000 : 40000))
				levl[x][y].typ = WOODENTABLE;
			else if (!rn2(ishaxor ? 25000 : 50000))
				levl[x][y].typ = CARVEDBED;
			else if (!rn2(ishaxor ? 10000 : 20000))
				levl[x][y].typ = STRAWMATTRESS;
			else if (!rn2(ishaxor ? 2500 : 5000)) {
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				}
			else if (!rn2(ishaxor ? 2500 : 5000)) {
				levl[x][y].typ = SINK;
				level.flags.nsinks++;
				}
			else if (!rn2(ishaxor ? 5000 : 10000))
				levl[x][y].typ = TOILET;
			else if (!rn2(ishaxor ? 1000 : 2000)) {
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
			else if (!rn2(ishaxor ? 10000 : 20000)) {
				levl[x][y].typ = ALTAR;
				if (rn2(10)) levl[x][y].altarmask = Align2amask( A_NONE );
				else switch (rnd(3)) {
		
				case 1: levl[x][y].altarmask = Align2amask( A_LAWFUL ); break;
				case 2: levl[x][y].altarmask = Align2amask( A_NEUTRAL ); break;
				case 3: levl[x][y].altarmask = Align2amask( A_CHAOTIC ); break;
		
				}
			}

		}

		if ((levl[x][y].typ <= DBWALL && !rn2(100000)) && !t_at(x, y) ) {
			maketrap(x, y, SUPERTHING_TRAP, 0, FALSE);
		}

		if ((levl[x][y].typ <= DBWALL && Role_if(PM_SPACEWARS_FIGHTER) && !rn2(10000)) && !t_at(x, y) ) {
			maketrap(x, y, SUPERTHING_TRAP, 0, FALSE);
		}

		if ((levl[x][y].typ <= DBWALL && Role_if(PM_CAMPERSTRIKER) && !rn2(5000)) && !t_at(x, y) ) {
			maketrap(x, y, SUPERTHING_TRAP, 0, FALSE);
		}

		if ((levl[x][y].typ <= DBWALL && !rn2(100000)) && !t_at(x, y) ) {
			maketrap(x, y, ARABELLA_SPEAKER, 0, FALSE);
		}

		if ((levl[x][y].typ <= DBWALL && Role_if(PM_SPACEWARS_FIGHTER) && !rn2(5000)) && !t_at(x, y) ) {
			maketrap(x, y, ARABELLA_SPEAKER, 0, FALSE);
		}

		if ((levl[x][y].typ <= DBWALL && Role_if(PM_CAMPERSTRIKER) && !rn2(2500)) && !t_at(x, y) ) {
			maketrap(x, y, ARABELLA_SPEAKER, 0, FALSE);
		}

		/* Since you can now pick up items from the bottom with swimming, let's reduce the amount of kelp --Amy */
		if (((levl[x][y].typ == POOL && !rn2(150)) || (levl[x][y].typ == MOAT && !rn2(50))) && timebasedlowerchance())
	    	    (void)mksobj_at(KELP_FROND, x, y, TRUE, FALSE, FALSE);

		/* locate level for camperstriker role should be filled end to end with traps on trees */
		if (levl[x][y].typ == TREE && Role_if(PM_CAMPERSTRIKER) && !rn2(10) && Is_qlocate(&u.uz) )
			makerandomtrap_at(x, y, TRUE);

		/* give a random, low, chance that any given square has a trap --Amy */

		if ((!(In_sokoban(&u.uz)) || (levl[x][y].typ != CORR && levl[x][y].typ != ROOM)) && !(depth(&u.uz) == 1 && In_dod(&u.uz) && rn2(3)) && !(depth(&u.uz) == 2 && (trapdensity > rn2(100)) && In_dod(&u.uz) && rn2(2)) ) {

			int reduceramount = (level_difficulty() / 5);
			if (reduceramount < 1) reduceramount = 1;

		    	if (!rn2(25000 / reduceramount)) makerandomtrap_at(x, y, TRUE);
		    	if (ishaxor && !rn2(25000 / reduceramount)) makerandomtrap_at(x, y, TRUE);

		    	if (ishomicider && !rn2(10000 / reduceramount)) makerandomtrap_at(x, y, TRUE);
		    	if (Role_if(PM_GANG_SCHOLAR) && !rn2(15000 / reduceramount)) makerandomtrap_at(x, y, TRUE);

		}

		if (!ishomicider) {

		/* Random sea monsters if there is water. --Amy */

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 25 : (issuxxor && !ishaxor) ? 100 : 50)) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 25 : (issuxxor && !ishaxor) ? 100 : 50)))
	    	    makemon(mkclass(S_EEL,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		/* More random monsters on other terrain, too. --Amy */

		if ((levl[x][y].typ == LAVAPOOL && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_FLYFISH,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == ROOM && !rn2( ((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000) / level_difficulty() )) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == CORR && !rn2( ((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000) / level_difficulty() )) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == ICE && !rn2( ((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000) / level_difficulty() )) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == AIR && !rn2( ((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000) / level_difficulty() )) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == CLOUD && !rn2( ((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000) / level_difficulty() )) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == CORR && !rn2((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000)) )
			makemon(mkclass(S_WALLMONST,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == CORR && !rn2((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000 )) )
			makemon(mkclass(S_TURRET,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == TREE && !rn2((ishaxor && !issuxxor) ? 200 : (issuxxor && !ishaxor) ? 800 : 400)) )
			makemon(mkclass(S_BAT,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_SNAKE,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_LEPRECHAUN,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_NYMPH,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_SPIDER,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_RUBMONST,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 10 : (issuxxor && !ishaxor) ? 40 : 20)) )
	    	    makemon(courtmon(), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_VORTEX,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_LIGHT,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_TRAPPER,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_ANGEL,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_ELEMENTAL,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_HUMAN,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makemon(mkclass(S_NEMESE,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 5000 : (issuxxor && !ishaxor) ? 20000 : 10000)) )
	    	    makemon(mkclass(S_ARCHFIEND,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_PUDDING,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_BLOB,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_JELLY,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_FUNGUS,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_GRUE,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_LIZARD,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_MIMIC,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_PIERCER,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_RODENT,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_WORM,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == FARMLAND && !rn2((ishaxor && !issuxxor) ? 400 : (issuxxor && !ishaxor) ? 1600 : 800)) )
	    	    makemon(mkclass(S_QUADRUPED,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if (levl[x][y].typ == WATERTUNNEL && In_swimmingpool(&u.uz) && (dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz)) && !rn2(15) )
		    mkobj_at(!rn2(100) ? IMPLANT_CLASS : !rn2(4) ? AMULET_CLASS : RING_CLASS, x, y, FALSE, FALSE);

		if ((levl[x][y].typ == MOUNTAIN && !rn2((ishaxor && !issuxxor) ? 250 : (issuxxor && !ishaxor) ? 1000 : 500)) )
	    	    makemon(specialtensmon(61), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* flying */
		if ((levl[x][y].typ == WATERTUNNEL && !rn2((ishaxor && !issuxxor) ? 12 : (issuxxor && !ishaxor) ? 50 : 25)) )
	    	    makemon(mkclass(S_EEL,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == CRYSTALWATER && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_EEL,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == MOORLAND && !rn2((ishaxor && !issuxxor) ? 250 : (issuxxor && !ishaxor) ? 1000 : 500)) )
	    	    makemon(specialtensmon(70), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* amphibious */
		if ((levl[x][y].typ == URINELAKE && !rn2((ishaxor && !issuxxor) ? 400 : (issuxxor && !ishaxor) ? 1600 : 800)) )
	    	    makemon(specialtensmon(88), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* acidic */
		if ((levl[x][y].typ == SHIFTINGSAND && !rn2((ishaxor && !issuxxor) ? 125 : (issuxxor && !ishaxor) ? 500 : 250)) )
	    	    makemon(specialtensmon(219), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* AD_WRAP */
		if ((levl[x][y].typ == STYXRIVER && !rn2((ishaxor && !issuxxor) ? 450 : (issuxxor && !ishaxor) ? 1800 : 900)) )
	    	    makemon(mkclass(S_FLYFISH,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == STYXRIVER && !rn2((ishaxor && !issuxxor) ? 450 : (issuxxor && !ishaxor) ? 1800 : 900)) )
	    	    makemon(specialtensmon(337), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* AD_CONT */
		if ((levl[x][y].typ == PENTAGRAM && !rn2((ishaxor && !issuxxor) ? 10 : (issuxxor && !ishaxor) ? 40 : 20)) )
	    	    makemon(specialtensmon(313), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* AD_CAST */
		if ((levl[x][y].typ == WELL && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_ZOMBIE,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == POISONEDWELL && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(specialtensmon(89), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* poisonous */
		if ((levl[x][y].typ == WAGON && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(specialtensmon(68), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* concealing */
		if ((levl[x][y].typ == BURNINGWAGON && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(specialtensmon(194), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* AD_FIRE */
		if ((levl[x][y].typ == WOODENTABLE && !rn2((ishaxor && !issuxxor) ? 25 : (issuxxor && !ishaxor) ? 100 : 50)) )
	    	    makemon(specialtensmon(82), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* thick hide */
		if ((levl[x][y].typ == CARVEDBED && !rn2((ishaxor && !issuxxor) ? 10 : (issuxxor && !ishaxor) ? 40 : 20)) )
	    	    makemon(specialtensmon(45), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* resist sleep */
		if ((levl[x][y].typ == STRAWMATTRESS && !rn2((ishaxor && !issuxxor) ? 100 : (issuxxor && !ishaxor) ? 400 : 200)) )
	    	    makemon(specialtensmon(205), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* AD_PLYS */
		if ((levl[x][y].typ == SNOW && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makemon(specialtensmon(44), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* resist cold */
		if ((levl[x][y].typ == ASH && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makemon(specialtensmon(43), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* resist fire */
		if ((levl[x][y].typ == SAND && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makemon(specialtensmon(90), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* carnivorous */
		if ((levl[x][y].typ == PAVEDFLOOR && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makemon(specialtensmon(332), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* MS_SHOE */
		if ((levl[x][y].typ == HIGHWAY && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makemon(speedymon(), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == GRASSLAND && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makemon(specialtensmon(172), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* AT_BITE */
		if ((levl[x][y].typ == NETHERMIST && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makemon(specialtensmon(310), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* AD_NTHR */
		if ((levl[x][y].typ == STALACTITE && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makemon(mkclass(S_PIERCER,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == CRYPTFLOOR && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makemon(morguemonX(), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == CRYPTFLOOR && !rn2(50)) )
			(void) maketrap(x, y, CURSED_GRAVE, 100, FALSE);
		if ((levl[x][y].typ == CRYPTFLOOR && !rn2(50)) )
			(void) maketrap(x, y, randomtrap(), 100, TRUE);
		if ((levl[x][y].typ == BUBBLES && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makemon(specialtensmon(203), x, y, MM_ADJACENTOK|MM_MAYSLEEP); /* AD_STUN */
		if ((levl[x][y].typ == RAINCLOUD && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makemon(mkclass(S_EEL,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == GRAVE && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makemon(morguemonX(), x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == ALTAR && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 10 : 5)) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == STONE && !rn2((ishaxor && !issuxxor) ? 5000 : (issuxxor && !ishaxor) ? 20000 : 10000)) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if (( (levl[x][y].typ == VWALL || levl[x][y].typ == ROCKWALL || levl[x][y].typ == GRAVEWALL || levl[x][y].typ == TUNNELWALL || levl[x][y].typ == HWALL || levl[x][y].typ == TLCORNER || levl[x][y].typ == TRCORNER || levl[x][y].typ == BLCORNER || levl[x][y].typ == BRCORNER || levl[x][y].typ == CROSSWALL || levl[x][y].typ == TUWALL || levl[x][y].typ == TDWALL || levl[x][y].typ == TRWALL || levl[x][y].typ == TLWALL || levl[x][y].typ == DBWALL ) && !rn2((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000)) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		if ((levl[x][y].typ == SDOOR && !rn2((ishaxor && !issuxxor) ? 100 : (issuxxor && !ishaxor) ? 400 : 200)) )
			makemon(mkclass(S_MIMIC,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == SCORR && !rn2((ishaxor && !issuxxor) ? 200 : (issuxxor && !ishaxor) ? 800 : 400)) )
			makemon(mkclass(S_MIMIC,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);
		if ((levl[x][y].typ == DOOR && !rn2((ishaxor && !issuxxor) ? 300 : (issuxxor && !ishaxor) ? 1200 : 600)) )
			makemon(mkclass(S_MIMIC,0), x, y, MM_ADJACENTOK|MM_MAYSLEEP);

		} /* !ishomicider check */

		if (ishomicider) {	/* idea by deepy - a race for which monsters don't spawn normally */

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 25 : (issuxxor && !ishaxor) ? 100 : 50)) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 25 : (issuxxor && !ishaxor) ? 100 : 50)))
	    	    makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == LAVAPOOL && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == ROOM && !rn2( ((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000) / level_difficulty() )) )
			makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == CORR && !rn2( ((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000) / level_difficulty() )) )
			makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == ICE && !rn2( ((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000) / level_difficulty() )) )
			makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == AIR && !rn2( ((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000) / level_difficulty() )) )
			makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == CLOUD && !rn2( ((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000) / level_difficulty() )) )
			makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == CORR && !rn2((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000)) )
			makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == CORR && !rn2((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000 )) )
			makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == TREE && !rn2((ishaxor && !issuxxor) ? 200 : (issuxxor && !ishaxor) ? 800 : 400)) )
			makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 10 : (issuxxor && !ishaxor) ? 40 : 20)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 5000 : (issuxxor && !ishaxor) ? 20000 : 10000)) )
	    	    makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == FARMLAND && !rn2((ishaxor && !issuxxor) ? 400 : (issuxxor && !ishaxor) ? 1600 : 800)) )
	    	    makerandomtrap_at(x, y, TRUE);

		if (levl[x][y].typ == WATERTUNNEL && In_swimmingpool(&u.uz) && (dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz)) && !rn2(15) )
		    mkobj_at(!rn2(100) ? IMPLANT_CLASS : !rn2(4) ? AMULET_CLASS : RING_CLASS, x, y, FALSE, FALSE);

		if ((levl[x][y].typ == MOUNTAIN && !rn2((ishaxor && !issuxxor) ? 250 : (issuxxor && !ishaxor) ? 1000 : 500)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == WATERTUNNEL && !rn2((ishaxor && !issuxxor) ? 12 : (issuxxor && !ishaxor) ? 50 : 25)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == CRYSTALWATER && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == MOORLAND && !rn2((ishaxor && !issuxxor) ? 250 : (issuxxor && !ishaxor) ? 1000 : 500)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == URINELAKE && !rn2((ishaxor && !issuxxor) ? 400 : (issuxxor && !ishaxor) ? 1600 : 800)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == SHIFTINGSAND && !rn2((ishaxor && !issuxxor) ? 125 : (issuxxor && !ishaxor) ? 500 : 250)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == STYXRIVER && !rn2((ishaxor && !issuxxor) ? 450 : (issuxxor && !ishaxor) ? 1800 : 900)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == STYXRIVER && !rn2((ishaxor && !issuxxor) ? 450 : (issuxxor && !ishaxor) ? 1800 : 900)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == PENTAGRAM && !rn2((ishaxor && !issuxxor) ? 10 : (issuxxor && !ishaxor) ? 40 : 20)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == WELL && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == POISONEDWELL && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == WAGON && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == BURNINGWAGON && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == WOODENTABLE && !rn2((ishaxor && !issuxxor) ? 25 : (issuxxor && !ishaxor) ? 100 : 50)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == CARVEDBED && !rn2((ishaxor && !issuxxor) ? 10 : (issuxxor && !ishaxor) ? 40 : 20)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == STRAWMATTRESS && !rn2((ishaxor && !issuxxor) ? 100 : (issuxxor && !ishaxor) ? 400 : 200)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == SNOW && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == ASH && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == SAND && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == PAVEDFLOOR && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == HIGHWAY && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == GRASSLAND && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == NETHERMIST && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == STALACTITE && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == CRYPTFLOOR && !rn2(50)) )
			(void) maketrap(x, y, CURSED_GRAVE, 100, FALSE);
		if ((levl[x][y].typ == CRYPTFLOOR && !rn2(50)) )
			(void) maketrap(x, y, randomtrap(), 100, TRUE);
		if ((levl[x][y].typ == CRYPTFLOOR && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == BUBBLES && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == RAINCLOUD && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == GRAVE && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == ALTAR && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 10 : 5)) )
			makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == STONE && !rn2((ishaxor && !issuxxor) ? 5000 : (issuxxor && !ishaxor) ? 20000 : 10000)) )
			makerandomtrap_at(x, y, TRUE);

		if (( (levl[x][y].typ == VWALL || levl[x][y].typ == ROCKWALL || levl[x][y].typ == GRAVEWALL || levl[x][y].typ == TUNNELWALL || levl[x][y].typ == HWALL || levl[x][y].typ == TLCORNER || levl[x][y].typ == TRCORNER || levl[x][y].typ == BLCORNER || levl[x][y].typ == BRCORNER || levl[x][y].typ == CROSSWALL || levl[x][y].typ == TUWALL || levl[x][y].typ == TDWALL || levl[x][y].typ == TRWALL || levl[x][y].typ == TLWALL || levl[x][y].typ == DBWALL ) && !rn2((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000)) )
			makerandomtrap_at(x, y, TRUE);

		if ((levl[x][y].typ == SDOOR && !rn2((ishaxor && !issuxxor) ? 100 : (issuxxor && !ishaxor) ? 400 : 200)) )
			makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == SCORR && !rn2((ishaxor && !issuxxor) ? 200 : (issuxxor && !ishaxor) ? 800 : 400)) )
			makerandomtrap_at(x, y, TRUE);
		if ((levl[x][y].typ == DOOR && !rn2((ishaxor && !issuxxor) ? 300 : (issuxxor && !ishaxor) ? 1200 : 600)) )
			makerandomtrap_at(x, y, TRUE);

		} /* ishomicider check */

		}

	/* determine if it is even allowed;
	   almost all special levels are excluded */
	if (issoviet && (In_hell(&u.uz) || In_V_tower(&u.uz) ||
#ifdef REINCARNATION
		Is_rogue_level(&u.uz) ||
#endif
		level.flags.arboreal ||
		((sp = Is_special(&u.uz)) != 0 && !Is_oracle_level(&u.uz)
					&& (!In_mines(&u.uz) || sp->flags.town)
	    ))) return;
	/* Amy edit - let's just ditch that. If players want to dig for items, let them do so! */
	/* In Soviet Russia, restrictions are par for the course. Therefore it's strictly forbidden to hide gold and gems
	 * in the walls of special levels. Violators of this rule are punished with at least 20 years in the
	 * detention camp. --Amy */

	/* basic level-related probabilities */
	goldprob = 20 + depth(&u.uz) / 3;
	goldamount = 20 + depth(&u.uz) / 3;
	gemprob = goldprob / 10;
	objprob = goldprob / 20;

	/* mines have ***MORE*** goodies - otherwise why mine? */
	if (In_mines(&u.uz)) {
	    goldprob *= 2;
	    gemprob *= 2;
	} else if (In_deepmines(&u.uz)) {
	    goldprob *= 3;
	    gemprob *= 3;
	} else if (In_quest(&u.uz)) {
	    goldprob /= 4;
	    gemprob /= 6;
	}

	if (ishaxor) {

	goldprob *= 2;
	gemprob *= 2;
	objprob *= 2;
	}

	/*
	 * Seed rock areas with gold and/or gems.
	 * We use fairly low level object handling to avoid unnecessary
	 * overhead from placing things in the floor chain prior to burial.
	 */
	for (x = 2; x < (COLNO - 2); x++)
	  for (y = 1; y < (ROWNO - 1); y++)
	    if (levl[x][y+1].typ != STONE && rn2(100) ) {	 /* <x,y> spot not eligible */
		y += 2;		/* next two spots aren't eligible either */
	    } else if (levl[x][y].typ != STONE && rn2(100) ) { /* this spot not eligible */
		y += 1;		/* next spot isn't eligible either */
	    } else if ( (!(levl[x][y].wall_info & W_NONDIGGABLE) || !rn2(1000) ) &&
		  ((levl[x][y-1].typ   == STONE &&
		  levl[x+1][y-1].typ == STONE && levl[x-1][y-1].typ == STONE &&
		  levl[x+1][y].typ   == STONE && levl[x-1][y].typ   == STONE &&
		  levl[x+1][y+1].typ == STONE && levl[x-1][y+1].typ == STONE) || !rn2(100) ) ) {
		if ((rn2(1000) < goldprob) && !rn2(10) && (depth(&u.uz) > rn2(100)) && timebasedlowerchance()) {
		    if ((otmp = mksobj(GOLD_PIECE, FALSE, FALSE, FALSE)) != 0) {
			otmp->ox = x,  otmp->oy = y;
			otmp->quan = 1L + rnd(goldamount * 3);
			otmp->owt = weight(otmp);
			if (!rn2((uarmc && itemhasappearance(uarmc, APP_CLOISTER_CLOAK)) ? 2 : 3)) add_to_buried(otmp);
			else place_object(otmp, x, y);
		    }
		}
		if ((rn2(1000) < gemprob) && !rn2(10) && (depth(&u.uz) > rn2(100)) && timebasedlowerchance()) {
		    for (cnt = rnd(2 + dunlev(&u.uz) / 3); cnt > 0; cnt--)
			if ((otmp = mkobj(GEM_CLASS, FALSE, FALSE)) != 0) {
			    if (otmp->otyp == ROCK) {
				dealloc_obj(otmp);	/* discard it */
			    } else {
				otmp->ox = x,  otmp->oy = y;
				if (!rn2((uarmc && itemhasappearance(uarmc, APP_CLOISTER_CLOAK)) ? 2 : 3)) add_to_buried(otmp);
				else place_object(otmp, x, y);
			    }
		    }
		}
		if ((rn2(1500) < objprob) && !rn2(10) && (depth(&u.uz) > rn2(100)) && timebasedlowerchance()) {
		    for (cnt = rnd(2 + dunlev(&u.uz) / 3); cnt > 0; cnt--)
			if ((otmp = mkobj(RANDOM_CLASS, FALSE, FALSE)) != 0) {
			    if (otmp->otyp == ROCK) {
				dealloc_obj(otmp);	/* discard it */
			    } else {
				otmp->ox = x,  otmp->oy = y;
				if (!rn2((uarmc && itemhasappearance(uarmc, APP_CLOISTER_CLOAK)) ? 2 : 3)) add_to_buried(otmp);
				else place_object(otmp, x, y);
			    }
		    }
		}
	    }
}

void
mklev()
{
	struct mkroom *croom;

	init_mapseen(&u.uz);

	if(getbones()) {

		u.gottenbones = 1;
		return;
	}

	in_mklev = TRUE;
	makelevel();

	bound_digging();
	specdungeoninit();
	mineralize();
	in_mklev = FALSE;
	/* has_morgue gets cleared once morgue is entered; graveyard stays
	   set (graveyard might already be set even when has_morgue is clear
	   [see fixup_special()], so don't update it unconditionally) */
	if (level.flags.has_morgue)
	    level.flags.graveyard = 1;
	if(!level.flags.is_maze_lev) {
	    for (croom = &rooms[0]; croom != &rooms[nroom]; croom++)
#ifdef SPECIALIZATION
		topologize(croom, FALSE);
#else
		topologize(croom);
#endif
	}
	set_wall_state();
}

void
#ifdef SPECIALIZATION
topologize(croom, do_ordinary)
register struct mkroom *croom;
boolean do_ordinary;
#else
topologize(croom)
register struct mkroom *croom;
#endif
{
	register int x, y, roomno = (croom - rooms) + ROOMOFFSET;
	register int lowx = croom->lx, lowy = croom->ly;
	register int hix = croom->hx, hiy = croom->hy;
#ifdef SPECIALIZATION
	register schar rtype = croom->rtype;
#endif
	register int subindex, nsubrooms = croom->nsubrooms;

	/* skip the room if already done; i.e. a shop handled out of order */
	/* also skip if this is non-rectangular (it _must_ be done already) */
	if ((int) levl[lowx][lowy].roomno == roomno || croom->irregular)
	    return;
#ifdef SPECIALIZATION
# ifdef REINCARNATION
	if (Is_rogue_level(&u.uz))
	    do_ordinary = TRUE;		/* vision routine helper */
# endif
	if ((rtype != OROOM) || do_ordinary)
#endif
	{
	    /* do innards first */
	    for(x = lowx; x <= hix; x++)
		for(y = lowy; y <= hiy; y++)
#ifdef SPECIALIZATION
		    if (rtype == OROOM)
			levl[x][y].roomno = NO_ROOM;
		    else
#endif
			levl[x][y].roomno = roomno;
	    /* top and bottom edges */
	    for(x = lowx-1; x <= hix+1; x++)
		for(y = lowy-1; y <= hiy+1; y += (hiy-lowy+2)) {
		    levl[x][y].edge = 1;
		    if (levl[x][y].roomno)
			levl[x][y].roomno = SHARED;
		    else
			levl[x][y].roomno = roomno;
		}
	    /* sides */
	    for(x = lowx-1; x <= hix+1; x += (hix-lowx+2))
		for(y = lowy; y <= hiy; y++) {
		    levl[x][y].edge = 1;
		    if (levl[x][y].roomno)
			levl[x][y].roomno = SHARED;
		    else
			levl[x][y].roomno = roomno;
		}
	}
	/* subrooms */
	for (subindex = 0; subindex < nsubrooms; subindex++)
#ifdef SPECIALIZATION
		topologize(croom->sbrooms[subindex], (rtype != OROOM));
#else
		topologize(croom->sbrooms[subindex]);
#endif
}

/* Find an unused room for a branch location. */
STATIC_OVL struct mkroom *
find_branch_room(mp)
    coord *mp;
{
    struct mkroom *croom = 0;

    if (nroom == 0) {
	mazexy(mp);		/* already verifies location */
    } else {
	/* not perfect - there may be only one stairway */
	if(nroom > 2) {
	    int tryct = 0;

	    do
		croom = &rooms[rn2(nroom)];
	    while((croom == dnstairs_room || croom == upstairs_room ||
		  croom->rtype != OROOM) && (++tryct < 100));

	    if (at_dgn_entrance("Green Cross")) {
		    do
			croom = &rooms[rn2(nroom)];
		    while((croom->rtype != GREENCROSSROOM) && (++tryct < 100));
	    }

	} else
	    croom = &rooms[rn2(nroom)];

	register int roomattempts = 0;

	do {
	    roomattempts++;
	    if (!somexy(croom, mp))
		impossible("Can't place branch!");
		return croom;
	} while( ((roomattempts > 50000) ? reallyoccupied(mp->x, mp->y) : occupied(mp->x, mp->y)) ||
	    (levl[mp->x][mp->y].typ != CORR && levl[mp->x][mp->y].typ != ROOM));
    }
    return croom;
}

/* Find the room for (x,y).  Return null if not in a room. */
STATIC_OVL struct mkroom *
pos_to_room(x, y)
    xchar x, y;
{
    int i;
    struct mkroom *curr;

    for (curr = rooms, i = 0; i < nroom; curr++, i++)
	if (inside_room(curr, x, y)) return curr;;
    return (struct mkroom *) 0;
}


/* If given a branch, randomly place a special stair or portal. */
void
place_branch(br, x, y)
branch *br;	/* branch to place */
xchar x, y;	/* location */
{
	coord	      m;
	d_level	      *dest;
	boolean	      make_stairs;
	struct mkroom *br_room;

	int stairtryct = 0;

	/*
	 * Return immediately if there is no branch to make or we have
	 * already made one.  This routine can be called twice when
	 * a special level is loaded that specifies an SSTAIR location
	 * as a favored spot for a branch.
	 */

	if (!br || made_branch) return;

	if (!x) {	/* find random coordinates for branch */
	    br_room = (find_branch_room(&m));
	    x = m.x;
	    y = m.y;

	    /* somehow this genius code can place the branch staircase on an existing branch!!! we can't have that --Amy */
	    if (On_stairs(x, y) && stairtryct++ < 50000) {
			impossible("placing branch on existing staircase?");
			br_room = (find_branch_room(&m));
			x = m.x;
			y = m.y;
	    }
	    while (On_stairs(x, y) && stairtryct++ < 50000) {
			br_room = (find_branch_room(&m));
			x = m.x;
			y = m.y;
	    }

		/* stairseeker changes stuff here */
		if (br && (evilfriday || isstairseeker || !(at_dgn_entrance("Green Cross") || at_dgn_entrance("The Subquest") || at_dgn_entrance("The Quest") || at_dgn_entrance("Lawful Quest") || at_dgn_entrance("Neutral Quest") || at_dgn_entrance("Chaotic Quest") || at_dgn_entrance("The Elemental Planes") || at_dgn_entrance("Sheol") || at_dgn_entrance("Bell Caves") || at_dgn_entrance("Vlad's Tower") || at_dgn_entrance("Forging Chamber") || at_dgn_entrance("Dead Grounds") || at_dgn_entrance("Ordered Chaos")) )) {

			int steetries = 0;
			while (steetries < 50000) {
				steetries++;
				x = rnd(COLNO-1);
				y = rn2(ROWNO);
				if (ACCESSIBLE(levl[x][y].typ) && !(t_at(x,y) && ((t_at(x,y))->ttyp == MAGIC_PORTAL)) && (levl[x][y].typ == CORR || levl[x][y].typ == ROOM) && !On_stairs(x, y)) break; /* we got a good location! */
			}
			if (!ACCESSIBLE(levl[x][y].typ) || (t_at(x,y) && ((t_at(x,y))->ttyp == MAGIC_PORTAL)) || On_stairs(x, y)) {
				x = m.x;
				y = m.y;
			}
		}

	} else {

	    br_room = pos_to_room(x, y);
	}

	if (on_level(&br->end1, &u.uz)) {
	    /* we're on end1 */
	    make_stairs = br->type != BR_NO_END1;
	    dest = &br->end2;
	} else {
	    /* we're on end2 */
	    make_stairs = br->type != BR_NO_END2;
	    dest = &br->end1;
	}

	if (br->type == BR_PORTAL) {
	    mkportal(x, y, dest->dnum, dest->dlevel);
		/* Amy edit: make sure it's accessible */
		if (IS_ROCK(levl[x][y].typ)) levl[x][y].typ = ROOM;
	} else if (make_stairs) {
	    sstairs.sx = x;
	    sstairs.sy = y;
	    sstairs.up = (char) on_level(&br->end1, &u.uz) ?
					    br->end1_up : !br->end1_up;
	    assign_level(&sstairs.tolev, dest);
	    sstairs_room = br_room;

	    levl[x][y].ladder = sstairs.up ? LA_UP : LA_DOWN;
	    levl[x][y].typ = STAIRS;
	}
	/*
	 * Set made_branch to TRUE even if we didn't make a stairwell (i.e.
	 * make_stairs is false) since there is currently only one branch
	 * per level, if we failed once, we're going to fail again on the
	 * next call.
	 */
	made_branch = TRUE;

	/* now that the branch exists, it can no longer happen that the stair/portal appears in an impossible place... */

}

STATIC_OVL boolean
bydoor(x, y)
register xchar x, y;
{
	register int typ;

	if (isok(x+1, y)) {
		typ = levl[x+1][y].typ;
		if (IS_DOOR(typ) || typ == SDOOR) return TRUE;
	}
	if (isok(x-1, y)) {
		typ = levl[x-1][y].typ;
		if (IS_DOOR(typ) || typ == SDOOR) return TRUE;
	}
	if (isok(x, y+1)) {
		typ = levl[x][y+1].typ;
		if (IS_DOOR(typ) || typ == SDOOR) return TRUE;
	}
	if (isok(x, y-1)) {
		typ = levl[x][y-1].typ;
		if (IS_DOOR(typ) || typ == SDOOR) return TRUE;
	}
	return FALSE;
}

/* see whether it is allowable to create a door at [x,y] */
int
okdoor(x,y)
register xchar x, y;
{
	register boolean near_door = bydoor(x, y);

	return((levl[x][y].typ == HWALL || levl[x][y].typ == VWALL) &&
			doorindex < DOORMAX && !near_door);
}

void
dodoor(x,y,aroom)
register int x, y;
register struct mkroom *aroom;
{
	if(doorindex >= DOORMAX) {
		impossible("DOORMAX exceeded?");
		return;
	}

	dosdoor(x,y,aroom,rn2(8) ? DOOR : SDOOR);
}

boolean
occupied(x, y)
register xchar x, y;
{
	return((boolean)(t_at(x, y)
		|| IS_FURNITURE(levl[x][y].typ)
		|| is_lava(x,y)
		|| is_waterypool(x,y)
		|| is_watertunnel(x,y)
		|| is_shiftingsand(x,y)
		|| is_moorland(x,y)
		|| is_urinelake(x,y)
		|| invocation_pos(x,y)
		));
}

/* K2 found out that the game can enter an infinite loop occasionally; I've seen that happen in slex before, especially
 * when a pool room was on one of my special mazes. Apparently, if the game tries to put a branch stair or something in
 * one of those rooms and there's only water everywhere, it searches and searches for a position where it could put the
 * staircase and never finds one, so I had to make sure it can exit that loop eventually. --Amy */
boolean
reallyoccupied(x, y)
register xchar x, y;
{
	return((boolean) (levl[x][y].typ == STAIRS) || (levl[x][y].typ == LADDER) || invocation_pos(x,y));
}

/* make a trap somewhere (in croom if mazeflag = 0 && !tm) */
/* if tm != null, make trap at that location */
void
mktrap(num, mazeflag, croom, tm, cangivehp)
register int num, mazeflag;
register struct mkroom *croom;
coord *tm;
boolean cangivehp;
{
	register int kind;
	coord m;
	boolean isspecific;

	/* no traps in pools */
	if (tm && is_waterypool(tm->x,tm->y)) return;

	if (num < -1) {
		num += 256; /* sucky workaround --Amy */
		if (num == MAGIC_PORTAL) num = 0;
	}

	if (num > 0 && num < TRAPNUM) {
	    kind = num;
	    isspecific = 1;
#ifdef REINCARNATION
	} else if (Is_rogue_level(&u.uz) && rn2(2) ) {
	    switch (rn2(7)) {
		default: kind = BEAR_TRAP; break; /* 0 */
		case 1: kind = ARROW_TRAP; break;
		case 2: kind = DART_TRAP; break;
		case 3: kind = TRAPDOOR; break;
		case 4: kind = PIT; break;
		case 5: kind = SLP_GAS_TRAP; break;
		case 6: kind = RUST_TRAP; break;
	    }
#endif
	} else if (( (Inhell && !Race_if(PM_HERETIC) ) || flags.gehenna) && !rn2(25)) {
	    /* bias the frequency of fire traps in Gehennom, but generate them less often
	     * because all the other traps should also have a chance of spawning --Amy */
	    kind = FIRE_TRAP;
	} else if (Role_if(PM_TRANSSYLVANIAN) && !rn2(20) ) {
	    kind = HEEL_TRAP;
	} else if (Role_if(PM_BUTT_LOVER) && !rn2(100) ) {
	    kind = FART_TRAP;
	} else if (Role_if(PM_BUTT_LOVER) && !rn2(500) ) {
	    kind = FARTING_WEB;
	} else if ((uarmf && itemhasappearance(uarmf, APP_BUFFALO_BOOTS)) && !rn2(100) ) {
	    kind = SHIT_TRAP;
	} else {
	    unsigned lvl = level_difficulty();

	    do {
selecttrap:
		kind = rnd(TRAPNUM-1);

		/* The trap distribution was, to put it blunt, bullshit. Make it more sane again. --Amy */
		if (!Race_if(PM_DEVELOPER) && !u.arabellahack && !rn2(issoviet ? 2 : 3)) kind = rnd(rn2(3) ? ANTI_MAGIC : POLY_TRAP);

		/* reject "too hard" traps */
		switch (kind) {
		    case MAGIC_PORTAL:
		    case S_PRESSING_TRAP:
			goto selecttrap;
			break;
		    case NUPESELL_TRAP:
			if (In_sokoban(&u.uz) && rn2(100)) goto selecttrap;
			break;
		    case TRAPDOOR:
		    case SHAFT_TRAP:
		    case PIT:
		    case SPIKED_PIT:
		    case GIANT_CHASM:
		    case SHIT_PIT:
		    case MANA_PIT:
		    case ACID_PIT:
			if (In_sokoban(&u.uz) && rn2(10)) goto selecttrap;
			break;
		    case LEVEL_TELEP:
		    case LEVEL_BEAMER:
		    case NEXUS_TRAP:
			if (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) ) goto selecttrap;
			break;
		    case TELEP_TRAP:
			if (level.flags.noteleport || Race_if(PM_STABILISATOR)) goto selecttrap;
			break;
		    case BEAMER_TRAP:
			if (level.flags.noteleport || Race_if(PM_STABILISATOR)) goto selecttrap;
			break;
		    case HOLE:
			/* make these much less often than other traps */
			if (rn2(7)) goto selecttrap;
			if (In_sokoban(&u.uz) && rn2(10)) goto selecttrap;
			break;
		    case MENU_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8)) goto selecttrap;
			break;
		    case SPEED_TRAP: 
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40)) goto selecttrap;
			break;
		    case AUTOMATIC_SWITCHER:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && rn2(ishaxor ? (Role_if(PM_GRADUATE) ? 125 : Role_if(PM_SPACEWARS_FIGHTER) ? 250 : Role_if(PM_GEEK) ? 250 : 500) : (Role_if(PM_GRADUATE) ? 250 : Role_if(PM_SPACEWARS_FIGHTER) ? 500 : Role_if(PM_GEEK) ? 500 : 1000) )) goto selecttrap;
			break;

		    case RMB_LOSS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && !Role_if(PM_SPACEWARS_FIGHTER) && rn2(2)) goto selecttrap;
			break;
		    case DISPLAY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3)) goto selecttrap;
			break;
		    case SPELL_LOSS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4)) goto selecttrap;
			break;
		    case YELLOW_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 5)) goto selecttrap;
			break;
		    case AUTO_DESTRUCT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10)) goto selecttrap;
			break;
		    case MEMORY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20)) goto selecttrap;
			break;
		    case INVENTORY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50)) goto selecttrap;
			break;
		    case BLACK_NG_WALL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100)) goto selecttrap;
			break;
		    case SUPERSCROLLER_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 100 : 200)) goto selecttrap;
			break;
		    case ACTIVE_SUPERSCROLLER_TRAP: goto selecttrap;
			break;

		    case ITEM_TELEP_TRAP:
			if (rn2(evilfriday ? 3 : 15) && !NastyTrapNation) goto selecttrap;
			break;
		    case LAVA_TRAP:
			if (rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap;
			break;
		    case FLOOD_TRAP:
			if (rn2(evilfriday ? 2 : 20) && !NastyTrapNation) goto selecttrap;
			break;

		    case PERSISTENT_FART_TRAP:
			if (rn2(evilfriday ? 5 : 16) && !NastyTrapNation) goto selecttrap;
			break;
		    case ATTACKING_HEEL_TRAP:
			if (rn2(evilfriday ? 5 : 16) && !NastyTrapNation) goto selecttrap;
			break;
		    case ALIGNMENT_TRASH_TRAP:
			if (rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap;
			break;
		    case DOGSIDE_TRAP:
			if (rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap;
			break;
		    case BANKRUPT_TRAP:
			if (rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap;
			break;
		    case DYNAMITE_TRAP:
			if (rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap;
			break;
		    case MALEVOLENCE_TRAP:
			if (rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap;
			break;
		    case LEAFLET_TRAP:
			if (rn2(evilfriday ? 2 : 20) && !NastyTrapNation) goto selecttrap;
			break;
		    case TENTADEEP_TRAP:
			if (rn2(evilfriday ? 3 : 15) && !NastyTrapNation) goto selecttrap;
			break;
		    case STATHALF_TRAP:
			if (rn2(evilfriday ? 8 : 50) && !NastyTrapNation) goto selecttrap;
			break;
		    case RARE_SPAWN_TRAP:
			if (rn2(evilfriday ? 3 : 15) && !NastyTrapNation) goto selecttrap;
			break;
		    case YOU_ARE_AN_IDIOT_TRAP:
			if (rn2(evilfriday ? 50 : 200) && !NastyTrapNation) goto selecttrap;
			break;
		    case NASTYCURSE_TRAP:
			if (rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap;
			break;

		    case DRAIN_TRAP:
			if ((rn2(3) && !evilfriday) && !NastyTrapNation) goto selecttrap;
			break;
		    case TIME_TRAP:
			if (rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap;
			break;
		    case FREE_HAND_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap;
			break;
		    case DISINTEGRATION_TRAP:
			if (rn2(evilfriday ? 5 : 20) && !NastyTrapNation) goto selecttrap;
			break;
		    case UNIDENTIFY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;
		    case THIRST_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap;
			break;
		    case SHADES_OF_GREY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;
		    case LUCK_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) goto selecttrap;
			break;
		    case DEATH_TRAP:
			if (rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap;
			break;
		    case STONE_TO_FLESH_TRAP:
			if (rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap;
			break;
		    case FAINT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) goto selecttrap;
			break;
		    case CURSE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 32 : 64 )) goto selecttrap;
			break;
		    case DIFFICULTY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 12 : 24 )) goto selecttrap;
			break;
		    case SOUND_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 14 )) goto selecttrap;
			break;
		    case CASTER_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap;
			break;
		    case WEAKNESS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) goto selecttrap;
			break;
		    case ROT_THIRTEEN_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap;
			break;
		    case BISHOP_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 28 : 56 )) goto selecttrap;
			break;
		    case CONFUSION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 40 : 80 )) goto selecttrap;
			break;
		    case DROP_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 35 : 70 )) goto selecttrap;
			break;
		    case DSTW_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap;
			break;
		    case STATUS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 9 )) goto selecttrap;
			break;
		    case ALIGNMENT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 7 )) goto selecttrap;
			break;
		    case STAIRS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 11 )) goto selecttrap;
			break;
		    case UNINFORMATION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3 )) goto selecttrap;
			break;
		    case TIMERUN_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap;
			break;
		    case REPEATING_NASTYCURSE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100 )) goto selecttrap;
			break;
		    case REALLY_BAD_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;
		    case COVID_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap;
			break;
		    case ARTIBLAST_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 9 )) goto selecttrap;
			break;
		    case GIANT_EXPLORER_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 40 : 80 )) goto selecttrap;
			break;
		    case TRAPWARP_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;
		    case YAWM_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 15 )) goto selecttrap;
			break;
		    case CRADLE_OF_CHAOS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;
		    case TEZCATLIPOCA_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap;
			break;
		    case ENTHUMESIS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100 )) goto selecttrap;
			break;
		    case MIKRAANESIS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 100 : 200 )) goto selecttrap;
			break;
		    case GOTS_TOO_GOOD_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap;
			break;
		    case KILLER_ROOM_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;
		    case NO_FUN_WALLS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 17 : 35 )) goto selecttrap;
			break;
		    case SANITY_TREBLE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 51 )) goto selecttrap;
			break;
		    case STAT_DECREASE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;
		    case SIMEOUT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100 )) goto selecttrap;
			break;
		    case BAD_PART_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;
		    case COMPLETELY_BAD_PART_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 26 : 52 )) goto selecttrap;
			break;
		    case EVIL_VARIANT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 500 : 1000 )) goto selecttrap;
			break;

		    case INTRINSIC_LOSS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap;
			break;
		    case BLOOD_LOSS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 18 )) goto selecttrap;
			break;
		    case BAD_EFFECT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3 )) goto selecttrap;
			break;
		    case MULTIPLY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 17 )) goto selecttrap;
			break;
		    case AUTO_VULN_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap;
			break;
		    case TELE_ITEMS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;
		    case NASTINESS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 250 : 500 )) goto selecttrap;
			break;
		    case FARLOOK_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 17 : 34 )) goto selecttrap;
			break;
		    case RESPAWN_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;
		    case CAPTCHA_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 5 )) goto selecttrap;
			break;

		    case RECURRING_AMNESIA_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 35 : 70 )) goto selecttrap;
			break;
		    case BIGSCRIPT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3 )) goto selecttrap;
			break;
		    case BANK_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 13 : 26 )) goto selecttrap;
			break;
		    case ONLY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) goto selecttrap;
			break;
		    case MAP_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;
		    case TECH_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;
		    case DISENCHANT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) goto selecttrap;
			break;
		    case VERISIERT:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap;
			break;
		    case CHAOS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 17 : 33 )) goto selecttrap;
			break;
		    case MUTENESS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) goto selecttrap;
			break;
		    case NTLL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap;
			break;
		    case ENGRAVING_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 17 )) goto selecttrap;
			break;
		    case MAGIC_DEVICE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) goto selecttrap;
			break;
		    case BOOK_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 19 )) goto selecttrap;
			break;
		    case LEVEL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 15 )) goto selecttrap;
			break;
		    case QUIZ_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 11 )) goto selecttrap;
			break;

		    case METABOLIC_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;
		    case TRAP_OF_NO_RETURN:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap;
			break;
		    case EGOTRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 15 )) goto selecttrap;
			break;
		    case FAST_FORWARD_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;
		    case TRAP_OF_ROTTENNESS:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 17 )) goto selecttrap;
			break;
		    case UNSKILLED_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap;
			break;
		    case LOW_STATS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 13 )) goto selecttrap;
			break;
		    case TRAINING_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) goto selecttrap;
			break;
		    case EXERCISE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4 )) goto selecttrap;
			break;
		    case LIMITATION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;
		    case WEAK_SIGHT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 16 )) goto selecttrap;
			break;
		    case RANDOM_MESSAGE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 16 )) goto selecttrap;
			break;

		    case DESECRATION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;
		    case STARVATION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap;
			break;
		    case DROPLESS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap;
			break;
		    case LOW_EFFECT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 14 )) goto selecttrap;
			break;
		    case INVISIBLE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 45 : 90 )) goto selecttrap;
			break;
		    case GHOST_WORLD_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap;
			break;
		    case DEHYDRATION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 15 )) goto selecttrap;
			break;
		    case HATE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap;
			break;
		    case TOTTER_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 150 : 300 )) goto selecttrap;
			break;
		    case NONINTRINSICAL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap;
			break;
		    case DROPCURSE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) goto selecttrap;
			break;
		    case NAKEDNESS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap;
			break;
		    case ANTILEVEL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 75 : 150 )) goto selecttrap;
			break;
		    case STEALER_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4 )) goto selecttrap;
			break;
		    case REBELLION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 5 )) goto selecttrap;
			break;
		    case CRAP_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 13 )) goto selecttrap;
			break;
		    case MISFIRE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 13 )) goto selecttrap;
			break;
		    case TRAP_OF_WALLS:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) goto selecttrap;
			break;

		    case DISCONNECT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;
		    case INTERFACE_SCREW_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 80 : 160 )) goto selecttrap;
			break;
		    case BOSSFIGHT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap;
			break;
		    case ENTIRE_LEVEL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 17 )) goto selecttrap;
		    case BONES_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4 )) goto selecttrap;
			break;
		    case AUTOCURSE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 49 )) goto selecttrap;
			break;
		    case HIGHLEVEL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 13 )) goto selecttrap;
			break;
		    case SPELL_FORGETTING_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 21 : 42 )) goto selecttrap;
			break;
		    case SOUND_EFFECT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 7 )) goto selecttrap;
			break;

		    case LOOTCUT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 18 )) goto selecttrap;
			break;
		    case MONSTER_SPEED_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;
		    case SCALING_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap;
			break;
		    case ENMITY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 15 )) goto selecttrap;
			break;
		    case WHITE_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap;
			break;
		    case COMPLETE_GRAY_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap;
			break;
		    case QUASAR_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 100 : 200 )) goto selecttrap;
			break;
		    case MOMMA_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap;
			break;
		    case HORROR_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 14 )) goto selecttrap;
			break;
		    case ARTIFICER_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 40 : 80 )) goto selecttrap;
			break;
		    case WEREFORM_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap;
			break;
		    case NON_PRAYER_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;
		    case EVIL_PATCH_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 500 : 1000 )) goto selecttrap;
			break;
		    case HARD_MODE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 45 : 90 )) goto selecttrap;
			break;
		    case SECRET_ATTACK_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap;
			break;
		    case EATER_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;
		    case COVETOUSNESS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 7 )) goto selecttrap;
			break;
		    case NOT_SEEN_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 7 )) goto selecttrap;
			break;
		    case DARK_MODE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 9 )) goto selecttrap;
			break;
		    case ANTISEARCH_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap;
			break;
		    case HOMICIDE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) goto selecttrap;
			break;
		    case NASTY_NATION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 70 : 140 )) goto selecttrap;
			break;
		    case WAKEUP_CALL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;
		    case GRAYOUT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 17 : 34 )) goto selecttrap;
			break;
		    case GRAY_CENTER_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 18 : 37 )) goto selecttrap;
			break;
		    case CHECKERBOARD_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;
		    case CLOCKWISE_SPIN_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 350 : 700 )) goto selecttrap;
			break;
		    case COUNTERCLOCKWISE_SPIN_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 250 : 500 )) goto selecttrap;
			break;
		    case LAG_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 90 : 180 )) goto selecttrap;
			break;
		    case BLESSCURSE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 11 : 22 )) goto selecttrap;
			break;
		    case DE_LIGHT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) goto selecttrap;
			break;
		    case DISCHARGE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 5 )) goto selecttrap;
			break;
		    case TRASHING_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) goto selecttrap;
			break;
		    case FILTERING_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap;
			break;
		    case DEFORMATTING_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) goto selecttrap;
			break;
		    case FLICKER_STRIP_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4 )) goto selecttrap;
			break;
		    case UNDRESSING_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 17 : 34 )) goto selecttrap;
			break;
		    case HYPERBLUEWALL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 18 : 36 )) goto selecttrap;
			break;
		    case NOLITE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap;
			break;
		    case PARANOIA_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap;
			break;
		    case FLEECESCRIPT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 7 )) goto selecttrap;
			break;
		    case INTERRUPT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 12 : 25 )) goto selecttrap;
			break;
		    case DUSTBIN_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 13 : 26 )) goto selecttrap;
			break;
		    case MANA_BATTERY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 19 )) goto selecttrap;
			break;
		    case MONSTERFINGERS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 19 )) goto selecttrap;
			break;
		    case MISCAST_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 12 : 23 )) goto selecttrap;
			break;
		    case MESSAGE_SUPPRESSION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 12 : 25 )) goto selecttrap;
			break;
		    case STUCK_ANNOUNCEMENT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 15 )) goto selecttrap;
			break;
		    case BLOODTHIRSTY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 18 : 35 )) goto selecttrap;
			break;
		    case MAXIMUM_DAMAGE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 16 : 32 )) goto selecttrap;
			break;
		    case LATENCY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) goto selecttrap;
			break;
		    case STARLIT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 35 : 70 )) goto selecttrap;
			break;
		    case KNOWLEDGE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap;
			break;
		    case HIGHSCORE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 40 : 80 )) goto selecttrap;
			break;
		    case PINK_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) goto selecttrap;
			break;
		    case GREEN_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 13 : 26 )) goto selecttrap;
			break;
		    case EVC_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap;
			break;
		    case UNDERLAYER_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 39 )) goto selecttrap;
			break;
		    case DAMAGE_METER_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 9 )) goto selecttrap;
			break;
		    case ARBITRARY_WEIGHT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap;
			break;
		    case FUCKED_INFO_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap;
			break;
		    case BLACK_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 16 : 32 )) goto selecttrap;
			break;
		    case CYAN_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;
		    case HEAP_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 11 : 22 )) goto selecttrap;
			break;
		    case BLUE_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 17 )) goto selecttrap;
			break;
		    case TRON_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 26 : 53 )) goto selecttrap;
			break;
		    case RED_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap;
			break;
		    case TOO_HEAVY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) goto selecttrap;
			break;
		    case ELONGATION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 51 )) goto selecttrap;
			break;
		    case WRAPOVER_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 36 : 72 )) goto selecttrap;
			break;
		    case DESTRUCTION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 16 : 32 )) goto selecttrap;
			break;
		    case MELEE_PREFIX_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 40 : 80 )) goto selecttrap;
			break;
		    case AUTOMORE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;
		    case UNFAIR_ATTACK_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 35 : 70 )) goto selecttrap;
			break;

		    case SPACEWARS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && !Role_if(PM_SPACEWARS_FIGHTER) && rn2(15) ) goto selecttrap;
			break;

		    case ORANGE_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 16 : 31 )) goto selecttrap;
			break;

		    case VIOLET_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap;
			break;

		    case TRAP_OF_LONGING:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;

		    case CURSED_PART_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 11 )) goto selecttrap;
			break;

		    case QUAVERSAL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 100 : 200 )) goto selecttrap;
			break;

		    case APPEARANCE_SHUFFLING_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap;
			break;

		    case BROWN_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 11 )) goto selecttrap;
			break;

		    case CHOICELESS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 16 : 32 )) goto selecttrap;
			break;

		    case GOLDSPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;

		    case DEPROVEMENT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 45 : 90 )) goto selecttrap;
			break;

		    case INITIALIZATION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap;
			break;

		    case GUSHLUSH_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 12 : 24 )) goto selecttrap;
			break;

		    case SOILTYPE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 7 )) goto selecttrap;
			break;

		    case DANGEROUS_TERRAIN_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 13 : 26 )) goto selecttrap;
			break;

		    case FALLOUT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) goto selecttrap;
			break;

		    case MOJIBAKE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) goto selecttrap;
			break;

		    case GRAVATION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 16 : 32 )) goto selecttrap;
			break;

		    case UNCALLED_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 17 )) goto selecttrap;
			break;

		    case EXPLODING_DICE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap;
			break;

		    case PERMACURSE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 26 : 52 )) goto selecttrap;
			break;

		    case SHROUDED_IDENTITY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap;
			break;

		    case FEELER_GAUGES_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap;
			break;

		    case LONG_SCREWUP_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) goto selecttrap;
			break;

		    case WING_YELLOW_CHANGER:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 33 : 66 )) goto selecttrap;
			break;

		    case LIFE_SAVING_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 27 : 54 )) goto selecttrap;
			break;

		    case CURSEUSE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 27 )) goto selecttrap;
			break;

		    case CUT_NUTRITION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;

		    case SKILL_LOSS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 32 : 64 )) goto selecttrap;
			break;

		    case AUTOPILOT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 100 : 200 )) goto selecttrap;
			break;

		    case FORCE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) goto selecttrap;
			break;

		    case MONSTER_GLYPH_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 13 : 26 )) goto selecttrap;
			break;

		    case CHANGING_DIRECTIVE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;

		    case CONTAINER_KABOOM_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 16 : 32 )) goto selecttrap;
			break;

		    case STEAL_DEGRADE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 19 : 38 )) goto selecttrap;
			break;

		    case LEFT_INVENTORY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;

		    case FLUCTUATING_SPEED_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100 )) goto selecttrap;
			break;

		    case TARMUSTROKINGNORA_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;

		    case FAILURE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;

		    case BRIGHT_CYAN_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap;
			break;

		    case FREQUENTATION_SPAWN_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 14 )) goto selecttrap;
			break;

		    case PET_AI_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap;
			break;

		    case SATAN_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap;
			break;

		    case REMEMBERANCE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 19 )) goto selecttrap;
			break;

		    case POKELIE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4 )) goto selecttrap;
			break;

		    case AUTOPICKUP_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 9 )) goto selecttrap;
			break;

		    case DYWYPI_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 11 : 22 )) goto selecttrap;
			break;

		    case SILVER_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) goto selecttrap;
			break;

		    case METAL_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 27 )) goto selecttrap;
			break;

		    case PLATINUM_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 29 )) goto selecttrap;
			break;

		    case MANLER_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 36 : 72 )) goto selecttrap;
			break;

		    case DOORNING_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) goto selecttrap;
			break;

		    case NOWNSIBLE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 45 : 90 )) goto selecttrap;
			break;

		    case ELM_STREET_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 13 : 26 )) goto selecttrap;
			break;

		    case MONNOISE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap;
			break;

		    case RANG_CALL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 18 )) goto selecttrap;
			break;

		    case RECURRING_SPELL_LOSS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;

		    case ANTITRAINING_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;

		    case TECHOUT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;

		    case STAT_DECAY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100 )) goto selecttrap;
			break;

		    case MOVEMORK_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) goto selecttrap;
			break;

		    case FALLING_LOADSTONE_TRAP:
			if ((rn2(3) && !evilfriday) && !NastyTrapNation) goto selecttrap;
			break;
		    case FALLING_NASTYSTONE_TRAP:
			if (rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap;
			break;
		    case EVIL_ARTIFACT_TRAP:
			if (rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap;
			break;
		    case RODNEY_TRAP:
			if (rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap;
			break;

		    case HYBRID_TRAP:
			if (rn2(500) && !NastyTrapNation) goto selecttrap;
			break;
		    case SHAPECHANGE_TRAP:
			if (rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap;
			break;
		    case CATACLYSM_TRAP:
			if (rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap;
			break;
		    case KILL_SYMBIOTE_TRAP:
			if (rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap;
			break;
		    case FEMINISM_STONE_TRAP:
			if (rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap;
			break;
		    case BRANCH_TELEPORTER:
			if (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) ) goto selecttrap;
			if (rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap;
			break;
		    case BRANCH_BEAMER:
			if (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) ) goto selecttrap;
			if (rn2(evilfriday ? 2 : 12) && !NastyTrapNation) goto selecttrap;
			break;
		    case TV_TROPES_TRAP:
			if (rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap;
			break;
		    case SHUEFT_TRAP:
			if (rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap;
			break;
		    case CONDUCT_TRAP:
			if (rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap;
			break;
		    case SKILL_POINT_LOSS_TRAP:
			if (rn2(evilfriday ? 20 : 200) && !NastyTrapNation) goto selecttrap;
			break;
		    case DUMBIE_LIGHTSABER_TRAP:
			if (rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap;
			break;
		    case WRONG_STAIRS:
			if (rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap;
			break;
		    case AMNESIA_SWITCH_TRAP:
			if (rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap;
			break;
		    case DATA_DELETE_TRAP:
			if (rn2(evilfriday ? 1000 : 10000)) goto selecttrap;
			break;
		    case ELDER_TENTACLING_TRAP:
			if (rn2(evilfriday ? 100 : 1000)) goto selecttrap;
			break;
		    case FOOTERER_TRAP:
			if (rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap;
			break;

		    case RECURSION_TRAP:
			if (rn2(500) && !NastyTrapNation) goto selecttrap;
			break;
		    case TEMPORARY_RECURSION_TRAP:
			if (rn2(50) && !NastyTrapNation) goto selecttrap;
			break;
		    case WARP_ZONE:
			if (rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap;
			break;
		    case KOP_CUBE:
			if (rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap;
			break;
		    case BOSS_SPAWNER:
			if (rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap;
			break;
		    case MIND_WIPE_TRAP:
			if (rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap;
			break;
		    case GATEWAY_FROM_HELL:
			if (rn2(evilfriday ? 2 : 20) && !NastyTrapNation) goto selecttrap;
			break;

		    case PESTILENCE_TRAP:
			if (rn2(evilfriday ? 5 : 20) && !NastyTrapNation) goto selecttrap;
			break;
		    case FAMINE_TRAP:
			if ((rn2(2) && !evilfriday) && !NastyTrapNation) goto selecttrap;
			break;

		    case ARTIFACT_JACKPOT_TRAP:
			if (rn2(200)) goto selecttrap;
			break;
		    case SPREADING_TRAP:
			if (rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap;
			break;
		    case SUPERTHING_TRAP:
			if (rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap;
			break;
		    case CURRENT_SHAFT:
			if (In_sokoban(&u.uz) && rn2(10)) goto selecttrap;
			if (rn2(3) && !NastyTrapNation) goto selecttrap;
			break;
		    case LEVITATION_TRAP:
			if (rn2(evilfriday ? 2 : 3) && !NastyTrapNation) goto selecttrap;
			break;
		    case GOOD_ARTIFACT_TRAP:
			if (rn2(200)) goto selecttrap;
			break;
		    case GENDER_TRAP:
			if (rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case TRAP_OF_OPPOSITE_ALIGNMENT:
			if (rn2(20) && !NastyTrapNation) goto selecttrap;
			break;
		    case PET_TRAP:
			if (rn2(evilfriday ? 5 : 25) && !NastyTrapNation) goto selecttrap;
			break;
		    case PIERCING_BEAM_TRAP:
			if (rn2(3) && !NastyTrapNation) goto selecttrap;
			break;
		    case BACK_TO_START_TRAP:
			if (rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap;
			break;
		    case NEMESIS_TRAP:
			if (rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap;
			break;
		    case STREW_TRAP:
			if (rn2(evilfriday ? 5 : 30) && !NastyTrapNation) goto selecttrap;
			break;
		    case OUTTA_DEPTH_TRAP:
			if (rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap;
			break;
		    case ITEM_NASTIFICATION_TRAP:
			if (rn2(evilfriday ? 20 : 200) && !NastyTrapNation) goto selecttrap;
			break;
		    case GAY_TRAP:
			if (rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap;
			break;
		    case BOON_TRAP:
			if (rn2(200)) goto selecttrap;
			break;
		    case ANOXIC_PIT:
			if (In_sokoban(&u.uz) && rn2(10)) goto selecttrap;
			if ((rn2(3) && !evilfriday) && !NastyTrapNation) goto selecttrap;
			break;
		    case ARABELLA_SPEAKER:
			if (rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case FEMMY_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case MADELEINE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case MARLENA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case NADINE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case LUISA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case IRINA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case LISELOTTE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case GRETA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case JANE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case SUE_LYN_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case CHARLOTTE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case HANNAH_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case LITTLE_MARIE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(50) && !NastyTrapNation) goto selecttrap;
			break;
		    case KSENIA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case LYDIA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case CONNY_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case KATIA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap;
			break;
		    case MARIYA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(15) && !NastyTrapNation) goto selecttrap;
			break;
		    case ELISE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case RONJA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case ARIANE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case JOHANNA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case INGE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case ROSA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case KRISTIN_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case ANNA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap;
			break;
		    case RUEA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case DORA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case MARIKE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(25) && !NastyTrapNation) goto selecttrap;
			break;
		    case JETTE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap;
			break;
		    case INA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case SING_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap;
			break;
		    case VICTORIA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case MELISSA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case ANITA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case HENRIETTA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap;
			break;
		    case VERENA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case ARABELLA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(25) && !NastyTrapNation) goto selecttrap;
			break;
		    case SARAH_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case CLAUDIA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case LUDGERA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case KATI_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case ANASTASIA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case ANNEMARIE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case JIL_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case JANINA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case JANA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case KATRIN_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap;
			break;
		    case GUDRUN_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case ELLA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case MANUELA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(15) && !NastyTrapNation) goto selecttrap;
			break;
		    case JENNIFER_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case PATRICIA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case ANTJE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case ANTJE_TRAP_X:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case KERSTIN_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case LAURA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case LARISSA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap;
			break;
		    case NORA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case NATALIA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case SUSANNE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(25) && !NastyTrapNation) goto selecttrap;
			break;
		    case LISA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case BRIDGHITTE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case JULIA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap;
			break;
		    case NICOLE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case RITA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap;
			break;
		    case RUTH_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap;
			break;
		    case MAGDALENA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case MARLEEN_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case KLARA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case FRIEDERIKE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(25) && !NastyTrapNation) goto selecttrap;
			break;
		    case NAOMI_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case UTE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(30) && !NastyTrapNation) goto selecttrap;
			break;
		    case JASIEEN_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case YASAMAN_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap;
			break;
		    case MAY_BRITT_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case TOXIC_VENOM_TRAP:
			if (rn2(evilfriday ? 2 : 7) && !NastyTrapNation) goto selecttrap;
			break;
		    case INSANITY_TRAP:
			if (rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap;
			break;
		    case MADNESS_TRAP:
			if (rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap;
			break;
		    case JESSICA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case SOLVEJG_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case WENDY_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case KATHARINA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case ELENA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case THAI_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case ELIF_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case NADJA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap;
			break;
		    case SANDRA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(15) && !NastyTrapNation) goto selecttrap;
			break;
		    case NATALJE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(50) && !NastyTrapNation) goto selecttrap;
			break;
		    case JEANETTA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case YVONNE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case MAURAH_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case MELTEM_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case NELLY_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case EVELINE_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case KARIN_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case JUEN_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case KRISTINA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case LOU_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;
		    case ALMUT_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case JULIETTA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap;
			break;

		    case PREMATURE_DEATH_TRAP:
			if (rn2(evilfriday ? 10 : 200) && !NastyTrapNation) goto selecttrap;
			break;
		    case RAGNAROK_TRAP:
			if (rn2(evilfriday ? 64 : 640) && !NastyTrapNation) goto selecttrap;
			break;
		    case SEVERE_DISENCHANT_TRAP:
			if (rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap;
			break;

		    case SIN_TRAP:
			if (rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap;
			break;
		    case DESTROY_ARMOR_TRAP:
			if (rn2(evilfriday ? 2 : 15) && !NastyTrapNation) goto selecttrap;
			break;
		    case DIVINE_ANGER_TRAP:
			if (rn2(evilfriday ? 5 : 40) && !NastyTrapNation) goto selecttrap;
			break;
		    case GENETIC_TRAP:
			if (rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap;
			break;
		    case MISSINGNO_TRAP:
			if (rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap;
			break;
		    case CANCELLATION_TRAP:
			if (rn2(evilfriday ? 5 : 25) && !NastyTrapNation) goto selecttrap;
			break;
		    case HOSTILITY_TRAP:
			if (rn2(evilfriday ? 2 : 4) && !NastyTrapNation) goto selecttrap;
			break;
		    case BOSS_TRAP:
			if (rn2(evilfriday ? 2 : 16) && !NastyTrapNation) goto selecttrap;
			break;
		    case WISHING_TRAP:
			if (rn2(500)) goto selecttrap;
			break;
		    case GUILLOTINE_TRAP:
			if (rn2(evilfriday ? 10 : 200) && !NastyTrapNation) goto selecttrap;
			break;
		    case BISECTION_TRAP:
			if (rn2(evilfriday ? 10 : 200) && !NastyTrapNation) goto selecttrap;
			break;
		    case HORDE_TRAP:
			if (rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap;
			break;
		    case IMMOBILITY_TRAP:
			if (rn2(evilfriday ? 2 : 15) && !NastyTrapNation) goto selecttrap;
			break;
		    case GREEN_GLYPH:
			if (rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap;
			break;
		    case BLUE_GLYPH:
			if (rn2(evilfriday ? 4 : 10) && !NastyTrapNation) goto selecttrap;
			break;
		    case YELLOW_GLYPH:
			if (rn2(evilfriday ? 3 : 7) && !NastyTrapNation) goto selecttrap;
			break;
		    case ORANGE_GLYPH:
			if (rn2(evilfriday ? 5 : 20) && !NastyTrapNation) goto selecttrap;
			break;
		    case BLACK_GLYPH:
			if (rn2(evilfriday ? 6 : 35) && !NastyTrapNation) goto selecttrap;
			break;
		    case PURPLE_GLYPH:
			if (rn2(evilfriday ? 7 : 49) && !NastyTrapNation) goto selecttrap;
			break;

		}
		if (!rn2(12)) kind = STATUE_TRAP;

	    } while (kind == NO_TRAP);
	}

	if ((kind == TRAPDOOR || kind == SHAFT_TRAP || kind == CURRENT_SHAFT || kind == HOLE) && !Can_fall_thru(&u.uz) && !Is_stronghold(&u.uz) )
		kind = ROCKTRAP;

	if (tm)
	    m = *tm;
	else {
	    register int tryct = 0;
	    boolean avoid_boulder = (kind == PIT || kind == SPIKED_PIT || kind == GIANT_CHASM || kind == SHIT_PIT || kind == MANA_PIT || kind == ANOXIC_PIT || kind == ACID_PIT || kind == SHAFT_TRAP || kind == CURRENT_SHAFT ||
				     kind == TRAPDOOR || kind == HOLE);

	    do {
		if (++tryct > 200)
		    return;
		if (mazeflag)
		    mazexy_all(&m);
		else if (!somexy(croom,&m))
		    return;
	    } while (occupied(m.x, m.y) ||
			(avoid_boulder && sobj_at(BOULDER, m.x, m.y)));
	}

	(void) maketrap(m.x, m.y, kind, isspecific ? ((u.monstertimefinish % 2) ? 5 : 10) : 100, cangivehp);
	/* Webs can generate on dlvl1, where giant spiders would be totally out of depth. Let's make random spiders. --Amy */
	if (kind == WEB) (void) makemon( /*&mons[PM_GIANT_SPIDER]*/ mkclass(S_SPIDER,0),
						m.x, m.y, MM_MAYSLEEP);
}

void
mkstairs(x, y, up, croom)
xchar x, y;
char  up;
struct mkroom *croom;
{
	if (!x) {
	    impossible("mkstairs:  bogus stair attempt at <%d,%d>", x, y);
	    return;
	}

	/*
	 * We can't make a regular stair off an end of the dungeon.  This
	 * attempt can happen when a special level is placed at an end and
	 * has an up or down stair specified in its description file.
	 */
	if ((dunlev(&u.uz) == 1 && up) ||
			(dunlev(&u.uz) == real_dunlevs_in_dungeon(&u.uz) && !up))
	    return;

	if(up) {
		xupstair = x;
		yupstair = y;
		upstairs_room = croom;
	} else {
		xdnstair = x;
		ydnstair = y;
		dnstairs_room = croom;
	}

	levl[x][y].typ = STAIRS;
	levl[x][y].ladder = up ? LA_UP : LA_DOWN;
}

STATIC_OVL
void
mkfount(mazeflag,croom)
register int mazeflag;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if(mazeflag)
		mazexy_all(&m);
	    else
		if (!somexy(croom, &m))
		    return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a fountain at m.x, m.y */
	levl[m.x][m.y].typ = FOUNTAIN;
	/* Is it a "blessed" fountain? (affects drinking from fountain) */
	if(!rn2(7)) levl[m.x][m.y].blessedftn = 1;

	level.flags.nfountains++;
}

STATIC_OVL void
mkcloudX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = CLOUD;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkgravewallX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = GRAVEWALL;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mktunnelwallX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = TUNNELWALL;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkfarmlandX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = FARMLAND;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkmountainX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = MOUNTAIN;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkwatertunnelX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = WATERTUNNEL;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkcrystalwaterX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = CRYSTALWATER;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkmoorlandX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = MOORLAND;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkurinelakeX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = URINELAKE;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkshiftingsandX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = SHIFTINGSAND;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkstyxriverX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = STYXRIVER;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mksnowX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = SNOW;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkashX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = ASH;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mksandX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = SAND;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkpavedfloorX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = PAVEDFLOOR;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkhighwayX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = HIGHWAY;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkgrasslandX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = GRASSLAND;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mknethermistX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = NETHERMIST;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkstalactiteX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = STALACTITE;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkcryptfloorX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = CRYPTFLOOR;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkbubblesX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = BUBBLES;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkraincloudX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = RAINCLOUD;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkiceX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = ICE;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkironbarsX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = IRONBARS;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mklavapoolX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = LAVAPOOL;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkpoolX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = POOL;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mktreeX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = TREE;
		n++;
		tryct = 0;

	}

}

STATIC_OVL void
mkstoneX(amount,croom)
register int amount;
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	int n = 0;

	while (n < amount) {

		do {
		    if(++tryct > 1) return;
		    if (!somexy(croom, &m))
			return;
		} while(occupied(m.x, m.y) || bydoor(m.x, m.y));
	
		levl[m.x][m.y].typ = STONE;
		n++;
		tryct = 0;

	}
}

STATIC_OVL void
mkcloud(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = CLOUD;

}

STATIC_OVL void
mkgravewall(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = GRAVEWALL;

}

STATIC_OVL void
mktunnelwall(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = TUNNELWALL;

}

STATIC_OVL void
mkfarmland(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = FARMLAND;

}

STATIC_OVL void
mkmountain(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = MOUNTAIN;

}

STATIC_OVL void
mkwatertunnel(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = WATERTUNNEL;

}

STATIC_OVL void
mkcrystalwater(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = CRYSTALWATER;

}

STATIC_OVL void
mkmoorland(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = MOORLAND;

}

STATIC_OVL void
mkurinelake(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = URINELAKE;

}

STATIC_OVL void
mkshiftingsand(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = SHIFTINGSAND;

}

STATIC_OVL void
mkstyxriver(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = STYXRIVER;

}

STATIC_OVL void
mksnow(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = SNOW;

}

STATIC_OVL void
mkash(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = ASH;

}

STATIC_OVL void
mksand(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = SAND;

}

STATIC_OVL void
mkpavedfloor(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = PAVEDFLOOR;

}

STATIC_OVL void
mkhighway(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = HIGHWAY;

}

STATIC_OVL void
mkgrassland(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = GRASSLAND;

}

STATIC_OVL void
mknethermist(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = NETHERMIST;

}

STATIC_OVL void
mkstalactite(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = STALACTITE;

}

STATIC_OVL void
mkcryptfloor(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = CRYPTFLOOR;

}

STATIC_OVL void
mkbubbles(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = BUBBLES;

}

STATIC_OVL void
mkraincloud(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = RAINCLOUD;

}

STATIC_OVL void
mkice(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = ICE;

}

STATIC_OVL void
mkdoor(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = DOOR;

}

STATIC_OVL void
mkironbars(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = IRONBARS;

}

STATIC_OVL void
mklavapool(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = LAVAPOOL;

}

STATIC_OVL void
mkpool(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = POOL;

}

STATIC_OVL void
mktree(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = TREE;

}

STATIC_OVL void
mkstone(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = STONE;

}

STATIC_OVL void
mkthrone(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = THRONE;

}

STATIC_OVL void
mkpentagram(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = PENTAGRAM;

}

STATIC_OVL void
mkwell(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = WELL;
	/* occasionally it's a blessed one --Amy */
	if(!rn2(10)) levl[m.x][m.y].blessedftn = 1;

}

STATIC_OVL void
mkpoisonedwell(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = POISONEDWELL;

}

STATIC_OVL void
mkwagon(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = WAGON;

}

STATIC_OVL void
mkburningwagon(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = BURNINGWAGON;

}

STATIC_OVL void
mkwoodentable(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = WOODENTABLE;

}

STATIC_OVL void
mkcarvedbed(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = CARVEDBED;

}

STATIC_OVL void
mkstrawmattress(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = STRAWMATTRESS;

}

STATIC_OVL void
mksink(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a sink at m.x, m.y */
	levl[m.x][m.y].typ = SINK;

	level.flags.nsinks++;
}


static void
mktoilet(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a toilet at m.x, m.y */
	levl[m.x][m.y].typ = TOILET;

	level.flags.nsinks++; /* counted as a sink for sounds.c */
}

STATIC_OVL void
mkaltar(croom)
register struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	aligntyp al;

	if(croom->rtype != OROOM) return;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while(occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put an altar at m.x, m.y */
	levl[m.x][m.y].typ = ALTAR;

	/* -1 - A_CHAOTIC, 0 - A_NEUTRAL, 1 - A_LAWFUL */
	al = rn2((int)A_LAWFUL+2) - 1;
	if (!rn2(10) || issoviet) levl[m.x][m.y].altarmask = Align2amask( al );
	/* "Unaligned altars have next to no reason for being generated in the early part of the game." Of course they do! They're there to reduce the chance of players lucking into a coaligned altar! Shouldn't that be obvious? --Amy */
	else levl[m.x][m.y].altarmask = Align2amask( A_NONE );
}

static void
mkgrave(croom)
struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	register struct obj *otmp;
	boolean dobell = !rn2(100); /* reduced chance by Amy */


	if(croom->rtype != OROOM) return;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while (occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a grave at m.x, m.y */
	make_grave(m.x, m.y, dobell ? "Saved by the bell!" : (char *) 0);

	/* Possibly fill it with objects */
	if (!rn2(5)) (void) mkgold(0L, m.x, m.y);
	for (tryct = rn2(2 + rn2(4)); tryct; tryct--) {
		if (timebasedlowerchance()) {
		    otmp = mkobj(rn2(3) ? COIN_CLASS : RANDOM_CLASS, TRUE, FALSE);
		    if (!otmp) return;
		    curse(otmp);
		    otmp->ox = m.x;
		    otmp->oy = m.y;
		    add_to_buried(otmp);
		}
	}

	/* Leave a bell, in case we accidentally buried someone alive */
	if (dobell) (void) mksobj_at(BELL, m.x, m.y, TRUE, FALSE, FALSE);
	return;
}

/* maze levels have slightly different constraints from normal levels */
#define x_maze_min 2
#define y_maze_min 2
/*
 * Major level transmutation: add a set of stairs (to the Sanctum) after
 * an earthquake that leaves behind a a new topology, centered at inv_pos.
 * Assumes there are no rooms within the invocation area and that inv_pos
 * is not too close to the edge of the map.  Also assume the hero can see,
 * which is guaranteed for normal play due to the fact that sight is needed
 * to read the Book of the Dead.
 */
void
mkinvokearea()
{
    int dist;
    xchar xmin = inv_pos.x, xmax = inv_pos.x;
    xchar ymin = inv_pos.y, ymax = inv_pos.y;
    register xchar i;

    pline_The("floor shakes violently under you!");
    pline_The("walls around you begin to bend and crumble!");
    display_nhwindow(WIN_MESSAGE, TRUE);

    mkinvpos(xmin, ymin, 0);		/* middle, before placing stairs */

    for(dist = 1; dist < 7; dist++) {
	xmin--; xmax++;

	/* top and bottom */
	if(dist != 3) { /* the area is wider that it is high */
	    ymin--; ymax++;
	    for(i = xmin+1; i < xmax; i++) {
		mkinvpos(i, ymin, dist);
		mkinvpos(i, ymax, dist);
	    }
	}

	/* left and right */
	for(i = ymin; i <= ymax; i++) {
	    mkinvpos(xmin, i, dist);
	    mkinvpos(xmax, i, dist);
	}

	flush_screen(1);	/* make sure the new glyphs shows up */
	delay_output();
    }

    You("are standing at the top of a stairwell leading down!");
    mkstairs(u.ux, u.uy, 0, (struct mkroom *)0); /* down */
    newsym(u.ux, u.uy);
    vision_full_recalc = 1;	/* everything changed */

#ifdef RECORD_ACHIEVE

	if (!achieve.perform_invocation) {
		trophy_get();
	}

    achieve.perform_invocation = 1;
#ifdef LIVELOGFILE
    livelog_achieve_update();
    livelog_report_trophy("performed the invocation");

#endif
#endif

	/* let's spice things up a bit... --Amy */
    (void) makemon(&mons[PM_CHAOS_MONKEY], 0, 0, NO_MM_FLAGS);

}

/* Change level topology.  Boulders in the vicinity are eliminated.
 * Temporarily overrides vision in the name of a nice effect.
 */
STATIC_OVL void
mkinvpos(x,y,dist)
xchar x,y;
int dist;
{
    struct trap *ttmp;
    struct obj *otmp;
    boolean make_rocks;
    register struct rm *lev = &levl[x][y];

    /* clip at existing map borders if necessary */
    if (!within_bounded_area(x, y, x_maze_min + 1, y_maze_min + 1,
				   x_maze_max - 1, y_maze_max - 1)) {
	/* only outermost 2 columns and/or rows may be truncated due to edge */
	if (dist < (7 - 2))
	    panic("mkinvpos: <%d,%d> (%d) off map edge!", x, y, dist);
	return;
    }

    /* clear traps */
    if ((ttmp = t_at(x,y)) != 0) deltrap(ttmp);

    /* clear boulders; leave some rocks for non-{moat|trap} locations */
    make_rocks = (dist != 1 && dist != 4 && dist != 5) ? TRUE : FALSE;
    while ((otmp = sobj_at(BOULDER, x, y)) != 0) {
	if (make_rocks) {
	    fracture_rock(otmp);
	    make_rocks = FALSE;		/* don't bother with more rocks */
	} else {
	    obj_extract_self(otmp);
	    obfree(otmp, (struct obj *)0);
	}
    }
    unblock_point(x,y);	/* make sure vision knows this location is open */

    /* fake out saved state */
    lev->seenv = 0;
    lev->doormask = 0;
    if(dist < 6) lev->lit = TRUE;
    lev->waslit = TRUE;
    lev->horizontal = FALSE;
    viz_array[y][x] = (dist < 6 ) ?
	(IN_SIGHT|COULD_SEE) : /* short-circuit vision recalc */
	COULD_SEE;

    switch(dist) {
    case 1: /* fire traps */
	if (is_waterypool(x,y)) break;
	lev->typ = ROOM;
	ttmp = maketrap(x, y, FIRE_TRAP, 0, TRUE);
	if (ttmp) ttmp->tseen = TRUE;
	break;
    case 0: /* lit room locations */
    case 2:
    case 3:
    case 6: /* unlit room locations */
	lev->typ = ROOM;
	break;
    case 4: /* pools (aka a wide moat) */
    case 5:
	lev->typ = MOAT;
	/* No kelp! */
	break;
    default:
	impossible("mkinvpos called with dist %d", dist);
	break;
    }

    /* display new value of position; could have a monster/object on it */
    newsym(x,y);
}

/*
 * The portal to Ludios is special.  The entrance can only occur within a
 * vault in the main dungeon at a depth greater than 10.  The Ludios branch
 * structure reflects this by having a bogus "source" dungeon:  the value
 * of n_dgns (thus, Is_branchlev() will never find it).
 *
 * Ludios will remain isolated until the branch is corrected by this function.
 */
void
mk_knox_portal(x, y)
xchar x, y;
{
	extern int n_dgns;		/* from dungeon.c */
	d_level *source;
	branch *br;
	schar u_depth;

	br = dungeon_branch("Fort Ludios");
	if (on_level(&knox_level, &br->end1)) {
	    source = &br->end2;
	} else {
	    /* disallow Knox branch on a level with one branch already */
	    if(Is_branchlev(&u.uz))
		return;
	    source = &br->end1;
	}

	/* Already set or 2/3 chance of deferring until a later level. */
	if (source->dnum < n_dgns || (!rn2(5) /* Amy change to make sure the chance of it appearing is higher */
#ifdef WIZARD
				      && !wizard
#endif
				      )) return;

	if (! (u.uz.dnum == oracle_level.dnum	    /* in main dungeon */
		&& !at_dgn_entrance("The Quest")    /* but not Quest's entry */
		&& (u_depth = depth(&u.uz)) > 10    /* beneath 10 */
		&& u_depth < depth(&medusa_level))) /* and above Medusa */
	    return;

	/* Adjust source to be current level and re-insert branch. */
	*source = u.uz;
	insert_branch(br, TRUE);

#ifdef DEBUG
	pline("Made knox portal.");
#endif
	place_branch(br, x, y);
}

STATIC_OVL struct permonst *
morguemonX()
{
	register int i = rn2(100), hd = rn2(level_difficulty());

	if(hd > 10 && i < 10)
		return((Inhell || In_endgame(&u.uz)) ? mkclass(S_DEMON,0) :
						       &mons[ndemon(A_NONE)]);
	if(hd > 8 && i > 90)
		return(mkclass(S_VAMPIRE,0));

	return((i < 30) ? mkclass(S_GHOST,0) : (i < 40) ? mkclass(S_WRAITH,0) : (i < 70) ? mkclass(S_MUMMY,0) : (i < 71) ? mkclass(S_LICH,0) : mkclass(S_ZOMBIE,0));
} /* added mummies, enabled all of S_wraith type monsters --Amy */

/*mklev.c*/
