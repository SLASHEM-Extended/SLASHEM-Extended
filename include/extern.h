/*	SCCS Id: @(#)extern.h	3.4	2003/03/10	*/
/* Copyright (c) Steve Creps, 1988.				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef EXTERN_H
#define EXTERN_H

#define E extern

/* ### alloc.c ### */

#if 0
E long *FDECL(alloc, (unsigned int));
#endif
E char *FDECL(fmt_ptr, (const genericptr,char *));

/* This next pre-processor directive covers almost the entire file,
 * interrupted only occasionally to pick up specific functions as needed. */
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)

/* ### allmain.c ### */

E void NDECL(moveloop);
E void NDECL(stop_occupation);
E void NDECL(display_gamewindows);
E void NDECL(newgame);
E void FDECL(welcome, (BOOLEAN_P));
#if defined(RECORD_REALTIME) || defined(REALTIME_ON_BOTL)
E time_t NDECL(get_realtime);
#endif
E boolean NDECL(timebasedlowerchance);

/* ### apply.c ### */

E int NDECL(doapply);
E int NDECL(dorub);
E int NDECL(dojump);
E int FDECL(jump, (int));
E int FDECL(jump, (int));
E int NDECL(number_leashed);
E void FDECL(o_unleash, (struct obj *));
E void FDECL(m_unleash, (struct monst *,BOOLEAN_P));
E void NDECL(unleash_all);
E boolean NDECL(next_to_u);
E struct obj *FDECL(get_mleash, (struct monst *));
E void FDECL(check_leash, (XCHAR_P,XCHAR_P));
E boolean FDECL(um_dist, (XCHAR_P,XCHAR_P,XCHAR_P));
E boolean FDECL(snuff_candle, (struct obj *));
E boolean FDECL(snuff_lit, (struct obj *));
E boolean FDECL(catch_lit, (struct obj *));
E void FDECL(use_unicorn_horn, (struct obj *));
E boolean FDECL(tinnable, (struct obj *));
E void NDECL(reset_trapset);
E void FDECL(fig_transform, (genericptr_t, long));
E int FDECL(unfixable_trouble_count,(BOOLEAN_P));
E int FDECL(wand_explode, (struct obj *,BOOLEAN_P));

/* ### artifact.c ### */

E void NDECL(init_artifacts);
E void NDECL(init_artifacts1);
E void FDECL(save_artifacts, (int));
E void FDECL(restore_artifacts, (int));
E const char *FDECL(artiname, (int));
E struct obj *FDECL(mk_artifact, (struct obj *,ALIGNTYP_P));
E const char *FDECL(artifact_name, (const char *,int *));
E boolean FDECL(exist_artifact, (int,const char *));
E void FDECL(artifact_exists, (struct obj *,const char *,BOOLEAN_P));
E int NDECL(nartifact_exist);
E boolean FDECL(spec_ability, (struct obj *,unsigned long));
E boolean FDECL(confers_luck, (struct obj *));
E boolean FDECL(arti_reflects, (struct obj *));
E boolean FDECL(restrict_name, (struct obj *,const char *));
E boolean FDECL(defends, (int,struct obj *));
E boolean FDECL(protects, (int,struct obj *));
E void FDECL(set_artifact_intrinsic, (struct obj *,BOOLEAN_P,long));
E int FDECL(touch_artifact, (struct obj *,struct monst *));
E int FDECL(spec_abon, (struct obj *,struct monst *));
E int FDECL(spec_dbon, (struct obj *,struct monst *,int));
E void FDECL(discover_artifact, (int));
E boolean FDECL(undiscovered_artifact, (int));
E int FDECL(disp_artifact_discoveries, (winid));
E boolean FDECL(artifact_hit, (struct monst *,struct monst *,
				struct obj *,int *,int));
E int NDECL(doinvoke);
E void FDECL(arti_speak, (struct obj *));
E boolean FDECL(artifact_light, (struct obj *));
E int FDECL(artifact_wet, (struct obj *, BOOLEAN_P));
E void FDECL(arti_speak, (struct obj *));
E boolean FDECL(artifact_light, (struct obj *));
E long FDECL(spec_m2, (struct obj *));
E boolean FDECL(artifact_has_invprop, (struct obj *,UCHAR_P));
E long FDECL(arti_cost, (struct obj *));
E void FDECL(arti_poly_contents, (struct obj *));

/* ### attrib.c ### */

E boolean FDECL(adjattrib, (int,int,int));
E void FDECL(change_luck, (SCHAR_P));
E int FDECL(stone_luck, (BOOLEAN_P));
E void NDECL(set_moreluck);
E void FDECL(gainstr, (struct obj *,int));
E void FDECL(losestr, (int));
E void NDECL(restore_attrib);
E void FDECL(exercise, (int,BOOLEAN_P));
E void NDECL(exerchk);
E void NDECL(reset_attribute_clock);
E void FDECL(init_attr, (int));
E void NDECL(redist_attr);
E void FDECL(adjabil, (int,int));
E int NDECL(newhp);
E schar FDECL(acurr, (int));
E schar NDECL(acurrstr);
E void FDECL(adjalign, (int));
/* KMH, balance patch -- new function */
E void NDECL(recalc_health);
E int NDECL(recalc_mana);

/* ### ball.c ### */

E void NDECL(ballfall);
E void NDECL(placebc);
E void NDECL(unplacebc);
E void FDECL(set_bc, (int));
E void FDECL(move_bc, (int,int,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P));
E boolean FDECL(drag_ball, (XCHAR_P,XCHAR_P,
		int *,xchar *,xchar *,xchar *,xchar *, boolean *,BOOLEAN_P));
E void FDECL(drop_ball, (XCHAR_P,XCHAR_P));
E void NDECL(drag_down);

/* ### bones.c ### */

E boolean NDECL(can_make_bones);
E void FDECL(savebones, (struct obj *));
E int NDECL(getbones);

/* ### borg.c ### */

/* E char borg_on;
E char borg_line[80];
E char FDECL(borg_input, (void)); */

/* ### botl.c ### */

E int FDECL(xlev_to_rank, (int));
E int FDECL(title_to_mon, (const char *,int *,int *));
E void NDECL(max_rank_sz);
#ifdef SCORE_ON_BOTL
E long NDECL(botl_score);
#endif
E int FDECL(describe_level, (char *, int));
E const char *NDECL(rank);
E const char *FDECL(rank_of, (int,SHORT_P,BOOLEAN_P));
E void FDECL(bot_set_handler, (void (*)()));
E void NDECL(bot_reconfig);
E void NDECL(bot);
#ifdef DUMP_LOG
E void FDECL(bot1str, (char *));
E void FDECL(bot2str, (char *));
#endif

#if 0
E const char * FDECL(shorten_bot1, (const char *, int));
#endif
#ifdef TTY_GRAPHICS
E const char * FDECL(shorten_bot2, (const char *, unsigned int));
#endif

/* ### cmd.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(doextcmd);
E int NDECL(domonability);
E int NDECL(domonability);
E int NDECL(polyatwill);
E int NDECL(playersteal);
E int NDECL(doprev_message);
E int NDECL(timed_occupation);
E int NDECL(wiz_attributes);
E int NDECL(enter_explore_mode);
# ifdef WIZARD
E int NDECL(wiz_detect);
E int NDECL(wiz_genesis);
E int NDECL(wiz_identify);
E int NDECL(wiz_level_tele);
E int NDECL(wiz_map);
E int NDECL(wiz_where);
E int NDECL(wiz_wish);
# endif /* WIZARD */
#endif /* USE_TRAMPOLI */
E void NDECL(reset_occupations);
E void FDECL(set_occupation, (int (*)(void),const char *,int));
#ifdef REDO
E char NDECL(pgetchar);
E void FDECL(pushch, (CHAR_P));
E void FDECL(savech, (CHAR_P));
#endif
#ifdef WIZARD
E void NDECL(add_debug_extended_commands);
#endif /* WIZARD */
E void FDECL(rhack, (char *));
E int NDECL(doextlist);
E int NDECL(extcmd_via_menu);
E void FDECL(enlightenment, (int));
E void FDECL(show_conduct, (int));
#ifdef DUMP_LOG
E void FDECL(dump_enlightenment, (int));
E void FDECL(dump_conduct, (int));
#endif
E int FDECL(xytod, (SCHAR_P,SCHAR_P));
E void FDECL(dtoxy, (coord *,int));
E int FDECL(movecmd, (CHAR_P));
E int FDECL(getdir, (const char *));
E void NDECL(confdir);
E int FDECL(isok, (int,int));
E int FDECL(get_adjacent_loc, (const char *, const char *, XCHAR_P, XCHAR_P, coord *));
E const char *FDECL(click_to_cmd, (int,int,int));
E char NDECL(readchar);
#ifdef WIZARD
E void NDECL(sanity_check);
#endif
E char FDECL(yn_function, (const char *, const char *, CHAR_P));

/* ### dbridge.c ### */

E boolean FDECL(is_pool, (int,int));
E boolean FDECL(is_lava, (int,int));
E boolean FDECL(is_ice, (int,int));
E int FDECL(is_drawbridge_wall, (int,int));
E boolean FDECL(is_db_wall, (int,int));
E boolean FDECL(find_drawbridge, (int *,int*));
E boolean FDECL(create_drawbridge, (int,int,int,BOOLEAN_P));
E void FDECL(open_drawbridge, (int,int));
E void FDECL(close_drawbridge, (int,int));
E void FDECL(destroy_drawbridge, (int,int));

/* ### decl.c ### */

E void NDECL(decl_init);

/* ### detect.c ### */

E struct obj *FDECL(o_in, (struct obj*,CHAR_P));
E struct obj *FDECL(o_material, (struct obj*,unsigned));
E int FDECL(gold_detect, (struct obj *));
E int FDECL(food_detect, (struct obj *));
E int FDECL(object_detect, (struct obj *,int));
E int FDECL(artifact_detect, (struct obj *));
E int FDECL(monster_detect, (struct obj *,int));
E int FDECL(trap_detect, (struct obj *));
E const char *FDECL(level_distance, (d_level *));
E void FDECL(use_crystal_ball, (struct obj *));
E void NDECL(do_mapping);
E void NDECL(do_vicinity_map);
E void FDECL(cvt_sdoor_to_door, (struct rm *));
#ifdef USE_TRAMPOLI
E void FDECL(findone, (int,int,genericptr_t));
E void FDECL(openone, (int,int,genericptr_t));
#endif
E int NDECL(findit);
E int NDECL(openit);
E void FDECL(find_trap, (struct trap *));
E int FDECL(dosearch0, (int));
E int NDECL(dosearch);
E void NDECL(sokoban_detect);
/* KMH -- Sokoban levels */
E void NDECL(sokoban_detect);

/* ### dig.c ### */

E boolean NDECL(is_digging);
#ifdef USE_TRAMPOLI
E int NDECL(dig);
#endif
E int NDECL(holetime);
E boolean FDECL(dig_check, (struct monst *, BOOLEAN_P, int, int));
E void FDECL(digactualhole, (int,int,struct monst *,int));
E boolean FDECL(dighole, (BOOLEAN_P));
E int FDECL(use_pick_axe, (struct obj *));
E int FDECL(use_pick_axe2, (struct obj *));
E boolean FDECL(mdig_tunnel, (struct monst *));
E void FDECL(watch_dig, (struct monst *,XCHAR_P,XCHAR_P,BOOLEAN_P));
E void NDECL(zap_dig);
E struct obj *FDECL(bury_an_obj, (struct obj *));
E void FDECL(bury_objs, (int,int));
E void FDECL(unearth_objs, (int,int));
E void FDECL(rot_organic, (genericptr_t, long));
E void FDECL(rot_corpse, (genericptr_t, long));
#if 0
E void FDECL(bury_monst, (struct monst *));
E void NDECL(bury_you);
E void NDECL(unearth_you);
E void NDECL(escape_tomb);
E void FDECL(bury_obj, (struct obj *));
#endif

/* ### display.c ### */

#ifdef INVISIBLE_OBJECTS
E struct obj * FDECL(vobj_at, (XCHAR_P,XCHAR_P));
#endif /* INVISIBLE_OBJECTS */
E void FDECL(magic_map_background, (XCHAR_P,XCHAR_P,int));
E void FDECL(map_background, (XCHAR_P,XCHAR_P,int));
E void FDECL(map_trap, (struct trap *,int));
E void FDECL(map_object, (struct obj *,int));
E void FDECL(map_invisible, (XCHAR_P,XCHAR_P));
E void FDECL(unmap_object, (int,int));
E void FDECL(map_location, (int,int,int));
E int FDECL(memory_glyph, (int, int));
E void FDECL(clear_memory_glyph, (int, int, int));
E void FDECL(feel_location, (XCHAR_P,XCHAR_P));
E void FDECL(newsym, (int,int));
E void FDECL(shieldeff, (XCHAR_P,XCHAR_P));
E void FDECL(tmp_at, (int,int));
#ifdef DISPLAY_LAYERS
E int FDECL(glyph_is_floating, (int));
#endif
E void FDECL(swallowed, (int));
E void FDECL(under_ground, (int));
E void FDECL(under_water, (int));
E void NDECL(see_monsters);
E void NDECL(set_mimic_blocking);
E void NDECL(see_objects);
E void NDECL(see_traps);
E void NDECL(curs_on_u);
E int NDECL(doredraw);
E void NDECL(docrt);
E void FDECL(show_glyph, (int,int,int));
E void NDECL(clear_glyph_buffer);
E void FDECL(row_refresh, (int,int,int));
E void NDECL(cls);
E void FDECL(flush_screen, (int));
#ifdef DUMP_LOG
E void NDECL(dump_screen);
#endif
E int FDECL(back_to_glyph, (XCHAR_P,XCHAR_P));
E int FDECL(zapdir_to_glyph, (int,int,int));
E int FDECL(glyph_at, (XCHAR_P,XCHAR_P));
E void NDECL(set_wall_state);

/* ### do.c ### */

#ifdef USE_TRAMPOLI
E int FDECL(drop, (struct obj *));
E int NDECL(wipeoff);
#endif
E int NDECL(dodrop);
E boolean FDECL(boulder_hits_pool, (struct obj *,int,int,BOOLEAN_P));
E boolean FDECL(flooreffects, (struct obj *,int,int,const char *));
E void FDECL(doaltarobj, (struct obj *));
E boolean FDECL(canletgo, (struct obj *,const char *));
E void FDECL(dropx, (struct obj *));
E void FDECL(dropy, (struct obj *));
E void FDECL(obj_no_longer_held, (struct obj *));
E int NDECL(doddrop);
E int NDECL(dodown);
E int NDECL(doup);
#ifdef INSURANCE
E void NDECL(save_currentstate);
#endif
E void FDECL(goto_level, (d_level *,BOOLEAN_P,BOOLEAN_P,BOOLEAN_P));
E void FDECL(schedule_goto, (d_level *,BOOLEAN_P,BOOLEAN_P,int,
			     const char *,const char *));
E void NDECL(deferred_goto);
E boolean FDECL(revive_corpse, (struct obj *, BOOLEAN_P));
E void FDECL(revive_mon, (genericptr_t, long));
E void FDECL(moldy_corpse, (genericptr_t, long));
E int NDECL(donull);
E int NDECL(dowipe);
E void FDECL(set_wounded_legs, (long,int));
E void NDECL(heal_legs);

