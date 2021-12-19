/*	SCCS Id: @(#)extern.h	3.4	2003/03/10	*/
/* Copyright (c) Steve Creps, 1988.				  */
/* NetHack may be freely redistributed.  See license for details. */


#include "config.h"
#ifndef EXTERN_H
#define EXTERN_H

#define E extern

/* ### alloc.c ### */

#if 0
E void * alloc(size_t);
#endif
E char *fmt_ptr(const void *,char *);

/* This next pre-processor directive covers almost the entire file,
 * interrupted only occasionally to pick up specific functions as needed. */
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)

/* ### allmain.c ### */

E void moveloop(void);
E void stop_occupation(void);
E void display_gamewindows(void);
E void newgame(void);
E boolean contaminationcheck(void);
E void welcome(BOOLEAN_P);
#if defined(RECORD_REALTIME) || defined(REALTIME_ON_BOTL)
E time_t get_realtime(void);
#endif
E boolean timebasedlowerchance(void);
E int speedbonus(int, int);

/* ### apply.c ### */

E int doapply(void);
E int dorub(void);
E int dojump(void);
E int jump(int);
E int jump(int);
E int number_leashed(void);
E void o_unleash(struct obj *);
E void m_unleash(struct monst *,BOOLEAN_P);
E void unleash_all(void);
E boolean next_to_u(void);
E struct obj *get_mleash(struct monst *);
E void check_leash(XCHAR_P,XCHAR_P);
E void use_floppies(struct obj *);
E boolean um_dist(XCHAR_P,XCHAR_P,XCHAR_P);
E boolean snuff_candle(struct obj *);
E boolean snuff_lit(struct obj *);
E boolean catch_lit(struct obj *);
E boolean use_unicorn_horn(struct obj *);
E boolean use_bubble_horn(struct obj *);
E boolean tinnable(struct obj *);
E void reset_trapset(void);
E void fig_transform(void *, long);
E int unfixable_trouble_count(BOOLEAN_P);
E int wand_explode(struct obj *,BOOLEAN_P);
E boolean cyanspellok(int, int);

/* ### artifact.c ### */

E void elemental_imbue(int);
E void init_artifacts(void);
E void init_artifacts1(void);
E void save_artifacts(int);
E void restore_artifacts(int);
E const char *artiname(int);
E struct obj *mk_artifact(struct obj *,ALIGNTYP_P,BOOLEAN_P);
E void bad_artifact(void);
E void bad_artifact_xtra(void);
E const char *artifact_name(const char *,int *);
E boolean exist_artifact(int,const char *);
E void artifact_exists(struct obj *,const char *,BOOLEAN_P);
E int nartifact_exist(void);
E boolean spec_ability(struct obj *,unsigned long);
E boolean arti_is_evil(struct obj *);
E boolean arti_nonwishable(struct obj *);
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
E void dump_artifact_discoveries(void);
E boolean artifact_hit(struct monst *,struct monst *, struct obj *,int *,int);
E int doinvoke(void);
E void arti_speak(struct obj *);
E boolean artifact_light(struct obj *);
E int artifact_wet(struct obj *, BOOLEAN_P);
E void arti_speak(struct obj *);
E boolean artifact_light(struct obj *);
E long spec_m2(struct obj *);
E boolean artifact_has_invprop(struct obj *,int);
E long arti_cost(struct obj *);
E void arti_poly_contents(struct obj *);
E int find_wildtalentartifact(void);
E int find_prostituteartifact(void);
E int find_kurwaartifact(void);
E void intrinsicgainorloss(void);

/* ### attrib.c ### */

E boolean adjattrib(int,int,int,BOOLEAN_P);
E void change_luck(SCHAR_P);
E int stone_luck(BOOLEAN_P);
E void set_moreluck(void);
E void gainstr(struct obj *,int);
E void losestr(int,BOOLEAN_P);
E void restore_attrib(void);
E void exercise(int,BOOLEAN_P);
E void exerchk(void);
E void reset_attribute_clock(void);
E void init_attr(int,BOOLEAN_P);
E void redist_attr(void);
E void adjabil(int,int);
E int newhp(void);
E schar acurr(int);
E schar acurrstr(void);
E void adjalign(int);
/* KMH, balance patch -- new function */
E void recalc_health(void);
E int recalc_mana(void);
E boolean attr_will_go_up(int, BOOLEAN_P);
E void bodymorph(void);

/* ### ball.c ### */

E void ballfall(void);
E void placebc(void);
E void unplacebc(void);
E void set_bc(int);
E void move_bc(int,int,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P);
E boolean drag_ball(XCHAR_P,XCHAR_P, int *,xchar *,xchar *,xchar *,xchar *, boolean *,BOOLEAN_P);
E void drop_ball(XCHAR_P,XCHAR_P);
E void drag_down(void);

/* ### bones.c ### */

E boolean can_make_bones(void);
E void savebones(struct obj *);
E int getbones(void);
E char *bonedunlvl(XCHAR_P);

/* ### borg.c ### */

#ifdef BORG
E char borg_on;
E char borg_line[80];
E char borg_input(void);
#endif

/* ### botl.c ### */

E int xlev_to_rank(int);
E int title_to_mon(const char *,int *,int *);
E void max_rank_sz(void);
E long botl_score(void);
E int describe_level(char *, int);
E const char *rank(void);
E const char *rank_of(int,int,BOOLEAN_P);
E void bot_set_handler(void (*)());
E void bot_reconfig(void);
E void bot(void);
E void botreal(void);
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

E char randomkey(void);
#ifdef USE_TRAMPOLI
E int doextcmd(void);
E int domonability(void);
E int domonability(void);
E int polyatwill(void);
E int playersteal(void);
E int doprev_message(void);
E int timed_occupation(void);
E int wiz_attributes(void);
E int enter_explore_mode(void);
# ifdef WIZARD
E int wiz_detect(void);
E int wiz_genesis(void);
E int wiz_identify(void);
E int wiz_level_tele(void);
E int wiz_map(void);
E int wiz_where(void);
E int wiz_wish(void);
E char *montraitname(int);
# endif /* WIZARD */
#endif /* USE_TRAMPOLI */
E void reset_occupations(void);
E void set_occupation(int (*)(void),const char *,int);
E char pgetchar(void);
E void pushch(CHAR_P);
E void savech(CHAR_P);
E void rhack(char *);
E void dokeylist(void);
E int doextlist(void);
E int extcmd_via_menu(void);
E void enlightenment(int, int);
E void show_conduct(int);
#ifdef DUMP_LOG
E void dump_enlightenment(int);
E void dump_conduct(int);
E void end_of_input(void);
#endif
E int xytod(SCHAR_P,SCHAR_P);
E void dtoxy(coord *,int);
E int movecmd(CHAR_P);
E int getdir(const char *);
E void confdir(void);
E int isok(int,int);
E int get_adjacent_loc(const char *, const char *, XCHAR_P, XCHAR_P, coord *);
E const char *click_to_cmd(int,int,int);
E char readchar(void);
#ifdef WIZARD
E void sanity_check(void);
#endif
E void commands_init(void);
E char* stripspace(char*);
E void parsebindings(char*);
E void parseautocomplete(char*,boolean);
E char txt2key(char*);
E char* key2txt(char, char*);
E char* str2txt(char*, char*);
E char yn_function(const char *, const char *, CHAR_P);

/* ### dbridge.c ### */

E boolean is_pool(int,int);
E boolean is_waterypool(int,int);
E boolean is_drowningpool(int,int);
E boolean is_lava(int,int);
E boolean is_watertunnel(int,int);
E boolean is_crystalwater(int,int);
E boolean is_moorland(int,int);
E boolean is_pavedfloor(int,int);
E boolean is_highway(int,int);
E boolean is_urinelake(int,int);
E boolean is_shiftingsand(int,int);
E boolean is_styxriver(int,int);
E boolean is_carvedbed(int,int);
E boolean is_wagon(int,int);
E boolean is_burningwagon(int,int);
E boolean is_well(int,int);
E boolean is_poisonedwell(int,int);
E boolean is_pentagram(int,int);
E boolean is_nethermist(int,int);
E boolean is_stalactite(int,int);
E boolean is_raincloud(int,int);
E boolean is_bubble(int,int);
E boolean is_ice(int,int);
E boolean is_ash(int,int);
E boolean is_farmland(int,int);
E boolean is_sand(int,int);
E boolean is_grassland(int,int);
E boolean is_snow(int,int);
E boolean is_table(int,int);
E boolean is_mattress(int,int);
E boolean isimportantlocation(int,int);
E int is_drawbridge_wall(int,int);
E boolean is_db_wall(int,int);
E boolean find_drawbridge(int *,int*);
E boolean create_drawbridge(int,int,int,BOOLEAN_P);
E void open_drawbridge(int,int);
E void close_drawbridge(int,int);
E void destroy_drawbridge(int,int);

/* ### decl.c ### */

E void decl_init(void);

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
E boolean use_crystal_ball(struct obj *);
E void water_detect(void);
E void water_detectX(void);
E void do_mapping(void);
E void do_mappingX(void);
E void do_mappingY(void);
E void do_mappingZ(void);
E void do_vicinity_map(void);
E void do_vicinity_mapX(void);
E void cvt_sdoor_to_door(struct rm *);
#ifdef USE_TRAMPOLI
E void findone(int,int,void *);
E void openone(int,int,void *);
#endif
E int findit(void);
E int finditX(void);
E int openit(void);
E void find_trap(struct trap *);
E int dosearch0(int);
E int dosearch(void);
E void sokoban_detect(void);
/* KMH -- Sokoban levels */
E void sokoban_detect(void);

/* ### dig.c ### */

E boolean is_digging(void);
#ifdef USE_TRAMPOLI
E int dig(void);
#endif
E int holetime(void);
E boolean dig_check(struct monst *, BOOLEAN_P, int, int);
E void digactualhole(int,int,struct monst *,int);
E boolean dighole(BOOLEAN_P);
E int use_pick_axe(struct obj *);
E int use_pick_axe2(struct obj *);
E boolean mdig_tunnel(struct monst *);
E void watch_dig(struct monst *,XCHAR_P,XCHAR_P,BOOLEAN_P);
E void zap_dig(BOOLEAN_P);
E void stardigging(void);
E struct obj *bury_an_obj(struct obj *);
E void bury_objs(int,int);
E void unearth_objs(int,int);
E void rot_organic(void *, long);
E void rot_corpse(void *, long);
#if 0
E void bury_monst(struct monst *);
E void bury_you(void);
E void unearth_you(void);
E void escape_tomb(void);
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
E void see_monsters(void);
E void see_monstersX(void);
E void set_mimic_blocking(void);
E void see_objects(void);
E void see_traps(void);
E void curs_on_u(void);
E int doredraw(void);
E void docrt(void);
E void show_glyph(int,int,int);
E void show_glyphX(int,int,int);
E void clear_glyph_buffer(void);
E void row_refresh(int,int,int);
E void cls(void);
E void flush_screen(int);
#ifdef DUMP_LOG
E void dump_screen(void);
#endif
E int back_to_glyph(XCHAR_P,XCHAR_P);
E int zapdir_to_glyph(int,int,int);
E int glyph_at(XCHAR_P,XCHAR_P);
E void set_wall_state(void);
E boolean sensemon(struct monst *);

/* ### do.c ### */

#ifdef USE_TRAMPOLI
E int drop(struct obj *);
E int wipeoff(void);
#endif
E int dodrop(void);
E boolean boulder_hits_pool(struct obj *,int,int,BOOLEAN_P);
E boolean flooreffects(struct obj *,int,int,const char *);
E void doaltarobj(struct obj *);
E boolean canletgo(struct obj *,const char *);
E void dropx(struct obj *);
E void dropy(struct obj *);
E void obj_no_longer_held(struct obj *);
E int doddrop(void);
E int dodown(void);
E int doup(void);
#ifdef INSURANCE
E void save_currentstate(void);
#endif
E void goto_level(d_level *,BOOLEAN_P,BOOLEAN_P,BOOLEAN_P);
E void schedule_goto(d_level *,BOOLEAN_P,BOOLEAN_P,int, const char *,const char *);
E void deferred_goto(void);
E boolean revive_corpse(struct obj *, BOOLEAN_P);
E void revive_mon(void *, long);
E void moldy_corpse(void *, long);
E int donull(void);
E int dowipe(void);
E void set_wounded_legs(long,int);
E void heal_legs(void);

/* ### do_name.c ### */

