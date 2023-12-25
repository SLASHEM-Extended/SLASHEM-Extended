/*	SCCS Id: @(#)hack.h	3.4	2001/04/12	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef HACK_H
#define HACK_H

#ifndef CONFIG_H
#include "config.h"
#endif

/*	For debugging beta code.	*/
#ifdef BETA
#define Dpline	pline
#endif

#define TELL		1
#define NOTELL		0
#define ON		1
#define OFF		0
#define BOLT_LIM    6	/* from this distance ranged attacks will be made */
/*#define MAX_CARR_CAP	5000*/	/* so that boulders can be heavier; changed to max_carr_cap() in hack.c --Amy */
#define DUMMY { 0 }

/* symbolic names for capacity levels */
#define UNENCUMBERED	0
#define SLT_ENCUMBER	1	/* Burdened */
#define MOD_ENCUMBER	2	/* Stressed */
#define HVY_ENCUMBER	3	/* Strained */
#define EXT_ENCUMBER	4	/* Overtaxed */
#define OVERLOADED	5	/* Overloaded */

/* Macros for how a rumor was delivered in outrumor() */
#define BY_ORACLE	0
#define BY_COOKIE	1
#define BY_PAPER	2
#define BY_OTHER	9

/* Macros for why you are no longer riding */
#define DISMOUNT_GENERIC	0
#define DISMOUNT_FELL		1
#define DISMOUNT_THROWN		2
#define DISMOUNT_POLY		3
#define DISMOUNT_ENGULFED	4
#define DISMOUNT_BONES		5
#define DISMOUNT_BYCHOICE	6

/* Special returns from mapglyph() */
#define MG_CORPSE	0x001
#define MG_INVIS	0x002
#define MG_DETECT	0x004
#define MG_PET		0x008
#define MG_RIDDEN	0x010
#define MG_STAIRS	0x020
#define MG_OBJPILE	0x040
#define MG_EGOTYPE	0x080
#define MG_PEACEFUL	0x100
#define MG_ENGRAVING	0x200

/* sellobj_state() states */
#define SELL_NORMAL	(0)
#define SELL_DELIBERATE	(1)
#define SELL_DONTSELL	(2)

/*
 * This is the way the game ends.  If these are rearranged, the arrays
 * in end.c and topten.c will need to be changed.  Some parts of the
 * code assume that PANIC separates the deaths from the non-deaths.
 */
#define DIED		 0
#define BETRAYED	 1
#define CHOKING		 2
#define POISONING	 3
#define STARVING	 4
#define DROWNING	 5
#define BURNING		 6
#define DISSOLVED	 7
#define CRUSHING	 8
#define STONING		 9
#define TURNED_SLIME 10
#define GENOCIDED	11
#define PANICKED	12
#define TRICKED		13
#define QUIT		14
#define ESCAPED		15
#define ASCENDED	16

#include "align.h"
#include "dungeon.h"
#include "monsym.h"
#include "mkroom.h"
#include "objclass.h"
#include "youprop.h"
#include "wintype.h"
#include "decl.h"
#include "timeout.h"

NEARDATA extern coord bhitpos;	/* place where throw or zap hits or stops */

/* types of calls to bhit() */
#define ZAPPED_WAND	0
#define THROWN_WEAPON	1
#define KICKED_WEAPON	2
#define FLASHED_LIGHT	3
#define INVIS_BEAM	4

#define MATCH_WARN_OF_MON(mon)	 (Warn_of_mon && flags.warntype && \
		   		 (flags.warntype & (mon)->data->mflags2))

#include "trap.h"
#include "flag.h"
#include "rm.h"
#include "vision.h"
#include "display.h"
#include "engrave.h"
#include "rect.h"
#include "region.h"

#include "extern.h"
#include "winprocs.h"

#ifdef USE_TRAMPOLI
#include "wintty.h"
#undef WINTTY_H
#include "trampoli.h"
#undef EXTERN_H
#include "extern.h"
#endif /* USE_TRAMPOLI */

#define NO_SPELL	0

