/*	SCCS Id: @(#)extern.h	3.4	2003/03/10	*/
/* Copyright (c) Steve Creps, 1988.				  */
/* NetHack may be freely redistributed.  See license for details. */


#include "config.h"
#ifndef EXTERN_H
#define EXTERN_H

#define E extern

/* ### alloc.c ### */

#if 0
E genericptr_t alloc(size_t);
#endif
E char *fmt_ptr(const genericptr,char *);

/* This next pre-processor directive covers almost the entire file,
 * interrupted only occasionally to pick up specific functions as needed. */
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)

/* ### allmain.c ### */

E void NDECL(moveloop);
E void NDECL(stop_occupation);
E void NDECL(display_gamewindows);
E void NDECL(newgame);
E void welcome(BOOLEAN_P);
#if defined(RECORD_REALTIME) || defined(REALTIME_ON_BOTL)
E time_t NDECL(get_realtime);
#endif
E boolean NDECL(timebasedlowerchance);

/* ### apply.c ### */

E int NDECL(doapply);
E int NDECL(dorub);
E int NDECL(dojump);
E int jump(int);
E int jump(int);
E int NDECL(number_leashed);
E void o_unleash(struct obj *);
E void m_unleash(struct monst *,BOOLEAN_P);
E void NDECL(unleash_all);
E boolean NDECL(next_to_u);
E struct obj *get_mleash(struct monst *);
E void check_leash(XCHAR_P,XCHAR_P);
E void use_floppies(struct obj *);
E boolean um_dist(XCHAR_P,XCHAR_P,XCHAR_P);
E boolean snuff_candle(struct obj *);
E boolean snuff_lit(struct obj *);
E boolean catch_lit(struct obj *);
E void use_unicorn_horn(struct obj *);
E boolean tinnable(struct obj *);
E void NDECL(reset_trapset);
E void fig_transform(genericptr_t, long);
E int unfixable_trouble_count(BOOLEAN_P);
E int wand_explode(struct obj *,BOOLEAN_P);

/* ### artifact.c ### */

E void NDECL(init_artifacts);
E void NDECL(init_artifacts1);
E void save_artifacts(int);
E void restore_artifacts(int);
E const char *artiname(int);
E struct obj *mk_artifact(struct obj *,ALIGNTYP_P);
E void NDECL(bad_artifact);
E const char *artifact_name(const char *,int *);
E boolean exist_artifact(int,const char *);
E void artifact_exists(struct obj *,const char *,BOOLEAN_P);
E int NDECL(nartifact_exist);
E boolean spec_ability(struct obj *,unsigned long);
E boolean arti_is_evil(struct obj *);
E boolean confers_luck(struct obj *);
E boolean arti_reflects(struct obj *);
E boolean restrict_name(struct obj *,const char *);
E boolean defends(int,struct obj *);
E boolean protects(int,struct obj *);
E void set_artifact_intrinsic(struct obj *,BOOLEAN_P,long);
E int touch_artifact(struct obj *,struct monst *);
E int spec_abon(struct obj *,struct monst *);
E int spec_dbon(struct obj *,struct monst *,int);
E void discover_artifact(int);
E boolean undiscovered_artifact(int);
E int disp_artifact_discoveries(winid);
E void NDECL(dump_artifact_discoveries);
E boolean artifact_hit(struct monst *,struct monst *, struct obj *,int *,int);
E int NDECL(doinvoke);
E void arti_speak(struct obj *);
E boolean artifact_light(struct obj *);
E int artifact_wet(struct obj *, BOOLEAN_P);
E void arti_speak(struct obj *);
E boolean artifact_light(struct obj *);
E long spec_m2(struct obj *);
E boolean artifact_has_invprop(struct obj *,int);
E long arti_cost(struct obj *);
E void arti_poly_contents(struct obj *);
E int NDECL(find_wildtalentartifact);
E int NDECL(find_prostituteartifact);
E int NDECL(find_kurwaartifact);

/* ### attrib.c ### */

E boolean adjattrib(int,int,int);
E void change_luck(SCHAR_P);
E int stone_luck(BOOLEAN_P);
E void NDECL(set_moreluck);
E void gainstr(struct obj *,int);
E void losestr(int);
E void NDECL(restore_attrib);
E void exercise(int,BOOLEAN_P);
E void NDECL(exerchk);
E void NDECL(reset_attribute_clock);
E void init_attr(int);
E void NDECL(redist_attr);
E void adjabil(int,int);
E int NDECL(newhp);
E schar acurr(int);
E schar NDECL(acurrstr);
E void adjalign(int);
/* KMH, balance patch -- new function */
E void NDECL(recalc_health);
E int NDECL(recalc_mana);

/* ### ball.c ### */

E void NDECL(ballfall);
E void NDECL(placebc);
E void NDECL(unplacebc);
E void set_bc(int);
E void move_bc(int,int,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P);
E boolean drag_ball(XCHAR_P,XCHAR_P, int *,xchar *,xchar *,xchar *,xchar *, boolean *,BOOLEAN_P);
E void drop_ball(XCHAR_P,XCHAR_P);
E void NDECL(drag_down);

/* ### bones.c ### */

E boolean NDECL(can_make_bones);
E void savebones(struct obj *);
E int NDECL(getbones);

/* ### borg.c ### */

/* E char borg_on;
E char borg_line[80];
E char borg_input(void); */

/* ### botl.c ### */

E int xlev_to_rank(int);
E int title_to_mon(const char *,int *,int *);
E void NDECL(max_rank_sz);
E long NDECL(botl_score);
E int describe_level(char *, int);
E const char *NDECL(rank);
E const char *rank_of(int,SHORT_P,BOOLEAN_P);
E void bot_set_handler(void (*)());
E void NDECL(bot_reconfig);
E void NDECL(bot);
E void NDECL(botreal);
#ifdef DUMP_LOG
E void bot1str(char *);
E void bot2str(char *);
#endif

#if 0
E const char * shorten_bot1(const char *, int);
#endif
#ifdef TTY_GRAPHICS
E const char * shorten_bot2(const char *, unsigned int);
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
E char *montraitname(int);
# endif /* WIZARD */
#endif /* USE_TRAMPOLI */
E void NDECL(reset_occupations);
E void set_occupation(int (*)(void),const char *,int);
E char NDECL(pgetchar);
E void pushch(CHAR_P);
E void savech(CHAR_P);
E void rhack(char *);
E void NDECL(dokeylist);
E int NDECL(doextlist);
E int NDECL(extcmd_via_menu);
E void enlightenment(int, int);
E void show_conduct(int);
#ifdef DUMP_LOG
E void dump_enlightenment(int);
E void dump_conduct(int);
#endif
E int xytod(SCHAR_P,SCHAR_P);
E void dtoxy(coord *,int);
E int movecmd(CHAR_P);
E int getdir(const char *);
E void NDECL(confdir);
E int isok(int,int);
E int get_adjacent_loc(const char *, const char *, XCHAR_P, XCHAR_P, coord *);
E const char *click_to_cmd(int,int,int);
E char NDECL(readchar);
#ifdef WIZARD
E void NDECL(sanity_check);
#endif
E void NDECL(commands_init);
E char* stripspace(char*);
E void parsebindings(char*);
E void parseautocomplete(char*,boolean);
E char txt2key(char*);
E char* key2txt(char, char*);
E char* str2txt(char*, char*);
E char yn_function(const char *, const char *, CHAR_P);

/* ### dbridge.c ### */

E boolean is_pool(int,int);
E boolean is_lava(int,int);
E boolean is_ice(int,int);
E int is_drawbridge_wall(int,int);
E boolean is_db_wall(int,int);
E boolean find_drawbridge(int *,int*);
E boolean create_drawbridge(int,int,int,BOOLEAN_P);
E void open_drawbridge(int,int);
E void close_drawbridge(int,int);
E void destroy_drawbridge(int,int);

/* ### decl.c ### */

E void NDECL(decl_init);

/* ### detect.c ### */

E struct obj *o_in(struct obj*,CHAR_P);
E struct obj *o_material(struct obj*,unsigned);
E int gold_detect(struct obj *);
E int food_detect(struct obj *);
E int object_detect(struct obj *,int);
E int artifact_detect(struct obj *);
E int monster_detect(struct obj *,int);
E int trap_detect(struct obj *);
E int trap_detectX(struct obj *);
E const char *level_distance(d_level *);
E void use_crystal_ball(struct obj *);
E void NDECL(water_detect);
E void NDECL(water_detectX);
E void NDECL(do_mapping);
E void NDECL(do_mappingX);
E void NDECL(do_mappingY);
E void NDECL(do_vicinity_map);
E void NDECL(do_vicinity_mapX);
E void cvt_sdoor_to_door(struct rm *);
#ifdef USE_TRAMPOLI
E void findone(int,int,genericptr_t);
E void openone(int,int,genericptr_t);
#endif
E int NDECL(findit);
E int NDECL(finditX);
E int NDECL(openit);
E void find_trap(struct trap *);
E int dosearch0(int);
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
E boolean dig_check(struct monst *, BOOLEAN_P, int, int);
E void digactualhole(int,int,struct monst *,int);
E boolean dighole(BOOLEAN_P);
E int use_pick_axe(struct obj *);
E int use_pick_axe2(struct obj *);
E boolean mdig_tunnel(struct monst *);
E void watch_dig(struct monst *,XCHAR_P,XCHAR_P,BOOLEAN_P);
E void zap_dig(BOOLEAN_P);
E struct obj *bury_an_obj(struct obj *);
E void bury_objs(int,int);
E void unearth_objs(int,int);
E void rot_organic(genericptr_t, long);
E void rot_corpse(genericptr_t, long);
#if 0
E void bury_monst(struct monst *);
E void NDECL(bury_you);
E void NDECL(unearth_you);
E void NDECL(escape_tomb);
E void bury_obj(struct obj *);
#endif

/* ### display.c ### */

E struct obj * vobj_at(XCHAR_P,XCHAR_P);
E void magic_map_background(XCHAR_P,XCHAR_P,int);
E void map_background(XCHAR_P,XCHAR_P,int);
E void map_trap(struct trap *,int);
E void map_object(struct obj *,int);
E void map_invisible(XCHAR_P,XCHAR_P);
E void map_invisibleX(XCHAR_P,XCHAR_P);
E void unmap_object(int,int);
E void map_location(int,int,int);
E int memory_glyph(int, int);
E void clear_memory_glyph(int, int, int);
E void feel_location(XCHAR_P,XCHAR_P);
E void newsym(int,int);
E void newsymX(int,int);
E void shieldeff(XCHAR_P,XCHAR_P);
E void tmp_at(int,int);
#ifdef DISPLAY_LAYERS
E int glyph_is_floating(int);
#endif
E void swallowed(int);
E void under_ground(int);
E void under_water(int);
E void NDECL(see_monsters);
E void NDECL(see_monstersX);
E void NDECL(set_mimic_blocking);
E void NDECL(see_objects);
E void NDECL(see_traps);
E void NDECL(curs_on_u);
E int NDECL(doredraw);
E void NDECL(docrt);
E void show_glyph(int,int,int);
E void show_glyphX(int,int,int);
E void NDECL(clear_glyph_buffer);
E void row_refresh(int,int,int);
E void NDECL(cls);
E void flush_screen(int);
#ifdef DUMP_LOG
E void NDECL(dump_screen);
#endif
E int back_to_glyph(XCHAR_P,XCHAR_P);
E int zapdir_to_glyph(int,int,int);
E int glyph_at(XCHAR_P,XCHAR_P);
E void NDECL(set_wall_state);

/* ### do.c ### */

#ifdef USE_TRAMPOLI
E int drop(struct obj *);
E int NDECL(wipeoff);
#endif
E int NDECL(dodrop);
E boolean boulder_hits_pool(struct obj *,int,int,BOOLEAN_P);
E boolean flooreffects(struct obj *,int,int,const char *);
E void doaltarobj(struct obj *);
E boolean canletgo(struct obj *,const char *);
E void dropx(struct obj *);
E void dropy(struct obj *);
E void obj_no_longer_held(struct obj *);
E int NDECL(doddrop);
E int NDECL(dodown);
E int NDECL(doup);
#ifdef INSURANCE
E void NDECL(save_currentstate);
#endif
E void goto_level(d_level *,BOOLEAN_P,BOOLEAN_P,BOOLEAN_P);
E void schedule_goto(d_level *,BOOLEAN_P,BOOLEAN_P,int, const char *,const char *);
E void NDECL(deferred_goto);
E boolean revive_corpse(struct obj *, BOOLEAN_P);
E void revive_mon(genericptr_t, long);
E void moldy_corpse(genericptr_t, long);
E int NDECL(donull);
E int NDECL(dowipe);
E void set_wounded_legs(long,int);
E void NDECL(heal_legs);

/* ### do_name.c ### */

E int getpos(coord *,BOOLEAN_P,const char *);
E struct monst *christen_monst(struct monst *,const char *);
E int NDECL(do_mname);
E struct obj *oname(struct obj *,const char *);
E int NDECL(ddocall);
E void docall(struct obj *);
E const char *NDECL(rndghostname);
E const char *NDECL(rndplrmonname);
E const char *NDECL(rndplrmonnamefemale);
E char *x_monnam(struct monst *,int,const char *,int,BOOLEAN_P);
E char *l_monnam(struct monst *);
E char *mon_nam(struct monst *);
E char *noit_mon_nam(struct monst *);
E char *Monnam(struct monst *);
E char *noit_Monnam(struct monst *);
E char *m_monnam(struct monst *);
E char *y_monnam(struct monst *);
E char *Adjmonnam(struct monst *,const char *);
E char *Amonnam(struct monst *);
E char *a_monnam(struct monst *);
E char *distant_monnam(struct monst *,int,char *);
E const char *NDECL(rndmonnam);
E const char *hcolor(const char *);
E const char *NDECL(rndcolor);
#ifdef REINCARNATION
E const char *NDECL(roguename);
#endif
E struct obj *realloc_obj(struct obj *, int, genericptr_t, int, const char *);
E char *coyotename(struct monst *,char *);


