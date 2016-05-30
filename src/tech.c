/*      SCCS Id: @(#)tech.c    3.2     98/Oct/30        */
/*      Original Code by Warren Cheung (Basis: spell.c, attrib.c) */
/*      Copyright (c) M. Stephenson 1988                          */
/* NetHack may be freely redistributed.  See license for details. */

/* All of the techs from cmd.c are ported here */

#include "hack.h"

/* #define DEBUG */		/* turn on for diagnostics */

static boolean FDECL(gettech, (int *));
static boolean FDECL(dotechmenu, (int, int *));
static void NDECL(doblitzlist);
static int FDECL(get_tech_no,(int));
static int FDECL(techeffects, (int));
static void FDECL(hurtmon, (struct monst *,int));
static int FDECL(mon_to_zombie, (int));
STATIC_PTR int NDECL(tinker);
#ifdef JEDI
STATIC_PTR int NDECL(charge_saber);
#endif
STATIC_PTR int NDECL(draw_energy);
static const struct innate_tech * NDECL(role_tech);
static const struct innate_tech * NDECL(race_tech);
static int NDECL(doblitz);
static int NDECL(blitz_chi_strike);
static int NDECL(blitz_e_fist);
static int NDECL(blitz_pummel);
static int NDECL(blitz_g_slam);
static int NDECL(blitz_dash);
static int NDECL(blitz_power_surge);
static int NDECL(blitz_spirit_bomb);
static void FDECL(maybe_tameX, (struct monst *));

static NEARDATA schar delay;            /* moves left for tinker/energy draw */
static NEARDATA const char revivables[] = { ALLOW_FLOOROBJ, FOOD_CLASS, 0 };

/* 
 * Do try to keep the names <= 25 chars long, or else the
 * menu will look bad :B  WAC
 */
 
STATIC_OVL NEARDATA const char *tech_names[] = {
	"no technique",
	"berserk",
	"kiii",
	"research",
	"surgery",
	"reinforce memory",
	"missile flurry",
	"weapon practice",
	"eviscerate",
	"healing hands",
	"calm steed",
	"turn undead",
	"vanish",
	"cutthroat",
	"blessing",
	"elemental fist",
	"primal roar",
	"liquid leap",
	"critical strike",
	"sigil of control",
	"sigil of tempest",
	"sigil of discharge",
	"raise zombies",
	"revivification",
	"ward against flame",
	"ward against ice",
	"ward against lightning",
	"tinker",
	"rage eruption",
	"blink",
	"chi strike",
	"draw energy",
	"chi healing",
	"disarm",
	"dazzle",
	"chained blitz",
	"pummel",
	"ground slam",
	"air dash",
	"power surge",
	"spirit bomb",
	"draw blood",
	"world fall",
	"create ammo",
	"poke ball",
	"attire charm",
	"summon team ant",
	"appraisal",
	"egg bomb",
	"booze",
	"invoke deity",
	"double trouble",
	"phase door",
#ifdef JEDI
	"jedi jump",
	"charge saber",
	"telekinesis",
#endif
	""
};

static const struct innate_tech 
	/* Roles */
	arc_tech[] = { {   1, T_RESEARCH, 1},
		       {   0, 0, 0} },
	ass_tech[] = { {   1, T_CUTTHROAT, 1},
		       {   0, 0, 0} },
	lad_tech[] = { {   1, T_ATTIRE_CHARM, 1},
		       {   0, 0, 0} },
	fnc_tech[] = { {   1, T_CRIT_STRIKE, 1},
		       {   0, 0, 0} },
	fig_tech[] = { {   1, T_BERSERK, 1},
		       {   0, 0, 0} },
	sex_tech[] = { {   1, T_ATTIRE_CHARM, 1},
		       {   1, T_POKE_BALL, 1},
		       {   0, 0, 0} },
	jes_tech[] = { {   30, T_EGG_BOMB, 1},
		       {   0, 0, 0} },
	stu_tech[] = { {   1, T_LIQUID_LEAP, 1},
		       {   1, T_JEDI_JUMP, 1},
		       {   0, 0, 0} },
	gun_tech[] = { {   1, T_CREATE_AMMO, 1},
		       {   1, T_SURGERY, 1},
		       {   0, 0, 0} },
	mar_tech[] = { {   1, T_CREATE_AMMO, 1},
		       {   0, 0, 0} },
	ana_tech[] = { {   1, T_CREATE_AMMO, 1},
		       {   0, 0, 0} },
	lib_tech[] = { {   1, T_RESEARCH, 1},
		       {   0, 0, 0} },
	med_tech[] = { {   1, T_TELEKINESIS, 1},
		       {   0, 0, 0} },
	dol_tech[] = { {   1, T_ATTIRE_CHARM, 1},
		       {   7, T_RESEARCH, 1},
		       {   10, T_POKE_BALL, 1},
		       {   0, 0, 0} },
	ama_tech[] = { {   1, T_FLURRY, 1},
		       {   1, T_PRACTICE, 1},
		       {   5, T_CRIT_STRIKE, 1},
		       {   15, T_VANISH, 1},
		       {   25, T_ATTIRE_CHARM, 1},
		       {   0, 0, 0} },
	rin_tech[] = { {   1, T_BLINK, 1},
		       {   0, 0, 0} },

	bos_tech[] = { {   1, T_FLURRY, 1},
		       {   5, T_VANISH, 1},
		       {   15, T_BLINK, 1},
		       {   30, T_TELEKINESIS, 1},
		       {   0, 0, 0} },

	ord_tech[] = { {   1, T_PRACTICE, 1},
		       {   1, T_TURN_UNDEAD, 1},
		       {   10, T_TINKER, 1},
		       {   0, 0, 0} },

	dun_tech[] = { {   1, T_CUTTHROAT, 1},
		       {   10, T_CRIT_STRIKE, 1},
		       {   20, T_RAISE_ZOMBIES, 1},
		       {   0, 0, 0} },

	alt_tech[] = { {   1, T_BLESSING, 1},
		       {   1, T_SIGIL_CONTROL, 1},
		       {   1, T_SIGIL_TEMPEST, 1},
		       {   1, T_SIGIL_DISCHARGE, 1},
		       {   1, T_DRAW_ENERGY, 1},
		       {   1, T_POWER_SURGE, 1},
		       {   10, T_E_FIST, 1},
		       {   10, T_CHI_STRIKE, 1},
		       {   20, T_BLITZ, 1},
		       {   25, T_REVIVE, 1},
		       {   0, 0, 0} },

	tha_tech[] = { {   5, T_DAZZLE, 1},
		       {   10, T_WARD_FIRE, 1},
		       {   10, T_DRAW_ENERGY, 1},
			 {   15, T_LIQUID_LEAP, 1},
			 {   15, T_WARD_ELEC, 1},
			 {   20, T_WARD_COLD, 1},
		       {   20, T_POWER_SURGE, 1},
		       {   0, 0, 0} },

	gof_tech[] = { {   1, T_DRAW_BLOOD, 1},
		       {   1, T_CALM_STEED, 1},
		       {   1, T_DAZZLE, 1},
		       {   1, T_TURN_UNDEAD, 1},
		       {   1, T_CREATE_AMMO, 1},
		       {   1, T_ATTIRE_CHARM, 1},
		       {   5, T_VANISH, 1},
		       {   10, T_FLURRY, 1},
		       {   15, T_REVIVE, 1},
		       {   20, T_RAISE_ZOMBIES, 1},
		       {   25, T_BOOZE, 1},
		       {   30, T_TELEKINESIS, 1},
		       {   0, 0, 0} },
	dru_tech[] = { {   1, T_BOOZE, 1},
		       {   0, 0, 0} },
	bar_tech[] = { {   1, T_BERSERK, 1},
		       {   0, 0, 0} },
	gol_tech[] = { {   1, T_VANISH, 1},
		       {   7, T_TINKER, 1},
		       {   0, 0, 0} },
	ble_tech[] = { {   1, T_SURGERY, 1},
		       {   1, T_HEAL_HANDS, 1},
		       {   1, T_DRAW_BLOOD, 1},
		       {   10, T_CHI_HEALING, 1},
		       {   20, T_REVIVE, 1},
		       {   0, 0, 0} },
	mid_tech[] = { {   1, T_RAGE, 1},
		       {   0, 0, 0} },
	spa_tech[] = { {   1, T_REINFORCE, 1},
			 {   5, T_RESEARCH, 1},
		       {  8, T_WARD_FIRE, 1},
		       {  10, T_CHARGE_SABER, 1},
		       {  11, T_WARD_COLD, 1},
		       {  14, T_WARD_ELEC, 1},
		       {  20, T_DRAW_ENERGY, 1},
		       {   0, 0, 0} },
	cam_tech[] = { {   12, T_CREATE_AMMO, 1},
		       {   0, 0, 0} },
	gan_tech[] = { {   1, T_CREATE_AMMO, 1},
		       {   0, 0, 0} },
	roc_tech[] = { {   1, T_EGG_BOMB, 1},
		       {   1, T_FLURRY, 1},
		       {   0, 0, 0} },
	dea_tech[] = { {   1, T_WORLD_FALL, 1},
		       {   0, 0, 0} },
	cav_tech[] = { {   1, T_PRIMAL_ROAR, 1},
		       {   0, 0, 0} },
	sli_tech[] = { {   1, T_LIQUID_LEAP, 1},
		       {   6, T_HEAL_HANDS, 1},
		       {   12, T_WORLD_FALL, 1},
		       {   15, T_TELEKINESIS, 1},
		       {   19, T_SUMMON_TEAM_ANT, 1},
		       {   25, T_VANISH, 1},
		       {   30, T_EGG_BOMB, 1},
		       {   0, 0, 0} },
	erd_tech[] = { {   1, T_CRIT_STRIKE, 1},
		       {   10, T_PRACTICE, 1},
		       {   20, T_ATTIRE_CHARM, 1},
		       {   0, 0, 0} },
	brd_tech[] = { {   1, T_PRIMAL_ROAR, 1},
		       {  10, T_BOOZE, 1},
		       {  20, T_RAGE, 1},
		       {   0, 0, 0} },
	aci_tech[] = { {   1, T_REINFORCE, 1},
		       {   3, T_POWER_SURGE, 1},
		       {   5, T_DRAW_ENERGY, 1},
		       {   7, T_SIGIL_CONTROL, 1},
		       {  10, T_SIGIL_TEMPEST, 1},
		       {  20, T_SIGIL_DISCHARGE, 1},
		       {   0, 0, 0} },
	fla_tech[] = { {   1, T_REINFORCE, 1},
		       {   3, T_POWER_SURGE, 1},
		       {   5, T_DRAW_ENERGY, 1},
		       {  10, T_SIGIL_TEMPEST, 1},
		       {  20, T_SIGIL_DISCHARGE, 1},
		       {   0, 0, 0} },
	ele_tech[] = { {   1, T_REINFORCE, 1},
		       {   3, T_POWER_SURGE, 1},
		       {   5, T_DRAW_ENERGY, 1},
		       {   7, T_SIGIL_CONTROL, 1},
		       {  10, T_SIGIL_TEMPEST, 1},
		       {  20, T_SIGIL_DISCHARGE, 1},
		       {   0, 0, 0} },
	psn_tech[] = { {   1, T_REINFORCE, 1},
		       {   1, T_SIGIL_CONTROL, 1},
		       {   1, T_SIGIL_TEMPEST, 1},
		       {   1, T_SIGIL_DISCHARGE, 1},
		       {   3, T_POWER_SURGE, 1},
		       {   5, T_DRAW_ENERGY, 1},
		       {   0, 0, 0} },
	hea_tech[] = { {   1, T_SURGERY, 1},
		       {  20, T_REVIVE, 1},
		       {   0, 0, 0} },
	ice_tech[] = { {   1, T_REINFORCE, 1},
		       {   5, T_DRAW_ENERGY, 1},
		       {  10, T_SIGIL_TEMPEST, 1},
		       {  12, T_POWER_SURGE, 1},
		       {  20, T_SIGIL_DISCHARGE, 1},
		       {   0, 0, 0} },
#ifdef JEDI
	jed_tech[] = { {   1, T_JEDI_JUMP, 1},
		       {   5, T_CHARGE_SABER, 1},
		       {   8, T_TELEKINESIS, 1},
		       {   0, 0, 0,} },
