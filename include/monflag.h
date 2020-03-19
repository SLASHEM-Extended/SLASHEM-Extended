/*	SCCS Id: @(#)monflag.h	3.4	1996/05/04	*/
/* Copyright (c) 1989 Mike Threepoint				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef MONFLAG_H
#define MONFLAG_H

#define MS_SILENT	0	/* makes no sound */
#define MS_BARK		1	/* if full moon, may howl */
#define MS_MEW		2	/* mews or hisses */
#define MS_ROAR		3	/* roars */
#define MS_GROWL	4	/* growls */
#define MS_SQEEK	5	/* squeaks, as a rodent */
#define MS_SQAWK	6	/* squawks, as a bird */
#define MS_HISS		7	/* hisses */
#define MS_BUZZ		8	/* buzzes (killer bee) */
#define MS_GRUNT	9	/* grunts (or speaks own language) */
#define MS_NEIGH	10	/* neighs, as an equine */
#define MS_WAIL		11	/* wails, as a tortured soul */
#define MS_GURGLE	12	/* gurgles, as liquid or through saliva */
#define MS_BURBLE	13	/* burbles (jabberwock) */
#define MS_ANIMAL	13	/* up to here are animal noises */
#define MS_SHRIEK	15	/* wakes up others */
#define MS_BONES	16	/* rattles bones (skeleton) */
#define MS_LAUGH	17	/* grins, smiles, giggles, and laughs */
#define MS_MUMBLE	18	/* says something or other */
#define MS_IMITATE	19	/* imitates others (leocrotta) */
#define MS_ORC		MS_GRUNT	/* intelligent brutes */
#define MS_HUMANOID	20	/* generic traveling companion */
#define MS_ARREST	21	/* "Stop in the name of the law!" (Kops) */
#define MS_SOLDIER	22	/* army and watchmen expressions */
#define MS_GUARD	23	/* "Please drop that gold and follow me." */
#define MS_DJINNI	24	/* "Thank you for freeing me!" */
#define MS_NURSE	25	/* "Take off your shirt, please." */
#define MS_SEDUCE	26	/* "Hello, sailor." (Nymphs) */
#define MS_VAMPIRE	27	/* vampiric seduction, Vlad's exclamations */
#define MS_BRIBE	28	/* asks for money, or berates you */
#define MS_CUSS		29	/* berates (demons) or intimidates (Wiz) */
#define MS_RIDER	30	/* astral level special monsters */
#define MS_LEADER	31	/* your class leader */
#define MS_NEMESIS	32	/* your nemesis */
#define MS_GUARDIAN	33	/* your leader's guards */
#define MS_SELL		34	/* demand payment, complain about shoplifters */
#define MS_ORACLE	35	/* do a consultation */
#define MS_PRIEST	36	/* ask for contribution; do cleansing */
#define MS_SPELL	37	/* spellcaster not matching any of the above */
#define MS_WERE		38	/* lycanthrope in human form */
#define MS_BOAST	39	/* giants */
#define MS_GYPSY	40	/* gypsies */
#define MS_SHEEP	41	/* sheep */
#define MS_CHICKEN	42	/* chicken */
#define MS_COW		43	/* cows and bulls */
#define MS_PARROT	44	/* parrots */
#define MS_WHORE	45	/* sexy prostitute talking to you */
#define MS_SUPERMAN	46	/* fearful superman */
#define MS_FART_QUIET	47	/* nastier version of shrieking */
#define MS_FART_NORMAL	48	
#define MS_FART_LOUD	49	
#define MS_VICE	50	/* seven deadly sins patch */
#define MS_BOSS	51	/* boss monster with taunts */
#define MS_SOUND 52	/* soundwave "attack" */
#define MS_SHOE 53	/* they have been silent for too long, now they talk :D */
#define MS_STENCH 54	/* evil patch idea :D causes bad effects if you are reasonably close and the monster is hostile */
#define MS_CONVERT 55	/* reduces your alignment, can permaconvert you if it gets too low! */
#define MS_HCALIEN 56	/* similar to MS_STENCH */

