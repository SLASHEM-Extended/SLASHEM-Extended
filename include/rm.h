/*	SCCS Id: @(#)rm.h	3.4	1999/12/12	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef RM_H
#define RM_H

/*
 * The dungeon presentation graphics code and data structures were rewritten
 * and generalized for NetHack's release 2 by Eric S. Raymond (eric@snark)
 * building on Don G. Kneller's MS-DOS implementation.	See drawing.c for
 * the code that permits the user to set the contents of the symbol structure.
 *
 * The door representation was changed by Ari Huttunen(ahuttune@niksula.hut.fi)
 */

/*
 * TLCORNER	TDWALL		TRCORNER
 * +-		-+-		-+
 * |		 |		 |
 *
 * TRWALL	CROSSWALL	TLWALL		HWALL
 * |		 |		 |
 * +-		-+-		-+		---
 * |		 |		 |
 *
 * BLCORNER	TUWALL		BRCORNER	VWALL
 * |		 |		 |		|
 * +-		-+-		-+		|
 */

/* Level location types */
#define STONE		0
#define VWALL		1
#define HWALL		2
#define TLCORNER	3
#define TRCORNER	4
#define BLCORNER	5
#define BRCORNER	6
#define CROSSWALL	7	/* For pretty mazes and special levels */
#define TUWALL		8
#define TDWALL		9
#define TLWALL		10
#define TRWALL		11
#define DBWALL		12
#define TREE		13	/* Added by KMH */
#define SDOOR		14
#define SCORR		15
#define POOL		16
#define MOAT		17	/* pool that doesn't boil, adjust messages */
#define WATER		18
#define DRAWBRIDGE_UP	19
#define LAVAPOOL	20
#define IRONBARS		21	/* Added by KMH */
#define DOOR		22
#define CORR		23
#define ROOM		24
#define STAIRS		25
#define LADDER		26
#define FOUNTAIN	27
#define THRONE		28
#define SINK		29
#define TOILET          30
#define GRAVE           31
#define ALTAR           32
#define ICE             33
#define DRAWBRIDGE_DOWN 34
#define AIR             35
#define CLOUD           36

#define MAX_TYPE		37
#define INVALID_TYPE	127

/*
 * Avoid using the level types in inequalities:
 * these types are subject to change.
 * Instead, use one of the macros below.
 */
#define IS_WALL(typ)	((typ) && (typ) <= DBWALL)
#define IS_STWALL(typ)	((typ) <= DBWALL)	/* STONE <= (typ) <= DBWALL */
#define IS_ROCK(typ)	((typ) < POOL)		/* absolutely nonaccessible */
#define IS_DOOR(typ)	((typ) == DOOR)
#define IS_IRONBAR(typ)	((typ) == IRONBARS)
#define IS_TREE(typ)	((typ) == TREE || \
			(level.flags.arboreal && (typ) == STONE))
#define ACCESSIBLE(typ) ((typ) >= DOOR)		/* good position */
#define IS_ROOM(typ)	((typ) >= ROOM)		/* ROOM, STAIRS, furniture.. */
#define ZAP_POS(typ)	((typ) >= POOL)
#define IS_GRAVE(typ)	((typ) == GRAVE)
#define SPACE_POS(typ)	((typ) > DOOR)
#define IS_POOL(typ)	((typ) >= POOL && (typ) <= DRAWBRIDGE_UP)
#define IS_THRONE(typ)	((typ) == THRONE)
#define IS_FOUNTAIN(typ) ((typ) == FOUNTAIN)
#define IS_SINK(typ)	((typ) == SINK)
#define IS_TOILET(typ)  ((typ) == TOILET)
#define IS_GRAVE(typ)	((typ) == GRAVE)
#define IS_ALTAR(typ)	((typ) == ALTAR)
#define IS_DRAWBRIDGE(typ) ((typ) == DRAWBRIDGE_UP || (typ) == DRAWBRIDGE_DOWN)
#define IS_FURNITURE(typ) ((typ) >= STAIRS && (typ) <= ALTAR)
#define IS_AIR(typ)	((typ) == AIR || (typ) == CLOUD)
#define IS_SOFT(typ)	((typ) == AIR || (typ) == CLOUD || IS_POOL(typ))

