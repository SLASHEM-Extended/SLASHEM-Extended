/*	SCCS Id: @(#)sp_lev.c	3.4	2001/09/06	*/
/*	Copyright (c) 1989 by Jean-Christophe Collet */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * This file contains the various functions that are related to the special
 * levels.
 * It contains also the special level loader.
 *
 */

#include "hack.h"
#include "dlb.h"
/* #define DEBUG */	/* uncomment to enable code debugging */

#ifdef DEBUG
# ifdef WIZARD
#define debugpline	if (wizard) pline
# else
#define debugpline	pline
# endif
#endif

#include "sp_lev.h"
#include "rect.h"

extern void mkmap(lev_init *);

STATIC_DCL void get_room_loc(schar *, schar *, struct mkroom *);
STATIC_DCL void get_free_room_loc(schar *, schar *, struct mkroom *);
STATIC_DCL void create_trap(trap *, struct mkroom *);
STATIC_DCL int noncoalignment(ALIGNTYP_P);
STATIC_DCL void create_monster(monster *, struct mkroom *);
STATIC_DCL void create_object(object *, struct mkroom *);
STATIC_DCL void create_engraving(engraving *,struct mkroom *);
STATIC_DCL void create_stairs(stair *, struct mkroom *);
STATIC_DCL void create_altar(altar *, struct mkroom *);
STATIC_DCL void create_gold(gold *, struct mkroom *);
STATIC_DCL void create_feature(int,int,struct mkroom *,int);
STATIC_DCL boolean search_door(struct mkroom *, xchar *, xchar *, XCHAR_P, int);
STATIC_DCL void fix_stair_rooms(void);
STATIC_DCL void create_corridor(corridor *);

STATIC_DCL boolean create_subroom(struct mkroom *, XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P);

#define LEFT	1
#define H_LEFT	2
#define CENTER	3
#define H_RIGHT	4
#define RIGHT	5
#define RANDOMX	-1

#define TOP	1
#define BOTTOM	5
#define RANDOMY	-1

#define sq(x) ((x)*(x))

#define XLIM	1
#define YLIM	1

#define Fread	(void)dlb_fread
#define Fgetc	(schar)dlb_fgetc
#define New(type)		(type *) alloc(sizeof(type))
#define NewTab(type, size)	(type **) alloc(sizeof(type *) * (unsigned)size)

static NEARDATA walk walklist[50];
extern int min_rx, max_rx, min_ry, max_ry; /* from mkmap.c */

static char Map[COLNO][ROWNO];
static char robjects[10], rloc_x[10][10], rloc_y[10][10], rmonst[10];
static aligntyp	ralign[3] = { AM_CHAOTIC, AM_NEUTRAL, AM_LAWFUL };
static NEARDATA xchar xstart, ystart;
static NEARDATA char xsize, ysize;
static lev_region rarea[10];

STATIC_DCL void set_wall_property(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,int);
STATIC_DCL int rnddoor(void);
STATIC_DCL int rndtrap(void);
STATIC_DCL boolean get_location(schar *,schar *,int);
STATIC_DCL void sp_lev_shuffle(char *,char *,int);
STATIC_DCL void light_region(region *);
STATIC_DCL void load_common_data(dlb *,int);
STATIC_DCL void load_one_monster(dlb *,monster *);
STATIC_DCL void load_one_object(dlb *,object *);
STATIC_DCL void load_one_engraving(dlb *,engraving *);
STATIC_DCL boolean load_rooms(dlb *);
STATIC_DCL void maze1xy(coord *,int);
STATIC_DCL boolean load_maze(dlb *);
STATIC_DCL void create_door(room_door *, struct mkroom *);
STATIC_DCL void free_rooms(room **, int);
STATIC_DCL void build_room(room *, room*);

char *lev_message = 0;
lev_region *lregions = 0;
int num_lregions = 0;
lev_init init_lev;

/* Make a random dungeon feature --Amy */
void
mkfeature(x,y)
register int x, y;
{

	int whatisit;
	aligntyp al;
	register int tryct = 0;
	register struct obj *otmp;

	if (occupied(x, y)) return;

	whatisit = rnd(330);

	switch (whatisit) {

	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	levl[x][y].typ = FOUNTAIN; level.flags.nfountains++; break;
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
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	levl[x][y].typ = CLOUD; break;

	case 31:
	case 32:
	case 33:
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
	case 44:
	case 45:
	case 46:
	case 47:
	case 48:
	case 49:
	case 50:
	levl[x][y].typ = ICE; break;

	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
	case 56:
	case 57:
	case 58:
	case 59:
	case 60:
	case 61:
	case 62:
	case 63:
	case 64:
	case 65:
	case 66:
	case 67:
	case 68:
	case 69:
	case 70:
	levl[x][y].typ = POOL; break;

	case 71:
	case 72:
	case 73:
	case 74:
	case 75:
	case 76:
	case 77:
	case 78:
	case 79:
	case 80:
	levl[x][y].typ = LAVAPOOL; break;

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
	levl[x][y].typ = IRONBARS; break;

	case 91:
	case 92:
	case 93:
	case 94:
	case 95:
	case 96:
	case 97:
	case 98:
	case 99:
	case 100:
	case 101:
	case 102:
	case 103:
	case 104:
	case 105:
	case 106:
	case 107:
	case 108:
	case 109:
	case 110:
	levl[x][y].typ = TREE; break;

	case 111:
	case 112:
	case 113:
	case 114:
	case 115:
	case 116:
	case 117:
	case 118:
	case 119:
	case 120:
	case 121:
	case 122:
	case 123:
	case 124:
	case 125:
	case 126:
	case 127:
	case 128:
	case 129:
	case 130:
	make_grave(x, y, (char *) 0);
	/* Possibly fill it with objects */
	if (!rn2(5)) (void) mkgold(0L, x, y);
	for (tryct = rn2(2 + rn2(4)); tryct; tryct--) {
		if (timebasedlowerchance()) {
		    otmp = mkobj(rn2(3) ? COIN_CLASS : RANDOM_CLASS, TRUE, FALSE);
		    if (!otmp) return;
		    curse(otmp);
		    otmp->ox = x;
		    otmp->oy = y;
		    add_to_buried(otmp);
		}
	}
	break;

	case 131:
	case 132:
	case 133:
	case 134:
	case 135:
	levl[x][y].typ = TOILET; break;

	case 136:
	case 137:
	case 138:
	case 139:
	case 140:
	levl[x][y].typ = SINK; level.flags.nsinks++; break;

	case 141:
	levl[x][y].typ = THRONE; break;

	case 142:
		levl[x][y].typ = ALTAR;
		al = rn2((int)A_LAWFUL+2) - 1;
		if (!rn2(10)) levl[x][y].altarmask = Align2amask( al );
		else levl[x][y].altarmask = Align2amask( A_NONE );

		if (!rn2(25)) {
			register struct obj *altarwater;
			altarwater = mksobj_at(POT_WATER, x, y, FALSE, FALSE, FALSE);
			if (altarwater) {

				if (Amask2align(levl[x][y].altarmask) == A_NONE && !rn2(5)) curse(altarwater);
				else bless(altarwater);
			}
		}

	 break;

	case 143:
	levl[x][y].typ = PENTAGRAM; break;

	case 144:
	levl[x][y].typ = WELL; break;

	case 145:
	levl[x][y].typ = POISONEDWELL; break;

	case 146:
	levl[x][y].typ = WAGON; break;

	case 147:
	levl[x][y].typ = BURNINGWAGON; break;

	case 148:
	case 149:
	levl[x][y].typ = WOODENTABLE; break;

	case 150:
	levl[x][y].typ = CARVEDBED; break;

	case 151:
	case 152:
	case 153:
	case 154:
	case 155:
	levl[x][y].typ = STRAWMATTRESS; break;

	case 156:
	case 157:
	case 158:
	case 159:
	case 160:
	case 161:
	case 162:
	case 163:
	case 164:
	case 165:
	levl[x][y].typ = GRAVEWALL; break;

	case 166:
	case 167:
	case 168:
	case 169:
	case 170:
	levl[x][y].typ = TUNNELWALL; break;

	case 171:
	case 172:
	case 173:
	case 174:
	case 175:
	levl[x][y].typ = FARMLAND; break;

	case 176:
	case 177:
	case 178:
	levl[x][y].typ = MOUNTAIN; break;

	case 179:
	levl[x][y].typ = WATERTUNNEL; break;

	case 180:
	case 181:
	case 182:
	case 183:
	case 184:
	case 185:
	case 186:
	case 187:
	case 188:
	case 189:
	levl[x][y].typ = CRYSTALWATER; break;

	case 190:
	case 191:
	case 192:
	case 193:
	levl[x][y].typ = MOORLAND; break;

	case 194:
	case 195:
	levl[x][y].typ = URINELAKE; break;

	case 196:
	case 197:
	case 198:
	levl[x][y].typ = SHIFTINGSAND; break;

	case 199:
	case 200:
	case 201:
	levl[x][y].typ = STYXRIVER; break;

	case 202:
	case 203:
	case 204:
	case 205:
	case 206:
	case 207:
	case 208:
	case 209:
	case 210:
	case 211:
	case 212:
	case 213:
	case 214:
	case 215:
	case 216:
	levl[x][y].typ = SNOW; break;

	case 217:
	case 218:
	case 219:
	case 220:
	case 221:
	case 222:
	case 223:
	case 224:
	case 225:
	case 226:
	levl[x][y].typ = ASH; break;

	case 227:
	case 228:
	case 229:
	case 230:
	case 231:
	case 232:
	case 233:
	case 234:
	case 235:
	case 236:
	case 237:
	case 238:
	case 239:
	case 240:
	case 241:
	case 242:
	case 243:
	case 244:
	case 245:
	case 246:
	levl[x][y].typ = SAND; break;

	case 247:
	case 248:
	case 249:
	case 250:
	case 251:
	levl[x][y].typ = PAVEDFLOOR; break;

	case 252:
	case 253:
	case 254:
	case 255:
	levl[x][y].typ = HIGHWAY; break;

	case 256:
	case 257:
	case 258:
	case 259:
	case 260:
	case 261:
	case 262:
	case 263:
	case 264:
	case 265:
	case 266:
	case 267:
	case 268:
	case 269:
	case 270:
	case 271:
	case 272:
	case 273:
	case 274:
	case 275:
	case 276:
	case 277:
	case 278:
	case 279:
	case 280:
	levl[x][y].typ = GRASSLAND; break;

	case 281:
	case 282:
	case 283:
	case 284:
	case 285:
	levl[x][y].typ = NETHERMIST; break;

	case 286:
	case 287:
	case 288:
	levl[x][y].typ = STALACTITE; break;

	case 289:
	case 290:
	case 291:
	case 292:
	case 293:
	case 294:
	case 295:
	case 296:
	levl[x][y].typ = CRYPTFLOOR; break;

	case 297:
	case 298:
	case 299:
	levl[x][y].typ = BUBBLES; break;

	case 300:
	case 301:
	case 302:
	case 303:
	case 304:
	case 305:
	case 306:
	levl[x][y].typ = RAINCLOUD; break;

	case 307:
	levl[x][y].typ = PENTAGRAM; break;

	default:
	levl[x][y].typ = STONE; break;
	}

}

/*
 * Make walls of the area (x1, y1, x2, y2) non diggable/non passwall-able
 */

STATIC_OVL void
set_wall_property(x1,y1,x2,y2, prop)
xchar x1, y1, x2, y2;
int prop;
{
	register xchar x, y;

	for(y = y1; y <= y2; y++)
	    for(x = x1; x <= x2; x++)
		if(IS_STWALL(levl[x][y].typ) && !(IS_DIGGABLEWALL(levl[x][y].typ)) )
		    levl[x][y].wall_info |= prop;
}

/*
 * Choose randomly the state (nodoor, open, closed or locked) for a door
 */
STATIC_OVL int
rnddoor()
{
	int i = 1 << rn2(5);
	i >>= 1;
	return i;
}

/*
 * Select a random trap
 */
STATIC_OVL int
rndtrap()
{
	int rtrap;

	do {
selecttrap:
	    rtrap = rnd(TRAPNUM-1);
	    if (!Race_if(PM_DEVELOPER) && !rn2(issoviet ? 2 : 3)) rtrap = rnd(rn2(3) ? ANTI_MAGIC : POLY_TRAP);
	    switch (rtrap) {
	     case HOLE:		/* no random holes on special levels */
	     case S_PRESSING_TRAP:
	     case MAGIC_PORTAL:	goto selecttrap;
			break;
	     case NUPESELL_TRAP:
			if (In_sokoban(&u.uz) && rn2(100)) goto selecttrap;
			break;
	     case SHAFT_TRAP:
	     case TRAPDOOR:
			if (In_sokoban(&u.uz) && rn2(10)) goto selecttrap;
			if (!Can_dig_down(&u.uz) && !Is_stronghold(&u.uz) ) goto selecttrap;
			break;

	     case CURRENT_SHAFT:
			if (In_sokoban(&u.uz) && rn2(10)) goto selecttrap;
			if (!Can_dig_down(&u.uz) && !Is_stronghold(&u.uz) ) goto selecttrap;
			if (rn2(3)) goto selecttrap;
			break;
	     case PIT:
	     case SPIKED_PIT:
	     case GIANT_CHASM:
	     case SHIT_PIT:
	     case MANA_PIT:
	     case ACID_PIT:
			if (In_sokoban(&u.uz) && rn2(10)) goto selecttrap;
			break;

	     case LEVEL_TELEP:	if (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) )  goto selecttrap;
			break;
	     case LEVEL_BEAMER:	if (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) )  goto selecttrap;
			break;
	     case NEXUS_TRAP:	if (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) )  goto selecttrap;
			break;
	     case TELEP_TRAP:	if (level.flags.noteleport || Race_if(PM_STABILISATOR)) goto selecttrap;
	     case BEAMER_TRAP:	if (level.flags.noteleport || Race_if(PM_STABILISATOR)) goto selecttrap;
			break;
	     case ROLLING_BOULDER_TRAP:
	     case ROCKTRAP:	if (In_endgame(&u.uz)) goto selecttrap;
			break;

	     case MENU_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8)) goto selecttrap;
			break;
	     case SPEED_TRAP: 
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40)) goto selecttrap;
			break;
	     case AUTOMATIC_SWITCHER:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(ishaxor ? (Role_if(PM_GRADUATE) ? 125 : Role_if(PM_SPACEWARS_FIGHTER) ? 250 : Role_if(PM_GEEK) ? 250 : 500) : (Role_if(PM_GRADUATE) ? 250 : Role_if(PM_SPACEWARS_FIGHTER) ? 500 : Role_if(PM_GEEK) ? 500 : 1000) )) goto selecttrap;
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
	     case ITEM_TELEP_TRAP:
			if (rn2(evilfriday ? 3 : 15) && !NastyTrapNation) goto selecttrap;
			break;
	     case LAVA_TRAP:
			if (rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap;
			break;
	     case FLOOD_TRAP:
			if (rn2(evilfriday ? 2 : 20) && !NastyTrapNation) goto selecttrap;
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
		    case FUCKFUCKFUCK_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;
		    case OPTION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap;
			break;
		    case MISCOLOR_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap;
			break;
		    case ONE_RAINBOW_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 9 )) goto selecttrap;
			break;
		    case COLORSHIFT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 9 )) goto selecttrap;
			break;
		    case TOP_LINE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) goto selecttrap;
			break;
		    case CAPS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap;
			break;
		    case UN_KNOWLEDGE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 16 )) goto selecttrap;
			break;
		    case DARKHANCE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap;
			break;
		    case DSCHUEUEUET_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap;
			break;
		    case NOPESKILL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 11 : 22 )) goto selecttrap;
			break;
		    case REAL_LIE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 12 : 25 )) goto selecttrap;
			break;
		    case ESCAPE_PAST_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap;
			break;
		    case PETHATE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap;
			break;
		    case PET_LASHOUT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap;
			break;
		    case PETSTARVE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap;
			break;
		    case PETSCREW_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap;
			break;
		    case TECH_LOSS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap;
			break;
		    case UN_INVIS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap;
			break;
		    case PROOFLOSS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap;
			break;
		    case DETECTATION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 18 : 35 )) goto selecttrap;
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
			break;
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
			if (!Role_if(PM_CAMPERSTRIKER)  && !NastyTrapNation&& rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap;
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

	    case SPACEWARS_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && !Role_if(PM_SPACEWARS_FIGHTER) && rn2(15) ) goto selecttrap;
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
		if (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) )  goto selecttrap;
		if (rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap;
		break;
	    case BRANCH_BEAMER:
		if (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) )  goto selecttrap;
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
		    case TANJA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case SONJA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case RHEA_TRAP:
			if (!Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap;
			break;
		    case LARA_TRAP:
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
	} while (rtrap == NO_TRAP);
	return rtrap;
}

/*
 * Select a random trap (extended) --Amy
 */
