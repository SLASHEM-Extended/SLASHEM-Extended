/*	SCCS Id: @(#)decl.h	3.4	2001/12/10	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef DECL_H
#define DECL_H

#define E extern

E int (*occupation)(void);
E int (*afternmv)(void);

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
E char multi_txt[BUFSZ];
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

E NEARDATA int in_doagain;

E struct dgn_topology {		/* special dungeon levels for speed */
    d_level	d_oracle_level;
    d_level	d_bigroom_level;	/* unused */
#ifdef REINCARNATION
    d_level	d_rogue_level;
#endif
    d_level	d_medusa_level;
    d_level	d_elderpriest_level;
    d_level     d_mineend_level;
    d_level	d_stronghold_level;
    d_level	d_valley_level;
    d_level	d_wiz1_level;
    d_level	d_wiz2_level;
    d_level	d_wiz3_level;
    d_level	d_juiblex_level;
    d_level	d_orcus_level;
    d_level	d_nymph_level;
    d_level	d_forge_level;
    d_level	d_hitch_level;
    d_level	d_compu_level;
    d_level	d_key_level;
    d_level	d_baalzebub_level;	/* unused */
    d_level	d_mtemple_level;	/* unused */
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
    xchar	d_sheol_dnum;
    xchar   d_spiders_dnum;
    xchar   d_grund_dnum;
    xchar   d_wyrm_dnum;
    xchar   d_frnkn_dnum;
    xchar   d_gcavern_dnum;
    xchar   d_mtemple_dnum;
    xchar   d_slsea_dnum;
    xchar   d_tomb_dnum;
    xchar   d_dod_dnum;
    xchar   d_town_dnum;
    xchar   d_gehennom_dnum;
    xchar   d_yendoriantower_dnum;
    xchar   d_forgingchamber_dnum;
    xchar   d_orderedchaos_dnum;
    xchar   d_deadgrounds_dnum;
    xchar   d_subquest_dnum;
    xchar   d_rivalquest_dnum;
    xchar   d_bellcaves_dnum;
    xchar   d_illusorycastle_dnum;
    xchar   d_voiddungeon_dnum;
    xchar   d_netherrealm_dnum;
    xchar   d_deepmines_dnum;
    xchar   d_angmar_dnum;
    xchar   d_emynluin_dnum;
    xchar   d_greencross_dnum;
    xchar   d_minotaurmaze_dnum;
    xchar   d_swimmingpool_dnum;
    xchar   d_hellbathroom_dnum;
    xchar   d_spacebase_dnum;
    xchar   d_sewerplant_dnum;
    xchar   d_gammacaves_dnum;
    xchar   d_mainframe_dnum;
    xchar   d_joustchallenge_dnum;
    xchar   d_pacmanchallenge_dnum;
    xchar   d_digdugchallenge_dnum;
    xchar   d_gruechallenge_dnum;
    xchar   d_poolchallenge_dnum;
    xchar   d_restingzone_ga_dnum;
    xchar   d_restingzone_gb_dnum;
    xchar   d_restingzone_gc_dnum;
    xchar   d_restingzone_gd_dnum;
    xchar   d_restingzone_ge_dnum;
    xchar   d_restingzone_ta_dnum;
    xchar   d_restingzone_tb_dnum;
    xchar   d_restingzone_tc_dnum;
    xchar   d_restingzone_td_dnum;
    xchar   d_restingzone_te_dnum;
    xchar   d_restingzone_tf_dnum;
    xchar   d_restingzone_tg_dnum;
    xchar   d_restingzone_th_dnum;
    xchar   d_restingzone_ti_dnum;
    xchar   d_restingzone_tj_dnum;
    xchar   d_restingzone_a_dnum;
    xchar   d_restingzone_s_dnum;
    d_level	d_lawful_quest_level;
    d_level	d_neutral_quest_level;
    d_level	d_chaotic_quest_level;
    d_level	d_qstart_level, d_qlocate_level, d_nemesis_level;

    d_level d_qxone_level, d_qxtwo_level,  d_qxthree_level,  d_qxfour_level,  d_qxfive_level;
    d_level d_qxsix_level, d_qxseven_level,  d_qxeight_level,  d_qxnine_level,  d_qxten_level;
    d_level d_qya_level, d_qyb_level, d_qyc_level, d_qyd_level, d_qye_level;
    d_level	d_knox_level;
    d_level     d_blackmarket_level;
#ifdef RECORD_ACHIEVE
    /*d_level     d_mineend_level;*/ /* already added above */
    d_level     d_sokoend_level;
    d_level     d_deepend_level;
