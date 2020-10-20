/*	SCCS Id: @(#)rnd.c	3.4	1996/02/07	*/
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

/* "Rand()"s definition is determined by [OS]conf.h */
#if defined(LINT) && defined(UNIX)	/* rand() is long... */
extern int rand(void);
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

static int reseed_period = 0;
static int reseed_count = 0;

/* We need a cryptographically secure RNG. This is mainly for junethack, even though its rules already state that
 * exploiting flaws in a weak random number generator is forbidden, but apparently most other variants made it secure,
 * and one example of such a variant is dnethack which seems to have the easiest to implement method available.
 * Therefore I ported the dnethack code. It really is mainly meant to ensure that you can't exploit mechanics to get
 * specific RNG results during tournaments --Amy */

void
check_reseed()
{
	reseed_count++;
	if (reseed_count > reseed_period) {
		FILE *fptr = NULL;
		int rnd[2];

#ifdef PUBLIC_SERVER
		fptr = fopen("/dev/urandom","r");
		if (fptr) {
			fread((void *)rnd, sizeof(int),2,fptr);
			fclose(fptr);
			srandom((int) (time((time_t *)0)) + rnd[0]);
			reseed_period = (rnd[1] % 700) + 10;
		}
#endif
		reseed_count = 0;
	}
}

/* edit by Amy - since DEBUG isn't defined by default, we're printing impossible messages and returning a default value
 * because we don't want any of that segfaulting crap. Yes, RND(x) segfaults with a non-positive value for x.
 * And tracking down segfaults is NO fun, let me tell you. The game just closes with no message. */

