/*      SCCS Id: @(#)tech.c    3.2     98/Oct/30        */
/*      Original Code by Warren Cheung (Basis: spell.c, attrib.c) */
/*      Copyright (c) M. Stephenson 1988                          */
/* NetHack may be freely redistributed.  See license for details. */

/* All of the techs from cmd.c are ported here */

#include "hack.h"
#include "artilist.h"

/* #define DEBUG */		/* turn on for diagnostics */

static boolean gettech(int *);
static boolean dotechmenu(int, int *, int);
static void doblitzlist(void);
static int techeffects(int);
static int mon_to_zombie(int);
STATIC_PTR int tinker(void);
STATIC_PTR int charge_saber(void);
STATIC_PTR int draw_energy(void);
static const struct innate_tech * role_tech(void);
static const struct innate_tech * race_tech(void);
static int doblitz(void);
static int blitz_chi_strike(void);
static int blitz_e_fist(void);
static int blitz_pummel(void);
static int blitz_g_slam(void);
static int blitz_dash(void);
static int blitz_power_surge(void);
static int blitz_spirit_bomb(void);
static int blitz_combo_strike(void);
static int blitz_draining_punch(void);
static void maybe_tameX(struct monst *);

static NEARDATA schar delay;            /* moves left for tinker/energy draw */
static NEARDATA const char revivables[] = { ALLOW_FLOOROBJ, FOOD_CLASS, 0 };
static const char all_count[] = { ALLOW_COUNT, ALL_CLASSES, 0 };
static const char allnoncount[] = { ALL_CLASSES, 0 };
static NEARDATA const char recharge_type[] = { ALLOW_COUNT, ALL_CLASSES, 0 };

static int tech_in_memory;

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
	"secure identify",
	"iron skin",
	"polyform",
	"concentrating",
	"summon pet",
	"double thrownage",
	"shield bash",
	"recharge",
	"spirituality check",
	"eddy wind",
	"blood ritual",
	"ent's potion",
	"lucky gamble",
	"panic digging",
	"decontaminate",
	"wonderspell",
	"reset technique",
	"silent ocean",
	"glowhorn",
	"intrinsic roulette",
	"spectral sword",
	"reverse identify",
	"detect traps",
	"directive",
	"remove implant",
	"reroll implant",
	"time stop",
	"stat resist",
	"swap weapon",
	"milden curse",
	"force field",
	"pointiness",
	"bug spray",
	"whirlstaff",
	"deliberate curse",
	"acquire steed",
	"saddling",
	"shopping queen",
	"beauty charm",
	"asian kick",
	"legscratch attack",
	"ground stomp",
	"athletic combat",
	"praying success",
	"over-ray",
	"enchanterang",
	"batman arsenal",
	"jokerbane",
	"call the police",
	"dominate",
	"incarnation",
	"combo strike",
	"fungoism",
	"become undead",
	"jiu-jitsu",
	"blade anger",
	"re-taming",
	"uncurse saber",
	"energy conservation",
	"enchant robe",
	"wild slashing",
	"absorber shield",
	"psycho force",
	"intensive training",
	"surrender or die",
	"perilous whirl",
	"summon shoe",
	"kick in the nuts",
	"disarming kick",
	"inlay warfare",
	"diamond barrier",
	"steady hand",
	"force filling",
	"jedi tailoring",
	"intrinsic sacrifice",
	"beamsword",
	"energy transfer",
	"soften target",
	"protect weapon",
	"powerful aura",
	"boostaff",
	"clone javelin",
	"refuge",
	"draining punch",
	"escrobism",
	"pirate brothering",
	"nuts and bolts",
	"decapability",
	"no-hands curse",
	"high-heeled sneakers",
	"form choice",
	"star digging",
	"starwars friends",
	"use the force luke",
	"wonder yonder",
	"zap em",
	"card trick",
	"skillomorph",
	"shotty blast",
	"ammo upgrade",
	"laser power",
	"big daddy",
	"shut that bitch up",
	"s-pressing",
	"meltee",
	"woman noises",
	"extra long squeak",
	"sexual hug",
	"sex change",
	"even more ammo",
	"doubleself",
	"polyfix",
	"squeaky repair",
	"bulletreuse",
	"extracharge",
	"terrain cleanup",
	"symbiosis",
	"adjust symbiote",
	"heal symbiote",
	"boost symbiote",
	"powerbiosis",
	"implanted symbiosis",
	"assume symbiote",
	"generate offspring",
	"undertow",
	"definalize",
	"anti inertia",
	"grenades of course",
	"preaching",
	"on the same team",
	"permamorph",
	"pacify",
	"afterburner",
	"buggard",
	"thunderstorm",
	"autokill",
	"chain thunderbolt",
	"flashing mischief",
	"kamehameha",
	"shadow mantle",
	"vacuum star",
	"blade shield",
	"green weapon",
	"ballsliff",
	"pole melee",
	"chop chop",
	"banishment",
	"participation loss",
	"weapon blocker",
	"extra memory",
	"grap swap",
	"diabolic minion",
	"cure amnesia",
	"elemental imbue",
	"hidden power",
	"sword art",
	"firm cudgel",
	"lighter balls",
	"venom mixing",
	"javelin forging",
	"coronation/culmination",
	"restore life levels",
	"make a wish",
	"toilet visit",
	"rock-to-poison",
	"injection",
	"super poison",
	"green missile",
	"big'n'veiny",
	"plant terror",
	"poison pen letter",
	"unarmed focus",
	"spell spam",
	"drop boulder",
	"earthshake",
	"martial style",
	"sprint",
	"skull crush",
	"female combo",
	"docklock",
	"bitch posing",
	"sexy stand",
	"marathon",
	"perfume stride",
	"naughty heelot",
	"extreme sturdiness",
	"butt protection",
	"profiling",
	"star heel swap",
	"heel stab",
	"hardcore alienization",
	"jedi jump",
	"charge saber",
	"telekinesis",
	""
};

static const struct innate_tech 
	/* Roles */
	arc_tech[] = { {   1, T_RESEARCH, 1},
		       {   0, 0, 0} },
	ass_tech[] = { {   1, T_CUTTHROAT, 1},
		       {   25, T_BLOOD_RITUAL, 1},
		       {   0, 0, 0} },
	lad_tech[] = { {   1, T_ATTIRE_CHARM, 1},
		       {   0, 0, 0} },
	gee_tech[] = { {   15, T_ZAP_EM, 1},
		       {   0, 0, 0} },
	fnc_tech[] = { {   1, T_CRIT_STRIKE, 1},
		       {   1, T_EDDY_WIND, 1},
		       {   0, 0, 0} },
	fig_tech[] = { {   1, T_BERSERK, 1},
		       {   4, T_CONCENTRATING, 1},
		       {   8, T_IRON_SKIN, 1},
		       {   10, T_SHIELD_BASH, 1},
		       {   18, T_ZAP_EM, 1},
		       {   0, 0, 0} },
	sex_tech[] = { {   1, T_ATTIRE_CHARM, 1},
		       {   1, T_POKE_BALL, 1},
		       {   26, T_SUMMON_PET, 1},
		       {   0, 0, 0} },
	zoo_tech[] = { {   12, T_SUMMON_PET, 1},
		       {   0, 0, 0} },
	jes_tech[] = { {   10, T_CARD_TRICK, 1},
			 {   30, T_EGG_BOMB, 1},
		       {   0, 0, 0} },
	stu_tech[] = { {   1, T_LIQUID_LEAP, 1},
		       {   1, T_JEDI_JUMP, 1},
		       {   5, T_POLYFORM, 1},
		       {   15, T_EDDY_WIND, 1},
		       {   0, 0, 0} },
	gun_tech[] = { {   1, T_CREATE_AMMO, 1},
		       {   1, T_SURGERY, 1},
		       {   0, 0, 0} },
	mar_tech[] = { {   1, T_CREATE_AMMO, 1},
		       {   0, 0, 0} },
	cli_tech[] = { {   16, T_SURGERY, 1},
		       {   0, 0, 0} },
	wom_tech[] = { {   20, T_ATTIRE_CHARM, 1},
		       {   26, T_CARD_TRICK, 1},
		       {   0, 0, 0} },
	ana_tech[] = { {   1, T_CREATE_AMMO, 1},
		       {   1, T_DECONTAMINATE, 1},
		       {   10, T_SKILLOMORPH, 1},
			 {  20, T_TERRAIN_CLEANUP, 1},
		       {   0, 0, 0} },
	lib_tech[] = { {   1, T_RESEARCH, 1},
		       {   14, T_SPELL_SPAM, 1},
		       {   20, T_RECHARGE, 1},
		       {   20, T_WONDERSPELL, 1},
		       {   0, 0, 0} },
	med_tech[] = { {   1, T_TELEKINESIS, 1},
		       {   1, T_ON_THE_SAME_TEAM, 1},
		       {   10, T_POLYFORM, 1},
		       {   15, T_SPIRITUALITY_CHECK, 1},
		       {   0, 0, 0} },
	dol_tech[] = { {   1, T_ATTIRE_CHARM, 1},
		       {   7, T_RESEARCH, 1},
		       {   10, T_POKE_BALL, 1},
		       {   20, T_SUMMON_PET, 1},
		       {   25, T_CARD_TRICK, 1},
		       {   0, 0, 0} },
	ama_tech[] = { {   1, T_FLURRY, 1},
		       {   1, T_PRACTICE, 1},
		       {   5, T_CRIT_STRIKE, 1},
		       {   15, T_VANISH, 1},
		       {   18, T_CONCENTRATING, 1},
		       {   20, T_SKILLOMORPH, 1},
		       {   25, T_ATTIRE_CHARM, 1},
		       {   0, 0, 0} },
	art_tech[] = { {   10, T_DIAMOND_BARRIER, 1},
			 {   15, T_TERRAIN_CLEANUP, 1},
		       {   18, T_POLYFORM, 1},
		       {   20, T_BLOOD_RITUAL, 1},
		       {   0, 0, 0} },
	rin_tech[] = { {   1, T_BLINK, 1},
		       {   1, T_LUCKY_GAMBLE, 1},
		       {   0, 0, 0} },

	bos_tech[] = { {   1, T_FLURRY, 1},
		       {   1, T_DOUBLE_THROWNAGE, 1},
		       {   5, T_VANISH, 1},
		       {   10, T_ENT_S_POTION, 1},
		       {   15, T_BLINK, 1},
		       {   30, T_TELEKINESIS, 1},
		       {   0, 0, 0} },

	bul_tech[] = { {   6, T_IRON_SKIN, 1},
		       {   0, 0, 0} },

	sta_tech[] = { {   4, T_SKILLOMORPH, 1},
			 {   12, T_DECONTAMINATE, 1},
		       {   16, T_DIAMOND_BARRIER, 1},
		       {   20, T_ZAP_EM, 1},
		       {   26, T_SPELL_SPAM, 1},
		       {   0, 0, 0} },

	ord_tech[] = { {   1, T_PRACTICE, 1},
		       {   1, T_TURN_UNDEAD, 1},
		       {   5, T_CONCENTRATING, 1},
		       {   8, T_DOUBLE_THROWNAGE, 1},
		       {   10, T_TINKER, 1},
		       {   12, T_EDDY_WIND, 1},
		       {   0, 0, 0} },

	dun_tech[] = { {   1, T_CUTTHROAT, 1},
		       {   5, T_CONCENTRATING, 1},
		       {   10, T_CRIT_STRIKE, 1},
		       {   15, T_SHIELD_BASH, 1},
		       {   20, T_RAISE_ZOMBIES, 1},
		       {   25, T_BLOOD_RITUAL, 1},
		       {   0, 0, 0} },

	alt_tech[] = { {   1, T_BLESSING, 1},
		       {   1, T_SIGIL_CONTROL, 1},
		       {   1, T_SIGIL_TEMPEST, 1},
		       {   1, T_SIGIL_DISCHARGE, 1},
		       {   1, T_DRAW_ENERGY, 1},
		       {   1, T_POWER_SURGE, 1},
		       {   10, T_E_FIST, 1},
		       {   10, T_CHI_STRIKE, 1},
		       {   15, T_WONDERSPELL, 1},
		       {   20, T_BLITZ, 1},
		       {   25, T_REVIVE, 1},
		       {   0, 0, 0} },

	tha_tech[] = { {   5, T_DAZZLE, 1},
		       {   10, T_WARD_FIRE, 1},
		       {   10, T_DRAW_ENERGY, 1},
		       {   10, T_IRON_SKIN, 1},
			 {   15, T_LIQUID_LEAP, 1},
			 {   15, T_WARD_ELEC, 1},
			 {   20, T_WARD_COLD, 1},
		       {   20, T_POWER_SURGE, 1},
		       {   20, T_EDDY_WIND, 1},
		       {   27, T_SPELL_SPAM, 1},
		       {   0, 0, 0} },

	gof_tech[] = { {   1, T_DRAW_BLOOD, 1},
		       {   1, T_CALM_STEED, 1},
		       {   1, T_DAZZLE, 1},
		       {   1, T_TURN_UNDEAD, 1},
		       {   1, T_CREATE_AMMO, 1},
		       {   1, T_ATTIRE_CHARM, 1},
		       {   1, T_BLOOD_RITUAL, 1},
		       {   5, T_VANISH, 1},
		       {   10, T_FLURRY, 1},
		       {   15, T_REVIVE, 1},
		       {   20, T_RAISE_ZOMBIES, 1},
		       {   25, T_BOOZE, 1},
		       {   30, T_TELEKINESIS, 1},
		       {   0, 0, 0} },
	dru_tech[] = { {   1, T_BOOZE, 1}, /* drunk */
			 {   20, T_ENT_S_POTION, 1},
		       {   0, 0, 0} },
	drd_tech[] = { {   7, T_ENT_S_POTION, 1},
		       {   18, T_SPELL_SPAM, 1},
		       {   28, T_ON_THE_SAME_TEAM, 1},
		       {   0, 0, 0} },
	sof_tech[] = { {   5, T_ZAP_EM, 1},
			 {   10, T_TINKER, 1},
		       {   0, 0, 0} },
	jan_tech[] = { {   1, T_IRON_SKIN, 1},
		       {   1, T_CONCENTRATING, 1},
			 {   1, T_TERRAIN_CLEANUP, 1},
		       {   20, T_ZAP_EM, 1},
		       {   0, 0, 0} },
	sma_tech[] = { {   1, T_CREATE_AMMO, 1},
		       {   10, T_PRACTICE, 1},
		       {   12, T_ZAP_EM, 1},
		       {   15, T_CHARGE_SABER, 1},
		       {   0, 0, 0} },
	sto_tech[] = { {   1, T_BERSERK, 1},
		       {   1, T_ZAP_EM, 1},
		       {   5, T_SKILLOMORPH, 1},
		       {   10, T_KIII, 1},
		       {   12, T_DOUBLE_THROWNAGE, 1},
		       {   15, T_VANISH, 1},
		       {   20, T_SIGIL_TEMPEST, 1},
		       {   24, T_EDDY_WIND, 1},
		       {   0, 0, 0} },
	cra_tech[] = { {   1, T_ZAP_EM, 1},
			 {   10, T_CARD_TRICK, 1},
			 {   15, T_RESEARCH, 1},
		       {   20, T_RECHARGE, 1},
		       {   30, T_DOUBLE_TROUBLE, 1},
		       {   0, 0, 0} },
	fir_tech[] = { {   12, T_RECHARGE, 1},
		       {   0, 0, 0} },
	yau_tech[] = { {   2, T_BLOOD_RITUAL, 1},
			 {   8, T_CRIT_STRIKE, 1},
		       {   10, T_SIGIL_DISCHARGE, 1},
		       {   12, T_CHI_HEALING, 1},
		       {   15, T_ZAP_EM, 1},
		       {   18, T_DRAW_ENERGY, 1},
		       {   25, T_TELEKINESIS, 1},
		       {   0, 0, 0} },
	gam_tech[] = { {   1, T_CONCENTRATING, 1},
		       {   13, T_LUCKY_GAMBLE, 1},
		       {   20, T_SUMMON_PET, 1},
		       {   30, T_RESET_TECHNIQUE, 1},
		       {   0, 0, 0} },
	joc_tech[] = { {   1, T_CALM_STEED, 1},
			 {   5, T_DASH, 1},
		       {   0, 0, 0} },
	gla_tech[] = { {   5, T_SHIELD_BASH, 1},
			 {   10, T_IRON_SKIN, 1},
		       {   0, 0, 0} },
	bar_tech[] = { {   1, T_BERSERK, 1},
		       {   8, T_DOUBLE_THROWNAGE, 1},
		       {   10, T_CONCENTRATING, 1},
		       {   10, T_IRON_SKIN, 1},
		       {   20, T_EDDY_WIND, 1},
		       {   0, 0, 0} },
	noo_tech[] = { {   1, T_BERSERK, 1},
		       {   8, T_DOUBLE_THROWNAGE, 1},
		       {   10, T_CONCENTRATING, 1},
		       {   10, T_IRON_SKIN, 1},
		       {   20, T_EDDY_WIND, 1},
		       {   0, 0, 0} },
	blo_tech[] = { {   1, T_EDDY_WIND, 1},
		       {   10, T_BLOOD_RITUAL, 1},
		       {   0, 0, 0} },
	gol_tech[] = { {   1, T_VANISH, 1},
		       {   7, T_TINKER, 1},
		       {   0, 0, 0} },
	ble_tech[] = { {   1, T_SURGERY, 1},
		       {   1, T_HEAL_HANDS, 1},
		       {   1, T_DRAW_BLOOD, 1},
		       {   10, T_CHI_HEALING, 1},
		       {   20, T_REVIVE, 1},
		       {   30, T_BLOOD_RITUAL, 1},
		       {   0, 0, 0} },
	mid_tech[] = { {   1, T_RAGE, 1},
		       {   0, 0, 0} },
	psy_tech[] = { {   1, T_ZAP_EM, 1},
			 {   2, T_TURN_UNDEAD, 1},
			 {   7, T_REINFORCE, 1},
			 {   12, T_E_FIST, 1},
			 {   12, T_SIGIL_CONTROL, 1},
			 {   15, T_SPIRITUALITY_CHECK, 1},
			 {   18, T_DRAW_ENERGY, 1},
		       {   18, T_SPELL_SPAM, 1},
			 {   20, T_SPIRIT_BOMB, 1},
			 {   24, T_BLESSING, 1},
			 {   28, T_SUMMON_PET, 1},
		       {   0, 0, 0} },
	sag_tech[] = { {   4, T_SPELL_SPAM, 1},
		       {   0, 0, 0} },
	spa_tech[] = { {   1, T_REINFORCE, 1},
			 {   5, T_RESEARCH, 1},
			 {   6, T_RECHARGE, 1},
		       {  8, T_WARD_FIRE, 1},
		       {  10, T_CHARGE_SABER, 1},
		       {  11, T_WARD_COLD, 1},
		       {  14, T_WARD_ELEC, 1},
		       {  20, T_DRAW_ENERGY, 1},
		       {   0, 0, 0} },
	cam_tech[] = { {   12, T_CREATE_AMMO, 1},
			 {   26, T_RECHARGE, 1},
		       {   0, 0, 0} },
	qua_tech[] = { {   12, T_ZAP_EM, 1},
		       {   0, 0, 0} },
	gan_tech[] = { {   1, T_CREATE_AMMO, 1},
		       {   1, T_LUCKY_GAMBLE, 1},
		       {   0, 0, 0} },
	emp_tech[] = { {   1, T_SURGERY, 1},
		       {   1, T_REINFORCE, 1},
		       {   1, T_HEAL_HANDS, 1},
		       {   1, T_CALM_STEED, 1},
		       {   3, T_CHI_HEALING, 1},
		       {   5, T_TURN_UNDEAD, 1},
		       {   7, T_DRAW_BLOOD, 1},
		       {   8, T_ZAP_EM, 1},
		       {   10, T_DRAW_ENERGY, 1},
		       {   15, T_SPIRITUALITY_CHECK, 1},
		       {   18, T_PREACHING, 1},
		       {   20, T_ATTIRE_CHARM, 1},
		       {   20, T_ENT_S_POTION, 1},
		       {   25, T_REVIVE, 1},
		       {   30, T_WONDERSPELL, 1},
		       {   0, 0, 0} },
	nuc_tech[] = { {   1, T_DECONTAMINATE, 1},
			 {   14, T_PERMAMORPH, 1},
		       {   0, 0, 0} },
	for_tech[] = { {   20, T_PERMAMORPH, 1},
		       {   0, 0, 0} },
	gen_tech[] = { {   12, T_PREACHING, 1},
		       {   0, 0, 0} },
	mam_tech[] = { {   1, T_REINFORCE, 1},
		       {   1, T_RESEARCH, 1},
		       {   1, T_ZAP_EM, 1},
		       {   1, T_SPELL_SPAM, 1},
		       {   10, T_CONCENTRATING, 1},
		       {   10, T_POWER_SURGE, 1},
		       {   15, T_WONDERSPELL, 1},
		       {   18, T_TELEKINESIS, 1},
		       {   0, 0, 0} },
	sco_tech[] = { {   25, T_DIAMOND_BARRIER, 1},
		       {   0, 0, 0} },
	wal_tech[] = { {   1, T_DIAMOND_BARRIER, 1},
		       {   0, 0, 0} },
	off_tech[] = { {   10, T_ON_THE_SAME_TEAM, 1},
		       {   0, 0, 0} },
	wei_tech[] = { {   2, T_POLYFORM, 1},
			 {   3, T_SIGIL_DISCHARGE, 1},
			 {   4, T_G_SLAM, 1},
			 {   7, T_FLURRY, 1},
			 {   8, T_LUCKY_GAMBLE, 1},
			 {   9, T_SUMMON_TEAM_ANT, 1},
			 {   10, T_CARD_TRICK, 1},
		       {   11, T_CALM_STEED, 1},
		       {   14, T_HARDCORE_ALIENIZATION, 1},
		       {   17, T_RAISE_ZOMBIES, 1},
		       {   18, T_DIAMOND_BARRIER, 1},
		       {   19, T_CHI_STRIKE, 1},
		       {   20, T_PREACHING, 1},
		       {   22, T_ZAP_EM, 1},
		       {   23, T_BLITZ, 1},
		       {   24, T_EVISCERATE, 1},
		       {   29, T_WORLD_FALL, 1},
		       {   30, T_TURN_UNDEAD, 1},
		       {   0, 0, 0} },
	loc_tech[] = { {   15, T_DIAMOND_BARRIER, 1},
		       {   0, 0, 0} },
	roc_tech[] = { {   1, T_EGG_BOMB, 1},
		       {   1, T_FLURRY, 1},
		       {   0, 0, 0} },
	cel_tech[] = { {   6, T_SPELL_SPAM, 1},
			 {   10, T_TERRAIN_CLEANUP, 1},
			 {   24, T_PERMAMORPH, 1},
		       {   0, 0, 0} },
	ast_tech[] = { {   5, T_FLURRY, 1},
			 {   6, T_SIGIL_CONTROL, 1},
			 {   8, T_SHIELD_BASH, 1},
			 {   10, T_REINFORCE, 1},
			 {   10, T_ZAP_EM, 1},
			 {   12, T_RESEARCH, 1},
			 {   18, T_SIGIL_TEMPEST, 1},
			 {   18, T_CHARGE_SABER, 1},
			 {   20, T_TINKER, 1},
			 {   20, T_WORLD_FALL, 1},
			 {   24, T_DECONTAMINATE, 1},
		       {   24, T_SURGERY, 1},
		       {   24, T_DRAW_ENERGY, 1},
			 {   25, T_SIGIL_DISCHARGE, 1},
			 {   25, T_SKILLOMORPH, 1},
			 {   28, T_TERRAIN_CLEANUP, 1},
			 {   29, T_BLINK, 1},
		       {   30, T_LIQUID_LEAP, 1},
		       {   0, 0, 0} },
	dea_tech[] = { {   1, T_WORLD_FALL, 1},
		       {   1, T_RECHARGE, 1},
		       {   1, T_BLOOD_RITUAL, 1},
		       {   18, T_SPELL_SPAM, 1},
		       {   0, 0, 0} },
	div_tech[] = { {   15, T_POLYFORM, 1},
		       {   0, 0, 0} },
	cav_tech[] = { {   1, T_PRIMAL_ROAR, 1},
		       {   10, T_SHIELD_BASH, 1},
		       {   20, T_POLYFORM, 1},
		       {   0, 0, 0} },
	dem_tech[] = { {   1, T_PRIMAL_ROAR, 1},
		       {   25, T_PREACHING, 1},
		       {   0, 0, 0} },
	sli_tech[] = { {   1, T_LIQUID_LEAP, 1},
		       {   4, T_SPIRITUALITY_CHECK, 1},
		       {   6, T_HEAL_HANDS, 1},
		       {   10, T_LUCKY_GAMBLE, 1},
		       {   12, T_WORLD_FALL, 1},
		       {   15, T_TELEKINESIS, 1},
		       {   18, T_IRON_SKIN, 1},
		       {   19, T_SUMMON_TEAM_ANT, 1},
		       {   21, T_DECONTAMINATE, 1},
		       {   23, T_RECHARGE, 1},
		       {   25, T_VANISH, 1},
		       {   26, T_ROCK_TO_POISON, 1},
		       {   28, T_POLYFORM, 1},
		       {   30, T_EGG_BOMB, 1},
		       {   0, 0, 0} },
	erd_tech[] = { {   1, T_CRIT_STRIKE, 1},
		       {   10, T_PRACTICE, 1},
		       {   20, T_ATTIRE_CHARM, 1},
		       {   0, 0, 0} },
	brd_tech[] = { {   1, T_PRIMAL_ROAR, 1},
		       {  10, T_BOOZE, 1},
			 {  15, T_ON_THE_SAME_TEAM, 1},
		       {  20, T_RAGE, 1},
		       {   0, 0, 0} },
	hus_tech[] = { {  20, T_ROCK_TO_POISON, 1},
		       {   0, 0, 0} },
	aci_tech[] = { {   1, T_REINFORCE, 1},
		       {   3, T_POWER_SURGE, 1},
		       {   5, T_DRAW_ENERGY, 1},
		       {   7, T_SIGIL_CONTROL, 1},
		       {  10, T_SIGIL_TEMPEST, 1},
		       {  10, T_SPELL_SPAM, 1},
		       {  18, T_ROCK_TO_POISON, 1},
		       {  20, T_SIGIL_DISCHARGE, 1},
		       {   0, 0, 0} },
	fla_tech[] = { {   1, T_REINFORCE, 1},
		       {   3, T_POWER_SURGE, 1},
		       {   5, T_DRAW_ENERGY, 1},
		       {   8, T_SPELL_SPAM, 1},
		       {  10, T_SIGIL_TEMPEST, 1},
		       {  20, T_SIGIL_DISCHARGE, 1},
		       {   0, 0, 0} },
	ele_tech[] = { {   1, T_REINFORCE, 1},
		       {   3, T_POWER_SURGE, 1},
		       {   5, T_DRAW_ENERGY, 1},
		       {   7, T_SIGIL_CONTROL, 1},
		       {  10, T_SPELL_SPAM, 1},
		       {  10, T_SIGIL_TEMPEST, 1},
		       {  20, T_SIGIL_DISCHARGE, 1},
		       {   0, 0, 0} },
	psn_tech[] = { {   1, T_REINFORCE, 1},
		       {   1, T_SIGIL_CONTROL, 1},
		       {   1, T_SIGIL_TEMPEST, 1},
		       {   1, T_SIGIL_DISCHARGE, 1},
		       {   1, T_ROCK_TO_POISON, 1},
		       {   3, T_POWER_SURGE, 1},
		       {   5, T_DRAW_ENERGY, 1},
		       {   7, T_RECHARGE, 1},
		       {   14, T_SPELL_SPAM, 1},
		       {   0, 0, 0} },
	hea_tech[] = { {   1, T_SURGERY, 1},
		       {  10, T_ENT_S_POTION, 1},
		       {  20, T_REVIVE, 1},
		       {  30, T_ON_THE_SAME_TEAM, 1},
		       {   0, 0, 0} },
	ice_tech[] = { {   1, T_REINFORCE, 1},
		       {   5, T_DRAW_ENERGY, 1},
		       {   8, T_SPELL_SPAM, 1},
		       {  10, T_SIGIL_TEMPEST, 1},
		       {  12, T_POWER_SURGE, 1},
		       {  20, T_SIGIL_DISCHARGE, 1},
		       {   0, 0, 0} },
	jed_tech[] = { {   1, T_JEDI_JUMP, 1},
		       {   5, T_CHARGE_SABER, 1},
		       {   8, T_TELEKINESIS, 1},
		       {   15, T_SHIELD_BASH, 1},
		       {   25, T_SPIRITUALITY_CHECK, 1},
		       {   0, 0, 0,} },
	kni_tech[] = { {   1, T_TURN_UNDEAD, 1},
		       {   1, T_HEAL_HANDS, 1},
		       {   1, T_SHIELD_BASH, 1},
		       {   26, T_SPELL_SPAM, 1},
		       {   0, 0, 0} },

	car_tech[] = { {   1, T_CARD_TRICK, 1},
		       {   15, T_LUCKY_GAMBLE, 1},
		       {   0, 0, 0} },

	dra_tech[] = { {   10, T_PRIMAL_ROAR, 1},
		       {   15, T_ON_THE_SAME_TEAM, 1},
		       {   20, T_CALM_STEED, 1},
		       {   30, T_SUMMON_PET, 1},
		       {   0, 0, 0} },

	com_tech[] = { {   2, T_SHIELD_BASH, 1},
			 {   11, T_KIII, 1},
		       {   15, T_CONCENTRATING, 1},
		       {   20, T_G_SLAM, 1},
		       {   25, T_SPIRIT_BOMB, 1},
		       {   0, 0, 0} },

	fjo_tech[] = { {   5, T_SURGERY, 1},
		       {   10, T_LIQUID_LEAP, 1},
		       {   20, T_REINFORCE, 1},
		       {   25, T_BOOZE, 1},
		       {   0, 0, 0} },

	pra_tech[] = { {   1, T_RESEARCH, 1},
		       {   14, T_ON_THE_SAME_TEAM, 1},
		       {   15, T_VANISH, 1},
		       {   25, T_RECHARGE, 1},
		       {   0, 0, 0} },

	eme_tech[] = { {   10, T_RAGE, 1},
		       {   0, 0, 0} },

	sym_tech[] = { {   12, T_TERRAIN_CLEANUP, 1},
		       {   0, 0, 0} },

	but_tech[] = { {   15, T_SUMMON_PET, 1},
			 {   20, T_ATTIRE_CHARM, 1},
		       {   0, 0, 0} },

	dan_tech[] = { {   1, T_BLITZ, 1},
			 {   1, T_E_FIST, 1},
			 {   3, T_IRON_SKIN, 1},
			 {   5, T_DASH, 1},
			 {   7, T_SHIELD_BASH, 1},
			 {   9, T_DIAMOND_BARRIER, 1},
			 {   11, T_SIGIL_DISCHARGE, 1},
			 {   13, T_SPIRIT_BOMB, 1},
			 {   15, T_PUMMEL, 1},
			 {   18, T_G_SLAM, 1},
			 {   20, T_ATTIRE_CHARM, 1},
			 {   22, T_TERRAIN_CLEANUP, 1},
			 {   25, T_CHI_STRIKE, 1},
			 {   27, T_JEDI_JUMP, 1},
			 {   30, T_CHI_HEALING, 1},
		       {   0, 0, 0} },

	kor_tech[] = { {   1, T_DOUBLE_THROWNAGE, 1},
		       {   0, 0, 0} },

	pre_tech[] = { {   1, T_TERRAIN_CLEANUP, 1},
		       {   0, 0, 0} },

	sec_tech[] = { {   1, T_RECHARGE, 1},
			 {   8, T_VANISH, 1},
			 {   10, T_CHARGE_SABER, 1},
			 {   12, T_REINFORCE, 1},
			 {   20, T_RESEARCH, 1},
			 {   20, T_BOOZE, 1},
			 {   24, T_CREATE_AMMO, 1},
			 {   25, T_SURGERY, 1},
			 {   28, T_POLYFORM, 1},
			 {   30, T_BLOOD_RITUAL, 1},
		       {   0, 0, 0} },

	dia_tech[] = { {   1, T_REINFORCE, 1},
			 {   1, T_CRIT_STRIKE, 1},
			 {   1, T_SHIELD_BASH, 1},
			 {   6, T_WARD_FIRE, 1},
			 {   6, T_TELEKINESIS, 1},
			 {   10, T_PRACTICE, 1},
			 {   12, T_WARD_COLD, 1},
			 {   12, T_DOUBLE_THROWNAGE, 1},
			 {   18, T_FLURRY, 1},
			 {   18, T_WARD_ELEC, 1},
			 {   18, T_VANISH, 1},
			 {   18, T_IRON_SKIN, 1},
			 {   18, T_CONCENTRATING, 1},
		       {   20, T_SPELL_SPAM, 1},
			 {   24, T_TURN_UNDEAD, 1},
			 {   30, T_BERSERK, 1},
			 {   30, T_PRIMAL_ROAR, 1},
			 {   30, T_REVIVE, 1},
			 {   30, T_EDDY_WIND, 1},
		       {   0, 0, 0} },

	occ_tech[] = { {   1, T_SIGIL_DISCHARGE, 1},
			 {   8, T_RECHARGE, 1},
			 {   12, T_REINFORCE, 1},
		       {   15, T_SPELL_SPAM, 1},
			 {   16, T_RESEARCH, 1},
			 {   18, T_SPIRITUALITY_CHECK, 1},
			 {   20, T_BLOOD_RITUAL, 1},
			 {   24, T_POWER_SURGE, 1},
		       {   0, 0, 0} },

	emn_tech[] = { {   1, T_E_FIST, 1},
			 {   1, T_SIGIL_CONTROL, 1},
			 {   5, T_WARD_COLD, 1},
			 {   8, T_REINFORCE, 1},
		       {   8, T_SPELL_SPAM, 1},
			 {   10, T_WARD_ELEC, 1},
			 {   12, T_TERRAIN_CLEANUP, 1},
			 {   15, T_WARD_FIRE, 1},
			 {   20, T_ENT_S_POTION, 1},
		       {   0, 0, 0} },

	cha_tech[] = { {   1, T_SIGIL_TEMPEST, 1},
			 {   4, T_REINFORCE, 1},
			 {   10, T_LUCKY_GAMBLE, 1},
			 {   12, T_PERMAMORPH, 1},
		       {   18, T_EDDY_WIND, 1},
		       {   20, T_WORLD_FALL, 1},
		       {   22, T_EVISCERATE, 1},
		       {   24, T_SPIRIT_BOMB, 1},
		       {   0, 0, 0} },

	jus_tech[] = { {   1, T_PREACHING, 1},
			 {   8, T_ZAP_EM, 1},
			 {   12, T_CONCENTRATING, 1},
		       {   16, T_SHIELD_BASH, 1},
		       {   19, T_SPELL_SPAM, 1},
		       {   0, 0, 0} },

	pal_tech[] = { {   1, T_TURN_UNDEAD, 1},
		       {   1, T_HEAL_HANDS, 1},
		       {   1, T_SHIELD_BASH, 1},
		       {   1, T_SPIRITUALITY_CHECK, 1},
		       {   1, T_PREACHING, 1},
		       {   18, T_CONCENTRATING, 1},
		       {   0, 0, 0} },

	wan_tech[] = { {   1, T_RECHARGE, 1},
			 {   30, T_WORLD_FALL, 1},
		       {   0, 0, 0} },

	war_tech[] = { {   1, T_CONCENTRATING, 1},
		       {   10, T_SHIELD_BASH, 1},
		       {   24, T_IRON_SKIN, 1},
		       {   0, 0, 0} },

	sci_tech[] = { {   1, T_RESEARCH, 1},
		       {   1, T_TINKER, 1},
		       {   10, T_SURGERY, 1},
		       {   20, T_SUMMON_TEAM_ANT, 1},
		       {   25, T_DRAW_ENERGY, 1},
		       {   30, T_EGG_BOMB, 1},
		       {   0, 0, 0} },
	sha_tech[] = { {   1, T_LIQUID_LEAP, 1},
		       {   15, T_POLYFORM, 1},
		       {   0, 0, 0} },

	sla_tech[] = { {   1, T_SUMMON_PET, 1},
		       {  12, T_LUCKY_GAMBLE, 1},
		       {   0, 0, 0} },

	mon_tech[] = { {   1, T_PUMMEL, 1},
		       {   1, T_DASH, 1},
		       {   1, T_BLITZ, 1},
		       {   2, T_CHI_STRIKE, 1},
	  	       {   4, T_CHI_HEALING, 1},
	  	       {   5, T_PREACHING, 1},
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
		       {  18, T_IRON_SKIN, 1},
		       {  18, T_CONCENTRATING, 1},
		       {  18, T_DOUBLE_THROWNAGE, 1},
		       {  18, T_SHIELD_BASH, 1},
		       {   0, 0, 0} },

	sai_tech[] = { {   1, T_PUMMEL, 1},
		       {   1, T_DASH, 1},
		       {   1, T_BLITZ, 1},
		       {   2, T_CHI_STRIKE, 1},
	  	       {   4, T_CHI_HEALING, 1},
	  	       {   6, T_E_FIST, 1},
		       {   8, T_DRAW_ENERGY, 1},
		       {   8, T_CONCENTRATING, 1},
		       {  10, T_G_SLAM, 1},
		       {  11, T_WARD_FIRE, 1},
		       {  13, T_WARD_COLD, 1},
		       {  15, T_WARD_ELEC, 1},
		       {  17, T_SPIRIT_BOMB, 1},
		       {  20, T_POWER_SURGE, 1},
		       {  24, T_EDDY_WIND, 1},
		       {   0, 0, 0} },

	psi_tech[] = { {   1, T_PUMMEL, 1},
		       {   1, T_DASH, 1},
		       {   2, T_CHI_STRIKE, 1},
	  	       {   4, T_CHI_HEALING, 1},
	  	       {   6, T_E_FIST, 1},
		       {   7, T_SPELL_SPAM, 1},
		       {   8, T_DRAW_ENERGY, 1},
		       {   8, T_TELEKINESIS, 1},
		       {  10, T_G_SLAM, 1},
		       {  11, T_WARD_FIRE, 1},
		       {  13, T_WARD_COLD, 1},
		       {  15, T_WARD_ELEC, 1},
		       {  15, T_ATTIRE_CHARM, 1},
		       {  17, T_SPIRIT_BOMB, 1},
		       {  20, T_POWER_SURGE, 1},
		       {  30, T_PERMAMORPH, 1},
		       {   0, 0, 0} },
	pok_tech[] = { {   1, T_POKE_BALL, 1},
		       {   2, T_PUMMEL, 1},
		       {   3, T_SPIRITUALITY_CHECK, 1},
		       {   4, T_DASH, 1},
		       {  5, T_WARD_FIRE, 1},
		       {   7, T_SHIELD_BASH, 1},
		       {   8, T_BLITZ, 1},
		       {  9, T_WARD_COLD, 1},
		       {  10, T_RAISE_ZOMBIES, 1},
		       {  11, T_LUCKY_GAMBLE, 1},
		       {  12, T_ZAP_EM, 1},
		       {  13, T_WARD_ELEC, 1},
	  	       {   15, T_CREATE_AMMO, 1},
	  	       {   16, T_E_FIST, 1},
	  	       {   17, T_EGG_BOMB, 1},
	  	       {   18, T_WORLD_FALL, 1},
		       {   19, T_SPELL_SPAM, 1},
		       {   20, T_DRAW_ENERGY, 1},
		       {   21, T_RECHARGE, 1},
		       {   22, T_RESEARCH, 1},
		       {   23, T_POLYFORM, 1},
		       {   24, T_EDDY_WIND, 1},
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
		       {  25, T_BLOOD_RITUAL, 1},
		       {   0, 0, 0} },
	abu_tech[] = { {   1, T_CONCENTRATING, 1},
		       {   1, T_IRON_SKIN, 1},
		       {   0, 0, 0} },
	pri_tech[] = { {   1, T_TURN_UNDEAD, 1},
		       {   1, T_BLESSING, 1},
		       {   1, T_PREACHING, 1},
		       {   1, T_SPIRITUALITY_CHECK, 1},
		       {   22, T_SPELL_SPAM, 1},
		       {   24, T_BLOOD_RITUAL, 1},
		       {   0, 0, 0} },
	che_tech[] = { {   1, T_TURN_UNDEAD, 1},
		       {   1, T_BLESSING, 1},
		       {   1, T_HEAL_HANDS, 1},
		       {   1, T_SPIRITUALITY_CHECK, 1},
		       {   1, T_PREACHING, 1},
		       {   0, 0, 0} },
	ran_tech[] = { {   1, T_FLURRY, 1},
		       {   0, 0, 0} },
	aug_tech[] = { {   1, T_TELEKINESIS, 1},
		       {   1, T_SPELL_SPAM, 1},
		       {   5, T_WONDERSPELL, 1},
		       {   10, T_SPIRITUALITY_CHECK, 1},
		       {   15, T_CARD_TRICK, 1},
		       {   20, T_DIAMOND_BARRIER, 1},
		       {   0, 0, 0} },
	elp_tech[] = { {   1, T_FLURRY, 1},
		       {   0, 0, 0} },
	rog_tech[] = { {   1, T_CRIT_STRIKE, 1},
		       {  15, T_CUTTHROAT, 1},
		       {   0, 0, 0} },
	sam_tech[] = { {   1, T_KIII, 1},
		       {   6, T_SHIELD_BASH, 1},
		       {   18, T_IRON_SKIN, 1},
		       {   18, T_CONCENTRATING, 1},
		       {   30, T_BLOOD_RITUAL, 1},
		       {   0, 0, 0} },
	cyb_tech[] = { {   1, T_KIII, 1},
		       {   5, T_ZAP_EM, 1},
		       {   6, T_SHIELD_BASH, 1},
		       {   8, T_CUTTHROAT, 1},
		       {   10, T_CHARGE_SABER, 1},
		       {   12, T_CRIT_STRIKE, 1},
		       {   18, T_IRON_SKIN, 1},
		       {   18, T_CONCENTRATING, 1},
		       {   30, T_BLOOD_RITUAL, 1},
		       {   0, 0, 0} },
	gro_tech[] = { {   1, T_KIII, 1},
		       {   1, T_CREATE_AMMO, 1},
		       {   6, T_SHIELD_BASH, 1},
		       {   18, T_IRON_SKIN, 1},
		       {   18, T_CONCENTRATING, 1},
		       {   30, T_BLOOD_RITUAL, 1},
		       {   0, 0, 0} },
	nin_tech[] = { {   5, T_DOUBLE_THROWNAGE, 1},
		       {   25, T_EDDY_WIND, 1},
		       {   0, 0, 0} },
	tou_tech[] = { /* Put Tech here */
		       {   0, 0, 0} },
	tra_tech[] = { {   1, T_ATTIRE_CHARM, 1},
		       {   17, T_SUMMON_PET, 1},
		       {   0, 0, 0} },
	kur_tech[] = { {   1, T_ATTIRE_CHARM, 1}, /* also used for prostitute */
		       {   0, 0, 0} },
	dis_tech[] = { {   5, T_BERSERK, 1},
		       {  8, T_DAZZLE, 1},
		       {  10, T_RAGE, 1},
		       {  15, T_ZAP_EM, 1},
		       {  22, T_ON_THE_SAME_TEAM, 1},
		       {   0, 0, 0} },
	mas_tech[] = { {   1, T_DIAMOND_BARRIER, 1}, /* mason */
		       {   0, 0, 0} },
	xel_tech[] = { { 4, T_SPELL_SPAM, 1},
			 {   6, T_EVISCERATE, 1},
			 {   8, T_CRIT_STRIKE, 1},
			 {   9, T_DASH, 1},
			 {   10, T_ZAP_EM, 1},
			 {   10, T_BLOOD_RITUAL, 1},
			 {   12, T_RESEARCH, 1},
		       {   16, T_ROCK_TO_POISON, 1},
			 {   18, T_TELEKINESIS, 1},
		       {  20, T_REINFORCE, 1},
		       {  24, T_POLYFORM, 1},
		       {  25, T_EGG_BOMB, 1},
		       {  26, T_PERMAMORPH, 1},
		       {  30, T_SKILLOMORPH, 1},
		       {   0, 0, 0} },
	trs_tech[] = { {   1, T_ATTIRE_CHARM, 1},
		       {   17, T_SUMMON_PET, 1},
		       {   0, 0, 0} },
	act_tech[] = { {   1, T_REINFORCE, 1},
		       {  5, T_VANISH, 1},
		       {  8, T_ON_THE_SAME_TEAM, 1},
		       {  10, T_DAZZLE, 1},
		       {  10, T_PREACHING, 1},
			 {  25, T_TERRAIN_CLEANUP, 1},
		       {   0, 0, 0} },
	top_tech[] = { {   1, T_ATTIRE_CHARM, 1},
		       {  10, T_CREATE_AMMO, 1},
		       {  20, T_LIQUID_LEAP, 1},
		       {  30, T_POLYFORM, 1},
		       {   0, 0, 0} },
	fai_tech[] = { {   1, T_ATTIRE_CHARM, 1},
		       {  29, T_CREATE_AMMO, 1},
		       {   0, 0, 0} },
	und_tech[] = { {   1, T_TURN_UNDEAD, 1},
		       {   1, T_PRACTICE, 1},
		       {   10, T_IRON_SKIN, 1},
		       {   0, 0, 0} },
	unb_tech[] = { {   1, T_TURN_UNDEAD, 1},
		       {   1, T_PRACTICE, 1},
		       {   3, T_SHIELD_BASH, 1},
		       {   6, T_IRON_SKIN, 1},
		       {   6, T_CONCENTRATING, 1},
		       {   12, T_DOUBLE_THROWNAGE, 1},
		       {   15, T_RECHARGE, 1},
		       {   18, T_EDDY_WIND, 1},
		       {   20, T_DECONTAMINATE, 1},
		       {   0, 0, 0} },
	unt_tech[] = { {   1, T_TURN_UNDEAD, 1},
		       {   5, T_BLOOD_RITUAL, 1},
		       {   14, T_SPIRITUALITY_CHECK, 1},
		       {   0, 0, 0} },
	val_tech[] = { {   1, T_PRACTICE, 1},
		       {   9, T_CONCENTRATING, 1},
		       {   14, T_RECHARGE, 1},
		       {   20, T_IRON_SKIN, 1},
		       {   0, 0, 0} },
	lun_tech[] = { {   1, T_EVISCERATE, 1},
		       {  10, T_BERSERK, 1},
		       {   0, 0, 0} },
	yeo_tech[] = {
		       {   1, T_CALM_STEED, 1},
		       {   8, T_IRON_SKIN, 1},
		       {   17, T_EDDY_WIND, 1},
		       {   25, T_SUMMON_PET, 1},
		       {   0, 0, 0} },
	wiz_tech[] = { {   1, T_REINFORCE, 1},
		       {   3, T_DRAW_ENERGY, 1},
		       {   5, T_POWER_SURGE, 1},
		       {   7, T_SIGIL_CONTROL, 1},
		       {  14, T_SIGIL_TEMPEST, 1},
		       {  20, T_SIGIL_DISCHARGE, 1},
		       {   0, 0, 0} },		       
	/* Races */

	/* dummy, for copypasting into new races --Amy */
	/*
	dummy_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	*/

	red_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   18, T_HARDCORE_ALIENIZATION, 1},
		       {   0, 0, 0} },

	sdi_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_BLITZ, 1},
		       {   1, T_PUMMEL, 1},
		       {   10, T_DASH, 1},
		       {   12, T_G_SLAM, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_DRAW_ENERGY, 1},
		       {   25, T_SPIRIT_BOMB, 1},
		       {   0, 0, 0} },

	she_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_SPELL_SPAM, 1},
		       {   0, 0, 0} },

	mzi_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_SPELL_SPAM, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	bre_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_SPELL_SPAM, 1},
		       {   0, 0, 0} },

	arg_tech[] = { {   1, T_HEAL_HANDS, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	coc_tech[] = { {   1, T_EGG_BOMB, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	hca_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_PREACHING, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_HARDCORE_ALIENIZATION, 1},
		       {   0, 0, 0} },
	sly_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_PREACHING, 1},
		       {   1, T_HARDCORE_ALIENIZATION, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	azt_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_PREACHING, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	dop_tech[] = { {   1, T_LIQUID_LEAP, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	dwa_tech[] = { {   1, T_RAGE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	cur_tech[] = { /* Put Tech here */
		       {   1, T_BLESSING, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	imp_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_PREACHING, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	swi_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_CREATE_AMMO, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_TINKER, 1},
		       {   0, 0, 0} },
	tay_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_KIII, 1},
		       {   1, T_BLITZ, 1},
		       {   12, T_G_SLAM, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_PUMMEL, 1},
		       {   0, 0, 0} },
	spd_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   12, T_PREACHING, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	elf_tech[] = { /* Put Tech here */
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	kop_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_ON_THE_SAME_TEAM, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	gno_tech[] = { {   1, T_VANISH, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   7, T_TINKER, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	anc_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
			 {   11, T_CONCENTRATING, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	dee_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
			 {   10, T_BLOOD_RITUAL, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	nea_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
			 {   1, T_TINKER, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	and_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
			 {   10, T_CREATE_AMMO, 1},
			 {   15, T_SKILLOMORPH, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
			 {   20, T_CHARGE_SABER, 1},
		       {   0, 0, 0} },
	sho_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   8, T_ON_THE_SAME_TEAM, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	bga_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   15, T_CONCENTRATING, 1},
		       {   20, T_TERRAIN_CLEANUP, 1},
		       {   0, 0, 0} },
	dvp_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_RESET_TECHNIQUE, 1},
		       {   1, T_SKILLOMORPH, 1},
			 {   1, T_TERRAIN_CLEANUP, 1},
			 {   5, T_RESEARCH, 1},
			 {   10, T_DIAMOND_BARRIER, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
			 {   20, T_TURN_UNDEAD, 1},
			 {   20, T_WORLD_FALL, 1},
		       {   0, 0, 0} },
	bur_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
			 {   10, T_WARD_FIRE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
			 {   20, T_SPIRIT_BOMB, 1},
		       {   0, 0, 0} },
	gre_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_DAZZLE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	sti_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_RECHARGE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	thu_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_WARD_ELEC, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	jav_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_DOUBLE_THROWNAGE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	wis_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_DOUBLE_THROWNAGE, 1},
		       {   14, T_VANISH, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   15, T_WONDERSPELL, 1},
		       {   22, T_POLYFORM, 1},
		       {   0, 0, 0} },
	roh_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_BLINK, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_IRON_SKIN, 1},
		       {   0, 0, 0} },
	sal_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_DAZZLE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	din_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_BERSERK, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   16, T_EDDY_WIND, 1},
		       {   0, 0, 0} },
	faw_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   5, T_REINFORCE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   15, T_ENT_S_POTION, 1},
		       {   16, T_SPIRITUALITY_CHECK, 1},
		       {   27, T_LIQUID_LEAP, 1},
		       {   0, 0, 0} },
	sat_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_REINFORCE, 1},
		       {   1, T_ON_THE_SAME_TEAM, 1},
		       {   13, T_HEAL_HANDS, 1},
		       {   15, T_VANISH, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_CALM_STEED, 1},
		       {   20, T_DRAW_ENERGY, 1},
		       {   21, T_LIQUID_LEAP, 1},
		       {   22, T_SUMMON_TEAM_ANT, 1},
		       {   24, T_TELEKINESIS, 1},
		       {   30, T_SUMMON_PET, 1},
		       {   0, 0, 0} },
	clk_tech[] = { {   1, T_FLURRY, 1},
		       {   1, T_VANISH, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_BOOZE, 1},
		       {   2, T_SPIRITUALITY_CHECK, 1},
		       {   4, T_DOUBLE_THROWNAGE, 1},
		       {   5, T_TINKER, 1},
		       {   6, T_SHIELD_BASH, 1},
		       {   7, T_SIGIL_DISCHARGE, 1},
		       {   8, T_IRON_SKIN, 1},
		       {   10, T_BLINK, 1},
		       {   11, T_POLYFORM, 1},
		       {   12, T_DRAW_ENERGY, 1},
		       {   13, T_CONCENTRATING, 1},
		       {   15, T_SIGIL_TEMPEST, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_RECHARGE, 1},
		       {   20, T_ENT_S_POTION, 1},
		       {   23, T_SIGIL_CONTROL, 1},
		       {   25, T_EDDY_WIND, 1},
		       {   28, T_DECONTAMINATE, 1},
		       {   0, 0, 0} },
	ogr_tech[] = { {   1, T_FLURRY, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_BERSERK, 1},
		       {   15, T_PRIMAL_ROAR, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_CRIT_STRIKE, 1},
		       {   0, 0, 0} },
	fay_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_BLINK, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   16, T_SPELL_SPAM, 1},
		       {   20, T_DRAW_ENERGY, 1},
		       {   0, 0, 0} },
	alc_tech[] = { {   1, T_RESEARCH, 1},
		       {   1, T_TINKER, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_SURGERY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_SUMMON_TEAM_ANT, 1},
		       {   25, T_DRAW_ENERGY, 1},
		       {   30, T_EGG_BOMB, 1},
		       {   0, 0, 0} },
	ung_tech[] = { {   1, T_WORLD_FALL, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
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
		       {   1, T_DOUBLE_THROWNAGE, 1},
		       {   1, T_EDDY_WIND, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   16, T_SPIRITUALITY_CHECK, 1},
		       {   24, T_POLYFORM, 1},
		       {   0, 0, 0} },
	mou_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_POLYFORM, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	oct_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_LIQUID_LEAP, 1},
		       {   1, T_DAZZLE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_ENT_S_POTION, 1},
		       {   0, 0, 0} },
	ink_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   5, T_ENT_S_POTION, 1},
		       {   10, T_RESEARCH, 1},
		       {   12, T_ON_THE_SAME_TEAM, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   15, T_SPIRITUALITY_CHECK, 1},
		       {   20, T_HEAL_HANDS, 1},
		       {   30, T_SUMMON_PET, 1},
		       {   0, 0, 0} },
	hob_tech[] = { {   1, T_BLINK, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_LUCKY_GAMBLE, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	fen_tech[] = { {   1, T_EVISCERATE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_DISARM, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_VANISH, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_SUMMON_PET, 1},
		       {   0, 0, 0} },

	ent_tech[] = { {   1, T_ENT_S_POTION, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	alb_tech[] = { {   1, T_DAZZLE, 1},
		       {   1, T_PRACTICE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_BLOOD_RITUAL, 1},
		       {   5, T_CHARGE_SABER, 1},
		       {   10, T_CRIT_STRIKE, 1},
		       {   15, T_TELEKINESIS, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_BLINK, 1},
		       {   25, T_JEDI_JUMP, 1},
		       {   0, 0, 0} },

	ret_tech[] = { {   1, T_REINFORCE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   5, T_RESEARCH, 1},
		       {   10, T_ZAP_EM, 1},
		       {   10, T_CONCENTRATING, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_POWER_SURGE, 1},
		       {   25, T_WONDERSPELL, 1},
		       {   30, T_TELEKINESIS, 1},
		       {   0, 0, 0} },

	nor_tech[] = { {   1, T_BERSERK, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   8, T_CONCENTRATING, 1},
		       {   10, T_RAGE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	lic_tech[] = { {   1, T_BLOOD_RITUAL, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	ins_tech[] = { {   1, T_SUMMON_TEAM_ANT, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   12, T_DOUBLE_THROWNAGE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	kla_tech[] = { {   1, T_SUMMON_TEAM_ANT, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   15, T_CRIT_STRIKE, 1},
		       {   20, T_TELEKINESIS, 1},
		       {   25, T_SPIRIT_BOMB, 1},
		       {   0, 0, 0} },
	kob_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   3, T_DOUBLE_THROWNAGE, 1},
			 {   10, T_TINKER, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	kha_tech[] = { {   1, T_EVISCERATE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	gel_tech[] = { {   1, T_LIQUID_LEAP, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	ton_tech[] = { {   1, T_DASH, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   6, T_SHIELD_BASH, 1},
		       {   10, T_IRON_SKIN, 1},
		       {   10, T_ON_THE_SAME_TEAM, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_POKE_BALL, 1},
		       {   0, 0, 0} },
	hmo_tech[] = { {   1, T_BERSERK, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	zru_tech[] = { {   1, T_BERSERK, 1},
		       {   1, T_PUMMEL, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_PRIMAL_ROAR, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_RAGE, 1},
		       {   0, 0, 0} },
	lyc_tech[] = { {   1, T_EVISCERATE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {  10, T_BERSERK, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	vam_tech[] = { {   1, T_DAZZLE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_DRAW_BLOOD, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	lev_tech[] = { {   1, T_DAZZLE, 1},
		       {   1, T_VANISH, 1},
		       {   1, T_TELEKINESIS, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   4, T_SPIRITUALITY_CHECK, 1},
		       {   5, T_ATTIRE_CHARM, 1},
		       {   8, T_ENT_S_POTION, 1},
		       {   10, T_BLINK, 1},
		       {   13, T_RECHARGE, 1},
		       {   15, T_BLESSING, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   18, T_POLYFORM, 1},
		       {   20, T_LIQUID_LEAP, 1},
		       {   22, T_LUCKY_GAMBLE, 1},
		       {   25, T_DRAW_ENERGY, 1},
		       {   30, T_SUMMON_TEAM_ANT, 1},
		       {   0, 0, 0} },
	ang_tech[] = { {   1, T_DAZZLE, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_TURN_UNDEAD, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	hrb_tech[] = { {   1, T_SURGERY, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {  20, T_REVIVE, 1},
		       {   0, 0, 0} },
	mum_tech[] = { {   1, T_RAISE_ZOMBIES, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	vor_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_VANISH, 1},
		       {   5, T_SPIRITUALITY_CHECK, 1},
		       {   10, T_TELEKINESIS, 1},
		       {   15, T_EGG_BOMB, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_WORLD_FALL, 1},
		       {   25, T_RECHARGE, 1},
		       {   0, 0, 0} },
	cor_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_VANISH, 1},
		       {   1, T_EGG_BOMB, 1},
		       {   5, T_SPIRITUALITY_CHECK, 1},
		       {   10, T_TELEKINESIS, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_WORLD_FALL, 1},
		       {   25, T_RECHARGE, 1},
		       {   0, 0, 0} },
	bor_tech[] = { {   3, T_JEDI_JUMP, 1},
		       {   10, T_CHARGE_SABER, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_TELEKINESIS, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   0, 0, 0,} },

	woo_tech[] = { {   12, T_JEDI_JUMP, 1},
		       {   20, T_CHARGE_SABER, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   0, 0, 0,} },

	bat_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_PRACTICE, 1},
		       {   1, T_ON_THE_SAME_TEAM, 1},
		       {   1, T_DRAW_BLOOD, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_POLYFORM, 1},
		       {   0, 0, 0} },
	cen_tech[] = { {   1, T_INVOKE_DEITY, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   7, T_SHIELD_BASH, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },
	rod_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   5, T_CARD_TRICK, 1},
		       {   10, T_DOUBLE_TROUBLE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_WONDERSPELL, 1},
		       {   25, T_SPELL_SPAM, 1},
		       {   30, T_RESET_TECHNIQUE, 1},
		       {   0, 0, 0} },

	man_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_SKILLOMORPH, 1},
		       {   0, 0, 0} },

	jel_tech[] = { {   1, T_DRAW_ENERGY, 1},
		       {   1, T_LIQUID_LEAP, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_SPIRITUALITY_CHECK, 1},
		       {   12, T_RECHARGE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   22, T_POLYFORM, 1},
		       {   0, 0, 0} },

	dry_tech[] = { {   1, T_IRON_SKIN, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	tur_tech[] = { {   1, T_PUMMEL, 1},
			 {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_DASH, 1},
		       {   1, T_BLITZ, 1},
		       {   2, T_CHI_STRIKE, 1},
	  	       {   4, T_CHI_HEALING, 1},
		       {   5, T_DOUBLE_THROWNAGE, 1},
	  	       {   6, T_E_FIST, 1},
		       {   8, T_DRAW_ENERGY, 1},
		       {  10, T_G_SLAM, 1},
		       {  11, T_WARD_FIRE, 1},
		       {  13, T_WARD_COLD, 1},
		       {  15, T_WARD_ELEC, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {  17, T_SPIRIT_BOMB, 1},
		       {  20, T_POWER_SURGE, 1},
		       {  25, T_EDDY_WIND, 1},
		       {   0, 0, 0} },

	gri_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_RECHARGE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	tme_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_CREATE_AMMO, 1},
		       {   12, T_HARDCORE_ALIENIZATION, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	inc_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_RECHARGE, 1},
		       {   0, 0, 0} },

	mim_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_POLYFORM, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	mis_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_RECHARGE, 1},
			 {   12, T_TERRAIN_CLEANUP, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	nav_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   18, T_CONCENTRATING, 1},
		       {   0, 0, 0} },

	spi_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   15, T_EDDY_WIND, 1},
		       {   0, 0, 0} },

	ven_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_LUCKY_GAMBLE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	spr_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   6, T_IRON_SKIN, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	trf_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   5, T_POLYFORM, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	out_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
			 {   6, T_EVISCERATE, 1},
			 {   10, T_ZAP_EM, 1},
			 {   12, T_CRIT_STRIKE, 1},
			 {   15, T_DASH, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
			 {   18, T_TELEKINESIS, 1},
			 {   20, T_RESEARCH, 1},
		       {   20, T_REINFORCE, 1},
		       {   30, T_SKILLOMORPH, 1},
		       {   0, 0, 0} },

	trp_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   11, T_RECHARGE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   28, T_SPIRITUALITY_CHECK, 1},
		       {   0, 0, 0} },

	ira_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   4, T_HARDCORE_ALIENIZATION, 1},
		       {   5, T_ROCK_TO_POISON, 1},
		       {   12, T_CREATE_AMMO, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	una_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   3, T_SHIELD_BASH, 1},
		       {   6, T_IRON_SKIN, 1},
		       {   8, T_DOUBLE_THROWNAGE, 1},
		       {   10, T_CONCENTRATING, 1},
		       {   12, T_SPIRITUALITY_CHECK, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   18, T_POLYFORM, 1},
		       {   21, T_EDDY_WIND, 1},
		       {   24, T_RECHARGE, 1},
		       {   27, T_SUMMON_PET, 1},
		       {   0, 0, 0} },

	uni_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_LUCKY_GAMBLE, 1},
		       {   10, T_SUMMON_PET, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	unm_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_RECHARGE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	vee_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_SPIRITUALITY_CHECK, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   17, T_SPELL_SPAM, 1},
		       {   0, 0, 0} },

	wrp_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_POLYFORM, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	egy_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_ZAP_EM, 1},
		       {   1, T_PREACHING, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   25, T_HARDCORE_ALIENIZATION, 1},
		       {   0, 0, 0} },

	nem_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_WORLD_FALL, 1},
		       {   0, 0, 0} },

	cup_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_ATTIRE_CHARM, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	urg_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_BERSERK, 1},
		       {   10, T_PRIMAL_ROAR, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	thr_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   22, T_VANISH, 1},
		       {   25, T_BOOZE, 1},
		       {   0, 0, 0} },

	zau_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   12, T_EVISCERATE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   16, T_IRON_SKIN, 1},
		       {   20, T_CRIT_STRIKE, 1},
		       {   0, 0, 0} },

	wyl_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   6, T_SPELL_SPAM, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   25, T_REINFORCE, 1},
		       {   0, 0, 0} },

	bac_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   15, T_WORLD_FALL, 1},
		       {   18, T_PERMAMORPH, 1},
		       {   20, T_POKE_BALL, 1},
		       {   25, T_DECONTAMINATE, 1},
		       {   0, 0, 0} },

	cer_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   5, T_PRIMAL_ROAR, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	kst_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   6, T_FLURRY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   16, T_TERRAIN_CLEANUP, 1},
		       {   22, T_DIAMOND_BARRIER, 1},
		       {   0, 0, 0} },

	vie_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_CREATE_AMMO, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_SPIRIT_BOMB, 1},
		       {   0, 0, 0} },

	mog_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_BERSERK, 1},
		       {   5, T_DOUBLE_THROWNAGE, 1},
		       {   10, T_FLURRY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_EDDY_WIND, 1},
		       {   25, T_BLOOD_RITUAL, 1},
		       {   0, 0, 0} },

	per_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_CALM_STEED, 1},
		       {   1, T_HARDCORE_ALIENIZATION, 1},
		       {   5, T_WARD_FIRE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_SPIRITUALITY_CHECK, 1},
		       {   25, T_PREACHING, 1},
		       {   0, 0, 0} },

	bov_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   14, T_SUMMON_PET, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   28, T_BLOOD_RITUAL, 1},
		       {   0, 0, 0} },

	han_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   8, T_E_FIST, 1},
		       {   10, T_EVISCERATE, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   15, T_TELEKINESIS, 1},
		       {   20, T_HEAL_HANDS, 1},
		       {   25, T_CALM_STEED, 1},
		       {   0, 0, 0} },

	mac_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   12, T_CALM_STEED, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	crt_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_CALM_STEED, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_TERRAIN_CLEANUP, 1},
		       {   24, T_HARDCORE_ALIENIZATION, 1},
		       {   0, 0, 0} },

	rus_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_SKILLOMORPH, 1},
		       {   10, T_CONCENTRATING, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   18, T_BOOZE, 1},
		       {   0, 0, 0} },

	pic_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_TERRAIN_CLEANUP, 1},
		       {   0, 0, 0} },

	bab_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   1, T_IRON_SKIN, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	ger_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   10, T_ROCK_TO_POISON, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   0, 0, 0} },

	poi_tech[] = { {   1, T_APPRAISAL, 1},
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
		       {   20, T_ROCK_TO_POISON, 1},
		       {   0, 0, 0} },

	def_tech[] = { {   1, T_APPRAISAL, 1}, /* everyone is supposed to get this --Amy */
		       {   1, T_PANIC_DIGGING, 1},
		       {   1, T_PHASE_DOOR, 1},
		       {   1, T_INVOKE_DEITY, 1},
		       {   15, T_SECURE_IDENTIFY, 1},
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

STATIC_PTR void
undo_lockfloodP(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ < STONE || levl[x][y].typ > ROCKWALL)
		return;
	if (((levl[x][y].wall_info & W_NONDIGGABLE) != 0) && levl[x][y].typ != ROCKWALL)
		return;

	if (*in_rooms(x,y,SHOPBASE)) return;

	(*(int *)roomcnt)++;

	/* Get rid of stone at x, y */
	levl[x][y].typ = CORR;
	blockorunblock_point(x,y);
	if (!(levl[x][y].wall_info & W_HARDGROWTH)) levl[x][y].wall_info |= W_EASYGROWTH;
	newsym(x,y);
}

STATIC_PTR void
terraincleanup(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (Is_waterlevel(&u.uz)) return;

	if (levl[x][y].typ < GRAVEWALL)
		return;
	if (levl[x][y].typ >= SDOOR && levl[x][y].typ <= SCORR)
		return;
	if ((levl[x][y].wall_info & W_NONDIGGABLE) != 0)
		return;
	if (levl[x][y].typ == DRAWBRIDGE_UP || levl[x][y].typ == DRAWBRIDGE_DOWN)
		return;
	if (levl[x][y].typ >= DOOR && levl[x][y].typ <= STRAWMATTRESS)
		return;

	(*(int *)roomcnt)++;

	/* Get rid of stone at x, y */
	levl[x][y].typ = CORR;
	blockorunblock_point(x,y);
	if (!(levl[x][y].wall_info & W_HARDGROWTH)) levl[x][y].wall_info |= W_EASYGROWTH;
	newsym(x,y);
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
			u.uhpmax = 1;
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

void
learntech_or_leveltech(tech, mask, tlevel)
	short tech;
	long mask;
	int tlevel;
{
	int i;
	i = get_tech_no(tech);
	if (tech_list[i].t_intrinsic & mask) {
		techlevelspecific(tech_list[i].t_id);
	} else {
		learntech(tech, mask, tlevel);
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

	if (istechless && tlevel >= 0) return; /* tough luck... --Amy */

	i = get_tech_no(tech);
	/* Amy note: some techs are given only if you're the correct alignment when leveling up
	 * diablist for alignment-specific techs goes here */
	if (tlevel > 0) {
		if (tech == T_PREACHING && mask != FROMOUTSIDE && u.ualign.type != A_LAWFUL) return;
		if (tech == T_ON_THE_SAME_TEAM && mask != FROMOUTSIDE && u.ualign.type != A_NEUTRAL) return;
		if (tech == T_PERMAMORPH && mask != FROMOUTSIDE && u.ualign.type != A_CHAOTIC) return;
	}

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
		pline("Tech already known."); /* can happen with recursion --Amy */
		return;
	    }
	    if (mask == FROMOUTSIDE) {
		tech_list[i].t_intrinsic &= ~OUTSIDE_LEVEL;
		tech_list[i].t_intrinsic |= tlevel & OUTSIDE_LEVEL;
	    }
	    if (tlevel < tech_list[i].t_lev)
		tech_list[i].t_lev = tlevel;
	    tech_list[i].t_intrinsic |= mask;
	    /*tech_list[i].t_tout = 0;*/ /* Can use immediately*/
	    /* should be initialized as 0 if you newly learn it, but be saved as the earlier timeout if you level-drained
	     * yourself and thereby lost knowledge of the tech. This is to prevent drain-for-gain exploits at XL15 - players
	     * would certainly find out that you can use secure identify indefinitely that way... --Amy */
	}
	else if (tlevel < 0) {

	    if (i < 0 || !(tech_list[i].t_intrinsic & mask)) {
		pline("Tech not known."); /* can happen with recursion --Amy */
		return;
	    }
	    if (techlev(i) < 1) {
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

	/* display the menu anyway, because of vibrating square stuff --Amy */
/*	if (ntechs == 0)  {
            You("don't know any techniques right now.");
	    return FALSE;
	}*/
	if (flags.menu_style == MENU_TRADITIONAL) {
            if (ntechs == 1)  strcpy(lets, "a");
            else if (ntechs < 27)  sprintf(lets, "a-%c", 'a' + ntechs - 1);
            else if (ntechs == 27)  sprintf(lets, "a-z A");
            else sprintf(lets, "a-z A-%c", 'A' + ntechs - 27);

	    for(;;)  {
                sprintf(qbuf, "Perform which technique? [%s ?]", lets);
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
        return dotechmenu(PICK_ONE, tech_no, 0);
}

static boolean
dotechmenu(how, tech_no, specialmenutype)
	int how;
	int *tech_no;
	int specialmenutype; /* 0 = use techs, 1 = sort techs, 2 = pick tech to swap with */
{
	winid tmpwin;
	int i, n, len, longest, techs_useable, tlevel;
	int splnum, othnum;
	char buf[BUFSZ], let = 'a';
	const char *prefix;
	menu_item *selected;
	anything any;

restartmenu:
	let = 'a';

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;         /* zero out all bits */

	tech_in_memory = -2;
	struct tech spl_tmp;

	techs_useable = 0;

	if (!iflags.menu_tab_sep) {
	    /* find the length of the longest tech */
	    for (longest = 0, i = 0; i < MAXTECH; i++) {
		if (techid(i) == NO_TECH) continue;
		if ((len = strlen(techname(i))) > longest)
		    longest = len;
	    }
	    sprintf(buf, "    %-*s Level   Status", longest, "Name");
	} else
	    sprintf(buf, "Name\tLevel\tStatus");

	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, MENU_UNSELECTED);

	for (i = 0; i < MAXTECH; i++) {
	    if (techid(i) == NO_TECH)
		continue;
	    tlevel = techlev(i);
	    if (TechLossEffect || u.uprops[TECH_LOSS_EFFECT].extrinsic || have_techlossstone()) {
		techs_useable++;
		prefix = "";
		any.a_int = i + 1;
	    } else if (tlevel > 0 && (wizard || !techtout(i)) ) {
		/* Ready to use */
		techs_useable++;
		prefix = "";
		any.a_int = i + 1;
	    } else {
		prefix = "    ";
		any.a_int = 0;
	    }
#ifdef WIZARD
	    if (wizard || (RngeTechInsight && !(TechLossEffect || u.uprops[TECH_LOSS_EFFECT].extrinsic || have_techlossstone()) ) )
		if (!iflags.menu_tab_sep)
		    sprintf(buf, "%s%-*s %2d%c%c%c%c  %s(%i)",
			    prefix, longest, techname(i), tlevel, ((techtout(i) || (tlevel <= 0)) ? '*' : ' '),
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
		    sprintf(buf, "%s%s\t%2d%c%c%c%c\t%s(%i)",
			    prefix, techname(i), tlevel, ((techtout(i) || (tlevel <= 0)) ? '*' : ' '),
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
	    if (TechLossEffect || u.uprops[TECH_LOSS_EFFECT].extrinsic || have_techlossstone()) {
		sprintf(buf, "      ");
	    } else if (!iflags.menu_tab_sep) {
		sprintf(buf, "%s%-*s %5d%c  %s",
			prefix, longest, techname(i), tlevel, ((techtout(i) || (tlevel <= 0)) ? '*' : ' '),
			tech_inuse(techid(i)) ? "Active" :
			tlevel <= 0 ? "Beyond recall" :
			can_limitbreak() ? "LIMIT" :
			!techtout(i) ? "Prepared" : 
			techtout(i) > 10000 ? "Huge timeout" :
			techtout(i) > 1000 ? "Not Ready" :
			techtout(i) > 100 ? "Reloading" : "Soon");
	    } else
		sprintf(buf, "%s%s\t%5d%c\t%s",
			prefix, techname(i), tlevel, ((techtout(i) || (tlevel <= 0)) ? '*' : ' '),
			tech_inuse(techid(i)) ? "Active" :
			tlevel <= 0 ? "Beyond recall" :
			can_limitbreak() ? "LIMIT" :
			!techtout(i) ? "Prepared" : 
			techtout(i) > 10000 ? "Huge timeout" :
			techtout(i) > 1000 ? "Not Ready" :
			techtout(i) > 100 ? "Reloading" : "Soon");

	    add_menu(tmpwin, NO_GLYPH, &any,
		    (tlevel <= 0) ? 0 : (techtout(i) && !(TechLossEffect || u.uprops[TECH_LOSS_EFFECT].extrinsic || have_techlossstone()) && !wizard) ? 0 : let, 0, ATR_NONE, buf, MENU_UNSELECTED);
	    if (let++ == 'z') let = 'A';
	    if (let == 'Z') let = 'a';
	}

	if (specialmenutype == 0) { /* option for sorting your techs */
		any.a_int = -1;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any, '?', 0, ATR_NONE, "Sort techs", MENU_UNSELECTED);
	}

	if (!techs_useable)
	    how = PICK_NONE;

	/* Amy addition: if you are on the vibrating square, print a special message. The reason for it being that
	 * #technique is one of the commands that ALWAYS works, no matter which nasty traps you have. Yes, there is a trap
	 * that prevents the techniques from actually working, but the menu can be accessed anyway, and therefore it's
	 * always possible in theory (he he) to find the VS no matter which, or how many(!!!), interface screws are active */
	end_menu(tmpwin, (isok(u.ux, u.uy) && invocation_pos(u.ux, u.uy)) ? "You're standing on the vibrating square." : (specialmenutype == 1) ? "Pick tech to sort" : (specialmenutype == 2) ? "Swap with which tech?" : how == PICK_ONE ? "Choose a technique" :
					   "Currently known techniques");

	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	if (n > 0) {

	    if (selected[0].item.a_int == -1) {
		free((void *)selected);
		return dotechmenu(PICK_ONE, tech_no, 1);
	    }

	    int selection = selected[0].item.a_int - 1;
	    if (selection < 0) { /* shouldn't happen, but according to amateurhour it can... so he fixed it --Amy */
		    free((void *)selected);
		    return FALSE;
	    }

	    /* sort techniques, by Amy, with code shamelessly copied/adjusted from spell.c */
	    if (specialmenutype == 1) {
		splnum = selected[0].item.a_int - 1;
		dotechmenu(PICK_ONE, tech_no, 2);
		othnum = tech_in_memory;

		/*pline("nums to swap: %d, %d", splnum, othnum);*/

		if (techid(splnum) <= NO_TECH) {
			pline("Invalid first tech. Aborting.");
			free((void *)selected);
			return FALSE;
		}
		if (techid(othnum) <= NO_TECH) {
			pline("Invalid second tech. Aborting.");
			free((void *)selected);
			return FALSE;
		}

		spl_tmp = tech_list[splnum];
		tech_list[splnum] = tech_list[othnum];
		tech_list[othnum] = spl_tmp;

		free((void *)selected);

		goto restartmenu;

		return FALSE;
	    }

	    if (specialmenutype == 2) {

		tech_in_memory = selected[0].item.a_int - 1;

		free((void *)selected);

		return FALSE;
	    }

	    *tech_no = selection;
	    free((void *)selected);
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

	sprintf(buf, "    %-17s Level  Status", "Name");
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
		    sprintf(buf, "%s%-20s %2d%c%c%c   %s(%i)",
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
		    sprintf(buf, "%s%s\t%2d%c%c%c\t%s(%i)",
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

int
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
dotechwiz()
{
	int i;

	for (i = 0; i < MAXTECH; i++) {
	    if (techid(i) == NO_TECH)
		continue;
	    if (techtout(i)) techtout(i) = 0;
	}
	pline("The timeout on all your techniques has been set to zero.");

	return dotech();

}

void
datadeletetechs()
{
	int i;

	for (i = 0; i < MAXTECH; i++) {
	    if (techid(i) == NO_TECH)
		continue;
	    if (!rn2(5)) techtout(i) += 1000000;
	}

}

void
resettechs()
{
	int i;

	for (i = 0; i < MAXTECH; i++) {
	    if (techid(i) == NO_TECH)
		continue;
	    techtout(i) += rnz(100000);
	}

}

int
dotech()
{
	int tech_no;

	if (flags.tech_description && !(TechLossEffect || u.uprops[TECH_LOSS_EFFECT].extrinsic || have_techlossstone()) ) {

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
			pline("Using this technique allows you to reinforce your memory of an almost-forgotten spell, or increase the memory of a moderately memorized spell up to 110%%.");
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

		case T_PANIC_DIGGING:
			pline("In situations where there is no other way forward, you can use this technique to transform walls on the eight squares surrounding you into open floor. Of course it only works if those squares are diggable, and it also paralyzes you for up to 20 turns, so be careful about using it when monsters are around!");
			break;

		case T_SECURE_IDENTIFY:
			pline("Everyone learns this technique at experience level 15, and it allows the player to identify an item in open inventory without fail.");
			break;

		case T_PHASE_DOOR:
			pline("If you need to get out of trouble, you can use this tech for a short-range teleport. It will teleport you over an euclidean distance of at least 3 but no more than 100. Of course it doesn't work if teleportation is prohibited on the current dungeon level.");
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
			pline("You need a blade to use this technique. Doing so allows you to severely hurt an adjacent non-headless monster; high technique levels sometimes even allow you to instakill the monster, but only if the monster's level isn't too high relative to the technique's level.");
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
			pline("You must hold an item in your hand that can be upgraded if you want to use this technique effectively. Also, it takes a very long time to upgrade an object; the duration is decreased slightly if the technique's level is lower, but monsters are still likely to interrupt you. And if that happens, the technique will be on timeout anyway, so better lock yourself in a closet first.");
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
			pline("Tries to disarm a monster (read: knock their weapon away); you need to be holding a weapon you're at least skilled with, though. Also the weapon needs to be at least +1 and if it's successful, the weapon loses a point of enchantment.");
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
			pline("This technique allows you to do a 'kamehameha' attack against a monster, causing it to explode while also damaging other monsters nearby. According to Nethackwiki, you can't hit yourself or your pets with this attack either (and the page is even still there, because it's a SLASH'EM technique; certain evil people wiped all SLEX-specific info). But in-game experience says otherwise, i.e. pets can very well still be hit, sadly.");
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
			pline("A very powerful technique that will instakill all non-unique monsters on the current dungeon level whose monster level is lower than half the technique's level. Sometimes it will also kill higher-level monsters.");
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

		case T_IRON_SKIN:
			pline("Using this technique will improve your armor class for a short period of time.");
			break;

		case T_POLYFORM:
			pline("You can use this technique for an uncontrolled random polymorph. It can turn you into monsters that are normally not a valid polymorph form, but you cannot make those polymorphs permanent via an amulet of unchanging because that would be too easy.");
			break;

		case T_CONCENTRATING:
			pline("With this technique active, your melee attacks will almost never miss; however, it only lasts for a short time.");
			break;

		case T_SUMMON_PET:
			pline("A technique that summons a tame monster.");
			break;

		case T_DOUBLE_THROWNAGE:
			pline("Use this technique just before you start throwing darts, daggers or similar throwing weapons, and you can throw many more projectiles per turn!");
			break;

		case T_SHIELD_BASH:
			pline("If you use this technique, your one-handed melee attacks will deal more damage as long as you have a shield equipped. The damage bonus depends on the shield's enchantment value.");
			break;

		case T_RECHARGE:
			pline("A very useful technique that allows you to recharge wands, tools and similar items. However, blessed scrolls of charging are still better, so you should probably not use this technique on a wand of wishing.");
			break;

		case T_SPIRITUALITY_CHECK:
			pline("If you're unsure whether you can safely pray, you can use this technique to communicate with your god. There are no negative effects - it simply tells you whether it's safe to pray now.");
			break;

		case T_EDDY_WIND:
			pline("You can invoke this technique to walk right through monsters for a short time. Remember: if you want to attack them in melee, you'll need to prefix your movements with the F command. If you are wielding two weapons at the same time and attack a monster while having the technique active, you will deal extra damage with every hit.");
			break;

		case T_BLOOD_RITUAL:
			pline("Requires an athame in your open inventory. If you use this technique, you sacrifice up to 200 max HP and Pw, and all of your stats are reduced by up to 2, but you will be granted a wish, an acquirement, a random artifact or a random powerful item.");
			break;

		case T_ENT_S_POTION:
			pline("Restores some of your health and also fixes blindness, stunning, confusion, hallucination, numbness, fear, freezing, dimness and burns.");
			break;

		case T_LUCKY_GAMBLE:
			pline("Randomly increases or decreases your base luck by one, with equal probability.");
			break;

		case T_DECONTAMINATE:
			pline("Greatly reduces your contamination when used.");
			break;

		case T_SWAP_WEAPON:
			pline("Allows you to set a different secondary weapon, but only if you're not currently dual-wielding. If you're a master at dual-wielding, this even lets you skip potential bad effects that wielding the weapon would give you.");
			break;

		case T_WONDERSPELL:
			pline("Teaches a random spell when used.");
			break;

		case T_RESET_TECHNIQUE:
			pline("Allows you to select a technique that's currently on timeout, which will become usable again.");
			break;

		case T_SILENT_OCEAN:
			pline("Using this technique will try to cancel all ; and I monsters in a rather large area around you. Also, while the technique is active, you have swimming and magical breathing, plus your items are protected from water damage and drowning attacks cannot harm you. If the technique is about to run out, you get a message so you can get out of the water. However, you also cannot cast spells or chat while it's active.");
			break;

		case T_GLOWHORN:
			pline("If you use this technique, you will get the effect of applying a noncursed unicorn horn every turn for a period of time, even if you don't actually have a unicorn horn.");
			break;

		case T_INTRINSIC_ROULETTE:
			pline("This technique can give or remove a random intrinsic, which can be good or bad.");
			break;

		case T_SPECTRAL_SWORD:
			pline("Adds elemental damage to your melee weapon for a period of time.");
			break;

		case T_REVERSE_IDENTIFY:
			pline("Allows you to identify an item based on random appearance, or input the name of an unknown item which will then be identified. It can also randomly identify some other unknown items.");
			break;

		case T_DETECT_TRAPS:
			pline("Reveals some of the traps on the current dungeon level when used.");
			break;

		case T_DIRECTIVE:
			pline("Very useful for petmasters, this technique lets you give orders to your pets. The higher your petkeeping skill, the more things you can change about your pets' behavior.");
			break;

		case T_REMOVE_IMPLANT:
			pline("Allows you to take off a worn implant, even if it's a cursed one.");
			break;

		case T_REROLL_IMPLANT:
			pline("This technique targets a worn implant and will reroll its base stats.");
			break;

		case T_MILDEN_CURSE:
			pline("Use this technique to turn a sticky/heavily/prime cursed item into a regularly cursed one. If you use it on an item that has the black breath, topi ylinen or ancient morgothian curses, there is only a 10%% chance of turning them into a regularly cursed one.");
			break;

		case T_FORCE_FIELD:
			pline("Activates a temporary force field that makes monsters with ranged weapons or beam wands much less likely to hit you.");
			break;

		case T_POINTINESS:
			pline("This technique requires you to wield a polearm or lance. If it's a negatively enchanted one, it gains a random amount of enchantment (not past +0), otherwise it can gain an additional point of enchantment, but the chance is lower the more enchantment your weapon has. Maximum that can be reached is +25.");
			break;

		case T_BUG_SPRAY:
			pline("Allows you to place a stinking cloud that deals poison damage to enemies, and also to you should you happen to walk into it.");
			break;

		case T_WHIRLSTAFF:
			pline("After using this technique, for a certain period of time your quarterstaff can attack several times in a single turn. The higher your quarterstaff skill, the more extra attacks you get.");
			break;

		case T_DELIBERATE_CURSE:
			pline("This technique replicates the effect of a potion of unholy water, allowing you to curse an item. Take note that even blessed items go straight to cursed when using this technique, unlike unholy water which would only unbless them.");
			break;

		case T_ACQUIRE_STEED:
			pline("Tries to tame an adjacent monster of a 'rideable' class. Of course this is SLEX, you can theoretically ride any monster, but this technique specifically tames the following glyphs: q, u, A, C, D, J, z. Only one monster can be tamed per use, however there's a chance for monsters to resist so if you surround yourself with several potential pets, there's a better chance that not all of them will resist.");
			break;

		case T_SADDLING:
			pline("Creates a saddle out of thin air.");
			break;

		case T_SHOPPING_QUEEN:
			pline("Every woman's wet dream, this technique lets you shop for shoes! Depending on your high heels skill, there's a certain amount of shoes that you can pick from. You can only pick one pair from the list, which will be placed on the floor. So choose wisely and become the prettiest woman in the dungeon by donning your favorite high heels! (Disclaimer: You don't need to pay money; the technique is basically the voucher that you redeem to get your shoes.)");
			break;

		case T_BEAUTY_CHARM:
			pline("This technique requires you to be wearing high heels, and paralyzes you for 10 turns, so be careful when using it. During those turns, it repeatedly tries to pacify humanoids and animals in a 7x7 radius around you, hoping that they fall in love with your beautiful high heels and decide to stop fighting you. However, monsters may resist the attempt and attack you anyway.");
			break;

		case T_ASIAN_KICK:
			pline("If you want to use this technique, you must be wearing stiletto heels. Then it lasts for 50 turns, and once you kick a male humanoid or animal, you will place a devastating kick between their legs. The enemy's nuts will be pushed out of their original position by your very tender stiletto heels, causing unbearable pain to the monster and knocking it out for possibly hundreds of turns, but this technique immediately ends once you successfully kick something with it. Take note that using this technique puts a timeout on all of the following techniques: asian kick, legscratch attack, ground stomp, athletic combat.");
			break;

		case T_LEGSCRATCH_ATTACK:
			pline("Requires you to wear cone heels. If you use this technique, your kick attack will be able to scratch up and down the enemy's legs for a while, ripping their shins to shreds, and as a result they'll move more slowly. Do it often enough and their legs will be bleeding so badly that all your kicks deal double damage because your beautiful cone heels are so incredibly sharp-edged! Take note that using this technique puts a timeout on all of the following techniques: asian kick, legscratch attack, ground stomp, athletic combat.");
			break;

		case T_GROUND_STOMP:
			pline("In order to use this technique, you must be wearing wedge heels. It will last for a while, and if you kick a paralyzed enemy while the technique is active, you'll not only deal double damage but also increase the time for which the enemy is paralyzed, so you can effectively chain-paralyze them. Gotta kick 'em while they're down, and stomp them with your very lovely, massive wedge heels! Take note that using this technique puts a timeout on all of the following techniques: asian kick, legscratch attack, ground stomp, athletic combat.");
			break;

		case T_ATHLETIC_COMBAT:
			pline("Only characters in block heels can use this technique. It lasts for a generous amount of time, and while it's active, you can place a high kick with your block heels at the enemy's belly, shoulder or even their head. The elegance of your very feminine block heels will mesmerize them so much that they may spontaneously stop fighting back, allowing you to kick them again and again with your fleecy block heel. Take note that using this technique puts a timeout on all of the following techniques: asian kick, legscratch attack, ground stomp, athletic combat.");
			break;

		case T_PRAYING_SUCCESS:
			pline("This very helpful technique allows you to pray when it would normally not be safe to pray. Please be aware of the fact that your deity is still inaccessible in Gehennom; the gods won't get angry if you use this technique in Gehennom, but they won't be able to help you anyway. Also, there's limits as to what it can do; if your alignment is extremely low or you have a huge prayer timeout, it might still fail, and it won't necessarily work when your god is extremely angry.");
			break;

		case T_OVER_RAY:
			pline("After using this technique, your ray and beam wands and spells have a much greater range than usually. It lasts for a generous amount of time. Diagonally fired rays in particular will sometimes bounce for quite a long time!");
			break;

		case T_ENCHANTERANG:
			pline("A technique that can only be used if you're wielding a boomerang, and which will try to enchant it so you'll be more capable of killing the little poison ivies. You can also wield an entire stack, and as long as it's 10 or less boomerangs in it, the entire stack will be enchanted. Trying to enchant greater stacks can fail, more often the bigger the stack is. Also, boomerangs that are already +25 or higher can not be further enchanted with this technique.");
			break;

		case T_BATMAN_ARSENAL:
			pline("If you want to be able to fight Scarecrow, Clayface and the Penguin, you need a lot of batarangs. Using this technique will create one out of thin air!");
			break;

		case T_JOKERBANE:
			pline("Yes, Batman is capable of defeating the Joker, even though he's the biggest villain in all of Gotham City and has three separate ways of instakilling Batman and is also very capable of killing Batman via normal damage. Use this technique and your batarang will be able to paralyze monsters for a while if you throw it!");
			break;

		case T_CALL_THE_POLICE:
			pline("If you need help, use this technique and tame kops will appear to fight your enemies.");
			break;

		case T_DOMINATE:
			pline("Tries to tame an adjacent animal. At low technique levels you'll usually only dominate a single one, while with higher levels it may occasionally tame several animals in one use.");
			break;

		case T_INCARNATION:
			pline("Allows you to select an adjacent monster and try to polymorph into it. But beware, there's a one in three chance that you polymorph into something completely random instead.");
			break;

		case T_COMBO_STRIKE:
			pline("This technique can be used with chained blitz if you have that, and if you do so, every hit deals 10 extra damage compared to the previous one. Using it 'normally' gives a different effect: for a period of time, every melee hit you land will deal one more point of damage compared to the previous one, but it instantly terminates if you try to use chained blitz while having combo strike active, and it also terminates if you spend a turn doing something other than hitting a monster in melee, so you have to keep attacking things to maximize the effect.");
			break;

		case T_FUNGOISM:
			pline("With this technique, you will polymorph into a random F-class monster. Beware, polymorph control doesn't let you choose what you become. Since most fungi aren't capable of wearing armor or wielding weapons, this is a way to shake off cursed ones. However, most fungi also lack limbs and generally make poor fighting forms.");
			break;

		case T_BECOME_UNDEAD:
			pline("Unlike the turn undead technique, this one actually allows you to become an undead creature! This is done by polymorphing you, but the one you turn into is random, even if you have polymorph control.");
			break;

		case T_JIU_JITSU:
			pline("A bare-handed combat technique that lasts for a very long time. While it's active, your fists will deal more damage the better a monster's armor class is, but it doesn't increase your to-hit to match, so you'd better be good enough to actually hit the monsters :D");
			break;

		case T_BLADE_ANGER:
			pline("While this technique is active, throwing shuriken will additionally shoot invisible beams at the enemy that may hit several of them at once, but these beams require 10 mana each.");
			break;

		case T_RE_TAMING:
			pline("Use this technique while standing next to a monster that used to be your pet, and if the monster was peaceful it'll become tame again. Using it on a former pet that actually became hostile only has a 1 in 3 chance of working, and using it on a former pet that became frenzied will never work.");
			break;

		case T_TIME_STOP:
			pline("Using this technique stops time for 6-11 turns.");
			break;

		case T_STAT_RESIST:
			pline("Provides temporary resistance to sickness, stun, confusion, blindness, hallucination and fear.");
			break;

		case T_CHARGE_SABER:
			pline("This technique will use up all of your current mana and convert it into lightsaber energy. If the techlevel is at least 10, you can also win the jackpot. However, monsters may interrupt you, preventing your lightsaber from actually being charged.");
			break;

		case T_UNCURSE_SABER:
			pline("Use this technique while wielding a cursed lightsaber and it will be uncursed! If it's heavily cursed or worse, it gets the usual chance of resisting, of course.");
			break;

		case T_ENERGY_CONSERVATION:
			pline("It lasts for a very long time, and reduces the rate at which your lightsaber will use up power. The higher your Makashi skill, the better the reduction.");
			break;

		case T_ENCHANT_ROBE:
			pline("This technique requires you to wear a robe, which will receive a random enchantment. If your robe already has an enchantment, it will lose the one it has and get a new random one. And if your Soresu skill is at Grand Master or better, the technique can also occasionally increase the enchantment of your robe up to a maximum of +7; this will never vaporize the robe because SLEX adheres to the 'enhancing your skills should never have detrimental side effects' principle.");
			break;

		case T_WILD_SLASHING:
			pline("While this technique is active, your dual-wielded lightsabers will attack more quickly but have reduced to-hit. Beware: you can't just cheat by running a to-hit-increasing technique at the same time, those won't give you any to-hit bonuses while wild slashing is active.");
			break;

		case T_ABSORBER_SHIELD:
			pline("A technique that will last for a while, and as long as it's active, blocking a projectile with your lightsaber will add some saber power.");
			break;

		case T_PSYCHO_FORCE:
			pline("Allows you to select a monster that you can see, which will take damage and be paralyzed for a while.");
			break;

		case T_INTENSIVE_TRAINING:
			pline("Every time you use this technique, a random stat will be increased by 1.");
			break;

		case T_SURRENDER_OR_DIE:
			pline("This technique lasts for a few hundred turns and allows your lightsaber attacks to occasionally pacify the opponent, or sometimes your lightsaber will gain enchantment, or the lightsaber gains additional power. But it only triggers if you manage to smash apart an enemy's held weapon.");
			break;

		case T_PERILOUS_WHIRL:
			pline("If you use a lit double lightsaber while having this technique active, your attacks may occasionally drain the enemy's levels.");
			break;

		case T_SUMMON_SHOE:
			pline("Well, guess :D It summons a tame shoe that will kick monsters!");
			break;

		case T_KICK_IN_THE_NUTS:
			pline("Requires you to wear sexy flats, and can only be used on a male monster, which will deal a lot of damage to it and also prevent the target from fighting back for a while.");
			break;

		case T_DISARMING_KICK:
			pline("This technique can only be used if you wear sexy flats, and will disarm an adjacent monster. If successful, its weapon will land at your feet. Unlike the disarm technique, this works even if the target is holding a cursed weapon.");
			break;

		case T_INLAY_WARFARE:
			pline("Using this technique requires sexy flats to be worn, and if you do use it, you'll create a stinking cloud at your position and may confuse and paralyze nearby monsters. You will also be confused for a while and need to wait for 2 turns before you can act again, meaning you'll probably be affected by the cloud too.");
			break;

		case T_DIAMOND_BARRIER:
			pline("Creates grave walls on the eight squares surrounding you, as long as those squares are open floor tiles.");
			break;

		case T_STEADY_HAND:
			pline("This technique increases your to-hit by a flat 5 points for a period of time.");
			break;

		case T_FORCE_FILLING:
			pline("If you use this technique, you restore a bit of mana.");
			break;

		case T_JEDI_TAILORING:
			pline("Creates a random robe that you can use.");
			break;

		case T_INTRINSIC_SACRIFICE:
			pline("If you use this technique, you may lose an intrinsic. But it will add a lot of energy to both of your lightsabers. Oh, almost forgot to mention this: it can only be used if you're wielding two lightsabers at once.");
			break;

		case T_BEAMSWORD:
			pline("When using this technique, your thrown lightsaber will be able to fire invisible beams for a while.");
			break;

		case T_ENERGY_TRANSFER:
			pline("While this technique is active, casting spells while wielding a lit lightsaber will add energy to the saber.");
			break;

		case T_SOFTEN_TARGET:
			pline("Allows you to target a single monster next to you, which will lose an experience level even if it resists level drain.");
			break;

		case T_PROTECT_WEAPON:
			pline("Can only be used while wielding a two-handed weapon, which will be erosionproofed.");
			break;

		case T_POWERFUL_AURA:
			pline("This technique temporarily grants magic resistance, reflection and free action.");
			break;

		case T_BOOSTAFF:
			pline("Using this technique requires you to wield a staff, and will try to enchant it (up to a maximum of +8, although the chance of enchanting it decreases with higher enchantment). Can also uncurse a cursed staff.");
			break;

		case T_CLONE_JAVELIN:
			pline("Requires you to wield a javelin, which will receive an extra ammo. Yes, you can multiply artifact javelins that way; this is not a bug.");
			break;

		case T_REFUGE:
			pline("While having this technique active, undead creatures that are near you will continuously take damage and may also be scared.");
			break;

		case T_DRAINING_PUNCH:
			pline("This martial arts move can be chained, and it may drain levels from monsters if they fail to resist. It requires you to be bare-handed.");
			break;

		case T_ESCROBISM:
			pline("A technique that lasts for a period of time and requires a robe to be worn, in which case it increases your bare-handed or lightsaber melee damage.");
			break;

		case T_PIRATE_BROTHERING:
			pline("Requires you to dual-wield with a lightsaber in your primary hand and a scimitar in your off-hand, and the lightsaber must be lit. While the technique is active, the lightsaber will not consume energy, and every time the scimitar hits something your lightsaber is recharged a bit.");
			break;

		case T_NUTS_AND_BOLTS:
			pline("Using this technique creates some crossbow bolts and increases your nutrition.");
			break;

		case T_DECAPABILITY:
			pline("While this technique is active, your polearm may sometimes put monsters to sleep if you apply it at them, and if your lightsaber destroys an enemy's weapon, the enemy in question is also put to sleep.");
			break;

		case T_NO_HANDS_CURSE:
			pline("Only works if you are wielding a two-handed weapon that is not cursed. The weapon in question will become heavily cursed and get a negative enchantment value, but you gain +1 luck, +100 alignment record, -1 divine anger and -500 prayer timeout. Also, hostile monsters that are next to you have a chance of becoming peaceful.");
			break;

		case T_HIGH_HEELED_SNEAKERS:
			pline("If you use this technique, you can use the high heels and sexy flats skills simultaneously for a while. But this only works if you actually have a pair that qualifies, which means it has to be one that has both a high-heeled randomized appearance and a sexy flats base item type or the other way around.");
			break;

		case T_FORM_CHOICE:
			pline("Requires you to be wielding a lightsaber, and will transform it into a different type of lightsaber that you can choose.");
			break;

		case T_STAR_DIGGING:
			pline("Fires digging rays in all eight directions.");
			break;

		case T_WONDER_YONDER:
			pline("Teaches you a random spell when used, but also causes long-lasting inertia and some random bad effects.");
			break;

		case T_ZAP_EM:
			pline("Tries to tame adjacent evilvariant monsters (use the pokedex to look for the 'Origin: Evil variant' bit). They can try to resist via monster magic resistance though.");
			break;

		case T_CARD_TRICK:
			pline("Inspired by Splicehack, this technique lets you choose a scroll in your open inventory for a chance to duplicate it. The higher the cost of writing the scroll in question with a magic marker, the lower the chance that your duplication attempt works, but higher technique levels will increase the chance of success. Scrolls that cannot be written with a magic marker, e.g. wishing, can never be duplicated with this technique because that would be too powerful, and artifact scrolls are impossible too.");
			break;

		case T_SKILLOMORPH:
			pline("Lets you pick a skill in which you're not restricted, and trains it by a few points. This lets you bypass the RNG limitations that might have made a skill impossible to train, meaning that repeatedly using this technique may allow you to reach skill levels that you can't otherwise reach, but of course you can't go over the cap displayed in the #enhance screen.");
			break;

		case T_SHOTTY_BLAST:
			pline("Creates shotgun shells out of thin air.");
			break;

		case T_AMMO_UPGRADE:
			pline("This technique requires you to wield firearm ammo, which may be enhanced in enchantment value and BUC. Remember that like everything that can affect a stack of items, big stacks of ammo (more than 25 ammos in one stack) have a chance to resist depending on the exact stack size. Ammo that is already highly enchanted is less likely to gain additional enchantment points, and the maximum that you can reach with this technique is +10.");
			break;

		case T_LASER_POWER:
			pline("Creates laser gun ammo out of thin air.");
			break;

		case T_BIG_DADDY:
			pline("This powerful technique creates a rocket launcher with ammo out of thin air.");
			break;

		case T_SHUT_THAT_BITCH_UP:
			pline("A technique that lasts for a couple of turns and allows your firearm ammos to stop time if they hit a female enemy.");
			break;

		case T_S_PRESSING:
			pline("This technique works only if there's no trap on your current location. It also requires 500 points of nutrition. If you fulfill those requirements, you create a trap at your location that can only affect hostile monsters and deals damage relative to your squeaking skill and technique level. However, only monsters that didn't see you create the trap can trigger it and they can also only ever trigger it while they're outside of your line of sight!");
			break;

		case T_MELTEE:
			pline("Shoots acid rays in all directions and may destroy iron bars.");
			break;

		case T_WOMAN_NOISES:
			pline("If you use this technique, hostile monsters in a certain radius around you have to make a saving throw or be affected by several bad effects.");
			break;

		case T_EXTRA_LONG_SQUEAK:
			pline("After using this technique, it will stay active for a while and protect you from farting effects, and additionally all nearby hostile male monsters will have to make a saving throw each turn or take damage.");
			break;

		case T_SEXUAL_HUG:
			pline("This technique tames an adjacent male monster. It has a 1 in 5 chance of taming boss monsters.");
			break;

		case T_SEX_CHANGE:
			pline("Well DUH :D Using it will change your sex from male to female or the other way around!");
			break;

		case T_EVEN_MORE_AMMO:
			pline("You must be wielding firearm ammo to use this technique, and if you do, its stack size will increase.");
			break;

		case T_DOUBLESELF:
			pline("Only player characters that are currently polymorphed into a monster can use this technique, and if you do so, you'll spawn a tame monster of the same type as your current polymorph form.");
			break;

		case T_POLYFIX:
			pline("Using this technique lets you select a hazy item in your open inventory, which will become un-hazy so it won't polymorph back into the original item.");
			break;

		case T_SQUEAKY_REPAIR:
			pline("This technique creates a fart trap on your current location and triggers it, but only if there was no trap on your location. If the trap was successfully made, you then get to repair an item of your choice in your main inventory.");
			break;

		case T_BULLETREUSE:
			pline("Using this technique makes your firearm ammo reusable for a while, i.e. ammos that you shot may drop on the ground and be re-used, although most of the time they'll still be mulched.");
			break;

		case T_EXTRACHARGE:
			pline("This technique lets you pick a wand in your open inventory and add a charge to it without incrementing the recharge counter and without the chance of blowing up the wand, but caution: the wand must be one that can be recharged indefinitely. Using it on one that can only be recharged once (e.g. wand of wishing) will not work.");
			break;

		case T_STARWARS_FRIENDS:
			pline("A technique that tries to tame all adjacent monsters which are from Castle of the Winds or joke monsters, but they get a resistance roll. Also, one tame such monster is generated from thin air.");
			break;

		case T_USE_THE_FORCE_LUKE:
			pline("This technique lasts for a period of time and increases the damage of the #force command while active.");
			break;

		case T_TERRAIN_CLEANUP:
			pline("This powerful technique removes most annoying terrain from the eight squares surrounding you: water, trees, ice, farmland, nether mist and so on... It doesn't raze walls though, and furniture (e.g. wagons or fountains) is unaffected. The general rule of thumb is that if the terrain type can replace walls in maze levels, it will be affected.");
			break;

		case T_SYMBIOSIS:
			pline("The basic symbiosis technique, using this will create a symbiote item out of thin air with which you can then perform symbiosis. It's probably a good idea to use this technique whenever you can, as it allows you to slowly but steadily build up a supply of symbiote items for later use.");
			break;

		case T_ADJUST_SYMBIOTE:
			pline("Adjusts the aggressivity of your symbiotes, which is a global value that persists even if you switch to a different symbiote. Higher aggressivity means that your symbiote's attacks trigger more often, and damage that you would normally take is redirected to the symbiote more often.");
			break;

		case T_HEAL_SYMBIOTE:
			pline("Damages you by half of your current HP and transfers that amount of hit points to your symbiote, but if that causes it to be overhealed, the extra points will just go to waste.");
			break;

		case T_BOOST_SYMBIOTE:
			pline("Whenever you use this technique, your symbiote's maximum health will increase. However, there is a cap of 500 HP for the symbiote.");
			break;

		case T_POWERBIOSIS:
			pline("Activating this technique temporarily allows your symbiote to deal double damage and take half damage.");
			break;

		case T_UNARMED_FOCUS:
			pline("A technique meant to be used by those who really want to focus on bare-handed or martial arts combat. It lasts for a pretty long time, and even longer if you keep wearing a robe of focussing. While it's active, it increases the base damage multiplier of your fists by about 50%%. However, all other active techniques are stopped and while it's active, you cannot use any techniques at all. Also, trying to use a weapon with this technique active gives a hefty to-hit penalty, so you'd better make sure you really want to focus on fighting unarmed!");
			break;

		case T_UNDERTOW:
			pline("Allows you to try to drown a monster. For that to work, either you or the monster needs to be on a water square (can be a pool, moat, watery tunnel or crystal water) and the monster may not be unbreathing of course.");
			break;

		case T_DEFINALIZE:
			pline("If you're annoyed by all the finalized items that you cannot polymorph, blank or cancel, this technique allows you to remove the finalized flag from one item.");
			break;

		case T_ANTI_INERTIA:
			pline("Useful when you're slowed down by inertia, because it reduces the amount of time for which you're slowed.");
			break;

		case T_GRENADES_OF_COURSE:
			pline("Creates frag grenades or gas grenades.");
			break;

		case T_IMPLANTED_SYMBIOSIS:
			pline("This technique lasts for quite a while when you use it. While active, you benefit from the more powerful implant effects as long as you're in symbiosis, and if your implant is positively enchanted, your symbiote will take less damage based on the exact enchantment value.");
			break;

		case T_ASSUME_SYMBIOTE:
			pline("Polymorphs you into the base monster type of your current symbiote. Doing that causes your symbiote to persist, so don't worry about it dying from this tech - it just makes it so that both you and your symbiote are of the same species. What's more, you will be able to move around as long as you still have the symbiote, even though symbiotes are characterized by the fact that they're nonmoving monsters.");
			break;

		case T_GENERATE_OFFSPRING:
			pline("If you use this technique while having a symbiote, you will lay an egg of the symbiote's monster type that will hatch tame after a while.");
			break;

		case T_PREACHING:
			pline("Using this technique breaks the atheist conduct and slightly increases your alignment record, but not above the current maximum.");
			break;

		case T_ON_THE_SAME_TEAM:
			pline("Tries to pacify hostile neutral-aligned monsters next to you, but they get a resistance roll.");
			break;

		case T_SPELL_SPAM:
			pline("A tech that lasts for a hundred turns or so, and while active, your spells don't lose spell memory over time, cost a bit less mana, and are more effective if they're of a type that scales with skill.");
			break;

		case T_DROP_BOULDER:
			pline("Creates a boulder on your square when used.");
			break;

		case T_SPRINT:
			pline("Requires you to wear stiletto heels, and makes you super fast for a couple of turns when used, but it only affects movement. Other actions still happen at your regular speed.");
			break;
		case T_SKULL_CRUSH:
			pline("This technique can only be used when you're wearing stiletto heels. After using it, you have a couple of turns to find a paralyzed monster that has a head and is not a boss, and then you can kick that monster to kill it instantly! For best results, have a way of paralyzing opponents ready.");
			break;
		case T_FEMALE_COMBO:
			pline("Requires cone heels to be worn, and once activated, you have to kick a monster every turn or the technique will end. Every kick that hits an enemy while the female combo is active will deal more damage than the previous one.");
			break;
		case T_DOCKLOCK:
			pline("Creates a sexy leather pump with positive enchantment, which is a hammer-class weapon that you can wield and bash enemies with. While this technique is active, the sexy leather pump will do bonus damage against enemies who have a head and can also occasionally paralyze them.");
			break;
		case T_BITCH_POSING:
			pline("This technique works only while you're wearing block heels. If they're negatively enchanted, they lose some negative enchantment, otherwise there's a chance for the heels to gain a point of enchantment up to a maximum of +7. Additionally, monsters who are at most 3 squares away from you have to make saving throws or be turned peaceful.");
			break;
		case T_SEXY_STAND:
			pline("Lasts for a while, but works only if you wear block heels. While active, you have intrinsic free action and if you still get paralyzed, the paralysis will time out very quickly.");
			break;
		case T_MARATHON:
			pline("This technique lasts for quite a while, but doesn't do anything unless you're wearing wedge heels. If you are, your movement is faster than normal.");
			break;
		case T_PERFUME_STRIDE:
			pline("Requires you to be wearing wedge heels, which become sticky heavily cursed upon using this technique, and a tame perfume monster is summoned. The technique then lasts for a while and as long as it's still active, you cannot take your heels off, even if you manage to uncurse them.");
			break;
		case T_NAUGHTY_HEELOT:
			pline("This technique lasts for quite a while. When active, a pair of shoes that has a randomized appearance counting as cone heels and a base type counting as stiletto heels (or the other way around) allows you to use both of those skills at the same time. You'd have to get pretty lucky to find such a pair of shoes, though.");
			break;
		case T_EXTREME_STURDINESS:
			pline("Works while you're wearing either block or wedge heels, lasts for a bunch of turns, and gives you a 50%% chance of not taking damage whenever something tries to damage you.");
			break;
		case T_BUTT_PROTECTION:
			pline("If you're wearing cone heels, this technique will last for a while and as long as it's active, any male monster that hits you in melee takes some damage in return, provided it has legs that can be scratched by your heels. The more often such a monster is hit by that damage, the higher the amount of damage that it takes.");
			break;
		case T_PROFILING:
			pline("Allows you to enchant a pair of block-heeled boots that you're wearing, up to a maximum of +5, but you can't just use it an infinite amount of times on the same pair. Eventually, it'll stop working for a particular pair of boots, although you can still use it on others.");
			break;
		case T_STAR_HEEL_SWAP:
			pline("You must be wearing a pair of wedge heels and have a morning star wielded in your main hand if you want to use this technique. Then, what happens is that these two items swap their enchantment value and BUC.");
			break;
		case T_HEEL_STAB:
			pline("You can only use this technique if you're wearing a pair of stiletto heels and wielding a knife. Then, it'll last a while, and your knife will gain a +5 damage bonus, plus the enchantment value of your boots if that's higher than 0.");
			break;

		case T_EARTHSHAKE:
			pline("This technique mimics the scroll of earth, dropping boulders on your square and the eight squares surrounding you, but minus the effect of boulders hitting monsters.");
			break;

		case T_MARTIAL_STYLE:
			pline("A must have for martial arts users, this technique lets you switch to different martial arts styles. Depending on your skill levels, certain ones are available, plus brawling, which is the default. All other styles have upsides and downsides, so choose wisely, because the only way to switch is by using this tech again! Also, most of these styles work only if you're fighting bare-handed.");
			break;

		case T_PERMAMORPH:
			pline("This powerful technique lets you target an adjacent monster and tries to polymorph it, although it can resist via monster magic resistance and those that have player-style magic resistance are outright immune. If you're successful in polymorphing it, the monster's polymorph will be semi-permanent.");
			break;

		case T_HARDCORE_ALIENIZATION:
			pline("Creates a shemagh out of thin air and forces you to wear it, unequipping any other helmet-slot item you may be wearing.");
			break;

		case T_PACIFY:
			pline("Targets an adjacent humanoid monster and tries to turn it peaceful, although it can also resist. Shopkeepers, guards, temple priests and unique monsters are immune.");
			break;

		case T_AFTERBURNER:
			pline("For a few turns after using this technique, it shoots fire bolts in the direction you came from.");
			break;

		case T_BUGGARD:
			pline("Deals cold damage and tries to slow targets in a 5x5 area near you, although it doesn't always hit. This technique lasts for a few turns before it ends.");
			break;

		case T_THUNDERSTORM:
			pline("Lasts for a while and periodically deals lightning damage to nearby monsters.");
			break;

		case T_AUTOKILL:
			pline("Shoots highly damaging lightning bolts in all eight directions. This technique sets a timeout for all of the following techniques: autokill, chain thunderbolt and flashing mischief.");
			break;

		case T_CHAIN_THUNDERBOLT:
			pline("Shoots an invisible globe in a direction of your choosing, which will travel for 8 squares or until it hits a terrain obstacle. At the target destination, it shoots highly damaging lightning bolts in all eight directions. This technique sets a timeout for all of the following techniques: autokill, chain thunderbolt and flashing mischief.");
			break;

		case T_FLASHING_MISCHIEF:
			pline("Shoots an invisible globe in a direction of your choosing, which will travel for 8 squares or until it hits a terrain obstacle. For each square it travels, the globe shoots a highly damaging lightning bolt in a random direction. This technique sets a timeout for all of the following techniques: autokill, chain thunderbolt and flashing mischief.");
			break;

		case T_KAMEHAMEHA:
			pline("A single-target ranged attack that travels in the chosen direction for 8 squares, but not through terrain obstacles, and deals damage to the first monster it hits.");
			break;

		case T_SHADOW_MANTLE:
			pline("Darkens the area and blinds nearby monsters.");
			break;

		case T_VACUUM_STAR:
			pline("Creates throwing stars out of thin air.");
			break;

		case T_BLADE_SHIELD:
			pline("This technique lasts for a while and periodically deals damage to hostile monsters adjacent to you.");
			break;

		case T_GREEN_WEAPON:
			pline("Requires you to be wielding a claw, which will be badly poisoned, making it less likely to lose its poisoning compared to regular poisoned weapons.");
			break;

		case T_LIGHTER_BALLS:
			pline("Lasts for quite a while, and reduces the weight of your wielded heavy iron ball by half while active.");
			break;

		case T_BALLSLIFF:
			pline("This technique only works if you're wielding a heavy iron ball, or another type of heavy ball as a weapon, and repairs a bit of damage on the ball.");
			break;

		case T_JAVELIN_FORGING:
			pline("Creates a random javelin when used.");
			break;

		case T_VENOM_MIXING:
			pline("An arcane technique that allows you to create venom out of thin air.");
			break;

		case T_ROCK_TO_POISON:
			pline("Allows you to target a stack of regular rocks, which may not be an artifact, and attempts to turn them into acid venoms. The standard rules for manipulating a stack apply, so in most cases, attempting to target a stack of more than 25 rocks may fail.");
			break;

		case T_INJECTION:
			pline("Targets an adjacent monster and hits it with nerve poison, damaging and paralyzing it as long as it doesn't resist.");
			break;

		case T_SUPER_POISON:
			pline("Requires you to wield a stack of venoms, and tries to add some enchantment value to it. This works only if the venom's enchantment is less than +25.");
			break;

		case T_GREEN_MISSILE:
			pline("Lasts for a couple of turns, and as long as it's active, throwing venoms will additionally launch poison blasts at the enemy.");
			break;

		case T_BIG_N_VEINY:
			pline("Activating this technique means that monsters who hit you in melee are hit with poison for a while.");
			break;

		case T_PLANT_TERROR:
			pline("Transforms you into a meshera plant for a couple dozen turns. That's an immobile monster, so be aware that you cannot run away once transformed. In addition, a few tame little poison ivies are summoned who will help you plant new trees in Gotham Forest. You can only hope that Batsy, that old bat, won't come to try and stop you with his batarangs, though.");
			break;

		case T_POISON_PEN_LETTER:
			pline("Doesn't last very long at all, but while it does last, you can engrave with venoms. Doing so releases a stinking cloud at your location, which is bigger the more letters you engrave at once.");
			break;

		case T_CORONATION_CULMINATION:
			pline("Infects you with covid-19 and gives a random good effect when used.");
			break;

		case T_RESTORE_LIFE_LEVELS:
			pline("When using this technique after you got your levels drained, they will be restored. No effect if your level wasn't drained.");
			break;

		case T_MAKE_A_WISH:
			pline("Using this technique requires you to guess an item. If you guess correctly, you get the item in question, otherwise you're forced to equip a bad item.");
			break;

		case T_TOILET_VISIT:
			pline("If you want to play the antje trap 2 minigame where you visit the public toilet, you can use this technique. You will also gain maximum health and cure sickness when doing so.");
			break;

		case T_POLE_MELEE:
			pline("For a while, your polearms and lances are effective at close range after you've used this technique.");
			break;

		case T_CHOP_CHOP:
			pline("In order to use this technique, you have to be wielding an axe, which will hit all adjacent monsters. Be careful, it can also hit peaceful targets!");
			break;

		case T_BANISHMENT:
			pline("Allows you to banish an adjacent monster, sending it to a random dungeon level. Careful: if the demigod event has already been triggered, it only teleports the monster on the current level!");
			break;

		case T_PARTICIPATION_LOSS:
			pline("This extremely powerful technique lets you delete an adjacent monster from the game, but it doesn't work against bosses, shopkeepers or temple priests. Be aware that the monster's inventory is deleted with it (macguffins will drop to the floor to prevent the game from becoming unwinnable).");
			break;

		case T_WEAPON_BLOCKER:
			pline("A technique that lasts for a while, but requires you to be dual-wielding. While active, you have a chance of using your weapons to block melee attacks and projectiles.");
			break;

		case T_EXTRA_MEMORY:
			pline("You can select a spell and increase its memory with this technique. It has a minuscule chance of working on a forgotten spell, allowing you to cast it again, but most of the time, using it on a forgotten spell will simply fail.");
			break;

		case T_GRAP_SWAP:
			pline("Temporarily makes your grinder work like a lance and your lance will work like a grinder. Or if that sounds like Chinese to you, it means that you can joust with a grinder and grind with a lance.");
			break;

		case T_DIABOLIC_MINION:
			pline("If you use this technique, a random tame Diablo monster is summoned.");
			break;

		case T_CURE_AMNESIA:
			pline("A useful technique that removes the 'map amnesia' intrinsic. Be aware that if you're e.g. playing the amnesiac race, you'll still have map amnesia. It's useful especially when you triggered a lasting amnesia trap.");
			break;

		case T_ELEMENTAL_IMBUE:
			pline("This technique transforms your currently wielded weapon into an elemental-branded artifact. It works only if the weapon in question is neither a real nor fake artifact, and only if it's actually listed under 'weapons' in your inventory. Plus, it obviously doesn't work on a stack.");
			break;

		case T_HIDDEN_POWER:
			pline("Using this technique creates a specific item, the identity of which depends on the skill you used for unlocking the technique.");
			break;

		case T_SWORD_ART:
			pline("Requires you to wield a weapon that uses one of the following skills: short sword, broad sword, long sword or two-handed sword. The weapon in question will gain one level of enchantment, but not beyond +7.");
			break;

		case T_FIRM_CUDGEL:
			pline("Requires you to wield a weapon that uses one of the following skills: mace, flail, club or hammer. The weapon in question will be erosionproofed and all erosion damage is repaired.");
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

	boolean maybeleveltech = FALSE;

	/* check timeout */
	if (tech_inuse(techid(tech_no))) {
	    pline("This technique is already active!");
	    return (0);
	}
        if (techtout(tech_no) /*&& !can_limitbreak()*/) {
	    You("have to wait %s before using your technique again.",
                (techtout(tech_no) > 100) ?
                        "for a while" : "a little longer");
#ifdef WIZARD
            if (!wizard || (yn("Use technique anyways?") == 'n'))
#endif
                return(0);
        }

	if ((TechTrapEffect || u.uprops[TECHBUG].extrinsic || have_techniquestone()) && (rn2(10) || TechBugXtra ) ) {

		pline("Unfortunately, nothing happens.");
		techtout(tech_no) = rnz(5000);
		if (ishaxor && techtout(tech_no) > 1) techtout(tech_no) /= 2;
		/*By default,  action should take a turn*/
		return(1);

	}

	if (uarmc && uarmc->oartifact == ART_ARTIFICIAL_FAKE_DIFFICULTY && !rn2(6)) {

		pline("Unfortunately, nothing happens.");
		techtout(tech_no) = rnz(1000);
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

	if (tech_inuse(T_UNARMED_FOCUS)) {

		pline("While you're focussing on unarmed combat, you cannot use any techniques. Sorry!");
		return(0);

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
                    t_timeout = rnz(2000);
		    break;
		} else if(invent) {
			You("examine your possessions.");
			identify_pack((int) ((techlevX(tech_no) / 10) + 1), 0, 0);
		} else {
			/* KMH -- fixed non-compliant string */
		    You("are already quite familiar with the contents of your pack.");
		    break;
		}
                t_timeout = rnz(5000);
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
		techt_inuse(tech_no) = d(2,4) + techlevX(tech_no)/2 + 2;
		t_timeout = rnz(2000);
		break;
            case T_BERSERK:
		You("fly into a berserk rage!");
		techt_inuse(tech_no) = d(2,8) +
               		(techlevX(tech_no)/2) + 2;
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
                techt_inuse(tech_no) = rnd((int) (techlevX(tech_no)/6 + 1)) + 2;
                t_timeout = rnz(1500);
		break;
            case T_INVOKE_DEITY: /* ask for healing if your alignment record is positive --Amy */

			if (Race_if(PM_MACTHEIST)) {
				pline("As an atheist, you cannot use such a technique.");
				return 0;
			}

			if (u.ualign.record < 0) {

				if ( (Inhell && !Race_if(PM_HERETIC) ) || flags.gehenna ) {
					pline("%s is inaccessible, and %s decides to smite you!",u_gname(), Role_if(PM_GANG_SCHOLAR) ? "Anna" : Role_if(PM_WALSCHOLAR) ? "Anna" : "Moloch" );
					u.ublesscnt += rnz(-u.ualign.record);
					if (Role_if(PM_GANG_SCHOLAR)) losehp(rnz(-u.ualign.record), "annoying Anna", KILLED_BY);
					else if (Role_if(PM_WALSCHOLAR)) losehp(rnz(-u.ualign.record), "annoying Anna", KILLED_BY);
					else losehp(rnz(-u.ualign.record), "annoying Moloch", KILLED_BY);
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
				pline("%s is inaccessible, and %s decides to smite you!",u_gname(), Role_if(PM_GANG_SCHOLAR) ? "Anna" : Role_if(PM_WALSCHOLAR) ? "Anna" : "Moloch" );
				u.ublesscnt += rnz(monster_difficulty() + 1 );
				losehp(rnz(monster_difficulty() + 1 ), "trying to contact their deity in Gehennom", KILLED_BY);
/* Trying to invoke a deity in Gehennom is never a good idea... */
			} 

			else if (u.ualign.record > 0) {
				pline("%s commends your efforts and grants you a boon.",u_gname() );
				u.ublesscnt -= rnz(u.ualign.record + techlevX(tech_no) );
				if(u.ublesscnt < 0) u.ublesscnt = 0;
				healup( rnz(u.ualign.record + techlevX(tech_no)) , 0, FALSE, FALSE);
/* If your deity is pleased with you, heal some damage and decrease your prayer timeout. */
			}

			else { /* alignment record exactly 0, do nothing */
				pline("%s ignores your pleadings.",u_gname() );
			}

			u.uconduct.gnostic++;	/* you just tried to access your god */
			if (Race_if(PM_MAGYAR)) {
				You_feel("bad about breaking the atheist conduct.");
				badeffect();
			}
			use_skill(P_SPIRITUALITY, Role_if(PM_PRIEST) ? 10 : 2);
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
				if (u.enchantrecskill < 100 || !rn2(u.enchantrecskill)) {
					u.enchantrecskill++;
					if (u.enchantrecskill > 250) u.enchantrecskill = 250;
				}
				if (uwep && uwep->oartifact == ART_KAMAHEWA && uwep->spe < 7) {
					uwep->spe++;
					Your("gauntlets seem more effective.");
				}
            		t_timeout = rnz(2000);
			} else {
				You("examine %s.", doname(uwep));
				uwep->known = TRUE;
				You("discover it is %s",doname(uwep));
				if (u.weapchantrecskill < 100 || !rn2(u.weapchantrecskill)) {
					u.weapchantrecskill++;
					if (u.weapchantrecskill > 250) u.weapchantrecskill = 250;
				}

				if (Race_if(PM_GERTEUT) && is_poisonable(uwep) && !stack_too_big(uwep)) {
					Your("%s is poisoned.", doname(uwep));
					uwep->opoisoned = TRUE;
				}
				if (uwep && uwep->oartifact == ART_KAMAHEWA && uwep->spe < 7) {
					uwep->spe++;
					Your("gauntlets seem more effective.");
				}
				if (!(uarmc && uarmc->oartifact == ART_PINEAPPLE_TYCOON_S_FINISH) || !rn2(4)) {
					t_timeout = rnz(200);
				}
			}
		break;

            case T_PHASE_DOOR:
			phase_door(0);
                if (!Role_if(PM_CAMPERSTRIKER) || !rn2(4)) t_timeout = rnz(1000);
		break;

            case T_PANIC_DIGGING:
			pline("You perform the panic digging ritual and are paralyzed for a while.");
			int maderoom = 0;

			do_clear_areaX(u.ux, u.uy, 1, undo_lockfloodP, (void *)&maderoom);

			if (maderoom) pline("Some solid rock is pulverized!");
			else pline("There was nothing to dig out...");

			if (isstunfish) nomul(-rnz(20), "panic digging", TRUE);
			else nomul(-rn1(10, 10), "panic digging", TRUE);
			if (!PlayerCannotUseSkills && P_SKILL(P_PICK_AXE) >= P_SUPREME_MASTER) t_timeout = rnz(500);
			else if (!PlayerCannotUseSkills && P_SKILL(P_PICK_AXE) >= P_GRAND_MASTER) t_timeout = rnz(1500);
			else if (!PlayerCannotUseSkills && P_SKILL(P_PICK_AXE) >= P_MASTER) t_timeout = rnz(2500);
			else t_timeout = rnz(5000);
		break;

            case T_SECURE_IDENTIFY:

			if (CannotSelectItemsInPrompts) break;
			pline("Choose an item for secure identification.");

secureidchoice:
			{
			otmp = getobj(allnoncount, "secure identify");

			if (!otmp) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to secure identify your objects.");
				else goto secureidchoice;
				pline("A feeling of loss comes over you.");
				break;
			}
			if (otmp) {
				makeknown(otmp->otyp);
				if (otmp->oartifact) discover_artifact((int)otmp->oartifact);
				otmp->known = otmp->dknown = otmp->bknown = otmp->rknown = 1;
				if (otmp->otyp == EGG && otmp->corpsenm != NON_PM)
				learn_egg_type(otmp->corpsenm);
				prinv((char *)0, otmp, 0L);
			}

			}

			if (u.glassgolemdown) t_timeout = rnz(10000);
			else t_timeout = rnz(20000);
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
					if (u.weapchantrecskill < 100 || !rn2(u.weapchantrecskill)) {
						u.weapchantrecskill++;
						if (u.weapchantrecskill > 250) u.weapchantrecskill = 250;
					}
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

			use_skill(P_DEVICES,1);
			if (Race_if(PM_FAWN)) {
				use_skill(P_DEVICES,1);
			}
			if (Race_if(PM_SATRE)) {
				use_skill(P_DEVICES,1);
				use_skill(P_DEVICES,1);
			}

			pline("Using a bandage, you dress your wounds."); /* read after free fix by Demo */
			healup(techlevX(tech_no) * (rnd(2)+1) + (rn1(5,5) * techlevX(tech_no)),
			  0, FALSE, FALSE);
		    } else {
			You("strap your wounds as best you can.");
			healup(techlevX(tech_no) + rn1(5,5), 0, FALSE, FALSE);
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
		    healup(techlevX(tech_no) * 4, 0, FALSE, FALSE);
		    t_timeout = rnz(3000);
		} else {
			pline("%s", nothing_happens);
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
            case T_KIII:
		You("scream \"KIIILLL!\"");
		aggravate();
                techt_inuse(tech_no) = rnd((int) (techlevX(tech_no)/2 + 1)) + 2;
                t_timeout = rnz(1500);
		break;
	    case T_CALM_STEED:

		if (u.usteed) pline("You calm your steed.");
		int calmedX;
		calmedX = 0;

                if (u.usteed) {


                        pline("%s gets tamer.", Monnam(u.usteed));
                        tamedog(u.usteed, (struct obj *) 0, FALSE); /* caveat: you might be riding a non-tame monster */

				while (calmedX == 0) { /* remove the stupid bug that caused this tech to do nothing --Amy */

				if (u.usteed && u.usteed->mtame < 20) u.usteed->mtame++;

				if (techlevX(tech_no) < rnd(50)) calmedX++; /* high level tech has high chance of extra tameness */

				}

                        t_timeout = rnz(1500);
                } else
                        Your("technique is only effective when riding a monster.");
                break;
            case T_TURN_UNDEAD:
			if (!turn_undead()) t_timeout = rnz(10);
			else t_timeout = rnz(50);
			break;
	    case T_VANISH:
		if (Invisible && Fast) {
			You("are already quite nimble and undetectable.");
		}
                techt_inuse(tech_no) = rn1(10,10) + (techlevX(tech_no) * 2);
		if (!Invisible) pline("In a puff of smoke,  you disappear!");
		if (!Fast) You_feel("more nimble!");
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
		if (!isok(u.ux + u.dx, u.uy + u.dy)) {
			pline("Invalid target location.");
			return 0;
		}
		mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
		if (!mtmp) {
			You("perform a flashy twirl!");
			t_timeout = rnz(1500);
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
			tmp += techlevX(tech_no);
			t_timeout = rnz(1500);
			hurtmon(mtmp, tmp);
		    }
		}
		break;
	    case T_CUTTHROAT:
		if (!uwep) {
		    You("obviously can't perform that without a weapon.");
		    return 0;		
		}
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
		if (!isok(u.ux + u.dx, u.uy + u.dy)) {
			pline("Invalid target location.");
			return 0;
		}
		mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
		if (!mtmp) {
			You("attack...nothing!");
			t_timeout = rnz(6000);
		} else {
		    int oldhp = mtmp->mhp;

		    if (!attack(mtmp)) return 0;
		    if (!DEADMONSTER(mtmp) && mtmp->mhp < oldhp) {
			if (!has_head(mtmp->data) || u.uswallow)
			    You_cant("perform cutthroat on %s!", mon_nam(mtmp));
			else {
			    int tmp = 0;
			    int severlevel = techlevX(tech_no) * 2;
			    if (severlevel < 1) severlevel = 1; /* fail safe */

			    if ((rn2(8) < (techlevX(tech_no)/10 + 1)) && (mtmp->m_lev <= rnd(severlevel)) ) {
				You("sever %s head!", s_suffix(mon_nam(mtmp)));
				tmp = mtmp->mhp;
			    } else {
				You("hurt %s badly!", s_suffix(mon_nam(mtmp)));
				tmp = mtmp->mhp / 2;
			    }
			    tmp += techlevX(tech_no);
			    t_timeout = rnz(6000);
			    hurtmon(mtmp, tmp);
			}
		    }
		}
		break;
	    case T_BLESSING:
		allowall[0] = ALL_CLASSES; allowall[1] = '\0';
		
		if (CannotSelectItemsInPrompts) break;
		if ( !(obj = getobj(allowall, "bless"))) return(0);
		pline("An aura of holiness surrounds your hands!");
                if (!Blind) (void) Shk_Your(Your_buf, obj);
		if (obj->cursed) {
                	if (!Blind)
                    		pline("%s %s %s.",Your_buf,
						  aobjnam(obj, "softly glow"),
						  hcolor(NH_AMBER));
				if (!stack_too_big(obj)) uncurse(obj, FALSE);
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
		t_timeout = rnz(7500);
		if (uarmc && uarmc->oartifact == ART_PRIESTEST_BLESS) t_timeout = rnz(6000);
		break;
	    case T_E_FIST: 
	    	blitz_e_fist();
#if 0
		str = makeplural(body_part(HAND));
                You("focus the powers of the elements into your %s", str);
                techt_inuse(tech_no) = rnd((int) (techlevX(tech_no)/3 + 1)) + d(1,4) + 2;
#endif
		t_timeout = rnz(1500);
	    	break;
	    case T_PRIMAL_ROAR:	    	
	    	You("let out a bloodcurdling roar!");
	    	aggravate();

		techt_inuse(tech_no) = d(2,6) + (techlevX(tech_no) * rnd(4)) + 2;

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
	    	} else if (distu(cc.x, cc.y) > 19 + techlevX(tech_no)) {
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
		    if (Punished && !u.repunishcounter) {
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
		    	    if (!resists_acid(mtmp) || player_will_pierce_resistance()) {
				int tmp = 1;
				/* Need to add a to-hit */
				tmp += d(2,4);
				tmp += rn2((int) (techlevX(tech_no) + 1));
				if (!Blind) pline_The("acid burns %s!", mon_nam(mtmp));
				hurtmon(mtmp, tmp);
			    } else if (!Blind) pline_The("acid doesn't affect %s!", mon_nam(mtmp));
			}
			/* Clean up */
			tmp_at(DISP_END,0);
			if (tmp_invul) Invulnerable = 0;
		    }

		    /* A little Sokoban guilt... */
		    if (In_sokoban(&u.uz) && !playercancheatinsoko()) {
			change_luck(-1);
			pline("You cheater!");
			if (evilfriday) u.ugangr++;
		    }
		    You("reform!");
		    teleds(cc.x, cc.y, FALSE);
		    nomul(-1, "liquid leaping", TRUE);
		    nomovemsg = "";
	    	}
		t_timeout = rnz(1500);
	    	break;
	    }
            case T_SIGIL_TEMPEST: 
		/* Have enough power? */
		num = 50 - techlevX(tech_no);
		if (num < 10) num = 10;
		if (u.uen < num) {
			You("don't have enough power to invoke the sigil! You need at least %d!",num);
			return (0);
		}
		u.uen -= num;

		/* Invoke */
		You("invoke the sigil of tempest!");
                techt_inuse(tech_no) = d(1,6) + rnd(techlevX(tech_no)/2 + 1) + 2;
		u_wipe_engr(2);
                t_timeout = rnz(50);
		return(0);
		break;
            case T_SIGIL_CONTROL:
		/* Have enough power? */
		num = 30 - techlevX(tech_no)/2;
		if (num < 10) num = 10;
		if (u.uen < num) {
			You("don't have enough power to invoke the sigil! You need at least %d!",num);
			return (0);
		}
		u.uen -= num;

		/* Invoke */
		You("invoke the sigil of control!");
                techt_inuse(tech_no) = d(1,4) + rnd(techlevX(tech_no)/2 + 1) + 2;
		u_wipe_engr(2);
                t_timeout = rnz(50);
		return(0);
		break;
            case T_SIGIL_DISCHARGE:
		/* Have enough power? */
		num = 100 - techlevX(tech_no)*2;
		if (num < 10) num = 10;
		if (u.uen < num) {
			You("don't have enough power to invoke the sigil! You need at least %d!",num);
			return (0);
		}
		u.uen -= num;

		/* Invoke */
		You("invoke the sigil of discharge!");
                techt_inuse(tech_no) = d(1,4) + rnd(techlevX(tech_no)/2 + 1) + 2;
		u_wipe_engr(2);
                t_timeout = rnz(50);
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
				if (!resist(mtmp, SPBOOK_CLASS, 0, TELL) || ((rnd(30 - ACURR(A_CHA))) < 4) ) {
				   mtmp = tamedog(mtmp, (struct obj *) 0, FALSE);
				   You("dominate %s!", mon_nam(mtmp));
				} else setmangry(mtmp);
			    }
			}
		    }
		}
		nomul(-2, "recovering from an attempt to raise zombies", TRUE); /* You need to recover */
		nomovemsg = 0;
		t_timeout = rnz(1500);
		break;
            case T_REVIVE:

		if (CannotSelectItemsInPrompts) break;
		if (u.uswallow) {
		    You("%s", no_elbow_room);
		    return 0;
		}
            	num = 100 - techlevX(tech_no); /* WAC make this depend on mon? */
			if (num < 25) num = 25;
            	if ((Upolyd && u.mh <= num) || (!Upolyd && u.uhp <= num)){
		    You("don't have the strength to perform revivification!");
		    return 0;
            	}

            	obj = getobj((const char *)revivables, "revive");
            	if (!obj) return (0);
            	mtmp = revive(obj);
            	if (mtmp) {
		    if (Is_blackmarket(&u.uz))
			setmangry(mtmp);
		    else
		    if (mtmp->isshk)
			make_happy_shk(mtmp, FALSE);
		    else if (!resist(mtmp, SPBOOK_CLASS, 0, NOTELL) || (((rnd(30 - ACURR(A_CHA))) < 4) && !resist(mtmp, SPBOOK_CLASS, 0, TELL) ) || (((rnd(30 - ACURR(A_CHA))) < 4) && !resist(mtmp, SPBOOK_CLASS, 0, TELL) ) )
			(void) tamedog(mtmp, (struct obj *) 0, FALSE);
		}
            	if (Upolyd) u.mh -= num;
            	else u.uhp -= num;
		t_timeout = rnz(5000);
            	break;
	    case T_WARD_FIRE:
		/* Already have it intrinsically? */
		if (HFire_resistance & FROMOUTSIDE) {
			You("are already fire resistant.");
			return (0);
		}

		You("invoke the ward against flame!");
		HFire_resistance += rn1(100,50);
		HFire_resistance += techlevX(tech_no);
		t_timeout = rnz(1500);

	    	break;
	    case T_WARD_COLD:
		/* Already have it intrinsically? */
		if (HCold_resistance & FROMOUTSIDE) {
			You("are already cold resistant.");
			return (0);
		}

		You("invoke the ward against ice!");
		HCold_resistance += rn1(100,50);
		HCold_resistance += techlevX(tech_no);
		t_timeout = rnz(1500);

	    	break;
	    case T_WARD_ELEC:
		/* Already have it intrinsically? */
		if (HShock_resistance & FROMOUTSIDE) {
			You("are already shock resistant.");
			return (0);
		}

		You("invoke the ward against lightning!");
		HShock_resistance += rn1(100,50);
		HShock_resistance += techlevX(tech_no);
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
		delay = -150 + techlevX(tech_no);
		if (delay > -100) delay = -100;
		set_occupation(tinker, "tinkering", 0);
		t_timeout = rnz(200);
		break;
	    case T_RAGE:     	
		/*if (Upolyd) {
			You("cannot focus your anger!");
			return(0);
		}*/
	    	You_feel("the anger inside you erupt!");
		num = 50 + (4 * techlevX(tech_no));
	    	techt_inuse(tech_no) = num + 1;
		if (Upolyd) {
			u.mhmax += num;
			u.mh += num;
		}
		u.uhpmax += num;
		u.uhp += num;
		t_timeout = rnz(5000);
		break;	    
	    case T_BLINK:
	    	You_feel("the flow of time slow down.");
                techt_inuse(tech_no) = rnd(techlevX(tech_no) + 1) + 2;
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
                techt_inuse(tech_no) = techlevX(tech_no)*10 + 4;
                t_timeout = rnz(1500);
		break;	
	    case T_DISARM:
		if (!uwep) {
	    		You("aren't wielding a weapon!");
	    		return(0);
		}

	    	if (P_SKILL(weapon_type(uwep)) == P_NONE) {
	    		You("aren't wielding a proper weapon!");
	    		return(0);
	    	}
	    	if ((P_SKILL(weapon_type(uwep)) < P_SKILLED) || PlayerCannotUseSkills || (Blind)) {
	    		You("aren't capable of doing this!");
	    		return(0);
	    	}

		if (uwep && uwep->spe < 1) {
			Your("weapon isn't strong enough for that!");
			/* could be unidentified, in which case we give a timeout --Amy */
			if (!(uwep->known)) t_timeout = rnz(500);
	    		break;
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
		if (!isok(u.ux + u.dx, u.uy + u.dy)) {
			pline("Invalid target location.");
			return 0;
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
				|| (obj->oinvis && !See_invisible) || obj->oinvisreal
				) {
	    		You_cant("see %s weapon!", s_suffix(mon_nam(mtmp)));
	    		return(0);
		}
		num = ((rn2(techlevX(tech_no) + 15)) 
			* (P_SKILL(weapon_type(uwep)) - P_SKILLED + 1)) / 10;

		if (uarmf && uarmf->oartifact == ART_CORINA_S_SNOWY_TREAD) {
			if (num < 0) num = 0;
			num += 20;
		}

		You("attempt to disarm %s...",mon_nam(mtmp));
		/* WAC can't yank out cursed items */
                if (num > 0 && (!Fumbling || !rn2(10)) && !obj->cursed) {
		    int roll;
		    obj_extract_self(obj);
		    possibly_unwield(mtmp, FALSE);
		    setmnotwielded(mtmp, obj);
		    roll = rn2(num + 1);
		    if (roll > 3) roll = 3;

		    if (obj && obj->mstartinventB && !(obj->oartifact) && !(obj->fakeartifact && timebasedlowerchance()) && (!rn2(4) || (rn2(100) < u.equipmentremovechance) || !timebasedlowerchance() ) ) {
				uwep->spe--;
				You("vaporize %s %s!", s_suffix(mon_nam(mtmp)), xname(obj));
				delobj(obj);
	          		t_timeout = rnz(5000);
				break;
		    }
		    if (obj && obj->mstartinventC && !(obj->oartifact) && !(obj->fakeartifact && !rn2(10)) && rn2(10) ) {
				uwep->spe--;
				You("vaporize %s %s!", s_suffix(mon_nam(mtmp)), xname(obj));
				delobj(obj);
	          		t_timeout = rnz(5000);
				break;
		    }

		    if (obj && obj->mstartinventE && !(obj->oartifact) && !(obj->fakeartifact && !rn2(20)) && rn2(20) ) {
				uwep->spe--;
				You("vaporize %s %s!", s_suffix(mon_nam(mtmp)), xname(obj));
				delobj(obj);
	          		t_timeout = rnz(5000);
				break;
		    }

		    if (obj && obj->mstartinventD && !(obj->oartifact) && !(obj->fakeartifact && !rn2(4)) && rn2(4) ) {
				uwep->spe--;
				You("vaporize %s %s!", s_suffix(mon_nam(mtmp)), xname(obj));
				delobj(obj);
	          		t_timeout = rnz(5000);
				break;
		    }

		    if (obj && obj->mstartinventX) {
				uwep->spe--;
				You("vaporize %s %s!", s_suffix(mon_nam(mtmp)), xname(obj));
				delobj(obj);
	          		t_timeout = rnz(5000);
				break;
		    }

		    switch (roll) {
			case 2:
			default:
			    /* to floor near you */
			    You("knock %s %s to the %s!",
				s_suffix(mon_nam(mtmp)), xname(obj), surface(u.ux, u.uy));
			    if (obj->otyp == CRYSKNIFE &&
				    (!obj->oerodeproof || !rn2(10))) {
				obj->otyp = WORM_TOOTH;
				obj->oerodeproof = 0;
			    }
			    uwep->spe--;
			    place_object(obj, u.ux, u.uy);
			    stackobj(obj);
			    break;
			case 3:
			    /* right into your inventory */
			    uwep->spe--;
			    You("snatch %s %s!", s_suffix(mon_nam(mtmp)), xname(obj));
			    if (obj->otyp == CORPSE &&
				    touch_petrifies(&mons[obj->corpsenm]) &&
				    (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
				    !(poly_when_stoned(youmonst.data) &&
					polymon(PM_STONE_GOLEM))) {
				static char kbuf[BUFSZ];

				sprintf(kbuf, "snatching a petrifying corpse");
				pline("Snatching this type of corpse is a fatal mistake.");
				instapetrify(kbuf);
			    }
			    obj = hold_another_object(obj, "You drop %s!",
				    doname(obj), (const char *)0);
			    break;
		    }
		} else if (mtmp->mcanmove && !mtmp->msleeping)
		    pline("%s evades your attack.", Monnam(mtmp));
		else
		    You("fail to dislodge %s %s.", s_suffix(mon_nam(mtmp)), xname(obj));
		wakeup(mtmp);
		if (!mtmp->mcanmove && !rn2(10)) {
		    mtmp->masleep = 0;
		    mtmp->mcanmove = 1;
		    mtmp->mfrozen = 0;
		}
                t_timeout = rnz(5000);
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
		for(i = 0; (i  <= ((techlevX(tech_no) / 8) + 1) 
			&& isok(u.ux + (i*u.dx), u.uy + (i*u.dy))); i++) {
		    mtmp = m_at(u.ux + (i*u.dx), u.uy + (i*u.dy));
		    if (mtmp && canseemon(mtmp)) break;
		}
		if (!mtmp || !canseemon(mtmp)) {
			You("fail to make eye contact with anything!");
			/* regular SLASH'EM would allow you to scan for the presence of monsters until there is one...
			 * But this isn't regular SLASH'EM. If you use the technique, you get a timeout, period. --Amy */
               	t_timeout = rnz(75);
			break;
		}
		You("stare at %s.", mon_nam(mtmp));
		if (!haseyes(mtmp->data))
			pline("..but %s has no eyes!", mon_nam(mtmp));
		else if (!mtmp->mcansee)
			pline("..but %s cannot see you!", mon_nam(mtmp));
		else if ((rn2(6) + rn2(6) + (techlevX(tech_no) - mtmp->m_lev)) > rnd(10)) {

			if (!rn2(10)) goto breakstare;
			if (mtmp->mfrenzied && !rn2(5)) goto breakstare;

			You("dazzle %s!", mon_nam(mtmp));
			mtmp->mcanmove = 0;
			mtmp->mfrozen = rnd(10);
		} else {
breakstare:
			pline("%s breaks the stare!", Monnam(mtmp));
			if (!mtmp->mtame && !mtmp->mpeaceful && !mtmp->mfrenzied && !rn2(5)) {
				mtmp->mfrenzied = 1;
				pline("Now %s is really angry at you.", mon_nam(mtmp));
			}
		}
		t_timeout = rnz(750);
	    	break;
	    case T_BLITZ:
	    	if (!canuseunarmedtechs()) {
			You("can't do this while wielding a weapon!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    		return(0);
	    	} else if (uarms) {
			You("can't do this while holding a shield!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    		return(0);
	    	}
	    	if (!doblitz()) return (0);		
		u.combostrike = 0;
		u.comboactive = FALSE;
                t_timeout = rnz(5000);
	    	break;
            case T_PUMMEL:
	    	if (!canuseunarmedtechs()) {
			You("can't do this while wielding a weapon!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    		return(0);
	    	} else if (uarms) {
			You("can't do this while holding a shield!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    		return(0);
	    	}
		if (!getdir((char *)0)) return(0);
		if (!u.dx && !u.dy) {
			You("flex your muscles.");
			return(0);
		}
		if (!isok(u.ux + u.dx, u.uy + u.dy)) {
			pline("Invalid target location.");
			return 0;
		}
		    if (!blitz_pummel()) return(0);
		    t_timeout = rnz(2000);
		break;
            case T_G_SLAM:
	    	if (!canuseunarmedtechs()) {
			You("can't do this while wielding a weapon!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    		return(0);
	    	} else if (uarms) {
			You("can't do this while holding a shield!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    		return(0);
	    	}
		if (!getdir((char *)0)) return(0);
		if (!u.dx && !u.dy) {
			You("flex your muscles.");
			return(0);
		}
		if (!isok(u.ux + u.dx, u.uy + u.dy)) {
			pline("Invalid target location.");
			return 0;
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
                t_timeout = rnz(500);
		break;
            case T_POWER_SURGE:
            	if (!blitz_power_surge()) return(0);
		t_timeout = rnz(issoviet ? 1500 : 5000);
		if (issoviet) pline("Sovetskaya nichego ne znayet o balansirovaniya ne ponimayet i poetomu khochet etu tekhniku, kotoraya uzhe slishkom sil'na, chtoby byt' yeshche sil'neye.");
		break;            	
            case T_SPIRIT_BOMB:
	    	if (!canuseunarmedtechs()) {
			You("can't do this while wielding a weapon!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    		return(0);
	    	} else if (uarms) {
			You("can't do this while holding a shield!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
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

		use_skill(P_DEVICES,1);
		if (Race_if(PM_FAWN)) {
			use_skill(P_DEVICES,1);
		}
		if (Race_if(PM_SATRE)) {
			use_skill(P_DEVICES,1);
			use_skill(P_DEVICES,1);
		}

		pline("Using your medical kit, you draw off a phial of your blood.");
		/* Amy edit: let's make this much more useful by having the level drain only occur 1 out of 3 times. */
		if (!rn2(3)) {losexp("drawing blood", TRUE, FALSE);
		if (u.uexp > 0)
		    u.uexp = newuexp(u.ulevel - 1);
		}
		otmp = mksobj(POT_VAMPIRE_BLOOD, FALSE, FALSE, FALSE);
		if (otmp) {
			otmp->cursed = obj->cursed;
			otmp->blessed = obj->blessed;
			(void) hold_another_object(otmp,
				"You fill, but have to drop, %s!", doname(otmp),
				(const char *)0);
		}
		t_timeout = rnz(1500);
		break;
	    case T_JEDI_JUMP:
		if (u.uen < 25){
			You("can't channel the force around you. Jedi jumps require 25 points of mana!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return(0);
		}
		if (!jump((techlevX(tech_no)/5)+1)) return 0;
		u.uen -= 25;
		t_timeout = rnz(500);
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
		pline("%s used Poke Ball!", playeraliasname);

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
				t_timeout = rnz(3000);
				}

			else if ( (mtmp->m_lev > (2 * techlevX(tech_no)) || rn2(4) ) && mtmp->m_lev > techlevX(tech_no) && caught == 0 && ( (!is_pokemon(mtmp->data) && (!mtmp->egotype_pokemon) ) || rn2(2) ) )
				{
				pline("You missed the Pokemon!");
				}

			else if (caught == 0) /* other monster that can be caught */

				{
				/* If catchrate is a higher numeric value, the chance of catching the monster is lower. */

				catchrate = (60 + mtmp->m_lev - techlevX(tech_no));
				if (!rn2(4)) catchrate -= ACURR(A_CHA);
				if (!rn2(4)) catchrate -= techlevX(tech_no);
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
					t_timeout = rnz(3000);

					}
				}

			} /* monster is catchable loop */
		    } /* for loop */

		if (caught == 0) pline("The ball expodes in midair!");
/* This is an intentional typo, derived from another roguelike. Do you know which one it is? --Amy*/

		t_timeout = rnz(3000);
		break;
	    case T_SUMMON_TEAM_ANT:

		{
		int maxchance = techlevX(tech_no);
		if (maxchance > 50) maxchance = 50;
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
		if (maxchance < rnd(75)) caughtY++;
		}

		/* A high level Insectoid character can create quite the army of insects sometimes. --Amy */

		}

		t_timeout = rnz(5000);
		break;

	    case T_DOUBLE_TROUBLE:	/* inspired by Khor */

		{
		int maxchance = techlevX(tech_no);
		if (maxchance > 45) maxchance = 45;

		pline("Double Trouble...");

		int familiardone;
		familiardone = 0;

		if (!rn2(5)) mtmp = makemon(&mons[urace.malenum], u.ux, u.uy, NO_MM_FLAGS);
		else if (Upolyd) mtmp = makemon(&mons[u.umonnum], u.ux, u.uy, NO_MM_FLAGS);
		else mtmp = makemon(&mons[urole.malenum], u.ux, u.uy, NO_MM_FLAGS);
		if (mtmp) (void) tamedog(mtmp, (struct obj *) 0, TRUE);

		if ((techlevX(tech_no)) < rnd(50)) familiardone = 1;
		else pline("Sent in some familiars too.");

		while (familiardone == 0) {

	 	      mtmp = makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
			if (mtmp) (void) tamedog(mtmp, (struct obj *) 0, TRUE);

			if (maxchance < rnd(50)) familiardone = 1;
		}

		}

		t_timeout = rnz(10000);
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
		caughtX = 0;
		pline("You strike a sexy pose with your heels!");
		if (uarmf && uarmf->oartifact == ART_PRECURSOR_TO_THE___) adjattrib(A_CHA, 1, -1, TRUE);

		    for (k = -1; k <= 1; k++) for(l = -1; l <= 1; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;
			if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) && !is_infrastructure_monster(mtmp3) && mtmp3->mtame == 0 && mtmp3->isminion == 0 && mtmp3->isgyp == 0
			&& mtmp3->mnum != quest_info(MS_NEMESIS) && !(mtmp3->data->geno & G_UNIQ) && caughtX == 0)

				/* gotta write a huge function for this now --Amy */

			{

				if ( humanoid(mtmp3->data) || mtmp3->data->mlet == S_HUMAN) {
			      /*maybe_tameX(mtmp3);*/
				pline("%s is charmed, and wants to be your friend!", mon_nam(mtmp3));
				(void) tamedog(mtmp3, (struct obj *) 0, TRUE);
				if (techlevX(tech_no) < rnd(100)) caughtX++;
				t_timeout = rnz(5000);
				}

			else pline("%s is too stupid to fully appreciate you!", mon_nam(mtmp3));

			} /* monster is catchable loop */
		    } /* for loop */

		/* signature technique of some roles, but too OP for others --Amy */
		if (Role_if(PM_TRANSVESTITE) || Role_if(PM_TOPMODEL) || Role_if(PM_TRANSSYLVANIAN) || Role_if(PM_FAILED_EXISTENCE)) t_timeout = rnz(5000);
		else t_timeout = rnz(25000);
		break;

	    case T_WORLD_FALL:

		You("scream \"EYGOORTS-TOGAAL, JEZEHH!\"");
		/* Actually, it's "To win the game you must kill me, John Romero" recorded backwards.
		 * When I was little, I always thought it said "Eygoorts-togaal, jezehh". --Amy */
		{
			int wflvl = ((techlevX(tech_no)) / 2);
			if (wflvl < 1) wflvl = 1;

			register struct monst *mtmp, *mtmp2;

			num = 0;

			for (mtmp = fmon; mtmp; mtmp = mtmp2) {
				mtmp2 = mtmp->nmon;
				if ( ((mtmp->m_lev < wflvl) || (!rn2(4) && mtmp->m_lev < (2 * wflvl))) && mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) ) {
					mondead(mtmp);
					num++;
					}
			}
			pline("Eliminated %d monster%s.", num, plur(num));
		}

		t_timeout = rnz(100000);
		break;

	    case T_CREATE_AMMO:

	    {
		boolean canbfg = FALSE;
		boolean canrocket = FALSE;
		boolean canshotty = FALSE;
		boolean canblaster = FALSE;

		int ammotype;
		ammotype = 1; /* bullets */
		if (Role_if(PM_DOOM_MARINE)) {

			if (techlevX(tech_no) >= 25) {

				canbfg = TRUE;
				canrocket = TRUE;
				canshotty = TRUE;
				canblaster = TRUE;

			}

			else if (techlevX(tech_no) >= 20) {

				canrocket = TRUE;
				canshotty = TRUE;
				canblaster = TRUE;
			}

			else if (techlevX(tech_no) >= 15) {

				canshotty = TRUE;
				canblaster = TRUE;
			}

			else if (techlevX(tech_no) >= 10) {

				canblaster = TRUE;
			}

		}

		if (Role_if(PM_SPACE_MARINE)) {
			canblaster = TRUE;
		}

		if (Race_if(PM_TURMENE)) {
			canshotty = TRUE;
		}

		if (!Role_if(PM_GRENADONIN)) {
			boolean haschosen = FALSE;
			pline("Pick an ammo type that you want to create. The prompt will loop until you actually make a choice.");
			while (!haschosen) {
				if (yn("Do you want to create pistol bullets?") == 'y') {
					haschosen = TRUE;
					ammotype = 1;
				}
				else if (yn("Do you want to create five-seven bullets?") == 'y') {
					haschosen = TRUE;
					ammotype = 16;
				}
				else if (yn("Do you want to create SMG bullets?") == 'y') {
					haschosen = TRUE;
					ammotype = 6;
				}
				else if (yn("Do you want to create rifle bullets?") == 'y') {
					haschosen = TRUE;
					ammotype = 7;
				}
				else if (yn("Do you want to create sniper bullets?") == 'y') {
					haschosen = TRUE;
					ammotype = 8;
				}
				else if (yn("Do you want to create MG bullets?") == 'y') {
					haschosen = TRUE;
					ammotype = 9;
				}
				else if (yn("Do you want to create assault rifle bullets?") == 'y') {
					haschosen = TRUE;
					ammotype = 10;
				}
				else if (canshotty && yn("Do you want to create shotgun shells?") == 'y') {
					haschosen = TRUE;
					ammotype = 3;
				}
				else if (canshotty && yn("Do you want to create auto shotgun shells?") == 'y') {
					haschosen = TRUE;
					ammotype = 11;
				}
				else if (canrocket && yn("Do you want to create rockets?") == 'y') {
					haschosen = TRUE;
					ammotype = 4;
				}
				else if (canbfg && yn("Do you want to create BFG ammo?") == 'y') {
					haschosen = TRUE;
					ammotype = 5;
				}
				else if (canblaster && yn("Do you want to create blaster bolts?") == 'y') {
					haschosen = TRUE;
					ammotype = 2;
				}
				else if (canblaster && yn("Do you want to create heavy blaster bolts?") == 'y') {
					haschosen = TRUE;
					ammotype = 12;
				}
				else if (canblaster && yn("Do you want to create raygun bolts?") == 'y') {
					haschosen = TRUE;
					ammotype = 13;
				}
				else if (canblaster && yn("Do you want to create laser beams?") == 'y') {
					haschosen = TRUE;
					ammotype = 14;
				}
				else if (canblaster && yn("Do you want to create radios?") == 'y') {
					haschosen = TRUE;
					ammotype = 15;
				}
				else if (canblaster && yn("Do you want to create carcosan bolts?") == 'y') {
					haschosen = TRUE;
					ammotype = 17;
				}
			}

		}

		if (Role_if(PM_GRENADONIN)) ammotype = 666; /* grenades */

	    You("make some ammo for your gun.");

		struct obj *uammo;

		if (ammotype == 666) uammo = mksobj(rn2(2) ? GAS_GRENADE : FRAG_GRENADE, TRUE, FALSE, FALSE);
		else if (ammotype == 5) uammo = mksobj(BFG_AMMO, TRUE, FALSE, FALSE);
		else if (ammotype == 4) uammo = mksobj(ROCKET, TRUE, FALSE, FALSE);
		else if (ammotype == 3) uammo = mksobj(SHOTGUN_SHELL, TRUE, FALSE, FALSE);
		else if (ammotype == 2) uammo = mksobj(BLASTER_BOLT, TRUE, FALSE, FALSE);
		else if (ammotype == 6) uammo = mksobj(SMG_BULLET, TRUE, FALSE, FALSE);
		else if (ammotype == 7) uammo = mksobj(RIFLE_BULLET, TRUE, FALSE, FALSE);
		else if (ammotype == 8) uammo = mksobj(SNIPER_BULLET, TRUE, FALSE, FALSE);
		else if (ammotype == 9) uammo = mksobj(MG_BULLET, TRUE, FALSE, FALSE);
		else if (ammotype == 10) uammo = mksobj(ASSAULT_RIFLE_BULLET, TRUE, FALSE, FALSE);
		else if (ammotype == 11) uammo = mksobj(AUTO_SHOTGUN_SHELL, TRUE, FALSE, FALSE);
		else if (ammotype == 12) uammo = mksobj(HEAVY_BLASTER_BOLT, TRUE, FALSE, FALSE);
		else if (ammotype == 13) uammo = mksobj(RAYGUN_BOLT, TRUE, FALSE, FALSE);
		else if (ammotype == 14) uammo = mksobj(LASER_BEAM, TRUE, FALSE, FALSE);
		else if (ammotype == 15) uammo = mksobj(RADIO, TRUE, FALSE, FALSE);
		else if (ammotype == 16) uammo = mksobj(FIVE_SEVEN_BULLET, TRUE, FALSE, FALSE);
		else if (ammotype == 17) uammo = mksobj(CARCOSAN_BOLT, TRUE, FALSE, FALSE);
		else uammo = mksobj(PISTOL_BULLET, TRUE, FALSE, FALSE);
		if (uammo) {
			uammo->quan = techlevX(tech_no);
			/* gunner really specializes in ranged weapons, so needs a big bonus --Amy */
			if (Role_if(PM_GUNNER)) uammo->quan *= 1 + rnd(2);
			if (uarmf && uarmf->oartifact == ART_ZERDROY_GUNNING) {
				uammo->quan *= 3;
				if (uarmf) curse(uarmf);
			}
			if (uammo && ammotype == 1 && uarmh && uarmh->oartifact == ART_TURKISH_EMPIRE) uammo->quan *= 2;
			if (uammo && ammotype == 6 && uarmh && uarmh->oartifact == ART_TURKISH_EMPIRE) uammo->quan *= 2;
			if (uammo && ammotype == 7 && uarmh && uarmh->oartifact == ART_TURKISH_EMPIRE) uammo->quan *= 2;
			if (uammo && ammotype == 8 && uarmh && uarmh->oartifact == ART_TURKISH_EMPIRE) uammo->quan *= 2;
			if (uammo && ammotype == 9 && uarmh && uarmh->oartifact == ART_TURKISH_EMPIRE) uammo->quan *= 2;
			if (uammo && ammotype == 10 && uarmh && uarmh->oartifact == ART_TURKISH_EMPIRE) uammo->quan *= 2;
			if (uammo && ammotype == 16 && uarmh && uarmh->oartifact == ART_TURKISH_EMPIRE) uammo->quan *= 2;
			if (uarmc && uarmc->oartifact == ART_ARABELLA_S_WEAPON_STORAGE) uammo->quan *= 2;
			if (ammotype == 5) uammo->quan *= 4;
			if (ammotype == 4) uammo->quan /= 10;
			if (uammo->quan < 0) uammo->quan = 1; /* fail safe */
			uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
			uammo->owt = weight(uammo);
			dropy(uammo);
			stackobj(uammo);
		}

		if (Role_if(PM_GRENADONIN) && u.ulevel >= 10) {

			if (!u.grenadoninlauncher && !rn2(5)) {
				u.grenadoninlauncher = TRUE;
				uammo = mksobj(GRENADE_LAUNCHER, TRUE, FALSE, FALSE);
				if (uammo) {
					uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
					dropy(uammo);
					stackobj(uammo);
					pline("There's your grenade launcher!");
				}
			}

			if (!rn2(10)) {
				uammo = mksobj(STICK_OF_DYNAMITE, TRUE, FALSE, FALSE);
				if (uammo) {
					uammo->quan = techlevX(tech_no) - 9;
					if (uammo->quan < 1) uammo->quan = 1;
					if (uarmc && uarmc->oartifact == ART_ARABELLA_S_WEAPON_STORAGE) uammo->quan *= 2;
					uammo->quan /= 3;
					if (uammo->quan < 1) uammo->quan = 1; /* fail safe */
					uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
					uammo->owt = weight(uammo);
					dropy(uammo);
					stackobj(uammo);
				}

			}

		}

		if (uarmh && uarmh->oartifact == ART_TURKISH_EMPIRE) {
			uammo = mksobj(ROCKET, TRUE, FALSE, FALSE);
			if (uammo) {
				uammo->quan = techlevX(tech_no);
				if (uarmc && uarmc->oartifact == ART_ARABELLA_S_WEAPON_STORAGE) uammo->quan *= 2;
				uammo->quan /= 10;
				if (uammo->quan < 1) uammo->quan = 1; /* fail safe */
				uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
				uammo->owt = weight(uammo);
				dropy(uammo);
				stackobj(uammo);
			}

		}

		if (Role_if(PM_DOOM_MARINE) || Role_if(PM_GUNNER)) t_timeout = rnz(500);
		else t_timeout = rnz(2500);
	 	break;

	    }

	    case T_EGG_BOMB:

	    You("create some stoning grenades.");

		int caughtZ;
		caughtZ = 0;
		struct obj *uegg;

		while (caughtZ == 0) {

			uegg = mksobj(EGG, FALSE, FALSE, FALSE);
			if (uegg) {
				uegg->spe = 0;
				uegg->quan = 1;
				uegg->owt = weight(uegg);
				uegg->corpsenm = egg_type_from_parent(PM_COCKATRICE, FALSE);
				uegg->known = uegg->dknown = 1;
				attach_egg_hatch_timeout(uegg);
				kill_egg(uegg); /* make sure they're stale --Amy */
				dropy(uegg);
				stackobj(uegg);
			}
			if (techlevX(tech_no) < rnd(500)) caughtZ++;

		}

	      t_timeout = rnz(10000);
	      break;

	    case T_BOOZE:

		{
		int maxchance = techlevX(tech_no);
		if (maxchance > 50) maxchance = 50;

		You("procure some refreshing drinks.");

		make_confused(HConfusion + d(3,8), FALSE);
		healup(Role_if(PM_DRUNK) ? rnz(20 + u.ulevel) : 1, 0, FALSE, FALSE);
		u.uhunger += 20;
		if (Race_if(PM_CLOCKWORK_AUTOMATON)) u.uhunger += 200;
		if (Race_if(PM_RUSMOT)) u.uhunger += 100;
		if (Role_if(PM_DRUNK)) u.uhunger += 100;
		newuhs(FALSE);
		exercise(A_WIS, FALSE);

		int caughtW;
		caughtW = 0;
		struct obj *udrink;

		while (caughtW == 0) {

			udrink = mksobj(POT_BOOZE, TRUE, FALSE, FALSE);
			if (udrink) {
				udrink->known = udrink->dknown = 1;
				udrink->finalcancel = 1; /* goddamn it, these aren't supposed to be free holy water... */
				dropy(udrink);
				stackobj(udrink);
			}
			if (maxchance < rnd(75)) caughtW++;
			if (!rn2(10) && caughtW > 0) caughtW = 0;

		}

		}

	      t_timeout = rnz(2000);
	      break;

	    case T_IRON_SKIN:
		num = 9 + techlevX(tech_no);
	    	techt_inuse(tech_no) = num + 1;
		pline("Your skin becomes harder.");

	      t_timeout = rnz(2000);
	      break;

	    case T_POLYFORM:

		pline("You feel polyform.");
	    	techt_inuse(tech_no) = 1;
		polyself(FALSE);
	    	techt_inuse(tech_no) = 0;
	      t_timeout = rnz(10000);
	      break;

	    case T_CONCENTRATING:
		num = 1 + (techlevX(tech_no) / 2);
	    	techt_inuse(tech_no) = num + 1;

		pline("You start concentrating.");
	      t_timeout = rnz(2500);
	      break;

	    case T_SUMMON_PET:
		pline("You summon a pet.");

		if (Role_if(PM_BUTT_LOVER)) {
			register struct monst *buttpet;

			struct permonst *pm = 0;
			int attempts = 0;

buttpetmarker:
			do {
				pm = rndmonst();
				attempts++;
				if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!pm || (pm && !(pm->msound == MS_FART_NORMAL) && !(pm->msound == MS_FART_LOUD) && !(pm->msound == MS_FART_QUIET))) && attempts < 50000);

			if (!pm && rn2(50) ) {
				attempts = 0;
				goto buttpetmarker;
			}
			if (pm && !(pm->msound == MS_FART_NORMAL) && !(pm->msound == MS_FART_LOUD) && !(pm->msound == MS_FART_QUIET) && rn2(50) ) {
				attempts = 0;
				goto buttpetmarker;
			}

			if (pm) (buttpet = makemon(pm, u.ux, u.uy, NO_MM_FLAGS));

			if (buttpet) {
				tamedog(buttpet, (struct obj *) 0, TRUE);
				pline("Suddenly, you gain a new sexy pet!");
			}

			u.mondiffhack = 0;

		} else {
			(void) make_familiar((struct obj *)0, u.ux, u.uy, FALSE, FALSE);
		}

	      t_timeout = rnz(10000);
	      break;

	    case T_DOUBLE_THROWNAGE:

            Your("%s %s become blurs as they reach for your throwing weapons!", uarmg ? "gloved" : "bare", makeplural(body_part(HAND)));
            techt_inuse(tech_no) = rnd((int) (techlevX(tech_no)/6 + 1)) + 2;
            t_timeout = rnz(1500);

	      break;

	    case T_SHIELD_BASH:
		if (!uarms) {
			pline("You aren't wearing a shield!");
			return(0);
		}
		num = 1 + techlevX(tech_no);
	    	techt_inuse(tech_no) = num + 1;
		pline("You ready your shield as an additional weapon.");

	      t_timeout = rnz(2000);

	      break;

	    case T_RECHARGE:

		if (CannotSelectItemsInPrompts) break;
chargingchoice:
	    	{
		struct obj *otmpC = getobj(recharge_type, "charge");
		if (!otmpC) {
			if (yn("Really exit with no object selected?") == 'y')
				pline("You just wasted the opportunity to charge your items.");
			else goto chargingchoice;
			return(0);
		}
		recharge(otmpC, 0);

	      t_timeout = rnz(15000);
	      break;
		}

	    case T_SPIRITUALITY_CHECK:

		if (Race_if(PM_MACTHEIST)) {
			pline("As an atheist, you cannot use such a technique.");
			return 0;
		}

		if (can_pray(FALSE) && !u.ugangr) pline("You can safely pray!");
		else {
			pline("You can not safely pray.");
			if (u.ugangr) pline("The gods are angry with you.");
			if (u.ublesscnt > 0) pline("Your prayer timeout isn't zero.");
			if ((int)Luck < 0) pline("Your luck is negative.");
			if (u.ualign.record < 0) pline("Your alignment record is negative.");
		}
		u.uconduct.gnostic++;	/* you just communicated with your god */
		if (Race_if(PM_MAGYAR)) {
			You_feel("bad about breaking the atheist conduct.");
			badeffect();
		}
		use_skill(P_SPIRITUALITY, Role_if(PM_PRIEST) ? 25 : 5);

	      t_timeout = rnz(2000);
	      break;

	    case T_BLOOD_RITUAL:

		if (!(carrying(ATHAME) || carrying(MERCURIAL_ATHAME)) ) {
			pline("You need an athame to conduct the ritual!");
			return(0);
		}
		if (u.uhpmax < 201) {
			pline("You don't have enough health to conduct the ritual!");
			return(0);
		}
		if (Upolyd && u.mhmax < 201) {
			pline("You don't have enough health to conduct the ritual!");
			return(0);
		}
		if (u.uenmax < 201) {
			pline("You don't have enough mana to conduct the ritual!");
			return(0);
		}
		pline("Using your athame, you mutilate your own body. Don't ask me, I know nothing about those heretical rituals, apart from the fact they exist...");

		u.uhpmax -= rno(200);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (Upolyd) {
			u.mhmax -= rno(200);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		}
		u.uenmax -= rno(200);
		if (u.uen > u.uenmax) u.uen = u.uenmax;

		ABASE(A_STR) -= rno(2);
		if (ABASE(A_STR) < ATTRABSMIN(A_STR)) ABASE(A_STR) = ATTRABSMIN(A_STR);
		AMAX(A_STR) = ABASE(A_STR);

		ABASE(A_DEX) -= rno(2);
		if (ABASE(A_DEX) < ATTRABSMIN(A_DEX)) ABASE(A_DEX) = ATTRABSMIN(A_DEX);
		AMAX(A_DEX) = ABASE(A_DEX);

		ABASE(A_INT) -= rno(2);
		if (ABASE(A_INT) < ATTRABSMIN(A_INT)) ABASE(A_INT) = ATTRABSMIN(A_INT);
		AMAX(A_INT) = ABASE(A_INT);

		ABASE(A_WIS) -= rno(2);
		if (ABASE(A_WIS) < ATTRABSMIN(A_WIS)) ABASE(A_WIS) = ATTRABSMIN(A_WIS);
		AMAX(A_WIS) = ABASE(A_WIS);

		ABASE(A_CON) -= rno(2);
		if (ABASE(A_CON) < ATTRABSMIN(A_CON)) ABASE(A_CON) = ATTRABSMIN(A_CON);
		AMAX(A_CON) = ABASE(A_CON);

		ABASE(A_CHA) -= rno(2);
		if (ABASE(A_CHA) < ATTRABSMIN(A_CHA)) ABASE(A_CHA) = ATTRABSMIN(A_CHA);
		AMAX(A_CHA) = ABASE(A_CHA);

		if (!rn2(4)) makewish(TRUE);
		else othergreateffect();

	      t_timeout = rnz(25000);
	      break;

	    case T_ENT_S_POTION:
		pline("You feel very good.");
		healup(techlevX(tech_no) * 10, 0, FALSE, FALSE);
		make_blinded(0L,FALSE);
	      make_stunned(0L,TRUE);
	      make_confused(0L,TRUE);
	      (void) make_hallucinated(0L,FALSE,0L);
	      make_numbed(0L,TRUE);
	      make_feared(0L,TRUE);
	      make_frozen(0L,TRUE);
	      make_burned(0L,TRUE);
	      make_dimmed(0L,TRUE);
	      u.uhunger += techlevX(tech_no) * 10;

	      t_timeout = rnz(10000);
	      break;

	    case T_LUCKY_GAMBLE:

		if (rn2(2)) {
			change_luck(1);
			You_feel("lucky.");
		} else {
			change_luck(-1);
			You_feel("unlucky.");
		}

	      t_timeout = rnz(2000);
	      break;

	    case T_DECONTAMINATE:
		/* divide player's contamination by half, but don't reduce it by more than 1000 --Amy */

		if (!u.contamination) {
		      t_timeout = rnz(200);
			You("were not contaminated to begin with.");
			pline("Nothing happens.");
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
			break;
		}

		pline("Your contamination is cured greatly.");

		if (u.contamination < 2000) decontaminate(u.contamination / 2);
		else decontaminate(1000);
	      t_timeout = rnz(4000);

		break;

	    case T_WONDERSPELL:
		wonderspell(-1);
	      t_timeout = rnz(10000);
		break;

	    case T_SWAP_WEAPON:
		if (u.twoweap) {
			pline("Sorry, you can only swap your secondary weapon while not dual-wielding.");
			return(0);
		}
		swaptech();
	      t_timeout = rnz(500);
		break;

	    case T_RESET_TECHNIQUE:
		{
			int numtechs;
			pline("Choose a technique to reset. The prompt will loop until you actually make a choice. You can also opt to reset no technique at all, in which case there will be no timeout.");
resetretrying:
			for (numtechs = 0; numtechs < MAXTECH && techid(numtechs) != NO_TECH; numtechs++) {
				if (techid(numtechs) == T_RESET_TECHNIQUE) continue;
				if (techtout(numtechs) > 0) {
					pline("Your %s technique is currently on timeout.", techname(numtechs));
					if (yn("Make this technique usable again?") == 'y') {
						techtout(numtechs) = 0;
						pline("The timeout on your %s technique has been set to zero.", techname(numtechs));
						t_timeout = rnz(Race_if(PM_DEVELOPER) ? 50000 : 200000);
						goto resettechdone;
					}
				}
			}
			pline("If you don't feel like resetting a technique right now, you can choose to reset none at all.");
			if (yn("Do you want to reset no technique at all?") == 'y') {
				goto resettechdone;
			}
			goto resetretrying;

		}
resettechdone:
		break;

	    case T_SILENT_OCEAN:

		{

		pline("The ocean becomes silent. You can freely swim in the water without your equipment becoming wet, and are protected from wrap attacks, but you're also silenced.");

		register struct monst *nexusmon, *nextmon;
		register int silentoceandist = 60 + (techlevX(tech_no) * 3);

		for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			nextmon = nexusmon->nmon;
			if (DEADMONSTER(nexusmon)) continue;
			if (resist(nexusmon, SPBOOK_CLASS, 0, NOTELL)) continue;
			if ((distu(nexusmon->mx, nexusmon->my)) > silentoceandist) continue;
			nexusmon->mcan = 1; /* don't cancel egotypes */
		}

		num = 10 + (techlevX(tech_no) * 5);
	    	techt_inuse(tech_no) = num + 1;
		t_timeout = rnz(5000);

		}

		break;

	    case T_GLOWHORN:
		pline("Suddenly, a glowing unicorn horn appears above your %s!", body_part(HEAD));
		num = 10 + (techlevX(tech_no) / 2);
	    	techt_inuse(tech_no) = num + 1;
		t_timeout = rnz(2000);
		break;

		case T_MILDEN_CURSE:

			if (CannotSelectItemsInPrompts) break;

			allowall[0] = ALL_CLASSES; allowall[1] = '\0';
		
			if ( !(obj = getobj(allowall, "milden"))) return(0);
			if (!obj->cursed) {
				if (obj->bknown) {
					pline("That object wasn't cursed to begin with.");
					return 0;
				} else {
					obj->bknown = TRUE;
					pline("It seems that object wasn't cursed to begin with.");
				}
			} else if (obj->cursed && !obj->hvycurse && !obj->prmcurse && !obj->morgcurse && !obj->evilcurse && !obj->bbrcurse && !obj->stckcurse ) {
				if (obj->bknown) {
					pline("That object didn't have any kind of curse that could be mildened. It is still cursed.");
					return 0;
				} else {
					obj->bknown = TRUE;
					pline("It seems that object didn't have any kind of curse that could be mildened. It is still cursed.");
				}
			} else if (obj->cursed && !obj->morgcurse && !obj->evilcurse && !obj->bbrcurse) {
				obj->bknown = TRUE;

				if (stack_too_big(obj)) {
					pline("The stack was too big! Nothing happens.");
					t_timeout = rnz(7500);
					break;
				}

				if (obj->stckcurse) {
					obj->stckcurse = FALSE;
					pline("The sticky curse is broken!");
				}
				if (obj->prmcurse) {
					obj->prmcurse = obj->hvycurse = FALSE;
					pline("The prime curse is broken!");
				}
				if (obj->hvycurse) {
					obj->hvycurse = FALSE;
					pline("The heavy curse is broken!");
				}
			} else if (obj->cursed && (obj->morgcurse || obj->evilcurse || obj->bbrcurse)) {
				obj->bknown = TRUE;

				if (stack_too_big(obj)) {
					pline("The stack was too big! Nothing happens.");
					t_timeout = rnz(7500);
					break;
				}

				if (rn2(10)) {
					pline("Unfortunately, you failed to break the curse.");
					t_timeout = rnz(7500);
					break;
				}
				if (obj->stckcurse) {
					obj->stckcurse = FALSE;
					pline("The sticky curse is broken!");
				}
				if (obj->morgcurse) {
					obj->morgcurse = obj->prmcurse = obj->hvycurse = FALSE;
					pline("The ancient morgothian curse is broken!");
				}
				if (obj->evilcurse) {
					obj->evilcurse = obj->prmcurse = obj->hvycurse = FALSE;
					pline("The topi ylinen curse is broken!");
				}
				if (obj->bbrcurse) {
					obj->bbrcurse = obj->prmcurse = obj->hvycurse = FALSE;
					pline("The black breath curse is broken!");
				}
			}
			t_timeout = rnz(7500);
		break;

		case T_FORCE_FIELD:

			pline("Force field activated!");
			num = 50 + (techlevX(tech_no) * 3);
			if (Race_if(PM_PLAYER_ATLANTEAN)) num *= 2;
		    	techt_inuse(tech_no) = num + 1;
			t_timeout = rnz(4000);
			break;

		case T_POINTINESS:

			if (!uwep) {
				pline("That doesn't work without a weapon!");
				return 0;
			}
			if (uwep && weapon_type(uwep) != P_POLEARMS && weapon_type(uwep) != P_LANCE) {
				pline("That only works if your wielded weapon is a polearm or lance, and currently it's not!");
				return 0;
			}

			if (uwep && uwep->spe < 0) {
				uwep->spe += rnd(abs(uwep->spe));
				if (uwep->spe > 0) uwep->spe = 0; /* fail safe */
				pline("Your stick seems slightly sharp now.");
			} else if (uwep && uwep->spe < 2) {
				uwep->spe++;
				pline("Your stick seems sharper now.");
			} else if (uwep && uwep->spe < 25 && !rn2(uwep->spe) ) {
				uwep->spe++;
				pline("Your stick seems very sharp now.");
			} else pline("Your stick seems as sharp as it was before.");

			t_timeout = rnz(7500);
			break;

		case T_BUG_SPRAY:
			{
			coord cc;

			pline("You may place a stinking cloud on the map.");
			pline("Where do you want to center the cloud?");
			cc.x = u.ux;
			cc.y = u.uy;
			if (getpos(&cc, TRUE, "the desired position") < 0) {
			    pline("%s", Never_mind);
				if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
					pline("Oh wait, actually I do mind...");
					badeffect();
				}
			    return 0;
			}
			if (!cansee(cc.x, cc.y) || distu(cc.x, cc.y) >= 32) {
			    You("smell rotten eggs.");
			    return 0;
			}
			if (!PlayerCannotUseSkills && P_SKILL(P_PADDLE) >= P_GRAND_MASTER)
				(void) create_gas_cloud(cc.x, cc.y, 4, 12);
			else
				(void) create_gas_cloud(cc.x, cc.y, 3, 8);

			}

			t_timeout = rnz(2000);
			break;

		case T_WHIRLSTAFF:

			if (!uwep) {
				pline("That doesn't work without a weapon!");
				return 0;
			}
			if (uwep && weapon_type(uwep) != P_QUARTERSTAFF) {
				pline("That only works if your wielded weapon is a quarterstaff, and currently it's not!");
				return 0;
			}

			You("start whirling like mad with your quarterstaff!");
			num = 20 + techlevX(tech_no);
		    	techt_inuse(tech_no) = num + 1;
			t_timeout = rnz(2500);
			break;

		case T_DELIBERATE_CURSE:

			if (CannotSelectItemsInPrompts) break;
			allowall[0] = ALL_CLASSES; allowall[1] = '\0';
			if ( !(obj = getobj(allowall, "curse"))) return(0);
			obj->bknown = TRUE;

			if (stack_too_big(obj)) {
				pline("The stack was too big! Nothing happens.");
				t_timeout = rnz(5000);
				break;
			}

			curse(obj);
			pline("Okay, that object is cursed now.");

			t_timeout = rnz(5000);
			break;

		case T_ACQUIRE_STEED:

			{

			if (u.uswallow) {
			    pline("Engulfing steeds can't see you, and therefore the taming attempt fails.");
				return 0;
			}

			int k, l, caughtX;
			struct monst *mtmp3;
			caughtX = 0;
			pline("You try to acquire a steed!");

			    for (k = -1; k <= 1; k++) for(l = -1; l <= 1; l++) {
				if (!isok(u.ux + k, u.uy + l)) continue;
				if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) && !is_infrastructure_monster(mtmp3) && mtmp3->mtame == 0 && mtmp3->isminion == 0 && mtmp3->isgyp == 0
			&& mtmp3->mnum != quest_info(MS_NEMESIS) && !(mtmp3->data->geno & G_UNIQ) && caughtX == 0)

				{

					if (mtmp3->data->mlet == S_QUADRUPED || mtmp3->data->mlet == S_UNICORN || mtmp3->data->mlet == S_ANGEL || mtmp3->data->mlet == S_CENTAUR || mtmp3->data->mlet == S_DRAGON || mtmp3->data->mlet == S_JABBERWOCK || mtmp3->data->mlet == S_ZOUTHERN) {
						if (!(resist(mtmp3, RING_CLASS, 0, NOTELL) && resist(mtmp3, RING_CLASS, 0, NOTELL) && resist(mtmp3, RING_CLASS, 0, NOTELL))) {
							pline("%s is successfully tamed!", mon_nam(mtmp3));
							(void) tamedog(mtmp3, (struct obj *) 0, TRUE);
							if (techlevX(tech_no) < rnd(100)) caughtX++;
							t_timeout = rnz(5000);
						} else if ( ((rnd(30 - ACURR(A_CHA))) < 4) && (!resist(mtmp3, RING_CLASS, 0, NOTELL) || !resist(mtmp3, RING_CLASS, 0, NOTELL) || !resist(mtmp3, RING_CLASS, 0, NOTELL)) ) {
							pline("%s is successfully tamed!", mon_nam(mtmp3));
							(void) tamedog(mtmp3, (struct obj *) 0, TRUE);
							if (techlevX(tech_no) < rnd(100)) caughtX++;
							t_timeout = rnz(5000);
						} else {
							pline("%s resists the taming attempt!", mon_nam(mtmp3));
						}
					} else pline("%s cannot be tamed by this method!", mon_nam(mtmp3));

				} /* monster is catchable loop */
			    } /* for loop */

			}
			t_timeout = rnz(5000);

			break;

		case T_SADDLING:

			{
				struct obj *usaddling;

				pline("A saddle is created!");
				usaddling = mksobj(rnd_class(LEATHER_SADDLE, INKA_SADDLE), TRUE, FALSE, FALSE);
				if (usaddling) {
					usaddling->quan = 1;
					usaddling->known = usaddling->dknown = usaddling->bknown = usaddling->rknown = 1;
					usaddling->owt = weight(usaddling);
					dropy(usaddling);
					stackobj(usaddling);
				}

			}

			t_timeout = rnz(10000);
			break;

		case T_SHOPPING_QUEEN:
			{
			register struct obj *heelses1, *heelses2, *heelses3, *heelses4, *heelses5, *heelses6, *heelses7, *heelses8, *heelses9, *heelses10;
			register int heelsamount;
			int heelidentity;
			int trycnt = 0;
			if (PlayerCannotUseSkills || (P_SKILL(P_HIGH_HEELS) <= P_EXPERT)) heelsamount = 1;
			else if (P_SKILL(P_HIGH_HEELS) <= P_MASTER) heelsamount = 3;
			else if (P_SKILL(P_HIGH_HEELS) <= P_GRAND_MASTER) heelsamount = 5;
			else if (P_SKILL(P_HIGH_HEELS) <= P_SUPREME_MASTER) heelsamount = 10;
			else heelsamount = 1; /* should never happen; fail safe */

			pline("Wow! You can get a pair of shoes from a list!");

			heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
			trycnt = 0;

			while (trycnt < 5000 && !ishighheeledb(heelidentity) ) {
				heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
				trycnt++;
			}
			heelses1 = mksobj(heelidentity, TRUE, FALSE, FALSE);
			if (heelses1) {
				heelses1->quan = 1;
				heelses1->known = heelses1->dknown = heelses1->bknown = heelses1->rknown = 1;
				heelses1->owt = weight(heelses1);
			}

			if (heelsamount >= 3) {

				heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
				trycnt = 0;

				while (trycnt < 5000 && !ishighheeledb(heelidentity) ) {
					heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
					trycnt++;
				}
				heelses2 = mksobj(heelidentity, TRUE, FALSE, FALSE);
				if (heelses2) {
					heelses2->quan = 1;
					heelses2->known = heelses2->dknown = heelses2->bknown = heelses2->rknown = 1;
					heelses2->owt = weight(heelses2);
				}

				heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
				trycnt = 0;

				while (trycnt < 5000 && !ishighheeledb(heelidentity) ) {
					heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
					trycnt++;
				}
				heelses3 = mksobj(heelidentity, TRUE, FALSE, FALSE);
				if (heelses3) {
					heelses3->quan = 1;
					heelses3->known = heelses3->dknown = heelses3->bknown = heelses3->rknown = 1;
					heelses3->owt = weight(heelses3);
				}

			}

			if (heelsamount >= 5) {

				heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
				trycnt = 0;

				while (trycnt < 5000 && !ishighheeledb(heelidentity) ) {
					heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
					trycnt++;
				}
				heelses4 = mksobj(heelidentity, TRUE, FALSE, FALSE);
				if (heelses4) {
					heelses4->quan = 1;
					heelses4->known = heelses4->dknown = heelses4->bknown = heelses4->rknown = 1;
					heelses4->owt = weight(heelses4);
				}

				heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
				trycnt = 0;

				while (trycnt < 5000 && !ishighheeledb(heelidentity) ) {
					heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
					trycnt++;
				}
				heelses5 = mksobj(heelidentity, TRUE, FALSE, FALSE);
				if (heelses5) {
					heelses5->quan = 1;
					heelses5->known = heelses5->dknown = heelses5->bknown = heelses5->rknown = 1;
					heelses5->owt = weight(heelses5);
				}

			}

			if (heelsamount >= 10) {

				heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
				trycnt = 0;

				while (trycnt < 5000 && !ishighheeledb(heelidentity) ) {
					heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
					trycnt++;
				}
				heelses6 = mksobj(heelidentity, TRUE, FALSE, FALSE);
				if (heelses6) {
					heelses6->quan = 1;
					heelses6->known = heelses6->dknown = heelses6->bknown = heelses6->rknown = 1;
					heelses6->owt = weight(heelses6);
				}

				heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
				trycnt = 0;

				while (trycnt < 5000 && !ishighheeledb(heelidentity) ) {
					heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
					trycnt++;
				}
				heelses7 = mksobj(heelidentity, TRUE, FALSE, FALSE);
				if (heelses7) {
					heelses7->quan = 1;
					heelses7->known = heelses7->dknown = heelses7->bknown = heelses7->rknown = 1;
					heelses7->owt = weight(heelses7);
				}

				heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
				trycnt = 0;

				while (trycnt < 5000 && !ishighheeledb(heelidentity) ) {
					heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
					trycnt++;
				}
				heelses8 = mksobj(heelidentity, TRUE, FALSE, FALSE);
				if (heelses8) {
					heelses8->quan = 1;
					heelses8->known = heelses8->dknown = heelses8->bknown = heelses8->rknown = 1;
					heelses8->owt = weight(heelses8);
				}

				heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
				trycnt = 0;

				while (trycnt < 5000 && !ishighheeledb(heelidentity) ) {
					heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
					trycnt++;
				}
				heelses9 = mksobj(heelidentity, TRUE, FALSE, FALSE);
				if (heelses9) {
					heelses9->quan = 1;
					heelses9->known = heelses9->dknown = heelses9->bknown = heelses9->rknown = 1;
					heelses9->owt = weight(heelses9);
				}

				heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
				trycnt = 0;

				while (trycnt < 5000 && !ishighheeledb(heelidentity) ) {
					heelidentity = (PLASTEEL_BOOTS + rn2(LEVITATION_BOOTS - PLASTEEL_BOOTS + 1) );
					trycnt++;
				}
				heelses10 = mksobj(heelidentity, TRUE, FALSE, FALSE);
				if (heelses10) {
					heelses10->quan = 1;
					heelses10->known = heelses10->dknown = heelses10->bknown = heelses10->rknown = 1;
					heelses10->owt = weight(heelses10);
				}

			}

			pline("Pick a pair of heels to acquire. The prompt will loop until you actually make a choice.");
heelschoice:
			if (heelses1) {
				pline("You can get %s.", doname(heelses1));
				if (yn("Do you want them?") == 'y') {
					dropy(heelses1);
					stackobj(heelses1);
					if (heelses2 && heelsamount >= 3) dealloc_obj(heelses2);
					if (heelses3 && heelsamount >= 3) dealloc_obj(heelses3);
					if (heelses4 && heelsamount >= 5) dealloc_obj(heelses4);
					if (heelses5 && heelsamount >= 5) dealloc_obj(heelses5);
					if (heelses6 && heelsamount >= 10) dealloc_obj(heelses6);
					if (heelses7 && heelsamount >= 10) dealloc_obj(heelses7);
					if (heelses8 && heelsamount >= 10) dealloc_obj(heelses8);
					if (heelses9 && heelsamount >= 10) dealloc_obj(heelses9);
					if (heelses10 && heelsamount >= 10) dealloc_obj(heelses10);
					goto heelschosen;
				}
			}

			if (heelses2 && heelsamount >= 3) {
				pline("You can get %s.", doname(heelses2));
				if (yn("Do you want them?") == 'y') {
					dropy(heelses2);
					stackobj(heelses2);
					if (heelses1) dealloc_obj(heelses1);
					if (heelses3 && heelsamount >= 3) dealloc_obj(heelses3);
					if (heelses4 && heelsamount >= 5) dealloc_obj(heelses4);
					if (heelses5 && heelsamount >= 5) dealloc_obj(heelses5);
					if (heelses6 && heelsamount >= 10) dealloc_obj(heelses6);
					if (heelses7 && heelsamount >= 10) dealloc_obj(heelses7);
					if (heelses8 && heelsamount >= 10) dealloc_obj(heelses8);
					if (heelses9 && heelsamount >= 10) dealloc_obj(heelses9);
					if (heelses10 && heelsamount >= 10) dealloc_obj(heelses10);
					goto heelschosen;
				}
			}

			if (heelses3 && heelsamount >= 3) {
				pline("You can get %s.", doname(heelses3));
				if (yn("Do you want them?") == 'y') {
					dropy(heelses3);
					stackobj(heelses3);
					if (heelses1) dealloc_obj(heelses1);
					if (heelses2 && heelsamount >= 3) dealloc_obj(heelses2);
					if (heelses4 && heelsamount >= 5) dealloc_obj(heelses4);
					if (heelses5 && heelsamount >= 5) dealloc_obj(heelses5);
					if (heelses6 && heelsamount >= 10) dealloc_obj(heelses6);
					if (heelses7 && heelsamount >= 10) dealloc_obj(heelses7);
					if (heelses8 && heelsamount >= 10) dealloc_obj(heelses8);
					if (heelses9 && heelsamount >= 10) dealloc_obj(heelses9);
					if (heelses10 && heelsamount >= 10) dealloc_obj(heelses10);
					goto heelschosen;
				}
			}

			if (heelses4 && heelsamount >= 5) {
				pline("You can get %s.", doname(heelses4));
				if (yn("Do you want them?") == 'y') {
					dropy(heelses4);
					stackobj(heelses4);
					if (heelses1) dealloc_obj(heelses1);
					if (heelses2 && heelsamount >= 3) dealloc_obj(heelses2);
					if (heelses3 && heelsamount >= 3) dealloc_obj(heelses3);
					if (heelses5 && heelsamount >= 5) dealloc_obj(heelses5);
					if (heelses6 && heelsamount >= 10) dealloc_obj(heelses6);
					if (heelses7 && heelsamount >= 10) dealloc_obj(heelses7);
					if (heelses8 && heelsamount >= 10) dealloc_obj(heelses8);
					if (heelses9 && heelsamount >= 10) dealloc_obj(heelses9);
					if (heelses10 && heelsamount >= 10) dealloc_obj(heelses10);
					goto heelschosen;
				}
			}

			if (heelses5 && heelsamount >= 5) {
				pline("You can get %s.", doname(heelses5));
				if (yn("Do you want them?") == 'y') {
					dropy(heelses5);
					stackobj(heelses5);
					if (heelses1) dealloc_obj(heelses1);
					if (heelses2 && heelsamount >= 3) dealloc_obj(heelses2);
					if (heelses3 && heelsamount >= 3) dealloc_obj(heelses3);
					if (heelses4 && heelsamount >= 5) dealloc_obj(heelses4);
					if (heelses6 && heelsamount >= 10) dealloc_obj(heelses6);
					if (heelses7 && heelsamount >= 10) dealloc_obj(heelses7);
					if (heelses8 && heelsamount >= 10) dealloc_obj(heelses8);
					if (heelses9 && heelsamount >= 10) dealloc_obj(heelses9);
					if (heelses10 && heelsamount >= 10) dealloc_obj(heelses10);
					goto heelschosen;
				}
			}

			if (heelses6 && heelsamount >= 10) {
				pline("You can get %s.", doname(heelses6));
				if (yn("Do you want them?") == 'y') {
					dropy(heelses6);
					stackobj(heelses6);
					if (heelses1) dealloc_obj(heelses1);
					if (heelses2 && heelsamount >= 3) dealloc_obj(heelses2);
					if (heelses3 && heelsamount >= 3) dealloc_obj(heelses3);
					if (heelses4 && heelsamount >= 5) dealloc_obj(heelses4);
					if (heelses5 && heelsamount >= 5) dealloc_obj(heelses5);
					if (heelses7 && heelsamount >= 10) dealloc_obj(heelses7);
					if (heelses8 && heelsamount >= 10) dealloc_obj(heelses8);
					if (heelses9 && heelsamount >= 10) dealloc_obj(heelses9);
					if (heelses10 && heelsamount >= 10) dealloc_obj(heelses10);
					goto heelschosen;
				}
			}

			if (heelses7 && heelsamount >= 10) {
				pline("You can get %s.", doname(heelses7));
				if (yn("Do you want them?") == 'y') {
					dropy(heelses7);
					stackobj(heelses7);
					if (heelses1) dealloc_obj(heelses1);
					if (heelses2 && heelsamount >= 3) dealloc_obj(heelses2);
					if (heelses3 && heelsamount >= 3) dealloc_obj(heelses3);
					if (heelses4 && heelsamount >= 5) dealloc_obj(heelses4);
					if (heelses5 && heelsamount >= 5) dealloc_obj(heelses5);
					if (heelses6 && heelsamount >= 10) dealloc_obj(heelses6);
					if (heelses8 && heelsamount >= 10) dealloc_obj(heelses8);
					if (heelses9 && heelsamount >= 10) dealloc_obj(heelses9);
					if (heelses10 && heelsamount >= 10) dealloc_obj(heelses10);
					goto heelschosen;
				}
			}

			if (heelses8 && heelsamount >= 10) {
				pline("You can get %s.", doname(heelses8));
				if (yn("Do you want them?") == 'y') {
					dropy(heelses8);
					stackobj(heelses8);
					if (heelses1) dealloc_obj(heelses1);
					if (heelses2 && heelsamount >= 3) dealloc_obj(heelses2);
					if (heelses3 && heelsamount >= 3) dealloc_obj(heelses3);
					if (heelses4 && heelsamount >= 5) dealloc_obj(heelses4);
					if (heelses5 && heelsamount >= 5) dealloc_obj(heelses5);
					if (heelses6 && heelsamount >= 10) dealloc_obj(heelses6);
					if (heelses7 && heelsamount >= 10) dealloc_obj(heelses7);
					if (heelses9 && heelsamount >= 10) dealloc_obj(heelses9);
					if (heelses10 && heelsamount >= 10) dealloc_obj(heelses10);
					goto heelschosen;
				}
			}

			if (heelses9 && heelsamount >= 10) {
				pline("You can get %s.", doname(heelses9));
				if (yn("Do you want them?") == 'y') {
					dropy(heelses9);
					stackobj(heelses9);
					if (heelses1) dealloc_obj(heelses1);
					if (heelses2 && heelsamount >= 3) dealloc_obj(heelses2);
					if (heelses3 && heelsamount >= 3) dealloc_obj(heelses3);
					if (heelses4 && heelsamount >= 5) dealloc_obj(heelses4);
					if (heelses5 && heelsamount >= 5) dealloc_obj(heelses5);
					if (heelses6 && heelsamount >= 10) dealloc_obj(heelses6);
					if (heelses7 && heelsamount >= 10) dealloc_obj(heelses7);
					if (heelses8 && heelsamount >= 10) dealloc_obj(heelses8);
					if (heelses10 && heelsamount >= 10) dealloc_obj(heelses10);
					goto heelschosen;
				}
			}

			if (heelses10 && heelsamount >= 10) {
				pline("You can get %s.", doname(heelses10));
				if (yn("Do you want them?") == 'y') {
					dropy(heelses10);
					stackobj(heelses10);
					if (heelses1) dealloc_obj(heelses1);
					if (heelses2 && heelsamount >= 3) dealloc_obj(heelses2);
					if (heelses3 && heelsamount >= 3) dealloc_obj(heelses3);
					if (heelses4 && heelsamount >= 5) dealloc_obj(heelses4);
					if (heelses5 && heelsamount >= 5) dealloc_obj(heelses5);
					if (heelses6 && heelsamount >= 10) dealloc_obj(heelses6);
					if (heelses7 && heelsamount >= 10) dealloc_obj(heelses7);
					if (heelses8 && heelsamount >= 10) dealloc_obj(heelses8);
					if (heelses9 && heelsamount >= 10) dealloc_obj(heelses9);
					goto heelschosen;
				}
			}

			pline("There's nothing else left on the list.");
			if (yn("Do you choose to get no heels at all?") == 'y') {

				if (heelses1) dealloc_obj(heelses1);
				if (heelses2 && heelsamount >= 3) dealloc_obj(heelses2);
				if (heelses3 && heelsamount >= 3) dealloc_obj(heelses3);
				if (heelses4 && heelsamount >= 5) dealloc_obj(heelses4);
				if (heelses5 && heelsamount >= 5) dealloc_obj(heelses5);
				if (heelses6 && heelsamount >= 10) dealloc_obj(heelses6);
				if (heelses7 && heelsamount >= 10) dealloc_obj(heelses7);
				if (heelses8 && heelsamount >= 10) dealloc_obj(heelses8);
				if (heelses9 && heelsamount >= 10) dealloc_obj(heelses9);
				if (heelses10 && heelsamount >= 10) dealloc_obj(heelses10);
				goto heelschosen;

			}
			goto heelschoice;

			}
heelschosen:
			pline("There might be something waiting for you on the %s...", surface(u.ux, u.uy));
			t_timeout = rnz(7500);
			break;

		case T_BEAUTY_CHARM:

			if (!PlayerInHighHeels) {
			    pline("You must be wearing high heels for that.");
				return 0;
			}

		    	techt_inuse(tech_no) = 11;
			nomul(-11, "posing sexily", FALSE);
			pline("You start posing sexily with your high heels, hoping to charm the bystanders.");
			if (uarmf && uarmf->oartifact == ART_PRECURSOR_TO_THE___) adjattrib(A_CHA, 1, -1, TRUE);

			t_timeout = rnz(2000);
			break;

		case T_ASIAN_KICK:

			if (!PlayerInStilettoHeels) {
			    pline("Only heroes wearing stiletto heels can use this technique!");
				return 0;
			}

		    	techt_inuse(tech_no) = 51;
			pline("Your stiletto heels are ready. Quick! Find someone with nuts and kick him for a devastating effect!");
			t_timeout = rnz(2500);
			break;

		case T_LEGSCRATCH_ATTACK:

			if (!PlayerInConeHeels) {
			    pline("Only heroes wearing cone heels can use this technique!");
				return 0;
			}

			num = 50 + (techlevX(tech_no) * 2);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your cone heels are eager to scratch up and down hostile legs. Make sure you kick everyone that crosses your path!");

			t_timeout = rnz(3000);
			break;

		case T_GROUND_STOMP:

			if (!PlayerInWedgeHeels) {
			    pline("Only heroes wearing wedge heels can use this technique!");
				return 0;
			}

			num = 25 + (techlevX(tech_no) * 2);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your wedge heels absolutely want to stomp enemies, so you should try to kick monsters with them now!");

			t_timeout = rnz(4000);
			break;

		case T_ATHLETIC_COMBAT:

			if (!PlayerInBlockHeels) {
			    pline("Only heroes wearing block heels can use this technique!");
				return 0;
			}

			num = 50 + (techlevX(tech_no) * 3);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your block heels expect you to repeatedly kick enemies now, hoping to bludgeon them.");

			t_timeout = rnz(2500);
			break;

		case T_PRAYING_SUCCESS:

		    	techt_inuse(tech_no) = 1;
			dopray();
			t_timeout = rnz(30000);
			break;

		case T_OVER_RAY:

			num = 100 + (techlevX(tech_no) * 5);
		    	techt_inuse(tech_no) = num + 1;
			pline("For a while, all your rays have extended range!");

			t_timeout = rnz(5000);
			break;

		case T_ENCHANTERANG:

			if (!uwep) {
				pline("That doesn't work without a weapon!");
				return 0;
			}
			if (uwep && weapon_type(uwep) != P_BOOMERANG && weapon_type(uwep) != -P_BOOMERANG) {
				pline("That only works if your wielded weapon is a boomerang, and currently it's not!");
				return 0;
			}

			if (uwep && stack_too_big(uwep)) {
				pline("You were trying to enchant too many boomerangs at once and therefore they barely received any enchantment bonus.");
			} else if (uwep && uwep->spe < 0) {
				uwep->spe += rnd(abs(uwep->spe));
				if (uwep->spe > 0) uwep->spe = 0; /* fail safe */
			} else if (uwep && uwep->spe < 2) {
				uwep->spe++;
			} else if (uwep && uwep->spe < 25 && !rn2(uwep->spe) ) {
				uwep->spe++;
			}

			if (uwep && stack_too_big(uwep)) {
				pline("You were trying to enchant too many boomerangs at once and therefore they barely received any enchantment bonus.");
			} else if (uwep && uwep->spe < 2) {
				uwep->spe++;
			} else if (uwep && uwep->spe < 25 && !rn2(uwep->spe) ) {
				uwep->spe++;
			}

			if (uwep && stack_too_big(uwep)) {
				pline("You were trying to enchant too many boomerangs at once and therefore they barely received any enchantment bonus.");
			} else if (uwep && uwep->spe < 2) {
				uwep->spe++;
				pline("Your boomerang is barely enchanted.");
			} else if (uwep && uwep->spe < 25 && !rn2(uwep->spe) ) {
				uwep->spe++;
				pline("Your boomerang is well-enchanted.");
			} else pline("Your boomerang is moderately enchanted.");

			t_timeout = rnz(10000);
			break;

		case T_BATMAN_ARSENAL:

			{
				struct obj *uboomerang;

				pline("A boomerang is created!");
				uboomerang = mksobj(rn2(3) ? BOOMERANG : !rn2(4) ? ALU_BOOMERANG : !rn2(3) ? BATARANG : rn2(2) ? DARK_BATARANG : SILVER_CHAKRAM, TRUE, FALSE, FALSE);
				if (uboomerang) {
					uboomerang->quan = 1;
					uboomerang->known = uboomerang->dknown = uboomerang->bknown = uboomerang->rknown = 1;
					uboomerang->owt = weight(uboomerang);
					dropy(uboomerang);
					stackobj(uboomerang);
				}

			}

			t_timeout = rnz(5000);
			break;

		case T_JOKERBANE:

			num = 100 + (techlevX(tech_no) * 5);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your batarang is capable of paralyzing monsters for a while.");
			t_timeout = rnz(8000);

			break;

		case T_CALL_THE_POLICE:

			pline("You call the kops!");

			{

			int caughtY;
			caughtY = 0;

			while (caughtY == 0) {
		 	      mtmp = makemon(mkclass(S_KOP,0), u.ux, u.uy, NO_MM_FLAGS);
				if (!mtmp) break;
				(void) tamedog(mtmp, (struct obj *) 0, TRUE);
				if (techlevX(tech_no) < rnd(80)) caughtY++;
			}

			}

			t_timeout = rnz(10000);
			break;

		case T_DOMINATE:

			if (u.uswallow) {
			    pline("Domination doesn't work while you're engulfed!");
				return 0;
			}

			{

			int k, l, caughtX;
			struct monst *mtmp3;
			caughtX = 0;
			pline("You try to dominate monsters!");

		    for (k = -1; k <= 1; k++) for(l = -1; l <= 1; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;
			if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) && !is_infrastructure_monster(mtmp3) && mtmp3->mtame == 0 && mtmp3->isminion == 0 && mtmp3->isgyp == 0
			&& mtmp3->mnum != quest_info(MS_NEMESIS) && !(mtmp3->data->geno & G_UNIQ) && caughtX == 0)

			{

				if ( is_animal(mtmp3->data)) {
					pline("%s is dominated!", mon_nam(mtmp3));
					(void) tamedog(mtmp3, (struct obj *) 0, TRUE);
					if (techlevX(tech_no) < rnd(100)) caughtX++;
					t_timeout = rnz(10000);
				}

				else pline("%s can't be dominated with this method!", mon_nam(mtmp3));

			} /* monster is catchable loop */
		    } /* for loop */

			t_timeout = rnz(10000);

			}

			break;

		case T_INCARNATION:

			{

			int k, l, polymorphnumber;
			struct monst *mtmp3;

			pline("Pick a monster to incarnate into. The prompt will loop until you actually make a choice.");

incarnationselect:
			for (k = -1; k <= 1; k++) for(l = -1; l <= 1; l++) {
				if (!isok(u.ux + k, u.uy + l)) continue;
				if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0)) {
					polymorphnumber = mtmp3->mnum;
					pline("You can incarnate into %s.", mtmp3->data->mname);
					if (yn("Do it?") == 'y') {
						u.wormpolymorph = polymorphnumber;
						goto incarnationfinish;
					}
				}
			}

			pline("You can also choose to not incarnate at all, although that would be a waste.");
			if (yn("Do it?") == 'y') {
				pline("Well, if you really want to waste such a powerful technique...");
				t_timeout = rnz(25000);
				break;
			}
			goto incarnationselect;

incarnationfinish:
			if (!rn2(3)) {
				u.wormpolymorph = rn2(NUMMONS);
				pline("Your incarnation attempt goes out of control!");
			}
			polyself(FALSE);

			}

			t_timeout = rnz(25000);

			break;

		case T_COMBO_STRIKE:

			if (!canuseunarmedtechs()) {
				You("can't do that while wielding a weapon!");
				return 0;
			}

			num = 30 + (techlevX(tech_no) * 2);
		    	techt_inuse(tech_no) = num + 1;
			pline("You're starting your combo!");
			u.combostrike = 1;
			u.comboactive = TRUE;
			t_timeout = rnz(5000);

			break;

		case T_FUNGOISM:

			pline("You feel fungal.");
		    	techt_inuse(tech_no) = 1;
			polyself(FALSE);
		    	techt_inuse(tech_no) = 0;
		      t_timeout = rnz(8000);
			break;

		case T_BECOME_UNDEAD:

			pline("You feel dead.");
		    	techt_inuse(tech_no) = 1;
			polyself(FALSE);
		    	techt_inuse(tech_no) = 0;
		      t_timeout = rnz(8000);
			break;

		case T_JIU_JITSU:

			num = 1000 + (techlevX(tech_no) * 10);
		    	techt_inuse(tech_no) = num + 1;
			pline("You start doing jiu-jitsu!");
			t_timeout = rnz(10000);
			break;

		case T_BLADE_ANGER:

			num = 100 + (techlevX(tech_no) * 3);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your shuriken can fire beams now!");

			t_timeout = rnz(7500);
			break;

		case T_RE_TAMING:

			pline("You try to tame your former pet...");
			{

			int k, l;
			struct monst *mtmp3;

			for (k = -1; k <= 1; k++) for(l = -1; l <= 1; l++) {
				if (!isok(u.ux + k, u.uy + l)) continue;
				if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) && !mtmp3->mfrenzied && (mtmp3->mpeaceful || !rn2(3) || ((rnd(30 - ACURR(A_CHA))) < 4) ) && mtmp3->mtame == 0 && mtmp3->wastame) {

					pline("%s wants to be your pet again!", mon_nam(mtmp3));
					(void) tamedog(mtmp3, (struct obj *) 0, TRUE);
					t_timeout = rnz(1500);
					break;
				}

			}

			}

			t_timeout = rnz(1500);

			break;

	    case T_INTRINSIC_ROULETTE:
		You("spin the roulette wheel...");
		intrinsicgainorloss(0);
		t_timeout = rnz(10000);
		break;

	    case T_SPECTRAL_SWORD:
		You("focus the powers of the elements into your weapon.");
		techt_inuse(tech_no) = rnd((int) (techlevX(tech_no)/3 + 1)) + d(1,4) + 2;
		t_timeout = rnz(1500);
		break;

	    case T_REVERSE_IDENTIFY:
		{
			int j;
			long oldgold;
			char buf[BUFSZ];

			for (j = 0; j <= 5; j++) {
				if (j >= 5) {
					pline("%s", thats_enough_tries);
					goto revid_end;
				}
				getlin("What do you want to identify?", buf);
				if (buf[0] == 0) continue;
				oldgold = u.ugold;
				otmp = readobjnam(buf, (struct obj *)0, TRUE, FALSE);
				if (u.ugold != oldgold) {
					pline("Don't you date cheat me again! -- Your fault!");
					/* Make them pay */
					u.ugold = oldgold / 2;
					continue;
				}
				if (otmp == &zeroobj || otmp == (struct obj *) 0) {
					pline("That doesn't exist.");
					continue;
				}
				break;
			}
			strcpy(buf,xname(otmp));
			makeknown(otmp->otyp);
			pline("The %s is a %s.",buf,xname(otmp));
			if (Has_contents(otmp))
				delete_contents(otmp);
			obfree(otmp,(struct obj *) 0);
		}
revid_end:
		pline("You also learn the identity of some other objects:");
		for (i = 0; i < 5; i++) {
			j = rn2(NUM_OBJECTS);
			while (objects[j].oc_prob < 1) j = rn2(NUM_OBJECTS);
			makeknown(j);
			pline("%s (%s).", obj_descr[j].oc_name, obj_descr[j].oc_descr);
		}
		t_timeout = rnz(20000);

		break;

	    case T_DETECT_TRAPS:
		trap_detect((struct obj *)0);
		t_timeout = rnz(20000);
		break;

	    case T_DIRECTIVE:
		if (PlayerCannotUseSkills) {
			pline("Unfortunately, no one seems to follow any directives you're giving.");
			t_timeout = rnz(200);
			break;
		}
		if (P_SKILL(P_PETKEEPING) < P_SKILLED && P_SKILL(P_RIDING) < P_SKILLED) {
			pline("Unfortunately, no one seems to follow any directives you're giving.");
			t_timeout = rnz(200);
			break;
		}

		if (P_SKILL(P_RIDING) >= P_SKILLED) {

			pline("Currently your steed has %d%% chance of being targetted by monsters.", u.steedhitchance);
			if (yn("Change it?") == 'y') {

				int lowerbound, higherbound;
				lowerbound = 25;
				higherbound = 25;

				switch (P_SKILL(P_RIDING)) {
					case P_SKILLED:
						lowerbound = 20;
						higherbound = 33;
						break;
					case P_EXPERT:
						lowerbound = 10;
						higherbound = 50;
						break;
					case P_MASTER:
						lowerbound = 5;
						higherbound = 75;
						break;
					case P_GRAND_MASTER:
						lowerbound = 3;
						higherbound = 90;
						break;
					case P_SUPREME_MASTER:
						lowerbound = 1;
						higherbound = 100;
						break;
					default:
						lowerbound = 25;
						higherbound = 25;
						break;
				}

				pline("You can set the chance to values between %d%% and %d%% (inclusive).", lowerbound, higherbound);
				if (lowerbound <= 1 && yn("Set the chance to 1%%?") == 'y') {
					u.steedhitchance = 1;
					pline("The chance that attacks target your steed is 1%% now.");
				} else if (lowerbound <= 3 && yn("Set the chance to 3%%?") == 'y') {
					u.steedhitchance = 3;
					pline("The chance that attacks target your steed is 3%% now.");
				} else if (lowerbound <= 5 && yn("Set the chance to 5%%?") == 'y') {
					u.steedhitchance = 5;
					pline("The chance that attacks target your steed is 5%% now.");
				} else if (lowerbound <= 10 && yn("Set the chance to 10%%?") == 'y') {
					u.steedhitchance = 10;
					pline("The chance that attacks target your steed is 10%% now.");
				} else if (lowerbound <= 20 && yn("Set the chance to 20%%?") == 'y') {
					u.steedhitchance = 20;
					pline("The chance that attacks target your steed is 20%% now.");
				} else if (yn("Set the chance to 25%%?") == 'y') {
					u.steedhitchance = 25;
					pline("The chance that attacks target your steed is 25%% now.");
				} else if (higherbound >= 33 && yn("Set the chance to 33%%?") == 'y') {
					u.steedhitchance = 33;
					pline("The chance that attacks target your steed is 33%% now.");
				} else if (higherbound >= 50 && yn("Set the chance to 50%%?") == 'y') {
					u.steedhitchance = 50;
					pline("The chance that attacks target your steed is 50%% now.");
				} else if (higherbound >= 75 && yn("Set the chance to 75%%?") == 'y') {
					u.steedhitchance = 75;
					pline("The chance that attacks target your steed is 75%% now.");
				} else if (higherbound >= 90 && yn("Set the chance to 90%%?") == 'y') {
					u.steedhitchance = 90;
					pline("The chance that attacks target your steed is 90%% now.");
				} else if (higherbound >= 100 && yn("Set the chance to 100%%?") == 'y') {
					u.steedhitchance = 100;
					pline("The chance that attacks target your steed is 100%% now.");
				} else pline("The chance that attacks target your steed remains %d%%.", u.steedhitchance);

			}

		}

		if (P_SKILL(P_PETKEEPING) >= P_SKILLED) {
			pline("Currently your pets can%s pick up items.", u.petcollectitems ? "" : "'t");
			if (yn("Change it?") == 'y') {
				if (u.petcollectitems) u.petcollectitems = 0;
				else u.petcollectitems = 1;
				pline("Your pets can%s pick up items now.", u.petcollectitems ? "" : "'t");
			}
		}

		if (P_SKILL(P_PETKEEPING) >= P_EXPERT) {
			pline("Currently your pets can%s attack%s monsters.", (u.petattackenemies > 0) ? "" : "'t", u.petattackenemies == 2 ? " both hostile and peaceful" : u.petattackenemies == 1 ? " only hostile" : u.petattackenemies == -1 ? " or be attacked by" : "");
			if (yn("Change it?") == 'y') {
				pline("You got the following options: make the pet attack everything, make it attack only hostile monsters, or prevent it from attacking anything (with the choice that it may or may not still be attacked by hostiles).");
				if (yn("Do you want your pets to attack everything?") == 'y') {
					u.petattackenemies = 2;
					pline("Your pets can attack all monsters now.");
				} else if (yn("Do you want your pets to only attack hostile creatures?") == 'y') {
					u.petattackenemies = 1;
					pline("Your pets can attack hostile monsters now, but will leave peaceful ones alone.");
				} else if (yn("Do you want your pets to not attack any monsters?") == 'y') {
					u.petattackenemies = 0;
					pline("Your pets can't attack monsters now.");
				} else if (yn("Do you want your pets to not attack any monsters, and also not be attacked by them anymore?") == 'y') {
					u.petattackenemies = -1;
					pline("Your pets can't attack monsters now and also won't be attacked by monsters.");
				}
			}
		}

		if (P_SKILL(P_PETKEEPING) >= P_MASTER) {
			pline("Currently your pets can%s eat food off the floor.", u.petcaneat ? "" : "'t");
			if (yn("Change it?") == 'y') {
				if (u.petcaneat) u.petcaneat = 0;
				else u.petcaneat = 1;
				pline("Your pets can%s eat food off the floor now.", u.petcaneat ? "" : "'t");
			}
		}

		if (P_SKILL(P_PETKEEPING) >= P_GRAND_MASTER) {
			pline("Currently your pets can%s try to follow you.", u.petcanfollow ? "" : "'t");
			if (yn("Change it?") == 'y') {
				if (u.petcanfollow) u.petcanfollow = 0;
				else u.petcanfollow = 1;
				pline("Your pets can%s try to follow you now.", u.petcanfollow ? "" : "'t");
			}
		}

		t_timeout = rnz(P_SKILL(P_PETKEEPING) >= P_SUPREME_MASTER ? 50 : 200);
		break;

	    case T_REMOVE_IMPLANT:

		if (!uimplant) {
			pline("There is no implant that could be removed!");
			return 0;
		}
		pline("Your implant is removed.");
		Implant_off();

		t_timeout = rnz(20000);
		break;

	    case T_REROLL_IMPLANT:

		if (!uimplant) {
			pline("You're not wearing any implant!");
			return 0;
		}

		{
			register struct obj *wearimplant;
			long savewornmask;

			wearimplant = uimplant;
			if (!wearimplant) {
				pline("Somehow the implant went missing...");
				break;
			}

			savewornmask = wearimplant->owornmask;
			setworn((struct obj *)0, wearimplant->owornmask);

			if (wearimplant->otyp >= IMPLANT_OF_ABSORPTION && wearimplant->otyp <= IMPLANT_OF_PROSPERITY) {
				objects[wearimplant->otyp].a_ac = rnd(10);
			}
			if (wearimplant->otyp >= IMPLANT_OF_QUEEB_BUTT && wearimplant->otyp <= IMPLANT_OF_FINGER_POINTING) {
				objects[wearimplant->otyp].a_ac = rnd(10);
			}

			if (wearimplant->otyp >= IMPLANT_OF_QUICKENING && wearimplant->otyp <= IMPLANT_OF_BLITZEN) {
				objects[wearimplant->otyp].a_ac = rnd(8);
				objects[wearimplant->otyp].oc_oprop = randnastyenchantment();
			}
			if (wearimplant->otyp >= IMPLANT_OF_TOTAL_NONSENSE && wearimplant->otyp <= IMPLANT_OF_GALVANIZATION) {
				objects[wearimplant->otyp].a_ac = rnd(8);
				objects[wearimplant->otyp].oc_oprop = randnastyenchantment();
			}

			if (wearimplant->otyp >= IMPLANT_OF_SERENITY && wearimplant->otyp <= IMPLANT_OF_CARNAGE) {
				objects[wearimplant->otyp].a_ac = rnd(9);
				objects[wearimplant->otyp].oc_oprop = randfemienchantment();
			}
			if (wearimplant->otyp >= IMPLANT_OF_LEGEND && wearimplant->otyp <= IMPLANT_OF_VIRTUE) {
				objects[wearimplant->otyp].a_ac = rnd(9);
				objects[wearimplant->otyp].oc_oprop = randfemienchantment();
			}

			if (wearimplant->otyp >= IMPLANT_OF_IRE && wearimplant->otyp <= IMPLANT_OF_FREEDOM) {
				objects[wearimplant->otyp].a_ac = rnd(5);
				objects[wearimplant->otyp].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
			}
			if (wearimplant->otyp >= IMPLANT_OF_TEN_THOUSAND_THINGS && wearimplant->otyp <= IMPLANT_OF_VIRTUE) {
				objects[wearimplant->otyp].a_ac = rnd(5);
				objects[wearimplant->otyp].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
			}
			wearimplant->shirtmessage = rnd(1000000);

			setworn(wearimplant, savewornmask);
			pline("All done - your implant got re-initialized! Its base AC value got rerolled, and if it was a type that had a random enchantment, that was re-randomized as well. The bonus you'll get from wearing it while in a form without hands has also been changed.");
		}

		t_timeout = rnz(10000);
		break;

	    case T_TIME_STOP:

		pline((Role_if(PM_SAMURAI) || Role_if(PM_NINJA)) ? "Jikan ga teishi shimashita." : "Time has stopped.");
		TimeStopped += (5 + rnd(6));
		t_timeout = rnz(25000);
		break;

	    case T_STAT_RESIST:
		num = 50 + (techlevX(tech_no) * 10);
	    	techt_inuse(tech_no) = num + 1;
		t_timeout = rnz(3000);
		break;

	    case T_UNCURSE_SABER:
		if (!uwep) {
			pline("That doesn't work without a weapon!");
			break;
		}
		if (uwep && !(is_lightsaber(uwep))) {
			pline("Trying to cheat, huh? Nope. This technique can only uncurse lightsabers.");
			break;
		}
		if (uwep && !(uwep->cursed)) {
			uwep->bknown = TRUE;
			pline("Your lightsaber wasn't cursed to begin with, and therefore nothing happens.");
			t_timeout = rnz(5000);
			break;
		}
		t_timeout = rnz(5000);
		if (uwep) {
			pline("Your lightsaber is surrounded by a holy aura.");
			uncurse(uwep, TRUE);
		}
		break;

	    case T_ENERGY_CONSERVATION:
		if (!uwep || (uwep && !is_lightsaber(uwep))) {
			pline("You're not holding a lightsaber!");
			break;
		}
		num = 1000 + (techlevX(tech_no) * 10);
	    	techt_inuse(tech_no) = num + 1;
		pline("The energy consumption rate of your lightsaber slows down!");
		t_timeout = rnz(6000);
		break;

	    case T_UNARMED_FOCUS:

		if (!canuseunarmedtechs()) {
			pline("That requires you to be fighting without a weapon!");
			break;
		}
		docalm(); /* stop all other techs that might be active --Amy */

		num = 1000 + (techlevX(tech_no) * 10);
	    	techt_inuse(tech_no) = num + 1;
		pline("You're focussing on fighting unarmed!");
		t_timeout = rnz(6000);
		break;

	    case T_ENCHANT_ROBE:
		{
			register struct obj *wearrobe;
			long savewornmask;

			if (!uarm) {
				pline("Without armor, that technique won't do anything.");
				break;
			}
			if (uarm && !(uarm->otyp >= ROBE && uarm->otyp <= ROBE_OF_WEAKNESS)) {
				pline("Nope, that won't work. Wear a robe!");
				break;
			}
			wearrobe = uarm;
			if (!wearrobe) {
				pline("Somehow the robe is missing...");
				break;
			}
			savewornmask = wearrobe->owornmask;
			setworn((struct obj *)0, wearrobe->owornmask);
			t_timeout = rnz(10000);

			wearrobe->enchantment = randenchantment();
			pline("Your robe's special enchantment was randomized.");
			if (!PlayerCannotUseSkills && P_SKILL(P_SORESU) >= P_GRAND_MASTER) {
				if (wearrobe && wearrobe->spe < 0) {
					wearrobe->spe += rnd(abs(wearrobe->spe));
					if (wearrobe->spe > 0) wearrobe->spe = 0; /* fail safe */
					pline("Your robe's negative enchantment value is remedied.");
				} else if (wearrobe && wearrobe->spe < 2) {
					wearrobe->spe++;
					pline("Your robe gained a positive enchantment value.");
				} else if (wearrobe && wearrobe->spe < 7 && !rn2(wearrobe->spe) ) {
					wearrobe->spe++;
					pline("Your robe has a high positive enchantment value now.");
				} else pline("Your robe's enchantment value unfortunately didn't increase.");

			}
			setworn(wearrobe, savewornmask);
		}
		break;

	    case T_WILD_SLASHING:
		if (!(uwep && is_lightsaber(uwep) && uwep->lamplit && u.twoweap && uswapwep && is_lightsaber(uswapwep) && uswapwep->lamplit)) {
			pline("You must be dual-wielding lit lightsabers for that!");
			break;
		}
		num = 20 + (techlevX(tech_no) * 3);
	    	techt_inuse(tech_no) = num + 1;
		t_timeout = rnz(3000);
		pline("Your lightsabers start attacking rapidly at the cost of accuracy.");
		break;

	    case T_ABSORBER_SHIELD:
		if (!uwep || (uwep && !is_lightsaber(uwep))) {
			pline("You're not holding a lightsaber!");
			break;
		}
		num = 200 + (techlevX(tech_no) * 7);
	    	techt_inuse(tech_no) = num + 1;
		t_timeout = rnz(2500);
		pline("Your lightsaber becomes capable of absorbing enemy projectiles to gain energy.");
		break;

	    case T_PSYCHO_FORCE:
		{
			int forcedamage;
			coord cc;
			struct monst *psychmonst;
			pline("Select a monster to use psycho force");
			cc.x = u.ux;
			cc.y = u.uy;
			getpos(&cc, TRUE, "the spot to attack");
			if (cc.x == -10) return (0); /* user pressed esc */
			psychmonst = m_at(cc.x, cc.y);

			if (!psychmonst || (!canseemon(psychmonst) && !canspotmon(psychmonst))) {
				You("don't see a monster there!");
				return (0);
			}

			if (psychmonst) {

				forcedamage = u.ulevel + techlevX(tech_no);
				if (!PlayerCannotUseSkills) {

					switch (P_SKILL(P_DJEM_SO)) {
						case P_MASTER: forcedamage *= 3; forcedamage /= 2; break;
						case P_GRAND_MASTER: forcedamage *= 2; break;
						case P_SUPREME_MASTER: forcedamage *= 5; forcedamage /= 2; break;
					}

				}
				psychmonst->mcanmove = 0;
				psychmonst->mstrategy &= ~STRAT_WAITFORU;

				if (forcedamage < 31) psychmonst->mfrozen = 3;
				else psychmonst->mfrozen = (forcedamage / 10);

				pline("%s sputters at the forced hold!", Monnam(psychmonst));
				psychmonst->mhp -= forcedamage;
				if (psychmonst->mhp < 1) {
					pline("%s dies!", Monnam(psychmonst));
					xkilled(psychmonst,0);
				} else wakeup(psychmonst); /* monster becomes hostile */

			}

		}
		t_timeout = rnz(2000);
		break;

	    case T_INTENSIVE_TRAINING:

		pline("You train your attributes...");
		adjattrib(rn2(A_MAX), 1, -1, TRUE);
		t_timeout = rnz(7500);
		break;

	    case T_SURRENDER_OR_DIE:
		if (!uwep || (uwep && !is_lightsaber(uwep))) {
			pline("You're not holding a lightsaber!");
			break;
		}
		num = 200 + (techlevX(tech_no) * 20);
	    	techt_inuse(tech_no) = num + 1;
		t_timeout = rnz(5000);
		pline("Your lightsaber gains the ability to pacify monsters when breaking their weapon!");
		break;

	    case T_PERILOUS_WHIRL:
		if (!uwep || (uwep && !is_lightsaber(uwep)) || (uwep && !bimanual(uwep)) ) {
			pline("You're not holding a double lightsaber!");
			break;
		}
		num = 100 + (techlevX(tech_no) * 6);
	    	techt_inuse(tech_no) = num + 1;
		t_timeout = rnz(2500);
		pline("Your lightsaber can drain the life of opponents for a while!");
		break;

	    case T_SUMMON_SHOE:
		t_timeout = rnz(10000);

		{
			struct permonst *shoe = 0;
			int attempts = 0;
			struct monst *shoemonst;

			do {
				shoe = rndmonst();
				attempts++;
				if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!shoe || (shoe && !(shoe->msound == MS_SHOE))) && attempts < 50000);

			if (shoe && (shoe->msound == MS_SHOE) ) {
				pline("A shoe appears from nowhere!");
				shoemonst = makemon(shoe, u.ux, u.uy, NO_MM_FLAGS);
				if (shoemonst) (void) tamedog(shoemonst, (struct obj *) 0, TRUE);
			} else if (shoe) {
				pline("Hmm... you expected a shoe, but some other monster appeared instead!");
				shoemonst = makemon(shoe, u.ux, u.uy, NO_MM_FLAGS);
				if (shoemonst) (void) tamedog(shoemonst, (struct obj *) 0, TRUE);
			} else pline("Somehow, it failed... :(");

			u.mondiffhack = 0;

		}

		break;

	    case T_EDDY_WIND:
		num = 1 + techlevX(tech_no);
	    	techt_inuse(tech_no) = num + 1;
		pline("You prepare a powerful axe-and-sword-mill!");

	      t_timeout = rnz(3500);
	      break;

	    case T_KICK_IN_THE_NUTS:

		if (!PlayerInSexyFlats) {
			pline("You must be wearing sexy flats for that!");
			return 0;
		}

	    	if (!getdir((char *)0)) return 0;
		if (!u.dx && !u.dy) {
		    /* Hopefully a mistake ;B */
		    pline(flags.female ? "Since you don't have nuts, you cannot try to smash them with your feet either." : "For a moment, you feel the itch to ram your footwear into your own nuts, but then remember that you have a task to complete, and therefore decide against the idea.");
		    return 0;
		}
		if (!isok(u.ux + u.dx, u.uy + u.dy)) {
			pline("Invalid target location.");
			return 0;
		}
		mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
		if (!mtmp) {
			You("perform a powerful kick, and listen to very sexy air current noises as your %s whirls through thin air.", body_part(FOOT));
		} else if (mtmp->female || is_neuter(mtmp->data)) {
			pline("%s has no nuts to kick and is therefore unaffected!", Monnam(mtmp));
		} else {
			pline("%s moans in agony as his nuts are squashed by your sexy footwear!", Monnam(mtmp));
			mtmp->mcanmove = 0;
			mtmp->mfrozen = 10 + rnd(techlevX(tech_no));
			mtmp->mstrategy &= ~STRAT_WAITFORU;
			int tmp = 50 + rnd(techlevX(tech_no) * 4);
			hurtmon(mtmp, tmp);
		}
		if (mtmp && !DEADMONSTER(mtmp) && mtmp->mpeaceful && !mtmp->mtame) mtmp->mpeaceful = 0; /* monster becomes hostile */

	      t_timeout = rnz(2500);

	      break;

	    case T_DISARMING_KICK:

		if (!PlayerInSexyFlats) {
			pline("You must be wearing sexy flats for that!");
			return 0;
		}

		if (u.uswallow) {
	    		pline("Kicking an engulfer's weapon from the inside would be quite the feat.");
	    		return(0);
		}

	    	if (!getdir((char *)0)) return(0);
		if (!u.dx && !u.dy) {
			/* Hopefully a mistake ;B */
			pline("Why don't you try wielding something else instead.");
			return(0);
		}
		if (!isok(u.ux + u.dx, u.uy + u.dy)) {
			pline("Invalid target location.");
			return 0;
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

		You("kick %s in the %s with your sexy footwear...", mon_nam(mtmp), mbodypart(mtmp, HAND));

		obj_extract_self(obj);
		possibly_unwield(mtmp, FALSE);
		setmnotwielded(mtmp, obj);

		if (obj && obj->mstartinventB && !(obj->oartifact) && !(obj->fakeartifact && timebasedlowerchance()) && (!rn2(4) || (rn2(100) < u.equipmentremovechance) || !timebasedlowerchance() ) ) {
			You("vaporize %s %s!", s_suffix(mon_nam(mtmp)), xname(obj));
			delobj(obj);
			t_timeout = rnz(1000);
			break;
		} else if (obj && obj->mstartinventC && !(obj->oartifact) && !(obj->fakeartifact && !rn2(10)) && rn2(10) ) {
			You("vaporize %s %s!", s_suffix(mon_nam(mtmp)), xname(obj));
			delobj(obj);
			t_timeout = rnz(1000);
			break;
		} else if (obj && obj->mstartinventE && !(obj->oartifact) && !(obj->fakeartifact && !rn2(20)) && rn2(20) ) {
			You("vaporize %s %s!", s_suffix(mon_nam(mtmp)), xname(obj));
			delobj(obj);
			t_timeout = rnz(1000);
			break;
		} else if (obj && obj->mstartinventD && !(obj->oartifact) && !(obj->fakeartifact && !rn2(4)) && rn2(4) ) {
			You("vaporize %s %s!", s_suffix(mon_nam(mtmp)), xname(obj));
			delobj(obj);
			t_timeout = rnz(1000);
			break;
		} else if (obj && obj->mstartinventX) {
			You("vaporize %s %s!", s_suffix(mon_nam(mtmp)), xname(obj));
			delobj(obj);
			t_timeout = rnz(1000);
			break;
		}
		else if (obj) {
			You("knock %s %s to the %s!", s_suffix(mon_nam(mtmp)), xname(obj), surface(u.ux, u.uy));
			if (obj->otyp == CRYSKNIFE && (!obj->oerodeproof || !rn2(10))) {
				obj->otyp = WORM_TOOTH;
				obj->oerodeproof = 0;
			}
			place_object(obj, u.ux, u.uy);
			stackobj(obj);
		}
		t_timeout = rnz(1000);

	      break;

	    case T_INLAY_WARFARE:

		if (!PlayerInSexyFlats) {
			pline("You must be wearing sexy flats for that!");
			return 0;
		}

		nomul(-2, "inhaling their own smelly inlay", FALSE);
		make_confused(HConfusion + rnz(10), FALSE);
		set_itimeout(&HeavyConfusion, HConfusion);

		{

			int inlayradius = rnd(5);
			int mondistance = 0;
			struct monst *mtmp3;
			int k, l;

			for (k = -inlayradius; k <= inlayradius; k++) for(l = -inlayradius; l <= inlayradius; l++) {
				if (!isok(u.ux + k, u.uy + l)) continue;

				mondistance = 1;
				if (k > 1) mondistance = k;
				if (k < -1) mondistance = -k;

				if (l > 1 && l > mondistance) mondistance = l;
				if (l < -1 && (-l > mondistance)) mondistance = -l;

				if ( (mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) {
					mtmp3->mcanmove = 0;
					mtmp3->mfrozen = (16 - (mondistance * 2));
					mtmp3->mstrategy &= ~STRAT_WAITFORU;
					mtmp3->mconf = TRUE;
					pline("%s becomes dizzy from the smell!", Monnam(mtmp3));
					if (mtmp3->mpeaceful && !mtmp3->mtame) mtmp3->mpeaceful = 0; /* monster becomes hostile */
				}
		}

		if (!PlayerCannotUseSkills && P_SKILL(P_SEXY_FLATS) >= P_SUPREME_MASTER)
			(void) create_gas_cloud(u.ux, u.uy, 4, 12);
		else
			(void) create_gas_cloud(u.ux, u.uy, 3, 8);
		}

	      t_timeout = rnz(4000);

	      break;

	    case T_DIAMOND_BARRIER:

		pline("You try to erect barriers!");

		if (Role_if(PM_WALSCHOLAR)) {
			register struct obj *waldiamond;

			waldiamond = carrying(DIAMOND);

			if (!waldiamond) {
				pline("But you don't have a diamond to create them...");
				break;
			}

			if (waldiamond) {
				if (waldiamond->quan > 1) {
					waldiamond->quan--;
					waldiamond->owt = weight(waldiamond);
				}
				else useup(waldiamond);
				You("use a diamond to create the barriers.");
			}

		}

		{

			int diamondradius = 1;

			if (Role_if(PM_WALSCHOLAR)) diamondradius = 3; /* can't reduce, unlike mason */

			if (Role_if(PM_MASON)) {

				diamondradius = rounddiv(techlevX(tech_no), 5);
				if ((techlevX(tech_no), 5) > 4) diamondradius++; /* fix off-by-one error --Amy */
				if (diamondradius < 1) diamondradius = 1;

				if (diamondradius > 1) pline("The maximum possible radius for the diamond barrier is %d, but you may opt to choose a smaller radius if you want.", diamondradius);

				while (diamondradius > 1) {
					pline("Current diamond barrier radius is %d.", diamondradius);

					if (yn("Reduce the radius by one?") == 'y') diamondradius--;
					else break;
				}

				pline("Using radius %d for the diamond barrier.", diamondradius);

			}

			int k, l;

			for (k = -diamondradius; k <= diamondradius; k++) for(l = -diamondradius; l <= diamondradius; l++) {
				if (!isok(u.ux + k, u.uy + l)) continue;
				if (k == 0 && l == 0) continue; /* don't place wall on your own position --Amy */

				if (isok(u.ux + k, u.uy + l) && (levl[u.ux + k][u.uy + l].typ == ROOM || levl[u.ux + k][u.uy + l].typ == CORR || (levl[u.ux + k][u.uy + l].typ == DOOR && levl[u.ux + k][u.uy + l].doormask == D_NODOOR) ) ) {

					if (levl[u.ux + k][u.uy + l].typ != DOOR) levl[u.ux + k][u.uy + l].typ = GRAVEWALL;
					else doorlockX(u.ux + k, u.uy + l, TRUE);
					blockorunblock_point(u.ux + k, u.uy + l);
					newsym(u.ux + k, u.uy + l);

				}

			}

		}
		vision_recalc(0);

		if (Role_if(PM_WALSCHOLAR)) {
			u.walscholarpass += (rnd(50) + rnd(techlevX(tech_no) * 5) );
			pline("For a while, you can freely pass through grave walls!");
		}

	      t_timeout = Role_if(PM_MASON) ? rnz(1000) : rnz(5000);

	      break;

		case T_STEADY_HAND:
			num = 100 + (techlevX(tech_no) * 5);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your %s become steady.", makeplural(body_part(HAND)));

			t_timeout = rnz(2000);
			break;

		case T_FORCE_FILLING:

			You_feel("full of mystic power!");

			if (!rn2(20)) u.uen += (400 + rnz(techlevX(tech_no)));
			else if (!rn2(5)) u.uen += (d(6,8) + rnz(techlevX(tech_no)));
			else u.uen += (d(5,6) + rnz(techlevX(tech_no)));
			if (u.uen > u.uenmax) u.uen = u.uenmax;

			t_timeout = rnz(1000);
			break;

		case T_JEDI_TAILORING:

			{

			struct obj *uroub;

			uroub = mksobj(rnd_class(ROBE, ROBE_OF_WEAKNESS), TRUE, FALSE, FALSE);

			if (uroub) {
				dropy(uroub);
				stackobj(uroub);
				pline("A robe appeared at your %s!", makeplural(body_part(FOOT)));
			} else pline("For some unknown reason, the tailoring attempt failed.");

			}

			t_timeout = rnz(15000);
			break;

		case T_INTRINSIC_SACRIFICE:

			if (!(uwep && is_lightsaber(uwep) && u.twoweap && uswapwep && is_lightsaber(uswapwep))) {
				pline("That only works if you are dual-wielding lightsabers!");
				break;
			}

			attrcurse();

			if (uwep && is_lightsaber(uwep)) {
				uwep->age += 1000;
				if (uwep->otyp == ORANGE_LIGHTSABER) uwep->age += (1000 * rnd(2));
				if (uwep->oartifact == ART_DESANN_S_WRATH) uwep->age += (1000 * rnd(2));
			}
			if (uswapwep && is_lightsaber(uswapwep)) {
				uswapwep->age += 1000;
				if (uswapwep->otyp == ORANGE_LIGHTSABER) uswapwep->age += (1000 * rnd(2));
				if (uswapwep->oartifact == ART_DESANN_S_WRATH) uswapwep->age += (1000 * rnd(2));
			}

			pline("Lightsaber energy replenished. Did it cost you an important intrinsic?");

			t_timeout = rnz(10000);
			break;

		case T_BEAMSWORD:
			num = 100 + (techlevX(tech_no) * 3);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your lightsaber can fire beams now!");

			t_timeout = rnz(2500);
			break;

		case T_ENERGY_TRANSFER:
			num = 200 + (techlevX(tech_no) * 5);
		    	techt_inuse(tech_no) = num + 1;
			pline("For a while, casting spells will recharge your lightsaber (but it must be lit).");

			t_timeout = rnz(5000);
			break;

		case T_SOFTEN_TARGET:

			if (u.uswallow) {
		    		pline("Softening an engulfer unfortunately doesn't work.");
		    		return(0);
			}

		    	if (!getdir((char *)0)) return(0);
			if (!u.dx && !u.dy) {
				/* Hopefully a mistake ;B */
				pline("You can't soften yourself!");
				return(0);
			}
			if (!isok(u.ux + u.dx, u.uy + u.dy)) {
				pline("Invalid target location.");
				return 0;
			}
			mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
			if (!mtmp || !canspotmon(mtmp)) {
				if (memory_is_invisible(u.ux + u.dx, u.uy + u.dy))
					pline("Can't soften what you can't see!");
				else
					You("don't see anything there!");
				return (0);
			}

			{
				int xtmp = d(2,6);
				pline("%s suddenly seems weaker!", Monnam(mtmp));
				mtmp->mhpmax -= xtmp;
#ifdef SHOW_DMG
				if (xtmp < mtmp->mhp) showdmg(xtmp);
#endif
				if ((mtmp->mhp -= xtmp) <= 0 || !mtmp->m_lev) {
					pline("%s dies!", Monnam(mtmp));
					xkilled(mtmp,0);
				} else {
					mtmp->m_lev--;
					wakeup(mtmp); /* monster becomes hostile */
				}
			}

			t_timeout = rnz(10000);
			break;

		case T_PROTECT_WEAPON:
			if (!uwep || !bimanual(uwep)) {
				pline("This requires you to wield a bimanual weapon, which you currently don't.");
				break;
			}
			uwep->oerodeproof = 1;
			uwep->rknown = 1;
			pline("Alright - your weapon is erosionproof now.");

			t_timeout = rnz(10000);
			break;

		case T_POWERFUL_AURA:
			num = 40 + (techlevX(tech_no) * 3);
		    	techt_inuse(tech_no) = num + 1;
			pline("A powerful aura surrounds you!");

			t_timeout = rnz(3000);
			break;

		case T_BOOSTAFF:
			if (!uwep || weapon_type(uwep) != P_QUARTERSTAFF) {
				pline("How are you going to boost your staff if you don't wield it?");
				break;
			}
			pline("Your staff is surrounded by a shimmering aura.");
			uncurse(uwep, TRUE);

			if (uwep && uwep->spe < 2) {
				uwep->spe++;
			} else if (uwep && uwep->spe < 8 && !rn2(uwep->spe) ) {
				uwep->spe++;
			}

			t_timeout = rnz(6000);
			break;

		case T_CLONE_JAVELIN:
			if (!uwep || weapon_type(uwep) != P_JAVELIN) {
				pline("This requires you to actually wield the javelin that you want to clone.");
				break;
			}
			uwep->quan++;
			uwep->owt = weight(uwep);
			pline("A new javelin is created out of thin air!");
			t_timeout = rnz(7000);
			break;

		case T_REFUGE:
			num = 20 + techlevX(tech_no);
		    	techt_inuse(tech_no) = num + 1;
			pline("Refuge activated!");

			t_timeout = rnz(2400);
			break;

		case T_DRAINING_PUNCH:

	    	if (!canuseunarmedtechs()) {
			You("can't do this while wielding a weapon!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    		return(0);
	    	}
		/* used to be impossible with a shield but that was dumb --Amy */
		if (!getdir((char *)0)) return(0);
		if (!u.dx && !u.dy) {
			You("flex your muscles.");
			return(0);
		}
		if (!isok(u.ux + u.dx, u.uy + u.dy)) {
			pline("Invalid target location.");
			return 0;
		}

            	if (!blitz_draining_punch()) return(0);

			t_timeout = rnz(2000);
			break;

		case T_ESCROBISM:

			if (!uarm) {
				pline("Without armor, that technique won't do anything.");
				break;
			}
			if (uarm && !(uarm->otyp >= ROBE && uarm->otyp <= ROBE_OF_WEAKNESS)) {
				pline("That technique won't work if your worn armor isn't a robe!");
				break;
			}

			num = 50 + (techlevX(tech_no) * 3);
		    	techt_inuse(tech_no) = num + 1;
			pline("Escrobism started - your bare hands or lightsaber will temporarily deal extra damage while you are wearing a robe.");

			t_timeout = rnz(4000);
			break;

		case T_PIRATE_BROTHERING:

			if (!u.twoweap) {
				pline("This won't work if you don't dual-wield!");
				break;
			}
			if (uwep && !is_lightsaber(uwep)) {
				pline("Your primary weapon isn't a lightsaber, and therefore this technique won't work!");
				break;
			}
			if (uswapwep && weapon_type(uswapwep) != P_SCIMITAR) {
				pline("Your secondary weapon isn't a scimitar, and therefore this technique won't work!");
				break;
			}
			if (!uwep || !uswapwep) {
				pline("At least one of your hands isn't wielding any weapon, and therefore this technique won't work!");
				break;
			}

			num = 250 + (techlevX(tech_no) * 10);
		    	techt_inuse(tech_no) = num + 1;
			pline("Alright, your lightsaber loses no energy and can be recharged by hitting things with the scimitar.");

			t_timeout = rnz(5000);
			break;

		case T_NUTS_AND_BOLTS:

			You("make crossbow ammo and increase your satiation.");

			struct obj *uammo;

			uammo = mksobj(CROSSBOW_BOLT, TRUE, FALSE, FALSE);
			if (uammo) {
				uammo->quan = rnd(20 + (2 * techlevX(tech_no)));
				uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
				uammo->owt = weight(uammo);
				dropy(uammo);
				stackobj(uammo);
			}

			lesshungry(200);

			t_timeout = rnz(4000);
			break;

		case T_WONDER_YONDER:

			{
				int inerduration = rnz(1000);
				int nastyfects = rnd(5);
				if (!inertiaprotection()) {
					u.uprops[DEAC_FAST].intrinsic += inerduration;
					u.inertia += inerduration;
					You_feel("lethargic...");
				}
				while (nastyfects > 0) {
					nastyfects--;
					badeffect();
				}

			}
			wonderspell(-1);
			t_timeout = rnz(15000);
			break;

		case T_CARD_TRICK:

			if (CannotSelectItemsInPrompts) break;
cardtrickchoice:
			otmp = getobj(allnoncount, "duplicate");
			if (!otmp) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to duplicate a scroll.");
				else goto cardtrickchoice;
				pline("A feeling of loss comes over you.");
				break;

			}
			if (otmp) {
				if (otmp->oclass != SCROLL_CLASS) {
					pline("That isn't a scroll, and therefore the card trick was wasted.");
					t_timeout = rnz(10000);
					break;
				}
				if (otmp->oartifact) {
					pline("Artifact scrolls cannot be duplicated, and therefore the card trick was wasted.");
					t_timeout = rnz(10000);
					break;
				}

				int cardpotency = 50 + techlevX(tech_no);
				if (rnd(cardpotency) > writecost(otmp)) {
					pline("Success! You managed to duplicate the scroll.");
					otmp->quan++;
					otmp->owt = weight(otmp);
				} else {
					pline("Unfortunately your attempt to duplicate the scroll failed.");
				}

			}
			t_timeout = rnz(10000);
			break;

		case T_SKILLOMORPH:

			additionalskilltraining(rnd(10));
			t_timeout = rnz(10000);
			break;

		case T_SHOTTY_BLAST:
		{

			int ammotype;

			pline("You can choose from these kinds of ammo: shotgun shells or auto shotgun shells.");

			boolean haschosen = FALSE;
			pline("Pick an ammo type that you want to create. The prompt will loop until you actually make a choice.");
			while (!haschosen) {
				if (yn("Do you want to create shotgun shells?") == 'y') {
					haschosen = TRUE;
					ammotype = 2;
				}
				else if (yn("Do you want to create auto shotgun shells?") == 'y') {
					haschosen = TRUE;
					ammotype = 1;
				}
			}

			You("make some shotgun shells.");

			{

			struct obj *uammo;

			if (ammotype == 1) uammo = mksobj(AUTO_SHOTGUN_SHELL, TRUE, FALSE, FALSE);
			else uammo = mksobj(SHOTGUN_SHELL, TRUE, FALSE, FALSE);
			if (uammo) {
				uammo->quan = 5 + (techlevX(tech_no) / 3);
				if (uammo->quan < 1) uammo->quan = 1;
				if (uarmc && uarmc->oartifact == ART_ARABELLA_S_WEAPON_STORAGE) uammo->quan *= 2;
				uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
				uammo->owt = weight(uammo);
				dropy(uammo);
				stackobj(uammo);
			}

			}

			t_timeout = rnz(5000);
			break;

		}
		case T_AMMO_UPGRADE:

			if (!uwep) {
				pline("You're not wielding anything!");
				break;
			}
			if (uwep && !is_bullet(uwep)) {
				pline("You're not wielding firearm ammo!");
				break;
			}
			if (uwep && is_bullet(uwep)) {
				You("try to upgrade your ammo.");
				if (stack_too_big(uwep)) {
					t_timeout = rnz(4000);
					pline("But the stack size was too big, and therefore it failed.");
					break;

				} else {
					if (uwep->cursed) uncurse(uwep, TRUE);
					else if (!uwep->cursed && !uwep->blessed) bless(uwep);
					if (uwep->spe < 2) uwep->spe++;
					else if (uwep->spe < 10 && uwep->spe > 1 && !rn2(uwep->spe)) uwep->spe++;
				}
			}

			t_timeout = rnz(10000);
			break;

		case T_LASER_POWER:
		{
			int ammotype;

			pline("You can choose from these kinds of ammo: blaster bolts, heavy blaster bolts, laser beams, raygun bolts or radios.");

			boolean haschosen = FALSE;
			pline("Pick an ammo type that you want to create. The prompt will loop until you actually make a choice.");
			while (!haschosen) {
				if (yn("Do you want to create blaster bolts?") == 'y') {
					haschosen = TRUE;
					ammotype = 5;
				}
				else if (yn("Do you want to create heavy blaster bolts?") == 'y') {
					haschosen = TRUE;
					ammotype = 4;
				}
				else if (yn("Do you want to create laser beams?") == 'y') {
					haschosen = TRUE;
					ammotype = 3;
				}
				else if (yn("Do you want to create raygun bolts?") == 'y') {
					haschosen = TRUE;
					ammotype = 2;
				}
				else if (yn("Do you want to create radios?") == 'y') {
					haschosen = TRUE;
					ammotype = 1;
				}
				else if (yn("Do you want to create carcosan bolts?") == 'y') {
					haschosen = TRUE;
					ammotype = 6;
				}
			}

			You("make some laser ammo.");

			{

			struct obj *uammo;

			if (ammotype == 5) uammo = mksobj(BLASTER_BOLT, TRUE, FALSE, FALSE);
			else if (ammotype == 4) uammo = mksobj(HEAVY_BLASTER_BOLT, TRUE, FALSE, FALSE);
			else if (ammotype == 3) uammo = mksobj(LASER_BEAM, TRUE, FALSE, FALSE);
			else if (ammotype == 2) uammo = mksobj(RAYGUN_BOLT, TRUE, FALSE, FALSE);
			else if (ammotype == 6) uammo = mksobj(CARCOSAN_BOLT, TRUE, FALSE, FALSE);
			else uammo = mksobj(RADIO, TRUE, FALSE, FALSE);

			if (uammo) {
				uammo->quan = 10 + techlevX(tech_no);
				if (uammo->quan < 1) uammo->quan = 1;
				if (uarmc && uarmc->oartifact == ART_ARABELLA_S_WEAPON_STORAGE) uammo->quan *= 2;
				uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
				uammo->owt = weight(uammo);
				dropy(uammo);
				stackobj(uammo);
			}

			}

			t_timeout = rnz(7500);
			break;

		}
		case T_BIG_DADDY:

			You("make a rocket launcher.");

			{

			struct obj *uammo;

			uammo = mksobj(ROCKET_LAUNCHER, TRUE, FALSE, FALSE);
			if (uammo) {
				uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
				dropy(uammo);
				stackobj(uammo);
			}

			uammo = mksobj(ROCKET, TRUE, FALSE, FALSE);
			if (uammo) {
				uammo->quan = 5 + (techlevX(tech_no) / 3);
				if (uammo->quan < 1) uammo->quan = 1;
				if (uarmc && uarmc->oartifact == ART_ARABELLA_S_WEAPON_STORAGE) uammo->quan *= 2;
				uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
				uammo->owt = weight(uammo);
				dropy(uammo);
				stackobj(uammo);
			}

			}

			t_timeout = rnz(25000);
			break;

		case T_SHUT_THAT_BITCH_UP:

			num = 20 + (techlevX(tech_no) * 3);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your firearms can temporarily shut up bitches upon hitting them.");

			t_timeout = rnz(15000);
			break;

		case T_S_PRESSING:

			if (t_at(u.ux, u.uy)) {
				pline("There is a trap at your location, and therefore the attempt fails!");
				t_timeout = rnz(2000);
				break;
			}
			if (u.uhunger < 500) {
				You("don't have enough nutrition, and therefore the attempt fails!");
				t_timeout = rnz(2000);
				break;
			}
			{
				struct trap *ttrap;
				ttrap = maketrap(u.ux, u.uy, S_PRESSING_TRAP, 0, FALSE);
				if (ttrap && !ttrap->hiddentrap) {
					ttrap->tseen = 1;
					ttrap->madeby_u = 1;
				}
			}
			/* launch_otyp is set in trap.c and controls the trap's strength --Amy */
			You("laid a trap.");
			u.uhunger -= 500; /* not "morehungry", not affected by full nutrients */

			if (practicantterror) {
				pline("%s thunders: 'You shitty practicant lass! Your act of shitting up my laboratory will not be tolerated! Now you pay 5000 zorkmids and clean away your shit, got it?'", noroelaname());
				fineforpracticant(5000, 0, 0);

			}

			{
				register struct monst *nexusmon, *nextmon;

				for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
				    nextmon = nexusmon->nmon;
				    if (DEADMONSTER(nexusmon)) continue;
				    if (canseemon(nexusmon)) nexusmon->spressingseen = TRUE;

				}
			}
			use_skill(P_SQUEAKING, rnd(20));

			t_timeout = rnz(2000);
			break;

		case T_MELTEE:

			{
			int melteestrength = 1;
			if (techlevX(tech_no) > 9) melteestrength += (techlevX(tech_no) / 10);

			if (practicantterror) {
				pline("%s thunders: 'What do you think you're doing here, peeing in my laboratory? That makes 10000 zorkmids, and your mirror is confiscated too to make you hopefully repent your deeds!'", noroelaname());
				fineforpracticant(10000, 0, 0);
				u.uprops[DEAC_REFLECTING].intrinsic += 10000;
			}

		    	techt_inuse(tech_no) = 1;
			buzz(17, melteestrength, u.ux, u.uy, -1, 0);
			buzz(17, melteestrength, u.ux, u.uy, 1, 0);
			buzz(17, melteestrength, u.ux, u.uy, -1, 1);
			buzz(17, melteestrength, u.ux, u.uy, 1, 1);
			buzz(17, melteestrength, u.ux, u.uy, 0, 1);
			buzz(17, melteestrength, u.ux, u.uy, -1, -1);
			buzz(17, melteestrength, u.ux, u.uy, 1, -1);
			buzz(17, melteestrength, u.ux, u.uy, 0, -1);
		    	techt_inuse(tech_no) = 0;

			}

			use_skill(P_SQUEAKING, rnd(50));
			t_timeout = rnz(5000);
			break;

		case T_WOMAN_NOISES:

			You("make very female noises!");
			{
				register struct monst *nexusmon, *nextmon;

				for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
				    nextmon = nexusmon->nmon; /* trap might kill mon */
				    if (DEADMONSTER(nexusmon)) continue;
				    if (nexusmon->mpeaceful || nexusmon->mtame) continue;
				    if (resist(nexusmon, SPBOOK_CLASS, 0, NOTELL)) continue;
				    if (resist(nexusmon, SPBOOK_CLASS, 0, NOTELL)) continue;

				    if (distu(nexusmon->mx, nexusmon->my) > rnd(50)) continue;

					/* doesn't anger peaceful ones - not a bug --Amy */
					pline("%s cannot resist!", Monnam(nexusmon));
					monflee(nexusmon, rnd(20), FALSE, TRUE);
					if (nexusmon->mblinded < 100) nexusmon->mblinded += 20;
					nexusmon->mcansee = 0;
					nexusmon->mstun = TRUE;
					nexusmon->mconf = TRUE;

					if (!resists_drain(nexusmon) && !resist(nexusmon, SPBOOK_CLASS, 0, NOTELL)) {
						pline("%s shudders in dread!", Monnam(nexusmon));
						nexusmon->mhpmax -= rnd(10);
						if (nexusmon->mhpmax < 1) nexusmon->mhpmax = 1;
						if (nexusmon->mhp > nexusmon->mhpmax) nexusmon->mhp = nexusmon->mhpmax;
						nexusmon->m_enmax -= rnd(10);
						if (nexusmon->m_enmax < 0) nexusmon->m_enmax = 0;
						if (nexusmon->m_en > nexusmon->m_enmax) nexusmon->m_en = nexusmon->m_enmax;
						if (nexusmon->m_lev > 0) nexusmon->m_lev--;
						if (nexusmon->m_lev > 0 && !rn2(4)) nexusmon->m_lev--;
					}

				}
			}

			if (practicantterror) {
				pline("%s rings out: 'Your irritating noises are disturbing the other practicants, so here's a warning for you to not do that again and I also receive 50 zorkmids from you.'", noroelaname());
				fineforpracticant(50, 0, 0);
			}

			use_skill(P_SQUEAKING, rnd(50));
			t_timeout = rnz(5000);
			break;

		case T_EXTRA_LONG_SQUEAK:

			num = 20 + techlevX(tech_no);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your butt starts squeaking.");
			use_skill(P_SQUEAKING, rnd(50));
			t_timeout = rnz(5000);
			break;

		case T_SEXUAL_HUG:

			if (u.uswallow) {
				pline("Did you seriously expect to be able to hug a monster that has engulfed you?");
				return 0;
			}

			{

			int k, l, caughtX;
			struct monst *mtmp3;
			caughtX = 0;
			pline("You try to hug the monsters next to you!");

		    for (k = -1; k <= 1; k++) for(l = -1; l <= 1; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;
			if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) && !is_infrastructure_monster(mtmp3) && mtmp3->mtame == 0 && mtmp3->isminion == 0 && mtmp3->isgyp == 0 && !is_rider(mtmp3->data) && !is_deadlysin(mtmp3->data)
			&& mtmp3->mnum != quest_info(MS_NEMESIS) && !(rn2(5) && mtmp3->data->geno & G_UNIQ) && caughtX == 0)

			{

				if (humanoid(mtmp3->data) && !mtmp3->female) {
					pline("%s is infatuated with you and asks to become your slave!", mon_nam(mtmp3));
					(void) tamedog(mtmp3, (struct obj *) 0, TRUE);
					if (techlevX(tech_no) < rnd(100)) caughtX++;
					t_timeout = rnz(20000);
				}

				else pline("%s brushes you away.", mon_nam(mtmp3));

			} /* monster is catchable loop */
		    } /* for loop */

			}

			if (practicantterror) {
				pline("%s thunders: 'You've molested the male practicants! That's 5000 zorkmids on the spot and I told all of them that you're a mean sexual predator. Do that again and you'll receive hall exclusion.'", noroelaname());
				fineforpracticant(5000, 0, 0);
				HateTrapEffect += 10000;
			}

			t_timeout = rnz(20000);
			break;

		case T_SEX_CHANGE:

			Your("gender is changed!");
			change_sex();

			if (practicantterror) {
				pline("%s thunders: 'That type of surgery is dangerous! You've practiced it without adhering to standard safety precautions, meaning there's now a fine of 20000 zorkmids and I'll also collect any money you gain from now on to teach you a lesson!'", noroelaname());
				fineforpracticant(20000, 0, 0);
				BankTrapEffect |= FROMOUTSIDE;
			}

			t_timeout = rnz(10000);
			break;

		case T_EVEN_MORE_AMMO:

			if (!uwep) {
				pline("You're not wielding anything!");
				break;
			}
			if (uwep && !is_bullet(uwep)) {
				pline("You're not wielding firearm ammo!");
				break;
			}
			if (uwep && is_bullet(uwep)) {
				if (uwep->otyp == SMG_BULLET || uwep->otyp == ANTIMATTER_SMG_BULLET || uwep->otyp == MG_BULLET || uwep->otyp == ANTIMATTER_MG_BULLET || uwep->otyp == RIFLE_BULLET || uwep->otyp == ANTIMATTER_RIFLE_BULLET || uwep->otyp == SNIPER_BULLET || uwep->otyp == ANTIMATTER_SNIPER_BULLET || uwep->otyp == ASSAULT_RIFLE_BULLET || uwep->otyp == LOWRATE_ASSAULT_RIFLE_BULLET || uwep->otyp == ANTIMATTER_ASSAULT_RIFLE_BULLE || uwep->otyp == PISTOL_BULLET || uwep->otyp == SILVER_PISTOL_BULLET || uwep->otyp == LEAD_PISTOL_BULLET || uwep->otyp == FIVE_SEVEN_BULLET || uwep->otyp == SHADOW_FIVE_SEVEN_BULLET || uwep->otyp == LEAD_FIVE_SEVEN_BULLET || uwep->otyp == LEAD_SMG_BULLET || uwep->otyp == BRONZE_SMG_BULLET || uwep->otyp == LEAD_RIFLE_BULLET || uwep->otyp == COPPER_RIFLE_BULLET || uwep->otyp == LEAD_MG_BULLET || uwep->otyp == HOLLOW_POINT_MG_BULLET || uwep->otyp == LEAD_SNIPER_BULLET || uwep->otyp == PLATINUM_SNIPER_BULLET || uwep->otyp == LEAD_ASSAULT_RIFLE_BULLET || uwep->otyp == ANTIMATTER_PISTOL_BULLET || uwep->otyp == ANTIMATTER_FIVE_SEVEN_BULLET || uwep->otyp == BLASTER_BOLT || uwep->otyp == RAYGUN_BOLT || uwep->otyp == RADIO || uwep->otyp == HEAVY_BLASTER_BOLT || uwep->otyp == LASER_BEAM) {
					uwep->quan += (20 + techlevX(tech_no));
					pline("The amount of ammo was increased!");
				} else if (uwep->otyp == BFG_AMMO || uwep->otyp == CARCOSAN_BOLT) {
					uwep->quan += (80 + (techlevX(tech_no) * 5));
					pline("The amount of ammo was increased!");
				} else if (uwep->otyp == SHOTGUN_SHELL || uwep->otyp == CHROME_PELLET || uwep->otyp == AUTO_SHOTGUN_SHELL || uwep->otyp == LEAD_SHOT || uwep->otyp == AUTO_LEAD_SHOT || uwep->otyp == COATED_AUTO_SHELL) {
					uwep->quan += (10 + (techlevX(tech_no) / 2));
					pline("The amount of ammo was increased!");
				} else if (uwep->otyp == ROCKET) {
					uwep->quan += (1 + (techlevX(tech_no) / 5));
					pline("The amount of ammo was increased!");
				} else {
					pline("The ammo you're wielding cannot be duplicated. Sorry. Please try some other type of ammo.");
					break;
				}
			}
			t_timeout = rnz(8000);
			break;

		case T_DOUBLESELF:

			if (!Upolyd) {
				pline("That doesn't work if you're not polymorphed.");
				break;
			}
			You("try to clone yourself!");
			cloneu();
			t_timeout = rnz(20000);
			break;

		case T_POLYFIX:

			if (CannotSelectItemsInPrompts) break;
polyfixchoice:
			otmp = getobj(allnoncount, "polyfix");
			if (!otmp) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to polyfix an item.");
				else goto polyfixchoice;
				pline("A feeling of loss comes over you.");
				break;

			}
			if (otmp) {
				if (!is_hazy(otmp)) {
					pline("That item wasn't hazy, and therefore nothing happens.");
				} else if (!stack_too_big(otmp)) {
					stop_timer(UNPOLY_OBJ, (void *) otmp);
					otmp->oldtyp = STRANGE_OBJECT;
					pline("%s is no longer hazy.", Yname2(otmp));
					if (otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(otmp)) {
						if (!otmp->cursed) bless(otmp);
						else uncurse(otmp, FALSE);
					}
				} else pline("The stack was too big, and therefore nothing happens.");

			}

			t_timeout = rnz(5000);
			break;

		case T_SQUEAKY_REPAIR:

			if (CannotSelectItemsInPrompts) break;
			if (t_at(u.ux, u.uy)) {
				pline("There is a trap at your location, and therefore the attempt fails!");
				t_timeout = rnz(8000);
				break;
			}
			{
				struct trap *ttrap;
				ttrap = maketrap(u.ux, u.uy, FART_TRAP, 0, TRUE);
				if (ttrap) {
					dotrap(ttrap, 0);

					pline("You may repair a damaged item.");
repairitemchoice:
					otmp = getobj(allnoncount, "magically repair");
					if (!otmp) {
						if (yn("Really exit with no object selected?") == 'y')
							pline("You just wasted the opportunity to repair your items.");
						else goto repairitemchoice;
						pline("A feeling of loss comes over you.");
						t_timeout = rnz(8000);
						break;
					} else if (stack_too_big(otmp)) {
						pline("The stack was too big, and therefore the attempt failed.");
					} else if (otmp && greatest_erosion(otmp) > 0) {
						pline("Your %s is in perfect condition again!", xname(otmp));
						if (otmp->oeroded > 0) { otmp->oeroded = 0; }
						if (otmp->oeroded2 > 0) { otmp->oeroded2 = 0; }
						if (otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(otmp)) {
							if (!otmp->cursed) bless(otmp);
							else uncurse(otmp, FALSE);
						}

					} else pline("Your %s is still as undamaged as ever.", xname(otmp));

				} else pline("For some unknown reason, the attempt failed.");
			}
			use_skill(P_SQUEAKING, rnd(80));
			t_timeout = rnz(8000);
			break;

		case T_BULLETREUSE:

			num = 100 + (techlevX(tech_no) * 5);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your ammo can temporarily be reused.");
			t_timeout = rnz(10000);
			break;

		case T_TERRAIN_CLEANUP:

			pline("You try to clean up annoying terrain.");
			int maderoomX = 0;

			do_clear_areaX(u.ux, u.uy, 1, terraincleanup, (void *)&maderoomX);

			if (maderoomX) pline("Some annoying terrain was cleaned up!");
			else pline("There was nothing to clean up...");

			t_timeout = rnz(8000);

			break;

		case T_SYMBIOSIS:

			{

			/* symbiant and goauld can use it more often... but they're too different, and therefore
			 * being a goauld symbiant doesn't reduce the timeout by more :P --Amy */

				int symbiotimer = 20000;

				if (!PlayerCannotUseSkills) {
					switch (P_SKILL(P_SYMBIOSIS)) {
						default: break;
						case P_BASIC:
							symbiotimer = 18000;
							break;
						case P_SKILLED:
							symbiotimer = 16000;
							break;
						case P_EXPERT:
							symbiotimer = 14000;
							break;
						case P_MASTER:
							symbiotimer = 12000;
							break;
						case P_GRAND_MASTER:
							symbiotimer = 10000;
							break;
						case P_SUPREME_MASTER:
							symbiotimer = 8000;
							break;
					}

				}

				if (Role_if(PM_SYMBIANT) || Race_if(PM_GOAULD)) symbiotimer /= 2;

				struct obj *usymbioteitem;

				pline("A symbiote is created!");
				usymbioteitem = mksobj(SYMBIOTE, TRUE, FALSE, FALSE);
				if (usymbioteitem) {
					usymbioteitem->quan = 1;
					usymbioteitem->known = usymbioteitem->dknown = usymbioteitem->bknown = usymbioteitem->rknown = 1;
					usymbioteitem->owt = weight(usymbioteitem);
					usymbioteitem->finalcancel = TRUE; /* no polymorphing exploits, sorry --Amy */
					dropy(usymbioteitem);
					stackobj(usymbioteitem);
				}

			t_timeout = rnz(symbiotimer);

			}

			break;

		case T_ADJUST_SYMBIOTE:

			/* intentional that you can use this even while not having a symbiote --Amy */
			if (!PlayerCannotUseSkills && (P_SKILL(P_SYMBIOSIS) >= P_SKILLED)) {

				pline("Currently your symbiote's aggressiveness is %d%%.", u.symbioteaggressivity);

				int lowerbound, higherbound;
				lowerbound = 25;
				higherbound = 25;

				switch (P_SKILL(P_SYMBIOSIS)) {
					case P_SKILLED:
						lowerbound = 20;
						higherbound = 33;
						break;
					case P_EXPERT:
						lowerbound = 15;
						higherbound = 40;
						break;
					case P_MASTER:
						lowerbound = 12;
						higherbound = 50;
						break;
					case P_GRAND_MASTER:
						lowerbound = 10;
						higherbound = 60;
						break;
					case P_SUPREME_MASTER:
						lowerbound = 5;
						higherbound = 75;
						break;
					default:
						lowerbound = 25;
						higherbound = 25;
						break;
				}

				pline("You can set the aggressivity to values between %d%% and %d%% (inclusive).", lowerbound, higherbound);
				if (lowerbound <= 5 && yn("Set the aggressivity to 5%%?") == 'y') {
					u.symbioteaggressivity = 5;
					pline("Your symbiote's aggressivity is 5%% now.");
				} else if (lowerbound <= 10 && yn("Set the aggressivity to 10%%?") == 'y') {
					u.symbioteaggressivity = 10;
					pline("Your symbiote's aggressivity is 10%% now.");
				} else if (lowerbound <= 12 && yn("Set the aggressivity to 12%%?") == 'y') {
					u.symbioteaggressivity = 12;
					pline("Your symbiote's aggressivity is 12%% now.");
				} else if (lowerbound <= 15 && yn("Set the aggressivity to 15%%?") == 'y') {
					u.symbioteaggressivity = 15;
					pline("Your symbiote's aggressivity is 15%% now.");
				} else if (lowerbound <= 20 && yn("Set the aggressivity to 20%%?") == 'y') {
					u.symbioteaggressivity = 20;
					pline("Your symbiote's aggressivity is 20%% now.");
				} else if (yn("Set the aggressivity to 25%%?") == 'y') {
					u.symbioteaggressivity = 25;
					pline("Your symbiote's aggressivity is 25%% now.");
				} else if (higherbound >= 33 && yn("Set the aggressivity to 33%%?") == 'y') {
					u.symbioteaggressivity = 33;
					pline("Your symbiote's aggressivity is 33%% now.");
				} else if (higherbound >= 40 && yn("Set the aggressivity to 40%%?") == 'y') {
					u.symbioteaggressivity = 40;
					pline("Your symbiote's aggressivity is 40%% now.");
				} else if (higherbound >= 50 && yn("Set the aggressivity to 50%%?") == 'y') {
					u.symbioteaggressivity = 50;
					pline("Your symbiote's aggressivity is 50%% now.");
				} else if (higherbound >= 60 && yn("Set the aggressivity to 60%%?") == 'y') {
					u.symbioteaggressivity = 60;
					pline("Your symbiote's aggressivity is 60%% now.");
				} else if (higherbound >= 75 && yn("Set the aggressivity to 75%%?") == 'y') {
					u.symbioteaggressivity = 75;
					pline("Your symbiote's aggressivity is 75%% now.");
				} else pline("Your symbiote's aggressivity remains %d%%.", u.symbioteaggressivity);

			} else pline("Unfortunately your symbiosis skill is too low to use this ability.");

			t_timeout = rnz(500);

			break;

		case T_HEAL_SYMBIOTE:

			if (!uinsymbiosis) {
				pline("You don't have a symbiote, so you can't heal it either!");
				break;
			}
			if (u.uhp < ((u.uhpmax / 2) + 1)) {
				pline("Your health is too low!");
				break;
			}
			if (u.uhpmax < 5) { /* arbitrary */
				pline("Your health is too low!");
				break;
			}
			u.uhp -= (u.uhpmax / 2);
			u.usymbiote.mhp += (u.uhpmax / 2);
			if (u.usymbiote.mhp > u.usymbiote.mhpmax) u.usymbiote.mhp = u.usymbiote.mhpmax;
			flags.botl = TRUE;
			You("sap some of your health, and transfer it to your symbiote!");

			t_timeout = rnz(2000);
			break;

		case T_BOOST_SYMBIOTE:

			if (!uinsymbiosis) {
				pline("You can't boost a nonexistant symbiote!");
				break;
			}

			u.usymbiote.mhpmax += rnd(8);
			maybe_evolve_symbiote();
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
			if (flags.showsymbiotehp) flags.botl = TRUE;
			Your("symbiote's health is improved!");

			t_timeout = rnz(4000);
			break;

		case T_GRENADES_OF_COURSE:

			You("make some grenades.");

			{

			struct obj *uammo;

			if (!rn2(2)) uammo = mksobj(FRAG_GRENADE, TRUE, FALSE, FALSE);
			else uammo = mksobj(GAS_GRENADE, TRUE, FALSE, FALSE);
			if (uammo) {
				uammo->quan = 5 + (techlevX(tech_no) / 5);
				if (Role_if(PM_GRENADONIN)) uammo->quan *= 5;
				if (uammo->quan < 1) uammo->quan = 1;
				if (uarmc && uarmc->oartifact == ART_ARABELLA_S_WEAPON_STORAGE) uammo->quan *= 2;
				uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
				uammo->owt = weight(uammo);
				dropy(uammo);
				stackobj(uammo);
			}

			}

			t_timeout = rnz(20000);
			break;

		case T_PREACHING:
			if (Race_if(PM_MACTHEIST)) {
				pline("As an atheist, you cannot use such a technique.");
				return 0;
			}

			u.uconduct.gnostic++;	/* you just communicated with your god */
			if (Race_if(PM_MAGYAR)) {
				You_feel("bad about breaking the atheist conduct.");
				badeffect();
			}
			use_skill(P_SPIRITUALITY, Role_if(PM_PRIEST) ? 5 : 1);
			adjalign(5 + techlevX(tech_no) / 3);
			You_feel("pious.");
			t_timeout = rnz(4000);
			break;

		case T_ON_THE_SAME_TEAM:

			if (u.uswallow) {
			    pline("That doesn't work while you're engulfed!");
				return 0;
			}

			{

			int k, l;
			struct monst *mtmp3;
			pline("You try to pacify monsters!");

		    for (k = -1; k <= 1; k++) for(l = -1; l <= 1; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;
			if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) && !is_infrastructure_monster(mtmp3) && mtmp3->mtame == 0 && mtmp3->mpeaceful == 0 && mtmp3->mfrenzied == 0 && mtmp3->isminion == 0 && mtmp3->isgyp == 0
			&& mtmp3->mnum != quest_info(MS_NEMESIS) && !resist(mtmp3, RING_CLASS, 0, NOTELL) && !(mtmp3->data->geno & G_UNIQ) )

			{

				if (mtmp3->data->maligntyp == 0) {
					pline("%s looks calmer.", mon_nam(mtmp3));
					mtmp3->mpeaceful = 1;
				}

				else pline("%s can't be pacified with this method!", mon_nam(mtmp3));

			} /* monster is catchable loop */
		    } /* for loop */

			} /* variable definition bracket */

			t_timeout = rnz(6000);
			break;

		case T_CURE_AMNESIA:

			You("decide to cure your amnesia.");

			if (HMap_amnesia & INTRINSIC) {
				HMap_amnesia &= ~INTRINSIC;
				You_feel("less forgetful!");
			}
			if (HMap_amnesia & TIMEOUT) {
				HMap_amnesia &= ~TIMEOUT;
				You_feel("less forgetful!");
			}

			t_timeout = rnz(50000);
			break;

		case T_ELEMENTAL_IMBUE:
		{
			int randomelement = rnd(3);

			if (!uwep) {
				pline("Without a weapon, this technique won't work!");
				return 0;
			}

			if (uwep && (uwep->quan > 1)) {
				pline("You're wielding a stack of weapons! Drop all but one!");
				return 0;
			}

			if (uwep && (uwep->oartifact || uwep->fakeartifact)) {
				pline("Transforming a weapon into an artifact doesn't work on weapons that are already artifacts.");
				return 0;
			}

			if (uwep && (uwep->oclass != WEAPON_CLASS)) {
				pline("That's not a proper weapon, and therefore cannot be transformed into an artifact!");
				return 0;
			}

			if (uwep) {
				switch (randomelement) {
					case 1:
						elemental_imbue(1); /* see artifact.c */
						uwep = onameX(uwep, artiname(ART_FIRE_SWING));
						break;
					case 2:
						elemental_imbue(2);
						uwep = onameX(uwep, artiname(ART_FROST_SWING));
						break;
					case 3:
						elemental_imbue(3);
						uwep = onameX(uwep, artiname(ART_SHOCK_SWING));
						break;
				}

				update_inventory();
				Your("weapon transforms, and is now imbued with elemental power!");
			}

			t_timeout = rnz(20000);

			break;

		}

		case T_SPELL_SPAM:

			num = 100 + (techlevX(tech_no) * 2);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your spells are more powerful for a while.");
			t_timeout = rnz(12000);

			break;

		case T_DROP_BOULDER:

			if (Is_rogue_level(&u.uz) || (In_endgame(&u.uz) && !Is_earthlevel(&u.uz))) {
				pline("That doesn't work in this area.");
				break;
			}

		    	if (In_sokoban(&u.uz) && !playercancheatinsoko()) {
				change_luck(-1);
				pline("You cheater!");
				if (evilfriday) u.ugangr++;
			}

			{
				struct obj *otmp2;
				if (isok(u.ux, u.uy)) {
					otmp2 = mksobj(BOULDER, FALSE, FALSE, FALSE);
					if (otmp2) {
			    	    	    if (!flooreffects(otmp2, u.ux, u.uy, "fall")) {
			    	    	    	place_object(otmp2, u.ux, u.uy);
			    	    	    	stackobj(otmp2);
			    	    	    	newsym(u.ux, u.uy);  /* map the rock */
			    	    	    }
					}
				}
				pline("Klonk!");
			}

			t_timeout = rnz(20000);

			break;

		case T_MARTIAL_STYLE:

			{
				boolean madechoice = 0;

				if (Race_if(PM_SAMEDI)) {
					u.martialstyle = MARTIALSTYLE_CAPOEIRA;
					pline("The Sons of Samedi are permanently locked into the capoeira martial arts style!");
					return 0;
				}

				if (PlayerCannotUseSkills) {
					t_timeout = rnz(8000);
					pline("Somehow, nothing happens.");
					break;
				}

				pline("Your current martial arts style is %s.", currentmartialstyle());

				pline("Choose a martial arts style. The prompt will loop until you actually make a choice.");

				while (madechoice == 0) {

					if (yn("Switch to the 'brawling' style (default martial arts style, no special effects)?")=='y') {
						madechoice = 1; u.martialstyle = MARTIALSTYLE_BRAWLING;
					} else if (P_SKILL(P_MARTIAL_ARTS) >= P_BASIC && yn("Switch to the 'judo' style (break holding attacks, boosts dexterity, more frequency for special martial arts strikes, reduces your damage output)?")=='y') {
						madechoice = 1; u.martialstyle = MARTIALSTYLE_JUDO;
					} else if (P_SKILL(P_MARTIAL_ARTS) >= P_SKILLED && yn("Switch to the 'taekwondo' style (powers up your kick attack, can block projectiles if both of your hands are free, can also be used while wielding a weapon, your melee attacks have reduced to-hit)?")=='y') {
						madechoice = 1; u.martialstyle = MARTIALSTYLE_TAEKWONDO;
					} else if (P_SKILL(P_MARTIAL_ARTS) >= P_EXPERT && yn("Switch to the 'muay thai' style (speeds you up, boosts unarmed damage, makes your AC worse, your spells fail more often and are more expensive, your ranged weapons have reduced to-hit)?")=='y') {
						madechoice = 1; u.martialstyle = MARTIALSTYLE_MUAYTHAI;
					} else if (P_SKILL(P_MARTIAL_ARTS) >= P_MASTER && yn("Switch to the 'silat' style (boosts unarmed to-hit, unarmed attacks can sometimes stun opponents, improves your AC, makes your inventory weigh more and slows you down slightly)?")=='y') {
						madechoice = 1; u.martialstyle = MARTIALSTYLE_SILAT;
					} else if (P_SKILL(P_MARTIAL_ARTS) >= P_GRAND_MASTER && yn("Switch to the 'krav maga' style (greatly boosts unarmed damage, causes aggravate monster, reduces your regeneration rate and can send enemies into a frenzy)?")=='y') {
						madechoice = 1; u.martialstyle = MARTIALSTYLE_KRAVMAGA;
					} else if (P_SKILL(P_QUARTERSTAFF) >= P_MASTER && yn("Switch to the 'bojutsu' style (requires quarterstaff, allows you to block projectiles with the staff, boosts your AC and allows you to sometimes evade rays, reduces your melee damage output)?")=='y') {
						madechoice = 1; u.martialstyle = MARTIALSTYLE_BOJUTSU;
					} else if (P_SKILL(P_MARTIAL_ARTS) >= P_EXPERT && P_SKILL(P_BARE_HANDED_COMBAT) >= P_EXPERT && yn("Switch to the 'kung fu' style (slightly improves your unarmed and kick damage and boosts unarmed to-hit, kick hits enemies more often, your weapons are less effective)?")=='y') {
						madechoice = 1; u.martialstyle = MARTIALSTYLE_KUNGFU;
					} else if (uwep && uwep->otyp == JEONTU_GEOM && yn("Switch to the 'haidong gumdo' style (requires jeontu geom, boosts melee damage and to-hit and allows you to use martial arts techniques while wielding a jeontu geom, reduces your melee to-hit whenever you're not wielding a jeontu geom)?")=='y') {
						madechoice = 1; u.martialstyle = MARTIALSTYLE_HAIDONGGUMDO;
					} else if (P_SKILL(P_MARTIAL_ARTS) >= P_EXPERT && P_SKILL(P_GENERAL_COMBAT) >= P_EXPERT && yn("Switch to the 'karate' style (higher unarmed to-hit, kick cannot be clumsy and not be dodged, can occasionally avoid bad effects, aggravates monsters, reduces your ability to use weapons)?")=='y') {
						madechoice = 1; u.martialstyle = MARTIALSTYLE_KARATE;
					} else if (P_SKILL(P_MARTIAL_ARTS) >= P_EXPERT && P_SKILL(P_BODY_ARMOR) >= P_EXPERT && yn("Switch to the 'marshal arts' style (armor and shield don't hinder your martial prowess, reduced dexterity)?")=='y') {
						madechoice = 1; u.martialstyle = MARTIALSTYLE_MARSHALARTS;
					} else if (P_SKILL(P_TWO_WEAPON_COMBAT) >= P_SKILLED && (P_SKILL(P_MARTIAL_ARTS) >= P_SKILLED || P_SKILL(P_BARE_HANDED_COMBAT) >= P_SKILLED) && yn("Switch to the 'wrestling' style (dual-wielding with at least one hand free occasionally paralyzes enemies, better STR/DEX, less INT/WIS, magic vacuum, fuck over and killer spawns)?")=='y') {
						madechoice = 1; u.martialstyle = MARTIALSTYLE_WRESTLING;
					} else if (yn("Do you choose to not switch your style after all?")=='y') {
						madechoice = 1;
					}
					/* capoeira cannot be selected, this is on purpose --Amy */

				}

				pline("Your new martial arts style is %s.", currentmartialstyle());
			}

			t_timeout = rnz(8000);

			break;

		case T_SPRINT:
			if (!PlayerInStilettoHeels) {
				pline("That doesn't work without stiletto heels!");
				break;
			}

			num = 50 + (techlevX(tech_no) * 2);
		    	techt_inuse(tech_no) = num + 1;
			pline("You start sprinting with your heels.");

			t_timeout = rnz(10000);
			break;
		case T_SKULL_CRUSH:
			if (!PlayerInStilettoHeels) {
				pline("That doesn't work without stiletto heels!");
				break;
			}

			num = 20;
		    	techt_inuse(tech_no) = num + 1;
			pline("Quick! Paralyze an opponent and then kick him to death! You have 20 turns to do so!");

			t_timeout = rnz(50000);
			break;
		case T_FEMALE_COMBO:
			if (!PlayerInConeHeels) {
				pline("That doesn't work without cone heels!");
				break;
			}

			num = 50 + (techlevX(tech_no));
		    	techt_inuse(tech_no) = num + 1;
			pline("You start your female combo. Now make sure you kick an enemy every turn to keep the combo going!");

			u.femcombostrike = 2;
			u.femcomboactive = TRUE;

			t_timeout = rnz(5000);
			break;
		case T_DOCKLOCK:
		{
			struct obj *uammo;

			num = 100 + (techlevX(tech_no) * 3);
		    	techt_inuse(tech_no) = num + 1;
			pline("Yay, now you can bash enemies with a sexy leather pump! (Be sure to pick it up from the floor.)");

			uammo = mksobj(SEXY_LEATHER_PUMP, TRUE, FALSE, FALSE);
			if (uammo) {
				uammo->quan = 1;
				uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
				uammo->spe = rnd(7);
				uammo->cursed = uammo->hvycurse = uammo->stckcurse = uammo->prmcurse = uammo->bbrcurse = uammo->morgcurse = uammo->evilcurse = FALSE;
				uammo->owt = weight(uammo);
				dropy(uammo);
				stackobj(uammo);
			}

			t_timeout = rnz(20000);
		}
			break;
		case T_BITCH_POSING:
			if (!PlayerInBlockHeels) {
				pline("That doesn't work without block heels!");
				break;
			}

			if (uarmf->spe < 0) {
				int sperepair = abs(uarmf->spe);
				sperepair = rnd(sperepair);
				uarmf->spe += sperepair;
				Your("heels got repaired!");
			} else {
				if (uarmf && uarmf->spe < 2) {
					uarmf->spe++;
				} else if (uarmf && uarmf->spe < 8 && !rn2(uarmf->spe) ) {
					uarmf->spe++;
				}
				Your("heels shine for a moment.");
			}

		{
			int i, j;
			struct monst *rainedmon;

			for (i = -3; i <= 3; i++) for(j = -3; j <= 3; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;

				if ( (rainedmon = m_at(u.ux + i, u.uy + j)) != 0) {
					if (rainedmon->mpeaceful || rainedmon->mtame || rainedmon->mfrenzied) continue;
					if (resist(rainedmon, WEAPON_CLASS, 0, NOTELL) && resist(rainedmon, WEAPON_CLASS, 0, NOTELL) && resist(rainedmon, WEAPON_CLASS, 0, NOTELL) ) continue;
					rainedmon->mpeaceful = TRUE;
					pline("%s cannot resist your sexy pose and is charmed!", Monnam(rainedmon));
				}

			}

		}

			t_timeout = rnz(8000);
			break;
		case T_SEXY_STAND:
			if (!PlayerInBlockHeels) {
				pline("That doesn't work without block heels!");
				break;
			}

			num = 200 + (techlevX(tech_no) * 10);
		    	techt_inuse(tech_no) = num + 1;
			pline("You perform a sexy stand.");

			t_timeout = rnz(20000);
			break;
		case T_MARATHON:
			if (!PlayerInWedgeHeels) {
				pline("That doesn't work without wedge heels!");
				break;
			}

			num = 1000 + (techlevX(tech_no) * 10);
		    	techt_inuse(tech_no) = num + 1;
			pline("You're starting to run a marathon.");

			t_timeout = rnz(30000);
			break;
		case T_PERFUME_STRIDE:
			if (!PlayerInWedgeHeels) {
				pline("That doesn't work without wedge heels!");
				break;
			}
			if (!uarmf) {
				pline("Where the hell are your shoes???"); /* shouldn't happen, but just making sure --Amy */
				break;
			}

			num = 500 - (techlevX(tech_no) * 5);
			if (num < 50) num = 50;
		    	techt_inuse(tech_no) = num + 1;
			pline("A perfume companion starts to follow you, but now you're obligated to wear your heels for a while.");

			curse(uarmf);
			uarmf->hvycurse = uarmf->stckcurse = TRUE;

		{
			int attempts = 0;
			register struct permonst *ptrZ;
			register struct monst *bossmon;

perfumestriding:

			do {

				ptrZ = rndmonst();
				attempts++;
				if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!ptrZ || (ptrZ && !(ptrZ->msound == MS_STENCH))) && attempts < 50000);

			if (ptrZ && ptrZ->msound == MS_STENCH) {
				bossmon = makemon(ptrZ, u.ux, u.uy, NO_MM_FLAGS);
			}
			else if (rn2(50)) {
				attempts = 0;
				goto perfumestriding;
			}

			if (bossmon) {
				tamedog(bossmon, (struct obj *) 0, TRUE);
			}
			u.mondiffhack = 0;

		}

			t_timeout = rnz(16000);
			break;
		case T_NAUGHTY_HEELOT:

			num = 1000 + (techlevX(tech_no) * 10);
		    	techt_inuse(tech_no) = num + 1;
			pline("You become the naughty heelot.");

			if (!PlayerInConeHeels && !PlayerInStilettoHeels) pline("But without the proper shoes, that won't do jack squat.");
			else if (!PlayerInConeHeels || !PlayerInStilettoHeels) pline("Seems like your current footwear isn't eligible for that, though, so you'd better find the right pair, fast.");
			/* still allows the player to use the tech, this is not a mistake --Amy */

			t_timeout = rnz(10000);
			break;
		case T_EXTREME_STURDINESS:
			if (!PlayerInWedgeHeels && !PlayerInBlockHeels) {
				pline("You need to either wear wedge or block heels for that!");
				break;
			}

			num = 50 + (techlevX(tech_no));
		    	techt_inuse(tech_no) = num + 1;
			pline("You become extremely sturdy!");

			t_timeout = rnz(20000);
			break;
		case T_BUTT_PROTECTION:
			if (!PlayerInConeHeels) {
				pline("That doesn't work without cone heels!");
				break;
			}

			num = 50 + (techlevX(tech_no) * 2);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your butt is protected now.");

			t_timeout = rnz(10000);
			break;
		case T_PROFILING:
			if (!PlayerInBlockHeels) {
				pline("That doesn't work without block heels!");
				break;
			}
			if (!uarmf) {
				pline("Where the hell are your shoes???"); /* shouldn't happen, but just making sure --Amy */
				break;
			}

			if (uarmf->obrittle >= 3 || uarmf->obrittle2 >= 3) {
				pline("Your pair of heels is maximally profiled already! Any more and you'd risk breaking them!");
				break;
			}
			if (uarmf->spe >= 5) {
				pline("It wouldn't make sense to add more profiling to this pair of boots.");
				break;
			}
			uarmf->spe++;
			if (rn2(2)) uarmf->obrittle++;
			else uarmf->obrittle2++;
			pline("You added some profiling to your boots.");

			t_timeout = rnz(8000);
			break;
		case T_STAR_HEEL_SWAP:
			if (!PlayerInWedgeHeels) {
				pline("That doesn't work without wedge heels!");
				break;
			}
			if (!uarmf) {
				pline("Where the hell are your shoes???"); /* shouldn't happen, but just making sure --Amy */
				break;
			}
			if (!uwep) {
				pline("That doesn't work without a weapon!");
				return 0;
			}
			if (uwep && weapon_type(uwep) != P_MORNING_STAR) {
				pline("That only works if your wielded weapon is a morning star, and currently it's not!");
				return 0;
			}

		{
			int knifeenchant = uwep->spe;
			int heelsenchant = uarmf->spe;
			int knifebuc = 0;
			int heelsbuc = 0;

			/* now for a system that will make every C programmer's skin crawl... :-P --Amy */
			if (uwep->blessed) knifebuc = 1;
			if (uwep->cursed) knifebuc = 2;
			if (uwep->hvycurse) knifebuc = 3;
			if (uwep->prmcurse) knifebuc = 4;
			if (uwep->stckcurse) knifebuc += 10;
			if (uwep->bbrcurse) knifebuc += 100;
			if (uwep->morgcurse) knifebuc += 1000;
			if (uwep->evilcurse) knifebuc += 10000;

			if (uarmf->blessed) heelsbuc = 1;
			if (uarmf->cursed) heelsbuc = 2;
			if (uarmf->hvycurse) heelsbuc = 3;
			if (uarmf->prmcurse) heelsbuc = 4;
			if (uarmf->stckcurse) heelsbuc += 10;
			if (uarmf->bbrcurse) heelsbuc += 100;
			if (uarmf->morgcurse) heelsbuc += 1000;
			if (uarmf->evilcurse) heelsbuc += 10000;

			uwep->spe = heelsenchant;
			uarmf->spe = knifeenchant;

			uwep->blessed = uwep->cursed = uwep->hvycurse = uwep->prmcurse = uwep->stckcurse = uwep->morgcurse = uwep->bbrcurse = uwep->evilcurse = 0;
			uarmf->blessed = uarmf->cursed = uarmf->hvycurse = uarmf->prmcurse = uarmf->stckcurse = uarmf->morgcurse = uarmf->bbrcurse = uarmf->evilcurse = 0;

			if (knifebuc == 1) uarmf->blessed = TRUE;
			else {
				if (knifebuc >= 10000) {
					knifebuc -= 10000;
					uarmf->cursed = uarmf->hvycurse = uarmf->prmcurse = uarmf->evilcurse = TRUE;
				}
				if (knifebuc >= 1000) {
					knifebuc -= 1000;
					uarmf->cursed = uarmf->hvycurse = uarmf->prmcurse = uarmf->morgcurse = TRUE;
				}
				if (knifebuc >= 100) {
					knifebuc -= 100;
					uarmf->cursed = uarmf->hvycurse = uarmf->prmcurse = uarmf->bbrcurse = TRUE;
				}
				if (knifebuc >= 10) {
					knifebuc -= 10;
					uarmf->cursed = uarmf->stckcurse = TRUE;
				}
				if (knifebuc >= 2) {
					uarmf->cursed = TRUE;
					if (knifebuc >= 3) uarmf->hvycurse = TRUE;
					if (knifebuc >= 4) uarmf->prmcurse = TRUE;
				}
			}

			if (heelsbuc == 1) uwep->blessed = TRUE;
			else {
				if (heelsbuc >= 10000) {
					heelsbuc -= 10000;
					uwep->cursed = uwep->hvycurse = uwep->prmcurse = uwep->evilcurse = TRUE;
				}
				if (heelsbuc >= 1000) {
					heelsbuc -= 1000;
					uwep->cursed = uwep->hvycurse = uwep->prmcurse = uwep->morgcurse = TRUE;
				}
				if (heelsbuc >= 100) {
					heelsbuc -= 100;
					uwep->cursed = uwep->hvycurse = uwep->prmcurse = uwep->bbrcurse = TRUE;
				}
				if (heelsbuc >= 10) {
					heelsbuc -= 10;
					uwep->cursed = uwep->stckcurse = TRUE;
				}
				if (heelsbuc >= 2) {
					uwep->cursed = TRUE;
					if (heelsbuc >= 3) uwep->hvycurse = TRUE;
					if (heelsbuc >= 4) uwep->prmcurse = TRUE;
				}
			}

			pline("Enchantments and BUC of your knife and heels have been swapped.");

		}

			t_timeout = rnz(9000);
			break;
		case T_HEEL_STAB:
			if (!PlayerInStilettoHeels) {
				pline("That doesn't work without stiletto heels!");
				break;
			}
			if (!uwep) {
				pline("That doesn't work without a weapon!");
				return 0;
			}
			if (uwep && weapon_type(uwep) != P_KNIFE) {
				pline("That only works if your wielded weapon is a knife, and currently it's not!");
				return 0;
			}

			num = 100 + (techlevX(tech_no) * 2);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your heel-shaped knife can stab enemies much more effectively now.");

			t_timeout = rnz(6000);
			break;

		case T_EARTHSHAKE:

			if (Is_rogue_level(&u.uz) || (In_endgame(&u.uz) && !Is_earthlevel(&u.uz))) {
				pline("That doesn't work in this area.");
				break;
			}

		    	if (In_sokoban(&u.uz) && !playercancheatinsoko()) {
				change_luck(-1);
				pline("You cheater!");
				if (evilfriday) u.ugangr++;
			}

			{
			    	register int x, y;
				struct obj *otmp2;

				int earthradius = 1;

				for (x = u.ux-earthradius; x <= u.ux+earthradius; x++) {
			    	    for (y = u.uy-earthradius; y <= u.uy+earthradius; y++) {
					if (isok(x, y) && !IS_ROCK(levl[x][y].typ)) {
						otmp2 = mksobj(BOULDER, FALSE, FALSE, FALSE);
						if (otmp2) {
				    	    	    if (!flooreffects(otmp2, x, y, "fall")) {
				    	    	    	place_object(otmp2, x, y);
				    	    	    	stackobj(otmp2);
				    	    	    	newsym(x, y);  /* map the rock */
				    	    	    }
						}
					}

				    }
				}
				pline("Several boulders fall.");
			}

			t_timeout = rnz(40000);

			break;

		case T_HARDCORE_ALIENIZATION:
		{
			register struct obj *shemagh;

			if ((find_shemagh()) != -1) {
				shemagh = mksobj(find_shemagh(), TRUE, FALSE, FALSE);

				if (shemagh) {

					(void) pickup_object(shemagh, shemagh->quan, TRUE, TRUE);

					if (shemagh) {

						if (is_helmet(shemagh)) {
							if (uarmh) remove_worn_item(uarmh, TRUE);
							setworn(shemagh, W_ARMH);
							Helmet_on();
							if (shemagh) curse(shemagh);
							You("are wearing a shemagh now.");
						} else {
							pline("Somehow, no shemagh was created!");
						}

					} else pline("Somehow, it failed.");
				} else pline("Something has gone wrong...");
			}

		}		

			t_timeout = rnz(20000);

			break;

		case T_PERMAMORPH:

		    	if (!getdir((char *)0)) return 0;
			if (!u.dx && !u.dy) {
			    pline("This technique is for polymorphing monsters, not yourself.");
			    return 0;
			}
			if (!isok(u.ux + u.dx, u.uy + u.dy)) {
				pline("Invalid target location.");
				return 0;
			}
			mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
			if (!mtmp) {
				You("were such a noob and wasted this powerful technique by aiming it at thin air! Why didn't you target a monster instead?");
			} else if (!is_infrastructure_monster(mtmp) && mtmp->isminion == 0 && mtmp->isgyp == 0
			&& mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) ) {
				if (!resist(mtmp, WAND_CLASS, 0, NOTELL)) {
					(void) mon_spec_polyX(mtmp, (struct permonst *)0, 0L, FALSE, TRUE, FALSE, FALSE);
				} else {
					pline("%s resists! Hahahahahahaha! Now you can wait for an eternity before the technique becomes available again.", Monnam(mtmp));
				}
			} else {
				pline("What a shame - you apparently targetted a monster that is immune, and therefore nothing happens.");
			}

		      t_timeout = rnz(75000);

			break;

		case T_PACIFY:
		    	if (!getdir((char *)0)) return 0;
			if (!u.dx && !u.dy) {
			    pline("You cannot pacify yourself.");
			    return 0;
			}
			if (!isok(u.ux + u.dx, u.uy + u.dy)) {
				pline("Invalid target location.");
				return 0;
			}
			mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
			if (!mtmp) {
				pline("There is no monster in that direction. Nothing happens.");
			} else if (mtmp->mtame == 0 && !is_infrastructure_monster(mtmp) && mtmp->mpeaceful == 0 && mtmp->mfrenzied == 0 && mtmp->isminion == 0 && mtmp->isgyp == 0
			&& mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) ) {
				if (!resist(mtmp, WAND_CLASS, 0, NOTELL)) {
					int pacifychance = 10;
					if (!PlayerCannotUseSkills) {
						switch (P_SKILL(P_ORB)) {
							default: break;
							case P_BASIC:
							pacifychance += 5;
							break;
						case P_SKILLED:
							pacifychance += 10;
							break;
						case P_EXPERT:
							pacifychance += 15;
							break;
						case P_MASTER:
							pacifychance += 20;
							break;
						case P_GRAND_MASTER:
							pacifychance += 25;
							break;
						case P_SUPREME_MASTER:
							pacifychance += 30;
							break;
						}
					}
					if (u.ualign.record < -20) pacifychance -= 50;
					else if (u.ualign.record < -10) pacifychance -= 20;
					else if (u.ualign.record < 0) pacifychance -= 10;
					else if (u.ualign.record > 4 && u.ualign.record < 10) pacifychance += 5;
					else if (u.ualign.record > 9 && u.ualign.record < 14) pacifychance += 10;
					else if (u.ualign.record > 14 && u.ualign.record < 20) pacifychance += 15;
					else if (u.ualign.record > 19 && u.ualign.record < 40) pacifychance += 20;
					else if (u.ualign.record > 39 && u.ualign.record < 60) pacifychance += 30;
					else if (u.ualign.record > 59 && u.ualign.record < 80) pacifychance += 40;
					else if (u.ualign.record > 79 && u.ualign.record < 100) pacifychance += 50;
					else if (u.ualign.record > 99 && u.ualign.record < 120) pacifychance += 60;
					else if (u.ualign.record > 119 && u.ualign.record < 140) pacifychance += 70;
					else if (u.ualign.record > 139) pacifychance += 80;

					if (rn2(100) < pacifychance) {
						mtmp->mpeaceful = 1;
						pline("%s looks calmer.", mon_nam(mtmp));
					} else {
						pline("%s resists the attempt.", Monnam(mtmp));
					}

				} else {
					pline("%s is as angry as before.", Monnam(mtmp));
				}
			} else {
				pline("That monster is immune.");
			}

		      t_timeout = rnz(5000);
			break;

		case T_AFTERBURNER:
			num = 5 + (techlevX(tech_no) / 5);
		    	techt_inuse(tech_no) = num + 1;
			pline("You've activated your afterburner.");
		      t_timeout = rnz(6000);
			break;

		case T_BUGGARD:
			num = 20 + techlevX(tech_no);
		    	techt_inuse(tech_no) = num + 1;
			pline("You've summoned a blizzard!");
		      t_timeout = rnz(8000);
			break;

		case T_THUNDERSTORM:
			num = 60 + (techlevX(tech_no) * 20);
		    	techt_inuse(tech_no) = num + 1;
			pline("You've conjured a thunderstorm!");
		      t_timeout = rnz(12000);
			break;

		case T_AUTOKILL:

			{
			int melteestrength = 5 + (techlevX(tech_no) / 4);

			buzz(15, melteestrength, u.ux, u.uy, -1, 0);
			buzz(15, melteestrength, u.ux, u.uy, 1, 0);
			buzz(15, melteestrength, u.ux, u.uy, -1, 1);
			buzz(15, melteestrength, u.ux, u.uy, 1, 1);
			buzz(15, melteestrength, u.ux, u.uy, 0, 1);
			buzz(15, melteestrength, u.ux, u.uy, -1, -1);
			buzz(15, melteestrength, u.ux, u.uy, 1, -1);
			buzz(15, melteestrength, u.ux, u.uy, 0, -1);

			}
		      t_timeout = rnz(10000);

			break;

		case T_CHAIN_THUNDERBOLT:

			{
			int melteestrength = 5 + (techlevX(tech_no) / 4);
			int ctx, cty;
			ctx = u.ux, cty = u.uy;

		    	if (!getdir((char *)0)) return(0);
			if (!u.dx && !u.dy) {
				You("really shouldn't be aiming that at yourself.");
				return(0);
			}
			for(i = 0; i < 8; i++) {
				if (!isok(ctx + u.dx, cty + u.dy)) break;
				if (levl[ctx + u.dx][cty + u.dy].typ < POOL) break;

				ctx += u.dx;
				cty += u.dy;
			}

			if (ctx == u.ux && cty == u.uy) {
				pline("The effect dissipated because you didn't aim properly!");
				return(0);
			}
			if (!isok(ctx, cty)) { /* shouldn't happen */
				pline("The effect dissipated because it travelled off the map.");
				return(0);
			}

			buzz(15, melteestrength, ctx, cty, -1, 0);
			buzz(15, melteestrength, ctx, cty, 1, 0);
			buzz(15, melteestrength, ctx, cty, -1, 1);
			buzz(15, melteestrength, ctx, cty, 1, 1);
			buzz(15, melteestrength, ctx, cty, 0, 1);
			buzz(15, melteestrength, ctx, cty, -1, -1);
			buzz(15, melteestrength, ctx, cty, 1, -1);
			buzz(15, melteestrength, ctx, cty, 0, -1);
			}

		      t_timeout = rnz(10000);
			break;

		case T_FLASHING_MISCHIEF:

			{
			int melteestrength = 5 + (techlevX(tech_no) / 4);
			int ctx, cty;
			ctx = u.ux, cty = u.uy;

		    	if (!getdir((char *)0)) return(0);
			if (!u.dx && !u.dy) {
				You("really shouldn't be aiming that at yourself.");
				return(0);
			}
			for(i = 0; i < 8; i++) {
				if (!isok(ctx + u.dx, cty + u.dy)) break;
				if (levl[ctx + u.dx][cty + u.dy].typ < POOL) break;

				ctx += u.dx;
				cty += u.dy;

				if (isok(ctx, cty)) {
					switch (rnd(8)) {
						case 1:
							buzz(15, melteestrength, ctx, cty, -1, 0);
							break;
						case 2:
							buzz(15, melteestrength, ctx, cty, 1, 0);
							break;
						case 3:
							buzz(15, melteestrength, ctx, cty, -1, 1);
							break;
						case 4:
							buzz(15, melteestrength, ctx, cty, 1, 1);
							break;
						case 5:
							buzz(15, melteestrength, ctx, cty, 0, 1);
							break;
						case 6:
							buzz(15, melteestrength, ctx, cty, -1, -1);
							break;
						case 7:
							buzz(15, melteestrength, ctx, cty, 1, -1);
							break;
						case 8:
							buzz(15, melteestrength, ctx, cty, 0, -1);
							break;

					}
				}
			}

			}

		      t_timeout = rnz(10000);
			break;

		case T_SWORD_ART:
			if (!uwep) {
				pline("That requires you to wield a weapon!");
				return 0;

			}

			if (uwep && weapon_type(uwep) != P_SHORT_SWORD && weapon_type(uwep) != P_BROAD_SWORD && weapon_type(uwep) != P_LONG_SWORD && weapon_type(uwep) != P_TWO_HANDED_SWORD) {
				pline("That only works if your wielded weapon is a short, broad, long or two-handed sword, and currently it's not!");
				return 0;
			}

			if (uwep && uwep->spe < 7) {
				uwep->spe++;
                   		pline("Your weapon glows %s.", hcolor(NH_LIGHT_BLUE));
			} else pline("Your currently wielded weapon can't be enchanted any further with this technique, and therefore nothing happens.");

		      t_timeout = rnz(20000);
			break;

		case T_FIRM_CUDGEL:

			if (!uwep) {
				pline("That requires you to wield a weapon!");
				return 0;

			}

			if (uwep && weapon_type(uwep) != P_MACE && weapon_type(uwep) != P_FLAIL && weapon_type(uwep) != P_CLUB && weapon_type(uwep) != P_HAMMER) {
				pline("That only works if your wielded weapon is a mace, flail, club or hammer, and currently it's not!");
				return 0;
			}

			if (uwep) {
				uwep->oerodeproof = uwep->rknown = TRUE;
				uwep->oeroded = uwep->oeroded2 = 0;
				Your("weapon is surrounded by a shimmering shield!");
			}

		      t_timeout = rnz(30000);
			break;

		case T_KAMEHAMEHA:

			{
			int melteestrength = rnd(20 + techlevX(tech_no));
			if (uwep && uwep->oartifact == ART_CAMOHAMEHA) melteestrength *= 2;
			int ctx, cty;
			ctx = u.ux, cty = u.uy;

		    	if (!getdir((char *)0)) return(0);
			if (!u.dx && !u.dy) {
				You("can't direct that at yourself.");
				return(0);
			}
			verbalize("Kamehameha!");

			for(i = 0; i < ((uwep && uwep->oartifact == ART_CAMOHAMEHA) ? 13 : 8); i++) {
				if (!isok(ctx + u.dx, cty + u.dy)) break;
				if (levl[ctx + u.dx][cty + u.dy].typ < POOL) break;

				ctx += u.dx;
				cty += u.dy;

				mtmp = m_at(ctx, cty);

				if (mtmp) {
					pline("The projectile hits %s!", mon_nam(mtmp));
					wakeup(mtmp); /* turn the monster hostile */
					hurtmon(mtmp, melteestrength);
					break;
				}

			}

			}

		      t_timeout = rnz(2000);
			break;

		case T_SHADOW_MANTLE:

			litroomlite(FALSE);

			{

		    int i, j, bd = 2;
		    struct monst *mtmp;

		    for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) {
				wakeup(mtmp); /* monster becomes hostile */
				mtmp->mcansee = 0;
				mtmp->mblinded = rnd(20 + techlevX(tech_no));
				pline("%s is blinded!", Monnam(mtmp));
			}

		    }

			}

		      t_timeout = rnz(5000);

			break;

		case T_VACUUM_STAR:

			You("create throwing stars.");
			{
			struct obj *uammo;

			uammo = mksobj(SHURIKEN, TRUE, FALSE, FALSE);
			if (uammo) {
				uammo->quan = rnd(40 + (3 * techlevX(tech_no)));
				uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
				uammo->owt = weight(uammo);
				dropy(uammo);
				stackobj(uammo);
			}

			}

		      t_timeout = rnz(12000);

			break;

		case T_BLADE_SHIELD:
			num = 20 + techlevX(tech_no);
		    	techt_inuse(tech_no) = num + 1;
			pline("Blade shield activated.");
		      t_timeout = rnz(3000);
			break;

		case T_GREEN_WEAPON:

			if (!uwep) {
				You("can't use this technique without a weapon!");
				return(0);
			}

			if (uwep && weapon_type(uwep) != P_CLAW) {
				pline("That only works if your wielded weapon is a claw, and currently it's not!");
				return 0;
			}

			uwep->opoisoned = TRUE;
			uwep->superpoison = TRUE;
			Your("claw became green.");

		      t_timeout = rnz(10000);
			break;

		case T_HIDDEN_POWER:

			{
				struct obj *hiddenpwitem;
				int hiddenpowertype;

				hiddenpowertype = POT_HEALING; /* fail safe */

				switch (u.hiddenpowerskill) {
					case P_DAGGER:
						hiddenpowertype = WAN_INERTIA; break;
					case P_KNIFE:
						hiddenpowertype = WAN_STONING; break;
					case P_AXE:
						hiddenpowertype = SCR_TAMING; break;
					case P_PICK_AXE:
						hiddenpowertype = WAN_DISINTEGRATION; break;
					case P_SHORT_SWORD:
						hiddenpowertype = POT_BENEFICIAL_EFFECT; break;
					case P_BROAD_SWORD:
						hiddenpowertype = POT_GAIN_LEVEL; break;
					case P_LONG_SWORD:
						hiddenpowertype = WAN_THUNDER; break;
					case P_TWO_HANDED_SWORD:
						hiddenpowertype = MAGIC_HARP; break;
					case P_SCIMITAR:
						hiddenpowertype = WAN_INCREASE_MAX_HITPOINTS; break;
					case P_SABER:
						hiddenpowertype = WAN_REMOVE_CURSE; break;
					case P_CLUB:
						hiddenpowertype = POT_HEROISM; break;
					case P_PADDLE:
						hiddenpowertype = WAN_TIME; break;
					case P_MACE:
						hiddenpowertype = WAN_NETHER_BEAM; break;
					case P_MORNING_STAR:
						hiddenpowertype = POT_RESTORE_ABILITY; break;
					case P_FLAIL:
						hiddenpowertype = WAN_INFERNO; break;
					case P_HAMMER:
						hiddenpowertype = WAN_GRAVITY_BEAM; break;
					case P_QUARTERSTAFF:
						hiddenpowertype = SCR_POWER_CHARGING; break;
					case P_ORB:
						hiddenpowertype = POT_RECOVERY; break;
					case P_CLAW:
						hiddenpowertype = WAN_TRAP_DISARMING; break;
					case P_GRINDER:
						hiddenpowertype = SCR_REPAIR_ITEM; break;
					case P_POLEARMS:
						hiddenpowertype = WAN_PARALYSIS; break;
					case P_SPEAR:
						hiddenpowertype = WAN_ICE_BEAM; break;
					case P_JAVELIN:
						hiddenpowertype = POT_WATER; break;
					case P_TRIDENT:
						hiddenpowertype = WAN_DISINTEGRATION_BEAM; break;
					case P_LANCE:
						hiddenpowertype = POT_GAIN_HEALTH; break;
					case P_BOW:
						hiddenpowertype = SCR_RANDOM_ENCHANTMENT; break;
					case P_SLING:
						hiddenpowertype = SCR_REGULAR_MATERIAL; break;
					case P_FIREARM:
						hiddenpowertype = SCR_TRAP_DISARMING; break;
					case P_CROSSBOW:
						hiddenpowertype = SCR_GREATER_ENCHANT_WEAPON; break;
					case P_DART:
						hiddenpowertype = EXPENSIVE_CAMERA; break;
					case P_SHURIKEN:
						hiddenpowertype = WAN_CHROMATIC_BEAM; break;
					case P_BOOMERANG:
						hiddenpowertype = SCR_CREATE_FAMILIAR; break;
					case P_WHIP:
						hiddenpowertype = SCR_CURE; break;
					case P_UNICORN_HORN:
						hiddenpowertype = SCR_TERRAFORMING; break;
					case P_LIGHTSABER:
						hiddenpowertype = SCR_SECURE_CURSE_REMOVAL; break;
					case P_ATTACK_SPELL:
						hiddenpowertype = SCR_SKILL_GROWTH; break;
					case P_HEALING_SPELL:
						hiddenpowertype = POT_FULL_HEALING; break;
					case P_DIVINATION_SPELL:
						hiddenpowertype = SCR_SECURE_IDENTIFY; break;
					case P_ENCHANTMENT_SPELL:
						hiddenpowertype = WAN_HYPER_BEAM; break;
					case P_PROTECTION_SPELL:
						hiddenpowertype = SCR_EBB_TIDE; break;
					case P_BODY_SPELL:
						hiddenpowertype = WAN_TOXIC; break;
					case P_OCCULT_SPELL:
						hiddenpowertype = WAN_IDENTIFY; break;
					case P_ELEMENTAL_SPELL:
						hiddenpowertype = SCR_SUPERIOR_MATERIAL; break;
					case P_CHAOS_SPELL:
						hiddenpowertype = TINNING_KIT; break;
					case P_MATTER_SPELL:
						hiddenpowertype = WAN_SLUDGE; break;
					case P_BARE_HANDED_COMBAT:
						hiddenpowertype = SCR_SKILL_UP; break;
					case P_HIGH_HEELS:
						hiddenpowertype = TWELVE_COURSE_DINNER; break;
					case P_GENERAL_COMBAT:
						hiddenpowertype = CRYSTAL_BALL; break;
					case P_SHIELD:
						hiddenpowertype = SCR_REVERSE_IDENTIFY; break;
					case P_BODY_ARMOR:
						hiddenpowertype = SCR_GREATER_ENCHANT_ARMOR; break;
					case P_TWO_HANDED_WEAPON:
						hiddenpowertype = CAN_OF_GREASE; break;
					case P_POLYMORPHING:
						hiddenpowertype = SCR_HYBRIDIZATION; break;
					case P_DEVICES:
						hiddenpowertype = SCR_INVENTORY_ID; break;
					case P_SEARCHING:
						hiddenpowertype = STETHOSCOPE; break;
					case P_SPIRITUALITY:
						hiddenpowertype = SCR_CONSECRATION; break;
					case P_PETKEEPING:
						hiddenpowertype = DARK_MAGIC_WHISTLE; break;
					case P_MISSILE_WEAPONS:
						hiddenpowertype = WAN_ENTRAPPING; break;
					case P_TECHNIQUES:
						hiddenpowertype = HUGE_CHUNK_OF_MEAT; break;
					case P_IMPLANTS:
						hiddenpowertype = SCR_ARMOR_SPECIALIZATION; break;
					case P_SEXY_FLATS:
						hiddenpowertype = POT_TRAINING; break;
					case P_MEMORIZATION:
						hiddenpowertype = SCR_GAIN_MANA; break;
					case P_GUN_CONTROL:
						hiddenpowertype = WAN_DEATH; break;
					case P_SQUEAKING:
						hiddenpowertype = SHADOW_HORN; break;
					case P_SYMBIOSIS:
						hiddenpowertype = SCR_SYMBIOSIS; break;
					case P_SHII_CHO:
						hiddenpowertype = POT_RANDOM_INTRINSIC; break;
					case P_MAKASHI:
						hiddenpowertype = CHOCOLATE; break;
					case P_SORESU:
						hiddenpowertype = WAN_AURORA_BEAM; break;
					case P_ATARU:
						hiddenpowertype = WAN_BANISHMENT; break;
					case P_SHIEN:
						hiddenpowertype = WAN_TIME_STOP; break;
					case P_DJEM_SO:
						hiddenpowertype = POT_CURE_INSANITY; break;
					case P_NIMAN:
						hiddenpowertype = POT_DOWN_LEVEL; break;
					case P_JUYO:
						hiddenpowertype = SCR_ERASURE; break;
					case P_VAAPAD:
						hiddenpowertype = SCR_ARTIFACT_CREATION; break;
					case P_WEDI:
						hiddenpowertype = WAN_CLONE_MONSTER; break;
					case P_STILETTO_HEELS:
						hiddenpowertype = WAN_STERILIZE; break;
					case P_CONE_HEELS:
						hiddenpowertype = POT_INVULNERABILITY; break;
					case P_BLOCK_HEELS:
						hiddenpowertype = SCR_WONDER; break;
					case P_WEDGE_HEELS:
						hiddenpowertype = SCR_RETURN; break;
					case P_BOULDER_THROWING:
						hiddenpowertype = SCR_BOSS_COMPANION; break;
					case P_MARTIAL_ARTS:
						hiddenpowertype = WAN_CHARGING; break;
					case P_TWO_WEAPON_COMBAT:
						hiddenpowertype = POT_TECH_LEVEL_UP; break;
					case P_RIDING:
						hiddenpowertype = WAN_FULL_HEALING; break;
					case P_VENOM:
						hiddenpowertype = WAN_VENOM_SCATTERING; break;
				}

				pline("A hidden item is created!");
				hiddenpwitem = mksobj(hiddenpowertype, TRUE, FALSE, FALSE);
				if (hiddenpwitem) {
					hiddenpwitem->quan = 1;
					hiddenpwitem->known = hiddenpwitem->dknown = hiddenpwitem->bknown = hiddenpwitem->rknown = 1;
					hiddenpwitem->owt = weight(hiddenpwitem);
					dropy(hiddenpwitem);
					stackobj(hiddenpwitem);
				}

			}


		      t_timeout = rnz(50000);
			break;

		case T_LIGHTER_BALLS:

			if (!uwep) {
				You("can't use this technique without a weapon!");
				return(0);
			}

			if (uwep && uwep->oclass != BALL_CLASS) {
				You("aren't wielding a ball!");
				return(0);
			}

			num = 1000 + (techlevX(tech_no) * 10);
		    	techt_inuse(tech_no) = num + 1;
			Your("balls get lighter.");
			if (FunnyHallu) pline("Shame, now no girl will want to kick them because it wouldn't hurt you anymore.");

		      t_timeout = rnz(50000);
			break;

		case T_ROCK_TO_POISON:

			if (CannotSelectItemsInPrompts) break;
rockpoisonchoice:
			otmp = getobj(allnoncount, "turn into poison");
			if (!otmp) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to definalize an item.");
				else goto rockpoisonchoice;
				pline("A feeling of loss comes over you.");
				break;

			}
			if (otmp) {
				if (otmp->otyp != ROCK) {
					pline("That's not a rock, and therefore nothing happens.");
				} else if (otmp->oartifact) {
					pline("You knew that this wasn't gonna work on an artifact, so why do you try anyway?");
				} else if (!stack_too_big(otmp)) {
					otmp->otyp = ACID_VENOM;
					otmp->oclass = VENOM_CLASS;
					pline("Success! You've concocted some venom.");
				} else pline("The stack was too big, and therefore nothing happens.");

			}
		      t_timeout = rnz(25000);

			break;

		case T_INJECTION:

			if (u.uswallow) {
		    		pline("Not enough room. Get out of the swallowing monster first.");
		    		return(0);
			}

		    	if (!getdir((char *)0)) return(0);
			if (!u.dx && !u.dy) {
				/* Hopefully a mistake ;B */
				pline("Nah, you're not gonna do that, it'd be too painful.");
				return(0);
			}
			if (!isok(u.ux + u.dx, u.uy + u.dy)) {
				pline("Invalid target location.");
				return 0;
			}
			mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
			if (!mtmp) {
				pline("There was no monster at that location, and therefore your poisoned injection goes to waste!");
			      t_timeout = rnz(2500);
				break;
			}

			You("inject the poison into %s's %s...", mon_nam(mtmp), mbodypart(mtmp, BLOOD));
			{
				int injectiondmg = rn1(10, 6);
				injectiondmg += techlevX(tech_no);
				if (!resist(mtmp, VENOM_CLASS, 0, NOTELL)) {
					pline("%s's nerves are poisoned!", Monnam(mtmp));
					if (!dmgtype(mtmp->data, AD_PLYS)) {
						mtmp->mcanmove = 0;
						mtmp->mfrozen = rnd(4);
						mtmp->mstrategy &= ~STRAT_WAITFORU;
					}
				}
				if (!resists_poison(mtmp) || player_will_pierce_resistance()) {
					if (!rn2(500) && !resists_poison(mtmp) ) {
						Your("poison was deadly...");
						injectiondmg = (mtmp->mhp * 10);
					}
					if (injectiondmg > 0) {
						pline("%s is green now!", Monnam(mtmp));
						(void) resist(mtmp, VENOM_CLASS, injectiondmg, NOTELL);
					}
				} else pline("%s resisted the poison.", Monnam(mtmp));
			}

		      t_timeout = rnz(2500);
			break;

		case T_SUPER_POISON:

			if (!uwep) {
				pline("That doesn't work without a weapon!");
				return 0;
			}
			if (uwep && uwep->oclass != VENOM_CLASS) {
				pline("That only works if your wielded weapon is a venom, and currently it's not!");
				return 0;
			}

			if (uwep && stack_too_big(uwep)) {
				pline("You were trying to enchant too many venoms at once and therefore it failed.");
			} else if (uwep && uwep->spe < 25) {
				uwep->spe += rnd(4);
				pline("The venoms have been enchanted.");
			} else {
				pline("Unfortunately the venoms cannot be enchanted beyond the current enchantment level.");
			}

		      t_timeout = rnz(10000);
			break;

		case T_GREEN_MISSILE:

			num = 10 + (techlevX(tech_no) / 3);
		    	techt_inuse(tech_no) = num + 1;
			pline("You've readied your special wood that you can make into poisoned missiles. Launch them by throwing venoms.");

		      t_timeout = rnz(5000);
			break;

		case T_BIG_N_VEINY:

			num = 100 + (techlevX(tech_no));
		    	techt_inuse(tech_no) = num + 1;
			pline("Now your body is particularly poisonous!");

		      t_timeout = rnz(8000);
			break;

		case T_PLANT_TERROR:

			u.wormpolymorph = PM_ORIGINAL_MESHERA_PLANT;
			polyself(FALSE);
			if (u.mtimedone && (u.umonnum == PM_ORIGINAL_MESHERA_PLANT) ) {
				u.mtimedone = 100 + (techlevX(tech_no));
				pline("The Human World does not care about the Plant World. So you'll teach them.");

				mtmp = makemon(&mons[rn2(5) ? PM_LITTLE_POISON_IVY : PM_SWEET_ASIAN_POISON_IVY], u.ux, u.uy, MM_ADJACENTOK);
				if (mtmp) (void) tamedog(mtmp, (struct obj *) 0, TRUE);
				mtmp = makemon(&mons[rn2(5) ? PM_LITTLE_POISON_IVY : PM_SWEET_ASIAN_POISON_IVY], u.ux, u.uy, MM_ADJACENTOK);
				if (mtmp) (void) tamedog(mtmp, (struct obj *) 0, TRUE);
				if (!rn2(2)) {
					mtmp = makemon(&mons[rn2(5) ? PM_LITTLE_POISON_IVY : PM_SWEET_ASIAN_POISON_IVY], u.ux, u.uy, MM_ADJACENTOK);
					if (mtmp) (void) tamedog(mtmp, (struct obj *) 0, TRUE);
					if (!rn2(2)) {
						mtmp = makemon(&mons[rn2(5) ? PM_LITTLE_POISON_IVY : PM_SWEET_ASIAN_POISON_IVY], u.ux, u.uy, MM_ADJACENTOK);
						if (mtmp) (void) tamedog(mtmp, (struct obj *) 0, TRUE);
					}
				}

			} else pline("Somehow, you failed to transform into a plant. Maybe you can't polymorph?");

		      t_timeout = rnz(50000);
			break;

		case T_POISON_PEN_LETTER:

			num = 20 + (techlevX(tech_no) / 2);
		    	techt_inuse(tech_no) = num + 1;
			pline("You're planning to write poison pen letters.");

		      t_timeout = rnz(8000);
			break;

		case T_CORONATION_CULMINATION:
			nivellate();
			goodeffect();
		      t_timeout = rnz(15000);
			break;

		case T_RESTORE_LIFE_LEVELS:

			if (u.ulevel < u.ulevelmax) {
				int attempts = 0;
				while ((u.ulevel < u.ulevelmax) && (attempts++ < 10000)) {
					gainlevelmaybe();
				}
				Your("level has been restored.");
			} else {
				pline("Whoops! Apparently your experience level hasn't been drained, and therefore it can't be restored to its previous value either. Too bad.");
			}

		      t_timeout = rnz(20000);
			break;

		case T_TOILET_VISIT:

			antjeminigame();
			u.uhpmax++;
			if (Upolyd) u.mhmax++;
			Your("health is increased by one.");
			make_sick(0L, (char *) 0, TRUE, SICK_ALL);
			flags.botl = TRUE;

		      t_timeout = rnz(5000);
			break;

		case T_MAKE_A_WISH:

			{
				struct obj *uammo;
				struct obj *yourobj;
				uammo = mkobj(RANDOM_CLASS, TRUE, FALSE);
				char buf[BUFSZ];
				int attempts;
				long oldgold;

				if (!uammo) {
					pline("Something must have gone wrong. Sorry!");
				      t_timeout = rnz(100000);
					break;
				}

				You("have to take a guess! What's the item that was generated?");

				for (attempts = 0; attempts <= 5; attempts++) {

					if (attempts >= 5) {
						pline("%s", thats_enough_tries);
						goto mkwsh_end;
					}

					getlin("Your guess:", buf);

					if (buf[0] == 0) continue;
					oldgold = u.ugold;
					yourobj = readobjnam(buf, (struct obj *)0, TRUE, FALSE);
					if (u.ugold != oldgold) {
pline("Don't you date cheat me again! -- Your fault!");
						/* Make them pay */
						u.ugold = oldgold / 2;
						continue;
					}
					if (yourobj == &zeroobj || yourobj == (struct obj *) 0) {
						pline("That doesn't exist.");
						continue;
					}

					if (yourobj && (yourobj->otyp == uammo->otyp)) {
						uammo->owt = weight(uammo);
						dropy(uammo);
						stackobj(uammo);
						pline("Yes!!! You successfully guessed the object! It can now be found on the ground.");
						break;
					} else if (yourobj && (yourobj->oclass == uammo->oclass)) {
						bad_artifact();
						pline("Sorry, you had the right item class but not the right type. The item we were looking for was %s.", xname(uammo));
						delobj(uammo);
						break;

					} else {
						bad_equipment(0);
						pline("Sorry, your guess was completely wrong. The item we were looking for was %s.", xname(uammo));
						delobj(uammo);
						break;
					}

				}

			}
mkwsh_end:
		      t_timeout = rnz(100000);
			break;

		case T_VENOM_MIXING:

			{

				struct obj *uammo;
				uammo = mksobj(rn2(2) ? BLINDING_VENOM : ACID_VENOM, TRUE, FALSE, FALSE);
				if (uammo) {
					uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
					uammo->quan = 30 + techlevX(tech_no);
					uammo->owt = weight(uammo);
					dropy(uammo);
					stackobj(uammo);
					pline("A stack of venoms has been dropped on the ground.");
				}

			}

		      t_timeout = rnz(15000);
			break;

		case T_JAVELIN_FORGING:

			{

				struct obj *uammo;
				uammo = mksobj(rnd_class(JAVELIN, STACK_JAVELIN), TRUE, FALSE, FALSE);
				if (uammo) {
					uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
					dropy(uammo);
					stackobj(uammo);
					pline("A javelin was forged!");
				}

			}

		      t_timeout = rnz(10000);
			break;

		case T_BALLSLIFF:

			if (!uwep) {
				You("can't use this technique without a weapon!");
				return(0);
			}

			if (uwep && uwep->oclass != BALL_CLASS) {
				You("aren't wielding a ball!");
				return(0);
			}

			You("try to repair your ball...");
			if (!uwep->oeroded && !uwep->oeroded2) pline("But it wasn't damaged to begin with.");

			if (uwep->oeroded > 0) {
				uwep->oeroded--;
			}
			if (uwep->oeroded2 > 0) {
				uwep->oeroded2--;
			}

		      t_timeout = rnz(20000);

			break;

		case T_POLE_MELEE:
			num = 100 + (techlevX(tech_no) * 5);
			if (Race_if(PM_HUMANOID_CENTAUR)) num *= 10;
		    	techt_inuse(tech_no) = num + 1;
			pline("For a while, your polearms and lances are effective in melee range.");
		      t_timeout = rnz(5000);
			break;

		case T_CHOP_CHOP:

			if (!uwep) {
				You("can't use this technique without a weapon!");
				return(0);
			}

			if (uwep && weapon_type(uwep) != P_AXE) {
				pline("That only works if your wielded weapon is an axe, and currently it's not!");
				return 0;
			}

			{
				register struct monst *nexusmon, *nextmon;

				for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
				    nextmon = nexusmon->nmon; /* trap might kill mon */
				    if (DEADMONSTER(nexusmon)) continue;

				    if (!monnear(nexusmon, u.ux, u.uy)) continue;
					You("chop %s!", mon_nam(nexusmon));
					wakeup(nexusmon); /* monster becomes hostile */
					if (bigmonst(nexusmon->data)) hurtmon(nexusmon, (objects[uwep->otyp].oc_wldam) + ((uwep->spe > 0) ? uwep->spe : 0));
					else hurtmon(nexusmon, (objects[uwep->otyp].oc_wsdam) + ((uwep->spe > 0) ? uwep->spe : 0));
				}
			}

		      t_timeout = rnz(7500);
			break;

		case T_BANISHMENT:
		    	if (!getdir((char *)0)) return 0;
			if (!u.dx && !u.dy) {
			    pline("Nah. You can't use that on yourself. You'll have to target a monster instead.");
			    return 0;
			}
			if (!isok(u.ux + u.dx, u.uy + u.dy)) {
				pline("Invalid target location.");
				return 0;
			}
			mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
			if (!mtmp) {
				pline("That was a waste, man. The target square didn't contain a monster!");
			} else {
				u_teleport_monB(mtmp, TRUE);
			}

		      t_timeout = rnz(50000);

			break;

		case T_PARTICIPATION_LOSS:

		    	if (!getdir((char *)0)) return 0;
			if (!u.dx && !u.dy) {
				char buf[BUFSZ];
			    pline("Do you REALLY want to do that???");
				getlin ("If you answer yes now, you lose the game! [yes/no]",buf);
				(void) lcase (buf);
				if (!(strcmp (buf, "yes"))) {
					u.youaredead = 1;
					u.youarereallydead = 1;
					killer = "being wiped from existence";
					killer_format = KILLED_BY;
					done(DIED);
					pline("Unfortunately, you still don't exist anymore.");
					done(DIED);
					done(ESCAPED); /* you don't get to escape from this death! --Amy */

				}

			}
			if (!isok(u.ux + u.dx, u.uy + u.dy)) {
				pline("Invalid target location.");
				return 0;
			}
			mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
			if (!mtmp) {
				pline("That was a waste, man. The target square didn't contain a monster!");
			} else if (mtmp->isminion == 0 && !is_infrastructure_monster(mtmp) && mtmp->isgyp == 0
			&& mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) ) {
				mongone(mtmp);
				pline("*poof*");
			} else {
				pline("Dude you're such a noob who apparently doesn't read descriptions or you'd have known that this particular monster is immune.");
			}

		      t_timeout = rnz(250000);

			break;

		case T_WEAPON_BLOCKER:
			if (!u.twoweap || !uwep || !uswapwep) {
				pline("You need to be wielding two weapons at the same time for that!");
				return(0);
			}

			num = 100 + (techlevX(tech_no) * 5);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your weapons become capable of blocking attacks.");
		      t_timeout = rnz(3000);
			break;

		case T_EXTRA_MEMORY:

			extramemory(); /* spell.c */
		      t_timeout = rnz(20000);
			break;

		case T_GRAP_SWAP:
			num = 1000 + (techlevX(tech_no) * 50);
		    	techt_inuse(tech_no) = num + 1;
			pline("For a while, your grinders will work like lances while your lances will work like grinders.");
		      t_timeout = rnz(7000);
			break;

		case T_DIABOLIC_MINION:
		t_timeout = rnz(10000);

		{
			struct permonst *shoe = 0;
			int attempts = 0;
			struct monst *shoemonst;

			do {
				shoe = rndmonst();
				attempts++;
				if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!shoe || (shoe && !is_diablomonster(shoe))) && attempts < 50000);

			if (shoe && is_diablomonster(shoe) ) {
				pline("You summoned some diabolic monster!");
				shoemonst = makemon(shoe, u.ux, u.uy, NO_MM_FLAGS);
				if (shoemonst) (void) tamedog(shoemonst, (struct obj *) 0, TRUE);
			} else if (shoe) {
				pline("Some weirdness caused a different monster to appear. It's not what you expected.");
				shoemonst = makemon(shoe, u.ux, u.uy, NO_MM_FLAGS);
				if (shoemonst) (void) tamedog(shoemonst, (struct obj *) 0, TRUE);
			} else pline("It seems that the world of Sanctuary has run out of monsters.");
		}
			u.mondiffhack = 0;

			break;

		case T_UNDERTOW:
			if (!getdir((char *)0)) return 0;
			if (u.uswallow) {
				pline("You're way too busy right now!");
				return (0);
			}
			if (!isok(u.ux + u.dx, u.uy + u.dy)) {
				pline("It seems that you're in a location where you're not allowed to be...");
				return (0);
			}
			if (!isok(u.ux + u.dx, u.uy + u.dy)) {
				pline("You try to drown an invisible barrier, which fails (predictably).");
				/* make the player pay for the attempt --Amy */
				t_timeout = rnz(8000);
				break;
			}
			if (levl[u.ux][u.uy].typ != POOL && levl[u.ux][u.uy].typ != MOAT && levl[u.ux][u.uy].typ != WATER && levl[u.ux][u.uy].typ != WATERTUNNEL && levl[u.ux][u.uy].typ != CRYSTALWATER && levl[u.ux + u.dx][u.uy + u.dy].typ != POOL && levl[u.ux + u.dx][u.uy + u.dy].typ != MOAT && levl[u.ux + u.dx][u.uy + u.dy].typ != WATER && levl[u.ux + u.dx][u.uy + u.dy].typ != WATERTUNNEL && levl[u.ux + u.dx][u.uy + u.dy].typ != CRYSTALWATER) {
				pline("You can't drown someone here!");
				return (0);
			}

			mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
			if (!mtmp) {
				You("grasp at nothing.");
				/* splicehack would return 0 here, but I won't let you use the tech to
				 *  scan the area for monsters --Amy */
				t_timeout = rnz(8000);
				break;
			}
			if (!sticks(mtmp->data)) {
				u.ustuck = mtmp;
				pline("You wrap up %s with your tail and pull him/her/them/it into the depths with you!", Monnam(mtmp));
				if (!is_swimmer(mtmp->data) && !amphibious(mtmp->data)) {
						You("attempt to drown %s...", mon_nam(mtmp));
						hurtmon(mtmp, (!rn2(5) && (techlevX(tech_no) > mtmp->m_lev) && (techlevX(tech_no) >= 10)) ? mtmp->mhp : (rnd(techlevX(tech_no)) * 5) );
				} else {
						pline("%s does not drown!", Monnam(mtmp));
				}
				if (mtmp && !DEADMONSTER(mtmp)) wakeup(mtmp); /* monster becomes hostile */
			} else {
				You("cannot get a grip on %s!", mon_nam(mtmp));
			}

			t_timeout = rnz(8000);
			break;

		case T_DEFINALIZE:

			if (CannotSelectItemsInPrompts) break;
definalizechoice:
			otmp = getobj(allnoncount, "definalize");
			if (!otmp) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to definalize an item.");
				else goto definalizechoice;
				pline("A feeling of loss comes over you.");
				break;

			}
			if (otmp) {
				if (!otmp->finalcancel) {
					pline("That item wasn't finalized, and therefore nothing happens.");
				} else if (!stack_too_big(otmp)) {
					otmp->finalcancel = FALSE;
					pline("%s is no longer finalized.", Yname2(otmp));
					if (otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(otmp)) {
						if (!otmp->cursed) bless(otmp);
						else uncurse(otmp, FALSE);
					}
				} else pline("The stack was too big, and therefore nothing happens.");

			}

			t_timeout = rnz(50000);
			break;

		case T_ANTI_INERTIA:
			if (!u.inertia) You("weren't slowed by inertia and therefore nothing happens.");
			else {
				if (u.inertia < 200) {
					u.inertia -= 100;
					if (u.inertia < 0) u.inertia = 0;
					Your("inertia counter was reduced by 100.");
					if (!u.inertia) pline("You've been cured of inertia.");
				} else if (u.inertia < 1000) {
					u.inertia /= 2;
					Your("inertia counter was reduced, and amounts to %d turns now.", u.inertia);
				} else {
					u.inertia -= 500;
					Your("inertia counter was reduced by 500.");
				}
			}
			if (u.inertia < 0) u.inertia = 0; /* fail safe */

			t_timeout = rnz(5000);
			break;

		case T_POWERBIOSIS:

			if (!uactivesymbiosis) {
				pline("Without a symbiote, this technique won't do anything.");
				break;
			}

			num = 50 + (techlevX(tech_no) * 2);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your symbiote becomes super-powerful for a while!");

			t_timeout = rnz(6000);
			break;

		case T_IMPLANTED_SYMBIOSIS:

			if (!uactivesymbiosis && !uimplant) {
				pline("You know that this technique requires both an implant and a symbiote. You currently have neither, and therefore very obviously nothing happens!");
				break;
			}
			if (!uactivesymbiosis) {
				pline("While you do have an implant, you lack a symbiote! Get one first if you want to use this technique!");
				break;
			}
			if (!uimplant) {
				pline("No implant found! You must equip one if you want to use this technique!");
				break;
			}

			num = 1000 + (techlevX(tech_no) * 50);
		    	techt_inuse(tech_no) = num + 1;
			pline("Your implant boosts your symbiote!");

			t_timeout = rnz(7500);
			break;

		case T_ASSUME_SYMBIOTE:

			if (!uactivesymbiosis) {
				pline("What are you trying to do, polymorph into your symbiote while not actually having one?");
				if (FunnyHallu) pline("If you thought this would turn you into a missingno, bad luck - the dev team thinks of everything.");
				break;
			}

			You_feel("one with your symbiote.");
			u.wormpolymorph = u.usymbiote.mnum;

			polyself(FALSE);

		      t_timeout = rnz(10000);
			break;

		case T_GENERATE_OFFSPRING:

			if (!uactivesymbiosis) {
				pline("This requires a symbiote.");
				break;
			}

			{
				struct obj *uegg;

				You("lay a symbiote egg.");
				uegg = mksobj(EGG, FALSE, FALSE, FALSE);
				if (uegg) {
					uegg->spe = 1; /* "laid by you" */
					uegg->quan = 1;
					uegg->owt = weight(uegg);
					uegg->corpsenm = egg_type_from_parent(u.usymbiote.mnum, FALSE);
					uegg->known = uegg->dknown = 1;
					attach_egg_hatch_timeout(uegg);
					dropy(uegg);
					stackobj(uegg);
				}
			}

		      t_timeout = rnz(10000);

			break;

		case T_EXTRACHARGE:

			if (CannotSelectItemsInPrompts) break;
extrachargechoice:
			otmp = getobj(allnoncount, "extracharge");
			if (!otmp) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to extracharge a wand.");
				else goto extrachargechoice;
				pline("A feeling of loss comes over you.");
				t_timeout = rnz(20000);
				break;
			}
			if (otmp) {
				if (otmp->oclass != WAND_CLASS) {
					pline("You selected something that isn't a wand, and therefore the charge dissipates.");
					t_timeout = rnz(20000);
					break;
				} else {
					switch (otmp->otyp) {

						case WAN_WISHING:
						case WAN_CHARGING:
						case WAN_ACQUIREMENT:
						case WAN_GAIN_LEVEL:
						case WAN_INCREASE_MAX_HITPOINTS:
							pline("That wand is too powerful, and therefore the charge dissipates.");
							t_timeout = rnz(20000);
							break;
						default:
							if (otmp->spe > 125) { /* very unlikely to ever happen --Amy */
								pline("That wand already has the maximum amount of charges stored.");
							} else {
								otmp->spe++;
								pline("Success! Your wand gained an additional charge.");
								if (otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(otmp)) {
									if (!otmp->cursed) bless(otmp);
									else uncurse(otmp, FALSE);
								}
							}
					}
				}
			}

			t_timeout = rnz(20000);
			break;

		case T_ZAP_EM:

			{

			if (u.uswallow) {
				pline("You can't tame an engulfer with this technique.");
				return 0;
			}

			int k, l, caughtX;
			struct monst *mtmp3;
			caughtX = 0;
			pline("You try to tame evil monsters!");

			    for (k = -1; k <= 1; k++) for(l = -1; l <= 1; l++) {
				if (!isok(u.ux + k, u.uy + l)) continue;
				if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) && !is_infrastructure_monster(mtmp3) && mtmp3->mtame == 0 && mtmp3->isminion == 0 && mtmp3->isgyp == 0
			&& mtmp3->mnum != quest_info(MS_NEMESIS) && !(mtmp3->data->geno & G_UNIQ) && caughtX == 0)

				{

					if (is_evilpatchmonster(mtmp3->data)) {
						if (!(resist(mtmp3, RING_CLASS, 0, NOTELL) && resist(mtmp3, RING_CLASS, 0, NOTELL))) {
							pline("%s is successfully tamed!", mon_nam(mtmp3));
							(void) tamedog(mtmp3, (struct obj *) 0, TRUE);
							if (techlevX(tech_no) < rnd(100)) caughtX++;
							t_timeout = rnz(4000);

						} else if ( ((rnd(30 - ACURR(A_CHA))) < 4) && (!resist(mtmp3, RING_CLASS, 0, NOTELL) || !resist(mtmp3, RING_CLASS, 0, NOTELL)) ) {
							pline("%s is successfully tamed!", mon_nam(mtmp3));
							(void) tamedog(mtmp3, (struct obj *) 0, TRUE);
							if (techlevX(tech_no) < rnd(100)) caughtX++;
							t_timeout = rnz(4000);
						} else {
							pline("%s resists the taming attempt!", mon_nam(mtmp3));
						}
					} else pline("%s cannot be tamed by this method!", mon_nam(mtmp3));

				} /* monster is catchable loop */
			    } /* for loop */

			}

			t_timeout = rnz(4000);
			break;

		case T_DECAPABILITY:
			num = 50 + (techlevX(tech_no) * 3);
		    	techt_inuse(tech_no) = num + 1;
			pline("Polearms and lightsabers can temporarily put monsters to sleep.");

			t_timeout = rnz(2500);
			break;

		case T_NO_HANDS_CURSE:

			if (!uwep || !bimanual(uwep)) {
				pline("That requires you to wield a two-handed weapon.");
				break;
			}

			if (uwep && uwep->cursed) {
				pline("Your weapon is already cursed, and therefore nothing happens!");
				uwep->bknown = TRUE;
				t_timeout = rnz(600);
				break;
			}

			if (uwep && uwep->spe < -20) {
				pline("Your weapon is in such a bad condition that this technique doesn't work!");
				t_timeout = rnz(600);
				break;
			}

			if (uwep) {
				curse(uwep);
				uwep->hvycurse = TRUE;
				if (uwep->spe > 0) uwep->spe = 0;
				uwep->spe -= rne(2);
				if (uwep->spe < -120) uwep->spe = -120;
				uwep->bknown = TRUE;

				pline("Your weapon is heavily cursed, but you receive some benefits.");
				change_luck(1);
				adjalign(100);
				if (u.ugangr > 0) u.ugangr--;
				u.ublesscnt -= 500;
				if (u.ublesscnt < 0) u.ublesscnt = 0;
			}

			{
			struct monst *mtmp3;
			int k, l;

			for (k = -1; k <= 1; k++) for(l = -1; l <= 1; l++) {
				if (!isok(u.ux + k, u.uy + l)) continue;
				if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) && !is_infrastructure_monster(mtmp3) && mtmp3->mtame == 0 && mtmp3->isminion == 0 && mtmp3->isgyp == 0
			&& mtmp3->mnum != quest_info(MS_NEMESIS) && !(rn2(5) && (mtmp3->data->geno & G_UNIQ))) {

				if (mtmp3->mfrenzied) continue;
				if (mtmp3->mpeaceful) continue;
				if (resist(mtmp3, RING_CLASS, 0, NOTELL) && !(((rnd(30 - ACURR(A_CHA))) < 4) && !resist(mtmp3, RING_CLASS, 0, NOTELL)) ) continue;

				pline("%s is pacified!", mon_nam(mtmp3));
				mtmp3->mpeaceful = TRUE;

				} /* monster is catchable loop */
			}

			}

			t_timeout = rnz(6000);
			break;

		case T_HIGH_HEELED_SNEAKERS:
			num = 1000 + (techlevX(tech_no) * 20);
		    	techt_inuse(tech_no) = num + 1;
			pline("For a while, you can use high heels and sexy flats at the same time.");

			t_timeout = rnz(5000);
			break;

		case T_FORM_CHOICE:

			if (!uwep || !is_lightsaber(uwep)) {
				pline("You can only change the form of a wielded lightsaber.");
				break;
			}

			{
				boolean madechoice = 0;
				int sabertype = -1;

				pline("Pick the desired lightsaber form. The prompt will loop until you actually make a choice.");

				while (madechoice == 0 || sabertype < 0) {

					if (yn("Do you want to switch to a one-handed lightsaber?")=='y') {
						    madechoice = 1; sabertype = 1; }
					else if (yn("Do you want to switch to a two-handed lightsaber?")=='y') {
						    madechoice = 1; sabertype = 2; }
					else if (yn("Do you want to switch to a laser swatter?")=='y') {
						    madechoice = 1; sabertype = 3; }
					else if (yn("Do you want to switch to a nano hammer?")=='y') {
						    madechoice = 1; sabertype = 4; }
					else if (yn("Do you want to switch to a lightwhip?")=='y') {
						    madechoice = 1; sabertype = 5; }
					else if (yn("Do you want to switch to an electric cigarette?")=='y') {
						    madechoice = 1; sabertype = 6; }
					else if (yn("Do you want to switch to a laserdent?")=='y') {
						    madechoice = 1; sabertype = 7; }
					else if (yn("Do you want to switch to a laser sword?")=='y') {
						    madechoice = 1; sabertype = 8; }
					else if (yn("Do you want to switch to a sith staff?")=='y') {
						    madechoice = 1; sabertype = 9; }
					else if (yn("Do you want to switch to a laser pole?")=='y') {
						    madechoice = 1; sabertype = 10; }
					else if (yn("Do you want to switch to a starwars mace?")=='y') {
						    madechoice = 1; sabertype = 11; }
					else if (yn("Do you want to switch to a beamsword?")=='y') {
						    madechoice = 1; sabertype = 12; }
					else if (yn("Do you want to switch to a laserxbow?")=='y') {
						    madechoice = 1; sabertype = 13; }
					else if (yn("Do you want to switch to a heavy laser ball?")=='y') {
						    madechoice = 1; sabertype = 14; }
					else if (yn("Do you want to switch to a laser chain?")=='y') {
						    madechoice = 1; sabertype = 15; }
					else if (yn("Do you want to switch to a laserfist?")=='y') {
						    madechoice = 1; sabertype = 16; }
					else if (yn("Do you want to switch to a laser tin opener?")=='y') {
						    madechoice = 1; sabertype = 17; }
					else if (yn("Do you want to switch to a kliusling?")=='y') {
						    madechoice = 1; sabertype = 18; }
					else if (yn("Do you want to switch to a lighttorch?")=='y') {
						    madechoice = 1; sabertype = 19; }
					else if (yn("Do you want to switch to a pink lightsword?")=='y') {
						    madechoice = 1; sabertype = 20; }
					else if (yn("Do you want to switch to a laser flyaxe?")=='y') {
						    madechoice = 1; sabertype = 21; }
					else if (yn("Do you want to switch to a pink double lightsword?")=='y') {
						    madechoice = 1; sabertype = 22; }

				}

				if (sabertype == 2 && (uarms || u.twoweap)) {
					pline("Something is blocking your second %s!", body_part(HAND));
					break;
				}
				if (sabertype == 7 && (uarms || u.twoweap)) {
					pline("Something is blocking your second %s!", body_part(HAND));
					break;
				}
				if (sabertype == 9 && (uarms || u.twoweap)) {
					pline("Something is blocking your second %s!", body_part(HAND));
					break;
				}
				if (sabertype == 10 && (uarms || u.twoweap)) {
					pline("Something is blocking your second %s!", body_part(HAND));
					break;
				}
				if (sabertype == 13 && (uarms || u.twoweap)) {
					pline("Something is blocking your second %s!", body_part(HAND));
					break;
				}
				if (sabertype == 21 && (uarms || u.twoweap)) {
					pline("Something is blocking your second %s!", body_part(HAND));
					break;
				}
				if (sabertype == 22 && (uarms || u.twoweap)) {
					pline("Something is blocking your second %s!", body_part(HAND));
					break;
				}

				if (sabertype == 1) {
					switch (rnd(11)) {
						case 1: uwep->otyp = GREEN_LIGHTSABER; break;
						case 2: uwep->otyp = BLUE_LIGHTSABER; break;
						case 3: uwep->otyp = RED_LIGHTSABER; break;
						case 4: uwep->otyp = YELLOW_LIGHTSABER; break;
						case 5: uwep->otyp = VIOLET_LIGHTSABER; break;
						case 6: uwep->otyp = WHITE_LIGHTSABER; break;
						case 7: uwep->otyp = MYSTERY_LIGHTSABER; break;
						case 8: uwep->otyp = ORANGE_LIGHTSABER; break;
						case 9: uwep->otyp = BLACK_LIGHTSABER; break;
						case 10: uwep->otyp = RAINBOW_LIGHTSABER; break;
						case 11: uwep->otyp = DARK_LIGHTSABER; break;
					}
				} else if (sabertype == 2) {
					uwep->otyp = !rn2(3) ? CYAN_DOUBLE_LIGHTSABER : rn2(2) ? RED_DOUBLE_LIGHTSABER : WHITE_DOUBLE_LIGHTSABER;
				} else if (sabertype == 3) {
					uwep->otyp = LASER_SWATTER;
				} else if (sabertype == 4) {
					uwep->otyp = NANO_HAMMER;
				} else if (sabertype == 5) {
					uwep->otyp = LIGHTWHIP;
				} else if (sabertype == 6) {
					uwep->otyp = ELECTRIC_CIGARETTE;
				} else if (sabertype == 7) {
					uwep->otyp = LASERDENT;
				} else if (sabertype == 8) {
					uwep->otyp = LASER_SWORD;
				} else if (sabertype == 9) {
					uwep->otyp = SITH_STAFF;
				} else if (sabertype == 10) {
					uwep->otyp = LASER_POLE;
				} else if (sabertype == 11) {
					uwep->otyp = STARWARS_MACE;
				} else if (sabertype == 12) {
					uwep->otyp = BEAMSWORD;
				} else if (sabertype == 13) {
					uwep->otyp = LASERXBOW;
				} else if (sabertype == 14) {
					uwep->otyp = HEAVY_LASER_BALL;
				} else if (sabertype == 15) {
					uwep->otyp = LASER_CHAIN;
				} else if (sabertype == 16) {
					uwep->otyp = LASERFIST;
				} else if (sabertype == 17) {
					uwep->otyp = LASER_TIN_OPENER;
				} else if (sabertype == 18) {
					uwep->otyp = KLIUSLING;
				} else if (sabertype == 19) {
					uwep->otyp = LIGHTTORCH;
				} else if (sabertype == 20) {
					uwep->otyp = PINK_LIGHTSWORD;
				} else if (sabertype == 21) {
					uwep->otyp = LASER_FLYAXE;
				} else /* sabertype == 22 */
					uwep->otyp = PINK_DOUBLE_LIGHTSWORD;

				pline("Your lightsaber warps, and changes into a different one!");
				/* known problem: you can pick the form that your lightsaber already has */
			}

			t_timeout = rnz(2000);
			break;

		case T_STAR_DIGGING:
			stardigging();
			pline("Digging complete!");

			t_timeout = rnz(10000);
			break;

		case T_STARWARS_FRIENDS:

			{

			if (u.uswallow) {
				pline("That won't work while engulfed.");
				return 0;
			}

			int k, l, caughtX;
			struct monst *mtmp3;
			pline("You try to tame monsters!");

			    for (k = -1; k <= 1; k++) for(l = -1; l <= 1; l++) {
				if (!isok(u.ux + k, u.uy + l)) continue;
				if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) && !is_infrastructure_monster(mtmp3) && mtmp3->mtame == 0 && mtmp3->isminion == 0 && mtmp3->isgyp == 0
			&& mtmp3->mnum != quest_info(MS_NEMESIS) && !(mtmp3->data->geno & G_UNIQ) )

				{

					if (mtmp3->data->mflags5 & M5_SPACEWARS || mtmp3->data->mflags5 & M5_JOKE) {
						if (!(resist(mtmp3, RING_CLASS, 0, NOTELL) && !(((rnd(30 - ACURR(A_CHA))) < 4) ) )) {
							pline("%s is successfully tamed!", mon_nam(mtmp3));
							(void) tamedog(mtmp3, (struct obj *) 0, TRUE);
						} else {
							pline("%s resists the taming attempt!", mon_nam(mtmp3));
						}
					} else pline("%s cannot be tamed by this method!", mon_nam(mtmp3));

				} /* monster is catchable loop */
			    } /* for loop */

			}

			{
				struct permonst *shoe = 0;
				int attempts = 0;
				struct monst *shoemonst;

				do {
					shoe = rndmonst();
					attempts++;
					if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
					if (!rn2(2000)) reset_rndmonst(NON_PM);

				} while ( (!shoe || (shoe && !(shoe->mflags5 & M5_SPACEWARS) && !(shoe->mflags5 & M5_JOKE))) && attempts < 50000);

				if (shoe) {
					shoemonst = makemon(shoe, u.ux, u.uy, NO_MM_FLAGS);
					if (shoemonst) (void) tamedog(shoemonst, (struct obj *) 0, TRUE);
				}
				u.mondiffhack = 0;
			}

			t_timeout = rnz(5000);
			break;

		case T_USE_THE_FORCE_LUKE:
			num = 200 + (techlevX(tech_no) * 5);
		    	techt_inuse(tech_no) = num + 1;
			pline("Alright Luke, you can use the force powerfully for a while now!");

			t_timeout = rnz(2500);
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

			/* Shop items will interact incorrectly and maybe even segfault. Since I'm way too lazy to make it work
			 * correctly, I'm simply 'sealing' this error by making such items off-limits for telekinesis --Amy */
		  if (costly_spot(cc.x, cc.y)) {
			pline("This item is inside a telekinesis-proof shop. Your attempt to pick it up fails.");
			t_timeout = rnz(250);
			return(1);			
		  }

		  You("pick up an object from the %s.", surface(cc.x,cc.y));
		  (void) pickup_object(otmp, Race_if(PM_LEVITATOR) ? otmp->quan : 1L, TRUE, FALSE);
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
		if (uwep && uwep->otyp == LIGHTTORCH) {
			pline("The lighttorch cannot be recharged that way!");
			return(0);
		}
	      if (u.uen < 5){
		      You("lack the concentration to charge %s. You need at least 5 points of mana!", the(xname(uwep)));
			return(0);
	      }
	      You("start charging %s.", the(xname(uwep)));
	      delay=-10;
	      set_occupation(charge_saber, "charging", 0);
	      t_timeout = rnz(2500);
	      break;
	    default:
	    	pline ("Error!  No such effect (%i)", tech_no);
		return(0);
        }
	  if (t_timeout >= 1) {
		register int timeoutamount = t_timeout;
		while (timeoutamount >= 1000) {
			timeoutamount -= 1000;
			use_skill(P_TECHNIQUES, 1);
		}
		if (timeoutamount >= 1 && (timeoutamount > rn2(1000))) use_skill(P_TECHNIQUES, 1);
		u.cnd_techcount++;

		if (rn2(4) && (t_timeout > rnd(StrongExtra_wpn_practice ? 15000 : Extra_wpn_practice ? 25000 : 50000)) ) {
			maybeleveltech = TRUE;
		}

	  }

	  if (!PlayerCannotUseSkills) {

		switch (P_SKILL(P_TECHNIQUES)) {
			default: break;
			case P_BASIC:
				t_timeout *= 9;
				t_timeout /= 10;
				break;
			case P_SKILLED:
				t_timeout *= 8;
				t_timeout /= 10;
				break;
			case P_EXPERT:
				t_timeout *= 7;
				t_timeout /= 10;
				break;
			case P_MASTER:
				t_timeout *= 6;
				t_timeout /= 10;
				break;
			case P_GRAND_MASTER:
				t_timeout *= 5;
				t_timeout /= 10;
				break;
			case P_SUPREME_MASTER:
				t_timeout *= 4;
				t_timeout /= 10;
				break;
		}
	  }

        if (!can_limitbreak())
	    techtout(tech_no) = (t_timeout * (100 - min(techlevX(tech_no), 50)) / 100);
	  else if (!rn2(3))
	    techtout(tech_no) = (t_timeout * (100 - min(techlevX(tech_no), 50)) / 300);
	  else if (!rn2(2))
	    techtout(tech_no) = (t_timeout * (100 - min(techlevX(tech_no), 50)) / 1000);
	/* limit break can be used for endless exploits, so this fix was urgently necessary... --Amy */

		if (ishaxor && techtout(tech_no) > 1) techtout(tech_no) /= 2;

		if (RngeTechReuse && techtout(tech_no) > 1) { /* 10% reduction --Amy */
			techtout(tech_no) *= 9;
			techtout(tech_no) /= 10;
		}

		if (u.emynluincomplete && techtout(tech_no) > 99) { /* 2% reduction --Amy */
			techtout(tech_no) *= 49;
			techtout(tech_no) /= 50;
		}

		if (irisartiboost()) {
			techtout(tech_no) *= (100 - (irisartiboost() * 5));
			techtout(tech_no) /= 100;
		}

		if (uamul && uamul->oartifact == ART_TYRANITAR_S_QUEST && !rn2(2)) techtout(tech_no) = 0;

		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_NO_ABNORMAL_FUTURE) techtout(tech_no) /= 4;

		if (techid(tech_no) == T_ASIAN_KICK) {

			for (i = 0; i < MAXTECH; i++) {
			    if (techid(i) == T_LEGSCRATCH_ATTACK || techid(i) == T_GROUND_STOMP || techid(i) == T_ATHLETIC_COMBAT )
				if (techtout(tech_no) > 10) techtout(i) += rn2(3) ? rnz(techtout(tech_no)) : rnd(techtout(tech_no));
			}

		}

		if (techid(tech_no) == T_LEGSCRATCH_ATTACK) {

			for (i = 0; i < MAXTECH; i++) {
			    if (techid(i) == T_ASIAN_KICK || techid(i) == T_GROUND_STOMP || techid(i) == T_ATHLETIC_COMBAT )
				if (techtout(tech_no) > 10) techtout(i) += rn2(3) ? rnz(techtout(tech_no)) : rnd(techtout(tech_no));
			}

		}

		if (techid(tech_no) == T_GROUND_STOMP) {

			for (i = 0; i < MAXTECH; i++) {
			    if (techid(i) == T_LEGSCRATCH_ATTACK || techid(i) == T_ASIAN_KICK || techid(i) == T_ATHLETIC_COMBAT )
				if (techtout(tech_no) > 10) techtout(i) += rn2(3) ? rnz(techtout(tech_no)) : rnd(techtout(tech_no));
			}

		}

		if (techid(tech_no) == T_ATHLETIC_COMBAT) {

			for (i = 0; i < MAXTECH; i++) {
			    if (techid(i) == T_LEGSCRATCH_ATTACK || techid(i) == T_GROUND_STOMP || techid(i) == T_ASIAN_KICK )
				if (techtout(tech_no) > 10) techtout(i) += rn2(3) ? rnz(techtout(tech_no)) : rnd(techtout(tech_no));
			}

		}

		if (techid(tech_no) == T_AUTOKILL) {

			for (i = 0; i < MAXTECH; i++) {
			    if (techid(i) == T_CHAIN_THUNDERBOLT || techid(i) == T_FLASHING_MISCHIEF )
				if (techtout(tech_no) > 10) techtout(i) += rn2(3) ? rnz(techtout(tech_no)) : rnd(techtout(tech_no));
			}

		}

		if (techid(tech_no) == T_CHAIN_THUNDERBOLT) {

			for (i = 0; i < MAXTECH; i++) {
			    if (techid(i) == T_AUTOKILL || techid(i) == T_FLASHING_MISCHIEF )
				if (techtout(tech_no) > 10) techtout(i) += rn2(3) ? rnz(techtout(tech_no)) : rnd(techtout(tech_no));
			}

		}

		if (techid(tech_no) == T_FLASHING_MISCHIEF) {

			for (i = 0; i < MAXTECH; i++) {
			    if (techid(i) == T_CHAIN_THUNDERBOLT || techid(i) == T_AUTOKILL )
				if (techtout(tech_no) > 10) techtout(i) += rn2(3) ? rnz(techtout(tech_no)) : rnd(techtout(tech_no));
			}

		}

	if (maybeleveltech) {
		if (tech_list[tech_no].t_lev > 0) {
			tech_list[tech_no].t_lev -= 1;
			pline("Your %s technique leveled up to level %d!", techname(tech_no), techlev(tech_no));
		}
	}

	/*By default,  action should take a turn*/
	if (techid(tech_no) == T_DIRECTIVE) return 0;
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

	    if (techid(i) == T_SILENT_OCEAN) {
		if (techt_inuse(i) == 10) pline("Attention: The effect of Silent Ocean will only last for 10 more turns. If you can't exist in water without it, consider heading to the nearest shore.");
		if (techt_inuse(i) == 2) pline("Caution! Silent Ocean will end in 2 turns! If you're currently in the water and can't exist in water without the effect of Silent Ocean, you should get back on dry land quickly in order to prevent drowning!");
	    }

	    if (techt_inuse(i) && !(techid(i) == T_UNARMED_FOCUS && !rn2(4) && !uwep && !(u.twoweap && uswapwep) && uarm && uarm->otyp == ROBE_OF_FOCUSSING ) ) {
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
		    case T_DOUBLE_THROWNAGE:
			You("relax.");
			break;
		    case T_E_FIST:
			You_feel("the power dissipate.");
			break;
		    case T_SPECTRAL_SWORD:
			You_feel("your weapon losing its elemental power.");
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
		    case T_SHUT_THAT_BITCH_UP:
			You("can no longer shut up bitches.");
			break;
		    case T_EXTRA_LONG_SQUEAK:
			pline("Your butt has stopped squeaking.");
			break;
		    case T_BULLETREUSE:
			pline("Your ammo can no longer be reused.");
			break;
		    case T_CONCENTRATING:
			You("stop concentrating.");
			break;
		    case T_SHIELD_BASH:
			You("pull back your shield.");
			break;
		    case T_EDDY_WIND:
			You("stop whirling.");
			break;
		    case T_IRON_SKIN:
			Your("skin is no longer hard as iron.");
			break;
		    case T_POWER_SURGE:
			pline_The("awesome power within you fades.");
			break;
		    case T_BLINK:
			You("sense the flow of time returning to normal.");
			break;
		    case T_CHI_STRIKE:
			You_feel("the power in your hands dissipate.");
			break;
		    case T_SPELL_SPAM:
			You_feel("that the spell spam power has timed out.");
			break;
		    case T_CHI_HEALING:
			You_feel("the healing power dissipate.");
			break;
		    case T_SILENT_OCEAN:
			pline("The effect of Silent Ocean ends.");
			break;
		    case T_GLOWHORN:
			pline("The glowing unicorn horn disappears.");
			break;
		    case T_ENERGY_CONSERVATION:
			pline("Your lightsaber consumes energy at the normal rate again.");
			break;
		    case T_UNARMED_FOCUS:
			pline("You're no longer focussing on unarmed combat.");
			break;
		    case T_WILD_SLASHING:
			pline("You stop your wild slashing.");
			break;
		    case T_ABSORBER_SHIELD:
			pline("The absorber shield dissipates.");
			break;
		    case T_STEADY_HAND:
			pline("Your %s are less steady.", makeplural(body_part(HAND)));
			break;
		    case T_BEAMSWORD:
			pline("Your lightsaber can no longer fire beams.");
			break;
		    case T_ENERGY_TRANSFER:
			pline("The energy transfer has stopped.");
			break;
		    case T_POWERFUL_AURA:
			pline("You're no longer surrounded by the powerful aura.");
			break;
		    case T_REFUGE:
			pline("The refuge effect has ended.");
			break;
		    case T_ESCROBISM:
			pline("Escrobism is no longer active.");
			break;
		    case T_GREEN_MISSILE:
			pline("You ran out of wood for your poisonous missiles.");
			break;
		    case T_BIG_N_VEINY:
			pline("Your body is no longer poisonous.");
			break;
		    case T_POISON_PEN_LETTER:
			pline("You're done writing your poison pen letters, gotta conserve the ink after all.");
			break;
		    case T_AFTERBURNER:
			pline("Your afterburner is depleted.");
			break;
		    case T_BUGGARD:
			pline("Damn, the buggard effect has ended already! Curse those developers that made its duration so short relative to its long timer!");
			break;
		    case T_THUNDERSTORM:
			pline("Your thunderstorm flashes for one last time and ceases.");
			break;
		    case T_BLADE_SHIELD:
			pline("Blade shield effect has ended. At least now it no longer disrupts your attacks.");
			break;
		    case T_POLE_MELEE:
			pline("Your polearms and lances are no longer effective at close range.");
			break;
		    case T_WEAPON_BLOCKER:
			pline("Thankfully, weapon blocker is temporary here and has just ended. You got very annoyed by the block lock effect anyway.");
			break;
		    case T_GRAP_SWAP:
			pline("The effects of your grinders and lances are no longer swapped.");
			break;
		    case T_LIGHTER_BALLS:
			Your("balls are heavy again.");
			if (FunnyHallu) pline("Now's the time to present them to a sweet girl, who can kick them to test your pain threshold.");
			break;
		    case T_POWERBIOSIS:
			pline("Your symbiote's awesome power fades.");
			break;
		    case T_IMPLANTED_SYMBIOSIS:
			pline("The boost granted to your symbiote by your implant has been used up.");
			break;
		    case T_PIRATE_BROTHERING:
			pline("Your pirate brothering ability deactivates.");
			break;
		    case T_DECAPABILITY:
			pline("Your weapons are no longer capable of decapability.");
			break;
		    case T_HIGH_HEELED_SNEAKERS:
			pline("You can no longer use the sexy flats skill while wearing high heels.");
			break;
		    case T_SPRINT:
			pline("Your sprint ends.");
			break;
		    case T_SKULL_CRUSH:
			pline("You waited too long, and can no longer crush an opponent's skull!");
			break;
		    case T_FEMALE_COMBO:
			pline("Your female combo has ended.");
			break;
		    case T_DOCKLOCK:
			pline("You're done bashing enemies' heads with your heels.");
			break;
		    case T_SEXY_STAND:
			pline("You've finished your sexy stand.");
			break;
		    case T_MARATHON:
			pline("You've been running for far too long, and decide that you need a rest.");
			break;
		    case T_PERFUME_STRIDE:
			pline("Finally, you can take your heels off again. Beware that unless you uncursed them, you probably still can't.");
			break;
		    case T_NAUGHTY_HEELOT:
			pline("You're done playing the naughty heelot.");
			break;
		    case T_EXTREME_STURDINESS:
			pline("Your GOI has expired. Or, in human-readable terms, your extreme sturdiness ends and you need to be more careful again.");
			break;
		    case T_BUTT_PROTECTION:
			pline("Your butt is no longer protected.");
			break;
		    case T_HEEL_STAB:
			pline("You can no longer stab enemies with your heel.");
			break;
		    case T_USE_THE_FORCE_LUKE:
			pline("Sorry Luke, but you can no longer use the force with extra effectivity.");
			break;
		    case T_SURRENDER_OR_DIE:
			pline("Cutting an opponent's weapon no longer has special effects.");
			break;
		    case T_PERILOUS_WHIRL:
			pline("Your double lightsaber no longer drains life.");
			break;
		    case T_STAT_RESIST:
			pline("You no longer resist status effects.");
			break;
		    case T_FORCE_FIELD:
			pline("The force field dissipates.");
			break;
		    case T_WHIRLSTAFF:
			pline("Your staff no longer attacks as quickly as before.");
			break;
		    case T_BEAUTY_CHARM:
			pline("You're done posing with your heels.");
			break;
		    case T_ASIAN_KICK:
			pline("You waited too long and now the opportunity to smash an enemy's nuts with your stilettos is no longer there... :(");
			break;
		    case T_LEGSCRATCH_ATTACK:
			pline("Your sexy cone heels are done scratching hostile shins. They sure drew a lot of blood, and you feel that they're very pleased!");
			break;
		    case T_GROUND_STOMP:
			pline("Your wedge heels are done stomping defenseless enemies. It was a lot of fun, and your heels love you for using them as deadly weapons!");
			break;
		    case T_ATHLETIC_COMBAT:
			pline("Wow, you're so athletic, it's amazing watching you aim high kicks at your enemies' heads with your block heels. But now it's time to relax. I'm fairly sure your fleecy block heels like you even more now!");
			break;
		    case T_OVER_RAY:
			pline("Your rays have the normal range again.");
			break;
		    case T_JOKERBANE:
			pline("Your boomerangs are no longer capable of paralyzing enemies.");
			break;
		    case T_COMBO_STRIKE:
			pline("Your combo has ended.");
			break;
		    case T_JIU_JITSU:
			pline("You stop using jiu-jitsu.");
			break;
		    case T_BLADE_ANGER:
			pline("Your shuriken are no longer capable of piercing.");
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
	    }  /* technique active check */

	    if (techtout(i) == 1) pline("Your %s technique is ready to be used!", techname(i));
	    if (techtout(i) > 0) {
		if (!(uarmf && uarmf->oartifact == ART_THICKER_THAN_THE_HEAD && rn2(2) && (techtout(i) > 1) ) )
		techtout(i)--;
	    }
	    if (uarmc && uarmc->oartifact == ART_ARTIFICIAL_FAKE_DIFFICULTY && techtout(i) > 0) {
		if (techtout(i) == 1) pline("Your %s technique is ready to be used!", techname(i));
		techtout(i)--;
	    }

	    if (techtout(i) < 0) techtout(i) = 0; /* fail safe */
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

/* occasionally things will cause certain techs to end prematurely --Amy */
void
stopsingletechnique(whichtech)
int whichtech;
{
	int i, tech;

	for (i = 0; i < MAXTECH; i++) {
	    tech = techid(i);
	    if (tech != NO_TECH && techt_inuse(i) && tech == whichtech) {
		aborttech(tech);
	    }
	}

}

void
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
	if (Race_if(PM_YUGGER)) return ((struct innate_tech *) 0); /* none */

	switch (Role_switch) {
		case PM_ARCHEOLOGIST:	return (arc_tech);
		case PM_GOFF:	return (gof_tech);
		case PM_DRUID:	return (drd_tech);
		case PM_SOFTWARE_ENGINEER:	return (sof_tech);
		case PM_CRACKER:	return (cra_tech);
		case PM_STORMBOY:	return (sto_tech);
		case PM_SPACE_MARINE:	return (sma_tech);
		case PM_JANITOR:	return (jan_tech);
		case PM_FIREFIGHTER:	return (fir_tech);
		case PM_YAUTJA:	return (yau_tech);
		case PM_JOCKEY:	return (joc_tech);
		case PM_GAMER:	return (gam_tech);
		case PM_GLADIATOR:	return (gla_tech);
		case PM_AMAZON:	return (ama_tech);
		case PM_ARTIST:	return (art_tech);
		case PM_ALTMER:	return (alt_tech);
		case PM_BOSMER:	return (bos_tech);
		case PM_BULLY:	return (bul_tech);
		case PM_DUNMER:	return (dun_tech);
		case PM_STAND_USER:	return (sta_tech);
		case PM_ORDINATOR:	return (ord_tech);
		case PM_THALMOR:	return (tha_tech);
		case PM_DRUNK:	return (dru_tech);
		case PM_BARBARIAN:	return (bar_tech);
		case PM_NOOB_MODE_BARB:	return (noo_tech);
		case PM_BLOODSEEKER:	return (blo_tech);
		case PM_BLEEDER:	return (ble_tech);
		case PM_CAVEMAN:	return (cav_tech);
		case PM_DEMAGOGUE:	return (dem_tech);
		case PM_DQ_SLIME:	return (sli_tech);
		case PM_ERDRICK:	return (erd_tech);
		case PM_BARD:	return (brd_tech);
		case PM_FLAME_MAGE:	return (fla_tech);
		case PM_ACID_MAGE:	return (aci_tech);
		case PM_HUSSY:	return (hus_tech);
		case PM_TRANSVESTITE:	return (tra_tech);
		case PM_PROSTITUTE:	return (kur_tech);
		case PM_KURWA:	return (kur_tech);
		case PM_DISSIDENT:	return (dis_tech);
		case PM_MASON:	return (mas_tech);
		case PM_XELNAGA:	return (xel_tech);
		case PM_MASTERMIND:	return (mam_tech);
		case PM_SAGE:	return (sag_tech);
		case PM_TRANSSYLVANIAN:	return (trs_tech);
		case PM_TOPMODEL:	return (top_tech);
		case PM_FAILED_EXISTENCE:	return (fai_tech);
		case PM_LUNATIC:	return (lun_tech);
		case PM_ACTIVISTOR:	return (act_tech);
		case PM_ELECTRIC_MAGE:	return (ele_tech);
		case PM_POISON_MAGE:	return (psn_tech);
		case PM_HEALER:		return (hea_tech);
		case PM_ICE_MAGE:	return (ice_tech);
		case PM_JEDI:		return (jed_tech);
		case PM_HEDDERJEDI:		return (jed_tech);
		case PM_KNIGHT:		return (kni_tech);
		case PM_KORSAIR:		return (kor_tech);
		case PM_SECRET_ADVICE_MEMBER:		return (sec_tech);
		case PM_PREVERSIONER:		return (pre_tech);
		case PM_DANCER:		return (dan_tech);
		case PM_DIABLIST:		return (dia_tech);
		case PM_BUTT_LOVER:		return (but_tech);
		case PM_JUSTICE_KEEPER:		return (jus_tech);
		case PM_MONK: 		return (mon_tech);
		case PM_OCCULT_MASTER: 		return (occ_tech);
		case PM_ELEMENTALIST: 		return (emn_tech);
		case PM_CHAOS_SORCEROR: 		return (cha_tech);
		case PM_JESTER: 		return (jes_tech);
		case PM_LADIESMAN: 		return (lad_tech);
		case PM_GEEK: 		return (gee_tech);
		case PM_GRADUATE: 		return (gee_tech);
		case PM_SEXYMATE: 		return (sex_tech);
		case PM_ZOOKEEPER: 		return (zoo_tech);
		case PM_MEDIUM: 		return (med_tech);
		case PM_FENCER: 		return (fnc_tech);
		case PM_FIGHTER: 		return (fig_tech);
		case PM_SUPERMARKET_CASHIER: 		return (sup_tech);
		case PM_WANDKEEPER: 		return (wan_tech);
		case PM_WARRIOR: 		return (war_tech);
		case PM_PALADIN: 		return (pal_tech);
		case PM_FEAT_MASTER: 		return (stu_tech);
		case PM_DOLL_MISTRESS: 		return (dol_tech);
		case PM_MIDGET:		return (mid_tech);
		case PM_PSYKER:		return (psy_tech);
		case PM_QUARTERBACK:		return (qua_tech);
		case PM_GUNNER: 		return (gun_tech);
		case PM_CLIMACTERIAL: 		return (cli_tech);
		case PM_WOMANIZER: 		return (wom_tech);
		case PM_DOOM_MARINE: 		return (mar_tech);
		case PM_ANACHRONIST: 		return (ana_tech);
		case PM_LIBRARIAN: 		return (lib_tech);
		case PM_AUGURER: 		return (aug_tech);
		case PM_SAIYAN: 		return (sai_tech);
		case PM_PSION: 		return (psi_tech);
		case PM_SCIENTIST: 		return (sci_tech);
		case PM_ASTRONAUT: 		return (ast_tech);
		case PM_CELLAR_CHILD: 		return (cel_tech);
		case PM_DEATH_EATER: 		return (dea_tech);
		case PM_DIVER: 		return (div_tech);
		case PM_POKEMON: 		return (pok_tech);
		case PM_NUCLEAR_PHYSICIST: 		return (nuc_tech);
		case PM_FORM_CHANGER: 		return (for_tech);
		case PM_GENDERSTARIST: 		return (gen_tech);
		case PM_EMPATH: 		return (emp_tech);
		case PM_GANGSTER: 		return (gan_tech);
		case PM_ROCKER: 		return (roc_tech);
		case PM_NECROMANCER:	return (nec_tech);
		case PM_CRUEL_ABUSER:	return (abu_tech);
		case PM_PRIEST:		return (pri_tech);
		case PM_GOLDMINER:		return (gol_tech);
		case PM_CHEVALIER:		return (che_tech);
		case PM_ASSASSIN:		return (ass_tech);
		case PM_RANGER:		return (ran_tech);
		case PM_TWELPH:
		case PM_ELPH:		return (elp_tech);
		case PM_SPACEWARS_FIGHTER:		return (spa_tech);
		case PM_CAMPERSTRIKER:		return (cam_tech);
		case PM_LOCKSMITH:		return (loc_tech);
		case PM_WEIRDBOY:		return (wei_tech);
		case PM_GANG_SCHOLAR:		return (sco_tech);
		case PM_WALSCHOLAR:		return (wal_tech);
		case PM_OFFICER:		return (off_tech);
		case PM_ROGUE:		return (rog_tech);
		case PM_SAMURAI:	return (sam_tech);
		case PM_CYBERNINJA:	return (cyb_tech);
		case PM_GRENADONIN:	return (gro_tech);
		case PM_NINJA:	return (nin_tech);
		case PM_TOURIST:	return (tou_tech);
		case PM_UNDEAD_SLAYER:	return (und_tech);
		case PM_UNBELIEVER:	return (unb_tech);
		case PM_SHAPESHIFTER:	return (sha_tech);
		case PM_SLAVE_MASTER:	return (sla_tech);
		case PM_RINGSEEKER:		return (rin_tech);
		case PM_UNDERTAKER:	return (unt_tech);
		case PM_VALKYRIE:	return (val_tech);
		case PM_WIZARD:		return (wiz_tech);
		case PM_YEOMAN:		return (yeo_tech);
		case PM_CARTOMANCER:		return (car_tech);
		case PM_DRAGONMASTER:		return (dra_tech);
		case PM_COMBATANT:		return (com_tech);
		case PM_FJORDE:		return (fjo_tech);
		case PM_PRACTICANT:		return (pra_tech);
		case PM_EMERA:		return (eme_tech);
		case PM_SYMBIANT:		return (sym_tech);
		default: 		return ((struct innate_tech *) 0);
	}
}

static const struct     innate_tech *
race_tech()
{
	switch (Race_switch) {
		case PM_DOPPELGANGER:	return (dop_tech);
		case PM_AZTPOK:	return (azt_tech);
		case PM_HC_ALIEN:	return (hca_tech);
		case PM_SLYER_ALIEN:	return (sly_tech);
		case PM_DWARF:		return (dwa_tech);
		case PM_ELF:
		case PM_PLAYER_MYRKALFR:
		case PM_DROW:		return (elf_tech);
		case PM_CURSER:		return (cur_tech);
		case PM_IMPERIAL:		return (imp_tech);
		case PM_SWIKNI:		return (swi_tech);
		case PM_TAYIIN:		return (tay_tech);
		case PM_CLOCKWORK_AUTOMATON:		return (clk_tech);

		case PM_FENEK:		return (fen_tech);
		case PM_LOWER_ENT:		return (ent_tech);
		case PM_NORD:		return (nor_tech);
		case PM_RETICULAN:		return (ret_tech);
		case PM_OUTSIDER:		return (out_tech);
		case PM_LICH_WARRIOR:		return (lic_tech);
		case PM_ALBAE:		return (alb_tech);
		case PM_BABYLONIAN:		return (bab_tech);
		case PM_GERTEUT:		return (ger_tech);
		case PM_POISONER:		return (poi_tech);
		case PM_PLAYABLE_NEANDERTHAL:		return (nea_tech);
		case PM_VORTEX:		return (vor_tech);
		case PM_CORTEX:		return (cor_tech);
		case PM_GNOME:		return (gno_tech);
		case PM_KOP:		return (kop_tech);
		case PM_ANCIPITAL:		return (anc_tech);
		case PM_DEEP_ELF:		return (dee_tech);
		case PM_DEVELOPER:		return (dvp_tech);
		case PM_SHOE:		return (sho_tech);
		case PM_BULDOZGAR:		return (bga_tech);
		case PM_PLAYER_ANDROID:		return (and_tech);
		case PM_FAWN:		return (faw_tech);
		case PM_PLAYER_GREMLIN:		return (gre_tech);
		case PM_STICKER:		return (sti_tech);
		case PM_THUNDERLORD:		return (thu_tech);
		case PM_JAVA:		return (jav_tech);
		case PM_WISP:		return (wis_tech);
		case PM_ROHIRRIM:		return (roh_tech);
		case PM_PLAYER_SALAMANDER:		return (sal_tech);
		case PM_SATRE:		return (sat_tech);
		case PM_KORONST:		return (kst_tech);
		case PM_RUSMOT:		return (rus_tech);
		case PM_PIECE:		return (pic_tech);
		case PM_CARTHAGE:		return (crt_tech);
		case PM_MACTHEIST:		return (mac_tech);
		case PM_BOVER:		return (bov_tech);
		case PM_HAND:		return (han_tech);
		case PM_PERVERT:		return (per_tech);
		case PM_MONGUNG:		return (mog_tech);
		case PM_VIETIS:		return (vie_tech);
		case PM_SPARD:		return (spd_tech);
		case PM_WEAPONIZED_DINOSAUR:		return (din_tech);
		case PM_BURNINATOR:		return (bur_tech);
		case PM_KOBOLT:		return (kob_tech);
		case PM_OGRO:		return (ogr_tech);
		case PM_PLAYER_FAIRY:		return (fay_tech);
		case PM_BATMAN:		return (bat_tech);
		case PM_HUMANOID_CENTAUR:		return (cen_tech);
		case PM_BORG:		return (bor_tech);
		case PM_WOOKIE:		return (woo_tech);
		case PM_RODNEYAN:		return (rod_tech);
		case PM_TURTLE:		return (tur_tech);
		case PM_TURMENE:		return (tme_tech);
		case PM_JELLY:		return (jel_tech);
		case PM_HUMANOID_DRYAD:		return (dry_tech);
		case PM_UNGENOMOLD:		return (ung_tech);
		case PM_PLAYER_UNICORN:		return (uni_tech);
		case PM_UNMAGIC_FISH:		return (unm_tech);
		case PM_MOULD:		return (mou_tech);
		case PM_MANSTER:		return (man_tech);
		case PM_OCTOPODE:		return (oct_tech);
		case PM_INKA:		return (ink_tech);
		case PM_ARGONIAN:		return (arg_tech);
		case PM_BRETON:		return (bre_tech);
		case PM_MAZIN:		return (mzi_tech);
		case PM_PLAYER_SHEEP:		return (she_tech);
		case PM_SAMEDI:		return (sdi_tech);
		case PM_REDGUARD:		return (red_tech);
		case PM_ARMED_COCKATRICE:		return (coc_tech);
		case PM_INSECTOID:		return (ins_tech);
		case PM_KLACKON:		return (kla_tech);
		case PM_MUMMY:		return (mum_tech);
		case PM_KHAJIIT:	return (kha_tech);
		case PM_WEAPON_CUBE:	return (gel_tech);
		case PM_TONBERRY:	return (ton_tech);
		case PM_HOBBIT:		return (hob_tech);
		case PM_HUMAN_WEREWOLF:	return (lyc_tech);
		case PM_HUMAN_MONKEY:	return (hmo_tech);
		case PM_PLAYER_ZRUTY:	return (zru_tech);
		case PM_VAMPIRE:	return (vam_tech);
		case PM_SUCKING_FIEND:	return (vam_tech);
		case PM_VAMGOYLE:	return (vam_tech);
		case PM_LEVITATOR:	return (lev_tech);
		case PM_HERBALIST:		return (hrb_tech);
		case PM_ALCHEMIST: 		return (alc_tech);
		case PM_WEAPON_BUG: 		return (gri_tech);
		case PM_HUMANOID_ANGEL:	return (ang_tech);
		case PM_INCANTIFIER:	return (inc_tech);
		case PM_PLAYER_MIMIC:	return (mim_tech);
		case PM_MISSINGNO:	return (mis_tech);
		case PM_NAVI:	return (nav_tech);
		case PM_SPIDERMAN:	return (spi_tech);
		case PM_VENTURE_CAPITALIST:	return (ven_tech);
		case PM_TRANSFORMER:	return (trf_tech);
		case PM_WEAPON_TRAPPER:	return (trp_tech);
		case PM_IRAHA:	return (ira_tech);
		case PM_SPRIGGAN:	return (spr_tech);
		case PM_VEELA:	return (vee_tech);
		case PM_WARPER:	return (wrp_tech);
		case PM_EGYMID:	return (egy_tech);
		case PM_NEMESIS:	return (nem_tech);
		case PM_CUPID:	return (cup_tech);
		case PM_URGOTH:	return (urg_tech);
		case PM_WYLVAN:	return (wyl_tech);
		case PM_ZAUR:	return (zau_tech);
		case PM_THRALL:	return (thr_tech);
		case PM_PLAYER_CERBERUS:	return (cer_tech);
		case PM_BACTERIA:	return (bac_tech);
		case PM_UNALIGNMENT_THING:	return (una_tech);
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

STATIC_PTR int
charge_saber()
{
	int i, tlevel, yourmana, manamultiply, calchelpvar, effectlevel;
	if(delay) {
		delay++;
		return(1);
	}
	for (i = 0; i < MAXTECH; i++) {
	    if (techid(i) == NO_TECH)
		continue;
	    if (techid(i) != T_CHARGE_SABER)
		continue;
	    tlevel = techlevX(i);
	}
	if (tlevel >= 10) {

		int jackpotchance = 20; /* 1 in 5 was way too common --Amy */

		switch (tlevel) {
			default: jackpotchance = 20; if (tlevel >= 50) jackpotchance = 5; break;
			case 10:
			case 11:
			case 12:
			case 13:
				jackpotchance = 20; break;
			case 14:
			case 15:
			case 16:
				jackpotchance = 19; break;
			case 17:
			case 18:
			case 19:
				jackpotchance = 18; break;
			case 20:
			case 21:
			case 22:
				jackpotchance = 17; break;
			case 23:
			case 24:
				jackpotchance = 16; break;
			case 25:
			case 26:
			case 27:
				jackpotchance = 15; break;
			case 28:
			case 29:
				jackpotchance = 14; break;
			case 30:
			case 31:
			case 32:
				jackpotchance = 13; break;
			case 33:
			case 34:
				jackpotchance = 12; break;
			case 35:
			case 36:
				jackpotchance = 11; break;
			case 37:
			case 38:
				jackpotchance = 10; break;
			case 39:
			case 40:
			case 41:
				jackpotchance = 9; break;
			case 42:
			case 43:
			case 44:
				jackpotchance = 8; break;
			case 45:
			case 46:
			case 47:
				jackpotchance = 7; break;
			case 48:
			case 49:
				jackpotchance = 6; break;
			case 50:
				jackpotchance = 5; break;
		}

		if (!rn2(jackpotchance)) {
			You("manage to channel the force perfectly!");
			uwep->age += 1500; // Jackpot!
			if (uwep->oartifact == ART_KYLE_S_JACKPOT) uwep->age += (1500 * rnd(2));
		}

	} else
		You("channel the force into %s.", the(xname(uwep)));

	/* it was simply too strong... arbitrarily lower the amount you'll get --Amy */
	yourmana = u.uen;
	manamultiply = 0;
	while (yourmana > 99) {
		manamultiply++;
		yourmana -= 100;
	}
	yourmana = u.uen;
	while (manamultiply > 0) {
		calchelpvar = (yourmana - (manamultiply * 100));
		if (calchelpvar < 1) goto manacalccomplete;
		yourmana -= rnd(calchelpvar);
		manamultiply--;

	}
manacalccomplete:
	if (yourmana < 1) yourmana = 1; /* fail safe, shouldn't happen */

	if (techlevX(T_CHARGE_SABER) < 1) {
		pline("Your ability to charge the saber is beyond recall, and therefore nothing happens.");
		return(0);
	}
	effectlevel = rno(techlevX(T_CHARGE_SABER));

	/* yes no return above, it's a bonus :) */
	uwep->age += (yourmana * (( (effectlevel + rnd(5 + effectlevel) ) / rnd(10)) + rno(3)));
	if (uwep->oartifact == ART_KYLE_S_JACKPOT) {
		uwep->age += (yourmana * (( (effectlevel + rnd(5 + effectlevel) ) / rnd(10)) + rno(3)));
		if (!rn2(2)) uwep->age += (yourmana * (( (effectlevel + rnd(5 + effectlevel) ) / rnd(10)) + rno(3)));
	}
	/* improved results by Amy */

	u.uen = 0;
	flags.botl = 1;
	return(0);
}

int
techlevX(tech)
int tech;
{
	int finaltechlevel = (u.ulevel - tech_list[tech].t_lev);
	if (StrongTechnicality) {
		finaltechlevel *= 4;
		finaltechlevel /= 3;
		finaltechlevel += 10;
	} else if (Technicality) {
		finaltechlevel *= 4;
		finaltechlevel /= 3;
		finaltechlevel += 3;
	}
	if (uarmh && itemhasappearance(uarmh, APP_TECHNICAL_HELMET)) finaltechlevel++;

	finaltechlevel += (boost_power_value() * 2);

	if (finaltechlevel > 50) finaltechlevel = 50; /* fail safe */
	return finaltechlevel;
}

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
		upgrade_obj(otmp, FALSE);
	} else {
		/* object downgrade  - But for now,  nothing :) */
	}

	setuwep(otmp, FALSE, TRUE);
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
			    	powbonus =  (u.uenmax > 28 ? u.uenmax / 4 : 7);
				break;
			    case THRONE: /* Regal == pseudo divine */
			    	powbonus = (u.uenmax > 36 ? u.uenmax / 6 : 6);
				break;
			    case PENTAGRAM:
			    	powbonus = (u.uenmax > 10 ? u.uenmax / 2 : 4);
				break;
			    case CLOUD: /* Air */
			    case TREE: /* Earth */
			    case LAVAPOOL: /* Fire */
			    case ICE: /* Water - most ordered form */
			    case STYXRIVER:
			    case BURNINGWAGON:
			    case BUBBLES:
			    	powbonus = (u.uenmax > 40 ? u.uenmax / 8 : 5);
				break;
			    case AIR:
			    case MOAT: /* Doesn't freeze */
			    case WATER:
			    case WATERTUNNEL:
			    case WAGON:
			    case NETHERMIST:
			    	powbonus = (u.uenmax > 40 ? u.uenmax / 10 : 4);
				break;
			    case CRYSTALWATER:
			    case CARVEDBED:
			    case RAINCLOUD:
			    	powbonus = (u.uenmax > 20 ? u.uenmax / 5 : 4);
				break;
			    case POOL: /* Can dry up */
			    case WOODENTABLE:
			    case SNOW:
			    case ASH:
			    case HIGHWAY:
			    	powbonus = (u.uenmax > 36 ? u.uenmax / 12 : 3);
				break;
			    case STRAWMATTRESS:
			    case GRASSLAND:
			    case STALACTITE:
			    	powbonus = (u.uenmax > 50 ? u.uenmax / 20 : 2);
				break;
			    case FOUNTAIN:
			    case WELL:
			    case SHIFTINGSAND:
			    case SAND:
			    	powbonus = (u.uenmax > 30 ? u.uenmax / 15 : 2);
				break;
			    case FARMLAND:
			    case MOUNTAIN:
			    case CRYPTFLOOR:
			    	powbonus = (u.uenmax > 20 ? u.uenmax / 10 : 2);
				break;
			    case SINK:  /* Cleansing water */
			    	if (!rn2(3)) powbonus = 2;
				break;
			    case TOILET: /* Water Power...but also waste! */
			    case MOORLAND:
			    case POISONEDWELL:
			    	if (rn2(100) < 50)
			    		powbonus = 2;
			    	else powbonus = -2;
				break;
			    case GRAVE:
			    case GRAVEWALL:
			    	powbonus = -4;
				break;
			    case URINELAKE:
			    	powbonus = -10;
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
	{"LDL", 3, blitz_combo_strike, T_COMBO_STRIKE, BLITZ_CHAIN},
	{"RUR", 3, blitz_combo_strike, T_COMBO_STRIKE, BLITZ_CHAIN},
	{"LDDL", 4, blitz_draining_punch, T_DRAINING_PUNCH, BLITZ_CHAIN},
	{"RUUR", 4, blitz_draining_punch, T_DRAINING_PUNCH, BLITZ_CHAIN},
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
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
            	return(0);
	}

	bp = buf;
	
	if (!getdir((char *)0)) return(0);
	if (!u.dx && !u.dy) {
		return(0);
	}
	if (!isok(u.ux + u.dx, u.uy + u.dy)) {
		return 0;
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
	    	    blitz_num >= (MIN_CHAIN + (techlevX(tech_no) / 10)))
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

        sprintf(buf, "%16s %10s %-17s", "[LU = Left Up]", "[U = Up]", "[RU = Right Up]");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, MENU_UNSELECTED);
        sprintf(buf, "%16s %10s %-17s", "[L = Left]", "", "[R = Right]");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, MENU_UNSELECTED);
        sprintf(buf, "%16s %10s %-17s", "[LD = Left Down]", "[D = Down]", "[RD = Right Down]");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, MENU_UNSELECTED);        

        sprintf(buf, "%-30s %10s   %s", "Name", "Type", "Command");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, MENU_UNSELECTED);

        for (i = 0; blitzes[i].blitz_len; i++) {
	    if (tech_known(blitzes[i].blitz_tech)) {
                sprintf(buf, "%-30s %10s   %s",
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
	You_feel("energy surge through your hands!");
	techt_inuse(tech_no) = techlevX(tech_no) + 4;
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
	techt_inuse(tech_no) = rnd((int) (techlevX(tech_no)/3 + 1)) + d(1,4) + 2;
	return 1;
}

static int
blitz_combo_strike()
{
	struct monst *mtmp;

	if (tech_inuse(T_COMBO_STRIKE)) {
		u.combostrike = 0;
		u.comboactive = FALSE;
		stopsingletechnique(T_COMBO_STRIKE);
	}

	You("do a combo strike!");
	if (u.uswallow)
	    mtmp = u.ustuck;
	else
	    mtmp = m_at(u.ux + u.dx, u.uy + u.dy);

	if (!mtmp) {
		pline("But there was no target!");
		return (0);
	}
	if (!attack(mtmp)) return (0);
	else u.combostrike += 10;
	return(1);
}

static int
blitz_draining_punch()
{
	struct monst *mtmp;

	if (u.uswallow)
	    mtmp = u.ustuck;
	else
	    mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
	if (!mtmp) {
		You("strike nothing.");
		return (0);
	}
	if (!attack(mtmp)) return (0);

	if (!mtmp) return(0);

	if (mtmp && !(DEADMONSTER(mtmp)) && mtmp->mhp > 0 && !resists_drli(mtmp) && !resist(mtmp, WEAPON_CLASS, 0, NOTELL) ) {

		int xtmp = d(2,6);
		pline("%s suddenly seems weaker!", Monnam(mtmp));
		mtmp->mhpmax -= xtmp;
#ifdef SHOW_DMG
		if (xtmp < mtmp->mhp) showdmg(xtmp);
#endif
		if ((mtmp->mhp -= xtmp) <= 0 || !mtmp->m_lev) {
			pline("%s dies!", Monnam(mtmp));
			xkilled(mtmp,0);
		} else
			mtmp->m_lev--;
	}
	return(1);

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
	    if (rn2(90) > (techlevX(tech_no) + 30)) break;

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
	if (uarmf && uarmf->oartifact == ART_KILLCAP) canhitmon = 0;

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
	    objenchant = GushLevel / 4;

	tmp = (5 + rnd(6) + (techlevX(tech_no) / 5));
	
	chasm = maketrap(u.ux + u.dx, u.uy + u.dy, PIT, 0, FALSE);
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
	num = 50 + (2 * techlevX(tech_no));
    	techt_inuse(tech_no) = num + 1;
	u.uenmax += num;
	u.uen += num;
	if (u.uen > u.uenmax) u.uen = u.uenmax;
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

	/* the damage was way too low... no wonder players never use finishers as the last chained blitz move,
	 * because pummel is all-powerful! Therefore I increased the damage --Amy */
	num = 10 + (techlevX(tech_no) * 3);
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

/* for bad effects: add d50000 turns of timeout to a random tech */
void
eviltechincrease()
{
	int i, thisone, choicenumber;

	thisone = -1;
	choicenumber = 0;

	for (i = 0; i < MAXTECH; i++) {

		if (tech_list[i].t_id == NO_TECH) break;

		if (!choicenumber || (!rn2(choicenumber + 1))) {
			thisone = i;
		}
		choicenumber++;

	}

	if (choicenumber > 0 && thisone >= 0) {
		techtout(thisone) += rnd(50000);
		pline("Your %s technique is put on a lengthy timeout!", techname(thisone));
	}

}

void
techcapincrease(increaseamount)
int increaseamount;
{
	int i, thisone, choicenumber;

	thisone = -1;
	choicenumber = 0;

	for (i = 0; i < MAXTECH; i++) {

		if (tech_list[i].t_id == NO_TECH) break;

		if (!choicenumber || (!rn2(choicenumber + 1))) {
			thisone = i;
		}
		choicenumber++;

	}

	/* make this effect actually matter --Amy */
	if (!rn2(2) && choicenumber > 1) {
		increaseamount *= (50 + choicenumber);
		increaseamount /= 50;
	}
	increaseamount *= rnd(5);

	if (choicenumber > 0 && thisone >= 0) {
		techtout(thisone) += increaseamount;
		pline("Your %s technique's timeout increases!", techname(thisone));
	}

}

void
techdrain()
{

	int i, thisone, choicenumber, attempts;
	attempts = 0;

techdrainnew:
	thisone = -1;
	choicenumber = 0;

	for (i = 0; i < MAXTECH; i++) {

		if (tech_list[i].t_id == NO_TECH) break;

		if (!choicenumber || (!rn2(choicenumber + 1))) {
			thisone = i;
		}
		choicenumber++;

	}

	if (choicenumber > 0 && thisone >= 0 && attempts < 10000) {
		if (tech_list[thisone].t_lev < 130) {
			tech_list[thisone].t_lev += 1;
			pline("Your %s technique's level is drained to %d!", techname(thisone), techlev(thisone));
		} else {
			attempts++;
			goto techdrainnew;
		}
	}

}

void
techdrainsevere()
{

	int i, thisone, choicenumber, attempts;
	attempts = 0;

techdrainnewS:
	thisone = -1;
	choicenumber = 0;

	for (i = 0; i < MAXTECH; i++) {

		if (tech_list[i].t_id == NO_TECH) break;

		if (!choicenumber || (!rn2(choicenumber + 1))) {
			thisone = i;
		}
		choicenumber++;

	}

	if (choicenumber > 0 && thisone >= 0 && attempts < 10000) {
		if (tech_list[thisone].t_lev < 120) {
			tech_list[thisone].t_lev = 130;
			pline("Your %s technique becomes permanently unusable!", techname(thisone));
		} else {
			attempts++;
			goto techdrainnewS;
		}
	}

}

void
techdrainrll()
{
	int i;
	for (i = 0; i < MAXTECH; i++) {
	    if (techid(i) == T_RESTORE_LIFE_LEVELS )
		tech_list[i].t_lev = 130;
	}

}

/* drain all of the player's techniques by one level --Amy */
void
drain_all_techs()
{
	int i;
	for (i = 0; i < MAXTECH; i++) {

		if (tech_list[i].t_id == NO_TECH) break;

		if (tech_list[i].t_lev < 130) {
			tech_list[i].t_lev += 1;
		}
	}

}

void
techlevelup()
{

	int i, thisone, choicenumber, attempts;
	attempts = 0;

techlvlupnew:
	thisone = -1;
	choicenumber = 0;

	for (i = 0; i < MAXTECH; i++) {

		if (tech_list[i].t_id == NO_TECH) break;

		if (!choicenumber || (!rn2(choicenumber + 1))) {
			thisone = i;
		}
		choicenumber++;

	}

	if (choicenumber > 0 && thisone >= 0 && attempts < 10000) {
		if (tech_list[thisone].t_lev > 0) {
			tech_list[thisone].t_lev -= 1;
			pline("Your %s technique leveled up to level %d!", techname(thisone), techlev(thisone));
		} else {
			attempts++;
			goto techlvlupnew;
		}
	}

}

void
techlevelspecific(technumber)
int technumber;
{

	int i, thisone, choicenumber, attempts;
	attempts = 0;

techlvlupnew:

	thisone = -1;
	boolean foundtech = FALSE;

	for (i = 0; i < MAXTECH; i++) {

		if (tech_list[i].t_id == NO_TECH) break;
		if (tech_list[i].t_id == technumber) {
			thisone = i;
			foundtech = TRUE;
		}

	}

	if (foundtech && thisone >= 0) {
		if (tech_list[thisone].t_lev > 0) {
			tech_list[thisone].t_lev -= 1;
			pline("Your %s technique leveled up to level %d!", techname(thisone), techlev(thisone));
		} else {
			pline("Your %s technique has reached the maximum level already.", techname(thisone));
		}
	}

}

/* player learns a random "regular" tech, i.e. one not taught by leveling up skills or some such --Amy */
void
learnrandomregulartech()
{
	switch (rnd(190)) {

		case 1: 
		case 2: 
		case 3: 
			learntech_or_leveltech(T_BERSERK, FROMOUTSIDE, 1);
		    	You("learn how to perform berserk!");
			break;
		case 4: 
		case 5: 
		case 6: 
			learntech_or_leveltech(T_KIII, FROMOUTSIDE, 1);
		    	You("learn how to perform kiii!");
			break;
		case 7: 
		case 8: 
		case 9: 
			learntech_or_leveltech(T_RESEARCH, FROMOUTSIDE, 1);
		    	You("learn how to perform research!");
			break;
		case 10: 
		case 11: 
		case 12: 
			learntech_or_leveltech(T_SURGERY, FROMOUTSIDE, 1);
		    	You("learn how to perform surgery!");
			break;
		case 13: 
		case 14: 
		case 15: 
			learntech_or_leveltech(T_REINFORCE, FROMOUTSIDE, 1);
		    	You("learn how to perform reinforce memory!");
			break;
		case 16: 
		case 17:
		case 18: 
			learntech_or_leveltech(T_FLURRY, FROMOUTSIDE, 1);
		    	You("learn how to perform missile flurry!");
			break;
		case 19: 
		case 20: 
		case 21: 
			learntech_or_leveltech(T_PRACTICE, FROMOUTSIDE, 1);
		    	You("learn how to perform weapon practice!");
			break;
		case 22: 
		case 23: 
		case 24: 
			learntech_or_leveltech(T_EVISCERATE, FROMOUTSIDE, 1);
		    	You("learn how to perform eviscerate!");
			break;
		case 25: 
		case 26: 
		case 27: 
			learntech_or_leveltech(T_HEAL_HANDS, FROMOUTSIDE, 1);
		    	You("learn how to perform healing hands!");
			break;
		case 28: 
		case 29: 
		case 30: 
			learntech_or_leveltech(T_CALM_STEED, FROMOUTSIDE, 1);
		    	You("learn how to perform calm steed!");
			break;
		case 31: 
		case 32: 
		case 33: 
			learntech_or_leveltech(T_TURN_UNDEAD, FROMOUTSIDE, 1);
		    	You("learn how to perform turn undead!");
			break;
		case 34: 
		case 35: 
		case 36: 
			learntech_or_leveltech(T_VANISH, FROMOUTSIDE, 1);
		    	You("learn how to perform vanish!");
			break;
		case 37: 
		case 38: 
		case 39: 
			learntech_or_leveltech(T_CUTTHROAT, FROMOUTSIDE, 1);
		    	You("learn how to perform cutthroat!");
			break;
		case 40: 
		case 41: 
			learntech_or_leveltech(T_BLESSING, FROMOUTSIDE, 1);
		    	You("learn how to perform blessing!");
			break;
		case 42: 
		case 43: 
		case 44: 
			learntech_or_leveltech(T_E_FIST, FROMOUTSIDE, 1);
		    	You("learn how to perform elemental fist!");
			break;
		case 45: 
		case 46: 
		case 47: 
			learntech_or_leveltech(T_PRIMAL_ROAR, FROMOUTSIDE, 1);
		    	You("learn how to perform primal roar!");
			break;
		case 48: 
		case 49: 
			learntech_or_leveltech(T_LIQUID_LEAP, FROMOUTSIDE, 1);
		    	You("learn how to perform liquid leap!");
			break;
		case 50: 
		case 51: 
		case 52: 
			learntech_or_leveltech(T_CRIT_STRIKE, FROMOUTSIDE, 1);
		    	You("learn how to perform critical strike!");
			break;
		case 53: 
		case 54: 
		case 55: 
			learntech_or_leveltech(T_SIGIL_CONTROL, FROMOUTSIDE, 1);
		    	You("learn how to perform sigil of control!");
			break;
		case 56: 
		case 57: 
		case 58: 
			learntech_or_leveltech(T_SIGIL_TEMPEST, FROMOUTSIDE, 1);
		    	You("learn how to perform sigil of tempest!");
			break;
		case 59: 
		case 60: 
		case 61: 
			learntech_or_leveltech(T_SIGIL_DISCHARGE, FROMOUTSIDE, 1);
		    	You("learn how to perform sigil of discharge!");
			break;
		case 62: 
		case 63: 
		case 64: 
			learntech_or_leveltech(T_RAISE_ZOMBIES, FROMOUTSIDE, 1);
		    	You("learn how to perform raise zombies!");
			break;
		case 65: 
			learntech_or_leveltech(T_REVIVE, FROMOUTSIDE, 1);
		    	You("learn how to perform revivification!");
			break;
		case 66: 
		case 67: 
		case 68: 
			learntech_or_leveltech(T_WARD_FIRE, FROMOUTSIDE, 1);
		    	You("learn how to perform ward against fire!");
			break;
		case 69: 
		case 70: 
		case 71: 
			learntech_or_leveltech(T_WARD_COLD, FROMOUTSIDE, 1);
		    	You("learn how to perform ward against cold!");
			break;
		case 72: 
		case 73: 
		case 74: 
			learntech_or_leveltech(T_WARD_ELEC, FROMOUTSIDE, 1);
		    	You("learn how to perform ward against electricity!");
			break;
		case 75: 
		case 76: 
		case 77: 
			learntech_or_leveltech(T_TINKER, FROMOUTSIDE, 1);
		    	You("learn how to perform tinker!");
			break;
		case 78: 
		case 79: 
		case 80: 
			learntech_or_leveltech(T_RAGE, FROMOUTSIDE, 1);
		    	You("learn how to perform rage eruption!");
			break;
		case 81: 
		case 82: 
		case 83: 
			learntech_or_leveltech(T_BLINK, FROMOUTSIDE, 1);
		    	You("learn how to perform blink!");
			break;
		case 84: 
		case 85: 
		case 86: 
			learntech_or_leveltech(T_CHI_STRIKE, FROMOUTSIDE, 1);
		    	You("learn how to perform chi strike!");
			break;
		case 87: 
		case 88: 
		case 89: 
			learntech_or_leveltech(T_DRAW_ENERGY, FROMOUTSIDE, 1);
		    	You("learn how to perform draw energy!");
			break;
		case 90: 
		case 91: 
		case 92: 
			learntech_or_leveltech(T_CHI_HEALING, FROMOUTSIDE, 1);
		    	You("learn how to perform chi healing!");
			break;
		case 93: 
		case 94: 
		case 95: 
			learntech_or_leveltech(T_DAZZLE, FROMOUTSIDE, 1);
		    	You("learn how to perform dazzle!");
			break;
		case 96: 
		case 97: 
		case 98: 
			learntech_or_leveltech(T_BLITZ, FROMOUTSIDE, 1);
		    	You("learn how to perform chained blitz!");
			break;
		case 99: 
		case 100: 
		case 101: 
			learntech_or_leveltech(T_PUMMEL, FROMOUTSIDE, 1);
		    	You("learn how to perform pummel!");
			break;
		case 102: 
		case 103: 
		case 104: 
			learntech_or_leveltech(T_G_SLAM, FROMOUTSIDE, 1);
		    	You("learn how to perform ground slam!");
			break;
		case 105: 
		case 106: 
		case 107: 
			learntech_or_leveltech(T_DASH, FROMOUTSIDE, 1);
		    	You("learn how to perform air dash!");
			break;
		case 108: 
		case 109: 
		case 110: 
			learntech_or_leveltech(T_POWER_SURGE, FROMOUTSIDE, 1);
		    	You("learn how to perform power surge!");
			break;
		case 111: 
		case 112: 
		case 113: 
			learntech_or_leveltech(T_SPIRIT_BOMB, FROMOUTSIDE, 1);
		    	You("learn how to perform spirit bomb!");
			break;
		case 114: 
		case 115: 
		case 116: 
			learntech_or_leveltech(T_DRAW_BLOOD, FROMOUTSIDE, 1);
		    	You("learn how to perform draw blood!");
			break;
		case 117: 
			learntech_or_leveltech(T_WORLD_FALL, FROMOUTSIDE, 1);
		    	You("learn how to perform world fall!");
			break;
		case 118: 
		case 119: 
		case 120: 
			learntech_or_leveltech(T_CREATE_AMMO, FROMOUTSIDE, 1);
		    	You("learn how to perform create ammo!");
			break;
		case 121: 
		case 122: 
		case 123: 
			learntech_or_leveltech(T_POKE_BALL, FROMOUTSIDE, 1);
		    	You("learn how to perform poke ball!");
			break;
		case 124: 
		case 125: 
		case 126: 
			learntech_or_leveltech(T_ATTIRE_CHARM, FROMOUTSIDE, 1);
		    	You("learn how to perform attire charm!");
			break;
		case 127: 
		case 128: 
		case 129: 
			learntech_or_leveltech(T_SUMMON_TEAM_ANT, FROMOUTSIDE, 1);
		    	You("learn how to perform summon team ant!");
			break;
		case 130: 
		case 131: 
		case 132: 
			learntech_or_leveltech(T_JEDI_JUMP, FROMOUTSIDE, 1);
		    	You("learn how to perform jedi jump!");
			break;
		case 133: 
		case 134: 
		case 135: 
			learntech_or_leveltech(T_CHARGE_SABER, FROMOUTSIDE, 1);
		    	You("learn how to perform charge saber!");
			break;
		case 136: 
		case 137: 
		case 138: 
			learntech_or_leveltech(T_TELEKINESIS, FROMOUTSIDE, 1);
		    	You("learn how to perform telekinesis!");
			break;
		case 139: 
			learntech_or_leveltech(T_EGG_BOMB, FROMOUTSIDE, 1);
		    	You("learn how to perform egg bomb!");
			break;
		case 140: 
		case 141: 
		case 142: 
			learntech_or_leveltech(T_BOOZE, FROMOUTSIDE, 1);
		    	You("learn how to perform booze!");
			break;
		case 143: 
		case 144: 
		case 145: 
			learntech_or_leveltech(T_IRON_SKIN, FROMOUTSIDE, 1);
		    	You("learn how to perform iron skin!");
			break;
		case 146: 
			learntech_or_leveltech(T_POLYFORM, FROMOUTSIDE, 1);
		    	You("learn how to perform polyform!");
			break;
		case 147: 
		case 148: 
		case 149: 
			learntech_or_leveltech(T_CONCENTRATING, FROMOUTSIDE, 1);
		    	You("learn how to perform concentrating!");
			break;
		case 150: 
			learntech_or_leveltech(T_SUMMON_PET, FROMOUTSIDE, 1);
		    	You("learn how to perform summon pet!");
			break;
		case 151: 
		case 152: 
			learntech_or_leveltech(T_DOUBLE_THROWNAGE, FROMOUTSIDE, 1);
		    	You("learn how to perform double thrownage!");
			break;
		case 153: 
		case 154: 
		case 155: 
			learntech_or_leveltech(T_SHIELD_BASH, FROMOUTSIDE, 1);
		    	You("learn how to perform shield bash!");
			break;
		case 156: 
		case 157: 
			learntech_or_leveltech(T_RECHARGE, FROMOUTSIDE, 1);
		    	You("learn how to perform recharge!");
			break;
		case 158: 
		case 159: 
		case 160: 
			learntech_or_leveltech(T_SPIRITUALITY_CHECK, FROMOUTSIDE, 1);
		    	You("learn how to perform spirituality check!");
			break;
		case 161: 
			learntech_or_leveltech(T_EDDY_WIND, FROMOUTSIDE, 1);
		    	You("learn how to perform eddy wind!");
			break;
		case 162: 
			learntech_or_leveltech(T_BLOOD_RITUAL, FROMOUTSIDE, 1);
		    	You("learn how to perform blood ritual!");
			break;
		case 163: 
			learntech_or_leveltech(T_ENT_S_POTION, FROMOUTSIDE, 1);
		    	You("learn how to perform ent's potion!");
			break;
		case 164: 
		case 165: 
			learntech_or_leveltech(T_LUCKY_GAMBLE, FROMOUTSIDE, 1);
		    	You("learn how to perform lucky gamble!");
			break;
		case 166: 
			learntech_or_leveltech(T_DECONTAMINATE, FROMOUTSIDE, 1);
		    	You("learn how to perform decontaminate!");
			break;
		case 167:
			learntech_or_leveltech(T_WONDERSPELL, FROMOUTSIDE, 1);
		    	You("learn how to perform wonderspell!");
			break;
		case 168:
			learntech_or_leveltech(T_RESET_TECHNIQUE, FROMOUTSIDE, 1);
		    	You("learn how to perform reset technique!");
			break;
		case 169:
		case 170:
			learntech_or_leveltech(T_DIAMOND_BARRIER, FROMOUTSIDE, 1);
		    	You("learn how to perform diamond barrier!");
			break;
		case 171:
		case 172:
		case 173:
			learntech_or_leveltech(T_ZAP_EM, FROMOUTSIDE, 1);
		    	You("learn how to perform zap em!");
			break;
		case 174:
		case 175:
		case 176:
			learntech_or_leveltech(T_CARD_TRICK, FROMOUTSIDE, 1);
		    	You("learn how to perform card trick!");
			break;
		case 177:
			learntech_or_leveltech(T_SKILLOMORPH, FROMOUTSIDE, 1);
		    	You("learn how to perform skillomorph!");
			break;
		case 178:
		case 179:
		case 180:
			learntech_or_leveltech(T_TERRAIN_CLEANUP, FROMOUTSIDE, 1);
		    	You("learn how to perform terrain cleanup!");
			break;
		case 181:
		case 182:
		case 183:
			learntech_or_leveltech(T_PREACHING, FROMOUTSIDE, 1);
		    	You("learn how to perform preaching!");
			break;
		case 184:
		case 185:
		case 186:
			learntech_or_leveltech(T_ON_THE_SAME_TEAM, FROMOUTSIDE, 1);
		    	You("learn how to perform on the same team!");
			break;
		case 187:
			learntech_or_leveltech(T_PERMAMORPH, FROMOUTSIDE, 1);
		    	You("learn how to perform permamorph!");
			break;
		case 188:
			learntech_or_leveltech(T_SPELL_SPAM, FROMOUTSIDE, 1);
		    	You("learn how to perform spell spam!");
			break;
		case 189:
			learntech_or_leveltech(T_ROCK_TO_POISON, FROMOUTSIDE, 1);
		    	You("learn how to perform rock-to-poison!");
			break;
		case 190:
			learntech_or_leveltech(T_HARDCORE_ALIENIZATION, FROMOUTSIDE, 1);
		    	You("learn how to perform hardcore alienization!");
			break;

		default:
			break;

	}

}

/* can you use a martial arts technique? normally you have to be bare-handed, but there may be exceptions --Amy */
boolean
canuseunarmedtechs()
{
	/* dual-wielding prevents it entirely */
	if (u.twoweap && uswapwep) return FALSE;

	/* godae musul is a special artifact that is suitable for martial arts moves */
	if (uwep && uwep->oartifact == ART_GODAE_MUSUL) return TRUE;
	/* and so is blammer of doom */
	if (uwep && uwep->oartifact == ART_BLAMMER_OF_DOOM) return TRUE;

	/* supermarket cashier can use martial arts moves with all tin openers because he knows "tin-fu" */
	if (uwep && Role_if(PM_SUPERMARKET_CASHIER) && (uwep->otyp == LASER_TIN_OPENER || uwep->otyp == TIN_OPENER || uwep->otyp == BUDO_NO_SASU || uwep->otyp == JEONTU_GEOM) ) return TRUE;

	if (u.martialstyle == MARTIALSTYLE_HAIDONGGUMDO && uwep && uwep->otyp == JEONTU_GEOM) return TRUE;

	/* everyone else needs to be bare-handed */
	if (uwep) return FALSE;

	return TRUE;
}

void
resettechniqueextra()
{

	int numtechs;
	pline("Choose a technique to reset. The prompt will loop until you actually make a choice.");
resetretrying:
	for (numtechs = 0; numtechs < MAXTECH && techid(numtechs) != NO_TECH; numtechs++) {
		if (techid(numtechs) == T_RESET_TECHNIQUE) continue;
		if (techtout(numtechs) > 0) {
			pline("Your %s technique is currently on timeout.", techname(numtechs));
			if (yn("Make this technique usable again?") == 'y') {
				techtout(numtechs) = 0;
				pline("The timeout on your %s technique has been set to zero.", techname(numtechs));
				goto resettechdone;
			}
		}
	}
	pline("If you don't feel like resetting a technique right now, you can choose to reset none at all.");
	if (yn("Do you want to reset no technique at all?") == 'y') {
		goto resettechdone;
	}
	goto resetretrying;

resettechdone:
	return;
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
