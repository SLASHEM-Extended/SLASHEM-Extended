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
#define ROCKWALL		13
#define GRAVEWALL		14
#define TUNNELWALL	15
#define TREE		16	/* Added by KMH */
#define FARMLAND		17
#define MOUNTAIN		18
#define SDOOR		19
#define SCORR		20
#define POOL		21
#define MOAT		22	/* pool that doesn't boil, adjust messages */
#define WATER		23
#define WATERTUNNEL	24
#define CRYSTALWATER	25
#define MOORLAND		26
#define URINELAKE		27
#define DRAWBRIDGE_UP	28
#define SHIFTINGSAND	29
#define LAVAPOOL	30
#define STYXRIVER		31
#define IRONBARS		32	/* Added by KMH */
#define DOOR		33
#define CORR		34
#define ROOM		35
#define STAIRS		36
#define LADDER		37
#define FOUNTAIN	38
#define THRONE		39
#define SINK		40
#define TOILET          41
#define GRAVE           42
#define ALTAR           43
#define PENTAGRAM		44
#define WELL		45
#define POISONEDWELL	46
#define WAGON		47
#define BURNINGWAGON	48
#define WOODENTABLE	49
#define CARVEDBED		50
#define STRAWMATTRESS	51
#define ICE             52
#define SNOW		53
#define ASH		54
#define SAND	55
#define PAVEDFLOOR	56
#define HIGHWAY		57
#define GRASSLAND		58
#define NETHERMIST	59
#define STALACTITE	60
#define CRYPTFLOOR	61
#define DRAWBRIDGE_DOWN 62
#define AIR             63
#define CLOUD           64
#define BUBBLES		65
#define RAINCLOUD		66

#define MAX_TYPE		67
#define INVALID_TYPE	127

/*
 * Avoid using the level types in inequalities:
 * these types are subject to change.
 * Instead, use one of the macros below.
 */
#define IS_WALL(typ)	((typ) && (typ) <= TUNNELWALL)
#define IS_STWALL(typ)	((typ) <= TUNNELWALL)	/* STONE <= (typ) <= TUNNELWALL */
#define IS_RECOLORWALL(typ)	((typ) && (typ) <= DBWALL)
#define IS_ROCK(typ)	((typ) < POOL)		/* absolutely nonaccessible */
#define IS_ROCKWFL(typ)	(((typ) < POOL) && typ != FARMLAND)		/* absolutely nonaccessible */
#define IS_DOOR(typ)	((typ) == DOOR)
#define IS_STONE(typ)	((typ) == STONE)
#define IS_IRONBAR(typ)	((typ) == IRONBARS)
#define IS_TREE(typ)	((typ) == TREE || \
			(level.flags.arboreal && (typ) == STONE))