/*
 * The screen symbols may be the default or defined at game startup time.
 * See drawing.c for defaults.
 * Note: {ibm|dec}_graphics[] arrays (also in drawing.c) must be kept in synch.
 */

/* begin dungeon characters */

#define S_stone		0
#define S_vwall		1
#define S_hwall		2
#define S_tlcorn	3
#define S_trcorn	4
#define S_blcorn	5
#define S_brcorn	6
#define S_crwall	7
#define S_tuwall	8
#define S_tdwall	9
#define S_tlwall	10
#define S_trwall	11
#define S_ndoor		12
#define S_vodoor	13
#define S_hodoor	14
#define S_vcdoor	15	/* closed door, vertical wall */
#define S_hcdoor	16	/* closed door, horizontal wall */
#define S_bars		17	/* Added by KMH */
#define S_tree		18	/* Added by KMH */
#define S_room		19
#define S_darkroom		20
#define S_corr		21
#define S_litcorr	22
#define S_upstair	23
#define S_dnstair	24
#define S_upladder	25
#define S_dnladder	26
#define S_altar		27
#define S_grave		28
#define S_throne	29
#define S_sink		30
#define S_toilet	31
#define S_fountain	32
#define S_pool		33
#define S_ice		34
#define S_lava		35
#define S_vodbridge	36
#define S_hodbridge	37
#define S_vcdbridge	38	/* closed drawbridge, vertical wall */
#define S_hcdbridge	39	/* closed drawbridge, horizontal wall */
#define S_air		40
#define S_cloud		41
#define S_dungwall		42
#define S_water		43


/* end dungeon characters, begin traps */

#define S_arrow_trap		44
#define S_dart_trap		45
#define S_falling_rock_trap	46
#define S_squeaky_board		47
#define S_bear_trap		48
#define S_land_mine		49
#define S_rolling_boulder_trap	50
#define S_sleeping_gas_trap	51
#define S_rust_trap		52
#define S_fire_trap		53
#define S_pit			54
#define S_spiked_pit		55
#define S_hole			56
#define S_trap_door		57
#define S_teleportation_trap	58
#define S_level_teleporter	59
#define S_magic_portal		60
#define S_web			61
#define S_statue_trap		62
#define S_magic_trap		63
#define S_anti_magic_trap	64
#define S_polymorph_trap	65
#define S_ice_trap		66
#define S_spear_trap		67
#define S_falling_rocks_trap		68
#define S_magic_beam_trap		69
#define S_pile_of_shit		70
#define S_animation_trap		71
#define S_glyph_of_warding		72
#define S_scything_blade		73
#define S_bolt_trap		74
#define S_acid_pool		75
#define S_water_pool		76
#define S_poison_gas_trap		77
#define S_slow_gas_trap		78
#define S_shock_trap		79
#define S_rmb_loss_trap		80
#define S_display_trap		81
#define S_spell_loss_trap		82
#define S_yellow_spell_trap		83
#define S_auto_destruct_trap		84
#define S_memory_trap		85
#define S_inventory_trap		86
#define S_black_ng_wall_trap		87
#define S_menu_trap		88
#define S_speed_trap		89
#define S_superscroller_trap		90
#define S_active_superscroller_trap		91
#define S_swarm_trap		92
#define S_automatic_switcher		93
#define S_unknown_trap		94
#define S_heel_trap		95
#define S_vuln_trap		96

