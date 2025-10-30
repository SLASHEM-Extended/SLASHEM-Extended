/*	SCCS Id: @(#)obj.h	3.4	2002/01/07	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef OBJ_H
#define OBJ_H

/* #define obj obj_nh */ /* uncomment for SCO UNIX, which has a conflicting
			  * typedef for "obj" in <sys/types.h> */

union vptrs {
	    struct obj *v_nexthere;	/* floor location lists */
	    struct obj *v_ocontainer;	/* point back to container */
	    struct monst *v_ocarry;	/* point back to carrying monst */
};

struct obj {
	struct obj *nobj;
	union vptrs v;
#define nexthere	v.v_nexthere
#define ocontainer	v.v_ocontainer
#define ocarry		v.v_ocarry

	struct obj *cobj;	/* contents list for containers */
	unsigned o_id;
	xchar ox,oy;
	/*short*/long otyp;		/* object class number */
	/*short*/long oldtyp;	/* WAC for unpolymorph */
	unsigned owt;
	long quan;		/* number of items */

	int spe;		/* quality of weapon, armor or ring (+ or -)
				   number of charges for wand ( >= -1 )
				   marks your eggs, spinach tins
				   royal coffers for a court ( == 2)
				   tells which fruit a fruit is
				   special for uball and amulet
				   historic and gender for statues */
#define STATUE_HISTORIC 0x01
#define STATUE_MALE     0x02
#define STATUE_FEMALE   0x04
	char	oclass;		/* object class */
	char	invlet;		/* designation in inventory */
	/*char*/int	oartifact;	/* artifact array index */
	boolean fakeartifact;	/* so the player cannot rename them */
	schar 	altmode; 	/* alternate modes - eg. SMG, double Lightsaber */
				/* WP_MODEs are in decreasing speed */
#define WP_MODE_AUTO	0	/* Max firing speed */
#define WP_MODE_BURST	1	/* 1/3 of max rate */
#define WP_MODE_SINGLE 	2	/* Single shot */

	xchar where;		/* where the object thinks it is */
#define OBJ_FREE	0		/* object not attached to anything */
#define OBJ_FLOOR	1		/* object on floor */
#define OBJ_CONTAINED	2		/* object in a container */
#define OBJ_INVENT	3		/* object in the hero's inventory */
#define OBJ_MINVENT	4		/* object in a monster inventory */
#define OBJ_MIGRATING	5		/* object sent off to another level */
#define OBJ_BURIED	6		/* object buried */
#define OBJ_ONBILL	7		/* object on shk bill */
#define NOBJ_STATES	8
	xchar timed;		/* # of fuses (timers) attached to this obj */

	Bitfield(cursed,1);
	Bitfield(blessed,1);
	Bitfield(hvycurse,1);	/* harder to uncurse than "regular" cursed items --Amy */
	Bitfield(prmcurse,1);
	Bitfield(morgcurse,1);	/* Ancient Morgothian curse */
	Bitfield(evilcurse,1);	/* Topi Ylinen curse */
	Bitfield(bbrcurse,1);	/* Black Breath curse */
	Bitfield(stckcurse,1);	/* sticky curse - resists certain methods of getting the item off --Amy */
	Bitfield(unpaid,1);	/* on some bill */
	Bitfield(no_charge,1);	/* if shk shouldn't charge for this */
	Bitfield(known,1);	/* exact nature known */
	Bitfield(dknown,1);	/* color or text known */
	Bitfield(bknown,1);	/* blessing or curse known */
	Bitfield(rknown,1);	/* rustproof or not known */
	Bitfield(wornknown,1);	/* item has been worn at least once --Amy */
	Bitfield(petmarked,1);	/* pets will not drop this --Amy */
	Bitfield(nemtrident,1);	/* nemesis race PC has wielded this --Amy */
	Bitfield(objwassold,1);	/* to thwart endless credit cloning */
	Bitfield(ident_bst,1);	/* this and the next four flags determine whether identify spells reveal certain stuff */
	Bitfield(ident_knw,1);
	Bitfield(ident_dkn,1);
	Bitfield(ident_bkn,1);
	Bitfield(ident_rkn,1);
	Bitfield(icedobject,1);	/* has the corpse been in an ice box? */
	Bitfield(stackmarked,1);	/* has the object been marked to not merge with other objects? */
	Bitfield(judithwin,1);	/* for judith trap: did you defeat this pair of shoes? */

	Bitfield(oeroded,2);	/* rusted/burnt weapon/armor */
	Bitfield(oeroded2,2);	/* corroded/rotted weapon/armor */
	Bitfield(obrittle,2);	/* for antibar weapons */
	Bitfield(obrittle2,2);	/* ditto */
	Bitfield(dynamitekaboom,1);	/* hack to make sure that you can't move certain sticks of dynamite --Amy */
#define greatest_erosion(otmp) (int)((otmp)->oeroded > (otmp)->oeroded2 ? (otmp)->oeroded : (otmp)->oeroded2)

#define greatest_erosionX(otmp) (int) (objects[(otmp)->otyp].oc_material == MT_COMPOST ? (otmp)->oeroded : ((otmp)->oeroded > (otmp)->oeroded2 ? (otmp)->oeroded : (otmp)->oeroded2))

#define MAX_ERODE 3
#define orotten oeroded		/* rotten food */
#define odiluted oeroded	/* diluted potions */
#define norevive oeroded2
	Bitfield(oerodeproof,1); /* erodeproof weapon/armor */
	Bitfield(olocked,1);	/* object is locked */
#define oarmed olocked
#define odrained olocked	/* drained corpse */
	Bitfield(obroken,1);	/* lock has been broken */
	Bitfield(otrapped,1);	/* container is trapped */
	Bitfield(superpoison,1);	/* for poisoned weapons: stronger poison (less likely to wear off) */
				/* or accidental tripped rolling boulder trap */
#define opoisoned otrapped	/* object (weapon) is coated with poison */