#endif
} dungeon_topology;
/* macros for accesing the dungeon levels by their old names */
#define oracle_level		(dungeon_topology.d_oracle_level)
#define bigroom_level		(dungeon_topology.d_bigroom_level)
#ifdef REINCARNATION
#define rogue_level		(dungeon_topology.d_rogue_level)
#endif
#define medusa_level		(dungeon_topology.d_medusa_level)
#define elderpriest_level	(dungeon_topology.d_elderpriest_level)
#define stronghold_level	(dungeon_topology.d_stronghold_level)
#define valley_level		(dungeon_topology.d_valley_level)
#define mineend_level           (dungeon_topology.d_mineend_level)
#define wiz1_level		(dungeon_topology.d_wiz1_level)
#define wiz2_level		(dungeon_topology.d_wiz2_level)
#define wiz3_level		(dungeon_topology.d_wiz3_level)
#define juiblex_level		(dungeon_topology.d_juiblex_level)
#define orcus_level		(dungeon_topology.d_orcus_level)
#define baalzebub_level		(dungeon_topology.d_baalzebub_level)
#define mtemple_level		(dungeon_topology.d_mtemple_level)
#define nymph_level		(dungeon_topology.d_nymph_level)
#define forge_level		(dungeon_topology.d_forge_level)
#define hitch_level		(dungeon_topology.d_hitch_level)
#define compu_level		(dungeon_topology.d_compu_level)
#define key_level		(dungeon_topology.d_key_level)
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
#define sheol_dnum		(dungeon_topology.d_sheol_dnum)
#define yendoriantower_dnum		(dungeon_topology.d_yendoriantower_dnum)
#define forgingchamber_dnum		(dungeon_topology.d_forgingchamber_dnum)
#define orderedchaos_dnum		(dungeon_topology.d_orderedchaos_dnum)
#define deadgrounds_dnum		(dungeon_topology.d_deadgrounds_dnum)
#define illusorycastle_dnum		(dungeon_topology.d_illusorycastle_dnum)
#define voiddungeon_dnum		(dungeon_topology.d_voiddungeon_dnum)
#define netherrealm_dnum		(dungeon_topology.d_netherrealm_dnum)
#define deepmines_dnum		(dungeon_topology.d_deepmines_dnum)
#define angmar_dnum		(dungeon_topology.d_angmar_dnum)
#define emynluin_dnum		(dungeon_topology.d_emynluin_dnum)
#define greencross_dnum		(dungeon_topology.d_greencross_dnum)
#define minotaurmaze_dnum		(dungeon_topology.d_minotaurmaze_dnum)
#define swimmingpool_dnum		(dungeon_topology.d_swimmingpool_dnum)
#define hellbathroom_dnum		(dungeon_topology.d_hellbathroom_dnum)
#define spacebase_dnum		(dungeon_topology.d_spacebase_dnum)
#define sewerplant_dnum		(dungeon_topology.d_sewerplant_dnum)
#define gammacaves_dnum		(dungeon_topology.d_gammacaves_dnum)
#define mainframe_dnum		(dungeon_topology.d_mainframe_dnum)
#define joustchallenge_dnum	(dungeon_topology.d_joustchallenge_dnum)
#define pacmanchallenge_dnum	(dungeon_topology.d_pacmanchallenge_dnum)
#define digdugchallenge_dnum	(dungeon_topology.d_digdugchallenge_dnum)
#define gruechallenge_dnum	(dungeon_topology.d_gruechallenge_dnum)
#define poolchallenge_dnum	(dungeon_topology.d_poolchallenge_dnum)
#define subquest_dnum		(dungeon_topology.d_subquest_dnum)
#define rivalquest_dnum		(dungeon_topology.d_rivalquest_dnum)
#define bellcaves_dnum		(dungeon_topology.d_bellcaves_dnum)
#define dod_dnum			(dungeon_topology.d_dod_dnum)
#define town_dnum			(dungeon_topology.d_town_dnum)
#define gehennom_dnum		(dungeon_topology.d_gehennom_dnum)
#define quest_dnum		(dungeon_topology.d_quest_dnum)
#define qstart_level		(dungeon_topology.d_qstart_level)
#define qlocate_level		(dungeon_topology.d_qlocate_level)
#define nemesis_level		(dungeon_topology.d_nemesis_level)
#define qxone_level		(dungeon_topology.d_qxone_level)
#define qxtwo_level		(dungeon_topology.d_qxtwo_level)
#define qxthree_level		(dungeon_topology.d_qxthree_level)
#define qxfour_level		(dungeon_topology.d_qxfour_level)
#define qxfive_level		(dungeon_topology.d_qxfive_level)
#define qxsix_level		(dungeon_topology.d_qxsix_level)
#define qxseven_level		(dungeon_topology.d_qxseven_level)
#define qxeight_level		(dungeon_topology.d_qxeight_level)
#define qxnine_level		(dungeon_topology.d_qxnine_level)
#define qxten_level		(dungeon_topology.d_qxten_level)
#define qya_level		(dungeon_topology.d_qya_level)
#define qyb_level		(dungeon_topology.d_qyb_level)
#define qyc_level		(dungeon_topology.d_qyc_level)
#define qyd_level		(dungeon_topology.d_qyd_level)
#define qye_level		(dungeon_topology.d_qye_level)
#define knox_level		(dungeon_topology.d_knox_level)
#define spiders_dnum		(dungeon_topology.d_spiders_dnum)
#define grund_dnum		(dungeon_topology.d_grund_dnum)
#define wyrm_dnum		(dungeon_topology.d_wyrm_dnum)
#define frnkn_dnum		(dungeon_topology.d_frnkn_dnum)
#define gcavern_dnum		(dungeon_topology.d_gcavern_dnum)
#define mtemple_dnum		(dungeon_topology.d_mtemple_dnum)
#define slsea_dnum		(dungeon_topology.d_slsea_dnum)
#define tomb_dnum		(dungeon_topology.d_tomb_dnum)
#define restingzone_ga_dnum		(dungeon_topology.d_restingzone_ga_dnum)
#define restingzone_gb_dnum		(dungeon_topology.d_restingzone_gb_dnum)
#define restingzone_gc_dnum		(dungeon_topology.d_restingzone_gc_dnum)
#define restingzone_gd_dnum		(dungeon_topology.d_restingzone_gd_dnum)
#define restingzone_ge_dnum		(dungeon_topology.d_restingzone_ge_dnum)
#define restingzone_ta_dnum		(dungeon_topology.d_restingzone_ta_dnum)
#define restingzone_tb_dnum		(dungeon_topology.d_restingzone_tb_dnum)
#define restingzone_tc_dnum		(dungeon_topology.d_restingzone_tc_dnum)
#define restingzone_td_dnum		(dungeon_topology.d_restingzone_td_dnum)
#define restingzone_te_dnum		(dungeon_topology.d_restingzone_te_dnum)
#define restingzone_tf_dnum		(dungeon_topology.d_restingzone_tf_dnum)
#define restingzone_tg_dnum		(dungeon_topology.d_restingzone_tg_dnum)
#define restingzone_th_dnum		(dungeon_topology.d_restingzone_th_dnum)
#define restingzone_ti_dnum		(dungeon_topology.d_restingzone_ti_dnum)
#define restingzone_tj_dnum		(dungeon_topology.d_restingzone_tj_dnum)
#define restingzone_a_dnum		(dungeon_topology.d_restingzone_a_dnum)
#define restingzone_s_dnum		(dungeon_topology.d_restingzone_s_dnum)
#define lawful_quest_level	(dungeon_topology.d_lawful_quest_level)
#define neutral_quest_level	(dungeon_topology.d_neutral_quest_level)
#define chaotic_quest_level	(dungeon_topology.d_chaotic_quest_level)
#define blackmarket_level	(dungeon_topology.d_blackmarket_level)
#ifdef RECORD_ACHIEVE
#define mineend_level           (dungeon_topology.d_mineend_level)
#define sokoend_level           (dungeon_topology.d_sokoend_level)
#define deepend_level           (dungeon_topology.d_deepend_level)
#endif

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
#ifdef DUMP_LOG
E char dump_fn[];		/* dumpfile name (dump patch) */
#endif
E const char *configfile;
E NEARDATA char plname[PL_NSIZ];
E NEARDATA char plalias[PL_NSIZ];
E NEARDATA char dogname[];
E NEARDATA char dragonname[];
E NEARDATA char monkeyname[];
E NEARDATA char parrotname[];
E NEARDATA char girlname[];
E NEARDATA char boyname[];
E NEARDATA char ravenname[];
E NEARDATA char catname[];
E NEARDATA char ghoulname[];
E NEARDATA char horsename[];
E NEARDATA char ratname[];
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

