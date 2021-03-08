/*	SCCS Id: @(#)objclass.h 3.4	1996/06/16	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef OBJCLASS_H
#define OBJCLASS_H

#include "permonst.h"
#include "artifact.h"

/* definition of a class of objects */

struct objclass {
	short	oc_name_idx;		/* index of actual name */
	short	oc_descr_idx;		/* description when name unknown */
	char *	oc_uname;		/* called by user */
	Bitfield(oc_name_known,1);
	Bitfield(oc_merge,1);	/* merge otherwise equal objects */
	Bitfield(oc_uses_known,1); /* obj->known affects full decription */
				/* otherwise, obj->dknown and obj->bknown */
				/* tell all, and obj->known should always */
				/* be set for proper merging behavior */
	Bitfield(oc_pre_discovered,1);	/* Already known at start of game; */
					/* won't be listed as a discovery. */
	Bitfield(oc_magic,1);	/* inherently magical object */
	Bitfield(oc_charged,1); /* may have +n or (n) charges */
	Bitfield(oc_unique,1);	/* special one-of-a-kind object */
	Bitfield(oc_nowish,1);	/* cannot wish for this object */

	Bitfield(oc_big,1);
#define oc_bimanual	oc_big	/* for weapons & tools used as weapons */
#define oc_bulky	oc_big	/* for armor */
	Bitfield(oc_tough,1);	/* hard gems/rings */

	Bitfield(oc_dir,3);
#define NODIR		1	/* for wands/spells: non-directional */
#define IMMEDIATE	2	/*		     directional */
#define RAY		3	/*		     zap beams */

#define PIERCE		1	/* for weapons & tools used as weapons */
#define SLASH		2	/* (latter includes iron ball & chain) */
#define EXPLOSION	4	/* (rockets,  grenades) */
#define WHACK		0

	int oc_material; /* wow. Managed to run out of a bitfield again! --Amy */
/* edit by Amy: for easier grepping on windows and reducing the chance of naming clashes, we do the sensible thing
 * that the vanilla developers unfortunately never did: prefixing all the materials with MT_ */
#define MT_MYSTERIOUS	0	/* for objects.c */
#define MT_LIQUID		1	/* currently only for venom */
#define MT_WAX		2
#define MT_VEGGY		3	/* foodstuffs */
#define MT_FLESH		4	/*   ditto    */
#define MT_PAPER		5
#define MT_CLOTH		6
#define MT_LEATHER		7
#define MT_WOOD		8
#define MT_BONE		9
#define MT_DRAGON_HIDE	10	/* not leather! */
#define MT_IRON		11	/* Fe - includes steel */
#define MT_METAL		12	/* Sn, &c. */
#define MT_COPPER		13	/* Cu - includes brass */
#define MT_SILVER		14	/* Ag */
#define MT_GOLD		15	/* Au */
#define MT_PLATINUM	16	/* Pt */
#define MT_MITHRIL		17
#define MT_PLASTIC		18
#define MT_GLASS		19
#define MT_GEMSTONE	20
#define MT_MINERAL		21
/* new materials by Amy - some of them are obviously fictional */
#define MT_VIVA		22	/* vivardoradorium: a highly unstable, radioactive chemical element */
#define MT_INKA		23	/* extraordinarily durable form of leather */
#define MT_TAR		24	/* actually bitumen */
#define MT_SILK		25
#define MT_ARCANIUM		26	/* completely nondescript material */
#define MT_SECREE		27	/* "secretion" would sound too icky, I think */
#define MT_POURPOOR		28	/* the "poor man's" version of iron */
#define MT_COMPOST		29	/* organic material that's not flammable (too wet or humid to catch fire) */
#define MT_ETERNIUM		30	/* inedible material that resists destruction */
#define MT_ETHER		31	/* like Elona, except that we have generic contamination instead of ether disease */
#define MT_BRICK		32	/* lithic material that resists withering */
#define MT_SAND		33	/* lithic */
#define MT_SHADOWSTUFF		34	/* inedible, from dnethack */
#define MT_OBSIDIAN		35	/* lithic, practically equivalent to glass, from dnethack */
#define MT_LEAD		36	/* metallic, protects against contamination and extra bad effects */
#define MT_CHROME		37	/* metallic, protects against poison */
#define MT_CERAMIC		38	/* lithic, from dnethack */
#define MT_CELESTIUM		39	/* organic, improves spellcasting, easier to uncurse */
#define MT_CONUNDRUM		40	/* lithic, can protect destructible consumables */
#define MT_PWN_BUBBLE		41	/* inedible, acts as if it was finalized */
#define MT_METEOSTEEL		42	/* metallic, better dmg/AC, doesn't hinder spellcasting */
#define MT_ANTIDOTIUM		43	/* inedible, helps against corona */
#define MT_NANOMACHINE		44	/* inedible, repairs itself */
#define MT_FOAM		45	/* inedible */
#define MT_SCRAP		46	/* metallic */
#define MT_ALLOY		47	/* metallic */
#define LASTMATERIAL	MT_ALLOY
/* grep for "materialeffect" to see all the places that need changing if we add new ones --Amy */

#define is_organic(otmp)	(objects[(otmp)->otyp].oc_material <= MT_WOOD || objects[(otmp)->otyp].oc_material == MT_DRAGON_HIDE || objects[(otmp)->otyp].oc_material == MT_CELESTIUM || objects[(otmp)->otyp].oc_material == MT_INKA || objects[(otmp)->otyp].oc_material == MT_SILK || objects[(otmp)->otyp].oc_material == MT_SECREE || objects[(otmp)->otyp].oc_material == MT_COMPOST)
#define is_metallic(otmp)	((objects[(otmp)->otyp].oc_material >= MT_IRON && \
				 objects[(otmp)->otyp].oc_material <= MT_MITHRIL) || objects[(otmp)->otyp].oc_material == MT_VIVA || objects[(otmp)->otyp].oc_material == MT_ETHER || objects[(otmp)->otyp].oc_material == MT_METEOSTEEL || objects[(otmp)->otyp].oc_material == MT_POURPOOR || objects[(otmp)->otyp].oc_material == MT_LEAD || objects[(otmp)->otyp].oc_material == MT_CHROME)