#endif
	kni_tech[] = { {   1, T_TURN_UNDEAD, 1},
		       {   1, T_HEAL_HANDS, 1},
		       {   0, 0, 0} },

	pal_tech[] = { {   1, T_TURN_UNDEAD, 1},
		       {   1, T_HEAL_HANDS, 1},
		       {   0, 0, 0} },

	wan_tech[] = { {   30, T_WORLD_FALL, 1},
		       {   0, 0, 0} },

	sci_tech[] = { {   1, T_RESEARCH, 1},
		       {   1, T_TINKER, 1},
		       {   10, T_SURGERY, 1},
		       {   20, T_SUMMON_TEAM_ANT, 1},
		       {   25, T_DRAW_ENERGY, 1},
		       {   30, T_EGG_BOMB, 1},
		       {   0, 0, 0} },
	sha_tech[] = { {   1, T_LIQUID_LEAP, 1},
		       {   0, 0, 0} },

	mon_tech[] = { {   1, T_PUMMEL, 1},
		       {   1, T_DASH, 1},
		       {   1, T_BLITZ, 1},
		       {   2, T_CHI_STRIKE, 1},
	  	       {   4, T_CHI_HEALING, 1},
	  	       {   6, T_E_FIST, 1},
		       {   8, T_DRAW_ENERGY, 1},
		       {  10, T_G_SLAM, 1},
		       {  11, T_WARD_FIRE, 1},
		       {  13, T_WARD_COLD, 1},
		       {  15, T_WARD_ELEC, 1},
		       {  17, T_SPIRIT_BOMB, 1},
		       {  20, T_POWER_SURGE, 1},
		       {   0, 0, 0} },

	sup_tech[] = { {   18, T_PUMMEL, 1},
		       {   18, T_DASH, 1},
		       {   18, T_BLITZ, 1},
		       {   18, T_CHI_STRIKE, 1},
	  	       {   18, T_CHI_HEALING, 1},
	  	       {   18, T_E_FIST, 1},
		       {  18, T_G_SLAM, 1},
		       {  18, T_SPIRIT_BOMB, 1},
		       {   0, 0, 0} },

	sai_tech[] = { {   1, T_PUMMEL, 1},
		       {   1, T_DASH, 1},
		       {   1, T_BLITZ, 1},
		       {   2, T_CHI_STRIKE, 1},
	  	       {   4, T_CHI_HEALING, 1},
	  	       {   6, T_E_FIST, 1},
		       {   8, T_DRAW_ENERGY, 1},
		       {  10, T_G_SLAM, 1},
		       {  11, T_WARD_FIRE, 1},
		       {  13, T_WARD_COLD, 1},
		       {  15, T_WARD_ELEC, 1},
		       {  17, T_SPIRIT_BOMB, 1},
		       {  20, T_POWER_SURGE, 1},
		       {   0, 0, 0} },

	psi_tech[] = { {   1, T_PUMMEL, 1},
		       {   1, T_DASH, 1},
		       {   2, T_CHI_STRIKE, 1},
	  	       {   4, T_CHI_HEALING, 1},
	  	       {   6, T_E_FIST, 1},
		       {   8, T_DRAW_ENERGY, 1},
		       {   8, T_TELEKINESIS, 1},
		       {  10, T_G_SLAM, 1},
		       {  11, T_WARD_FIRE, 1},
		       {  13, T_WARD_COLD, 1},
		       {  15, T_WARD_ELEC, 1},
		       {  15, T_ATTIRE_CHARM, 1},
		       {  17, T_SPIRIT_BOMB, 1},
		       {  20, T_POWER_SURGE, 1},
		       {   0, 0, 0} },
	pok_tech[] = { {   1, T_POKE_BALL, 1},
		       {   2, T_PUMMEL, 1},
		       {   4, T_DASH, 1},
		       {  5, T_WARD_FIRE, 1},
		       {   8, T_BLITZ, 1},
		       {  9, T_WARD_COLD, 1},
		       {  10, T_RAISE_ZOMBIES, 1},
		       {  13, T_WARD_ELEC, 1},
	  	       {   15, T_CREATE_AMMO, 1},
	  	       {   16, T_E_FIST, 1},
	  	       {   17, T_EGG_BOMB, 1},
	  	       {   18, T_WORLD_FALL, 1},
		       {   20, T_DRAW_ENERGY, 1},
		       {   22, T_RESEARCH, 1},
		       {  25, T_G_SLAM, 1},
		       {  26, T_SUMMON_TEAM_ANT, 1},
		       {  27, T_BLESSING, 1},
		       {  28, T_REVIVE, 1},
		       {  29, T_SPIRIT_BOMB, 1},
		       {  30, T_POWER_SURGE, 1},
		       {   0, 0, 0} },
	nec_tech[] = { {   1, T_REINFORCE, 1},
		       {   1, T_RAISE_ZOMBIES, 1},
		       {  10, T_POWER_SURGE, 1},
		       {  15, T_SIGIL_TEMPEST, 1},
		       {   0, 0, 0} },
	pri_tech[] = { {   1, T_TURN_UNDEAD, 1},
		       {   1, T_BLESSING, 1},
		       {   0, 0, 0} },
	che_tech[] = { {   1, T_TURN_UNDEAD, 1},
		       {   1, T_BLESSING, 1},
		       {   1, T_HEAL_HANDS, 1},
		       {   0, 0, 0} },
	ran_tech[] = { {   1, T_FLURRY, 1},
		       {   0, 0, 0} },
	aug_tech[] = { {   1, T_TELEKINESIS, 1},
		       {   0, 0, 0} },
	elp_tech[] = { {   1, T_FLURRY, 1},
		       {   0, 0, 0} },
	rog_tech[] = { {   1, T_CRIT_STRIKE, 1},
		       {  15, T_CUTTHROAT, 1},
		       {   0, 0, 0} },
	sam_tech[] = { {   1, T_KIII, 1},
		       {   0, 0, 0} },
	tou_tech[] = { /* Put Tech here */
		       {   0, 0, 0} },
	tra_tech[] = { {   1, T_ATTIRE_CHARM, 1},
		       {   0, 0, 0} },
	trs_tech[] = { {   1, T_ATTIRE_CHARM, 1},
		       {   0, 0, 0} },
	act_tech[] = { {   1, T_REINFORCE, 1},
		       {  5, T_VANISH, 1},
		       {  10, T_DAZZLE, 1},
		       {   0, 0, 0} },
	top_tech[] = { {   1, T_ATTIRE_CHARM, 1},
		       {  10, T_CREATE_AMMO, 1},
		       {  20, T_LIQUID_LEAP, 1},
		       {   0, 0, 0} },
	fai_tech[] = { {   1, T_ATTIRE_CHARM, 1},
		       {  29, T_CREATE_AMMO, 1},
		       {   0, 0, 0} },
	und_tech[] = { {   1, T_TURN_UNDEAD, 1},
		       {   1, T_PRACTICE, 1},
		       {   0, 0, 0} },
	unb_tech[] = { {   1, T_TURN_UNDEAD, 1},
		       {   1, T_PRACTICE, 1},
		       {   0, 0, 0} },
	unt_tech[] = { {   1, T_TURN_UNDEAD, 1},
		       {   0, 0, 0} },
	val_tech[] = { {   1, T_PRACTICE, 1},
		       {   0, 0, 0} },
	lun_tech[] = { {   1, T_EVISCERATE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {  10, T_BERSERK, 1},
		       {   0, 0, 0} },
#ifdef YEOMAN
	yeo_tech[] = {
#ifdef STEED
		       {   1, T_CALM_STEED, 1},
#endif
		       {   0, 0, 0} },
#endif
	wiz_tech[] = { {   1, T_REINFORCE, 1},
		       {   3, T_DRAW_ENERGY, 1},
		       {   5, T_POWER_SURGE, 1},
		       {   7, T_SIGIL_CONTROL, 1},
		       {  14, T_SIGIL_TEMPEST, 1},
		       {  20, T_SIGIL_DISCHARGE, 1},
		       {   0, 0, 0} },		       
	/* Races */
	arg_tech[] = { {   1, T_HEAL_HANDS, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   0, 0, 0} },
	coc_tech[] = { {   1, T_EGG_BOMB, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   0, 0, 0} },
	dop_tech[] = { {   1, T_LIQUID_LEAP, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   0, 0, 0} },
	dwa_tech[] = { {   1, T_RAGE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   0, 0, 0} },
	cur_tech[] = { /* Put Tech here */
		       {   1, T_BLESSING, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   0, 0, 0} },
	elf_tech[] = { /* Put Tech here */
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   0, 0, 0} },
	gno_tech[] = { {   1, T_VANISH, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   7, T_TINKER, 1},
		       {   0, 0, 0} },
	clk_tech[] = { {   1, T_FLURRY, 1},
		       {   1, T_VANISH, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_BOOZE, 1},
		       {   5, T_TINKER, 1},
		       {   7, T_SIGIL_DISCHARGE, 1},
		       {   10, T_BLINK, 1},
		       {   12, T_DRAW_ENERGY, 1},
		       {   15, T_SIGIL_TEMPEST, 1},
		       {   23, T_SIGIL_CONTROL, 1},
		       {   0, 0, 0} },
	ogr_tech[] = { {   1, T_FLURRY, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_BERSERK, 1},
		       {   15, T_PRIMAL_ROAR, 1},
		       {   20, T_CRIT_STRIKE, 1},
		       {   0, 0, 0} },
	alc_tech[] = { {   1, T_RESEARCH, 1},
		       {   1, T_TINKER, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_SURGERY, 1},
		       {   20, T_SUMMON_TEAM_ANT, 1},
		       {   25, T_DRAW_ENERGY, 1},
		       {   30, T_EGG_BOMB, 1},
		       {   0, 0, 0} },

	ung_tech[] = { {   1, T_WORLD_FALL, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_POKE_BALL, 1},
		       {   1, T_RESEARCH, 1},
		       {   1, T_PRACTICE, 1},
		       {   1, T_CALM_STEED, 1},
		       {   1, T_TURN_UNDEAD, 1},
		       {   1, T_BLESSING, 1},
		       {   1, T_DRAW_BLOOD, 1},
		       {   1, T_SURGERY, 1},
		       {   0, 0, 0} },
	hob_tech[] = { {   1, T_BLINK, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   0, 0, 0} },

	fen_tech[] = { {   1, T_EVISCERATE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_DISARM, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_VANISH, 1},
		       {   0, 0, 0} },

	alb_tech[] = { {   1, T_DAZZLE, 1},
		       {   1, T_PRACTICE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   5, T_CHARGE_SABER, 1},
		       {   10, T_CRIT_STRIKE, 1},
		       {   15, T_TELEKINESIS, 1},
		       {   20, T_BLINK, 1},
		       {   25, T_JEDI_JUMP, 1},
		       {   0, 0, 0} },

	nor_tech[] = { {   1, T_BERSERK, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_RAGE, 1},
		       {   0, 0, 0} },

	ins_tech[] = { {   1, T_SUMMON_TEAM_ANT, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   0, 0, 0} },
	kob_tech[] = { {   10, T_TINKER, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   0, 0, 0} },
	kha_tech[] = { {   1, T_EVISCERATE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   0, 0, 0} },
	gel_tech[] = { {   1, T_LIQUID_LEAP, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   0, 0, 0} },
	hmo_tech[] = { {   1, T_BERSERK, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   0, 0, 0} },
	lyc_tech[] = { {   1, T_EVISCERATE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {  10, T_BERSERK, 1},
		       {   0, 0, 0} },
	vam_tech[] = { {   1, T_DAZZLE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_DRAW_BLOOD, 1},
		       {   0, 0, 0} },
	lev_tech[] = { {   1, T_DAZZLE, 1},
		       {   1, T_VANISH, 1},
		       {   1, T_TELEKINESIS, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   5, T_ATTIRE_CHARM, 1},
		       {   10, T_BLINK, 1},
		       {   15, T_BLESSING, 1},
		       {   20, T_LIQUID_LEAP, 1},
		       {   25, T_DRAW_ENERGY, 1},
		       {   30, T_SUMMON_TEAM_ANT, 1},
		       {   0, 0, 0} },
	ang_tech[] = { {   1, T_DAZZLE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_TURN_UNDEAD, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   0, 0, 0} },
	hrb_tech[] = { {   1, T_SURGERY, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {  20, T_REVIVE, 1},
		       {   0, 0, 0} },
	mum_tech[] = { {   1, T_RAISE_ZOMBIES, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   0, 0, 0} },
	vor_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_VANISH, 1},
		       {   10, T_TELEKINESIS, 1},
		       {   15, T_EGG_BOMB, 1},
		       {   20, T_WORLD_FALL, 1},
		       {   0, 0, 0} },
	cor_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_VANISH, 1},
		       {   1, T_EGG_BOMB, 1},
		       {   10, T_TELEKINESIS, 1},
		       {   20, T_WORLD_FALL, 1},
		       {   0, 0, 0} },
	bor_tech[] = { {   3, T_JEDI_JUMP, 1},
		       {   10, T_CHARGE_SABER, 1},
		       {   20, T_TELEKINESIS, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_APPRAISAL, 1},
		       {   0, 0, 0,} },

	bat_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_PRACTICE, 1},
		       {   1, T_DRAW_BLOOD, 1},
		       {   0, 0, 0} },
	rod_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_DOUBLE_TROUBLE, 1},
		       {   0, 0, 0} },

	jel_tech[] = { {   1, T_DRAW_ENERGY, 1},
		       {   1, T_LIQUID_LEAP, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_APPRAISAL, 1},
		       {   0, 0, 0} },

	tur_tech[] = { {   1, T_PUMMEL, 1},
			 {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_DASH, 1},
		       {   1, T_BLITZ, 1},
		       {   2, T_CHI_STRIKE, 1},
	  	       {   4, T_CHI_HEALING, 1},
	  	       {   6, T_E_FIST, 1},
		       {   8, T_DRAW_ENERGY, 1},
		       {  10, T_G_SLAM, 1},
		       {  11, T_WARD_FIRE, 1},
		       {  13, T_WARD_COLD, 1},
		       {  15, T_WARD_ELEC, 1},
		       {  17, T_SPIRIT_BOMB, 1},
		       {  20, T_POWER_SURGE, 1},
		       {   0, 0, 0} },

	def_tech[] = { {   1, T_APPRAISAL, 1}, /* everyone is supposed to get this --Amy */
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   0, 0, 0} };
	/* Orc */

/* Local Macros 
 * these give you direct access to the player's list of techs.  
 * Are you sure you don't want to use tech_inuse,  which is the
 * extern function for checking whether a fcn is inuse
 */

#define techt_inuse(tech)       tech_list[tech].t_inuse
#define techtout(tech)        tech_list[tech].t_tout
#define techlev(tech)         (u.ulevel - tech_list[tech].t_lev)
#define techid(tech)          tech_list[tech].t_id
#define techname(tech)        (tech_names[techid(tech)])
#define techlet(tech)  \
        ((char)((tech < 26) ? ('a' + tech) : ('A' + tech - 26)))

/* A simple pseudorandom number generator
 *
 * This should generate fairly random numbers that will be 
 * mod LP_HPMOD from 2 to 9,  with 0 mod LP_HPMOD 
 * but can't use the normal RNG since can_limitbreak() must
 * return the same state on the same turn.
 * This also has to depend on things that do NOT change during 
 * save and restore,  and also should only change between turns
 */
#if 0 /* Probably overkill */
#define LB_CYCLE 259993L	/* number of turns before the pattern repeats */
#define LB_BASE1 ((long) (monstermoves + u.uhpmax + 300L))
#define LB_BASE2 ((long) (moves + u.uenmax + u.ulevel + 300L))
#define LB_STRIP 6	/* Remove the last few bits as they tend to be less random */
#endif
 
#define LB_CYCLE 101L	/* number of turns before the pattern repeats */
#define LB_BASE1 ((long) (monstermoves + u.uhpmax + 10L))
#define LB_BASE2 ((long) (moves + u.uenmax + u.ulevel + 10L))
#define LB_STRIP 3	/* Remove the last few bits as they tend to be less random */
 
#define LB_HPMOD ((long) ((u.uhp * 10 / u.uhpmax > 2) ? \
        			(u.uhp * 10 / u.uhpmax) : 2))

#define can_limitbreak() (!Upolyd && (u.uhp*10 < u.uhpmax) && \
        		  (u.uhp == 1 || (!((((LB_BASE1 * \
        		  LB_BASE2) % LB_CYCLE) >> LB_STRIP) \
        		  % LB_HPMOD))))
        
/* Whether you know the tech */
boolean
tech_known(tech)
	short tech;
{
	int i;
	for (i = 0; i < MAXTECH; i++) {
		if (techid(i) == tech) 
		     return TRUE;
	}
	return FALSE;
}

/* Called to prematurely stop a technique */
void
aborttech(tech)
int tech;
{
	int i;

	i = get_tech_no(tech);
	if (tech_list[i].t_inuse) {
	    switch (tech_list[i].t_id) {
		case T_RAGE:
		    u.uhpmax -= tech_list[i].t_inuse - 1;
		    if (u.uhpmax < 1)
			u.uhpmax = 0;
		    u.uhp -= tech_list[i].t_inuse - 1;
		    if (u.uhp < 1)
			u.uhp = 1;
		    break;
		case T_POWER_SURGE:
		    u.uenmax -= tech_list[i].t_inuse - 1;
		    if (u.uenmax < 1)
			u.uenmax = 0;
		    u.uen -= tech_list[i].t_inuse - 1;
		    if (u.uen < 0)
			u.uen = 0;
		    break;
	    }
	    tech_list[i].t_inuse = 0;
	}
}

/* Called to teach a new tech.  Level is starting tech level */
void
learntech(tech, mask, tlevel)
	short tech;
	long mask;
	int tlevel;
{
	int i;
	const struct innate_tech *tp;

	i = get_tech_no(tech);
	if (tlevel > 0) {
	    if (i < 0) {
		i = get_tech_no(NO_TECH);
		if (i < 0) {
		    impossible("No room for new technique?");
		    return;
		}
	    }
	    tlevel = u.ulevel ? u.ulevel - tlevel : 0;
	    if (tech_list[i].t_id == NO_TECH) {
		tech_list[i].t_id = tech;
		tech_list[i].t_lev = tlevel;
		tech_list[i].t_inuse = 0; /* not in use */
		tech_list[i].t_intrinsic = 0;
	    }
	    else if (tech_list[i].t_intrinsic & mask) {
		impossible("Tech already known.");
		return;
	    }
	    if (mask == FROMOUTSIDE) {
		tech_list[i].t_intrinsic &= ~OUTSIDE_LEVEL;
		tech_list[i].t_intrinsic |= tlevel & OUTSIDE_LEVEL;
	    }
	    if (tlevel < tech_list[i].t_lev)
		tech_list[i].t_lev = tlevel;
	    tech_list[i].t_intrinsic |= mask;
	    tech_list[i].t_tout = 0; /* Can use immediately*/
	}
	else if (tlevel < 0) {
	    if (i < 0 || !(tech_list[i].t_intrinsic & mask)) {
		impossible("Tech not known.");
		return;
	    }
	    tech_list[i].t_intrinsic &= ~mask;
	    if (!(tech_list[i].t_intrinsic & INTRINSIC)) {
		if (tech_list[i].t_inuse)
		    aborttech(tech);
		tech_list[i].t_id = NO_TECH;
		return;
	    }
	    /* Re-calculate lowest t_lev */
	    if (tech_list[i].t_intrinsic & FROMOUTSIDE)
		tlevel = tech_list[i].t_intrinsic & OUTSIDE_LEVEL;
	    if (tech_list[i].t_intrinsic & FROMEXPER) {
		for(tp = role_tech(); tp->tech_id; tp++)
		    if (tp->tech_id == tech)
			break;
		if (!tp->tech_id)
		    impossible("No inate technique for role?");
		else if (tlevel < 0 || tp->ulevel - tp->tech_lev < tlevel)
		    tlevel = tp->ulevel - tp->tech_lev;
	    }
	    if (tech_list[i].t_intrinsic & FROMRACE) {
		for(tp = race_tech(); tp->tech_id; tp++)
		    if (tp->tech_id == tech)
			break;
		if (!tp->tech_id)
		    impossible("No inate technique for race?");
		else if (tlevel < 0 || tp->ulevel - tp->tech_lev < tlevel)
		    tlevel = tp->ulevel - tp->tech_lev;
	    }
	    tech_list[i].t_lev = tlevel;
	}
	else
	    impossible("Invalid Tech Level!");
}

/*
 * Return TRUE if a tech was picked, with the tech index in the return
 * parameter.  Otherwise return FALSE.
 */
static boolean
gettech(tech_no)
        int *tech_no;
{
        int i, ntechs, idx;
	char ilet, lets[BUFSZ], qbuf[QBUFSZ];

	for (ntechs = i = 0; i < MAXTECH; i++)
	    if (techid(i) != NO_TECH) ntechs++;
	if (ntechs == 0)  {
            You("don't know any techniques right now.");
	    return FALSE;
	}
	if (flags.menu_style == MENU_TRADITIONAL) {
            if (ntechs == 1)  Strcpy(lets, "a");
            else if (ntechs < 27)  Sprintf(lets, "a-%c", 'a' + ntechs - 1);
            else if (ntechs == 27)  Sprintf(lets, "a-z A");
            else Sprintf(lets, "a-z A-%c", 'A' + ntechs - 27);

	    for(;;)  {
                Sprintf(qbuf, "Perform which technique? [%s ?]", lets);
		if ((ilet = yn_function(qbuf, (char *)0, '\0')) == '?')
		    break;

		if (index(quitchars, ilet))
		    return FALSE;

		if (letter(ilet) && ilet != '@') {
		    /* in a-zA-Z, convert back to an index */
		    if (lowc(ilet) == ilet)     /* lower case */
			idx = ilet - 'a';
		    else
			idx = ilet - 'A' + 26;

                    if (idx < ntechs)
			for(i = 0; i < MAXTECH; i++)
			    if (techid(i) != NO_TECH) {
				if (idx-- == 0) {
				    *tech_no = i;
				    return TRUE;
				}
			    }
		}
                You("don't know that technique.");
	    }
	}
        return dotechmenu(PICK_ONE, tech_no);
}

