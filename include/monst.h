/*	SCCS Id: @(#)monst.h	3.4	1999/01/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef MONST_H
#define MONST_H

/* The weapon_check flag is used two ways:
 * 1) When calling mon_wield_item, is 2-6 depending on what is desired.
 * 2) Between calls to mon_wield_item, is 0 or 1 depending on whether or not
 *    the weapon is known by the monster to be cursed (so it shouldn't bother
 *    trying for another weapon).
 * I originally planned to also use 0 if the monster already had its best
 * weapon, to avoid the overhead of a call to mon_wield_item, but it turns out
 * that there are enough situations which might make a monster change its
 * weapon that this is impractical.  --KAA
 */
# define NO_WEAPON_WANTED 0
# define NEED_WEAPON 1
# define NEED_RANGED_WEAPON 2
# define NEED_HTH_WEAPON 3
# define NEED_PICK_AXE 4
# define NEED_AXE 5
# define NEED_PICK_OR_AXE 6

/* The following flags are used for the second argument to display_minventory
 * in invent.c:
 *
 * MINV_NOLET  If set, don't display inventory letters on monster's inventory.
 * MINV_ALL    If set, display all items in monster's inventory, otherwise
 *	       just display wielded weapons and worn items.
 */
#define MINV_NOLET 0x01
#define MINV_ALL   0x02

#ifndef ALIGN_H
#include "align.h"
#endif

struct monst {
	struct monst *nmon;
	struct permonst *data;
	unsigned m_id;
	/*short*/long mnum;		/* permanent monster index number */
	short movement;		/* movement points (derived from permonst definition and added effects */
	uchar m_lev;		/* adjusted difficulty level of monster */
	aligntyp malign;	/* alignment of this monster, relative to the
				   player (positive = good to kill) */
	xchar mx, my;
	xchar mux, muy;		/* where the monster thinks you are */
#define MTSZ	4
	coord mtrack[MTSZ];	/* monster track */
	int mhp, mhpmax;
	int m_en, m_enmax;	/* Power level (for spells, etc.) */
	unsigned mappearance;	/* for undetected mimics and the wiz */
	uchar	 m_ap_type;	/* what mappearance is describing: */
#define M_AP_NOTHING	0	/* mappearance is unused -- monster appears
				   as itself */
#define M_AP_FURNITURE	1	/* stairs, a door, an altar, etc. */
#define M_AP_OBJECT	2	/* an object */
#define M_AP_MONSTER	3	/* a monster */

	schar mtame;		/* level of tameness, implies peaceful */
	unsigned long mintrinsics;	/* initialized from mresists */
	int mspec_used;		/* monster's special ability attack timeout */
	int     oldmonnm;       /* Old monster number - for polymorph */

