/*	SCCS Id: @(#)objects.c	3.4	2002/07/31	*/
/* Copyright (c) Mike Threepoint, 1989.				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef OBJECTS_PASS_2_
/* first pass */
struct monst { struct monst *dummy; };	/* lint: struct obj's union */
#include "config.h"
#include "obj.h"
#include "objclass.h"
#include "prop.h"
#include "skills.h"
#include "skills.h"

#else	/* !OBJECTS_PASS_2_ */
/* second pass */
#include "color.h"
#  define COLOR_FIELD(X) X,
#endif	/* !OBJECTS_PASS_2_ */



/* objects have symbols: ) [ = " ( % ! ? + / $ * ` 0 _ . */

/*
 *	Note:  OBJ() and BITS() macros are used to avoid exceeding argument
 *	limits imposed by some compilers.  The ctnr field of BITS currently
 *	does not map into struct objclass, and is ignored in the expansion.
 *	The 0 in the expansion corresponds to oc_pre_discovered, which is
 *	set at run-time during role-specific character initialization.
 */

#ifndef OBJECTS_PASS_2_
/* first pass -- object descriptive text */
# define OBJ(name,desc) name,desc
# define OBJECT(obj,bits,prp,sym,prob,dly,wt,cost,sdam,ldam,oc1,oc2,nut,color) \
	{obj}