#define ACCESSIBLE(typ) ((typ) >= DOOR)		/* good position */
#define IS_ROOM(typ)	((typ) >= ROOM)		/* ROOM, STAIRS, furniture.. */
#define ZAP_POS(typ)	((typ) >= POOL || (typ) == TUNNELWALL || (typ) == FARMLAND)
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
#define IS_FURNITURE(typ) ((typ) >= STAIRS && (typ) <= STRAWMATTRESS)
#define IS_AIR(typ)	((typ) >= AIR && (typ) <= RAINCLOUD)
#define IS_SOFT(typ)	( ((typ) >= AIR && (typ) <= RAINCLOUD) || (typ) == SAND || (typ) == SNOW || (typ) == GRASSLAND || IS_POOL(typ))
#define IS_GRAVEWALL(typ)	((typ) == GRAVEWALL)
#define IS_TUNNELWALL(typ)	((typ) == TUNNELWALL)
#define IS_ROCKWALL(typ)	((typ) == ROCKWALL)
#define IS_DIGGABLEWALL(typ)	((typ) >= ROCKWALL && (typ) <= TUNNELWALL)
#define IS_FARMLAND(typ)	((typ) == FARMLAND)
#define IS_MOUNTAIN(typ)	((typ) == MOUNTAIN)
#define IS_WATERTUNNEL(typ)	((typ) == WATERTUNNEL)
#define IS_CRYSTALWATER(typ)	((typ) == CRYSTALWATER)
#define IS_MOORLAND(typ)	((typ) == MOORLAND)
#define IS_URINELAKE(typ)	((typ) == URINELAKE)
#define IS_SHIFTINGSAND(typ)	((typ) == SHIFTINGSAND)
#define IS_STYXRIVER(typ)	((typ) == STYXRIVER)
#define IS_PENTAGRAM(typ)	((typ) == PENTAGRAM)
#define IS_WELL(typ)	((typ) == WELL)
#define IS_POISONEDWELL(typ)	((typ) == POISONEDWELL)
#define IS_WAGON(typ)	((typ) == WAGON)
#define IS_BURNINGWAGON(typ)	((typ) == BURNINGWAGON)
#define IS_WOODENTABLE(typ)	((typ) == WOODENTABLE)
#define IS_CARVEDBED(typ)	((typ) == CARVEDBED)
#define IS_STRAWMATTRESS(typ)	((typ) == STRAWMATTRESS)
#define IS_SNOW(typ)	((typ) == SNOW)
#define IS_ASH(typ)	((typ) == ASH)
#define IS_SAND(typ)	((typ) == SAND)
#define IS_PAVEDFLOOR(typ)	((typ) == PAVEDFLOOR)
#define IS_HIGHWAY(typ)	((typ) == HIGHWAY)
#define IS_GRASSLAND(typ)	((typ) == GRASSLAND)
#define IS_NETHERMIST(typ)	((typ) == NETHERMIST)
#define IS_STALACTITE(typ)	((typ) == STALACTITE)
#define IS_CRYPTFLOOR(typ)	((typ) == CRYPTFLOOR)
#define IS_BUBBLES(typ)	((typ) == BUBBLES)
#define IS_RAINCLOUD(typ)	((typ) == RAINCLOUD)
#define IS_CLOUD(typ)	((typ) == CLOUD)

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
#define S_rockwall 12
#define S_gravewall 13
#define S_tunnelwall 14
#define S_ndoor		15
#define S_vodoor	16
#define S_hodoor	17
#define S_vcdoor	18	/* closed door, vertical wall */
#define S_hcdoor	19	/* closed door, horizontal wall */
#define S_bars		20	/* Added by KMH */
#define S_tree		21	/* Added by KMH */
#define S_farmland	22
#define S_mountain	23
#define S_room		24
#define S_darkroom		25
#define S_corr		26
#define S_litcorr	27
#define S_upstair	28
#define S_dnstair	29
#define S_upladder	30
#define S_dnladder	31
#define S_altar		32
#define S_grave		33
#define S_throne	34
#define S_sink		35
#define S_toilet	36
#define S_fountain	37
#define S_pool		38
#define S_ice		39
#define S_lava		40
#define S_vodbridge	41
#define S_hodbridge	42
#define S_vcdbridge	43	/* closed drawbridge, vertical wall */
#define S_hcdbridge	44	/* closed drawbridge, horizontal wall */
#define S_air		45
#define S_cloud		46
#define S_dungwall		47
#define S_water		48
#define S_watertunnel	49
#define S_crystalwater	50
#define S_moorland	51
#define S_urinelake	52
#define S_shiftingsand	53
#define S_styxriver	54
#define S_well	55
#define S_poisonedwell	56
#define S_wagon	57
#define S_burningwagon	58
#define S_woodentable	59
#define S_strawmattress	60
#define S_snow	61
#define S_ash	62
#define S_sand	63
#define S_pavedfloor	64
#define S_highway		65
#define S_grassland	66
#define S_nethermist	67
#define S_stalactite	68
#define S_cryptfloor	69
#define S_bubbles		70
#define S_raincloud	71
#define S_pentagram	72
#define S_carvedbed	73
#define S_grayglyph		74


/* end dungeon characters, begin traps */

#define S_arrow_trap		75
#define S_dart_trap		76
#define S_falling_rock_trap	77
#define S_squeaky_board		78
#define S_bear_trap		79
#define S_land_mine		80
#define S_rolling_boulder_trap	81
#define S_sleeping_gas_trap	82
#define S_rust_trap		83
#define S_fire_trap		84
#define S_pit			85
#define S_spiked_pit		86
#define S_hole			87
#define S_trap_door		88
#define S_teleportation_trap	89
#define S_level_teleporter	90
#define S_magic_portal		91
#define S_web			92
#define S_statue_trap		93
#define S_magic_trap		94
#define S_anti_magic_trap	95
#define S_polymorph_trap	96
#define S_ice_trap		97
#define S_spear_trap		98
#define S_falling_rocks_trap		99
#define S_magic_beam_trap		100
#define S_pile_of_shit		101
#define S_animation_trap		102
#define S_glyph_of_warding		103
#define S_scything_blade		104
#define S_bolt_trap		105
#define S_acid_pool		106
#define S_water_pool		107
#define S_poison_gas_trap		108
#define S_slow_gas_trap		109
#define S_shock_trap		110
#define S_rmb_loss_trap		111
#define S_display_trap		112
#define S_spell_loss_trap		113
#define S_yellow_spell_trap		114
#define S_auto_destruct_trap		115
#define S_memory_trap		116
#define S_inventory_trap		117
#define S_black_ng_wall_trap		118
#define S_menu_trap		119
#define S_speed_trap		120
#define S_superscroller_trap		121
#define S_active_superscroller_trap		122
#define S_swarm_trap		123
#define S_automatic_switcher		124
#define S_unknown_trap		125
#define S_heel_trap		126
#define S_vuln_trap		127

