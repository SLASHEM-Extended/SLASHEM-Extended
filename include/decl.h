/*	SCCS Id: @(#)decl.h	3.4	2001/12/10	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef DECL_H
#define DECL_H

#define E extern

E int NDECL((*occupation));
E int NDECL((*afternmv));

E const char *hname;
E int hackpid;
#if defined(UNIX) || defined(VMS)
E int locknum;
#endif
#ifdef DEF_PAGER
E char *catmore;
#endif	/* DEF_PAGER */

E char SAVEF[];
#ifdef MICRO
E char SAVEP[];
#endif

E NEARDATA int bases[MAXOCLASSES];

E NEARDATA int multi;
#if 0
E NEARDATA int warnlevel;
#endif
E NEARDATA int lastuse;
E NEARDATA int nextuse;
E NEARDATA int nroom;
E NEARDATA int nsubroom;
E NEARDATA int occtime;

#define WARNCOUNT 6			/* number of different warning levels */
E uchar warnsyms[WARNCOUNT];

E int x_maze_max, y_maze_max;
E int otg_temp;

#ifdef REDO
E NEARDATA int in_doagain;
#endif

E struct dgn_topology {		/* special dungeon levels for speed */
    d_level	d_oracle_level;
    d_level	d_bigroom_level;	/* unused */
#ifdef REINCARNATION
    d_level	d_rogue_level;
#endif
    d_level	d_medusa_level;
    d_level     d_mineend_level;
    d_level	d_stronghold_level;
    d_level	d_valley_level;
    d_level	d_wiz1_level;
    d_level	d_wiz2_level;
    d_level	d_wiz3_level;
    d_level	d_juiblex_level;
    d_level	d_orcus_level;
    d_level	d_baalzebub_level;	/* unused */
    d_level     d_demogorgon_level;      /* unused */
    d_level     d_dispater_level;      /* unused */
    d_level     d_geryon_level;      /* unused */
    d_level     d_yeenoghu_level;      /* unused */
    d_level	d_asmodeus_level;	/* unused */
    d_level	d_portal_level;		/* only in goto_level() [do.c] */
    d_level	d_sanctum_level;
    d_level	d_earth_level;
    d_level	d_water_level;
    d_level	d_fire_level;
    d_level	d_air_level;
    d_level	d_astral_level;
    xchar	d_tower_dnum;
    xchar	d_sokoban_dnum;
    xchar	d_mines_dnum, d_quest_dnum;
    xchar       d_spiders_dnum;
    d_level	d_lawful_quest_level;
    d_level	d_neutral_quest_level;
    d_level	d_chaotic_quest_level;
    d_level	d_qstart_level, d_qlocate_level, d_nemesis_level;
    d_level	d_knox_level;
#ifdef BLACKMARKET
    d_level     d_blackmarket_level;
#endif /* BLACKMARKET */
} dungeon_topology;
/* macros for accesing the dungeon levels by their old names */
#define oracle_level		(dungeon_topology.d_oracle_level)
#define bigroom_level		(dungeon_topology.d_bigroom_level)
#ifdef REINCARNATION
#define rogue_level		(dungeon_topology.d_rogue_level)
#endif
#define medusa_level		(dungeon_topology.d_medusa_level)
#define stronghold_level	(dungeon_topology.d_stronghold_level)
#define valley_level		(dungeon_topology.d_valley_level)
#define mineend_level           (dungeon_topology.d_mineend_level)
#define wiz1_level		(dungeon_topology.d_wiz1_level)
#define wiz2_level		(dungeon_topology.d_wiz2_level)
#define wiz3_level		(dungeon_topology.d_wiz3_level)
#define juiblex_level		(dungeon_topology.d_juiblex_level)
#define orcus_level		(dungeon_topology.d_orcus_level)
#define baalzebub_level		(dungeon_topology.d_baalzebub_level)
#define yeenoghu_level          (dungeon_topology.d_yeenoghu_level)
#define geryon_level            (dungeon_topology.d_geryon_level)
#define dispater_level          (dungeon_topology.d_dispater_level)
#define demogorgon_level        (dungeon_topology.d_demogorgon_level)
#define asmodeus_level		(dungeon_topology.d_asmodeus_level)
#define portal_level		(dungeon_topology.d_portal_level)
#define sanctum_level		(dungeon_topology.d_sanctum_level)
#define earth_level		(dungeon_topology.d_earth_level)
#define water_level		(dungeon_topology.d_water_level)
#define fire_level		(dungeon_topology.d_fire_level)
#define air_level		(dungeon_topology.d_air_level)
#define astral_level		(dungeon_topology.d_astral_level)
#define tower_dnum		(dungeon_topology.d_tower_dnum)
#define sokoban_dnum		(dungeon_topology.d_sokoban_dnum)
#define mines_dnum		(dungeon_topology.d_mines_dnum)
#define quest_dnum		(dungeon_topology.d_quest_dnum)
#define qstart_level		(dungeon_topology.d_qstart_level)
#define qlocate_level		(dungeon_topology.d_qlocate_level)
#define nemesis_level		(dungeon_topology.d_nemesis_level)
#define knox_level		(dungeon_topology.d_knox_level)
#define spiders_dnum		(dungeon_topology.d_spiders_dnum)
#define lawful_quest_level	(dungeon_topology.d_lawful_quest_level)
#define neutral_quest_level	(dungeon_topology.d_neutral_quest_level)
#define chaotic_quest_level	(dungeon_topology.d_chaotic_quest_level)
#ifdef BLACKMARKET
#define blackmarket_level	(dungeon_topology.d_blackmarket_level)
#endif /* BLACKMARKET */