/* ### do_wear.c ### */

E int NDECL(Armor_on);
E int NDECL(Boots_on);
E int NDECL(Cloak_on);
E int NDECL(Helmet_on);
E int NDECL(Gloves_on);
E int NDECL(Shield_on);
E int NDECL(Shirt_on);
E void NDECL(Amulet_on);
#ifdef USE_TRAMPOLI
E int select_off(struct obj *);
E int NDECL(take_off);
#endif
E void off_msg(struct obj *);
E void NDECL(set_wear);
E boolean donning(struct obj *);
E void NDECL(cancel_don);
E int NDECL(Armor_off);
E int NDECL(Armor_gone);
E int NDECL(Helmet_off);
E int NDECL(Gloves_off);
E int NDECL(Boots_off);
E int NDECL(Cloak_off);
E int NDECL(Shield_off);
E int NDECL(Shirt_off);
E void NDECL(Amulet_off);
E void Ring_on(struct obj *);
E void Ring_off(struct obj *);
E void Ring_gone(struct obj *);
E void Blindf_on(struct obj *);
E void Blindf_off(struct obj *);
E int NDECL(dotakeoff);
E int NDECL(doremring);
E int cursed(struct obj *);
E int armoroff(struct obj *);
E int canwearobj(struct obj *, long *, BOOLEAN_P);
E int NDECL(dowear);
E int NDECL(doputon);
E void NDECL(find_ac);
E void NDECL(glibr);
E struct obj *some_armor(struct monst *);
E void erode_armor(struct monst *,BOOLEAN_P);
E struct obj *stuck_ring(struct obj *,int);
E struct obj *NDECL(unchanger);
E void NDECL(reset_remarm);
E int NDECL(doddoremarm);
E int destroy_arm(struct obj *);
E int dowear2(const char *, const char *);

/* ### dog.c ### */

E void initedog(struct monst *);
E struct monst *make_familiar(struct obj *,XCHAR_P,XCHAR_P,BOOLEAN_P);
E struct monst *make_helper(int,XCHAR_P,XCHAR_P);
E struct monst *NDECL(makedog);
E void NDECL(update_mlstmv);
E void NDECL(losedogs);
E void mon_arrive(struct monst *,BOOLEAN_P);
E void mon_catchup_elapsed_time(struct monst *,long);
E void keepdogs(BOOLEAN_P);
E void migrate_to_level(struct monst *,XCHAR_P,XCHAR_P,coord *);
E int dogfood(struct monst *,struct obj *);
E struct monst *tamedog(struct monst *,struct obj *, BOOLEAN_P);
E int make_pet_minion(int,ALIGNTYP_P);
E void abuse_dog(struct monst *);
E void wary_dog(struct monst *, BOOLEAN_P);

/* ### dogmove.c ### */

E int dog_nutrition(struct monst *,struct obj *);
E int dog_eat(struct monst *,struct obj *,int,int,BOOLEAN_P);
E int dog_move(struct monst *,int);
E boolean betrayed(struct monst *);
#ifdef USE_TRAMPOLI
E void wantdoor(int,int,genericptr_t);
#endif

/* ### dokick.c ### */

E boolean ghitm(struct monst *,struct obj *);
E void container_impact_dmg(struct obj *);
E int NDECL(dokick);
E boolean ship_object(struct obj *,XCHAR_P,XCHAR_P,BOOLEAN_P);
E void NDECL(obj_delivery);
E schar down_gate(XCHAR_P,XCHAR_P);
E void impact_drop(struct obj *,XCHAR_P,XCHAR_P,XCHAR_P);

/* ### dothrow.c ### */

E struct obj *splitoneoff(struct obj **);
E int NDECL(dothrow);
E int NDECL(dofire);
E void hitfloor(struct obj *);
E void hurtle(int,int,int,BOOLEAN_P);
E void mhurtle(struct monst *,int,int,int);
E void throwit(struct obj *,long,BOOLEAN_P,int);
E int omon_adj(struct monst *,struct obj *,BOOLEAN_P);
E int thitmonst(struct monst *,struct obj *, int);
E int hero_breaks(struct obj *,XCHAR_P,XCHAR_P,BOOLEAN_P);
E int breaks(struct obj *,XCHAR_P,XCHAR_P);
E boolean breaktest(struct obj *);
E boolean walk_path(coord *, coord *, boolean (*)(genericptr_t,int,int), genericptr_t);
E boolean hurtle_step(genericptr_t, int, int);

/* ### drawing.c ### */
#endif /* !MAKEDEFS_C && !LEV_LEX_C */
E int def_char_to_objclass(CHAR_P);
E int def_char_to_monclass(CHAR_P);
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
E void assign_graphics(uchar *,int,int,int);
E void switch_graphics(int);
#ifdef REINCARNATION
E void assign_rogue_graphics(BOOLEAN_P);
#endif

/* ### dungeon.c ### */

E void save_dungeon(int,BOOLEAN_P,BOOLEAN_P);
E void restore_dungeon(int);
E void insert_branch(branch *,BOOLEAN_P);
E void NDECL(init_dungeons);
E s_level *find_level(const char *);
E s_level *Is_special(d_level *);
E branch *Is_branchlev(d_level *);
E xchar ledger_no(d_level *);
E xchar NDECL(maxledgerno);
E schar depth(d_level *);
E xchar dunlev(d_level *);
E xchar dunlevs_in_dungeon(d_level *);
E xchar real_dunlevs_in_dungeon(d_level *);
E xchar ledger_to_dnum(XCHAR_P);
E xchar ledger_to_dlev(XCHAR_P);
E xchar deepest_lev_reached(BOOLEAN_P);
E boolean on_level(d_level *,d_level *);
E void next_level(BOOLEAN_P);
E void prev_level(BOOLEAN_P);
E void u_on_newpos(int,int);
E void NDECL(u_on_sstairs);
E void NDECL(u_on_upstairs);
E void NDECL(u_on_dnstairs);
E boolean On_stairs(XCHAR_P,XCHAR_P);
E void get_level(d_level *,int);
E boolean Is_botlevel(d_level *);
E boolean Can_fall_thru(d_level *);
E boolean Can_dig_down(d_level *);
E boolean Can_rise_up(int,int,d_level *);
E boolean In_quest(d_level *);
E boolean In_mines(d_level *);
E boolean In_sheol(d_level *);
E boolean In_dod(d_level *);
E boolean In_gehennom(d_level *);
E boolean In_towndungeon(d_level *);
E boolean In_spiders(d_level *);
E boolean In_grund(d_level *);
E boolean In_wyrm(d_level *);
E boolean In_tomb(d_level *);
E boolean In_slsea(d_level *);
E boolean In_gcavern(d_level *);
E boolean In_frnkn(d_level *);
E boolean In_mtemple(d_level *);
E branch *dungeon_branch(const char *);
E boolean at_dgn_entrance(const char *);
E boolean In_hell(d_level *);
E boolean In_V_tower(d_level *);
E boolean On_W_tower_level(d_level *);
E boolean In_W_tower(int,int,d_level *);
E void find_hell(d_level *);
E void goto_hell(BOOLEAN_P,BOOLEAN_P);
E void assign_level(d_level *,d_level *);
E void assign_rnd_level(d_level *,d_level *,int);
E int induced_align(int);
E boolean Invocation_lev(d_level *);
E xchar NDECL(level_difficulty);
E xchar NDECL(monster_difficulty);
E schar lev_by_name(const char *);
#ifdef WIZARD
E schar print_dungeon(BOOLEAN_P,schar *,xchar *);
#endif
E int NDECL(donamelevel);
E int NDECL(dooverview);
E void forget_mapseen(int);
E void init_mapseen(d_level *);

/* ### eat.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(eatmdone);
E int NDECL(eatfood);
E int NDECL(opentin);
E int NDECL(unfaint);
#endif
E boolean is_edible(struct obj *);
E void NDECL(init_uhunger);
E int NDECL(Hear_again);
E void NDECL(reset_eat);
E int NDECL(doeat);
E void NDECL(gethungry);
E void morehungry(int);
E void lesshungry(int);
E boolean NDECL(is_fainted);
E void NDECL(reset_faint);
E void NDECL(violated_vegetarian);
#if 0
E void NDECL(sync_hunger);
#endif
E void newuhs(BOOLEAN_P);
E boolean NDECL(can_reach_floorobj);
E void NDECL(vomit);
E int eaten_stat(int,struct obj *);
E void food_disappears(struct obj *);
E void food_substitution(struct obj *,struct obj *);
E boolean bite_monster(struct monst *mon);
E void NDECL(fix_petrification);
E void consume_oeaten(struct obj *,int);
E boolean maybe_finished_meal(BOOLEAN_P);

/* ### end.c ### */

E void done1(int);
E int NDECL(done2);
#ifdef USE_TRAMPOLI
E void done_intr(int);
#endif
E void done_in_by(struct monst *);
#endif /* !MAKEDEFS_C && !LEV_LEX_C */
E void panic(const char *,...) PRINTF_F(1,2);
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
E void done(int);
E void container_contents(struct obj *,BOOLEAN_P,BOOLEAN_P);
#ifdef DUMP_LOG
E void dump(const char * const pre, const char * const str);
E void do_containerconts(struct obj *,BOOLEAN_P,BOOLEAN_P,BOOLEAN_P);
#endif
E void terminate(int);
E int NDECL(dolistvanq);
E int NDECL(num_genocides);
/* KMH, ethics */
E int NDECL(doethics);


/* ### engrave.c ### */

E char *random_engraving(char *);
E void wipeout_text(char *,int,unsigned);
E boolean NDECL(can_reach_floor);
E const char *surface(int,int);
E const char *ceiling(int,int);
E struct engr *engr_at(XCHAR_P,XCHAR_P);
E int sengr_at(const char *,XCHAR_P,XCHAR_P);
E void u_wipe_engr(int);
E void wipe_engr_at(XCHAR_P,XCHAR_P,XCHAR_P);
E boolean sense_engr_at(int,int,BOOLEAN_P);
E void make_engr_at(int,int,const char *,long,XCHAR_P);
E void del_engr_at(int,int);
E int NDECL(freehand);
E int NDECL(doengrave);
E void save_engravings(int,int);
E void rest_engravings(int);
E void del_engr(struct engr *);
E void rloc_engr(struct engr *);
E void make_grave(int,int,const char *);
E const char *NDECL(random_mesgX);
E const char *NDECL(random_epitaph);

/* ### exper.c ### */

E long newuexp(int);
E int experience(struct monst *,int);
E void more_experienced(int,int);
E void losexp(const char *, BOOLEAN_P, BOOLEAN_P);
E void NDECL(newexplevel);
E void pluslvl(BOOLEAN_P);
E long rndexp(BOOLEAN_P);

/* ### explode.c ### */

E void explode(int,int,int,int,CHAR_P,int);
E long scatter(int, int, int, unsigned int, struct obj *);
E void splatter_burning_oil(int, int);
E void grenade_explode(struct obj *, int, int, BOOLEAN_P, int);
E void arm_bomb(struct obj *, BOOLEAN_P);

/* ### extralev.c ### */

#ifdef REINCARNATION
E void NDECL(makeroguerooms);
E void corr(int,int);
E void NDECL(makerogueghost);
#endif

/* ### files.c ### */

E char *fname_encode(const char *, CHAR_P, char *, char *, int);
E char *fname_decode(CHAR_P, char *, char *, int);
E const char *fqname(const char *, int, int);
#ifndef FILE_AREAS
E FILE *fopen_datafile(const char *,const char *,int);
#endif
E boolean uptodate(int,const char *);
E void store_version(int);
#ifdef MFLOPPY
E void NDECL(set_lock_and_bones);
#endif
E void set_levelfile_name(char *,int);
E int create_levelfile(int,char *);
E int open_levelfile(int,char *);
E void delete_levelfile(int);
E void NDECL(clearlocks);
E int create_bonesfile(d_level*,char **, char *);
#ifdef MFLOPPY
E void NDECL(cancel_bonesfile);
#endif
E void commit_bonesfile(d_level *);
E int open_bonesfile(d_level*,char **);
E int delete_bonesfile(d_level*);
E void NDECL(compress_bonesfile);
E void NDECL(set_savefile_name);
#ifdef INSURANCE
E void save_savefile_name(int);
#endif
#if defined(WIZARD) && !defined(MICRO)
E void NDECL(set_error_savefile);
#endif
E int NDECL(create_savefile);
E int NDECL(open_savefile);
E int NDECL(delete_savefile);
E int NDECL(restore_saved_game);
E void compress_area(const char *, const char *);
E void uncompress_area(const char *, const char *);
#ifndef FILE_AREAS
E boolean lock_file(const char *,int,int);
E void unlock_file(const char *);
#endif
#ifdef USER_SOUNDS
E boolean can_read_file(const char *);
#endif
E void read_config_file(const char *);
E void check_recordfile(const char *);
#if defined(WIZARD)
E void NDECL(read_wizkit);
#endif
E void paniclog(const char *, const char *);
E int validate_prefix_locations(char *);
E char** NDECL(get_saved_games);
E void free_saved_games(char**);
#ifdef SELF_RECOVER
E boolean NDECL(recover_savefile);
#endif
#ifdef HOLD_LOCKFILE_OPEN
E void NDECL(really_close);
#endif

/* ### fountain.c ### */