int
rn2(x)		/* 0 <= rn2(x) < x */
register int x;
{
	/*check_reseed();*/
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

int
rn3(x)		/* like rn2, but the result is more likely to be a lower number --Amy */
register int x;
{
	int y;
	/*check_reseed();*/
	if (x <= 0) {
		impossible("rn3(%d) attempted - returning zero", x);
		return(0);
	}
	y = RND(x);
	if (!rn2(2) && (y > 1)) y /= 2;
	if (y < 0) y = 0; /* fail safe */
	return (y);

}

#endif /* OVL0 */
#ifdef OVLB

int
rnl(x)		/* 0 <= rnl(x) < x; sometimes subtracting Luck */
register int x;	/* good luck approaches 0, bad luck approaches (x-1) */
{
	register int i;
	/*check_reseed();*/

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

	if (LuckLoss || u.uprops[LUCK_LOSS].extrinsic || have_unluckystone() ) change_luck(-1);

	return i;
}

#endif /* OVLB */
#ifdef OVL0

int
rnd(x)		/* 1 <= rnd(x) <= x */
register int x;
{
	/*check_reseed();*/
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

int
rno(x)		/* like rnd, but the result is more likely to be a lower number --Amy */
register int x;
{
	int y;
	/*check_reseed();*/
	if (x <= 0) {
		impossible("rno(%d) attempted - returning 1", x);
		return(1);
	}
	y = RND(x)+1;
	if (!rn2(2) && (y > 1)) y /= 2;
	if (y < 1) y = 1; /* fail safe */
	return (y);
}

#endif /* OVL0 */
#ifdef OVL1

int
d(n,x)		/* n <= d(n,x) <= (n*x) */
register int n, x;
{
	register int tmp = n;
	check_reseed();

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

	if (youmonst.data && uarmc && uarmc->oartifact == ART_CHANGED_RANDOM_NUMBERS) {

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

	switch (rnd(351)) {

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
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			return AD_PHYS;
		case 16:
		case 17:
		case 18:
			return AD_MAGM;
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
			return AD_FIRE;
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
			return AD_COLD;
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
			return AD_SLEE;
		case 38:
			return AD_DISN;
		case 39:
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
			return AD_ELEC;
		case 46:
		case 47:
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
			return AD_DRST;
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
		case 58:
			return AD_ACID;
		case 59:
		case 60:
		case 61:
			return AD_LITE;
		case 62:
		case 63:
		case 64:
		case 65:
		case 66:
		case 67:
		case 68:
			return AD_BLND;
		case 69:
		case 70:
		case 71:
		case 72:
		case 73:
		case 74:
			return AD_STUN;
		case 75:
			return (rnd(5) ? AD_SLOW : AD_INER);
		case 76:
		case 77:
			return AD_SLOW;
		case 78:
		case 79:
			return AD_PLYS;
		case 80:
			return (rnd(7) ? AD_DRLI : AD_TIME);
		case 81:
			return AD_DRLI;
		case 82:
		case 83:
		case 84:
		case 85:
			return AD_DREN;
		case 86:
		case 87:
		case 88:
		case 89:
		case 90:
			return AD_LEGS;
		case 91:
			return AD_STON;
		case 92:
		case 93:
		case 94:
		case 95:
		case 96:
			return AD_STCK;
		case 97:
		case 98:
		case 99:
		case 100:
		case 101:
			return AD_SGLD;
		case 102:
		case 103:
		case 104:
		case 105:
		case 106:
			return AD_SITM;
		case 107:
		case 108:
		case 109:
		case 110:
		case 111:
			return AD_SEDU;
		case 112:
		case 113:
		case 114:
		case 115:
		case 116:
			return AD_TLPT;
		case 117:
		case 118:
		case 119:
			return AD_RUST;
		case 120:
		case 121:
		case 122:
		case 123:
		case 124:
		case 125:
		case 126:
			return AD_CONF;
		case 127:
			return AD_DGST;
		case 128:
		case 129:
			return AD_HEAL;
		case 130:
		case 131:
		case 132:
			return AD_WRAP;
		case 133:
		case 134:
		case 135:
			return AD_DRDX;
		case 136:
		case 137:
		case 138:
			return AD_DRCO;
		case 139:
		case 140:
			return AD_DRIN;
		case 141:
		case 142:
			return AD_DISE;
		case 143:
		case 144:
		case 145:
			return AD_DCAY;
		case 146:
			return AD_SSEX;
		case 147:
		case 148:
		case 149:
		case 150:
		case 151:
			return AD_HALU;
		case 152:
			return AD_DETH;
		case 153:
			return AD_PEST;
		case 154:
			return AD_FAMN;
		case 155:
			return AD_SLIM;
		case 156:
		case 157:
		case 158:
		case 159:
			return AD_CALM;
		case 160:
		case 161:
			return AD_ENCH;
		case 162:
			return AD_POLY;
		case 163:
		case 164:
		case 165:
			return AD_CORR;
		case 166:
		case 167:
		case 168:
			return AD_TCKL;
		case 169:
		case 170:
		case 171:
		case 172:
		case 173:
			return AD_NGRA;
		case 174:
		case 175:
		case 176:
			return AD_GLIB;
		case 177:
		case 178:
		case 179:
		case 180:
		case 181:
			return AD_DARK;
		case 182:
			return AD_WTHR;
		case 183:
		case 184:
			return AD_LUCK;
		case 185:
		case 186:
		case 187:
		case 188:
		case 189:
		case 190:
		case 191:
			return AD_NUMB;
		case 192:
		case 193:
		case 194:
		case 195:
			return AD_FRZE;
		case 196:
		case 197:
		case 198:
		case 199:
			return AD_DISP;
		case 200:
		case 201:
		case 202:
		case 203:
		case 204:
		case 205:
			return AD_BURN;
		case 206:
		case 207:
		case 208:
		case 209:
		case 210:
		case 211:
			return AD_FEAR;
		case 212:
			return AD_NPRO;
		case 213:
		case 214:
			return AD_POIS;
		case 215:
		case 216:
		case 217:
		case 218:
			return AD_THIR;
		case 219:
		case 220:
		case 221:
			return AD_LAVA;
		case 222:
		case 223:
		case 224:
		case 225:
		case 226:
		case 227:
		case 228:
			return AD_FAKE;
		case 229:
			return (rn2(5) ? AD_WET : AD_LETH);
		case 230:
			return AD_CNCL;
		case 231:
			return AD_BANI;
		case 232:
		case 233:
		case 234:
			return AD_WISD;
		case 235:
			return AD_SHRD;
		case 236:
			return AD_SUCK;
		case 237:
		case 238:
		case 239:
			return AD_MALK;
		case 240:
			return (rn2(10) ? AD_DRIN : AD_UVUU);
		case 241:
		case 242:
			return AD_ABDC;
		case 243:
			return AD_AXUS;
		case 244:
		case 245:
		case 246:
		case 247:
		case 248:
		case 249:
			return AD_CHKH;
		case 250:
		case 251:
		case 252:
		case 253:
			return AD_HODS;
		case 254:
		case 255:
			return AD_CHRN;
		case 256:
			return (rn2(3) ? AD_WEEP : AD_NEXU);
		case 257:
			return AD_VAMP;
		case 258:
		case 259:
		case 260:
		case 261:
		case 262:
			return AD_WEBS;
		case 263:
			return AD_STTP;
		case 264:
			return AD_DEPR;
		case 265:
			return AD_WRAT;
		case 266:
			return AD_LAZY;
		case 267:
		case 268:
		case 269:
			return AD_DRCH;
		case 270:
			return AD_DFOO;
		case 271:
			return AD_GRAV;
		case 272:
			return AD_SOUN;
		case 273:
			return AD_MANA;
		case 274:
			return AD_PLAS;
		case 275:
			return AD_SKIL;
		case 276:
			return AD_SPC2;
		case 277:
			return AD_VENO;
		case 278:
			return AD_DREA;
		case 279:
			return (rn2(20) ? AD_BADE : AD_NAST);
		case 280:
			return AD_SLUD;
		case 281:
			return AD_ICUR;
		case 282:
			return AD_VULN;
		case 283:
			return AD_FUMB;
		case 284:
		case 285:
		case 286:
		case 287:
		case 288:
			return AD_DIMN;
		case 289:
			return AD_AMNE;
		case 290:
			return AD_ICEB;
		case 291:
			return AD_VAPO;
		case 292:
			return AD_EDGE;
		case 293:
			return AD_VOMT;
		case 294:
			return AD_LITT;
		case 295:
			return AD_FREN;
		case 296:
			return AD_NGEN;
		case 297:
			return AD_CHAO;
		case 298:
			return AD_INSA;
		case 299:
			return AD_TRAP;
		case 300:
			return AD_WGHT;
		case 301:
			return AD_NTHR;
		case 302:
		case 303:
		case 304:
		case 305:
			return AD_CONT;
		case 306:
		case 307:
		case 308:
		case 309:
		case 310:
			return AD_ALIN;
		case 311:
			return AD_SIN;
		case 312:
		case 313:
		case 314:
			return AD_AGGR;
		case 315:
			return (rn2(100) ? AD_DEST : AD_RAGN);
		case 316:
			return AD_TREM;
		case 317:
			return AD_IDAM;
		case 318:
			return AD_ANTI;
		case 319:
			return AD_PAIN;
		case 320:
		case 321:
			return AD_TECH;
		case 322:
		case 323:
		case 324:
			return AD_MEMO;
		case 325:
			return AD_TRAI;
		case 326:
			return AD_STAT;
		case 327:
			return AD_NACU;
		case 328:
		case 329:
		case 330:
			return AD_SANI;
		case 331:
			return AD_RBAD;
		case 332:
		case 333:
			return AD_BLEE;
		case 334:
			return AD_SHAN;
		case 335:
		case 336:
		case 337:
		case 338:
		case 339:
			return AD_SCOR;
		case 340:
			return AD_TERR;
		case 341:
			return AD_FEMI;
		case 342:
			return AD_LEVI;
		case 343:
			return AD_MCRE;
		case 344:
			return AD_FLAM;
		case 345:
			return AD_DEBU;
		case 346:
			return AD_UNPR;
		case 347:
		case 348:
		case 349:
		case 350:
		case 351:
			return AD_NIVE;
		default:
			return AD_PHYS;

	}

}

int
randomdamageattack()
{
	switch (rnd(27)) {
		case 1:
			return AD_ACID;
		case 2:
			return AD_DRLI;
		case 3:
			return AD_RUST;
		case 4:
			return AD_DCAY;
		case 5:
			return AD_CORR;
		case 6:
			return AD_WTHR;
		case 7:
			return AD_NPRO;
		case 8:
			return AD_POIS;
		case 9:
			return AD_LAVA;
		case 10:
			return AD_LETH;
		case 11:
			return AD_CNCL;
		case 12:
			return AD_SHRD;
		case 13:
			return AD_WET;
		case 14:
			return AD_UVUU;
		case 15:
			return AD_VAMP;
		case 16:
			return AD_VENO;
		case 17:
			return AD_NAST;
		case 18:
			return AD_SLUD;
		case 19:
			return AD_ICUR;
		case 20:
			return AD_SIN;
		case 21:
			return AD_CONT;
		case 22:
			return AD_DEST;
		case 23:
			return AD_IDAM;
		case 24:
			return AD_ANTI;
		case 25:
			return AD_NACU;
		case 26:
			return AD_FLAM;
		case 27:
			return AD_UNPR;
		default:
			return AD_PHYS;
	}
}

int
randomthievingattack()
{
	switch (rnd(27)) {
		case 1:
			return AD_SLOW;
		case 2:
			return AD_DREN;
		case 3:
			return AD_SGLD;
		case 4:
			return AD_SITM;
		case 5:
			return AD_SEDU;
		case 6:
			return AD_ENCH;
		case 7:
			return AD_LUCK;
		case 8:
			return AD_SUCK;
		case 9:
			return AD_MALK;
		case 10:
			return AD_STTP;
		case 11:
			return AD_TIME;
		case 12:
			return AD_PLAS;
		case 13:
			return AD_SKIL;
		case 14:
			return AD_VULN;
		case 15:
			return AD_AMNE;
		case 16:
			return AD_ICEB;
		case 17:
			return AD_LITT;
		case 18:
			return AD_NGEN;
		case 19:
			return AD_ALIN;
		case 20:
			return AD_TREM;
		case 21:
			return AD_TECH;
		case 22:
			return AD_MEMO;
		case 23:
			return AD_TRAI;
		case 24:
			return AD_STAT;
		case 25:
			return AD_MCRE;
		case 26:
			return AD_DEBU;
		case 27:
			return AD_NIVE;
		default:
			return AD_PHYS;
	}
}

int
randomillusionattack()
{
	switch (rnd(30)) {
		case 1:
			return AD_SPC2;
		case 2:
			return AD_STUN;
		case 3:
			return AD_DRIN;
		case 4:
			return AD_HALU;
		case 5:
			return AD_DARK;
		case 6:
			return AD_FEAR;
		case 7:
			return AD_FAKE;
		case 8:
			return AD_LETH;
		case 9:
			return AD_CNCL;
		case 10:
			return AD_BANI;
		case 11:
			return AD_WISD;
		case 12:
			return AD_HODS;
		case 13:
			return AD_CHRN;
		case 14:
			return AD_WEEP;
		case 15:
			return AD_STTP;
		case 16:
			return AD_DEPR;
		case 17:
			return AD_LAZY;
		case 18:
			return AD_INER;
		case 19:
			return AD_VULN;
		case 20:
			return AD_AMNE;
		case 21:
			return AD_INSA;
		case 22:
			return AD_TRAP;
		case 23:
			return AD_SANI;
		case 24:
			return AD_SHAN;
		case 25:
			return AD_TERR;
		case 26:
			return AD_FEMI;
		case 27:
			return AD_LEVI;
		case 28:
			return AD_MCRE;
		case 29:
			return AD_DEBU;
		case 30:
			return AD_UNPR;
		default:
			return AD_PHYS;
	}
}

int
randmonsound()
{
	switch (rnd(51)) {
		case 1:
			return MS_SILENT;
		case 2:
			return MS_BARK;
		case 3:
			return MS_MEW;
		case 4:
			return MS_ROAR;
		case 5:
			return MS_GROWL;
		case 6:
			return MS_SQEEK;
		case 7:
			return MS_SQAWK;
		case 8:
			return MS_HISS;
		case 9:
			return MS_BUZZ;
		case 10:
			return MS_GRUNT;
		case 11:
			return MS_NEIGH;
		case 12:
			return MS_WAIL;
		case 13:
			return MS_GURGLE;
		case 14:
			return MS_BURBLE;
		case 15:
			return MS_ANIMAL;
		case 16:
			return MS_SHRIEK;
		case 17:
			return MS_BONES;
		case 18:
			return MS_LAUGH;
		case 19:
			return MS_MUMBLE;
		case 20:
			return MS_IMITATE;
		case 21:
			return MS_ORC;
		case 22:
			return MS_HUMANOID;
		case 23:
			return MS_SOLDIER;
		case 24:
			return MS_DJINNI;
		case 25:
			return MS_NURSE;
		case 26:
			return MS_SEDUCE;
		case 27:
			return MS_VAMPIRE;
		case 28:
			return MS_CUSS;
		case 29:
			return MS_SPELL;
		case 30:
			return MS_WERE;
		case 31:
			return MS_BOAST;
		case 32:
			return MS_GYPSY;
		case 33:
			return MS_SHEEP;
		case 34:
			return MS_CHICKEN;
		case 35:
			return MS_COW;
		case 36:
			return MS_PARROT;
		case 37:
			return MS_WHORE;
		case 38:
			return MS_SUPERMAN;
		case 39:
			return MS_FART_QUIET;
		case 40:
			return MS_FART_NORMAL;
		case 41:
			return MS_FART_LOUD;
		case 42:
			return MS_SOUND;
		case 43:
			return MS_SHOE;
		case 44:
			return MS_STENCH;
		case 45:
			return MS_CONVERT;
		case 46:
			return MS_HCALIEN;
		case 47:
			return MS_DOUGLAS_ADAMS;
		case 48:
			return MS_PUPIL;
		case 49:
			return MS_TEACHER;
		case 50:
			return MS_PRINCIPAL;
		case 51:
			return MS_SMITH;
		default:
			return MS_HUMANOID;
	}
}

int
randenchantment()
{

	switch (rnd(174)) {

		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			return FIRE_RES;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			return COLD_RES;
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			return SLEEP_RES;
		case 16:
		case 17:
		case 18:
			return DISINT_RES;
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			return SHOCK_RES;
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
			return POISON_RES;
		case 29:
		case 30:
		case 31:
			return ACID_RES;
		case 32:
			return STONE_RES;
		case 33:
		case 34:
		case 35:
			return REGENERATION;
		case 36:
		case 37:
		case 38:
			return SEARCHING;
		case 39:
		case 40:
		case 41:
			return SEE_INVIS;
		case 42:
		case 43:
		case 44:
			return INVIS;
		case 45:
		case 46:
		case 47:
		case 48:
		case 49:
			return TELEPORT;
		case 50:
		case 51:
		case 52:
			return TELEPORT_CONTROL;
		case 53:
		case 54:
		case 55:
			return POLYMORPH;
		case 56:
			return POLYMORPH_CONTROL;
		case 57:
		case 58:
		case 59:
			return LEVITATION;
		case 60:
		case 61:
		case 62:
			return STEALTH;
		case 63:
		case 64:
		case 65:
			return AGGRAVATE_MONSTER;
		case 66:
			return CONFLICT;
		case 67:
		case 68:
		case 69:
			return WARNING;
		case 70:
		case 71:
		case 72:
			return TELEPAT;
		case 73:
		case 74:
		case 75:
			return FAST;
		case 76:
			return (rn2(50) ? HUNGER : PASSES_WALLS);
		case 77:
		case 78:
			return HUNGER;
		case 79:
		case 80:
		case 81:
			return REFLECTING;
		case 82:
		case 83:
			return ANTIMAGIC;
		case 84:
		case 85:
			return ENERGY_REGENERATION;
		case 86:
		case 87:
		case 88:
			return MAGICAL_BREATHING;
		case 89:
			return HALF_SPDAM;
		case 90:
			return HALF_PHDAM;
		case 91:
			return SICK_RES;
		case 92:
		case 93:
			return DRAIN_RES;
		case 94:
			return WARN_UNDEAD;
		case 95:
		case 96:
		case 97:
			return FREE_ACTION;
		case 98:
		case 99:
			return FLYING;
		case 100:
		case 101:
			return SLOW_DIGESTION;
		case 102:
			return INFRAVISION;
		case 103:
		case 104:
		case 105:
			return FEAR_RES;
		case 106:
			return DISPLACED;
		case 107:
			return SWIMMING;
		case 108:
			return FIXED_ABIL;
		case 109:
			return KEEN_MEMORY;
		case 110:
			return THE_FORCE;
		case 111:
		case 112:
		case 113:
		case 114:
		case 115:
			switch (rnd(237)) {

			case 1: return SPELLS_LOST;
			case 2: return CASTER_PROBLEM;
			case 3: return STATUS_FAILURE;
			case 4: return LUCK_LOSS;
			case 5: return YELLOW_SPELLS;
			case 6: return REPEATING_VULNERABILITY;
			case 7: return INVENTORY_LOST;
			case 8: return RESPAWN_BUG;
			case 9: return VERISIERTEFFECT;
			case 10: return LEVELBUG;
			case 11: return ALWAYS_EGOTYPES;
			case 12: return FAST_FORWARD;
			case 13: return UNIDENTIFY;
			case 14: return DISPLAY_LOST;
			case 15: return THIRST;
			case 16: return BLACK_NG_WALLS;
			case 17: return DSTW_BUG;
			case 18: return RANDOM_RUMORS;
			case 19: return ROT_THIRTEEN;
			case 20: return ALIGNMENT_FAILURE;
			case 21: return DEAFNESS;
			case 22: return MEMORY_LOST;
			case 23: return FARLOOK_BUG;
			case 24: return CAPTCHA;
			case 25: return RECURRING_AMNESIA;
			case 26: return SENSORY_DEPRIVATION;
			case 27: return BIGSCRIPT;
			case 28: return QUIZZES;
			case 29: return MAPBUG;
			case 30: return MUTENESS;
			case 31: return FOOD_IS_ROTTEN;
			case 32: return MENU_LOST;
			case 33: return FREE_HAND_LOST;
			case 34: return CONFUSION_PROBLEM;
			case 35: return DROP_BUG;
			case 36: return UNINFORMATION;
			case 37: return AUTOMATIC_TRAP_CREATION;
			case 38: return SHADES_OF_GREY;
			case 39: return BANKBUG;
			case 40: return TECHBUG;
			case 41: return CHAOS_TERRAIN;
			case 42: return AUTO_DESTRUCT;
			case 43: return SPEED_BUG;
			case 44: return FAINT_ACTIVE;
			case 45: return DIFFICULTY_INCREASED;
			case 46: return WEAKNESS_PROBLEM;
			case 47: return BISHOP_GRIDBUG;
			case 48: return STAIRSTRAP;
			case 49: return RECURRING_DISENCHANT;
			case 50: return ITEMCURSING;
			case 51: return FAST_METABOLISM;
			case 52: return INTRINSIC_LOSS;
			case 53: return BLOOD_LOSS;
			case 54: return NASTINESS_EFFECTS;
			case 55: return BAD_EFFECTS;
			case 56: return ENGRAVINGBUG;
			case 57: return SUPERSCROLLER_ACTIVE;
			case 58: return SKILL_DEACTIVATED;
			case 59: return STATS_LOWERED;
			case 60: return TRAINING_DEACTIVATED;
			case 61: return EXERCISE_DEACTIVATED;
			case 62: return NORETURN;
			case 63: return RMB_LOST;
			case 64: return TELEPORTING_ITEMS;
			case 65: return MAGIC_DEVICE_BUG;
			case 66: return BOOKBUG;
			case 67: return RANDOM_MESSAGES;
			case 68: return DESECRATION;
			case 69: return STARVATION_EFFECT;
			case 70: return NO_DROPS_EFFECT;
			case 71: return LOW_EFFECTS;
			case 72: return INVIS_TRAPS_EFFECT;
			case 73: return GHOST_WORLD;
			case 74: return DEHYDRATION;
			case 75: return HATE_TRAP_EFFECT;
			case 76: return TOTTER_EFFECT;
			case 77: return NONINTRINSIC_EFFECT;
			case 78: return DROPCURSES_EFFECT;
			case 79: return NAKEDNESS;
			case 80: return ANTILEVELING;
			case 81: return ITEM_STEALING_EFFECT;
			case 82: return REBELLION_EFFECT;
			case 83: return CRAP_EFFECT;
			case 84: return PROJECTILES_MISFIRE;
			case 85: return WALL_TRAP_EFFECT;
			case 86: return TURNLIMITATION;
			case 87: return WEAKSIGHT;
			case 88: return DISCONNECTED_STAIRS;
			case 89: return INTERFACE_SCREW;
			case 90: return BOSSFIGHT;
			case 91: return ENTIRE_LEVEL;
			case 92: return BONES_CHANGE;
			case 93: return AUTOCURSE_EQUIP;
			case 94: return HIGHLEVEL_STATUS;
			case 95: return SPELL_FORGETTING;
			case 96: return SOUND_EFFECT_BUG;
			case 97: return TIMERUN_BUG;

			case 98: return LOOTCUT_BUG;
			case 99: return MONSTER_SPEED_BUG;
			case 100: return SCALING_BUG;
			case 101: return ENMITY_BUG;
			case 102: return WHITE_SPELLS;
			case 103: return COMPLETE_GRAY_SPELLS;
			case 104: return QUASAR_BUG;
			case 105: return MOMMA_BUG;
			case 106: return HORROR_BUG;
			case 107: return ARTIFICER_BUG;
			case 108: return WEREFORM_BUG;
			case 109: return NON_PRAYER_BUG;
			case 110: return EVIL_PATCH_EFFECT;
			case 111: return HARD_MODE_EFFECT;
			case 112: return SECRET_ATTACK_BUG;
			case 113: return EATER_BUG;
			case 114: return COVETOUSNESS_BUG;
			case 115: return NOT_SEEN_BUG;
			case 116: return DARK_MODE_BUG;
			case 117: return ANTISEARCH_EFFECT;
			case 118: return HOMICIDE_EFFECT;
			case 119: return NASTY_NATION_BUG;
			case 120: return WAKEUP_CALL_BUG;
			case 121: return GRAYOUT_BUG;
			case 122: return GRAY_CENTER_BUG;
			case 123: return CHECKERBOARD_BUG;
			case 124: return CLOCKWISE_SPIN_BUG;
			case 125: return COUNTERCLOCKWISE_SPIN_BUG;
			case 126: return LAG_BUG;
			case 127: return BLESSCURSE_EFFECT;
			case 128: return DE_LIGHT_BUG;
			case 129: return DISCHARGE_BUG;
			case 130: return TRASHING_EFFECT;
			case 131: return FILTERING_BUG;
			case 132: return DEFORMATTING_BUG;
			case 133: return FLICKER_STRIP_BUG;
			case 134: return UNDRESSING_EFFECT;
			case 135: return HYPERBLUEWALL_BUG;
			case 136: return NOLITE_BUG;
			case 137: return PARANOIA_BUG;
			case 138: return FLEECESCRIPT_BUG;
			case 139: return INTERRUPT_EFFECT;
			case 140: return DUSTBIN_BUG;
			case 141: return MANA_BATTERY_BUG;
			case 142: return MONSTERFINGERS_EFFECT;
			case 143: return MISCAST_BUG;
			case 144: return MESSAGE_SUPPRESSION_BUG;
			case 145: return STUCK_ANNOUNCEMENT_BUG;
			case 146: return BLOODTHIRSTY_EFFECT;
			case 147: return MAXIMUM_DAMAGE_BUG;
			case 148: return LATENCY_BUG;
			case 149: return STARLIT_BUG;
			case 150: return KNOWLEDGE_BUG;
			case 151: return HIGHSCORE_BUG;
			case 152: return PINK_SPELLS;
			case 153: return GREEN_SPELLS;
			case 154: return EVC_EFFECT;
			case 155: return UNDERLAYER_BUG;
			case 156: return DAMAGE_METER_BUG;
			case 157: return ARBITRARY_WEIGHT_BUG;
			case 158: return FUCKED_INFO_BUG;
			case 159: return BLACK_SPELLS;
			case 160: return CYAN_SPELLS;
			case 161: return HEAP_EFFECT;
			case 162: return BLUE_SPELLS;
			case 163: return TRON_EFFECT;
			case 164: return RED_SPELLS;
			case 165: return TOO_HEAVY_EFFECT;
			case 166: return ELONGATION_BUG;
			case 167: return WRAPOVER_EFFECT;
			case 168: return DESTRUCTION_EFFECT;
			case 169: return MELEE_PREFIX_BUG;
			case 170: return AUTOMORE_BUG;
			case 171: return UNFAIR_ATTACK_BUG;
			case 172: return ORANGE_SPELLS;
			case 173: return VIOLET_SPELLS;
			case 174: return LONGING_EFFECT;
			case 175: return CURSED_PARTS;
			case 176: return QUAVERSAL;
			case 177: return APPEARANCE_SHUFFLING;
			case 178: return BROWN_SPELLS;
			case 179: return CHOICELESSNESS;
			case 180: return GOLDSPELLS;
			case 181: return DEPROVEMENT;
			case 182: return INITIALIZATION_FAIL;
			case 183: return GUSHLUSH;
			case 184: return SOILTYPE;
			case 185: return DANGEROUS_TERRAINS;
			case 186: return FALLOUT_EFFECT;
			case 187: return MOJIBAKE;
			case 188: return GRAVATION;
			case 189: return UNCALLED_EFFECT;
			case 190: return EXPLODING_DICE;
			case 191: return PERMACURSE_EFFECT;
			case 192: return SHROUDED_IDENTITY;
			case 193: return FEELER_GAUGES;
			case 194: return LONG_SCREWUP;
			case 195: return WING_YELLOW_GLYPHS;
			case 196: return LIFE_SAVING_BUG;
			case 197: return CURSEUSE_EFFECT;
			case 198: return CUT_NUTRITION;
			case 199: return SKILL_LOSS_EFFECT;
			case 200: return AUTOPILOT_EFFECT;
			case 201: return MYSTERIOUS_FORCE_EFFECT;
			case 202: return MONSTER_GLYPH_BUG;
			case 203: return CHANGING_DIRECTIVES;
			case 204: return CONTAINER_KABOOM;
			case 205: return STEAL_DEGRADING;
			case 206: return LEFT_INVENTORY;
			case 207: return FLUCTUATING_SPEED;
			case 208: return TARMU_STROKING_NORA;
			case 209: return FAILURE_EFFECTS;
			case 210: return BRIGHT_CYAN_SPELLS;
			case 211: return FREQUENTATION_SPAWNS;
			case 212: return PET_AI_SCREWED;
			case 213: return SATAN_EFFECT;
			case 214: return REMEMBERANCE_EFFECT;
			case 215: return POKELIE_EFFECT;
			case 216: return AUTOPICKUP_ALWAYS;
			case 217: return DYWYPI_PROBLEM;
			case 218: return SILVER_SPELLS;
			case 219: return METAL_SPELLS;
			case 220: return PLATINUM_SPELLS;
			case 221: return MANLER_EFFECT;
			case 222: return DOORNING_EFFECT;
			case 223: return NOWNSIBLE_EFFECT;
			case 224: return ELM_STREET_EFFECT;
			case 225: return MONNOISE_EFFECT;
			case 226: return RANG_CALL_EFFECT;
			case 227: return RECURRING_SPELL_LOSS;
			case 228: return ANTI_TRAINING_EFFECT;
			case 229: return TECHOUT_BUG;
			case 230: return STAT_DECAY;
			case 231: return MOVEMORKING;
			case 232: return BAD_PARTS;
			case 233: return COMPLETELY_BAD_PARTS;
			case 234: return EVIL_VARIANT_ACTIVE;
			case 235: return SANITY_TREBLE_EFFECT;
			case 236: return STAT_DECREASE_BUG;
			case 237: return SIMEOUT_BUG;
			default: return POISON_RES;

			}
		case 116:
			return FROZEN;
		case 117:
			return GLIB;
		case 118:
			return HALLUC;
		case 119:
			return BURNED;
		case 120:
			return WOUNDED_LEGS;
		case 121:
			return NUMBED;
		case 122:
			return TRAP_REVEALING;
		case 123:
			return BLINDED;
		case 124:
			return PREMDEATH;
		case 125:
			return CLAIRVOYANT;
		case 126:
			return SIGHT_BONUS;
		case 127:
			return VERSUS_CURSES;
		case 128:
			return STUN_RES;
		case 129:
			return CONF_RES;
		case 130:
			return EXTRA_WEAPON_PRACTICE;
		case 131:
			return DTBEEM_RES;
		case 132:
			return (rn2(20) ? POISON_RES : DOUBLE_ATTACK);
		case 133:
			return (rn2(100) ? TELEPORT : QUAD_ATTACK);
		case 134:
			return PSI_RES;
		case 135:
			return MANALEECH;
		case 136:
			return MAP_AMNESIA;
		case 137:
			return DIMMED;
		case 138:
			return PEACEVISION;
		case 139:
		case 140:
		case 141:
		case 142:
		case 143:
		case 144:
		case 145:
		case 146:
		case 147:
		case 148:
		case 149:
		case 150:
		case 151:
		case 152:
		case 153:
			switch (rnd(103)) {
			case 1: return HALLU_PARTY;
			case 2: return DRUNKEN_BOXING;
			case 3: return STUNNOPATHY;
			case 4: return NUMBOPATHY;
			case 5: return FREEZOPATHY;
			case 6: return STONED_CHILLER;
			case 7: return CORROSIVITY;
			case 8: return FEAR_FACTOR;
			case 9: return BURNOPATHY;
			case 10: return SICKOPATHY;
			case 11: return WONDERLEGS;
			case 12: return GLIB_COMBAT;
			case 13: return DIMMOPATHY;
			case 14: return RNGE_IRREGULARITY; 
			case 15: return RNGE_INTERNET_ACCESS;
			case 16: return RNGE_ANTI_GOVERNMENT;
			case 17: return RNGE_GAS_FILTERING;
			case 18: return RNGE_VAMPIRIC_DRAIN;
			case 19: return RNGE_PSEUDO_IDENTIFICATION;
			case 20: return RNGE_GHOST_SUMMONING;
			case 21: return RNGE_PRAYER;
			case 22: return RNGE_ANTI_BEHEADING;
			case 23: return RNGE_POLAND;
			case 24: return RNGE_KICKING;
			case 25: return RNGE_TOESTOMPING;
			case 26: return RNGE_JUNGLE_ACTION;
			case 27: return RNGE_UNLIKABILITY;
			case 28: return RNGE_SPELL_DISRUPTION;
			case 29: return RNGE_BDSM;
			case 30: return RNGE_SLIPPING;
			case 31: return RNGE_MIRRORING;
			case 32: return RNGE_TRUE_SIGHT;
			case 33: return RNGE_LIQUID_DIET;
			case 34: return RNGE_ANGRY_GODS;
			case 35: return RNGE_RADIO_BROADCASTS;
			case 36: return RNGE_OCCASIONAL_DAMAGE;
			case 37: return RNGE_NOISE;
			case 38: return RNGE_VORTICES;
			case 39: return RNGE_EXPLOSIONS;
			case 40: return RNGE_FATAL_ATTRACTION;
			case 41: return RNGE_VOLTAGE;
			case 42: return RNGE_WEEPING;
			case 43: return RNGE_CASTLEVANIA;
			case 44: return RNGE_ENGLAND;
			case 45: return RNGE_FORGETTING;
			case 46: return RNGE_ANTIMAGIC_A;
			case 47: return RNGE_ANTIMAGIC_B;
			case 48: return RNGE_ANTIMAGIC_C;
			case 49: return RNGE_ANTIMAGIC_D;
			case 50: return RNGE_CHINA;
			case 51: return RNGE_LIGHT_ABSORPTION;
			case 52: return RNGE_CHILDBIRTH;
			case 53: return RNGE_RED_ATTUNEMENT;
			case 54: return RNGE_POLAR_OPPOSITES;
			case 55: return RNGE_SICKNESS;
			case 56: return RNGE_COQUETRY;
			case 57: return RNGE_IGNORANCE;
			case 58: return RNGE_VENGEANCE;
			case 59: return RNGE_HIGH_HEELING;
			case 60: return RNGE_GRAVITY;
			case 61: return RNGE_WISH_IMPROVEMENT;
			case 62: return RNGE_POCKET_MONSTERS;
			case 63: return RNGE_LEVUNTATION;
			case 64: return RNGE_PERMANENT_OCCUPATION;
			case 65: return RNGE_NURSERY;
			case 66: return RNGE_SLEXUALITY;
			case 67: return RNGE_ANGBAND;
			case 68: return RNGE_ANOREXIA;
			case 69: return RNGE_DNETHACK;
			case 70: return RNGE_UNNETHACK;
			case 71: return RNGE_NETHACK_FOURK;
			case 72: return RNGE_MARITAL_ARTS;
			case 73: return RNGE_TEAM_SPLAT;
			case 74: return RNGE_ABOMINATIONS;
			case 75: return RNGE_PUNISHMENT;
			case 76: return RNGE_BLOODLUST;
			case 77: return RNGE_IMMOBILITY;
			case 78: return RNGE_BOSS_ENCOUNTERS;
			case 79: return RNGE_IRONMAN_MODE;
			case 80: return RNGE_RIVERS;
			case 81: return RNGE_LOUDSPEAKERS;
			case 82: return RNGE_ARMOR_PRESERVATION;
			case 83: return RNGE_WEAKNESS;
			case 84: return RNGE_FRAILNESS;
			case 85: return RNGE_IRON_MAIDEN;
			case 86: return RNGE_DONORS;
			case 87: return RNGE_AIDS;
			case 88: return RNGE_TRAP_ALERT;
			case 89: return RNGE_EXTINCTION;
			case 90: return RNGE_DEFUSING;
			case 91: return RNGE_DENASTIFICATION;
			case 92: return RNGE_MULTISHOT;
			case 93: return RNGE_SKILL_REVEAL;
			case 94: return RNGE_TECH_INSIGHT;
			case 95: return RNGE_LISTENING;
			case 96: return RNGE_CANNIBALISM;
			case 97: return RNGE_POLYVISION;
			case 98: return RNGE_HEMOPHILIA;
			case 99: return RNGE_BLAST_SHIELDING;
			case 100: return RNGE_FUNNY_HALLU;
			case 101: return RNGE_POLYPILING;
			case 102: return RNGE_SAP_GENERATION;
			case 103: return RNGE_WEALTH;
			default: return POISON_RES;
			}
		case 154:
			return STUNNED;
		case 155:
			return CONFUSION;
		case 156:
			return FEARED;
		case 157:
			return CONT_RES;
		case 158:
			return DISCOUNT_ACTION;
		case 159:
			return FULL_NUTRIENT;
		case 160:
			return TECHNICALITY;
		case 161:
			return SCENT_VIEW;
		case 162:
			return DIMINISHED_BLEEDING;
		case 163:
		case 164:
		case 165:
		case 166:
		case 167:
		case 168:
		case 169:
		case 170:
		case 171:
		case 172:
			switch (rnd(31)) {
			case 1: return FEMTRAP_FEMMY;
			case 2: return FEMTRAP_MADELEINE;
			case 3: return FEMTRAP_MARLENA;
			case 4: return FEMTRAP_ANASTASIA;
			case 5: return FEMTRAP_JESSICA;
			case 6: return FEMTRAP_SOLVEJG;
			case 7: return FEMTRAP_WENDY;
			case 8: return FEMTRAP_KATHARINA;
			case 9: return FEMTRAP_ELENA;
			case 10: return FEMTRAP_THAI;
			case 11: return FEMTRAP_ELIF;
			case 12: return FEMTRAP_NADJA;
			case 13: return FEMTRAP_SANDRA;
			case 14: return FEMTRAP_NATALJE;
			case 15: return FEMTRAP_JEANETTA;
			case 16: return FEMTRAP_YVONNE;
			case 17: return FEMTRAP_MAURAH;
			case 18: return FEMTRAP_MELTEM;
			case 19: return FEMTRAP_SARAH;
			case 20: return FEMTRAP_CLAUDIA;
			case 21: return FEMTRAP_LUDGERA;
			case 22: return FEMTRAP_KATI;
			case 23: return FEMTRAP_NELLY;
			case 24: return FEMTRAP_EVELINE;
			case 25: return FEMTRAP_KARIN;
			case 26: return FEMTRAP_JUEN;
			case 27: return FEMTRAP_KRISTINA;
			case 28: return FEMTRAP_LOU;
			case 29: return FEMTRAP_ALMUT;
			case 30: return FEMTRAP_JULIETTA;
			case 31: return FEMTRAP_ARABELLA;
			default: return POISON_RES;
			}
		case 173:
			return CONTROL_MAGIC;
		case 174:
			return EXP_BOOST;

		default:
			return POISON_RES;

	}

}

int
randnastyenchantment()
{
	switch (rnd(237)) {

		case 1: return SPELLS_LOST;
		case 2: return CASTER_PROBLEM;
		case 3: return STATUS_FAILURE;
		case 4: return LUCK_LOSS;
		case 5: return YELLOW_SPELLS;
		case 6: return REPEATING_VULNERABILITY;
		case 7: return INVENTORY_LOST;
		case 8: return RESPAWN_BUG;
		case 9: return VERISIERTEFFECT;
		case 10: return LEVELBUG;
		case 11: return ALWAYS_EGOTYPES;
		case 12: return FAST_FORWARD;
		case 13: return UNIDENTIFY;
		case 14: return DISPLAY_LOST;
		case 15: return THIRST;
		case 16: return BLACK_NG_WALLS;
		case 17: return DSTW_BUG;
		case 18: return RANDOM_RUMORS;
		case 19: return ROT_THIRTEEN;
		case 20: return ALIGNMENT_FAILURE;
		case 21: return DEAFNESS;
		case 22: return MEMORY_LOST;
		case 23: return FARLOOK_BUG;
		case 24: return CAPTCHA;
		case 25: return RECURRING_AMNESIA;
		case 26: return SENSORY_DEPRIVATION;
		case 27: return BIGSCRIPT;
		case 28: return QUIZZES;
		case 29: return MAPBUG;
		case 30: return MUTENESS;
		case 31: return FOOD_IS_ROTTEN;
		case 32: return MENU_LOST;
		case 33: return FREE_HAND_LOST;
		case 34: return CONFUSION_PROBLEM;
		case 35: return DROP_BUG;
		case 36: return UNINFORMATION;
		case 37: return AUTOMATIC_TRAP_CREATION;
		case 38: return SHADES_OF_GREY;
		case 39: return BANKBUG;
		case 40: return TECHBUG;
		case 41: return CHAOS_TERRAIN;
		case 42: return AUTO_DESTRUCT;
		case 43: return SPEED_BUG;
		case 44: return FAINT_ACTIVE;
		case 45: return DIFFICULTY_INCREASED;
		case 46: return WEAKNESS_PROBLEM;
		case 47: return BISHOP_GRIDBUG;
		case 48: return STAIRSTRAP;
		case 49: return RECURRING_DISENCHANT;
		case 50: return ITEMCURSING;
		case 51: return FAST_METABOLISM;
		case 52: return INTRINSIC_LOSS;
		case 53: return BLOOD_LOSS;
		case 54: return NASTINESS_EFFECTS;
		case 55: return BAD_EFFECTS;
		case 56: return ENGRAVINGBUG;
		case 57: return SUPERSCROLLER_ACTIVE;
		case 58: return SKILL_DEACTIVATED;
		case 59: return STATS_LOWERED;
		case 60: return TRAINING_DEACTIVATED;
		case 61: return EXERCISE_DEACTIVATED;
		case 62: return NORETURN;
		case 63: return RMB_LOST;
		case 64: return TELEPORTING_ITEMS;
		case 65: return MAGIC_DEVICE_BUG;
		case 66: return BOOKBUG;
		case 67: return RANDOM_MESSAGES;
		case 68: return DESECRATION;
		case 69: return STARVATION_EFFECT;
		case 70: return NO_DROPS_EFFECT;
		case 71: return LOW_EFFECTS;
		case 72: return INVIS_TRAPS_EFFECT;
		case 73: return GHOST_WORLD;
		case 74: return DEHYDRATION;
		case 75: return HATE_TRAP_EFFECT;
		case 76: return TOTTER_EFFECT;
		case 77: return NONINTRINSIC_EFFECT;
		case 78: return DROPCURSES_EFFECT;
		case 79: return NAKEDNESS;
		case 80: return ANTILEVELING;
		case 81: return ITEM_STEALING_EFFECT;
		case 82: return REBELLION_EFFECT;
		case 83: return CRAP_EFFECT;
		case 84: return PROJECTILES_MISFIRE;
		case 85: return WALL_TRAP_EFFECT;
		case 86: return TURNLIMITATION;
		case 87: return WEAKSIGHT;
		case 88: return DISCONNECTED_STAIRS;
		case 89: return INTERFACE_SCREW;
		case 90: return BOSSFIGHT;
		case 91: return ENTIRE_LEVEL;
		case 92: return BONES_CHANGE;
		case 93: return AUTOCURSE_EQUIP;
		case 94: return HIGHLEVEL_STATUS;
		case 95: return SPELL_FORGETTING;
		case 96: return SOUND_EFFECT_BUG;
		case 97: return TIMERUN_BUG;
			case 98: return LOOTCUT_BUG;
			case 99: return MONSTER_SPEED_BUG;
			case 100: return SCALING_BUG;
			case 101: return ENMITY_BUG;
			case 102: return WHITE_SPELLS;
			case 103: return COMPLETE_GRAY_SPELLS;
			case 104: return QUASAR_BUG;
			case 105: return MOMMA_BUG;
			case 106: return HORROR_BUG;
			case 107: return ARTIFICER_BUG;
			case 108: return WEREFORM_BUG;
			case 109: return NON_PRAYER_BUG;
			case 110: return EVIL_PATCH_EFFECT;
			case 111: return HARD_MODE_EFFECT;
			case 112: return SECRET_ATTACK_BUG;
			case 113: return EATER_BUG;
			case 114: return COVETOUSNESS_BUG;
			case 115: return NOT_SEEN_BUG;
			case 116: return DARK_MODE_BUG;
			case 117: return ANTISEARCH_EFFECT;
			case 118: return HOMICIDE_EFFECT;
			case 119: return NASTY_NATION_BUG;
			case 120: return WAKEUP_CALL_BUG;
			case 121: return GRAYOUT_BUG;
			case 122: return GRAY_CENTER_BUG;
			case 123: return CHECKERBOARD_BUG;
			case 124: return CLOCKWISE_SPIN_BUG;
			case 125: return COUNTERCLOCKWISE_SPIN_BUG;
			case 126: return LAG_BUG;
			case 127: return BLESSCURSE_EFFECT;
			case 128: return DE_LIGHT_BUG;
			case 129: return DISCHARGE_BUG;
			case 130: return TRASHING_EFFECT;
			case 131: return FILTERING_BUG;
			case 132: return DEFORMATTING_BUG;
			case 133: return FLICKER_STRIP_BUG;
			case 134: return UNDRESSING_EFFECT;
			case 135: return HYPERBLUEWALL_BUG;
			case 136: return NOLITE_BUG;
			case 137: return PARANOIA_BUG;
			case 138: return FLEECESCRIPT_BUG;
			case 139: return INTERRUPT_EFFECT;
			case 140: return DUSTBIN_BUG;
			case 141: return MANA_BATTERY_BUG;
			case 142: return MONSTERFINGERS_EFFECT;
			case 143: return MISCAST_BUG;
			case 144: return MESSAGE_SUPPRESSION_BUG;
			case 145: return STUCK_ANNOUNCEMENT_BUG;
			case 146: return BLOODTHIRSTY_EFFECT;
			case 147: return MAXIMUM_DAMAGE_BUG;
			case 148: return LATENCY_BUG;
			case 149: return STARLIT_BUG;
			case 150: return KNOWLEDGE_BUG;
			case 151: return HIGHSCORE_BUG;
			case 152: return PINK_SPELLS;
			case 153: return GREEN_SPELLS;
			case 154: return EVC_EFFECT;
			case 155: return UNDERLAYER_BUG;
			case 156: return DAMAGE_METER_BUG;
			case 157: return ARBITRARY_WEIGHT_BUG;
			case 158: return FUCKED_INFO_BUG;
			case 159: return BLACK_SPELLS;
			case 160: return CYAN_SPELLS;
			case 161: return HEAP_EFFECT;
			case 162: return BLUE_SPELLS;
			case 163: return TRON_EFFECT;
			case 164: return RED_SPELLS;
			case 165: return TOO_HEAVY_EFFECT;
			case 166: return ELONGATION_BUG;
			case 167: return WRAPOVER_EFFECT;
			case 168: return DESTRUCTION_EFFECT;
			case 169: return MELEE_PREFIX_BUG;
			case 170: return AUTOMORE_BUG;
			case 171: return UNFAIR_ATTACK_BUG;
			case 172: return ORANGE_SPELLS;
			case 173: return VIOLET_SPELLS;
			case 174: return LONGING_EFFECT;
			case 175: return CURSED_PARTS;
			case 176: return QUAVERSAL;
			case 177: return APPEARANCE_SHUFFLING;
			case 178: return BROWN_SPELLS;
			case 179: return CHOICELESSNESS;
			case 180: return GOLDSPELLS;
			case 181: return DEPROVEMENT;
			case 182: return INITIALIZATION_FAIL;
			case 183: return GUSHLUSH;
			case 184: return SOILTYPE;
			case 185: return DANGEROUS_TERRAINS;
			case 186: return FALLOUT_EFFECT;
			case 187: return MOJIBAKE;
			case 188: return GRAVATION;
			case 189: return UNCALLED_EFFECT;
			case 190: return EXPLODING_DICE;
			case 191: return PERMACURSE_EFFECT;
			case 192: return SHROUDED_IDENTITY;
			case 193: return FEELER_GAUGES;
			case 194: return LONG_SCREWUP;
			case 195: return WING_YELLOW_GLYPHS;
			case 196: return LIFE_SAVING_BUG;
			case 197: return CURSEUSE_EFFECT;
			case 198: return CUT_NUTRITION;
			case 199: return SKILL_LOSS_EFFECT;
			case 200: return AUTOPILOT_EFFECT;
			case 201: return MYSTERIOUS_FORCE_EFFECT;
			case 202: return MONSTER_GLYPH_BUG;
			case 203: return CHANGING_DIRECTIVES;
			case 204: return CONTAINER_KABOOM;
			case 205: return STEAL_DEGRADING;
			case 206: return LEFT_INVENTORY;
			case 207: return FLUCTUATING_SPEED;
			case 208: return TARMU_STROKING_NORA;
			case 209: return FAILURE_EFFECTS;
			case 210: return BRIGHT_CYAN_SPELLS;
			case 211: return FREQUENTATION_SPAWNS;
			case 212: return PET_AI_SCREWED;
			case 213: return SATAN_EFFECT;
			case 214: return REMEMBERANCE_EFFECT;
			case 215: return POKELIE_EFFECT;
			case 216: return AUTOPICKUP_ALWAYS;
			case 217: return DYWYPI_PROBLEM;
			case 218: return SILVER_SPELLS;
			case 219: return METAL_SPELLS;
			case 220: return PLATINUM_SPELLS;
			case 221: return MANLER_EFFECT;
			case 222: return DOORNING_EFFECT;
			case 223: return NOWNSIBLE_EFFECT;
			case 224: return ELM_STREET_EFFECT;
			case 225: return MONNOISE_EFFECT;
			case 226: return RANG_CALL_EFFECT;
			case 227: return RECURRING_SPELL_LOSS;
			case 228: return ANTI_TRAINING_EFFECT;
			case 229: return TECHOUT_BUG;
			case 230: return STAT_DECAY;
			case 231: return MOVEMORKING;
			case 232: return BAD_PARTS;
			case 233: return COMPLETELY_BAD_PARTS;
			case 234: return EVIL_VARIANT_ACTIVE;
			case 235: return SANITY_TREBLE_EFFECT;
			case 236: return STAT_DECREASE_BUG;
			case 237: return SIMEOUT_BUG;
	}

	/* appease compiler, we should never end up here */
	return AUTOMORE_BUG;

}

int
randomdisableproperty()
{
	switch (rnd(68)) {

		case 1:
			return FIRE_RES;
		case 2:
			return COLD_RES;
		case 3:
			return SLEEP_RES;
		case 4:
			return DISINT_RES;
		case 5:
			return SHOCK_RES;
		case 6:
			return POISON_RES;
		case 7:
			return DRAIN_RES;
		case 8:
			return SICK_RES;
		case 9:
			return ANTIMAGIC;
		case 10:
			return ACID_RES;
		case 11:
			return STONE_RES;
		case 12:
			return FEAR_RES;
		case 13:
			return SEE_INVIS;
		case 14:
			return TELEPAT;
		case 15:
			return WARNING;
		case 16:
			return SEARCHING;
		case 17:
			return CLAIRVOYANT;
		case 18:
			return INFRAVISION;
		case 19:
			return DETECT_MONSTERS;
		case 20:
			return INVIS;
		case 21:
			return DISPLACED;
		case 22:
			return STEALTH;
		case 23:
			return JUMPING;
		case 24:
			return TELEPORT_CONTROL;
		case 25:
			return FLYING;
		case 26:
			return MAGICAL_BREATHING;
		case 27:
			return PASSES_WALLS;
		case 28:
			return SLOW_DIGESTION;
		case 29:
			return HALF_SPDAM;
		case 30:
			return HALF_PHDAM;
		case 31:
			return REGENERATION;
		case 32:
			return ENERGY_REGENERATION;
		case 33:
			return POLYMORPH_CONTROL;
		case 34:
			return FAST;
		case 35:
			return REFLECTING;
		case 36:
			return FREE_ACTION;
		case 37:
			return HALLU_PARTY;
		case 38:
			return DRUNKEN_BOXING;
		case 39:
			return STUNNOPATHY;
		case 40:
			return NUMBOPATHY;
		case 41:
			return FREEZOPATHY;
		case 42:
			return STONED_CHILLER;
		case 43:
			return CORROSIVITY;
		case 44:
			return FEAR_FACTOR;
		case 45:
			return BURNOPATHY;
		case 46:
			return SICKOPATHY;
		case 47:
			return KEEN_MEMORY;
		case 48:
			return THE_FORCE;
		case 49:
			return SIGHT_BONUS;
		case 50:
			return VERSUS_CURSES;
		case 51:
			return STUN_RES;
		case 52:
			return CONF_RES;
		case 53:
			return DOUBLE_ATTACK;
		case 54:
			return QUAD_ATTACK;
		case 55:
			return PSI_RES;
		case 56:
			return WONDERLEGS;
		case 57:
			return GLIB_COMBAT;
		case 58:
			return MANALEECH;
		case 59:
			return DIMMOPATHY;
		case 60:
			return PEACEVISION;
		case 61:
			return CONT_RES;
		case 62:
			return DISCOUNT_ACTION;
		case 63:
			return FULL_NUTRIENT;
		case 64:
			return TECHNICALITY;
		case 65:
			return SCENT_VIEW;
		case 66:
			return DIMINISHED_BLEEDING;
		case 67:
			return CONTROL_MAGIC;
		case 68:
			return EXP_BOOST;
		default:
			return POISON_RES;
	}
}

int
randominsidetrap()
{
	if (!rn2(7)) return TRAP_PERCENTS;
	else if (!rn2(9)) return UNKNOWN_TRAP;
	else if (!rn2(20)) return SPACEWARS_TRAP;
	else if (!rn2(50)) return ARABELLA_SPEAKER;
	else if (!rn2(100)) return SUPERTHING_TRAP;
	else if (!rn2(2000)) return AUTOMATIC_SWITCHER;
	else switch (rnd(237)) {
		case 1: return RMB_LOSS_TRAP;
		case 2: return UNINFORMATION_TRAP;
		case 3: return BIGSCRIPT_TRAP;
		case 4: return BAD_EFFECT_TRAP;
		case 5: return CAPTCHA_TRAP;
		case 6: return WEAKNESS_TRAP;
		case 7: return TRAINING_TRAP;
		case 8: return ALIGNMENT_TRAP;
		case 9: return DISPLAY_TRAP;
		case 10: return SPELL_LOSS_TRAP;
		case 11: return STEALER_TRAP;
		case 12: return REBELLION_TRAP;
		case 13: return EXERCISE_TRAP;
		case 14: return NUPESELL_TRAP;
		case 15: return CASTER_TRAP;
		case 16: return QUIZ_TRAP;
		case 17: return INTRINSIC_LOSS_TRAP;
		case 18: return NONINTRINSICAL_TRAP;
		case 19: return LOW_STATS_TRAP;
		case 20: return CRAP_TRAP;
		case 21: return YELLOW_SPELL_TRAP;
		case 22: return MISFIRE_TRAP;
		case 23: return SOUND_TRAP;
		case 24: return LOW_EFFECT_TRAP;
		case 25: return EGOTRAP;
		case 26: return LEVEL_TRAP;
		case 27: return WEAK_SIGHT_TRAP;
		case 28: return DEHYDRATION_TRAP;
		case 29: return RANDOM_MESSAGE_TRAP;
		case 30: return MULTIPLY_TRAP;
		case 31: return TRAP_OF_ROTTENNESS;
		case 32: return DSTW_TRAP;
		case 33: return STATUS_TRAP;
		case 34: return MENU_TRAP;
		case 35: return AUTO_DESTRUCT_TRAP;
		case 36: return DIFFICULTY_TRAP;
		case 37: return BANK_TRAP;
		case 38: return STAIRS_TRAP;
		case 39: return FREE_HAND_TRAP;
		case 40: return AUTO_VULN_TRAP;
		case 41: return STARVATION_TRAP;
		case 42: return HATE_TRAP;
		case 43: return UNSKILLED_TRAP;
		case 44: return MAGIC_DEVICE_TRAP;
		case 45: return DROPCURSE_TRAP;
		case 46: return ROT_THIRTEEN_TRAP;
		case 47: return TRAP_OF_NO_RETURN;
		case 48: return GHOST_WORLD_TRAP;
		case 49: return DROP_TRAP;
		case 50: return ENGRAVING_TRAP;
		case 51: return BLOOD_LOSS_TRAP;
		case 52: return BOOK_TRAP;
		case 53: return MEMORY_TRAP;
		case 54: return RESPAWN_TRAP;
		case 55: return MAP_TRAP;
		case 56: return METABOLIC_TRAP;
		case 57: return UNIDENTIFY_TRAP;
		case 58: return NTLL_TRAP;
		case 59: return INVENTORY_TRAP;
		case 60: return THIRST_TRAP;
		case 61: return VERISIERT;
		case 62: return DROPLESS_TRAP;
		case 63: return NAKEDNESS_TRAP;
		case 64: return FAINT_TRAP;
		case 65: return MUTENESS_TRAP;
		case 66: return TRAP_OF_WALLS;
		case 67: return CURSE_TRAP;
		case 68: return CHAOS_TRAP;
		case 69: return FARLOOK_TRAP;
		case 70: return SHADES_OF_GREY_TRAP;
		case 71: return TELE_ITEMS_TRAP;
		case 72: return TECH_TRAP;
		case 73: return FAST_FORWARD_TRAP;
		case 74: return LIMITATION_TRAP;
		case 75: return DESECRATION_TRAP;
		case 76: return BISHOP_TRAP;
		case 77: return SPEED_TRAP;
		case 78: return CONFUSION_TRAP;
		case 79: return INVISIBLE_TRAP;
		case 80: return LUCK_TRAP;
		case 81: return ONLY_TRAP;
		case 82: return DISENCHANT_TRAP;
		case 83: return BLACK_NG_WALL_TRAP;
		case 84: return ANTILEVEL_TRAP;
		case 85: return SUPERSCROLLER_TRAP;
		case 86: return TOTTER_TRAP;
		case 87: return NASTINESS_TRAP;
		case 88: return DISCONNECT_TRAP;
		case 89: return INTERFACE_SCREW_TRAP;
		case 90: return BOSSFIGHT_TRAP;
		case 91: return ENTIRE_LEVEL_TRAP;
		case 92: return BONES_TRAP;
		case 93: return AUTOCURSE_TRAP;
		case 94: return HIGHLEVEL_TRAP;
		case 95: return SPELL_FORGETTING_TRAP;
		case 96: return SOUND_EFFECT_TRAP;
		case 97: return TIMERUN_TRAP;

		case 98: return LOOTCUT_TRAP;
		case 99: return MONSTER_SPEED_TRAP;
		case 100: return SCALING_TRAP;
		case 101: return ENMITY_TRAP;
		case 102: return WHITE_SPELL_TRAP;
		case 103: return COMPLETE_GRAY_SPELL_TRAP;
		case 104: return QUASAR_TRAP;
		case 105: return MOMMA_TRAP;
		case 106: return HORROR_TRAP;
		case 107: return ARTIFICER_TRAP;
		case 108: return WEREFORM_TRAP;
		case 109: return NON_PRAYER_TRAP;
		case 110: return EVIL_PATCH_TRAP;
		case 111: return HARD_MODE_TRAP;
		case 112: return SECRET_ATTACK_TRAP;
		case 113: return EATER_TRAP;
		case 114: return COVETOUSNESS_TRAP;
		case 115: return NOT_SEEN_TRAP;
		case 116: return DARK_MODE_TRAP;
		case 117: return ANTISEARCH_TRAP;
		case 118: return HOMICIDE_TRAP;
		case 119: return NASTY_NATION_TRAP;
		case 120: return WAKEUP_CALL_TRAP;
		case 121: return GRAYOUT_TRAP;
		case 122: return GRAY_CENTER_TRAP;
		case 123: return CHECKERBOARD_TRAP;
		case 124: return CLOCKWISE_SPIN_TRAP;
		case 125: return COUNTERCLOCKWISE_SPIN_TRAP;
		case 126: return LAG_TRAP;
		case 127: return BLESSCURSE_TRAP;
		case 128: return DE_LIGHT_TRAP;
		case 129: return DISCHARGE_TRAP;
		case 130: return TRASHING_TRAP;
		case 131: return FILTERING_TRAP;
		case 132: return DEFORMATTING_TRAP;
		case 133: return FLICKER_STRIP_TRAP;
		case 134: return UNDRESSING_TRAP;
		case 135: return HYPERBLUEWALL_TRAP;
		case 136: return NOLITE_TRAP;
		case 137: return PARANOIA_TRAP;
		case 138: return FLEECESCRIPT_TRAP;
		case 139: return INTERRUPT_TRAP;
		case 140: return DUSTBIN_TRAP;
		case 141: return MANA_BATTERY_TRAP;
		case 142: return MONSTERFINGERS_TRAP;
		case 143: return MISCAST_TRAP;
		case 144: return MESSAGE_SUPPRESSION_TRAP;
		case 145: return STUCK_ANNOUNCEMENT_TRAP;
		case 146: return BLOODTHIRSTY_TRAP;
		case 147: return MAXIMUM_DAMAGE_TRAP;
		case 148: return LATENCY_TRAP;
		case 149: return STARLIT_TRAP;
		case 150: return KNOWLEDGE_TRAP;
		case 151: return HIGHSCORE_TRAP;
		case 152: return PINK_SPELL_TRAP;
		case 153: return GREEN_SPELL_TRAP;
		case 154: return EVC_TRAP;
		case 155: return UNDERLAYER_TRAP;
		case 156: return DAMAGE_METER_TRAP;
		case 157: return ARBITRARY_WEIGHT_TRAP;
		case 158: return FUCKED_INFO_TRAP;
		case 159: return BLACK_SPELL_TRAP;
		case 160: return CYAN_SPELL_TRAP;
		case 161: return HEAP_TRAP;
		case 162: return BLUE_SPELL_TRAP;
		case 163: return TRON_TRAP;
		case 164: return RED_SPELL_TRAP;
		case 165: return TOO_HEAVY_TRAP;
		case 166: return ELONGATION_TRAP;
		case 167: return WRAPOVER_TRAP;
		case 168: return DESTRUCTION_TRAP;
		case 169: return MELEE_PREFIX_TRAP;
		case 170: return AUTOMORE_TRAP;
		case 171: return UNFAIR_ATTACK_TRAP;
		case 172: return ORANGE_SPELL_TRAP;
		case 173: return VIOLET_SPELL_TRAP;
		case 174: return TRAP_OF_LONGING;
		case 175: return CURSED_PART_TRAP;
		case 176: return QUAVERSAL_TRAP;
		case 177: return APPEARANCE_SHUFFLING_TRAP;
		case 178: return BROWN_SPELL_TRAP;
		case 179: return CHOICELESS_TRAP;
		case 180: return GOLDSPELL_TRAP;
		case 181: return DEPROVEMENT_TRAP;
		case 182: return INITIALIZATION_TRAP;
		case 183: return GUSHLUSH_TRAP;
		case 184: return SOILTYPE_TRAP;
		case 185: return DANGEROUS_TERRAIN_TRAP;
		case 186: return FALLOUT_TRAP;
		case 187: return MOJIBAKE_TRAP;
		case 188: return GRAVATION_TRAP;
		case 189: return UNCALLED_TRAP;
		case 190: return EXPLODING_DICE_TRAP;
		case 191: return PERMACURSE_TRAP;
		case 192: return SHROUDED_IDENTITY_TRAP;
		case 193: return FEELER_GAUGES_TRAP;
		case 194: return LONG_SCREWUP_TRAP;
		case 195: return WING_YELLOW_CHANGER;
		case 196: return LIFE_SAVING_TRAP;
		case 197: return CURSEUSE_TRAP;
		case 198: return CUT_NUTRITION_TRAP;
		case 199: return SKILL_LOSS_TRAP;
		case 200: return AUTOPILOT_TRAP;
		case 201: return FORCE_TRAP;
		case 202: return MONSTER_GLYPH_TRAP;
		case 203: return CHANGING_DIRECTIVE_TRAP;
		case 204: return CONTAINER_KABOOM_TRAP;
		case 205: return STEAL_DEGRADE_TRAP;
		case 206: return LEFT_INVENTORY_TRAP;
		case 207: return FLUCTUATING_SPEED_TRAP;
		case 208: return TARMUSTROKINGNORA_TRAP;
		case 209: return FAILURE_TRAP;
		case 210: return BRIGHT_CYAN_SPELL_TRAP;
		case 211: return FREQUENTATION_SPAWN_TRAP;
		case 212: return PET_AI_TRAP;
		case 213: return SATAN_TRAP;
		case 214: return REMEMBERANCE_TRAP;
		case 215: return POKELIE_TRAP;
		case 216: return AUTOPICKUP_TRAP;
		case 217: return DYWYPI_TRAP;
		case 218: return SILVER_SPELL_TRAP;
		case 219: return METAL_SPELL_TRAP;
		case 220: return PLATINUM_SPELL_TRAP;
		case 221: return MANLER_TRAP;
		case 222: return DOORNING_TRAP;
		case 223: return NOWNSIBLE_TRAP;
		case 224: return ELM_STREET_TRAP;
		case 225: return MONNOISE_TRAP;
		case 226: return RANG_CALL_TRAP;
		case 227: return RECURRING_SPELL_LOSS_TRAP;
		case 228: return ANTITRAINING_TRAP;
		case 229: return TECHOUT_TRAP;
		case 230: return STAT_DECAY_TRAP;
		case 231: return MOVEMORK_TRAP;
		case 232: return BAD_PART_TRAP;
		case 233: return COMPLETELY_BAD_PART_TRAP;
		case 234: return EVIL_VARIANT_TRAP;
		case 235: return SANITY_TREBLE_TRAP;
		case 236: return STAT_DECREASE_TRAP;
		case 237: return SIMEOUT_TRAP;

	}

	return TRAP_PERCENTS; /* fail safe, should never happen */

}

int
randartmeleeweapon()
{
	int result;
reroll:
	result = rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1);
	if (objects[result].oc_skill >= P_BOW && objects[result].oc_skill <= P_CROSSBOW) goto reroll;
	if (objects[result].oc_skill >= -P_CROSSBOW && objects[result].oc_skill <= -P_BOW) goto reroll;
	if (objects[result].oc_skill >= -P_BOOMERANG && objects[result].oc_skill <= -P_DART) goto reroll;
	return result;
}

int
randartlauncher()
{
	int result;
reroll:
	result = rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1);
	if (!(objects[result].oc_skill >= P_BOW && objects[result].oc_skill <= P_CROSSBOW) ) goto reroll;
	return result;
}

int
randartmissile()
{
	int result;
reroll:
	result = rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1);
	if (!(objects[result].oc_skill >= -P_CROSSBOW && objects[result].oc_skill <= -P_BOW) && !(objects[result].oc_skill >= -P_BOOMERANG && objects[result].oc_skill <= -P_DART) ) goto reroll;
	return result;
}