	Bitfield(female,1);	/* is female */
	Bitfield(minvis,1);	/* currently invisible */
	Bitfield(minvisreal,1);	/* really invisible - doesn't become visible even if you have see invis --Amy */
	Bitfield(noegodesc,1);	/* no egotype description --Amy */
	Bitfield(noegodisplay,1);	/* no egotype display --Amy */
	Bitfield(invis_blkd,1); /* invisibility blocked */
	Bitfield(perminvis,1);	/* intrinsic minvis value */
	int cham;	/* shape-changer; begone bitfield, we want to have no silly limitations --Amy */
/* note: lychanthropes are handled elsewhere */
#define CHAM_ORDINARY		0	/* not a shapechanger */
#define CHAM_CHAMELEON		1	/* animal */
#define CHAM_DOPPELGANGER	2	/* demi-human */
#define CHAM_DOPPLEZON	3	/* valkyrie */
#define CHAM_SANDESTIN		4	/* demon */
#define CHAM_MISSINGNO		5	/* pokemon */
#define CHAM_TRANSFORMER		6	/* humanlike */
#define CHAM_WARPER		7	/* humanlike */
#define CHAM_CHAOS_SHAPECHANGER		8	/* humanlike */
#define CHAM_SANDWICH		9	/* blob-like things */
#define CHAM_KARMA_CHAMELEON		10	/* animal */
#define CHAM_JUNOW_TRICE		11	/* cockatrice */
#define CHAM_POLY_FLAYER		12	/* mind flayer */
#define CHAM_WILD_CHANGE_NYMPH	13	/* nymph */
#define CHAM_VERY_POLY_NYMPH		14	/* nymph */
#define CHAM_CORTEGEX		15	/* vortex */
#define CHAM_CHANGE_EXPLODER		16	/* light */
#define CHAM_BAM_CHAM		17	/* dragon */
#define CHAM_LAURA_S_PARLOR_TRICK	18	/* elemental */
#define CHAM_LAURA_S_MASTERPIECE	19	/* elemental */
#define CHAM_TSCHANG_SEPHIRAH		20	/* kop */
#define CHAM_GLONK_SEPHIRAH		21	/* kop */
#define CHAM_KUSCHOING_SEPHIRAH	22	/* kop */
#define CHAM_ULTRA_DESTRUCTIVE_MONSTER	23	/* rust monster */
#define CHAM_DARN_DEMENTOR	24	/* ghost */
#define CHAM_SHOEMELEON		25	/* shoe */
#define CHAM_POLYFESHNEE	26	/* demon */
#define CHAM_COVETOUSLEON	27	/* covetous */
#define CHAM_WHORED_HORE	28	/* whore */
#define CHAM_LULU_ASS	29	/* fart loud */
#define CHAM_TENDER_JESSE	30	/* fart quiet */
#define CHAM_ELEROTIC_DREAM_WOMAN	31	/* fart normal */
#define CHAM_MARTIIN	32	/* scent */
#define CHAM_FOREPREACHER_CONVERTER	33	/* converts */
#define CHAM_RICTIM_TERRORIZER	34	/* hcalien */
#define CHAM_POLYMORPHITIC_WOLF	35	/* spacewars */
#define CHAM_OFFDIVER	36	/* joke mon */
#define CHAM_SLUMBER_HULK	37	/* randomized monster */
#define CHAM_IVEL_WUXTINA	38	/* evil variant */
#define CHAM_EARLY_LEON		39	/* only select forms of level 5 or lower */
#define CHAM_CHAMECHAUN		40	/* animal */
#define CHAM_METAL_DOPPELGANGER	41	/* demi-human */
#define CHAM_GHELEON		42	/* animal */
#define CHAM_ZRUTINATOR		43	/* evil variant */
#define CHAM_METAMORPHOSE		44	/* random */
#define CHAM_GREEN_SLAAD		45	/* humanoid */
#define CHAM_GIANT_CHAMELEON		46	/* animal */
#define CHAM_MAX_INDX		CHAM_GIANT_CHAMELEON
	Bitfield(mundetected,1);	/* not seen in present hiding place */
				/* implies one of M1_CONCEAL or M1_HIDE,
				 * but not mimic (that is, snake, spider,
				 * trapper, piercer, eel)
				 */

	Bitfield(mcan,1);	/* has been cancelled */
	Bitfield(mburied,1);	/* has been buried */
	Bitfield(mspeed,2);	/* current speed */
	Bitfield(permspeed,2);	/* intrinsic mspeed value */
	Bitfield(mrevived,1);	/* has been revived from the dead */
	Bitfield(mavenge,1);	/* did something to deserve retaliation */

	Bitfield(mflee,1);	/* fleeing */
	Bitfield(mfleetim,7);	/* timeout for mflee */

	Bitfield(mcansee,1);	/* cansee 1, temp.blinded 0, blind 0 */
	Bitfield(mblinded,7);	/* cansee 0, temp.blinded n, blind 0 */

	Bitfield(mcanmove,1);	/* paralysis, similar to mblinded */
	Bitfield(mfrozen,7);
	Bitfield(masleep,1);	/* asleep, can spontaneously wake up */

