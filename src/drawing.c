/*	SCCS Id: @(#)drawing.c	3.4	1999/12/02	*/
/* Copyright (c) NetHack Development Team 1992.			  */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
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
	TOOL_SYM,
	FOOD_SYM,
	POTION_SYM,
	SCROLL_SYM,
/*10*/	SPBOOK_SYM,
	WAND_SYM,
	GOLD_SYM,
	GEM_SYM,
	ROCK_SYM,
/*15*/	BALL_SYM,
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
	"useful item (pick-axe, key, lamp...)",
	"piece of food",
	"potion",
	"scroll",
/*10*/	"spell book",
	"wand",
	"pile of coins",
	"gem or rock",
	"boulder or statue",
/*15*/	"iron ball",
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
	"tools",
	"food",
	"potions",
	"scrolls",
/*10*/	"spell books",
	"wands",
	"coins",
	"rocks",
	"large stones",
/*15*/	"iron balls",
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
	{'.', "doorway",	C(CLR_GRAY)},	/* ndoor */
	{'-', "open door",	C(CLR_BROWN)},	/* vodoor */
	{'|', "open door",	C(CLR_BROWN)},	/* hodoor */
	{'+', "closed door",	C(CLR_BROWN)},	/* vcdoor */
	{'+', "closed door",	C(CLR_BROWN)},	/* hcdoor */
	{'#', "iron bars",	C(HI_METAL)},	/* bars */
	{'#', "tree",		C(CLR_GREEN)},	/* tree */
	{'.', "floor of a room",C(CLR_GRAY)},	/* room */
	{'.', "dark part of a room",C(CLR_BLACK)},	/* dark room */
/*20*/	{',', "corridor",	C(CLR_BLACK)},	/* dark corr */
	{',', "lit corridor",	C(CLR_GRAY)},	/* lit corr (see mapglyph.c) */
	{'<', "staircase up",	C(CLR_WHITE)},	/* upstair */
	{'>', "staircase down",	C(CLR_WHITE)},	/* dnstair */
	{'<', "ladder up",	C(CLR_BROWN)},	/* upladder */
	{'>', "ladder down",	C(CLR_BROWN)},	/* dnladder */
	{'_', "altar",		C(CLR_GRAY)},	/* altar */
	{'|', "grave",      C(CLR_GRAY)},   /* grave */
	{'\\', "opulent throne",C(HI_GOLD)},	/* throne */
#ifdef SINKS
	{'{', "sink",		C(CLR_ORANGE)},	/* sink */
/*30*/	{'{', "toilet",         C(CLR_YELLOW)},  /* toilet */
#else
	{'#', "",		C(CLR_GRAY)},	/* sink */
/*30*/	{'\\', "",              C(CLR_YELLOW)},  /* toilet */
#endif
	{'{', "fountain",       C(CLR_BRIGHT_BLUE)},   /* fountain */
	{'}', "water",		C(CLR_BRIGHT_BLUE)},	/* pool */
	{'.', "ice",		C(CLR_CYAN)},	/* ice */
	{'}', "molten lava",	C(CLR_RED)},	/* lava */
	{'.', "lowered drawbridge",C(CLR_ORANGE)},	/* vodbridge */
	{'.', "lowered drawbridge",C(CLR_ORANGE)},	/* hodbridge */
	{'#', "raised drawbridge",C(CLR_ORANGE)},/* vcdbridge */
	{'#', "raised drawbridge",C(CLR_ORANGE)},/* hcdbridge */
	{'`', "air",		C(CLR_YELLOW)},	/* open air */
/*40*/	{'`', "cloud",		C(CLR_CYAN)},	/* [part of] a cloud */
	{'#', "solid rock",		C(CLR_BROWN)},	/* dungwall */
	{'}', "water",		C(CLR_BRIGHT_BLUE)},	/* under water */
	{'^', "arrow trap",	C(HI_METAL)},	/* trap */
	{'^', "dart trap",	C(HI_METAL)},	/* trap */
	{'^', "falling rock trap",C(CLR_GRAY)},	/* trap */
	{'^', "squeaky board",	C(CLR_BROWN)},	/* trap */
	{'^', "bear trap",	C(HI_METAL)},	/* trap */
	{'^', "land mine",	C(CLR_RED)},	/* trap */
	{'^', "rolling boulder trap",	C(CLR_GRAY)},	/* trap */
	{'^', "sleeping gas trap",C(HI_ZAP)},	/* trap */
