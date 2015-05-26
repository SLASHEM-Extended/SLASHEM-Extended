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


STATIC_DCL void FDECL(mkfount,(int,struct mkroom *));
#ifdef SINKS
STATIC_DCL void FDECL(mksink,(struct mkroom *));
STATIC_DCL void FDECL(mktoilet,(struct mkroom *));
#endif
STATIC_DCL void FDECL(mkaltar,(struct mkroom *));
STATIC_DCL void FDECL(mkgrave,(struct mkroom *));
STATIC_DCL void FDECL(mkthrone,(struct mkroom *));
STATIC_DCL void NDECL(makevtele);
STATIC_DCL void NDECL(clear_level_structures);
STATIC_DCL void NDECL(makelevel);
STATIC_DCL void NDECL(mineralize);
STATIC_DCL boolean FDECL(bydoor,(XCHAR_P,XCHAR_P));
STATIC_DCL struct mkroom *FDECL(find_branch_room, (coord *));
STATIC_DCL struct mkroom *FDECL(pos_to_room, (XCHAR_P, XCHAR_P));
STATIC_DCL boolean FDECL(place_niche,(struct mkroom *,int*,int*,int*));
STATIC_DCL void FDECL(makeniche,(int));
STATIC_DCL void NDECL(make_niches);
STATIC_DCL struct permonst * NDECL(morguemonX);

STATIC_DCL void FDECL(mkstone,(struct mkroom *));
STATIC_DCL void FDECL(mktree,(struct mkroom *));
STATIC_DCL void FDECL(mkpool,(struct mkroom *));
STATIC_DCL void FDECL(mklavapool,(struct mkroom *));
STATIC_DCL void FDECL(mkironbars,(struct mkroom *));
STATIC_DCL void FDECL(mkdoor,(struct mkroom *));
STATIC_DCL void FDECL(mkice,(struct mkroom *));
STATIC_DCL void FDECL(mkcloud,(struct mkroom *));

STATIC_DCL void FDECL(mkstoneX,(int,struct mkroom *));
STATIC_DCL void FDECL(mktreeX,(int,struct mkroom *));
STATIC_DCL void FDECL(mkpoolX,(int,struct mkroom *));
STATIC_DCL void FDECL(mklavapoolX,(int,struct mkroom *));
STATIC_DCL void FDECL(mkironbarsX,(int,struct mkroom *));
STATIC_DCL void FDECL(mkiceX,(int,struct mkroom *));
STATIC_DCL void FDECL(mkcloudX,(int,struct mkroom *));

STATIC_DCL int NDECL(findrandtype);
/*STATIC_DCL int NDECL(randomwalltype);*/

STATIC_PTR int FDECL( CFDECLSPEC do_comp,(const genericptr,const genericptr));

STATIC_DCL void FDECL(dosdoor,(XCHAR_P,XCHAR_P,struct mkroom *,int));
STATIC_DCL void FDECL(join,(int,int,BOOLEAN_P));
STATIC_DCL void FDECL(do_room_or_subroom, (struct mkroom *,int,int,int,int,
				       BOOLEAN_P,SCHAR_P,BOOLEAN_P,BOOLEAN_P,BOOLEAN_P));
STATIC_DCL void NDECL(makerooms);
STATIC_DCL void FDECL(finddpos,(coord *,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P));
STATIC_DCL void FDECL(mkinvpos, (XCHAR_P,XCHAR_P,int));
STATIC_DCL void FDECL(mk_knox_portal, (XCHAR_P,XCHAR_P));

#define create_vault()	create_room(-1, -1, 2, 2, -1, -1, VAULT, TRUE, FALSE)
#define init_vault()	vault_x = -1
#define do_vault()	(vault_x != -1)
static xchar		vault_x, vault_y;
boolean goldseen;
static boolean made_branch;	/* used only during level creation */

/* Args must be (const genericptr) so that qsort will always be happy. */

STATIC_PTR int CFDECLSPEC
do_comp(vx,vy)
const genericptr vx;
const genericptr vy;
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
	switch (rnd(36)) {

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
	}

}

int
randomwalltype()
{
	switch (rnd(8)) {

		case 1: return TREE;
		case 2: return MOAT;
		case 3: return LAVAPOOL;
		case 4: return IRONBARS;
		case 5: return CORR;
		case 6: return ICE;
		case 7: return CLOUD;
		case 8: return STONE;
	}

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
	qsort((genericptr_t) rooms, (unsigned)nroom, sizeof(struct mkroom), do_comp);
#else
	qsort((genericptr_t) rooms, nroom, sizeof(struct mkroom), do_comp);
#endif
}

STATIC_OVL void
do_room_or_subroom(croom, lowx, lowy, hix, hiy, lit, rtype, special, is_room, canbeshaped)
    register struct mkroom *croom;
    int lowx, lowy;
    register int hix, hiy;
    boolean lit;
    schar rtype;
    boolean special;
    boolean is_room;
    boolean canbeshaped;
{
	register int x, y;
	struct rm *lev;

	int wallifytype = STONE;
	boolean wallifyxtra = 0;
	if ( !((moves + u.monstertimefinish) % 9357 ) || (!(u.monstertimefinish % 77) && !rn2(iswarper ? 50 : 500)) || (!(u.monstertimefinish % 773) && !rn2(iswarper ? 10 : 100)) || (!rn2(iswarper ? 100 : 5000))) {

		switch (rnd(7)) {

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
		}

		if (!rn2(50)) wallifyxtra = 1;

	}

	int wallifytypeB = STONE;
	boolean wallifyBxtra = 0;
	if ( !((moves + u.monstertimefinish) % 8357 ) || (!(u.monstertimefinish % 73) && !rn2(iswarper ? 100 : 500)) || (!(u.monstertimefinish % 673) && !rn2(iswarper ? 20 : 100)) || (!rn2(iswarper ? 200 : 5000))) {

		switch (rnd(7)) {

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
		}

		if (!rn2(50)) wallifyBxtra = 1;

	}

	croom->colouur = 0;
	if (!special && rtype == OROOM) croom->colouur = (!rn2(20) ? 20 : rn2(15) );
	if (!special && rtype >= SHOPBASE) croom->colouur = (!rn2(20) ? 20 : rn2(15) );
	if (!special && rtype == BEEHIVE) croom->colouur = CLR_YELLOW;
	if (!special && rtype == COURT) croom->colouur = CLR_MAGENTA;
	if (!special && rtype == SWAMP) croom->colouur = CLR_GREEN;
	if (!special && rtype == VAULT) croom->colouur = CLR_YELLOW;
	if (!special && rtype == MORGUE) croom->colouur = CLR_BLACK;
	if (!special && rtype == BARRACKS) croom->colouur = CLR_RED;
	if (!special && rtype == ZOO) croom->colouur = CLR_BROWN;
	if (!special && rtype == REALZOO) croom->colouur = CLR_BROWN;
	if (!special && rtype == DELPHI) croom->colouur = CLR_BRIGHT_BLUE;
	if (!special && rtype == TEMPLE) croom->colouur = CLR_CYAN;
	if (!special && rtype == GIANTCOURT) croom->colouur = CLR_CYAN;
	if (!special && rtype == LEPREHALL) croom->colouur = CLR_GREEN;
	if (!special && rtype == DRAGONLAIR) croom->colouur = CLR_BRIGHT_MAGENTA;
	if (!special && rtype == BADFOODSHOP) croom->colouur = CLR_RED;
	if (!special && rtype == COCKNEST) croom->colouur = CLR_YELLOW;
	if (!special && rtype == ANTHOLE) croom->colouur = CLR_BROWN;
	if (!special && rtype == LEMUREPIT) croom->colouur = CLR_BLACK;
	if (!special && rtype == MIGOHIVE) croom->colouur = CLR_BRIGHT_GREEN;
	if (!special && rtype == FUNGUSFARM) croom->colouur = CLR_BRIGHT_GREEN;
	if (!special && rtype == CLINIC) croom->colouur = CLR_ORANGE;
	if (!special && rtype == TERRORHALL) croom->colouur = CLR_BRIGHT_CYAN;
	if (!special && rtype == RIVERROOM) croom->colouur = CLR_BRIGHT_BLUE;
	if (!special && rtype == ELEMHALL) croom->colouur = CLR_GRAY;
	if (!special && rtype == ANGELHALL) croom->colouur = CLR_WHITE;
	if (!special && rtype == NYMPHHALL) croom->colouur = CLR_GREEN;
	if (!special && rtype == SPIDERHALL) croom->colouur = CLR_GRAY;
	if (!special && rtype == TROLLHALL) croom->colouur = CLR_BROWN;
	if (!special && rtype == HUMANHALL) croom->colouur = CLR_BRIGHT_BLUE;
	if (!special && rtype == GOLEMHALL) croom->colouur = CLR_GRAY;
	if (!special && rtype == COINHALL) croom->colouur = CLR_YELLOW;
	if (!special && rtype == DOUGROOM) croom->colouur = CLR_BRIGHT_CYAN;
	if (!special && rtype == ARMORY) croom->colouur = CLR_CYAN;
	if (!special && rtype == TENSHALL) croom->colouur = 20;
	if (!special && rtype == INSIDEROOM) croom->colouur = 20;
	if (!special && rtype == POOLROOM) croom->colouur = CLR_BRIGHT_BLUE;

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
		    levl[x][y].typ = (wallifytypeB ? (wallifyBxtra ? randomwalltype() : wallifytypeB) : HWALL);
		    levl[x][y].horizontal = 1;	/* For open/secret doors. */
		}
	    for(x = lowx-1; x <= hix+1; x += (hix-lowx+2))
		for(y = lowy; y <= hiy; y++) {
		    levl[x][y].typ = (wallifytypeB ? (wallifyBxtra ? randomwalltype() : wallifytypeB) : VWALL);
		    levl[x][y].horizontal = 0;	/* For open/secret doors. */
		}
	    for(x = lowx; x <= hix; x++) {
		lev = &levl[x][lowy];
		for(y = lowy; y <= hiy; y++)
		    lev++->typ = ROOM;
	    }
	    if (is_room) {
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
                xcmax = (hix - lowx) * 2 / 5;
                ycmax = (hiy - lowy) * 2 / 5;
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
                xcmax = (hix - lowx) * 2 / 3;
                ycmax = (hiy - lowy) * 2 / 3;
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
                xcmax = (hix - lowx) * 2 / 5;
                ycmax = (hiy - lowy) * 2 / 5;
                dotr = TRUE;  
                dotl = TRUE;  
                dobr = TRUE;  
                dobl = TRUE;  
                break;  
            case 8:  
                /* square-O shaped (pillar cut out of middle) */  
                xcmax = (hix - lowx) / 2;  
                ycmax = (hiy - lowy) / 2;  
                docenter = TRUE;  
                break;  
            case 9:  
                /* X-shaped */  
                xcmax = (hix - lowx) / 3;  
                ycmax = (hiy - lowy) / 3;  
                dotr = TRUE;  
                dotl = TRUE;  
                dobr = TRUE;  
                dobl = TRUE;  
                docenter = TRUE;  
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
		wallification(lowx-1, lowy-1, hix+1, hiy+1, rn2( !(u.monstertimefinish % 87) ? (iswarper ? 3 : 30) : (iswarper ? 10 : 200) ) ? FALSE : TRUE);
	    }
	}
}


