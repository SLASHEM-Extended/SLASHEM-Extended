/*	SCCS Id: @(#)you.h	3.4	2000/05/21	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef YOU_H
#define YOU_H

#include "attrib.h"
#include "monst.h"
#ifndef PROP_H
#include "prop.h"		/* (needed here for util/makedefs.c) */
#endif
#include "skills.h"

/* KMH -- struct prop and assignments moved to prop.h */

/*** Substructures ***/

struct RoleName {
	const char	*m;	/* name when character is male */
	const char	*f;	/* when female; null if same as male */
};

struct RoleAdvance {
	/* "fix" is the fixed amount, "rnd" is the random amount */
	xchar infix, inrnd;	/* at character initialization */
	xchar lofix, lornd;	/* gained per level <  urole.xlev */
	xchar hifix, hirnd;	/* gained per level >= urole.xlev */
};

struct symbiotemon {
	boolean active;
	int mnum;	/* permanent monster index number */
	int mhp, mhpmax;
	boolean cursed, hvycurse, prmcurse, bbcurse, morgcurse, evilcurse, stckcurse;
};

struct u_have {
	Bitfield(amulet,1);	/* carrying Amulet	*/
	Bitfield(bell,1);	/* carrying Bell	*/
	Bitfield(book,1);	/* carrying Book	*/
	Bitfield(menorah,1);	/* carrying Candelabrum */
	Bitfield(questart,1);	/* carrying the Quest Artifact */
	Bitfield(unused,3);
};


struct u_event {
	Bitfield(minor_oracle,1);	/* received at least 1 cheap oracle */
	Bitfield(major_oracle,1);	/*  "  expensive oracle */
	Bitfield(qcalled,1);		/* called by Quest leader to do task */
	Bitfield(qexpelled,1);		/* expelled from the Quest dungeon */
	Bitfield(qcompleted,1);		/* successfully completed Quest task */
	Bitfield(uheard_tune,2);	/* 1=know about, 2=heard passtune */
	Bitfield(uopened_dbridge,1);	/* opened the drawbridge */

	Bitfield(invoked,1);		/* invoked Gate to the Sanctum level */
	Bitfield(gehennom_entered,1);	/* entered Gehennom via Valley */
	Bitfield(uhand_of_elbereth,2);	/* became Hand of Elbereth */
	Bitfield(udemigod,1);		/* killed the wiz */
	Bitfield(ascended,1);		/* has offered the Amulet */
};

/* KMH, conduct --
 * These are voluntary challenges.  Each field denotes the number of
 * times a challenge has been violated.
 */
struct u_conduct {		/* number of times... */
	long	unvegetarian;	/* eaten any animal */
	long	unvegan;	/* ... or any animal byproduct */
	long	food;		/* ... or any comestible */
	long	gnostic;	/* used prayer, priest, or altar */
	long	praydone;	/* used prayer */
	long	weaphit;	/* hit a monster with a weapon */
	long	killer;		/* killed a monster yourself */
	long	literate;	/* read something (other than BotD) */
	long	polypiles;	/* polymorphed an object */
	long	polyselfs;	/* transformed yourself */
	long	wishes;		/* used a wish */
	long	wisharti;	/* wished for an artifact */
				/* genocides already listed at end of game */
	long	celibacy;	/* How many times you've played around with *ubbi */
};

/*** Unified structure containing role information ***/
struct Role {
	/*** Strings that name various things ***/
	struct RoleName name;	/* the role's name (from u_init.c) */
	struct RoleName rank[9]; /* names for experience levels (from botl.c) */
	const char *lgod, *ngod, *cgod; /* god names (from pray.c) */
	const char *filecode;	/* abbreviation for use in file names */
	const char *homebase;	/* quest leader's location (from questpgr.c) */
	const char *intermed;	/* quest intermediate goal (from questpgr.c) */

	/*** Indices of important monsters and objects ***/
	int malenum,		/* index (PM_) as a male (botl.c) */
	      femalenum,	/* ...or as a female (NON_PM == same) */
		undeadmalenum,	/* index (PM_) as an undead */
		undeadfemalenum,	/* mainly for bones files --Amy */
	      petnum,		/* PM_ of preferred pet (NON_PM == random) */
	      ldrnum,		/* PM_ of quest leader (questpgr.c) */
	      guardnum,		/* PM_ of quest guardians (questpgr.c) */
	      neminum,		/* PM_ of quest nemesis (questpgr.c) */
		nemesnum,		/* PM_ of corresponding nemese (makemon.c) --Amy */
	      enemy1num,	/* specific quest enemies (NON_PM == random) */
	      enemy2num;
	char  enemy1sym,	/* quest enemies by class (S_) */
	      enemy2sym;
#if 0
	short gift1arti,		/* Index (ART_) of first artifact gift */
	      gift2arti;		/* Index (ART_) of second artifact gift */
#endif
	/*short*/int questarti;	/* index (ART_) of quest artifact (questpgr.c) */

	/*** Bitmasks ***/
	short allow;		/* bit mask of allowed variations */
#define ROLE_RACEMASK	0x0ff8		/* allowable races */
#define ROLE_GENDMASK	0xf000		/* allowable genders */
#define ROLE_MALE	0x1000
#define ROLE_FEMALE	0x2000
#define ROLE_NEUTER	0x4000
#define ROLE_ALIGNMASK	AM_MASK		/* allowable alignments */
#define ROLE_LAWFUL	AM_LAWFUL
#define ROLE_NEUTRAL	AM_NEUTRAL
#define ROLE_CHAOTIC	AM_CHAOTIC

	/*** Attributes (from attrib.c and exper.c) ***/
	xchar attrbase[A_MAX];	/* lowest initial attributes */
	xchar attrdist[A_MAX];	/* distribution of initial attributes */
	xchar attrlimt[A_MAX];	/* going over the limit is hard --Amy */
	struct RoleAdvance hpadv; /* hit point advancement */
	struct RoleAdvance enadv; /* energy advancement */
	xchar xlev;		/* cutoff experience level */
	xchar initrecord;	/* initial alignment record */

	/*** Spell statistics (from spell.c) ***/
	int spelbase;		/* base spellcasting penalty */
	int spelheal;		/* penalty (-bonus) for healing spells */
	int spelshld;		/* penalty for wearing any shield */
	int spelarmr;		/* penalty for wearing metal armour */
	int spelstat;		/* which stat (A_) is used */
	int spelspec;		/* spell (SPE_) the class excels at */
	int spelsbon;		/* penalty (-bonus) for that spell */
	int spelrete;		/* spell retention - how many spells you can memorize before forgetting them is faster */

	/*** Properties in variable-length arrays ***/
	/* intrinsics (see attrib.c) */
	/* initial inventory (see u_init.c) */
	/* skills (see u_init.c) */

	/*** Don't forget to add... ***/
	/* quest leader, guardians, nemesis (monst.c) */
	/* quest artifact (artilist.h) */
	/* quest dungeon definition (dat/Xyz.dat) */
	/* quest text (dat/quest.txt) */
	/* dictionary entries (dat/data.bas) */
};

extern const struct Role roles[];	/* table of available roles */
extern struct Role urole;
extern struct Role ustartrole;
#define Role_if(X)	(urole.malenum == (X))
#define Role_switch	(urole.malenum)

/* used during initialization for race, gender, and alignment
   as well as for character class */
#define ROLE_NONE	(-1)
#define ROLE_RANDOM	(-2)

/*** Unified structure specifying race information ***/

struct Race {
	/*** Strings that name various things ***/
	const char *noun;	/* noun ("human", "elf") */
	const char *adj;	/* adjective ("human", "elven") */
	const char *coll;	/* collective ("humanity", "elvenkind") */
	const char *filecode;	/* code for filenames */
	struct RoleName individual; /* individual as a noun ("man", "elf") */

	/*** Indices of important monsters and objects ***/
	int malenum,		/* PM_ as a male monster */
	      femalenum,	/* ...or as a female (NON_PM == same) */
	      mummynum,		/* PM_ as a mummy */
	      zombienum;	/* PM_ as a zombie */

	/*** Bitmasks ***/
	short allow;		/* bit mask of allowed variations */
	short selfmask,		/* your own race's bit mask */
	      lovemask,		/* bit mask of always peaceful */
	      hatemask;		/* bit mask of always hostile */

	/*** Attributes ***/
	xchar attrmin[A_MAX];	/* minimum allowable attribute */
	xchar attrmax[A_MAX];	/* maximum allowable attribute */
	xchar attrtrs[A_MAX];	/* going over the limit is hard --Amy */
	struct RoleAdvance hpadv; /* hit point advancement */
	struct RoleAdvance enadv; /* energy advancement */
#if 0	/* DEFERRED */
	int   nv_range;		/* night vision range */
	int   xray_range;	/* X-ray vision range */
#endif

	/*** Properties in variable-length arrays ***/
	/* intrinsics (see attrib.c) */

	/*** Don't forget to add... ***/
	/* quest leader, guardians, nemesis (monst.c) */
	/* quest dungeon definition (dat/Xyz.dat) */
	/* quest text (dat/quest.txt) */
	/* dictionary entries (dat/data.bas) */
};

extern const struct Race races[];	/* Table of available races */
extern struct Race urace;
extern struct Race ustartrace;
#define Race_if(X)	(urace.malenum == (X))
#define Race_switch	(urace.malenum)

/*** Unified structure specifying gender information ***/
struct Gender {
	const char *adj;	/* male/female/neuter */
	const char *he;		/* he/she/it */
	const char *him;	/* him/her/it */
	const char *his;	/* his/her/its */
	const char *filecode;	/* file code */
	short allow;		/* equivalent ROLE_ mask */
};
#define ROLE_GENDERS	2	/* number of permitted player genders */
				/* increment to 3 if you allow neuter roles */

extern const struct Gender genders[];	/* table of available genders */
#define uhe()	(Role_if(PM_GENDERSTARIST) ? "he/she/it" : genders[flags.female ? 1 : 0].he)
#define uhim()	(Role_if(PM_GENDERSTARIST) ? "him/her/it" : genders[flags.female ? 1 : 0].him)
#define uhis()	(Role_if(PM_GENDERSTARIST) ? "his/her/its" : genders[flags.female ? 1 : 0].his)
#define mhe(mtmp)	(Role_if(PM_GENDERSTARIST) ? "he/she/it" : genders[pronoun_gender(mtmp)].he)
#define mhim(mtmp)	(Role_if(PM_GENDERSTARIST) ? "him/her/it" : genders[pronoun_gender(mtmp)].him)
#define mhis(mtmp)	(Role_if(PM_GENDERSTARIST) ? "his/her/its" : genders[pronoun_gender(mtmp)].his)