	Bitfield(recharged,3);	/* number of times it's been recharged */
	Bitfield(lamplit,1);	/* a light-source -- can be lit */
	Bitfield(oinvis,1);	/* invisible */
	Bitfield(oinvisreal,1);	/* really invisible - won't be visible even with see invis --Amy */
	Bitfield(greased,2);	/* covered with grease */
	Bitfield(oattached,2);	/* obj struct has special attachment */
#define OATTACHED_NOTHING 0
#define OATTACHED_MONST   1	/* monst struct in oextra */
#define OATTACHED_M_ID    2	/* monst id in oextra */
#define OATTACHED_UNUSED3 3
	Bitfield(in_use,1);	/* for magic items before useup items */
	Bitfield(bypass,1);	/* mark this as an object to be skipped by bhito() */

	Bitfield(yours,1);	/* obj is yours (eg. thrown by you) */
	Bitfield(was_thrown,1); /* thrown by the hero since last picked up */
	Bitfield(mstartinvent,1); /* musable item is part of a monster's starting inventory --Amy */
	Bitfield(mstartinventB,1); /* equipment is part of a monster's starting inventory --Amy */
	Bitfield(mstartinventC,1); /* very likely (90%) to be erased on monster's death --Amy */
	Bitfield(mstartinventD,1); /* likely (75%) to be erased on monster's death --Amy */
	Bitfield(mstartinventE,1); /* extremely likely (95%) to be erased on monster's death --Amy */
	Bitfield(mstartinventX,1); /* guaranteed (100%) to be erased on monster's death, even if it's an artifact --Amy */
	/* ? free bits */

	int	corpsenm;	/* type of corpse is mons[corpsenm] */
#define leashmon  corpsenm	/* gets m_id of attached pet */
#define spestudied corpsenm	/* # of times a spellbook has been studied */
#define fromsink  corpsenm	/* a potion from a sink */

#ifdef RECORD_ACHIEVE
#define record_achieve_special corpsenm
#endif
	int enchantment;	/* in case of armors, special property --Amy */

	unsigned oeaten;	/* nutrition left in food, if partly eaten */
	long age;		/* creation date */
	long invoketimer;	/* stupid programmers, why didn't they make an extra flag for that... *headdesk* --Amy */

	int shirtmessage;	/* o_id is not random enough for me. --Amy */

	boolean finalcancel;	/* can't be cancelled, polymorphed etc. --Amy */

	uchar onamelth;		/* length of name (following oxlth) */
	Bitfield(selfmade,1);	/* made with chemistry set or magic marker */
	/*short*/long oxlth;		/* length of following data */
	/* in order to prevent alignment problems oextra should
	   be (or follow) a long int */
	long owornmask;
	long oextra[1];		/* used for name of ordinary objects - length
				   is flexible; amount for tmp gold objects */
};

#define newobj(xl)	(struct obj *)alloc((unsigned)(xl) + sizeof(struct obj))
#define ONAME(otmp)	(((char *)(otmp)->oextra) + (otmp)->oxlth)

#define finalcancelled(otmp)	(otmp->finalcancel || (objects[(otmp)->otyp].oc_material == MT_PWN_BUBBLE))

/* All objects */
#define is_hazy(otmp)	((otmp)->oldtyp != STRANGE_OBJECT)
/* [ALI] None of the objects listed here can be picked up by normal monsters.
 * If any such objects need to be marked as indestructible then consideration
 * will need to be given to what happens when such a monster disappears
 * carrying the object.
 */
#define is_macguffin(otmp) ( \
			(otmp)->otyp == AMULET_OF_YENDOR || \
			(otmp)->otyp == CANDELABRUM_OF_INVOCATION || \
			(otmp)->otyp == BELL_OF_OPENING || \
			(otmp)->otyp == SPE_BOOK_OF_THE_DEAD || \
			(otmp)->oartifact == ART_GAUNTLET_KEY || \
			(otmp)->oartifact == ART_KEY_OF_LAW || \
			(otmp)->oartifact == ART_KEY_OF_NEUTRALITY || \
			(otmp)->oartifact == ART_KEY_OF_CHAOS)
/* evades_destruction() moved to invent.c by Amy because we want them to become destructible once they're no longer needed
 * this will allow the player to unclutter their inventory once the invocation has been performed */

#define always_visible(otmp) ( \
			(otmp)->otyp == MUMMY_WRAPPING || \
			(otmp)->oclass == COIN_CLASS)

/* Weapons and weapon-tools */
/* KMH -- now based on skill categories.  Formerly:
 *	#define is_sword(otmp)	(otmp->oclass == WEAPON_CLASS && \
 *			 objects[otmp->otyp].oc_wepcat == WEP_SWORD)
 *	#define is_blade(otmp)	(otmp->oclass == WEAPON_CLASS && \
 *			 (objects[otmp->otyp].oc_wepcat == WEP_BLADE || \
 *			  objects[otmp->otyp].oc_wepcat == WEP_SWORD))
 *	#define is_weptool(o)	((o)->oclass == TOOL_CLASS && \
 *			 objects[(o)->otyp].oc_weptool)
 *	#define is_multigen(otyp) (otyp <= SHURIKEN)
 *	#define is_poisonable(otyp) (otyp <= BEC_DE_CORBIN)
 */