int
randomtrap()
{
	int rtrap;

	do {
selecttrap2:
	    rtrap = rnd(TRAPNUM-1);
	    if (!Race_if(PM_DEVELOPER) && !rn2(issoviet ? 2 : 3)) rtrap = rnd(rn2(3) ? ANTI_MAGIC : POLY_TRAP);
		if (rtrap == HOLE && !Is_stronghold(&u.uz) ) goto selecttrap2;
		if (In_sokoban(&u.uz) && rn2(10) && (rtrap == HOLE || rtrap == TRAPDOOR || rtrap == SHAFT_TRAP || rtrap == CURRENT_SHAFT || rtrap == PIT || rtrap == SPIKED_PIT || rtrap == GIANT_CHASM || rtrap == SHIT_PIT || rtrap == MANA_PIT || rtrap == ANOXIC_PIT || rtrap == ACID_PIT)) goto selecttrap2;
		if (In_sokoban(&u.uz) && rn2(100) && rtrap == NUPESELL_TRAP) goto selecttrap2;
		if (rtrap == MAGIC_PORTAL) goto selecttrap2;
		if (rtrap == S_PRESSING_TRAP) goto selecttrap2;
		if (rtrap == TRAPDOOR && !Can_dig_down(&u.uz) && !Is_stronghold(&u.uz) ) goto selecttrap2;
		if (rtrap == SHAFT_TRAP && !Can_dig_down(&u.uz) && !Is_stronghold(&u.uz) ) goto selecttrap2;
		if (rtrap == CURRENT_SHAFT && !Can_dig_down(&u.uz) && !Is_stronghold(&u.uz) ) goto selecttrap2;
		if (rtrap == LEVEL_TELEP && (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) ) ) goto selecttrap2;
		if (rtrap == LEVEL_BEAMER && (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) ) ) goto selecttrap2;
		if (rtrap == BRANCH_TELEPORTER && (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) ) ) goto selecttrap2;
		if (rtrap == BRANCH_BEAMER && (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) ) ) goto selecttrap2;
		if (rtrap == NEXUS_TRAP && (level.flags.noteleport || Race_if(PM_STABILISATOR) || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) ) ) goto selecttrap2;
		if (rtrap == TELEP_TRAP && (level.flags.noteleport || Race_if(PM_STABILISATOR) )) goto selecttrap2;
		if (rtrap == BEAMER_TRAP && (level.flags.noteleport || Race_if(PM_STABILISATOR) )) goto selecttrap2;
		if (rtrap == ROLLING_BOULDER_TRAP) goto selecttrap2;
		if (rtrap == NO_TRAP) goto selecttrap2;
		if (rtrap == RMB_LOSS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && !Role_if(PM_SPACEWARS_FIGHTER) && rn2(2)) goto selecttrap2;
		if (rtrap == DISPLAY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3)) goto selecttrap2;
		if (rtrap == SPELL_LOSS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4)) goto selecttrap2;
		if (rtrap == YELLOW_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 5)) goto selecttrap2;

		if (rtrap == MENU_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8)) goto selecttrap2;
		if (rtrap == SPEED_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40)) goto selecttrap2;
		if (rtrap == AUTOMATIC_SWITCHER && !Role_if(PM_CAMPERSTRIKER) && rn2(ishaxor ? (Role_if(PM_GRADUATE) ? 125 : Role_if(PM_SPACEWARS_FIGHTER) ? 250 : Role_if(PM_GEEK) ? 250 : 500) : (Role_if(PM_GRADUATE) ? 250 : Role_if(PM_SPACEWARS_FIGHTER) ? 500 : Role_if(PM_GEEK) ? 500 : 1000) )) goto selecttrap2;

		if (rtrap == AUTO_DESTRUCT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10)) goto selecttrap2;
		if (rtrap == MEMORY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20)) goto selecttrap2;
		if (rtrap == INVENTORY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50)) goto selecttrap2;
		if (rtrap == BLACK_NG_WALL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100)) goto selecttrap2;
		if (rtrap == SUPERSCROLLER_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 100 : 200)) goto selecttrap2;
		if (rtrap == ACTIVE_SUPERSCROLLER_TRAP) goto selecttrap2;

	      if (rtrap == ITEM_TELEP_TRAP && rn2(evilfriday ? 3 : 15) && !NastyTrapNation) goto selecttrap2;

	      if (rtrap == PERSISTENT_FART_TRAP && rn2(evilfriday ? 5 : 16) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ATTACKING_HEEL_TRAP && rn2(evilfriday ? 5 : 16) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ALIGNMENT_TRASH_TRAP && rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == DOGSIDE_TRAP && rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == BANKRUPT_TRAP && rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == DYNAMITE_TRAP && rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == MALEVOLENCE_TRAP && rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == LEAFLET_TRAP && rn2(evilfriday ? 2 : 20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == TENTADEEP_TRAP && rn2(evilfriday ? 3 : 15) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == STATHALF_TRAP && rn2(evilfriday ? 8 : 50) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == RARE_SPAWN_TRAP && rn2(evilfriday ? 3 : 15) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == YOU_ARE_AN_IDIOT_TRAP && rn2(evilfriday ? 50 : 200) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == NASTYCURSE_TRAP && rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap2;

	      if (rtrap == LAVA_TRAP && rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == FLOOD_TRAP && rn2(evilfriday ? 2 : 20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == DRAIN_TRAP && (rn2(3) && !evilfriday) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == TIME_TRAP && rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == FREE_HAND_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap2;
	      if (rtrap == DISINTEGRATION_TRAP && rn2(evilfriday ? 5 : 20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == UNIDENTIFY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == THIRST_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap2;
	      if (rtrap == SHADES_OF_GREY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
	      if (rtrap == LUCK_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) goto selecttrap2;
	      if (rtrap == DEATH_TRAP && rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == STONE_TO_FLESH_TRAP && rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == FAINT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) goto selecttrap2;
	      if (rtrap == CURSE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 32 : 64 )) goto selecttrap2;
	      if (rtrap == DIFFICULTY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 12 : 24 )) goto selecttrap2;
	      if (rtrap == SOUND_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 14 )) goto selecttrap2;
	      if (rtrap == CASTER_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap2;
	      if (rtrap == WEAKNESS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) goto selecttrap2;
	      if (rtrap == ROT_THIRTEEN_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap2;
	      if (rtrap == BISHOP_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 28 : 56 )) goto selecttrap2;
	      if (rtrap == CONFUSION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 40 : 80 )) goto selecttrap2;
	      if (rtrap == DROP_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 35 : 80 )) goto selecttrap2;
	      if (rtrap == DSTW_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap2;
	      if (rtrap == STATUS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 9 )) goto selecttrap2;
	      if (rtrap == ALIGNMENT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 7 )) goto selecttrap2;
	      if (rtrap == STAIRS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 11 )) goto selecttrap2;
	      if (rtrap == UNINFORMATION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3 )) goto selecttrap2;
	      if (rtrap == REPEATING_NASTYCURSE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100 )) goto selecttrap2;
	      if (rtrap == TIMERUN_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap2;
	      if (rtrap == FUCKFUCKFUCK_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == OPTION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
	      if (rtrap == MISCOLOR_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap2;
	      if (rtrap == ONE_RAINBOW_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 9 )) goto selecttrap2;
	      if (rtrap == COLORSHIFT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 9 )) goto selecttrap2;
	      if (rtrap == TOP_LINE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) goto selecttrap2;
	      if (rtrap == CAPS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap2;
	      if (rtrap == UN_KNOWLEDGE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 16 )) goto selecttrap2;
	      if (rtrap == DARKHANCE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap2;
	      if (rtrap == DSCHUEUEUET_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap2;
	      if (rtrap == NOPESKILL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 11 : 22 )) goto selecttrap2;
	      if (rtrap == REAL_LIE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 12 : 25 )) goto selecttrap2;
	      if (rtrap == ESCAPE_PAST_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == PETHATE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap2;
	      if (rtrap == PET_LASHOUT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap2;
	      if (rtrap == PETSTARVE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap2;
	      if (rtrap == PETSCREW_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap2;
	      if (rtrap == TECH_LOSS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap2;
	      if (rtrap == UN_INVIS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap2;
	      if (rtrap == PROOFLOSS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap2;
	      if (rtrap == DETECTATION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 18 : 35 )) goto selecttrap2;
	      if (rtrap == REALLY_BAD_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
	      if (rtrap == COVID_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap2;
	      if (rtrap == ARTIBLAST_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 9 )) goto selecttrap2;
	      if (rtrap == GIANT_EXPLORER_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 40 : 80 )) goto selecttrap2;
	      if (rtrap == TRAPWARP_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == YAWM_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 15 )) goto selecttrap2;
	      if (rtrap == CRADLE_OF_CHAOS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == TEZCATLIPOCA_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap2;
	      if (rtrap == ENTHUMESIS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100 )) goto selecttrap2;
	      if (rtrap == MIKRAANESIS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 100 : 200 )) goto selecttrap2;
	      if (rtrap == GOTS_TOO_GOOD_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap2;
	      if (rtrap == KILLER_ROOM_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
	      if (rtrap == NO_FUN_WALLS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 17 : 35 )) goto selecttrap2;
	      if (rtrap == SANITY_TREBLE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 51 )) goto selecttrap2;
	      if (rtrap == STAT_DECREASE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == SIMEOUT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100 )) goto selecttrap2;

		if (rtrap == BAD_PART_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
		if (rtrap == COMPLETELY_BAD_PART_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 26 : 52 )) goto selecttrap2;
		if (rtrap == EVIL_VARIANT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 500 : 1000 )) goto selecttrap2;

	      if (rtrap == INTRINSIC_LOSS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap2;
	      if (rtrap == BLOOD_LOSS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 18 )) goto selecttrap2;
	      if (rtrap == BAD_EFFECT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3 )) goto selecttrap2;
	      if (rtrap == MULTIPLY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 17 )) goto selecttrap2;
	      if (rtrap == AUTO_VULN_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap2;
	      if (rtrap == TELE_ITEMS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
	      if (rtrap == NASTINESS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 250 : 500 )) goto selecttrap2;

	      if (rtrap == FARLOOK_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 17 : 34 )) goto selecttrap2;
	      if (rtrap == RESPAWN_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == CAPTCHA_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 5 )) goto selecttrap2;

	      if (rtrap == RECURRING_AMNESIA_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 35 : 70 )) goto selecttrap2;
	      if (rtrap == BIGSCRIPT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3 )) goto selecttrap2;
	      if (rtrap == BANK_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 13 : 26 )) goto selecttrap2;
	      if (rtrap == ONLY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) goto selecttrap2;
	      if (rtrap == MAP_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == TECH_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
	      if (rtrap == DISENCHANT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) goto selecttrap2;
	      if (rtrap == VERISIERT && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap2;
	      if (rtrap == CHAOS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 17 : 33 )) goto selecttrap2;
	      if (rtrap == MUTENESS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) goto selecttrap2;
	      if (rtrap == NTLL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap2;
	      if (rtrap == ENGRAVING_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 17 )) goto selecttrap2;
	      if (rtrap == MAGIC_DEVICE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) goto selecttrap2;
	      if (rtrap == BOOK_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 19 )) goto selecttrap2;
	      if (rtrap == LEVEL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 15 )) goto selecttrap2;
	      if (rtrap == QUIZ_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 11 )) goto selecttrap2;

		if (rtrap == METABOLIC_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
		if (rtrap == TRAP_OF_NO_RETURN && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap2;
		if (rtrap == EGOTRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 15 )) goto selecttrap2;
		if (rtrap == FAST_FORWARD_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
		if (rtrap == TRAP_OF_ROTTENNESS && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 17 )) goto selecttrap2;
		if (rtrap == UNSKILLED_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap2;
		if (rtrap == LOW_STATS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 13 )) goto selecttrap2;
		if (rtrap == TRAINING_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) goto selecttrap2;
		if (rtrap == EXERCISE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4 )) goto selecttrap2;

		if (rtrap == LIMITATION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
		if (rtrap == WEAK_SIGHT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 16 )) goto selecttrap2;
		if (rtrap == RANDOM_MESSAGE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 16 )) goto selecttrap2;

		if (rtrap == DESECRATION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
		if (rtrap == STARVATION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap2;
		if (rtrap == DROPLESS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap2;
		if (rtrap == LOW_EFFECT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 14 )) goto selecttrap2;
		if (rtrap == INVISIBLE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 45 : 90 )) goto selecttrap2;
		if (rtrap == GHOST_WORLD_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap2;
		if (rtrap == DEHYDRATION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 15 )) goto selecttrap2;
		if (rtrap == HATE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap2;
		if (rtrap == TOTTER_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 150 : 300 )) goto selecttrap2;
		if (rtrap == NONINTRINSICAL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap2;
		if (rtrap == DROPCURSE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) goto selecttrap2;
		if (rtrap == NAKEDNESS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap2;
		if (rtrap == ANTILEVEL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 75 : 150 )) goto selecttrap2;
		if (rtrap == STEALER_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4 )) goto selecttrap2;
		if (rtrap == REBELLION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 5 )) goto selecttrap2;
		if (rtrap == CRAP_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 13 )) goto selecttrap2;
		if (rtrap == MISFIRE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 13 )) goto selecttrap2;
		if (rtrap == TRAP_OF_WALLS && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) goto selecttrap2;
		if (rtrap == DISCONNECT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
		if (rtrap == INTERFACE_SCREW_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 80 : 160 )) goto selecttrap2;
		if (rtrap == BOSSFIGHT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap2;
		if (rtrap == ENTIRE_LEVEL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 17 )) goto selecttrap2;
		if (rtrap == BONES_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4 )) goto selecttrap2;
		if (rtrap == AUTOCURSE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 49 )) goto selecttrap2;
		if (rtrap == HIGHLEVEL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 13 )) goto selecttrap2;
		if (rtrap == SPELL_FORGETTING_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 21 : 42 )) goto selecttrap2;
		if (rtrap == SOUND_EFFECT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 7 )) goto selecttrap2;
	      if (rtrap == LOOTCUT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 18 )) goto selecttrap2;
	      if (rtrap == MONSTER_SPEED_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
	      if (rtrap == SCALING_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap2;
	      if (rtrap == ENMITY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 15 )) goto selecttrap2;
	      if (rtrap == WHITE_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap2;
	      if (rtrap == COMPLETE_GRAY_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap2;
	      if (rtrap == QUASAR_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 100 : 200 )) goto selecttrap2;
	      if (rtrap == MOMMA_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap2;
	      if (rtrap == HORROR_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 14 )) goto selecttrap2;
	      if (rtrap == ARTIFICER_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 40 : 80 )) goto selecttrap2;
	      if (rtrap == WEREFORM_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap2;
	      if (rtrap == NON_PRAYER_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == EVIL_PATCH_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 500 : 1000 )) goto selecttrap2;
	      if (rtrap == HARD_MODE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 45 : 90 )) goto selecttrap2;
	      if (rtrap == SECRET_ATTACK_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap2;
	      if (rtrap == EATER_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == COVETOUSNESS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 7 )) goto selecttrap2;
	      if (rtrap == NOT_SEEN_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 7 )) goto selecttrap2;
	      if (rtrap == DARK_MODE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 9 )) goto selecttrap2;
	      if (rtrap == ANTISEARCH_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap2;
	      if (rtrap == HOMICIDE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) goto selecttrap2;
	      if (rtrap == NASTY_NATION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 70 : 140 )) goto selecttrap2;
	      if (rtrap == WAKEUP_CALL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
	      if (rtrap == GRAYOUT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 17 : 34 )) goto selecttrap2;
	      if (rtrap == GRAY_CENTER_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 18 : 37 )) goto selecttrap2;
	      if (rtrap == CHECKERBOARD_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
	      if (rtrap == CLOCKWISE_SPIN_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 350 : 700 )) goto selecttrap2;
	      if (rtrap == COUNTERCLOCKWISE_SPIN_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 250 : 500 )) goto selecttrap2;
	      if (rtrap == LAG_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 90 : 180 )) goto selecttrap2;
	      if (rtrap == BLESSCURSE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 11 : 22 )) goto selecttrap2;
	      if (rtrap == DE_LIGHT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) goto selecttrap2;
	      if (rtrap == DISCHARGE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 5 )) goto selecttrap2;
	      if (rtrap == TRASHING_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) goto selecttrap2;
	      if (rtrap == FILTERING_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap2;
	      if (rtrap == DEFORMATTING_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) goto selecttrap2;
	      if (rtrap == FLICKER_STRIP_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4 )) goto selecttrap2;
	      if (rtrap == UNDRESSING_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 17 : 34 )) goto selecttrap2;
	      if (rtrap == HYPERBLUEWALL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 18 : 36 )) goto selecttrap2;
	      if (rtrap == NOLITE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap2;
	      if (rtrap == PARANOIA_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap2;
	      if (rtrap == FLEECESCRIPT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 7 )) goto selecttrap2;
	      if (rtrap == INTERRUPT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 12 : 25 )) goto selecttrap2;
	      if (rtrap == DUSTBIN_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 13 : 26 )) goto selecttrap2;
	      if (rtrap == MANA_BATTERY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 19 )) goto selecttrap2;
	      if (rtrap == MONSTERFINGERS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 19 )) goto selecttrap2;
	      if (rtrap == MISCAST_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 12 : 23 )) goto selecttrap2;
	      if (rtrap == MESSAGE_SUPPRESSION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 12 : 25 )) goto selecttrap2;
	      if (rtrap == STUCK_ANNOUNCEMENT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 15 )) goto selecttrap2;
	      if (rtrap == BLOODTHIRSTY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 18 : 35 )) goto selecttrap2;
	      if (rtrap == MAXIMUM_DAMAGE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 16 : 32 )) goto selecttrap2;
	      if (rtrap == LATENCY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) goto selecttrap2;
	      if (rtrap == STARLIT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 35 : 70 )) goto selecttrap2;
	      if (rtrap == KNOWLEDGE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap2;
	      if (rtrap == HIGHSCORE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 40 : 80 )) goto selecttrap2;
	      if (rtrap == PINK_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) goto selecttrap2;
	      if (rtrap == GREEN_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 13 : 26 )) goto selecttrap2;
	      if (rtrap == EVC_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap2;
	      if (rtrap == UNDERLAYER_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 39 )) goto selecttrap2;
	      if (rtrap == DAMAGE_METER_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 9 )) goto selecttrap2;
	      if (rtrap == ARBITRARY_WEIGHT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap2;
	      if (rtrap == FUCKED_INFO_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap2;
	      if (rtrap == BLACK_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 16 : 32 )) goto selecttrap2;
	      if (rtrap == CYAN_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
	      if (rtrap == HEAP_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 11 : 22 )) goto selecttrap2;
	      if (rtrap == BLUE_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 17 )) goto selecttrap2;
	      if (rtrap == TRON_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 26 : 53 )) goto selecttrap2;
	      if (rtrap == RED_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap2;
	      if (rtrap == TOO_HEAVY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) goto selecttrap2;
	      if (rtrap == ELONGATION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 51 )) goto selecttrap2;
	      if (rtrap == WRAPOVER_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 36 : 72 )) goto selecttrap2;
	      if (rtrap == DESTRUCTION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 16 : 32 )) goto selecttrap2;
	      if (rtrap == MELEE_PREFIX_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 40 : 80 )) goto selecttrap2;
	      if (rtrap == AUTOMORE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == UNFAIR_ATTACK_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 35 : 70 )) goto selecttrap2;
	      if (rtrap == ORANGE_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 16 : 31 )) goto selecttrap2;
	      if (rtrap == VIOLET_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap2;
	      if (rtrap == TRAP_OF_LONGING && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == CURSED_PART_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 11 )) goto selecttrap2;
	      if (rtrap == QUAVERSAL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 100 : 200 )) goto selecttrap2;
	      if (rtrap == APPEARANCE_SHUFFLING_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap2;
	      if (rtrap == BROWN_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 11 )) goto selecttrap2;
	      if (rtrap == CHOICELESS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 16 : 32 )) goto selecttrap2;
	      if (rtrap == GOLDSPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
	      if (rtrap == DEPROVEMENT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 45 : 90 )) goto selecttrap2;
	      if (rtrap == INITIALIZATION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) goto selecttrap2;
	      if (rtrap == GUSHLUSH_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 12 : 24 )) goto selecttrap2;
	      if (rtrap == SOILTYPE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 7 )) goto selecttrap2;
	      if (rtrap == DANGEROUS_TERRAIN_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 13 : 26 )) goto selecttrap2;
	      if (rtrap == FALLOUT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) goto selecttrap2;
	      if (rtrap == MOJIBAKE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) goto selecttrap2;
	      if (rtrap == GRAVATION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 16 : 32 )) goto selecttrap2;
	      if (rtrap == UNCALLED_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 17 )) goto selecttrap2;
	      if (rtrap == EXPLODING_DICE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap2;
	      if (rtrap == PERMACURSE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 26 : 52 )) goto selecttrap2;
	      if (rtrap == SHROUDED_IDENTITY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) goto selecttrap2;
	      if (rtrap == FEELER_GAUGES_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) goto selecttrap2;
	      if (rtrap == LONG_SCREWUP_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) goto selecttrap2;
	      if (rtrap == WING_YELLOW_CHANGER && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 33 : 66 )) goto selecttrap2;
	      if (rtrap == LIFE_SAVING_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 27 : 54 )) goto selecttrap2;
	      if (rtrap == CURSEUSE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 27 )) goto selecttrap2;
	      if (rtrap == CUT_NUTRITION_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == SKILL_LOSS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 32 : 64 )) goto selecttrap2;
	      if (rtrap == AUTOPILOT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 100 : 200 )) goto selecttrap2;
	      if (rtrap == FORCE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) goto selecttrap2;
	      if (rtrap == MONSTER_GLYPH_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 13 : 26 )) goto selecttrap2;
	      if (rtrap == CHANGING_DIRECTIVE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
	      if (rtrap == CONTAINER_KABOOM_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 16 : 32 )) goto selecttrap2;
	      if (rtrap == STEAL_DEGRADE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 19 : 38 )) goto selecttrap2;
	      if (rtrap == LEFT_INVENTORY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
	      if (rtrap == FLUCTUATING_SPEED_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100 )) goto selecttrap2;
	      if (rtrap == TARMUSTROKINGNORA_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) goto selecttrap2;
	      if (rtrap == FAILURE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == BRIGHT_CYAN_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap2;
	      if (rtrap == FREQUENTATION_SPAWN_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 14 )) goto selecttrap2;
	      if (rtrap == PET_AI_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) goto selecttrap2;
	      if (rtrap == SATAN_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) goto selecttrap2;
	      if (rtrap == REMEMBERANCE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 19 )) goto selecttrap2;
	      if (rtrap == POKELIE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4 )) goto selecttrap2;
	      if (rtrap == AUTOPICKUP_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 9 )) goto selecttrap2;
	      if (rtrap == DYWYPI_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 11 : 22 )) goto selecttrap2;
	      if (rtrap == SILVER_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) goto selecttrap2;
	      if (rtrap == METAL_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 27 )) goto selecttrap2;
	      if (rtrap == PLATINUM_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 29 )) goto selecttrap2;
	      if (rtrap == MANLER_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 36 : 72 )) goto selecttrap2;
	      if (rtrap == DOORNING_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) goto selecttrap2;
	      if (rtrap == NOWNSIBLE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 45 : 90 )) goto selecttrap2;
	      if (rtrap == ELM_STREET_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 13 : 26 )) goto selecttrap2;
	      if (rtrap == MONNOISE_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) goto selecttrap2;
	      if (rtrap == RANG_CALL_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 18 )) goto selecttrap2;
	      if (rtrap == RECURRING_SPELL_LOSS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == ANTITRAINING_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == TECHOUT_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) goto selecttrap2;
	      if (rtrap == STAT_DECAY_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100 )) goto selecttrap2;
	      if (rtrap == MOVEMORK_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) goto selecttrap2;

		if (rtrap == SPACEWARS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !u.arabellahack && !NastyTrapNation && !Role_if(PM_SPACEWARS_FIGHTER) && rn2(15) ) goto selecttrap2;

	      if (rtrap == FALLING_LOADSTONE_TRAP && (rn2(3) && !evilfriday) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == FALLING_NASTYSTONE_TRAP && rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == EVIL_ARTIFACT_TRAP && rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == RODNEY_TRAP && rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap2;

		if (rtrap == HYBRID_TRAP && rn2(500) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == SHAPECHANGE_TRAP && rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == CATACLYSM_TRAP && rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == KILL_SYMBIOTE_TRAP && rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == FEMINISM_STONE_TRAP && rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == BRANCH_TELEPORTER && rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == BRANCH_BEAMER && rn2(evilfriday ? 2 : 12) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == TV_TROPES_TRAP && rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == SHUEFT_TRAP && rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == CONDUCT_TRAP && rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == SKILL_POINT_LOSS_TRAP && rn2(evilfriday ? 20 : 200) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == DUMBIE_LIGHTSABER_TRAP && rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == WRONG_STAIRS && rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == AMNESIA_SWITCH_TRAP && rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == DATA_DELETE_TRAP && rn2(evilfriday ? 1000 : 10000)) goto selecttrap2;
		if (rtrap == ELDER_TENTACLING_TRAP && rn2(evilfriday ? 100 : 1000)) goto selecttrap2;
		if (rtrap == FOOTERER_TRAP && rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap2;

	      if (rtrap == RECURSION_TRAP && rn2(500) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == TEMPORARY_RECURSION_TRAP && rn2(50) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == WARP_ZONE && rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == KOP_CUBE && rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == BOSS_SPAWNER && rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == MIND_WIPE_TRAP && rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == GATEWAY_FROM_HELL && rn2(evilfriday ? 2 : 20) && !NastyTrapNation) goto selecttrap2;

	      if (rtrap == PESTILENCE_TRAP && rn2(evilfriday ? 5 : 20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == FAMINE_TRAP && (rn2(2) && !evilfriday) && !NastyTrapNation) goto selecttrap2;

	      if (rtrap == ARTIFACT_JACKPOT_TRAP && rn2(200)) goto selecttrap2;
	      if (rtrap == SPREADING_TRAP && rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == SUPERTHING_TRAP && rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == CURRENT_SHAFT && rn2(3) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == LEVITATION_TRAP && rn2(evilfriday ? 2 : 3) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == GOOD_ARTIFACT_TRAP && rn2(200)) goto selecttrap2;
	      if (rtrap == GENDER_TRAP && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == TRAP_OF_OPPOSITE_ALIGNMENT && rn2(20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == PET_TRAP && rn2(evilfriday ? 5 : 25) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == PIERCING_BEAM_TRAP && rn2(3) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == BACK_TO_START_TRAP && rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == NEMESIS_TRAP && rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == STREW_TRAP && rn2(evilfriday ? 5 : 30) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == OUTTA_DEPTH_TRAP && rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == GAY_TRAP && rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ITEM_NASTIFICATION_TRAP && rn2(evilfriday ? 20 : 200) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == BOON_TRAP && rn2(200)) goto selecttrap2;
	      if (rtrap == ANOXIC_PIT && (rn2(3) && !evilfriday) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ARABELLA_SPEAKER && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == FEMMY_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == MADELEINE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == MARLENA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == TANJA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == SONJA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == RHEA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == LARA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == NADINE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == LUISA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == IRINA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == LISELOTTE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == GRETA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == JANE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == SUE_LYN_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == CHARLOTTE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == HANNAH_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == LITTLE_MARIE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(50) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == KSENIA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == LYDIA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == CONNY_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == KATIA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == MARIYA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(15) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ELISE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == RONJA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ARIANE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == JOHANNA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == INGE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ROSA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == KRISTIN_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ANNA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == RUEA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == DORA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == MARIKE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(25) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == JETTE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == INA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == SING_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == VICTORIA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == MELISSA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ANITA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == HENRIETTA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == VERENA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ARABELLA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(25) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == SARAH_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == CLAUDIA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == LUDGERA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == KATI_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ANASTASIA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == TOXIC_VENOM_TRAP && rn2(evilfriday ? 2 : 7) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == INSANITY_TRAP && rn2(evilfriday ? 10 : 100) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == MADNESS_TRAP && rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == JESSICA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == SOLVEJG_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == WENDY_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == KATHARINA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ELENA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == THAI_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ELIF_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == NADJA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == SANDRA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(15) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == NATALJE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(50) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == JEANETTA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == YVONNE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == MAURAH_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == MELTEM_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == NELLY_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == EVELINE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == KARIN_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == JUEN_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == KRISTINA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == LOU_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ALMUT_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == JULIETTA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ANNEMARIE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == JIL_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == JANINA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == JANA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == KATRIN_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == GUDRUN_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ELLA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == MANUELA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(15) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == JENNIFER_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == PATRICIA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ANTJE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ANTJE_TRAP_X && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == KERSTIN_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == LAURA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == LARISSA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == NORA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == NATALIA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == SUSANNE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(25) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == LISA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == BRIDGHITTE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == JULIA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == NICOLE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == RITA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == RUTH_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == MAGDALENA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == MARLEEN_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == KLARA_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == FRIEDERIKE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(25) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == NAOMI_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == UTE_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(30) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == JASIEEN_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == YASAMAN_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == MAY_BRITT_TRAP && !Role_if(PM_FEMINIST) && !Role_if(PM_EMERA) && !FemtrapActiveJette && rn2(10) && !NastyTrapNation) goto selecttrap2;

	      if (rtrap == PREMATURE_DEATH_TRAP && rn2(evilfriday ? 10 : 200) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == RAGNAROK_TRAP && rn2(evilfriday ? 64 : 640) && !NastyTrapNation) goto selecttrap2;
		if (rtrap == SEVERE_DISENCHANT_TRAP && rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == SIN_TRAP && rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == DESTROY_ARMOR_TRAP && rn2(evilfriday ? 2 : 15) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == DIVINE_ANGER_TRAP && rn2(evilfriday ? 5 : 40) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == GENETIC_TRAP && rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == MISSINGNO_TRAP && rn2(evilfriday ? 10 : 50) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == CANCELLATION_TRAP && rn2(evilfriday ? 5 : 25) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == HOSTILITY_TRAP && rn2(evilfriday ? 2 : 4) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == BOSS_TRAP && rn2(evilfriday ? 2 : 16) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == WISHING_TRAP && rn2(500)) goto selecttrap2;
	      if (rtrap == GUILLOTINE_TRAP && rn2(evilfriday ? 10 : 200) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == BISECTION_TRAP && rn2(evilfriday ? 10 : 200) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == HORDE_TRAP && rn2(evilfriday ? 2 : 10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == IMMOBILITY_TRAP && rn2(evilfriday ? 2 : 15) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == GREEN_GLYPH && rn2(evilfriday ? 2 : 5) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == BLUE_GLYPH && rn2(evilfriday ? 4 : 10) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == YELLOW_GLYPH && rn2(evilfriday ? 3 : 7) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == ORANGE_GLYPH && rn2(evilfriday ? 5 : 20) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == BLACK_GLYPH && rn2(evilfriday ? 6 : 35) && !NastyTrapNation) goto selecttrap2;
	      if (rtrap == PURPLE_GLYPH && rn2(evilfriday ? 7 : 49) && !NastyTrapNation) goto selecttrap2;

	} while (rtrap == NO_TRAP);
	return rtrap;
}

/*
 * Create a trap on some random empty location --Amy
 */

void
makerandomtrap(givehp)
boolean givehp;
{

	int rtrap;
	rtrap = randomtrap();
	int tryct = 0;
	int x, y;
	boolean canbeinawall = FALSE;
	if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

	for (tryct = 0; tryct < 2000; tryct++) {
		x = rn1(COLNO-3,2);
		y = rn2(ROWNO);

		if (isok(x, y) && ((levl[x][y].typ > DBWALL) || canbeinawall) && !(t_at(x, y)) ) {
			(void) maketrap(x, y, rtrap, 100, givehp);
			break;
			}

	}
}

/* make an invisible trap on some random empty location --Amy */
void
makeinvisotrap()
{

	int rtrap;
	rtrap = randomtrap();
	int tryct = 0;
	int x, y;
	register struct trap *ttmp;
	boolean canbeinawall = FALSE;
	if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

	for (tryct = 0; tryct < 2000; tryct++) {
		x = rn1(COLNO-3,2);
		y = rn2(ROWNO);

		if (isok(x, y) && ((levl[x][y].typ > DBWALL) || canbeinawall) && !(t_at(x, y)) ) {
			ttmp = maketrap(x, y, rtrap, 100, FALSE);
			if (ttmp) ttmp->hiddentrap = TRUE;
			break;
			}

	}
}

/*
 * Create either a fart trap or heel trap on some random empty location, for scroll of girliness --Amy
 */

void
makegirlytrap()
{

	int rtrap;
	rtrap = rn2(2) ? FART_TRAP : HEEL_TRAP;
	int tryct = 0;
	int x, y;
	boolean canbeinawall = FALSE;
	if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

	for (tryct = 0; tryct < 2000; tryct++) {
		x = rn1(COLNO-3,2);
		y = rn2(ROWNO);

		if (isok(x, y) && ((levl[x][y].typ > DBWALL) || canbeinawall) && !(t_at(x, y)) ) {
			(void) maketrap(x, y, rtrap, 0, TRUE);
			break;
			}

	}
}

void
makespacewarstrap()
{

	int rtrap;
	rtrap = randominsidetrap();
	int tryct = 0;
	int x, y;
	boolean canbeinawall = FALSE;
	if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

	for (tryct = 0; tryct < 2000; tryct++) {
		x = rn1(COLNO-3,2);
		y = rn2(ROWNO);

		if (isok(x, y) && ((levl[x][y].typ > DBWALL) || canbeinawall) && !(t_at(x, y)) ) {
			(void) maketrap(x, y, rtrap, 0, FALSE);
			break;
			}

	}
}

/*
 * Coordinates in special level files are handled specially:
 *
 *	if x or y is -11, we generate a random coordinate.
 *	if x or y is between -1 and -10, we read one from the corresponding
 *	register (x0, x1, ... x9).
 *	if x or y is nonnegative, we convert it from relative to the local map
 *	to global coordinates.
 *	The "humidity" flag is used to insure that engravings aren't
 *	created underwater, or eels on dry land.
 */
#define DRY	0x1
#define WET	0x2
#define MOLTEN	0x4

STATIC_DCL boolean is_ok_location(SCHAR_P, SCHAR_P, int);

STATIC_OVL boolean
get_location(x, y, humidity)
schar *x, *y;
int humidity;
{
	int cpt = 0;

	if (*x >= 0) {			/* normal locations */
		*x += xstart;
		*y += ystart;
	} else if (*x > -11) {		/* special locations */
	    char ry = rloc_y[ - *x - 1][ - *y - 1];
	    char rx = rloc_x[ - *x - 1][ - *y - 1];
	    if (ry == (char)-1 || rx == (char)-1)
		return FALSE;		/* nowhere */
	    else {
		*y = ystart + ry;
		*x = xstart + rx;
	    }
	} else if (*x > -12) {		/* within random region */
	    schar t = - *y - 1;

	    do {
		*x = rn2(rarea[t].inarea.x2 - rarea[t].inarea.x1 + 1) +
		     rarea[t].inarea.x1;
		*y = rn2(rarea[t].inarea.y2 - rarea[t].inarea.y1 + 1) +
		     rarea[t].inarea.y1;
		if (!rarea[t].in_islev) {
		    *x += xstart;
		    *y += ystart;
		}
		if (is_ok_location(*x,*y,humidity)) {
		    int x1 = rarea[t].delarea.x1;
		    int x2 = rarea[t].delarea.x2;
		    int y1 = rarea[t].delarea.y1;
		    int y2 = rarea[t].delarea.y2;
		    if (!rarea[t].del_islev) {
			x1 += xstart;
			x2 += xstart;
			y1 += ystart;
			y2 += ystart;
		    }
		    if (!within_bounded_area(*x,*y,x1,y1,x2,y2)) break;
		}
	    } while (++cpt < 100);
	    if (cpt >= 100) {
		register int xx, yy;
		for (xx = rarea[t].inarea.x1; xx < rarea[t].inarea.x2; xx++)
		    for (yy = rarea[t].inarea.y1; yy < rarea[t].inarea.y2; yy++) {
			*x = xx;
			*y = yy;
			if (!rarea[t].in_islev) {
			    *x += xstart;
			    *y += ystart;
			}
			if (is_ok_location(*x,*y,humidity)) {
			    int x1 = rarea[t].delarea.x1;
			    int x2 = rarea[t].delarea.x2;
			    int y1 = rarea[t].delarea.y1;
			    int y2 = rarea[t].delarea.y2;
			    if (!rarea[t].del_islev) {
				x1 += xstart;
				x2 += xstart;
				y1 += ystart;
				y2 += ystart;
			    }
			    if (!within_bounded_area(*x,*y,x1,y1,x2,y2))
				goto found_it;
			}
		    }
		panic("get_location:  can't find a place!");
	    }
	} else {			/* random location */
	    do {
		*x = xstart + rn2((int)xsize);
		*y = ystart + rn2((int)ysize);
		if (is_ok_location(*x,*y,humidity)) break;
	    } while (++cpt < 100);
	    if (cpt >= 100) {
		register int xx, yy;
		/* last try */
		for (xx = 0; xx < xsize; xx++)
		    for (yy = 0; yy < ysize; yy++) {
			*x = xstart + xx;
			*y = ystart + yy;
			if (is_ok_location(*x,*y,humidity)) goto found_it;
		    }
		panic("get_location:  can't find a place!");
	    }
	}
found_it:;

	if (!isok(*x,*y)) {
	    impossible("get_location:  (%d,%d) out of bounds", *x, *y);
	    *x = x_maze_max; *y = y_maze_max;
	}
	return TRUE;
}

STATIC_OVL boolean
is_ok_location(x, y, humidity)
register schar x, y;
register int humidity;
{
	register int typ;

	if (Is_waterlevel(&u.uz)) return TRUE;	/* accept any spot */

	if (humidity & DRY) {
	    typ = levl[x][y].typ;
	    if (typ == ROOM || typ == AIR ||
		    typ == CLOUD || typ == ICE || typ == CORR
		 || typ == SNOW || typ == ASH || typ == SAND || typ == PAVEDFLOOR || typ == HIGHWAY
		 || typ == GRASSLAND || typ == NETHERMIST || typ == STALACTITE || typ == CRYPTFLOOR
		 || typ == BUBBLES || typ == RAINCLOUD)
		return TRUE;
	}
	if (humidity & WET) {
	    if (is_waterypool(x,y))
		return TRUE;
	}
	if (humidity & MOLTEN) {
	    if (is_lava(x,y))
		return TRUE;
	}
	return FALSE;
}

/*
 * Shuffle the registers for locations, objects or monsters
 */

STATIC_OVL void
sp_lev_shuffle(list1, list2, n)
char list1[], list2[];
int n;
{
	register int i, j;
	register char k;

	for (i = n - 1; i > 0; i--) {
		if ((j = rn2(i + 1)) == i) continue;
		k = list1[j];
		list1[j] = list1[i];
		list1[i] = k;
		if (list2) {
			k = list2[j];
			list2[j] = list2[i];
			list2[i] = k;
		}
	}
}

/*
 * Get a relative position inside a room.
 * negative values for x or y means RANDOM!
 */

STATIC_OVL void
get_room_loc(x,y, croom)
schar		*x, *y;
struct mkroom	*croom;
{
	coord c;

	if (*x <0 && *y <0) {
		if (somexy(croom, &c)) {
			*x = c.x;
			*y = c.y;
		} else
		    panic("get_room_loc : can't find a place!");
	} else {
		if (*x < 0)
		    *x = rn2(croom->hx - croom->lx + 1);
		if (*y < 0)
		    *y = rn2(croom->hy - croom->ly + 1);
		*x += croom->lx;
		*y += croom->ly;
	}
}

/*
 * Get a relative position inside a room.
 * negative values for x or y means RANDOM!
 */

STATIC_OVL void
get_free_room_loc(x,y, croom)
schar		*x, *y;
struct mkroom	*croom;
{
	schar try_x, try_y;
	register int trycnt = 0;

	do {
	    try_x = *x,  try_y = *y;
	    get_room_loc(&try_x, &try_y, croom);
	} while (levl[try_x][try_y].typ != ROOM && ++trycnt <= 100);

	if (trycnt > 100)
	    panic("get_free_room_loc:  can't find a place!");
	*x = try_x,  *y = try_y;
}

boolean
check_room(lowx, ddx, lowy, ddy, vault, anywhere)
xchar *lowx, *ddx, *lowy, *ddy;
boolean vault;
boolean anywhere;
{
	register int x,y,hix = *lowx + *ddx, hiy = *lowy + *ddy;
	register struct rm *lev;
	int xlim, ylim, ymax;

	xlim = XLIM + (vault ? 1 : 0);
	ylim = YLIM + (vault ? 1 : 0);

	/* Allow rooms to be closer together. For warper race characters this will be the case even more often. --Amy */

	if (anywhere) {

		xlim = xlim - rnd(10);
		ylim = ylim - rnd(10);

	}

	/* After all, a warped dungeon is what we'd expect from a character called warper... */

	if (*lowx < 3)		*lowx = 3;
	if (*lowy < 2)		*lowy = 2;
	if (hix > COLNO-3)	hix = COLNO-3;
	if (hiy > ROWNO-3)	hiy = ROWNO-3;
chk:
	if (hix <= *lowx || hiy <= *lowy)	return FALSE;

	/* check area around room (and make room smaller if necessary) */
	for (x = *lowx - xlim; x<= hix + xlim; x++) {
		if(x <= 0 || x >= COLNO) continue;
		y = *lowy - ylim;	ymax = hiy + ylim;
		if(y < 0) y = 0;
		if(ymax >= ROWNO) ymax = (ROWNO-1);
		lev = &levl[x][y];
		for (; y <= ymax; y++) {
			if (lev++->typ && !anywhere) {
#ifdef DEBUG
				if(!vault)
				    debugpline("strange area [%d,%d] in check_room.",x,y);
#endif
				if (!rn2(3))	return FALSE;
				if (x < *lowx)
				    *lowx = x + xlim + 1;
				else
				    hix = x - xlim - 1;
				if (y < *lowy)
				    *lowy = y + ylim + 1;
				else
				    hiy = y - ylim - 1;
				goto chk;
			}
		}
	}
	*ddx = hix - *lowx;
	*ddy = hiy - *lowy;
	return TRUE;
}

/*
 * Create a new room.
 * This is still very incomplete...
 */

boolean
create_room(x,y,w,h,xal,yal,rtype,rlit, canbeshaped, anywhere)
xchar	x,y;
xchar	w,h;
xchar	xal,yal;
xchar	rtype, rlit;
boolean canbeshaped;
boolean anywhere;
{
    /*  
     * numeric args that are -1 mean random  
     * x and y are position within the rectangle grid for the level  
     *        I *think* these values range from 1 to 5.  
     * w and h are size (width and height)  
     * xal and yal are alignment (LEFT/TOP, CENTER, RIGHT/BOTTOM).  
     * footmp is the current working value of foo; this is typically  
     *        equal to foo, unless foo is -1 (random).  
     * xabs and yabs are the actual x and y coordinates where the  
     *        room will be placed on the level map.  
     * */  
	xchar	xabs, yabs;
	int	wtmp, htmp, xaltmp, yaltmp, xtmp, ytmp;
	NhRect	*r1 = 0, r2;
	int	trycnt = 0;
	boolean	vault = FALSE;
	int	xlim = XLIM, ylim = YLIM;

	/*if (anywhere) pline("anywhere!");*/

	if (anywhere) {

		xlim = xlim - rnd(10);
		ylim = ylim - rnd(10);

	}

	if (rtype == -1)	/* Is the type random ? */
	    rtype = OROOM;

	if (rtype == VAULT) {
		vault = TRUE;
		canbeshaped = FALSE;
		xlim++;
		ylim++;
	}

	/* on low levels the room is lit (usually) */
	/* some other rooms may require lighting */

	/* is light state random ? */
	if (rlit == -1)
	    rlit = (rnd(1+abs(depth(&u.uz))) < 25 && rn2(77)) ? TRUE : FALSE;

	/*
	 * Here we will try to create a room. If some parameters are
	 * random we are willing to make several try before we give
	 * it up.
	 */
	do {
		xchar xborder, yborder;
		wtmp = w; htmp = h;
		xtmp = x; ytmp = y;
		xaltmp = xal; yaltmp = yal;

		/* First case : a totaly random room */

		if((xtmp < 0 && ytmp <0 && wtmp < 0 && xaltmp < 0 &&
		   yaltmp < 0) || vault) {
			xchar hx, hy, lx, ly, dx, dy;
			r1 = rnd_rect(); /* Get a random rectangle */

			if (!r1) { /* No more free rectangles ! */
#ifdef DEBUG
				debugpline("No more rects...");
#endif
				return FALSE;
			}
			hx = r1->hx;
			hy = r1->hy;
			lx = r1->lx;
			ly = r1->ly;
			if (vault)
			    dx = dy = 1;
			else {
#ifdef BIGSLEX
				dx = 2 + rn2((hx-lx > 28) ? 20 : 12);
				dy = 2 + rn2(6);

				if (!rn2(500) && trycnt < 200) dx += rnd(160);
				if (!rn2(500) && trycnt < 200) dy += rnd(48);

				if (!rn2(50) && trycnt < 200) dx += rnd(80);
				if (!rn2(50) && trycnt < 200) dy += rnd(24);

				if (!rn2(5) && trycnt < 200) dx += rnd(30);
				if (!rn2(5) && trycnt < 200) dy += rnd(10);
#else
				dx = 2 + rn2((hx-lx > 28) ? 12 : 8);
				dy = 2 + rn2(4);

				if (!rn2(1000) && trycnt < 200) dx += rnd(80);
				if (!rn2(1000) && trycnt < 200) dy += rnd(24);

				if (!rn2(100) && trycnt < 200) dx += rnd(40);
				if (!rn2(100) && trycnt < 200) dy += rnd(12);

				if (!rn2(10) && trycnt < 200) dx += rnd(20);
				if (!rn2(10) && trycnt < 200) dy += rnd(6);
#endif

				/*if(dx*dy > 50)
				    dy = 50/dx;*/
			}

				/*if (wizard && trycnt == 500) pline("try count 500 a");*/

				if ((trycnt > 500) && (dx > 10)) dx = 10;
				if ((trycnt > 1000) && (dx > 6)) dx = 6;
				if ((trycnt > 2000) && (dx > 4)) dx = 4;
				if ((trycnt > 3000) && (dx > 2)) dx = 2;
				if ((trycnt > 500) && (dy > 6)) dy = 6;
				if ((trycnt > 1000) && (dy > 4)) dy = 4;
				if ((trycnt > 2000) && (dy > 3)) dy = 3;
				if ((trycnt > 3000) && (dy > 2)) dy = 2;

			xborder = (lx > 0 && hx < COLNO -1) ? 2*xlim : xlim+1;
			yborder = (ly > 0 && hy < ROWNO -1) ? 2*ylim : ylim+1;
			if(hx-lx < dx + 3 + xborder ||
			   hy-ly < dy + 3 + yborder) {
				r1 = 0;
				continue;
			}
			xabs = lx + (lx > 0 ? xlim : 3)
			    + rn2(hx - (lx>0?lx : 3) - dx - xborder + 1);
			yabs = ly + (ly > 0 ? ylim : 2)
			    + rn2(hy - (ly>0?ly : 2) - dy - yborder + 1);
			if (ly == 0 && hy >= (ROWNO-1) &&
			    (!nroom || !rn2(nroom)) && (yabs+dy > ROWNO/2)) {
			    yabs = rn1(3, 2);
			    if(nroom < 4 && dy>1) dy--;
		        }
			if (!check_room(&xabs, &dx, &yabs, &dy, vault, anywhere)) {
				r1 = 0;
				continue;
			}
			wtmp = dx+1;
			htmp = dy+1;
			r2.lx = xabs-1; r2.ly = yabs-1;
			r2.hx = xabs + wtmp;
			r2.hy = yabs + htmp;
		} else {	/* Only some parameters are random */
			int rndpos = 0;
			if (xtmp < 0 && ytmp < 0) { /* Position is RANDOM */
				xtmp = rnd(5);
				ytmp = rnd(5);
				rndpos = 1;
			}
			if (wtmp < 0 || htmp < 0) { /* Size is RANDOM */
				wtmp = rn1(15, 3);
				htmp = rn1(8, 2);

				if (!rn2(1000) && trycnt < 200) wtmp += rnd(80);
				if (!rn2(1000) && trycnt < 200) htmp += rnd(24);

				if (!rn2(100) && trycnt < 200) wtmp += rnd(40);
				if (!rn2(100) && trycnt < 200) htmp += rnd(12);

				if (!rn2(10) && trycnt < 200) wtmp += rnd(20);
				if (!rn2(10) && trycnt < 200) htmp += rnd(6);

			}

				/*if (wizard && trycnt == 501) pline("try count 500 b");*/

				if (trycnt > 500 && wtmp > 10) wtmp = 10;
				if (trycnt > 1000 && wtmp > 6) wtmp = 6;
				if (trycnt > 2000 && wtmp > 4) wtmp = 4;
				if (trycnt > 3000 && wtmp > 2) wtmp = 2;
				if (trycnt > 500 && htmp > 6) htmp = 6;
				if (trycnt > 1000 && htmp > 4) htmp = 4;
				if (trycnt > 2000 && htmp > 3) htmp = 3;
				if (trycnt > 3000 && htmp > 2) htmp = 2;

			if (xaltmp == -1) /* Horizontal alignment is RANDOM */
			    xaltmp = rnd(3);
			if (yaltmp == -1) /* Vertical alignment is RANDOM */
			    yaltmp = rnd(3);

			/* Try to generate real (absolute) coordinates here! */

			xabs = (((xtmp-1) * COLNO) / 5) + 1;
			yabs = (((ytmp-1) * ROWNO) / 5) + 1;
			switch (xaltmp) {
			      case LEFT:
				break;
			      case RIGHT:
				xabs += (COLNO / 5) - wtmp;
				break;
			      case CENTER:
				xabs += ((COLNO / 5) - wtmp) / 2;
				break;
			}
			switch (yaltmp) {
			      case TOP:
				break;
			      case BOTTOM:
				yabs += (ROWNO / 5) - htmp;
				break;
			      case CENTER:
				yabs += ((ROWNO / 5) - htmp) / 2;
				break;
			}

			if (xabs + wtmp - 1 > COLNO - 2)
			    xabs = COLNO - wtmp - 3;
			if (xabs < 2)
			    xabs = 2;
			if (yabs + htmp - 1> ROWNO - 2)
			    yabs = ROWNO - htmp - 3;
			if (yabs < 2)
			    yabs = 2;

			/* Try to find a rectangle that fit our room ! */

			r2.lx = xabs-1; r2.ly = yabs-1;
			r2.hx = xabs + wtmp + rndpos;
			r2.hy = yabs + htmp + rndpos;
			r1 = get_rect(&r2);
		}

	} while (++trycnt <= 5000 && !r1);
	if (!r1) {	/* creation of room failed ? */
		return FALSE;
	}
	split_rects(r1, &r2);

	if (!vault) {
		smeq[nroom] = nroom;
		add_room(xabs, yabs, xabs+wtmp-1, yabs+htmp-1,
			 rlit, rtype, FALSE, TRUE, FALSE);
	} else {
		rooms[nroom].lx = xabs;
		rooms[nroom].ly = yabs;
	}
	return TRUE;
}

/*
 * Create a subroom in room proom at pos x,y with width w & height h.
 * x & y are relative to the parent room.
 */

STATIC_OVL boolean
create_subroom(proom, x, y, w,  h, rtype, rlit)
struct mkroom *proom;
xchar x,y;
xchar w,h;
xchar rtype, rlit;
{
	xchar width, height;

	width = proom->hx - proom->lx + 1;
	height = proom->hy - proom->ly + 1;

	/* There is a minimum size for the parent room */
	if (width < 4 || height < 4)
	    return FALSE;

	/* Check for random position, size, etc... */

	if (w == -1)
	    w = rnd(width - 3);
	if (h == -1)
	    h = rnd(height - 3);
	if (x == -1)
	    x = rnd(width - w - 1) - 1;
	if (y == -1)
	    y = rnd(height - h - 1) - 1;
	if (x == 1)
	    x = 0;
	if (y == 1)
	    y = 0;
	if ((x + w + 1) == width)
	    x++;
	if ((y + h + 1) == height)
	    y++;
	if (rtype == -1)
	    rtype = OROOM;
	if (rlit == -1)
	    rlit = (rnd(1+abs(depth(&u.uz))) < 11 && rn2(77)) ? TRUE : FALSE;
	add_subroom(proom, proom->lx + x, proom->ly + y,
		    proom->lx + x + w - 1, proom->ly + y + h - 1,
		    rlit, rtype, FALSE);
	return TRUE;
}

/*
 * Create a new door in a room.
 * It's placed on a wall (north, south, east or west).
 */

STATIC_OVL void
create_door(dd, broom)
room_door *dd;
struct mkroom *broom;
{
	int	x, y;
	int	trycnt = 0;
	int	i;

	if (dd->secret == -1)
	    dd->secret = rn2(2);

	if (dd->mask == -1) {
		/* is it a locked door, closed, or a doorway? */
		if (!dd->secret) {
			if(!rn2(3)) {
				if(!rn2(5))
				    dd->mask = D_ISOPEN;
				else if(!rn2(6))
				    dd->mask = D_LOCKED;
				else
				    dd->mask = D_CLOSED;
				if (dd->mask != D_ISOPEN && !rn2(25))
				    dd->mask |= D_TRAPPED;
			} else
			    dd->mask = D_NODOOR;
		} else {
			if(!rn2(5))	dd->mask = D_LOCKED;
			else		dd->mask = D_CLOSED;

			if(!rn2(20)) dd->mask |= D_TRAPPED;
		}
	}

	do {
		register int dwall, dpos;

		dwall = dd->wall;
		if (dwall == -1)	/* The wall is RANDOM */
		    dwall = 1 << rn2(4);

		dpos = dd->pos;
		if (dpos == -1)	/* The position is RANDOM */
		    dpos = rn2((dwall == W_WEST || dwall == W_EAST) ?
			    (broom->hy - broom->ly) : (broom->hx - broom->lx));

		/* Convert wall and pos into an absolute coordinate! */

		switch (dwall) {
		      case W_NORTH:
			y = broom->ly - 1;
			x = broom->lx + dpos;
			break;
		      case W_SOUTH:
			y = broom->hy + 1;
			x = broom->lx + dpos;
			break;
		      case W_WEST:
			x = broom->lx - 1;
			y = broom->ly + dpos;
			break;
		      case W_EAST:
			x = broom->hx + 1;
			y = broom->ly + dpos;
			break;
		      default:
			x = y = 0;
			panic("create_door: No wall for door!");
			break;
		}
		if (okdoor(x,y))
		    break;
	} while (++trycnt <= 100);
	if (trycnt > 100) {
		if (wizard) pline("create_door: Can't find a proper place!");
		return;
	}
	i = add_door(x,y,broom);
	doors[i].arti_key = dd->arti_key;
	levl[x][y].typ = (dd->secret ? SDOOR : DOOR);
	levl[x][y].doormask = dd->mask;
}

/*
 * Create a secret door in croom on any one of the specified walls.
 */
void
create_secret_door(croom, walls)
    struct mkroom *croom;
    xchar walls; /* any of W_NORTH | W_SOUTH | W_EAST | W_WEST (or W_ANY) */
{
    xchar sx, sy; /* location of the secret door */
    int count;

    for(count = 0; count < 100; count++) {
	sx = rn1(croom->hx - croom->lx + 1, croom->lx);
	sy = rn1(croom->hy - croom->ly + 1, croom->ly);

	switch(rn2(4)) {
	case 0:  /* top */
	    if(!(walls & W_NORTH)) continue;
	    sy = croom->ly-1; break;
	case 1: /* bottom */
	    if(!(walls & W_SOUTH)) continue;
	    sy = croom->hy+1; break;
	case 2: /* left */
	    if(!(walls & W_EAST)) continue;
	    sx = croom->lx-1; break;
	case 3: /* right */
	    if(!(walls & W_WEST)) continue;
	    sx = croom->hx+1; break;
	}

	if(okdoor(sx,sy)) {
	    levl[sx][sy].typ = SDOOR;
	    levl[sx][sy].doormask = D_CLOSED;
	    add_door(sx,sy,croom);
	    return;
	}
    }

      sy = croom->hy+1;
	sx = croom->hx+1;
    levl[sx][sy].typ = SDOOR;
    levl[sx][sy].doormask = D_CLOSED;
    add_door(sx,sy,croom);
    return;
    /* impossible("couldn't create secret door on any walls 0x%x", walls); */
}

/*
 * Create a trap in a room.
 */

STATIC_OVL void
create_trap(t,croom)
trap	*t;
struct mkroom	*croom;
{
    schar	x,y;
    coord	tm;

    if (rn2(100) < t->chance) {
	x = t->x;
	y = t->y;
	if (croom)
	    get_free_room_loc(&x, &y, croom);
	else
	    if (!get_location(&x, &y, DRY))
		return;

	tm.x = x;
	tm.y = y;

	mktrap(t->type, 1, (struct mkroom*) 0, &tm, TRUE);
	/*pline("traptest %d", t->type);*/
    }
}

/*
 * Create a monster in a room.
 */

STATIC_OVL int
noncoalignment(alignment)
aligntyp alignment;
{
	int k;

	k = rn2(2);
	if (!alignment)
		return(k ? -1 : 1);
	return(k ? -alignment : 0);
}

STATIC_OVL void
create_monster(m,croom)
monster	*m;
struct mkroom	*croom;
{
    struct monst *mtmp;
    schar x, y;
    char class;
    aligntyp amask;
    coord cc;
    struct permonst *pm;
    unsigned g_mvflags;

    if (rn2(100) < m->chance) {

	/* ugly hack for monster symbols with ascii chars greater than 127 --Amy */

	if (m->class == -79) m->class = 177;
	if (m->class == -80) m->class = 176;
	if (m->class == -81) m->class = 175;
	if (m->class == -89) m->class = 167;

	if (m->class >= 0)
	    class = (char) def_char_to_monclass((char)m->class);
	else if (m->class > -11)
	    class = (char) def_char_to_monclass(rmonst[- m->class - 1]);
	else
	    class = 0;

	if (class == MAXMCLASSES)
	    panic("create_monster: unknown monster class '%c'", m->class);

	amask = (m->align == AM_SPLEV_CO) ?
			Align2amask(u.ualignbase[A_ORIGINAL]) :
		(m->align == AM_SPLEV_NONCO) ?
			Align2amask(noncoalignment(u.ualignbase[A_ORIGINAL])) :
		(m->align <= -11) ? induced_align(80) :
		(m->align < 0 ? ralign[-m->align-1] : m->align);

	if (!class)
	    pm = (struct permonst *) 0;
	else if (m->id != NON_PM) {
#if 0	/* OBSOLETE */
	    if (flags.female && Role_if(PM_ELF) /*&& !u.uelf_drow*/ && m->id == PM_EARENDIL)
		m->id = PM_ELWING;
#endif

	    if (flags.female && Role_if(PM_ELPH) && m->id == PM_EARENDIL) /* elf role re-enabled by Amy */
		m->id = PM_ELWING;

	    /* in the Gnome Mines, make the gnomes & dwarves into            
	       orcs, ogres, & zombies (because gnomes & dwarves are friendly...
	       the mines would be hella easy otherwise) */
		if (In_mines(&u.uz) && (
			Race_if(PM_DWARF) ||
			Race_if(PM_GNOME))) {
		switch (m->id) {
		  case PM_GNOME: m->id = PM_GNOME_ZOMBIE; break;
		  case PM_GNOMISH_WIZARD: m->id = PM_ORC_SHAMAN; break;
		  case PM_GNOME_LORD: m->id = PM_GNOME_MUMMY; break;
		  case PM_GNOME_KING: m->id = PM_OGRE; break;
		  case PM_DWARF: m->id = PM_DWARF_ZOMBIE; break;
		  case PM_DWARF_LORD: m->id = PM_DWARF_MUMMY; break;
		  case PM_DWARF_KING: m->id = PM_WAR_ORC; break;
		}
	    }
	    pm = &mons[m->id];
	    g_mvflags = (unsigned) mvitals[monsndx(pm)].mvflags;
	    if ((pm->geno & G_UNIQ) && (g_mvflags & G_EXTINCT))
		goto m_done;
	    else if (g_mvflags & G_GONE)	/* genocided or extinct */
		pm = (struct permonst *) 0;	/* make random monster */
	} else {
	    pm = mkclass(class,MKC_ULIMIT);
	    /* if we can't get a specific monster type (pm == 0) then all
	       the monsters of acceptable difficulty in the given class
	       have been genocided, so settle for a random monster */
	    /* Amy edit: what the fuck man why did this allow G_NOGEN monsters to spawn */
	}
	x = m->x;
	y = m->y;
	if (croom)
	    get_room_loc(&x, &y, croom);
	else {
	    boolean found;
	    if (!pm || !is_swimmer(pm) && !likes_lava(pm))
		found = get_location(&x, &y, DRY);
	    else if (pm->mlet == S_EEL)
		found = get_location(&x, &y, DRY|WET);
	    else if (likes_lava(pm))
		found = get_location(&x, &y, DRY|MOLTEN);
	    else
		found = get_location(&x, &y, DRY|WET);
	    if (!found)
		goto m_done;	/* nowhere */
	}
	/* try to find a close place if someone else is already there */
	if (MON_AT(x,y) && enexto(&cc, x, y, pm))
	    x = cc.x,  y = cc.y;

	if(m->align != -12)
	    mtmp = mk_roamer(pm, Amask2align(amask), x, y, m->peaceful);
	else if(PM_ARCHEOLOGIST <= m->id && m->id <= PM_WIZARD)
	         mtmp = mk_mplayer(pm, x, y, FALSE);
	else mtmp = makemon(pm, x, y, NO_MM_FLAGS);

	if (mtmp) {
	    /* handle specific attributes for some special monsters */
	    if (m->name.str) mtmp = christen_monst(mtmp, m->name.str);

	    /*
	     * This is currently hardwired for mimics only.  It should
	     * eventually be expanded.
	     */
	    if (m->appear_as.str && mtmp->data->mlet == S_MIMIC) {
		int i;

		switch (m->appear) {
		    case M_AP_NOTHING:
			impossible(
		"create_monster: mon has an appearance, \"%s\", but no type",
				m->appear_as.str);
			break;

		    case M_AP_FURNITURE:
			for (i = 0; i < MAXPCHARS; i++)
			    if (!strcmp(defsyms[i].explanation,
					m->appear_as.str))
				break;
			if (i == MAXPCHARS) {
			    impossible(
				"create_monster: can't find feature \"%s\"",
				m->appear_as.str);
			} else {
			    mtmp->m_ap_type = M_AP_FURNITURE;
			    mtmp->mappearance = i;
			}
			break;

		    case M_AP_OBJECT:
			for (i = 0; i < NUM_OBJECTS; i++)
			    if (OBJ_NAME(objects[i]) &&
				!strcmp(OBJ_NAME(objects[i]),m->appear_as.str))
				break;
			if (i == NUM_OBJECTS) {
			    impossible(
				"create_monster: can't find object \"%s\"",
				m->appear_as.str);
			} else {
			    mtmp->m_ap_type = M_AP_OBJECT;
			    mtmp->mappearance = i;
			}
			break;

		    case M_AP_MONSTER:
			/* note: mimics don't appear as monsters! */
			/*	 (but chameleons can :-)	  */
		    default:
			impossible(
		"create_monster: unimplemented mon appear type [%d,\"%s\"]",
				m->appear, m->appear_as.str);
			break;
		}
		if (does_block(x, y, &levl[x][y]))
		    block_point(x, y);
	    }

	    if (m->peaceful >= 0) {
		mtmp->mpeaceful = m->peaceful;
		/* changed mpeaceful again; have to reset malign */
		set_malign(mtmp);
	    }
	    if (m->asleep >= 0) {
#ifdef UNIXPC
		/* optimizer bug strikes again */
		if (m->asleep)
			mtmp->msleeping = 1;
		else
			mtmp->msleeping = 0;
#else
		mtmp->msleeping = m->asleep;
#endif
	    }
	}

    }		/* if (rn2(100) < m->chance) */
 m_done:
    free(m->name.str);
    free(m->appear_as.str);
}

/*
 * Create an object in a room.
 */

STATIC_OVL void
create_object(o,croom)
object	*o;
struct mkroom	*croom;
{
    struct obj *otmp;
    schar x, y;
    char c;
    boolean named;	/* has a name been supplied in level description? */

    if (rn2(100) < o->chance) {
	named = o->name.str ? TRUE : FALSE;

	x = o->x; y = o->y;
	if (croom)
	    get_room_loc(&x, &y, croom);
	else
	    if (!get_location(&x, &y, DRY))	/* nowhere */
		goto o_done;

	if (o->class >= 0)
	    c = o->class;
	else if (o->class > -11)
	    c = robjects[ -(o->class+1)];
	else
	    c = 0;

	if (!c)
	    otmp = mkobj_at(RANDOM_CLASS, x, y, !named, FALSE);
	else if (o->id != -1)
	    otmp = mksobj_at(o->id, x, y, TRUE, !named, FALSE);
	else {
	    /*
	     * The special levels are compiled with the default "text" object
	     * class characters.  We must convert them to the internal format.
	     */
	    char oclass = (char) def_char_to_objclass(c);

	    if (oclass == MAXOCLASSES)
		panic("create_object:  unexpected object class '%c'",c);

	    /* KMH -- Create piles of gold properly */
	    /* Bruce Cox/WAC - some clean ups */
	    if (oclass == COIN_CLASS && !o->containment) {
		mkgold(0L, x, y);
		otmp = g_at(x,y);
	    } else
		otmp = mkobj_at(oclass, x, y, !named, FALSE);
	}

	if (!otmp) return;

	if (o->spe != -127)	/* That means NOT RANDOM! */
	    otmp->spe = (schar)o->spe;

	switch (o->curse_state) {
	      case 1:	bless(otmp); break; /* BLESSED */
	      case 2:	unbless(otmp); uncurse(otmp, TRUE); break; /* uncursed */
	      case 3:	curse(otmp); break; /* CURSED */
	      default:	break;	/* Otherwise it's random and we're happy
				 * with what mkobj gave us! */
	}

	/*	corpsenm is "empty" if -1, random if -2, otherwise specific */
	if (o->corpsenm != NON_PM) {
	    if (o->corpsenm == NON_PM - 1) otmp->corpsenm = rndmonnum();
	    else otmp->corpsenm = o->corpsenm;
	    otmp->owt = weight(otmp);
	}

	if (otmp->otyp == EGG && In_spiders(&u.uz)) {
	    otmp->corpsenm = PM_GIANT_SPIDER;
	    otmp->age = monstermoves;
	}
	/* assume we wouldn't be given an egg corpsenm unless it was
	   hatchable */
	if (otmp->otyp == EGG && otmp->corpsenm != NON_PM) {
	    if (dead_species(otmp->otyp, TRUE))
		kill_egg(otmp);	/* make sure nothing hatches */
	    else
		attach_egg_hatch_timeout(otmp);	/* attach new hatch timeout */
	}

	if (named)
	    otmp = oname(otmp, o->name.str);

	switch(o->containment) {
	    static struct obj *container = 0;

	    /* contents */
	    case 1:
		if (!container) {
		    impossible("create_object: no container");
		    break;
		}
		if (container && container->otyp == GOLD_PIECE) { /* minimalist fix, though it doesn't completely fix things */
		    break;
		}
		remove_object(otmp);
		(void) add_to_container(container, otmp, TRUE);
		goto o_done;		/* don't stack, but do other cleanup */
	    /* container */
	    case 2:
		delete_contents(otmp);
		container = otmp;
		break;
	    /* nothing */
	    case 0: break;

	    default: impossible("containment type %d?", (int) o->containment);
	}

	/* Medusa level special case: statues are petrified monsters, so they
	 * are not stone-resistant and have monster inventory.  They also lack
	 * other contents, but that can be specified as an empty container.
	 */
	if (o->id == STATUE && Is_medusa_level(&u.uz) &&
		    o->corpsenm == NON_PM) {
	    struct monst *was;
	    struct obj *obj;
	    int wastyp;
	    int i = 0; /* prevent endless loop in case makemon always fails */ 

	    /* Named random statues are of player types, and aren't stone-
	     * resistant (if they were, we'd have to reset the name as well as
	     * setting corpsenm).
	     */
	    for (wastyp = otmp->corpsenm; i < 1000; i++) {
		/* makemon without rndmonst() might create a group */
		was = makemon(&mons[wastyp], 0, 0, NO_MM_FLAGS);
		if (was) {
			if (!resists_ston(was)) break;
			mongone(was);
		}
		wastyp = rndmonnum(); 
	    }
		if (was) {
		otmp->corpsenm = wastyp;
		while(was->minvent) {
		obj = was->minvent;
		obj->owornmask = 0;
		obj_extract_self(obj);
		(void) add_to_container(otmp, obj, TRUE);
		}
		otmp->owt = weight(otmp);
		mongone(was);
	    }
	}

#ifdef RECORD_ACHIEVE
        /* Nasty hack here: try to determine if this is the Mines or Sokoban
         * "prize" and then set record_achieve_special (maps to corpsenm)
         * for the object.  That field will later be checked to find out if
         * the player obtained the prize. */
        if(otmp->otyp == LUCKSTONE && Is_mineend_level(&u.uz)) {
                otmp->record_achieve_special = 1;
        } else if((otmp->otyp == AMULET_OF_REFLECTION || otmp->otyp == GAUNTLETS_OF_REFLECTION || otmp->otyp == RIN_POLYMORPH_CONTROL || otmp->otyp == RIN_TELEPORT_CONTROL || otmp->otyp == SHIELD_OF_MOBILITY || otmp->otyp == HELM_OF_DRAIN_RESISTANCE || otmp->otyp == CYAN_DRAGON_SCALE_MAIL || otmp->otyp == FLYING_BOOTS ||
                   otmp->otyp == BAG_OF_HOLDING) && 
                  Is_sokoend_level(&u.uz)) {
                otmp->record_achieve_special = 1;
        } else if (otmp->otyp == STONE_OF_MAGIC_RESISTANCE && Is_deepend_level(&u.uz)) {
                otmp->record_achieve_special = 1;
	  }
#endif

	stackobj(otmp);

	if (o->oflags & OBJF_LIT)
	    begin_burn(otmp, FALSE);
	if (o->oflags & OBJF_BURIED)
	    otmp = bury_an_obj(otmp);

    }		/* if (rn2(100) < o->chance) */
 o_done:
    free(o->name.str);
}

/*
 * Randomly place a specific engraving, then release its memory.
 */
STATIC_OVL void
create_engraving(e, croom)
engraving *e;
struct mkroom *croom;
{
	xchar x, y;
	boolean found = TRUE;

	x = e->x,  y = e->y;
	if (croom)
	    get_room_loc(&x, &y, croom);
	else
	    found = get_location(&x, &y, DRY);

	if (found)
	make_engr_at(x, y, e->engr.str, 0L, e->etype);
	free((void *) e->engr.str);
}

/*
 * Create stairs in a room.
 *
 */

STATIC_OVL void
create_stairs(s,croom)
stair	*s;
struct mkroom	*croom;
{
	schar		x,y;

	x = s->x; y = s->y;
	get_free_room_loc(&x, &y, croom);
	mkstairs(x,y,(char)s->up, croom);
}

/*
 * Create an altar in a room.
 */

STATIC_OVL void
create_altar(a, croom)
	altar		*a;
	struct mkroom	*croom;
{
	schar		sproom,x,y;
	aligntyp	amask;
	boolean		croom_is_temple = TRUE;
	int oldtyp; 

	x = a->x; y = a->y;

	if (croom) {
	    get_free_room_loc(&x, &y, croom);
	    if (croom->rtype != TEMPLE)
		croom_is_temple = FALSE;
	} else {
	    if (!get_location(&x, &y, DRY))
		return;		/* nowhere */
	    if ((sproom = (schar) *in_rooms(x, y, TEMPLE)) != 0)
		croom = &rooms[sproom - ROOMOFFSET];
	    else
		croom_is_temple = FALSE;
	}

	/* check for existing features */
	oldtyp = levl[x][y].typ;
	if (oldtyp == STAIRS || oldtyp == LADDER)
	    return;

	a->x = x;
	a->y = y;

	/* Is the alignment random ?
	 * If so, it's an 80% chance that the altar will be co-aligned.
	 *
	 * The alignment is encoded as amask values instead of alignment
	 * values to avoid conflicting with the rest of the encoding,
	 * shared by many other parts of the special level code.
	 */

	amask = (a->align == AM_SPLEV_CO) ?
			Align2amask(u.ualignbase[A_ORIGINAL]) :
		(a->align == AM_SPLEV_NONCO) ?
			Align2amask(noncoalignment(u.ualignbase[A_ORIGINAL])) :
		(a->align == -11) ? induced_align(80) :
		(a->align < 0 ? ralign[-a->align-1] : a->align);

	levl[x][y].typ = ALTAR;
	levl[x][y].altarmask = amask;

	if (a->shrine < 0) a->shrine = rn2(2);	/* handle random case */

	if (oldtyp == FOUNTAIN)
	    level.flags.nfountains--;
	else if (oldtyp == SINK)
	    level.flags.nsinks--;

	if (!croom_is_temple || !a->shrine) return;

	if (a->shrine) {	/* Is it a shrine  or sanctum? */
		priestini(&u.uz, croom, x, y, (a->shrine > 1));
		levl[x][y].altarmask |= AM_SHRINE;
		level.flags.has_temple = TRUE;

		if (!rn2(10)) {
			register struct obj *otmp;
			otmp = mksobj_at(POT_WATER, x, y, FALSE, FALSE, FALSE);
			if (otmp) {
				  bless(otmp);
			}
		}

	}
}

/*
 * Create a gold pile in a room.
 */

STATIC_OVL void
create_gold(g,croom)
gold *g;
struct mkroom	*croom;
{
	schar		x,y;

	x = g->x; y= g->y;
	if (croom)
	    get_room_loc(&x, &y, croom);
	else
	    if (!get_location(&x, &y, DRY))
		return;		/* nowhere */

	if (g->amount == -1)
	    g->amount = rnd(200);
	(void) mkgold((long) g->amount, x, y);
}

/*
 * Create a feature (e.g a fountain) in a room.
 */

STATIC_OVL void
create_feature(fx, fy, croom, typ)
int		fx, fy;
struct mkroom	*croom;
int		typ;
{
	schar		x,y;
	int		trycnt = 0;

	x = fx;  y = fy;
	if (croom) {
	    if (x < 0 && y < 0)
		do {
		    x = -1;  y = -1;
		    get_room_loc(&x, &y, croom);
		} while (++trycnt <= 200 && occupied(x,y));
	    else
		get_room_loc(&x, &y, croom);
	    if(trycnt > 200)
		return;
	} else {
	    if (!get_location(&x, &y, DRY))
		return;		/* nowhere */
	}
	/* Don't cover up an existing feature (particularly randomly
	   placed stairs).  However, if the _same_ feature is already
	   here, it came from the map drawing and we still need to
	   update the special counters. */
	if (IS_FURNITURE(levl[x][y].typ) && levl[x][y].typ != typ)
	    return;

	levl[x][y].typ = typ;
	if (typ == FOUNTAIN)
	    level.flags.nfountains++;
	else if (typ == SINK)
	    level.flags.nsinks++;
/*      else if (typ == TOILET)
	    level.flags.nsinks++;*/
}

/*
 * Search for a door in a room on a specified wall.
 */

STATIC_OVL boolean
search_door(croom,x,y,wall,cnt)
struct mkroom *croom;
xchar *x, *y;
xchar wall;
int cnt;
{
	int dx, dy;
	int xx,yy;

	switch(wall) {
	      case W_NORTH:
		dy = 0; dx = 1;
		xx = croom->lx;
		yy = croom->hy + 1;
		break;
	      case W_SOUTH:
		dy = 0; dx = 1;
		xx = croom->lx;
		yy = croom->ly - 1;
		break;
	      case W_EAST:
		dy = 1; dx = 0;
		xx = croom->hx + 1;
		yy = croom->ly;
		break;
	      case W_WEST:
		dy = 1; dx = 0;
		xx = croom->lx - 1;
		yy = croom->ly;
		break;
	      default:
		dx = dy = xx = yy = 0;
		panic("search_door: Bad wall!");
		break;
	}
	while (xx <= croom->hx+1 && yy <= croom->hy+1) {
		if (IS_DOOR(levl[xx][yy].typ) || levl[xx][yy].typ == SDOOR) {
			*x = xx;
			*y = yy;
			if (cnt-- <= 0)
			    return TRUE;
		}
		xx += dx;
		yy += dy;
	}
	return FALSE;
}

/*
 * Dig a corridor between two points.
 */

boolean
dig_corridor(org,dest,nxcor,ftyp,btyp)
coord *org, *dest;
boolean nxcor;
schar ftyp, btyp;
{
	register int dx=0, dy=0, dix, diy, cct;
	register struct rm *crm;
	register int tx, ty, xx, yy;
	int randomcolouur;
	randomcolouur = rnd(16);

	xx = org->x;  yy = org->y;
	tx = dest->x; ty = dest->y;

	/* there's no real reason to restrict corridors to tiles one square away from the edge...
	 * in fact, that restriction causes disjointed levels with the new sporkhack code in mklev.c --Amy */
	if (xx </*=*/ 0 || yy </*=*/ 0 || tx </*=*/ 0 || ty </*=*/ 0 ||
	    xx > COLNO/*-1*/ || tx > COLNO/*-1*/ ||
	    yy > ROWNO/*-1*/ || ty > ROWNO/*-1*/) {
#ifdef DEBUG
		debugpline("dig_corridor: bad coords : (%d,%d) (%d,%d).",
			   xx,yy,tx,ty);
#endif
		return FALSE;
	}
	if (tx > xx)		dx = 1;
	else if (ty > yy)	dy = 1;
	else if (tx < xx)	dx = -1;
	else			dy = -1;

	xx -= dx;
	yy -= dy;
	cct = 0;
	while(xx != tx || yy != ty) {
	    /* loop: dig corridor at [xx,yy] and find new [xx,yy] */
	    if(cct++ > 500 || (nxcor && !rn2(35)))
		return FALSE;

	    xx += dx;
	    yy += dy;

	    if(xx >= COLNO/*-1*/ || xx </*=*/ 0 || yy </*=*/ 0 || yy >= ROWNO/*-1*/)
		return FALSE;		/* impossible */

	    crm = &levl[xx][yy];
	    if(crm->typ == btyp) {
		if(ftyp != CORR || rn2(100)) {
			crm->typ = ftyp;
			crm->fleecycolor = randomcolouur;

			if(!rn2(ishaxor ? 38 : 75))
				(void) mksobj_at(BOULDER, xx, yy, TRUE, FALSE, FALSE);
			else if(!rn2(ishaxor ? 20 : 40) && timebasedlowerchance())
				(void) mkobj_at(0, xx, yy, TRUE, FALSE);
			else if(moves == 1 && !rn2(60)) /* some earlygame help... --Amy */
				(void) mkobj_at(0, xx, yy, TRUE, FALSE);
			else if(!rn2(ishaxor ? 50 : 100)){ 
			    char buf[BUFSZ];
				const char *mesg = random_engraving(buf);
			    make_engr_at(xx, yy, mesg, 0L, (xchar)0);
			}
		    else if(!rn2(ishaxor ? 150 : 300) && !(depth(&u.uz) == 1 && In_dod(&u.uz) && rn2(3)) && !(depth(&u.uz) == 2 && In_dod(&u.uz) && rn2(2)) ) 
				(void) maketrap(xx, yy, rndtrap(), 100, TRUE);
		    else if(!rn2(ishaxor ? 100 : 200)) {
				if (!ishomicider) (void) makemon((struct permonst *)0, xx, yy, MM_MAYSLEEP);
				else makerandomtrap_at(xx, yy, TRUE);
				}
		    else if(!rn2(ishaxor ? 200 : 400)) 
				(void) mkfeature(xx, yy);
		} else {
			crm->typ = SCORR;
		}
	    } else
	    if(crm->typ != ftyp && crm->typ != SCORR) {
		/* strange ... */
		return FALSE;
	    }

	    /* find next corridor position */
	    dix = abs(xx-tx);
	    diy = abs(yy-ty);

		/* randomization code by jonadab */
		if ((dix > diy) && diy && !rn2(dix-diy+1)) {
			dix = 0;
		} else if ((diy > dix) && dix && !rn2(diy-dix+1)) {
			diy = 0;
		}

	    /* do we have to change direction ? */
	    if(dy && dix > diy) {
		register int ddx = (xx > tx) ? -1 : 1;

		crm = &levl[xx+ddx][yy];
		if(crm->typ == btyp || crm->typ == ftyp || crm->typ == SCORR) {
		    dx = ddx;
		    dy = 0;
		    continue;
		}
	    } else if(dx && diy > dix) {
		register int ddy = (yy > ty) ? -1 : 1;

		crm = &levl[xx][yy+ddy];
		if(crm->typ == btyp || crm->typ == ftyp || crm->typ == SCORR) {
		    dy = ddy;
		    dx = 0;
		    continue;
		}
	    }

	    /* continue straight on? */
	    crm = &levl[xx+dx][yy+dy];
	    if(crm->typ == btyp || crm->typ == ftyp || crm->typ == SCORR)
		continue;

	    /* no, what must we do now?? */
	    if(dx) {
		dx = 0;
		dy = (ty < yy) ? -1 : 1;
	    } else {
		dy = 0;
		dx = (tx < xx) ? -1 : 1;
	    }
	    crm = &levl[xx+dx][yy+dy];
	    if(crm->typ == btyp || crm->typ == ftyp || crm->typ == SCORR)
		continue;
	    dy = -dy;
	    dx = -dx;
	}
	return TRUE;
}

/*
 * Disgusting hack: since special levels have their rooms filled before
 * sorting the rooms, we have to re-arrange the speed values upstairs_room
 * and dnstairs_room after the rooms have been sorted.  On normal levels,
 * stairs don't get created until _after_ sorting takes place.
 */
STATIC_OVL void
fix_stair_rooms()
{
    int i;
    struct mkroom *croom;

    if(xdnstair &&
       !((dnstairs_room->lx <= xdnstair && xdnstair <= dnstairs_room->hx) &&
	 (dnstairs_room->ly <= ydnstair && ydnstair <= dnstairs_room->hy))) {
	for(i=0; i < nroom; i++) {
	    croom = &rooms[i];
	    if((croom->lx <= xdnstair && xdnstair <= croom->hx) &&
	       (croom->ly <= ydnstair && ydnstair <= croom->hy)) {
		dnstairs_room = croom;
		break;
	    }
	}
	if(i == nroom)
	    panic("Couldn't find dnstair room in fix_stair_rooms!");
    }
    if(xupstair &&
       !((upstairs_room->lx <= xupstair && xupstair <= upstairs_room->hx) &&
	 (upstairs_room->ly <= yupstair && yupstair <= upstairs_room->hy))) {
	for(i=0; i < nroom; i++) {
	    croom = &rooms[i];
	    if((croom->lx <= xupstair && xupstair <= croom->hx) &&
	       (croom->ly <= yupstair && yupstair <= croom->hy)) {
		upstairs_room = croom;
		break;
	    }
	}
	if(i == nroom)
	    panic("Couldn't find upstair room in fix_stair_rooms!");
    }
}

/*
 * Corridors always start from a door. But it can end anywhere...
 * Basically we search for door coordinates or for endpoints coordinates
 * (from a distance).
 */

STATIC_OVL void
create_corridor(c)
corridor	*c;
{
	coord org, dest;

	boolean specialcorridor = 0;
	if (!rn2(iswarper ? 50 : 500)) specialcorridor = 1;

	if (c->src.room == -1) {
		sort_rooms();
		fix_stair_rooms();
		makecorridors();
		return;
	}

	if( !search_door(&rooms[c->src.room], &org.x, &org.y, c->src.wall,
			 c->src.door))
	    return;

	if (c->dest.room != -1) {
		if(!search_door(&rooms[c->dest.room], &dest.x, &dest.y,
				c->dest.wall, c->dest.door))
		    return;
		switch(c->src.wall) {
		      case W_NORTH: org.y--; break;
		      case W_SOUTH: org.y++; break;
		      case W_WEST:  org.x--; break;
		      case W_EAST:  org.x++; break;
		}
		switch(c->dest.wall) {
		      case W_NORTH: dest.y--; break;
		      case W_SOUTH: dest.y++; break;
		      case W_WEST:  dest.x--; break;
		      case W_EAST:  dest.x++; break;
		}
		if (!specialcorridor) {
			(void) dig_corridor(&org, &dest, FALSE, CORR, STONE);
		} else {
			(void) dig_corridor(&org, &dest, FALSE, walkableterrain(), STONE);
		}
	}
}


/*
 * Fill a room (shop, zoo, etc...) with appropriate stuff.
 */

void
fill_room(croom, prefilled)
struct mkroom *croom;
boolean prefilled;
{

	register int sx,sy,i,eelct=0;
	int rtrap;
	int randomnes = 0;
	int typ, typ2;
	struct monst *mtmp;
	coord mm;

	register int tryct = 0;
	register struct obj *otmp;

	if (croom && croom->rtype == OROOM && at_dgn_entrance("Green Cross") && !(level.flags.has_greencrossroom)) croom->rtype = GREENCROSSROOM;

	if (croom && croom->rtype == OROOM && !rn2( ((isironman || RngeIronmanMode || In_netherrealm(&u.uz)) && (depth(&u.uz) > 1 && !(u.preversionmode && In_greencross(&u.uz) && (dunlev(&u.uz) == 1)) && !(iszapem && In_spacebase(&u.uz) && (dunlev(&u.uz) == 1))) ) ? 1 : ((isironman || RngeIronmanMode || In_netherrealm(&u.uz)) && depth(&u.uz) < 2) ? 10 : Role_if(PM_CAMPERSTRIKER) ? 50 : 5000) ) {

retryrandtype:
		switch (rnd(109)) {

			case 1: croom->rtype = COURT; break;
			case 2: croom->rtype = SWAMP; break;
			case 3: croom->rtype = BEEHIVE; break;
			case 4: croom->rtype = MORGUE; break;
			case 5: croom->rtype = BARRACKS; break;
			case 6: croom->rtype = ZOO; break;
			case 7: croom->rtype = REALZOO; break;
			case 8: croom->rtype = GIANTCOURT; break;
			case 9: croom->rtype = LEPREHALL; break;
			case 10: croom->rtype = DRAGONLAIR; break;
			case 11: croom->rtype = BADFOODSHOP; break;
			case 12: croom->rtype = COCKNEST; break;
			case 13: croom->rtype = ANTHOLE; break;
			case 14: croom->rtype = LEMUREPIT; break;
			case 15: croom->rtype = MIGOHIVE; break;
			case 16: croom->rtype = FUNGUSFARM; break;
			case 17: croom->rtype = CLINIC; break;
			case 18: croom->rtype = TERRORHALL; break;
			case 19: croom->rtype = ELEMHALL; break;
			case 20: croom->rtype = ANGELHALL; break;
			case 21: croom->rtype = MIMICHALL; break;
			case 22: croom->rtype = NYMPHHALL; break;
			case 23: croom->rtype = SPIDERHALL; break;
			case 24: croom->rtype = TROLLHALL; break;
			case 25: croom->rtype = HUMANHALL; break;
			case 26: croom->rtype = GOLEMHALL; break;
			case 27: croom->rtype = COINHALL; break;
			case 28: croom->rtype = DOUGROOM; break;
			case 29: croom->rtype = ARMORY; break;
			case 30: croom->rtype = TENSHALL; break;
			case 31: croom->rtype = TRAPROOM; break;
			case 32: croom->rtype = POOLROOM; break;
			case 33: croom->rtype = STATUEROOM; break;
			case 34: croom->rtype = INSIDEROOM; break;
			case 35: croom->rtype = RIVERROOM; break;
			case 36: croom->rtype = TEMPLE; break;
			case 37: croom->rtype = EMPTYNEST; break;
			case 38: croom->rtype = INSIDEROOM; break;
			case 39: croom->rtype = INSIDEROOM; break;
			case 40: croom->rtype = INSIDEROOM; break;
			case 41: croom->rtype = INSIDEROOM; break;
			case 42: croom->rtype = INSIDEROOM; break;
			case 43: croom->rtype = INSIDEROOM; break;
			case 44: croom->rtype = INSIDEROOM; break;
			case 45: croom->rtype = INSIDEROOM; break;
			case 46: croom->rtype = INSIDEROOM; break;
			case 47: croom->rtype = INSIDEROOM; break;
			case 48: croom->rtype = RIVERROOM; break;
			case 49: croom->rtype = RIVERROOM; break;
			case 50: croom->rtype = RIVERROOM; break;
			case 51: croom->rtype = RIVERROOM; break;
			case 52: croom->rtype = RIVERROOM; break;
			case 53: croom->rtype = RIVERROOM; break;
			case 54: croom->rtype = RIVERROOM; break;
			case 55: croom->rtype = RIVERROOM; break;
			case 56: croom->rtype = RIVERROOM; break;
			case 57: croom->rtype = RIVERROOM; break;
			case 58: croom->rtype = GRUEROOM; break;
			case 59: croom->rtype = CRYPTROOM; break;
			case 60: croom->rtype = TROUBLEZONE; break;
			case 61: croom->rtype = WEAPONCHAMBER; break;
			case 62: croom->rtype = HELLPIT; break;
			case 63: croom->rtype = FEMINISMROOM; break;
			case 64: croom->rtype = MEADOWROOM; break;
			case 65: croom->rtype = COOLINGCHAMBER; break;
			case 66: croom->rtype = VOIDROOM; break;
			case 67: croom->rtype = HAMLETROOM; break;
			case 68: croom->rtype = KOPSTATION; break;
			case 69: croom->rtype = BOSSROOM; break;
			case 70: croom->rtype = RNGCENTER; break;
			case 71: croom->rtype = WIZARDSDORM; break;
			case 72: croom->rtype = DOOMEDBARRACKS; break;
			case 73: croom->rtype = SLEEPINGROOM; break;
			case 74: croom->rtype = DIVERPARADISE; break;
			case 75: croom->rtype = MENAGERIE; break;
			case 76:
				if (!rn2(20)) {
					croom->rtype = NASTYCENTRAL; break;
				}
				else goto retryrandtype;
			case 77: croom->rtype = EMPTYDESERT; break;
			case 78: croom->rtype = RARITYROOM; break;
			case 79: croom->rtype = EXHIBITROOM; break;
			case 80: croom->rtype = PRISONCHAMBER; break;
			case 81: croom->rtype = NUCLEARCHAMBER; break;
			case 82: croom->rtype = LEVELSEVENTYROOM; break;
			case 83: croom->rtype = VARIANTROOM; break;
			case 84: croom->rtype = EVILROOM; break;
			case 85: croom->rtype = RELIGIONCENTER; break;
			case 86: croom->rtype = CHAOSROOM; break;
			case 87: croom->rtype = CURSEDMUMMYROOM; break;
			case 88: croom->rtype = MIXEDPOOL; break;
			case 89: croom->rtype = ARDUOUSMOUNTAIN; break;
			case 90: croom->rtype = LEVELFFROOM; break;
			case 91: croom->rtype = VERMINROOM; break;
			case 92: croom->rtype = MIRASPA; break;
			case 93: croom->rtype = MACHINEROOM; break;
			case 94: croom->rtype = SHOWERROOM; break;
			case 95: croom->rtype = GREENCROSSROOM; break;
			case 96: croom->rtype = CENTRALTEDIUM; break;
			case 97: croom->rtype = RUINEDCHURCH; break;
			case 98: croom->rtype = RAMPAGEROOM; break;
			case 99: croom->rtype = GAMECORNER; break;
			case 100: croom->rtype = ILLUSIONROOM; break;
			case 101: croom->rtype = ROBBERCAVE; break;
			case 102: croom->rtype = SANITATIONCENTRAL; break;
			case 103: croom->rtype = PLAYERCENTRAL; break;
			case 104: croom->rtype = CASINOROOM; break;
			case 105: croom->rtype = FULLROOM; break;
			case 106: croom->rtype = LETTERSALADROOM; break;
			case 107: croom->rtype = THE_AREA_ROOM; break;
			case 108: croom->rtype = CHANGINGROOM; break;
			case 109: croom->rtype = QUESTORROOM; break;

		}

	}

	if (!croom || croom->rtype == OROOM)
	    return;

	if (croom->rtype == RANDOMSHOP) {

		switch (rnd(100)) {	/* random shop - probabilities should match shknam.c --Amy */

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
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
			case 23:
				croom->rtype = SHOPBASE; break;
			case 24:
			case 25:
			case 26:
			case 27:
			case 28:
			case 29:
			case 30:
			case 31:
			case 32:
			case 33:
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
			case 40:
				croom->rtype = ARMORSHOP; break;
			case 41:
			case 42:
			case 43:
			case 44:
				croom->rtype = SCROLLSHOP; break;
			case 45:
			case 46:
			case 47:
			case 48:
				croom->rtype = POTIONSHOP; break;
			case 49:
			case 50:
			case 51:
			case 52:
			case 53:
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 64:
			case 65:
				croom->rtype = WEAPONSHOP; break;
			case 66:
			case 67:
			case 68:
			case 69:
				croom->rtype = FOODSHOP; break;
			case 70:
			case 71:
			case 72:
				croom->rtype = RINGSHOP; break;
			case 73:
			case 74:
			case 75:
			case 76:
				croom->rtype = WANDSHOP; break;
			case 77:
			case 78:
			case 79:
				croom->rtype = TOOLSHOP; break;
			case 80:
			case 81:
			case 82:
			case 83:
				croom->rtype = PETSHOP; break;
			case 84:
			case 85:
			case 86:
			case 87:
				croom->rtype = TINSHOP; break;
			case 88:
			case 89:
			case 90:
			case 91:
				croom->rtype = BOOKSHOP; break;
			case 92:
				croom->rtype = USELESSSHOP; break;
			case 93:
				croom->rtype = GUNSHOP; break;
			case 94:
				croom->rtype = BANGSHOP; break;
			case 95:
				croom->rtype = AMMOSHOP; break;
			case 96:
				croom->rtype = LITENSHOP; break;
			case 97:
				croom->rtype = BOULDSHOP; break;
			case 98:
				croom->rtype = ROCKSSHOP; break;
			case 99:
				croom->rtype = WIERDSHOP; break;
			case 100:
				croom->rtype = ZORKSHOP; break;

		}
	}

	if (croom->rtype == RANDOMROOM) {

		switch (rnd(89)) {

			case 1: croom->rtype = COURT; break;
			case 2: croom->rtype = SWAMP; break;
			case 3: croom->rtype = BEEHIVE; break;
			case 4: croom->rtype = MORGUE; break;
			case 5: croom->rtype = BARRACKS; break;
			case 6: croom->rtype = ZOO; break;
			case 7: croom->rtype = REALZOO; break;
			case 8: croom->rtype = GIANTCOURT; break;
			case 9: croom->rtype = LEPREHALL; break;
			case 10: croom->rtype = DRAGONLAIR; break;
			case 11: croom->rtype = BADFOODSHOP; break;
			case 12: croom->rtype = COCKNEST; break;
			case 13: croom->rtype = ANTHOLE; break;
			case 14: croom->rtype = LEMUREPIT; break;
			case 15: croom->rtype = MIGOHIVE; break;
			case 16: croom->rtype = FUNGUSFARM; break;
			case 17: croom->rtype = CLINIC; break;
			case 18: croom->rtype = TERRORHALL; break;
			case 19: croom->rtype = ELEMHALL; break;
			case 20: croom->rtype = ANGELHALL; break;
			case 21: croom->rtype = MIMICHALL; break;
			case 22: croom->rtype = NYMPHHALL; break;
			case 23: croom->rtype = SPIDERHALL; break;
			case 24: croom->rtype = TROLLHALL; break;
			case 25: croom->rtype = HUMANHALL; break;
			case 26: croom->rtype = GOLEMHALL; break;
			case 27: croom->rtype = COINHALL; break;
			case 28: croom->rtype = DOUGROOM; break;
			case 29: croom->rtype = ARMORY; break;
			case 30: croom->rtype = TENSHALL; break;
			case 31: croom->rtype = TRAPROOM; break;
			case 32: croom->rtype = POOLROOM; break;
			case 33: croom->rtype = STATUEROOM; break;
			case 34: croom->rtype = INSIDEROOM; break;
			case 35: croom->rtype = RIVERROOM; break;
			case 36: croom->rtype = TEMPLE; break;
			case 37: croom->rtype = EMPTYNEST; break;
			case 38: croom->rtype = GRUEROOM; break;
			case 39: croom->rtype = CRYPTROOM; break;
			case 40: croom->rtype = TROUBLEZONE; break;
			case 41: croom->rtype = WEAPONCHAMBER; break;
			case 42: croom->rtype = HELLPIT; break;
			case 43: croom->rtype = FEMINISMROOM; break;
			case 44: croom->rtype = MEADOWROOM; break;
			case 45: croom->rtype = COOLINGCHAMBER; break;
			case 46: croom->rtype = VOIDROOM; break;
			case 47: croom->rtype = HAMLETROOM; break;
			case 48: croom->rtype = KOPSTATION; break;
			case 49: croom->rtype = BOSSROOM; break;
			case 50: croom->rtype = RNGCENTER; break;
			case 51: croom->rtype = WIZARDSDORM; break;
			case 52: croom->rtype = DOOMEDBARRACKS; break;
			case 53: croom->rtype = SLEEPINGROOM; break;
			case 54: croom->rtype = DIVERPARADISE; break;
			case 55: croom->rtype = MENAGERIE; break;
			case 56: croom->rtype = rn2(20) ? TROUBLEZONE : NASTYCENTRAL; break;
			case 57: croom->rtype = EMPTYDESERT; break;
			case 58: croom->rtype = RARITYROOM; break;
			case 59: croom->rtype = EXHIBITROOM; break;
			case 60: croom->rtype = PRISONCHAMBER; break;
			case 61: croom->rtype = NUCLEARCHAMBER; break;
			case 62: croom->rtype = LEVELSEVENTYROOM; break;
			case 63: croom->rtype = VARIANTROOM; break;
			case 64: croom->rtype = EVILROOM; break;
			case 65: croom->rtype = RELIGIONCENTER; break;
			case 66: croom->rtype = CHAOSROOM; break;
			case 67: croom->rtype = CURSEDMUMMYROOM; break;
			case 68: croom->rtype = MIXEDPOOL; break;
			case 69: croom->rtype = ARDUOUSMOUNTAIN; break;
			case 70: croom->rtype = LEVELFFROOM; break;
			case 71: croom->rtype = VERMINROOM; break;
			case 72: croom->rtype = MIRASPA; break;
			case 73: croom->rtype = MACHINEROOM; break;
			case 74: croom->rtype = SHOWERROOM; break;
			case 75: croom->rtype = GREENCROSSROOM; break;
			case 76: croom->rtype = CENTRALTEDIUM; break;
			case 77: croom->rtype = RUINEDCHURCH; break;
			case 78: croom->rtype = RAMPAGEROOM; break;
			case 79: croom->rtype = GAMECORNER; break;
			case 80: croom->rtype = ILLUSIONROOM; break;
			case 81: croom->rtype = ROBBERCAVE; break;
			case 82: croom->rtype = SANITATIONCENTRAL; break;
			case 83: croom->rtype = PLAYERCENTRAL; break;
			case 84: croom->rtype = CASINOROOM; break;
			case 85: croom->rtype = FULLROOM; break;
			case 86: croom->rtype = LETTERSALADROOM; break;
			case 87: croom->rtype = THE_AREA_ROOM; break;
			case 88: croom->rtype = CHANGINGROOM; break;
			case 89: croom->rtype = QUESTORROOM; break;

		}

	}

	if (!prefilled) {
	    int x,y;

	    /* Shop ? */
	    if (croom->rtype >= SHOPBASE) {
		    stock_room(croom->rtype - SHOPBASE, croom);
		    level.flags.has_shop = TRUE;
		    return;
	    }

	    switch (croom->rtype) {
		case VAULT:
		    for (x=croom->lx;x<=croom->hx;x++)
			for (y=croom->ly;y<=croom->hy;y++)
			    (void) mkgold((long)rn1(abs(depth(&u.uz))*20, 501), x, y);
			if (!rn2(5)) { /* sporkhack code */
				/* it's an aquarium!  :) */
				level.flags.vault_is_aquarium = TRUE;
				create_feature(0,0,croom,POOL);
				create_feature(1,1,croom,POOL);
				if (!rn2(3)) {
					(void)makemon(mkclass(S_EEL,0),croom->lx,croom->ly, MM_MAYSLEEP);
				}
				if (!rn2(3)) {
					(void)makemon(mkclass(S_EEL,0),croom->hx,croom->hy, MM_MAYSLEEP);
				}
			}
			else if (!rn2(100)) { /* evil patch idea by jonadab - cursed vaults filled with L */

				level.flags.vault_is_cursed = TRUE;

				u.aggravation = 1; /* make them high-level for good measure --Amy */
				u.heavyaggravation = 1; /* make them really high-level */
				DifficultyIncreased += 1; /* make them REALLY high-level (ugly hack) */
				HighlevelStatus += 1; /* and make the high-level ones more likely to spawn (ugly hack #2) */
				reset_rndmonst(NON_PM);

				mtmp = makemon(mkclass(S_LICH,0),croom->lx,croom->ly, MM_MAYSLEEP);
				if (mtmp) mtmp->msleeping = 1;
				mtmp = makemon(mkclass(S_LICH,0),croom->hx,croom->hy, MM_MAYSLEEP);
				if (mtmp) mtmp->msleeping = 1;
				if (rn2(2)) {mtmp = makemon(mkclass(S_LICH,0),croom->hx,croom->ly, MM_MAYSLEEP);
				if (mtmp) mtmp->msleeping = 1;
				}
				if (rn2(2)) {mtmp = makemon(mkclass(S_LICH,0),croom->lx,croom->hy, MM_MAYSLEEP);
				if (mtmp) mtmp->msleeping = 1;
				}

				u.aggravation = 0;
				u.heavyaggravation = 0;
				if (DifficultyIncreased > 0) DifficultyIncreased -= 1;
				if (HighlevelStatus > 0) HighlevelStatus -= 1;

			}

		    break;
		case COURT:
		case ZOO:
		case REALZOO:
		case GIANTCOURT:
		case BEEHIVE:
		case BADFOODSHOP:
		case LEMUREPIT:
		case MIGOHIVE:
		case FUNGUSFARM:
		case MORGUE:
		case DRAGONLAIR:
		case ANTHOLE:
		case COCKNEST:
		case EMPTYNEST:
		case LEPREHALL:
		case BARRACKS:
		case HUMANHALL:
		case MIMICHALL:
		case CLINIC:
		case TERRORHALL:
		case ELEMHALL:
		case ANGELHALL:
		case NYMPHHALL:
		case SPIDERHALL:
		case TROLLHALL:
		case GOLEMHALL:
		case COINHALL:
		case ARMORY:
		case TENSHALL:
		/*case TRAPROOM:*/
		/*case POOLROOM:*/
		case GRUEROOM:
		/*case STATUEROOM:*/
		case DOUGROOM:

	    case CRYPTROOM:
	    case TROUBLEZONE:
	    case WEAPONCHAMBER:
	    case HELLPIT:
	    case ROBBERCAVE:
	    case CASINOROOM:
	    case SANITATIONCENTRAL:
	    case FEMINISMROOM:
	    case MEADOWROOM:
	    case COOLINGCHAMBER:
	    case VOIDROOM:
	    case HAMLETROOM:
	    case KOPSTATION:
	    case BOSSROOM:
	    case RNGCENTER:
	    case WIZARDSDORM:
	    case DOOMEDBARRACKS:
	    case SLEEPINGROOM:
	    case DIVERPARADISE:
	    case MENAGERIE:
	    case RARITYROOM:
	    case EXHIBITROOM:
	    case PRISONCHAMBER:
	    case NUCLEARCHAMBER:
	    case LEVELSEVENTYROOM:
	    case PLAYERCENTRAL:
	    case VARIANTROOM:

	case EVILROOM:
	case RELIGIONCENTER:
	case CURSEDMUMMYROOM:
	case ARDUOUSMOUNTAIN:
	case LETTERSALADROOM:
	case THE_AREA_ROOM:
	case CHANGINGROOM:
	case QUESTORROOM:
	case LEVELFFROOM:
	case VERMINROOM:
	case MIRASPA:
	case FULLROOM:
	case MACHINEROOM:
	case GREENCROSSROOM:
	case RUINEDCHURCH:
	case GAMECORNER:
	case ILLUSIONROOM:

		    fill_zoo(croom);
		    break;
	    }
	}

	if (croom->rtype == SWAMP) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) ) {
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

	}

	if (croom->rtype == TRAPROOM) {

		rtrap = randomtrap();

		if (!rn2(4)) randomnes = 1;

			if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
			if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

			for(sx = croom->lx; sx <= croom->hx; sx++)
			for(sy = croom->ly; sy <= croom->hy; sy++)
			if((levl[sx][sy].typ > DBWALL) && !t_at(sx,sy) ) {
			    if(rn2(5)) 
					(void) maketrap(sx, sy, rtrap, 100, TRUE);
				if (randomnes == 1) rtrap = randomtrap();
			}

	}

	if (croom->rtype == POOLROOM) {

	    typ = !rn2(6) ? POOL : !rn2(5) ? MOAT : !rn2(8) ? ICE : !rn2(14) ? GRASSLAND : !rn2(25) ? SNOW : !rn2(30) ? ASH : !rn2(35) ? SAND : !rn2(35) ? PAVEDFLOOR : !rn2(50) ? HIGHWAY : !rn2(45) ? GRAVEWALL : !rn2(20) ? CLOUD : !rn2(32) ? AIR : !rn2(40) ? ROCKWALL : !rn2(40) ? TREE : !rn2(50) ? FARMLAND : !rn2(75) ? NETHERMIST : !rn2(100) ? STALACTITE : !rn2(100) ? MOUNTAIN : !rn2(60) ? IRONBARS : !rn2(70) ? CRYSTALWATER : !rn2(100) ? TUNNELWALL : !rn2(150) ? WATERTUNNEL : !rn2(100) ? MOORLAND : !rn2(150) ? URINELAKE : !rn2(200) ? SHIFTINGSAND : !rn2(50) ? STYXRIVER : !rn2(400) ? WOODENTABLE : !rn2(800) ? CARVEDBED : !rn2(300) ? STRAWMATTRESS : !rn2(800) ? WELL : !rn2(800) ? POISONEDWELL : !rn2(800) ? WAGON : !rn2(800) ? BURNINGWAGON : !rn2(480) ? FOUNTAIN : !rn2(60) ? CRYPTFLOOR : !rn2(100) ? BUBBLES : !rn2(80) ? RAINCLOUD : !rn2(1000) ? PENTAGRAM : !rn2(1000) ? THRONE : !rn2(240) ? SINK : !rn2(160) ? TOILET : !rn2(80) ? GRAVE : !rn2(1000) ? ALTAR : LAVAPOOL;

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) ) {
		    if(rn2(5)) {
			levl[sx][sy].typ = typ;
			if (typ == FOUNTAIN) 	level.flags.nfountains++;
			if (typ == SINK) 	level.flags.nsinks++;

			}

		}

	}

	if (croom->rtype == CHAOSROOM) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) ) {
		    if(rn2(10)) {
			levl[sx][sy].typ = randomwalltype();

			}

		}

	}

	if (croom->rtype == RAMPAGEROOM) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++) {
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) ) {
		    if(!rn2(10)) {
			levl[sx][sy].typ = ROCKWALL;

			}

		}

		if (!rn2(3)) (void) mksobj_at(BOULDER, sx, sy, TRUE, FALSE, FALSE);
		if (!rn2(3)) (void) maketrap(sx, sy, rndtrap(), 100, TRUE);

		} /* for loop */

	}

	if (croom->rtype == MIXEDPOOL) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++) {
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) ) {
		    if(rn2(4)) {
			levl[sx][sy].typ = rn2(2) ? MOAT : CRYSTALWATER;

			}

		}

		} /* for loop */

		if (somexy(croom, &mm)) {
			  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
		}

	}

	if (croom->rtype == SHOWERROOM) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++) {
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) ) {
		    if(rn2(3)) {
			levl[sx][sy].typ = rn2(2) ? BUBBLES : RAINCLOUD;

			}

		}

		} /* for loop */

	}

	if (croom->rtype == CENTRALTEDIUM) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++) {
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

		} /* for loop */

	}

	if (croom->rtype == BOSSROOM) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++) {

			if(!rn2(5) && !t_at(sx, sy))
				(void) maketrap(sx, sy, BOSS_SPAWNER, 100, FALSE);
		}

	}

	if (croom->rtype == NASTYCENTRAL) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++) {
			(void) mksobj_at(rnd_class(RIGHT_MOUSE_BUTTON_STONE, NASTY_STONE), sx, sy, TRUE, FALSE, FALSE);
		}

	}

	if (croom->rtype == EMPTYDESERT) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++) {
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && rn2(10)) {
				levl[sx][sy].typ = rn2(5) ? SAND : SHIFTINGSAND;
			}
		}

	}

	if (croom->rtype == INSIDEROOM) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		  if (!rn2(10)) {
			  if (somexy(croom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);

			while (!rn2(2)) {
			  if (somexy(croom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
			}
		  }

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) ) {

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

			/*else*/ if (!rn2(Role_if(PM_CAMPERSTRIKER) ? 5 : 10))	(void) maketrap(sx, sy, typ2, 100, TRUE);

			if (!rn2(1000)) 	(void) mksobj_at(SWITCHER, sx, sy, TRUE, FALSE, FALSE);
			if (!rn2(Role_if(PM_CAMPERSTRIKER) ? 25 : 100)) 	(void) mksobj_at(UGH_MEMORY_TO_CREATE_INVENTORY, sx, sy, TRUE, FALSE, FALSE);

			if (!rn2(Role_if(PM_CAMPERSTRIKER) ? 20 : 40)) 	(void) makemon(insidemon(), sx, sy, MM_ADJACENTOK|MM_ANGRY);
		}

	}

	if (croom->rtype == RIVERROOM) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		  if (!rn2(30)) {
			  if (somexy(croom, &mm))
				  (void) mksobj_at(TREASURE_CHEST, mm.x, mm.y, TRUE, FALSE, FALSE);
		  }

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) ) {

	    typ = !rn2(5) ? POOL : !rn2(6) ? MOAT : !rn2(20) ? ICE : !rn2(20) ? GRASSLAND : !rn2(80) ? HIGHWAY : !rn2(20) ? FOUNTAIN : !rn2(30) ? FARMLAND : !rn2(35) ? MOUNTAIN : !rn2(50) ? CRYSTALWATER : !rn2(100) ? TUNNELWALL : !rn2(150) ? WATERTUNNEL : !rn2(6) ? ROCKWALL : !rn2(16) ? TREE : ROOM;

		levl[sx][sy].typ = typ;
		if (typ == FOUNTAIN) 	level.flags.nfountains++;
		if (typ == SINK) 	level.flags.nsinks++;
		if(Role_if(PM_CAMPERSTRIKER) && !rn2(50)) (void) maketrap(sx, sy, randomtrap(), 100, TRUE);
		}

	}

	if (croom->rtype == STATUEROOM) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) ) {
		    if(rn2(2)) 
				(void) maketrap(sx, sy, (rn2(10) ? STATUE_TRAP : ANIMATION_TRAP), 100, TRUE);
		}

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++)
		    if(rn2(2)) 
			{
			    struct obj *sobj = mksobj_at(STATUE, sx, sy, TRUE, FALSE, FALSE);

			    if (sobj && !rn2(3) ) {
				for (i = rn2(2 + rn2(4)); i; i--)
					if (timebasedlowerchance()) {
					    (void) add_to_container(sobj, mkobj(RANDOM_CLASS, FALSE, FALSE), TRUE);
					}
				sobj->owt = weight(sobj);
			    }
			    if (sobj) sobj->owt = weight(sobj);
			}

	}

	switch (croom->rtype) {
	    case VAULT:
		level.flags.has_vault = TRUE;
		break;
	    case ZOO:
	    case DOUGROOM:
		level.flags.has_zoo = TRUE;
		break;
	    case COURT:
		level.flags.has_court = TRUE;
		break;
	    case MORGUE:
		level.flags.has_morgue = TRUE;
		break;
	    case BEEHIVE:
		level.flags.has_beehive = TRUE;
		break;
	    case LEMUREPIT:
		level.flags.has_lemurepit = TRUE;
		break;
	    case MIGOHIVE:
		level.flags.has_migohive = TRUE;
		break;
	    case FUNGUSFARM:
		level.flags.has_fungusfarm = TRUE;
		break;
	    case CLINIC:
		level.flags.has_clinic = TRUE;
		break;
	    case MIMICHALL:
		level.flags.has_mimichall = TRUE;
		break;
	    case ANGELHALL:
		level.flags.has_angelhall = TRUE;
		break;
	    case ELEMHALL:
		level.flags.has_elemhall = TRUE;
		break;
	    case NYMPHHALL:
		level.flags.has_nymphhall = TRUE;
		break;
	    case GOLEMHALL:
		level.flags.has_golemhall = TRUE;
		break;
	    case GRUEROOM:
		level.flags.has_grueroom = TRUE;
		break;
	    case CRYPTROOM:
		level.flags.has_cryptroom = TRUE;
		break;
	    case TROUBLEZONE:
		level.flags.has_troublezone = TRUE;
		break;
	    case WEAPONCHAMBER:
		level.flags.has_weaponchamber = TRUE;
		break;
	    case HELLPIT:
		level.flags.has_hellpit = TRUE;
		break;
	    case ROBBERCAVE:
		level.flags.has_robbercave = TRUE;
		break;
	    case CASINOROOM:
		level.flags.has_casinoroom = TRUE;
		break;
	    case SANITATIONCENTRAL:
		level.flags.has_sanitationcentral = TRUE;
		break;
	    case FEMINISMROOM:
		level.flags.has_feminismroom = TRUE;
		break;
	    case MEADOWROOM:
		level.flags.has_meadowroom = TRUE;
		break;
	    case COOLINGCHAMBER:
		level.flags.has_coolingchamber = TRUE;
		break;
	    case VOIDROOM:
		level.flags.has_voidroom = TRUE;
		break;
	    case ARMORY:
		level.flags.has_armory = TRUE;
		break;
	    case HAMLETROOM:
		level.flags.has_hamletroom = TRUE;
		break;
	    case KOPSTATION:
		level.flags.has_kopstation = TRUE;
		break;
	    case BOSSROOM:
		level.flags.has_bossroom = TRUE;
		break;
	    case RNGCENTER:
		level.flags.has_rngcenter = TRUE;
		break;
	    case WIZARDSDORM:
		level.flags.has_wizardsdorm = TRUE;
		break;
	    case DOOMEDBARRACKS:
		level.flags.has_doomedbarracks = TRUE;
		break;
	    case SLEEPINGROOM:
		level.flags.has_sleepingroom = TRUE;
		break;
	    case DIVERPARADISE:
		level.flags.has_diverparadise = TRUE;
		break;
	    case MENAGERIE:
		level.flags.has_menagerie = TRUE;
		break;
	    case NASTYCENTRAL:
		level.flags.has_nastycentral = TRUE;
		break;
	    case EMPTYDESERT:
		level.flags.has_emptydesert = TRUE;
		break;
	    case RARITYROOM:
		level.flags.has_rarityroom = TRUE;
		break;
	    case EXHIBITROOM:
		level.flags.has_exhibitroom = TRUE;
		break;
	    case PRISONCHAMBER:
		level.flags.has_prisonchamber = TRUE;
		break;
	    case NUCLEARCHAMBER:
		level.flags.has_nuclearchamber = TRUE;
		break;
	    case LEVELSEVENTYROOM:
		level.flags.has_levelseventyroom = TRUE;
		break;
	    case PLAYERCENTRAL:
		level.flags.has_playercentral = TRUE;
		break;
	    case VARIANTROOM:
		level.flags.has_variantroom = TRUE;
		break;
	    case POOLROOM:
		level.flags.has_poolroom = TRUE;
		break;
	    case STATUEROOM:
		level.flags.has_statueroom = TRUE;
		break;
	    case INSIDEROOM:
		level.flags.has_insideroom = TRUE;
		break;
	    case RIVERROOM:
		level.flags.has_riverroom = TRUE;
		break;
	    case TRAPROOM:
		level.flags.has_traproom = TRUE;
		break;
	    case COINHALL:
		level.flags.has_coinhall = TRUE;
		break;
	    case SPIDERHALL:
		level.flags.has_spiderhall = TRUE;
		break;
	    case TROLLHALL:
		level.flags.has_trollhall = TRUE;
		break;
	    case HUMANHALL:
		level.flags.has_humanhall = TRUE;
		break;
	    case TERRORHALL:
		level.flags.has_terrorhall = TRUE;
		break;
	    case TENSHALL:
		level.flags.has_tenshall = TRUE;
		break;
	    case BARRACKS:
		level.flags.has_barracks = TRUE;
		break;
	    case TEMPLE:
		level.flags.has_temple = TRUE;
		break;
	    case SWAMP:
		level.flags.has_swamp = TRUE;
		break;
	    case EVILROOM:
		level.flags.has_evilroom = TRUE;
		break;
	    case RELIGIONCENTER:
		level.flags.has_religioncenter = TRUE;
		break;
	    case CURSEDMUMMYROOM:
		level.flags.has_cursedmummyroom = TRUE;
		break;
	    case ARDUOUSMOUNTAIN:
		level.flags.has_arduousmountain = TRUE;
		break;
	    case QUESTORROOM:
		level.flags.has_questorroom = TRUE;
		break;
	    case CHANGINGROOM:
		level.flags.has_changingroom = TRUE;
		break;
	    case LEVELFFROOM:
		level.flags.has_levelffroom = TRUE;
		break;
	    case VERMINROOM:
		level.flags.has_verminroom = TRUE;
		break;
	    case MIRASPA:
		level.flags.has_miraspa = TRUE;
		break;
	    case MACHINEROOM:
		level.flags.has_machineroom = TRUE;
		break;
	    case SHOWERROOM:
		level.flags.has_showerroom = TRUE;
		break;
	    case GREENCROSSROOM:
		level.flags.has_greencrossroom = TRUE;
		break;
	    case RUINEDCHURCH:
		level.flags.has_ruinedchurch = TRUE;
		break;
	    case GAMECORNER:
		level.flags.has_gamecorner = TRUE;
		break;
	    case ILLUSIONROOM:
		level.flags.has_illusionroom = TRUE;
		break;
	}
}

