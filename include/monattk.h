/*	SCCS Id: @(#)monattk.h	3.4	2002/03/24	*/
/* NetHack may be freely redistributed.  See license for details. */
/* Copyright 1988, M. Stephenson */

#ifndef MONATTK_H
#define MONATTK_H

/*	Add new attack types below - ordering affects experience (exper.c).
 *	Attacks > AT_BUTT are worth extra experience.
 */
#define AT_ANY		(-1)	/* fake attack; dmgtype_fromattack wildcard */
#define AT_NONE		0	/* passive monster (ex. acid blob) */
#define AT_CLAW		1	/* claw (punch, hit, etc.) */
#define AT_BITE		2	/* bite */
#define AT_KICK		3	/* kick */
#define AT_BUTT		4	/* head butt (ex. a unicorn) */
#define AT_TUCH		5	/* touches */
#define AT_STNG		6	/* sting */
#define AT_HUGS		7	/* crushing bearhug */
#define AT_SCRA		8	/* similar to claw */
#define AT_LASH		9	/* whirl around and hit */
#define AT_SPIT		10	/* spits substance - ranged */
#define AT_ENGL		11	/* engulf (swallow or by a cloud) */
#define AT_BREA		12	/* breath - ranged */
#define AT_EXPL		13	/* explodes - proximity */
#define AT_BOOM		14	/* explodes when killed */
#define AT_GAZE		15	/* gaze - ranged */
#define AT_TENT		16	/* tentacles */
#define AT_TRAM		17	/* trample */
#define AT_BEAM		18	/* non-contact ranged beam attack from dnethack - thanks Chris for the implementation */

/* AT_MULTIPLY is also used in u_init.c as a placeholder - please put all "standard" attack types above it! --Amy */
#define AT_MULTIPLY	19	/* RJ - multiplies (yes, it's an attack) */

#define AT_WEAP		254	/* uses weapon */
#define AT_MAGC		255	/* uses magic spell(s) */

/*	Add new damage types below.
 *
 *	Note that 1-10 correspond to the types of attack used in buzz().
 *	Please don't disturb the order unless you rewrite the buzz() code.
 */
#define AD_ANY		(-1)	/* fake damage; attacktype_fordmg wildcard */
#define AD_PHYS		0	/* ordinary physical */
#define AD_MAGM		1	/* magic missiles */
#define AD_FIRE		2	/* fire damage */
#define AD_COLD		3	/* frost damage */
#define AD_SLEE		4	/* sleep ray */
#define AD_DISN		5	/* disintegration (death ray) */
#define AD_ELEC		6	/* shock damage */
#define AD_DRST		7	/* drains str (poison) */
#define AD_ACID		8	/* acid damage */
#define AD_LITE		9		/* KMH -- light ray */
#define AD_SPC2		10	/* for extension of buzz() */
#define AD_BLND		11	/* blinds (yellow light) */
#define AD_STUN		12	/* stuns */
#define AD_SLOW		13	/* slows */
#define AD_PLYS		14	/* paralyses */
#define AD_DRLI		15	/* drains life levels (Vampire) */
#define AD_DREN		16	/* drains magic energy */
#define AD_LEGS		17	/* damages legs (xan) */
#define AD_STON		18	/* petrifies (Medusa, cockatrice) */
#define AD_STCK		19	/* sticks to you (mimic) */
#define AD_SGLD		20	/* steals gold (leppie) */
#define AD_SITM		21	/* steals item (nymphs) */
#define AD_SEDU		22	/* seduces & steals multiple items */
#define AD_TLPT		23	/* teleports you (Quantum Mech.) */
#define AD_RUST		24	/* rusts armour (Rust Monster)*/
#define AD_CONF		25	/* confuses (Umber Hulk) */
#define AD_DGST		26	/* digests opponent (trapper, etc.) */
#define AD_HEAL		27	/* heals opponent's wounds (nurse) */
#define AD_WRAP		28	/* special "stick" for eels */
#define AD_WERE		29	/* confers lycanthropy */
#define AD_DRDX		30	/* drains dexterity (quasit) */
#define AD_DRCO		31	/* drains constitution */
#define AD_DRIN		32	/* drains intelligence (mind flayer) */
#define AD_DISE		33	/* confers diseases */
#define AD_DCAY		34	/* decays organics (brown pudding) */
#define AD_SSEX		35	/* Succubus seduction (extended) */
				/* If no SEDUCE then same as AD_SEDU */