/* ### do_name.c ### */

E int FDECL(getpos, (coord *,BOOLEAN_P,const char *));
E struct monst *FDECL(christen_monst, (struct monst *,const char *));
E int NDECL(do_mname);
E struct obj *FDECL(oname, (struct obj *,const char *));
E int NDECL(ddocall);
E void FDECL(docall, (struct obj *));
E const char *NDECL(rndghostname);
E const char *NDECL(rndplrmonname);
E const char *NDECL(rndplrmonnamefemale);
E char *FDECL(x_monnam, (struct monst *,int,const char *,int,BOOLEAN_P));
E char *FDECL(l_monnam, (struct monst *));
E char *FDECL(mon_nam, (struct monst *));
E char *FDECL(noit_mon_nam, (struct monst *));
E char *FDECL(Monnam, (struct monst *));
E char *FDECL(noit_Monnam, (struct monst *));
E char *FDECL(m_monnam, (struct monst *));
E char *FDECL(y_monnam, (struct monst *));
E char *FDECL(Adjmonnam, (struct monst *,const char *));
E char *FDECL(Amonnam, (struct monst *));
E char *FDECL(a_monnam, (struct monst *));
E char *FDECL(distant_monnam, (struct monst *,int,char *));
E const char *NDECL(rndmonnam);
E const char *FDECL(hcolor, (const char *));
E const char *NDECL(rndcolor);
#ifdef REINCARNATION
E const char *NDECL(roguename);
#endif
E struct obj *FDECL(realloc_obj,
		(struct obj *, int, genericptr_t, int, const char *));
E char *FDECL(coyotename, (struct monst *,char *));


/* ### do_wear.c ### */

E int NDECL(Armor_on);
E int NDECL(Boots_on);
E int NDECL(Cloak_on);
E int NDECL(Helmet_on);
E int NDECL(Gloves_on);
E int NDECL(Shield_on);
#ifdef TOURIST
E int NDECL(Shirt_on);
#endif
E void NDECL(Amulet_on);
#ifdef USE_TRAMPOLI
E int FDECL(select_off, (struct obj *));
E int NDECL(take_off);
#endif
E void FDECL(off_msg, (struct obj *));
E void NDECL(set_wear);
E boolean FDECL(donning, (struct obj *));
E void NDECL(cancel_don);
E int NDECL(Armor_off);
E int NDECL(Armor_gone);
E int NDECL(Helmet_off);
E int NDECL(Gloves_off);
E int NDECL(Boots_off);
E int NDECL(Cloak_off);
E int NDECL(Shield_off);
#ifdef TOURIST
E int NDECL(Shirt_off);
#endif
E void NDECL(Amulet_off);
E void FDECL(Ring_on, (struct obj *));
E void FDECL(Ring_off, (struct obj *));
E void FDECL(Ring_gone, (struct obj *));
E void FDECL(Blindf_on, (struct obj *));
E void FDECL(Blindf_off, (struct obj *));
E int NDECL(dotakeoff);
E int NDECL(doremring);
E int FDECL(cursed, (struct obj *));
E int FDECL(armoroff, (struct obj *));
E int FDECL(canwearobj, (struct obj *, long *, BOOLEAN_P));
E int NDECL(dowear);
E int NDECL(doputon);
E void NDECL(find_ac);
E void NDECL(glibr);
E struct obj *FDECL(some_armor,(struct monst *));
E void FDECL(erode_armor, (struct monst *,BOOLEAN_P));
E struct obj *FDECL(stuck_ring, (struct obj *,int));
E struct obj *NDECL(unchanger);
E void NDECL(reset_remarm);
E int NDECL(doddoremarm);
E int FDECL(destroy_arm, (struct obj *));
E void FDECL(adj_abon, (struct obj *,SCHAR_P));
E int FDECL(dowear2, (const char *, const char *));

/* ### dog.c ### */

E void FDECL(initedog, (struct monst *));
E struct monst *FDECL(make_familiar, (struct obj *,XCHAR_P,XCHAR_P,BOOLEAN_P));
E struct monst *FDECL(make_helper, (int,XCHAR_P,XCHAR_P));
E struct monst *NDECL(makedog);
E void NDECL(update_mlstmv);
E void NDECL(losedogs);
E void FDECL(mon_arrive, (struct monst *,BOOLEAN_P));
E void FDECL(mon_catchup_elapsed_time, (struct monst *,long));
E void FDECL(keepdogs, (BOOLEAN_P));
E void FDECL(migrate_to_level, (struct monst *,XCHAR_P,XCHAR_P,coord *));
E int FDECL(dogfood, (struct monst *,struct obj *));
E struct monst *FDECL(tamedog, (struct monst *,struct obj *, BOOLEAN_P));
E int FDECL(make_pet_minion, (int,ALIGNTYP_P));
E void FDECL(abuse_dog, (struct monst *));
E void FDECL(wary_dog, (struct monst *, BOOLEAN_P));

/* ### dogmove.c ### */

E int FDECL(dog_nutrition, (struct monst *,struct obj *));
E int FDECL(dog_eat, (struct monst *,struct obj *,int,int,BOOLEAN_P));
E int FDECL(dog_move, (struct monst *,int));
E boolean FDECL(betrayed, (struct monst *));
#ifdef USE_TRAMPOLI
E void FDECL(wantdoor, (int,int,genericptr_t));
#endif

/* ### dokick.c ### */

E boolean FDECL(ghitm, (struct monst *,struct obj *));
E void FDECL(container_impact_dmg, (struct obj *));
E int NDECL(dokick);
E boolean FDECL(ship_object, (struct obj *,XCHAR_P,XCHAR_P,BOOLEAN_P));
E void NDECL(obj_delivery);
E schar FDECL(down_gate, (XCHAR_P,XCHAR_P));
E void FDECL(impact_drop, (struct obj *,XCHAR_P,XCHAR_P,XCHAR_P));

/* ### dothrow.c ### */

E struct obj *FDECL(splitoneoff, (struct obj **));
E int NDECL(dothrow);
E int NDECL(dofire);
E void FDECL(hitfloor, (struct obj *));
E void FDECL(hurtle, (int,int,int,BOOLEAN_P));
E void FDECL(mhurtle, (struct monst *,int,int,int));
E void FDECL(throwit, (struct obj *,long,BOOLEAN_P,int));
E int FDECL(omon_adj, (struct monst *,struct obj *,BOOLEAN_P));
E int FDECL(thitmonst, (struct monst *,struct obj *, int));
E int FDECL(hero_breaks, (struct obj *,XCHAR_P,XCHAR_P,BOOLEAN_P));
E int FDECL(breaks, (struct obj *,XCHAR_P,XCHAR_P));
E boolean FDECL(breaktest, (struct obj *));
E boolean FDECL(walk_path, (coord *, coord *, boolean (*)(genericptr_t,int,int), genericptr_t));
E boolean FDECL(hurtle_step, (genericptr_t, int, int));

/* ### drawing.c ### */
#endif /* !MAKEDEFS_C && !LEV_LEX_C */
E int FDECL(def_char_to_objclass, (CHAR_P));
E int FDECL(def_char_to_monclass, (CHAR_P));
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
E void FDECL(assign_graphics, (uchar *,int,int,int));
E void FDECL(switch_graphics, (int));
#ifdef REINCARNATION
E void FDECL(assign_rogue_graphics, (BOOLEAN_P));
#endif

/* ### dungeon.c ### */

E void FDECL(save_dungeon, (int,BOOLEAN_P,BOOLEAN_P));
E void FDECL(restore_dungeon, (int));
E void FDECL(insert_branch, (branch *,BOOLEAN_P));
E void NDECL(init_dungeons);
E s_level *FDECL(find_level, (const char *));
E s_level *FDECL(Is_special, (d_level *));
E branch *FDECL(Is_branchlev, (d_level *));
E xchar FDECL(ledger_no, (d_level *));
E xchar NDECL(maxledgerno);
E schar FDECL(depth, (d_level *));
E xchar FDECL(dunlev, (d_level *));
E xchar FDECL(dunlevs_in_dungeon, (d_level *));
E xchar FDECL(real_dunlevs_in_dungeon, (d_level *));
E xchar FDECL(ledger_to_dnum, (XCHAR_P));
E xchar FDECL(ledger_to_dlev, (XCHAR_P));
E xchar FDECL(deepest_lev_reached, (BOOLEAN_P));
E boolean FDECL(on_level, (d_level *,d_level *));
E void FDECL(next_level, (BOOLEAN_P));
E void FDECL(prev_level, (BOOLEAN_P));
E void FDECL(u_on_newpos, (int,int));
E void NDECL(u_on_sstairs);
E void NDECL(u_on_upstairs);
E void NDECL(u_on_dnstairs);
E boolean FDECL(On_stairs, (XCHAR_P,XCHAR_P));
E void FDECL(get_level, (d_level *,int));
E boolean FDECL(Is_botlevel, (d_level *));
E boolean FDECL(Can_fall_thru, (d_level *));
E boolean FDECL(Can_dig_down, (d_level *));
E boolean FDECL(Can_rise_up, (int,int,d_level *));
E boolean FDECL(In_quest, (d_level *));
E boolean FDECL(In_mines, (d_level *));
E boolean FDECL(In_sheol, (d_level *));
E boolean FDECL(In_dod, (d_level *));
E boolean FDECL(In_gehennom, (d_level *));
E boolean FDECL(In_towndungeon, (d_level *));
E boolean FDECL(In_spiders, (d_level *));
E branch *FDECL(dungeon_branch, (const char *));
E boolean FDECL(at_dgn_entrance, (const char *));
E boolean FDECL(In_hell, (d_level *));
E boolean FDECL(In_V_tower, (d_level *));
E boolean FDECL(On_W_tower_level, (d_level *));
E boolean FDECL(In_W_tower, (int,int,d_level *));
E void FDECL(find_hell, (d_level *));
E void FDECL(goto_hell, (BOOLEAN_P,BOOLEAN_P));
E void FDECL(assign_level, (d_level *,d_level *));
E void FDECL(assign_rnd_level, (d_level *,d_level *,int));
E int FDECL(induced_align, (int));
E boolean FDECL(Invocation_lev, (d_level *));
E xchar NDECL(level_difficulty);
E xchar NDECL(monster_difficulty);
E schar FDECL(lev_by_name, (const char *));
#ifdef WIZARD
E schar FDECL(print_dungeon, (BOOLEAN_P,schar *,xchar *));
#endif

/* ### eat.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(eatmdone);
E int NDECL(eatfood);
E int NDECL(opentin);
E int NDECL(unfaint);
#endif
E boolean FDECL(is_edible, (struct obj *));
E void NDECL(init_uhunger);
E int NDECL(Hear_again);
E void NDECL(reset_eat);
E int NDECL(doeat);
E void NDECL(gethungry);
E void FDECL(morehungry, (int));
E void FDECL(lesshungry, (int));
E boolean NDECL(is_fainted);
E void NDECL(reset_faint);
E void NDECL(violated_vegetarian);
#if 0
E void NDECL(sync_hunger);
#endif
E void FDECL(newuhs, (BOOLEAN_P));
E boolean NDECL(can_reach_floorobj);
E void NDECL(vomit);
E int FDECL(eaten_stat, (int,struct obj *));
E void FDECL(food_disappears, (struct obj *));
E void FDECL(food_substitution, (struct obj *,struct obj *));
E boolean FDECL(bite_monster, (struct monst *mon));
E void NDECL(fix_petrification);
E void FDECL(consume_oeaten, (struct obj *,int));
E boolean FDECL(maybe_finished_meal, (BOOLEAN_P));

/* ### end.c ### */

E void FDECL(done1, (int));
E int NDECL(done2);
#ifdef USE_TRAMPOLI
E void FDECL(done_intr, (int));
#endif
E void FDECL(done_in_by, (struct monst *));
#endif /* !MAKEDEFS_C && !LEV_LEX_C */
E void VDECL(panic, (const char *,...)) PRINTF_F(1,2);
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
E void FDECL(done, (int));
E void FDECL(container_contents, (struct obj *,BOOLEAN_P,BOOLEAN_P));
#ifdef DUMP_LOG
E void FDECL(dump, (char *, char *));
E void FDECL(do_containerconts, (struct obj *,BOOLEAN_P,BOOLEAN_P,BOOLEAN_P));
#endif
E void FDECL(terminate, (int));
E int NDECL(dolistvanq);
E int NDECL(num_genocides);
/* KMH, ethics */
E int NDECL(doethics);


/* ### engrave.c ### */

E char *FDECL(random_engraving, (char *));
E void FDECL(wipeout_text, (char *,int,unsigned));
E boolean NDECL(can_reach_floor);
E const char *FDECL(surface, (int,int));
E const char *FDECL(ceiling, (int,int));
E struct engr *FDECL(engr_at, (XCHAR_P,XCHAR_P));
#ifdef ELBERETH
E int FDECL(sengr_at, (const char *,XCHAR_P,XCHAR_P));
#endif
E void FDECL(u_wipe_engr, (int));
E void FDECL(wipe_engr_at, (XCHAR_P,XCHAR_P,XCHAR_P));
E boolean FDECL(sense_engr_at, (int,int,BOOLEAN_P));
E void FDECL(make_engr_at, (int,int,const char *,long,XCHAR_P));
E void FDECL(del_engr_at, (int,int));
E int NDECL(freehand);
E int NDECL(doengrave);
E void FDECL(save_engravings, (int,int));
E void FDECL(rest_engravings, (int));
E void FDECL(del_engr, (struct engr *));
E void FDECL(rloc_engr, (struct engr *));
E void FDECL(make_grave, (int,int,const char *));

/* ### exper.c ### */

E long FDECL(newuexp, (int));
E int FDECL(experience, (struct monst *,int));
E void FDECL(more_experienced, (int,int));
E void FDECL(losexp, (const char *, BOOLEAN_P, BOOLEAN_P));
E void NDECL(newexplevel);
E void FDECL(pluslvl, (BOOLEAN_P));
E long FDECL(rndexp, (BOOLEAN_P));

/* ### explode.c ### */

E void FDECL(explode, (int,int,int,int,CHAR_P,int));
E long FDECL(scatter, (int, int, int, unsigned int, struct obj *));
E void FDECL(splatter_burning_oil, (int, int));
#ifdef FIREARMS
E void FDECL(grenade_explode, (struct obj *, int, int, BOOLEAN_P, int));
E void FDECL(arm_bomb, (struct obj *, BOOLEAN_P));
#endif

/* ### extralev.c ### */

#ifdef REINCARNATION
E void NDECL(makeroguerooms);
E void FDECL(corr, (int,int));
E void NDECL(makerogueghost);
#endif

/* ### files.c ### */