static boolean
dotechmenu(how, tech_no)
	int how;
        int *tech_no;
{
	winid tmpwin;
	int i, n, len, longest, techs_useable, tlevel;
	char buf[BUFSZ], let = 'a';
	const char *prefix;
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;         /* zero out all bits */

	techs_useable = 0;

	if (!iflags.menu_tab_sep) {
	    /* find the length of the longest tech */
	    for (longest = 0, i = 0; i < MAXTECH; i++) {
		if (techid(i) == NO_TECH) continue;
		if ((len = strlen(techname(i))) > longest)
		    longest = len;
	    }
	    Sprintf(buf, "    %-*s Level   Status", longest, "Name");
	} else
	    Sprintf(buf, "Name\tLevel\tStatus");

	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, MENU_UNSELECTED);

	for (i = 0; i < MAXTECH; i++) {
	    if (techid(i) == NO_TECH)
		continue;
	    tlevel = techlev(i);
	    if (!techtout(i) && tlevel > 0) {
		/* Ready to use */
		techs_useable++;
		prefix = "";
		any.a_int = i + 1;
	    } else {
		prefix = "    ";
		any.a_int = 0;
	    }
#ifdef WIZARD
	    if (wizard) 
		if (!iflags.menu_tab_sep)			
		    Sprintf(buf, "%s%-*s %2d%c%c%c   %s(%i)",
			    prefix, longest, techname(i), tlevel,
			    tech_list[i].t_intrinsic & FROMEXPER ? 'X' : ' ',
			    tech_list[i].t_intrinsic & FROMRACE ? 'R' : ' ',
			    tech_list[i].t_intrinsic & FROMOUTSIDE ? 'O' : ' ',
			    tech_inuse(techid(i)) ? "Active" :
			    tlevel <= 0 ? "Beyond recall" :
			    can_limitbreak() ? "LIMIT" :
			    !techtout(i) ? "Prepared" : 
			    techtout(i) > 10000 ? "Huge timeout" :
			    techtout(i) > 1000 ? "Not Ready" :
			    techtout(i) > 100 ? "Reloading" : "Soon",
			    techtout(i));
		else
		    Sprintf(buf, "%s%s\t%2d%c%c%c\t%s(%i)",
			    prefix, techname(i), tlevel,
			    tech_list[i].t_intrinsic & FROMEXPER ? 'X' : ' ',
			    tech_list[i].t_intrinsic & FROMRACE ? 'R' : ' ',
			    tech_list[i].t_intrinsic & FROMOUTSIDE ? 'O' : ' ',
			    tech_inuse(techid(i)) ? "Active" :
			    tlevel <= 0 ? "Beyond recall" :
			    can_limitbreak() ? "LIMIT" :
			    !techtout(i) ? "Prepared" : 
			    techtout(i) > 10000 ? "Huge timeout" :
			    techtout(i) > 1000 ? "Not Ready" :
			    techtout(i) > 100 ? "Reloading" : "Soon",
			    techtout(i));
	    else
#endif
	    if (!iflags.menu_tab_sep)			
		Sprintf(buf, "%s%-*s %5d   %s",
			prefix, longest, techname(i), tlevel,
			tech_inuse(techid(i)) ? "Active" :
			tlevel <= 0 ? "Beyond recall" :
			can_limitbreak() ? "LIMIT" :
			!techtout(i) ? "Prepared" : 
			techtout(i) > 10000 ? "Huge timeout" :
			techtout(i) > 1000 ? "Not Ready" :
			techtout(i) > 100 ? "Reloading" : "Soon");
	    else
		Sprintf(buf, "%s%s\t%5d\t%s",
			prefix, techname(i), tlevel,
			tech_inuse(techid(i)) ? "Active" :
			tlevel <= 0 ? "Beyond recall" :
			can_limitbreak() ? "LIMIT" :
			!techtout(i) ? "Prepared" : 
			techtout(i) > 10000 ? "Huge timeout" :
			techtout(i) > 1000 ? "Not Ready" :
			techtout(i) > 100 ? "Reloading" : "Soon");

	    add_menu(tmpwin, NO_GLYPH, &any,
		    techtout(i) ? 0 : let, 0, ATR_NONE, buf, MENU_UNSELECTED);
	    if (let++ == 'z') let = 'A';
	}

	if (!techs_useable) 
	    how = PICK_NONE;

	end_menu(tmpwin, how == PICK_ONE ? "Choose a technique" :
					   "Currently known techniques");

	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	if (n > 0) {
	    *tech_no = selected[0].item.a_int - 1;
	    free((genericptr_t)selected);
	    return TRUE;
	}
	return FALSE;
}

#ifdef DUMP_LOG
void 
dump_techniques()
{

	winid tmpwin;
	int i, n, len, tlevel;
	char buf[BUFSZ];
	const char *prefix;

	if (techid(0) == NO_TECH) {
	    dump("", "You didn't know any techniques.");
	    dump("", "");
	    return;
	}
	dump("", "Techniques known in the end");

	Sprintf(buf, "    %-17s Level  Status", "Name");
	dump("  ",buf);
	for (i = 0; i < MAXTECH; i++) {
	    if (techid(i) == NO_TECH)
		continue;
	    tlevel = techlev(i);
	    if (!techtout(i) && tlevel > 0) {
		/* Ready to use */
		prefix = "";
	    } else {
		prefix = "    ";
	    }
		if (!iflags.menu_tab_sep) {		
		    Sprintf(buf, "%s%-20s %2d%c%c%c   %s(%i)",
			    prefix, techname(i), tlevel,
			    tech_list[i].t_intrinsic & FROMEXPER ? 'X' : ' ',
			    tech_list[i].t_intrinsic & FROMRACE ? 'R' : ' ',
			    tech_list[i].t_intrinsic & FROMOUTSIDE ? 'O' : ' ',
			    tech_inuse(techid(i)) ? "Active" :
			    tlevel <= 0 ? "Beyond recall" :
			    can_limitbreak() ? "LIMIT" :
			    !techtout(i) ? "Prepared" : 
			    techtout(i) > 10000 ? "Huge timeout" :
			    techtout(i) > 1000 ? "Not Ready" :
			    techtout(i) > 100 ? "Reloading" : "Soon",
			    techtout(i));
			dump("  ", buf);
		} else {
		    Sprintf(buf, "%s%s\t%2d%c%c%c\t%s(%i)",
			    prefix, techname(i), tlevel,
			    tech_list[i].t_intrinsic & FROMEXPER ? 'X' : ' ',
			    tech_list[i].t_intrinsic & FROMRACE ? 'R' : ' ',
			    tech_list[i].t_intrinsic & FROMOUTSIDE ? 'O' : ' ',
			    tech_inuse(techid(i)) ? "Active" :
			    tlevel <= 0 ? "Beyond recall" :
			    can_limitbreak() ? "LIMIT" :
			    !techtout(i) ? "Prepared" : 
			    techtout(i) > 10000 ? "Huge timeout" :
			    techtout(i) > 1000 ? "Not Ready" :
			    techtout(i) > 100 ? "Reloading" : "Soon",
			    techtout(i));
			dump("  ", buf);
	    } 
	}
      dump("", "");

} /* dump_techniques */
#endif

static int
get_tech_no(tech)
int tech;
{
	int i;

	for (i = 0; i < MAXTECH; i++) {
		if (techid(i) == tech) {
			return(i);
		}
	}
	return (-1);
}

int
dotech()
{
	int tech_no;

	if (flags.tech_description) {

	if (gettech(&tech_no)) {

		switch (techid(tech_no)) {

		case T_RESEARCH:
			pline("This technique sometimes allow you to identify some of your items. Higher intelligence and wisdom increase the chance of success, higher technique levels increase the average amount of stuff identified.");
			break;

		case T_EVISCERATE:
			pline("You can only use this technique while bare-handed and not polymorphed. If you do, your unarmed attacks do more damage for a while; the technique's level determines the duration of this effect.");
			break;

		case T_BERSERK:
			pline("This technique allows you to do stronger attacks for a while; the technique's level determines the duration of this effect.");
			break;

		case T_REINFORCE:
			pline("Using this technique allows you to reinforce your memory of an almost-forgotten spell.");
			break;

		case T_FLURRY:
			pline("A technique that can be used to get a multishot bonus with bows and certain other weapons, the duration of which depends on the technique's level.");
			break;

		case T_INVOKE_DEITY:
			pline("Invoke your deity for healing. If successful, some or all of your hit point damage will be restored, but there may be negative outcomes depending on certain circumstances.");
			break;

		case T_DOUBLE_TROUBLE:
			pline("Create a clone of yourself with this technique. At higher technique levels you might summon some familiars too.");
			break;

		case T_APPRAISAL:
			pline("This technique allows you to determine the enchantment value of your currently wielded weapon. If you use it while wielding a non-weapon (e.g. a wand), the timeout of this technique will be much longer.");
			break;

		case T_PHASE_DOOR:
			pline("If you need to get out of trouble, you can use this tech for a short-range teleport. It will teleport you over an euclidean distance of at least 3 but no more than 100.");
			break;

		case T_PRACTICE:
			pline("With this technique, you can practice with your currently wielded weapon, sometimes determining the enchantment value. Also, it gets you closer to increasing your skill level, but monsters may interrupt you.");
			break;

		case T_SURGERY:
			pline("A technique that can be used to cure a variety of afflictions, but you should have either a scalpel or a medical kit filled with bandages for it to work.");
			break;

		case T_HEAL_HANDS:
			pline("Using this technique can cure sliming or sickness, or heal a techlevel-dependant amount of hit points.");
			break;

		case T_KIII:
			pline("If you use this technique, your melee attacks do much more damage for a period of time, the length of which depends on the technique's level.");
			break;

		case T_CALM_STEED:
			pline("You need to be riding in order to use this technique. If you do, your steed's tameness will increase; higher technique levels increase the average amount of tameness points gained.");
			break;

		case T_TURN_UNDEAD:
			pline("Turn undead is the act of causing nearby undead creatures to flee; weak undead might even be killed outright by this effect.");
			break;

		case T_VANISH:
			pline("By using this technique, you will become invisible and fast for some time. Higher technique levels give a longer effect.");
			break;

		case T_CRIT_STRIKE:
			pline("This technique allows you to perform a powerful attack against a monster close by, the strength of which depends on the technique's level.");
			break;

		case T_CUTTHROAT:
			pline("You need a blade to use this technique. Doing so allows you to severely hurt an adjacent non-headless monster; high technique levels sometimes even allow you to instakill the monster.");
			break;

		case T_BLESSING:
			pline("Using this technique allows you to uncurse a cursed item or bless an uncursed one.");
			break;

		case T_E_FIST:
			pline("This technique enhances your melee attacks with elemental powers for a techlevel-dependant amount of turns.");
			break;

		case T_PRIMAL_ROAR:
			pline("A technique that temporarily promotes your pets to their grown-up form, but after a techlevel-dependent time they will revert.");
			break;

		case T_LIQUID_LEAP:
			pline("This allows you to jump in a direction of your choice, passing through monsters, iron bars and similar stuff. The technique's level affects the damage done to monsters you jump through.");
			break;

		case T_SIGIL_TEMPEST:
			pline("This technique costs quite some mana, but if used, your ray spells will create 3x3 explosions around you as long as the technique is in effect. The duration increases with the technique's level. Combine it with sigil of control to control the explosions.");
			break;

		case T_SIGIL_CONTROL:
			pline("This technique costs quite some mana, but if used, you can control the direction of your ray spells' rebounds for a while. The duration increases with the technique's level.");
			break;

		case T_SIGIL_DISCHARGE:
			pline("This technique costs quite some mana, but if used, your ray spells will create huge explosions that can hit lots of monsters in a single shot as long as it's active. The duration increases with the technique's level.");
			break;

		case T_RAISE_ZOMBIES:
			pline("This technique searches the squares adjacent to you for corpses, trying to make tame zombies out of them.");
			break;

		case T_REVIVE:
			pline("Allows you to revive a corpse from the floor or your inventory, but at the cost of health. This cost is reduced by increasing the technique's level.");
			break;

		case T_WARD_FIRE:
			pline("Using this technique makes you resistant to fire for a while (longer with higher technique levels).");
			break;

		case T_WARD_COLD:
			pline("Using this technique makes you resistant to cold for a while (longer with higher technique levels).");
			break;

		case T_WARD_ELEC:
			pline("Using this technique makes you resistant to shock for a while (longer with higher technique levels).");
			break;

		case T_TINKER:
			pline("You must hold an item in your hand that can be upgraded if you want to use this technique effectively. Also, it takes a very long time to upgrade an object; the duration is decreased slightly if the technique's level is lower, but monsters are still likely to interrupt you.");
			break;

		case T_RAGE:
			pline("Temporarily increases your maximum hit points by a techlevel-dependant amount, but your HP will slowly bleed out until they reach their previous value, so be careful.");
			break;

		case T_BLINK:
			pline("Using this technique allows you to move super-fast for a period of time, the length of which increases with the technique's level.");
			break;

		case T_CHI_STRIKE:
			pline("A technique that uses your mana in order to enhance your melee attacks.");
			break;

		case T_DRAW_ENERGY:
			pline("Draws energy (mana) from your surroundings for 15 turns; if altars, thrones or similar stuff are on the squares adjacent to you, the effect will be stronger. However, if a monster interrupts you, you'll get nothing at all.");
			break;

		case T_CHI_HEALING:
			pline("Your mana will be converted to health for a period of time if you use this technique.");
			break;

		case T_DISARM:
			pline("Tries to disarm a monster; you need to be holding a weapon you're at least skilled with, though.");
			break;

		case T_DAZZLE:
			pline("A technique that can stun a monster and cause it to flee; it is much more likely to succeed if the target monster's level is lower than the technique's level.");
			break;

		case T_BLITZ:
			pline("Allows you to input several successive commands that will be converted into moves. You can always do at least 2 moves; 3 at techlevel 10 or higher, 4 at techlevel 20 or higher, and 5 at techlevel 30.");
			break;

		case T_PUMMEL:
			pline("This technique can be used to rapidly strike an adjacent monster using your fists.");
			break;

		case T_G_SLAM:
			pline("A technique that can be used to slam an adjacent monster into the ground, creating a pit and doing lots of damage.");
			break;

		case T_DASH:
			pline("You can use this technique to move by several squares in a single turn, but only in a straight line.");
			break;

		case T_POWER_SURGE:
			pline("A very powerful technique that temporarily increases your maximum mana to a very high amount, but it slowly bleeds off to its original value.");
			break;

		case T_SPIRIT_BOMB:
			pline("This technique allows you to do a 'kamehameha' attack against a monster, causing it to explode while also damaging other monsters nearby. You can't hit yourself or your pets with this attack either.");
			break;

		case T_DRAW_BLOOD:
			pline("A nearly useless technique that will use a medical kit to drain your experience for a single potion of blood.");
			break;

		case T_JEDI_JUMP:
			pline("Uses mana to jump. Higher technique levels allow you to jump farther.");
			break;

		case T_POKE_BALL:
			pline("This technique allows you to throw a poke ball, trying to catch any adjacent non-unique monster. Higher technique levels give a greater chance of success and sometimes allow you to catch several monsters in a single turn. Actual pokemon are more likely to be caught, and 'petty' monsters are guaranteed to be caught.");
			break;

		case T_SUMMON_TEAM_ANT:
			pline("A technique that summons tame Team Ant members (monsters represented by the letter a). Higher technique levels are more likely to summon several of them at once.");
			break;

		case T_ATTIRE_CHARM:
			pline("With this technique, you can attempt to tame adjacent intelligent human(oid) monsters, but you need to be wearing high heels for it to work. A higher level of this technique sometimes tames several monsters at once.");
			break;

		case T_WORLD_FALL:
			pline("A very powerful technique that will instakill all non-unique monsters on the current dungeon level whose monster level is lower than the technique's level. Sometimes it will also kill higher-level monsters.");
			break;

		case T_CREATE_AMMO:
			pline("Creates a techlevel-dependant amount of bullets.");
			break;

		case T_EGG_BOMB:
			pline("This technique creates stoning grenades at your feet. Higher techlevels create more of them at once on average.");
			break;

		case T_BOOZE:
			pline("Using this technique is very similar to quaffing a potion of booze, but it will also create some potions of booze out of thin air. The technique's level determines the average amount of potions created.");
			break;

		case T_TELEKINESIS:
			pline("A technique that can be used to interact with dungeon features from far away.");
			break;

		case T_CHARGE_SABER:
			pline("This technique will use up all of your current mana and convert it into lightsaber energy. If the techlevel is at least 10, you can also win the jackpot. However, monsters may interrupt you, preventing your lightsaber from actually being charged.");
			break;

		default:
			pline("This technique doesn't have a description yet, but it might get one in future. --Amy");
			break;
		}

	    if (yn("Use this technique?") == 'y') return techeffects(tech_no);
	}

	return 0;

	} else if (gettech(&tech_no)) {
		return techeffects(tech_no);
	}
	return 0;

}