#define is_sword(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill >= P_SHORT_SWORD && \
			 objects[(otmp)->otyp].oc_skill <= P_SABER)
#define is_blade(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill >= P_DAGGER && \
			 objects[(otmp)->otyp].oc_skill <= P_SABER)
#define can_weld(otmp)	((otmp)->oclass == WEAPON_CLASS \
				|| is_weptool(otmp) \
				|| (otmp)->oclass == BALL_CLASS \
				|| (otmp)->oclass == GEM_CLASS \
				|| (otmp)->oclass == VENOM_CLASS \
				|| (otmp)->oclass == CHAIN_CLASS)
#define is_pole(otmp)	(((otmp)->oclass == WEAPON_CLASS || \
			(otmp)->oclass == TOOL_CLASS) && \
			 ((otmp)->otyp == CIGAR || objects[(otmp)->otyp].oc_skill == P_POLEARMS || \
			 objects[(otmp)->otyp].oc_skill == (tech_inuse(T_GRAP_SWAP) ? P_GRINDER : P_LANCE) ))
#define is_applypole(otmp)	(((otmp)->oclass == WEAPON_CLASS || (otmp)->oclass == CHAIN_CLASS || (otmp)->oclass == BALL_CLASS || \
			(otmp)->oclass == TOOL_CLASS) && \
			 (objects[(otmp)->otyp].oc_skill == P_POLEARMS || \
			 objects[(otmp)->otyp].oc_skill == P_GRINDER || \
			 (otmp)->otyp == CIGAR || \
			 (otmp)->otyp == AKLYS || \
			 (otmp)->otyp == LASER_CHAIN || \
			 (otmp)->otyp == BLOW_AKLYS || \
			 (otmp)->otyp == REACH_TRIDENT || \
			 (otmp)->otyp == SPINED_BALL || \
			 (otmp)->otyp == CHAIN_AND_SICKLE || \
			 (otmp)->otyp == LAJATANG || \
			 (otmp)->oartifact == ART_WILD_OAR || \
			 (otmp)->oartifact == ART_SIMON_S_ULTRALONGNESS || \
			 (otmp)->oartifact == ART_WOETSCHERSMAG || \
			 (otmp)->oartifact == ART_VORGATH_S_GRAVITON || \
			 (otmp)->oartifact == ART_STRECKOUT_AND_PULL || \
			 (otmp)->oartifact == ART_HEAVY_POLE_SKILL || \
			 objects[(otmp)->otyp].oc_skill == P_LANCE))
#define is_spear(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill >= P_SPEAR && \
			 objects[(otmp)->otyp].oc_skill <= P_JAVELIN)
#define is_axe(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill == P_AXE)
#define is_launcher(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill >= P_BOW && \
			 objects[(otmp)->otyp].oc_skill <= P_CROSSBOW)
#define is_ammo(otmp)	(((otmp)->oclass == WEAPON_CLASS || \
			 (otmp)->oclass == GEM_CLASS) && \
			 objects[(otmp)->otyp].oc_skill >= -P_CROSSBOW && \
			 objects[(otmp)->otyp].oc_skill <= -P_BOW)
#define is_missile(otmp)	(((otmp)->oclass == WEAPON_CLASS || \
			 (otmp)->oclass == TOOL_CLASS) && \
			 objects[(otmp)->otyp].oc_skill >= -P_BOOMERANG && \
			 objects[(otmp)->otyp].oc_skill <= -P_DART)
#define is_grenade(otmp)	(is_ammo(otmp) && \
			 	 objects[(otmp)->otyp].w_ammotyp == WP_GRENADE)
#define is_multigen(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill >= -P_SHURIKEN && \
			 objects[(otmp)->otyp].oc_skill <= -P_BOW)
#define is_lower_multigen(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 (objects[(otmp)->otyp].oc_skill == -P_BOOMERANG || objects[(otmp)->otyp].oc_skill == P_BOOMERANG || objects[(otmp)->otyp].oc_skill == P_SPEAR || objects[(otmp)->otyp].oc_skill == P_DAGGER || objects[(otmp)->otyp].oc_skill == P_KNIFE || objects[(otmp)->otyp].oc_skill == P_JAVELIN ) )
#define is_unpoisonable_firearm_ammo(otmp)	\
			 (is_bullet(otmp) || (otmp)->otyp == STICK_OF_DYNAMITE)
#define is_poisonable(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 (objects[(otmp)->otyp].oc_skill <= P_SABER || \
			 (objects[(otmp)->otyp].oc_skill >= P_CLAW && \
			 objects[(otmp)->otyp].oc_skill <= P_LANCE)) && \
			 !is_unpoisonable_firearm_ammo(otmp))
#define uslinging()	(uwep && objects[uwep->otyp].oc_skill == P_SLING)
#define is_weptool(o)	((o)->oclass == TOOL_CLASS && \
			 (objects[(o)->otyp].oc_skill != P_NONE || ((o)->otyp == TIN_OPENER) || ((o)->otyp == BUDO_NO_SASU) || ((o)->otyp == JEONTU_GEOM) ) )

#define is_weptoolbase(o)	(objects[o].oc_class == TOOL_CLASS && \
			 (objects[o].oc_skill != P_NONE || (o == TIN_OPENER) || (o == BUDO_NO_SASU) || (o == JEONTU_GEOM) ) )