/*50*/	{'^', "rust trap",	C(CLR_BRIGHT_BLUE)},	/* trap */
	{'^', "fire trap",	C(CLR_ORANGE)},	/* trap */
	{'^', "pit",		C(CLR_BLACK)},	/* trap */
	{'^', "spiked pit",	C(CLR_BLACK)},	/* trap */
	{'^', "hole",	C(CLR_BROWN)},	/* trap */
	{'^', "trap door",	C(CLR_BROWN)},	/* trap */
	{'^', "teleportation trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "level teleporter", C(CLR_MAGENTA)},	/* trap */
	{'^', "magic portal",	C(CLR_BRIGHT_MAGENTA)},	/* trap */
	{'^', "web",		C(CLR_WHITE)},	/* web */
/*60*/	{'^', "statue trap",	C(CLR_GRAY)},	/* trap */
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
	{'^', "wishing trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "amnesia trap", C(CLR_MAGENTA)},	/* trap */
	{'^', "bigscript trap", C(CLR_WHITE)},	/* trap */
	{'^', "bank trap", C(CLR_BROWN)},	/* trap */
	{'^', "Only %a%s can go there!", C(CLR_RED)},	/* trap */
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

	{'^', "trap %s", C(CLR_WHITE)},	/* trap */
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
void NDECL((*ibmgraphics_mode_callback)) = 0;	/* set in tty_start_screen() */
#endif /* PC9800 */

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
	0xfa,	/* S_ndoor:	meta-z, centered dot */
	0xfe,	/* S_vodoor:	meta-~, small centered square */
	0xfe,	/* S_hodoor:	meta-~, small centered square */
	g_FILLER(S_vcdoor),
	g_FILLER(S_hcdoor),
	g_FILLER(S_bars),	/* S_bars:	equivalency, courtesy Sascha; but Amy says it just shows as "-" on some systems */
	241,	/* S_tree:	plus or minus symbol */
	0xfa,	/* S_room:	meta-z, centered dot */
	g_FILLER(S_stone),	/* S_darkroom:	meta-z, centered dot */
/*20*/	0xb0,	/* S_corr:	meta-0, light shading */
	0xb1,	/* S_litcorr:	meta-1, medium shading */
	g_FILLER(S_upstair),
	g_FILLER(S_dnstair),
	g_FILLER(S_upladder),
	g_FILLER(S_dnladder),
	g_FILLER(S_altar),
	g_FILLER(S_grave),
	g_FILLER(S_throne),
	0xf4, /*(S_sink)*/
/*30*/	0xf4, /*(S_toilet),*/
	0xf4,	/* S_fountain:	meta-t, integral top half */
	0xf7,	/* S_pool:	meta-w, approx. equals */
	0xfa,	/* S_ice:	meta-z, centered dot */
	0xf7,	/* S_lava:	meta-w, approx. equals */
	0xfa,	/* S_vodbridge:	meta-z, centered dot */
	0xfa,	/* S_hodbridge:	meta-z, centered dot */
	g_FILLER(S_vcdbridge),
	g_FILLER(S_hcdbridge),
	g_FILLER(S_air),
/*40*/	g_FILLER(S_cloud),
	g_FILLER(S_dungwall),	/* dungwall */
	0xf7,	/* S_water:	meta-w, approx. equals */
	g_FILLER(S_arrow_trap),
	g_FILLER(S_dart_trap),
	g_FILLER(S_falling_rock_trap),
	g_FILLER(S_squeaky_board),
	g_FILLER(S_bear_trap),
	g_FILLER(S_land_mine),
	g_FILLER(S_rolling_boulder_trap),
	g_FILLER(S_sleeping_gas_trap),
/*50*/	g_FILLER(S_rust_trap),
	g_FILLER(S_fire_trap),
	g_FILLER(S_pit),
	g_FILLER(S_spiked_pit),
	g_FILLER(S_hole),
	g_FILLER(S_trap_door),
	g_FILLER(S_teleportation_trap),
	g_FILLER(S_level_teleporter),
	g_FILLER(S_magic_portal),
	g_FILLER(S_web),
/*60*/	g_FILLER(S_statue_trap),
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
	g_FILLER(S_automatic switcher),
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

	g_FILLER(S_trap_percents),
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
void NDECL((*decgraphics_mode_callback)) = 0;  /* set in tty_start_screen() */

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
	0xfe,	/* S_ndoor:	meta-~, centered dot */
	0xe1,	/* S_vodoor:	meta-a, solid block */
	0xe1,	/* S_hodoor:	meta-a, solid block */
	g_FILLER(S_vcdoor),
	g_FILLER(S_hcdoor),
	0xfb,	/* S_bars:	meta-{, small pi, courtesy Pat Rankin */
	0xe7,	/* S_tree:	meta-g, plus-or-minus, courtesy Pat Rankin */
	0xfe,	/* S_room:	meta-~, centered dot */
	g_FILLER(S_stone),	/* S_darkroom:	meta-~, centered dot */
/*20*/	g_FILLER(S_corr),
	g_FILLER(S_litcorr),
	g_FILLER(S_upstair),
	g_FILLER(S_dnstair),
	0xf9,	/* S_upladder:	meta-y, greater-than-or-equals */
	0xfa,	/* S_dnladder:	meta-z, less-than-or-equals */
	g_FILLER(S_altar),	/* 0xc3, \E)3: meta-C, dagger */
	g_FILLER(S_grave),
	g_FILLER(S_throne),
	g_FILLER(S_sink),
/*30*/	g_FILLER(S_toilet),
	g_FILLER(S_fountain),	/* 0xdb, \E)3: meta-[, integral top half */
	0xe0,	/* S_pool:	meta-\, diamond */
	0xfe,	/* S_ice:	meta-~, centered dot */
	0xe0,	/* S_lava:	meta-\, diamond */
	0xfe,	/* S_vodbridge:	meta-~, centered dot */
	0xfe,	/* S_hodbridge:	meta-~, centered dot */
	g_FILLER(S_vcdbridge),
	g_FILLER(S_hcdbridge),
	g_FILLER(S_air),
/*40*/	g_FILLER(S_cloud),
	g_FILLER(S_dungwall),	/* dungwall */
	0xe0,	/* S_water:	meta-\, diamond */
	g_FILLER(S_arrow_trap),
	g_FILLER(S_dart_trap),
	g_FILLER(S_falling_rock_trap),
	g_FILLER(S_squeaky_board),
	g_FILLER(S_bear_trap),
	g_FILLER(S_land_mine),
	g_FILLER(S_rolling_boulder_trap),
	g_FILLER(S_sleeping_gas_trap),
/*50*/	g_FILLER(S_rust_trap),
	g_FILLER(S_fire_trap),
	g_FILLER(S_pit),
	g_FILLER(S_spiked_pit),
	g_FILLER(S_hole),
	g_FILLER(S_trap_door),
	g_FILLER(S_teleportation_trap),
	g_FILLER(S_level_teleporter),
	g_FILLER(S_magic_portal),
	g_FILLER(S_web),	/* 0xbd, \E)3: meta-=, int'l currency */
/*60*/	g_FILLER(S_statue_trap),
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
	g_FILLER(S_automatic switcher),
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

	g_FILLER(S_trap_percents),
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
	0xb0,	/* S_ndoor */
	0xee,	/* S_vodoor */
	0xee,	/* S_hodoor */
	0xef,	/* S_vcdoor */
	0xef,	/* S_hcdoor */
	0xf0,	/* S_bars:	equivalency symbol */
	0xf1,	/* S_tree:	plus-or-minus */
	g_FILLER(S_Room),
	g_FILLER(S_stone), /* S_darkroom */
/*20*/	0xB0,	/* S_corr */
	g_FILLER(S_litcorr),
	g_FILLER(S_upstair),
	g_FILLER(S_dnstair),
	g_FILLER(S_upladder),
	g_FILLER(S_dnladder),
	g_FILLER(S_altar),
	0xef,	/* S_grave:	same as open door */
	g_FILLER(S_throne),
	g_FILLER(S_sink),
/*30*/	g_FILLER(S_toilet),
	g_FILLER(S_fountain),
	0xe0,	/* S_pool */
	g_FILLER(S_ice),
	g_FILLER(S_lava),
	g_FILLER(S_vodbridge),
	g_FILLER(S_hodbridge),
	g_FILLER(S_vcdbridge),
	g_FILLER(S_hcdbridge),
	g_FILLER(S_air),
/*40*/	g_FILLER(S_cloud),
	g_FILLER(S_dungwall),	/* dungwall */
	g_FILLER(S_water),
	g_FILLER(S_arrow_trap),
	g_FILLER(S_dart_trap),
	g_FILLER(S_falling_rock_trap),
	g_FILLER(S_squeaky_board),
	g_FILLER(S_bear_trap),
	g_FILLER(S_land_mine),
	g_FILLER(S_rolling_boulder_trap),
	g_FILLER(S_sleeping_gas_trap),
/*50*/	g_FILLER(S_rust_trap),
	g_FILLER(S_fire_trap),
	g_FILLER(S_pit),
	g_FILLER(S_spiked_pit),
	g_FILLER(S_hole),
	g_FILLER(S_trap_door),
	g_FILLER(S_teleportation_trap),
	g_FILLER(S_level_teleporter),
	g_FILLER(S_magic_portal),
	g_FILLER(S_web),
/*60*/	g_FILLER(S_statue_trap),
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
	g_FILLER(S_automatic switcher),
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

	g_FILLER(S_trap_percents),
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
void NDECL((*ascgraphics_mode_callback)) = 0;	/* set in tty_start_screen() */
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

    for (i = 0; i < maxlen; i++)
	showsyms[i+offset] = (((i < glth) && graph_chars[i]) ?
		       graph_chars[i] : defsyms[i+offset].sym);
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
	    assign_graphics(dec_graphics, SIZE(dec_graphics), MAXPCHARS, 0);
	    if (decgraphics_mode_callback) (*decgraphics_mode_callback)();
	    break;
#endif /* TERMLIB */
#ifdef MAC_GRAPHICS_ENV
	case MAC_GRAPHICS:
	    assign_graphics(mac_graphics, SIZE(mac_graphics), MAXPCHARS, 0);
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
	TOOL_SYM,
	':',			/* food */
	POTION_SYM,
	SCROLL_SYM,
/*10*/	SPBOOK_SYM,
	WAND_SYM,
	GEM_SYM,		/* gold -- yes it's the same as gems */
	GEM_SYM,
	ROCK_SYM,
/*15*/	BALL_SYM,
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
/*	0x0a, */ ARMOR_SYM,	/* armor:  Vert rect with o */
/*	0x09, */ RING_SYM,	/* ring:   circle with arrow */
/* 5*/	0x0c,			/* amulet: "female" symbol */
	TOOL_SYM,
	0x05,			/* food:   club (as in cards) */
	0xad,			/* potion: upside down '!' */
	0x0e,			/* scroll: musical note */
/*10*/	SPBOOK_SYM,
	0xe7,			/* wand:   greek tau */
	0x0f,			/* gold:   yes it's the same as gems */
	0x0f,			/* gems:   fancy '*' */
#  else
	')',			/* weapon  */
	ARMOR_SYM,		/* armor */
	RING_SYM,		/* ring */
/* 5*/	',',			/* amulet  */
	TOOL_SYM,
	':',			/* food    */
	0xad,			/* potion: upside down '!' */
	SCROLL_SYM,		/* scroll  */
/*10*/	SPBOOK_SYM,
	0xe7,			/* wand:   greek tau */
	GEM_SYM,		/* gold:   yes it's the same as gems */
	GEM_SYM,		/* gems    */
#  endif
	ROCK_SYM,
/*15*/	BALL_SYM,
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

	(void) memcpy((genericptr_t)save_showsyms,
		      (genericptr_t)showsyms, sizeof showsyms);
	(void) memcpy((genericptr_t)save_oc_syms,
		      (genericptr_t)oc_syms, sizeof oc_syms);
	(void) memcpy((genericptr_t)save_monsyms,
		      (genericptr_t)monsyms, sizeof monsyms);

	/* Use a loop: char != uchar on some machines. */
	for (i = 0; i < MAXMCLASSES; i++)
	    monsyms[i] = def_monsyms[i];
# if defined(ASCIIGRAPH) && !defined(MSWIN_GRAPHICS)
	if (iflags.IBMgraphics
#  if defined(USE_TILES) && defined(MSDOS)
		&& !iflags.grmode
#  endif
		)
	    monsyms[S_HUMAN] = 0x01; /* smiley face */
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
	    showsyms[S_vodoor]  = 0xce;
	    showsyms[S_hodoor]  = 0xce;
	    showsyms[S_room]    = 0xfa; /* centered dot */
	    showsyms[S_corr]    = 0xb1;
	    showsyms[S_litcorr] = 0xb2;
	    showsyms[S_upstair] = 0xf0; /* Greek Xi */
	    showsyms[S_dnstair] = 0xf0;
#ifndef MSWIN_GRAPHICS
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

	    showsyms[S_trap_percents] = 0x04;
#endif
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
	(void) memcpy((genericptr_t)showsyms,
		      (genericptr_t)save_showsyms, sizeof showsyms);
	(void) memcpy((genericptr_t)oc_syms,
		      (genericptr_t)save_oc_syms, sizeof oc_syms);
	(void) memcpy((genericptr_t)monsyms,
		      (genericptr_t)save_monsyms, sizeof monsyms);
#if defined(MSDOS) && defined(USE_TILES)
	if (iflags.grmode) tileview(TRUE);
#endif
    }
}
#endif /* REINCARNATION */

/*drawing.c*/
