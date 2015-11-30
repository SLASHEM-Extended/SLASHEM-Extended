/*	SCCS Id: @(#)prop.h	3.4	1999/07/07	*/
/* Copyright (c) 1989 Mike Threepoint				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef PROP_H
#define PROP_H

/*** What the properties are ***/
#define FIRE_RES		 1
#define COLD_RES		 2
#define SLEEP_RES		 3
#define DISINT_RES		 4
#define SHOCK_RES		 5
#define POISON_RES		 6
#define ACID_RES		 7
#define STONE_RES		 8
/* note: for the first eight properties, MR_xxx == (1 << (xxx_RES - 1)) */
#define ADORNED			 9
#define REGENERATION		10
#define SEARCHING		11
#define SEE_INVIS		12
#define INVIS			13
#define TELEPORT		14
#define TELEPORT_CONTROL	15
#define POLYMORPH		16
#define POLYMORPH_CONTROL	17
#define LEVITATION		18
#define STEALTH			19
#define AGGRAVATE_MONSTER	20
#define CONFLICT		21
#define PROTECTION		22
#define PROT_FROM_SHAPE_CHANGERS 23
#define WARNING			24
#define TELEPAT			25
#define FAST			26
#define STUNNED			27
#define CONFUSION		28
#define SICK			29
#define BLINDED			30
#define SLEEPING		31
#define WOUNDED_LEGS		32
#define STONED			33
#define STRANGLED		34
#define HALLUC			35
#define HALLUC_RES		36
#define FUMBLING		37
#define JUMPING			38
#define WWALKING		39
#define HUNGER			40
#define GLIB			41
#define REFLECTING		42
#define LIFESAVED		43
#define ANTIMAGIC		44
#define DISPLACED		45
#define CLAIRVOYANT		46
#define VOMITING		47
#define ENERGY_REGENERATION	48
#define MAGICAL_BREATHING	49
#define HALF_SPDAM		50
#define HALF_PHDAM		51
#define SICK_RES		52
#define DRAIN_RES		53
#define WARN_UNDEAD		54
#define INVULNERABLE		55
#define FREE_ACTION		56
#define SWIMMING		57
#define SLIMED			58
#define FIXED_ABIL		59
#define FLYING			60
#define UNCHANGING		61
#define PASSES_WALLS		62
#define SLOW_DIGESTION		63
#define INFRAVISION		64
#define WARN_OF_MON		65
#define DETECT_MONSTERS		66
#define NUMBED			67
#define FEARED			68
#define FEAR_RES			69
#define FROZEN			70
#define BURNED			71
#define RMB_LOST			72
#define DISPLAY_LOST		73
#define SPELLS_LOST		74
#define YELLOW_SPELLS		75
#define AUTO_DESTRUCT		76
#define MEMORY_LOST		77
#define INVENTORY_LOST		78
#define BLACK_NG_WALLS		79
#define MENU_LOST			80
#define SPEED_BUG		81
#define SUPERSCROLLER_ACTIVE		82

#define DEAC_FIRE_RES	83
#define DEAC_COLD_RES	84
#define DEAC_SLEEP_RES	85
#define DEAC_DISINT_RES	86
#define DEAC_SHOCK_RES	87
#define DEAC_POISON_RES	88
#define DEAC_DRAIN_RES	89
#define DEAC_SICK_RES	90
#define DEAC_ANTIMAGIC	91
#define DEAC_ACID_RES	92
#define DEAC_STONE_RES	93
#define DEAC_FEAR_RES	94
#define DEAC_SEE_INVIS	95
#define DEAC_TELEPAT	96
#define DEAC_WARNING	97
#define DEAC_SEARCHING	98
#define DEAC_CLAIRVOYANT	99
#define DEAC_INFRAVISION	100
#define DEAC_DETECT_MONSTERS	101
#define DEAC_INVIS	102
#define DEAC_DISPLACED	103
#define DEAC_STEALTH	104
#define DEAC_JUMPING	105
#define DEAC_TELEPORT_CONTROL	106
#define DEAC_FLYING	107
#define DEAC_MAGICAL_BREATHING	108
#define DEAC_PASSES_WALLS	109
#define DEAC_SLOW_DIGESTION	110
#define DEAC_HALF_SPDAM	111
#define DEAC_HALF_PHDAM	112
#define DEAC_REGENERATION	113
#define DEAC_ENERGY_REGENERATION	114
#define DEAC_POLYMORPH_CONTROL	115
#define DEAC_FAST	116
#define DEAC_REFLECTING	117
#define DEAC_FREE_ACTION	118