#define S_lava_trap		97
#define S_flood_trap		98
#define S_drain_trap		99
#define S_free_hand_trap		100
#define S_disintegration_trap		101
#define S_unidentify_trap		102
#define S_thirst_trap		103
#define S_luck_trap		104
#define S_shades_of_grey_trap		105
#define S_item_telep_trap		106
#define S_gravity_trap		107
#define S_death_trap		108
#define S_stone_to_flesh_trap		109
#define S_quicksand_trap		110
#define S_faint_trap		111
#define S_curse_trap		112
#define S_difficulty_trap		113
#define S_sound_trap		114
#define S_caster_trap		115
#define S_weakness_trap		116
#define S_rot_thirteen_trap		117
#define S_bishop_trap		118
#define S_confusion_trap		119
#define S_nupesell_trap		120
#define S_drop_trap		121
#define S_dstw_trap		122
#define S_status_trap		123
#define S_alignment_trap		124
#define S_stairs_trap		125

#define S_pestilence_trap		126
#define S_famine_trap		127

#define S_relocation_trap		128

#define S_uninformation_trap		129

#define S_intrinsic_loss_trap	130
#define S_blood_loss_trap	131
#define S_bad_effect_trap	132
#define S_multiplying_trap	133
#define S_auto_vuln_trap	134
#define S_tele_items_trap	135
#define S_nastiness_trap	136

#define S_recursion_trap	137
#define S_respawn_trap	138
#define S_warp_zone	139
#define S_captcha_trap	140
#define S_mind_wipe_trap	141
#define S_shuriken_trap	142
#define S_shit_pit	143
#define S_shaft_trap	144
#define S_lock_trap	145
#define S_mc_trap	146
#define S_farlook_trap	147
#define S_gateway_trap	148
#define S_growing_trap	149
#define S_cooling_trap	150
#define S_bar_trap	151
#define S_locking_trap	152
#define S_air_trap	153
#define S_terrain_trap	154

#define S_loudspeaker	155
#define S_nest_trap	156
#define S_bullet_trap	157
#define S_paralysis_trap	158
#define S_cyanide_trap	159
#define S_laser_trap	160
#define S_giant_chasm	161
#define S_fart_trap	162
#define S_confuse_trap	163
#define S_stun_trap	164
#define S_hallucination_trap	165
#define S_petrification_trap	166
#define S_numbness_trap	167
#define S_freezing_trap	168
#define S_burning_trap	169
#define S_fear_trap	170
#define S_blindness_trap	171
#define S_glib_trap	172
#define S_slime_trap	173
#define S_inertia_trap	174
#define S_time_trap	175
#define S_lycanthropy_trap	176
#define S_unlight_trap	177
#define S_elemental_trap	178
#define S_escalating_trap	179
#define S_negative_trap	180
#define S_mana_trap	181
#define S_sin_trap	182
#define S_destroy_armor_trap	183
#define S_divine_anger_trap	184
#define S_genetic_trap	185
#define S_missingno_trap	186
#define S_cancellation_trap	187
#define S_hostility_trap	188
#define S_boss_trap	189
#define S_wishing_trap	190
#define S_recurring_amn_trap	191
#define S_bigscript_trap	192
#define S_bank_trap	193
#define S_only_trap	194
#define S_map_trap	195
#define S_tech_trap	196
#define S_disenchant_trap	197
#define S_verisiert	198
#define S_chaos_trap	199
#define S_muteness_trap	200
#define S_ntll_trap	201
#define S_engraving_trap	202
#define S_magic_device_trap	203
#define S_book_trap	204
#define S_level_trap	205
#define S_quiz_trap	206

#define S_falling_boulder_trap	207
#define S_glass_arrow_trap	208
#define S_glass_bolt_trap	209
#define S_out_of_magic_trap	210
#define S_plasma_trap	211
#define S_bomb_trap	212
#define S_earthquake_trap	213
#define S_noise_trap	214
#define S_glue_trap	215
#define S_guillotine_trap	216
#define S_bisection_trap	217
#define S_volt_trap	218
#define S_horde_trap	219
#define S_immobility_trap	220
#define S_green_glyph_trap	221
#define S_blue_glyph_trap	222
#define S_yellow_glyph_trap	223
#define S_orange_glyph_trap	224
#define S_black_glyph_trap	225
#define S_purple_glyph_trap	226