#define S_lava_trap		128
#define S_flood_trap		129
#define S_drain_trap		130
#define S_free_hand_trap		131
#define S_disintegration_trap		132
#define S_unidentify_trap		133
#define S_thirst_trap		134
#define S_luck_trap		135
#define S_shades_of_grey_trap		136
#define S_item_telep_trap		137
#define S_gravity_trap		138
#define S_death_trap		139
#define S_stone_to_flesh_trap		140
#define S_quicksand_trap		141
#define S_faint_trap		142
#define S_curse_trap		143
#define S_difficulty_trap		144
#define S_sound_trap		145
#define S_caster_trap		146
#define S_weakness_trap		147
#define S_rot_thirteen_trap		148
#define S_bishop_trap		149
#define S_confusion_trap		150
#define S_nupesell_trap		151
#define S_drop_trap		152
#define S_dstw_trap		153
#define S_status_trap		154
#define S_alignment_trap		155
#define S_stairs_trap		156

#define S_pestilence_trap		157
#define S_famine_trap		158

#define S_relocation_trap		159

#define S_uninformation_trap		160

#define S_intrinsic_loss_trap	161
#define S_blood_loss_trap	162
#define S_bad_effect_trap	163
#define S_multiplying_trap	164
#define S_auto_vuln_trap	165
#define S_tele_items_trap	166
#define S_nastiness_trap	167

#define S_recursion_trap	168
#define S_respawn_trap	169
#define S_warp_zone	170
#define S_captcha_trap	171
#define S_mind_wipe_trap	172
#define S_shuriken_trap	173
#define S_shit_pit	174
#define S_shaft_trap	175
#define S_lock_trap	176
#define S_mc_trap	177
#define S_farlook_trap	178
#define S_gateway_trap	179
#define S_growing_trap	180
#define S_cooling_trap	181
#define S_bar_trap	182
#define S_locking_trap	183
#define S_air_trap	184
#define S_terrain_trap	185

#define S_loudspeaker	186
#define S_nest_trap	187
#define S_bullet_trap	188
#define S_paralysis_trap	189
#define S_cyanide_trap	190
#define S_laser_trap	191
#define S_giant_chasm	192
#define S_fart_trap	193
#define S_confuse_trap	194
#define S_stun_trap	195
#define S_hallucination_trap	196
#define S_petrification_trap	197
#define S_numbness_trap	198
#define S_freezing_trap	199
#define S_burning_trap	200
#define S_fear_trap	201
#define S_blindness_trap	202
#define S_glib_trap	203
#define S_slime_trap	204
#define S_inertia_trap	205
#define S_time_trap	206
#define S_lycanthropy_trap	207
#define S_unlight_trap	208
#define S_elemental_trap	209
#define S_escalating_trap	210
#define S_negative_trap	211
#define S_mana_trap	212
#define S_sin_trap	213
#define S_destroy_armor_trap	214
#define S_divine_anger_trap	215
#define S_genetic_trap	216
#define S_missingno_trap	217
#define S_cancellation_trap	218
#define S_hostility_trap	219
#define S_boss_trap	220
#define S_wishing_trap	221
#define S_recurring_amn_trap	222
#define S_bigscript_trap	223
#define S_bank_trap	224
#define S_only_trap	225
#define S_map_trap	226
#define S_tech_trap	227
#define S_disenchant_trap	228
#define S_verisiert	229
#define S_chaos_trap	230
#define S_muteness_trap	231
#define S_ntll_trap	232
#define S_engraving_trap	233
#define S_magic_device_trap	234
#define S_book_trap	235
#define S_level_trap	236
#define S_quiz_trap	237

#define S_falling_boulder_trap	238
#define S_glass_arrow_trap	239
#define S_glass_bolt_trap	240
#define S_out_of_magic_trap	241
#define S_plasma_trap	242
#define S_bomb_trap	243
#define S_earthquake_trap	244
#define S_noise_trap	245
#define S_glue_trap	246
#define S_guillotine_trap	247
#define S_bisection_trap	248
#define S_volt_trap	249
#define S_horde_trap	250
#define S_immobility_trap	251
#define S_green_glyph_trap	252
#define S_blue_glyph_trap	253
#define S_yellow_glyph_trap	254
#define S_orange_glyph_trap	255
#define S_black_glyph_trap	256
#define S_purple_glyph_trap	257

#define S_metabolic_trap	258
#define S_no_return_trap	259
#define S_ego_trap	260
#define S_fast_forward_trap	261
#define S_rotten_trap	262
#define S_unskilled_trap	263
#define S_low_stats_trap	264
#define S_training_trap	265
#define S_exercise_trap	266
#define S_falling_loadstone_trap	267
#define S_summon_undead_trap	268
#define S_falling_nastystone_trap	269

#define S_spined_ball_trap	270
#define S_pendulum_trap	271
#define S_turn_table	272
#define S_scent_trap	273
#define S_banana_trap	274
#define S_falling_tub_trap	275
#define S_alarm	276
#define S_caltrops_trap	277
#define S_blade_wire	278
#define S_magnet_trap	279
#define S_slingshot_trap	280
#define S_cannon_trap	281
#define S_venom_sprinkler	282
#define S_fumarole	283