E char *FDECL(fname_encode, (const char *, CHAR_P, char *, char *, int));
E char *FDECL(fname_decode, (CHAR_P, char *, char *, int));
E const char *FDECL(fqname, (const char *, int, int));
#ifndef FILE_AREAS
E FILE *FDECL(fopen_datafile, (const char *,const char *,int));
#endif
E boolean FDECL(uptodate, (int,const char *));
E void FDECL(store_version, (int));
#ifdef MFLOPPY
E void NDECL(set_lock_and_bones);
#endif
E void FDECL(set_levelfile_name, (char *,int));
E int FDECL(create_levelfile, (int,char *));
E int FDECL(open_levelfile, (int,char *));
E void FDECL(delete_levelfile, (int));
E void NDECL(clearlocks);
E int FDECL(create_bonesfile, (d_level*,char **, char *));
#ifdef MFLOPPY
E void NDECL(cancel_bonesfile);
#endif
E void FDECL(commit_bonesfile, (d_level *));
E int FDECL(open_bonesfile, (d_level*,char **));
E int FDECL(delete_bonesfile, (d_level*));
E void NDECL(compress_bonesfile);
E void NDECL(set_savefile_name);
#ifdef INSURANCE
E void FDECL(save_savefile_name, (int));
#endif
#if defined(WIZARD) && !defined(MICRO)
E void NDECL(set_error_savefile);
#endif
E int NDECL(create_savefile);
E int NDECL(open_savefile);
E int NDECL(delete_savefile);
E int NDECL(restore_saved_game);
E void FDECL(compress_area, (const char *, const char *));
E void FDECL(uncompress_area, (const char *, const char *));
#ifndef FILE_AREAS
E boolean FDECL(lock_file, (const char *,int,int));
E void FDECL(unlock_file, (const char *));
#endif
#ifdef USER_SOUNDS
E boolean FDECL(can_read_file, (const char *));
#endif
E void FDECL(read_config_file, (const char *));
E void FDECL(check_recordfile, (const char *));
#if defined(WIZARD)
E void NDECL(read_wizkit);
#endif
E void FDECL(paniclog, (const char *, const char *));
E int FDECL(validate_prefix_locations, (char *));
E char** NDECL(get_saved_games);
E void FDECL(free_saved_games, (char**));
#ifdef SELF_RECOVER
E boolean NDECL(recover_savefile);
#endif
#ifdef HOLD_LOCKFILE_OPEN
E void NDECL(really_close);
#endif

/* ### fountain.c ### */

E void FDECL(floating_above, (const char *));
E void FDECL(dogushforth, (int));
# ifdef USE_TRAMPOLI
E void FDECL(gush, (int,int,genericptr_t));
# endif
E void FDECL(dryup, (XCHAR_P,XCHAR_P, BOOLEAN_P));
E void NDECL(drinkfountain);
E void FDECL(dipfountain, (struct obj *));
E void FDECL(whetstone_fountain_effects, (struct obj *));
#ifdef SINKS
E void FDECL(diptoilet, (struct obj *));
E void FDECL(breaksink, (int,int));
E void FDECL(breaktoilet, (int,int));
E void NDECL(drinksink);
E void NDECL(drinktoilet);
E void FDECL(whetstone_sink_effects, (struct obj *));
E void FDECL(whetstone_toilet_effects, (struct obj *));
#endif

/* ### gypsy.c ### */

E void FDECL(gypsy_init, (struct monst *));
E void FDECL(gypsy_chat, (struct monst *));


/* ### hack.c ### */

#ifdef OVL1
E void NDECL(maybe_wail);
#endif /*OVL1*/

#ifdef DUNGEON_GROWTH
E void FDECL(catchup_dgn_growths, (int));
E void FDECL(dgn_growths, (BOOLEAN_P,BOOLEAN_P));
#endif
E boolean FDECL(revive_nasty, (int,int,const char*));
E void FDECL(movobj, (struct obj *,XCHAR_P,XCHAR_P));
E boolean FDECL(may_dig, (XCHAR_P,XCHAR_P));
E boolean FDECL(may_passwall, (XCHAR_P,XCHAR_P));
E boolean FDECL(bad_rock, (struct monst *,XCHAR_P,XCHAR_P));
E boolean FDECL(invocation_pos, (XCHAR_P,XCHAR_P));
E boolean FDECL(test_move, (int, int, int, int, int));
E void NDECL(domove);
E void NDECL(invocation_message);
E void FDECL(wounds_message,(struct monst *)); 
E char *FDECL(mon_wounds,(struct monst *)); 
E void FDECL(spoteffects, (BOOLEAN_P));
E char *FDECL(in_rooms, (XCHAR_P,XCHAR_P,int));
E char *FDECL(in_roomscolouur, (XCHAR_P,XCHAR_P,int));
E boolean FDECL(in_town, (int,int));
E void FDECL(check_special_room, (BOOLEAN_P));
E int NDECL(dopickup);
E void NDECL(lookaround);
E int NDECL(monster_nearby);
E void FDECL(nomul, (int, const char *));
E void FDECL(unmul, (const char *));
#ifdef SHOW_DMG
E void FDECL(showdmg, (int));
#endif
E void FDECL(losehp, (int,const char *, int));
E int NDECL(weight_cap);
E int NDECL(inv_weight);
E int NDECL(near_capacity);
E int FDECL(calc_capacity, (int));
E int NDECL(max_capacity);
E boolean FDECL(check_capacity, (const char *));
E int NDECL(inv_cnt);
#ifdef GOLDOBJ
E long FDECL(money_cnt, (struct obj *));
#endif

/* ### hacklib.c ### */

E boolean FDECL(digit, (CHAR_P));
E boolean FDECL(letter, (CHAR_P));
E char FDECL(highc, (CHAR_P));
E char FDECL(lowc, (CHAR_P));
E char *FDECL(lcase, (char *));
E char *FDECL(upstart, (char *));
E char *FDECL(mungspaces, (char *));
E char *FDECL(eos, (char *));
E char *FDECL(strkitten, (char *,CHAR_P));
E char *FDECL(s_suffix, (const char *));
E char *FDECL(xcrypt, (const char *,char *));
E boolean FDECL(onlyspace, (const char *));
E char *FDECL(tabexpand, (char *));
E char *FDECL(visctrl, (CHAR_P));
E const char *FDECL(ordin, (int));
E char *FDECL(sitoa, (int));
E int FDECL(sgn, (int));
E int FDECL(rounddiv, (long,int));
E int FDECL(dist2, (int,int,int,int));
E int FDECL(distmin, (int,int,int,int));
E boolean FDECL(online2, (int,int,int,int));
E boolean FDECL(pmatch, (const char *,const char *));
#ifndef STRNCMPI
E int FDECL(strncmpi, (const char *,const char *,int));
#endif
#ifndef STRSTRI
E char *FDECL(strstri, (const char *,const char *));
#endif
E boolean FDECL(fuzzymatch, (const char *,const char *,const char *,BOOLEAN_P));
E void NDECL(setrandom);
E int NDECL(getyear);
E int NDECL(getmonth);	/* KMH -- Used by gypsies */
#if 0
E char *FDECL(yymmdd, (time_t));
#endif
E long FDECL(yyyymmdd, (time_t));
E int NDECL(phase_of_the_moon);
E boolean NDECL(friday_13th);
E boolean NDECL(groundhog_day);	/* KMH -- February 2 */
E int NDECL(night);
E int NDECL(midnight);

/* ### invent.c ### */

E void FDECL(assigninvlet, (struct obj *));
E struct obj *FDECL(merge_choice, (struct obj *,struct obj *));
E int FDECL(merged, (struct obj **,struct obj **));
#ifdef USE_TRAMPOLI
E int FDECL(ckunpaid, (struct obj *));
#endif
E void FDECL(addinv_core1, (struct obj *));
E void FDECL(addinv_core2, (struct obj *));
E struct obj *FDECL(addinv, (struct obj *));
E struct obj *FDECL(hold_another_object,
			(struct obj *,const char *,const char *,const char *));
E struct obj *FDECL(hold_another_objectX,
			(struct obj *,const char *,const char *,const char *));
E void FDECL(useupall, (struct obj *));
E void FDECL(useup, (struct obj *));
E void FDECL(consume_obj_charge, (struct obj *,BOOLEAN_P));
E void FDECL(freeinv_core, (struct obj *));
E void FDECL(freeinv, (struct obj *));
E void FDECL(delallobj, (int,int));
E void FDECL(delobj, (struct obj *));
E struct obj *FDECL(sobj_at, (int,int,int));
E struct obj *FDECL(carrying, (int));
E boolean NDECL(have_lizard);
E boolean NDECL(have_loadstone);
E boolean NDECL(have_sleepstone);
E boolean NDECL(have_magicresstone);
E boolean NDECL(have_cursedmagicresstone);
E boolean NDECL(have_loadboulder);
E boolean NDECL(have_starlightstone);

E boolean FDECL(stack_too_big, (struct obj *));

E boolean NDECL(have_rmbstone);
E boolean NDECL(have_displaystone);
E boolean NDECL(have_yellowspellstone);
E boolean NDECL(have_spelllossstone);
E boolean NDECL(have_autodestructstone);
E boolean NDECL(have_memorylossstone);
E boolean NDECL(have_inventorylossstone);
E boolean NDECL(have_blackystone);
E boolean NDECL(have_menubugstone);
E boolean NDECL(have_speedbugstone);
E boolean NDECL(have_superscrollerstone);
E boolean NDECL(have_freehandbugstone);
E boolean NDECL(have_unidentifystone);
E boolean NDECL(have_thirststone);
E boolean NDECL(have_unluckystone);
E boolean NDECL(have_shadesofgreystone);
E boolean NDECL(have_faintingstone);
E boolean NDECL(have_cursingstone);
E boolean NDECL(have_difficultystone);
E boolean NDECL(have_deafnessstone);
E boolean NDECL(have_weaknessstone);
E boolean NDECL(have_antimagicstone);
E boolean NDECL(have_rotthirteenstone);
E boolean NDECL(have_bishopstone);
E boolean NDECL(have_confusionstone);
E boolean NDECL(have_dropbugstone);
E boolean NDECL(have_dstwstone);
E boolean NDECL(have_statusstone);
E boolean NDECL(have_alignmentstone);
E boolean NDECL(have_stairstrapstone);
E boolean NDECL(have_uninformationstone);

E boolean NDECL(have_amnesiastone);
E boolean NDECL(have_bigscriptstone);
E boolean NDECL(have_bankstone);
E boolean NDECL(have_mapstone);
E boolean NDECL(have_techniquestone);
E boolean NDECL(have_disenchantmentstone);
E boolean NDECL(have_verisiertstone);
E boolean NDECL(have_chaosterrainstone);
E boolean NDECL(have_mutenessstone);
E boolean NDECL(have_engravingstone);
E boolean NDECL(have_magicdevicestone);
E boolean NDECL(have_bookstone);
E boolean NDECL(have_levelstone);
E boolean NDECL(have_quizstone);

E boolean NDECL(have_intrinsiclossstone);
E boolean NDECL(have_bloodlossstone);
E boolean NDECL(have_badeffectstone);
E boolean NDECL(have_trapcreationstone);
E boolean NDECL(have_vulnerabilitystone);
E boolean NDECL(have_itemportstone);
E boolean NDECL(have_nastystone);

E boolean NDECL(have_respawnstone);
E boolean NDECL(have_captchastone);
E boolean NDECL(have_farlookstone);

E boolean NDECL(have_metabolicstone);
E boolean NDECL(have_noreturnstone);
E boolean NDECL(have_egostone);
E boolean NDECL(have_fastforwardstone);
E boolean NDECL(have_rottenstone);
E boolean NDECL(have_unskilledstone);
E boolean NDECL(have_lowstatstone);
E boolean NDECL(have_trainingstone);
E boolean NDECL(have_exercisestone);

E boolean NDECL(have_primecurse);

E boolean NDECL(have_mothrelay);

E struct obj *FDECL(o_on, (unsigned int,struct obj *));
E boolean FDECL(obj_here, (struct obj *,int,int));
E boolean NDECL(wearing_armor);
E boolean FDECL(is_worn, (struct obj *));
E struct obj *FDECL(g_at, (int,int));
E struct obj *FDECL(mkgoldobj, (long));
E struct obj *FDECL(getobj, (const char *,const char *));
E int FDECL(ggetobj, (const char *,int (*)(OBJ_P),int,BOOLEAN_P,unsigned *));
E void FDECL(fully_identify_obj, (struct obj *));
E int FDECL(identify, (struct obj *));
E void FDECL(identify_pack, (int));
E int FDECL(askchain, (struct obj **,const char *,int,int (*)(OBJ_P),
			int (*)(OBJ_P),int,const char *));
E void FDECL(prinv, (const char *,struct obj *,long));
E char *FDECL(xprname, (struct obj *,const char *,CHAR_P,BOOLEAN_P,long,long));
E int NDECL(ddoinv);
E char FDECL(display_inventory, (const char *,BOOLEAN_P));
#ifdef DUMP_LOG
E char FDECL(dump_inventory, (const char *,BOOLEAN_P));
#endif
E int FDECL(display_binventory, (int,int,BOOLEAN_P));
E struct obj *FDECL(display_cinventory,(struct obj *));
E struct obj *FDECL(display_minventory,(struct monst *,int,char *));
E int NDECL(dotypeinv);
E const char *FDECL(dfeature_at, (int,int,char *));
E int FDECL(look_here, (int,BOOLEAN_P));
E int NDECL(dolook);
E boolean FDECL(will_feel_cockatrice, (struct obj *,BOOLEAN_P));
E void FDECL(feel_cockatrice, (struct obj *,BOOLEAN_P));
E void FDECL(stackobj, (struct obj *));
E int NDECL(doprgold);
E int NDECL(doprwep);
E int NDECL(doprarm);
E int NDECL(doprring);
E int NDECL(dopramulet);
E int NDECL(doprtool);
E int NDECL(doprinuse);
E void FDECL(useupf, (struct obj *,long));
E char *FDECL(let_to_name, (CHAR_P,BOOLEAN_P,BOOLEAN_P));
E void NDECL(free_invbuf);
E void NDECL(reassign);
E int NDECL(doorganize);
E int FDECL(count_unpaid, (struct obj *));
E int FDECL(count_buc, (struct obj *,int));
E void FDECL(carry_obj_effects, (struct monst *, struct obj *));
E const char *FDECL(currency, (long));
E void FDECL(silly_thing, (const char *,struct obj *));
E int NDECL(doinvinuse);
/* KMH, balance patch -- new function */
E int NDECL(jumble_pack);

/* ### ioctl.c ### */

#if defined(UNIX) || defined(__BEOS__)
E void NDECL(getwindowsz);
E void NDECL(getioctls);
E void NDECL(setioctls);
# ifdef SUSPEND
E int NDECL(dosuspend);
# endif /* SUSPEND */
#endif /* UNIX || __BEOS__ */

/* ### light.c ### */

E void FDECL(new_light_source, (XCHAR_P, XCHAR_P, int, int, genericptr_t));
E void FDECL(del_light_source, (int, genericptr_t));
E void FDECL(do_light_sources, (char **));
E struct monst *FDECL(find_mid, (unsigned, unsigned));
E void FDECL(save_light_sources, (int, int, int));
E void FDECL(restore_light_sources, (int));
E void FDECL(relink_light_sources, (BOOLEAN_P));
E void FDECL(obj_move_light_source, (struct obj *, struct obj *));
E boolean NDECL(any_light_source);
E void FDECL(snuff_light_source, (int, int));
E boolean FDECL(obj_sheds_light, (struct obj *));
E boolean FDECL(obj_is_burning, (struct obj *));
E boolean FDECL(obj_permanent_light, (struct obj *));
E void FDECL(obj_split_light_source, (struct obj *, struct obj *));
E void FDECL(obj_merge_light_sources, (struct obj *,struct obj *));
E int FDECL(candle_light_range, (struct obj *));
#ifdef WIZARD
E int NDECL(wiz_light_sources);
#endif