/*** Unified structure specifying alignment information ***/
struct Align {
	const char *noun;	/* law/balance/chaos */
	const char *adj;	/* lawful/neutral/chaotic */
	const char *filecode;	/* file code */
	short allow;		/* equivalent ROLE_ mask */
	aligntyp value;		/* equivalent A_ value */
};
#define ROLE_ALIGNS	3	/* number of permitted player alignments */

extern const struct Align aligns[];	/* table of available alignments */


/*** Information about the player ***/
struct you {
	xchar ux, uy;
	schar dx, dy, dz;	/* direction of move (or zap or ... ) */
	schar di;		/* direction of FF */
	xchar tx, ty;		/* destination of travel */
	xchar ux0, uy0;		/* initial position FF */
	d_level uz, uz0;	/* your level on this and the previous turn */
	d_level utolev;		/* level monster teleported you to, or uz */
	uchar utotype;		/* bitmask of goto_level() flags for utolev */
	boolean umoved;		/* changed map location (post-move) */
	int last_str_turn;	/* 0: none, 1: half turn, 2: full turn */
				/* +: turn right, -: turn left */
	int ulevel;		/* 1 to MAXULEV */
	int ulevelmax;		/* Maximmum level achieved */
	unsigned utrap;		/* trap timeout */
	unsigned utraptype;	/* defined if utrap nonzero */
#define TT_BEARTRAP	0
#define TT_PIT		1
#define TT_WEB		2
#define TT_LAVA		3
#define TT_INFLOOR	4
#define TT_GLUE	5
	char	urooms[5];	/* rooms (roomno + 3) occupied now */
	char	urooms0[5];	/* ditto, for previous position */
	char	uentered[5];	/* rooms (roomno + 3) entered this turn */
	char	ushops[5];	/* shop rooms (roomno + 3) occupied now */
	char	ushops0[5];	/* ditto, for previous position */
	char	ushops_entered[5]; /* ditto, shops entered this turn */
	char	ushops_left[5]; /* ditto, shops exited this turn */

	int	 uhunger;	/* refd only in eat.c and shk.c */
	unsigned uhs;		/* hunger state - see eat.c */
	int	weakcheat;	/* for wiseguys who think they can get max strength with a ring of stat lock --Amy */
	long	stethocheat;	/* prevents stethoscopes from being used for free after restoring */

	boolean ukinghill; /* records if you are carying the pirate treasure (and are therefor king of the hill) */
	int protean; /* counter for the auto-polypiling power of the*/
	struct prop uprops[LAST_PROP+1];

	unsigned umconf;
	char usick_cause[PL_PSIZ+20]; /* sizeof "unicorn horn named "+1 */
	Bitfield(usick_type,2);
#define SICK_VOMITABLE 0x01
#define SICK_NONVOMITABLE 0x02
#define SICK_ALL 0x03

	/* These ranges can never be more than MAX_RANGE (vision.h). */
	int nv_range;		/* current night vision range */
	int xray_range;		/* current xray vision range */

	/*
	 * These variables are valid globally only when punished and blind.
	 */
#define BC_BALL  0x01	/* bit mask for ball  in 'bc_felt' below */
#define BC_CHAIN 0x02	/* bit mask for chain in 'bc_felt' below */
	int bglyph;	/* glyph under the ball */
	int cglyph;	/* glyph under the chain */
	int bc_order;	/* ball & chain order [see bc_order() in ball.c] */
	int bc_felt;	/* mask for ball/chain being felt */

	int umonster;			/* hero's "real" monster num */
	int umonnum;			/* current monster number */

	int mh, mhmax, mtimedone;	/* for polymorph-self */
	struct attribs	macurr,		/* for monster attribs */
			mamax;		/* for monster attribs */
	int ulycn;			/* lycanthrope type */

	unsigned ucreamed;
	unsigned uswldtim;		/* time you have been swallowed */

	Bitfield(uswallow,1);		/* true if swallowed */
	Bitfield(uinwater,1);		/* if you're currently in water (only
					   underwater possible currently) */
	Bitfield(uundetected,1);	/* if you're a hiding monster/piercer */
	Bitfield(mfemale,1);		/* saved human value of flags.female */
	Bitfield(uinvulnerable,1);	/* you're invulnerable (praying) */
	Bitfield(uburied,1);		/* you're buried */
	Bitfield(uedibility,1);		/* blessed food detection; sense unsafe food */
	/* 1 free bit! */

	struct u_conduct uconduct;	/* KMH, conduct */
	unsigned udg_cnt;		/* how long you have been demigod */
	struct u_event	uevent;		/* certain events have happened */
	struct u_have	uhave;		/* you're carrying special objects */
	struct attribs	acurr,		/* your current attributes (eg. str)*/
			aexe,		/* for gain/loss via "exercise" */
			abon,		/* your bonus attributes (eg. str) */
			amax,		/* your max attributes (eg. str) */
			atemp,		/* used for temporary loss/gain */
			atime;		/* used for loss/gain countdown */
	align	ualign;			/* character alignment */
	int alignlim;	/* used to be a define, but we want variation --Amy */
#define CONVERT		2
#define A_ORIGINAL	1
#define A_CURRENT	0
	aligntyp ualignbase[CONVERT];	/* for ualign conversion record */
	schar uluck, moreluck, moreluckpts;		/* luck and luck bonus */
#define Luck	(AssholeModeActive ? -13 : flags.dudley ? -13 : issuxxor ? -13 : (uleft && uleft->otyp == RIN_DOOM) ? -13 : (uright && uright->otyp == RIN_DOOM) ? -13 : (Role_if(PM_FAILED_EXISTENCE) && ((u.uluck + u.moreluck + Role_if(PM_AUGURER) ) > 0) ) ? 0 : (uwep && uwep->oartifact == ART_ONCE_IN_A_BLUE_MOON && flags.friday13 && flags.moonphase == NEW_MOON) ? 13 :  (uarmh && uarmh->otyp == HELM_OF_BEGINNER_S_LUCK && flags.beginner) ? 13 : (u.uluck + u.moreluck + Role_if(PM_AUGURER) )) /* credits to the Dudley's Dungeon guys --Amy */
#define LUCKADD		3	/* added value when carrying luck stone */
#define LUCKMAX		10	/* on moonlit nights 11 */
#define LUCKMIN		(-10)
	schar	uhitinc;		/* KMH -- additional to-hit bonus */
	int uhitincxtra;
	long	uhealbonus;		/* KMH -- Healing bonus from healthstones */

	schar	udaminc;		/* Additional damage bonus */
	int udamincxtra;
	int	uac;

	uchar	uspellprot;		/* protection by SPE_PROTECTION */
	uchar	usptime;		/* #moves until uspellprot-- */
	uchar	uspmtime;		/* #moves between uspellprot-- */

	const char *rivalcode;	/* "filecode" for rival quest, abbreviation for use in file names */
	int rivalneminum;
	int rivalenemy1num;
	int rivalenemy2num;
	char rivalenemy1sym;
	char rivalenemy2sym;

	int     uhp, uhpmax,uhplast;
	int	uen, uenmax;		/* magical energy - M. Stephenson */
	int ugangr;			/* if the gods are angry at you */
	int ugifts;			/* number of artifacts bestowed */
	int ublessed, ublesscnt;	/* blessing/duration from #pray */
	int monstertimeout; 	/* when monster spawns are increasing in frequency */
	int monstertimefinish; 	/* point in time when monster spawns are at their maximum */
	int ascensiontimelimit;	/* If you haven't ascended at this turn yet, add random nasty effects for annoyance. */
	int ascensionfirsthint;
	int ascensionsecondhint;
	int legscratching; 	/* for special AT_KICK attacks */
	int tensionmonster; 	/* for tension rooms */
	int tensionmonsteX; 	/* for the chance of getting really bad stuff in tension rooms */
	int tensionmonsterB; 	/* for mixed tension rooms */
	int colormonster; 	/* for tension rooms */
	int colormonsterB; 	/* for tension rooms */
	int specialtensionmonster; 	/* for themed tension rooms */
	int specialtensionmonsterB; 	/* for themed tension rooms */
	int statuetrapname;	/* for mimics pretending to be statues etc. */

	struct permonst *tensionmonsterspec; 	/* for tension rooms */
	struct permonst *tensionmonsterspecB; 	/* for mixed tension rooms */
	int next_check; 	/* attrib.c check */

	int urmaxlvl;		/* for rodneyan race */
	int urmaxlvlB;		/* for cyborg role */
	int urmaxlvlC;		/* for binder role */
	int urmaxlvlD;		/* for bard role */
	int urmaxlvlE;		/* for american race */
	int urmaxlvlF;		/* for wild talent role */
	int urmaxlvlG;		/* for wild talent role */
	int urmaxlvlH;		/* for mystic role */
	int urmaxlvlI;		/* for DQ slime combined with green slime race */
	int urmaxlvlJ;		/* for anachronounbinder role */
	int urmaxlvlK;		/* for SJW role */
	int urmaxlvlL;		/* for asgardian race */
	int urmaxlvlM;		/* for missingno race */
	int urmaxlvlN;		/* for extra skill slots */
	int urmaxlvlUP;		/* for determining whether levelups refill your HP and Pw */
	int xtralevelmult;	/* multiplier that determines how much EXP you need to advance again if at XL30 */
	int uhereticgodinit;
	int uhereticgodlawful;
	int uhereticgodneutral;
	int uhereticgodchaotic;
	const char *hereticlgod, *hereticngod, *hereticcgod; /* god names (from pray.c) */
	int urealedibility;	/* no longer a boolean --Amy */
	int wormpolymorph;	/* for worm that walks race */

	int youpolyamount;	/* how many times you can use #youpoly --Amy */

	int casinochips;		/* how often you can play blackjack without having to pay money */

	int moneydebt;		/* for AD_DEBT: player has to pay money to the bank --Amy */

	int eeveelution;		/* mainly for pokemon role but might be useful for others */

	int ughmemory;		/* set to 1 if you eat an ugh memory to create inventory window */

	int gottenbones;		/* set to 1 if you encountered a bones level */

	int ungenocidable;	/* evil patch idea by jonadab - monster class that can't be genocided at all */

	int alwaysinvisible;	/* evil patch idea by alpha123 - monster class that's always invisible */
	int alwayshidden;

	int minimalism;		/* chance that an item is generated is only 1 in X for minimalist race */

	int frequentmonster;	/* evil patch idea by Amy - monster class that is generated more often */
	int freqmonsterbonus;	/* amount that is added to said monster class's frequency */

	int frequentcolor;	/* evil patch idea by Amy - monster color that is generated more often */
	int freqcolorbonus;	/* amount that is added to said monster color's frequency */

	int frequentspecies;	/* evil patch idea by Amy - monster species that is generated more often */
	int freqspeciesbonus;	/* amount that is added to said monster species's frequency */