int
randartsuit()
{
	int result;
reroll:
	result = rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS);
	if (!(objects[result].oc_armcat == ARM_SUIT)) goto reroll;
	return result;
}

int
randartshirt()
{
	int result;
reroll:
	result = rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS);
	if (!(objects[result].oc_armcat == ARM_SHIRT)) goto reroll;
	return result;
}

int
randartcloak()
{
	int result;
reroll:
	result = rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS);
	if (!(objects[result].oc_armcat == ARM_CLOAK)) goto reroll;
	return result;
}

int
randarthelm()
{
	int result;
reroll:
	result = rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS);
	if (!(objects[result].oc_armcat == ARM_HELM)) goto reroll;
	return result;
}

int
randartgloves()
{
	int result;
reroll:
	result = rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS);
	if (!(objects[result].oc_armcat == ARM_GLOVES)) goto reroll;
	return result;
}

int
randartshield()
{
	int result;
reroll:
	result = rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS);
	if (!(objects[result].oc_armcat == ARM_SHIELD)) goto reroll;
	return result;
}

int
randartboots()
{
	int result;
reroll:
	result = rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS);
	if (!(objects[result].oc_armcat == ARM_BOOTS)) goto reroll;
	return result;
}

int
randartring()
{
	int result;
	result = rnd_class(RIN_ADORNMENT,RIN_TELEPORT_CONTROL);
	return result;
}