/* ### lock.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(forcelock);
E int NDECL(picklock);
#endif
E boolean FDECL(picking_lock, (int *,int *));
E boolean FDECL(picking_at, (int,int));
E void NDECL(reset_pick);
E int FDECL(pick_lock, (struct obj **));
E int NDECL(doforce);
E boolean FDECL(boxlock, (struct obj *,struct obj *));
E boolean FDECL(doorlock, (struct obj *,int,int));
E boolean FDECL(doorlockX, (int,int));
E int NDECL(doopen);
E int NDECL(doclose);
E int FDECL(artifact_door, (int,int));

#ifdef MAC
/* These declarations are here because the main code calls them. */

/* ### macfile.c ### */

E int FDECL(maccreat, (const char *,long));
E int FDECL(macopen, (const char *,int,long));
E int FDECL(macclose, (int));
E int FDECL(macread, (int,void *,unsigned));
E int FDECL(macwrite, (int,void *,unsigned));
E long FDECL(macseek, (int,long,short));
E int FDECL(macunlink, (const char *));

/* ### macsnd.c ### */

E void FDECL(mac_speaker, (struct obj *,char *));

/* ### macunix.c ### */

E void FDECL(regularize, (char *));
E void NDECL(getlock);

/* ### macwin.c ### */

E void FDECL(lock_mouse_cursor, (Boolean));
E int NDECL(SanePositions);

/* ### mttymain.c ### */

E void FDECL(getreturn, (const char *));
E void VDECL(msmsg, (const char *,...));
E void NDECL(gettty);
E void NDECL(setftty);
E void FDECL(settty, (const char *));
E int NDECL(tgetch);
E void FDECL(cmov, (int x, int y));
E void FDECL(nocmov, (int x, int y));

#endif /* MAC */

/* ### mail.c ### */

#ifdef MAIL
# ifdef UNIX
E void NDECL(getmailstatus);
# endif
E void NDECL(ckmailstatus);
E void FDECL(readmail, (struct obj *));
#endif /* MAIL */

/* ### makemon.c ### */

E boolean FDECL(is_home_elemental, (struct permonst *));
E struct monst *FDECL(clone_mon, (struct monst *,XCHAR_P,XCHAR_P));
E struct monst *FDECL(makemon, (struct permonst *,int,int,int));
E boolean FDECL(create_critters, (int,struct permonst *));
E struct permonst *NDECL(rndmonst);
E void FDECL(reset_rndmonst, (int));
E struct permonst *FDECL(mkclass, (CHAR_P,int));
E int FDECL(pm_mkclass, (CHAR_P,int));
E int FDECL(adj_lev, (struct permonst *));
E struct permonst *FDECL(grow_up, (struct monst *,struct monst *));
E int FDECL(mongets, (struct monst *,int));
E int FDECL(golemhp, (int));
E boolean FDECL(peace_minded, (struct permonst *));
E void FDECL(set_malign, (struct monst *));
E void FDECL(set_mimic_sym, (struct monst *));
E int FDECL(mbirth_limit, (int));
E void FDECL(mimic_hit_msg, (struct monst *, SHORT_P));
#ifdef GOLDOBJ
E void FDECL(mkmonmoney, (struct monst *, long));
#endif
E void FDECL(bagotricks, (struct obj *));
E boolean FDECL(propagate, (int, BOOLEAN_P,BOOLEAN_P));

/* ### mapglyph.c ### */

E void FDECL(mapglyph, (int, int *, int *, unsigned *, int, int));

/* ### mcastu.c ### */

E int FDECL(castmu, (struct monst *,struct attack *,BOOLEAN_P,BOOLEAN_P));
E int FDECL(buzzmu, (struct monst *,struct attack *));

/* ### mhitm.c ### */

E int FDECL(fightm, (struct monst *));
E int FDECL(mattackm, (struct monst *,struct monst *));
E int FDECL(noattacks, (struct permonst *));
E int FDECL(sleep_monst, (struct monst *,int,int));
E void FDECL(slept_monst, (struct monst *));
E long FDECL(attk_protection, (int));

/* ### mhitu.c ### */

E const char *FDECL(mpoisons_subj, (struct monst *,struct attack *));
E void NDECL(u_slow_down);
E struct monst *NDECL(cloneu);
E void FDECL(expels, (struct monst *,struct permonst *,BOOLEAN_P));
E struct attack *FDECL(getmattk, (struct permonst *,int,int *,struct attack *));
E int FDECL(mattacku, (struct monst *));
E int FDECL(magic_negation, (struct monst *));
E int FDECL(gazemu, (struct monst *,struct attack *));
E void FDECL(mdamageu, (struct monst *,int));
E int FDECL(could_seduce, (struct monst *,struct monst *,struct attack *));
E int FDECL(could_seduceX, (struct monst *,struct monst *,struct attack *));
#ifdef SEDUCE
E int FDECL(doseduce, (struct monst *));
#endif

/* ### minion.c ### */

E void FDECL(msummon, (struct monst *));
E void FDECL(summon_minion, (ALIGNTYP_P,BOOLEAN_P));
E int FDECL(demon_talk, (struct monst *));
E int FDECL(lawful_minion, (int));
E int FDECL(neutral_minion, (int));
E int FDECL(chaotic_minion, (int));
E long FDECL(bribe, (struct monst *));
E int FDECL(dprince, (ALIGNTYP_P));
E int FDECL(dlord, (ALIGNTYP_P));
E int NDECL(llord);
E int FDECL(ndemon, (ALIGNTYP_P));
E int NDECL(lminion);
E int NDECL(ntrminion);

/* ### mklev.c ### */

#ifdef USE_TRAMPOLI
E int FDECL(do_comp, (genericptr_t,genericptr_t));
#endif
E void NDECL(sort_rooms);
E void FDECL(add_room, (int,int,int,int,BOOLEAN_P,SCHAR_P,BOOLEAN_P,BOOLEAN_P));
E void FDECL(add_subroom, (struct mkroom *,int,int,int,int,
			   BOOLEAN_P,SCHAR_P,BOOLEAN_P));
E void NDECL(makecorridors);
E int NDECL(randomwalltype);
E int FDECL(add_door, (int,int,struct mkroom *));
E void NDECL(mklev);
#ifdef SPECIALIZATION
E void FDECL(topologize, (struct mkroom *,BOOLEAN_P));
#else
E void FDECL(topologize, (struct mkroom *));
#endif
E void FDECL(place_branch, (branch *,XCHAR_P,XCHAR_P));
E boolean FDECL(occupied, (XCHAR_P,XCHAR_P));
E int FDECL(okdoor, (XCHAR_P,XCHAR_P));
E void FDECL(dodoor, (int,int,struct mkroom *));
E void FDECL(mktrap, (int,int,struct mkroom *,coord*));
E void FDECL(mkstairs, (XCHAR_P,XCHAR_P,CHAR_P,struct mkroom *));
E void NDECL(mkinvokearea);

/* ### mkmap.c ### */

void FDECL(flood_fill_rm, (int,int,int,BOOLEAN_P,BOOLEAN_P));
void FDECL(remove_rooms, (int,int,int,int));

/* ### mkmaze.c ### */

E void FDECL(wallification, (int,int,int,int, BOOLEAN_P));
E void FDECL(walkfrom, (int,int));
E void FDECL(makemaz, (const char *));
E void FDECL(mazexy, (coord *));
E void FDECL(mazexy_all, (coord *));
E void NDECL(bound_digging);
E void FDECL(mkportal, (XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P));
E boolean FDECL(bad_location, (XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P));
E void FDECL(place_lregion, (XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,
			     XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,
			     XCHAR_P,d_level *));
E void NDECL(movebubbles);
E void NDECL(water_friction);
E void FDECL(save_waterlevel, (int,int));
E void FDECL(restore_waterlevel, (int));
E const char *FDECL(waterbody_name, (XCHAR_P,XCHAR_P));

/* ### mkobj.c ### */

E struct obj *FDECL(mkobj_at, (CHAR_P,int,int,BOOLEAN_P));
E struct obj *FDECL(mksobj_at, (int,int,int,BOOLEAN_P,BOOLEAN_P));
E struct obj *FDECL(mkobj, (CHAR_P,BOOLEAN_P));
E int NDECL(rndmonnum);
E int NDECL(usefulitem);
E struct obj *FDECL(splitobj, (struct obj *,long));
E void FDECL(replace_object, (struct obj *,struct obj *));
E void FDECL(bill_dummy_object, (struct obj *));
E struct obj *FDECL(mksobj, (int,BOOLEAN_P,BOOLEAN_P));
E int FDECL(bcsign, (struct obj *));
E int FDECL(weight, (struct obj *));
E struct obj *FDECL(mkgold, (long,int,int));
E struct obj *FDECL(mkcorpstat,
		(int,struct monst *,struct permonst *,int,int,BOOLEAN_P));
E struct obj *FDECL(obj_attach_mid, (struct obj *, unsigned));
E struct monst *FDECL(get_mtraits, (struct obj *, BOOLEAN_P));
E struct obj *FDECL(mk_tt_object, (int,int,int));
E struct obj *FDECL(mk_named_object,
			(int,struct permonst *,int,int,const char *));
E struct obj *FDECL(rnd_treefruit_at, (int, int));
E void FDECL(start_corpse_timeout, (struct obj *));
E void FDECL(bless, (struct obj *));
E void FDECL(unbless, (struct obj *));
E void FDECL(curse, (struct obj *));
E void FDECL(uncurse, (struct obj *));
E void FDECL(blessorcurse, (struct obj *,int));
E boolean FDECL(is_flammable, (struct obj *));
E boolean FDECL(is_rottable, (struct obj *));
E void FDECL(place_object, (struct obj *,int,int));
E void FDECL(remove_object, (struct obj *));
E void FDECL(discard_minvent, (struct monst *));
E void FDECL(obj_extract_self, (struct obj *));
E struct obj *FDECL(container_extract_indestructable, (struct obj *obj));
E void FDECL(extract_nobj, (struct obj *, struct obj **));
E void FDECL(extract_nexthere, (struct obj *, struct obj **));
E int FDECL(add_to_minv, (struct monst *, struct obj *));
E struct obj *FDECL(add_to_container, (struct obj *, struct obj *));
E void FDECL(add_to_migration, (struct obj *));
E void FDECL(add_to_buried, (struct obj *));
E void FDECL(dealloc_obj, (struct obj *));
E void FDECL(obj_ice_effects, (int, int, BOOLEAN_P));
E long FDECL(peek_at_iced_corpse_age, (struct obj *));
#if defined(OBJ_SANITY) || defined(WIZARD)
E void NDECL(obj_sanity_check);
#endif

/* ### mkroom.c ### */

E void FDECL(mkroom, (int));
E void FDECL(fill_zoo, (struct mkroom *));
E boolean FDECL(nexttodoor, (int,int));
E boolean FDECL(has_dnstairs, (struct mkroom *));
E boolean FDECL(has_upstairs, (struct mkroom *));
E int FDECL(somex, (struct mkroom *));
E int FDECL(somey, (struct mkroom *));
E boolean FDECL(inside_room, (struct mkroom *,XCHAR_P,XCHAR_P));
E boolean FDECL(somexy, (struct mkroom *,coord *));
E void FDECL(mkundead, (coord *,BOOLEAN_P,int));
/*E void FDECL(mkundeadboo, (coord *,BOOLEAN_P,int));*/
E void FDECL(mkundeadX, (coord *,BOOLEAN_P,int));
E struct permonst *NDECL(courtmon);
E struct permonst *NDECL(insidemon);
E struct permonst *NDECL(antholemon);
E struct permonst *NDECL(realzoomon);
E void FDECL(save_rooms, (int));
E void FDECL(rest_rooms, (int));
E struct mkroom *FDECL(search_special, (SCHAR_P));
E struct permonst * FDECL(colormon, (int));

/* ### mon.c ### */

E int FDECL(undead_to_corpse, (int));
E int FDECL(genus, (int,int));
E int FDECL(pm_to_cham, (int));
E int FDECL(minliquid, (struct monst *));
E int NDECL(movemon);
E int FDECL(meatmetal, (struct monst *));
E void FDECL(meatcorpse, (struct monst *));
E int FDECL(meatobj, (struct monst *));
E void FDECL(mpickgold, (struct monst *));
E boolean FDECL(mpickstuff, (struct monst *,const char *));
E int FDECL(curr_mon_load, (struct monst *));
E int FDECL(max_mon_load, (struct monst *));
E boolean FDECL(can_carry, (struct monst *,struct obj *));
E int FDECL(mfndpos, (struct monst *,coord *,long *,long));
E boolean FDECL(monnear, (struct monst *,int,int));
E void NDECL(dmonsfree);
E int FDECL(mcalcmove, (struct monst*));
E void NDECL(mcalcdistress);
E void FDECL(replmon, (struct monst *,struct monst *));
E void FDECL(relmon, (struct monst *));
E struct obj *FDECL(mlifesaver, (struct monst *));
E boolean FDECL(corpse_chance,(struct monst *,struct monst *,BOOLEAN_P));
E void FDECL(mondead, (struct monst *));
E void FDECL(mondied, (struct monst *));
E void FDECL(mongone, (struct monst *));
E void FDECL(monstone, (struct monst *));
E void FDECL(monkilled, (struct monst *,const char *,int));
E void FDECL(mon_xkilled, (struct monst *,const char *,int));
E void FDECL(unstuck, (struct monst *));
E void FDECL(killed, (struct monst *));
E void FDECL(xkilled, (struct monst *,int));
E void FDECL(mon_to_stone, (struct monst*));
E void FDECL(mnexto, (struct monst *));
E boolean FDECL(mnearto, (struct monst *,XCHAR_P,XCHAR_P,BOOLEAN_P));
E void FDECL(poisontell, (int));
E void FDECL(poisoned, (const char *,int,const char *,int));
E void FDECL(m_respond, (struct monst *));
E void FDECL(setmangry, (struct monst *));
E void FDECL(wakeup, (struct monst *));
E void NDECL(wake_nearby);
E void FDECL(wake_nearto, (int,int,int));
E void FDECL(seemimic, (struct monst *));
E void NDECL(rescham);
E void NDECL(restartcham);
E void FDECL(restore_cham, (struct monst *));
E void FDECL(mon_animal_list, (BOOLEAN_P));
E int FDECL(newcham, (struct monst *,struct permonst *,BOOLEAN_P,BOOLEAN_P));
E int FDECL(can_be_hatched, (int));
E int FDECL(egg_type_from_parent, (int,BOOLEAN_P));
E boolean FDECL(dead_species, (int,BOOLEAN_P));
E void NDECL(kill_genocided_monsters);
E void FDECL(golemeffects, (struct monst *,int,int));
E boolean FDECL(angry_guards, (BOOLEAN_P));
E void NDECL(pacify_guards);
E boolean FDECL(damage_mon,(struct monst*,int,int)); /* sporkhack */