#define is_lithic(otmp)		(objects[(otmp)->otyp].oc_material == MT_BONE || objects[(otmp)->otyp].oc_material == MT_GLASS || objects[(otmp)->otyp].oc_material == MT_GEMSTONE || objects[(otmp)->otyp].oc_material == MT_MINERAL || objects[(otmp)->otyp].oc_material == MT_SAND || objects[(otmp)->otyp].oc_material == MT_OBSIDIAN || objects[(otmp)->otyp].oc_material == MT_CONUNDRUM || objects[(otmp)->otyp].oc_material == MT_CERAMIC || objects[(otmp)->otyp].oc_material == MT_TAR || objects[(otmp)->otyp].oc_material == MT_BRICK)

/* primary damage: fire/rust/--- */
/* secondary damage: rot/acid/acid */
/* all of them in mkobj.c now --Amy */

#define is_unwitherable(otmp)	(objects[otmp->otyp].oc_material == MT_BRICK)
#define hard_to_destruct(otmp)	(objects[otmp->otyp].oc_material == MT_ETERNIUM || otmp->stckcurse || (uimplant && uimplant->oartifact == ART_HENRIETTA_S_TENACIOUSNESS) || (otmp->oartifact == ART_SOFTSTEP) || (otmp->oartifact && otmp->cursed && (spec_ability(otmp, SPFX_EVIL)) ) )
#define is_etheritem(otmp)	(objects[otmp->otyp].oc_material == MT_ETHER)
#define is_meteosteelitem(otmp)	(objects[otmp->otyp].oc_material == MT_METEOSTEEL)

#define is_damageable(otmp) (is_rustprone(otmp) || is_flammable(otmp) || \
				is_rottable(otmp) || is_corrodeable(otmp))

	schar	oc_subtyp;
/*	Bitfield(oc_subtyp,3);*/	/* Now too big for a bitfield */
#define oc_skill	oc_subtyp   /* for weapons, spellbooks, tools, gems */
#define oc_armcat	oc_subtyp   /* for armor */
#define ARM_SHIELD	1	/* needed for special wear function */
#define ARM_HELM	2
#define ARM_GLOVES	3
#define ARM_BOOTS	4
#define ARM_CLOAK	5
#define ARM_SHIRT	6
#define ARM_SUIT	0

	/*uchar*/int	oc_oprop;		/* property (invis, &c.) conveyed */
	char	oc_class;		/* object class */
	int	oc_delay;		/* delay when using such an object */
	uchar	oc_color;		/* color of the object */

	int oc_minlvl;
	int oc_appearindex;

	/*short*/int	oc_prob;		/* probability, used in mkobj() */
	unsigned short	oc_weight;	/* encumbrance (1 cn = 0.1 lb.) */
	short	oc_cost;		/* base cost in shops */
