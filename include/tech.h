/*      SCCS Id: @(#)tech.h    3.2     98/Oct/30                  */
/* Original Code by Warren Cheung                                 */
/* Copyright 1986, M. Stephenson				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef TECH_H
#define TECH_H

/* An array of this struct holds your current techs */
struct tech {
	int         t_id;                  /* tech id 
					    * 	Unique number for the tech
					    *	see the #defines below
					    */
	int       t_lev;                 /* power level 
					    *   This is the level 
					    *   you were when you got the tech
					    * note by Amy: higher levels mean a lower displayed level in the menu!!!
				            */
	
	int         t_tout;                /* timeout - time until tech can be 
					    * used again 
					    */
	int         t_inuse;               /* time till effect stops 
					    */
	long        t_intrinsic;           /* Source(s) */
#define OUTSIDE_LEVEL	TIMEOUT            /* Level "from outside" started at */
};

struct innate_tech {
	schar   ulevel; /* gains tech at level */
	short   tech_id; /* the tech unique ID*/ 
	int	tech_lev; /* starts at level */
};

struct blitz_tab {
        const char *blitz_cmd; /* the typed command */
        const int blitz_len; /* The length of blitz_cmd */
        int (*blitz_funct)(void); /* function called when the command is executed */
        const int blitz_tech; /* the tech designation - determines tech needed
         		       * to access this blitz and the name of the blitz
          		       */
        const int blitz_type; /* the type of blitz chain */
#define BLITZ_START 	0 /* Starts the chain */
#define BLITZ_CHAIN 	1 /* Goes anywhere in the chain (usually middle) */
#define BLITZ_END 	2 /* Finishes the chain */
};


#define NO_TECH 	0 /* annotation "lvlup" means tech can be given in tech.c for various roles and races */
#define T_BERSERK 	1 /* lvlup */
#define T_KIII 		2 /* lvlup */
#define T_RESEARCH 	3 /* lvlup */
#define T_SURGERY 	4 /* lvlup */
#define T_REINFORCE 	5 /* lvlup */
#define T_FLURRY 	6 /* lvlup */
#define T_PRACTICE 	7 /* lvlup */
#define T_EVISCERATE 	8 /* lvlup */
#define T_HEAL_HANDS 	9 /* lvlup */
#define T_CALM_STEED 	10 /* lvlup */
#define T_TURN_UNDEAD 	11 /* lvlup */
#define T_VANISH 	12 /* lvlup */
#define T_CUTTHROAT 	13 /* lvlup */
#define T_BLESSING 	14 /* lvlup */
#define T_E_FIST 	15 /* lvlup */
#define T_PRIMAL_ROAR 	16 /* lvlup */
#define T_LIQUID_LEAP 	17 /* lvlup */
#define T_CRIT_STRIKE 	18 /* lvlup */
#define T_SIGIL_CONTROL 19 /* lvlup */
#define T_SIGIL_TEMPEST 20 /* lvlup */
#define T_SIGIL_DISCHARGE 21 /* lvlup */
#define T_RAISE_ZOMBIES 22 /* lvlup */
#define T_REVIVE 	23 /* lvlup */
#define T_WARD_FIRE 	24 /* lvlup */
#define T_WARD_COLD 	25 /* lvlup */
#define T_WARD_ELEC 	26 /* lvlup */
#define T_TINKER 	27 /* lvlup */
#define T_RAGE	 	28 /* lvlup */
#define T_BLINK	 	29 /* lvlup */
#define T_CHI_STRIKE 	30 /* lvlup */
#define T_DRAW_ENERGY 	31 /* lvlup */
#define T_CHI_HEALING 	32 /* lvlup */
#define T_DISARM 	33
#define T_DAZZLE 	34 /* lvlup */
#define T_BLITZ 	35 /* lvlup */
#define T_PUMMEL 	36 /* lvlup */
#define T_G_SLAM 	37 /* lvlup */
#define T_DASH	 	38 /* lvlup */
#define T_POWER_SURGE 	39 /* lvlup */
#define T_SPIRIT_BOMB 	40 /* lvlup */
#define T_DRAW_BLOOD	41 /* lvlup */