int
randartamulet()
{
	int result;
	result = rnd_class(AMULET_OF_CHANGE,AMULET_OF_VULNERABILITY);
	return result;
}

int
randartimplant()
{
	int result;
	result = rnd_class(IMPLANT_OF_ABSORPTION,IMPLANT_OF_ENFORCING);
	return result;
}

int
randartwand()
{
	int result;
	result = rnd_class(WAN_LIGHT,WAN_PSYBEAM);
	return result;
}

int
randartspellbook()
{
	int result;
	result = rnd_class(SPE_FORCE_BOLT,SPE_PSYBEAM);
	return result;
}

int
randartmeleeweaponX()
{
	int result;
reroll:
	result = ORCISH_DAGGER + rn2(HAWAIIAN_SHIRT - ORCISH_DAGGER);
	if (objects[result].oc_skill >= P_BOW && objects[result].oc_skill <= P_CROSSBOW) goto reroll;
	if (objects[result].oc_skill >= -P_CROSSBOW && objects[result].oc_skill <= -P_BOW) goto reroll;
	if (objects[result].oc_skill >= -P_BOOMERANG && objects[result].oc_skill <= -P_DART) goto reroll;
	return result;
}

int
randartlauncherX()
{
	int result;
reroll:
	result = ORCISH_DAGGER + rn2(HAWAIIAN_SHIRT - ORCISH_DAGGER);
	if (!(objects[result].oc_skill >= P_BOW && objects[result].oc_skill <= P_CROSSBOW) ) goto reroll;
	return result;
}