	int frequentspecies2;	
	int freqspeciesbonus2;	
	int frequentspecies3;	
	int freqspeciesbonus3;	
	int frequentspecies4;	
	int freqspeciesbonus4;	
	int frequentspecies5;	
	int freqspeciesbonus5;	
	int frequentspecies6;	
	int freqspeciesbonus6;	
	int frequentspecies7;	
	int freqspeciesbonus7;	
	int frequentspecies8;	
	int freqspeciesbonus8;	
	int frequentspecies9;	
	int freqspeciesbonus9;	
	int frequentspecies10;	
	int freqspeciesbonus10;	

	int frequentspecies11;	
	int freqspeciesbonus11;	
	int frequentspecies12;	
	int freqspeciesbonus12;	
	int frequentspecies13;	
	int freqspeciesbonus13;	
	int frequentspecies14;	
	int freqspeciesbonus14;	
	int frequentspecies15;	
	int freqspeciesbonus15;	
	int frequentspecies16;	
	int freqspeciesbonus16;	
	int frequentspecies17;	
	int freqspeciesbonus17;	
	int frequentspecies18;	
	int freqspeciesbonus18;	
	int frequentspecies19;	
	int freqspeciesbonus19;	
	int frequentspecies20;	
	int freqspeciesbonus20;	

	int frequenttrait1;	/* monsters with this trait spawn more often --Amy */
	int freqtraitbonus1;
	int frequenttrait2;
	int freqtraitbonus2;
	int frequenttrait3;
	int freqtraitbonus3;
	int frequenttrait4;
	int freqtraitbonus4;
	int frequenttrait5;
	int freqtraitbonus5;
	int frequenttrait6;
	int freqtraitbonus6;
	int frequenttrait7;
	int freqtraitbonus7;
	int frequenttrait8;
	int freqtraitbonus8;
	int frequenttrait9;
	int freqtraitbonus9;
	int frequenttrait10;
	int freqtraitbonus10;

	int speedymonster;	/* monster species that is generated with speed */
	int musemonster;		/* monster species that has a higher chance of generating with musable stuff */

	int idscrollpenalty;	/* evil patch idea by Amy - certain item types may randomly be much harder to id */
	int idpotionpenalty;
	int idringpenalty;
	int idamuletpenalty;
	int idimplantpenalty;
	int idwandpenalty;
	int idarmorpenalty;
	int idspellbookpenalty;
	int idtoolpenalty;
	int idgempenalty;

	int nospawnspecies;	/* nice patch idea by Amy - monster species that isn't randomly generated */
	int nospawnspecies2;
	int nospawnspecies3;
	int nospawnspecies4;
	int nospawnspecies5;
	int nospawnspecies6;
	int nospawnspecies7;
	int nospawnspecies8;
	int nospawnspecies9;
	int nospawnspecies10;

	int unobtainable;		/* evil patch idea - item type that can't be obtained at all */
	int unobtainable2;	
	int unobtainable3;	/* amount of such items must be lower than the amount of candles in existence */
	int unobtainable4;	
	int unobtainable5;	
	int unobtainable6;	
	int unobtainable7;	
	int unobtainable8;	
	int unobtainable9;	
	int unobtainable10;	
	int unobtainablegeno;	/* for item genocide */
	int veryobtainable;	/* nice patch idea - item type that is generated more frequently */
	int veryobtainableboost;	/* frequency added */
	int veryobtainable2;
	int veryobtainableboost2;
	int veryobtainable3;
	int veryobtainableboost3;
	int veryobtainable4;
	int veryobtainableboost4;
	int veryobtainable5;
	int veryobtainableboost5;
	int veryobtainable6;
	int veryobtainableboost6;
	int veryobtainable7;
	int veryobtainableboost7;
	int veryobtainable8;
	int veryobtainableboost8;
	int veryobtainable9;
	int veryobtainableboost9;
	int veryobtainable10;
	int veryobtainableboost10;
	int veryobtainable11;
	int veryobtainableboost11;
	int veryobtainable12;
	int veryobtainableboost12;
	int veryobtainable13;
	int veryobtainableboost13;
	int veryobtainable14;
	int veryobtainableboost14;
	int veryobtainable15;
	int veryobtainableboost15;
	int veryobtainable16;
	int veryobtainableboost16;
	int veryobtainable17;
	int veryobtainableboost17;
	int veryobtainable18;
	int veryobtainableboost18;
	int veryobtainable19;
	int veryobtainableboost19;
	int veryobtainable20;
	int veryobtainableboost20;

	int veryobtainclass1;		/* item class that is generated more often */
	int veryobtainclass1boost;	/* frequency added */
	int veryobtainclass2;
	int veryobtainclass2boost;
	int veryobtainclass3;
	int veryobtainclass3boost;

	int alwayscurseditem;		/* evil patch idea - item type that is always generated cursed */
	int alwayscurseditem2;
	int alwayscurseditem3;
	int alwayscurseditem4;
	int alwayscurseditem5;
	int alwayscurseditem6;
	int alwayscurseditem7;
	int alwayscurseditem8;
	int alwayscurseditem9;
	int alwayscurseditem10;

	int spellbookbias1;	/* spell school that is slightly more common */
	int spellbookbias2;
	int spellbookbias3;
	int spellbookchance1;
	int spellbookchance2;
	int spellbookchance3;

	int menoraget;
	int bookofthedeadget;
	int silverbellget;
	int chaoskeyget;
	int neutralkeyget;
	int lawfulkeyget;
	int luckstoneget;
	int sokobanfinished;
	int deepminefinished;
	int medusaremoved;
	int glassgolemdown;
	int tiksrvzllatspawn;
	int tiksrvzllatdown;
	int netherrealmcomplete;
	int angmarcomplete;
	int rivalquestcomplete;
	int stahngnirdown;
	int veradown;
	int elainedown;
	int arianespawned;
	int arianedown;
	int emynluincomplete;
	int minotaurmazecomplete;
	int katiaremoved;
	int sewerplantcomplete;
	int gammacavescomplete;
	int bofhremoved;
	int poolchallengecomplete;
	int digdugchallengecomplete;
	int pacmanchallengecomplete;
	int joustchallengecomplete;
	int gruechallengecomplete;
	int devnullcomplete;
	boolean arstechnicadone;

	int maxrndmonstchoicecount;	/* for rndmonst() in makemon.c */

	int greencrosschance;
	boolean greencrossopen;

	boolean prematuresubquest;
	boolean prematureyendortower;

	int knowofthecursetimer;

	int laratraptimer;

	boolean roxannemode; /* for Fuel Nail artifact */
	int roxannetimer;

	int coronations;	/* how often have you been crowned? */

	int musableremovechance;	/* extra chance for musable items to get removed */
	int equipmentremovechance;

	int bookspawnchance;	/* lower spawn chance for certain item classes */
	int scrollspawnchance;
	int potionspawnchance;
	int ringspawnchance;
	int wandspawnchance;
	int amuletspawnchance;
	int implantspawnchance;
	int weaponspawnchance;
	int armorspawnchance;
	int toolspawnchance;
	int foodspawnchance;

	int concealitemchance;	/* chance for concealing monsters to start with zorkmids instead */

	int usefulitemchance; /* lower spawn chance for miscellaneous useful items */

	int antimusablebias; /* to make it less likely that random scrolls, potions or wands are musable */

	int invisotrap;	/* evil patch idea - trap that's always generated invisible */

	int frequenttrap;		/* evil patch idea by Amy - trap that is generated more often */
	int freqtrapbonus;	/* frequency bonus - the actual formula of how this affects generation odds is complicated */

	int traprandomizing;	/* chance that a random trap is "completely random" */

	int trapxtradiff;	/* additional difficulty for finding a trap */

	int shaperoomchance;	/* to control the chance of irregular rooms */

	int fumbleduration;	/* for wand of fumbling */
	int antimagicshell;	/* for scroll of antimagic */

	int stonedchilltimer;	/* to control whether you get the damage reduction */

	int missingstaircase;	/* for artifact that creates downstairs on the castle level */

	int soviettemporary;	/* for fleecey light */
	int evilvartemporary;	/* for fleecey light */

	int tumblrerpersist;	/* starting as tumblrer */
	int addictpersist;	/* starting as addict */
	int demagoguepersist;	/* starting as demagogue */

	boolean artifactprotection;	/* because NHTNG's protection code is faulty */

	boolean antiamysquad;	/* monster that gives its speech only once */

	boolean lamefarmer;	/* while this is true, newly generated monsters will have their equipment flagged as "doesn't drop" to reduce farming via e.g. create monster spells --Amy */

	int superspecialspell;	/* for pagan poetry artifact: this spell is easier to cast */