static NEARDATA const char kits[] = { TOOL_CLASS, 0 };

static struct obj *
use_medical_kit(type, feedback, verb)
int type;
boolean feedback;
char *verb;
{
    struct obj *obj, *otmp;
    makeknown(MEDICAL_KIT);
    if (!(obj = carrying(MEDICAL_KIT))) {
	if (feedback) You("need a medical kit to do that.");
	return (struct obj *)0;
    }
    for (otmp = invent; otmp; otmp = otmp->nobj)
	if (otmp->otyp == MEDICAL_KIT && otmp != obj)
	    break;
    if (otmp) {	/* More than one medical kit */
	obj = getobj(kits, verb);
	if (!obj)
	    return (struct obj *)0;
    }
    for (otmp = obj->cobj; otmp; otmp = otmp->nobj)
	if (otmp->otyp == type)
	    break;
    if (!otmp) {
	if (feedback)
	    You_cant("find any more %s in %s.",
		    makeplural(simple_typename(type)), yname(obj));
	return (struct obj *)0;
    }
    return otmp;
}

/* gettech is reworked getspell */
/* reworked class special effects code */
/* adapted from specialpower in cmd.c */
static int
techeffects(tech_no)
int tech_no;
{
	/* These variables are used in various techs */
	struct obj *obj, *otmp;
	const char *str;
	struct monst *mtmp;
	int num;
	char Your_buf[BUFSZ];
	char allowall[2];
	int i, j, t_timeout = 0;


	/* check timeout */
	if (tech_inuse(techid(tech_no))) {
	    pline("This technique is already active!");
	    return (0);
	}
        if (techtout(tech_no) && !can_limitbreak()) {
	    You("have to wait %s before using your technique again.",
                (techtout(tech_no) > 100) ?
                        "for a while" : "a little longer");
#ifdef WIZARD
            if (!wizard || (yn("Use technique anyways?") == 'n'))
#endif
                return(0);
        }

	if (TechTrapEffect || u.uprops[TECHBUG].extrinsic || have_techniquestone()) {

		pline("Unfortunately, nothing happens.");
		techtout(tech_no) = rnz(5000);
		if (ishaxor && techtout(tech_no) > 1) techtout(tech_no) /= 2;
		/*By default,  action should take a turn*/
		return(1);

	}

	if (Role_if(PM_FAILED_EXISTENCE) && rn2(2)) {

		pline("Unfortunately, nothing happens.");
		techtout(tech_no) = rnz(1000);
		if (ishaxor && techtout(tech_no) > 1) techtout(tech_no) /= 2;
		/*By default,  action should take a turn*/
		return(1);

	}

	/* switch to the tech and do stuff */
        switch (techid(tech_no)) {
            case T_RESEARCH:
		/* WAC stolen from the spellcasters...'A' can identify from
        	   historical research*/
		if(Hallucination || (Stunned && !Stun_resist) || (Confusion && !Conf_resist) ) {
		    You("can't concentrate right now!");
		    return(0);
		} else if((ACURR(A_INT) + ACURR(A_WIS)) < rnd(60)) {
			pline("Nothing in your pack looks familiar.");
                    t_timeout = rnz(1000);
		    break;
		} else if(invent) {
			You("examine your possessions.");
			identify_pack((int) ((techlev(tech_no) / 10) + 1), 0);
		} else {
			/* KMH -- fixed non-compliant string */
		    You("are already quite familiar with the contents of your pack.");
		    break;
		}
                t_timeout = rnz(2000);
		break;
            case T_EVISCERATE:
		/* only when empty handed, in human form */
		if (Upolyd || uwep || uarmg) {
		    You_cant("do this while %s!", Upolyd ? "polymorphed" :
			    uwep ? "holding a weapon" : "wearing gloves");
		    return 0;
		}
		Your("fingernails extend into claws!");
		aggravate();
		techt_inuse(tech_no) = d(2,4) + techlev(tech_no)/5 + 2;
		t_timeout = rnz(2000);
		break;
            case T_BERSERK:
		You("fly into a berserk rage!");
		techt_inuse(tech_no) = d(2,8) +
               		(techlev(tech_no)/5) + 2;
		incr_itimeout(&HFast, techt_inuse(tech_no));
		t_timeout = rnz(1500);
		break;
            case T_REINFORCE:
		/* WAC spell-users can study their known spells*/
		if(Hallucination || (Stunned && !Stun_resist) || (Confusion && !Conf_resist) ) {
		    You("can't concentrate right now!");
		    break;
               	} else {
		    You("concentrate...");
		    if (studyspell()) t_timeout = rnz(1500); /*in spell.c*/
		}
               break;
            case T_FLURRY:
                Your("%s %s become blurs as they reach for your quiver!",
			uarmg ? "gloved" : "bare",      /* Del Lamb */
			makeplural(body_part(HAND)));
                techt_inuse(tech_no) = rnd((int) (techlev(tech_no)/6 + 1)) + 2;
                t_timeout = rnz(1500);
		break;
            case T_INVOKE_DEITY: /* ask for healing if your alignment record is positive --Amy */

			if (u.ualign.record < 0) {

				if ( (Inhell && !Race_if(PM_HERETIC) ) || flags.gehenna ) {
					pline("%s is inaccessible, and Moloch decides to smite you!",u_gname() );
					u.ublesscnt += rnz(-u.ualign.record);
					losehp(rnz(-u.ualign.record), "annoying Moloch", KILLED_BY);
				} else {
					pline("%s smites you for your sins!",u_gname() );
					u.ublesscnt += rnz(-u.ualign.record);
					losehp(rnz(-u.ualign.record), "disturbing their deity", KILLED_BY);
				}
/* If your deity feels annoyed, they will damage you and increase your prayer timeout. They won't get angry though. */
			} 

			else if (Race_if(PM_IMPERIAL)) {
				pline("%s hates you and decides you need to be punished!",u_gname() );
				u.ublesscnt += rnz(monster_difficulty() + 1 );
				losehp(rnz(monster_difficulty() + 1 ), "being a pesky heretic", KILLED_BY);
/* Imperials cannot use this technique successfully. */
			} 

			else if ( (Inhell && !Race_if(PM_HERETIC) ) || flags.gehenna ) {
				pline("%s is inaccessible, and Moloch decides to smite you!",u_gname() );
				u.ublesscnt += rnz(monster_difficulty() + 1 );
				losehp(rnz(monster_difficulty() + 1 ), "trying to contact their deity in Gehennom", KILLED_BY);
/* Trying to invoke a deity in Gehennom is never a good idea... */
			} 

			else if (u.ualign.record > 0) {
				pline("%s commends your efforts and grants you a boon.",u_gname() );
				u.ublesscnt -= rnz(u.ualign.record + techlev(tech_no) );
				if(u.ublesscnt < 0) u.ublesscnt = 0;
				healup( rnz(u.ualign.record + techlev(tech_no)) , 0, FALSE, FALSE);
/* If your deity is pleased with you, heal some damage and decrease your prayer timeout. */
			}

			else { /* alignment record exactly 0, do nothing */
				pline("%s ignores your pleadings.",u_gname() );
			}

			u.uconduct.gnostic++;	/* you just tried to access your god */
                t_timeout = rnz(3000);
		break;
            case T_APPRAISAL:
			if(!uwep) {
		    You("are not wielding anything!");
		    return(0);
		} else if (weapon_type(uwep) == P_NONE) {
                        You("examine %s.", doname(uwep));
                                uwep->known = TRUE;
                                You("discover it is %s",doname(uwep));
                t_timeout = rnz(2000);
		} else {
                        You("examine %s.", doname(uwep));
                                uwep->known = TRUE;
                                You("discover it is %s",doname(uwep));
                t_timeout = rnz(200);
		}
		break;

            case T_PHASE_DOOR:
			phase_door(0);
                if (!Role_if(PM_CAMPERSTRIKER) || !rn2(4)) t_timeout = rnz(1000);
		break;

            case T_PRACTICE:
                if(!uwep || (weapon_type(uwep) == P_NONE)) {
		    You("are not wielding a weapon!");
		    return(0);
		} else if(uwep->known == TRUE) {
                    practice_weapon();
		} else {
                    if (not_fully_identified(uwep)) {
                        You("examine %s.", doname(uwep));
                            if (rnd(15) <= ACURR(A_INT)) {
                                makeknown(uwep->otyp);
                                uwep->known = TRUE;
                                You("discover it is %s",doname(uwep));
                                } else
                     pline("Unfortunately, you didn't learn anything new.");
                    } 
                /*WAC Added practicing code - in weapon.c*/
                    practice_weapon();
		}
                t_timeout = rnz(1000);
		break;
            case T_SURGERY:
		if (Hallucination || (Stunned && !Stun_resist) || (Confusion && !Conf_resist) ) {
		    You("are in no condition to perform surgery!");
		    break;
		}
		if (Sick || Slimed) {
		    if (carrying(SCALPEL)) {
			pline("Using your scalpel (ow!), you cure your infection!");
			make_sick(0L, (char *)0, TRUE, SICK_ALL);
			Slimed = 0;
			if (Upolyd) {
			    u.mh -= 5;
			    if (u.mh < 1)
				rehumanize();
			} else if (u.uhp > 6)
			    u.uhp -= 5;
			else
			    u.uhp = 1;
                        t_timeout = rnz(1000);
			flags.botl = TRUE;
			break;
		    } else pline("If only you had a scalpel...");
		}
		if (Upolyd ? u.mh < u.mhmax : u.uhp < u.uhpmax) {
		    otmp = use_medical_kit(BANDAGE, FALSE,
			    "dress your wounds with");
		    if (otmp) {
			check_unpaid(otmp);
			if (otmp->quan > 1L) {
			    otmp->quan--;
			    otmp->ocontainer->owt = weight(otmp->ocontainer);
			} else {
			    obj_extract_self(otmp);
			    obfree(otmp, (struct obj *)0);
			}
			pline("Using %s, you dress your wounds.", yname(otmp));
			healup(techlev(tech_no) * (rnd(2)+1) + rn1(5,5),
			  0, FALSE, FALSE);
		    } else {
			You("strap your wounds as best you can.");
			healup(techlev(tech_no) + rn1(5,5), 0, FALSE, FALSE);
		    }
                    t_timeout = rnz(1500);
		    flags.botl = TRUE;
		} else You("don't need your healing powers!");
		break;
            case T_HEAL_HANDS:
		if (Slimed) {
		    Your("body is on fire!");
		    burn_away_slime();
		    t_timeout = rnz(3000);
		} else if (Sick) {
		    You("lay your hands on the foul sickness...");
		    make_sick(0L, (char*)0, TRUE, SICK_ALL);
		    t_timeout = rnz(3000);
		} else if (Upolyd ? u.mh < u.mhmax : u.uhp < u.uhpmax) {
		    pline("A warm glow spreads through your body!");
		    healup(techlev(tech_no) * 4, 0, FALSE, FALSE);
		    t_timeout = rnz(3000);
		} else
		    pline(nothing_happens);
		break;
            case T_KIII:
		You("scream \"KIIILLL!\"");
		aggravate();
                techt_inuse(tech_no) = rnd((int) (techlev(tech_no)/6 + 1)) + 2;
                t_timeout = rnz(1500);
		break;
#ifdef STEED
	    case T_CALM_STEED:

		if (u.usteed) pline("You calm your steed.");
		int calmedX;
		calmedX = 0;

                if (u.usteed) {


                        pline("%s gets tamer.", Monnam(u.usteed));
                        tamedog(u.usteed, (struct obj *) 0, FALSE);

				while (calmedX == 0) { /* remove the stupid bug that caused this tech to do nothing --Amy */

				if (u.usteed->mtame < 20) u.usteed->mtame++;

				if (techlev(tech_no) < rnd(50)) calmedX++; /* high level tech has high chance of extra tameness */

				}

                        t_timeout = rnz(1500);
                } else
                        Your("technique is only effective when riding a monster.");
                break;
#endif
            case T_TURN_UNDEAD:
                return(turn_undead());
	    case T_VANISH:
		if (Invisible && Fast) {
			You("are already quite nimble and undetectable.");
		}
                techt_inuse(tech_no) = rn1(50,50) + techlev(tech_no);
		if (!Invisible) pline("In a puff of smoke,  you disappear!");
		if (!Fast) You("feel more nimble!");
		incr_itimeout(&HInvis, techt_inuse(tech_no));
		incr_itimeout(&HFast, techt_inuse(tech_no));
		newsym(u.ux,u.uy);      /* update position */
		t_timeout = rnz(1500);
		break;
	    case T_CRIT_STRIKE:
		if (!getdir((char *)0)) return(0);
		if (!u.dx && !u.dy) {
		    /* Hopefully a mistake ;B */
		    You("decide against that idea.");
		    return(0);
		}
		mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
		if (!mtmp) {
		    You("perform a flashy twirl!");
		    return (0);
		} else {
		    int oldhp = mtmp->mhp;
		    int tmp;

		    if (!attack(mtmp)) return(0);
		    if (!DEADMONSTER(mtmp) && mtmp->mhp < oldhp &&
			    !noncorporeal(mtmp->data) && !unsolid(mtmp->data)) {
			You("strike %s vital organs!", s_suffix(mon_nam(mtmp)));
			/* Base damage is always something, though it may be
			 * reduced to zero if the hero is hampered. However,
			 * since techlev will never be zero, stiking vital
			 * organs will always do _some_ damage.
			 */
			tmp = mtmp->mhp > 1 ? mtmp->mhp / 2 : 1;
			if (!humanoid(mtmp->data) || is_golem(mtmp->data) ||
				mtmp->data->mlet == S_CENTAUR) {
			    You("are hampered by the differences in anatomy.");
			    tmp /= 2;
			}
			tmp += techlev(tech_no);
			t_timeout = rnz(1500);
			hurtmon(mtmp, tmp);
		    }
		}
		break;
	    case T_CUTTHROAT:
		if (!is_blade(uwep)) {
		    You("need a blade to perform cutthroat!");
		    return 0;
		}
	    	if (!getdir((char *)0)) return 0;
		if (!u.dx && !u.dy) {
		    /* Hopefully a mistake ;B */
		    pline("Things may be going badly, but that's extreme.");
		    return 0;
		}
		mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
		if (!mtmp) {
		    You("attack...nothing!");
		    return 0;
		} else {
		    int oldhp = mtmp->mhp;

		    if (!attack(mtmp)) return 0;
		    if (!DEADMONSTER(mtmp) && mtmp->mhp < oldhp) {
			if (!has_head(mtmp->data) || u.uswallow)
			    You_cant("perform cutthroat on %s!", mon_nam(mtmp));
			else {
			    int tmp = 0;

			    if (rn2(5) < (techlev(tech_no)/10 + 1)) {
				You("sever %s head!", s_suffix(mon_nam(mtmp)));
				tmp = mtmp->mhp;
			    } else {
				You("hurt %s badly!", s_suffix(mon_nam(mtmp)));
				tmp = mtmp->mhp / 2;
			    }
			    tmp += techlev(tech_no);
			    t_timeout = rn1(1000,500);
			    hurtmon(mtmp, tmp);
			}
		    }
		}
		break;
	    case T_BLESSING:
		allowall[0] = ALL_CLASSES; allowall[1] = '\0';
		
		if ( !(obj = getobj(allowall, "bless"))) return(0);
		pline("An aura of holiness surrounds your hands!");
                if (!Blind) (void) Shk_Your(Your_buf, obj);
		if (obj->cursed) {
                	if (!Blind)
                    		pline("%s %s %s.",Your_buf,
						  aobjnam(obj, "softly glow"),
						  hcolor(NH_AMBER));
				if (!stack_too_big(obj)) uncurse(obj);
				else pline("But it failed! The stack was too big...");
				obj->bknown=1;
		} else if(!obj->blessed) {
			if (!Blind) {
				str = hcolor(NH_LIGHT_BLUE);
				pline("%s %s with a%s %s aura.",
					  Your_buf,
					  aobjnam(obj, "softly glow"),
					  index(vowels, *str) ? "n" : "", str);
			}
			if (!stack_too_big(obj)) bless(obj);
			else pline("But it failed! The stack was too big...");
			obj->bknown=1;
		} else {
			if (obj->bknown) {
				pline ("That object is already blessed!");
				return(0);
			}
			obj->bknown=1;
			pline("The aura fades.");
		}
		t_timeout = rnz(1500);
		break;
	    case T_E_FIST: 
	    	blitz_e_fist();
#if 0
		str = makeplural(body_part(HAND));
                You("focus the powers of the elements into your %s", str);
                techt_inuse(tech_no) = rnd((int) (techlev(tech_no)/3 + 1)) + d(1,4) + 2;
#endif
		t_timeout = rnz(1500);
	    	break;
	    case T_PRIMAL_ROAR:	    	
	    	You("let out a bloodcurdling roar!");
	    	aggravate();

		techt_inuse(tech_no) = d(2,6) + (techlev(tech_no)) + 2;

		incr_itimeout(&HFast, techt_inuse(tech_no));

	    	for(i = -5; i <= 5; i++) for(j = -5; j <= 5; j++)
		    if(isok(u.ux+i, u.uy+j) && (mtmp = m_at(u.ux+i, u.uy+j))) {
		    	if (mtmp->mtame != 0 && !mtmp->isspell) {
		    	    struct permonst *ptr = mtmp->data;
			    struct monst *mtmp2;
		    	    int ttime = techt_inuse(tech_no);
		    	    int type = little_to_big(monsndx(ptr));
		    	    
		    	    mtmp2 = tamedog(mtmp, (struct obj *) 0, TRUE);
			    if (mtmp2)
				mtmp = mtmp2;

		    	    if (type && type != monsndx(ptr)) {
				ptr = &mons[type];
		    	    	mon_spec_poly(mtmp, ptr, ttime, FALSE,
					canseemon(mtmp), FALSE, TRUE);
		    	    }
		    	}
		    }
		t_timeout = rnz(1500);
	    	break;
	    case T_LIQUID_LEAP: {
	    	coord cc;
	    	int dx, dy, sx, sy, range;

		pline("Where do you want to leap to?");
    		cc.x = sx = u.ux;
		cc.y = sy = u.uy;

		getpos(&cc, TRUE, "the desired position");
		if (cc.x == -10) return 0; /* user pressed esc */

		dx = cc.x - u.ux;
		dy = cc.y - u.uy;
		/* allow diagonals */
	    	if (dx && dy && dx != dy && dx != -dy) {
		    You("can only leap in straight lines!");
		    return 0;
	    	} else if (distu(cc.x, cc.y) > 19 + techlev(tech_no)) {
		    pline("Too far!");
		    return 0;
		} else if (m_at(cc.x, cc.y) || !isok(cc.x, cc.y) ||
			IS_ROCK(levl[cc.x][cc.y].typ) ||
			sobj_at(BOULDER, cc.x, cc.y) ||
			closed_door(cc.x, cc.y)) {
		    You_cant("flow there!"); /* MAR */
		    return 0;
		} else {
		    You("liquify!");
		if (Stoned) fix_petrification(); /* basically, you become an acid beam, and while I have absolutely no idea why acid would fix petrification in the first place, at least I'll make it consistent throughout the game. --Amy */
		    if (Punished) {
			You("slip out of the iron chain.");
			unpunish();
		    }
		    if(u.utrap) {
			switch(u.utraptype) {
			    case TT_BEARTRAP: 
				You("slide out of the bear trap.");
				break;
			    case TT_PIT:
				You("leap from the pit!");
				break;
			    case TT_WEB:
				You("flow through the web!");
				break;
			    case TT_GLUE:
				You("get rid of the sticky glue!");
				break;
			    case TT_LAVA:
				You("separate from the lava!");
				u.utrap = 0;
				break;
			    case TT_INFLOOR:
				u.utrap = 0;
				You("ooze out of the floor!");
			}
			u.utrap = 0;
		    }
		    /* Fry the things in the path ;B */
		    if (dx) range = dx;
		    else range = dy;
		    if (range < 0) range = -range;
		    
		    dx = sgn(dx);
		    dy = sgn(dy);
		    
		    while (range-- > 0) {
		    	int tmp_invul = 0;
		    	
		    	if (!Invulnerable) Invulnerable = tmp_invul = 1;
			sx += dx; sy += dy;
			tmp_at(DISP_BEAM, zapdir_to_glyph(dx, dy, AD_ACID-1));
			tmp_at(sx,sy);
			delay_output(); /* wait a little */
		    	if ((mtmp = m_at(sx, sy)) != 0) {
			    int chance;
			    
			    chance = rn2(20);
		    	    if (!chance || (3 - chance) > AC_VALUE(find_mac(mtmp)))
		    	    	break;
			    setmangry(mtmp);
		    	    You("catch %s in your acid trail!", mon_nam(mtmp));
		    	    if (!resists_acid(mtmp)) {
				int tmp = 1;
				/* Need to add a to-hit */
				tmp += d(2,4);
				tmp += rn2((int) (techlev(tech_no)/5 + 1));
				if (!Blind) pline_The("acid burns %s!", mon_nam(mtmp));
				hurtmon(mtmp, tmp);
			    } else if (!Blind) pline_The("acid doesn't affect %s!", mon_nam(mtmp));
			}
			/* Clean up */
			tmp_at(DISP_END,0);
			if (tmp_invul) Invulnerable = 0;
		    }

		    /* A little Sokoban guilt... */
		    if (In_sokoban(&u.uz))
			{change_luck(-1);
			pline("You cheater!");
			}
		    You("reform!");
		    teleds(cc.x, cc.y, FALSE);
		    nomul(-1, "liquid leaping");
		    nomovemsg = "";
	    	}
		t_timeout = rnz(1500);
	    	break;
	    }
            case T_SIGIL_TEMPEST: 
		/* Have enough power? */
		num = 50 - techlev(tech_no);
		if (u.uen < num) {
			You("don't have enough power to invoke the sigil! You need at least %d!",num);
			return (0);
		}
		u.uen -= num;

		/* Invoke */
		You("invoke the sigil of tempest!");
                techt_inuse(tech_no) = d(1,6) + rnd(techlev(tech_no)/5 + 1) + 2;
		u_wipe_engr(2);
		return(0);
		break;
            case T_SIGIL_CONTROL:
		/* Have enough power? */
		num = 30 - techlev(tech_no)/2;
		if (u.uen < num) {
			You("don't have enough power to invoke the sigil! You need at least %d!",num);
			return (0);
		}
		u.uen -= num;

		/* Invoke */
		You("invoke the sigil of control!");
                techt_inuse(tech_no) = d(1,4) + rnd(techlev(tech_no)/5 + 1) + 2;
		u_wipe_engr(2);
		return(0);
		break;
            case T_SIGIL_DISCHARGE:
		/* Have enough power? */
		num = 100 - techlev(tech_no)*2;
		if (u.uen < num) {
			You("don't have enough power to invoke the sigil! You need at least %d!",num);
			return (0);
		}
		u.uen -= num;

		/* Invoke */
		You("invoke the sigil of discharge!");
                techt_inuse(tech_no) = d(1,4) + rnd(techlev(tech_no)/5 + 1) + 2;
		u_wipe_engr(2);
		return(0);
		break;
            case T_RAISE_ZOMBIES:
            	You("chant the ancient curse...");
		for(i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {
		    int corpsenm;

		    if (!isok(u.ux+i, u.uy+j)) continue;
		    for (obj = level.objects[u.ux+i][u.uy+j]; obj; obj = otmp) {
			otmp = obj->nexthere;

			if (obj->otyp != CORPSE) continue;
			/* Only generate undead */
			corpsenm = mon_to_zombie(obj->corpsenm);
			if (corpsenm != -1 && !cant_create(&corpsenm, TRUE) &&
			  (!obj->oxlth || obj->oattached != OATTACHED_MONST)) {
			    /* Maintain approx. proportion of oeaten to cnutrit
			     * so that the zombie's HP relate roughly to how
			     * much of the original corpse was left.
			     */
			    if (obj->oeaten)
				obj->oeaten =
					eaten_stat(mons[corpsenm].cnutrit, obj);
			    obj->corpsenm = corpsenm;
			    mtmp = revive(obj);
			    if (mtmp) {
				if (!resist(mtmp, SPBOOK_CLASS, 0, TELL)) {
				   mtmp = tamedog(mtmp, (struct obj *) 0, FALSE);
				   You("dominate %s!", mon_nam(mtmp));
				} else setmangry(mtmp);
			    }
			}
		    }
		}
		nomul(-2, "recovering from an attempt to raise zombies"); /* You need to recover */
		nomovemsg = 0;
		t_timeout = rnz(1500);
		break;
            case T_REVIVE: 
		if (u.uswallow) {
		    You(no_elbow_room);
		    return 0;
		}
            	num = 100 - techlev(tech_no); /* WAC make this depend on mon? */
            	if ((Upolyd && u.mh <= num) || (!Upolyd && u.uhp <= num)){
		    You("don't have the strength to perform revivification!");
		    return 0;
            	}

            	obj = getobj((const char *)revivables, "revive");
            	if (!obj) return (0);
            	mtmp = revive(obj);
            	if (mtmp) {
#ifdef BLACKMARKET
		    if (Is_blackmarket(&u.uz))
			setmangry(mtmp);
		    else
#endif
		    if (mtmp->isshk)
			make_happy_shk(mtmp, FALSE);
		    else if (!resist(mtmp, SPBOOK_CLASS, 0, NOTELL))
			(void) tamedog(mtmp, (struct obj *) 0, FALSE);
		}
            	if (Upolyd) u.mh -= num;
            	else u.uhp -= num;
		t_timeout = rnz(1500);
            	break;
	    case T_WARD_FIRE:
		/* Already have it intrinsically? */
		if (HFire_resistance & FROMOUTSIDE) return (0);

		You("invoke the ward against flame!");
		HFire_resistance += rn1(100,50);
		HFire_resistance += techlev(tech_no);
		t_timeout = rnz(1500);

	    	break;
	    case T_WARD_COLD:
		/* Already have it intrinsically? */
		if (HCold_resistance & FROMOUTSIDE) return (0);

		You("invoke the ward against ice!");
		HCold_resistance += rn1(100,50);
		HCold_resistance += techlev(tech_no);
		t_timeout = rnz(1500);

	    	break;
	    case T_WARD_ELEC:
		/* Already have it intrinsically? */
		if (HShock_resistance & FROMOUTSIDE) return (0);

		You("invoke the ward against lightning!");
		HShock_resistance += rn1(100,50);
		HShock_resistance += techlev(tech_no);
		t_timeout = rnz(1500);

	    	break;
	    case T_TINKER:
		if (Blind) {
			You("can't do any tinkering if you can't see!");
			return (0);
		}
		if (!uwep) {
			You("aren't holding an object to work on!");
			return (0);
		}
		You("are holding %s.", doname(uwep));
		if (yn("Start tinkering on this?") != 'y') return(0);
		You("start working on %s",doname(uwep));
		delay=-150 + techlev(tech_no);
		set_occupation(tinker, "tinkering", 0);
		break;
	    case T_RAGE:     	
		/*if (Upolyd) {
			You("cannot focus your anger!");
			return(0);
		}*/
	    	You("feel the anger inside you erupt!");
		num = 50 + (4 * techlev(tech_no));
	    	techt_inuse(tech_no) = num + 1;
		if (Upolyd) {
			u.mhmax += num;
			u.mh += num;
		}
		u.uhpmax += num;
		u.uhp += num;
		t_timeout = rnz(1500);
		break;	    
	    case T_BLINK:
	    	You("feel the flow of time slow down.");
                techt_inuse(tech_no) = rnd(techlev(tech_no) + 1) + 2;
		t_timeout = rnz(1500);
	    	break;
            case T_CHI_STRIKE:
            	if (!blitz_chi_strike()) return(0);
                t_timeout = rnz(1500);
		break;
            case T_DRAW_ENERGY:
            	if (u.uen == u.uenmax) {
            		if (Hallucination) You("are fully charged!");
			else You("cannot hold any more energy!");
			return(0);           		
            	}
                You("begin drawing energy from your surroundings!");
		delay=-15;
		set_occupation(draw_energy, "drawing energy", 0);                
                t_timeout = rnz(1500);
		break;
            case T_CHI_HEALING:
            	if (u.uen < 1) {
            		You("are too weak to attempt this! You need at least one point of mana!");
            		return(0);
            	}
		You("direct your internal energy to restoring your body!");
                techt_inuse(tech_no) = techlev(tech_no)*2 + 4;
                t_timeout = rnz(1500);
		break;	
	    case T_DISARM:
	    	if (P_SKILL(weapon_type(uwep)) == P_NONE) {
	    		You("aren't wielding a proper weapon!");
	    		return(0);
	    	}
	    	if ((P_SKILL(weapon_type(uwep)) < P_SKILLED) || AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || have_unskilledstone() || (Blind)) {
	    		You("aren't capable of doing this!");
	    		return(0);
	    	}
		if (u.uswallow) {
	    		pline("What do you think %s is?  A sword swallower?",
				mon_nam(u.ustuck));
	    		return(0);
		}

	    	if (!getdir((char *)0)) return(0);
		if (!u.dx && !u.dy) {
			/* Hopefully a mistake ;B */
			pline("Why don't you try wielding something else instead.");
			return(0);
		}
		mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
		if (!mtmp || !canspotmon(mtmp)) {
			if (memory_is_invisible(u.ux + u.dx, u.uy + u.dy))
			    You("don't know where to aim for!");
			else
			    You("don't see anything there!");
			return (0);
		}
	    	obj = MON_WEP(mtmp);   /* can be null */
	    	if (!obj) {
	    		You_cant("disarm an unarmed foe!");
	    		return(0);
	    	}
		/* Blindness dealt with above */
		if (!mon_visible(mtmp)
#ifdef INVISIBLE_OBJECTS
				|| (obj->oinvis && !See_invisible) || obj->oinvisreal
#endif
				) {
	    		You_cant("see %s weapon!", s_suffix(mon_nam(mtmp)));
	    		return(0);
		}
		num = ((rn2(techlev(tech_no) + 15)) 
			* (P_SKILL(weapon_type(uwep)) - P_SKILLED + 1)) / 10;

		You("attempt to disarm %s...",mon_nam(mtmp));
		/* WAC can't yank out cursed items */
                if (num > 0 && (!Fumbling || !rn2(10)) && !obj->cursed) {
		    int roll;
		    obj_extract_self(obj);
		    possibly_unwield(mtmp, FALSE);
		    setmnotwielded(mtmp, obj);
		    roll = rn2(num + 1);
		    if (roll > 3) roll = 3;
		    switch (roll) {
			case 2:
			default:
			    /* to floor near you */
			    You("knock %s %s to the %s!",
				s_suffix(mon_nam(mtmp)),
				xname(obj),
				surface(u.ux, u.uy));
			    if (obj->otyp == CRYSKNIFE &&
				    (!obj->oerodeproof || !rn2(10))) {
				obj->otyp = WORM_TOOTH;
				obj->oerodeproof = 0;
			    }
			    place_object(obj, u.ux, u.uy);
			    stackobj(obj);
			    break;
			case 3:
#if 0
			    if (!rn2(25)) {
				/* proficient at disarming, but maybe not
				   so proficient at catching weapons */
				int hitu, hitvalu;

				hitvalu = 8 + obj->spe;
				hitu = thitu(hitvalu,
					dmgval(obj, &youmonst),
					obj, xname(obj));
				if (hitu)
				    pline("%s hits you as you try to snatch it!",
					    The(xname(obj)));
				place_object(obj, u.ux, u.uy);
				stackobj(obj);
				break;
			    }
#endif /* 0 */
			    /* right into your inventory */
			    You("snatch %s %s!", s_suffix(mon_nam(mtmp)),
				    xname(obj));
			    if (obj->otyp == CORPSE &&
				    touch_petrifies(&mons[obj->corpsenm]) &&
				    !uarmg && !Stone_resistance &&
				    !(poly_when_stoned(youmonst.data) &&
					polymon(PM_STONE_GOLEM))) {
				char kbuf[BUFSZ];

				Sprintf(kbuf, "%s corpse",
					an(mons[obj->corpsenm].mname));
				pline("Snatching %s is a fatal mistake.", kbuf);
				instapetrify(kbuf);
			    }
			    obj = hold_another_object(obj, "You drop %s!",
				    doname(obj), (const char *)0);
			    break;
		    }
		} else if (mtmp->mcanmove && !mtmp->msleeping)
		    pline("%s evades your attack.", Monnam(mtmp));
		else
		    You("fail to dislodge %s %s.", s_suffix(mon_nam(mtmp)),
			    xname(obj));
		wakeup(mtmp);
		if (!mtmp->mcanmove && !rn2(10)) {
		    mtmp->mcanmove = 1;
		    mtmp->mfrozen = 0;
		}
		break;
	    case T_DAZZLE:
	    	/* Short range stun attack */
	    	if (Blind) {
	    		You("can't see anything!");
	    		return(0);
	    	}
	    	if (!getdir((char *)0)) return(0);
		if (!u.dx && !u.dy) {
			/* Hopefully a mistake ;B */
			You("can't see yourself!");
			return(0);
		}
		for(i = 0; (i  <= ((techlev(tech_no) / 8) + 1) 
			&& isok(u.ux + (i*u.dx), u.uy + (i*u.dy))); i++) {
		    mtmp = m_at(u.ux + (i*u.dx), u.uy + (i*u.dy));
		    if (mtmp && canseemon(mtmp)) break;
		}
		if (!mtmp || !canseemon(mtmp)) {
			You("fail to make eye contact with anything!");
			return (0);
		}
                You("stare at %s.", mon_nam(mtmp));
                if (!haseyes(mtmp->data))
                	pline("..but %s has no eyes!", mon_nam(mtmp));
                else if (!mtmp->mcansee)
                	pline("..but %s cannot see you!", mon_nam(mtmp));
                if ((rn2(6) + rn2(6) + (techlev(tech_no) - mtmp->m_lev)) > 10) {
			You("dazzle %s!", mon_nam(mtmp));
			mtmp->mcanmove = 0;
			mtmp->mfrozen = rnd(10);
		} else {
                       pline("%s breaks the stare!", Monnam(mtmp));
		}
               	t_timeout = rnz(75);
	    	break;
	    case T_BLITZ:
	    	if (uwep || (u.twoweap && uswapwep)) {
			You("can't do this while wielding a weapon!");
	    		return(0);
	    	} else if (uarms) {
			You("can't do this while holding a shield!");
	    		return(0);
	    	}
	    	if (!doblitz()) return (0);		
		
                t_timeout = rnz(1500);
	    	break;
            case T_PUMMEL:
	    	if (uwep || (u.twoweap && uswapwep)) {
			You("can't do this while wielding a weapon!");
	    		return(0);
	    	} else if (uarms) {
			You("can't do this while holding a shield!");
	    		return(0);
	    	}
		if (!getdir((char *)0)) return(0);
		if (!u.dx && !u.dy) {
			You("flex your muscles.");
			return(0);
		}
            	if (!blitz_pummel()) return(0);
                t_timeout = rnz(1500);
		break;
            case T_G_SLAM:
	    	if (uwep || (u.twoweap && uswapwep)) {
			You("can't do this while wielding a weapon!");
	    		return(0);
	    	} else if (uarms) {
			You("can't do this while holding a shield!");
	    		return(0);
	    	}
		if (!getdir((char *)0)) return(0);
		if (!u.dx && !u.dy) {
			You("flex your muscles.");
			return(0);
		}
            	if (!blitz_g_slam()) return(0);
                t_timeout = rnz(1500);
		break;
            case T_DASH:
		if (!getdir((char *)0)) return(0);
		if (!u.dx && !u.dy) {
			You("stretch.");
			return(0);
		}
            	if (!blitz_dash()) return(0);
                t_timeout = rnz(75);
		break;
            case T_POWER_SURGE:
            	if (!blitz_power_surge()) return(0);
		t_timeout = rnz(issoviet ? 1500 : 5000);
		if (issoviet) pline("Sovetskaya nichego ne znayet o balansirovaniya ne ponimayet i poetomu khochet etu tekhniku, kotoraya uzhe slishkom sil'na, chtoby byt' yeshche sil'neye.");
		break;            	
            case T_SPIRIT_BOMB:
	    	if (uwep || (u.twoweap && uswapwep)) {
			You("can't do this while wielding a weapon!");
	    		return(0);
	    	} else if (uarms) {
			You("can't do this while holding a shield!");
	    		return(0);
	    	}
		if (!getdir((char *)0)) return(0);
            	if (!blitz_spirit_bomb()) return(0);
		t_timeout = rnz(1500);
		break;            	
	    case T_DRAW_BLOOD:
		/*if (!maybe_polyd(is_vampire(youmonst.data),
		  Race_if(PM_VAMPIRE)) && !Race_if(PM_UNGENOMOLD) ) {*/
		    /* ALI
		     * Otherwise we get problems with what we create:
		     * potions of vampire blood would no longer be
		     * appropriate.
		     * Note by Amy: Ungenomolds will be able to use this anyway. They may polymorph into a vampire,
		     * which would make the potions useful for them. Even if they don't, they can still dilute them.
		     * another note by Amy: who cares? If they can't use it in their current form, too bad. It's still
		     * better than simply disabling the tech altogether.
		     */
		    /*You("must be in your natural form to draw blood.");
		    return(0);
		}*/
		obj = use_medical_kit(PHIAL, TRUE, "draw blood with");
		if (!obj)
		    return 0;
		if (u.ulevel <= 1) {
		    You_cant("seem to find a vein.");
		    return 0;
		}
		check_unpaid(obj);
		if (obj->quan > 1L)
		    obj->quan--;
		else {
		    obj_extract_self(obj);
		    obfree(obj, (struct obj *)0);
		}
		pline("Using your medical kit, you draw off a phial of your blood.");
		/* Amy edit: let's make this much more useful by having the level drain only occur 1 out of 3 times. */
		if (!rn2(3)) {losexp("drawing blood", TRUE, FALSE);
		if (u.uexp > 0)
		    u.uexp = newuexp(u.ulevel - 1);
		}
		otmp = mksobj(POT_VAMPIRE_BLOOD, FALSE, FALSE);
		otmp->cursed = obj->cursed;
		otmp->blessed = obj->blessed;
		(void) hold_another_object(otmp,
			"You fill, but have to drop, %s!", doname(otmp),
			(const char *)0);
		t_timeout = rnz(1500);
		break;
#ifdef JEDI
	    case T_JEDI_JUMP:
		if (u.uen < 25){
			You("can't channel the force around you. Jedi jumps require 25 points of mana!");
			display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return(0);
		}
		if (!jump((techlev(tech_no)/5)+1)) return 0;
		u.uen -= 25;
		t_timeout = rnz(50);
		break;
	    case T_POKE_BALL:

		if (u.uswallow) {
		    You("don't have enough free space to throw the ball!");
			return 0;
		}

		int i, j, caught, catchrate;
		struct monst *mtmp;
		register struct monst *mtmp2;
		caught = 0;
		pline("%s used Poke Ball!", plname);

		    for (i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ( ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) && mtmp->mtame == 0 
			&& mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) && caught == 0)

				/* using the M3_PETTY attribute now --Amy */

			{

				if (is_petty(mtmp->data) || mtmp->egotype_petty)

				{
			      /*maybe_tameX(mtmp);*/
				pline("Gotcha! %s was caught!", mon_nam(mtmp));
				(void) tamedog(mtmp, (struct obj *) 0, TRUE);
				caught++;
				t_timeout = rnz(1000);
				}

			else if ( (mtmp->m_lev > (2 * techlev(tech_no)) || rn2(4) ) && mtmp->m_lev > techlev(tech_no) && caught == 0 && ( (!is_pokemon(mtmp->data) && (!mtmp->egotype_pokemon) ) || rn2(2) ) )
				{
				pline("You missed the Pokemon!");
				}

			else if (caught == 0) /* other monster that can be caught */

				{
				/* If catchrate is a higher numeric value, the chance of catching the monster is lower. */

				catchrate = (60 + mtmp->m_lev - techlev(tech_no));
				if (!rn2(4)) catchrate -= techlev(tech_no);
				if (is_pokemon(mtmp->data) || mtmp->egotype_pokemon) catchrate = (catchrate / 2);
				if (catchrate < 3) catchrate = 3;
				if (rnd(100) < catchrate) pline("Oh, no! The Pokemon broke free!");
				else if (rnd(100) < catchrate) pline("Aww! It appeared to be caught!");
				else if (rnd(100) < catchrate) pline("Arrgh! Almost had it!");
				else if (rnd(100) < catchrate) pline("Shit! It was so close too!");
				else {
				      /*maybe_tameX(mtmp);*/
					pline("Gotcha! %s was caught!", mon_nam(mtmp));
					(void) tamedog(mtmp, (struct obj *) 0, TRUE);
					caught++;
					t_timeout = rnz(1000);

					}
				}

			} /* monster is catchable loop */
		    } /* for loop */

		if (caught == 0) pline("The ball expodes in midair!");
/* This is an intentional typo, derived from another roguelike. Do you know which one it is? --Amy*/

		t_timeout = rnz(1000);
		break;
	    case T_SUMMON_TEAM_ANT:

		    pline("Go Team Ant!");

		int caughtY;
		caughtY = 0;

		while (caughtY == 0) {
		/*mtmp = make_helper(S_ANT, u.ux, u.uy); */
 	      mtmp = makemon(mkclass(S_ANT,0), u.ux, u.uy, NO_MM_FLAGS);
		if (!mtmp) break;
		/*mtmp->mtame = 10;*/
	      /*maybe_tameX(mtmp);*/
		(void) tamedog(mtmp, (struct obj *) 0, TRUE);
		if (techlev(tech_no) < rnd(50)) caughtY++;
		}

		/* A high level Insectoid character can create quite the army of insects sometimes. --Amy */

		t_timeout = rnz(1000);
		break;

	    case T_DOUBLE_TROUBLE:	/* inspired by Khor */

		pline("Double Trouble...");

		int familiardone;
		familiardone = 0;

		if (!rn2(5)) mtmp = makemon(&mons[PM_RODNEYAN], u.ux, u.uy, NO_MM_FLAGS);
		else if (Upolyd) mtmp = makemon(&mons[u.umonnum], u.ux, u.uy, NO_MM_FLAGS);
		else mtmp = makemon(&mons[urole.malenum], u.ux, u.uy, NO_MM_FLAGS);
		if (mtmp) (void) tamedog(mtmp, (struct obj *) 0, TRUE);

		if ((techlev(tech_no)) < rnd(50)) familiardone = 1;
		else pline("Sent in some familiars too.");

		while (familiardone == 0) {

	 	      mtmp = makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
			if (mtmp) (void) tamedog(mtmp, (struct obj *) 0, TRUE);

			if ((techlev(tech_no)) < rnd(50)) familiardone = 1;
		}

		t_timeout = rnz(5000);
		break;

	    case T_ATTIRE_CHARM:

		if (u.uswallow) {
		    pline("The monster can't see its inside anyway!");
			return 0;
		}

		if (!PlayerInHighHeels) {
		    pline("You must be wearing high heels for that.");
			return 0;
		}

		int k, l, caughtX;
		struct monst *mtmp3;
		register struct monst *mtmp4;
		caughtX = 0;
		pline("You strike a sexy pose with your heels!");

		    for (k = -1; k <= 1; k++) for(l = -1; l <= 1; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;
			if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) && mtmp3->mtame == 0 && mtmp3->isshk == 0 && mtmp3->isgd 			== 0 && mtmp3->ispriest == 0 && mtmp3->isminion == 0 && mtmp3->isgyp == 0
&& mtmp3->data != &mons[PM_SHOPKEEPER] && mtmp3->data != &mons[PM_BLACK_MARKETEER] && mtmp3->data != &mons[PM_ALIGNED_PRIEST] && mtmp3->data != &mons[PM_HIGH_PRIEST] && mtmp3->data != &mons[PM_GUARD]
			&& mtmp3->mnum != quest_info(MS_NEMESIS) && !(mtmp3->data->geno & G_UNIQ) && caughtX == 0)

				/* gotta write a huge function for this now --Amy */

			{

				if ( humanoid(mtmp3->data) || mtmp3->data->mlet == S_HUMAN) {
			      /*maybe_tameX(mtmp3);*/
				pline("%s is charmed, and wants to be your friend!", mon_nam(mtmp3));
				(void) tamedog(mtmp3, (struct obj *) 0, TRUE);
				if (techlev(tech_no) < rnd(100)) caughtX++;
				t_timeout = rnz(2000);
				}

			else pline("%s is too stupid to fully appreciate you!", mon_nam(mtmp3));

			} /* monster is catchable loop */
		    } /* for loop */

		t_timeout = rnz(2000);
		break;
	    case T_WORLD_FALL:

		You("scream \"EYGOORTS-TOGAAL, JEZEHH!\"");