int
randartmissileX()
{
	int result;
reroll:
	result = ORCISH_DAGGER + rn2(HAWAIIAN_SHIRT - ORCISH_DAGGER);
	if (!(objects[result].oc_skill >= -P_CROSSBOW && objects[result].oc_skill <= -P_BOW) && !(objects[result].oc_skill >= -P_BOOMERANG && objects[result].oc_skill <= -P_DART) ) goto reroll;
	return result;
}

int
randartsuitX()
{
	int result;
reroll:
	result = HAWAIIAN_SHIRT + rn2((LEVITATION_BOOTS + 1) - HAWAIIAN_SHIRT);
	if (!(objects[result].oc_armcat == ARM_SUIT)) goto reroll;
	return result;
}

int
randartshirtX()
{
	int result;
reroll:
	result = HAWAIIAN_SHIRT + rn2((LEVITATION_BOOTS + 1) - HAWAIIAN_SHIRT);
	if (!(objects[result].oc_armcat == ARM_SHIRT)) goto reroll;
	return result;
}

int
randartcloakX()
{
	int result;
reroll:
	result = HAWAIIAN_SHIRT + rn2((LEVITATION_BOOTS + 1) - HAWAIIAN_SHIRT);
	if (!(objects[result].oc_armcat == ARM_CLOAK)) goto reroll;
	return result;
}

int
randarthelmX()
{
	int result;
reroll:
	result = HAWAIIAN_SHIRT + rn2((LEVITATION_BOOTS + 1) - HAWAIIAN_SHIRT);
	if (!(objects[result].oc_armcat == ARM_HELM)) goto reroll;
	return result;
}

int
randartglovesX()
{
	int result;
reroll:
	result = HAWAIIAN_SHIRT + rn2((LEVITATION_BOOTS + 1) - HAWAIIAN_SHIRT);
	if (!(objects[result].oc_armcat == ARM_GLOVES)) goto reroll;
	return result;
}

int
randartshieldX()
{
	int result;
reroll:
	result = HAWAIIAN_SHIRT + rn2((LEVITATION_BOOTS + 1) - HAWAIIAN_SHIRT);
	if (!(objects[result].oc_armcat == ARM_SHIELD)) goto reroll;
	return result;
}

int
randartbootsX()
{
	int result;
reroll:
	result = HAWAIIAN_SHIRT + rn2((LEVITATION_BOOTS + 1) - HAWAIIAN_SHIRT);
	if (!(objects[result].oc_armcat == ARM_BOOTS)) goto reroll;
	return result;
}

int
randartringX()
{
	int result;
	result = RIN_ADORNMENT + rn2((RIN_TELEPORT_CONTROL + 1) - RIN_ADORNMENT);
	return result;
}

int
randartamuletX()
{
	int result;
	result = AMULET_OF_CHANGE + rn2((AMULET_OF_VULNERABILITY + 1) - AMULET_OF_CHANGE);
	return result;
}

int
randartimplantX()
{
	int result;
	result = IMPLANT_OF_ABSORPTION + rn2((IMPLANT_OF_ENFORCING + 1) - IMPLANT_OF_ABSORPTION);
	return result;
}

int
randartwandX()
{
	int result;
	result = WAN_LIGHT + rn2((WAN_PSYBEAM + 1) - WAN_LIGHT);
	return result;
}

int
randartspellbookX()
{
	int result;
	result = SPE_FORCE_BOLT + rn2((SPE_PSYBEAM + 1) - SPE_FORCE_BOLT);
	return result;
}