	/* Extra nastiness - nastinator race, but also other stuff uses this --Amy
	 * timerun (keyword for my grepping) */
	boolean nastinator01;
	boolean nastinator02;
	boolean nastinator03;
	boolean nastinator04;
	boolean nastinator05;
	boolean nastinator06;
	boolean nastinator07;
	boolean nastinator08;
	boolean nastinator09;
	boolean nastinator10;
	boolean nastinator11;
	boolean nastinator12;
	boolean nastinator13;
	boolean nastinator14;
	boolean nastinator15;
	boolean nastinator16;
	boolean nastinator17;
	boolean nastinator18;
	boolean nastinator19;
	boolean nastinator20;
	boolean nastinator21;
	boolean nastinator22;
	boolean nastinator23;
	boolean nastinator24;
	boolean nastinator25;
	boolean nastinator26;
	boolean nastinator27;
	boolean nastinator28;
	boolean nastinator29;
	boolean nastinator30;
	boolean nastinator31;
	boolean nastinator32;
	boolean nastinator33;
	boolean nastinator34;
	boolean nastinator35;
	boolean nastinator36;
	boolean nastinator37;
	boolean nastinator38;
	boolean nastinator39;
	boolean nastinator40;
	boolean nastinator41;
	boolean nastinator42;
	boolean nastinator43;
	boolean nastinator44;
	boolean nastinator45;
	boolean nastinator46;
	boolean nastinator47;
	boolean nastinator48;
	boolean nastinator49;
	boolean nastinator50;
	boolean nastinator51;
	boolean nastinator52;
	boolean nastinator53;
	boolean nastinator54;
	boolean nastinator55;
	boolean nastinator56;
	boolean nastinator57;
	boolean nastinator58;
	boolean nastinator59;
	boolean nastinator60;
	boolean nastinator61;
	boolean nastinator62;
	boolean nastinator63;
	boolean nastinator64;
	boolean nastinator65;
	boolean nastinator66;
	boolean nastinator67;
	boolean nastinator68;
	boolean nastinator69;
	boolean nastinator70;
	boolean nastinator71;
	boolean nastinator72;
	boolean nastinator73;
	boolean nastinator74;
	boolean nastinator75;
	boolean nastinator76;
	boolean nastinator77;
	boolean nastinator78;
	boolean nastinator79;
	boolean nastinator80;
	boolean nastinator81;
	boolean nastinator82;
	boolean nastinator83;
	boolean nastinator84;
	boolean nastinator85;
	boolean nastinator86;
	boolean nastinator87;
	boolean nastinator88;
	boolean nastinator89;
	boolean nastinator90;
	boolean nastinator91;
	boolean nastinator92;
	boolean nastinator93;
	boolean nastinator94;
	boolean nastinator95;
	boolean nastinator96;
	boolean nastinator97;
	boolean nastinator98;
	boolean nastinator99;
	boolean nastinator100;
	boolean nastinator101;
	boolean nastinator102;
	boolean nastinator103;
	boolean nastinator104;
	boolean nastinator105;
	boolean nastinator106;
	boolean nastinator107;
	boolean nastinator108;
	boolean nastinator109;
	boolean nastinator110;
	boolean nastinator111;
	boolean nastinator112;
	boolean nastinator113;
	boolean nastinator114;
	boolean nastinator115;
	boolean nastinator116;
	boolean nastinator117;
	boolean nastinator118;
	boolean nastinator119;
	boolean nastinator120;
	boolean nastinator121;
	boolean nastinator122;
	boolean nastinator123;
	boolean nastinator124;
	boolean nastinator125;
	boolean nastinator126;
	boolean nastinator127;
	boolean nastinator128;
	boolean nastinator129;
	boolean nastinator130;
	boolean nastinator131;
	boolean nastinator132;
	boolean nastinator133;
	boolean nastinator134;
	boolean nastinator135;
	boolean nastinator136;
	boolean nastinator137;
	boolean nastinator138;
	boolean nastinator139;
	boolean nastinator140;
	boolean nastinator141;
	boolean nastinator142;
	boolean nastinator143;
	boolean nastinator144;
	boolean nastinator145;
	boolean nastinator146;
	boolean nastinator147;
	boolean nastinator148;
	boolean nastinator149;
	boolean nastinator150;
	boolean nastinator151;
	boolean nastinator152;
	boolean nastinator153;
	boolean nastinator154;
	boolean nastinator155;
	boolean nastinator156;
	boolean nastinator157;
	boolean nastinator158;
	boolean nastinator159;
	boolean nastinator160;
	boolean nastinator161;
	boolean nastinator162;
	boolean nastinator163;
	boolean nastinator164;
	boolean nastinator165;
	boolean nastinator166;
	boolean nastinator167;
	boolean nastinator168;
	boolean nastinator169;
	boolean nastinator170;
	boolean nastinator171;
	boolean nastinator172;
	boolean nastinator173;
	boolean nastinator174;
	boolean nastinator175;
	boolean nastinator176;
	boolean nastinator177;
	boolean nastinator178;
	boolean nastinator179;
	boolean nastinator180;
	boolean nastinator181;
	boolean nastinator182;
	boolean nastinator183;
	boolean nastinator184;
	boolean nastinator185;
	boolean nastinator186;
	boolean nastinator187;
	boolean nastinator188;
	boolean nastinator189;
	boolean nastinator190;
	boolean nastinator191;
	boolean nastinator192;
	boolean nastinator193;
	boolean nastinator194;
	boolean nastinator195;
	boolean nastinator196;
	boolean nastinator197;
	boolean nastinator198;
	boolean nastinator199;
	boolean nastinator200;
	boolean nastinator201;
	boolean nastinator202;
	boolean nastinator203;
	boolean nastinator204;
	boolean nastinator205;
	boolean nastinator206;
	boolean nastinator207;
	boolean nastinator208;
	boolean nastinator209;
	boolean nastinator210;
	boolean nastinator211;
	boolean nastinator212;
	boolean nastinator213;
	boolean nastinator214;
	boolean nastinator215;
	boolean nastinator216;
	boolean nastinator217;
	boolean nastinator218;
	boolean nastinator219;
	boolean nastinator220;
	boolean nastinator221;
	boolean nastinator222;
	boolean nastinator223;
	boolean nastinator224;
	boolean nastinator225;
	boolean nastinator226;
	boolean nastinator227;
	boolean nastinator228;
	boolean nastinator229;
	boolean nastinator230;
	boolean nastinator231;
	boolean nastinator232;
	boolean nastinator233;
	boolean nastinator234;
	boolean nastinator235;
	boolean nastinator236;
	boolean nastinator237;
	boolean nastinator238;
	boolean nastinator239;
	boolean nastinator240;
	boolean nastinator241;
	boolean nastinator242;
	boolean nastinator243;
	boolean nastinator244;
	boolean nastinator245;
	boolean nastinator246;
	boolean nastinator247;
	boolean nastinator248;
	boolean nastinator249;
	boolean nastinator250;
	boolean nastinator251;
	boolean nastinator252;
	boolean nastinator253;
	boolean nastinator254;
	boolean nastinator255;
	boolean nastinator256;
	boolean nastinator257;
	boolean nastinator258;
	boolean nastinator259;
	boolean nastinator260;
	boolean nastinator261;
	boolean nastinator262;
	boolean nastinator263;
	boolean nastinator264;
	boolean nastinator265;
	boolean nastinator266;
	boolean nastinator267;
	boolean nastinator268;
	boolean nastinator269;
	boolean nastinator270;

	/* SJW role - I wonder if there'd be an easier way to do this... --Amy */
	int sjwL1E1;
	int sjwL2E1;
	int sjwL3E1;
	int sjwL4E1;
	int sjwL5E1;
	int sjwL5E2;
	int sjwL6E1;
	int sjwL6E2;
	int sjwL7E1;
	int sjwL7E2;
	int sjwL8E1;
	int sjwL8E2;
	int sjwL9E1;
	int sjwL9E2;
	int sjwL10E1;
	int sjwL10E2;
	int sjwL10E3;
	int sjwL11E1;
	int sjwL11E2;
	int sjwL11E3;
	int sjwL12E1;
	int sjwL12E2;
	int sjwL12E3;
	int sjwL13E1;
	int sjwL13E2;
	int sjwL13E3;
	int sjwL14E1;
	int sjwL14E2;
	int sjwL14E3;
	int sjwL15E1;
	int sjwL15E2;
	int sjwL15E3;
	int sjwL15E4;
	int sjwL16E1;
	int sjwL16E2;
	int sjwL16E3;
	int sjwL16E4;
	int sjwL17E1;
	int sjwL17E2;
	int sjwL17E3;
	int sjwL17E4;
	int sjwL18E1;
	int sjwL18E2;
	int sjwL18E3;
	int sjwL18E4;
	int sjwL19E1;
	int sjwL19E2;
	int sjwL19E3;
	int sjwL19E4;
	int sjwL20E1;
	int sjwL20E2;
	int sjwL20E3;
	int sjwL20E4;
	int sjwL20E5;
	int sjwL21E1;
	int sjwL21E2;
	int sjwL21E3;
	int sjwL21E4;
	int sjwL21E5;
	int sjwL22E1;
	int sjwL22E2;
	int sjwL22E3;
	int sjwL22E4;
	int sjwL22E5;
	int sjwL23E1;
	int sjwL23E2;
	int sjwL23E3;
	int sjwL23E4;
	int sjwL23E5;
	int sjwL24E1;
	int sjwL24E2;
	int sjwL24E3;
	int sjwL24E4;
	int sjwL24E5;
	int sjwL25E1;
	int sjwL25E2;
	int sjwL25E3;
	int sjwL25E4;
	int sjwL25E5;
	int sjwL25E6;
	int sjwL26E1;
	int sjwL26E2;
	int sjwL26E3;
	int sjwL26E4;
	int sjwL26E5;
	int sjwL26E6;
	int sjwL27E1;
	int sjwL27E2;
	int sjwL27E3;
	int sjwL27E4;
	int sjwL27E5;
	int sjwL27E6;
	int sjwL28E1;
	int sjwL28E2;
	int sjwL28E3;
	int sjwL28E4;
	int sjwL28E5;
	int sjwL28E6;
	int sjwL29E1;
	int sjwL29E2;
	int sjwL29E3;
	int sjwL29E4;
	int sjwL29E5;
	int sjwL29E6;
	int sjwL30E1;
	int sjwL30E2;
	int sjwL30E3;
	int sjwL30E4;
	int sjwL30E5;
	int sjwL30E6;
	int sjwL30E7;
	int sjwL31E1;
	int sjwL31E2;
	int sjwL31E3;
	int sjwL31E4;
	int sjwL31E5;
	int sjwL31E6;
	int sjwL31E7;
	int sjwL32E1;
	int sjwL32E2;
	int sjwL32E3;
	int sjwL32E4;
	int sjwL32E5;
	int sjwL32E6;
	int sjwL32E7;
	int sjwL33E1;
	int sjwL33E2;
	int sjwL33E3;
	int sjwL33E4;
	int sjwL33E5;
	int sjwL33E6;
	int sjwL33E7;
	int sjwL34E1;
	int sjwL34E2;
	int sjwL34E3;
	int sjwL34E4;
	int sjwL34E5;
	int sjwL34E6;
	int sjwL34E7;
	int sjwL35E1;
	int sjwL35E2;
	int sjwL35E3;
	int sjwL35E4;
	int sjwL35E5;
	int sjwL35E6;
	int sjwL35E7;
	int sjwL36E1;
	int sjwL36E2;
	int sjwL36E3;
	int sjwL36E4;
	int sjwL36E5;
	int sjwL36E6;
	int sjwL36E7;
	int sjwL37E1;
	int sjwL37E2;
	int sjwL37E3;
	int sjwL37E4;
	int sjwL37E5;
	int sjwL37E6;
	int sjwL37E7;
	int sjwL38E1;
	int sjwL38E2;
	int sjwL38E3;
	int sjwL38E4;
	int sjwL38E5;
	int sjwL38E6;
	int sjwL38E7;
	int sjwL39E1;
	int sjwL39E2;
	int sjwL39E3;
	int sjwL39E4;
	int sjwL39E5;
	int sjwL39E6;
	int sjwL39E7;
	int sjwL40E1;
	int sjwL40E2;
	int sjwL40E3;
	int sjwL40E4;
	int sjwL40E5;
	int sjwL40E6;
	int sjwL40E7;
	int sjwL41E1;
	int sjwL41E2;
	int sjwL41E3;
	int sjwL41E4;
	int sjwL41E5;
	int sjwL41E6;
	int sjwL41E7;
	int sjwL42E1;
	int sjwL42E2;
	int sjwL42E3;
	int sjwL42E4;
	int sjwL42E5;
	int sjwL42E6;
	int sjwL42E7;
	int sjwL43E1;
	int sjwL43E2;
	int sjwL43E3;
	int sjwL43E4;
	int sjwL43E5;
	int sjwL43E6;
	int sjwL43E7;
	int sjwL44E1;
	int sjwL44E2;
	int sjwL44E3;
	int sjwL44E4;
	int sjwL44E5;
	int sjwL44E6;
	int sjwL44E7;
	int sjwL45E1;
	int sjwL45E2;
	int sjwL45E3;
	int sjwL45E4;
	int sjwL45E5;
	int sjwL45E6;
	int sjwL45E7;
	int sjwL46E1;
	int sjwL46E2;
	int sjwL46E3;
	int sjwL46E4;
	int sjwL46E5;
	int sjwL46E6;
	int sjwL46E7;
	int sjwL47E1;
	int sjwL47E2;
	int sjwL47E3;
	int sjwL47E4;
	int sjwL47E5;
	int sjwL47E6;
	int sjwL47E7;
	int sjwL48E1;
	int sjwL48E2;
	int sjwL48E3;
	int sjwL48E4;
	int sjwL48E5;
	int sjwL48E6;
	int sjwL48E7;
	int sjwL49E1;
	int sjwL49E2;
	int sjwL49E3;
	int sjwL49E4;
	int sjwL49E5;
	int sjwL49E6;
	int sjwL49E7;
	int sjwL50E1;
	int sjwL50E2;
	int sjwL50E3;
	int sjwL50E4;
	int sjwL50E5;
	int sjwL50E6;
	int sjwL50E7;
	int sjwL51E1;
	int sjwL51E2;
	int sjwL51E3;
	int sjwL51E4;
	int sjwL51E5;
	int sjwL51E6;
	int sjwL51E7;
	int sjwL52E1;
	int sjwL52E2;
	int sjwL52E3;
	int sjwL52E4;
	int sjwL52E5;
	int sjwL52E6;
	int sjwL52E7;
	int sjwL53E1;
	int sjwL53E2;
	int sjwL53E3;
	int sjwL53E4;
	int sjwL53E5;
	int sjwL53E6;
	int sjwL53E7;
	int sjwL54E1;
	int sjwL54E2;
	int sjwL54E3;
	int sjwL54E4;
	int sjwL54E5;
	int sjwL54E6;
	int sjwL54E7;
	int sjwL55E1;
	int sjwL55E2;
	int sjwL55E3;
	int sjwL55E4;
	int sjwL55E5;
	int sjwL55E6;
	int sjwL55E7;
	int sjwL56E1;
	int sjwL56E2;
	int sjwL56E3;
	int sjwL56E4;
	int sjwL56E5;
	int sjwL56E6;
	int sjwL56E7;
	int sjwL57E1;
	int sjwL57E2;
	int sjwL57E3;
	int sjwL57E4;
	int sjwL57E5;
	int sjwL57E6;
	int sjwL57E7;
	int sjwL58E1;
	int sjwL58E2;
	int sjwL58E3;
	int sjwL58E4;
	int sjwL58E5;
	int sjwL58E6;
	int sjwL58E7;
	int sjwL59E1;
	int sjwL59E2;
	int sjwL59E3;
	int sjwL59E4;
	int sjwL59E5;
	int sjwL59E6;
	int sjwL59E7;