#define is_pick(otmp)	(((otmp)->oclass == WEAPON_CLASS || \
			 (otmp)->oclass == TOOL_CLASS) && \
			 (objects[(otmp)->otyp].oc_skill == P_PICK_AXE || (otmp)->otyp == SHOVEL) )
#define is_antibar(otmp)	((otmp)->otyp == ELECTRIC_SWORD || (otmp)->otyp == GOLDEN_SABER || (otmp)->otyp == BLOCK_HEELED_SANDAL || (otmp)->otyp == PLATINUM_SABER  || (otmp)->otyp == METAL_CLUB  || (otmp)->otyp == RADIOACTIVE_DAGGER  || (otmp)->otyp == ETERNIUM_BLADE  || (otmp)->otyp == DEVIL_STAR  || (otmp)->otyp == SEXY_MARY_JANE  || (otmp)->otyp == OBSID  || (otmp)->otyp == IMPACT_STAFF  || (otmp)->otyp == SOFT_LADY_SHOE  || (otmp)->otyp == INKA_BOOT  || (otmp)->otyp == BRIDGE_MUZZLE  || (otmp)->otyp == MALLET  || (otmp)->otyp == BREAKER_STAFF  || (otmp)->otyp == BREAKING_WHEEL  || (otmp)->otyp == YATAGAN  || (otmp)->otyp == GROM_AXE  || (otmp)->otyp == HONOR_KATANA  || (otmp)->otyp == WEDGED_LITTLE_GIRL_SANDAL  || (otmp)->otyp == HUGGING_BOOT || (otmp)->otyp == DIAMOND_SMASHER  || (otmp)->otyp == BLOCK_HEELED_COMBAT_BOOT  || (otmp)->otyp == HIGH_HEELED_SANDAL  || (otmp)->otyp == SEXY_LEATHER_PUMP  || (otmp)->otyp == TORPEDO  || (otmp)->otyp == HOMING_TORPEDO  || (otmp)->otyp == STEEL_WHIP  )

#define ammo_and_launcher(otmp,ltmp) \
			(is_ammo(otmp) && (ltmp) && \
			((objects[(otmp)->otyp].oc_skill == -objects[(ltmp)->otyp].oc_skill && \
			  objects[(otmp)->otyp].w_ammotyp == objects[(ltmp)->otyp].w_ammotyp) || \
			(objects[(otmp)->otyp].w_ammotyp == WP_BULLET_PISTOL && itemhasappearance(ltmp, APP_PISTOL_BOOTS)) ))
#define bimanual(otmp)	(((otmp)->oclass == WEAPON_CLASS || (otmp)->oclass == BALL_CLASS || \
			  (otmp)->oclass == CHAIN_CLASS || (otmp)->oclass == TOOL_CLASS) && \
			 objects[(otmp)->otyp].oc_bimanual)

#define is_lightsaber(otmp) (objects[(otmp)->otyp].oc_skill == P_LIGHTSABER || (otmp)->otyp == LASER_SWATTER || (otmp)->otyp == NANO_HAMMER || (otmp)->otyp == LOWER_ARM_SWORD || (otmp)->otyp == LIGHTWHIP || (otmp)->otyp == LASERDENT || (otmp)->otyp == LASER_FLYAXE || (otmp)->otyp == PINK_DOUBLE_LIGHTSWORD || (otmp)->otyp == LASERXBOW || (otmp)->otyp == STARWARS_MACE || (otmp)->otyp == LASER_POLE || (otmp)->otyp == KLIUSLING || (otmp)->otyp == LASER_SWORD || (otmp)->otyp == LIGHTTORCH || (otmp)->otyp == PINK_LIGHTSWORD || (otmp)->otyp == BEAMSWORD || (otmp)->otyp == SITH_STAFF || (otmp)->otyp == ELECTRIC_CIGARETTE || (otmp)->otyp == HEAVY_LASER_BALL || (otmp)->otyp == LASER_CHAIN || (otmp)->otyp == LASERFIST || (otmp)->otyp == LASER_TIN_OPENER)

#define is_lightsaber_onum(number) (objects[number].oc_skill == P_LIGHTSABER || number == LASER_SWATTER || number == NANO_HAMMER || number == LOWER_ARM_SWORD || number == LIGHTWHIP || number == LASERDENT || number == LASER_FLYAXE || number == PINK_DOUBLE_LIGHTSWORD || number == LASERXBOW || number == STARWARS_MACE || number == LASER_POLE || number == KLIUSLING || number == LASER_SWORD || number == LIGHTTORCH || number == PINK_LIGHTSWORD || number == BEAMSWORD || number == SITH_STAFF || number == ELECTRIC_CIGARETTE || number == HEAVY_LASER_BALL || number == LASER_CHAIN || number == LASERFIST || number == LASER_TIN_OPENER)

#define is_firearm(otmp) \
			((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill == P_FIREARM)
#define has_firemodes(otmp) \
			((otmp)->otyp == ASSAULT_RIFLE || (otmp)->otyp == STORM_RIFLE || (otmp)->otyp == KALASHNIKOV || (otmp)->otyp == AUTO_SHOTGUN || (otmp)->otyp == DEMON_CROSSBOW || (otmp)->otyp == SUBMACHINE_GUN || (otmp)->otyp == LEAD_UNLOADER || (otmp)->otyp == MILITARY_RIFLE)
#define is_bullet(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill == -P_FIREARM)

/* Armor */
#define is_shield(otmp) ((otmp)->oclass == ARMOR_CLASS && \
			 objects[(otmp)->otyp].oc_armcat == ARM_SHIELD)
