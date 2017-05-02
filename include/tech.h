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
	xchar       t_lev;                 /* power level 
					    *   This is the level 
					    *   you were when you got the tech
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
        int NDECL((*blitz_funct)); /* function called when the command is executed */
        const int blitz_tech; /* the tech designation - determines tech needed
         		       * to access this blitz and the name of the blitz
          		       */
        const int blitz_type; /* the type of blitz chain */
#define BLITZ_START 	0 /* Starts the chain */
#define BLITZ_CHAIN 	1 /* Goes anywhere in the chain (usually middle) */
#define BLITZ_END 	2 /* Finishes the chain */
};


#define NO_TECH 	0
#define T_BERSERK 	1
#define T_KIII 		2
#define T_RESEARCH 	3
#define T_SURGERY 	4
#define T_REINFORCE 	5
#define T_FLURRY 	6
#define T_PRACTICE 	7
#define T_EVISCERATE 	8
#define T_HEAL_HANDS 	9
#define T_CALM_STEED 	10
#define T_TURN_UNDEAD 	11
#define T_VANISH 	12
#define T_CUTTHROAT 	13
#define T_BLESSING 	14
#define T_E_FIST 	15
#define T_PRIMAL_ROAR 	16
#define T_LIQUID_LEAP 	17
#define T_CRIT_STRIKE 	18
#define T_SIGIL_CONTROL 19
#define T_SIGIL_TEMPEST 20
#define T_SIGIL_DISCHARGE 21
#define T_RAISE_ZOMBIES 22
#define T_REVIVE 	23
#define T_WARD_FIRE 	24
#define T_WARD_COLD 	25
#define T_WARD_ELEC 	26
#define T_TINKER 	27
#define T_RAGE	 	28
#define T_BLINK	 	29
#define T_CHI_STRIKE 	30
#define T_DRAW_ENERGY 	31
#define T_CHI_HEALING 	32
#define T_DISARM 	33
#define T_DAZZLE 	34
#define T_BLITZ 	35
#define T_PUMMEL 	36
#define T_G_SLAM 	37
#define T_DASH	 	38
#define T_POWER_SURGE 	39
#define T_SPIRIT_BOMB 	40
#define T_DRAW_BLOOD	41

#define T_WORLD_FALL	42
#define T_CREATE_AMMO	43
#define T_POKE_BALL	44
#define T_ATTIRE_CHARM	45
#define T_SUMMON_TEAM_ANT	46
#define T_APPRAISAL	47
#define T_EGG_BOMB	48
#define T_BOOZE	49

#define T_INVOKE_DEITY	50

#define T_DOUBLE_TROUBLE	51

#define T_PHASE_DOOR	52

#define T_SECURE_IDENTIFY	53

#define T_IRON_SKIN	54
#define T_POLYFORM	55
#define T_CONCENTRATING	56
#define T_SUMMON_PET	57
#define T_DOUBLE_THROWNAGE	58
#define T_SHIELD_BASH	59
#define T_RECHARGE	60
#define T_SPIRITUALITY_CHECK	61
#define T_EDDY_WIND	62

#define T_BLOOD_RITUAL	63
#define T_ENT_S_POTION	64
#define T_LUCKY_GAMBLE	65

#define T_PANIC_DIGGING	66

#define T_JEDI_JUMP	67
#define T_CHARGE_SABER	68
#define T_TELEKINESIS	69
 
#define MAXTECH 70

#endif /* TECH_H */