/* MS_DOUGLAS_ADAMS used to be 42, however I fear the DevTeam eventually 
** reaching that number.  - sk */
/* The slashem devteam did. :D --Amy */

#define MS_DOUGLAS_ADAMS 95	/* "42"; "Nice day"; etc. */
#define MS_PUPIL 	96
#define MS_TEACHER	97
#define MS_PRINCIPAL	98
#define MS_SMITH	99


#define MR_FIRE         0x01    /* resists fire */
#define MR_COLD         0x02    /* resists cold */
#define MR_SLEEP        0x04    /* resists sleep */
#define MR_DISINT       0x08    /* resists disintegration */
#define MR_ELEC         0x10    /* resists electricity */
#define MR_POISON       0x20    /* resists poison */
#define MR_ACID         0x40    /* resists acid */
#define MR_STONE        0x80    /* resists petrification */

/* other resistances: magic, sickness */
/* other conveyances: teleport, teleport control, telepathy */

/* individual resistances */
#define MR2_SEE_INVIS	0x0100	/* see invisible */
#define MR2_LEVITATE	0x0200	/* levitation */
#define MR2_WATERWALK	0x0400	/* water walking */
#define MR2_MAGBREATH	0x0800	/* magical breathing */
#define MR2_DISPLACED	0x1000	/* displaced */
#define MR2_STRENGTH	0x2000	/* gauntlets of power */
#define MR2_FUMBLING	0x4000	/* clumsy */

/* more type resistances */
#define MR_DEATH	0x00400000L	/* resists death magic */
#define MR_DRAIN	0x00800000L	/* resists level drain */

#define MR_PLUSONE	0x01000000L	/* need +1 weapon to hit */
#define MR_PLUSTWO	0x02000000L	/* need +2 weapon to hit */
#define MR_PLUSTHREE	0x04000000L	/* need +3 weapon to hit */
#define MR_PLUSFOUR	0x08000000L	/* need +4 weapon to hit */

/* Doesn't really belong under resistances, but ... */
#define MR_HITASONE	0x10000000L	/* hits ceatures as +1 weapon */
#define MR_HITASTWO	0x20000000L	/* hits ceatures as +2 weapon */
#define MR_HITASTHREE	0x40000000L	/* hits ceatures as +3 weapon */
#define MR_HITASFOUR	0x80000000L	/* hits ceatures as +4 weapon */

#define MR_TYPEMASK	0xFFC000FFL	/* bits to take from mresists */