E void floating_above(const char *);
E void dogushforth(int);
# ifdef USE_TRAMPOLI
E void gush(int,int,genericptr_t);
# endif
E void dryup(XCHAR_P,XCHAR_P, BOOLEAN_P);
E void NDECL(drinkfountain);
E void dipfountain(struct obj *);
E void whetstone_fountain_effects(struct obj *);
E void diptoilet(struct obj *);
E void breaksink(int,int);
E void breaktoilet(int,int);
E void NDECL(drinksink);
E void NDECL(drinktoilet);
E void whetstone_sink_effects(struct obj *);
E void whetstone_toilet_effects(struct obj *);

/* ### gypsy.c ### */

E void gypsy_init(struct monst *);
E void gypsy_chat(struct monst *);


/* ### hack.c ### */

#ifdef OVL1
E void NDECL(maybe_wail);
#endif /*OVL1*/

#ifdef DUNGEON_GROWTH
E void catchup_dgn_growths(int);
E void dgn_growths(BOOLEAN_P,BOOLEAN_P);
#endif
E boolean revive_nasty(int,int,const char*);
E void movobj(struct obj *,XCHAR_P,XCHAR_P);
E boolean may_dig(XCHAR_P,XCHAR_P);
E boolean may_passwall(XCHAR_P,XCHAR_P);
E boolean bad_rock(struct monst *,XCHAR_P,XCHAR_P);
E boolean invocation_pos(XCHAR_P,XCHAR_P);
E boolean test_move(int, int, int, int, int);
E void NDECL(domove);
E void NDECL(invocation_message);
E void wounds_message(struct monst *); 
E char *mon_wounds(struct monst *); 
E void spoteffects(BOOLEAN_P);
E char *in_rooms(XCHAR_P,XCHAR_P,int);
E char *in_roomscolouur(XCHAR_P,XCHAR_P,int);
E boolean in_town(int,int);
E void check_special_room(BOOLEAN_P);
E int NDECL(dopickup);
E void NDECL(lookaround);
E int NDECL(monster_nearby);
E void nomul(int, const char *);
E void forcenomul(int, const char *);
E void unmul(const char *);
#ifdef SHOW_DMG
E void showdmg(int);
#endif
E void losehp(int,const char *, int);
E int NDECL(weight_cap);
E int NDECL(inv_weight);
E int NDECL(near_capacity);
E int calc_capacity(int);
E int NDECL(max_capacity);
E boolean check_capacity(const char *);
E int NDECL(inv_cnt);
#ifdef GOLDOBJ
E long money_cnt(struct obj *);
#endif

/* ### hacklib.c ### */

E boolean digit(CHAR_P);
E boolean letter(CHAR_P);
E char highc(CHAR_P);
E char lowc(CHAR_P);
E char *lcase(char *);
E char *upstart(char *);
E char *mungspaces(char *);
E char *eos(char *);
E char *strkitten(char *,CHAR_P);
E char *s_suffix(const char *);
E char *xcrypt(const char *,char *);
E boolean onlyspace(const char *);
E char *tabexpand(char *);
E char *visctrl(CHAR_P);
E const char *ordin(int);
E char *sitoa(int);
E int sgn(int);
E int rounddiv(long,int);
E int dist2(int,int,int,int);
E int distmin(int,int,int,int);
E boolean online2(int,int,int,int);
E boolean pmatch(const char *,const char *);
#ifndef STRNCMPI
E int strncmpi(const char *,const char *,int);
#endif
#ifndef STRSTRI
E char *strstri(const char *,const char *);
#endif
E boolean fuzzymatch(const char *,const char *,const char *,BOOLEAN_P);
E void NDECL(setrandom);
E int NDECL(getyear);
E int NDECL(getmonth);	/* KMH -- Used by gypsies */
#if 0
E char *yymmdd(time_t);
#endif
E long yyyymmdd(time_t);
E struct tm *getlt();
E int NDECL(phase_of_the_moon);
E boolean NDECL(friday_13th);
E boolean NDECL(groundhog_day);	/* KMH -- February 2 */
E int NDECL(night);
E int NDECL(midnight);

/* ### invent.c ### */

E void assigninvlet(struct obj *);
E struct obj *merge_choice(struct obj *,struct obj *);
E int merged(struct obj **,struct obj **);
#ifdef USE_TRAMPOLI
E int ckunpaid(struct obj *);
#endif
E void addinv_core1(struct obj *);
E void addinv_core2(struct obj *);
E struct obj *addinv(struct obj *);
E struct obj *hold_another_object(struct obj *,const char *,const char *,const char *);
E struct obj *hold_another_objectX(struct obj *,const char *,const char *,const char *);
E void useupall(struct obj *);
E void useup(struct obj *);
E void consume_obj_charge(struct obj *,BOOLEAN_P);
E void freeinv_core(struct obj *);
E void freeinv(struct obj *);
E void delallobj(int,int);
E void delobj(struct obj *);
E struct obj *sobj_at(int,int,int);
E struct obj *carrying(int);
E boolean NDECL(have_lizard);
E boolean NDECL(have_loadstone);
E boolean NDECL(have_pokeloadstone);
E boolean NDECL(have_invisoloadstone);
E boolean NDECL(have_sleepstone);
E boolean NDECL(have_magicresstone);
E boolean NDECL(have_cursedmagicresstone);
E boolean NDECL(have_loadboulder);
E boolean NDECL(have_starlightstone);

E boolean stack_too_big(struct obj *);

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

E boolean NDECL(have_nonsacredstone);
E boolean NDECL(have_starvationstone);
E boolean NDECL(have_droplessstone);
E boolean NDECL(have_loweffectstone);
E boolean NDECL(have_invisostone);
E boolean NDECL(have_ghostlystone);
E boolean NDECL(have_dehydratingstone);
E boolean NDECL(have_hatestone);
E boolean NDECL(have_directionswapstone);
E boolean NDECL(have_nonintrinsicstone);
E boolean NDECL(have_dropcursestone);
E boolean NDECL(have_nakedstone);
E boolean NDECL(have_antilevelstone);
E boolean NDECL(have_stealerstone);
E boolean NDECL(have_rebelstone);
E boolean NDECL(have_shitstone);
E boolean NDECL(have_misfirestone);
E boolean NDECL(have_wallstone);

E boolean NDECL(have_metabolicstone);
E boolean NDECL(have_noreturnstone);
E boolean NDECL(have_egostone);
E boolean NDECL(have_fastforwardstone);
E boolean NDECL(have_rottenstone);
E boolean NDECL(have_unskilledstone);
E boolean NDECL(have_lowstatstone);
E boolean NDECL(have_trainingstone);
E boolean NDECL(have_exercisestone);

E boolean NDECL(have_limitationstone);
E boolean NDECL(have_weaksightstone);
E boolean NDECL(have_messagestone);

E boolean NDECL(have_disconnectstone);
E boolean NDECL(have_interfacescrewstone);
E boolean NDECL(have_bossfightstone);
E boolean NDECL(have_entirelevelstone);
E boolean NDECL(have_bonestone);
E boolean NDECL(have_autocursestone);
E boolean NDECL(have_highlevelstone);
E boolean NDECL(have_spellforgettingstone);
E boolean NDECL(have_soundeffectstone);
E boolean NDECL(have_timerunstone);

E boolean NDECL(have_lootcutstone);
E boolean NDECL(have_monsterspeedstone);
E boolean NDECL(have_scalingstone);
E boolean NDECL(have_inimicalstone);
E boolean NDECL(have_whitespellstone);
E boolean NDECL(have_greyoutstone);
E boolean NDECL(have_quasarstone);
E boolean NDECL(have_mommystone);
E boolean NDECL(have_horrorstone);
E boolean NDECL(have_artificialstone);
E boolean NDECL(have_wereformstone);
E boolean NDECL(have_antiprayerstone);
E boolean NDECL(have_evilpatchstone);
E boolean NDECL(have_hardmodestone);
E boolean NDECL(have_secretattackstone);
E boolean NDECL(have_eaterstone);
E boolean NDECL(have_covetousstone);
E boolean NDECL(have_nonseeingstone);
E boolean NDECL(have_darkmodestone);
E boolean NDECL(have_unfindablestone);
E boolean NDECL(have_homicidestone);
E boolean NDECL(have_multitrappingstone);
E boolean NDECL(have_wakeupcallstone);
E boolean NDECL(have_grayoutstone);
E boolean NDECL(have_graycenterstone);
E boolean NDECL(have_checkerboardstone);
E boolean NDECL(have_clockwisestone);
E boolean NDECL(have_counterclockwisestone);
E boolean NDECL(have_lagstone);
E boolean NDECL(have_blesscursestone);
E boolean NDECL(have_delightstone);
E boolean NDECL(have_dischargestone);
E boolean NDECL(have_trashstone);
E boolean NDECL(have_filteringstone);
E boolean NDECL(have_deformattingstone);
E boolean NDECL(have_flickerstripstone);
E boolean NDECL(have_undressingstone);
E boolean NDECL(have_hyperbluestone);
E boolean NDECL(have_nolightstone);
E boolean NDECL(have_paranoiastone);
E boolean NDECL(have_fleecestone);
E boolean NDECL(have_interruptionstone);
E boolean NDECL(have_dustbinstone);
E boolean NDECL(have_batterystone);
E boolean NDECL(have_butterfingerstone);
E boolean NDECL(have_miscastingstone);
E boolean NDECL(have_messagesuppressionstone);
E boolean NDECL(have_stuckannouncementstone);
E boolean NDECL(have_stormstone);
E boolean NDECL(have_maximumdamagestone);
E boolean NDECL(have_latencystone);
E boolean NDECL(have_starlitskystone);
E boolean NDECL(have_trapknowledgestone);
E boolean NDECL(have_highscorestone);
E boolean NDECL(have_pinkspellstone);
E boolean NDECL(have_greenspellstone);
E boolean NDECL(have_evcstone);
E boolean NDECL(have_underlaidstone);
E boolean NDECL(have_damagemeterstone);
E boolean NDECL(have_weightstone);
E boolean NDECL(have_infofuckstone);
E boolean NDECL(have_blackspellstone);
E boolean NDECL(have_cyanspellstone);
E boolean NDECL(have_heapstone);
E boolean NDECL(have_bluespellstone);
E boolean NDECL(have_tronstone);
E boolean NDECL(have_redspellstone);
E boolean NDECL(have_tooheavystone);
E boolean NDECL(have_elongatedstone);
E boolean NDECL(have_wrapoverstone);
E boolean NDECL(have_destructionstone);
E boolean NDECL(have_meleeprefixstone);
E boolean NDECL(have_automorestone);
E boolean NDECL(have_unfairattackstone);

E boolean NDECL(have_primecurse);

E boolean NDECL(have_morgothiancurse);
E boolean NDECL(have_topiylinencurse);
E boolean NDECL(have_blackbreathcurse);

E boolean NDECL(have_mothrelay);

E struct obj *o_on(unsigned int,struct obj *);
E boolean obj_here(struct obj *,int,int);
E boolean NDECL(wearing_armor);
E boolean is_worn(struct obj *);
E struct obj *g_at(int,int);
E struct obj *mkgoldobj(long);
E struct obj *getobj(const char *,const char *);
E int ggetobj(const char *,int (*)(OBJ_P),int,BOOLEAN_P,unsigned *);
E void fully_identify_obj(struct obj *);
E void maybe_fully_identify_obj(struct obj *);
E int identify(struct obj *);
E int identifyless(struct obj *);
E void identify_pack(int, BOOLEAN_P);
E int askchain(struct obj **,const char *,int,int (*)(OBJ_P), int (*)(OBJ_P),int,const char *);
E void prinv(const char *,struct obj *,long);
E char *xprname(struct obj *,const char *,CHAR_P,BOOLEAN_P,long,long);
E int NDECL(ddoinv);
E char display_inventory(const char *,BOOLEAN_P);
#ifdef DUMP_LOG
E char dump_inventory(const char *,BOOLEAN_P);
#endif
E int display_binventory(int,int,BOOLEAN_P);
E struct obj *display_cinventory(struct obj *);
E struct obj *display_minventory(struct monst *,int,char *);
E int NDECL(dotypeinv);
E const char *dfeature_at(int,int,char *);
E int look_here(int,BOOLEAN_P);
E int NDECL(dolook);
E boolean will_feel_cockatrice(struct obj *,BOOLEAN_P);
E void feel_cockatrice(struct obj *,BOOLEAN_P);
E void stackobj(struct obj *);
E int NDECL(doprgold);
E int NDECL(doprwep);
E int NDECL(doprarm);
E int NDECL(doprring);
E int NDECL(dopramulet);
E int NDECL(doprtool);
E int NDECL(doprinuse);
E void useupf(struct obj *,long);
E char *let_to_name(CHAR_P,BOOLEAN_P,BOOLEAN_P);
E void NDECL(free_invbuf);
E void NDECL(reassign);
E int NDECL(doorganize);
E int NDECL(domarkforpet);
E int count_unpaid(struct obj *);
E int count_buc(struct obj *,int);
E void carry_obj_effects(struct monst *, struct obj *);
E const char *currency(long);
E void silly_thing(const char *,struct obj *);
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

E void new_light_source(XCHAR_P, XCHAR_P, int, int, genericptr_t);
E void del_light_source(int, genericptr_t);
E void do_light_sources(char **);
E struct monst *find_mid(unsigned, unsigned);
E void save_light_sources(int, int, int);
E void restore_light_sources(int);
E void relink_light_sources(BOOLEAN_P);
E void obj_move_light_source(struct obj *, struct obj *);
E boolean NDECL(any_light_source);
E void snuff_light_source(int, int);
E boolean obj_sheds_light(struct obj *);
E boolean obj_is_burning(struct obj *);
E boolean obj_permanent_light(struct obj *);
E void obj_split_light_source(struct obj *, struct obj *);
E void obj_merge_light_sources(struct obj *,struct obj *);
E int candle_light_range(struct obj *);
#ifdef WIZARD
E int NDECL(wiz_light_sources);
#endif