#define S_mana_pit	284
#define S_elemental_portal	285
#define S_girliness_trap	286
#define S_fumbling_trap	287
#define S_egomonster_trap	288
#define S_flooding_trap	289
#define S_monster_cube	290
#define S_cursed_grave	291
#define S_limitation_trap	292
#define S_weak_sight_trap	293
#define S_random_message_trap	294

#define S_desecration_trap 295
#define S_starvation_trap 296
#define S_dropless_trap 297
#define S_low_effect_trap 298
#define S_invisible_trap 299
#define S_ghost_world_trap 300
#define S_dehydration_trap 301
#define S_hate_trap 302
#define S_spacewars_trap 303
#define S_temporary_recursion_trap 304
#define S_totter_trap 305
#define S_nonintrinsical_trap 306
#define S_dropcurse_trap 307
#define S_nakedness_trap 308
#define S_antilevel_trap 309
#define S_ventilator 310
#define S_stealer_trap 311
#define S_rebellion_trap 312
#define S_crap_trap 313
#define S_misfire_trap 314
#define S_trap_of_walls 315

#define S_disconnect_trap 316
#define S_interface_screw_trap 317
#define S_dimness_trap 318
#define S_evil_artifact_trap 319
#define S_bossfight_trap 320
#define S_entire_level_trap 321
#define S_bones_trap 322
#define S_rodney_trap 323
#define S_autocurse_trap 324
#define S_highlevel_trap 325
#define S_spell_forgetting_trap 326
#define S_sound_effect_trap 327

#define S_trap_percents		328

#define S_nexus_trap		329
#define S_leg_trap		330
#define S_artifact_jackpot_trap		331
#define S_map_amnesia_trap		332
#define S_spreading_trap		333
#define S_adjacent_trap		334
#define S_superthing_trap		335
#define S_current_shaft		336
#define S_levitation_trap		337
#define S_bowel_cramps_trap		338
#define S_unequipping_trap		339
#define S_good_artifact_trap		340
#define S_gender_trap		341
#define S_trap_of_oa		342
#define S_sincount_trap		343
#define S_beamer_trap		344
#define S_level_beamer		345
#define S_pet_trap		346
#define S_piercing_beam_trap		347
#define S_wrenching_trap		348
#define S_tracker_trap		349
#define S_nurse_trap		350
#define S_back_to_start_trap		351
#define S_nemesis_trap		352
#define S_strew_trap		353
#define S_outta_depth_trap		354
#define S_punishment_trap		355
#define S_boon_trap		356
#define S_fountain_trap		357
#define S_throne_trap		358
#define S_anoxic_pit		359
#define S_arabella_speaker		360
#define S_femmy_trap		361
#define S_madeleine_trap		362
#define S_marlena_trap		363
#define S_anastasia_trap		364
#define S_filler_trap		365
#define S_toxic_venom_trap		366
#define S_insanity_trap		367
#define S_madness_trap		368
#define S_jessica_trap		369
#define S_solvejg_trap		370
#define S_wendy_trap		371
#define S_katharina_trap		372
#define S_elena_trap		373
#define S_thai_trap		374
#define S_elif_trap		375
#define S_nadja_trap		376
#define S_sandra_trap		377
#define S_natalje_trap		378
#define S_jeanetta_trap		379
#define S_yvonne_trap		380
#define S_maurah_trap		381

#define S_lootcut_trap	382
#define S_monster_speed_trap	383
#define S_scaling_trap	384
#define S_enmity_trap	385
#define S_white_spell_trap	386
#define S_gray_spell_trap	387
#define S_quasar_trap	388
#define S_momma_trap	389
#define S_horror_trap	390
#define S_artificer_trap	391
#define S_wereform_trap	392
#define S_nonprayer_trap	393
#define S_evil_patch_trap	394
#define S_hard_mode_trap	395
#define S_secret_attack_trap	396
#define S_eater_trap	397
#define S_covetous_trap	398
#define S_not_seen_trap	399
#define S_dark_mode_trap	400
#define S_antisearch_trap	401
#define S_homicide_trap	402
#define S_nasty_nation_trap	403
#define S_wakeup_call_trap	404
#define S_grayout_trap	405
#define S_gray_center_trap	406
#define S_checkerboard_trap	407
#define S_clockwise_spin_trap	408
#define S_counterclockwise_trap	409
#define S_lag_trap	410
#define S_blesscurse_trap	411
#define S_delight_trap	412
#define S_discharge_trap	413
#define S_trashing_trap	414
#define S_filtering_trap	415
#define S_deformatting_trap	416
#define S_flicker_strip_trap	417
#define S_undressing_trap	418
#define S_hyperbluewall_trap	419
#define S_nolite_trap	420
#define S_paranoia_trap	421
#define S_fleecescript_trap	422
#define S_interrupt_trap	423
#define S_dustbin_trap	424
#define S_mana_battery_trap	425
#define S_monsterfingers_trap	426
#define S_miscast_trap	427
#define S_message_suppression_trap	428
#define S_stuck_announcement_trap	429
#define S_bloodthirsty_trap	430
#define S_maximum_damage_trap	431
#define S_latency_trap	432
#define S_starlit_trap	433
#define S_knowledge_trap	434
#define S_highscore_trap	435
#define S_pink_spell_trap	436
#define S_green_spell_trap	437
#define S_evc_trap	438
#define S_underlayer_trap	439
#define S_damage_meter_trap	440
#define S_arbitrary_weight_trap	441
#define S_fucked_info_trap	442
#define S_black_spell_trap	443
#define S_cyan_spell_trap	444
#define S_heap_trap	445
#define S_blue_spell_trap	446
#define S_tron_trap	447
#define S_red_spell_trap	448
#define S_too_heavy_trap	449
#define S_elongation_trap	450
#define S_wrapover_trap	451
#define S_destruction_trap	452
#define S_melee_prefix_trap	453
#define S_automore_trap	454
#define S_unfair_attack_trap	455