/* flags to control makemon() */
#define NO_MM_FLAGS	  0x0000	/* use this rather than plain 0 */
#define NO_MINVENT	  0x0001	/* suppress minvent when creating mon */
#define MM_NOWAIT	  0x0002	/* don't set STRAT_WAITMASK flags */
#define MM_EDOG		  0x0004	/* add edog structure */
#define MM_EMIN		  0x0008	/* add emin structure */
#define MM_ANGRY	  0x0010  /* monster is created angry */
#define MM_NONAME	  0x0020  /* monster is not christened */
#define MM_NOCOUNTBIRTH	  0x0040  /* don't increment born counter (for revival) */
#define MM_IGNOREWATER	  0x0080	/* ignore water when positioning */
#define MM_ADJACENTOK	  0x0100 /* it is acceptable to use adjacent coordinates */
#define MM_NOSPECIALS     0x0200 /* no cockatrice corpses etc. */
#define MM_CRYSTALORNOT   0x0400 /* for if the player falls into water, to see whether they can crawl out */
#define MM_FRENZIED	0x0800	/* monster has 1 in 3 chance of spawning frenzied */
#define MM_XFRENZIED	0x1000	/* monster always spawns frenzied */
#define MM_MAYSLEEP	0x2000	/* monster has 20% chance of spawning asleep */
#define MM_REVIVED	0x4000	/* to ensure that MS_DEAD don't immediately die again */

/* special mhpmax value when loading bones monster to flag as extinct or genocided */
#define DEFUNCT_MONSTER	(-100)

/* flags for special ggetobj status returns */
#define ALL_FINISHED	  0x01  /* called routine already finished the job */

/* flags to control query_objlist() */
#define BY_NEXTHERE	  0x1	/* follow objlist by nexthere field */
#define AUTOSELECT_SINGLE 0x2	/* if only 1 object, don't ask */
#define USE_INVLET	  0x4	/* use object's invlet */
#define INVORDER_SORT	  0x8	/* sort objects by packorder */
#define SIGNAL_NOMENU	  0x10	/* return -1 rather than 0 if none allowed */
#define FEEL_COCKATRICE   0x20  /* engage cockatrice checks and react */
#define SIGNAL_CANCEL	  0x40	/* return -4 rather than 0 if explicit cancel */

/* Flags to control query_category() */
/* BY_NEXTHERE used by query_category() too, so skip 0x01 */
#define UNPAID_TYPES 0x02
#define GOLD_TYPES   0x04
#define WORN_TYPES   0x08
#define ALL_TYPES    0x10
#define BILLED_TYPES 0x20
#define CHOOSE_ALL   0x40
#define BUC_BLESSED  0x80
#define BUC_CURSED   0x100
#define BUC_UNCURSED 0x200
#define BUC_UNKNOWN  0x400
#define NOTFULLYIDED  0x800
#define BUC_ALLBKNOWN (BUC_BLESSED|BUC_CURSED|BUC_UNCURSED)
#define ALL_TYPES_SELECTED -2

/* Flags to control find_mid() */
#define FM_FMON	       0x01	/* search the fmon chain */
#define FM_MIGRATE     0x02	/* search the migrating monster chain */
#define FM_MYDOGS      0x04	/* search mydogs */
#define FM_EVERYWHERE  (FM_FMON | FM_MIGRATE | FM_MYDOGS)

/* Flags to control pick_[race,role,gend,align] routines in role.c */
#define PICK_RANDOM	0
#define PICK_RIGID	1

/* Flags to control dotrap() in trap.c */
#define NOWEBMSG	0x01	/* suppress stumble into web message */
#define FORCEBUNGLE	0x02	/* adjustments appropriate for bungling */
#define RECURSIVETRAP	0x04	/* trap changed into another type this same turn */
#define DONTREVEAL	0x08	/* trap won't become visible */
#define SKIPGARBAGE	0x10	/* skip stuff like ka-blammo, jana's fairness cup etc. */

/* Flags to control test_move in hack.c */
#define DO_MOVE		0	/* really doing the move */
#define TEST_MOVE	1	/* test a normal move (move there next) */
#define TEST_TRAV	2	/* test a future travel location */

