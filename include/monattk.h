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
#define AT_RATH		19	/* ranged thorns */

/* AT_MULTIPLY is also used in u_init.c as a placeholder - please put all "standard" attack types above it! --Amy */
#define AT_MULTIPLY	20	/* RJ - multiplies (yes, it's an attack) */

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

/* evil patch idea by jonadab: */
#define AD_SKIL		91	/* 1 in 1000 chance to reduce the cap on one of your skills */

/* some other more or less evil ideas: */
#define AD_VENO		92	/* strong poison: tries to damage all six of the player's stats */
#define AD_DREA		93	/* dream eater: 4x damage if player sleeps or is otherwise unable to move */
#define AD_NAST		94	/* nasty trap effect */
#define AD_BADE		95	/* random bad effect */
#define AD_SLUD		96	/* corrosive sludge: tries to corrode the player's entire inventory */
#define AD_ICUR		97	/* replicates the "curse items" monster spell */
#define AD_VULN		98	/* temporarily deactivate the player's positive intrinsics */
#define AD_FUMB		99	/* fumbling-inducing attack */

#define AD_DIMN		100	/* dimness */
#define AD_AMNE		101	/* map amnesia (inspired by deepy), re-obscures random amount of tiles on current level */

#define AD_ICEB		102	/* ice block - cold damage, can break potions and inflict freeze status */
#define AD_VAPO		103	/* vaporization - disintegrates target, and if player doesn't resist, 3x damage */
#define AD_EDGE		104	/* stone edge - petrification, and reduces maximum health of player as well */
#define AD_VOMT		105	/* vomiting, or if player is already nauseated, make them deathly sick */
#define AD_LITT		106	/* litter - sliming, but faster than AD_SLIM, can corrode player's inventory */
#define AD_FREN		107	/* frenzy - can frenzy monsters, or make player go berserk; bad effect if already berserk */
#define AD_NGEN		108	/* negative enchantment - like disenchanter, but can go to -20 and curse the item */
#define AD_CHAO		109	/* chaos - combines the effects of AD_POLY and AD_CHRN */
#define AD_INSA		110	/* insanity - afflicts target with fear, confusion and stun at the same time */
#define AD_TRAP		111	/* trapping - creates a trap underneath the target, or on a random square if one exists */
#define AD_WGHT		112	/* weight - temporarily increases player's carry weight to encumber you */
#define AD_NTHR		113	/* nether - drains a bit of experience from the target and heals attacker */

#define AD_ALIN		114	/* drain alignment record */
#define AD_SIN		115	/* increase sin counter */
#define AD_CONT		116	/* contamination (similar to weeping angels in dnethack) */
#define AD_AGGR		117	/* aggravate monster (gives the intrinsic temporarily, may spawn monsters) */
#define AD_DEST		118	/* destroy weapon/armor - subject to saving throws */
#define AD_TREM		119	/* trembling - temporarily decreases your to-hit and spellcasting chances */

#define AD_IDAM		120	/* can damage items in the player's inventory */
#define AD_ANTI		121	/* antimatter - like AD_IDAM but the item cannot be resistant */
#define AD_PAIN		122	/* adds 10% of your current health to the damage done */
#define AD_TECH		123	/* increases timeout of your techniques */
#define AD_MEMO		124	/* reduces memory of your spells */
#define AD_TRAI		125	/* reduces skill training */
#define AD_STAT		126	/* permanently damages stats */

#define AD_NACU		127	/* curse a random worn armor piece and replace its egotype with a nastytrap one */
#define AD_SANI		128	/* increase sanity */

#define AD_RBAD		129	/* really bad effect - like regular bad effect, but all effects have the same chance */
#define AD_BLEE		130	/* bleedout - target continuously loses health for a while */
#define AD_SHAN		131	/* shanking - unequips player's items */
#define AD_SCOR		132	/* drains player's score */
#define AD_TERR		133	/* terrain terror - changes floor into other terrain and may sometimes summon monsters */
#define AD_FEMI		134	/* feminism trap effect */
#define AD_LEVI		135	/* levitation (always uncontrolled) */

#define AD_MCRE		136	/* temporarily reduce magic cancellation (by K2) */

#define AD_FLAM		137	/* flame attack - burns items (weaker form of AD_LAVA) */
#define AD_DEBU		138	/* debuff - temporarily reduces the player's stats */

#define AD_UNPR		139	/* unproofing - remove erosionproofing from items */
#define AD_NIVE		140	/* nivellation - can reduce max HP or Pw, but only if they're high enough */

#define AD_TDRA		141	/* tech drain - can drain technique levels */
#define AD_BLAS		142	/* blasphemy - chance to increase divine anger by one */

#define AD_ENDS		143	/* placeholder */

#define AD_ILLU		229	/* illusion attack - random effect also seen in illusion rooms */
#define AD_PART		230	/* specific regular attack (rolled at game start) */
#define AD_RUNS		231	/* specific nastytrap effect (rolled at game start) */

#define AD_DAMA		232	/* random effect that damages your gear or otherwise screws you up */
#define AD_THIE		233	/* random effect that steals your gear or otherwise screws you up */

#define AD_RAGN		234	/* ragnarok - creates lava tiles and lots of hostile giants, dragons, demons etc. */

#define AD_DATA		235	/* data delete - probably one of the most evil attacks in the game */

#define AD_MINA		236	/* monster identity nastiness - pick nasty trap effect based on the monster's m_id */

#define AD_CAST		237	/* spellcasting, can select either AD_CLRC or AD_SPEL monster spell at random */
#define AD_RNG		238	/* RNG intervention - like AD_RBRE, but all damage types have the same chance */
#define AD_MIDI		239	/* identity-specific attack - pick an attack type based on the monster's m_id */

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