STATIC_OVL void
free_rooms(ro, n)
room **ro;
int n;
{
	short j;
	room *r;

	while(n--) {
		r = ro[n];
		free(r->name);
		free(r->parent);
		if ((j = r->ndoor) != 0) {
			while(j--)
			    free(r->doors[j]);
			free(r->doors);
		}
		if ((j = r->nstair) != 0) {
			while(j--)
			    free(r->stairs[j]);
			free(r->stairs);
		}
		if ((j = r->naltar) != 0) {
			while (j--)
			    free(r->altars[j]);
			free(r->altars);
		}
		if ((j = r->nfountain) != 0) {
			while(j--)
			    free(r->fountains[j]);
			free(r->fountains);
		}
		if ((j = r->nsink) != 0) {
			while(j--)
			    free(r->sinks[j]);
			free(r->sinks);
		}
		if ((j = r->npool) != 0) {
			while(j--)
			    free(r->pools[j]);
			free(r->pools);
		}
		if ((j = r->ntrap) != 0) {
			while (j--)
			    free(r->traps[j]);
			free(r->traps);
		}
		if ((j = r->nmonster) != 0) {
			while (j--)
				free(r->monsters[j]);
			free(r->monsters);
		}
		if ((j = r->nobject) != 0) {
			while (j--)
				free(r->objects[j]);
			free(r->objects);
		}
		if ((j = r->ngold) != 0) {
			while(j--)
			    free(r->golds[j]);
			free(r->golds);
		}
		if ((j = r->nengraving) != 0) {
			while (j--)
				free(r->engravings[j]);
			free(r->engravings);
		}
		free(r);
	}
	free(ro);
}