E const char qykbd_dir[], qzkbd_dir[], ndir[];
E char const *sdir;
E const schar xdir[], ydir[], zdir[];
E char misc_cmds[];

#define DORUSH			misc_cmds[0]
#define DORUN			misc_cmds[1]
#define DOFORCEFIGHT		misc_cmds[2]
#define DONOPICKUP		misc_cmds[3]
#define DORUN_NOPICKUP		misc_cmds[4]
#define DOESCAPE		misc_cmds[5]
#define DOAGAIN			misc_cmds[6]

/* the number of miscellaneous commands */
# define MISC_CMD_COUNT		7

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
	*uarmu,				/* under-wear, so to speak */
	*usaddle,
	*uskin, *uamul, *uimplant, *uleft, *uright, *ublindf,
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
	int	born;
	int	died;
	uchar	mvflags;
	uchar	eaten;		/* WAC -- eaten memory */
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

#ifdef SIMPLE_MAIL
E int mailckfreq;
#endif

#ifdef RECORD_ACHIEVE
struct u_achieve {
        Bitfield(get_bell,1);        /* You have obtained the bell of 
                                      * opening */
        Bitfield(get_candelabrum,1); /* You have obtained the candelabrum */
        Bitfield(get_book,1);        /* You have obtained the book of 
                                      * the dead */
        Bitfield(enter_gehennom,1);  /* Entered Gehennom (including the 
                                      * Valley) by any means */
        Bitfield(perform_invocation,1); /* You have performed the invocation
                                         * ritual */
        Bitfield(get_amulet,1);      /* You have obtained the amulet
                                      * of Yendor */
        Bitfield(ascended,1);        /* You ascended to demigod[dess]hood.
                                      * Not quite the same as 
                                      * u.uevent.ascended. */
        Bitfield(get_luckstone,1);   /* You obtained the luckstone at the
                                      * end of the mines. */
        Bitfield(finish_sokoban,1);  /* You obtained the sokoban prize. */
        Bitfield(killed_medusa,1);   /* You defeated Medusa. */