NEARDATA struct objdescr obj_descr[] = {
#else
/* second pass -- object definitions */

# define BITS(nmkn,mrg,uskn,ctnr,mgc,chrg,uniq,nwsh,big,tuf,dir,sub,mtrl) \
	nmkn,mrg,uskn,0,mgc,chrg,uniq,nwsh,big,tuf,dir,mtrl,sub /* SCO ODT 1.1 cpp fodder */
# define OBJECT(obj,bits,prp,sym,prob,dly,wt,cost,sdam,ldam,oc1,oc2,nut,color) \
	{0, 0, (char *)0, bits, prp, sym, dly, COLOR_FIELD(color) \
	 prob, wt, cost, sdam, ldam, oc1, oc2, nut}
# ifndef lint
#  define HARDGEM(n) (n >= 8)
# else
#  define HARDGEM(n) (0)
# endif

NEARDATA struct objclass objects[] = {
#endif
/* dummy object[0] -- description [2nd arg] *must* be NULL */
	OBJECT(OBJ("strange object",(char *)0), BITS(1,0,0,0,0,0,0,0,0,0,0,0,0),
			0, ILLOBJ_CLASS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),

/* weapons ... */
#define WEAPON(name,app,kn,mg,bi,prob,wt,cost,sdam,ldam,hitbon,typ,sub,metal,color) \
	OBJECT( \
		OBJ(name,app), BITS(kn,mg,1,0,0,1,0,0,bi,0,typ,sub,metal), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, sdam, ldam, hitbon, 0, wt, color )
#define PROJECTILE(name,app,kn,prob,wt,cost,sdam,ldam,hitbon,metal,sub,color) \
	OBJECT( \
		OBJ(name,app), BITS(kn,1,1,0,0,1,0,0,0,0,PIERCE,sub,metal), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, sdam, ldam, hitbon, WP_GENERIC, wt, color )
#define BOW(name,app,kn,bi,prob,wt,cost,hitbon,metal,sub,color) \
	OBJECT( \
		OBJ(name,app), BITS(kn,0,1,0,0,1,0,0,bi,0,0,sub,metal), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, 0, 0, hitbon, WP_GENERIC, wt, color )
#define BULLET(name,app,kn,prob,wt,cost,sdam,ldam,hitbon,ammotyp,typ,metal,sub,color) \
	OBJECT( \
		OBJ(name,app), BITS(kn,1,1,0,0,1,0,0,0,0,typ,sub,metal), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, sdam, ldam, hitbon, ammotyp, wt, color )
#define GUN(name,app,kn,bi,prob,wt,cost,range,rof,hitbon,ammotyp,metal,sub,color) \
	OBJECT( \
		OBJ(name,app), BITS(kn,0,1,0,0,1,0,0,bi,0,0,sub,metal), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, range, rof, hitbon, ammotyp, wt, color )

/* Note: for weapons that don't do an even die of damage (ex. 2-7 or 3-18)
 * the extra damage is added on in weapon.c, not here! */

#define P PIERCE
#define S SLASH
#define B WHACK
#define E EXPLOSION

/* Daggers */
WEAPON("orcish dagger", "crude dagger",
	0, 1, 0, 10, 10,  4,  3,  3, 2, P,   P_DAGGER, IRON, CLR_BLACK),
WEAPON("dagger", (char *)0,
	1, 1, 0, 25, 10,  4,  4,  3, 2, P,   P_DAGGER, IRON, HI_METAL),
WEAPON("athame", (char *)0,
	1, 1, 0,  0, 10,  4,  4,  3, 2, S,   P_DAGGER, IRON, HI_METAL),
WEAPON("silver dagger", (char *)0,
	1, 1, 0,  2, 12, 40,  4,  3, 2, P,   P_DAGGER, SILVER, HI_SILVER),
	/* STEPHEN WHITE'S NEW CODE */
	/* WAC silver dagger now pierces, to be same as other daggers
        allows it to be thrown without penalty as well*/
WEAPON("elven dagger", "runed dagger",
	0, 1, 0,  8, 10,  4,  5,  3, 2, P,   P_DAGGER, WOOD, HI_METAL),
WEAPON("dark elven dagger", "black runed dagger",
	0, 1, 0,  0, 10,  4,  5,  3, 2, P,   P_DAGGER, WOOD, CLR_BLACK),
WEAPON("wooden stake", (char *)0,
	1, 0, 0,  0, 20, 50,  6,  6, 0, P,   P_DAGGER, WOOD, HI_WOOD),
	/* STEPHEN WHITE'S NEW CODE */
    /* Base for artifact (Stake of Van Helsing) */
WEAPON("great dagger", (char *)0,
	1, 0, 0,  0, 20,500,  6,  7, 2, P,   P_DAGGER, METAL, CLR_BLACK),
	/* STEPHEN WHITE'S NEW CODE */
	/* for necromancer artifact... */

/* Knives */
WEAPON("worm tooth", (char *)0,
	1, 0, 0,  0, 20,  2,  2,  2, 0, 0,   P_KNIFE, 0, CLR_WHITE),
WEAPON("knife", (char *)0,
	1, 1, 0, 15,  5,  4,  3,  2, 0, P|S, P_KNIFE, IRON, HI_METAL),
WEAPON("stiletto", (char *)0,
	1, 1, 0,  5,  5,  4,  3,  2, 0, P|S, P_KNIFE, IRON, HI_METAL),
WEAPON("scalpel", (char *)0,
	1, 1, 0,  0,  5,  4,  3,  3, 2, S,   P_KNIFE, METAL, HI_METAL),
WEAPON("crysknife", (char *)0,
	1, 0, 0,  0, 20,100, 20, 30, 3, P,   P_KNIFE, MINERAL, CLR_WHITE),
	/* [Tom] increased crysknife damage from d10/d10 */
	/* to d20/d30 (otherwise, it's useless to make them...) */

/* Axes */
WEAPON("axe", (char *)0,
	1, 0, 0, 35, 60,  8,  6,  4, 0, S,   P_AXE, IRON, HI_METAL),
WEAPON("battle-axe", "double-headed axe", /* "double-bitted" ? */
	0, 0, 1, 10,120, 40,  8,  6, 0, S,   P_AXE, IRON, HI_METAL),

/* Pick-axes */
/* (also weptool pick-axe) */
WEAPON("dwarvish mattock", "broad pick",
	0, 0, 1, 13,120, 50, 12,  8,-1, B,   P_PICK_AXE, IRON, HI_METAL),

/* Short swords */
WEAPON("orcish short sword", "crude short sword",
	0, 0, 0,  3, 30, 10,  5,  8, 0, P,   P_SHORT_SWORD, IRON, CLR_BLACK),
WEAPON("short sword", (char *)0,
	1, 0, 0,  8, 30, 10,  6,  8, 0, P,   P_SHORT_SWORD, IRON, HI_METAL),
WEAPON("silver short sword", (char *)0,
	1, 0, 0,  2, 36, 50,  6,  8, 0, P,   P_SHORT_SWORD, SILVER, HI_SILVER),
	/* STEPHEN WHITE'S NEW CODE */
WEAPON("dwarvish short sword", "broad short sword",
	0, 0, 0,  2, 30, 10,  7,  8, 0, P,   P_SHORT_SWORD, IRON, HI_METAL),
WEAPON("elven short sword", "runed short sword",
	0, 0, 0,  2, 30, 10,  8,  8, 0, P,   P_SHORT_SWORD, WOOD, HI_METAL),
WEAPON("dark elven short sword", "black runed short sword",
	0, 0, 0,  2, 30, 10,  8,  8, 0, P,   P_SHORT_SWORD, WOOD, CLR_BLACK),

/* Broadswords */
WEAPON("broadsword", (char *)0,
	1, 0, 0, 20, 70, 10,  4,  6, 0, S,   P_BROAD_SWORD, IRON, HI_METAL),
						/* +d4 small, +1 large */
WEAPON("runesword", "runed broadsword",
	0, 0, 0,  0, 40,300,  4,  6, 0, S,   P_BROAD_SWORD, IRON, CLR_BLACK),
	/* +d4 small, +1 large; base for artifact (Stormbringer) */
	/* +5d2 +d8 from level drain */
WEAPON("elven broadsword", "runed broadsword",
	0, 0, 0,  4, 70, 10,  6,  6, 0, S,   P_BROAD_SWORD, WOOD, HI_METAL),
						/* +d4 small, +1 large */

/* Long swords */
WEAPON("long sword", (char *)0,
	1, 0, 0, 50, 40, 15,  8, 12, 0, S,   P_LONG_SWORD, IRON, HI_METAL),
WEAPON("silver long sword", (char *)0,
	1, 0, 0,  2, 48, 75,  8, 12, 0, S,   P_LONG_SWORD,SILVER, HI_SILVER),
	/* STEPHEN WHITE'S NEW CODE */
WEAPON("katana", "samurai sword",
	0, 0, 0,  4, 40, 80, 10, 12, 1, S,   P_LONG_SWORD, IRON, HI_METAL),

/* Two-handed swords */
WEAPON("two-handed sword", (char *)0,
	1, 0, 1, 25,150, 50, 12,  6, 0, S,   P_TWO_HANDED_SWORD, IRON, HI_METAL),
	/* +2d6 large */
WEAPON("tsurugi", "long samurai sword",
	0, 0, 1,  0, 60,500, 16,  8, 2, S,   P_TWO_HANDED_SWORD, METAL, HI_METAL),
	/* +2d6 large; base for artifact (T of Muramasa) */

/* Scimitars */
WEAPON("scimitar", "curved sword",
	0, 0, 0, 15, 40, 15,  8,  8, 0, S,   P_SCIMITAR, IRON, HI_METAL),

/* Sabers */
WEAPON("rapier", (char *)0,
	1, 0, 0,  0, 30, 40,  6,  8,  0, P,  P_SABER, METAL, CLR_BLACK),
	/* STEPHEN WHITE'S NEW CODE */
	/* Base for artifact (Scalpel) */
WEAPON("silver saber", (char *)0,
	1, 0, 0, 27, 40, 75,  8,  8, 0, S,   P_SABER, SILVER, HI_SILVER),

/* Clubs */
WEAPON("club", (char *)0,
	1, 0, 0, 22, 30,  3,  6,  3, 0, B,   P_CLUB, WOOD, HI_WOOD),
WEAPON("aklys", "thonged club",
	0, 0, 0, 16, 15,  4,  6,  3, 0, B,   P_CLUB, IRON, HI_METAL),
WEAPON("baseball bat", (char *)0,
	1, 0, 0,  0, 40, 50,  8,  6, 0, B,   P_CLUB, WOOD, HI_WOOD),
	/* STEPHEN WHITE'S NEW CODE */
	/* Base for artifact */

/* Paddles */
/* Good to-hit and small damage, but low large damage */
WEAPON("fly swatter", (char *)0,
	1, 0, 0,  2, 10,  3, 10,  2, 2, B,   P_PADDLE, PLASTIC, CLR_GREEN),

/* Maces */
WEAPON("silver mace", (char *)0,
	1, 0, 0, 12, 36, 65,  6,  6, 0, B,   P_MACE, SILVER, HI_SILVER),
	/* STEPHEN WHITE'S NEW CODE */
WEAPON("mace", (char *)0,
	1, 0, 0, 40, 30,  5,  6,  6, 0, B,   P_MACE, IRON, HI_METAL),
	/* +1 small */

/* Morning stars */
WEAPON("morning star", (char *)0,
	1, 0, 0, 12,120, 10,  4,  6, 0, B,   P_MORNING_STAR, IRON, HI_METAL),
						/* +d4 small, +1 large */

/* Flails */
WEAPON("flail", (char *)0,
	1, 0, 0, 30, 15,  4,  6,  4, 0, B,   P_FLAIL, IRON, HI_METAL),
	/* +1 small, +1d4 large */

/* Hammers */
WEAPON("war hammer", (char *)0,
	1, 0, 0, 25, 50,  5,  4,  4, 0, B,   P_HAMMER, IRON, HI_METAL),
	/* +1 small */
WEAPON("heavy hammer", (char *)0,
	1, 0, 0,  0, 60,500,  6,  6, 0, B,   P_HAMMER, METAL, HI_METAL),
	/* STEPHEN WHITE'S NEW CODE */
	/* Base for artifact */

/* Quarterstaves */
WEAPON("quarterstaff", "staff",
	0, 0, 1, 11, 40,  5,  6,  6, 0, B,   P_QUARTERSTAFF, WOOD, HI_WOOD),

/* Polearms */
/* (also weptool fishing pole) */
/* spear-type */
WEAPON("partisan", "vulgar polearm",
	0, 0, 1,  3, 80, 10,  6,  6, 0, P,   P_POLEARMS, IRON, HI_METAL),
						/* +1 large */
WEAPON("glaive", "single-edged polearm",
	0, 0, 1,  4, 75,  6,  6, 10, 0, S,   P_POLEARMS, IRON, HI_METAL),
WEAPON("spetum", "forked polearm",
	0, 0, 1,  3, 50,  5,  6,  6, 0, P,   P_POLEARMS, IRON, HI_METAL),
						/* +1 small, +d6 large */
WEAPON("ranseur", "hilted polearm",
	0, 0, 1,  3, 50,  6,  4,  4, 0, P,   P_POLEARMS, IRON, HI_METAL),
	/* +d4 both */
/* axe-type */
WEAPON("bardiche", "long poleaxe",
	0, 0, 1,  2,120,  7,  4,  4, 0, S,   P_POLEARMS, IRON, HI_METAL),
						/* +1d4 small, +2d4 large */
WEAPON("voulge", "pole cleaver",
	0, 0, 1,  2,125,  5,  4,  4, 0, S,   P_POLEARMS, IRON, HI_METAL),
						/* +d4 both */
WEAPON("halberd", "angled poleaxe",
	0, 0, 1,  4,150, 10, 10,  6, 0, P|S, P_POLEARMS, IRON, HI_METAL),
	/* +1d6 large */
/* curved/hooked */
WEAPON("fauchard", "pole sickle",
	0, 0, 1,  3, 60,  5,  6,  8, 0, P|S, P_POLEARMS, IRON, HI_METAL),
WEAPON("guisarme", "pruning hook",
	0, 0, 1,  3, 80,  5,  4,  8, 0, S,   P_POLEARMS, IRON, HI_METAL),
						/* +1d4 small */
WEAPON("bill-guisarme", "hooked polearm",
	0, 0, 1,  2,120,  7,  4, 10, 0, P|S, P_POLEARMS, IRON, HI_METAL),
						/* +1d4 small */
/* other */
WEAPON("lucern hammer", "pronged polearm",
	0, 0, 1,  3,150,  7,  4,  6, 0, B|P, P_POLEARMS, IRON, HI_METAL),
						/* +1d4 small */
WEAPON("bec de corbin", "beaked polearm",
	0, 0, 1,  2,100,  8,  8,  6, 0, B|P, P_POLEARMS, IRON, HI_METAL),

/* Spears */
WEAPON("orcish spear", "crude spear",
	0, 1, 0, 13, 30,  3,  5,  8, 0, P,   P_SPEAR, IRON, CLR_BLACK),
WEAPON("spear", (char *)0,
	1, 1, 0, 50, 30,  3,  6,  8, 0, P,   P_SPEAR, IRON, HI_METAL),
WEAPON("silver spear", (char *)0,
	1, 1, 0,  3, 36, 40,  6,  8, 0, P,   P_SPEAR, SILVER, HI_SILVER),
	/* STEPHEN WHITE'S NEW CODE */
WEAPON("elven spear", "runed spear",
	0, 1, 0, 10, 30,  3,  7,  8, 0, P,   P_SPEAR, WOOD, HI_METAL),
WEAPON("dwarvish spear", "stout spear",
	0, 1, 0, 12, 35,  3,  8,  8, 0, P,   P_SPEAR, IRON, HI_METAL),

/* Javelins */
WEAPON("javelin", "throwing spear",
	0, 1, 0, 10, 20,  3,  6,  6, 0, P,   P_JAVELIN, IRON, HI_METAL),

/* Tridents */
WEAPON("trident", (char *)0,
	1, 0, 0,  8, 25,  5,  6,  4, 0, P,   P_TRIDENT, IRON, HI_METAL),
	/* +1 small, +2d4 large */

/* Lances */
WEAPON("lance", (char *)0,
	1, 0, 0,  1,180, 10,  6,  8, 0, P,   P_LANCE, IRON, HI_METAL),

/* Bows (and arrows) */
/* KMH, balance patch -- damage of launchers restored to d2 big and small */
BOW("orcish bow", "crude bow",	0, 1, 12, 30, 60, 0, WOOD, P_BOW, CLR_BLACK),
BOW("bow", (char *)0,			1, 1, 24, 30, 60, 0, WOOD, P_BOW, HI_WOOD),
BOW("elven bow", "runed bow",	0, 1, 12, 30, 60, 0, WOOD, P_BOW, HI_WOOD),
BOW("dark elven bow",  "black runed bow", 0, 1, 0, 30, 60, 0, WOOD, P_BOW, CLR_BLACK),
BOW("yumi", "long bow",			0, 1, 0, 30, 60, 0, WOOD, P_BOW, HI_WOOD),
PROJECTILE("orcish arrow", "crude arrow",
	0, 15, 1, 2, 5, 6, 0, IRON, -P_BOW, CLR_BLACK),
PROJECTILE("arrow", (char *)0,
	1, 50, 1, 2, 6, 6, 0, IRON, -P_BOW, HI_METAL),
PROJECTILE("silver arrow", (char *)0,
	1, 15, 1, 5, 6, 6, 0, SILVER, -P_BOW, HI_SILVER),
PROJECTILE("elven arrow", "runed arrow",
	0, 25, 1, 2, 7, 6, 0, WOOD, -P_BOW, HI_METAL),
PROJECTILE("dark elven arrow", "black runed arrow",
	0,  0, 1, 2, 7, 6, 0, WOOD, -P_BOW, CLR_BLACK),
PROJECTILE("ya", "bamboo arrow",
	0, 10, 1, 4, 7, 7, 1, METAL, -P_BOW, HI_METAL),

/* Slings */
BOW("sling", (char *)0,		1, 0, 40,  3, 20, 0, WOOD, P_SLING, HI_LEATHER),

/* Firearms */
#ifdef FIREARMS
GUN("pistol", (char *)0,	   1,  0, 0,  20,  100, 15,  0,  0, WP_BULLET, IRON, P_FIREARM, HI_METAL),
GUN("submachine gun", (char *)0,   1,  0, 0,  25,  250, 10,  3, -1, WP_BULLET, IRON, P_FIREARM, HI_METAL),
GUN("heavy machine gun", (char *)0,1,  1, 0, 500, 2000, 20,  8, -4, WP_BULLET, IRON, P_FIREARM, HI_METAL),
GUN("rifle", (char *)0,		   1,  1, 0,  30,  150, 22, -1,  1, WP_BULLET, IRON, P_FIREARM, HI_METAL),
GUN("assault rifle", (char *)0,	   1,  0, 0,  40, 1000, 20,  5, -2, WP_BULLET, IRON, P_FIREARM, HI_METAL),
GUN("sniper rifle", (char *)0,	   1,  1, 0,  50, 4000, 25, -3,  4, WP_BULLET, IRON, P_FIREARM, HI_METAL),
GUN("shotgun", (char *)0,	   1,  0, 0,  35,  200,  3, -1,  3,  WP_SHELL, IRON, P_FIREARM, HI_METAL),
GUN("auto shotgun", (char *)0,	   1,  1, 0,  60, 1500,  3,  2,  0,  WP_SHELL, IRON, P_FIREARM, HI_METAL),
GUN("rocket launcher", (char *)0,  1,  1, 0, 750, 3500, 20, -5, -4, WP_ROCKET, IRON, P_FIREARM, HI_METAL),
GUN("grenade launcher", (char *)0, 1,  1, 0,  55, 1500,  6, -3, -3,WP_GRENADE, IRON, P_FIREARM, HI_METAL),
BULLET("bullet", (char *)0,
	1,  0,   1,   5, 20, 30, 0, WP_BULLET,   P,   IRON, -P_FIREARM, HI_METAL),
BULLET("silver bullet", (char *)0,
	1,  0,   1,  15, 20, 30, 0, WP_BULLET,   P, SILVER, -P_FIREARM, HI_SILVER),
BULLET("shotgun shell", (char *)0,
	1,  0,   1,  10, 30, 45, 0,  WP_SHELL,   P,   IRON, -P_FIREARM, CLR_RED),
BULLET("rocket", (char *)0,
	1,  0, 200, 450, 45, 60, 0, WP_ROCKET, P|E,   IRON, -P_FIREARM, CLR_BLUE),
BULLET("frag grenade", (char *)0,
	1,  0,  25, 350, 0, 0, 0,  WP_GRENADE, B|E,   IRON, -P_FIREARM, CLR_GREEN),
BULLET("gas grenade", (char *)0,
	1,  0,  25, 350, 0, 0, 0,  WP_GRENADE, B|E,   IRON, -P_FIREARM, CLR_ORANGE),
BULLET("stick of dynamite", "red stick",
	0,  0,  30, 150, 0, 0, 0,  WP_GENERIC,   B,   PLASTIC, P_NONE, CLR_RED),
#endif

/* Crossbows (and bolts) */
/* Crossbow range is now independant of strength */
GUN("crossbow", (char *)0,	0, 1, 45, 50, 40, 12, -1, 0, WP_GENERIC, WOOD, P_CROSSBOW, HI_WOOD),
PROJECTILE("crossbow bolt", (char *)0,
	1, 45, 1, 2, 4, 6, 0, IRON, -P_CROSSBOW, HI_METAL),

/* Darts */
/* (also weptoool spoon) */
WEAPON("dart", (char *)0,
	1, 1, 0, 55,  1,  2,  3,  2, 0, P,   -P_DART, IRON, HI_METAL),

/* Shurikens */
WEAPON("shuriken", "throwing star",
	0, 1, 0, 35,  1,  5,  8,  6, 2, P,   -P_SHURIKEN, IRON, HI_METAL),

/* Boomerangs */
WEAPON("boomerang", (char *)0,
	1, 1, 0, 15,  5, 20,  9,  9, 0, 0,   -P_BOOMERANG, WOOD, HI_WOOD),

/* Whips */
WEAPON("bullwhip", (char *)0,
	1, 0, 0,  2, 20,  4,  2,  1, 0, 0,   P_WHIP, LEATHER, CLR_BROWN),
#ifdef KOPS
WEAPON("rubber hose", (char *)0,
	1, 0, 0,  0, 20,  3,  4,  3, 0, B,   P_WHIP, PLASTIC, CLR_BROWN),
#endif

/* With shuffled appearances... */
#undef P
#undef S
#undef B
#undef E

#undef WEAPON
#undef PROJECTILE
#undef BOW
#undef BULLET
#undef GUN

/* armor ... */
/* IRON denotes ferrous metals, including steel.
 * Only IRON weapons and armor can rust.
 * Only COPPER (including brass) corrodes.
 * Some creatures are vulnerable to SILVER.
 */
#define ARMOR(name,desc,kn,mgc,blk,power,prob,delay,wt,cost,ac,can,sub,metal,c) \
	OBJECT( \
		OBJ(name,desc), BITS(kn,0,1,0,mgc,1,0,0,blk,0,0,sub,metal), power, \
		ARMOR_CLASS, prob, delay, wt, cost, \
		0, 0, 10 - ac, can, wt, c )
#define CLOAK(name,desc,kn,mgc,power,prob,delay,wt,cost,ac,can,metal,c) \
	ARMOR(name,desc,kn,mgc,0,power,prob,delay,wt,cost,ac,can,ARM_CLOAK,metal,c)
#define HELM(name,desc,kn,mgc,power,prob,delay,wt,cost,ac,can,metal,c) \
	ARMOR(name,desc,kn,mgc,0,power,prob,delay,wt,cost,ac,can,ARM_HELM,metal,c)
#define GLOVES(name,desc,kn,mgc,power,prob,delay,wt,cost,ac,can,metal,c) \
	ARMOR(name,desc,kn,mgc,0,power,prob,delay,wt,cost,ac,can,ARM_GLOVES,metal,c)
#define SHIELD(name,desc,kn,mgc,blk,power,prob,delay,wt,cost,ac,can,metal,c) \
	ARMOR(name,desc,kn,mgc,blk,power,prob,delay,wt,cost,ac,can,ARM_SHIELD,metal,c)
#define BOOTS(name,desc,kn,mgc,power,prob,delay,wt,cost,ac,can,metal,c) \
	ARMOR(name,desc,kn,mgc,0,power,prob,delay,wt,cost,ac,can,ARM_BOOTS,metal,c)

#ifdef TOURIST
/* Shirts */
ARMOR("Hawaiian shirt", (char *)0,
	1, 0, 0, 0,	 8, 0,	 5,   3, 10, 0, ARM_SHIRT, CLOTH, CLR_MAGENTA),
ARMOR("T-shirt", (char *)0,
	1, 0, 0, 0,	 2, 0,	 5,   2, 10, 0, ARM_SHIRT, CLOTH, CLR_WHITE),
#endif

/* Suits of armor */
ARMOR("plate mail", (char *)0,
	1, 0, 1, 0,	40, 5, 450, 600,  3, 2, ARM_SUIT, IRON, HI_METAL),
ARMOR("crystal plate mail", (char *)0,
	1, 0, 1, 0,	10, 5, 450, 820,  3, 2, ARM_SUIT, GLASS, CLR_WHITE),
#ifdef TOURIST
ARMOR("bronze plate mail", (char *)0,
	1, 0, 1, 0,	25, 5, 450, 400,  4, 0, ARM_SUIT, COPPER, HI_COPPER),
#else
ARMOR("bronze plate mail", (char *)0,
	1, 0, 1, 0,	35, 5, 450, 400,  4, 0, ARM_SUIT, COPPER, HI_COPPER),
#endif
ARMOR("splint mail", (char *)0,
	1, 0, 1, 0,	65, 5, 400,  80,  4, 1, ARM_SUIT, IRON, HI_METAL),
ARMOR("banded mail", (char *)0,
	1, 0, 1, 0,	75, 5, 350,  90,  4, 0, ARM_SUIT, IRON, HI_METAL),
ARMOR("dwarvish mithril-coat", (char *)0,
	1, 0, 0, 0,	10, 1, 150, 240,  4, 3, ARM_SUIT, MITHRIL, HI_METAL),
ARMOR("dark elven mithril-coat", (char *)0,
	1, 0, 0, 0,  0, 1, 150, 240,  4, 3, ARM_SUIT, MITHRIL, CLR_BLACK),
ARMOR("elven mithril-coat", (char *)0,
	1, 0, 0, 0,	15, 1, 150, 240,  5, 3, ARM_SUIT, MITHRIL, HI_METAL),
ARMOR("chain mail", (char *)0,
	1, 0, 0, 0,	70, 5, 300,  75,  5, 1, ARM_SUIT, IRON, HI_METAL),
ARMOR("orcish chain mail", "crude chain mail",
	0, 0, 0, 0,	20, 5, 300,  75,  6, 1, ARM_SUIT, IRON, CLR_BLACK),
ARMOR("scale mail", (char *)0,
	1, 0, 0, 0,	70, 5, 250,  45,  6, 0, ARM_SUIT, IRON, HI_METAL),
ARMOR("studded leather armor", (char *)0,
	1, 0, 0, 0,	70, 3, 200,  15,  7, 1, ARM_SUIT, LEATHER, HI_LEATHER),
ARMOR("ring mail", (char *)0,
	1, 0, 0, 0,	70, 5, 250, 100,  7, 0, ARM_SUIT, IRON, HI_METAL),
ARMOR("orcish ring mail", "crude ring mail",
	0, 0, 0, 0,	20, 5, 250,  80,  8, 1, ARM_SUIT, IRON, CLR_BLACK),
ARMOR("leather armor", (char *)0,
	1, 0, 0, 0,	75, 3, 150,   5,  8, 0, ARM_SUIT, LEATHER, HI_LEATHER),
ARMOR("leather jacket", (char *)0,
	1, 0, 0, 0,	11, 0,	30,  10,  9, 0, ARM_SUIT, LEATHER, CLR_BLACK),

/* Robes */
/* STEPHEN WHITE'S NEW CODE */
ARMOR("robe", "red robe",
	0, 0, 0, 0,		1, 1,  40,  25,  9, 0, ARM_SUIT, LEATHER, CLR_RED),
ARMOR("robe of protection", "blue robe",
	0, 1, 0, PROTECTION,	1, 1,  40,  50,  5, 0, ARM_SUIT, LEATHER, CLR_BLUE),
ARMOR("robe of power", "orange robe",
	0, 1, 0, 0,		0, 1,  40,  50,  9, 0, ARM_SUIT, LEATHER, CLR_ORANGE),
ARMOR("robe of weakness", "green robe",
	0, 1, 0, 0,		1, 1,  40,  50,  9, 0, ARM_SUIT, LEATHER, CLR_GREEN),

/*
 * Dragon suits
 * There is code in polyself.c that assumes (1) and (2).
 * There is code in obj.h, objnam.c, mon.c, read.c that assumes (2).
 *
 *	(1) The dragon scale mails and the dragon scales are together.
 *	(2) That the order of the dragon scale mail and dragon scales is the
 *	    the same defined in monst.c.
 */
#define DRGN_ARMR(name,mgc,power,cost,ac,color) \
	ARMOR(name,(char *)0,1,mgc,1,power,0,5,50,cost,ac,0,ARM_SUIT,DRAGON_HIDE,color)
/* 3.4.1: dragon scale mail reclassified as "magic" since magic is
   needed to create them */
DRGN_ARMR("gray dragon scale mail",   1, ANTIMAGIC,  1200, 1, CLR_GRAY),
DRGN_ARMR("silver dragon scale mail", 1, REFLECTING, 1200, 1, SILVER),
DRGN_ARMR("shimmering dragon scale mail", 1, DISPLACED, 1200, 1, CLR_CYAN),
DRGN_ARMR("deep dragon scale mail",   1, DRAIN_RES,  1200, 1, CLR_MAGENTA),
DRGN_ARMR("red dragon scale mail",    1, FIRE_RES,    900, 1, CLR_RED),
DRGN_ARMR("white dragon scale mail",  1, COLD_RES,    900, 1, CLR_WHITE),
DRGN_ARMR("orange dragon scale mail", 1, SLEEP_RES,   900, 1, CLR_ORANGE),
DRGN_ARMR("black dragon scale mail",  1, DISINT_RES, 1200, 1, CLR_BLACK),
DRGN_ARMR("blue dragon scale mail",   1, SHOCK_RES,   900, 1, CLR_BLUE),
DRGN_ARMR("green dragon scale mail",  1, POISON_RES,  900, 1, CLR_GREEN),
DRGN_ARMR("yellow dragon scale mail", 1, ACID_RES,    900, 1, CLR_YELLOW),

/* For now, only dragons leave these. */
/* 3.4.1: dragon scales left classified as "non-magic"; they confer
   magical properties but are produced "naturally" */
DRGN_ARMR("gray dragon scales",   0, ANTIMAGIC,  700, 7, CLR_GRAY),
DRGN_ARMR("silver dragon scales", 0, REFLECTING, 700, 7, SILVER),
DRGN_ARMR("shimmering dragon scales", 0, DISPLACED,  700, 7, CLR_CYAN),
DRGN_ARMR("deep dragon scales",   0, DRAIN_RES,  500, 7, CLR_MAGENTA),
DRGN_ARMR("red dragon scales",    0, FIRE_RES,   500, 7, CLR_RED),
DRGN_ARMR("white dragon scales",  0, COLD_RES,   500, 7, CLR_WHITE),
DRGN_ARMR("orange dragon scales", 0, SLEEP_RES,  500, 7, CLR_ORANGE),
DRGN_ARMR("black dragon scales",  0, DISINT_RES, 700, 7, CLR_BLACK),
DRGN_ARMR("blue dragon scales",   0, SHOCK_RES,  500, 7, CLR_BLUE),
DRGN_ARMR("green dragon scales",  0, POISON_RES, 500, 7, CLR_GREEN),
DRGN_ARMR("yellow dragon scales", 0, ACID_RES,   500, 7, CLR_YELLOW),
#undef DRGN_ARMR

/* Cloaks */
/*  'cope' is not a spelling mistake... leave it be */
CLOAK("mummy wrapping", (char *)0,
		1, 0,	0,	    0, 0,  3,  2, 10, 1, CLOTH, CLR_GRAY),
CLOAK("orcish cloak", "coarse mantelet",
		0, 0,	0,	    8, 0, 10, 40, 10, 2, CLOTH, CLR_BLACK),
CLOAK("dwarvish cloak", "hooded cloak",
		0, 0,	0,	    8, 0, 10, 50, 10, 2, CLOTH, HI_CLOTH),
CLOAK("oilskin cloak", "slippery cloak",
		0, 0,	0,	    8, 0, 10, 50,  9, 3, CLOTH, HI_CLOTH),
CLOAK("elven cloak", "faded pall",
		0, 1,	STEALTH,    8, 0, 10, 60,  9, 3, CLOTH, CLR_BLACK),
CLOAK("lab coat", "white coat",
		0, 1,   POISON_RES,10, 0, 10, 60,  9, 3, CLOTH, CLR_WHITE),
CLOAK("leather cloak", (char *)0,
		1, 0,	0,	    8, 0, 15, 40,  9, 1, LEATHER, CLR_BROWN),
#if 0
CLOAK("robe", (char *)0,
		1, 1,	0,	    3, 0, 15, 50,  8, 3, CLOTH, CLR_RED),
CLOAK("alchemy smock", "apron",
		0, 1,	POISON_RES, 9, 0, 10, 50,  9, 1, CLOTH, CLR_WHITE),
#endif
/* With shuffled appearances... */
CLOAK("cloak of protection", "tattered cape",
		0, 1,	PROTECTION, 9, 0, 10, 50,  7, 3, CLOTH, HI_CLOTH),
CLOAK("poisonous cloak", "dirty rag",
		0, 1,   0,          5, 0, 10, 40, 10, 3, CLOTH, CLR_GREEN),
CLOAK("cloak of invisibility", "opera cloak",
		0, 1,	INVIS,	   10, 0, 10, 60,  9, 2, CLOTH, CLR_BRIGHT_MAGENTA),
CLOAK("cloak of magic resistance", "ornamental cope",
		0, 1,	ANTIMAGIC,  2, 0, 10, 60,  9, 3, CLOTH, CLR_WHITE),
CLOAK("cloak of displacement", "piece of cloth",
		0, 1,	DISPLACED, 10, 0, 10, 50,  9, 2, CLOTH, HI_CLOTH),
/* Helmets */
HELM("elven leather helm", "leather hat",
		0, 0, 0,			6, 1,  3,   8, 9, 0, LEATHER, HI_LEATHER),
HELM("orcish helm", "iron skull cap",
		0, 0, 0,			6, 1, 30,  10, 9, 0, IRON, CLR_BLACK),
HELM("dwarvish iron helm", "hard hat",
		0, 0, 0,			6, 1, 40,  20, 8, 0, IRON, HI_METAL),
HELM("fedora", (char *)0,
		1, 0, 0,			0, 0,  3,   1,10, 0, CLOTH, CLR_BROWN),
HELM("cornuthaum", "conical hat",
		0, 1, CLAIRVOYANT,	3, 1,  4,  80,10, 2, CLOTH, CLR_BLUE),
HELM("dunce cap", "conical hat",
		0, 1, 0,			3, 1,  4,   1,10, 0, CLOTH, CLR_BLUE),
HELM("dented pot", (char *)0,
		1, 0, 0,			2, 0, 10,   8, 9, 0, IRON, CLR_BLACK),
/* ...with shuffled appearances */
HELM("helmet", "plumed helmet",
		0, 0, 0,		   10, 1, 30,  10, 9, 0, IRON, HI_METAL),
HELM("helm of brilliance", "etched helmet",
		0, 1, 0,			6, 1, 50,  50, 9, 0, IRON, CLR_GREEN),
HELM("helm of opposite alignment", "crested helmet",
		0, 1, 0,			6, 1, 50,  50, 9, 0, IRON, HI_METAL),
HELM("helm of telepathy", "visored helmet",
		0, 1, TELEPAT,		2, 1, 50,  50, 9, 0, IRON, HI_METAL),

/* Gloves */
/* these have their color but not material shuffled, so the IRON must stay
 * CLR_BROWN (== HI_LEATHER)
 */
GLOVES("leather gloves", "old gloves",
		0, 0,  0,			16, 1, 10,  8,  9, 0, LEATHER, HI_LEATHER),
GLOVES("gauntlets of fumbling", "padded gloves",
		0, 1,  FUMBLING,	8, 1, 10, 50,  9, 0, LEATHER, HI_LEATHER),
GLOVES("gauntlets of power", "riding gloves",
		0, 1,  0,			8, 1, 30, 50,  9, 0, IRON, CLR_BROWN),
GLOVES("gauntlets of swimming", "black gloves",
		0, 1,  SWIMMING,	8, 1, 10, 50,  9, 0, LEATHER, HI_LEATHER),
GLOVES("gauntlets of dexterity", "fencing gloves",
		0, 1,  0,			8, 1, 10, 50,  9, 0, LEATHER, HI_LEATHER),

/* Shields */
SHIELD("small shield", (char *)0,
		1, 0, 0, 0,	     6, 0, 30,	3,  9, 0, WOOD, HI_WOOD),
/* Elven ... orcish shields can't be differentiated by feel */
SHIELD("elven shield", "blue and green shield",
		0, 0, 0, 0,	     2, 0, 50,	7,  8, 0, WOOD, CLR_GREEN),
SHIELD("Uruk-hai shield", "white-handed shield",
		0, 0, 0, 0,	     2, 0, 50,	7,  9, 0, IRON, HI_METAL),
SHIELD("orcish shield", "red-eyed shield",
		0, 0, 0, 0,	     2, 0, 50,	7,  9, 0, IRON, CLR_RED),
SHIELD("large shield", (char *)0,
		1, 0, 1, 0,	     7, 0,100, 10,  8, 0, IRON, HI_METAL),
SHIELD("dwarvish roundshield", "large round shield",
		0, 0, 0, 0,	     4, 0,100, 10,  8, 0, IRON, HI_METAL),
SHIELD("shield of reflection", "polished silver shield",
		0, 1, 0, REFLECTING, 3, 0, 50, 50,  8, 0, SILVER, HI_SILVER),

/* Boots */
BOOTS("low boots", "walking shoes",
		0, 0,  0,	  25, 2, 10,  8,  9, 0, LEATHER, HI_LEATHER),
BOOTS("iron shoes", "hard shoes",
		0, 0,  0,	   7, 2, 50, 16,  8, 0, IRON, HI_METAL),
BOOTS("high boots", "jackboots",
		0, 0,  0,	  15, 2, 20, 12,  8, 0, LEATHER, HI_LEATHER),
/* ...with shuffled appearances */
BOOTS("speed boots", "combat boots",
		0, 1,  FAST,	  12, 2, 20, 50,  9, 0, LEATHER, HI_LEATHER),
/* With shuffled appearances... */
BOOTS("water walking boots", "jungle boots",
		0, 1,  WWALKING,  12, 2, 20, 50,  9, 0, LEATHER, HI_LEATHER),
BOOTS("jumping boots", "hiking boots",
		0, 1,  JUMPING,   12, 2, 20, 50,  9, 0, LEATHER, HI_LEATHER),
BOOTS("elven boots", "mud boots",
		0, 1,  STEALTH,   12, 2, 15,  8,  9, 0, LEATHER, HI_LEATHER),
BOOTS("kicking boots", "steel boots",
		0, 1,  0,         12, 2, 15,  8,  9, 0, IRON, CLR_BROWN),
BOOTS("fumble boots", "riding boots",
		0, 1,  FUMBLING,  12, 2, 20, 30,  9, 0, LEATHER, HI_LEATHER),
BOOTS("levitation boots", "snow boots",
		0, 1,  LEVITATION,12, 2, 15, 30,  9, 0, LEATHER, HI_LEATHER),

#undef HELM
#undef CLOAK
#undef SHIELD
#undef GLOVES
#undef BOOTS
#undef ARMOR

/* rings ... */
/* [Tom] looks like there are no probs to change... */
#define RING(name,power,stone,cost,mgc,spec,mohs,metal,color) OBJECT( \
		OBJ(name,stone), \
		BITS(0,0,spec,0,mgc,spec,0,0,0,HARDGEM(mohs),0,0,metal), \
		power, RING_CLASS, 0, 0, 3, cost, 0, 0, 0, 0, 15, color )
RING("adornment", ADORNED, "wooden",        100, 1, 1, 2, WOOD, HI_WOOD),
RING("hunger",     HUNGER,     "topaz",     100, 1, 0, 8, GEMSTONE, CLR_CYAN),
RING("mood",       0,          "ridged",    100, 1, 0, 8, IRON, HI_METAL),
RING("protection", PROTECTION, "black onyx",100, 1, 1, 7, MINERAL, CLR_BLACK),
RING("protection from shape changers", PROT_FROM_SHAPE_CHANGERS, "shiny",
					    					100, 1, 0, 5, IRON, CLR_BRIGHT_CYAN),
RING("sleeping",   SLEEPING,   "wedding",   100, 1, 0, 7, GEMSTONE, CLR_WHITE),
RING("stealth", STEALTH, "jade",            100, 1, 0, 6, GEMSTONE, CLR_GREEN),
RING("sustain ability", FIXED_ABIL, "bronze",
					    100, 1, 0, 4, COPPER, HI_COPPER),
RING("warning",    WARNING,    "diamond",   100, 1, 0,10, GEMSTONE, CLR_WHITE),
RING("aggravate monster", AGGRAVATE_MONSTER, "sapphire",
					    150, 1, 0, 9, GEMSTONE, CLR_BLUE),
RING("cold resistance", COLD_RES, "brass",  150, 1, 0, 4, COPPER, HI_COPPER),
RING("gain constitution", 0,   "opal",      150, 1, 1, 7, MINERAL,  HI_MINERAL),
RING("gain dexterity",  0,     "obsidian",  150, 1, 1, 7, GEMSTONE, CLR_BLACK),
RING("gain intelligence", 0,   "plain",     150, 1, 1, 7, MINERAL,  HI_MINERAL),
RING("gain strength", 0,       "granite",   150, 1, 1, 7, MINERAL, HI_MINERAL),
RING("gain wisdom",     0,     "glass",     150, 1, 1, 7, MINERAL,  CLR_CYAN),
RING("increase accuracy", 0,   "clay",      150, 1, 1, 4, MINERAL, CLR_RED),
RING("increase damage", 0,     "coral",     150, 1, 1, 4, MINERAL, CLR_ORANGE),
RING("slow digestion",  SLOW_DIGESTION, "steel",
					    200, 1, 0, 8, IRON, HI_METAL),
RING("invisibility", INVIS,    "wire",      150, 1, 0, 5, IRON, HI_METAL),
RING("poison resistance", POISON_RES, "pearl",
					    150, 1, 0, 4, IRON, CLR_WHITE),
RING("see invisible", SEE_INVIS, "engagement",
											150, 1, 0, 5, IRON, HI_METAL),
RING("shock resistance", SHOCK_RES, "copper",
					    150, 1, 0, 3, COPPER, HI_COPPER),
RING("fire resistance", FIRE_RES, "iron",   200, 1, 0, 5, IRON, HI_METAL),
RING("free action",     FREE_ACTION, "twisted",
					    200, 1, 0, 6, IRON, HI_METAL),
/*RING("infravision", 0, "zinc",              200, 1, 0, 5, MITHRIL, HI_METAL),*/
RING("levitation", LEVITATION, "agate",     200, 1, 0, 7, GEMSTONE, CLR_RED),
RING("regeneration", REGENERATION, "moonstone",
											200, 1, 0, 6, MINERAL, HI_MINERAL),
RING("searching",  SEARCHING,  "tiger eye", 200, 1, 0, 6, GEMSTONE, CLR_BROWN),
RING("teleportation", TELEPORT, "silver",   200, 1, 0, 3, SILVER, HI_SILVER),
RING("conflict",   CONFLICT, "ruby",        300, 1, 0, 9, GEMSTONE, CLR_RED),
RING("polymorph", POLYMORPH, "ivory",       300, 1, 0, 4, BONE, CLR_WHITE),
RING("polymorph control", POLYMORPH_CONTROL, "emerald",
					    300, 1, 0, 8, GEMSTONE, CLR_BRIGHT_GREEN),
RING("teleport control", TELEPORT_CONTROL, "gold",
											300, 1, 0, 3, GOLD, HI_GOLD),
/* More descriptions: cameo, intaglio */
#undef RING

/* amulets ... - THE Amulet comes last because it is special */
#define AMULET(name,desc,power,prob) OBJECT( \
		OBJ(name,desc), BITS(0,0,0,0,1,0,0,0,0,0,0,0,IRON), power, \
		AMULET_CLASS, prob, 0, 20, 150, 0, 0, 0, 0, 20, HI_METAL )
