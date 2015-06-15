/*	SCCS Id: @(#)rnd.c	3.4	1996/02/07	*/
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

/* "Rand()"s definition is determined by [OS]conf.h */
#if defined(LINT) && defined(UNIX)	/* rand() is long... */
extern int NDECL(rand);
#define RND(x)	(rand() % x)
#else /* LINT */
# if defined(UNIX) || defined(RANDOM)
#define RND(x)	(int)(Rand() % (long)(x))
# else
/* Good luck: the bottom order bits are cyclic. */
#define RND(x)	(int)((Rand()>>3) % (x))
# endif
#endif /* LINT */

#ifdef OVL0

/* edit by Amy - since DEBUG isn't defined by default, we're printing impossible messages and returning a default value
 * because we don't want any of that segfaulting crap. Yes, RND(x) segfaults with a non-positive value for x.
 * And tracking down segfaults is NO fun, let me tell you. The game just closes with no message. */

int
rn2(x)		/* 0 <= rn2(x) < x */
register int x;
{
#ifdef DEBUG
	if (x <= 0) {
		impossible("rn2(%d) attempted", x);
		return(0);
	}
	x = RND(x);
	return(x);
#else
	if (x <= 0) {
		impossible("rn2(%d) attempted - returning zero", x);
		return(0);
	}
	return(RND(x));
#endif
}

#endif /* OVL0 */
#ifdef OVLB

int
rnl(x)		/* 0 <= rnl(x) < x; sometimes subtracting Luck */
register int x;	/* good luck approaches 0, bad luck approaches (x-1) */
{
	register int i;

#ifdef DEBUG
	if (x <= 0) {
		impossible("rnl(%d) attempted", x);
		return(0);
	}
#endif
	if (x <= 0) {
		impossible("rnl(%d) attempted - returning zero", x);
		return(0);
	}

	i = RND(x);

	if (Luck && rn2(50 - Luck)) {
	    i -= (x <= 15 && Luck >= -5 ? Luck/3 : Luck);
	    if (i < 0) i = 0;
	    else if (i >= x) i = x-1;
	}

	if (LuckLoss || have_unluckystone() ) change_luck(-1);

	return i;
}

#endif /* OVLB */
#ifdef OVL0

int
rnd(x)		/* 1 <= rnd(x) <= x */
register int x;
{
#ifdef DEBUG
	if (x <= 0) {
		impossible("rnd(%d) attempted", x);
		return(1);
	}
	x = RND(x)+1;
	return(x);
#else
	if (x <= 0) {
		impossible("rnd(%d) attempted - returning 1", x);
		return(1);
	}
	return(RND(x)+1);
#endif
}

#endif /* OVL0 */
#ifdef OVL1

int
d(n,x)		/* n <= d(n,x) <= (n*x) */
register int n, x;
{
	register int tmp = n;

#ifdef DEBUG
	if (x < 0 || n < 0 || (x == 0 && n != 0)) {
		impossible("d(%d,%d) attempted", n, x);
		return(1);
	}
#endif
	if (x < 0 || n < 0 || (x == 0 && n != 0)) {
		impossible("d(%d,%d) attempted - returning 1", n, x);
		return(1);
	}
	while(n--) tmp += RND(x);
	return(tmp); /* Alea iacta est. -- J.C. */
}

#endif /* OVL1 */
#ifdef OVLB

int
rne(x)
register int x;
{
	register int tmp, utmp;

	utmp = /*(u.ulevel < 15) ? 5 : u.ulevel/3*/100;
	tmp = 1;
	while (tmp < utmp && !rn2(x))
		tmp++;

	if (isxrace) {	/* x-race is the "rng-changing race", which affects rne and thus rnz too --Amy */

		while (tmp < utmp && !rn2(x) && !rn2(2) )
			tmp++;

	}

	return tmp;

	/* was:
	 *	tmp = 1;
	 *	while(!rn2(x)) tmp++;
	 *	return(min(tmp,(u.ulevel < 15) ? 5 : u.ulevel/3));
	 * which is clearer but less efficient and stands a vanishingly
	 * small chance of overflowing tmp
	 */
}