#define HALLU_PARTY 119
#define DRUNKEN_BOXING 120
#define STUNNOPATHY 121
#define NUMBOPATHY 122
#define FREEZOPATHY 123
#define STONED_CHILLER 124
#define CORROSIVITY 125
#define FEAR_FACTOR 126
#define BURNOPATHY 127
#define SICKOPATHY 128

#define DEAC_HALLU_PARTY 129
#define DEAC_DRUNKEN_BOXING 130
#define DEAC_STUNNOPATHY 131
#define DEAC_NUMBOPATHY 132
#define DEAC_FREEZOPATHY 133
#define DEAC_STONED_CHILLER 134
#define DEAC_CORROSIVITY 135
#define DEAC_FEAR_FACTOR 136
#define DEAC_BURNOPATHY 137
#define DEAC_SICKOPATHY 138

#define HEAVY_STUNNED 139
#define HEAVY_CONFUSION 140
#define HEAVY_NUMBED 141
#define HEAVY_FEARED 142
#define HEAVY_FROZEN 143
#define HEAVY_BURNED 144
#define HEAVY_BLIND 145
#define HEAVY_HALLU 146

#define FREE_HAND_LOST 147
#define UNIDENTIFY 148
#define THIRST 149
#define LUCK_LOSS 150
#define SHADES_OF_GREY 151
#define INCREASED_GRAVITY 152
#define FAINT_ACTIVE 153
#define ITEMCURSING 154
#define DIFFICULTY_INCREASED 155
#define DEAFNESS 156
#define CASTER_PROBLEM 157
#define WEAKNESS_PROBLEM 158
#define ROT_THIRTEEN 159
#define BISHOP_GRIDBUG 160
#define CONFUSION_PROBLEM 161
#define DROP_BUG 162
#define DSTW_BUG 163
#define STATUS_FAILURE 164
#define ALIGNMENT_FAILURE 165
#define STAIRSTRAP 166

#define STORM_HELM 167

#define NO_STAIRCASE 168

#define TIME_STOPPED 169

#define UNINFORMATION 170

#define INTRINSIC_LOSS 171
#define TRAP_REVEALING 172
#define BLOOD_LOSS 173
#define NASTINESS_EFFECTS 174
#define BAD_EFFECTS 175
#define AUTOMATIC_TRAP_CREATION 176
#define SENTIENT_HIGH_HEELS 177
#define REPEATING_VULNERABILITY 178
#define TELEPORTING_ITEMS 179
#define RANDOM_RUMORS 180

#define CAPTCHA 181
#define FARLOOK_BUG 182
#define RESPAWN_BUG 183
#define MC_REDUCTION 184

#define PREMDEATH		185

#define KEEN_MEMORY	186
#define DEAC_KEEN_MEMORY	187

#define THE_FORCE	188
#define DEAC_THE_FORCE	189

#define SENSORY_DEPRIVATION 190
#define RECURRING_AMNESIA 191

#define BIGSCRIPT 192
#define BANKBUG 193
#define MAPBUG 194
#define TECHBUG 195
#define RECURRING_DISENCHANT 196
#define VERISIERTEFFECT 197
#define CHAOS_TERRAIN 198
#define MUTENESS 199
#define ENGRAVINGBUG 200
#define MAGIC_DEVICE_BUG 201
#define BOOKBUG 202
#define LEVELBUG 203
#define QUIZZES 204