	Bitfield(msleeping,1);	/* asleep until woken */
	Bitfield(mstun,1);	/* stunned (off balance) */
	Bitfield(mconf,1);	/* confused */
	Bitfield(mpeaceful,1);	/* does not attack unprovoked */
	Bitfield(mfrenzied,1);	/* cannot be tamed anymore */
	Bitfield(mtrapped,1);	/* trapped in a pit, web or bear trap */
	Bitfield(mleashed,1);	/* monster is on a leash */
	Bitfield(isspell,1);	/* is a temporary spell being */
	Bitfield(uexp,1);		/* you get experience for its kills */

	Bitfield(mtraitor,1);	/* Former pet that turned traitor */
	Bitfield(isshk,1);	/* is shopkeeper */
	Bitfield(isminion,1);	/* is a minion */
	Bitfield(isgd,1);	/* is guard */
	Bitfield(isgyp, 1);	/* is a gypsy */
	Bitfield(ispriest,1);	/* is a priest */
	Bitfield(iswiz,1);	/* is the Wizard of Yendor */
	Bitfield(wormno,5);	/* at most 31 worms on any level */
#define MAX_NUM_WORMS	32	/* should be 2^(wormno bitfield size) */

	long mstrategy;		/* for monsters with mflag3: current strategy */
#define STRAT_ARRIVE	0x40000000L	/* just arrived on current level */
#define STRAT_WAITFORU	0x20000000L
#define STRAT_CLOSE	0x10000000L
#define STRAT_WAITMASK	0x30000000L
#define STRAT_HEAL	0x08000000L
#define STRAT_GROUND	0x04000000L
#define STRAT_MONSTR	0x02000000L
#define STRAT_PLAYER	0x01000000L
#define STRAT_NONE	0x00000000L
#define STRAT_STRATMASK 0x0f000000L
#define STRAT_XMASK	0x00ff0000L
#define STRAT_YMASK	0x0000ff00L
#define STRAT_GOAL	0x000000ffL
#define STRAT_GOALX(s)	((xchar)((s & STRAT_XMASK) >> 16))
#define STRAT_GOALY(s)	((xchar)((s & STRAT_YMASK) >> 8))

	/*long mtrapseen;*/		/* bitmap of traps we've been trapped in */

	/* Amy edit: mtrapseen gets the axe. Sakusha (FHS dev) told me about a potential problem with it because we have
	 * more than 32 trap types, and the bitfield cannot handle that many. This bitfield may or may not have been
	 * the reason for some of the savegame errors and random crashes in the game, but the original implementation of it
	 * has never been very good anyway; why would a monster suddenly know about the location of all dart traps in the
	 * entire dungeon just because it triggered one once??? It doesn't make sense! */

	long mlstmv;		/* for catching up with lost time */
#ifndef GOLDOBJ
	long mgold;
#endif
	struct obj *minvent;

	Bitfield(isegotype,1);	/* monster has egotypes */
	Bitfield(egotype_thief,1);
	Bitfield(egotype_wallwalk,1);
	Bitfield(egotype_disenchant,1);
	Bitfield(egotype_rust,1);
	Bitfield(egotype_corrosion,1);
	Bitfield(egotype_decay,1);
	Bitfield(egotype_wither,1);
	Bitfield(egotype_grab,1);
	Bitfield(egotype_flying,1);
	Bitfield(egotype_hide,1);
	Bitfield(egotype_regeneration,1);
	Bitfield(egotype_undead,1);	/* This makes the monster mindless and unbreathing too. --Amy */
	Bitfield(egotype_domestic,1);
	Bitfield(egotype_covetous,1);
	Bitfield(egotype_avoider,1);
	Bitfield(egotype_petty,1);
	Bitfield(egotype_pokemon,1);
	Bitfield(egotype_slows,1);
	Bitfield(egotype_vampire,1);
	Bitfield(egotype_teleportself,1);
	Bitfield(egotype_teleportyou,1);
	Bitfield(egotype_wrap,1);
	Bitfield(egotype_disease,1);
	Bitfield(egotype_slime,1);
	Bitfield(egotype_engrave,1);
	Bitfield(egotype_dark,1);
	Bitfield(egotype_luck,1);
	Bitfield(egotype_push,1);
	Bitfield(egotype_arcane,1);
	Bitfield(egotype_clerical,1);