#define T_WORLD_FALL	42 /* lvlup */
#define T_CREATE_AMMO	43 /* lvlup */
#define T_POKE_BALL	44 /* lvlup */
#define T_ATTIRE_CHARM	45 /* lvlup */
#define T_SUMMON_TEAM_ANT	46 /* lvlup */
#define T_APPRAISAL	47
#define T_EGG_BOMB	48 /* lvlup */
#define T_BOOZE	49 /* lvlup */

#define T_INVOKE_DEITY	50

#define T_DOUBLE_TROUBLE	51 /* lvlup */

#define T_PHASE_DOOR	52

#define T_SECURE_IDENTIFY	53

#define T_IRON_SKIN	54 /* lvlup */
#define T_POLYFORM	55 /* lvlup */
#define T_CONCENTRATING	56 /* lvlup */
#define T_SUMMON_PET	57 /* lvlup */
#define T_DOUBLE_THROWNAGE	58 /* lvlup */
#define T_SHIELD_BASH	59 /* lvlup */
#define T_RECHARGE	60 /* lvlup */
#define T_SPIRITUALITY_CHECK	61 /* lvlup */
#define T_EDDY_WIND	62 /* lvlup */

#define T_BLOOD_RITUAL	63 /* lvlup */
#define T_ENT_S_POTION	64 /* lvlup */
#define T_LUCKY_GAMBLE	65 /* lvlup */

#define T_PANIC_DIGGING	66

#define T_DECONTAMINATE	67 /* lvlup */
#define T_WONDERSPELL	68 /* lvlup */
#define T_RESET_TECHNIQUE	69 /* lvlup */

#define T_SILENT_OCEAN	70
#define T_GLOWHORN	71
#define T_INTRINSIC_ROULETTE	72
#define T_SPECTRAL_SWORD	73
#define T_REVERSE_IDENTIFY	74
#define T_DETECT_TRAPS	75
#define T_DIRECTIVE	76
#define T_REMOVE_IMPLANT	77
#define T_REROLL_IMPLANT	78
#define T_TIME_STOP	79
#define T_STAT_RESIST	80
#define T_SWAP_WEAPON	81
#define T_MILDEN_CURSE	82
#define T_FORCE_FIELD	83
#define T_POINTINESS	84
#define T_BUG_SPRAY	85
#define T_WHIRLSTAFF	86
#define T_DELIBERATE_CURSE	87
#define T_ACQUIRE_STEED	88
#define T_SADDLING	89
#define T_SHOPPING_QUEEN	90
#define T_BEAUTY_CHARM	91
#define T_ASIAN_KICK	92
#define T_LEGSCRATCH_ATTACK	93
#define T_GROUND_STOMP	94
#define T_ATHLETIC_COMBAT	95
#define T_PRAYING_SUCCESS	96
#define T_OVER_RAY	97
#define T_ENCHANTERANG	98
#define T_BATMAN_ARSENAL	99
#define T_JOKERBANE	100
#define T_CALL_THE_POLICE	101
#define T_DOMINATE	102
#define T_INCARNATION	103
#define T_COMBO_STRIKE	104
#define T_FUNGOISM	105
#define T_BECOME_UNDEAD	106
#define T_JIU_JITSU	107
#define T_BLADE_ANGER	108
#define T_RE_TAMING	109
#define T_UNCURSE_SABER	110
#define T_ENERGY_CONSERVATION	111
#define T_ENCHANT_ROBE	112
#define T_WILD_SLASHING	113
#define T_ABSORBER_SHIELD	114
#define T_PSYCHO_FORCE	115
#define T_INTENSIVE_TRAINING	116
#define T_SURRENDER_OR_DIE	117
#define T_PERILOUS_WHIRL	118
#define T_SUMMON_SHOE	119
#define T_KICK_IN_THE_NUTS	120
#define T_DISARMING_KICK	121
#define T_INLAY_WARFARE	122

#define T_DIAMOND_BARRIER	123 /* lvlup */