#define S_metabolic_trap	227
#define S_no_return_trap	228
#define S_ego_trap	229
#define S_fast_forward_trap	230
#define S_rotten_trap	231
#define S_unskilled_trap	232
#define S_low_stats_trap	233
#define S_training_trap	234
#define S_exercise_trap	235
#define S_falling_loadstone_trap	236
#define S_summon_undead_trap	237
#define S_falling_nastystone_trap	238

#define S_spined_ball_trap	239
#define S_pendulum_trap	240
#define S_turn_table	241
#define S_scent_trap	242
#define S_banana_trap	243
#define S_falling_tub_trap	244
#define S_alarm	245
#define S_caltrops_trap	246
#define S_blade_wire	247
#define S_magnet_trap	248
#define S_slingshot_trap	249
#define S_cannon_trap	250
#define S_venom_sprinkler	251
#define S_fumarole	252

#define S_mana_pit	253
#define S_elemental_portal	254
#define S_girliness_trap	255
#define S_fumbling_trap	256
#define S_egomonster_trap	257
#define S_flooding_trap	258
#define S_monster_cube	259
#define S_cursed_grave	260
#define S_limitation_trap	261
#define S_weak_sight_trap	262
#define S_random_message_trap	263

#define S_trap_percents		264

/* end traps, begin special effects */

#define S_vbeam		265	/* The 4 zap beam symbols.  Do NOT separate. */
#define S_hbeam		266	/* To change order or add, see function     */
#define S_lslant	267	/* zapdir_to_glyph() in display.c.	    */
#define S_rslant	268
#define S_digbeam	269	/* dig beam symbol */
#define S_flashbeam	270	/* camera flash symbol */
#define S_boomleft	271	/* thrown boomerang, open left, e.g ')'    */
#define S_boomright	272	/* thrown boomerand, open right, e.g. '('  */
#define S_ss1		273	/* 4 magic shield glyphs */
#define S_ss2		274
#define S_ss3		275
#define S_ss4		276

/* The 8 swallow symbols.  Do NOT separate.  To change order or add, see */
/* the function swallow_to_glyph() in display.c.			 */
#define S_sw_tl		277	/* swallow top left [1]			*/
#define S_sw_tc		278	/* swallow top center [2]	Order:	*/
#define S_sw_tr		279	/* swallow top right [3]		*/
#define S_sw_ml		280	/* swallow middle left [4]	1 2 3	*/
#define S_sw_mr		281	/* swallow middle right [6]	4 5 6	*/
#define S_sw_bl		282	/* swallow bottom left [7]	7 8 9	*/
#define S_sw_bc		283	/* swallow bottom center [8]		*/
#define S_sw_br		284	/* swallow bottom right [9]		*/

#define S_explode1	285	/* explosion top left			*/
#define S_explode2	286	/* explosion top center			*/
#define S_explode3	287	/* explosion top right		 Ex.	*/
#define S_explode4	288	/* explosion middle left		*/
#define S_explode5	289	/* explosion middle center	 /-\	*/
#define S_explode6	290	/* explosion middle right	 |@|	*/
#define S_explode7	291	/* explosion bottom left	 \-/	*/
#define S_explode8	292	/* explosion bottom center		*/
#define S_explode9	293	/* explosion bottom right		*/
  
/* end effects */
  
#define MAXPCHARS	294	/* maximum number of mapped characters */
#define MAXDCHARS	44	/* maximum of mapped dungeon characters */
#define MAXTCHARS	221	/* maximum of mapped trap characters */
#define MAXECHARS       29      /* maximum of mapped effects characters */
#define MAXEXPCHARS	9	/* number of explosion characters */

struct symdef {
    uchar sym;
    const char	*explanation;
#ifdef TEXTCOLOR
    uchar color;
#endif
};

extern const struct symdef defsyms[MAXPCHARS];	/* defaults */
extern uchar showsyms[MAXPCHARS];
extern const struct symdef def_warnsyms[WARNCOUNT];