	Bitfield(egotype_armorer,1);
	Bitfield(egotype_tank,1);
	Bitfield(egotype_speedster,1);
	Bitfield(egotype_racer,1);

	Bitfield(egotype_randomizer,1);
	Bitfield(egotype_blaster,1); /* drains intelligence */
	Bitfield(egotype_multiplicator,1);

	Bitfield(egotype_gator,1);

	Bitfield(egotype_reflecting,1);
	Bitfield(egotype_hugger,1);

	Bitfield(egotype_mimic,1);
	Bitfield(egotype_permamimic,1);

	Bitfield(egotype_poisoner,1); /* resists poison, does AD_POIS */
	Bitfield(egotype_elementalist,1); /* resists fire/cold/elec, does AD_AXUS */
	Bitfield(egotype_resistor,1); /* resists death, disint and stoning */
	Bitfield(egotype_acidspiller,1); /* resists acid, does AD_ACID */
	Bitfield(egotype_watcher,1); /* resists sleep and drain */
	Bitfield(egotype_metallivore,1);
	Bitfield(egotype_lithivore,1);
	Bitfield(egotype_organivore,1);
	Bitfield(egotype_breather,1); /* also has reflection */
	Bitfield(egotype_beamer,1);
	Bitfield(egotype_troll,1);

	Bitfield(egotype_faker,1);
	Bitfield(egotype_farter,1);
	Bitfield(egotype_timer,1);
	Bitfield(egotype_thirster,1);
	Bitfield(egotype_watersplasher,1); /* AD_WET, or AD_LETH on lethe levels, plus M1_SWIM */
	Bitfield(egotype_cancellator,1);
	Bitfield(egotype_banisher,1);
	Bitfield(egotype_shredder,1);
	Bitfield(egotype_abductor,1);
	Bitfield(egotype_incrementor,1);
	Bitfield(egotype_mirrorimage,1);
	Bitfield(egotype_curser,1); /* AD_CURS */
	Bitfield(egotype_horner,1); /* 1 in 1000 chance of AD_UVUU, otherwise AD_CHRN */
	Bitfield(egotype_lasher,1); /* AT_LASH, AD_MALK */
	Bitfield(egotype_cullen,1); /* AD_VAMP */
	Bitfield(egotype_webber,1);
	Bitfield(egotype_itemporter,1);
	Bitfield(egotype_schizo,1); /* AD_DEPR */
	Bitfield(egotype_nexus,1);
	Bitfield(egotype_sounder,1);
	Bitfield(egotype_gravitator,1);
	Bitfield(egotype_inert,1);
	Bitfield(egotype_antimage,1); /* AD_MANA */
	Bitfield(egotype_plasmon,1);
	Bitfield(egotype_weaponizer,1); /* AT_WEAP */
	Bitfield(egotype_engulfer,1);
	Bitfield(egotype_bomber,1); /* AT_EXPL */
	Bitfield(egotype_exploder,1); /* AT_BOOM */

	Bitfield(egotype_unskillor,1); /* AD_SKIL */
	Bitfield(egotype_blinker,1);
	Bitfield(egotype_psychic,1);
	Bitfield(egotype_abomination,1);
	Bitfield(egotype_gazer,1);
	Bitfield(egotype_seducer,1); /* AD_SSEX */
	Bitfield(egotype_flickerer,1); /* cannot be damaged in melee by the player */
	Bitfield(egotype_hitter,1);
	Bitfield(egotype_piercer,1);
	Bitfield(egotype_petshielder,1); /* pets do not attack it */
	Bitfield(egotype_displacer,1);
	Bitfield(egotype_lifesaver,1); /* 80% chance */
	Bitfield(egotype_venomizer,1); /* AD_VENO */
	Bitfield(egotype_dreameater,1); /* AD_DREA */
	Bitfield(egotype_nastinator,1); /* AD_NAST */
	Bitfield(egotype_baddie,1); /* AD_BADE */
	Bitfield(egotype_sludgepuddle,1); /* AD_SLUD */
	Bitfield(egotype_vulnerator,1); /* AD_VULN */
	Bitfield(egotype_marysue,1); /* AD_FUMB */
	Bitfield(egotype_shader,1); /* only silver and arcanium can hurt it */
	Bitfield(egotype_amnesiac,1); /* AD_AMNE */
	Bitfield(egotype_trapmaster,1); /* AD_TRAP */
	Bitfield(egotype_midiplayer,1); /* AD_MIDI */
	Bitfield(egotype_rngabuser,1); /* AD_RNG */
	Bitfield(egotype_mastercaster,1); /* AD_CAST */