#define is_helmet(otmp) ((otmp)->oclass == ARMOR_CLASS && \
			 objects[(otmp)->otyp].oc_armcat == ARM_HELM)
#define is_boots(otmp)	((otmp)->oclass == ARMOR_CLASS && \
			 objects[(otmp)->otyp].oc_armcat == ARM_BOOTS)
#define is_gloves(otmp) ((otmp)->oclass == ARMOR_CLASS && \
			 objects[(otmp)->otyp].oc_armcat == ARM_GLOVES)
#define is_cloak(otmp)	((otmp)->oclass == ARMOR_CLASS && \
			 objects[(otmp)->otyp].oc_armcat == ARM_CLOAK)
#define is_shirt(otmp)	((otmp)->oclass == ARMOR_CLASS && \
			 objects[(otmp)->otyp].oc_armcat == ARM_SHIRT)
#define is_suit(otmp)	((otmp)->oclass == ARMOR_CLASS && \
			 objects[(otmp)->otyp].oc_armcat == ARM_SUIT)
#define is_droven_armor(otmp)	((otmp)->otyp == DROVEN_HELM\
				|| (otmp)->otyp == DROVEN_PLATE_MAIL\
				|| (otmp)->otyp == DROVEN_CHAIN_MAIL\
				|| (otmp)->otyp == DROVEN_MITHRIL_COAT\
				|| (otmp)->otyp == DROVEN_CLOAK)
#define is_elven_armor(otmp)	((otmp)->otyp == ELVEN_LEATHER_HELM\
				|| (otmp)->otyp == ELVEN_MITHRIL_COAT\
				|| (otmp)->otyp == ELVEN_CLOAK\
				|| (otmp)->otyp == ELVEN_GAUNTLETS\
				|| (otmp)->otyp == ELVEN_HELM\
				|| (otmp)->otyp == HIGH_ELVEN_HELM\
				|| (otmp)->otyp == ELVEN_TOGA\
				|| (otmp)->otyp == IMPERIAL_ELVEN_ARMOR\
				|| (otmp)->otyp == HIGH_ELVEN_PLATE\
				|| (otmp)->otyp == ELVEN_SHIELD\
				|| (otmp)->otyp == ELVEN_BOOTS)
#define is_orcish_armor(otmp)	((otmp)->otyp == ORCISH_HELM\
				|| (otmp)->otyp == ORCISH_CHAIN_MAIL\
				|| (otmp)->otyp == ORCISH_RING_MAIL\
				|| (otmp)->otyp == ORCISH_CLOAK\
				|| (otmp)->otyp == ORCISH_BOOTS\
				|| (otmp)->otyp == URUK_HAI_SHIELD\
				|| (otmp)->otyp == ORCISH_GUARD_SHIELD\
				|| (otmp)->otyp == ORCISH_SHIELD)
#define is_dwarvish_armor(otmp)	((otmp)->otyp == DWARVISH_IRON_HELM\
				|| (otmp)->otyp == DWARVISH_MITHRIL_COAT\
				|| (otmp)->otyp == DWARVISH_CLOAK\
				|| (otmp)->otyp == DWARVISH_ROUNDSHIELD)
#define is_gnomish_armor(otmp)	((otmp)->otyp == GNOMISH_HELM\
				|| (otmp)->otyp == GNOMISH_SUIT\
				|| (otmp)->otyp == GNOMISH_BOOTS)

#define is_blindfold_slot(otmp)	((otmp)->otyp == BLINDFOLD\
				|| (otmp)->otyp == CONDOME\
				|| (otmp)->otyp == TOWEL\
				|| (otmp)->otyp == CLIMBING_SET\
				|| (otmp)->otyp == DEFUSING_BOX\
				|| (otmp)->otyp == SOFT_CHASTITY_BELT\
				|| (otmp)->otyp == EYECLOSER\
				|| (otmp)->otyp == DRAGON_EYEPATCH\
				|| (otmp)->otyp == LENSES\
				|| (otmp)->otyp == BOSS_VISOR\
				|| (otmp)->otyp == NIGHT_VISION_GOGGLES\
				|| (otmp)->otyp == SHIELD_PATE_GLASSES\
				|| (otmp)->otyp == RADIOGLASSES)

#define is_blindfoldbase(o)	(o == BLINDFOLD\
				|| o == CONDOME\
				|| o == TOWEL\
				|| o == CLIMBING_SET\
				|| o == DEFUSING_BOX\
				|| o == SOFT_CHASTITY_BELT\
				|| o == EYECLOSER\
				|| o == DRAGON_EYEPATCH\
				|| o == LENSES\
				|| o == BOSS_VISOR\
				|| o == NIGHT_VISION_GOGGLES\
				|| o == SHIELD_PATE_GLASSES\
				|| o == RADIOGLASSES)