E int getpos(coord *,BOOLEAN_P,const char *);
E struct monst *christen_monst(struct monst *,const char *);
E int do_mname(void);
E struct obj *oname(struct obj *,const char *);
E struct obj *onameX(struct obj *,const char *);
E int ddocall(void);
E void docall(struct obj *);
E const char *rndghostname(void);
E const char *rndplrmonname(void);
E const char *rndplrmonnamefemale(void);
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
E char *a_noit_monnam(struct monst *);
E char *distant_monnam(struct monst *,int,char *);
E const char *rndmonnam(void);
E const char *hcolor(const char *);
E const char *rndcolor(void);
#ifdef REINCARNATION
E const char *roguename(void);
#endif
E struct obj *realloc_obj(struct obj *, int, void *, int, const char *);
E char *coyotename(struct monst *,char *);


/* ### do_wear.c ### */

E int Armor_on(void);
E int Boots_on(void);
E int Cloak_on(void);
E int Helmet_on(void);
E int Gloves_on(void);
E int Shield_on(void);
E int Shirt_on(void);
E void Amulet_on(void);
E void Implant_on(void);
#ifdef USE_TRAMPOLI
E int select_off(struct obj *);
E int take_off(void);
#endif
E void off_msg(struct obj *);
E void set_wear(void);
E boolean donning(struct obj *);
E void cancel_don(void);
E int Armor_off(void);
E int Armor_gone(void);
E int Helmet_off(void);
E int Gloves_off(void);
E int Boots_off(void);
E int Cloak_off(void);
E int Shield_off(void);
E int Shirt_off(void);
E void Amulet_off(void);
E void Implant_off(void);
E void Ring_on(struct obj *);
E void Ring_off(struct obj *);
E void Ring_gone(struct obj *);
E void Blindf_on(struct obj *);
E void Blindf_off(struct obj *);
E int dotakeoff(void);
E int doremring(void);
E int cursed(struct obj *);
E int armoroff(struct obj *);
E int canwearobj(struct obj *, long *, BOOLEAN_P);
E int dowear(void);
E int doputon(void);
E void find_ac(void);
E void glibr(void);
E struct obj *some_armor(struct monst *);
E void erode_armor(struct monst *,BOOLEAN_P);
E struct obj *stuck_ring(struct obj *,int);
E struct obj *unchanger(void);
E void reset_remarm(void);
E int doddoremarm(void);
E int destroy_arm(struct obj *);
E int dowear2(const char *, const char *);

/* ### dog.c ### */

E void initedog(struct monst *);
E struct monst *make_familiar(struct obj *,XCHAR_P,XCHAR_P,BOOLEAN_P,BOOLEAN_P);
E struct monst *make_helper(int,XCHAR_P,XCHAR_P);
E struct monst *makedog(void);
E void update_mlstmv(void);
E void losedogs(void);
E void mon_arrive(struct monst *,BOOLEAN_P);
E void mon_catchup_elapsed_time(struct monst *,long);
E void keepdogs(BOOLEAN_P);
E void migrate_to_level(struct monst *,XCHAR_P,XCHAR_P,coord *);
E int dogfood(struct monst *,struct obj *);
E struct monst *tamedog(struct monst *,struct obj *, BOOLEAN_P);
E int make_pet_minion(int,ALIGNTYP_P);
E void abuse_dog(struct monst *);
E void wary_dog(struct monst *, BOOLEAN_P);
E void makedoghungry(struct monst *, int);
E boolean befriend_with_obj(struct permonst *, struct obj *);
E boolean control_magic_works(void);

/* ### dogmove.c ### */

E int dog_nutrition(struct monst *,struct obj *);
E int dog_eat(struct monst *,struct obj *,int,int,BOOLEAN_P);
E int dog_move(struct monst *,int);
E boolean betrayed(struct monst *);
#ifdef USE_TRAMPOLI
E void wantdoor(int,int,void *);
#endif

/* ### dokick.c ### */

E boolean ghitm(struct monst *,struct obj *);
E void container_impact_dmg(struct obj *);
E int dokick(void);
E boolean ship_object(struct obj *,XCHAR_P,XCHAR_P,BOOLEAN_P);
E void obj_delivery(void);
E schar down_gate(XCHAR_P,XCHAR_P);
E void impact_drop(struct obj *,XCHAR_P,XCHAR_P,XCHAR_P);

/* ### dothrow.c ### */

E struct obj *splitoneoff(struct obj **);
E int dothrow(void);
E int dofire(void);
E void hitfloor(struct obj *);
E void hurtle(int,int,int,BOOLEAN_P);
E void mhurtle(struct monst *,int,int,int);
E void throwit(struct obj *,long,BOOLEAN_P,int);
E int omon_adj(struct monst *,struct obj *,BOOLEAN_P);
E int thitmonst(struct monst *,struct obj *, int, BOOLEAN_P);
E int hero_breaks(struct obj *,XCHAR_P,XCHAR_P,BOOLEAN_P);
E int breaks(struct obj *,XCHAR_P,XCHAR_P);
E boolean breaktest(struct obj *);
E boolean walk_path(coord *, coord *, boolean (*)(void *,int,int), void *);
E boolean hurtle_step(void *, int, int);

/* ### drawing.c ### */
#endif /* !MAKEDEFS_C && !LEV_LEX_C */
E int def_char_to_objclass(CHAR_P);
E int def_char_to_monclass(CHAR_P);
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
E void assign_graphics(uchar *,int,int,int);
E void assignwinggraphics();
E void switch_graphics(int);
#ifdef REINCARNATION
E void assign_rogue_graphics(BOOLEAN_P);
#endif

/* ### dungeon.c ### */

E xchar dname_to_dnum(const char *);
E void save_dungeon(int,BOOLEAN_P,BOOLEAN_P);
E void restore_dungeon(int);
E void insert_branch(branch *,BOOLEAN_P);
E void init_dungeons(void);
E s_level *find_level(const char *);
E s_level *Is_special(d_level *);
E branch *Is_branchlev(d_level *);
E xchar ledger_no(d_level *);
E xchar maxledgerno(void);
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
E void u_on_sstairs(void);
E void u_on_upstairs(void);
E void u_on_dnstairs(void);
E boolean On_stairs(XCHAR_P,XCHAR_P);
E void get_level(d_level *,int);
E boolean Is_botlevel(d_level *);
E boolean Can_fall_thru(d_level *);
E boolean Can_dig_down(d_level *);
E boolean Can_rise_up(int,int,d_level *);
E boolean In_quest(d_level *);
E boolean In_mines(d_level *);
E boolean In_sheol(d_level *);
E boolean In_yendorian(d_level *);
E boolean In_forging(d_level *);
E boolean In_ordered(d_level *);
E boolean In_deadground(d_level *);
E boolean In_illusorycastle(d_level *);
E boolean In_voiddungeon(d_level *);
E boolean In_netherrealm(d_level *);
E boolean In_deepmines(d_level *);
E boolean In_angmar(d_level *);
E boolean In_greencross(d_level *);
E boolean In_emynluin(d_level *);
E boolean In_minotaurmaze(d_level *);
E boolean In_swimmingpool(d_level *);
E boolean In_hellbathroom(d_level *);
E boolean In_minusworld(d_level *);
E boolean In_spacebase(d_level *);
E boolean In_sewerplant(d_level *);
E boolean In_gammacaves(d_level *);
E boolean In_mainframe(d_level *);
E boolean In_poolchallenge(d_level *);
E boolean In_joustchallenge(d_level *);
E boolean In_digdugchallenge(d_level *);
E boolean In_gruechallenge(d_level *);
E boolean In_pacmanchallenge(d_level *);
E boolean In_subquest(d_level *);
E boolean In_rivalquest(d_level *);
E boolean In_bellcaves(d_level *);
E boolean In_dod(d_level *);
E boolean In_gehennom(d_level *);
E boolean In_towndungeon(d_level *);
E boolean In_spiders(d_level *);
E boolean In_grund(d_level *);
E boolean In_icequeen(d_level *);
E boolean In_wyrm(d_level *);
E boolean In_tomb(d_level *);
E boolean In_slsea(d_level *);
E boolean In_gcavern(d_level *);
E boolean In_frnkn(d_level *);
E boolean In_mtemple(d_level *);
E boolean In_restingzone(d_level *);
E boolean playerlevelportdisabled(void);
E branch *dungeon_branch(const char *);
E boolean at_dgn_entrance(const char *);
E boolean In_hell(d_level *);
E boolean In_ZAPM(d_level *);
E boolean In_Devnull(d_level *);
E boolean In_lategame(d_level *);
E boolean In_V_tower(d_level *);
E boolean On_W_tower_level(d_level *);
E boolean In_W_tower(int,int,d_level *);
E void find_hell(d_level *);
E void goto_hell(BOOLEAN_P,BOOLEAN_P);
E void assign_level(d_level *,d_level *);
E void assign_rnd_level(d_level *,d_level *,int);
E int induced_align(int);
E boolean Invocation_lev(d_level *);
E xchar level_difficulty(void);
E xchar monster_difficulty(void);
E schar lev_by_name(const char *);
#ifdef WIZARD
E schar print_dungeon(BOOLEAN_P,schar *,xchar *);
#endif
E int donamelevel(void);
E int dooverview(void);
E void forget_mapseen(int);
E void init_mapseen(d_level *);

/* ### eat.c ### */

#ifdef USE_TRAMPOLI
E int eatmdone(void);
E int eatfood(void);
E int opentin(void);
E int unfaint(void);
#endif
E boolean is_edible(struct obj *);
E void init_uhunger(void);
E int Hear_again(void);
E void reset_eat(void);
E int doeat(void);
E void gethungry(void);
E void morehungry(int);
E void lesshungry(int);
E boolean is_fainted(void);
E void reset_faint(void);
E void violated_vegetarian(void);
#if 0
E void sync_hunger(void);
#endif
E void newuhs(BOOLEAN_P);
E boolean can_reach_floorobj(void);
E void vomit(void);
E int eaten_stat(int,struct obj *);
E void food_disappears(struct obj *);
E void food_substitution(struct obj *,struct obj *);
E boolean bite_monster(struct monst *mon);
E void fix_petrification(void);
E void consume_oeaten(struct obj *,int);
E boolean maybe_finished_meal(BOOLEAN_P);
E void energysap(struct obj *);

/* ### end.c ### */

E void done1(int);
E int done2(void);
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
E int dolistvanq(void);
E int num_genocides(void);
E void mk_dgl_extrainfo(void);
/* KMH, ethics */
E int doethics(void);
E char *dump_format_str(char *);


/* ### engrave.c ### */

E char *random_engraving(char *);
E void wipeout_text(char *,int,unsigned);
E boolean can_reach_floor(void);
E const char *surface(int,int);
E const char *ceiling(int,int);
E struct engr *engr_at(XCHAR_P,XCHAR_P);
E int sengr_at(const char *,XCHAR_P,XCHAR_P);
E void u_wipe_engr(int);
E void wipe_engr_at(XCHAR_P,XCHAR_P,XCHAR_P);
E boolean sense_engr_at(int,int,BOOLEAN_P);
E void make_engr_at(int,int,const char *,long,XCHAR_P);
E void del_engr_at(int,int);
E int freehand(void);
E int freehandX(void);
E int doengrave(void);
E void save_engravings(int,int);
E void rest_engravings(int);
E void del_engr(struct engr *);
E void rloc_engr(struct engr *);
E void make_grave(int,int,const char *);
E const char *random_mesgX(void);
E const char *random_epitaph(void);

/* ### exper.c ### */

E long newuexp(int);
E int experience(struct monst *,int);
E void more_experienced(int,int);
E void losexp(const char *, BOOLEAN_P, BOOLEAN_P);
E void newexplevel(void);
E void pluslvl(BOOLEAN_P);
E long rndexp(BOOLEAN_P);
E void exprecalc(void);
E void gainlevelmaybe(void);

/* ### explode.c ### */

E void explode(int,int,int,int,CHAR_P,int);
E long scatter(int, int, int, unsigned int, struct obj *);
E void splatter_burning_oil(int, int);
E void grenade_explode(struct obj *, int, int, BOOLEAN_P, int);
E void arm_bomb(struct obj *, BOOLEAN_P);

/* ### extralev.c ### */