	int dorian4;
	int dorian8;
	int dorian12;
	int dorian16;
	int dorian20;
	int dorian24;
	int dorian28;

	int feminizeffect; /* for feminizer race to decide which feminism trap effect is currently active */
	int contamjeweleffect; /* for "not gonna contaminate" artifact jewel */

	char	starlit1[BUFSZ];
	char	starlit2[BUFSZ];
	char	starlit3[BUFSZ];

	char	strshamblerx[BUFSZ];
	char	strshamblerxa[BUFSZ];
	char	strpokshamblert[BUFSZ];
	char	strpokshamblertp[BUFSZ];
	char	strshamblerz[BUFSZ];
	char	strshamblerza[BUFSZ];

	char	strandommimic[BUFSZ];
	char	strandommimicb[BUFSZ];

	char	strandomkopb[BUFSZ];

	char	strandomfungus[BUFSZ];
	char	strandomfungusb[BUFSZ];
	char	strandomfungusc[BUFSZ];
	char	strandomfungusd[BUFSZ];
	char	strandomfungusdx[BUFSZ];
	char	strandomfunguse[BUFSZ];
	char	strandomfungusf[BUFSZ];
	char	strandomfungusg[BUFSZ];
	char	strandomfungush[BUFSZ];
	char	strandomfungusi[BUFSZ];
	char	strandomfungusj[BUFSZ];
	char	strandomfungusk[BUFSZ];
	char	strandomfungusl[BUFSZ];

	char	strandomdragonb[BUFSZ];
	char	strandomdragonc[BUFSZ];
	char	strandombdragonb[BUFSZ];
	char	strandombdragonc[BUFSZ];

	char	aliasname[PL_NSIZ];

	int echolocationspell;	/* timeout when you cast echolocation */

	int graundweight;		/* increased encumbrance that times out very slowly */

	int randomquestlevels;	/* chance to get quest levels instead of random ones */

	int hangupcheat;		/* for cheating players who think hanging up is cool */
	int hanguppenalty;
	int hangupparalysis;	/* saved paralysis counter so you *really* cannot cheat */

	int youaredead;		/* if you hang up at a prompt before an instadeath, i.e. sickness */
	int youarereallydead;	/* if you hang up at something like mind flayer instadeath, where life saving doesn't work */
	int datadeletedefer;	/* if you hang up at a pending data delete, you ch3at0r */

	int negativeprotection;	/* AD_NPRO - deduct this amount from the player's AC */
	int tremblingamount;	/* AD_TREM - reduces player's to-hit and spellcasting chances */
	int chokhmahdamage;	/* AD_CHKH - damage increases by 1 with every hit */
	int extralives;		/* for scroll of resurrection */

	int banishmentbeam;	/* an attempt to make wands of banishment used by monsters *not* segfault :P --Amy */
	int levelporting;		/* and nexus/weeping attacks should not segfault either */

	int inertia;	/* for inertia attack - if non-zero, the player moves at half speed */

	int sweatyshoetimer;	/* for "sweaty shoes" randomized appearance */

	int bankcashamount; /* for bank trap */
	int bankcashlimit;

	int egglayingtimeout;	/* otherwise you could just spam them, which would be unbalanced! */

	int dragonpolymorphtime;	/* elemental mages shall not be able to polymorph indefinitely, sorry ziratha */
	int werepolymorphtime;		/* lycanthropic races shouldn't, either */

	int dehydrationtime;
	int yawmtime;	/* this is not a typo, I know it's spelled "yawn" normally --Amy */

	int fakeartifacts;	/* keeping track of how many fake artifacts got generated */

	int impossibleproperty;	/* evil patch idea by jonadab - a property that you simply cannot get in this game */
	int nonextrinsicproperty;
	int nonintrinsicproperty;
	int nondoubleproperty;

	boolean temprecursion; /* temporary recursion trap */
	int oldrecursionrole;
	int oldrecursionrace;
	int temprecursiontime;

	boolean demagoguerecursion; /* demagogue special ability */
	int demagoguerecursiontime;
	int demagogueabilitytimer;

	boolean totter;	/* scroll of symmetry toggles this */

	boolean ataruinvoked;
	boolean ataratarinvoked;

	int nurseextracost;	/* for chat-to-nurses functionality */
	int nursedecontamcost;	/* ditto */
	int nursevaccinecost;
	int nursesymbiotecost;
	int nurseshutdowncost;

	int mondiffhack;	/* for functions that spawn monsters with specific traits */

#ifdef NOARTIFACTWISH
	int usacrifice;                 /* number of sacrifices so far */
#endif
#if 0
	/* KMH -- Drow is now its own role... */
	boolean uelf_drow;		/* are you a drow? */
	int ustrucklast;		/* Seems unused */
#endif
#ifndef GOLDOBJ
    long    ugold, ugold0;
#else
	long	umoney0;
#endif
	long	uexp, urexp;
	long	ucleansed;	/* to record moves when player was cleansed */
	long	usleep;		/* sleeping; monstermove you last started */
	int uinvault;
	struct monst *ustuck;
	struct monst *usteed;
	long ugallop;
	int urideturns;
	int uhighheelturns;
	int usexyflatturns;
	int ugeneralcombatturns;
	int utwohandedcombatturns;
	int ubodyarmorturns;
	int umissileweaponturns;
	int uimplantturns;
	int ushiichoturns;
	int usoresuturns;
	int umakashiturns;
	int uataruturns;
	int unimanturns;
	int uvaapadturns;
	int uwediturns;
	int umemorizationturns;
	int uguncontrolturns;
	int usymbiosisslowturns;
	int usymbiosisfastturns;
	int	umortality;		/* how many times you died */
	int lifesavepenalty;	/* punish the player for being such a noob and dying :P */
	int lifesavepenallevel;	/* which level the punishment will occur on */
	int ugrave_arise; /* you die and become something aside from a ghost */
	time_t	ubirthday;		/* real world time when game began */

	int untrainableskill; /* you can't train all skills equally well (evil patch idea) */
	int halfspeedskill;
	int fifthspeedskill;
	int basiclimitskill;
	int skilledlimitskill;
	int expertlimitskill;
	int earlytrainingskill; /* becomes untrainable if you try to train it too early */
	int earlytrainingtimer;
	boolean earlytrainingblown;
	int frtrainingskill; /* becomes untrainable if you try to train it too early */
	int frtrainingtimer;
	boolean frtrainingblown;
	int latetrainingskill; /* becomes untrainable as soon as X turns have passed */
	int latetrainingtimer;
	int lavtrainingskill; /* is untrainable unless at least X turns have passed */
	int lavtrainingtimer;
	int slowtrainingskill; /* becomes harder to train the higher it already is */

	int invistrapchance; /* percentage chance for a random trap to be generated invisible */
	int xdifftrapchance; /* percentage chance for a random trap to have arbitrarily inflated difficulty */
	int shopitemreduction; /* percentage chance for shop items to not exist */

	int drippingtread;
	int drippingtreadtype;
	int geolysis;
	int disruptionshield;
	int spellbinder;
	int spellbinder1;
	int spellbinder2;
	int spellbinder3;
	int spellbinder4;
	int spellbinder5;
	int spellbinder6;
	int spellbinder7;
	int inertiacontrol;
	int inertiacontrolspell;
	int inertiacontrolspellno;
	int sterilized;
	int holyshield;

	int startstr, startdex, startint, startwis, startcon, startcha; /* your starting values for the six stats */