/*
 * Graphics sets for display symbols
 */
#define ASCII_GRAPHICS	0	/* regular characters: '-', '+', &c */
#define IBM_GRAPHICS	1	/* PC graphic characters */
#define DEC_GRAPHICS	2	/* VT100 line drawing characters */
#define MAC_GRAPHICS	3	/* Macintosh drawing characters */

/*
 * The 5 possible states of doors
 */

#define D_NODOOR	0
#define D_BROKEN	1
#define D_ISOPEN	2
#define D_CLOSED	4
#define D_LOCKED	8
#define D_TRAPPED	16

/*
 * Some altars are considered as shrines, so we need a flag.
 */
#define AM_SHRINE	8

/*
 * Thrones should only be looted once.
 */
#define T_LOOTED	1

/*
 * Trees have more than one kick result.
 */
#define TREE_LOOTED	1
#define TREE_SWARM	2

/*
 * Fountains have limits, and special warnings.
 */
#define F_LOOTED	1
#define F_WARNED	2
#define FOUNTAIN_IS_WARNED(x,y)		(levl[x][y].looted & F_WARNED)
#define FOUNTAIN_IS_LOOTED(x,y)		(levl[x][y].looted & F_LOOTED)
#define SET_FOUNTAIN_WARNED(x,y)	levl[x][y].looted |= F_WARNED;
#define SET_FOUNTAIN_LOOTED(x,y)	levl[x][y].looted |= F_LOOTED;
#define CLEAR_FOUNTAIN_WARNED(x,y)	levl[x][y].looted &= ~F_WARNED;
#define CLEAR_FOUNTAIN_LOOTED(x,y)	levl[x][y].looted &= ~F_LOOTED;

/*
 * Doors are even worse :-) The special warning has a side effect
 * of instantly trapping the door, and if it was defined as trapped,
 * the guards consider that you have already been warned!
 */
#define D_WARNED	16

/*
 * Sinks have 3 different types of loot that shouldn't be abused
 */
#define S_LPUDDING	1
#define S_LDWASHER	2
#define S_LRING		4

/*
 * The four directions for a DrawBridge.
 */
#define DB_NORTH	0
#define DB_SOUTH	1
#define DB_EAST		2
#define DB_WEST		3
#define DB_DIR		3	/* mask for direction */

/*
 * What's under a drawbridge.
 */
#define DB_MOAT		0
#define DB_LAVA		4
#define DB_ICE		8
#define DB_FLOOR	16
#define DB_UNDER	28	/* mask for underneath */

/*
 * Wall information.
 */
#define WM_MASK		0x07	/* wall mode (bottom three bits) */
#define W_NONDIGGABLE	0x08
#define W_NONPASSWALL	0x10

/*
 * Ladders (in Vlad's tower) may be up or down.
 */
#define LA_UP		1
#define LA_DOWN		2

/*
 * Room areas may be iced pools
 */
#define ICED_POOL	8
#define ICED_MOAT	16

/*
 * The structure describing a coordinate position.
 * Before adding fields, remember that this will significantly affect
 * the size of temporary files and save files.
 */
struct rm {
#ifdef DISPLAY_LAYERS
	Bitfield(mem_bg,6);	/* Remembered background */
	Bitfield(mem_trap,8);	/* Remembered trap */
	Bitfield(mem_obj,18);	/* Remembered object/corpse */
	Bitfield(mem_corpse,1);	/* Set if mem_obj refers to a corpse */
	Bitfield(mem_invis,1);	/* Set if invisible monster remembered */
	Bitfield(mem_spare,9);
#else
	int glyph;		/* what the hero thinks is there */
#endif
	schar typ;		/* what is really there */
	uchar seenv;		/* seen vector */
	Bitfield(flags,5);	/* extra information for typ */
	Bitfield(horizontal,1); /* wall/door/etc is horiz. (more typ info) */
	Bitfield(lit,1);	/* speed hack for lit rooms */
	Bitfield(waslit,1);	/* remember if a location was lit */
	Bitfield(roomno,6);	/* room # for special rooms */
	Bitfield(edge,1);	/* marks boundaries for special rooms*/
	int fleecycolor;	/* mainly for corridors --Amy */
};