#define M1_FLY		0x00000001L	/* can fly or float */
#define M1_SWIM		0x00000002L	/* can traverse water */
#define M1_AMORPHOUS	0x00000004L	/* can flow under doors */
#define M1_WALLWALK	0x00000008L	/* can phase thru rock */
#define M1_CLING	0x00000010L	/* can cling to ceiling */
#define M1_TUNNEL	0x00000020L	/* can tunnel thru rock */
#define M1_NEEDPICK	0x00000040L	/* needs pick to tunnel */
#define M1_CONCEAL	0x00000080L	/* hides under objects */
#define M1_HIDE		0x00000100L	/* mimics, blends in with ceiling */
#define M1_AMPHIBIOUS	0x00000200L	/* can survive underwater */
#define M1_BREATHLESS	0x00000400L	/* doesn't need to breathe */
#define M1_NOTAKE	0x00000800L	/* cannot pick up objects */
#define M1_NOEYES	0x00001000L	/* no eyes to gaze into or blind */
#define M1_NOHANDS	0x00002000L	/* no hands to handle things */
#define M1_NOLIMBS	0x00006000L	/* no arms/legs to kick/wear on */
#define M1_NOHEAD	0x00008000L	/* no head to behead */
#define M1_MINDLESS	0x00010000L	/* has no mind--golem, zombie, mold */
#define M1_HUMANOID	0x00020000L	/* has humanoid head/arms/torso */
#define M1_ANIMAL	0x00040000L	/* has animal body */
#define M1_SLITHY	0x00080000L	/* has serpent body */
#define M1_UNSOLID	0x00100000L	/* has no solid or liquid body */
#define M1_THICK_HIDE	0x00200000L	/* has thick hide or scales */
#define M1_OVIPAROUS	0x00400000L	/* can lay eggs */
#define M1_REGEN	0x00800000L	/* regenerates hit points */
#define M1_SEE_INVIS	0x01000000L	/* can see invisible creatures */
#define M1_TPORT	0x02000000L	/* can teleport */
#define M1_TPORT_CNTRL	0x04000000L	/* controls where it teleports to */
#define M1_ACID		0x08000000L	/* acidic to eat */
#define M1_POIS		0x10000000L	/* poisonous to eat */
#define M1_CARNIVORE	0x20000000L	/* eats corpses */
#define M1_HERBIVORE	0x40000000L	/* eats fruits */
#define M1_OMNIVORE	0x60000000L	/* eats both */
#ifdef NHSTDC
#define M1_METALLIVORE	0x80000000UL	/* eats metal */
#else
#define M1_METALLIVORE	0x80000000L	/* eats metal */
#endif

#define M2_NOPOLY	0x00000001L	/* players mayn't poly into one */
#define M2_UNDEAD	0x00000002L	/* is walking dead */
#define M2_MERC		0x00000004L	/* is a guard or soldier */
#define M2_HUMAN	0x00000008L	/* is a human */
#define M2_ELF		0x00000010L	/* is an elf */
#define M2_DWARF	0x00000020L	/* is a dwarf */
#define M2_GNOME	0x00000040L	/* is a gnome */
#define M2_ORC		0x00000080L	/* is an orc */
#define M2_HOBBIT	0x00000100L	/* is a hobbit */
#define M2_WERE		0x00000200L	/* is a lycanthrope */
#define M2_VAMPIRE	0x00000400L	/* is a vampire */
#define M2_LORD		0x00000800L	/* is a lord to its kind */
#define M2_PRINCE	0x00001000L	/* is an overlord to its kind */
#define M2_MINION	0x00002000L	/* is a minion of a deity */
#define M2_GIANT	0x00004000L	/* is a giant */
#define M2_DEMON	0x00008000L	/* is a demon */
#define M2_MALE		0x00010000L	/* always male */
#define M2_FEMALE	0x00020000L	/* always female */
#define M2_NEUTER	0x00040000L	/* neither male nor female */
#define M2_PNAME	0x00080000L	/* monster name is a proper name */
#define M2_HOSTILE	0x00100000L	/* always starts hostile */
#define M2_PEACEFUL	0x00200000L	/* always starts peaceful */
#define M2_DOMESTIC	0x00400000L	/* can be tamed by feeding */
#define M2_WANDER	0x00800000L	/* wanders randomly */
#define M2_STALK	0x01000000L	/* follows you to other levels */
#define M2_NASTY	0x02000000L	/* extra-nasty monster (more xp) */
#define M2_STRONG	0x04000000L	/* strong (or big) monster */
#define M2_ROCKTHROW	0x08000000L	/* throws boulders */
#define M2_GREEDY	0x10000000L	/* likes gold */
#define M2_JEWELS	0x20000000L	/* likes gems */
#define M2_COLLECT	0x40000000L	/* picks up weapons and food */
#ifdef NHSTDC
#define M2_MAGIC	0x80000000UL	/* picks up magic items */
#else
#define M2_MAGIC	0x80000000L	/* picks up magic items */
#endif