	int berserktime;
	int magicshield;
	int burrowed;
	int stasistime;
	int powerfailure;
	int tunnelized;
	int nailpolish;
	int enchantspell;
	int thornspell;
	boolean extracontres;

	int halresdeactivated;

	int pinkspelldirection;
	int trontrapdirection;
	int trontrapturn;

	int miscolornumber;
	int onerainbownumber;

	boolean polyformed;

	boolean boomshined;
	boolean kyliemode;

	boolean aggravation;
	boolean heavyaggravation;

	boolean outtadepthtrap;

	int riennevaplus;

	int stairscumslowing;

	boolean dirtifiedexcalibur;

	boolean symbioteattacking;
	boolean marleenproperties;

	int nataljetrapturns;
	int nataljetrapx, nataljetrapy;

	int contamination;	/* weeping angel, but also other effects */
	int alla;	/* for MS_ALLA */

	int copwantedlevel;	/* for hot pursuits, i.e. when you've stolen from a shop */
	int treesquadwantedlevel;	/* for MS_TREESQUAD, if this is positive they'll immediately turn hostile, always */
	int pompejiwantedlevel;	/* for MS_POMPEJI */
	int pompejipermanentrecord;
	int fluidatorwantedlevel;	/* for MS_FLUIDATOR */
	int fluidatorwarning;	/* game should warn you if you're about to increase your fluidator wanted level */
	int bulletatorwantedlevel;	/* for MS_BULLETATOR */
	int bulletatortimer;
	boolean bulletatorgun;
	boolean bulletatorbow;
	boolean bulletatorxbow;
	boolean bulletatorsling;
	boolean bulletatorjavelin;
	boolean itemcleanupneeded;

	int gmmailsreceived;	/* in game master mode, track how often I spawn monsters for you */

	int bedsleeping;	/* used to prevent you from constantly sleeping */
	int shiftingsandsinking;	/* to instakill you if you stand in shifting sand for too long */

	int femauspices4;	/* for feminist role: determine at game start which nasty trap effects you'll get later */
	int femauspices6;
	int femauspices8;
	int femauspices11;
	int femauspices13;
	int femauspices15;
	int femauspices17;
	int femauspices20;
	int femauspices22;
	int femauspices25;
	int femauspices28;
	int femauspices30;

	int climauspices3; /* for climacterial role: determine at game start which feminism trap effects you'll get later */
	int climauspices6;
	int climauspices9;
	int climauspices12;
	int climauspices15;
	int climauspices18;
	int climauspices21;
	int climauspices24;
	int climauspices27;
	int climauspices30;

	int stickycursechance; /* extra chance for items to become sticky cursed */
	int heavycursechance; /* extra chance for items that generate cursed to be generated heavily cursed */
	int primecursechance; /* ditto, but prime cursed */
	int genericcursechance; /* general extra chance that any random item is generated cursed */

	int scholarturns; /* to give warnings when trying to pray */

	/* KMH -- Info shared among gypsies */
	int umonteluck;		/* Chance of winning; lower is better */
	char umontelast;	/* Last monte card picked */

	int hangupamount;	/* amount of times you hanged up; used to weed out cheaters */

	int	weapon_slots;		/* unused skill slots */
	int	skills_advanced;		/* # of advances made so far */
	xchar	skill_record[P_SKILL_LIMIT];	/* skill advancements */
	struct skills weapon_skills[P_NUM_SKILLS];
	boolean twoweap;		/* KMH -- using two-weapon combat */

	boolean bellimbued;	/* player reached Bell Caves */
	boolean amuletimbued1;	/* player entered Forging Chamber */
	boolean amuletimbued2;	/* player entered Dead Grounds */
	boolean amuletimbued3;	/* player entered Ordered Chaos */
	boolean amuletcompletelyimbued;	/* player completed imbuing the amulet */

	boolean vibratingsquarefind;	/* player stepped on the VS for the first time */

	boolean wonderlandescape;	/* wonderland mode player made it to the portal */
	boolean zapemescape;	/* zapm mode player made it to the portal */

	boolean preversionmode;	/* decided at game start: true if you're a preversioner */
	boolean preversionescape;	/* preversion mode made it to deepest green cross level */

	d_level markedlevel;
	boolean levelmarked;

	int felidlives;	/* set to 9 at game start even if you're not a felid, because recursion is a thing */

	int homosexual;	/* starts at 0 = undefined; 1 = hetero, 2 = homo, defined when you have intercourse with a foocubus */

	boolean petcollectitems; /* variables for directive technique */
	int petattackenemies;
	boolean petcaneat;
	boolean petcanfollow;

	boolean segfaultpanic;	/* for segfaulter race */

	boolean annoyingmessages; /* don't show the annoying number pad message infinite times */

	int bizarrotries;	/* used to nerf the bizarro orgasmatron */

	boolean disablemartial;

	int hussyhurtturn;
	int hussykillturn;
	int hussyperfume;
	int handpulling;

	int irahapoison;

	int snaildigging;

	int acutraining;

	int adpartattack;	/* what AD_PART does in the given game */
	int adrunsattack;	/* what AD_RUNS does in the given game */

	int combostrike;	/* for technique */
	boolean comboactive;

	int dancercombostrike;
	boolean dancercomboactive;

	int dachacombostrike;
	boolean dachacomboactive;

	int steedhitchance;	/* default 25%, can be changed via directive */

	int bucskill;	/* how likely you are to recognize an item's BUC at a glance */
	int enchantrecskill;	/* ditto for the item's + (all except weapons) */
	int weapchantrecskill;	/* ditto for the item's + (weapons only) */

	int footererlevel;

	int ragnaroktimer;

	int usanity;

	int tarmustrokingturn;	/* for tarmustrokingnora trap */

	int frequentationtrait;	/* for frequentation spawn trap */

	int pokelieresistances;
	int pokeliegeneration;
	int pokelieflags;
	int pokeliespeed;
	int pokelieattacktype;
	int pokeliedamagetype;

	int manlerx, manlery;

	int smexyberries;

	boolean seesilverspell;

	boolean polyprotected;

	boolean wingyellowhack;
	boolean arabellahack;
	boolean linkmasterswordhack;
	boolean wollohhack;
	boolean polyattackhack;
	boolean dynamitehack;
	boolean detonationhack;
	int mongetshack;
	boolean evilvarhack;

	int secretcodenumber;

	boolean captchahack;

	boolean explodewandhack;

	int barbertimer;	/* for MS_BARBER service, can't use it too often */
	int barberamount;	/* amount of haircuts you got */
	int femalehaircut;
	int malehaircut;
	int malebeard;

	int areoladiameter;	/* for MS_AREOLA */

	int sokosolveboulder;
	int sokosolveuntrap;

	int artifactaffinity;	/* to check how much (or not) you use your quest artifact */
	int artifinitythreshold;	/* highest value reached */

	int elberethcheese;	/* if you use Elbereth, scare monster or temple sanctuary, increase this counter */

	int durirepaircost;
	int durienchantcost;
	int duriarticostevil;
	int duriarticostnormal;
	int durirequest;
	int duriworking;

	int cellargravate;

	int walscholarpass;

	boolean grenadoninlauncher;

	boolean mushroompoles;
	boolean mushroompoleused;

	boolean automorefuckthisshit;	/* gah */

	boolean dungeongrowthhack;	/* to circumvent goddamn savegame errors */

	boolean katitrapocc;	/* checking whether you have the occupation */
	boolean singtrapocc;
	boolean singtraptreaded;
	boolean singtraphighheel;
	boolean singtrapcowdung;

	boolean howtoenhanceskills;	/* the first time you can enhance a skill, the game tells you how */

	boolean roommatehack;	/* to allow monsters to spawn inside walls */

	boolean metalguard;	/* for metal guard spell */

	boolean coronationculmination;	/* for a certain artifact */

	boolean youhavememorized;	/* one-time message for memorization skill */
	boolean juyofleeing;	/* control whether your juyo skill increases the odds that a monster flees */
	boolean controlmiguc;	/* does the petkeeping skill make it less likely for you to hit your pets? */

	int garbagecleaned;	/* for janitor role */
	int garbagetrucktime;	/* ditto */

	int mojibakecount;	/* hack for pager.c */

	int swappositioncount;	/* for the swap position spell */
	int contingencyturns;	/* how long you have until contingency spell runs out */
	int horsehopturns;	/* how long you can use the horse hop spell effect */
	int ragnarokspelltimeout;	/* timer that counts down until you can use the spell again */
	int breathenhancetimer;	/* how long you have until enhance breath spell runs out */
	int bodyfluideffect;	/* how long you have until bodyfluid strengthening spell runs out */
	int antitelespelltimeout;	/* how long you have until anti-teleportation spell runs out */

	int tsloss_str;	/* temporarily lost stats, from e.g. AD_DEBU */
	int tsloss_dex;
	int tsloss_con;
	int tsloss_wis;
	int tsloss_int;
	int tsloss_cha;

	int lightsabermax1;	/* your highest value you ever had in a lightsaber form */
	int lightsabermax2;
	int lightsabermax3;
	int lightsabermax4;
	int lightsabermax5;
	int lightsabermax6;
	int lightsabermax7;
	int lightsabermax8;
	int lightsabermax9;
	int lightsabermax10;

	int pervertsex;	/* pervert needs to have sex often enough or get debuffs */
	int pervertpray;	/* same for praying (and yes, the gods can still get angry :P) */

	boolean fungalsandals;
	boolean castlecrushglitch;

	int gaugetimer;	/* one point shoot and particle cannon require 50 turns to reload */
	int battertimer;	/* battering ram requires 2 turns to reload */
	int combatcommand;	/* spell that temporarily gives +1 all stats */

	int conclusiocount;	/* to check whether Mr. Conclusio can spawn */

	int goodoffercount;	/* getting good effects from offering corpses reduces the odds of getting more of them */

	int persiantimer;	/* for persian boots */

	int stoogedepth;	/* occasionally the three stooges will spawn */

	int funnyhalluroll;	/* how much sanity you need to have in order to get funny hallu messages */

	boolean udrowning;	/* you fell in the water but didn't instadie */

	boolean zapem_mode;	/* decided at game start: true if you're a ZAPM role or used the ZAPM birth option */
	boolean riderhack;	/* to prevent you from easily removing riders */

	boolean freeplaymode;	/* set to 1 if you ascend and decide to go on playing */
	boolean freeplaytransit;	/* set to 1 if you're supposed to be transported back to dlvl1 and hang up or so */
	boolean freeplayplanes;	/* set to 1 once you visit the sanctum and thereby re-enable the ability to visit the planes */

	boolean antjetwotriggered;	/* if you ever trigger the antje trap 2, having it time out teaches a technique */

	boolean havebeeninludios;

	int gudrunspawncount;	/* for gudrun trap */
	int ellaspawncount;	/* for ella trap */
	int larissatimer;	/* for larissa trap */