#define FAST_METABOLISM 205
#define NORETURN 206
#define ALWAYS_EGOTYPES 207
#define FAST_FORWARD 208
#define FOOD_IS_ROTTEN 209
#define SKILL_DEACTIVATED 210
#define STATS_LOWERED 211
#define TRAINING_DEACTIVATED 212
#define EXERCISE_DEACTIVATED 213

#define SECOND_CHANCE		214
#define LAST_PROP		(SECOND_CHANCE)

/*** Where the properties come from ***/
/* Definitions were moved here from obj.h and you.h */
struct prop {
	/*** Properties conveyed by objects ***/
	long extrinsic;
	/* Armor */
#	define W_ARM	    0x00000001L /* Body armor */
#	define W_ARMC	    0x00000002L /* Cloak */
#	define W_ARMH	    0x00000004L /* Helmet/hat */
#	define W_ARMS	    0x00000008L /* Shield */
#	define W_ARMG	    0x00000010L /* Gloves/gauntlets */
#	define W_ARMF	    0x00000020L /* Footwear */
#ifdef TOURIST
#	define W_ARMU	    0x00000040L /* Undershirt */
#	define W_ARMOR	     (W_ARM | W_ARMC | W_ARMH | W_ARMS | W_ARMG | W_ARMF | W_ARMU)
#else
#	define W_ARMOR	     (W_ARM | W_ARMC | W_ARMH | W_ARMS | W_ARMG | W_ARMF)
#endif
	/* Weapons and artifacts */
#	define W_WEP	    0x00000100L /* Wielded weapon */
#	define W_QUIVER     0x00000200L /* Quiver for (f)iring ammo */
#	define W_SWAPWEP    0x00000400L /* Secondary weapon */
#	define W_ART	    0x00001000L /* Carrying artifact (not really worn) */
#	define W_ARTI	    0x00002000L /* Invoked artifact  (not really worn) */
	/* Amulets, rings, tools, and other items */
#	define W_AMUL	    0x00010000L /* Amulet */
#	define W_RINGL	    0x00020000L /* Left ring */
#	define W_RINGR	    0x00040000L /* Right ring */
#	define W_RING	    (W_RINGL | W_RINGR)
#	define W_TOOL	    0x00080000L /* Eyewear */
#ifdef STEED
#	define W_SADDLE     0x00100000L	/* KMH -- For riding monsters */
#endif
#	define W_BALL	    0x00200000L /* Punishment ball */
#	define W_CHAIN	    0x00400000L /* Punishment chain */

	/*** Property is blocked by an object ***/
	long blocked;					/* Same assignments as extrinsic */

	/*** Timeouts, permanent properties, and other flags ***/
	long intrinsic;
	/* Timed properties */
#	define TIMEOUT	    0x00ffffffL /* Up to 16 million turns */
	/* Permanent properties */
#	define FROMEXPER    0x01000000L /* Gain/lose with experience, for role */
#	define FROMRACE     0x02000000L /* Gain/lose with experience, for race */
#	define FROMOUTSIDE  0x04000000L /* By corpses, prayer, thrones, etc. */
#	define INTRINSIC    (FROMOUTSIDE|FROMRACE|FROMEXPER)
	/* Control flags */
#	define I_SPECIAL    0x10000000L /* Property is controllable */

};

/*** Definitions for backwards compatibility ***/
#define LEFT_RING	W_RINGL
#define RIGHT_RING	W_RINGR
#define LEFT_SIDE	LEFT_RING
#define RIGHT_SIDE	RIGHT_RING
#define BOTH_SIDES	(LEFT_SIDE | RIGHT_SIDE)
#define WORN_ARMOR	W_ARM
#define WORN_CLOAK	W_ARMC
#define WORN_HELMET	W_ARMH
#define WORN_SHIELD	W_ARMS
#define WORN_GLOVES	W_ARMG
#define WORN_BOOTS	W_ARMF
#define WORN_AMUL	W_AMUL
#define WORN_BLINDF	W_TOOL
#ifdef TOURIST
#define WORN_SHIRT	W_ARMU
#endif

#endif /* PROP_H */