AMULET("amulet of change",        "square",     0,          110),
AMULET("amulet of drain resistance","warped",   DRAIN_RES,   60),
AMULET("amulet of ESP",           "circular",   TELEPAT,    140),
AMULET("amulet of flying",        "convex",     FLYING,      50),
AMULET("amulet of life saving",   "spherical",  LIFESAVED,   60),
AMULET("amulet of magical breathing", "octagonal", MAGICAL_BREATHING, 50),
AMULET("amulet of reflection",    "hexagonal",  REFLECTING,  60),
AMULET("amulet of restful sleep", "triangular", SLEEPING,   110),
AMULET("amulet of strangulation", "oval",       STRANGLED,  110),
AMULET("amulet of unchanging",    "concave",    UNCHANGING,	 50),
AMULET("amulet versus poison",    "pyramidal",  POISON_RES, 140),
AMULET("amulet versus stone",     "lunate",   /*STONE_RES*/0,60),
OBJECT(OBJ("cheap plastic imitation of the Amulet of Yendor",
	"Amulet of Yendor"), BITS(0,0,1,0,0,0,0,0,0,0,0,0,PLASTIC), 0,
	AMULET_CLASS, 0, 0, 20,    0, 0, 0, 0, 0,  1, HI_METAL),
OBJECT(OBJ("Amulet of Yendor",	/* note: description == name */
	"Amulet of Yendor"), BITS(0,0,1,0,1,0,1,1,0,0,0,0,MITHRIL), 0,
	AMULET_CLASS, 0, 0, 20, 30000, 0, 0, 0, 0, 20, HI_METAL),