void
deacrandomintrinsic(amount)
register int amount;
{

	switch (rnd(131)) {

		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			u.uprops[DEAC_FIRE_RES].intrinsic += amount;
			pline("You are prevented from having fire resistance!");
			break;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			u.uprops[DEAC_COLD_RES].intrinsic += amount;
			pline("You are prevented from having cold resistance!");
			break;
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			u.uprops[DEAC_SLEEP_RES].intrinsic += amount;
			pline("You are prevented from having sleep resistance!");
			break;
		case 16:
		case 17:
			u.uprops[DEAC_DISINT_RES].intrinsic += amount;
			pline("You are prevented from having disintegration resistance!");
			break;
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
			u.uprops[DEAC_SHOCK_RES].intrinsic += amount;
			pline("You are prevented from having shock resistance!");
			break;
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
			u.uprops[DEAC_POISON_RES].intrinsic += amount;
			pline("You are prevented from having poison resistance!");
			break;
		case 28:
		case 29:
		case 30:
			u.uprops[DEAC_DRAIN_RES].intrinsic += amount;
			pline("You are prevented from having drain resistance!");
			break;
		case 31:
		case 32:
			u.uprops[DEAC_SICK_RES].intrinsic += amount;
			pline("You are prevented from having sickness resistance!");
			break;
		case 33:
		case 34:
			u.uprops[DEAC_ANTIMAGIC].intrinsic += amount;
			pline("You are prevented from having magic resistance!");
			break;
		case 35:
		case 36:
		case 37:
		case 38:
			u.uprops[DEAC_ACID_RES].intrinsic += amount;
			pline("You are prevented from having acid resistance!");
			break;
		case 39:
		case 40:
			u.uprops[DEAC_STONE_RES].intrinsic += amount;
			pline("You are prevented from having petrification resistance!");
			break;
		case 41:
			u.uprops[DEAC_FEAR_RES].intrinsic += amount;
			pline("You are prevented from having fear resistance!");
			break;
		case 42:
		case 43:
		case 44:
			u.uprops[DEAC_SEE_INVIS].intrinsic += amount;
			pline("You are prevented from having see invisible!");
			break;
		case 45:
		case 46:
		case 47:
			u.uprops[DEAC_TELEPAT].intrinsic += amount;
			pline("You are prevented from having telepathy!");
			break;
		case 48:
		case 49:
		case 50:
			u.uprops[DEAC_WARNING].intrinsic += amount;
			pline("You are prevented from having warning!");
			break;
		case 51:
		case 52:
		case 53:
			u.uprops[DEAC_SEARCHING].intrinsic += amount;
			pline("You are prevented from having automatic searching!");
			break;
		case 54:
			u.uprops[DEAC_CLAIRVOYANT].intrinsic += amount;
			pline("You are prevented from having clairvoyance!");
			break;
		case 55:
		case 56:
		case 57:
		case 58:
		case 59:
			u.uprops[DEAC_INFRAVISION].intrinsic += amount;
			pline("You are prevented from having infravision!");
			break;
		case 60:
			u.uprops[DEAC_DETECT_MONSTERS].intrinsic += amount;
			pline("You are prevented from having detect monsters!");
			break;
		case 61:
		case 62:
		case 63:
			u.uprops[DEAC_INVIS].intrinsic += amount;
			pline("You are prevented from having invisibility!");
			break;
		case 64:
			u.uprops[DEAC_DISPLACED].intrinsic += amount;
			pline("You are prevented from having displacement!");
			break;
		case 65:
		case 66:
		case 67:
			u.uprops[DEAC_STEALTH].intrinsic += amount;
			pline("You are prevented from having stealth!");
			break;
		case 68:
			u.uprops[DEAC_JUMPING].intrinsic += amount;
			pline("You are prevented from having jumping!");
			break;
		case 69:
		case 70:
		case 71:
			u.uprops[DEAC_TELEPORT_CONTROL].intrinsic += amount;
			pline("You are prevented from having teleport control!");
			break;
		case 72:
			u.uprops[DEAC_FLYING].intrinsic += amount;
			pline("You are prevented from having flying!");
			break;
		case 73:
			u.uprops[DEAC_MAGICAL_BREATHING].intrinsic += amount;
			pline("You are prevented from having magical breathing!");
			break;
		case 74:
			u.uprops[DEAC_PASSES_WALLS].intrinsic += amount;
			pline("You are prevented from having phasing!");
			break;
		case 75:
		case 76:
			u.uprops[DEAC_SLOW_DIGESTION].intrinsic += amount;
			pline("You are prevented from having slow digestion!");
			break;
		case 77:
			u.uprops[DEAC_HALF_SPDAM].intrinsic += amount;
			pline("You are prevented from having half spell damage!");
			break;
		case 78:
			u.uprops[DEAC_HALF_PHDAM].intrinsic += amount;
			pline("You are prevented from having half physical damage!");
			break;
		case 79:
		case 80:
		case 81:
		case 82:
		case 83:
			u.uprops[DEAC_REGENERATION].intrinsic += amount;
			pline("You are prevented from having regeneration!");
			break;
		case 84:
		case 85:
			u.uprops[DEAC_ENERGY_REGENERATION].intrinsic += amount;
			pline("You are prevented from having mana regeneration!");
			break;
		case 86:
		case 87:
		case 88:
			u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += amount;
			pline("You are prevented from having polymorph control!");
			break;
		case 89:
		case 90:
		case 91:
		case 92:
		case 93:
			u.uprops[DEAC_FAST].intrinsic += amount;
			pline("You are prevented from having speed!");
			break;
		case 94:
		case 95:
		case 96:
			u.uprops[DEAC_REFLECTING].intrinsic += amount;
			pline("You are prevented from having reflection!");
			break;
		case 97:
		case 98:
		case 99:
			u.uprops[DEAC_FREE_ACTION].intrinsic += amount;
			pline("You are prevented from having free action!");
			break;
		case 100:
			u.uprops[DEAC_HALLU_PARTY].intrinsic += amount;
			pline("You are prevented from hallu partying!");
			break;
		case 101:
			u.uprops[DEAC_DRUNKEN_BOXING].intrinsic += amount;
			pline("You are prevented from drunken boxing!");
			break;
		case 102:
			u.uprops[DEAC_STUNNOPATHY].intrinsic += amount;
			pline("You are prevented from having stunnopathy!");
			break;
		case 103:
			u.uprops[DEAC_NUMBOPATHY].intrinsic += amount;
			pline("You are prevented from having numbopathy!");
			break;
		case 104:
			u.uprops[DEAC_FREEZOPATHY].intrinsic += amount;
			pline("You are prevented from having freezopathy!");
			break;
		case 105:
			u.uprops[DEAC_STONED_CHILLER].intrinsic += amount;
			pline("You are prevented from being a stoned chiller!");
			break;
		case 106:
			u.uprops[DEAC_CORROSIVITY].intrinsic += amount;
			pline("You are prevented from having corrosivity!");
			break;
		case 107:
			u.uprops[DEAC_FEAR_FACTOR].intrinsic += amount;
			pline("You are prevented from having an increased fear factor!");
			break;
		case 108:
			u.uprops[DEAC_BURNOPATHY].intrinsic += amount;
			pline("You are prevented from having burnopathy!");
			break;
		case 109:
			u.uprops[DEAC_SICKOPATHY].intrinsic += amount;
			pline("You are prevented from having sickopathy!");
			break;
		case 110:
			u.uprops[DEAC_KEEN_MEMORY].intrinsic += amount;
			pline("You are prevented from having keen memory!");
			break;
		case 111:
			u.uprops[DEAC_THE_FORCE].intrinsic += amount;
			pline("You are prevented from using the force like a real jedi!");
			break;
		case 112:
			u.uprops[DEAC_SIGHT_BONUS].intrinsic += amount;
			pline("You are prevented from having extra sight!");
			break;
		case 113:
			u.uprops[DEAC_VERSUS_CURSES].intrinsic += amount;
			pline("You are prevented from having curse resistance!");
			break;
		case 114:
			u.uprops[DEAC_STUN_RES].intrinsic += amount;
			pline("You are prevented from having stun resistance!");
			break;
		case 115:
			u.uprops[DEAC_CONF_RES].intrinsic += amount;
			pline("You are prevented from having confusion resistance!");
			break;
		case 116:
			u.uprops[DEAC_DOUBLE_ATTACK].intrinsic += amount;
			pline("You are prevented from having double attacks!");
			break;
		case 117:
			u.uprops[DEAC_QUAD_ATTACK].intrinsic += amount;
			pline("You are prevented from having quad attacks!");
			break;
		case 118:
			u.uprops[DEAC_PSI_RES].intrinsic += amount;
			pline("You are prevented from having psi resistance!");
			break;
		case 119:
			u.uprops[DEAC_WONDERLEGS].intrinsic += amount;
			pline("You are prevented from having wonderlegs!");
			break;
		case 120:
			u.uprops[DEAC_GLIB_COMBAT].intrinsic += amount;
			pline("You are prevented from having glib combat!");
			break;
		case 121:
			u.uprops[DEAC_MANALEECH].intrinsic += amount;
			pline("You are prevented from having manaleech!");
			break;
		case 122:
			u.uprops[DEAC_DIMMOPATHY].intrinsic += amount;
			pline("You are prevented from having dimmopathy!");
			break;
		case 123:
			u.uprops[DEAC_PEACEVISION].intrinsic += amount;
			pline("You are prevented from having peacevision!");
			break;
		case 124:
			u.uprops[DEAC_CONT_RES].intrinsic += amount;
			pline("You are prevented from having contamination resistance!");
			break;
		case 125:
			u.uprops[DEAC_DISCOUNT_ACTION].intrinsic += amount;
			pline("You are prevented from having discount action!");
			break;
		case 126:
			u.uprops[DEAC_FULL_NUTRIENT].intrinsic += amount;
			pline("You are prevented from having full nutrients!");
			break;
		case 127:
			u.uprops[DEAC_TECHNICALITY].intrinsic += amount;
			pline("You are prevented from having technicality!");
			break;
		case 128:
			u.uprops[DEAC_SCENT_VIEW].intrinsic += amount;
			pline("You are prevented from having scent view!");
			break;
		case 129:
			u.uprops[DEAC_DIMINISHED_BLEEDING].intrinsic += amount;
			pline("You are prevented from having diminished bleeding!");
			break;
		case 130:
			u.uprops[DEAC_CONTROL_MAGIC].intrinsic += amount;
			pline("You are prevented from having control magic!");
			break;
		case 131:
			u.uprops[DEAC_EXP_BOOST].intrinsic += amount;
			pline("You are prevented from having EXP boost!");
			break;
	}

}

int
goodimplanteffect(obj)
struct obj *obj;
{
	switch (obj->shirtmessage % 55) {

		case 0:
			return DTBEEM_RES;
		case 1:
			return FIRE_RES;
		case 2:
			return COLD_RES;
		case 3:
			return SLEEP_RES;
		case 4:
			return DISINT_RES;
		case 5:
			return SHOCK_RES;
		case 6:
			return POISON_RES;
		case 7:
			return ACID_RES;
		case 8:
			return STONE_RES;
		case 9:
			return REGENERATION;
		case 10:
			return SEARCHING;
		case 11:
			return SEE_INVIS;
		case 12:
			return INVIS;
		case 13:
			return TELEPORT_CONTROL;
		case 14:
			return POLYMORPH_CONTROL;
		case 15:
			return STEALTH;
		case 16:
			return WARNING;
		case 17:
			return TELEPAT;
		case 18:
			return FAST;
		case 19:
			return PASSES_WALLS;
		case 20:
			return REFLECTING;
		case 21:
			return ANTIMAGIC;
		case 22:
			return ENERGY_REGENERATION;
		case 23:
			return MAGICAL_BREATHING;
		case 24:
			return HALF_SPDAM;
		case 25:
			return HALF_PHDAM;
		case 26:
			return SICK_RES;
		case 27:
			return DRAIN_RES;
		case 28:
			return FREE_ACTION;
		case 29:
			return FLYING;
		case 30:
			return SLOW_DIGESTION;
		case 31:
			return INFRAVISION;
		case 32:
			return FEAR_RES;
		case 33:
			return DISPLACED;
		case 34:
			return SWIMMING;
		case 35:
			return KEEN_MEMORY;
		case 36:
			return THE_FORCE;
		case 37:
			return TRAP_REVEALING;
		case 38:
			return CLAIRVOYANT;
		case 39:
			return SIGHT_BONUS;
		case 40:
			return VERSUS_CURSES;
		case 41:
			return STUN_RES;
		case 42:
			return CONF_RES;
		case 43:
			return EXTRA_WEAPON_PRACTICE;
		case 44:
			return PSI_RES;
		case 45:
			return MANALEECH;
		case 46:
			return PEACEVISION;
		case 47:
			return CONT_RES;
		case 48:
			return DISCOUNT_ACTION;
		case 49:
			return FULL_NUTRIENT;
		case 50:
			return TECHNICALITY;
		case 51:
			return SCENT_VIEW;
		case 52:
			return DIMINISHED_BLEEDING;
		case 53:
			return CONTROL_MAGIC;
		case 54:
			return EXP_BOOST;
		default:
			return POISON_RES;

	}

}

