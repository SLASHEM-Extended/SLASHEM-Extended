/*	SCCS Id: @(#)artifact.h 3.4	1995/05/31	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef ARTIFACT_H
#define ARTIFACT_H

#define SPFX_NONE   0x00000000L	/* no special effects, just a bonus */
#define SPFX_NOGEN  0x00000001L	/* item is special, bequeathed by gods */
#define SPFX_RESTR  0x00000002L	/* item is restricted - can't be named */
#define SPFX_INTEL  0x00000004L	/* item is self-willed - intelligent */
#define SPFX_SPEAK  0x00000008L	/* item can speak (not implemented) */
#define SPFX_SEEK   0x00000010L	/* item helps you search for things */
#define SPFX_WARN   0x00000020L	/* item warns you of danger */
#define SPFX_ATTK   0x00000040L	/* item has a special attack (attk) */
#define SPFX_DEFN   0x00000080L	/* item has a special defence (defn) */
#define SPFX_DRLI   0x00000100L	/* drains a level from monsters */
#define SPFX_SEARCH 0x00000200L	/* helps searching */
#define SPFX_BEHEAD 0x00000400L	/* beheads monsters */
#define SPFX_HALRES 0x00000800L	/* blocks hallucinations */
#define SPFX_ESP    0x00001000L	/* ESP (like amulet of ESP) */
#define SPFX_STLTH  0x00002000L	/* Stealth */
#define SPFX_REGEN  0x00004000L	/* Regeneration */
#define SPFX_EREGEN 0x00008000L	/* Energy Regeneration */
#define SPFX_HSPDAM 0x00010000L	/* 1/2 spell damage (on player) in combat */
#define SPFX_HPHDAM 0x00020000L	/* 1/2 physical damage (on player) in combat */
#define SPFX_TCTRL  0x00040000L	/* Teleportation Control */
#define SPFX_LUCK   0x00080000L	/* Increase Luck (like Luckstone) */
#define SPFX_DMONS  0x00100000L	/* attack bonus on one monster type */
#define SPFX_DCLAS  0x00200000L	/* attack bonus on monsters w/ symbol mtype */
#define SPFX_DFLAG1 0x00400000L	/* attack bonus on monsters w/ mflags1 flag */
#define SPFX_DFLAG2 0x00800000L	/* attack bonus on monsters w/ mflags2 flag */
#define SPFX_DALIGN 0x01000000L	/* attack bonus on non-aligned monsters  */
#define SPFX_XRAY   0x02000000L	/* gives X-RAY vision to player */
#define SPFX_REFLECT 0x04000000L /* Reflection */
#define SPFX_PROTEC 0x08000000L	/* protects you if worn (+2 to AC) */
#define SPFX_EVIL   0x10000000L	/* considered to be an artifact with disadvantages --Amy */
#define SPFX_ONLYONE 0x20000000L	/* artifact never spawns again if it was already generated --Amy */
#define SPFX_NOWISH 0x40000000L	/* not wishable --Amy */

#define SPFX_DBONUS 0x01F00000L	/* attack bonus mask */

struct artifact {
	/*short*/int	    otyp;
	const char  *name;
	unsigned long spfx;	/* special effect from wielding/wearing */
	unsigned long cspfx;	/* special effect just from carrying obj */
	unsigned long mtype;	/* monster type, symbol, or flag */
	struct attack attk, defn, cary;
	/*uchar*/int	    inv_prop;	/* property obtained by invoking artifact */
	aligntyp    alignment;	/* alignment of bequeathing gods */
	int	    role;	/* character role associated with */
	int	    race;	/* character race associated with */
	long        cost;	/* price when sold to hero (default 100 x base cost) */
	
	Bitfield(questarti,1);	/* is quest artifact */
};

/* invoked properties with special powers */
#define TAMING		(LAST_PROP+1)
#define HEALING		(LAST_PROP+2)
#define ENERGY_BOOST	(LAST_PROP+3)
#define UNTRAP		(LAST_PROP+4)
#define CHARGE_OBJ	(LAST_PROP+5)
#define LEV_TELE	(LAST_PROP+6)
#define CREATE_PORTAL	(LAST_PROP+7)
#define ENLIGHTENING	(LAST_PROP+8)
#define CREATE_AMMO	(LAST_PROP+9)
#define SUMMON_UNDEAD   (LAST_PROP+10)
#define DEATH_GAZE      (LAST_PROP+11)
#define LIGHT_AREA      (LAST_PROP+12) /* STEPHEN WHITE'S NEW CODE */
#define PROT_POLY       (LAST_PROP+13)
#define SUMMON_FIRE_ELEMENTAL           (LAST_PROP+14)
#define SUMMON_WATER_ELEMENTAL          (LAST_PROP+15)
#define OBJ_DETECTION	(LAST_PROP+16)
#define PHASING		(LAST_PROP+17) /* convict role */
#define IDENTIFY	(LAST_PROP+18)
#define DRAGON_BREATH	(LAST_PROP+19)
#define OBJECT_DET	(LAST_PROP+20)
#define BOSS_NUKE		(LAST_PROP+21)
#define CHOCOLATE_CREATION		(LAST_PROP+22)
#define SPECIAL_INVOKE	(LAST_PROP+23)

#endif /* ARTIFACT_H */
