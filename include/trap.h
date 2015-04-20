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

#define PESTILENCE_TRAP		81
#define FAMINE_TRAP		82

#define TRAP_PERCENTS		83

#define TRAPNUM 84

#endif /* TRAP_H */