#define S_kop_cube	456
#define S_boss_spawner	457
#define S_contamination_trap	458

#define S_evil_heel_trap	459
#define S_bad_equipment_trap	460
#define S_tempoconflict_trap	461
#define S_tempohunger_trap	462
#define S_teleportitis_trap	463
#define S_polymorphitis_trap	464
#define S_premature_death_trap	465
#define S_lasting_amnesia_trap	466
#define S_ragnarok_trap	467
#define S_single_disenchant_trap	468
#define S_acid_pit	469

#define S_severe_disenchant_trap	470
#define S_pain_trap	471
#define S_trembling_trap	472
#define S_techcap_trap	473
#define S_spell_memory_trap	474
#define S_skill_reduction_trap	475
#define S_skillcap_trap	476
#define S_permanent_stat_damage_trap	477

#define S_orange_spell_trap	478
#define S_violet_spell_trap	479
#define S_trap_of_longing	480
#define S_cursed_part_trap	481
#define S_quaversal_trap	482
#define S_appearance_shuffling_trap	483
#define S_brown_spell_trap	484
#define S_choiceless_trap	485
#define S_goldspell_trap	486
#define S_deprovement_trap	487
#define S_initialization_trap	488
#define S_gushlush_trap	489
#define S_soiltype_trap	490
#define S_dangerous_terrain_trap	491
#define S_fallout_trap	492
#define S_mojibake_trap	493
#define S_gravation_trap	494
#define S_uncalled_trap	495
#define S_exploding_dice_trap	496
#define S_permacurse_trap	497
#define S_shrouded_identity_trap	498
#define S_feeler_gauges_trap	499
#define S_long_screwup_trap	500
#define S_wing_yellow_changer	501
#define S_life_saving_trap	502
#define S_curseuse_trap	503
#define S_cut_nutrition_trap	504
#define S_skill_loss_trap	505
#define S_autopilot_trap	506
#define S_force_trap	507
#define S_monster_glyph_trap	508
#define S_changing_directive_trap	509
#define S_container_kaboom_trap	510
#define S_steal_degrade_trap	511
#define S_left_inventory_trap	512
#define S_fluctuating_speed_trap	513
#define S_tarmustrokingnora_trap	514
#define S_failure_trap	515
#define S_bright_cyan_spell_trap	516
#define S_frequentation_spawn_trap	517
#define S_pet_ai_trap	518
#define S_satan_trap	519
#define S_rememberance_trap	520
#define S_pokelie_trap	521
#define S_autopickup_trap	522
#define S_dywypi_trap	523
#define S_silver_spell_trap	524
#define S_metal_spell_trap	525
#define S_platinum_spell_trap	526
#define S_manler_trap	527
#define S_doorning_trap	528
#define S_nownsible_trap	529
#define S_elm_street_trap	530
#define S_monnoise_trap	531
#define S_rang_call_trap	532
#define S_recurring_spell_loss_trap	533
#define S_antitraining_trap	534
#define S_techout_trap	535
#define S_stat_decay_trap	536
#define S_movemork_trap	537

#define S_hybrid_trap	538
#define S_shapechange_trap	539
#define S_meltem_trap	540
#define S_miguc_trap	541
#define S_directive_trap	542
#define S_satatue_trap	543
#define S_farting_web	544
#define S_cataclysm_trap	545
#define S_data_delete_trap	546
#define S_elder_tentacling_trap	547
#define S_footerer_trap	548

#define S_bad_part_trap		549
#define S_completely_bad_part_trap		550
#define S_evil_variant_trap		551

#define S_grave_wall_trap		552
#define S_tunnel_trap		553
#define S_farmland_trap		554
#define S_mountain_trap		555
#define S_water_tunnel_trap		556
#define S_crystal_flood_trap		557
#define S_moorland_trap		558
#define S_urine_trap		559
#define S_shifting_sand_trap		560
#define S_styx_trap		561
#define S_pentagram_trap		562
#define S_snow_trap		563
#define S_ash_trap		564
#define S_sand_trap		565
#define S_pavement_trap		566
#define S_highway_trap		567
#define S_grassland_trap		568
#define S_nether_mist_trap		569
#define S_stalactite_trap		570
#define S_cryptfloor_trap		571
#define S_bubble_trap		572
#define S_rain_cloud_trap		573
#define S_item_nastification_trap		574
#define S_sanity_increase_trap		575
#define S_psi_trap		576
#define S_gay_trap		577
#define S_sarah_trap		578
#define S_claudia_trap		579
#define S_ludgera_trap		580
#define S_kati_trap		581
#define S_sanity_treble_trap		582
#define S_stat_decrease_trap		583
#define S_simeout_trap		584
#define S_spressing_trap		585