/* ### lock.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(forcelock);
E int NDECL(picklock);
#endif
E boolean picking_lock(int *,int *);
E boolean picking_at(int,int);
E void NDECL(reset_pick);
E int pick_lock(struct obj **);
E int NDECL(doforce);
E boolean boxlock(struct obj *,struct obj *);
E boolean doorlock(struct obj *,int,int);
E boolean doorlockX(int,int,BOOLEAN_P);
E int NDECL(doopen);
E int NDECL(doclose);
E int artifact_door(int,int);

#ifdef MAC
/* These declarations are here because the main code calls them. */

/* ### macfile.c ### */

E int maccreat(const char *,long);
E int macopen(const char *,int,long);
E int macclose(int);
E int macread(int,void *,unsigned);
E int macwrite(int,void *,unsigned);
E long macseek(int,long,short);
E int macunlink(const char *);

/* ### macsnd.c ### */

E void mac_speaker(struct obj *,char *);

/* ### macunix.c ### */

E void regularize(char *);
E void NDECL(getlock);

/* ### macwin.c ### */

E void lock_mouse_cursor(Boolean);
E int NDECL(SanePositions);

/* ### mttymain.c ### */

E void getreturn(const char *);
E void msmsg(const char *,...);
E void NDECL(gettty);
E void NDECL(setftty);
E void settty(const char *);
E int NDECL(tgetch);
E void cmov(int x, int y);
E void nocmov(int x, int y);

#endif /* MAC */

/* ### mail.c ### */

#ifdef MAIL
# ifdef UNIX
E void NDECL(getmailstatus);
# endif
E void NDECL(ckmailstatus);
E void readmail(struct obj *);
#endif /* MAIL */

/* ### makemon.c ### */

E boolean is_home_elemental(struct permonst *);
E boolean monster_with_trait(struct permonst *, int);
E struct monst *clone_mon(struct monst *,XCHAR_P,XCHAR_P);
E struct monst *makemon(struct permonst *,int,int,int);
E boolean create_critters(int,struct permonst *);
E struct permonst *NDECL(rndmonst);
E void reset_rndmonst(int);
E struct permonst *mkclass(CHAR_P,int);
E int pm_mkclass(CHAR_P,int);
E int adj_lev(struct permonst *);
E struct permonst *grow_up(struct monst *,struct monst *);
E int mongets(struct monst *,int);
E int golemhp(int);
E boolean peace_minded(struct permonst *);
E void set_malign(struct monst *);
E void set_mimic_sym(struct monst *);
E int mbirth_limit(int);
E void mimic_hit_msg(struct monst *, SHORT_P);
#ifdef GOLDOBJ
E void mkmonmoney(struct monst *, long);
#endif
E void bagotricks(struct obj *);
E boolean propagate(int, BOOLEAN_P,BOOLEAN_P);

/* ### mapglyph.c ### */

E void mapglyph(int, int *, int *, unsigned *, int, int);

/* ### mcastu.c ### */

E int castmu(struct monst *,struct attack *,BOOLEAN_P,BOOLEAN_P);
E int buzzmu(struct monst *,struct attack *);

/* ### mhitm.c ### */

E int fightm(struct monst *);
E int mattackm(struct monst *,struct monst *);
E int noattacks(struct permonst *);
E int sleep_monst(struct monst *,int,int);
E void slept_monst(struct monst *);
E long attk_protection(int);

/* ### mhitu.c ### */

E const char *mpoisons_subj(struct monst *,struct attack *);
E void NDECL(u_slow_down);
E struct monst *NDECL(cloneu);
E void expels(struct monst *,struct permonst *,BOOLEAN_P);
E struct attack *getmattk(struct permonst *,int,int *,struct attack *);
E int mattacku(struct monst *);
E int magic_negation(struct monst *);
E int gazemu(struct monst *,struct attack *);
E void mdamageu(struct monst *,int);
E int could_seduce(struct monst *,struct monst *,struct attack *);
E int could_seduceX(struct monst *,struct monst *,struct attack *);
E int doseduce(struct monst *);

/* ### minion.c ### */

E void msummon(struct monst *);
E void summon_minion(ALIGNTYP_P,BOOLEAN_P);
E int demon_talk(struct monst *);
E int lawful_minion(int);
E int neutral_minion(int);
E int chaotic_minion(int);
E long bribe(struct monst *);
E int dprince(ALIGNTYP_P);
E int dlord(ALIGNTYP_P);
E int NDECL(llord);
E int ndemon(ALIGNTYP_P);
E int NDECL(lminion);
E int NDECL(ntrminion);

/* ### mklev.c ### */

#ifdef USE_TRAMPOLI
E int do_comp(genericptr_t,genericptr_t);
#endif
E void NDECL(sort_rooms);
E void add_room(int,int,int,int,BOOLEAN_P,SCHAR_P,BOOLEAN_P,BOOLEAN_P);
E void add_subroom(struct mkroom *,int,int,int,int, BOOLEAN_P,SCHAR_P,BOOLEAN_P);
E void NDECL(makecorridors);
E int NDECL(randomwalltype);
E int add_door(int,int,struct mkroom *);
E void NDECL(mklev);
#ifdef SPECIALIZATION
E void topologize(struct mkroom *,BOOLEAN_P);
#else
E void topologize(struct mkroom *);
#endif
E void place_branch(branch *,XCHAR_P,XCHAR_P);
E boolean occupied(XCHAR_P,XCHAR_P);
E int okdoor(XCHAR_P,XCHAR_P);
E void dodoor(int,int,struct mkroom *);
E void mktrap(int,int,struct mkroom *,coord*);
E void mkstairs(XCHAR_P,XCHAR_P,CHAR_P,struct mkroom *);
E void NDECL(mkinvokearea);

/* ### mkmap.c ### */

void flood_fill_rm(int,int,int,BOOLEAN_P,BOOLEAN_P);
void remove_rooms(int,int,int,int);

/* ### mkmaze.c ### */

E void wallification(int,int,int,int, BOOLEAN_P);
E void walkfrom(int,int);
E void makemaz(const char *);
E void mazexy(coord *);
E void mazexy_all(coord *);
E void NDECL(bound_digging);
E void mkportal(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P);
E boolean bad_location(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P);
E boolean really_bad_location(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P);
E void place_lregion(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P, XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P, XCHAR_P,d_level *);
E void NDECL(movebubbles);
E void NDECL(water_friction);
E void save_waterlevel(int,int);
E void restore_waterlevel(int);
E const char *waterbody_name(XCHAR_P,XCHAR_P);

/* ### mkobj.c ### */

E struct obj *mkobj_at(CHAR_P,int,int,BOOLEAN_P);
E struct obj *mksobj_at(int,int,int,BOOLEAN_P,BOOLEAN_P);
E struct obj *mkobj(CHAR_P,BOOLEAN_P);
E int NDECL(rndmonnum);
E int NDECL(usefulitem);
E int NDECL(nastymusableitem);
E int NDECL(makegreatitem);
E struct obj *splitobj(struct obj *,long);
E void replace_object(struct obj *,struct obj *);
E void bill_dummy_object(struct obj *);
E struct obj *mksobj(int,BOOLEAN_P,BOOLEAN_P);
E int bcsign(struct obj *);
E int weight(struct obj *);
E struct obj *mkgold(long,int,int);
E struct obj *mkcorpstat(int,struct monst *,struct permonst *,int,int,BOOLEAN_P);
E struct obj *obj_attach_mid(struct obj *, unsigned);
E struct monst *get_mtraits(struct obj *, BOOLEAN_P);
E struct obj *mk_tt_object(int,int,int);
E struct obj *mk_named_object(int,struct permonst *,int,int,const char *);
E struct obj *rnd_treefruit_at(int, int);
E void start_corpse_timeout(struct obj *);
E void bless(struct obj *);
E void unbless(struct obj *);
E void curse(struct obj *);
E void curse_on_creation(struct obj *);
E void uncurse(struct obj *);
E void blessorcurse(struct obj *,int);
E void blessorcurse_on_creation(struct obj *,int);
E boolean is_flammable(struct obj *);
E boolean is_rottable(struct obj *);
E void place_object(struct obj *,int,int);
E void remove_object(struct obj *);
E void discard_minvent(struct monst *);
E void obj_extract_self(struct obj *);
E struct obj *container_extract_indestructable(struct obj *obj);
E void extract_nobj(struct obj *, struct obj **);
E void extract_nexthere(struct obj *, struct obj **);
E int add_to_minv(struct monst *, struct obj *);
E struct obj *add_to_container(struct obj *, struct obj *);
E void add_to_migration(struct obj *);
E void add_to_buried(struct obj *);
E void dealloc_obj(struct obj *);
E void obj_ice_effects(int, int, BOOLEAN_P);
E long peek_at_iced_corpse_age(struct obj *);
#if defined(OBJ_SANITY) || defined(WIZARD)
E void NDECL(obj_sanity_check);
#endif

/* ### mkroom.c ### */

E void mkroom(int);
E void fill_zoo(struct mkroom *);
E boolean nexttodoor(int,int);
E boolean has_dnstairs(struct mkroom *);
E boolean has_upstairs(struct mkroom *);
E int somex(struct mkroom *);
E int somey(struct mkroom *);
E boolean inside_room(struct mkroom *,XCHAR_P,XCHAR_P);
E boolean somexy(struct mkroom *,coord *);
E void mkundead(coord *,BOOLEAN_P,int);
/*E void mkundeadboo(coord *,BOOLEAN_P,int);*/
E void mkundeadX(coord *,BOOLEAN_P,int);
E struct permonst *NDECL(courtmon);
E struct permonst *NDECL(insidemon);
E struct permonst *NDECL(antholemon);
E struct permonst *NDECL(realzoomon);
E void save_rooms(int);
E void rest_rooms(int);
E struct mkroom *search_special(SCHAR_P);
E struct permonst * colormon(int);
E struct permonst * specialtensmon(int);

/* ### mon.c ### */

E int undead_to_corpse(int);
E int genus(int,int);
E int pm_to_cham(int);
E int minliquid(struct monst *);
E int NDECL(movemon);
E int meatmetal(struct monst *);
E int meatlithic(register struct monst *);
E int meatanything(struct monst *);
E void meatcorpse(struct monst *);
E int meatobj(struct monst *);
E void mpickgold(struct monst *);
E boolean mpickstuff(struct monst *,const char *);
E int curr_mon_load(struct monst *);
E int max_mon_load(struct monst *);
E boolean can_carry(struct monst *,struct obj *);
E int mfndpos(struct monst *,coord *,long *,long);
E boolean monnear(struct monst *,int,int);
E void NDECL(dmonsfree);
E int mcalcmove(struct monst*);
E void NDECL(mcalcdistress);
E void replmon(struct monst *,struct monst *);
E void relmon(struct monst *);
E struct obj *mlifesaver(struct monst *);
E boolean corpse_chance(struct monst *,struct monst *,BOOLEAN_P);
E void mondead(struct monst *);
E void mondied(struct monst *);
E void mongone(struct monst *);
E void monstone(struct monst *);
E void monkilled(struct monst *,const char *,int);
E void mon_xkilled(struct monst *,const char *,int);
E void unstuck(struct monst *);
E void killed(struct monst *);
E void xkilled(struct monst *,int);
E void mon_to_stone(struct monst*);
E void mnexto(struct monst *);
E boolean mnearto(struct monst *,XCHAR_P,XCHAR_P,BOOLEAN_P);
E void poisontell(int);
E void poisoned(const char *,int,const char *,int);
E void m_respond(struct monst *);
E void setmangry(struct monst *);
E void wakeup(struct monst *);
E void NDECL(wake_nearby);
E void wake_nearto(int,int,int);
E void seemimic(struct monst *);
E void NDECL(rescham);
E void NDECL(restartcham);
E void restore_cham(struct monst *);
E void mon_animal_list(BOOLEAN_P);
E int newcham(struct monst *,struct permonst *,BOOLEAN_P,BOOLEAN_P);
E int can_be_hatched(int);
E int egg_type_from_parent(int,BOOLEAN_P);
E boolean dead_species(int,BOOLEAN_P);
E void NDECL(kill_genocided_monsters);
E void golemeffects(struct monst *,int,int);
E boolean angry_guards(BOOLEAN_P);
E void NDECL(pacify_guards);
E boolean damage_mon(struct monst*,int,int); /* sporkhack */

/* ### mondata.c ### */

E void set_mon_data(struct monst *,struct permonst *,int);
E struct attack *attacktype_fordmg(struct permonst *,int,int);
E boolean attacktype(struct permonst *,int);
E struct attack *attdmgtype_fromattack(struct permonst *,int,int);
E boolean attackdamagetype(struct permonst *,int,int);
E boolean poly_when_stoned(struct permonst *);
E boolean resists_drli(struct monst *);
E boolean resists_magm(struct monst *);
E boolean resists_blnd(struct monst *);
E boolean can_blnd(struct monst *,struct monst *,UCHAR_P,struct obj *);
E boolean ranged_attk(struct permonst *);
E boolean passes_bars(struct permonst *);
E boolean can_track(struct permonst *);
E boolean breakarm(struct permonst *);
E boolean sliparm(struct permonst *);
E boolean sticks(struct permonst *);
E int num_horns(struct permonst *);
/* E boolean canseemon(struct monst *); */
E struct attack *dmgtype_fromattack(struct permonst *,int,int);
E boolean dmgtype(struct permonst *,int);
E int max_passive_dmg(struct monst *,struct monst *);
E int monsndx(struct permonst *);
E int name_to_mon(const char *);
E int gender(struct monst *);
E int pronoun_gender(struct monst *);
E boolean levl_follower(struct monst *);
E int little_to_big(int);
E int big_to_little(int);
E const char *locomotion(const struct permonst *,const char *);
E const char *stagger(const struct permonst *,const char *);
E const char *on_fire(struct permonst *,struct attack *);
E const struct permonst *raceptr(struct monst *);