/* ### mondata.c ### */

E void FDECL(set_mon_data, (struct monst *,struct permonst *,int));
E struct attack *FDECL(attacktype_fordmg, (struct permonst *,int,int));
E boolean FDECL(attacktype, (struct permonst *,int));
E boolean FDECL(poly_when_stoned, (struct permonst *));
E boolean FDECL(resists_drli, (struct monst *));
E boolean FDECL(resists_magm, (struct monst *));
E boolean FDECL(resists_blnd, (struct monst *));
E boolean FDECL(can_blnd, (struct monst *,struct monst *,UCHAR_P,struct obj *));
E boolean FDECL(ranged_attk, (struct permonst *));
E boolean FDECL(passes_bars, (struct permonst *));
E boolean FDECL(can_track, (struct permonst *));
E boolean FDECL(breakarm, (struct permonst *));
E boolean FDECL(sliparm, (struct permonst *));
E boolean FDECL(sticks, (struct permonst *));
E int FDECL(num_horns, (struct permonst *));
/* E boolean FDECL(canseemon, (struct monst *)); */
E struct attack *FDECL(dmgtype_fromattack, (struct permonst *,int,int));
E boolean FDECL(dmgtype, (struct permonst *,int));
E int FDECL(max_passive_dmg, (struct monst *,struct monst *));
E int FDECL(monsndx, (struct permonst *));
E int FDECL(name_to_mon, (const char *));
E int FDECL(gender, (struct monst *));
E int FDECL(pronoun_gender, (struct monst *));
E boolean FDECL(levl_follower, (struct monst *));
E int FDECL(little_to_big, (int));
E int FDECL(big_to_little, (int));
E const char *FDECL(locomotion, (const struct permonst *,const char *));
E const char *FDECL(stagger, (const struct permonst *,const char *));
E const char *FDECL(on_fire, (struct permonst *,struct attack *));
E const struct permonst *FDECL(raceptr, (struct monst *));

/* ### monmove.c ### */

E boolean FDECL(itsstuck, (struct monst *));
E boolean FDECL(mb_trapped, (struct monst *));
E void FDECL(mon_regen, (struct monst *,BOOLEAN_P));
E int FDECL(dochugw, (struct monst *));
E boolean FDECL(onscary, (int,int,struct monst *));
E void FDECL(monflee, (struct monst *, int, BOOLEAN_P, BOOLEAN_P));
E int FDECL(dochug, (struct monst *));
E int FDECL(m_move, (struct monst *,int));
E boolean FDECL(closed_door, (int,int));
E boolean FDECL(accessible, (int,int));
E void FDECL(set_apparxy, (struct monst *));
E boolean FDECL(can_ooze, (struct monst *));

/* ### monst.c ### */

E void NDECL(monst_init);

/* ### monstr.c ### */

E void NDECL(monstr_init);

/* ### mplayer.c ### */

E struct monst *FDECL(mk_mplayer, (struct permonst *,XCHAR_P,
				   XCHAR_P,BOOLEAN_P));
E void FDECL(create_mplayers, (int,BOOLEAN_P));
E void FDECL(create_umplayers, (int,BOOLEAN_P));
E void FDECL(mplayer_talk, (struct monst *));

#if defined(MICRO) || defined(WIN32)

/* ### msdos.c,os2.c,tos.c,winnt.c ### */

#  ifndef WIN32
E int NDECL(tgetch);
#  endif
#  ifndef TOS
E char NDECL(switchar);
#  endif
# ifndef __GO32__
E long FDECL(freediskspace, (char *));
#  ifdef MSDOS
E int FDECL(findfirst_file, (char *));
E int NDECL(findnext_file);
E long FDECL(filesize_nh, (char *));
#  else
E int FDECL(findfirst, (char *));
E int NDECL(findnext);
E long FDECL(filesize, (char *));
#  endif /* MSDOS */
E char *NDECL(foundfile_buffer);
# endif /* __GO32__ */
# ifndef __CYGWIN__
E void FDECL(chdrive, (char *));
# endif
# ifndef TOS
E void NDECL(disable_ctrlP);
E void NDECL(enable_ctrlP);
# endif
# if defined(MICRO) && !defined(WINNT)
E void NDECL(get_scr_size);
#  ifndef TOS
E void FDECL(gotoxy, (int,int));
#  endif
# endif
# ifdef TOS
E int FDECL(_copyfile, (char *,char *));
E int NDECL(kbhit);
E void NDECL(set_colors);
E void NDECL(restore_colors);
#  ifdef SUSPEND
E int NDECL(dosuspend);
#  endif
# endif /* TOS */
# ifdef WIN32
E char *FDECL(get_username, (int *));
E int FDECL(set_binary_mode, (int, int));
E void FDECL(nt_regularize, (char *));
E int NDECL((*nt_kbhit));
E void FDECL(Delay, (int));
# endif /* WIN32 */
#endif /* MICRO || WIN32 */

/* ### mthrowu.c ### */

E int FDECL(thitu, (int,int,struct obj *,const char *));
E int FDECL(ohitmon, (struct monst *,struct monst *,struct obj *,int,BOOLEAN_P));
E void FDECL(thrwmu, (struct monst *));
E int FDECL(spitmu, (struct monst *,struct attack *));
E int FDECL(breamu, (struct monst *,struct attack *));
E boolean FDECL(breamspot, (struct monst *, struct attack *, XCHAR_P, XCHAR_P));
E boolean FDECL(linedup, (XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P));
E boolean FDECL(linedupB, (XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P));
E boolean FDECL(lined_up, (struct monst *));
E boolean FDECL(lined_upB, (struct monst *));
E struct obj *FDECL(m_carrying, (struct monst *,int));
E void FDECL(m_useup, (struct monst *,struct obj *));
E void FDECL(m_throw, (struct monst *,int,int,int,int,int,struct obj *));
E boolean FDECL(hits_bars, (struct obj **,int,int,int,int));

/* ### muse.c ### */

E boolean FDECL(find_defensive, (struct monst *));
E int FDECL(use_defensive, (struct monst *));
E int FDECL(rnd_defensive_item, (struct monst *));
E boolean FDECL(find_offensive, (struct monst *));
#ifdef USE_TRAMPOLI
E int FDECL(mbhitm, (struct monst *,struct obj *));
#endif
E int FDECL(use_offensive, (struct monst *));
E int FDECL(rnd_offensive_item, (struct monst *));
E boolean FDECL(find_misc, (struct monst *));
E int FDECL(use_misc, (struct monst *));
E int FDECL(rnd_misc_item, (struct monst *));
E boolean FDECL(searches_for_item, (struct monst *,struct obj *));
E boolean FDECL(mon_reflects, (struct monst *,const char *));
E boolean FDECL(ureflects, (const char *,const char *));
E boolean FDECL(munstone, (struct monst *,BOOLEAN_P));

/* ### music.c ### */

E void NDECL(awaken_soldiers);
E int FDECL(do_play_instrument, (struct obj *));
E void FDECL(do_earthquake,(int));

/* ### nhlan.c ### */
#ifdef LAN_FEATURES
E void NDECL(init_lan_features);
E char *NDECL(lan_username);
# ifdef LAN_MAIL
E boolean NDECL(lan_mail_check);
E void FDECL(lan_mail_read, (struct obj *));
E void NDECL(lan_mail_init);
E void NDECL(lan_mail_finish);
E void NDECL(lan_mail_terminate);
# endif
#endif

/* ### nttty.c ### */

#ifdef WIN32CON
E void NDECL(get_scr_size);
E int NDECL(nttty_kbhit);
E void NDECL(nttty_check_stdio);
E void NDECL(nttty_open);
E void NDECL(nttty_rubout);
E int NDECL(tgetch);
E int FDECL(ntposkey,(int *, int *, int *));
E void FDECL(set_output_mode, (int));
E void NDECL(synch_cursor);
#endif

/* ### o_init.c ### */

E void NDECL(init_objects);
E int NDECL(find_skates);
E void NDECL(oinit);
E void FDECL(savenames, (int,int));
E void FDECL(restnames, (int));
E void FDECL(discover_object, (int,BOOLEAN_P,BOOLEAN_P));
E void FDECL(undiscover_object, (int));
E int NDECL(dodiscovered);

/* ### objects.c ### */

E void NDECL(objects_init);

/* ### objnam.c ### */

E char *FDECL(obj_typename, (int));
E char *FDECL(simple_typename, (int));
E boolean FDECL(obj_is_pname, (struct obj *));
E char *FDECL(distant_name, (struct obj *,char *(*)(OBJ_P)));
E char *FDECL(fruitname, (BOOLEAN_P));
E char *FDECL(xname, (struct obj *));
E char *FDECL(mshot_xname, (struct obj *));
E boolean FDECL(the_unique_obj, (struct obj *obj));
E char *FDECL(doname, (struct obj *));
E boolean FDECL(not_fully_identified, (struct obj *));
E char *FDECL(corpse_xname, (struct obj *,BOOLEAN_P));
E char *FDECL(cxname, (struct obj *));
E char *FDECL(killer_xname, (struct obj *));
E char *FDECL(killer_cxname, (struct obj *,BOOLEAN_P));
E const char *FDECL(singular, (struct obj *,char *(*)(OBJ_P)));
E char *FDECL(an, (const char *));
E char *FDECL(An, (const char *));
E char *FDECL(The, (const char *));
E char *FDECL(the, (const char *));
E char *FDECL(aobjnam, (struct obj *,const char *));
E char *FDECL(Tobjnam, (struct obj *,const char *));
E char *FDECL(otense, (struct obj *,const char *));
E char *FDECL(vtense, (const char *,const char *));
E char *FDECL(Doname2, (struct obj *));
E char *FDECL(yname, (struct obj *));
E char *FDECL(Yname2, (struct obj *));
E char *FDECL(ysimple_name, (struct obj *));
E char *FDECL(Ysimple_name2, (struct obj *));
E char *FDECL(makeplural, (const char *));
E char *FDECL(makesingular, (const char *));
E struct obj *FDECL(readobjnam, (char *,struct obj *,BOOLEAN_P));
E int FDECL(rnd_class, (int,int));
E const char *FDECL(cloak_simple_name, (struct obj *));
E const char *FDECL(mimic_obj_name, (struct monst *));

/* ### options.c ### */

E boolean FDECL(match_optname, (const char *,const char *,int,BOOLEAN_P));
E void NDECL(initoptions);
E void FDECL(parseoptions, (char *,BOOLEAN_P,BOOLEAN_P));
E void FDECL(parsetileset, (char *));
E int NDECL(doset);
E int NDECL(dotogglepickup);
E void NDECL(option_help);
E void FDECL(next_opt, (winid,const char *));
E int FDECL(fruitadd, (char *));
E int FDECL(choose_classes_menu, (const char *,int,BOOLEAN_P,char *,char *));
E void FDECL(add_menu_cmd_alias, (CHAR_P, CHAR_P));
E char FDECL(map_menu_cmd, (CHAR_P));
E void FDECL(assign_warnings, (uchar *));
E char *FDECL(nh_getenv, (const char *));
E void FDECL(set_duplicate_opt_detection, (int));
E void FDECL(set_wc_option_mod_status, (unsigned long, int));
E void FDECL(set_wc2_option_mod_status, (unsigned long, int));
E void FDECL(set_option_mod_status, (const char *, int));
#ifdef MENU_COLOR
E boolean FDECL(add_menu_coloring, (char *));
#endif
#ifdef AUTOPICKUP_EXCEPTIONS
E int FDECL(add_autopickup_exception, (const char *));
E void NDECL(free_autopickup_exceptions);
#endif /* AUTOPICKUP_EXCEPTIONS */

/* ### pager.c ### */

E int NDECL(dowhatis);
E int NDECL(doquickwhatis);
E int NDECL(doidtrap);
E int NDECL(dowhatdoes);
E char *FDECL(dowhatdoes_core,(CHAR_P, char *));
E int NDECL(dohelp);
E int NDECL(dohistory);

E const char *NDECL(fauxmessage);
E const char *NDECL(bosstaunt);

/* ### pcmain.c ### */

#if defined(MICRO) || defined(WIN32)
# ifdef CHDIR
E void FDECL(chdirx, (char *,BOOLEAN_P));
# endif /* CHDIR */
#endif /* MICRO || WIN32 */

/* ### pcsys.c ### */

#if defined(MICRO) || defined(WIN32)
E void NDECL(flushout);
E int NDECL(dosh);
# ifdef MFLOPPY
E void FDECL(eraseall, (const char *,const char *));
E void FDECL(copybones, (int));
E void NDECL(playwoRAMdisk);
E int FDECL(saveDiskPrompt, (int));
E void NDECL(gameDiskPrompt);
# endif
E void FDECL(append_slash, (char *));
E void FDECL(getreturn, (const char *));
# ifndef AMIGA
E void VDECL(msmsg, (const char *,...));
# endif
E FILE *FDECL(fopenp, (const char *,const char *));
#endif /* MICRO || WIN32 */

/* ### pctty.c ### */

#if defined(MICRO) || defined(WIN32)
E void NDECL(gettty);
E void FDECL(settty, (const char *));
E void NDECL(setftty);
E void VDECL(error, (const char *,...));
#if defined(TIMED_DELAY) && defined(_MSC_VER)
E void FDECL(msleep, (unsigned));
#endif
#endif /* MICRO || WIN32 */

/* ### pcunix.c ### */

#if defined(MICRO)
E void FDECL(regularize, (char *));
#endif /* MICRO */
#if defined(PC_LOCKING)
E void NDECL(getlock);
#endif

/* ### pickup.c ### */

#ifdef GOLDOBJ
E int FDECL(collect_obj_classes,
	(char *,struct obj *,BOOLEAN_P,boolean FDECL((*),(OBJ_P)), int *));
#else
E int FDECL(collect_obj_classes,
	(char *,struct obj *,BOOLEAN_P,BOOLEAN_P,boolean FDECL((*),(OBJ_P)), int *));
#endif
E void FDECL(add_valid_menu_class, (int));
E boolean FDECL(allow_all, (struct obj *));
E boolean FDECL(allow_category, (struct obj *));
E boolean FDECL(is_worn_by_type, (struct obj *));
E boolean FDECL(mbag_explodes, (struct obj *, int));
E void FDECL(destroy_mbag, (struct obj *, BOOLEAN_P));
#ifdef USE_TRAMPOLI
E int FDECL(ck_bag, (struct obj *));
E int FDECL(in_container, (struct obj *));
E int FDECL(out_container, (struct obj *));
#endif
E int FDECL(pickup, (int));
E int FDECL(pickup_object, (struct obj *, long, BOOLEAN_P));
E int FDECL(query_category, (const char *, struct obj *, int,
				menu_item **, int));
E int FDECL(query_objlist, (const char *, struct obj *, int,
				menu_item **, int, boolean (*)(OBJ_P)));