STATIC_OVL void
build_room(r, pr)
room *r, *pr;
{
	boolean okroom;
	struct mkroom	*aroom;
	short i;
	xchar rtype = (!r->chance || rn2(100) < r->chance) ? r->rtype : OROOM;

	if(pr) {
		aroom = &subrooms[nsubroom];
		okroom = create_subroom(pr->mkr, r->x, r->y, r->w, r->h,
					rtype, r->rlit);
	} else {
		aroom = &rooms[nroom];
		okroom = create_room(r->x, r->y, r->w, r->h, r->xalign,
				     r->yalign, rtype, r->rlit, FALSE, FALSE);
		r->mkr = aroom;
	}

	if (okroom) {
		/* Create subrooms if necessary... */
		for(i=0; i < r->nsubroom; i++)
		    build_room(r->subrooms[i], r);
		/* And now we can fill the room! */

		/* Priority to the stairs */

		for(i=0; i <r->nstair; i++)
		    create_stairs(r->stairs[i], aroom);

		/* Then to the various elements (sinks, etc..) */
		for(i = 0; i<r->nsink; i++)
		    create_feature(r->sinks[i]->x, r->sinks[i]->y, aroom, SINK);
/*              for(i = 0; i<r->ntoilet; i++)
		    create_feature(r->toilets[i]->x, r->toilets[i]->y, aroom, TOILET);*/
		for(i = 0; i<r->npool; i++)
		    create_feature(r->pools[i]->x, r->pools[i]->y, aroom, POOL);
		for(i = 0; i<r->nfountain; i++)
		    create_feature(r->fountains[i]->x, r->fountains[i]->y,
				   aroom, FOUNTAIN);
		for(i = 0; i<r->naltar; i++)
		    create_altar(r->altars[i], aroom);
		for(i = 0; i<r->ndoor; i++)
		    create_door(r->doors[i], aroom);

		/* The traps */
		for(i = 0; i<r->ntrap; i++)
		    create_trap(r->traps[i], aroom);

		/* The monsters */
		for(i = 0; i<r->nmonster; i++)
		    create_monster(r->monsters[i], aroom);

		/* The objects */
		for(i = 0; i<r->nobject; i++)
		    create_object(r->objects[i], aroom);

		/* The gold piles */
		for(i = 0; i<r->ngold; i++)
		    create_gold(r->golds[i], aroom);

		/* The engravings */
		for (i = 0; i < r->nengraving; i++)
		    create_engraving(r->engravings[i], aroom);

#ifdef SPECIALIZATION
		topologize(aroom,FALSE);		/* set roomno */
#else
		topologize(aroom);			/* set roomno */
#endif
		/* MRS - 07/04/91 - This is temporary but should result
		 * in proper filling of shops, etc.
		 * DLC - this can fail if corridors are added to this room
		 * at a later point.  Currently no good way to fix this.
		 */
		if( (aroom->rtype != OROOM || (at_dgn_entrance("Green Cross") && !(level.flags.has_greencrossroom)) || !rn2( ((isironman || RngeIronmanMode || In_netherrealm(&u.uz)) && (depth(&u.uz) > 1 && !(u.preversionmode && In_greencross(&u.uz) && (dunlev(&u.uz) == 1)) && !(iszapem && In_spacebase(&u.uz) && (dunlev(&u.uz) == 1))) ) ? 1 : ((isironman || RngeIronmanMode || In_netherrealm(&u.uz)) && depth(&u.uz) < 2) ? 10 : Role_if(PM_CAMPERSTRIKER) ? 50 : 5000) ) && r->filled) fill_room(aroom, FALSE);

		if ( (aroom->rtype == OROOM || !rn2(5)) && (!r->nsubroom || !rn2(5)) ) {
			add_amy_stuff(aroom);
		}
	}
}