E NEARDATA stairway dnstair, upstair;		/* stairs up and down */
#define xdnstair	(dnstair.sx)
#define ydnstair	(dnstair.sy)
#define xupstair	(upstair.sx)
#define yupstair	(upstair.sy)

E NEARDATA stairway dnladder, upladder;		/* ladders up and down */
#define xdnladder	(dnladder.sx)
#define ydnladder	(dnladder.sy)
#define xupladder	(upladder.sx)
#define yupladder	(upladder.sy)

E NEARDATA stairway sstairs;

E NEARDATA dest_area updest, dndest;	/* level-change destination areas */

E NEARDATA coord inv_pos;
E NEARDATA dungeon dungeons[];
E NEARDATA s_level *sp_levchn;
#define dunlev_reached(x)	(dungeons[(x)->dnum].dunlev_ureached)

#include "quest.h"
E struct q_score quest_status;

E NEARDATA char pl_fruit[PL_FSIZ];
E NEARDATA int current_fruit;
E NEARDATA struct fruit *ffruit;

E NEARDATA char tune[6];

#define MAXLINFO (MAXDUNGEON * MAXLEVEL)
E struct linfo level_info[MAXLINFO];

E NEARDATA struct sinfo {
	int gameover;		/* self explanatory? */
	int stopprint;		/* inhibit further end of game disclosure */
#if defined(UNIX) || defined(VMS) || defined (__EMX__) || defined(WIN32)
	int done_hup;		/* SIGHUP or moral equivalent received
				 * -- no more screen output */
#endif
	int something_worth_saving;	/* in case of panic */
	int panicking;		/* `panic' is in progress */
#if defined(VMS) || defined(WIN32)
	int exiting;		/* an exit handler is executing */
#endif
	int in_impossible;
#ifdef PANICLOG
	int in_paniclog;
#endif
} program_state;

E boolean restoring;

E const char quitchars[];
E const char vowels[];
E const char ynchars[];
E const char ynqchars[];
E const char ynaqchars[];
E const char ynNaqchars[];
E NEARDATA long yn_number;

E const char disclosure_options[];

E NEARDATA int smeq[];
E NEARDATA int doorindex;
E NEARDATA char *save_cm;
#define KILLED_BY_AN	 0
#define KILLED_BY	 1
#define NO_KILLER_PREFIX 2
E NEARDATA int killer_format;
E const char *killer;
E const char *delayed_killer;
#ifdef GOLDOBJ
E long done_money;
#endif
E char killer_buf[BUFSZ];
E const char *configfile;
E NEARDATA char plname[PL_NSIZ];
E NEARDATA char dogname[];
E NEARDATA char catname[];
E NEARDATA char ghoulname[];
E NEARDATA char horsename[];
E NEARDATA char wolfname[];
#if 0
E NEARDATA char batname[];
E NEARDATA char snakename[];
E NEARDATA char ratname[];
E NEARDATA char badgername[];
E NEARDATA char reddragonname[];
E NEARDATA char whitedragonname[];
#endif
E char preferred_pet;
E const char *occtxt;			/* defined when occupation != NULL */
E const char *nomovemsg;
E const char nul[];
E char lock[];

E const char sdir[], ndir[];
E const schar xdir[], ydir[], zdir[];