#ifdef REINCARNATION
E void makeroguerooms(void);
E void corr(int,int);
E void makerogueghost(void);
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
E void set_lock_and_bones(void);
#endif
E void set_levelfile_name(char *,int);
E int create_levelfile(int,char *);
E int open_levelfile(int,char *);
E void delete_levelfile(int);
E void clearlocks(void);
E int create_bonesfile(d_level*,char **, char *);
#ifdef MFLOPPY
E void cancel_bonesfile(void);
#endif
E void commit_bonesfile(d_level *);
E int open_bonesfile(d_level*,char **);
E int delete_bonesfile(d_level*);
E void compress_bonesfile(void);
E void set_savefile_name(void);
#ifdef INSURANCE
E void save_savefile_name(int);
#endif
#if defined(WIZARD) && !defined(MICRO)
E void set_error_savefile(void);
#endif
E int create_savefile(void);
E int open_savefile(void);
E int delete_savefile(void);
E int restore_saved_game(void);
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
E void read_wizkit(void);
#endif
E void paniclog(const char *, const char *);
E int validate_prefix_locations(char *);
E char** get_saved_games(void);
E void free_saved_games(char**);
#ifdef SELF_RECOVER
E boolean recover_savefile(void);
#endif
#ifdef HOLD_LOCKFILE_OPEN
E void really_close(void);
#endif
#ifdef WHEREIS_FILE
E void touch_whereis(void);
E void delete_whereis(void);
#endif

/* ### fountain.c ### */

E void floating_above(const char *);
E void dogushforth(int);
# ifdef USE_TRAMPOLI
E void gush(int,int,void *);
# endif
E void dryup(XCHAR_P,XCHAR_P, BOOLEAN_P);
E void drinkfountain(void);
E void dipfountain(struct obj *);
E void whetstone_fountain_effects(struct obj *);
E void diptoilet(struct obj *);
E void breaksink(int,int);
E void breaktoilet(int,int);
E void drinksink(void);
E void drinktoilet(void);
E void whetstone_sink_effects(struct obj *);
E void whetstone_toilet_effects(struct obj *);

/* ### gypsy.c ### */

E void gypsy_init(struct monst *);
E void gypsy_chat(struct monst *);
E boolean play_blackjack(void);
E int blackjack_card(void);

/* ### hack.c ### */

E void maybe_wail(void);

#ifdef DUNGEON_GROWTH
E void catchup_dgn_growths(int);
E void dgn_growths(BOOLEAN_P,BOOLEAN_P,BOOLEAN_P);
#endif
E boolean revive_nasty(int,int,const char*);
E void movobj(struct obj *,XCHAR_P,XCHAR_P);
E boolean may_dig(XCHAR_P,XCHAR_P);
E boolean may_passwall(XCHAR_P,XCHAR_P);
E boolean bad_rock(struct monst *,XCHAR_P,XCHAR_P);
E boolean invocation_pos(XCHAR_P,XCHAR_P);
E boolean test_move(int, int, int, int, int);
E void domove(void);
E void invocation_message(void);
E void wounds_message(struct monst *); 
E char *mon_wounds(struct monst *); 
E void spoteffects(BOOLEAN_P);
E char *in_rooms(XCHAR_P,XCHAR_P,int);
E char *in_roomscolouur(XCHAR_P,XCHAR_P,int);
E boolean in_town(int,int);
E void check_special_room(BOOLEAN_P);
E int dopickup(void);
E void lookaround(void);
E int monster_nearby(void);
E void nomul(int, const char *, BOOLEAN_P);
E void forcenomul(int, const char *);
E void unmul(const char *);
#ifdef SHOW_DMG
E void showdmg(int);
#endif
E void losehp(int,const char *, int);
E int weight_cap(void);
E int inv_weight(void);
E int near_capacity(void);
E int calc_capacity(int);
E int max_capacity(void);
E boolean check_capacity(const char *);
E int inv_cnt(void);
E void contaminate(int, BOOLEAN_P);
E void decontaminate(int);
E void reducesanity(int);
#ifdef GOLDOBJ
E long money_cnt(struct obj *);
#endif
E boolean zapmrename(void);
E int max_carr_cap(void);

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
E void setrandom(void);
E int getyear(void);
E int getmonth(void);	/* KMH -- Used by gypsies */
#if 0
E char *yymmdd(time_t);
#endif
E long yyyymmdd(time_t);
E struct tm *getlt();
E int phase_of_the_moon(void);
E boolean friday_13th(void);
E boolean groundhog_day(void);	/* KMH -- February 2 */
E int night(void);
E int midnight(void);

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
E struct obj *carryingappearance(int);
E boolean have_lizard(void);
E boolean have_loadstone(void);
E boolean have_pokeloadstone(void);
E boolean have_whitewaterrafting(void);
E boolean have_daisybag(void);
E boolean have_invisoloadstone(void);
E boolean have_spectrumplinggem(void);
E boolean have_amateurluckstone(void);
E boolean have_femityjewel(void);
E boolean have_sleepstone(void);
E boolean have_magicresstone(void);
E boolean have_cursedmagicresstone(void);
E boolean have_loadboulder(void);
E boolean have_starlightstone(void);
E boolean sjwcheck(int);
E boolean feminizecheck(int);
E int itemactions(struct obj *,BOOLEAN_P);
E boolean have_hungerhealer(void);

E int dostackmark(void);

E int numberofetheritems(void);
E int numberofwornetheritems(void);

E boolean stack_too_big(struct obj *);

E boolean have_rmbstone(void);
E boolean have_displaystone(void);
E boolean have_yellowspellstone(void);
E boolean have_spelllossstone(void);
E boolean have_autodestructstone(void);
E boolean have_memorylossstone(void);
E boolean have_inventorylossstone(void);
E boolean have_blackystone(void);
E boolean have_menubugstone(void);
E boolean have_speedbugstone(void);
E boolean have_superscrollerstone(void);
E boolean have_freehandbugstone(void);
E boolean have_unidentifystone(void);
E boolean have_thirststone(void);
E boolean have_unluckystone(void);
E boolean have_shadesofgreystone(void);
E boolean have_faintingstone(void);
E boolean have_cursingstone(void);
E boolean have_difficultystone(void);
E boolean have_deafnessstone(void);
E boolean have_weaknessstone(void);
E boolean have_antimagicstone(void);
E boolean have_rotthirteenstone(void);
E boolean have_bishopstone(void);
E boolean have_confusionstone(void);
E boolean have_dropbugstone(void);
E boolean have_dstwstone(void);
E boolean have_statusstone(void);
E boolean have_alignmentstone(void);
E boolean have_stairstrapstone(void);
E boolean have_uninformationstone(void);

E boolean have_amnesiastone(void);
E boolean have_bigscriptstone(void);
E boolean have_bankstone(void);
E boolean have_mapstone(void);
E boolean have_techniquestone(void);
E boolean have_disenchantmentstone(void);
E boolean have_verisiertstone(void);
E boolean have_chaosterrainstone(void);
E boolean have_mutenessstone(void);
E boolean have_engravingstone(void);
E boolean have_magicdevicestone(void);
E boolean have_bookstone(void);
E boolean have_levelstone(void);
E boolean have_quizstone(void);

E boolean have_intrinsiclossstone(void);
E boolean have_bloodlossstone(void);
E boolean have_badeffectstone(void);
E boolean have_trapcreationstone(void);
E boolean have_vulnerabilitystone(void);
E boolean have_itemportstone(void);
E boolean have_nastystone(void);

E boolean have_respawnstone(void);
E boolean have_captchastone(void);
E boolean have_farlookstone(void);

E boolean have_nonsacredstone(void);
E boolean have_starvationstone(void);
E boolean have_droplessstone(void);
E boolean have_loweffectstone(void);
E boolean have_invisostone(void);
E boolean have_ghostlystone(void);
E boolean have_dehydratingstone(void);
E boolean have_hatestone(void);
E boolean have_directionswapstone(void);
E boolean have_nonintrinsicstone(void);
E boolean have_dropcursestone(void);
E boolean have_nakedstone(void);
E boolean have_antilevelstone(void);
E boolean have_stealerstone(void);
E boolean have_rebelstone(void);
E boolean have_shitstone(void);
E boolean have_misfirestone(void);
E boolean have_wallstone(void);

E boolean have_metabolicstone(void);
E boolean have_noreturnstone(void);
E boolean have_egostone(void);
E boolean have_fastforwardstone(void);
E boolean have_rottenstone(void);
E boolean have_unskilledstone(void);
E boolean have_lowstatstone(void);
E boolean have_trainingstone(void);
E boolean have_exercisestone(void);

E boolean have_limitationstone(void);
E boolean have_weaksightstone(void);
E boolean have_messagestone(void);

E boolean have_disconnectstone(void);
E boolean have_interfacescrewstone(void);
E boolean have_bossfightstone(void);
E boolean have_entirelevelstone(void);
E boolean have_bonestone(void);
E boolean have_autocursestone(void);
E boolean have_highlevelstone(void);
E boolean have_spellforgettingstone(void);
E boolean have_soundeffectstone(void);
E boolean have_timerunstone(void);

E boolean have_sanitytreblestone(void);
E boolean have_statdecreasestone(void);
E boolean have_simeoutstone(void);

E boolean have_orangespellstone(void);
E boolean have_violetspellstone(void);
E boolean have_longingstone(void);
E boolean have_cursedpartstone(void);
E boolean have_quaversalstone(void);
E boolean have_appearanceshufflingstone(void);
E boolean have_brownspellstone(void);
E boolean have_choicelessstone(void);
E boolean have_goldspellstone(void);
E boolean have_deprovementstone(void);
E boolean have_initializationstone(void);
E boolean have_gushlushstone(void);
E boolean have_soiltypestone(void);
E boolean have_dangerousterrainstone(void);
E boolean have_falloutstone(void);
E boolean have_mojibakestone(void);
E boolean have_gravationstone(void);
E boolean have_uncalledstone(void);
E boolean have_explodingdicestone(void);
E boolean have_permacursestone(void);
E boolean have_shroudedidentitystone(void);
E boolean have_feelergaugesstone(void);
E boolean have_longscrewupstone(void);
E boolean have_wingyellowstone(void);
E boolean have_lifesavingstone(void);
E boolean have_curseusestone(void);
E boolean have_cutnutritionstone(void);
E boolean have_skilllossstone(void);
E boolean have_autopilotstone(void);
E boolean have_forcestone(void);
E boolean have_monsterglyphstone(void);
E boolean have_changingdirectivestone(void);
E boolean have_containerkaboomstone(void);
E boolean have_stealdegradestone(void);
E boolean have_leftinventorystone(void);
E boolean have_fluctuatingspeedstone(void);
E boolean have_tarmustrokingnorastone(void);
E boolean have_failurestone(void);
E boolean have_brightcyanspellstone(void);
E boolean have_frequentationspawnstone(void);
E boolean have_petaistone(void);
E boolean have_satanstone(void);
E boolean have_rememberancestone(void);
E boolean have_pokeliestone(void);
E boolean have_autopickupstone(void);
E boolean have_dywypistone(void);
E boolean have_silverspellstone(void);
E boolean have_metalspellstone(void);
E boolean have_platinumspellstone(void);
E boolean have_manlerstone(void);
E boolean have_doorningstone(void);
E boolean have_nownsiblestone(void);
E boolean have_elmstreetstone(void);
E boolean have_monnoisestone(void);
E boolean have_rangcallstone(void);
E boolean have_recurringspelllossstone(void);
E boolean have_antitrainingstone(void);
E boolean have_techoutstone(void);
E boolean have_statdecaystone(void);
E boolean have_movemorkstone(void);