/*
 * set lighting in a region that will not become a room.
 */
STATIC_OVL void
light_region(tmpregion)
    region  *tmpregion;
{
    register boolean litstate = tmpregion->rlit ? 1 : 0;
    register int hiy = tmpregion->y2;
    register int x, y;
    register struct rm *lev;
    int lowy = tmpregion->y1;
    int lowx = tmpregion->x1, hix = tmpregion->x2;

    if(litstate) {
	/* adjust region size for walls, but only if lighted */
	lowx = max(lowx-1,1);
	hix = min(hix+1,COLNO-1);
	lowy = max(lowy-1,0);
	hiy = min(hiy+1, ROWNO-1);
    }
    for(x = lowx; x <= hix; x++) {
	lev = &levl[x][lowy];
	for(y = lowy; y <= hiy; y++) {
	    if (lev->typ != LAVAPOOL) /* this overrides normal lighting */
		lev->lit = litstate;
	    lev++;
	}
    }
}

/* initialization common to all special levels */
STATIC_OVL void
load_common_data(fd, typ)
dlb *fd;
int typ;
{
	uchar	n;
	long	lev_flags;
	int	i;

      {
	aligntyp atmp;
	/* shuffle 3 alignments; can't use sp_lev_shuffle() on aligntyp's */
	i = rn2(3);   atmp=ralign[2]; ralign[2]=ralign[i]; ralign[i]=atmp;
	if (rn2(2)) { atmp=ralign[1]; ralign[1]=ralign[0]; ralign[0]=atmp; }
      }

	level.flags.is_maze_lev = typ == SP_LEV_MAZE;

	/* Read the level initialization data */
	Fread((void *) &init_lev, 1, sizeof(lev_init), fd);
	if(init_lev.init_present) {
	    if(init_lev.lit < 0)
		init_lev.lit = rn2(2);
	    mkmap(&init_lev);
	}

	/* Read the per level flags */
	Fread((void *) &lev_flags, 1, sizeof(lev_flags), fd);
	if (lev_flags & NOTELEPORT)
	    level.flags.noteleport = 1;
	if (lev_flags & HARDFLOOR)
	    level.flags.hardfloor = 1;
	if (lev_flags & NOMMAP)
	    level.flags.nommap = 1;
	if (lev_flags & SHORTSIGHTED)
	    level.flags.shortsighted = 1;
	if (lev_flags & ARBOREAL)
	    level.flags.arboreal = 1;
	if (lev_flags & SPOOKY)
	    level.flags.spooky = 1;
	if (lev_flags & LETHE)
	    level.flags.lethe = 1;

	/* Read message */
	Fread((void *) &n, 1, sizeof(n), fd);
	if (n) {
	    lev_message = (char *) alloc(n + 1);
	    Fread((void *) lev_message, 1, (int) n, fd);
	    lev_message[n] = 0;
	}
}

