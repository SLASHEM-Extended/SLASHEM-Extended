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
#define P_ORB			18
#define P_CLAW			19
#define P_GRINDER			20
#define P_POLEARMS			21
#define P_SPEAR				22
#define P_JAVELIN			23
#define P_TRIDENT			24
#define P_LANCE				25
#define P_BOW				26
#define P_SLING				27
#define P_FIREARM			28	/* KMH */
#define P_CROSSBOW			29
#define P_DART				30
#define P_SHURIKEN			31
#define P_BOOMERANG			32
#define P_VENOM				33
#define P_BOULDER_THROWING		34	/* can only be used by giants, or players polyd into one */
#define P_WHIP				35
#define P_UNICORN_HORN			36	
#define P_LIGHTSABER			37	/* last weapon */
#define P_FIRST_WEAPON			P_DAGGER
#define P_LAST_WEAPON			P_LIGHTSABER

/* Spell skills -- Wizard Patch by Larry Stewart-Zerba */
#define P_ATTACK_SPELL		(P_LAST_WEAPON + 1)
#define P_HEALING_SPELL		(P_LAST_WEAPON + 2)
#define P_DIVINATION_SPELL	(P_LAST_WEAPON + 3)
#define P_ENCHANTMENT_SPELL	(P_LAST_WEAPON + 4)
#define P_PROTECTION_SPELL  	(P_LAST_WEAPON + 5)
#define P_BODY_SPELL		(P_LAST_WEAPON + 6)
#define P_OCCULT_SPELL		(P_LAST_WEAPON + 7)
#define P_ELEMENTAL_SPELL	(P_LAST_WEAPON + 8)
#define P_CHAOS_SPELL		(P_LAST_WEAPON + 9)
#define P_MATTER_SPELL		(P_LAST_WEAPON + 10)
#define P_FIRST_SPELL		P_ATTACK_SPELL
#define P_LAST_SPELL		P_MATTER_SPELL

/* Other types of combat */
#define P_BARE_HANDED_COMBAT 	(P_LAST_SPELL + 1)
#define P_HIGH_HEELS 	(P_LAST_SPELL + 2) /* idea by yasdorian :-) */
#define P_GENERAL_COMBAT	(P_LAST_SPELL + 3)
#define P_SHIELD	(P_LAST_SPELL + 4)
#define P_BODY_ARMOR	(P_LAST_SPELL + 5)
#define P_TWO_HANDED_WEAPON	(P_LAST_SPELL + 6)
#define P_POLYMORPHING	(P_LAST_SPELL + 7)
#define P_DEVICES	(P_LAST_SPELL + 8)
#define P_SEARCHING	(P_LAST_SPELL + 9)
#define P_SPIRITUALITY	(P_LAST_SPELL + 10)
#define P_PETKEEPING	(P_LAST_SPELL + 11)
#define P_MISSILE_WEAPONS	(P_LAST_SPELL + 12)
#define P_TECHNIQUES	(P_LAST_SPELL + 13)
#define P_IMPLANTS	(P_LAST_SPELL + 14)
#define P_SEXY_FLATS	(P_LAST_SPELL + 15)
#define P_MEMORIZATION	(P_LAST_SPELL + 16)
#define P_GUN_CONTROL	(P_LAST_SPELL + 17)
#define P_SQUEAKING	(P_LAST_SPELL + 18)
#define P_SYMBIOSIS	(P_LAST_SPELL + 19)
#define P_WEIGHT_LIFTING	(P_LAST_SPELL + 20)

#define P_SHII_CHO	(P_LAST_SPELL + 21)
#define P_MAKASHI	(P_LAST_SPELL + 22)
#define P_SORESU	(P_LAST_SPELL + 23)
#define P_ATARU	(P_LAST_SPELL + 24)
#define P_SHIEN	(P_LAST_SPELL + 25)
#define P_DJEM_SO	(P_LAST_SPELL + 26)
#define P_NIMAN	(P_LAST_SPELL + 27)
#define P_JUYO	(P_LAST_SPELL + 28)
#define P_VAAPAD	(P_LAST_SPELL + 29)
#define P_WEDI	(P_LAST_SPELL + 30)

#define P_STILETTO_HEELS	(P_LAST_SPELL + 31)
#define P_CONE_HEELS	(P_LAST_SPELL + 32)
#define P_BLOCK_HEELS	(P_LAST_SPELL + 33)
#define P_WEDGE_HEELS	(P_LAST_SPELL + 34)

#define P_MARTIAL_ARTS		(P_LAST_SPELL + 35) /* WAC used to be role distinguishes */
#define P_TWO_WEAPON_COMBAT	(P_LAST_SPELL + 36) /* Finally implemented */
#define P_RIDING		(P_LAST_SPELL + 37) /* How well you control your steed */
#define P_LAST_H_TO_H		P_RIDING
#define P_FIRST_H_TO_H		P_BARE_HANDED_COMBAT

#define P_NUM_SKILLS		(P_LAST_H_TO_H+1)