/*** some utility macros ***/
#define yn(query) yn_function(query,ynchars, 'n')
#define ynq(query) yn_function(query,ynqchars, 'q')
#define ynaq(query) yn_function(query,ynaqchars, 'y')
#define nyaq(query) yn_function(query,ynaqchars, 'n')
#define nyNaq(query) yn_function(query,ynNaqchars, 'n')
#define ynNaq(query) yn_function(query,ynNaqchars, 'y')

/* Macros for scatter */
#define VIS_EFFECTS	0x01	/* display visual effects */
#define MAY_HITMON	0x02	/* objects may hit monsters */
#define MAY_HITYOU	0x04	/* objects may hit you */
#define MAY_HIT		(MAY_HITMON|MAY_HITYOU)
#define MAY_DESTROY	0x08	/* objects may be destroyed at random */
#define MAY_FRACTURE	0x10	/* boulders & statues may fracture */

/* Macros for launching objects */
#define ROLL		0x01	/* the object is rolling */
#define FLING		0x02	/* the object is flying thru the air */
#define LAUNCH_UNSEEN	0x40	/* hero neither caused nor saw it */
#define LAUNCH_KNOWN	0x80	/* the hero caused this by explicit action */

/* Macros for explosion types */
#define EXPL_DARK	0
#define EXPL_NOXIOUS	1
#define EXPL_MUDDY	2
#define EXPL_WET	3
#define EXPL_MAGICAL	4
#define EXPL_FIERY	5
#define EXPL_FROSTY	6
#define EXPL_MAX	7

/* Macros for messages referring to hands, eyes, feet, etc... */
#define ARM 0
#define EYE 1
#define FACE 2
#define FINGER 3
#define FINGERTIP 4
#define FOOT 5
#define HAND 6
#define HANDED 7
#define HEAD 8
#define LEG 9
#define LIGHT_HEADED 10
#define NECK 11
#define SPINE 12
#define TOE 13
#define HAIR 14
#define BLOOD 15
#define LUNG 16
#define NOSE 17
#define STOMACH 18

#define BALL_IN_MON	(u.uswallow && uball && uball->where == OBJ_FREE)
#define CHAIN_IN_MON	(u.uswallow && uchain && uchain->where == OBJ_FREE)

/* Flags to control menus */
#define MENUTYPELEN sizeof("traditional ")
#define MENU_TRADITIONAL 0
#define MENU_COMBINATION 1
#define MENU_PARTIAL	 2
#define MENU_FULL	 3

#define MENU_SELECTED	TRUE
#define MENU_UNSELECTED FALSE

/*
 * Option flags
 * Each higher number includes the characteristics of the numbers
 * below it.
 */
#define SET_IN_FILE	0 /* config file option only */
#define SET_VIA_PROG	1 /* may be set via extern program, not seen in game */
#define DISP_IN_GAME	2 /* may be set via extern program, displayed in game */
#define SET_IN_GAME	3 /* may be set via extern program or set in the game */

#define FEATURE_NOTICE_VER(major,minor,patch) (((unsigned long)major << 24) | \
	((unsigned long)minor << 16) | \
	((unsigned long)patch << 8) | \
	((unsigned long)0))

#define FEATURE_NOTICE_VER_MAJ	  (flags.suppress_alert >> 24)
#define FEATURE_NOTICE_VER_MIN	  (((unsigned long)(0x0000000000FF0000L & flags.suppress_alert)) >> 16)
#define FEATURE_NOTICE_VER_PATCH  (((unsigned long)(0x000000000000FF00L & flags.suppress_alert)) >>  8)

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif
#define plur(x) (((x) == 1) ? "" : "s")

/* armor bonus value moved to do_wear.c --Amy */
#define ARM_BONUS(obj)	armorbonusvalue(obj)

/* implants give extra AC from enchantment ONLY if you're in a form that gets their good bonuses --Amy
 * but negatively enchanted ones will always make your AC worse! */
#define ARM_BONUS_IMPLANT(obj)	(objects[(obj)->otyp].a_ac + ((objects[obj->otyp].oc_charged && (powerfulimplants() || ((obj)->spe < 0) )) ? (obj)->spe : 0) \
			 - min((int)greatest_erosionX(obj),objects[(obj)->otyp].a_ac))

