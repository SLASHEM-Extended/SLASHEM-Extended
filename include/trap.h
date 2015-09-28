/*	SCCS Id: @(#)trap.h	3.4	2000/08/30	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* note for 3.1.0 and later: no longer manipulated by 'makedefs' */

#ifndef TRAP_H
#define TRAP_H

union vlaunchinfo {
	short v_launch_otyp;	/* type of object to be triggered */
	coord v_launch2;	/* secondary launch point (for boulders) */
};

struct trap {
	struct trap *ntrap;
	xchar tx,ty;
	d_level dst;	/* destination for portals */
	coord launch;
	Bitfield(ttyp,8); /* are you kidding me? You only make room for 32 traps? That's not nearly enough for me! --Amy */
	Bitfield(tseen,1);
	Bitfield(hiddentrap,1); /* invisible, and I mean really invisible, even if you have see invis --Amy */
	Bitfield(once,1);
	Bitfield(madeby_u,1); /* So monsters may take offence when you trap
				 them.	Recognizing who made the trap isn't
				 completely unreasonable, everybody has
				 their own style.  This flag is also needed
				 when you untrap a monster.  It would be too
				 easy to make a monster peaceful if you could
				 set a trap for it and then untrap it. */
	union vlaunchinfo vl;
#define launch_otyp	vl.v_launch_otyp
#define launch2		vl.v_launch2
};

extern struct trap *ftrap;
#define newtrap()	(struct trap *) alloc(sizeof(struct trap))
#define dealloc_trap(trap) free((genericptr_t) (trap))

/* reasons for statue animation */
#define ANIMATE_NORMAL	0
#define ANIMATE_SHATTER 1
#define ANIMATE_SPELL	2

/* reasons for animate_statue's failure */
#define AS_OK		 0	/* didn't fail */
#define AS_NO_MON	 1	/* makemon failed */
#define AS_MON_IS_UNIQUE 2	/* statue monster is unique */

/* Note: if adding/removing a trap, adjust trap_engravings[] in mklev.c */

/* unconditional traps */
#define NO_TRAP		0
#define ARROW_TRAP	1
#define DART_TRAP	2
#define ROCKTRAP	3
#define SQKY_BOARD	4
#define BEAR_TRAP	5
#define LANDMINE	6
#define ROLLING_BOULDER_TRAP	7
#define SLP_GAS_TRAP	8
#define RUST_TRAP	9
#define FIRE_TRAP	10
#define PIT		11
#define SPIKED_PIT	12
#define HOLE		13
#define TRAPDOOR	14
#define TELEP_TRAP	15
#define LEVEL_TELEP	16
#define MAGIC_PORTAL	17
#define WEB		18
#define STATUE_TRAP	19
#define MAGIC_TRAP	20
#define ANTI_MAGIC	21
#define POLY_TRAP	22
#define ICE_TRAP	23
#define SPEAR_TRAP	24
#define COLLAPSE_TRAP	25
#define MAGIC_BEAM_TRAP	26
#define SHIT_TRAP	27
#define ANIMATION_TRAP	28
#define GLYPH_OF_WARDING	29
#define SCYTHING_BLADE	30
#define BOLT_TRAP	31
#define ACID_POOL	32
#define WATER_POOL	33
#define POISON_GAS_TRAP	34
#define SLOW_GAS_TRAP	35
#define SHOCK_TRAP	36
#define RMB_LOSS_TRAP		37
#define DISPLAY_TRAP		38
#define SPELL_LOSS_TRAP		39
#define YELLOW_SPELL_TRAP		40
#define AUTO_DESTRUCT_TRAP		41
#define MEMORY_TRAP		42
#define INVENTORY_TRAP		43
#define BLACK_NG_WALL_TRAP		44
#define MENU_TRAP		45
#define SPEED_TRAP		46
#define SUPERSCROLLER_TRAP		47
#define ACTIVE_SUPERSCROLLER_TRAP		48
#define SWARM_TRAP		49
#define AUTOMATIC_SWITCHER		50
#define UNKNOWN_TRAP		51
#define HEEL_TRAP		52
#define VULN_TRAP		53
#define LAVA_TRAP		54
#define FLOOD_TRAP		55
#define DRAIN_TRAP		56
#define FREE_HAND_TRAP		57
#define DISINTEGRATION_TRAP		58
#define UNIDENTIFY_TRAP		59
#define THIRST_TRAP		60
#define LUCK_TRAP		61
#define SHADES_OF_GREY_TRAP		62
#define ITEM_TELEP_TRAP		63
#define GRAVITY_TRAP		64
#define DEATH_TRAP		65
#define STONE_TO_FLESH_TRAP		66
#define QUICKSAND_TRAP		67
#define FAINT_TRAP		68
#define CURSE_TRAP		69
#define DIFFICULTY_TRAP		70
#define SOUND_TRAP		71
#define CASTER_TRAP		72
#define WEAKNESS_TRAP		73
#define ROT_THIRTEEN_TRAP		74
#define BISHOP_TRAP		75
#define CONFUSION_TRAP		76
#define NUPESELL_TRAP		77
#define DROP_TRAP		78
#define DSTW_TRAP		79
#define STATUS_TRAP		80
#define ALIGNMENT_TRAP		81
#define STAIRS_TRAP		82

