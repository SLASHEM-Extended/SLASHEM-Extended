/*	SCCS Id: @(#)skills.h	3.4	1999/10/27	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985-1999. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef SKILLS_H
#define SKILLS_H

/* KMH, balance patch -- Much of this code was taken from you.h */

/* Code to denote that no skill is being used */
#define P_NONE				0

/* Weapon Skills -- Stephen White
 * Order matters and are used in macros.
 * Positive values denote a wielded weapon or launcher.
 * Negative values denote ammunition or missiles.
 * Update weapon.c if you ammend any skills.
 * Also used for oc_subtyp.
 */
#define P_DAGGER			1
#define P_KNIFE				2
#define P_AXE				3
#define P_PICK_AXE			4
#define P_SHORT_SWORD			5
#define P_BROAD_SWORD			6
#define P_LONG_SWORD			7
#define P_TWO_HANDED_SWORD		8
#define P_SCIMITAR			9
#define P_SABER				10
#define P_CLUB				11	/* Bludgeon with a heavy shaft */
#define P_PADDLE			12	/* Bludgeon with a flattened shaft */
#define P_MACE				13	/* Bludgeon with a spiked shaft? */
#define P_MORNING_STAR			14	/* Spiked ball chained to stick */
#define P_FLAIL				15	/* Two pieces hinged or chained together */
#define P_HAMMER			16	/* Bludgeon with a heavy head on the end */
#define P_QUARTERSTAFF			17	/* Bludgeon with a long shaft */
#define P_POLEARMS			18
#define P_SPEAR				19
#define P_JAVELIN			20
#define P_TRIDENT			21
#define P_LANCE				22
#define P_BOW				23
#define P_SLING				24
#ifdef FIREARMS
#define P_FIREARM			25	/* KMH */
#endif
#define P_CROSSBOW			26
#define P_DART				27
#define P_SHURIKEN			28
#define P_BOOMERANG			29
#define P_WHIP				30
#define P_UNICORN_HORN			31	
#ifdef LIGHTSABERS
#define P_LIGHTSABER			32	/* last weapon */
#endif
#define P_FIRST_WEAPON			P_DAGGER
#ifdef LIGHTSABERS
#define P_LAST_WEAPON			P_LIGHTSABER
#else
#define P_LAST_WEAPON			P_UNICORN_HORN
#endif

/* Spell skills -- Wizard Patch by Larry Stewart-Zerba */
#define P_ATTACK_SPELL		(P_LAST_WEAPON + 1)
#define P_HEALING_SPELL		(P_LAST_WEAPON + 2)
#define P_DIVINATION_SPELL	(P_LAST_WEAPON + 3)
#define P_ENCHANTMENT_SPELL	(P_LAST_WEAPON + 4)
#define P_PROTECTION_SPELL  	(P_LAST_WEAPON + 5)
#define P_BODY_SPELL		(P_LAST_WEAPON + 6)
#define P_MATTER_SPELL		(P_LAST_WEAPON + 7)
#define P_FIRST_SPELL		P_ATTACK_SPELL
#define P_LAST_SPELL		P_MATTER_SPELL

/* Other types of combat */
#define P_BARE_HANDED_COMBAT 	(P_LAST_SPELL + 1)
#define P_MARTIAL_ARTS		(P_LAST_SPELL + 2) /* WAC used to be role distinguishes */
#define P_TWO_WEAPON_COMBAT	(P_LAST_SPELL + 3) /* Finally implemented */
#ifdef STEED
#define P_RIDING		(P_LAST_SPELL + 4) /* How well you control your steed */
#define P_LAST_H_TO_H		P_RIDING
#else
#define P_LAST_H_TO_H		P_TWO_WEAPON_COMBAT
#endif
#define P_FIRST_H_TO_H		P_BARE_HANDED_COMBAT

#define P_NUM_SKILLS		(P_LAST_H_TO_H+1)


/*
 * These are the standard weapon skill levels.  It is important that
 * the lowest "valid" skill be be 1.  The code calculates the
 * previous amount to practice by calling  practice_needed_to_advance()
 * with the current skill-1.  To work out for the UNSKILLED case,
 * a value of 0 needed.
 */
#define P_ISRESTRICTED		-1
#define P_UNSKILLED		1
#define P_BASIC			2
#define P_SKILLED		3
#define P_EXPERT		4
#define P_MASTER		5
#define P_GRAND_MASTER		6
#define P_LIMIT P_GRAND_MASTER

#define practice_needed_to_advance(level,skill) \
   (P_SKILL(skill) == P_ISRESTRICTED ? 500 : \
    P_SKILL(skill) >= P_MAX_SKILL(skill) ? (level)*(level)*200 : \
    (level)*(level)*20)

/* The hero's skill in various weapons. */
struct skills {
	xchar skill;
	xchar max_skill;
	unsigned short advance;
};

#define P_SKILL(type)		(u.weapon_skills[type].skill)
#define P_MAX_SKILL(type)	(u.weapon_skills[type].max_skill)
#define P_ADVANCE(type)		(u.weapon_skills[type].advance)
#define P_RESTRICTED(type)	(u.weapon_skills[type].skill == P_ISRESTRICTED)

#define P_SKILL_LIMIT 60	/* max number of skill advancements */

/* initial skill matrix structure; used in u_init.c and weapon.c */
struct def_skill {
	xchar skill;
	xchar skmax;
};

/* these roles qualify for a martial arts bonus */
/* WAC - made this dependant on weapon skills if defined */

# define martial_bonus() (P_SKILL(P_MARTIAL_ARTS) >= P_BASIC)
/*
#else
# define martial_bonus() (Role_if(PM_SAMURAI) || Role_if(PM_PRIEST) || Role_if(PM_MONK))
*/

#endif  /* SKILLS_H */