/*
 * Add wall angle viewing by defining "modes" for each wall type.  Each
 * mode describes which parts of a wall are finished (seen as as wall)
 * and which are unfinished (seen as rock).
 *
 * We use the bottom 3 bits of the flags field for the mode.  This comes
 * in conflict with secret doors, but we avoid problems because until
 * a secret door becomes discovered, we know what sdoor's bottom three
 * bits are.
 *
 * The following should cover all of the cases.
 *
 *	type	mode				Examples: R=rock, F=finished
 *	-----	----				----------------------------
 *	WALL:	0 none				hwall, mode 1
 *		1 left/top (1/2 rock)			RRR
 *		2 right/bottom (1/2 rock)		---
 *							FFF
 *
 *	CORNER: 0 none				trcorn, mode 2
 *		1 outer (3/4 rock)			FFF
 *		2 inner (1/4 rock)			F+-
 *							F|R
 *
 *	TWALL:	0 none				tlwall, mode 3
 *		1 long edge (1/2 rock)			F|F
 *		2 bottom left (on a tdwall)		-+F
 *		3 bottom right (on a tdwall)		R|F
 *
 *	CRWALL: 0 none				crwall, mode 5
 *		1 top left (1/4 rock)			R|F
 *		2 top right (1/4 rock)			-+-
 *		3 bottom left (1/4 rock)		F|R
 *		4 bottom right (1/4 rock)
 *		5 top left & bottom right (1/2 rock)
 *		6 bottom left & top right (1/2 rock)
 */

#define WM_W_LEFT 1			/* vertical or horizontal wall */
#define WM_W_RIGHT 2
#define WM_W_TOP WM_W_LEFT
#define WM_W_BOTTOM WM_W_RIGHT

#define WM_C_OUTER 1			/* corner wall */
#define WM_C_INNER 2
#define WM_T_LONG 1			/* T wall */
#define WM_T_BL   2
#define WM_T_BR   3

#define WM_X_TL   1			/* cross wall */
#define WM_X_TR   2
#define WM_X_BL   3
#define WM_X_BR   4
#define WM_X_TLBR 5
#define WM_X_BLTR 6

/*
 * Seen vector values.	The seen vector is an array of 8 bits, one for each
 * octant around a given center x:
 *
 *			0 1 2
 *			7 x 3
 *			6 5 4
 *
 * In the case of walls, a single wall square can be viewed from 8 possible
 * directions.	If we know the type of wall and the directions from which
 * it has been seen, then we can determine what it looks like to the hero.
 */
#define SV0 0x1
#define SV1 0x2
#define SV2 0x4
#define SV3 0x8
#define SV4 0x10
#define SV5 0x20
#define SV6 0x40
#define SV7 0x80
#define SVALL 0xFF



#define doormask	flags
#define altarmask	flags
#define wall_info	flags
#define ladder		flags
#define drawbridgemask	flags
#define looted		flags
#define icedpool	flags

#define blessedftn	horizontal  /* a fountain that grants attribs */
#define disturbed	horizontal  /* a grave that has been disturbed */

struct damage {
	struct damage *next;
	long when, cost;
	coord place;
	schar typ;
};

struct levelflags {
	uchar	nfountains;		/* number of fountains on level */
	uchar	nsinks;		/* number of sinks + toilets on the level */
	/* Several flags that give hints about what's on the level */
	Bitfield(has_shop, 1);
	Bitfield(has_vault, 1);
	Bitfield(has_zoo, 1);
	Bitfield(has_court, 1);
	Bitfield(has_morgue, 1);
	Bitfield(has_beehive, 1);
	Bitfield(has_barracks, 1);
	Bitfield(has_temple, 1);
	Bitfield(has_lemurepit, 1);
	Bitfield(has_migohive, 1);
	Bitfield(has_fungusfarm, 1);
	Bitfield(has_clinic, 1);
	Bitfield(has_terrorhall, 1);
	Bitfield(has_elemhall, 1);
	Bitfield(has_angelhall, 1);
	Bitfield(has_mimichall, 1);
	Bitfield(has_nymphhall, 1);
	Bitfield(has_spiderhall, 1);
	Bitfield(has_trollhall, 1);
	Bitfield(has_humanhall, 1);
	Bitfield(has_golemhall, 1);
	Bitfield(has_coinhall, 1);