#define PESTILENCE_TRAP		83
#define FAMINE_TRAP		84
#define RELOCATION_TRAP		85

#define UNINFORMATION_TRAP		86

#define INTRINSIC_LOSS_TRAP	87
#define BLOOD_LOSS_TRAP	88
#define BAD_EFFECT_TRAP	89
#define MULTIPLY_TRAP	90
#define AUTO_VULN_TRAP	91
#define TELE_ITEMS_TRAP	92
#define NASTINESS_TRAP	93

#define RECURSION_TRAP	94
#define RESPAWN_TRAP	95
#define WARP_ZONE	96
#define CAPTCHA_TRAP	97
#define MIND_WIPE_TRAP	98
#define THROWING_STAR_TRAP	99
#define SHIT_PIT	100
#define SHAFT_TRAP	101
#define LOCK_TRAP	102
#define MAGIC_CANCELLATION_TRAP	103
#define FARLOOK_TRAP	104
#define GATEWAY_FROM_HELL	105
#define GROWING_TRAP	106
#define COOLING_TRAP	107
#define BAR_TRAP	108
#define LOCKING_TRAP	109
#define AIR_TRAP	110
#define TERRAIN_TRAP	111

#define LOUDSPEAKER 112
#define NEST_TRAP 113
#define BULLET_TRAP 114
#define PARALYSIS_TRAP 115
#define CYANIDE_TRAP 116
#define LASER_TRAP 117
#define GIANT_CHASM 118
#define FART_TRAP 119
#define CONFUSE_TRAP 120
#define STUN_TRAP 121
#define HALLUCINATION_TRAP 122
#define PETRIFICATION_TRAP 123
#define NUMBNESS_TRAP 124
#define FREEZING_TRAP 125
#define BURNING_TRAP 126
#define FEAR_TRAP 127
#define BLINDNESS_TRAP 128
#define GLIB_TRAP 129
#define SLIME_TRAP 130
#define INERTIA_TRAP 131
#define TIME_TRAP 132
#define LYCANTHROPY_TRAP 133
#define UNLIGHT_TRAP 134
#define ELEMENTAL_TRAP 135
#define ESCALATING_TRAP 136
#define NEGATIVE_TRAP 137
#define MANA_TRAP 138
#define SIN_TRAP 139
#define DESTROY_ARMOR_TRAP 140
#define DIVINE_ANGER_TRAP 141
#define GENETIC_TRAP 142
#define MISSINGNO_TRAP 143
#define CANCELLATION_TRAP 144
#define HOSTILITY_TRAP 145
#define BOSS_TRAP 146
#define WISHING_TRAP 147
#define RECURRING_AMNESIA_TRAP 148
#define BIGSCRIPT_TRAP 149
#define BANK_TRAP 150
#define ONLY_TRAP 151
#define MAP_TRAP 152
#define TECH_TRAP 153
#define DISENCHANT_TRAP 154
#define VERISIERT 155
#define CHAOS_TRAP 156
#define MUTENESS_TRAP 157
#define NTLL_TRAP 158
#define ENGRAVING_TRAP 159
#define MAGIC_DEVICE_TRAP 160
#define BOOK_TRAP 161
#define LEVEL_TRAP 162
#define QUIZ_TRAP 163

#define FALLING_BOULDER_TRAP 164
#define GLASS_ARROW_TRAP 165
#define GLASS_BOLT_TRAP 166
#define OUT_OF_MAGIC_TRAP 167
#define PLASMA_TRAP 168
#define BOMB_TRAP 169
#define EARTHQUAKE_TRAP 170
#define NOISE_TRAP 171
#define GLUE_TRAP 172
#define GUILLOTINE_TRAP 173
#define BISECTION_TRAP 174
#define VOLT_TRAP 175
#define HORDE_TRAP 176
#define IMMOBILITY_TRAP 177
#define GREEN_GLYPH 178
#define BLUE_GLYPH 179
#define YELLOW_GLYPH 180
#define ORANGE_GLYPH 181
#define BLACK_GLYPH 182
#define PURPLE_GLYPH 183

#define TRAP_PERCENTS		184

#define TRAPNUM 185

#endif /* TRAP_H */