#undef AMULET

/* tools ... */
/* tools with weapon characteristics come last */
#define TOOL(name,desc,kn,mrg,mgc,chg,prob,wt,cost,mat,color) \
	OBJECT( OBJ(name,desc), \
		BITS(kn,mrg,chg,0,mgc,chg,0,0,0,0,0,P_NONE,mat), \
		0, TOOL_CLASS, prob, 0, \
		wt, cost, 0, 0, 0, 0, wt, color )
#define CONTAINER(name,desc,kn,mgc,chg,bi,prob,wt,cost,mat,color) \
	OBJECT( OBJ(name,desc), \
		BITS(kn,0,chg,1,mgc,chg,0,0,bi,0,0,P_NONE,mat), \
		0, TOOL_CLASS, prob, 0, \
		wt, cost, 0, 0, 0, 0, wt, color )
#define WEPTOOL(name,desc,kn,mgc,chg,bi,prob,wt,cost,sdam,ldam,hitbon,typ,sub,mat,clr) \
	OBJECT( OBJ(name,desc), \
		BITS(kn,0,1,chg,mgc,1,0,0,bi,0,typ,sub,mat), \
		0, TOOL_CLASS, prob, 0, \
		wt, cost, sdam, ldam, hitbon, 0, wt, clr )
/* Containers */
CONTAINER("large box", (char *)0,       1, 0, 0, 1,  40,350,   8, WOOD, HI_WOOD),
CONTAINER("chest", (char *)0,           1, 0, 0, 1,  35,600,  16, WOOD, HI_WOOD),
CONTAINER("ice box", (char *)0,         1, 0, 0, 1,  10,900,  42, PLASTIC, CLR_WHITE),
CONTAINER("sack", "bag",                0, 0, 0, 0,  40, 15,   2, CLOTH, HI_CLOTH),
CONTAINER("oilskin sack", "bag",        0, 0, 0, 0,  10, 15, 100, CLOTH, HI_CLOTH),
CONTAINER("bag of holding", "bag",      0, 1, 0, 0,  20, 15, 100, CLOTH, HI_CLOTH),
CONTAINER("bag of tricks", "bag",       0, 1, 1, 0,  20, 15, 100, CLOTH, HI_CLOTH),

