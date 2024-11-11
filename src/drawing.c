/*	SCCS Id: @(#)drawing.c	3.4	1999/12/02	*/
/* Copyright (c) NetHack Development Team 1992.			  */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "extern.h"
#include "tcap.h"
/* Relevent header information in rm.h and objclass.h. */

#ifdef C
#undef C
#endif

#ifdef TEXTCOLOR
#define C(n) n
#else
#define C(n)
#endif

#define g_FILLER(symbol) 0

uchar oc_syms[MAXOCLASSES] = DUMMY; /* the current object  display symbols */
uchar showsyms[MAXPCHARS]  = DUMMY; /* the current feature display symbols */
uchar monsyms[MAXMCLASSES] = DUMMY; /* the current monster display symbols */
uchar warnsyms[WARNCOUNT]  = DUMMY;  /* the current warning display symbols */

/* Default object class symbols.  See objclass.h. */
const char def_oc_syms[MAXOCLASSES] = {
/* 0*/	'\0',		/* placeholder for the "random class" */
	ILLOBJ_SYM,
	WEAPON_SYM,
	ARMOR_SYM,
	RING_SYM,
/* 5*/	AMULET_SYM,
	IMPLANT_SYM,
	TOOL_SYM,
	FOOD_SYM,
	POTION_SYM,
/*10*/	SCROLL_SYM,
	SPBOOK_SYM,
	WAND_SYM,
	GOLD_SYM,
	GEM_SYM,
/*15*/	ROCK_SYM,
	BALL_SYM,
	CHAIN_SYM,
	VENOM_SYM
};

const char invisexplain[] = "remembered, unseen, creature";

/* Object descriptions.  Used in do_look(). */
const char * const objexplain[] = {	/* these match def_oc_syms, above */
/* 0*/	0,
	"strange object",
	"weapon",
	"suit or piece of armor",
	"ring",
/* 5*/	"amulet",
	"implant",
	"useful item (pick-axe, key, lamp...)",
	"piece of food",
	"potion",
/*10*/	"scroll",
	"spell book",
	"wand",
	"pile of coins",
	"gem or rock",
/*15*/	"boulder or statue",
	"iron ball",
	"iron chain",
	"splash of venom"
};

/* Object class names.  Used in object_detect(). */
const char * const oclass_names[] = {
/* 0*/	0,
	"illegal objects",
	"weapons",
	"armor",
	"rings",
/* 5*/	"amulets",
	"implants",
	"tools",
	"food",
	"potions",
/*10*/	"scrolls",
	"spell books",
	"wands",
	"coins",
	"rocks",
/*15*/	"large stones",
	"iron balls",
	"chains",
	"venoms"
};

/* Default monster class symbols.  See monsym.h. */
const char def_monsyms[MAXMCLASSES] = {
	'\0',		/* holder */
	DEF_ANT,
	DEF_BLOB,
	DEF_COCKATRICE,
	DEF_DOG,
	DEF_EYE,
	DEF_FELINE,
	DEF_GREMLIN,
	DEF_HUMANOID,
	DEF_IMP,
	DEF_JELLY,		/* 10 */
	DEF_KOBOLD,
	DEF_LEPRECHAUN,
	DEF_MIMIC,
	DEF_NYMPH,
	DEF_ORC,
	DEF_PIERCER,
	DEF_QUADRUPED,
	DEF_RODENT,
	DEF_SPIDER,
	DEF_TRAPPER,		/* 20 */
	DEF_UNICORN,
	DEF_VORTEX,
	DEF_WORM,
	DEF_XAN,
	DEF_LIGHT,
	DEF_ZRUTY,
	DEF_ANGEL,
	DEF_BAT,
	DEF_CENTAUR,
	DEF_DRAGON,		/* 30 */
	DEF_ELEMENTAL,
	DEF_FUNGUS,
	DEF_GNOME,
	DEF_GIANT,
	'\0',
	DEF_JABBERWOCK,
	DEF_KOP,
	DEF_LICH,
	DEF_MUMMY,
	DEF_NAGA,		/* 40 */
	DEF_OGRE,
	DEF_PUDDING,
	DEF_QUANTMECH,
	DEF_RUSTMONST,
	DEF_SNAKE,
	DEF_TROLL,
	DEF_UMBER,
	DEF_VAMPIRE,
	DEF_WRAITH,
	DEF_XORN,		/* 50 */
	DEF_YETI,
	DEF_ZOMBIE,
	DEF_HUMAN,
	DEF_GHOST,
	DEF_GOLEM,
	DEF_DEMON,
	DEF_EEL,
	DEF_LIZARD,
	DEF_BAD_FOOD,
	DEF_BAD_COINS,		/* 60 */
	DEF_NEMESE,
	DEF_GRUE,
	DEF_WALLMONST,
	DEF_RUBMONST,
	DEF_ARCHFIEND,
	DEF_TURRET,
	DEF_FLYFISH,
	DEF_TROVE,
	DEF_WORM_TAIL,
	DEF_MIMIC_DEF,
};

/* The explanations below are also used when the user gives a string
 * for blessed genocide, so no text should wholly contain any later
 * text.  They should also always contain obvious names (eg. cat/feline).
 */
/* KMH -- changed u and z */
/* Robin Johnson -  changed Q */
const char * const monexplain[MAXMCLASSES] = {
    0,
    "ant or other insect",	"blob",			"cockatrice",
    "dog or other canine",	"eye or sphere",	"cat or other feline",
    "gremlin",			"humanoid",		"imp or minor demon",
    "jelly",			"kobold",		"leprechaun",
    "mimic",			"nymph",		"orc",
    "piercer",			"quadruped",		"rodent",
    "arachnid or centipede",	"trapper or lurker above", "unicorn or horse",
    "vortex",		"worm", "xan or other mythical/fantastic insect",
    "light",			"Zouthern animal",
#if 0
    "light",			"zruty",
#endif
    "angelic being",		"bat or bird",		"centaur",
    "dragon",			"elemental",		"fungus or mold",
    "gnome",			"giant humanoid",	0,
    "jabberwock",		"Keystone Kop",		"lich",
    "mummy",			"naga",			"ogre",
    "pudding or ooze",  "quantum mechanic or other scientist",
    "rust monster or disenchanter",
    "snake",			"troll",		"umber hulk",
    "vampire",			"wraith",		"xorn",
    "apelike creature",		"zombie",

    "human or elf",		"ghost",		"golem",
    "major demon",		"sea monster",		"lizard",
    "piece of food",	"pile of coins", "nemese",		"grue",		"wall monster",		"rub monster",		"archfiend",	"turret",		"flying fish", 	"trove",
    "long worm tail",		"mimic"
};

const struct symdef def_warnsyms[WARNCOUNT] = {
	{'1', "unknown creature causing you worry", C(CLR_WHITE)},  	/* white warning  */
	{'2', "unknown creature causing you concern", C(CLR_BROWN)},	/* pink warning   */
	{'3', "unknown creature causing you anxiety", C(CLR_RED)},	/* red warning    */
	{'4', "unknown creature causing you disquiet", C(CLR_MAGENTA)},	/* ruby warning   */
	{'5', "unknown creature causing you alarm",
						C(CLR_BRIGHT_MAGENTA)},        /* purple warning */
	{'6', "unknown creature causing you dread",
						C(CLR_ORANGE)}	/* black warning  */
};

/*
 *  Default screen symbols with explanations and colors.
 *  Note:  {ibm|dec|mac}_graphics[] arrays also depend on this symbol order.
 */