	int badfcursed;	/* another long-lasting punishment for fountain quaffers */
	int badfdoomed;

	int hiddenpowerskill;	/* reaching master in this particular skill unlocks hidden power */

	int inasuppression;	/* dying and lifesaving while having the ina trap active will set this */

	int hunkturns;	/* inofficial lightsaber forms that don't appear as regular skills */
	int hunkskill;
	int kliuturns;
	int kliuskill;
	int aldebaranturns;
	int aldebaranskill;
	int polgoturns;
	int polgoskill;
	int firgaturns;
	int firgaskill;

	int nataliacyclestart;	/* for natalia trap: starts at rn2(2800) */
	int nataliacycletimer;
	int nataliafollicularend;	/* when does the follicular phase end? 1200 + rn2(401) turns */
	int natalialutealstart;	/* when does the luteal phase start? 200 + rn2(401) turns */

	boolean keythief;	/* did the player use certain keys on Vlad's? if yes, give a reward once */
	boolean keynocturn;
	boolean keyaccess;
	boolean keygauntlet;

	/* extra statistics: some of these display with #conduct, others only at the end of your game --Amy */
	int cnd_applycount;	/* how often you applied items */
	int cnd_unihorncount;	/* how often you benefitted from a unicorn horn effect */
	int cnd_markercount;	/* how many items you created with magic markers */
	int cnd_chemistrycount;	/* how many items you created with the chemistry set */
	int cnd_quaffcount;	/* how many potions you quaffed */
	int cnd_zapcount;		/* how many wands you zapped */
	int cnd_scrollcount;	/* how many scrolls you read */
	int cnd_spellbookcount;	/* how many spellbooks you read successfully */
	int cnd_spellcastcount;	/* how many spells you successfully cast */
	int cnd_spellfailcount;	/* how many times you failed to cast a spell */
	int cnd_forgottenspellcount;	/* how many times you cast a forgotten spell */
	int cnd_invokecount;	/* how many times you invoked an artifact that had an actual invoke effect */
	int cnd_techcount;	/* how many times you used a technique that had a nonzero timeout */
	int cnd_phasedoorcount;	/* how many times you phase doored */
	int cnd_teleportcount;	/* how many times you teleported */
	int cnd_telelevelcount;	/* how many times you levelported */
	int cnd_branchportcount;	/* how many times you branchported */
	int cnd_banishmentcount;	/* how many times you were banished */
	int cnd_punishmentcount;	/* how many times you got punished */
	int cnd_petdeathcount;	/* how many of your pets died */
	int cnd_amnesiacount;	/* how many times you suffered from amnesia */
	int cnd_maxalignment;	/* the maximum positive alignment you ever had */
	int cnd_minalignment;	/* the minimum negative alignment you ever had */
	int cnd_maxsanity;	/* the maximum amount of sanity you ever had */
	int cnd_maxcontamination;	/* the maximum amount of contamination you ever had */
	int cnd_searchtrapcount;	/* how many traps you found via searching */
	int cnd_searchsecretcount;	/* how many secret doors and corridors you found via searching */
	int cnd_fartingcount;	/* how many times you heard farting noises */
	int cnd_crappingcount;	/* how many times you heard crapping noises */
	int cnd_conversioncount;	/* how many times you were subjected to converting taunts */
	int cnd_wouwoucount;	/* how many times you heard wouwou taunts */
	int cnd_perfumecount;	/* how many times you inhaled perfume */
	int cnd_supermancount;	/* how many times you heard superman taunts */
	int cnd_nutkickamount;	/* how many times you were kicked in the nuts */
	int cnd_breastripamount;	/* how many times your breasts were violated */
	int cnd_saveamount;	/* how many times you saved the game */
	int cnd_ragnarokamount;	/* how many times ragnarok was caused */
	int cnd_datadeleteamount;	/* how many times your data got deleted */
	int cnd_curseitemsamount;	/* how many times you got hit with the curse items effect */
	int cnd_nastytrapamount;	/* how many times you triggered a nasty trap */
	int cnd_feminismtrapamount;	/* how many times you triggered a feminism trap */
	int cnd_plineamount;	/* how many random plines you read */
	int cnd_aggravateamount;	/* how many times monsters came out of portals */
	int cnd_eatrinsicamount;	/* how often you got intrinsics from eating corpses */
	int cnd_shkserviceamount;	/* how many times you purchased shopkeeper services */
	int cnd_kopsummonamount;	/* how often shopkeepers, watchmen etc. called the kops */
	int cnd_captchaamount;	/* how many captchas you solved */
	int cnd_captchafail;	/* how many times you failed to solve a captcha */
	int cnd_quizamount;	/* how many quiz questions you answered correctly */
	int cnd_quizfail;		/* how many times you failed to answer a quiz question */
	int cnd_alterrealityamount;	/* how many times reality was altered */
	int cnd_unlockamount;	/* how many times you unlocked something with an unlocking tool */
	int cnd_altarconvertamount;	/* how many times you converted an altar to another deity */
	int cnd_itemstealamount;	/* how many times your items were stolen */
	int cnd_poisonamount;	/* how many times you were poisoned */
	int cnd_nursehealamount;	/* how many extra HP you got from nurse dancing */
	int cnd_nurseserviceamount;	/* how many times you purchased nurse services */
	int cnd_elberethamount;	/* how many times an Elbereth engraving scared a monster */
	int cnd_disenchantamount;	/* how many times your items got disenchanted */
	int cnd_permstatdamageamount;	/* how many times your stats got permanently damaged */
	int cnd_shoedamageamount;	/* how many times you got hurt with female shoes */
	int cnd_farmlandremoved;	/* how many farmland tiles you removed via #force */
	int cnd_nethermistremoved;	/* how many nether mist tiles you dissipated by killing lawful monsters */
	int cnd_ammomulched;	/* how many of your ranged weapon ammos (excluding bullets) were mulched */
	int cnd_gunpowderused;	/* how many bullets, grenades etc. you used */
	int cnd_chargingcount;	/* how often you recharged items */
	int cnd_offercount;	/* how many corpses you offered to the gods */
	int cnd_forcecount;	/* how many times you used #force on a monster */
	int cnd_forcebuttcount;	/* how often you forced a farting monster */
	int cnd_kickmonstercount;	/* how often you kicked a monster */
	int cnd_kicklockcount;	/* how often you kicked doors, containers etc. */
	int cnd_fumbled;	/* how many times you fumbled */
	int cnd_wandwresting;	/* how many wands you wrested */
	int cnd_firedestroy;	/* how many of your items were destroyed by fire */
	int cnd_colddestroy;	/* how many of your items were destroyed by cold */
	int cnd_shockdestroy;	/* how many of your items were destroyed by shock */
	int cnd_poisondestroy;	/* how many of your items were destroyed by poison */
	int cnd_diggingamount;	/* how many walls or holes you dug */
	int cnd_gravewallamount;	/* how many grave walls you dug out */
	int cnd_treechopamount;	/* how many trees you chopped down */
	int cnd_barbashamount;	/* how many iron bars you bashed down */
	int cnd_fountainamount;	/* how many times you quaffed or dipped a fountain */
	int cnd_throneamount;	/* how many times you sat on a throne */
	int cnd_sinkamount;	/* how many times you quaffed, dipped or dropped rings down a sink */
	int cnd_toiletamount;	/* how many times you quaffed, sit, dipped or dropped amulets down a toilet */
	int cnd_pentagramamount;	/* how many times you invoked a pentagram */
	int cnd_wellamount;	/* how many times you quaffed from a well */
	int cnd_poisonedwellamount;	/* how many times you quaffed from a poisoned well */
	int cnd_bedamount;	/* how many times you slept in a bed */
	int cnd_mattressamount;	/* how many times you slept on a mattress */
	int cnd_criticalcount;	/* how many critical hits you landed */
	int cnd_stealamount;	/* how many $ worth of items you stole */
	int cnd_monsterdigamount;	/* how many tiles were dug out by monsters */
	int cnd_untrapamount;	/* how many times you untrapped a trap successfully */
	int cnd_longingamount;	/* how many longing messages you had to read */
	int cnd_symbiotesdied;	/* how many of your symbiotes died */
	int cnd_sickfoodpois;	/* how many times you got sick from food poisoning */
	int cnd_sickillness;	/* how many times you got sick from illness */
	int cnd_slimingcount;	/* how many times you got slimed */
	int cnd_stoningcount;	/* how many times you got stoned */
	int cnd_polybreak;	/* how many of your armor pieces broke due to bad polymorphs */
	int cnd_monpotioncount;	/* how many potions were quaffed by monsters */
	int cnd_monscrollcount;	/* how many scrolls were read by monsters */
	int cnd_monwandcount;	/* how many wands were zapped by monsters */
	int cnd_demongates;	/* how often monsters gated in demons */
	int cnd_demonlordgates;	/* how many demon lords got gated in */
	int cnd_demonprincegates;	/* how many demon princes got gated in */
	int cnd_artiblastcount;	/* how often you were blasted by an artifact */
	int cnd_moneatmetal;	/* how many items were eaten by metallivorous monsters */
	int cnd_moneatstone;	/* how many items were eaten by lithivorous monsters */
	int cnd_moneatorganic;	/* how many items were eaten by organivorous monsters */
	int cnd_moneatall;	/* how many items were eaten by allivorous monsters */
	int cnd_unidentifycount;	/* how many times your items unidentified themselves */
	int cnd_dehydratedcount;	/* how many times you were dehydrated */
	int cnd_luckrollovercount;	/* how many times your luck rolled over */
	int cnd_confusedscrollread;	/* how many times you got the confused scroll effect */
	int cnd_nonworkpotioncount;	/* how many times your potions didn't work */
	int cnd_stairstrapcount;	/* how many stairs traps you encountered */
	int cnd_intrinsiclosscount;	/* how many intrinsics you lost */
	int cnd_traprevealcount;	/* how many traps you revealed magically */
	int cnd_badeffectcount;	/* how often you got bad effects */
	int cnd_reallybadeffectcount;	/* how often you got really bad effects */
	int cnd_goodeffectcount;	/* how often you got good effects */
	int cnd_itemportcount;	/* how many of your items were teleported away */
	int cnd_bonescount;	/* how many bones levels you found */
	int cnd_mommacount;	/* how often your momma was insulted */
	int cnd_badequipcount;	/* how often you were forced to wear bad equipment */
	int cnd_badarticount;	/* how often you were forced to wear bad artifacts */
	int cnd_badheelcount;	/* how often you were forced to wear bad high heels */
	int cnd_appearanceshufflingcount;	/* how often the randomized item appearances got shuffled */
	int cnd_mysteriousforcecount;	/* how often you got hit by the mysterious force */
	int cnd_manlergetcount;	/* now often the manler caught you */
	int cnd_wandbreakcount;	/* how many wands you broke */
	int cnd_alchemycount;	/* how often you did alchemy */
	int cnd_weaponbreakcount;	/* how many opponents' weapons you broke/cut */
	int cnd_newmancount;	/* how often you felt like a new man */
	int cnd_eggcount;	/* how many eggs hatched */
	int cnd_wipecount;	/* how often you successfully use the #wipe command */
	int cnd_acquirementcount;	/* how often you use acquirement */
	int cnd_cannibalcount;	/* how often you ate your own race without permission (i.e. caveman doesn't count) */
	int cnd_ringtrinsiccount;	/* how often you actually gain effects from eating jewelry */
	int cnd_potionthrowyoucount;	/* how many potions have been thrown by you */
	int cnd_potionthrowmoncount;	/* how many potions have been thrown by monsters */
	int cnd_overlevelcount;	/* how many times you leveled past XL30 */
	int cnd_vomitingcount;	/* how many times you vomited */
	int cnd_cwnannwncount;	/* how many times you were low on health */
	int cnd_bansheecount;	/* how many times you were down to your last hit point */
	int cnd_lycanthropecount;	/* how many times you contracted lycanthropy */
	int cnd_blackjackwins;	/* how many times you won the blackjack minigame */
	int cnd_blackjackdealercheat;	/* how many times the blackjack dealer cheated */
	int cnd_covideffect;	/* how many times you suffered from covid symptoms */
	int cnd_covidantidote;	/* how many times you received the cure for covid */
	int cnd_socksmell;	/* for MS_SOCKS */
	int cnd_pantsmell;	/* for MS_PANTS */
	int cnd_photo_op;	/* for MS_PHOTO */
	int cnd_weapondull;	/* how many times your weapons dulled */
	int cnd_armordull;	/* how many times your armor pieces dulled */
	int cnd_symbiotekills;	/* how many times your symbiote killed an enemy */
	int cnd_singhelped;	/* how many pairs of shoes you cleaned for Sing */
	int cnd_singrefused;	/* how many times you refused to clear a pair of shoes for Sing */
	int cnd_trophiesobtained;	/* how many trophies you scored */