E boolean have_lootcutstone(void);
E boolean have_monsterspeedstone(void);
E boolean have_scalingstone(void);
E boolean have_inimicalstone(void);
E boolean have_whitespellstone(void);
E boolean have_greyoutstone(void);
E boolean have_quasarstone(void);
E boolean have_mommystone(void);
E boolean have_horrorstone(void);
E boolean have_artificialstone(void);
E boolean have_wereformstone(void);
E boolean have_antiprayerstone(void);
E boolean have_evilpatchstone(void);
E boolean have_hardmodestone(void);
E boolean have_secretattackstone(void);
E boolean have_eaterstone(void);
E boolean have_covetousstone(void);
E boolean have_nonseeingstone(void);
E boolean have_darkmodestone(void);
E boolean have_unfindablestone(void);
E boolean have_homicidestone(void);
E boolean have_multitrappingstone(void);
E boolean have_wakeupcallstone(void);
E boolean have_grayoutstone(void);
E boolean have_graycenterstone(void);
E boolean have_checkerboardstone(void);
E boolean have_clockwisestone(void);
E boolean have_counterclockwisestone(void);
E boolean have_lagstone(void);
E boolean have_blesscursestone(void);
E boolean have_delightstone(void);
E boolean have_dischargestone(void);
E boolean have_trashstone(void);
E boolean have_filteringstone(void);
E boolean have_deformattingstone(void);
E boolean have_flickerstripstone(void);
E boolean have_undressingstone(void);
E boolean have_hyperbluestone(void);
E boolean have_nolightstone(void);
E boolean have_paranoiastone(void);
E boolean have_fleecestone(void);
E boolean have_interruptionstone(void);
E boolean have_dustbinstone(void);
E boolean have_batterystone(void);
E boolean have_butterfingerstone(void);
E boolean have_miscastingstone(void);
E boolean have_messagesuppressionstone(void);
E boolean have_stuckannouncementstone(void);
E boolean have_stormstone(void);
E boolean have_maximumdamagestone(void);
E boolean have_latencystone(void);
E boolean have_starlitskystone(void);
E boolean have_trapknowledgestone(void);
E boolean have_highscorestone(void);
E boolean have_pinkspellstone(void);
E boolean have_greenspellstone(void);
E boolean have_evcstone(void);
E boolean have_underlaidstone(void);
E boolean have_damagemeterstone(void);
E boolean have_weightstone(void);
E boolean have_infofuckstone(void);
E boolean have_blackspellstone(void);
E boolean have_cyanspellstone(void);
E boolean have_heapstone(void);
E boolean have_bluespellstone(void);
E boolean have_tronstone(void);
E boolean have_redspellstone(void);
E boolean have_tooheavystone(void);
E boolean have_elongatedstone(void);
E boolean have_wrapoverstone(void);
E boolean have_destructionstone(void);
E boolean have_meleeprefixstone(void);
E boolean have_automorestone(void);
E boolean have_unfairattackstone(void);

E boolean have_badpartstone(void);
E boolean have_completelybadpartstone(void);
E boolean have_evilvariantstone(void);

E boolean have_femtrapfemmy(void);
E boolean have_femtrapmadeleine(void);
E boolean have_femtrapmarlena(void);
E boolean have_femtrapanastasia(void);
E boolean have_femtrapjessica(void);
E boolean have_femtrapsolvejg(void);
E boolean have_femtrapwendy(void);
E boolean have_femtrapkatharina(void);
E boolean have_femtrapelena(void);
E boolean have_femtrapthai(void);
E boolean have_femtrapelif(void);
E boolean have_femtrapnadja(void);
E boolean have_femtrapsandra(void);
E boolean have_femtrapnatalje(void);
E boolean have_femtrapjeanetta(void);
E boolean have_femtrapyvonne(void);
E boolean have_femtrapmaurah(void);
E boolean have_femtrapmeltem(void);
E boolean have_femtrapsarah(void);
E boolean have_femtrapclaudia(void);
E boolean have_femtrapludgera(void);
E boolean have_femtrapkati(void);
E boolean have_femtrapnelly(void);
E boolean have_femtrapeveline(void);
E boolean have_femtrapkarin(void);
E boolean have_femtrapjuen(void);
E boolean have_femtrapkristina(void);
E boolean have_femtraplou(void);
E boolean have_femtrapalmut(void);
E boolean have_femtrapjulietta(void);
E boolean have_femtraparabella(void);
E boolean have_femtrapkristin(void);
E boolean have_femtrapanna(void);
E boolean have_femtrapruea(void);
E boolean have_femtrapdora(void);
E boolean have_femtrapmarike(void);
E boolean have_femtrapjette(void);
E boolean have_femtrapina(void);
E boolean have_femtrapsing(void);
E boolean have_femtrapvictoria(void);
E boolean have_femtrapmelissa(void);
E boolean have_femtrapanita(void);
E boolean have_femtraphenrietta(void);
E boolean have_femtrapverena(void);

E boolean have_primecurse(void);

E boolean have_morgothiancurse(void);
E boolean have_topiylinencurse(void);
E boolean have_blackbreathcurse(void);

E boolean have_mothrelay(void);

E boolean usingquestarti(void);

E int foreignartifactcount(void);

E struct obj *o_on(unsigned int,struct obj *);
E boolean obj_here(struct obj *,int,int);
E boolean wearing_armor(void);
E boolean is_worn(struct obj *);
E struct obj *g_at(int,int);
E struct obj *mkgoldobj(long);
E struct obj *getobj(const char *,const char *);
E int ggetobj(const char *,int (*)(OBJ_P),int,BOOLEAN_P,unsigned *);
E void fully_identify_obj(struct obj *);
E void maybe_fully_identify_obj(struct obj *,BOOLEAN_P);
E int identify(struct obj *);
E int identifyless(struct obj *,BOOLEAN_P);
E void identify_pack(int, BOOLEAN_P, BOOLEAN_P);
E int askchain(struct obj **,const char *,int,int (*)(OBJ_P), int (*)(OBJ_P),int,const char *);
E void prinv(const char *,struct obj *,long);
E char *xprname(struct obj *,const char *,CHAR_P,BOOLEAN_P,long,long);
E int ddoinv(void);
E char display_inventory(const char *,BOOLEAN_P);
#ifdef DUMP_LOG
E char dump_inventory(const char *,BOOLEAN_P);
#endif
E int display_binventory(int,int,BOOLEAN_P);
E struct obj *display_cinventory(struct obj *);
E struct obj *display_minventory(struct monst *,int,char *);
E int dotypeinv(void);
E const char *dfeature_at(int,int,char *);
E int look_here(int,BOOLEAN_P);
E int dolook(void);
E boolean will_feel_cockatrice(struct obj *,BOOLEAN_P);
E void feel_cockatrice(struct obj *,BOOLEAN_P);
E void stackobj(struct obj *);
E int doprgold(void);
E int doprwep(void);
E int doprarm(void);
E int doprring(void);
E int dopramulet(void);
E int doprtool(void);
E int doprinuse(void);
E void useupf(struct obj *,long);
E char *let_to_name(CHAR_P,BOOLEAN_P,BOOLEAN_P);
E void free_invbuf(void);
E void reassign(void);
E int doorganize(void);
E int domarkforpet(void);
E int count_unpaid(struct obj *);
E int count_notfullyided(struct obj *);
E int count_buc(struct obj *,int);
E void carry_obj_effects(struct monst *, struct obj *);
E const char *currency(long);
E void silly_thing(const char *,struct obj *);
E int doinvinuse(void);
/* KMH, balance patch -- new function */
E int jumble_pack(void);

/* ### ioctl.c ### */

#if defined(UNIX) || defined(__BEOS__)
E void getwindowsz(void);
E void getioctls(void);
E void setioctls(void);
# ifdef SUSPEND
E int dosuspend(void);
# endif /* SUSPEND */
#endif /* UNIX || __BEOS__ */

/* ### light.c ### */

E void new_light_source(XCHAR_P, XCHAR_P, int, int, void *);
E void del_light_source(int, void *);
E void do_light_sources(char **);
E struct monst *find_mid(unsigned, unsigned);
E void save_light_sources(int, int, int);
E void restore_light_sources(int);
E void relink_light_sources(BOOLEAN_P);
E void obj_move_light_source(struct obj *, struct obj *);
E boolean any_light_source(void);
E void snuff_light_source(int, int);
E boolean obj_sheds_light(struct obj *);
E boolean obj_is_burning(struct obj *);
E boolean obj_permanent_light(struct obj *);
E void obj_split_light_source(struct obj *, struct obj *);
E void obj_merge_light_sources(struct obj *,struct obj *);
E int candle_light_range(struct obj *);
#ifdef WIZARD
E int wiz_light_sources(void);
#endif

/* ### lock.c ### */

#ifdef USE_TRAMPOLI
E int forcelock(void);
E int picklock(void);
#endif
E boolean picking_lock(int *,int *);
E boolean picking_at(int,int);
E void reset_pick(void);
E int pick_lock(struct obj **);
E int doforce(void);
E boolean boxlock(struct obj *,struct obj *);
E boolean doorlock(struct obj *,int,int);
E boolean doorlockX(int,int,BOOLEAN_P);
E int doopen(void);
E int doclose(void);
E int artifact_door(int,int);
E int doopen_indir(int,int);

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
E void getlock(void);

/* ### macwin.c ### */

E void lock_mouse_cursor(Boolean);
E int SanePositions(void);

/* ### mttymain.c ### */

E void getreturn(const char *);
E void msmsg(const char *,...);
E void gettty(void);
E void setftty(void);
E void settty(const char *);
E int tgetch(void);
E void cmov(int x, int y);
E void nocmov(int x, int y);

#endif /* MAC */

/* ### mail.c ### */

#ifdef MAIL
# ifdef UNIX
E void getmailstatus(void);
# endif
E void ckmailstatus(void);
E void readmail(struct obj *);
#endif /* MAIL */

/* ### makemon.c ### */

E boolean is_home_elemental(struct permonst *);
E boolean monster_with_trait(struct permonst *, int);
E struct monst *clone_mon(struct monst *,XCHAR_P,XCHAR_P);
E struct monst *makemon(struct permonst *,int,int,int);
E boolean create_critters(int,struct permonst *);
E struct permonst *rndmonst(void);
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
E void u_slow_down(void);
E struct monst *cloneu(void);
E void expels(struct monst *,struct permonst *,BOOLEAN_P);
E struct attack *getmattk(struct permonst *,int,int *,struct attack *);
E int mattacku(struct monst *);
E int magic_negation(struct monst *);
E int gazemu(struct monst *,struct attack *);
E void mdamageu(struct monst *,int);
E int could_seduce(struct monst *,struct monst *,struct attack *);
E int could_seduceX(struct monst *,struct monst *,struct attack *);
E int doseduce(struct monst *);
E void hurtarmor(int);
E void witherarmor(void);

/* ### minion.c ### */

E void msummon(struct monst *, BOOLEAN_P);
E void summon_minion(ALIGNTYP_P,BOOLEAN_P);
E int demon_talk(struct monst *);
E int lawful_minion(int);
E int neutral_minion(int);
E int chaotic_minion(int);
E long bribe(struct monst *);
E int dprince(ALIGNTYP_P);
E int dlord(ALIGNTYP_P);
E int llord(void);
E int ndemon(ALIGNTYP_P);
E int lminion(void);
E int ntrminion(void);

/* ### mklev.c ### */

#ifdef USE_TRAMPOLI
E int do_comp(void *,void *);
#endif
E void sort_rooms(void);
E void add_room(int,int,int,int,BOOLEAN_P,SCHAR_P,BOOLEAN_P,BOOLEAN_P,int);
E void add_subroom(struct mkroom *,int,int,int,int, BOOLEAN_P,SCHAR_P,BOOLEAN_P);
E void makecorridors(void);
E int randomwalltype(void);
E int walkableterrain(void);
E int add_door(int,int,struct mkroom *);
E void mklev(void);
#ifdef SPECIALIZATION
E void topologize(struct mkroom *,BOOLEAN_P);
#else
E void topologize(struct mkroom *);
#endif
E void place_branch(branch *,XCHAR_P,XCHAR_P);
E boolean occupied(XCHAR_P,XCHAR_P);
E boolean reallyoccupied(XCHAR_P,XCHAR_P);
E int okdoor(XCHAR_P,XCHAR_P);
E void dodoor(int,int,struct mkroom *);
E void mktrap(int,int,struct mkroom *,coord*,BOOLEAN_P);
E void mkstairs(XCHAR_P,XCHAR_P,CHAR_P,struct mkroom *);
E void mkinvokearea(void);
E void mkrivers(void);
E void mkrandrivers(void);
E void specdungeoninit(void);
E void mk_knox_portal(XCHAR_P,XCHAR_P);

/* ### mkmap.c ### */

void flood_fill_rm(int,int,int,BOOLEAN_P,BOOLEAN_P);
void remove_rooms(int,int,int,int);

/* ### mkmaze.c ### */

E void wallification(int,int,int,int, BOOLEAN_P);
E void walkfrom(int,int);
E void makemaz(const char *);
E void mazexy(coord *);
E void mazexy_all(coord *);
E void bound_digging(void);
E void mkportal(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P);
E boolean bad_location(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P);
E boolean really_bad_location(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P);
E void place_lregion(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P, XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P, XCHAR_P,d_level *);
E void movebubbles(void);
E void water_friction(void);
E void save_waterlevel(int,int);
E void restore_waterlevel(int);
E const char *waterbody_name(XCHAR_P,XCHAR_P);

/* ### mkobj.c ### */