void
add_room(lowx, lowy, hix, hiy, lit, rtype, special, canbeshaped)
register int lowx, lowy, hix, hiy;
boolean lit;
schar rtype;
boolean special;
boolean canbeshaped;
{
	register struct mkroom *croom;

	croom = &rooms[nroom];
	do_room_or_subroom(croom, lowx, lowy, hix, hiy, lit,
					    rtype, special, (boolean) TRUE, canbeshaped);
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
					    rtype, special, (boolean) FALSE, (boolean) FALSE);
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
		if(nroom >= (MAXNROFROOMS/6) && rn2(2) && !tried_vault) {
			tried_vault = TRUE;
			if (create_vault()) {
				vault_x = rooms[nroom].lx;
				vault_y = rooms[nroom].ly;
				rooms[nroom].hx = -1;
			}
		} else
		    if (!create_room(-1, -1, -1, -1, -1, -1, OROOM, -1, TRUE) && !rn2(10) )
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

	if (!(u.monstertimefinish % 337) && !rn2(iswarper ? 10 : 50)) specialcorridor = 1;

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
		if (!dig_corridor(&org, &dest, nxcor, rn2(2) ? ICE : CLOUD, STONE))
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
	int a, b, i;
	boolean any = TRUE;

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

		if (level_difficulty() >= /*9*/5 && !rn2(5) ) {
		    /* make a mimic instead */
		    levl[x][y].doormask = D_NODOOR;
		    mtmp = makemon(mkclass(S_MIMIC,0), x, y, NO_MM_FLAGS);
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
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
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
		    if((trap_type == HOLE || trap_type == TRAPDOOR)
			&& !Can_fall_thru(&u.uz))
			trap_type = ROCKTRAP;
		    ttmp = maketrap(xx, yy+dy, trap_type);
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
		if(rn2(7))
		    dosdoor(xx, yy, aroom, rn2(5) ? SDOOR : DOOR);
		else {

		    if (!rn2(2) && IS_WALL(levl[xx][yy].typ)) levl[xx][yy].typ = IRONBARS;

		    if (!level.flags.noteleport)
			(void) mksobj_at(SCR_TELEPORTATION,
					 xx, yy+dy, TRUE, FALSE);
		    if (!rn2(3)) (void) mkobj_at(0, xx, yy+dy, TRUE);
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

	boolean	ltptr = (!level.flags.noteleport && dep > 15),
		vamp = (dep > 5 && dep < 25);

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
	if (rndomizat) trynmbr = (rn2(5) ? rnd(20) : rnd(7));

	if (rn2(100) < chance && !t_at(cx,cy)) {
	    if (lava) {
		if (rndom) { 

			if (trynmbr == 1) levl[cx][cy].typ = POOL;
			else if (trynmbr == 2) levl[cx][cy].typ = TREE;
			else if (trynmbr == 3) levl[cx][cy].typ = IRONBARS;
			else if (trynmbr == 4) levl[cx][cy].typ = ICE;
			else if (trynmbr == 5) levl[cx][cy].typ = CLOUD;
			else if (trynmbr == 6) levl[cx][cy].typ = CORR;
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
			else levl[cx][cy].typ = POOL;
		}
		else 
		levl[cx][cy].typ = !rn2(3) ? POOL : MOAT;

		if (!rn2(ishaxor ? 10000 : 20000))
			levl[cx][cy].typ = THRONE;
		else if (!((moves + u.monstertimefinish) % 857 ) && !rn2(ishaxor ? 1000 : 2000))
			levl[cx][cy].typ = THRONE;
		else if (!rn2(ishaxor ? 2500 : 5000)) {
			levl[cx][cy].typ = FOUNTAIN;
			level.flags.nfountains++;
			}
		else if (!((moves + u.monstertimefinish) % 859 ) && !rn2(ishaxor ? 250 : 500)) {
			levl[cx][cy].typ = FOUNTAIN;
			level.flags.nfountains++;
			}
		else if (!rn2(ishaxor ? 2500 : 5000)) {
			levl[cx][cy].typ = SINK;
			level.flags.nsinks++;
			}
		else if (!((moves + u.monstertimefinish) % 861 ) && !rn2(ishaxor ? 250 : 500)) {
			levl[cx][cy].typ = SINK;
			level.flags.nsinks++;
			}
		else if (!rn2(ishaxor ? 5000 : 10000))
			levl[cx][cy].typ = TOILET;
		else if (!((moves + u.monstertimefinish) % 863 ) && !rn2(ishaxor ? 500 : 1000))
			levl[cx][cy].typ = TOILET;
		else if (!rn2(ishaxor ? 1000 : 2000)) {
			levl[cx][cy].typ = GRAVE;
	
			if (!rn2(3)) (void) mkgold(0L, cx, cy);
			for (tryct = rn2(5); tryct; tryct--) {
				    otmpX = mkobj(RANDOM_CLASS, TRUE);
				    if (!otmpX) return;
				    curse(otmpX);
				    otmpX->ox = cx;
				    otmpX->oy = cy;
				    add_to_buried(otmpX);
				}
			}
		else if (!((moves + u.monstertimefinish) % 865 ) && !rn2(ishaxor ? 100 : 200)) {
			levl[cx][cy].typ = GRAVE;
	
			if (!rn2(3)) (void) mkgold(0L, cx, cy);
			for (tryct = rn2(5); tryct; tryct--) {
				    otmpX = mkobj(RANDOM_CLASS, TRUE);
				    if (!otmpX) return;
				    curse(otmpX);
				    otmpX->ox = cx;
				    otmpX->oy = cy;
				    add_to_buried(otmpX);
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
		else if (!((moves + u.monstertimefinish) % 867 ) && !rn2(ishaxor ? 1000 : 2000)) {
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
makerandriver(lava,rndom)
boolean lava,rndom;

{
    int cx,cy;
    int chance;
    int count = 0;
    int ammount = rnz(10 + rnd(40) + rnz(5) + (rn2(5) ? 0 : 50) + (rn2(25) ? 0 : 200) );
    int trynmbr = 0;
    int rndomizat = 0;
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
	if (rndomizat) trynmbr = (rn2(5) ? rnd(12) : rnd(7));

	if (rn2(100) < chance && !t_at(cx,cy)) {
	    if (lava) {
		if (rndom) { 

			if (trynmbr == 1) levl[cx][cy].typ = POOL;
			else if (trynmbr == 2) levl[cx][cy].typ = TREE;
			else if (trynmbr == 3) levl[cx][cy].typ = IRONBARS;
			else if (trynmbr == 4) levl[cx][cy].typ = ICE;
			else if (trynmbr == 5) levl[cx][cy].typ = CLOUD;
			else if (trynmbr == 6) levl[cx][cy].typ = CORR;
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
			else levl[cx][cy].typ = POOL;
		}
		else 
		levl[cx][cy].typ = !rn2(3) ? POOL : MOAT;

		if (!rn2(ishaxor ? 10000 : 20000))
			levl[cx][cy].typ = THRONE;
		else if (!((moves + u.monstertimefinish) % 877 ) && !rn2(ishaxor ? 1000 : 2000))
			levl[cx][cy].typ = THRONE;
		else if (!rn2(ishaxor ? 2500 : 5000)) {
			levl[cx][cy].typ = FOUNTAIN;
			level.flags.nfountains++;
			}
		else if (!((moves + u.monstertimefinish) % 879 ) && !rn2(ishaxor ? 250 : 500)) {
			levl[cx][cy].typ = FOUNTAIN;
			level.flags.nfountains++;
			}
		else if (!rn2(ishaxor ? 2500 : 5000)) {
			levl[cx][cy].typ = SINK;
			level.flags.nsinks++;
			}
		else if (!((moves + u.monstertimefinish) % 881 ) && !rn2(ishaxor ? 250 : 500)) {
			levl[cx][cy].typ = SINK;
			level.flags.nsinks++;
			}
		else if (!rn2(ishaxor ? 5000 : 10000))
			levl[cx][cy].typ = TOILET;
		else if (!((moves + u.monstertimefinish) % 883 ) && !rn2(ishaxor ? 500 : 1000))
			levl[cx][cy].typ = TOILET;
		else if (!rn2(ishaxor ? 1000 : 2000)) {
			levl[cx][cy].typ = GRAVE;
	
			if (!rn2(3)) (void) mkgold(0L, cx, cy);
			for (tryct = rn2(5); tryct; tryct--) {
				    otmpX = mkobj(RANDOM_CLASS, TRUE);
				    if (!otmpX) return;
				    curse(otmpX);
				    otmpX->ox = cx;
				    otmpX->oy = cy;
				    add_to_buried(otmpX);
				}
			}
		else if (!((moves + u.monstertimefinish) % 885 ) && !rn2(ishaxor ? 100 : 200)) {
			levl[cx][cy].typ = GRAVE;
	
			if (!rn2(3)) (void) mkgold(0L, cx, cy);
			for (tryct = rn2(5); tryct; tryct--) {
				    otmpX = mkobj(RANDOM_CLASS, TRUE);
				    if (!otmpX) return;
				    curse(otmpX);
				    otmpX->ox = cx;
				    otmpX->oy = cy;
				    add_to_buried(otmpX);
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
		else if (!((moves + u.monstertimefinish) % 887 ) && !rn2(ishaxor ? 1000 : 2000)) {
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
      rndom = (rn2(5) ? 0 : 1);
	if (rn2(2)) makeriver(0, rn2(ROWNO), COLNO-1, rn2(ROWNO), lava, rndom);
	else makeriver(rn2(COLNO), 0, rn2(COLNO), ROWNO-1, lava, rndom);
    }
}

STATIC_OVL void
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
	(void) memset((genericptr_t)level.objects, 0, sizeof(level.objects));
	(void) memset((genericptr_t)level.monsters, 0, sizeof(level.monsters));
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
	    } else if (dungeons[u.uz.dnum].proto[0] && (rn2(2)) ) {
		    makemaz("");
		    return;
	    } else if (In_mines(&u.uz) && rn2(!(u.monstertimefinish % 247) ? (iswarper ? 2 : 20) : (iswarper ? 5 : 50) )) {
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
	    } else if (In_quest(&u.uz)) {
		    char        fillname[16];
		    s_level	*loc_lev;

		    Sprintf(fillname, "%s-loca", urole.filecode);
		    loc_lev = find_level(fillname);

		    Sprintf(fillname, "%s-fil", urole.filecode);
		    Strcat(fillname,
			   (u.uz.dlevel < loc_lev->dlevel.dlevel) ? "a" : "b");
		    makemaz(fillname);
		    return;
	    } else if( /*u.uz.dnum == wiz1_level.dnum || u.uz.dnum == (wiz1_level.dnum + 1) || u.uz.dnum == (wiz1_level.dnum + 2) ||*/ (In_V_tower(&u.uz)) || Invocation_lev(&u.uz) ||
		 (In_hell(&u.uz) && (!rn2(2) && (In_sheol(&u.uz) ? rn2(iswarper ? 2 : 5) : rn2(iswarper ? 3 : 10) ) )  ) || /* allowing random rooms-and-corridors in Gehennom --Amy */
		  (rn2(5) && u.uz.dnum == medusa_level.dnum
			  && depth(&u.uz) > depth(&medusa_level))) {
		    makemaz("");
		    return;
	    }
	}

	/* very random levels --Amy */

	if ( (In_dod(&u.uz) && (depth(&u.uz) > 1) && !rn2(!(u.monstertimefinish % 245) ? (iswarper ? 4 : 40) : (iswarper ? 10 : 100))) || (In_mines(&u.uz) /* check moved upwards */ ) || (In_sokoban(&u.uz) && rn2(!(u.monstertimefinish % 241) ? (iswarper ? 10 : 4) : (iswarper ? 5 : 2))) || (In_towndungeon(&u.uz) && !rn2(!(u.monstertimefinish % 243) ? (iswarper ? 2 : 10) : (iswarper ? 3 : 20))) ) {


	    switch (rnd(73)) {

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

		switch (rnd(49)) {

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

		switch (rnd(87)) {

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

		switch (rnd(18)) {

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

		}
		break;

	    case 30:
	    case 31:

		switch (rnd(3)) {

			case 1: makemaz("minend-1"); return;
			case 2: makemaz("minend-2"); return;
			case 3: makemaz("minend-3"); return;

		}
		break;

	    case 32:
	    case 33:

		switch (rnd(16)) {

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

		switch (rnd(152)) {

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

		}
		break;

	    case 44:
	    case 45:

		switch (rnd(14)) {

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

		}
		break;

	    case 46:

		makemaz("oracleX"); return;
		break;

	    case 47:

		switch (rnd(3)) {

			case 1: makemaz("erats-1"); return;
			case 2: makemaz("erats-2"); return;
			case 3: makemaz("erats-3"); return;

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

		switch (rnd(4)) {

			case 1: makemaz("forge"); return;
			case 2: makemaz("hitchE"); return;
			case 3: makemaz("compuE"); return;
			case 4: makemaz("keyE"); return;

		}
		break;

	    case 53:
	    case 54:

		switch (rnd(11)) {

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

		}
		break;

	    case 55:
	    case 56:

		switch (rnd(9)) {

			case 1: makemaz("ecastl-1"); return;
			case 2: makemaz("ecastl-2"); return;
			case 3: makemaz("ecastl-3"); return;
			case 4: makemaz("ecastl-4"); return;
			case 5: makemaz("ecastl-5"); return;
			case 6: makemaz("ecastl-6"); return;
			case 7: makemaz("ecastl-7"); return;
			case 8: makemaz("ecastl-8"); return;
			case 9: makemaz("ecastl-9"); return;

		}
		break;

	    case 57:
	    case 58:
	    case 59:
	    case 60:
	    case 61:

		switch (rnd(26)) {

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

		makemaz("eknox"); return;
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

		makemaz("ecav2"); return;
		break;

	    case 72:

		makemaz("efrnk"); return;
		break;

	    case 73:

		switch (rnd(4)) {

			case 1: makemaz("esheo-1"); return;
			case 2: makemaz("esheo-2"); return;
			case 3: makemaz("esheo-3"); return;
			case 4: makemaz("esheo-4"); return;

		}
		break;

          }

	}

	if ( (In_gehennom(&u.uz) && !rn2(!(u.monstertimefinish % 237) ? (iswarper ? 2 : 5) : (iswarper ? 3 : 10))) || (In_sheol(&u.uz) && (!(u.monstertimefinish % 235) ? (iswarper || !rn2(3)) : (!rn2(iswarper ? 2 : 5)) ) ) ) {

	    switch (rnd(73)) {

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

		switch (rnd(49)) {

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

		switch (rnd(87)) {

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

		switch (rnd(18)) {

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

		}
		break;

	    case 30:
	    case 31:

		switch (rnd(3)) {

			case 1: makemaz("hminen-1"); return;
			case 2: makemaz("hminen-2"); return;
			case 3: makemaz("hminen-3"); return;

		}
		break;

	    case 32:
	    case 33:

		switch (rnd(16)) {

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

		switch (rnd(152)) {

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

		}
		break;

	    case 44:
	    case 45:

		switch (rnd(9)) {

			case 1: makemaz("hellma-1"); return;
			case 2: makemaz("hellma-2"); return;
			case 3: makemaz("hellma-3"); return;
			case 4: makemaz("hellma-4"); return;
			case 5: makemaz("hellma-5"); return;
			case 6: makemaz("hellma-6"); return;
			case 7: makemaz("hellma-7"); return;
			case 8: makemaz("hellma-8"); return;
			case 9: makemaz("hellma-9"); return;

		}
		break;

	    case 46:

		makemaz("oracleX"); return;
		break;

	    case 47:

		switch (rnd(3)) {

			case 1: makemaz("hrats-1"); return;
			case 2: makemaz("hrats-2"); return;
			case 3: makemaz("hrats-3"); return;

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

		switch (rnd(4)) {

			case 1: makemaz("forgeX"); return;
			case 2: makemaz("hitchX"); return;
			case 3: makemaz("compuX"); return;
			case 4: makemaz("keyX"); return;

		}
		break;

	    case 53:
	    case 54:

		switch (rnd(11)) {

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

		}
		break;

	    case 55:
	    case 56:

		switch (rnd(9)) {

			case 1: makemaz("hcastl-1"); return;
			case 2: makemaz("hcastl-2"); return;
			case 3: makemaz("hcastl-3"); return;
			case 4: makemaz("hcastl-4"); return;
			case 5: makemaz("hcastl-5"); return;
			case 6: makemaz("hcastl-6"); return;
			case 7: makemaz("hcastl-7"); return;
			case 8: makemaz("hcastl-8"); return;
			case 9: makemaz("hcastl-9"); return;

		}
		break;

	    case 57:
	    case 58:
	    case 59:
	    case 60:
	    case 61:

		switch (rnd(26)) {

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

		makemaz("hknox"); return;
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

		makemaz("hcav2"); return;
		break;

	    case 72:

		makemaz("hfrnk"); return;
		break;

	    case 73:

		switch (rnd(4)) {

			case 1: makemaz("hsheo-1"); return;
			case 2: makemaz("hsheo-2"); return;
			case 3: makemaz("hsheo-3"); return;
			case 4: makemaz("hsheo-4"); return;

		}
		break;

          }

	}

	/* mazewalker only gets mazes (evil patch idea by jonadab) */

	if (ismazewalker && depth(&u.uz) > 1) {

	    makemaz("");
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

	/* construct stairs (up and down in different rooms if possible) */
	croom = &rooms[rn2(nroom)];
	if (!Is_botlevel(&u.uz))
	     mkstairs(somex(croom), somey(croom), 0, croom);	/* down */
	if (nroom > 1) {
	    troom = croom;
	    croom = &rooms[rn2(nroom-1)];
	    if (croom == troom) croom++;
	}

	if (u.uz.dlevel != 1) {
	    xchar sx, sy;
	    do {
		sx = somex(croom);
		sy = somey(croom);
	    } while(occupied(sx, sy));
	    mkstairs(sx, sy, 1, croom);	/* up */
	}

	branchp = Is_branchlev(&u.uz);	/* possible dungeon branch */
	room_threshold = branchp ? 4 : 3; /* minimum number of rooms needed
					     to allow a random special room */
#ifdef REINCARNATION
	if (Is_rogue_level(&u.uz)) goto skip0;
#endif
	makecorridors();
	make_niches();

	if (!rn2(5)) make_ironbarwalls(rn2(20) ? rn2(20) : rn2(50));

	/* make a secret treasure vault, not connected to the rest */
	if(do_vault()) {
		xchar w,h;
#ifdef DEBUG
		debugpline("trying to make a vault...");
#endif
		w = 1;
		h = 1;
		if (check_room(&vault_x, &w, &vault_y, &h, TRUE)) {
		    fill_vault:
			add_room(vault_x, vault_y, vault_x+w,
				 vault_y+h, TRUE, VAULT, FALSE, FALSE);
			level.flags.has_vault = 1;
			++room_threshold;
			fill_room(&rooms[nroom - 1], FALSE);
			mk_knox_portal(vault_x+w, vault_y+h);
			if(!level.flags.noteleport && !rn2(3)) makevtele();
		} else if(rnd_rect() && create_vault()) {
			vault_x = rooms[nroom].lx;
			vault_y = rooms[nroom].ly;
			if (check_room(&vault_x, &w, &vault_y, &h, TRUE))
				goto fill_vault;
			else
				rooms[nroom].hx = -1;
		}
	}

    {
	register int u_depth = depth(&u.uz);

#ifdef WIZARD
	if(wizard && nh_getenv("SHOPTYPE")) mkroom(SHOPBASE); else
#endif
	if (u_depth > 1 /*&&
	    u_depth < depth(&medusa_level)*/ &&
	    nroom >= room_threshold &&
	    rn2(u_depth) < 3) mkroom(SHOPBASE);
 
	/* [Tom] totally reorganized this into categories... used
	   to be only one special room on a level... now allows
	   one of each major type */
	/*else {*/


	    /* courtrooms & barracks */
	    if(depth(&u.uz) > 4 && (ishaxor ? !rn2(6) : !rn2(12))) mkroom(COURT);
		else if (depth(&u.uz) > 5 && (ishaxor ? !rn2(4) : !rn2(8))) mkroom(LEPREHALL);
	    else if(depth(&u.uz) > 14 && (ishaxor ? !rn2(8) : !rn2(16))) mkroom(GIANTCOURT);
	    else if(depth(&u.uz) > 10 && (ishaxor ? !rn2(5) : !rn2(10))) mkroom(BARRACKS);
	else if (u_depth > 3 && (ishaxor ? !rn2(8) : !rn2(16))) mkroom(HUMANHALL);
	else if (u_depth > 1 && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(TRAPROOM);
	else if (u_depth > 1 && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(TENSHALL);
	else if (u_depth > 1 && ( (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(INSIDEROOM);
	else if (u_depth > 1 && ( (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(RIVERROOM);

	    /* hives */
	    if(depth(&u.uz) > 5 && (ishaxor ? !rn2(6) : !rn2(12))) mkroom(BEEHIVE);
	else if (u_depth > 8 && (ishaxor ? !rn2(4) : !rn2(8))) mkroom(ANTHOLE);
	else if (u_depth > 3 && (ishaxor ? !rn2(6) : !rn2(12))) mkroom(NYMPHHALL);
	else if (u_depth > 2 && (ishaxor ? !rn2(7) : !rn2(13))) mkroom(COINHALL);
	else if (u_depth > 7 && (ishaxor ? !rn2(8) : !rn2(16))) mkroom(ARMORY);
	else if (u_depth > 1 && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(POOLROOM);
	else if (u_depth > 1 && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(TENSHALL);
	else if (u_depth > 1 && ( (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(INSIDEROOM);
	else if (u_depth > 1 && ( (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(RIVERROOM);


	    /* zoos */
	    if(depth(&u.uz) > 5 && (ishaxor ? !rn2(6) : !rn2(12))) mkroom(ZOO);
	    /* fungus farms are rare... */
	    else if (u_depth > 3 && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(FUNGUSFARM);
	    else if(depth(&u.uz) > 9 && (ishaxor ? !rn2(9) : !rn2(18))) mkroom(REALZOO);
	    else if(depth(&u.uz) > 9 && (ishaxor ? !rn2(10) : !rn2(20))) mkroom(ELEMHALL);
	    else if(depth(&u.uz) > 12 && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(ANGELHALL);
	    else if(depth(&u.uz) > 2 && (ishaxor ? !rn2(9) : !rn2(17))) mkroom(MIMICHALL);
	else if (u_depth > 3 && (ishaxor ? !rn2(8) : !rn2(15))) mkroom(SPIDERHALL);
	else if (u_depth > 1 && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(STATUEROOM);
	else if (u_depth > 1 && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(TENSHALL);
	else if (u_depth > 1 && ( (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(INSIDEROOM);
	else if (u_depth > 1 && ( (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(RIVERROOM);

	    /* neat rooms */
	    if(depth(&u.uz) > 1 && (ishaxor ? !rn2(7) : !rn2(13))) mkroom(TEMPLE);
	    else if(depth(&u.uz) > 4 && (ishaxor ? !rn2(7) : !rn2(14))) mkroom(MORGUE);
	    else if(depth(&u.uz) > 1 && (ishaxor ? !rn2(8) : !rn2(15))) mkroom(BADFOODSHOP);
	    else if(depth(&u.uz) > 1 && (ishaxor ? !rn2(4) : !rn2(7))) mkroom(SWAMP);
        else if (depth(&u.uz) > 10 && (ishaxor ? !rn2(20) : !rn2(40))) mkroom(CLINIC); /*supposed to be very rare --Amy*/
        else if (depth(&u.uz) > 3 && (ishaxor ? !rn2(10) : !rn2(20))) mkroom(TERRORHALL);
        else if (depth(&u.uz) > 10 && (ishaxor ? !rn2(12) : !rn2(24))) mkroom(TROLLHALL);
 	else if(depth(&u.uz) > 10 && (ishaxor ? !rn2(8) : !rn2(16))) mkroom(DOUGROOM);
	else if (u_depth > 1 && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(TENSHALL);
	else if (u_depth > 1 && ( (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(INSIDEROOM);
	else if (u_depth > 1 && ( (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(RIVERROOM);

	    /* dangerous ones */
	    if (u_depth > 10 && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(COCKNEST);
	    else if(depth(&u.uz) > 2 && (ishaxor ? !rn2(12) : !rn2(24))) mkroom(DRAGONLAIR);
	    else if (u_depth > 12 && (ishaxor ? !rn2(15) : !rn2(30))) mkroom(MIGOHIVE);
	    /* [DS] the restriction of lemure pits to Gehennom means they're
	     *      never going to show up randomly (no random room+corridor
	     *      levels in Gehennom). Perhaps this should be removed? */
	    else if (depth(&u.uz) > 5 && (ishaxor ? !rn2(11) : !rn2(22))) mkroom(LEMUREPIT);
	else if (u_depth > 3 && (ishaxor ? !rn2(12) : !rn2(24))) mkroom(GOLEMHALL);
	else if (u_depth > 1 && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(GRUEROOM);
	else if (u_depth > 1 && (ishaxor ? !rn2(13) : !rn2(25))) mkroom(TENSHALL);
	else if (u_depth > 1 && ( (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(INSIDEROOM);
	else if (u_depth > 1 && ( (ishaxor && Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(25) : (ishaxor || Role_if(PM_SPACEWARS_FIGHTER)) ? !rn2(50) : !rn2(100))) mkroom(RIVERROOM);

		/* random rooms, which means a chance of getting several of the same type of room --Amy */
		while ((u_depth > 10 || (rn2(u_depth) && !rn2(20 - u_depth) ) ) && !rn2(ishaxor ? 7 : 15)) mkroom(RANDOMROOM);

		if (u_depth > 1 && !((moves + u.monstertimefinish) % (ishaxor ? 437 : 837) )) {

			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);

		}

		if (u_depth > 1 && !((moves + u.monstertimefinish) % (ishaxor ? 1637 : 3237) )) {

			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);

		}

		if (u_depth > 1 && !((moves + u.monstertimefinish) % (ishaxor ? 6437 : 12837) )) {

			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);

		}

		if (u_depth > 1 && !((moves + u.monstertimefinish) % (ishaxor ? 25637 : 51237) )) {

			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);
			mkroom(RANDOMROOM);

		}

		if (u_depth > 1 && !((moves + u.monstertimefinish) % (ishaxor ? 439 : 839) )) {

			randrmtyp = findrandtype();

			mkroom(randrmtyp);
			mkroom(randrmtyp);

		}

		if (u_depth > 1 && !((moves + u.monstertimefinish) % (ishaxor ? 1639 : 3239) )) {

			randrmtyp = findrandtype();

			mkroom(randrmtyp);
			mkroom(randrmtyp);
			mkroom(randrmtyp);

		}

		if (u_depth > 1 && !((moves + u.monstertimefinish) % (ishaxor ? 6439 : 12839) )) {

			randrmtyp = findrandtype();

			mkroom(randrmtyp);
			mkroom(randrmtyp);
			mkroom(randrmtyp);
			mkroom(randrmtyp);

		}

		if (u_depth > 1 && !((moves + u.monstertimefinish) % (ishaxor ? 25639 : 51239) )) {

			randrmtyp = findrandtype();

			mkroom(randrmtyp);
			mkroom(randrmtyp);
			mkroom(randrmtyp);
			mkroom(randrmtyp);
			mkroom(randrmtyp);

		}

	    /* Underground rivers */
	    if ( u_depth > 13 && !rn2(7)) mkrivers();
	    if ( u_depth <= 13 && !rn2(15) && rn2(u_depth) ) mkrivers();

		if (ishaxor) {
	    if ( u_depth > 13 && !rn2(7)) mkrivers();
	    if ( u_depth <= 13 && !rn2(15) && rn2(u_depth) ) mkrivers();
		}

		if (isaquarian && u_depth > 1) mkrivers();

	    if ( u_depth > 13 && !rn2(7)) mkrandrivers();
	    if ( u_depth <= 13 && !rn2(15) && rn2(u_depth) ) mkrandrivers();

		if (ishaxor) {
	    if ( u_depth > 13 && !rn2(7)) mkrandrivers();
	    if ( u_depth <= 13 && !rn2(15) && rn2(u_depth) ) mkrandrivers();
		}

		if (isaquarian && u_depth > 1) mkrandrivers();

	/*}*/

#ifdef REINCARNATION
skip0:
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
		if(u.uhave.amulet || !rn2(3)) {
		    x = somex(croom); y = somey(croom);
		    if (!ishomicider) { tmonst = makemon((struct permonst *) 0, x,y,NO_MM_FLAGS);
		    if (tmonst && webmaker(tmonst->data) /*== &mons[PM_GIANT_SPIDER]*/ &&
			    !occupied(x, y))
			(void) maketrap(x, y, WEB);
		    }
		    if (ishomicider) (void) makerandomtrap_at(x, y);
		}
		if(ishaxor && (u.uhave.amulet || !rn2(3)) ) {
		    x = somex(croom); y = somey(croom);
		    if (!ishomicider) { tmonst = makemon((struct permonst *) 0, x,y,NO_MM_FLAGS);
		    if (tmonst && webmaker(tmonst->data) /*== &mons[PM_GIANT_SPIDER]*/ &&
			    !occupied(x, y))
			(void) maketrap(x, y, WEB);
		    }
		    if (ishomicider) (void) makerandomtrap_at(x, y);
		}
		/* put traps and mimics inside */
		goldseen = FALSE;
		x = 7 - (level_difficulty()/5);
		if (x <= 1) x = 2;
		while (!rn2(x))
		    mktrap(0,0,croom,(coord*)0);

		if(ishaxor) {while (!rn2(x))
		    mktrap(0,0,croom,(coord*)0); }

		if (!goldseen && !rn2(3))
		    (void) mkgold(0L, somex(croom), somey(croom));
#ifdef REINCARNATION
		x = 80 - (depth(&u.uz) * 2);
		if (x < 2) x = 2;
		if(!rn2(x)) mkgrave(croom);

		if(ishaxor && !rn2(x)) mkgrave(croom);

		if(Is_rogue_level(&u.uz)) goto skip_nonrogue;
#endif
		if(!rn2(10)) mkfount(0,croom);
		if(ishaxor && !rn2(10)) mkfount(0,croom);

		if(!rn2(500)) mkthrone(croom); /* rare cause they may give wishes --Amy */
		if(ishaxor && !rn2(500)) mkthrone(croom); /* rare cause they may give wishes --Amy */

#ifdef SINKS
		if(!rn2(60)) {
		    mksink(croom);
		    if(!rn2(3)) mktoilet(croom);
		}

		if(ishaxor && !rn2(60)) {
		    mksink(croom);
		    if(!rn2(3)) mktoilet(croom);
		}
#endif
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
		    (void) mksobj_at((rn2(3)) ? LARGE_BOX : CHEST,
				     somex(croom), somey(croom), TRUE, FALSE);

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
			    make_engr_at(x, y, mesg, 0L, MARK);
		    }
		}

#ifdef REINCARNATION
	skip_nonrogue:
#endif

/* STEPHEN WHITE'S NEW CODE */
		if(!rn2(4)) {
		    (void) mkobj_at(0, somex(croom), somey(croom), TRUE);
		    tryct = 0;
		    while(!rn2(3)) {
			if(++tryct > 100) {
			    impossible("tryct overflow4");
			    break;
			}
			(void) mkobj_at(0, somex(croom), somey(croom), TRUE);
		    }
		}

		if(ishaxor && !rn2(4)) {
		    (void) mkobj_at(0, somex(croom), somey(croom), TRUE);
		    tryct = 0;
		    while(!rn2(3)) {
			if(++tryct > 100) {
			    impossible("tryct overflow4");
			    break;
			}
			(void) mkobj_at(0, somex(croom), somey(croom), TRUE);
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
	int goldprob, gemprob, objprob, x, y, cnt;

	int density = 3;
	if (!(u.monstertimefinish % 477)) density += rnd(5);
	if (!(u.monstertimefinish % 1877)) density += rnd(10);
	if (!rn2(5)) density += 1;
	if (!rn2(10)) density += rnd(3);
	if (!rn2(25)) density += rnd(5);
	if (!rn2(125)) density += rnd(10);
	if (!rn2(750)) density += rnd(20);

	register struct obj *otmpX;
	register int tryct = 0;

	int otherwalltype = STONE;
	boolean otherwallxtra = 0;
	if (!rn2(!(u.monstertimefinish % 1777) ? (iswarper ? 3 : 20) : !(u.monstertimefinish % 277) ? (iswarper ? 10 : 50) : (iswarper ? 100 : 500) )) {

		switch (rnd(7)) {

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
		}

		if (!rn2(50)) otherwallxtra = 1;

	}

	/* Place kelp, except on the plane of water */
	if (In_endgame(&u.uz)) return;
	for (x = 1; x < (COLNO); x++)
	    for (y = 0; y < (ROWNO); y++) {

		if ((levl[x][y].typ == STONE) && otherwalltype && (rnd(density) < 4) ) {

			if (otherwallxtra) {
				switch (rnd(8)) {

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
						otherwalltype = CROSSWALL; break;
				}
			}

			levl[x][y].typ = otherwalltype;

			if (!rn2(ishaxor ? 10000 : 20000))
				levl[x][y].typ = THRONE;
			else if (!((moves + u.monstertimefinish) % 897 ) && !rn2(ishaxor ? 1000 : 2000))
				levl[x][y].typ = THRONE;
			else if (!rn2(ishaxor ? 2500 : 5000)) {
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				}
			else if (!((moves + u.monstertimefinish) % 899 ) && !rn2(ishaxor ? 250 : 500)) {
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				}
			else if (!rn2(ishaxor ? 2500 : 5000)) {
				levl[x][y].typ = SINK;
				level.flags.nsinks++;
				}
			else if (!((moves + u.monstertimefinish) % 901 ) && !rn2(ishaxor ? 250 : 500)) {
				levl[x][y].typ = SINK;
				level.flags.nsinks++;
				}
			else if (!rn2(ishaxor ? 5000 : 10000))
				levl[x][y].typ = TOILET;
			else if (!((moves + u.monstertimefinish) % 903 ) && !rn2(ishaxor ? 500 : 1000))
				levl[x][y].typ = TOILET;
			else if (!rn2(ishaxor ? 1000 : 2000)) {
				levl[x][y].typ = GRAVE;
		
				if (!rn2(3)) (void) mkgold(0L, x, y);
				for (tryct = rn2(5); tryct; tryct--) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = x;
					    otmpX->oy = y;
					    add_to_buried(otmpX);
					}
				}
			else if (!((moves + u.monstertimefinish) % 905 ) && !rn2(ishaxor ? 100 : 200)) {
				levl[x][y].typ = GRAVE;
		
				if (!rn2(3)) (void) mkgold(0L, x, y);
				for (tryct = rn2(5); tryct; tryct--) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = x;
					    otmpX->oy = y;
					    add_to_buried(otmpX);
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
			else if (!((moves + u.monstertimefinish) % 907 ) && !rn2(ishaxor ? 1000 : 2000)) {
				levl[x][y].typ = ALTAR;
				if (rn2(10)) levl[x][y].altarmask = Align2amask( A_NONE );
				else switch (rnd(3)) {
		
				case 1: levl[x][y].altarmask = Align2amask( A_LAWFUL ); break;
				case 2: levl[x][y].altarmask = Align2amask( A_NEUTRAL ); break;
				case 3: levl[x][y].altarmask = Align2amask( A_CHAOTIC ); break;
		
				}
			}

		}

		if ((levl[x][y].typ == POOL && !rn2(10)) ||
			(levl[x][y].typ == MOAT && !rn2(30)))
	    	    (void)mksobj_at(KELP_FROND, x, y, TRUE, FALSE);

		if (!ishomicider) {

		/* Random sea monsters if there is water. --Amy */

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 10 : (issuxxor && !ishaxor) ? 40 : 20)) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 10 : (issuxxor && !ishaxor) ? 40 : 20)))
	    	    makemon(mkclass(S_EEL,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000) && level_difficulty() > 4 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000) && level_difficulty() > 4 ))
	    	    makemon(&mons[PM_HUMAN_WEREPIRANHA], x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000) && level_difficulty() > 9 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000) && level_difficulty() > 9 ))
	    	    makemon(&mons[PM_HUMAN_WEREEEL], x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000) && level_difficulty() > 19 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000) && level_difficulty() > 19 ))
	    	    makemon(&mons[PM_HUMAN_WEREKRAKEN], x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) ))
	    	    makemon(&mons[PM_SUBMARINE_GOBLIN], x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 17 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 17 ))
	    	    makemon(&mons[PM_PUNT], x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 10 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 10 ))
	    	    makemon(&mons[PM_SWIMMER_TROLL], x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 20 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 20 ))
	    	    makemon(&mons[PM_DIVER_TROLL], x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 12 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 12 ))
	    	    makemon(&mons[PM_WATER_TURRET], x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 25 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 25 ))
	    	    makemon(&mons[PM_AQUA_TURRET], x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 15 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 15 ))
	    	    makemon(&mons[PM_LUXURY_YACHT], x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 15 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 15 ))
	    	    makemon(&mons[PM_MISTER_SUBMARINE], x, y, MM_ADJACENTOK);

		/* More random monsters on other terrain, too. --Amy */

		if ((levl[x][y].typ == LAVAPOOL && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_FLYFISH,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == LAVAPOOL && !rn2((ishaxor && !issuxxor) ? 2000 : (issuxxor && !ishaxor) ? 8000 : 4000) && level_difficulty() > 23 ) )
	    	    makemon(&mons[PM_HUMAN_WEREFLYFISH], x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == LAVAPOOL && !rn2((ishaxor && !issuxxor) ? 5000 : (issuxxor && !ishaxor) ? 20000 : 10000) && level_difficulty() > 7 ) )
	    	    makemon(&mons[PM_CONCORDE__], x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == ROOM && !rn2( ((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000) / level_difficulty() )) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == CORR && !rn2( ((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000) / level_difficulty() )) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == ICE && !rn2( ((ishaxor && !issuxxor) ? 250 : (issuxxor && !ishaxor) ? 1000 : 500) / level_difficulty() )) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == AIR && !rn2( ((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000) / level_difficulty() )) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == CLOUD && !rn2( ((ishaxor && !issuxxor) ? 200 : (issuxxor && !ishaxor) ? 800 : 400) / level_difficulty() )) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == CORR && !rn2((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000)) )
			makemon(mkclass(S_WALLMONST,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == CORR && !rn2((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000 )) )
			makemon(mkclass(S_TURRET,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == TREE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
			makemon(mkclass(S_BAT,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_SNAKE,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_LEPRECHAUN,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_NYMPH,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_SPIDER,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_RUBMONST,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 10 : (issuxxor && !ishaxor) ? 40 : 20)) )
	    	    makemon(courtmon(), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_VORTEX,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_LIGHT,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_TRAPPER,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_ANGEL,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_ELEMENTAL,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_HUMAN,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makemon(mkclass(S_NEMESE,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 5000 : (issuxxor && !ishaxor) ? 20000 : 10000)) )
	    	    makemon(mkclass(S_ARCHFIEND,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_PUDDING,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_BLOB,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_JELLY,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_FUNGUS,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_GRUE,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_LIZARD,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_MIMIC,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_PIERCER,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_RODENT,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makemon(mkclass(S_WORM,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == GRAVE && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makemon(morguemonX(), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == ALTAR && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 10 : 5)) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == STONE && !rn2((ishaxor && !issuxxor) ? 5000 : (issuxxor && !ishaxor) ? 20000 : 10000)) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if (( (levl[x][y].typ == VWALL || levl[x][y].typ == HWALL || levl[x][y].typ == TLCORNER || levl[x][y].typ == TRCORNER || levl[x][y].typ == BLCORNER || levl[x][y].typ == BRCORNER || levl[x][y].typ == CROSSWALL || levl[x][y].typ == TUWALL || levl[x][y].typ == TDWALL || levl[x][y].typ == TRWALL || levl[x][y].typ == TLWALL || levl[x][y].typ == DBWALL ) && !rn2((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000)) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == SDOOR && !rn2((ishaxor && !issuxxor) ? 100 : (issuxxor && !ishaxor) ? 400 : 200)) )
			makemon(mkclass(S_MIMIC,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == SCORR && !rn2((ishaxor && !issuxxor) ? 200 : (issuxxor && !ishaxor) ? 800 : 400)) )
			makemon(mkclass(S_MIMIC,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == DOOR && !rn2((ishaxor && !issuxxor) ? 300 : (issuxxor && !ishaxor) ? 1200 : 600)) )
			makemon(mkclass(S_MIMIC,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == POOL && !((moves + u.monstertimefinish) % 4339 ) && !rn2((ishaxor && !issuxxor) ? 2 : (issuxxor && !ishaxor) ? 8 : 4)) ||
			(levl[x][y].typ == MOAT && !((moves + u.monstertimefinish) % 4339 ) && !rn2((ishaxor && !issuxxor) ? 2 : (issuxxor && !ishaxor) ? 8 : 4)) )
	    	    makemon(mkclass(S_EEL,0), x, y, MM_ADJACENTOK);

		/* More random monsters on other terrain, too. --Amy */

		if ((levl[x][y].typ == LAVAPOOL && !((moves + u.monstertimefinish) % 4341 ) && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makemon(mkclass(S_FLYFISH,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == ROOM && !((moves + u.monstertimefinish) % 4343 ) && !rn2( ((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100) / level_difficulty() )) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == CORR && !((moves + u.monstertimefinish) % 4345 ) && !rn2( ((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100) / level_difficulty() )) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == ICE && !((moves + u.monstertimefinish) % 4347 ) && !rn2( ((ishaxor && !issuxxor) ? 25 : (issuxxor && !ishaxor) ? 100 : 50) / level_difficulty() )) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == AIR && !((moves + u.monstertimefinish) % 4349 ) && !rn2( ((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60) / level_difficulty() )) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == CLOUD && !((moves + u.monstertimefinish) % 4351 ) && !rn2( ((ishaxor && !issuxxor) ? 20 : (issuxxor && !ishaxor) ? 80 : 40) / level_difficulty() )) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == CORR && !((moves + u.monstertimefinish) % 4353 ) && !rn2((ishaxor && !issuxxor) ? 100 : (issuxxor && !ishaxor) ? 400 : 200)) )
			makemon(mkclass(S_WALLMONST,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == CORR && !((moves + u.monstertimefinish) % 4355 )  && !rn2((ishaxor && !issuxxor) ? 100 : (issuxxor && !ishaxor) ? 400 : 200 )) )
			makemon(mkclass(S_TURRET,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == TREE && !((moves + u.monstertimefinish) % 4357 ) && !rn2((ishaxor && !issuxxor) ? 4 : (issuxxor && !ishaxor) ? 16 : 8)) )
			makemon(mkclass(S_BAT,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == FOUNTAIN && !((moves + u.monstertimefinish) % 4359 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makemon(mkclass(S_SNAKE,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == FOUNTAIN && !((moves + u.monstertimefinish) % 4361 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makemon(mkclass(S_LEPRECHAUN,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == FOUNTAIN && !((moves + u.monstertimefinish) % 4363 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makemon(mkclass(S_NYMPH,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == FOUNTAIN && !((moves + u.monstertimefinish) % 4365 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makemon(mkclass(S_SPIDER,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == FOUNTAIN && !((moves + u.monstertimefinish) % 4367 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makemon(mkclass(S_RUBMONST,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4369 ) && !rn2((ishaxor && !issuxxor) ? 1 : (issuxxor && !ishaxor) ? 4 : 2)) )
	    	    makemon(courtmon(), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4371 ) && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makemon(mkclass(S_VORTEX,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4373 ) && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makemon(mkclass(S_LIGHT,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4375 ) && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makemon(mkclass(S_TRAPPER,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4377 ) && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makemon(mkclass(S_ANGEL,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4379 ) && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makemon(mkclass(S_ELEMENTAL,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4381 ) && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makemon(mkclass(S_HUMAN,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4383 ) && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makemon(mkclass(S_NEMESE,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4385 ) && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makemon(mkclass(S_ARCHFIEND,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == SINK && !((moves + u.monstertimefinish) % 4387 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makemon(mkclass(S_PUDDING,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == SINK && !((moves + u.monstertimefinish) % 4389 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makemon(mkclass(S_BLOB,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == SINK && !((moves + u.monstertimefinish) % 4391 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makemon(mkclass(S_JELLY,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == SINK && !((moves + u.monstertimefinish) % 4393 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makemon(mkclass(S_FUNGUS,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == SINK && !((moves + u.monstertimefinish) % 4395 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makemon(mkclass(S_GRUE,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == TOILET && !((moves + u.monstertimefinish) % 4397 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makemon(mkclass(S_LIZARD,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == TOILET && !((moves + u.monstertimefinish) % 4399 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makemon(mkclass(S_MIMIC,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == TOILET && !((moves + u.monstertimefinish) % 4401 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makemon(mkclass(S_PIERCER,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == TOILET && !((moves + u.monstertimefinish) % 4403 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makemon(mkclass(S_RODENT,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == TOILET && !((moves + u.monstertimefinish) % 4405 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makemon(mkclass(S_WORM,0), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == GRAVE && !((moves + u.monstertimefinish) % 4407 ) && !rn2((ishaxor && !issuxxor) ? 1 : (issuxxor && !ishaxor) ? 3 : 2)) )
	    	    makemon(morguemonX(), x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == ALTAR && !((moves + u.monstertimefinish) % 4409 ) && !rn2((ishaxor && !issuxxor) ? 1 : (issuxxor && !ishaxor) ? 3 : 2)) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == STONE && !((moves + u.monstertimefinish) % 4411 ) && !rn2((ishaxor && !issuxxor) ? 250 : (issuxxor && !ishaxor) ? 1000 : 500)) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if (( (levl[x][y].typ == VWALL || levl[x][y].typ == HWALL || levl[x][y].typ == TLCORNER || levl[x][y].typ == TRCORNER || levl[x][y].typ == BLCORNER || levl[x][y].typ == BRCORNER || levl[x][y].typ == CROSSWALL || levl[x][y].typ == TUWALL || levl[x][y].typ == TDWALL || levl[x][y].typ == TRWALL || levl[x][y].typ == TLWALL || levl[x][y].typ == DBWALL ) && !((moves + u.monstertimefinish) % 4413 ) && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
			makemon((struct permonst *)0, x, y, MM_ADJACENTOK);

		if ((levl[x][y].typ == SDOOR && !((moves + u.monstertimefinish) % 4415 ) && !rn2((ishaxor && !issuxxor) ? 10 : (issuxxor && !ishaxor) ? 40 : 20)) )
			makemon(mkclass(S_MIMIC,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == SCORR && !((moves + u.monstertimefinish) % 4417 ) && !rn2((ishaxor && !issuxxor) ? 20 : (issuxxor && !ishaxor) ? 80 : 40)) )
			makemon(mkclass(S_MIMIC,0), x, y, MM_ADJACENTOK);
		if ((levl[x][y].typ == DOOR && !((moves + u.monstertimefinish) % 4419 ) && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
			makemon(mkclass(S_MIMIC,0), x, y, MM_ADJACENTOK);


		}

		if (ishomicider) {	/* idea by deepy - a race for which monsters don't spawn normally */

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 10 : (issuxxor && !ishaxor) ? 40 : 20)) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 10 : (issuxxor && !ishaxor) ? 40 : 20)))
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000) && level_difficulty() > 4 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000) && level_difficulty() > 4 ))
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000) && level_difficulty() > 9 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000) && level_difficulty() > 9 ))
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000) && level_difficulty() > 19 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000) && level_difficulty() > 19 ))
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) ))
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 17 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 17 ))
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 10 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 10 ))
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 20 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 20 ))
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 12 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 12 ))
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 25 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 25 ))
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 15 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 15 ))
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == POOL && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 15 ) ||
			(levl[x][y].typ == MOAT && !rn2((ishaxor && !issuxxor) ? 8000 : (issuxxor && !ishaxor) ? 32000 : 16000) && level_difficulty() > 15 ))
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == LAVAPOOL && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == LAVAPOOL && !rn2((ishaxor && !issuxxor) ? 2000 : (issuxxor && !ishaxor) ? 8000 : 4000) && level_difficulty() > 23 ) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == LAVAPOOL && !rn2((ishaxor && !issuxxor) ? 5000 : (issuxxor && !ishaxor) ? 20000 : 10000) && level_difficulty() > 7 ) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == ROOM && !rn2( ((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000) / level_difficulty() )) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == CORR && !rn2( ((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000) / level_difficulty() )) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == ICE && !rn2( ((ishaxor && !issuxxor) ? 250 : (issuxxor && !ishaxor) ? 1000 : 500) / level_difficulty() )) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == AIR && !rn2( ((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000) / level_difficulty() )) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == CLOUD && !rn2( ((ishaxor && !issuxxor) ? 200 : (issuxxor && !ishaxor) ? 800 : 400) / level_difficulty() )) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == CORR && !rn2((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000)) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == CORR && !rn2((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000 )) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == TREE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == FOUNTAIN && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 10 : (issuxxor && !ishaxor) ? 40 : 20)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !rn2((ishaxor && !issuxxor) ? 5000 : (issuxxor && !ishaxor) ? 20000 : 10000)) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == SINK && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == TOILET && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == GRAVE && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == ALTAR && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 10 : 5)) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == STONE && !rn2((ishaxor && !issuxxor) ? 5000 : (issuxxor && !ishaxor) ? 20000 : 10000)) )
			makerandomtrap_at(x, y);

		if (( (levl[x][y].typ == VWALL || levl[x][y].typ == HWALL || levl[x][y].typ == TLCORNER || levl[x][y].typ == TRCORNER || levl[x][y].typ == BLCORNER || levl[x][y].typ == BRCORNER || levl[x][y].typ == CROSSWALL || levl[x][y].typ == TUWALL || levl[x][y].typ == TDWALL || levl[x][y].typ == TRWALL || levl[x][y].typ == TLWALL || levl[x][y].typ == DBWALL ) && !rn2((ishaxor && !issuxxor) ? 1000 : (issuxxor && !ishaxor) ? 4000 : 2000)) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == SDOOR && !rn2((ishaxor && !issuxxor) ? 100 : (issuxxor && !ishaxor) ? 400 : 200)) )
			makerandomtrap_at(x, y);
		if ((levl[x][y].typ == SCORR && !rn2((ishaxor && !issuxxor) ? 200 : (issuxxor && !ishaxor) ? 800 : 400)) )
			makerandomtrap_at(x, y);
		if ((levl[x][y].typ == DOOR && !rn2((ishaxor && !issuxxor) ? 300 : (issuxxor && !ishaxor) ? 1200 : 600)) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == POOL && !((moves + u.monstertimefinish) % 4339 ) && !rn2((ishaxor && !issuxxor) ? 2 : (issuxxor && !ishaxor) ? 8 : 4)) ||
			(levl[x][y].typ == MOAT && !((moves + u.monstertimefinish) % 4339 ) && !rn2((ishaxor && !issuxxor) ? 2 : (issuxxor && !ishaxor) ? 8 : 4)) )
	    	    makerandomtrap_at(x, y);

		/* More random monsters on other terrain, too. --Amy */

		if ((levl[x][y].typ == LAVAPOOL && !((moves + u.monstertimefinish) % 4341 ) && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == ROOM && !((moves + u.monstertimefinish) % 4343 ) && !rn2( ((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100) / level_difficulty() )) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == CORR && !((moves + u.monstertimefinish) % 4345 ) && !rn2( ((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100) / level_difficulty() )) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == ICE && !((moves + u.monstertimefinish) % 4347 ) && !rn2( ((ishaxor && !issuxxor) ? 25 : (issuxxor && !ishaxor) ? 100 : 50) / level_difficulty() )) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == AIR && !((moves + u.monstertimefinish) % 4349 ) && !rn2( ((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60) / level_difficulty() )) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == CLOUD && !((moves + u.monstertimefinish) % 4351 ) && !rn2( ((ishaxor && !issuxxor) ? 20 : (issuxxor && !ishaxor) ? 80 : 40) / level_difficulty() )) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == CORR && !((moves + u.monstertimefinish) % 4353 ) && !rn2((ishaxor && !issuxxor) ? 100 : (issuxxor && !ishaxor) ? 400 : 200)) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == CORR && !((moves + u.monstertimefinish) % 4355 )  && !rn2((ishaxor && !issuxxor) ? 100 : (issuxxor && !ishaxor) ? 400 : 200 )) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == TREE && !((moves + u.monstertimefinish) % 4357 ) && !rn2((ishaxor && !issuxxor) ? 4 : (issuxxor && !ishaxor) ? 16 : 8)) )
			makerandomtrap_at(x, y);

		if ((levl[x][y].typ == FOUNTAIN && !((moves + u.monstertimefinish) % 4359 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == FOUNTAIN && !((moves + u.monstertimefinish) % 4361 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == FOUNTAIN && !((moves + u.monstertimefinish) % 4363 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == FOUNTAIN && !((moves + u.monstertimefinish) % 4365 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == FOUNTAIN && !((moves + u.monstertimefinish) % 4367 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4369 ) && !rn2((ishaxor && !issuxxor) ? 1 : (issuxxor && !ishaxor) ? 4 : 2)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4371 ) && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4373 ) && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4375 ) && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4377 ) && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4379 ) && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4381 ) && !rn2((ishaxor && !issuxxor) ? 5 : (issuxxor && !ishaxor) ? 20 : 10)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4383 ) && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == THRONE && !((moves + u.monstertimefinish) % 4385 ) && !rn2((ishaxor && !issuxxor) ? 500 : (issuxxor && !ishaxor) ? 2000 : 1000)) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == SINK && !((moves + u.monstertimefinish) % 4387 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == SINK && !((moves + u.monstertimefinish) % 4389 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == SINK && !((moves + u.monstertimefinish) % 4391 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == SINK && !((moves + u.monstertimefinish) % 4393 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == SINK && !((moves + u.monstertimefinish) % 4395 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == TOILET && !((moves + u.monstertimefinish) % 4397 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == TOILET && !((moves + u.monstertimefinish) % 4399 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == TOILET && !((moves + u.monstertimefinish) % 4401 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == TOILET && !((moves + u.monstertimefinish) % 4403 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == TOILET && !((moves + u.monstertimefinish) % 4405 ) && !rn2((ishaxor && !issuxxor) ? 3 : (issuxxor && !ishaxor) ? 12 : 6)) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == GRAVE && !((moves + u.monstertimefinish) % 4407 ) && !rn2((ishaxor && !issuxxor) ? 1 : (issuxxor && !ishaxor) ? 3 : 2)) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == ALTAR && !((moves + u.monstertimefinish) % 4409 ) && !rn2((ishaxor && !issuxxor) ? 1 : (issuxxor && !ishaxor) ? 3 : 2)) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == STONE && !((moves + u.monstertimefinish) % 4411 ) && !rn2((ishaxor && !issuxxor) ? 250 : (issuxxor && !ishaxor) ? 1000 : 500)) )
	    	    makerandomtrap_at(x, y);

		if (( (levl[x][y].typ == VWALL || levl[x][y].typ == HWALL || levl[x][y].typ == TLCORNER || levl[x][y].typ == TRCORNER || levl[x][y].typ == BLCORNER || levl[x][y].typ == BRCORNER || levl[x][y].typ == CROSSWALL || levl[x][y].typ == TUWALL || levl[x][y].typ == TDWALL || levl[x][y].typ == TRWALL || levl[x][y].typ == TLWALL || levl[x][y].typ == DBWALL ) && !((moves + u.monstertimefinish) % 4413 ) && !rn2((ishaxor && !issuxxor) ? 50 : (issuxxor && !ishaxor) ? 200 : 100)) )
	    	    makerandomtrap_at(x, y);

		if ((levl[x][y].typ == SDOOR && !((moves + u.monstertimefinish) % 4415 ) && !rn2((ishaxor && !issuxxor) ? 10 : (issuxxor && !ishaxor) ? 40 : 20)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == SCORR && !((moves + u.monstertimefinish) % 4417 ) && !rn2((ishaxor && !issuxxor) ? 20 : (issuxxor && !ishaxor) ? 80 : 40)) )
	    	    makerandomtrap_at(x, y);
		if ((levl[x][y].typ == DOOR && !((moves + u.monstertimefinish) % 4419 ) && !rn2((ishaxor && !issuxxor) ? 30 : (issuxxor && !ishaxor) ? 120 : 60)) )
	    	    makerandomtrap_at(x, y);

		}

		}

	/* determine if it is even allowed;
	   almost all special levels are excluded */
	if (In_hell(&u.uz) || In_V_tower(&u.uz) ||
#ifdef REINCARNATION
		Is_rogue_level(&u.uz) ||
#endif
		level.flags.arboreal ||
		((sp = Is_special(&u.uz)) != 0 && !Is_oracle_level(&u.uz)
					&& (!In_mines(&u.uz) || sp->flags.town)
	    )) return;

	/* basic level-related probabilities */
	goldprob = 20 + depth(&u.uz) / 3;
	gemprob = goldprob / 4;
	objprob = goldprob / 5;

	/* mines have ***MORE*** goodies - otherwise why mine? */
	if (In_mines(&u.uz)) {
	    goldprob *= 2;
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
	    if (levl[x][y+1].typ != STONE) {	 /* <x,y> spot not eligible */
		y += 2;		/* next two spots aren't eligible either */
	    } else if (levl[x][y].typ != STONE) { /* this spot not eligible */
		y += 1;		/* next spot isn't eligible either */
	    } else if (!(levl[x][y].wall_info & W_NONDIGGABLE) &&
		  levl[x][y-1].typ   == STONE &&
		  levl[x+1][y-1].typ == STONE && levl[x-1][y-1].typ == STONE &&
		  levl[x+1][y].typ   == STONE && levl[x-1][y].typ   == STONE &&
		  levl[x+1][y+1].typ == STONE && levl[x-1][y+1].typ == STONE) {
		if (rn2(1000) < goldprob) {
		    if ((otmp = mksobj(GOLD_PIECE, FALSE, FALSE)) != 0) {
			otmp->ox = x,  otmp->oy = y;
			otmp->quan = 1L + rnd(goldprob * 3);
			otmp->owt = weight(otmp);
			if (!rn2(3)) add_to_buried(otmp);
			else place_object(otmp, x, y);
		    }
		}
		if (rn2(1000) < gemprob) {
		    for (cnt = rnd(2 + dunlev(&u.uz) / 3); cnt > 0; cnt--)
			if ((otmp = mkobj(GEM_CLASS, FALSE)) != 0) {
			    if (otmp->otyp == ROCK) {
				dealloc_obj(otmp);	/* discard it */
			    } else {
				otmp->ox = x,  otmp->oy = y;
				if (!rn2(3)) add_to_buried(otmp);
				else place_object(otmp, x, y);
			    }
		    }
		}
		if (rn2(1000) < objprob) {
		    for (cnt = rnd(2 + dunlev(&u.uz) / 3); cnt > 0; cnt--)
			if ((otmp = mkobj(RANDOM_CLASS, FALSE)) != 0) {
			    if (otmp->otyp == ROCK) {
				dealloc_obj(otmp);	/* discard it */
			    } else {
				otmp->ox = x,  otmp->oy = y;
				if (!rn2(3)) add_to_buried(otmp);
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

	if(getbones()) {

		u.gottenbones = 1;
		return;
	}

	in_mklev = TRUE;
	makelevel();
	bound_digging();
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
	} else
	    croom = &rooms[rn2(nroom)];

	do {
	    if (!somexy(croom, mp))
		impossible("Can't place branch!");
	} while(occupied(mp->x, mp->y) ||
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

	/*
	 * Return immediately if there is no branch to make or we have
	 * already made one.  This routine can be called twice when
	 * a special level is loaded that specifies an SSTAIR location
	 * as a favored spot for a branch.
	 */
	if (!br || made_branch) return;

	if (!x) {	/* find random coordinates for branch */
	    br_room = find_branch_room(&m);
	    x = m.x;
	    y = m.y;
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
		|| is_pool(x,y)
		|| invocation_pos(x,y)
		));
}

/* make a trap somewhere (in croom if mazeflag = 0 && !tm) */
/* if tm != null, make trap at that location */
void
mktrap(num, mazeflag, croom, tm)
register int num, mazeflag;
register struct mkroom *croom;
coord *tm;
{
	register int kind;
	coord m;

	/* no traps in pools */
	if (tm && is_pool(tm->x,tm->y)) return;

	if (num > 0 && num < TRAPNUM) {
	    kind = num;
#ifdef REINCARNATION
	} else if (Is_rogue_level(&u.uz)) {
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
	} else if (( (Inhell && !Race_if(PM_HERETIC) ) || flags.gehenna) && !rn2(5)) {
	    /* bias the frequency of fire traps in Gehennom */
	    kind = FIRE_TRAP;
	} else {
	    unsigned lvl = level_difficulty();

	    do {
		kind = rnd(TRAPNUM-1);
		/* reject "too hard" traps */
		switch (kind) {
		    case MAGIC_PORTAL:
			kind = ROCKTRAP; break;
		    case ROLLING_BOULDER_TRAP:
		    case SLP_GAS_TRAP:
			/* if (lvl < 2) kind = NO_TRAP; */ break;
		    case LEVEL_TELEP:
			if (level.flags.noteleport || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) ) kind = ANTI_MAGIC; break;
		    case SPIKED_PIT:
			/* if (lvl < 5) kind = NO_TRAP; */ break;
		    case LANDMINE:
			/* if (lvl < 6) kind = NO_TRAP; */ break;
		    case WEB:
			/*if ( (lvl < 7) && (!rn2(3)) ) kind = STATUE_TRAP;*/ break;
		    case STATUE_TRAP:
		    case POLY_TRAP:
			/* if (lvl < 8) kind = NO_TRAP; */ break;
		    case FIRE_TRAP:
			/* if (!Inhell) kind = NO_TRAP; */ break;
		    case TELEP_TRAP:
			if (level.flags.noteleport) kind = SQKY_BOARD; break;
		    case HOLE:
			/* make these much less often than other traps */
			if (rn2(7)) kind = STATUE_TRAP; break;

		    case MENU_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8)) kind = FIRE_TRAP;
		    case SPEED_TRAP: 
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40)) kind = ICE_TRAP;
		    case AUTOMATIC_SWITCHER:
			if (rn2(ishaxor ? (Role_if(PM_GRADUATE) ? 125 : Role_if(PM_SPACEWARS_FIGHTER) ? 250 : Role_if(PM_GEEK) ? 250 : 500) : (Role_if(PM_GRADUATE) ? 250 : Role_if(PM_SPACEWARS_FIGHTER) ? 500 : Role_if(PM_GEEK) ? 500 : 1000) )) kind = SHOCK_TRAP;

		    case RMB_LOSS_TRAP:
			if (!Role_if(PM_SPACEWARS_FIGHTER) && rn2(2)) kind = ACID_POOL; break;
		    case DISPLAY_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3)) kind = GLYPH_OF_WARDING; break;
		    case SPELL_LOSS_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4)) kind = SLOW_GAS_TRAP; break;
		    case YELLOW_SPELL_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 5)) kind = POISON_GAS_TRAP; break;
		    case AUTO_DESTRUCT_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10)) kind = WATER_POOL; break;
		    case MEMORY_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20)) kind = SCYTHING_BLADE; break;
		    case INVENTORY_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50)) kind = ANIMATION_TRAP; break;
		    case BLACK_NG_WALL_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100)) kind = UNKNOWN_TRAP; break;
		    case SUPERSCROLLER_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 100 : 200)) kind = WEB; break;
		    case ACTIVE_SUPERSCROLLER_TRAP: kind = RUST_TRAP; break;

		    case ITEM_TELEP_TRAP:
			if (rn2(15)) kind = ANIMATION_TRAP; break;
		    case LAVA_TRAP:
			if (rn2(50)) kind = PIT; break;
		    case FLOOD_TRAP:
			if (rn2(20)) kind = ROCKTRAP; break;
		    case DRAIN_TRAP:
			if (rn2(3)) kind = FIRE_TRAP; break;
		    case FREE_HAND_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) kind = SLP_GAS_TRAP; break;
		    case DISINTEGRATION_TRAP:
			if (rn2(20)) kind = ICE_TRAP; break;
		    case UNIDENTIFY_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) kind = MAGIC_TRAP; break;
		    case THIRST_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) kind = ANTI_MAGIC; break;
		    case SHADES_OF_GREY_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) kind = WEB; break;
		    case LUCK_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) kind = STATUE_TRAP; break;
		    case DEATH_TRAP:
			if (rn2(100)) kind = SHOCK_TRAP; break;
		    case STONE_TO_FLESH_TRAP:
			if (rn2(100)) kind = ACID_POOL; break;
		    case FAINT_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) kind = BOLT_TRAP; break;
		    case CURSE_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 32 : 64 )) kind = ARROW_TRAP; break;
		    case DIFFICULTY_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 12 : 24 )) kind = DART_TRAP; break;
		    case SOUND_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 14 )) kind = SQKY_BOARD; break;
		    case CASTER_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) kind = LANDMINE; break;
		    case WEAKNESS_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) kind = BEAR_TRAP; break;
		    case ROT_THIRTEEN_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) kind = RUST_TRAP; break;
		    case BISHOP_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 28 : 56 )) kind = SPIKED_PIT; break;
		    case CONFUSION_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 40 : 80 )) kind = SHIT_TRAP; break;
		    case DROP_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 35 : 70 )) kind = Role_if(PM_SPACEWARS_FIGHTER) ? NUPESELL_TRAP : POISON_GAS_TRAP; break;
		    case DSTW_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) kind = SLOW_GAS_TRAP; break;
		    case STATUS_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 9 )) kind = SCYTHING_BLADE; break;
		    case ALIGNMENT_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 7 )) kind = ANTI_MAGIC; break;
		    case STAIRS_TRAP:
			if (rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 11 )) kind = GLYPH_OF_WARDING; break;

		    case PESTILENCE_TRAP:
			if (rn2(20)) kind = POISON_GAS_TRAP; break;
		    case FAMINE_TRAP:
			if (rn2(2)) kind = SLOW_GAS_TRAP; break;

		}
		if (!rn2(12)) kind = STATUE_TRAP;

	    } while (kind == NO_TRAP);
	}

	if ((kind == TRAPDOOR || kind == HOLE) && !Can_fall_thru(&u.uz))
		kind = ROCKTRAP;

	if (tm)
	    m = *tm;
	else {
	    register int tryct = 0;
	    boolean avoid_boulder = (kind == PIT || kind == SPIKED_PIT ||
				     kind == TRAPDOOR || kind == HOLE);

	    do {
		if (++tryct > 200)
		    return;
		if (mazeflag)
		    mazexy(&m);
		else if (!somexy(croom,&m))
		    return;
	    } while (occupied(m.x, m.y) ||
			(avoid_boulder && sobj_at(BOULDER, m.x, m.y)));
	}

	(void) maketrap(m.x, m.y, kind);
	/* Webs can generate on dlvl1, where giant spiders would be totally out of depth. Let's make random spiders. --Amy */
	if (kind == WEB) (void) makemon( /*&mons[PM_GIANT_SPIDER]*/ mkclass(S_SPIDER,0),
						m.x, m.y, NO_MM_FLAGS);
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
		mazexy(&m);
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