STATIC_OVL void
load_one_monster(fd, m)
dlb *fd;
monster *m;
{
	int size;

	Fread((void *) m, 1, sizeof *m, fd);
	if ((size = m->name.len) != 0) {
	    m->name.str = (char *) alloc((unsigned)size + 1);
	    Fread((void *) m->name.str, 1, size, fd);
	    m->name.str[size] = '\0';
	} else
	    m->name.str = (char *) 0;
	if ((size = m->appear_as.len) != 0) {
	    m->appear_as.str = (char *) alloc((unsigned)size + 1);
	    Fread((void *) m->appear_as.str, 1, size, fd);
	    m->appear_as.str[size] = '\0';
	} else
	    m->appear_as.str = (char *) 0;
}

STATIC_OVL void
load_one_object(fd, o)
dlb *fd;
object *o;
{
	int size;

	Fread((void *) o, 1, sizeof *o, fd);
	if ((size = o->name.len) != 0) {
	    o->name.str = (char *) alloc((unsigned)size + 1);
	    Fread((void *) o->name.str, 1, size, fd);
	    o->name.str[size] = '\0';
	} else
	    o->name.str = (char *) 0;
}

STATIC_OVL void
load_one_engraving(fd, e)
dlb *fd;
engraving *e;
{
	int size;

	Fread((void *) e, 1, sizeof *e, fd);
	size = e->engr.len;
	e->engr.str = (char *) alloc((unsigned)size+1);
	Fread((void *) e->engr.str, 1, size, fd);
	e->engr.str[size] = '\0';
}