#define AD_HALU		36	/* causes hallucination */
#define AD_DETH		37	/* for Death only */
#define AD_PEST		38	/* for Pestilence only */
#define AD_FAMN		39	/* for Famine only */
#define AD_SLIM		40	/* turns you into green slime */
#define AD_CALM		41	/* KMH -- calms its enemies (koala) */
#define AD_ENCH		42	/* KMH -- remove enchantment */
#define AD_POLY		43	/* RJ -- polymorphs (genetic engineer) */
#define AD_CORR		44	/* corrode armor (black pudding) */
#define AD_TCKL		45	/* Tickle (Nightgaunts) */
#define AD_NGRA		46	/* supposed to remove an existing engraving underneath the player (Elbereth) */
#define AD_GLIB		47	/* disarm the player by damaging their hands */
#define AD_DARK		48	/* acts similar to cursed scroll of light, making an area unlit */
#define AD_WTHR		49	/* withers items */
#define AD_LUCK		50	/* reduces luck */
#define AD_NUMB		51	/* causes numbness */
#define AD_FRZE		52	/* freezes you solid */
#define AD_DISP		53	/* moves you to a random nearby empty location */
#define AD_BURN		54	/* burns you */
#define AD_FEAR		55	/* causes fear */

/* evil patch ideas by jonadab */
#define AD_NPRO		56	/* negative protection - temporarily make player's AC worse */
#define AD_POIS		57	/* poison attack that damages a random stat */
#define AD_THIR		58	/* thirsty attack - monster heals HP by the amount of damage it deals to you */
#define AD_LAVA		59	/* deals fire damage and burns items */
#define AD_FAKE		60	/* sends random messages */
#define AD_LETH		61	/* lethe - amnesia, may randomly turn magic items into mundane ones */
#define AD_CNCL		62	/* cancellation, by ais523 */
#define AD_BANI		63	/* banishment (causes segfaults) */

/* the following ones were stolen from dnethack: */
#define AD_WISD		64	/* wisdom damage */
#define AD_SHRD		65	/* withers non-enchanted armor, disenchants enchanted ones, MC doesn't prevent this! */
#define AD_WET		66	/* random items carried by the player become wet (similar to AD_LETH but weaker) */
#define AD_SUCK		67	/* suck off equipment and do damage */
#define AD_MALK		68	/* sticks to you, shock damage with high chance of busting wands or rings */
#define AD_UVUU		69	/* Uvuudaum head spike attack - several really nasty effects */
#define AD_ABDC		70	/* Abduction attack, teleports you regardless of magic cancellation */
#define AD_AXUS		71	/* Multi-element counterattack - deals fire, cold and shock damage, plus level drain */
#define AD_CHKH		72	/* damage is increased by 1 every time; slowly times out after a while */
#define AD_HODS		73	/* mirror attack - extra damage done to you depending on your weapon */
#define AD_CHRN		74	/* cursed unicorn horn */
#define AD_WEEP		75	/* level teleport, or level drain if you can't levelport */
#define AD_VAMP		76	/* Vampire's blood drain attack - ignores MC */
#define AD_WEBS		77	/* Spreads webbing on a hit */
#define AD_STTP		78	/* Steal by Teleportation: Teleports your gear away */

/* the following ones were stolen from FHS: */
#define AD_DEPR		79	/* depression - many weird effects */
#define AD_WRAT		80	/* (Wrath) drains half your current energy, or all of it if less than 10 */
#define AD_LAZY		81	/* (Sloth) laziness attack, does all sorts of nasty things */
#define AD_DRCH		82	/* charisma damage, supposed to be specific to Pride */
#define AD_DFOO		83	/* (Pride) drains a random stat, or energy, or hit points, or life level */

/* the following ones are from Angband: */
#define AD_NEXU		84	/* nexus - damage and various types of teleportation */
#define AD_SOUN		85	/* sound - stun, shatters potions */
#define AD_GRAV		86	/* gravity - displace player, disables speed for a while, stun */
#define AD_INER		87	/* inertia - disables intrinsic speed, temporarily disables extrinsic speed, extra slowness */
#define AD_TIME		88	/* time - drain life or stats, ignores resistances */
#define AD_MANA		89	/* mana - like AD_DREN but also deals hitpoint damage */
#define AD_PLAS		90	/* plasma - stun, destroys flammable items, more damage if not fire resistant */

#define AD_ENDS		91	/* placeholder */

#define AD_CLRC		240	/* random clerical spell */
#define AD_SPEL		241	/* random magic spell */
#define AD_RBRE		242	/* random breath weapon */

#define AD_SAMU		252	/* hits, may steal Amulet (Wizard) */
#define AD_CURS		253	/* random curse (ex. gremlin) */


/*
 *  Monster to monster attacks.  When a monster attacks another (mattackm),
 *  any or all of the following can be returned.  See mattackm() for more
 *  details.
 */
#define MM_MISS		0x0	/* aggressor missed */
#define MM_HIT		0x1	/* aggressor hit defender */
#define MM_DEF_DIED	0x2	/* defender died */
#define MM_AGR_DIED	0x4	/* aggressor died */

#endif /* MONATTK_H */