#define makeknown(x)	discover_object((x),TRUE,TRUE)
#define distu(xx,yy)	dist2((int)(xx),(int)(yy),(int)u.ux,(int)u.uy)
#define onlineu(xx,yy)	online2((int)(xx),(int)(yy),(int)u.ux,(int)u.uy)
#define setustuck(v)	(flags.botl = 1, u.ustuck = (v))

#define missingnoprotect	(u.ughmemory)

#define isangbander (Race_if(PM_ANGBANDER) || flags.hybridangbander)
#define isaquarian (Race_if(PM_AQUATIC_MONSTER) || flags.hybridaquarian)
#define iscurser (Race_if(PM_CURSER) || flags.hybridcurser)
#define ishaxor (Race_if(PM_HAXOR) || flags.hybridhaxor)
#define ishomicider (Race_if(PM_HOMICIDER) || flags.hybridhomicider || (!u.dungeongrowthhack && autismweaponcheck(ART_TRAP_EM_LONG)) )
#define issuxxor (Race_if(PM_SUXXOR) || flags.hybridsuxxor)
#define iswarper (Race_if(PM_WARPER) || flags.hybridwarper)
#define israndomizer (Race_if(PM_RANDOMIZER) || flags.hybridrandomizer)
#define isnullrace (Race_if(PM_NULL) || flags.hybridnullrace)
#define ismazewalker (Race_if(PM_MAZEWALKER) || flags.hybridmazewalker)
#define issoviet issovietmode() /* moved to potion.c */
#define isxrace (Race_if(PM_RACE_X) || flags.hybridxrace)
#define isheretic (Race_if(PM_HERETIC) || flags.hybridheretic)
#define issokosolver (Race_if(PM_SOKOSOLVER) || flags.hybridsokosolver)
#define isspecialist (Race_if(PM_SPECIALIST) || flags.hybridspecialist)
#define isamerican (Race_if(PM_AMERICAN) || flags.hybridamerican)
#define isminimalist (Race_if(PM_MINIMALIST) || flags.hybridminimalist || (!u.dungeongrowthhack && have_minimejewel() ) )
#define isnastinator (Race_if(PM_NASTINATOR) || flags.hybridnastinator)
#define isrougelike (Race_if(PM_ROUGELIKE) || flags.hybridrougelike)
#define issegfaulter (Race_if(PM_SEGFAULTER) || flags.hybridsegfaulter)
#define isironman (Race_if(PM_IRONMAN) || flags.hybridironman)
#define isamnesiac (Race_if(PM_AMNESIAC) || flags.hybridamnesiac)
#define isproblematic (Race_if(PM_PROBLEMATIC) || flags.hybridproblematic)
#define iswindinhabitant (Race_if(PM_WIND_INHABITANT) || flags.hybridwindinhabitant)
#define isaggravator (Race_if(PM_AGGRAVATOR) || flags.hybridaggravator)
#define isevilvariant isevilvariantmode() /* moved to potion.c */
#define islevelscaler (Race_if(PM_LEVELSCALER) || flags.hybridlevelscaler)
#define iserosator (Race_if(PM_EROSATOR) || flags.hybriderosator)
#define isroommate (Race_if(PM_ROOMMATE) || flags.hybridroommate)
#define isextravator (Race_if(PM_EXTRAVATOR) || flags.hybridextravator)
#define ishallucinator (Race_if(PM_HALLUCINATOR) || flags.hybridhallucinator)
#define isbossrusher (Race_if(PM_BOSSRUSHER) || flags.hybridbossrusher)
#define isdorian (Race_if(PM_DORIAN) || flags.hybriddorian)
#define istechless (Race_if(PM_TECHLESS) || flags.hybridtechless)
#define isblait (Race_if(PM_BLAIT) || flags.hybridblait)
#define isgrouper (Race_if(PM_GROUPER) || flags.hybridgrouper)
#define isscriptor (Race_if(PM_SCRIPTOR) || flags.hybridscriptor)
#define isunbalancor (Race_if(PM_UNBALANCOR) || flags.hybridunbalancor)
#define isbeacher (Race_if(PM_BEACHER) || flags.hybridbeacher)
#define isstairseeker (Race_if(PM_STAIRSEEKER) || flags.hybridstairseeker)
#define ismatrayser (Race_if(PM_MATRAYSER) || flags.hybridmatrayser)
#define isfeminizer (Race_if(PM_FEMINIZER) || flags.hybridfeminizer)
#define ischallenger (Race_if(PM_CHALLENGER) || flags.hybridchallenger)
#define ishardmoder (Race_if(PM_HARDMODER) || flags.hybridhardmoder)
#define isstunfish (Race_if(PM_STUNNED_LIKE_A_FISH) || flags.hybridstunfish)
#define iskillfiller (Race_if(PM_KILLFILLER) || flags.hybridkillfiller)
#define isbadstatter (Race_if(PM_BADSTATTER) || flags.hybridbadstatter)
#define isdroughter (Race_if(PM_DROUGHTER) || flags.hybriddroughter)
#define isvanillaoid (Race_if(PM_VANILLAOID) || flags.hybridvanillaoid)