E struct obj *FDECL(pick_obj, (struct obj *));
E int NDECL(encumber_msg);
E int NDECL(doloot);
E boolean FDECL(container_gone, (int (*)(OBJ_P)));
E int FDECL(use_container, (struct obj **,int));
E int FDECL(loot_mon, (struct monst *,int *,boolean *));
E const char *FDECL(safe_qbuf, (const char *,unsigned,
				const char *,const char *,const char *));
E boolean FDECL(is_autopickup_exception, (struct obj *, BOOLEAN_P));

/* ### pline.c ### */

E void VDECL(pline, (const char *,...)) PRINTF_F(1,2);
E void VDECL(Norep, (const char *,...)) PRINTF_F(1,2);
E void NDECL(free_youbuf);
E void VDECL(You, (const char *,...)) PRINTF_F(1,2);
E void VDECL(Your, (const char *,...)) PRINTF_F(1,2);
E void VDECL(You_feel, (const char *,...)) PRINTF_F(1,2);
E void VDECL(You_cant, (const char *,...)) PRINTF_F(1,2);
E void VDECL(You_hear, (const char *,...)) PRINTF_F(1,2);
E void VDECL(pline_The, (const char *,...)) PRINTF_F(1,2);
E void VDECL(There, (const char *,...)) PRINTF_F(1,2);
E void VDECL(verbalize, (const char *,...)) PRINTF_F(1,2);
E void VDECL(raw_printf, (const char *,...)) PRINTF_F(1,2);
E void VDECL(impossible, (const char *,...)) PRINTF_F(1,2);
E const char *FDECL(align_str, (ALIGNTYP_P));
E const char *NDECL(hybrid_str);
E const char *NDECL(hybrid_strcode);
E void FDECL(mstatusline, (struct monst *));
E void NDECL(ustatusline);
E void NDECL(self_invis_message);

/* ### polyself.c ### */

E void NDECL(init_uasmon);
E void NDECL(set_uasmon);
E void NDECL(change_sex);
E void NDECL(newman);
E void FDECL(polyself, (BOOLEAN_P));
E int FDECL(polymon, (int));
E void NDECL(rehumanize);
E int NDECL(dobreathe);
E int NDECL(dospit);
E int NDECL(doremove);
E int NDECL(dospinweb);
E int NDECL(dosummon);
E int NDECL(dogaze);
E int NDECL(dohide);
E int NDECL(domindblast);
E void FDECL(skinback, (BOOLEAN_P));
E const char *FDECL(mbodypart, (struct monst *,int));
E const char *FDECL(body_part, (int));
E int NDECL(poly_gender);
E void FDECL(ugolemeffects, (int,int));
E int NDECL(polyatwill);

/* ### potion.c ### */

E void FDECL(set_itimeout, (long *,long));
E void FDECL(incr_itimeout, (long *,int));
E void FDECL(make_confused, (long,BOOLEAN_P));
E void FDECL(make_stunned, (long,BOOLEAN_P));
E void FDECL(make_numbed, (long,BOOLEAN_P));
E void FDECL(make_feared, (long,BOOLEAN_P));
E void FDECL(make_frozen, (long,BOOLEAN_P));
E void FDECL(make_burned, (long,BOOLEAN_P));
E void FDECL(make_blinded, (long,BOOLEAN_P));
E void FDECL(make_sick, (long, const char *, BOOLEAN_P,int));
E void FDECL(make_vomiting, (long,BOOLEAN_P));
E boolean FDECL(make_hallucinated, (long,BOOLEAN_P,long));
E int NDECL(dodrink);
E void NDECL(badeffect);
E int FDECL(dopotion, (struct obj *));
E int FDECL(peffects, (struct obj *));
E void FDECL(healup, (int,int,BOOLEAN_P,BOOLEAN_P));
E void FDECL(strange_feeling, (struct obj *,const char *));
E void FDECL(potionhit, (struct monst *,struct obj *,BOOLEAN_P));
E void FDECL(potionbreathe, (struct obj *));
E boolean FDECL(get_wet, (struct obj *, BOOLEAN_P));
E int NDECL(dodip);
E void FDECL(djinni_from_bottle, (struct obj *,int));
/* KMH, balance patch -- new function */
E int FDECL(upgrade_obj, (struct obj *));
E struct monst *FDECL(split_mon, (struct monst *,struct monst *));
E const char *NDECL(bottlename);

/* ### pray.c ### */

/*#ifdef USE_TRAMPOLI*/
E int NDECL(prayer_done);

E void FDECL(god_zaps_you,(ALIGNTYP_P));
/*#endif*/
E int NDECL(dosacrifice);
E boolean FDECL(can_pray, (BOOLEAN_P));
E int NDECL(dopray);
E const char *NDECL(u_gname);
E int NDECL(doturn);
E int NDECL(turn_undead);
E int NDECL(turn_allmonsters);
E const char *NDECL(a_gname);
E const char *FDECL(a_gname_at, (XCHAR_P x,XCHAR_P y));
E const char *FDECL(align_gname, (ALIGNTYP_P));
E const char *FDECL(halu_gname, (ALIGNTYP_P));
E const char *FDECL(align_gtitle, (ALIGNTYP_P));
E void FDECL(altar_wrath, (int,int));


/* ### priest.c ### */

E int FDECL(move_special, (struct monst *,BOOLEAN_P,SCHAR_P,BOOLEAN_P,BOOLEAN_P,
			   XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P));
E char FDECL(temple_occupied, (char *));
E int FDECL(pri_move, (struct monst *));
E void FDECL(priestini, (d_level *,struct mkroom *,int,int,BOOLEAN_P));
E char *FDECL(priestname, (struct monst *,char *));
E boolean FDECL(p_coaligned, (struct monst *));
E struct monst *FDECL(findpriest, (CHAR_P));
E void FDECL(intemple, (int));
E void FDECL(priest_talk, (struct monst *));
E struct monst *FDECL(mk_roamer, (struct permonst *,ALIGNTYP_P,
				  XCHAR_P,XCHAR_P,BOOLEAN_P));
E void FDECL(reset_hostility, (struct monst *));
E boolean FDECL(in_your_sanctuary, (struct monst *,XCHAR_P,XCHAR_P));
E void FDECL(ghod_hitsu, (struct monst *));
E void NDECL(angry_priest);
E void NDECL(clearpriests);
E void FDECL(restpriest, (struct monst *,BOOLEAN_P));

/* ### quest.c ### */

E void NDECL(onquest);
E void NDECL(nemdead);
E void NDECL(artitouch);
E boolean NDECL(ok_to_quest);
E void FDECL(leader_speaks, (struct monst *));
E void NDECL(nemesis_speaks);
E void FDECL(quest_chat, (struct monst *));
E void FDECL(quest_talk, (struct monst *));
E void FDECL(quest_stat_check, (struct monst *));
E void FDECL(finish_quest, (struct obj *));

/* ### questpgr.c ### */

E void NDECL(load_qtlist);
E void NDECL(unload_qtlist);
E short FDECL(quest_info, (int));
E const char *NDECL(ldrname);
E boolean FDECL(is_quest_artifact, (struct obj*));
E void FDECL(com_pager, (int));
E void FDECL(qt_pager, (int));
E struct permonst *NDECL(qt_montype);

/* ### random.c ### */

#if defined(RANDOM) && !defined(__GO32__) /* djgpp has its own random */
E void FDECL(srandom, (unsigned));
E char *FDECL(initstate, (unsigned,char *,int));
E char *FDECL(setstate, (char *));
E long NDECL(random);
#endif /* RANDOM */

/* ### read.c ### */

E int NDECL(doread);
E boolean FDECL(is_chargeable, (struct obj *));
E boolean FDECL(is_enchantable, (struct obj *));
E void FDECL(recharge, (struct obj *,int));
E void FDECL(randomenchant, (struct obj *,int,BOOLEAN_P));
E void FDECL(forget, (int));
E void FDECL(forget_objects, (int));
E void FDECL(forget_levels, (int));
E void NDECL(forget_traps);
E void FDECL(forget_map, (int));
E int FDECL(seffects, (struct obj *));
#ifdef USE_TRAMPOLI
E void FDECL(set_lit, (int,int,genericptr_t));
#endif
E void FDECL(litroom, (BOOLEAN_P,struct obj *));
E void FDECL(do_genocide, (int));
E void FDECL(punish, (struct obj *));
E void NDECL(unpunish);
E boolean FDECL(cant_create, (int *, BOOLEAN_P));
#ifdef WIZARD
E struct monst *NDECL(create_particular);
#endif

/* ### rect.c ### */

E void NDECL(init_rect);
E NhRect *FDECL(get_rect, (NhRect *));
E NhRect *NDECL(rnd_rect);
E void FDECL(remove_rect, (NhRect *));
E void FDECL(add_rect, (NhRect *));
E void FDECL(split_rects, (NhRect *,NhRect *));

/* ## region.c ### */
E void NDECL(clear_regions);
E void NDECL(run_regions);
E boolean FDECL(in_out_region, (XCHAR_P,XCHAR_P));
E boolean FDECL(m_in_out_region, (struct monst *,XCHAR_P,XCHAR_P));
E void NDECL(update_player_regions);
E void FDECL(update_monster_region, (struct monst *));
E NhRegion *FDECL(visible_region_at, (XCHAR_P,XCHAR_P));
E void FDECL(show_region, (NhRegion*, XCHAR_P, XCHAR_P));
E void FDECL(save_regions, (int,int));
E void FDECL(rest_regions, (int,BOOLEAN_P));
E NhRegion* FDECL(create_gas_cloud, (XCHAR_P, XCHAR_P, int, int));
E NhRegion* FDECL(create_cthulhu_death_cloud, (XCHAR_P, XCHAR_P, int, int));

/* ### restore.c ### */

E void FDECL(inven_inuse, (BOOLEAN_P));
E int FDECL(dorecover, (int));
E void FDECL(trickery, (char *));
E void FDECL(getlev, (int,int,XCHAR_P,BOOLEAN_P));
E void NDECL(minit);
E boolean FDECL(lookup_id_mapping, (unsigned, unsigned *));
#ifdef ZEROCOMP
E int FDECL(mread, (int,genericptr_t,unsigned int));
#else
E void FDECL(mread, (int,genericptr_t,unsigned int));
#endif

/* ### rip.c ### */

E void FDECL(genl_outrip, (winid,int));

/* ### rnd.c ### */

E int FDECL(rn2, (int));
E int FDECL(rnl, (int));
E int FDECL(rnd, (int));
E int FDECL(d, (int,int));
E int FDECL(rne, (int));
E int FDECL(rnz, (int));
E int NDECL(randattack);
E int NDECL(randenchantment);

/* ### role.c ### */

E boolean FDECL(validrole, (int));
E boolean FDECL(validrace, (int, int));
E boolean FDECL(validgend, (int, int, int));
E boolean FDECL(validalign, (int, int, int));
E int NDECL(randrole);
E int FDECL(randrace, (int));
E int FDECL(randgend, (int, int));
E int FDECL(randalign, (int, int));
E int FDECL(str2role, (char *));
E int FDECL(str2race, (char *));
E int FDECL(str2gend, (char *));
E int FDECL(str2align, (char *));
E int FDECL(mrace2race, (int));
E boolean FDECL(ok_role, (int, int, int, int));
E int FDECL(pick_role, (int, int, int, int));
E boolean FDECL(ok_race, (int, int, int, int));
E int FDECL(pick_race, (int, int, int, int));
E boolean FDECL(ok_gend, (int, int, int, int));
E int FDECL(pick_gend, (int, int, int, int));
E boolean FDECL(ok_align, (int, int, int, int));
E int FDECL(pick_align, (int, int, int, int));
E void NDECL(role_init);
E void NDECL(rigid_role_checks);
E void NDECL(plnamesuffix);
E const char *FDECL(Hello, (struct monst *));
E const char *NDECL(Goodbye);
E char *FDECL(build_plselection_prompt, (char *, int, int, int, int, int));
E char *FDECL(root_plselection_prompt, (char *, int, int, int, int, int));
E void NDECL(recursion_effect);

/* ### rumors.c ### */

E char *FDECL(getrumor, (int,char *, BOOLEAN_P));
E void FDECL(outrumor, (int,int));
E void FDECL(outoracle, (BOOLEAN_P, BOOLEAN_P));
E void FDECL(save_oracles, (int,int));
E void FDECL(restore_oracles, (int));
E int FDECL(doconsult, (struct monst *));

/* ### save.c ### */

E int NDECL(dosave);
#if defined(UNIX) || defined(VMS) || defined(__EMX__) || defined(WIN32)
E void FDECL(hangup, (int));
#endif
E int NDECL(dosave0);
#ifdef INSURANCE
E void NDECL(savestateinlock);
#endif
#ifdef MFLOPPY
E boolean FDECL(savelev, (int,XCHAR_P,int));
E boolean FDECL(swapin_file, (int));
E void NDECL(co_false);
#else
E void FDECL(savelev, (int,XCHAR_P,int));
#endif
E void FDECL(bufon, (int));
E void FDECL(bufoff, (int));
E void FDECL(bflush, (int));
E void FDECL(bwrite, (int,genericptr_t,unsigned int));
E void FDECL(bclose, (int));
E void FDECL(savefruitchn, (int,int));
/* ### school.c ### */

E boolean FDECL(is_principal, (struct monst *));
E boolean FDECL(is_teacher, (struct monst *));
E boolean FDECL(is_educator, (struct monst *));
#ifdef SOUNDS
E void FDECL(pupil_sound, (struct monst *));
E void FDECL(teacher_sound, (struct monst *));
E void FDECL(principal_sound, (struct monst *));
#endif

E void NDECL(free_dungeons);
E void NDECL(freedynamicdata);

/* ### shk.c ### */

#ifdef GOLDOBJ
E long FDECL(money2mon, (struct monst *, long));
E void FDECL(money2u, (struct monst *, long));
#endif
E char *FDECL(shkname, (struct monst *));
E void FDECL(shkgone, (struct monst *));
E void FDECL(set_residency, (struct monst *,BOOLEAN_P));
E void FDECL(replshk, (struct monst *,struct monst *));
E void FDECL(restshk, (struct monst *,BOOLEAN_P));
E char FDECL(inside_shop, (XCHAR_P,XCHAR_P));
E void FDECL(u_left_shop, (char *,BOOLEAN_P));
E void FDECL(remote_burglary, (XCHAR_P,XCHAR_P));
E void FDECL(u_entered_shop, (char *));
E boolean FDECL(same_price, (struct obj *,struct obj *));
E void NDECL(shopper_financial_report);
E int FDECL(inhishop, (struct monst *));
E struct monst *FDECL(shop_keeper, (CHAR_P));
E boolean FDECL(tended_shop, (struct mkroom *));
E void FDECL(delete_contents, (struct obj *));
E void FDECL(obfree, (struct obj *,struct obj *));
E void FDECL(home_shk, (struct monst *,BOOLEAN_P));
E void FDECL(make_happy_shk, (struct monst *,BOOLEAN_P));
E void FDECL(hot_pursuit, (struct monst *));
E void FDECL(make_angry_shk, (struct monst *,XCHAR_P,XCHAR_P));
E int NDECL(dopay);
E boolean FDECL(paybill, (int));
E void NDECL(finish_paybill);
E struct obj *FDECL(find_oid, (unsigned));
E long FDECL(contained_cost, (struct obj *,struct monst *,long,BOOLEAN_P, BOOLEAN_P));
E long FDECL(contained_gold, (struct obj *));
E void FDECL(picked_container, (struct obj *));
E long FDECL(unpaid_cost, (struct obj *));
E void FDECL(addtobill, (struct obj *,BOOLEAN_P,BOOLEAN_P,BOOLEAN_P));
E void FDECL(splitbill, (struct obj *,struct obj *));
E void FDECL(subfrombill, (struct obj *,struct monst *));
E long FDECL(stolen_value, (struct obj *,XCHAR_P,XCHAR_P,BOOLEAN_P,BOOLEAN_P,
			    BOOLEAN_P));
