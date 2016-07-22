/*	SCCS Id: @(#)artilist.h 3.4	2003/02/12	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "config.h" /* WAC for blackmarket,  spoon */

#ifdef MAKEDEFS_C
/* in makedefs.c, all we care about is the list of names */

#define A(nam,typ,s1,s2,mt,atk,dfn,cry,inv,al,cl,rac,cost) nam

static const char *artifact_names[] = {
#else
/* in artifact.c, set up the actual artifact list structure */

#define A(nam,typ,s1,s2,mt,atk,dfn,cry,inv,al,cl,rac,cost) \
 { typ, nam, s1, s2, mt, atk, dfn, cry, inv, al, cl, rac, cost, 0 }

#define     NO_ATTK	{0,0,0,0}		/* no attack */
#define     NO_DFNS	{0,0,0,0}		/* no defense */
#define     NO_CARY	{0,0,0,0}		/* no carry effects */
#define     DFNS(c)	{0,c,0,0}
#define     CARY(c)	{0,c,0,0}
#define     PHYS(a,b)	{0,AD_PHYS,a,b}		/* physical */
#define     DRLI(a,b)	{0,AD_DRLI,a,b}		/* life drain */
#define     COLD(a,b)	{0,AD_COLD,a,b}
#define     FIRE(a,b)	{0,AD_FIRE,a,b}
#define     ELEC(a,b)	{0,AD_ELEC,a,b}		/* electrical shock */
#define     STUN(a,b)	{0,AD_STUN,a,b}		/* magical attack */
#define     ACID(a,b)	{0,AD_ACID,a,b}

STATIC_OVL NEARDATA struct artifact artilist[] = {
#endif	/* MAKEDEFS_C */

/* Artifact cost rationale:
 * 1.  The more useful the artifact, the better its cost.
 * 2.  Quest artifacts are highly valued.
 * 3.  Chaotic artifacts are inflated due to scarcity (and balance).
 */


/* [Tom] rearranged by alignment, so when people ask... */
/* KMH -- Further arranged:
 * 1.  By alignment (but quest artifacts last).
 * 2.  By weapon class (skill).
 */

/*  dummy element #0, so that all interesting indices are non-zero */
A("",				STRANGE_OBJECT,
	0, 0, 0, NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 0L ),

/*** Lawful artifacts ***/
A("Firewall",                  ATHAME,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(4,2),      FIRE(0,0),      NO_CARY,        0, A_LAWFUL, PM_FLAME_MAGE, NON_PM, 400L ),

/*
 *	The combination of SPFX_WARN and M2_something on an artifact
 *	will trigger EWarn_of_mon for all monsters that have the appropriate
 *	M2_something flags.  In Sting's case it will trigger EWarn_of_mon
 *	for M2_ORC monsters.
 */
A("Sting",			ELVEN_DAGGER,
	(SPFX_WARN|SPFX_DCLAS), 0, M2_ORC,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, PM_ELF, 800L ),

A("Giantkiller",                AXE,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_GIANT,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 800L ),

A("Quick Blade",                ELVEN_SHORT_SWORD,
	SPFX_RESTR, 0, 0,
	PHYS(9,1),      NO_DFNS,        NO_CARY,        0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Orcrist",                    ELVEN_BROADSWORD,
	SPFX_DFLAG2, 0, M2_ORC,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, PM_ELF, 2000L ),

A("Dragonbane",			BROADSWORD,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_DRAGON,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 500L ),

A("Excalibur",                  LONG_SWORD, /* removed NOGEN --Amy */
	(SPFX_RESTR|SPFX_SEEK|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH),0,0,
	PHYS(5,5),	DRLI(0,0),	NO_CARY,	0, A_LAWFUL, PM_KNIGHT, NON_PM, 4000L ),

#ifdef CONVICT
A("Luck Blade",			BROADSWORD,
	(SPFX_RESTR|SPFX_LUCK|SPFX_INTEL),0,0,
	PHYS(5,3),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, PM_CONVICT, NON_PM, 3000L ),
#endif /* CONVICT */

A("Sunsword",                   LONG_SWORD,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_UNDEAD,
	PHYS(5,0),	DFNS(AD_BLND),	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 1500L ),

/*
 *	Ah, never shall I forget the cry,
 *		or the shriek that shrieked he,
 *	As I gnashed my teeth, and from my sheath
 *		I drew my Snickersnee!
 *			--Koko, Lord high executioner of Titipu
 *			  (From Sir W.S. Gilbert's "The Mikado")
 */
A("Snickersnee",                KATANA,
	SPFX_RESTR, 0, 0,
	PHYS(0,4),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, PM_SAMURAI, NON_PM, 1200L ),

#ifdef YEOMAN
/* KMH -- Renamed from Holy Sword of Law (Stephen White)
 * This is an actual sword used in British coronations!
 */
A("Sword of Justice",           LONG_SWORD,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(5,6),     NO_DFNS,        NO_CARY,        0, A_LAWFUL, PM_YEOMAN, NON_PM, 1500L ),
#endif

A("Demonbane",			LONG_SWORD,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_DEMON,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 2500L ),

A("Werebane",			SILVER_SABER,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_WERE,
	PHYS(5,0),	DFNS(AD_WERE),	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Grayswandir",		SILVER_SABER,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 8000L ),

A("Skullcrusher",               CLUB,
	SPFX_RESTR, 0, 0,
	PHYS(3,5),     NO_DFNS,        NO_CARY,        0, A_LAWFUL, PM_CAVEMAN, NON_PM, 300L ),

A("Trollsbane",                 MORNING_STAR,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_TROLL,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 200L ),

A("Ogresmasher",		WAR_HAMMER,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_OGRE,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 200L ),

#ifdef YEOMAN
A("Reaper",                     HALBERD,
	SPFX_RESTR, 0, 0,
	PHYS(5,10),      NO_DFNS,        NO_CARY,        0, A_LAWFUL, PM_YEOMAN, NON_PM, 1000L ),
#endif

A("Holy Spear of Light",        SILVER_SPEAR,
       (SPFX_RESTR|SPFX_INTEL|SPFX_DFLAG2), 0, M2_UNDEAD,
       PHYS(5,5),      NO_DFNS,  NO_CARY,     LIGHT_AREA, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Rod of Lordly Might", MACE, /*needs quote*/
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(3,0),	NO_DFNS,	NO_CARY,
	0,	A_LAWFUL, PM_NOBLEMAN, NON_PM, 4000L ), 

/*** Neutral artifacts ***/
A("Magicbane",                  ATHAME,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	STUN(3,2),	DFNS(AD_MAGM),	NO_CARY,	0, A_NEUTRAL, PM_WIZARD, NON_PM, 3500L ),

A("Doom Chainsaw",	CHAINSWORD,
	(SPFX_RESTR|SPFX_INTEL|SPFX_ATTK), 0, 0,
	PHYS(20,0),	NO_DFNS,	NO_CARY,
	0,	A_NEUTRAL, PM_DOOM_MARINE, NON_PM, 2000L ),

A("Luckblade",                  SHORT_SWORD,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	PHYS(5,3),      NO_DFNS,        NO_CARY,        0, A_NEUTRAL, NON_PM, PM_GNOME, 1000L ),

A("Sword of Balance",           SILVER_SHORT_SWORD,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(2,5),      NO_DFNS,        NO_CARY,        0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Frost Brand",                LONG_SWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(5,0),	COLD(0,0),	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Fire Brand",                 LONG_SWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(5,0),	FIRE(0,0),	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),

/*
 *	Two problems:  1) doesn't let trolls regenerate heads,
 *	2) doesn't give unusual message for 2-headed monsters (but
 *	allowing those at all causes more problems than worth the effort).
 */
A("Vorpal Blade",		LONG_SWORD,
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	PHYS(5,1),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Disrupter",                  MACE,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_UNDEAD,
	PHYS(5,15),     NO_DFNS,        NO_CARY,        0, A_NEUTRAL, PM_PRIEST, NON_PM, 500L ),

/*
 *	Mjollnir will return to the hand of a Valkyrie when thrown
 *	if the wielder is a Valkyrie with strength of 25 or more.
 */
A("Mjollnir",                   HEAVY_HAMMER,           /* Mjo:llnir */
	(SPFX_RESTR|SPFX_ATTK),  0, 0,
	ELEC(5,12),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, PM_VALKYRIE, NON_PM, 4000L ),

/* STEPHEN WHITE'S NEW CODE */
A("Gauntlets of Defense",    GAUNTLETS_OF_DEXTERITY,
	SPFX_RESTR, SPFX_HPHDAM, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,    INVIS, A_NEUTRAL, PM_MONK, NON_PM, 5000L ),

A("Mirrorbright",               SHIELD_OF_REFLECTION,
	(SPFX_RESTR|SPFX_HALRES|SPFX_REFLECT), 0, 0,
	NO_ATTK,      NO_DFNS,        NO_CARY,        0, A_NEUTRAL, PM_HEALER, NON_PM, 5000L ),

A("Deluder",               CLOAK_OF_DISPLACEMENT,
	(SPFX_RESTR|SPFX_STLTH|SPFX_LUCK), 0, 0,
	NO_ATTK,      NO_DFNS,        NO_CARY,        0, A_NEUTRAL, PM_WIZARD, NON_PM, 5000L ),

#ifdef TOURIST
A("Whisperfeet",               SPEED_BOOTS,
	(SPFX_RESTR|SPFX_STLTH|SPFX_LUCK), 0, 0,
	NO_ATTK,      NO_DFNS,        NO_CARY,        0, A_NEUTRAL, PM_TOURIST, NON_PM, 5000L ),
#else
A("Whisperfeet",               SPEED_BOOTS,
	(SPFX_RESTR|SPFX_STLTH|SPFX_LUCK), 0, 0,
	NO_ATTK,      NO_DFNS,        NO_CARY,        0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),
#endif

/*** Chaotic artifacts ***/
A("Grimtooth",                  ORCISH_DAGGER,
	SPFX_RESTR, 0, 0,
	PHYS(2,3),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, NON_PM, PM_ORC, 300L ),

A("Deep Freeze",                ATHAME,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(5,3),      COLD(0,0),      NO_CARY,        0, A_CHAOTIC, PM_ICE_MAGE, NON_PM, 400L ),


A("Serpent's Tongue",            DAGGER,
	SPFX_RESTR, 0, 0,
        PHYS(2,0),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_NECROMANCER, NON_PM, 400L ),
	/* See artifact.c for special poison damage */

A("The Marauder's Map", SCR_MAGIC_MAPPING,
	(SPFX_RESTR), 0, 0, NO_ATTK,	NO_DFNS,	NO_CARY,
	OBJECT_DET,	A_CHAOTIC, PM_PIRATE, NON_PM, 2000L ),


A("Cleaver",                    BATTLE_AXE,
	SPFX_RESTR, 0, 0,
	PHYS(3,3),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, PM_BARBARIAN, NON_PM, 1500L ),

A("Doomblade",                  ORCISH_SHORT_SWORD,
	SPFX_RESTR, 0, 0,
	PHYS(0,5),     NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_HUMAN_WEREWOLF, NON_PM, 1000L ),

A("Sea Gull",				DAGGER,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(2,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Jungle Guard",				SURVIVAL_KNIFE,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_REGEN), 0, 0,
	ACID(1,5), DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Dark Moon Rising",			OBSIDIAN_AXE,
	(SPFX_RESTR|SPFX_WARN|SPFX_ATTK), 0, 0,
	PHYS(4,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Digging Dog",			DWARVISH_MATTOCK,
	(SPFX_RESTR|SPFX_SEEK), 0, 0,
	PHYS(2,6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("World's Largest Cock",		VIBROBLADE,
	(SPFX_RESTR|SPFX_ESP|SPFX_ATTK|SPFX_DCLAS), 0, S_HUMAN,
	PHYS(20, 10), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2500L ),

A("Stormbringer's Little Brother",			BLACK_AESTIVALIS,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	DRLI(1,1), DRLI(0,0), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Thorn Rose",			WHITE_FLOWER_SWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(5,1), FIRE(0,0), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Bluewrath",			CRYSTAL_SWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(5,1), COLD(0,0), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Kamehamehadoken",			SUGUHANOKEN,
	(SPFX_RESTR), 0, 0,
	PHYS(8,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Electrifier",			GREAT_HOUCHOU,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ELEC(5,1), ELEC(0,0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

/* intentional misspelling --Amy */
A("Double Bestard",			BASTERD_SWORD,
	(SPFX_RESTR|SPFX_SPEAK), 0, 0,
	PHYS(0,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Guardian of Aranoch",			BENT_SABLE,
	(SPFX_RESTR), 0, 0,
	PHYS(0,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Dullswandir",			IRON_SABER,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Goldswandir",		GOLDEN_SABER,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5,5),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Sounding Iron",			AKLYS,
	(SPFX_RESTR|SPFX_SPEAK), 0, 0,
	PHYS(2,3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Firmnail",			SPIKED_CLUB,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK|SPFX_LUCK), 0, 0,
	FIRE(1,4), FIRE(0,0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1600L ),

A("Superclean Deseamer",			BROOM,
	(SPFX_RESTR|SPFX_STLTH|SPFX_PROTEC), 0, 0,
	PHYS(10,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Soothing Fan",			CARDBOARD_FAN,
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("Generic Japanese Melee Weapon",			OTAMA,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Thwack-Whacker",                  FLANGED_MACE,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_UNDEAD,
	PHYS(10,24),     NO_DFNS,        NO_CARY,        0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Evening Star",			MORNING_STAR,
	(SPFX_RESTR|SPFX_DCLAS|SPFX_ESP), 0, S_LIGHT,
	PHYS(10,30), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Flogging Rhythm",			JAGGED_STAR,
	(SPFX_RESTR), 0, 0,
	PHYS(4,8), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Modified Pocket Calculator",			FLAIL,
	(SPFX_RESTR|SPFX_SPEAK), 0, 0,
	PHYS(24,1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 240L ),

A("Bitchwhipper",			KNOUT,
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_WARN), 0, M2_FEMALE,
	PHYS(5,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Orc Magic",			ORCISH_RING_MAIL,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Anti-Disenchanter",			DISENCHANTING_BOOTS,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Hot and Cold",			RIN_FIRE_RESISTANCE,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Glorious Dead",			CORPSE,
	(SPFX_RESTR|SPFX_REFLECT), (SPFX_DEFN|SPFX_REFLECT), 0,
	NO_ATTK, NO_DFNS, CARY(AD_MAGM), 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Precious Wish",			SCR_WISHING,
	(SPFX_RESTR), (SPFX_DEFN), 0,
	NO_ATTK, NO_DFNS, CARY(AD_MAGM), 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Trolled by the RNG",			ROCK,
	(SPFX_RESTR), 0, 0,
	PHYS(5,5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Starcraft Flail",			OBSID,
	(SPFX_RESTR|SPFX_HSPDAM), 0, 0,
	PHYS(1,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6500L ),

A("Pwnhammer",			MALLET,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_HPHDAM), 0, 0,
	COLD(5,8), DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Pwnhammer Duece",			SLEDGE_HAMMER,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_REGEN), 0, 0,
	FIRE(8,12), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Dockside Walk",			WEDGED_LITTLE_GIRL_SANDAL,
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	PHYS(2,5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Karate Kid",			SOFT_GIRL_SNEAKER, /* free action */
	(SPFX_RESTR), 0, 0,
	PHYS(5,8), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 10000L ),

A("Girlful Bonking",			STURDY_PLATEAU_BOOT_FOR_GIRLS,
	(SPFX_RESTR), 0, 0,
	PHYS(20,15), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Armor-Piercing Hug",			HUGGING_BOOT,
	(SPFX_RESTR|SPFX_DEFN|SPFX_PROTEC), 0, 0,
	PHYS(50,1), DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Asian Winter",			HUGGING_BOOT, /* aggravate monster */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(4,9), DFNS(AD_FIRE), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("French Maria",			BLOCK_HEELED_COMBAT_BOOT,
	(SPFX_RESTR|SPFX_DEFN|SPFX_WARN), 0, 0,
	PHYS(2,6), DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Force India",			WOODEN_GETA,
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	PHYS(20,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Stupidity In Motion",			LACQUERED_DANCING_SHOE,
	(SPFX_RESTR|SPFX_DRLI|SPFX_REFLECT), 0, 0,
	DRLI(1,1), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 6000L ),

A("Sexy Nurse Sandal",			HIGH_HEELED_SANDAL,
	(SPFX_RESTR|SPFX_REGEN|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2500L ),

A("Tender Beauty",			SEXY_LEATHER_PUMP,
	(SPFX_RESTR), 0, 0,
	PHYS(5,6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Massive But Lovely",			SEXY_LEATHER_PUMP,
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	PHYS(6,9), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Sweetheart Pump",			SEXY_LEATHER_PUMP, /* psi resistance */
	(SPFX_RESTR), 0, 0,
	PHYS(15,1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Sandra's Evil Minddrill",			SPIKED_BATTLE_BOOT, /* aggravate monster, can cause amnesia when wielded */
	(SPFX_RESTR|SPFX_DEFN|SPFX_SEARCH), 0, 0,
	PHYS(0,16), DFNS(AD_ELEC), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Ribcracker",			QUARTERSTAFF,
	(SPFX_RESTR), 0, 0,
	PHYS(8,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Dull Metal",			IRON_BAR,
	(SPFX_RESTR), 0, 0,
	PHYS(1,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Gnarlwhack",			PLATINUM_FIRE_HOOK,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK|SPFX_HALRES|SPFX_SEARCH), 0, 0,
	FIRE(5,4), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Fire Leader",			PARTISAN,
	(SPFX_RESTR|SPFX_ATTK|SPFX_LUCK), 0, 0,
	FIRE(9,12), NO_DFNS, NO_CARY, UNTRAP, A_NEUTRAL, NON_PM, NON_PM, 11200L ),

A("Fumata Yari",			SPETUM, /* makes the player very fast when wielded */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ACID(2,8), DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3400L ),

A("Non-Sucker",			RANSEUR,
	(SPFX_RESTR), 0, 0,
	PHYS(6,6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Dimoak's Hew",			BARDICHE, /* blindness resistance */
	(SPFX_RESTR), 0, 0,
	PHYS(0,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Land Knight Piercer",			VOULGE,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	PHYS(2,5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Apply B",			HALBERD,
	(SPFX_RESTR|SPFX_SPEAK|SPFX_ESP|SPFX_STLTH), 0, 0,
	PHYS(1,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7500L ),

A("Cock Application",			BILL_GUISARME,
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	DRLI(3,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6500L ),

A("Not A Hammer",			LUCERN_HAMMER,
	(SPFX_RESTR|SPFX_ATTK|SPFX_REFLECT), 0, 0,
	COLD(4,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("It's A Polearm",			LUCERN_HAMMER,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ELEC(4,8), DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Bec De Ascorbin",			BEC_DE_CORBIN,
	(SPFX_RESTR), 0, 0,
	PHYS(1,2), NO_DFNS, NO_CARY, HEALING, A_LAWFUL, NON_PM, NON_PM, 3000L ),

A("Paleolithic Relic",			FLINT_SPEAR,
	(SPFX_RESTR), 0, 0,
	PHYS(2,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Bronze Age Relic",			BRONZE_SPEAR,
	(SPFX_RESTR), 0, 0,
	PHYS(2,6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Misguided Missile",			SPIRIT_THROWER,
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	PHYS(0,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Mare's Special Rocket",			TORPEDO,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_REFLECT), 0, 0,
	COLD(8,8), DFNS(AD_COLD), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 9000L ),

A("Lightning Blade",			TRIDENT,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(2,6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Fishing Grandpa",			STYGIAN_PIKE,
	(SPFX_RESTR|SPFX_DCLAS|SPFX_WARN), 0, S_EEL,
	PHYS(20,20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Static Stick",			MARE_TRIDENT,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ELEC(4,7), DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("The People-Eating Trident",			MANCATCHER, /* sight bonus */
	(SPFX_RESTR|SPFX_DCLAS|SPFX_WARN), 0, S_HUMAN,
	PHYS(8,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 9000L ),

A("Madeline's Guardian",			PARASOL, /* Pokemon Emerald */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Penguin's Thrusting Sword",		UMBRELLA, /* It sure looks like a sword to me! --Amy also, conveys flying */
	(SPFX_RESTR), 0, 0,
	PHYS(12,9), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 500L ),

A("Lackware",			ORCISH_BOW,
	(SPFX_RESTR), 0, 0,
	PHYS(1,1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Wild Hunt",			HYDRA_BOW,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_SEARCH|SPFX_PROTEC), 0, 0,
	FIRE(2,8), DFNS(AD_FIRE), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 20000L ),

A("Buffy Ammo",			SILVER_ARROW,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_DEMON,
	PHYS(10,20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Heavy Hitter Arrow",			YA,
	(SPFX_RESTR), 0, 0,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 100L ),

A("Agora",			CATAPULT,
	(SPFX_RESTR), 0, 0,
	PHYS(0,7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Upgraded Lemure",			CATAPULT,
	(SPFX_RESTR|SPFX_SEEK), 0, 0,
	PHYS(5,9), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Walther PPK",			PISTOL,
	(SPFX_RESTR), 0, 0,
	PHYS(2,5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Desert Eagle",			PISTOL,
	(SPFX_RESTR), 0, 0,
	PHYS(5,9), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Ingram Mac-10",			SUBMACHINE_GUN, /* makes the player very fast when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("FN M249 Para",			HEAVY_MACHINE_GUN, /* aggravate monster */
	(SPFX_RESTR), 0, 0,
	PHYS(0,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Surefire Gun",			RIFLE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Mosin-Nagant",			SNIPER_RIFLE,
	(SPFX_RESTR|SPFX_SEEK), 0, 0,
	PHYS(20,15), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Leone M3 Guage Super",			SHOTGUN,
	(SPFX_RESTR), 0, 0,
	PHYS(0,20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1600L ),

A("Citykiller Combat Shotgun",			AUTO_SHOTGUN,
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	PHYS(0,5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3600L ),

A("Smugglers End",			HAND_BLASTER,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(10,1), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Colonel Bastard's Laser Pistol",			ARM_BLASTER,
	(SPFX_RESTR), 0, 0,
	PHYS(10,5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Cookie Cutter",			CUTTING_LASER,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Doomguy's Wet Dream",			BFG,
	(SPFX_RESTR), 0, 0,
	PHYS(2,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 9000L ),

A("Grand Daddy",			ROCKET_LAUNCHER,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(20,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Extra Firepower",			BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(2,5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Demon Machine",			DEMON_CROSSBOW, /* infravision when wielded */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(6,6), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("ICBM",			HELO_CROSSBOW,
	(SPFX_RESTR), 0, 0,
	PHYS(20,1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 9900L ),

A("Bearkiller",			CROSSBOW_BOLT,
	(SPFX_RESTR|SPFX_DFLAG1), 0, M1_THICK_HIDE,
	PHYS(5,25), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Puncture Missile",			DART,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_HUMAN,
	PHYS(8,20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Instant Death",			DART_OF_DISINTEGRATION,
	(SPFX_RESTR), 0, 0,
	PHYS(0,50), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Needle-like the new log",			SHURIKEN,
	(SPFX_RESTR), 0, 0,
	PHYS(7,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 70L ),

A("Catwomanbane",			BATARANG,
	(SPFX_RESTR|SPFX_DCLAS|SPFX_SPEAK), 0, S_FELINE,
	PHYS(10,40), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Doctor Jones' Aid",			BULLWHIP,
	(SPFX_RESTR|SPFX_WARN|SPFX_DCLAS), 0, S_SNAKE,
	PHYS(5,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Cruel Punisher",			STEEL_WHIP, /* grants aggravate monster */
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	PHYS(2,6), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Bristly String",			ROSE_WHIP, /* monsters take 1d4 damage if they melee you */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(3,6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Police Brutality",			RUBBER_HOSE,
	(SPFX_RESTR), 0, 0,
	PHYS(4,7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Demonstrants Go Home",			RUBBER_HOSE,
	(SPFX_RESTR|SPFX_DCLAS|SPFX_DEFN|SPFX_WARN), 0, S_HUMAN,
	PHYS(6,8), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("We Are Not Oppressive",			RUBBER_HOSE,
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	PHYS(8,8), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Death To Shoplifters",			RUBBER_HOSE,
	(SPFX_RESTR|SPFX_BEHEAD|SPFX_DCLAS), 0, S_HUMAN,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Legendary Shirt",			KYRT_SHIRT,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Sexy Convicts",			STRIPED_SHIRT,
	(SPFX_RESTR|SPFX_ESP|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Nobility Worldwide",			RUFFLED_SHIRT,
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Peace Advocate",			RUFFLED_SHIRT, /* free action while worn */
	(SPFX_RESTR|SPFX_WARN|SPFX_DCLAS), 0, S_HUMAN,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Gentle-Soft Clothing",			VICTORIAN_UNDERWEAR, /* poison and disintegration resistance while worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Helen's Discarded Shirt",			VICTORIAN_UNDERWEAR, /* aggravate monster while worn */
	(SPFX_RESTR|SPFX_TCTRL|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Antimagic Shell",			VICTORIAN_UNDERWEAR, /* prevents spellcasting */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Memorial Garments",			VICTORIAN_UNDERWEAR, /* versus curses and keen memory */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 6000L ),

A("Total Control",			VICTORIAN_UNDERWEAR, /* resist confusion and stun */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Victoria Is Evil But Pretty",			VICTORIAN_UNDERWEAR, /* polymorph control and manaleech */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Medical Power Armor Prototype",			FULL_PLATE_MAIL,
	(SPFX_RESTR|SPFX_SPEAK|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, HEALING, A_NONE, NON_PM, NON_PM, 2500L ),

A("As Heavy As It Is Ugly",			BRONZE_PLATE_MAIL,
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Volume Armament",			SPLINT_MAIL, /* superscrolling screen effect */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Fucking Orichalcum",			OLIHARCON_SPLINT_MAIL,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Stealth Suit",			ELVEN_MITHRIL_COAT,
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Little Big Midget",			GNOMISH_SUIT, /* grants free action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Cathan's Network",			CHAIN_MAIL, /* increases STR by its enchantment value +3 */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3400L ),

A("Fly Like An Eagle",			FEATHER_ARMOR, /* flying */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Pretty Little Magical Girl",			SCHOOL_UNIFORM, /* manaleech if worn by a female char */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Playboy With Ears",			BUNNY_UNIFORM, /* increases CHA by its enchantment value +5 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Antisepsis Coat",			NURSE_UNIFORM, /* sickness resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1600L ),

A("Falcet",			SCALE_MAIL,
	(SPFX_RESTR|SPFX_DEFN|SPFX_REGEN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4400L ),

A("Superescape Mail",			RING_MAIL, /* autocurses when worn, disables fire resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Grayscale Wanderer",			LEATHER_ARMOR, /* shades of grey effect */
	(SPFX_RESTR|SPFX_DEFN|SPFX_WARN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("CD Rome Arena",			ELVEN_TOGA, /* slows the player down */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Chastity Armor",			CONSORT_S_SUIT, /* prevents you from contracting STD */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Laura Croft's Battlewear",			LEATHER_JACKET, /* like d-type equipment, prevents lava from harming player */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Offense Owns Defense",			ROBE, /* double attacks */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Protection With A Price",			ROBE_OF_WEAKNESS, /* stun resistance and 5 extra points of AC */
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Cute Idea",			ORANGE_DRAGON_SCALE_MAIL,
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("All Hail The Mighty RNG",			PURPLE_DRAGON_SCALE_MAIL,
	(SPFX_RESTR|SPFX_LUCK|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 10000L ),

A("Ho-Oh's Feathers",			SKY_DRAGON_SCALE_MAIL, /* aggravate monster and conflict */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 20000L ),

A("Upgrade This",			MERCURIAL_DRAGON_SCALES,
	(SPFX_RESTR|SPFX_SEEK|SPFX_SPEAK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Don Suicune Does Not Approve",			PLATINUM_DRAGON_SCALES, /* aggravate monster and random nasty effects */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Pretty Puff",			SAPPHIRE_DRAGON_SCALES,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Invisible Visibilitity",			MUMMY_WRAPPING, /* the Amy is a troll and makes this thing grant invis :P */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Visible Invisibilitity",			MUMMY_WRAPPING, /* see invis */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Blackcloak",			ORCISH_CLOAK,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Eveline's Lab Coat",			LAB_COAT,
	(SPFX_RESTR|SPFX_STLTH|SPFX_LUCK), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Ina's Lab Coat",			LAB_COAT, /* hunger, random fainting, and disintegration/sickness resistance; autocurses */
	(SPFX_RESTR|SPFX_SPEAK|SPFX_SEEK), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Superman's Super Suit",			SUPER_MANTLE, /* quad attacks */
	(SPFX_RESTR|SPFX_REGEN|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Full Wings",			WINGS_OF_ANGEL, /* flying */
	(SPFX_RESTR|SPFX_HALRES|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Broken Wings",			DUMMY_WINGS, /* disables the flying intrinsic, aggravates monsters, and causes intrinsic freezing; autocurses */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5L ),

A("Acquired Poison Resistance",			POISONOUS_CLOAK, /* poison resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("It's A Wonderful Failure",			CLOAK_OF_DEATH,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Rita's Deceptive Mantle",			CLOAK_OF_FUMBLING, /* autocurses and sets its enchantment to -10 when worn! */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Stunted Helper",			MANACLOAK,
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Insufficient Protection",			CLOAK_OF_MATADOR,
	(SPFX_RESTR|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Memory Aid",			CLOAK_OF_UNSPELLING, /* keen memory */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Frequent But Weak Status",			HEAVY_STATUS_CLOAK, /* resist confusion and stun */
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("A Reason To Live",			YELLOW_SPELL_CLOAK, /* deactivates teleport control */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Full Moon Tonight",			VULNERABILITY_CLOAK,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("All-In-One Ascension Kit",			CLOAK_OF_INVENTORYLESSNESS, /* also magic and drain resistance */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Radar Cloak",			CLOAK_OF_RESPAWNING,
	(SPFX_RESTR|SPFX_ESP|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Hostes Ad Pulverem Ferire",			SPAWN_CLOAK, /* increase STR/DEX by enchantment value +5 */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Uberjackal Effect",			ADOM_CLOAK, /* aggravate monster */
	(SPFX_RESTR|SPFX_SPEAK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Variatio Delectat",			EGOIST_CLOAK, /* does nothing special - just meant to annoy lawful players who hoped for a good sacrifice gift */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Speedrunner's Dream",			CLOAK_OF_TIME, /* makes the player very fast */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Can't Kill What You Can't See",			EERIE_CLOAK, /* see invis and makes you invisible */
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Image Projector",			CLOAK_OF_NAKEDNESS, /* displacement */
	(SPFX_RESTR|SPFX_TCTRL|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Silent Noise",			CLOAK_OF_AGGRAVATION,
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Dark Angels",			CLOAK_OF_CONFLICT, /* flying */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2500L ),

A("Skills Beat Stats",			CLOAK_OF_STAT_LOCK, /* faster skill training */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Baron Von Richthofen's Premiere",			WING_CLOAK,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 16000L ),

A("RNG's Gamble",			CLOAK_OF_PREMATURE_DEATH,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Fightright",			GNOMISH_HELM,
	(SPFX_RESTR|SPFX_SEEK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 200L ),

A("Vitamin B",			HELM_OF_LOSE_IDENTIFICATION,
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 600L ),

A("Spectral Resistance",			COLOR_CONE, /* resist fire, cold, elec, poison */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 7500L ),

A("Giant WOK",			DENTED_POT,
	(SPFX_RESTR|SPFX_PROTEC|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Playboy Supplement",			BUNNY_EAR, /* increases CHA by its enchantment value +5 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Real Speed Devil",			SPEEDWAGON_S_HAT, /* makes the player very fast and adds extra speed sometimes */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Royal Tiara",			CROWN,
	(SPFX_RESTR|SPFX_ESP|SPFX_TCTRL|SPFX_SEEK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Functional Radio",			ANTENNA, /* works like radio helmet */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Warner Brother",			PLASTEEL_HELM,
	(SPFX_RESTR|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 250L ),

A("Dark Nadir",			HELMET, /* autocurses, creates darkness once every 100 turns, but improves your to-hit by 5 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Loving Deity",			HELM_OF_OPPOSITE_ALIGNMENT,
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("COW Enchantment",			HELM_OF_DETECT_MONSTERS, /* 9 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 900L ),

A("Thor's Mythical Helmet",			HELM_OF_STORMS,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50000L ),

A("Clang Compilation",			HELM_OF_STEEL,
	(SPFX_RESTR|SPFX_PROTEC|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Surface-To-Air Site",			FLACK_HELMET, /* +1 multishot */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Massive Cranium",			CRYSTAL_HELM,
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Burger Eater",			HELM_OF_HUNGER,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Omniscient",			HELMET_OF_UNDEAD_WARNING,
	(SPFX_RESTR|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("Spacewarp",			HELM_OF_TELEPORTATION,
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Dictatorship",			HELM_OF_TELEPORT_CONTROL, /* also gives polymorph control */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Safe Insecurity",			HELM_OF_OBSCURED_DISPLAY,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("You Are Already Dead",			BLACKY_HELMET, /* also magic and drain resistance */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 20000L ),

A("Shpx Guvf Fuvg",			CYPHER_HELM, /* also unbreathing */
	(SPFX_RESTR|SPFX_SEEK|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Go Other Place",			HELM_OF_BAD_ALIGNMENT, /* teleportitis */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Beeswax Helmet",			SOUNDPROOF_HELMET,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Low Local Memory",			OUT_OF_MEMORY_HELMET, /* free action and drain resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Severe Aggravation",			ANGER_HELM, /* aggravate monster */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Formfiller",			CAPTCHA_HELM, /* heavily curses itself when worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Stone Erosion",			HELM_OF_AMNESIA, /* petrification resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Blinding Fog",			DIMMER_HELMET, /* blindness resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Big Bonnet",			WAR_HAT, /* poison resistance */
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Eulogy's Eulogy",			BOOGEYMAN_HELMET,
	(SPFX_RESTR|SPFX_PROTEC|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Mind Shielding",			TINFOIL_HELMET, /* stun resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1600L ),

A("Conspiracy Theory",			PARANOIA_HELMET,
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2300L ),

A("Box Fist",			LEATHER_GLOVES, /* +5 unarmed damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Swing Less Cast More",			GAUNTLETS_OF_PANIC,
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Molass Tank",			GAUNTLETS_OF_SLOWING, /* 10 extra points of AC */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Science Fliction",			OILSKIN_GLOVES,
	(SPFX_RESTR|SPFX_REGEN|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("AFK Means Ass-Fucker",			GAUNTLETS_OF_TYPING, /* autocurses, monsters with AD_SEDU or similar attacks have +12 speed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Signons Steel Total",			GAUNTLETS_OF_STEEL, /* resist poison, cold, stone */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Double Luck",			GAUNTLETS_OF_GOOD_FORTUNE,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Plug And Pray",			GAUNTLETS_OF_PLUGSUIT,
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Gauntlets of Slaying",			GAUNTLETS, /* increase STR and DEX by enchantment +3 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Iris's Precious Metal",			SILVER_GAUNTLETS, /* demons are spawned peaceful 90% of the time while you wear these */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 9000L ),

A("Flictionless Combat",			GAUNTLETS_OF_NO_FLICTION, /* putting them on for the first time gives you the blessing technique but also permanent itemcursing! */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Come Back To Life",			MENU_NOSE_GLOVES, /* 50% chance of life saving while wearing them */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("ScrollScrollScroll",			CONFUSING_GLOVES, /* increased drop rate of standard id and similar scrolls */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Sealed Knowledge",			GAUNTLETS_OF_MISSING_INFORMATI,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_REFLECT|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Fifty Shades Of Fucked Up",			SADO_MASO_GLOVES, /* +5 damage unarmed, +10 damage while wielding a whip */
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Arabella's Bank of Crossroads",			BANKING_GLOVES, /* levelteleportitis, heavily curse themselves when worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Out Of Control",			CHAOS_GLOVES, /* give teleportitis and disable tele control */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Shadowdisk",			SMALL_SHIELD, /* blindness resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Lurtz's Wall",			URUK_HAI_SHIELD, /* 20% boost to block rate, free action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Aeaeaeaeaegis",			STEEL_SHIELD, /* 10 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Shattered Dreams",			CRYSTAL_SHIELD, /* autocurses, aggravates monsters and causes conflict */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 6000L ),

A("Burning Disk",			FLAME_SHIELD, /* causes burn, autocurses */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("The Type Of Ice Block Hates You",			ICE_SHIELD, /* causes freezing, autocurses */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Numbed Can't Do",			LIGHTNING_SHIELD, /* causes numbness, autocurses */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Venomat",			VENOM_SHIELD, /* causes stun, autocurses */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("They Must All Die",			ORCISH_GUARD_SHIELD,
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Whang Clink Clonk",			SHIELD, /* 5 extra points of AC and +10% chance to block */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Reflector Ejector",			SILVER_SHIELD, /* teleportitis */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Little Thorn Rose",			MIRROR_SHIELD, /* d5 passive damage to attackers, negates sleep resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Teh Bash0r",			RAPIRAPI, /* adds a flat +2 damage to your weapon attacks */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Too Good To Be True",			SILVER_DRAGON_SCALE_SHIELD,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 8000L ),

A("Solar Power",			BLACK_DRAGON_SCALE_SHIELD, /* sight bonus */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 8000L ),

A("Brass Guard",			COPPER_DRAGON_SCALE_SHIELD, /* free action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 8000L ),

A("Systematic Chaos",			PURPLE_DRAGON_SCALE_SHIELD, /* autocurses, grants conflict and sustain ability */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 12000L ),

A("Golden Dawn",			EMERALD_DRAGON_SCALE_SHIELD, /* autocurses, causes the "verisiert" effect and gives keen memory */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 12000L ),

A("Real Psychos Wear Purple",			PSYCHIC_DRAGON_SCALE_SHIELD, /* autocurses, grants psi resistance and the hate trap and farlook bug effects */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 12000L ),

A("Binder Crash",			PLASTEEL_BOOTS, /* 1 out of 20 games, putting them on transforms you into a Binder permanently. Otherwise, they heavily curse themselves and increase your deity's anger by three. No ill effect if you actually are a Binder, but for them they count as high heels. */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Mephisto's Brogues",			LOW_BOOTS, /* autocurses, resist cold/poison, flying, disables fire resistance */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Gnomish Boobs",			GNOMISH_BOOTS, /* +3 charisma */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 100L ),

A("Kokyo no pafomansuu-oku",			ATSUZOKO_BOOTS, /* increases charisma by +5 + enchantment */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Little Girl's Revenge",			RUBBER_BOOTS,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("AmyBSOD's Vampiric Sneakers",			SNEAKERS, /* cause blood loss */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Cinderella's Slippers",			CRYSTAL_BOOTS, /* aggravate monster and wounded legs, because your feet are bigger than the owner's :-P */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0, /* actually prism reflection */
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Eveline's Lovelies",			WEDGE_SANDALS, /* +5 kicking damage, +3 charisma */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Natalia's Punisher",			WEDGE_SANDALS, /* +8 damage with hammer-class weapons, petrification resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Ella's Bloodlust",			WEDGE_SANDALS, /* double attacks, autocurses, aggravate monster */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Anastasia's Gentleness",			DANCING_SHOES, /* free action, reduces your STR by 10 */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Katrin's Paralysis",			DANCING_SHOES, /* kicking a monster causes it to be stuck to you, resist elec and cold */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Eva's Inconspicuous Charm",			SWEET_MOCASSINS, /* increase CHA and DEX by enchantment value */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Solvejg's Stinking Slippers",			SWEET_MOCASSINS, /* heavily curses itself, aggravates monsters, resist elec, manaleech, +5 CHA, -5 INT and WIS */
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 7500L ),

A("Jessica's Tenderness",			SWEET_MOCASSINS, /* resist cold, sleep and psi */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 8000L ),

A("Leather Pumps of Horror",			FEMININE_PUMPS, /* poison resistance, increase CHA by 5 + enchantment value */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Lilac Beauty",			FEMININE_PUMPS, /* +10 CHA, -5 to all other stats, taking them off drains an experience level (ignoring resistance) */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN|SPFX_STLTH), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Rhea's Combat Pumps",			FEMININE_PUMPS, /* resist poison/sickness, d10 passive damage to non-poison-resistant monsters with 2% chance of instakill */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 8000L ),

A("Mandy's Rough Beauty",			LEATHER_PEEP_TOES, /* +10 kick damage, free action, increase CHA by 5 + enchantment */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 11000L ),

A("I'm A Bitch, Deal With It",			HIPPIE_HEELS, /* aggravate monster, +5 AC, becomes +0 if the enchantment is negative */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Manuela's Torture Heels",			HIPPIE_HEELS, /* aggravate monster, conflict, heavily curse themselves when worn */
	(SPFX_RESTR|SPFX_DEFN|SPFX_ESP), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Beautiful Topmodel",			HIPPIE_HEELS, /* +25 CHA */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 16000L ),

A("Corina's Unfair Scratcher",			COMBAT_STILETTOS, /* flying, resist cold, shock and petrification */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Sporked",			BOOTS_OF_MOLASSES,
	(SPFX_RESTR|SPFX_REGEN|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Hermes' Unfairness",			FLYING_BOOTS, /* aggravate monster, unbreathing, monsters are always spawned hostile */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Yet Another Stupid Idea",			ZIPPER_BOOTS, /* resist fire and petrification */
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Hot Flame",			FIRE_BOOTS,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Jesus Footwear",			BOOTS_OF_SWIMMING, /* prevents your inventory from getting wet */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7500L ),

A("Cursing Anomaly",			ANTI_CURSE_BOOTS, /* autocurses :-P, causes random itemcursing */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_SEEK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Fun All In One",			MULTI_SHOES, /* conflict, unbreathing, aggravate monster */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Lollerskates",			ROLLER_BLADE,
	(SPFX_RESTR|SPFX_DEFN|SPFX_TCTRL), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2400L ),

A("Desertwalk",			FREEZING_BOOTS,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Waiting Timeout",			AUTODESTRUCT_DE_VICE_BOOTS, /* free action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Nose Enchantment",			SPEEDBUG_BOOTS, /* polymorphitis */
	(SPFX_RESTR|SPFX_DEFN|SPFX_HPHDAM), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Fantastic Shoes",			SENTIENT_HIGH_HEELED_SHOES,
	(SPFX_RESTR|SPFX_SPEAK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Untrained Half Marathon",			BOOTS_OF_FAINTING, /* wounded legs, very fast speed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Black Diamond Icon",			DIFFICULT_BOOTS, /* multiply monster spawn rate by 4 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Riddle Me This",			GRIDBUG_CONDUCT_BOOTS, /* causes quizzes */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Base for Speed Ascension",			LIMITATION_BOOTS, /* sickness resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Paranoia Stride",			DEMENTIA_BOOTS, /* resist fear */
	(SPFX_RESTR|SPFX_HALRES|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Ding Dong Ping Pong",			RIN_DISARMING,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2500L ),

A("Ring of Woe",			RIN_DOOM, /* prime curses itself, aggravate monster, hunger and conflict */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Werefoo Go Home",			RIN_PROTECTION_FROM_SHAPE_CHAN,
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Secret Detective",			RIN_STEALTH,
	(SPFX_RESTR|SPFX_ESP|SPFX_SEEK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7500L ),

A("Magic Signet",			RIN_SUSTAIN_ABILITY,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Eat More Kittens",			RIN_AGGRAVATE_MONSTER, /* autocurses */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("The Ring of the Schwartz",			RIN_TIMELY_BACKUP, /* disintegration resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Afterthought",			RIN_POISON_RESISTANCE,
	(SPFX_RESTR|SPFX_SEEK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Polar Opposites",			RIN_SHOCK_RESISTANCE,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Win Or Lose",			RIN_INTRINSIC_LOSS,
	(SPFX_RESTR|SPFX_ESP|SPFX_LUCK|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Crylock",			RIN_BLOOD_LOSS, /* heavily curses itself, improves AC by 10 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("GoldenIvy's Rage",			RIN_SUPERSCROLLING, /* aggravate monster, teleportitis, sickness resistance, flying */
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Team Nastytrap's Bauble",			RIN_ENSNARING,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Float Eyeler's Condition",			RIN_DIARRHEA, /* levitation */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Skill-less of the service",			RIN_NO_SKILL,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Fatally Low",			RIN_LOW_STATS,
	(SPFX_RESTR|SPFX_ESP|SPFX_SEEK|SPFX_LUCK|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Criminal Queen",			RIN_DANGER, /* +5 charisma, very fast speed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6000L ),

A("Life Sucks",			AMULET_OF_CHANGE, /* another trolling artifact, because it disintegrates when worn :-P */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Bestestor",			AMULET_OF_UNDEAD_WARNING,
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Traitorious Devil",			AMULET_OF_OWN_RACE_WARNING,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Warned and Protected",			AMULET_OF_POISON_WARNING, /* poison resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Controlled Teleportitis",			AMULET_OF_COVETOUS_WARNING,
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Guardian Angle",			AMULET_OF_LIFE_SAVING, /* prism reflection */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Snorefest",			AMULET_OF_RESTFUL_SLEEP, /* 90% sleep resistance :-P, also aggravates monsters */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Precious Unobtainable Properties",			AMULET_OF_STRANGULATION, /* warp reflection, resist psi/stun/petrification/sickness */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Ballsy Bastard",			AMULET_OF_PREMATURE_DEATH, /* free action, resist petrification, manaleech */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Fix Everything",			AMULET_OF_UNCHANGING, /* sustain ability */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Only One Escape",			AMULET_OF_ANTI_TELEPORTATION, /* jumping */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Oh Come On",			AMULET_OF_ACID_RESISTANCE, /* +5 charisma */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Amulet of Carlammas",			AMULET_OF_REGENERATION,
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4500L ),

A("Computer Amulet",			AMULET_OF_DATA_STORAGE, /* keen memory */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Stinging Medallion",			AMULET_OF_DANGER, /* disintegration resistance, flying */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Recovered Relic",			AMULET_OF_MENTAL_STABILITY, /* stun res */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Tyranitar's Quest",			AMULET_OF_PRISM, /* techniques get no timeout 50% of the time */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Space Cycle",			AMULET_OF_WARP_DIMENSION, /* teleportitis and polymorphitis */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Necklace of Adornment",			NECKLACE, /* +10 charisma */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Good Bee",			AMULET_OF_RMB_LOSS, /* sight bonus, poison resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Where Did It Go",			AMULET_OF_ITEM_TELEPORTATION,
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Confusticator",			AMULET_OF_DIRECTIONAL_SWAP, /* confusion */
	(SPFX_RESTR|SPFX_SEEK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Eeh-Eeh",			AMULET_OF_SUDDEN_CURSE,
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("The 10th Nazgul",			AMULET_OF_ANTI_EXPERIENCE, /* heavily curses itself, disables drain resistance, gives free action and manaleech */
	(SPFX_RESTR|SPFX_REGEN|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Very Tricky Indeed",			BAG_OF_TRICKS, /* spawns more monsters than usual */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("One Size Fits Everything",			ICE_BOX_OF_HOLDING, /* like Wallet of Perseus */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Stonesplitter",			PICK_AXE,
	(SPFX_RESTR|SPFX_SEEK), 0, 0,
	PHYS(2,5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Darkening Thing",			UNICORN_HORN, /* applying it aggravates monsters */
	(SPFX_RESTR), 0, 0,
	PHYS(3,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Roommate's Special Idea",			LASER_SWATTER,
	(SPFX_RESTR|SPFX_BEHEAD), 0, S_XAN,
	PHYS(0,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Lights, Camera, Action",			EXPENSIVE_CAMERA, /* if applied while it has charges, it scares all nearby monsters */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Fairest In The Land",			MIRROR, /* supposed to pacify nymphs if you apply it at them */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("The Eyes of the Spying Academy",			LENSES,
	(SPFX_RESTR|SPFX_ESP|SPFX_SEEK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Blindfold of Mispelling",			BLINDFOLD, /* confusion problem, autocurses */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("The Answer Is 42",			TOWEL, /* applying it, or putting it on, causes a random bad effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Penis Safety",			CONDOME,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Knight's Aid",			SADDLE, /* artifact saddles exist just so that deities can sometimes unrestrict your riding skill */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Vroom Vroom",			SADDLE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Motorcycle Race",			SADDLE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Yasdorian's Trophy Getter",			TINNING_KIT, /* always generates blessed tins, but also summons a random monster! */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Yasdorian's Junethack Identity",			TIN_OPENER, /* psi resistance */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Tin-Fu",			TIN_OPENER, /* +20 damage when used as a melee weapon */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_SUPERMARKET_CASHIER, NON_PM, 0L ),

A("Vibe Lube",			CAN_OF_GREASE, /* instantly applies the full 3 layers of grease with a single charge */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Guaranteed Special Pet",			FIGURINE, /* whatever monster comes out of it will always be tame */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Pen of Randomness",			MAGIC_MARKER, /* randomly chooses BUC status of target scroll */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Egg of Splat",			EGG, /* eating it makes you deathly sick */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Hoe Pa",			FOOD_RATION, /* eating it gives temporary resistance to fire, cold, shock, poison, rnz(10000) turns for each */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Yasdorian's Partly Eaten Tin",			TIN, /* eating it gives intrinsic magic resistance and nastiness, disables poison and sickness resistance for 1 million turns */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Boomshine",			POT_BOOZE, /* does d50 damage and gives intrinsic confusion */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Cursed Parts",			POT_BANISHING_FEAR, /* gives intrinsic fear resistance and item cursing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Planechangers",			POT_POLYMORPH, /* gives intrinsic polymorphitis */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Sandman Volume 8",			SPE_DRAIN_LIFE, /* all the artifact spellbooks should have +10 melee damage */
	(SPFX_RESTR|SPFX_DRLI|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("And Your Mortal World Shall Perish In Flames",			SPE_CURE_BURN,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	FIRE(5,5), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Source Codes of Work Avoidance",			SPE_AMNESIA, /* free action and flying when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Eru Iluvatar's Bible",			SPE_ALTER_REALITY, /* poison resistance and 20% life saving when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Orthodox Manifest",			SPE_TURN_UNDEAD, /* the turn undead technique works even in Gehennom while you wield this */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("The Secrets of Invisible Pleasure",			SPE_MAKE_VISIBLE, /* invisibility and see invis when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Acta Metallurgica Vol. 3",			SPE_CORRODE_METAL, /* improves AC by 5 when wielded */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("IBM guild manual",			SCR_CONFUSE_MONSTER, /* intrinsic confusing problem when read */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Nothing Ventured Nothing Gained",			WAN_NOTHING, /* if you or a monster zap an artifact wand, it should not lose a charge 50% of the time; bashing with it should add the # of charges (if positive) to the damage output */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Know Your Intrinsics",			WAN_ENLIGHTENMENT, /* +5 INT/WIS when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Stealing Protection",			WAN_MAKE_INVISIBLE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Rare Findings",			WAN_SECRET_DOOR_DETECTION,
	(SPFX_RESTR|SPFX_SEEK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Snare Begone",			WAN_TRAP_DISARMING,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Fook Yoo",			WAN_GENOCIDE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Overleveler",			WAN_STRIKING,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Un-Death",			WAN_UNDEAD_TURNING,
	(SPFX_RESTR|SPFX_DEFN|SPFX_DRLI), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Not Known Anymore",			WAN_AMNESIA, /* after the amnesia incident, secure identifies exactly one item for you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Uselessness of Plenty",			WAN_FEAR, /* fear resistance when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Arabella's Secret Weapon",			WAN_BANISHMENT, /* 90% chance of not losing a charge if a monster zaps it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("AWOL Party",			WAN_CREATE_HORDE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Funny Segfault",			WAN_VENOM_SCATTERING, /* does not actually cause segfaults */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Bunga Bunga",			WAN_SUMMON_SEXY_GIRL, /* +5 + number of charges charisma when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Sovetskaya Pyat' Lo Nenavidit Igroki",			WAN_DESLEXIFICATION,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Faeaeaeaeaeau",			WAN_FIRE,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Digger Gem",			EMERALD,
	(SPFX_RESTR), 0, 0,
	ACID(5,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Khor's Curse",			LUCKSTONE, /* autocurses and sets itself to -5 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Strange Protector",			ACID_VENOM,
	(SPFX_RESTR), (SPFX_PROTEC), 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Roadrash Weapon",			IRON_CHAIN,
	(SPFX_RESTR), 0, 0,
	PHYS(0,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Vuvuzela",			TOOLED_HORN, /* aggravates monsters when applied (idea by yasdorian) */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

/*
 *	Stormbringer only has a 2 because it can drain a level,
 *	providing 8 more.
 */
A("Stormbringer",               RUNESWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_INTEL|SPFX_DRLI), 0, 0,
	DRLI(5,1),	DRLI(0,0),	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),


A("Reaver",			SCIMITAR,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL),0,0,
	PHYS(5,4),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, PM_PIRATE, NON_PM, 6000L ),

#ifdef BLACKMARKET
A("Thiefbane",                  LONG_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_BEHEAD|SPFX_DCLAS|SPFX_DRLI), 0, S_HUMAN,
	DRLI(5,1),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),
#endif /* BLACKMARKET */

A("Deathsword",                   TWO_HANDED_SWORD,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_HUMAN,
	PHYS(5,7),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_BARBARIAN, NON_PM, 5000L ),

A("Bat from Hell",                BASEBALL_BAT,
	(SPFX_RESTR), 0, 0,
	PHYS(3,10),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_ROGUE, NON_PM, 5000L ),

A("Elfrist",                    ORCISH_SPEAR,
	SPFX_DFLAG2, 0, M2_ELF,
	PHYS(5,8),     NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_HUMAN_WEREWOLF, PM_ORC, 300L ),

A("Plague", DARK_ELVEN_BOW,	/* KMH */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5,4),        DFNS(AD_DRST),  NO_CARY,        0, A_CHAOTIC, PM_DROW, NON_PM, 6000L ),
	/* Auto-poison code in dothrow.c */


A("Mumakbane",			LONG_SWORD,
	(SPFX_RESTR|SPFX_DMONS), 0, PM_MUMAK,
	FIRE(5,30),	NO_DFNS,	NO_CARY,
	0, 		A_NEUTRAL,	NON_PM, NON_PM, 500L ),

A("Wormbiter",			CRYSKNIFE,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_WORM,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	0,		A_NEUTRAL,	NON_PM, NON_PM, 2000L ),

A("Shocker",			ELECTRIC_SWORD,
	SPFX_RESTR, 0, 0,
	ELEC(3,0),	NO_DFNS,	NO_CARY,
	0,		A_NEUTRAL,	NON_PM, NON_PM, 1000L ),

A("Scales of the Dragon Lord",	GRAY_DRAGON_SCALE_MAIL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_PROTEC), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	DRAGON_BREATH,	A_CHAOTIC,	NON_PM, NON_PM, 5000L ),

A("The Burned Moth Relay",	RELAY,
	(SPFX_RESTR), SPFX_PROTEC, 0, 	
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,		A_NEUTRAL,	NON_PM, NON_PM, 200L ),

A("The Key Of Access",		SKELETON_KEY,
	(SPFX_RESTR|SPFX_NOGEN),	0,		0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	CREATE_PORTAL,	A_NONE,		NON_PM, NON_PM, 200L ),

A("Hellfire", CROSSBOW,	/* WAC */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5,4),        FIRE(0,0),  NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),
	/* Auto-explode code in dothrow.c, uhitm.c */

A("Houchou",                SPOON,
	(SPFX_RESTR), 0, 0,
        NO_ATTK,      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 50000L ),

/*** Special Artifacts ***/

/* KMH -- made it a bag of holding */
A("Wallet of Perseus",       BAG_OF_HOLDING,
        (SPFX_RESTR), 0, 0,
        NO_ATTK,        NO_DFNS,        NO_CARY,
        0,      A_NONE, NON_PM, NON_PM, 10000L ),

A("Nighthorn", UNICORN_HORN,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,      A_LAWFUL, NON_PM, NON_PM, 10000L ),

A("The Key of Law", SKELETON_KEY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,      A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("The Eye of the Beholder", EYEBALL,
	(SPFX_NOGEN|SPFX_RESTR), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	DEATH_GAZE,     A_NEUTRAL, NON_PM, NON_PM, 500L ),

A("The Key of Neutrality", SKELETON_KEY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,      A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("The Hand of Vecna",       SEVERED_HAND,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_REGEN|SPFX_HPHDAM), 0, 0,
	NO_ATTK,        DRLI(0,0),      CARY(AD_COLD),
	SUMMON_UNDEAD,          A_CHAOTIC, NON_PM, NON_PM, 700L ),

A("The Key of Chaos", SKELETON_KEY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,      A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("The Gauntlet Key", SKELETON_KEY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,      A_LAWFUL, NON_PM, NON_PM, 1000L ),


/*** The artifacts for the quest dungeon, all self-willed ***/

A("The Orb of Detection",	CRYSTAL_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_ESP|SPFX_HSPDAM), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	INVIS,		A_LAWFUL, PM_ARCHEOLOGIST, NON_PM, 2500L ),

A("The Ball of Light",	CRYSTAL_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_ESP|SPFX_HSPDAM), 0,
	NO_ATTK,	DFNS(AD_BLND),	CARY(AD_MAGM),
	LIGHT_AREA,		A_LAWFUL, PM_ERDRICK, NON_PM, 2500L ),

A("The Heart of Ahriman",	LUCKSTONE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), SPFX_STLTH, 0,
	/* this stone does double damage if used as a projectile weapon */
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	LEVITATION,	A_NEUTRAL, PM_BARBARIAN, NON_PM, 2500L ),

A("The Arkenstone",	DIAMOND,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), SPFX_ESP, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	HEALING,	A_LAWFUL, PM_MIDGET, NON_PM, 2500L ),

A("The Sceptre of Might",	MACE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DALIGN), 0, 0,
	PHYS(3,3),      NO_DFNS,        CARY(AD_MAGM),
	CONFLICT,	A_LAWFUL, PM_CAVEMAN, NON_PM, 2500L ),

A("Mysterious Spikes",	TAIL_SPIKES,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DALIGN|SPFX_XRAY), 0, 0,
	PHYS(30,30),      NO_DFNS,        CARY(AD_MAGM),
	HEALING,	A_LAWFUL, PM_MYSTIC, NON_PM, 200000L ),

#ifdef CONVICT
A("The Iron Ball of Liberation", HEAVY_IRON_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL),
		(SPFX_STLTH|SPFX_SEARCH|SPFX_WARN), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	PHASING,	A_NEUTRAL, PM_PRISONER, NON_PM, 5000L ),
#endif	/* CONVICT */

A("Pitch Blade",			BROADSWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_TCTRL|SPFX_INTEL),0,0,
	PHYS(5,3),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, PM_MURDERER, NON_PM, 3000L ),

#if 0	/* OBSOLETE */
A("The Palantir of Westernesse",	CRYSTAL_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL),
		(SPFX_ESP|SPFX_REGEN|SPFX_HSPDAM), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	TAMING,		A_CHAOTIC, NON_PM , PM_ELF, 8000L ),
#endif

/* STEPHEN WHITE'S NEW CODE */

/* Let's reintroduce the elf role. --Amy */
A("The Palantir of Westernesse",	CRYSTAL_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL),
		(SPFX_ESP|SPFX_REGEN|SPFX_HSPDAM), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	TAMING,		A_CHAOTIC, PM_ELPH , NON_PM, 8000L ),

A("Rocker Sling",                SLING,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DFLAG2), 0, M2_GIANT,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, PM_ROCKER, NON_PM, 800L ),

A("Liontamer",                BULLWHIP,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DCLAS), 0, S_FELINE,
	PHYS(5,8),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, PM_ZOOKEEPER, NON_PM, 800L ),

A("Dragonclan Sword",                KATANA,
	(SPFX_RESTR|SPFX_BEHEAD|SPFX_INTEL), 0, 0,
	PHYS(3,10),      NO_DFNS,        NO_CARY,        0, A_LAWFUL, PM_NINJA, NON_PM, 5000L ),

A("Killing Edge",                KATANA,
	(SPFX_RESTR|SPFX_BEHEAD|SPFX_INTEL), 0, 0,
	PHYS(3,3),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_ASSASSIN, NON_PM, 5000L ),

A("Black Death",                     HALBERD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK), 0, 0,
	DRLI(5,5),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_UNDERTAKER, NON_PM, 3000L ),

A("Slow Blade",                ORCISH_SHORT_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_SEARCH|SPFX_REGEN ), 0,
	PHYS(2,1),      NO_DFNS,        NO_CARY,        HEALING, A_LAWFUL, PM_ACID_MAGE, NON_PM, 1000L ),

A("Fire-Brigade Reel",                RUBBER_HOSE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_HSPDAM|SPFX_HPHDAM ), 0,
	COLD(4,4),      NO_DFNS,        NO_CARY,        SUMMON_WATER_ELEMENTAL, A_LAWFUL, PM_FIREFIGHTER, NON_PM, 2000L ),

A("The Candle of Eternal Flame",        MAGIC_CANDLE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_WARN|SPFX_TCTRL), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_COLD),
	SUMMON_FIRE_ELEMENTAL,         A_NEUTRAL, PM_FLAME_MAGE, NON_PM, 50000L ),

A("The Nethack Sources",	PACK_OF_FLOPPIES,
	(SPFX_NOGEN | SPFX_RESTR | SPFX_INTEL),
		 (SPFX_SEARCH | SPFX_ESP | SPFX_REGEN), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	IDENTIFY,	A_NEUTRAL,	PM_GEEK, NON_PM, 5000L ),

A("Master Boot Disk",	PACK_OF_FLOPPIES,
	(SPFX_NOGEN | SPFX_RESTR | SPFX_INTEL),
		 (SPFX_REFLECT), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	PHASING,	A_NEUTRAL,	PM_GRADUATE, NON_PM, 5000L ),

A("The Lyre of Orpheus",	WOODEN_HARP,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK), 0, 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	TAMING,		A_NEUTRAL, PM_BARD, NON_PM, 5000L ),

A("Operational Scalpel",	SCALPEL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_ATTK|SPFX_INTEL|SPFX_DRLI|SPFX_REGEN), 0,0,
	DRLI(3,0),      NO_DFNS,        NO_CARY,
	HEALING,	A_NEUTRAL, PM_SCIENTIST, NON_PM, 5000L ),

A("The Staff of Aesculapius",	QUARTERSTAFF,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_ATTK|SPFX_INTEL|SPFX_DRLI|SPFX_REGEN), 0,0,
	DRLI(3,0),      NO_DFNS,        NO_CARY,
	HEALING,	A_NEUTRAL, PM_HEALER, NON_PM, 5000L ),

A("The Tentacle Staff",	QUARTERSTAFF,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_ATTK|SPFX_INTEL|SPFX_WARN|SPFX_STLTH), 0,0,
	ELEC(8,0),      NO_DFNS,        NO_CARY,
	CHARGE_OBJ,	A_CHAOTIC, PM_TWELPH, NON_PM, 5000L ),

/* STEPHEN WHITE'S NEW CODE */
A("The Storm Whistle",          MAGIC_WHISTLE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_WARN|SPFX_TCTRL), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_FIRE),
	SUMMON_WATER_ELEMENTAL,         A_LAWFUL, PM_ICE_MAGE, NON_PM, 1000L ),

A("The Thunder Whistle",          MAGIC_WHISTLE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_WARN|SPFX_TCTRL), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_ELEC),
	0,         A_CHAOTIC, PM_ELECTRIC_MAGE, NON_PM, 1000L ),

A("Immunity Ring",          RIN_SICKNESS_RESISTANCE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN), (SPFX_ESP), 0,
	NO_ATTK,        DFNS(AD_MAGM),        CARY(AD_DRLI),
	CHARGE_OBJ,         A_NEUTRAL, PM_POISON_MAGE, NON_PM, 1000L ),

A("Blackharp",          MAGIC_HARP,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_WARN|SPFX_TCTRL), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_DRLI),
	0,         A_CHAOTIC, PM_MUSICIAN, NON_PM, 1000L ),

A("The Magic Mirror of Merlin", MIRROR,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK), SPFX_ESP, 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_COLD),
	0,		A_LAWFUL, PM_KNIGHT, NON_PM, 1500L ),

A("The Magic Mirror of Arthubert", MIRROR,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK), SPFX_HSPDAM, 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_STUN),
	0,		A_LAWFUL, PM_CHEVALIER, NON_PM, 1500L ),

A("Magic Mirror of Jason", MIRROR,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK), SPFX_HPHDAM, 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	0,		A_LAWFUL, PM_WARRIOR, NON_PM, 1500L ),

A("Chekhov's Gun", PISTOL,	/* KMH */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_DEFN|SPFX_INTEL), 0, 0,
	PHYS(5,4),        DFNS(AD_DRST),  NO_CARY,        0, A_CHAOTIC, PM_GANGSTER, NON_PM, 6000L ),

A("Shiny Magnum", PISTOL,	/* KMH */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_DEFN|SPFX_INTEL), 0, 0,
	PHYS(10,8),        DFNS(AD_DETH),  NO_CARY,        0, A_LAWFUL, PM_OFFICER, NON_PM, 10000L ),

A("Withered Nine Millimeter", PISTOL,	/* KMH */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_DEFN|SPFX_INTEL), 0, 0,
	PHYS(5,4),        NO_DFNS,  NO_CARY,        0, A_NEUTRAL, PM_COURIER, NON_PM, 3000L ),

A("Training SMG", SUBMACHINE_GUN,	/* KMH */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_DEFN|SPFX_INTEL), 0, 0,
	PHYS(5,4),        NO_DFNS,  NO_CARY,        ENLIGHTENING, A_NEUTRAL, PM_INTEL_SCRIBE, NON_PM, 3000L ),

A("Charged USB stick", INSECT_SQUASHER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,        NO_DFNS,  NO_CARY,        CHARGE_OBJ, A_NEUTRAL, PM_WANDKEEPER, NON_PM, 3000L ),

A("Verbal Blade",		ELVEN_SHORT_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_BEHEAD), 0, 0,
	PHYS(5,1),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, PM_ZYBORG, NON_PM, 3000L ),

A("Tome Dark Sword",		BLACK_AESTIVALIS,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, SPFX_HSPDAM|SPFX_HPHDAM|SPFX_ESP|SPFX_STLTH,
	NO_ATTK,	NO_DFNS,	NO_CARY,	ENLIGHTENING, A_NEUTRAL, PM_UNBELIEVER, NON_PM, 1000L ),

A("The Elder Staff",	QUARTERSTAFF,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_ATTK|SPFX_INTEL|SPFX_DRLI|SPFX_REGEN), 0,0,
	DRLI(6,0),      NO_DFNS,        NO_CARY,
	0,	A_CHAOTIC, PM_DEATH_EATER, NON_PM, 5000L ),

A("Gauntlets of Illusion",    GAUNTLETS_OF_DEXTERITY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), SPFX_HALRES, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,    INVIS, A_NEUTRAL, PM_POKEMON, NON_PM, 5000L ),

A("Lovely Pink Pumps",    FEMININE_PUMPS,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), SPFX_REGEN, 0,
	NO_ATTK,        DRLI(0,0),        NO_CARY,    INVIS, A_NEUTRAL, PM_TRANSVESTITE, NON_PM, 5000L ),

A("Kiss Boots",    HIPPIE_HEELS,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), SPFX_EREGEN, SPFX_HSPDAM|SPFX_HPHDAM,
	NO_ATTK,        DRLI(0,0),        NO_CARY,    IDENTIFY, A_NEUTRAL, PM_TRANSSYLVANIAN, NON_PM, 5000L ),

A("Golden High Heels",    LEATHER_PEEP_TOES,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), SPFX_STLTH, 0,
	NO_ATTK,        ACID(0,0),        NO_CARY,    LEVITATION, A_NEUTRAL, PM_TOPMODEL, NON_PM, 5000L ),

A("Unobtainable Beauties",    SENTIENT_HIGH_HEELS,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_WARN|SPFX_REFLECT|SPFX_SPEAK), SPFX_REGEN, 0,
	NO_ATTK,        ACID(0,0),        NO_CARY,    HEALING, A_NEUTRAL, PM_FAILED_EXISTENCE, NON_PM, 5000L ),

A("Activist Stick",			FLY_SWATTER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DCLAS), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, PM_ACTIVISTOR, NON_PM, 4800L ),

A("The Eyes of the Overworld",	LENSES,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_XRAY), 0, 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	ENLIGHTENING,	A_NEUTRAL,	 PM_MONK, NON_PM, 2500L ),

A("Gauntlets of Offense",    GAUNTLETS_OF_POWER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), SPFX_HPHDAM, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,    INVIS, A_NEUTRAL, PM_PSION, NON_PM, 5000L ),

A("the Pen of the Void",	ATHAME,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	0,	A_NEUTRAL, PM_BINDER, NON_PM, 2500L),

A("Bloody Beauty",	BLOCK_HEELED_COMBAT_BOOT,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK), 0, 0,
	DRLI(4,2),	NO_DFNS,	NO_CARY,
	0,	A_NEUTRAL, PM_BLEEDER, NON_PM, 2500L),

A("Goffic Backpack",	SACK,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL),
		 (SPFX_HSPDAM|SPFX_HPHDAM|SPFX_REGEN), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	ENERGY_BOOST,	A_NEUTRAL,	PM_GOFF, NON_PM, 5000L ),

A("The Mantle of Heaven",	LEATHER_CLOAK, /*Actually an ornamental cope in game*/
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HSPDAM), 0, 0,
	NO_ATTK,	ELEC(0,0),	COLD(0,0), /* Plus double AC bonus */
	0,	A_LAWFUL,	 PM_NOBLEMAN, NON_PM, 2500L),

A("The Vestment of Hell",	LEATHER_CLOAK, /*Actually an opera cloak in game*/
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM), 0, 0,
	NO_ATTK,	ACID(0,0),	FIRE(0,0), /* Plus double AC bonus */
	0,	A_CHAOTIC,	 PM_NOBLEMAN, NON_PM, 2500L),

A("The Cloak of Neutrality",	LEATHER_CLOAK, /*Actually an opera cloak in game*/
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_EREGEN), 0, 0,
	NO_ATTK,	DRLI(0,0),	ACID(0,0), /* Plus double AC bonus */
	0,	A_NEUTRAL,	 PM_DRUNK, NON_PM, 2500L),

A("The Great Dagger of Glaurgnaa",       GREAT_DAGGER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_ATTK|SPFX_INTEL|SPFX_DRLI|SPFX_DALIGN), 0, 0,
	DRLI(8,2),      NO_DFNS,        CARY(AD_MAGM),
	ENERGY_BOOST,   A_CHAOTIC, PM_NECROMANCER, NON_PM, 50000L ),

A("The Mitre of Holiness",	HELM_OF_BRILLIANCE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_DFLAG2|SPFX_INTEL), 0, M2_UNDEAD,
	NO_ATTK,	NO_DFNS,	CARY(AD_FIRE),
	ENERGY_BOOST,	A_LAWFUL, PM_PRIEST, NON_PM, 2000L ),

A("Painkiller",        STEEL_WHIP,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DFLAG2), 0, M2_HUMAN,
	PHYS(12,12),    NO_DFNS,        CARY(AD_MAGM),
	0,              A_CHAOTIC, PM_CRUEL_ABUSER, NON_PM, 5000L ),

A("Dragon Whip",                  BULLWHIP,
	(SPFX_RESTR|SPFX_NOGEN|SPFX_DEFN|SPFX_INTEL|SPFX_ATTK|SPFX_DFLAG2),0,M2_DOMESTIC,
	PHYS(5,5),	DFNS(AD_FIRE),	NO_CARY,	DRAGON_BREATH, A_CHAOTIC, PM_SLAVE_MASTER, NON_PM, 4000L ),

A("Rupturer",                  RUNESWORD,
	(SPFX_RESTR|SPFX_NOGEN|SPFX_WARN|SPFX_INTEL|SPFX_ATTK),0,0,
	PHYS(3,7),	DFNS(AD_COLD),	NO_CARY,	DRAGON_BREATH, A_CHAOTIC, PM_BLOODSEEKER, NON_PM, 4000L ),

A("Blood Marker",        KNIFE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_HSPDAM), 0, 0,
	NO_ATTK,    DFNS(AD_MAGM),        NO_CARY,
	IDENTIFY,              A_NEUTRAL, PM_LIBRARIAN, NON_PM, 5000L ),

A("Cutthroat Blade",        CRYSKNIFE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_BEHEAD), 0, 0,
	NO_ATTK,    DFNS(AD_MAGM),        NO_CARY,
	DEATH_GAZE,              A_NEUTRAL, PM_PICKPOCKET, NON_PM, 5000L ),

A("Sharpened Toothpick",        GREAT_DAGGER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_SEEK|SPFX_SEARCH), 0, 0,
	PHYS(8,8),    NO_DFNS,        NO_CARY,
	0,              A_NEUTRAL, PM_BULLY, NON_PM, 5000L ),

A("Kitchen Cutter",        KNIFE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DFLAG2), 0, M2_STRONG,
	PHYS(6,6),    NO_DFNS,        NO_CARY,
	0,              A_NEUTRAL, PM_COOK, NON_PM, 3000L ),

A("Archon Staff",        BATTLE_STAFF,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DFLAG2), SPFX_EREGEN, M2_DEMON,
	PHYS(10,10),    NO_DFNS,        NO_CARY,
	OBJ_DETECTION,              A_LAWFUL, PM_AUGURER, NON_PM, 3000L ),

A("Shillelagh",        BATTLE_STAFF,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DFLAG2), SPFX_EREGEN, M2_NASTY,
	PHYS(16,4),    NO_DFNS,        NO_CARY,
	LIGHT_AREA,              A_LAWFUL, PM_SAGE, NON_PM, 3000L ),

A("Altar Carver",        ATHAME,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DRLI|SPFX_DFLAG2), 0, M2_STALK,
	PHYS(8,8),    NO_DFNS,        NO_CARY,
	SUMMON_UNDEAD,              A_LAWFUL, PM_OTAKU, NON_PM, 3000L ),

A("Mirage Tail",        ROTATING_CHAIN,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN), 0, 0,
	PHYS(4,4),    DFNS(AD_FIRE),        NO_CARY,
	DRAGON_BREATH,              A_NEUTRAL, PM_ARTIST, NON_PM, 5000L ),

A("Game Disc",        SPOON,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_HPHDAM), 0, 0,
	NO_ATTK,    DFNS(AD_FIRE),        NO_CARY,
	0,              A_NEUTRAL, PM_GAMER, NON_PM, 5000L ),

A("Modified Z-Sword",        PICK_AXE, /* sorry Bug Sniper, I'm too lazy coding a sword to work as a pick-axe. :( Good idea though. --Amy */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(8,4),    NO_DFNS,        NO_CARY,
	ENERGY_BOOST,              A_LAWFUL, PM_SAIYAN, NON_PM, 5000L ),

A("The Pick of Flandal Steelskin",        PICK_AXE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM), 0, 0,
	NO_ATTK,    NO_DFNS,        NO_CARY,
	0,              A_NEUTRAL, PM_GOLDMINER, NON_PM, 5000L ),

A("Prime Minister's Tuxedo",	T_SHIRT,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM|SPFX_DEFN), 0, 0,
	NO_ATTK,	DFNS(AD_MAGM),	NO_CARY,
	0,	A_LAWFUL, PM_POLITICIAN, NON_PM, 2000L ),

A("Slowness Shirt",	HAWAIIAN_SHIRT,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HSPDAM|SPFX_DEFN), 0, 0,
	NO_ATTK,	DFNS(AD_DRLI),	NO_CARY,
	0,	A_LAWFUL, PM_LADIESMAN, NON_PM, 2000L ),

A("Coat of Style",	LEATHER_JACKET,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM|SPFX_DEFN), 0, 0,
	NO_ATTK,	DFNS(AD_ACID),	NO_CARY,
	ENLIGHTENING,	A_CHAOTIC, PM_FEAT_MASTER, NON_PM, 2000L ),

A("Carbon Nanotube Suit",	LEATHER_ARMOR,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM|SPFX_DEFN), 0, 0,
	NO_ATTK,	DFNS(AD_MAGM),	NO_CARY,
	UNTRAP,	A_CHAOTIC, PM_GUNNER, NON_PM, 2000L ),

A("Brutal Chainsaw",	CHAINSWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK), 0, 0,
	PHYS(10,10),	NO_DFNS,	NO_CARY,
	CREATE_PORTAL,	A_NEUTRAL, PM_DOOM_MARINE, NON_PM, 4000L ),

A("The Treasury of Proteus",	CHEST,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_LUCK), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	0,	A_CHAOTIC,	 PM_PIRATE, NON_PM, 2500L ),

A("Portchest",	CHEST,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	CREATE_PORTAL,	A_LAWFUL,	 PM_FOXHOUND_AGENT, NON_PM, 2500L ),

A("Saint Something Four Crystal",	DILITHIUM_CRYSTAL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_REGEN|SPFX_EREGEN|SPFX_REFLECT), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	TAMING,	A_NEUTRAL,	 PM_MAHOU_SHOUJO, NON_PM, 12500L ),

A("Master Ball",	REALLY_HEAVY_IRON_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_REGEN|SPFX_EREGEN|SPFX_REFLECT), 0, 0,
	PHYS(16,16),	DFNS(AD_MAGM),	NO_CARY,
	TAMING,	A_NEUTRAL,	 PM_DOLL_MISTRESS, NON_PM, 50000L ),

A("The One Ring",	RIN_INVISIBILITY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_REGEN|SPFX_EREGEN|SPFX_REFLECT), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	CREATE_PORTAL,	A_LAWFUL,	 PM_RINGSEEKER, NON_PM, 50000L ),

A("Imperial Token", FEDORA, /* sorry Fyr, making the rings work correctly would be a major pain in the butt --Amy */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN), 0, 0,
	NO_ATTK,       DRLI(0,0),        NO_CARY,
	0,  A_NEUTRAL, PM_GLADIATOR, NON_PM, 2000L ),

A("The Pearl of Wisdom", FEDORA,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM), 0, 0,
	NO_ATTK,       NO_DFNS,        NO_CARY,
	0,  A_NEUTRAL, PM_KORSAIR, NON_PM, 2000L ),

A("Maui's Fishhook",	GRAPPLING_HOOK,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HSPDAM|SPFX_WARN|SPFX_SEEK|SPFX_DCLAS), 0, S_EEL,
	PHYS(10,0),        NO_DFNS,        NO_CARY,
	0,		A_CHAOTIC, PM_DIVER, NON_PM, 4500L ),


A("Helm of Storms", HELMET,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM), 0, 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_MAGM),
	0,  A_NEUTRAL, PM_SPACEWARS_FIGHTER, NON_PM, 5000L ),

A("Amulet of Kings", AMULET_OF_DRAIN_RESISTANCE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	CREATE_PORTAL,  A_LAWFUL, PM_CAMPERSTRIKER, NON_PM, 1000L ),

A("The Longbow of Diana", BOW,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_REFLECT), SPFX_ESP, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	CREATE_AMMO, A_CHAOTIC, PM_RANGER, NON_PM, 4000L ),

A("Heffer's Bow", BOW,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_WARN|SPFX_REGEN|SPFX_EREGEN), 0, 0,
	PHYS(6,0),	NO_DFNS,	NO_CARY,
	CREATE_AMMO, A_LAWFUL, PM_DRUID, NON_PM, 8000L ),

A("Gunbow", BOW,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM), SPFX_EREGEN, 0,
	ELEC(7,0),	DRLI(0,0),	NO_CARY,
	CREATE_AMMO, A_NEUTRAL, PM_AMAZON, NON_PM, 4000L ),

A("The Master Key of Thievery", SKELETON_KEY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK),
		(SPFX_WARN|SPFX_TCTRL|SPFX_HPHDAM), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	UNTRAP,		A_CHAOTIC, PM_ROGUE, NON_PM, 3500L ),

A("Nocturnal Key", SKELETON_KEY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK),
		(SPFX_SEEK|SPFX_HALRES|SPFX_LUCK), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	CREATE_PORTAL,		A_CHAOTIC, PM_LOCKSMITH, NON_PM, 3500L ),

A("The Tsurugi of Muramasa",	TSURUGI,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_BEHEAD|SPFX_LUCK), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,		A_LAWFUL, PM_SAMURAI, NON_PM, 4500L ),

A("Vivec Blade",	TSURUGI,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_LUCK), 0, 0,
	PHYS(8,4),        NO_DFNS,        NO_CARY,
	0,		A_NEUTRAL, PM_ORDINATOR, NON_PM, 4500L ),

A("Summoned Sword",                ELVEN_BROADSWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_STLTH|SPFX_REGEN ), 0,
	PHYS(4,4),      NO_DFNS,        NO_CARY,        DRAGON_BREATH, A_CHAOTIC, PM_THALMOR, NON_PM, 3000L ),

A("Bow of Vines", ELVEN_BOW,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_LUCK), SPFX_ESP, 0,
	STUN(7,5),	NO_DFNS,	NO_CARY,
	ENERGY_BOOST, A_NEUTRAL, PM_BOSMER, NON_PM, 4000L ),

A("Ambassador Robe",	ROBE_OF_WEAKNESS,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HSPDAM|SPFX_HPHDAM), SPFX_EREGEN, 0,
	NO_ATTK,	DRLI(0,0),	CARY(AD_MAGM),
	CREATE_PORTAL,	A_CHAOTIC,	 PM_ALTMER, NON_PM, 2500L),

A("N'wah Killer",        ELVEN_BROADSWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(5,6),    NO_DFNS,        CARY(AD_MAGM),
	0,              A_LAWFUL, PM_DUNMER, NON_PM, 5000L ),

A("Supreme Justice Keeper",		TWO_HANDED_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_LUCK|SPFX_PROTEC|SPFX_INTEL|SPFX_DALIGN), 0, A_NONE,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	0, 		A_LAWFUL, NON_PM, NON_PM, 200L ),

A("The Holydirk",		ATHAME,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_REFLECT|SPFX_INTEL|SPFX_DFLAG2), SPFX_HPHDAM, M2_UNDEAD,
	PHYS(5,0),	NO_DFNS,	CARY(AD_MAGM),
	0,	A_LAWFUL, PM_MEDIUM, NON_PM, 1500L ),

A("The Charmpoint",		SILVER_DAGGER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_REFLECT|SPFX_INTEL|SPFX_DFLAG2), SPFX_HPHDAM, M2_UNDEAD,
	PHYS(5,0),	NO_DFNS,	CARY(AD_MAGM),
	TAMING,	A_CHAOTIC, PM_SEXYMATE, NON_PM, 1500L ),

A("The Silver Crystal",		DILITHIUM_CRYSTAL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_ESP|SPFX_REGEN|SPFX_DBONUS), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	HEALING,	A_LAWFUL, PM_FIGHTER, NON_PM, 8000L ),

A("The Red Stone of Eigia",		RUBY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_WARN|SPFX_DRLI), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_FIRE),
	OBJ_DETECTION,	A_NEUTRAL, PM_STAND_USER, NON_PM, 1000L ),

A("The Fortune Sword",			SHORT_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_LUCK),
		(SPFX_ESP|SPFX_DBONUS), 0,
	PHYS(3,4),	NO_DFNS,	CARY(AD_MAGM),
	0,	A_NEUTRAL,	PM_FENCER, NON_PM, 10000L ),

# ifdef TOURIST
A("The Platinum Yendorian Express Card", CREDIT_CARD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN),
		(SPFX_ESP|SPFX_HSPDAM), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	CHARGE_OBJ,	A_NEUTRAL, PM_TOURIST, NON_PM, 7000L ),
# endif

A("Credex Gold", CREDIT_CARD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN),
		(SPFX_STLTH|SPFX_TCTRL), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	CHARGE_OBJ,	A_NEUTRAL, PM_SUPERMARKET_CASHIER, NON_PM, 7000L ),

/* KMH -- More effective against normal monsters
 * Was +10 to-hit, +d20 damage only versus vampires
 */
A("The Stake of Van Helsing",        WOODEN_STAKE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(5,6),    NO_DFNS,        CARY(AD_MAGM),
	0,              A_LAWFUL, PM_UNDEAD_SLAYER, NON_PM, 5000L ),

A("Vampire Killer",        BULLWHIP,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(5,6),    NO_DFNS,        CARY(AD_MAGM),
	0,              A_LAWFUL, PM_LUNATIC, NON_PM, 5000L ),

A("Itchalaquiaque",        DROVEN_DAGGER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(5,6),    NO_DFNS,        CARY(AD_MAGM),
	0,              A_LAWFUL, PM_ANACHRONIST, NON_PM, 5000L ),

A("The Orb of Fate",		CRYSTAL_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_LUCK),
		(SPFX_WARN|SPFX_HSPDAM|SPFX_HPHDAM), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	LEV_TELE,	A_NEUTRAL, PM_VALKYRIE, NON_PM, 3500L ),

A("The Orb of Resistance",		CRYSTAL_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL),
		(SPFX_WARN|SPFX_HSPDAM|SPFX_HPHDAM), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	LIGHT_AREA,	A_NEUTRAL, PM_PALADIN, NON_PM, 3500L ),

A("The Eye of the Aethiopica",	AMULET_OF_ESP,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_EREGEN|SPFX_HSPDAM), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	CREATE_PORTAL,	A_NEUTRAL, PM_WIZARD, NON_PM, 4000L ),

A("The Medallion of Shifters",	AMULET_OF_ESP,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_EREGEN|SPFX_HPHDAM), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	LEV_TELE,	A_NEUTRAL, PM_SHAPESHIFTER, NON_PM, 4000L ),

A("The King's Stolen Crown", HELM_OF_OPPOSITE_ALIGNMENT,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_HSPDAM|SPFX_HPHDAM), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_MAGM),
	LEV_TELE,  A_LAWFUL, PM_JESTER, NON_PM, 5000L ),

A("The Slime Crown", HELM_OF_STORMS,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_HSPDAM|SPFX_HPHDAM), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_MAGM),
	LEV_TELE,  A_CHAOTIC, PM_DQ_SLIME, NON_PM, 5000L ),

#ifdef YEOMAN
A("The Crown of Saint Edward", HELM_OF_TELEPATHY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_HSPDAM), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_MAGM),
	0,  A_LAWFUL, PM_YEOMAN, NON_PM, 5000L ),
#endif

#ifdef JEDI
A("The Lightsaber Prototype", RED_LIGHTSABER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_REFLECT), 0, 0,
	PHYS(5, 5),	NO_DFNS, NO_CARY,
	ENERGY_BOOST,	A_LAWFUL, PM_JEDI, NON_PM, 3500L ),
#endif


/*
 *  terminator; otyp must be zero
 */
A(0, 0, 0, 0, 0, NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 0L )

};	/* artilist[] (or artifact_names[]) */

#undef	A

#ifndef MAKEDEFS_C
#undef	NO_ATTK
#undef	NO_DFNS
#undef	DFNS
#undef	PHYS
#undef	DRLI
#undef	COLD
#undef	FIRE
#undef	ELEC
#undef	STUN
#endif

/*artilist.h*/
