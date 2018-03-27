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
	short malenum,		/* index (PM_) as a male (botl.c) */
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
	short malenum,		/* PM_ as a male monster */
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
#define uhe()	(genders[flags.female ? 1 : 0].he)
#define uhim()	(genders[flags.female ? 1 : 0].him)
#define uhis()	(genders[flags.female ? 1 : 0].his)
#define mhe(mtmp)	(genders[pronoun_gender(mtmp)].he)
#define mhim(mtmp)	(genders[pronoun_gender(mtmp)].him)
#define mhis(mtmp)	(genders[pronoun_gender(mtmp)].his)


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
#define Luck	(flags.dudley ? -13 : issuxxor ? -13 : (uleft && uleft->otyp == RIN_DOOM) ? -13 : (uright && uright->otyp == RIN_DOOM) ? -13 : (Role_if(PM_FAILED_EXISTENCE) && ((u.uluck + u.moreluck + Role_if(PM_AUGURER) ) > 0) ) ? 0 : (u.uluck + u.moreluck + Role_if(PM_AUGURER) )) /* credits to the Dudley's Dungeon guys --Amy */
#define LUCKADD		3	/* added value when carrying luck stone */
#define LUCKMAX		10	/* on moonlit nights 11 */
#define LUCKMIN		(-10)
	schar	uhitinc;		/* KMH -- additional to-hit bonus */
	int uhitincxtra;
	long	uhealbonus;		/* KMH -- Healing bonus from healthstones */

	schar	udaminc;		/* Additional damage bonus */
	int udamincxtra;
	schar	uac;

	uchar	uspellprot;		/* protection by SPE_PROTECTION */
	uchar	usptime;		/* #moves until uspellprot-- */
	uchar	uspmtime;		/* #moves between uspellprot-- */

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

	int urmaxlvl;		/* for asgardian race */
	int urmaxlvlB;		/* for cyborg role */
	int urmaxlvlC;		/* for binder role */
	int urmaxlvlD;		/* for bard role */
	int urmaxlvlE;		/* for american race */
	int urmaxlvlF;		/* for wild talent role */
	int urmaxlvlG;		/* for wild talent role */
	int urmaxlvlH;		/* for mystic role */
	int urmaxlvlI;		/* for DQ slime combined with green slime race */
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

	int menoraget;
	int bookofthedeadget;
	int silverbellget;
	int chaoskeyget;
	int neutralkeyget;
	int lawfulkeyget;
	int luckstoneget;
	int sokobanfinished;
	int medusaremoved;

	int musableremovechance;	/* extra chance for musable items to get removed */
	int equipmentremovechance;

	int bookspawnchance;	/* lower spawn chance for certain item classes */
	int scrollspawnchance;
	int potionspawnchance;
	int ringspawnchance;
	int wandspawnchance;
	int amuletspawnchance;

	int concealitemchance;	/* chance for concealing monsters to start with zorkmids instead */

	int usefulitemchance; /* lower spawn chance for miscellaneous useful items */

	int antimusablebias; /* to make it less likely that random scrolls, potions or wands are musable */

	int invisotrap;	/* evil patch idea - trap that's always generated invisible */

	int frequenttrap;		/* evil patch idea by Amy - trap that is generated more often */
	int freqtrapbonus;	/* frequency bonus - the actual formula of how this affects generation odds is complicated */

	int shaperoomchance;	/* to control the chance of irregular rooms */

	int fumbleduration;	/* for wand of fumbling */
	int antimagicshell;	/* for scroll of antimagic */

	boolean artifactprotection;	/* because NHTNG's protection code is faulty */

	/* Extra nastiness - currently only nastinator race, but might add more functionality someday --Amy */
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

	int randomquestlevels;	/* chance to get quest levels instead of random ones */

	int hangupcheat;		/* for cheating players who think hanging up is cool */
	int hanguppenalty;
	int hangupparalysis;	/* saved paralysis counter so you *really* cannot cheat */

	int youaredead;		/* if you hang up at a prompt before an instadeath, i.e. sickness */

	int negativeprotection;	/* AD_NPRO - deduct this amount from the player's AC */
	int chokhmahdamage;	/* AD_CHKH - damage increases by 1 with every hit */
	int extralives;		/* for scroll of resurrection */

	int banishmentbeam;	/* an attempt to make wands of banishment used by monsters *not* segfault :P --Amy */
	int levelporting;		/* and nexus/weeping attacks should not segfault either */

	int inertia;	/* for inertia attack - if non-zero, the player moves at half speed */

	int bankcashamount; /* for bank trap */
	int bankcashlimit;

	int egglayingtimeout;	/* otherwise you could just spam them, which would be unbalanced! */

	int dragonpolymorphtime;	/* elemental mages shall not be able to polymorph indefinitely, sorry ziratha */

	int dehydrationtime;

	int fakeartifacts;	/* keeping track of how many fake artifacts got generated */

	int impossibleproperty;	/* evil patch idea by jonadab - a property that you simply cannot get in this game */
	int nonextrinsicproperty;
	int nonintrinsicproperty;

	boolean temprecursion; /* temporary recursion trap */
	int oldrecursionrole;
	int oldrecursionrace;
	int temprecursiontime;

	boolean totter;	/* scroll of symmetry toggles this */

	int nurseextracost;	/* for chat-to-nurses functionality */
	int nursedecontamcost;	/* ditto */

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
	int ugeneralcombatturns;
	int utwohandedcombatturns;
	int ubodyarmorturns;
	int umissileweaponturns;
	int	umortality;		/* how many times you died */
	int ugrave_arise; /* you die and become something aside from a ghost */
	time_t	ubirthday;		/* real world time when game began */

	int untrainableskill; /* you can't train all skills equally well (evil patch idea) */
	int halfspeedskill;
	int fifthspeedskill;
	int basiclimitskill;
	int skilledlimitskill;
	int expertlimitskill;
	int earlytrainingskill;
	int earlytrainingtimer;
	boolean earlytrainingblown;

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

	int berserktime;
	int magicshield;
	int burrowed;
	int stasistime;
	int powerfailure;
	int tunnelized;
	int nailpolish;
	int enchantspell;
	int thornspell;

	int halresdeactivated;

	int pinkspelldirection;
	int trontrapdirection;
	int trontrapturn;

	boolean polyformed;

	boolean boomshined;
	boolean kyliemode;

	boolean aggravation;

	boolean outtadepthtrap;

	int riennevaplus;

	int stairscumslowing;

	int nataljetrapturns;
	int nataljetrapx, nataljetrapy;

	int contamination;	/* weeping angel, but also other effects */

	int copwantedlevel;	/* for hot pursuits, i.e. when you've stolen from a shop */

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

	int felidlives;	/* set to 9 at game start even if you're not a felid, because recursion is a thing */

	int homosexual;	/* starts at 0 = undefined; 1 = hetero, 2 = homo, defined when you have intercourse with a foocubus */

};	/* end of `struct you' */

#define Upolyd (u.umonnum != u.umonster)


#endif	/* YOU_H */