E void FDECL(sellobj_state, (int));
E void FDECL(sellobj, (struct obj *,XCHAR_P,XCHAR_P));
E int FDECL(doinvbill, (int));
E struct monst *FDECL(shkcatch, (struct obj *,XCHAR_P,XCHAR_P));
E void FDECL(add_damage, (XCHAR_P,XCHAR_P,long));
E int FDECL(repair_damage, (struct monst *,struct damage *,BOOLEAN_P));
E int FDECL(shk_move, (struct monst *));
E void FDECL(after_shk_move, (struct monst *));
E boolean FDECL(is_fshk, (struct monst *));
E void FDECL(shopdig, (int));
E void FDECL(pay_for_damage, (const char *,BOOLEAN_P));
E boolean FDECL(costly_spot, (XCHAR_P,XCHAR_P));
E struct obj *FDECL(shop_object, (XCHAR_P,XCHAR_P));
E void FDECL(price_quote, (struct obj *));
E void FDECL(shk_chat, (struct monst *));
E void FDECL(check_unpaid_usage, (struct obj *,BOOLEAN_P));
E void FDECL(check_unpaid, (struct obj *));
E void FDECL(costly_gold, (XCHAR_P,XCHAR_P,long));
E boolean FDECL(block_door, (XCHAR_P,XCHAR_P));
E boolean FDECL(block_entry, (XCHAR_P,XCHAR_P));
E boolean FDECL(block_entry, (XCHAR_P,XCHAR_P));
#ifdef BLACKMARKET
E void FDECL(blkmar_guards, (struct monst *));
#endif /* BLACKMARKET */
E char *FDECL(shk_your, (char *,struct obj *));
E char *FDECL(Shk_Your, (char *,struct obj *));

/* ### shknam.c ### */

E void FDECL(stock_room, (int,struct mkroom *));
E boolean FDECL(saleable, (struct monst *,struct obj *));
E int FDECL(get_shop_item, (int));

/* ### sit.c ### */

E void NDECL(take_gold);
E int NDECL(dosit);
E void NDECL(rndcurse);
E void NDECL(attrcurse);

/* ### sounds.c ### */

E void NDECL(dosounds);
E void FDECL(pet_distress, (struct monst *, int));
E const char *FDECL(growl_sound, (struct monst *));
/* JRN: converted growl,yelp,whimper to macros based on pet_distress.
  Putting them here since I don't know where else (TOFIX) */
#define growl(mon) pet_distress((mon),3)
#define yelp(mon) pet_distress((mon),2)
#define whimper(mon) pet_distress((mon),1)
E void FDECL(beg, (struct monst *));
E int NDECL(dotalk);
#ifdef USER_SOUNDS
E int FDECL(add_sound_mapping, (const char *));
E void FDECL(play_sound_for_message, (const char *));
#endif

/* ### sys/msdos/sound.c ### */

#ifdef MSDOS
E int FDECL(assign_soundcard, (char *));
#endif

/* ### sp_lev.c ### */

E boolean FDECL(check_room, (xchar *,xchar *,xchar *,xchar *,BOOLEAN_P));
E boolean FDECL(create_room, (XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,
			      XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,BOOLEAN_P));
E void FDECL(create_secret_door, (struct mkroom *,XCHAR_P));
E boolean FDECL(dig_corridor, (coord *,coord *,BOOLEAN_P,SCHAR_P,SCHAR_P));
E void FDECL(fill_room, (struct mkroom *,BOOLEAN_P));
E boolean FDECL(load_special, (const char *));
E int NDECL(randomtrap);
E void NDECL(makerandomtrap);
E void NDECL(makegirlytrap);

/* ### spell.c ### */

E boolean FDECL(spell_known, (int));
#ifdef USE_TRAMPOLI
E int NDECL(learn);
#endif
E int FDECL(study_book, (struct obj *));
E void FDECL(book_disappears, (struct obj *));
E void FDECL(book_substitution, (struct obj *,struct obj *));
E void NDECL(age_spells);
E int NDECL(docast);
E int FDECL(spell_skilltype, (int));
E int FDECL(spelleffects, (int,BOOLEAN_P));
E void NDECL(losespells);
E int NDECL(dovspell);
E void FDECL(learnspell, (struct obj *));
E boolean NDECL(studyspell);
E void FDECL(initialspell, (struct obj *));

/* ### steal.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(stealarm);
#endif
#ifdef GOLDOBJ
E long FDECL(somegold, (long));
#else
E long NDECL(somegold);
#endif
E void FDECL(stealgold, (struct monst *));
E void FDECL(remove_worn_item, (struct obj *,BOOLEAN_P));
E int FDECL(steal, (struct monst *, char *));
E int FDECL(mpickobj, (struct monst *,struct obj *,BOOLEAN_P));
E void FDECL(stealamulet, (struct monst *));
E void FDECL(mdrop_special_objs, (struct monst *));
E void FDECL(relobj, (struct monst *,int,BOOLEAN_P));
#ifdef GOLDOBJ
E struct obj *FDECL(findgold, (struct obj *));
#endif

/* ### steed.c ### */

#ifdef STEED
E void NDECL(rider_cant_reach);
E boolean FDECL(can_saddle, (struct monst *));
E int FDECL(use_saddle, (struct obj *));
E boolean FDECL(can_ride, (struct monst *));
E int NDECL(doride);
E boolean FDECL(mount_steed, (struct monst *, BOOLEAN_P));
E void NDECL(exercise_steed);
E void NDECL(kick_steed);
E void FDECL(dismount_steed, (int));
E void FDECL(place_monster, (struct monst *,int,int));
#endif

/* ### tech.c ### */

E void FDECL(adjtech, (int,int));
E int NDECL(dotech);
E void NDECL(docalm);
E int FDECL(tech_inuse, (int));
E void NDECL(tech_timeout);
E boolean FDECL(tech_known, (SHORT_P));
E void FDECL(learntech, (SHORT_P,long,int));
E int FDECL(disarm_holdingtrap, (struct trap *));
E int FDECL(disarm_rust_trap, (struct trap *));
E int FDECL(disarm_fire_trap, (struct trap *));
E int FDECL(disarm_landmine, (struct trap *));
E int FDECL(disarm_squeaky_board, (struct trap *));
E int FDECL(disarm_shooting_trap, (struct trap *, int));


/* ### teleport.c ### */

E boolean FDECL(goodpos, (int,int,struct monst *,unsigned));
E boolean FDECL(enexto, (coord *,XCHAR_P,XCHAR_P,struct permonst *));
E boolean FDECL(enexto_core, (coord *,XCHAR_P,XCHAR_P,struct permonst *,unsigned));
E int FDECL(epathto, (coord *,int,XCHAR_P,XCHAR_P,struct permonst *));
E void FDECL(xpathto, (int,XCHAR_P,XCHAR_P,int (*)(genericptr_t,int,int),void *));
E void FDECL(teleds, (int,int,BOOLEAN_P));
E boolean FDECL(safe_teleds, (BOOLEAN_P));
E boolean FDECL(safe_teledsPD, (BOOLEAN_P));
E boolean FDECL(teleport_pet, (struct monst *,BOOLEAN_P));
E void NDECL(tele);
E void FDECL(phase_door, (BOOLEAN_P));
E int NDECL(dotele);
E void NDECL(level_tele);
E void FDECL(domagicportal, (struct trap *));
E void FDECL(tele_trap, (struct trap *));
E void FDECL(level_tele_trap, (struct trap *));
E void FDECL(rloc_to, (struct monst *,int,int));
E boolean FDECL(rloc, (struct monst *, BOOLEAN_P));
E boolean FDECL(tele_restrict, (struct monst *));
E void FDECL(mtele_trap, (struct monst *, struct trap *,int));
E int FDECL(mlevel_tele_trap, (struct monst *, struct trap *,BOOLEAN_P,int));
E void FDECL(rloco, (struct obj *));
E int NDECL(random_teleport_level);
E int NDECL(random_banish_level);
E boolean FDECL(u_teleport_mon, (struct monst *,BOOLEAN_P));
E boolean FDECL(u_teleport_monB, (struct monst *,BOOLEAN_P));

/* ### tile.c ### */
#ifdef USE_TILES
E void FDECL(substitute_tiles, (d_level *));
#endif

/* ### timeout.c ### */