#ifdef SINKS
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
#endif /* SINKS */

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
	if (!rn2(10)) levl[m.x][m.y].altarmask = Align2amask( al );
	else levl[m.x][m.y].altarmask = Align2amask( A_NONE );
}

static void
mkgrave(croom)
struct mkroom *croom;
{
	coord m;
	register int tryct = 0;
	register struct obj *otmp;
	boolean dobell = !rn2(10);


	if(croom->rtype != OROOM) return;

	do {
	    if(++tryct > 200) return;
	    if (!somexy(croom, &m))
		return;
	} while (occupied(m.x, m.y) || bydoor(m.x, m.y));

	/* Put a grave at m.x, m.y */
	make_grave(m.x, m.y, dobell ? "Saved by the bell!" : (char *) 0);

	/* Possibly fill it with objects */
	if (!rn2(3)) (void) mkgold(0L, m.x, m.y);
	for (tryct = rn2(5); tryct; tryct--) {
	    otmp = mkobj(RANDOM_CLASS, TRUE);
	    if (!otmp) return;
	    curse(otmp);
	    otmp->ox = m.x;
	    otmp->oy = m.y;
	    add_to_buried(otmp);
	}

	/* Leave a bell, in case we accidentally buried someone alive */
	if (dobell) (void) mksobj_at(BELL, m.x, m.y, TRUE, FALSE);
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
    achieve.perform_invocation = 1;
#ifdef LIVELOGFILE
    livelog_achieve_update();

	/* let's spice things up a bit... --Amy */
    (void) makemon(&mons[PM_CHAOS_MONKEY], 0, 0, NO_MM_FLAGS);

#endif
#endif
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
	if (is_pool(x,y)) break;
	lev->typ = ROOM;
	ttmp = maketrap(x, y, FIRE_TRAP);
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
STATIC_OVL void
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
	if (source->dnum < n_dgns || (rn2(3)
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
	if(hd > 8 && i > 85)
		return(mkclass(S_VAMPIRE,0));

	return((i < 20) ? &mons[PM_GHOST]
			: (i < 40) ? mkclass(S_WRAITH,0) : (i < 70) ? mkclass(S_MUMMY,0) : mkclass(S_ZOMBIE,0));
} /* added mummies, enabled all of S_wraith type monsters --Amy */

/*mklev.c*/