/*
 * Select a random coordinate in the maze.
 *
 * We want a place not 'touched' by the loader.  That is, a place in
 * the maze outside every part of the special level.
 */

STATIC_OVL boolean
load_rooms(fd)
dlb *fd;
{
	xchar		nrooms, ncorr;
	char		n;
	short		size;
	corridor	tmpcor;
	room**		tmproom;
	int		i, j;
	coord mm;

	load_common_data(fd, SP_LEV_ROOMS);

	Fread((void *) &n, 1, sizeof(n), fd); /* nrobjects */
	if (n) {
		Fread((void *)robjects, sizeof(*robjects), n, fd);
		sp_lev_shuffle(robjects, (char *)0, (int)n);
	}

	Fread((void *) &n, 1, sizeof(n), fd); /* nrmonst */
	if (n) {
		Fread((void *)rmonst, sizeof(*rmonst), n, fd);
		sp_lev_shuffle(rmonst, (char *)0, (int)n);
	}

	Fread((void *) &nrooms, 1, sizeof(nrooms), fd);
						/* Number of rooms to read */
	tmproom = NewTab(room,nrooms);
	for (i=0;i<nrooms;i++) {
		room *r;

		r = tmproom[i] = New(room);

		/* Let's see if this room has a name */
		Fread((void *) &size, 1, sizeof(size), fd);
		if (size > 0) {	/* Yup, it does! */
			r->name = (char *) alloc((unsigned)size + 1);
			Fread((void *) r->name, 1, size, fd);
			r->name[size] = 0;
		} else
		    r->name = (char *) 0;

		/* Let's see if this room has a parent */
		Fread((void *) &size, 1, sizeof(size), fd);
		if (size > 0) {	/* Yup, it does! */
			r->parent = (char *) alloc((unsigned)size + 1);
			Fread((void *) r->parent, 1, size, fd);
			r->parent[size] = 0;
		} else
		    r->parent = (char *) 0;

		Fread((void *) &r->x, 1, sizeof(r->x), fd);
					/* x pos on the grid (1-5) */
		Fread((void *) &r->y, 1, sizeof(r->y), fd);
					 /* y pos on the grid (1-5) */
		Fread((void *) &r->w, 1, sizeof(r->w), fd);
					 /* width of the room */
		Fread((void *) &r->h, 1, sizeof(r->h), fd);
					 /* height of the room */
		Fread((void *) &r->xalign, 1, sizeof(r->xalign), fd);
					 /* horizontal alignment */
		Fread((void *) &r->yalign, 1, sizeof(r->yalign), fd);
					 /* vertical alignment */
		Fread((void *) &r->rtype, 1, sizeof(r->rtype), fd);
					 /* type of room (zoo, shop, etc.) */
		Fread((void *) &r->chance, 1, sizeof(r->chance), fd);
					 /* chance of room being special. */
		Fread((void *) &r->rlit, 1, sizeof(r->rlit), fd);
					 /* lit or not ? */
		Fread((void *) &r->filled, 1, sizeof(r->filled), fd);
					 /* to be filled? */
		r->nsubroom= 0;

		/* read the doors */
		Fread((void *) &r->ndoor, 1, sizeof(r->ndoor), fd);
		if ((n = r->ndoor) != 0)
		    r->doors = NewTab(room_door, n);
		while(n--) {
			r->doors[(int)n] = New(room_door);
			Fread((void *) r->doors[(int)n], 1,
				sizeof(room_door), fd);
		}

		/* read the stairs */
		Fread((void *) &r->nstair, 1, sizeof(r->nstair), fd);
		if ((n = r->nstair) != 0)
		    r->stairs = NewTab(stair, n);
		while (n--) {
			r->stairs[(int)n] = New(stair);
			Fread((void *) r->stairs[(int)n], 1,
				sizeof(stair), fd);
		}

		/* read the altars */
		Fread((void *) &r->naltar, 1, sizeof(r->naltar), fd);
		if ((n = r->naltar) != 0)
		    r->altars = NewTab(altar, n);
		while (n--) {
			r->altars[(int)n] = New(altar);
			Fread((void *) r->altars[(int)n], 1,
				sizeof(altar), fd);
		}

		/* read the fountains */
		Fread((void *) &r->nfountain, 1,
			sizeof(r->nfountain), fd);
		if ((n = r->nfountain) != 0)
		    r->fountains = NewTab(fountain, n);
		while (n--) {
			r->fountains[(int)n] = New(fountain);
			Fread((void *) r->fountains[(int)n], 1,
				sizeof(fountain), fd);
		}

		/* read the sinks */
		Fread((void *) &r->nsink, 1, sizeof(r->nsink), fd);
		if ((n = r->nsink) != 0)
		    r->sinks = NewTab(sink, n);
		while (n--) {
			r->sinks[(int)n] = New(sink);
			Fread((void *) r->sinks[(int)n], 1, sizeof(sink), fd);
		}

		/* read the pools */
		Fread((void *) &r->npool, 1, sizeof(r->npool), fd);
		if ((n = r->npool) != 0)
		    r->pools = NewTab(pool,n);
		while (n--) {
			r->pools[(int)n] = New(pool);
			Fread((void *) r->pools[(int)n], 1, sizeof(pool), fd);
		}

		/* read the traps */
		Fread((void *) &r->ntrap, 1, sizeof(r->ntrap), fd);
		if ((n = r->ntrap) != 0)
		    r->traps = NewTab(trap, n);
		while(n--) {
			r->traps[(int)n] = New(trap);
			Fread((void *) r->traps[(int)n], 1, sizeof(trap), fd);
		}

		/* read the monsters */
		Fread((void *) &r->nmonster, 1, sizeof(r->nmonster), fd);
		if ((n = r->nmonster) != 0) {
		    r->monsters = NewTab(monster, n);
		    while(n--) {
			r->monsters[(int)n] = New(monster);
			load_one_monster(fd, r->monsters[(int)n]);
		    }
		} else
		    r->monsters = 0;

		/* read the objects, in same order as mazes */
		Fread((void *) &r->nobject, 1, sizeof(r->nobject), fd);
		if ((n = r->nobject) != 0) {
		    r->objects = NewTab(object, n);
		    for (j = 0; j < n; ++j) {
			r->objects[j] = New(object);
			load_one_object(fd, r->objects[j]);
		    }
		} else
		    r->objects = 0;

		/* read the gold piles */
		Fread((void *) &r->ngold, 1, sizeof(r->ngold), fd);
		if ((n = r->ngold) != 0)
		    r->golds = NewTab(gold, n);
		while (n--) {
			r->golds[(int)n] = New(gold);
			Fread((void *) r->golds[(int)n], 1, sizeof(gold), fd);
		}

		/* read the engravings */
		Fread((void *) &r->nengraving, 1,
			sizeof(r->nengraving), fd);
		if ((n = r->nengraving) != 0) {
		    r->engravings = NewTab(engraving,n);
		    while (n--) {
			r->engravings[(int)n] = New(engraving);
			load_one_engraving(fd, r->engravings[(int)n]);
		    }
		} else
		    r->engravings = 0;

	}

	/* Now that we have loaded all the rooms, search the
	 * subrooms and create the links.
	 */

	for (i = 0; i<nrooms; i++)
	    if (tmproom[i]->parent) {
		    /* Search the parent room */
		    for(j=0; j<nrooms; j++)
			if (tmproom[j]->name && !strcmp(tmproom[j]->name,
						       tmproom[i]->parent)) {
				n = tmproom[j]->nsubroom++;
				tmproom[j]->subrooms[(int)n] = tmproom[i];
				break;
			}
	    }

	/*
	 * Create the rooms now...
	 */

	for (i=0; i < nrooms; i++)
	    if(!tmproom[i]->parent)
		build_room(tmproom[i], (room *) 0);

	free_rooms(tmproom, nrooms);

	/* read the corridors */

	Fread((void *) &ncorr, sizeof(ncorr), 1, fd);
	for (i=0; i<ncorr; i++) {
		Fread((void *) &tmpcor, 1, sizeof(tmpcor), fd);
		create_corridor(&tmpcor);
	}

	/* chance to create Ludios portal, by Amy */
	if (In_dod(&u.uz)) {
		mazexy_all(&mm);
		if (isok(mm.x, mm.y)) mk_knox_portal(mm.x, mm.y);
	}

	return TRUE;
}


STATIC_OVL void
maze1xy(m, humidity)
coord *m;
int humidity;
{
	register int x, y, tryct = 2000;
	/* tryct:  normally it won't take more than ten or so tries due
	   to the circumstances under which we'll be called, but the
	   `humidity' screening might drastically change the chances */

	do {
	    x = rn1(x_maze_max - 3, 3);
	    y = rn1(y_maze_max - 3, 3);
	    if (--tryct < 0) break;	/* give up */
	} while (Map[x][y] || !is_ok_location((schar)x, (schar)y, humidity));
	/* Amy edit: used to also check for "!(x % 2) || !(y % 2) ||"... we want all tiles to have a chance of
	 * getting stuff though, so that players can't simply avoid certain squares to never step on a trap */

	m->x = (xchar)x,  m->y = (xchar)y;
}

/*
 * The Big Thing: special maze loader
 *
 * Could be cleaner, but it works.
 */