/* Unlocking tools */
TOOL("skeleton key", "key",     0, 0, 0, 0,  80,  3,  10, IRON, HI_METAL),
#ifdef TOURIST
TOOL("lock pick", (char *)0,    1, 0, 0, 0,  60,  4,  20, IRON, HI_METAL),
TOOL("credit card", (char *)0,  1, 0, 0, 0,  15,  1,  10, PLASTIC, CLR_WHITE),
#else
TOOL("lock pick", (char *)0,    1, 0, 0, 0,  75,  4,  20, IRON, HI_METAL),
#endif

/* Light sources */
/* [Tom] made candles cheaper & more common */
TOOL("tallow candle", "candle", 0, 1, 0, 0,  50,  2,   1, WAX, CLR_WHITE),
TOOL("wax candle", "candle",    0, 1, 0, 0,  40,  2,   2, WAX, CLR_WHITE),
TOOL("magic candle",  "candle", 0, 1, 1, 0,   5,  2, 500, WAX, CLR_WHITE),
TOOL("oil lamp", "lamp",        0, 0, 0, 0,  25, 20,  10, COPPER, CLR_YELLOW),
TOOL("brass lantern", (char *)0,1, 0, 0, 0,  15, 30,  12, COPPER, CLR_YELLOW),
TOOL("magic lamp", "lamp",      0, 0, 1, 0,  10, 20,1000, COPPER, CLR_YELLOW),

/* Instruments */
/* KMH -- made less common */
TOOL("tin whistle", "whistle",  0, 0, 0, 0,  63,  3,  10, METAL, HI_METAL),
TOOL("magic whistle", "whistle",0, 0, 1, 0,  25,  3,  10, METAL, HI_METAL),
/* "If tin whistles are made out of tin, what do they make foghorns out of?" */
TOOL("wooden flute", "flute",   0, 0, 0, 0,   2,  5,  12, WOOD, HI_WOOD),
TOOL("magic flute", "flute",    0, 0, 1, 1,   1,  5,  36, WOOD, HI_WOOD),
TOOL("tooled horn", "horn",     0, 0, 0, 0,   2, 18,  15, BONE, CLR_WHITE),
TOOL("frost horn", "horn",      0, 0, 1, 1,   1, 18,  50, BONE, CLR_WHITE),
TOOL("fire horn", "horn",       0, 0, 1, 1,   1, 18,  50, BONE, CLR_WHITE),
TOOL("horn of plenty", "horn",  0, 0, 1, 1,   1, 18,  50, BONE, CLR_WHITE),
TOOL("wooden harp", "harp",     0, 0, 0, 0,   2, 30,  50, WOOD, HI_WOOD),
TOOL("magic harp", "harp",      0, 0, 1, 1,   1, 30,  50, WOOD, HI_WOOD),
TOOL("bell", (char *)0,         1, 0, 0, 0,   1, 30,  50, COPPER, HI_COPPER),
TOOL("bugle", (char *)0,        1, 0, 0, 0,   2, 10,  15, COPPER, HI_COPPER),
TOOL("leather drum", "drum",    0, 0, 0, 0,   2, 25,  25, LEATHER, HI_LEATHER),
TOOL("drum of earthquake", "drum",
                                0, 0, 1, 1,   1, 25,  25, LEATHER, HI_LEATHER),

/* Traps */
TOOL("land mine",(char *)0,     1, 0, 0, 0,   0,300, 180, IRON, CLR_RED),
TOOL("beartrap", (char *)0,     1, 0, 0, 0,   0,200,  60, IRON, HI_METAL),

/* Weapon-tools */
# ifdef P_SPOON
/* Added by Tsanth, in homage to Final Fantasy 2 */
/* KMH -- Not randomly generated (no damage!) */
WEPTOOL("spoon", (char *)0,
	1, 0, 0, 0,  0,  1, 5000,  0,  0, 0, WHACK,  -P_DART, PLATINUM, HI_METAL),
# endif /* P_SPOON */
#ifdef LIGHTSABERS
# ifdef D_SABER
WEPTOOL("pick-axe", (char *)0,
	1, 0, 0, 0, 17, 80,   50,  6,  3, 0, WHACK,  P_PICK_AXE, IRON, HI_METAL),
# else
WEPTOOL("pick-axe", (char *)0,
	1, 0, 0, 0, 18, 80,   50,  6,  3, 0, WHACK,  P_PICK_AXE, IRON, HI_METAL),
# endif
#else	/* LIGHTSABERS */
WEPTOOL("pick-axe", (char *)0,
	1, 0, 0, 0, 20, 80,   50,  6,  3, 0, WHACK,  P_PICK_AXE, IRON, HI_METAL),
#endif
WEPTOOL("fishing pole", (char *)0,
	1, 0, 0, 0,  5,  30,  50,  2,  6, 0, WHACK,  P_POLEARMS, METAL, HI_METAL),
WEPTOOL("grappling hook", "iron hook",
	0, 0, 0, 0,  5,  30,  50,  2,  6, 0, WHACK,  P_FLAIL, IRON, HI_METAL),
/* 3.4.1: unicorn horn left classified as "magic" */
WEPTOOL("unicorn horn", (char *)0,
	1, 1, 0, 1,  0,  20, 100, 12, 12, 0, PIERCE, P_UNICORN_HORN, BONE, CLR_WHITE),
/* WEPTOOL("torch", (char *)0,
	   1, 0, 0,  0,  25, 8, 5, 2, WHACK, P_CLUB, WOOD, HI_WOOD), */

OBJECT(OBJ("torch", (char *)0),
	BITS(1,1,1,0,0,1,0,0,0,0,WHACK,P_CLUB,WOOD),
	0, TOOL_CLASS, 25, 0,
	20, 8, 2, 5, WHACK, 0, 20, HI_WOOD ),

#ifdef LIGHTSABERS
/* [WAC]
 * Lightsabers are -3 to hit 
 * Double lightsaber is -4 to hit (only red)
 * DMG is increased: 10.5/15.5
 * green :9 + d3, 13 + d5
 * blue : 8 + d5, 12 + d7
 * red :  6 + d9, 10 + d11
 * red double: 6 + d9 + d9, 10 + d11 + d11  (15/21) in double mode
 */
WEPTOOL("green lightsaber", "lightsaber",
	0, 0, 1, 0,  1, 60, 500, 3,  5, -3, SLASH, P_LIGHTSABER, PLASTIC, HI_METAL),
# ifdef D_SABER
WEPTOOL("blue lightsaber",  "lightsaber",
	0, 0, 1, 0,  1, 60, 500, 5,  7, -3, SLASH, P_LIGHTSABER, PLATINUM, HI_METAL),
# endif /* D_SABER */
WEPTOOL("red lightsaber",  "lightsaber",
	0, 0, 1, 0,  1, 60, 500, 9,  11, -3, SLASH, P_LIGHTSABER, PLATINUM, HI_METAL),
WEPTOOL("red double lightsaber",  "double lightsaber",
	0, 0, 1, 1,  0, 60,1000, 9,  11, -4, SLASH, P_LIGHTSABER, PLATINUM, HI_METAL),
#endif	/* LIGHTSABERS */

/* Other tools */
#ifdef TOURIST
TOOL("expensive camera", (char *)0,
				1, 0, 0, 1,  10, 12, 200, PLASTIC, CLR_BLACK),
TOOL("mirror", "looking glass", 0, 0, 0, 0,  40, 13,  10, GLASS, HI_SILVER),
#else
TOOL("mirror", "looking glass", 0, 0, 0, 0,  50, 13,  10, GLASS, HI_SILVER),
#endif
TOOL("crystal ball", "glass orb",
								0, 0, 1, 1,  10,150,  60, GLASS, HI_GLASS),
#if 0
/* STEPHEN WHITE'S NEW CODE */
/* KMH -- removed because there's potential for abuse */
TOOL("orb of enchantment", "glass orb",
				0, 0, 1, 1,   5, 75, 750, GLASS, HI_GLASS),
TOOL("orb of charging", "glass orb",
				0, 0, 1, 1,   5, 75, 750, GLASS, HI_GLASS),
TOOL("orb of destruction", "glass orb",
				0, 0, 0, 0,   5, 75, 750, GLASS, HI_GLASS),
#endif
TOOL("lenses", (char *)0,	1, 0, 0, 0,   5,  3,  80, GLASS, HI_GLASS),
TOOL("blindfold", (char *)0,    1, 0, 0, 0,  50,  2,  20, CLOTH, CLR_BLACK),
TOOL("towel", (char *)0,        1, 0, 0, 0,  50,  2,  50, CLOTH, CLR_MAGENTA),
#ifdef STEED
TOOL("saddle", (char *)0,       1, 0, 0, 0,   5,100, 150, LEATHER, HI_LEATHER),
TOOL("leash", (char *)0,        1, 0, 0, 0,  65, 12,  20, LEATHER, HI_LEATHER),
#else
TOOL("leash", (char *)0,        1, 0, 0, 0,  70, 12,  20, LEATHER, HI_LEATHER),
#endif
TOOL("stethoscope", (char *)0,  1, 0, 0, 0,  25,  4,  75, IRON, HI_METAL),
TOOL("tinning kit", (char *)0,  1, 0, 0, 1,  15, 75,  30, IRON, HI_METAL),
CONTAINER("medical kit", "leather bag",
				0, 0, 0, 0,  10, 25, 500, LEATHER, HI_LEATHER),
TOOL("tin opener", (char *)0,   1, 0, 0, 0,  25,  4,  30, IRON, HI_METAL),
TOOL("can of grease", (char *)0,1, 0, 0, 1,  15, 15,  20, IRON, HI_METAL),
TOOL("figurine", (char *)0,     1, 0, 1, 0,  25, 50,  80, MINERAL, HI_MINERAL),
TOOL("magic marker", (char *)0, 1, 0, 1, 1,  15,  2,  50, PLASTIC, CLR_RED),