/* ### monmove.c ### */

E boolean itsstuck(struct monst *);
E boolean mb_trapped(struct monst *);
E void mon_regen(struct monst *,BOOLEAN_P);
E int dochugw(struct monst *);
E boolean onscary(int,int,struct monst *);
E void monflee(struct monst *, int, BOOLEAN_P, BOOLEAN_P);
E int dochug(struct monst *);
E int m_move(struct monst *,int);
E boolean closed_door(int,int);
E boolean accessible(int,int);
E void set_apparxy(struct monst *);
E boolean can_ooze(struct monst *);

/* ### monst.c ### */

E void NDECL(monst_init);

/* ### monstr.c ### */

E void NDECL(monstr_init);

/* ### mplayer.c ### */

E struct monst *mk_mplayer(struct permonst *,XCHAR_P, XCHAR_P,BOOLEAN_P);
E void create_mplayers(int,BOOLEAN_P);
E void create_umplayers(int,BOOLEAN_P);
E void mplayer_talk(struct monst *);

#if defined(MICRO) || defined(WIN32)

/* ### msdos.c,os2.c,tos.c,winnt.c ### */

#  ifndef WIN32
E int NDECL(tgetch);
#  endif
#  ifndef TOS
E char NDECL(switchar);
#  endif
# ifndef __GO32__
E long freediskspace(char *);
#  ifdef MSDOS
E int findfirst_file(char *);
E int NDECL(findnext_file);
E long filesize_nh(char *);
#  else
E int findfirst(char *);
E int NDECL(findnext);
E long filesize(char *);
#  endif /* MSDOS */
E char *NDECL(foundfile_buffer);
# endif /* __GO32__ */
# ifndef __CYGWIN__
E void chdrive(char *);
# endif
# ifndef TOS
E void NDECL(disable_ctrlP);
E void NDECL(enable_ctrlP);
# endif
# if defined(MICRO) && !defined(WINNT)
E void NDECL(get_scr_size);
#  ifndef TOS
E void gotoxy(int,int);
#  endif
# endif
# ifdef TOS
E int _copyfile(char *,char *);
E int NDECL(kbhit);
E void NDECL(set_colors);
E void NDECL(restore_colors);
#  ifdef SUSPEND
E int NDECL(dosuspend);
#  endif
# endif /* TOS */
# ifdef WIN32
E char *get_username(int *);
E int set_binary_mode(int, int);
E void nt_regularize(char *);
E int NDECL((*nt_kbhit));
E void Delay(int);
# endif /* WIN32 */
#endif /* MICRO || WIN32 */

/* ### mthrowu.c ### */

E int thitu(int,int,struct obj *,const char *);
E int ohitmon(struct monst *,struct monst *,struct obj *,int,BOOLEAN_P);
E void thrwmu(struct monst *);
E int spitmu(struct monst *,struct attack *);
E int breamu(struct monst *,struct attack *);
E boolean breamspot(struct monst *, struct attack *, XCHAR_P, XCHAR_P);
E boolean linedup(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P);
E boolean linedupB(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P);
E boolean lined_up(struct monst *);
E boolean lined_upB(struct monst *);
E struct obj *m_carrying(struct monst *,int);
E void m_useup(struct monst *,struct obj *);
E void m_throw(struct monst *,int,int,int,int,int,struct obj *);
E boolean hits_bars(struct obj **,int,int,int,int);

/* ### muse.c ### */

E boolean find_defensive(struct monst *);
E int use_defensive(struct monst *);
E int rnd_defensive_item(struct monst *);
E int rnd_defensive_item_new(struct monst *);
E boolean find_offensive(struct monst *);
#ifdef USE_TRAMPOLI
E int mbhitm(struct monst *,struct obj *);
#endif
E int use_offensive(struct monst *);
E int rnd_offensive_item(struct monst *);
E int rnd_offensive_item_new(struct monst *);
E boolean find_misc(struct monst *);
E int use_misc(struct monst *);
E int rnd_misc_item(struct monst *);
E int rnd_misc_item_new(struct monst *);
E boolean searches_for_item(struct monst *,struct obj *);
E boolean mon_reflects(struct monst *,const char *);
E boolean ureflects(const char *,const char *);
E boolean munstone(struct monst *,BOOLEAN_P);

/* ### music.c ### */

E void NDECL(awaken_soldiers);
E int do_play_instrument(struct obj *);
E void do_earthquake(int);

/* ### nhlan.c ### */
#ifdef LAN_FEATURES
E void NDECL(init_lan_features);
E char *NDECL(lan_username);
# ifdef LAN_MAIL
E boolean NDECL(lan_mail_check);
E void lan_mail_read(struct obj *);
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
E int ntposkey(int *, int *, int *);
E void set_output_mode(int);
E void NDECL(synch_cursor);
#endif

/* ### o_init.c ### */

E void NDECL(init_objects);
E void NDECL(randommaterials);
E int NDECL(find_skates);
E int NDECL(find_skates2);
E int NDECL(find_skates3);
E int NDECL(find_skates4);
E int NDECL(find_opera_cloak);
E int NDECL(find_explosive_boots);
E int NDECL(find_irregular_boots);
E int NDECL(find_wedge_boots);
E int NDECL(find_aluminium_helmet);
E int NDECL(find_ghostly_cloak);
E int NDECL(find_polnish_gloves);
E int NDECL(find_velcro_boots);
E int NDECL(find_clumsy_gloves);
E int NDECL(find_fin_boots);
E int NDECL(find_profiled_boots);
E int NDECL(find_void_cloak);
E int NDECL(find_grey_shaded_gloves);
E int NDECL(find_weeb_cloak);
E int NDECL(find_persian_boots);
E int NDECL(find_hugging_boots);
E int NDECL(find_fleecy_boots);
E int NDECL(find_fingerless_gloves);
E int NDECL(find_mantle_of_coat);
E int NDECL(find_fatal_gloves);
E int NDECL(find_beautiful_heels);
E int NDECL(find_homicidal_cloak);
E int NDECL(find_castlevania_boots);
E int NDECL(find_greek_cloak);
E int NDECL(find_celtic_helmet);
E int NDECL(find_english_gloves);
E int NDECL(find_korean_sandals);
E int NDECL(find_octarine_robe);
E int NDECL(find_chinese_cloak);
E int NDECL(find_polyform_cloak);
E int NDECL(find_absorbing_cloak);
E int NDECL(find_birthcloth);
E int NDECL(find_poke_mongo_cloak);
E int NDECL(find_levuntation_cloak);
E int NDECL(find_quicktravel_cloak);
E int NDECL(find_angband_cloak);
E int NDECL(find_anorexia_cloak);
E int NDECL(find_dnethack_cloak);
E int NDECL(find_team_splat_cloak);
E int NDECL(find_eldritch_cloak);
E int NDECL(find_erotic_boots);
E int NDECL(find_secret_helmet);
E int NDECL(find_difficult_cloak);
E int NDECL(find_velvet_gloves);
E int NDECL(find_sputa_boots);
E int NDECL(find_formula_one_helmet);
E int NDECL(find_excrement_cloak);
E int NDECL(find_racer_gloves);
E int NDECL(find_turbo_boots);
E int NDECL(find_guild_cloak);
E int NDECL(find_shitty_gloves);
E int NDECL(find_foundry_cloak);
E int NDECL(find_spellsucking_cloak);
E int NDECL(find_storm_coat);
E int NDECL(find_fleeceling_cloak);
E int NDECL(find_princess_gloves);
E int NDECL(find_uncanny_gloves);
E int NDECL(find_slaying_gloves);
E int NDECL(find_blue_sneakers);
E int NDECL(find_femmy_boots);
E int NDECL(find_red_sneakers);
E int NDECL(find_yellow_sneakers);
E int NDECL(find_pink_sneakers);
E int NDECL(find_calf_leather_sandals);
E int NDECL(find_velcro_sandals);
E int NDECL(find_buffalo_boots);
E int NDECL(find_heroine_mocassins);
E int NDECL(find_lolita_boots);
E int NDECL(find_fetish_heels);
E int NDECL(find_weapon_light_boots);
E int NDECL(find_rubynus_helmet);
E void NDECL(oinit);
E void savenames(int,int);
E void restnames(int);
E void discover_object(int,BOOLEAN_P,BOOLEAN_P);
E void undiscover_object(int);
E int NDECL(dodiscovered);

/* ### objects.c ### */

E void NDECL(objects_init);

/* ### objnam.c ### */

E char *enchname(int);
E char *obj_typename(int);
E char *simple_typename(int);
E boolean obj_is_pname(struct obj *);
E char *distant_name(struct obj *,char *(*)(OBJ_P));
E char *fruitname(BOOLEAN_P);
E char *xname(struct obj *);
E char *mshot_xname(struct obj *);
E boolean the_unique_obj(struct obj *obj);
E char *doname(struct obj *);
E boolean not_fully_identified(struct obj *);
E char *corpse_xname(struct obj *,BOOLEAN_P);
E char *cxname(struct obj *);
E char *killer_xname(struct obj *);
E char *killer_cxname(struct obj *,BOOLEAN_P);
E const char *singular(struct obj *,char *(*)(OBJ_P));
E char *an(const char *);
E char *An(const char *);
E char *The(const char *);
E char *the(const char *);
E char *aobjnam(struct obj *,const char *);
E char *Tobjnam(struct obj *,const char *);
E char *otense(struct obj *,const char *);
E char *vtense(const char *,const char *);
E char *Doname2(struct obj *);
E char *yname(struct obj *);
E char *Yname2(struct obj *);
E char *ysimple_name(struct obj *);
E char *Ysimple_name2(struct obj *);
E char *makeplural(const char *);
E char *makesingular(const char *);
E struct obj *readobjnam(char *,struct obj *,BOOLEAN_P);
E int rnd_class(int,int);
E const char *cloak_simple_name(struct obj *);
E const char *mimic_obj_name(struct monst *);

/* ### options.c ### */

E boolean match_optname(const char *,const char *,int,BOOLEAN_P);
E void NDECL(initoptions);
E void parseoptions(char *,BOOLEAN_P,BOOLEAN_P);
E void parsetileset(char *);
E int NDECL(doset);
E int NDECL(dotogglepickup);
E void NDECL(option_help);
E void next_opt(winid,const char *);
E int fruitadd(char *);
E int choose_classes_menu(const char *,int,BOOLEAN_P,char *,char *);
E void add_menu_cmd_alias(CHAR_P, CHAR_P);
E char map_menu_cmd(CHAR_P);
E void assign_warnings(uchar *);
E char *nh_getenv(const char *);
E void set_duplicate_opt_detection(int);
E void set_wc_option_mod_status(unsigned long, int);
E void set_wc2_option_mod_status(unsigned long, int);
E void set_option_mod_status(const char *, int);
#ifdef STATUS_COLORS
E boolean parse_status_color_options(char *);
#endif
#ifdef MENU_COLOR
E boolean add_menu_coloring(char *);
#endif
#ifdef AUTOPICKUP_EXCEPTIONS
E int add_autopickup_exception(const char *);
E void NDECL(free_autopickup_exceptions);
#endif /* AUTOPICKUP_EXCEPTIONS */

/* ### pager.c ### */

E int NDECL(dowhatis);
E int NDECL(doquickwhatis);
E int NDECL(doidtrap);
E int NDECL(dowhatdoes);
E char *dowhatdoes_core(CHAR_P, char *);
E int NDECL(dohelp);
E int NDECL(dohistory);

E const char *NDECL(fauxmessage);
E const char *NDECL(bosstaunt);
E const char *NDECL(soviettaunt);

/* ### pcmain.c ### */

#if defined(MICRO) || defined(WIN32)
# ifdef CHDIR
E void chdirx(char *,BOOLEAN_P);
# endif /* CHDIR */
#endif /* MICRO || WIN32 */

/* ### pcsys.c ### */

#if defined(MICRO) || defined(WIN32)
E void NDECL(flushout);
E int NDECL(dosh);
# ifdef MFLOPPY
E void eraseall(const char *,const char *);
E void copybones(int);
E void NDECL(playwoRAMdisk);
E int saveDiskPrompt(int);
E void NDECL(gameDiskPrompt);
# endif
E void append_slash(char *);
E void getreturn(const char *);
# ifndef AMIGA
E void msmsg(const char *,...);
# endif
E FILE *fopenp(const char *,const char *);
#endif /* MICRO || WIN32 */

/* ### pctty.c ### */

#if defined(MICRO) || defined(WIN32)
E void NDECL(gettty);
E void settty(const char *);
E void NDECL(setftty);
E void error(const char *,...);
#if defined(TIMED_DELAY) && defined(_MSC_VER)
E void msleep(unsigned);
#endif
#endif /* MICRO || WIN32 */

/* ### pcunix.c ### */

#if defined(MICRO)
E void regularize(char *);
#endif /* MICRO */
#if defined(PC_LOCKING)
E void NDECL(getlock);
#endif

/* ### pickup.c ### */