/* Actually, it's "To win the game you must kill me, John Romero" recorded backwards.
   When I was little, I always thought it said "Eygoorts-togaal, jezehh". --Amy */
				{
			    register struct monst *mtmp, *mtmp2;

				num = 0;

			    for (mtmp = fmon; mtmp; mtmp = mtmp2) {
				mtmp2 = mtmp->nmon;
				if ( ((mtmp->m_lev < techlev(tech_no)) || (!rn2(4) && mtmp->m_lev < (2 * techlev(tech_no)))) && mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) ) { mondead(mtmp);
						num++;
						}
			    }
	pline("Eliminated %d monster%s.", num, plur(num));
			}

		t_timeout = rnz(10000);
		break;
	    case T_CREATE_AMMO:

	    {
		int ammotype;
		ammotype = 1; /* bullets */
		if (Role_if(PM_DOOM_MARINE)) {

			if (techlev(tech_no) >= 25) {

				pline("You can choose from these kinds of ammo: BFG ammo, rockets, shotgun shells, blaster bolts or bullets.");
				if (yn("Do you want to create BFG ammo?") == 'y') ammotype = 5;
				else if (yn("Do you want to create rockets?") == 'y') ammotype = 4;
				else if (yn("Do you want to create shotgun shells?") == 'y') ammotype = 3;
				else if (yn("Do you want to create blaster bolts?") == 'y') ammotype = 2;
				else ammotype = 1;
			}

			else if (techlev(tech_no) >= 20) {

				pline("You can choose from these kinds of ammo: rockets, shotgun shells, blaster bolts or bullets.");
				if (yn("Do you want to create rockets?") == 'y') ammotype = 4;
				else if (yn("Do you want to create shotgun shells?") == 'y') ammotype = 3;
				else if (yn("Do you want to create blaster bolts?") == 'y') ammotype = 2;
				else ammotype = 1;
			}

			else if (techlev(tech_no) >= 15) {

				pline("You can choose from these kinds of ammo: shotgun shells, blaster bolts or bullets.");
				if (yn("Do you want to create shotgun shells?") == 'y') ammotype = 3;
				else if (yn("Do you want to create blaster bolts?") == 'y') ammotype = 2;
				else ammotype = 1;
			}

			else if (techlev(tech_no) >= 10) {

				pline("You can choose from these kinds of ammo: blaster bolts or bullets.");
				if (yn("Do you want to create blaster bolts?") == 'y') ammotype = 2;
				else ammotype = 1;
			}

		}

	    You("make some ammo for your gun.");

		struct obj *uammo;

		if (ammotype == 5) uammo = mksobj(BFG_AMMO, TRUE, FALSE);
		else if (ammotype == 4) uammo = mksobj(ROCKET, TRUE, FALSE);
		else if (ammotype == 3) uammo = mksobj(SHOTGUN_SHELL, TRUE, FALSE);
		else if (ammotype == 2) uammo = mksobj(BLASTER_BOLT, TRUE, FALSE);
		else uammo = mksobj(BULLET, TRUE, FALSE);
		uammo->quan = techlev(tech_no);
		/* gunner really specializes in ranged weapons, so needs a big bonus --Amy */
		if (Role_if(PM_GUNNER)) uammo->quan *= 1 + rnd(2);
		if (ammotype == 5) uammo->quan *= 4;
		if (ammotype == 4) uammo->quan /= 10;
		if (uammo->quan < 0) uammo->quan = 1; /* fail safe */
		uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
		uammo->owt = weight(uammo);
		dropy(uammo);
		stackobj(uammo);

	      t_timeout = rnz(500);
	      break;

	    }

	    case T_EGG_BOMB:

	    You("create some stoning grenades.");

		int caughtZ;
		caughtZ = 0;
		struct obj *uegg;

		while (caughtZ == 0) {

			uegg = mksobj(EGG, FALSE, FALSE);
			uegg->spe = 0;
			uegg->quan = 1;
			uegg->owt = weight(uegg);
			uegg->corpsenm = egg_type_from_parent(PM_COCKATRICE, FALSE);
			uegg->known = uegg->dknown = 1;
			attach_egg_hatch_timeout(uegg);
			kill_egg(uegg); /* make sure they're stale --Amy */
			dropy(uegg);
			stackobj(uegg);
			if (techlev(tech_no) < rnd(100)) caughtZ++;

		}

	      t_timeout = rnz(2000);
	      break;

	    case T_BOOZE:

	    You("procure some refreshing drinks.");

	    make_confused(HConfusion + d(3,8), FALSE);
		healup(Role_if(PM_DRUNK) ? rnz(20 + u.ulevel) : 1, 0, FALSE, FALSE);
		u.uhunger += 20;
		if (Race_if(PM_CLOCKWORK_AUTOMATON)) u.uhunger += 200;
		if (Role_if(PM_DRUNK)) u.uhunger += 100;
		newuhs(FALSE);
		exercise(A_WIS, FALSE);

		int caughtW;
		caughtW = 0;
		struct obj *udrink;

		while (caughtW == 0) {

			udrink = mksobj(POT_BOOZE, TRUE, FALSE);
			udrink ->known = udrink ->dknown = 1;
			dropy(udrink);
			stackobj(udrink);
			if (techlev(tech_no) < rnd(40)) caughtW++;

		}

	      t_timeout = rnz(2000);
	      break;

	    case T_TELEKINESIS:
	      {
		coord cc;
		struct trap *ttrap;
		cc.x=u.ux;
		cc.y=u.uy;
		pline("Where do you want to apply telekinesis?");
		if (getpos(&cc, TRUE, "apply telekinesis where") < 0)
			return(0);
		if (!cansee(cc.x, cc.y)){
			You("can't see what's there!");
			return(0);
		}
		if ((ttrap=t_at(cc.x, cc.y)) && ttrap->tseen &&
			yn("Handle the trap here?") == 'y'){
		  if (yn("Disarm the trap?") == 'y'){
		    techt_inuse(tech_no) = 1;
		    /* copied from trap.c */
		switch(ttrap->ttyp) {
			case BEAR_TRAP:
			case WEB:
				t_timeout = rnz(250);
				return disarm_holdingtrap(ttrap);
			case LANDMINE:
				t_timeout = rnz(250);
				return disarm_landmine(ttrap);
			case SQKY_BOARD:
				t_timeout = rnz(250);
				return disarm_squeaky_board(ttrap);
			case DART_TRAP:
				t_timeout = rnz(250);
				return disarm_shooting_trap(ttrap, DART);
			case ARROW_TRAP:
				t_timeout = rnz(250);
				return disarm_shooting_trap(ttrap, ARROW);
			case RUST_TRAP:
				t_timeout = rnz(250);
				return disarm_rust_trap(ttrap);
			case FIRE_TRAP:
				t_timeout = rnz(250);
				return disarm_fire_trap(ttrap);
			default:
				You("cannot disable %s trap.", (u.dx || u.dy) ? "that" : "this");
				return 1;
		    }
		  } else if(yn("Spring this trap?")=='y') {
		    switch(ttrap->ttyp) {
		      case LANDMINE: 
			You("trigger the landmine.");
			pline("KABLAAAM!");
			blow_up_landmine(ttrap);
			fill_pit(cc.x, cc.y);
			newsym(cc.x, cc.y);
		        t_timeout = rnz(250);
			break;
		      case ROLLING_BOULDER_TRAP:
		      {
			int style = ROLL | (ttrap->tseen ? LAUNCH_KNOWN : 0);
			You("trigger the trap!");
			if(!launch_obj(BOULDER, ttrap->launch.x, ttrap->launch.y, ttrap->launch2.x, ttrap->launch2.y, style)){
			  deltrap(ttrap);
			  newsym(cc.x, cc.y);
			  pline("But no boulder was released.");
		        }
		        t_timeout = rnz(250);
		      }
		      default:
		        You("can't spring this trap.");
			return(0);
		    }
		  }
		} else if ((otmp = level.objects[cc.x][cc.y]) != 0) {
		  char buf[BUFSZ];
		  sprintf(buf, "Pick up %s?", the(xname(otmp)));
		  if (yn(buf) == 'n')
			return(0);
		  You("pick up an object from the %s.", surface(cc.x,cc.y));
		  (void) pickup_object(otmp, 1L, TRUE);
		  newsym(cc.x, cc.y);
		  t_timeout = rnz(250);
		} else {
		  You("can't do anything there");
			return(0);
		}
		break;
	      }
	    case T_CHARGE_SABER:
	      if (!uwep || !is_lightsaber(uwep)){
		      You("are not holding a lightsaber!");
			return(0);
	      }
	      if (u.uen < 5){
		      You("lack the concentration to charge %s. You need at least 5 points of mana!", the(xname(uwep)));
			return(0);
	      }
	      You("start charging %s.", the(xname(uwep)));
	      delay=-10;
	      set_occupation(charge_saber, "charging", 0);
	      t_timeout = rnz(500);
	      break;