/* odd skill names */
#define PN_POLEARMS		(-1)
#define PN_SABER		(-2)
#define PN_HAMMER		(-3)
#define PN_WHIP			(-4)
#define PN_PADDLE		(-5)
#define PN_FIREARMS		(-6)
#define PN_ATTACK_SPELL		(-7)
#define PN_HEALING_SPELL	(-8)
#define PN_DIVINATION_SPELL	(-9)
#define PN_ENCHANTMENT_SPELL	(-10)
#define PN_PROTECTION_SPELL	(-11)
#define PN_BODY_SPELL		(-12)
#define PN_OCCULT_SPELL		(-13)
#define PN_ELEMENTAL_SPELL		(-14)
#define PN_CHAOS_SPELL		(-15)
#define PN_MATTER_SPELL		(-16)
#define PN_BARE_HANDED		(-17)
#define PN_HIGH_HEELS		(-18)
#define PN_GENERAL_COMBAT		(-19)
#define PN_SHIELD		(-20)
#define PN_BODY_ARMOR		(-21)
#define PN_TWO_HANDED_WEAPON		(-22)
#define PN_POLYMORPHING		(-23)
#define PN_DEVICES		(-24)
#define PN_SEARCHING		(-25)
#define PN_SPIRITUALITY		(-26)
#define PN_PETKEEPING		(-27)
#define PN_MISSILE_WEAPONS		(-28)
#define PN_TECHNIQUES		(-29)
#define PN_IMPLANTS		(-30)
#define PN_SEXY_FLATS		(-31)
#define PN_MEMORIZATION		(-32)
#define PN_GUN_CONTROL		(-33)
#define PN_SQUEAKING		(-34)
#define PN_SYMBIOSIS		(-35)
#define PN_WEIGHT_LIFTING		(-36)
#define PN_SHII_CHO		(-37)
#define PN_MAKASHI		(-38)
#define PN_SORESU		(-39)
#define PN_ATARU		(-40)
#define PN_SHIEN		(-41)
#define PN_DJEM_SO		(-42)
#define PN_NIMAN		(-43)
#define PN_JUYO		(-44)
#define PN_VAAPAD		(-45)
#define PN_WEDI		(-46)
#define PN_STILETTO_HEELS		(-47)
#define PN_CONE_HEELS		(-48)
#define PN_BLOCK_HEELS		(-49)
#define PN_WEDGE_HEELS		(-50)
#define PN_MARTIAL_ARTS		(-51)
#define PN_RIDING		(-52)
#define PN_TWO_WEAPONS		(-53)
#define PN_LIGHTSABER		(-54)
#define PN_ORB		(-55)
#define PN_CLAW		(-56)
#define PN_GRINDER		(-57)

/*
 * These are the standard weapon skill levels.  It is important that
 * the lowest "valid" skill be be 1.  The code calculates the
 * previous amount to practice by calling  practice_needed_to_advance()
 * with the current skill-1.  To work out for the UNSKILLED case,
 * a value of 0 needed.
 */
#define P_ISRESTRICTED		-1
#define P_UNSKILLED		1
#define P_BASIC			2 /* actually skill level 1 */
#define P_SKILLED		3	/* skilled = skill level 2 */
#define P_EXPERT		4	/* expert = skill level 3 */
#define P_MASTER		5	/* master = skill level 4 */
#define P_GRAND_MASTER		6 /* grand = skill level 5 */
#define P_SUPREME_MASTER		7 /* sup = skill level 6 */
#define P_LIMIT P_SUPREME_MASTER

#define practice_needed_to_advance(level,skill) \
   (P_SKILL(skill) == P_ISRESTRICTED ? 500 : \
    P_SKILL(skill) >= P_MAX_SKILL(skill) ? (level)*(level)*(level)*200 : \
    (level)*(level)*(level)*20)

/* for AD_TRAI attack: calculate it without the 10x cap */
#define practice_needed_to_advance_nonmax(level,skill) \
   (P_SKILL(skill) == P_ISRESTRICTED ? 500 : (level)*(level)*(level)*20)

/* The hero's skill in various weapons. */
struct skills {
	xchar skill;
	xchar max_skill;
	unsigned short advance;
	int matrixskill;
};

#define P_SKILL(type)		(u.weapon_skills[type].skill)
#define P_MAX_SKILL(type)	(u.weapon_skills[type].max_skill)
#define P_ADVANCE(type)		(u.weapon_skills[type].advance)
#define P_MATRIX(type)		(u.weapon_skills[type].matrixskill)
#define P_RESTRICTED(type)	(u.weapon_skills[type].skill == P_ISRESTRICTED)

#define P_SKILL_LIMIT 2000	/* max number of skill advancements */

/* initial skill matrix structure; used in u_init.c and weapon.c */
struct def_skill {
	xchar skill;
	xchar skmax;
	xchar skvan; /* 1 = vanillaoid capped at basic, 2 = skilled, 0 = unchanged */
};

/* these roles qualify for a martial arts bonus */
/* WAC - made this dependant on weapon skills if defined */

# define martial_bonus() ((P_SKILL(P_MARTIAL_ARTS) >= P_UNSKILLED) && !u.disablemartial && !PlayerCannotUseSkills) /* because sometimes they cap at expert --Amy */
# define martial_bonus_specialmode() ((P_SKILL(P_MARTIAL_ARTS) >= P_UNSKILLED)&& !PlayerCannotUseSkills) /* for kicking, i.e. ignores u.disablemartial --Amy */
/*
#else
# define martial_bonus() (Role_if(PM_SAMURAI) || Role_if(PM_PRIEST) || Role_if(PM_MONK))
*/

#endif  /* SKILLS_H */