const struct symdef defsyms[MAXPCHARS] = {
/* 0*/	{' ', "unexplored area",C(NO_COLOR)},	/* stone */
	{'|', "wall",		C(CLR_GRAY)},	/* vwall */
	{'-', "wall",		C(CLR_GRAY)},	/* hwall */
	{'-', "wall",		C(CLR_GRAY)},	/* tlcorn */
	{'-', "wall",		C(CLR_GRAY)},	/* trcorn */
	{'-', "wall",		C(CLR_GRAY)},	/* blcorn */
	{'-', "wall",		C(CLR_GRAY)},	/* brcorn */
	{'-', "wall",		C(CLR_GRAY)},	/* crwall */
	{'-', "wall",		C(CLR_GRAY)},	/* tuwall */
	{'-', "wall",		C(CLR_GRAY)},	/* tdwall */
/*10*/	{'|', "wall",		C(CLR_GRAY)},	/* tlwall */
	{'|', "wall",		C(CLR_GRAY)},	/* trwall */
	{'#', "solid rock",		C(CLR_BROWN)},	/* rockwall - displays as "solid rock" */
	{'#', "grave wall",		C(CLR_YELLOW)},	/* gravewall */
	{'#', "tunnel",		C(CLR_RED)},	/* tunnelwall */
	{'.', "doorway",	C(CLR_GRAY)},	/* ndoor */
	{'-', "open door",	C(CLR_BROWN)},	/* vodoor */
	{'|', "open door",	C(CLR_BROWN)},	/* hodoor */
	{'+', "closed door",	C(CLR_BROWN)},	/* vcdoor */
	{'+', "closed door",	C(CLR_BROWN)},	/* hcdoor */
/*20*/	{'#', "iron bars",	C(HI_METAL)},	/* bars */
	{'#', "tree",		C(CLR_GREEN)},	/* tree */
	{'#', "farmland",		C(CLR_BRIGHT_GREEN)},	/* farmland */
	{'#', "mountain",		C(CLR_WHITE)},	/* mountain */
	{'.', "floor of a room",C(CLR_GRAY)},	/* room */
	{'.', "dark part of a room",C(CLR_BLACK)},	/* dark room */
	{',', "corridor",	C(CLR_BLACK)},	/* dark corr */
	{',', "lit corridor",	C(CLR_GRAY)},	/* lit corr (see mapglyph.c) */
	{'<', "staircase up",	C(CLR_WHITE)},	/* upstair */
	{'>', "staircase down",	C(CLR_WHITE)},	/* dnstair */
/*30*/	{'<', "ladder up",	C(CLR_BROWN)},	/* upladder */
	{'>', "ladder down",	C(CLR_BROWN)},	/* dnladder */
	{'_', "altar",		C(CLR_GRAY)},	/* altar */
	{'|', "grave",      C(CLR_GRAY)},   /* grave */
	{'\\', "opulent throne",C(HI_GOLD)},	/* throne */
	{'{', "sink",		C(CLR_ORANGE)},	/* sink */
	{'{', "toilet",         C(CLR_YELLOW)},  /* toilet */
	{'{', "fountain",       C(CLR_BRIGHT_BLUE)},   /* fountain */
	{'}', "water",		C(CLR_BRIGHT_BLUE)},	/* pool */
	{'1', "ice",		C(CLR_CYAN)},	/* ice */
/*40*/	{'}', "molten lava",	C(CLR_RED)},	/* lava */
	{'1', "lowered drawbridge",C(CLR_MAGENTA)},	/* vodbridge */
	{'1', "lowered drawbridge",C(CLR_MAGENTA)},	/* hodbridge */
	{'#', "raised drawbridge",C(CLR_ORANGE)},/* vcdbridge */
	{'#', "raised drawbridge",C(CLR_ORANGE)},/* hcdbridge */
	{'`', "air",		C(CLR_BLACK)},	/* open air */
	{'`', "cloud",		C(CLR_CYAN)},	/* [part of] a cloud */
	{'#', "solid rock",		C(CLR_BROWN)},	/* dungwall */
	{'}', "water",		C(CLR_BRIGHT_BLUE)},	/* under water */
	{'#', "water tunnel",		C(CLR_BRIGHT_BLUE)},	/* watertunnel */
/*50*/	{'}', "crystal water",		C(CLR_BRIGHT_CYAN)},	/* crystalwater */
	{'}', "moorland",		C(CLR_GREEN)},	/* moorland */
	{'}', "urine lake",		C(CLR_YELLOW)},	/* urinelake */
	{'}', "shifting sand",		C(CLR_BROWN)},	/* shiftingsand */
	{'}', "styx river",		C(CLR_BRIGHT_GREEN)},	/* styxriver */
	{'{', "well",		C(CLR_GREEN)},	/* well */
	{'{', "poisoned well",		C(CLR_BRIGHT_GREEN)},	/* poisonedwell */
	{'\\', "wagon",		C(CLR_RED)},	/* wagon */
	{'\\', "burning wagon",		C(CLR_ORANGE)},	/* burningwagon */
	{'|', "wooden table",		C(CLR_BROWN)},	/* woodentable */
/*60*/	{'|', "rude straw mattress",		C(CLR_YELLOW)},	/* strawmattress */
	{'1', "snow",		C(CLR_BRIGHT_CYAN)},	/* snow */
	{'`', "ash",		C(CLR_RED)},	/* ash */
	{'`', "sand",		C(CLR_BROWN)},	/* sand */
	{'`', "paved floor",		C(CLR_BRIGHT_GREEN)},	/* pavedfloor */
	{'`', "highway",		C(CLR_YELLOW)},	/* highway */
	{'`', "grass",		C(CLR_GREEN)},	/* grassland */
	{'2', "nether mist",		C(CLR_BRIGHT_MAGENTA)},	/* nethermist */
	{'1', "stalactite",		C(CLR_ORANGE)},	/* stalactite */
	{'`', "floor of the crypt",		C(CLR_BRIGHT_BLUE)},	/* cryptfloor */
/*70*/	{'2', "bubble",		C(CLR_GRAY)},	/* bubbles */
	{'2', "rain cloud",		C(CLR_BRIGHT_BLUE)},	/* raincloud */
	{'|', "pentagram inscribed on the floor",		C(CLR_ORANGE)},	/* pentagram */
	{'|', "ornately carved bed",		C(CLR_BRIGHT_CYAN)},	/* carvedbed */
	{'#', "grayout",		C(CLR_GRAY)},	/* gray glyph */
	{'^', "arrow trap",	C(HI_METAL)},	/* trap */
	{'^', "dart trap",	C(HI_METAL)},	/* trap */
	{'^', "falling rock trap",C(CLR_GRAY)},	/* trap */
	{'^', "squeaky board",	C(CLR_BROWN)},	/* trap */
	{'^', "bear trap",	C(HI_METAL)},	/* trap */
	{'^', "land mine",	C(CLR_RED)},	/* trap */
	{'^', "rolling boulder trap",	C(CLR_GRAY)},	/* trap */
	{'^', "sleeping gas trap",C(HI_ZAP)},	/* trap */
	{'^', "rust trap",	C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "fire trap",	C(CLR_ORANGE)},	/* trap */
	{'^', "pit",		C(CLR_BLACK)},	/* trap */
	{'^', "spiked pit",	C(CLR_BLACK)},	/* trap */
	{'^', "hole",	C(CLR_BROWN)},	/* trap */
	{'^', "trap door",	C(CLR_BROWN)},	/* trap */
	{'^', "teleportation trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "level teleporter", C(CLR_MAGENTA)},	/* trap */
	{'^', "magic portal",	C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "web",		C(CLR_WHITE)},	/* web */
	{'^', "statue trap",	C(CLR_GRAY)},	/* trap */
	{'^', "magic trap",	C(HI_ZAP)},	/* trap */
	{'^', "anti-magic field", C(HI_ZAP)},	/* trap */
	{'^', "polymorph trap",	C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "ice trap",	C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "spear trap",	C(CLR_BROWN)},	/* trap */
	{'^', "ceiling collapse trap", C(CLR_GRAY)},	/* trap */
	{'^', "magic beam trap", C(CLR_YELLOW)},	/* trap */
	{'^', "heap of shit", C(CLR_ORANGE)},	/* trap */
	{'^', "animation trap", C(CLR_WHITE)},	/* trap */
	{'^', "glyph of warding", C(CLR_RED)},	/* trap */
	{'^', "blade trap", C(CLR_RED)},	/* trap */
	{'^', "bolt trap", C(CLR_GREEN)},	/* trap */
	{'^', "acidpool", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "waterpool", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "poison gas trap", C(CLR_YELLOW)},	/* trap */
	{'^', "slow gas trap", C(CLR_GREEN)},	/* trap */
	{'^', "shock trap", C(CLR_ORANGE)},	/* trap */
	{'^', "rmb loss trap", C(CLR_WHITE)},	/* trap */
	{'^', "display trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "spell loss trap", C(CLR_GRAY)},	/* trap */
	{'^', "yellow spell trap", C(CLR_YELLOW)},	/* trap */
	{'^', "auto destruct trap", C(CLR_ORANGE)},	/* trap */
	{'^', "memory trap", C(CLR_RED)},	/* trap */
	{'^', "inventory trap", C(CLR_WHITE)},	/* trap */
	{'^', "black ng wall trap", C(CLR_BLACK)},	/* trap */
	{'^', "menu trap", C(CLR_BROWN)},	/* trap */
	{'^', "speed trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "superscroller trap", C(CLR_ORANGE)},	/* trap */
	{'^', "active superscroller trap", C(CLR_ORANGE)},	/* trap */
	{'^', "swarm trap", C(CLR_WHITE)},	/* trap */
	{'^', "automatic switcher", C(CLR_GREEN)},	/* trap */
	{'^', "an unknown trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "heel trap", C(CLR_GREEN)},	/* trap */
	{'^', "vulnerability trap", C(CLR_YELLOW)},	/* trap */

	{'^', "lava trap", C(CLR_RED)},	/* trap */
	{'^', "flood trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "drain trap", C(CLR_BLACK)},	/* trap */
	{'^', "free hand trap", C(CLR_YELLOW)},	/* trap */
	{'^', "disintegration trap", C(CLR_BLACK)},	/* trap */
	{'^', "unidentify trap", C(CLR_GREEN)},	/* trap */
	{'^', "thirst trap", C(CLR_CYAN)},	/* trap */
	{'^', "luck trap", C(CLR_YELLOW)},	/* trap */
	{'^', "shades of grey trap", C(CLR_GRAY)},	/* trap */
	{'^', "item teleporter", C(CLR_MAGENTA)},	/* trap */
	{'^', "gravity trap", C(CLR_GRAY)},	/* trap */
	{'^', "death trap", C(CLR_BLACK)},	/* trap */
	{'^', "stone to flesh trap", C(CLR_WHITE)},	/* trap */
	{'^', "quicksand", C(CLR_BROWN)},	/* trap */
	{'^', "faint trap", C(CLR_BLACK)},	/* trap */
	{'^', "curse trap", C(CLR_GREEN)},	/* trap */
	{'^', "difficulty trap", C(CLR_BLACK)},	/* trap */
	{'^', "sound trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "caster trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "weakness trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "rot13 trap", C(CLR_ORANGE)},	/* trap */
	{'^', "bishop trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "sudden confusion trap", C(CLR_CYAN)},	/* trap */
	{'^', "super nupesell nix area", C(NO_COLOR)},	/* trap */
	{'^', "drop trap", C(CLR_BROWN)},	/* trap */
	{'^', "DSTW trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "status trap", C(CLR_WHITE)},	/* trap */
	{'^', "alignment trap", C(CLR_BLACK)},	/* trap */
	{'^', "stairs trap", C(CLR_GRAY)},	/* trap */

	{'^', "pestilence trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "famine trap", C(CLR_MAGENTA)},	/* trap */

	{'^', "relocation trap", C(CLR_MAGENTA)},	/* trap */

	{'^', "uninformation trap", C(CLR_BRIGHT_CYAN)},	/* trap */

	{'^', "intrinsic loss trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "blood loss trap", C(CLR_RED)},	/* trap */
	{'^', "bad effect trap", C(CLR_BROWN)},	/* trap */
	{'^', "multiplicator trap", C(CLR_WHITE)},	/* trap */
	{'^', "automatic vulnerability trap", C(CLR_YELLOW)},	/* trap */
	{'^', "teleporting items trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "nastiness trap", C(CLR_MAGENTA)},	/* trap */

	{'^', "recursion trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "respawn trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "warp zone", C(CLR_MAGENTA)},	/* trap */
	{'^', "captcha trap", C(CLR_YELLOW)},	/* trap */
	{'^', "mind wipe trap", C(CLR_YELLOW)},	/* trap */
	{'^', "throwing star trap", C(CLR_CYAN)},	/* trap */
	{'^', "shit pit", C(CLR_ORANGE)},	/* trap */
	{'^', "shaft", C(CLR_BROWN)},	/* trap */
	{'^', "lock trap", C(CLR_CYAN)},	/* trap */
	{'^', "MC trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "farlook trap", C(CLR_RED)},	/* trap */
	{'^', "gateway from hell", C(CLR_MAGENTA)},	/* trap */
	{'^', "growing trap", C(CLR_GREEN)},	/* trap */
	{'^', "cooling trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "bar trap", C(CLR_CYAN)},	/* trap */
	{'^', "locking trap", C(CLR_BLACK)},	/* trap */
	{'^', "air trap", C(CLR_WHITE)},	/* trap */
	{'^', "terrain trap", C(CLR_BRIGHT_BLUE)},	/* trap */

	{'^', "loudspeaker", C(CLR_BLACK)},	/* trap */
	{'^', "nest trap", C(CLR_YELLOW)},	/* trap */
	{'^', "bullet trap", C(CLR_GRAY)},	/* trap */
	{'^', "paralysis trap", C(CLR_CYAN)},	/* trap */
	{'^', "cyanide trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "laser trap", C(CLR_YELLOW)},	/* trap */
	{'^', "giant chasm", C(CLR_BROWN)},	/* trap */
	{'^', "fart trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "confusion trap", C(CLR_GREEN)},	/* trap */
	{'^', "stun trap", C(CLR_ORANGE)},	/* trap */
	{'^', "hallucination trap", C(CLR_YELLOW)},	/* trap */
	{'^', "petrification trap", C(CLR_BLACK)},	/* trap */
	{'^', "numbness trap", C(CLR_ORANGE)},	/* trap */
	{'^', "freezing trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "burning trap", C(CLR_RED)},	/* trap */
	{'^', "fear trap", C(CLR_WHITE)},	/* trap */
	{'^', "blindness trap", C(CLR_YELLOW)},	/* trap */
	{'^', "glib trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "slime trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "inertia trap", C(CLR_YELLOW)},	/* trap */
	{'^', "time trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "lycanthropy trap", C(CLR_BLACK)},	/* trap */
	{'^', "unlight trap", C(CLR_BLACK)},	/* trap */
	{'^', "elemental trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "escalating trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "negative trap", C(CLR_BLACK)},	/* trap */
	{'^', "mana trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "sin trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "destroy armor trap", C(CLR_BLACK)},	/* trap */
	{'^', "divine anger trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "genetic trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "missingno trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "cancellation trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "hostility trap", C(CLR_BLACK)},	/* trap */
	{'^', "boss trap", C(CLR_MAGENTA)},	/* trap */
	{'$', "wishing trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "amnesia trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "bigscript trap", C(CLR_WHITE)},	/* trap */
	{'^', "bank trap", C(CLR_BROWN)},	/* trap */
	{'^', "Only percentapercents can go there!", C(CLR_RED)},	/* trap */
	{'^', "map trap", C(CLR_BROWN)},	/* trap */
	{'^', "tech trap", C(CLR_YELLOW)},	/* trap */
	{'^', "disenchantment trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "verisiert!", C(CLR_WHITE)},	/* trap */
	{'^', "chaos trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "muteness trap", C(CLR_BROWN)},	/* trap */
	{'^', "ntll", C(CLR_BROWN)},	/* trap */
	{'^', "engraving trap", C(CLR_WHITE)},	/* trap */
	{'^', "magical device trap", C(CLR_CYAN)},	/* trap */
	{'^', "book trap", C(CLR_BROWN)},	/* trap */
	{'^', "level trap", C(CLR_RED)},	/* trap */
	{'^', "quiz trap", C(CLR_ORANGE)},	/* trap */

	{'^', "falling boulder trap", C(CLR_GRAY)},	/* trap */
	{'^', "glass arrow trap", C(CLR_CYAN)},	/* trap */
	{'^', "glass bolt trap", C(CLR_CYAN)},	/* trap */
	{'^', "out of magic trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "plasma trap", C(CLR_ORANGE)},	/* trap */
	{'^', "contact bomb", C(CLR_ORANGE)},	/* trap */
	{'^', "earthquake trap", C(CLR_BLACK)},	/* trap */
	{'^', "noise trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "glue trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "guillotine trap", C(CLR_BLACK)},	/* trap */
	{'^', "bisection trap", C(CLR_RED)},	/* trap */
	{'^', "volt trap", C(CLR_YELLOW)},	/* trap */
	{'^', "horde trap", C(CLR_WHITE)},	/* trap */
	{'^', "immobility trap", C(CLR_YELLOW)},	/* trap */
	{'^', "glyph of guarding", C(CLR_GREEN)},	/* trap */
	{'^', "glyph of protection", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "glyph of animation", C(CLR_YELLOW)},	/* trap */
	{'^', "glyph of summoning", C(CLR_ORANGE)},	/* trap */
	{'^', "glyph of massive damage", C(CLR_BLACK)},	/* trap */
	{'^', "glyph number seven", C(CLR_MAGENTA)},	/* trap */

	{'^', "metabolic trap", C(CLR_ORANGE)},	/* trap */
	{'^', "trap of no return", C(CLR_YELLOW)},	/* trap */
	{'^', "egotrap", C(CLR_YELLOW)},	/* trap */
	{'^', "fast forward trap", C(CLR_YELLOW)},	/* trap */
	{'^', "trap of rottenness", C(CLR_BROWN)},	/* trap */
	{'^', "unskilled trap", C(CLR_CYAN)},	/* trap */
	{'^', "low stats trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "training trap", C(CLR_BROWN)},	/* trap */
	{'^', "exercise trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "falling loadstone trap", C(CLR_GRAY)},	/* trap */
	{'^', "summon undead trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "falling nastystone trap", C(CLR_GRAY)},	/* trap */

	{'^', "spined ball trap", C(CLR_CYAN)},	/* trap */
	{'^', "pendulum trap", C(CLR_CYAN)},	/* trap */
	{'^', "turn table", C(CLR_BROWN)},	/* trap */
	{'^', "scent trap", C(CLR_ORANGE)},	/* trap */
	{'^', "banana trap", C(CLR_YELLOW)},	/* trap */
	{'^', "falling tub trap", C(CLR_CYAN)},	/* trap */
	{'^', "alarm", C(CLR_RED)},	/* trap */
	{'^', "caltrops trap", C(CLR_CYAN)},	/* trap */
	{'^', "blade wire", C(CLR_BLACK)},	/* trap */
	{'^', "magnet trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "slingshot trap", C(CLR_GRAY)},	/* trap */
	{'^', "cannon trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "venom sprinkler", C(CLR_YELLOW)},	/* trap */
	{'^', "fumarole", C(CLR_GREEN)},	/* trap */

	{'^', "mana pit", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "elemental portal", C(CLR_GRAY)},	/* trap */
	{'^', "girliness trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "fumbling trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "egomonster trap", C(CLR_YELLOW)},	/* trap */
	{'^', "flooding trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "monster cube", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "cursed grave", C(CLR_BLACK)},	/* trap */
	{'^', "limitation trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "weak sight trap", C(CLR_YELLOW)},	/* trap */
	{'^', "random message trap", C(CLR_WHITE)},	/* trap */

	{'^', "desecration trap", C(CLR_BLACK)},	/* trap */
	{'^', "starvation trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "dropless trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "low effect trap", C(CLR_YELLOW)},	/* trap */
	{'^', "invisible trap", C(CLR_WHITE)},	/* trap */
	{'^', "ghost world trap", C(CLR_WHITE)},	/* trap */
	{'^', "dehydration trap", C(CLR_CYAN)},	/* trap */
	{'^', "hate trap", C(CLR_BLACK)},	/* trap */
	{'^', "spacewars trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "temporary recursion trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "totter trap", C(CLR_GREEN)},	/* trap */
	{'^', "nonintrinsical trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "dropcurse trap", C(CLR_YELLOW)},	/* trap */
	{'^', "nakedness trap", C(CLR_BROWN)},	/* trap */
	{'^', "antilevel trap", C(CLR_BLACK)},	/* trap */
	{'^', "ventilator", C(CLR_BLACK)},	/* trap */
	{'^', "stealer trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "rebellion trap", C(CLR_WHITE)},	/* trap */
	{'^', "crap trap", C(CLR_YELLOW)},	/* trap */
	{'^', "misfire trap", C(CLR_RED)},	/* trap */
	{'^', "trap of walls", C(CLR_ORANGE)},	/* trap */

	{'^', "disconnect trap", C(CLR_CYAN)},	/* trap */
	{'^', "interface screw trap", C(CLR_YELLOW)},	/* trap */
	{'^', "dimness trap", C(CLR_CYAN)},	/* trap */
	{'^', "evil artifact trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "bossfight trap", C(CLR_RED)},	/* trap */
	{'^', "entire level trap", C(CLR_YELLOW)},	/* trap */
	{'^', "bones trap", C(CLR_GRAY)},	/* trap */
	{'^', "rodney trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "autocurse trap", C(CLR_ORANGE)},	/* trap */
	{'^', "highlevel trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "spell forgetting trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "sound effect trap", C(CLR_ORANGE)},	/* trap */

	{'^', "trap percents", C(CLR_WHITE)},	/* trap */

	{'^', "nexus trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "leg trap", C(CLR_BROWN)},	/* trap */
	{'$', "artifact jackpot trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "map amnesia trap", C(CLR_YELLOW)},	/* trap */
	{'^', "spreading trap", C(CLR_CYAN)},	/* trap */
	{'^', "adjacent trap", C(CLR_BLACK)},	/* trap */
	{'#', "superthing from level 55", C(CLR_GRAY)},	/* trap */
	{'^', "current shaft", C(CLR_BROWN)},	/* trap */
	{'^', "levitation trap", C(CLR_CYAN)},	/* trap */
	{'^', "bowel cramps trap", C(CLR_RED)},	/* trap */
	{'^', "unequipping trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'$', "good artifact trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "gender trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "trap of opposite alignment", C(CLR_CYAN)},	/* trap */
	{'^', "sincount trap", C(CLR_YELLOW)},	/* trap */
	{'^', "beamer", C(CLR_MAGENTA)},	/* trap */
	{'^', "level beamer", C(CLR_MAGENTA)},	/* trap */
	{'^', "pet trap", C(CLR_BLACK)},	/* trap */
	{'^', "piercing beam trap", C(CLR_YELLOW)},	/* trap */
	{'^', "wrenching trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "tracker trap", C(CLR_MAGENTA)},	/* trap */
	{'$', "nurse trap", C(CLR_ORANGE)},	/* trap */
	{'^', "back to start trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "nemesis trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "strew trap", C(CLR_WHITE)},	/* trap */
	{'^', "outta depth trap", C(CLR_BLACK)},	/* trap */
	{'^', "punishment trap", C(CLR_CYAN)},	/* trap */
	{'$', "boon trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "fountain trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "throne trap", C(CLR_YELLOW)},	/* trap */
	{'^', "anoxic pit", C(CLR_BLACK)},	/* trap */
	{'#', "arabella speaker", C(CLR_BLACK)},	/* trap */
	{'^', "femmy trap", C(CLR_YELLOW)},	/* trap */
	{'^', "madeleine trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "marlena trap", C(CLR_GREEN)},	/* trap */
	{'^', "anastasia trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "filler trap", C(CLR_YELLOW)},	/* trap */
	{'^', "toxic venom trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "insanity trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "madness trap", C(CLR_YELLOW)},	/* trap */
	{'^', "jessica trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "solvejg trap", C(CLR_ORANGE)},	/* trap */
	{'^', "wendy trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "katharina trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "elena trap", C(CLR_BLACK)},	/* trap */
	{'^', "thai trap", C(CLR_BLACK)},	/* trap */
	{'^', "elif trap", C(CLR_WHITE)},	/* trap */
	{'^', "nadja trap", C(CLR_ORANGE)},	/* trap */
	{'^', "sandra trap", C(CLR_BROWN)},	/* trap */
	{'^', "natalje trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "jeanetta trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "yvonne trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "maurah trap", C(CLR_YELLOW)},	/* trap */

	{'^', "lootcut trap", C(CLR_BROWN)},	/* trap */
	{'^', "monster speed trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "scaling trap", C(CLR_YELLOW)},	/* trap */
	{'^', "enmity trap", C(CLR_BLACK)},	/* trap */
	{'^', "white spell trap", C(CLR_WHITE)},	/* trap */
	{'^', "complete gray spell trap", C(CLR_GRAY)},	/* trap */
	{'^', "quasar trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "momma trap", C(CLR_CYAN)},	/* trap */
	{'^', "horror trap", C(CLR_ORANGE)},	/* trap */
	{'^', "artificer trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "wereform trap", C(CLR_BROWN)},	/* trap */
	{'^', "non-prayer trap", C(CLR_BLACK)},	/* trap */
	{'^', "evil patch trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "hard mode trap", C(CLR_RED)},	/* trap */
	{'^', "secret attack trap", C(CLR_GREEN)},	/* trap */
	{'^', "eater trap", C(CLR_GREEN)},	/* trap */
	{'^', "covetousness trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "not seen trap", C(CLR_BLACK)},	/* trap */
	{'^', "dark mode trap", C(CLR_BLACK)},	/* trap */
	{'^', "antisearch trap", C(CLR_CYAN)},	/* trap */
	{'^', "homicide trap", C(CLR_BLACK)},	/* trap */
	{'^', "nasty nation trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "wakeup call trap", C(CLR_WHITE)},	/* trap */
	{'^', "grayout trap", C(CLR_GRAY)},	/* trap */
	{'^', "gray center trap", C(CLR_GRAY)},	/* trap */
	{'^', "checkerboard trap", C(CLR_GRAY)},	/* trap */
	{'^', "clockwise spin trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "counterclockwise spin trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "lag trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "blesscurse trap", C(CLR_RED)},	/* trap */
	{'^', "de-light trap", C(CLR_CYAN)},	/* trap */
	{'^', "discharge trap", C(CLR_CYAN)},	/* trap */
	{'^', "trashing trap", C(CLR_BROWN)},	/* trap */
	{'^', "filtering trap", C(CLR_BROWN)},	/* trap */
	{'^', "deformatting trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "flicker strip trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "undressing trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "hyperbluewall trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "nolite trap", C(CLR_WHITE)},	/* trap */
	{'^', "paranoia trap", C(CLR_YELLOW)},	/* trap */
	{'^', "fleecescript trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "interrupt trap", C(CLR_YELLOW)},	/* trap */
	{'^', "dustbin trap", C(CLR_BROWN)},	/* trap */
	{'^', "mana battery trap", C(CLR_CYAN)},	/* trap */
	{'^', "monsterfingers trap", C(CLR_GREEN)},	/* trap */
	{'^', "miscast trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "message suppression trap", C(CLR_WHITE)},	/* trap */
	{'^', "stuck announcement trap", C(CLR_YELLOW)},	/* trap */
	{'^', "bloodthirsty trap", C(CLR_RED)},	/* trap */
	{'^', "maximum damage trap", C(CLR_RED)},	/* trap */
	{'^', "latency trap", C(CLR_WHITE)},	/* trap */
	{'^', "starlit trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "knowledge trap", C(CLR_YELLOW)},	/* trap */
	{'^', "highscore trap", C(CLR_GRAY)},	/* trap */
	{'^', "pink spell trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "green spell trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "EVC trap", C(CLR_GRAY)},	/* trap */
	{'^', "underlayer trap", C(CLR_BLACK)},	/* trap */
	{'^', "damage meter trap", C(CLR_RED)},	/* trap */
	{'^', "arbitrary weight trap", C(CLR_GRAY)},	/* trap */
	{'^', "fucked info trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "black spell trap", C(CLR_BLACK)},	/* trap */
	{'^', "cyan spell trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "heap trap", C(CLR_BROWN)},	/* trap */
	{'^', "blue spell trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "tron trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "red spell trap", C(CLR_RED)},	/* trap */
	{'^', "too heavy trap", C(CLR_BROWN)},	/* trap */
	{'^', "elongation trap", C(CLR_YELLOW)},	/* trap */
	{'^', "wrapover trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "destruction trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "melee prefix trap", C(CLR_CYAN)},	/* trap */
	{'^', "automore trap", C(CLR_BLACK)},	/* trap */
	{'^', "unfair attack trap", C(CLR_MAGENTA)},	/* trap */

	{'^', "kop cube", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "boss spawner", C(CLR_MAGENTA)},	/* trap */
	{'^', "contamination trap", C(CLR_BRIGHT_CYAN)},	/* trap */

	{'^', "evil heel trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "bad equipment trap", C(CLR_BLACK)},	/* trap */
	{'^', "tempoconflict trap", C(CLR_CYAN)},	/* trap */
	{'^', "tempohunger trap", C(CLR_YELLOW)},	/* trap */
	{'^', "teleportitis trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "polymorphitis trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "premature death trap", C(CLR_BLACK)},	/* trap */
	{'^', "lasting amnesia trap", C(CLR_YELLOW)},	/* trap */
	{'^', "ragnarok trap", C(CLR_ORANGE)},	/* trap */
	{'^', "single disenchant trap", C(CLR_BLACK)},	/* trap */
	{'^', "acid pit", C(CLR_GREEN)},	/* trap */

	{'^', "severe disenchant trap", C(CLR_BLACK)},	/* trap */
	{'^', "pain trap", C(CLR_YELLOW)},	/* trap */
	{'^', "trembling trap", C(CLR_BROWN)},	/* trap */
	{'^', "techcap trap", C(CLR_BROWN)},	/* trap */
	{'^', "spell memory trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "skill reduction trap", C(CLR_CYAN)},	/* trap */
	{'^', "skillcap trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "permanent stat damage trap", C(CLR_BRIGHT_BLUE)},	/* trap */

	{'^', "orange spell trap", C(CLR_ORANGE)},	/* trap */
	{'^', "violet spell trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "trap of longing", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "cursed part trap", C(CLR_CYAN)},	/* trap */
	{'^', "quaversal trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "appearance shuffling trap", C(CLR_YELLOW)},	/* trap */
	{'^', "brown spell trap", C(CLR_BROWN)},	/* trap */
	{'^', "choiceless trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "goldspell trap", C(CLR_YELLOW)},	/* trap */
	{'^', "deprovement trap", C(CLR_BLACK)},	/* trap */
	{'^', "initialization trap", C(CLR_WHITE)},	/* trap */
	{'^', "gushlush trap", C(CLR_GREEN)},	/* trap */
	{'^', "soiltype trap", C(CLR_BROWN)},	/* trap */
	{'^', "dangerous terrain trap", C(CLR_RED)},	/* trap */
	{'^', "fallout trap", C(CLR_CYAN)},	/* trap */
	{'^', "mojibake trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "gravation trap", C(CLR_BLACK)},	/* trap */
	{'^', "uncalled trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "exploding dice trap", C(CLR_ORANGE)},	/* trap */
	{'^', "permacurse trap", C(CLR_BLACK)},	/* trap */
	{'^', "shrouded identity trap", C(CLR_GRAY)},	/* trap */
	{'^', "feeler gauges trap", C(CLR_RED)},	/* trap */
	{'^', "long screwup trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "wing yellow changer", C(CLR_YELLOW)},	/* trap */
	{'^', "life saving trap", C(CLR_BLACK)},	/* trap */
	{'^', "curseuse trap", C(CLR_BLACK)},	/* trap */
	{'^', "cut nutrition trap", C(CLR_YELLOW)},	/* trap */
	{'^', "skill loss trap", C(CLR_BROWN)},	/* trap */
	{'^', "autopilot trap", C(CLR_CYAN)},	/* trap */
	{'^', "force trap", C(CLR_CYAN)},	/* trap */
	{'^', "monster glyph trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "changing directive trap", C(CLR_WHITE)},	/* trap */
	{'^', "container kaboom trap", C(CLR_RED)},	/* trap */
	{'^', "steal degrade trap", C(CLR_GRAY)},	/* trap */
	{'^', "left inventory trap", C(CLR_WHITE)},	/* trap */
	{'^', "fluctuating speed trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "tarmustrokingnora trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "failure trap", C(CLR_GREEN)},	/* trap */
	{'^', "bright cyan spell trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "frequentation spawn trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "pet AI trap", C(CLR_YELLOW)},	/* trap */
	{'^', "satan trap", C(CLR_RED)},	/* trap */
	{'^', "rememberance trap", C(CLR_BROWN)},	/* trap */
	{'^', "pokelie trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "autopickup trap", C(CLR_BLACK)},	/* trap */
	{'^', "dywypi trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "silver spell trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "metal spell trap", C(CLR_CYAN)},	/* trap */
	{'^', "platinum spell trap", C(CLR_BLACK)},	/* trap */
	{'^', "manler trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "doorning trap", C(CLR_BROWN)},	/* trap */
	{'^', "nownsible trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "elm street trap", C(CLR_GREEN)},	/* trap */
	{'^', "monster noise trap", C(CLR_RED)},	/* trap */
	{'^', "rang call trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "recurring spell loss trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "antitraining trap", C(CLR_BROWN)},	/* trap */
	{'^', "techout trap", C(CLR_BROWN)},	/* trap */
	{'^', "stat decay trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "movemork trap", C(CLR_GREEN)},	/* trap */

	{'^', "hybrid trap", C(CLR_YELLOW)},	/* trap */
	{'^', "shapechange trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "meltem trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "miguc trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "directive trap", C(CLR_WHITE)},	/* trap */
	{'^', "satatue trap", C(CLR_GRAY)},	/* trap */
	{'^', "farting web", C(CLR_MAGENTA)},	/* trap */
	{'^', "cataclysm trap", C(CLR_ORANGE)},	/* trap */
	{'^', "data delete trap", C(CLR_BLACK)},	/* trap */
	{'^', "elder tentacling trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "footerer trap", C(CLR_GREEN)},	/* trap */

	{'^', "bad part trap", C(CLR_BLACK)},	/* trap */
	{'^', "completely bad part trap", C(CLR_GRAY)},	/* trap */
	{'^', "evil variant trap", C(CLR_MAGENTA)},	/* trap */

	{'^', "grave wall trap", C(CLR_YELLOW)},	/* trap */
	{'^', "tunnel trap", C(CLR_RED)},	/* trap */
	{'^', "farmland trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "mountain trap", C(CLR_BLACK)},	/* trap */
	{'^', "water tunnel trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "crystal flood trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "moorland trap", C(CLR_GREEN)},	/* trap */
	{'^', "urine trap", C(CLR_YELLOW)},	/* trap */
	{'^', "shifting sand trap", C(CLR_BROWN)},	/* trap */
	{'^', "styx trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "pentagram trap", C(CLR_ORANGE)},	/* trap */
	{'^', "snow trap", C(CLR_WHITE)},	/* trap */
	{'^', "ash trap", C(CLR_RED)},	/* trap */
	{'^', "sand trap", C(CLR_BROWN)},	/* trap */
	{'^', "pavement trap", C(CLR_GRAY)},	/* trap */
	{'^', "highway trap", C(CLR_YELLOW)},	/* trap */
	{'^', "grassland trap", C(CLR_GREEN)},	/* trap */
	{'^', "nether mist trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "stalactite trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "cryptfloor trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "bubble trap", C(CLR_ORANGE)},	/* trap */
	{'^', "rain cloud trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "item nastification trap", C(CLR_BLACK)},	/* trap */
	{'^', "sanity increase trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "psi trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "gay trap", C(CLR_YELLOW)},	/* trap */
	{'^', "sarah trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "claudia trap", C(CLR_YELLOW)},	/* trap */
	{'^', "ludgera trap", C(CLR_GRAY)},	/* trap */
	{'^', "kati trap", C(CLR_RED)},	/* trap */
	{'^', "sanity treble trap", C(CLR_GRAY)},	/* trap */
	{'^', "stat decrease trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "simeout trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "s-pressing trap", C(CLR_BROWN)},	/* trap */
	{'^', "nelly trap", C(CLR_BROWN)},	/* trap */
	{'^', "eveline trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "karin trap", C(CLR_WHITE)},	/* trap */
	{'^', "juen trap", C(CLR_BLACK)},	/* trap */
	{'^', "kristina trap", C(CLR_ORANGE)},	/* trap */
	{'^', "lou trap", C(CLR_GRAY)},	/* trap */
	{'^', "almut trap", C(CLR_WHITE)},	/* trap */
	{'^', "julietta trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "arabella trap", C(CLR_ORANGE)},	/* trap */
	{'^', "kristin trap", C(CLR_BLACK)},	/* trap */
	{'^', "anna trap", C(CLR_BLACK)},	/* trap */
	{'^', "ruea trap", C(CLR_BROWN)},	/* trap */
	{'^', "dora trap", C(CLR_ORANGE)},	/* trap */
	{'^', "marike trap", C(CLR_WHITE)},	/* trap */
	{'^', "jette trap", C(CLR_BLACK)},	/* trap */
	{'^', "ina trap", C(CLR_ORANGE)},	/* trap */
	{'^', "sing trap", C(CLR_RED)},	/* trap */
	{'^', "victoria trap", C(CLR_BROWN)},	/* trap */
	{'^', "melissa trap", C(CLR_BLACK)},	/* trap */
	{'^', "anita trap", C(CLR_BLACK)},	/* trap */
	{'^', "henrietta trap", C(CLR_BROWN)},	/* trap */
	{'^', "verena trap", C(CLR_BLACK)},	/* trap */
	{'^', "annemarie trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "jil trap", C(CLR_BROWN)},	/* trap */
	{'^', "jana trap", C(CLR_BLUE)},	/* trap */
	{'^', "katrin trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "gudrun trap", C(CLR_CYAN)},	/* trap */
	{'^', "ella trap", C(CLR_WHITE)},	/* trap */
	{'^', "manuela trap", C(CLR_RED)},	/* trap */
	{'^', "jennifer trap", C(CLR_BLACK)},	/* trap */
	{'^', "patricia trap", C(CLR_BLUE)},	/* trap */
	{'^', "antje trap", C(CLR_BLUE)},	/* trap */
	{'^', "antje trap 2", C(CLR_BLACK)},	/* trap */
	{'^', "kerstin trap", C(CLR_BLUE)},	/* trap */
	{'^', "laura trap", C(CLR_ORANGE)},	/* trap */
	{'^', "larissa trap", C(CLR_BLUE)},	/* trap */
	{'^', "nora trap", C(CLR_BLACK)},	/* trap */
	{'^', "natalia trap", C(CLR_GRAY)},	/* trap */
	{'^', "susanne trap", C(CLR_BLACK)},	/* trap */
	{'^', "lisa trap", C(CLR_YELLOW)},	/* trap */
	{'^', "bridghitte trap", C(CLR_RED)},	/* trap */
	{'^', "julia trap", C(CLR_ORANGE)},	/* trap */
	{'^', "nicole trap", C(CLR_BLACK)},	/* trap */
	{'^', "rita trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "janina trap", C(CLR_ORANGE)},	/* trap */
	{'^', "rosa trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "ksenia trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "lydia trap", C(CLR_RED)},	/* trap */
	{'^', "conny trap", C(CLR_BLACK)},	/* trap */
	{'^', "katia trap", C(CLR_BROWN)},	/* trap */
	{'^', "mariya trap", C(CLR_BLACK)},	/* trap */
	{'^', "elise trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "ronja trap", C(CLR_BLACK)},	/* trap */
	{'^', "ariane trap", C(CLR_BLACK)},	/* trap */
	{'^', "johanna trap", C(CLR_RED)},	/* trap */
	{'^', "inge trap", C(CLR_BLACK)},	/* trap */

	{'^', "wall trap", C(CLR_GRAY)},	/* trap */
	{'^', "monster generator", C(CLR_CYAN)},	/* trap */
	{'^', "potion dispenser", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "spacewars spawn trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "tv tropes trap", C(CLR_ORANGE)},	/* trap */
	{'^', "symbiote trap", C(CLR_WHITE)},	/* trap */
	{'^', "kill symbiote trap", C(CLR_BLACK)},	/* trap */
	{'^', "symbiote replacement trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "shutdown trap", C(CLR_BLUE)},	/* trap */
	{'^', "corona trap", C(CLR_BLUE)},	/* trap */
	{'^', "unproofing trap", C(CLR_BLUE)},	/* trap */
	{'^', "visibility trap", C(CLR_WHITE)},	/* trap */
	{'^', "feminism stone trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "branch teleporter", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "branch beamer", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "poison arrow trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "poison bolt trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "mace trap", C(CLR_CYAN)},	/* trap */
	{'^', "shueft trap", C(CLR_YELLOW)},	/* trap */
	{'^', "moth larvae trap", C(CLR_BROWN)},	/* trap */
	{'^', "worthiness trap", C(CLR_YELLOW)},	/* trap */
	{'^', "conduct trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "strikethrough trap", C(CLR_YELLOW)},	/* trap */
	{'^', "multiple gather trap", C(CLR_BROWN)},	/* trap */
	{'^', "vivisection trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "instafeminism trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "instanasty trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "skill point loss trap", C(CLR_BLUE)},	/* trap */
	{'^', "perfect match trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "dumbie lightsaber trap", C(CLR_MAGENTA)},	/* trap */
	{'>', "wrong stairs", C(CLR_CYAN)},	/* trap */
	{'^', "techstop trap", C(CLR_ORANGE)},	/* trap */
	{'^', "amnesia switch trap", C(CLR_WHITE)},	/* trap */
	{'^', "skill swap trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "skill upordown trap", C(CLR_BLUE)},	/* trap */
	{'^', "skill randomize trap", C(CLR_RED)},	/* trap */

	{'^', "skill multiply trap", C(CLR_GRAY)},	/* trap */
	{'^', "trapwalk trap", C(CLR_CYAN)},	/* trap */
	{'^', "cluster trap", C(CLR_GREEN)},	/* trap */
	{'^', "field trap", C(CLR_WHITE)},	/* trap */
	{'^', "monicide trap", C(CLR_YELLOW)},	/* trap */
	{'^', "trap creation trap", C(CLR_ORANGE)},	/* trap */
	{'^', "leold trap", C(CLR_RED)},	/* trap */
	{'^', "animeband trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "perfume trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "court trap", C(CLR_GREEN)},	/* trap */
	{'^', "elder scrolls trap", C(CLR_BLACK)},	/* trap */
	{'^', "joke trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "dungeon lords trap", C(CLR_GRAY)},	/* trap */
	{'^', "fortytwo trap", C(CLR_YELLOW)},	/* trap */
	{'^', "randomize trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "evilroom trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "aoe trap", C(CLR_BROWN)},	/* trap */
	{'^', "elona trap", C(CLR_YELLOW)},	/* trap */
	{'^', "religion trap", C(CLR_BROWN)},	/* trap */
	{'^', "steamband trap", C(CLR_RED)},	/* trap */
	{'^', "hardcore trap", C(CLR_BLACK)},	/* trap */
	{'^', "machine trap", C(CLR_CYAN)},	/* trap */
	{'^', "bee trap", C(CLR_YELLOW)},	/* trap */
	{'^', "migo trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "angband trap", C(CLR_BLACK)},	/* trap */
	{'^', "dnethack trap", C(CLR_BLUE)},	/* trap */
	{'^', "evil spawn trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "shoe trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "inside trap", C(CLR_BLUE)},	/* trap */
	{'^', "doom trap", C(CLR_GRAY)},	/* trap */
	{'^', "military trap", C(CLR_GRAY)},	/* trap */
	{'^', "illusion trap", C(CLR_CYAN)},	/* trap */
	{'^', "diablo trap", C(CLR_ORANGE)},	/* trap */

	{'^', "giant explorer trap", C(CLR_GRAY)},	/* trap */
	{'^', "trapwarp trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "yawm trap", C(CLR_YELLOW)},	/* trap */
	{'^', "cradle of chaos trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "tezcatlipoca trap", C(CLR_ORANGE)},	/* trap */
	{'^', "enthumesis trap", C(CLR_WHITE)},	/* trap */
	{'^', "mikraanesis trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "go'ts too good trap", C(CLR_RED)},	/* trap */
	{'^', "killer room trap", C(CLR_RED)},	/* trap */
	{'^', "no fun walls trap", C(CLR_BLUE)},	/* trap */

	{'^', "ruth trap", C(CLR_WHITE)},	/* trap */
	{'^', "magdalena trap", C(CLR_BROWN)},	/* trap */
	{'^', "marleen trap", C(CLR_YELLOW)},	/* trap */
	{'^', "klara trap", C(CLR_BLUE)},	/* trap */
	{'^', "friederike trap", C(CLR_ORANGE)},	/* trap */
	{'^', "naomi trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "ute trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "jasieen trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "yasaman trap", C(CLR_GRAY)},	/* trap */
	{'^', "may-britt trap", C(CLR_BLACK)},	/* trap */

	{'^', "trap of calling out", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "field break trap", C(CLR_BROWN)},	/* trap */
	{'^', "tenth trap", C(CLR_BROWN)},	/* trap */
	{'^', "debt trap", C(CLR_RED)},	/* trap */
	{'^', "inversion trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "wince trap", C(CLR_RED)},	/* trap */
	{'^', "u have been trap", C(CLR_BLUE)},	/* trap */
	{'^', "really bad trap", C(CLR_RED)},	/* trap */
	{'^', "covid trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "artiblast trap", C(CLR_MAGENTA)},	/* trap */

	{'^', "persistent fart trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "attacking heel trap", C(CLR_GREEN)},	/* trap */
	{'^', "trap teleporter", C(CLR_MAGENTA)},	/* trap */
	{'^', "alignment trash trap", C(CLR_BLACK)},	/* trap */
	{'^', "reshuffle trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "musehand trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "dogside trap", C(CLR_BLUE)},	/* trap */
	{'^', "bankrupt trap", C(CLR_BLACK)},	/* trap */
	{'^', "fillup trap", C(CLR_GRAY)},	/* trap */
	{'^', "airstrike trap", C(CLR_ORANGE)},	/* trap */
	{'^', "dynamite trap", C(CLR_ORANGE)},	/* trap */
	{'^', "malevolence trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "leaflet trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "tentadeep trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "stathalf trap", C(CLR_WHITE)},	/* trap */
	{'^', "cutstat trap", C(CLR_YELLOW)},	/* trap */
	{'^', "rare spawn trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "you are an idiot trap", C(CLR_WHITE)},	/* trap */
	{'^', "nastycurse trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "repeating nastycurse trap", C(CLR_MAGENTA)},	/* trap */

	{'^', "nadine trap", C(CLR_BLACK)},	/* trap */
	{'^', "luisa trap", C(CLR_ORANGE)},	/* trap */
	{'^', "irina trap", C(CLR_GRAY)},	/* trap */
	{'^', "liselotte trap", C(CLR_BLUE)},	/* trap */
	{'^', "greta trap", C(CLR_BLUE)},	/* trap */
	{'^', "jane trap", C(CLR_BROWN)},	/* trap */
	{'^', "sue lyn trap", C(CLR_WHITE)},	/* trap */
	{'^', "charlotte trap", C(CLR_BROWN)},	/* trap */
	{'^', "hannah trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "little marie trap", C(CLR_YELLOW)},	/* trap */

	{'^', "real lie trap", C(CLR_ORANGE)},	/* trap */
	{'^', "escape past trap", C(CLR_BLACK)},	/* trap */
	{'^', "pethate trap", C(CLR_BLACK)},	/* trap */
	{'^', "pet lashout trap", C(CLR_RED)},	/* trap */
	{'^', "petstarve trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "petscrew trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "tech loss trap", C(CLR_BROWN)},	/* trap */
	{'^', "proofloss trap", C(CLR_CYAN)},	/* trap */
	{'^', "un-invis trap", C(CLR_CYAN)},	/* trap */
	{'^', "detectation trap", C(CLR_WHITE)},	/* trap */

	{'^', "option trap", C(CLR_WHITE)},	/* trap */
	{'^', "miscolor trap", C(CLR_BLUE)},	/* trap */
	{'^', "one rainbow trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "colorshift trap", C(CLR_ORANGE)},	/* trap */
	{'^', "top line trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "caps trap", C(CLR_RED)},	/* trap */
	{'^', "un-knowledge trap", C(CLR_BLUE)},	/* trap */
	{'^', "darkhance trap", C(CLR_BLACK)},	/* trap */
	{'^', "dschueueuet trap", C(CLR_BROWN)},	/* trap */
	{'^', "nopeskill trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "fuckfuckfuck trap", C(CLR_BRIGHT_MAGENTA)},	/* trap */

	{'^', "burden trap", C(CLR_GRAY)},	/* trap */
	{'^', "magic vacuum trap", C(CLR_BLUE)},	/* trap */

	{'^', "tanja trap", C(CLR_GRAY)},	/* trap */
	{'^', "sonja trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "rhea trap", C(CLR_BLACK)},	/* trap */
	{'^', "lara trap", C(CLR_BLACK)},	/* trap */

	{'^', "fuck over trap", C(CLR_YELLOW)},	/* trap */

	{'^', "dagger trap", C(CLR_CYAN)},	/* trap */
	{'^', "razor trap", C(CLR_RED)},	/* trap */
	{'^', "phosgene trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "chloroform trap", C(CLR_CYAN)},	/* trap */
	{'^', "corrosion trap", C(CLR_GREEN)},	/* trap */
	{'^', "flame trap", C(CLR_ORANGE)},	/* trap */
	{'^', "wither trap", C(CLR_WHITE)},	/* trap */
	{'^', "phaseporter", C(CLR_MAGENTA)},	/* trap */
	{'^', "phase beamer", C(CLR_MAGENTA)},	/* trap */
	{'^', "vulnerate trap", C(CLR_YELLOW)},	/* trap */

	{'^', "epvi trap", C(CLR_RED)},	/* trap */

	{'^', "sabrina trap", C(CLR_BLUE)},	/* trap */

	{'^', "we select now the aefde trap", C(CLR_CYAN)},	/* trap */

	{'^', "falling rock cold", C(CLR_CYAN)},	/* trap */
	{'^', "return trap", C(CLR_YELLOW)},	/* trap */
	{'^', "intrinsic steal trap", C(CLR_BRIGHT_GREEN)},	/* trap */
	{'^', "score axe trap", C(CLR_GRAY)},	/* trap */
	{'^', "score drain trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "single unidentify trap", C(CLR_BLACK)},	/* trap */
	{'^', "hypoxic pit", C(CLR_BLACK)},	/* trap */
	{'^', "unlucky trap", C(CLR_YELLOW)},	/* trap */
	{'^', "alignment reduction trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "malignant trap", C(CLR_BLACK)},	/* trap */
	{'^', "stat damage trap", C(CLR_RED)},	/* trap */
	{'^', "half memory trap", C(CLR_GRAY)},	/* trap */
	{'^', "half training trap", C(CLR_BROWN)},	/* trap */
	{'^', "debuff trap", C(CLR_CYAN)},	/* trap */
	{'^', "trip once trap", C(CLR_GREEN)},	/* trap */
	{'^', "narcolepsy trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "martial arts trap", C(CLR_WHITE)},	/* trap */
	{'^', "mean burden trap", C(CLR_ORANGE)},	/* trap */
	{'^', "carrcap trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'^', "umeng trap", C(CLR_MAGENTA)},	/* trap */

	{'^', "spell cooldown trap", C(CLR_BLUE)},	/* trap */
	{'^', "turbodull trap", C(CLR_GRAY)},	/* trap */
	{'^', "reddam trap", C(CLR_ORANGE)},	/* trap */
	{'^', "redinc trap", C(CLR_ORANGE)},	/* trap */
	{'^', "mulch trap", C(CLR_GRAY)},	/* trap */
	{'^', "inaccuracy trap", C(CLR_YELLOW)},	/* trap */
	{'^', "monster multishot trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "do you have a pin trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "costly failure trap", C(CLR_RED)},	/* trap */
	{'^', "inventory size trap", C(CLR_YELLOW)},	/* trap */

	{'^', "spike trap", C(CLR_WHITE)},	/* trap */
	{'^', "paper arrow trap", C(CLR_WHITE)},	/* trap */
	{'^', "knife trap", C(CLR_CYAN)},	/* trap */
	{'^', "summon monster trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "red damage trap", C(CLR_RED)},	/* trap */
	{'^', "green damage trap", C(CLR_GREEN)},	/* trap */
	{'^', "yellow damage trap", C(CLR_YELLOW)},	/* trap */
	{'^', "white damage trap", C(CLR_WHITE)},	/* trap */
	{'^', "blue damage trap", C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "multi debuff trap", C(CLR_BRIGHT_CYAN)},	/* trap */

	{'^', "magic damage trap", C(CLR_BRIGHT_BLUE)},	/* trap */

	{'^', "wild weather trap", C(CLR_BRIGHT_BLUE)},	/* trap */

	{'^', "timerun trap", C(CLR_BRIGHT_CYAN)},	/* trap */
	{'|', "wall",		C(CLR_GRAY)},	/* vbeam */
	{'-', "wall",		C(CLR_GRAY)},	/* hbeam */
	{'\\',"wall",		C(CLR_GRAY)},	/* lslant */
	{'/', "wall",		C(CLR_GRAY)},	/* rslant */
	{'*', "",		C(CLR_WHITE)},	/* dig beam */
	{'!', "",		C(CLR_WHITE)},	/* camera flash beam */
/*70*/	{')', "",		C(HI_WOOD)},	/* boomerang open left */
	{'(', "",		C(HI_WOOD)},	/* boomerang open right */
	{'0', "",		C(HI_ZAP)},	/* 4 magic shield symbols */
	{'#', "",		C(HI_ZAP)},
	{'@', "",		C(HI_ZAP)},
	{'*', "",		C(HI_ZAP)},
	{'/', "",		C(CLR_GREEN)},	/* swallow top left	*/
	{'-', "",		C(CLR_GREEN)},	/* swallow top center	*/
	{'\\', "",		C(CLR_GREEN)},	/* swallow top right	*/
	{'|', "",		C(CLR_GREEN)},	/* swallow middle left	*/
/*80*/	{'|', "",		C(CLR_GREEN)},	/* swallow middle right	*/
	{'\\', "",		C(CLR_GREEN)},	/* swallow bottom left	*/
	{'-', "",		C(CLR_GREEN)},	/* swallow bottom center*/
	{'/', "",		C(CLR_GREEN)},	/* swallow bottom right	*/
	{'/', "",		C(CLR_ORANGE)},	/* explosion top left     */
	{'-', "",		C(CLR_ORANGE)},	/* explosion top center   */
	{'\\', "",		C(CLR_ORANGE)},	/* explosion top right    */
	{'|', "",		C(CLR_ORANGE)},	/* explosion middle left  */
	{' ', "",		C(CLR_ORANGE)},	/* explosion middle center*/
	{'|', "",		C(CLR_ORANGE)},	/* explosion middle right */
/*90*/	{'\\', "",		C(CLR_ORANGE)},	/* explosion bottom left  */
	{'-', "",		C(CLR_ORANGE)},	/* explosion bottom center*/
	{'/', "",		C(CLR_ORANGE)},	/* explosion bottom right */

/*
 *  Note: Additions to this array should be reflected in the
 *	  {ibm,dec,mac}_graphics[] arrays below.
 */
};

#undef C

#ifdef ASCIIGRAPH

#ifdef PC9800
void (*ibmgraphics_mode_callback)) = 0;	/* set in tty_start_screen((void) */
#endif /* PC9800 */

#ifdef CURSES_GRAPHICS
void (*cursesgraphics_mode_callback)(void) = 0;
#endif

static uchar ibm_graphics[MAXPCHARS] = {
/* 0*/	g_FILLER(S_stone),
	0xb3,	/* S_vwall:	meta-3, vertical rule */
	0xc4,	/* S_hwall:	meta-D, horizontal rule */
	0xda,	/* S_tlcorn:	meta-Z, top left corner */
	0xbf,	/* S_trcorn:	meta-?, top right corner */
	0xc0,	/* S_blcorn:	meta-@, bottom left */
	0xd9,	/* S_brcorn:	meta-Y, bottom right */
	0xc5,	/* S_crwall:	meta-E, cross */
	0xc1,	/* S_tuwall:	meta-A, T up */
	0xc2,	/* S_tdwall:	meta-B, T down */
/*10*/	0xb4,	/* S_tlwall:	meta-4, T left */
	0xc3,	/* S_trwall:	meta-C, T right */
	g_FILLER(S_rockwall),	/* rockwall */
	g_FILLER(S_gravewall),	/* gravewall */
	g_FILLER(S_tunnelwall),	/* tunnelwall */
	0xfa,	/* S_ndoor:	meta-z, centered dot */
	0xfe,	/* S_vodoor:	meta-~, small centered square */
	0xfe,	/* S_hodoor:	meta-~, small centered square */
	g_FILLER(S_vcdoor),
	g_FILLER(S_hcdoor),
/*20*/	g_FILLER(S_bars),	/* S_bars:	equivalency, courtesy Sascha; but Amy says it just shows as "-" on some systems */
	241,	/* S_tree:	plus or minus symbol */
	241,	/* S_farmland:	plus or minus symbol */
	g_FILLER(S_mountain),	/* mountain */
	0xfa,	/* S_room:	meta-z, centered dot */
	g_FILLER(S_stone),	/* S_darkroom:	meta-z, centered dot */
	0xb0,	/* S_corr:	meta-0, light shading */
	0xb1,	/* S_litcorr:	meta-1, medium shading */
	g_FILLER(S_upstair),
	g_FILLER(S_dnstair),
/*30*/	g_FILLER(S_upladder),
	g_FILLER(S_dnladder),
	g_FILLER(S_altar),
	g_FILLER(S_grave),
	g_FILLER(S_throne),
	0xf4, /*(S_sink)*/
	0xf4, /*(S_toilet),*/
	0xf4,	/* S_fountain:	meta-t, integral top half */
	0xf7,	/* S_pool:	meta-w, approx. equals */
	g_FILLER(S_ice),	/* S_ice:	meta-z, centered dot */
/*40*/	0xf7,	/* S_lava:	meta-w, approx. equals */
	g_FILLER(S_vodbridge),	/* S_vodbridge:	meta-z, centered dot */
	g_FILLER(S_hodbridge),	/* S_hodbridge:	meta-z, centered dot */
	g_FILLER(S_vcdbridge),
	g_FILLER(S_hcdbridge),
	g_FILLER(S_air),
	g_FILLER(S_cloud),
	g_FILLER(S_dungwall),	/* dungwall */
	0xf7,	/* S_water:	meta-w, approx. equals */
	g_FILLER(S_watertunnel),	/* watertunnel */
/*50*/	0xf7,	/* S_crystalwater:	meta-w, approx. equals */
	0xf7, /* moorland */
	0xf7, /* urinelake */
	0xf7, /* shiftingsand */
	0xf7, /* styxriver */
	0xf4, /* well */
	0xf4, /* poisonedwell */
	g_FILLER(S_wagon),
	g_FILLER(S_burningwagon),
	g_FILLER(S_woodentable),
/*60*/	g_FILLER(S_strawmattress),
	g_FILLER(S_snow), /* S_snow */
	g_FILLER(S_ash), /* S_ash */
	g_FILLER(S_sand), /* S_sand */
	g_FILLER(S_pavedfloor), /* S_pavedfloor */
	g_FILLER(S_highway), /* S_highway */
	g_FILLER(S_grass), /* S_grass */
	g_FILLER(S_nethermist), /* S_nethermist */
	g_FILLER(S_stalactite),
	g_FILLER(S_cryptfloor), /* S_cryptfloor */
/*70*/	g_FILLER(S_bubbles),
	g_FILLER(S_raincloud),
	g_FILLER(S_pentagram),
	g_FILLER(S_carvedbed),
	g_FILLER(S_grayglyph),
	g_FILLER(S_arrow_trap),
	g_FILLER(S_dart_trap),
	g_FILLER(S_falling_rock_trap),
	g_FILLER(S_squeaky_board),
	g_FILLER(S_bear_trap),
	g_FILLER(S_land_mine),
	g_FILLER(S_rolling_boulder_trap),
	g_FILLER(S_sleeping_gas_trap),
	g_FILLER(S_rust_trap),
	g_FILLER(S_fire_trap),
	g_FILLER(S_pit),
	g_FILLER(S_spiked_pit),
	g_FILLER(S_hole),
	g_FILLER(S_trap_door),
	g_FILLER(S_teleportation_trap),
	g_FILLER(S_level_teleporter),
	g_FILLER(S_magic_portal),
	g_FILLER(S_web),
	g_FILLER(S_statue_trap),
	g_FILLER(S_magic_trap),
	g_FILLER(S_anti_magic_trap),
	g_FILLER(S_polymorph_trap),
	g_FILLER(S_ice_trap),
	g_FILLER(S_spear_trap),
	g_FILLER(S_falling_rocks_trap),
	g_FILLER(S_magic_beam_trap),
	g_FILLER(S_pile_of_shit),
	g_FILLER(S_animation_trap),
	g_FILLER(S_glyph_of_warding),
	g_FILLER(S_scything_blade),
	g_FILLER(S_bolt_trap),
	g_FILLER(S_acid_pool),
	g_FILLER(S_water_pool),
	g_FILLER(S_poison_gas_trap),
	g_FILLER(S_slow_gas_trap),
	g_FILLER(S_shock_trap),
	g_FILLER(S_rmb_loss_trap),
	g_FILLER(S_display_trap),
	g_FILLER(S_spell_loss_trap),
	g_FILLER(S_yellow_spell_trap),
	g_FILLER(S_auto_destruct_trap),
	g_FILLER(S_memory_trap),
	g_FILLER(S_inventory_trap),
	g_FILLER(S_black_ng_wall_trap),
	g_FILLER(S_menu_trap),
	g_FILLER(S_speed_trap),
	g_FILLER(S_superscroller_trap),
	g_FILLER(S_active_superscroller_trap),
	g_FILLER(S_swarm_trap),
	g_FILLER(S_automatic_switcher),
	g_FILLER(S_unknown_trap),
	g_FILLER(S_heel_trap),
	g_FILLER(S_vuln_trap),

	g_FILLER(S_lava_trap),
	g_FILLER(S_flood_trap),
	g_FILLER(S_drain_trap),
	g_FILLER(S_free_hand_trap),
	g_FILLER(S_disintegration_trap),
	g_FILLER(S_unidentify_trap),
	g_FILLER(S_thirst_trap),
	g_FILLER(S_luck_trap),
	g_FILLER(S_shades_of_grey_trap),
	g_FILLER(S_item_telep_trap),
	g_FILLER(S_gravity_trap),
	g_FILLER(S_death_trap),
	g_FILLER(S_stone_to_flesh_trap),
	g_FILLER(S_quicksand_trap),
	g_FILLER(S_faint_trap),
	g_FILLER(S_curse_trap),
	g_FILLER(S_difficulty_trap),
	g_FILLER(S_sound_trap),
	g_FILLER(S_caster_trap),
	g_FILLER(S_weakness_trap),
	g_FILLER(S_rot_thirteen_trap),
	g_FILLER(S_bishop_trap),
	g_FILLER(S_confusion_trap),
	g_FILLER(S_nupesell_trap),
	g_FILLER(S_drop_trap),
	g_FILLER(S_dstw_trap),
	g_FILLER(S_status_trap),
	g_FILLER(S_alignment_trap),
	g_FILLER(S_stairs_trap),

	g_FILLER(S_pestilence_trap),
	g_FILLER(S_famine_trap),

	g_FILLER(S_relocation_trap),

	g_FILLER(S_uninformation_trap),

	g_FILLER(S_intrinsic_loss_trap),
	g_FILLER(S_blood_loss_trap),
	g_FILLER(S_bad_effect_trap),
	g_FILLER(S_multiplying_trap),
	g_FILLER(S_auto_vuln_trap),
	g_FILLER(S_tele_items_trap),
	g_FILLER(S_nastiness_trap),

	g_FILLER(S_recursion_trap),
	g_FILLER(S_respawn_trap),
	g_FILLER(S_warp_zone),
	g_FILLER(S_captcha_trap),
	g_FILLER(S_mind_wipe_trap),
	g_FILLER(S_shuriken_trap),
	g_FILLER(S_shit_pit),
	g_FILLER(S_shaft_trap),
	g_FILLER(S_lock_trap),
	g_FILLER(S_mc_trap),
	g_FILLER(S_farlook_trap),
	g_FILLER(S_gateway_trap),
	g_FILLER(S_growing_trap),
	g_FILLER(S_cooling_trap),
	g_FILLER(S_bar_trap),
	g_FILLER(S_locking_trap),
	g_FILLER(S_air_trap),
	g_FILLER(S_terrain_trap),

	g_FILLER(S_loudspeaker),
	g_FILLER(S_nest_trap),
	g_FILLER(S_bullet_trap),
	g_FILLER(S_paralysis_trap),
	g_FILLER(S_cyanide_trap),
	g_FILLER(S_laser_trap),
	g_FILLER(S_giant_chasm),
	g_FILLER(S_fart_trap),
	g_FILLER(S_confuse_trap),
	g_FILLER(S_stun_trap),
	g_FILLER(S_hallucination_trap),
	g_FILLER(S_petrification_trap),
	g_FILLER(S_numbness_trap),
	g_FILLER(S_freezing_trap),
	g_FILLER(S_burning_trap),
	g_FILLER(S_fear_trap),
	g_FILLER(S_blindness_trap),
	g_FILLER(S_glib_trap),
	g_FILLER(S_slime_trap),
	g_FILLER(S_inertia_trap),
	g_FILLER(S_time_trap),
	g_FILLER(S_lycanthropy_trap),
	g_FILLER(S_unlight_trap),
	g_FILLER(S_elemental_trap),
	g_FILLER(S_escalating_trap),
	g_FILLER(S_negative_trap),
	g_FILLER(S_mana_trap),
	g_FILLER(S_sin_trap),
	g_FILLER(S_destroy_armor_trap),
	g_FILLER(S_divine_anger_trap),
	g_FILLER(S_genetic_trap),
	g_FILLER(S_missingno_trap),
	g_FILLER(S_cancellation_trap),
	g_FILLER(S_hostility_trap),
	g_FILLER(S_boss_trap),
	g_FILLER(S_wishing_trap),
	g_FILLER(S_recurring_amn_trap),
	g_FILLER(S_bigscript_trap),
	g_FILLER(S_bank_trap),
	g_FILLER(S_only_trap),
	g_FILLER(S_map_trap),
	g_FILLER(S_tech_trap),
	g_FILLER(S_disenchant_trap),
	g_FILLER(S_verisiert),
	g_FILLER(S_chaos_trap),
	g_FILLER(S_muteness_trap),
	g_FILLER(S_ntll_trap),
	g_FILLER(S_engraving_trap),
	g_FILLER(S_magic_device_trap),
	g_FILLER(S_book_trap),
	g_FILLER(S_level_trap),
	g_FILLER(S_quiz_trap),

	g_FILLER(S_falling_boulder_trap),
	g_FILLER(S_glass_arrow_trap),
	g_FILLER(S_glass_bolt_trap),
	g_FILLER(S_out_of_magic_trap),
	g_FILLER(S_plasma_trap),
	g_FILLER(S_bomb_trap),
	g_FILLER(S_earthquake_trap),
	g_FILLER(S_noise_trap),
	g_FILLER(S_glue_trap),
	g_FILLER(S_guillotine_trap),
	g_FILLER(S_bisection_trap),
	g_FILLER(S_volt_trap),
	g_FILLER(S_horde_trap),
	g_FILLER(S_immobility_trap),
	g_FILLER(S_green_glyph_trap),
	g_FILLER(S_blue_glyph_trap),
	g_FILLER(S_yellow_glyph_trap),
	g_FILLER(S_orange_glyph_trap),
	g_FILLER(S_black_glyph_trap),
	g_FILLER(S_purple_glyph_trap),

	g_FILLER(S_metabolic_trap),
	g_FILLER(S_no_return_trap),
	g_FILLER(S_ego_trap),
	g_FILLER(S_fast_forward_trap),
	g_FILLER(S_rotten_trap),
	g_FILLER(S_unskilled_trap),
	g_FILLER(S_low_stats_trap),
	g_FILLER(S_training_trap),
	g_FILLER(S_exercise_trap),
	g_FILLER(S_falling_loadstone_trap),
	g_FILLER(S_summon_undead_trap),
	g_FILLER(S_falling_nastystone_trap),

	g_FILLER(S_spined_ball_trap),
	g_FILLER(S_pendulum_trap),
	g_FILLER(S_turn_table),
	g_FILLER(S_scent_trap),
	g_FILLER(S_banana_trap),
	g_FILLER(S_falling_tub_trap),
	g_FILLER(S_alarm),
	g_FILLER(S_caltrops_trap),
	g_FILLER(S_blade_wire),
	g_FILLER(S_magnet_trap),
	g_FILLER(S_slingshot_trap),
	g_FILLER(S_cannon_trap),
	g_FILLER(S_venom_sprinkler),
	g_FILLER(S_fumarole),

	g_FILLER(S_mana_pit),
	g_FILLER(S_elemental_portal),
	g_FILLER(S_girliness_trap),
	g_FILLER(S_fumbling_trap),
	g_FILLER(S_egomonster_trap),
	g_FILLER(S_flooding_trap),
	g_FILLER(S_monster_cube),
	g_FILLER(S_cursed_grave),
	g_FILLER(S_limitation_trap),
	g_FILLER(S_weak_sight_trap),
	g_FILLER(S_random_message_trap),

	g_FILLER(S_desecration_trap),
	g_FILLER(S_starvation_trap),
	g_FILLER(S_dropless_trap),
	g_FILLER(S_low_effect_trap),
	g_FILLER(S_invisible_trap),
	g_FILLER(S_ghost_world_trap),
	g_FILLER(S_dehydration_trap),
	g_FILLER(S_hate_trap),
	g_FILLER(S_spacewars_trap),
	g_FILLER(S_temporary_recursion_trap),
	g_FILLER(S_totter_trap),
	g_FILLER(S_nonintrinsical_trap),
	g_FILLER(S_dropcurse_trap),
	g_FILLER(S_nakedness_trap),
	g_FILLER(S_antilevel_trap),
	g_FILLER(S_ventilator),
	g_FILLER(S_stealer_trap),
	g_FILLER(S_rebellion_trap),
	g_FILLER(S_crap_trap),
	g_FILLER(S_misfire_trap),
	g_FILLER(S_trap_of_walls),

	g_FILLER(S_disconnect_trap),
	g_FILLER(S_interface_screw_trap),
	g_FILLER(S_dimness_trap),
	g_FILLER(S_evil_artifact_trap),
	g_FILLER(S_bossfight_trap),
	g_FILLER(S_entire_level_trap),
	g_FILLER(S_bones_trap),
	g_FILLER(S_rodney_trap),
	g_FILLER(S_autocurse_trap),
	g_FILLER(S_highlevel_trap),
	g_FILLER(S_spell_forgetting_trap),
	g_FILLER(S_sound_effect_trap),

	g_FILLER(S_trap_percents),

	g_FILLER(S_nexus_trap),
	g_FILLER(S_leg_trap),
	g_FILLER(S_artifact_jackpot_trap),
	g_FILLER(S_map_amnesia_trap),
	g_FILLER(S_spreading_trap),
	g_FILLER(S_adjacent_trap),
	g_FILLER(S_superthing_trap),
	g_FILLER(S_current_shaft),
	g_FILLER(S_levitation_trap),
	g_FILLER(S_bowel_cramps_trap),
	g_FILLER(S_unequipping_trap),
	g_FILLER(S_good_artifact_trap),
	g_FILLER(S_gender_trap),
	g_FILLER(S_trap_of_oa),
	g_FILLER(S_sincount_trap),
	g_FILLER(S_beamer_trap),
	g_FILLER(S_level_beamer),
	g_FILLER(S_pet_trap),
	g_FILLER(S_piercing_beam_trap),
	g_FILLER(S_wrenching_trap),
	g_FILLER(S_tracker_trap),
	g_FILLER(S_nurse_trap),
	g_FILLER(S_back_to_start_trap),
	g_FILLER(S_nemesis_trap),
	g_FILLER(S_strew_trap),
	g_FILLER(S_outta_depth_trap),
	g_FILLER(S_punishment_trap),
	g_FILLER(S_boon_trap),
	g_FILLER(S_fountain_trap),
	g_FILLER(S_throne_trap),
	g_FILLER(S_anoxic_pit),
	g_FILLER(S_arabella_speaker),
	g_FILLER(S_femmy_trap),
	g_FILLER(S_madeleine_trap),
	g_FILLER(S_marlena_trap),
	g_FILLER(S_anastasia_trap),
	g_FILLER(S_filler_trap),
	g_FILLER(S_toxic_venom_trap),
	g_FILLER(S_insanity_trap),
	g_FILLER(S_madness_trap),
	g_FILLER(S_jessica_trap),
	g_FILLER(S_solvejg_trap),
	g_FILLER(S_wendy_trap),
	g_FILLER(S_katharina_trap),
	g_FILLER(S_elena_trap),
	g_FILLER(S_thai_trap),
	g_FILLER(S_elif_trap),
	g_FILLER(S_nadja_trap),
	g_FILLER(S_sandra_trap),
	g_FILLER(S_natalje_trap),
	g_FILLER(S_jeanetta_trap),
	g_FILLER(S_yvonne_trap),
	g_FILLER(S_maurah_trap),

	g_FILLER(S_lootcut_trap),
	g_FILLER(S_monster_speed_trap),
	g_FILLER(S_scaling_trap),
	g_FILLER(S_enmity_trap),
	g_FILLER(S_white_spell_trap),
	g_FILLER(S_gray_spell_trap),
	g_FILLER(S_quasar_trap),
	g_FILLER(S_momma_trap),
	g_FILLER(S_horror_trap),
	g_FILLER(S_artificer_trap),
	g_FILLER(S_wereform_trap),
	g_FILLER(S_nonprayer_trap),
	g_FILLER(S_evil_patch_trap),
	g_FILLER(S_hard_mode_trap),
	g_FILLER(S_secret_attack_trap),
	g_FILLER(S_eater_trap),
	g_FILLER(S_covetous_trap),
	g_FILLER(S_not_seen_trap),
	g_FILLER(S_dark_mode_trap),
	g_FILLER(S_antisearch_trap),
	g_FILLER(S_homicide_trap),
	g_FILLER(S_nasty_nation_trap),
	g_FILLER(S_wakeup_call_trap),
	g_FILLER(S_grayout_trap),
	g_FILLER(S_gray_center_trap),
	g_FILLER(S_checkerboard_trap),
	g_FILLER(S_clockwise_spin_trap),
	g_FILLER(S_counterclockwise_trap),
	g_FILLER(S_lag_trap),
	g_FILLER(S_blesscurse_trap),
	g_FILLER(S_delight_trap),
	g_FILLER(S_discharge_trap),
	g_FILLER(S_trashing_trap),
	g_FILLER(S_filtering_trap),
	g_FILLER(S_deformatting_trap),
	g_FILLER(S_flicker_strip_trap),
	g_FILLER(S_undressing_trap),
	g_FILLER(S_hyperbluewall_trap),
	g_FILLER(S_nolite_trap),
	g_FILLER(S_paranoia_trap),
	g_FILLER(S_fleecescript_trap),
	g_FILLER(S_interrupt_trap),
	g_FILLER(S_dustbin_trap),
	g_FILLER(S_mana_battery_trap),
	g_FILLER(S_monsterfingers_trap),
	g_FILLER(S_miscast_trap),
	g_FILLER(S_message_suppression_trap),
	g_FILLER(S_stuck_announcement_trap),
	g_FILLER(S_bloodthirsty_trap),
	g_FILLER(S_maximum_damage_trap),
	g_FILLER(S_latency_trap),
	g_FILLER(S_starlit_trap),
	g_FILLER(S_knowledge_trap),
	g_FILLER(S_highscore_trap),
	g_FILLER(S_pink_spell_trap),
	g_FILLER(S_green_spell_trap),
	g_FILLER(S_evc_trap),
	g_FILLER(S_underlayer_trap),
	g_FILLER(S_damage_meter_trap),
	g_FILLER(S_arbitrary_weight_trap),
	g_FILLER(S_fucked_info_trap),
	g_FILLER(S_black_spell_trap),
	g_FILLER(S_cyan_spell_trap),
	g_FILLER(S_heap_trap),
	g_FILLER(S_blue_spell_trap),
	g_FILLER(S_tron_trap),
	g_FILLER(S_red_spell_trap),
	g_FILLER(S_too_heavy_trap),
	g_FILLER(S_elongation_trap),
	g_FILLER(S_wrapover_trap),
	g_FILLER(S_destruction_trap),
	g_FILLER(S_melee_prefix_trap),
	g_FILLER(S_automore_trap),
	g_FILLER(S_unfair_attack_trap),

	g_FILLER(S_kop_cube),
	g_FILLER(S_boss_spawner),
	g_FILLER(S_contamination_trap),

	g_FILLER(S_evil_heel_trap),
	g_FILLER(S_bad_equipment_trap),
	g_FILLER(S_tempoconflict_trap),
	g_FILLER(S_tempohunger_trap),
	g_FILLER(S_teleportitis_trap),
	g_FILLER(S_polymorphitis_trap),
	g_FILLER(S_premature_death_trap),
	g_FILLER(S_lasting_amnesia_trap),
	g_FILLER(S_ragnarok_trap),
	g_FILLER(S_single_disenchant_trap),
	g_FILLER(S_acid_pit),

	g_FILLER(S_severe_disenchant_trap),
	g_FILLER(S_pain_trap),
	g_FILLER(S_trembling_trap),
	g_FILLER(S_techcap_trap),
	g_FILLER(S_spell_memory_trap),
	g_FILLER(S_skill_reduction_trap),
	g_FILLER(S_skillcap_trap),
	g_FILLER(S_permanent_stat_damage_trap),

	g_FILLER(S_orange_spell_trap),
	g_FILLER(S_violet_spell_trap),
	g_FILLER(S_trap_of_longing),
	g_FILLER(S_cursed_part_trap),
	g_FILLER(S_quaversal_trap),
	g_FILLER(S_appearance_shuffling_trap),
	g_FILLER(S_brown_spell_trap),
	g_FILLER(S_choiceless_trap),
	g_FILLER(S_goldspell_trap),
	g_FILLER(S_deprovement_trap),
	g_FILLER(S_initialization_trap),
	g_FILLER(S_gushlush_trap),
	g_FILLER(S_soiltype_trap),
	g_FILLER(S_dangerous_terrain_trap),
	g_FILLER(S_fallout_trap),
	g_FILLER(S_mojibake_trap),
	g_FILLER(S_gravation_trap),
	g_FILLER(S_uncalled_trap),
	g_FILLER(S_exploding_dice_trap),
	g_FILLER(S_permacurse_trap),
	g_FILLER(S_shrouded_identity_trap),
	g_FILLER(S_feeler_gauges_trap),
	g_FILLER(S_long_screwup_trap),
	g_FILLER(S_wing_yellow_changer),
	g_FILLER(S_life_saving_trap),
	g_FILLER(S_curseuse_trap),
	g_FILLER(S_cut_nutrition_trap),
	g_FILLER(S_skill_loss_trap),
	g_FILLER(S_autopilot_trap),
	g_FILLER(S_force_trap),
	g_FILLER(S_monster_glyph_trap),
	g_FILLER(S_changing_directive_trap),
	g_FILLER(S_container_kaboom_trap),
	g_FILLER(S_steal_degrade_trap),
	g_FILLER(S_left_inventory_trap),
	g_FILLER(S_fluctuating_speed_trap),
	g_FILLER(S_tarmustrokingnora_trap),
	g_FILLER(S_failure_trap),
	g_FILLER(S_bright_cyan_spell_trap),
	g_FILLER(S_frequentation_spawn_trap),
	g_FILLER(S_pet_ai_trap),
	g_FILLER(S_satan_trap),
	g_FILLER(S_rememberance_trap),
	g_FILLER(S_pokelie_trap),
	g_FILLER(S_autopickup_trap),
	g_FILLER(S_dywypi_trap),
	g_FILLER(S_silver_spell_trap),
	g_FILLER(S_metal_spell_trap),
	g_FILLER(S_platinum_spell_trap),
	g_FILLER(S_manler_trap),
	g_FILLER(S_doorning_trap),
	g_FILLER(S_nownsible_trap),
	g_FILLER(S_elm_street_trap),
	g_FILLER(S_monnoise_trap),
	g_FILLER(S_rang_call_trap),
	g_FILLER(S_recurring_spell_loss_trap),
	g_FILLER(S_antitraining_trap),
	g_FILLER(S_techout_trap),
	g_FILLER(S_stat_decay_trap),
	g_FILLER(S_movemork_trap),

	g_FILLER(S_hybrid_trap),
	g_FILLER(S_shapechange_trap),
	g_FILLER(S_meltem_trap),
	g_FILLER(S_miguc_trap),
	g_FILLER(S_directive_trap),
	g_FILLER(S_satatue_trap),
	g_FILLER(S_farting_web),
	g_FILLER(S_cataclysm_trap),
	g_FILLER(S_data_delete_trap),
	g_FILLER(S_elder_tentacling_trap),
	g_FILLER(S_footerer_trap),

	g_FILLER(S_bad_part_trap),
	g_FILLER(S_completely_bad_part_trap),
	g_FILLER(S_evil_variant_trap),

	g_FILLER(S_grave_wall_trap),
	g_FILLER(S_tunnel_trap),
	g_FILLER(S_farmland_trap),
	g_FILLER(S_mountain_trap),
	g_FILLER(S_water_tunnel_trap),
	g_FILLER(S_crystal_flood_trap),
	g_FILLER(S_moorland_trap),
	g_FILLER(S_urine_trap),
	g_FILLER(S_shifting_sand_trap),
	g_FILLER(S_styx_trap),
	g_FILLER(S_pentagram_trap),
	g_FILLER(S_snow_trap),
	g_FILLER(S_ash_trap),
	g_FILLER(S_sand_trap),
	g_FILLER(S_pavement_trap),
	g_FILLER(S_highway_trap),
	g_FILLER(S_grassland_trap),
	g_FILLER(S_nether_mist_trap),
	g_FILLER(S_stalactite_trap),
	g_FILLER(S_cryptfloor_trap),
	g_FILLER(S_bubble_trap),
	g_FILLER(S_rain_cloud_trap),
	g_FILLER(S_item_nastification_trap),
	g_FILLER(S_sanity_increase_trap),
	g_FILLER(S_psi_trap),
	g_FILLER(S_gay_trap),
	g_FILLER(S_sarah_trap),
	g_FILLER(S_claudia_trap),
	g_FILLER(S_ludgera_trap),
	g_FILLER(S_kati_trap),
	g_FILLER(S_sanity_treble_trap),
	g_FILLER(S_stat_decrease_trap),
	g_FILLER(S_simeout_trap),
	g_FILLER(S_spressing_trap),
	g_FILLER(S_nelly_trap),
	g_FILLER(S_eveline_trap),
	g_FILLER(S_karin_trap),
	g_FILLER(S_juen_trap),
	g_FILLER(S_kristina_trap),
	g_FILLER(S_lou_trap),
	g_FILLER(S_almut_trap),
	g_FILLER(S_julietta_trap),
	g_FILLER(S_arabella_trap),
	g_FILLER(S_kristin_trap),
	g_FILLER(S_anna_trap),
	g_FILLER(S_ruea_trap),
	g_FILLER(S_dora_trap),
	g_FILLER(S_marike_trap),
	g_FILLER(S_jette_trap),
	g_FILLER(S_ina_trap),
	g_FILLER(S_sing_trap),
	g_FILLER(S_victoria_trap),
	g_FILLER(S_melissa_trap),
	g_FILLER(S_anita_trap),
	g_FILLER(S_henrietta_trap),
	g_FILLER(S_verena_trap),
	g_FILLER(S_annemarie_trap),
	g_FILLER(S_jil_trap),
	g_FILLER(S_jana_trap),
	g_FILLER(S_katrin_trap),
	g_FILLER(S_gudrun_trap),
	g_FILLER(S_ella_trap),
	g_FILLER(S_manuela_trap),
	g_FILLER(S_jennifer_trap),
	g_FILLER(S_patricia_trap),
	g_FILLER(S_antje_trap),
	g_FILLER(S_antje_x_trap),
	g_FILLER(S_kerstin_trap),
	g_FILLER(S_laura_trap),
	g_FILLER(S_larissa_trap),
	g_FILLER(S_nora_trap),
	g_FILLER(S_natalia_trap),
	g_FILLER(S_susanne_trap),
	g_FILLER(S_lisa_trap),
	g_FILLER(S_bridghitte_trap),
	g_FILLER(S_julia_trap),
	g_FILLER(S_nicole_trap),
	g_FILLER(S_rita_trap),
	g_FILLER(S_janina_trap),
	g_FILLER(S_rosa_trap),
	g_FILLER(S_ksenia_trap),
	g_FILLER(S_lydia_trap),
	g_FILLER(S_conny_trap),
	g_FILLER(S_katia_trap),
	g_FILLER(S_mariya_trap),
	g_FILLER(S_elise_trap),
	g_FILLER(S_ronja_trap),
	g_FILLER(S_ariane_trap),
	g_FILLER(S_johanna_trap),
	g_FILLER(S_inge_trap),

	g_FILLER(S_wall_trap),
	g_FILLER(S_monster_generator),
	g_FILLER(S_potion_dispenser),
	g_FILLER(S_spacewars_spawn_trap),
	g_FILLER(S_tv_tropes_trap),
	g_FILLER(S_symbiote_trap),
	g_FILLER(S_kill_symbiote_trap),
	g_FILLER(S_symbiote_replacement_trap),
	g_FILLER(S_shutdown_trap),
	g_FILLER(S_corona_trap),
	g_FILLER(S_unproofing_trap),
	g_FILLER(S_visibility_trap),
	g_FILLER(S_feminism_stone_trap),
	g_FILLER(S_branch_teleporter),
	g_FILLER(S_branch_beamer),
	g_FILLER(S_poison_arrow_trap),
	g_FILLER(S_poison_bolt_trap),
	g_FILLER(S_mace_trap),
	g_FILLER(S_shueft_trap),
	g_FILLER(S_moth_larvae_trap),
	g_FILLER(S_worthiness_trap),
	g_FILLER(S_conduct_trap),
	g_FILLER(S_strikethrough_trap),
	g_FILLER(S_multiple_gather_trap),
	g_FILLER(S_vivisection_trap),
	g_FILLER(S_instafeminism_trap),
	g_FILLER(S_instanasty_trap),
	g_FILLER(S_skill_point_loss_trap),
	g_FILLER(S_perfect_match_trap),
	g_FILLER(S_dumbie_lightsaber_trap),
	g_FILLER(S_wrong_stairs),
	g_FILLER(S_techstop_trap),
	g_FILLER(S_amnesia_switch_trap),
	g_FILLER(S_skill_swap_trap),
	g_FILLER(S_skill_upordown_trap),
	g_FILLER(S_skill_randomize_trap),

	g_FILLER(S_skill_multiply_trap),
	g_FILLER(S_trapwalk_trap),
	g_FILLER(S_cluster_trap),
	g_FILLER(S_field_trap),
	g_FILLER(S_monicide_trap),
	g_FILLER(S_trap_creation_trap),
	g_FILLER(S_leold_trap),
	g_FILLER(S_animeband_trap),
	g_FILLER(S_perfume_trap),
	g_FILLER(S_court_trap),
	g_FILLER(S_elder_scrolls_trap),
	g_FILLER(S_joke_trap),
	g_FILLER(S_dungeon_lords_trap),
	g_FILLER(S_fortytwo_trap),
	g_FILLER(S_randomize_trap),
	g_FILLER(S_evilroom_trap),
	g_FILLER(S_aoe_trap),
	g_FILLER(S_elona_trap),
	g_FILLER(S_religion_trap),
	g_FILLER(S_steamband_trap),
	g_FILLER(S_hardcore_trap),
	g_FILLER(S_machine_trap),
	g_FILLER(S_bee_trap),
	g_FILLER(S_migo_trap),
	g_FILLER(S_angband_trap),
	g_FILLER(S_dnethack_trap),
	g_FILLER(S_evil_spawn_trap),
	g_FILLER(S_shoe_trap),
	g_FILLER(S_inside_trap),
	g_FILLER(S_doom_trap),
	g_FILLER(S_military_trap),
	g_FILLER(S_illusion_trap),
	g_FILLER(S_diablo_trap),

	g_FILLER(S_giant_explorer_trap),
	g_FILLER(S_trapwarp_trap),
	g_FILLER(S_yawm_trap),
	g_FILLER(S_cradle_of_chaos_trap),
	g_FILLER(S_tez_trap),
	g_FILLER(S_enthu_trap),
	g_FILLER(S_mikra_trap),
	g_FILLER(S_gots_too_good_trap),
	g_FILLER(S_killer_room_trap),
	g_FILLER(S_no_fun_walls_trap),

	g_FILLER(S_ruth_trap),
	g_FILLER(S_magdalena_trap),
	g_FILLER(S_marleen_trap),
	g_FILLER(S_klara_trap),
	g_FILLER(S_friederike_trap),
	g_FILLER(S_naomi_trap),
	g_FILLER(S_ute_trap),
	g_FILLER(S_jasieen_trap),
	g_FILLER(S_yasaman_trap),
	g_FILLER(S_may_britt_trap),

	g_FILLER(S_calling_out_trap),
	g_FILLER(S_field_break_trap),
	g_FILLER(S_tenth_trap),
	g_FILLER(S_debt_trap),
	g_FILLER(S_inversion_trap),
	g_FILLER(S_wince_trap),
	g_FILLER(S_u_have_been_trap),
	g_FILLER(S_really_bad_trap),
	g_FILLER(S_covid_trap),
	g_FILLER(S_artiblast_trap),

	g_FILLER(S_persistent_fart_trap),
	g_FILLER(S_attacking_heel_trap),
	g_FILLER(S_trap_teleporter),
	g_FILLER(S_alignment_trash_trap),
	g_FILLER(S_reshuffle_trap),
	g_FILLER(S_musehand_trap),
	g_FILLER(S_dogside_trap),
	g_FILLER(S_bankrupt_trap),
	g_FILLER(S_fillup_trap),
	g_FILLER(S_airstrike_trap),
	g_FILLER(S_dynamite_trap),
	g_FILLER(S_malevolence_trap),
	g_FILLER(S_leaflet_trap),
	g_FILLER(S_tentadeep_trap),
	g_FILLER(S_stathalf_trap),
	g_FILLER(S_cutstat_trap),
	g_FILLER(S_rare_spawn_trap),
	g_FILLER(S_you_are_an_idiot_trap),
	g_FILLER(S_nastycurse_trap),
	g_FILLER(S_repeating_nastycurse_trap),

	g_FILLER(S_nadine_trap),
	g_FILLER(S_luisa_trap),
	g_FILLER(S_irina_trap),
	g_FILLER(S_liselotte_trap),
	g_FILLER(S_greta_trap),
	g_FILLER(S_jane_trap),
	g_FILLER(S_sue_lyn_trap),
	g_FILLER(S_charlotte_trap),
	g_FILLER(S_hannah_trap),
	g_FILLER(S_little_marie_trap),

	g_FILLER(S_real_lie_trap),
	g_FILLER(S_escape_past_trap),
	g_FILLER(S_pethate_trap),
	g_FILLER(S_pet_lashout_trap),
	g_FILLER(S_petstarve_trap),
	g_FILLER(S_petscrew_trap),
	g_FILLER(S_tech_loss_trap),
	g_FILLER(S_proofloss_trap),
	g_FILLER(S_un_invis_trap),
	g_FILLER(S_detectation_trap),

	g_FILLER(S_option_trap),
	g_FILLER(S_miscolor_trap),
	g_FILLER(S_one_rainbow_trap),
	g_FILLER(S_colorshift_trap),
	g_FILLER(S_top_line_trap),
	g_FILLER(S_caps_trap),
	g_FILLER(S_un_knowledge_trap),
	g_FILLER(S_darkhance_trap),
	g_FILLER(S_dschueueuet_trap),
	g_FILLER(S_nopeskill_trap),
	g_FILLER(S_fuckfuckfuck_trap),

	g_FILLER(S_burden_trap),
	g_FILLER(S_magic_vacuum_trap),

	g_FILLER(S_tanja_trap),
	g_FILLER(S_sonja_trap),
	g_FILLER(S_rhea_trap),
	g_FILLER(S_lara_trap),

	g_FILLER(S_fuck_over_trap),

	g_FILLER(S_dagger_trap),
	g_FILLER(S_razor_trap),
	g_FILLER(S_phosgene_trap),
	g_FILLER(S_chloroform_trap),
	g_FILLER(S_corrosion_trap),
	g_FILLER(S_flame_trap),
	g_FILLER(S_wither_trap),
	g_FILLER(S_phaseport_trap),
	g_FILLER(S_phasebeam_trap),
	g_FILLER(S_vulnerate_trap),

	g_FILLER(S_epvi_trap),

	g_FILLER(S_sabrina_trap),

	g_FILLER(S_aefde_trap),

	g_FILLER(S_falling_rock_cold_trap),
	g_FILLER(S_return_trap),
	g_FILLER(S_intrinsic_steal_trap),
	g_FILLER(S_score_axe_trap),
	g_FILLER(S_score_drain_trap),
	g_FILLER(S_single_unidentify_trap),
	g_FILLER(S_hypoxic_pit),
	g_FILLER(S_unlucky_trap),
	g_FILLER(S_alignment_reduction_trap),
	g_FILLER(S_malignant_trap),
	g_FILLER(S_stat_damage_trap),
	g_FILLER(S_half_memory_trap),
	g_FILLER(S_half_training_trap),
	g_FILLER(S_debuff_trap),
	g_FILLER(S_trip_once_trap),
	g_FILLER(S_narcolepsy_trap),
	g_FILLER(S_martial_arts_trap),
	g_FILLER(S_mean_burden_trap),
	g_FILLER(S_carrcap_trap),
	g_FILLER(S_umeng_trap),

	g_FILLER(S_spell_cooldown_trap),
	g_FILLER(S_turbodull_trap),
	g_FILLER(S_reddam_trap),
	g_FILLER(S_redinc_trap),
	g_FILLER(S_mulch_trap),
	g_FILLER(S_inaccuracy_trap),
	g_FILLER(S_monster_multishot_trap),
	g_FILLER(S_have_a_pin_trap),
	g_FILLER(S_costly_failure_trap),
	g_FILLER(S_inventory_size_trap),

	g_FILLER(S_spike_trap),
	g_FILLER(S_paper_arrow_trap),
	g_FILLER(S_knife_trap),
	g_FILLER(S_summon_monster_trap),
	g_FILLER(S_red_damage_trap),
	g_FILLER(S_green_damage_trap),
	g_FILLER(S_yellow_damage_trap),
	g_FILLER(S_white_damage_trap),
	g_FILLER(S_blue_damage_trap),
	g_FILLER(S_multi_debuff_trap),

	g_FILLER(S_magic_damage_trap),

	g_FILLER(S_wild_weather_trap),

	g_FILLER(S_timerun_trap),
	0xb3,	/* S_vbeam:	meta-3, vertical rule */
	0xc4,	/* S_hbeam:	meta-D, horizontal rule */
	g_FILLER(S_lslant),
	g_FILLER(S_rslant),
	g_FILLER(S_digbeam),
	g_FILLER(S_flashbeam),
/*70*/	g_FILLER(S_boomleft),
	g_FILLER(S_boomright),
	g_FILLER(S_ss1),
	g_FILLER(S_ss2),
	g_FILLER(S_ss3),
	g_FILLER(S_ss4),
	g_FILLER(S_sw_tl),
	g_FILLER(S_sw_tc),
	g_FILLER(S_sw_tr),
	0xb3,	/* S_sw_ml:	meta-3, vertical rule */
/*80*/	0xb3,	/* S_sw_mr:	meta-3, vertical rule */
	g_FILLER(S_sw_bl),
	g_FILLER(S_sw_bc),
	g_FILLER(S_sw_br),
	g_FILLER(S_explode1),
	g_FILLER(S_explode2),
	g_FILLER(S_explode3),
	0xb3,	/* S_explode4:	meta-3, vertical rule */
	g_FILLER(S_explode5),
	0xb3,	/* S_explode6:	meta-3, vertical rule */
/*90*/	g_FILLER(S_explode7),
	g_FILLER(S_explode8),
	g_FILLER(S_explode9),
};
#endif  /* ASCIIGRAPH */

#ifdef TERMLIB
void (*decgraphics_mode_callback)(void) = NULL;  /* set in tty_start_screen() */

static uchar dec_graphics[MAXPCHARS] = {
/* 0*/	g_FILLER(S_stone),
	0xf8,	/* S_vwall:	meta-x, vertical rule */
	0xf1,	/* S_hwall:	meta-q, horizontal rule */
	0xec,	/* S_tlcorn:	meta-l, top left corner */
	0xeb,	/* S_trcorn:	meta-k, top right corner */
	0xed,	/* S_blcorn:	meta-m, bottom left */
	0xea,	/* S_brcorn:	meta-j, bottom right */
	0xee,	/* S_crwall:	meta-n, cross */
	0xf6,	/* S_tuwall:	meta-v, T up */
	0xf7,	/* S_tdwall:	meta-w, T down */
/*10*/	0xf5,	/* S_tlwall:	meta-u, T left */
	0xf4,	/* S_trwall:	meta-t, T right */
	g_FILLER(S_rockwall),	/* rockwall */
	g_FILLER(S_gravewall),	/* gravewall */
	g_FILLER(S_tunnelwall),	/* tunnelwall */
	0xfe,	/* S_ndoor:	meta-~, centered dot */
	0xe1,	/* S_vodoor:	meta-a, solid block */
	0xe1,	/* S_hodoor:	meta-a, solid block */
	g_FILLER(S_vcdoor),
	g_FILLER(S_hcdoor),
/*20*/	0xfb,	/* S_bars:	meta-{, small pi, courtesy Pat Rankin */

	0xe7,	/* S_tree:	meta-g, plus-or-minus, courtesy Pat Rankin */
	0xe7,	/* S_farmland */
	g_FILLER(S_mountain),	/* mountain */
	0xfe,	/* S_room:	meta-~, centered dot */
	g_FILLER(S_stone),	/* S_darkroom:	meta-~, centered dot */
	g_FILLER(S_corr),
	g_FILLER(S_litcorr),
	g_FILLER(S_upstair),
	g_FILLER(S_dnstair),
/*30*/	0xf9,	/* S_upladder:	meta-y, greater-than-or-equals */
	0xfa,	/* S_dnladder:	meta-z, less-than-or-equals */
	g_FILLER(S_altar),	/* 0xc3, \E)3: meta-C, dagger */
	g_FILLER(S_grave),
	g_FILLER(S_throne),
	g_FILLER(S_sink),
	g_FILLER(S_toilet),
	g_FILLER(S_fountain),	/* 0xdb, \E)3: meta-[, integral top half */
	0xe0,	/* S_pool:	meta-\, diamond */
	g_FILLER(S_ice),	/* S_ice:	meta-~, centered dot */
/*40*/	0xe0,	/* S_lava:	meta-\, diamond */
	g_FILLER(S_vodbridge),	/* S_vodbridge:	meta-~, centered dot */
	g_FILLER(S_hodbridge),	/* S_hodbridge:	meta-~, centered dot */
	g_FILLER(S_vcdbridge),
	g_FILLER(S_hcdbridge),
	g_FILLER(S_air),
	g_FILLER(S_cloud),
	g_FILLER(S_dungwall),	/* dungwall */
	0xe0,	/* S_water:	meta-\, diamond */
	g_FILLER(S_watertunnel),	/* watertunnel */
/*50*/	0xe0,	/* S_crystalwater:	meta-w, approx. equals */
	0xe0, /* moorland */
	0xe0, /* urinelake */
	0xe0, /* shiftingsand */
	0xe0, /* styxriver */
	g_FILLER(S_well),
	g_FILLER(S_poisonedwell),
	g_FILLER(S_wagon),
	g_FILLER(S_burningwagon),
	g_FILLER(S_woodentable),
/*60*/	g_FILLER(S_strawmattress),
	g_FILLER(S_snow), /* S_snow */
	g_FILLER(S_ash), /* S_ash */
	g_FILLER(S_sand), /* S_sand */
	g_FILLER(S_pavedfloor), /* S_pavedfloor */
	g_FILLER(S_highway), /* S_highway */
	g_FILLER(S_grass), /* S_grass */
	g_FILLER(S_nethermist), /* S_nethermist */
	g_FILLER(S_stalactite),
	g_FILLER(S_cryptfloor), /* S_cryptfloor */
/*70*/	g_FILLER(S_bubbles),
	g_FILLER(S_raincloud),
	g_FILLER(S_pentagram),
	g_FILLER(S_carvedbed),
	g_FILLER(S_grayglyph),
	g_FILLER(S_arrow_trap),
	g_FILLER(S_dart_trap),
	g_FILLER(S_falling_rock_trap),
	g_FILLER(S_squeaky_board),
	g_FILLER(S_bear_trap),
	g_FILLER(S_land_mine),
	g_FILLER(S_rolling_boulder_trap),
	g_FILLER(S_sleeping_gas_trap),
	g_FILLER(S_rust_trap),
	g_FILLER(S_fire_trap),
	g_FILLER(S_pit),
	g_FILLER(S_spiked_pit),
	g_FILLER(S_hole),
	g_FILLER(S_trap_door),
	g_FILLER(S_teleportation_trap),
	g_FILLER(S_level_teleporter),
	g_FILLER(S_magic_portal),
	g_FILLER(S_web),	/* 0xbd, \E)3: meta-=, int'l currency */
	g_FILLER(S_statue_trap),
	g_FILLER(S_magic_trap),
	g_FILLER(S_anti_magic_trap),
	g_FILLER(S_polymorph_trap),
	g_FILLER(S_ice_trap),
	g_FILLER(S_spear_trap),
	g_FILLER(S_falling_rocks_trap),
	g_FILLER(S_magic_beam_trap),
	g_FILLER(S_pile_of_shit),
	g_FILLER(S_animation_trap),
	g_FILLER(S_glyph_of_warding),
	g_FILLER(S_scything_blade),
	g_FILLER(S_bolt_trap),
	g_FILLER(S_acid_pool),
	g_FILLER(S_water_pool),
	g_FILLER(S_poison_gas_trap),
	g_FILLER(S_slow_gas_trap),
	g_FILLER(S_shock_trap),
	g_FILLER(S_rmb_loss_trap),
	g_FILLER(S_display_trap),
	g_FILLER(S_spell_loss_trap),
	g_FILLER(S_yellow_spell_trap),
	g_FILLER(S_auto_destruct_trap),
	g_FILLER(S_memory_trap),
	g_FILLER(S_inventory_trap),
	g_FILLER(S_black_ng_wall_trap),
	g_FILLER(S_menu_trap),
	g_FILLER(S_speed_trap),
	g_FILLER(S_superscroller_trap),
	g_FILLER(S_active_superscroller_trap),
	g_FILLER(S_swarm_trap),
	g_FILLER(S_automatic_switcher),
	g_FILLER(S_unknown_trap),
	g_FILLER(S_heel_trap),
	g_FILLER(S_vuln_trap),

	g_FILLER(S_lava_trap),
	g_FILLER(S_flood_trap),
	g_FILLER(S_drain_trap),
	g_FILLER(S_free_hand_trap),
	g_FILLER(S_disintegration_trap),
	g_FILLER(S_unidentify_trap),
	g_FILLER(S_thirst_trap),
	g_FILLER(S_luck_trap),
	g_FILLER(S_shades_of_grey_trap),
	g_FILLER(S_item_telep_trap),
	g_FILLER(S_gravity_trap),
	g_FILLER(S_death_trap),
	g_FILLER(S_stone_to_flesh_trap),
	g_FILLER(S_quicksand_trap),
	g_FILLER(S_faint_trap),
	g_FILLER(S_curse_trap),
	g_FILLER(S_difficulty_trap),
	g_FILLER(S_sound_trap),
	g_FILLER(S_caster_trap),
	g_FILLER(S_weakness_trap),
	g_FILLER(S_rot_thirteen_trap),
	g_FILLER(S_bishop_trap),
	g_FILLER(S_confusion_trap),
	g_FILLER(S_nupesell_trap),
	g_FILLER(S_drop_trap),
	g_FILLER(S_dstw_trap),
	g_FILLER(S_status_trap),
	g_FILLER(S_alignment_trap),
	g_FILLER(S_stairs_trap),

	g_FILLER(S_pestilence_trap),
	g_FILLER(S_famine_trap),

	g_FILLER(S_relocation_trap),

	g_FILLER(S_uninformation_trap),

	g_FILLER(S_intrinsic_loss_trap),
	g_FILLER(S_blood_loss_trap),
	g_FILLER(S_bad_effect_trap),
	g_FILLER(S_multiplying_trap),
	g_FILLER(S_auto_vuln_trap),
	g_FILLER(S_tele_items_trap),
	g_FILLER(S_nastiness_trap),

	g_FILLER(S_recursion_trap),
	g_FILLER(S_respawn_trap),
	g_FILLER(S_warp_zone),
	g_FILLER(S_captcha_trap),
	g_FILLER(S_mind_wipe_trap),
	g_FILLER(S_shuriken_trap),
	g_FILLER(S_shit_pit),
	g_FILLER(S_shaft_trap),
	g_FILLER(S_lock_trap),
	g_FILLER(S_mc_trap),
	g_FILLER(S_farlook_trap),
	g_FILLER(S_gateway_trap),
	g_FILLER(S_growing_trap),
	g_FILLER(S_cooling_trap),
	g_FILLER(S_bar_trap),
	g_FILLER(S_locking_trap),
	g_FILLER(S_air_trap),
	g_FILLER(S_terrain_trap),

	g_FILLER(S_loudspeaker),
	g_FILLER(S_nest_trap),
	g_FILLER(S_bullet_trap),
	g_FILLER(S_paralysis_trap),
	g_FILLER(S_cyanide_trap),
	g_FILLER(S_laser_trap),
	g_FILLER(S_giant_chasm),
	g_FILLER(S_fart_trap),
	g_FILLER(S_confuse_trap),
	g_FILLER(S_stun_trap),
	g_FILLER(S_hallucination_trap),
	g_FILLER(S_petrification_trap),
	g_FILLER(S_numbness_trap),
	g_FILLER(S_freezing_trap),
	g_FILLER(S_burning_trap),
	g_FILLER(S_fear_trap),
	g_FILLER(S_blindness_trap),
	g_FILLER(S_glib_trap),
	g_FILLER(S_slime_trap),
	g_FILLER(S_inertia_trap),
	g_FILLER(S_time_trap),
	g_FILLER(S_lycanthropy_trap),
	g_FILLER(S_unlight_trap),
	g_FILLER(S_elemental_trap),
	g_FILLER(S_escalating_trap),
	g_FILLER(S_negative_trap),
	g_FILLER(S_mana_trap),
	g_FILLER(S_sin_trap),
	g_FILLER(S_destroy_armor_trap),
	g_FILLER(S_divine_anger_trap),
	g_FILLER(S_genetic_trap),
	g_FILLER(S_missingno_trap),
	g_FILLER(S_cancellation_trap),
	g_FILLER(S_hostility_trap),
	g_FILLER(S_boss_trap),
	g_FILLER(S_wishing_trap),
	g_FILLER(S_recurring_amn_trap),
	g_FILLER(S_bigscript_trap),
	g_FILLER(S_bank_trap),
	g_FILLER(S_only_trap),
	g_FILLER(S_map_trap),
	g_FILLER(S_tech_trap),
	g_FILLER(S_disenchant_trap),
	g_FILLER(S_verisiert),
	g_FILLER(S_chaos_trap),
	g_FILLER(S_muteness_trap),
	g_FILLER(S_ntll_trap),
	g_FILLER(S_engraving_trap),
	g_FILLER(S_magic_device_trap),
	g_FILLER(S_book_trap),
	g_FILLER(S_level_trap),
	g_FILLER(S_quiz_trap),

	g_FILLER(S_falling_boulder_trap),
	g_FILLER(S_glass_arrow_trap),
	g_FILLER(S_glass_bolt_trap),
	g_FILLER(S_out_of_magic_trap),
	g_FILLER(S_plasma_trap),
	g_FILLER(S_bomb_trap),
	g_FILLER(S_earthquake_trap),
	g_FILLER(S_noise_trap),
	g_FILLER(S_glue_trap),
	g_FILLER(S_guillotine_trap),
	g_FILLER(S_bisection_trap),
	g_FILLER(S_volt_trap),
	g_FILLER(S_horde_trap),
	g_FILLER(S_immobility_trap),
	g_FILLER(S_green_glyph_trap),
	g_FILLER(S_blue_glyph_trap),
	g_FILLER(S_yellow_glyph_trap),
	g_FILLER(S_orange_glyph_trap),
	g_FILLER(S_black_glyph_trap),
	g_FILLER(S_purple_glyph_trap),

	g_FILLER(S_metabolic_trap),
	g_FILLER(S_no_return_trap),
	g_FILLER(S_ego_trap),
	g_FILLER(S_fast_forward_trap),
	g_FILLER(S_rotten_trap),
	g_FILLER(S_unskilled_trap),
	g_FILLER(S_low_stats_trap),
	g_FILLER(S_training_trap),
	g_FILLER(S_exercise_trap),
	g_FILLER(S_falling_loadstone_trap),
	g_FILLER(S_summon_undead_trap),
	g_FILLER(S_falling_nastystone_trap),

	g_FILLER(S_spined_ball_trap),
	g_FILLER(S_pendulum_trap),
	g_FILLER(S_turn_table),
	g_FILLER(S_scent_trap),
	g_FILLER(S_banana_trap),
	g_FILLER(S_falling_tub_trap),
	g_FILLER(S_alarm),
	g_FILLER(S_caltrops_trap),
	g_FILLER(S_blade_wire),
	g_FILLER(S_magnet_trap),
	g_FILLER(S_slingshot_trap),
	g_FILLER(S_cannon_trap),
	g_FILLER(S_venom_sprinkler),
	g_FILLER(S_fumarole),

	g_FILLER(S_mana_pit),
	g_FILLER(S_elemental_portal),
	g_FILLER(S_girliness_trap),
	g_FILLER(S_fumbling_trap),
	g_FILLER(S_egomonster_trap),
	g_FILLER(S_flooding_trap),
	g_FILLER(S_monster_cube),
	g_FILLER(S_cursed_grave),
	g_FILLER(S_limitation_trap),
	g_FILLER(S_weak_sight_trap),
	g_FILLER(S_random_message_trap),

	g_FILLER(S_desecration_trap),
	g_FILLER(S_starvation_trap),
	g_FILLER(S_dropless_trap),
	g_FILLER(S_low_effect_trap),
	g_FILLER(S_invisible_trap),
	g_FILLER(S_ghost_world_trap),
	g_FILLER(S_dehydration_trap),
	g_FILLER(S_hate_trap),
	g_FILLER(S_spacewars_trap),
	g_FILLER(S_temporary_recursion_trap),
	g_FILLER(S_totter_trap),
	g_FILLER(S_nonintrinsical_trap),
	g_FILLER(S_dropcurse_trap),
	g_FILLER(S_nakedness_trap),
	g_FILLER(S_antilevel_trap),
	g_FILLER(S_ventilator),
	g_FILLER(S_stealer_trap),
	g_FILLER(S_rebellion_trap),
	g_FILLER(S_crap_trap),
	g_FILLER(S_misfire_trap),
	g_FILLER(S_trap_of_walls),

	g_FILLER(S_disconnect_trap),
	g_FILLER(S_interface_screw_trap),
	g_FILLER(S_dimness_trap),
	g_FILLER(S_evil_artifact_trap),
	g_FILLER(S_bossfight_trap),
	g_FILLER(S_entire_level_trap),
	g_FILLER(S_bones_trap),
	g_FILLER(S_rodney_trap),
	g_FILLER(S_autocurse_trap),
	g_FILLER(S_highlevel_trap),
	g_FILLER(S_spell_forgetting_trap),
	g_FILLER(S_sound_effect_trap),

	g_FILLER(S_trap_percents),

	g_FILLER(S_nexus_trap),
	g_FILLER(S_leg_trap),
	g_FILLER(S_artifact_jackpot_trap),
	g_FILLER(S_map_amnesia_trap),
	g_FILLER(S_spreading_trap),
	g_FILLER(S_adjacent_trap),
	g_FILLER(S_superthing_trap),
	g_FILLER(S_current_shaft),
	g_FILLER(S_levitation_trap),
	g_FILLER(S_bowel_cramps_trap),
	g_FILLER(S_unequipping_trap),
	g_FILLER(S_good_artifact_trap),
	g_FILLER(S_gender_trap),
	g_FILLER(S_trap_of_oa),
	g_FILLER(S_sincount_trap),
	g_FILLER(S_beamer_trap),
	g_FILLER(S_level_beamer),
	g_FILLER(S_pet_trap),
	g_FILLER(S_piercing_beam_trap),
	g_FILLER(S_wrenching_trap),
	g_FILLER(S_tracker_trap),
	g_FILLER(S_nurse_trap),
	g_FILLER(S_back_to_start_trap),
	g_FILLER(S_nemesis_trap),
	g_FILLER(S_strew_trap),
	g_FILLER(S_outta_depth_trap),
	g_FILLER(S_punishment_trap),
	g_FILLER(S_boon_trap),
	g_FILLER(S_fountain_trap),
	g_FILLER(S_throne_trap),
	g_FILLER(S_anoxic_pit),
	g_FILLER(S_arabella_speaker),
	g_FILLER(S_femmy_trap),
	g_FILLER(S_madeleine_trap),
	g_FILLER(S_marlena_trap),
	g_FILLER(S_anastasia_trap),
	g_FILLER(S_filler_trap),
	g_FILLER(S_toxic_venom_trap),
	g_FILLER(S_insanity_trap),
	g_FILLER(S_madness_trap),
	g_FILLER(S_jessica_trap),
	g_FILLER(S_solvejg_trap),
	g_FILLER(S_wendy_trap),
	g_FILLER(S_katharina_trap),
	g_FILLER(S_elena_trap),
	g_FILLER(S_thai_trap),
	g_FILLER(S_elif_trap),
	g_FILLER(S_nadja_trap),
	g_FILLER(S_sandra_trap),
	g_FILLER(S_natalje_trap),
	g_FILLER(S_jeanetta_trap),
	g_FILLER(S_yvonne_trap),
	g_FILLER(S_maurah_trap),

	g_FILLER(S_lootcut_trap),
	g_FILLER(S_monster_speed_trap),
	g_FILLER(S_scaling_trap),
	g_FILLER(S_enmity_trap),
	g_FILLER(S_white_spell_trap),
	g_FILLER(S_gray_spell_trap),
	g_FILLER(S_quasar_trap),
	g_FILLER(S_momma_trap),
	g_FILLER(S_horror_trap),
	g_FILLER(S_artificer_trap),
	g_FILLER(S_wereform_trap),
	g_FILLER(S_nonprayer_trap),
	g_FILLER(S_evil_patch_trap),
	g_FILLER(S_hard_mode_trap),
	g_FILLER(S_secret_attack_trap),
	g_FILLER(S_eater_trap),
	g_FILLER(S_covetous_trap),
	g_FILLER(S_not_seen_trap),
	g_FILLER(S_dark_mode_trap),
	g_FILLER(S_antisearch_trap),
	g_FILLER(S_homicide_trap),
	g_FILLER(S_nasty_nation_trap),
	g_FILLER(S_wakeup_call_trap),
	g_FILLER(S_grayout_trap),
	g_FILLER(S_gray_center_trap),
	g_FILLER(S_checkerboard_trap),
	g_FILLER(S_clockwise_spin_trap),
	g_FILLER(S_counterclockwise_trap),
	g_FILLER(S_lag_trap),
	g_FILLER(S_blesscurse_trap),
	g_FILLER(S_delight_trap),
	g_FILLER(S_discharge_trap),
	g_FILLER(S_trashing_trap),
	g_FILLER(S_filtering_trap),
	g_FILLER(S_deformatting_trap),
	g_FILLER(S_flicker_strip_trap),
	g_FILLER(S_undressing_trap),
	g_FILLER(S_hyperbluewall_trap),
	g_FILLER(S_nolite_trap),
	g_FILLER(S_paranoia_trap),
	g_FILLER(S_fleecescript_trap),
	g_FILLER(S_interrupt_trap),
	g_FILLER(S_dustbin_trap),
	g_FILLER(S_mana_battery_trap),
	g_FILLER(S_monsterfingers_trap),
	g_FILLER(S_miscast_trap),
	g_FILLER(S_message_suppression_trap),
	g_FILLER(S_stuck_announcement_trap),
	g_FILLER(S_bloodthirsty_trap),
	g_FILLER(S_maximum_damage_trap),
	g_FILLER(S_latency_trap),
	g_FILLER(S_starlit_trap),
	g_FILLER(S_knowledge_trap),
	g_FILLER(S_highscore_trap),
	g_FILLER(S_pink_spell_trap),
	g_FILLER(S_green_spell_trap),
	g_FILLER(S_evc_trap),
	g_FILLER(S_underlayer_trap),
	g_FILLER(S_damage_meter_trap),
	g_FILLER(S_arbitrary_weight_trap),
	g_FILLER(S_fucked_info_trap),
	g_FILLER(S_black_spell_trap),
	g_FILLER(S_cyan_spell_trap),
	g_FILLER(S_heap_trap),
	g_FILLER(S_blue_spell_trap),
	g_FILLER(S_tron_trap),
	g_FILLER(S_red_spell_trap),
	g_FILLER(S_too_heavy_trap),
	g_FILLER(S_elongation_trap),
	g_FILLER(S_wrapover_trap),
	g_FILLER(S_destruction_trap),
	g_FILLER(S_melee_prefix_trap),
	g_FILLER(S_automore_trap),
	g_FILLER(S_unfair_attack_trap),

	g_FILLER(S_kop_cube),
	g_FILLER(S_boss_spawner),
	g_FILLER(S_contamination_trap),

	g_FILLER(S_evil_heel_trap),
	g_FILLER(S_bad_equipment_trap),
	g_FILLER(S_tempoconflict_trap),
	g_FILLER(S_tempohunger_trap),
	g_FILLER(S_teleportitis_trap),
	g_FILLER(S_polymorphitis_trap),
	g_FILLER(S_premature_death_trap),
	g_FILLER(S_lasting_amnesia_trap),
	g_FILLER(S_ragnarok_trap),
	g_FILLER(S_single_disenchant_trap),
	g_FILLER(S_acid_pit),

	g_FILLER(S_severe_disenchant_trap),
	g_FILLER(S_pain_trap),
	g_FILLER(S_trembling_trap),
	g_FILLER(S_techcap_trap),
	g_FILLER(S_spell_memory_trap),
	g_FILLER(S_skill_reduction_trap),
	g_FILLER(S_skillcap_trap),
	g_FILLER(S_permanent_stat_damage_trap),

	g_FILLER(S_orange_spell_trap),
	g_FILLER(S_violet_spell_trap),
	g_FILLER(S_trap_of_longing),
	g_FILLER(S_cursed_part_trap),
	g_FILLER(S_quaversal_trap),
	g_FILLER(S_appearance_shuffling_trap),
	g_FILLER(S_brown_spell_trap),
	g_FILLER(S_choiceless_trap),
	g_FILLER(S_goldspell_trap),
	g_FILLER(S_deprovement_trap),
	g_FILLER(S_initialization_trap),
	g_FILLER(S_gushlush_trap),
	g_FILLER(S_soiltype_trap),
	g_FILLER(S_dangerous_terrain_trap),
	g_FILLER(S_fallout_trap),
	g_FILLER(S_mojibake_trap),
	g_FILLER(S_gravation_trap),
	g_FILLER(S_uncalled_trap),
	g_FILLER(S_exploding_dice_trap),
	g_FILLER(S_permacurse_trap),
	g_FILLER(S_shrouded_identity_trap),
	g_FILLER(S_feeler_gauges_trap),
	g_FILLER(S_long_screwup_trap),
	g_FILLER(S_wing_yellow_changer),
	g_FILLER(S_life_saving_trap),
	g_FILLER(S_curseuse_trap),
	g_FILLER(S_cut_nutrition_trap),
	g_FILLER(S_skill_loss_trap),
	g_FILLER(S_autopilot_trap),
	g_FILLER(S_force_trap),
	g_FILLER(S_monster_glyph_trap),
	g_FILLER(S_changing_directive_trap),
	g_FILLER(S_container_kaboom_trap),
	g_FILLER(S_steal_degrade_trap),
	g_FILLER(S_left_inventory_trap),
	g_FILLER(S_fluctuating_speed_trap),
	g_FILLER(S_tarmustrokingnora_trap),
	g_FILLER(S_failure_trap),
	g_FILLER(S_bright_cyan_spell_trap),
	g_FILLER(S_frequentation_spawn_trap),
	g_FILLER(S_pet_ai_trap),
	g_FILLER(S_satan_trap),
	g_FILLER(S_rememberance_trap),
	g_FILLER(S_pokelie_trap),
	g_FILLER(S_autopickup_trap),
	g_FILLER(S_dywypi_trap),
	g_FILLER(S_silver_spell_trap),
	g_FILLER(S_metal_spell_trap),
	g_FILLER(S_platinum_spell_trap),
	g_FILLER(S_manler_trap),
	g_FILLER(S_doorning_trap),
	g_FILLER(S_nownsible_trap),
	g_FILLER(S_elm_street_trap),
	g_FILLER(S_monnoise_trap),
	g_FILLER(S_rang_call_trap),
	g_FILLER(S_recurring_spell_loss_trap),
	g_FILLER(S_antitraining_trap),
	g_FILLER(S_techout_trap),
	g_FILLER(S_stat_decay_trap),
	g_FILLER(S_movemork_trap),

	g_FILLER(S_hybrid_trap),
	g_FILLER(S_shapechange_trap),
	g_FILLER(S_meltem_trap),
	g_FILLER(S_miguc_trap),
	g_FILLER(S_directive_trap),
	g_FILLER(S_satatue_trap),
	g_FILLER(S_farting_web),
	g_FILLER(S_cataclysm_trap),
	g_FILLER(S_data_delete_trap),
	g_FILLER(S_elder_tentacling_trap),
	g_FILLER(S_footerer_trap),

	g_FILLER(S_bad_part_trap),
	g_FILLER(S_completely_bad_part_trap),
	g_FILLER(S_evil_variant_trap),

	g_FILLER(S_grave_wall_trap),
	g_FILLER(S_tunnel_trap),
	g_FILLER(S_farmland_trap),
	g_FILLER(S_mountain_trap),
	g_FILLER(S_water_tunnel_trap),
	g_FILLER(S_crystal_flood_trap),
	g_FILLER(S_moorland_trap),
	g_FILLER(S_urine_trap),
	g_FILLER(S_shifting_sand_trap),
	g_FILLER(S_styx_trap),
	g_FILLER(S_pentagram_trap),
	g_FILLER(S_snow_trap),
	g_FILLER(S_ash_trap),
	g_FILLER(S_sand_trap),
	g_FILLER(S_pavement_trap),
	g_FILLER(S_highway_trap),
	g_FILLER(S_grassland_trap),
	g_FILLER(S_nether_mist_trap),
	g_FILLER(S_stalactite_trap),
	g_FILLER(S_cryptfloor_trap),
	g_FILLER(S_bubble_trap),
	g_FILLER(S_rain_cloud_trap),
	g_FILLER(S_item_nastification_trap),
	g_FILLER(S_sanity_increase_trap),
	g_FILLER(S_psi_trap),
	g_FILLER(S_gay_trap),
	g_FILLER(S_sarah_trap),
	g_FILLER(S_claudia_trap),
	g_FILLER(S_ludgera_trap),
	g_FILLER(S_kati_trap),
	g_FILLER(S_sanity_treble_trap),
	g_FILLER(S_stat_decrease_trap),
	g_FILLER(S_simeout_trap),
	g_FILLER(S_spressing_trap),
	g_FILLER(S_nelly_trap),
	g_FILLER(S_eveline_trap),
	g_FILLER(S_karin_trap),
	g_FILLER(S_juen_trap),
	g_FILLER(S_kristina_trap),
	g_FILLER(S_lou_trap),
	g_FILLER(S_almut_trap),
	g_FILLER(S_julietta_trap),
	g_FILLER(S_arabella_trap),
	g_FILLER(S_kristin_trap),
	g_FILLER(S_anna_trap),
	g_FILLER(S_ruea_trap),
	g_FILLER(S_dora_trap),
	g_FILLER(S_marike_trap),
	g_FILLER(S_jette_trap),
	g_FILLER(S_ina_trap),
	g_FILLER(S_sing_trap),
	g_FILLER(S_victoria_trap),
	g_FILLER(S_melissa_trap),
	g_FILLER(S_anita_trap),
	g_FILLER(S_henrietta_trap),
	g_FILLER(S_verena_trap),
	g_FILLER(S_annemarie_trap),
	g_FILLER(S_jil_trap),
	g_FILLER(S_jana_trap),
	g_FILLER(S_katrin_trap),
	g_FILLER(S_gudrun_trap),
	g_FILLER(S_ella_trap),
	g_FILLER(S_manuela_trap),
	g_FILLER(S_jennifer_trap),
	g_FILLER(S_patricia_trap),
	g_FILLER(S_antje_trap),
	g_FILLER(S_antje_x_trap),
	g_FILLER(S_kerstin_trap),
	g_FILLER(S_laura_trap),
	g_FILLER(S_larissa_trap),
	g_FILLER(S_nora_trap),
	g_FILLER(S_natalia_trap),
	g_FILLER(S_susanne_trap),
	g_FILLER(S_lisa_trap),
	g_FILLER(S_bridghitte_trap),
	g_FILLER(S_julia_trap),
	g_FILLER(S_nicole_trap),
	g_FILLER(S_rita_trap),
	g_FILLER(S_janina_trap),
	g_FILLER(S_rosa_trap),
	g_FILLER(S_ksenia_trap),
	g_FILLER(S_lydia_trap),
	g_FILLER(S_conny_trap),
	g_FILLER(S_katia_trap),
	g_FILLER(S_mariya_trap),
	g_FILLER(S_elise_trap),
	g_FILLER(S_ronja_trap),
	g_FILLER(S_ariane_trap),
	g_FILLER(S_johanna_trap),
	g_FILLER(S_inge_trap),

	g_FILLER(S_wall_trap),
	g_FILLER(S_monster_generator),
	g_FILLER(S_potion_dispenser),
	g_FILLER(S_spacewars_spawn_trap),
	g_FILLER(S_tv_tropes_trap),
	g_FILLER(S_symbiote_trap),
	g_FILLER(S_kill_symbiote_trap),
	g_FILLER(S_symbiote_replacement_trap),
	g_FILLER(S_shutdown_trap),
	g_FILLER(S_corona_trap),
	g_FILLER(S_unproofing_trap),
	g_FILLER(S_visibility_trap),
	g_FILLER(S_feminism_stone_trap),
	g_FILLER(S_branch_teleporter),
	g_FILLER(S_branch_beamer),
	g_FILLER(S_poison_arrow_trap),
	g_FILLER(S_poison_bolt_trap),
	g_FILLER(S_mace_trap),
	g_FILLER(S_shueft_trap),
	g_FILLER(S_moth_larvae_trap),
	g_FILLER(S_worthiness_trap),
	g_FILLER(S_conduct_trap),
	g_FILLER(S_strikethrough_trap),
	g_FILLER(S_multiple_gather_trap),
	g_FILLER(S_vivisection_trap),
	g_FILLER(S_instafeminism_trap),
	g_FILLER(S_instanasty_trap),
	g_FILLER(S_skill_point_loss_trap),
	g_FILLER(S_perfect_match_trap),
	g_FILLER(S_dumbie_lightsaber_trap),
	g_FILLER(S_wrong_stairs),
	g_FILLER(S_techstop_trap),
	g_FILLER(S_amnesia_switch_trap),
	g_FILLER(S_skill_swap_trap),
	g_FILLER(S_skill_upordown_trap),
	g_FILLER(S_skill_randomize_trap),

	g_FILLER(S_skill_multiply_trap),
	g_FILLER(S_trapwalk_trap),
	g_FILLER(S_cluster_trap),
	g_FILLER(S_field_trap),
	g_FILLER(S_monicide_trap),
	g_FILLER(S_trap_creation_trap),
	g_FILLER(S_leold_trap),
	g_FILLER(S_animeband_trap),
	g_FILLER(S_perfume_trap),
	g_FILLER(S_court_trap),
	g_FILLER(S_elder_scrolls_trap),
	g_FILLER(S_joke_trap),
	g_FILLER(S_dungeon_lords_trap),
	g_FILLER(S_fortytwo_trap),
	g_FILLER(S_randomize_trap),
	g_FILLER(S_evilroom_trap),
	g_FILLER(S_aoe_trap),
	g_FILLER(S_elona_trap),
	g_FILLER(S_religion_trap),
	g_FILLER(S_steamband_trap),
	g_FILLER(S_hardcore_trap),
	g_FILLER(S_machine_trap),
	g_FILLER(S_bee_trap),
	g_FILLER(S_migo_trap),
	g_FILLER(S_angband_trap),
	g_FILLER(S_dnethack_trap),
	g_FILLER(S_evil_spawn_trap),
	g_FILLER(S_shoe_trap),
	g_FILLER(S_inside_trap),
	g_FILLER(S_doom_trap),
	g_FILLER(S_military_trap),
	g_FILLER(S_illusion_trap),
	g_FILLER(S_diablo_trap),

	g_FILLER(S_giant_explorer_trap),
	g_FILLER(S_trapwarp_trap),
	g_FILLER(S_yawm_trap),
	g_FILLER(S_cradle_of_chaos_trap),
	g_FILLER(S_tez_trap),
	g_FILLER(S_enthu_trap),
	g_FILLER(S_mikra_trap),
	g_FILLER(S_gots_too_good_trap),
	g_FILLER(S_killer_room_trap),
	g_FILLER(S_no_fun_walls_trap),

	g_FILLER(S_ruth_trap),
	g_FILLER(S_magdalena_trap),
	g_FILLER(S_marleen_trap),
	g_FILLER(S_klara_trap),
	g_FILLER(S_friederike_trap),
	g_FILLER(S_naomi_trap),
	g_FILLER(S_ute_trap),
	g_FILLER(S_jasieen_trap),
	g_FILLER(S_yasaman_trap),
	g_FILLER(S_may_britt_trap),

	g_FILLER(S_calling_out_trap),
	g_FILLER(S_field_break_trap),
	g_FILLER(S_tenth_trap),
	g_FILLER(S_debt_trap),
	g_FILLER(S_inversion_trap),
	g_FILLER(S_wince_trap),
	g_FILLER(S_u_have_been_trap),
	g_FILLER(S_really_bad_trap),
	g_FILLER(S_covid_trap),
	g_FILLER(S_artiblast_trap),

	g_FILLER(S_persistent_fart_trap),
	g_FILLER(S_attacking_heel_trap),
	g_FILLER(S_trap_teleporter),
	g_FILLER(S_alignment_trash_trap),
	g_FILLER(S_reshuffle_trap),
	g_FILLER(S_musehand_trap),
	g_FILLER(S_dogside_trap),
	g_FILLER(S_bankrupt_trap),
	g_FILLER(S_fillup_trap),
	g_FILLER(S_airstrike_trap),
	g_FILLER(S_dynamite_trap),
	g_FILLER(S_malevolence_trap),
	g_FILLER(S_leaflet_trap),
	g_FILLER(S_tentadeep_trap),
	g_FILLER(S_stathalf_trap),
	g_FILLER(S_cutstat_trap),
	g_FILLER(S_rare_spawn_trap),
	g_FILLER(S_you_are_an_idiot_trap),
	g_FILLER(S_nastycurse_trap),
	g_FILLER(S_repeating_nastycurse_trap),

	g_FILLER(S_nadine_trap),
	g_FILLER(S_luisa_trap),
	g_FILLER(S_irina_trap),
	g_FILLER(S_liselotte_trap),
	g_FILLER(S_greta_trap),
	g_FILLER(S_jane_trap),
	g_FILLER(S_sue_lyn_trap),
	g_FILLER(S_charlotte_trap),
	g_FILLER(S_hannah_trap),
	g_FILLER(S_little_marie_trap),

	g_FILLER(S_real_lie_trap),
	g_FILLER(S_escape_past_trap),
	g_FILLER(S_pethate_trap),
	g_FILLER(S_pet_lashout_trap),
	g_FILLER(S_petstarve_trap),
	g_FILLER(S_petscrew_trap),
	g_FILLER(S_tech_loss_trap),
	g_FILLER(S_proofloss_trap),
	g_FILLER(S_un_invis_trap),
	g_FILLER(S_detectation_trap),

	g_FILLER(S_option_trap),
	g_FILLER(S_miscolor_trap),
	g_FILLER(S_one_rainbow_trap),
	g_FILLER(S_colorshift_trap),
	g_FILLER(S_top_line_trap),
	g_FILLER(S_caps_trap),
	g_FILLER(S_un_knowledge_trap),
	g_FILLER(S_darkhance_trap),
	g_FILLER(S_dschueueuet_trap),
	g_FILLER(S_nopeskill_trap),
	g_FILLER(S_fuckfuckfuck_trap),

	g_FILLER(S_burden_trap),
	g_FILLER(S_magic_vacuum_trap),

	g_FILLER(S_tanja_trap),
	g_FILLER(S_sonja_trap),
	g_FILLER(S_rhea_trap),
	g_FILLER(S_lara_trap),

	g_FILLER(S_fuck_over_trap),

	g_FILLER(S_dagger_trap),
	g_FILLER(S_razor_trap),
	g_FILLER(S_phosgene_trap),
	g_FILLER(S_chloroform_trap),
	g_FILLER(S_corrosion_trap),
	g_FILLER(S_flame_trap),
	g_FILLER(S_wither_trap),
	g_FILLER(S_phaseport_trap),
	g_FILLER(S_phasebeam_trap),
	g_FILLER(S_vulnerate_trap),

	g_FILLER(S_epvi_trap),

	g_FILLER(S_sabrina_trap),

	g_FILLER(S_aefde_trap),

	g_FILLER(S_falling_rock_cold_trap),
	g_FILLER(S_return_trap),
	g_FILLER(S_intrinsic_steal_trap),
	g_FILLER(S_score_axe_trap),
	g_FILLER(S_score_drain_trap),
	g_FILLER(S_single_unidentify_trap),
	g_FILLER(S_hypoxic_pit),
	g_FILLER(S_unlucky_trap),
	g_FILLER(S_alignment_reduction_trap),
	g_FILLER(S_malignant_trap),
	g_FILLER(S_stat_damage_trap),
	g_FILLER(S_half_memory_trap),
	g_FILLER(S_half_training_trap),
	g_FILLER(S_debuff_trap),
	g_FILLER(S_trip_once_trap),
	g_FILLER(S_narcolepsy_trap),
	g_FILLER(S_martial_arts_trap),
	g_FILLER(S_mean_burden_trap),
	g_FILLER(S_carrcap_trap),
	g_FILLER(S_umeng_trap),

	g_FILLER(S_spell_cooldown_trap),
	g_FILLER(S_turbodull_trap),
	g_FILLER(S_reddam_trap),
	g_FILLER(S_redinc_trap),
	g_FILLER(S_mulch_trap),
	g_FILLER(S_inaccuracy_trap),
	g_FILLER(S_monster_multishot_trap),
	g_FILLER(S_have_a_pin_trap),
	g_FILLER(S_costly_failure_trap),
	g_FILLER(S_inventory_size_trap),

	g_FILLER(S_spike_trap),
	g_FILLER(S_paper_arrow_trap),
	g_FILLER(S_knife_trap),
	g_FILLER(S_summon_monster_trap),
	g_FILLER(S_red_damage_trap),
	g_FILLER(S_green_damage_trap),
	g_FILLER(S_yellow_damage_trap),
	g_FILLER(S_white_damage_trap),
	g_FILLER(S_blue_damage_trap),
	g_FILLER(S_multi_debuff_trap),

	g_FILLER(S_magic_damage_trap),

	g_FILLER(S_wild_weather_trap),

	g_FILLER(S_timerun_trap),
	0xf8,	/* S_vbeam:	meta-x, vertical rule */
	0xf1,	/* S_hbeam:	meta-q, horizontal rule */
	g_FILLER(S_lslant),
	g_FILLER(S_rslant),
	g_FILLER(S_digbeam),
	g_FILLER(S_flashbeam),
/*70*/	g_FILLER(S_boomleft),
	g_FILLER(S_boomright),
	g_FILLER(S_ss1),
	g_FILLER(S_ss2),
	g_FILLER(S_ss3),
	g_FILLER(S_ss4),
	g_FILLER(S_sw_tl),
	0xef,	/* S_sw_tc:	meta-o, high horizontal line */
	g_FILLER(S_sw_tr),
	0xf8,	/* S_sw_ml:	meta-x, vertical rule */
/*80*/	0xf8,	/* S_sw_mr:	meta-x, vertical rule */
	g_FILLER(S_sw_bl),
	0xf3,	/* S_sw_bc:	meta-s, low horizontal line */
	g_FILLER(S_sw_br),
	g_FILLER(S_explode1),
	0xef,	/* S_explode2:	meta-o, high horizontal line */
	g_FILLER(S_explode3),
	0xf8,	/* S_explode4:	meta-x, vertical rule */
	g_FILLER(S_explode5),
	0xf8,	/* S_explode6:	meta-x, vertical rule */
/*90*/	g_FILLER(S_explode7),
	0xf3,	/* S_explode8:	meta-s, low horizontal line */
	g_FILLER(S_explode9),
};
#endif  /* TERMLIB */

static uchar wing_graphics[MAXPCHARS] = {
/* 0*/	g_FILLER(S_stone),
	0x64,	/* d */
	0x40,	/* @ */
	0x37,	/* 7 */
	0x7b,	/* { */
	0x5b,	/* [ */
	0x21,	/* ! */
	0x4E,	/* N */
	0x6f,	/* o */
	0x7c,	/* | */
/*10*/	0x3a,	/* : */
	0x47,	/* G */
	0x5f,	/* _ */
	0x4c,	/* L */
	0x24,	/* $ */
	0x7e,	/* ~ */
	0x5c,	/* closing slash */
	0x33,	/* " */
	0x2e, /* . */
	0x67, /* g */
/*20*/	0x44,	/* D */

	0x74,	/* t */
	0x3d,	/* = */
	0x30,	/* 0 */
	0x55,	/* U */
	0x25,	/* % */
	0x3f, /* ? */
	0x79, /* y */
	0x2d, /* - */
	0x28, /* ( */
/*30*/	0x6c,	/* l */
	0x5e,	/* ^ */
	0x23,	/* # */
	0x76, /* v */
	0x54, /* T */
	0x2b, /* + */
	0x63, /* c */
	0x3c, /* < */
	0x50,	/* P */
	0x69,	/* i */
/*40*/	0x31,	/* 1 */
	0x62,	/* b */
	0x5a,	/* Z */
	0x4b, /* K */
	0x58, /* X */
	0x26, /* & */
	0x2a, /* * */
	0x3b, /* ; */
	0x2f,	/* opening slash */
	0x57, /* W */
/*50*/	0x34,	/* 4 */
	0x6d, /* m */
	0x60, /* ` */
	0x6a, /* j */
	0x28, /* ( */
	0x42, /* B */
	0x70, /* p */
	0x7d, /* } */
	0x52, /* R */
	0x71, /* q */
/*60*/	0x51, /* Q */
	0x27, /* apostroph */
	0x39, /* 9 */
	0x65, /* e */
	0x6e, /* n */
	0x68, /* h */
	0x3e, /* > */
	0x46, /* F */
	0x49, /* I */
	0x4f, /* O */
/*70*/	0x36, /* 6 */
	0x66, /* f */
	0x56, /* V */
	0x6b, /* k */
	0x29, /* ) */
	g_FILLER(S_arrow_trap),
	g_FILLER(S_dart_trap),
	g_FILLER(S_falling_rock_trap),
	g_FILLER(S_squeaky_board),
	g_FILLER(S_bear_trap),
	g_FILLER(S_land_mine),
	g_FILLER(S_rolling_boulder_trap),
	g_FILLER(S_sleeping_gas_trap),
	g_FILLER(S_rust_trap),
	g_FILLER(S_fire_trap),
	g_FILLER(S_pit),
	g_FILLER(S_spiked_pit),
	g_FILLER(S_hole),
	g_FILLER(S_trap_door),
	g_FILLER(S_teleportation_trap),
	g_FILLER(S_level_teleporter),
	g_FILLER(S_magic_portal),
	g_FILLER(S_web),	/* 0xbd, \E)3: meta-=, int'l currency */
	g_FILLER(S_statue_trap),
	g_FILLER(S_magic_trap),
	g_FILLER(S_anti_magic_trap),
	g_FILLER(S_polymorph_trap),
	g_FILLER(S_ice_trap),
	g_FILLER(S_spear_trap),
	g_FILLER(S_falling_rocks_trap),
	g_FILLER(S_magic_beam_trap),
	g_FILLER(S_pile_of_shit),
	g_FILLER(S_animation_trap),
	g_FILLER(S_glyph_of_warding),
	g_FILLER(S_scything_blade),
	g_FILLER(S_bolt_trap),
	g_FILLER(S_acid_pool),
	g_FILLER(S_water_pool),
	g_FILLER(S_poison_gas_trap),
	g_FILLER(S_slow_gas_trap),
	g_FILLER(S_shock_trap),
	g_FILLER(S_rmb_loss_trap),
	g_FILLER(S_display_trap),
	g_FILLER(S_spell_loss_trap),
	g_FILLER(S_yellow_spell_trap),
	g_FILLER(S_auto_destruct_trap),
	g_FILLER(S_memory_trap),
	g_FILLER(S_inventory_trap),
	g_FILLER(S_black_ng_wall_trap),
	g_FILLER(S_menu_trap),
	g_FILLER(S_speed_trap),
	g_FILLER(S_superscroller_trap),
	g_FILLER(S_active_superscroller_trap),
	g_FILLER(S_swarm_trap),
	g_FILLER(S_automatic_switcher),
	g_FILLER(S_unknown_trap),
	g_FILLER(S_heel_trap),
	g_FILLER(S_vuln_trap),

	g_FILLER(S_lava_trap),
	g_FILLER(S_flood_trap),
	g_FILLER(S_drain_trap),
	g_FILLER(S_free_hand_trap),
	g_FILLER(S_disintegration_trap),
	g_FILLER(S_unidentify_trap),
	g_FILLER(S_thirst_trap),
	g_FILLER(S_luck_trap),
	g_FILLER(S_shades_of_grey_trap),
	g_FILLER(S_item_telep_trap),
	g_FILLER(S_gravity_trap),
	g_FILLER(S_death_trap),
	g_FILLER(S_stone_to_flesh_trap),
	g_FILLER(S_quicksand_trap),
	g_FILLER(S_faint_trap),
	g_FILLER(S_curse_trap),
	g_FILLER(S_difficulty_trap),
	g_FILLER(S_sound_trap),
	g_FILLER(S_caster_trap),
	g_FILLER(S_weakness_trap),
	g_FILLER(S_rot_thirteen_trap),
	g_FILLER(S_bishop_trap),
	g_FILLER(S_confusion_trap),
	g_FILLER(S_nupesell_trap),
	g_FILLER(S_drop_trap),
	g_FILLER(S_dstw_trap),
	g_FILLER(S_status_trap),
	g_FILLER(S_alignment_trap),
	g_FILLER(S_stairs_trap),

	g_FILLER(S_pestilence_trap),
	g_FILLER(S_famine_trap),

	g_FILLER(S_relocation_trap),

	g_FILLER(S_uninformation_trap),

	g_FILLER(S_intrinsic_loss_trap),
	g_FILLER(S_blood_loss_trap),
	g_FILLER(S_bad_effect_trap),
	g_FILLER(S_multiplying_trap),
	g_FILLER(S_auto_vuln_trap),
	g_FILLER(S_tele_items_trap),
	g_FILLER(S_nastiness_trap),

	g_FILLER(S_recursion_trap),
	g_FILLER(S_respawn_trap),
	g_FILLER(S_warp_zone),
	g_FILLER(S_captcha_trap),
	g_FILLER(S_mind_wipe_trap),
	g_FILLER(S_shuriken_trap),
	g_FILLER(S_shit_pit),
	g_FILLER(S_shaft_trap),
	g_FILLER(S_lock_trap),
	g_FILLER(S_mc_trap),
	g_FILLER(S_farlook_trap),
	g_FILLER(S_gateway_trap),
	g_FILLER(S_growing_trap),
	g_FILLER(S_cooling_trap),
	g_FILLER(S_bar_trap),
	g_FILLER(S_locking_trap),
	g_FILLER(S_air_trap),
	g_FILLER(S_terrain_trap),

	g_FILLER(S_loudspeaker),
	g_FILLER(S_nest_trap),
	g_FILLER(S_bullet_trap),
	g_FILLER(S_paralysis_trap),
	g_FILLER(S_cyanide_trap),
	g_FILLER(S_laser_trap),
	g_FILLER(S_giant_chasm),
	g_FILLER(S_fart_trap),
	g_FILLER(S_confuse_trap),
	g_FILLER(S_stun_trap),
	g_FILLER(S_hallucination_trap),
	g_FILLER(S_petrification_trap),
	g_FILLER(S_numbness_trap),
	g_FILLER(S_freezing_trap),
	g_FILLER(S_burning_trap),
	g_FILLER(S_fear_trap),
	g_FILLER(S_blindness_trap),
	g_FILLER(S_glib_trap),
	g_FILLER(S_slime_trap),
	g_FILLER(S_inertia_trap),
	g_FILLER(S_time_trap),
	g_FILLER(S_lycanthropy_trap),
	g_FILLER(S_unlight_trap),
	g_FILLER(S_elemental_trap),
	g_FILLER(S_escalating_trap),
	g_FILLER(S_negative_trap),
	g_FILLER(S_mana_trap),
	g_FILLER(S_sin_trap),
	g_FILLER(S_destroy_armor_trap),
	g_FILLER(S_divine_anger_trap),
	g_FILLER(S_genetic_trap),
	g_FILLER(S_missingno_trap),
	g_FILLER(S_cancellation_trap),
	g_FILLER(S_hostility_trap),
	g_FILLER(S_boss_trap),
	g_FILLER(S_wishing_trap),
	g_FILLER(S_recurring_amn_trap),
	g_FILLER(S_bigscript_trap),
	g_FILLER(S_bank_trap),
	g_FILLER(S_only_trap),
	g_FILLER(S_map_trap),
	g_FILLER(S_tech_trap),
	g_FILLER(S_disenchant_trap),
	g_FILLER(S_verisiert),
	g_FILLER(S_chaos_trap),
	g_FILLER(S_muteness_trap),
	g_FILLER(S_ntll_trap),
	g_FILLER(S_engraving_trap),
	g_FILLER(S_magic_device_trap),
	g_FILLER(S_book_trap),
	g_FILLER(S_level_trap),
	g_FILLER(S_quiz_trap),

	g_FILLER(S_falling_boulder_trap),
	g_FILLER(S_glass_arrow_trap),
	g_FILLER(S_glass_bolt_trap),
	g_FILLER(S_out_of_magic_trap),
	g_FILLER(S_plasma_trap),
	g_FILLER(S_bomb_trap),
	g_FILLER(S_earthquake_trap),
	g_FILLER(S_noise_trap),
	g_FILLER(S_glue_trap),
	g_FILLER(S_guillotine_trap),
	g_FILLER(S_bisection_trap),
	g_FILLER(S_volt_trap),
	g_FILLER(S_horde_trap),
	g_FILLER(S_immobility_trap),
	g_FILLER(S_green_glyph_trap),
	g_FILLER(S_blue_glyph_trap),
	g_FILLER(S_yellow_glyph_trap),
	g_FILLER(S_orange_glyph_trap),
	g_FILLER(S_black_glyph_trap),
	g_FILLER(S_purple_glyph_trap),

	g_FILLER(S_metabolic_trap),
	g_FILLER(S_no_return_trap),
	g_FILLER(S_ego_trap),
	g_FILLER(S_fast_forward_trap),
	g_FILLER(S_rotten_trap),
	g_FILLER(S_unskilled_trap),
	g_FILLER(S_low_stats_trap),
	g_FILLER(S_training_trap),
	g_FILLER(S_exercise_trap),
	g_FILLER(S_falling_loadstone_trap),
	g_FILLER(S_summon_undead_trap),
	g_FILLER(S_falling_nastystone_trap),

	g_FILLER(S_spined_ball_trap),
	g_FILLER(S_pendulum_trap),
	g_FILLER(S_turn_table),
	g_FILLER(S_scent_trap),
	g_FILLER(S_banana_trap),
	g_FILLER(S_falling_tub_trap),
	g_FILLER(S_alarm),
	g_FILLER(S_caltrops_trap),
	g_FILLER(S_blade_wire),
	g_FILLER(S_magnet_trap),
	g_FILLER(S_slingshot_trap),
	g_FILLER(S_cannon_trap),
	g_FILLER(S_venom_sprinkler),
	g_FILLER(S_fumarole),

	g_FILLER(S_mana_pit),
	g_FILLER(S_elemental_portal),
	g_FILLER(S_girliness_trap),
	g_FILLER(S_fumbling_trap),
	g_FILLER(S_egomonster_trap),
	g_FILLER(S_flooding_trap),
	g_FILLER(S_monster_cube),
	g_FILLER(S_cursed_grave),
	g_FILLER(S_limitation_trap),
	g_FILLER(S_weak_sight_trap),
	g_FILLER(S_random_message_trap),

	g_FILLER(S_desecration_trap),
	g_FILLER(S_starvation_trap),
	g_FILLER(S_dropless_trap),
	g_FILLER(S_low_effect_trap),
	g_FILLER(S_invisible_trap),
	g_FILLER(S_ghost_world_trap),
	g_FILLER(S_dehydration_trap),
	g_FILLER(S_hate_trap),
	g_FILLER(S_spacewars_trap),
	g_FILLER(S_temporary_recursion_trap),
	g_FILLER(S_totter_trap),
	g_FILLER(S_nonintrinsical_trap),
	g_FILLER(S_dropcurse_trap),
	g_FILLER(S_nakedness_trap),
	g_FILLER(S_antilevel_trap),
	g_FILLER(S_ventilator),
	g_FILLER(S_stealer_trap),
	g_FILLER(S_rebellion_trap),
	g_FILLER(S_crap_trap),
	g_FILLER(S_misfire_trap),
	g_FILLER(S_trap_of_walls),

	g_FILLER(S_disconnect_trap),
	g_FILLER(S_interface_screw_trap),
	g_FILLER(S_dimness_trap),
	g_FILLER(S_evil_artifact_trap),
	g_FILLER(S_bossfight_trap),
	g_FILLER(S_entire_level_trap),
	g_FILLER(S_bones_trap),
	g_FILLER(S_rodney_trap),
	g_FILLER(S_autocurse_trap),
	g_FILLER(S_highlevel_trap),
	g_FILLER(S_spell_forgetting_trap),
	g_FILLER(S_sound_effect_trap),

	g_FILLER(S_trap_percents),

	g_FILLER(S_nexus_trap),
	g_FILLER(S_leg_trap),
	g_FILLER(S_artifact_jackpot_trap),
	g_FILLER(S_map_amnesia_trap),
	g_FILLER(S_spreading_trap),
	g_FILLER(S_adjacent_trap),
	g_FILLER(S_superthing_trap),
	g_FILLER(S_current_shaft),
	g_FILLER(S_levitation_trap),
	g_FILLER(S_bowel_cramps_trap),
	g_FILLER(S_unequipping_trap),
	g_FILLER(S_good_artifact_trap),
	g_FILLER(S_gender_trap),
	g_FILLER(S_trap_of_oa),
	g_FILLER(S_sincount_trap),
	g_FILLER(S_beamer_trap),
	g_FILLER(S_level_beamer),
	g_FILLER(S_pet_trap),
	g_FILLER(S_piercing_beam_trap),
	g_FILLER(S_wrenching_trap),
	g_FILLER(S_tracker_trap),
	g_FILLER(S_nurse_trap),
	g_FILLER(S_back_to_start_trap),
	g_FILLER(S_nemesis_trap),
	g_FILLER(S_strew_trap),
	g_FILLER(S_outta_depth_trap),
	g_FILLER(S_punishment_trap),
	g_FILLER(S_boon_trap),
	g_FILLER(S_fountain_trap),
	g_FILLER(S_throne_trap),
	g_FILLER(S_anoxic_pit),
	g_FILLER(S_arabella_speaker),
	g_FILLER(S_femmy_trap),
	g_FILLER(S_madeleine_trap),
	g_FILLER(S_marlena_trap),
	g_FILLER(S_anastasia_trap),
	g_FILLER(S_filler_trap),
	g_FILLER(S_toxic_venom_trap),
	g_FILLER(S_insanity_trap),
	g_FILLER(S_madness_trap),
	g_FILLER(S_jessica_trap),
	g_FILLER(S_solvejg_trap),
	g_FILLER(S_wendy_trap),
	g_FILLER(S_katharina_trap),
	g_FILLER(S_elena_trap),
	g_FILLER(S_thai_trap),
	g_FILLER(S_elif_trap),
	g_FILLER(S_nadja_trap),
	g_FILLER(S_sandra_trap),
	g_FILLER(S_natalje_trap),
	g_FILLER(S_jeanetta_trap),
	g_FILLER(S_yvonne_trap),
	g_FILLER(S_maurah_trap),

	g_FILLER(S_lootcut_trap),
	g_FILLER(S_monster_speed_trap),
	g_FILLER(S_scaling_trap),
	g_FILLER(S_enmity_trap),
	g_FILLER(S_white_spell_trap),
	g_FILLER(S_gray_spell_trap),
	g_FILLER(S_quasar_trap),
	g_FILLER(S_momma_trap),
	g_FILLER(S_horror_trap),
	g_FILLER(S_artificer_trap),
	g_FILLER(S_wereform_trap),
	g_FILLER(S_nonprayer_trap),
	g_FILLER(S_evil_patch_trap),
	g_FILLER(S_hard_mode_trap),
	g_FILLER(S_secret_attack_trap),
	g_FILLER(S_eater_trap),
	g_FILLER(S_covetous_trap),
	g_FILLER(S_not_seen_trap),
	g_FILLER(S_dark_mode_trap),
	g_FILLER(S_antisearch_trap),
	g_FILLER(S_homicide_trap),
	g_FILLER(S_nasty_nation_trap),
	g_FILLER(S_wakeup_call_trap),
	g_FILLER(S_grayout_trap),
	g_FILLER(S_gray_center_trap),
	g_FILLER(S_checkerboard_trap),
	g_FILLER(S_clockwise_spin_trap),
	g_FILLER(S_counterclockwise_trap),
	g_FILLER(S_lag_trap),
	g_FILLER(S_blesscurse_trap),
	g_FILLER(S_delight_trap),
	g_FILLER(S_discharge_trap),
	g_FILLER(S_trashing_trap),
	g_FILLER(S_filtering_trap),
	g_FILLER(S_deformatting_trap),
	g_FILLER(S_flicker_strip_trap),
	g_FILLER(S_undressing_trap),
	g_FILLER(S_hyperbluewall_trap),
	g_FILLER(S_nolite_trap),
	g_FILLER(S_paranoia_trap),
	g_FILLER(S_fleecescript_trap),
	g_FILLER(S_interrupt_trap),
	g_FILLER(S_dustbin_trap),
	g_FILLER(S_mana_battery_trap),
	g_FILLER(S_monsterfingers_trap),
	g_FILLER(S_miscast_trap),
	g_FILLER(S_message_suppression_trap),
	g_FILLER(S_stuck_announcement_trap),
	g_FILLER(S_bloodthirsty_trap),
	g_FILLER(S_maximum_damage_trap),
	g_FILLER(S_latency_trap),
	g_FILLER(S_starlit_trap),
	g_FILLER(S_knowledge_trap),
	g_FILLER(S_highscore_trap),
	g_FILLER(S_pink_spell_trap),
	g_FILLER(S_green_spell_trap),
	g_FILLER(S_evc_trap),
	g_FILLER(S_underlayer_trap),
	g_FILLER(S_damage_meter_trap),
	g_FILLER(S_arbitrary_weight_trap),
	g_FILLER(S_fucked_info_trap),
	g_FILLER(S_black_spell_trap),
	g_FILLER(S_cyan_spell_trap),
	g_FILLER(S_heap_trap),
	g_FILLER(S_blue_spell_trap),
	g_FILLER(S_tron_trap),
	g_FILLER(S_red_spell_trap),
	g_FILLER(S_too_heavy_trap),
	g_FILLER(S_elongation_trap),
	g_FILLER(S_wrapover_trap),
	g_FILLER(S_destruction_trap),
	g_FILLER(S_melee_prefix_trap),
	g_FILLER(S_automore_trap),
	g_FILLER(S_unfair_attack_trap),

	g_FILLER(S_kop_cube),
	g_FILLER(S_boss_spawner),
	g_FILLER(S_contamination_trap),

	g_FILLER(S_evil_heel_trap),
	g_FILLER(S_bad_equipment_trap),
	g_FILLER(S_tempoconflict_trap),
	g_FILLER(S_tempohunger_trap),
	g_FILLER(S_teleportitis_trap),
	g_FILLER(S_polymorphitis_trap),
	g_FILLER(S_premature_death_trap),
	g_FILLER(S_lasting_amnesia_trap),
	g_FILLER(S_ragnarok_trap),
	g_FILLER(S_single_disenchant_trap),
	g_FILLER(S_acid_pit),

	g_FILLER(S_severe_disenchant_trap),
	g_FILLER(S_pain_trap),
	g_FILLER(S_trembling_trap),
	g_FILLER(S_techcap_trap),
	g_FILLER(S_spell_memory_trap),
	g_FILLER(S_skill_reduction_trap),
	g_FILLER(S_skillcap_trap),
	g_FILLER(S_permanent_stat_damage_trap),

	g_FILLER(S_orange_spell_trap),
	g_FILLER(S_violet_spell_trap),
	g_FILLER(S_trap_of_longing),
	g_FILLER(S_cursed_part_trap),
	g_FILLER(S_quaversal_trap),
	g_FILLER(S_appearance_shuffling_trap),
	g_FILLER(S_brown_spell_trap),
	g_FILLER(S_choiceless_trap),
	g_FILLER(S_goldspell_trap),
	g_FILLER(S_deprovement_trap),
	g_FILLER(S_initialization_trap),
	g_FILLER(S_gushlush_trap),
	g_FILLER(S_soiltype_trap),
	g_FILLER(S_dangerous_terrain_trap),
	g_FILLER(S_fallout_trap),
	g_FILLER(S_mojibake_trap),
	g_FILLER(S_gravation_trap),
	g_FILLER(S_uncalled_trap),
	g_FILLER(S_exploding_dice_trap),
	g_FILLER(S_permacurse_trap),
	g_FILLER(S_shrouded_identity_trap),
	g_FILLER(S_feeler_gauges_trap),
	g_FILLER(S_long_screwup_trap),
	g_FILLER(S_wing_yellow_changer),
	g_FILLER(S_life_saving_trap),
	g_FILLER(S_curseuse_trap),
	g_FILLER(S_cut_nutrition_trap),
	g_FILLER(S_skill_loss_trap),
	g_FILLER(S_autopilot_trap),
	g_FILLER(S_force_trap),
	g_FILLER(S_monster_glyph_trap),
	g_FILLER(S_changing_directive_trap),
	g_FILLER(S_container_kaboom_trap),
	g_FILLER(S_steal_degrade_trap),
	g_FILLER(S_left_inventory_trap),
	g_FILLER(S_fluctuating_speed_trap),
	g_FILLER(S_tarmustrokingnora_trap),
	g_FILLER(S_failure_trap),
	g_FILLER(S_bright_cyan_spell_trap),
	g_FILLER(S_frequentation_spawn_trap),
	g_FILLER(S_pet_ai_trap),
	g_FILLER(S_satan_trap),
	g_FILLER(S_rememberance_trap),
	g_FILLER(S_pokelie_trap),
	g_FILLER(S_autopickup_trap),
	g_FILLER(S_dywypi_trap),
	g_FILLER(S_silver_spell_trap),
	g_FILLER(S_metal_spell_trap),
	g_FILLER(S_platinum_spell_trap),
	g_FILLER(S_manler_trap),
	g_FILLER(S_doorning_trap),
	g_FILLER(S_nownsible_trap),
	g_FILLER(S_elm_street_trap),
	g_FILLER(S_monnoise_trap),
	g_FILLER(S_rang_call_trap),
	g_FILLER(S_recurring_spell_loss_trap),
	g_FILLER(S_antitraining_trap),
	g_FILLER(S_techout_trap),
	g_FILLER(S_stat_decay_trap),
	g_FILLER(S_movemork_trap),

	g_FILLER(S_hybrid_trap),
	g_FILLER(S_shapechange_trap),
	g_FILLER(S_meltem_trap),
	g_FILLER(S_miguc_trap),
	g_FILLER(S_directive_trap),
	g_FILLER(S_satatue_trap),
	g_FILLER(S_farting_web),
	g_FILLER(S_cataclysm_trap),
	g_FILLER(S_data_delete_trap),
	g_FILLER(S_elder_tentacling_trap),
	g_FILLER(S_footerer_trap),

	g_FILLER(S_bad_part_trap),
	g_FILLER(S_completely_bad_part_trap),
	g_FILLER(S_evil_variant_trap),

	g_FILLER(S_grave_wall_trap),
	g_FILLER(S_tunnel_trap),
	g_FILLER(S_farmland_trap),
	g_FILLER(S_mountain_trap),
	g_FILLER(S_water_tunnel_trap),
	g_FILLER(S_crystal_flood_trap),
	g_FILLER(S_moorland_trap),
	g_FILLER(S_urine_trap),
	g_FILLER(S_shifting_sand_trap),
	g_FILLER(S_styx_trap),
	g_FILLER(S_pentagram_trap),
	g_FILLER(S_snow_trap),
	g_FILLER(S_ash_trap),
	g_FILLER(S_sand_trap),
	g_FILLER(S_pavement_trap),
	g_FILLER(S_highway_trap),
	g_FILLER(S_grassland_trap),
	g_FILLER(S_nether_mist_trap),
	g_FILLER(S_stalactite_trap),
	g_FILLER(S_cryptfloor_trap),
	g_FILLER(S_bubble_trap),
	g_FILLER(S_rain_cloud_trap),
	g_FILLER(S_item_nastification_trap),
	g_FILLER(S_sanity_increase_trap),
	g_FILLER(S_psi_trap),
	g_FILLER(S_gay_trap),
	g_FILLER(S_sarah_trap),
	g_FILLER(S_claudia_trap),
	g_FILLER(S_ludgera_trap),
	g_FILLER(S_kati_trap),
	g_FILLER(S_sanity_treble_trap),
	g_FILLER(S_stat_decrease_trap),
	g_FILLER(S_simeout_trap),
	g_FILLER(S_spressing_trap),
	g_FILLER(S_nelly_trap),
	g_FILLER(S_eveline_trap),
	g_FILLER(S_karin_trap),
	g_FILLER(S_juen_trap),
	g_FILLER(S_kristina_trap),
	g_FILLER(S_lou_trap),
	g_FILLER(S_almut_trap),
	g_FILLER(S_julietta_trap),
	g_FILLER(S_arabella_trap),
	g_FILLER(S_kristin_trap),
	g_FILLER(S_anna_trap),
	g_FILLER(S_ruea_trap),
	g_FILLER(S_dora_trap),
	g_FILLER(S_marike_trap),
	g_FILLER(S_jette_trap),
	g_FILLER(S_ina_trap),
	g_FILLER(S_sing_trap),
	g_FILLER(S_victoria_trap),
	g_FILLER(S_melissa_trap),
	g_FILLER(S_anita_trap),
	g_FILLER(S_henrietta_trap),
	g_FILLER(S_verena_trap),
	g_FILLER(S_annemarie_trap),
	g_FILLER(S_jil_trap),
	g_FILLER(S_jana_trap),
	g_FILLER(S_katrin_trap),
	g_FILLER(S_gudrun_trap),
	g_FILLER(S_ella_trap),
	g_FILLER(S_manuela_trap),
	g_FILLER(S_jennifer_trap),
	g_FILLER(S_patricia_trap),
	g_FILLER(S_antje_trap),
	g_FILLER(S_antje_x_trap),
	g_FILLER(S_kerstin_trap),
	g_FILLER(S_laura_trap),
	g_FILLER(S_larissa_trap),
	g_FILLER(S_nora_trap),
	g_FILLER(S_natalia_trap),
	g_FILLER(S_susanne_trap),
	g_FILLER(S_lisa_trap),
	g_FILLER(S_bridghitte_trap),
	g_FILLER(S_julia_trap),
	g_FILLER(S_nicole_trap),
	g_FILLER(S_rita_trap),
	g_FILLER(S_janina_trap),
	g_FILLER(S_rosa_trap),
	g_FILLER(S_ksenia_trap),
	g_FILLER(S_lydia_trap),
	g_FILLER(S_conny_trap),
	g_FILLER(S_katia_trap),
	g_FILLER(S_mariya_trap),
	g_FILLER(S_elise_trap),
	g_FILLER(S_ronja_trap),
	g_FILLER(S_ariane_trap),
	g_FILLER(S_johanna_trap),
	g_FILLER(S_inge_trap),

	g_FILLER(S_wall_trap),
	g_FILLER(S_monster_generator),
	g_FILLER(S_potion_dispenser),
	g_FILLER(S_spacewars_spawn_trap),
	g_FILLER(S_tv_tropes_trap),
	g_FILLER(S_symbiote_trap),
	g_FILLER(S_kill_symbiote_trap),
	g_FILLER(S_symbiote_replacement_trap),
	g_FILLER(S_shutdown_trap),
	g_FILLER(S_corona_trap),
	g_FILLER(S_unproofing_trap),
	g_FILLER(S_visibility_trap),
	g_FILLER(S_feminism_stone_trap),
	g_FILLER(S_branch_teleporter),
	g_FILLER(S_branch_beamer),
	g_FILLER(S_poison_arrow_trap),
	g_FILLER(S_poison_bolt_trap),
	g_FILLER(S_mace_trap),
	g_FILLER(S_shueft_trap),
	g_FILLER(S_moth_larvae_trap),
	g_FILLER(S_worthiness_trap),
	g_FILLER(S_conduct_trap),
	g_FILLER(S_strikethrough_trap),
	g_FILLER(S_multiple_gather_trap),
	g_FILLER(S_vivisection_trap),
	g_FILLER(S_instafeminism_trap),
	g_FILLER(S_instanasty_trap),
	g_FILLER(S_skill_point_loss_trap),
	g_FILLER(S_perfect_match_trap),
	g_FILLER(S_dumbie_lightsaber_trap),
	g_FILLER(S_wrong_stairs),
	g_FILLER(S_techstop_trap),
	g_FILLER(S_amnesia_switch_trap),
	g_FILLER(S_skill_swap_trap),
	g_FILLER(S_skill_upordown_trap),
	g_FILLER(S_skill_randomize_trap),

	g_FILLER(S_skill_multiply_trap),
	g_FILLER(S_trapwalk_trap),
	g_FILLER(S_cluster_trap),
	g_FILLER(S_field_trap),
	g_FILLER(S_monicide_trap),
	g_FILLER(S_trap_creation_trap),
	g_FILLER(S_leold_trap),
	g_FILLER(S_animeband_trap),
	g_FILLER(S_perfume_trap),
	g_FILLER(S_court_trap),
	g_FILLER(S_elder_scrolls_trap),
	g_FILLER(S_joke_trap),
	g_FILLER(S_dungeon_lords_trap),
	g_FILLER(S_fortytwo_trap),
	g_FILLER(S_randomize_trap),
	g_FILLER(S_evilroom_trap),
	g_FILLER(S_aoe_trap),
	g_FILLER(S_elona_trap),
	g_FILLER(S_religion_trap),
	g_FILLER(S_steamband_trap),
	g_FILLER(S_hardcore_trap),
	g_FILLER(S_machine_trap),
	g_FILLER(S_bee_trap),
	g_FILLER(S_migo_trap),
	g_FILLER(S_angband_trap),
	g_FILLER(S_dnethack_trap),
	g_FILLER(S_evil_spawn_trap),
	g_FILLER(S_shoe_trap),
	g_FILLER(S_inside_trap),
	g_FILLER(S_doom_trap),
	g_FILLER(S_military_trap),
	g_FILLER(S_illusion_trap),
	g_FILLER(S_diablo_trap),

	g_FILLER(S_giant_explorer_trap),
	g_FILLER(S_trapwarp_trap),
	g_FILLER(S_yawm_trap),
	g_FILLER(S_cradle_of_chaos_trap),
	g_FILLER(S_tez_trap),
	g_FILLER(S_enthu_trap),
	g_FILLER(S_mikra_trap),
	g_FILLER(S_gots_too_good_trap),
	g_FILLER(S_killer_room_trap),
	g_FILLER(S_no_fun_walls_trap),

	g_FILLER(S_ruth_trap),
	g_FILLER(S_magdalena_trap),
	g_FILLER(S_marleen_trap),
	g_FILLER(S_klara_trap),
	g_FILLER(S_friederike_trap),
	g_FILLER(S_naomi_trap),
	g_FILLER(S_ute_trap),
	g_FILLER(S_jasieen_trap),
	g_FILLER(S_yasaman_trap),
	g_FILLER(S_may_britt_trap),

	g_FILLER(S_calling_out_trap),
	g_FILLER(S_field_break_trap),
	g_FILLER(S_tenth_trap),
	g_FILLER(S_debt_trap),
	g_FILLER(S_inversion_trap),
	g_FILLER(S_wince_trap),
	g_FILLER(S_u_have_been_trap),
	g_FILLER(S_really_bad_trap),
	g_FILLER(S_covid_trap),
	g_FILLER(S_artiblast_trap),

	g_FILLER(S_persistent_fart_trap),
	g_FILLER(S_attacking_heel_trap),
	g_FILLER(S_trap_teleporter),
	g_FILLER(S_alignment_trash_trap),
	g_FILLER(S_reshuffle_trap),
	g_FILLER(S_musehand_trap),
	g_FILLER(S_dogside_trap),
	g_FILLER(S_bankrupt_trap),
	g_FILLER(S_fillup_trap),
	g_FILLER(S_airstrike_trap),
	g_FILLER(S_dynamite_trap),
	g_FILLER(S_malevolence_trap),
	g_FILLER(S_leaflet_trap),
	g_FILLER(S_tentadeep_trap),
	g_FILLER(S_stathalf_trap),
	g_FILLER(S_cutstat_trap),
	g_FILLER(S_rare_spawn_trap),
	g_FILLER(S_you_are_an_idiot_trap),
	g_FILLER(S_nastycurse_trap),
	g_FILLER(S_repeating_nastycurse_trap),

	g_FILLER(S_nadine_trap),
	g_FILLER(S_luisa_trap),
	g_FILLER(S_irina_trap),
	g_FILLER(S_liselotte_trap),
	g_FILLER(S_greta_trap),
	g_FILLER(S_jane_trap),
	g_FILLER(S_sue_lyn_trap),
	g_FILLER(S_charlotte_trap),
	g_FILLER(S_hannah_trap),
	g_FILLER(S_little_marie_trap),

	g_FILLER(S_real_lie_trap),
	g_FILLER(S_escape_past_trap),
	g_FILLER(S_pethate_trap),
	g_FILLER(S_pet_lashout_trap),
	g_FILLER(S_petstarve_trap),
	g_FILLER(S_petscrew_trap),
	g_FILLER(S_tech_loss_trap),
	g_FILLER(S_proofloss_trap),
	g_FILLER(S_un_invis_trap),
	g_FILLER(S_detectation_trap),

	g_FILLER(S_option_trap),
	g_FILLER(S_miscolor_trap),
	g_FILLER(S_one_rainbow_trap),
	g_FILLER(S_colorshift_trap),
	g_FILLER(S_top_line_trap),
	g_FILLER(S_caps_trap),
	g_FILLER(S_un_knowledge_trap),
	g_FILLER(S_darkhance_trap),
	g_FILLER(S_dschueueuet_trap),
	g_FILLER(S_nopeskill_trap),
	g_FILLER(S_fuckfuckfuck_trap),

	g_FILLER(S_burden_trap),
	g_FILLER(S_magic_vacuum_trap),

	g_FILLER(S_tanja_trap),
	g_FILLER(S_sonja_trap),
	g_FILLER(S_rhea_trap),
	g_FILLER(S_lara_trap),

	g_FILLER(S_fuck_over_trap),

	g_FILLER(S_dagger_trap),
	g_FILLER(S_razor_trap),
	g_FILLER(S_phosgene_trap),
	g_FILLER(S_chloroform_trap),
	g_FILLER(S_corrosion_trap),
	g_FILLER(S_flame_trap),
	g_FILLER(S_wither_trap),
	g_FILLER(S_phaseport_trap),
	g_FILLER(S_phasebeam_trap),
	g_FILLER(S_vulnerate_trap),

	g_FILLER(S_epvi_trap),

	g_FILLER(S_sabrina_trap),

	g_FILLER(S_aefde_trap),

	g_FILLER(S_falling_rock_cold_trap),
	g_FILLER(S_return_trap),
	g_FILLER(S_intrinsic_steal_trap),
	g_FILLER(S_score_axe_trap),
	g_FILLER(S_score_drain_trap),
	g_FILLER(S_single_unidentify_trap),
	g_FILLER(S_hypoxic_pit),
	g_FILLER(S_unlucky_trap),
	g_FILLER(S_alignment_reduction_trap),
	g_FILLER(S_malignant_trap),
	g_FILLER(S_stat_damage_trap),
	g_FILLER(S_half_memory_trap),
	g_FILLER(S_half_training_trap),
	g_FILLER(S_debuff_trap),
	g_FILLER(S_trip_once_trap),
	g_FILLER(S_narcolepsy_trap),
	g_FILLER(S_martial_arts_trap),
	g_FILLER(S_mean_burden_trap),
	g_FILLER(S_carrcap_trap),
	g_FILLER(S_umeng_trap),

	g_FILLER(S_spell_cooldown_trap),
	g_FILLER(S_turbodull_trap),
	g_FILLER(S_reddam_trap),
	g_FILLER(S_redinc_trap),
	g_FILLER(S_mulch_trap),
	g_FILLER(S_inaccuracy_trap),
	g_FILLER(S_monster_multishot_trap),
	g_FILLER(S_have_a_pin_trap),
	g_FILLER(S_costly_failure_trap),
	g_FILLER(S_inventory_size_trap),

	g_FILLER(S_spike_trap),
	g_FILLER(S_paper_arrow_trap),
	g_FILLER(S_knife_trap),
	g_FILLER(S_summon_monster_trap),
	g_FILLER(S_red_damage_trap),
	g_FILLER(S_green_damage_trap),
	g_FILLER(S_yellow_damage_trap),
	g_FILLER(S_white_damage_trap),
	g_FILLER(S_blue_damage_trap),
	g_FILLER(S_multi_debuff_trap),

	g_FILLER(S_magic_damage_trap),

	g_FILLER(S_wild_weather_trap),

	g_FILLER(S_timerun_trap),
	0xf8,	/* S_vbeam:	meta-x, vertical rule */
	0xf1,	/* S_hbeam:	meta-q, horizontal rule */
	g_FILLER(S_lslant),
	g_FILLER(S_rslant),
	g_FILLER(S_digbeam),
	g_FILLER(S_flashbeam),
/*70*/	g_FILLER(S_boomleft),
	g_FILLER(S_boomright),
	g_FILLER(S_ss1),
	g_FILLER(S_ss2),
	g_FILLER(S_ss3),
	g_FILLER(S_ss4),
	g_FILLER(S_sw_tl),
	0xef,	/* S_sw_tc:	meta-o, high horizontal line */
	g_FILLER(S_sw_tr),
	0xf8,	/* S_sw_ml:	meta-x, vertical rule */
/*80*/	0xf8,	/* S_sw_mr:	meta-x, vertical rule */
	g_FILLER(S_sw_bl),
	0xf3,	/* S_sw_bc:	meta-s, low horizontal line */
	g_FILLER(S_sw_br),
	g_FILLER(S_explode1),
	0xef,	/* S_explode2:	meta-o, high horizontal line */
	g_FILLER(S_explode3),
	0xf8,	/* S_explode4:	meta-x, vertical rule */
	g_FILLER(S_explode5),
	0xf8,	/* S_explode6:	meta-x, vertical rule */
/*90*/	g_FILLER(S_explode7),
	0xf3,	/* S_explode8:	meta-s, low horizontal line */
	g_FILLER(S_explode9),
};

#ifdef MAC_GRAPHICS_ENV
static uchar mac_graphics[MAXPCHARS] = {
/* 0*/	g_FILLER(S_stone),
	0xba,	/* S_vwall */
	0xcd,	/* S_hwall */
	0xc9,	/* S_tlcorn */
	0xbb,	/* S_trcorn */
	0xc8,	/* S_blcorn */
	0xbc,	/* S_brcorn */
	0xce,	/* S_crwall */
	0xca,	/* S_tuwall */
	0xcb,	/* S_tdwall */
/*10*/	0xb9,	/* S_tlwall */
	0xcc,	/* S_trwall */
	g_FILLER(S_rockwall),	/* rockwall */
	g_FILLER(S_gravewall),	/* gravewall */
	g_FILLER(S_tunnelwall),	/* tunnelwall */
	0xb0,	/* S_ndoor */
	0xee,	/* S_vodoor */
	0xee,	/* S_hodoor */
	0xef,	/* S_vcdoor */
	0xef,	/* S_hcdoor */
/*20*/	0xf0,	/* S_bars:	equivalency symbol */

	0xf1,	/* S_tree:	plus-or-minus */
	0xf1,	/* S_farmland:	plus or minus symbol */
	g_FILLER(S_mountain),	/* mountain */
	g_FILLER(S_Room),
	g_FILLER(S_stone), /* S_darkroom */
	0xB0,	/* S_corr */
	g_FILLER(S_litcorr),
	g_FILLER(S_upstair),
	g_FILLER(S_dnstair),
/*30*/	g_FILLER(S_upladder),
	g_FILLER(S_dnladder),
	g_FILLER(S_altar),
	0xef,	/* S_grave:	same as open door */
	g_FILLER(S_throne),
	g_FILLER(S_sink),
	g_FILLER(S_toilet),
	g_FILLER(S_fountain),
	0xe0,	/* S_pool */
	g_FILLER(S_ice),
/*40*/	g_FILLER(S_lava),
	g_FILLER(S_vodbridge),
	g_FILLER(S_hodbridge),
	g_FILLER(S_vcdbridge),
	g_FILLER(S_hcdbridge),
	g_FILLER(S_air),
	g_FILLER(S_cloud),
	g_FILLER(S_dungwall),	/* dungwall */
	0xf7,	/* S_water:	meta-w, approx. equals */
	g_FILLER(S_watertunnel),	/* watertunnel */
/*50*/	0xf7,	/* S_crystalwater:	meta-w, approx. equals */
	0xf7, /* moorland */
	0xf7, /* urinelake */
	0xf7, /* shiftingsand */
	0xf7, /* styxriver */
	g_FILLER(S_well),
	g_FILLER(S_poisonedwell),
	g_FILLER(S_wagon),
	g_FILLER(S_burningwagon),
	g_FILLER(S_woodentable),
/*60*/	g_FILLER(S_strawmattress),
	g_FILLER(S_snow),
	g_FILLER(S_ash),
	g_FILLER(S_sand),
	g_FILLER(S_pavedfloor),
	g_FILLER(S_highway),
	g_FILLER(S_grass),
	g_FILLER(S_nethermist),
	g_FILLER(S_stalactite),
	g_FILLER(S_cryptfloor),
/*70*/	g_FILLER(S_bubbles),
	g_FILLER(S_raincloud),
	g_FILLER(S_pentagram),
	g_FILLER(S_carvedbed),
	g_FILLER(S_grayglyph),
	g_FILLER(S_water),
	g_FILLER(S_arrow_trap),
	g_FILLER(S_dart_trap),
	g_FILLER(S_falling_rock_trap),
	g_FILLER(S_squeaky_board),
	g_FILLER(S_bear_trap),
	g_FILLER(S_land_mine),
	g_FILLER(S_rolling_boulder_trap),
	g_FILLER(S_sleeping_gas_trap),
	g_FILLER(S_rust_trap),
	g_FILLER(S_fire_trap),
	g_FILLER(S_pit),
	g_FILLER(S_spiked_pit),
	g_FILLER(S_hole),
	g_FILLER(S_trap_door),
	g_FILLER(S_teleportation_trap),
	g_FILLER(S_level_teleporter),
	g_FILLER(S_magic_portal),
	g_FILLER(S_web),
	g_FILLER(S_statue_trap),
	g_FILLER(S_magic_trap),
	g_FILLER(S_anti_magic_trap),
	g_FILLER(S_polymorph_trap),
	g_FILLER(S_ice_trap),
	g_FILLER(S_spear_trap),
	g_FILLER(S_falling_rocks_trap),
	g_FILLER(S_magic_beam_trap),
	g_FILLER(S_pile_of_shit),
	g_FILLER(S_animation_trap),
	g_FILLER(S_glyph_of_warding),
	g_FILLER(S_scything_blade),
	g_FILLER(S_bolt_trap),
	g_FILLER(S_acid_pool),
	g_FILLER(S_water_pool),
	g_FILLER(S_poison_gas_trap),
	g_FILLER(S_slow_gas_trap),
	g_FILLER(S_shock_trap),
	g_FILLER(S_rmb_loss_trap),
	g_FILLER(S_display_trap),
	g_FILLER(S_spell_loss_trap),
	g_FILLER(S_yellow_spell_trap),
	g_FILLER(S_auto_destruct_trap),
	g_FILLER(S_memory_trap),
	g_FILLER(S_inventory_trap),
	g_FILLER(S_black_ng_wall_trap),
	g_FILLER(S_menu_trap),
	g_FILLER(S_speed_trap),
	g_FILLER(S_superscroller_trap),
	g_FILLER(S_active_superscroller_trap),
	g_FILLER(S_swarm_trap),
	g_FILLER(S_automatic_switcher),
	g_FILLER(S_unknown_trap),
	g_FILLER(S_heel_trap),
	g_FILLER(S_vuln_trap),

	g_FILLER(S_lava_trap),
	g_FILLER(S_flood_trap),
	g_FILLER(S_drain_trap),
	g_FILLER(S_free_hand_trap),
	g_FILLER(S_disintegration_trap),
	g_FILLER(S_unidentify_trap),
	g_FILLER(S_thirst_trap),
	g_FILLER(S_luck_trap),
	g_FILLER(S_shades_of_grey_trap),
	g_FILLER(S_item_telep_trap),
	g_FILLER(S_gravity_trap),
	g_FILLER(S_death_trap),
	g_FILLER(S_stone_to_flesh_trap),
	g_FILLER(S_quicksand_trap),
	g_FILLER(S_faint_trap),
	g_FILLER(S_curse_trap),
	g_FILLER(S_difficulty_trap),
	g_FILLER(S_sound_trap),
	g_FILLER(S_caster_trap),
	g_FILLER(S_weakness_trap),
	g_FILLER(S_rot_thirteen_trap),
	g_FILLER(S_bishop_trap),
	g_FILLER(S_confusion_trap),
	g_FILLER(S_nupesell_trap),
	g_FILLER(S_drop_trap),
	g_FILLER(S_dstw_trap),
	g_FILLER(S_status_trap),
	g_FILLER(S_alignment_trap),
	g_FILLER(S_stairs_trap),

	g_FILLER(S_pestilence_trap),
	g_FILLER(S_famine_trap),

	g_FILLER(S_relocation_trap),

	g_FILLER(S_uninformation_trap),

	g_FILLER(S_intrinsic_loss_trap),
	g_FILLER(S_blood_loss_trap),
	g_FILLER(S_bad_effect_trap),
	g_FILLER(S_multiplying_trap),
	g_FILLER(S_auto_vuln_trap),
	g_FILLER(S_tele_items_trap),
	g_FILLER(S_nastiness_trap),

	g_FILLER(S_recursion_trap),
	g_FILLER(S_respawn_trap),
	g_FILLER(S_warp_zone),
	g_FILLER(S_captcha_trap),
	g_FILLER(S_mind_wipe_trap),
	g_FILLER(S_shuriken_trap),
	g_FILLER(S_shit_pit),
	g_FILLER(S_shaft_trap),
	g_FILLER(S_lock_trap),
	g_FILLER(S_mc_trap),
	g_FILLER(S_farlook_trap),
	g_FILLER(S_gateway_trap),
	g_FILLER(S_growing_trap),
	g_FILLER(S_cooling_trap),
	g_FILLER(S_bar_trap),
	g_FILLER(S_locking_trap),
	g_FILLER(S_air_trap),
	g_FILLER(S_terrain_trap),

	g_FILLER(S_loudspeaker),
	g_FILLER(S_nest_trap),
	g_FILLER(S_bullet_trap),
	g_FILLER(S_paralysis_trap),
	g_FILLER(S_cyanide_trap),
	g_FILLER(S_laser_trap),
	g_FILLER(S_giant_chasm),
	g_FILLER(S_fart_trap),
	g_FILLER(S_confuse_trap),
	g_FILLER(S_stun_trap),
	g_FILLER(S_hallucination_trap),
	g_FILLER(S_petrification_trap),
	g_FILLER(S_numbness_trap),
	g_FILLER(S_freezing_trap),
	g_FILLER(S_burning_trap),
	g_FILLER(S_fear_trap),
	g_FILLER(S_blindness_trap),
	g_FILLER(S_glib_trap),
	g_FILLER(S_slime_trap),
	g_FILLER(S_inertia_trap),
	g_FILLER(S_time_trap),
	g_FILLER(S_lycanthropy_trap),
	g_FILLER(S_unlight_trap),
	g_FILLER(S_elemental_trap),
	g_FILLER(S_escalating_trap),
	g_FILLER(S_negative_trap),
	g_FILLER(S_mana_trap),
	g_FILLER(S_sin_trap),
	g_FILLER(S_destroy_armor_trap),
	g_FILLER(S_divine_anger_trap),
	g_FILLER(S_genetic_trap),
	g_FILLER(S_missingno_trap),
	g_FILLER(S_cancellation_trap),
	g_FILLER(S_hostility_trap),
	g_FILLER(S_boss_trap),
	g_FILLER(S_wishing_trap),
	g_FILLER(S_recurring_amn_trap),
	g_FILLER(S_bigscript_trap),
	g_FILLER(S_bank_trap),
	g_FILLER(S_only_trap),
	g_FILLER(S_map_trap),
	g_FILLER(S_tech_trap),
	g_FILLER(S_disenchant_trap),
	g_FILLER(S_verisiert),
	g_FILLER(S_chaos_trap),
	g_FILLER(S_muteness_trap),
	g_FILLER(S_ntll_trap),
	g_FILLER(S_engraving_trap),
	g_FILLER(S_magic_device_trap),
	g_FILLER(S_book_trap),
	g_FILLER(S_level_trap),
	g_FILLER(S_quiz_trap),

	g_FILLER(S_falling_boulder_trap),
	g_FILLER(S_glass_arrow_trap),
	g_FILLER(S_glass_bolt_trap),
	g_FILLER(S_out_of_magic_trap),
	g_FILLER(S_plasma_trap),
	g_FILLER(S_bomb_trap),
	g_FILLER(S_earthquake_trap),
	g_FILLER(S_noise_trap),
	g_FILLER(S_glue_trap),
	g_FILLER(S_guillotine_trap),
	g_FILLER(S_bisection_trap),
	g_FILLER(S_volt_trap),
	g_FILLER(S_horde_trap),
	g_FILLER(S_immobility_trap),
	g_FILLER(S_green_glyph_trap),
	g_FILLER(S_blue_glyph_trap),
	g_FILLER(S_yellow_glyph_trap),
	g_FILLER(S_orange_glyph_trap),
	g_FILLER(S_black_glyph_trap),
	g_FILLER(S_purple_glyph_trap),

	g_FILLER(S_metabolic_trap),
	g_FILLER(S_no_return_trap),
	g_FILLER(S_ego_trap),
	g_FILLER(S_fast_forward_trap),
	g_FILLER(S_rotten_trap),
	g_FILLER(S_unskilled_trap),
	g_FILLER(S_low_stats_trap),
	g_FILLER(S_training_trap),
	g_FILLER(S_exercise_trap),
	g_FILLER(S_falling_loadstone_trap),
	g_FILLER(S_summon_undead_trap),
	g_FILLER(S_falling_nastystone_trap),

	g_FILLER(S_spined_ball_trap),
	g_FILLER(S_pendulum_trap),
	g_FILLER(S_turn_table),
	g_FILLER(S_scent_trap),
	g_FILLER(S_banana_trap),
	g_FILLER(S_falling_tub_trap),
	g_FILLER(S_alarm),
	g_FILLER(S_caltrops_trap),
	g_FILLER(S_blade_wire),
	g_FILLER(S_magnet_trap),
	g_FILLER(S_slingshot_trap),
	g_FILLER(S_cannon_trap),
	g_FILLER(S_venom_sprinkler),
	g_FILLER(S_fumarole),

	g_FILLER(S_mana_pit),
	g_FILLER(S_elemental_portal),
	g_FILLER(S_girliness_trap),
	g_FILLER(S_fumbling_trap),
	g_FILLER(S_egomonster_trap),
	g_FILLER(S_flooding_trap),
	g_FILLER(S_monster_cube),
	g_FILLER(S_cursed_grave),
	g_FILLER(S_limitation_trap),
	g_FILLER(S_weak_sight_trap),
	g_FILLER(S_random_message_trap),

	g_FILLER(S_desecration_trap),
	g_FILLER(S_starvation_trap),
	g_FILLER(S_dropless_trap),
	g_FILLER(S_low_effect_trap),
	g_FILLER(S_invisible_trap),
	g_FILLER(S_ghost_world_trap),
	g_FILLER(S_dehydration_trap),
	g_FILLER(S_hate_trap),
	g_FILLER(S_spacewars_trap),
	g_FILLER(S_temporary_recursion_trap),
	g_FILLER(S_totter_trap),
	g_FILLER(S_nonintrinsical_trap),
	g_FILLER(S_dropcurse_trap),
	g_FILLER(S_nakedness_trap),
	g_FILLER(S_antilevel_trap),
	g_FILLER(S_ventilator),
	g_FILLER(S_stealer_trap),
	g_FILLER(S_rebellion_trap),
	g_FILLER(S_crap_trap),
	g_FILLER(S_misfire_trap),
	g_FILLER(S_trap_of_walls),

	g_FILLER(S_disconnect_trap),
	g_FILLER(S_interface_screw_trap),
	g_FILLER(S_dimness_trap),
	g_FILLER(S_evil_artifact_trap),
	g_FILLER(S_bossfight_trap),
	g_FILLER(S_entire_level_trap),
	g_FILLER(S_bones_trap),
	g_FILLER(S_rodney_trap),
	g_FILLER(S_autocurse_trap),
	g_FILLER(S_highlevel_trap),
	g_FILLER(S_spell_forgetting_trap),
	g_FILLER(S_sound_effect_trap),

	g_FILLER(S_trap_percents),

	g_FILLER(S_nexus_trap),
	g_FILLER(S_leg_trap),
	g_FILLER(S_artifact_jackpot_trap),
	g_FILLER(S_map_amnesia_trap),
	g_FILLER(S_spreading_trap),
	g_FILLER(S_adjacent_trap),
	g_FILLER(S_superthing_trap),
	g_FILLER(S_current_shaft),
	g_FILLER(S_levitation_trap),
	g_FILLER(S_bowel_cramps_trap),
	g_FILLER(S_unequipping_trap),
	g_FILLER(S_good_artifact_trap),
	g_FILLER(S_gender_trap),
	g_FILLER(S_trap_of_oa),
	g_FILLER(S_sincount_trap),
	g_FILLER(S_beamer_trap),
	g_FILLER(S_level_beamer),
	g_FILLER(S_pet_trap),
	g_FILLER(S_piercing_beam_trap),
	g_FILLER(S_wrenching_trap),
	g_FILLER(S_tracker_trap),
	g_FILLER(S_nurse_trap),
	g_FILLER(S_back_to_start_trap),
	g_FILLER(S_nemesis_trap),
	g_FILLER(S_strew_trap),
	g_FILLER(S_outta_depth_trap),
	g_FILLER(S_punishment_trap),
	g_FILLER(S_boon_trap),
	g_FILLER(S_fountain_trap),
	g_FILLER(S_throne_trap),
	g_FILLER(S_anoxic_pit),
	g_FILLER(S_arabella_speaker),
	g_FILLER(S_femmy_trap),
	g_FILLER(S_madeleine_trap),
	g_FILLER(S_marlena_trap),
	g_FILLER(S_anastasia_trap),
	g_FILLER(S_filler_trap),
	g_FILLER(S_toxic_venom_trap),
	g_FILLER(S_insanity_trap),
	g_FILLER(S_madness_trap),
	g_FILLER(S_jessica_trap),
	g_FILLER(S_solvejg_trap),
	g_FILLER(S_wendy_trap),
	g_FILLER(S_katharina_trap),
	g_FILLER(S_elena_trap),
	g_FILLER(S_thai_trap),
	g_FILLER(S_elif_trap),
	g_FILLER(S_nadja_trap),
	g_FILLER(S_sandra_trap),
	g_FILLER(S_natalje_trap),
	g_FILLER(S_jeanetta_trap),
	g_FILLER(S_yvonne_trap),
	g_FILLER(S_maurah_trap),

	g_FILLER(S_lootcut_trap),
	g_FILLER(S_monster_speed_trap),
	g_FILLER(S_scaling_trap),
	g_FILLER(S_enmity_trap),
	g_FILLER(S_white_spell_trap),
	g_FILLER(S_gray_spell_trap),
	g_FILLER(S_quasar_trap),
	g_FILLER(S_momma_trap),
	g_FILLER(S_horror_trap),
	g_FILLER(S_artificer_trap),
	g_FILLER(S_wereform_trap),
	g_FILLER(S_nonprayer_trap),
	g_FILLER(S_evil_patch_trap),
	g_FILLER(S_hard_mode_trap),
	g_FILLER(S_secret_attack_trap),
	g_FILLER(S_eater_trap),
	g_FILLER(S_covetous_trap),
	g_FILLER(S_not_seen_trap),
	g_FILLER(S_dark_mode_trap),
	g_FILLER(S_antisearch_trap),
	g_FILLER(S_homicide_trap),
	g_FILLER(S_nasty_nation_trap),
	g_FILLER(S_wakeup_call_trap),
	g_FILLER(S_grayout_trap),
	g_FILLER(S_gray_center_trap),
	g_FILLER(S_checkerboard_trap),
	g_FILLER(S_clockwise_spin_trap),
	g_FILLER(S_counterclockwise_trap),
	g_FILLER(S_lag_trap),
	g_FILLER(S_blesscurse_trap),
	g_FILLER(S_delight_trap),
	g_FILLER(S_discharge_trap),
	g_FILLER(S_trashing_trap),
	g_FILLER(S_filtering_trap),
	g_FILLER(S_deformatting_trap),
	g_FILLER(S_flicker_strip_trap),
	g_FILLER(S_undressing_trap),
	g_FILLER(S_hyperbluewall_trap),
	g_FILLER(S_nolite_trap),
	g_FILLER(S_paranoia_trap),
	g_FILLER(S_fleecescript_trap),
	g_FILLER(S_interrupt_trap),
	g_FILLER(S_dustbin_trap),
	g_FILLER(S_mana_battery_trap),
	g_FILLER(S_monsterfingers_trap),
	g_FILLER(S_miscast_trap),
	g_FILLER(S_message_suppression_trap),
	g_FILLER(S_stuck_announcement_trap),
	g_FILLER(S_bloodthirsty_trap),
	g_FILLER(S_maximum_damage_trap),
	g_FILLER(S_latency_trap),
	g_FILLER(S_starlit_trap),
	g_FILLER(S_knowledge_trap),
	g_FILLER(S_highscore_trap),
	g_FILLER(S_pink_spell_trap),
	g_FILLER(S_green_spell_trap),
	g_FILLER(S_evc_trap),
	g_FILLER(S_underlayer_trap),
	g_FILLER(S_damage_meter_trap),
	g_FILLER(S_arbitrary_weight_trap),
	g_FILLER(S_fucked_info_trap),
	g_FILLER(S_black_spell_trap),
	g_FILLER(S_cyan_spell_trap),
	g_FILLER(S_heap_trap),
	g_FILLER(S_blue_spell_trap),
	g_FILLER(S_tron_trap),
	g_FILLER(S_red_spell_trap),
	g_FILLER(S_too_heavy_trap),
	g_FILLER(S_elongation_trap),
	g_FILLER(S_wrapover_trap),
	g_FILLER(S_destruction_trap),
	g_FILLER(S_melee_prefix_trap),
	g_FILLER(S_automore_trap),
	g_FILLER(S_unfair_attack_trap),

	g_FILLER(S_kop_cube),
	g_FILLER(S_boss_spawner),
	g_FILLER(S_contamination_trap),

	g_FILLER(S_evil_heel_trap),
	g_FILLER(S_bad_equipment_trap),
	g_FILLER(S_tempoconflict_trap),
	g_FILLER(S_tempohunger_trap),
	g_FILLER(S_teleportitis_trap),
	g_FILLER(S_polymorphitis_trap),
	g_FILLER(S_premature_death_trap),
	g_FILLER(S_lasting_amnesia_trap),
	g_FILLER(S_ragnarok_trap),
	g_FILLER(S_single_disenchant_trap),
	g_FILLER(S_acid_pit),

	g_FILLER(S_severe_disenchant_trap),
	g_FILLER(S_pain_trap),
	g_FILLER(S_trembling_trap),
	g_FILLER(S_techcap_trap),
	g_FILLER(S_spell_memory_trap),
	g_FILLER(S_skill_reduction_trap),
	g_FILLER(S_skillcap_trap),
	g_FILLER(S_permanent_stat_damage_trap),

	g_FILLER(S_orange_spell_trap),
	g_FILLER(S_violet_spell_trap),
	g_FILLER(S_trap_of_longing),
	g_FILLER(S_cursed_part_trap),
	g_FILLER(S_quaversal_trap),
	g_FILLER(S_appearance_shuffling_trap),
	g_FILLER(S_brown_spell_trap),
	g_FILLER(S_choiceless_trap),
	g_FILLER(S_goldspell_trap),
	g_FILLER(S_deprovement_trap),
	g_FILLER(S_initialization_trap),
	g_FILLER(S_gushlush_trap),
	g_FILLER(S_soiltype_trap),
	g_FILLER(S_dangerous_terrain_trap),
	g_FILLER(S_fallout_trap),
	g_FILLER(S_mojibake_trap),
	g_FILLER(S_gravation_trap),
	g_FILLER(S_uncalled_trap),
	g_FILLER(S_exploding_dice_trap),
	g_FILLER(S_permacurse_trap),
	g_FILLER(S_shrouded_identity_trap),
	g_FILLER(S_feeler_gauges_trap),
	g_FILLER(S_long_screwup_trap),
	g_FILLER(S_wing_yellow_changer),
	g_FILLER(S_life_saving_trap),
	g_FILLER(S_curseuse_trap),
	g_FILLER(S_cut_nutrition_trap),
	g_FILLER(S_skill_loss_trap),
	g_FILLER(S_autopilot_trap),
	g_FILLER(S_force_trap),
	g_FILLER(S_monster_glyph_trap),
	g_FILLER(S_changing_directive_trap),
	g_FILLER(S_container_kaboom_trap),
	g_FILLER(S_steal_degrade_trap),
	g_FILLER(S_left_inventory_trap),
	g_FILLER(S_fluctuating_speed_trap),
	g_FILLER(S_tarmustrokingnora_trap),
	g_FILLER(S_failure_trap),
	g_FILLER(S_bright_cyan_spell_trap),
	g_FILLER(S_frequentation_spawn_trap),
	g_FILLER(S_pet_ai_trap),
	g_FILLER(S_satan_trap),
	g_FILLER(S_rememberance_trap),
	g_FILLER(S_pokelie_trap),
	g_FILLER(S_autopickup_trap),
	g_FILLER(S_dywypi_trap),
	g_FILLER(S_silver_spell_trap),
	g_FILLER(S_metal_spell_trap),
	g_FILLER(S_platinum_spell_trap),
	g_FILLER(S_manler_trap),
	g_FILLER(S_doorning_trap),
	g_FILLER(S_nownsible_trap),
	g_FILLER(S_elm_street_trap),
	g_FILLER(S_monnoise_trap),
	g_FILLER(S_rang_call_trap),
	g_FILLER(S_recurring_spell_loss_trap),
	g_FILLER(S_antitraining_trap),
	g_FILLER(S_techout_trap),
	g_FILLER(S_stat_decay_trap),
	g_FILLER(S_movemork_trap),

	g_FILLER(S_hybrid_trap),
	g_FILLER(S_shapechange_trap),
	g_FILLER(S_meltem_trap),
	g_FILLER(S_miguc_trap),
	g_FILLER(S_directive_trap),
	g_FILLER(S_satatue_trap),
	g_FILLER(S_farting_web),
	g_FILLER(S_cataclysm_trap),
	g_FILLER(S_data_delete_trap),
	g_FILLER(S_elder_tentacling_trap),
	g_FILLER(S_footerer_trap),

	g_FILLER(S_bad_part_trap),
	g_FILLER(S_completely_bad_part_trap),
	g_FILLER(S_evil_variant_trap),

	g_FILLER(S_grave_wall_trap),
	g_FILLER(S_tunnel_trap),
	g_FILLER(S_farmland_trap),
	g_FILLER(S_mountain_trap),
	g_FILLER(S_water_tunnel_trap),
	g_FILLER(S_crystal_flood_trap),
	g_FILLER(S_moorland_trap),
	g_FILLER(S_urine_trap),
	g_FILLER(S_shifting_sand_trap),
	g_FILLER(S_styx_trap),
	g_FILLER(S_pentagram_trap),
	g_FILLER(S_snow_trap),
	g_FILLER(S_ash_trap),
	g_FILLER(S_sand_trap),
	g_FILLER(S_pavement_trap),
	g_FILLER(S_highway_trap),
	g_FILLER(S_grassland_trap),
	g_FILLER(S_nether_mist_trap),
	g_FILLER(S_stalactite_trap),
	g_FILLER(S_cryptfloor_trap),
	g_FILLER(S_bubble_trap),
	g_FILLER(S_rain_cloud_trap),
	g_FILLER(S_item_nastification_trap),
	g_FILLER(S_sanity_increase_trap),
	g_FILLER(S_psi_trap),
	g_FILLER(S_gay_trap),
	g_FILLER(S_sarah_trap),
	g_FILLER(S_claudia_trap),
	g_FILLER(S_ludgera_trap),
	g_FILLER(S_kati_trap),
	g_FILLER(S_sanity_treble_trap),
	g_FILLER(S_stat_decrease_trap),
	g_FILLER(S_simeout_trap),
	g_FILLER(S_spressing_trap),
	g_FILLER(S_nelly_trap),
	g_FILLER(S_eveline_trap),
	g_FILLER(S_karin_trap),
	g_FILLER(S_juen_trap),
	g_FILLER(S_kristina_trap),
	g_FILLER(S_lou_trap),
	g_FILLER(S_almut_trap),
	g_FILLER(S_julietta_trap),
	g_FILLER(S_arabella_trap),
	g_FILLER(S_kristin_trap),
	g_FILLER(S_anna_trap),
	g_FILLER(S_ruea_trap),
	g_FILLER(S_dora_trap),
	g_FILLER(S_marike_trap),
	g_FILLER(S_jette_trap),
	g_FILLER(S_ina_trap),
	g_FILLER(S_sing_trap),
	g_FILLER(S_victoria_trap),
	g_FILLER(S_melissa_trap),
	g_FILLER(S_anita_trap),
	g_FILLER(S_henrietta_trap),
	g_FILLER(S_verena_trap),
	g_FILLER(S_annemarie_trap),
	g_FILLER(S_jil_trap),
	g_FILLER(S_jana_trap),
	g_FILLER(S_katrin_trap),
	g_FILLER(S_gudrun_trap),
	g_FILLER(S_ella_trap),
	g_FILLER(S_manuela_trap),
	g_FILLER(S_jennifer_trap),
	g_FILLER(S_patricia_trap),
	g_FILLER(S_antje_trap),
	g_FILLER(S_antje_x_trap),
	g_FILLER(S_kerstin_trap),
	g_FILLER(S_laura_trap),
	g_FILLER(S_larissa_trap),
	g_FILLER(S_nora_trap),
	g_FILLER(S_natalia_trap),
	g_FILLER(S_susanne_trap),
	g_FILLER(S_lisa_trap),
	g_FILLER(S_bridghitte_trap),
	g_FILLER(S_julia_trap),
	g_FILLER(S_nicole_trap),
	g_FILLER(S_rita_trap),
	g_FILLER(S_janina_trap),
	g_FILLER(S_rosa_trap),
	g_FILLER(S_ksenia_trap),
	g_FILLER(S_lydia_trap),
	g_FILLER(S_conny_trap),
	g_FILLER(S_katia_trap),
	g_FILLER(S_mariya_trap),
	g_FILLER(S_elise_trap),
	g_FILLER(S_ronja_trap),
	g_FILLER(S_ariane_trap),
	g_FILLER(S_johanna_trap),
	g_FILLER(S_inge_trap),

	g_FILLER(S_wall_trap),
	g_FILLER(S_monster_generator),
	g_FILLER(S_potion_dispenser),
	g_FILLER(S_spacewars_spawn_trap),
	g_FILLER(S_tv_tropes_trap),
	g_FILLER(S_symbiote_trap),
	g_FILLER(S_kill_symbiote_trap),
	g_FILLER(S_symbiote_replacement_trap),
	g_FILLER(S_shutdown_trap),
	g_FILLER(S_corona_trap),
	g_FILLER(S_unproofing_trap),
	g_FILLER(S_visibility_trap),
	g_FILLER(S_feminism_stone_trap),
	g_FILLER(S_branch_teleporter),
	g_FILLER(S_branch_beamer),
	g_FILLER(S_poison_arrow_trap),
	g_FILLER(S_poison_bolt_trap),
	g_FILLER(S_mace_trap),
	g_FILLER(S_shueft_trap),
	g_FILLER(S_moth_larvae_trap),
	g_FILLER(S_worthiness_trap),
	g_FILLER(S_conduct_trap),
	g_FILLER(S_strikethrough_trap),
	g_FILLER(S_multiple_gather_trap),
	g_FILLER(S_vivisection_trap),
	g_FILLER(S_instafeminism_trap),
	g_FILLER(S_instanasty_trap),
	g_FILLER(S_skill_point_loss_trap),
	g_FILLER(S_perfect_match_trap),
	g_FILLER(S_dumbie_lightsaber_trap),
	g_FILLER(S_wrong_stairs),
	g_FILLER(S_techstop_trap),
	g_FILLER(S_amnesia_switch_trap),
	g_FILLER(S_skill_swap_trap),
	g_FILLER(S_skill_upordown_trap),
	g_FILLER(S_skill_randomize_trap),

	g_FILLER(S_skill_multiply_trap),
	g_FILLER(S_trapwalk_trap),
	g_FILLER(S_cluster_trap),
	g_FILLER(S_field_trap),
	g_FILLER(S_monicide_trap),
	g_FILLER(S_trap_creation_trap),
	g_FILLER(S_leold_trap),
	g_FILLER(S_animeband_trap),
	g_FILLER(S_perfume_trap),
	g_FILLER(S_court_trap),
	g_FILLER(S_elder_scrolls_trap),
	g_FILLER(S_joke_trap),
	g_FILLER(S_dungeon_lords_trap),
	g_FILLER(S_fortytwo_trap),
	g_FILLER(S_randomize_trap),
	g_FILLER(S_evilroom_trap),
	g_FILLER(S_aoe_trap),
	g_FILLER(S_elona_trap),
	g_FILLER(S_religion_trap),
	g_FILLER(S_steamband_trap),
	g_FILLER(S_hardcore_trap),
	g_FILLER(S_machine_trap),
	g_FILLER(S_bee_trap),
	g_FILLER(S_migo_trap),
	g_FILLER(S_angband_trap),
	g_FILLER(S_dnethack_trap),
	g_FILLER(S_evil_spawn_trap),
	g_FILLER(S_shoe_trap),
	g_FILLER(S_inside_trap),
	g_FILLER(S_doom_trap),
	g_FILLER(S_military_trap),
	g_FILLER(S_illusion_trap),
	g_FILLER(S_diablo_trap),

	g_FILLER(S_giant_explorer_trap),
	g_FILLER(S_trapwarp_trap),
	g_FILLER(S_yawm_trap),
	g_FILLER(S_cradle_of_chaos_trap),
	g_FILLER(S_tez_trap),
	g_FILLER(S_enthu_trap),
	g_FILLER(S_mikra_trap),
	g_FILLER(S_gots_too_good_trap),
	g_FILLER(S_killer_room_trap),
	g_FILLER(S_no_fun_walls_trap),

	g_FILLER(S_ruth_trap),
	g_FILLER(S_magdalena_trap),
	g_FILLER(S_marleen_trap),
	g_FILLER(S_klara_trap),
	g_FILLER(S_friederike_trap),
	g_FILLER(S_naomi_trap),
	g_FILLER(S_ute_trap),
	g_FILLER(S_jasieen_trap),
	g_FILLER(S_yasaman_trap),
	g_FILLER(S_may_britt_trap),

	g_FILLER(S_calling_out_trap),
	g_FILLER(S_field_break_trap),
	g_FILLER(S_tenth_trap),
	g_FILLER(S_debt_trap),
	g_FILLER(S_inversion_trap),
	g_FILLER(S_wince_trap),
	g_FILLER(S_u_have_been_trap),
	g_FILLER(S_really_bad_trap),
	g_FILLER(S_covid_trap),
	g_FILLER(S_artiblast_trap),

	g_FILLER(S_persistent_fart_trap),
	g_FILLER(S_attacking_heel_trap),
	g_FILLER(S_trap_teleporter),
	g_FILLER(S_alignment_trash_trap),
	g_FILLER(S_reshuffle_trap),
	g_FILLER(S_musehand_trap),
	g_FILLER(S_dogside_trap),
	g_FILLER(S_bankrupt_trap),
	g_FILLER(S_fillup_trap),
	g_FILLER(S_airstrike_trap),
	g_FILLER(S_dynamite_trap),
	g_FILLER(S_malevolence_trap),
	g_FILLER(S_leaflet_trap),
	g_FILLER(S_tentadeep_trap),
	g_FILLER(S_stathalf_trap),
	g_FILLER(S_cutstat_trap),
	g_FILLER(S_rare_spawn_trap),
	g_FILLER(S_you_are_an_idiot_trap),
	g_FILLER(S_nastycurse_trap),
	g_FILLER(S_repeating_nastycurse_trap),

	g_FILLER(S_nadine_trap),
	g_FILLER(S_luisa_trap),
	g_FILLER(S_irina_trap),
	g_FILLER(S_liselotte_trap),
	g_FILLER(S_greta_trap),
	g_FILLER(S_jane_trap),
	g_FILLER(S_sue_lyn_trap),
	g_FILLER(S_charlotte_trap),
	g_FILLER(S_hannah_trap),
	g_FILLER(S_little_marie_trap),

	g_FILLER(S_real_lie_trap),
	g_FILLER(S_escape_past_trap),
	g_FILLER(S_pethate_trap),
	g_FILLER(S_pet_lashout_trap),
	g_FILLER(S_petstarve_trap),
	g_FILLER(S_petscrew_trap),
	g_FILLER(S_tech_loss_trap),
	g_FILLER(S_proofloss_trap),
	g_FILLER(S_un_invis_trap),
	g_FILLER(S_detectation_trap),

	g_FILLER(S_option_trap),
	g_FILLER(S_miscolor_trap),
	g_FILLER(S_one_rainbow_trap),
	g_FILLER(S_colorshift_trap),
	g_FILLER(S_top_line_trap),
	g_FILLER(S_caps_trap),
	g_FILLER(S_un_knowledge_trap),
	g_FILLER(S_darkhance_trap),
	g_FILLER(S_dschueueuet_trap),
	g_FILLER(S_nopeskill_trap),
	g_FILLER(S_fuckfuckfuck_trap),

	g_FILLER(S_burden_trap),
	g_FILLER(S_magic_vacuum_trap),

	g_FILLER(S_tanja_trap),
	g_FILLER(S_sonja_trap),
	g_FILLER(S_rhea_trap),
	g_FILLER(S_lara_trap),

	g_FILLER(S_fuck_over_trap),

	g_FILLER(S_dagger_trap),
	g_FILLER(S_razor_trap),
	g_FILLER(S_phosgene_trap),
	g_FILLER(S_chloroform_trap),
	g_FILLER(S_corrosion_trap),
	g_FILLER(S_flame_trap),
	g_FILLER(S_wither_trap),
	g_FILLER(S_phaseport_trap),
	g_FILLER(S_phasebeam_trap),
	g_FILLER(S_vulnerate_trap),

	g_FILLER(S_epvi_trap),

	g_FILLER(S_sabrina_trap),

	g_FILLER(S_aefde_trap),

	g_FILLER(S_falling_rock_cold_trap),
	g_FILLER(S_return_trap),
	g_FILLER(S_intrinsic_steal_trap),
	g_FILLER(S_score_axe_trap),
	g_FILLER(S_score_drain_trap),
	g_FILLER(S_single_unidentify_trap),
	g_FILLER(S_hypoxic_pit),
	g_FILLER(S_unlucky_trap),
	g_FILLER(S_alignment_reduction_trap),
	g_FILLER(S_malignant_trap),
	g_FILLER(S_stat_damage_trap),
	g_FILLER(S_half_memory_trap),
	g_FILLER(S_half_training_trap),
	g_FILLER(S_debuff_trap),
	g_FILLER(S_trip_once_trap),
	g_FILLER(S_narcolepsy_trap),
	g_FILLER(S_martial_arts_trap),
	g_FILLER(S_mean_burden_trap),
	g_FILLER(S_carrcap_trap),
	g_FILLER(S_umeng_trap),

	g_FILLER(S_spell_cooldown_trap),
	g_FILLER(S_turbodull_trap),
	g_FILLER(S_reddam_trap),
	g_FILLER(S_redinc_trap),
	g_FILLER(S_mulch_trap),
	g_FILLER(S_inaccuracy_trap),
	g_FILLER(S_monster_multishot_trap),
	g_FILLER(S_have_a_pin_trap),
	g_FILLER(S_costly_failure_trap),
	g_FILLER(S_inventory_size_trap),

	g_FILLER(S_spike_trap),
	g_FILLER(S_paper_arrow_trap),
	g_FILLER(S_knife_trap),
	g_FILLER(S_summon_monster_trap),
	g_FILLER(S_red_damage_trap),
	g_FILLER(S_green_damage_trap),
	g_FILLER(S_yellow_damage_trap),
	g_FILLER(S_white_damage_trap),
	g_FILLER(S_blue_damage_trap),
	g_FILLER(S_multi_debuff_trap),

	g_FILLER(S_magic_damage_trap),

	g_FILLER(S_wild_weather_trap),

	g_FILLER(S_timerun_trap),

	g_FILLER(S_vbeam),
	g_FILLER(S_hbeam),
	g_FILLER(S_lslant),
	g_FILLER(S_rslant),
	g_FILLER(S_digbeam),
	g_FILLER(S_flashbeam),
/*70*/	g_FILLER(S_boomleft),
	g_FILLER(S_boomright),
	g_FILLER(S_ss1),
	g_FILLER(S_ss2),
	g_FILLER(S_ss3),
	g_FILLER(S_ss4),
	g_FILLER(S_sw_tl),
	g_FILLER(S_sw_tc),
	g_FILLER(S_sw_tr),
	g_FILLER(S_sw_ml),
/*80*/	g_FILLER(S_sw_mr),
	g_FILLER(S_sw_bl),
	g_FILLER(S_sw_bc),
	g_FILLER(S_sw_br),
	g_FILLER(S_explode1),
	g_FILLER(S_explode2),
	g_FILLER(S_explode3),
	g_FILLER(S_explode4),
	g_FILLER(S_explode5),
	g_FILLER(S_explode6),
/*90*/	g_FILLER(S_explode7),
	g_FILLER(S_explode8),
	g_FILLER(S_explode9),
};
#endif	/* MAC_GRAPHICS_ENV */

#ifdef PC9800
void (*ascgraphics_mode_callback)) = 0;	/* set in tty_start_screen((void) */
#endif

/*
 * Convert the given character to an object class.  If the character is not
 * recognized, then MAXOCLASSES is returned.  Used in detect.c invent.c,
 * options.c, pickup.c, sp_lev.c, and lev_main.c.
 */
int
def_char_to_objclass(ch)
    char ch;
{
    int i;
    for (i = 1; i < MAXOCLASSES; i++)
	if (ch == def_oc_syms[i]) break;
    return i;
}

/*
 * Convert a character into a monster class.  This returns the _first_
 * match made.  If there are are no matches, return MAXMCLASSES.
 */
int
def_char_to_monclass(ch)
    char ch;
{
    int i;

    for (i = 1; i < MAXMCLASSES; i++)
	if (def_monsyms[i] == ch) break;
    return i;
}

void
assign_graphics(graph_chars, glth, maxlen, offset)
register uchar *graph_chars;
int glth, maxlen, offset;
{
    register int i;

	if (iflags.winggraphics) {

	    for (i = 0; i < maxlen; i++)
		showsyms[i+offset] = (((i < glth) && wing_graphics[i]) ? wing_graphics[i] : defsyms[i+offset].sym);

	return;
	}

    for (i = 0; i < maxlen; i++)
	showsyms[i+offset] = (((i < glth) && graph_chars[i]) ?
		       graph_chars[i] : defsyms[i+offset].sym);
}

void
assignwinggraphics()
{
    register int i;

	if (iflags.winggraphics) {

	    for (i = 0; i < MAXPCHARS; i++)
		showsyms[i] = (((i < (SIZE(wing_graphics)) ) && wing_graphics[i]) ? wing_graphics[i] : defsyms[i].sym);

	return;
	}

}

void
switch_graphics(gr_set_flag)
int gr_set_flag;
{
    switch (gr_set_flag) {
	default:
	case ASCII_GRAPHICS:
	    assign_graphics((uchar *)0, 0, MAXPCHARS, 0);
#ifdef PC9800
	    if (ascgraphics_mode_callback) (*ascgraphics_mode_callback)();
#endif
	    break;
#ifdef ASCIIGRAPH
	case IBM_GRAPHICS:
/*
 * Use the nice IBM Extended ASCII line-drawing characters (codepage 437).
 *
 * OS/2 defaults to a multilingual character set (codepage 850, corresponding
 * to the ISO 8859 character set.  We should probably do a VioSetCp() call to
 * set the codepage to 437.
 */
	    iflags.IBMgraphics = TRUE;
	    iflags.DECgraphics = FALSE;
#ifdef CURSES_GRAPHICS
        iflags.cursesgraphics = FALSE;
#endif
	    assign_graphics(ibm_graphics, SIZE(ibm_graphics), MAXPCHARS, 0);
#ifdef PC9800
	    if (ibmgraphics_mode_callback) (*ibmgraphics_mode_callback)();
#endif
	    break;
#endif /* ASCIIGRAPH */
#ifdef TERMLIB
	case DEC_GRAPHICS:
/*
 * Use the VT100 line drawing character set.
 */
	    iflags.DECgraphics = TRUE;
	    iflags.IBMgraphics = FALSE;
#ifdef CURSES_GRAPHICS
        iflags.cursesgraphics = FALSE;
#endif
	    assign_graphics(dec_graphics, SIZE(dec_graphics), MAXPCHARS, 0);
	    if (decgraphics_mode_callback) (*decgraphics_mode_callback)();
	    break;
#endif /* TERMLIB */
#ifdef MAC_GRAPHICS_ENV
	case MAC_GRAPHICS:
	    assign_graphics(mac_graphics, SIZE(mac_graphics), MAXPCHARS, 0);
	    break;
#endif
#ifdef CURSES_GRAPHICS
    case CURS_GRAPHICS:
	    assign_graphics((uchar *)0, 0, MAXPCHARS, 0);
        iflags.cursesgraphics = TRUE;
	    iflags.IBMgraphics = FALSE;
	    iflags.DECgraphics = FALSE;
        break;
#endif
	}
    return;
}


#ifdef REINCARNATION

/*
 * saved display symbols for objects & features.
 */
static uchar save_oc_syms[MAXOCLASSES] = DUMMY;
static uchar save_showsyms[MAXPCHARS]  = DUMMY;
static uchar save_monsyms[MAXPCHARS]   = DUMMY;

static const uchar r_oc_syms[MAXOCLASSES] = {
/* 0*/	'\0',
	ILLOBJ_SYM,
	WEAPON_SYM,
	']',			/* armor */
	RING_SYM,
/* 5*/	',',			/* amulet */
	IMPLANT_SYM,
	TOOL_SYM,
	':',			/* food */
	POTION_SYM,
/*10*/	SCROLL_SYM,
	SPBOOK_SYM,
	WAND_SYM,
	GEM_SYM,		/* gold -- yes it's the same as gems */
	GEM_SYM,
/*15*/	ROCK_SYM,
	BALL_SYM,
	CHAIN_SYM,
	VENOM_SYM
};

# ifdef ASCIIGRAPH
/* Rogue level graphics.  Under IBM graphics mode, use the symbols that were
 * used for Rogue on the IBM PC.  Unfortunately, this can't be completely
 * done because some of these are control characters--armor and rings under
 * DOS, and a whole bunch of them under Linux.  Use the TTY Rogue characters
 * for those cases.
 */
static const uchar IBM_r_oc_syms[MAXOCLASSES] = {	/* a la EPYX Rogue */
/* 0*/	'\0',
	ILLOBJ_SYM,
#  if defined(MSDOS) || defined(OS2) || ( defined(WIN32) && !defined(MSWIN_GRAPHICS) )
	0x18,			/* weapon: up arrow */
	0x0a, /* ARMOR_SYM,*/	/* armor:  Vert rect with o */
	0x09, /* RING_SYM,*/	/* ring:   circle with arrow */
/* 5*/	0x0c,			/* amulet: "female" symbol */
	/*IMPLANT_SYM,*/0x03,
	/*TOOL_SYM*/0x06,
	0x05,			/* food:   club (as in cards) */
	0xad,			/* potion: upside down '!' */
/*10*/	0x0e,			/* scroll: musical note */
	/*SPBOOK_SYM*/0x0d,
	0xe7,			/* wand:   greek tau */
	0x0f,			/* gold:   yes it's the same as gems */
	0x0f,			/* gems:   fancy '*' */
#  else
	')',			/* weapon  */
	ARMOR_SYM,		/* armor */
	RING_SYM,		/* ring */
/* 5*/	',',			/* amulet  */
	IMPLANT_SYM,
	TOOL_SYM,
	':',			/* food    */
	0xad,			/* potion: upside down '!' */
/*10*/	SCROLL_SYM,		/* scroll  */
	SPBOOK_SYM,
	0xe7,			/* wand:   greek tau */
	GEM_SYM,		/* gold:   yes it's the same as gems */
	GEM_SYM,		/* gems    */
#  endif
/*15*/	ROCK_SYM,
	BALL_SYM,
	CHAIN_SYM,
	VENOM_SYM
};
# endif /* ASCIIGRAPH */

void
assign_rogue_graphics(is_rlevel)
boolean is_rlevel;
{
    /* Adjust graphics display characters on Rogue levels */

    if (is_rlevel) {
	register int i;

	(void) memcpy((void *)save_showsyms,
		      (void *)showsyms, sizeof showsyms);
	(void) memcpy((void *)save_oc_syms,
		      (void *)oc_syms, sizeof oc_syms);
	(void) memcpy((void *)save_monsyms,
		      (void *)monsyms, sizeof monsyms);

	/* Use a loop: char != uchar on some machines. */
	for (i = 0; i < MAXMCLASSES; i++)
	    monsyms[i] = def_monsyms[i];
# if defined(ASCIIGRAPH) && !defined(MSWIN_GRAPHICS)
	if (iflags.IBMgraphics
#  if defined(USE_TILES) && defined(MSDOS)
		&& !iflags.grmode
#  endif
		)
		/* putty can't display non-printable characters... so we need to provide a way to disable
		 * that horrible interface screw --Amy */

	    if (flags.epyxmode) monsyms[S_HUMAN] = 0x01; /* smiley face */


# endif
	for (i = 0; i < MAXPCHARS; i++)
	    showsyms[i] = defsyms[i].sym;

/*
 * Some day if these rogue showsyms get much more extensive than this,
 * we may want to create r_showsyms, and IBM_r_showsyms arrays to hold
 * all of this info and to simply initialize it via a for() loop like r_oc_syms.
 */

# ifdef ASCIIGRAPH
	if (!iflags.IBMgraphics
#  if defined(USE_TILES) && defined(MSDOS)
		|| iflags.grmode
#  endif
				) {
# endif
	    showsyms[S_vodoor]  = showsyms[S_hodoor]  = showsyms[S_ndoor] = '+';
	    showsyms[S_upstair] = showsyms[S_dnstair] = '%';
# ifdef ASCIIGRAPH
	} else {
	    /* a la EPYX Rogue */
	    showsyms[S_vwall]   = 0xba; /* all walls now use	*/
	    showsyms[S_hwall]   = 0xcd; /* double line graphics	*/
	    showsyms[S_tlcorn]  = 0xc9;
	    showsyms[S_trcorn]  = 0xbb;
	    showsyms[S_blcorn]  = 0xc8;
	    showsyms[S_brcorn]  = 0xbc;
	    showsyms[S_crwall]  = 0xce;
	    showsyms[S_tuwall]  = 0xca;
	    showsyms[S_tdwall]  = 0xcb;
	    showsyms[S_tlwall]  = 0xb9;
	    showsyms[S_trwall]  = 0xcc;
	    showsyms[S_ndoor]   = 0xce;
	    showsyms[S_vodoor]  = 0xe1;
	    showsyms[S_hodoor]  = 0xe1;
	    showsyms[S_room]    = 0xfa; /* centered dot */
	    showsyms[S_corr]    = 0xb1;
	    showsyms[S_litcorr] = 0xb2;
	    showsyms[S_upstair] = 0xf0; /* Greek Xi */
	    showsyms[S_dnstair] = 0xf0;
#ifndef MSWIN_GRAPHICS

	    if (flags.epyxmode) {

	    showsyms[S_arrow_trap] = 0x04; /* diamond (cards) */
	    showsyms[S_dart_trap] = 0x04;
	    showsyms[S_falling_rock_trap] = 0x04;
	    showsyms[S_squeaky_board] = 0x04;
	    showsyms[S_bear_trap] = 0x04;
	    showsyms[S_land_mine] = 0x04;
	    showsyms[S_rolling_boulder_trap] = 0x04;
	    showsyms[S_sleeping_gas_trap] = 0x04;
	    showsyms[S_rust_trap] = 0x04;
	    showsyms[S_fire_trap] = 0x04;
	    showsyms[S_pit] = 0x04;
	    showsyms[S_spiked_pit] = 0x04;
	    showsyms[S_hole] = 0x04;
	    showsyms[S_trap_door] = 0x04;
	    showsyms[S_teleportation_trap] = 0x04;
	    showsyms[S_level_teleporter] = 0x04;
	    showsyms[S_magic_portal] = 0x04;
	    showsyms[S_web] = 0x04;
	    showsyms[S_statue_trap] = 0x04;
	    showsyms[S_magic_trap] = 0x04;
	    showsyms[S_anti_magic_trap] = 0x04;
	    showsyms[S_polymorph_trap] = 0x04;
	    showsyms[S_ice_trap] = 0x04;
	    showsyms[S_spear_trap] = 0x04;
	    showsyms[S_falling_rocks_trap] = 0x04;
	    showsyms[S_magic_beam_trap] = 0x04;
	    showsyms[S_pile_of_shit] = 0x04;
	    showsyms[S_animation_trap] = 0x04;
	    showsyms[S_glyph_of_warding] = 0x04;
	    showsyms[S_scything_blade] = 0x04;
	    showsyms[S_bolt_trap] = 0x04;
	    showsyms[S_acid_pool] = 0x04;
	    showsyms[S_water_pool] = 0x04;
	    showsyms[S_poison_gas_trap] = 0x04;
	    showsyms[S_slow_gas_trap] = 0x04;
	    showsyms[S_shock_trap] = 0x04;
	    showsyms[S_rmb_loss_trap] = 0x04;
	    showsyms[S_display_trap] = 0x04;
	    showsyms[S_spell_loss_trap] = 0x04;
	    showsyms[S_yellow_spell_trap] = 0x04;
	    showsyms[S_auto_destruct_trap] = 0x04;
	    showsyms[S_memory_trap] = 0x04;
	    showsyms[S_inventory_trap] = 0x04;
	    showsyms[S_black_ng_wall_trap] = 0x04;
	    showsyms[S_menu_trap] = 0x04;
	    showsyms[S_speed_trap] = 0x04;
	    showsyms[S_superscroller_trap] = 0x04;
	    showsyms[S_active_superscroller_trap] = 0x04;
	    showsyms[S_swarm_trap] = 0x04;
	    showsyms[S_automatic_switcher] = 0x04;
	    showsyms[S_unknown_trap] = 0x04;
	    showsyms[S_heel_trap] = 0x04;
	    showsyms[S_vuln_trap] = 0x04;

	    showsyms[S_lava_trap] = 0x04;
	    showsyms[S_flood_trap] = 0x04;
	    showsyms[S_drain_trap] = 0x04;
	    showsyms[S_free_hand_trap] = 0x04;
	    showsyms[S_disintegration_trap] = 0x04;
	    showsyms[S_unidentify_trap] = 0x04;
	    showsyms[S_thirst_trap] = 0x04;
	    showsyms[S_luck_trap] = 0x04;
	    showsyms[S_shades_of_grey_trap] = 0x04;
	    showsyms[S_item_telep_trap] = 0x04;
	    showsyms[S_gravity_trap] = 0x04;
	    showsyms[S_death_trap] = 0x04;
	    showsyms[S_stone_to_flesh_trap] = 0x04;
	    showsyms[S_quicksand_trap] = 0x04;
	    showsyms[S_faint_trap] = 0x04;
	    showsyms[S_curse_trap] = 0x04;
	    showsyms[S_difficulty_trap] = 0x04;
	    showsyms[S_sound_trap] = 0x04;
	    showsyms[S_caster_trap] = 0x04;
	    showsyms[S_weakness_trap] = 0x04;
	    showsyms[S_rot_thirteen_trap] = 0x04;
	    showsyms[S_bishop_trap] = 0x04;
	    showsyms[S_confusion_trap] = 0x04;
	    showsyms[S_nupesell_trap] = 0x04;
	    showsyms[S_drop_trap] = 0x04;
	    showsyms[S_dstw_trap] = 0x04;
	    showsyms[S_status_trap] = 0x04;
	    showsyms[S_alignment_trap] = 0x04;
	    showsyms[S_stairs_trap] = 0x04;

	    showsyms[S_pestilence_trap] = 0x04;
	    showsyms[S_famine_trap] = 0x04;

	    showsyms[S_relocation_trap] = 0x04;

	    showsyms[S_uninformation_trap] = 0x04;

	    showsyms[S_intrinsic_loss_trap] = 0x04;
	    showsyms[S_blood_loss_trap] = 0x04;
	    showsyms[S_bad_effect_trap] = 0x04;
	    showsyms[S_multiplying_trap] = 0x04;
	    showsyms[S_auto_vuln_trap] = 0x04;
	    showsyms[S_tele_items_trap] = 0x04;
	    showsyms[S_nastiness_trap] = 0x04;

	    showsyms[S_recursion_trap] = 0x04;
	    showsyms[S_respawn_trap] = 0x04;
	    showsyms[S_warp_zone] = 0x04;
	    showsyms[S_captcha_trap] = 0x04;
	    showsyms[S_mind_wipe_trap] = 0x04;
	    showsyms[S_shuriken_trap] = 0x04;
	    showsyms[S_shit_pit] = 0x04;
	    showsyms[S_shaft_trap] = 0x04;
	    showsyms[S_lock_trap] = 0x04;
	    showsyms[S_mc_trap] = 0x04;
	    showsyms[S_farlook_trap] = 0x04;
	    showsyms[S_gateway_trap] = 0x04;
	    showsyms[S_growing_trap] = 0x04;
	    showsyms[S_cooling_trap] = 0x04;
	    showsyms[S_bar_trap] = 0x04;
	    showsyms[S_locking_trap] = 0x04;
	    showsyms[S_air_trap] = 0x04;
	    showsyms[S_terrain_trap] = 0x04;

	    showsyms[S_loudspeaker] = 0x04;
	    showsyms[S_nest_trap] = 0x04;
	    showsyms[S_bullet_trap] = 0x04;
	    showsyms[S_paralysis_trap] = 0x04;
	    showsyms[S_cyanide_trap] = 0x04;
	    showsyms[S_laser_trap] = 0x04;
	    showsyms[S_giant_chasm] = 0x04;
	    showsyms[S_fart_trap] = 0x04;
	    showsyms[S_confuse_trap] = 0x04;
	    showsyms[S_stun_trap] = 0x04;
	    showsyms[S_hallucination_trap] = 0x04;
	    showsyms[S_petrification_trap] = 0x04;
	    showsyms[S_numbness_trap] = 0x04;
	    showsyms[S_freezing_trap] = 0x04;
	    showsyms[S_burning_trap] = 0x04;
	    showsyms[S_fear_trap] = 0x04;
	    showsyms[S_blindness_trap] = 0x04;
	    showsyms[S_glib_trap] = 0x04;
	    showsyms[S_slime_trap] = 0x04;
	    showsyms[S_inertia_trap] = 0x04;
	    showsyms[S_time_trap] = 0x04;
	    showsyms[S_lycanthropy_trap] = 0x04;
	    showsyms[S_unlight_trap] = 0x04;
	    showsyms[S_elemental_trap] = 0x04;
	    showsyms[S_escalating_trap] = 0x04;
	    showsyms[S_negative_trap] = 0x04;
	    showsyms[S_mana_trap] = 0x04;
	    showsyms[S_sin_trap] = 0x04;
	    showsyms[S_destroy_armor_trap] = 0x04;
	    showsyms[S_divine_anger_trap] = 0x04;
	    showsyms[S_genetic_trap] = 0x04;
	    showsyms[S_missingno_trap] = 0x04;
	    showsyms[S_cancellation_trap] = 0x04;
	    showsyms[S_hostility_trap] = 0x04;
	    showsyms[S_boss_trap] = 0x04;
	    showsyms[S_wishing_trap] = 0x04;
	    showsyms[S_recurring_amn_trap] = 0x04;
	    showsyms[S_bigscript_trap] = 0x04;
	    showsyms[S_bank_trap] = 0x04;
	    showsyms[S_only_trap] = 0x04;
	    showsyms[S_map_trap] = 0x04;
	    showsyms[S_tech_trap] = 0x04;
	    showsyms[S_disenchant_trap] = 0x04;
	    showsyms[S_verisiert] = 0x04;
	    showsyms[S_chaos_trap] = 0x04;
	    showsyms[S_muteness_trap] = 0x04;
	    showsyms[S_ntll_trap] = 0x04;
	    showsyms[S_engraving_trap] = 0x04;
	    showsyms[S_magic_device_trap] = 0x04;
	    showsyms[S_book_trap] = 0x04;
	    showsyms[S_level_trap] = 0x04;
	    showsyms[S_quiz_trap] = 0x04;

	    showsyms[S_falling_boulder_trap] = 0x04;
	    showsyms[S_glass_arrow_trap] = 0x04;
	    showsyms[S_glass_bolt_trap] = 0x04;
	    showsyms[S_out_of_magic_trap] = 0x04;
	    showsyms[S_plasma_trap] = 0x04;
	    showsyms[S_bomb_trap] = 0x04;
	    showsyms[S_earthquake_trap] = 0x04;
	    showsyms[S_noise_trap] = 0x04;
	    showsyms[S_glue_trap] = 0x04;
	    showsyms[S_guillotine_trap] = 0x04;
	    showsyms[S_bisection_trap] = 0x04;
	    showsyms[S_volt_trap] = 0x04;
	    showsyms[S_horde_trap] = 0x04;
	    showsyms[S_immobility_trap] = 0x04;
	    showsyms[S_green_glyph_trap] = 0x04;
	    showsyms[S_blue_glyph_trap] = 0x04;
	    showsyms[S_yellow_glyph_trap] = 0x04;
	    showsyms[S_orange_glyph_trap] = 0x04;
	    showsyms[S_black_glyph_trap] = 0x04;
	    showsyms[S_purple_glyph_trap] = 0x04;

	    showsyms[S_metabolic_trap] = 0x04;
	    showsyms[S_no_return_trap] = 0x04;
	    showsyms[S_ego_trap] = 0x04;
	    showsyms[S_fast_forward_trap] = 0x04;
	    showsyms[S_rotten_trap] = 0x04;
	    showsyms[S_unskilled_trap] = 0x04;
	    showsyms[S_low_stats_trap] = 0x04;
	    showsyms[S_training_trap] = 0x04;
	    showsyms[S_exercise_trap] = 0x04;
	    showsyms[S_falling_loadstone_trap] = 0x04;
	    showsyms[S_summon_undead_trap] = 0x04;
	    showsyms[S_falling_nastystone_trap] = 0x04;

	    showsyms[S_spined_ball_trap] = 0x04;
	    showsyms[S_pendulum_trap] = 0x04;
	    showsyms[S_turn_table] = 0x04;
	    showsyms[S_scent_trap] = 0x04;
	    showsyms[S_banana_trap] = 0x04;
	    showsyms[S_falling_tub_trap] = 0x04;
	    showsyms[S_alarm] = 0x04;
	    showsyms[S_caltrops_trap] = 0x04;
	    showsyms[S_blade_wire] = 0x04;
	    showsyms[S_magnet_trap] = 0x04;
	    showsyms[S_slingshot_trap] = 0x04;
	    showsyms[S_cannon_trap] = 0x04;
	    showsyms[S_venom_sprinkler] = 0x04;
	    showsyms[S_fumarole] = 0x04;

	    showsyms[S_mana_pit] = 0x04;
	    showsyms[S_elemental_portal] = 0x04;
	    showsyms[S_girliness_trap] = 0x04;
	    showsyms[S_fumbling_trap] = 0x04;
	    showsyms[S_egomonster_trap] = 0x04;
	    showsyms[S_flooding_trap] = 0x04;
	    showsyms[S_monster_cube] = 0x04;
	    showsyms[S_cursed_grave] = 0x04;
	    showsyms[S_limitation_trap] = 0x04;
	    showsyms[S_weak_sight_trap] = 0x04;
	    showsyms[S_random_message_trap] = 0x04;

	    showsyms[S_desecration_trap] = 0x04;
	    showsyms[S_starvation_trap] = 0x04;
	    showsyms[S_dropless_trap] = 0x04;
	    showsyms[S_low_effect_trap] = 0x04;
	    showsyms[S_invisible_trap] = 0x04;
	    showsyms[S_ghost_world_trap] = 0x04;
	    showsyms[S_dehydration_trap] = 0x04;
	    showsyms[S_hate_trap] = 0x04;
	    showsyms[S_spacewars_trap] = 0x04;
	    showsyms[S_temporary_recursion_trap] = 0x04;
	    showsyms[S_totter_trap] = 0x04;
	    showsyms[S_nonintrinsical_trap] = 0x04;
	    showsyms[S_dropcurse_trap] = 0x04;
	    showsyms[S_nakedness_trap] = 0x04;
	    showsyms[S_antilevel_trap] = 0x04;
	    showsyms[S_ventilator] = 0x04;
	    showsyms[S_stealer_trap] = 0x04;
	    showsyms[S_rebellion_trap] = 0x04;
	    showsyms[S_crap_trap] = 0x04;
	    showsyms[S_misfire_trap] = 0x04;
	    showsyms[S_trap_of_walls] = 0x04;

	    showsyms[S_disconnect_trap] = 0x04;
	    showsyms[S_interface_screw_trap] = 0x04;
	    showsyms[S_dimness_trap] = 0x04;
	    showsyms[S_evil_artifact_trap] = 0x04;
	    showsyms[S_bossfight_trap] = 0x04;
	    showsyms[S_entire_level_trap] = 0x04;
	    showsyms[S_bones_trap] = 0x04;
	    showsyms[S_rodney_trap] = 0x04;
	    showsyms[S_autocurse_trap] = 0x04;
	    showsyms[S_highlevel_trap] = 0x04;
	    showsyms[S_spell_forgetting_trap] = 0x04;
	    showsyms[S_sound_effect_trap] = 0x04;

	    showsyms[S_trap_percents] = 0x04;

	    showsyms[S_nexus_trap] = 0x04;
	    showsyms[S_leg_trap] = 0x04;
	    showsyms[S_artifact_jackpot_trap] = 0x04;
	    showsyms[S_map_amnesia_trap] = 0x04;
	    showsyms[S_spreading_trap] = 0x04;
	    showsyms[S_adjacent_trap] = 0x04;
	    showsyms[S_superthing_trap] = 0x04;
	    showsyms[S_current_shaft] = 0x04;
	    showsyms[S_levitation_trap] = 0x04;
	    showsyms[S_bowel_cramps_trap] = 0x04;
	    showsyms[S_unequipping_trap] = 0x04;
	    showsyms[S_good_artifact_trap] = 0x04;
	    showsyms[S_gender_trap] = 0x04;
	    showsyms[S_trap_of_oa] = 0x04;
	    showsyms[S_sincount_trap] = 0x04;
	    showsyms[S_beamer_trap] = 0x04;
	    showsyms[S_level_beamer] = 0x04;
	    showsyms[S_pet_trap] = 0x04;
	    showsyms[S_piercing_beam_trap] = 0x04;
	    showsyms[S_wrenching_trap] = 0x04;
	    showsyms[S_tracker_trap] = 0x04;
	    showsyms[S_nurse_trap] = 0x04;
	    showsyms[S_back_to_start_trap] = 0x04;
	    showsyms[S_nemesis_trap] = 0x04;
	    showsyms[S_strew_trap] = 0x04;
	    showsyms[S_outta_depth_trap] = 0x04;
	    showsyms[S_punishment_trap] = 0x04;
	    showsyms[S_boon_trap] = 0x04;
	    showsyms[S_fountain_trap] = 0x04;
	    showsyms[S_throne_trap] = 0x04;
	    showsyms[S_anoxic_pit] = 0x04;
	    showsyms[S_arabella_speaker] = 0x04;
	    showsyms[S_femmy_trap] = 0x04;
	    showsyms[S_madeleine_trap] = 0x04;
	    showsyms[S_marlena_trap] = 0x04;
	    showsyms[S_anastasia_trap] = 0x04;
	    showsyms[S_filler_trap] = 0x04;
	    showsyms[S_toxic_venom_trap] = 0x04;
	    showsyms[S_insanity_trap] = 0x04;
	    showsyms[S_madness_trap] = 0x04;
	    showsyms[S_jessica_trap] = 0x04;
	    showsyms[S_solvejg_trap] = 0x04;
	    showsyms[S_wendy_trap] = 0x04;
	    showsyms[S_katharina_trap] = 0x04;
	    showsyms[S_elena_trap] = 0x04;
	    showsyms[S_thai_trap] = 0x04;
	    showsyms[S_elif_trap] = 0x04;
	    showsyms[S_nadja_trap] = 0x04;
	    showsyms[S_sandra_trap] = 0x04;
	    showsyms[S_natalje_trap] = 0x04;
	    showsyms[S_jeanetta_trap] = 0x04;
	    showsyms[S_yvonne_trap] = 0x04;
	    showsyms[S_maurah_trap] = 0x04;

	    showsyms[S_lootcut_trap] = 0x04;
	    showsyms[S_monster_speed_trap] = 0x04;
	    showsyms[S_scaling_trap] = 0x04;
	    showsyms[S_enmity_trap] = 0x04;
	    showsyms[S_white_spell_trap] = 0x04;
	    showsyms[S_gray_spell_trap] = 0x04;
	    showsyms[S_quasar_trap] = 0x04;
	    showsyms[S_momma_trap] = 0x04;
	    showsyms[S_horror_trap] = 0x04;
	    showsyms[S_artificer_trap] = 0x04;
	    showsyms[S_wereform_trap] = 0x04;
	    showsyms[S_nonprayer_trap] = 0x04;
	    showsyms[S_evil_patch_trap] = 0x04;
	    showsyms[S_hard_mode_trap] = 0x04;
	    showsyms[S_secret_attack_trap] = 0x04;
	    showsyms[S_eater_trap] = 0x04;
	    showsyms[S_covetous_trap] = 0x04;
	    showsyms[S_not_seen_trap] = 0x04;
	    showsyms[S_dark_mode_trap] = 0x04;
	    showsyms[S_antisearch_trap] = 0x04;
	    showsyms[S_homicide_trap] = 0x04;
	    showsyms[S_nasty_nation_trap] = 0x04;
	    showsyms[S_wakeup_call_trap] = 0x04;
	    showsyms[S_grayout_trap] = 0x04;
	    showsyms[S_gray_center_trap] = 0x04;
	    showsyms[S_checkerboard_trap] = 0x04;
	    showsyms[S_clockwise_spin_trap] = 0x04;
	    showsyms[S_counterclockwise_trap] = 0x04;
	    showsyms[S_lag_trap] = 0x04;
	    showsyms[S_blesscurse_trap] = 0x04;
	    showsyms[S_delight_trap] = 0x04;
	    showsyms[S_discharge_trap] = 0x04;
	    showsyms[S_trashing_trap] = 0x04;
	    showsyms[S_filtering_trap] = 0x04;
	    showsyms[S_deformatting_trap] = 0x04;
	    showsyms[S_flicker_strip_trap] = 0x04;
	    showsyms[S_undressing_trap] = 0x04;
	    showsyms[S_hyperbluewall_trap] = 0x04;
	    showsyms[S_nolite_trap] = 0x04;
	    showsyms[S_paranoia_trap] = 0x04;
	    showsyms[S_fleecescript_trap] = 0x04;
	    showsyms[S_interrupt_trap] = 0x04;
	    showsyms[S_dustbin_trap] = 0x04;
	    showsyms[S_mana_battery_trap] = 0x04;
	    showsyms[S_monsterfingers_trap] = 0x04;
	    showsyms[S_miscast_trap] = 0x04;
	    showsyms[S_message_suppression_trap] = 0x04;
	    showsyms[S_stuck_announcement_trap] = 0x04;
	    showsyms[S_bloodthirsty_trap] = 0x04;
	    showsyms[S_maximum_damage_trap] = 0x04;
	    showsyms[S_latency_trap] = 0x04;
	    showsyms[S_starlit_trap] = 0x04;
	    showsyms[S_knowledge_trap] = 0x04;
	    showsyms[S_highscore_trap] = 0x04;
	    showsyms[S_pink_spell_trap] = 0x04;
	    showsyms[S_green_spell_trap] = 0x04;
	    showsyms[S_evc_trap] = 0x04;
	    showsyms[S_underlayer_trap] = 0x04;
	    showsyms[S_damage_meter_trap] = 0x04;
	    showsyms[S_arbitrary_weight_trap] = 0x04;
	    showsyms[S_fucked_info_trap] = 0x04;
	    showsyms[S_black_spell_trap] = 0x04;
	    showsyms[S_cyan_spell_trap] = 0x04;
	    showsyms[S_heap_trap] = 0x04;
	    showsyms[S_blue_spell_trap] = 0x04;
	    showsyms[S_tron_trap] = 0x04;
	    showsyms[S_red_spell_trap] = 0x04;
	    showsyms[S_too_heavy_trap] = 0x04;
	    showsyms[S_elongation_trap] = 0x04;
	    showsyms[S_wrapover_trap] = 0x04;
	    showsyms[S_destruction_trap] = 0x04;
	    showsyms[S_melee_prefix_trap] = 0x04;
	    showsyms[S_automore_trap] = 0x04;
	    showsyms[S_unfair_attack_trap] = 0x04;

	    showsyms[S_kop_cube] = 0x04;
	    showsyms[S_boss_spawner] = 0x04;
	    showsyms[S_contamination_trap] = 0x04;

	    showsyms[S_evil_heel_trap] = 0x04;
	    showsyms[S_bad_equipment_trap] = 0x04;
	    showsyms[S_tempoconflict_trap] = 0x04;
	    showsyms[S_tempohunger_trap] = 0x04;
	    showsyms[S_teleportitis_trap] = 0x04;
	    showsyms[S_polymorphitis_trap] = 0x04;
	    showsyms[S_premature_death_trap] = 0x04;
	    showsyms[S_lasting_amnesia_trap] = 0x04;
	    showsyms[S_ragnarok_trap] = 0x04;
	    showsyms[S_single_disenchant_trap] = 0x04;
	    showsyms[S_acid_pit] = 0x04;

	    showsyms[S_severe_disenchant_trap] = 0x04;
	    showsyms[S_pain_trap] = 0x04;
	    showsyms[S_trembling_trap] = 0x04;
	    showsyms[S_techcap_trap] = 0x04;
	    showsyms[S_spell_memory_trap] = 0x04;
	    showsyms[S_skill_reduction_trap] = 0x04;
	    showsyms[S_skillcap_trap] = 0x04;
	    showsyms[S_permanent_stat_damage_trap] = 0x04;

	    showsyms[S_orange_spell_trap] = 0x04;
	    showsyms[S_violet_spell_trap] = 0x04;
	    showsyms[S_trap_of_longing] = 0x04;
	    showsyms[S_cursed_part_trap] = 0x04;
	    showsyms[S_quaversal_trap] = 0x04;
	    showsyms[S_appearance_shuffling_trap] = 0x04;
	    showsyms[S_brown_spell_trap] = 0x04;
	    showsyms[S_choiceless_trap] = 0x04;
	    showsyms[S_goldspell_trap] = 0x04;
	    showsyms[S_deprovement_trap] = 0x04;
	    showsyms[S_initialization_trap] = 0x04;
	    showsyms[S_gushlush_trap] = 0x04;
	    showsyms[S_soiltype_trap] = 0x04;
	    showsyms[S_dangerous_terrain_trap] = 0x04;
	    showsyms[S_fallout_trap] = 0x04;
	    showsyms[S_mojibake_trap] = 0x04;
	    showsyms[S_gravation_trap] = 0x04;
	    showsyms[S_uncalled_trap] = 0x04;
	    showsyms[S_exploding_dice_trap] = 0x04;
	    showsyms[S_permacurse_trap] = 0x04;
	    showsyms[S_shrouded_identity_trap] = 0x04;
	    showsyms[S_feeler_gauges_trap] = 0x04;
	    showsyms[S_long_screwup_trap] = 0x04;
	    showsyms[S_wing_yellow_changer] = 0x04;
	    showsyms[S_life_saving_trap] = 0x04;
	    showsyms[S_curseuse_trap] = 0x04;
	    showsyms[S_cut_nutrition_trap] = 0x04;
	    showsyms[S_skill_loss_trap] = 0x04;
	    showsyms[S_autopilot_trap] = 0x04;
	    showsyms[S_force_trap] = 0x04;
	    showsyms[S_monster_glyph_trap] = 0x04;
	    showsyms[S_changing_directive_trap] = 0x04;
	    showsyms[S_container_kaboom_trap] = 0x04;
	    showsyms[S_steal_degrade_trap] = 0x04;
	    showsyms[S_left_inventory_trap] = 0x04;
	    showsyms[S_fluctuating_speed_trap] = 0x04;
	    showsyms[S_tarmustrokingnora_trap] = 0x04;
	    showsyms[S_failure_trap] = 0x04;
	    showsyms[S_bright_cyan_spell_trap] = 0x04;
	    showsyms[S_frequentation_spawn_trap] = 0x04;
	    showsyms[S_pet_ai_trap] = 0x04;
	    showsyms[S_satan_trap] = 0x04;
	    showsyms[S_rememberance_trap] = 0x04;
	    showsyms[S_pokelie_trap] = 0x04;
	    showsyms[S_autopickup_trap] = 0x04;
	    showsyms[S_dywypi_trap] = 0x04;
	    showsyms[S_silver_spell_trap] = 0x04;
	    showsyms[S_metal_spell_trap] = 0x04;
	    showsyms[S_platinum_spell_trap] = 0x04;
	    showsyms[S_manler_trap] = 0x04;
	    showsyms[S_doorning_trap] = 0x04;
	    showsyms[S_nownsible_trap] = 0x04;
	    showsyms[S_elm_street_trap] = 0x04;
	    showsyms[S_monnoise_trap] = 0x04;
	    showsyms[S_rang_call_trap] = 0x04;
	    showsyms[S_recurring_spell_loss_trap] = 0x04;
	    showsyms[S_antitraining_trap] = 0x04;
	    showsyms[S_techout_trap] = 0x04;
	    showsyms[S_stat_decay_trap] = 0x04;
	    showsyms[S_movemork_trap] = 0x04;

	    showsyms[S_hybrid_trap] = 0x04;
	    showsyms[S_shapechange_trap] = 0x04;
	    showsyms[S_meltem_trap] = 0x04;
	    showsyms[S_miguc_trap] = 0x04;
	    showsyms[S_directive_trap] = 0x04;
	    showsyms[S_satatue_trap] = 0x04;
	    showsyms[S_farting_web] = 0x04;
	    showsyms[S_cataclysm_trap] = 0x04;
	    showsyms[S_data_delete_trap] = 0x04;
	    showsyms[S_elder_tentacling_trap] = 0x04;
	    showsyms[S_footerer_trap] = 0x04;

	    showsyms[S_bad_part_trap] = 0x04;
	    showsyms[S_completely_bad_part_trap] = 0x04;
	    showsyms[S_evil_variant_trap] = 0x04;

	    showsyms[S_grave_wall_trap] = 0x04;
	    showsyms[S_tunnel_trap] = 0x04;
	    showsyms[S_farmland_trap] = 0x04;
	    showsyms[S_mountain_trap] = 0x04;
	    showsyms[S_water_tunnel_trap] = 0x04;
	    showsyms[S_crystal_flood_trap] = 0x04;
	    showsyms[S_moorland_trap] = 0x04;
	    showsyms[S_urine_trap] = 0x04;
	    showsyms[S_shifting_sand_trap] = 0x04;
	    showsyms[S_styx_trap] = 0x04;
	    showsyms[S_pentagram_trap] = 0x04;
	    showsyms[S_snow_trap] = 0x04;
	    showsyms[S_ash_trap] = 0x04;
	    showsyms[S_sand_trap] = 0x04;
	    showsyms[S_pavement_trap] = 0x04;
	    showsyms[S_highway_trap] = 0x04;
	    showsyms[S_grassland_trap] = 0x04;
	    showsyms[S_nether_mist_trap] = 0x04;
	    showsyms[S_stalactite_trap] = 0x04;
	    showsyms[S_cryptfloor_trap] = 0x04;
	    showsyms[S_bubble_trap] = 0x04;
	    showsyms[S_rain_cloud_trap] = 0x04;
	    showsyms[S_item_nastification_trap] = 0x04;
	    showsyms[S_sanity_increase_trap] = 0x04;
	    showsyms[S_psi_trap] = 0x04;
	    showsyms[S_gay_trap] = 0x04;
	    showsyms[S_sarah_trap] = 0x04;
	    showsyms[S_claudia_trap] = 0x04;
	    showsyms[S_ludgera_trap] = 0x04;
	    showsyms[S_kati_trap] = 0x04;
	    showsyms[S_sanity_treble_trap] = 0x04;
	    showsyms[S_stat_decrease_trap] = 0x04;
	    showsyms[S_simeout_trap] = 0x04;
	    showsyms[S_spressing_trap] = 0x04;
	    showsyms[S_nelly_trap] = 0x04;
	    showsyms[S_eveline_trap] = 0x04;
	    showsyms[S_karin_trap] = 0x04;
	    showsyms[S_juen_trap] = 0x04;
	    showsyms[S_kristina_trap] = 0x04;
	    showsyms[S_lou_trap] = 0x04;
	    showsyms[S_almut_trap] = 0x04;
	    showsyms[S_julietta_trap] = 0x04;
	    showsyms[S_arabella_trap] = 0x04;
	    showsyms[S_kristin_trap] = 0x04;
	    showsyms[S_anna_trap] = 0x04;
	    showsyms[S_ruea_trap] = 0x04;
	    showsyms[S_dora_trap] = 0x04;
	    showsyms[S_marike_trap] = 0x04;
	    showsyms[S_jette_trap] = 0x04;
	    showsyms[S_ina_trap] = 0x04;
	    showsyms[S_sing_trap] = 0x04;
	    showsyms[S_victoria_trap] = 0x04;
	    showsyms[S_melissa_trap] = 0x04;
	    showsyms[S_anita_trap] = 0x04;
	    showsyms[S_henrietta_trap] = 0x04;
	    showsyms[S_verena_trap] = 0x04;
	    showsyms[S_annemarie_trap] = 0x04;
	    showsyms[S_jil_trap] = 0x04;
	    showsyms[S_jana_trap] = 0x04;
	    showsyms[S_katrin_trap] = 0x04;
	    showsyms[S_gudrun_trap] = 0x04;
	    showsyms[S_ella_trap] = 0x04;
	    showsyms[S_manuela_trap] = 0x04;
	    showsyms[S_jennifer_trap] = 0x04;
	    showsyms[S_patricia_trap] = 0x04;
	    showsyms[S_antje_trap] = 0x04;
	    showsyms[S_antje_x_trap] = 0x04;
	    showsyms[S_kerstin_trap] = 0x04;
	    showsyms[S_laura_trap] = 0x04;
	    showsyms[S_larissa_trap] = 0x04;
	    showsyms[S_nora_trap] = 0x04;
	    showsyms[S_natalia_trap] = 0x04;
	    showsyms[S_susanne_trap] = 0x04;
	    showsyms[S_lisa_trap] = 0x04;
	    showsyms[S_bridghitte_trap] = 0x04;
	    showsyms[S_julia_trap] = 0x04;
	    showsyms[S_nicole_trap] = 0x04;
	    showsyms[S_rita_trap] = 0x04;
	    showsyms[S_janina_trap] = 0x04;
	    showsyms[S_rosa_trap] = 0x04;
	    showsyms[S_ksenia_trap] = 0x04;
	    showsyms[S_lydia_trap] = 0x04;
	    showsyms[S_conny_trap] = 0x04;
	    showsyms[S_katia_trap] = 0x04;
	    showsyms[S_mariya_trap] = 0x04;
	    showsyms[S_elise_trap] = 0x04;
	    showsyms[S_ronja_trap] = 0x04;
	    showsyms[S_ariane_trap] = 0x04;
	    showsyms[S_johanna_trap] = 0x04;
	    showsyms[S_inge_trap] = 0x04;

	    showsyms[S_wall_trap] = 0x04;
	    showsyms[S_monster_generator] = 0x04;
	    showsyms[S_potion_dispenser] = 0x04;
	    showsyms[S_spacewars_spawn_trap] = 0x04;
	    showsyms[S_tv_tropes_trap] = 0x04;
	    showsyms[S_symbiote_trap] = 0x04;
	    showsyms[S_kill_symbiote_trap] = 0x04;
	    showsyms[S_symbiote_replacement_trap] = 0x04;
	    showsyms[S_shutdown_trap] = 0x04;
	    showsyms[S_corona_trap] = 0x04;
	    showsyms[S_unproofing_trap] = 0x04;
	    showsyms[S_visibility_trap] = 0x04;
	    showsyms[S_feminism_stone_trap] = 0x04;
	    showsyms[S_branch_teleporter] = 0x04;
	    showsyms[S_branch_beamer] = 0x04;
	    showsyms[S_poison_arrow_trap] = 0x04;
	    showsyms[S_poison_bolt_trap] = 0x04;
	    showsyms[S_mace_trap] = 0x04;
	    showsyms[S_shueft_trap] = 0x04;
	    showsyms[S_moth_larvae_trap] = 0x04;
	    showsyms[S_worthiness_trap] = 0x04;
	    showsyms[S_conduct_trap] = 0x04;
	    showsyms[S_strikethrough_trap] = 0x04;
	    showsyms[S_multiple_gather_trap] = 0x04;
	    showsyms[S_vivisection_trap] = 0x04;
	    showsyms[S_instafeminism_trap] = 0x04;
	    showsyms[S_instanasty_trap] = 0x04;
	    showsyms[S_skill_point_loss_trap] = 0x04;
	    showsyms[S_perfect_match_trap] = 0x04;
	    showsyms[S_dumbie_lightsaber_trap] = 0x04;
	    showsyms[S_wrong_stairs] = 0x04;
	    showsyms[S_techstop_trap] = 0x04;
	    showsyms[S_amnesia_switch_trap] = 0x04;
	    showsyms[S_skill_swap_trap] = 0x04;
	    showsyms[S_skill_upordown_trap] = 0x04;
	    showsyms[S_skill_randomize_trap] = 0x04;

	    showsyms[S_skill_multiply_trap] = 0x04;
	    showsyms[S_trapwalk_trap] = 0x04;
	    showsyms[S_cluster_trap] = 0x04;
	    showsyms[S_field_trap] = 0x04;
	    showsyms[S_monicide_trap] = 0x04;
	    showsyms[S_trap_creation_trap] = 0x04;
	    showsyms[S_leold_trap] = 0x04;
	    showsyms[S_animeband_trap] = 0x04;
	    showsyms[S_perfume_trap] = 0x04;
	    showsyms[S_court_trap] = 0x04;
	    showsyms[S_elder_scrolls_trap] = 0x04;
	    showsyms[S_joke_trap] = 0x04;
	    showsyms[S_dungeon_lords_trap] = 0x04;
	    showsyms[S_fortytwo_trap] = 0x04;
	    showsyms[S_randomize_trap] = 0x04;
	    showsyms[S_evilroom_trap] = 0x04;
	    showsyms[S_aoe_trap] = 0x04;
	    showsyms[S_elona_trap] = 0x04;
	    showsyms[S_religion_trap] = 0x04;
	    showsyms[S_steamband_trap] = 0x04;
	    showsyms[S_hardcore_trap] = 0x04;
	    showsyms[S_machine_trap] = 0x04;
	    showsyms[S_bee_trap] = 0x04;
	    showsyms[S_migo_trap] = 0x04;
	    showsyms[S_angband_trap] = 0x04;
	    showsyms[S_dnethack_trap] = 0x04;
	    showsyms[S_evil_spawn_trap] = 0x04;
	    showsyms[S_shoe_trap] = 0x04;
	    showsyms[S_inside_trap] = 0x04;
	    showsyms[S_doom_trap] = 0x04;
	    showsyms[S_military_trap] = 0x04;
	    showsyms[S_illusion_trap] = 0x04;
	    showsyms[S_diablo_trap] = 0x04;

	    showsyms[S_giant_explorer_trap] = 0x04;
	    showsyms[S_trapwarp_trap] = 0x04;
	    showsyms[S_yawm_trap] = 0x04;
	    showsyms[S_cradle_of_chaos_trap] = 0x04;
	    showsyms[S_tez_trap] = 0x04;
	    showsyms[S_enthu_trap] = 0x04;
	    showsyms[S_mikra_trap] = 0x04;
	    showsyms[S_gots_too_good_trap] = 0x04;
	    showsyms[S_killer_room_trap] = 0x04;
	    showsyms[S_no_fun_walls_trap] = 0x04;

	    showsyms[S_ruth_trap] = 0x04;
	    showsyms[S_magdalena_trap] = 0x04;
	    showsyms[S_marleen_trap] = 0x04;
	    showsyms[S_klara_trap] = 0x04;
	    showsyms[S_friederike_trap] = 0x04;
	    showsyms[S_naomi_trap] = 0x04;
	    showsyms[S_ute_trap] = 0x04;
	    showsyms[S_jasieen_trap] = 0x04;
	    showsyms[S_yasaman_trap] = 0x04;
	    showsyms[S_may_britt_trap] = 0x04;

	    showsyms[S_calling_out_trap] = 0x04;
	    showsyms[S_field_break_trap] = 0x04;
	    showsyms[S_tenth_trap] = 0x04;
	    showsyms[S_debt_trap] = 0x04;
	    showsyms[S_inversion_trap] = 0x04;
	    showsyms[S_wince_trap] = 0x04;
	    showsyms[S_u_have_been_trap] = 0x04;
	    showsyms[S_really_bad_trap] = 0x04;
	    showsyms[S_covid_trap] = 0x04;
	    showsyms[S_artiblast_trap] = 0x04;

	    showsyms[S_persistent_fart_trap] = 0x04;
	    showsyms[S_attacking_heel_trap] = 0x04;
	    showsyms[S_trap_teleporter] = 0x04;
	    showsyms[S_alignment_trash_trap] = 0x04;
	    showsyms[S_reshuffle_trap] = 0x04;
	    showsyms[S_musehand_trap] = 0x04;
	    showsyms[S_dogside_trap] = 0x04;
	    showsyms[S_bankrupt_trap] = 0x04;
	    showsyms[S_fillup_trap] = 0x04;
	    showsyms[S_airstrike_trap] = 0x04;
	    showsyms[S_dynamite_trap] = 0x04;
	    showsyms[S_malevolence_trap] = 0x04;
	    showsyms[S_leaflet_trap] = 0x04;
	    showsyms[S_tentadeep_trap] = 0x04;
	    showsyms[S_stathalf_trap] = 0x04;
	    showsyms[S_cutstat_trap] = 0x04;
	    showsyms[S_rare_spawn_trap] = 0x04;
	    showsyms[S_you_are_an_idiot_trap] = 0x04;
	    showsyms[S_nastycurse_trap] = 0x04;
	    showsyms[S_repeating_nastycurse_trap] = 0x04;

	    showsyms[S_nadine_trap] = 0x04;
	    showsyms[S_luisa_trap] = 0x04;
	    showsyms[S_irina_trap] = 0x04;
	    showsyms[S_liselotte_trap] = 0x04;
	    showsyms[S_greta_trap] = 0x04;
	    showsyms[S_jane_trap] = 0x04;
	    showsyms[S_sue_lyn_trap] = 0x04;
	    showsyms[S_charlotte_trap] = 0x04;
	    showsyms[S_hannah_trap] = 0x04;
	    showsyms[S_little_marie_trap] = 0x04;

	    showsyms[S_real_lie_trap] = 0x04;
	    showsyms[S_escape_past_trap] = 0x04;
	    showsyms[S_pethate_trap] = 0x04;
	    showsyms[S_pet_lashout_trap] = 0x04;
	    showsyms[S_petstarve_trap] = 0x04;
	    showsyms[S_petscrew_trap] = 0x04;
	    showsyms[S_tech_loss_trap] = 0x04;
	    showsyms[S_proofloss_trap] = 0x04;
	    showsyms[S_un_invis_trap] = 0x04;
	    showsyms[S_detectation_trap] = 0x04;

	    showsyms[S_option_trap] = 0x04;
	    showsyms[S_miscolor_trap] = 0x04;
	    showsyms[S_one_rainbow_trap] = 0x04;
	    showsyms[S_colorshift_trap] = 0x04;
	    showsyms[S_top_line_trap] = 0x04;
	    showsyms[S_caps_trap] = 0x04;
	    showsyms[S_un_knowledge_trap] = 0x04;
	    showsyms[S_darkhance_trap] = 0x04;
	    showsyms[S_dschueueuet_trap] = 0x04;
	    showsyms[S_nopeskill_trap] = 0x04;
	    showsyms[S_fuckfuckfuck_trap] = 0x04;

	    showsyms[S_burden_trap] = 0x04;
	    showsyms[S_magic_vacuum_trap] = 0x04;

	    showsyms[S_tanja_trap] = 0x04;
	    showsyms[S_sonja_trap] = 0x04;
	    showsyms[S_rhea_trap] = 0x04;
	    showsyms[S_lara_trap] = 0x04;

	    showsyms[S_fuck_over_trap] = 0x04;

	    showsyms[S_dagger_trap] = 0x04;
	    showsyms[S_razor_trap] = 0x04;
	    showsyms[S_phosgene_trap] = 0x04;
	    showsyms[S_chloroform_trap] = 0x04;
	    showsyms[S_corrosion_trap] = 0x04;
	    showsyms[S_flame_trap] = 0x04;
	    showsyms[S_wither_trap] = 0x04;
	    showsyms[S_phaseport_trap] = 0x04;
	    showsyms[S_phasebeam_trap] = 0x04;
	    showsyms[S_vulnerate_trap] = 0x04;

	    showsyms[S_epvi_trap] = 0x04;

	    showsyms[S_sabrina_trap] = 0x04;

	    showsyms[S_aefde_trap] = 0x04;

	    showsyms[S_falling_rock_cold_trap] = 0x04;
	    showsyms[S_return_trap] = 0x04;
	    showsyms[S_intrinsic_steal_trap] = 0x04;
	    showsyms[S_score_axe_trap] = 0x04;
	    showsyms[S_score_drain_trap] = 0x04;
	    showsyms[S_single_unidentify_trap] = 0x04;
	    showsyms[S_hypoxic_pit] = 0x04;
	    showsyms[S_unlucky_trap] = 0x04;
	    showsyms[S_alignment_reduction_trap] = 0x04;
	    showsyms[S_malignant_trap] = 0x04;
	    showsyms[S_stat_damage_trap] = 0x04;
	    showsyms[S_half_memory_trap] = 0x04;
	    showsyms[S_half_training_trap] = 0x04;
	    showsyms[S_debuff_trap] = 0x04;
	    showsyms[S_trip_once_trap] = 0x04;
	    showsyms[S_narcolepsy_trap] = 0x04;
	    showsyms[S_martial_arts_trap] = 0x04;
	    showsyms[S_mean_burden_trap] = 0x04;
	    showsyms[S_carrcap_trap] = 0x04;
	    showsyms[S_umeng_trap] = 0x04;

	    showsyms[S_spell_cooldown_trap] = 0x04;
	    showsyms[S_turbodull_trap] = 0x04;
	    showsyms[S_reddam_trap] = 0x04;
	    showsyms[S_redinc_trap] = 0x04;
	    showsyms[S_mulch_trap] = 0x04;
	    showsyms[S_inaccuracy_trap] = 0x04;
	    showsyms[S_monster_multishot_trap] = 0x04;
	    showsyms[S_have_a_pin_trap] = 0x04;
	    showsyms[S_costly_failure_trap] = 0x04;
	    showsyms[S_inventory_size_trap] = 0x04;

	    showsyms[S_spike_trap] = 0x04;
	    showsyms[S_paper_arrow_trap] = 0x04;
	    showsyms[S_knife_trap] = 0x04;
	    showsyms[S_summon_monster_trap] = 0x04;
	    showsyms[S_red_damage_trap] = 0x04;
	    showsyms[S_green_damage_trap] = 0x04;
	    showsyms[S_yellow_damage_trap] = 0x04;
	    showsyms[S_white_damage_trap] = 0x04;
	    showsyms[S_blue_damage_trap] = 0x04;
	    showsyms[S_multi_debuff_trap] = 0x04;

	    showsyms[S_magic_damage_trap] = 0x04;

	    showsyms[S_wild_weather_trap] = 0x04;

 	    showsyms[S_timerun_trap] = 0x04;

	    } /* epyx mode */

#endif /* MSWIN */
	}
#endif /* ASCIIGRAPH */

	for (i = 0; i < MAXOCLASSES; i++) {
#if defined(ASCIIGRAPH) && !defined(LINUX)
	    if (iflags.IBMgraphics
# if defined(USE_TILES) && defined(MSDOS)
		&& !iflags.grmode
# endif
		)
		oc_syms[i] = IBM_r_oc_syms[i];
	    else
#endif /* ASCIIGRAPH && !LINUX */
		oc_syms[i] = r_oc_syms[i];
	}
#if defined(MSDOS) && defined(USE_TILES)
	if (iflags.grmode) tileview(TRUE);
#endif
    } else {
	(void) memcpy((void *)showsyms,
		      (void *)save_showsyms, sizeof showsyms);
	(void) memcpy((void *)oc_syms,
		      (void *)save_oc_syms, sizeof oc_syms);
	(void) memcpy((void *)monsyms,
		      (void *)save_monsyms, sizeof monsyms);
#if defined(MSDOS) && defined(USE_TILES)
	if (iflags.grmode) tileview(TRUE);
#endif
    }
}
#endif /* REINCARNATION */

/*drawing.c*/