void
getnastytrapintrinsic()
{
			switch (rnd(235)) {

				case 1: 
				    SpeedBug |= FROMOUTSIDE; break;
				case 2: 
				    MenuBug |= FROMOUTSIDE; break;
				case 3: 
				    RMBLoss |= FROMOUTSIDE; break;
				case 4: 
				    DisplayLoss |= FROMOUTSIDE; break;
				case 5: 
				    SpellLoss |= FROMOUTSIDE; break;
				case 6: 
				    YellowSpells |= FROMOUTSIDE; break;
				case 7: 
				    AutoDestruct |= FROMOUTSIDE; break;
				case 8: 
				    MemoryLoss |= FROMOUTSIDE; break;
				case 9: 
				    InventoryLoss |= FROMOUTSIDE; break;
				case 10: 
				    BlackNgWalls |= FROMOUTSIDE; break;
				case 11: 
				    Superscroller |= FROMOUTSIDE; break;
				case 12: 
				    FreeHandLoss |= FROMOUTSIDE; break;
				case 13: 
				    Unidentify |= FROMOUTSIDE; break;
				case 14: 
				    Thirst |= FROMOUTSIDE; break;
				case 15: 
				    LuckLoss |= FROMOUTSIDE; break;
				case 16: 
				    ShadesOfGrey |= FROMOUTSIDE; break;
				case 17: 
				    FaintActive |= FROMOUTSIDE; break;
				case 18: 
				    Itemcursing |= FROMOUTSIDE; break;
				case 19: 
				    DifficultyIncreased |= FROMOUTSIDE; break;
				case 20: 
				    Deafness |= FROMOUTSIDE; break;
				case 21: 
				    CasterProblem |= FROMOUTSIDE; break;
				case 22: 
				    WeaknessProblem |= FROMOUTSIDE; break;
				case 23: 
				    RotThirteen |= FROMOUTSIDE; break;
				case 24: 
				    BishopGridbug |= FROMOUTSIDE; break;
				case 25: 
				    ConfusionProblem |= FROMOUTSIDE; break;
				case 26: 
				    NoDropProblem |= FROMOUTSIDE; break;
				case 27: 
				    DSTWProblem |= FROMOUTSIDE; break;
				case 28: 
				    StatusTrapProblem |= FROMOUTSIDE; break;
				case 29: 
				    AlignmentProblem |= FROMOUTSIDE; break;
				case 30: 
				    StairsProblem |= FROMOUTSIDE; break;
				case 31: 
				    UninformationProblem |= FROMOUTSIDE; break;
				case 32: 
				    IntrinsicLossProblem |= FROMOUTSIDE; break;
				case 33: 
				    BloodLossProblem |= FROMOUTSIDE; break;
				case 34: 
				    BadEffectProblem |= FROMOUTSIDE; break;
				case 35: 
				    TrapCreationProblem |= FROMOUTSIDE; break;
				case 36: 
				    AutomaticVulnerabilitiy |= FROMOUTSIDE; break;
				case 37: 
				    TeleportingItems |= FROMOUTSIDE; break;
				case 38: 
				    NastinessProblem |= FROMOUTSIDE; break;
				case 39: 
				    RecurringAmnesia |= FROMOUTSIDE; break;
				case 40: 
				    BigscriptEffect |= FROMOUTSIDE; break;
				case 41: 
				    BankTrapEffect |= FROMOUTSIDE; break;
				case 42: 
				    MapTrapEffect |= FROMOUTSIDE; break;
				case 43: 
				    TechTrapEffect |= FROMOUTSIDE; break;
				case 44: 
				    RecurringDisenchant |= FROMOUTSIDE; break;
				case 45: 
				    verisiertEffect |= FROMOUTSIDE; break;
				case 46: 
				    ChaosTerrain |= FROMOUTSIDE; break;
				case 47: 
				    Muteness |= FROMOUTSIDE; break;
				case 48: 
				    EngravingDoesntWork |= FROMOUTSIDE; break;
				case 49: 
				    MagicDeviceEffect |= FROMOUTSIDE; break;
				case 50: 
				    BookTrapEffect |= FROMOUTSIDE; break;
				case 51: 
				    LevelTrapEffect |= FROMOUTSIDE; break;
				case 52: 
				    QuizTrapEffect |= FROMOUTSIDE; break;
				case 53: 
				    CaptchaProblem |= FROMOUTSIDE; break;
				case 54: 
				    FarlookProblem |= FROMOUTSIDE; break;
				case 55: 
				    RespawnProblem |= FROMOUTSIDE; break;
				case 56: 
				    FastMetabolismEffect |= FROMOUTSIDE; break;
				case 57: 
				    NoReturnEffect |= FROMOUTSIDE; break;
				case 58: 
				    AlwaysEgotypeMonsters |= FROMOUTSIDE; break;
				case 59: 
				    TimeGoesByFaster |= FROMOUTSIDE; break;
				case 60: 
				    FoodIsAlwaysRotten |= FROMOUTSIDE; break;
				case 61: 
				    AllSkillsUnskilled |= FROMOUTSIDE; break;
				case 62: 
				    AllStatsAreLower |= FROMOUTSIDE; break;
				case 63: 
				    PlayerCannotTrainSkills |= FROMOUTSIDE; break;
				case 64: 
				    PlayerCannotExerciseStats |= FROMOUTSIDE; break;
				case 65: 
				    TurnLimitation |= FROMOUTSIDE; break;
				case 66: 
				    WeakSight |= FROMOUTSIDE; break;
				case 67: 
				    RandomMessages |= FROMOUTSIDE; break;
				case 68: 
				    Desecration |= FROMOUTSIDE; break;
				case 69: 
				    StarvationEffect |= FROMOUTSIDE; break;
				case 70: 
				    NoDropsEffect |= FROMOUTSIDE; break;
				case 71: 
				    LowEffects |= FROMOUTSIDE; break;
				case 72: 
				    InvisibleTrapsEffect |= FROMOUTSIDE; break;
				case 73: 
				    GhostWorld |= FROMOUTSIDE; break;
				case 74: 
				    Dehydration |= FROMOUTSIDE; break;
				case 75: 
				    HateTrapEffect |= FROMOUTSIDE; break;
				case 76: 
				    TotterTrapEffect |= FROMOUTSIDE; break;
				case 77: 
				    Nonintrinsics |= FROMOUTSIDE; break;
				case 78: 
				    Dropcurses |= FROMOUTSIDE; break;
				case 79: 
				    Nakedness |= FROMOUTSIDE; break;
				case 80: 
				    Antileveling |= FROMOUTSIDE; break;
				case 81: 
				    ItemStealingEffect |= FROMOUTSIDE; break;
				case 82: 
				    Rebellions |= FROMOUTSIDE; break;
				case 83: 
				    CrapEffect |= FROMOUTSIDE; break;
				case 84: 
				    ProjectilesMisfire |= FROMOUTSIDE; break;
				case 85: 
				    WallTrapping |= FROMOUTSIDE; break;
				case 86: 
				    DisconnectedStairs |= FROMOUTSIDE; break;
				case 87: 
				    InterfaceScrewed |= FROMOUTSIDE; break;
				case 88: 
				    Bossfights |= FROMOUTSIDE; break;
				case 89: 
				    EntireLevelMode |= FROMOUTSIDE; break;
				case 90: 
				    BonesLevelChange |= FROMOUTSIDE; break;
				case 91: 
				    AutocursingEquipment |= FROMOUTSIDE; break;
				case 92: 
				    HighlevelStatus |= FROMOUTSIDE; break;
				case 93: 
				    SpellForgetting |= FROMOUTSIDE; break;
				case 94: 
				    SoundEffectBug |= FROMOUTSIDE; break;
				case 95: 
				    TimerunBug |= FROMOUTSIDE; break;
				case 96:
				    LootcutBug |= FROMOUTSIDE; break;
				case 97:
				    MonsterSpeedBug |= FROMOUTSIDE; break;
				case 98:
				    ScalingBug |= FROMOUTSIDE; break;
				case 99:
				    EnmityBug |= FROMOUTSIDE; break;
				case 100:
				    WhiteSpells |= FROMOUTSIDE; break;
				case 101:
				    CompleteGraySpells |= FROMOUTSIDE; break;
				case 102:
				    QuasarVision |= FROMOUTSIDE; break;
				case 103:
				    MommaBugEffect |= FROMOUTSIDE; break;
				case 104:
				    HorrorBugEffect |= FROMOUTSIDE; break;
				case 105:
				    ArtificerBug |= FROMOUTSIDE; break;
				case 106:
				    WereformBug |= FROMOUTSIDE; break;
				case 107:
				    NonprayerBug |= FROMOUTSIDE; break;
				case 108:
				    EvilPatchEffect |= FROMOUTSIDE; break;
				case 109:
				    HardModeEffect |= FROMOUTSIDE; break;
				case 110:
				    SecretAttackBug |= FROMOUTSIDE; break;
				case 111:
				    EaterBugEffect |= FROMOUTSIDE; break;
				case 112:
				    CovetousnessBug |= FROMOUTSIDE; break;
				case 113:
				    NotSeenBug |= FROMOUTSIDE; break;
				case 114:
				    DarkModeBug |= FROMOUTSIDE; break;
				case 115:
				    AntisearchEffect |= FROMOUTSIDE; break;
				case 116:
				    HomicideEffect |= FROMOUTSIDE; break;
				case 117:
				    NastynationBug |= FROMOUTSIDE; break;
				case 118:
				    WakeupCallBug |= FROMOUTSIDE; break;
				case 119:
				    GrayoutBug |= FROMOUTSIDE; break;
				case 120:
				    GrayCenterBug |= FROMOUTSIDE; break;
				case 121:
				    CheckerboardBug |= FROMOUTSIDE; break;
				case 122:
				    ClockwiseSpinBug |= FROMOUTSIDE; break;
				case 123:
				    CounterclockwiseSpin |= FROMOUTSIDE; break;
				case 124:
				    LagBugEffect |= FROMOUTSIDE; break;
				case 125:
				    BlesscurseEffect |= FROMOUTSIDE; break;
				case 126:
				    DeLightBug |= FROMOUTSIDE; break;
				case 127:
				    DischargeBug |= FROMOUTSIDE; break;
				case 128:
				    TrashingBugEffect |= FROMOUTSIDE; break;
				case 129:
				    FilteringBug |= FROMOUTSIDE; break;
				case 130:
				    DeformattingBug |= FROMOUTSIDE; break;
				case 131:
				    FlickerStripBug |= FROMOUTSIDE; break;
				case 132:
				    UndressingEffect |= FROMOUTSIDE; break;
				case 133:
				    Hyperbluewalls |= FROMOUTSIDE; break;
				case 134:
				    NoliteBug |= FROMOUTSIDE; break;
				case 135:
				    ParanoiaBugEffect |= FROMOUTSIDE; break;
				case 136:
				    FleecescriptBug |= FROMOUTSIDE; break;
				case 137:
				    InterruptEffect |= FROMOUTSIDE; break;
				case 138:
				    DustbinBug |= FROMOUTSIDE; break;
				case 139:
				    ManaBatteryBug |= FROMOUTSIDE; break;
				case 140:
				    Monsterfingers |= FROMOUTSIDE; break;
				case 141:
				    MiscastBug |= FROMOUTSIDE; break;
				case 142:
				    MessageSuppression |= FROMOUTSIDE; break;
				case 143:
				    StuckAnnouncement |= FROMOUTSIDE; break;
				case 144:
				    BloodthirstyEffect |= FROMOUTSIDE; break;
				case 145:
				    MaximumDamageBug |= FROMOUTSIDE; break;
				case 146:
				    LatencyBugEffect |= FROMOUTSIDE; break;
				case 147:
				    StarlitBug |= FROMOUTSIDE; break;
				case 148:
				    KnowledgeBug |= FROMOUTSIDE; break;
				case 149:
				    HighscoreBug |= FROMOUTSIDE; break;
				case 150:
				    PinkSpells |= FROMOUTSIDE; break;
				case 151:
				    GreenSpells |= FROMOUTSIDE; break;
				case 152:
				    EvencoreEffect |= FROMOUTSIDE; break;
				case 153:
				    UnderlayerBug |= FROMOUTSIDE; break;
				case 154:
				    DamageMeterBug |= FROMOUTSIDE; break;
				case 155:
				    ArbitraryWeightBug |= FROMOUTSIDE; break;
				case 156:
				    FuckedInfoBug |= FROMOUTSIDE; break;
				case 157:
				    BlackSpells |= FROMOUTSIDE; break;
				case 158:
				    CyanSpells |= FROMOUTSIDE; break;
				case 159:
				    HeapEffectBug |= FROMOUTSIDE; break;
				case 160:
				    BlueSpells |= FROMOUTSIDE; break;
				case 161:
				    TronEffect |= FROMOUTSIDE; break;
				case 162:
				    RedSpells |= FROMOUTSIDE; break;
				case 163:
				    TooHeavyEffect |= FROMOUTSIDE; break;
				case 164:
				    ElongationBug |= FROMOUTSIDE; break;
				case 165:
				    WrapoverEffect |= FROMOUTSIDE; break;
				case 166:
				    DestructionEffect |= FROMOUTSIDE; break;
				case 167:
				    MeleePrefixBug |= FROMOUTSIDE; break;
				case 168:
				    AutomoreBug |= FROMOUTSIDE; break;
				case 169:
				    UnfairAttackBug |= FROMOUTSIDE; break;
				case 170:
				    OrangeSpells |= FROMOUTSIDE; break;
				case 171:
				    VioletSpells |= FROMOUTSIDE; break;
				case 172:
				    LongingEffect |= FROMOUTSIDE; break;
				case 173:
				    CursedParts |= FROMOUTSIDE; break;
				case 174:
				    Quaversal |= FROMOUTSIDE; break;
				case 175:
				    AppearanceShuffling |= FROMOUTSIDE; break;
				case 176:
				    BrownSpells |= FROMOUTSIDE; break;
				case 177:
				    Choicelessness |= FROMOUTSIDE; break;
				case 178:
				    Goldspells |= FROMOUTSIDE; break;
				case 179:
				    Deprovement |= FROMOUTSIDE; break;
				case 180:
				    InitializationFail |= FROMOUTSIDE; break;
				case 181:
				    GushlushEffect |= FROMOUTSIDE; break;
				case 182:
				    SoiltypeEffect |= FROMOUTSIDE; break;
				case 183:
				    DangerousTerrains |= FROMOUTSIDE; break;
				case 184:
				    FalloutEffect |= FROMOUTSIDE; break;
				case 185:
				    MojibakeEffect |= FROMOUTSIDE; break;
				case 186:
				    GravationEffect |= FROMOUTSIDE; break;
				case 187:
				    UncalledEffect |= FROMOUTSIDE; break;
				case 188:
				    ExplodingDiceEffect |= FROMOUTSIDE; break;
				case 189:
				    PermacurseEffect |= FROMOUTSIDE; break;
				case 190:
				    ShroudedIdentity |= FROMOUTSIDE; break;
				case 191:
				    FeelerGauges |= FROMOUTSIDE; break;
				case 192:
				    LongScrewup |= FROMOUTSIDE; break;
				case 193:
				    WingYellowChange |= FROMOUTSIDE; break;
				case 194:
				    LifeSavingBug |= FROMOUTSIDE; break;
				case 195:
				    CurseuseEffect |= FROMOUTSIDE; break;
				case 196:
				    CutNutritionEffect |= FROMOUTSIDE; break;
				case 197:
				    SkillLossEffect |= FROMOUTSIDE; break;
				case 198:
				    AutopilotEffect |= FROMOUTSIDE; break;
				case 199:
				    MysteriousForceActive |= FROMOUTSIDE; break;
				case 200:
				    MonsterGlyphChange |= FROMOUTSIDE; break;
				case 201:
				    ChangingDirectives |= FROMOUTSIDE; break;
				case 202:
				    ContainerKaboom |= FROMOUTSIDE; break;
				case 203:
				    StealDegrading |= FROMOUTSIDE; break;
				case 204:
				    LeftInventoryBug |= FROMOUTSIDE; break;
				case 205:
				    FluctuatingSpeed |= FROMOUTSIDE; break;
				case 206:
				    TarmuStrokingNora |= FROMOUTSIDE; break;
				case 207:
				    FailureEffects |= FROMOUTSIDE; break;
				case 208:
				    BrightCyanSpells |= FROMOUTSIDE; break;
				case 209:
				    FrequentationSpawns |= FROMOUTSIDE; break;
				case 210:
				    PetAIScrewed |= FROMOUTSIDE; break;
				case 211:
				    SatanEffect |= FROMOUTSIDE; break;
				case 212:
				    RememberanceEffect |= FROMOUTSIDE; break;
				case 213:
				    PokelieEffect |= FROMOUTSIDE; break;
				case 214:
				    AlwaysAutopickup |= FROMOUTSIDE; break;
				case 215:
				    DywypiProblem |= FROMOUTSIDE; break;
				case 216:
				    SilverSpells |= FROMOUTSIDE; break;
				case 217:
				    MetalSpells |= FROMOUTSIDE; break;
				case 218:
				    PlatinumSpells |= FROMOUTSIDE; break;
				case 219:
				    ManlerEffect |= FROMOUTSIDE; break;
				case 220:
				    DoorningEffect |= FROMOUTSIDE; break;
				case 221:
				    NownsibleEffect |= FROMOUTSIDE; break;
				case 222:
				    ElmStreetEffect |= FROMOUTSIDE; break;
				case 223:
				    MonnoiseEffect |= FROMOUTSIDE; break;
				case 224:
				    RangCallEffect |= FROMOUTSIDE; break;
				case 225:
				    RecurringSpellLoss |= FROMOUTSIDE; break;
				case 226:
				    AntitrainingEffect |= FROMOUTSIDE; break;
				case 227:
				    TechoutBug |= FROMOUTSIDE; break;
				case 228:
				    StatDecay |= FROMOUTSIDE; break;
				case 229:
				    Movemork |= FROMOUTSIDE; break;
				case 230:
				    BadPartBug |= FROMOUTSIDE; break;
				case 231:
				    CompletelyBadPartBug |= FROMOUTSIDE; break;
				case 232:
				    EvilVariantActive |= FROMOUTSIDE; break;
				case 233:
				    SanityTrebleEffect |= FROMOUTSIDE; break;
				case 234:
				    StatDecreaseBug |= FROMOUTSIDE; break;
				case 235:
				    SimeoutBug |= FROMOUTSIDE; break;


			}

}

void
randomfeminismtrap(duration)
int duration;
{
	switch (rnd(31)) {
		case 1:
			pline("You can already imagine the farting noises you're gonna hear.");
			FemaleTrapSarah += duration;
			break;
		case 2:
			pline("Suddenly you feel a little confused, and also feel like stroking the sexy butt cheeks of a woman in wooden sandals.");
			FemaleTrapClaudia += duration;
			break;
		case 3:
			pline("You'll certainly like to listen to the disgusting toilet noises.");
			FemaleTrapLudgera += duration;
			break;
		case 4:
			pline("You feel like being kicked by sexy girls and cleaning their shoes.");
			FemaleTrapKati += duration;
			break;
		case 5:
			pline("All the girls want to use their sexy butt cheeks as weapons now!");
			FemaleTrapMeltem += duration;
			break;
		case 6:
			pline("Now, the dungeon will be more feminine for a while!");
			FemaleTrapFemmy += duration;
			break;
		case 7:
			pline("Your shins can expect to get kicked repeatedly by all the girls and women now!");
			FemaleTrapMadeleine += duration;
			break;
		case 8:
			pline("Green is the new favorite color, it seems!");
			FemaleTrapMarlena += duration;
			break;
		case 9:
			pline("Suddenly, you feel that you're going to step into a heap of shit.");
			FemaleTrapAnastasia += duration;
			break;
		case 10:
			pline("Your butt cheeks suddenly feel very tender, and in fact, a similar thing is happening to your entire body!");
			FemaleTrapJessica += duration;
			break;
		case 11:
			pline("You suddenly have a very grating, aggravating voice, and you start to emit a beguiling odor! In fact, you're super sexy and sweet now!");
			FemaleTrapSolvejg += duration;
			break;
		case 12:
			pline("All the female denizens of the dungeon will show you their true power, and it will happen very soon!");
			FemaleTrapWendy += duration;
			break;
		case 13:
			pline("You feel that the girls and women are getting ready to use their sexy butts as weapons.");
			FemaleTrapKatharina += duration;
			break;
		case 14:
			pline("You long for beautiful sexy women with tender butt cheeks and lovely high heels.");
			FemaleTrapElena += duration;
			break;
		case 15:
			pline("You feel that you'll want to use the toilet more often. Also, somehow your physique seems weaker now...");
			FemaleTrapThai += duration;
			break;
		case 16:
			pline("A ghostly girl (named Elif) starts following you around, and apparently she wants to play with you!");
			FemaleTrapElif += duration;
			break;
		case 17:
			pline("You feel that you angered the womanhood. If you now hit a woman, you will be hit with retribution!");
			FemaleTrapNadja += duration;
			break;
		case 18:
			pline("You just know that your legs are going to be ripped open by very sharp-edged combat boot heels.");
			FemaleTrapSandra += duration;
			break;
		case 19:
			pline("You gotta dance! You've suddenly become super sexy and attractive, and neither sleep nor paralysis can stop you in your tracks. And you can kick your enemies to stomp their toes flat. But if you ever stand still for too long, a bunch of bloodthirsty female painted toenails is going to hurt your beautiful skin, so make sure you keep moving!");
			FemaleTrapNatalje += duration;
			break;
		case 20:
			pline("Lots of shreds of skin will be scraped off your shins, and the girls will enjoy it.");
			FemaleTrapJeanetta += duration;
			break;
		case 21:
			pline("You feel that people are building toilets for you to use.");
			FemaleTrapYvonne += duration;
			break;
		case 22:
			pline("Your sexy butt signals that it wants to produce beautiful farting noises!");
			FemaleTrapMaurah += duration;
			break;
		case 23:
			pline("You fear that the women are going to crush you with a hug.");
			FemaleTrapNelly += duration;
			break;
		case 24:
			pline("It seems that the women are running faster.");
			FemaleTrapEveline += duration;
			break;
		case 25:
			pline("All the women want to knee you in the delicate nuts now!");
			FemaleTrapKarin += duration;
			break;
		case 26:
			pline("Oh no, your shins are almost broken!");
			FemaleTrapJuen += duration;
			break;
		case 27:
			pline("Urgh, you can already smell the stench of stinking cigarettes!");
			FemaleTrapKristina += duration;
			break;
		case 28:
			pline("These damn people want to use your precious clothing to brush off their dirty shoes!");
			FemaleTrapLou += duration;
			break;
		case 29:
			pline("The girls feel like kicking your hands bloodily with their sneakers!");
			FemaleTrapAlmut += duration;
			break;
		case 30:
			pline("Now you'll be subjected to Julietta's torture, and she'll subject you to random punishments from time to time.");
			FemaleTrapJulietta += duration;
			break;
		case 31:
			pline("Oh no, the master of nasty traps is laying out her snares to get you!");
			FemaleTrapArabella += duration;
			break;
	}
}