/* Two pseudo tools. These can never exist outside of medical kits. */
OBJECT(OBJ("bandage", (char *)0),
		BITS(1,1,0,0,0,0,0,1,0,0,0,P_NONE,CLOTH), 0,
		TOOL_CLASS, 0, 0, 1, 1, 0, 0, 0, 0, 0, CLR_WHITE),
OBJECT(OBJ("phial", (char *)0),
		BITS(1,1,0,0,0,0,0,1,0,0,0,P_NONE,GLASS), 0,
		TOOL_CLASS, 0, 0, 2, 1, 0, 0, 0, 0, 1, HI_GLASS),

/* Two special unique artifact "tools" */
OBJECT(OBJ("Candelabrum of Invocation", "candelabrum"),
		BITS(0,0,1,0,1,0,1,1,0,0,0,0,GOLD), 0,
		TOOL_CLASS, 0, 0,10, 5000, 0, 0, 0, 0, 200, HI_GOLD),
OBJECT(OBJ("Bell of Opening", "silver bell"),
		BITS(0,0,1,0,1,1,1,1,0,0,0,0,SILVER), 0,
		TOOL_CLASS, 0, 0,10, 5000, 0, 0, 0, 0, 50, HI_SILVER),
#undef TOOL
#undef CONTAINER
#undef WEPTOOL


/* Comestibles ... */
#define FOOD(name,prob,delay,wt,unk,tin,nutrition,color) OBJECT( \
		OBJ(name,(char *)0), BITS(1,1,unk,0,0,0,0,0,0,0,0,0,tin), 0, \
		FOOD_CLASS, prob, delay, \
		wt, nutrition/20 + 5, 0, 0, 0, 0, nutrition, color )
/* all types of food (except tins & corpses) must have a delay of at least 1. */
/* delay on corpses is computed and is weight dependant */
/* dog eats foods 0-4 but prefers tripe rations above all others */
/* fortune cookies can be read */
/* carrots improve your vision */
/* +0 tins contain monster meat */
/* +1 tins (of spinach) make you stronger (like Popeye) */
/* food CORPSE is a cadaver of some type */
/* meatballs/sticks/rings are only created from objects via stone to flesh */

/* Meat */
FOOD("tripe ration",       142, 2, 10, 0, FLESH, 200, CLR_BROWN),
FOOD("corpse",               0, 1,  0, 0, FLESH,   0, CLR_BROWN),
FOOD("egg",                 80, 1,  1, 1, FLESH,  80, CLR_WHITE),
FOOD("meatball",             0, 1,  1, 0, FLESH,   5, CLR_BROWN),
FOOD("meat stick",           0, 1,  1, 0, FLESH,   5, CLR_BROWN),
FOOD("huge chunk of meat",   0,20,400, 0, FLESH,2000, CLR_BROWN),
/* special case because it's not mergable */
OBJECT(OBJ("meat ring", (char *)0),
    BITS(1,0,0,0,0,0,0,0,0,0,0,0,FLESH),
    0, FOOD_CLASS, 0, 1, 5, 1, 0, 0, 0, 0, 5, CLR_BROWN),


/* Body parts.... eeeww */
FOOD("eyeball",              0, 1,  0, 0, FLESH,  10, CLR_WHITE),
FOOD("severed hand",         0, 1,  0, 0, FLESH,  40, CLR_BROWN),

/* Fruits & veggies */
FOOD("kelp frond",           0, 1,  1, 0, VEGGY,  30, CLR_GREEN),
FOOD("eucalyptus leaf",      4, 1,  1, 0, VEGGY,  30, CLR_GREEN),
FOOD("clove of garlic",      7, 1,  1, 0, VEGGY,  40, CLR_WHITE),
FOOD("sprig of wolfsbane",   7, 1,  1, 0, VEGGY,  40, CLR_GREEN),
FOOD("apple",               13, 1,  2, 0, VEGGY,  50, CLR_RED),
FOOD("carrot",              15, 1,  2, 0, VEGGY,  50, CLR_ORANGE),
FOOD("pear",                 9, 1,  2, 0, VEGGY,  50, CLR_BRIGHT_GREEN),
FOOD("asian pear",           1, 1,  2, 0, VEGGY,  75, CLR_BRIGHT_GREEN),
FOOD("banana",              10, 1,  2, 0, VEGGY,  80, CLR_YELLOW),
FOOD("orange",              10, 1,  2, 0, VEGGY,  80, CLR_ORANGE),
FOOD("mushroom",             1, 1,  5, 0, VEGGY,  90, CLR_BLACK),
FOOD("melon",                9, 1,  5, 0, VEGGY, 100, CLR_BRIGHT_GREEN),
FOOD("slime mold",          75, 1,  5, 0, VEGGY, 250, HI_ORGANIC),

/* People food */
FOOD("lump of royal jelly",  1, 1,  2, 0, VEGGY, 200, CLR_YELLOW),
FOOD("cream pie",           10, 1, 10, 0, VEGGY, 100, CLR_WHITE),
FOOD("sandwich",            10, 1, 10, 0, FLESH, 100, CLR_WHITE),
FOOD("candy bar",           13, 1,  2, 0, VEGGY, 100, CLR_BROWN),
FOOD("fortune cookie",      55, 1,  1, 0, VEGGY,  40, CLR_YELLOW),
FOOD("pancake",             14, 2,  2, 0, VEGGY, 200, CLR_YELLOW),
FOOD("tortilla",             1, 2,  2, 0, VEGGY,  80, CLR_WHITE),
/* [Tom] more food.... taken off pancake (25) */
FOOD("cheese",              10, 2,  2, 0, FLESH, 250, CLR_YELLOW),
FOOD("pill",                 1, 1,  1, 0, VEGGY,   0, CLR_BRIGHT_MAGENTA),
FOOD("holy wafer",           7, 1,  1, 0, VEGGY, 150, CLR_WHITE),
FOOD("lembas wafer",        20, 2,  5, 0, VEGGY, 800, CLR_WHITE),
FOOD("cram ration",         20, 3, 15, 0, VEGGY, 600, HI_ORGANIC),
FOOD("food ration",        380, 5, 20, 0, VEGGY, 800, HI_ORGANIC),
FOOD("K-ration",             0, 1, 10, 0, VEGGY, 400, HI_ORGANIC),
FOOD("C-ration",             0, 1, 10, 0, VEGGY, 300, HI_ORGANIC),
FOOD("tin",                 75, 0, 10, 1, METAL,   0, HI_METAL),
#undef FOOD

/* potions ... */
#define POTION(name,desc,mgc,power,prob,cost,color) OBJECT( \
		OBJ(name,desc), BITS(0,1,0,0,mgc,0,0,0,0,0,0,0,GLASS), power, \
		POTION_CLASS, prob, 0, 20, cost, 0, 0, 0, 0, 10, color )
POTION("booze", "brown",                0, 0,          40,  50, CLR_BROWN),
POTION("fruit juice", "dark",           0, 0,          40,  50, CLR_BLACK),
POTION("see invisible", "magenta",      1, SEE_INVIS,  38,  50, CLR_MAGENTA),
POTION("sickness", "fizzy",             0, 0,          40,  50, CLR_CYAN),
POTION("sleeping",  "effervescent",     1, 0,          40, 100, CLR_GRAY),
POTION("clairvoyance","luminescent",    1, 0,          20, 100, CLR_WHITE),
POTION("confusion", "orange",           1, CONFUSION,  40, 100, CLR_ORANGE),
POTION("hallucination", "sky blue",     1, HALLUC,     40, 100, CLR_CYAN),
POTION("healing", "purple-red",         1, 0,          55, 100, CLR_MAGENTA),
POTION("extra healing", "puce",         1, 0,          45, 100, CLR_RED),
POTION("restore ability", "pink",       1, 0,          40, 100, CLR_BRIGHT_MAGENTA),
POTION("blindness", "yellow",           1, BLINDED,    38, 150, CLR_YELLOW),
POTION("ESP", "muddy",                  1, TELEPAT,    20, 150, CLR_BROWN),
POTION("gain energy", "cloudy",         1, 0,          40, 150, CLR_WHITE),
POTION("invisibility", "brilliant blue",1, INVIS,      40, 150, CLR_BRIGHT_BLUE),
POTION("monster detection", "bubbly",   1, 0,          38, 150, CLR_WHITE),
POTION("object detection", "smoky",     1, 0,          38, 150, CLR_GRAY),
POTION("enlightenment", "swirly",       1, 0,          20, 200, CLR_BROWN),
POTION("full healing",  "black",        1, 0,          20, 200, CLR_BLACK),
POTION("levitation", "cyan",            1, LEVITATION, 38, 200, CLR_CYAN),
POTION("polymorph", "golden",           1, 0,          10, 200, CLR_YELLOW),
POTION("speed", "dark green",           1, FAST,       38, 200, CLR_GREEN),
POTION("acid", "white",                 0, 0,          20, 250, CLR_WHITE),
POTION("oil", "murky",                  0, 0,          30, 250, CLR_BROWN),
POTION("gain ability", "ruby",          1, 0,          38, 300, CLR_RED),
POTION("gain level", "milky",           1, 0,          20, 300, CLR_WHITE),
POTION("invulnerability", "icy",        1, 0,           5, 300, CLR_BRIGHT_BLUE),
POTION("paralysis", "emerald",          1, 0,          38, 300, CLR_BRIGHT_GREEN),
POTION("water", "clear",                0, 0,          55, 100, CLR_CYAN),
POTION("blood", "blood-red",            0, 0,           0,  50, CLR_RED),
POTION("vampire blood", "blood-red",    1, 0,           0, 350, CLR_RED),
POTION("amnesia", "sparkling",          1, 0,          16, 100, CLR_CYAN),
#undef POTION

/* scrolls ... */
#define SCROLL(name,text,sub,mgc,prob,cost) OBJECT( \
		OBJ(name,text), BITS(0,1,0,0,mgc,0,0,0,0,0,0,sub,PAPER), 0, \
		SCROLL_CLASS, prob, 0, 5, cost, 0, 0, 0, 0, 6, HI_PAPER )
/* Attack */
SCROLL("create monster",        "LEP GEX VEN ZEA",      P_ATTACK_SPELL, 1,  45, 200),
/* Enchantment */
SCROLL("taming",                "PRIRUTSENIE",          P_ENCHANTMENT_SPELL, 1,  15, 200),
/* Divination */
SCROLL("light",                 "VERR YED HORRE",       P_DIVINATION_SPELL, 1,  90,  50),
SCROLL("food detection",        "YUM YUM",              P_DIVINATION_SPELL, 1,  25, 100),
SCROLL("gold detection",        "THARR",                P_DIVINATION_SPELL, 1,  33, 100),
SCROLL("identify",              "KERNOD WEL",           P_DIVINATION_SPELL, 1, 185,  20),
SCROLL("magic mapping",         "ELAM EBOW",            P_DIVINATION_SPELL, 1,  45, 100),
/* Enchantment */
SCROLL("confuse monster",       "NR 9",                 P_ENCHANTMENT_SPELL,  1,  43, 100),
SCROLL("scare monster",         "XIXAXA XOXAXA XUXAXA", P_ENCHANTMENT_SPELL,  1,  35, 100),
SCROLL("enchant weapon",        "DAIYEN FOOELS",        P_ENCHANTMENT_SPELL,  1,  80,  60),
SCROLL("enchant armor",         "ZELGO MER",            P_ENCHANTMENT_SPELL,  1,  63,  80),
/* Protection */
SCROLL("remove curse",          "PRATYAVAYAH",          P_PROTECTION_SPELL, 1,  65,  80),
/* Body */
SCROLL("teleportation",         "VENZAR BORGAVVE",      P_BODY_SPELL, 1,  55, 100),
/* Matter */
SCROLL("fire",                  "ANDOVA BEGARIN",       P_MATTER_SPELL, 1,  33, 100),
SCROLL("earth",                 "KIRJE",                P_MATTER_SPELL, 1,  20, 200),