	Bitfield(egotype_aligner,1); /* AD_ALIN */
	Bitfield(egotype_sinner,1); /* AD_SIN */
	Bitfield(egotype_aggravator,1); /* AD_AGGR */
	Bitfield(egotype_minator,1); /* AD_MINA */
	Bitfield(egotype_contaminator,1); /* AD_CONT melee */
	Bitfield(egotype_radiator,1); /* AD_CONT breath */
	Bitfield(egotype_weeper,1); /* AD_CONT gaze */
	Bitfield(egotype_reactor,1); /* AD_CONT hug */

	Bitfield(egotype_destructor,1); /* AD_DEST */
	Bitfield(egotype_trembler,1); /* AD_TREM */
	Bitfield(egotype_worldender,1); /* AD_RAGN */

	Bitfield(egotype_damager,1); /* AD_IDAM */
	Bitfield(egotype_antitype,1); /* AD_ANTI */
	Bitfield(egotype_painlord,1); /* AD_PAIN */
	Bitfield(egotype_empmaster,1); /* AD_TECH */
	Bitfield(egotype_spellsucker,1); /* AD_MEMO */
	Bitfield(egotype_eviltrainer,1); /* AD_TRAI */
	Bitfield(egotype_statdamager,1); /* AD_STAT */
	Bitfield(egotype_damagedisher,1); /* AD_DAMA */
	Bitfield(egotype_thiefguildmember,1); /* AD_THIE */
	Bitfield(egotype_rogue,1); /* AD_SEDU */

	Bitfield(egotype_steed,1); /* monster can be ridden */

	Bitfield(egotype_champion,1); /* deals 25% more damage */
	Bitfield(egotype_boss,1); /* deals 75% more damage */
	Bitfield(egotype_atomizer,1); /* deals 200% more damage */
	Bitfield(egotype_perfumespreader,1); /* MS_STENCH */
	Bitfield(egotype_converter,1); /* MS_CONVERT */
	Bitfield(egotype_wouwouer,1); /* MS_HCALIEN */

	Bitfield(egotype_allivore,1); /* eats every item it comes across */

	Bitfield(egotype_nastycurser,1); /* AD_NACU */
	Bitfield(egotype_sanitizer,1); /* AD_SANI */

	Bitfield(egotype_laserpwnzor,1); /* ultra-mega-hyper-dyper laser cannon */

	Bitfield(egotype_badowner,1); /* AD_RBAD */
	Bitfield(egotype_bleeder,1); /* AD_BLEE */
	Bitfield(egotype_shanker,1); /* AD_SHAN */
	Bitfield(egotype_terrorizer,1); /* AD_TERR */
	Bitfield(egotype_feminizer,1); /* AD_FEMI */
	Bitfield(egotype_levitator,1); /* AD_LEVI */
	Bitfield(egotype_illusionator,1); /* AD_ILLU */
	Bitfield(egotype_stealer,1); /* AD_SAMU */
	Bitfield(egotype_stoner,1); /* AD_STON, by Mickmane */
	Bitfield(egotype_maecke,1); /* AD_MAGC */
	Bitfield(egotype_flamer,1); /* AD_FLAM */