	struct symbiotemon usymbiote;	/* for the symbiosis skill */
	int symbioteaggressivity;	/* how often your symbiote attacks or takes damage */
	boolean symbiotedmghack;	/* make something damage you even if you have a symbiote */
	int shutdowntime;	/* for nurse's shutdown symbiote service */

	/* practicant role: has to pay a fine sometimes */
	int practicantpenalty;	/* how many zorkmids you have to pay */
	int practicanttime;	/* how much time you have until you need to have paid */
	int practicantseverity;	/* gets higher the more often you refuse to pay */
	int practicantstones;	/* do you have to pay in rocks? if so, set this variable */
	int practicantarrows;	/* do you have to pay in arrows? if so, set this variable */
	int practicantcash;	/* how many zorkmids your nemesis has collected - this decreases over time because she spends the money on clothing, perfume and expensive crap, the bitch :P when you kill her, you get back the amount she didn't spend yet */

	/* individual fines for practicant: <crime> = <amount to pay> */
	boolean pract_toomanykills;	/* killed 1000 monsters = 1000 zorkmids */
	boolean pract_toomanykills2;	/* killed 2000 monsters = 2000 zorkmids */
	boolean pract_toomanykills3;	/* killed 3000 monsters = 4000 zorkmids */
	boolean pract_toomanykills4;	/* killed 4000 monsters = pay 1000 zorkmids once every 5000 turns */
	int pract_conv1timer;	/* timer for the above */
	boolean pract_toomuchmoney;	/* carried 10k zorkmids out in the open = 5000 zorkmids */
	boolean pract_toomanysacs;	/* sacrificed 10 corpses = 1000 zorkmids */
	boolean pract_toomanysacs2;	/* sacrificed 20 corpses = 2000 zorkmids */
	boolean pract_toomanysacs3;	/* sacrificed 30 corpses = pay 1000 zorkmids once every 5000 turns */
	int pract_conv2timer;	/* timer for the above */
	boolean pract_altarconvert;	/* converted an altar = 2000 zorkmids */
	boolean pract_enchantarmor;	/* enchant an armor piece to +5 or higher = 1000 zorkmids and 1000 stones */
	boolean pract_toomucharmor;	/* wear armor pieces in all seven slots with all of them at least +2 = 3000 zorkmids, and disenchant all those armor pieces by one point each */
	boolean pract_bosskill;	/* killed a boss monster (other than the nemesis) = 10000 zorkmids */
	boolean pract_mrstone;	/* obtained stone of magic resistance = 6000 zorkmids */
	boolean pract_wow;	/* tried to zap a wand of wishing = nothing happens and the wand is set to 1:-1 */
	boolean pract_altartest;	/* tested 10 items on altars = amnesia */
	int pract_altartestamount;	/* so we can count those 10 items */
	boolean pract_fastform;	/* ride or be polyd into speed 25 or faster monster = frenzied trebuchet dragon spawns */
	boolean pract_fastregen;	/* have both intrinsic and extrinsic regen at the same time = intrinsic blood loss */
	boolean pract_toomuchrange;	/* wield a sniper rifle = pay all money you have out in the open or in containers */
	boolean pract_enhancing;	/* enhanced at least 30 times = lose 10 random intrinsics */
	int pract_enhanceamount;	/* so we can count those 30 times */
	boolean pract_shopkeepers;	/* killed at least 5 shopkeepers = 8000 zorkmids */
	int pract_shopkeepercount;	/* so we can count them */
	boolean pract_peacedisturb;	/* killed at least 20 shopkeepers, watchmen, temple priests or guards = pay 2000 zorkmids once every 5000 turns */
	int pract_disturbcount;	/* so we can count them */
	int pract_conv3timer;	/* timer for the above */
	/* any shoplifting also gives 2x the value of stolen goods as fine */
	boolean pract_gottoonear;	/* spent at least 500 turns on the quest goal level = 50 zorkmids */
	int pract_tooneartimer;	/* timer for the above */
	boolean pract_gottoonear2;	/* spent at least 1000 turns on the quest goal level = 100 zorkmids */
	boolean pract_gottoonear3;	/* spent at least 1500 turns on the quest goal level = 200 zorkmids */
	boolean pract_gottoonear4;	/* spent at least 2000 turns on the quest goal level = 1000 zorkmids */
	boolean pract_idling;	/* issued a wait command that takes at least 50 turns = 400 zorkmids */
	int pract_idlingtimer;	/* the above can happen again after 1k turns have passed */
	boolean pract_waterfreeze;	/* froze at least 5 water tiles = 20 arrows and 1000 zorkmids */
	int pract_frozentiles;	/* so we can count them */
	boolean pract_oremining;	/* used the ore mining spell to raze a stalactite = 1000 zorkmids */
	boolean pract_forbiddenskill;	/* be at least basic in two-handed sword or pick-axe = 2000 zorkmids, and reduce player's nutrition by 2000 */
	boolean pract_espionage;	/* have in/extrinsic detect monsters = 20000 zorkmids */
	boolean pract_magicresistance;	/* have intrinsic magic resistance = 2000 zorkmids every 5000 turns */
	int pract_conv4timer;	/* timer for the above */
	boolean pract_trespassing;	/* be met by the nemesis (see quest.txt) = 2000 zorkmids */
	boolean pract_trespassing2;	/* be met by the nemesis again (see quest.txt) = 4000 zorkmids; every future occurrence of the relevant quest text is another 5000 */
	boolean pract_artitheft;	/* nemesis finds out that you stole the quest artifact = 50000 zorkmids */
	boolean pract_nofines;	/* didn't get any fine in 10000 turns = 1000 zorkmids */
	int pract_finetimer;	/* timer for the above */
	boolean pract_bullets;	/* 100 bullets fired = 2000 zorkmids */
	boolean pract_bullets2;	/* 200 bullets fired = 4000 zorkmids */
	boolean pract_bullets3;	/* 300 bullets fired = 8000 zorkmids and rnz(5000) turns of low skill */
	boolean pract_heavymg;	/* equip a heavy MG = 1000 zorkmids and 1000 stones */
	boolean pract_wodzap;	/* zap wand of death = 10000 zorkmids */
	boolean pract_fodzap;	/* cast finger of death = 25000 zorkmids and rnz(5000) turns of muteness */
	boolean pract_oilspill;	/* dilute oil in a pool = 10000 zorkmids and +1000 long-lasting wight */
	boolean pract_fatalcontamination;	/* be fatally contaminated = 10000 zorkmids and disables poison resistance for 20000 turns */
	boolean pract_sanity;	/* have over 900 sanity = 2000 zorkmids */
	boolean pract_sanity2;	/* have over 9000 sanity = 20000 zorkmids */
	boolean pract_lowscore;	/* less than 10k score after 10k turns = 2000 zorkmids */
	boolean pract_lotsofhp;	/* have over 500 max HP = monster spawns with scroll of offlevel item and levelports */
	boolean pract_lotsofmp;	/* have over 500 max Pw = ragnarok will happen eventually */
	boolean pract_procrastinate;	/* if the nemesis is still not dead by a certain point, ragnarok happens */
	int pract_procrastinatetimer;	/* timer for the above */
	boolean pract_expertguncontrol;	/* have gun control on expert = can decide whether to lose all skills or spawn a boss */
	boolean pract_void;	/* entered the void = 5 turns to leave, otherwise kops and demon lords can spawn */
	int pract_voidtimer;	/* turn at which you may no longer be in the void */
	boolean pract_voidinitial;	/* whether the initial spawn of badguys for the above has happened */
	int pract_smokingtimer;	/* whenever you use a cigarette or similar, timer for being penalized */
	boolean pract_cwnannwn;	/* if you hear the cwnannwn or banshee too often, every additional time results in fines */

	/* killing the nemesis would result in another fine but she's dead at that point so you no longer care :P */

};	/* end of `struct you' */

/* is the player in symbiosis? make extra sure the monster is set up correctly --Amy */
#define uinsymbiosis	(u.usymbiote.active && u.usymbiote.mnum > PM_PLAYERMON && u.usymbiote.mnum < NUMMONS)
#define uactivesymbiosis	(u.usymbiote.active && !u.shutdowntime && u.usymbiote.mnum > PM_PLAYERMON && u.usymbiote.mnum < NUMMONS)

/* is the player a practicant who has to pay fines? (only the case when nemesis is still alive) */
#define practicantterror	((Role_if(PM_PRACTICANT) || isbeacher) && !quest_status.killed_nemesis)

#define Upolyd (u.umonnum != u.umonster)


#endif	/* YOU_H */