E struct obj *mkobj_at(CHAR_P,int,int,int,BOOLEAN_P);
E struct obj *mksobj_at(int,int,int,BOOLEAN_P,int,BOOLEAN_P);
E struct obj *mkobj(CHAR_P,int,BOOLEAN_P);
E int rndmonnum(void);
E int usefulitem(void);
E int nastymusableitem(void);
E int makegreatitem(void);
E int makemusableitem(void);
E struct obj *splitobj(struct obj *,long);
E void replace_object(struct obj *,struct obj *);
E void bill_dummy_object(struct obj *);
E struct obj *mksobj(int,BOOLEAN_P,int,BOOLEAN_P);
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
E void uncurse(struct obj *, BOOLEAN_P);
E void blessorcurse(struct obj *,int);
E void blessorcurse_on_creation(struct obj *,int);
E boolean is_rustprone(struct obj *);
E boolean is_corrodeable(struct obj *);
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
E void obj_sanity_check(void);

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
E void mkundead(coord *,BOOLEAN_P,int,BOOLEAN_P);
/*E void mkundeadboo(coord *,BOOLEAN_P,int);*/
E void mkundeadX(coord *,BOOLEAN_P,int);
E struct permonst *courtmon(void);
E struct permonst *insidemon(void);
E struct permonst *antholemon(void);
E struct permonst *realzoomon(void);
E struct permonst *illusionmon(void);
E struct permonst *beehivemon(void);
E void save_rooms(int);
E void rest_rooms(int);
E struct mkroom *search_special(SCHAR_P);
E struct permonst * colormon(int);
E struct permonst * specialtensmon(int);
E struct permonst * speedymon(void);
E void mkroommateroom(int);

/* ### mon.c ### */

E int undead_to_corpse(int);
E int genus(int,int);
E int pm_to_cham(int);
E int minliquid(struct monst *);
E int movemon(void);
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
E void dmonsfree(void);
E int mcalcmove(struct monst*);
E void mcalcdistress(void);
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
E void wake_nearby(void);
E void wake_nearto(int,int,int);
E void seemimic(struct monst *);
E void rescham(void);
E void restartcham(void);
E void restore_cham(struct monst *);
E void mon_animal_list(BOOLEAN_P);
E int newcham(struct monst *,struct permonst *,BOOLEAN_P,BOOLEAN_P);
E int can_be_hatched(int);
E int egg_type_from_parent(int,BOOLEAN_P);
E boolean dead_species(int,BOOLEAN_P);
E void kill_genocided_monsters(void);
E void golemeffects(struct monst *,int,int);
E boolean angry_guards(BOOLEAN_P);
E void pacify_guards(void);
E boolean damage_mon(struct monst*,int,int); /* sporkhack */
E void turnmonintosymbiote(struct monst *, BOOLEAN_P);
E void killsymbiote(void);
E void uncursesymbiote(BOOLEAN_P);
E void cursesymbiote(void);
E boolean symbiotemelee(void);
E boolean symbiotepassive(void);
E void getrandomsymbiote(BOOLEAN_P,BOOLEAN_P);
E void monst_globals_init(void);

/* ### mondata.c ### */

E void set_mon_data(struct monst *,struct permonst *,int);
E struct attack *attacktype_fordmg(struct permonst *,int,int);
E boolean attacktype(struct permonst *,int);
E struct attack *attdmgtype_fromattack(struct permonst *,int,int);
E boolean attackdamagetype(struct permonst *,int,int);
E boolean haspassive(struct permonst *);
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
E boolean elongation_monster(struct permonst *);
E boolean immune_timestop(struct permonst *);
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

/* ### monstr.c ### */

E void monstr_init(void);

/* ### mplayer.c ### */

E struct monst *mk_mplayer(struct permonst *,XCHAR_P, XCHAR_P,BOOLEAN_P);
E void create_mplayers(int,BOOLEAN_P);
E void create_umplayers(int,BOOLEAN_P);
E void mplayer_talk(struct monst *);

#if defined(MICRO) || defined(WIN32)

/* ### msdos.c,os2.c,tos.c,winnt.c ### */

#  ifndef WIN32
E int tgetch(void);
#  endif
#  ifndef TOS
E char switchar(void);
#  endif
# ifndef __GO32__
E long freediskspace(char *);
#  ifdef MSDOS
E int findfirst_file(char *);
E int findnext_file(void);
E long filesize_nh(char *);
#  else
E int findfirst(char *);
E int findnext(void);
E long filesize(char *);
#  endif /* MSDOS */
E char *foundfile_buffer(void);
# endif /* __GO32__ */
# ifndef __CYGWIN__
E void chdrive(char *);
# endif
# ifndef TOS
E void disable_ctrlP(void);
E void enable_ctrlP(void);
# endif
# if defined(MICRO) && !defined(WINNT)
E void get_scr_size(void);
#  ifndef TOS
E void gotoxy(int,int);
#  endif
# endif
# ifdef TOS
E int _copyfile(char *,char *);
E int kbhit(void);
E void set_colors(void);
E void restore_colors(void);
#  ifdef SUSPEND
E int dosuspend(void);
#  endif
# endif /* TOS */
# ifdef WIN32
E char *get_username(int *);
E int set_binary_mode(int, int);
E void nt_regularize(char *);
E int (*nt_kbhit)(void);
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
E boolean linedup(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,BOOLEAN_P);
E boolean linedupB(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P);
E boolean lined_up(struct monst *);
E boolean lined_upB(struct monst *);
E struct obj *m_carrying(struct monst *,int);
E void m_useup(struct monst *,struct obj *);
E void m_throw(struct monst *,int,int,int,int,int,struct obj *);
E boolean hits_bars(struct obj **,int,int,int,int);
E struct monst *mfind_target(struct monst *, BOOLEAN_P);
E boolean mlined_up(struct monst *,struct monst *,BOOLEAN_P);

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
E int rnd_offensive_potion(struct monst *);
E boolean find_misc(struct monst *);
E int use_misc(struct monst *);
E int rnd_misc_item(struct monst *);
E int rnd_misc_item_new(struct monst *);
E boolean searches_for_item(struct monst *,struct obj *);
E boolean mon_reflects(struct monst *,const char *);
E boolean ureflects(const char *,const char *);
E boolean munstone(struct monst *,BOOLEAN_P);
E void monsteremptycontainers(struct monst *);

/* ### music.c ### */

E void awaken_soldiers(void);
E int do_play_instrument(struct obj *);
E void do_earthquake(int);
E void awaken_monsters(int);

/* ### nhlan.c ### */
#ifdef LAN_FEATURES
E void init_lan_features(void);
E char *lan_username(void);
# ifdef LAN_MAIL
E boolean lan_mail_check(void);
E void lan_mail_read(struct obj *);
E void lan_mail_init(void);
E void lan_mail_finish(void);
E void lan_mail_terminate(void);
# endif
#endif

/* ### nttty.c ### */

#ifdef WIN32CON
E void get_scr_size(void);
E int nttty_kbhit(void);
E void nttty_check_stdio(void);
E void nttty_open(void);
E void nttty_rubout(void);
E int tgetch(void);
E int ntposkey(int *, int *, int *);
E void set_output_mode(int);
E void synch_cursor(void);
#endif

/* ### o_init.c ### */

E void initobjectsamnesia(void);
E void init_objects(BOOLEAN_P);
E void randommaterials(void);
E void matraysershuffle(void);
E int find_skates(void);
E int find_skates2(void);
E int find_skates3(void);
E int find_skates4(void);
E int find_opera_cloak(void);
E int find_explosive_boots(void);
E int find_irregular_boots(void);
E int find_wedge_boots(void);
E int find_aluminium_helmet(void);
E int find_ghostly_cloak(void);
E int find_polnish_gloves(void);
E int find_velcro_boots(void);
E int find_clumsy_gloves(void);
E int find_fin_boots(void);
E int find_profiled_boots(void);
E int find_void_cloak(void);
E int find_grey_shaded_gloves(void);
E int find_weeb_cloak(void);
E int find_persian_boots(void);
E int find_hugging_boots(void);
E int find_fleecy_boots(void);
E int find_fingerless_gloves(void);
E int find_mantle_of_coat(void);
E int find_fatal_gloves(void);
E int find_beautiful_heels(void);
E int find_homicidal_cloak(void);
E int find_castlevania_boots(void);
E int find_greek_cloak(void);
E int find_celtic_helmet(void);
E int find_english_gloves(void);
E int find_korean_sandals(void);
E int find_octarine_robe(void);
E int find_chinese_cloak(void);
E int find_polyform_cloak(void);
E int find_absorbing_cloak(void);
E int find_birthcloth(void);
E int find_poke_mongo_cloak(void);
E int find_levuntation_cloak(void);
E int find_quicktravel_cloak(void);
E int find_angband_cloak(void);
E int find_anorexia_cloak(void);
E int find_dnethack_cloak(void);
E int find_team_splat_cloak(void);
E int find_eldritch_cloak(void);
E int find_erotic_boots(void);
E int find_secret_helmet(void);
E int find_difficult_cloak(void);
E int find_velvet_gloves(void);
E int find_velvet_pumps(void);
E int find_sputa_boots(void);
E int find_formula_one_helmet(void);
E int find_excrement_cloak(void);
E int find_racer_gloves(void);
E int find_turbo_boots(void);
E int find_guild_cloak(void);
E int find_shitty_gloves(void);
E int find_foundry_cloak(void);
E int find_spellsucking_cloak(void);
E int find_storm_coat(void);
E int find_fleeceling_cloak(void);
E int find_princess_gloves(void);
E int find_uncanny_gloves(void);
E int find_slaying_gloves(void);
E int find_blue_sneakers(void);
E int find_femmy_boots(void);
E int find_red_sneakers(void);
E int find_yellow_sneakers(void);
E int find_pink_sneakers(void);
E int find_calf_leather_sandals(void);
E int find_velcro_sandals(void);
E int find_buffalo_boots(void);
E int find_heroine_mocassins(void);
E int find_lolita_boots(void);
E int find_fetish_heels(void);
E int find_weapon_light_boots(void);
E int find_rubynus_helmet(void);
E int find_cursed_called_cloak(void);
E int find_fourchan_cloak(void);
E int find_inalish_cloak(void);
E int find_grunter_helmet(void);
E int find_cloudy_helmet(void);
E int find_breath_control_helmet(void);
E int find_gas_mask(void);
E int find_sages_helmet(void);
E int find_rayductnay_gloves(void);
E int find_orgasm_pumps(void);
E int find_worn_out_sneakers(void);
E int find_noble_sandals(void);
E int find_doctor_claw_boots(void);
E int find_feelgood_heels(void);
E int find_plof_heels(void);
E int find_princess_pumps(void);
E int find_ballet_heels(void);
E int find_steel_toed_boots(void);
E int find_marji_shoes(void);
E int find_mary_janes(void);
E int find_visored_helmet(void);
E int find_orange_visored_helmet(void);
E int find_business_shoes(void);
E int find_filigree_stilettos(void);
E int find_ugly_boots(void);
E int find_unisex_pumps(void);
E int find_cuddle_cloth_boots(void);
E int find_garden_slippers(void);
E int find_dyke_boots(void);
E int find_ankle_strap_sandals(void);
E int find_demonologist_boots(void);
E int find_mud_boots(void);
E int find_failed_shoes(void);
E int find_opera_pumps(void);
E int find_wooden_clogs(void);
E int find_regular_sneakers(void);
E int find_exceptional_sneakers(void);
E int find_cyan_sneakers(void);
E int find_elite_sneakers(void);
E int find_biker_boots(void);
E int find_zero_drop_shoes(void);
E int find_hiking_boots(void);
E int find_pope_hat(void);
E int find_corona_mask(void);
E int find_anachro_helmet(void);
E int find_ornamental_cope(void);
E int find_wetsuit(void);
E int find_multilinguitis_gloves(void);
E int find_throwaway_sandals(void);
E int find_sharp_edged_sandals(void);
E int find_nondescript_gloves(void);
E int find_lead_boots(void);
E int find_volcanic_cloak(void);
E int find_fuel_cloak(void);
E int find_cloister_cloak(void);
E int find_shemagh(void);
E int find_serrated_helmet(void);
E int find_thick_boots(void);
E int find_sand_als(void);
E int find_shadowy_heels(void);
E int find_weight_attachment_boots(void);
E int find_fungal_sandals(void);
E int find_standing_footwear(void);
E int find_frequent_helmet(void);
E int find_machinery_boots(void);
E int find_christmas_child_mode_boots(void);
E int find_sandals_with_socks(void);
E int find_wedge_sneakers(void);
E int find_barefoot_shoes(void);
E void oinit(void);
E void savenames(int,int);
E void restnames(int);
E void discover_object(int,BOOLEAN_P,BOOLEAN_P);
E void undiscover_object(int);
E int dodiscovered(void);