	Bitfield(butthurt,6);
	Bitfield(fartbonus,6); /* actually, max should be 9 */
	Bitfield(crapbonus,6);

	Bitfield(shinymonst,1);	/* 1 in 8192 chance, by aosdict */

	Bitfield(wastame,1);	/* for re-taming technique */

	Bitfield(sagesvisible,1);	/* for seeing the monster via sages helmet */
	Bitfield(internetvisible,1);	/* 1 in 9 chance to be visible to internet access */
	Bitfield(stunnovisible,1);	/* 3 in 4 chance to be visible to stunnopathy */
	Bitfield(selfhybridvisible,1);	/* 1 in 2 chance to be visible for self-hybridization */
	Bitfield(warningvisible,2);	/* 1 in 4 chance to be visible to warning (1 in 2 if double warning) */
	Bitfield(telepatvisible,2);	/* 1 in 6 chance to be visible to telepathy (1 in 3 if double telepathy) */
	/* these bigger bitfields mean: 1 = single source is enough, 2 = only see monster if two sources */
	Bitfield(seeinvisble,1);	/* 2 in 3 chance that single see invisible is enough */
	Bitfield(infravisble,1);	/* 2 in 3 chance that single infravision is enough */
	Bitfield(willbebanished,1);	/* got hit by a banishment attack and will be banished on its next turn */
	Bitfield(spressingseen,1);	/* saw you create a s-pressing trap and can thus no longer trigger it */
	Bitfield(scentvisible,1);	/* 2 in 3 chance to be visible to scent view */
	Bitfield(echolocatevisible,1);	/* 4 in 5 chance to be visible to echolocation */
	int hominglazer;	/* normally zero; very specific monsters can use this to charge up a cannon */
	int ogrethief;	/* normally zero; used for the huge ogre thief */
	int bleedout;	/* normally zero; monster bleeds out (takes continuous damage) if nonzero */
	int healblock;	/* normally zero; monster cannot heal over time if nonzero */
	int inertia;	/* normally zero; monster moves at half speed */

	struct obj *mw;
	long misc_worn_check;
	xchar weapon_check;

#ifdef LIVELOG_BONES_KILLER 
	char former_rank[50];	/* for bones' ghost rank in the former life */ 
#endif 
 
        /*
         * NOTE: DO NOT ADD PARTS TO MONST STRUCT AFTER THIS POINT!
         * ALL ADDITIONS SHOULD GO BEFORE!!
         *       --WAC
         */
	uchar mnamelth;		/* length of name (following mxlth) */
	short mxlth;		/* length of following data */
	/* in order to prevent alignment problems mextra should
	   be (or follow) a long int */
	int meating;		/* monster is eating timeout */
	long mextra[1]; /* monster dependent info */
};

/*
 * Note that mextra[] may correspond to any of a number of structures, which
 * are indicated by some of the other fields.
 *	isgd	 ->	struct egd
 *	ispriest ->	struct epri
 *	isshk	 ->	struct eshk
 *	isminion ->	struct emin
 *			(struct epri for roaming priests and angels, which is
 *			 compatible with emin for polymorph purposes)
 *	mtame	 ->	struct edog
 *			(struct epri for guardian angels, which do not eat
 *			 or do other doggy things)
 * Since at most one structure can be indicated in this manner, it is not
 * possible to tame any creatures using the other structures (the only
 * exception being the guardian angels which are tame on creation).
 */

#define newmonst(xl) (struct monst *)alloc((unsigned)(xl) + sizeof(struct monst))
#define dealloc_monst(mon) free((void *)(mon))

/* these are in mspeed */
#define MSLOW 1		/* slow monster */
#define MFAST 2		/* speeded monster */

#define NAME(mtmp)	(((char *)(mtmp)->mextra) + (mtmp)->mxlth)

#define MON_WEP(mon)	((mon)->mw)
#define MON_NOWEP(mon)	((mon)->mw = (struct obj *)0)

#define DEADMONSTER(mon)	((mon)->mhp < 1)

#endif /* MONST_H */
