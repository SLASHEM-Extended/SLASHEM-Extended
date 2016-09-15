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

int
rn3(x)		/* like rn2, but the result is more likely to be a lower number --Amy */
register int x;
{
	int y;
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

	switch (rnd(283)) {

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
		default:
			return AD_PHYS;

	}

}

int
randenchantment()
{

	switch (rnd(136)) {

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
			return STUNNED;
		case 112:
			return CONFUSION;
		case 113:
		case 114:
			switch (rnd(87)) {

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
			}
		case 115:
			return FEARED;
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
		default:
			return POISON_RES;

	}

}

int
randnastyenchantment()
{
	switch (rnd(87)) {

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
	}

}

#endif /* OVLB */

/*rnd.c*/