SCROLL("destroy armor",         "JUYED AWK YACC",       P_NONE, 1,  45, 100),
SCROLL("amnesia",               "DUAM XNAHT",           P_NONE, 1,  35, 200),
SCROLL("charging",              "HACKEM MUCHE",         P_NONE, 1,  15, 300),
SCROLL("genocide",              "ELBIB YLOH",           P_NONE, 1,  15, 300),
SCROLL("punishment",            "VE FORBRYDERNE",       P_NONE, 1,  15, 300),
SCROLL("stinking cloud",	"VELOX NEB",            P_NONE, 1,  15, 300),
SCROLL((char *)0,		"FOOBIE BLETCH",        P_NONE, 1,   0, 100),
SCROLL((char *)0,		"TEMOV",                P_NONE, 1,   0, 100),
SCROLL((char *)0,		"GARVEN DEH",           P_NONE, 1,   0, 100),
SCROLL((char *)0,		"READ ME",              P_NONE, 1,   0, 100),
/* these must come last because they have special descriptions */
#ifdef MAIL
SCROLL("mail",                  "stamped",              P_NONE, 0,   0,   0),
#endif
SCROLL("blank paper",           "unlabeled",            P_NONE, 0,  28,  60),
#undef SCROLL

/* spell books ... */
#define SPELL(name,desc,sub,prob,delay,level,mgc,dir,color) OBJECT( \
		OBJ(name,desc), BITS(0,0,1,0,mgc,1,0,0,0,0,dir,sub,PAPER), 0, \
		SPBOOK_CLASS, prob, delay, \
		50, level*100, 0, 0, 0, level, 20, color )
/* Attack spells */
SPELL("force bolt",      "red",         P_ATTACK_SPELL, 25,  2, 1, 1, IMMEDIATE, CLR_RED),
SPELL("create monster",  "turquoise",   P_ATTACK_SPELL, 25,  3, 2, 1, NODIR,     CLR_BRIGHT_CYAN),
SPELL("drain life",      "velvet",      P_ATTACK_SPELL, 10,  4, 3, 1, IMMEDIATE, CLR_MAGENTA),
  /* NEEDS TILE */ /* WAC -- probs from force bolt and extra healing */
SPELL("command undead",  "dark",        P_ATTACK_SPELL, 10,  7, 5, 1, IMMEDIATE, CLR_BLACK),
SPELL("summon undead",   "black",       P_ATTACK_SPELL, 10,  7, 5, 1, IMMEDIATE, CLR_BLACK),
/* Healing spells */
SPELL("stone to flesh",	 "thick",       P_HEALING_SPELL, 15,  1, 3, 1, IMMEDIATE, HI_PAPER),
SPELL("healing",         "white",       P_HEALING_SPELL, 30,  2, 1, 1, IMMEDIATE, CLR_WHITE),
SPELL("cure blindness",  "yellow",      P_HEALING_SPELL, 20,  2, 2, 1, IMMEDIATE, CLR_YELLOW),
SPELL("cure sickness",   "indigo",      P_HEALING_SPELL, 20,  3, 3, 1, NODIR,     CLR_BLUE),
SPELL("extra healing",   "plaid",       P_HEALING_SPELL, 15,  5, 3, 1, IMMEDIATE, CLR_GREEN),
SPELL("restore ability", "light brown", P_HEALING_SPELL, 15,  5, 4, 1, NODIR,     CLR_BROWN),
SPELL("create familiar", "glittering",  P_HEALING_SPELL, 10,  7, 6, 1, NODIR,     CLR_WHITE),
/* Divination spells */
SPELL("light",           "cloth",       P_DIVINATION_SPELL, 30,  1, 1, 1, NODIR,     HI_CLOTH),
SPELL("detect monsters", "leather",     P_DIVINATION_SPELL, 20,  1, 1, 1, NODIR,     HI_LEATHER),
SPELL("detect food",     "cyan",        P_DIVINATION_SPELL, 15,  3, 2, 1, NODIR,     CLR_CYAN),
SPELL("clairvoyance",    "dark blue",   P_DIVINATION_SPELL, 15,  3, 3, 1, NODIR,     CLR_BLUE),
SPELL("detect unseen",   "violet",      P_DIVINATION_SPELL, 15,  4, 3, 1, NODIR,     CLR_MAGENTA),
SPELL("identify",        "bronze",      P_DIVINATION_SPELL, 30,  8, 5, 1, NODIR,     HI_COPPER),
SPELL("detect treasure", "gray",        P_DIVINATION_SPELL, 25,  5, 4, 1, NODIR,     CLR_GRAY),
SPELL("magic mapping",   "dusty",       P_DIVINATION_SPELL, 15,  7, 5, 1, NODIR,     HI_PAPER),
/* Enchantment spells */
SPELL("confuse monster", "orange",      P_ENCHANTMENT_SPELL, 25,  2, 2, 1, IMMEDIATE, CLR_ORANGE),
SPELL("slow monster",    "light green", P_ENCHANTMENT_SPELL, 25,  2, 2, 1, IMMEDIATE, CLR_BRIGHT_GREEN),
SPELL("cause fear",      "light blue",  P_ENCHANTMENT_SPELL, 20,  3, 3, 1, NODIR,     CLR_BRIGHT_BLUE),
SPELL("charm monster",   "magenta",     P_ENCHANTMENT_SPELL, 20,  3, 3, 1, IMMEDIATE, CLR_MAGENTA),
SPELL("enchant weapon",  "dull",        P_ENCHANTMENT_SPELL,  5,  8, 7, 1, NODIR,     CLR_WHITE),
SPELL("enchant armor",   "thin",        P_ENCHANTMENT_SPELL,  5,  8, 7, 1, NODIR,     CLR_WHITE),
/* Protection spells */
SPELL("protection",      "wide",        P_PROTECTION_SPELL,  5,  3, 1, 1, NODIR,     HI_PAPER),
SPELL("resist poison",   "big",         P_PROTECTION_SPELL, 20,  2, 1, 1, NODIR,     CLR_GRAY),
SPELL("resist sleep",    "fuzzy",       P_PROTECTION_SPELL, 20,  2, 1, 1, NODIR,     CLR_BROWN),
SPELL("endure cold",     "deep",        P_PROTECTION_SPELL, 15,  3, 2, 1, NODIR,     HI_PAPER),
SPELL("endure heat",     "spotted",     P_PROTECTION_SPELL, 15,  3, 2, 1, NODIR,     HI_PAPER),
SPELL("insulate",        "long",        P_PROTECTION_SPELL, 15,  3, 2, 1, NODIR,     HI_PAPER),
SPELL("remove curse",    "wrinkled",    P_PROTECTION_SPELL, 25,  5, 5, 1, NODIR,     HI_PAPER),
SPELL("turn undead",     "copper",      P_PROTECTION_SPELL, 15,  8, 6, 1, IMMEDIATE, HI_COPPER),
/* Body spells */
SPELL("jumping",         "torn",        P_BODY_SPELL, 15,  3, 1, 1, IMMEDIATE, HI_PAPER),
SPELL("haste self",      "purple",      P_BODY_SPELL, 20,  4, 3, 1, NODIR,     CLR_MAGENTA),
SPELL("enlighten",       "faded",       P_BODY_SPELL, 15,  5, 4, 1, NODIR,     CLR_GRAY),
SPELL("invisibility",    "dark brown",  P_BODY_SPELL, 20,  5, 4, 1, NODIR,     CLR_BROWN),
SPELL("levitation",      "tan",         P_BODY_SPELL, 15,  4, 4, 1, NODIR,     CLR_BROWN),
SPELL("teleport away",   "gold",        P_BODY_SPELL, 15,  6, 6, 1, IMMEDIATE, HI_GOLD),
SPELL("passwall",        "ochre",       P_BODY_SPELL,  5,  7, 6, 1, NODIR,     CLR_YELLOW),
SPELL("polymorph",       "silver",      P_BODY_SPELL, 15,  8, 6, 1, IMMEDIATE, HI_SILVER),
/* Matter spells */
SPELL("knock",           "pink",        P_MATTER_SPELL, 25,  1, 1, 1, IMMEDIATE, CLR_BRIGHT_MAGENTA),
SPELL("flame sphere",    "canvas",      P_MATTER_SPELL, 20,  2, 1, 1, NODIR, CLR_BROWN),
SPELL("freeze sphere",   "hardcover",   P_MATTER_SPELL, 20,  2, 1, 1, NODIR, CLR_BROWN),
SPELL("wizard lock",     "dark green",  P_MATTER_SPELL, 30,  3, 2, 1, IMMEDIATE, CLR_GREEN),
SPELL("dig",             "parchment",   P_MATTER_SPELL, 20,  6, 5, 1, RAY,       HI_PAPER),
SPELL("cancellation",    "shining",     P_MATTER_SPELL, 15,  8, 7, 1, IMMEDIATE, CLR_WHITE),
/* Ray type spells are all here.  Kludge for zap.c */
SPELL("magic missile",   "vellum",      P_ATTACK_SPELL, 40,  3, 2, 1, RAY,       HI_PAPER),
SPELL("fireball",        "ragged",      P_MATTER_SPELL, 15,  6, 4, 1, RAY,       HI_PAPER),
SPELL("cone of cold",    "dog eared",   P_MATTER_SPELL, 15,  8, 5, 1, RAY,       HI_PAPER),
SPELL("sleep",           "mottled",     P_ENCHANTMENT_SPELL, 35,  1, 1, 1, RAY,  HI_PAPER),
SPELL("finger of death", "stained",     P_ATTACK_SPELL,  5, 10, 7, 1, RAY,       HI_PAPER),
SPELL("lightning",       "rainbow",     P_MATTER_SPELL, 10,  7, 4, 1, RAY,       HI_PAPER),
SPELL("poison blast",    "tattered",    P_ATTACK_SPELL,  5,  7, 4, 1, RAY,       HI_PAPER),
SPELL("acid stream",     "colorful",    P_MATTER_SPELL,  5,  7, 4, 1, RAY,       HI_PAPER),
/* Description placeholders and special spellbooks */
SPELL((char *)0,		 "tartan",      P_NONE,  0,  0, 0, 1, 0,         CLR_RED),
SPELL((char *)0,		 "stylish",     P_NONE,  0,  0, 0, 1, 0,         HI_PAPER),
SPELL((char *)0,		 "psychedelic", P_NONE,  0,  0, 0, 1, 0,         CLR_BRIGHT_MAGENTA),
SPELL((char *)0,		 "spiral-bound",P_NONE,  0,  0, 0, 1, 0,         HI_PAPER),
SPELL((char *)0,		 "left-handed", P_NONE,  0,  0, 0, 1, 0,         HI_PAPER),
SPELL((char *)0,		 "stapled",     P_NONE,  0,  0, 0, 1, 0,         HI_PAPER),
SPELL("blank paper",	 "plain",		P_NONE, 20,  0, 0, 0, 0,         HI_PAPER),
	/* ...Blank spellbook must come last because it retains its description */
OBJECT(OBJ("Book of the Dead", "papyrus"),
	BITS(0,0,1,0,1,0,1,1,0,0,0,0,PAPER), 0,
	SPBOOK_CLASS, 0, 0,20, 10000, 0, 0, 0, 7, 20, HI_PAPER),
	/* ...A special, one of a kind, spellbook */
#undef SPELL

/* wands ... */
#define WAND(name,typ,prob,cost,mgc,dir,metal,color) OBJECT( \
		OBJ(name,typ), BITS(0,0,1,0,mgc,1,0,0,0,0,dir,0,metal), 0, \
		WAND_CLASS, prob, 0, 7, cost, 0, 0, 0, 0, 30, color )