#endif
	    default:
	    	pline ("Error!  No such effect (%i)", tech_no);
		return(0);
        }
        if (!can_limitbreak())
	    techtout(tech_no) = (t_timeout * (100 - techlev(tech_no))/100);
		if (ishaxor && techtout(tech_no) > 1) techtout(tech_no) /= 2;

	/*By default,  action should take a turn*/
	return(1);
}

/* Whether or not a tech is in use.
 * 0 if not in use, turns left if in use. Tech is done when techinuse == 1
 */
int
tech_inuse(tech_id)
int tech_id;
{
        int i;

        if (tech_id < 1 || tech_id > MAXTECH) {
                impossible ("invalid tech: %d", tech_id);
                return(0);
        }
        for (i = 0; i < MAXTECH; i++) {
                if (techid(i) == tech_id) {
                        return (techt_inuse(i));
                }
        }
	return (0);
}

static void
maybe_tameX(mtmp)
struct monst *mtmp;
{
	(void) tamedog(mtmp, (struct obj *) 0, TRUE);
}


void
tech_timeout()
{
	int i;
	
        for (i = 0; i < MAXTECH; i++) {
	    if (techid(i) == NO_TECH)
		continue;
	    if (techt_inuse(i)) {
	    	/* Check if technique is done */
	        if (!(--techt_inuse(i)))
	        switch (techid(i)) {
		    case T_EVISCERATE:
			You("retract your claws.");
			/* You're using bare hands now,  so new msg for next attack */
			unweapon=TRUE;
			/* Lose berserk status */
			repeat_hit = 0;
			break;
		    case T_BERSERK:
			The("red haze in your mind clears.");
			break;
		    case T_KIII:
			You("calm down.");
			break;
		    case T_FLURRY:
			You("relax.");
			break;
		    case T_E_FIST:
			You("feel the power dissipate.");
			break;
		    case T_SIGIL_TEMPEST:
			pline_The("sigil of tempest fades.");
			break;
		    case T_SIGIL_CONTROL:
			pline_The("sigil of control fades.");
			break;
		    case T_SIGIL_DISCHARGE:
			pline_The("sigil of discharge fades.");
			break;
		    case T_RAGE:
			Your("anger cools.");
			break;
		    case T_POWER_SURGE:
			pline_The("awesome power within you fades.");
			break;
		    case T_BLINK:
			You("sense the flow of time returning to normal.");
			break;
		    case T_CHI_STRIKE:
			You("feel the power in your hands dissipate.");
			break;
		    case T_CHI_HEALING:
			You("feel the healing power dissipate.");
			break;
	            default:
	            	break;
	        } else switch (techid(i)) {
	        /* During the technique */
		    case T_RAGE:
			/* Bleed but don't kill */
			if (u.uhpmax > 1) u.uhpmax--;
			if (u.uhp > 1) u.uhp--;
			if (Upolyd) {
				if (u.mhmax > 1) u.uhpmax--;
				if (u.mh > 1) u.uhp--;
			}
			break;
		    case T_POWER_SURGE:
			/* Bleed off power.  Can go to zero as 0 power is not fatal */
			if (u.uenmax > 1) u.uenmax--;
			if (u.uen > 0) u.uen--;
			break;
	            default:
	            	break;
	        }
	    } 

	    if (techtout(i) == 1) pline("Your %s technique is ready to be used!", techname(i));
	    if (techtout(i) > 0) techtout(i)--;
        }
}