#ifdef GOLDOBJ
E int collect_obj_classes(char *,struct obj *,BOOLEAN_P,boolean (*)(OBJ_P), int *);
#else
E int collect_obj_classes(char *,struct obj *,BOOLEAN_P,BOOLEAN_P,boolean (*)(OBJ_P), int *);
#endif
E void add_valid_menu_class(int);
E boolean allow_all(struct obj *);
E boolean allow_category(struct obj *);
E boolean is_worn_by_type(struct obj *);
E boolean mbag_explodes(struct obj *, int);
E void destroy_mbag(struct obj *, BOOLEAN_P);
#ifdef USE_TRAMPOLI
E int ck_bag(struct obj *);
E int in_container(struct obj *);
E int out_container(struct obj *);
#endif
E int pickup(int);
E int pickup_object(struct obj *, long, BOOLEAN_P);
E int query_category(const char *, struct obj *, int, menu_item **, int);
E int query_objlist(const char *, struct obj *, int, menu_item **, int, boolean (*)(OBJ_P));
E struct obj *pick_obj(struct obj *);
E int NDECL(encumber_msg);
E int NDECL(doloot);
E boolean container_gone(int (*)(OBJ_P));
E int use_container(struct obj **,int);
E int loot_mon(struct monst *,int *,boolean *);
E const char *safe_qbuf(const char *,unsigned, const char *,const char *,const char *);
E boolean is_autopickup_exception(struct obj *, BOOLEAN_P);

/* ### pline.c ### */

E void msgpline_add(int, char *);
E void NDECL(msgpline_free);
E void pline(const char *,...) PRINTF_F(1,2);
E void Norep(const char *,...) PRINTF_F(1,2);
E void NDECL(free_youbuf);
E void You(const char *,...) PRINTF_F(1,2);
E void Your(const char *,...) PRINTF_F(1,2);
E void You_feel(const char *,...) PRINTF_F(1,2);
E void You_cant(const char *,...) PRINTF_F(1,2);
E void You_hear(const char *,...) PRINTF_F(1,2);
E void pline_The(const char *,...) PRINTF_F(1,2);
E void There(const char *,...) PRINTF_F(1,2);
E void verbalize(const char *,...) PRINTF_F(1,2);
E void raw_printf(const char *,...) PRINTF_F(1,2);
E void impossible(const char *,...) PRINTF_F(1,2);
E const char *align_str(ALIGNTYP_P);
E const char *NDECL(hybrid_str);
E const char *NDECL(hybrid_strcode);
E const char *NDECL(generate_garbage_string);
E void mstatusline(struct monst *);
E void mstatuslinebl(struct monst *);
E void NDECL(ustatusline);
E void NDECL(self_invis_message);

/* ### polyself.c ### */

E void NDECL(init_uasmon);
E void NDECL(set_uasmon);
E void NDECL(change_sex);
E void NDECL(newman);
E void polyself(BOOLEAN_P);
E int polymon(int);
E void NDECL(rehumanize);
E int NDECL(dobreathe);
E int NDECL(dospit);
E int NDECL(doremove);
E int NDECL(dospinweb);
E int NDECL(dosummon);
E int NDECL(dogaze);
E int NDECL(dohide);
E int NDECL(domindblast);
E void skinback(BOOLEAN_P);
E const char *mbodypart(struct monst *,int);
E const char *body_part(int);
E int NDECL(poly_gender);
E void ugolemeffects(int,int);
E int NDECL(polyatwill);

/* ### potion.c ### */

E void set_itimeout(long *,long);
E void incr_itimeout(long *,int);
E void make_confused(long,BOOLEAN_P);
E void make_stunned(long,BOOLEAN_P);
E void make_numbed(long,BOOLEAN_P);
E void make_feared(long,BOOLEAN_P);
E void make_frozen(long,BOOLEAN_P);
E void make_burned(long,BOOLEAN_P);
E void make_dimmed(long,BOOLEAN_P);
E void make_blinded(long,BOOLEAN_P);
E void make_sick(long, const char *, BOOLEAN_P,int);
E void make_vomiting(long,BOOLEAN_P);
E boolean make_hallucinated(long,BOOLEAN_P,long);
E int NDECL(dodrink);
E void NDECL(badeffect);
E int dopotion(struct obj *);
E int peffects(struct obj *);
E void healup(int,int,BOOLEAN_P,BOOLEAN_P);
E void strange_feeling(struct obj *,const char *);
E void potionhit(struct monst *,struct obj *,BOOLEAN_P);
E void potionbreathe(struct obj *);
E boolean get_wet(struct obj *, BOOLEAN_P);
E int NDECL(dodip);
E void djinni_from_bottle(struct obj *,int);
/* KMH, balance patch -- new function */
E int upgrade_obj(struct obj *);
E struct monst *split_mon(struct monst *,struct monst *);
E const char *NDECL(bottlename);

/* ### pray.c ### */

/*#ifdef USE_TRAMPOLI*/
E int NDECL(prayer_done);

E void god_zaps_you(ALIGNTYP_P);
/*#endif*/
E int NDECL(dosacrifice);
E boolean can_pray(BOOLEAN_P);
E int NDECL(dopray);
E const char *NDECL(u_gname);
E int NDECL(doturn);
E int NDECL(turn_undead);
E int NDECL(turn_allmonsters);
E const char *NDECL(a_gname);
E const char *a_gname_at(XCHAR_P x,XCHAR_P y);
E const char *align_gname(ALIGNTYP_P);
E const char *halu_gname(ALIGNTYP_P);
E const char *align_gtitle(ALIGNTYP_P);
E void altar_wrath(int,int);


/* ### priest.c ### */

E int move_special(struct monst *,BOOLEAN_P,SCHAR_P,BOOLEAN_P,BOOLEAN_P, XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P);
E char temple_occupied(char *);
E int pri_move(struct monst *);
E void priestini(d_level *,struct mkroom *,int,int,BOOLEAN_P);
E char *priestname(struct monst *,char *);
E boolean p_coaligned(struct monst *);
E struct monst *findpriest(CHAR_P);
E void intemple(int);
E void priest_talk(struct monst *);
E struct monst *mk_roamer(struct permonst *,ALIGNTYP_P, XCHAR_P,XCHAR_P,BOOLEAN_P);
E void reset_hostility(struct monst *);
E boolean in_your_sanctuary(struct monst *,XCHAR_P,XCHAR_P);
E void ghod_hitsu(struct monst *);
E void NDECL(angry_priest);
E void NDECL(clearpriests);
E void restpriest(struct monst *,BOOLEAN_P);

/* ### quest.c ### */

E void NDECL(onquest);
E void NDECL(nemdead);
E void NDECL(artitouch);
E boolean NDECL(ok_to_quest);
E void leader_speaks(struct monst *);
E void NDECL(nemesis_speaks);
E void quest_chat(struct monst *);
E void quest_talk(struct monst *);
E void quest_stat_check(struct monst *);
E void finish_quest(struct obj *);

/* ### questpgr.c ### */

E void NDECL(load_qtlist);
E void NDECL(unload_qtlist);
E short quest_info(int);
E const char *NDECL(ldrname);
E boolean is_quest_artifact(struct obj*);
E void com_pager(int);
E void qt_pager(int);
E struct permonst *NDECL(qt_montype);

/* ### random.c ### */

#if defined(RANDOM) && !defined(__GO32__) /* djgpp has its own random */
E void srandom(unsigned);
E char *initstate(unsigned,char *,int);
E char *setstate(char *);
E long NDECL(random);
#endif /* RANDOM */

/* ### read.c ### */

E int NDECL(doread);
E boolean is_chargeable(struct obj *);
E boolean is_enchantable(struct obj *);
E void recharge(struct obj *,int);
E void randomenchant(struct obj *,int,BOOLEAN_P);
E void forget(int);
E void forget_objects(int);
E void forget_levels(int);
E void NDECL(forget_traps);
E void forget_map(int);
E void NDECL(maprot);
E int seffects(struct obj *);
#ifdef USE_TRAMPOLI
E void set_lit(int,int,genericptr_t);
#endif
E void litroom(BOOLEAN_P,struct obj *);
E void litroomlite(boolean);
E void do_genocide(int);
E void punish(struct obj *);
E void NDECL(unpunish);
E void NDECL(punishx);
E boolean cant_create(int *, BOOLEAN_P);
#ifdef WIZARD
E struct monst *NDECL(create_particular);
#endif

/* ### rect.c ### */

E void NDECL(init_rect);
E NhRect *get_rect(NhRect *);
E NhRect *NDECL(rnd_rect);
E void remove_rect(NhRect *);
E void add_rect(NhRect *);
E void split_rects(NhRect *,NhRect *);

/* ## region.c ### */
E void NDECL(clear_regions);
E void NDECL(run_regions);
E boolean in_out_region(XCHAR_P,XCHAR_P);
E boolean m_in_out_region(struct monst *,XCHAR_P,XCHAR_P);
E void NDECL(update_player_regions);
E void update_monster_region(struct monst *);
E NhRegion *visible_region_at(XCHAR_P,XCHAR_P);
E void show_region(NhRegion*, XCHAR_P, XCHAR_P);
E void save_regions(int,int);
E void rest_regions(int,BOOLEAN_P);
E NhRegion* create_gas_cloud(XCHAR_P, XCHAR_P, int, int);
E NhRegion* create_cthulhu_death_cloud(XCHAR_P, XCHAR_P, int, int);

/* ### restore.c ### */

E void inven_inuse(BOOLEAN_P);
E int dorecover(int);
E void trickery(char *);
E void getlev(int,int,XCHAR_P,BOOLEAN_P);
E void NDECL(minit);
E boolean lookup_id_mapping(unsigned, unsigned *);
#ifdef ZEROCOMP
E int mread(int,genericptr_t,unsigned int);
#else
E void mread(int,genericptr_t,unsigned int);
#endif

/* ### rip.c ### */

E void genl_outrip(winid,int);

/* ### rnd.c ### */

E int rn2(int);
E int rn3(int);
E int rnl(int);
E int rnd(int);
E int rno(int);
E int d(int,int);
E int rne(int);
E int rnz(int);
E int NDECL(randattack);
E int NDECL(randenchantment);
E int NDECL(randnastyenchantment);
E int NDECL(randomdisableproperty);
E int NDECL(randominsidetrap);
E int NDECL(randartmeleeweapon);
E int NDECL(randartlauncher);
E int NDECL(randartmissile);
E int NDECL(randartsuit);
E int NDECL(randartshirt);
E int NDECL(randartcloak);
E int NDECL(randarthelm);
E int NDECL(randartgloves);
E int NDECL(randartshield);
E int NDECL(randartboots);
E int NDECL(randartring);
E int NDECL(randartamulet);
E int NDECL(randartwand);
E int NDECL(randartspellbook);
E int NDECL(randartmeleeweaponX);
E int NDECL(randartlauncherX);
E int NDECL(randartmissileX);
E int NDECL(randartsuitX);
E int NDECL(randartshirtX);
E int NDECL(randartcloakX);
E int NDECL(randarthelmX);
E int NDECL(randartglovesX);
E int NDECL(randartshieldX);
E int NDECL(randartbootsX);
E int NDECL(randartringX);
E int NDECL(randartamuletX);
E int NDECL(randartwandX);
E int NDECL(randartspellbookX);

/* ### role.c ### */

E boolean validrole(int);
E boolean validrace(int, int);
E boolean validgend(int, int, int);
E boolean validalign(int, int, int);
E int NDECL(randrole);
E int randrace(int);
E int randgend(int, int);
E int randalign(int, int);
E int str2role(char *);
E int str2race(char *);
E int str2gend(char *);
E int str2align(char *);
E int mrace2race(int);
E boolean ok_role(int, int, int, int);
E int pick_role(int, int, int, int);
E boolean ok_race(int, int, int, int);
E int pick_race(int, int, int, int);
E boolean ok_gend(int, int, int, int);
E int pick_gend(int, int, int, int);
E boolean ok_align(int, int, int, int);
E int pick_align(int, int, int, int);
E void NDECL(role_init);
E void NDECL(rigid_role_checks);
E void NDECL(plnamesuffix);
E const char *Hello(struct monst *);
E const char *NDECL(Goodbye);
E char *build_plselection_prompt(char *, int, int, int, int, int);
E char *root_plselection_prompt(char *, int, int, int, int, int);
E void NDECL(recursioneffect);
E void NDECL(greenslimetransformation);
E void NDECL(bindertransformation);

/* ### rumors.c ### */

E char *getrumor(int,char *, BOOLEAN_P);
E void outrumor(int,int);
E void outoracle(BOOLEAN_P, BOOLEAN_P);
E void save_oracles(int,int);
E void restore_oracles(int);
E int doconsult(struct monst *);

/* ### save.c ### */

E int NDECL(dosave);
#if defined(UNIX) || defined(VMS) || defined(__EMX__) || defined(WIN32)
E void hangup(int);
#endif
E int NDECL(dosave0);
#ifdef INSURANCE
E void NDECL(savestateinlock);
#endif
#ifdef MFLOPPY
E boolean savelev(int,XCHAR_P,int);
E boolean swapin_file(int);
E void NDECL(co_false);
#else
E void savelev(int,XCHAR_P,int);
#endif
E void bufon(int);
E void bufoff(int);
E void bflush(int);
E void bwrite(int,genericptr_t,unsigned int);
E void bclose(int);
E void savefruitchn(int,int);
/* ### school.c ### */

E boolean is_principal(struct monst *);
E boolean is_teacher(struct monst *);
E boolean is_educator(struct monst *);
#ifdef SOUNDS
E void pupil_sound(struct monst *);
E void teacher_sound(struct monst *);
E void principal_sound(struct monst *);
#endif

E void NDECL(free_dungeons);
E void NDECL(freedynamicdata);

/* ### shk.c ### */