E NEARDATA schar tbx, tby;		/* set in mthrowu.c */

E NEARDATA struct multishot { int n, i; short o; boolean s; } m_shot;

E NEARDATA struct dig_info {		/* apply.c, hack.c */
	int	effort;
	d_level level;
	coord	pos;
	long lastdigtime;
	boolean down, chew, warned, quiet;
} digging;

E NEARDATA long moves, monstermoves;
E NEARDATA long wailmsg;

E NEARDATA boolean in_mklev;
E NEARDATA boolean stoned;
E NEARDATA boolean unweapon;
E NEARDATA boolean mrg_to_wielded;
E NEARDATA struct obj *current_wand;
E NEARDATA boolean defer_see_monsters;

E NEARDATA boolean in_steed_dismounting;

E const int shield_static[];


/*** Objects ***/
#include "obj.h"

E NEARDATA struct obj *invent, *uarm, *uarmc, *uarmh, *uarms, *uarmg, *uarmf,
#ifdef TOURIST
	*uarmu,				/* under-wear, so to speak */
#endif
#ifdef STEED
	*usaddle,
#endif
	*uskin, *uamul, *uleft, *uright, *ublindf,
	*uwep, *uswapwep, *uquiver;

E NEARDATA struct obj *uchain;		/* defined only when punished */
E NEARDATA struct obj *uball;
E NEARDATA struct obj *migrating_objs;
E NEARDATA struct obj *billobjs;
E NEARDATA struct obj zeroobj;		/* init'd and defined in decl.c */
E NEARDATA struct obj thisplace;	/* init'd and defined in decl.c */

#include "spell.h"
E NEARDATA struct spell spl_book[];	/* sized in decl.c */

#ifndef TECH_H
#include "tech.h"
#endif
E NEARDATA struct tech tech_list[];     /* sized in decl.c */


/*** The player ***/
E NEARDATA char pl_character[PL_CSIZ];
E NEARDATA char pl_race;		/* character's race */
/* KMH, role patch -- more maintainable when declared as an array */
E const char pl_classes[];

#include "you.h"
#include "onames.h"

E NEARDATA struct you u;
E NEARDATA struct Role urole;


/*** Monsters ***/
#ifndef PM_H
#include "pm.h"
#endif

E NEARDATA struct permonst playermon, *uasmon;
					/* also decl'd extern in permonst.h */
					/* init'd in monst.c */

E NEARDATA struct monst youmonst;	/* init'd and defined in decl.c */
E NEARDATA struct monst *mydogs, *migrating_mons;

E NEARDATA struct permonst upermonst;	/* init'd in decl.c, 
					 * defined in polyself.c 
					 */

E NEARDATA struct mvitals {
	uchar	born;
	uchar	died;
	uchar	mvflags;
#ifdef EATEN_MEMORY
	uchar	eaten;		/* WAC -- eaten memory */
#endif
} mvitals[NUMMONS];


/* The names of the colors used for gems, etc. */
E const char *c_obj_colors[];

E struct c_common_strings {
    const char	*const c_nothing_happens, *const c_thats_enough_tries,
		*const c_silly_thing_to, *const c_shudder_for_moment,
		*const c_something, *const c_Something,
		*const c_You_can_move_again,
		*const c_Never_mind, *c_vision_clears,
		*const c_the_your[2];
} c_common_strings;
#define nothing_happens    c_common_strings.c_nothing_happens
#define thats_enough_tries c_common_strings.c_thats_enough_tries
#define silly_thing_to	   c_common_strings.c_silly_thing_to
#define shudder_for_moment c_common_strings.c_shudder_for_moment
#define something	   c_common_strings.c_something
#define Something	   c_common_strings.c_Something
#define You_can_move_again c_common_strings.c_You_can_move_again
#define Never_mind	   c_common_strings.c_Never_mind
#define vision_clears	   c_common_strings.c_vision_clears
#define the_your	   c_common_strings.c_the_your

E const char no_elbow_room[];

/* material strings */
E const char *materialnm[];

/* Monster name articles */
#define ARTICLE_NONE	0
#define ARTICLE_THE	1
#define ARTICLE_A	2
#define ARTICLE_YOUR	3

/* Monster name suppress masks */
#define SUPPRESS_IT		0x01
#define SUPPRESS_INVISIBLE	0x02
#define SUPPRESS_HALLUCINATION  0x04
#define SUPPRESS_SADDLE		0x08
#define EXACT_NAME		0x0F