#define S_nelly_trap		586
#define S_eveline_trap		587
#define S_karin_trap		588
#define S_juen_trap		589
#define S_kristina_trap		590
#define S_lou_trap		591
#define S_almut_trap		592
#define S_julietta_trap		593
#define S_arabella_trap		594

#define S_kristin_trap		595
#define S_anna_trap		596
#define S_ruea_trap		597
#define S_dora_trap		598
#define S_marike_trap		599
#define S_jette_trap		600
#define S_ina_trap		601
#define S_sing_trap		602
#define S_victoria_trap		603
#define S_melissa_trap		604
#define S_anita_trap		605
#define S_henrietta_trap		606
#define S_verena_trap		607

#define S_annemarie_trap		608
#define S_jil_trap		609
#define S_jana_trap		610
#define S_katrin_trap		611
#define S_gudrun_trap		612
#define S_ella_trap		613
#define S_manuela_trap		614
#define S_jennifer_trap		615
#define S_patricia_trap		616
#define S_antje_trap		617
#define S_antje_x_trap		618
#define S_kerstin_trap		619
#define S_laura_trap		620
#define S_larissa_trap		621
#define S_nora_trap		622
#define S_natalia_trap		623
#define S_susanne_trap		624
#define S_lisa_trap		625
#define S_bridghitte_trap		626
#define S_julia_trap		627
#define S_nicole_trap		628
#define S_rita_trap		629
#define S_janina_trap		630
#define S_rosa_trap		631

#define S_ksenia_trap		632
#define S_lydia_trap		633
#define S_conny_trap		634
#define S_katia_trap		635
#define S_mariya_trap		636
#define S_elise_trap		637
#define S_ronja_trap		638
#define S_ariane_trap		639
#define S_johanna_trap		640
#define S_inge_trap		641

#define S_wall_trap		642
#define S_monster_generator		643
#define S_potion_dispenser		644
#define S_spacewars_spawn_trap		645
#define S_tv_tropes_trap		646
#define S_symbiote_trap		647
#define S_kill_symbiote_trap		648
#define S_symbiote_replacement_trap		649
#define S_shutdown_trap		650
#define S_corona_trap		651
#define S_unproofing_trap		652
#define S_visibility_trap		653
#define S_feminism_stone_trap		654
#define S_branch_teleporter		655
#define S_branch_beamer		656
#define S_poison_arrow_trap		657
#define S_poison_bolt_trap		658
#define S_mace_trap		659
#define S_shueft_trap		660
#define S_moth_larvae_trap		661
#define S_worthiness_trap		662
#define S_conduct_trap		663
#define S_strikethrough_trap		664
#define S_multiple_gather_trap		665
#define S_vivisection_trap		666
#define S_instafeminism_trap		667
#define S_instanasty_trap		668
#define S_skill_point_loss_trap		669
#define S_perfect_match_trap		670
#define S_dumbie_lightsaber_trap		671
#define S_wrong_stairs		672
#define S_techstop_trap		673
#define S_amnesia_switch_trap		674
#define S_skill_swap_trap		675
#define S_skill_upordown_trap		676
#define S_skill_randomize_trap		677

#define S_skill_multiply_trap		678
#define S_trapwalk_trap		679
#define S_cluster_trap		680
#define S_field_trap		681
#define S_monicide_trap		682
#define S_trap_creation_trap		683
#define S_leold_trap		684
#define S_animeband_trap		685
#define S_perfume_trap		686
#define S_court_trap		687
#define S_elder_scrolls_trap		688
#define S_joke_trap		689
#define S_dungeon_lords_trap		690
#define S_fortytwo_trap		691
#define S_randomize_trap		692
#define S_evilroom_trap		693
#define S_aoe_trap		694
#define S_elona_trap		695
#define S_religion_trap		696
#define S_steamband_trap		697
#define S_hardcore_trap		698
#define S_machine_trap		699
#define S_bee_trap		700
#define S_migo_trap		701
#define S_angband_trap		702
#define S_dnethack_trap		703
#define S_evil_spawn_trap		704
#define S_shoe_trap		705
#define S_inside_trap		706
#define S_doom_trap		707
#define S_military_trap		708
#define S_illusion_trap		709
#define S_diablo_trap		710

#define S_giant_explorer_trap		711
#define S_trapwarp_trap		712
#define S_yawm_trap		713
#define S_cradle_of_chaos_trap		714
#define S_tez_trap		715
#define S_enthu_trap		716
#define S_mikra_trap		717
#define S_gots_too_good_trap		718
#define S_killer_room_trap		719
#define S_no_fun_walls_trap		720