/* and yet another one... mkobj.c checks whether a magical and/or non-vanilla piece of armor is rerolled */
#define isvanillaarmor(number) (number == HAWAIIAN_SHIRT || number == T_SHIRT || number == STRIPED_SHIRT || number == PLATE_MAIL || number == RIBBED_PLATE_MAIL || number == PARTIAL_PLATE_MAIL || number == FULL_PLATE_MAIL || number == PLASTEEL_ARMOR || number == CRYSTAL_PLATE_MAIL || number == BRONZE_PLATE_MAIL || number == SPLINT_MAIL || number == THROWAWAY_ARMOR || number == MESH_ARMOR || number == CUIRASS_ARMOR || number == METAL_LAMELLAR_ARMOR || number == BANDED_MAIL || number == DWARVISH_MITHRIL_COAT || number == DARK_ELVEN_MITHRIL_COAT || number == ELVEN_MITHRIL_COAT || number == GNOMISH_SUIT || number == CHAIN_MAIL || number == BAR_CHAIN_MAIL || number == BAMBOO_MAIL || number == SILK_MAIL || number == ORCISH_CHAIN_MAIL || number == SCALE_MAIL || number == STUDDED_LEATHER_ARMOR || number == RING_MAIL || number == HEAVY_MAIL || number == ROUND_MAIL || number == LIGHT_MAIL || number == ORCISH_RING_MAIL || number == LEATHER_ARMOR || number == LEATHER_JACKET || number == PLAIN_ROBE || number == MUMMY_WRAPPING || number == ORCISH_CLOAK || number == DWARVISH_CLOAK || number == ELVEN_CLOAK || number == LEATHER_CLOAK || number == PLASTEEL_CLOAK || number == ELVEN_LEATHER_HELM || number == GNOMISH_HELM || number == ORCISH_HELM || number == DWARVISH_IRON_HELM || number == FEDORA || number == DENTED_POT || number == STANDARD_HELMET || number == PLASTEEL_GLOVES || number == REGULAR_GLOVES || number == SMALL_SHIELD || number == BUCKLER || number == LIGHT_SHIELD || number == CRINGE_SHIELD || number == BRONZE_SHIELD || number == HEAVY_SHIELD || number == ELVEN_SHIELD || number == URUK_HAI_SHIELD || number == ORCISH_SHIELD || number == LARGE_SHIELD || number == PAPER_SHIELD || number == DWARVISH_ROUNDSHIELD || number == SHIELD || number == LOW_BOOTS || number == IRON_SHOES || number == GNOMISH_BOOTS || number == HIGH_BOOTS)
	
/* Eggs and other food */
#define MAX_EGG_HATCH_TIME 200	/* longest an egg can remain unhatched */
#define stale_egg(egg)	((monstermoves - (egg)->age) > (2*MAX_EGG_HATCH_TIME))
#define ofood(o) ((o)->otyp == CORPSE || (o)->otyp == EGG || (o)->otyp == TIN)
#define polyfodder(obj)	(ofood(obj) && (obj)->corpsenm == PM_CHAMELEON)
#define mlevelgain(obj) (ofood(obj) && (obj)->corpsenm == PM_WRAITH)
#define mhealup(obj)	(ofood(obj) && (obj)->corpsenm == PM_NURSE)
#define drainlevel(corpse) (mons[(corpse)->corpsenm].cnutrit*4/5)

/* Containers */
#define carried(o)	((o)->where == OBJ_INVENT)
#define mcarried(o)	((o)->where == OBJ_MINVENT)
#define Has_contents(o) (/* (Is_container(o) || (o)->otyp == STATUE) && */ \
			 (o)->cobj != (struct obj *)0)
#define Is_container(o) ((o)->otyp == MEDICAL_KIT || \
			 (o)->otyp >= LARGE_BOX && (o)->otyp <= BAG_OF_TRICKS)
#define Is_box(otmp)	((otmp)->otyp == LARGE_BOX || (otmp)->otyp == LEAD_BOX || (otmp)->otyp == TOP_BOX || (otmp)->otyp == CHEST || (otmp)->otyp == NANO_CHEST || (otmp)->otyp == TREASURE_CHEST || (otmp)->otyp == LOOT_CHEST || (otmp)->otyp == LARGE_BOX_OF_DIGESTION || (otmp)->otyp == CHEST_OF_HOLDING)
#define Is_mbag(otmp)	((otmp)->otyp == BAG_OF_HOLDING || (otmp)->otyp == ICE_BOX_OF_HOLDING || (otmp)->otyp == CHEST_OF_HOLDING || \
                         ((otmp)->oartifact && \
                          ( (otmp)->oartifact == ART_WALLET_OF_PERSEUS || (otmp)->oartifact == ART_SACK_OF_HOLDING || (otmp)->oartifact == ART_PRIAMOS__TREASURE || (otmp)->oartifact == ART_ONE_SIZE_FITS_EVERYTHING) ) || \
  			             (otmp)->otyp == BAG_OF_TRICKS)

/* dragon gear */
#define Is_dragon_scales(obj)	((obj)->otyp >= GRAY_DRAGON_SCALES && \
				 (obj)->otyp <= YELLOW_DRAGON_SCALES)
#define Is_dragon_mail(obj)	((obj)->otyp >= GRAY_DRAGON_SCALE_MAIL && \
				 (obj)->otyp <= YELLOW_DRAGON_SCALE_MAIL)
#define Is_dragon_armor(obj)	(Is_dragon_scales(obj) || Is_dragon_mail(obj))
#define Dragon_scales_to_pm(obj) &mons[PM_GRAY_DRAGON + (obj)->otyp \
				       - GRAY_DRAGON_SCALES]
#define Dragon_mail_to_pm(obj)	&mons[PM_GRAY_DRAGON + (obj)->otyp \
				      - GRAY_DRAGON_SCALE_MAIL]
#define Dragon_to_scales(pm)	(GRAY_DRAGON_SCALES + (pm - mons))