/* ### objects.c ### */

E void objects_init(void);

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
E char *cxname2(struct obj *);
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
E struct obj *readobjnam(char *,struct obj *,BOOLEAN_P, BOOLEAN_P);
E int rnd_class(int,int);
E const char *cloak_simple_name(struct obj *);
E const char *mimic_obj_name(struct monst *);

/* ### options.c ### */

E boolean match_optname(const char *,const char *,int,BOOLEAN_P);
E void initoptions(void);
E void parseoptions(char *,BOOLEAN_P,BOOLEAN_P);
E void parsetileset(char *);
E int doset(void);
E int dotogglepickup(void);
E void option_help(void);
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
E void free_autopickup_exceptions(void);
#endif /* AUTOPICKUP_EXCEPTIONS */

/* ### pager.c ### */

E int dowhatis(void);
E int doquickwhatis(void);
E int doidtrap(void);
E int dowhatdoes(void);
E char *dowhatdoes_core(CHAR_P, char *);
E int dohelp(void);
E int dohistory(void);
#ifdef EXTENDED_INFO
E void corpsepager(int);
#endif

E const char *noroelaname(void);

E const char *fauxmessage(void);
E const char *bosstaunt(void);
E const char *soviettaunt(void);
E const char *gangscholartaunt(void);
E const char *gangscholartaunt2(void);
E const char *gangscholartaunt3(void);
E const char *gangscholartaunt_specific(void);
E const char *gangscholartaunt_specific2(void);
E const char *longingmessage(void);
E const char *walscholartaunt(void);
E const char *walscholartaunt2(void);

E void gangscholarmessage(void);
E void walscholarmessage(void);
E void longingtrapeffect(void);
E void demagogueparole(void);
E void practicantmessage(void);
E void noroelataunt(void);

/* ### pcmain.c ### */

#if defined(MICRO) || defined(WIN32)
# ifdef CHDIR
E void chdirx(char *,BOOLEAN_P);
# endif /* CHDIR */
#endif /* MICRO || WIN32 */

/* ### pcsys.c ### */

#if defined(MICRO) || defined(WIN32)
E void flushout(void);
E int dosh(void);
# ifdef MFLOPPY
E void eraseall(const char *,const char *);
E void copybones(int);
E void playwoRAMdisk(void);
E int saveDiskPrompt(int);
E void gameDiskPrompt(void);
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
E void gettty(void);
E void settty(const char *);
E void setftty(void);
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
E void getlock(void);
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
E int pickup_object(struct obj *, long, BOOLEAN_P, BOOLEAN_P);
E int dump_container(struct obj *, BOOLEAN_P, int, int);
E int dump_container_tele(struct obj *, BOOLEAN_P, int, int);
E int dump_container_huro(struct obj *, BOOLEAN_P, int, int);
E int dump_container_superhuro(struct obj *, BOOLEAN_P, int, int);
E int query_category(const char *, struct obj *, int, menu_item **, int);
E int query_objlist(const char *, struct obj *, int, menu_item **, int, boolean (*)(OBJ_P));
E struct obj *pick_obj(struct obj *);
E int encumber_msg(void);
E int doloot(void);
E boolean container_gone(int (*)(OBJ_P));
E int use_container(struct obj **,int);
E int loot_mon(struct monst *,int *,boolean *);
E const char *safe_qbuf(const char *,unsigned, const char *,const char *,const char *);
E boolean is_autopickup_exception(struct obj *, BOOLEAN_P);
E void containerkaboom(void);
E long mbag_item_gone(int, struct obj *, BOOLEAN_P);

/* ### pline.c ### */

E void msgpline_add(int, char *);
E void msgpline_free(void);
E void pline(const char *,...) PRINTF_F(1,2);
E void Norep(const char *,...) PRINTF_F(1,2);
E void free_youbuf(void);
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
E const char *hybrid_str(void);
E const char *hybrid_strcode(void);
E const char *generate_garbage_string(void);
E const char *generate_garbage_char(void);
E void mstatusline(struct monst *);
E void mstatuslinebl(struct monst *);
E void ustatusline(void);
E void self_invis_message(void);

/* ### polyself.c ### */

E void init_uasmon(void);
E void set_uasmon(void);
E void change_sex(void);
E void newman(void);
E void polyself(BOOLEAN_P);
E int polymon(int);
E void rehumanize(void);
E int dobreathe(void);
E int dospit(void);
E int doremove(void);
E int dospinweb(void);
E int dosummon(void);
E int dogaze(void);
E int dohide(void);
E int domindblast(void);
E void skinback(BOOLEAN_P);
E const char *mbodypart(struct monst *,int);
E const char *body_part(int);
E int poly_gender(void);
E void ugolemeffects(int,int);
E int polyatwill(void);
E boolean polyskillchance(void);
E boolean powerfulimplants(void);

/* ### potion.c ### */

E void set_itimeout(long *,long);
E void incr_itimeout(long *,int);
E void playerbleed(long);
E void make_confused(long,BOOLEAN_P);
E void make_stunned(long,BOOLEAN_P);
E void make_numbed(long,BOOLEAN_P);
E void make_feared(long,BOOLEAN_P);
E void make_frozen(long,BOOLEAN_P);
E void make_burned(long,BOOLEAN_P);
E void make_dimmed(long,BOOLEAN_P);
E void make_blinded(long,BOOLEAN_P);
E void make_sick(long, const char *, BOOLEAN_P,int);
E void make_slimed(long);
E void make_vomiting(long,BOOLEAN_P);
E boolean make_hallucinated(long,BOOLEAN_P,long);
E int dodrink(void);
E void badeffect(void);
E void reallybadeffect(void);
E void goodeffect(void);
E void badpeteffect(struct monst *);
E void allbadpeteffects(struct monst *);
E void add_monster_egotype(struct monst *);
E void ragnarok(BOOLEAN_P);
E void evilragnarok(BOOLEAN_P,int);
E void datadeleteattack(void);
E void destroyarmorattack(void);
E int dopotion(struct obj *);
E int peffects(struct obj *);
E void healup(int,int,BOOLEAN_P,BOOLEAN_P);
E void healup_mon(struct monst *, int,int,BOOLEAN_P,BOOLEAN_P);
E void strange_feeling(struct obj *,const char *);
E void potionhit(struct monst *,struct obj *,BOOLEAN_P);
E void potionbreathe(struct obj *);
E boolean get_wet(struct obj *, BOOLEAN_P);
E int dodip(void);
E void djinni_from_bottle(struct obj *,int);
/* KMH, balance patch -- new function */
E int upgrade_obj(struct obj *);
E struct monst *split_mon(struct monst *,struct monst *);
E const char *bottlename(void);
E void bad_equipment(int);
E void bad_equipment_heel(void);
E void bad_equipment_femshoes(void);
E void bad_equipment_implant(void);
E void increasesanity(int);
E boolean conundrumbreak(void);
E void statdrain(void);
E void nastytrapcurse(void);
E void shank_player(void);
E void terrainterror(void);
E void giftartifact(void);
E void heraldgift(void);
E void sjwtrigger(void);
E void buttlovertrigger(void);
E void emerafrenzy(void);
E boolean have_anorexia(void);
E void uncurseoneitem(void);
E void maybehackimplant(void);
E boolean everythingfleecy(void);
E void dropitemattack(void);

E boolean sandprotection(void);
E boolean shadowprotection(void);
E boolean obsidianprotection(void);
E boolean chromeprotection(void);
E int angelshadowstuff(void);
E void statdebuff(void);
E void nivellate(void);
E void upnivel(BOOLEAN_P);
E void nexus_swap(void);

E boolean weaponwilldull(struct obj *);
E boolean armorwilldull(struct obj *);

E boolean hybridragontype(int);

E boolean extralongsqueak(void);

E boolean is_musable(struct obj *);
E boolean ismusablenumber(int);
E boolean ishighheeled(struct obj *);
E boolean ishighheeledb(int);

E boolean playerwearshighheels(void);
E boolean playerwearssexyflats(void);
E boolean maybestilettoheels(void);
E boolean maybeconeheels(void);
E boolean maybeblockheels(void);
E boolean maybewedgeheels(void);
E boolean playerextrinsicaggravatemon(void);
E boolean automore_active(void);

E boolean playerextrinsicfireres(void);
E boolean playerextrinsiccoldres(void);
E boolean playerextrinsicshockres(void);
E boolean playerextrinsicpoisonres(void);
E boolean playerextrinsicspeed(void);

E boolean itemhasappearance(struct obj *, int);
E boolean itemnumwithappearance(int, int);

E void fineforpracticant(int, int, int);
E void practicant_payup(void);

/* ### pray.c ### */

/*#ifdef USE_TRAMPOLI*/
E int prayer_done(void);

E void god_zaps_you(ALIGNTYP_P);
E void godvoice(ALIGNTYP_P, const char*);
E void angrygods(ALIGNTYP_P);
/*#endif*/
E int dosacrifice(void);
E boolean can_pray(BOOLEAN_P);
E int dopray(void);
E const char *u_gname(void);
E int doturn(void);
E int turn_undead(void);
E int turn_allmonsters(void);
E const char *a_gname(void);
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
E void angry_priest(void);
E void clearpriests(void);
E void restpriest(struct monst *,BOOLEAN_P);

/* ### quest.c ### */

E void onquest(void);
E void nemdead(void);
E void artitouch(void);
E boolean ok_to_quest(void);
E void leader_speaks(struct monst *);
E void nemesis_speaks(void);
E void quest_chat(struct monst *);
E void quest_talk(struct monst *);
E void quest_stat_check(struct monst *);
E void finish_quest(struct obj *);

/* ### questpgr.c ### */

E void load_qtlist(void);
E void unload_qtlist(void);
E int quest_info(int);
E const char *ldrname(void);
E boolean is_quest_artifact(struct obj*);
E void com_pager(int);
E void qt_pager(int);
E struct permonst *qt_montype(void);
E struct permonst *qt_rival_montype(void);
E const char *neminame(void);

/* ### random.c ### */

#if defined(RANDOM) && !defined(__GO32__) /* djgpp has its own random */
E void srandom(unsigned);
E char *initstate(unsigned,char *,int);
E char *setstate(char *);
E long random(void);
#endif /* RANDOM */

/* ### read.c ### */

E int doread(void);
E boolean is_chargeable(struct obj *);
E boolean is_enchantable(struct obj *);
E void recharge(struct obj *,int);
E void randomenchant(struct obj *,int,BOOLEAN_P);
E void forget(int);
E void forget_objects(int);
E void forget_levels(int);
E void forget_traps(void);
E void forget_map(int);
E void maprot(void);
E int seffects(struct obj *);
#ifdef USE_TRAMPOLI
E void set_lit(int,int,void *);
#endif
E void litroom(BOOLEAN_P,struct obj *);
E void litroomlite(boolean);
E void do_genocide(int);
E void punish(struct obj *);
E void unpunish(void);
E void punishx(void);
E boolean cant_create(int *, BOOLEAN_P);
#ifdef WIZARD
E struct monst *create_particular(void);
#endif
#ifdef GMMODE
E void gmmode_genesis(const char *);
#endif
E void forget_single_object(int);
E void wandofchaosterrain(void);
E void wandoffleecyterrain(void);

/* ### rect.c ### */

E void init_rect(void);
E NhRect *get_rect(NhRect *);
E NhRect *rnd_rect(void);
E void remove_rect(NhRect *);
E void add_rect(NhRect *);
E void split_rects(NhRect *,NhRect *);