#ifdef GOLDOBJ
E long money2mon(struct monst *, long);
E void money2u(struct monst *, long);
#endif
E char *shkname(struct monst *);
E void shkgone(struct monst *);
E void set_residency(struct monst *,BOOLEAN_P);
E void replshk(struct monst *,struct monst *);
E void restshk(struct monst *,BOOLEAN_P);
E char inside_shop(XCHAR_P,XCHAR_P);
E void u_left_shop(char *,BOOLEAN_P);
E void remote_burglary(XCHAR_P,XCHAR_P);
E void u_entered_shop(char *);
E boolean same_price(struct obj *,struct obj *);
E void NDECL(shopper_financial_report);
E int inhishop(struct monst *);
E struct monst *shop_keeper(CHAR_P);
E boolean tended_shop(struct mkroom *);
E void delete_contents(struct obj *);
E void obfree(struct obj *,struct obj *);
E void home_shk(struct monst *,BOOLEAN_P);
E void make_happy_shk(struct monst *,BOOLEAN_P);
E void hot_pursuit(struct monst *);
E void make_angry_shk(struct monst *,XCHAR_P,XCHAR_P);
E int NDECL(dopay);
E boolean paybill(int);
E void NDECL(finish_paybill);
E struct obj *find_oid(unsigned);
E long contained_cost(struct obj *,struct monst *,long,BOOLEAN_P, BOOLEAN_P);
E long contained_gold(struct obj *);
E void picked_container(struct obj *);
E long unpaid_cost(struct obj *);
E void addtobill(struct obj *,BOOLEAN_P,BOOLEAN_P,BOOLEAN_P);
E void splitbill(struct obj *,struct obj *);
E void subfrombill(struct obj *,struct monst *);
E long stolen_value(struct obj *,XCHAR_P,XCHAR_P,BOOLEAN_P,BOOLEAN_P, BOOLEAN_P);
E void sellobj_state(int);
E void sellobj(struct obj *,XCHAR_P,XCHAR_P);
E int doinvbill(int);
E struct monst *shkcatch(struct obj *,XCHAR_P,XCHAR_P);
E void add_damage(XCHAR_P,XCHAR_P,long);
E int repair_damage(struct monst *,struct damage *,BOOLEAN_P);
E int shk_move(struct monst *);
E void after_shk_move(struct monst *);
E boolean is_fshk(struct monst *);
E void shopdig(int);
E void pay_for_damage(const char *,BOOLEAN_P);
E boolean costly_spot(XCHAR_P,XCHAR_P);
E struct obj *shop_object(XCHAR_P,XCHAR_P);
E void price_quote(struct obj *);
E void shk_chat(struct monst *);
E void check_unpaid_usage(struct obj *,BOOLEAN_P);
E void check_unpaid(struct obj *);
E void costly_gold(XCHAR_P,XCHAR_P,long);
E boolean block_door(XCHAR_P,XCHAR_P);
E boolean block_entry(XCHAR_P,XCHAR_P);
E boolean block_entry(XCHAR_P,XCHAR_P);
E void blkmar_guards(struct monst *);
E char *shk_your(char *,struct obj *);
E char *Shk_Your(char *,struct obj *);

/* ### shknam.c ### */

E void stock_room(int,struct mkroom *);
E boolean saleable(struct monst *,struct obj *);
E int get_shop_item(int);

/* ### sit.c ### */

E void NDECL(take_gold);
E int NDECL(dosit);
E void NDECL(rndcurse);
E void NDECL(attrcurse);
E void NDECL(skillcaploss);

/* ### sounds.c ### */

E void NDECL(dosounds);
E void pet_distress(struct monst *, int);
E const char *growl_sound(struct monst *);
/* JRN: converted growl,yelp,whimper to macros based on pet_distress.
  Putting them here since I don't know where else (TOFIX) */
#define growl(mon) pet_distress((mon),3)
#define yelp(mon) pet_distress((mon),2)
#define whimper(mon) pet_distress((mon),1)
E void beg(struct monst *);
E int NDECL(dotalk);
#ifdef USER_SOUNDS
E int add_sound_mapping(const char *);
E void play_sound_for_message(const char *);
#endif

/* ### sys/msdos/sound.c ### */

#ifdef MSDOS
E int assign_soundcard(char *);
#endif

/* ### sp_lev.c ### */

E boolean check_room(xchar *,xchar *,xchar *,xchar *,BOOLEAN_P, BOOLEAN_P);
E boolean create_room(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P, XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,BOOLEAN_P, BOOLEAN_P);
E void create_secret_door(struct mkroom *,XCHAR_P);
E boolean dig_corridor(coord *,coord *,BOOLEAN_P,SCHAR_P,SCHAR_P);
E void fill_room(struct mkroom *,BOOLEAN_P);
E boolean load_special(const char *);
E int NDECL(randomtrap);
E void NDECL(makerandomtrap);
E void NDECL(makegirlytrap);
E void NDECL(makespacewarstrap);

/* ### spell.c ### */

E boolean spell_known(int);
#ifdef USE_TRAMPOLI
E int NDECL(learn);
#endif
E int study_book(struct obj *);
E void book_disappears(struct obj *);
E void book_substitution(struct obj *,struct obj *);
E void NDECL(age_spells);
E int NDECL(docast);
E int spell_skilltype(int);
E int spelleffects(int,BOOLEAN_P);
E void NDECL(losespells);
E int NDECL(dovspell);
E void learnspell(struct obj *);
E boolean NDECL(studyspell);
E void initialspell(struct obj *);
E void NDECL(castinertiaspell);

/* ### steal.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(stealarm);
#endif
#ifdef GOLDOBJ
E long somegold(long);
#else
E long NDECL(somegold);
#endif
E void stealgold(struct monst *);
E void remove_worn_item(struct obj *,BOOLEAN_P);
E int steal(struct monst *, char *);
E int mpickobj(struct monst *,struct obj *,BOOLEAN_P);
E void stealamulet(struct monst *);
E void mdrop_special_objs(struct monst *);
E void relobj(struct monst *,int,BOOLEAN_P);
#ifdef GOLDOBJ
E struct obj *findgold(struct obj *);
#endif

/* ### steed.c ### */

E void NDECL(rider_cant_reach);
E boolean can_saddle(struct monst *);
E int use_saddle(struct obj *);
E boolean can_ride(struct monst *);
E int NDECL(doride);
E boolean mount_steed(struct monst *, BOOLEAN_P);
E void NDECL(exercise_steed);
E void NDECL(kick_steed);
E void dismount_steed(int);
E void place_monster(struct monst *,int,int);

/* ### tech.c ### */

E void adjtech(int,int);
E int NDECL(dotech);
E int NDECL(dotechwiz);
E void NDECL(docalm);
E int tech_inuse(int);
E void NDECL(tech_timeout);
E boolean tech_known(SHORT_P);
E void learntech(SHORT_P,long,int);
E int disarm_holdingtrap(struct trap *);
E int disarm_rust_trap(struct trap *);
E int disarm_fire_trap(struct trap *);
E int disarm_landmine(struct trap *);
E int disarm_squeaky_board(struct trap *);
E int disarm_shooting_trap(struct trap *, int);


/* ### teleport.c ### */

E boolean goodpos(int,int,struct monst *,unsigned);
E boolean enexto(coord *,XCHAR_P,XCHAR_P,struct permonst *);
E boolean enexto_core(coord *,XCHAR_P,XCHAR_P,struct permonst *,unsigned);
E int epathto(coord *,int,XCHAR_P,XCHAR_P,struct permonst *);
E void xpathto(int,XCHAR_P,XCHAR_P,int (*)(genericptr_t,int,int),void *);
E void teleds(int,int,BOOLEAN_P);
E boolean safe_teleds(BOOLEAN_P);
E boolean safe_teledsPD(BOOLEAN_P);
E boolean teleport_pet(struct monst *,BOOLEAN_P);
E void NDECL(tele);
E void phase_door(BOOLEAN_P);
E int NDECL(dotele);
E void NDECL(level_tele);
E void domagicportal(struct trap *);
E void tele_trap(struct trap *);
E void tele_trapX(struct trap *);
E void level_tele_trap(struct trap *);
E void level_tele_trapX(struct trap *);
E void rloc_to(struct monst *,int,int);
E boolean rloc(struct monst *, BOOLEAN_P);
E boolean tele_restrict(struct monst *);
E void mtele_trap(struct monst *, struct trap *,int);
E int mlevel_tele_trap(struct monst *, struct trap *,BOOLEAN_P,int);
E void rloco(struct obj *);
E int NDECL(random_teleport_level);
E int NDECL(random_banish_level);
E void NDECL(teleX);
E boolean u_teleport_mon(struct monst *,BOOLEAN_P);
E boolean u_teleport_monB(struct monst *,BOOLEAN_P);
E void NDECL(pushplayer);
E void NDECL(pushplayersilently);
E boolean teleok(int,int,BOOLEAN_P);

/* ### tile.c ### */
#ifdef USE_TILES
E void substitute_tiles(d_level *);
#endif

/* ### timeout.c ### */

E void NDECL(burn_away_slime);
E void NDECL(nh_timeout);
E void fall_asleep(int, BOOLEAN_P);
E void set_obj_poly(struct obj *, struct obj *);
E void unpoly_obj(genericptr_t, long);
E int mon_poly(struct monst *, BOOLEAN_P, const char *);
E int mon_spec_poly(struct monst *, struct permonst *, long, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P);
E int mon_spec_polyX(struct monst *, struct permonst *, long, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P);
E void unpoly_mon(genericptr_t, long);
E void attach_bomb_blow_timeout(struct obj *, int, BOOLEAN_P);
E void attach_egg_hatch_timeout(struct obj *);
E void attach_fig_transform_timeout(struct obj *);
E void kill_egg(struct obj *);
E void hatch_egg(genericptr_t, long);
E void learn_egg_type(int);
E void burn_object(genericptr_t, long);
E void begin_burn(struct obj *, BOOLEAN_P);
E void end_burn(struct obj *, BOOLEAN_P);
E void burn_faster(struct obj *, long);
E void lightsaber_deactivate(struct obj *, BOOLEAN_P);
E void NDECL(do_storms);
E boolean start_timer(long, SHORT_P, SHORT_P, genericptr_t);
E long stop_timer(SHORT_P, genericptr_t);
E void NDECL(run_timers);
E void obj_move_timers(struct obj *, struct obj *);
E void obj_split_timers(struct obj *, struct obj *);
E void obj_stop_timers(struct obj *);
E void mon_stop_timers(struct monst *);
E boolean obj_is_local(struct obj *);
E void save_timers(int,int,int);
E void restore_timers(int,int,BOOLEAN_P,long);
E void relink_timers(BOOLEAN_P);
#ifdef WIZARD
E int NDECL(wiz_timeout_queue);
E void NDECL(timer_sanity_check);
#endif
E void NDECL(cryogenics);

/* ### topten.c ### */

E void topten(int);
E void prscore(int,char **);
E struct obj *tt_oname(struct obj *);
/*E void tt_mname(struct monst *);*/
/*E void mkundeadboo(coord *,BOOLEAN_P,int);*/
E void tt_mname(coord *,BOOLEAN_P,int);
#ifdef GTK_GRAPHICS
E winid NDECL(create_toptenwin);
E void NDECL(destroy_toptenwin);
#endif

/* ### track.c ### */

E void NDECL(initrack);
E void NDECL(settrack);
E coord *gettrack(int,int);

/* ### trap.c ### */

E boolean burnarmor(struct monst *);
E boolean rust_dmg(struct obj *,const char *,int,BOOLEAN_P,struct monst *);

E boolean wither_dmg(struct obj *,const char *,int,BOOLEAN_P,struct monst *);

E void grease_protect(struct obj *,const char *,struct monst *);
E struct trap *maketrap(int,int,int,int);
E void makerandomtrap_at(int,int);
E void fall_through(BOOLEAN_P);
E void fall_throughX(BOOLEAN_P);
E struct monst *animate_statue(struct obj *,XCHAR_P,XCHAR_P,int,int *);
E struct monst *activate_statue_trap(struct trap *,XCHAR_P,XCHAR_P,BOOLEAN_P);
E void dotrap(struct trap *, unsigned);
E void seetrap(struct trap *);
E int mintrap(struct monst *);
E void instapetrify(const char *);
E void minstapetrify(struct monst *,BOOLEAN_P);
E void selftouch(const char *);
E void mselftouch(struct monst *,const char *,BOOLEAN_P);
E void NDECL(float_up);
E void fill_pit(int,int);
E int float_down(long, long);
E int fire_damage(struct obj *,BOOLEAN_P,BOOLEAN_P,XCHAR_P,XCHAR_P);
E void water_damage(struct obj *,BOOLEAN_P,BOOLEAN_P);
E void doshittrap(struct obj *);
E void lethe_damage(struct obj *,BOOLEAN_P,BOOLEAN_P);
E void withering_damage(struct obj *,BOOLEAN_P,BOOLEAN_P);
E void antimatter_damage(struct obj *,BOOLEAN_P,BOOLEAN_P);
E boolean NDECL(drown);
E void mon_drain_en(struct monst *, int);
E void drain_en(int);
E int NDECL(dountrap);
E int untrap(BOOLEAN_P);
E boolean chest_trap(struct obj *,int,BOOLEAN_P);
E void deltrap(struct trap *);
E boolean delfloortrap(struct trap *);
E struct trap *t_at(int,int);
E void b_trapped(const char *,int);
E boolean NDECL(unconscious);
E boolean NDECL(lava_effects);
E void blow_up_landmine(struct trap *);
E int launch_obj(SHORT_P,int,int,int,int,int);
/* KMH, balance patch -- new function */
E int NDECL(uunstone);

/* ### u_init.c ### */

E void NDECL(u_init);
E void NDECL(alter_reality);
E void NDECL(polyinitors);