void
docalm()
{
	int i, tech, n = 0;

	for (i = 0; i < MAXTECH; i++) {
	    tech = techid(i);
	    if (tech != NO_TECH && techt_inuse(i)) {
		aborttech(tech);
		n++;
	    }
	}
	if (n)
	    You("calm down.");
}

static void
hurtmon(mtmp, tmp)
struct monst *mtmp;
int tmp;
{
	mtmp->mhp -= tmp;
	if (mtmp->mhp < 1) killed (mtmp);
#ifdef SHOW_DMG
	else showdmg(tmp);
#endif
}

static const struct 	innate_tech *
role_tech()
{
	switch (Role_switch) {
		case PM_ARCHEOLOGIST:	return (arc_tech);
		case PM_GOFF:	return (gof_tech);
		case PM_AMAZON:	return (ama_tech);
		case PM_ALTMER:	return (alt_tech);
		case PM_BOSMER:	return (bos_tech);
		case PM_DUNMER:	return (dun_tech);
		case PM_ORDINATOR:	return (ord_tech);
		case PM_THALMOR:	return (tha_tech);
		case PM_DRUNK:	return (dru_tech);
		case PM_BARBARIAN:	return (bar_tech);
		case PM_BLEEDER:	return (ble_tech);
		case PM_CAVEMAN:	return (cav_tech);
		case PM_DQ_SLIME:	return (sli_tech);
		case PM_ERDRICK:	return (erd_tech);
		case PM_BARD:	return (brd_tech);
		case PM_FLAME_MAGE:	return (fla_tech);
		case PM_ACID_MAGE:	return (aci_tech);
		case PM_TRANSVESTITE:	return (tra_tech);
		case PM_TRANSSYLVANIAN:	return (trs_tech);
		case PM_TOPMODEL:	return (top_tech);
		case PM_FAILED_EXISTENCE:	return (fai_tech);
		case PM_LUNATIC:	return (lun_tech);
		case PM_ACTIVISTOR:	return (act_tech);
		case PM_ELECTRIC_MAGE:	return (ele_tech);
		case PM_POISON_MAGE:	return (psn_tech);
		case PM_HEALER:		return (hea_tech);
		case PM_ICE_MAGE:	return (ice_tech);
#ifdef JEDI
		case PM_JEDI:		return (jed_tech);
#endif
		case PM_KNIGHT:		return (kni_tech);
		case PM_MONK: 		return (mon_tech);
		case PM_JESTER: 		return (jes_tech);
		case PM_LADIESMAN: 		return (lad_tech);
		case PM_SEXYMATE: 		return (sex_tech);
		case PM_MEDIUM: 		return (med_tech);
		case PM_FENCER: 		return (fnc_tech);
		case PM_FIGHTER: 		return (fig_tech);
		case PM_SUPERMARKET_CASHIER: 		return (sup_tech);
		case PM_WANDKEEPER: 		return (wan_tech);
		case PM_PALADIN: 		return (pal_tech);
		case PM_FEAT_MASTER: 		return (stu_tech);
		case PM_DOLL_MISTRESS: 		return (dol_tech);
		case PM_MIDGET:		return (mid_tech);
		case PM_GUNNER: 		return (gun_tech);
		case PM_DOOM_MARINE: 		return (mar_tech);
		case PM_ANACHRONIST: 		return (ana_tech);
		case PM_LIBRARIAN: 		return (lib_tech);
		case PM_AUGURER: 		return (aug_tech);
		case PM_SAIYAN: 		return (sai_tech);
		case PM_PSION: 		return (psi_tech);
		case PM_SCIENTIST: 		return (sci_tech);
		case PM_DEATH_EATER: 		return (dea_tech);
		case PM_POKEMON: 		return (pok_tech);
		case PM_GANGSTER: 		return (gan_tech);
		case PM_ROCKER: 		return (roc_tech);
		case PM_NECROMANCER:	return (nec_tech);
		case PM_PRIEST:		return (pri_tech);
		case PM_GOLDMINER:		return (gol_tech);
		case PM_CHEVALIER:		return (che_tech);
		case PM_ASSASSIN:		return (ass_tech);
		case PM_RANGER:		return (ran_tech);
		case PM_TWELPH:
		case PM_ELPH:		return (elp_tech);
		case PM_SPACEWARS_FIGHTER:		return (spa_tech);
		case PM_CAMPERSTRIKER:		return (cam_tech);
		case PM_ROGUE:		return (rog_tech);
		case PM_SAMURAI:	return (sam_tech);
#ifdef TOURIST        
		case PM_TOURIST:	return (tou_tech);
#endif        
		case PM_UNDEAD_SLAYER:	return (und_tech);
		case PM_UNBELIEVER:	return (unb_tech);
		case PM_SHAPESHIFTER:	return (sha_tech);
		case PM_RINGSEEKER:		return (rin_tech);
		case PM_UNDERTAKER:	return (unt_tech);
		case PM_VALKYRIE:	return (val_tech);
		case PM_WIZARD:		return (wiz_tech);
#ifdef YEOMAN
		case PM_YEOMAN:		return (yeo_tech);
#endif
		default: 		return ((struct innate_tech *) 0);
	}
}

static const struct     innate_tech *
race_tech()
{
	switch (Race_switch) {
		case PM_DOPPELGANGER:	return (dop_tech);
#ifdef DWARF
		case PM_DWARF:		return (dwa_tech);
#endif
		case PM_ELF:
		case PM_DROW:		return (elf_tech);
		case PM_CURSER:		return (cur_tech);
		case PM_CLOCKWORK_AUTOMATON:		return (clk_tech);

		case PM_FENEK:		return (fen_tech);
		case PM_NORD:		return (nor_tech);
		case PM_ALBAE:		return (alb_tech);
		case PM_VORTEX:		return (vor_tech);
		case PM_CORTEX:		return (cor_tech);
		case PM_GNOME:		return (gno_tech);
		case PM_KOBOLT:		return (kob_tech);
		case PM_OGRO:		return (ogr_tech);
		case PM_BATMAN:		return (bat_tech);
		case PM_BORG:		return (bor_tech);
		case PM_RODNEYAN:		return (rod_tech);
		case PM_TURTLE:		return (tur_tech);
		case PM_JELLY:		return (jel_tech);
		case PM_UNGENOMOLD:		return (ung_tech);
		case PM_ARGONIAN:		return (arg_tech);
		case PM_ARMED_COCKATRICE:		return (coc_tech);
		case PM_INSECTOID:		return (ins_tech);
		case PM_MUMMY:		return (mum_tech);
		case PM_KHAJIIT:	return (kha_tech);
		case PM_WEAPON_CUBE:	return (gel_tech);
		case PM_HOBBIT:		return (hob_tech);
		case PM_HUMAN_WEREWOLF:	return (lyc_tech);
		case PM_HUMAN_MONKEY:	return (hmo_tech);
		case PM_VAMPIRE:	return (vam_tech);
		case PM_SUCKING_FIEND:	return (vam_tech);
		case PM_VAMGOYLE:	return (vam_tech);
		case PM_LEVITATOR:	return (lev_tech);
		case PM_HERBALIST:		return (hrb_tech);
		case PM_ALCHEMIST: 		return (alc_tech);
		case PM_HUMANOID_ANGEL:	return (ang_tech);
		default: 		/*return ((struct innate_tech *) 0)*/return (def_tech);
	}
}

void
adjtech(oldlevel,newlevel)
int oldlevel, newlevel;
{
	const struct   innate_tech  
		*tech = role_tech(), *rtech = race_tech();
	long mask = FROMEXPER;

	while (tech || rtech) {
	    /* Have we finished with the tech lists? */
	    if (!tech || !tech->tech_id) {
	    	/* Try the race intrinsics */
	    	if (!rtech || !rtech->tech_id) break;
	    	tech = rtech;
	    	rtech = (struct innate_tech *) 0;
		mask = FROMRACE;
	    }
		
	    for(; tech->tech_id; tech++)
		if(oldlevel < tech->ulevel && newlevel >= tech->ulevel) {
		    if (tech->ulevel != 1 && !tech_known(tech->tech_id))
			You("learn how to perform %s!",
			  tech_names[tech->tech_id]);
		    learntech(tech->tech_id, mask, tech->tech_lev);
		} else if (oldlevel >= tech->ulevel && newlevel < tech->ulevel
		    && tech->ulevel != 1) {
		    learntech(tech->tech_id, mask, -1);
		    if (!tech_known(tech->tech_id))
			You("lose the ability to perform %s!",
			  tech_names[tech->tech_id]);
		}
	}
}

int
mon_to_zombie(monnum)
int monnum;
{
	if ((&mons[monnum])->mlet == S_ZOMBIE) return monnum;  /* is already zombie */
	if ((&mons[monnum])->mlet == S_KOBOLD) return PM_KOBOLD_ZOMBIE;
	if ((&mons[monnum])->mlet == S_GNOME) return PM_GNOME_ZOMBIE;
	if (is_orc(&mons[monnum])) return PM_ORC_ZOMBIE;
	if (is_dwarf(&mons[monnum])) return PM_DWARF_ZOMBIE;
	if (is_elf(&mons[monnum])) return PM_ELF_ZOMBIE;
	if (is_human(&mons[monnum])) return PM_HUMAN_ZOMBIE;
	if (monnum == PM_ETTIN) return PM_ETTIN_ZOMBIE;
	if (is_giant(&mons[monnum])) return PM_GIANT_ZOMBIE;
	/* Is it humanoid? */
	/* Amy edit - what the heck??? No wonder the effing tech never worked! Because NO ONE tells you that! :( */
	/* if (!humanoid(&mons[monnum])) return (-1); */
	/* Otherwise,  return a ghoul or ghast */
	/* or a random Z because, well, we have so many monster species, let's give the necro some better undead. --Amy */
	if (!rn2(3)) return (pm_mkclass(S_ZOMBIE, 0) );
	else if (!rn2(4)) return PM_GHAST;
	else return PM_GHOUL;
}

#ifdef JEDI
STATIC_PTR int
charge_saber()
{
	int i, tlevel;
	if(delay) {
		delay++;
		return(1);
	}
	for (i = 0; i < MAXTECH; i++) {
	    if (techid(i) == NO_TECH)
		continue;
	    if (techid(i) != T_CHARGE_SABER)
		continue;
	    tlevel = techlev(i);
	}
	if (tlevel >= 10 && !rn2(5)){
		You("manage to channel the force perfectly!");
		uwep->age+=1500; // Jackpot!
	} else
		You("channel the force into %s.", the(xname(uwep)));

	// yes no return above, it's a bonus :)
	uwep->age+=u.uen*(( (techlev(T_CHARGE_SABER) + rnd(5 + techlev(T_CHARGE_SABER)) ) /rnd(10))+3); /* improved results by Amy */
	u.uen=0;
	flags.botl=1;
	return(0);
}
#endif

/*WAC tinker code*/
STATIC_PTR int
tinker()
{
	int chance;
	struct obj *otmp = uwep;


	if (delay) {    /* not if (delay++), so at end delay == 0 */
		delay++;
#if 0
		use_skill(P_TINKER, 1); /* Tinker skill */
#endif
		/*WAC a bit of practice so even if you're interrupted
		you won't be wasting your time ;B*/
		return(1); /* still busy */
	}

	if (!uwep)
		return (0);

	You("finish your tinkering.");
	chance = 5;
/*	chance += PSKILL(P_TINKER); */
	if (rnl(10) < chance) {		
		upgrade_obj(otmp);
	} else {
		/* object downgrade  - But for now,  nothing :) */
	}

	setuwep(otmp, FALSE);
	You("now hold %s!", doname(otmp));
	return(0);
}