STATIC_OVL boolean
load_maze(fd)
dlb *fd;
{
    xchar   x, y, typ;
    boolean prefilled, room_not_needed;

    char    n, numpart = 0;
    xchar   nwalk = 0, nwalk_sav;
    schar   filling;
    char    halign, valign;

    int     xi, dir, size;
    coord   mm;
    int     mapcount, mapcountmax, mapfact;

    lev_region  tmplregion;
    region  tmpregion;
    door    tmpdoor;
    trap    tmptrap;
    monster tmpmons;
    object  tmpobj;
    drawbridge tmpdb;
    walk    tmpwalk;
    digpos  tmpdig;
    lad     tmplad;
    stair   tmpstair, prevstair;
    altar   tmpaltar;
    gold    tmpgold;
    fountain tmpfountain;
    engraving tmpengraving;
    xchar   mustfill[(MAXNROFROOMS+1)*2];
    struct trap *badtrap;
    boolean has_bounds;

    int specialcorridor;
    if (!rn2(iswarper ? 50 : 500)) specialcorridor = rnd(2);

    (void) memset((void *)&Map[0][0], 0, sizeof Map);
    load_common_data(fd, SP_LEV_MAZE);

    /* Initialize map */
    Fread((void *) &filling, 1, sizeof(filling), fd);
    if (!init_lev.init_present) { /* don't init if mkmap() has been called */
      for(x = 2; x <= x_maze_max; x++)
	for(y = 0; y <= y_maze_max; y++)
	    if (filling == -1) {
#ifndef WALLIFIED_MAZE
		    levl[x][y].typ = STONE;
#else
		    levl[x][y].typ =
			(y < 2 || ((x % 2) && (y % 2))) ? STONE : HWALL;
#endif
	    } else {
		    levl[x][y].typ = filling;
	    }
    }

    /* Start reading the file */
    Fread((void *) &numpart, 1, sizeof(numpart), fd);
						/* Number of parts */
    if (!numpart || numpart > 9)
	panic("load_maze error: numpart = %d", (int) numpart);

    while (numpart--) {
	Fread((void *) &halign, 1, sizeof(halign), fd);
					/* Horizontal alignment */
	Fread((void *) &valign, 1, sizeof(valign), fd);
					/* Vertical alignment */
	Fread((void *) &xsize, 1, sizeof(xsize), fd);
					/* size in X */
	Fread((void *) &ysize, 1, sizeof(ysize), fd);
					/* size in Y */
	switch((int) halign) {
	    case LEFT:	    xstart = 3;					break;
	    case H_LEFT:    xstart = 2+((x_maze_max-2-xsize)/4);	break;
	    case CENTER:    xstart = 2+((x_maze_max-2-xsize)/2);	break;
	    case H_RIGHT:   xstart = 2+((x_maze_max-2-xsize)*3/4);	break;
	    case RIGHT:     xstart = x_maze_max-xsize-1;		break;
	    case RANDOMX:
			switch (rnd(5)) {
				case 1:
					xstart = 3; break;
				case 2:
					xstart = 2+((x_maze_max-2-xsize)/4); break;
				case 3:
					xstart = 2+((x_maze_max-2-xsize)/2); break;
				case 4:
					xstart = 2+((x_maze_max-2-xsize)*3/4); break;
				case 5:
					xstart = x_maze_max-xsize-1; break;
			}
			break;
	}
	switch((int) valign) {
	    case TOP:	    ystart = 3;					break;
	    case CENTER:    ystart = 2+((y_maze_max-2-ysize)/2);	break;
	    case BOTTOM:    ystart = y_maze_max-ysize-1;		break;
	    case RANDOMY:
			switch (rnd(3)) {
				case 1:
					ystart = 3; break;
				case 2:
					ystart = 2+((y_maze_max-2-ysize)/2); break;
				case 3:
					ystart = y_maze_max-ysize-1; break;
			}
			break;
	}
	if (!(xstart % 2)) xstart++;
#ifdef BIGSLEX
	if ((ystart % 2)) ystart++;
#else
	if (!(ystart % 2)) ystart++;
#endif
	if ((ystart < 0) || (ystart + ysize > ROWNO)) {
	    /* try to move the start a bit */
	    ystart += (ystart > 0) ? -2 : 2;
	    if(ysize == ROWNO) ystart = 0;
	    if(ystart < 0 || ystart + ysize > ROWNO)
		panic("reading special level with ysize too large");
	}

	/*
	 * If any CROSSWALLs are found, must change to ROOM after REGION's
	 * are laid out.  CROSSWALLS are used to specify "invisible"
	 * boundaries where DOOR syms look bad or aren't desirable.
	 */
	has_bounds = FALSE;

	if(init_lev.init_present && xsize <= 1 && ysize <= 1) {
	    xstart = 1;
	    ystart = 0;
	    xsize = COLNO-1;
	    ysize = ROWNO;
	} else {
	    /* Load the map */
	    for(y = ystart; y < ystart+ysize; y++)
		for(x = xstart; x < xstart+xsize; x++) {
		    levl[x][y].typ = Fgetc(fd);
		    levl[x][y].lit = FALSE;
		    /* clear out levl: load_common_data may set them */
		    levl[x][y].flags = 0;
		    levl[x][y].horizontal = 0;
		    levl[x][y].roomno = 0;
		    levl[x][y].edge = 0;
		    /*
		     * Note: Even though levl[x][y].typ is type schar,
		     *	 lev_comp.y saves it as type char. Since schar != char
		     *	 all the time we must make this exception or hack
		     *	 through lev_comp.y to fix.
		     */

		    /*
		     *  Set secret doors to closed (why not trapped too?).  Set
		     *  the horizontal bit.
		     */
		    if (levl[x][y].typ == SDOOR || IS_DOOR(levl[x][y].typ)) {
			if(levl[x][y].typ == SDOOR)
			    levl[x][y].doormask = D_CLOSED;
			/*
			 *  If there is a wall to the left that connects to a
			 *  (secret) door, then it is horizontal.  This does
			 *  not allow (secret) doors to be corners of rooms.
			 */
			if (x != xstart && (IS_WALL(levl[x-1][y].typ) ||
					    levl[x-1][y].horizontal))
			    levl[x][y].horizontal = 1;
		    } else if(levl[x][y].typ == HWALL ||
				levl[x][y].typ == IRONBARS)
			levl[x][y].horizontal = 1;
		    else if(levl[x][y].typ == LAVAPOOL)
			levl[x][y].lit = 1;
		    else if(levl[x][y].typ == CROSSWALL)
			has_bounds = TRUE;
		    Map[x][y] = 1;
		}
	    if (init_lev.init_present && init_lev.joined)
		remove_rooms(xstart, ystart, xstart+xsize, ystart+ysize);
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of level regions */
	if(n) {
	    if(num_lregions) {
		/* realloc the lregion space to add the new ones */
		/* don't really free it up until the whole level is done */
		lev_region *newl = (lev_region *) alloc(sizeof(lev_region) *
						(unsigned)(n+num_lregions));
		(void) memcpy((void *)(newl+n), (void *)lregions,
					sizeof(lev_region) * num_lregions);
		free(lregions);
		num_lregions += n;
		lregions = newl;
	    } else {
		num_lregions = n;
		lregions = (lev_region *)
				alloc(sizeof(lev_region) * (unsigned)n);
	    }
	}

	while(n--) {
	    boolean found = TRUE;
	    Fread((void *) &tmplregion, sizeof(tmplregion), 1, fd);
	    if ((size = tmplregion.rname.len) != 0) {
		tmplregion.rname.str = (char *) alloc((unsigned)size + 1);
		Fread((void *) tmplregion.rname.str, size, 1, fd);
		tmplregion.rname.str[size] = '\0';
	    } else
		tmplregion.rname.str = (char *) 0;
	    if(!tmplregion.in_islev) {
		found &= get_location(&tmplregion.inarea.x1,
				      &tmplregion.inarea.y1, DRY|WET);
		found &= get_location(&tmplregion.inarea.x2,
				      &tmplregion.inarea.y2, DRY|WET);
	    }
	    if(!tmplregion.del_islev) {
		found &= get_location(&tmplregion.delarea.x1,
				      &tmplregion.delarea.y1, DRY|WET);
		found &= get_location(&tmplregion.delarea.x2,
				      &tmplregion.delarea.y2, DRY|WET);
	    }
	    if (!found)
		panic("reading special level with region located nowhere");
	    lregions[(int)n] = tmplregion;
	}

	/* random level region registers */
	Fread((void *) &n, 1, sizeof(n), fd);	
	if (n) {
	    int tmpn = n;
	    while(n--) {
		boolean found = TRUE;
		Fread((void *) &tmplregion, sizeof(tmplregion), 1, fd);
		if ((size = tmplregion.rname.len) != 0) {
		    tmplregion.rname.str = (char *) alloc((unsigned)size + 1);
		    Fread((void *) tmplregion.rname.str, size, 1, fd);
		    tmplregion.rname.str[size] = '\0';
		} else
		    tmplregion.rname.str = (char *) 0;
		if (!found)
		    panic("reading special level with random region located nowhere");
		(void) memcpy((void *)&rarea[(int)tmpn - n - 1],
			(void *)&tmplregion, sizeof(lev_region));
	    }
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Random objects */
	if(n) {
		Fread((void *)robjects, sizeof(*robjects), (int) n, fd);
		sp_lev_shuffle(robjects, (char *)0, (int)n);
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Random locations */
	if(n) {
		char nloc[10];
		Fread((void *)nloc, sizeof(*nloc), (int) n, fd);
		for(xi = 0; xi < n; xi++) {
		    Fread((void *)rloc_x[xi], sizeof(*rloc_x[xi]),
			    (int) nloc[xi], fd);
		    Fread((void *)rloc_y[xi], sizeof(*rloc_y[xi]),
			    (int) nloc[xi], fd);
		    sp_lev_shuffle(rloc_x[xi], rloc_y[xi], (int)nloc[xi]);
		}
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Random monsters */
	if(n) {
		Fread((void *)rmonst, sizeof(*rmonst), (int) n, fd);
		sp_lev_shuffle(rmonst, (char *)0, (int)n);
	}

	(void) memset((void *)mustfill, 0, sizeof(mustfill));
	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of subrooms */
	while(n--) {
		register struct mkroom *troom;

		Fread((void *)&tmpregion, 1, sizeof(tmpregion), fd);

		if(tmpregion.rtype > MAXRTYPE) {
		    tmpregion.rtype -= MAXRTYPE+1;
		    prefilled = TRUE;
		} else
		    prefilled = FALSE;

		if(tmpregion.rlit < 0)
		    tmpregion.rlit = (rnd(1+abs(depth(&u.uz))) < 11 && rn2(77))
			? TRUE : FALSE;

		if (!get_location(&tmpregion.x1, &tmpregion.y1, DRY|WET) ||
		  !get_location(&tmpregion.x2, &tmpregion.y2, DRY|WET))
		    panic("reading special level with region located nowhere");

		/* for an ordinary room, `prefilled' is a flag to force
		   an actual room to be created (such rooms are used to
		   control placement of migrating monster arrivals) */
		room_not_needed = (tmpregion.rtype == OROOM &&
				   !tmpregion.rirreg && !prefilled);
		if (room_not_needed || nroom >= MAXNROFROOMS) {
		    if (!room_not_needed)
			impossible("Too many rooms on new level!");
		    light_region(&tmpregion);
		    continue;
		}

		troom = &rooms[nroom];

		/* mark rooms that must be filled, but do it later */
		if (tmpregion.rtype != OROOM || (at_dgn_entrance("Green Cross") && !(level.flags.has_greencrossroom)) || !rn2( ((isironman || RngeIronmanMode || In_netherrealm(&u.uz)) && (depth(&u.uz) > 1 && !(u.preversionmode && In_greencross(&u.uz) && (dunlev(&u.uz) == 1)) && !(iszapem && In_spacebase(&u.uz) && (dunlev(&u.uz) == 1))) ) ? 1 : ((isironman || RngeIronmanMode || In_netherrealm(&u.uz)) && depth(&u.uz) < 2) ? 10 : Role_if(PM_CAMPERSTRIKER) ? 50 : 5000) )
		    mustfill[nroom] = (prefilled ? 2 : 1);

		if(tmpregion.rirreg) {
		    min_rx = max_rx = tmpregion.x1;
		    min_ry = max_ry = tmpregion.y1;
		    flood_fill_rm(tmpregion.x1, tmpregion.y1,
				  nroom+ROOMOFFSET, tmpregion.rlit, TRUE);
		    add_room(min_rx, min_ry, max_rx, max_ry,
			     FALSE, tmpregion.rtype, TRUE, FALSE, FALSE);
		    troom->rlit = tmpregion.rlit;
		    troom->irregular = TRUE;
		} else {
		    add_room(tmpregion.x1, tmpregion.y1,
			     tmpregion.x2, tmpregion.y2,
			     tmpregion.rlit, tmpregion.rtype, TRUE, FALSE, FALSE);
#ifdef SPECIALIZATION
		    topologize(troom,FALSE);		/* set roomno */
#else
		    topologize(troom);			/* set roomno */
#endif
		}
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of doors */
	while(n--) {
		struct mkroom *croom = &rooms[0];

		Fread((void *)&tmpdoor, 1, sizeof(tmpdoor), fd);

		x = tmpdoor.x;	y = tmpdoor.y;
		typ = tmpdoor.mask == -1 ? rnddoor() : tmpdoor.mask;

		if (get_location(&x, &y, DRY)) {
		    if(levl[x][y].typ != SDOOR)
			levl[x][y].typ = DOOR;
		    else {
			if(typ < D_CLOSED)
			    typ = D_CLOSED; /* force it to be closed */
		    }
		    levl[x][y].doormask = typ;

		    /* Fix random door alignment */

		    if (y > 0 && (IS_WALL(levl[x][y-1].typ) ||
				levl[x][y-1].horizontal))
			levl[x][y].horizontal = 0;
		}

		/* Now the complicated part, list it with each subroom */
		/* The dog move and mail daemon routines use this */
		xi = -1;
		while(croom->hx >= 0 && doorindex < DOORMAX) {
		    if(croom->hx >= x-1 && croom->lx <= x+1 &&
		       croom->hy >= y-1 && croom->ly <= y+1) {
			/* Found it */
			xi = add_door(x, y, croom);
			doors[xi].arti_key = tmpdoor.arti_key;
		    }
		    croom++;
		}
		if (xi < 0) {	/* Not in any room */
		    if (doorindex >= DOORMAX)
			impossible("Too many doors?");
		    else {
			xi = add_door(x, y, (struct mkroom *)0);
			doors[xi].arti_key = tmpdoor.arti_key;
		    }
		}
	}

	/* now that we have rooms _and_ associated doors, fill the rooms */
	for(n = 0; n < SIZE(mustfill); n++)
	    if(mustfill[(int)n])
		fill_room(&rooms[(int)n], (mustfill[(int)n] == 2));

	/* if special boundary syms (CROSSWALL) in map, remove them now */
	if(has_bounds) {
	    for(x = xstart; x < xstart+xsize; x++)
		for(y = ystart; y < ystart+ysize; y++)
		    if(levl[x][y].typ == CROSSWALL)
			levl[x][y].typ = ROOM;
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of drawbridges */
	while(n--) {
		Fread((void *)&tmpdb, 1, sizeof(tmpdb), fd);

		x = tmpdb.x;  y = tmpdb.y;
		if (get_location(&x, &y, DRY|WET)) {
		if (!create_drawbridge(x, y, tmpdb.dir, tmpdb.db_open))
		    impossible("Cannot create drawbridge.");
	}
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of mazewalks */
	while(n--) {
		Fread((void *)&tmpwalk, 1, sizeof(tmpwalk), fd);

		if (get_location(&tmpwalk.x, &tmpwalk.y, DRY|WET))
		walklist[nwalk++] = tmpwalk;
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of non_diggables */
	while(n--) {
		Fread((void *)&tmpdig, 1, sizeof(tmpdig), fd);

		get_location(&tmpdig.x1, &tmpdig.y1, DRY|WET);
		get_location(&tmpdig.x2, &tmpdig.y2, DRY|WET);

		set_wall_property(tmpdig.x1, tmpdig.y1,
				  tmpdig.x2, tmpdig.y2, W_NONDIGGABLE);
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of non_passables */
	while(n--) {
		Fread((void *)&tmpdig, 1, sizeof(tmpdig), fd);

		get_location(&tmpdig.x1, &tmpdig.y1, DRY|WET);
		get_location(&tmpdig.x2, &tmpdig.y2, DRY|WET);

		set_wall_property(tmpdig.x1, tmpdig.y1,
				  tmpdig.x2, tmpdig.y2, W_NONPASSWALL);
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of ladders */
	while(n--) {
		Fread((void *)&tmplad, 1, sizeof(tmplad), fd);

		x = tmplad.x;  y = tmplad.y;
		if (get_location(&x, &y, DRY)) {
		levl[x][y].typ = LADDER;
		if (tmplad.up == 1) {
			xupladder = x;	yupladder = y;
			levl[x][y].ladder = LA_UP;
		} else {
			xdnladder = x;	ydnladder = y;
			levl[x][y].ladder = LA_DOWN;
		}
	}
	}

	prevstair.x = prevstair.y = 0;
	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of stairs */
	while(n--) {
		boolean found;
		Fread((void *)&tmpstair, 1, sizeof(tmpstair), fd);

		xi = 0;
		do {
		    x = tmpstair.x;  y = tmpstair.y;
		    found = get_location(&x, &y, DRY);
		} while(found && prevstair.x && xi++ < 100 &&
			distmin(x,y,prevstair.x,prevstair.y) <= 8);
		if (!found)
		    continue;
		if ((badtrap = t_at(x,y)) != 0) deltrap(badtrap);
		mkstairs(x, y, (char)tmpstair.up, (struct mkroom *)0);
		prevstair.x = x;
		prevstair.y = y;
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of altars */
	while(n--) {
		Fread((void *)&tmpaltar, 1, sizeof(tmpaltar), fd);

		create_altar(&tmpaltar, (struct mkroom *)0);
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of fountains */
	while (n--) {
		Fread((void *)&tmpfountain, 1, sizeof(tmpfountain), fd);

		create_feature(tmpfountain.x, tmpfountain.y,
			       (struct mkroom *)0, FOUNTAIN);
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of traps */
	while(n--) {
		Fread((void *)&tmptrap, 1, sizeof(tmptrap), fd);

		create_trap(&tmptrap, (struct mkroom *)0);
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of monsters */
	while(n--) {
		load_one_monster(fd, &tmpmons);

		create_monster(&tmpmons, (struct mkroom *)0);
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of objects */
	while(n--) {
		load_one_object(fd, &tmpobj);

		create_object(&tmpobj, (struct mkroom *)0);
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of gold piles */
	while (n--) {
		Fread((void *)&tmpgold, 1, sizeof(tmpgold), fd);

		create_gold(&tmpgold, (struct mkroom *)0);
	}

	Fread((void *) &n, 1, sizeof(n), fd);
						/* Number of engravings */
	while(n--) {
		load_one_engraving(fd, &tmpengraving);

		create_engraving(&tmpengraving, (struct mkroom *)0);
	}

    }		/* numpart loop */

    nwalk_sav = nwalk;
    while(nwalk--) {
	    x = (xchar) walklist[nwalk].x;
	    y = (xchar) walklist[nwalk].y;
	    dir = walklist[nwalk].dir;

	    /* don't use move() - it doesn't use W_NORTH, etc. */
	    switch (dir) {
		case W_NORTH: --y; break;
		case W_SOUTH: y++; break;
		case W_EAST:  x++; break;
		case W_WEST:  --x; break;
		default: panic("load_maze: bad MAZEWALK direction");
	    }

	    if(!IS_DOOR(levl[x][y].typ) && !specialcorridor) {
#ifndef WALLIFIED_MAZE
		levl[x][y].typ = CORR;
#else
		levl[x][y].typ = ROOM;
#endif
		levl[x][y].flags = 0;
	    }
	    if(!IS_DOOR(levl[x][y].typ) && specialcorridor) {

		levl[x][y].typ = walkableterrain();
		levl[x][y].flags = 0;
	    }

	    /*
	     * We must be sure that the parity of the coordinates for
	     * walkfrom() is odd.  But we must also take into account
	     * what direction was chosen.
	     */
	    if(!(x % 2)) {
		if (dir == W_EAST)
		    x++;
		else
		    x--;

		/* no need for IS_DOOR check; out of map bounds */

		if (!specialcorridor) {
#ifndef WALLIFIED_MAZE
		levl[x][y].typ = CORR;
#else
		levl[x][y].typ = ROOM;
#endif
		}
		else levl[x][y].typ = walkableterrain();

		levl[x][y].flags = 0;
	    }

	    if (!(y % 2)) {
		if (dir == W_SOUTH)
		    y++;
		else
		    y--;
	    }

	    walkfrom(x, y);
    }
    /* wallification(1, 0, COLNO-1, ROWNO-1, FALSE); */
    wallification(1, 0, COLNO-1, ROWNO-1, TRUE);

    /*
     * If there's a significant portion of maze unused by the special level,
     * we don't want it empty.
     *
     * Makes the number of traps, monsters, etc. proportional
     * to the size of the maze.
     */
    mapcountmax = mapcount = (x_maze_max - 2) * (y_maze_max - 2);

    for(x = 2; x < x_maze_max; x++)
	for(y = 0; y < y_maze_max; y++)
	    if(Map[x][y]) mapcount--;

    if (nwalk_sav && (mapcount > (int) (mapcountmax / 10))) {
	    mapfact = (int) ((mapcount * 100L) / mapcountmax);
#ifdef BIGSLEX
	    for(x = rnd((int) ((rn2(4) ? 30 : 60) * mapfact) / 100); x; x--) {
#else
	    for(x = rnd((int) ((rn2(4) ? 20 : 40) * mapfact) / 100); x; x--) {
#endif
		if (timebasedlowerchance()) {
		    maze1xy(&mm, DRY);
		    (void) mkobj_at(!rn2(5) ? GEM_CLASS : RANDOM_CLASS, mm.x, mm.y, TRUE, FALSE);
		}
	    }

	if (moves == 1 && !rn2(2)) { /* some earlygame help... --Amy */
#ifdef BIGSLEX
	    for(x = rnd((int) ((rn2(4) ? 30 : 60) * mapfact) / 100); x; x--) {
#else
	    for(x = rnd((int) ((rn2(4) ? 20 : 40) * mapfact) / 100); x; x--) {
#endif
		if (timebasedlowerchance()) {
		    maze1xy(&mm, DRY);
		    (void) mkobj_at(!rn2(5) ? GEM_CLASS : RANDOM_CLASS, mm.x, mm.y, TRUE, FALSE);
		}
	    }
	}

	    for(x = rnd((int) (12 * mapfact) / 100); x; x--) {
		    maze1xy(&mm, DRY);
		    (void) mksobj_at(BOULDER, mm.x, mm.y, TRUE, FALSE, FALSE);
	    }
#ifdef BIGSLEX
	    for (x = rn2(70); x; x--) 	{ 
#else
	    for (x = rn2(20); x; x--) 	{ 
#endif
		    maze1xy(&mm, DRY);
			    char buf[BUFSZ];
				const char *mesg = random_engraving(buf);
			    make_engr_at(mm.x, mm.y, mesg, 0L, (xchar)0);
			}
	    for (x = rn2(2); x; x--) { if (!(iszapem && !(u.zapemescape)) && !(u.preversionmode && !u.preversionescape) && (depth(&u.uz) > depth(&medusa_level))) {
		maze1xy(&mm, DRY);
		if (!ishomicider) (void) makemon(minotaurvariant(), mm.x, mm.y, MM_MAYSLEEP);
		else makerandomtrap_at(mm.x, mm.y, TRUE);
		} /* cause they would be outta depth when mazes are generated at a shallow level --Amy */
	    }
#ifdef BIGSLEX
	    for(x = rnd((int) ((rn2(2) ? 168 : rn2(4) ? 42 : 84) * mapfact) / 100); x; x--) {
#else
	    for(x = rnd((int) ((rn2(2) ? 48 : rn2(4) ? 12 : 24) * mapfact) / 100); x; x--) {
#endif
		    maze1xy(&mm, WET|DRY);
		    if (!ishomicider) (void) makemon((struct permonst *) 0, mm.x, mm.y, MM_MAYSLEEP);
		    else makerandomtrap_at(mm.x, mm.y, TRUE);
	    }
	    for(x = rn2((int) (15 * mapfact) / 100); x; x--) {
		    maze1xy(&mm, DRY);
		    (void) mkgold(0L,mm.x,mm.y);
	    }
#ifdef BIGSLEX
	    for(x = rn2((int) ((rn2(2) ? 210 : rn2(4) ? 52 : 105) * mapfact) / 100); x; x--) {
#else
	    for(x = rn2((int) ((rn2(2) ? 60 : rn2(4) ? 15 : 30) * mapfact) / 100); x; x--) {
#endif
		    int trytrap;

		    maze1xy(&mm, DRY);
		    trytrap = rndtrap();
		    if (sobj_at(BOULDER, mm.x, mm.y))
			while (trytrap == PIT || trytrap == SPIKED_PIT || trytrap == GIANT_CHASM || trytrap == SHIT_PIT || trytrap == MANA_PIT || trytrap == ANOXIC_PIT || trytrap == ACID_PIT || trytrap == SHAFT_TRAP || trytrap == CURRENT_SHAFT ||
				trytrap == TRAPDOOR || trytrap == HOLE)
			    trytrap = rndtrap();
		    (void) maketrap(mm.x, mm.y, trytrap, 100, TRUE);
	    }

		if (ishaxor) {
#ifdef BIGSLEX
	    for(x = rnd((int) ((rn2(4) ? 30 : 60) * mapfact) / 100); x; x--) {
#else
	    for(x = rnd((int) ((rn2(4) ? 20 : 40) * mapfact) / 100); x; x--) {
#endif
		if (timebasedlowerchance()) {
		    maze1xy(&mm, DRY);
		    (void) mkobj_at(!rn2(5) ? GEM_CLASS : RANDOM_CLASS, mm.x, mm.y, TRUE, FALSE);
		}
	    }
	    for(x = rnd((int) (12 * mapfact) / 100); x; x--) {
		    maze1xy(&mm, DRY);
		    (void) mksobj_at(BOULDER, mm.x, mm.y, TRUE, FALSE, FALSE);
	    }
#ifdef BIGSLEX
	    for (x = rn2(70); x; x--) 	{ 
#else
	    for (x = rn2(20); x; x--) 	{ 
#endif
		    maze1xy(&mm, DRY);
			    char buf[BUFSZ];
				const char *mesg = random_engraving(buf);
			    make_engr_at(mm.x, mm.y, mesg, 0L, (xchar)0);
			}
	    for (x = rn2(2); x; x--) { if (!(iszapem && !(u.zapemescape)) && !(u.preversionmode && !u.preversionescape) && (depth(&u.uz) > depth(&medusa_level))) {
		maze1xy(&mm, DRY);
		if (!ishomicider) (void) makemon(minotaurvariant(), mm.x, mm.y, MM_MAYSLEEP);
		else makerandomtrap_at(mm.x, mm.y, TRUE);
		} /* cause they would be outta depth when mazes are generated at a shallow level --Amy */
	    }
#ifdef BIGSLEX
	    for(x = rnd((int) ((rn2(2) ? 168 : rn2(4) ? 42 : 84) * mapfact) / 100); x; x--) {
#else
	    for(x = rnd((int) ((rn2(2) ? 48 : rn2(4) ? 12 : 24) * mapfact) / 100); x; x--) {
#endif
		    maze1xy(&mm, WET|DRY);
		    if (!ishomicider) (void) makemon((struct permonst *) 0, mm.x, mm.y, MM_MAYSLEEP);
		    else makerandomtrap_at(mm.x, mm.y, TRUE);
	    }
	    for(x = rn2((int) (15 * mapfact) / 100); x; x--) {
		    maze1xy(&mm, DRY);
		    (void) mkgold(0L,mm.x,mm.y);
	    }
#ifdef BIGSLEX
	    for(x = rn2((int) ((rn2(2) ? 210 : rn2(4) ? 52 : 105) * mapfact) / 100); x; x--) {
#else
	    for(x = rn2((int) ((rn2(2) ? 60 : rn2(4) ? 15 : 30) * mapfact) / 100); x; x--) {
#endif
		    int trytrap;

		    maze1xy(&mm, DRY);
		    trytrap = rndtrap();
		    if (sobj_at(BOULDER, mm.x, mm.y))
			while (trytrap == PIT || trytrap == SPIKED_PIT || trytrap == GIANT_CHASM || trytrap == SHIT_PIT || trytrap == MANA_PIT || trytrap == ANOXIC_PIT || trytrap == ACID_PIT || trytrap == SHAFT_TRAP || trytrap == CURRENT_SHAFT ||
				trytrap == TRAPDOOR || trytrap == HOLE)
			    trytrap = rndtrap();
		    (void) maketrap(mm.x, mm.y, trytrap, 100, TRUE);
	    }
		}

    }

	/* make rivers if possible --Amy */
	if (!rn2(50) && !In_endgame(&u.uz) ) mkrivers();
	if (!rn2(250) && !In_endgame(&u.uz) ) mkrivers();

	if (ishaxor) {
		if (!rn2(50) && !In_endgame(&u.uz) ) mkrivers();
		if (!rn2(250) && !In_endgame(&u.uz) ) mkrivers();
	}

	if (isaquarian && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrivers();
	if (RngeRivers && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrivers();

	if (Race_if(PM_HUNKAJUNK) && !rn2(25) && depth(&u.uz) > 1 && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) hunkajunkriver();
	if (Race_if(PM_HUNKAJUNK) && depth(&u.uz) > 1 && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) randhunkrivers();

	if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandrivers();
	if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandrivers();

	if (ishaxor) {
		if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandrivers();
		if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandrivers();
	}

	if (isaquarian && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandrivers();
	if (RngeRivers && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandrivers();

	if ((isroommate || !rn2(100)) && ((depth(&u.uz) > 1 && !(u.preversionmode && In_greencross(&u.uz) && (dunlev(&u.uz) == 1)) && !(iszapem && In_spacebase(&u.uz) && (dunlev(&u.uz) == 1))) || !rn2(10)) && !Is_branchlev(&u.uz) && !In_endgame(&u.uz)) {

		mkroommateroom(0);
		if (!rn2(5)) {
			mkroommateroom(0);
			while (!rn2(3)) mkroommateroom(0);

		}

	}

	/* chance to create Ludios portal, by Amy */
	if (In_dod(&u.uz)) {
		mazexy_all(&mm);
		if (isok(mm.x, mm.y)) mk_knox_portal(mm.x, mm.y);
	}

    return TRUE;
}

/*
 * General loader
 */

boolean
load_special(name)
const char *name;
{
	dlb *fd;
	boolean result = FALSE;
	char c;
	struct version_info vers_info;

	fd = dlb_fopen_area(FILE_AREA_UNSHARE, name, RDBMODE);
	if (!fd) return FALSE;

	Fread((void *) &vers_info, sizeof vers_info, 1, fd);
	if (!check_version(&vers_info, name, TRUE))
	    goto give_up;

	Fread((void *) &c, sizeof c, 1, fd); /* c Header */

	switch (c) {
		case SP_LEV_ROOMS:
		    result = load_rooms(fd);
		    break;
		case SP_LEV_MAZE:
		    result = load_maze(fd);
		    break;
		default:	/* ??? */
		    result = FALSE;
	}
 give_up:
	(void)dlb_fclose(fd);
	return result;
}




/*sp_lev.c*/