/* ## region.c ### */
E void clear_regions(void);
E void run_regions(void);
E boolean in_out_region(XCHAR_P,XCHAR_P);
E boolean m_in_out_region(struct monst *,XCHAR_P,XCHAR_P);
E void update_player_regions(void);
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
E void minit(void);
E boolean lookup_id_mapping(unsigned, unsigned *);
#ifdef ZEROCOMP
E int mread(int,void *,unsigned int);
#else
E void mread(int,void *,unsigned int);
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
E int randattack(void);
E int randomdamageattack(void);
E int randomthievingattack(void);
E int randomillusionattack(void);
E int randmonsound(void);
E int randenchantment(void);
E int randnastyenchantment(void);
E int randomdisableproperty(void);
E int randominsidetrap(void);
E int randartmeleeweapon(void);
E int randartlightsaber(void);
E int randartlauncher(void);
E int randartmissile(void);
E int randartsuit(void);
E int randartshirt(void);
E int randartcloak(void);
E int randarthelm(void);
E int randartgloves(void);
E int randartshield(void);
E int randartboots(void);
E int randartring(void);
E int randartamulet(void);
E int randartimplant(void);
E int randartwand(void);
E int randartspellbook(void);
E int randartpotion(void);
E int randartscroll(void);
E int randartball(void);
E int randartchain(void);
E int randartgem(void);
E int randartmeleeweaponX(void);
E int randartlightsaberX(void);
E int randartlauncherX(void);
E int randartmissileX(void);
E int randartsuitX(void);
E int randartshirtX(void);
E int randartcloakX(void);
E int randarthelmX(void);
E int randartglovesX(void);
E int randartshieldX(void);
E int randartbootsX(void);
E int randartringX(void);
E int randartamuletX(void);
E int randartimplantX(void);
E int randartwandX(void);
E int randartspellbookX(void);
E int randartpotionX(void);
E int randartscrollX(void);
E int randartballX(void);
E int randartchainX(void);
E int randartgemX(void);

E int randartbadgraystone(void);
E int randartfeminismjewel(void);

E void deacrandomintrinsic(int);
E int goodimplanteffect(struct obj *);
E void randomnastytrapeffect(int, int);
E int randomgoodskill(void);
E int randomgoodcombatskill(void);
E int randomgoodmclass(BOOLEAN_P);
E void getnastytrapintrinsic(void);
E void randomfeminismtrap(int);

/* ### role.c ### */

E boolean validrole(int);
E boolean validrace(int, int);
E boolean validgend(int, int, int);
E boolean validalign(int, int, int);
E int randrole(void);
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
E void role_init(void);
E void rigid_role_checks(void);
E void plnamesuffix(void);
E const char *Hello(struct monst *);
E const char *Goodbye(void);
E char *build_plselection_prompt(char *, int, int, int, int, int);
E char *root_plselection_prompt(char *, int, int, int, int, int);
E void recursioneffect(void);
E void temprecursioneffect(void);
E void greenslimetransformation(void);
E void bindertransformation(void);
E void climrecursioneffect(void);
E void changehybridization(int);
E void demagoguerecursioneffect(void);
E void rivalroleinit(void);
E void rivalrolerestore(void);

/* ### rumors.c ### */

E char *getrumor(int,char *, BOOLEAN_P);
E void outrumor(int,int,BOOLEAN_P);
E void outoracle(BOOLEAN_P, BOOLEAN_P);
E void save_oracles(int,int);
E void restore_oracles(int);
E int doconsult(struct monst *);

/* ### save.c ### */

E int dosave(void);
#if defined(UNIX) || defined(VMS) || defined(__EMX__) || defined(WIN32)
E void hangup(int);
#endif
E int dosave0(void);
#ifdef INSURANCE
E void savestateinlock(void);
#endif
#ifdef MFLOPPY
E boolean savelev(int,XCHAR_P,int);
E boolean swapin_file(int);
E void co_false(void);
#else
E void savelev(int,XCHAR_P,int);
#endif
E void bufon(int);
E void bufoff(int);
E void bflush(int);
E void bwrite(int,void *,unsigned int);
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

E void free_dungeons(void);
E void freedynamicdata(void);

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
E void shopper_financial_report(void);
E int inhishop(struct monst *);
E struct monst *shop_keeper(CHAR_P);
E boolean tended_shop(struct mkroom *);
E void delete_contents(struct obj *);
E void obfree(struct obj *,struct obj *);
E void home_shk(struct monst *,BOOLEAN_P);
E void make_happy_shk(struct monst *,BOOLEAN_P);
E void hot_pursuit(struct monst *);
E void make_angry_shk(struct monst *,XCHAR_P,XCHAR_P);
E int dopay(void);
E boolean paybill(int);
E void finish_paybill(void);
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

E void take_gold(void);
E int dosit(void);
E void rndcurse(void);
E void attrcurse(void);
E void skillcaploss(void);
E void skillcaploss_severe(void);
E void skillcaploss_specific(int);

/* ### sounds.c ### */

E void dosounds(void);
E void pet_distress(struct monst *, int);
E const char *growl_sound(struct monst *);
/* JRN: converted growl,yelp,whimper to macros based on pet_distress.
  Putting them here since I don't know where else (TOFIX) */
#define growl(mon) pet_distress((mon),3)
#define yelp(mon) pet_distress((mon),2)
#define whimper(mon) pet_distress((mon),1)
E void beg(struct monst *);
E int dotalk(void);
#ifdef USER_SOUNDS
E int add_sound_mapping(const char *);
E void play_sound_for_message(const char *);
#endif
E void maybegaincha(void);
E void playermsconvert(void);
E void playerwouwoutaunt(void);
E void playerwhoretalk(void);
E void playersupermantaunt(void);
E void playerrattlebones(void);
E void conversionsermon(void);
E void wouwoutaunt(void);
E void supermantaunt(void);

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
E int randomtrap(void);
E void makerandomtrap(BOOLEAN_P);
E void makeinvisotrap(void);
E void makegirlytrap(void);
E void makespacewarstrap(void);

/* ### spell.c ### */

E boolean spell_known(int);
#ifdef USE_TRAMPOLI
E int learn(void);
#endif
E int study_book(struct obj *);
E void book_disappears(struct obj *);
E void book_substitution(struct obj *,struct obj *);
E void age_spells(void);
E int docast(void);
E int spell_skilltype(int);
E int spelleffects(int,BOOLEAN_P);
E void losespells(void);
E void spellmemoryloss(int);
E int dovspell(void);
E void learnspell(struct obj *);
E boolean studyspell(void);
E boolean inertiacontrolspell(void);
E boolean dememorizespell(void);
E boolean addsomespellmemory(void);
E boolean addsomespellmemoryX(void);
E void initialspell(struct obj *);
E void castinertiaspell(void);
E void wonderspell(void);
E int isqrt(int);
E boolean mastermindsave(void);
E void removeforgottenspell(void);
E void evilspellforget(void);
E int dodeletespell(void);
E void extramemory(void);

/* ### steal.c ### */

#ifdef USE_TRAMPOLI
E int stealarm(void);
#endif
#ifdef GOLDOBJ
E long somegold(long);
#else
E long somegold(void);
#endif
E void stealgold(struct monst *);
E void remove_worn_item(struct obj *,BOOLEAN_P);
E int steal(struct monst *, char *,BOOLEAN_P,BOOLEAN_P);
E int mpickobj(struct monst *,struct obj *,BOOLEAN_P);
E void stealamulet(struct monst *);
E void mdrop_special_objs(struct monst *);
E void relobj(struct monst *,int,BOOLEAN_P);
#ifdef GOLDOBJ
E struct obj *findgold(struct obj *);
#endif

/* ### steed.c ### */

E void rider_cant_reach(void);
E boolean can_saddle(struct monst *);
E int use_saddle(struct obj *);
E boolean can_ride(struct monst *);
E int doride(void);
E boolean mount_steed(struct monst *, BOOLEAN_P);
E void exercise_steed(void);
E void kick_steed(void);
E void dismount_steed(int);
E void place_monster(struct monst *,int,int);
E boolean mayfalloffsteed(void);
E boolean will_hit_steed(void);
E boolean confsteeddir(void);
E boolean flysaddle(void);
E boolean bmwride(int);
E boolean mercedesride(int, struct monst *);

/* ### tech.c ### */

E void adjtech(int,int);
E int dotech(void);
E int dotechwiz(void);
E void datadeletetechs(void);
E void docalm(void);
E void stopsingletechnique(int);
E int tech_inuse(int);
E void tech_timeout(void);
E boolean tech_known(SHORT_P);
E void learntech_or_leveltech(SHORT_P,long,int);
E void learntech(SHORT_P,long,int);
E int disarm_holdingtrap(struct trap *);
E int disarm_rust_trap(struct trap *);
E int disarm_fire_trap(struct trap *);
E int disarm_landmine(struct trap *);
E int disarm_squeaky_board(struct trap *);
E int disarm_shooting_trap(struct trap *, int);
E void techcapincrease(int);
E void eviltechincrease(void);
E int get_tech_no(int);
E int techlevX(int);
E void hurtmon(struct monst *,int);
E void techdrain(void);
E void techdrainsevere(void);
E void techlevelup(void);
E void techlevelspecific(int);
E void learnrandomregulartech(void);

/* ### teleport.c ### */

E boolean goodpos(int,int,struct monst *,unsigned);
E boolean enexto(coord *,XCHAR_P,XCHAR_P,struct permonst *);
E boolean enexto_core(coord *,XCHAR_P,XCHAR_P,struct permonst *,unsigned);
E int epathto(coord *,int,XCHAR_P,XCHAR_P,struct permonst *);
E void xpathto(int,XCHAR_P,XCHAR_P,int (*)(void *,int,int),void *);
E void teleds(int,int,BOOLEAN_P);
E boolean safe_teleds(BOOLEAN_P);
E boolean safe_teledsPD(BOOLEAN_P);
E boolean safe_teledsNOTRAP(BOOLEAN_P);
E boolean teleport_pet(struct monst *,BOOLEAN_P);
E void tele(void);
E void phase_door(BOOLEAN_P);
E int dotele(void);
E int dotele_post(BOOLEAN_P);
E void level_tele(void);
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
E int random_teleport_level(void);
E int random_banish_level(void);
E d_level random_branchport_level(void);
E void randombranchtele(void);
E void banishplayer(void);
E void teleX(void);
E boolean u_teleport_mon(struct monst *,BOOLEAN_P);
E boolean u_teleport_monB(struct monst *,BOOLEAN_P);
E boolean u_teleport_monC(struct monst *,BOOLEAN_P);
E void pushplayer(BOOLEAN_P);
E void pushplayersilently(BOOLEAN_P);
E boolean pushmonster(struct monst *);
E boolean teleok(int,int,BOOLEAN_P);

/* ### tile.c ### */
#ifdef USE_TILES
E void substitute_tiles(d_level *);
#endif

/* ### timeout.c ### */

E void burn_away_slime(void);
E void nh_timeout(void);
E void fall_asleep(int, BOOLEAN_P);
E void set_obj_poly(struct obj *, struct obj *);
E void unpoly_obj(void *, long);
E int mon_poly(struct monst *, BOOLEAN_P, const char *);
E int mon_spec_poly(struct monst *, struct permonst *, long, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P);
E int mon_spec_polyX(struct monst *, struct permonst *, long, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P);
E void unpoly_mon(void *, long);
E void attach_bomb_blow_timeout(struct obj *, int, BOOLEAN_P);
E void attach_egg_hatch_timeout(struct obj *);
E void attach_fig_transform_timeout(struct obj *);
E void kill_egg(struct obj *);
E void hatch_egg(void *, long);
E void learn_egg_type(int);
E void burn_object(void *, long);
E void begin_burn(struct obj *, BOOLEAN_P);
E void end_burn(struct obj *, BOOLEAN_P);
E void burn_faster(struct obj *, long);
E void lightsaber_deactivate(struct obj *, BOOLEAN_P);
E void do_storms(void);
E boolean start_timer(long, SHORT_P, SHORT_P, void *);
E long stop_timer(SHORT_P, void *);
E void run_timers(void);
E void obj_move_timers(struct obj *, struct obj *);
E void obj_split_timers(struct obj *, struct obj *);
E void obj_stop_timers(struct obj *);
E void mon_stop_timers(struct monst *);
E boolean obj_is_local(struct obj *);
E void save_timers(int,int,int);
E void restore_timers(int,int,BOOLEAN_P,long);
E void relink_timers(BOOLEAN_P);
#ifdef WIZARD
E int wiz_timeout_queue(void);
E void timer_sanity_check(void);
#endif
E void cryogenics(void);

/* ### topten.c ### */

E void topten(int);
E void prscore(int,char **);
E struct obj *tt_oname(struct obj *);
/*E void tt_mname(struct monst *);*/
/*E void mkundeadboo(coord *,BOOLEAN_P,int);*/
E void tt_mname(coord *,BOOLEAN_P,int);
#ifdef GTK_GRAPHICS
E winid create_toptenwin(void);
E void destroy_toptenwin(void);
#endif
E const char *gamemode_strcode(void);

/* ### track.c ### */

E void initrack(void);
E void settrack(void);
E coord *gettrack(int,int);

/* ### trap.c ### */

E boolean burnarmor(struct monst *);
E boolean rust_dmg(struct obj *,const char *,int,BOOLEAN_P,struct monst *);