E void NDECL(burn_away_slime);
E void NDECL(nh_timeout);
E void FDECL(fall_asleep, (int, BOOLEAN_P));
#ifdef UNPOLYPILE
E void FDECL(set_obj_poly, (struct obj *, struct obj *));
E void FDECL(unpoly_obj, (genericptr_t, long));
#endif
E int FDECL(mon_poly, (struct monst *, BOOLEAN_P, const char *));
E int FDECL(mon_spec_poly, (struct monst *, struct permonst *, long,
			    BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E int FDECL(mon_spec_polyX, (struct monst *, struct permonst *, long,
			    BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E void FDECL(unpoly_mon, (genericptr_t, long));
E void FDECL(attach_bomb_blow_timeout, (struct obj *, int, BOOLEAN_P));
E void FDECL(attach_egg_hatch_timeout, (struct obj *));
E void FDECL(attach_fig_transform_timeout, (struct obj *));
E void FDECL(kill_egg, (struct obj *));
E void FDECL(hatch_egg, (genericptr_t, long));
E void FDECL(learn_egg_type, (int));
E void FDECL(burn_object, (genericptr_t, long));
E void FDECL(begin_burn, (struct obj *, BOOLEAN_P));
E void FDECL(end_burn, (struct obj *, BOOLEAN_P));
E void FDECL(burn_faster, (struct obj *, long));
#ifdef LIGHTSABERS
E void FDECL(lightsaber_deactivate, (struct obj *, BOOLEAN_P));
#endif
E void NDECL(do_storms);
E boolean FDECL(start_timer, (long, SHORT_P, SHORT_P, genericptr_t));
E long FDECL(stop_timer, (SHORT_P, genericptr_t));
E void NDECL(run_timers);
E void FDECL(obj_move_timers, (struct obj *, struct obj *));
E void FDECL(obj_split_timers, (struct obj *, struct obj *));
E void FDECL(obj_stop_timers, (struct obj *));
E void FDECL(mon_stop_timers, (struct monst *));
E boolean FDECL(obj_is_local, (struct obj *));
E void FDECL(save_timers, (int,int,int));
E void FDECL(restore_timers, (int,int,BOOLEAN_P,long));
E void FDECL(relink_timers, (BOOLEAN_P));
#ifdef WIZARD
E int NDECL(wiz_timeout_queue);
E void NDECL(timer_sanity_check);
#endif

/* ### topten.c ### */

E void FDECL(topten, (int));
E void FDECL(prscore, (int,char **));
E struct obj *FDECL(tt_oname, (struct obj *));
/*E void FDECL(tt_mname, (struct monst *));*/
/*E void FDECL(mkundeadboo, (coord *,BOOLEAN_P,int));*/
E void FDECL(tt_mname, (coord *,BOOLEAN_P,int));
#ifdef GTK_GRAPHICS
E winid NDECL(create_toptenwin);
E void NDECL(destroy_toptenwin);
#endif

/* ### track.c ### */

E void NDECL(initrack);
E void NDECL(settrack);
E coord *FDECL(gettrack, (int,int));

/* ### trap.c ### */

E boolean FDECL(burnarmor,(struct monst *));
E boolean FDECL(rust_dmg, (struct obj *,const char *,int,BOOLEAN_P,struct monst *));

E boolean FDECL(wither_dmg, (struct obj *,const char *,int,BOOLEAN_P,struct monst *));

E void FDECL(grease_protect, (struct obj *,const char *,struct monst *));
E struct trap *FDECL(maketrap, (int,int,int));
E void FDECL(makerandomtrap_at, (int,int));
E void FDECL(fall_through, (BOOLEAN_P));
E void FDECL(fall_throughX, (BOOLEAN_P));
E struct monst *FDECL(animate_statue, (struct obj *,XCHAR_P,XCHAR_P,int,int *));
E struct monst *FDECL(activate_statue_trap,
			(struct trap *,XCHAR_P,XCHAR_P,BOOLEAN_P));
E void FDECL(dotrap, (struct trap *, unsigned));
E void FDECL(seetrap, (struct trap *));
E int FDECL(mintrap, (struct monst *));
E void FDECL(instapetrify, (const char *));
E void FDECL(minstapetrify, (struct monst *,BOOLEAN_P));
E void FDECL(selftouch, (const char *));
E void FDECL(mselftouch, (struct monst *,const char *,BOOLEAN_P));
E void NDECL(float_up);
E void FDECL(fill_pit, (int,int));
E int FDECL(float_down, (long, long));
E int FDECL(fire_damage, (struct obj *,BOOLEAN_P,BOOLEAN_P,XCHAR_P,XCHAR_P));
E void FDECL(water_damage, (struct obj *,BOOLEAN_P,BOOLEAN_P));
E void FDECL(lethe_damage, (struct obj *,BOOLEAN_P,BOOLEAN_P));
E void FDECL(withering_damage, (struct obj *,BOOLEAN_P,BOOLEAN_P));
E boolean NDECL(drown);
E void FDECL(mon_drain_en, (struct monst *, int));
E void FDECL(drain_en, (int));
E int NDECL(dountrap);
E int FDECL(untrap, (BOOLEAN_P));
E boolean FDECL(chest_trap, (struct obj *,int,BOOLEAN_P));
E void FDECL(deltrap, (struct trap *));
E boolean FDECL(delfloortrap, (struct trap *));
E struct trap *FDECL(t_at, (int,int));
E void FDECL(b_trapped, (const char *,int));
E boolean NDECL(unconscious);
E boolean NDECL(lava_effects);
E void FDECL(blow_up_landmine, (struct trap *));
E int FDECL(launch_obj,(SHORT_P,int,int,int,int,int));
/* KMH, balance patch -- new function */
E int NDECL(uunstone);

/* ### u_init.c ### */

E void NDECL(u_init);
E void NDECL(alter_reality);

/* ### uhitm.c ### */

E void FDECL(hurtmarmor,(struct monst *,int));
E int FDECL(attack_checks, (struct monst *,BOOLEAN_P));
E void FDECL(check_caitiff, (struct monst *));
E schar FDECL(find_roll_to_hit, (struct monst *));
E boolean FDECL(attack, (struct monst *));
E boolean FDECL(hmon, (struct monst *,struct obj *,int));
E int FDECL(damageum, (struct monst *,struct attack *));
E void FDECL(missum, (struct monst *,int, int, struct attack *));
E int FDECL(passive, (struct monst *,int,int,UCHAR_P));
E void FDECL(passive_obj, (struct monst *,struct obj *,struct attack *));
E void FDECL(stumble_onto_mimic, (struct monst *));
E int FDECL(flash_hits_mon, (struct monst *,struct obj *));

/* ### unixmain.c ### */

#ifdef UNIX
# ifdef PORT_HELP
E void NDECL(port_help);
# endif
#endif /* UNIX */

/* ### unixtty.c ### */

#if defined(UNIX) || defined(__BEOS__)
E void NDECL(gettty);
E void FDECL(settty, (const char *));
E void NDECL(setftty);
E void NDECL(intron);
E void NDECL(introff);
E void VDECL(error, (const char *,...)) PRINTF_F(1,2);
#endif /* UNIX || __BEOS_ */

/* ### unixunix.c ### */

#ifdef UNIX
E void NDECL(getlock);
E void FDECL(regularize, (char *));
# if defined(TIMED_DELAY) && !defined(msleep) && defined(SYSV)
E void FDECL(msleep, (unsigned));
# endif
# ifdef SHELL
E int NDECL(dosh);
# endif /* SHELL */
# if defined(SHELL) || defined(DEF_PAGER) || defined(DEF_MAILREADER)
E int FDECL(child, (int));
# endif
#ifdef FILE_AREAS
E char *FDECL(make_file_name, (const char *, const char *));
E FILE *FDECL(fopen_datafile_area, (const char *,const char *,const char *,
				BOOLEAN_P));
E FILE *FDECL(freopen_area, (const char *,const char *,const char *, FILE *));
E int FDECL(chmod_area, (const char *, const char *, int));
E int FDECL(open_area, (const char *, const char *, int, int));
E int FDECL(creat_area, (const char *, const char *, int));
E boolean FDECL(lock_file_area, (const char *, const char *,int));
E void FDECL(unlock_file_area, (const char *, const char *));
#endif
#endif /* UNIX */

/* ### unixres.c ### */

#ifdef UNIX
E int FDECL(hide_privileges, (BOOLEAN_P));
#endif /* UNIX */

/* ### unixres.c ### */

#ifdef UNIX
# ifdef GNOME_GRAPHICS 
E int FDECL(hide_privileges, (BOOLEAN_P));
# endif
#endif /* UNIX */

/* ### vault.c ### */

E boolean FDECL(grddead, (struct monst *));
E char FDECL(vault_occupied, (char *));
E void NDECL(invault);
E int FDECL(gd_move, (struct monst *));
E void NDECL(paygd);
E long NDECL(hidden_gold);
E boolean NDECL(gd_sound);

/* ### version.c ### */

E char *FDECL(version_string, (char *));
E char *FDECL(getversionstring, (char *));
E int NDECL(doversion);
E int NDECL(doextversion);
#ifdef MICRO
E boolean FDECL(comp_times, (long));
#endif
E boolean FDECL(check_version, (struct version_info *,
				const char *,BOOLEAN_P));
E unsigned long FDECL(get_feature_notice_ver, (char *));
E unsigned long NDECL(get_current_feature_ver);
#ifdef RUNTIME_PORT_ID
E void FDECL(append_port_id, (char *));
#endif

/* ### video.c ### */

#ifdef MSDOS
E int FDECL(assign_video, (char *));
# ifdef NO_TERMS
E void NDECL(gr_init);
E void NDECL(gr_finish);
# endif
E void FDECL(tileview,(BOOLEAN_P));
#endif
#ifdef VIDEOSHADES
E int FDECL(assign_videoshades, (char *));
E int FDECL(assign_videocolors, (char *));
#endif

/* ### vis_tab.c ### */

#ifdef VISION_TABLES
E void NDECL(vis_tab_init);
#endif

/* ### vision.c ### */

E void NDECL(vision_init);
E int FDECL(does_block, (int,int,struct rm*));
E void NDECL(vision_reset);
E void FDECL(vision_recalc, (int));
E void FDECL(block_point, (int,int));
E void FDECL(unblock_point, (int,int));
E boolean FDECL(clear_path, (int,int,int,int));
E void FDECL(do_clear_area, (int,int,int,
			     void (*)(int,int,genericptr_t),genericptr_t));
E void FDECL(do_clear_areaX, (int,int,int,
			     void (*)(int,int,genericptr_t),genericptr_t));

#ifdef VMS

/* ### vmsfiles.c ### */

E int FDECL(vms_link, (const char *,const char *));
E int FDECL(vms_unlink, (const char *));
E int FDECL(vms_creat, (const char *,unsigned int));
E int FDECL(vms_open, (const char *,int,unsigned int));
E boolean FDECL(same_dir, (const char *,const char *));
E int FDECL(c__translate, (int));
E char *FDECL(vms_basename, (const char *));

/* ### vmsmail.c ### */

E unsigned long NDECL(init_broadcast_trapping);
E unsigned long NDECL(enable_broadcast_trapping);
E unsigned long NDECL(disable_broadcast_trapping);
# if 0
E struct mail_info *NDECL(parse_next_broadcast);
# endif /*0*/

/* ### vmsmain.c ### */

E int FDECL(main, (int, char **));
# ifdef CHDIR
E void FDECL(chdirx, (const char *,BOOLEAN_P));
# endif /* CHDIR */

/* ### vmsmisc.c ### */

E void NDECL(vms_abort);
E void FDECL(vms_exit, (int));

/* ### vmstty.c ### */

E int NDECL(vms_getchar);
E void NDECL(gettty);
E void FDECL(settty, (const char *));
E void FDECL(shuttty, (const char *));
E void NDECL(setftty);
E void NDECL(intron);
E void NDECL(introff);
E void VDECL(error, (const char *,...)) PRINTF_F(1,2);
#ifdef TIMED_DELAY
E void FDECL(msleep, (unsigned));
#endif

/* ### vmsunix.c ### */

E void NDECL(getlock);
E void FDECL(regularize, (char *));
E int NDECL(vms_getuid);
E boolean FDECL(file_is_stmlf, (int));
E int FDECL(vms_define, (const char *,const char *,int));
E int FDECL(vms_putenv, (const char *));
E char *NDECL(verify_termcap);
# if defined(CHDIR) || defined(SHELL) || defined(SECURE)
E void NDECL(privoff);
E void NDECL(privon);
# endif
# ifdef SHELL
E int NDECL(dosh);
# endif
# if defined(SHELL) || defined(MAIL)
E int FDECL(vms_doshell, (const char *,BOOLEAN_P));
# endif
# ifdef SUSPEND
E int NDECL(dosuspend);
# endif

#endif /* VMS */

/* ### weapon.c ### */

E int FDECL(hitval, (struct obj *,struct monst *));
E int FDECL(dmgval, (struct obj *,struct monst *));
E int FDECL(dmgvalX, (struct obj *,struct monst *));
E struct obj *FDECL(select_rwep, (struct monst *));
E struct obj *FDECL(select_hwep, (struct monst *));
E void FDECL(possibly_unwield, (struct monst *,BOOLEAN_P));
E int FDECL(mon_wield_item, (struct monst *));
E int NDECL(abon);
E int NDECL(dbon);
E int NDECL(enhance_weapon_skill);
#ifdef DUMP_LOG
E void NDECL(dump_weapon_skill);
#endif
E void FDECL(unrestrict_weapon_skill, (int));
E void FDECL(use_skill, (int,int));
E void FDECL(add_weapon_skill, (int));
E void FDECL(lose_weapon_skill, (int));
E int FDECL(weapon_type, (struct obj *));
E int NDECL(uwep_skill_type);
E int FDECL(weapon_hit_bonus, (struct obj *));
E int FDECL(weapon_dam_bonus, (struct obj *));
E int FDECL(skill_bonus, (int));
E void FDECL(skill_init, (const struct def_skill *));
E void NDECL(practice_weapon);
E void NDECL(xtraskillinit);

/* ### were.c ### */

E int FDECL(counter_were,(int));
E void FDECL(were_change, (struct monst *));
E void FDECL(new_were, (struct monst *));
E int FDECL(were_summon, (struct permonst *,BOOLEAN_P,int *,char *));
E void NDECL(you_were);
E void FDECL(you_unwere, (BOOLEAN_P));

/* ### wield.c ### */

E void FDECL(setuwep, (struct obj *,BOOLEAN_P));
E void FDECL(setuqwep, (struct obj *));
E void FDECL(setuswapwep, (struct obj *,BOOLEAN_P));
E int NDECL(dowield);
E int NDECL(doswapweapon);
E int NDECL(dowieldquiver);
E boolean FDECL(wield_tool, (struct obj *,const char *));
E int NDECL(can_twoweapon);
E void NDECL(drop_uswapwep);
E int NDECL(dotwoweapon);
E void NDECL(uwepgone);
E void NDECL(uswapwepgone);
E void NDECL(uqwepgone);
E void NDECL(untwoweapon);
E void FDECL(erode_obj, (struct obj *,BOOLEAN_P,BOOLEAN_P));
E int FDECL(chwepon, (struct obj *,int));
E int FDECL(welded, (struct obj *));
E void FDECL(weldmsg, (struct obj *));
E void FDECL(setmnotwielded, (struct monst *,struct obj *));
E void FDECL(unwield, (struct obj *,BOOLEAN_P));

/* ### windows.c ### */

E void FDECL(choose_windows, (const char *));
E char FDECL(genl_message_menu, (CHAR_P,int,const char *));
E void FDECL(genl_preference_update, (const char *));

/* ### wizard.c ### */

E void NDECL(amulet);
E int FDECL(mon_has_amulet, (struct monst *));
E int FDECL(mon_has_special, (struct monst *));
E int FDECL(tactics, (struct monst *));
E void NDECL(aggravate);
E void NDECL(clonewiz);
E int NDECL(pick_nasty);
E int FDECL(nasty, (struct monst*));
E void NDECL(resurrect);
E void NDECL(intervene);
E void NDECL(wizdead);
E void FDECL(cuss, (struct monst *));

/* ### worm.c ### */

E int NDECL(get_wormno);
E void FDECL(initworm, (struct monst *,int));
E void FDECL(worm_move, (struct monst *));
E void FDECL(worm_nomove, (struct monst *));
E void FDECL(wormgone, (struct monst *));
E void FDECL(wormhitu, (struct monst *));
E int FDECL(cutworm, (struct monst *,XCHAR_P,XCHAR_P,struct obj *));
E void FDECL(see_wsegs, (struct monst *));
E void FDECL(detect_wsegs, (struct monst *,BOOLEAN_P));
E void FDECL(save_worm, (int,int));
E void FDECL(rest_worm, (int));
E void FDECL(place_wsegs, (struct monst *));
E void FDECL(remove_worm, (struct monst *));
E void FDECL(place_worm_tail_randomly, (struct monst *,XCHAR_P,XCHAR_P));
E int FDECL(count_wsegs, (struct monst *));
E boolean FDECL(worm_known, (struct monst *));

/* ### worn.c ### */

E void FDECL(setworn, (struct obj *,long));
E void FDECL(setnotworn, (struct obj *));
E void FDECL(mon_set_minvis, (struct monst *));
E void FDECL(mon_adjust_speed, (struct monst *,int,struct obj *));
E void FDECL(update_mon_intrinsics,
		(struct monst *,struct obj *,BOOLEAN_P,BOOLEAN_P));
E int FDECL(find_mac, (struct monst *));
E void FDECL(m_dowear, (struct monst *,BOOLEAN_P));
E struct obj *FDECL(which_armor, (struct monst *,long));
E void FDECL(mon_break_armor, (struct monst *,BOOLEAN_P));
E void FDECL(bypass_obj, (struct obj *));
E void NDECL(clear_bypasses);
E int FDECL(racial_exception, (struct monst *, struct obj *));

/* ### write.c ### */

E int FDECL(dowrite, (struct obj *));

/* ### zap.c ### */

E int FDECL(bhitm, (struct monst *,struct obj *));
E void FDECL(probe_monster, (struct monst *));
E boolean FDECL(get_obj_location, (struct obj *,xchar *,xchar *,int));
E boolean FDECL(get_mon_location, (struct monst *,xchar *,xchar *,int));
E struct monst *FDECL(get_container_location, (struct obj *obj, int *, int *));
E struct monst *FDECL(montraits, (struct obj *,coord *));
E struct monst *FDECL(revive, (struct obj *));
E int FDECL(unturn_dead, (struct monst *));
E void FDECL(cancel_item, (struct obj *));
E boolean FDECL(drain_item, (struct obj *));	/* KMH */
E boolean FDECL(drain_item_negative, (struct obj *));	/* KMH */
E boolean FDECL(obj_resists, (struct obj *,int,int));
E boolean FDECL(obj_shudders, (struct obj *));
E void FDECL(do_osshock, (struct obj *));
E void FDECL(puton_worn_item, (struct obj *));
E struct obj *FDECL(poly_obj, (struct obj *, int));
E int FDECL(bhito, (struct obj *,struct obj *));
E int FDECL(bhitpile, (struct obj *,int (*)(OBJ_P,OBJ_P),int,int));
E int FDECL(zappable, (struct obj *));
E void FDECL(zapnodir, (struct obj *));
E int NDECL(dozap);
E int FDECL(zapyourself, (struct obj *,BOOLEAN_P));
E boolean FDECL(cancel_monst, (struct monst *,struct obj *,
			       BOOLEAN_P,BOOLEAN_P,BOOLEAN_P));
E void FDECL(weffects, (struct obj *));
E int FDECL(spell_damage_bonus, (int));
E const char *FDECL(exclam, (int force));
E void FDECL(hit, (const char *,struct monst *,const char *));
E void FDECL(miss, (const char *,struct monst *));
E struct monst *FDECL(bhit, (int,int,int,int,int (*)(MONST_P,OBJ_P),
			     int (*)(OBJ_P,OBJ_P),struct obj **));
E struct monst *FDECL(boomhit, (int,int));
E int FDECL(burn_floor_paper, (int,int,BOOLEAN_P,BOOLEAN_P));
E void FDECL(buzz, (int,int,XCHAR_P,XCHAR_P,int,int));
E void FDECL(melt_ice, (XCHAR_P,XCHAR_P));
E int FDECL(zap_over_floor, (XCHAR_P,XCHAR_P,int,boolean *));
E void FDECL(fracture_rock, (struct obj *));
E boolean FDECL(break_statue, (struct obj *));
E void FDECL(destroy_item, (int,int));
E int FDECL(destroy_mitem, (struct monst *,int,int));
E int FDECL(resist, (struct monst *,CHAR_P,int,int));
E void NDECL(makewish);
/* KMH -- xchar to XCHAR_P */
E void FDECL(zap_strike_fx, (XCHAR_P, XCHAR_P, int));
E void NDECL(throwspell);

/* ### livelog.c ### */
#ifdef LIVELOGFILE
E boolean NDECL(livelog_start);
E void NDECL(livelog_achieve_update);
E void FDECL(livelog_wish, (char*));
E void NDECL(livelog_avert_death);
#ifdef LIVELOG_SHOUT
E int NDECL(doshout);
#endif
#endif

#endif /* !MAKEDEFS_C && !LEV_LEX_C */

#undef E

#endif /* EXTERN_H */