#define isdemagogue (Role_if(PM_DEMAGOGUE) || u.demagoguerecursion)

#define iszapem (flags.zapem || u.zapem_mode)

/* Friday the 13th is supposed to make many things harder for the player, including some evilvariant stuff --Amy
 * can also deliberately invoke the effect by playing in elm street mode */
#define evilfriday (isevilvariant || (flags.elmstreet && firgatest() && !rn2(5)) || (flags.friday13 && firgatest() && !rn2(5)))
#define isfriday ((flags.elmstreet || (!powerfulimplants() && uimplant && uimplant->oartifact == ART_CLEAR_CATASTROPHE) || (uarm && uarm->oartifact == ART_ELMHERE) || AssholeModeActive || u.badfcursed || flags.friday13 || ElmStreetEffect || u.uprops[ELM_STREET_EFFECT].extrinsic || have_elmstreetstone()) && firgatest() )

/* Luxidream wants to be able to name his characters */
#define playeraliasname (plalias[0] ? plalias : plname)

#define isselfhybrid ( (Race_if(PM_ANGBANDER) && flags.hybridangbander) || (Race_if(PM_AQUATIC_MONSTER) && flags.hybridaquarian) || (Race_if(PM_CURSER) && flags.hybridcurser) || (Race_if(PM_HAXOR) && flags.hybridhaxor) || (Race_if(PM_HOMICIDER) && flags.hybridhomicider) || (Race_if(PM_SUXXOR) && flags.hybridsuxxor) || (Race_if(PM_WARPER) && flags.hybridwarper) || (Race_if(PM_RANDOMIZER) && flags.hybridrandomizer) || (Race_if(PM_NULL) && flags.hybridnullrace) || (Race_if(PM_MAZEWALKER) && flags.hybridmazewalker) || (Race_if(PM_SOVIET) && flags.hybridsoviet) || (Race_if(PM_RACE_X) && flags.hybridxrace) || (Race_if(PM_HERETIC) && flags.hybridheretic) || (Race_if(PM_SOKOSOLVER) && flags.hybridsokosolver) || (Race_if(PM_SPECIALIST) && flags.hybridspecialist) || (Race_if(PM_AMERICAN) && flags.hybridamerican) || (Race_if(PM_MINIMALIST) && flags.hybridminimalist) || (Race_if(PM_NASTINATOR) && flags.hybridnastinator) || (Race_if(PM_ROUGELIKE) && flags.hybridrougelike) || (Race_if(PM_SEGFAULTER) && flags.hybridsegfaulter) || (Race_if(PM_IRONMAN) && flags.hybridironman) || (Race_if(PM_AMNESIAC) && flags.hybridamnesiac) || (Race_if(PM_PROBLEMATIC) && flags.hybridproblematic) || (Race_if(PM_WIND_INHABITANT) && flags.hybridwindinhabitant) || (Race_if(PM_AGGRAVATOR) && flags.hybridaggravator) || (Race_if(PM_EVILVARIANT) && flags.hybridevilvariant) || (Race_if(PM_LEVELSCALER) && flags.hybridlevelscaler) || (Race_if(PM_EROSATOR) && flags.hybriderosator) || (Race_if(PM_ROOMMATE) && flags.hybridroommate) || (Race_if(PM_EXTRAVATOR) && flags.hybridextravator) || (Race_if(PM_HALLUCINATOR) && flags.hybridhallucinator) || (Race_if(PM_BOSSRUSHER) && flags.hybridbossrusher) || (Race_if(PM_DORIAN) && flags.hybriddorian) || (Race_if(PM_TECHLESS) && flags.hybridtechless) || (Race_if(PM_BLAIT) && flags.hybridblait) || (Race_if(PM_GROUPER) && flags.hybridgrouper) || (Race_if(PM_SCRIPTOR) && flags.hybridscriptor) || (Race_if(PM_UNBALANCOR) && flags.hybridunbalancor) || (Race_if(PM_BEACHER) && flags.hybridbeacher) || (Race_if(PM_STAIRSEEKER) && flags.hybridstairseeker) || (Race_if(PM_MATRAYSER) && flags.hybridmatrayser) || (Race_if(PM_HARDMODER) && flags.hybridhardmoder) || (Race_if(PM_BADSTATTER) && flags.hybridbadstatter) || (Race_if(PM_KILLFILLER) && flags.hybridkillfiller) || (Race_if(PM_DROUGHTER) && flags.hybriddroughter) || (Race_if(PM_FEMINIZER) && flags.hybridfeminizer) || (Race_if(PM_CHALLENGER) && flags.hybridchallenger) || (Race_if(PM_VANILLAOID) && flags.hybridvanillaoid) || (Race_if(PM_STUNNED_LIKE_A_FISH) && flags.hybridstunfish) )