#define M3_WANTSAMUL	0x00000001L		/* would like to steal the amulet */
#define M3_WANTSBELL	0x00000002L		/* wants the bell */
#define M3_WANTSBOOK	0x00000004L		/* wants the book */
#define M3_WANTSCAND	0x00000008L		/* wants the candelabrum */
#define M3_WANTSARTI	0x00000010L		/* wants the quest artifact */
#define M3_WANTSALL	0x0000001fL		/* wants any major artifact */
#define M3_WAITFORU	0x00000040L		/* waits to see you or get attacked */
#define M3_CLOSE		0x00000080L		/* lets you close unless attacked */

#define M3_COVETOUS	0x0000001fL		/* wants something */
#define M3_WAITMASK	0x000000c0L		/* waiting... */

/* Infravision is currently implemented for players only */
#define M3_INFRAVISION	0x00000100L		/* has infravision */
#define M3_INFRAVISIBLE 0x00000200L		/* visible by infravision */

/* [DS] Lethe */
#define M3_TRAITOR	0x00000400L			/* Capable of treachery */
#define M3_NOTAME		0x00000800L			/* Cannot be tamed */

/* more stuff --Amy */
#define M3_AVOIDER	0x00001000L			/* Avoids player */
#define M3_LITHIVORE	0x00002000L			/* can eat bone, glass, gems and minerals */
#define M3_PETTY		0x00004000L			/* kelp frond and poke ball can tame them */
#define M3_POKEMON	0x00008000L			/* poke ball has an increased chance of catching one of these */

#define M3_NOPLRPOLY	0x00010000L			/* monsters may poly into it, players can't */
#define M3_NONMOVING	0x00020000L			/* does not move around, but can attack */
#define M3_EGOTYPE	0x00040000L			/* always generated with at least one egotype */
#define M3_TELEPATHIC	0x00080000L			/* floating eye, mind flayer etc. */
#define M3_SPIDER		0x00100000L			/* can walk on webs without being trapped */
#define M3_PETRIFIES	0x00200000L			/* touch petrifies */
#define M3_IS_MIND_FLAYER	0x00400000L			/* is a mind flayer */
#define M3_NO_DECAY	0x00800000L			/* corpse won't decay - lizard, lichen etc. */
#define M3_MIMIC		0x01000000L			/* non-m-class-monster that mimics stuff */
#define M3_PERMAMIMIC	0x02000000L			/* non-m-class-monster that mimics stuff and keeps doing so when hit */
#define M3_SLIME		0x04000000L			/* coming into contact with this monster causes sliming */

#define M3_FREQ_UNCOMMON2	0x08000000L			/* 2x more rare */
#define M3_FREQ_UNCOMMON3	0x10000000L			/* 3x more rare */
#define M3_FREQ_UNCOMMON5	0x20000000L			/* 5x more rare */
#define M3_FREQ_UNCOMMON7	0x40000000L			/* 7x more rare */
#define M3_FREQ_UNCOMMON10	0x80000000L			/* 10x more rare */
/* these can stack to make monsters even more rare --Amy */

#define M3_MIND_FLAYER	0x00480000L			/* generates "faint wave of psychic energy" message, telepathic */

#define M4_BAT	0x00000001L		/* monster is a bat - speeds up in Gehennom */
#define M4_REVIVE	0x00000002L		/* monster that is not a troll or fungus but revives anyway */
#define M4_RAT	0x00000004L		/* monster is a rat and can be tamed with cheese */
#define M4_SHADE	0x00000008L		/* attacks pass harmlessly through the monster unless it's a silver weapon */
#define M4_REFLECT	0x00000010L		/* monster has intrinsic reflection */
#define M4_MULTIHUED	0x00000020L		/* monster is fleecy-colored <3 */
#define M4_TAME	0x00000040L		/* spawns tame; if also M2_HOSTILE, only tame with a 1 in 3 chance */
#define M4_ORGANIVORE	0x00000080L		/* eats leather and other organic material (e.g. gelatinous cube) */
#define M4_SHAPESHIFT	0x00000100L		/* chameleons and similar monsters */
#define M4_GRIDBUG	0x00000200L		/* can only move in cardinal directions */
#define M4_HUMANWERE	0x00000400L		/* for makedefs only - append HUMAN_ in front of werewolf name */