void
randomnastytrapeffect(duration, blackngvar)
int duration, blackngvar;
{
		if (LongScrewup || u.uprops[LONG_SCREWUP].extrinsic || have_longscrewupstone()) duration *= 20;

		switch (rnd(235)) {

			case 1: RMBLoss += rnz(duration); break;
			case 2: NoDropProblem += rnz(duration); break;
			case 3: DSTWProblem += rnz(duration); break;
			case 4: StatusTrapProblem += rnz(duration); 
				if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
				if (HStun) set_itimeout(&HeavyStunned, HStun);
				if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
				if (HFeared) set_itimeout(&HeavyFeared, HFeared);
				if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
				if (HBurned) set_itimeout(&HeavyBurned, HBurned);
				if (HDimmed) set_itimeout(&HeavyDimmed, HDimmed);
				if (Blinded) set_itimeout(&HeavyBlind, Blinded);
				if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);
				break;
			case 5: Superscroller += rnz(duration * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10)); 
				(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
				break;
			case 6: MenuBug += rnz(duration); break;
			case 7: FreeHandLoss += rnz(duration); break;
			case 8: Unidentify += rnz(duration); break;
			case 9: Thirst += rnz(duration); break;
			case 10: LuckLoss += rnz(duration); break;
			case 11: ShadesOfGrey += rnz(duration); break;
			case 12: FaintActive += rnz(duration); break;
			case 13: Itemcursing += rnz(duration); break;
			case 14: DifficultyIncreased += rnz(duration); break;
			case 15: Deafness += rnz(duration); flags.soundok = 0; break;
			case 16: CasterProblem += rnz(duration); break;
			case 17: WeaknessProblem += rnz(duration); break;
			case 18: RotThirteen += rnz(duration); break;
			case 19: BishopGridbug += rnz(duration); break;
			case 20: UninformationProblem += rnz(duration); break;
			case 21: StairsProblem += rnz(duration); break;
			case 22: AlignmentProblem += rnz(duration); break;
			case 23: ConfusionProblem += rnz(duration); break;
			case 24: SpeedBug += rnz(duration); break;
			case 25: DisplayLoss += rnz(duration); break;
			case 26: SpellLoss += rnz(duration); break;
			case 27: YellowSpells += rnz(duration); break;
			case 28: AutoDestruct += rnz(duration); break;
			case 29: MemoryLoss += rnz(duration); break;
			case 30: InventoryLoss += rnz(duration); break;
			case 31: {

				if (BlackNgWalls) break;

				BlackNgWalls = blackngvar;
				if (BlackNgWalls < 100) BlackNgWalls = 100;
				(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
				break;
			}
			case 32: IntrinsicLossProblem += rnz(duration); break;
			case 33: BloodLossProblem += rnz(duration); break;
			case 34: BadEffectProblem += rnz(duration); break;
			case 35: TrapCreationProblem += rnz(duration); break;
			case 36: AutomaticVulnerabilitiy += rnz(duration); break;
			case 37: TeleportingItems += rnz(duration); break;
			case 38: NastinessProblem += rnz(duration); break;
			case 39: CaptchaProblem += rnz(duration); break;
			case 40: FarlookProblem += rnz(duration); break;
			case 41: RespawnProblem += rnz(duration); break;

			case 42: RecurringAmnesia += rnz(duration); break;
			case 43: BigscriptEffect += rnz(duration); break;
			case 44: {
				BankTrapEffect += rnz(duration);
				if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));
				u.bankcashamount += u.ugold;
				u.ugold = 0;

				break;
			}
			case 45: MapTrapEffect += rnz(duration); break;
			case 46: TechTrapEffect += rnz(duration); break;
			case 47: RecurringDisenchant += rnz(duration); break;
			case 48: verisiertEffect += rnz(duration); break;
			case 49: ChaosTerrain += rnz(duration); break;
			case 50: Muteness += rnz(duration); break;
			case 51: EngravingDoesntWork += rnz(duration); break;
			case 52: MagicDeviceEffect += rnz(duration); break;
			case 53: BookTrapEffect += rnz(duration); break;
			case 54: LevelTrapEffect += rnz(duration); break;
			case 55: QuizTrapEffect += rnz(duration); break;
			case 56: FastMetabolismEffect += rnz(duration); break;
			case 57: NoReturnEffect += rnz(duration); break;
			case 58: AlwaysEgotypeMonsters += rnz(duration); break;
			case 59: TimeGoesByFaster += rnz(duration); break;
			case 60: FoodIsAlwaysRotten += rnz(duration); break;
			case 61: AllSkillsUnskilled += rnz(duration); break;
			case 62: AllStatsAreLower += rnz(duration); break;
			case 63: PlayerCannotTrainSkills += rnz(duration); break;
			case 64: PlayerCannotExerciseStats += rnz(duration); break;
			case 65: TurnLimitation += rnz(duration); break;
			case 66: WeakSight += rnz(duration); break;
			case 67: RandomMessages += rnz(duration); break;

			case 68: Desecration += rnz(duration); break;
			case 69: StarvationEffect += rnz(duration); break;
			case 70: NoDropsEffect += rnz(duration); break;
			case 71: LowEffects += rnz(duration); break;
			case 72: InvisibleTrapsEffect += rnz(duration); break;
			case 73: GhostWorld += rnz(duration); break;
			case 74: Dehydration += rnz(duration); break;
			case 75: HateTrapEffect += rnz(duration); break;
			case 76: TotterTrapEffect += rnz(duration); break;
			case 77: Nonintrinsics += rnz(duration); break;
			case 78: Dropcurses += rnz(duration); break;
			case 79: Nakedness += rnz(duration); break;
			case 80: Antileveling += rnz(duration); break;
			case 81: ItemStealingEffect += rnz(duration); break;
			case 82: Rebellions += rnz(duration); break;
			case 83: CrapEffect += rnz(duration); break;
			case 84: ProjectilesMisfire += rnz(duration); break;
			case 85: WallTrapping += rnz(duration); break;
			case 86: DisconnectedStairs += rnz(duration); break;
			case 87: InterfaceScrewed += rnz(duration); break;
			case 88: Bossfights += rnz(duration); break;
			case 89: EntireLevelMode += rnz(duration); break;
			case 90: BonesLevelChange += rnz(duration); break;
			case 91: AutocursingEquipment += rnz(duration); break;
			case 92: HighlevelStatus += rnz(duration); break;
			case 93: SpellForgetting += rnz(duration); break;
			case 94: SoundEffectBug += rnz(duration); break;
			case 95: TimerunBug += rnz(duration); break;
			case 96: LootcutBug += rnz(duration); break;
			case 97: MonsterSpeedBug += rnz(duration); break;
			case 98: ScalingBug += rnz(duration); break;
			case 99: EnmityBug += rnz(duration); break;
			case 100: WhiteSpells += rnz(duration); break;
			case 101: CompleteGraySpells += rnz(duration); break;
			case 102: QuasarVision += rnz(duration); break;
			case 103: MommaBugEffect += rnz(duration); break;
			case 104: HorrorBugEffect += rnz(duration); break;
			case 105: ArtificerBug += rnz(duration); break;
			case 106: WereformBug += rnz(duration); break;
			case 107: NonprayerBug += rnz(duration); break;
			case 108: EvilPatchEffect += rnz(duration); break;
			case 109: HardModeEffect += rnz(duration); break;
			case 110: SecretAttackBug += rnz(duration); break;
			case 111: EaterBugEffect += rnz(duration); break;
			case 112: CovetousnessBug += rnz(duration); break;
			case 113: NotSeenBug += rnz(duration); break;
			case 114: DarkModeBug += rnz(duration); break;
			case 115: AntisearchEffect += rnz(duration); break;
			case 116: HomicideEffect += rnz(duration); break;
			case 117: NastynationBug += rnz(duration); break;
			case 118: WakeupCallBug += rnz(duration); break;
			case 119: GrayoutBug += rnz(duration); break;
			case 120: GrayCenterBug += rnz(duration); break;
			case 121: CheckerboardBug += rnz(duration); break;
			case 122: ClockwiseSpinBug += rnz(duration); break;
			case 123: CounterclockwiseSpin += rnz(duration); break;
			case 124: LagBugEffect += rnz(duration); break;
			case 125: BlesscurseEffect += rnz(duration); break;
			case 126: DeLightBug += rnz(duration); break;
			case 127: DischargeBug += rnz(duration); break;
			case 128: TrashingBugEffect += rnz(duration); break;
			case 129: FilteringBug += rnz(duration); break;
			case 130: DeformattingBug += rnz(duration); break;
			case 131: FlickerStripBug += rnz(duration); break;
			case 132: UndressingEffect += rnz(duration); break;
			case 133: Hyperbluewalls += rnz(duration); break;
			case 134: NoliteBug += rnz(duration); break;
			case 135: ParanoiaBugEffect += rnz(duration); break;
			case 136: FleecescriptBug += rnz(duration); break;
			case 137: InterruptEffect += rnz(duration); break;
			case 138: DustbinBug += rnz(duration); break;
			case 139: ManaBatteryBug += rnz(duration); break;
			case 140: Monsterfingers += rnz(duration); break;
			case 141: MiscastBug += rnz(duration); break;
			case 142: MessageSuppression += rnz(duration); break;
			case 143: StuckAnnouncement += rnz(duration); break;
			case 144: BloodthirstyEffect += rnz(duration); break;
			case 145: MaximumDamageBug += rnz(duration); break;
			case 146: LatencyBugEffect += rnz(duration); break;
			case 147: StarlitBug += rnz(duration); break;
			case 148: KnowledgeBug += rnz(duration); break;
			case 149: HighscoreBug += rnz(duration); break;
			case 150: PinkSpells += rnz(duration); break;
			case 151: GreenSpells += rnz(duration); break;
			case 152: EvencoreEffect += rnz(duration); break;
			case 153: UnderlayerBug += rnz(duration); break;
			case 154: DamageMeterBug += rnz(duration); break;
			case 155: ArbitraryWeightBug += rnz(duration); break;
			case 156: FuckedInfoBug += rnz(duration); break;
			case 157: BlackSpells += rnz(duration); break;
			case 158: CyanSpells += rnz(duration); break;
			case 159: HeapEffectBug += rnz(duration); break;
			case 160: BlueSpells += rnz(duration); break;
			case 161: TronEffect += rnz(duration); break;
			case 162: RedSpells += rnz(duration); break;
			case 163: TooHeavyEffect += rnz(duration); break;
			case 164: ElongationBug += rnz(duration); break;
			case 165: WrapoverEffect += rnz(duration); break;
			case 166: DestructionEffect += rnz(duration); break;
			case 167: MeleePrefixBug += rnz(duration); break;
			case 168: AutomoreBug += rnz(duration); break;
			case 169: UnfairAttackBug += rnz(duration); break;
			case 170: OrangeSpells += rnz(duration); break;
			case 171: VioletSpells += rnz(duration); break;
			case 172: LongingEffect += rnz(duration); break;
			case 173: CursedParts += rnz(duration); break;
			case 174: Quaversal += rnz(duration); break;
			case 175: AppearanceShuffling += rnz(duration); break;
			case 176: BrownSpells += rnz(duration); break;
			case 177: Choicelessness += rnz(duration); break;
			case 178: Goldspells += rnz(duration); break;
			case 179: Deprovement += rnz(duration); break;
			case 180: InitializationFail += rnz(duration); break;
			case 181: GushlushEffect += rnz(duration); break;
			case 182: SoiltypeEffect += rnz(duration); break;
			case 183: DangerousTerrains += rnz(duration); break;
			case 184: FalloutEffect += rnz(duration); break;
			case 185: MojibakeEffect += rnz(duration); break;
			case 186: GravationEffect += rnz(duration); break;
			case 187: UncalledEffect += rnz(duration); break;
			case 188: ExplodingDiceEffect += rnz(duration); break;
			case 189: PermacurseEffect += rnz(duration); break;
			case 190: ShroudedIdentity += rnz(duration); break;
			case 191: FeelerGauges += rnz(duration); break;
			case 192: LongScrewup += rnz(duration * 20); break;
			case 193: WingYellowChange += rnz(duration); break;
			case 194: LifeSavingBug += rnz(duration); break;
			case 195: CurseuseEffect += rnz(duration); break;
			case 196: CutNutritionEffect += rnz(duration); break;
			case 197: SkillLossEffect += rnz(duration); break;
			case 198: AutopilotEffect += rnz(duration); break;
			case 199: MysteriousForceActive += rnz(duration); break;
			case 200: MonsterGlyphChange += rnz(duration); break;
			case 201: ChangingDirectives += rnz(duration); break;
			case 202: ContainerKaboom += rnz(duration); break;
			case 203: StealDegrading += rnz(duration); break;
			case 204: LeftInventoryBug += rnz(duration); break;
			case 205: FluctuatingSpeed += rnz(duration); break;
			case 206: TarmuStrokingNora += rnz(duration); break;
			case 207: FailureEffects += rnz(duration); break;
			case 208: BrightCyanSpells += rnz(duration); break;
			case 209: FrequentationSpawns += rnz(duration); break;
			case 210: PetAIScrewed += rnz(duration); break;
			case 211: SatanEffect += rnz(duration); break;
			case 212: RememberanceEffect += rnz(duration); break;
			case 213: PokelieEffect += rnz(duration); break;
			case 214: AlwaysAutopickup += rnz(duration); break;
			case 215: DywypiProblem += rnz(duration); break;
			case 216: SilverSpells += rnz(duration); break;
			case 217: MetalSpells += rnz(duration); break;
			case 218: PlatinumSpells += rnz(duration); break;
			case 219: ManlerEffect += rnz(duration); break;
			case 220: DoorningEffect += rnz(duration); break;
			case 221: NownsibleEffect += rnz(duration); break;
			case 222: ElmStreetEffect += rnz(duration); break;
			case 223: MonnoiseEffect += rnz(duration); break;
			case 224: RangCallEffect += rnz(duration); break;
			case 225: RecurringSpellLoss += rnz(duration); break;
			case 226: AntitrainingEffect += rnz(duration); break;
			case 227: TechoutBug += rnz(duration); break;
			case 228: StatDecay += rnz(duration); break;
			case 229: Movemork += rnz(duration); break;
			case 230: BadPartBug += rnz(duration); break;
			case 231: CompletelyBadPartBug += rnz(duration); break;
			case 232: EvilVariantActive += rnz(duration); break;
			case 233: SanityTrebleEffect += rnz(duration); break;
			case 234: StatDecreaseBug += rnz(duration); break;
			case 235: SimeoutBug += rnz(duration); break;

		}

}

int
randomgoodskill()
{
	int result;

skillreroll:
	result = rnd(P_NUM_SKILLS);

	if (result >= P_SHII_CHO && result <= P_WEDI && rn2(10)) goto skillreroll;

	return result;
}

int
randomgoodcombatskill()
{
	int result;

combatreroll:
	result = P_BARE_HANDED_COMBAT + rn2(( P_RIDING - P_BARE_HANDED_COMBAT) + 1);
	if (result >= P_SHII_CHO && result <= P_WEDI && rn2(10)) goto combatreroll;

	return result;
}

#endif /* OVLB */

/*rnd.c*/