#define rn1(x,y)	(rn2(x)+(y))

/* negative armor class is randomly weakened to prevent invulnerability */
#define AC_VALUE(AC)	((AC) >= 0 ? (AC) : -rnd(-(AC)))

#if defined(MICRO) && !defined(__DJGPP__)
#define getuid() 1
#define getlogin() ((char *)0)
#endif /* MICRO */

#if defined(OVERLAY)&&(defined(OVL0)||defined(OVL1)||defined(OVL2)||defined(OVL3)||defined(OVLB))
# define USE_OVLx
# define STATIC_DCL extern
# define STATIC_OVL
# ifdef OVLB
#  define STATIC_VAR
# else
#  define STATIC_VAR extern
# endif

#else	/* !OVERLAY || (!OVL0 && !OVL1 && !OVL2 && !OVL3 && !OVLB) */
# define STATIC_DCL static
# define STATIC_OVL static
# define STATIC_VAR static

/* If not compiling an overlay, compile everything. */
# define OVL0	/* highest priority */
# define OVL1
# define OVL2
# define OVL3	/* lowest specified priority */
# define OVLB	/* the base overlay segment */
#endif	/* OVERLAY && (OVL0 || OVL1 || OVL2 || OVL3 || OVLB) */

/* Macro for a few items that are only static if we're not overlaid.... */
#if defined(USE_TRAMPOLI) || defined(USE_OVLx)
# define STATIC_PTR
#else
# define STATIC_PTR static
#endif

/* For my clever ending messages... */
extern int Instant_Death;
extern int Quick_Death;
extern int Nibble_Death;
extern int last_hit;
extern int second_last_hit;
extern int third_last_hit;

/* For those tough guys who get carried away... */
extern int repeat_hit;

/* Raw status flags */
#define RAW_STAT_LEVITATION	0x00000001
#define RAW_STAT_CONFUSION	0x00000002
#define RAW_STAT_FOODPOIS	0x00000004
#define RAW_STAT_ILL		0x00000008
#define RAW_STAT_BLIND		0x00000010
#define RAW_STAT_STUNNED	0x00000020
#define RAW_STAT_HALLUCINATION	0x00000040
#define RAW_STAT_SLIMED		0x00000080
#define RAW_STAT_NUMBED		0x00000100
#define RAW_STAT_FROZEN		0x00000200
#define RAW_STAT_BURNED		0x00000400
#define RAW_STAT_FEARED		0x00000800
#define RAW_STAT_DIMMED		0x00001000

/* The function argument to qsort() requires a particular
 * calling convention under WINCE which is not the default
 * in that environment.
 */
#if defined(WIN_CE)
# define CFDECLSPEC __cdecl
#else
# define CFDECLSPEC
#endif
 
#endif /* HACK_H */