        Bitfield(killed_nightmare,1);   /* You defeated Nightmare. */
        Bitfield(killed_vecna,1);   /* You defeated Vecna. */
        Bitfield(killed_beholder,1);   /* You defeated the Beholder. */
        Bitfield(killed_ruggo,1);   /* You defeated Ruggo the Gnome King. */
        Bitfield(killed_kroo,1);   /* You defeated Kroo the Kobold King. */
        Bitfield(killed_grund,1);   /* You defeated Grund the Orc King. */
        Bitfield(killed_largestgiant,1);   /* You defeated the Largest Giant. */
        Bitfield(killed_shelob,1);   /* You defeated Shelob. */
        Bitfield(killed_girtab,1);   /* You defeated Girtab. */
        Bitfield(killed_aphrodite,1);   /* You defeated Aphrodite. */
        Bitfield(killed_frankenstein,1);   /* You defeated Doctor Frankenstein. */
        Bitfield(killed_croesus,1);   /* You defeated Croesus. */
        Bitfield(killed_dagon,1);   /* You defeated Father Dagon. */
        Bitfield(killed_hydra,1);   /* You defeated Mother Hydra. */
	  Bitfield(imbued_bell,1);	/* You imbued the Bell of Opening */
	  Bitfield(imbued_amulet,1);	/* You imbued the Amulet of Yendor */

};

E struct u_achieve achieve;

struct u_achieveX {
	  Bitfield(killed_elderpriest,1);	/* You killed at least one elder priest */
	  Bitfield(killed_glassgolem,1);	/* You killed the Illusory Castle glass golem boss */
	  Bitfield(killed_tiksrvzllat,1);	/* You killed Tiksrvzllat */
	  Bitfield(killed_bofh,1);	/* You killed the BOFH */
	  Bitfield(swimmingpool_cleared,1);	/* You reached the bottom of the Swimming Pool branch */
	  Bitfield(killed_katia,1);	/* You killed Erogenous Katia */
	  Bitfield(killed_witchking,1);	/* You killed the Witch King of Angmar */
	  Bitfield(get_magresstone,1);	/* You obtained the stone of magic resistance from the Deep Mines */
	  Bitfield(devnull_complete,1);	/* You visited all DevNull dungeon branches */
	  Bitfield(killed_minotaur,1);	/* You killed the Minotaur of the Maze */
	  Bitfield(killed_kalwina,1);	/* You killed Kalwina */
	  Bitfield(killed_stahngnir,1);	/* You killed Stahngnir */
	  Bitfield(killed_ariane,1);	/* You killed Ariane */
	  Bitfield(completed_rivalquest,1);	/* You killed the nemesis on the rival quest */

};

E struct u_achieveX achieveX;

#endif

#if defined(RECORD_REALTIME) || defined(REALTIME_ON_BOTL)
E struct realtime_data {
  time_t realtime;    /* Amount of actual playing time up until the last time
                       * the game was restored. */
  time_t restoretime; /* The time that the game was started or restored. */
  time_t last_displayed_time; /* Last time displayed on the status line */
} realtime_data;
#endif /* RECORD_REALTIME || REALTIME_ON_BOTL */

struct _plinemsg {
    xchar msgtype;
    char *pattern;
    struct _plinemsg *next;
};

E struct _plinemsg *pline_msg;

#define MSGTYP_NORMAL	0
#define MSGTYP_NOREP	1
#define MSGTYP_NOSHOW	2
#define MSGTYP_STOP	3

E boolean curses_stupid_hack;

#undef E

#endif /* DECL_H */