#define Is_dragon_shield(obj)	((obj)->otyp >= GRAY_DRAGON_SCALE_SHIELD && \
				 (obj)->otyp <= YELLOW_DRAGON_SCALE_SHIELD)

/* Elven gear */
#define is_elven_weapon(otmp)	((otmp)->otyp == ELVEN_ARROW\
				|| (otmp)->otyp == ELVEN_SPEAR\
				|| (otmp)->otyp == ELVEN_MACE\
				|| (otmp)->otyp == ELVEN_LANCE\
				|| (otmp)->otyp == ELVEN_DAGGER\
				|| (otmp)->otyp == ELVEN_SHORT_SWORD\
				|| (otmp)->otyp == HIGH_ELVEN_WARSWORD\
				|| (otmp)->otyp == ELVEN_BROADSWORD\
				|| (otmp)->otyp == ELVEN_BOW)
#define is_elven_obj(otmp)	(is_elven_armor(otmp) || is_elven_weapon(otmp))

#define is_droven_weapon(otmp)	((otmp)->otyp == DROVEN_ARROW\
				|| (otmp)->otyp == DROVEN_SPEAR\
				|| (otmp)->otyp == DROVEN_LANCE\
				|| (otmp)->otyp == DROVEN_DAGGER\
				|| (otmp)->otyp == DROVEN_SHORT_SWORD\
				|| (otmp)->otyp == DROVEN_GREATSWORD\
				|| (otmp)->otyp == DROVEN_BOW\
				|| (otmp)->otyp == DROVEN_CROSSBOW\
				|| (otmp)->otyp == DROVEN_BOLT)

/* Orcish gear */
#define is_orcish_obj(otmp)	(is_orcish_armor(otmp)\
				|| (otmp)->otyp == ORCISH_ARROW\
				|| (otmp)->otyp == ORCISH_SPEAR\
				|| (otmp)->otyp == ORCISH_DAGGER\
				|| (otmp)->otyp == ORCISH_SCIMITAR\
				|| (otmp)->otyp == ORCISH_LONG_SWORD\
				|| (otmp)->otyp == ORCISH_MORNING_STAR\
				|| (otmp)->otyp == ORCISH_SHORT_SWORD\
				|| (otmp)->otyp == ORCISH_BOW)

/* Dwarvish gear */
#define is_dwarvish_obj(otmp)	(is_dwarvish_armor(otmp)\
				|| (otmp)->otyp == DWARVISH_SPEAR\
				|| (otmp)->otyp == DWARVISH_BATTLE_AXE\
				|| (otmp)->otyp == DWARVISH_SHORT_SWORD\
				|| (otmp)->otyp == DWARVISH_MATTOCK)

/* Gnomish gear */
#define is_gnomish_obj(otmp)	(is_gnomish_armor(otmp))

/* items that break when thrown even if you change their material to something else --Amy */
#define is_vitric(otmp) ((otmp)->otyp == CRYSTAL_SWORD\
				|| (otmp)->otyp == DIAMOND_SMASHER\
				|| (otmp)->otyp == LIGHTBULB\
				|| (otmp)->otyp == DROVEN_BOW\
				|| (otmp)->otyp == DROVEN_CROSSBOW\
				|| (otmp)->otyp == BULLETPROOF_CHAINWHIP\
				|| (otmp)->otyp == DROVEN_DAGGER\
				|| (otmp)->otyp == OBSIDIAN_AXE\
				|| (otmp)->otyp == DROVEN_SHORT_SWORD\
				|| (otmp)->otyp == VOLCANIC_BROADSWORD\
				|| (otmp)->otyp == DROVEN_GREATSWORD\
				|| (otmp)->otyp == DROVEN_SPEAR\
				|| (otmp)->otyp == DROVEN_LANCE\
				|| (otmp)->otyp == DROVEN_ARROW\
				|| (otmp)->otyp == DROVEN_BOLT)

/* special artifacts and stuff that can be used for digging despite not being a pick --Amy */
#define can_dig_with(otmp) ((otmp)->oartifact == ART_EN_GARDE____TOUCHE_\
				|| (otmp)->oartifact == ART_STONEBREAKER)

/* Light sources */
#define Is_candle(otmp)	((otmp)->otyp == TALLOW_CANDLE || \
			 (otmp)->otyp == WAX_CANDLE || \
			 (otmp)->otyp == JAPAN_WAX_CANDLE || \
			 (otmp)->otyp == OIL_CANDLE || \
			 (otmp)->otyp == GENERAL_CANDLE || \
			 (otmp)->otyp == NATURAL_CANDLE || \
			 (otmp)->otyp == UNSPECIFIED_CANDLE || \
			 (otmp)->otyp == MAGIC_CANDLE || \
			 (otmp)->otyp == UNAFFECTED_CANDLE || \
			 (otmp)->otyp == SPECIFIC_CANDLE || \
			 (otmp)->otyp == __CANDLE)
/* maximum amount of oil in a potion of oil */
#define MAX_OIL_IN_FLASK 400

