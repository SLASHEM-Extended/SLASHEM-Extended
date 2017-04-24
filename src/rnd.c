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

	switch (rnd(302)) {

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
		default:
			return AD_PHYS;

	}

}

int
randenchantment()
{

	switch (rnd(156)) {

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
			switch (rnd(97)) {

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
			switch (rnd(88)) {
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
			}
		case 154:
			return STUNNED;
		case 155:
			return CONFUSION;
		case 156:
			return FEARED;

		default:
			return POISON_RES;

	}

}

int
randnastyenchantment()
{
	switch (rnd(97)) {

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
	}

}

int
randomdisableproperty()
{
	switch (rnd(60)) {

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
	else if (!rn2(2000)) return AUTOMATIC_SWITCHER;
	else switch (rnd(97)) {
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
	result = rnd_class(AMULET_OF_CHANGE,AMULET_OF_DEPRESSION);
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
	result = AMULET_OF_CHANGE + rn2((AMULET_OF_DEPRESSION + 1) - AMULET_OF_CHANGE);
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


#endif /* OVLB */

/*rnd.c*/