	Bitfield(has_tenshall, 1);

	Bitfield(has_traproom, 1);
	Bitfield(has_poolroom, 1);
	Bitfield(has_grueroom, 1);

	Bitfield(has_statueroom, 1);
	Bitfield(has_insideroom, 1);
	Bitfield(has_riverroom, 1);

	Bitfield(has_swamp, 1);
	Bitfield(noteleport,1);
	Bitfield(hardfloor,1);
	Bitfield(nommap,1);
	Bitfield(hero_memory,1);	/* hero has memory */
	Bitfield(shortsighted,1);	/* monsters are shortsighted */
	Bitfield(graveyard,1);		/* has_morgue, but remains set */
	Bitfield(is_maze_lev,1);

	Bitfield(is_cavernous_lev,1);
	Bitfield(arboreal, 1);		/* Trees replace rock */
	/* and where it is written "ad aquarium"... */
	Bitfield(vault_is_aquarium,1);
	Bitfield(vault_is_cursed,1);
	Bitfield(spooky,1);		/* Spooky sounds (Tina Hall) */
	Bitfield(lethe, 1);			/* All water on level causes amnesia */
};

typedef struct
{
    struct rm		locations[COLNO][ROWNO];
#ifndef MICROPORT_BUG
    struct obj		*objects[COLNO][ROWNO];
    struct monst	*monsters[COLNO][ROWNO];
#else
    struct obj		*objects[1][ROWNO];
    char		*yuk1[COLNO-1][ROWNO];
    struct monst	*monsters[1][ROWNO];
    char		*yuk2[COLNO-1][ROWNO];
#endif
    struct obj		*objlist;
    struct obj		*buriedobjlist;
    struct monst	*monlist;
    struct damage	*damagelist;
    struct levelflags	flags;
}
dlevel_t;

extern dlevel_t level;	/* structure describing the current level */

/*
 * Macros for compatibility with old code. Someday these will go away.
 */
#define levl		level.locations
#define fobj		level.objlist
#define fmon		level.monlist

/*
 * Covert a trap number into the defsym graphics array.
 * Convert a defsym number into a trap number.
 * Assumes that arrow trap will always be the first trap.
 */
#define trap_to_defsym(t) (S_arrow_trap+(t)-1)
#define defsym_to_trap(d) ((d)-S_arrow_trap+1)

#define OBJ_AT(x,y)	(level.objects[x][y] != (struct obj *)0)
/*
 * Macros for encapsulation of level.monsters references.
 */
#define MON_AT(x,y)	(level.monsters[x][y] != (struct monst *)0 && \
			 !(level.monsters[x][y])->mburied)
#define MON_BURIED_AT(x,y)	(level.monsters[x][y] != (struct monst *)0 && \
				(level.monsters[x][y])->mburied)
#ifndef STEED
#define place_monster(m,x,y)	((m)->mx=(x),(m)->my=(y),\
				 level.monsters[(m)->mx][(m)->my]=(m))
#endif
#define place_worm_seg(m,x,y)	level.monsters[x][y] = m
#define remove_monster(x,y)	level.monsters[x][y] = (struct monst *)0
#define m_at(x,y)		(MON_AT(x,y) ? level.monsters[x][y] : \
						(struct monst *)0)
#define m_buried_at(x,y)	(MON_BURIED_AT(x,y) ? level.monsters[x][y] : \
						       (struct monst *)0)

#endif /* RM_H */