#define S_ruth_trap		721
#define S_magdalena_trap		722
#define S_marleen_trap		723
#define S_klara_trap		724
#define S_friederike_trap		725
#define S_naomi_trap		726
#define S_ute_trap		727
#define S_jasieen_trap		728
#define S_yasaman_trap		729
#define S_may_britt_trap		730

#define S_calling_out_trap		731
#define S_field_break_trap		732
#define S_tenth_trap		733
#define S_debt_trap		734
#define S_inversion_trap		735
#define S_wince_trap		736
#define S_u_have_been_trap		737
#define S_really_bad_trap		738
#define S_covid_trap		739
#define S_artiblast_trap		740

#define S_persistent_fart_trap		741
#define S_attacking_heel_trap		742
#define S_trap_teleporter		743
#define S_alignment_trash_trap		744
#define S_reshuffle_trap		745
#define S_musehand_trap		746
#define S_dogside_trap		747
#define S_bankrupt_trap		748
#define S_fillup_trap		749
#define S_airstrike_trap		750
#define S_dynamite_trap		751
#define S_malevolence_trap		752
#define S_leaflet_trap		753
#define S_tentadeep_trap		754
#define S_stathalf_trap		755
#define S_cutstat_trap		756
#define S_rare_spawn_trap		757
#define S_you_are_an_idiot_trap		758
#define S_nastycurse_trap		759
#define S_repeating_nastycurse_trap		760

#define S_nadine_trap		761
#define S_luisa_trap		762
#define S_irina_trap		763
#define S_liselotte_trap		764
#define S_greta_trap		765
#define S_jane_trap		766
#define S_sue_lyn_trap		767
#define S_charlotte_trap		768
#define S_hannah_trap		769
#define S_little_marie_trap		770

#define S_real_lie_trap		771
#define S_escape_past_trap		772
#define S_pethate_trap		773
#define S_pet_lashout_trap		774
#define S_petstarve_trap		775
#define S_petscrew_trap		776
#define S_tech_loss_trap		777
#define S_proofloss_trap		778
#define S_un_invis_trap		779
#define S_detectation_trap		780

#define S_option_trap		781
#define S_miscolor_trap		782
#define S_one_rainbow_trap		783
#define S_colorshift_trap		784
#define S_top_line_trap		785
#define S_caps_trap		786
#define S_un_knowledge_trap		787
#define S_darkhance_trap		788
#define S_dschueueuet_trap		789
#define S_nopeskill_trap		790
#define S_fuckfuckfuck_trap		791

#define S_burden_trap		792
#define S_magic_vacuum_trap		793

#define S_tanja_trap		794
#define S_sonja_trap		795
#define S_rhea_trap		796
#define S_lara_trap		797

#define S_fuck_over_trap	798

#define S_dagger_trap		799
#define S_razor_trap		800
#define S_phosgene_trap		801
#define S_chloroform_trap		802
#define S_corrosion_trap		803
#define S_flame_trap		804
#define S_wither_trap		805
#define S_phaseport_trap		806
#define S_phasebeam_trap		807
#define S_vulnerate_trap		808

#define S_epvi_trap		809

#define S_sabrina_trap		810

#define S_aefde_trap		811

#define S_falling_rock_cold_trap		812
#define S_return_trap		813
#define S_intrinsic_steal_trap		814
#define S_score_axe_trap		815
#define S_score_drain_trap		816
#define S_single_unidentify_trap		817
#define S_hypoxic_pit		818
#define S_unlucky_trap		819
#define S_alignment_reduction_trap		820
#define S_malignant_trap		821
#define S_stat_damage_trap		822
#define S_half_memory_trap		823
#define S_half_training_trap		824
#define S_debuff_trap		825
#define S_trip_once_trap		826
#define S_narcolepsy_trap		827
#define S_martial_arts_trap		828
#define S_mean_burden_trap		829
#define S_carrcap_trap		830
#define S_umeng_trap		831

#define S_timerun_trap		832

/* end traps, begin special effects */

#define S_vbeam		833	/* The 4 zap beam symbols.  Do NOT separate. */
#define S_hbeam		834	/* To change order or add, see function     */
#define S_lslant	835	/* zapdir_to_glyph() in display.c.	    */
#define S_rslant	836
#define S_digbeam	837	/* dig beam symbol */
#define S_flashbeam	838	/* camera flash symbol */
#define S_boomleft	839	/* thrown boomerang, open left, e.g ')'    */
#define S_boomright	840	/* thrown boomerand, open right, e.g. '('  */
#define S_ss1		841	/* 4 magic shield glyphs */
#define S_ss2		842
#define S_ss3		843
#define S_ss4		844

