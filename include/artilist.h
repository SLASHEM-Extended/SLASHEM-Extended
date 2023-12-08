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
#define	MAGM(a,b)	{0,AD_MAGM,a,b}
/* if new types are added, keep randartiattacktype() in rnd.c synced --Amy */

STATIC_OVL NEARDATA struct artifact artilist_pre[] = {
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

/* Amy test entry (dummy) for easier copy-n-pasting */
/*
A("",				itemname,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, ---L ),

*/

/*  dummy element #0, so that all interesting indices are non-zero */
A("",				STRANGE_OBJECT,
	0, 0, 0, NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 0L ),

/*** Lawful artifacts ***/
A("Firewall",                  ATHAME,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(4,4),      FIRE(0,0),      NO_CARY,        0, A_LAWFUL, PM_FLAME_MAGE, NON_PM, 400L ),

/*
 *	The combination of SPFX_WARN and M2_something on an artifact
 *	will trigger EWarn_of_mon for all monsters that have the appropriate
 *	M2_something flags.  In Sting's case it will trigger EWarn_of_mon
 *	for M2_ORC monsters.
 */
A("Sting",			ELVEN_DAGGER,
	(SPFX_WARN|SPFX_DFLAG2), 0, M2_ORC,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, PM_ELF, 800L ),

A("Giantkiller",                AXE,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_GIANT,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 800L ),

A("Quick Blade",                ELVEN_SHORT_SWORD,
	SPFX_RESTR, 0, 0,
	PHYS(9,2),      NO_DFNS,        NO_CARY,        0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Orcrist",                    ELVEN_BROADSWORD,
	SPFX_DFLAG2, 0, M2_ORC,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, PM_ELF, 2000L ),

A("Dragonbane",			BROADSWORD,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_DRAGON,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 500L ),

A("Excalibur",                  LONG_SWORD, /* removed NOGEN --Amy */
	(SPFX_RESTR|SPFX_SEEK|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH),0,0,
	PHYS(5,10),	DRLI(0,0),	NO_CARY,	0, A_LAWFUL, PM_KNIGHT, NON_PM, 4000L ),

A("Luck Blade",			BROADSWORD,
	(SPFX_RESTR|SPFX_LUCK|SPFX_INTEL),0,0,
	PHYS(5,6),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, PM_CONVICT, NON_PM, 3000L ),

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
	PHYS(0,8),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, PM_SAMURAI, NON_PM, 1200L ),

/* KMH -- Renamed from Holy Sword of Law (Stephen White)
 * This is an actual sword used in British coronations!
 */
A("Sword of Justice",           LONG_SWORD,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(5,12),     NO_DFNS,        NO_CARY,        0, A_LAWFUL, PM_YEOMAN, NON_PM, 1500L ),

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
	PHYS(3,10),     NO_DFNS,        NO_CARY,        0, A_LAWFUL, PM_CAVEMAN, NON_PM, 300L ),

A("Trollsbane",                 MORNING_STAR,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_TROLL,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 200L ),

A("Ogresmasher",		WAR_HAMMER,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_OGRE,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 200L ),

A("Reaper",                     HALBERD,
	SPFX_RESTR, 0, 0,
	PHYS(5,20),      NO_DFNS,        NO_CARY,        0, A_LAWFUL, PM_YEOMAN, NON_PM, 1000L ),

A("Holy Spear of Light",        SILVER_SPEAR,
       (SPFX_RESTR|SPFX_INTEL|SPFX_DFLAG2), 0, M2_UNDEAD,
       PHYS(5,10),      NO_DFNS,  NO_CARY,     LIGHT_AREA, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Rod of Lordly Might", MACE, /*needs quote*/
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(3,0),	NO_DFNS,	NO_CARY,
	0,	A_LAWFUL, PM_NOBLEMAN, NON_PM, 4000L ), 

/*** Neutral artifacts ***/
A("Magicbane",                  ATHAME,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	STUN(3,4),	DFNS(AD_MAGM),	NO_CARY,	0, A_NEUTRAL, PM_WIZARD, NON_PM, 3500L ),

A("Doom Chainsaw",	CHAINSWORD,
	(SPFX_RESTR|SPFX_INTEL|SPFX_ATTK), 0, 0,
	PHYS(20,4),	NO_DFNS,	NO_CARY,
	0,	A_NEUTRAL, PM_DOOM_MARINE, NON_PM, 2000L ),

A("Luckblade",                  SHORT_SWORD,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	PHYS(5,6),      NO_DFNS,        NO_CARY,        0, A_NEUTRAL, NON_PM, PM_GNOME, 1000L ),

A("Sword of Balance",           SILVER_SHORT_SWORD,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(2,10),      NO_DFNS,        NO_CARY,        0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

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
	PHYS(5,2),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Disrupter",                  MACE,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_UNDEAD,
	PHYS(5,30),     NO_DFNS,        NO_CARY,        0, A_NEUTRAL, PM_PRIEST, NON_PM, 500L ),

/*
 *	Mjollnir will return to the hand of a Valkyrie when thrown
 *	if the wielder is a Valkyrie with strength of 25 or more.
 */
A("Mjollnir",                   HEAVY_HAMMER,           /* Mjo:llnir - makes your AC worse by 10 points and counts as 500 extra units of weight */
	(SPFX_RESTR|SPFX_ATTK|SPFX_EVIL),  0, 0,
	ELEC(5,24),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, PM_VALKYRIE, NON_PM, 4000L ),

A("Other Mjollnir",                   WAR_HAMMER, /* makes your AC worse by 10 points and counts as 500 extra units of weight */
	(SPFX_RESTR|SPFX_ATTK|SPFX_EVIL),  0, 0,
	ELEC(5,24),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, PM_VANILLA_VALK, NON_PM, 4000L ),

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

A("Whisperfeet",               SPEED_BOOTS,
	(SPFX_RESTR|SPFX_STLTH|SPFX_LUCK), 0, 0,
	NO_ATTK,      NO_DFNS,        NO_CARY,        0, A_NEUTRAL, PM_TOURIST, NON_PM, 5000L ),

/*** Chaotic artifacts ***/
A("Grimtooth",                  ORCISH_DAGGER,
	SPFX_RESTR, 0, 0,
	PHYS(2,6),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, NON_PM, PM_ORC, 300L ),

A("Deep Freeze",                ATHAME,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(5,6),      COLD(0,0),      NO_CARY,        0, A_CHAOTIC, PM_ICE_MAGE, NON_PM, 400L ),

A("Serpent's Tongue",            DAGGER,
	SPFX_RESTR, 0, 0,
        PHYS(2,0),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_NECROMANCER, NON_PM, 400L ),
	/* See artifact.c for special poison damage */

A("The Marauder's Map", SCR_MAGIC_MAPPING,
	(SPFX_RESTR), 0, 0, NO_ATTK,	NO_DFNS,	NO_CARY,
	OBJECT_DET,	A_CHAOTIC, PM_PIRATE, NON_PM, 2000L ),

A("Cleaver",                    BATTLE_AXE,
	SPFX_RESTR, 0, 0,
	PHYS(3,6),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, PM_BARBARIAN, NON_PM, 1500L ),

A("Doomblade",                  ORCISH_SHORT_SWORD,
	SPFX_RESTR, 0, 0,
	PHYS(0,10),     NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_HUMAN_WEREWOLF, NON_PM, 1000L ),

/*
 *	Stormbringer only has a 2 because it can drain a level,
 *	providing 8 more.
 */
A("Stormbringer",               RUNESWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_INTEL|SPFX_DRLI), 0, 0,
	DRLI(5,2),	DRLI(0,0),	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),


A("Reaver",			SCIMITAR,
	(SPFX_RESTR|SPFX_INTEL),0,0,
	PHYS(5,8),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, PM_PIRATE, NON_PM, 6000L ),

A("Deathsword",                   TWO_HANDED_SWORD,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_HUMAN,
	PHYS(5,14),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_BARBARIAN, NON_PM, 5000L ),

A("Bat from Hell",                BASEBALL_BAT, /* your HP can't ever be higher than half of the max, unless you're a rogue */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(3,20),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_ROGUE, NON_PM, 5000L ),

A("Elfrist",                    ORCISH_SPEAR,
	SPFX_DFLAG2, 0, M2_ELF,
	PHYS(5,16),     NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_HUMAN_WEREWOLF, PM_ORC, 300L ),

A("Plague", DARK_ELVEN_BOW,	/* KMH */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5,8),        DFNS(AD_DRST),  NO_CARY,        0, A_CHAOTIC, PM_DROW, NON_PM, 6000L ),
	/* Auto-poison code in dothrow.c */

A("Mumakbane",			LONG_SWORD,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_QUADRUPED,
	FIRE(5,60),	NO_DFNS,	NO_CARY,
	0, 		A_NEUTRAL,	NON_PM, NON_PM, 500L ),

A("Wormbiter",			CRYSKNIFE,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_WORM,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	0,		A_NEUTRAL,	NON_PM, NON_PM, 2000L ),

A("Shocker",			ELECTRIC_SWORD,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(3,0),	NO_DFNS,	NO_CARY,
	0,		A_NEUTRAL,	NON_PM, NON_PM, 1000L ),

A("The Burned Moth Relay",	RELAY,
	(SPFX_RESTR), SPFX_PROTEC, 0, 	
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,		A_NEUTRAL,	NON_PM, NON_PM, 200L ),

A("Hellfire", CROSSBOW,	/* WAC */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5,8),        FIRE(0,0),  NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),
	/* Auto-explode code in dothrow.c, uhitm.c */

A("Houchou",                SPOON,
	(SPFX_RESTR), 0, 0,
        NO_ATTK,      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 50000L ),

/* KMH -- made it a bag of holding */
A("Wallet of Perseus",       BAG_OF_HOLDING,
        (SPFX_RESTR), 0, 0,
        NO_ATTK,        NO_DFNS,        NO_CARY,
        0,      A_NONE, NON_PM, NON_PM, 10000L ),

/* Amy artifacts begin here */

A("Sea Gull",				DAGGER,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(2,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Jungle Guard",				SURVIVAL_KNIFE,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_REGEN), 0, 0,
	ACID(1,10), DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Dark Moon Rising",			OBSIDIAN_AXE,
	(SPFX_RESTR|SPFX_WARN|SPFX_ATTK), 0, 0,
	PHYS(4,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Digging Dog",			DWARVISH_MATTOCK,
	(SPFX_RESTR|SPFX_SEEK), 0, 0,
	PHYS(2,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("World's Largest Cock",		VIBROBLADE,
	(SPFX_RESTR|SPFX_ESP|SPFX_ATTK|SPFX_DCLAS), 0, S_HUMAN,
	PHYS(20, 20), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2500L ),

A("Stormbringer's Little Brother",			BLACK_AESTIVALIS,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_DRLI), 0, 0,
	DRLI(1,2), DRLI(0,0), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Thorn Rose",			WHITE_FLOWER_SWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(5,2), FIRE(0,0), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Bluewrath",			CRYSTAL_SWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(5,2), COLD(0,0), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Kamehamehadoken",			SUGUHANOKEN,
	(SPFX_RESTR), 0, 0,
	PHYS(8,16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Electrifier",			GREAT_HOUCHOU,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ELEC(5,2), ELEC(0,0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

/* intentional misspelling --Amy */
A("Double Bestard",			BASTERD_SWORD, /* -d20 to-hit, slows you down twice as much as spirit */
	(SPFX_RESTR|SPFX_EVIL|SPFX_SPEAK), 0, 0,
	PHYS(0,20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Guardian of Aranoch",			BENT_SABLE, /* hunger, disables cold resistance for 1000 turns, hostile monsters have 20% chance to move at double speed for a given turn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0,20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Dullswandir",			IRON_SABER,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Goldswandir",		GOLDEN_SABER,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5,10),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Sounding Iron",			AKLYS,
	(SPFX_RESTR|SPFX_SPEAK), 0, 0,
	PHYS(2,6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Firmnail",			SPIKED_CLUB,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK|SPFX_LUCK), 0, 0,
	FIRE(1,8), FIRE(0,0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1600L ),

A("Superclean Deseamer",			BROOM,
	(SPFX_RESTR|SPFX_STLTH|SPFX_PROTEC), 0, 0,
	PHYS(10,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Soothing Fan",			CARDBOARD_FAN,
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("Generic Japanese Melee Weapon",			OTAMA,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Thwack-Whacker",                  FLANGED_MACE,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_UNDEAD,
	PHYS(10,48),     NO_DFNS,        NO_CARY,        0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Evening Star",			MORNING_STAR,
	(SPFX_RESTR|SPFX_DCLAS|SPFX_ESP), 0, S_LIGHT,
	PHYS(10,60), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Flogging Rhythm",			JAGGED_STAR,
	(SPFX_RESTR), 0, 0,
	PHYS(4,16), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Modified Pocket Calculator",			FLAIL,
	(SPFX_RESTR|SPFX_SPEAK), 0, 0,
	PHYS(24,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 240L ),

A("Bitchwhipper",			KNOUT,
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_WARN), 0, M2_FEMALE,
	PHYS(5,24), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Orc Magic",			ORCISH_RING_MAIL,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Anti-Disenchanter",			DISENCHANTING_BOOTS,
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Hot and Cold",			RIN_FIRE_RESISTANCE,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Glorious Dead",			CORPSE,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_ONLYONE), (SPFX_DEFN|SPFX_REFLECT), 0,
	NO_ATTK, NO_DFNS, CARY(AD_MAGM), 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Precious Wish",			SCR_WISHING,
	(SPFX_RESTR|SPFX_NOWISH), (SPFX_DEFN), 0,
	NO_ATTK, NO_DFNS, CARY(AD_MAGM), 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Trolled by the RNG",			ROCK,
	(SPFX_RESTR|SPFX_ONLYONE), 0, 0,
	PHYS(5,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Starcraft Flail",			OBSID,
	(SPFX_RESTR|SPFX_HSPDAM), 0, 0,
	PHYS(1,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6500L ),

A("Pwnhammer",			MALLET, /* grayout */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	COLD(5,16), DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Pwnhammer Duece",			SLEDGE_HAMMER, /* +10 difficulty, can't see lit areas */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_REGEN|SPFX_EVIL), 0, 0,
	FIRE(8,24), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Dockside Walk",			WEDGED_LITTLE_GIRL_SANDAL,
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	PHYS(2,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Karate Kid",			SOFT_GIRL_SNEAKER, /* free action, monsters with kick attacks have +20 to-hit against you */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5,16), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 10000L ),

A("Girlful Bonking",			STURDY_PLATEAU_BOOT_FOR_GIRLS, /* aggravate monster, diarrhea, claw attacks deal extra damage to you, -500 carry capacity, all skills behave as if they were unskilled, gushlush effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(20,30), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Armor-Piercing Hug",			HUGGING_BOOT,
	(SPFX_RESTR|SPFX_DEFN|SPFX_PROTEC), 0, 0,
	PHYS(50,2), DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Asian Winter",			HUGGING_BOOT, /* aggravate monster */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_EVIL), 0, 0,
	COLD(4,18), DFNS(AD_FIRE), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("French Maria",			BLOCK_HEELED_COMBAT_BOOT,
	(SPFX_RESTR|SPFX_DEFN|SPFX_WARN), 0, 0,
	PHYS(2,12), DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Force India",			WOODEN_GETA,
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	PHYS(20,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Stupidity In Motion",			LACQUERED_DANCING_SHOE,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DRLI|SPFX_REFLECT), 0, 0,
	DRLI(1,2), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 6000L ),

A("Sexy Nurse Sandal",			HIGH_HEELED_SANDAL,
	(SPFX_RESTR|SPFX_REGEN|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2500L ),

A("Tender Beauty",			SEXY_LEATHER_PUMP,
	(SPFX_RESTR), 0, 0,
	PHYS(5,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Massive But Lovely",			SEXY_LEATHER_PUMP, /* wakeup call, gives 10000 turns of wendy trap effect and 1000 turns of fumbling */
	(SPFX_RESTR|SPFX_STLTH|SPFX_EVIL), 0, 0,
	PHYS(6,18), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Sweetheart Pump",			SEXY_LEATHER_PUMP, /* psi resistance */
	(SPFX_RESTR), 0, 0,
	PHYS(15,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Sandra's Evil Minddrill",			SPIKED_BATTLE_BOOT, /* aggravate monster, confusion and can cause amnesia when wielded; wielding it disenchants it as long as it's better than -20 */
	(SPFX_RESTR|SPFX_DEFN|SPFX_SEARCH|SPFX_EVIL), 0, 0,
	PHYS(0,32), DFNS(AD_ELEC), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Ribcracker",			QUARTERSTAFF,
	(SPFX_RESTR), 0, 0,
	PHYS(8,16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Dull Metal",			IRON_BAR, /* 1 in 50 that it dulls */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(1,20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Gnarlwhack",			PLATINUM_FIRE_HOOK,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK|SPFX_HALRES|SPFX_SEARCH), 0, 0,
	FIRE(5,8), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Fire Leader",			PARTISAN, /* disables fire resistance for 1000 turns, +5 spawn freq for fire-resistant monsters */
	(SPFX_RESTR|SPFX_ATTK|SPFX_LUCK|SPFX_EVIL), 0, 0,
	FIRE(9,24), NO_DFNS, NO_CARY, UNTRAP, A_NEUTRAL, NON_PM, NON_PM, 11200L ),

A("Fumata Yari",			SPETUM, /* etherwind and makes the player very fast when wielded */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_EVIL), 0, 0,
	ACID(2,16), DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3400L ),

A("Non-Sucker",			RANSEUR,
	(SPFX_RESTR), 0, 0,
	PHYS(6,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Dimoak's Hew",			BARDICHE, /* blindness resistance */
	(SPFX_RESTR), 0, 0,
	PHYS(0,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Land Knight Piercer",			VOULGE,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	PHYS(2,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Apply B",			HALBERD, /* increases your sanity by one per turn */
	(SPFX_RESTR|SPFX_SPEAK|SPFX_ESP|SPFX_STLTH|SPFX_EVIL), 0, 0,
	PHYS(1,20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7500L ),

A("Cock Application",			BILL_GUISARME, /* saps your maximum health by one with a 1 in 100 chance */
	(SPFX_ATTK|SPFX_RESTR|SPFX_PROTEC|SPFX_DRLI|SPFX_EVIL), 0, 0,
	DRLI(3,16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6500L ),

A("Not A Hammer",			LUCERN_HAMMER, /* while wielding it, your AC cannot be better than 10 */
	(SPFX_RESTR|SPFX_ATTK|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	COLD(4,16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("It's A Polearm",			LUCERN_HAMMER, /* aids when wielded */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_EVIL), 0, 0,
	ELEC(4,16), DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Bec De Ascorbin",			BEC_DE_CORBIN,
	(SPFX_RESTR), 0, 0,
	PHYS(1,4), NO_DFNS, NO_CARY, HEALING, A_LAWFUL, NON_PM, NON_PM, 3000L ),

A("Paleolithic Relic",			FLINT_SPEAR,
	(SPFX_RESTR), 0, 0,
	PHYS(2,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Bronze Age Relic",			BRONZE_SPEAR,
	(SPFX_RESTR), 0, 0,
	PHYS(2,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Misguided Missile",			SPIRIT_THROWER, /* while wielding it, you occasionally move in a random direction */
	(SPFX_RESTR|SPFX_TCTRL|SPFX_EVIL), 0, 0,
	PHYS(0,16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Mare's Special Rocket",			TORPEDO, /* slows you down twice as much as spirit when wielded */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	COLD(8,16), DFNS(AD_COLD), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 9000L ),

A("Lightning Blade",			TRIDENT,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(2,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Fishing Grandpa",			STYGIAN_PIKE, /* warns of semicolons */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_EEL,
	PHYS(20,40), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Static Stick",			MARE_TRIDENT,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ELEC(4,14), DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("The People-Eating Trident",			MANCATCHER, /* sight bonus, warns of humans */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_HUMAN,
	PHYS(8,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 9000L ),

A("Madeline's Guardian",			PARASOL, /* Pokemon Emerald */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Penguin's Thrusting Sword",		UMBRELLA, /* It sure looks like a sword to me! --Amy also, conveys flying */
	(SPFX_RESTR), 0, 0,
	PHYS(12,18), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 500L ),

A("Lackware",			ORCISH_BOW,
	(SPFX_RESTR), 0, 0,
	PHYS(1,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Wild Hunt",			HYDRA_BOW,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_SEARCH|SPFX_PROTEC), 0, 0,
	FIRE(2,16), DFNS(AD_FIRE), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 20000L ),

A("Buffy Ammo",			SILVER_ARROW,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_DEMON,
	PHYS(10,40), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Heavy Hitter Arrow",			YA,
	(SPFX_RESTR), 0, 0,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 100L ),

A("Agora",			CATAPULT,
	(SPFX_RESTR), 0, 0,
	PHYS(0,14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Upgraded Lemure",			CATAPULT, /* verisiert effect when wielded, and +100 turns of verisiert effect intrinsic for every ammo you fire */
	(SPFX_RESTR|SPFX_SEEK|SPFX_EVIL), 0, 0,
	PHYS(5,18), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Walther PPK",			PISTOL,
	(SPFX_RESTR), 0, 0,
	PHYS(2,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Desert Eagle",			PISTOL, /* -1 multishot */
	(SPFX_RESTR), 0, 0,
	PHYS(5,18), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Ingram Mac-10",			SUBMACHINE_GUN, /* makes the player very fast when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("FN M249 Para",			HEAVY_MACHINE_GUN, /* aggravate monster, -d15 to-hit */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0,16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Surefire Gun",			RIFLE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Mosin-Nagant",			SNIPER_RIFLE, /* paralyzes you for 2 turns after each shot, -3 multishot */
	(SPFX_RESTR|SPFX_SEEK), 0, 0,
	PHYS(20,30), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Leone M3 Guage Super",			SHOTGUN, /* paralyzes you for a turn after each shot, -2 multishot */
	(SPFX_RESTR), 0, 0,
	PHYS(0,40), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1600L ),

A("Citykiller Combat Shotgun",			AUTO_SHOTGUN, /* -d10 to-hit */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	PHYS(0,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3600L ),

A("Smugglers End",			HAND_BLASTER,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(10,2), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Colonel Bastard's Laser Pistol",			ARM_BLASTER, /* -d5 to-hit */
	(SPFX_RESTR), 0, 0,
	PHYS(10,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Cookie Cutter",			CUTTING_LASER,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Doomguy's Wet Dream",			BFG,
	(SPFX_RESTR), 0, 0,
	PHYS(2,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 9000L ),

A("Grand Daddy",			ROCKET_LAUNCHER,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(20,20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Extra Firepower",			PISTOL_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(2,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Demon Machine",			DEMON_CROSSBOW, /* infravision when wielded */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(6,12), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("ICBM",			HELO_CROSSBOW,
	(SPFX_RESTR), 0, 0,
	PHYS(20,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 9900L ),

A("Bearkiller",			CROSSBOW_BOLT,
	(SPFX_RESTR|SPFX_DFLAG1), 0, M1_THICK_HIDE,
	PHYS(5,50), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Puncture Missile",			DART,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_HUMAN,
	PHYS(8,40), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Instant Death",			DART_OF_DISINTEGRATION,
	(SPFX_RESTR), 0, 0,
	PHYS(0,100), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Needle-like the new log",			SHURIKEN,
	(SPFX_RESTR), 0, 0,
	PHYS(7,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 70L ),

A("Catwomanbane",			BATARANG,
	(SPFX_RESTR|SPFX_DCLAS|SPFX_SPEAK), 0, S_FELINE,
	PHYS(10,80), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Doctor Jones' Aid",			BULLWHIP, /* warns of snakes */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_SNAKE,
	PHYS(5,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Cruel Punisher",			STEEL_WHIP, /* grants aggravate monster */
	(SPFX_RESTR|SPFX_EREGEN|SPFX_EVIL), 0, 0,
	PHYS(2,12), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Bristly String",			ROSE_WHIP, /* monsters take 1d4 damage if they melee you */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(3,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Police Brutality",			RUBBER_HOSE,
	(SPFX_RESTR), 0, 0,
	PHYS(4,14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Demonstrants Go Home",			RUBBER_HOSE, /* warns of humans */
	(SPFX_RESTR|SPFX_DCLAS|SPFX_DEFN), 0, S_HUMAN,
	PHYS(6,16), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("We Are Not Oppressive",			RUBBER_HOSE,
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	PHYS(8,16), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

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

A("Peace Advocate",			RUFFLED_SHIRT, /* free action while worn, warns of humans */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_HUMAN,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Gentle-Soft Clothing",			VICTORIAN_UNDERWEAR, /* poison and disintegration resistance while worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Helen's Discarded Shirt",			VICTORIAN_UNDERWEAR, /* aggravate monster while worn */
	(SPFX_RESTR|SPFX_TCTRL|SPFX_LUCK|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Antimagic Shell",			VICTORIAN_UNDERWEAR, /* prevents spellcasting */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
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
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN|SPFX_EVIL), 0, 0,
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
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Grayscale Wanderer",			LEATHER_ARMOR, /* shades of grey effect */
	(SPFX_RESTR|SPFX_DEFN|SPFX_WARN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("CD Rome Arena",			ELVEN_TOGA, /* slows the player down */
	(SPFX_RESTR|SPFX_REGEN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Chastity Armor",			CONSORT_S_SUIT, /* prevents you from contracting STD */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Laura Croft's Battlewear",			LEATHER_JACKET, /* like d-type equipment, prevents lava from harming player */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Offense Owns Defense",			ROBE, /* double attacks */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Protection With A Price",			ROBE_OF_WEAKNESS, /* stun resistance and 5 extra points of AC */
	(SPFX_RESTR|SPFX_HALRES|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Cute Idea",			ORANGE_DRAGON_SCALE_MAIL,
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("All Hail The Mighty RNG",			PURPLE_DRAGON_SCALE_MAIL,
	(SPFX_RESTR|SPFX_LUCK|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 10000L ),

A("Ho-Oh's Feathers",			SKY_DRAGON_SCALE_MAIL, /* aggravate monster and conflict */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 20000L ),

A("Upgrade This",			MERCURIAL_DRAGON_SCALES,
	(SPFX_RESTR|SPFX_SEEK|SPFX_SPEAK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Don Suicune Does Not Approve",			PLATINUM_DRAGON_SCALES, /* aggravate monster and random nasty effects */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN|SPFX_EVIL), 0, 0,
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
	(SPFX_RESTR|SPFX_SPEAK|SPFX_SEEK|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Superman's Super Suit",			SUPER_MANTLE, /* quad attacks */
	(SPFX_RESTR|SPFX_REGEN|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Full Wings",			WINGS_OF_ANGEL, /* flying */
	(SPFX_RESTR|SPFX_HALRES|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Broken Wings",			DUMMY_WINGS, /* disables the flying intrinsic, aggravates monsters, and causes intrinsic freezing; autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5L ),

A("Acquired Poison Resistance",			POISONOUS_CLOAK, /* poison resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("It's A Wonderful Failure",			CLOAK_OF_DEATH,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Rita's Deceptive Mantle",			CLOAK_OF_FUMBLING, /* autocurses and sets its enchantment to -10 when worn! */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Stunted Helper",			MANACLOAK,
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Insufficient Protection",			CLOAK_OF_MATADOR,
	(SPFX_RESTR|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Memory Aid",			CLOAK_OF_UNSPELLING, /* keen memory */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Frequent But Weak Status",			HEAVY_STATUS_CLOAK, /* resist confusion and stun */
	(SPFX_RESTR|SPFX_PROTEC|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("A Reason To Live",			YELLOW_SPELL_CLOAK, /* deactivates teleport control */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Full Moon Tonight",			VULNERABILITY_CLOAK,
	(SPFX_RESTR|SPFX_LUCK|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("All-In-One Ascension Kit",			CLOAK_OF_INVENTORYLESSNESS, /* also magic and drain resistance */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Radar Cloak",			CLOAK_OF_RESPAWNING,
	(SPFX_RESTR|SPFX_ESP|SPFX_WARN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Hostes Ad Pulverem Ferire",			SPAWN_CLOAK, /* increase STR/DEX by enchantment value +5 */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Uberjackal Effect",			ADOM_CLOAK, /* aggravate monster */
	(SPFX_RESTR|SPFX_SPEAK|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Variatio Delectat",			EGOIST_CLOAK, /* does nothing special - just meant to annoy lawful players who hoped for a good sacrifice gift */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Speedrunner's Dream",			CLOAK_OF_TIME, /* makes the player very fast */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Can't Kill What You Can't See",			EERIE_CLOAK, /* see invis and makes you invisible */
	(SPFX_RESTR|SPFX_HALRES|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Image Projector",			CLOAK_OF_NAKEDNESS, /* displacement */
	(SPFX_RESTR|SPFX_TCTRL|SPFX_HSPDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Silent Noise",			CLOAK_OF_AGGRAVATION,
	(SPFX_RESTR|SPFX_STLTH|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Dark Angels",			CLOAK_OF_CONFLICT, /* flying */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2500L ),

A("Skills Beat Stats",			CLOAK_OF_STAT_LOCK, /* faster skill training */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Baron Von Richthofen's Premiere",			WING_CLOAK,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 16000L ),

A("RNG's Gamble",			CLOAK_OF_PREMATURE_DEATH,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Fightright",			GNOMISH_HELM,
	(SPFX_RESTR|SPFX_SEEK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 200L ),

A("Vitamin B",			HELM_OF_LOSE_IDENTIFICATION,
	(SPFX_RESTR|SPFX_ESP|SPFX_EVIL), 0, 0,
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
	(SPFX_RESTR|SPFX_WARN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 250L ),

A("Dark Nadir",			STANDARD_HELMET, /* autocurses, creates darkness once every 100 turns, but improves your to-hit by 5 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Loving Deity",			HELM_OF_OPPOSITE_ALIGNMENT,
	(SPFX_RESTR|SPFX_PROTEC|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("COW Enchantment",			HELM_OF_DETECT_MONSTERS, /* 9 extra points of AC */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 900L ),

A("Thor's Mythical Helmet",			HELM_OF_STORMS,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_HSPDAM|SPFX_EVIL), 0, 0,
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
	(SPFX_RESTR|SPFX_HALRES|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Omniscient",			HELMET_OF_UNDEAD_WARNING,
	(SPFX_RESTR|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("Spacewarp",			HELM_OF_TELEPORTATION,
	(SPFX_RESTR|SPFX_TCTRL|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Dictatorship",			HELM_OF_TELEPORT_CONTROL, /* also gives polymorph control */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Safe Insecurity",			HELM_OF_OBSCURED_DISPLAY,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_HSPDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("You Are Already Dead",			BLACKY_HELMET, /* also magic and drain resistance */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 20000L ),

A("Shpx Guvf Fuvg",			CYPHER_HELM, /* also unbreathing */
	(SPFX_RESTR|SPFX_SEEK|SPFX_STLTH|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Go Other Place",			HELM_OF_BAD_ALIGNMENT, /* teleportitis */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Beeswax Helmet",			SOUNDPROOF_HELMET,
	(SPFX_RESTR|SPFX_LUCK|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Low Local Memory",			OUT_OF_MEMORY_HELMET, /* free action and drain resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Severe Aggravation",			ANGER_HELM, /* aggravate monster */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Formfiller",			CAPTCHA_HELM, /* heavily curses itself when worn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Stone Erosion",			HELM_OF_AMNESIA, /* petrification resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Blinding Fog",			DIMMER_HELMET, /* blindness resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Big Bonnet",			WAR_HAT, /* poison resistance */
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Eulogy's Eulogy",			BOOGEYMAN_HELMET,
	(SPFX_RESTR|SPFX_PROTEC|SPFX_STLTH|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Mind Shielding",			TINFOIL_HELMET, /* stun resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1600L ),

A("Conspiracy Theory",			PARANOIA_HELMET,
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2300L ),

A("Box Fist",			REGULAR_GLOVES, /* +5 unarmed damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Swing Less Cast More",			GAUNTLETS_OF_PANIC,
	(SPFX_RESTR|SPFX_EREGEN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Molass Tank",			GAUNTLETS_OF_SLOWING, /* 10 extra points of AC */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Science Fliction",			OILSKIN_GLOVES,
	(SPFX_RESTR|SPFX_REGEN|SPFX_EREGEN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("AFK Means Ass-Fucker",			GAUNTLETS_OF_TYPING, /* autocurses, monsters with AD_SEDU or similar attacks have +12 speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
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
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Come Back To Life",			MENU_NOSE_GLOVES, /* 50% chance of life saving while wearing them */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("ScrollScrollScroll",			CONFUSING_GLOVES, /* increased drop rate of standard id and similar scrolls */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Sealed Knowledge",			GAUNTLETS_OF_MISSING_INFORMATI,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_REFLECT|SPFX_ESP|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Fifty Shades Of Fucked Up",			SADO_MASO_GLOVES, /* +5 damage unarmed, +10 damage while wielding a whip */
	(SPFX_RESTR|SPFX_PROTEC|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Arabella's Bank of Crossroads",			BANKING_GLOVES, /* levelteleportitis, heavily curse themselves when worn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Out Of Control",			CHAOS_GLOVES, /* give teleportitis and disable tele control */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
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
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 6000L ),

A("Burning Disk",			FLAME_SHIELD, /* causes burn, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("The Type Of Ice Block Hates You",			ICE_SHIELD, /* causes freezing, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Numbed Can't Do",			LIGHTNING_SHIELD, /* causes numbness, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Venomat",			VENOM_SHIELD, /* causes stun, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
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
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
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
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 12000L ),

A("Golden Dawn",			EMERALD_DRAGON_SCALE_SHIELD, /* autocurses, causes the "verisiert" effect and gives keen memory */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 12000L ),

A("Real Psychos Wear Purple",			PSYCHIC_DRAGON_SCALE_SHIELD, /* autocurses, grants psi resistance and the hate trap and farlook bug effects */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 12000L ),

A("Binder Crash",			PLASTEEL_BOOTS, /* 1 out of 20 games, putting them on transforms you into a Binder permanently. Otherwise, they heavily curse themselves and increase your deity's anger by three. No ill effect if you actually are a Binder, but for them they count as high heels and don't disable free action. */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Mephisto's Brogues",			LOW_BOOTS, /* autocurses, resist cold/poison, flying, disables fire resistance */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Gnomish Boobs",			GNOMISH_BOOTS, /* +3 charisma */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 100L ),

A("Kokyo no pafomansuu-oku",			ATSUZOKO_BOOTS, /* increases charisma by +5 + enchantment */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_HALRES|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Little Girl's Revenge",			RUBBER_BOOTS,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("AmyBSOD's Vampiric Sneakers",			SNEAKERS, /* cause blood loss */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Cinderella's Slippers",			CRYSTAL_BOOTS, /* aggravate monster and wounded legs, because your feet are bigger than the owner's :-P */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0, /* actually prism reflection */
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Eveline's Lovelies",			WEDGE_SANDALS, /* +5 kicking damage, +3 charisma */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Natalia's Punisher",			WEDGE_SANDALS, /* +8 damage with hammer-class weapons, petrification resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Ella's Bloodlust",			WEDGE_SANDALS, /* double attacks, autocurses, aggravate monster */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Anastasia's Gentleness",			DANCING_SHOES, /* free action, reduces your STR by 10 */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Katrin's Paralysis",			DANCING_SHOES, /* kicking a monster causes it to be stuck to you, resist elec and cold */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Eva's Inconspicuous Charm",			SWEET_MOCASSINS, /* increase CHA and DEX by enchantment value */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Solvejg's Stinking Slippers",			SWEET_MOCASSINS, /* heavily curses itself, aggravates monsters, resist elec, manaleech, +5 CHA, -5 INT and WIS */
	(SPFX_RESTR|SPFX_EREGEN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 7500L ),

A("Jessica's Tenderness",			SWEET_MOCASSINS, /* resist cold, sleep and psi */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 8000L ),

A("Leather Pumps of Horror",			FEMININE_PUMPS, /* poison resistance, increase CHA by 5 + enchantment value */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Lilac Beauty",			FEMININE_PUMPS, /* +10 CHA, -5 to all other stats, taking them off drains an experience level (ignoring resistance) */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN|SPFX_STLTH|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Rhea's Combat Pumps",			FEMININE_PUMPS, /* resist poison/sickness, d10 passive damage to non-poison-resistant monsters with 2% chance of instakill */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 8000L ),

A("Mandy's Rough Beauty",			LEATHER_PEEP_TOES, /* +10 kick damage, free action, increase CHA by 5 + enchantment */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 11000L ),

A("I'm A Bitch, Deal With It",			HIPPIE_HEELS, /* aggravate monster, +5 AC, becomes +0 if the enchantment is negative */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Manuela's Torture Heels",			HIPPIE_HEELS, /* aggravate monster, conflict, heavily curse themselves when worn */
	(SPFX_RESTR|SPFX_DEFN|SPFX_ESP|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Beautiful Topmodel",			HIPPIE_HEELS, /* +25 CHA */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 16000L ),

A("Corina's Unfair Scratcher",			COMBAT_STILETTOS, /* flying, resist cold, shock and petrification */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Sporked",			BOOTS_OF_MOLASSES,
	(SPFX_RESTR|SPFX_REGEN|SPFX_ESP|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Hermes' Unfairness",			FLYING_BOOTS, /* aggravate monster, unbreathing, monsters are always spawned hostile */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Yet Another Stupid Idea",			ZIPPER_BOOTS, /* resist fire and petrification */
	(SPFX_RESTR|SPFX_LUCK|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Hot Flame",			FIRE_BOOTS,
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Jesus Footwear",			BOOTS_OF_SWIMMING, /* prevents your inventory from getting wet */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7500L ),

A("Cursing Anomaly",			ANTI_CURSE_BOOTS, /* autocurses :-P, causes random itemcursing */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_SEEK|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Fun All In One",			MULTI_SHOES, /* conflict, unbreathing, aggravate monster */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Lollerskates",			ROLLER_BLADE,
	(SPFX_RESTR|SPFX_DEFN|SPFX_TCTRL|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2400L ),

A("Desertwalk",			FREEZING_BOOTS,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Waiting Timeout",			AUTODESTRUCT_DE_VICE_BOOTS, /* free action */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Nose Enchantment",			SPEEDBUG_BOOTS, /* polymorphitis */
	(SPFX_RESTR|SPFX_DEFN|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Fantastic Shoes",			SENTIENT_HIGH_HEELED_SHOES,
	(SPFX_RESTR|SPFX_SPEAK|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Untrained Half Marathon",			BOOTS_OF_FAINTING, /* wounded legs, very fast speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Black Diamond Icon",			DIFFICULT_BOOTS, /* multiply monster spawn rate by 4 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Riddle Me This",			GRIDBUG_CONDUCT_BOOTS, /* causes quizzes */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Base for Speed Ascension",			LIMITATION_BOOTS, /* sickness resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Paranoia Stride",			DEMENTIA_BOOTS, /* resist fear */
	(SPFX_RESTR|SPFX_HALRES|SPFX_STLTH|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Ding Dong Ping Pong",			RIN_DISARMING,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_TCTRL|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2500L ),

A("Ring of Woe",			RIN_DOOM, /* prime curses itself, aggravate monster, hunger and conflict */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Werefoo Go Home",			RIN_PROTECTION_FROM_SHAPE_CHAN,
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Secret Detective",			RIN_STEALTH,
	(SPFX_RESTR|SPFX_ESP|SPFX_SEEK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7500L ),

A("Magic Signet",			RIN_SUSTAIN_ABILITY,
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Eat More Kittens",			RIN_AGGRAVATE_MONSTER, /* autocurses */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
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
	(SPFX_RESTR|SPFX_ESP|SPFX_LUCK|SPFX_TCTRL|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Crylock",			RIN_BLOOD_LOSS, /* heavily curses itself, improves AC by 10 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("GoldenIvy's Rage",			RIN_SUPERSCROLLING, /* aggravate monster, teleportitis, sickness resistance, flying */
	(SPFX_RESTR|SPFX_TCTRL|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Team Nastytrap's Bauble",			RIN_ENSNARING,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Float Eyeler's Condition",			RIN_DIARRHEA, /* levitation */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Skill-less of the service",			RIN_NO_SKILL,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Fatally Low",			RIN_LOW_STATS,
	(SPFX_RESTR|SPFX_ESP|SPFX_SEEK|SPFX_LUCK|SPFX_HSPDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Criminal Queen",			RIN_DANGER, /* +5 charisma, very fast speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
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
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Precious Unobtainable Properties",			AMULET_OF_STRANGULATION, /* warp reflection, resist psi/stun/petrification/sickness */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Ballsy Bastard",			AMULET_OF_PREMATURE_DEATH, /* free action, resist petrification, manaleech */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Fix Everything",			AMULET_OF_UNCHANGING, /* sustain ability */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Only One Escape",			AMULET_OF_ANTI_TELEPORTATION, /* jumping */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
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
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Recovered Relic",			AMULET_OF_MENTAL_STABILITY, /* stun res */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Tyranitar's Quest",			AMULET_OF_PRISM, /* techniques get no timeout 50% of the time */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Space Cycle",			AMULET_OF_WARP_DIMENSION, /* teleportitis and polymorphitis */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Necklace of Adornment",			NECKLACE, /* +10 charisma */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Good Bee",			AMULET_OF_RMB_LOSS, /* sight bonus, poison resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Where Did It Go",			AMULET_OF_ITEM_TELEPORTATION,
	(SPFX_RESTR|SPFX_TCTRL|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Confusticator",			AMULET_OF_DIRECTIONAL_SWAP, /* confusion */
	(SPFX_RESTR|SPFX_SEEK|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Eeh-Eeh",			AMULET_OF_SUDDEN_CURSE,
	(SPFX_RESTR|SPFX_EREGEN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("The 10th Nazgul",			AMULET_OF_ANTI_EXPERIENCE, /* heavily curses itself, disables drain resistance, gives free action and manaleech */
	(SPFX_RESTR|SPFX_REGEN|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Very Tricky Indeed",			BAG_OF_TRICKS, /* spawns more monsters than usual */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("One Size Fits Everything",			ICE_BOX_OF_HOLDING, /* like Wallet of Perseus */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Stonesplitter",			PICK_AXE,
	(SPFX_RESTR|SPFX_SEEK), 0, 0,
	PHYS(2,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Darkening Thing",			UNICORN_HORN, /* applying it aggravates monsters */
	(SPFX_RESTR), 0, 0,
	PHYS(3,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Roommate's Special Idea",			LASER_SWATTER,
	(SPFX_RESTR|SPFX_BEHEAD|SPFX_DCLAS), 0, S_XAN,
	PHYS(1,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

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
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("The Answer Is 42",			TOWEL, /* applying it, or putting it on, causes a random bad effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Penis Safety",			CONDOME,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Knight's Aid",			LEATHER_SADDLE, /* artifact saddles exist just so that deities can sometimes unrestrict your riding skill */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Vroom Vroom",			LEATHER_SADDLE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Motorcycle Race",			LEATHER_SADDLE,
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
	(SPFX_RESTR|SPFX_ATTK|SPFX_DRLI|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("And Your Mortal World Shall Perish In Flames",			SPE_CURE_BURN,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK), 0, 0,
	FIRE(5,10), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

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
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK|SPFX_DRLI), 0, 0,
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
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Faeaeaeaeaeau",			WAN_FIRE,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Digger Gem",			EMERALD,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ACID(5,16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Khor's Curse",			LUCKSTONE, /* autocurses and sets itself to -5 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Strange Protector",			ACID_VENOM,
	(SPFX_RESTR), (SPFX_PROTEC), 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Roadrash Weapon",			IRON_CHAIN,
	(SPFX_RESTR), 0, 0,
	PHYS(0,16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Vuvuzela",			TOOLED_HORN, /* aggravates monsters when applied (idea by yasdorian) */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

/* The following artifacts are from SLASHTHEM. Many of these had to be redesigned, since all of them can now be generated
 * randomly. And it takes one overpowered artifact to destroy game balance - as soon as players become aware of its
 * existence, everyone will wish for it as soon as possible. Also, none of these are associated with any role now. --Amy
 * Soviet's comments are preserved just for the sake of it; his "temp names" are of course permanent in this variant. */

A("Clarent",			SHORT_SWORD,
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_LUCK|SPFX_DFLAG1), 0, M1_THICK_HIDE,
	PHYS(8,2),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Shadowblade",			KNIFE,
	(SPFX_RESTR|SPFX_STLTH),0,0,
	PHYS(5,6),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Yoichi no yumi",                YUMI, /* 5lo: Artifact from dNethack */
	SPFX_RESTR, 0, 0,
	PHYS(20,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 1200L ),

A("Kiku-ichimonji",                KATANA, /* 5lo: New sacrifice gift for Samurai */
	SPFX_RESTR, 0, 0,
	PHYS(4,12),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 1200L ),

A("Itlachiayaque",             SHIELD_OF_REFLECTION,
	(SPFX_RESTR|SPFX_HSPDAM|SPFX_DEFN), 0, 0,
	NO_ATTK,        DFNS(AD_FIRE),        NO_CARY,  0,    A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Queen's Guard",		RAPIER,
	(SPFX_RESTR), 0, 0,
	PHYS(6,6),	NO_DFNS,	NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1500L ),

A("Peacekeeper",		PISTOL,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(4,8),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 1500L ),

A("Restkeeper",		CLUB,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(4,8),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 1500L ),

A("Icebiter",			AXE,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(6,6),	COLD(0,0),	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 1500L ),

A("Shock Brand",                LONG_SWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ELEC(5,0),	ELEC(0,0),	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Acid Brand",                 LONG_SWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ACID(5,0),	ACID(0,0),	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),

/* 5lo: Generic artifact since Psions are deferred for now */
A("Snakeskin",             ROBE,
    (SPFX_RESTR|SPFX_HALRES|SPFX_PROTEC), 0, 0,
    NO_ATTK,    ACID(0,0),   NO_CARY,    0, A_NEUTRAL, NON_PM, NON_PM, 700L ),

A("Poseidon's Treasure",	TRIDENT,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(10, 10),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Gladius",		SHORT_SWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(8,6),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),

A("Hrunting",		LONG_SWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(4,4),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Debugger",       ATHAME,
    (SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
    ELEC(5,4),  ELEC(0,0), NO_CARY,     0, A_NEUTRAL, NON_PM, NON_PM, 400L ),
    
A("Acidtester",       ATHAME, /* temp name */
    (SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
    ACID(5,4),  ACID(0,0), NO_CARY,     0, A_NEUTRAL, NON_PM, NON_PM, 400L ),

A("Straightshot",   BOW,
    SPFX_RESTR, 0, 0,
    PHYS(10,2),     NO_DFNS,    NO_CARY,    0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),
    
A("Shimmerstaff",   SILVER_KHAKKHARA,
    SPFX_RESTR, 0, 0,
    PHYS(8,4),     NO_DFNS,    NO_CARY,    0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Full Metal Jacket",   LEATHER_JACKET, /* Temp name */
    (SPFX_RESTR|SPFX_DEFN|SPFX_PROTEC|SPFX_HALRES), 0, 0,
    NO_ATTK,    FIRE(0,0),    NO_CARY,   0, A_NEUTRAL, NON_PM, NON_PM, 1500L ), 
    
A("Tesla's Coat", LAB_COAT,
    (SPFX_RESTR|SPFX_DEFN|SPFX_HPHDAM), 0, 0,
    NO_ATTK,    ELEC(0,0),  NO_CARY,    0,  A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("Oathkeeper", 		STILETTO, /* temp name? */
    (SPFX_RESTR), 0, 0,
    PHYS(7,8),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),

A("Blackshroud",		CLOAK_OF_PROTECTION,
    (SPFX_RESTR|SPFX_LUCK|SPFX_WARN),	0,	0,
    NO_ATTK,	DRLI(0,0),	NO_CARY,	0,	A_NEUTRAL,	NON_PM,	NON_PM, 1500L ),

A("Silvershard", SILVER_DAGGER,
    (SPFX_RESTR), 0, 0,
    PHYS(2,0),	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Mirror Brand",                 SILVER_LONG_SWORD, /* dNethack */
	(SPFX_RESTR|SPFX_ATTK|SPFX_REFLECT|SPFX_DALIGN), 0, 0,
	STUN(5,0),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 3500L ),

/* From Nethack--, This is pretty much a temp for Musicians
 * Until I can think of something better for them.	
 * Amy edit: that comment is from Soviet the Type Of Ice Block of course :D */
A("Dirk", 		DAGGER, 
	SPFX_RESTR, 	0, 	0,
	PHYS(5,4),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),

A("Tenderizer",                   MALLET,           
	(SPFX_RESTR|SPFX_ATTK|SPFX_EVIL),  0, 0,
	PHYS(3,6),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

/* 5lo: Some more artifacts... */
A("Fungisword", LONG_SWORD,
    (SPFX_RESTR|SPFX_DCLAS|SPFX_HALRES), 0, S_FUNGUS,
    PHYS(10,0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 400L ),
    
A("Diplomat", GOLDEN_SABER,
    (SPFX_RESTR|SPFX_DFLAG2), 0, M2_PNAME,
    PHYS(10,24), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),
    
A("Petslayer", CLUB,
    (SPFX_RESTR|SPFX_DFLAG2), 0, M2_DOMESTIC,
    PHYS(5,10), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Mouser's Scalpel",			RAPIER,
	(SPFX_RESTR), 0, 0,
	PHYS(5,2),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 600L ),
    
A("Graywand",		TWO_HANDED_SWORD,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	COLD(3,6),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),
    
A("Heartseeker",		SHORT_SWORD,
	(SPFX_RESTR),	0, 0,
	PHYS(3,2),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Cat's Claw",		DAGGER,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_RODENT,
	PHYS(5,6),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Nightingale",                KATANA,
	SPFX_RESTR, 0, 0,
	PHYS(6,2),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),
	/* 5lo: Sacrifice gift for Ninja */

A("Bloodmarker",		KNIFE,
	(SPFX_RESTR), 0, 0,
	PHYS(3,6),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Shawshank",			KNIFE,
	(SPFX_RESTR), 0, 0,
	PHYS(9,8),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Spineseeker",	STILETTO,
	SPFX_RESTR, 0, 0,
	PHYS(5,4),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Crown Royal Cloak",	LEATHER_CLOAK,
	(SPFX_RESTR|SPFX_PROTEC|SPFX_LUCK), 0, 0,
	NO_ATTK,	ACID(0,0),	NO_CARY,
	0,	A_NEUTRAL,	 NON_PM, NON_PM, 1000L),

A("The Gambler's Suit",		COMMANDER_SUIT,
	(SPFX_RESTR|SPFX_PROTEC|SPFX_LUCK), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 1000L ),

A("The Wand of Might",		WAN_WISHING,
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,	A_NONE, NON_PM, NON_PM, 3000L ),

A("Warforger",  HEAVY_HAMMER,   /* giant blacksmithing hammer */
	(SPFX_RESTR|SPFX_ATTK),  0, 0,
	PHYS(15,14),	FIRE(0,0),	NO_CARY, /* phys so fire resistance doesn't negate */
    0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("The Sling of David",                SLING,
	(SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_HPHDAM), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("The Golden Whistle of Nora",          MAGIC_WHISTLE, /* 5lo: temp name? Amy edit: you come up with a really good name ONCE and then decide that it's temporary? Seriously??? */
	(SPFX_RESTR|SPFX_INTEL|SPFX_WARN|SPFX_HPHDAM|SPFX_ESP), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,         A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Fuma-itto no Ken",                BROADSWORD,    /* 5lo: New quest artifact for Ninja - Credit to Chris as this comes from dNethack with minor changes */
	(SPFX_RESTR|SPFX_DALIGN|SPFX_DEFN), 0, 0,
	PHYS(8,8),      DFNS(AD_DRLI),     NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("The Pick of the Grave",		PICK_AXE, /* Amy addition: aggravates monsters and causes hunger when wielded */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DRLI|SPFX_DEFN|SPFX_REGEN|SPFX_HPHDAM|SPFX_TCTRL|SPFX_EVIL), 0, 0,
	DRLI(8,10),	COLD(0,0),   NO_CARY,
	0,		A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("The Flute of Slime",          MAGIC_FLUTE, /* 5lo: temp name */
	(SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_WARN|SPFX_TCTRL), 0, 0,
	NO_ATTK,        DFNS(AD_ELEC),        NO_CARY,
	0,         A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("The Fire Chief Helmet",                FIRE_HELMET, /* Amy edit: weak sight when worn */
	(SPFX_RESTR|SPFX_INTEL|SPFX_WARN|SPFX_PROTEC|SPFX_HSPDAM|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK,      NO_DFNS,        NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("The Deluxe Yendorian Knife", KNIFE,
	(SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_ESP), 0, 0,
	PHYS(6,20),  	FIRE(0,0), 	NO_CARY,
	0,  A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("The Harp of Lightning",          MAGIC_HARP, 
 	(SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_WARN|SPFX_TCTRL), 0, 0,
 	NO_ATTK,        DFNS(AD_ACID),        NO_CARY,
	0,         A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("The Harp of Harmony",          MAGIC_HARP,
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_SPEAK|SPFX_WARN|SPFX_STLTH), 0, 0,
	NO_ATTK,	DFNS(AD_DRLI),	NO_CARY,
	0,		A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("The Cudgel of Cuthbert", QUARTERSTAFF, /* 5lo: from Nethack-- 3.1.3 */
	/* Amy edit: trap of walls and +10 difficulty effect when wielded */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_HALRES|SPFX_REGEN|SPFX_DALIGN|SPFX_WARN|SPFX_HSPDAM|SPFX_EVIL), 0, 0,
	PHYS(5,0),	DFNS(AD_DRLI),	NO_CARY,
	0,		A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("The Sword of Svyatogor", TWO_HANDED_SWORD, /* way to completely ignore the warrior's elder scrolls origin, Soviet! --Amy */
	(SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_HPHDAM), 0, 0,
	PHYS(7,8),	COLD(0,0),	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("The Tommy Gun of Capone", SUBMACHINE_GUN,
	(SPFX_RESTR|SPFX_DEFN|SPFX_LUCK|SPFX_INTEL|SPFX_WARN|SPFX_STLTH), 0, 0,
	PHYS(5,6),        DFNS(AD_FIRE),  NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("The Whistle of the Warden", MAGIC_WHISTLE,
	(SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_ESP|SPFX_TCTRL), 0, 0,
	NO_ATTK,        DFNS(AD_DRLI),  NO_CARY,
	0, 	A_LAWFUL, NON_PM, NON_PM, 3000L ),

A("The Hand Mirror of Cthylla",	MIRROR, /* 5lo: replaces Pen of the Void */
	(SPFX_RESTR|SPFX_INTEL|SPFX_TCTRL|SPFX_SEARCH), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,	A_NONE, NON_PM, NON_PM, 5000L),

A("The Scalpel of the Bloodletter",	SCALPEL, /* bleedout when wielded */
	(SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DRLI|SPFX_HPHDAM|SPFX_REGEN|SPFX_EVIL), 0, 0,
	DRLI(9,10),	NO_DFNS,	NO_CARY,
	0,	A_NEUTRAL, NON_PM, NON_PM, 2500L),

A("The Gourd of Infinity", HORN_OF_PLENTY,
	(SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_ESP|SPFX_HSPDAM|SPFX_HALRES), 0, 0,
	NO_ATTK,    DFNS(AD_DRLI),    NO_CARY,
	0, A_NEUTRAL,  NON_PM,   NON_PM, 2500L),

A("The Lockpick of Arsene Lupin", LOCK_PICK,
	(SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK|SPFX_LUCK|SPFX_SEEK|SPFX_ESP|SPFX_STLTH|SPFX_WARN), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,		A_NEUTRAL, NON_PM, NON_PM, 3500L ),

A("The Staff of Withering",	QUARTERSTAFF,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_INTEL|SPFX_DRLI|SPFX_DALIGN), 0, 0,
	DRLI(3,4),	DFNS(AD_COLD),	NO_CARY,
	0,	A_CHAOTIC, NON_PM, NON_PM, 5000L ),

/* The following artifacts are from dnethack. Like the SLASHTHEM ones, they had to be redesigned to fit into this variant.
 * IMHO dnethack's balance is completely out of whack, so I tried my best to make them all balanced. --Amy
 * Chris's comments are preserved just for the sake of it. */

A("The Bow of Skadi",	BOW, /* slower rate of fire because seriously, +24 damage??? --Amy */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_ATTK), 0, 0,
	COLD(1,24),	DFNS(AD_COLD),	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 4000L), /*Read to learn Cone of Cold (Skadi's Galdr) */

A("The Crown of the Saint King",	STANDARD_HELMET, /*Actually gold circlet*/
	(SPFX_RESTR|SPFX_INTEL), 0, 0, /* Amy: +5 AC and no other effect */
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 4000L), /*Also causes pets to always follow you when worn*/

A("The Helm of the Dark Lord",	STANDARD_HELMET, /*Actually visored helmet*/
	(SPFX_RESTR|SPFX_INTEL), 0, 0, /* Amy: +5 AC and no other effect */
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 4000L), /*Also causes pets to always follow you when worn*/

A("Sunbeam",	GOLDEN_ARROW,
	(SPFX_RESTR|SPFX_INTEL|SPFX_DEFN), 0, 0,
	PHYS(10,0),	DFNS(AD_DRLI),	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 1000L),

A("Moonbeam",	SILVER_ARROW,
	(SPFX_RESTR|SPFX_INTEL|SPFX_DEFN), 0, 0,
	PHYS(10,0),	DFNS(AD_DRLI),	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 1000L),

A("Veil of Latona",	CLOAK_OF_INVISIBILITY, /* drain res, superscroller, black ng walls and permanent confusion, heavily autocurses when worn */
	(SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK,	DFNS(AD_MAGM),	NO_CARY, /* yes this is an example of an artifact that's uber imba in dnethack */
	0, A_NEUTRAL, NON_PM, NON_PM, 1000L),

A("Carnwennan",			DAGGER, /*needs quote*/
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_STLTH|SPFX_WARN), 0, M2_MAGIC,
	PHYS(5,10),	NO_DFNS,		NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 4000L), 

A("The Slave to Armok",			DWARVISH_MATTOCK, /*two handed, so no twoweaponing.*/
	(SPFX_RESTR|SPFX_DFLAG2), 0, (M2_ELF|M2_LORD|M2_PEACEFUL|M2_ORC), /*DF Dwarves can be a nasty lot.*/
	PHYS(5,0),	NO_DFNS,	NO_CARY, /* supposed to be bloodthirsty */
	0, A_LAWFUL, NON_PM, NON_PM, 2500L), 

A("Dragonlance",			LANCE, /* warns of dragons */
	(SPFX_RESTR|SPFX_DCLAS|SPFX_REFLECT), 0, S_DRAGON, /* also makes a handy weapon for knights, since it can't break */
	PHYS(10,20),	NO_DFNS,	NO_CARY,				/* plus, reflection */
	0, A_NONE, NON_PM, NON_PM, 5000L), 

A("Kingslayer",		STILETTO,
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_WARN), 0, (M2_LORD|M2_PRINCE), /* works against just about all the late game badies */
	PHYS(10,20),	NO_DFNS,	NO_CARY,	
	0, A_CHAOTIC, NON_PM, NON_PM, 2500L), 

A("Peace Keeper",		ATHAME, 
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_WARN), 0, (M2_HOSTILE), /* speaks for itself */
	PHYS(5,10),	NO_DFNS,	NO_CARY,	/*Weaker attack and damage */
	0, A_LAWFUL, NON_PM, NON_PM, 2500L), 

A("Rhongomyniad",			LANCE, /*needs quote*/
	(SPFX_RESTR), 0, 0,
	PHYS(3,0),	NO_DFNS,		NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 4000L), 

A("Gilded Sword of Y'ha-Talla",			SCIMITAR,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	STUN(5,0),	DFNS(AD_DRST),	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 3000L), 

A("The Axe of the Dwarvish Lords", BATTLE_AXE, /*can be thrown by dwarves*/
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,	/*x-ray vision is for dwarves only.*/
	PHYS(1, 0),	NO_DFNS,	NO_CARY, /* Amy edit: vision bonus instead of x-ray */
	0, A_LAWFUL, NON_PM, NON_PM, 4000L), //needs quote

A("Windrider",	BOOMERANG, /*returns to your hand.*/
	SPFX_RESTR, 0,0,
	PHYS(1,0),	NO_DFNS,	NO_CARY,//needs quote
	0,	A_NONE, NON_PM, NON_PM, 4000L), 

A("The Rod of the Ram",			MACE, /* Wolf, Ram, and Hart? Ram demon? */
	(SPFX_RESTR), 0, 0,
	PHYS(1,0),	NO_DFNS,	NO_CARY, //needs quote	
	0, A_NEUTRAL, NON_PM, NON_PM, 3000L), 

A("Atma Weapon", 		RED_LIGHTSABER, /*Sword whose attack power is bound to its wielder's life force*/
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_NASTY,
	PHYS(6,6),	NO_DFNS,	NO_CARY, //needs quote
	0, A_NONE, NON_PM, NON_PM, 6660L), 

A("Limited Moon", 		MOON_AXE, /*Axe whose attack power is bound to its wielder's magical energy*/
	(SPFX_RESTR), 0, 0,
	PHYS(1,0),	NO_DFNS,	NO_CARY, //needs quote
	0, A_CHAOTIC, NON_PM, NON_PM, 6660L), 

A("The Black Arrow",		ANCIENT_ARROW,
	(SPFX_RESTR), 0, 0,
	PHYS(1,0),	NO_DFNS,	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 4444L),

A("Tensa Zangetsu",		TSURUGI,
	(SPFX_RESTR|SPFX_HSPDAM|SPFX_EVIL), 0, 0, /*also has a haste effect when wielded, but massively increases hunger and damages the wielder*/
	PHYS(1,0),	NO_DFNS,	NO_CARY,
	0, A_NEUTRAL, NON_PM, NON_PM, 4444L),/*The invoked attack is very powerful*/

A("Sode no Shirayuki",		KATANA,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	COLD(1,0),	COLD(0,0),	NO_CARY,  /*Sort of intermediate between a double damage and a utility weapon,*/
	0, A_LAWFUL, NON_PM, NON_PM, 8000L),/*Sode no Shirayuki gains x2 ice damage after using the third dance.*/
	/*however, it only keeps it for a few rounds, and the other dances are attack magic. */

A("Tobiume",		LONG_SWORD,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(1,2),	FIRE(0,0),	NO_CARY,/*Tobiume is an awkward weapon.  It loses 3 damage vs large and 2 vs small*/
	0, A_CHAOTIC, NON_PM, NON_PM, 8000L),
	/*Ram and Fire blast only trigger if enemy is low hp*/

A("The Lance of Longinus",			SILVER_SPEAR, /* stun, confusion, hallu and freezing when wielded because seriously, Chris... --Amy */
	(SPFX_RESTR|SPFX_HSPDAM|SPFX_HPHDAM|SPFX_DEFN|SPFX_REFLECT|SPFX_EVIL), 0, 0, //needs quote
	NO_ATTK,	DFNS(AD_MAGM),	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 1500L), 

A("The Harkenstone", DIAMOND, /* aggravate monster when wielded */
    (SPFX_RESTR|SPFX_EVIL), 0, 0,
    PHYS(5,0), NO_DFNS, NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 8000L), 

A("Release from Care",			SCYTHE,
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_BEHEAD),0,0,
	PHYS(1,10),	DRLI(0,0),	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 4000L), 

A("The Silence Glaive",		GLAIVE, //needs quote
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_DRLI), 0, 0,
	DRLI(1,2),	DRLI(0,0),	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 8000L), 

A("The Garnet Rod",		STAR_ROD, //needs quote
	(SPFX_RESTR|SPFX_EREGEN|SPFX_REGEN|SPFX_EVIL), 0, 0, /*also has a haste effect when wielded, but massively increases hunger*/
	NO_ATTK,	NO_ATTK,	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 8000L), 

A("Helping Hand",			GRAPPLING_HOOK, //needs quote
	(SPFX_RESTR|SPFX_SEEK|SPFX_SEARCH|SPFX_WARN|SPFX_STLTH),0,0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 2000L), 

A("The Blade Singer's Spear",		SILVER_SPEAR,//needs quote
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(6,6),	NO_DFNS,	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 1500L), 

A("The Blade Dancer's Dagger",		SILVER_DAGGER,//needs quote
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(4,4),	NO_DFNS,	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 1500L), 

A("The Limb of the Black Tree",			CLUB,//needs quote
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(4,2),	DFNS(AD_FIRE),	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 3000L), 

A("The Lash of the Cold Waste",		BULLWHIP,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(4,2),	DFNS(AD_COLD),	NO_CARY,	
	0, A_CHAOTIC, NON_PM, NON_PM, 3000L), 

A("Ramiel",			PARTISAN,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK), 0, 0,
	ELEC(4,2),	DFNS(AD_ELEC),	NO_CARY,	/*Ramiel's ranged attack is far more useful than the lash and the limb*/
	0, A_LAWFUL, NON_PM, NON_PM, 3000L), /*So it's your job to use it right!*/

A("Spinesearcher",	SHORT_SWORD,
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	PHYS(1,6),	NO_DFNS,	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 1200L),

A("Quicksilver",	FLAIL,
	SPFX_RESTR, 0, 0,
	PHYS(4,8),	NO_DFNS,	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 1200L),

A("Sky Render",		KATANA, /* displacement when wielded */
	SPFX_RESTR, 0, 0,
	PHYS(10,10),	NO_DFNS,	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 1200L),

A("Fluorite Octahedron", FLUORITE, /*Needs encyc entry*/
	(SPFX_RESTR), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 4000L), 

A("The Tie-Dye Shirt of Shambhala",	T_SHIRT, //needs quote
	(SPFX_RESTR|SPFX_INTEL|SPFX_EVIL), 0, 0, /* Amy edit: sets itself to +10 when worn, but has create traps effect */
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0, A_NEUTRAL, NON_PM, NON_PM, 4500L), 

A("The Grandmaster's Robe",	ROBE, /*double robe effect*/
	(SPFX_RESTR), 0, 0,/*martial arts attacks use exploding dice and get extra damage*/
	NO_ATTK,	NO_DFNS,	NO_CARY, /* Amy edit: +5 AC, +10 marital arts damage */
	0, A_NEUTRAL, NON_PM, NON_PM, 4500L), 

A("The Platinum Dragon Plate",	SILVER_DRAGON_SCALE_MAIL,/*heavier than normal, and causes spell penalties*/
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL), 0, 0, //needs quote
	NO_ATTK,	DFNS(AD_MAGM),	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 9000L), 

A("Beastmaster's Duster", LEATHER_JACKET, //needs quote
	(SPFX_RESTR), 0, 0, /* Amy edit: M1_ANIMAL are spawned peaceful 90% and tame 1% */
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 9000L), 

A("The Shield of the All-Seeing", ORCISH_SHIELD, //needs quote
	(SPFX_RESTR|SPFX_DEFN|SPFX_SEEK|SPFX_SEARCH|SPFX_WARN|SPFX_DFLAG2), 0, M2_ELF,
     NO_ATTK, DFNS(AD_FIRE), NO_CARY,
	 0, A_NONE, NON_PM, NON_PM, 3000L),//needs message

A("The Shield of Yggdrasil", ELVEN_SHIELD, //needs quote
	(SPFX_RESTR|SPFX_DEFN|SPFX_REGEN), 0, 0,
     NO_ATTK, DFNS(AD_DRST), NO_CARY,
	 0, A_NONE, NON_PM, NON_PM, 3000L),

A("Water Flowers", WATER_WALKING_BOOTS, /* displacement */
	(SPFX_RESTR), 0, 0, //needs quote
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 4000L), //needs message

A("Hammerfeet", KICKING_BOOTS,
	(SPFX_RESTR), 0, 0, //needs quote
	PHYS(1,0),	NO_DFNS,	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 4000L), 

A("The Shield of the Resolute Heart",		GAUNTLETS_OF_DEXTERITY,
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 4000L), 

A("The Gauntlets of Spell Power",		GAUNTLETS_OF_POWER, /*Note: it is quite deliberate that these cause */
	(SPFX_RESTR|SPFX_HSPDAM), 0, 0,						/*		a spellcasting penalty. */
	NO_ATTK,	NO_DFNS,	NO_CARY, //needs quote
	0, A_NONE, NON_PM, NON_PM, 4000L), /* supposed to make spellcasting easier though... --Amy */

A("Premium Heart",		GAUNTLETS_OF_POWER,
	(SPFX_RESTR), 0, 0,
	PHYS(1,0),	NO_DFNS,	NO_CARY, //needs quote
	0, A_NONE, NON_PM, NON_PM, 4000L), 
	
A("Stormhelm",		HELM_OF_BRILLIANCE, /* elec resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	COLD(0,0),	NO_CARY, //needs quote
	0, A_CHAOTIC, NON_PM, NON_PM, 3000L), 

A("Hellrider's Saddle",			LEATHER_SADDLE,
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 4000L), 

A("The Rod of Seven Parts",	SILVER_SPEAR, /*From D&D*/
	(SPFX_ATTK|SPFX_RESTR|SPFX_INTEL|SPFX_DALIGN|SPFX_DEFN), 0,0,
	PHYS(7,20),	DRLI(0,0),	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 7777L), 

A("The Field Marshal's Baton",	MACE,
	(SPFX_RESTR|SPFX_WARN|SPFX_DFLAG2), 0, M2_MERC,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 5000L), 

A("Werebuster",			LONG_SWORD, //needs quote
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_WERE,/*should not be gifted or gened randomly*/
	PHYS(10,20),	NO_DFNS,	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 1500L), /*does not protect agains lycathropy*/

A("Masamune",			TSURUGI, //needs quote
	(SPFX_RESTR|SPFX_INTEL), 0, 0,/*should not be gifted or gened randomly*/
	NO_ATTK,	NO_DFNS,	NO_CARY, /* Amy edit: trap revealing effect */
	0, A_NONE, NON_PM, NON_PM, 7500L),

A("The Black Crystal", CRYSTAL_BALL, /*from Final Fantasy*/
        (SPFX_RESTR|SPFX_DEFN|SPFX_DALIGN|SPFX_WARN), 0, 0,
        PHYS(3,0), DFNS(AD_MAGM), NO_CARY, 
		0, A_CHAOTIC, NON_PM, NON_PM, 100L), 
	/*The crystals are generated together.  The Black Crystal counts, and the others don't.*/

A("The Water Crystal", CRYSTAL_BALL,
        (SPFX_RESTR|SPFX_DEFN), 0, 0,
        NO_ATTK, DFNS(AD_COLD), NO_CARY, 
		0, A_NONE, NON_PM, NON_PM, 100L), 

A("The Fire Crystal", CRYSTAL_BALL,
        (SPFX_RESTR|SPFX_DEFN), 0, 0,
        NO_ATTK, DFNS(AD_FIRE), NO_CARY, 
		0, A_NONE, NON_PM, NON_PM, 100L), 

A("The Earth Crystal", CRYSTAL_BALL,
        (SPFX_RESTR|SPFX_DEFN|SPFX_HPHDAM), 0, 0,
        NO_ATTK, NO_DFNS, NO_CARY, 
		0, A_NONE, NON_PM, NON_PM, 100L), 

A("The Air Crystal", CRYSTAL_BALL,
        (SPFX_RESTR|SPFX_DEFN), 0, 0,
        NO_ATTK, DFNS(AD_ELEC), NO_CARY, 
		0, A_NONE, NON_PM, NON_PM, 100L), 

A("The Annulus", SILVER_CHAKRAM,
	(SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_HSPDAM), 0, 0,
     PHYS(5,2), DFNS(AD_MAGM), NO_CARY, /*Actually Phys(5,0) if not a lightsaber*/
	 0, A_CHAOTIC, NON_PM, NON_PM, 3000L), 

A("The Iron Ball of Levitation", HEAVY_IRON_BALL, /* levitation when wielded */
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK|SPFX_DALIGN|SPFX_LUCK|SPFX_INTEL|SPFX_STLTH|SPFX_WARN|SPFX_EVIL), 0, 0,
	PHYS(5,10),	DFNS(AD_DRLI),	NO_CARY,
	0,	A_CHAOTIC, NON_PM, NON_PM, 5000L),
	/*Note: it had caried stealth before*/

A("The Iron Spoon of Liberation", SPOON,
	(SPFX_RESTR|SPFX_DEFN|SPFX_LUCK|SPFX_INTEL|SPFX_STLTH|SPFX_SEARCH|SPFX_SEEK), 0, 0,
	PHYS(5,0),	DFNS(AD_MAGM),	NO_CARY,
	0,	A_CHAOTIC, NON_PM, NON_PM, 5000L),
	/*Note: it had caried stealth before*/

A("Silver Starlight",		RAPIER,
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(4,4),	NO_DFNS,	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 5000L), /*Creates throwing stars. Makes throwing stars count as silver if wielded */
	/*Also can be (a)pplied as a magic flute.								  */

A("Wrathful Spider",		DROVEN_CROSSBOW,
	(SPFX_RESTR|SPFX_INTEL|SPFX_STLTH), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 5000L),

A("The Tentacle Rod",		FLAIL,
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(7,2),	NO_DFNS,	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 5000L),

A("The Crescent Blade",		SILVER_SABER,
	(SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_BEHEAD|SPFX_REFLECT), 0, 0,
	FIRE(4,0),	NO_DFNS,	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 5000L),

A("The Darkweaver's Cloak",	DROVEN_CLOAK,
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL), 0, 0,
	NO_ATTK,	DFNS(AD_MAGM),	NO_CARY, 
	0,	A_NONE,	 NON_PM, NON_PM, 5000L),

A("Spidersilk",	ELVEN_MITHRIL_COAT, /* spellcasting chance bonus */
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY, 
	0,	A_CHAOTIC,	 NON_PM, NON_PM, 5000L),
	/*Adds sleep poison to unarmed attacks*/

A("Webweaver's Crook",	FAUCHARD,
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL), 0, 0,
	PHYS(1,0),	DFNS(AD_MAGM),	NO_CARY, 
	0,	A_LAWFUL,	 NON_PM, NON_PM, 5000L),
	/*Adds poison to all attacks*/

A("Lolth's Fang",		DROVEN_SHORT_SWORD, /* Hedrow crowning gift, chaotic or neutral */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_INTEL), 0, 0,
	ACID(10,10),	DRLI(0,0),	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 8000L), 

A("The Web of Lolth",	ELVEN_MITHRIL_COAT, /* Drow crowning gift, chaotic */
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_INTEL|SPFX_WARN|SPFX_EVIL), 0, M2_ELF, /* spellcasting chance bonus */
	NO_ATTK,	DRLI(0,0),	NO_CARY, /* magic resistance and half speed when worn */
	0,	A_CHAOTIC, NON_PM, NON_PM, 2000L), 

A("The Claws of the Revenancer",		GAUNTLETS_OF_DEXTERITY, /* Drow crowning gift, neutral */
	(SPFX_RESTR|SPFX_ATTK|SPFX_EREGEN|SPFX_DRLI|SPFX_DEFN|SPFX_INTEL|SPFX_EVIL), 0, 0, /* causes fainting when worn */
	DRLI(1,2),	DRLI(0,0),	NO_CARY,
	0, A_NEUTRAL, NON_PM, NON_PM, 8000L), 

A("Liecleaver",		DROVEN_CROSSBOW, /* Drow noble crowning gift, lawful */
	(SPFX_RESTR|SPFX_SEEK|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH|SPFX_HALRES), 0, 0,
	PHYS(5,10),	DRLI(0,0),	NO_CARY,	
	0, A_LAWFUL, NON_PM, NON_PM, 8000L), 

A("The Ruinous Descent of Stars",		MORNING_STAR, /* Herow noble crowning gift, Chaotic */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL),0,0,
	PHYS(1,0),	DFNS(AD_MAGM),	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 8000L), 

A("Sickle Moon",	SICKLE, /*returns to your hand.*/
	SPFX_RESTR, 0,0,
	PHYS(1,0),	NO_DFNS,	NO_CARY,//needs quote
	0,	A_NONE, NON_PM, NON_PM, 4000L), 

A("Arcor Kerym",		LONG_SWORD, /* Lawful Elf crowning gift */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL), 0, 0,
	PHYS(5,0),	DRLI(0,0),	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 8000L), 

A("Aryfaern Kerym",		RUNESWORD, /* Neutral Elf crowning gift */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_INTEL), 0, 0, /* spellcasting chance bonus */
	ELEC(5,10),	ELEC(0,0),	NO_CARY,
	0, A_NEUTRAL, NON_PM, NON_PM, 8000L), 

A("Aryvelahr Kerym",		CRYSTAL_SWORD, /* Chaotic Elf crowning gift */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_REFLECT), 0, 0,
	PHYS(5,0),	DRLI(0,0),	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 8000L), 

A("The Armor of Khazad-dum",	DWARVISH_MITHRIL_COAT, /*Moria dwarf noble first gift */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL), 0, 0,
	NO_ATTK,	DFNS(AD_MAGM),	NO_CARY,
	0,	A_LAWFUL,	 NON_PM, NON_PM, 2500L),

A("The War-mask of Durin",	OUT_OF_MEMORY_HELMET, /*Moria dwarf noble */
	(SPFX_RESTR|SPFX_INTEL|SPFX_HSPDAM|SPFX_EVIL), 0, 0, /*+5 attk and damage with axes*/
	NO_ATTK,	NO_DFNS,	NO_CARY, /* Gives Fire, Acid, and Poison resistance */
	0,	A_LAWFUL,	 NON_PM, NON_PM, 2500L),

A("Durin's Axe",			AXE, /*Moria dwarf noble crowning */
	(SPFX_RESTR|SPFX_SEEK|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH),0,0,
	PHYS(10,10),	DRLI(0,0),	NO_CARY,	
	0, A_LAWFUL, NON_PM, NON_PM, 4000L), 

A("Glamdring",			ELVEN_BROADSWORD, /*Lonely Mountain dwarf noble first gift */
	(SPFX_WARN|SPFX_RESTR|SPFX_DFLAG2), 0, (M2_ORC|M2_DEMON), /* the old elves fought balrogs too. */
	PHYS(10,10),	NO_DFNS,	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 2000L), 

A("The Armor of Erebor",	PLATE_MAIL, /*Lonely Mountain dwarf noble */
	(SPFX_RESTR|SPFX_DEFN|SPFX_HPHDAM|SPFX_EVIL), 0, 0,  /*+10 AC*/
	NO_ATTK,	DFNS(AD_MAGM),	NO_CARY, /*Also gives Fire and Cold*/
	0,	A_LAWFUL,	 NON_PM, NON_PM, 2500L), /* but disables flying and causes chaos terrain --Amy */
	/* and disables reflection 25% of the time */

A("The Sceptre of Lolth", SILVER_KHAKKHARA, /* Drow noble first gift (hedrow get lordly might) */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL), 0, 0, /*needs quote*/
	PHYS(1,0),	NO_DFNS,	NO_CARY,
	0,	A_CHAOTIC, NON_PM, NON_PM, 4000L), 

A("The Web of the Chosen",	DROVEN_CLOAK, /* Drow noble quest */
	(SPFX_RESTR|SPFX_INTEL|SPFX_HSPDAM|SPFX_REFLECT|SPFX_EVIL), 0, 0, /* elec res */
	NO_ATTK,	ACID(0,0),	NO_CARY, /* Plus double AC bonus */
	0,	A_CHAOTIC,	 NON_PM, NON_PM, 2500L), /* slows you down like being a spirit */

A("The Cloak of the Consort",	DROVEN_CLOAK, /* Hedrow noble quest */
	(SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM|SPFX_EVIL), 0, 0, /* cold res */
	NO_ATTK,	DRLI(0,0),	NO_CARY, /* Plus double AC bonus */
	0,	A_NEUTRAL,	 NON_PM, NON_PM, 2500L), /* display loss unless turn counter is divisible by 10 */

A("The Rogue Gear-spirits", CROSSBOW,
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_SPEAK|SPFX_SEARCH|SPFX_SEEK|SPFX_WARN|SPFX_ESP), 0, 0,
	PHYS(5,0),	DFNS(AD_FIRE), NO_CARY,
	0, A_NEUTRAL, NON_PM, NON_PM, 4000L), 

A("The Moonbow of Sehanine", ELVEN_BOW,
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 4000L), 
	/*silver is the moon's metal... but bows don't enter this code...*/

A("The Spellsword of Corellon", HIGH_ELVEN_WARSWORD, /*Needs encyc entry*/
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(1,10),	NO_DFNS,	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 4000L), 

A("The Warhammer of Vandria", WAR_HAMMER,
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 4000L), 

A("The Shield of Saint Cuthbert", SHIELD_OF_REFLECTION,
	(SPFX_RESTR|SPFX_HSPDAM|SPFX_HPHDAM), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 4000L), 

A("Belthronding", ELVEN_BOW, /* displacement when wielded */
	(SPFX_RESTR|SPFX_INTEL|SPFX_STLTH), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 4000L), 

A("The Rod of the Elvish Lords", ELVEN_MACE, /* Elf noble first gift */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL), 0, 0, /*needs quote*/
	PHYS(3,0),	NO_DFNS,	NO_CARY,
	0,	A_CHAOTIC, NON_PM, NON_PM, 4000L), 

A("Sol Valtiva",			TWO_HANDED_SWORD,		/* The Sun of the Gods of the Dead */
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK|SPFX_EVIL),  0, 0, /* random item destruction and disables fire resistance for 1000 turns when wielded */
	FIRE(5,24),	DFNS(AD_FIRE),	NO_CARY,	
	0, A_CHAOTIC, NON_PM, NON_PM, 4000L), 

A("The Staff of the Archmagi",			QUARTERSTAFF, /* resist cold, elec, fire */
	(SPFX_RESTR|SPFX_ATTK|SPFX_SEARCH|SPFX_LUCK|SPFX_INTEL), 0, 0,
	STUN(20,4),	NO_DFNS,	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 9000L), 

A("The Robe of the Archmagi",			ROBE, /* displacement, blood mana, stun */
	(SPFX_RESTR|SPFX_DEFN|SPFX_REFLECT|SPFX_INTEL|SPFX_EVIL), 0, 0,
	NO_ATTK,	DFNS(AD_MAGM),	NO_CARY,	
	0, A_NONE, NON_PM, NON_PM, 9000L), 

A("The Hat of the Archmagi",			CORNUTHAUM, /* sight bonus */
	(SPFX_RESTR|SPFX_SPEAK|SPFX_WARN|SPFX_INTEL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	
	0, A_NONE, NON_PM, NON_PM, 9000L), 

A("The Kusanagi no Tsurugi",	LONG_SWORD, /* aggravate monster, recurring disenchantment and itemcursing when wielded */
	(SPFX_RESTR|SPFX_INTEL|SPFX_EREGEN|SPFX_BEHEAD|SPFX_LUCK|SPFX_SEARCH|SPFX_SEEK|SPFX_EVIL), 0, 0,
	PHYS(20,12),	NO_DFNS,	NO_CARY,
	0,		A_LAWFUL, NON_PM, NON_PM, 4500L), 

A("Genocide", TWO_HANDED_SWORD, /* bloodthirsty, uberjackal effect, disables reflection for 1000 turns */
	(SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_ATTK|SPFX_EVIL), 0, 0,
	FIRE(10,20),	DFNS(AD_FIRE),	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 9999L), 

A("The Rod of Dis", MACE,
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(10,8),	NO_DFNS,	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 9999L), 

A("Avarice", SHORT_SWORD,
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(10,2),	NO_DFNS,	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 9999L), 

A("Fire of Heaven", TRIDENT, /* resist elec */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_ATTK), 0, 0,
	FIRE(1,0),	DFNS(AD_FIRE),	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 9999L), 

A("The Diadem of Amnesia", DUNCE_CAP, /* guess what the effect of this thing is :-P --Amy */
	(SPFX_RESTR|SPFX_INTEL|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 9999L), /*was DRAIN_MEMORIES*/ 

A("Thunder's Voice", SILVER_DAGGER,
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_ATTK), 0, 0,
	ELEC(6,6),	DFNS(AD_ELEC),	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 3333L), 

A("Serpent's Tooth", ATHAME, /* poison res when wielded */
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 3333L), 

A("Unblemished Soul", UNICORN_HORN,
	(SPFX_RESTR|SPFX_INTEL|SPFX_LUCK), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 3333L), 

A("Wrath of Heaven", LONG_SWORD, /* resist fire */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_ATTK), 0, 0,
	ELEC(1,0),	DFNS(AD_ELEC),	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 9999L), 

A("The All-seeing Eye of the Fly", HELM_OF_TELEPATHY, /* undead warning */
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 9999L), /* Was SLAY_LIVING */

A("Cold Soul", RANSEUR, /* resist cold, elec, fire */
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 9999L), 

A("The Sceptre of the Frozen Floor of Hell", IRON_BAR,
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_ATTK), 0, 0,
	COLD(1,0),	DFNS(AD_COLD),	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 9999L), 

A("Caress", BULLWHIP,
	(SPFX_RESTR|SPFX_INTEL|SPFX_DFLAG2), 0, (M2_ELF|M2_HUMAN|M2_DWARF),
	ELEC(1,20),	ELEC(0,0),	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 9999L), 

A("The Iconoclast", SILVER_SABER,  /*Weapon of Lixer, Prince of Hell, from Dicefreaks the Gates of Hell*/
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_DFLAG2), 0, (M2_HUMAN|M2_ELF|M2_DWARF),
	PHYS(9,18),	DFNS(AD_MAGM),	NO_CARY, /*also does +9 damage to S_ANGELs*/
	0,	A_LAWFUL, NON_PM, NON_PM, 9999L), 

A("The Three-Headed Flail", FLAIL, /* speed when wielded */
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,	A_CHAOTIC, NON_PM, NON_PM, 6660L), 

A("Heartcleaver", HALBERD,
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(1,0),	NO_DFNS,	NO_CARY,
	0,	A_CHAOTIC, NON_PM, NON_PM, 6660L), 

A("Wrathful Wind", CLUB,
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_ATTK), 0, 0,
	COLD(10,0),	DFNS(AD_COLD),	NO_CARY,
	0,	A_CHAOTIC, NON_PM, NON_PM, 6660L), 

A("The Sting of the Poison Queen", FLAIL,
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_DEFN), 0, 0,
	PHYS(4,12),	DFNS(AD_MAGM),	NO_CARY,
	0,	A_CHAOTIC, NON_PM, NON_PM, 6660L), 

A("The Scourge of Lolth", BULLWHIP,
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(1,0),	NO_DFNS,	NO_CARY,
	0,	A_CHAOTIC, NON_PM, NON_PM, 6660L), 

A("Doomscreamer", TWO_HANDED_SWORD,  /*Weapon of Graz'zt, from Gord the Rogue*/
	(SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ACID(1,0),	ACID(0,0),	NO_CARY,
	0,	A_CHAOTIC, NON_PM, NON_PM, 6660L), 

A("The Wand of Orcus", WAN_DEATH, /* bloodthirsty */
	(SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DRLI), 0, 0,
	DRLI(5,12),	NO_DFNS,	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 8000L), 

A("The Sword of Erathaol",			LONG_SWORD, /* blindness resistance */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH), 0, 0,
	PHYS(7,10),	DFNS(AD_BLND),	NO_CARY,	/**/
	0, A_LAWFUL, NON_PM, NON_PM, 7500L), 	/**/

A("The Saber of Sabaoth",			SILVER_SABER,		/* blindness resistance */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH), 0, 0,
	PHYS(7,10),	DFNS(AD_BLND),	NO_CARY,	/**/
	0, A_LAWFUL, NON_PM, NON_PM, 7500L), 	/**/

A("The Sword of Onoel",			TWO_HANDED_SWORD,		/* blindness resistance */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH), 0, 0,
	PHYS(7,10),	DFNS(AD_BLND),	NO_CARY,	/**/
	0, A_LAWFUL, NON_PM, NON_PM, 7500L), 	/**/

A("The Glaive of Shamsiel",			GLAIVE,		/* blindness resistance */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH), 0, 0,
	PHYS(7,10),	DFNS(AD_BLND),	NO_CARY,	/**/
	0, A_LAWFUL, NON_PM, NON_PM, 7500L), 	/**/

A("The Lance of Uriel",			LANCE,		/* blindness resistance */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH), 0, 0,
	PHYS(7,10),	DFNS(AD_BLND),	NO_CARY,	/**/
	0, A_LAWFUL, NON_PM, NON_PM, 7500L), 	/**/

A("The Hammer of Barquiel",			LUCERN_HAMMER, /* blindness resistance */		/* Yes, I know it's bladed ;-) */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH), 0, 0,
	PHYS(7,10),	DFNS(AD_BLND),	NO_CARY,	/**/
	0, A_LAWFUL, NON_PM, NON_PM, 7500L), 	/**/

A("Silver Sniver",				MERCURIAL_ATHAME, /* Yes I know "sniver" is not a word. --Amy */
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_WARN), 0, M2_DEMON,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Jesus Must Die",				WOODEN_STAKE,
	(SPFX_RESTR), 0, 0,
	PHYS(4,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("The Randomisator",				GREAT_DAGGER, /* bad effects while wielded */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ATTK), 0, 0,
	STUN(4,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Mini People Eater",				WORM_TOOTH, /* warns of humanoids */
	(SPFX_RESTR|SPFX_DFLAG1), 0, M1_HUMANOID,
	PHYS(10,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Tigator's Thorn",				TOOTH_OF_AN_ALGOLIAN_SUNTIGER, /* displays all M3_POKEMON while wielded */
	(SPFX_RESTR|SPFX_DFLAG1), 0, M1_ANIMAL,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Gimli's War Axe",				DWARVISH_BATTLE_AXE,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5,10), DFNS(AD_FIRE), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("Rattle Battle",				DWARVISH_SHORT_SWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(2,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Sleepless Nights",				DARK_ELVEN_SHORT_SWORD, /* sleep resistance while wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Schringeling",				DROVEN_GREATSWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(8,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Memetal",				METAL_CLUB, /* deafness while wielded */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(6,16), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Cubic Bone",				BONE_CLUB, /* drain res while wielded */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(4,4), DFNS(AD_COLD), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Broomchamber Endurance",				MOP,
	(SPFX_RESTR|SPFX_REGEN|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2500L ),

A("Pinselflinseling",				MAGICAL_PAINTBRUSH, /* all engravings (even ones in the dust) never misengrave while you wield it */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	PHYS(7,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Teamantbane",				FUTON_SWATTER,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_ANT,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Laser Paladin",				REINFORCED_MACE, /* while having it equipped, all ranged attacks done by you have +1 multishot */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Armorwreaker",				BRONZE_MORNING_STAR, /* aggravate monster, disables stealth while equipped */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0,20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Destruction Ball",				SPINED_BALL, /* every time you hit with it, there's a 33% chance that its enchantment goes down by 1 unless it's already -20 or worse */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0,40), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Yesterday Asterisk",				DEVIL_STAR, /* will time you 1 in 5000 turns */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5,14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Melee Duality",				CHAIN_AND_SICKLE, /* double attacks */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Smaaaaaaaaaaaash",				TWO_HANDED_FLAIL,
	(SPFX_RESTR), 0, 0,
	PHYS(10,6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Strombringer",				RUNED_ROD, /* bloodthirsty */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_INTEL|SPFX_DRLI), 0, 0,
	DRLI(5,2),	DRLI(0,0),	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Blazerunner",				FIRE_HOOK,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK), 0, 0,
	FIRE(8,8), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Civil War",				PITCHFORK, /* conflict while wielded */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Heavy Thunderstorm",				BLACK_HALBERD, /* bloodthirsty */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_DRLI), 0, 0,
	DRLI(5,2), DRLI(0,0), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Rightlash Left",				GUISARME, /* every time you successfully apply it at a monster, it has 1 in 100 chance to gain enchantment unless it's already at +15 or higher */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Alassea Telemnar",				ELVEN_SICKLE, /* heavily curses itself, bloodthirsty, 1 in 20000 chance per turn that it disintegrates, all glyphs are fleecy-colored */
	(SPFX_RESTR|SPFX_DRLI|SPFX_EVIL|SPFX_ATTK), 0, 0,
	DRLI(5,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Gilraen Seregon",				ELVEN_SPEAR, /* petrification resistance */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5,10), DFNS(AD_FIRE), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2500L ),

A("Vampirebane",				LONG_STAKE, /* warns of vampires */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_VAMPIRE,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Golembane",				BAMBOO_SPEAR, /* warns of golems */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_GOLEM,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Pointless Javelin",				JAVELIN,
	(SPFX_RESTR), 0, 0,
	PHYS(2,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Eelbane",				TWO_HANDED_TRIDENT, /* warns of eels */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_EEL,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Move In The Shadows",				COURSE_LANCE, /* invisibility */
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Anachrononononaut Package",				FORCE_PIKE, /* unbreathing if you're an anachronist */
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK), 0, 0,
	ELEC(8,8), DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1400L ),

A("Glasspoint",				DROVEN_LANCE,
	(SPFX_RESTR), 0, 0,
	PHYS(8,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Eamane Luinwe",				ELVEN_LANCE, /* confusion and speed while wielded */
	(SPFX_RESTR|SPFX_STLTH|SPFX_EVIL|SPFX_ATTK), 0, 0,
	FIRE(10,12), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Gun Control Laws",				FLINTLOCK, /* ancient Morgothian curse, autocurses when wielded */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5,40), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10L ),

A("Overheater",				RAYGUN, /* autocurses, 1 in 1000 chance per turn to spawn a fire trap underneath the player */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL|SPFX_ATTK), 0, 0,
	FIRE(5,16), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Maximum Launch Power",				GRENADE_LAUNCHER, /* +rnd(2) multishot */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Kill Them All",				ANTIMATTER_PISTOL_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(20,40), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Pschiiiiiiiii",				HEAVY_BLASTER_BOLT,
	(SPFX_RESTR), 0, 0,
	PHYS(5,40), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 150L ),

A("Electrocution",				LASER_BEAM,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(10,20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Power Pellet",				SHOTGUN_SHELL,
	(SPFX_RESTR), 0, 0,
	PHYS(1,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Fire In The Hole",				FRAG_GRENADE,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(5,40), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Powered Arbalest",				PILE_BUNKER,
	(SPFX_RESTR), 0, 0,
	PHYS(2,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Demon Breakpoint",				KOKKEN,
	(SPFX_RESTR), 0, 0,
	PHYS(0,20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Little Annoyance",				SPIKE,
	(SPFX_RESTR), 0, 0,
	PHYS(6,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 40L ),

A("Ouchie Ouch",				NEEDLE,
	(SPFX_RESTR), 0, 0,
	PHYS(2,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 75L ),

A("Lag Spike",				CALTROP,
	(SPFX_RESTR), 0, 0,
	PHYS(10,6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Punishment For You",				CHAINWHIP, /* +3 damage per flail skill */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Shocklash",				MITHRIL_WHIP,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK), 0, 0,
	ELEC(8,12), DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Everything Must Burn",				FLAME_WHIP, /* 1 in 100 chance to be burned for d10 turns, allows player to survive in lava */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL|SPFX_ATTK), 0, 0,
	FIRE(2,16), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Female Beauty",				BLACK_DRESS, /* +5 CHA if you're female, -5 to all stats otherwise */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Bienvenido a Miami",				PRINTED_SHIRT, /* resist fire and cold, +3 CHA */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 0L ),

A("Thermal Bath",				BATH_TOWEL, /* swimming, sickness resistance, stuff does not get wet, turn limitation effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Giant Swinging Penis",				MEN_S_UNDERWEAR, /* +10 AC if you're male, otherwise nakedness effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Maedhros Saralonde",				DARK_ELVEN_MITHRIL_COAT, /* 5 extra points of AC, +2 melee damage, prayer timeout can be 250 turns higher and you can still pray */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Water Shyness",				SAFEGUARD_SUIT, /* heavily curses itself, disables flying and swimming, 1 in 100 chance to spawn a pool underneath you per turn (doesn't autotrigger) */
	(SPFX_RESTR|SPFX_DEFN|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Precious Virginity",				MAID_DRESS, /* prevents AD_SSEX if you're female */
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Very Invisible",				SPECIAL_CAMOUFLAGED_CLOTHES, /* invis, displacement */
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Check Your Escapes",				SHOULDER_RINGS, /* anti-teleportation, sickness resistance, free action */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Woodstock",				BAMBOO_MAIL, /* +5% chance to block if you have a shield; arrows (and only arrows, not e.g. bolts) have 50% extra chance of not breaking */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Nopped Suit",				STUDDED_LEATHER_ARMOR, /* 3 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Luke's Jedi Power",				FORCE_ARMOR, /* "use the force" effect, jumping */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("Gregor's Gangster Garments",				GENTLEMAN_S_SUIT, /* improves "borrow" command */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, OBJ_DETECTION, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Soft Girl",				GENTLEWOMAN_S_DRESS, /* makes you female if you aren't, 5 extra points of AC */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Shrink's Aid",				STRAITJACKET, /* autocurses (DUH), +7 AC */
	(SPFX_RESTR|SPFX_DEFN|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Lea's School Uniform",				DWARVISH_CLOAK, /* heavily autocurses (and does nothing else) because Lea is devious :P */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Larien Telrunya",				ELVEN_CLOAK, /* "Amy Bluescreen" translated to Elvish by www.chriswetherell.com/elf/index.php */
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Fireburn Coldshatter",				PLASTEEL_CLOAK, /* resist fire and cold, 5 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("No More Explosions",				ALCHEMY_SMOCK, /* alchemy will not result in explosions, unless you do stupid things with acid potions; making potions also works more often */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Premium Viscosity",				RUBBER_APRON, /* works like an oilskin cloak, also poison res */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Cooking Course",				KITCHEN_APRON, /* stepping on a fire will delete it with 50% chance */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2500L ),

A("Rokko Chan's suit",				FRILLED_APRON, /* jumping, speed, your ranged attacks have +5 to-hit, but you cannot score critical hits and your STR/DEX cap out at 12 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Fully Lionized",				LEO_NEMAEUS_HIDE, /* speed, hunger */
	(SPFX_RESTR|SPFX_STLTH|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Cold like a Corpse",				AYANAMI_WRAPPING,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Yaui Gaui Furs",				FUR, /* 5 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Deathclaw Hide",				HIDE, /* +10 to-hit with all melee attacks */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Fast Camo Predator",				DISPLACER_BEAST_HIDE, /* speed and hunger */
	(SPFX_RESTR|SPFX_STLTH|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Predatory Stability",				THE_NEMEAN_LION_HIDE, /* free action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Spacewaste",				FILLER_CLOAK, /* 3 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Bugnose",				CLOAK_OF_SPRAY, /* displays all 'a' and 'x' */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Disbelieving Powerlord",				ANTI_CASTER_CLOAK, /* +d5 melee damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Double Negation",				CLOAK_OF_LUCK_NEGATION,
	(SPFX_RESTR|SPFX_LUCK|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Donald Trump's Presidential Coat",				CHATBOX_CLOAK,
	(SPFX_RESTR|SPFX_WARN|SPFX_HALRES|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6000L ),

A("Godless Void",				HERETIC_CLOAK, /* blocks telepathy */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Number 8192",				MISSING_CLOAK, /* doesn't do anything special */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Jana's Extreme Hide-and-Seek",				NASTY_CLOAK, /* you can conceal underneath items, but uncovering a concealing monster has 1 in 100 chance to instakill you (no joke). */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("The Last Steeling",				SPECIAL_CLOAK, /* allows you to rustproof an iron object with 1 in 5000 chance per turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Nothing Really Special",				PLAIN_CLOAK,
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Primitive Shielding",				ARCHAIC_CLOAK,
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Tari Fefalas",				ELVEN_LEATHER_HELM, /* poison res, monsters attacking you take poison damage, radio broadcasts, glib effect times out in one turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("Steelskull Protector",				ORCISH_HELM, /* 3 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Elessar Elendil",				HIGH_ELVEN_HELM, /* speed and fumbling */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Sexyness Has A Name",				NURSE_CAP, /* doubles healup effects, or quadruples if you're a healer */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Squeaky Tenderness",				KATYUSHA, /* farting monsters are 25% generated tame */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("It Breathes More",				DRAGON_HORNED_HEADPIECE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, DRAGON_BREATH, A_NONE, NON_PM, NON_PM, 4000L ),

A("Coneshape Hat",				STRAW_HAT,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Hard Hat Area",				MECHA_IRAZU, /* 5 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Dunce Pounce",				SCHOOL_CAP, /* caps your INT and WIS at 6, but gives +5 STR, +3 DEX */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Remote Gamble",				CHAIN_COIF, /* +2 increase damage and accuracy */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Hot-Headed Hat",				SEDGE_HAT,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Green Status",				SKULLCAP, /* poison resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Alluration",				HELM_OF_DRAIN_RESISTANCE, /* gradually curses itself while worn (1 in 1000 chance per turn), and may give intrinsic nastiness if it does */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("The Wolf King",				HELM_OF_DISCOVERY, /* no digestion, weak sight, right mouse button loss, blocks telepathy, heavily autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Wschiiiiie!",				HELM_OF_THIRST, /* clairvoyance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Never Clean",				ANTI_DRINKER_HELMET, /* confusion */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Web Radio",				WHISPERING_HELMET, /* internet access */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2400L ),

A("Dullifier",				HELM_OF_SENSORY_DEPRIVATION, /* invisibility, disables sleep resistance */
	(SPFX_RESTR|SPFX_STLTH|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("SOON THERE WILL BE AN ERROR",				BIGSCRIPT_HELM, /* every turn there is 1 in 5000 chance to give you intrinsic nastiness */
	(SPFX_RESTR|SPFX_DEFN|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7500L ),

A("Double Jeopardy",				QUIZ_HELM, /* teleportitis and polymorphitis */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("If The Right Mouse Button Fails Now",				DIZZY_HELMET, /* 1 in 2000 chance per turn to summon a Gunnhild's General Store */
	(SPFX_RESTR|SPFX_EREGEN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("The Iron Helm of Gorlim",				MUTING_HELM, /* topi ylinen curse, +10 to-hit and to-damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Nevereater",				ULCH_HELMET, /* slow digestion */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Certain Slow Death",				HELM_OF_STARVATION, /* conflict */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Drink Coca Cola",				QUAFFER_HELMET, /* hunger */
	(SPFX_RESTR|SPFX_REGEN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Have All You Need",				INCORRECTLY_ADJUSTED_HELMET, /* resist fire, cold, shock, sleep */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Nosed Bug",				RANDOMIZED_HELMET, /* 7 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Massive Iron Crown of Morgoth",				UNWANTED_HELMET, /* prime curses itself, carries ancient morgothian curse, +5 all stats, resist fire/cold/shock/poison/acid */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Randomness Prevails",				WONDER_HELMET, /* teleportitis and polymorphitis, disables tele and poly control */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Casquespire Translate",				POINTED_HELMET, /* 5 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Gold Standard",				BOG_STANDARD_HELMET, /* random gold drops have higher amounts of zorkmids in them */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Helmet of Digging",				MINING_HELM, /* supposed to increase digging speed with e.g. pick-axe */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Army Leader",				FIELD_HELM, /* soldier types are 5% tame */
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Secure Bathmaster",				HELM_OF_SAFEGUARD, /* resist fire, cold and light */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Deep Insanity",				HELM_OF_UNDERWATER_ACTION, /* inventorylessness, autocurses when worn */
	(SPFX_RESTR|SPFX_DEFN|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2500L ),

A("Radar Not Working",				HELM_OF_JAMMING, /* monsters never approach, but no detection methods work either and newly spawned ones are completely invisible */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Bettervision",				HELM_OF_TRUE_SIGHT,
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Refuel Badly",				HELM_OF_NO_DIGESTION, /* manaleech */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Burn Or No",				PLASTEEL_GLOVES, /* player acquires intrinsic burnopathy from putting them on */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Free Action Called Free Action",				ORIHALCYON_GAUNTLETS, /* free action (DUH) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Fumblefingers Quest",				GAUNTLETS_OF_FUMBLING, /* sitting on a throne always gives the fumblefingers effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Oh Look At That",				COMMANDER_GLOVES, /* unlocks petkeeping skill and caps it at expert but also prime curses itself if you didn't have the skill before */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Line In The Sand",				FIELD_GLOVES, /* trap revealing */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Handboxed",				UNWIELDY_GLOVES, /* +1000 carry capacity */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Yes to Ranged Combat",				UNDROPPABLE_GLOVES, /* +d6 damage when throwing */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Spectrator",				GAUNTLETS_OF_TRAP_CREATION, /* 20% chance of resist fire, cold, shock, poison, acid, stone, drain and magic */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Use The Force Luke",				GAUNTLETS_OF_THE_FORCE, /* +10 damage for the force command */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, LEVITATION, A_NONE, NON_PM, NON_PM, 200L ),

A("Expertengame the entire level",				DIFFICULT_GLOVES, /* flying, monsters often create a trap upon spawning */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("What's Up Bitches",				GAUNTLETS_OF_STEALING, /* displays all 'n', and they're generated peaceful 90% and tame 2% */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Balls Flying Back And Forth",				GAUNTLETS_OF_MISFIRING,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("What Do",				UNKNOWN_GAUNTLETS,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Grabber Master",				EVIL_GLOVES, /* while worn, you have 1 in 1000 chance per turn to detect gold (cannot be exploited to detect traps) */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Defensive Magic",				ARCANE_GAUNTLETS,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Non-Something",				PLACEHOLDER_GLOVES,
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Doubler Gloves",				PROTECTIVE_GLOVES,
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 250L ),

A("Dwarven Bong",				DWARVISH_ROUNDSHIELD, /* no special effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Absurd-Heeled Tileset",				HIGH_BOOTS, /* count as high heels */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Grandpa's Brogues",				LEATHER_SHOES, /* fear resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Vera's Freezer",				SOFT_SNEAKERS, /* give a freezopathy-like effect when worn, kicking a monster will slow it down unless it resists, player's potions cannot shatter from cold attacks, being frozen slows the player less than usual, but fire resistance is deactivated and burn effects will always become "heavy burn" */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2500L ),

A("High Heeled Hug",				BOOTS_OF_ACID_RESISTANCE, /* count as high heels */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 8000L ),

A("Free For Enough",				FUMBLE_BOOTS, /* free action */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Double Safety",				BOOTS_OF_TOTAL_STABILITY,
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Lovely Girls Wear Plateau Soles",				STOMPING_BOOTS,
	(SPFX_RESTR|SPFX_DEFN|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Fatality",				BOOTS_OF_WEAKNESS, /* conflict */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Porcelain Elephant",				STAIRWELL_STOMPING_BOOTS, /* aggravate monster, triggers wakeup 1 in 100 turns, +5 AC */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 250L ),

A("FD Deth",				PET_STOMPING_PLATFORM_BOOTS, /* displays all 'f' and 'd' monsters */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Lovely Girl Plateaus",				ASS_KICKER_BOOTS, /* kicking a monster will always stun and confuse it, aggravate monster, monsters are always spawned hostile */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Kylie Lum's Snakeskin Boots",				HIGH_STILETTOS, /* putting them on will cause all monsters to spawn hostile for the rest of the game; +10 kicking damage and your kick cannot be "clumsy" and has 25% chance of paralyzing the enemy; they carry an ancient Morgothian curse unless you're a topmodel, and if you try to put them on as a failed existence, you acquire intrinsic nastiness! */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 25000L ),

A("Sandra's Beautiful Footwear",				UNFAIR_STILETTOS, /* confusion resistance, swimming, unbreathing */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Uneven Stilts",				SKY_HIGH_HEELS, /* fumbling (like irregular heels), +15 charisma */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Neanderthal Soccer Club",				PREHISTORIC_BOOTS, /* kicking an item will exercise STR and DEX if the item moves */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Shin Kick of Love",				SYNTHETIC_SANDALS, /* kicking a monster has 1% chance to make it peaceful, unless it resists */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 3000L ),

A("Absolute Autocurse",				RIN_CURSE, /* autocurses your entire inventory if you put it on */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Lucky Gadget",				RIN_RANDOM_EFFECTS,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Clearvision",				RIN_SPECIAL_EFFECTS,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Guaranteed Hit Power",				RIN_INCREASE_ACCURACY, /* increase DEX by enchantment value */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Unbelievably Strong Punch",				RIN_INCREASE_DAMAGE, /* increase STR by enchantment value */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Deathly Cold",				RIN_FAST_METABOLISM, /* disables cold resistance */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Magical Shock",				RIN_NASTINESS, /* drain res */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Good Things Will Happen Eventually",				RIN_BAD_EFFECT, /* turn counter increases half as fast */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Required Gladness",				RIN_DISENGRAVING, /* speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Never Needed",				RIN_ANTI_DROP,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Knowledgeable Failure",				RIN_FAILED_TRAINING,
	(SPFX_RESTR|SPFX_ESP|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 700L ),

A("Scrawny Pipsqueak",				RIN_FAILED_EXERCISE, /* +5 AC */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Tschoeck Kloeck",				AMULET_OF_POLYMORPH, /* deactivates poly control for 1 million turns when put on */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Spellcaster's Dream",				AMULET_OF_THE_RNG, /* disables sleep res */
	(SPFX_RESTR|SPFX_EREGEN|SPFX_HSPDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Low Zero Number",				AMULET_OF_INFINITY, /* banking effect, silently autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Dynamitus",				AMULET_OF_EXPLOSION, /* causes an explosion centered on you 1 in 2000 turns */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("I Never Take Drugs",				AMULET_OF_WRONG_SEEING,
	(SPFX_RESTR|SPFX_HALRES|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Uoy Ot Kcab Dnes",				AMULET_OF_WEAK_MAGIC,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Goodbye Trolls",				LARGE_BOX_OF_DIGESTION, /* warns of trolls when wielded */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_TROLL,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Priamos' Treasure",				CHEST_OF_HOLDING, /* like Wallet of Perseus */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Ice Block Harharharharhar",				ICE_BOX_OF_WATERPROOFING, /* +2 melee damage for every corpse in it, up to a maximum of +30 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Recycler Bin",				ICE_BOX_OF_DIGESTION, /* +1 alignment for every item that is deleted by it; +50 alignment and +5 max alignment if it deletes an artifact */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Surfing Fun",				OILSKIN_SACK, /* never leaks, even if cursed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Monsterator",				BAG_OF_DIGESTION, /* if it deletes at least 10 items at once, it will summon as many monsters as the # of items deleted and increases your prayer timeout by 50x that value */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Go Away You Bastard",				DARK_MAGIC_WHISTLE, /* also has a phase door effect on you if not cursed, but will instantly curse itself (even if blessed) 25% of the time */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Battlehorn of Sescheron",				FROST_HORN, /* these artifact horns only consume a charge 50% of the time they're played */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Hellish Wartube",				FIRE_HORN,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Heaven's Call to Arms",				TEMPEST_HORN,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Bimmel Bimmel",				BELL, /* applying it tames all 'x' that fail a resistance check */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Scrabble Babble",				FELT_TIP_MARKER, /* only loses charges 10% of the time */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("i the Sage",				SWITCHER, /* spawns a Gunnhild's General Store on the level */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("F-PROT",				PACK_OF_FLOPPIES, /* applying it can also contain 'Bandarchor', which causes a random bad effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Panic in Gotham Forest",				SCR_GROWTH, /* fills the entire level with trees */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("ENSBADEB FRAUSED",				WAN_TOXIC,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Movie Analogy",				WAN_CHLOROFORM,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Supergirl's Jump and Run Fun",				MORION, /* jumping when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Automatic Poke Ball",				LOADSTONE, /* if you have it in your inventory, all pokemon have 3% chance of being spawned tame */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Cubic Sodium Chloride",				SALT_CHUNK, /* dissolving it creates 5 tame familiars */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Magic Resistance Get",				BOULDER,
	(SPFX_RESTR), (SPFX_DEFN), 0,
	NO_ATTK, NO_DFNS, CARY(AD_MAGM), 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Shocking Therapy",				REALLY_HEAVY_IRON_BALL, /* disables shock resistance */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ATTK), 0, 0,
	ELEC(5,14), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Enigmatic Riddle",				HEAVY_CONUNDRUM_BALL,
	(SPFX_RESTR), 0, 0,
	PHYS(7,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2400L ),

A("Do You Even Lift",				IMPOSSIBLY_HEAVY_MINERAL_BALL,
	(SPFX_RESTR|SPFX_DEFN|SPFX_REFLECT), 0, 0,
	PHYS(1,0), DFNS(AD_DRLI), CARY(AD_MAGM), 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Gangbanging Like A Boss",				GOLD_CHAIN,
	(SPFX_RESTR), 0, 0,
	PHYS(0,20), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Freezemetal",				ELYSIUM_SCOURGE,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	COLD(7,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2400L ),

A("Kings Ransom For You",				MINERAL_HOSTAGE_CHAIN, /* curses itself with TY curse when wielded, half speed */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_PROTEC|SPFX_EVIL|SPFX_ATTK), 0, 0,
	STUN(5,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Do Not Throw Me",				SEGFAULT_VENOM, /* causes segfault panic if you're stupid enough to throw it! */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 0L ),

A("Waters of Oblivion",				LETHE_CLOAK, /* amnesia 1 in 1000 turns, all & are 95% peaceful and 20% tame, swimming */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Jonadab's Winter Wear",				UGG_BOOTS,
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Madman's Power",				AMULET_OF_MAP_AMNESIA, /* manaleech */
	(SPFX_RESTR|SPFX_EREGEN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 500L ),

A("Remembering The Bad Times",				CLOAK_OF_MAP_AMNESIA, /* keen memory */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Eighth Deadly Sin",				AMULET_OF_DEPRESSION, /* surrounds you with sin traps 1 in 10000 turns */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Permanentitis",				CLOAK_OF_TRANSFORMATION, /* polymorphs never time out, but also disables poly control */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Waterforce 2.0",				CLOAK_OF_WATER_SQUIRTING, /* swimming, unbreathing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Now It's For Real",				CLOAK_OF_STARVING, /* conflict */
	(SPFX_RESTR|SPFX_REGEN|SPFX_EREGEN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Black Veil of Blackness",				CLOAK_OF_CURSE, /* ancient morgothian curse, antimagic */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Clapclap",				CLOAK_OF_DISENCHANTMENT,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("More Higher",				CLOAK_OF_ESCALATION, /* 1 in 2000 chance per turn to increase escalation counter, 1 in 100 monsters has escalation egotype */
	(SPFX_RESTR|SPFX_DEFN|SPFX_ESP|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Filthy Mortals Will Die",				CLOAK_OF_ANGRINESS, /* +10 negative protection when putting it on */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Dschlschlschlschlsch",				CLOAK_OF_CANCELLATION, /* deactivates MR for exactly one turn when you put it on */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Hahaha-ha hahahahaha",				CLOAK_OF_TOTTER, /* putting it on gives permanent respawning and displays a very funny message :D */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Cleanliness Lab",				HUGGING_GOWN, /* sickness resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Floating Flame",				FLOATCLOAK,
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Demonic Undead Radar",				BESTEST_CLOAK, /* warning of demons */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Sexy Stroking Units",				BIKINI, /* +10 AC, +5 CHA */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("War Me Nevertheless",				CLOAK_OF_PERMANENCE,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 6000L ),

A("Jedi Mind Power",				CLOAK_OF_THE_FORCE, /* putting it on offers you to learn telekinesis, but if you say yes, three random intrinsics get disabled for 1 million turns each */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Arabella's Lightningrod",				CLOAK_OF_CURSE_CATCHING, /* banishmentitis, itemcursing, silently becomes prime cursed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Ka-Blammo",				SAWED_OFF_SHOTGUN, /* every time you trigger a trap, its enchantment randomly goes either up or down, but cannot go below -20 or above +10. BUC status determines if it's more likely to go up or down */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("RNG's Fun",				ROBE_OF_RANDOMNESS, /* if you put it on while it's +0, it will randomly set itself to something between -5 and +5 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("You're The Best",				ROBE_OF_SPECIALTY,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Anastasia's Soft Clothes",				ROBE_OF_NASTINESS, /* +10 AC */
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Plentyhorn of Famine",				HORN_OF_PLENTY, /* causes hunger when applied */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Marine Threat Neutered",				FLINT,
	(SPFX_RESTR|SPFX_DFLAG1), 0, M1_SWIM,
	PHYS(1,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Bang Bang",				SAWED_OFF_SHOTGUN, /* deafness when wielded, and sets itself to +2 if it's lower */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ATTK), 0, 0,
	STUN(5,20), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Tuna Cannon",				POWER_CROSSBOW, /* +1 multishot, you take maximum damage, cuts your AC to one quarter of the normal amount */
	(SPFX_RESTR|SPFX_ATTK|SPFX_EVIL), 0, 0,
	COLD(0, 20), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Pfiiiiiiiiet",				TIN_WHISTLE, /* no specialty - just to unlock petkeeping skill */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Dogwalk",				LEATHER_LEASH, /* ditto */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Friend Call",				SCR_CREATE_FAMILIAR, /* creates two pets at once */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("For The Good Cause",				SCR_TAMING, /* confused radius even if you're not confused */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("Warpchange",				RIN_POLYMORPH, /* teleportitis */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Semi Shape Control",				RIN_POLYMORPH_CONTROL, /* also gives polymorphitis */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Formtaker",				AMULET_OF_POLYMORPH, /* gives permanent intrinsic polymorphitis if you put it on! */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Colorless Variety",				AMULET_OF_POLYMORPH_CONTROL, /* shades of grey effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Medical Ophthalmoscope",				STETHOSCOPE, /* always acts like a blessed one */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Search And You Will Find",				RIN_SEARCHING,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Lots of Knowledge",				RIN_TRAP_REVEALING,
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Hoyo Hoyo Wololo",				GOD_O_METER, /* always acts like a blessed one */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Haaaaaaaaaaaaa leluja",				POT_WATER, /* blesses itself upon being picked up and identifies its BUC? */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Gruum Gruum",				HOLY_WAFER, /* no specialty - just meant to unlock spirituality skill */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Ancient Sacrifice",				SCR_CONSECRATION, /* ditto */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Zen Buddhism",				SCR_CREATE_ALTAR, /* ditto */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Chrrrrrrrrrrrrr",				SPE_GOOD_NIGHT, /* all of these are there just so gifts have a chance of unrestricting spell schools */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Useless Element",				SPE_POISON_BLAST,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Inversion Therapy",				SPE_BANISHING_FEAR,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Magical Blindfolding",				SPE_BLIND_SELF,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Tool-Assisted Magic",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Add Acid To Water",				SPE_CHEMISTRY,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Ohmygodhelpme",				SPE_CAUSE_FEAR,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Cursebreaking 101",				SPE_DESTROY_ARMOR,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Malignant Aura",				SPE_CURSE_ITEMS,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Coated For Good",				SPE_ACIDSHIELD,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Arabella's Escape Route",				SPE_WARPING,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Night Movement",				SPE_TIME_SHIFT,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Spamming Defense Magic",				SPE_REFLECTION,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Fartbolt",				SPE_FIRE_BOLT,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Jonadab's Evil Patch Artifact",			AKLYS,
	(SPFX_RESTR), 0, 0,
	PHYS(0,2), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 20L ),

A("What It Says On The Tin",				SPE_GENOCIDE,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK), 0, 0,
	FIRE(2,20), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Deadly Gambling",				SPE_RUSSIAN_ROULETTE, /* +d30 damage; every time you wield it there's a 1% chance to die instantly */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Prismatic Protection",				SPE_ELEMENTAL_BEAM, /* resist cold, fire, poison, lightning when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Irresistible Offense",				SPE_MANA_BOLT,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Eurgh",				SPE_INDUCE_VOMITING,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Quick Slowness",				SPE_RANDOM_SPEED,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Final Explosion",				SPE_SELFDESTRUCT,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Stalwart Bunker",				SPE_BURROW,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Frozen Polar Bear",				SPE_FROST,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Doenerteller Versace",				CRAM_RATION, /* eating it gives temporary drain and magic resistance as well as reflection, rnz(2000) turns for each */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 150L ),

/* below are artifacts suggested by ProzacElf */
A("ProzacElf Shatterhand",				GAUNTLETS_OF_FUMBLING,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("ProzacElf's Autohealer",				CLOAK_OF_INVENTORYLESSNESS, /* 1 in 1000 chance of gaining an additional max HP per turn, but if that effect procs, you also get rnd(1000) turns of nastiness */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("ProzacElf's Poopdeck",				SPE_CHAOS_TERRAIN, /* wielding it causes chaos terrain */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

/* below are artifacts suggested by Dikkin; they're a bit different from his original suggestions,
 * because (believe it or not) game balance is very important to me. --Amy */
A("Dikkin's Deadlight",				WAN_POLYMORPH, /* yellow spells when wielded, temporary yellow spells when zapped by anyone, zapping yourself gives 1 turn of polymorph control and allows you to choose a polyform effect instead */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Dikkin's Dragon Teeth",				AMULET_OF_POLYMORPH_CONTROL, /* yellow spells except if you're a kobold bard, being polymorphed allows you to choose a polyform effect instead, flying, prevents you from wearing body armor */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, DRAGON_BREATH, A_NONE, NON_PM, NON_PM, 4000L ),

A("Dikkin's Favorite Spell",				SPE_POLYMORPH, /* +8 INT and yellow spells when wielded, allows you to choose a polyform effect when you zap yourself with the spell but doing so causes temporary yellow spells */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Anastasia's Perilous Gamble",			SCR_WONDER, /* teaches a random technique and gives intrinsic evilpatch effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Erase All Data",			SCR_ROOT_PASSWORD_DETECTION, /* data delete if you're stupid enough to read it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Garok's Hammer Kit",			MATERIAL_KIT, /* can be used several times */
	(SPFX_RESTR|SPFX_SPEAK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50000L ),

A("Actual Precision",				IMPLANT_OF_PRECISION, /* +5 increase accuracy, and an additional +5 if in a form without hands */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Henrietta's Tenaciousness",				IMPLANT_OF_VILENESS, /* autocurses, evil artifact effect, resist acid, prevents your gear from being destroyed by erosion, 10 extra points of AC and 1 extra point of MC if in a form without hands */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Healeneration",				IMPLANT_OF_REMEDY, /* if in a form without hands, applies an uncursed unicorn effect every turn */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Can't Bring Us Down",				IMPLANT_OF_STOICISM, /* free action, and discount action if in a form without hands */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Scrooge's Money Memory",				IMPLANT_OF_AVARICE, /* doubles randomly generated gold, or quadruples if in a form without hands */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("The White Whale Hath Come",				IMPLANT_OF_WINTER, /* if in a form without hands, your potions cannot be destroyed by cold and you cannot slip on ice and are unaffected by snowstorms */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Brrrrrrrrrrrrrmmmmmm",				IMPLANT_OF_ACCELERATION, /* +50% movement speed if you're on a highway, or always if in a form without hands; drains 1 Pw every odd turn while in a form without hands, or every turn otherwise */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Katrin's Sudden Appearance",				IMPLANT_OF_QUICKENING, /* very fast speed, fainting effect, disables free action and discount action, halves the cost for Ctrl-T if in a form without hands */
	(SPFX_RESTR|SPFX_EVIL|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Sinful Repenter",				IMPLANT_OF_KARMA, /* bad alignment effect, things that increase your alignment record increase it by 2x the usual amount or 5x if in a form without hands, and if in a form without hands there's also a 20% chance of getting +1 to the maximum */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Gymnastic Love",				IMPLANT_OF_ELUSION, /* 20% chance of avoiding melee attacks, and 33% chance of avoiding missile attacks if in a form without hands */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("SLEX Wants You To Die A Painful Death",				IMPLANT_OF_SUFFERING, /* putting it on gives +1 max HP but also a random nasty trap effect as an intrinsic; magic resistance and reflection if in a form without hands */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Fukrosion",				IMPLANT_OF_FAST_REPAIR, /* allows you to repair an eroded item 1 in 2500 turns, and will also erosionproof the item in question if in a form without hands */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Yes You Can",				IMPLANT_OF_CONTROL, /* polymorph control, allows you to get an inertia control-like effect 1 in 500 turns if in a form without hands */
	(SPFX_RESTR|SPFX_EVIL|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Rhea's Missing Eyesight",				IMPLANT_OF_CLUMSINESS, /* -rnd(20) to-hit; +rnd(5) increase damage and poison resistance if in a form without hands */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Rubber Shoals",				IMPLANT_OF_INSULATION, /* randomly turns floor terrain into ash, fire resistance and lava swimming if in a form without hands */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Thai's Erotic Bitch-Fu",				IMPLANT_OF_FRAILTY, /* -10 constitution, +5 strength; if in a form without hands, also +5 CHA/INT/WIS and +10 DEX */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Dompfination",				IMPLANT_OF_KNOWLEDGE, /* you can read spellbooks successfully even if they're cursed; if in a form without hands, spells cost 10% less mana to cast */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Burn Baby Burn",				IMPLANT_OF_MALICE, /* gives you the burn status; if in a form without hands, killing a monster recovers some HP and Pw */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Timeage of realms",				IMPLANT_OF_AGES, /* time resistance if in a form without hands */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Wary Protectorate",				IMPLANT_OF_SUSTENANCE, /* half spell damage if in a form without hands */
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Some Little Aid",				IMPLANT_OF_COMBAT, /* +1 increase accuracy; +1 increase damage if in a form without hands */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Ho yoo-yoyo",				IMPLANT_OF_FAITH, /* gives you a message whether it's safe to pray, lack of hands is not considered a trouble while wearing this */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Decapitation Up",				IMPLANT_OF_VITA, /* life saving (and is used up if you die), prevents beheading if in a form without hands */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Wonderloveliness",				IMPLANT_OF_BEAUTY, /* +5 charisma, and an additional +5 if in a form without hands */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Mighty Molass",				IMPLANT_OF_INERTIA, /* freeze status effect; if in a form without hands, 1 in 500 chance per turn that hostile monsters adjacent to you are hit with slow monster */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Unforgettable Event",				IMPLANT_OF_MEMORY, /* keen memory; if in a form without hands, 90% chance to resist the effects of amnesia */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Dubai Tower Break",				IMPLANT_OF_THAWING, /* 20% chance per turn that the freeze status effect is cured; if in a form without hands, cold resistance and your potions cannot be destroyed by cold */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Arrgh Ouch",				IMPLANT_OF_PAIN, /* 20% chance per turn to take 10 points of damage; if in a form without hands, every such incident trains your healing spell skill */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Etheratorgarden",				IMPLANT_OF_CORRUPTION, /* nastiness effect, +20% speed if in a form without hands */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Ready For A Ride",				IMPLANT_OF_TRAVELING, /* +20% speed when riding, but only for movement; if in a form without hands, can pick up stuff while riding even when unskilled and increases the HP regeneration rate of a ridden mount even more when the skill is high */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Jana's Make-Up Putty",				IMPLANT_OF_CHEATING, /* flying and unbreathing, but if you use a staircase and are not in a form without hands, 1 in 100 chance to die instantly (no joke). */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Potatorok",				IMPLANT_OF_VALHALLA, /* resist fire and contamination, see invisible, teleportitis, and every monster can cause Ragnarok if it hits you in melee. Disables teleport control unless you are in a form without hands. If in a form without hands, also gives half physical and spell damage. */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 10000L ),

A("They're All Yellow",				IMPLANT_OF_BILE, /* acid resistance, regeneration if in a form without hands */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Gelmer-Kelana Twin",				IMPLANT_OF_VANILLA, /* +10 spawn frequency for M5_VANILLA monsters, game behaves as if it were soviet mode unless you're in a form without hands */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("No Abnormal Future",				IMPLANT_OF_ABRASION, /* whenever you take a hit via mhitu.c, 1 in 20 chance that a random armor piece you're wearing is withered and 1 in 500 chance that you're hit with antimatter; if you use a technique while in a form without hands, the timeout is 4 times faster */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Significant RNG Jitter",				IMPLANT_OF_OSMOSIS, /* polymorphitis, regeneration; if in a form without hands, it also grants swimming, magical breathing, polymorph control and the ability to walk straight through iron bars, farmland, mountains and potentially other terrain */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Laughing At Midnight",				IMPLANT_OF_ENVY, /* random intrinsic loss; if in a form without hands, resist poison/acid and 5 extra points of AC */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("You Should Surrender",				IMPLANT_OF_IMPOSSIBILITY, /* monster spawn frequency becomes 5x faster and respawn effect when they're killed; when in a form without hands, 50% extra speed and double attacks */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Arabella's Sexy Charm",				IMPLANT_OF_ADMIRATION, /* starlit and RMB loss effect, teleportitis; if in a form without hands, also gives teleport control, full nutrients, technicality, contamination resistance and 20 extra points of AC */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Newfound And Useful",				IMPLANT_OF_FREEDOM, /* free action; if in a form without hands, also gives swimming, magical breathing and protects items from becoming wet */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Magical Purpose",				RIN_IMMUNITY_TO_DRAWBRIDGES,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Luxidream's Ascension",				HELM_OF_BEGINNER_S_LUCK, /* 10% chance of life saving and 10% increased speed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Good Guys Always Win", 		GREEN_LIGHTSABER,
	(SPFX_RESTR), 0, 0,
	PHYS(7,6),	NO_DFNS,	NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L), 

A("Nex-Xus", 		VIOLET_LIGHTSABER, /* teleportitis */
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	PHYS(5,2),	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L), 

A("Litebane", 		WHITE_LIGHTSABER,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK|SPFX_DEFN), 0, 0,
	STUN(3,4),	DFNS(AD_MAGM),	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L),

A("Juupad Style", 		WHITE_DOUBLE_LIGHTSABER,
	(SPFX_RESTR), 0, 0,
	PHYS(1,0),	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L), 

A("Rafschar's Superweapon",			QUARTERSTAFF, /* +1 to-hit, strength and constitution, detect monsters, banishmentitis (and if you can't be banished, your data is deleted), +1000 spawn frequency for AD_DATA monsters, all monsters can cause Ragnarok in melee and you occasionally get permanent nasty trap effects over time */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50000L ),

A("Leng's Kryptonite",			FEMININE_PUMPS, /* +50 damage to elder priests */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Cure Hassia Course",			LEATHER_SADDLE, /* both you and your steed regenerate faster */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L), 

A("Western Frankish Course",			LEATHER_SADDLE, /* controlled teleportitis and levelteleportitis */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L), 

A("Amy's Sex Toy",			STICKSHOE, /* 1 in 100 monsters spawn with seducer egotype */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L),

A("Ether Penetrator",			GREAT_POLLAX, /* even bigger thrusting range but also contaminates you faster */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5,8),	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L), 

A("Fuurker",			GARDEN_FORK, /* extra range, but while you wield it, the game is in evilvariant mode */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L), 

A("Overlong Stick",			HELMET_BEARD, /* extra range, but the minimum thrusting range is also increased */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L), 

A("Japanese Women",			THRESHER, /* no minimum range, but you have to follow the samurai's code of conduct and your STR and CON are capped at 14 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L), 

A("Plaidswandir",		LEATHER_SABER, /* suggested by K2 */
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Rip Strategy",		INKUTLASS, /* -5 accuracy/damage, +5 INT/CON, killing a monster with it heals you */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Tooth of Shai-hulud",	CRYSKNIFE, /* clairvoyance when wielded */
	(SPFX_RESTR|SPFX_INTEL|SPFX_WARN), 0, 0,
	PHYS(2,4),	NO_DFNS,	NO_CARY, 0,	A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Dolores' Virginity",	SPECIAL_SHIELD, /* totter and hyperbluewalls, autocurses when worn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY, 0,	A_NONE, NON_PM, NON_PM, 2000L ),

A("Jonadab's Brainstorming",	AKLYS, /* bonus against S_DEMON too */
	(SPFX_RESTR|SPFX_DCLAS), 0, (S_LICH),
	PHYS(1, 0),	NO_DFNS,	NO_CARY, 0,	A_NONE, NON_PM, NON_PM, 500L ),

A("Lynn's Fleecy Wish",	CRYSTAL_PLATE_MAIL,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_HSPDAM), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY, 0,	A_NONE, NON_PM, NON_PM, 5000L ),

A("Staff of Saint Patrick",		QUARTERSTAFF,
	(SPFX_RESTR|SPFX_LUCK|SPFX_DCLAS), 0, S_SNAKE,
	PHYS(5, 20),	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L), 

A("Ogresmasher 2.0",		WAR_HAMMER, /* stun and confusion resistance when wielded, by jonadab */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_OGRE,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Bashcrash",		CROSSBOW, /* by jonadab - deals extra damage when bashing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L), 

A("You're Stucco",		GLAIVE, /* can never be applied successfully, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(8,10),	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L), 

A("Lucky Shards",		DROVEN_LANCE, /* if you manage to break it by jousting, your maximum lance skill increases */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L), 

A("Royal Casino Bets",		WOODEN_BAR, /* extremely low chance to gain enchantment when applying it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L), 

A("Everything Well Protected",		SOFT_CHASTITY_BELT,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK,	DFNS(AD_MAGM),	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L), 

A("Multicharge",		CHARGER, /* only has a 1 in 5 chance of being used up when applied */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L), 

A("Suck The Mind Flayer",		INTELLIGENCE_PACK, /* increases your intelligence by 5 points */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L), 

/* below are randarts: they're supposed to be a random base item
 * melee weapons will become some other melee weapon, launchers will become
 * some other launcher, body armors will become some other body armor etc. --Amy */

A("Soulcalibur",                  LONG_SWORD,
	(SPFX_RESTR|SPFX_SEEK|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH),0,0,
	PHYS(5,10),	DRLI(0,0),	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Undeadbane",			LONG_SWORD,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_UNDEAD,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 2500L ),

A("Rainbowswandir",		LONG_SWORD,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 8000L ),

A("Wizardbane",                  LONG_SWORD,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK|SPFX_DEFN), 0, 0,
	STUN(3,4),	DFNS(AD_MAGM),	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 3500L ),

A("Vorpal Edge",		LONG_SWORD,
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	PHYS(5,2),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Dark Magic",			LEATHER_ARMOR,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Beam Control",			LONG_SWORD,
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	PHYS(2,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Sandra's Secret Weapon",			LONG_SWORD, /* aggravate monster, can cause amnesia when wielded */
	(SPFX_RESTR|SPFX_DEFN|SPFX_SEARCH|SPFX_EVIL), 0, 0,
	PHYS(0,12), DFNS(AD_ELEC), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Dumboak's Hew",			LONG_SWORD, /* blindness resistance */
	(SPFX_RESTR), 0, 0,
	PHYS(0,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Power Ammo",			DART,
	(SPFX_RESTR), 0, 0,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 100L ),

A("Blobloblobloblo",			BOW,
	(SPFX_RESTR), 0, 0,
	PHYS(0,14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Pschiuditt",			BOW,
	(SPFX_RESTR), 0, 0,
	PHYS(2,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Rattattattattatt",			BOW, /* aggravate monster */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0,16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Flam0r",			BOW,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK), 0, 0,
	FIRE(10,2), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Sureshot",			BOW,
	(SPFX_RESTR), 0, 0,
	PHYS(20,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 9900L ),

A("Stingwing",			DART,
	(SPFX_RESTR), 0, 0,
	PHYS(7,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 70L ),

A("Nobile Mobility",			VICTORIAN_UNDERWEAR, /* "nobile" is intentional --Amy */
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Antimagic Field",			VICTORIAN_UNDERWEAR, /* prevents spellcasting */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Natalia Is Lovely But Dangerous",			VICTORIAN_UNDERWEAR, /* polymorph control and manaleech */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Tape Armament",			LEATHER_ARMOR, /* superscrolling screen effect */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Cathan's Sigil",			RIN_AGGRAVATE_MONSTER, /* increases STR by its enchantment value +3 */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3400L ),

A("Fleeing Mine Mail",			LEATHER_ARMOR, /* autocurses when worn, disables fire resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Grey Fuckery",			LEATHER_ARMOR, /* shades of grey effect */
	(SPFX_RESTR|SPFX_DEFN|SPFX_WARN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Little Penis Wanker",			LEATHER_ARMOR, /* prevents you from contracting STD */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("D-Type Equipment",			LEATHER_ARMOR, /* prevents lava from harming player */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Incredible Sweetness",			LEATHER_ARMOR,
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Queen Articuno's Hull",			LEATHER_ARMOR, /* aggravate monster and conflict */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 20000L ),

A("Don Suicune Used Selfdestruct",			LEATHER_ARMOR, /* aggravate monster and random nasty effects */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Wondercloak",			ORCISH_CLOAK,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Eveline's Civil Mantle",			ORCISH_CLOAK,
	(SPFX_RESTR|SPFX_DEFN|SPFX_STLTH|SPFX_LUCK), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Ina's Overcoat",			ORCISH_CLOAK, /* hunger, random fainting, and disintegration/sickness resistance; autocurses */
	(SPFX_RESTR|SPFX_DEFN|SPFX_SPEAK|SPFX_SEEK|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Groundbummer",			ORCISH_CLOAK, /* disables the flying intrinsic, aggravates monsters, and causes intrinsic freezing; autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5L ),

A("Rita's Lovely Overgarment",			ORCISH_CLOAK, /* autocurses and sets its enchantment to -10 when worn! */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Lunar Eclipse Tonight",			ORCISH_CLOAK,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("Lorskel's Speed",			STANDARD_HELMET, /* makes the player very fast and adds extra speed sometimes */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("106Dot1 Alien Radio",			STANDARD_HELMET, /* works like radio helmet */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Nadja's Darkness Generator",			STANDARD_HELMET, /* autocurses, creates darkness once every 100 turns, but improves your to-hit by 5 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Tha Wall",			LEATHER_ARMOR, /* 9 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 900L ),

A("LLLLLLLLLLLLLM",			STANDARD_HELMET, /* LLM messages, free action and drain resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Arabella's Great Banisher",			REGULAR_GLOVES, /* levelteleportitis, heavily curse themselves when worn, disables teleport control */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("No Future But Agony",			SHIELD, /* autocurses, aggravates monsters and causes conflict */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 6000L ),

A("Bonus Hold",			SHIELD, /* autocurses, grants conflict and sustain ability */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 12000L ),

A("The Grexit Is Near",			SHIELD, /* autocurses, causes the "verisiert" effect and gives keen memory */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 12000L ),

A("Real Men Wear Psychos",			SHIELD, /* autocurses, grants psi resistance and the hate trap and farlook bug effects */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 12000L ),

A("AmyBSOD's New Footwear",			LOW_BOOTS, /* cause blood loss */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Manuela's Unknown Heels",			LOW_BOOTS, /* aggravate monster, conflict, heavily curse themselves when worn, count as high heels */
	(SPFX_RESTR|SPFX_DEFN|SPFX_ESP|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Hades the Meanie",			LOW_BOOTS, /* aggravate monster, unbreathing, monsters are always spawned hostile */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Amy Loves Autocursing Items",			LOW_BOOTS, /* autocurses :-P, causes random itemcursing */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_SEEK|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("AllyNone",			LOW_BOOTS, /* conflict, unbreathing, aggravate monster */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Khor's Required Idea",			LOW_BOOTS, /* free action, auto destruct */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Error In Play Enchantment",			LOW_BOOTS, /* polymorphitis, teleportitis, regeneration, speed bug */
	(SPFX_RESTR|SPFX_DEFN|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Whoa Hold On Dude",			RIN_AGGRAVATE_MONSTER,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("The Achromantic Ring",			RIN_AGGRAVATE_MONSTER, /* disintegration resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("GoldenIvy's Engagement Ring",			RIN_AGGRAVATE_MONSTER, /* aggravate monster, teleportitis, sickness resistance, flying */
	(SPFX_RESTR|SPFX_TCTRL|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Tyranitar's Own Game",			AMULET_OF_STRANGULATION, /* prism reflection */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("One Moment In Time",			AMULET_OF_STRANGULATION, /* warp reflection, resist psi/stun/petrification/sickness, nastiness */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Bueing",			AMULET_OF_STRANGULATION, /* sight bonus, poison resistance, rmb loss */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Nazgul's Revenge",			AMULET_OF_STRANGULATION, /* heavily curses itself, disables drain resistance, gives free action and manaleech, anti-experience */
	(SPFX_RESTR|SPFX_REGEN|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Harry's Blackthorn Wand",			WAN_MAKE_INVISIBLE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Professor Snape's Dildo",			WAN_MAKE_INVISIBLE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("French Magical Device",			WAN_MAKE_INVISIBLE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Saggittii",		DART, /* Yes this misspelling is intentional! --Amy */
	(SPFX_RESTR), 0, 0,
	PHYS(8,6),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),

A("Bentshot",   BOW,
    SPFX_RESTR, 0, 0,
    PHYS(10,2),     NO_DFNS,    NO_CARY,    0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),
    
A("Jellywhack", LONG_SWORD,
    (SPFX_RESTR|SPFX_DCLAS|SPFX_HALRES), 0, S_JELLY,
    PHYS(10,0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 400L ),
    
A("One Through Four Scepter", LONG_SWORD, /* trap of walls and +10 difficulty effect when wielded */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_HALRES|SPFX_REGEN|SPFX_DALIGN|SPFX_WARN|SPFX_HSPDAM|SPFX_EVIL), 0, 0,
	PHYS(5,0),	DFNS(AD_DRLI),	NO_CARY,
	0,		A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("Al Canone", BOW,
	(SPFX_RESTR|SPFX_DEFN|SPFX_LUCK|SPFX_INTEL|SPFX_WARN|SPFX_STLTH), 0, 0,
	PHYS(5,6),        DFNS(AD_FIRE),  NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Veil of Ministry",	ORCISH_CLOAK, /* drain res, superscroller, black ng walls and permanent confusion, heavily autocurses when worn */
	(SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK,	DFNS(AD_MAGM),	NO_CARY,
	0, A_NEUTRAL, NON_PM, NON_PM, 1000L),

A("Zankai Hung Ze Tung Do Hai",		LONG_SWORD, /* very fast when wielded, but massively increases hunger and damages the wielder */
	(SPFX_RESTR|SPFX_HSPDAM|SPFX_EVIL), 0, 0, 
	PHYS(1,0),	NO_DFNS,	NO_CARY,
	0, A_NEUTRAL, NON_PM, NON_PM, 4444L),

A("Awkwardness",		LONG_SWORD,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(1,2),	FIRE(0,0),	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 8000L),

A("Schwanzus Langus",			LONG_SWORD, /* stun, confusion, hallu and freezing when wielded */
	(SPFX_RESTR|SPFX_DEFN|SPFX_HSPDAM|SPFX_HPHDAM|SPFX_DEFN|SPFX_REFLECT|SPFX_EVIL), 0, 0, //needs quote
	NO_ATTK,	DFNS(AD_MAGM),	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 1500L), 

A("Trap Dungeon of Shambhala",	VICTORIAN_UNDERWEAR, /* sets itself to +10 when worn, but has create traps and random bad effects */
	(SPFX_RESTR|SPFX_INTEL|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0, A_NEUTRAL, NON_PM, NON_PM, 4500L), 

A("Zero Percent Failure",		STANDARD_HELMET, /* makes spellcasting easier */
	(SPFX_RESTR|SPFX_HSPDAM), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 4000L),

A("Henrietta's Heavy Caster",	ORCISH_CLOAK, /* spellcasting chance bonus, aggravate monster :D */
	(SPFX_RESTR|SPFX_INTEL|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY, 
	0,	A_CHAOTIC,	 NON_PM, NON_PM, 5000L),

A("The ROFLcopter Web",	LEATHER_ARMOR,  /* spellcasting chance bonus, magic resistance and half speed when worn */
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_INTEL|SPFX_WARN|SPFX_EVIL), 0, M2_ELF,
	NO_ATTK,	DRLI(0,0),	NO_CARY,
	0,	A_CHAOTIC, NON_PM, NON_PM, 2000L), 

A("ShivanHunter's Unused Prize",			LEATHER_ARMOR, /* displacement, blood mana, stun */
	(SPFX_RESTR|SPFX_DEFN|SPFX_REFLECT|SPFX_INTEL|SPFX_EVIL), 0, 0,
	NO_ATTK,	DFNS(AD_MAGM),	NO_CARY,	
	0, A_NONE, NON_PM, NON_PM, 9000L), 

A("Arabella's Artifact Creation Result",	LONG_SWORD, /* aggravate monster, recurring disenchantment and itemcursing when wielded */
	(SPFX_RESTR|SPFX_INTEL|SPFX_EREGEN|SPFX_BEHEAD|SPFX_LUCK|SPFX_SEARCH|SPFX_SEEK|SPFX_EVIL), 0, 0,
	PHYS(20,12),	NO_DFNS,	NO_CARY,
	0,		A_LAWFUL, NON_PM, NON_PM, 4500L), 

A("The Tiara of Amnesia", STANDARD_HELMET, /* guess what the effect of this thing is :-P --Amy */
	(SPFX_RESTR|SPFX_INTEL|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 9999L),

A("Flue Flue Flueflue Flue", STANDARD_HELMET, /* undead warning, flying */
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 9999L),

A("Lixertypie", LONG_SWORD,
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_DFLAG2), 0, (M2_HUMAN|M2_ELF|M2_DWARF),
	PHYS(9,18),	DFNS(AD_MAGM),	NO_CARY,
	0,	A_LAWFUL, NON_PM, NON_PM, 9999L), 

A("The Sameness of Chris",			LONG_SWORD, /* blindness resistance */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH), 0, 0,
	PHYS(7,10),	DFNS(AD_BLND),	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 7500L),

A("Donald Trump's Rage",                  LONG_SWORD,
	(SPFX_RESTR|SPFX_BEHEAD|SPFX_DCLAS|SPFX_DRLI), 0, S_HUMAN,
	DRLI(5,2),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Prick Pass",                    LONG_SWORD,
	SPFX_DFLAG2, 0, M2_ELF,
	PHYS(5,16),     NO_DFNS,        NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 300L ),

A("Thranduil Lossehelin",				LONG_SWORD, /* heavily curses itself, bloodthirsty, 1 in 20000 chance per turn that it disintegrates, all glyphs are fleecy-colored */
	(SPFX_RESTR|SPFX_DRLI|SPFX_EVIL|SPFX_ATTK), 0, 0,
	DRLI(5,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Feanaro Singollo",				LONG_SWORD, /* confusion and speed while wielded */
	(SPFX_RESTR|SPFX_STLTH|SPFX_EVIL|SPFX_ATTK), 0, 0,
	FIRE(10,12), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Winsett's Big Daddy",				BOW, /* +rnd(2) multishot */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Feminist Girl's Purple Wear",				VICTORIAN_UNDERWEAR, /* +5 CHA if you're female, -5 to all stats otherwise */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Lea's Spokeswoman Uniform",				ORCISH_CLOAK, /* heavily autocurses (and does nothing else) because Lea is devious :P */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Heretical Figure",				ORCISH_CLOAK, /* anti caster effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Jana's Secret Car",				ORCISH_CLOAK, /* speed, unbreathing, but bumping into an invisible monster has 1 in 100 chance to instakill you (no joke). */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Unimplemented Feature",				STANDARD_HELMET, /* confusion, DSTW effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Flat Insanity",				STANDARD_HELMET, /* inventorylessness, autocurses when worn */
	(SPFX_RESTR|SPFX_DEFN|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2500L ),

A("Freeze Or Yes",				REGULAR_GLOVES, /* player acquires intrinsic freezopathy from putting them on */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Princess Bitch",				REGULAR_GLOVES, /* sitting on a throne always gives the princess bitch effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Would You Raight That",				REGULAR_GLOVES, /* unlocks searching skill and caps it at expert but also prime curses itself if you didn't have the skill before */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Difficulty 5",				REGULAR_GLOVES, /* flying, monsters often create a trap upon spawning, monster spawn frequency doubled and difficulty + (6 + your experience level) */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Swarm-Soft High Heels",				LOW_BOOTS, /* count as high heels */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Weak From Hunger",				LOW_BOOTS, /* conflict, weakness effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Arabella's Resist Cold",				RIN_AGGRAVATE_MONSTER, /* autocurses your entire inventory if you put it on, bank trap effect */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Ratsch Watsch",				AMULET_OF_STRANGULATION, /* deactivates tele control for 1 million turns when put on */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Arabella's Precious Gadget",				AMULET_OF_STRANGULATION, /* banking effect, silently autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Arabella's Warding Hoe",				LONG_SWORD, /* silently curses itself with TY curse when wielded, half speed */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_PROTEC|SPFX_EVIL|SPFX_ATTK), 0, 0,
	STUN(5,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Shapetake Number Five",				ORCISH_CLOAK, /* polymorphs never time out, but also disables poly control and gives polymorphitis */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Arabella's Wand Booster",				ORCISH_CLOAK, /* ancient morgothian curse, antimagic, curse items; silently curses itself heavily */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Intelligent Pope",				BOW, /* every time you trigger a trap, its enchantment randomly goes either up or down, but cannot go below -20 or above +10. BUC status determines if it's more likely to go up or down */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("RNG's Pride",				LEATHER_ARMOR, /* if you put it on while it's +0, it will randomly set itself to something between -5 and +5 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("RNG's Joy",				REGULAR_GLOVES, /* if you put it on while it's +0, it will randomly set itself to something between -5 and +5 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("RNG's Sexiness",				LOW_BOOTS, /* if you put it on while it's +0, it will randomly set itself to something between -5 and +5 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("RNG's Embrace",				ORCISH_CLOAK, /* if you put it on while it's +0, it will randomly set itself to something between -5 and +5 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("RNG's Grimace",				STANDARD_HELMET, /* if you put it on while it's +0, it will randomly set itself to something between -5 and +5 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Complete Mon Dieu",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Agathe Bauer",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Anneliese Brown",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("I Will Think About You",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Del Olelong",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Jubeljubijeeah",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Duedelduedelduedelduedeldueueueueue",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Cause I'm A Cheater",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Batman Night",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Nikkenikkenik",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),



/* second batch of randarts - these use a different RNG to determine their base type --Amy */

A("Jana's Grave Wall",				ORCISH_CLOAK, /* speed, unbreathing, but bumping into a cloaked mimic has 1 in 100 chance to instakill you (no joke). */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Henrietta's Dogshit Boots",				LOW_BOOTS, /* aggravate monster, disables stealth, monsters are always spawned hostile and always approach the player, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Fireproof Wall",                  LONG_SWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(4,4),      FIRE(0,0),      NO_CARY,        0, A_LAWFUL, NON_PM, NON_PM, 400L ),

A("Spearblade",                LONG_SWORD,
	SPFX_RESTR, 0, 0,
	PHYS(9,2),      NO_DFNS,        NO_CARY,        0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Radiator Area",                   LONG_SWORD,
	(SPFX_RESTR|SPFX_DEFN|SPFX_DFLAG2), 0, M2_UNDEAD,
	PHYS(5,0),	DFNS(AD_BLND),	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 1500L ),

A("Jessica's Winning Strike",                LONG_SWORD,
	SPFX_RESTR, 0, 0,
	PHYS(0,8),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 1200L ),

A("Markus's Justice",           LONG_SWORD,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(5,12),     NO_DFNS,        NO_CARY,        0, A_LAWFUL, NON_PM, NON_PM, 1500L ),

A("Katharina's Melee Prowess",               LONG_SWORD,
	SPFX_RESTR, 0, 0,
	PHYS(3,10),     NO_DFNS,        NO_CARY,        0, A_LAWFUL, NON_PM, NON_PM, 300L ),

A("Lichbane",                 LONG_SWORD,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_LICH,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 200L ),

A("Forked Tongue",                  LONG_SWORD,
	SPFX_RESTR, 0, 0,
	PHYS(2,6),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 300L ),

A("All Seriousness",                    LONG_SWORD,
	SPFX_RESTR, 0, 0,
	PHYS(3,6),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),

A("Special Lack",                  LONG_SWORD,
	SPFX_RESTR, 0, 0,
	PHYS(0,10),     NO_DFNS,        NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Wing Wing",				LONG_SWORD,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(2,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Carmoufalsch",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Wie Es Auch Sei",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Morton Theirs of Ravel Radio",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Deep Friends",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Hae-hae hiiii",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("For Mommy Ever Forselessault",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Lawfire",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Wae wae wae daedeldaedeldaedellodeledeledelel",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("President Schiesskanista",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Knbloeloeloelodrio",				SPE_MAGICTORCH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Desert Maid",			LONG_SWORD, /* kurwa claw weakness and dora trap effect when wielded */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0,20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Cygniswan",			LONG_SWORD,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Talkator",			LONG_SWORD,
	(SPFX_RESTR|SPFX_SPEAK), 0, 0,
	PHYS(2,6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Magesooze",			LONG_SWORD,
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("Resistant Resistor",			LONG_SWORD,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Vernon's Potterbasher",			LONG_SWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(4,16), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Schwoingeloingeloing oooar",			LONG_SWORD,
	(SPFX_RESTR|SPFX_SPEAK), 0, 0,
	PHYS(24,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 240L ),

A("Femmy's Lash",			LONG_SWORD,
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_WARN), 0, M2_MALE,
	PHYS(5,24), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Casque Outlook",			LONG_SWORD,
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	PHYS(2,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Unfair Pee",			LONG_SWORD,
	(SPFX_RESTR|SPFX_DEFN|SPFX_WARN), 0, 0,
	PHYS(2,12), DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Sea Captain Piercer",			LONG_SWORD,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	PHYS(2,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("End Of Look Worse",			LONG_SWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(2,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Space Begins After Here",			LONG_SWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(2,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Corina's Thunder",			LONG_SWORD,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(2,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Inner Tube",			LONG_SWORD,
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Solo Slacker",			BOW,
	(SPFX_RESTR), 0, 0,
	PHYS(1,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Ammo of the Machine",			DART,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_GOLEM,
	PHYS(10,40), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Dae-oe-oe-oe-oe-oe",			BOW,
	(SPFX_RESTR), 0, 0,
	PHYS(0,14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Cannoneer",			BOW,
	(SPFX_RESTR), 0, 0,
	PHYS(2,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Speedhack",			BOW, /* makes the player very fast when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Earth Gas Gun",			BOW,
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	PHYS(0,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3600L ),

A("Fire Already",			DART,
	(SPFX_RESTR), 0, 0,
	PHYS(2,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Human Wipeout",			LONG_SWORD,
	(SPFX_RESTR|SPFX_BEHEAD|SPFX_DCLAS), 0, S_HUMAN,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Splinter Armament",			LEATHER_ARMOR, /* superscrolling screen effect */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Absolute Monster Mail",			LEATHER_ARMOR, /* autocurses when worn, disables fire resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Rita's Tender Stilettos",			LOW_BOOTS, /* counts as high heels, but also autocurses and sets its enchantment to -10 when worn! */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Half Moon Tonight",			ORCISH_CLOAK,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("Pantap",			ORCISH_CLOAK,
	(SPFX_RESTR|SPFX_ESP|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Ruth's Dark Force",			STANDARD_HELMET, /* autocurses, creates darkness once every 100 turns, but improves your to-hit by 5 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Hamburg One",			STANDARD_HELMET, /* hunger */
	(SPFX_RESTR|SPFX_HALRES|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Arabella's Melee Power",			LONG_SWORD, /* nastiness, spawns a Gunnhild when wielded and also 1 in 1000 chance to spawn another one if you keep wielding it */
	(SPFX_RESTR|SPFX_EVIL|SPFX_DCLAS), 0, S_HUMAN,
	PHYS(10,20), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 8000L ),

A("Asbestos Material",			LONG_SWORD, /* always poisoned */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Tanks A Lot",			REGULAR_GLOVES, /* glibbery hands, 5 extra points of AC */
	(SPFX_RESTR|SPFX_REGEN|SPFX_EREGEN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Dimvision",			REGULAR_GLOVES, /* putting them on for the first time gives you the eddy wind technique but also permanent weak sight! */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("I'm Getting Hungry",			SHIELD, /* 20% boost to block rate, free action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("CCC CCC CCCCCCC",			SHIELD, /* causes confusion, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Five Star Party",			SHIELD, /* autocurses, causes the uninformation effect and gives confusion/stun resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 12000L ),

A("Gudrun's Stomping",			LOW_BOOTS,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Goeffelboeffel",			LOW_BOOTS, /* half speed */
	(SPFX_RESTR|SPFX_REGEN|SPFX_ESP|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Temperator",			LOW_BOOTS, /* resist fire */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Green Color",			RIN_AGGRAVATE_MONSTER, /* poison resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Scarab of Adornment",			RIN_AGGRAVATE_MONSTER, /* +10 charisma */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Schwueu",			AMULET_OF_STRANGULATION, /* teleporting items */
	(SPFX_RESTR|SPFX_TCTRL|SPFX_ESP|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Fully The Long Penis",			WAN_MAKE_INVISIBLE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("World of Coq",			WAN_MAKE_INVISIBLE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Whooshzap",			WAN_MAKE_INVISIBLE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Nadja's Silence",			LONG_SWORD,
	(SPFX_RESTR|SPFX_STLTH),0,0,
	PHYS(5,6),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("A Sword Named Sword",		LONG_SWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(8,6),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),

A("Heritage Ignorer",		LONG_SWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(4,4),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Mimicbane", LONG_SWORD,
    (SPFX_RESTR|SPFX_DCLAS|SPFX_HALRES), 0, S_MIMIC,
    PHYS(10,0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 400L ),

A("Hahahahahahahaaaaaaaaaaaa",		LONG_SWORD,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	COLD(3,6),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),
    
A("Poison Pen Letter",	LONG_SWORD, /* poisons you each turn you wield it */
	(SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DRLI|SPFX_HPHDAM|SPFX_REGEN|SPFX_EVIL), 0, 0,
	DRLI(9,10),	NO_DFNS,	NO_CARY,
	0,	A_NEUTRAL, NON_PM, NON_PM, 2500L),

A("Sunali's Summoning Storm",			LOW_BOOTS, /* improve spellcasting chances by 15% */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Filthy Press",		BOW, /* messages often get replaced by random ones */
	(SPFX_RESTR|SPFX_SEEK|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH|SPFX_HALRES|SPFX_EVIL), 0, 0,
	PHYS(5,10),	DRLI(0,0),	NO_CARY,	
	0, A_LAWFUL, NON_PM, NON_PM, 8000L), 

A("Mub Puh Mub Dit Dit",			LONG_SWORD, /* blindness resistance */
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH), 0, 0,
	PHYS(7,10),	DFNS(AD_BLND),	NO_CARY,
	0, A_LAWFUL, NON_PM, NON_PM, 7500L),

A("Donnnnnnnnnnnng",				LONG_SWORD, /* every time you hit with it, there's a 33% chance that its enchantment goes down by 1 unless it's already -20 or worse */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0,40), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Provocateur",				LONG_SWORD, /* conflict while wielded */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Foeoeoeoeoeoeoe",				BOW, /* +rnd(3) multishot, projectiles misfire 1 out of 7 times */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Never Will This Be Useful",				LONG_SWORD, /* +4 damage per trident skill */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Quarry",				LEATHER_ARMOR, /* +5 AC; ammos made of mineral have 50% extra chance of not breaking */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Conny's Combat Coat",				ORCISH_CLOAK, /* heavily autocurses, your kicks do 5 extra points of damage and have 10% chance of causing paralysis and stun */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Acidshock Castlecrusher",				ORCISH_CLOAK, /* resist shock and acid, 5 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Laura's Swimsuit",				ORCISH_CLOAK, /* works like an oilskin cloak, also swimming and unbreathing, but -5 AC */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Protect What Cannot Be Protected",				ORCISH_CLOAK, /* allows you to erodeproof a nonerodable object with 1 in 5000 chance per turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Girlful Farting Noises",				STANDARD_HELMET, /* spawns a farting monster 1 in 1000 turns, flying */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("YOU SEE HERE AN ARTIFACT",				STANDARD_HELMET, /* bigscript */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 700L ),

A("Numb Or Maybe",				REGULAR_GLOVES, /* player acquires intrinsic numbopathy from putting them on 50%, gets a random bad effect 49% or acquires intrinsic yellow spells 1% */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Dead-Slam The Time Shut",				REGULAR_GLOVES, /* unlocks device skill and caps it at expert but also prime curses itself if you didn't have the skill before */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Anastasia's Unexpected Ability",				LOW_BOOTS, /* count as high heels, autocurses, teaches you the attire charm and world fall techniques if you didn't have them, but gives 3 or 9 random nasty properties as intrinsic */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Eliane's Shin Smash",				LOW_BOOTS, /* kicking a monster will always paralyze it and do double damage, and heaps of shit don't affect you and you cannot have wounded legs, but if you ever come into contact with water or something farts, they are vaporized instantly. */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 25000L ),

A("Mysterious Magic",				AMULET_OF_STRANGULATION, /* weak effects */
	(SPFX_RESTR|SPFX_EREGEN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("BangCock",				BOW, /* every time you trigger a trap, its enchantment randomly goes either up or down, but cannot go below -20 or above +10. BUC status determines if it's more likely to go up or down */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("RNG's Completion",				LEATHER_ARMOR, /* if you put it on while it's +0, it will randomly set itself to something between -7 and +7 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("RNG's Beauty",				LOW_BOOTS, /* if you put it on while it's +0, it will randomly set itself to something between -7 and +7 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("RNG's Safeguard",				SHIELD, /* if you put it on while it's +0, it will randomly set itself to something between -7 and +7 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Black Darkness",				AMULET_OF_STRANGULATION, /* every glyph is black (like blacky trap) */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Fleecy Green",				AMULET_OF_STRANGULATION, /* every glyph is green */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Peek",                    LONG_SWORD,
	SPFX_DFLAG2, 0, M2_ELF,
	PHYS(5,16),     NO_DFNS,        NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 300L ),

A("Tailcutter",			LONG_SWORD,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_WORM_TAIL,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	0,		A_NEUTRAL,	NON_PM, NON_PM, 2000L ),


/* below here we have artifacts that depend on a randomized appearance --Amy */

A("Phantom Of The Opera",				ORCISH_CLOAK, /* initialized to always be an opera cloak; 5 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("High Desire of Fatality",				LOW_BOOTS, /* initialized to always be explosive boots; very fast speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Choice of Matter",				LOW_BOOTS, /* initialized to always be irregular boots; cold resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Melissa's Beauty",				LOW_BOOTS, /* initialized to always be wedge boots; +10 charisma, +5 AC and +5 to-hit */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Corina's Snowy Tread",				LOW_BOOTS, /* initialized to always be winter stilettos; cold resistance, prevents potions from freezing, aggravate monster, the disarm technique works even if you're not skilled with your weapon */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Number 23",				STANDARD_HELMET, /* initialized to always be an aluminium helmet; psi resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Hauntnight",				ORCISH_CLOAK, /* initialized to always be a ghostly cloak; monsters are always spawned permanently invisible */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Lorskel's Special Protection",				REGULAR_GLOVES, /* initialized to always be polnish gloves; AD_SITM fails 4 out of 5 times */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Robbery Gone Right",				REGULAR_GLOVES, /* initialized to always be polnish gloves; random gold spawns have 3 times more gold in them */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6000L ),

A("Josefine's Evilness",				LOW_BOOTS, /* initialized to always be velcro boots; resist fire, cold, shock and psi, random fainting */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("Whiny Mary",				REGULAR_GLOVES, /* initialized to always be clumsy gloves; while wearing them, your weapon automatically curses itself every turn, but ranged attacks get +rnd(5) multishot */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2400L ),

A("Warp Speed",				LOW_BOOTS, /* initialized to always be fin boots; your speed is multiplied by 6 while you're on a water square */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Greneuvenia's Hug",				LOW_BOOTS, /* initialized to always be profiled boots; flying, fire resistance and improved vision, but displays a demonic taunt once per turn to drive the player nuts :-P */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Shelly",				ORCISH_CLOAK, /* initialized to always be a void cloak; antimagic on turns not divisible by 3 */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Spread Your Legs Wide",				REGULAR_GLOVES, /* initialized to always be grey-shaded gloves; sets your AC to 10, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Greeb",				ORCISH_CLOAK, /* initialized to always be a weeb cloak; attracts monsters that are green or bright green once every 2000 turns */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Prince of Persia",				LOW_BOOTS, /* initialized to always be persian boots; jumping, 50% chance of life saving while wearing them */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 8000L ),

A("Anastasia's Playfulness",				LOW_BOOTS, /* initialized to always be hugging boots; spawns a shit trap somewhere on the level once every 1000 turns, and you will trigger them even if you fly */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Katie Melua's Female Weapon",				LOW_BOOTS, /* initialized to always be fleecy boots; count as high heels */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Cockuetry",				REGULAR_GLOVES, /* initialized to always be fingerless gloves; petrification resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("PercentIoeoepsPercentD-thief",				ORCISH_CLOAK, /* initialized to always be a mantle of coat; thievery effect, stairs traps, monsters respawn when killed, heavily autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Peeping Groove",				REGULAR_GLOVES, /* initialized to always be fatal gloves; if you use a shotgun while wearing them, you get +rnd(7) multishot */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Rare Asian Lady",				LOW_BOOTS, /* initialized to always be beautiful heels; resist cold, +20 charisma, can always resist if seducing monsters try to take off your gear */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 12000L ),

A("Jana's Fairness Cup",				ORCISH_CLOAK, /* initialized to always be a homicidal cloak; speed, flying, but triggering a trap has 1 in 100 chance to instakill you (no joke). */
	(SPFX_RESTR|SPFX_DEFN|SPFX_STLTH|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6000L ),

A("Out Of Time",				LOW_BOOTS, /* initialized to always be castlevania boots; +5 strength and dexterity, turn limitation effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Paleolithic Elbow Contractor",				ORCISH_CLOAK, /* initialized to always be a greek cloak; +5 multishot with a bow, unskilled effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2500L ),

A("Nuclear Bomb",				STANDARD_HELMET, /* initialized to always be a celtic helmet; newly generated golems always have the exploder and bomber egotypes */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 20000L ),

A("Beeeeeeeanpole",				REGULAR_GLOVES, /* initialized to always be english gloves; +5 range for arrows fired from bows */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Legma",				LOW_BOOTS, /* initialized to always be korean sandals */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("Terry Pratchett's Ingenuity",				ROBE, /* initialized to always be an octarine robe; prism reflection whenever it triggers */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Arabella's Sexy Girl Butt",				ORCISH_CLOAK, /* initialized to always be a chinese cloak; tools need to be wielded if you want to apply them, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Long-Lasting Joy",				ORCISH_CLOAK, /* initialized to always be a polyform cloak; your polymorphs last 2-3 times longer */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Light Absorption",				ORCISH_CLOAK, /* initialized to always be an absorbing cloak */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, LIGHT_AREA, A_NONE, NON_PM, NON_PM, 8000L ),

A("Catherine's Sexuality",				ORCISH_CLOAK, /* initialized to always be a birthcloth; if you have children while wearing it, you instantly die of complications. */
	(SPFX_RESTR|SPFX_HSPDAM|SPFX_HPHDAM|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 3500L ),

A("Pokewalker",				ORCISH_CLOAK, /* initialized to always be a poke mongo cloak; displays all pokemon on the level */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Winds of Change",				ORCISH_CLOAK, /* initialized to always be a levuntation cloak; confusing problem, occasionally increases movement speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Lightspeed Travel",				ORCISH_CLOAK, /* initialized to always be a quicktravel cloak; nomul(0,0) will not do anything, blinking speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("T-O-M-E",				ORCISH_CLOAK, /* initialized to always be an angband cloak; upon entering a level you've not been to yet, you will either get a princess or fumblefingers effect. */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Femmy Fatale",				ORCISH_CLOAK, /* initialized to always be an anorexia cloak; weakness problem, diarrhea, monsters do not spawn corpses, slow digestion */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 8000L ),

A("Artificial Fake Difficulty",				ORCISH_CLOAK, /* initialized to always be a dnethack cloak; weaker form of failed existence effects, halves experience point gains, techniques become re-usable in half the usual amount of time */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Junethack 2016 Winner",				ORCISH_CLOAK, /* initialized to always be a team splat cloak; every time you get a trophy while wearing it, you gain +10 max HP and Pw */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Yog-Sothoth Help Me",				ORCISH_CLOAK, /* initialized to always be an eldritch cloak; psi resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2400L ),

A("Softness of Teleportation",				INKA_LEASH,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Joy Ride",				INKA_SADDLE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Fissility",				UNSTABLE_STETHOSCOPE, /* 10x higher chance of breakage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Blocking Extreme",				PAPER_SHIELD, /* 10% extra chance to block */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Ewscratch",				SECRETION_DAGGER,
	(SPFX_RESTR), 0, 0,
	PHYS(2,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 140L ),

A("Tarmac Champion",				BITUKNIFE, /* very fast speed when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Gauge-O-Meter",				MEASURER,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	STUN(2,4), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2500L ),

A("U Are A Cheater",				COLLUSION_KNIFE,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Cool Chamber",				SOFT_MATTOCK,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	COLD(4,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Animalbane",				INKA_BLADE, /* warns of animals */
	(SPFX_RESTR|SPFX_DFLAG1), 0, M1_ANIMAL,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Always Hit For Little Damage",				PAPER_SWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(10,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Grinder",				MEATSWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(4,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Ouchfire",				ICKY_BLADE, /* you take maximum damage */
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK|SPFX_EVIL), 0, 0,
	FIRE(1,16), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("Antivamp Whoosh",				GRANITE_IMPALER,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_VAMPIRE,
	PHYS(8,20), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Resistomatic",				ORGANOBLADE, /* resist shock and acid */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Goodie of Use",				BIDENHANDER,
	(SPFX_RESTR), 0, 0,
	PHYS(0,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Arrrrrr Matey",				INKUTLASS, /* korsair speak when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(0,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Spambait Fire",				HOE_SABLE, /* +2 increase damage */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(0,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Arabstreet Sound",				YATAGAN, /* 1 in 50 monsters spawns with sounder egotype */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ATTK), 0, 0,
	FIRE(6,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Halluduckdir",				PLATINUM_SABER,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Whackdock",				NATURAL_STICK,
	(SPFX_RESTR), 0, 0,
	PHYS(2,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Jonadab's Idea Generator",				POURED_CLUB,
	(SPFX_RESTR), 0, 0,
	PHYS(0,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Rattleclinker",				DIAMOND_SMASHER,
	(SPFX_RESTR), 0, 0,
	PHYS(6,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("See Animals",				VERMIN_SWATTER, /* warns of animals */
	(SPFX_RESTR|SPFX_DFLAG1), 0, M1_ANIMAL,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Wild Heavy Swings",				MILL_PAIL, /* -10 to-hit */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(1,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Orangery",				RHYTHMIC_STAR, /* orange monsters are 10% spawned tame and 90% peaceful */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Melissa's Peacebringer",				FLOGGER, /* attacks twice per round without the "double attacks" disadvantage - but only if you don't dual-wield */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Manuela's Practicant Terrorizer",				RIDING_CROP, /* autocurses, aggravate monster, all monsters are spawned hostile, uberjackal effect, allows you to survive in lava */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	PHYS(5,0), DFNS(AD_FIRE), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Thor's Strike",				THUNDER_HAMMER, /* if you wield it with a strength of 25, you have +5 increase damage */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(6,12), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 8000L ),

A("Black Poison Inside",				BRIDGE_MUZZLE, /* always poisoned */
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Luisa's Charming Beauty",				INKA_BOOT, /* autocurses, spawns a shit trap 1 in 200 turns and you trigger them even if you fly, makes you nauseated 1 in 500 turns and causes diarrhea and thirst, makes you immune to being kicked in the nuts or having your breasts ripped open */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Amy's First Girlfriend",				SOFT_LADY_SHOE, /* autocurses, bonks you 1 in 500 turns */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL|SPFX_ESP|SPFX_ATTK), 0, 0,
	STUN(5,4), DFNS(AD_COLD), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Patricia's Femininity",				STEEL_CAPPED_SANDAL, /* makes you thick-skinned, bloodthirsty, less likely to lose enchantment */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 20000L ),

A("Henrietta's Mistake",				DOGSHIT_BOOT, /* replicates shit trap effect 1 in 10 turns (but only if you moved), heavily autocurses, aggaravate monster, disables stealth */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ATTK), 0, 0,
	ACID(2,16), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Teaching Stick",				IMPACT_STAFF,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(4,8), DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Wetting Weather",				TROUTSTAFF,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Mana Meter Booster",				OLDEST_STAFF, /* spells cost 10% less mana */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Grandpa",				COLOSSUS_BLADE,
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Road Trash",				TUBING_PLIERS,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ACID(2,14), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Also Matte Mask",				CHEMISTRY_SPACE_AXE, /* poison resistance, always poisoned */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Crushing Impact",				RAIN_PIPE, /* double attacks without speed penalty if you wield it without dual-wielding */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Why Do You Have Such A Little Penis",				PENIS_POLE, /* aggravate monster */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(4,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("The Longest Stick",				PIKE,
	(SPFX_RESTR), 0, 0,
	PHYS(0,20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Healhealhealheal",				PHYSICIAN_BAR, /* heals 1 HP every time you successfully hit a monster with it, doubles healup effects, or quadruples if you're a healer */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Type Of Arms Discovery",				HELMET_BEARD,
	(SPFX_RESTR), 0, 0,
	PHYS(0,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Extremely Hard Mode",				TRAFFIC_LIGHT, /* boss trap effect, uncommon monsters are no longer uncommon and high-level ones aren't either */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	PHYS(10,6), DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Equalhit",				GIANT_SCYTHE,
	(SPFX_RESTR), 0, 0,
	PHYS(15,2), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 750L ),

A("Softspire",				SILK_SPEAR,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(6,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Explosion Missile",				HOMING_TORPEDO,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(8,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("And It Keeps On Moving",				COURSE_JAVELIN, /* autocurses, pushes you back every turn (but without giving a message) */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("High Roller's Luck",				POKER_STICK, /* applying it has 1 in 100 chance to reduce enchantment by 1 (but not below 0), and 1 in 2000 chance to increase enchantment by 10 (but not if it's already +21 or higher) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Long Rod of Est",				BRONZE_LANCE,
	(SPFX_RESTR), 0, 0,
	PHYS(0,16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Minolong Elbow",				COMPOST_BOW, /* england mode when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Alchemical Prohibition",				FORBIDDEN_ARROW,
	(SPFX_RESTR), 0, 0,
	PHYS(4,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Auntie Hilda",				WILDHILD_BOW,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("Red Gas Bullet",				ODOR_SHOT,
	(SPFX_RESTR), 0, 0,
	PHYS(10,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 500L ),

A("Flysting",				PAPER_ARROW,
	(SPFX_RESTR), 0, 0,
	PHYS(5,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Streamshooter",				METAL_SLING, /* +1 multishot, higher chance of firing more than 3 shots per turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Luck Versus Bad",				INKA_SLING, /* 50% chance of avoiding bad effects when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Eloplus Stat",				PAPER_SHOTGUN, /* 1 extra point of AC when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(1,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Immobilaser",				PROCESS_CARD, /* free action when wielded */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	STUN(5,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Sniper Crosshair",				ZOOM_SHOT_CROSSBOW, /* +30 range for bolts */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Atomic Missing",				BALLISTA, /* autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0,40), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Leather-Soft Sting",				FLEECE_BOLT,
	(SPFX_RESTR), 0, 0,
	PHYS(6,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Super Effective Rock",				MINERAL_BOLT,
	(SPFX_RESTR), 0, 0,
	PHYS(0,16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Anti-Intelligence",				INKA_STINGER,
	(SPFX_RESTR|SPFX_DFLAG1), 0, (M1_HUMANOID|M1_ANIMAL),
	PHYS(10,0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 40L ),

A("Completely Off",				FLIMSY_DART, /* always misfires */
	(SPFX_RESTR), 0, 0,
	PHYS(1,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Ow Wow Wow",				SOFT_STAR,
	(SPFX_RESTR), 0, 0,
	PHYS(5,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 70L ),

A("Racer Projectile",				TAR_STAR, /* double thrown range */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Indigenous Fun",				INKA_SHACKLE, /* warns of humanoids */
	(SPFX_RESTR|SPFX_DFLAG1), 0, M1_HUMANOID,
	PHYS(5,20), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("Vest Replacement",				BULLETPROOF_CHAINWHIP, /* +5 AC when wielded. If you also wear a shield, another +5 AC and +10% chance to block. */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Whereabout of X",				SECRET_WHIP, /* invisibility when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(2,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Prismatic Shirt",				BEAUTIFUL_SHIRT, /* resist fire, cold, sleep, poison */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("Non-Bladeturner",				PARTIAL_PLATE_MAIL, /* aggravate monster */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Armor of Isildur",				RIBBED_PLATE_MAIL, /* sets itself to a random value from +1 to +10 if worn while +0 or lower */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("Essentiality Extreme",				METAL_LAMELLAR_ARMOR, /* free action when worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Nonexistance",				BAR_CHAIN_MAIL, /* no specialties */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Formula One Suit",				TAR_CHAIN_MAIL, /* adds extra speed sometimes */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Incredibly Frequent Cage",				GOTHIC_PLATE_MAIL,
	(SPFX_RESTR|SPFX_DEFN|SPFX_HPHDAM), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Mithral Cancellation",				INKA_MITHRIL_COAT, /* +1 MC, recurring disenchantment */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("Impractical Combat Wear",				SILK_MAIL, /* +5 charisma, +1 MC, reduces AC by half */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Blueform",				HEAVY_MAIL, /* 2 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("The Light of Deception",				CLOAK_OF_PEACE, /* 10% of all monsters are spawned peaceful, farlook trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("The Enemies Shall Laugh Too",				CLOAK_OF_DIMNESS, /* +10 increase accuracy */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Armor Class Wall",				ICKY_SHIELD, /* +5 AC, and one more per shield skill */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2400L ),

A("Someprotector",				HEAVY_SHIELD,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Cutting Through",				BARRIER_SHIELD, /* 5 extra points of AC, +5% chance to block */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Antinstant Death",				TARRIER, /* resist disintegration and death rays, poison cannot instakill you, warns of ants */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_ANT,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Blind Pilot",				RIN_DIMNESS, /* weak sight, -10 increase accuracy, +10 increase damage, random fainting */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 10000L ),

A("Guantanamera",				GRASS_WHISTLE, /* applying it tries to put monsters to sleep like a magic flute, but also you, even if you are sleep resistant */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Derrsch",				CONGLOMERATE_PICK,
	(SPFX_RESTR), 0, 0,
	PHYS(0,6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Steel Greater Rock",				BRONZE_PICK,
	(SPFX_RESTR), 0, 0,
	PHYS(0,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Hellbringer",				GUITAR,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK), 0, 0,
	FIRE(8,8), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Killer Piano",				PIANO, /* aggravate monster, gridbug conduct, applying it gives a permanent intrinsic nastytrap effect */
	(SPFX_RESTR|SPFX_EVIL|SPFX_BEHEAD), 0, 0,
	PHYS(6,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Soundtone FM",				RADIOGLASSES, /* sound effect trap when worn */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Stable Stunt",				EYECLOSER, /* disintegration resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Crawling From The Woodwork",				BOSS_VISOR, /* bosses spawn more often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Breather Show",				DRAGON_EYEPATCH, /* displays all AT_BREA monsters when worn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("True Grime",				BINNING_KIT, /* +5 alignment and +1 max alignment every time you remove a corpse with it, and displays your current alignment and max alignment if you do */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Supermarket Fu",				BUDO_NO_SASU, /* +d10 damage, and another +d10 if you're a supermarket cashier */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Grease Your Butt",				LUBRICANT_CAN, /* every time you apply it while it has charges will polymorph you into a farting monster */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Too Precious To Eat",				DISGUSTING_BALL,
	(SPFX_RESTR), 0, 0,
	PHYS(6,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7500L ),

A("Mmmmmmmmmmmm x 200",				ELASTHAN_CHAIN,
	(SPFX_RESTR), 0, 0,
	PHYS(20,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Arvogenia's High Heelses",				DISCONNECTED_BOOTS, /* count as high heels (DUH), disintegration resistance, infravision */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Now You Have Lost",				AMULET_OF_SCREWY_INTERFACE, /* +10 increase damage, ancient Morgothian curse */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Fingermash",				BOSS_BOOTS, /* kicking a monster that is wielding a weapon will curse that monster's weapon, and set its enchantment to +0 if it was positive */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Null the Living Database",				RARE_HELMET, /* cancels you 1 in 5000 turns */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Archeologist Song",				AMULET_OF_BONES, /* if you wear it as an archeologist, you have +2 increase damage/accuracy, +2 AC and 10% lower spellcasting failure rates */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2500L ),

A("Difficult!",				LEVELING_GLOVES, /* level difficulty is doubled */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ESP|SPFX_HSPDAM|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Luisa's Irresistible Charm",				LOW_BOOTS, /* initialized to always be erotic boots; prevents your items from being eroded 50% of the time */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Jana's Deceptive Mask",				STANDARD_HELMET, /* initialized to always be secret helmet; if a monster uncovers you, 1% chance to instantly die (no joke). */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Now It Becomes Different",				ORCISH_CLOAK, /* initialized to always be difficult cloak; monster spawn frequencies behave as if you're a mystic */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Natascha's Stroking Units",				REGULAR_GLOVES, /* initialized to always be velvet gloves; monsters attacking you take 1d10 damage, unless they need a +1 or higher weapon to hit */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 10000L ),

A("Speak to OJ",				LOW_BOOTS, /* initialized to always be sputa boots; farting monsters are spawned 90% peaceful and 25% tame, chatting to a hostile one will make it peaceful */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Due Due-Due-Due Brmmmmmmm",				STANDARD_HELMET, /* initialized to always be formula one helmet; very fast speed, and +2 AC */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Toilet Noises",				ORCISH_CLOAK, /* initialized to always be excrement cloak; monsters walking over a tile with a toilet will do... nasty things (you don't even wanna know) :D */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Line Can Play By Yourself",				REGULAR_GLOVES, /* initialized to always be racer gloves; gridbug conduct, doubles your speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Too Fast, Too Furious",				LOW_BOOTS, /* initialized to always be turbo boots; prevents you from being interrupted */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Nourofibroma",				ORCISH_CLOAK, /* initialized to always be guild cloak; free action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Madeline's Stupid Girl",				REGULAR_GLOVES, /* initialized to always be shitty gloves; 1 in 500 chance to spawn a shit trap on the level and you trigger them even if you're flying; +3 increase damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Arabella's Radar",				RIN_AUTOCURSING, /* detect monsters, dropcurses, itemcursing, heavily autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Henrietta's Magical Aid",				RIN_TIME_SPENDING, /* 50% less spellcasting failure rate, spells cost 20% less mana, teleportitis, disables teleport control, create traps effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Jonadab's Heavyload",				LOADSTONE, /* invisibility while carried */
	(SPFX_RESTR|SPFX_EVIL), 0, (SPFX_ESP),
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Hanging Call",       RADIOACTIVE_DAGGER,
    (SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
    ACID(5,4),  ACID(0,0), NO_CARY,     0, A_NEUTRAL, NON_PM, NON_PM, 400L ),

A("Blue Screen of Death",				BLACKY_HELMET, /* makes everything blue (idea by bugsniper), and if Blacky spawns, the game will also spawn a bunch of blue monsters */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Saddle of Reflection",			LEATHER_SADDLE, /* idea by Lorskel, applying it reduces wisdom */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0, A_NONE, NON_PM, NON_PM, 1000L), 

A("50-Shades-Of-Grayswandir",		SILVER_SABER, /* shades of grey effect (DUH), idea by K2 */
	(SPFX_RESTR|SPFX_HALRES|SPFX_EVIL), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 8000L ),

A("Puddingbane",			KNIFE, /* idea by jonadab */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_PUDDING,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 500L ),

A("Faded Uselessness",			KNIFE, /* evil patch idea by jonadab - autocurses, slippery fingers, hallucination */
	(SPFX_RESTR|SPFX_EVIL|SPFX_DCLAS), 0, S_IMP,
	PHYS(0,4),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 500L ),


A("Most Charismatic President",				TRUMP_COAT, /* +10 charisma, wall trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Magicaint",				CLOAK_OF_BAD_TRAPPING,
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Secant Wherelocation",				GRAYOUT_CLOAK, /* teleportitis */
	(SPFX_RESTR|SPFX_EVIL|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("DuffDuffDuff",				PSEUDO_TELEPORTER_CLOAK, /* +3 increase damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Insane Mind Screw",				CLOCKLOAK, /* psi resistance, horror trap effect */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL|SPFX_REFLECT), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 7000L ),

A("Resistant Punching Bag",				GIMP_CLOAK, /* +1 MC */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Honored Fairness",				SNARENET_CLOAK,
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL|SPFX_ESP|SPFX_STLTH), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Fast Speed Bump",				PINK_SPELL_CLOAK, /* very fast speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Turn Loss Extreme",				UNDERLAYER_CLOAK, /* ctrl-U does not work */
	(SPFX_RESTR|SPFX_EVIL|SPFX_SEARCH|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Can't Touch This",				ELONGATION_CLOAK, /* 10 extra points of AC */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Changed Random Numbers",				UNFAIR_ATTACK_CLOAK, /* fiddles with the RNG, similar to x-race */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("UBB Rupture",				CARTRIDGE_OF_HAVING_A_HORROR, /* random fainting, displays garbage strings 1 in 10 turns, deactivates disintegration and confusion resistance but grants stun resistance */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("No RMB Vacation",				BORDERLESS_HELMET, /* right mouse button loss */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Sincere Mania",				HELM_OF_COUNTER_ROTATION,
	(SPFX_RESTR|SPFX_EVIL|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Television Wonder",				FLICKER_VISOR, /* fleecescript */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Video Decoder",				SCRIPTED_HELMET, /* flicker strips */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Unimportant Elements",				EMPTY_LINE_HELMET, /* resist acid, petrification, sleep */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Marlena's Song",				GREEN_SPELL_HELMET, /* invisibility, displacement */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Drelitt",				MORE_HELMET, /* recurring amnesia and disenchantment */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Russian Ice Blocks",				SCALER_MITTENS,
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Blacky's Back Without L",				BLACK_SPELL_GAUNTLETS, /* black ng walls, all L are always spawned peaceful */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Disenchanting Blackness",				GAUNTLETS_OF_REVERSE_ENCHANTME, /* recurring disenchantment that can push items down to -20 */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HSPDAM|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Raaaaaaaarrrrrrgh",				FUCKUP_MELEE_GAUNTLETS, /* +5 damage and AC, but all monsters look like grayouts */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Uneven Engine",				COMPETITION_BOOTS, /* very fast speed, speed bug */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Indian Smoke Symbol",				QUASIMODULAR_BOOTS, /* negative AC gives more damage reduction */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Powerwarp",				COVETED_BOOTS, /* all covetous monsters have +5 spawning frequency */
	(SPFX_RESTR|SPFX_EVIL|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Equipped for Trouble",				KILLER_HEELS, /* flying */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Stonewall Checkerboard Disease",				CHECKER_BOOTS, /* petrification resistance, allows you to chew through rock */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Bluede",				ELVIS_SHOES, /* spawns a natalje trap 1 in 1000 turns, resist psi, fire, cold, shock, poison, sleep */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 25000L ),

A("Shortfall",				AIRSTEP_BOOTS, /* magical breathing */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Bridge Shitte",				HIGH_HEELED_SKIERS, /* spawns a shit trap 1 in 500 turns and you trigger them even if you fly, can walk on snow */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Such A Wonderful Roommate",				HIGH_SCORING_HEELS, /* if you are paralyzed while hungry or worse, 100 nutrition points are restored per turn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Vrrrrrrrrrrrr",				TRON_BOOTS, /* very fast speed, adds extra speed sometimes */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Nastist",				RED_SPELL_HEELS, /* nastiness, resist fire, cold, petrification, sleep */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Zerdroy Gunning",				DESTRUCTIVE_HEELS, /* teaches you the ammo creation technique if you didn't have it, and it creates 3x as many bullets but will try to curse the boots again, even if they are already cursed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Hypocritical Fun",				RIN_FORM_SHIFTING, /* polymorph control */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Like A Real Server",				RIN_LAGGING, /* disconnected stairs */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Evil Detector",				RIN_BLESSCURSING, /* if it curses an item, you will know that item's BUC */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Wouuu",				AMULET_OF_TRASH, /* +5 AC, clairvoyance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Gagarin's Translator",				CYAN_SPELL_CLOAK, /* renames all items to soviet, infravision, spells cost 10% less mana */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL|SPFX_WARN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Surterstaff",				AMULET_OF_STARLIGHT, /* detect monsters if you wield a quarterstaff-class weapon, carries a Topi Ylinen curse if you do and fills you with the black breath otherwise */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Heliokopis's Wizarding Aid",				GREAT_DAGGER, /* petrification resistance when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2400L ),

A("Total Genocide",				SCR_GENOCIDE, /* genocides everything that can be genocided */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Jana's Roulette of Life",				ORCISH_CLOAK, /* initialized to always be foundry cloak; 10x higher chance of getting a wish from fountain quaffing, but 1 in 100 chance to die outright no matter what the fountain's effect was. */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Magic Juggulate",				ORCISH_CLOAK, /* initialized to always be spellsucking cloak */
	(SPFX_RESTR|SPFX_EREGEN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("High King of Skirim",				ORCISH_CLOAK, /* initialized to always be storm coat, +5 AC, STR and CHA */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Allcolor Prism",				ORCISH_CLOAK, /* initialized to always be fleeceling cloak; prism reflection */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Mary Inscription",				REGULAR_GLOVES, /* initialized to always be princess gloves; all monsters always spawn hostile, +10 CHA, +5 AC */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 10000L ),

A("Fathien Elder's Secret Power",				REGULAR_GLOVES, /* initialized to always be uncanny gloves; occult spells don't cause backlash */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Si-Oh-Wee",				REGULAR_GLOVES, /* initialized to always be slaying gloves; +2 damage and accuracy */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Johanetta's Rough Gentleness",				LOW_BOOTS, /* initialized to always be blue sneakers; resist cold and shock */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 8000L ),

A("Jana's Vaginal Fun",				LOW_BOOTS, /* initialized to always be blue sneakers; prevents your inventory from getting wet and provides unbreathing, but if you get wet, 1 in 100 chance to die instantly. */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Very Nice Person",				LOW_BOOTS, /* initialized to always be femmy boots; aggravate monster, monsters in special rooms always spawn awake, but monsters generally have 20% chance of spawning peaceful otherwise */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Julia's Real Love",				LOW_BOOTS, /* initialized to always be red sneakers; +3 CHA */
	(SPFX_RESTR|SPFX_DEFN|SPFX_REGEN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Eliane's Combat Sneakers",				LOW_BOOTS, /* initialized to always be yellow sneakers; +20 CHA, kicking has 5% chance of instakilling the monster, but if something farts, you get 4 extra bad effects */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Mailie's Challenge",				LOW_BOOTS, /* initialized to always be pink sneakers; aggravate monster, your kick can never be clumsy, resist poison, psi and drain */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 9000L ),

A("Merlot Future",				LOW_BOOTS, /* initialized to always be ski heels; can walk on "snow" and give rnd(500) turns of very fast speed if you didn't have temporary speed yet */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 0L ),

A("Madeleine's Girl Footsteps",				LOW_BOOTS, /* initialized to always be calf-leather sandals; kicking a monster gives +1 alignment, resist disintegration and death rays */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Ruth's Mortal Enemy",				LOW_BOOTS, /* initialized to always be velcro sandals; flying */
	(SPFX_RESTR|SPFX_EVIL|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Larissa's Anger",				LOW_BOOTS, /* initialized to always be buffalo boots; aggravate monster, +5 kick damage, resist cold/shock/disintegration */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Pretty Roommaid",				LOW_BOOTS, /* initialized to always be heroine mocassins; resist shock and petrification */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Aliseh's Red Color",				LOW_BOOTS, /* initialized to always be lolita boots; stealer trap effect, +10 CHA, having sex will cause you to have children more often */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Katie Melua's Fleeciness",				LOW_BOOTS, /* initialized to always be weapon light boots; your potions cannot be destroyed by cold, treachery and hate trap effects, doubles healup effects or quadruples if you're a healer */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL|SPFX_REGEN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Elona's Snail Trail",				STANDARD_HELMET, /* initialized to always be rubynus helmet; half speed unless you're a snail, +10 CON and very fast speed if you are one */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Henrietta's Little Problem",				BROWN_SHIT_CLOAK, /* can walk on ice and snow while wearing this, but 1 in 200 times you move, you get the shit trap effect, and 1 in 2000 times you move, items can fall out of your inventory like if you were wearing cone heels while restricted */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL|SPFX_HPHDAM|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Failset Gamble",				YELLOW_WING, /* yellow spells, flying */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Kangaroo Snort",				ELESDE_CLOAK, /* very fast speed, but hallucination always becomes heavy */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Ultraggravate",				CLOAK_OF_GRAVATION, /* multiplies monster difficulty by 2, forces heavy aggravation */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Ziratha's Casting Legend",				SPELL_RETAIN_CLOAK, /* spells cost 10% less mana to cast */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Trip Terrain",				QUAVERSAL_HELMET, /* farlooking a quasar square gives a bad effect; can always Ctrl-T to teleport and it costs 10 mana only */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Jestes Taka Kurwa",				HELM_OF_SHUFFLING, /* leveling up gives you the same stuff that a kurwa would get, resist fire, cold, sleep and petrification */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Fully Working at Half Maximum",				GOLDSPELL_HELMET, /* spells cost half the usual amount */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Claudia's Direction Finder",				AIRHEAD_CAP, /* resist confusion and stun */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Bomb Blow",				FALLOUT_HELMET, /* 1 in 100 randomly generated traps is a cataclysm trap */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("You Don't Know Shit",				IDENTIFY_CURD_HELMET, /* unidentify effect, -3 INT/WIS when worn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Turkish Empire",				BAEAEAEP_SPY_HELMET, /* if you use create ammo while wearing it, you get double the amount of bullets and also a bunch of rockets */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ESP|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Virus Attack",				DISTORTED_GRIMACE, /* +2 range for all missile weapons and +1 multishot */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Bad Luck In Droves",				ELM_ET, /* if your luck is ever greater than 0, you lose one point of it */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Yith Versus Baby",				SANEMAKER_HELMET, /* deafness */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Stout Immurring",				ORANGE_SPELL_GLOVES, /* wall trap effect, +8 AC, free action */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Jonadab's Keycode",				MOJIBAKE_GLOVES, /* 20% of all glyphs are fleecy-colored */
	(SPFX_RESTR|SPFX_EVIL|SPFX_SEEK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Volcano Boom",				DIXPLOSION_GLOVES, /* creates an explosion centered on you 1 in 2000 turns */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Hugging, Groping and Stroking",				VIOLET_BEAUTY_HEELS, /* half physical damage when a monster has grabbed you, being kicked in the nuts or clawed in the breasts heals you instead of dealing damage, and being lashed restores your mana; +10 CHA, +2 damage when bashing with heels and +5 kick damage, thick-skinned monsters don't get damage reduction when you kick them */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Smexy Berries",				LONG_POINTY_HEELS, /* while you wear them, a counter goes up that starts at 1000, and if you chat to a female G_UNIQ with MS_BOSS or MS_FART_(something) while wearing them and having at least 1000 on the counter, the boss monster in question becomes tame */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Smell Like Dog Shit",				SOIL_CLINGING_BOOTS, /* triggering a heap of shit damages your charisma, and 1 in 1000 chance that a heap of shit is generated somewhere, monsters have a 1 in 10 chance of getting appr=0, disintegration resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Elenettes",				PERMANENTLY_BLACK_SHOES, /* any monster that has less than 90% of its max HP is shown to you, resist petrification and disintegration, sustain ability, +5 charisma, your kick cannot be clumsy and does +2 damage, count as wedge heels */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Headcrunch",				AUTOSCOOTER_HEELS, /* kicking a monster that has a head deals +10 damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Variantish Design",				FORCEFUL_BOOTS, /* converting a Moloch altar summons orcs, angering a peaceful monster causes an enrage effect, Elbereth doesn't work, hostile V have 50% chance of life saving */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Wumshin",				BUM_BUM_BOOTS, /* 1 in 200 chance to take some damage and be stunned for d10 turns, kicking a monster has 75% chance of paralyzing it for one turn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Jonadab's Bug Mastery",				FLUCKERING_BOOTS, /* uses different calculation for the nasty trap effect that makes you faster on average */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Cruel Goddess Ana",				SPRAP_BOOTS, /* 1 in 250 turns you start vomiting if you aren't already, and the vomiting cannot be cured at all, +5 CHA, -5 STR/CON, monsters have 1% chance to spawn peaceful */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Kristin's Nobility",				BRIGHT_CYAN_BEAUTIES, /* while wearing them, shops will buy stuff even if they normally wouldn't, +5 CHA, dimness and stun time out much faster but confusion is always heavy confusion */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Satan's Chopping Block",				SIA_BOOTS, /* deactivates disintegration resistance, and disintegration breathers have +200 spawn frequency */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Satan's Hypercharge",				MORK_BOOTS, /* gives d24 extra movement energy to randomly generated monsters */
	(SPFX_RESTR|SPFX_EVIL|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("A Spoonful Of Fo(u)rk",				MORK_BOOTS, /* evilvariant mode while worn, maxxes out your carry capacity no matter what */
	(SPFX_RESTR|SPFX_EVIL|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Fire Night",				RIN_DAYSHIFT, /* resist fire and sleep, teaches beauty charm technique if you don't know it and have at least 10000 zorkmids in open inventory (but you then lose those 10000 zorkmids) */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Arabella's Swooning Beauty",				AMULET_OF_ITEM_INSECURITY, /* disintegration and death ray resistance, anti-teleportation, clairvoyance, can't drop items */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("You Have Ugh Memory",				AMULET_OF_INITIALIZATION_FAILU, /* rmb loss, death ray resistance, gaze attacks only have 10% chance of affecting you */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Anastasia's Lure",				AMULET_OF_REAL_SCREWERY, /* all bad traps have the same chance of being spawned, very fast speed */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM|SPFX_HSPDAM|SPFX_REGEN|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Apathy Strategy",				AMULET_OF_ENEMY_SAVING, /* 50% chance of halving your speed, random fainting, but monsters with a speed of at least 2 also have their speed halved */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Mosh Pit Scramble",				AMULET_OF_SPELL_METAL, /* female monsters that have kick attacks get +20 spawn frequency, aggravate monster, +2 AC for every worn armor piece as long as no single worn piece is non-metallic, +1 CON for every worn metallic piece, free action */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Arabella's Weapon Storage",				ORCISH_CLOAK, /* initialized to always be a cursed called cloak, 1 in 1000 chance per turn to generate a random weapon at your feet, 1 in 10000 chance for evil artifact effect, your wands (not wishing or other powerful ones) only lose a charge 1 in 4 times, firearm ammos are generated with double stack size (also applies to techniques), mulchable ranged weapons have 50% unconditional chance to not mulch, 1 in 10000 chance per turn to curse your entire inventory including stuff in containers with increased chance of giving more dangerous types of curses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Kys Yourself",				ORCISH_CLOAK, /* initialized to always be a fourchan cloak, premature death, horses are 90% spawned peaceful and intelligent female monsters 25% */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Ina's Sorrow",				ORCISH_CLOAK, /* initialized to always be an inalish cloak, walking over a toilet with enough nutrition causes you to crap and lose more nutrition than normal and -20 alignment/+1 sins, enforces anorexia conduct when worn; when fainting or worse, your kick does +10 damage and your weapons do +3 plus you get resistance to lightning, contamination and petrification */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Why Not Do The Real Thing",				STANDARD_HELMET, /* initialized to always be a grunter helmet, 75% chance to not become sick from a sickness effect, but activates evilvariant mode; armor pieces have 5% extra chance to be spawned with a random enchantment */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Claudia's Sexy Scent",				STANDARD_HELMET, /* initialized to always be a cloudy helmet, monsters with MS_STENCH are 99% peaceful and chatting to them with at least 1000 zorkmids allows you to hire them, your kick does +10 damage and has 25% chance to paralyze a monster for d10 turns, but you occasionally have to fart and also have the shit trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Vacuum Cleaner Death",				STANDARD_HELMET, /* initialized to always be a breath control helmet, amulets of strangulation kill you instantly while wearing this, farting effects deal extra damage and cause additional bad effects, sleep/slow/poison gas traps damage and paralyze you, magical breathing, monsters with AD_SUCK have +50 spawn frequency, 1 in 25000 chance per turn to spawn a tame sweet blonde next to you */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Nypherisbane",				STANDARD_HELMET, /* initialized to always be a gas mask, poison and sickness resistance, kicking a snake or naga does +100 extra damage, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Christmas Medal",				STANDARD_HELMET, /* initialized to always be a sages helmet, see invisible */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 25000L ),

A("Longest Ray",				REGULAR_GLOVES, /* initialized to always be rayductnay gloves, adds extra range to beams like over-ray (and stacks with that) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Endorphic Scratching",				LOW_BOOTS, /* initialized to always be orgasm pumps, 1 in 4000 chance that you have to play a minigame */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Sonja's Torn Soul",				LOW_BOOTS, /* initialized to always be worn-out sneakers, bank trap effect, but for every 100 gold you deposit you gain a point of alignment */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Ruea's Failed Conversion",				LOW_BOOTS, /* initialized to always be noble sandals, MS_CONVERT has 95% chance to fail on you, +10 CHA, but you get the kurwa claw effects and spawn chance increase */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 12000L ),

A("Hawaiian Kamehameha",				LOW_BOOTS, /* initialized to always be doctor claw boots, allows you to use Ctrl-T to teleport at will */
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Larissa's Gentle Sleep",				LOW_BOOTS, /* initialized to always be feelgood heels, sleeping on a bed stops time for longer than the amount of turns you sleep for */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Stefanje's Problem",				LOW_BOOTS, /* initialized to always be plof heels, being kicked stuns you and has 5% chance of causing a bad effect, but if you kick a monster, it permanently loses 1 max HP; +2 to all stats */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Malena's Ladyness",				LOW_BOOTS, /* initialized to always be princess pumps, give controlled polymorphitis and you don't break your stuff if you polymorph and they will stay on your feet no matter what */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Elevecult",				LOW_BOOTS, /* initialized to always be ballet heels, reduces your speed like the spirit race but monsters also have 33% chance to waste their turns, +5 CHA, 75% chance to not fumble if you otherwise would, discount action */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Shit Kickers",				LOW_BOOTS, /* initialized to always be steel toed boots, confuses and stuns monsters with a kick (but they get a resistance check) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Sarah's Granny Wear",				LOW_BOOTS, /* initialized to always be marji shoes, soft farting noises heal you instead of dealing damage, but you get the stealer trap and teleporting items effects, resist confusion and sleep */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Cliche Wear",				LOW_BOOTS, /* initialized to always be mary janes, resist fire/cold/shock/sleep/poison, monsters with item damage attacks have +5 spawn frequency */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Dirge",			LONG_SWORD,
	(SPFX_ATTK|SPFX_RESTR|SPFX_DEFN|SPFX_INTEL),0,0,
	PHYS(5,2),	DRLI(0,0),	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Keolewa",CLUB,
	(SPFX_RESTR), 0, 0,
	PHYS(5,6), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("The Sword of Bheleu", TWO_HANDED_SWORD, /* petrification resistance, disables poison resistance when wielded */
	(SPFX_RESTR|SPFX_ATTK|SPFX_INTEL|SPFX_DALIGN|SPFX_EVIL), 0, 0,
	PHYS(10, 10), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 12000L),

A("The Ring of P'hul", RIN_POISON_RESISTANCE, /* sickness resistance */
	(SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L),

A("The Really Cool Shirt", T_SHIRT,
	(SPFX_RESTR|SPFX_LUCK|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L),

A("Heliokopis's Keyboard Curse", AMULET_OF_SANITY_TREBLE, /* automore effect in memory of how Heliokopis plays */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L),

A("Satan's Final Trick", AMULET_OF_EVIL_VARIANT, /* nastiness */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L),

A("Arabella's Dicinator", AMULET_OF_BAD_PART, /* cursed parts, curseuse, disintegration resistance, speed, invisibility, see invisible, keen memory */
	(SPFX_RESTR|SPFX_EVIL|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L),

A("The King of Porn", VICTORIAN_UNDERWEAR, /* by Porkman; can't refuse seduction attacks, discount action */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L),

A("Unicorn Drill", UNICORN_HORN, /* polymorph control and drain resistance when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L),

A("Automatically Metal", AMULET_OF_SPELL_METAL, /* makes it so that all metallic items are immune to erosion while all non-metallic ones are susceptible to all erosion effects; if this amulet is a nonmetallic type, its material automatically changes to "metal" */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L),

A("Yvonne's Model Ambition", LOW_BOOTS, /* initialized to always be velvet pumps, cold and shock resistance, +2 charisma */
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L),

A("Version Control", RIN_AGGRAVATE_MONSTER, /* randart, autocurses, putting it on allows you to activate or deactivate soviet and evilvariant hybrid races */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L),

A("Aegis", DWARVISH_ROUNDSHIELD, /* petrification resistance, provides partial protection from gaze attacks */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L),

A("Egrid Bug",			LONG_SWORD, /* black spells, de-light, cursed parts, displays all 'x' and they can't move diagonally */
	(SPFX_ATTK|SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_EVIL), 0, 0,
	PHYS(5,2),	DRLI(0,0),	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("The Trident of Poseidon", TRIDENT, /* magical breathing, swimming and protects items from getting wet */
	(SPFX_RESTR|SPFX_DFLAG1), 0, M1_SWIM,
	PHYS(5, 10), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6000L),

A("Weakite Thrust",			DARK_BAR, /* can attack through walls, weak sight, de-light and nolite when wielded */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Dizzy Metal Storm",			POLE_LANTERN, /* jumping, can displace monsters by walking into them, silver spells, manler effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Payback Time",			NASTYPOLE, /* player can use it at increased range */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Annoying Dog Whistle",			OTAMA, /* by Demo: acts as a magic whistle whenever you hit */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 12),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Zombiebane",			ORNATE_MACE, /* by Demo: while you wield it, killing a potentially resurrecting monster prevents it from resurrecting */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_ZOMBIE,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Petrogeneration",			LIZARD_SCALE_MAIL,
	(SPFX_RESTR|SPFX_REGEN|SPFX_EREGEN), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Konrad's Attack",			FLAME_MOUNTAIN,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 12),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 4500L ),

A("Sylvie's Invention",			BEAM_REFLECTOR_GUN, /* when wielded, allows you to evade missile attacks sometimes */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 2),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Protection Racket",			TENNIS_RACKET,
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	PHYS(0, 6),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Multijuice",			ENERGY_SAP, /* can potentially be used several times */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Keith's Underoos",			YOGA_PANTS, /* greatly increases spellcasting success chance, but not above regular limits; poison resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Flowerbane",			GRAIN_SCYTHE, /* while you wield it, %-class monsters may randomly lose turns */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_BAD_FOOD,
	PHYS(5, 0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Rhorn",			RAZOR_WHIP, /* while wielded, you deal level-dependant passive damage to attacking monsters */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 14),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Transmemorizer",			HELM_OF_OPAQUE_THOUGHTS, /* keen memory when worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 6000L ),

A("HEV Suit",			SECOND_SKIN, /* resist poison, sickness and contamination */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Too Many Affixes",			CLOAK_OF_FLIGHT, /* +3 constitution, allows you to gain XP twice as fast */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Realsteal",			ROGUES_GLOVES, /* improves borrow command */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Lepre-Luck",			GREEN_GOWN, /* teleportitis */
	(SPFX_RESTR|SPFX_LUCK|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Teh Physique",			HELM_OF_ANTI_MAGIC, /* +10 STR/CON/CHA/DEX, -10 INT/WIS */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Very Memory",			OILSKIN_COIF, /* keen memory */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Russia's Salvo",			KALASHNIKOV,
	(SPFX_RESTR), 0, 0,
	PHYS(10, 8),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Belkar's Warpoportation",			RIN_JUMPING, /* teleportitis, polymorphitis */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Reliable Trinsics",			RIN_ILLNESS, /* poison and stun resistance, full nutrients */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Dragonhide Armer",			CLOAK_OF_MEMORY, /* cold and fire res */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 4000L ),

A("WhooshZhoosh",			GAUNTLETS_OF_FAST_CASTING, /* every time you cast a spell, you temporarily get intrinsic speed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Bile Patch",			RIN_MEMORY, /* acid resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 1600L ),

A("Do Not Forget Grace",			AMULET_OF_POWER, /* +5 STR, +10 DEX */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 3000L ),

A("Perpetual Memory",			POT_KEEN_MEMORY, /* lasts for a much longer time than usual */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 6000L ),

A("RNG's Extravaganza",			IMPLANT_OF_QUEEB_BUTT, /* if it's +0 when you put it on, it sets itself to a random value from -9 to +9; half physical damage if in a form without hands */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Ime Spew",			IMPLANT_OF_TOTAL_NONSENSE, /* fleecescript, flicker strips, quad attacks, you take double damage; if in a form without hands, squeaking skill trains twice as fast */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Eternal Soreness",			IMPLANT_OF_ANAL_RETENTION, /* whenever your inertia counter is below 5, 10 turns will be added to it; whenever you kill a monster, 1 in 50 chance of getting +1 max HP; if in a form without hands, 1 in 10 chance per turn to get a unihorn effect like glowhorn */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REGEN), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Fullgrease",			IMPLANT_OF_HEAVY_LUBRICATION, /* 1 in 5000 chance per turn that items in your inventory randomly become greased; if in a form without hands, also 1 in 5000 chance that you get to choose an item that can be greased */
	(SPFX_RESTR), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Bucket House",			IMPLANT_OF_CRAP, /* crap trap effect, slow digestion, hunger; if in a form without hands, also discount action */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Coronation Culmination",			IMPLANT_OF_KING_BEEFCAKE, /* resist cold, fire, poison, sleep and shock, see invisible, greatly increased prayer timeout, and putting it on for the first time gives an additional skill slot and a random artifact; if in a form without hands, also grants technicality */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Restroom Drenching",			IMPLANT_OF_MENSTRATING, /* disables regeneration, but your speed increases relative to how wounded you are; bigger bonus if you're female; if you're in a form without hands, grants double stealth */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Deine Mudda",			IMPLANT_OF_YOUR_MOMMA, /* displays your momma messages; if you're eligible for an enchantment-based AC bonus, that bonus is multiplied by 5 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Hair Bracket",				HAIRCLIP, /* no special effect, just there to unlock squeaking */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Diceroller's Kit",				MATERIAL_KIT, /* can be used several times, changes the material after each use */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Material Rain",				MATERIAL_KIT, /* randomly changes some item materials, like at game start */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("New Overcast",				MATERIAL_KIT, /* the material it contains is applied to several other randomly selected items */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Scentfold",				BLINDFOLD, /* scent view when worn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Extra Heavy Bullet",				LEAD_CLUMP,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Arena Litter",				BONE_FRAGMENT,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Bad Hitter Boy",				HEAVY_SHADOW_BALL, /* -d20 to-hit */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Sven's Garbage Booster",				HEAVY_WOOD_BALL, /* hitting a monster with it makes it bleed for d10 (more) turns */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Giant Meat Stick",				BULKY_FLESH_BALL, /* hunger and full nutrition when wielded */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Variant Source Package",				HEAVY_TAR_BALL, /* clairvoyance when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Different Timer",				WONDER_BALL,
	(SPFX_RESTR), 0, 0,
	PHYS(6, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Waller Smack",				BRICK_CHAIN,
	(SPFX_RESTR), 0, 0,
	PHYS(3, 12), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Cycle Whack-Whack",				LEAD_NUNCHIAKU, /* double attacks if you're riding */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Fuyer Brev",				COPPER_SCOURGE, /* displays all F while wielded */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_FUNGUS,
	PHYS(20, 40), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Goldification",				GEMSTONE_LASH, /* killing a monster while wielding it creates zorkmids equal to the monster's level */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 10000L ),

A("Softrash",				SILK_SCOURGE,
	(SPFX_RESTR), 0, 0,
	PHYS(6, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Zero Notion",				NULL_CHAIN,
	(SPFX_RESTR), 0, 0,
	PHYS(1, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Geology Rules The World",				BRICK_PICK, /* while wielded, pick-axe skill trains three times as fast */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Yes, Hit It More Often",				ARCANE_HORN,
	(SPFX_RESTR), 0, 0,
	PHYS(15, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Science Power",				NANO_HAMMER,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Equality Twin",				LIGHTWHIP,
	(SPFX_RESTR), 0, 0,
	PHYS(8, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Heritage Digger",				CERAMIC_KNIFE, /* petrification resistance when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 12), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 8000L ),

A("Sandy Oases",				SAND_SWORD,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(5, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("H.'s Brittle Replica",				VOLCANIC_BROADSWORD, /* 1 in 10 chance to be destroyed if you hit something */
	(SPFX_RESTR), 0, 0,
	PHYS(12, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Daedra Seeker",				SHADOWBLADE, /* displays all & while wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Pernicious Grid",				ETHER_SAW, /* resist poison and free action when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Today's Repair",				DESERT_SWORD, /* technicality when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Hallow Moonfall",				CHROME_BLADE, /* sight bonus when wielded, permanently poisoned */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Need Elite Upgrade",				DARKNESS_CLUB, /* hitting something with it adds +1 with 1 in (enchantment * 100, at least 100) chance, up to a maximum of +12 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Deathwrecker",				LEAD_FILLED_MACE, /* stun resistance when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Peregrine of Night",				ETHER_KNOUT, /* echolocation when wielded */
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Boarded Shelf",				SPIKERACK, /* successfully using enchant weapon on it adds 5 extra points of enchantment, up to a maximum of +25 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Queue Staff",				CHROME_STAFF, /* permanently poisoned */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Mana Eater",				CHROME_STAFF, /* manaleech when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Quicker Rheology",				LEADBAR, /* very fast speed and swimming when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Cantonese Smack",				NANO_POLE,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Pricker Smacker",				BRICK_MISSILE,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Arabella's Black Prong",				DIFFICULT_TRIDENT, /* difficulty +10 when wielded, etherwind, diminished bleeding and increased training */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(8, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Crash Joust",				BRICKLAYER_BAR, /* jousting a target does +10 damage and 1 in 3 chance of paralyzing target for d5 turns */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Lulwy's Trick",				WIND_BOW, /* adds extra speed when wielded */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Parable Ammunition",				LEAD_PISTOL_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 18), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("Casing Buckshot",				LEAD_SHOT,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 30), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Becoming Greener",				POISON_BOLT,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Mesherabane",				ETHER_BOLT, /* +d40 to M5_ELONA monsters */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_FUNGUS,
	PHYS(10, 40), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Manyfolt Shots",				SAND_DART,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Technical Wonder",				NANO_SHURIKEN,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Clayface's Fear",				DARK_BATARANG,
	(SPFX_RESTR), 0, 0,
	PHYS(8, 14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Harmony Virgin",				ETHER_WHIP, /* protects against STDs, discount action when wielded */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(6, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Onyx Mantle",				METAL_SHIRT, /* disintegration resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Asteroid Belt",				RED_STRING,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Required Power Plant Gear",				FULL_LEAD_SUIT, /* +5 AC, resist fire, poison and sickness, technicality */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Stable Exoskeleton",				NANO_SUIT, /* +10 AC when wielded */
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Terrifying Loss",				CLOAK_OF_COAGULATION, /* blood loss, resist fear */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Pheromone Case",				CLOAK_OF_SCENT, /* +10 spawn frequency for MS_STENCH */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("No Reduction Allowed",				CHROME_SHIELD, /* sustain ability */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("I in the sky",				ANTISHADOW_SHIELD, /* flying */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 9000L ),

A("Lisa's Buckled Beauties",				LADY_BOOTS, /* disintegration resistance, discount action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Kristin's Cheats",				LADY_BOOTS, /* stun resistance, free action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Johanna's Red Charm",				LADY_BOOTS, /* can walk in lava, +5 charisma, +1 all other stats */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Dora's Scratchy Heels",				LADY_BOOTS, /* sickness resistance, -5 AC, +3 CHA and +1 STR/DEX but -2 WIS/INT, kicking an enemy causes d10 bleeding damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Elena's Challenge",				LADY_BOOTS, /* kicking a male monster has 1% chance that it has to make a saving throw or become tame; autocurses, random bad effects and random nastytrap effects */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Kati's Irresistible Stilettos",				STILETTO_SANDALS, /* +5 charisma, +2 AC, +2 increase damage, diminished bleeding */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Verena's Dueling Sandals",				STILETTO_SANDALS, /* +10 charisma, hammer shoes deal +d10 damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 10000L ),

A("The Impossible Catwalk",				STILETTO_SANDALS, /* can walk on ice and snow, +10 CHA, free action and discount action, disables speed */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Corgon's Ring",				RIN_DIMINISHED_BLEEDING, /* resist sickness, magic and lightning, +100 carry cap, disables speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6000L ),

A("Jana's Diminisher",				RIN_DIMINISHED_BLEEDING, /* flying, unbreathing, if you start bleeding there's a 1 in 100 chance to die instantly (no joke). */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Walt Versus Anna",				AMULET_OF_SCENT, /* autocurses, kicking a diggable wall may cause it to crumble, farting and perfume monsters spawn more often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Sue Lyn's Sex Game",				TOILET_ROLL, /* chatting to a MS_FART_NORMAL pacifies it, or if you pay 100*monsterlevel zorkmids, tames it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Rejuvenation By Gate",				TAPERED_MAIL, /* inertia is halved every turn */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Incredibly Sexy Squeaking",				EXCITING_ARMOR, /* unbreathing, putting it on while it's -4 or better summons a tame MS_FART_NORMAL and reduces enchantment by 5 points */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Fooking Tank",				LORICATED_CLOAK, /* 10 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Darksight Helm",				BASINET, /* increases sight range by 2 (stacks with sight bonus), blindness resistance */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_BLND), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("AA's Crashing Tragedy",				ROCKET_GAUNTLETS, /* +5 AC, random nastytrap effects */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Kite Me",				ANCIENT_SHIELD,
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Prada's Devil Wear",				ITALIAN_HEELS, /* all & are spawned peaceful 90% of the time, autocurses, DYWYPI problem, autopilot and death resistance when worn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Lena's Peace Offering",				WONDER_DAGGER, /* peacevision when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Self-Protection Implement",				SHARP_AXE, /* diminished bleeding when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Suicidal Unique",				SHORT_BLADE,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Once In A Blue Moon",				ELEGANT_BROADSWORD, /* if it's both a new moon and Friday the 13th, your luck is maxxed while wielding it */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1313L ),

A("Sortie A Gauche",				MAIN_SWORD, /* can reach the ground while riding even when unskilled */
	(SPFX_RESTR), 0, 0,
	PHYS(8, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Etruscian Swimming Lesson",				ROMAN_SWORD, /* swimming, you count as having a thick hide while wielding it, but 1 in 8 chance to move slower */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(2, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Missing Unique Item",				MYTHICAL_SABLE, /* no specialties */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Glacierdale",				CRYPTIC_SABER, /* allows you to walk on snow and ice, protects your potions from cold */
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK), 0, 0,
	COLD(8, 10), DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Jonadab's Vehemence",				BLOW_AKLYS,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Signo One",				TELEPHONE, /* psi resistance when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Moloch's Personal Phone",				TELEPHONE, /* using it as a phone will call a very hostile demon lord */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 2), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Klingon Commander's Arms",				BATLETH,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(10, 14), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Prick Bearer's Ransom",				ELITE_BATLETH, /* hitting an enemy with it causes d10 bleeding damage, killing an enemy creates zorkmids equal to the monster's level */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Texas Instruments XS",				CALCULATOR,
	(SPFX_RESTR), 0, 0,
	PHYS(12, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2400L ),

A("Full-Lead A Fast-Being",				WEIGHTED_FLAIL, /* 1 in 1000 chance that you get temporary (d50 turns) speed */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Saxs Beauty",				BLOCK_HEELED_SANDAL, /* resist fire, cold, shock, poison and psi when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Yvonne's Honor",				BLOCK_HEELED_SANDAL, /* shock and fear resistance when wielded */
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK), 0, 0,
	COLD(8, 12), DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Millicent's Cone",				PROSTITUTE_SHOE,
	(SPFX_RESTR), 0, 0,
	PHYS(6, 8), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Martha's Foreign Goer",				PROSTITUTE_SHOE, /* hitting an enemy with it causes d10 bleeding damage, foocubuses are more likely to want sex with you */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Tommy's Deferred Heel",				PROSTITUTE_SHOE, /* monsters are always spawned hostile while you wield it */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(10, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Marital Action",				BO_STAFF,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_HUMAN,
	PHYS(10, 16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Hit Em Really Hard",				LONG_POLE,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Stop Evading Me",				SHARP_POLE, /* monsters are less likely to dodge */
	(SPFX_RESTR), 0, 0,
	PHYS(14, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Projection Faulter",				STACK_JAVELIN,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("Jouster Power",				SPEC_LANCE, /* stun resistance, successful jousts deal +d20 damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Game Knowledge Cheat Sheet",				META_BOW, /* clairvoyance and echolocation when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("APEC Production",				MATERIAL_BOLT,
	(SPFX_RESTR), 0, 0,
	PHYS(3, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("APEC Exploit",				RANDOM_DART,
	(SPFX_RESTR), 0, 0,
	PHYS(3, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Nez Special Offer",				CUBIC_STAR, /* deals d10 bleeding damage to monsters */
	(SPFX_RESTR), 0, 0,
	PHYS(8, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Misty's Melee Pleasure",				SEXPLAY_WHIP, /* +5 charisma and fire resistance when wielded, prevents STDs */
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_MALE,
	PHYS(8, 14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Corporal Digging",				MYSTERY_PICK,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Colonel Proudster",				MYSTERY_LIGHTSABER, /* applying it while it's empty allows you to get 750 extra units of charge for 10k zorkmids */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Common Shotter",				SLING_AMMO,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Jarmen Cheatertype Kell",				BLUE_BOW, /* invis when wielded */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6000L ),

A("Elvin's Press",				AIR_PRESSURE_HAMMER, /* 1 in 100 chance when hitting something with it that you trigger d3 turns of time stop */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Tubhackor",				BUBBLEHORN,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Sorc Tendency",				EAGLE_BALL,
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Poison Burst",				QATAR, /* always poisoned, resist poison when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Acceleration Claw",				LOWER_ARM_BLADE, /* speed when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(8, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Ironbox",				CESTUS,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4500L ),

A("Camohameha",				CAMO_QATAR, /* double damage and +5 range for kamehameha tech when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Ubergage",				FIELD_PLATE, /* +4 AC and +3 STR */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Garyx",				CONTRO_DRAGON_SCALE_MAIL, /* +1 all stats when worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Hitec Neo",				CLOAK_OF_EXPERIENCE, /* technicality */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Floatover",				PASSTHROUGH_BOOTS, /* flying */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("People Coat",				GIANT_SEA_ANEMONE, /* +5 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Away-Hamm Armor",				BARNACLE_ARMOR, /* 1 in 1000 chance that a monster attacking you in melee takes d100 damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Susanne's Headsmasher",				MARBLE_CHUNK,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Iron Ball of Ball Ironing",				HEAVY_IRON_BALL,
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_ATTK), 0, M2_MALE,
	FIRE(5, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Gary's Rivalry",				CONTRO_STAFF, /* player can walk over farmland, magical breathing, items don't get wet, +2 increase damage when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 8000L ),

A("Sunscreen",				LONG_SWORD, /* blindness resistance, emits light */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Real Life Effector",				RIN_MOOD, /* 1 in 2000 chance to spawn a MS_SUPERMAN somewhere on the level */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Avada Porkavra",				WAN_DEATH, /* fires death rays in all directions but also loses d8 charges every time */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Blue Shirt of Death",				FOAM_SHIRT, /* hyperbluewalls, death ray resistance */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Redwood Stinker",				CIGARETTE, /* 1 in 1000 chance of creating a stinking cloud */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Peeplue",				ELECTRIC_CIGARETTE, /* +2 kick damage and player cannot be engulfed; if under the effect of a juen trap, free action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Natalia's Mark",				BELL_CLAWS, /* very fast speed */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(20, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 12000L ),

A("Suntinopener",				TIN_OPENER, /* emits light */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Sunrubberhose",				RUBBER_HOSE, /* emits light */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Neutron Star",				BUBBLETAR,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("The Staff of Necromancy",				FOAMY_STAFF, /* curse resistance when wielded */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	DRLI(0, 0), DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Nude And Free",				PETA_COMPLIANT_SHIRT, /* free action if you're not wearing a suit */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Ziratha's Iron Sights",				SNIPER_RIFLE, /* increased vision, infravision */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Piece of Sky",				EAGLE_BALL,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Drama Staff",				JARED_STONE, /* conflict, farlook bug, hostility, wakeup call */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Save Your Run",				LIGHTBULB,
	(SPFX_RESTR|SPFX_REGEN|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("What You Really Needed",				LIGHTBULB,
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	PHYS(2, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Frozen Orb",				HEATH_BALL,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK), 0, 0,
	COLD(0, 16), DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Clever Cleaver",				GRINDER,
	(SPFX_RESTR), 0, 0,
	PHYS(3,6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Jinada",				LOWER_ARM_BLADE,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Counterstrike Cestus",				CESTUS, /* resist shock, control magic, discount action */
	(SPFX_RESTR), 0, 0,
	PHYS(10, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Norse Mithril",				METEORIC_STEEL_SHIELD, /* +5 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Compliant Armor",				OSFA_CHAIN_MAIL,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Rishrash Saw",				VARIED_GRINDER,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Wrong Turn",				ROBE_OF_DEATH_RESISTANCE, /* heavily autocurses, putting it on turns you into an undead creature and it doesn't fall off if you become something too big */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Hellpit Flame",				INFERNAL_AXE,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Straight Outta Coding Hell",				INFERNAL_BARDICHE,
	(SPFX_RESTR), 0, 0,
	PHYS(2, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Weight Anchor",				INFERNAL_ANCUS, /* +200 weight */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Sharpening Slat",				FLINDBAR, /* if you hit something and it's negatively enchanted, 1 in 100 chance of repairing one point of enchantment */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Dolores's Sadism",				CROW_QUILL, /* diminished bleeding */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DRLI), 0, 0,
	DRLI(0, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Covetor Saber",				RAKUYO, /* teleportitis */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Greenlings Lash",				VIPERWHIP, /* deals poison damage like dirge */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Valensina's Blade",				VICTIM_KNIFE,
	(SPFX_RESTR), 0, 0,
	PHYS(2, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Meager Curd",				CUDSWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(2, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Big Smoke's Murder Car",				GREEN_SABER, /* very fast speed */
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Brass Cup",				METAL_STAR,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(0, 4), DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Shrinekeeper",				CELESTIAL_POLE,
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1600L ),

A("War Declaration",				ZEBES_POLE, /* deals poison damage like dirge */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Why Always Conundrum",				CRYPTIC_POLE, /* wielding it sets it to a random material if it was conundrum */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Foamonia Water",				SHOOMDENT, /* magical breathing, swimming, items don't get wet */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Falling Perseis",				HEAVY_METEORIC_BALL,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Latchstack",				HEAVY_BUBBLE_BALL, /* player cannot be grabbed by AD_STCK and similar */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("High Oriental Praise",				FOAM_CHAIN, /* 95% chance to resist MS_STENCH, unbreathing, +3 CHA */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Bowser's Fun Arena",				ORCISH_CLOAK, /* initialized to always be volcanic cloak, protects your items from fire */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Bills Paid",				ORCISH_CLOAK, /* initialized to always be fuel cloak, sight bonus when worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Finding Thyself",				ORCISH_CLOAK, /* initialized to always be cloister cloak, spirituality skill trains five times as fast */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Saladin's Desert Fox",				STANDARD_HELMET, /* initialized to always be shemagh, full nutrients when worn, swords of all kinds do more damage, prayer may occasionally warn you if it's not safe to pray */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Jamila's Belief",				STANDARD_HELMET, /* initialized to always be shemagh, protects from MS_CONVERT and MS_HCALIEN effects, increases your alignment by one with 1 in 1000 chance per turn, but taking it off gives -200 alignment and +1 sins */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Slipping Sucking",				STANDARD_HELMET, /* initialized to always be ceramic helmet, sustain ability */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Inert Greaves",				LOW_BOOTS, /* initialized to always be thick boots, +4 AC */
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Desert Meditation",				LOW_BOOTS, /* initialized to always be sand-als, keen memory */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Gorgeous Veil Model",				LOW_BOOTS, /* initialized to always be shadowy heels, invisibility */
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 9000L ),

A("Sublevel Flooding",				LOW_BOOTS, /* initialized to always be weight attachment boots, 1 in 10000 chance of flooding effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Use The Normalness Turns",				LOW_BOOTS, /* initialized to always be fungal sandals, wereform effect, drain resistance */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Unfellable Tree",				LOW_BOOTS, /* initialized to always be standing footwear; while burrowed, you have +20 AC and resist fire, cold, shock, sleep, confusion, stoning and disintegration */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Free Extra Cancel",				CANCEL_DRAGON_SCALES, /* +1 MC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7500L ),

A("Nice Cyan Color",				CANCEL_DRAGON_SCALE_MAIL, /* fear resistance when worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Look How It Blocks",				CANCEL_DRAGON_SCALE_SHIELD, /* +20% chance to block */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Scientific Symbiont Kit",				SYMBIOTE, /* used up only 20% of the time */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Xom's Scrollinator",				SYMBIOTE, /* gives a random symbiote */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Healthy Parasite",				SYMBIOTE, /* adds a 3x health boost to the symbiote */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Holden Middle Post",				SYMBIOTE, /* doesn't remove the monster */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Rock Solid Seat",				LEATHER_SADDLE, /* cannot be disintegrated */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Tasty Tame Nasty",				RIN_INCREASE_DAMAGE, /* conflict and aggravate monster when worn; by jonadab */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Laughtersnee",				ETHER_SAW, /* resist confusion and stun, displacement; by jonadab */
	(SPFX_RESTR), 0, 0,
	PHYS(0,8), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 6000L ),

A("Nenya",				RIN_PROTECTION, /* unchanging, causes elves to generate peaceful; by aosdict */
	(SPFX_RESTR|SPFX_WARN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Porkman's Balls of Steel",				HEAVY_SILVER_BALL, /* silver spells, you take extra damage of silver */
	(SPFX_RESTR|SPFX_REGEN|SPFX_EREGEN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Overrated Face Protection",				CORONA_DRAGON_SCALE_MAIL, /* resist fear, weak sight */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Carmark",				LOW_BOOTS, /* initialized to always be business shoes, -5 CHA, very fast speed, shopkeepers charge you double the price */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Anita's Rashlust",				LOW_BOOTS, /* initialized to always be filigree stilettos, resist disintegration and death rays, diminished bleeding */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Jonadab's Everyday Wear",				LOW_BOOTS, /* initialized to always be ugly boots, 5% better spellcasting chances */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Amateursports",				LOW_BOOTS, /* initialized to always be unisex pumps, very fast speed, +3 DEX */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Ronja's Female Pushing",				LOW_BOOTS, /* initialized to always be cuddle cloth boots, death resistance, kick should push monsters back more often */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Snailhunt",				LOW_BOOTS, /* initialized to always be garden slippers, displays all 'b' and 'w', +1 increase damage, swimming */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Antje's Powerstride",				LOW_BOOTS, /* initialized to always be dyke boots, resist cold, death and disintegration, +10 STR, -5 CHA, makes you thick-skinned and too big to fit through diagonal passages */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Nadine's Cuteness",				LOW_BOOTS, /* initialized to always be ankle strap sandals, discount action, death resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Beautyqueak",				FEMMY_STILETTO_BOOTS, /* 1 in 10000 per turn to spawn a tame MS_FART_NORMAL, full nutrients */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Little But Robust",				MADELEINE_PLATEAU_BOOTS, /* disintegration resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Everything Is Green",				MARLENA_HIKING_BOOTS, /* poison and acid resistance, can walk through trees */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Endless Deseaming",				ANASTASIA_DANCING_SHOES, /* can #monster to repair them, resist cold, fire and shock, stun resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Far East Relation",				JESSICA_LADY_SHOES, /* resist cold, swimming, unbreathing, -3 STR and CON, fear factor */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Camelic Scent",				SOLVEJG_MOCASSINS, /* scent view, displays all 'z' and 'Y', +7 CHA, wakeup call effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4500L ),

A("Airship Dancing",				WENDY_LEATHER_PUMPS, /* resist psi, jumping, +5 CHA, +2 kicking damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Deep Sigh",				KATHARINA_PLATFORM_BOOTS, /* resist cold and disintegration, +5 CHA */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Final Challenge",				ELENA_COMBAT_BOOTS, /* resist disintegration and death, +10 CHA, kicking a male monster has 1 in 5 chance to paralyze it for 1d5 turns, can always resist taking off stuff when seduced by a male AD_SSEX */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Little Bitch is ructious",				THAI_COMBAT_BOOTS, /* resist death and fear, +3 kicking damage */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Hit Their Hands",				ELIF_SNEAKERS, /* glib combat, resist cold, manaleech */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Totally The Sexy Bitch",				NADJA_BUCKLED_LADY_SHOES, /* resist sleep, +5 CHA, monsters always spawn hostile, free action */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Thick Farting Girl",				SANDRA_COMBAT_BOOTS, /* resist sleep, petrification and acid, makes you thick-skinned, you have to fart every once in a while */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Crashing Your Sister's Wedding",				NATALJE_BLOCK_HEEL_SANDALS, /* resist psi and confusion, discount action, +2 increase damage, -5 increase accuracy, control magic */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 15000L ),

A("Star Soles",				JEANETTA_GIRL_BOOTS, /* starlight sky trap effect, wonderlegs, monsters need a +1 weapon to hit you effectively, -2 all stats */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("What A Cuddly Color",				YVONNE_GIRL_SNEAKERS, /* resist cold, petrification and shock */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Strong Getaway Desire",				MAURAH_HUGGING_BOOTS, /* levelteleportitis, monsters with AD_WEEP have +20 spawn freq, resist petrification */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Wild Sex Game",				MELTEM_COMBAT_BOOTS, /* resist disintegration, disables half physical damage, +2 kicking damage, drunken boxing */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("First Place Guaranteed",				SARAH_HUGGING_BOOTS, /* very fast speed plus extra speed on top of that, resist shock */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Rubber Love",				CLAUDIA_WOODEN_SANDALS, /* you don't fall into water or lava, resist stun */
	(SPFX_RESTR|SPFX_EVIL|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Find The Secret Course",				LUDGERA_HIKING_BOOTS, /* magic resistance */
	(SPFX_RESTR|SPFX_EVIL|SPFX_SEEK|SPFX_SEARCH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("That's Super Unfair",				KATI_GIRL_BOOTS, /* resist petrification, replicates anastasia trap effect, can #monster to repair them */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Final Exam Time",				NELLY_LADY_PUMPS, /* resist disintegration and cold, displays all G_UNIQ, dimness and dimmopathy */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Everywhere At Once",				EVELINE_WEDGE_SANDALS, /* no slowness from walking on sand, no noise on paved floor, resist petrification and acid, +3 DEX, aggravate monster */
	(SPFX_RESTR|SPFX_EVIL|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Too Much Bravery",				KARIN_LADY_SANDALS, /* resist fear, cold and petrification, disables stealth, pethate trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Shin Kicking Game",				JUEN_PEEP_TOES, /* +5 kicking damage, but if a female monster kicks you, you're paralyzed for one more turn; death resistance */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Bitchsmoke",				KRISTINA_PLATFORM_SNEAKERS, /* resist fire, burnopathy, aggravate monster */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Wasteful Player",				LOU_SNEAKERS, /* 1 in 1000 chance per turn that your items are randomly withered, but the shoes themselves cannot be hit by that effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Don't Fall Into The Abyss",				ALMUT_SNEAKERS, /* flying, glib combat, resist cold, 10% improved spellcasting chances, disables resist fear and gives fear status */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Ouu-eech",				JULIETTA_PEEP_TOES, /* you sometimes crash into the floor regardless of skill, +10 CHA */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Arabella's Girl Kick",				ARABELLA_HUGGING_BOOTS, /* resist petrification, sleep and psi, free action, control magic, versus curses, diminished bleeding, +5 CHA, bad part, cursed parts, brown spells, soiltype effect, autopilot */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM|SPFX_HSPDAM|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Grasshopper's Antidote",				BOOTS_OF_SHOCK_RESISTANCE, /* poison resistance */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Stroking Combat",				SKILL_CAP, /* searching can create traps, +2 kick damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Doggo Friendship",				BOBBLE_HAT, /* displays all 'd' and they have 90% chance of spawning peaceful, 10% of peacefully generated ones spawn tame */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Asuka's Torture Instrument",				MASSAGER,
	(SPFX_RESTR), 0, 0,
	PHYS(2, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Swimchamp",				FISH_SCALE_MAIL, /* prevents items from getting wet */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Sweet Victoria",				FLEECY_CORSET, /* +3 CHA, +2 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Give Me Stroke, Jo'Anna",				FISHNET, /* resist cold, high heels skill trains 2x as fast, sentient monsters have an AD_HEAL touch sometimes that works even while you're wearing these */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Resist Against Its Damage",				PETRIFYIUM_BAR, /* petrification resistance when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Ain't Gonna Disintegrate Nothing",				DISINTEGRATION_BAR, /* disintegration resistance when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Demobling",				AMULET_OF_TIME, /* resist fire, clairvoyance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Max One Gray",				AMULET_OF_PET_VIEW, /* gray monsters have 10% chance of spawning tame */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Ampersand Harem",				LOW_BOOTS, /* initialized to always be demonologist boots; can tame demons reliably while wearing them, resist fire and poison */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Dip-Dive",				LOW_BOOTS, /* initialized to always be mud boots; can pick up stuff from the bottom of pools and lava */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Mandy's Raidwear",				LOW_BOOTS, /* initialized to always be failed shoes; resist fear, 50% chance of not losing alignment by adjalign() */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Phanto's Retardedness",				LOW_BOOTS, /* initialized to always be opera pumps; wallwalking, monsters need a +4 weapon to hit you reliably, 1 in 100 chance per turn to get hit by a random nastytrap effect, sticky prime curses itself */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Claudia's Self-Will",				LOW_BOOTS, /* initialized to always be wooden clogs; autopilot, resist cold and poison, 75% chance to avoid perfume, you always have at least 10 contamination, +5 CHA */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Faster Than All Others Into Shit",				LOW_BOOTS, /* initialized to always be regular sneakers; anastasia trap effect, very fast speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2500L ),

A("Ruth's Undead Inlay",				LOW_BOOTS, /* initialized to always be elite sneakers; resist poison, sleep and cold */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Krawasaki Steerer",				LOW_BOOTS, /* initialized to always be biker boots; confusion resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Upward Heels",				LOW_BOOTS, /* initialized to always be zero drop shoes; count as cone heels and slow you down like a spirit, but monsters act as if they were short-sighted */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Ludgera's Secret Course",				LOW_BOOTS, /* initialized to always be hiking boots; if you take a crap while wearing them (but only via a toilet), you get temporary speed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("There Are Several Of Them",				STANDARD_HELMET, /* initialized to always be pope hat; improved light radius, spirituality skill trains twice as fast */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Football Mask",				STANDARD_HELMET, /* initialized to always be corona mask; footerer effect, +2 AC */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("FFP-3 Mask",				STANDARD_HELMET, /* initialized to always be corona mask; 80% chance to resist corona and another -5 CHA */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Acu Become Have",				STANDARD_HELMET, /* initialized to always be anachronononono helmet; 1 in 50000 chance per turn to unlock skills like an acu, but you need to have at least a million score and it resets your score to zero whenever it triggers. */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Priestest Bless",				ORCISH_CLOAK, /* initialized to always be ornamental cope; blessing technique times out slightly faster */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Satan's Suggestion",				ORCISH_CLOAK, /* initialized to always be wetsuit; magical breathing, S_EEL don't become undetected again when they move */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Egasso's Gibberish",				REGULAR_GLOVES, /* initialized to always be multilinguitis gloves, confusion + confusion resistance, your MC is always at least 5, improves AC by 10%, +5 CHA */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Cortex Coprocessor",				IMPLANT_OF_MEMORY, /* randart, totter, +2 INT and WIS, and another +2 while in a form without hands */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Amysbane",				BLOCK_HEELED_SANDAL, /* disables magic resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Shoe Brand",				UNKNOWN_KNIFE, /* supposed to deal extra damage to MS_SHOE and sometimes tame them */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Sandals of Jesus Christ",				LOW_BOOTS, /* initialized to always be throwaway sandals; waterwalking */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Amulet of Splendor",				AMULET_OF_STRANGULATION, /* randart, polymorphitis, poly control, magical breathing, poison res, ESP, reflection, cannot eat food while wearing it, player falls asleep every once in a while */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Luisa's Fellow Feeling",				BEAUTIFUL_SHIRT, /* player is no longer kicked in the nuts by AT_KICK */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("The Pill That Killed Michael Jackson",				PILL, /* food poisoning, damages all stats and deals severe HP damage when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Bug Bazooka",				ROCKET_LAUNCHER, /* summons random 'a' and 'x' when firing */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Now For The Discharge",				CAMOUFLAGED_CLOTHES, /* teaches sigil of discharge when put on but also gives intrinsic discharge trap effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Ring of Fast Living",				RIN_REGENERATION, /* EXP boost, lose 3 extra nutrition per turn, double attacks but you take double damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Mareyoungwoman Action",				WATER_WALKING_BOOTS, /* very fast speed while you're on water or a fountain */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("CrySpear",				CERAMIC_SPEAR, /* regenerate one extra hit point per turn without extra hunger */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Gauget Unlocker",				SCR_SKILL_UP, /* prior to processing its own effect, unlocks a skill like acu level up, or three if you actually are an acu; this item cannot be wished for */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Hostilitawn",				FIGURINE, /* always becomes hostile */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Neutralness Neutered",				FIGURINE, /* always becomes peaceful */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Pierce Device",				STETHOSCOPE, /* deals 1 point of damage to the target */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("PolyFast",				RIN_POLYMORPH, /* very fast speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Wobblesteady",				RIN_POLYMORPH, /* stun resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Psi-Change",				WAN_POLYMORPH, /* zapping it gives 1000 turns of temporary confusion resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Arthur's high-heeled platform sandals",				LOW_BOOTS, /* initialized to always be sharp-edged sandals, +2 AC and kicking damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("The Angel And The Thin Sections",				SPE_ORE_MINING, /* no special effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Sinsword",				TWO_HANDED_SWORD, /* increase accuracy and damage if your alignment is negative, +1 up to a maximum of +6 for every -50 alignment */
	(SPFX_RESTR|SPFX_DCLAS), 0, (S_ANGEL),
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Schwilililili Morph",				TIN_OPENER, /* using #youpoly has only 50% chance of taking off an use */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Green Thumb",				REGULAR_GLOVES, /* initialized to always be nondescript gloves; can walk through trees and you regain an extra HP per turn when being on a tree square */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("No-Damage",				LEATHER_SADDLE, /* attacks never get redirected to your steed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Bike Saddle",				LEATHER_SADDLE, /* +50% movement speed when riding */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Clown Car",				LEATHER_SADDLE, /* flying */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Steering Wheel",				LEATHER_SADDLE, /* steed always heals confusion in one turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Bridgebane",				BARDICHE, /* immunity to drawbridges; applying it at a drawbridge destroys it */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("System of Sexual Pleasure",				SENTIENT_HIGH_HEELS, /* intelligent female enemies can randomly act as if they had random heels, you gain 10% bonus experience */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Kronscher Bar",				LEADBAR, /* platinum spells, hyperbluewalls, you can eat metal while wielding it */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(10, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Sigix Broadsword",				BROADSWORD, /* -5 to-hit, +20 AC, 1 in 20 chance per hit that it loses enchantment and gets destroyed if it goes below -20 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Geavy Greaves",				LOW_BOOTS, /* initialized to always be lead boots */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Heel of Fortune",				STEEL_CAPPED_SANDAL,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Swiss Amy Knife",				SURVIVAL_KNIFE, /* displays all MS_SHOE */
	(SPFX_RESTR), 0, 0,
	PHYS(14, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Magicbone",				UNICORN_HORN, /* all 'f' and 'd' have 50% chance of spawning peaceful */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Robe of Retribution",				ROBE_OF_FAST_CASTING, /* deals 1d(one third of your level) damage to attacking monsters, 33% of the time */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Robe of Infestation",				ROBE_OF_NASTINESS, /* occasionally spawns a flock of hostile S or s somewhere on the level */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("The Blade of Grasshopper",				TRASH_SWORD,
	(SPFX_RESTR|SPFX_DCLAS), 0, (S_SPIDER),
	PHYS(10, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("The Mysterious Force",				WORM_TOOTH, /* teleportitis, clairvoyance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("The Staff of Leibniz",				IMPACT_STAFF, /* +1 all stats */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Moebius Armor",				SPECIAL_LEATHER_ARMOR, /* everything is in fleecy colors, +10 AC, +10% chance to block */
	(SPFX_RESTR|SPFX_EVIL|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Stingswanbur",				WONDER_DAGGER,
	(SPFX_RESTR|SPFX_WARN|SPFX_DFLAG2|SPFX_HALRES|SPFX_SEARCH|SPFX_DEFN), 0, (M2_ORC),
	PHYS(5, 0), DRLI(0, 0), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6000L ),

A("Coppered Off From Me",				BRONZE_PICK, /* stealer trap effect, resist poison and shock, extra digging speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Flotation Device",				LENSES, /* waterwalking, life saving */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Fastplant",				IMPLANT_OF_MEMORY, /* randart, implants skill trains twice as fast; if you're in a form without hands, also extra skill training */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Gimmie Dat Skill",				SYMBIOTE, /* trains symbiosis skill by 50 when used */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Fook The Obstacles",				REACH_TRIDENT, /* can attack through walls, choicelessness, exploding dice, cyan spells */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Pogo Stick",				CLIMBING_STICK, /* very fast speed and teleportitis when wielded */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Duurvoid",				CLIMBING_STICK, /* occasionally warns you when a door is trapped */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("It Become Light",				LIGHTBULB, /* increases sight by 2 squares */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Speedo-Car",				LEATHER_SADDLE, /* you move 20% faster */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Deeper-Laid BMW",				LEATHER_SADDLE, /* can always reach the ground while riding, even when unskilled */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Freebound",				CLIMBING_SET, /* flying when worn and allows you to move over farmland */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Macan-Stretch",				INKA_SADDLE, /* MC3 while riding */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Owesome Bro",				PETRIFYIUM_BRA, /* petrification resistance when worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Nuse Le Grand",				LASERDENT,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Luke's Bluelight",				LASER_SWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(8, 8), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("Enturn Swash",				LASER_POLE,
	(SPFX_RESTR|SPFX_PROTEC|SPFX_ATTK), 0, 0,
	ELEC(10, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6500L ),

A("Baobhan Mountain",				SITH_STAFF, /* manler effect, 10% better spellcasting */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HSPDAM), 0, 0,
	PHYS(6, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Unfair Fighting",				KRISTIN_COMBAT_BOOTS, /* free action, and you have 25% chance of lifesaving */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Eroticlamp",				ANNA_HUGGING_BOOTS, /* resist disintegration and death; if you're being held by a monster, +2 melee and kick damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Ratch Closure Scratching",				RUEA_COMBAT_BOOTS, /* if your HP is at the current maximum, you start bleeding; resistant to erosion like withered gloves, putting them on increases enchantment value if it's lower than +1 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Scratche Hussy",				DORA_COMBAT_BOOTS, /* +3 charisma, kicking a monster causes it to bleed, allows you to spread scentful perfume */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Fingernail Front",				MARIKE_SNEAKERS, /* +3 unarmed damage if you're not wearing finger-covering gloves */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Give The Art A Home",				JETTE_COMBAT_BOOTS, /* +2 damage with hammer-class weapons, you can engrave more characters per turn, slows you down like spirit */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Sorrow and Despair",				INA_HUGGING_BOOTS, /* sickness resistance, aggravate monster and black breath */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Deseaming Game",				SING_PLATFORM_BOOTS, /* resist fire and disintegration, full nutrients, +3 STR and CON, killing a monster has 1 in 50 chance of spawning a hostile shoe monster somewhere on the level */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Do Not Pee",				VICTORIA_COMBAT_BOOTS, /* if they're not damaged at all, petrification resistance and kicking a male monster has 1 in 5 chance of paralyzing it; male monsters have 1 in 3 chance of attacking the boots */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("PractiClassy",				MELISSA_WEDGE_BOOTS, /* if you put them on while the nemesis isn't dead yet, you automatically gain the beacher race; resist disintegration and death, randomly generated gold is doubled */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Now You Are However To",				ANITA_LADY_PUMPS, /* aggravate monster, all monsters spawn hostile, magic resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Such A Sigher",				HENRIETTA_COMBAT_BOOTS, /* when you fumble, you're paralyzed for 10 turns; resist petrification and sleep */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Steerboat",				VERENA_STILETTO_SANDALS, /* swimming, items don't get wet in water */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Perminant Increase",				LOW_BOOTS, /* initialized to always be machinery boots; entering a machinery room sets your intrinsic protection to 4 if it was lower */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Whiny Teacher Inside Woman",				LOW_BOOTS, /* initialized to always be christmas child mode boots, aggravate monster, 1 in 100 chance per turn that nearby monsters turn to flee like you had cast cause fear */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Now You Look Like A Beggar",				LOW_BOOTS, /* initialized to always be sandals with socks, banking effect, resist poison, cold and sickness */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Anja's Wide Field",				LOW_BOOTS, /* initialized to always be wedge sneakers, resist death, allows you to spread the perfume */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("U Be Curry",				LOW_BOOTS, /* initialized to always be barefoot shoes, full nutrients, eating a tin gives 3x nutrition */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Brightmace",				STARWARS_MACE, /* doesn't get deactivated by darkness */
	(SPFX_RESTR), 0, 0,
	PHYS(4,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Delightsaber",				STARWARS_MACE, /* doesn't get deactivated by darkness, de-light trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(10,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Bitcher 3: The Wild Cunt",				BITCHER, /* gives a new random feminism effect when used */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Havana Nero",				CIGAR, /* disintegration resistance when wielded, cut nutrition effect */
	(SPFX_RESTR|SPFX_ATTK|SPFX_EVIL), 0, 0,
	FIRE(4,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Lucky Melee Attacks",				CIGARETTE, /* +10 melee to-hit when wielded */
	(SPFX_RESTR|SPFX_LUCK|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("The Eighth Badge",				COMMANDER_SUIT, /* pokemon never betray you while worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Elbergofukyourself",				CRAPPY_SPEAR, /* monsters ignore Elbereth */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(2, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Omghaxereth",				PLASTIC_MACE, /* E is as powerful as scare monster scrolls, quaversal effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("The Vore of the Rings",				HELM_OF_HUNGER, /* you can eat all rings */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Political Correctness for Game Design 101",				SPE_HEALING, /* randart, wielding it mimics SJW nasty trap effects */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("H.'s Gangster Kicks",				LOW_BOOTS, /* initialized to always be exceptional sneakers, knives do +2 melee damage and throwing weapons have +1 multishot */
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 6000L ),

A("The Longbone of Banana",				BONE_CLUB, /* +3 DEX when wielded */
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("The Longbow of Banana",				YUMI, /* +3 DEX when wielded */
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Fleecyswandir",				SEXPLAY_WHIP,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Hallucolorswandir",				PROSTITUTE_SHOE, /* everything displays in fleecy colors */
	(SPFX_RESTR|SPFX_HALRES|SPFX_EVIL), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Veryverydarkgreyswandir",				ARCANE_RAPIER,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 6000L ),

A("Blackswandir",				LOWER_ARM_BLADE, /* hallu res, stealth and invisibility if you're the batman race */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Darkerthanblackswandir",				DARK_BAR,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 7000L ),

A("Wtfswandir",				DROVEN_BOW,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6000L ),

A("Brighterthanwhiteswandir",				PAPER_SWORD,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Ultravioletswandir",				YITH_TENTACLE,
	(SPFX_RESTR|SPFX_HALRES|SPFX_EVIL), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Hooloovooswandir",				MYSTERY_PICK,
	(SPFX_RESTR|SPFX_HALRES|SPFX_INTEL), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Octarineswandir",				RADIOACTIVE_DAGGER, /* etherwind when wielded */
	(SPFX_RESTR|SPFX_HALRES|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 9000L ),

A("#F183D5swandir",				PARASOL,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Agswandir",				DAGGER,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Constructionworkeryellowswandir",				SHARP_AXE,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Schoolbusyellowswandir",				BROOM,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Aquamarineswandir",				BULLETPROOF_CHAINWHIP,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Wizardswandir",				CONTRO_STAFF,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Clayswandir",				MILL_PAIL,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Yayswandir",				SPECIAL_MOP,
	(SPFX_RESTR|SPFX_HALRES|SPFX_LUCK), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Bundleswandir",				SILK_SPEAR,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Excalibundle",					SILK_SPEAR,
	(SPFX_RESTR|SPFX_SEEK|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH), 0, 0,
	PHYS(5,10),	DRLI(0,0), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Fleecybrand",				SILK_SPEAR,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	STUN(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Ogrebundle",		SHOOMDENT,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_OGRE,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 200L ),

A("Trollsbundle",                 CARDBOARD_FAN,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_TROLL,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 200L ),

A("Stormbundler",               SOFT_LADY_SHOE,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_INTEL|SPFX_DRLI), 0, 0,
	DRLI(5,2),	DRLI(0,0),	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("The Longbundle of Diana", ICKY_BLADE,
	(SPFX_RESTR|SPFX_INTEL|SPFX_REFLECT|SPFX_ESP), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Quiver of Fleecy Arrows",				PAPER_ARROW,
	(SPFX_RESTR), 0, 0,
	PHYS(10, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("The Orb of Fleece",	CRYSTAL_BALL, /* invisibility and magic resistance when wielded */
	(SPFX_RESTR|SPFX_INTEL|SPFX_ESP|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2500L ),

A("The Eye of the FleecyBundle",	AMULET_OF_NECK_BRACE,
	(SPFX_RESTR|SPFX_INTEL|SPFX_EREGEN|SPFX_HSPDAM), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Fear Brand",                 LONG_SWORD, /* resist fear when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 6),	NO_ATTK,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Faith Brand",                 HEAVY_LONG_SWORD, /* alignment record recovers by one with 1 in 200 chance per turn */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 6),	NO_ATTK,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Fleece Brand",				LONG_SWORD,
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Bundlebundlebundle Brand",				MAGICAL_PAINTBRUSH,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	STUN(5, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Brand Brand",				ASTERISK, /* mutters 'sponsored by nuka-cola' all the time */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Bundle of Opening",				WAN_OPENING,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Candelebundle",				NATURAL_CANDLE,
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Bundleabrum of Invocation",				TELEPHONE, /* can be applied successfully as long as it's better than -10 */
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("The Book of the Fleecy Bundlebundlebundle",				SPE_APPLY_NAIL_POLISH,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_HSPDAM|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Enchanteasy",				DROVEN_DAGGER, /* hitting something with it while it's less than +7 has 1 in 1000 chance of adding +1 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Elven Dooh",				DARK_ELVEN_DAGGER,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 16), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("True Masterpiece",				QUALITY_AXE, /* discount action when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Autosmash",				NANO_AXE,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Guns In My Head",				SPIRIT_AXE, /* +1 multishot with ranged weapons while you wield it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Everything Else Too",				ETERNIUM_MATTOCK, /* disintegration resistance when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Blackgash",				ETERNIUM_BLADE, /* disintegration and death resistance when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Double Me!",				AUTOMATIC_KATANA, /* double attacks */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Censerhero",				OSBANE_KATANA,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5, 6), DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("True Aim",				ETERNIUM_SABER,
	(SPFX_RESTR), 0, 0,
	PHYS(16, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Whaw Whaw",				WILD_BLADE, /* random bad effects and horror trap effect while wielded */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(20, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Bong-Gong",				ALLOY_CLUB,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Sharptooth Sayer",				JAGGED_TOOTH_CLUB, /* very fast speed while wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Doors Are No Obstacles",				HUGE_CLUB, /* can ooze through doors while wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Makeshift Bridge",				LOG, /* waterwalking when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Go Away You Stupid Bee",				FLY_SWATTER,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_ANT,
	PHYS(5, 26), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Exterminate The Bugs",				FLY_SWATTER,
	(SPFX_RESTR|SPFX_DCLAS|SPFX_SEEK), 0, S_XAN,
	PHYS(10, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Drop Everything And Kill The Mosquito",				INSECT_SQUASHER, /* if something stings you, you go berserk */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(6, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Dainty Sload",				BOAT_OAR, /* +3 CHA and improves MC by one when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("Thirst For Blood",				HEAVY_GRINDER, /* bloodthirsty */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(6, 4), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Discharger",				SILVER_MACE,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(8, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Melated Metal",				BRONZE_MACE, /* if you hit something, 1 in 1000 that you can change the material of a worn armor piece to copper, but if you choose nothing or an item that's already made of bronze, the mace disenchants itself up to -20 */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Sneakeritis",				ELVEN_MACE,
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	PHYS(2, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Wanderzapper",				KAMEREL_VAJRA, /* zapping a wand trains devices skill twice as fast */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Death To All Who Oppose Us",				BACKHAND_MACE,
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	PHYS(2, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Wonderlight",				SHINY_STAR, /* extra poison damage, +2 light radius */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 9000L ),

A("Shred Them!",				STONE_STAR, /* diminished bleeding, petrification resistance */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Timeshifter",				YESTERDAY_STAR, /* 1 in 10 chance per turn that the turn counter doesn't increase, inertia times out twice as fast */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Charsi's Throwaway Tool",				NOVICE_HAMMER, /* no specialties */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Orgruk's Training",				NOVICE_HAMMER,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Psi-Tec",				NOVICE_HAMMER, /* psi resistance and technicality when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Nursing The Flame",				FIRE_STICK, /* +1 light radius */
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK), 0, 0,
	FIRE(10, 10), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Real Walking",				WALKING_STICK, /* anastasia trap effect, resist poison, can walk over farmland */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(6, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7500L ),

A("Wizmelee",				MASSIVE_STAFF,
	(SPFX_RESTR), 0, 0,
	PHYS(12, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("At Last You Found It",				BATTLE_STAFF,
	(SPFX_RESTR|SPFX_SEARCH), 0, 0,
	PHYS(8, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Not Quite A Noob",				NOOB_POLLAX,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("One Hundred Stars",				ETERNAL_POLE, /* you resist item theft like a nymph, flying */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Bohem Fuelkanal",				BOHEMIAN_EAR_SPOON, /* 1 in 100 chance to attempt to slow down the target, but it can also resist */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Cha-Shatter",				BRITTLE_SPEAR, /* degrades upon hitting something, including when thrown */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(2, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Lightning Stroke",				DROVEN_SPEAR,
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	PHYS(2, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Dumpsterman",				RANDOSPEAR, /* full nutrients */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Searsharp",				SILVER_SPEAR, /* undead warning */
	(SPFX_RESTR), 0, 0,
	PHYS(8, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4500L ),

A("Gift To Nature",				INKA_SPEAR, /* you can walk through trees */
	(SPFX_RESTR|SPFX_DFLAG1), 0, M1_ANIMAL,
	PHYS(10, 16), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 3000L ),

A("The Stake of Khazad-Dum",				DWARVISH_SPEAR,
	(SPFX_RESTR), 0, 0,
	PHYS(6, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Selfbane",				DRAGON_SPEAR,
	(SPFX_RESTR|SPFX_DCLAS|SPFX_REFLECT), 0, S_DRAGON,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Danger Sign",				ASBESTOS_JAVELIN, /* poison and contamination resistance when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Hoe BlackPro",				FOURDENT,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(2, 4), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1600L ),

A("Edergrade",				PLOW, /* full nutrients */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Cosomonono",				ELVEN_LANCE,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Hal Scope",				SNIPESLING, /* infravision when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("M1 Garand",				HUNTING_RIFLE,
	(SPFX_RESTR), 0, 0,
	PHYS(10, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Sig-SG 552",				ASSAULT_RIFLE,
	(SPFX_RESTR|SPFX_WARN), 0, 0,
	PHYS(4, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Cronvert",				HEAVENLY_WHIP, /* 90% chance to resist MS_CONVERT taunts */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Double Mystery",				CONUNDRUM_PICK, /* EXP boost, gives asgardian level-up intrinsics when you do level up while having it wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Digsru",				NANO_PICK, /* can train wedi skill by digging or forcing with it */
	(SPFX_RESTR), 0, 0,
	PHYS(3, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Irmgar's Non-Inventory",				FISHING_POLE, /* no specialties */
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Wowzer Spike",				DARK_HORN,
	(SPFX_RESTR), 0, 0,
	PHYS(2, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Allelujah",				SKY_HORN,
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	PHYS(6, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Doubleburn",				TORCH,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Flamescorch",				TORCH,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(0, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Thunk Thunk",				TORCH,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Darklite",				TORCH, /* +2 light radius while wielding a lit one */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Astraltor-tschh",				TORCH, /* astral vision while it's lit */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Alora's Practice",				BLUE_LIGHTSABER,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Rosh Trainor",				RED_DOUBLE_LIGHTSABER, /* all lightsaber forms train twice as fast */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 2), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Great Antilles",				YELLOW_LIGHTSABER, /* swimming when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(10, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Soresure",                ROBE_OF_WEAKNESS, /* can train soresu while wearing it */
    (SPFX_RESTR|SPFX_EVIL), 0, 0,
    NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Neverman",                OLDEST_STAFF, /* can train niman while wielding it */
    (SPFX_RESTR), 0, 0,
    NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Ataru One",                CRYSTAL_SWORD, /* can train ataru while dual-wielding it with ataru two */
    (SPFX_RESTR), 0, 0,
    NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Ataru Two",                ETERNIUM_BLADE, /* invoking one of these ataru artifacts generates the other */
    (SPFX_RESTR), 0, 0,
    NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Outjuyoing",                PIKE, /* can train juyo by using it to smash enemy weapons */
    (SPFX_RESTR), 0, 0,
    NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("This Is Vaapad",                TWO_HANDED_TRIDENT, /* can train vaapad while wielding it */
    (SPFX_RESTR), 0, 0,
    NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Concentrator",                OTAMA, /* can train shii-cho while wielding it */
    (SPFX_RESTR), 0, 0,
    PHYS(0, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Ma-Strike",                BRONZE_MORNING_STAR, /* can train makashi while wielding it */
    (SPFX_RESTR), 0, 0,
    NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Rusma-Sro",                BRIDGE_MUZZLE, /* can train djem so by throwing it */
    (SPFX_RESTR), 0, 0,
    NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Shienside",                CRYSTAL_SHIELD, /* can train shien by blocking with it */
    (SPFX_RESTR), 0, 0,
    NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Control Help",				OLDEST_STAFF, /* control magic when wielded */
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	PHYS(5, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Woeblade",				ETERNIUM_BLADE, /* deals extra damage like doomblade */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Elements Time Three",				PIKE, /* cold resistance when wielded, applying it at something has 1 in 100 chance to shoot lightning in a random direction */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(8, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("BellelDouble",				TWO_HANDED_TRIDENT, /* double attacks */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Techction",				BRONZE_MORNING_STAR, /* technicality and free action when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Damn Ski Wedge",				BRIDGE_MUZZLE, /* your footwear counts as wedge heels and can walk on snow, but if you don't wear shoes, the anastasia and julietta traps are in effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Teleportits",				FLEECY_CORSET, /* inspired by nabru, teleportitis when worn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Vera's Icecube Smash",				LOW_BOOTS, /* initialized to always be cyan sneakers, kicking a monster slows it down unless it's immune to cold or makes a saving throw */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2500L ),

A("Keystrusite",				CLOAK_OF_BLINDNESS_RESISTANCE,
	(SPFX_RESTR|SPFX_XRAY), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Be Hi Without Drugs",				CLOAK_OF_HALLUCINATION_RESISTA, /* funny hallu messages */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Runez Scazmatar",				HOE_SABLE,
	(SPFX_RESTR), 0, 0,
	PHYS(2, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("ScjWillX5",				BLUE_LIGHTSABER, /* makes you slower like spirit, +5 wisdom */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Serialsaber",				GREEN_LIGHTSABER, /* drains less energy the higher your makashi skill is */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Pessivetroin",				BEAMSWORD, /* your training amount in lightsaber forms randomly goes up or down */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Link's Master Sword",				BEAMSWORD, /* fires a beam if thrown */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Actual Beamer",				BEAMSWORD, /* teleportitis */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(2, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Schwillschwillschwillschwillschwill",				BEAMSWORD, /* fires a beam if you attack in melee, monsters spawn doubly invisible, you take maximum damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Djarwethereyet",				LASERXBOW, /* hitting things with crossbow bolts trains djem so */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Multishottemso",				LASERXBOW, /* +1 multishot per djem so skill point */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Spectrate Etteckor",				WEAPON_SIGN, /* resist cold, fire, shock and poison, +10 damage when using shield bash, double bonuses if you're a paladin */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Now It's I-Vel",				RIN_THREE_POINT_SEVEN_PROTECTI, /* evilvariant mode when worn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Variant Guard",				AMULET_OF_GUARDING, /* protection from shape changers, inertia times out very quickly */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Wonder Watering Place",				POT_BOOZE, /* randart2, quaffing gives d10 anti-corona effects */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Secret Recipe",				SCR_CREATE_MONSTER, /* randart2, tells you the hidden power skill */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Upside Down Playing Card",				SCR_CREATE_MONSTER, /* randart, has a chance to be preserved when read */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Berrybrew",				POT_BOOZE, /* randart, quaffing it gives you an additional bottle */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Cursed With The Worst",				RIGHT_MOUSE_BUTTON_STONE, /* randart bad gray stone */
	(SPFX_RESTR|SPFX_EVIL), (SPFX_LUCK), 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Femity Solve",				ELIF_S_JEWEL, /* randart feminism jewel, allows you to #monster to spread the perfume but doing so spawns a perfume monster */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Heavy-Heavy-Babe",				HEAVY_IRON_BALL, /* randart2 */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Hamstrung Four Sure",				IRON_CHAIN, /* randart2, free action when wielded, hitting a monster paralyzes it for one turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Vaper-Baper",				HEAVY_IRON_BALL, /* randart, wielding it may improve your max HP over time if your vaapad skill is high enough */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Tschend For Eternity",				IRON_CHAIN, /* randart, if you're chained to it you have reflection and MR, but if you wield it, 1 in 1000 chance per turn to be punished as long as you aren't already */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Pawnermaster",				DIAMOND, /* randart2 gem, eating it summons a gypsy */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Machine That Goes Pling",				DIAMOND, /* randart gem */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Jonadab's Death Ray Insurance",				LOADSTONE,
	(SPFX_RESTR|SPFX_EVIL), (SPFX_DEFN), 0,
	NO_ATTK, NO_DFNS, CARY(AD_MAGM), 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Jonadab's Reflectiosus",				LOADSTONE,
	(SPFX_RESTR|SPFX_EVIL), (SPFX_REFLECT), 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Jonadab's Stalwartness",				LOADSTONE,
	(SPFX_RESTR|SPFX_EVIL), (SPFX_HPHDAM), 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Donald Thump!",				LOADSTONE, /* by maxlunar, kicking it can cause bad effects */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Jonadab's Metal Guard",				GAUNTLETS_OF_FUMBLING, /* putting them on changes their material to IRON */
	(SPFX_RESTR|SPFX_HSPDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Cockbanger Armor",				LEATHER_ARMOR, /* randart, sets itself to a random value from +1 to +10 if worn while +0 or lower */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Tlahuizcalpantecuhtli",				SHINY_STAR, /* diminished bleeding, allows you to sacrifice your own race when wielded */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Panzer Tank",				LEATHER_SADDLE, /* while riding, you have +10 constitution, 10 extra points of AC and half physical damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Eimi wa bakadesu",				HIPPIE_HEELS, /* if your INT is higher than 10, 1 in 2000 chance per turn to lose one point of it, renames items to japanese, high heels skill trains five times as fast, grenades thrown by you get grenadonin-like damage boosts */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Shut Up You Fuck",				INKA_SADDLE, /* if the steed is hostile, it won't be able to use MS_FART or similar attacks against you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Utter Uselessness",				INKA_SADDLE, /* if you put it on a farting, perfume or superman monster, you'll be able to ride it even if it's not tame */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Chrome Good",				SHOTGUN, /* poison resistance when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Gaygun",				RAYGUN, /* +5 melee damage if you're homosexual, and your footwear (if present) counts as high heels but if you're not homosexual, you're also numbed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Antimagicbane",                  COLLUSION_KNIFE, /* prevents both you and monsters from spellcasting */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_EVIL), 0, 0,
	STUN(3,4),	DFNS(AD_MAGM),	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 3500L ),

A("Gantulets of Mispealing",				REGULAR_GLOVES, /* randart2, trying to engrave Elbereth will always misengrave, other engravings also randomly misengrave */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Sue Lyn's Usage",				TOILET_ROLL, /* replicates maurah trap effect, if it triggers you gain +1 max HP and Pw up to a max of 20*XL; if it's cursed, 1 in 1000 chance per turn to receive d50 turns of inertia */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Mr. Outlash",				PIN_BOLT,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Schokolade-eea",				CHOCOLATE, /* rnz(500) turns of deafness when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Christmas Egg",				CHOCOEGG, /* rn1(1000,1000) turns of half physical damage when eating */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Anikibombo-neau",				SPE_BOMBING, /* no special effects */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Goa-Evilgate",				SCR_SYMBIOSIS, /* can also give symbiotes that only goaulds could normally get (or if you are a goauld, can also give ones that a goauld cannot have) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Pushmicebire",				WAR_HAMMER,
	(SPFX_RESTR|SPFX_DCLAS), 0, (S_RODENT),
	PHYS(0, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("The Secret Book of Venom",				SPE_MAGICTORCH, /* randart2, teaches venom mixing tech if successfully read or increases its tech level by one if you already know it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 15000L ),

A("Aldebaran Form",				SITH_STAFF, /* flying, killing something heals you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Jeditation",				YELLOW_LIGHTSABER,
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	PHYS(0, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("The Wire of Luck",				RELAY, /* +5 luck when wielded */
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Amateurhour's Sigh",				LUCKSTONE, /* +5 luck and fallout effect when carried */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Huro Haehae",				SCR_OFFLEVEL_ITEM, /* creates a monster for each of your items and levelports it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Teh-Hunk",				YELLOW_LIGHTSABER, /* +5 bashing damage and +5 to-hit when unlit; if you're dual-wielding and this weapon is in your off-hand and your primary weapon is a launcher, non-ridden pole, lightsaber or non-weapon, it also gets +5 to-hit */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Thermo Nuclear Chamber",				SHIELD, /* randart, fallout, lightsabers drain energy at one third of the usual rate, makes your AC worse by 10 points and cannot block */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Super Energy Lines",				SHIELD, /* randart2, lightsabers drain energy at half of the usual rate, makes your AC worse by 10 points and cannot block */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Sarah's Sneakers of Instant Sadness",				SOFT_SNEAKERS, /* resist cold, shock and confusion, but 1 in 5000 chance that they are transferred to a randomly generated monster and levelported away */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Lamerang",				BOOMERANG,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Sexcalibur",				ROSE_WHIP, /* chatting causes you to have sex, like a kurwa, heavily autocurses (note that it is not a bug that this particular artifact doesn't have the evil flag) */
	(SPFX_RESTR|SPFX_SEEK|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH),0,0,
	PHYS(5,10),	DRLI(0,0),	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Slexcalibur",				TIN_OPENER, /* monsters that have no M5 flags for their origin have +5 spawn freq */
	(SPFX_RESTR|SPFX_SEEK|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH),0,0,
	PHYS(5,10),	DRLI(0,0),	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Flockdown",				WHITE_DRAGON_SCALES, /* by Amy's roommate, transforms terrain you walk over into snow */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Flingtile",				BONE_ARROW,
	(SPFX_RESTR), 0, 0,
	PHYS(6, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 40L ),

A("Al'azzared Gyeth Gyeth",				BONE_DAGGER, /* diminished bleeding */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Pouschen Oph Gaein Lewel",				WRAITHBLADE,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Tezcatlipoca's Bubblestorm",				TECPATL, /* waterproofs your inventory, swimming and unbreathing when wielded, 1 in 100 per turn to contract corona */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ATTK), 0, 0,
	PHYS(10, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("You Lil Pussy",				SPRAY_BOX, /* chance to paralyze the target */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Regularer Anchor",				ANCUS, /* protects you from eel drowning attacks in water */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Stop The Airships",				ANCUS, /* flying enemies have 1 in 6 chance to miss their turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Fire-Branded Hellhunk",				ANCUS,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(4, 10), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Seeveen",				ANCUS, /* +2 light radius when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Birdbane",				ATGEIR,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_BAT,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Rasschen Taak",				KHOPESH, /* autocurses, soviet mode when wielded, improves spellcasting chances like failuncap equipment */
	(SPFX_RESTR|SPFX_PROTEC|SPFX_EVIL), 0, 0,
	PHYS(2, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Smoking Squirrel",				MACUAHUITL, /* etherwind when wielded, full nutrients and discount action */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("SpeerThrow",				ATLATL, /* +2 multishot when throwing spears */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Neko's Whiteout",				TINSEL_LONGSWORD,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, CHOCOLATE_CREATION, A_NONE, NON_PM, NON_PM, 4000L ),

A("Klima Pledsche",				TREESTAFF, /* poison resistance when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Bimmselimmelimm",				GARBOWHIP, /* +10% chance to block when wearing a shield, but if your shield actually blocks something, 1 in 50 chance for it to dull up to -20 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(20, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Vampire-Sing's Bloodrush",				GROM_AXE, /* sing trap effect */
	(SPFX_RESTR|SPFX_EVIL|SPFX_DEFN|SPFX_ATTK|SPFX_DRLI), 0, 0,
	DRLI(5, 6), DRLI(0,0), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Ultimate World Fall",				HONOR_KATANA, /* activates ragnarok timer, teaches world fall technique */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(24, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Death's Fathom",				DIMENSIONAL_SHARD, /* resist lightning and fire */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ATTK), 0, 0,
	COLD(10, 16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Zantasbore",				CIRCULAR_SAW, /* very fast speed if wielded */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(4, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Strike The Ball",				AMBUSH_QATAR, /* 1 in 1000 chance to go berserk if not already berserk */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Hachurate",				PHEONIX_STAFF, /* very fast speed if wielded, +5 charisma */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Ore Evil Wight",				JUMPING_FLAMER, /* paralyzes target for 3 turns */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Woh",				SECRET_SOUND_WHIP,
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Elvis's Bones File",				CANDY_BRA, /* bones effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Capital Rap",				CANDY_BRA, /* sound effects, +5 charisma, double skill training */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Silks of the Victor",				OLD_MAIL, /* manaleech, +1 light radius, +5 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Healthier Feel",				ORGANOSHIELD, /* resist poison, sickness and drain */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Whitewater Rafting",				DISPERSION_BOX, /* waterproofs inventory while carried */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Daisy's Handbag",				HANDYBAG, /* +1000 carry cap and +500 max carry cap while carried */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Minnie's Handbag",				HANDYBAG, /* can be used even if both of your hands are curse-welded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Ilja's Assholery",				CONTROVERSY_CODE, /* +d10 bleeding damage to a target that has a head and isn't wearing a helmet */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Attic Code",				CONTROVERSY_CODE, /* wielding it allows you to enter Green Cross and the various ZAPM sub-branches even if they're closed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50000L ),

A("Vader's Charge",				BLACK_LIGHTSABER, /* spawns with extra-high charge count */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Kyle's Jackpot",				ORANGE_LIGHTSABER, /* gains extra charge with charge saber too */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Puckock",				HEAVY_LASER_BALL, /* if punished with this one, throwing it deals 30 extra damage */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Ero-Ero-Ero-Ero-Mushroom-Mushroom",				LASER_CHAIN, /* if punished with this one, you can use melee weapons as polearms like the mushroom race */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Cutrition",				LASER_TIN_OPENER, /* full nutrients */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Fisto!",				POWERFIST,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Pneumatic Force",				LASERFIST,
	(SPFX_RESTR), 0, 0,
	PHYS(7, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 9000L ),

A("Tavion's Charge",				RED_LIGHTSABER, /* randart lightsaber, spawns with rnz(5000) extra charge */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Desann's Wrath",				RED_LIGHTSABER, /* randart2 lightsaber, -8 to-hit, gains extra charge when charged with something other than charge saber */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Boah What A Stack",				WONDER_ARROW, /* spawns with 200 extra ammo */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Niner",				HEAVY_CROSSBOW_BOLT, /* spawning enchantment is increased by 9 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 70L ),

A("Self-Sufficience",				AMBER_FRAGMENT, /* when hitting a monster with it, uncurses itself and loses a point of negative enchantment (if present) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Slittem Off",				MYSTERY_SHURIKEN,
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Zieieie!",				BFG_AMMO, /* spawns with 400 extra ammo */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 40L ),

A("Maximum Penalty",				SCR_COURSE_TRAVELING, /* 10 super-bad effects when someone reads it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Gluckercharge",				CYAN_DOUBLE_LIGHTSABER, /* gives an additional 4 charges when training a lightsaber form */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Lets Make It Official",				KLIUSLING, /* sight bonus at basic kliu skill, infravision at skilled, free action at expert, control magic at master, reflection at grand master */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Mindturner",				INFERIOR_BOLT, /* 5% chance to change the target's gender */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Change The Play",				DIVINE_DART, /* if you're restricted with darts and train the skill high enough to reach the cap of your shuriken skill, you lose the shuriken skill and gain the dart skill instead */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Pauered By The Cap",				ARROW, /* randart ammo, +5 damage multiplied by the cap in its corresponding skill but if you hit something with it, 10% chance that the cap permanently decreases by one skill level */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Strew Anywhere",				ARROW, /* randart2 ammo, always fires in a random direction */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Fuel Nail",				DROVEN_BOLT, /* can be invoked once to spawn a tame Roxanne, which trains squeaking by 20 points and then you can chat to Roxanne and give her bolts to temporarily give her extra poison attacks */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Trapperate",				WINDMILL_BLADE, /* if it hits a monster and there's no trap on its square, create one */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 60L ),

A("Femmy Loves You",				INKA_STINGER, /* if you hit something, +d1000 turns for femmy trap effect */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Powered By Hunger",				TAR_STAR, /* every time you throw one, you get hungry by 50 points */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DRLI), 0, 0,
	DRLI(0, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Dessintergrate",				ARROW, /* randart ammo, never stacks */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 28), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Piercethrough",				ARROW, /* randart2 ammo, can continue when colliding with a monster */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Actually The Matter Comes From Switzerland",				HEAVY_CROSSBOW_BOLT, /* always has exactly 90% chance of being preserved, regardless of anything that modifies mulching odds */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Dam Hit",				ARROW,
	(SPFX_RESTR), 0, 0,
	PHYS(2, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 30L ),

A("Re-Poison",				ORCISH_ARROW, /* can be invoked to poison them */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 40L ),

A("Stelshot",				ELVEN_ARROW, /* hitting something with them gives +200 turns of stealth */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Mattertack",				DARK_ELVEN_ARROW,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ACID(5, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 80L ),

A("Thousand Fragments",				DROVEN_ARROW, /* bisects targets */
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Nuka Shot",				METEOR_ARROW,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(5, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Use-A-Lot",				BRONZE_ARROW, /* if it would have mulched, 90% chance that it doesn't */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Ampersand Eraser",				SILVER_PISTOL_BULLET,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_DEMON,
	PHYS(20, 50), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 70L ),

A("Moretrain",				BLASTER_BOLT, /* hitting something with them trains firearms by an additional 5 and gun control by one point */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Kaflummp",				ROCKET,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Loitemup",				GAS_GRENADE, /* igniting them trains firearms and gun control by an extra 2 points */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Elven Spandex",				SMALL_PIECE_OF_UNREFINED_MITHR,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Trackstop",				SILVER_SLINGSTONE, /* hitting a monster that can currently move will paralyze it for 2 turns */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 70L ),

A("Flusch",				TALC, /* always fires in the wrong direction */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 30L ),

A("Become Normal",				GRAPHITE, /* hitting a monster with it causes a random graphite to spawn at your feet, which can never be an artifact */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Razorshard",				VOLCANIC_GLASS_FRAGMENT, /* hitting a monster causes it to bleed for 10 (more) turns */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 60L ),

A("Superhard Stone",				CONUNDRUM_NUGGET,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Map End",				METEOR_FRAGMENT, /* hitting something with it causes a fireball to be shot on a random square */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Zackschrack",				ARROW, /* bashing with it deals +5 damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5L ),

A("Hunksterman",				LEATHER_ARMOR, /* randart2, hunk form gives +1 damage when bashing with a lightsaber and +1 to-hit when bashing in general, speed bug, monster speed bug */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Lu Nonname",				LEATHER_ARMOR, /* randart2, kliu form gives +1 multishot when firing a sling, ludgera trap effect, petrification resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Alpha Tauri",				LEATHER_ARMOR, /* randart2, aldebaran form heals you when you kill something while flying, and more if you have a lit lightsaber, elona monsters have +20 spawn chance, scaling bug */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("In Awe",				ANNEMARIE_PLATFORM_SANDALS, /* +5 DEX, +7 CHA */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("FlySkyHgh",				JIL_VELCRO_SHOES, /* flying, random MS_SHOE have 50% chance of spawning peaceful */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Gravy Hide",				JANA_SNEAKERS, /* infravision, cold resistance and +5 digging speed, but reading a scroll of phase door or teleportation has 1% chance of instakilling you (no joke). */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Psi-Onic",				KATRIN_COMBAT_BOOTS, /* resist psi and contamination */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Onset of Winter",				GUDRUN_WEDGE_SANDALS, /* resist cold, potions cannot be destroyed by cold, can walk on snow and ice */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Hersay Price",				ELLA_WEDGE_SANDALS, /* heavily autocurses, monsters need a +1 weapon to hit you effectively, but you go berserk with a 1 in 100 chance if not already berserk, all monsters spawn hostile, and items randomly teleport out of your inventory */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Very Prostituted",				MANUELA_COMBAT_BOOTS, /* can seduce monsters by chatting to them but also gives the kurwa claw weakness */
	(SPFX_RESTR|SPFX_EVIL|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Way Too Soft",				JENNIFER_COMBAT_BOOTS, /* invoke to get a shadow mantle-like effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 2000L ),

A("Silvesterblam",				PATRICIA_STEEL_CAPPED_SANDALS, /* resist fire, +3 kick damage, being caught in a grenade explosion damages or eventually destroys this artifact */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Dyke Resists",				ANTJE_SNEAKERS, /* resist cold and shock */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Double Desire",				ANTJE_COMBAT_BOOTS, /* increases chance that you have to play the minigame, resist disintegration */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Shockgene",				KERSTIN_COMBAT_BOOTS,
	(SPFX_RESTR|SPFX_EVIL|SPFX_REGEN|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Cutesy Girl",				KERSTIN_WOODEN_SANDALS, /* resist drain and psi */
	(SPFX_RESTR|SPFX_EVIL|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Take Won Do",				KERSTIN_SNEAKERS, /* if you're restricted in martial arts and manage to get the martial arts skill training up to 2500, the skill unlocks itself with a cap of expert */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Oh The Elements",				LAURA_WINTER_BOOTS, /* resist fire, cold and shock; if at least 200 of your items have been destroyed by fire, learn afterburner technique; 200 items destroyed by cold lets you learn buggard; 200 items destroyed by shock lets you learn thunderstorm; if you have all three of these, learn the grand master orb techs */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Exciting Spflotch",				LARISSA_PLATFORM_BOOTS, /* cannot be destroyed by shit traps, 2 extra points of AC, resist disintegration and petrification, +2 kick damage */
	(SPFX_RESTR|SPFX_EVIL|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Skin And Bone",				NORA_COMBAT_BOOTS, /* caps your STR and DEX at 6, adds 20% extra movement speed (but not more than 6) */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Sandalmens",				NATALIA_WEDGE_SANDALS, /* if male, petrification resistance; if female, diminished bleeding */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Away You Stalker",				SUSANNE_COMBAT_BOOTS, /* see all M2_STALK */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Stolp",				LISA_COMBAT_BOOTS, /* hitting an intelligent M2_FEMALE in melee causes that monster to be blinded */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Callgirl",				BRIDGHITTE_SKI_HEELS, /* special invoke that replicates summon shoe technique */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Lovelane",				JULIA_COMBAT_BOOTS, /* +5 charisma */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Dark Ball of Light",				NICOLE_COMBAT_BOOTS, /* becomes sticky heavily cursed when equipped, weak sight, causes darkness effect on you every turn, slows you down like a spirit, but monsters have appr=0 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Giggling",				RITA_STILETTOS, /* sets itself to -10 when equipped */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Thosefromthere",				JANINA_LADY_PUMPS, /* resist sleep */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Too Old Model",				LOW_BOOTS, /* initialized to always be block chocolate heel boots, +3 CHA, resist death rays */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Dampener",				LOW_BOOTS, /* initialized to always be platform sneakers, 5 extra points of AC */
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("May-Britt's Adulthood",				LOW_BOOTS, /* initialized to always be iceblock heels, resist cold, wakeup call effect, +1 increase damage but -2 increase accuracy */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Lydia's Sexyroom",				LOW_BOOTS, /* initialized to always be paragraph shoes, diminished bleeding, +5 CHA */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Sharpspike",				LOW_BOOTS, /* initialized to always be paragraph sandals, resist death */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Wedding Challenge",				LOW_BOOTS, /* initialized to always be bridal shoes, jessica trap effect, +10 CHA, resist shock */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, HEALING, A_NONE, NON_PM, NON_PM, 3000L ),

A("Anaconda Heels",				LOW_BOOTS, /* initialized to always be pistol boots, gives +12 enchantment to bullets fired from them, but the bullets can never be preserved */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3600L ),

A("Jana's Deviousness",				LOW_BOOTS, /* initialized to always be ng shoes, does bad stuff if you view its description */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Playing Anastasia",				LOW_BOOTS, /* initialized to always be chelsea boots, anastasia trap effect, resist petrification */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Lilly's Secret Agency",				ROSA_GIRL_SHOES, /* invoke causes role recursion but summons a peaceful Lilly who will steal the shoes and branchport away */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Dis Is E Prisem",				RAINBOW_LIGHTSABER, /* reflection when lit */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Constant Change",				RAINBOW_LIGHTSABER, /* when lit, changes color and material every turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Switch To Another",				RAINBOW_LIGHTSABER, /* 1 in 1000 per turn to change to a random other lightsaber-type item; if this causes it to become something two-handed, your secondary hand is automatically emptied */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Plantoplim",				LEATHER_ARMOR, /* randart2, polgo form works for all lightsabers, 1 in 2500 that you're forced to equip a cursed implant if you don't have one yet, heavily autocurses, bank trap, deactivates invisibility and displacement, cannot remove implants at all */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Elmhere",				LEATHER_ARMOR, /* randart2, firga form works without lightsaber, fainting, deactivates free action, elmstreet effect, improves AC by 5 points while you're paralyzed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Ne Prosto Krasivo",				KSENIA_PLATFORM_SANDALS, /* +3 kick damage, resist cold, renames items to russian, +4 CHA */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Sexroom For Free",				LYDIA_LADY_PUMPS, /* +5 CHA, won't be removed if you have sex */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Full Program",				CONNY_COMBAT_BOOTS, /* kicking a monster deals +1 damage and causes 3 points of bleeding damage to it, discount action */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Bend From The Noise",				KATIA_COMBAT_BOOTS, /* deafness, resist stone and sleep, causes darkness effect every turn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Exhaust Damage",				MARIYA_COMBAT_BOOTS, /* resist death and disintegration, MS_CONVERT doesn't affect you negatively; if a MS_FART_LOUD farts at you, -1 alla and +1 trembling */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("So Wonderfully Fluffy-Soft",				ELISE_HIPPIE_HEELS, /* also gives intrinsic half physical damage and prevents fumbling */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Heels Made Of Cuddle Cloth",				RONJA_COMBAT_BOOTS,
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM|SPFX_HSPDAM|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Got That Starwars Entrance",				ARIANE_COMBAT_BOOTS, /* diminished bleeding, can use medical kit without fail, can be invoked to open the green cross dungeon */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 6000L ),

A("Say The Wrong Line",				JOHANNA_COMBAT_BOOTS, /* +2 kick damage against male monsters, but 1 in 100 chance that they curse the boots and disenchant them up to -20 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Is There A Sound?",				INGE_COMBAT_BOOTS, /* deafness, sound effects, monster sound effect */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Jasieen's Fear",				LOW_BOOTS, /* initialized to always be wedge espadrilles, makes you permanently feared until you take them off, but adds speed like the intrinsic, which stacks with other speed sources */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("She Really Likes It",				LOW_BOOTS, /* initialized to always be treaded heels, larissa and dora trap effects, monsters need a +1 weapon to hit you effectively, skill loss effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("P-p-p-p!",				SMG_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Sustained Fires",				ANTIMATTER_SMG_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(20, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("BOOM BOOM BOOM BOOM BOOM BOOM BOOM",				MG_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(2, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("ShredShredShred",				ANTIMATTER_MG_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(20, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Totally The Cunt",				RIFLE_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Head Shot!",				ANTIMATTER_RIFLE_BULLET,
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	PHYS(0, 50), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Paeaeaeaeaeaeau Klink Klink",				SNIPER_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Turn Into Ash Heap",				ANTIMATTER_SNIPER_BULLET, /* tries to cancel the target, but it can also resist */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 40), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Reuseme",				ASSAULT_RIFLE_BULLET, /* 75% chance that it may be preserved like lead ammo */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Heavydrain",				ANTIMATTER_ASSAULT_RIFLE_BULLE, /* reduces target's max HP by one */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Damn I'm Good",				RADIO, /* tries to slow down the target, but it can also resist */
	(SPFX_RESTR), 0, 0,
	PHYS(10, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Tschieieieieie",				RAYGUN_BOLT,
	(SPFX_RESTR), 0, 0,
	PHYS(20, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 150L ),

A("Combat Pellet",				AUTO_SHOTGUN_SHELL, /* 90% chance that it may be preserved like lead ammo */
	(SPFX_RESTR), 0, 0,
	PHYS(12, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Rare 10mm",				LEAD_SMG_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(2, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("You And Your Para",				LEAD_MG_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("7.62mm Warzone",				LEAD_RIFLE_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Legendary Shot",				LEAD_SNIPER_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(20, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("5.56 Stockpile",				LEAD_ASSAULT_RIFLE_BULLET, /* +400 ammo when the stack is generated */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Tschongboeller",				AUTO_LEAD_SHOT,
	(SPFX_RESTR), 0, 0,
	PHYS(8, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Everything Greens So Green",				CHROME_PELLET, /* 100% chance to not be removed, but can still mulch */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 150L ),

A("Tschamm",				COPPER_RIFLE_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Partial Mantle Projectile",				PLATINUM_SNIPER_BULLET, /* additionally deals 10% of the monster's current HP as damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Barrowgrim's Army",				GRIM_SHIELD, /* drain resistance when worn */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Peace Brother",				CLOAK_OF_PAIN_SENSE, /* peacevision when worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Heatem Up",				ONE_EATING_SIGN, /* double fire resistance when worn but disables cold resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Bidethander",				BIDENHANDER, /* thai and ludgera trap effects, MS_FART_LOUD spawn with +20 crapbonus, suggested by amateurhour */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(1, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Justice For Garlic",				CLOVE_OF_GARLIC, /* +5 increase accuracy when wielded (applies to ranged weapons too), name suggested by amateurhour */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Divine Garlic",				CLOVE_OF_GARLIC, /* +5 damage to vampires and drains their maximum HP by one when hitting, skips the magic resistance roll for vampires so it's more likely to affect them; by amateurhour */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Hope of Sokoban",				PICK_AXE, /* dimmopathy, 66% chance to avoid luck penalties in sokoban while wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Dreamoule Eleyele",				SECRET_WHIP, /* wonderlegs */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Puzzle Me Maybe",				RIN_SPECIAL_EFFECTS, /* drunken boxing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Porter's Think",				LORICATED_CLOAK, /* stunnopathy, teleportitis */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Lissie's Sheagentur",				LOW_BOOTS, /* initialized to always be fleecy boots, +3 CHA, fear factor, resist psi, wakeup call and bad alignment effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Schoscho Barbituer",				CHEMISTRY_SPACE_AXE, /* sickopathy, resist poison and acid, permapoisoned like Dirge but reduces your alignment by one whenever you hit with any poisoned weapon */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Siechelaluer",				FRILLED_APRON, /* freezopathy, low stats, +5 increased melee damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Bale of Boden Speedstock",				FAILED_ATTEMPT_BOOTS, /* burnopathy, very fast speed and adds extra speed sometimes, time goes by faster */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Sa Bla Ndo Mwa",				PROCESS_CARD, /* hallu party */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("The End of Lewdness",				LEATHER_SHOES, /* numbopathy, monsters cannot purposefully fart anymore and can't force you to have sex with them, prevents you from taking off any armor or rings, disables speed and steals intrinsic speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Verscent!",				SEDGE_HAT, /* corrosivity, all MS_STENCH and perfume spreader monsters try to move away from you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Brighte See, Eeee",				CIGARETTE, /* stoned chilling, sight bonus */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ATTK), 0, 0,
	FIRE(4, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Kamahewa",				GAUNTLETS_OF_SAFEGUARD, /* glib combat; using appraisal on them gives +1 up to a maximum of +7 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Arshtrittmesser",				MEASURER, /* by amateurhour, doesn't give -4 to-hit penalty when restricted */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Oleg's Attack",				BRASS_KNUCKLES,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Basta Time",				SUPER_KNUCKLES,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(7, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Boindil's Choice",				ULTRA_KNUCKLES, /* very fast speed when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Munzur's Clubmate",				ELITE_KNUCKLES, /* while wielding it, all pets regenerate an extra HP per turn */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	PHYS(5, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Becht's Ride",				LEATHER_SADDLE, /* mounts with less than 12 speed behave as if they had 12 speed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Becht's Speedmobile",				LEATHER_SADDLE, /* mounts with less than 15 speed behave as if they had 15 speed; your AC is worse by 20% */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Mount of the Mad Potato",				LEATHER_SADDLE, /* mount always counts as dedicated steed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Hovercraft Ride",				INKA_SADDLE, /* if the mount is a vortex, it doesn't get a speed reduction if your riding skill is low, and in fact gives 50% bonus to movement speed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Green Assistant",				SURVIVAL_KNIFE, /* doesn't drop if you're glibbed */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Lettuce Blunt",				CIGAR,
	(SPFX_RESTR|SPFX_HALRES|SPFX_EVIL), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2500L ),

A("Grand Theft Auto",				LEATHER_SADDLE, /* putting it on a MS_CAR allows you to ride it, even if it's not tame */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Driver's License",				LEATHER_SADDLE, /* riding a MS_CAR gives 40% extra speed for everything */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Satan's Secret Storage",				STATUE, /* can apply it and put in or take out items like a container */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Sweet Sweet Lana",				CHOCOLATE, /* eating it creates a stack of 16 additional chocolates that cannot be artifacts */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Quadratic Practic Bang Bag",				CHOCOLATE, /* invoking it requires you to select a scroll in your inventory, which will be used up */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, CHOCOLATE_CREATION, A_NONE, NON_PM, NON_PM, 2000L ),

A("Pooling",				SYMBIOTE, /* if you use it while you already have a symbiote, it turns back into a monster */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Incredible View",				BUGXPLORE_HELMET, /* gives all foo-o-pathies as extrinsics */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Waiting For Melee",				YAWNING_VISOR, /* +2 melee damage, -2 melee and ranged to-hit, 15% more spellcasting failure */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Super Graphics Card Bug",				AMULET_OF_TRAP_WARPING, /* sight bonus, morphs every color to the next one */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Fall Warting Number",				RIN_INCESSANT_FARTING, /* +2 wisdom */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Welltravel",				RIN_WALLFARTING, /* randomly generated gold stacks have 50% more gold */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Yohualli Tezcatl",				TEZ_SHIELD, /* +2 DEX, blindness resistance, invoke to fix all status effects */
	(SPFX_RESTR|SPFX_EVIL|SPFX_DEFN|SPFX_HSPDAM), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 6000L ),

A("Truly Magnified",				KILLER_SPAWN_BOOTS, /* clairvoyance, multiplies killer room monster amount by d3 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Soarup",				STANDARD_HELMET, /* always initialized to be higher helmet, flying, if it's +0 when you put it on it becomes +1 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Safari Rockz",				LOW_BOOTS, /* initialized to always be missys, allows you to walk through trees, scent view */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Spirit Rockz",				LOW_BOOTS, /* initialized to always be missys, wallwalking */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Purple Jungle",				LOW_BOOTS, /* initialized to always be missys, allows you to cross farmland and you regenerate an extra HP while being in farmland */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Eazy Going",				LOW_BOOTS, /* initialized to always be missys */
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Simply White",				LOW_BOOTS, /* initialized to always be missys, cold resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Simply Black",				LOW_BOOTS, /* initialized to always be missys, disint resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Wood Feeling",				LOW_BOOTS, /* initialized to always be missys, stone resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("College Rockz",				LOW_BOOTS, /* initialized to always be missys, +3 INT/WIS when worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Welcome On Board",				LOW_BOOTS, /* initialized to always be missys, swimming and your items don't get wet */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Wonderful Lotus",				LOW_BOOTS, /* initialized to always be missys, poison resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Underwater Love",				LOW_BOOTS, /* initialized to always be missys, unbreathing and your items don't get wet */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Sweet Cherry",				LOW_BOOTS, /* initialized to always be missys, full nutrients */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Tire Rockz",				LOW_BOOTS, /* initialized to always be missys, 40% faster movement when riding */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Happy Cloud",				LOW_BOOTS, /* initialized to always be missys, you transform regular floor into clouds when moving, +5 luck */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Delfi Rockz",				LOW_BOOTS, /* initialized to always be missys, prayer timeout is 3x faster and tells you when it becomes 0 again */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("All Time Favz",				LOW_BOOTS, /* initialized to always be missys */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Rockz Army",				LOW_BOOTS, /* initialized to always be missys, 10 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Sketch It",				LOW_BOOTS, /* initialized to always be missys, clairvoyance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Pokerface",				LOW_BOOTS, /* initialized to always be missys */
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("White Karma",				LOW_BOOTS, /* initialized to always be missys, gain 1 alignment with a 1 in 100 chance per turn (not beyond the maximum) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Street Rockz",				LOW_BOOTS, /* initialized to always be missys, +2 increase damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Black Mandala",				LOW_BOOTS, /* initialized to always be missys */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("My Love",				LOW_BOOTS, /* initialized to always be missys, resist fire */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Yes I Rockz",				LOW_BOOTS, /* initialized to always be missys, blindness resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Bunny Rockz",				LOW_BOOTS, /* initialized to always be missys, peacevision */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Cat Rockz",				LOW_BOOTS, /* initialized to always be missys, +5 increase accuracy */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Scan Me",				LOW_BOOTS, /* initialized to always be missys, supertelepathy */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Bacteria",				LOW_BOOTS, /* initialized to always be missys, sickness resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Blockparry",				MAIN_SWORD, /* if it's in your secondary hand, 30% chance to block + 2% for every enchantment or dual-wielding skill level */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Profane Streichenblockansatz",			MIRROR_SHIELD, /* by amateurhour, deals d(mlvl) passive damage to monsters meleeing you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Prickbuff",				ELVEN_GAUNTLETS, /* if you attack unarmed, the monster bleeds for d5 (more) turns */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2400L ),

A("Final Consequence",				LONG_SWORD, /* randart, always deals maximum base melee damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Autoswitch",				SPLINT_MAIL, /* can be invoked to fix all nastytrap effects, but this deletes the armor */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 10000L ),

A("Versus Instadeath",				SPLINT_MAIL, /* lets you survive an instadeath once, but is used up in the process */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Going Display",				SPLINT_MAIL, /* invoke fixes the display loss nastytrap effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 2000L ),

A("Secret Course Vacuum Cleaner",				SPLINT_MAIL, /* allows you to enter Green Cross */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Third Next Mission",				SPLINT_MAIL, /* can be invoked for self-banishment */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 6000L ),

A("Nothingatallbane",				SPLINT_MAIL, /* melee attacks cause you to fire invisible beams, but the description doesn't say so */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Demo Case",				SPLINT_MAIL, /* can be invoked to create a landmine trap at your location */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 1000L ),

A("Spy Em Out",				SPLINT_MAIL, /* farlooking a monster displays the status effects it has (if any) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("See The Entire Map",				SPLINT_MAIL, /* can be invoked for magic mapping, but 20% chance that the armor disappears */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 8000L ),

A("Backpack Bomber",				SPLINT_MAIL, /* can be invoked to cause a dynamite explosion */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 6000L ),

A("Lasershit",				SPLINT_MAIL, /* can be invoked to create a s-pressing trap on your location */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 4000L ),

A("Assslasher Katana",				KATANA, /* by Demo, invoking it while male will neuter you */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 2000L ),

A("Suckstone",				SLEEPSTONE, /* stealer trap and elmstreet effect while carried and +5 luck */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Player Radar",				SPLINT_MAIL, /* displays all player monsters while worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("tz8;glgg!3aIv$fm03",				SPLINT_MAIL, /* can be invoked to fire a disintegration beam; trying to view its description displays nothing and paralyzes you for d30 turns */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 20000L ),

A("Go Away Type Of Arms",				SPLINT_MAIL, /* can be invoked to delete an adjacent shopkeeper, but is used up in the process */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 500L ),

A("Avenger Appearance",				SPLINT_MAIL, /* can be invoked to spawn two tame MS_BOSS, one of which is male and the other is female, but is used up in the process */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 18000L ),

A("Complete Reset",				SPLINT_MAIL, /* invoke causes world fall that is guaranteed to kill everything whose level isn't higher than yours, but you need to be at least XL15 and are resetted to XL1, and you'll lose all training in all skills, all spells are erased, all techniques get a timeout of rnz(100000) and your score, which has to be at least 2 million for this to work, is set back to 0, plus you die when you invoke it */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 10000L ),

A("Radiogram Overhearing",				SPLINT_MAIL, /* you sense the arrival of every monster */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("MG Rattering",				SPLINT_MAIL, /* MG bullets don't disappear automatically */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Orbcharge",				SPLINT_MAIL, /* if you equip it while not knowing the buggard technique, you learn it but detect monsters is deactivated for 1 million turns */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Curse Purify",				SPLINT_MAIL, /* can be invoked to attempt to uncurse your entire open inventory, but is used up in the process */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 10000L ),

A("effect that is DEACTIVATED",				LEVITATION_BOOTS, /* prevents levitation */
	(SPFX_RESTR), 0, 0, /* not SPFX_EVIL, this is intentional */
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Purple Spectrum",				VIOLET_LIGHTSABER, /* resist fire, cold, elec and poison */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Occasional Eeh",				ORANGE_LIGHTSABER,
	(SPFX_RESTR|SPFX_DRLI|SPFX_ATTK|SPFX_EVIL), 0, 0,
	DRLI(0, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Damnblast",				BLACK_LIGHTSABER,
	(SPFX_RESTR), 0, 0,
	PHYS(1, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Collecting Expansion",				SPLINT_MAIL, /* +2000 carry cap and +5000 max carry cap */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 30000L ),

A("Kerstin's Cowboy Boost",				LEATHER_SADDLE, /* riding a steed with this saddle gives kerstin trap effect, 20% increased movement speed, +2 increase damage and to-hit and 5 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Experimental Chunk",				CLUB, /* resist psi, contamination and death */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Rotate Me",				SPLINT_MAIL, /* can be invoked to play blackjack */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 5000L ),

A("Julia's Slippers",				LOW_BOOTS, /* initialized to always be house slippers, by Demo, see invisible */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Que Tangherisonja",				ELIF_S_JEWEL, /* randart feminism jewel, poison resistance when carried, entering a new level spawns a perfume monster */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Fluidshield",				SHIELD, /* randart, reduces fluidator spawns by a factor 10 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Gat From Hell",				ASSAULT_RIFLE, /* by Demo, every bullet you fire from it reduces your max HP by one */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(3,20), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Vivarium Stick",				WAN_NOTHING, /* randart, zapping it changes its material to "viva" */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Key to the Golden Items",				SECRET_KEY,
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 25000L ),

A("Key to the Green Items",				SECRET_KEY,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 5000L ),

A("Invstate",				CLOAK_OF_INVERSION, /* intrinsic reflection when worn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Open Retribution",				INVERSION_SHIELD, /* rays don't lose range when they hit something */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Bernceld",				RIN_WINCING, /* deactivates cold resistance, grants infravision and sight bonus */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2400L ),

A("Undead Stink",				RUTH_SNEAKERS, /* resist poison, undead warning, aggravate monster, 1 in 1000 chance per turn that rnz(5) turns of stasis happen */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Scratch The Shred",				MAGDALENA_COMBAT_BOOTS, /* if you kick a monster that has less than 10% of its health left, the monster dies */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Soft Karate Kick",				MARLEEN_COMBAT_BOOTS, /* +2 CHA, kicking a monster while martial arts is below master deals more damage the lower your MA skill is */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Front Target",				KLARA_PLATFORM_SANDALS, /* resist disintegration and fear, your kick cannot be clumsy */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("As Strong As Boots",				FRIEDERIKE_DANCING_SHOES, /* resist petrification, kicking deals +5 damage and has 10% chance of paralyzing the target for d5 turns */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Laugh When You Fall",				NAOMI_STILETTOS, /* while paralyzed, you have +5 charisma and monsters with appr=1 have 90% chance to have appr=0 instead */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Live Among The Wild",				UTE_PLATFORM_SANDALS, /* animals have 50% chance of being spawned peaceful */
	(SPFX_RESTR|SPFX_EVIL|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Dad's Money",				JASIEEN_WEDGE_SANDALS, /* every time you score a trophy, you gain 1000 zorkmids and get a mocking message from your dad */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Barbed Hook Zipper",				YASAMAN_LEATHER_BOOTS, /* resist petrification and level drain, hostile monsters do not use perfume against you (but frenzied ones still do), 1 in 1000 that you get rnd(20 + (level_difficulty * 3)) turns of bleeding and wounded legs, kicking a monster has 1% chance that it needs to pass a MR check or become peaceful */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Little Ice Block With The White Skirt",				MAY_BRITT_SANDALS, /* resist cold, freezopathy, protects potions from cold, can walk on snow and ice, immunity to snowstorms, +2 CHA */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Arvogenia's Biker Heels",				LOW_BOOTS, /* initialized to always be stone-carved heels; while riding, you can jump and deal +5 kicking damage, kicking the steed gives double gallop time */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Precursor to the 69",				LOW_BOOTS, /* initialized to always be homo shoes, intrinsic speed, using attire charm or beauty charm gives +1 CHA */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Heelmarji",				LOW_BOOTS, /* initialized to always be devil's lettuce shoes, cone heels */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("She's Still An Anastasia",				LOW_BOOTS, /* initialized to always be loafers, unchanging, anastasia trap effect, resist psi */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Killcap",				LOW_BOOTS, /* initialized to always be sweaty shoes, monsters that need +X weapons to be hit can be hit with everything, -2 INT and your INT is capped at 6 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("PSG",				SNIPER_RIFLE, /* pushes you away after every bullet fired */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Borked Para",				HEAVY_MACHINE_GUN, /* pushes you away after every bullet fired */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5500L ),

A("Wrong Rune",				PLATE_MAIL, /* invoke to levelport to level 62 of Gehennom */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 2000L ),

A("Mark and Recall",				PLATE_MAIL, /* invoke to mark the current level, then invoke again to instantly levelport to that level */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 8000L ),

A("Strong Enchantment",				PLATE_MAIL, /* 10 extra points of AC, acid resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Airstrike!",				PLATE_MAIL, /* invoke to get a target cursor, and dynamite explosion at the square you pick */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 5000L ),

A("Randart Reroll",				PLATE_MAIL, /* invoke to reroll all randarts (artifact.c) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 10000L ),

A("Skill Reset",				PLATE_MAIL, /* invoke to undo all skill points you've spent */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 10000L ),

A("Build A Wall",				PLATE_MAIL, /* invoke to transform terrain into rock walls on squares whose x and/or y coordinates match yours */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 4000L ),

A("Blankit Now",				PLATE_MAIL, /* invoke to remove the enchantment ("egotype") from an armor piece */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 6000L ),

A("Wild Whirling",				RED_DOUBLE_LIGHTSABER, /* -8 to-hit, double attacks */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Artitfact",				CANDY_BRA, /* special effect only when you're female: +5 CHA, monsters have 20% chance to miss their turn, but claw attacks always proc their special anti-female effect. */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Entertaining Clear",				PLATE_MAIL, /* invoke to teleport all hostile monsters to you (multiple gather trap) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 4000L ),

A("Free Skill Slots",				PLATE_MAIL, /* invoke to gain d5 skill slots, but the armor is used up in the process */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 50000L ),

A("Moneysack",				PLATE_MAIL, /* invoke to gain d1000 zorkmids */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 5000L ),

A("Joke's Over",				PLATE_MAIL, /* displays all joke monsters while worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Boner Hider",				PLATE_MAIL, /* if you're male, and you'd lose alla, you get contaminated instead */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Hazardous Equipment",				PLATE_MAIL, /* contamination resistance, aggravate monster, disables stealth */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Changering Enviroment",				PLATE_MAIL, /* resist fire, cold, poison, shock, sleep, disintegration, petrification, drain life and magic, but also gives banishmentitis and if you can't be banished, you die! */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Fully Stupid Item",				WAN_SUMMON_UNDEAD, /* whenever you zap it, you gain 10*XL experience but also rnz(1000) turns of the satan trap effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Do Not Display The Charges",				LONG_SWORD, /* randart2 melee weapon, +5 INT, invoke to reset your Pw to the maximum but 1 in 5 chance that the weapon becomes brittle, and if the weapon is already brittle while invoking, you still get the Pw-restoring message but it doesn't actually restore */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 6), NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 6000L ),

A("Versus Electrically-Based Monsters",				LONG_SWORD, /* randart melee weapon, 10 extra points of AC, deals 3d10 extra damage to monsters that have AD_ELEC or AD_MALK */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Star-Slay Giants",				BASTERD_SWORD, /* +5 STR, DEX and INT */
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_GIANT,
	PHYS(20, 26), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Ooop Elixer",				POT_GAIN_ABILITY, /* doesn't have the gain ability effect when quaffed, but instead causes the permanent stat damage effect without you being able to resist and cures all feminism trap effects in the process */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Completely Nothing",				POT_RESTORE_ABILITY, /* doesn't have the restore ability effect when quaffed, but instead restores temporarily drained stats */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("ooooooooooar.",				SCR_TRAP_DETECTION, /* all traps in a 11x11 area centered on you become revealed, even if they were super-invisible */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Let Blind People See",				SCR_CURE_BLINDNESS, /* if you read it while blind, you gain rnz(10000) turns of blindness resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Nez Morsel",				WAKAME, /* eating it gives rn1(8,8) turns of invulnerability */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Incandescent Triangle",				WHITE_SWEETS, /* eating it causes rnz(2000) turns of sterilization */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Flushemout",				BROWN_SWEETS, /* eating it cures lycanthropy, contamination and sickness, but gives rnz(2000) turns of diarrhea */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Colorvision",				GREEN_SWEETS, /* eating it gives intrinsic poison resistance and see invisible, but also the marlena trap effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("That's An Arti!",				PINK_SWEETS, /* eating it creates a random artifact, and can unlock the skill associated with it */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Ripper",				BLACK_SWEETS, /* eating it stops all currently active corpse revival timers */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Rare Res Obtained",				YELLOW_SWEETS, /* eating it gives intrinsic acid resistance */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Feed The Horse",				SHEAF_OF_STRAW, /* if a pet eats it, provides 100000 additional turns of satiation to it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Flufflight",				COTTON, /* eating it gives rn1(50,50) turns of flying */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Newtraltoxin",				ONION, /* eating it restores your abilities and can increase one, like magic fountain, then causes a random bad effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Lansio'r Blifiau",				WELSH_ONION, /* eating it doubles your sling skill training, and increases sling skill cap by one level */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Blank Slate",				WHITE_PEACH, /* eating it causes the intrinsic loss effect 10000 times, meaning you almost always lose all your intrinsics (good and bad ones alike) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Nanika Ga Okotta",				SENTOU, /* eating it either causes a good or bad effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Gnurps",				BEAN, /* +1 CON when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Kokoro o kaiho suru",				SENZU, /* cure confusion and stun when eaten, and rnz(5000) turns of conf/stun res */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Grape Taste",				PARFAIT, /* +1 luck when eaten and 500 additional units of nutrition */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("White Christmas Dream",				X_MAS_CAKE, /* if you eat it during december, all floor/corridor tiles on the current level turn into snow and you get 5 random weapons as well as 10 random armor pieces; otherwise, it flat out refuses to be eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Very Good Meal",				BUNNY_CAKE, /* eating it increases all stats by one */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Oh Nooooooo",				BAKED_SWEET_POTATO, /* eating it induces vomiting and gives you the ina trap effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Melon Flavor",				BREAD, /* cures confusion when eaten, and 1000 extra units of nutrition */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Make Yourself Heavy",				PASTA, /* +2 CON and rnz(10000) turns of patricia trap effect when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Acrolein Addiction",				CHARRED_BREAD, /* eating it gives rnz(250) contamination and allows you to train a skill by 100 points */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Arti-Choke",				SLICE_OF_PIZZA, /* eating it gives rnz(1000) turns of magical breathing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Not In The Lab",				PIZZA, /* eating it while your quest nemesis isn't dead yet gives you the beacher hybrid race, but in any case you get intrinsic full nutrients */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Ugh-Ugghh",				BOTA_MOCHI, /* eating it gives +1 luck but drains your maximum health by 10 (kills you if you don't have enough) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Yakkaina Tori O Mitsuketa",				KIBI_DANGO, /* eating it gives rnz(200) turns of detect monsters */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Oishi",				SAKURA_MOCHI, /* no specialties */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Matia Seibun Juyo",				KOUHAKU_MANJYUU, /* eating it cures totter, and if it wasn't brittle, also unlocks Juyo (with a cap of expert) or increases its cap by one, but wishing for this artifact causes it to be created brittle */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Gekito ni yoru hentai shojo-zeme",				YOUKAN, /* eating it summons a squad of M5_ELONA somewhere, and a tame M5_ELONA next to you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Okukuba Obutereevu",				MAGIC_BANANA, /* eating it reduces current HP of all monsters on the current level by half */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Mind Cleared Off",				LUNCH_OF_BOILED_EGG, /* eating it sets your sanity to zero and causes amnesia of magnitude 27, plus gives rnz(10000) turns of psi resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Ultra Annoyance",				BARDICHE, /* randart polearm, artiblast and melee fuckup effect, +10 STR, monsters are less likely to evade the pounding attack, +2 sight range */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(6,14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Dimension Fishing",				FISHING_POLE, /* can be invoked to teleport a hostile monster to you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Houzanha",				BRONZE_PICK, /* can be invoked for field break */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Sally's Baking Mixture",				PEANUT_BAG, /* can be invoked to create a fortune cookie */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 1500L ),

A("Lorenzi's Cleaning Residue",				LOW_BOOTS, /* initialized to always be everlasting boots, +2 all stats, brown spells and anastasia trap effect, if you're a singslave you also get free action and discount action */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_SINGSLAVE, NON_PM, 2600L ),

A("Final Door Solution",				LONG_SWORD, /* randart; you automatically unlock doors and containers while wielding it, and if you're an assassin you also get technicality */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Stat Jackpot",				HONEYCOMB, /* gives an additional +1 to all stats when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Cha-otherstat",				LUMP_OF_ROYAL_JELLY, /* +5 CHA when eaten, but all the other stats have an individual 50% chance to be permanently reduced by one */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Smart at the cost of Self",				LUMP_OF_ROYAL_JELLY, /* if you eat it while your HP and Pw maximums are at least 100, they are reduced by 10% and you gain +3 INT */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Soldier's Nutricase",				LUMP_OF_ROYAL_JELLY, /* +1 CON when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Dancer's Vegetables",				LUMP_OF_ROYAL_JELLY, /* +1 DEX when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Philosopher's Breath",				LUMP_OF_ROYAL_JELLY, /* +1 INT when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Priestess's Manna",				LUMP_OF_ROYAL_JELLY, /* +1 WIS when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Rhetor's Crumbs",				LUMP_OF_ROYAL_JELLY, /* +1 CHA when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Piranhabane",				RUNED_ROD,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_EEL,
	PHYS(0, 50), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("The Ring of Everything Resistance",				RIN_ADORNMENT, /* randart, resist fire, cold, shock, poison, sleep and acid, giant explorer, autocurses when worn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Doorbane",				LOCK_PICK, /* doors cannot resist when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Magic Carpet",				TOWEL, /* flying when worn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Mantle of the Manta Ray",				OILSKIN_CLOAK, /* grants magical breathing and swimming */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Nularmor",				RING_MAIL, /* autocurses, makes your AC worse by 5 points and gives immunity to acid that also protects your inventory */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("BS-3008",				LOW_BOOTS, /* randart; if you're male you get free action, otherwise you get discount action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Formo-212",				LOW_BOOTS, /* randart2; if your footwear counts as sexy flats, you have stun resistance and super regene, if it counts as high heels you're slowed down, and if it's none of those two it grants +2 charisma and half physical damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Coal Peer",				CHAIN_MAIL, /* autocurses, makes your AC worse by 5 points, gives immunity to fire and double hunger */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Cutrelease",				KNIFE, /* randart knife, cold immunity, whenever a monster leaves a corpse you take damage equal to the monster's level, 1 in 1000 chance per turn that you get rnd(20 + (level_difficulty * 3)) turns of bleeding and your sanity and contamination are reduced by the same amount */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Strike One Run Ther",				SHOVEL, /* improved bashing damage: +5 and one more per sling skill level */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Aftermine",				SHOVEL, /* digs as fast as a pick-axe */
	(SPFX_RESTR), 0, 0,
	PHYS(10, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Xtra Cuteness",				NADINE_STILETTO_SANDALS, /* 5 extra points of AC, death resistance, +10 CHA, but 10% chance that you take double damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 8000L ),

A("Saucy Girl",				LUISA_HUGGING_BOOTS,
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Teach Ya But Cannot Learn",				IRINA_WEDGE_SANDALS, /* can't train skills, but 1 in 20000 chance per turn that you may pick a restricted skill and unlock it with a cap of basic */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Cruel Parents",				LISELOTTE_HUGGING_BOOTS, /* can be invoked to change your character's name to "Liselotte"; doing so conveys intrinsic cold and shock resistance as if you had eaten a corpse granting them but heavily curses the boots and permanently damages your charisma by d5, and this only works if you're female, otherwise you get hit with rn1(50,50) turns of confusion and stun and receive rnz(500) sanity */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Climate Protection Is Erring Sense",				GRETA_HUGGING_BOOTS, /* allows you to raze trees by kicking them; if your cop wanted level is below 1000, it's increased by 1000 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Jump Kick Action",				JANE_HUGGING_BOOTS, /* jumping, +5 kick damage and your kick cannot be clumsy */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Thicker Than The Head",				SUE_LYN_WINTER_BOOTS, /* +5 AC, can walk on snow, stun resistance but -20% success rate for spellcasting and your technique timeout counts down only 50% of the time */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Background Holding",				CHARLOTTE_HUGGING_BOOTS, /* invisibility, stealth, allows you to backstab monsters, can be invoked to get rid of intrinsic aggravate monster */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Gloatsane",				HANNAH_CUDDLE_BOOTS, /* psi resistance; if your sanity increases, there's an 80% chance that it doesn't, and even if it does, the amount of sanity you get is only 80% of the normal value */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Clone!",				LITTLE_MARIE_HUGGING_BOOTS, /* all M5_JOKE have 50% chance of spawning peaceful, resist confusion and stun, magical breathing, disables polymorph control and conveys polymorphitis; if the latter triggers, you automatically try to clone yourself */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("The Godly Postman",				LOW_BOOTS, /* initialized to always be winged sandals, 10% chance per turn that you're impervious to all damage, provides life saving and is used up if it does save you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Such A Lovely Shark",				LOW_BOOTS, /* initialized to always be remora heels, all ; spawn peaceful and are visible while they're peaceful, taking them off does not kill the remora */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Spflotch! Hahahahaha!",				LOW_BOOTS, /* initialized to always be high-heeled chelsea boots, henrietta trap effect, resist sleep, improves AC by 3 and MC by 1, can be invoked for corona antidote but doing so heavily curses the boots */
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3500L ),

A("Helicopter Twirl",				QUARTERSTAFF, /* randart quarterstaff, flying */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Counter-Terrorists Win",				DEFUSING_BOX, /* if you're lawful, mines and bombs are easier to disarm */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 200L ),

A("Mac's Box",				DEFUSING_BOX, /* improves success chance when untrapping */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Protect From Backlash",				GAUNTLETS_OF_DEFUSING, /* 3 extra points of AC and 50% chance to avoid bad effects */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Ksschl! Ksschl!",				BLINDING_VENOM, /* spawns with +400 stack size */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 40L ),

A("Wediforce",				BLINDING_VENOM, /* randart, +1 range for every level in your Wedi skill */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 30L ),

A("Dongor",				BLINDING_VENOM, /* randart2 */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 15), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Wiwiu!",				BLINDING_VENOM, /* +d3 multishot */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 40L ),

A("Real Acid",				ACID_VENOM,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ACID(4, 16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 80L ),

A("Strong!",				ACID_VENOM, /* spawns with rn1(7,7) extra points of enchantment */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 70L ),

A("Wendy's Drain",				FAERIE_FLOSS_RHING,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DRLI), 0, 0,
	DRLI(0, 2), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 100L ),

A("Klieau!",				TAIL_SPIKES,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 36), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Ridget Phasto",				BLINDING_VENOM, /* randart, melee fuckup effect, gives intrinsic nastytrap effects over time, spawns with stack size of 1, sticky prime curses itself when wielded, doesn't say anything in the description (you have to figure out yourself how to get rid of it) */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 80L ),

A("Hol'on Man",				BLINDING_VENOM, /* randart2, spawns with stack size of 1, wallwalking, detect monsters, difficulty effect, monsters respawn when killed, verisiert effect, julietta trap effect, autocurses when wielded */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Main Gauche",				PARRY_DAGGER,
	(SPFX_RESTR), 0, 0,
	PHYS(1, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Piratte Hunting",				PARRY_SWORD,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(2, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Double Beretta",				PISTOL_PAIR,
	(SPFX_RESTR), 0, 0,
	PHYS(6, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Double Colt 1911",				PISTOL_PAIR,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1400L ),

A("Double SAA",				PISTOL_PAIR,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2400L ),

A("Death To Heathen",				FIVE_SEVEN_BULLET,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(10, 25), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 400L ),

A("Lead Syringe",				LEAD_FIVE_SEVEN_BULLET, /* +2 multishot */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Fommp",				SHADOW_FIVE_SEVEN_BULLET, /* if it hits a monster, 1 in 20 chance to get a random good effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Their Ded",				ANTIMATTER_FIVE_SEVEN_BULLET, /* bisects targets */
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Krerratsch",				RAPID_DART,
	(SPFX_RESTR), 0, 0,
	PHYS(3, 7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 80L ),

A("Ding-Ding",				NINJA_STAR,
	(SPFX_RESTR), 0, 0,
	PHYS(2, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Actual Flame",				FLAMETHROWER, /* throwing while having at least 5 mana costs 5 mana and blasts a bolt of fire in the direction of your throw */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Burdensome Log",				HEAVY_SPEAR, /* +15 damage when thrown */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Corroser Lance",				SUPERHEAVY_SPEAR, /* +5 melee to-hit when riding but -5 otherwise */
	(SPFX_RESTR|SPFX_ATTK|SPFX_EVIL), 0, 0,
	ACID(0, 13), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Astral Lightwell",				LIGHTTORCH, /* astral vision when lit and wielded, makes undead deal double damage to you */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Minopower",				LASER_FLYAXE, /* bisects enemies, -3 INT/WIS and map amnesia */
	(SPFX_RESTR|SPFX_BEHEAD|SPFX_EVIL), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Jaden's Mission",				PINK_LIGHTSWORD,
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Fyarr's Fyre",				PINK_DOUBLE_LIGHTSWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_EVIL), 0, 0,
	FIRE(5,0),	FIRE(0,0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Movenin Hosasa",				SPE_COMBAT_COMMAND, /* very fast speed when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Spiked Knuckles",				RIN_IMPACT, /* +d5 bleeding damage if you fight unarmed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Iwa Erwi",				SPE_DEFUSING, /* reading increases or decreases the level of the defusing spell by one */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("SymPlant",				IMPLANT_OF_ABRASION, /* randart, your symbiote (if present) has 50% chance per turn to regenerate an extra HP but you also lose an extra point of nutrition every turn; if you're in a form without hands, it also conveys technicality */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Maxhit Boost",				IMPLANT_OF_ABRASION, /* randart, your symbiote (if present) has 1 in 1000 chance per turn to maybe get extra max HP if you're lucky, and double chance for that if you're in a form without hands */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Extrapugeln",				ROBE_OF_FOCUSSING, /* increases unarmed damage like robe of pugilism */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Trinnie's Snipersights",				SHIELD_PATE_GLASSES, /* infravision when worn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Nag me, fuck me!",				AMULET_VERSUS_NAGGING, /* increases nagging resistance to 95% */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("You Cannot See Me",				SPLINT_MAIL, /* if you're invisible, monsters still cannot see you even if they have see invisible */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Total Gauche",				DAGGER, /* randart dagger, if it's in your secondary weapon slot while dual-wielding it gives 10 extra AC and one more per enchantment value or dual-wielding skill level */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Vorpal Heels",				LOW_BOOTS, /* initialized to always be fetish heels; if you kick something with it while they're still counting as stiletto heels, 1 in 20 chance to behead the target */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("The Master Sword",				SILVER_LONG_SWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(3, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Erdrick's Armor",				PLATE_MAIL, /* 5 extra points of AC, free action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Tendo Mallet",				MALLET,
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(24, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Grandleon",				BROADSWORD, /* polymorph control */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Kusanagi Blade",				KATANA,
	(SPFX_RESTR|SPFX_SEARCH|SPFX_SEEK), 0, 0,
	PHYS(3, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Alucart Mail",				PLATE_MAIL, /* makes your AC worse by 5 points, resist death and drain life */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Klobb",				SUBMACHINE_GUN, /* -6 increase damage and accuracy, bashing a monster with it has 10% chance to paralyze it for d10 turns */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Excalipoor",				RUNESWORD, /* -9 increase damage and accuracy */
	(SPFX_RESTR|SPFX_SEEK|SPFX_DEFN|SPFX_SEARCH|SPFX_EVIL), 0, 0,
	NO_ATTK, DRLI(0,0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Soul Edge",				BROADSWORD, /* clairvoyance, killing a monster heals you by one point */
	(SPFX_RESTR), 0, 0,
	PHYS(3, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Covidivac",				INFUSION, /* corona antidote when used */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Entropy Shootout",				SCR_NAME, /* creates boss spawners on squares adjacent to you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Grassswandir",		ELVEN_SHORT_SWORD, /* transforms floor into grass */
	(SPFX_RESTR|SPFX_HALRES|SPFX_EVIL), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Rayswandir",		QUARTERSTAFF, /* randart2 quarterstaff, can be invoked to fire a magic missile */
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	SPECIAL_INVOKE, A_LAWFUL, NON_PM, NON_PM, 8000L ),

A("Laidswandir",		SEXY_LEATHER_PUMP, /* prevents you from resisting foocubus advances */
	(SPFX_RESTR|SPFX_HALRES|SPFX_EVIL), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Painbowswandir",				BOW, /* randart2 bow, you take double damage */
	(SPFX_RESTR|SPFX_HALRES|SPFX_EVIL), 0, 0,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Moldswandir",		ASTERISK, /* deals d10 extra damage to F-class monsters */
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Fullswandir",				CHAINSWORD, /* doubles monster spawn rate */
	(SPFX_RESTR|SPFX_HALRES|SPFX_EVIL), 0, 0,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 10000L ),

A("Repeating Load",				LOADSTONE, /* by jonadab, carrying it has 1 in 2500 chance per turn to give you another cursed loadstone */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(10, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Ultracursed Vampiric High-Heeled Shoes",				SENTIENT_HIGH_HEELED_SHOES, /* sticky prime curse themselves when worn, resist death and drain */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Iron of Innermost Joy",				IMPLANT_OF_ABRASION, /* randart, material sets itself to IRON when equipped, heavily curses itself, wallwalking, you walk in random directions for 10 turns and then normally for 3 turns; if in a form without hands, astral vision and half physical/spell damage. also you can deflect projectiles */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100000L ),

A("Lower Turncount",				ENERGY_SAP, /* by bugsniper, reduces turncount by rn1(500, 500) when used */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Hahareduction",				REALLY_BAD_HELM, /* reduces the time it takes for nastytraps triggered by you to time out by half */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("See The Mulch State",				RING_MAIL, /* tells you when your ammos mulch, +3 DEX, infravision */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Erotic Stat Train",				LOW_BOOTS, /* randart2, +5 soft cap for stat training, marleen trap effect, can be invoked to cure all status effects */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Paeau paeau paeau paeau paeau!",				BANDED_MAIL, /* can be invoked to fire magic missiles in all directions */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 2000L ),

A("Go Up In Flameses",				BANDED_MAIL, /* can be invoked to detonate 10 grenades on your location; doing so uses up the armor and increases your intrinsic protection by 2 points */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 4000L ),

A("Fire Cold Aeff",				BANDED_MAIL, /* resist fire and cold, but deactivates lightning resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Upper Rum",				BANDED_MAIL, /* 10 extra points of AC, but any monsters with size of "small" or less have +20 to-hit */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("What Miss I Constantly",				BANDED_MAIL, /* fear, very fast speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Brings Nothing",				CHAIN_MAIL, /* 5 extra points of AC and poison resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Brings Which",				CHAIN_MAIL, /* 5 extra points of AC and can be invoked to recover 8d8 Pw */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Dueueueuet",				STUDDED_LEATHER_ARMOR, /* autocurses, makes AC worse by 5 points and provides double fire resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Made Of Iron",				STUDDED_LEATHER_ARMOR, /* 10 extra points of AC but reduces spell success rates by 20% */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Beginner Suit",				LEATHER_ARMOR, /* 5 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Ah Nah",				LEATHER_ARMOR, /* heavily curses itself */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Rare Early Bauble",				LEATHER_ARMOR,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Incaputable",				SCALE_MAIL, /* can be invoked to repair the armor */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 6000L ),

A("Gilded Mail",				SCALE_MAIL, /* all M2_LORD or M2_PRINCE have 75% chance of spawning peaceful */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Demanding Entry",				SCALE_MAIL, /* 5 extra points of AC and drain resistance, shopkeepers always let you into their shop even if you're e.g. invisible */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Acidfall",				LONG_SWORD, /* randart long sword */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ACID(5, 0), DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Avalon",				SCIMITAR, /* randart scimitar */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	PHYS(1, 1), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4500L ),

A("Balmung",				BROADSWORD,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(4, 9), DFNS(AD_FIRE), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Bradamante's Fury",				LANCE,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 10), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 800L ),

A("Worse Carnwennan",				KNIFE, /* randart knife */
	(SPFX_RESTR|SPFX_SEARCH|SPFX_STLTH), 0, 0,
	PHYS(3, 8), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 400L ),

A("Chains of Malcanthet",				IRON_CHAIN, /* randart chain */
	(SPFX_RESTR|SPFX_WARN), 0, 0,
	PHYS(6, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Circe's Witchstaff",				QUARTERSTAFF,
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3500L ),

A("The End (c Grim Reaper)",				SCYTHE, /* turn counter advances twice as fast */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_EVIL), 0, 0,
	COLD(3, 20), DRLI(0, 0), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 6000L ),

A("Gae Bulg",				JAVELIN,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DRLI), 0, 0,
	DRLI(5, 5), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 9000L ),

A("Gae Dearg",				ELVEN_SPEAR,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DRLI), 0, 0,
	DRLI(3, 7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Gae Buidhe",				ELVEN_SPEAR, /* causes monster to bleed for d10 (more) turns */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	DRLI(3, 7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Gleipnir",				GRAPPLING_HOOK,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1700L ),

A("Gungnir",				DWARVISH_SPEAR,
	(SPFX_RESTR), 0, 0,
	PHYS(100, 4), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Hand Grenade of Antioch",				FRAG_GRENADE,
	(SPFX_RESTR), 0, 0,
	PHYS(1, 1), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 300L ),

A("Idspike",				ATHAME, /* resist psi */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Imhullu",				GLAIVE, /* 90% chance to resist "you are pushed back!" */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 5), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Luckless Folly",				SHORT_SWORD, /* -3 luck, increases to-hit and damage by the opposite of your luck */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5, 5), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Origin of Casting",				QUARTERSTAFF, /* 20% better spellcasting chances */
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	PHYS(2, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Pridwen",				LARGE_SHIELD,
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1500L ),

A("Poseidon's Other Trident",				TRIDENT, /* magical breathing, water walking */
	(SPFX_RESTR), 0, 0,
	PHYS(3, 7), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Seafoam",				TRIDENT, /* randart trident, magical breathing, water walking */
	(SPFX_RESTR), 0, 0,
	PHYS(3, 7), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Sonicboom",				MORNING_STAR, /* deafness */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Sunspot",				MACE, /* blindness resistance */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Vladsbane",				FOOD_RATION, /* undead warning, -5 increase accuracy and damage */
	(SPFX_RESTR|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("War's Sword",				TWO_HANDED_SWORD, /* conflict, autocurses, doesn't make your hands unusable */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Sword of Fair Play",           BROADSWORD,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(5,12),     NO_DFNS,        NO_CARY,        0, A_LAWFUL, NON_PM, NON_PM, 1500L ),

A("The Power Pole",				BARDICHE, /* randart2 polearm, deafness */
	(SPFX_RESTR|SPFX_LUCK|SPFX_PROTEC|SPFX_EVIL|SPFX_INTEL|SPFX_EVIL), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Unlimited Moon",				FLAIL, /* randart flail */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(2, 2), COLD(0, 0), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),

A("Dismounter",				LANCE, /* randart lance */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_QUADRUPED,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 200L ),

A("Final Death",				BULLWHIP, /* randart whip */
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_DEFN), 0, M2_UNDEAD,
	PHYS(5, 0), DRLI(0, 0), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 200L ),

A("Balance Pseudoshield",				QUARTERSTAFF, /* randart2 quarterstaff */
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	PHYS(2, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Seven-League Boots",				SPEED_BOOTS, /* jumping */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("The King in Yellow",				SPE_FINGER_OF_DEATH, /* summons a demon prince if read */
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 30L ),

A("Murasame",				KATANA,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Dagger of Uukai",				ELVEN_DAGGER, /* poison resistance */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 4), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Winblows",				FLAIL,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_GREMLIN,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Maneater",				SHORT_SWORD, /* randart short sword */
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_HUMAN,
	PHYS(5, 10), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("The Sword of Madurai",				KATANA,
	(SPFX_RESTR|SPFX_INTEL|SPFX_EREGEN|SPFX_DEFN), 0, 0,
	PHYS(0, 8), DFNS(AD_MAGM), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 8000L ),

A("Gnollbane",				WAR_HAMMER, /* randart hammer */
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_DEFN), 0, M2_GNOME,
	PHYS(1, 20), DFNS(AD_ELEC), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 500L ),

A("Crossbow of the Gnoll Lords",				CROSSBOW, /* randart2 crossbow */
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	PHYS(1, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Howling Flail",				FLAIL, /* randart2 flail */
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	PHYS(1, 10), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6000L ),

A("Mace of Orcus",				MACE, /* randart2 mace */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	DRLI(1, 10), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6000L ),

A("The Ark of the Covenant",				PICK_AXE, /* randart2 pick-axe, antimagic shell when wielded, invoke tries to bless this artifact */
	(SPFX_RESTR|SPFX_INTEL|SPFX_LUCK|SPFX_PROTEC|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, SPECIAL_INVOKE, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("Famous Lance",				LANCE, /* randart2 lance, curse and death resistance, to-hit h@ck */
	(SPFX_RESTR), 0, 0,
	PHYS(1, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2500L ),

A("The Emerald Sword",				TWO_HANDED_SWORD, /* randart two-handed sword, monsters always know where you are and always spawn hostile, +5 STR and WIS, to-hit h@ck */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_HALRES|SPFX_EVIL), 0, 0,
	PHYS(1, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("The Katana of Masamune",				OSBANE_KATANA, /* bisection and bisection resistance */
	(SPFX_RESTR|SPFX_BEHEAD|SPFX_LUCK), 0, 0,
	PHYS(1, 4), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 6000L ),

A("The Kusanagi",				TSURUGI, /* sickness resistance */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 5), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 3000L ),

A("Serpent Ring of Set",				RIN_AGGRAVATE_MONSTER, /* randart2, +5 INT and WIS */
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Gauntlets of Yin and Yang",				GAUNTLETS_OF_DEXTERITY, /* randart2 */
	(SPFX_RESTR), 0, 0,
	PHYS(1, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("The Holy Grail",				QUARTERSTAFF, /* randart2 quarterstaff, slow digestion */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 3000L ),

A("Hammer of Justice",				MORNING_STAR,
	(SPFX_RESTR|SPFX_LUCK|SPFX_PROTEC|SPFX_INTEL|SPFX_DALIGN), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 200L ),

A("Snow Illusion",				SHORT_SWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(5,0),	COLD(0,0), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 3000L ),

A("Swordbreaker",				DAGGER, /* disenchants enemy weapon up to -10 */
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	PHYS(2,4), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 500L ),

A("Winkchainsword",				CHAINSWORD,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_FEMALE,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("Misumaru",				KATANA,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	PHYS(20, 1), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1200L ),

A("Barukkheled",				BATTLE_AXE,
	(SPFX_RESTR), 0, 0,
	PHYS(3,6), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1500L ),

A("Celestial Sceptre",				MACE, /* randart mace, 50% better spellcasting chance for healing spells */
	(SPFX_RESTR), 0, 0,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Dwarven Thrower",				HEAVY_HAMMER,
	(SPFX_RESTR|SPFX_DFLAG1), 0, M1_FLY,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 3000L ),

A("Remmah Dluks",				SLEDGE_HAMMER, /* disintegration resistance */
	(SPFX_RESTR|SPFX_BEHEAD|SPFX_DCLAS), 0, S_XAN,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2500L ),

A("Escaliborg",				SPIKED_CLUB,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_MALE,
	PHYS(3,20), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("Laiceps Niksud",				SPECIAL_MOP, /* blindness resistance */
	(SPFX_RESTR), 0, 0,
	PHYS(5,5), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 300L ),

A("Stringfellow Houki",				BROOM, /* jumping */
	(SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK), 0, 0,
	PHYS(5, 1), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 500L ),

A("Libra of Justice",				FLAIL, /* randart2 flail */
	(SPFX_RESTR|SPFX_INTEL|SPFX_DALIGN), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Holybasher",				FLAIL, /* randart flail, invoke to uncurse it */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Raising Heart",				QUARTERSTAFF, /* randart quarterstaff, you take double damage */
	(SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK|SPFX_PROTEC|SPFX_HSPDAM|SPFX_HPHDAM|SPFX_WARN|SPFX_EVIL|SPFX_ATTK), 0, 0,
	STUN(5, 1), DFNS(AD_MAGM), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Tenrai",				SPEAR, /* randart2 spear */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ELEC(5,0),	ELEC(0,0), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 3000L ),

A("Amanonuboko",				HALBERD,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_BLOB,
	PHYS(5,2), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Bardiche Assalt",				BARDICHE, /* replaces messages with random ones, renames items to soviet, autocurses, doesn't make hands unusable */
	(SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK|SPFX_EVIL), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Windmillbaster",				LANCE,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_GOLEM,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 105L ),

A("Quixotic Spirit",				LANCE, /* randart2 lance */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_GOLEM,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 105L ),

A("Lance Armstrong",				LANCE, /* randart lance */
	(SPFX_RESTR), 0, 0,
	PHYS(0,5), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Break Out",				DWARVISH_MATTOCK, /* +5 digging effort */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DFLAG1), 0, M1_THICK_HIDE,
	PHYS(5,6), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1500L ),

A("Lovemechain",				CHAINWHIP, /* undead warning */
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_WARN|SPFX_DEFN), 0, M2_UNDEAD,
	PHYS(5,0),	DRLI(0,0), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1500L ),

A("Ebony Lacquered Bow",				YUMI,
	(SPFX_RESTR|SPFX_SEARCH), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("Aquarhapsody",				BOW, /* randart2 bow */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(5,0),	COLD(0,0), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 3000L ),

A("Goliathbane",				SLING, /* randart sling */
	(SPFX_RESTR), 0, 0,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 300L ),

A("Firebird",				BOOMERANG, /* randart boomerang, 1 in 10 to be disenchanted up to -20 when hitting */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(5,24),	FIRE(0,0), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1500L ),

A("Shugo",				SUGUHANOKEN, /* bonus against S_UNICORN too */
	(SPFX_RESTR|SPFX_PROTEC|SPFX_DCLAS), 0, (S_SNAKE),
	PHYS(5,3), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Steambringer",				BROADSWORD, /* randart broadsword */
	(SPFX_RESTR|SPFX_DEFN|SPFX_DFLAG1), 0, M1_AMORPHOUS,
	PHYS(5,30), FIRE(0, 0), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Fire Soul",				SHORT_SWORD, /* randart2 short sword */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(5,0),	FIRE(0,0), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("Sword of Revolution",				RAPIER,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),

A("Daefaroth",				DAGGER, /* randart2 dagger, protects from u_slow_down */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	PHYS(3,6), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 500L ),

A("Noclaf",				SCIMITAR, /* randart2 scimitar */
	(SPFX_RESTR), 0, 0,
	PHYS(5,2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Jinrai",				AUTOMATIC_KATANA,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ELEC(5,0),	ELEC(0,0), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("Dual Mastery",				KHOPESH, /* +5 increase accuracy if it's in the off-hand slot */
	(SPFX_RESTR), 0, 0,
	PHYS(5,3), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 800L ),

A("Eutats Enots",				AXE, /* randart2 axe, petrification resistance */
	(SPFX_RESTR), 0, 0,
	PHYS(3,6), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),

A("Labryth",				BATTLE_AXE,
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2500L ),

A("Pendulum of Balance",				MACE, /* randart mace, stun resistance */
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_UNDEAD,
	PHYS(5,24), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("The Sharpened Oar",				BOAT_OAR, /* 5 extra points of AC */
	(SPFX_RESTR), 0, 0,
	PHYS(5,5), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 700L ),

A("Brainstorm",				FLAIL, /* randart2 flail */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	STUN(5,0), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2500L ),

A("Fowler",				TWO_HANDED_FLAIL,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_RODENT,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 700L ),

A("Polar Star",				MORNING_STAR, /* randart morning star, waterwalking, darkens your surroundings every turn */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_EVIL), 0, 0,
	COLD(5,16),	COLD(0,0), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("Polaris",				MORNING_STAR, /* randart2 morning star, waterwalking, darkens your surroundings every turn */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_EVIL), 0, 0,
	COLD(5,16),	COLD(0,0), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("Runebreaker",				RUNED_ROD,
	(SPFX_RESTR|SPFX_DCLAS|SPFX_DEFN), 0, S_LICH,
	PHYS(5,0), DFNS(AD_MAGM), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 700L ),

A("Parryingstaff",				QUARTERSTAFF, /* randart quarterstaff, 7 extra points of AC */
	(SPFX_RESTR), 0, 0,
	PHYS(8,3), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 700L ),

A("Sakushnir",				DWARVISH_SPEAR, /* enforces samurai conduct */
	(SPFX_RESTR|SPFX_ATTK|SPFX_EVIL), 0, 0,
	ELEC(5,15), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Revolter",				BAMBOO_SPEAR, /* conflict */
	(SPFX_RESTR|SPFX_EVIL|SPFX_DFLAG2), 0, (M2_LORD|M2_PRINCE),
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),

A("Asuibihc fo Naidraug",				UNICORN_HORN,
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	PHYS(5,3), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Stonebiter",				PICK_AXE, /* randart pick-axe, +5 digging effort */
	(SPFX_RESTR), 0, 0,
	PHYS(5,8), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),

A("Cliffhanger",				BULLWHIP,
	(SPFX_RESTR), 0, 0,
	PHYS(8,1), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Snakefire",				CHAINWHIP,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(5,0),	FIRE(0,0), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Flamesniper",				BOW, /* randart2 bow */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(5,0),	FIRE(0,0), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("Southern Cross",				CROSSBOW, /* randart crossbow */
	(SPFX_RESTR|SPFX_INTEL|SPFX_DEFN), 0, 0,
	PHYS(5,7),	DFNS(AD_MAGM), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Thunderbird",				BOOMERANG, /* randart2 boomerang */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ELEC(10,0),	ELEC(0,0), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),

A("Flower Hurricane",				WHITE_FLOWER_SWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Black Sarena",				BLACK_AESTIVALIS,
	(SPFX_RESTR), 0, 0,
	PHYS(5,3), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Snatcher",				SHORT_SWORD, /* randart short sword, protects from stealing like being a nymph */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_NYMPH,
	PHYS(9,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Stellaris Materia",				SHORT_SWORD, /* randart2 short sword, life saving when wielded */
	(SPFX_RESTR|SPFX_REGEN|SPFX_DCLAS|SPFX_DEFN), 0, S_WRAITH,
	PHYS(5,0), DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Calcar",				STILETTO, /* sleep resistance */
	(SPFX_RESTR), 0, 0,
	PHYS(4,1), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 500L ),

A("The Fang of Midgardsormr",				WORM_TOOTH,
	(SPFX_RESTR|SPFX_DFLAG1|SPFX_DEFN), 0, M1_ACID,
	PHYS(5,0),	ACID(0,0), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1200L ),

A("Dragonkiller",				BASTERD_SWORD,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_DRAGON,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4500L ),

A("Redrum",				CHAINSWORD,
	(SPFX_RESTR|SPFX_DFLAG1), 0, M1_HUMANOID,
	PHYS(5,14), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Atlach Nacha",				SCIMITAR, /* randart2 scimitar, free action */
	(SPFX_RESTR|SPFX_DFLAG2), 0, (M2_HUMAN|M2_ELF|M2_DWARF|M2_GNOME|M2_HOBBIT),
	PHYS(8,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Flame Eater",				BATTLE_AXE, /* destruction effect */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_EVIL), 0, 0,
	FIRE(5,16),	FIRE(0,0), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Fire Eater",				INKUTLASS, /* weakens sight like hc alien */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_EVIL), 0, 0,
	FIRE(5,16),	FIRE(0,0), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Onikirimaru",				KATANA,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_OGRE,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1200L ),

A("Spineshooter",				SPIKED_CLUB, /* poison resistance, +5 increase accuracy for ranged attack */
	(SPFX_RESTR), 0, 0,
	PHYS(5,4), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Confetto",				HEAVY_HAMMER,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_MALE,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Crusty Hammer",				SLEDGE_HAMMER,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_GOLEM,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Wincows",				HEAVY_HAMMER,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_QUADRUPED,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Battering Ram",				LOG,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_BAD_FOOD,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Seven in One Blow",				FLY_SWATTER, /* conflict */
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_EVIL), 0, M2_GIANT,
	PHYS(7,3), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 300L ),

A("Rekaerb Arbil",				TWO_HANDED_FLAIL,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2500L ),

A("Lux Regina",				FLAIL, /* randart flail, sight bonus */
	(SPFX_RESTR|SPFX_DFLAG2), 0, (M2_DEMON|M2_MINION),
	PHYS(4,4), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Regina Lux",				MORNING_STAR, /* randart morning star, sight bonus */
	(SPFX_RESTR|SPFX_DFLAG2), 0, (M2_DEMON|M2_MINION),
	PHYS(4,4), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Nyoibou",				TREESTAFF,
	(SPFX_RESTR), 0, 0,
	PHYS(5,3), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Staff of Moon",				FIRE_HOOK, /* bonus against S_VORTEX too */
	(SPFX_RESTR|SPFX_DCLAS|SPFX_DEFN), 0, (S_ELEMENTAL),
	STUN(5,24),	DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Pole of Moon",				BEC_DE_CORBIN, /* bonus against S_VORTEX too */
	(SPFX_RESTR|SPFX_DCLAS|SPFX_DEFN), 0, (S_ELEMENTAL),
	STUN(5,24),	DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Staff of Star",				STAR_ROD, /* bonus against S_VORTEX too */
	(SPFX_RESTR|SPFX_DCLAS|SPFX_DEFN), 0, (S_ELEMENTAL),
	STUN(5,0),	DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Kemononoyari",				SPEAR, /* randart2 spear, resist fear */
	(SPFX_RESTR|SPFX_REGEN|SPFX_DCLAS), 0, S_DOG,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Sun Piercer",				FLINT_SPEAR, /* blindness resistance */
	(SPFX_RESTR), 0, 0,
	PHYS(7,7), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 700L ),

A("The Harpoon of Triton",				TRIDENT, /* randart trident */
	(SPFX_RESTR|SPFX_DCLAS|SPFX_DEFN), 0, S_EEL,
	PHYS(5,15),	COLD(0,0), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Merciless Mare",				UNICORN_HORN, /* randart unihorn, sleep resist */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	PHYS(5,3), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Enrai",				GLAIVE, /* randart polearm */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ELEC(5,0),	ELEC(0,0), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Niur Krad",				GLAIVE, /* randart2 polearm */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DRLI), 0, 0,
	DRLI(5, 0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Principium Incerti",				GLAIVE, /* randart polearm */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DRLI), 0, 0,
	DRLI(5, 0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Tira-mi-su",				CHAINWHIP,
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_WARN), 0, M2_WERE,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Durin's Bane",				FLAME_WHIP, /* sleep resist */
	(SPFX_RESTR|SPFX_DFLAG2), 0, (M2_ELF|M2_DWARF),
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("The Bow of Hercules",				BOW, /* randart bow, auto-poison code like plague */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5,7),	DFNS(AD_DRST), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6000L ),

A("Kikyounoreikyuu",				YUMI,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5,0),	DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("The Seventh Scripture",				PILE_BUNKER, /* auto-explode code like hellfire */
	(SPFX_RESTR|SPFX_INTEL|SPFX_DEFN), 0, 0,
	PHYS(5,7),	FIRE(0,0), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Edelweiss",				WHITE_FLOWER_SWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(5,3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Ajihei",				GREAT_HOUCHOU, /* can be invoked to make it erosionproof */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_EEL,
	PHYS(5,15), NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 5000L ),

A("The First Carrying Box",				GREAT_HOUCHOU, /* +1000 weight cap and max weight cap */
	(SPFX_RESTR), 0, 0,
	PHYS(5,3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Magemasher",				DWARVISH_SHORT_SWORD, /* protects from mind flayer draining attacks */
	(SPFX_RESTR|SPFX_DCLAS|SPFX_HSPDAM), 0, S_LICH,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Gradius",				SHORT_SWORD, /* randart short sword */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_LIZARD,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Shizugami's Mizuchi",				SILVER_DAGGER, /* poison resistance, permapoisoned like dirge */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	PHYS(2, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Azoth",				MERCURIAL_ATHAME,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5,0),	DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Progressiveknife",				SURVIVAL_KNIFE,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_MINION,
	PHYS(2,6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Occam's razor",				SCALPEL,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Lunaticbringer",				BASTERD_SWORD, /* wereform effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4500L ),

A("Benizakura",				OSBANE_KATANA, /* disintegration resistance */
	(SPFX_RESTR), 0, 0,
	PHYS(5,3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Kouka",				HONOR_KATANA,
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	PHYS(5,3),	FIRE(0,0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Momiji",				KATANA,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5,3),	ELEC(0,0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Kousetsu",				AUTOMATIC_KATANA,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5,3),	COLD(0,0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Hatchet of Hinamizawa",				SHARP_AXE,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_HUMAN,
	PHYS(5,14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Gourai",				SPIKED_CLUB,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ELEC(5,0),	ELEC(0,0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Maxwell's Hammer",				AIR_PRESSURE_HAMMER,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_HUMAN,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Cooking of Devoting Wife",				OTAMA,
	(SPFX_RESTR|SPFX_BEHEAD|SPFX_DCLAS), 0, S_YETI,
	PHYS(3,1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Bsikeslayer",				FUTON_SWATTER,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_ANT,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Gossip Harisen",				CARDBOARD_FAN, /* protects from mind flayer attack */
	(SPFX_RESTR|SPFX_SPEAK), 0, 0,
	PHYS(0,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Tukkomi Harisen",				CARDBOARD_FAN, /* prevents AD_SAMU from stealing your shit */
	(SPFX_RESTR), 0, 0,
	PHYS(20,1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Ojousamahamajo",				BROOM, /* flying */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_FUNGUS,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Miracle Paint",				MAGICAL_PAINTBRUSH, /* see invisible */
	(SPFX_RESTR|SPFX_SEARCH|SPFX_HALRES|SPFX_LUCK|SPFX_ATTK|SPFX_DEFN), 0, 0,
	STUN(16, 4), ELEC(0, 0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Flare Flake",				FLOGGER, /* disables cold resistance */
	(SPFX_RESTR|SPFX_ATTK|SPFX_EVIL), 0, 0,
	FIRE(5,20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Jack-o' Lantern",				POLE_LANTERN,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(5,6),	COLD(0,0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Twinklestar",				STAR_ROD,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5,4),	DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Starring Inferno",				STAR_ROD, /* blue spells and starlit sky effect */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EREGEN|SPFX_HSPDAM|SPFX_DEFN|SPFX_EVIL), 0, 0,
	PHYS(3,0), DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Yokoyari",				ORCISH_SPEAR, /* prevents AD_SAMU from stealing your shit */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_NYMPH,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Arm of Olympia",				SPEAR, /* randart2 spear, +10 strength */
	(SPFX_RESTR|SPFX_DFLAG2), 0, S_GOLEM,
	PHYS(5,3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Hide Penetrator",				STACK_JAVELIN,
	(SPFX_RESTR|SPFX_SEARCH|SPFX_DEFN), 0, 0,
	PHYS(12,0), DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Flame Pillar",				SPEAR, /* halves your AC (stacks with dimness) */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_EVIL), 0, 0,
	FIRE(5,20),	FIRE(0,0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Ice Pillar",				SPEAR, /* halves your AC (stacks with dimness) */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_EVIL), 0, 0,
	COLD(5,20),	COLD(0,0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Skazka ob Durake",				PITCHFORK, /* 7 extra points of AC, sickness resistance, -3 INT when wielded */
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_EVIL), 0, M2_DEMON,
	PHYS(0,30), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Ten-Dimensional Scythe",				SCYTHE, /* prevents you from being digested, prism reflection */
	(SPFX_RESTR|SPFX_DCLAS|SPFX_DEFN|SPFX_REFLECT), 0, S_LICH,
	PHYS(5,0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Alietta's Parasol",				PARASOL, /* 7 extra points of AC */
	(SPFX_RESTR), 0, 0,
	PHYS(5,3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Buttobasu Boomerang",				BOOMERANG,
	(SPFX_RESTR), 0, 0,
	PHYS(5,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Silpheed",				WINGS_OF_ANGEL, /* protects from theft like being a nymph */
	(SPFX_RESTR|SPFX_SEARCH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2000L ),

A("A.M. Suit",				OLIHARCON_SPLINT_MAIL,
	(SPFX_RESTR|SPFX_SEEK|SPFX_SEARCH|SPFX_HPHDAM|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Ladies Nightingale",				NURSE_UNIFORM, /* poison resistance */
	(SPFX_RESTR|SPFX_HALRES|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Gentlemans Nightingale",				ALCHEMY_SMOCK, /* poison resistance */
	(SPFX_RESTR|SPFX_HALRES|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Neutrino",				LAB_COAT, /* +1000 carry cap and max carry cap, can slip out of wrap attacks like oilskin cloak */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Brass Snakeskin",				ROBE_OF_PROTECTION,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 7500L ),

A("Scale Mail of the Dragon Lord",		PLAIN_DRAGON_SCALE_MAIL,
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Mantle of Beast",				LEATHER_CLOAK, /* +2 melee damage */
	(SPFX_RESTR|SPFX_PROTEC|SPFX_DEFN), 0, 0,
	NO_ATTK, COLD(0, 0), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Powered Suit",				PLATE_MAIL, /* drains maximum Pw with 1 in 100 chance per turn, or alla if you have no Pw left */
	(SPFX_RESTR|SPFX_SEEK|SPFX_WARN|SPFX_SEARCH|SPFX_HPHDAM|SPFX_XRAY|SPFX_PROTEC|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Poppy's Striped Tights",				BUNNY_UNIFORM, /* can be invoked for dynamite explosion */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 5000L ),

A("Phoenix Battlesuit",				FEATHER_ARMOR,
	(SPFX_RESTR|SPFX_HSPDAM|SPFX_REGEN|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("The Hat of Lady",				CORNUTHAUM, /* x-ray vision deactivated if you're male */
	(SPFX_RESTR|SPFX_XRAY|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("The Overcoat of Daughter",				CLOAK_OF_MAGIC_RESISTANCE, /* dimness if you're male */
	(SPFX_RESTR|SPFX_STLTH|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Mink Coat",				FUR,
	(SPFX_RESTR|SPFX_PROTEC|SPFX_LUCK|SPFX_DEFN), 0, 0,
	NO_ATTK, COLD(0,0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Fist of Fury",				SILVER_GAUNTLETS,
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(5,10), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Kazaana",				REGULAR_GLOVES, /* randart2 gloves, can be invoked for amnesia of magnitude 3 */
	(SPFX_RESTR|SPFX_HSPDAM|SPFX_INTEL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 2000L ),

A("Satellite Linker",				ANTENNA, /* 5 extra points of AC, can be invoked for rn1(40,40) turns of detect monsters */
	(SPFX_RESTR|SPFX_HSPDAM|SPFX_ESP|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, SPECIAL_INVOKE, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("Fairy Ear",				STETHOSCOPE, /* blindness resistance when wielded */
	(SPFX_RESTR|SPFX_SEARCH|SPFX_HALRES|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("The Beckoning Cat",				FIGURINE,
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Shadow Reflector",				MIRROR,
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Summonor",				MAGIC_WHISTLE, /* 10x less likely to become cursed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("The Lapis of Star",				LAPIS_LAZULI,
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Praefame",				RIN_HUNGER, /* autocurses, can be invoked to lose 500 nutrition */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 100L ),

A("Philosopher's Stone",				ELIF_S_JEWEL, /* randart feminism jewel */
	(SPFX_RESTR|SPFX_EVIL), (SPFX_DEFN), 0,
	NO_ATTK, NO_DFNS, CARY(AD_MAGM), 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Secespita",				VICTIM_KNIFE,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Bag of the Hesperides",				BAG_OF_HOLDING, /* like wallet of perseus but also waterproof */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Angelslayer",				TRIDENT,
	(SPFX_RESTR|SPFX_WARN|SPFX_ATTK|SPFX_SEARCH|SPFX_HSPDAM|SPFX_DCLAS), 0, S_ANGEL,
	FIRE(5, 10), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Yeen's Butcher",				TWO_HANDED_FLAIL,
	(SPFX_RESTR|SPFX_INTEL|SPFX_ATTK), 0, 0,
	STUN(5, 8), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Xiuhcoatl",				ATLATL, /* autocurses, levitation when wielded */
	(SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_ESP|SPFX_DEFN|SPFX_EVIL), 0, 0,
	FIRE(5, 24), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("The Crossbow of Carl",				CROSSBOW,
	(SPFX_RESTR|SPFX_INTEL|SPFX_REFLECT|SPFX_ESP), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Gjallar",				TOOLED_HORN,
	(SPFX_RESTR|SPFX_INTEL|SPFX_LUCK|SPFX_WARN|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Tomorrow Wendy's Choicena",				LEATHER_CLOAK, /* initialized to always be deadly cloak, wendy trap effect, if you're female it also has premature death and perilous life saving */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Nock Gun",				FLINTLOCK, /* if it has no artifact timeout, +6 multishot but firing it in that state gives a timeout */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Cannondancer",				PISTOL, /* +d(firearms skill level) multishot but if any bullet misses, you're paralyzed for rn1(5,5) turns */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Emergency Radiation Remover",				RADAWAY, /* cures 10x as much contamination */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Residual Blood Effect",				RAD_X, /* permanently gives 20% chance to not be contaminated if you otherwise would, but if you vomit, that effect ends */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Dolcevain",				SPE_RESIST_CONTAMINATION, /* reading it gives rnz(1000) turns of thorns */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("You Can Hurt",				SPE_RESIST_CONTAMINATION, /* reading it gives 500% additional memory to the spell, but damages you by half of your maximum HP (can kill you if you don't have enough) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("John Cena Split Mail",				SCALE_MAIL, /* resist sleep, discount action, increases unarmed damage like robe of pugilism */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("jyl -not xlated call ken",				ELIF_S_JEWEL, /* randart feminism gem, can be invoked to gain d500 zorkmids */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 1000L ),

A("Musical Snorkel",				WOODEN_FLUTE, /* magical breathing when wielded, can be invoked to turn water around you into dry land in a 5x5 area */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 2000L ),

A("Quick Jock",				LEATHER_SADDLE, /* 20% increased speed when riding, 1 in 1000 chance per turn that the steed is healed back up to full HP and has all negative status effects cured */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_JOCKEY, NON_PM, 2000L ),

A("Faer Me",				LEATHER_JACKET, /* resist fear; if you're riding, low-level monsters close by will walk randomly sometimes and you can enter shops where you get a discount */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_JOCKEY, NON_PM, 3000L ),

A("Give Crap A Bad Name",				KNIFE, /* randart knife */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Claudia's Beauty",				LOW_BOOTS, /* initialized to always be superhard sandals, allows you to #monster to fart or spread perfume */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Max the Secret Agent",				REGULAR_GLOVES, /* initialized to always be telescope, gives you maxxed sight range while worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Kati's Antagonism",				REGULAR_GLOVES, /* initialized to always be telescope, autocurses, aggravate monster and feminism aggravation */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Now Your Able To Poly",				LEATHER_CLOAK, /* initialized to always be osfa cloak, polymorphitis */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Pineapple Tycoon's Finish",				LEATHER_CLOAK, /* initialized to always be tarpaulin cloak, death ray resistance, using appraisal on a weapon gives 75% chance that you don't get a timeout */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Protected Mode Run-time",				AMULET_OF_ESCAPE_PROBLEM, /* timerun effect, free action and discount action */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Heliokopis's Playstyle",				QUARTERSTAFF, /* escape past effect, 20% better spellcasting chances */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(8, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Property Grumble",				ETERNAL_LIAR_BOOTS, /* permacurse effect, 10 extra points of AC, +8 to-damage but -5 to-accuracy */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Arabella's Lightswitch",				RING_MAIL, /* detectation effect, can be invoked for litroom(TRUE, obj);, highscore bug, gaining score (urexp) via exper.c is doubled, infravision and +25 spawn freq for M4_ORGANIVORE */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 6000L ),

A("Habiba's Matronage",				STANDARD_HELMET, /* initialized to always be hardcore cloth, activates hardcore alien mode, also gives intrinsic half physical damage and regeneration, double magical breathing and +2 increase damage */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Ahaund",				GAUNTLETS_OF_RAINBOW_MOUD,
	(SPFX_RESTR|SPFX_EVIL|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Particularly Heavy Stone",				THROUGH_THE_FLOOR_BOOTS, /* increases the chance of going down more levels at once via downstair */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("CAPS LOCK",				ABCDE_HELMET, /* uppercase monsters have +5 spawn freq */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("On Trance",				AMULET_OF_MISCOLORATION, /* technicality */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Klenschgor",				AMULET_OF_LOST_KNOWLEDGE, /* +5 int */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Taste The Rainbow",				SHADOW_SCOURGE, /* one rainbow effect, resist psi */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ATTK), 0, 0,
	STUN(3,4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Unattainable Nineteen",				SHORT_SWORD, /* randart short sword, dschueueuet trap effect, +2 STR */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(10, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Godawful Enchantment",				CLUB, /* randart2 club, autocurses, johanna trap effect, resist psi, contamination and death */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(15, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Some Chamber Door",				ICHCAHUIPILLI, /* antje trap 2, sight bonus, infravision, heavily autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Gondolin's Hidden Passage",				LONG_SWORD, /* randart2, autocurses, kerstin trap effect, +5 STR and CON, free action, see invisible */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(2, 2), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 10000L ),

A("School Satchel",				BRONZE_PLATE_MAIL, /* may-britt effect, +2000 carry cap and max carry cap */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Farther Into The Jungle",				BAMBOO_MAIL, /* autocurses, charlotte trap effect, +2 vision range (stacks with other sight bonuses), +10% chance to block if you're using a shield, scent view and echolocation, 80% chance that if you would normally have received sanity, you don't */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Just A Hunk Of AC",				BRONZE_PLATE_MAIL, /* 7 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Cold Insignia",				BRONZE_PLATE_MAIL,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Scarabrooch",				BRONZE_PLATE_MAIL, /* blindness resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Fartdetection",				BRONZE_PLATE_MAIL, /* can be invoked for trap detection but doing so also summons a hostile MS_FART_LOUD next to you with superaggravatefrenzy */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 1000L ),

A("Meet Woman Antje",				STRIPE_MAIL, /* autocurses, antje trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Punked by Heidi",				STRIPE_MAIL, /* echolocation */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Arms Oversight",				STRIPE_MAIL, /* defusing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Go Fully Off",				MORNING_STAR, /* randart2 morning star, defusing */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Half Super Enchantment",				RED_LIGHTSABER, /* randart lightsaber, defusing */
	(SPFX_RESTR), 0, 0,
	PHYS(10, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Gambleboy",				CASINO_CHIP, /* gives 4 extra chips */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Welcoming Gift",				CASINO_CHIP, /* gives 19 extra chips */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Woozie's Fish",				CASINO_CHIP, /* creates a peaceful croupier next to you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Main Win",				CASINO_CHIP, /* gives 2000 zorkmids when used */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Sindacco's Forgery",				CASINO_CHIP, /* doesn't give you a casino chip, but activates the cops */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Web Conference",				WAN_SLEEP, /* shoots an additional sleep ray in a random direction */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("blu.toe",				EAGLE_BALL, /* randart2 orb, free action, juen trap effect, etherwind, +5 kick damage and your kick cannot be clumsy */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("asseignment(curse)",				ORCISH_CHAIN_MAIL, /* autocurses, can be invoked to curse an item of your choice */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 1000L ),

A("Dolores' Winning Strat",				BOW, /* randart bow, can't shoot more than 2 arrows per turn, doesn't get penalties for trying to fire at point blank range */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Fivefoldswitch!",				HEAVY_MAIL, /* invoking it while it's at least +1 cures all nasty trap effects, and the armor becomes prime cursed -20 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 5555L ),

A("Children Mjollnir",                   HEAVY_HAMMER,
	(SPFX_RESTR|SPFX_ATTK),  0, 0,
	ELEC(5,12),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Weaker Mjollnir",                   WAR_HAMMER,
	(SPFX_RESTR|SPFX_ATTK),  0, 0,
	ELEC(5,12),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Schwoingle",			PARTISAN,
	(SPFX_RESTR), 0, 0,
	PHYS(5,12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2800L ),

A("Latin Spear",			SPETUM,
	(SPFX_RESTR), 0, 0,
	PHYS(2,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1700L ),

A("Dark Singer",			HALBERD,
	(SPFX_RESTR), 0, 0,
	PHYS(1,10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Seu",			LUCERN_HAMMER,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(4,8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Ga-bradda dana breight bag",		UMBRELLA,
	(SPFX_RESTR), 0, 0,
	PHYS(6, 9), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Iceblock's Negation",  HEAVY_HAMMER,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN),  0, 0,
	FIRE(15,14),	FIRE(0,0),	NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Back To Origin",				WAN_RESTORATION, /* resets all base stat values to their starting values */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("The Best Offense Is A Good Defense",				LONG_SWORD, /* randart melee weapon, 1 in 1000 when hitting something that your torso armor's enchantment value increases by one up to a max of +7 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Bat from Nowopetrovsk",                BASEBALL_BAT,
	(SPFX_RESTR), 0, 0,
	PHYS(3,10),      NO_DFNS,        NO_CARY,        0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Bat from Baltimore",                BASEBALL_BAT, /* by Demo, stealer trap, banking, aggravate monster, monsters always know where you are, bad effects, dimness, and equipping it sets the timeouts for the curseuse and autocurse nastytraps to 5000 if they were lower */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(3,30),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 6000L ),

A("fook sri saeve",				AMULET_OF_SEVERE_FUCKUP, /* monster speed bug and dschueueuet trap effect when worn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Severe Uninformation",				UNINFORMATION_STONE, /* items just display ??? as their name */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("The Womanler Gives Chase",				MANLER_STONE, /* manler moves towards you more often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Doorning Day",				DOORNING_STONE, /* the allmain.c part of doorning always strikes */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("del alles.bkk",				UNIDENTIFY_STONE, /* your shit unidentifies itself 5 times faster */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Arabella's Bedrock",				STONE_OF_CURSING, /* your shit curses itself 5 times faster */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Extra-High Difficulty",				STONE_OF_DIFFICULTY, /* increases difficulty by another 10 points */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Fred's Optiometrist",				OPTION_STONE, /* your options change 10 times more often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Fully Id Or Nope",				UN_KNOWLEDGE_STONE, /* the items in question need to have known, rknown etc. identified as well, or they don't work */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Deep Descent",				DSCHUEUEUET_STONE, /* dschueueuet effect drops you deeper on average */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Halfcut Hahahahaha Hahaha Haha",				NOPESKILL_STONE, /* trying to enhance a skill cuts its training amount in half */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Clap It Away",				PROOFLOSS_STONE, /* your items unproof themselves 10 times more often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("I See Youuuuu",				UN_INVIS_STONE, /* your items become visible 10 times more often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Beyond Help",				COVID_STONE, /* you suffer from covid-19 symptoms 50 times more often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Fffff Queak Prrrt",				UBERGOD_STONE, /* farting webs are created 5 times more often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Gas As A Weapon",				FUNWALL_STONE, /* the wallfarting effect uses reallybadeffect() */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Last Foreva",				CURSED_PART_STONE, /* the cursed items have 50% to become heavily cursed, 20% prime cursed, and an independant 50% to become sticky cursed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Nuclear War Aftermath",				FALLOUT_STONE, /* contamination increases twice as fast */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Screen Glitch",				MOJIBAKE_STONE, /* more mojibake than usual */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Quasi Permanence",				LONG_SCREWUP_STONE, /* multiplies nastytrap durations by 10 again */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Trash It!",				SKILL_LOSS_STONE, /* skill loss happens 3 times as often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Flucker Low",				FLUCTUATING_SPEED_STONE, /* worse values for the fluctuating speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Grrm. TAA TATATATAAAA, TA TAAA TA TAAAAAA...",				SATAN_STONE, /* satan paralysis is rn1(5,5) turns */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Blabberblabberwanamada.",				MONNOISE_STONE, /* monster noises are 5 times as common */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Leiki's Quest",				RANG_CALL_STONE, /* rang call increases sanity by 5 times as much */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Lose ALL the spells",				RECURRING_SPELL_LOSS_STONE, /* spell memory loss drains 5 times as much */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Man Are You Lush",				ANTI_TRAINING_STONE, /* skill training loss drains 5 times as much */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Tout daau - daau - daau",				TECHOUT_STONE, /* tech timeouts are 5 times higher */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Wasteful Wither",				STAT_DECAY_STONE, /* stats decay 5 times as fast */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Mork More",				MOVEMORK_STONE, /* monsters are generated with even more movement energy */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("This Is So Funny",				HORROR_STONE, /* horror status effects happen twice as fast */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Good Luck, You'll Need It",				ARTIFICIAL_STONE, /* the cursed artifacts have 50% to become heavily cursed, 20% prime cursed, and an independant 50% to become sticky cursed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Unplayable Game.",				EVIL_PATCH_STONE, /* you gain intrinsic nasty trap effects 3 times as fast */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Not So Secret Anymore",				SECRET_ATTACK_STONE, /* monsters use their secret attacks 5 times as often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Robbie's Weariness",				WAKEUP_CALL_STONE, /* wakeup call happens 10 times as often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Tiny Visibility Window",				GRAYOUT_STONE, /* display is gray for 10 turns, normal for 5 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Make The Shroud Bigger",				GRAY_CENTER_STONE, /* shrouds a 5x5 area around you */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("See Even Less",				CHECKERBOARD_STONE, /* only one out of 4 tiles is visible */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Hoho I Ate Your Input",				LAG_STONE, /* input is discarded 50% of the time */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Arrgh He Comes For Us",				UNDRESSING_STONE, /* 5 times as likely to be undressed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Waiting For Godot",				LATENCY_STONE, /* increases the average amount of lag */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Better Put It All Away",				WRAPOVER_STONE, /* wrapover happens 5 times as often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("It Explodes!",				DESTRUCTION_STONE, /* item destruction happens 5 times as often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Who Needs Fairness Anyway",				UNFAIR_ATTACK_STONE, /* monsters use unfair attacks 5 times as often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Try It Out Longer",				AUTOCURSE_STONE, /* the cursed items have 50% to become heavily cursed, 20% prime cursed, and an independant 50% to become sticky cursed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Donot Work Atall",				TECHNIQUE_STONE, /* techs don't work at all */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("There Is A Loud Clap",				DISENCHANTMENT_STONE, /* disenchantment happens 5 times as often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Navigate This Mess",				CHAOS_TERRAIN_STONE, /* terrain transformations happen 5 times as often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Gremlin's Revenge",				STONE_OF_INTRINSIC_LOSS, /* intrinsic loss happens 10 times as often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Infernal Area",				NASTY_STONE, /* nasty trap effects happen 5 times as often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Keep Stumbling",				TRAP_CREATION_STONE, /* traps are created twice as often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Pure Aids",				STONE_OF_VULNERABILITY, /* you get hit with vulnerability effects 10 times as often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Who Needs Items Anyway?",				ITEM_TELEPORTING_STONE, /* items teleport away twice as often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Quaff That Sparkling Wine",				AMNESIA_STONE, /* amnesia happens 5 times as often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Live Fast, Die Young, Hae-iae",				TURN_LIMIT_STONE, /* taking damage reduces ascension limit by 10 times as much */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Thats Derp",				DROPCURSE_STONE, /* the cursed items have 50% to become heavily cursed, 20% prime cursed, and an independant 50% to become sticky cursed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Wallify Up!",				STONE_OF_PERMANENCE, /* walls regrow another 5 times as fast */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("FUUUUUUUUCK!",				STONE_OF_COMPLETE_FUCKUP, /* also unidentifies the base item */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Dweeeeeeeauweauw",				VACUUM_STAFF, /* can be invoked to fire a solar beam in a direction of your choice with power (gushlevel/4) */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(2, 5), NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 2000L ),

A("That Old Bug",				AMULET_OF_BURDEN, /* picking up things makes you heavier */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("No Mind Decay",				VACUUM_HELMET, /* 5 extra points of AC and makes you immune to mind flaying */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Go On A Survey Recreation",				LEATHER_CLOAK, /* initialized to always be warning coat, using the wait command drains your score by 50 points */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REGEN|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Krasch Xtra",				CLOAK_OF_RESISTANCE_PIERCING, /* resist cold, fire and shock */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("floemmelfloemmelFloemmelfloemmel",				GAUNTLETS_OF_ELEMENTAL_POWER, /* +1 to-hit and damage, 5% better spellcasting chances */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Jederdi",				ROBE_OF_MAGIC_POWER, /* lightsaber forms train twice as fast */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Castomania",				ROBE_OF_MAGIC_POWER, /* spellcasting skills train twice as fast */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Everything Comes With A Cost",				ROBE_OF_MAGIC_POWER, /* extra skill training, yawm and trapwarp effects */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Speechbreak",				RIN_LAMITY, /* magic vacuum, +5 STR and DEX */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2400L ),

A("Datlest Krann",				GAUNTLETS_OF_BAD_CASTING, /* resistance piercing */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("SuperheavyKlonk",				WEIGHTING_GAUNTLETS, /* burden, too heavy effect, +4 melee damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Trapburner",				SCR_TRAP_DISARMING, /* removes traps in an 11x11 area centered on you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Death To Traps",				DROVEN_MITHRIL_COAT, /* invoke to remove all traps on the current dungeon level, but doing so uses up the armor */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 10000L ),

A("Posso Continuare Laterale",				EAGLE_BALL, /* randart2 orb, invoke to unequip an equipped item regardless of curses */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 4000L ),

A("Curse The Time Shift",				AMULET_OF_STRANGULATION, /* randart2 amulet, 1 in 10000 to advance the turn counter by 1000 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Stealth or Stealing",				ELVEN_CLOAK, /* double stealth, stealer trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Scribe What You Want To Scribe",				SCR_BLANK_PAPER, /* doesn't disappear if you fail to write an unknown scroll */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Pagan Poetry",				SPE_BLANK_PAPER, /* if you turned it into some other spellbook and read it successfully, the spell in question becomes the "super special spell" which is easier to cast and costs less mana */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Arabella's Exchanger",				IMPLANT_OF_ANAL_RETENTION, /* randart implant, stronger version of bad equipment trap effect, and if you don't have hands the "invert" trinsic is suppressed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Magdalena's Cuddleweapon",				LONG_SWORD, /* randart melee weapon, magdalena trap effect, 5 extra points of AC */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Duh Bewegeo Zisch",				LEATHER_ARMOR, /* randart2 armor, 1 in 100 that you become confused for rn1(10,10) turns, are pushed away, and a lightning bolt is fired by you in a random direction with d6 strength */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("It Power Knoepp",				LEATHER_ARMOR, /* randart armor, 1 in 100 that you are pushed away and all monsters also have their positions shuffled */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Sevebreakyou, sevebreak!",				LEATHER_ARMOR, /* randart2 armor, double drain resistance, intrinsic speed, 1 in 200 chance to gain a point of alignment, autocurses and if you load the game, you get a hangup penalty */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Magistus",				ARROW, /* randart ammo, reduces target's max HP by one */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Trismagistus",				ARROW, /* randart2 ammo, reduces target's max HP by three */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Skull Sword",				SHORT_SWORD, /* randart short sword, halves melee damage output but deals +5 damage when thrown */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Violent Skull Sword",				LONG_SWORD, /* randart2 long sword, halves melee damage output but deals +10 damage when thrown and a thrown one can also drain the enemy's level */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Ultralaser",				SCR_LIGHT, /* randart2 scroll, reading it allows you to fire a disintegration beam in a direction of your choice */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("XXXXXXXXXXXXXXXXXXXXX",				STEEL_SHIELD, /* levuntation, 1 in 10000 that a random good or bad effect happens */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Dramborleg",				DWARVISH_BATTLE_AXE,
	(SPFX_RESTR|SPFX_INTEL|SPFX_WARN|SPFX_DFLAG2|SPFX_DEFN), 0, M2_DEMON,
	PHYS(8, 8), DFNS(AD_MAGM), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 9000L ),

A("The Sword of Annihilation",				LONG_SWORD, /* disintegration resistance when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 25000L ),

A("Glamdring (chinese bootleg version)",				LONG_SWORD, /* randart2 long sword */
	(SPFX_RESTR|SPFX_WARN|SPFX_DEFN|SPFX_DFLAG2), 0, M2_ORC,
	PHYS(8, 0), DFNS(AD_ELEC), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("The Staff of the Archmagigi",				QUARTERSTAFF, /* randart2 quarterstaff */
	(SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DEFN|SPFX_HSPDAM), 0, 0,
	STUN(5, 8), DFNS(AD_MAGM), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 35000L ),

A("Shadowblade based on Stormbringer and Grimtooth",				ATHAME, /* bloodthirsty, autocurses, magic vacuum */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ATTK|SPFX_DEFN|SPFX_INTEL|SPFX_DRLI|SPFX_SEARCH|SPFX_STLTH), 0, 0,
	DRLI(8, 10), DFNS(AD_DRLI), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 15000L ),

A("Gauntlets of Purity",				GAUNTLETS_OF_POWER, /* death ray resistance */
	(SPFX_RESTR|SPFX_INTEL|SPFX_REFLECT|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 25000L ),

A("Ashmar",				DWARVISH_ROUNDSHIELD,
	(SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_REGEN|SPFX_HPHDAM), 0, 0,
	NO_ATTK, DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("How/As Crude Liver",				FULL_PLATE_MAIL, /* life saving when worn, and is used up if it saves you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Cold Protecti",				HEAVY_MAIL, /* protects potions from cold */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Superheavy Garbo",				HEAVY_MAIL, /* weighs 250 additional units, 50% chance to resist an AD_DCAY melee attack */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Sartiro",				HEAVY_MAIL, /* cannot be eroded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Swing For The Fences",				BASEBALL_BAT, /* +10 strength */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Fully On The Twelve",				BASEBALL_BAT, /* +20% chance to land a critical hit */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Fully On It",				CLUB, /* randart2 club, +20% chance to land a critical hit */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Lynn's Secludedness",				CRYSTAL_PLATE_MAIL, /* invis when worn */
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Lynn's Elusion",				CRYSTAL_PLATE_MAIL, /* teleportitis, disables tele control */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Clanking Rattle",				CRYSTAL_PLATE_MAIL, /* aggravate monster, and wake_nearby is triggered every turn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Lynn's Evervigilance",				CRYSTAL_PLATE_MAIL, /* sleep resistance and discount action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Grounded Forever",				SMALL_SHIELD, /* disables flying, but if its material is WOOD it grants lightning immunity */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Little Protecter",				SMALL_SHIELD, /* +5% chance to block */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("It Shimmers",				LARGE_SHIELD,
	(SPFX_RESTR|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Unknowingness As A Weapon",				REGULAR_GLOVES, /* randart2 gloves, +5 to-hit and damage as long as the base item isn't identified */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Kawa Jur Fes",				STANDARD_HELMET, /* randart helmet, autocurses, you cannot eat or quaff, 20% chance to resist corona */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Krol-Gr",				REGULAR_GLOVES, /* randart gloves, 1 in 1000 to go berserk for 25 turns */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Hoho-Dingo",				LOW_BOOTS, /* randart2 boots, 12 extra points of AC if spe isn't known */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Proofingness Poofs",				LONG_SWORD, /* randart2 melee weapon, cannot be eroded if rknown isn't known */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Buck Shot",				BOW, /* randart launcher, +2 increase damage if BUC isn't known */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("For The Permablind Potato",				SCR_EBB_TIDE, /* deals +20 damage if its appearance isn't known */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Hell Or High Water",				SCR_FLOOD_TIDE, /* flying and magical breathing when wielded; if you are chaotic and read it over a water tile, that water tile transforms into lava */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("of Voiding",				SHIELD, /* randart shield, you take d2 less damage (not below 1) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("of Nulling",				SHIELD, /* randart2 shield, you take d4 less damage (not below 1) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("DarthaDart",				ARROW, /* randart ammo, gains a point of enchantment if it hits something (up to a max of +15) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("U-Tschelli",				FLY_SWATTER, /* randart paddle */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ACID(5, 8), DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("M! M! M!",				FLY_SWATTER, /* randart2 paddle, damage h@ck, for every skill level in paddle below supreme master it also deals +2 damage */
	(SPFX_RESTR), 0, 0,
	PHYS(20, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Skoglo",				LONG_SWORD, /* randart melee weapon, if you hit something there's 1 in 200 chance to be pushed away for up to 100 tiles */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Lakitu Get Me Out",				FISHING_POLE, /* you can always climb out of water and fishing doesn't hit yourself */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Ore Deposit",				PICK_AXE, /* +5 to-hit and +20 damage against lithivore and "helpfully" there is no DFLAG3 so this effect has to be h@cked in */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Clench Fist",				PICK_AXE, /* resist fear */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Hibernia Skirimi",				PICK_AXE, /* randart pick-axe */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(6, 10), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Wegeo Acqua De Eisu Forte",				LEATHER_ARMOR, /* randart2 leather armor, transforms water into ice when you walk over it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Indoburdenia Virii",				MACE, /* randart mace, burden, +10 strength */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Altadoon Herma Mora",				ROBE, /* randart robe, 20% better spellcasting like failuncap, resistance piercing, wakeup call, enmity */
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6000L ),

A("Kronsch Zadroer Woff",				LONG_SWORD, /* randart2 melee weapon, disint res */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Just A Little Power",				ARROW,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 30L ),

A("Arrow Of Slay Good",				ORCISH_ARROW, /* +15 damage against lawful monsters */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Egri Dueu",				ORCISH_ARROW, /* does poison damage like dirge */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Eeep",				ORCISH_SHORT_SWORD, /* +10% critical hits */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Cowwer",				ORCISH_BOW,
	(SPFX_RESTR), 0, 0,
	PHYS(6, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Elegy Res",				ORCISH_BOW, /* poison res */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Rabbit Hunting",				CROSSBOW_BOLT,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 60L ),

A("Snipesnipesnipe",				CROSSBOW_BOLT, /* +5 range */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 80L ),

A("Wu-Tschi!",				DAGGER, /* +10 throwing damage, spawns with d10 extra ammo */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 30L ),

A("Poepoepoepoeoeu!",				ORCISH_DAGGER, /* +4 multishot, +5 stack size */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 40L ),

A("Wummp",				ORCISH_SPEAR, /* +12 thrown damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Particularly Solid Skull",				ORCISH_HELM, /* cannot be eroded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Mini Computer",				ORCISH_HELM,
	(SPFX_RESTR|SPFX_WARN|SPFX_DFLAG2), 0, M2_ELF,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Russet Enchantment",				STANDARD_HELMET,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Poison Hand",				REGULAR_GLOVES, /* poison resistance, lower risk of losing stats to poison */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Is Everywhere",				__CANDLE, /* +4 sight range if it's on and wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Useful Base",				ORCISH_CLOAK, /* poison resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Lightweightness",				ORCISH_CLOAK, /* intrinsic speed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Behind Cuntingness",				ORCISH_CLOAK, /* allows you to backstab monsters */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Forged of Steel",				DWARVISH_CLOAK, /* 3 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Mining for Fun and Profit",				DWARVISH_CLOAK, /* increased digging speed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Corny Dood",				DWARVISH_CLOAK, /* if you're male, +3 STR and CON */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Casteasy",				ELVEN_CLOAK, /* 5% better spellcasting chances */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Oh My God Spandex",				ELVEN_CLOAK, /* +200 carry cap */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("No Metal Allowed",				ELVEN_CLOAK, /* if you wear absolutely zero metallic items, it grants free action and discount action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Unwerth",				LEATHER_CLOAK, /* aggravate monster and sustain ability */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Fradle of Eg",				LEATHER_CLOAK, /* +1 MC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("But Shes Homeless",				LEATHER_CLOAK, /* while "umoved" is true, you resist poison and sickness and have full nutrients and your items don't get wet */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Notonhead",				DWARVISH_IRON_HELM, /* 10% chance to take no damage in melee */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Rest The Case",				DWARVISH_IRON_HELM, /* immunity to beheading */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("A Little Sugar",				REGULAR_GLOVES, /* randart gloves, +3 DEX, free action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Measure Skilling",				ELVEN_BOW, /* +1 multishot */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Monksterman",				ROBE, /* +5 unarmed damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Tech It Up",				ROBE, /* technicality */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Hoardit",				ROBE, /* body armor skill trains twice as fast, and the passive soresu training when you have a lightsaber is 10x faster */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Some Actual Armor",				ROBE, /* putting it on sets it to +5 if it was lower */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Cremated",				SHIELD, /* randart2 shield, d5 thorns in melee, sleep resistance, color becomes orange when equipped */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Doedoedoedoedoedoedoe-test",				LEATHER_CLOAK, /* randart2 cloak, grayout, message suppression while everything is gray, 1 in 100000 per turn to trigger ragnarok, double invisibility, 66% chance to dodge a missile attack */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Bamsel In The Way",				LEATHER_CLOAK, /* randart cloak, "whicharmor" always selects the cloak */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Iceblock Sword",				SILVER_LONG_SWORD, /* disables fire resistance */
	(SPFX_RESTR|SPFX_ATTK|SPFX_EVIL), 0, 0,
	COLD(6, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Aval0n",				SCIMITAR, /* randart2 scimitar */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	PHYS(1, 10), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4500L ),

A("Sonicboom Baby, let's do this thing.",				MORNING_STAR, /* randart2 morning star, deafness */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Stay Full For A Week",			FOOD_RATION, /* eating it gives 7000 extra nutrition */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("No Moon At All",				FLAIL, /* randart2 flail */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(2, 12), COLD(0, 0), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),

A("Amanokuboko",				HALBERD,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 2), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Esshugo",				SUGUHANOKEN,
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	PHYS(5,3), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("One Strike",				FLY_SWATTER, /* conflict */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(7,3), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 300L ),

A("Schrongschrong",				OTAMA,
	(SPFX_RESTR), 0, 0,
	PHYS(3,1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Mons Infernalis X2",				SPEAR, /* randart2 spear, +10 strength */
	(SPFX_RESTR), 0, 0,
	PHYS(5,3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Verygrimtooth",				ORCISH_DAGGER, /* poison damage like dirge */
	(SPFX_RESTR), 0, 0,
	PHYS(2,6),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("Now That's A Good Enchantment",				ORCISH_DAGGER, /* equipping it while it's less than +7 sets it to +7 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("The Typical Orc",				ORCISH_DAGGER, /* poison resist when wielded, +4 to-hit when dual-wielding and if you happen to be dual-wielding two of these, another +10 to-hit */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Stabdab",				ELVEN_DAGGER,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Talonhide",				SCALPEL,
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Wendelder",				ORCISH_SHORT_SWORD, /* death and disint res */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Grrrrrrr!",				ORCISH_SHORT_SWORD,
	(SPFX_RESTR|SPFX_WARN|SPFX_DFLAG2), 0, (M2_ELF|M2_GNOME|M2_DWARF),
	PHYS(8, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1400L ),

A("Plltsch",				SCIMITAR, /* swimming, unbreathing */
	(SPFX_RESTR), 0, 0,
	PHYS(3, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("HackNSlash",				SCIMITAR, /* +5 bleeding damage, very fast speed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Tap Bouncer",				BRASS_KNUCKLES,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Aintgetin",				BRASS_KNUCKLES, /* 1 in 3 chance to teleport the target away if it fails a MR check */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Hard Impact",				BRASS_KNUCKLES, /* +5 STR */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Flll...sch",				IRON_SABER,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Clah",				IRON_SABER,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(3, 3), DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Ueh The Part Is +3",				IRON_SABER, /* sets itself to +3 when wielded, regardless of what enchantment value it had before */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 333L ),

A("Slash The Undead",				IRON_SABER,
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_WARN), 0, (M2_UNDEAD),
	PHYS(4, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Blullglull",				LEATHER_SABER,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Solgol",				LEATHER_SABER,
	(SPFX_RESTR|SPFX_DFLAG2), 0, (M2_LORD|M2_PRINCE),
	PHYS(6, 15), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 700L ),

A("Falco's Orb",				EAGLE_BALL, /* +1 increase accuracy and damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Powcharge",				EAGLE_BALL, /* +1 multishot with ranged weapons */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Smart Cleaver",				GRINDER, /* the grinding attack deals +d6 extra damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Dumber Cleaver",				GRINDER, /* bloodthirsty, the grinding attack deals +d6 extra damage but also hits peaceful and tame creatures */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(3, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Krart-t-t-t-t",				GRINDER, /* +2 sight range */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Inconspicuous Start",				QATAR,
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	PHYS(3, 7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("Kicksin Girl",				QATAR, /* +5 kick damage if you're female */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 700L ),

A("Trapass (what a name)",				QATAR, /* trap revealing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("ASS ASS in ASS",				QATAR, /* randart2 claw, can be invoked for trap detection */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 4000L ),

A("Boemm",				SILVER_MACE,
	(SPFX_RESTR), 0, 0,
	PHYS(9, 9), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("A Siya",				SILVER_MACE,
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_WARN), 0, (M2_DEMON),
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Pricktrick",				BATLETH, /* +6 bleeding damage, d3 thorns in melee */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Secanted",				BATLETH, /* 3 extra points of AC when wielded, and if you have a shield, +5% chance to block */
	(SPFX_RESTR), 0, 0,
	PHYS(14, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Clop-clop-clop",				MORNING_STAR,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 700L ),

A("Very-specificness",				WAR_HAMMER, /* +1 increase accuracy */
	(SPFX_RESTR|SPFX_DCLAS|SPFX_DEFN), 0, S_WORM,
	PHYS(12, 29), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2700L ),

A("Charr",				SPETUM, /* acid res when wielded */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	COLD(4, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1400L ),

A("Longloss",				RANSEUR, /* sight bonus, -d10 melee and ranged to-hit */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Garant of Uara",				RANSEUR,
	(SPFX_RESTR), 0, 0,
	PHYS(21, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("Storm On",				VOULGE, /* very fast speed */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Hey There",				VOULGE, /* successfully applying it at an enemy drains its maximum health by one */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1600L ),

A("Fooch",				FAUCHARD,
	(SPFX_RESTR), 0, 0,
	PHYS(6, 7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1400L ),

A("hit a corner",				FAUCHARD, /* can hit squares that you cannot see */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1800L ),

A("+5 To All",				GUISARME, /* 5 extra AC when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 555L ),

A("PhysStatBoost",				GUISARME, /* +5 STR, CON and DEX when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("GlangGlang",				BILL_GUISARME, /* intrinsic speed */
	(SPFX_RESTR), 0, 0,
	PHYS(7, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Dongdodongdodong",				BILL_GUISARME, /* intrinsic speed, but deactivates extrinsic speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(8, 13), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Sorbitur",				BEC_DE_CORBIN, /* resist poison */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Gad Inaf",				ORCISH_SPEAR,
	(SPFX_RESTR), 0, 0,
	PHYS(8, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Infaspel",				ORCISH_SPEAR, /* 5% better spellcasting chances */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Miss Does It Real",		 		SPEAR, /* -10 melee to-hit */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 700L ),

A("Aeiaeaeaeaeaeae",				SPEAR,
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1800L ),

A("Nosbado",				SPEAR, /* 20% chance to resist bad effects */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("dot line dot line dot line (and so on)",				SPEAR,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(6, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1400L ),

A("Underirdic!",				SPEAR, /* you can walk on farmland and mountains */
	(SPFX_RESTR), 0, 0,
	PHYS(3, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Pointed Javelin",				JAVELIN, /* spawns with rn1(5,5) stack size */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Goldstruck",				JAVELIN, /* +10 damage when thrown */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Tropical Wood Selection",				JAVELIN, /* spawns with +4 stack size, becomes poisoned when thrown */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Just Fire Away",				BOW,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Sights Zoomed",				BOW, /* infravision */
	(SPFX_RESTR|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Kreeso",				ELVEN_ARROW,
	(SPFX_RESTR), 0, 0,
	PHYS(3, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Goodres Elven",				ELVEN_ARROW, /* cannot be eroded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 150L ),

A("Samski-err",				YA,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 70L ),

A("Goliath Be Afraid",				SLING,
	(SPFX_RESTR), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Snipegiant",				SLING, /* +1 multishot */
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_WARN), 0, M2_GIANT,
	PHYS(5, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1400L ),

A("Dole 'Em All Out",				SLING, /* +2 multishot */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1800L ),

A("Flingpower!",				SLING, /* sling skill trains twice as fast */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Dotri",				DART,
	(SPFX_RESTR), 0, 0,
	PHYS(2, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Flawwer",				DART,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(4, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 40L ),

A("Muhish",				DART, /* damage h@ck, spawns with 3x as much stack size */
	(SPFX_RESTR), 0, 0,
	PHYS(12, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 12L ),

A("Way Too Long",				SHURIKEN, /* +10 range */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Seven's Good",				SHURIKEN,
	(SPFX_RESTR), 0, 0,
	PHYS(3, 7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 73L ),

A("Cutnerve",				SHURIKEN, /* +d6 bleeding damage to targets */
	(SPFX_RESTR), 0, 0,
	PHYS(1, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 60L ),

A("Aeres Running Shoes",				SPEED_BOOTS, /* fear and fear factor, prostitute taunts give you xfear for rnz(1000) (more) turns */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Say 'Chess'",				SHOTGUN, /* checkerboard effect, +2 range */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 6000L ),

A("Hyper-Intelligence",				SHOTGUN, /* +7 INT when wielded, and if your ammo hits a target there's a mightbooststat(INT) effect, but you cannot have speed, free action, warning or sight bonus while wielding it and unequipping it increases your debts by d100 if your debts are below 10 million */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Enemy Dead At Close Range",				SAWED_OFF_SHOTGUN, /* -1 range, -10 to-hit */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Italy Si-Es",				AUTO_SHOTGUN, /* +4 range, -d25 to-hit */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Spreadout",				SHOTGUN_SHELL,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Bullpow",				SHOTGUN_SHELL,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(7, 14), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 30L ),

A("Klarnigur",				AUTO_SHOTGUN_SHELL, /* spawns with double the usual ammo amount */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 40L ),

A("Wattz Power",				HAND_BLASTER,
	(SPFX_RESTR), 0, 0,
	PHYS(10, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Skilled Throughload",				HAND_BLASTER, /* +1 multishot */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Spew The Bow",				ARM_BLASTER, /* if you fire a salvo, you also fire a d4-strength magic missile in the same direction */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1800L ),

A("Watertrooper",				MG_BULLET, /* dmg h@ck */
	(SPFX_RESTR), 0, 0,
	PHYS(12, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("A Mile And A Half",				SNIPER_BULLET, /* +25 range */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 150L ),

A("Long Mile",				BLASTER_BOLT, /* +5 range */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 70L ),

A("Half Mile",				HEAVY_BLASTER_BOLT,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DRLI), 0, 0,
	DRLI(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 80L ),

A("Homing Beam",				BFG_AMMO, /* dmg h@ck */
	(SPFX_RESTR), 0, 0,
	PHYS(8, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 40L ),

A("Eight Thousand",				BFG_AMMO,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 40L ),

A("MRLS",				ROCKET, /* +1 multishot */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Miss Launcher",				ROCKET_LAUNCHER, /* dmg h@ck, +2 range */
	(SPFX_RESTR), 0, 0,
	PHYS(8, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Vihat Baguetten Bus Stop",				QATAR, /* randart claw, dmg h@ck, full nutrients, clairvoyance */
	(SPFX_RESTR), 0, 0,
	PHYS(10, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Vilea's Secret",				QATAR, /* randart2 claw, searching has 1 in 20 chance that "cause fear" is triggered with no downsides for the player */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Emma's Sympathy",				WAR_HAMMER, /* randart hammer, 1 in 25 chance that the target monster has to pass a MR check or becomes peaceful */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("UT Ruler",				LEATHER_SABER, /* randart saber, +d10 bleeding damage to targets */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Vorpal Ruler",				LEATHER_SABER, /* randart2 saber, +d10 bleeding damage to targets */
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Bread For The World",				GRINDER, /* randart grinder, full nutrients, slow digestion, regeneration */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Blood-Smeared Saw",				GRINDER, /* randart2 grinder, bloodthirsty */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(10, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Pirattery",				SCIMITAR, /* randart scimitar, pirate speak */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Blue Corsar Swimming",				SCIMITAR, /* randart2 scimitar, infravision, prevents items from becoming wet, resist cold and shock */
	(SPFX_RESTR), 0, 0,
	PHYS(7, 7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Manaboost",				EAGLE_BALL, /* randart orb */
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	PHYS(0, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 700L ),

A("Nail Impact",				BRASS_KNUCKLES, /* randart2 knuckles, +10 bleeding damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Steel On Steel",				BRASS_KNUCKLES, /* randart knuckles, +3 AC */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("FiveOnPin",				CROSSBOW_BOLT, /* randart crossbow bolt */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 55L ),

A("Sigmund's Small Load",				CROSSBOW_BOLT, /* randart2 crossbow bolt, spawns with stack size of 1, bisects targets */
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("D---!",				ARROW, /* randart arrow */
	(SPFX_RESTR), 0, 0,
	PHYS(3, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 12L ),

A("flai awei",				ARROW, /* randart2 arrow, +10 range */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 30L ),

A("Creamres",				SLING, /* randart2 sling, resist fire, cold, sleep, acid and psi, invoke for cure glib */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 800L ),

A("Be Conserved",				DART, /* randart dart, 90% chance to be preserved if it would have mulched */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Fliufliufliuuuuuuu!",				DART, /* randart2 dart, spawns with 3x stack size */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 30L ),

A("Ninjingy",				SHURIKEN, /* randart shuriken, +2 range */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 30L ),

A("Capauer",				SHURIKEN, /* randart2 shuriken, if your shuriken skill isn't maxxed, every maximally reachable skill level starting from expert gives +1 multishot */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 30L ),

A("Let It Stay",				RED_DRAGON_SCALES, /* randart dragon scales, if you put them on while they're no DSM yet they become +3 if they were lower and in non-DSM form they also give 7 extra AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Ha-Monk",				RED_DRAGON_SCALE_MAIL, /* randart DSM, doesn't hinder you if you're supposed to wear light armor, including no armor spellcasting penalty, and gives 1 extra AC per soresu skill level */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4500L ),

A("It Is Complete Now",				RED_DRAGON_SCALE_SHIELD, /* randart DSS, gives all foo-o-pathies */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Drowsing Rod",				QUARTERSTAFF, /* randart2 quarterstaff, sleep res when wielded, 1 in 3 that targets have to pass a MR check or be put to sleep if not sleep resistant */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Mystic Eyes",				LENSES,
	(SPFX_RESTR|SPFX_SEARCH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Mortality Dial",				MACE, /* randart2 mace */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	PHYS(10, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Staff of Rot",				QUARTERSTAFF, /* randart2 quarterstaff, can rot target's armor */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 6), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Xanathar's Ring of Proof",				RIN_ADORNMENT, /* randart ring, also gives intrinsic stealth */
	(SPFX_RESTR|SPFX_STLTH|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Hammer of the Gods",				HEAVY_HAMMER, /* sickness resistance when wielded */
	(SPFX_RESTR|SPFX_WARN|SPFX_INTEL|SPFX_DFLAG2), 0, (M2_DEMON|M2_UNDEAD),
	PHYS(8, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 25000L ),

A("I Don't Particularly Like High Heels",				LOW_BOOTS, /* randart2 boots, never count as high heels */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("This Trench War Has Been Fought",				LONG_SWORD, /* randart melee weapon, if the monster has less than 10% health remaining it dies outright and if it has less than 33%, it loses half of what it had remaining */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Plus To Dam",				REGULAR_GLOVES, /* +2 increase damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Plus To Hit",				REGULAR_GLOVES, /* +5 increase accuracy */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("strbonus",				REGULAR_GLOVES, /* +5 STR */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("dexbonus",				REGULAR_GLOVES, /* +5 DEX */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Quite Normal",				STANDARD_HELMET, /* +1 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Steeler",				STANDARD_HELMET, /* +3 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Havewings",				STANDARD_HELMET, /* res fire, cold and shock, anti-teleportation, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Cliervoyens",				STANDARD_HELMET, /* shows monsters in a 5x5 area around you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Speeeeeed",				HIGH_BOOTS, /* fast speed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Slowing",				HIGH_BOOTS, /* slows you down twice as much as spirit, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Ultra-P",				HIGH_BOOTS, /* levitation */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Oh Man Boring",				HIGH_BOOTS, /* +1 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 12L ),

A("Flufiv",				LOW_BOOTS,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Sterdynes",				LOW_BOOTS, /* +1 AC, becomes erosionproof when worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Anti Slide",				LOW_BOOTS, /* can walk on snow and ice */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Tscheinschform",				LOW_BOOTS, /* invoking it changes its base item to a completely random ("randart2") type of boots */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 8000L ),

A("Extra Latitude",				PARTIAL_PLATE_MAIL, /* +5 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("And Longitude",				PARTIAL_PLATE_MAIL, /* +500 carry cap, +1000 max carry cap */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Smiley Face",				RIBBED_PLATE_MAIL, /* +3 CHA, +1 all other stats */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Redguard Appearance",				RIBBED_PLATE_MAIL, /* resist fire and poison */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Godly Protect",				FULL_PLATE_MAIL, /* 20% that a mhitu attack doesn't damage you */
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Full Spectrum",				FULL_PLATE_MAIL, /* resist fire, cold, shock, poison, sleep, acid and psi */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 12000L ),

A("Dark-L",				FULL_PLATE_MAIL, /* fire cannot destroy your items nor burn your armor, infravision, nolite, monsters are short-sighted */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Feiljur",				PLASTEEL_ARMOR, /* AC becomes worse by 5 points */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Floex",				PLASTEEL_ARMOR,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Hirr",				PLASTEEL_ARMOR, /* becomes erosionproof when worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Winnnnng",				PLASTEEL_HELM, /* autocurses, invoke detects objects and traps and gives 20 turns of detect monsters */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 4000L ),

A("Flyyyyy",				PLASTEEL_HELM, /* flying, anti-teleportation, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Em-Si",				PLASTEEL_GLOVES, /* +1 MC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Not Below Nine",				PLASTEEL_GLOVES, /* AD_MCRE cannot reduce your MC below 9 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 999L ),

A("White Loafers",				PLASTEEL_BOOTS, /* sexy flats, anastasia trap effect */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Whoa Look At Those",				PLASTEEL_BOOTS, /* sexy flats, anna trap effect, heavily autocurses, can be invoked to gain a random intrinsic */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 10000L ),

A("Fascend",				PLASTEEL_CLOAK, /* MC = 10, but your AC is halved */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8888L ),

A("At Arms Length",				PLASTEEL_CLOAK, /* monsters also keep walking randomly in melee */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("You Are Ugly",				TAPERED_MAIL, /* 1 in 2000 that an attempt is made to reduce your CHA by one, +1 increase damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Dammit Pick Up",				TAPERED_MAIL, /* -5 ranged to-hit, aggravate monster, sleep resist */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Stand you forwards, there would now the earl come",				LEATHER_ARMOR, /* randart armor, invis, M2_PRINCE have +5 spawn freq and they always spawn with a random gun */
	(SPFX_RESTR|SPFX_STLTH|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("There Goes She To",				LEATHER_ARMOR, /* randart2 armor, permanent stat damage nastytrap effect, wakeup call, autocurse, +4 increase damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Ten Minutes Colder",				STANDARD_HELMET, /* randart helmet, res fire and cold, but if ZT_FIRE tries to damage your items, fire res is deactivated for 1 (more) turn; searching gives 50% chance to regenerate one HP */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Poisustain",				METAL_LAMELLAR_ARMOR, /* resist poison, sustain ability */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Cannot be harmed bla-bla",				METAL_LAMELLAR_ARMOR, /* all your armor pieces cannot be eroded, heap trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Foking Tenk",				DWARVISH_MITHRIL_COAT, /* 7 extra AC */
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Clangfriend",				DWARVISH_MITHRIL_COAT, /* increased digging speed, M2_DWARF are spawned peaceful, +3 STR */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Yesexi",				BAR_CHAIN_MAIL, /* suppresses stealth */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 700L ),

A("Isimoud",				BAR_CHAIN_MAIL, /* halves monster spawn rate and level difficulty */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Seien Glou",				ORCISH_CHAIN_MAIL,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Emsi Woers",				ORCISH_CHAIN_MAIL, /* sets your MC to exactly 2 no matter what (except unbalancor, then it's still 0) */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Blaswon",				ORCISH_CHAIN_MAIL, /* +1 AC, can be invoked to increase enchantment by one up to a maximum of +1 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 1000L ),

A("Korholt",				ORCISH_CHAIN_MAIL, /* M2_ORC have +20 spawn freq and M5_VANILLA have +2 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Duedueueueueue",				ORCISH_RING_MAIL, /* teleportitis */
	(SPFX_RESTR|SPFX_EVIL|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Wu-Wu",				ORCISH_RING_MAIL, /* if you dual-wield, +2 damage and +4 to-hit in melee */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1800L ),

A("Worschat",				ORCISH_RING_MAIL,
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Klaer Ov",				ORCISH_RING_MAIL, /* recurring amnesia, 1 in 15 per turn to get a confused clairvoyance effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1800L ),

A("New Coat",				LAB_COAT, /* makes AC worse by 1 and increases MC by 1 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Ramming Support Rippage",				LAB_COAT, /* if turn counter is divisible by 3, you cannot have resist acid; peace_minded returns FALSE 50% of the time */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Detonations Mantle",				LAB_COAT, /* invoke for dynamite explosion centered on you, which doesn't hit you */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 4000L ),

A("Post Office Farmer Heng",				MUMMY_WRAPPING, /* slightly increases range of polearms (probably to 9, so 3-square distance enemies can be hit only in a straight line) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Sithe Ded",				MUMMY_WRAPPING, /* shows all S_MUMMY */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Won Steschan",				MUMMY_WRAPPING, /* invoke for 500 turns of resist fire, cold and shock */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 800L ),

A("Very Good Fit",				OILSKIN_CLOAK, /* +3 AC, AD_MCRE cannot reduce your MC below 3 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Denh Obvio",				OILSKIN_CLOAK, /* invoke to fire a poison beam with strength of 6 in a direction of your choice */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 700L ),

A("Klalter",				OILSKIN_CLOAK, /* teleportitis, you cannot have double tele control */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, LIGHT_AREA, A_NONE, NON_PM, NON_PM, 1800L ),

A("Resist Boredom",				LORICATED_CLOAK, /* resist sleep */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Protect From Heavy Rain",				ELVEN_LEATHER_HELM, /* you can pass through rain clouds with no ill effects */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2400L ),

A("Give The Bonus Stuff",				ELVEN_LEATHER_HELM, /* +1 multishot */
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Metal Negater",				ELVEN_LEATHER_HELM, /* halves metal spellcasting penalties */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2400L ),

A("Douvoned",				ELVEN_LEATHER_HELM, /* +1 damage in melee and if you hit, +5 nutrition */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1800L ),

A("Badly Dented",				DENTED_POT, /* makes AC worse by one */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Can't Be Damaged Further",				DENTED_POT, /* immune to rust and disenchantment */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Be The Lite",				DENTED_POT, /* sight bonus, +1 increase accuracy */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2600L ),

A("Yaggfalse",				ELVEN_SHIELD,
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Kluusch",				ELVEN_SHIELD, /* if the shield blocks a projectile or melee attack, you regenerate 1 HP */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Tartsch",				SHIELD, /* randart shield, resist piercing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Ronditsch",				SHIELD, /* randart2 shield, +1 increase damage and accuracy */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Havener",				URUK_HAI_SHIELD, /* 1 in 2 chance that if an arrow is shot at you, the shield will unconditionally block it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Spickar",				URUK_HAI_SHIELD, /* if you block a projectile, you can opt to fire a magic missile with strength gushlevel/3 in the direction the projectile came from, but only if the shield has no artifact timeout and it then gets one */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("First Thirtel",				ORCISH_SHIELD,
	(SPFX_RESTR|SPFX_SEARCH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Second Thirtel",				ORCISH_SHIELD,
	(SPFX_RESTR|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Third Thirtel",				ORCISH_SHIELD,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Angularity",				LARGE_SHIELD, /* prism reflection */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Yellow Status",				LARGE_SHIELD, /* resist poison, +1 AC, +2 INT */
	(SPFX_RESTR|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Spikemash",				IRON_SHOES, /* falling into a spiked pit turns it into a regular pit */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Daroh No",				IRON_SHOES, /* dart traps cannot fire poisoned darts at you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("House Animal Warner",				IRON_SHOES, /* pets whine when near traps, even when not leashed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Deepmine Safety",				IRON_SHOES, /* if you trigger a poly trap, you can choose whether or not you want to actually be polyd */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Glitch In The Monitor",				IRON_SHOES, /* double telepathy, EVC effect */
	(SPFX_RESTR|SPFX_ESP|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Cure Good",				UNICORN_HORN, /* randart unihorn, applying works as if it had 2 extra points of enchantment */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Dig, of course",				DWARVISH_MATTOCK, /* randart mattock, faster digging speed, dmg h@ck */
	(SPFX_RESTR), 0, 0,
	PHYS(9, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Eternale Delay",				DWARVISH_MATTOCK, /* randart2 mattock, digs slower than normal, resist fear */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("old person talk",				LEATHER_CLOAK, /* randart2 cloak, -5 increase accuracy, magic vacuum, halves multishot, +50 spawn freq for M5_VANILLA */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Gallow Digs",				LONG_SWORD, /* randart2 melee weapon, invoke to fire a digging beam in a direction of your choice */
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 6000L ),

A("Griene Eidi",				GREEN_LIGHTSABER, /* peacevision */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Berserk Rage",				RED_LIGHTSABER, /* doesn't go out when cursed, menu bug, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(10, 14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Fara Weapon",				RED_LIGHTSABER,
	(SPFX_RESTR), 0, 0,
	PHYS(6, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Palace Terror",				RED_LIGHTSABER, /* +5 spawn freq for MS_DEAD and 1 in 100 per turn to receive d5 sanity */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(10, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Hah-Tschh",				BLUE_LIGHTSABER, /* suppresses freeze status effect */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	COLD(3, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Cannot Do Is Frozen",				BLUE_LIGHTSABER, /* protects potions from cold, can be invoked to cure freeze */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 2500L ),

A("Vintage Memory",				RED_DOUBLE_LIGHTSABER, /* dulls 5x as often and down to -2, shades of gray, invoke for a controlled teleport */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 6000L ),

A("Fading From Memory",				VIOLET_LIGHTSABER, /* keen memory, recurring amnesia */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ATTK), 0, 0,
	STUN(0, 1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Helf Me Nevertheless",				WHITE_LIGHTSABER, /* halves speed, resist sleep, free action and discount action */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Black Mark",				WHITE_LIGHTSABER, /* -1 increase damage and accuracy, life saving */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Calf Cutlet Without Beod",				WHITE_DOUBLE_LIGHTSABER, /* hunger, full nutrients */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Smarted Diamond",				WHITE_DOUBLE_LIGHTSABER,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ELEC(8, 8), DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Kruruink",				POWERFIST, /* if you hit something and your temporary very fast speed was less than 2 turns, it is now set to 2 turns */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2400L ),

A("Extended Durability",				POWERFIST, /* 75% chance to not be eroded, 80% chance to not be dulled if it would have */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Dit Dit",				PISTOL_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Large Magazine",				PISTOL_BULLET, /* 3x stack size */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 80L ),

A("Seldom Price",				SMG_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ), /* the price is not a mistake --Amy */

A("Washington's Capper",				SMG_BULLET, /* +5 range, +1 multishot, -5 to-hit */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Lincoln's Repeater",				FLINTLOCK, /* +1 multishot */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4400L ),

A("Fire Free",				FLINTLOCK,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Actually Usable GL",				GRENADE_LAUNCHER, /* +6 range */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3400L ),

A("Ping Em Away",				GRENADE_LAUNCHER, /* +12 bashing damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Colt 1911",				PISTOL, /* resist acid */
	(SPFX_RESTR), 0, 0,
	PHYS(7, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1900L ),

A("MP5",				SUBMACHINE_GUN,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Loud Shitter",				SUBMACHINE_GUN, /* -7 to-hit, +2-3 multishot, every bullet you fire from it causes wake_nearby */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1600L ),

A("Winnetou's Friend",				RIFLE, /* damage h@ck */
	(SPFX_RESTR), 0, 0,
	PHYS(15, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Ay-Pee Rifle",				RIFLE, /* shooting a target tries to corrode its armor */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("FTS",				RIFLE_BULLET, /* +10 range, to-hit doesn't decline with range */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 120L ),

A("Evasion Break",				RIFLE_BULLET, /* enemies cannot evade, dodge, shrug off etc. the bullet */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 80L ),

A("Chinese Model",				ASSAULT_RIFLE, /* -5 to-hit */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1600L ),

A("Xuanlong",				ASSAULT_RIFLE, /* -5 to-hit, aggravate monster */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4400L ),

A("Theo's Box",				ASSAULT_RIFLE_BULLET, /* damage h@ck, spawns with d(4x stack size) extra ammo */
	(SPFX_RESTR), 0, 0,
	PHYS(10, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 180L ),

A("All Wasted",				ASSAULT_RIFLE_BULLET, /* stack size is reduced to one third of the normal amount */
	(SPFX_RESTR), 0, 0,
	PHYS(1, 1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 30L ),

A("AK-2074",				KALASHNIKOV, /* invert, every firearms skill level below expert gives +2 to-hit and +1 damage, but every skill level above it gives -2 both */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Gigantic Sun",				EAGLE_BALL, /* randart2 orb, increases your sight range by 3 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Double Free Corruption",				FRAG_GRENADE, /* if it explodes, you get a temporary nasty trap effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Bugboots of Errorness",				LOW_BOOTS, /* randart2 boots, become -1 when worn while they're higher than that, speed bug, kicking monsters while they're still -1 or lower can cause them to not do any damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Fonlauschi",				VOULGE, /* randart2 polearm, material becomes SILK when wielded, doesn't shatter when applied but is instead disenchanted down to -20 (if already -20 or worse, no change) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Twisted Turn",				KNIFE, /* randart2 knife, perma-poisoned, 1 in 100 to become either blinded, confused or hallucinating for d10 (more) turns */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Mccauley's Intelligence",				BROADSWORD, /* randart2 broadsword, your INT cannot be higher than 6, autocurses */
	(SPFX_RESTR|SPFX_DFLAG1|SPFX_EVIL), 0, (M1_HUMANOID|M1_ANIMAL),
	PHYS(8, 14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Mccauley's Argument",				SHORT_SWORD, /* randart2 short sword, magic vacuum, autocurses */
	(SPFX_RESTR|SPFX_DFLAG1|SPFX_EVIL), 0, (M1_TPORT),
	PHYS(4, 22), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Stab All Over",				UNICORN_HORN, /* randart2 unihorn, all unihorns act as if they had an additional +1 while you're wielding this and they are less likely to be vaporized or lose enchantment when applied */
	(SPFX_RESTR), 0, 0,
	PHYS(3, 11), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Unwieldytine",				TRIDENT, /* randart2 trident, weighs 200 additional units, -rn1(5, 5) melee to-hit; if you're female, also increases critical hit chance by 5% */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Turvang's Thunder",				WAR_HAMMER, /* randart2 hammer, resist stun */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(0, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Kloeb",				CLUB, /* randart club */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Nooby Bonus Style",				AXE, /* randart axe; if it's one-handed and in your main hand, it grants +2 increase damage, but if it's two-handed, it grants very fast speed and discount action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Hakuna Maalum",				SPEAR, /* randart spear */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Ashikaga's Revenge",				TWO_HANDED_SWORD, /* randart2 two-handed sword, bisects targets, stealer trap effect and AD_SEDU have +10 spawn freq */
	(SPFX_RESTR|SPFX_BEHEAD|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Tschapsnap",				BULLWHIP, /* randart2 whip, sight bonus, monsters with distu(mtmp->mx,mtmp->my) greater than 290 move with 2-3 times as much speed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(20, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Raggo's Rock",			ROCK, /* always spawns with quantity of 1, by Demo */
	(SPFX_RESTR|SPFX_ONLYONE|SPFX_SPEAK|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("glorkum 3 721 5",				WATER_DRAGON_SCALES, /* 5 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("svetochuvstvitel'nost'",				POT_SPEED, /* quaffing it gives 200 turns of berserk and 2000 turns of thirst nastytrap effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Space Shuttle",				LOW_BOOTS, /* initialized to always be missys, contamination resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Art Rockz",				LOW_BOOTS, /* initialized to always be missys, engravings scuff less quickly */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Cover Up",				LOW_BOOTS, /* initialized to always be missys, displacement */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Chest Tank",				BANDED_MAIL, /* 20 extra points of AC but you take double damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Is Only Owwe",				BANDED_MAIL, /* fast speed, 10% chance to evade missile attacks but 20% less AC */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Suit of Jane Mail",				CHAIN_MAIL, /* jane trap effect and 20% higher caps for nivellation */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Fettis Slot",				SHIELD, /* randart2 shield, you are thick-skinned and +3 CON but don't fit through narrow passages */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Bobais",				STUDDED_LEATHER_ARMOR, /* doesn't hinder monks, jedi and such, 20% chance that your lightsaber doesn't lose energy */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Give It Me Your Will",				IMPLANT_OF_ABRASION, /* randart2 implant, invoke for acquirement but doing so sticky prime curses the implant and if you're not in a form without hands, also gives you a nastytrap effect permanently */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 4000L ),

A("Bulletstopper",				STUDDED_LEATHER_ARMOR, /* 50% chance to deflect bullets */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Ranged Reducer",				STUDDED_LEATHER_ARMOR, /* 33% chance that monsters cannot use missile weapons */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Slow Missiles",				STUDDED_LEATHER_ARMOR, /* monsters cannot use ranged attacks on a distance greater than 6 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Internal View",				STUDDED_LEATHER_ARMOR, /* monsters have 30% less AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Kopkiller",				LONG_SWORD, /* randart melee weapon */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_KOP,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Silencer",				HALBERD,
	(SPFX_RESTR|SPFX_DFLAG2), 0, (M2_FEMALE),
	PHYS(5, 10), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Through The Motions",				LEATHER_ARMOR, /* invoke upgrades its base type */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 2000L ),

A("Cloudyband",				T_SHIRT, /* randart2 shirt, claudia trap effect, disintegration resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Now You Made Her Sad",				T_SHIRT, /* randart2 shirt, victoria trap effect, rays don't lose range when hitting something, your ranged weapons have +2 range */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Ha ha ha ha...",				T_SHIRT, /* randart2 shirt, little marie trap effect, double resistance piercing, clairvoyance, sight bonus, infravision, autocurses, becomes +18 if you put it on while it's lower */
	(SPFX_RESTR|SPFX_EVIL|SPFX_WARN|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Greentop",				T_SHIRT, /* randart2 shirt, elise trap effect, becomes CLR_GREEN when worn, +3 AC for every worn armor piece that is also green (including this one) */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Arabella's Feminizer",				FEMINISM_DRAGON_SCALE_MAIL, /* arabella trap effect, cursed parts, completely bad part, clairvoyance, detect monsters, heavily autocurses, uncurse() fails 50% of the time */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Her Unreachable Brook",				LONG_SWORD, /* randart melee weapon, jette trap effect, antisearching, unwielding it increases jette trap timeout by 1000 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(10, 16), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1600L ),

A("Ignorance Is Bliss",				CHAIN_COIF, /* verena trap effect */
	(SPFX_RESTR|SPFX_EVIL|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Marji Jana",				LOW_BOOTS, /* initialized to always be ankle boots, jana trap effect, flying, unbreathing, getting hit by amnesia has 1% chance to instakill you (no joke), heavily autocurses */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Fight With Platform-Ina",				SINFUL_HEELS, /* bridghitte effect, +10 kick damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("De-Sid",				SITH_STAFF, /* ute trap effect, autocurses, can still use your hands and allows the hunk lightsaber form to work, can dig as fast as a pick-axe, +5 damage when using the force against enemies */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Jonadab's Extra Mode",				LOADBOULDER, /* +10 all stats while carried, trying to pray tells you whether it's safe like gang scholar but always, double extra skill training, trap revealing */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Shiny Brightness",				STARLIGHTSTONE,
	(SPFX_RESTR|SPFX_EVIL), (SPFX_XRAY), 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Gulp-Gulp-Gulp",				POT_FULL_HEALING, /* if it's in your open inventory and you run out of health, it lifesaves you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Reju Gluck Gluck Gluck Bug",				STANDARD_HELMET, /* randart2 helmet, healup doesn't heal you at all if your HP is below one quarter of the max */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Unidentified Hellcast",				POWER_CROSSBOW, /* fires exploding bolts, description doesn't display its stats (intentional) */
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK), 0, 0,
	FIRE(3, 5), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Why Does The Game Generate So Many Crossbows",				QUARTERSTAFF, /* randart quarterstaff, 1 in 50 for a randomly generated weapon to be changed to some sort of crossbow */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Rumple Rumple",				SCR_EARTH, /* creates boulders in a 5x5 area */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Block It Real",				SCR_LOCKOUT, /* every blocked square gets a boulder */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Healaportation",				SCR_TELEPORTATION, /* heals the reader */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Damn Word Play",				POT_EXTRA_HEALING, /* +5 max HP on top of the regular effect but you become cursed and doomed for 12k turns when quaffing it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Giv Smth Gud",				SCR_ALLY, /* activates super aggravation, difficulty trap and others for 1 turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Max's Capsule",				POT_CYANIDE, /* instakills you if you're stupid enough to quaff it, breathing its vapors gives conf, stun and fear */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Design Your Own",				SCR_BLANK_PAPER, /* randart2 scroll, reading it allows you to enter the name of any scroll that you have formally identified and which can be written with the magic marker. if you gave a valid type, you get 5 heavily cursed scrolls of that type and your alla is drained by 5 times the base magic marker cost of that scroll type */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Glen Hospital",				IMPLANT_OF_SERENITY, /* 10% chance to nullify incoming damage, resist fear, and if you're in a form without hands, +10 WIS and wisdom can go up beyond the soft cap without the random chance of failure */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Rivers of Babylon",				IMPLANT_OF_GILGAMESH, /* 10% extra maximum health for newly obtained symbiotes, or 25% if you're in a form without hands */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Clean Ascension Run",				IMPLANT_OF_THE_PLANES, /* if you're in the elemental planes, free action, discount action and sleep resistance, and if you're in a form without hands you have the double version of those properties as well as intrinsic half physical and half spell damage and 10 extra points of AC */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Food For Thought",				IMPLANT_OF_RECKONING, /* increases INT and WIS soft cap by 2, or 5 if you're in a form without hands, and if you're in a form without hands you additionally have +2 INT */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Niobe's Anger",				IMPLANT_OF_HUBRIS, /* eggs cannot hatch tame, divine anger is always at least 3 but if you're in a form without hands, you have +4 increase accuracy and +2 increase damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Foreboding",				IMPLANT_OF_OMEN, /* allows you to listen in on item generation like the listener race, and if you're in a form without hands you also have 1 in 2000 chance per turn to get an enlightenment effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Will's Sacrifice",				IMPLANT_OF_LEGEND, /* if you die and happen to have life saving, acts as a confused scroll of taming; if you're in a form without hands, all frenzied monsters on the current level additionally become un-frenzied */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Dungeon Boss (with sharp ss)",				IMPLANT_OF_VIRTUE, /* +1 all stats, or if you're in a form without hands, +10 STR and +2 all other stats */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Spacel-Swim",				MACE, /* material becomes IRON when equipped and it loses erosionproofing, flying and if you swim in water, the map reveals itself */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Thaiforce",				RED_LIGHTSABER, /* randart lightsaber, use the force, thai trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Henriettenforce",				RED_LIGHTSABER, /* randart2 lightsaber, use the force, 1000 turns of henrietta trap effect, resist sleep and stone, and you get permanent intrinsic aggravate monster */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Susa Mail",				HAWAIIAN_SHORTS, /* susanne trap effect, autocurses, 8 extra points of AC, use the force, 10% chance to nullify incoming damage, monster speed bug */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Artigrade",				POT_GAIN_LEVEL, /* can also upgrade artifacts when dipped */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Add The Missing Staircase",				LEATHER_ARMOR, /* randart2 armor, going up to the castle when coming from the valley creates a downstair; if that has already happened, the armor conveys sight bonus */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Tunguska Event",				SCR_WORLD_FALL, /* causes dynamite explosions that can also blow up walls and generate lava like the ragnarok effect */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Library Hiding",				LEATHER_CLOAK, /* randart cloak, invisibility, this item spawns invisible */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Find Me Nevertheless",				LEATHER_CLOAK, /* randart2 cloak, double invisibility, this item spawns hidden */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Dat's God",				LONG_SWORD, /* randart melee weapon, spawns erodeproof */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Ideal Base",				LOW_BOOTS, /* randart2 boots, spawns with random erosion, invoke to gain a point of enchantment up to a max of +3 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Suk Me Hahahahahah",				STANDARD_HELMET, /* randart helmet, spawns with d3 grease */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Wear Off The Damn RNG",				STANDARD_HELMET, /* randart2 helmet, spawns with 3 grease and can be invoked to gain a layer of grease */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 6000L ),

A("Nothing But Filler",				LEATHER_ARMOR, /* randart2 suit, spawns with random erosion and erosionproofing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Olscratch",				ICKY_BLADE, /* if you get hit with the playerbleed() effect, you bleed for twice as long */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(8, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("St-ickyness",				ICKY_BLADE, /* julietta trap effect, suppresses glib, protects from item-stealers */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ATTK), 0, 0,
	ACID(6, 9), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Help With The Mine",				LEATHER_ARMOR, /* +2 sight range while you're in the gnomish mines and +1 while you're in the deep mines */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Townie Dump",				LEATHER_ARMOR, /* +5 charisma */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Tilling Fields",				LEATHER_ARMOR, /* allows you to walk through farmland */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Sharp Smoky Stench",				STUDDED_LEATHER_ARMOR, /* fire traps reveal themselves with 1 in 500 chance per turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Throw Blostones",				STUDDED_LEATHER_ARMOR, /* shopkeepers spawn with a boulder on their square */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("The Patrol's Orders",				STUDDED_LEATHER_ARMOR, /* you see all 'o' and 'O' */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("Chest in the bandit's lair",				CHAIN_MAIL, /* random class objects have 1 in 40 to become armor, newly spawned chests have d3 additional armor pieces in them */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Oh no I saved, am I nevertheless so stupid!",				CHAIN_MAIL, /* turn counter advances twice as fast, you gain 10% more experience */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Crypt Smell",				BANDED_MAIL, /* undead warning, and if you're in a crypt room you also have scent view */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("A Roomful Will Be A Challenge",				STRIPE_MAIL, /* 1 in 5000 to spawn rn1(9, 9) giants of identical type */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Maps To Play",				SCR_BLANK_PAPER, /* randart2 scroll, if it's a type that can be multiplied via card trick then reading it creates a stack of additional scrolls of the same base type */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Multipot",				POT_WATER, /* randart2 potion, spawns with rn1(6,6) extra stack size */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 800L ),

A("Zarl's Shot",				CROSSBOW,
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Powervalk",				SPEAR, /* very fast speed when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Finally! A usable polearm.",				RANSEUR, /* randart polearm, sight bonus when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Perfect Universe",				RANSEUR, /* monsters with melee weapons have -5 to-hit against you */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Thunderfist",				LUCERN_HAMMER,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(5, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Smile'z",				LOW_BOOTS, /* initialized to always be missys, resistance piercing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Secret Rainbow",				LOW_BOOTS, /* initialized to always be missys, warp reflection */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Yelly",				LOW_BOOTS, /* initialized to always be missys, 50% chance to deflect projectiles */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1170L ),

A("Dat Nigga's Got Da Bling",				LOW_BOOTS, /* randart2 boots, count as columnar heels */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8800L ),

A("Powaspel",				ROBE_OF_SPELL_BOOSTING, /* +4 spell power on top of existing effect, -3 increase accuracy and damage, -d2 multishot with ranged weapons */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Total Parry Gauche",				MAGISWORD, /* if it's in the off-hand, double mystery res and 10 extra points of AC as well as 40% chance to block */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Fastfuck",				AMULET_OF_FUCKING, /* fast speed */
	(SPFX_RESTR|SPFX_SEEK|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("The Fortune Is Good",				GAUNTLETS_OF_FORTUNE,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Like Grace",				GAUNTLETS_OF_MYSTERY_RESISTANC, /* +5 STR and DEX */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Really Find Em",				CLOAK_OF_MAGIC_FIND, /* 1 in 2000 while worn to get an effect that mimics uncursed potion of object detection */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Super Combo!",				COMBINATION_CLOAK, /* psi res */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Winter Age",				CLOAK_OF_LOWER_MYSTERY,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Identification Help",				CLOAK_OF_MYSTERY, /* scroll of identify identifies one more item (unless it decides to identify the entire inventory) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Dang Armor",				CLOAK_OF_ARMOR_MYSTERY, /* 3 extra points of AC and one more for every positive enchantment, material becomes TITANIUM when putting it on */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Blachwartz",				RIN_MYSTERY_RESISTANCE, /* resist cold and fire */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("RNG Cessation",				MYSTERY_DRAGON_SCALES, /* displays all monsters with AD_RBRE or AD_RNG */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Whackabit",				MYSTERIOUS_PICK,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Frenchyposs",				MYSTERIOUS_BOOTS, /* +2 damage with axes and one more per enchantment level of the boots; if they are blessed, +1 multishot with all firearms */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Tschingfirm",				MYSTERY_SHIELD, /* 10% extra chance to block */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4500L ),

A("Dimethyl Escutcheon",				CASTSHIRT, /* poison resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Hogwild Wiz",				GAUNTLETS_OF_SPELL_POWER, /* 10% extra spellcasting chance and 5% extra like failuncap */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Finetuning",				AMULET_OF_SPELLCASTING, /* food appraisal */
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Heliopolis Mistake",				CASTER_STAFF, /* killer spawn trap effect for 5000 turns, clairvoyance, scent view, echolocation, red spells */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ATTK), 0, 0,
	ELEC(8, 12), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 7000L ),

A("SaatusPlease",				IMPLANT_OF_FATE, /* invoking curses the implant unless you're in a form without hands; if you are in such a form, it also grants peacevision while worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, ENLIGHTENING, A_NONE, NON_PM, NON_PM, 4000L ),

A("Clear Catastrophe",				IMPLANT_OF_DISASTER, /* nastiness, and friday 13th effect unless you're in a form without hands */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("Yah, Yah",				IMPLANT_OF_REDRESS, /* +1 AC for every worn armor piece that is colored red; if you're in a form without hands, putting a red-colored armor piece on will remove one point of negative enchantment from it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Biuuu!",				IMPLANT_OF_QUARRELS, /* +1 multishot with crossbows, and if you're in a form without hands also +1 multishot with bows */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("They're Really After Me",				IMPLANT_OF_PARANOIA, /* trap of walls effect, scent view if in a form without hands */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Throw My Life Away",				IMPLANT_OF_SWIFT_DEFEAT, /* premature death, but lifesaves you if in a form without hands; double attacks */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Stalwart Omm",				IMPLANT_OF_INTEGRITY, /* disint res, and if you're in a form without hands there's 1% chance per turn that wince, invert, stun, burn or freeze effects end */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Do Not Want Sex",				IMPLANT_OF_CHASTITY, /* prevents AD_SSEX, and diminished bleeding if in a form without hands */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Dear God, Help Me!",				IMPLANT_OF_MOLLIFICATION, /* 1 in 15000 per turn that divine anger times out by one, or 1 in 5000 if in a form without hands */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Dingdoeng!",				IMPLANT_OF_FULL_HEALING, /* can be invoked to heal you back to full HP but 1 in 5 chance that the artifact becomes brittle; if it's already brittle and you're not in a form without hands, it doesn't actually restore your HP */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 5000L ),

A("I'm Gonna Crush Ya!",				IMPLANT_OF_DESTRUCTION, /* destruction effect, +4 increase accuracy, and +4 increase damage if in a form without hands */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Friend Of All The Animals",				IMPLANT_OF_FRIENDSHIP, /* all "d" and "f" have 50% chance of spawning peaceful; if in a form without hands, additionally all M1_ANIMAL have 50% chance of spawning peaceful and 2% of peaceful ones spawn tame */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Enterogaster Alto",				IMPLANT_OF_SATIATION, /* full nutrients, and slow digestion if in a form without hands */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Higherholder",				IMPLANT_OF_PRESERVATION, /* sustain ability, and fully prevents stat loss if in a form without hands */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Real Time Switching",				IMPLANT_OF_QUICK_DRAWING, /* fast speed, and if you're in a form without hands, wielding a weapon doesn't use a turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Charge-O-Mania",				IMPLANT_OF_WAND_CHARGING, /* charging effect is blessed if in a form without hands */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, CHARGE_OBJ, A_NONE, NON_PM, NON_PM, 10000L ),

A("SarreFree",				IMPLANT_OF_CLEAR_THINKING, /* resist confusion, and if in a form without hands, keen memory */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Tschhkrzkrz",				IMPLANT_OF_MIND_BLASTING, /* you can #monster to emit mind blasts; if in a form without hands, they deal more damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Nyehehehehe.",				AMULET_OF_MONSTER_SAVING, /* if it lifesaves a hostile monster, it's not used up */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Lightbloom",				FLEURE, /* resist light, 1 in 100 to blast a strength 3 solar beam in melee */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Roofridge Class",				FLEURE,
	(SPFX_RESTR), 0, 0,
	PHYS(10, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4500L ),

A("Don't Die While In There",				OVER_SHOES, /* lifesaves you while you're in a potentially restricted area as long as you didn't run out of health, but uses up the boots in the process */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Fourth Chance",				AMULET_OF_THIRD_CHANCE, /* if it lifesaves you, its brittleness increases and only if the brittleness becomes too high it's used up, otherwise you get to keep it so it can lifesave you again */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Find The Combat Stance",				TANJA_SNEAKERS, /* 20% chance to sidestep a projectile, +10 kick damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Sister's Acid",				SONJA_SNEAKERS, /* d4 passive acid damage to monsters attacking you in melee */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Doubtly Poison",				RHEA_LADY_PUMPS, /* if your wielded weapon can be superpoisoned, it is; if you fire a projectile that can be superpoisoned, it is; can be invoked to fire a poison blast with strength 10 in a direction of your choice */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NEUTRAL, NON_PM, NON_PM, 18000L ),

A("Play The Game Yourself",				LARA_COMBAT_BOOTS, /* henrietta trap effect, resist mystery and disintegration, can lifesave you */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 10000L ),

A("Katharina's Sigh",				LOW_BOOTS, /* initialized to always be white buffalo boots, katharina trap effect, immunity to MS_SOCKS and 90% chance to resist perfume */
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 16000L ),

A("Sassy Julia",				LOW_BOOTS, /* initialized to always be sister shoes, julia trap effect, mystery and curse resistance */
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Pocahontas Home",				LOW_BOOTS, /* initialized to always be sister shoes, laura trap effect, acid immunity */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("They All Feel Fleecy",				T_SHIRT, /* randart shirt, free action, tanja trap effect */
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Castle Crush Glitch",				SHIELD, /* randart2 shield, lara trap effect, heavily sticky autocurses, 1 in 2000 to be polyformed and the shield doesn't drop when you poly */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Tillmann's Target",				T_SHIRT, /* randart shirt, sonja trap effect, -5 str and con, fear, life saving, heavily autocurses, invoke for create familiar */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Highway Hunter",				LOW_BOOTS, /* randart boots, rhea trap and killer spawn effect, transforms floor underneath you into highway, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Highway Fighter",				LOW_BOOTS, /* randart2 boots, rhea trap effect, transforms floor underneath you into highway, autocurses, penalizes you for saving the game */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Atar-Atar",				GREEN_LIGHTSABER, /* can be invoked exactly once to spawn a second copy of this artifact, d(2*ataru skill) extra damage, and if you're dual-wielding two of them they drain power at half the normal rate */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Dablagra Wenn",				BLUE_LIGHTSABER, /* 10% chance per niman skill to regenerate an additional Pw per turn, but only if the lightsaber is on */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Flygel Boots",				LOW_BOOTS, /* randart boots, flying */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("rule-conforming Schwanzlut",				STANDARD_HELMET, /* randart helmet, 3 axtra points of AC but equipment can't spawn +1 or higher */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Ammy's Complaint",				CHAIN_MAIL, /* -5 luck, autocurses */
	(SPFX_RESTR|SPFX_EVIL|SPFX_DEFN|SPFX_LUCK), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Ammy's Band",				BANDED_MAIL, /* conny trap effect, autocurse, makes AC worse by 33% */
	(SPFX_RESTR|SPFX_EVIL|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Ammy's Retribution",				PLATE_MAIL, /* doesn't hinder monks, jedi and such, autocurses, randomly generated items cannot be artifacts */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Ammy's RNG Changer",				LEATHER_ARMOR, /* randart2 armor, x-race effect on the rng, autocurses, all traps have the same spawn freq */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Ammy's Easymode",				STUDDED_LEATHER_ARMOR, /* level difficulty reduced by 1 (not below 1), turn limitation */
	(SPFX_RESTR|SPFX_EVIL|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Ammy's Wail",				LEATHER_ARMOR, /* randart armor, wince */
	(SPFX_RESTR|SPFX_EVIL|SPFX_DEFN|SPFX_ESP), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("First Pair",				LADY_BOOTS, /* diminished bleeding */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_TRANSVESTITE, NON_PM, 8000L ),

A("Soft on the Cloud",				LOW_BOOTS, /* initialized to always be sister shoes */
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_TRANSVESTITE, NON_PM, 4000L ),

A("Sharp-Edged And Dangerous",				LOW_BOOTS, /* initialized to always be weapon light boots, +10 bleeding damage when kicking and only 1 in X chance to increase sin counter */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_TRANSVESTITE, NON_PM, 4000L ),

A("Cannot Walk With The Arianen",				FEMININE_PUMPS, /* slows you down like spirit, high heels skill trains 3x as fast, technicality, ariane trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_TRANSSYLVANIAN, NON_PM, 2500L ),

A("Thick Platform Craze",				LOW_BOOTS, /* initialized to always be white buffalo boots; anastasia trap effect, +5 kick damage, +2 ac, prevents you from being very fast, intelligent female monsters can dim you */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_TRANSSYLVANIAN, NON_PM, 10000L ),

A("Dieamend",				AMULET_OF_THIRD_CHANCE, /* +2 CHA */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_ERDRICK, NON_PM, 5000L ),

A("NOOBNOOBNOOB",				CHARLOTTE_S_JEWEL, /* if this item is generated as a noob mode barb, you get the charlotte trap effect permanently */
	(SPFX_RESTR|SPFX_EVIL), (SPFX_DEFN), 0,
	NO_ATTK, NO_DFNS, CARY(AD_MAGM), 0, A_NONE, PM_NOOB_MODE_BARB, NON_PM, 1500L ),

A("Fliumill",				WINDMILL_BLADE, /* dmg h@ck because programmers are so ultra stupid and don't just make a SPFX_DBLDAMAGE flag, spawns with double stack size and can be gifted again even after it has already been generated */
	(SPFX_RESTR|SPFX_GIFTAGAIN), 0, 0,
	PHYS(12, 0), NO_DFNS, NO_CARY, 0, A_NONE, PM_NINJA, NON_PM, 100L ),

A("Baeff",				CESTUS, /* +8 unarmed damage (requires dual-wielding with one hand empty if that wasn't obvious) even if this arti is in the off-hand, also +8 strength and dmgval +4 and another +6 against that which the base damage value is low against */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_ASSASSIN, NON_PM, 6000L ),

A("Time For A Call",				SHOVEL, /* clairvoyance, +6 bash damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_UNDERTAKER, NON_PM, 4000L ),

A("Dontplode",				WAN_ACID, /* doesn't blow up when charging or zapping while it's cursed etc. */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_ACID_MAGE, NON_PM, 3000L ),

A("Break Everything",				BATTLE_AXE, /* if you're a firefighter, it can be used to raze iron bars, mountains, farmland and more, and it won't damage the axe */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(0, 8), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, PM_FIREFIGHTER, NON_PM, 5000L ),

A("Tada",				FIRE_HORN, /* starts with +10 charges and gains double amount of additional charges when charged */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_FLAME_MAGE, NON_PM, 2500L ),

A("Fergie's Glacier",				FREEZING_BOOTS, /* suppresses freeze status effect, levitation if your weapon is a quarterstaff-type, heavily autocurses, you cannot have resist stun or conf */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_ICE_MAGE, NON_PM, 8000L ),

A("Lessee Dat",				SPE_THUNDER_WAVE, /* reading it successfully as an electric mage has chance like throne skill improvement to improve thunder wave skill */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_ELECTRIC_MAGE, NON_PM, 2000L ),

A("Multiply Me",				SPE_BLINDING_RAY, /* always a random type of occult spellbook, 5x spell memory if read by an occult master or 2x otherwise */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_OCCULT_MASTER, NON_PM, 3000L ),

A("Thats Enough Right There",				SHIELD, /* randart2 shield, heavily autocurses, fire immunity */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_CHAOS_SORCEROR, NON_PM, 12000L ),

A("Fultra",				DEATH_HORN,
	(SPFX_RESTR|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_CHAOS_SORCEROR, NON_PM, 1000L ),

A("Spellsnipe",				REGULAR_GLOVES, /* randart gloves, infravision, resistance piercing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_ELEMENTALIST, NON_PM, 3000L ),

A("Super Peep Toes",				LEATHER_PEEP_TOES, /* resist death and mystery */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_TOPMODEL, NON_PM, 4000L ),

A("Jump-Hurries",				CLIMBING_STICK, /* jumping, extra speed like real speed devil */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_TOPMODEL, NON_PM, 5000L ),

A("Wild Oar",				BOAT_OAR, /* can be used as a polearm */
	(SPFX_RESTR|SPFX_DFLAG1), 0, M1_ANIMAL,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, PM_TOPMODEL, NON_PM, 6000L ),

A("Tripping",				HENRIETTA_S_JEWEL, /* carrying it causes randomized gold spawns to have 50% more gold in them */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_FAILED_EXISTENCE, NON_PM, 1000L ),

A("All Is Lost",				ITEM_TELEPORTING_STONE, /* resist death and hallu when carried */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_FAILED_EXISTENCE, NON_PM, 500L ),

A("Know of the Curse",				HANDYBAG, /* waterproof, always costs a turn when used, if it's more than 50 turns since the last use it also creates a trap on a random square */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_FAILED_EXISTENCE, NON_PM, 100L ),

A("Venoreal",				ACID_VENOM, /* poisons targets, if it's generated when player is a poison mage it has 3x stack size and then 25 more on top of that */
	(SPFX_RESTR), 0, 0,
	PHYS(15, 12), NO_DFNS, NO_CARY, 0, A_NONE, PM_POISON_MAGE, NON_PM, 25L ),

A("Pfie Pfiepfie",				MAGIC_FLUTE, /* using it reveals its charges, doesn't explode */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_MUSICIAN, NON_PM, 3000L ),

A("Excalident",				TRIDENT,
	(SPFX_RESTR|SPFX_WARN|SPFX_SEEK|SPFX_INTEL|SPFX_SEARCH), 0, 0,
	PHYS(5,10), NO_DFNS, NO_CARY, 0, A_NONE, PM_CHEVALIER, NON_PM, 4000L ),

A("Winterhold's Keep",				LONG_SWORD, /* randart melee weapon, cold immunity */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_WARRIOR, NON_PM, 7000L ),

A("Drive-By",				SUBMACHINE_GUN, /* +2 multishot when riding and another +1 when the steed is gallopping */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_GANGSTER, NON_PM, 3000L ),

A("yes,occifer",				IRON_CHAIN, /* randart iron chain, sic, double free action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_OFFICER, NON_PM, 2000L ),

A("Sunset Sasaparilla",				POT_BOOZE, /* +rn1(10,10) stack size, deactivates speed for 1000 turns when quaffed, heals you fully and +200 nutrition, can be gifted more than once */
	(SPFX_RESTR|SPFX_GIFTAGAIN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_COURIER, NON_PM, 50L ),

A("Benny's Chip",				DATA_CHIP, /* invoke for alter reality but doing so deletes all spells, -20 max HP and -1 INT permanently, and the invoke only works for a courier */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, PM_COURIER, NON_PM, 8000L ),

A("Death Avert",				SCR_RESURRECTION, /* no specialties */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_INTEL_SCRIBE, NON_PM, 10000L ),

A("Your Lucky Day",				WAN_NOTHING, /* randart2 wand; as a wandkeeper, spawns with 50 extra charges, or 3 if the wand can only be recharged once; other roles get 20 or 1 extra charges, respectively */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_WANDKEEPER, NON_PM, 4000L ),

A("Charging Made Easy",				RED_LIGHTSABER, /* randart lightsaber, invoke gives 500 extra charge but also rnz(5000) vulnerability, also wielding it gives -5 increase accuracy and damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, PM_ZYBORG, NON_PM, 4000L ),

A("Cast AK",				WAN_DEATH, /* doesn't explode, every zap gives -100 alignment, +10 sins and +1 divine anger */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_DEATH_EATER, NON_PM, 2500L ),

A("Heeeeeleeeeen",				VICTORIAN_UNDERWEAR, /* displays all pets and they cannot rebel and will follow you to other levels regardless of distance, sticky autocurse, non-activistors also have jette effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_ACTIVISTOR, NON_PM, 1000L ),

A("Mine Out",				DWARVISH_MATTOCK, /* faster digging, sight bonus */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_MIDGET, NON_PM, 4000L ),

A("Cerny!",				DWARVISH_MITHRIL_COAT, /* +50% hp cap for corona, +5 softcap for con, maybegainstat con 3x as likely to work */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_MIDGET, NON_PM, 2000L ),

A("Psi Staff",				QUARTERSTAFF, /* randart quarterstaff, resist psi */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	STUN(4, 2), NO_DFNS, NO_CARY, 0, A_NONE, PM_PSION, NON_PM, 5000L ),

A("Elena's Epitome",				IRINA_WEDGE_SANDALS, /* trap creation, stone res, fear factor, 10% chance to nullify incoming damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_SHOE_FETISHIST, NON_PM, 8000L ),

A("Elena's Competition",				LOW_BOOTS, /* initialized to always be block-heeled boots, 1 in 100 when you get hit in melee to become invulnerable for 2-5 turns if you aren't already */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_SHOE_FETISHIST, NON_PM, 2000L ),

A("Elena's Reward",				ELENA_COMBAT_BOOTS, /* +50 spawnfreq for MS_FART_NORMAL and if they fart, you gain 5 alla and sanity, plus 1 in 20 to be paralyzed for 20 turns; if the quest isn't complete, you are backtostarted, otherwise the monster becomes peaceful. if the quest is complete, chatting to MS_FART_NORMAL pacifies them too */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_SHOE_FETISHIST, NON_PM, 6000L ),

A("Unbindall Cheat",				LEATHER_CLOAK, /* randart2 cloak, invoke to set training in all restricted skills to zero */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, PM_ANACHRONOUNBINDER, NON_PM, 1000L ),

A("Preim-oh",				LEATHER_CLOAK, /* randart cloak, shows skill training in enhance screen */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_BINDER, NON_PM, 8000L ),

A("Repair Your Brain",				PICK_AXE, /* randart2 pick-axe, invoke for restore intelligence but doing so curses the pick and reduces enchantment by d5; if already -20 or worse, the invoke doesn't work */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Britta's Murder Story",				LOW_BOOTS, /* initialized to always be sling pumps, power failure, radar not working, resist fear, +7 INT, +5 WIS, stat exercise happens 3x faster, maybegainstat happens 5x as often, 1 in 1000 that they re-curse themselves while worn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Superdragonbane",			BROADSWORD, /* randart2 broadsword */
	(SPFX_RESTR|SPFX_DCLAS|SPFX_REFLECT), 0, S_DRAGON,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 4500L ),

A("Giantcrusher",                LONG_SWORD, /* randart2 long sword, +5 STR */
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_GIANT,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Ogre Power",		WAR_HAMMER, /* randart2 hammer, +3 CON */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_OGRE,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Haxortroll",                 MORNING_STAR, /* randart2 morning star */
	(SPFX_RESTR|SPFX_DCLAS|SPFX_REGEN), 0, S_TROLL,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 2000L ),

A("That Gives It Not",				FEDORA, /* displays all covetous monsters */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Notknow Ey",				FEDORA, /* prevents mind flayer blasts from affecting you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Xtraluck",				FEDORA, /* +3 luck */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Third Car",				FEDORA, /* +5 damage for whips, +10 digging effort with digging tools */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_ARCHEOLOGIST, NON_PM, 1000L ),

A("Dan The Mage",				CORNUTHAUM, /* spellboost, resistance piercing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("All The Better Spelling",				CORNUTHAUM, /* 5% casting chance and 5% failuncap-like */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Sudunsel",				DUNCE_CAP, /* low local memory, +2 increase damage and accuracy, timebasedlowerchance always returns TRUE, heavily autocurses, monsters delete their money upon death */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Werkauf",				DUNCE_CAP, /* lag trap effect, +20 spawn chance for team Z, monsters spawn with musable items more often */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Shit Happenz",				GNOMISH_SUIT, /* triggering a heap of shit that you didn't trigger yet gives +1 alla */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Beware of the Pitfall",				GNOMISH_BOOTS, /* you don't fall into pits */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Worth Checking Out",				GNOMISH_HELM, /* if it would have generated as +0, it gets a randomized enchantment value */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Idea in Headback",				GAUNTLETS_OF_RAINBOW_MOUD, /* resist mystery and death */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Signum Argentum Polishium",				SILVER_SHIELD, /* double reflection */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Light: 0 (stated)",				RED_LIGHTSABER, /* randart lightsaber, blinking speed, causes unlight every turn and also gives nolite, de-light and dark mode */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Owener",				RED_LIGHTSABER, /* randart lightsaber, starts with charge of 0 but 50% chance to not use up power */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Diplite",				BLACK_LIGHTSABER, /* while wielding it, if it's not on, 1 in 10 chance that the charge goes up by one, but this heavily curses the saber if it isn't already */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Cure for Anorexia",				POT_GREEN_TEA, /* cures ina and nora trap effects when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Arabella's Rectifier",				IMPLANT_OF_THE_SEA_LION, /* doubles MC after all bonuses have been calculated, permacurse trap and curse items trap effects, if you're in a form without hands there's 1 in 1000 chance per turn to gain +1 max alignment */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 10000L ),

A("Canc...",				IMPLANT_OF_TORMENT, /* +1 MC, and if you're in a form without hands your items are 10x as likely to resist accidental cancellation effects */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Doublebind",				IMPLANT_OF_BINDING, /* your MC is at least twice that of the base item; if you're in a form without hands, the enhance screen displays skill training amounts */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Almost Tangible",				IMPLANT_OF_THE_OCTOPUS, /* your MC doesn't fall below that of the base item; if you're in a form without hands, you can eat all rings */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Octopoddy",				IMPLANT_OF_THE_OCTOPUS, /* invoke for bad equipment; if you're in a form without hands, the generated item doesn't always have a negative enchantment value and will spawn with its + known */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 4000L ),

A("Shattering Elixir",				POT_PORTER, /* randomly branchports you when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("gendergendergender",				LONG_SWORD, /* randart2 melee weapon, no specialties */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_GENDERSTARIST, NON_PM, 1000L ),

A("Ars Technica",				MAGICAL_PAINTBRUSH, /* if you're an artist and equip it for the first time, your paddle skill cap increases by one level */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(0, 10), DFNS(AD_MAGM), NO_CARY, 0, A_NONE, PM_ARTIST, NON_PM, 5000L ),

A("May-Britt's Click",				MAY_BRITT_S_JEWEL, /* displays all joke monsters when carried */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_GANG_SCHOLAR, NON_PM, 2000L ),

A("Moist Cherry",				CHERRY, /* invoke to cure bleeding, but if you're not a bleeder, your sanity and contamination are increased by the amount of bleeding damage that was healed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, PM_BLEEDER, NON_PM, 3000L ),

A("ei ken wook wif dis",				LOW_BOOTS, /* initialized to always be stupid stilettos, fast speed, you don't have the "stupid stiletto" stumbling effect */
	(SPFX_RESTR), 0, 0, /* not SPFX_EVIL - the bad effect is suppressed */
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_GOFF, NON_PM, 4000L ),

A("Efgourd",				PLASTEEL_HELM, /* confusion resistance; if you're a drunk, it doesn't prevent quaffing */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_DRUNK, NON_PM, 6000L ),

A("Womanbeater",				RIDING_CROP,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_FEMALE,
	PHYS(0, 15), NO_DFNS, NO_CARY, 0, A_NONE, PM_CRUEL_ABUSER, NON_PM, 2000L ),

A("Bang The Head Away",				FLINTLOCK, /* ammos can behead enemies */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_SLAVE_MASTER, NON_PM, 8000L ),

A("For Stygwyr!",				RED_LIGHTSABER, /* randart lightsaber, gives additional HP when you kill something */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_BLOODSEEKER, NON_PM, 5000L ),

A("Geb Me All Your Money",				DAGGER, /* randart2 dagger, hitting something in melee causes it to flee unless it resists */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, PM_BULLY, NON_PM, 2500L ),

A("Pourri(R)",				DENTED_POT, /* full nutrients */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_COOK, NON_PM, 1500L ),

A("Magicrystal",				CHALCEDON, /* when carried, 5% better spellcasting chance and 5% like failuncap but also meltem trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_AUGURER, NON_PM, 8000L ),

A("Smashin",				MACE, /* randart mace, superaggravatemonster, autocurses. if you're not a sage, it heavily sticky autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, PM_SAGE, NON_PM, 6000L ),

A("Haha Owned",				OSBANE_KATANA, /* bisects enemies, but every time it does, the sword is disenchanted up to -20 */
	(SPFX_RESTR|SPFX_BEHEAD|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_OTAKU, NON_PM, 4000L ),

A("Up-Down Stab",				ELVEN_SHORT_SWORD, /* if the opponent is either on a table or a non-crystal water tile, a flat +20 damage is added */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, PM_GAMER, NON_PM, 1400L ),

A("Y-Sword",				PICK_AXE, /* randart pick-axe, checks your highest sword-type skill and gives +3 damage per skill level in that */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_SAIYAN, NON_PM, 8000L ),

A("tschueuu",				CROSSBOW, /* randart crossbow, dmg h@ck, +10 range for bolts, cannot have multishot and if you fire, you gain 6 movement energy */
	(SPFX_RESTR), 0, 0,
	PHYS(15, 0), NO_DFNS, NO_CARY, 0, A_NONE, PM_GOLDMINER, NON_PM, 3000L ),

A("Back Kick",				FEMININE_PUMPS, /* if you move, and there was a monster "behind" you, the pumps kick that monster automatically */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_POLITICIAN, NON_PM, 6000L ),

A("World Class Jump",				JUMPING_BOOTS, /* reduces jumping mana cost to 2, or 8 if you're not a feat master */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_FEAT_MASTER, NON_PM, 10000L ),

A("Knife'Em",				KNIFE, /* randart knife */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, PM_GUNNER, NON_PM, 3000L ),

A("jumping flaming he tie-clip",				JUMPING_FLAMER, /* spawns with rnz(50) extra ammo */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_DOOM_MARINE, NON_PM, 5000L ),

A("Superstab",				QATAR, /* randart2 claw, double stealth, allows backstabbing */
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_FOXHOUND_AGENT, NON_PM, 3000L ),

A("First Exchange",				RIN_GAIN_INTELLIGENCE, /* +5 INT, -5 STR */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_MAHOU_SHOUJO, NON_PM, 3000L ),

A("Second Exchange",				RIN_GAIN_DEXTERITY, /* +5 DEX, -5 CHA, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_MAHOU_SHOUJO, NON_PM, 3000L ),

A("fling in the dark",				SLING, /* infravision when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 2), NO_DFNS, NO_CARY, 0, A_NONE, PM_RINGSEEKER, NON_PM, 1000L ),

A("Air-On-Hold",				HELM_OF_UNDERWATER_ACTION, /* items don't get wet, prevents water turbulence */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_DIVER, NON_PM, 6000L ),

A("Britney's Deception",				BRONZE_PLATE_MAIL, /* 6 extra points of AC, d(gushlevel) thorns damage in melee */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_DRUID, NON_PM, 8000L ),

A("The Great Matron",				PIKE, /* if you're not an amazon, autocurse and weak sight; if you're not female, 2000 turns of natalia trap effect and 5000 turns of arabella trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, PM_AMAZON, NON_PM, 6000L ),

A("Slam!",				HEAVY_HAMMER, /* resist sleep, all worn armor pieces are 90% resistant to erosion, withering and disenchantment */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, PM_ORDINATOR, NON_PM, 10000L ),

A("Protection Set",				SHIELD, /* randart2 shield, double half physical damage */
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_THALMOR, NON_PM, 8000L ),

A("Alternate Spellweave",				SHIELD, /* randart2 shield, double half spell damage, prism reflection, bad alignment */
	(SPFX_RESTR|SPFX_HSPDAM|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_THALMOR, NON_PM, 10000L ),

A("Complete Sight",				STANDARD_HELMET, /* randart helmet, clairvoyance, scent view, echolocation, and if you're not a medium, the game lags */
	(SPFX_RESTR|SPFX_WARN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_MEDIUM, NON_PM, 5000L ),

A("MELEE DAMMIT",				SEXY_LEATHER_PUMP, /* death resistance, if you're wearing noncursed shoes they automatically take themselves off and if you're wearing cursed ones, they become -10 if they were higher */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(6, 8), NO_DFNS, NO_CARY, 0, A_NONE, PM_SEXYMATE, NON_PM, 4000L ),

A("Astra Mail",				DIAMOND_DRAGON_SCALE_MAIL, /* flying, heavy sticky autocurse, difficulty trap effect, aggravation trap effect, corona cap for HP is divided by 3 */
	(SPFX_RESTR|SPFX_EVIL|SPFX_XRAY), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_FIGHTER, NON_PM, 6500L ),

A("Eastern Clash",				SCIMITAR, /* fast speed */
	(SPFX_RESTR), 0, 0,
	PHYS(7, 5), NO_DFNS, NO_CARY, 0, A_NONE, PM_FENCER, NON_PM, 3000L ),

A("Goosch Hidari",				PARRY_SWORD, /* if it's your secondary weapon when dual-wielding, it grants free action and discount action */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, PM_FENCER, NON_PM, 3000L ),

A("dacha-dacha-dacha",				FORCE_PIKE, /* 10% extra chance to land a critical hit, hitting something increases a combo meter that is added to the damage done but not hitting anything sets the combo meter to 0 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_ANACHRONIST, NON_PM, 1800L ),

A("Excalishield",				WEAPON_SIGN, /* also gives drain res when wielded, +6 bashing damage and if you're using shield bash it also deals +6 damage */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, PM_PALADIN, NON_PM, 6000L ),

A("Dessesga",				WHITE_DOUBLE_LIGHTSABER, /* keen memory */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 10), NO_DFNS, NO_CARY, 0, A_NONE, PM_HEDDERJEDI, NON_PM, 8000L ),

A("Blag-Lite",				BLACK_LIGHTSABER, /* 50% chance to not use up power, de-light trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(7, 8), NO_DFNS, NO_CARY, 0, A_NONE, PM_SHADOW_JEDI, NON_PM, 7000L ),

A("Larissa's Laughter",				BLOCK_HEELED_COMBAT_BOOT, /* larissa trap effect, if you're not a hussy you also get the larissa trap effect for 5000 turns. 1 in 1000 per turn to train squeaking by one point, can be invoked to change material to MT_INKA */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, PM_HUSSY, NON_PM, 8000L ),

A("Bombersnee",				GRENADE_LAUNCHER,
	(SPFX_RESTR), 0, 0,
	PHYS(0,8), NO_DFNS, NO_CARY, 0, A_LAWFUL, PM_GRENADONIN, NON_PM, 1200L ),

A("Go Up And Soar",				ROCKET_GAUNTLETS, /* flying, very fast speed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_ASTRONAUT, NON_PM, 18000L ),

A("Hearing Ear",				BUNNY_EAR, /* scent view, echolocation, covetous warning */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_DISSIDENT, NON_PM, 5000L ),

A("Pectoral Heel",				LOW_BOOTS, /* initialized to always be fin boots; counts as stiletto heels, swimming, and if you're in water you're very fast with extra speed. displays semicolons which aren't more than distu(100) away from you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_FJORDE, NON_PM, 7000L ),

A("Slam Shut And Be Done",				WAR_HAMMER, /* randart2 hammer */
	(SPFX_RESTR|SPFX_DFLAG2), 0, (M2_LORD|M2_PRINCE),
	PHYS(0, 25), NO_DFNS, NO_CARY, 0, A_NONE, PM_PRACTICANT, NON_PM, 6000L ),

A("Needlessly Mean",				BULLWHIP, /* randart2 whip, +4 strength */
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_WARN), 0, M2_FEMALE,
	PHYS(6, 14), NO_DFNS, NO_CARY, 0, A_NONE, PM_EMERA, NON_PM, 2000L ),

A("Regrown Resource",				JAVELIN, /* randart javelin, spawns erodeproof and with +9 stack size */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_TOSSER, NON_PM, 500L ),

A("Baeau",				BLOW_AKLYS, /* spawns with +d10 enchantment */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_AKLYST, NON_PM, 3000L ),

A("Lucky Pack",				LUCKSTONE, /* while carried, your symbiotes have 5% extra chance to attack both in melee and passively */
	(SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_SYMBIANT, NON_PM, 3000L ),

A("Hammer-Good Helm",				HELM_OF_STEEL, /* 10 extra AC if you're a combatant, 5 otherwise */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_COMBATANT, NON_PM, 3000L ),

A("Miracle-Beautiful Noises",				KATHARINA_S_JEWEL, /* 1 in 200 per turn to reduce sanity by one when carried */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_BUTT_LOVER, NON_PM, 6000L ),

A("Hardcore Addiction",				MELTEM_S_JEWEL, /* 90% resistance against MS_CONVERT and 75% against MS_WOUWOU */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_BUTT_LOVER, NON_PM, 4000L ),

A("Slam-Into",				FRIEDERIKE_DANCING_SHOES, /* if you kick a male monster, you deal +8 damage and the monster has to make a saving throw, otherwise it is slowed, stunned, feared and heal blocked and 1 in 10 cancelled */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_DANCER, NON_PM, 3000L ),

A("Garbage Staff",				QUARTERSTAFF, /* randart quarterstaff, +3 CON and INT */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(0, 6), DFNS(AD_DRST), NO_CARY, 0, A_NONE, PM_DIABLIST, NON_PM, 2500L ),

A("Khalim's Femur",				FLAIL, /* randart2 melee weapon, can leech life or mana like an orb */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(8, 12), NO_DFNS, NO_CARY, 0, A_NONE, PM_DIABLIST, NON_PM, 4000L ),

A("Browning",				HEAVY_MACHINE_GUN, /* +d5 multishot, dmg h@ck */
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	PHYS(4, 0), NO_DFNS, NO_CARY, 0, A_NONE, PM_SECRET_ADVICE_MEMBER, NON_PM, 6000L ),

A("Menstruation Hurts",				T_SHIRT, /* randart2 shirt, natalia trap effect, +6 all stats, prime curses itself and if you're not a climacterial it also gives the permacurse trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_CLIMACTERIAL, NON_PM, 6666L ),

A("Give Us Today Our Daily Game Of Elona",				LEATHER_CLOAK, /* initialized to always be bamboo cloak, 1 in 2000 to have an elona tension spawn somewhere on the current level */
	(SPFX_RESTR|SPFX_EREGEN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Pewwwwwww",				JAVELIN, /* randart2 javelin, +15 range, +rn1(9,9) stack size */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Special Robung",				ROBE, /* randart2 robe, intrinsic control magic and double spellboost */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Eternal Bammeling",				ROBE, /* randart robe, spellboost, prayer timeout decreases twice as fast */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Mysteresis X",				QUARTERSTAFF, /* mystery resist */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Hysteresis X",				QUARTERSTAFF, /* randart quarterstaff, mystery resist */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("lyst-erg",				QUARTERSTAFF, /* randart2 quarterstaff, mystery resist */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Ooooooh",				STUDDED_LEATHER_ARMOR, /* mystery resist, 5 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("That's Armor",				CLOAK_OF_ARMOR_MYSTERY, /* 5 extra points of AC and one more per body armor skill level */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4500L ),

A("Just To Have It",				CLOAK_OF_LOWER_MYSTERY, /* +1 STR and DEX */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Absolute Antimagic",				CLOAK_OF_MYSTERY,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("rand-arm",				LEATHER_ARMOR, /* randart armor, mystery resist */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("especiallyrandarm",				LEATHER_ARMOR, /* randart2 armor, mystery resist */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Heart Infact",				LEATHER_ARMOR, /* randart armor, fainting, sleep resistance, spellboost */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Chip Infact",				LEATHER_ARMOR, /* randart2 armor, fainting, sleep resistance, spellboost, technicality, hyperbluewalls, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 600L ),

A("Sama-kata",				KATANA, /* mystery resist, spellboost */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Ferrum D",				DAGGER, /* randart2 dagger, mystery resist, allows you to eat metal when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(8, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Schwi Schwi",				EAGLE_BALL, /* randart2 orb, spellboost, prism reflection */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_ATTK), 0, 0,
	FIRE(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("The Shroud",				LEATHER_CLOAK, /* randart cloak, nicole trap effect, fuck over, +7 CON and 10% extra casting chance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Nothing For It",				PICK_AXE, /* randart2 pick-axe, fuck over, +5 digging effort */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Head-W",				STANDARD_HELMET, /* randart helmet, spellboost, you take 10% more damage, resist sleep, free action */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Green Mystery",				FEMININE_PUMPS, /* mystery resist, color changes to GREEN when you put them on; if your weapon is green, poison resist; if your weapon is bright green, sickness resist and sickopathy */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Mr. of high heels",				SEXY_LEATHER_PUMP, /* color changes to BLACK when you wield it, resistance piercing, magic find and if your char is male and some random item would have generated +0, there's 10% for it to generate with a random enchantment value. if your torso armor is black, death resistance and perilous life saving */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Ammy's Normalness",				BANDED_MAIL, /* mystery resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Teh Norm",				CHAIN_MAIL, /* mystery resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Res Circle",				SPLINT_MAIL, /* mystery resistance, invoke for 1000 turns of resistance piercing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3500L ),

A("Grant Especial",				PLATE_MAIL, /* mystery resistance, 10 extra points of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Nutrition and Dietetics",				STANDARD_HELMET, /* randart helmet, full nutrients, eating meat gives temporary stat debuff */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("How Can One Please Look Like That",				STANDARD_HELMET, /* randart2 helmet, -10 charisma, aggravate monster, autocurses, fuck over */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Nancy's Assistance",				ITALIAN_HEELS, /* anastasia trap effect, color becomes RED when equipped, resist fear, discount action; if you're female they also give mystery resist; if they're eroded, chaotic monsters have 25% chance of spawning peaceful */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Firm-Top",				LEATHER_CLOAK, /* randart cloak, material becomes FIRMAMENT when equipped */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Grisgren",				LONG_SWORD, /* randart melee weapon, material becomes GREENSTEEL when equipped */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Eww That's Disgusting",				LONG_SWORD, /* randart2 melee weapon, material becomes SECREE when equipped */
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK), 0, 0,
	ACID(10, 5), DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Complete Armoring",				LEATHER_ARMOR, /* randart armor, material becomes STEEL when equipped, double fire resistance */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Melugas Rocket",				REGULAR_GLOVES, /* randart gloves, material becomes TITANIUM when equipped, invoke for random breath of strength 5 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Cob-Auto",				DART, /* randart dart, material becomes COBALT when generated and 3-5x as much ammo */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Git All De Arts No",				ORCISH_GUARD_SHIELD, /* double magic find */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Castor Boost",				QUARTERSTAFF, /* randart quarterstaff, spellboost, contamination resistance */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	COLD(0, 7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Fanny's Brook That You Miss",				LOW_BOOTS, /* randart2 boots, counts as stiletto heels, +7 kick damage when female or +2 when male, but if your char is female and you kill a male boss monster outside of the planes, your char permanently turns into a feminist */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Wand Into Spell",				GLOVES_OF_ENERGY_DRAINING, /* double spellboost, 10% extra casting chance and 10% like failuncap, devices skill is drained by one with 1 in 100 per turn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Endarken Everything",				LIGHTLESS_BOOTS, /* mystery resist, material becomes SHADOWSTUFF when equipped, dark mode, hostile monsters with appr=1 have 1 in 3 that it becomes 0 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 9000L ),

A("Findeet",				CLOAK_OF_WRONG_ANNOUNCEMENT, /* magic find, 10% chance for items to generate cursed, if an item would have generated +0 there's 50% chance that the item gets a random negative enchantment value, +10 chance for timebasedlowerchance function */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Lookie-Lookie",				RIN_VISIBLE_BECOMING, /* magic find, clairvoyance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Nuclear Worth",				AMULET_OF_EVIL_CRAFTING, /* contamination resistance, fuck over */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Elros Eaerfalas",				ELVEN_MITHRIL_COAT, /* mystery resist */
	(SPFX_RESTR|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Anden Drer",				ELVEN_LEATHER_HELM, /* mystery resist */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Dlangout",				DWARVISH_IRON_HELM, /* magic find */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Outmine of Metal",				DWARVISH_MITHRIL_COAT, /* magic find, magical breathing */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Deepfound",				DWARVISH_CLOAK, /* magic find */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Pow-rer",				ELVEN_CLOAK, /* mystery resist */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Caaaaast",				ELVEN_LEATHER_HELM, /* spellboost */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("All In One Eff",				T_SHIRT, /* randart shirt, mystery resist, magic find, fuck over, spellboost, 1 in 1000 if it's not cursed to become cursed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Unusual Ench",				ELVEN_SHIELD, /* magic find, +1 increase damage, 20% chance that spells don't lose memory */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("One-In-Four",				RIGHT_MOUSE_BUTTON_STONE, /* randart bad gray stone, depending on the turn you have either mystery resistance, magic resistance, contamination resistance or death resistance */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Doubleshell",				ELIF_S_JEWEL, /* randart feminism gem, double mystery resist */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Not Gonna Contaminate",				ELIF_S_JEWEL, /* randart feminism gem, contamination resist, you always have some specific feminism trap effect that has 1 in 5000 chance per turn to change to another */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Have It All But Not Get",				RIGHT_MOUSE_BUTTON_STONE, /* randart bad gray stone, magic, death and mystery resist, can't get positive intrinsics from leveling up, and this item is immune to uncurse() effects */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Terror Drop",				SHORT_SWORD, /* randart2 short sword, if the enemy doesn't resist he has to flee for 2 turns when hit */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Gauge Reduce",				LONG_SWORD, /* randart2 long sword, if you hit the enemy his mspec_used variable is increased by 2 */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Pulverize Em",				CLUB, /* randart2 club, to-hit h@ck, does 3x damage, autocurses */
	(SPFX_RESTR), 0, 0,
	PHYS(1, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Mana Slasher",				SCYTHE, /* reduces enemy's mana by 5; if the enemy had less than that, the maximum is reduced */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Arrow Rain",				BOW, /* randart2 bow, +8 multishot if it doesn't have an artifact timeout but when that multishot is launched, the bow gets such a timeout */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Ghand",				REGULAR_GLOVES, /* randart2 gloves, if you attack unarmed and a monster with eyes fails a resistance check, it's blinded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Diffusion Wave",				WAN_NOTHING, /* randart2 wand, if you zap it while it still has charges you restore 2*gushlevel HP and Pw */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Bakuretu-Ken",				AMULET_OF_MAGICAL_BREATHING, /* randart2 amulet, attacking unarmed has 20% chance of spirit-bomb-like explosion that deals low damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Zin-Ba",				LEATHER_SADDLE, /* randart saddle, 10% better spellcasting chance, +1 increase and +4 damage when riding */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Mount Bond",				LEATHER_SADDLE, /* randart2 saddle, you can #monster to check up on your steed's HP and doing so doesn't use up a turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Full Dose Life",				STRIPE_MAIL, /* mystery resist, slow digestion */
	(SPFX_RESTR|SPFX_REGEN|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Res But Bad",				RING_MAIL, /* contamination resist, -15 AC, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("And How She Is Cursed",				SCALE_MAIL, /* putting it on while it's better than -3 sets it to -3, autocurses, monsters have 20% that appr=1 becomes 0 and 5% that appr becomes -1 */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Fortutiousness(sic)",				LEATHER_ARMOR, /* magic find */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Well Fuck",				STUDDED_LEATHER_ARMOR, /* autocurses, fuck over, can be invoked for 4 temporary points of protection and rnz(250) turns of controlled levitation */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Lite The Area",				JUMPING_BOOTS, /* sight bonus */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Skodit",				LONG_SWORD, /* randart melee weapon, fuck over, echolocation */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Whizzy Wiz",				WAN_TIME_STOP, /* stops time for d3 more turns */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Main Container",				STATUE, /* rolls a specific item class and then has 10 random items of that class inside */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Arti Lockbox",				STATUE, /* contains a random artifact */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Hoards of Treasure",				STATUE, /* contains 50 random items */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Emergency Cash",				STATUE, /* contains rn1(10000,10000) dollars */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Break-Into-Pieces",				BOULDER, /* if it's broken, the amount of resulting rocks is multiplied by rn1(10,10) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Entrap The Unwary",				BOULDER, /* if you push it, 1 in 100 chance that a trap is created underneath you unless there is one already */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Signal Tone",				BOULDER, /* calls wake_nearby() if you push it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Cool Dumbbell",				BOULDER, /* trains strength as fast as a vanilla boulder if you push it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Blohit",				BOULDER, /* +10 damage and to-hit if you throw it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("This Is Useless",				IRON_CHAIN,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Fire Is Good",				IRON_CHAIN, /* becomes +9 if you wield it while it's lower */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(0, 9), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 999L ),

A("Do The Control",				IRON_CHAIN, /* trap revealing when wielded, detect monsters if you're punished with it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Lucky Generation",				HEAVY_IRON_BALL, /* if this artifact is generated, your flail skill cap increases by one */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("How Is The Car Rowed",				HEAVY_IRON_BALL, /* -6 melee to-hit, totter */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Useless Talk",				HEAVY_IRON_BALL, /* resist piercing, psi resist, full nutrition and food appraisal when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Arabella's Thinner",				HEAVY_IRON_BALL, /* randart heavy iron ball, prime autocurse, yawm, magic vacuum, corona nastytrap effect, halved corona caps, and if this is your punishment ball it grants detect monsters when wielded */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Streeeeeam",				IRON_CHAIN, /* randart iron chain, if you're punished with it then your melee attacks have 1 in 20 to fire a magic missile with strength 6, can be invoked to fire the same type of magic missile */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 1000L ),

A("Klong-right",				HEAVY_IRON_BALL, /* randart2 heavy iron ball, becomes rustproof and repairs erosion when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Siyid",				IRON_CHAIN, /* randart2 iron chain, giant explorer, autocurses, if you're punished with it you have astral vision and auto destruct */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Race Along The Highway",				IRON_CHAIN, /* randart2 iron chain, very fast speed, if you're punished with this and are wielding your punishment ball then you're also very fast and get twice as much speed bonus as the "real speed devil" artifact */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Gormaler",				HEAVY_IRON_BALL, /* randart2 heavy iron ball, material becomes CHROME when wielded, orange spells, 10000 turns of jil trap effect when wielded, and if you've been wielding it for at least 1000 turns straight then there's 1 in 3 chance that attacking in melee puts enemies to sleep who don't resist */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(5, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("BEEEEEEEP",				BROKEN_SWORD, /* -3 melee to-hit and damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Most Scrapped Person In Existence",				RIPPED_CLOAK, /* -5 charisma, quaffing booze gives +100 nutrition and causes heavy confusion, enmity trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Rat Problem",				BROKEN_HELMET, /* +10 spawn freq for S_RODENT; if you're at full health, you have infravision */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("They Somehow Have A Radar",				RUSTED_MAIL, /* feeler gauges effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("The Tiny Thread",				CRUMBLED_SHIRT, /* deactivates disintegration resistance, enthumesis effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Some Attraction",				RUSTED_GAUNTLETS, /* +5 spawn freq for metallivores, 1 extra point of AC */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Its No Good",				BROKEN_SHIELD, /* burden */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("RollKabudd",				BROKEN_BOOTS, /* fumble counter is always at least 1000, protects you from AD_SLOW */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Gongde Ertal",				GOLDEN_SHIELD, /* mystery resist, spellboost */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Ems Boost",				MESH_SADDLE, /* +1 MC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Death Squad Mobile",				TANK_SADDLE, /* 5 extra points of AC for both you and the steed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Petcamera",				ADAMANT_LEASH, /* if this leash is in use, all leashed pets are always shown on screen */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Infinite Range",				LEATHER_LEASH, /* randart leash, a pet leashed to this one doesn't snap loose or choke no matter how far away it is */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Annoying Pet Monitor",				LEATHER_LEASH, /* randart2 leash; if this leash is in use, all pets are always shown on screen and you have the sound effect trap active */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Con Hold",				COPPER_PLATE_MAIL, /* prevents CON from being damaged, even for permanent stat damage effects */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Stachel Satchel",				COPPER_PLATE_MAIL, /* hannah trap effect, +2000 carry cap and +500 max carry cap, d5 thorns in melee */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Make It All Clothed",				COPPER_PLATE_MAIL, /* sickness resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Fungiradar",				COPPER_PLATE_MAIL, /* displays all F on the current level */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Okay, you win.",				COPPER_PLATE_MAIL, /* mystery res, and if your weapon is made of copper you also have free action */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Go Gassi",				ADAMANT_LEASH, /* no special effects */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_ZOOKEEPER, NON_PM, 2000L ),

A("Saber Tonfa",				TONFA, /* if it's in the secondary slot, diminished bleeding and +5 DEX */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Sacrifice Tonfa",				TONFA, /* aggravate monster, teleportitis, blood loss; if it's in the secondary slot, 5% more critical hits, +2 CON, +5 WIS, reflection, +5 melee to-hit and damage and 10 extra points of AC */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Shield Tonfa",				TONFA, /* if it's in the secondary slot, 12 extra points of AC, +3 CON, half physical damage and 10% chance to nullify incoming damage */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Sonic Tonfa",				TONFA, /* if it's in the secondary slot, 3 extra points of AC, ultra fast speed and mystery resist */
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	PHYS(2, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 15000L ),

A("Smash Tonfa",				TONFA, /* if it's in the secondary slot, +3 STR, 8 extra points of AC and double attacks without the speed malus */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Don't Go Away",				IRON_CHAIN, /* randart iron chain, spawns erosionproof and if it's your punishment chain, uncurse() doesn't unpunish you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Dockem Good",				HEAVY_IRON_BALL, /* randart heavy iron ball; if it's your punishment ball, uncurse() doesn't unpunish you */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Hoho Re-Punish Me",				IRON_CHAIN, /* while you're punished with it or also while you're wielding it, a counter is set to 50000 if it was lower and while the counter isn't zero yet, whenever you're not punished there's a 1 in 1000 chance for you to become punished. being whirly, amorphous or w/e does not prevent or stop punishment while the counter is nonzero */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Clang Sound",				UNWIELDY_PICK,
	(SPFX_RESTR), 0, 0,
	PHYS(2, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Senshi no Bichiku",				FAR_EAST_ARROW, /* spawns with 3x stack size */
	(SPFX_RESTR), 0, 0,
	PHYS(3, 6), NO_DFNS, NO_CARY, 0, A_NONE, PM_SAMURAI, NON_PM, 50L ),

A("Bakuhatsu-sei Misairu",				FAR_EAST_ARROW, /* explodes like hellfire bolts */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_GRENADONIN, NON_PM, 100L ),

A("Mr. Ahlblow's Signage",				EAGLE_BALL, /* randart2 orb, to-hit h@ck, multiplies your speed by 1.5, your torso armor and helmet automatically get removed or if they're cursed they become -10, you cannot have more than 5 STR while wielding it */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(1, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Diablo's Fire New Facts",				SHORT_SWORD,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(10, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Diablo's Cold Smackage",				SHORT_SWORD, /* randart short sword */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	COLD(0, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Diablo's LBOD",				SHORT_SWORD, /* randart2 short sword, 1 in 20 when meleeing that you fire a d10-strength lightning bolt in the direction of your attack */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Gitta's Handbag",				HANDYBAG, /* taking out items doesn't use a turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Klarabella's Handbag",				HANDYBAG, /* putting in items doesn't use a turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Stashit",				LEAD_BOX, /* putting in items doesn't use a turn, carrying this item causes 1000 additional units of weight */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Just Like That",				TOP_BOX,
	(SPFX_RESTR|SPFX_NOWISH), (SPFX_DEFN), 0,
	NO_ATTK, NO_DFNS, CARY(AD_MAGM), 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Giddem,fast!",				TITAN_SACK, /* taking out items doesn't use a turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("ND1, NND0D0, NDmD+-Dm D?D3/4ND0 D.D0D1ND, D2 D1/2D0NN IRC-NDmNN.", IMPLANT_OF_ABRASION, /* randart implant, mojibake, anti-teleportation, and if you're in a form without hands, 1 in 200 chance per turn to be healed back up to full HP */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("ND2D3/4N NDmNN ND3/4NDmN ND>>DmD1/2, NND0D0, NND0ND1/2D, D1/4D0D1/4N.", IMPLANT_OF_ABRASION, /* randart2 implant, quaversal, vulnerability, and if you're in a form without hands, trap revealing with 5x the normal rate */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Heap From The Yard",				JUNK_METAL, /* spawns with rn1(500,500) extra ammo */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Coldcrab",				COBALT_CHUNK,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	COLD(0, 7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 30L ),

A("Electric Shock",				BRONZE_NUGGET,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(8, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 40L ),

A("Gigabonk",				STEEL_SLUG,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Industrial Lightning",				SATURNITE_FIST,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(6, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Godae Musul",				JEONTU_GEOM, /* while wielded, you can use martial arts techs */
	(SPFX_RESTR), 0, 0,
	PHYS(8, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Quasimagical Shine",				DIM_LANTERN, /* spawns with 3x as much fuel */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Gonna Be Another Bloodbath",				JACK_KNIFE, /* autocurses */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 18), NO_DFNS, NO_CARY, 0, A_NONE, PM_MURDERER, NON_PM, 1800L ),

A("Ffff err what am I typing there",				REPLICA_UNICORN_HORN, /* 30% base chance of working but applying it gives rnz(500) turns of an alignment-dependant feminism trap effect: either inge, elif or jessica */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Atlus Heave",				TITANIUM_HORN, /* if it would have vaporized, it doesn't and instead becomes heavily cursed; +10 strength when wielded but also confusion and prevents you from having resist confusion, half physical or half spell damage, loses a point of enchantment (up to -20) every time you wield it and if it's -20 or worse, it becomes sticky prime cursed! */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("On Top",				FIRMAMENT_CHAIN,
	(SPFX_RESTR), 0, 0,
	PHYS(10, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Frigid Cancer",				COBALT_NUNCHIAKU,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(0, 8), DFNS(AD_DRST), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Block The Myst",				HEAVY_MESH_BALL, /* mystery resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Plim plim",				HEAVY_CORAL_BALL,
	(SPFX_RESTR|SPFX_SEARCH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Afterhall",				HEAVY_BRONZE_BALL,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Hardened in the Forge",				HEAVY_STEEL_BALL, /* fire immunity when wielded or also if it's your punishment ball */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Diljaned",				HEAVY_MERCURIAL_BALL, /* sickness resistance */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRST), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Snickersnack",				HEAVY_BAMBOO_BALL, /* displays all J when wielded */
	(SPFX_RESTR), 0, 0,
	PHYS(3, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Acicorner",				GREEN_STEEL_BALL, /* if it's your punishment ball, it grants acid immunity */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ACID(6, 6), DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Hei The Cold",				SINNUM_SCOURGE, /* if you're punished with it, cold immunity and freezopathy */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(18, 8), DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Everything-proof",				TITANIUM_SCOURGE, /* can be invoked to proof an item of your choice; if you're punished with it, 1 in 2000 per turn that you may proof an item of your choice */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 8000L ),

A("Crap Oxide",				ALU_SCOURGE, /* if you're punished with it, you can eat stone */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Experimental MIRV",				FATMAN, /* +7 multishot */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Megaton Load",				MINI_NUKE, /* +10 range, +8 radius and +6 damage dice for the explosion */
	(SPFX_RESTR), 0, 0,
	PHYS(25, 50), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Little Boy",				MINI_NUKE, /* -5 radius and -5 damage dice for the explosion, spawns with rnz(10) extra ammo */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Schatzki's ring",				RIN_INDIGESTION, /* slow digestion */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Downdrive",				IMPLANT_OF_ABRASION, /* randart implant, invoke for 1000 turns of shutdown or if you're in a form without hands, you can specify a number of 1 to 10000 turns of shutdown */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 4000L ),

A("Efficient Sharing",				STANDARD_HELMET, /* randart helmet, if your symbiote has less current HP than your character's current HP then the symbiote is shut down */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Maximum Shutlock",				RIN_ADORNMENT, /* randart ring, autocurses, if your symbiote has less than maximum HP then it's shut down */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Teh-Shock",				SHUTTER_SHIELD, /* lightning immunity */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Katherine's Beachwear",				LEATHER_CLOAK, /* initialized to always be tankini, 4 extra points of AC and if you're female, +3 CHA and peace_minded() has 10% of being true */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Boxing Lesson",				GAUNTLETS_OF_AUTOMATIC_SHUTDOW, /* +5 unarmed damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Arabella's Nastyguard",				RIN_NASTINESS_RESISTANCE, /* halves nastytrap durations, arabella trap effect, magic vacuum, fuck over and your score drains by one per turn */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Jubilex's Code",				RIN_INDIGESTION, /* sickopathy, diarrhea, stat decrease bug */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, IDENTIFY, A_NONE, NON_PM, NON_PM, 3000L ),

A("Fackle There",				LIGHT_MAIL, /* sight bonus */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 9000L ),

A("Arms Level One",				LIGHT_MAIL, /* +1 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Arms Level Two",				LIGHT_MAIL, /* +2 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Arms Level Three",				LIGHT_MAIL, /* +3 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Arms Level Four",				LIGHT_MAIL, /* +4 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Arms Level Five",				LIGHT_MAIL, /* +5 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Arms Level Ten",				LIGHT_MAIL, /* +10 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Greyshade",				LIGHT_MAIL, /* +5 DEX, you always have at least MC4 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4500L ),

A("Overheal Me",				SCR_HEALING, /* heals 3x as much */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Full Of Mystic Power",				SCR_MANA, /* restores 3x as much mana */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Saving For A Rainy Day",				SCR_EXTRA_HEALING, /* heals 10x as much */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Restore All Mana",				SCR_GREATER_MANA_RESTORATION, /* restores 10x as much mana */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Superphasedoor",				SCR_PHASE_DOOR, /* greater range of teleportation targets */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Antitoxin In The System",				SCR_CURE, /* rnz(1000) turns of resist confusion/stun/blind/hallu/sick/stoning */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Identifai Regular",				SCR_STANDARD_ID, /* identifies 2-5 items */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Allysaver",				SCR_HEAL_OTHER, /* heals 5x as much */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Whoa-Acid",				CORPSE, /* acid resistance when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Burnup",				CORPSE, /* fire resistance when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Somnus Mortus",				CORPSE, /* sleep resistance when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Ylva Blimp",				CORPSE, /* cold resistance when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Veryfirm",				CORPSE, /* disintegration resistance when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Goodshock",				CORPSE, /* shock resistance when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Emergency Assistance",				CORPSE, /* poison resistance when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Nymphism",				CORPSE, /* teleportitis when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Tengu Shuffle",				CORPSE, /* teleport control when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Eyes Everywhere",				CORPSE, /* telepathy when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Radar Up!",				CORPSE, /* warning when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Let's Go On A Hunt",				CORPSE, /* searching when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Stalk-me",				CORPSE, /* invis when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("found it.",				CORPSE, /* see invis when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Lying Novel Cerium",				CORPSE, /* sickness resistance when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Not Gonna Die",				CORPSE, /* death resistance when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Mysteriumtart",				CORPSE, /* mystery resistance when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Feel The Living",				CORPSE, /* drain resistance when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Dueueueueueuei",				CORPSE, /* regeneration when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Oh, well...",				CORPSE, /* stealth when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("You Badass Enough?",				CORPSE, /* conflict when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Ice Snack",				CORPSE, /* swimming when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Redeye",				CORPSE, /* infravision when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Luke's Little Snack",				CORPSE, /* use the force when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Mindbonuses For You",				CORPSE, /* psi resistance when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Nose-Up",				CORPSE, /* scent view when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Blue-blue-blue...",				CORPSE, /* pain sense when eaten */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("The 'u' command",				CORPSE, /* defusing when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("MFer",				CORPSE, /* magic find when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Wizardlunch",				CORPSE, /* spellboost when eaten */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Assload of Loot",				TREASURE_CHEST, /* contains rn1(100,100) additional items */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Rurihunt",				TREASURE_CHEST, /* contains d3 random artifacts */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100000L ),

A("Demonseal",				TREASURE_CHEST, /* contains 50 items of a specific type but if you open it while it has no artifact timeout, it gets one and spawns a hostile demon lord who may be frenzied */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50000L ),

A("Dollar Deposit",				LOOT_CHEST, /* contains 10000 $ */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Weaponmemory",				LOOT_CHEST, /* contains 10 random weapons */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Armormemory",				LOOT_CHEST, /* contains 10 random armor pieces */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Gradiating Work",				ROUND_MAIL, /* can be worn even if you're too small. if you wear it for 20k turns straight and aren't wearing any other armor pieces during that time, you obtain intrinsic food appraisal but performing a multi-turn wait or search command erases it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Actual Gradiating Work",				ROUND_MAIL, /* magical breathing and 50% higher HP cap for corona when worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Jefferson's Lookaround",				ROUND_MAIL, /* displays all big monsters with distu of 100 or less */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Go-Under-Or-Up",				ROUND_MAIL, /* swimming, protects stuff from getting wet */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Casserino",				ROUND_MAIL, /* can be invoked to play blackjack */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 5000L ),

A("First There We Were",				T_SHIRT, /* randart2 shirt, +3 STR, resist fear, heavily autocurses, anna trap effect, halves monster spawn rate */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Blinkblinkblink, blinkblinkblink",				TRANSPARENCE_THROWER, /* +50 turns of invis every time a thrown one hits something */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Crabbomat",				PINCER_STAFF, /* +d15 bleeding damage, dmg h@ck, +4 AC and if the turn counter is divisible by 10, you have prism reflection */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("See The Rest Of The World",				UNDERWATER_TRIDENT, /* underwater vision bug, but if you are underwater and this weapon isn't cursed, it becomes cursed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Schwiuquiek",				SET_OF_CROW_TALONS, /* +8 bleeding damage */
	(SPFX_RESTR), 0, 0,
	PHYS(8, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Incorrect Healing Adjust",				BLADE_OF_PITY, /* 10% chance to heal both you and the enemy by 50 with a hit */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Point Dexter",				PARAZONIUM, /* damage bonus is actually +DEX (up to 25), wielding it gives greta trap effect permanently */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(10, 1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Cloudcracker",				BRONZE_DAGGER,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(4, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Eldritchkruun",				FANG_OF_APEP,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	STUN(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Struck-on",				STEEL_KNIFE, /* dmg h@ck, throwing deals +10 damage */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Necmeasure",				STEEL_KNIFE, /* poisons the target */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 6), NO_DFNS, NO_CARY, 0, A_NONE, PM_NECROMANCER, NON_PM, 5000L ),

A("On Cloud Seven",				CONSECRATED_KNIFE, /* magical breathing, jumping, +2 kick damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("The Olympide",				DISKOS, /* +5 STR, throwing it deals +8 dmg */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Freak Versus Banger",				QUATTUORST_AXE, /* heavily autocurses, jane and greta trap effects */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Obvious As Fuck",				MIRRORBLADE, /* 75% chance to resist gaze attacks */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("No Prisoners Taken",				BLADE_OF_MERCY, /* fast speed, for every 25% that the enemy is below max HP you deal a flat 6 extra damage to it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Screamout",				BLADE_OF_GRACE, /* the STUN attack has 1 in 3 to confuse the opponent */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	STUN(7, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Named After Itself",				GLADIUS, /* to-hit h@ck */
	(SPFX_RESTR), 0, 0,
	PHYS(1, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Skin Degree",				ISAMUSEI, /* swimming, unbreathing, dmg h@ck */
	(SPFX_RESTR), 0, 0,
	PHYS(7, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Kimyo ni hikaru sonzai",				ISAMUSEI, /* +2 sight range */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Kosomusei",				KACHISEI, /* multilinguitis */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Kami sori no usui ha",				KACHISEI, /* bisects targets */
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Heavy Pole Skill",				ZANBATO, /* can be applied like a polearm, if your polearms skill isn't at least skilled you can't be very fast, if you're restricted in polearms you also can't be fast */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Ice Temple Background",				UNNATURAL_SWORD, /* 5000 turns of mariya trap effect */
	(SPFX_RESTR|SPFX_ATTK|SPFX_EVIL), 0, 0,
	COLD(9, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Wierd Accent",				DOUBLE_FORCE_BLADE, /* flying, killing a monster restores 1 HP */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Chrismiss",				DOUBLE_FORCE_BLADE, /* deals +4 flat damage per vaapad skill */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Megaforce",				DOUBLE_FORCE_BLADE, /* improves the force command as if this was a lightsaber */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Lainchantment",				COBALT_STAFF, /* double cold res */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Mambo Number Nine",				BAMBOO_STAFF, /* 3x as much bonus speed as real speed devil */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Byebye Bones",				BREAKER_STAFF,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 14), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("P-wing",				FEATHER_STAFF, /* flying, very fast, but if you hear the cwnannwn the staff is cursed and if you hear the banshee it becomes -10 if it was higher */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Woetschersmag",				FORCE_WHIP, /* can be used like a polearm */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Insectmasher",				CHITIN_WHIP, /* scent view, your kick cannot be clumsy */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_ANT,
	PHYS(0, 15), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Tankfriend",				FORMICAN_WHIP, /* +5 AC, S_ANT spawn peaceful 90% of the time, pain sense */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_BAT,
	PHYS(0, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Streckout-and-Pull",				CLAWED_HAND, /* can be used like a polearm but only orthogonally */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Roarbear",				BESTIAL_CLAW, /* aggravate monster, enemies have half as much AC */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(6, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Nadja's Broken Nail",				MEASURER_QATAR, /* to-hit h@ck, resist fear */
	(SPFX_RESTR), 0, 0,
	PHYS(1, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Those Lazy Programmers",				QUHAB, /* poisons the enemy, if you have a shield you get +10% chance to block */
	(SPFX_RESTR), 0, 0,
	PHYS(4, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6600L ),

A("Milena's Misguiding",				SUWAYYAH, /* confusion, flying, full nutrients, food appraisal */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ATTK), 0, 0,
	FIRE(2, 7), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Rhalalalalalaaaar",				BREAKING_WHEEL, /* +6 AC, and if the enemy has less than 10% HP remaining after a hit, it dies */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Clackindra",				CARCOSAN_STING, /* +3 multishot */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("UZ-I",				LEAD_UNLOADER, /* -d10 to-hit, +d3 multishot */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("AR-Armalyte",				STORM_RIFLE, /* +1 multishot, -2 ranged to-hit */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Middling Piddling",				CARCOSAN_BOLT, /* can be safely enchanted up to +20 */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 30L ),

A("Shredshot",				BRONZE_SMG_BULLET, /* erodes the target's armor if possible, or if already max erosion then it loses a point of enchantment up to -20 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Duskshredder",				HOLLOW_POINT_MG_BULLET,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Fuck the server",				LOWRATE_ASSAULT_RIFLE_BULLET, /* +2 multishot */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Shugshog",				COATED_AUTO_SHELL,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ACID(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 150L ),

A("Megical Strike",				LIGHT_ARROW,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	MAGM(5, 11), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Trap-Em-Long",				FALCHION, /* game behaves as if you were in homicider mode while wielding it, and your speed remains deactivated for 1000 more turns if you unwield it */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 22), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Now Go And Climb",				MOUNTAIN_SWORD, /* allows you to walk on mountains */
	(SPFX_RESTR), 0, 0,
	PHYS(7, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Uruk Taunt",				ORCISH_SCIMITAR,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(0, 2), DFNS(AD_DRST), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Meat's Back On The Menu",				ORCISH_SCIMITAR, /* full nutrients, monsters drop corpses more often */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Iron Reinforcement",				STINGING_SWORD, /* double spellboost */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("yawnen",				ORCISH_LONG_SWORD, /* res sleep */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Always In It",				FLOWING_SWORD, /* sight bonus, prevents items from getting wet */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Now Ascend Already",				ULTRA_SWORD, /* invoke for enchant armor */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_NOWISH), 0, 0,
	ELEC(0, 7), DFNS(AD_COLD), NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 20000L ),

A("It Brings-s",				HEAVY_MACE,
	(SPFX_RESTR), 0, 0,
	PHYS(10, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Jean-Baptiste's Terminator",				EXECUTIONER_S_MACE,
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Coolshielding",				TINMACE, /* cold immunity */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Resole the woman's ass",				TINMACE, /* if you hit a MS_FART_something with it, +1 butthurt */
	(SPFX_RESTR|SPFX_DFLAG2), 0, (M2_FEMALE),
	PHYS(0, 9), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Give Us A Name",				ORCISH_MORNING_STAR, /* poisons enemies, magic find */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Zuse's Comp",				TURNING_STAR, /* dmg h@ck, extra skill training, clairvoyance */
	(SPFX_RESTR), 0, 0,
	PHYS(8, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Donk O'Bonk",				HEAVY_WAR_HAMMER, /* hitting something reduces your sanity and contamination by 1 */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(0, 8), DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Silver on Black",				SEXY_MARY_JANE, /* disint res when wielded */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Raidgood",				SEXY_MARY_JANE, /* resist death */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ACID(7, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Extreme Bitch",				KITTEN_HEEL_PUMP, /* quad attacks, 5000 turns of julietta trap effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(8, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Inram",				KITTEN_HEEL_PUMP, /* 1 in 5 chance to deal +d20 damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("The Lord's Lash",				TRIPLE_HEADED_FLAIL, /* every time you wield it, u.lifesavepenalty is increased by one */
	(SPFX_RESTR|SPFX_ATTK|SPFX_EVIL), 0, 0,
	FIRE(0, 18), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Kraschde Gloman",				SPIKED_CHAIN, /* 1 in 5 chance that the enemy has to resist or gets 100 turns of heal block */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("ozyzevpdwtvp",				FOOTBOW, /* -2 range, -1 multishot, autocurses, doesn't prevent you from using your hands, +rn1(7,4) bashing damage. ella trap if you're gay, gudrun trap if hetero and manuela trap if undecided */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_DANCER, NON_PM, 6000L ),

A("For The Real Gaming Experience",				MASTER_KNUCKLES, /* -5 melee to-hit, 10000 turns of doomed */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Yet Another Useless Part",				SHELL_CLUB,
	(SPFX_RESTR), 0, 0,
	PHYS(2, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Cnuttel-em",				CUDGEL, /* +10 spawn freq for cdda monsters */
	(SPFX_RESTR|SPFX_DCLAS|SPFX_EVIL), 0, S_ZOMBIE,
	PHYS(4, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Wamm-Bamm",				CUDGEL,
	(SPFX_RESTR), 0, 0,
	PHYS(21, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Titanic Strike",				LIGHT_CLUB, /* +20 STR */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Smash To Mush",				STEEL_CLUB,
	(SPFX_RESTR), 0, 0,
	PHYS(6, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Skystormer",				HEAVENLY_BALL, /* extra skill training, 10% chance to fire a rn1(10,10) cold breath in melee */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 18000L ),

A("Onchange Staff",				DRAMA_ORB, /* double fire res, +3 INT, protects items from shock */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	COLD(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Disappear Off The Map",				MARKET_GRINDER, /* +d12 grinding damage, map amnesia that persists for 10k turns after unwielding it */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(6, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Maercofog",				GRAND_GRINDER, /* double sight bonus */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 18000L ),

A("HK Clear",				SHICKSTAKE, /* full nutrients, magical breathing */
	(SPFX_RESTR), 0, 0,
	PHYS(7, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Oxidized Alloy",				PATINA_BAR, /* becomes very rusty when wielded while it's less rusty than that */
	(SPFX_RESTR|SPFX_EVIL|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ACID(8, 6), DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("The Meat Scraper",				LOCHABER_AXE, /* to-hit h@ck, magic find, +6 bleeding damage */
	(SPFX_RESTR), 0, 0,
	PHYS(1, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3500L ),

A("Flag Raised",				TOP_POLE, /* 1 in 20 to replicate the "cause fear" effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Den Daer Tjejen Aer Din Undergang",				SVAERDSTAV,
	(SPFX_RESTR), 0, 0,
	PHYS(3, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Vores Horder Vil Lemlaeste Og Plyndre",				STANGOEKSE,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(10, 11), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Eva's Colorchange",				COBALT_SPEAR, /* color becomes ORANGE when wielded, resist sleep, +3 kick damage */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(0, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("From Nix Comes Nix",				CHEAP_SPEAR,
	(SPFX_RESTR), 0, 0,
	PHYS(2, 3), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 300L ),

A("Wiunew",				CHEAP_SPEAR, /* spawns with rn1(4,4) more ammo, +3 range, dmg h@ck, 90% chance if it would have broken that it doesn't */
	(SPFX_RESTR), 0, 0,
	PHYS(7, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Dohlow",				PUNCTURING_JAVELIN, /* +12 thrown damage, spawns with 4 more ammo */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Gehenna Mode",				UNDERWORLD_JAVELIN, /* if you're in gehennom, this has +4 range or if you wield it, other ranged weapons have +3 range */
	(SPFX_RESTR), 0, 0,
	PHYS(6, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Kabooooom!",				AIRSTRIKE_PIKE, /* fiery explosion whenever the ranged attack hits something */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Useful Bindle",				BAGGY_SLING, /* +1000 carrcap and maxcarrcap when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Tact-Ebony-Glass",				LIGHT_CROSSBOW, /* resist disintegration and poison */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Long Strale",				SPIKY_BOLT, /* +7 bleeding damage */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Supahit",				HARDENED_BOLT, /* to-hit h@ck because it's soooooooo gay that you can't just have a double damage flag */
	(SPFX_RESTR), 0, 0,
	PHYS(1, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Hvyhit",				HARDENED_BOLT,
	(SPFX_RESTR), 0, 0,
	PHYS(0, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Hugestock",				LOWLY_DART, /* spawns with 5x as much ammo */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4L ),

A("Annoyprick",				WOOD_DART, /* dmg h@ck */
	(SPFX_RESTR), 0, 0,
	PHYS(12, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20L ),

A("Fjell Barb",				LIGHT_STAR, /* if it hits something, the square that got hit becomes lit */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("Wellder Grant",				ALU_BOOMERANG, /* spawns with rn1(3,2) more ammo */
	(SPFX_RESTR), 0, 0,
	PHYS(10, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Thundering Glaive",				BARDICHE, /* randart polearm */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ELEC(6, 10), DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Soul Reaper",				BARDICHE, /* randart2 polearm */
	(SPFX_RESTR|SPFX_DRLI|SPFX_ATTK), 0, 0,
	DRLI(5, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Frostbite Pike",				BARDICHE, /* randart polearm */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(7, 9), DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Doomsday Halberd",				BARDICHE, /* randart2 polearm */
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Phoenix Lance",				LANCE, /* randart lance */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(4, 14), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Void Scythe",				BARDICHE, /* randart2 polearm */
	(SPFX_RESTR), 0, 0,
	PHYS(20, 5), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Electronic Lock",				LOOT_CHEST, /* sets container kaboom trap to 10000 if it was lower */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Failprize",				TREASURE_CHEST, /* spawns empty */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Goodfilm",				EXPENSIVE_CAMERA, /* can take two photos per turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_TOURIST, NON_PM, 3000L ),

A("Rapidclick",				EXPENSIVE_CAMERA, /* doesn't use a turn when used while it has charges */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_TOURIST, NON_PM, 2000L ),

A("Modern Digicam",				EXPENSIVE_CAMERA, /* spawns with 10x as many charges */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Loudness Amplifier",				TIN_WHISTLE, /* works on bigger distances */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Pet Come Here!",				TIN_WHISTLE, /* pet follows for a longer time after applying */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Blammer of Doom",				TIN_WHISTLE, /* +10 bash damage, can use martial arts techs while wielding it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Make Broken",				BELL, /* 50% chance to not wake up monsters */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 400L ),

A("solvedbridge",				BELL, /* can open the drawbridge */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Hear Far And Wide",				BUGLE, /* wakes up more monsters, not just soldiers */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Taetaeraetaeae-tae",				BUGLE, /* uses a turn 50% of the time only when applied */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Invuni",				BUGLE, /* can be invoked to turn it into a unicorn horn or back into a bugle, and while it's a bugle it deals +5 bash damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 1500L ),

A("This Is Not A Drill",				BUGLE, /* frenzies soldiers when used */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Sack of Holding",				SACK, /* works like a boh */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Feeldasag",				SACK, /* pain sense when carried */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Oortocash",				SACK, /* spawns with rnz(2000) $ */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Booty Case",				LARGE_BOX, /* contains 3 random items in addition to its regular contents */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Ironit",				LARGE_BOX, /* material becomes IRON when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Snap-Too",				LARGE_BOX, /* 1% when used that it locks itself */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Pirate Treasure",				CHEST, /* contains 6 random items in addition to its normal contents */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_KORSAIR, NON_PM, 3000L ),

A("Cannotrap",				CHEST, /* if you apply it while it has a trap, the trap automatically becomes disabled */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Fuck Off Thief",				CHEST, /* resistant to being stolen by enemies */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Eatbonanza",				ICE_BOX, /* contains 5x as many corpses */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Cold-haha",				ICE_BOX, /* cold immunity when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("klueueueu",				ICE_BOX, /* +5 range when thrown, always hits the enemy */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Forty Four Is Luck",				POTATO_BAG, /* contains 5 additional musable items */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4444L ),

A("seminarios kartoffles",				POTATO_BAG, /* waterproof */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("A Mage Did It",				POTATO_BAG, /* invoke applies a layer of grease to the bag */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 4000L ),

A("Keep It Warm Inside",				OILSKIN_SACK, /* invoke uncurses the sack, or blesses if already uncursed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 4000L ),

A("Don't Run Out Of Halloween Material",				BAG_OF_TRICKS, /* spawns with 3x as many charges */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Spawn Da Killaz",				BAG_OF_TRICKS, /* if it has charges, applying it gives +5000 turns of killer spawn trap effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("The Gauntlet Abbreviation",				SKELETON_KEY, /* can be used on the gauntlet key door, clairvoyance and psi resistance when wielded */
	(SPFX_RESTR|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Vladimir Be Afraid",				SKELETON_KEY, /* double drain res when wielded */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_VAMPIRE,
	FIRE(0, 15), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Unbrit-sov",				SKELETON_KEY, /* keeps working when brittle, but if it becomes brittle you get 10000 turns of cursed and if it's already brittle also 10000 turns of doomed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Vanulla Score",				SKELETON_KEY, /* doesn't become brittle, but using it sets the nastiness trap timeout to 5000 if it was lower */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Sysette's Thievingness",				SKELETON_KEY, /* wielding gives +7 kick damage and another +8 if wearing block heels, passively trains firearms 1 in 100 turns and gun control 1 in 1000, gun control is trained more slowly specifically when male (not female) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Dithers Wuma",				LOCK_PICK, /* doesn't become brittle, but if it would have, +50000 turns of jette trap */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Hold It Out",				LOCK_PICK, /* 20 extra AC and if you have a shield, 20% more chance to block */
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Jastarti",				LOCK_PICK, /* no specialties */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Silver Yendorian Express Card",				CREDIT_CARD,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ESP|SPFX_HSPDAM), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Tight Spot",				CREDIT_CARD,
	(SPFX_RESTR|SPFX_STLTH|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Strange Lilchen",				CREDIT_CARD, /* sets jil trap to 5000 if it was lower when used, invoke can un-brittle this artifact for 10000 $ */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Getin on Vlads",				CREDIT_CARD, /* can open the doors in vlads tower */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Crude Sensei",				CREDIT_CARD, /* +150% unarmed damage (when dual-wielding with other hand empty) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Complete Bugnose",				GOLD_PIECE, /* displays "Lol, you have encountered a nose!" when generated */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Lorofem",				TALLOW_CANDLE, /* +rn1(7,3) stack size */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Widelite",				TALLOW_CANDLE, /* +1 light radius */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Mucho Talcum",				TALLOW_CANDLE, /* 3x as much duration */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Couple-in-the-box",				WAX_CANDLE, /* 3x as much stack size */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Duratex",				WAX_CANDLE, /* +4000 duration */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Free From Strain",				WAX_CANDLE, /* if it's on and wielded, aggravate monster doesn't randomly summon monsters from a portal */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Galadriel's Aid",				OIL_LAMP, /* +1 radius */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Everyglass is weird",				OIL_LAMP, /* gets +5000 duration with a potion of oil */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Out-oiled",				OIL_LAMP, /* spawns empty */
	(SPFX_RESTR|SPFX_STLTH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Kitzle The Wish Out",				MAGIC_LAMP, /* vanilla chances of getting a wish */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Illumer The Great",				MAGIC_LAMP, /* works consistently when cursed */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("You Got Me",				MAGIC_CANDLE, /* no specialties */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Firering",				MAGIC_CANDLE, /* deactivates stealth when carried */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Halloween Fuel",				BRASS_LANTERN, /* 2x as much fuel when generated */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Jenny's Protecter",				BRASS_LANTERN, /* displays all MS_MOLEST when on and wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Andy's Signweiser",				BRASS_LANTERN, /* clairvoyance when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Leo Is Always High",				BRASS_LANTERN, /* funny hallu when carried, invoke for gold detection but doing so gives +2000 cop wanted level */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Leli Dab",				MAGIC_WHISTLE, /* when cursed, only has 1 in 3 risk of malfunctioning */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("dbl play speed",				WOODEN_FLUTE, /* tries twice to affect the monster */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Hard String",				WOODEN_HARP, /* 5x less likely to break */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Change The Tune",				WOODEN_HARP, /* invoke to turn into wooden flute or back */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 2000L ),

A("Bom Bom Bom",				LEATHER_DRUM, /* louder than usual */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Bobobobobom",				LEATHER_DRUM, /* even louder, vuvuzela effect when used */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Motherfucking Bomb",				LAND_MINE, /* places a bomb when applied */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Precision Detonating Stuff",				LAND_MINE, /* you don't fumble when arming it */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Clickfield",				LAND_MINE, /* generates mines in a certain radius */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Webburound",				BEARTRAP, /* generates webs on the 8 surrounding squares */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Place on top of snare",				BEARTRAP, /* can be used on a trapped square to delete the trap that is there */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("field inited, sorry",				BEARTRAP, /* spawns random traps upon generation */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Make The Fog Away",				MIRROR, /* works properly even when cursed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Sub-Etha",				TOWEL, /* double searching */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Total Perspective Vortex",				TOWEL, /* detect monsters, prime autocurses, starvation trap, no death drops, simeout */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Iustitia Bringer",				BLINDFOLD, /* all your weapons deal a flat +10 damage against chaotic monsters */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Seeing The Person Anyway",				BLINDFOLD, /* displays all chaotic monsters with distu of 25 or less, 2000 turns of choiceless, no prompts and goldspells */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Satan's Specific Enchantment",				LENSES, /* 5000 turns of satan trap effect */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Nutriroll",				TINNING_KIT, /* spawns with 3x as many charges */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Refill Properly",				TINNING_KIT, /* regains many more charges when recharged */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Corona's Toll",				MEDICAL_KIT, /* contains 3x as much stuff */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Sykes's Infinity",				MEDICAL_KIT, /* contains 10x as many bandages */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Marvelous Function",				MEDICAL_KIT, /* doesn't malfunction when not cursed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Sing's Stock",				CAN_OF_GREASE, /* spawns with 3x as many charges */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Sing's Last Laugh",				CAN_OF_GREASE, /* spawns with 10x as many charges but every time you use it, a singcleaning monster is spawned somewhere */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Euro's Unwaste",				CAN_OF_GREASE, /* uses a charge 50% of the time only */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Beh, too easy",				MAGIC_MARKER, /* spawns with twice as many charges */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 25000L ),

A("Write The Unknown",				MAGIC_MARKER, /* doesn't fail to write unknown scrolls, but writing any scroll with it uses 2-3 times as much ink */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Seep Into The Soul",				MAGIC_MARKER, /* every time you write it, 1 permanent damage to INT but you gain as much alla as the amount of ink you used */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("End-Of-The-World Switch",				HITCHHIKER_S_GUIDE_TO_THE_GALA, /* applying it gives ragnarok timeout if none is active */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Peculiar Markings",				EGG, /* spawns with your markings */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Manylings",				EGG, /* if it hatches, spawns rnd(9) extra copies of the specified monster type */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Fertilization Fail",				EGG, /* never hatches */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Nutri-Roulette",				TIN, /* 10x as much nutrition but not below -1000 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("There Will Be Revenge",				TIN, /* eating it spawns rn1(10,10) copies of the monster somewhere */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Half Crawls Back Out",				TIN, /* the monster spawns next to you with half of its maximum HP */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Crawl Into Life",				SUNLIGHT_MAGGOT, /* can be invoked to gain a symbiote if you don't have one yet, but if you remove this helm, regardless of how, the symbiote dies and you get penalized as if you had murdered it */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Magma Bringer",				TOQUE, /* fire immunity */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("In Balance",				HARMONIUM_HELM, /* free action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Eeoyoo Eeoyoo",				PONTIFF_S_CROWN, /* prayer timeout goes down twice as fast and tells you if it becomes zero, dropping stuff on altars trains BUC knowledge more quickly and sacrificing trains spirituality more quickly */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Fesdebsi",				FACELESS_HELM, /* psi res */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("See The Total",				HELM_OF_MADNESS, /* resist confusion and stun, prevents hallu res, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Staer Waers March",				IMPERIAL_ELVEN_ARMOR, /* 1 in 10000 to roll a random lightsaber form and unlock it or have a chance of increasing its cap per turn; if it does make a change, you also gain a random hybrid race permanently and get 5 AD_DEBU effects as well as one AD_STAT effect */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Be Fat And Still Healthy",				HARMONIUM_PLATE, /* fast speed, inertia times out very quickly */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Completely Light",				HARMONIUM_SCALE_MAIL, /* +500 carry cap and max carry cap */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("A Waste",				THROWAWAY_ARMOR, /* no specialties */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Deeeeet",				THROWAWAY_ARMOR, /* -5 AC */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1L ),

A("Wschie.",				THROWAWAY_ARMOR, /* +5 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Dorl-Tsch",				THROWAWAY_ARMOR, /* prevents the negative effects from standing in grass */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Go To The High Range",				THROWAWAY_ARMOR, /* works like climbing set */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Goodnet",				MESH_ARMOR, /* +5 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1200L ),

A("Resistant to Deadgoing",				MESH_ARMOR, /* +5 AC */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Swordhiltstop",				MESH_ARMOR, /* also gives intrinsic half physical damage */
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Beepa Device",				MESH_ARMOR, /* +5 AC, defusing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Rememberance - the burst time",				MESH_ARMOR, /* keen memory */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Kwourstomal!",				CUIRASS_ARMOR, /* +10 AC, very fast speed, monsters who hit you in melee must make a saving throw or be slowed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("I Am Your Fall",				CUIRASS_ARMOR, /* very fast speed, +10 increase accuracy and +2 increase damage */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 25000L ),

A("Jeanetta's Revenge",				CUIRASS_ARMOR, /* heavily autocurses, jeanetta trap effect, +5 kick damage, resist fear/mystery, rays have 1 in 3 chance to miss you */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Radar Fell Up",				CUIRASS_ARMOR, /* detect monsters, prime autocurses, unchanging and if you can't be unchanging, +rnz(5000) sanity whenever you polymorph */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Eth-Ith",				HELLFORGED_PLATE_MAIL, /* +3 increase damage, monsters have half as much AC, your cop wanted level is always at least 10000 while wearing this */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Cover The Sexy Butt",				WAISTCLOTH, /* katia trap effect, trapwarping, +2 MC */
	(SPFX_RESTR|SPFX_EVIL|SPFX_SEEK), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Friederike's Bundling",				REINFORCED_SHIRT, /* autocurses, friederike trap effect, resist fear, +5 kick damage, if you don't have the meltee technique you learn it but in that case your acid resistance is disabled for one million turns */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Glaringadernadir",				SMOKY_VIOLET_FACELESS_ROBE, /* resist confusion and psi */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Derandergraaa",				ROBE, /* always initialized to be faceless robe, resist stun and psi */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Da Pelta",				BUCKLER, /* AD_DARK doesn't make areas dark if it hits you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Why Is It Intelligent",				BUCKLER,
	(SPFX_RESTR|SPFX_INTEL|SPFX_ESP|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("BlockBetter",				LIGHT_SHIELD, /* +5% chance to block */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Inside-Out",				LIGHT_SHIELD, /* resist light, +3 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Higher Training",				RESONANT_SHIELD, /* stat exercise happens more quickly and the soft caps are higher */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Youth Unword",				CRINGE_SHIELD, /* +2 increase melee damage when worn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Always Melee Cover",				CRINGE_SHIELD, /* 20% chance to nullify incoming melee damage (mhitu.c) */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Super Skellie",				BRONZE_SHIELD, /* +5 AC, +10% chance to block */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Who cares about a little rust?",				BRONZE_SHIELD, /* spawns with one level of rust, +3 AC and +15% chance to block */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Fitting Color",				BRONZE_SHIELD, /* d6 thorns in melee */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Vitality Storm",				TOWER_SHIELD, /* +8 CON, +2% chance to block, +3 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Healing Waters",				HARMONIUM_GAUNTLETS, /* full nutrients */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Wideforce Grip",				REGULAR_GLOVES, /* initialized to always be hooked gloves, can be invoked to pick up an item from a square of your choice */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 2000L ),

A("Gehnac",				ORCISH_BOOTS, /* +1 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Lagolwesh",				ORCISH_BOOTS,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRST), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Dajusdubidaelv",				ORCISH_BOOTS,
	(SPFX_RESTR|SPFX_ATTK|SPFX_WARN|SPFX_DFLAG2), 0, M2_ELF,
	PHYS(0, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Natural Spring",				HARMONIUM_BOOTS, /* flying, prevents fumbling */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Stand Tall In The Storm",				WIND_AND_FIRE_WHEELS, /* prevents you from being pushed back */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("The Heme Flow",				LOW_BOOTS, /* initialized to always be bladed disks, adds +d10 bleeding damage to the kick */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("gendergendergendergendergendergendergender",				LEATHER_ARMOR, /* randart2 armor */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_GENDERSTARIST, NON_PM, 1000L ),

A("General Ta's Annoyance Factor",				CUIRASS_ARMOR, /* extra skill training */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Equifriend",				POTATO_BAG, /* contains 1 extra weapon and 1 extra armor */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("The Toolbox",				POTATO_BAG, /* contains 3 random tools */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Burner Dream",				OIL_CANDLE, /* spawns with +2000 fuel, has +2 light radius and if it's on and in your inventory, you have peacevision and 5000 turns of kristina trap */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Fightebossit",				TREASURE_CHEST, /* when it's generated, a random G_UNIQ spawns somewhere */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Legendary Battle",				LOOT_CHEST, /* when it's generated, a boss spawner is placed somewhere */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Itak Hone",				LONG_SWORD, /* randart long sword, spawns with stack size of 5 */
	(SPFX_RESTR), 0, 0,
	PHYS(2, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Hamma Hamma",				WAR_HAMMER, /* randart hammer, invoke to create a copy of "having hamma" which is re-initialized first but this artifact needs to be at least +0 and not cursed, and will become heavily cursed -5 in the process. */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 5000L ),

A("Having Hamma",				WAR_HAMMER, /* randart2 hammer */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("enchanted, of defusing",				LEATHER_ARMOR, /* randart armor, 5 extra points of AC, defusing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Evil Mother Celine",				LACQUERED_DANCING_SHOE, /* tries to mindflay the monster, resist fear, but if you're wearing boots, they get destroyed or if they are an artifact, they're transferred to a monster and levelported away. */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	PHYS(0, 10), DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Spellnotforget",				AMETHYST_DRAGON_SCALE_MAIL, /* keen memory */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Acidirge",			LONG_SWORD, /* randart long sword */
	(SPFX_ATTK|SPFX_RESTR|SPFX_DEFN), 0, 0,
	ACID(5,2), ACID(0,0), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("The Cell",				QUARTERSTAFF, /* randart2 quarterstaff */
	(SPFX_RESTR|SPFX_DFLAG2|SPFX_REGEN), 0, (M2_UNDEAD),
	FIRE(0, 25), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Good Game Zuang Li",				ZANBATO, /* bisects enemies */
	(SPFX_RESTR|SPFX_ESP|SPFX_BEHEAD), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Lise's Underwear",				T_SHIRT, /* randart2 shirt, autocurses, lisa trap effect, +6 CHA, engraving bug, 50% chance to resist disenchantment */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Kuse Muse",				LEATHER_ARMOR, /* randart2 suit, heavily autocurses, 1 in 5000 per turn to give musehand effect, double mystery res, klara trap effect, and if you have an implant it counts as being handicapped so you get the stronger effects */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Claideamh",				FLOWING_SWORD,
	(SPFX_RESTR|SPFX_WARN|SPFX_DFLAG2), 0, (M2_ELF|M2_GIANT),
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Nodensfork",				SHOOMDENT,
	(SPFX_RESTR|SPFX_DEFN|SPFX_WARN), 0, 0,
	PHYS(10, 10), DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Gaia's Fate",				SICKLE,
	(SPFX_RESTR|SPFX_WARN), 0, 0,
	PHYS(10, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Atlantean Royal Sword",				TWO_HANDED_SWORD, /* randart two-handed sword */
	(SPFX_RESTR), 0, 0,
	PHYS(3, 0), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),

A("Jinja Naginata",				LOCHABER_AXE,
	(SPFX_RESTR), 0, 0,
	PHYS(4, 12), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1200L ),

A("Singing Sword",				LONG_SWORD, /* randart long sword */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	PHYS(1, 1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Green Dragon Crescent Blade",				PATINA_BAR, /* 20000 turns of respawn trap effect, deactivates reflection for 10k turns */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(1, 25), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1200L ),

A("The Pink Panther",				MORGANITE, /* trap revealing when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Lifehunt Scythe",				SCYTHE, /* drain res when wielded */
	(SPFX_RESTR|SPFX_BEHEAD|SPFX_STLTH|SPFX_DEFN), 0, 0,
	PHYS(6, 6), DFNS(AD_COLD), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Holy Moonlight Sword",				LONG_SWORD, /* randart2 long sword */
	(SPFX_RESTR), 0, 0,
	PHYS(12, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Bloodletter",				JAGGED_STAR, /* dmg h@ck, +d12 bleeding damage */
	(SPFX_RESTR), 0, 0,
	PHYS(8, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Fallingstar Mandibles",				PINCER_STAFF,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	MAGM(12, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Callandor",				CRYSTAL_SWORD, /* spellboost when wielded */
	(SPFX_RESTR|SPFX_HSPDAM|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 6660L ),

A("The Unstoppable",				CROSSBOW, /* randart2 crossbow */
	(SPFX_RESTR), 0, 0,
	PHYS(12, 8), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("Epoch's Curve",				WIND_BOW,
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	PHYS(5, 1), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Cloak of the Unheld One",				OILSKIN_CLOAK, /* free action and sleep res when worn, rang call trap effect */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4500L ),

A("Cloak of the Unheld Potato",				OILSKIN_CLOAK, /* randart2 cloak, free action and sleep res when worn, rang call trap effect */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4500L ),

A("Soulmirror",				LANTERN_PLATE_MAIL,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 9000L ),

A("Scorpion Carapace",				HARMONIUM_SCALE_MAIL,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRST), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("The Golden Knight",				BUCKLER, /* +5 STR */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("SevenLeagueBoots",				JUMPING_BOOTS, /* ultra fast speed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Frost Treads",		 		BOOTS_OF_SAFEGUARD,
	(SPFX_RESTR|SPFX_DEFN|SPFX_ATTK), 0, 0,
	COLD(1, 8), DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Grappler's Grasp",				GAUNTLETS_OF_SWIMMING, /* prevents monsters from grabbing you */
	(SPFX_RESTR), 0, 0,
	PHYS(1, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Godhands",				SUPERPOWER_GAUNTLETS,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(20, 10), DFNS(AD_DRLI), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 7777L ),

A("Wrappings of the Sacred Fist",				GAUNTLETS_OF_PHYSICAL_SUSTENAN,
	(SPFX_RESTR), 0, 0,
	PHYS(8, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Helm of Undeath",				WAR_HAT, /* life saving */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2500L ),

A("Apotheosis Veil",				HELM_OF_HOLDING, /* see invis, reduces hunger when spellcasting */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Enforced Mind",				ANTIMINDFLAY_HELMET, /* resist sleep and conf */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2500L ),

A("Ring of Thror",				RIN_UPPER_ADORNMENT, /* aggravate monster, +2 increase damage */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Narya",				RIN_ADORNMENT, /* randart ring, attacking in melee has 1 in 20 chance to fire a d4-strength blast of fire */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Vilya",				RIN_ADORNMENT, /* randart2 ring, magical breathing */
	(SPFX_RESTR|SPFX_DEFN|SPFX_WARN|SPFX_STLTH), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Lomya",				RIN_ADORNMENT, /* randart2 ring, teleportitis */
	(SPFX_RESTR|SPFX_EVIL|SPFX_SEARCH|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Mantle of Wrath",				ORCISH_CLOAK, /* aggravate monster, acid res */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HSPDAM|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 2500L ),

A("Heart of Shadow",				MOON_AXE,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(20, 10), DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6660L ),

A("Shard from Morgoth's Crown",				RIN_ADORNMENT, /* randart ring, res shock */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Infinity's Mirrored Arc",				CYAN_DOUBLE_LIGHTSABER,
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("Staff of Twelve Mirrors",				SILVER_KHAKKHARA, /* displacement */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("Sesderiro",				LONG_SWORD, /* randart2 melee weapon, displacement */
	(SPFX_RESTR|SPFX_REFLECT), 0, 0,
	PHYS(5, 6), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("The Mirrored Mask",				HELM_OF_MYSTERY_RES,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("Sansara Mirror",				MIRRORBLADE,
	(SPFX_RESTR|SPFX_REFLECT|SPFX_HPHDAM), 0, 0,
	PHYS(8, 8), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("Gleldreral",				LONG_SWORD, /* randart2 melee weapon */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_HPHDAM), 0, 0,
	PHYS(8, 8), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("The Spell-warded Wrappings of Nitocris",				MUMMY_WRAPPING, /* antimagic shell */
	(SPFX_RESTR|SPFX_EVIL|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Blungnir",				ATGEIR,
	(SPFX_RESTR), 0, 0,
	PHYS(20, 12), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 8000L ),

A("Hermes's Sandals",				SPEED_HOLDING_BOOTS, /* makes you very fast */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Poseidon's Trident",				TRIDENT, /* randart2 trident */
	(SPFX_RESTR), 0, 0,
	PHYS(3, 0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Feng Huo Lun",				WIND_AND_FIRE_WHEELS, /* very fast speed, flying, heavy status effects */
	(SPFX_RESTR|SPFX_ATTK|SPFX_EVIL), 0, 0,
	FIRE(1, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4500L ),

A("Jin Gang Zuo",				IRON_CHAIN, /* randart2 chain, waterproofs your inventory while wielded */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4500L ),

A("Ruyi Jingu Bang",				BO_STAFF,
	(SPFX_RESTR), 0, 0,
	PHYS(1, 0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4500L ),

A("Sickle of Thunderblasts",				SICKLE,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(1, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4500L ),

A("The War-helm of the Dreaming",				FACELESS_HELM, /* magical breathing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4500L ),

A("Spear of Peace",				INKA_SPEAR, /* resist stun and confusion */
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_HSPDAM|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),

A("Ibite arm",				CUDGEL,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ACID(20, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4500L ),

A("Star-emperor's Ring",				RIN_ADORNMENT, /* randart2 ring, 1 in 1000 to light up the area around you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("The Eye of the Oracle",				EYEBALL,
	(SPFX_RESTR|SPFX_HSPDAM|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 500L ),

A("The Macuahuitl of Quetzalcoatl",				MACUAHUITL,
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 3000L ),

A("The Mask of Tlaloc",				STANDARD_HELMET, /* randart2 helmet, cold and shock resistance and waterproofs your inventory when worn but traps become invisible when triggered */
	(SPFX_RESTR|SPFX_HSPDAM|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 2500L ),

A("Ehecailacocozcatl",				AMULET_OF_CLEAR_MIND,
	(SPFX_RESTR|SPFX_EREGEN|SPFX_REFLECT), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 3000L ),

A("Amhimitl",				JAVELIN, /* randart javelin */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(5, 8), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 3000L ),

A("The Tecpatl of Huhetotl",				TECPATL, /* if you kill something while wielding it, reduces prayer timeout by 50 */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	PHYS(5, 8), DFNS(AD_DRLI), NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 3000L ),

A("The Star of Hypernotus",				AMULET_VERSUS_CURSES, /* resist confusion */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2500L ),

A("Avenger",				LONG_SWORD, /* randart long sword, drain res */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(1, 7), DFNS(AD_MAGM), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("The Key of Erebor",				SKELETON_KEY, /* works as a climbing set when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1500L ),

A("The Death-Spear of Keptolo",				DROVEN_SPEAR,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(10, 12), DFNS(AD_MAGM), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Esscooahlipboourrr",				BASTERD_SWORD, /* heap trap while wielded */
	(SPFX_RESTR|SPFX_SEARCH|SPFX_SEEK|SPFX_DEFN|SPFX_EVIL), 0, 0,
	PHYS(20, 10), DFNS(AD_DRLI), NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("The Robe of Closed Eyes",				ROBE, /* randart2 robe, invis, 75% chance to resist gazes */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("The Red Cords of Ilmater",				REGULAR_GLOVES, /* randart gloves, free action, stone resist */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("The Crown of the Percipient",				GOOD_ESP_HELMET, /* detect monsters, resist drain, stone, fire, cold, shock, sleep, magic and conf, sticky prime autocurses and carries an ancient morgothian curse */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HALRES), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Painting Fragment",				SCR_REVERSE_IDENTIFY, /* allows you to reverse identify d5 more items */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("The Profaned Greatscythe",				GIANT_SCYTHE, /* heavily autocurses, bad alignment trap, prayer timeout goes up instead of down, SOL effect, ruth trap effect and +20 spawn freq for AD_FEMI */
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_EVIL), 0, 0,
	FIRE(8, 30), DFNS(AD_FIRE), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Friede's Scythe",				GRAIN_SCYTHE,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	MAGM(6, 6), DFNS(AD_MAGM), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Yorshka's Spear",				HEAVY_SPEAR, /* +6 STR, DEX and WIS */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Dragonhead Shield",				STONE_DRAGON_SCALE_SHIELD, /* resist disintegration */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 4000L ),

A("Crucifix of the Mad King",				HALBERD, /* makes areas dark if it hits something, like collusion knife */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	PHYS(1, 10), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Ringed Brass Armor",				ROUND_MAIL, /* 5 extra points of AC, or 10 if you're chaotic */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Ritual Ringed Spear",				RANDOSPEAR, /* to-hit h@ck */
	(SPFX_RESTR), 0, 0,
	PHYS(1, 0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("Velka's Rapier",				ARCANE_RAPIER,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	MAGM(20, 0), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("The Dragon's Heart-Stone",				BONE_FRAGMENT,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(10, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 40L ),

A("The Steel Scales of Kurtulmak",				GRAY_DRAGON_SCALES, /* free action, slows you twice as much as spirit */
	(SPFX_RESTR|SPFX_REFLECT|SPFX_HPHDAM|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_FIRE), NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 5000L ),

A("The Glitterstone",				AMBER_FRAGMENT,
	(SPFX_RESTR|SPFX_EREGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 80L ),

A("Great Claws of Urdlen",				GAUNTLETS_OF_FISTFIGHTING,
	(SPFX_RESTR), 0, 0,
	PHYS(10, 10), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),

A("The Chromatic Dragon Scales",				BLACK_DRAGON_SCALES, /* res fire, cold, shock, poison, sick, acid and stone, heavily autocurses and fills you with the black breath */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 9000L ),

A("The Eye of Vecna",				EYEBALL,
	(SPFX_RESTR|SPFX_HSPDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 500L ),

A("Shadowlock",				RAPIER, /* bosses aren't uncommon while you're wielding it */
	(SPFX_RESTR|SPFX_BEHEAD|SPFX_EVIL), 0, 0,
	PHYS(20, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 9999L ),

A("Ramithaine",				FLEURE,
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	PHYS(6, 1), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 3333L ),

A("Thorns",				RAZOR_WHIP, /* poisons opponents */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(1, 10), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 6660L ),

A("Arrow of Slaying",				SILVER_ARROW,
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	PHYS(20, 0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 77L ),

A("The Trusty Adventurer's Whip",				BULLWHIP, /* randart2 whip */
	(SPFX_RESTR), 0, 0,
	PHYS(20, 4), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Log of the Curator",				CLUB, /* randart2 club */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Fedora of the Investigator",				FEDORA,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Force Pike of the Red Guard",				FORCE_PIKE,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	MAGM(1, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Gauntlets of the Berserker",				REGULAR_GLOVES, /* randart2 gloves, +5 STR and DEX, +10 CON */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Declaration of the Apostate",				SCR_REMOVE_CURSE, /* reading sets training in all restricted skills to zero */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Soul Lens",				LENSES,
	(SPFX_RESTR|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Seal of the Spirits",				SCR_SKILL_GROWTH, /* reading trains every skill by d5 points */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Torch of Origins",				WAN_FIRE,
	(SPFX_RESTR|SPFX_BEHEAD|SPFX_DEFN|SPFX_ATTK), 0, 0,
	FIRE(1, 5), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Striped Shirt of the Murderer",				STRIPED_SHIRT, /* +1000 carry cap and max carry cap, allows you to backstab opponents */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Striped Shirt of the Thief",				STRIPED_SHIRT, /* protects from theft */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Striped Shirt of the Falsely Accused",				STRIPED_SHIRT, /* double discount action, resist sleep */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Scalpel of Life and Death",				SCALPEL, /* to-hit h@ck */
	(SPFX_RESTR), 0, 0,
	PHYS(1, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Gauntlets of the Healing Hand",				REGULAR_GLOVES, /* randart2 gloves, also gives intrinsic regen */
	(SPFX_RESTR|SPFX_REGEN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Ring of Hygiene's Disciple",				RIN_REGENERATION, /* also gives intrinsic regen */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Cope of the Eldritch Knight",				ROBE, /* randart2 robe, spells lose memory 50% more slowly */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Shield of the Paladin",				SHIELD, /* randart2 shield, warns of undead and demons */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Booze of the Drunken Master",				POT_BOOZE, /* gives 20k turns of drunken boxing when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Khakkhara of the Monkey",				SILVER_KHAKKHARA, /* jumping */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	STUN(1, 1), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Ruffled Shirt of the Aristocrat",				RUFFLED_SHIRT, /* see invis, resist sleep and poison */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Victorian Underwear of the Aristocrat",				VICTORIAN_UNDERWEAR, /* double magic find */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Mark of the Rightful Scion",				RIN_ADORNMENT, /* randart2 ring */
	(SPFX_RESTR|SPFX_WARN|SPFX_DFLAG2), 0, M2_ORC,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Gauntlets of the Divine Disciple",				ORIHALCYON_GAUNTLETS, /* +3 AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Mace of the Evangelist",				MACE, /* randart2 mace */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Dart of the Assassin",				DART, /* randart2 dart, becomes poisoned when thrown */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("The Sword of the Kleptomaniac",				SHORT_SWORD, /* randart2 short sword */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5, 5), DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Helm of the Arcane Archer",				STANDARD_HELMET, /* randart2 helmet, +1 multishot with bows */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Figurine of Pygmalion",				FIGURINE, /* stone res when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Figurine of Galatea",				FIGURINE, /* immunity to AD_SSEX when wielded */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Helm of the Ninja",				HELM_OF_OPPOSITE_ALIGNMENT, /* if your current weapon can be poisoned, it becomes so */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Towel of the Interstellar Hitchhiker",				TOWEL, /* if you wield it and your other hand is wielding a whip, the latter deals +5 damage */
	(SPFX_RESTR|SPFX_EVIL|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Encyclopedia Galactica",				SPE_BLANK_PAPER, /* randart2 spellbook */
	(SPFX_RESTR|SPFX_LUCK|SPFX_WARN|SPFX_ESP), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Twig of Yggdrasil",				WAN_TELEPORTATION,
	(SPFX_RESTR|SPFX_TCTRL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Saddle of Brynhildr",				LEATHER_SADDLE, /* randart2 saddle, flying while riding */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Staff of Wild Magic",				QUARTERSTAFF, /* randart2 quarterstaff */
	(SPFX_RESTR|SPFX_ATTK|SPFX_LUCK|SPFX_SEARCH), 0, 0,
	MAGM(20, 6), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Forge Hammer of the Artificer",				WAR_HAMMER, /* randart2 hammer, double magic find */
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_UNDEAD,
	PHYS(5, 0), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Bulwark of the Dwarven Defender",				DWARVISH_ROUNDSHIELD,
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Hat of the Giant Killer",				GNOMISH_HELM,
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_WARN), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Prismatic Dragon Plate",				RED_DRAGON_SCALE_MAIL, /* randart DSM, petkeeping skill trains 3x as fast */
	(SPFX_RESTR|SPFX_HPHDAM), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Footprints in the Labyrinth",				LONG_SWORD, /* randart2 melee weapon, spellboost */
	(SPFX_RESTR), 0, 0,
	PHYS(20, 10), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Trappings of the Grave",				AMULET_OF_RESTFUL_SLEEP,
	(SPFX_RESTR|SPFX_EVIL|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("The Bright Aurora",				LOW_BOOTS, /* initialized to always be not-slowing-down boots, +2 sight range, makes you very fast */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 20000L ),

A("Pu-Pu",				GAUNTLETS_OF_FISTFIGHTING, /* 3x unarmed damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Get The Old Values Back",				SUPERPOWER_GAUNTLETS, /* +3 STR, +3 ranged to-hit */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Livin' It Up",				HELM_OF_HOLDING, /* double max carry cap */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Cumbersome Desc",				GOOD_ESP_HELMET, /* immune to erosion, +2 DEX, 1 extra point of AC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Also Not Inert",				SPEED_HOLDING_BOOTS, /* inertia times out very quickly */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Triangle Girl",				MESH_ARMOR, /* +1000 carry capacity and max carry cap, and if you're a female character of XL9 or lower you'll get an additional +4000 to both and +2 CHA plus protection from theft */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Cool Mug",				POT_WINE, /* quaffing gives +10000 turns of fire and cold resistance */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Most Hair-Raising Coffee Break Of All Time",				POT_COFFEE, /* +20 turns of invulnerability, gives you the respawn trap and verisiert trap effects for 2000 turns */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Seeyou,hon",				POT_WONDER, /* auto-identifies the potion it becomes */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Kodo Bango",				POT_TERCES_DLU, /* gives a digit of the secret code when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Make You Actually Hidden",				POT_HIDING, /* causes you to hide if quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Huggingvalk",				POT_DECOY_MAKING, /* summons a tame valkyrie when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Porta-Shaft",				POT_DOWN_LEVEL, /* sends you down like a shaft when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Plus One Line",				POT_CURE_WOUNDS, /* +1 max HP when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Plus Two Lines",				POT_CURE_SERIOUS_WOUNDS, /* +2 max HP when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Full Recovery",				POT_CURE_CRITICAL_WOUNDS, /* heals you back to full and gives 5 anti-corona effects */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Opacity",				POT_NIGHT_VISION, /* lasts 10x as long but also deactivates invisibility for 10k turns when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Elona Greeting",				POT_RESISTANCE, /* also gives temporary resistances to psi and death as well as free action */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Pregnancy Abort",				POT_POISON, /* heals 1000 points of contamination when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Restwell",				POT_RED_TEA, /* cures sickness when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 500L ),

A("Huck - ...",				POT_OOLONG_TEA, /* induces vomiting when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("Energizer Drink",				POT_COCOA, /* also grants temporary full nutrients when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Flush Particles Out",				POT_GREEN_MATE, /* reduces sanity by 1000 when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Tamana's Secret Drink",				POT_TERERE, /* permanent +1 increase damage when quaffed */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50000L ),

A("Burondo no on'nanoko no himitsu no nimotsu",				POT_AOJIRU, /* permanent +1 increase accuracy when quaffed */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 30000L ),

A("Coded Heroism",				POT_ULTIMATE_TSUYOSHI_SPECIAL, /* gainstr(10) and 5000 turns of resist confusion when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Anata wa shishidesu ka?",				POT_MEHOHO_BURUSASAN_G, /* +100 max HP and 50000 turns of cursed/doomed when quaffed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Lawfight",				SCR_COPYING, /* summons 8 random L around you when read */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Be careful. There everywhere.",				SCR_CREATE_VICTIM, /* summons rn1(25,25) additional monsters */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Kuroi fukei wa koryoto shite iru",				SCR_CREATE_FACILITY, /* creates 10x as much terrain */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Fuck You Dude!",				SCR_ERASURE, /* erases all monsters on the level who aren't ungenocidable */
	(SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 25000L ),

A("Oh, while we're at it...",				SCR_POWER_HEALING, /* also restores all mana */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Fuyo No Wandarando",				SCR_FROST, /* creates 100x as much ice terrain */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Fufufufufu, kekekekeke...",				SCR_CREATE_TRAP, /* also creates 20 random traps on the current level when read */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Clonescroll of no use",				SCR_CREATE_SINK, /* doesn't do anything special */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 50L ),

A("Bitches to you ese",				SCR_SYMMETRY, /* cures all feminism trap effects when read */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Pitch had.",				SCR_CREATE_CREATE_SCROLL, /* +1000 turns of nastiness upon generation */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 100L ),

A("Hijo Ni Hayai Warui",				SCR_DESTROY_WEAPON, /* causes 5 bad effects when read */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 200L ),

A("hie huehei fluemei sglwln",				BREAKING_WHEEL, /* 5000 turns of heavy confusion when wielded */
	(SPFX_RESTR|SPFX_EVIL|SPFX_HPHDAM|SPFX_REGEN), 0, 0,
	PHYS(0, 12), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("dnethackc noonishness",				SMALL_SHIELD, /* randart2 shield, heavy sticky autocurse, dnethack cloak effect, x-race RNG changes, double magic find and adds extra chances of finding artifacts on top of that */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 7000L ),

A("Nozzle Change",				LEAD_UNLOADER, /* fire mode changes after each salvo and cannot be set manually, poisons ammos */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Does the stecking dose go?",				STEEL_KNIFE, /* 1 in 1000 that its enchantment randomly goes up or down by one, with limits of +7 ... -20 */
	(SPFX_RESTR|SPFX_ATTK|SPFX_EVIL), 0, 0,
	STUN(5, 2), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Halt Shaman",				FLOWING_SWORD, /* spellboost */
	(SPFX_RESTR), 0, 0,
	PHYS(3, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("The Magyar Idea",				BULLWHIP, /* randart2 whip, killer spawn effect, autocurses and increases contamination and sanity by 100 every time you wield it */
	(SPFX_RESTR|SPFX_ATTK|SPFX_EVIL), 0, 0,
	FIRE(4, 20), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Draining Arrow",				WAN_MAGIC_MISSILE, /* fires an invisible beam that takes off a maximum hit point of every monster that got hit */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Real Force",				REGULAR_GLOVES, /* randart gauntlets, invoke for psycho blast */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Kristin's Sachtness",				SOFT_SNEAKERS, /* d4 passive acid damage to things meleeing you */
	(SPFX_RESTR|SPFX_HPHDAM|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_ACID), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Lisa's Yellow Stripe",				SOFT_SNEAKERS, /* acid immunity */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Lexi's Powerkick",				SOFT_SNEAKERS, /* +5 kick damage, +5 CON */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Tanja's Martial Prowess",				SOFT_SNEAKERS, /* 1% per martial arts skill that a kick paralyzes the monster for (1 + martial arts skill level) turns, and monsters cannot resist that effect */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 16000L ),

A("Bright White",				DANCING_SHOES, /* cold immunity, dark mode effect, monsters who melee you must make a saving throw or be blinded */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10000L ),

A("Juen's Weakness",				LEATHER_PEEP_TOES, /* kristina trap effect, +5 kick damage, bad alignment, super regene and +5 CHA */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 6000L ),

A("Ute's Greenchange",				LEATHER_PEEP_TOES, /* invoke gives them an item property or replaces the existing one, you can move over farmland and poison cannot instakill you */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 3000L ),

A("Fightboost",				COMBAT_STILETTOS, /* +5 kick damage, very fast speed */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Energeen's",				COMBAT_STILETTOS, /* fast speed, adds extra speed sometimes */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Giordana's Radiance",				ITALIAN_HEELS, /* +2 sight range */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Measurer Tip",				ITALIAN_HEELS, /* kicking deals +d(3 * knife skill) bleeding damage and trains the knife skill */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("naQ QeH",				HELM_OF_MADNESS, /* +3 CHA, autocurses */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Iris's Hidden Allergy",				LOW_BOOTS, /* initialized to always be silver filigreed stilettos */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),
/* each of these four iris artifacts makes it so that you're vulnerable to silver and for every worn silver item, you get
 * 3 extra AC, 10% reduced paralysis duration, 1% more spell memory when casting a spell, 5% reduced technique timeout
 * and 10% chance that training a skill trains it twice as much. */

A("Iris's Secret Vulnerability",				STANDARD_HELMET, /* initialized to always be echo helmet */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Iris's Unrevealed Love",				LEATHER_CLOAK, /* initialized to always be demonic cloak */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Iris's Favored Material",				REGULAR_GLOVES, /* initialized to always be spiky gloves */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),
/* if you're wearing several of these Iris artifacts, which is almost impossible unless you wish for them, you get:
 * 2 artifacts - double reflection
 * 3 artifacts - double magic resistance
 * 4 artifacts - double astral vision
 * take note that this does not include the "Iris's Precious Metal" artifact, this is by design --Amy */

A("Really Wanna See Blood!",				STEEL_KNIFE, /* +rn1(11,11) bleeding damage */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_MURDERER, NON_PM, 4000L ),

A("Jana's Mud Obsession",				DANCING_SHOES, /* 10000 turns of jana trap effect, hard to destruct and kicking a monster has 5% chance to fire a strength 6 acid ray */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Jenny Supersoft",				LADY_BOOTS, /* jennifer trap effect, stun resistance and prevents fumbling */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("People Dumbing",				STANDARD_HELMET, /* randart2 helmet, trap revealing and 1 in 20 that the revealed trap is no longer invisible if it was before */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_DEMAGOGUE, NON_PM, 4000L ),

A("Bangbangbang",				STORM_RIFLE,
	(SPFX_RESTR), 0, 0,
	PHYS(6, 3), NO_DFNS, NO_CARY, 0, A_NONE, PM_SPACE_MARINE, NON_PM, 3500L ),

A("Anti-Animal Bar",				DRAGON_SPEAR, /* +3 AC, and if you're a gladiator, your armor pieces are 50% less likely to dull */
	(SPFX_RESTR|SPFX_DFLAG1), 0, M1_ANIMAL,
	PHYS(2, 12), NO_DFNS, NO_CARY, 0, A_NONE, PM_GLADIATOR, NON_PM, 2000L ),

A("Fyryoni",				RIN_REGENERATION, /* aggravate monster, magic find, autocurses */
	(SPFX_RESTR|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_DRLI), NO_CARY, 0, A_NONE, PM_GLADIATOR, NON_PM, 2500L ),

A("Dressing Em",				MESH_SADDLE, /* ridden monster regenerates an additional HP per turn */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_LADIESMAN, NON_PM, 4000L ),

A("Irma's Choice",				LOW_BOOTS, /* initialized to always be copper stilettos, 10% chance to nullify incoming damage */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	STUN(2, 2), NO_DFNS, NO_CARY, 0, A_NONE, PM_LIBRARIAN, NON_PM, 7000L ),

A("Antiluckblade",				BLADE_OF_GRACE, /* if your luck is negative, to-hit and damage bonuses are actually equal to the absolute value of your luck */
	(SPFX_RESTR), 0, 0,
	PHYS(0, 1), NO_DFNS, NO_CARY, 0, A_NONE, PM_MURDERER, NON_PM, 2500L ),

A("The Mage Staff of Eternity",				QUARTERSTAFF, /* randart2 quarterstaff, see invis, infravision, boosts INT and WIS by its enchantment if positive, spellboost, -d20 melee to-hit, sticky autocurses; if you're not an unbeliever, +1000 sanity every time you wield it */
	(SPFX_RESTR|SPFX_LUCK|SPFX_ESP|SPFX_EREGEN|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, PM_UNBELIEVER, NON_PM, 20000L ),

A("The Long Sword of Eternity",				LONG_SWORD, /* randart2 long sword, +10 CON, cold immunity, free action, heavily autocurses, antimagic shell, see invisible, slow digestion; if you're not an unbeliever, +1000 sanity every time you wield it */
	(SPFX_RESTR|SPFX_EVIL|SPFX_LUCK|SPFX_BEHEAD|SPFX_ATTK|SPFX_DEFN|SPFX_REGEN), 0, 0,
	ELEC(0, 10), DFNS(AD_FIRE), NO_CARY, 0, A_NONE, PM_UNBELIEVER, NON_PM, 20000L ),

A("The Heavy Crossbow of Eternity",				CROSSBOW, /* randart2 crossbow, heavy sticky autocurse, antimagic shell, see invisible, slow digestion, free action, very fast speed, +10 DEX, flying, +2 multishot, shock immunity, invis, resist blind and confusion, infravision; if you're not an unbeliever, +1000 sanity every time you wield it */
	(SPFX_RESTR|SPFX_EVIL|SPFX_LUCK|SPFX_REFLECT|SPFX_STLTH), 0, 0,
	PHYS(0, 16), NO_DFNS, NO_CARY, 0, A_NONE, PM_UNBELIEVER, NON_PM, 20000L ),

A("Spiked Barding",				BARDING, /* prevents engulfing */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("Plated Barding",				BARDING, /* +2 AC for player */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2500L ),

A("Senator's Speed",				BARDING, /* you and the steed are immune to slowing effects "%s doesn't slow down!" */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("GLAE",				LONG_SWORD, /* randart melee weapon, invoke rerolls base item */
	(SPFX_RESTR), 0, 0,
	PHYS(5, 10), NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 4000L ),

A("IGTOS Corp, Inc.",				LONG_SWORD, /* randart2 melee weapon, initialized d8 to-hit, d8 damage, random elemental attack */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	PHYS(8, 8), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Postscheme M",				LONG_SWORD, /* randart melee weapon, initialized d11 to-hit, d15 damage, random elemental attack */
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	PHYS(11, 15), NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Angular Sign",				KITE_SHIELD, /* resist fire, cold, shock and poison */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Angular Angularity",				KITE_SHIELD, /* 5 extra AC and +1 MC */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("The Shield of the Chevalier, who is not called Paladin regardless of what the type of ice block thinks",	KITE_SHIELD, /* warns of undead and demons */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	NO_ATTK, DFNS(AD_MAGM), NO_CARY, 0, A_NONE, NON_PM, NON_PM, 5000L ),

A("BacklashProtect",				SPE_RETURN, /* gives +400% spell memory when read */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 8000L ),

A("Ranofrit",				SCR_RETURN, /* can be invoked for return, 1 in 10 chance that doing so uses up the scroll */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 8000L ),

A("Do stay a while!",				SCALE_MAIL, /* can be invoked for return, but doing so has 1 in 10 chance that the armor is used up and if it's not, it becomes heavily cursed and gains +1 up to a maximum of +20 */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_NONE, NON_PM, NON_PM, 8000L ),

A("No Little Bit Of Wisdom",				HEEL_WISE_SHOES, /* sustain ability */
	(SPFX_RESTR|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Infection Chain Break",				DISINFECTANT_GLOVES, /* resist poison and sickness */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 4000L ),

A("Nightly Highway",				HYPERSONIC_GLOVES, /* if it's nighttime, you have fast speed, otherwise you can hide underneath items */
	(SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 2000L ),

/* place new artifacts above this, and NOGEN or otherwise "special" artifacts below --Amy */

A("Wendyhole",				BOULDER, /* 1 in 10000 randomly generated boulders should be this; if you push it, the boulder farts */
	(SPFX_NOGEN|SPFX_RESTR), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 10L ),

A("The Red Sword",			FLAME_MOUNTAIN,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_ATTK), 0, 0,
	FIRE(5, 12),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 4500L ),

A("Siegfried's Deathbolt",			CROSSBOW_BOLT, /* if it hits you, you're disintegrated */
	(SPFX_NOGEN|SPFX_RESTR), 0, 0,
	PHYS(0, 20),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 100L ),

A("Thiefbane",                  LONG_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_BEHEAD|SPFX_DCLAS|SPFX_DRLI|SPFX_NOWISH), 0, S_HUMAN,
	DRLI(5,2),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("Scales of the Dragon Lord",	GRAY_DRAGON_SCALE_MAIL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_PROTEC), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	DRAGON_BREATH,	A_CHAOTIC,	NON_PM, NON_PM, 5000L ),

A("The Key Of Access",		SKELETON_KEY,
	(SPFX_RESTR|SPFX_NOGEN),	0,		0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	CREATE_PORTAL,	A_NONE,		NON_PM, NON_PM, 200L ),

/*** Special Artifacts ***/

/* nighthorn can generate randomly after defeating nightmare */
A("Nighthorn", UNICORN_HORN,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_REFLECT|SPFX_NOWISH), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,      A_LAWFUL, NON_PM, NON_PM, 10000L ),

A("The Key of Law", SKELETON_KEY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_NOWISH), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,      A_LAWFUL, NON_PM, NON_PM, 1000L ),

/* eye of the beholder can generate randomly after defeating beholder */
A("The Eye of the Beholder", EYEBALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	DEATH_GAZE,     A_NEUTRAL, NON_PM, NON_PM, 500L ),

A("The Key of Neutrality", SKELETON_KEY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_NOWISH), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,      A_NEUTRAL, NON_PM, NON_PM, 1000L ),

/* hand of vecna can generate randomly after defeating vecna */
A("The Hand of Vecna",       SEVERED_HAND,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_REGEN|SPFX_HPHDAM|SPFX_NOWISH), 0, 0,
	NO_ATTK,        DRLI(0,0),      CARY(AD_COLD),
	SUMMON_UNDEAD,          A_CHAOTIC, NON_PM, NON_PM, 700L ),

A("The Key of Chaos", SKELETON_KEY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_NOWISH), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,      A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("The Gauntlet Key", SKELETON_KEY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_NOWISH), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,      A_LAWFUL, NON_PM, NON_PM, 1000L ),

/* reward for defeating the elder priest, can generate randomly after you both defeated an elder priest and ascended */
A("Motherfucker Trophy", ROBE, /* +5 increase damage and accuracy, 20% better spellcasting chances, fast skill training */
	(SPFX_NOGEN|SPFX_DEFN|SPFX_RESTR|SPFX_INTEL|SPFX_REFLECT|SPFX_NOWISH), 0, 0,
	NO_ATTK,        DFNS(AD_MAGM),        NO_CARY,
	0,      A_NONE, NON_PM, NON_PM, 50000L ),

/* for Emyn Luin boss - no special effects, cannot be wished for, can generate randomly after defeating kalwina */
A("Kal",			KALASHNIKOV,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 2500L ),

/* for Green Cross boss - cannot be wished for, can generate randomly after defeating stahngnir */
A("Stahngnir's Bashhunk", AIR_PRESSURE_HAMMER, /* petrification resistance when wielded */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_NOWISH), 0, 0,
	PHYS(5, 0),	NO_DFNS, NO_CARY,
	0,	A_NONE, NON_PM, NON_PM, 20000L ),

/* three artifacts for elemental imbue technique, cannot be wished for */
A("Fire Swing",			LONG_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_NOWISH|SPFX_ATTK), 0, 0,
	FIRE(0,8),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Frost Swing",			LONG_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_NOWISH|SPFX_ATTK), 0, 0,
	COLD(0,8),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 2000L ),

A("Shock Swing",			LONG_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_NOWISH|SPFX_ATTK), 0, 0,
	ELEC(0,8),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 2000L ),

/* procomfort super can generate randomly after completing the quest */
A("ProComfort Super",			RED_STRING,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_NOWISH|SPFX_REGEN), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, PM_CLIMACTERIAL, NON_PM, 6000L ),


/* reward for finishing the Illusory Castle, can generate randomly after beating the motherfucker glass golem */
A("The Helm of Knowledge", HELM_OF_TELEPATHY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	IDENTIFY,	A_NONE, NON_PM, NON_PM, 10000L),

/* artifact on the special level in the Illusory Castle, can generate randomly after beating the motherfucker glass golem */
A("Boots of the Machine", SPEED_BOOTS, /* aggravate monster, confusion resistance, displays all golems and unliving monsters */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_NOWISH|SPFX_EVIL), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,	A_NONE, NON_PM, NON_PM, 10000L),

/* artifact on the special level in the Deep Mines, can generate randomly after beating the mines or ascending */
A("The Arkenstone of Thrain", LUCKSTONE, /* invokes for perilous identify - curses the stone and gives 10 bad effects */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	IDENTIFY,	A_NONE, NON_PM, NON_PM, 10000L),

/* reward for finishing the Mainframe, can generate randomly after beating the bofh */
A("Bizarro Orgasmatron", HITCHHIKER_S_GUIDE_TO_THE_GALA,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	CREATE_PORTAL,	A_NONE, NON_PM, NON_PM, 10000L),

/* reward for finishing Hell's Bathroom, can generate randomly after beating katia */
A("Katia's Soft Cotton", BATH_TOWEL, /* if you take a crap while wearing it, you may gain charisma */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_NOWISH), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,	A_NONE, NON_PM, NON_PM, 25000L),



/*** The artifacts for the quest dungeon, all self-willed ***/

/* Amy note: The Orb of Detection MUST be the first one!!! see foreignartifactcount() in invent.c
 * these can generate randomly if you completed either both the regular and rival quest, or picked up the AoY */

A("The Orb of Detection",	CRYSTAL_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_ESP|SPFX_HSPDAM), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	INVIS,		A_LAWFUL, PM_ARCHEOLOGIST, NON_PM, 2500L ),

A("The Ball of Light",	CRYSTAL_BALL,
	(SPFX_NOGEN|SPFX_DEFN|SPFX_RESTR|SPFX_INTEL), (SPFX_ESP|SPFX_HSPDAM), 0,
	NO_ATTK,	DFNS(AD_BLND),	CARY(AD_MAGM),
	LIGHT_AREA,		A_LAWFUL, PM_ERDRICK, NON_PM, 2500L ),

A("The Heart of Ahriman",	LUCKSTONE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), SPFX_STLTH, 0,
	/* this stone does double damage if used as a projectile weapon */
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	LEVITATION,	A_NEUTRAL, PM_BARBARIAN, NON_PM, 2500L ),

A("The Heart of Other Ahriman",	LUCKSTONE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), SPFX_STLTH, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	LEVITATION,	A_NEUTRAL, PM_NOOB_MODE_BARB, NON_PM, 2500L ),

A("The Arkenstone",	DIAMOND,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), SPFX_ESP, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	HEALING,	A_LAWFUL, PM_MIDGET, NON_PM, 2500L ),

A("The Sceptre of Might",	MACE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DALIGN), 0, 0,
	PHYS(3,6),      NO_DFNS,        CARY(AD_MAGM),
	CONFLICT,	A_LAWFUL, PM_CAVEMAN, NON_PM, 2500L ),

A("Mysterious Spikes",	TAIL_SPIKES,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DALIGN|SPFX_XRAY), 0, 0,
	PHYS(30,60),      NO_DFNS,        CARY(AD_MAGM),
	HEALING,	A_LAWFUL, PM_MYSTIC, NON_PM, 200000L ),

A("The Iron Ball of Liberation", HEAVY_IRON_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL),
		(SPFX_STLTH|SPFX_SEARCH|SPFX_WARN), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	PHASING,	A_NEUTRAL, PM_CONVICT, NON_PM, 5000L ),

A("Pitch Blade",			BROADSWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_TCTRL|SPFX_INTEL),0,0,
	PHYS(5,6),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, PM_MURDERER, NON_PM, 3000L ),

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
	PHYS(5,16),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, PM_ZOOKEEPER, NON_PM, 800L ),

A("Dragonclan Sword",                KATANA,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_BEHEAD|SPFX_INTEL), 0, 0,
	PHYS(3,20),      NO_DFNS,        NO_CARY,        0, A_LAWFUL, PM_NINJA, NON_PM, 5000L ),

A("Killing Edge",                KATANA,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_BEHEAD|SPFX_INTEL), 0, 0,
	PHYS(3,6),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_ASSASSIN, NON_PM, 5000L ),

A("Black Death",                     HALBERD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DRLI), 0, 0,
	DRLI(5,10),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_UNDERTAKER, NON_PM, 3000L ),

A("Slow Blade",                ORCISH_SHORT_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_SEARCH|SPFX_REGEN), 0,
	PHYS(2,2),      NO_DFNS,        NO_CARY,        HEALING, A_LAWFUL, PM_ACID_MAGE, NON_PM, 1000L ),

A("Fire-Brigade Reel",                RUBBER_HOSE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK), (SPFX_HSPDAM|SPFX_HPHDAM), 0,
	COLD(4,8),      NO_DFNS,        NO_CARY,        SUMMON_WATER_ELEMENTAL, A_LAWFUL, PM_FIREFIGHTER, NON_PM, 2000L ),

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

A("Whistle of Protection",          DARK_MAGIC_WHISTLE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_EREGEN), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_MAGM),
	ENERGY_BOOST,         A_CHAOTIC, PM_OCCULT_MASTER, NON_PM, 2000L ),

A("Blade of Gothmog",          GRANITE_IMPALER, /* heavily autocurses */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_ATTK), 0, 0,
	FIRE(13,14),        DFNS(AD_FIRE),        NO_CARY,
	SUMMON_FIRE_ELEMENTAL,         A_CHAOTIC, PM_CHAOS_SORCEROR, NON_PM, 10000L ),

A("Beam Multiplier",          T_SHIRT, /* england mode when worn, resist fire/cold/shock */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_EVIL), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,         A_NEUTRAL, PM_ELEMENTALIST, NON_PM, 6000L ),

A("Elli's Pseudoband of Pos",          LONG_SWORD, /* randart */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_REFLECT|SPFX_DRLI), 0, 0,
	DRLI(8,8),        NO_DFNS,        NO_CARY,
	0,         A_CHAOTIC, PM_WILD_TALENT, NON_PM, 6000L ),

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
	PHYS(5,8),        DFNS(AD_DRST),  NO_CARY,        0, A_CHAOTIC, PM_GANGSTER, NON_PM, 6000L ),

A("Shiny Magnum", PISTOL,	/* KMH */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_DEFN|SPFX_INTEL), 0, 0,
	PHYS(10,16),        DFNS(AD_DETH),  NO_CARY,        0, A_LAWFUL, PM_OFFICER, NON_PM, 10000L ),

A("Withered Nine Millimeter", PISTOL,	/* KMH */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_DEFN|SPFX_INTEL), 0, 0,
	PHYS(5,8),        NO_DFNS,  NO_CARY,        0, A_NEUTRAL, PM_COURIER, NON_PM, 3000L ),

A("Training SMG", SUBMACHINE_GUN,	/* KMH */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_DEFN|SPFX_INTEL), 0, 0,
	PHYS(5,8),        NO_DFNS,  NO_CARY,        ENLIGHTENING, A_NEUTRAL, PM_INTEL_SCRIBE, NON_PM, 3000L ),

A("Charged USB stick", INSECT_SQUASHER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,        NO_DFNS,  NO_CARY,        CHARGE_OBJ, A_NEUTRAL, PM_WANDKEEPER, NON_PM, 3000L ),

A("Verbal Blade",		ELVEN_SHORT_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_BEHEAD), 0, 0,
	PHYS(5,2),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, PM_ZYBORG, NON_PM, 3000L ),

A("Tome Dark Sword",		BLACK_AESTIVALIS,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HSPDAM|SPFX_HPHDAM|SPFX_ESP|SPFX_STLTH), 0, 0,
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
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HSPDAM|SPFX_HPHDAM|SPFX_EVIL), SPFX_EREGEN, 0,
	NO_ATTK,        DRLI(0,0),        NO_CARY,    IDENTIFY, A_NEUTRAL, PM_TRANSSYLVANIAN, NON_PM, 5000L ),

A("Golden High Heels",    LEATHER_PEEP_TOES,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), SPFX_STLTH, 0,
	NO_ATTK,        ACID(0,0),        NO_CARY,    LEVITATION, A_NEUTRAL, PM_TOPMODEL, NON_PM, 5000L ),

A("Unobtainable Beauties",    SENTIENT_HIGH_HEELS,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_WARN|SPFX_REFLECT|SPFX_SPEAK|SPFX_EVIL), SPFX_REGEN, 0,
	NO_ATTK,        ACID(0,0),        NO_CARY,    HEALING, A_NEUTRAL, PM_FAILED_EXISTENCE, NON_PM, 5000L ),

A("Activist Stick",			FLY_SWATTER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, PM_ACTIVISTOR, NON_PM, 4800L ),

A("The Eyes of the Overworld",	LENSES,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_XRAY), 0, 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	ENLIGHTENING,	A_NEUTRAL,	 PM_MONK, NON_PM, 2500L ),

A("The Eyes of the Half-Baked World",	LENSES,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_XRAY), 0, 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	ENLIGHTENING,	A_NEUTRAL,	 PM_HALF_BAKED, NON_PM, 2500L ),

A("Gauntlets of Offense",    GAUNTLETS_OF_POWER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), SPFX_HPHDAM, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,    INVIS, A_NEUTRAL, PM_PSION, NON_PM, 5000L ),

A("the Pen of the Void",	ATHAME,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	0,	A_NEUTRAL, PM_BINDER, NON_PM, 2500L),

A("Bloody Beauty",	BLOCK_HEELED_COMBAT_BOOT,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DRLI), 0, 0,
	DRLI(4,4),	NO_DFNS,	NO_CARY,
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
	DRLI(8,4),      NO_DFNS,        CARY(AD_MAGM),
	ENERGY_BOOST,   A_CHAOTIC, PM_NECROMANCER, NON_PM, 50000L ),

A("The Mitre of Holiness",	HELM_OF_BRILLIANCE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_DFLAG2|SPFX_INTEL), 0, M2_UNDEAD,
	NO_ATTK,	NO_DFNS,	CARY(AD_FIRE),
	ENERGY_BOOST,	A_LAWFUL, PM_PRIEST, NON_PM, 2000L ),

A("Painkiller",        STEEL_WHIP,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DFLAG2), 0, M2_HUMAN,
	PHYS(12,24),    NO_DFNS,        CARY(AD_MAGM),
	0,              A_CHAOTIC, PM_CRUEL_ABUSER, NON_PM, 5000L ),

A("Dragon Whip",                  BULLWHIP,
	(SPFX_RESTR|SPFX_NOGEN|SPFX_DEFN|SPFX_INTEL|SPFX_ATTK|SPFX_DFLAG2),0,M2_DOMESTIC,
	PHYS(5,10),	DFNS(AD_FIRE),	NO_CARY,	DRAGON_BREATH, A_CHAOTIC, PM_SLAVE_MASTER, NON_PM, 4000L ),

A("Rupturer",                  RUNESWORD,
	(SPFX_RESTR|SPFX_DEFN|SPFX_NOGEN|SPFX_WARN|SPFX_INTEL|SPFX_ATTK),0,0,
	PHYS(3,14),	DFNS(AD_COLD),	NO_CARY,	DRAGON_BREATH, A_CHAOTIC, PM_BLOODSEEKER, NON_PM, 4000L ),

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
	PHYS(8,16),    NO_DFNS,        NO_CARY,
	0,              A_NEUTRAL, PM_BULLY, NON_PM, 5000L ),

A("Kitchen Cutter",        KNIFE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DFLAG2), 0, M2_STRONG,
	PHYS(6,12),    NO_DFNS,        NO_CARY,
	0,              A_NEUTRAL, PM_COOK, NON_PM, 3000L ),

A("Archon Staff",        BATTLE_STAFF,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DFLAG2), SPFX_EREGEN, M2_DEMON,
	PHYS(10,20),    NO_DFNS,        NO_CARY,
	OBJ_DETECTION,              A_LAWFUL, PM_AUGURER, NON_PM, 3000L ),

A("Shillelagh",        BATTLE_STAFF,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DFLAG2), SPFX_EREGEN, M2_NASTY,
	PHYS(16,8),    NO_DFNS,        NO_CARY,
	LIGHT_AREA,              A_LAWFUL, PM_SAGE, NON_PM, 3000L ),

A("Altar Carver",        ATHAME,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DRLI|SPFX_DFLAG2), 0, M2_STALK,
	PHYS(8,16),    NO_DFNS,        NO_CARY,
	SUMMON_UNDEAD,              A_LAWFUL, PM_OTAKU, NON_PM, 3000L ),

A("Mirage Tail",        ROTATING_CHAIN,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN), 0, 0,
	PHYS(4,8),    DFNS(AD_FIRE),        NO_CARY,
	DRAGON_BREATH,              A_NEUTRAL, PM_ARTIST, NON_PM, 5000L ),

A("Game Disc",        SPOON,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_HPHDAM), 0, 0,
	NO_ATTK,    DFNS(AD_FIRE),        NO_CARY,
	0,              A_NEUTRAL, PM_GAMER, NON_PM, 5000L ),

A("Modified Z-Sword",        PICK_AXE, /* sorry Bug Sniper, I'm too lazy coding a sword to work as a pick-axe. :( Good idea though. --Amy */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(8,8),    NO_DFNS,        NO_CARY,
	ENERGY_BOOST,              A_LAWFUL, PM_SAIYAN, NON_PM, 5000L ),

A("The Pick of Flandal Steelskin",        PICK_AXE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM), 0, 0,
	NO_ATTK,    NO_DFNS,        NO_CARY,
	0,              A_NEUTRAL, PM_GOLDMINER, NON_PM, 5000L ),

A("Prime Minister's Tuxedo",	T_SHIRT,
	(SPFX_NOGEN|SPFX_DEFN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM|SPFX_DEFN), 0, 0,
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
	PHYS(10,20),	NO_DFNS,	NO_CARY,
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

A("Master Ball",	REALLY_HEAVY_IRON_BALL, /* bad effects when wielded */
	(SPFX_NOGEN|SPFX_DEFN|SPFX_RESTR|SPFX_INTEL|SPFX_REGEN|SPFX_EREGEN|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	PHYS(16,32),	DFNS(AD_MAGM),	NO_CARY,
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

A("Maui's Fishhook",	GRAPPLING_HOOK, /* warns of eels */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HSPDAM|SPFX_SEEK|SPFX_DCLAS), 0, S_EEL,
	PHYS(10,0),        NO_DFNS,        NO_CARY,
	0,		A_CHAOTIC, PM_DIVER, NON_PM, 4500L ),


A("Helm of Storms", STANDARD_HELMET,
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
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM|SPFX_ATTK), SPFX_EREGEN, 0,
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
	PHYS(8,8),        NO_DFNS,        NO_CARY,
	0,		A_NEUTRAL, PM_ORDINATOR, NON_PM, 4500L ),

A("Summoned Sword",                ELVEN_BROADSWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_STLTH|SPFX_REGEN), 0,
	PHYS(4,8),      NO_DFNS,        NO_CARY,        DRAGON_BREATH, A_CHAOTIC, PM_THALMOR, NON_PM, 3000L ),

A("Bow of Vines", ELVEN_BOW,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_LUCK|SPFX_ATTK), SPFX_ESP, 0,
	STUN(7,10),	NO_DFNS,	NO_CARY,
	ENERGY_BOOST, A_NEUTRAL, PM_BOSMER, NON_PM, 4000L ),

A("Ambassador Robe",	ROBE_OF_WEAKNESS,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HSPDAM|SPFX_HPHDAM|SPFX_EVIL), SPFX_EREGEN, 0,
	NO_ATTK,	DRLI(0,0),	CARY(AD_MAGM),
	CREATE_PORTAL,	A_CHAOTIC,	 PM_ALTMER, NON_PM, 2500L),

A("N'wah Killer",        ELVEN_BROADSWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(5,12),    NO_DFNS,        CARY(AD_MAGM),
	0,              A_LAWFUL, PM_DUNMER, NON_PM, 5000L ),

A("Supreme Justice Keeper",		TWO_HANDED_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_LUCK|SPFX_PROTEC|SPFX_INTEL|SPFX_DALIGN), 0, A_NONE,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	0, 		A_LAWFUL, PM_JUSTICE_KEEPER, NON_PM, 200L ),

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
	PHYS(3,8),	NO_DFNS,	CARY(AD_MAGM),
	0,	A_NEUTRAL,	PM_FENCER, NON_PM, 10000L ),

A("The Platinum Yendorian Express Card", CREDIT_CARD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN),
		(SPFX_ESP|SPFX_HSPDAM), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	CHARGE_OBJ,	A_NEUTRAL, PM_TOURIST, NON_PM, 7000L ),

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
	PHYS(5,12),    NO_DFNS,        CARY(AD_MAGM),
	0,              A_LAWFUL, PM_UNDEAD_SLAYER, NON_PM, 5000L ),

A("Vampire Killer",        BULLWHIP,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(5,12),    NO_DFNS,        CARY(AD_MAGM),
	0,              A_LAWFUL, PM_LUNATIC, NON_PM, 5000L ),

A("Itchalaquiaque",        DROVEN_DAGGER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(5,12),    NO_DFNS,        CARY(AD_MAGM),
	0,              A_LAWFUL, PM_ANACHRONIST, NON_PM, 5000L ),

A("The Orb of Fate",		CRYSTAL_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL),
		(SPFX_WARN|SPFX_HSPDAM|SPFX_HPHDAM|SPFX_LUCK), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	LEV_TELE,	A_NEUTRAL, PM_VALKYRIE, NON_PM, 3500L ),

A("The Orb of Weight",		CRYSTAL_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL),
		(SPFX_WARN|SPFX_HSPDAM|SPFX_HPHDAM|SPFX_LUCK), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	LEV_TELE,	A_NEUTRAL, PM_VANILLA_VALK, NON_PM, 3500L ),

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
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_EVIL), (SPFX_HSPDAM|SPFX_HPHDAM), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_MAGM),
	LEV_TELE,  A_LAWFUL, PM_JESTER, NON_PM, 5000L ),

A("Highest Feeling",    LOW_BOOTS, /* initialized to always be fetish heels; 50% chance of extra speed */
	(SPFX_NOGEN|SPFX_DEFN|SPFX_RESTR|SPFX_INTEL|SPFX_TCTRL|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK,        DFNS(AD_FIRE),        NO_CARY,    0, A_NEUTRAL, PM_PROSTITUTE, NON_PM, 10000L ),

A("Lorskel's Integrity",    LOW_BOOTS, /* initialized to always be lolita boots, spawns fart traps every once in a while, has the lolita boots effect even if the base item type changes (because kurwa) */
	(SPFX_NOGEN|SPFX_DEFN|SPFX_RESTR|SPFX_INTEL|SPFX_REFLECT|SPFX_EVIL), 0, 0,
	NO_ATTK,        DFNS(AD_MAGM),        NO_CARY,    0, A_CHAOTIC, PM_KURWA, NON_PM, 5000L ),

A("The Slime Crown", HELM_OF_STORMS,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_EVIL), (SPFX_HSPDAM|SPFX_HPHDAM), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_MAGM),
	LEV_TELE,  A_CHAOTIC, PM_DQ_SLIME, NON_PM, 5000L ),

A("Gender-Inspecific Whip", BULLETPROOF_CHAINWHIP,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DFLAG2), 0, (M2_FEMALE|M2_MALE),
	PHYS(8,20),        NO_DFNS,        NO_CARY,
	0,  A_NEUTRAL, PM_FEMINIST, NON_PM, 2000L ),

A("Extra Control", WAN_INERTIA_CONTROL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,  A_LAWFUL, PM_FORM_CHANGER, NON_PM, 300L ),

A("Meteoric AC", HELM_OF_STEEL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,  A_LAWFUL, PM_GANG_SCHOLAR, NON_PM, 5000L ),

A("Nuclear Spear", ORCISH_SPEAR,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK), 0, 0,
	FIRE(0, 14),        NO_DFNS,        NO_CARY,
	DRAGON_BREATH,  A_CHAOTIC, PM_NUCLEAR_PHYSICIST, NON_PM, 5000L ),

A("Sword of Gilgamesh", ETERNIUM_BLADE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_WARN|SPFX_REFLECT), 0, 0,
	PHYS(5,10),        NO_DFNS,        NO_CARY,
	0,  A_NEUTRAL, PM_TRACER, NON_PM, 2000L ),

A("The Crown of Saint Edward", HELM_OF_TELEPATHY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_HSPDAM), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_MAGM),
	0,  A_LAWFUL, PM_YEOMAN, NON_PM, 5000L ),

A("The Lightsaber Prototype", RED_LIGHTSABER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_REFLECT), 0, 0,
	PHYS(5, 10),	NO_DFNS, NO_CARY,
	ENERGY_BOOST,	A_LAWFUL, PM_JEDI, NON_PM, 3500L ),

A("Gammasaber", RAINBOW_LIGHTSABER, /* +d10 damage when bashing */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_REFLECT), 0, 0,
	PHYS(5, 10),	NO_DFNS, NO_CARY,
	ENERGY_BOOST,	A_LAWFUL, PM_SHADOW_JEDI, NON_PM, 3500L ),

A("Definite Lightsaber", LASER_SWORD, /* never runs out of power */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_REFLECT), 0, 0,
	PHYS(5, 10),	NO_DFNS, NO_CARY,
	ENERGY_BOOST,	A_LAWFUL, PM_HEDDERJEDI, NON_PM, 3500L ),

A("The Elder Cerebral Fluid", ICKY_BLADE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DRLI), (SPFX_EREGEN|SPFX_HSPDAM), 0,
	DRLI(10, 2),	NO_DFNS, NO_CARY,
	ENERGY_BOOST,	A_LAWFUL, PM_ANACHRONOUNBINDER, NON_PM, 2500L ),

A("Cerulean Smash", HIGH_HEELED_SANDAL, /* +10 charisma when wielded, can dig through grave walls in one turn, wielding it as a non-hussy will set it to -10 and heavily curse it (in case you're a smartass and wish for it) */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_EVIL), 0, 0,
	COLD(10, 14),	NO_DFNS, NO_CARY,
	PHASING,	A_CHAOTIC, PM_HUSSY, NON_PM, 8000L ),

A("The Ultimate Pick", DWARVISH_MATTOCK,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(0, 20),	NO_DFNS, NO_CARY,
	0,	A_NEUTRAL, PM_MASON, NON_PM, 2500L ),

A("Pediatric Gawking Gangs", MEN_S_UNDERWEAR, /* resist psi, discount action */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ESP), 0, 0,
	NO_ATTK,	NO_DFNS, NO_CARY,
	0,	A_LAWFUL, PM_CELLAR_CHILD, NON_PM, 4000L ),

A("Message Megaphone", BUGLE, /* tries to make monsters flee, like a tooled horn, but reduces alignment by 5 and max alignment by 1 with every use */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,	NO_DFNS, CARY(AD_MAGM),
	0,	A_CHAOTIC, PM_DEMAGOGUE, NON_PM, 6000L ),

A("Team Fortress GL", GRENADE_LAUNCHER, /* multishot bonus */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN), SPFX_STLTH, 0,
	NO_ATTK,	DFNS(AD_MAGM), NO_CARY,
	0,	A_LAWFUL, PM_GRENADONIN, NON_PM, 10000L ),

A("Marker of Safe Speech", MAGIC_MARKER, /* can potentially be recharged several times */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_HSPDAM|SPFX_SPEAK), 0,
	NO_ATTK,	NO_DFNS, NO_CARY,
	0,	A_CHAOTIC, PM_SOCIAL_JUSTICE_WARRIOR, NON_PM, 6000L ),

A("Stahngnir's Giant Crusher", HUGE_CLUB, /* petrification resistance when wielded */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_ESP), 0,
	PHYS(5, 0),	NO_DFNS, NO_CARY,
	0,	A_NEUTRAL, PM_WALSCHOLAR, NON_PM, 20000L ),

A("Flailmouse", FLAIL, 
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(24, 2),	NO_DFNS, NO_CARY,
	UNTRAP,	A_LAWFUL, PM_SOFTWARE_ENGINEER, NON_PM, 4000L ),

A("Rotating Through", SCOURGE, 
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ESP), 0, 0,
	PHYS(5, 8),	NO_DFNS, NO_CARY,
	CONFLICT,	A_CHAOTIC, PM_CRACKER, NON_PM, 3000L ),

A("Superclean Awaywiper", BROOM, 
	(SPFX_NOGEN|SPFX_DEFN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(30, 10),	DFNS(AD_FIRE), CARY(AD_ELEC),
	OBJ_DETECTION,	A_NEUTRAL, PM_JANITOR, NON_PM, 5000L ),

A("Pin Racket", SPIKED_CLUB, 
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_WARN|SPFX_HPHDAM|SPFX_PROTEC), 0, 0,
	PHYS(4, 12),	NO_DFNS, NO_CARY,
	0,	A_LAWFUL, PM_SPACE_MARINE, NON_PM, 5000L ),

A("Thunderstrike", IMPACT_STAFF, 
	(SPFX_NOGEN|SPFX_DEFN|SPFX_RESTR|SPFX_INTEL|SPFX_TCTRL|SPFX_ATTK), 0, 0,
	ELEC(5, 18),	DFNS(AD_ELEC), NO_CARY,
	LIGHT_AREA,	A_CHAOTIC, PM_STORMBOY, NON_PM, 8000L ),

A("Eyehander", BLINDFOLD, /* +5 increase damage/accuracy when worn */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_ESP), 0,
	NO_ATTK,	NO_DFNS, NO_CARY,
	DEATH_GAZE,	A_CHAOTIC, PM_YAUTJA, NON_PM, 6000L ),

A("World League Hitter", BASEBALL_BAT, 
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_LUCK|SPFX_REFLECT), 0, 0,
	PHYS(50, 2),	NO_DFNS, NO_CARY,
	0,	A_NEUTRAL, PM_QUARTERBACK, NON_PM, 10000L ),

A("Psigun", KALASHNIKOV, /* resist psi when wielded */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HSPDAM|SPFX_EREGEN), (SPFX_REGEN), 0,
	NO_ATTK,	NO_DFNS, NO_CARY,
	CREATE_AMMO,	A_LAWFUL, PM_PSYKER, NON_PM, 2000L ),

A("Controller Hat", HELM_OF_TELEPATHY, /* undead warning when worn */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_SEEK|SPFX_WARN), 0, 0,
	NO_ATTK,	NO_DFNS, NO_CARY,
	TAMING,	A_LAWFUL, PM_EMPATH, NON_PM, 3000L ),

A("Red Pin", OTAMA, 
	(SPFX_NOGEN|SPFX_DEFN|SPFX_RESTR|SPFX_INTEL|SPFX_STLTH|SPFX_ATTK), (SPFX_HALRES), 0,
	FIRE(10, 26),	DFNS(AD_FIRE), NO_CARY,
	SUMMON_FIRE_ELEMENTAL,	A_NEUTRAL, PM_MASTERMIND, NON_PM, 6000L ),

A("Bus Error", SEGFAULT_VENOM, 
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DRLI|SPFX_SPEAK|SPFX_DEFN|SPFX_ATTK), (SPFX_STLTH|SPFX_PROTEC), 0,
	DRLI(10, 10),	DFNS(AD_ACID), NO_CARY,
	PHASING,	A_LAWFUL, PM_WEIRDBOY, NON_PM, 1000L ),

A("Rocket Impulse", COMMANDER_SUIT, /* speed and magical breathing when worn, 10 extra points of AC */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM|SPFX_XRAY), (SPFX_TCTRL), 0,
	NO_ATTK,	NO_DFNS, NO_CARY,
	ENERGY_BOOST,	A_LAWFUL, PM_ASTRONAUT, NON_PM, 20000L ),

A("Gaysect", CHEMISTRY_SPACE_AXE, /* tsurugi of muramasa with different base item, can bisect enemies */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_BEHEAD|SPFX_LUCK), 0, 0,
	NO_ATTK,	NO_DFNS, NO_CARY,
	0,	A_CHAOTIC, PM_CYBERNINJA, NON_PM, 4500L ),

A("Voiceshutter", HEAVY_HAMMER, /* deafness when wielded */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_STLTH|SPFX_ATTK|SPFX_EVIL), 0, 0,
	ELEC(10, 20),	DFNS(AD_ELEC), NO_CARY,
	LEV_TELE,	A_LAWFUL, PM_DISSIDENT, NON_PM, 6000L ),

A("Snakelash", SECRET_WHIP, /* always poisoned, poison and sickness res while wielded */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(2, 14),	NO_DFNS, NO_CARY,
	HEALING,	A_CHAOTIC, PM_XELNAGA, NON_PM, 5000L ),

A("The Holographic Void Lily",				CREDIT_CARD,
      (SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN), (SPFX_EREGEN|SPFX_HSPDAM|SPFX_REFLECT), 0,
	NO_ATTK, NO_DFNS, NO_CARY, SUMMON_UNDEAD, A_CHAOTIC, PM_CARTOMANCER, NON_PM, 7000L),

A("Sharur",				ORNATE_MACE,
      (SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK|SPFX_WARN|SPFX_DEFN), 0, 0,
	PHYS(5, 10), DFNS(AD_MAGM), NO_CARY, 0, A_LAWFUL, PM_DRAGONMASTER, NON_PM, 4000L),

A("Scooba Cooba",				CLOAK_OF_MAGIC_RESISTANCE, /* protects items from water and lets you swim in lava */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, PM_FJORDE, NON_PM, 10000L ),

A("Too Late Prize",				CHEMISTRY_SET,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, SUMMON_WATER_ELEMENTAL, A_LAWFUL, PM_PRACTICANT, NON_PM, 2000L ),

A("Meancane",				WALKING_STICK, /* disintegration resistance and free action */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(8, 14), NO_DFNS, NO_CARY, 0, A_CHAOTIC, PM_EMERA, NON_PM, 3000L ),

A("Clean Mauler",				DWARVISH_SPEAR, /* always hits as a +4 weapon but dulls quickly like ceramic */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_EVIL), 0, 0,
	PHYS(10, 20), NO_DFNS, NO_CARY, 0, A_NEUTRAL, PM_TOSSER, NON_PM, 2500L ),

A("Nobreak",				AKLYS, /* instead of breaking, it loses enchantment */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(20, 10), NO_DFNS, NO_CARY, 0, A_LAWFUL, PM_AKLYST, NON_PM, 2000L ),

A("Fertilizator",				BINNING_KIT, /* creates random sling ammo whenever it trashes a corpse */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, PM_MILL_SWALLOWER, NON_PM, 1000L ),

A("Hungerhealer",				HITCHHIKER_S_GUIDE_TO_THE_GALA, /* passively increases hunger and heals symbiote while it's in your inventory */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, PM_SYMBIANT, NON_PM, 4242L ),

A("Flagellator",				SEXPLAY_WHIP, /* 1 in 5000 chance to polymorph you into a M2_NEUTER, doesn't drop upon poly */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_EVIL), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, 0, A_NEUTRAL, PM_GENDERSTARIST, NON_PM, 2000L ),

A("Major Presence",				GAUNTLETS, /* +2 to-hit and damage */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, PM_COMBATANT, NON_PM, 4000L ),

A("Gropestroker",				SADO_MASO_GLOVES, /* stroking a farting monster has 1 in 20 of pacifying it and 1 in 100 of frenzying it */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM|SPFX_EVIL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, PM_BUTT_LOVER, NON_PM, 3000L ),

A("Softstep",				ANASTASIA_DANCING_SHOES, /* cannot be destroyed by erosion, stepping into a heap of shit that wasn't yet detected gives +1 CHA */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_ELEC), NO_CARY, 0, A_LAWFUL, PM_DANCER, NON_PM, 10000L ),

A("Hammer of Hellforge",				MALLET,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DEFN|SPFX_PROTEC), 0, 0,
	FIRE(0, 10), DFNS(AD_FIRE), NO_CARY, 0, A_CHAOTIC, PM_DIABLIST, NON_PM, 8000L ),

A("Downsize Glass",				MIRROR, /* boss nuke works only for preversioner */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_LUCK), 0,
	NO_ATTK, NO_DFNS, NO_CARY, BOSS_NUKE, A_NEUTRAL, PM_PREVERSIONER, NON_PM, 6000L ),

A("Ass Deaf Auger",				WAN_BANISHMENT, /* doesn't get zapped by monsters; if a secret advice member zaps it, 10% chance to delete the monster unless it's a unique */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, PM_SECRET_ADVICE_MEMBER, NON_PM, 8000L ),

A("Katharina's Loveliness",				KATHARINA_PLATFORM_BOOTS, /* +10 kick damage, +10 CHA, maurah trap effect */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN|SPFX_EVIL), 0, 0,
	NO_ATTK, DFNS(AD_COLD), NO_CARY, 0, A_CHAOTIC, PM_SHOE_FETISHIST, NON_PM, 16000L ),

A("Clickpass",				SOFT_CHASTITY_BELT, /* prevents foocubi from using their sex attack on you, protects from item theft like being a nymph */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_LAWFUL, PM_CLIMACTERIAL, NON_PM, 8000L ),

A("Spectrum Pling",				CIRMOCLINE, /* prism reflection and resist fire, cold, shock, poison and psi when carried */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_REFLECT), 0,
	NO_ATTK, NO_DFNS, NO_CARY, 0, A_NEUTRAL, PM_WOMANIZER, NON_PM, 20000L ),

A("Sabrina's Rescue",				STURDY_PLATEAU_BOOT_FOR_GIRLS, /* aggravate monster when wielded */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM|SPFX_WARN|SPFX_ATTK|SPFX_EVIL), 0, 0,
	ACID(4, 12), NO_DFNS, NO_CARY, DRAGON_BREATH, A_LAWFUL, PM_SINGSLAVE, NON_PM, 18000L ),

A("Penumbral Lasso",				LANCE, /* if it would break from jousting, it might not */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(0, 10), NO_DFNS, NO_CARY, SPECIAL_INVOKE, A_LAWFUL, PM_JOCKEY, NON_PM, 4000L ),

/* Penumbral Lasso must be last!!! */

/* Amy note: DO NOT add non-quest artifacts beyond this point!!! foreignartifactcount() in invent.c depends on it */

/*
 *  terminator; otyp must be zero
 */
A(0, 0, 0, 0, 0, NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 0L )

};	/* artilist_pre[] (or artifact_names[]) */

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