#define M5_SPACEWARS	0x00000001L		/* Castle of the Winds monster */
#define M5_JOKE	0x00000002L		/* joke monster */
#define M5_ANGBAND	0x00000004L		/* Angband monster */
#define M5_STEAMBAND	0x00000008L		/* Steamband monster */
#define M5_ANIMEBAND	0x00000010L		/* Animeband monster */
#define M5_DIABLO	0x00000020L		/* Diablo monster */
#define M5_DLORDS	0x00000040L		/* Dungeon Lords monster */
#define M5_VANILLA	0x00000080L		/* vanilla Nethack or SLASH'EM monster (yes, SLASH'EM is officially "vanilla") */
#define M5_DNETHACK	0x00000100L		/* dnethack monster */
#define M5_RANDOMIZED	0x00000200L		/* shambling horrors and similar monsters that are initialized in u_init.c */
#define M5_JONADAB	0x00000400L		/* silly randomized monster created by jonadab's script */
#define M5_EVIL	0x00000800L		/* evil patch monster; which ones count as such is determined by Amy :D */
#define M5_ELONA	0x00001000L		/* Elona monster */
#define M5_AOE	0x00002000L		/* Age of Empires monster */
#define M5_ELDERSCROLLS	0x00004000L		/* monster from Fallout or The Elder Scrolls (yes they're the same :P) */

#define MZ_TINY		0		/* < 2' */
#define MZ_SMALL	1		/* 2-4' */
#define MZ_MEDIUM	2		/* 4-7' */
#define MZ_HUMAN	MZ_MEDIUM	/* human-sized */
#define MZ_LARGE	3		/* 7-12' */
#define MZ_HUGE		4		/* 12-25' */
#define MZ_GIGANTIC	7		/* off the scale */


/* Monster races -- must stay within ROLE_RACEMASK */
/* Eventually this may become its own field */
#define MH_HUMAN	M2_HUMAN
#define MH_ELF		M2_ELF
#define MH_DWARF	M2_DWARF
#define MH_GNOME	M2_GNOME
#define MH_ORC		M2_ORC
#define MH_HOBBIT	M2_HOBBIT
#define MH_WERE		M2_WERE
#define MH_VAMPIRE	M2_VAMPIRE


/* for mons[].geno (constant during game) */
#define G_VLGROUP	0x8000		/* appear in very large groups normally */
#define G_UNIQ		0x4000		/* generated only once */
/*#define G_NOHELL	0x2000*/		/* not generated in "hell" */
/*#define G_HELL		0x1000*/		/* generated only in "hell" */
#define G_PLATOON	0x2000		/* if group spawn, this monster is spawned with random same-class monsters */
#define G_RGROUP		0x1000		/* appears in random groups normally */
#define G_NOGEN		0x0800		/* generated only specially */
#define G_SGROUP	0x0400		/* appear in small groups normally */
#define G_LGROUP	0x0200		/* appear in large groups normally */
#define G_GENO		0x0100		/* can be genocided */
#define G_NOCORPSE	0x0080		/* no corpse left ever */
#define G_FREQ		0x007f		/* creation frequency mask */

/* for mvitals[].mvflags (variant during game), along with G_NOCORPSE */
#define G_KNOWN		0x0004		/* have been encountered */
#define G_GONE		(G_GENOD|G_EXTINCT)
#define G_GENOD		0x0002		/* have been genocided */
#define G_EXTINCT	0x0001		/* have been extinguished as
					   population control */
#define MV_KNOWS_EGG	0x0008		/* player recognizes egg of this
					   monster type */

/* Additional flag for mkclass() */
#define MKC_ULIMIT	0x10000		/* place an upper limit on difficulty
					   of generated monster */
#endif /* MONFLAG_H */