/* MAGIC_LAMP intentionally excluded below */
/* age field of this is relative age rather than absolute */
#define age_is_relative(otmp)	((otmp)->otyp == BRASS_LANTERN\
				|| (otmp)->otyp == OIL_LAMP\
				|| (otmp)->otyp == DIM_LANTERN\
				|| (otmp)->otyp == TORCH\
				|| (otmp)->otyp == CANDELABRUM_OF_INVOCATION\
				|| (otmp)->otyp == TALLOW_CANDLE\
				|| (otmp)->otyp == WAX_CANDLE\
				|| (otmp)->otyp == JAPAN_WAX_CANDLE\
				|| (otmp)->otyp == OIL_CANDLE\
				|| (otmp)->otyp == GENERAL_CANDLE\
				|| (otmp)->otyp == NATURAL_CANDLE\
				|| (otmp)->otyp == UNSPECIFIED_CANDLE\
				|| (otmp)->otyp == UNAFFECTED_CANDLE\
				|| (otmp)->otyp == SPECIFIC_CANDLE\
				|| (otmp)->otyp == __CANDLE\
				|| (otmp)->otyp == POT_OIL)
/* object can be ignited */
#define ignitable(otmp)	((otmp)->otyp == BRASS_LANTERN\
				|| (otmp)->otyp == OIL_LAMP\
				|| (otmp)->otyp == DIM_LANTERN\
				|| (otmp)->otyp == TORCH\
				|| (otmp)->otyp == CANDELABRUM_OF_INVOCATION\
				|| (otmp)->otyp == TALLOW_CANDLE\
				|| (otmp)->otyp == WAX_CANDLE\
				|| (otmp)->otyp == JAPAN_WAX_CANDLE\
				|| (otmp)->otyp == OIL_CANDLE\
				|| (otmp)->otyp == GENERAL_CANDLE\
				|| (otmp)->otyp == NATURAL_CANDLE\
				|| (otmp)->otyp == UNSPECIFIED_CANDLE\
				|| (otmp)->otyp == UNAFFECTED_CANDLE\
				|| (otmp)->otyp == SPECIFIC_CANDLE\
				|| (otmp)->otyp == __CANDLE\
				|| (otmp)->otyp == MAGIC_CANDLE\
				|| (otmp)->otyp == POT_OIL)

/* special stones */
#define is_nastygraystone(obj)	( ((obj)->otyp >= RIGHT_MOUSE_BUTTON_STONE) && ((obj)->otyp <= NASTY_STONE) )
#define is_feminismstone(obj)	( ((obj)->otyp >= ELIF_S_JEWEL) && ((obj)->otyp <= DORA_S_JEWEL) )

#define is_graystone(obj)	((obj)->otyp == LUCKSTONE || \
				 (obj)->otyp == LOADSTONE || \
				 (obj)->otyp == FLINT     || \
				 (obj)->otyp == TALC     || \
				 (obj)->otyp == GRAPHITE     || \
				 (obj)->otyp == VOLCANIC_GLASS_FRAGMENT     || \
				 (obj)->otyp == TOUCHSTONE || \
				 (obj)->otyp == LEAD_CLUMP || \
				 (obj)->otyp == JUNK_METAL || \
				 (obj)->otyp == COBALT_CHUNK || \
				 (obj)->otyp == POINTY_SHELL || \
				 (obj)->otyp == BRONZE_NUGGET || \
				 (obj)->otyp == STEEL_SLUG || \
				 (obj)->otyp == SLING_AMMO || \
				 (obj)->otyp == BONE_FRAGMENT || \
				 (obj)->otyp == METEOR_FRAGMENT || \
				 (obj)->otyp == AMBER_FRAGMENT || \
				 (obj)->otyp == HEALTHSTONE || \
				 (obj)->otyp == SALT_CHUNK || \
				 (obj)->otyp == MANASTONE || \
				 (obj)->otyp == SMALL_PIECE_OF_UNREFINED_MITHR || \
				 (obj)->otyp == SILVER_SLINGSTONE || \
				 (obj)->otyp == CONUNDRUM_NUGGET || \
				 (obj)->otyp == STONE_OF_MAGIC_RESISTANCE || \
				 (obj)->otyp == LOADBOULDER || \
				 (obj)->otyp == STARLIGHTSTONE || \
				 (obj)->otyp == SLEEPSTONE || \
				 (obj)->otyp == WHETSTONE || is_nastygraystone(obj) )

#define is_heavyweapon(obj)	((bimanual(obj) && objects[(obj)->otyp].oc_skill >= P_DAGGER && objects[(obj)->otyp].oc_skill <= P_WHIP && objects[(obj)->otyp].oc_skill != P_POLEARMS && !is_lightsaber(obj) && objects[(obj)->otyp].oc_skill != P_QUARTERSTAFF && !(objects[(obj)->otyp].oc_skill >= P_LANCE && objects[(obj)->otyp].oc_skill <= P_BOOMERANG) ) || obj->otyp == ATGEIR)

/* misc - grep target "materialeffect" because materials could be flimsy */
#define is_flimsy(otmp)		(objects[(otmp)->otyp].oc_material <= MT_LEATHER || objects[(otmp)->otyp].oc_material == MT_INKA || objects[(otmp)->otyp].oc_material == MT_SILK || objects[(otmp)->otyp].oc_material == MT_FILM || objects[(otmp)->otyp].oc_material == MT_FOAM || objects[(otmp)->otyp].oc_material == MT_TINSEL || \
				 (otmp)->otyp == RUBBER_HOSE)
/* note by Amy: it is intentional that secree and compost don't appear here */

/* helpers, simple enough to be macros */
#define is_plural(o)	((o)->quan > 1 || \
			 (o)->oartifact == ART_EYES_OF_THE_OVERWORLD)

/* Flags for get_obj_location(). */
#define CONTAINED_TOO	0x1
#define BURIED_TOO	0x2

#endif /* OBJ_H */