/*WAC  draw energy from surrounding objects */
STATIC_PTR int
draw_energy()
{
	int powbonus = 1;
	if (delay) {    /* not if (delay++), so at end delay == 0 */
		delay++;
		confdir();
		if(isok(u.ux + u.dx, u.uy + u.dy)) {
			switch((&levl[u.ux + u.dx][u.uy + u.dy])->typ) {
			    case ALTAR: /* Divine power */
			    	powbonus =  (u.uenmax > 28 ? u.uenmax / 4
			    			: 7);
				break;
			    case THRONE: /* Regal == pseudo divine */
			    	powbonus =  (u.uenmax > 36 ? u.uenmax / 6
			    			: 6);			    		 	
				break;
			    case CLOUD: /* Air */
			    case TREE: /* Earth */
			    case LAVAPOOL: /* Fire */
			    case ICE: /* Water - most ordered form */
			    	powbonus = (u.uenmax > 40 ? u.uenmax / 8 : 5);
				break;
			    case AIR:
			    case MOAT: /* Doesn't freeze */
			    case WATER:
			    	powbonus = (u.uenmax > 40 ? u.uenmax / 10 : 4);
				break;
			    case POOL: /* Can dry up */
			    	powbonus = (u.uenmax > 36 ? u.uenmax / 12 : 3);
				break;
			    case FOUNTAIN:
			    	powbonus = (u.uenmax > 30 ? u.uenmax / 15 : 2);
				break;
			    case SINK:  /* Cleansing water */
			    	if (!rn2(3)) powbonus = 2;
				break;
			    case TOILET: /* Water Power...but also waste! */
			    	if (rn2(100) < 50)
			    		powbonus = 2;
			    	else powbonus = -2;
				break;
			    case GRAVE:
			    	powbonus = -4;
				break;
			    default:
				break;
			}
		}
		u.uen += powbonus;
		if (u.uen > u.uenmax) {
			delay = 0;
			u.uen = u.uenmax;
		}
		if (u.uen < 1) u.uen = 0;
		flags.botl = 1;
		return(1); /* still busy */
	}
	You("finish drawing energy from your surroundings.");
	return(0);
}

static const char 
	*Enter_Blitz = "Enter Blitz Command[. to end]: ";

/* Keep commands that reference the same blitz together 
 * Keep the BLITZ_START before the BLITZ_CHAIN before the BLITZ_END
 */
static const struct blitz_tab blitzes[] = { 	
	{"LLDDR", 5, blitz_chi_strike, T_CHI_STRIKE, BLITZ_START},
	{"LLDDRDR", 7, blitz_chi_strike, T_CHI_STRIKE, BLITZ_START},
	{"RR",  2, blitz_dash, T_DASH, BLITZ_START},
	{"LL",  2, blitz_dash, T_DASH, BLITZ_START},
	{"UURRDDL", 7, blitz_e_fist, T_E_FIST, BLITZ_START},
	{"URURRDDLDL", 10, blitz_e_fist, T_E_FIST, BLITZ_START},
	{"DDRRDDRR", 8, blitz_power_surge, T_POWER_SURGE, BLITZ_START},
	{"DRDRDRDR", 8, blitz_power_surge, T_POWER_SURGE, BLITZ_START},
	{"LRL", 3, blitz_pummel, T_PUMMEL, BLITZ_CHAIN},
	{"RLR", 3, blitz_pummel, T_PUMMEL, BLITZ_CHAIN},
	{"DDDD", 4, blitz_g_slam, T_G_SLAM, BLITZ_END},
	{"DUDUUDDD", 8, blitz_spirit_bomb, T_SPIRIT_BOMB, BLITZ_END},
	{"", 0, (void *)0, 0, BLITZ_END} /* Array terminator */
};

#define MAX_BLITZ 50
#define MIN_CHAIN 2
#define MAX_CHAIN 5

/* parse blitz input */
static int
doblitz()
{
	int i, j, dx, dy, bdone = 0, tech_no;
	char buf[BUFSZ];
	char *bp;
	int blitz_chain[MAX_CHAIN], blitz_num;
        
	tech_no = (get_tech_no(T_BLITZ));

	if (tech_no == -1) {
		return(0);
	}
	
	if (u.uen < 10) {
		You("are too weak to attempt this! You need at least 10 points of mana!");
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
            	return(0);
	}

	bp = buf;
	
	if (!getdir((char *)0)) return(0);
	if (!u.dx && !u.dy) {
		return(0);
	}
	
	dx = u.dx;
	dy = u.dy;

	doblitzlist();

    	for (i= 0; i < MAX_BLITZ; i++) {
		if (!getdir(Enter_Blitz)) return(0); /* Get directional input */
    		if (!u.dx && !u.dy && !u.dz) break;
    		if (u.dx == -1) {
    			*(bp) = 'L';
    			bp++;
    		} else if (u.dx == 1) {
    			*(bp) = 'R';
    			bp++;
    		}
    		if (u.dy == -1) {
    			*(bp) = 'U';
    			bp++;
    		} else if (u.dy == 1) {
    			*(bp) = 'D';
    			bp++;
    		}
    		if (u.dz == -1) {
    			*(bp) = '>';
    			bp++;
    		} else if (u.dz == 1) {
    			*(bp) = '<';
    			bp++;
    		}
    	}
	*(bp) = '.';
	bp++;
	*(bp) = '\0';
	bp = buf;

	/* Point of no return - You've entered and terminated a blitz, so... */
    	u.uen -= 10;

    	/* parse input */
    	/* You can't put two of the same blitz in a row */
    	blitz_num = 0;
    	while(strncmp(bp, ".", 1)) {
	    bdone = 0;
	    for (j = 0; blitzes[j].blitz_len; j++) {
	    	if (blitz_num >= MAX_CHAIN || 
	    	    blitz_num >= (MIN_CHAIN + (techlev(tech_no) / 10)))
	    		break; /* Trying to chain too many blitz commands */
		else if (!strncmp(bp, blitzes[j].blitz_cmd, blitzes[j].blitz_len)) {
	    		/* Trying to chain in a command you don't know yet */
			if (!tech_known(blitzes[j].blitz_tech))
				break;
	    		if (blitz_num) {
				/* Check if trying to chain two of the exact same 
				 * commands in a row 
				 */
	    			if (j == blitz_chain[(blitz_num - 1)]) 
	    				break;
	    			/* Trying to chain after chain finishing command */
	    			if (blitzes[blitz_chain[(blitz_num - 1)]].blitz_type 
	    							== BLITZ_END)
	    				break;
	    			/* Trying to put a chain starter after starting
	    			 * a chain
	    			 * Note that it's OK to put two chain starters in a 
	    			 * row
	    			 */
	    			if ((blitzes[j].blitz_type == BLITZ_START) &&
	    			    (blitzes[blitz_chain[(blitz_num - 1)]].blitz_type 
	    							!= BLITZ_START))
	    				break;
	    		}
			bp += blitzes[j].blitz_len;
			blitz_chain[blitz_num] = j;
			blitz_num++;
			bdone = 1;
			break;
		}
	    }
	    if (!bdone) {
		You("stumble!");
		return(1);
	    }
    	}
	for (i = 0; i < blitz_num; i++) {
	    u.dx = dx;
	    u.dy = dy;
	    if (!( (*blitzes[blitz_chain[i]].blitz_funct)() )) break;
	}
	
    	/* done */
	return(1);
}

static void
doblitzlist()
{
	winid tmpwin;
	int i, n;
	char buf[BUFSZ];
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;         /* zero out all bits */

        Sprintf(buf, "%16s %10s %-17s", "[LU = Left Up]", "[U = Up]", "[RU = Right Up]");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, MENU_UNSELECTED);
        Sprintf(buf, "%16s %10s %-17s", "[L = Left]", "", "[R = Right]");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, MENU_UNSELECTED);
        Sprintf(buf, "%16s %10s %-17s", "[LD = Left Down]", "[D = Down]", "[RD = Right Down]");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, MENU_UNSELECTED);        

        Sprintf(buf, "%-30s %10s   %s", "Name", "Type", "Command");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, MENU_UNSELECTED);

        for (i = 0; blitzes[i].blitz_len; i++) {
	    if (tech_known(blitzes[i].blitz_tech)) {
                Sprintf(buf, "%-30s %10s   %s",
                    (i && blitzes[i].blitz_tech == blitzes[(i-1)].blitz_tech ?
                    	"" : tech_names[blitzes[i].blitz_tech]), 
                    (blitzes[i].blitz_type == BLITZ_START ? 
                    	"starter" :
                    	(blitzes[i].blitz_type == BLITZ_CHAIN ? 
	                    	"chain" : 
	                    	(blitzes[i].blitz_type == BLITZ_END ? 
                    			"finisher" : "unknown"))),
                    blitzes[i].blitz_cmd);

		add_menu(tmpwin, NO_GLYPH, &any,
                         0, 0, ATR_NONE, buf, MENU_UNSELECTED);
	    }
	}
        end_menu(tmpwin, "Currently known blitz manoeuvers");

	n = select_menu(tmpwin, PICK_NONE, &selected);
	destroy_nhwindow(tmpwin);
	return;
}

static int
blitz_chi_strike()
{
	int tech_no;
	
	tech_no = (get_tech_no(T_CHI_STRIKE));

	if (tech_no == -1) {
		return(0);
	}

	if (u.uen < 1) {
		You("are too weak to attempt this! You need at least one point of mana!");
            	return(0);
	}
	You("feel energy surge through your hands!");
	techt_inuse(tech_no) = techlev(tech_no) + 4;
	return(1);
}

static int
blitz_e_fist()
{
	int tech_no;
	const char *str;
	
	tech_no = (get_tech_no(T_E_FIST));

	if (tech_no == -1) {
		return(0);
	}
	
	str = makeplural(body_part(HAND));
	You("focus the powers of the elements into your %s.", str);
	techt_inuse(tech_no) = rnd((int) (techlev(tech_no)/3 + 1)) + d(1,4) + 2;
	return 1;
}

/* Assumes u.dx, u.dy already set up */
static int
blitz_pummel()
{
	int i = 0, tech_no;
	struct monst *mtmp;
	tech_no = (get_tech_no(T_PUMMEL));

	if (tech_no == -1) {
		return(0);
	}

	You("let loose a barrage of blows!");

	if (u.uswallow)
	    mtmp = u.ustuck;
	else
	    mtmp = m_at(u.ux + u.dx, u.uy + u.dy);

	if (!mtmp) {
		You("strike nothing.");
		return (0);
	}
	if (!attack(mtmp)) return (0);
	
	/* Perform the extra attacks
	 */
	for (i = 0; (i < 4); i++) {
	    if (rn2(70) > (techlev(tech_no) + 30)) break;

	    if (u.uswallow)
		mtmp = u.ustuck;
	    else
		mtmp = m_at(u.ux + u.dx, u.uy + u.dy);

	    if (!mtmp) return (1);
	    if (!attack(mtmp)) return (1);
	} 
	
	return(1);
}

/* Assumes u.dx, u.dy already set up */
static int
blitz_g_slam()
{
	int tech_no, tmp, canhitmon, objenchant;
	struct monst *mtmp;
	struct trap *chasm;

	tech_no = (get_tech_no(T_G_SLAM));

	if (tech_no == -1) {
		return(0);
	}

	mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
	if (!mtmp) {
		You("strike nothing.");
		return (0);
	}
	if (!attack(mtmp)) return (0);

	/* Slam the monster into the ground */
	mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
	if (!mtmp || u.uswallow) return(1);

	You("hurl %s downwards...", mon_nam(mtmp));
	if (Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)) return(1);

	if (need_four(mtmp)) canhitmon = 4;
	else if (need_three(mtmp)) canhitmon = 3;
	else if (need_two(mtmp)) canhitmon = 2;
	else if (need_one(mtmp)) canhitmon = 1;
	else canhitmon = 0;
	if (Upolyd) {
	    if (hit_as_four(&youmonst))	objenchant = 4;
	    else if (hit_as_three(&youmonst)) objenchant = 3;
	    else if (hit_as_two(&youmonst)) objenchant = 2;
	    else if (hit_as_one(&youmonst)) objenchant = 1;
	    else if (need_four(&youmonst)) objenchant = 4;
	    else if (need_three(&youmonst)) objenchant = 3;
	    else if (need_two(&youmonst)) objenchant = 2;
	    else if (need_one(&youmonst)) objenchant = 1;
	    else objenchant = 0;
	} else
	    objenchant = u.ulevel / 4;

	tmp = (5 + rnd(6) + (techlev(tech_no) / 5));
	
	chasm = maketrap(u.ux + u.dx, u.uy + u.dy, PIT, 0);
	if (chasm) {
	    if (!is_flyer(mtmp->data) && (!mtmp->egotype_flying) && !is_clinger(mtmp->data))
		mtmp->mtrapped = 1;
	    if (!chasm->hiddentrap) chasm->tseen = 1;
	    levl[u.ux + u.dx][u.uy + u.dy].doormask = 0;
	    pline("%s slams into the ground, creating a crater!", Monnam(mtmp));
	    tmp *= 2;
	}

	mselftouch(mtmp, "Falling, ", TRUE);
	if (!DEADMONSTER(mtmp)) {
	    if (objenchant < canhitmon)
		pline("%s doesn't seem to be harmed.", Monnam(mtmp));
	    else if ((mtmp->mhp -= tmp) <= 0) {
		if(!cansee(u.ux + u.dx, u.uy + u.dy))
		    pline("It is destroyed!");
		else {
		    You("destroy %s!", 	
		    	mtmp->mtame
			    ? x_monnam(mtmp, ARTICLE_THE, "poor",
				mtmp->mnamelth ? SUPPRESS_SADDLE : 0, FALSE)
			    : mon_nam(mtmp));
		}
		xkilled(mtmp,0);
	    }
	}

	return(1);
}

/* Assumes u.dx, u.dy already set up */
static int
blitz_dash()
{
	int tech_no;
	tech_no = (get_tech_no(T_DASH));

	if (tech_no == -1) {
		return(0);
	}
	
	if ((!Punished || carried(uball)) && !u.utrap)
	    You("dash forwards!");
	hurtle(u.dx, u.dy, 2, FALSE);
	multi = 0;		/* No paralysis with dash */
	return 1;
}

static int
blitz_power_surge()
{
	int tech_no, num;
	
	tech_no = (get_tech_no(T_POWER_SURGE));

	if (tech_no == -1) {
		return(0);
	}

	/* what the heck??? --Amy */
	/*if (Upolyd) {
		You("cannot tap into your full potential in this form.");
		return(0);
	}*/
    	You("tap into the full extent of your power!");
	num = 50 + (2 * techlev(tech_no));
    	techt_inuse(tech_no) = num + 1;
	u.uenmax += num;
	u.uen = u.uenmax;
	return 1;
}

/* Assumes u.dx, u.dy already set up */
static int
blitz_spirit_bomb()
{
	int tech_no, num;
	int sx = u.ux, sy = u.uy, i;
	
	tech_no = (get_tech_no(T_SPIRIT_BOMB));

	if (tech_no == -1) {
		return(0);
	}

	You("gather your energy...");
	
	if (u.uen < 10) {
		pline("But it fizzles out because you have less than 10 points of mana.");
		u.uen = 0;
	}

	num = 10 + (techlev(tech_no) / 5);
	num = (u.uen < num ? u.uen : num);
	
	u.uen -= num;
	
	for( i = 0; i < 2; i++) {		
	    if (!isok(sx,sy) || !cansee(sx,sy) || 
	    		IS_STWALL(levl[sx][sy].typ) || u.uswallow)
	    	break;

	    /* Display the center of the explosion */
	    tmp_at(DISP_FLASH, explosion_to_glyph(EXPL_MAGICAL, S_explode5));
	    tmp_at(sx, sy);
	    delay_output();
	    tmp_at(DISP_END, 0);

	    sx += u.dx;
	    sy += u.dy;
	}
	/* Magical Explosion */
	explode(sx, sy, 10, (d(3,6) + num), WAND_CLASS, EXPL_MAGICAL);
	return 1;
}

#ifdef DEBUG
void
wiz_debug_cmd() /* in this case, allow controlled loss of techniques */
{
	int tech_no, id, n = 0;
	long mask;
	if (gettech(&tech_no)) {
		id = techid(tech_no);
		if (id == NO_TECH) {
		    impossible("Unknown technique ([%d])?", tech_no);
		    return;
		}
		mask = tech_list[tech_no].t_intrinsic;
		if (mask & FROMOUTSIDE) n++;
		if (mask & FROMRACE) n++;
		if (mask & FROMEXPER) n++;
		if (!n) {
		    impossible("No intrinsic masks set (0x%lX).", mask);
		    return;
		}
		n = rn2(n);
		if (mask & FROMOUTSIDE && !n--) mask = FROMOUTSIDE;
		if (mask & FROMRACE && !n--) mask = FROMRACE;
		if (mask & FROMEXPER && !n--) mask = FROMEXPER;
		learntech(id, mask, -1);
		if (!tech_known(id))
		    You("lose the ability to perform %s.", tech_names[id]);
	}
}
#endif /* DEBUG */