#define T_STEADY_HAND	124
#define T_FORCE_FILLING	125
#define T_JEDI_TAILORING	126
#define T_INTRINSIC_SACRIFICE	127
#define T_BEAMSWORD	128
#define T_ENERGY_TRANSFER	129
#define T_SOFTEN_TARGET	130
#define T_PROTECT_WEAPON	131
#define T_POWERFUL_AURA	132
#define T_BOOSTAFF	133
#define T_CLONE_JAVELIN	134
#define T_REFUGE	135
#define T_DRAINING_PUNCH	136
#define T_ESCROBISM	137
#define T_PIRATE_BROTHERING	138
#define T_NUTS_AND_BOLTS	139
#define T_DECAPABILITY	140
#define T_NO_HANDS_CURSE	141
#define T_HIGH_HEELED_SNEAKERS	142
#define T_FORM_CHOICE	143
#define T_STAR_DIGGING	144
#define T_STARWARS_FRIENDS	145
#define T_USE_THE_FORCE_LUKE	146

#define T_WONDER_YONDER	147

#define T_ZAP_EM	148 /* lvlup */

#define T_CARD_TRICK	149 /* lvlup */
#define T_SKILLOMORPH	150 /* lvlup */

#define T_SHOTTY_BLAST	151
#define T_AMMO_UPGRADE	152
#define T_LASER_POWER	153
#define T_BIG_DADDY	154
#define T_SHUT_THAT_BITCH_UP	155
#define T_S_PRESSING	156
#define T_MELTEE	157
#define T_WOMAN_NOISES	158
#define T_EXTRA_LONG_SQUEAK	159
#define T_SEXUAL_HUG	160
#define T_SEX_CHANGE	161
#define T_EVEN_MORE_AMMO	162
#define T_DOUBLESELF	163
#define T_POLYFIX	164
#define T_SQUEAKY_REPAIR	165
#define T_BULLETREUSE	166
#define T_EXTRACHARGE	167

#define T_TERRAIN_CLEANUP	168 /* lvlup */

#define T_SYMBIOSIS	169
#define T_ADJUST_SYMBIOTE	170
#define T_HEAL_SYMBIOTE	171
#define T_BOOST_SYMBIOTE	172
#define T_POWERBIOSIS	173
#define T_IMPLANTED_SYMBIOSIS	174
#define T_ASSUME_SYMBIOTE	175
#define T_GENERATE_OFFSPRING	176

#define T_UNDERTOW	177
#define T_DEFINALIZE	178
#define T_ANTI_INERTIA	179
#define T_GRENADES_OF_COURSE	180

#define T_PREACHING	181 /* lvlup */
#define T_ON_THE_SAME_TEAM	182 /* lvlup */
#define T_PERMAMORPH	183 /* lvlup */

#define T_PACIFY	184
#define T_AFTERBURNER	185
#define T_BUGGARD	186
#define T_THUNDERSTORM	187
#define T_AUTOKILL	188
#define T_CHAIN_THUNDERBOLT	189
#define T_FLASHING_MISCHIEF	190
#define T_KAMEHAMEHA	191
#define T_SHADOW_MANTLE	192
#define T_VACUUM_STAR	193
#define T_BLADE_SHIELD	194
#define T_GREEN_WEAPON	195
#define T_BALLSLIFF	196
#define T_POLE_MELEE	197
#define T_CHOP_CHOP	198
#define T_BANISHMENT	199
#define T_PARTICIPATION_LOSS	200
#define T_WEAPON_BLOCKER	201
#define T_EXTRA_MEMORY	202
#define T_GRAP_SWAP	203
#define T_DIABOLIC_MINION	204

#define T_CURE_AMNESIA	205
#define T_ELEMENTAL_IMBUE	206

#define T_HIDDEN_POWER	207

#define T_SWORD_ART	208
#define T_FIRM_CUDGEL	209

#define T_LIGHTER_BALLS	210

#define T_VENOM_MIXING	211
#define T_JAVELIN_FORGING	212

#define T_CORONATION_CULMINATION	213
#define T_RESTORE_LIFE_LEVELS	214
#define T_MAKE_A_WISH	215

#define T_TOILET_VISIT	216

#define T_ROCK_TO_POISON	217 /* lvlup */