/* ### uhitm.c ### */

E void hurtmarmor(struct monst *,int);
E int attack_checks(struct monst *,BOOLEAN_P);
E void check_caitiff(struct monst *);
E schar find_roll_to_hit(struct monst *);
E boolean attack(struct monst *);
E boolean hmon(struct monst *,struct obj *,int);
E int damageum(struct monst *,struct attack *);
E void missum(struct monst *,int, int, struct attack *);
E int passive(struct monst *,int,int,UCHAR_P);
E void passive_obj(struct monst *,struct obj *,struct attack *);
E void stumble_onto_mimic(struct monst *);
E int flash_hits_mon(struct monst *,struct obj *);

/* ### unixmain.c ### */

#ifdef UNIX
# ifdef PORT_HELP
E void NDECL(port_help);
# endif
#endif /* UNIX */

/* ### unixtty.c ### */

#if defined(UNIX) || defined(__BEOS__)
E void NDECL(gettty);
E void settty(const char *);
E void NDECL(setftty);
E void NDECL(intron);
E void NDECL(introff);
E void error(const char *,...) PRINTF_F(1,2);
#endif /* UNIX || __BEOS_ */

/* ### unixunix.c ### */

#ifdef UNIX
E void NDECL(getlock);
E void regularize(char *);
# if defined(TIMED_DELAY) && !defined(msleep) && defined(SYSV)
E void msleep(unsigned);
# endif
# ifndef PUBLIC_SERVER
E int NDECL(dosh);
# endif /* PUBLIC_SERVER */
# if !defined(PUBLIC_SERVER) || defined(DEF_PAGER) || defined(DEF_MAILREADER)
E int child(int);
# endif
#ifdef FILE_AREAS
E char *make_file_name(const char *, const char *);
E FILE *fopen_datafile_area(const char *,const char *,const char *, BOOLEAN_P);
E FILE *freopen_area(const char *,const char *,const char *, FILE *);
E int chmod_area(const char *, const char *, int);
E int open_area(const char *, const char *, int, int);
E int creat_area(const char *, const char *, int);
E boolean lock_file_area(const char *, const char *,int);
E void unlock_file_area(const char *, const char *);
#endif
#endif /* UNIX */

/* ### unixres.c ### */

#ifdef UNIX
E int hide_privileges(BOOLEAN_P);
#endif /* UNIX */

/* ### unixres.c ### */

#ifdef UNIX
# ifdef GNOME_GRAPHICS 
E int hide_privileges(BOOLEAN_P);
# endif
#endif /* UNIX */

/* ### vault.c ### */

E boolean grddead(struct monst *);
E char vault_occupied(char *);
E void NDECL(invault);
E int gd_move(struct monst *);
E void NDECL(paygd);
E long NDECL(hidden_gold);
E boolean NDECL(gd_sound);

/* ### version.c ### */

E char *version_string(char *);
E char *getversionstring(char *);
E int NDECL(doversion);
E int NDECL(doextversion);
#ifdef MICRO
E boolean comp_times(long);
#endif
E boolean check_version(struct version_info *, const char *,BOOLEAN_P);
E unsigned long get_feature_notice_ver(char *);
E unsigned long NDECL(get_current_feature_ver);
#ifdef RUNTIME_PORT_ID
E void append_port_id(char *);
#endif

/* ### video.c ### */

#ifdef MSDOS
E int assign_video(char *);
# ifdef NO_TERMS
E void NDECL(gr_init);
E void NDECL(gr_finish);
# endif
E void tileview(BOOLEAN_P);
#endif
#ifdef VIDEOSHADES
E int assign_videoshades(char *);
E int assign_videocolors(char *);
#endif

/* ### vis_tab.c ### */

#ifdef VISION_TABLES
E void NDECL(vis_tab_init);
#endif

/* ### vision.c ### */

E void NDECL(vision_init);
E int does_block(int,int,struct rm*);
E void NDECL(vision_reset);
E void vision_recalc(int);
E void block_point(int,int);
E void unblock_point(int,int);
E boolean clear_path(int,int,int,int);
E void do_clear_area(int,int,int, void (*)(int,int,genericptr_t),genericptr_t);
E void do_clear_areaX(int,int,int, void (*)(int,int,genericptr_t),genericptr_t);

#ifdef VMS

/* ### vmsfiles.c ### */

E int vms_link(const char *,const char *);
E int vms_unlink(const char *);
E int vms_creat(const char *,unsigned int);
E int vms_open(const char *,int,unsigned int);
E boolean same_dir(const char *,const char *);
E int c__translate(int);
E char *vms_basename(const char *);

/* ### vmsmail.c ### */

E unsigned long NDECL(init_broadcast_trapping);
E unsigned long NDECL(enable_broadcast_trapping);
E unsigned long NDECL(disable_broadcast_trapping);
# if 0
E struct mail_info *NDECL(parse_next_broadcast);
# endif /*0*/

/* ### vmsmain.c ### */

E int main(int, char **);
# ifdef CHDIR
E void chdirx(const char *,BOOLEAN_P);
# endif /* CHDIR */

/* ### vmsmisc.c ### */

E void NDECL(vms_abort);
E void vms_exit(int);

/* ### vmstty.c ### */

E int NDECL(vms_getchar);
E void NDECL(gettty);
E void settty(const char *);
E void shuttty(const char *);
E void NDECL(setftty);
E void NDECL(intron);
E void NDECL(introff);
E void error(const char *,...) PRINTF_F(1,2);
#ifdef TIMED_DELAY
E void msleep(unsigned);
#endif

/* ### vmsunix.c ### */

E void NDECL(getlock);
E void regularize(char *);
E int NDECL(vms_getuid);
E boolean file_is_stmlf(int);
E int vms_define(const char *,const char *,int);
E int vms_putenv(const char *);
E char *NDECL(verify_termcap);
# if defined(CHDIR) || !defined(PUBLIC_SERVER) || defined(SECURE)
E void NDECL(privoff);
E void NDECL(privon);
# endif
# ifndef PUBLIC_SERVER
E int NDECL(dosh);
# endif
# if !defined(PUBLIC_SERVER) || defined(MAIL)
E int vms_doshell(const char *,BOOLEAN_P);
# endif
# ifdef SUSPEND
E int NDECL(dosuspend);
# endif

#endif /* VMS */

/* ### weapon.c ### */

E int hitval(struct obj *,struct monst *);
E int dmgval(struct obj *,struct monst *);
E int dmgvalX(struct obj *,struct monst *);
E struct obj *select_rwep(struct monst *);
E struct obj *select_hwep(struct monst *);
E void possibly_unwield(struct monst *,BOOLEAN_P);
E int mon_wield_item(struct monst *);
E int NDECL(abon);
E int NDECL(dbon);
E int NDECL(enhance_weapon_skill);
#ifdef DUMP_LOG
E void NDECL(dump_weapon_skill);
#endif
E void unrestrict_weapon_skill(int);
E void use_skill(int,int);
E void add_weapon_skill(int);
E void lose_weapon_skill(int);
E int weapon_type(struct obj *);
E int NDECL(uwep_skill_type);
E int weapon_hit_bonus(struct obj *);
E int weapon_dam_bonus(struct obj *);
E int skill_bonus(int);
E void skill_init(const struct def_skill *);
E void NDECL(practice_weapon);
E void NDECL(xtraskillinit);
E int get_obj_skill(struct obj *);

/* ### were.c ### */

E int counter_were(int);
E void were_change(struct monst *);
E void new_were(struct monst *);
E int were_summon(struct permonst *,BOOLEAN_P,int *,char *);
E void NDECL(you_were);
E void you_unwere(BOOLEAN_P);

/* ### wield.c ### */

E void setuwep(struct obj *,BOOLEAN_P);
E void setuqwep(struct obj *);
E void setuswapwep(struct obj *,BOOLEAN_P);
E int NDECL(dowield);
E int NDECL(doswapweapon);
E int NDECL(dowieldquiver);
E boolean wield_tool(struct obj *,const char *);
E int NDECL(can_twoweapon);
E void NDECL(drop_uswapwep);
E int NDECL(dotwoweapon);
E void NDECL(uwepgone);
E void NDECL(uswapwepgone);
E void NDECL(uqwepgone);
E void NDECL(untwoweapon);
E void erode_obj(struct obj *,BOOLEAN_P,BOOLEAN_P);
E int chwepon(struct obj *,int);
E int welded(struct obj *);
E void weldmsg(struct obj *);
E void setmnotwielded(struct monst *,struct obj *);
E void unwield(struct obj *,BOOLEAN_P);

/* ### windows.c ### */

E void choose_windows(const char *);
E char genl_message_menu(CHAR_P,int,const char *);
E void genl_preference_update(const char *);

/* ### wizard.c ### */

E void NDECL(amulet);
E int mon_has_amulet(struct monst *);
E int mon_has_special(struct monst *);
E int tactics(struct monst *);
E void NDECL(aggravate);
E void NDECL(clonewiz);
E int NDECL(pick_nasty);
E int nasty(struct monst*);
E void NDECL(resurrect);
E void NDECL(intervene);
E void NDECL(wizdead);
E void cuss(struct monst *);
E void NDECL(randomcuss);

/* ### worm.c ### */

E int NDECL(get_wormno);
E void initworm(struct monst *,int);
E void worm_move(struct monst *);
E void worm_nomove(struct monst *);
E void wormgone(struct monst *);
E void wormhitu(struct monst *);
E int cutworm(struct monst *,XCHAR_P,XCHAR_P,struct obj *);
E void see_wsegs(struct monst *);
E void detect_wsegs(struct monst *,BOOLEAN_P);
E void save_worm(int,int);
E void rest_worm(int);
E void place_wsegs(struct monst *);
E void remove_worm(struct monst *);
E void place_worm_tail_randomly(struct monst *,XCHAR_P,XCHAR_P);
E int count_wsegs(struct monst *);
E boolean worm_known(struct monst *);

/* ### worn.c ### */

E void setworn(struct obj *,long);
E void setnotworn(struct obj *);
E void mon_set_minvis(struct monst *);
E void mon_adjust_speed(struct monst *,int,struct obj *);
E void update_mon_intrinsics(struct monst *,struct obj *,BOOLEAN_P,BOOLEAN_P);
E int find_mac(struct monst *);
E void m_dowear(struct monst *,BOOLEAN_P);
E struct obj *which_armor(struct monst *,long);
E void mon_break_armor(struct monst *,BOOLEAN_P);
E void bypass_obj(struct obj *);
E void NDECL(clear_bypasses);
E int racial_exception(struct monst *, struct obj *);

/* ### write.c ### */

E int dowrite(struct obj *);

/* ### zap.c ### */

E int bhitm(struct monst *,struct obj *);
E void probe_monster(struct monst *);
E boolean get_obj_location(struct obj *,xchar *,xchar *,int);
E boolean get_mon_location(struct monst *,xchar *,xchar *,int);
E struct monst *get_container_location(struct obj *obj, int *, int *);
E struct monst *montraits(struct obj *,coord *);
E struct monst *revive(struct obj *);
E int unturn_dead(struct monst *);
E void cancel_item(struct obj *);
E boolean drain_item(struct obj *);	/* KMH */
E boolean drain_item_reverse(struct obj *);	/* KMH */
E boolean drain_item_severely(struct obj *);	/* KMH */
E boolean drain_item_negative(struct obj *);	/* KMH */
E boolean obj_resists(struct obj *,int,int);
E boolean obj_shudders(struct obj *);
E void do_osshock(struct obj *);
E void puton_worn_item(struct obj *);
E struct obj *poly_obj(struct obj *, int);
E int bhito(struct obj *,struct obj *);
E int bhitpile(struct obj *,int (*)(OBJ_P,OBJ_P),int,int);
E int zappable(struct obj *);
E void zapnodir(struct obj *);
E int NDECL(dozap);
E int zapyourself(struct obj *,BOOLEAN_P);
E boolean cancel_monst(struct monst *,struct obj *, BOOLEAN_P,BOOLEAN_P,BOOLEAN_P);
E void weffects(struct obj *);
E int spell_damage_bonus(int);
E const char *exclam(int force);
E void hit(const char *,struct monst *,const char *);
E void miss(const char *,struct monst *);
E struct monst *bhit(int,int,int,int,int (*)(MONST_P,OBJ_P), int (*)(OBJ_P,OBJ_P),struct obj **);
E struct monst *boomhit(int,int);
E int burn_floor_paper(int,int,BOOLEAN_P,BOOLEAN_P);
E void buzz(int,int,XCHAR_P,XCHAR_P,int,int);
E void melt_ice(XCHAR_P,XCHAR_P);
E int zap_over_floor(XCHAR_P,XCHAR_P,int,boolean *);
E void fracture_rock(struct obj *);
E boolean break_statue(struct obj *);
E void destroy_item(int,int);
E int destroy_mitem(struct monst *,int,int);
E int resist(struct monst *,CHAR_P,int,int);
E void NDECL(makewish);
E void NDECL(makenonworkingwish);
E void NDECL(othergreateffect);
/* KMH -- xchar to XCHAR_P */
E void zap_strike_fx(XCHAR_P, XCHAR_P, int);
E void NDECL(throwspell);

/* ### livelog.c ### */
#ifdef LIVELOGFILE
E boolean NDECL(livelog_start);
E void NDECL(livelog_achieve_update);
E void livelog_wish(char*);
E void NDECL(livelog_avert_death);
#ifdef LIVELOG_SHOUT
E int NDECL(doshout);
#endif
#endif

/* ### termcap.c ### */
E void term_start_bgcolor(int);
E void NDECL(term_end_bgcolor);

#endif /* !MAKEDEFS_C && !LEV_LEX_C */

#undef E

#endif /* EXTERN_H */