WAND("light",          "glass",    50, 100, 1, NODIR,     GLASS,    HI_GLASS),
WAND("nothing",        "oak",      20, 100, 0, IMMEDIATE, WOOD,     HI_WOOD),
WAND("enlightenment",  "crystal",  30, 150, 1, NODIR,     GLASS,    HI_GLASS),
WAND("healing",        "bamboo",   60, 150, 1, IMMEDIATE, WOOD,     CLR_YELLOW),
WAND("locking",        "aluminum", 25, 150, 1, IMMEDIATE, METAL,    HI_METAL),
WAND("make invisible", "marble",   45, 150, 1, IMMEDIATE, MINERAL,  HI_MINERAL),
WAND("opening",        "zinc",     25, 150, 1, IMMEDIATE, METAL,    HI_METAL),
WAND("probing",        "uranium",  30, 150, 1, IMMEDIATE, METAL,    HI_METAL),
WAND("secret door detection", "balsa",
                                   30, 150, 1, NODIR,     WOOD,     HI_WOOD),
WAND("slow monster",   "tin",      45, 150, 1, IMMEDIATE, METAL,    HI_METAL),
WAND("speed monster",  "brass",    45, 150, 1, IMMEDIATE, COPPER,   HI_COPPER),
WAND("striking",       "ebony",    65, 150, 1, IMMEDIATE, WOOD,     HI_WOOD),
WAND("undead turning", "copper",   45, 150, 1, IMMEDIATE, COPPER,   HI_COPPER),
WAND("draining",       "ceramic",  15, 175, 1, IMMEDIATE, MINERAL,  HI_MINERAL),
		/* KMH -- 15/1000 probability from light */
WAND("cancellation",   "platinum", 45, 200, 1, IMMEDIATE, PLATINUM, CLR_WHITE),
WAND("create monster", "maple",    35, 200, 1, NODIR,     WOOD,     HI_WOOD),
WAND("fear",           "rusty",    25, 200, 1, IMMEDIATE, IRON,     CLR_RED),
WAND("polymorph",      "silver",   45, 200, 1, IMMEDIATE, SILVER,   HI_SILVER),
WAND("teleportation",  "iridium",  45, 200, 1, IMMEDIATE, METAL,    CLR_BRIGHT_CYAN),
WAND("create horde",   "black",     5, 300, 1, NODIR,     PLASTIC,  CLR_BLACK),
WAND("extra healing",  "bronze",   30, 300, 1, IMMEDIATE, COPPER,   CLR_YELLOW),
WAND("wishing",        "pine",      5, 500, 1, NODIR,     WOOD,     HI_WOOD),
/* Ray wands have to come last, and in this order. */
/* This is extremely kludgy, but that's what zap.c expects. */
WAND("digging",        "iron",     50, 150, 1, RAY,       IRON,     HI_METAL),
WAND("magic missile",  "steel",    50, 150, 1, RAY,       IRON,     HI_METAL),
WAND("fire",           "hexagonal",25, 175, 1, RAY,       IRON,     HI_METAL),
WAND("cold",           "short",    30, 175, 1, RAY,       IRON,     HI_METAL),
WAND("sleep",          "runed",    50, 175, 1, RAY,       IRON,     HI_METAL),
WAND("death",          "long",      5, 500, 1, RAY,       IRON,     HI_METAL),
WAND("lightning",      "curved",   20, 175, 1, RAY,       IRON,     HI_METAL),
WAND("fireball",       "octagonal", 5, 300, 1, RAY,       IRON,     HI_METAL),
WAND((char *)0,        "forked",    0, 150, 1, 0,         WOOD,     HI_WOOD),
WAND((char *)0,        "spiked",    0, 150, 1, 0,         IRON,     HI_METAL),
WAND((char *)0,        "jeweled",   0, 150, 1, 0,         IRON,     HI_MINERAL),
#undef WAND

/* coins ... - so far, gold is all there is */
#define COIN(name,prob,metal,worth) OBJECT( \
		OBJ(name,(char *)0), BITS(0,1,0,0,0,0,0,0,0,0,0,P_NONE,metal), 0, \
		COIN_CLASS, prob, 0, 1, worth, 0, 0, 0, 0, 0, HI_GOLD )
	COIN("gold piece",      1000, GOLD,1),
#undef COIN

/* gems ... - includes stones and rocks but not boulders */
#define GEM(name,desc,prob,wt,gval,nutr,mohs,glass,color) OBJECT( \
	    OBJ(name,desc), \
	    BITS(0,1,0,0,0,0,0,0,0,HARDGEM(mohs),0,-P_SLING,glass), 0, \
	    GEM_CLASS, prob, 0, 1, gval, 3, 3, 0, 0, nutr, color )
#define ROCK(name,desc,kn,prob,wt,gval,sdam,ldam,mgc,nutr,mohs,glass,color) OBJECT( \
	    OBJ(name,desc), \
	    BITS(kn,1,0,0,mgc,0,0,0,0,HARDGEM(mohs),0,-P_SLING,glass), 0, \
	    GEM_CLASS, prob, 0, wt, gval, sdam, ldam, 0, 0, nutr, color )
GEM("dilithium crystal", "white",      2,  1, 4500, 15,  5, GEMSTONE, CLR_WHITE),
GEM("diamond", "white",                3,  1, 4000, 15, 10, GEMSTONE, CLR_WHITE),
GEM("ruby", "red",                     4,  1, 3500, 15,  9, GEMSTONE, CLR_RED),
GEM("jacinth", "orange",               3,  1, 3250, 15,  9, GEMSTONE, CLR_ORANGE),
GEM("sapphire", "blue",                4,  1, 3000, 15,  9, GEMSTONE, CLR_BLUE),
GEM("black opal", "black",             3,  1, 2500, 15,  8, GEMSTONE, CLR_BLACK),
GEM("emerald", "green",                5,  1, 2500, 15,  8, GEMSTONE, CLR_GREEN),
GEM("turquoise", "green",              6,  1, 2000, 15,  6, GEMSTONE, CLR_GREEN),
GEM("citrine", "yellow",               4,  1, 1500, 15,  6, GEMSTONE, CLR_YELLOW),
GEM("aquamarine", "green",             6,  1, 1500, 15,  8, GEMSTONE, CLR_GREEN),
GEM("amber", "yellowish brown",        8,  1, 1000, 15,  2, GEMSTONE, CLR_BROWN),
GEM("topaz", "yellowish brown",       10,  1,  900, 15,  8, GEMSTONE, CLR_BROWN),
GEM("jet", "black",                    6,  1,  850, 15,  7, GEMSTONE, CLR_BLACK),
GEM("opal", "white",                  12,  1,  800, 15,  6, GEMSTONE, CLR_WHITE),
GEM("chrysoberyl", "yellow",           8,  1,  700, 15,  5, GEMSTONE, CLR_YELLOW),
GEM("garnet", "red",                  12,  1,  700, 15,  7, GEMSTONE, CLR_RED),
GEM("amethyst", "violet",             14,  1,  600, 15,  7, GEMSTONE, CLR_MAGENTA),
GEM("jasper", "red",                  15,  1,  500, 15,  7, GEMSTONE, CLR_RED),
GEM("fluorite", "violet",             15,  1,  400, 15,  4, GEMSTONE, CLR_MAGENTA),
GEM("obsidian", "black",               9,  1,  200, 15,  6, GEMSTONE, CLR_BLACK),
GEM("agate", "orange",                12,  1,  200, 15,  6, GEMSTONE, CLR_ORANGE),
GEM("jade", "green",                  10,  1,  300, 15,  6, GEMSTONE, CLR_GREEN),
GEM("worthless piece of white glass", "white",   76, 1, 0, 6, 5, GLASS, CLR_WHITE),
GEM("worthless piece of blue glass", "blue",     76, 1, 0, 6, 5, GLASS, CLR_BLUE),
GEM("worthless piece of red glass", "red",       76, 1, 0, 6, 5, GLASS, CLR_RED),
GEM("worthless piece of yellowish brown glass", "yellowish brown",
												 76, 1, 0, 6, 5, GLASS, CLR_BROWN),
GEM("worthless piece of orange glass", "orange", 76, 1, 0, 6, 5, GLASS, CLR_ORANGE),
GEM("worthless piece of yellow glass", "yellow", 76, 1, 0, 6, 5, GLASS, CLR_YELLOW),
GEM("worthless piece of black glass",  "black",  76, 1, 0, 6, 5, GLASS, CLR_BLACK),
GEM("worthless piece of green glass", "green",   76, 1, 0, 6, 5, GLASS, CLR_GREEN),
GEM("worthless piece of violet glass", "violet", 76, 1, 0, 6, 5, GLASS, CLR_MAGENTA),

/* Placement note: there is a wishable subrange for   
 * "gray stones" in the o_ranges[] array in objnam.c  
 * that is currently everything between luckstones and flint (inclusive).
 */
ROCK("luckstone", "gray",	0, 10,  10, 60, 3, 3, 1, 10, 7, MINERAL, CLR_GRAY),
ROCK("healthstone", "gray",	0,  8,  10, 60, 3, 3, 1, 10, 7, MINERAL, CLR_GRAY),
ROCK("loadstone", "gray",	0, 10, 500,  1, 3, 3, 1, 10, 6, MINERAL, CLR_GRAY),
ROCK("touchstone", "gray",	0,  4,  10, 45, 3, 3, 1, 10, 6, MINERAL, CLR_GRAY),
ROCK("whetstone", "gray",	0,  3,  10, 45, 3, 3, 1, 10, 7, MINERAL, CLR_GRAY),
ROCK("flint", "gray",		0, 10,  10,  1, 6, 6, 0, 10, 7, MINERAL, CLR_GRAY),
ROCK("rock", (char *)0,		1,100,  10,  0, 3, 3, 0, 10, 7, MINERAL, CLR_GRAY),
#undef GEM
#undef ROCK

/* miscellaneous ... */
/* Note: boulders and rocks are not normally created at random; the
 * probabilities only come into effect when you try to polymorph them.
 * Boulders weigh more than MAX_CARR_CAP; statues use corpsenm to take
 * on a specific type and may act as containers (both affect weight).
 */
OBJECT(OBJ("boulder",(char *)0), BITS(1,0,0,0,0,0,0,0,1,0,0,P_NONE,MINERAL), 0,
		ROCK_CLASS,   100, 0, 6000,  0, 20, 20, 0, 0, 2000, HI_MINERAL),
OBJECT(OBJ("statue", (char *)0), BITS(1,0,0,1,0,0,0,0,0,0,0,P_NONE,MINERAL), 0,
		ROCK_CLASS,   900, 0, 2500,  0, 20, 20, 0, 0, 2500, CLR_WHITE),

OBJECT(OBJ("heavy iron ball", (char *)0), BITS(1,0,0,0,0,0,0,0,0,0,WHACK,P_NONE,IRON), 0,
		BALL_CLASS,  1000, 0,  480, 10, 25, 25, 0, 0,  200, HI_METAL),
						/* +d4 when "very heavy" */
OBJECT(OBJ("iron chain", (char *)0), BITS(1,0,0,0,0,0,0,0,0,0,WHACK,P_NONE,IRON), 0,
		CHAIN_CLASS, 1000, 0,  120,  0,  4,  4, 0, 0,  200, HI_METAL),
						/* +1 both l & s */

OBJECT(OBJ("blinding venom", "splash of venom"),
		BITS(0,1,0,0,0,0,0,1,0,0,0,P_NONE,LIQUID), 0,
		VENOM_CLASS,  500, 0,	 1,  0,  0,  0, 0, 0,	 0, HI_ORGANIC),
OBJECT(OBJ("acid venom", "splash of venom"),
		BITS(0,1,0,0,0,0,0,1,0,0,0,P_NONE,LIQUID), 0,
		VENOM_CLASS,  500, 0,	 1,  0,  6,  6, 0, 0,	 0, HI_ORGANIC),
		/* +d6 small or large */

/* fencepost, the deadly Array Terminator -- name [1st arg] *must* be NULL */
	OBJECT(OBJ((char *)0,(char *)0), BITS(0,0,0,0,0,0,0,0,0,0,0,P_NONE,0), 0,
		ILLOBJ_CLASS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
}; /* objects[] */

#ifndef OBJECTS_PASS_2_

/* perform recursive compilation for second structure */
#  undef OBJ
#  undef OBJECT
#  define OBJECTS_PASS_2_
#include "objects.c"

void NDECL(objects_init);

/* dummy routine used to force linkage */
void
objects_init()
{
    return;
}

#endif	/* !OBJECTS_PASS_2_ */

/*objects.c*/