#define T_INJECTION	218
#define T_SUPER_POISON	219
#define T_GREEN_MISSILE	220
#define T_BIG_N_VEINY	221
#define T_PLANT_TERROR	222
#define T_POISON_PEN_LETTER	223

#define T_UNARMED_FOCUS	224

#define T_SPELL_SPAM	225 /* lvlup */

#define T_DROP_BOULDER	226
#define T_EARTHSHAKE	227

#define T_MARTIAL_STYLE	228

#define T_SPRINT	229
#define T_SKULL_CRUSH	230
#define T_FEMALE_COMBO	231
#define T_DOCKLOCK	232
#define T_BITCH_POSING	233
#define T_SEXY_STAND	234
#define T_MARATHON	235
#define T_PERFUME_STRIDE	236
#define T_NAUGHTY_HEELOT	237
#define T_EXTREME_STURDINESS	238
#define T_BUTT_PROTECTION	239
#define T_PROFILING	240
#define T_STAR_HEEL_SWAP	241
#define T_HEEL_STAB	242

#define T_HARDCORE_ALIENIZATION	243 /* lvlup */

#define T_BUC_TEST	244

#define T_JEDI_JUMP	245 /* lvlup */
#define T_CHARGE_SABER	246 /* lvlup */
#define T_TELEKINESIS	247 /* lvlup */
 
#define MAXTECH 248

/* by Amy: #monster abilities */

struct monsterabil {
	int abil_id;	/* ability id */
				/* Unique number for the ability, see the #defines below */
};

#define NO_ABILITY 0

#define ABIL_SNAIL_DIG	1
#define ABIL_STEFANJE_REPAIR	2
#define ABIL_ANASTASIA_DESEAMING	3
#define ABIL_KATI_CLEAN	4
#define ABIL_SOKO_BOULDER	5
#define ABIL_SOKO_DISARM	6
#define ABIL_POLY_BREATHE	7
#define ABIL_POLY_SPIT	8
#define ABIL_POLY_IRON_BALL	9
#define ABIL_POLY_GAZE	10
#define ABIL_WERE_SUMMON	11
#define ABIL_POLY_WEB	12
#define ABIL_POLY_HIDE	13
#define ABIL_POLY_MIND_BLAST	14
#define ABIL_POLY_GREMWATER	15
#define ABIL_POLY_GREMLAVA	16
#define ABIL_POLY_UNIHORN	17
#define ABIL_POLY_CONVERT	18
#define ABIL_POLY_WOUWOU	19
#define ABIL_POLY_WHORE	20
#define ABIL_POLY_SUPERMAN	21
#define ABIL_POLY_BONES	22
#define ABIL_POLY_SHRIEK	23
#define ABIL_POLY_FARTQUIET	24
#define ABIL_POLY_FARTNORMAL	25
#define ABIL_POLY_FARTLOUD	26
#define ABIL_NATALIA_MENS	27
#define ABIL_MOUNT_BOND	28
#define ABIL_HAND_PULL	29
#define ABIL_POLY_PERFUME	30
#define ABIL_HUSSY_CRAP	31
#define ABIL_IRAHA_POISON	32
#define ABIL_JUYO_TOGGLE_FLEE	33
#define ABIL_PETKEEPING_CONTROL_MAGIC	34
#define ABIL_JANITOR_CLEAN	35
#define ABIL_MUSHROOM_POLE	36
#define ABIL_MARTIAL_SWITCH	37
#define ABIL_DEMAGOGUE_RECURSION	38
#define ABIL_SYMBIOSIS_CHECK	39
#define ABIL_WEATHER_RAIN	40
#define ABIL_WEATHER_THUNDERSTORM	41
#define ABIL_WEATHER_SLEET	42
#define ABIL_WEATHER_SUNNY	43
#define ABIL_WEATHER_SANDSTORM	44
#define ABIL_WEATHER_NOCTEM	45
#define ABIL_WEATHER_OVERCAST	46
#define ABIL_PET_DMG_REDUC	47
#define ABIL_SYMBIO_DMG_REDUC	48

#define ABIL_EUTHANIZE_SYMBIOTE	49

#define MAX_ABILITY 50

#endif /* TECH_H */