int
rnz(i)
int i;
{
#ifdef LINT
	int x = i;
	int tmp = 1000;
#else
	register long x = i;
	register long tmp = 1000;
#endif
	tmp += rn2(1000);
	if (rn2(5)) {tmp *= rne(4); } /*adjustments by Amy*/
	else { tmp *= rne(3); }
	if (rn2(2)) { x *= tmp; x /= 1000; }
	else { x *= 1000; x /= tmp; }
	if (x < 1) x = 1;
	return((int)x);
}

int
randattack()
{

	switch (rnd(169)) {

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
			return AD_PHYS;
		case 11:
		case 12:
		case 13:
			return AD_MAGM;
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
			return AD_FIRE;
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			return AD_COLD;
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
			return AD_SLEE;
		case 29:
			return AD_DISN;
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
			return AD_ELEC;
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
			return AD_DRST;
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
			return AD_ACID;
		case 45:
		case 46:
		case 47:
			return AD_LITE;
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
			return AD_BLND;
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
			return AD_STUN;
		case 58:
		case 59:
		case 60:
			return AD_SLOW;
		case 61:
		case 62:
			return AD_PLYS;
		case 63:
		case 64:
			return AD_DRLI;
		case 65:
		case 66:
		case 67:
			return AD_DREN;
		case 68:
		case 69:
		case 70:
		case 71:
			return AD_LEGS;
		case 72:
			return AD_STON;
		case 73:
		case 74:
		case 75:
			return AD_STCK;
		case 76:
		case 77:
		case 78:
			return AD_SGLD;
		case 79:
		case 80:
		case 81:
			return AD_SITM;
		case 82:
		case 83:
		case 84:
			return AD_SEDU;
		case 85:
		case 86:
		case 87:
			return AD_TLPT;
		case 88:
		case 89:
		case 90:
			return AD_RUST;
		case 91:
		case 92:
		case 93:
		case 94:
		case 95:
			return AD_CONF;
		case 96:
			return AD_DGST;
		case 97:
			return AD_HEAL;
		case 98:
		case 99:
		case 100:
			return AD_WRAP;
		case 101:
		case 102:
		case 103:
			return AD_DRDX;
		case 104:
		case 105:
		case 106:
			return AD_DRCO;
		case 107:
		case 108:
			return AD_DRIN;
		case 109:
		case 110:
			return AD_DISE;
		case 111:
		case 112:
		case 113:
			return AD_DCAY;
		case 114:
			return AD_SSEX;
		case 115:
		case 116:
		case 117:
		case 118:
		case 119:
			return AD_HALU;
		case 120:
			return AD_DETH;
		case 121:
			return AD_PEST;
		case 122:
			return AD_FAMN;
		case 123:
			return AD_SLIM;
		case 124:
		case 125:
		case 126:
			return AD_CALM;
		case 127:
		case 128:
			return AD_ENCH;
		case 129:
			return AD_POLY;
		case 130:
		case 131:
		case 132:
			return AD_CORR;
		case 133:
		case 134:
		case 135:
			return AD_TCKL;
		case 136:
		case 137:
		case 138:
			return AD_NGRA;
		case 139:
		case 140:
		case 141:
			return AD_GLIB;
		case 142:
		case 143:
		case 144:
			return AD_DARK;
		case 145:
			return AD_WTHR;
		case 146:
		case 147:
			return AD_LUCK;
		case 148:
		case 149:
		case 150:
		case 151:
		case 152:
			return AD_NUMB;
		case 153:
		case 154:
		case 155:
		case 156:
			return AD_FRZE;
		case 157:
		case 158:
		case 159:
			return AD_DISP;
		case 160:
		case 161:
		case 162:
		case 163:
		case 164:
			return AD_BURN;
		case 165:
		case 166:
		case 167:
		case 168:
		case 169:
			return AD_FEAR;
		default:
			return AD_PHYS;

	}

}

#endif /* OVLB */

/*rnd.c*/