E boolean wither_dmg(struct obj *,const char *,int,BOOLEAN_P,struct monst *);

E void dofiretrap(struct obj *);

E void grease_protect(struct obj *,const char *,struct monst *);
E struct trap *maketrap(int,int,int,int,BOOLEAN_P);
E void makerandomtrap_at(int,int,BOOLEAN_P);
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
E void float_up(void);
E void fill_pit(int,int);
E int float_down(long, long);
E int fire_damage(struct obj *,BOOLEAN_P,BOOLEAN_P,XCHAR_P,XCHAR_P);
E void water_damage(struct obj *,BOOLEAN_P,BOOLEAN_P);
E void doshittrap(struct obj *);
E void lethe_damage(struct obj *,BOOLEAN_P,BOOLEAN_P);
E void actual_lethe_damage(struct obj *,BOOLEAN_P,BOOLEAN_P);
E void withering_damage(struct obj *,BOOLEAN_P,BOOLEAN_P);
E void antimatter_damage(struct obj *,BOOLEAN_P,BOOLEAN_P);
E boolean drown(void);
E boolean crystaldrown(void);
E void mon_drain_en(struct monst *, int);
E void drain_en(int);
E int dountrap(void);
E int untrap(BOOLEAN_P);
E boolean chest_trap(struct obj *,int,BOOLEAN_P);
E void deltrap(struct trap *);
E boolean delfloortrap(struct trap *);
E struct trap *t_at(int,int);
E void b_trapped(const char *,int);
E boolean unconscious(void);
E boolean lava_effects(void);
E void blow_up_landmine(struct trap *);
E int launch_obj(SHORT_P,int,int,int,int,int);
/* KMH, balance patch -- new function */
E int uunstone(void);
E void fartingweb(void);

/* ### u_init.c ### */

E void u_init(void);
E void alter_reality(int);
E void polyinitors(void);
E void reinitgods(void);

/* ### uhitm.c ### */

E void hurtmarmor(struct monst *,int);
E int attack_checks(struct monst *,BOOLEAN_P);
E void check_caitiff(struct monst *);
E int find_roll_to_hit(struct monst *);
E boolean attack(struct monst *);
E boolean hmon(struct monst *,struct obj *,int,int);
E int damageum(struct monst *,struct attack *);
E void missum(struct monst *,int, int, struct attack *);
E int passive(struct monst *,int,int,UCHAR_P,BOOLEAN_P);
E void passive_obj(struct monst *,struct obj *,struct attack *);
E void stumble_onto_mimic(struct monst *);
E int flash_hits_mon(struct monst *,struct obj *);
E void ranged_thorns(struct monst *);

/* ### unixmain.c ### */

#ifdef UNIX
# ifdef PORT_HELP
E void port_help(void);
# endif
#endif /* UNIX */

/* ### unixtty.c ### */

#if defined(UNIX) || defined(__BEOS__)
E void gettty(void);
E void settty(const char *);
E void setftty(void);
E void intron(void);
E void introff(void);
E void error(const char *,...) PRINTF_F(1,2);
#endif /* UNIX || __BEOS_ */

/* ### unixunix.c ### */

#ifdef UNIX
E void getlock(void);
E void regularize(char *);
# if defined(TIMED_DELAY) && !defined(msleep) && defined(SYSV)
E void msleep(unsigned);
# endif
# ifndef PUBLIC_SERVER
E int dosh(void);
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
E void invault(void);
E int gd_move(struct monst *);
E void paygd(void);
E long hidden_gold(void);
E boolean gd_sound(void);

/* ### version.c ### */

E char *version_string(char *);
E char *getversionstring(char *);
E int doversion(void);
E int doextversion(void);
#ifdef MICRO
E boolean comp_times(long);
#endif
E boolean check_version(struct version_info *, const char *,BOOLEAN_P);
E unsigned long get_feature_notice_ver(char *);
E unsigned long get_current_feature_ver(void);
#ifdef RUNTIME_PORT_ID
E void append_port_id(char *);
#endif

/* ### video.c ### */

#ifdef MSDOS
E int assign_video(char *);
# ifdef NO_TERMS
E void gr_init(void);
E void gr_finish(void);
# endif
E void tileview(BOOLEAN_P);
#endif
#ifdef VIDEOSHADES
E int assign_videoshades(char *);
E int assign_videocolors(char *);
#endif

/* ### vis_tab.c ### */

#ifdef VISION_TABLES
E void vis_tab_init(void);
#endif

/* ### vision.c ### */

E void vision_init(void);
E int does_block(int,int,struct rm*);
E void vision_reset(void);
E void vision_recalc(int);
E void block_point(int,int);
E void unblock_point(int,int);
E boolean clear_path(int,int,int,int);
E void do_clear_area(int,int,int, void (*)(int,int,void *),void *);
E void do_clear_areaX(int,int,int, void (*)(int,int,void *),void *);
E void blockorunblock_point(int,int);

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

E unsigned long init_broadcast_trapping(void);
E unsigned long enable_broadcast_trapping(void);
E unsigned long disable_broadcast_trapping(void);
# if 0
E struct mail_info *parse_next_broadcast(void);
# endif /*0*/

/* ### vmsmain.c ### */

E int main(int, char **);
# ifdef CHDIR
E void chdirx(const char *,BOOLEAN_P);
# endif /* CHDIR */

/* ### vmsmisc.c ### */

E void vms_abort(void);
E void vms_exit(int);

/* ### vmstty.c ### */

E int vms_getchar(void);
E void gettty(void);
E void settty(const char *);
E void shuttty(const char *);
E void setftty(void);
E void intron(void);
E void introff(void);
E void error(const char *,...) PRINTF_F(1,2);
#ifdef TIMED_DELAY
E void msleep(unsigned);
#endif

/* ### vmsunix.c ### */

E void getlock(void);
E void regularize(char *);
E int vms_getuid(void);
E boolean file_is_stmlf(int);
E int vms_define(const char *,const char *,int);
E int vms_putenv(const char *);
E char *verify_termcap(void);
# if defined(CHDIR) || !defined(PUBLIC_SERVER) || defined(SECURE)
E void privoff(void);
E void privon(void);
# endif
# ifndef PUBLIC_SERVER
E int dosh(void);
# endif
# if !defined(PUBLIC_SERVER) || defined(MAIL)
E int vms_doshell(const char *,BOOLEAN_P);
# endif
# ifdef SUSPEND
E int dosuspend(void);
# endif

#endif /* VMS */

/* ### weapon.c ### */

E int hitval(struct obj *,struct monst *);
E int dmgval(struct obj *,struct monst *);
E int dmgvalX(struct obj *,struct monst *);
E struct obj *select_rwep(struct monst *,BOOLEAN_P);
E struct obj *select_hwep(struct monst *);
E void possibly_unwield(struct monst *,BOOLEAN_P);
E int mon_wield_item(struct monst *);
E int abon(void);
E int dbon(void);
E int enhance_weapon_skill(void);
#ifdef DUMP_LOG
E void dump_weapon_skill(void);
#endif
E void unrestrict_weapon_skill(int);
E void use_skill(int,int);
E void add_weapon_skill(int);
E void lose_weapon_skill(int);
E void lose_last_spent_skill(void);
E int weapon_type(struct obj *);
E int uwep_skill_type(void);
E int weapon_hit_bonus(struct obj *);
E int weapon_dam_bonus(struct obj *);
E int melee_dam_bonus(struct obj *);
E int ranged_dam_bonus(struct obj *);
E int skill_bonus(int);
E void skill_init(const struct def_skill *);
E void practice_weapon(void);
E void xtraskillinit(void);
E int get_obj_skill(struct obj *,BOOLEAN_P);
E void skilltrainingdecrease(int);
E void evilskilldecrease(void);
E void dataskilldecrease(void);
E void doubleskilltraining(void);
E void additionalskilltraining(void);
E void unrestrictskillchoice(void);
E const char *wpskillname(int);
E void grinderattack(int);
E int difeasemu(struct permonst *);
E void climtrainsqueaking(int);

/* ### were.c ### */

E int counter_were(int);
E void were_change(struct monst *);
E void new_were(struct monst *);
E int were_summon(struct permonst *,BOOLEAN_P,int *,char *,BOOLEAN_P);
E void you_were(void);
E void you_unwere(BOOLEAN_P);

/* ### wield.c ### */

E void setuwep(struct obj *,BOOLEAN_P,BOOLEAN_P);
E void swaptech(void);
E void setuqwep(struct obj *);
E void setuswapwep(struct obj *,BOOLEAN_P);
E int dowield(void);
E int doswapweapon(void);
E int dowieldquiver(void);
E boolean wield_tool(struct obj *,const char *);
E int can_twoweapon(void);
E void drop_uswapwep(void);
E int dotwoweapon(void);
E void uwepgone(void);
E void uswapwepgone(void);
E void uqwepgone(void);
E void untwoweapon(void);
E void erode_obj(struct obj *,BOOLEAN_P,BOOLEAN_P);
E void wither_obj(struct obj *,BOOLEAN_P,BOOLEAN_P);
E int chwepon(struct obj *,int);
E int welded(struct obj *);
E void weldmsg(struct obj *);
E void setmnotwielded(struct monst *,struct obj *);
E void unwield(struct obj *,BOOLEAN_P);
E int ready_weapon(struct obj *, BOOLEAN_P);

/* ### windows.c ### */

E void choose_windows(const char *);
E char genl_message_menu(CHAR_P,int,const char *);
E void genl_preference_update(const char *);

/* ### wizard.c ### */

E void amulet(void);
E int mon_has_amulet(struct monst *);
E int mon_has_special(struct monst *);
E int tactics(struct monst *);
E void aggravate(void);
E void clonewiz(void);
E int pick_nasty(void);
E int nasty(struct monst*);
E void resurrect(void);
E void intervene(void);
E void wizdead(void);
E void cuss(struct monst *);
E void randomcuss(void);

/* ### worm.c ### */

E int get_wormno(void);
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
E void clear_bypasses(void);
E int racial_exception(struct monst *, struct obj *);

/* ### write.c ### */

E int writecost(struct obj *);
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
E void cancel_item(struct obj *,BOOLEAN_P);
E boolean drain_item(struct obj *);	/* KMH */
E boolean drain_item_reverse(struct obj *);	/* KMH */
E boolean drain_item_severely(struct obj *);	/* KMH */
E boolean drain_item_negative(struct obj *);	/* KMH */
E boolean obj_resists(struct obj *,int,int);
E boolean obj_shudders(struct obj *);
E void throwstorm(struct obj *, int, int, int);
E void do_osshock(struct obj *);
E void puton_worn_item(struct obj *);
E struct obj *poly_obj(struct obj *, int, BOOLEAN_P);
E int bhito(struct obj *,struct obj *);
E int bhitpile(struct obj *,int (*)(OBJ_P,OBJ_P),int,int);
E int zappable(struct obj *);
E void zapnodir(struct obj *);
E int dozap(void);
E int zapyourself(struct obj *,BOOLEAN_P);
E boolean cancel_monst(struct monst *,struct obj *, BOOLEAN_P,BOOLEAN_P,BOOLEAN_P);
E void weffects(struct obj *);
E int spell_damage_bonus(int);
E const char *exclam(int force);
E void hit(const char *,struct monst *,const char *);
E void miss(const char *,struct monst *);
E struct monst *bhit(int,int,int,int,int (*)(MONST_P,OBJ_P), int (*)(OBJ_P,OBJ_P),struct obj **, BOOLEAN_P);
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
E void makewish(BOOLEAN_P);
E void acquireitem(void);
E void makenonworkingwish(void);
E void othergreateffect(void);
/* KMH -- xchar to XCHAR_P */
E void zap_strike_fx(XCHAR_P, XCHAR_P, int);
E void throwspell(void);

E void cancelmonsterlite(struct monst *);

/* ### livelog.c ### */
#ifdef LIVELOGFILE
E boolean livelog_start(void);
E void livelog_achieve_update(void);
E void livelog_wish(char*);
E void livelog_avert_death(void);
E void livelog_report_trophy(char *);
#ifdef LIVELOG_BONES_KILLER 
E void livelog_bones_killed(struct monst *);
#endif
#ifdef LIVELOG_SHOUT
E int doshout(void);
#endif
#endif

/* ### termcap.c ### */
E void term_start_bgcolor(int);
E void term_end_bgcolor(void);

#endif /* !MAKEDEFS_C && !LEV_LEX_C */

#undef E

#endif /* EXTERN_H */