/* The 8 swallow symbols.  Do NOT separate.  To change order or add, see */
/* the function swallow_to_glyph() in display.c.			 */
#define S_sw_tl		845	/* swallow top left [1]			*/
#define S_sw_tc		846	/* swallow top center [2]	Order:	*/
#define S_sw_tr		847	/* swallow top right [3]		*/
#define S_sw_ml		848	/* swallow middle left [4]	1 2 3	*/
#define S_sw_mr		849	/* swallow middle right [6]	4 5 6	*/
#define S_sw_bl		850	/* swallow bottom left [7]	7 8 9	*/
#define S_sw_bc		851	/* swallow bottom center [8]		*/
#define S_sw_br		852	/* swallow bottom right [9]		*/

#define S_explode1	853	/* explosion top left			*/
#define S_explode2	854	/* explosion top center			*/
#define S_explode3	855	/* explosion top right		 Ex.	*/
#define S_explode4	856	/* explosion middle left		*/
#define S_explode5	857	/* explosion middle center	 /-\	*/
#define S_explode6	858	/* explosion middle right	 |@|	*/
#define S_explode7	859	/* explosion bottom left	 \-/	*/
#define S_explode8	860	/* explosion bottom center		*/
#define S_explode9	861	/* explosion bottom right		*/
  
/* end effects */
  
#define MAXPCHARS	862 	/* maximum number of mapped characters */
#define MAXDCHARS	75	/* maximum of mapped dungeon characters */
#define MAXTCHARS	758	/* maximum of mapped trap characters */
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
#define CURS_GRAPHICS   4   /* Portable curses drawing characters */

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
#define W_EASYGROWTH	0x20 /* set this if a wall without W_HARDGROWTH is dug out */
#define W_HARDGROWTH	0x40 /* set this if a tile without W_EASYGROWTH is made into a new wall */
#define W_QUASAROK	0x80 /* tile will be visible while quasar effect is active */

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
	Bitfield(mem_bg,7);	/* Remembered background */
	Bitfield(mem_trap,16);	/* Remembered trap */
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
	Bitfield(flagsX,8);	/* extra information for typ */
	Bitfield(horizontal,1); /* wall/door/etc is horiz. (more typ info) */
	Bitfield(lit,1);	/* speed hack for lit rooms */
	Bitfield(waslit,1);	/* remember if a location was lit */
	Bitfield(roomno,6);	/* room # for special rooms */
	Bitfield(edge,1);	/* marks boundaries for special rooms*/
	Bitfield(fleecycolor,8);	/* mainly for corridors --Amy */
	Bitfield(nofunwall,8);	/* for "no fun walls" nastytrap --Amy */
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
#define wall_info	flagsX
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

	Bitfield(has_cryptroom, 1);
	Bitfield(has_troublezone, 1);
	Bitfield(has_weaponchamber, 1);
	Bitfield(has_hellpit, 1);
	Bitfield(has_feminismroom, 1);
	Bitfield(has_meadowroom, 1);
	Bitfield(has_coolingchamber, 1);
	Bitfield(has_voidroom, 1);
	Bitfield(has_armory, 1);
	Bitfield(has_hamletroom, 1);
	Bitfield(has_kopstation, 1);
	Bitfield(has_bossroom, 1);
	Bitfield(has_rngcenter, 1);
	Bitfield(has_wizardsdorm, 1);
	Bitfield(has_doomedbarracks, 1);
	Bitfield(has_sleepingroom, 1);
	Bitfield(has_diverparadise, 1);
	Bitfield(has_menagerie, 1);
	Bitfield(has_nastycentral, 1);
	Bitfield(has_emptydesert, 1);
	Bitfield(has_rarityroom, 1);
	Bitfield(has_exhibitroom, 1);
	Bitfield(has_prisonchamber, 1);
	Bitfield(has_nuclearchamber, 1);
	Bitfield(has_levelseventyroom, 1);
	Bitfield(has_variantroom, 1);

	Bitfield(has_evilroom, 1);
	Bitfield(has_religioncenter, 1);
	Bitfield(has_cursedmummyroom, 1);
	Bitfield(has_arduousmountain, 1);
	Bitfield(has_changingroom, 1);
	Bitfield(has_questorroom, 1);
	Bitfield(has_levelffroom, 1);
	Bitfield(has_verminroom, 1);
	Bitfield(has_miraspa, 1);
	Bitfield(has_machineroom, 1);
	Bitfield(has_showerroom, 1);
	Bitfield(has_greencrossroom, 1);
	Bitfield(has_ruinedchurch, 1);
	Bitfield(has_gamecorner, 1);
	Bitfield(has_illusionroom, 1);
	Bitfield(has_robbercave, 1);
	Bitfield(has_sanitationcentral, 1);
	Bitfield(has_playercentral, 1);
	Bitfield(has_casinoroom, 1);

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
#define place_worm_seg(m,x,y)	level.monsters[x][y] = m
#define remove_monster(x,y)	level.monsters[x][y] = (struct monst *)0
#define m_at(x,y)		(MON_AT(x,y) ? level.monsters[x][y] : \
						(struct monst *)0)
#define m_buried_at(x,y)	(MON_BURIED_AT(x,y) ? level.monsters[x][y] : \
						       (struct monst *)0)

#endif /* RM_H */