/* Check the AD&D rules!  The FIRST is small monster damage. */
/* for weapons, and tools, rocks, and gems useful as weapons */
	schar	oc_wsdam, oc_wldam;	/* max small/large monster damage */
#define oc_range	oc_wsdam	/* for strength independant ranged weapons */
#define oc_rof		oc_wldam	/* rate of fire bonus for ranged weapons */
	
	schar	oc_oc1, oc_oc2;
#define oc_hitbon	oc_oc1		/* weapons: "to hit" bonus */
#define w_ammotyp	oc_oc2		/* type of ammo taken by ranged weapon */
#define WP_GENERIC	0		/* all ammo subclasses ok */
#define WP_BULLET	1
#define WP_SHELL	2
#define WP_ROCKET	3
#define WP_GRENADE	4
#define WP_BLASTER	5
#define WP_BFG	6

#define a_ac		oc_oc1	/* armor class, used in ARM_BONUS in do.c */
#define a_can		oc_oc2		/* armor: used in mhitu.c */
#define oc_level	oc_oc2		/* books: spell level */

	int	oc_nutrition;	/* food value */
};

struct objdescr {
	const char *oc_name;		/* actual name */
	const char *oc_descr;		/* description when name unknown */
};

extern NEARDATA struct objclass objects[];
extern NEARDATA struct objdescr obj_descr[];

/*
 * All objects have a class. Make sure that all classes have a corresponding
 * symbol below.
 */
#define RANDOM_CLASS	 0	/* used for generating random objects */
#define ILLOBJ_CLASS	 1
#define WEAPON_CLASS	 2
#define ARMOR_CLASS	 3
#define RING_CLASS	 4
#define AMULET_CLASS	 5
#define IMPLANT_CLASS	 6
#define TOOL_CLASS	 7
#define FOOD_CLASS	 8
#define POTION_CLASS	 9
#define SCROLL_CLASS	10
#define SPBOOK_CLASS	11	/* actually SPELL-book */
#define WAND_CLASS	12
#define COIN_CLASS	13
#define GEM_CLASS	14
#define ROCK_CLASS	15
#define BALL_CLASS	16
#define CHAIN_CLASS	17
#define VENOM_CLASS	18
#define MAXOCLASSES	19

#define WILDCARD_CLASS	69	/* RANDOM_CLASS but without timebasedlowerchance effect */

#define ALLOW_COUNT	(MAXOCLASSES+1) /* Can be used in the object class */
#define ALL_CLASSES	(MAXOCLASSES+2) /* input to getobj().		   */
#define ALLOW_NONE	(MAXOCLASSES+3) /*				   */
#define ALLOW_FLOOROBJ	(MAXOCLASSES+4) /*				   */
#define ALLOW_THISPLACE	(MAXOCLASSES+5) /*				   */

#define BURNING_OIL	(MAXOCLASSES+1) /* Can be used as input to explode. */
#define MON_EXPLODE	(MAXOCLASSES+2) /* Exploding monster (e.g. gas spore) */

#if 0	/* moved to decl.h so that makedefs.c won't see them */
extern const char def_oc_syms[MAXOCLASSES];	/* default class symbols */
extern uchar oc_syms[MAXOCLASSES];		/* current class symbols */
#endif

/* Default definitions of all object-symbols (must match classes above). */

#define ILLOBJ_SYM	']'	/* also used for mimics */
#define WEAPON_SYM	')'
#define ARMOR_SYM	'['
#define RING_SYM	'='
#define AMULET_SYM	'"'
#define IMPLANT_SYM	']'
#define TOOL_SYM	'('
#define FOOD_SYM	'%'
#define POTION_SYM	'!'
#define SCROLL_SYM	'?'
#define SPBOOK_SYM	'+'
#define WAND_SYM	'/'
#define GOLD_SYM	'$'
#define GEM_SYM		'*'
#define ROCK_SYM	'`'
#define BALL_SYM	'0'
#define CHAIN_SYM	'_'
#define VENOM_SYM	'.'

struct fruit {
	char fname[PL_FSIZ];
	int fid;
	struct fruit *nextf;
};
#define newfruit() (struct fruit *)alloc(sizeof(struct fruit))
#define dealloc_fruit(rind) free((void *) (rind))

#define OBJ_NAME(obj)  (obj_descr[(obj).oc_name_idx].oc_name)
#define OBJ_DESCR(obj) (obj_descr[(obj).oc_descr_idx].oc_descr)
#endif /* OBJCLASS_H */