/*** Vision ***/
E NEARDATA boolean vision_full_recalc;	/* TRUE if need vision recalc */
E NEARDATA char **viz_array;		/* could see/in sight row pointers */


/*** Window system stuff ***/
#include "color.h"
#ifdef TEXTCOLOR
E const int zapcolors[];
#endif

E const char def_oc_syms[MAXOCLASSES];	/* default class symbols */
E uchar oc_syms[MAXOCLASSES];		/* current class symbols */
E const char def_monsyms[MAXMCLASSES];	/* default class symbols */
E uchar monsyms[MAXMCLASSES];		/* current class symbols */

E NEARDATA struct c_color_names {
    const char	*const c_black, *const c_amber, *const c_golden,
		*const c_light_blue,*const c_red, *const c_green,
		*const c_silver, *const c_blue, *const c_purple,
		*const c_white;
} c_color_names;
#define NH_BLACK		c_color_names.c_black
#define NH_AMBER		c_color_names.c_amber
#define NH_GOLDEN		c_color_names.c_golden
#define NH_LIGHT_BLUE		c_color_names.c_light_blue
#define NH_RED			c_color_names.c_red
#define NH_GREEN		c_color_names.c_green
#define NH_SILVER		c_color_names.c_silver
#define NH_BLUE			c_color_names.c_blue
#define NH_PURPLE		c_color_names.c_purple
#define NH_WHITE		c_color_names.c_white

E NEARDATA winid WIN_MESSAGE, WIN_STATUS;
E NEARDATA winid WIN_MAP, WIN_INVEN;
E char toplines[];
#ifndef TCAP_H
E struct tc_gbl_data {	/* also declared in tcap.h */
    char *tc_AS, *tc_AE;	/* graphics start and end (tty font swapping) */
    int   tc_LI,  tc_CO;	/* lines and columns */
} tc_gbl_data;
#define AS tc_gbl_data.tc_AS
#define AE tc_gbl_data.tc_AE
#define LI tc_gbl_data.tc_LI
#define CO tc_gbl_data.tc_CO
#endif

E struct authentication {
    char prog[BUFSZ];
    char args[BUFSZ];
} authentication;

#define MAXNOTILESETS		20
#ifndef TILESET_MAX_FILENAME
#define TILESET_MAX_FILENAME	256
#endif

#define TILESET_TRANSPARENT	1
#define TILESET_PSEUDO3D	2

E struct tileset {
    char name[PL_PSIZ];
    char file[TILESET_MAX_FILENAME];
    unsigned long flags;
    void *data;				/* For windowing port's use */
} tilesets[MAXNOTILESETS];
E int no_tilesets;
E struct tileset def_tilesets[];
E char tileset[PL_PSIZ];

/* xxxexplain[] is in drawing.c */
E const char * const monexplain[], invisexplain[], * const objexplain[], * const oclass_names[];

/* Some systems want to use full pathnames for some subsets of file names,
 * rather than assuming that they're all in the current directory.  This
 * provides all the subclasses that seem reasonable, and sets up for all
 * prefixes being null.  Port code can set those that it wants.
 */
#define HACKPREFIX	0
#define LEVELPREFIX	1
#define SAVEPREFIX	2
#define BONESPREFIX	3
#define DATAPREFIX	4	/* this one must match hardcoded value in dlb.c */
#define SCOREPREFIX	5
#define LOCKPREFIX	6
#define CONFIGPREFIX	7
#define TROUBLEPREFIX	8
#define PREFIX_COUNT	9
/* used in files.c; xxconf.h can override if needed */
# ifndef FQN_MAX_FILENAME
#define FQN_MAX_FILENAME 512
# endif

#if defined(NOCWD_ASSUMPTIONS) || defined(VAR_PLAYGROUND)
/* the bare-bones stuff is unconditional above to simplify coding; for
 * ports that actually use prefixes, add some more localized things
 */
#define PREFIXES_IN_USE
#endif

E char *fqn_prefix[PREFIX_COUNT];
#ifdef PREFIXES_IN_USE
E char *fqn_prefix_names[PREFIX_COUNT];
#endif


#ifdef AUTOPICKUP_EXCEPTIONS
struct autopickup_exception {
	char *pattern;
	boolean grab;
	struct autopickup_exception *next;
};
#endif /* AUTOPICKUP_EXCEPTIONS */

#undef E

#endif /* DECL_H */
