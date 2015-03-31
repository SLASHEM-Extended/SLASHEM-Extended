/*	SCCS Id: @(#)mon.c	3.4	2003/12/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* If you're using precompiled headers, you don't want this either */
#ifdef MICROPORT_BUG
#define MKROOM_H
#endif

#include "hack.h"
#include "mfndpos.h"
#include "edog.h"
#include "artifact.h"
#include "display.h"
#include "global.h" 
#include "quest.h"

#include <ctype.h>

void FDECL(display_monster,(XCHAR_P,XCHAR_P,struct monst *,int,XCHAR_P));
 
STATIC_DCL boolean FDECL(restrap,(struct monst *));
STATIC_DCL long FDECL(mm_aggression, (struct monst *,struct monst *));
#ifdef OVL2
STATIC_DCL int NDECL(pick_animal);
STATIC_DCL int FDECL(select_newcham_form, (struct monst *));
STATIC_DCL void FDECL(kill_eggs, (struct obj *));
#endif

/* make wraith luring unnecessary --Amy */

#ifdef REINCARNATION
#define LEVEL_SPECIFIC_NOCORPSE(mdat) \
	 (Is_rogue_level(&u.uz) || \
	   ( (level.flags.graveyard || mdat == &mons[PM_WRAITH] || mdat == &mons[PM_CREEPING___] ) && is_undead(mdat) && \
	    mdat != &mons[PM_VECNA] && rn2(3)))
#else
#define LEVEL_SPECIFIC_NOCORPSE(mdat) \
	   ((level.flags.graveyard || mdat == &mons[PM_WRAITH] || mdat == &mons[PM_CREEPING___] ) && is_undead(mdat) && \
	    mdat != &mons[PM_VECNA] && rn2(3))
#endif

#define Qstatf(x)	(quest_status.x)

#if 0
/* part of the original warning code which was replaced in 3.3.1 */
#ifdef OVL1
#define warnDelay 10
long lastwarntime;
int lastwarnlev;


STATIC_DCL void NDECL(warn_effects);
#endif /* OVL1 */
#endif /* 0 */

#ifndef OVLB
STATIC_VAR short cham_to_pm[];
#else
STATIC_DCL struct obj *FDECL(make_corpse,(struct monst *));
STATIC_DCL void FDECL(m_detach, (struct monst *, struct permonst *));
STATIC_DCL void FDECL(lifesaved_monster, (struct monst *));
static void FDECL(unpoly_monster, (struct monst *));

/* convert the monster index of an undead to its living counterpart */
int
undead_to_corpse(mndx)
int mndx;
{
	switch (mndx) {
	case PM_KOBOLD_ZOMBIE:
	case PM_KOBOLD_MUMMY:	mndx = PM_KOBOLD;  break;
	case PM_OGRE_ZOMBIE:
	case PM_OGRE_MUMMY:	mndx = PM_OGRE;  break;
	case PM_DWARF_ZOMBIE:
	case PM_DWARF_MUMMY:	mndx = PM_DWARF;  break;
	case PM_GNOME_ZOMBIE:
	case PM_GNOME_MUMMY:	mndx = PM_GNOME;  break;
	case PM_ORC_ZOMBIE:
	case PM_ORC_MUMMY:	mndx = PM_ORC;  break;
	case PM_DROW_MUMMY:
	case PM_DROW_ZOMBIE:
	case PM_ELF_ZOMBIE:
	case PM_ELF_MUMMY:	mndx = PM_ELF;  break;
	case PM_VAMPIRE:
	case PM_VAMPIRE_LORD:
	case PM_VAMPIRE_MAGE:
	case PM_HUMAN_ZOMBIE:
	case PM_BLACK_DEATH:
	case PM_DRAUGR_DEATHLORD:
	case PM_WALKING_CORPSE:
	case PM_HUNGRY_DEAD_POTATO:
	case PM_ROTTING_CARCASS:
	case PM_DROWNED_DEAD_ONE:
	case PM_FLESHEATER:
	case PM_PALE_SHAMBLER:
	case PM_EMBER_THRALL:
	case PM_FIERY_CARCASS:
	case PM_ZOMBIE_CAR:
	case PM_PLAGUE_BEARER:
	case PM_GREENER_ZOMBIE:
	case PM_ZOMBIE_BADDIE:
	case PM_ZOMBIE_WARRIOR:
	case PM_MAFIOSO:
	case PM_TALIBAN_MILITIA:
	case PM_M__TO_M___:
	case PM_ETA_TERRORIST:
	case PM_BLACK_ONES:
	case PM_CORNFLAKE_ZOMBIE:
	case PM_ZOMBIE:
	case PM_GNOLL_GHOUL:
	case PM_DRAUGR_ZOMBIE:
	case PM_STEEL_ZOMBIE:
	case PM_DRAUGR_MUMMY:
	case PM_GREATER_MUMMY:
	case PM_MUMMY:
	case PM_TOMB_MUMMY:
	case PM_CADAVER:
	case PM_POISONOUS_WIMP:
	case PM_PRESERVED_DEAD:
	case PM_RECEIVED_DEAD_POTATO:
	case PM_EMBALMED_CORPSE:
	case PM_TOXICATED_POTATO:
	case PM_DRIED_CORPSE:
	case PM_CORRUPTED_SHAMBLER:
	case PM_DEAD_PHARAO:
	case PM_DECAYED_CORPSE:
	case PM_WAIVED_POTATO:
	case PM_DEAD_POTATO:
	case PM_ADOM_MUMMY:
	case PM_HUMAN_MUMMY:	mndx = PM_HUMAN;  break;
	case PM_GIANT_ZOMBIE:
	case PM_GIANT_MUMMY:	mndx = PM_GIANT;  break;
	case PM_ETTIN_ZOMBIE:
	case PM_ETTIN_MUMMY:	mndx = PM_ETTIN;  break;
	case PM_TROLL_ZOMBIE:
	case PM_TROLL_MUMMY:    mndx = PM_TROLL;  break;
	case PM_MIMIC_MUMMY:    mndx = PM_GIANT_MIMIC;  break;
	case PM_TASMANIAN_ZOMBIE:    mndx = PM_TASMANIAN_DEVIL;  break;
	default:  break;
	}
	return mndx;
}

/* Convert the monster index of some monsters (such as quest guardians)
 * to their generic species type.
 *
 * Return associated character class monster, rather than species
 * if mode is 1.
 */
int
genus(mndx, mode)
int mndx, mode;
{
	switch (mndx) {
/* Quest guardians */
	case PM_STUDENT:     mndx = mode ? PM_ARCHEOLOGIST  : PM_HUMAN; break;
	case PM_CHIEFTAIN:   mndx = mode ? PM_BARBARIAN : PM_HUMAN; break;
	case PM_NEANDERTHAL: mndx = mode ? PM_CAVEMAN   : PM_HUMAN; break;
	case PM_ATTENDANT:   mndx = mode ? PM_HEALER    : PM_HUMAN; break;
	case PM_UNFORTUNATE_VICTIM:   mndx = mode ? PM_BLEEDER    : PM_HUMAN; break;
	case PM_PAGE:        mndx = mode ? PM_KNIGHT    : PM_HUMAN; break;
	case PM_RHYMER:      mndx = mode ? PM_BARD   : PM_HUMAN; break;
	case PM_ABBOT:       mndx = mode ? PM_MONK      : PM_HUMAN; break;
	case PM_ACOLYTE:     mndx = mode ? PM_PRIEST    : PM_HUMAN; break;
	case PM_HUNTER:      mndx = mode ? PM_RANGER    : PM_HUMAN; break;
	case PM_THUG:        mndx = mode ? PM_ROGUE     : PM_HUMAN; break;
	case PM_ROSHI:       mndx = mode ? PM_SAMURAI   : PM_HUMAN; break;
#ifdef TOURIST
	case PM_GUIDE:       mndx = mode ? PM_TOURIST   : PM_HUMAN; break;
#endif
	case PM_APPRENTICE:  mndx = mode ? PM_WIZARD    : PM_HUMAN; break;
	case PM_SUPER_WARRIOR:     mndx = mode ? PM_VALKYRIE  : PM_HUMAN; break;
	default:
		if (mndx >= LOW_PM && mndx < NUMMONS) {
			struct permonst *ptr = &mons[mndx];
			if (is_human(ptr))      mndx = PM_HUMAN;
			else if (is_elf(ptr))   mndx = PM_ELF;
			else if (is_dwarf(ptr)) mndx = PM_DWARF;
			else if (is_gnome(ptr)) mndx = PM_GNOME;
			else if (is_orc(ptr))   mndx = PM_ORC;
		}
		break;
	}
	return mndx;
}

/* convert monster index to chameleon index */
int
pm_to_cham(mndx)
int mndx;
{
	int mcham;

	switch (mndx) {
	case PM_CHAMELEON:	mcham = CHAM_CHAMELEON; break;
	case PM_DOPPELGANGER:	mcham = CHAM_DOPPELGANGER; break;
	case PM_DOPPLEZON:	mcham = CHAM_DOPPLEZON; break;
	case PM_SANDESTIN:	mcham = CHAM_SANDESTIN; break;
	case PM_MISSINGNO:	mcham = CHAM_MISSINGNO; break;
	case PM_TRANSFORMER:	mcham = CHAM_TRANSFORMER; break;
	case PM_CHAOS_SHAPECHANGER:	mcham = CHAM_CHAOS_SHAPECHANGER; break;
	case PM_GIANT_CHAMELEON:	mcham = CHAM_GIANT_CHAMELEON; break;
	default: mcham = CHAM_ORDINARY; break;
	}
	return mcham;
}

/* convert chameleon index to monster index */
STATIC_VAR short cham_to_pm[] = {
		NON_PM,		/* placeholder for CHAM_ORDINARY */
		PM_CHAMELEON,
		PM_DOPPELGANGER,
		PM_DOPPLEZON,
		PM_SANDESTIN,
		PM_MISSINGNO,
		PM_TRANSFORMER,
		PM_CHAOS_SHAPECHANGER,
		PM_GIANT_CHAMELEON,
};

/* for deciding whether corpse or statue will carry along full monster data */
#define KEEPTRAITS(mon)	((mon)->isshk || (mon)->isgyp || (mon)->mtame ||\
			 ((mon)->data->geno & G_UNIQ) ||		\
			 is_reviver((mon)->data) ||			\
			 /* normally leader the will be unique, */	\
			 /* but he might have been polymorphed  */	\
			 (mon)->m_id == quest_status.leader_m_id ||	\
			 /* special cancellation handling for these */	\
			 (dmgtype((mon)->data, AD_SEDU) ||		\
			  dmgtype((mon)->data, AD_SSEX)))

/* Creates a monster corpse, a "special" corpse, or nothing if it doesn't
 * leave corpses.  Monsters which leave "special" corpses should have
 * G_NOCORPSE set in order to prevent wishing for one, finding tins of one,
 * etc....
 */
STATIC_OVL struct obj *
make_corpse(mtmp)
register struct monst *mtmp;
{
	register struct permonst *mdat = mtmp->data;
	int num;
	struct obj *obj = (struct obj *)0;
	int x = mtmp->mx, y = mtmp->my;
	int mndx = monsndx(mdat);

	switch(mndx) {
	    case PM_GRAY_DRAGON:
	    case PM_SILVER_DRAGON:
	    case PM_SHIMMERING_DRAGON:
	    case PM_DEEP_DRAGON:
	    case PM_RED_DRAGON:
	    case PM_ORANGE_DRAGON:
	    case PM_WHITE_DRAGON:
	    case PM_BLACK_DRAGON:
	    case PM_BLUE_DRAGON:
	    case PM_GREEN_DRAGON:
	    case PM_GOLDEN_DRAGON:
	    case PM_STONE_DRAGON:
	    case PM_CYAN_DRAGON:
	    case PM_YELLOW_DRAGON:
		/* Make dragon scales.  This assumes that the order of the */
		/* dragons is the same as the order of the scales.	   */
		if (!rn2(mtmp->mrevived ? 20 : 3)) {
		    num = GRAY_DRAGON_SCALES + monsndx(mdat) - PM_GRAY_DRAGON;
		    obj = mksobj_at(num, x, y, TRUE, FALSE); /* allow random enchantment and BUC --Amy */
		    /*obj->spe = 0;
		    obj->cursed = obj->blessed = FALSE;*/
		}
		goto default_1;

	    case PM_YOUNG_UNICORN:
		if (rn2(2)) goto default_1;
		/* fall through */
	    case PM_WHITE_UNICORN:
	    case PM_GRAY_UNICORN:
	    case PM_BLACK_UNICORN:
	    case PM_PURPLE_UNICORN:
	    case PM_BIG_PURPLE_UNICORN:
	    case PM_BIG_WHITE_UNICORN:
	    case PM_BIG_GRAY_UNICORN:
	    case PM_BIG_BLACK_UNICORN:
		if (mtmp->mrevived && rn2(20)) {
			if (canseemon(mtmp))
			   pline("%s recently regrown horn crumbles to dust.",
				s_suffix(Monnam(mtmp)));
		} else
			(void) mksobj_at(UNICORN_HORN, x, y, TRUE, FALSE);
		goto default_1;
	    case PM_LONG_WORM:
		(void) mksobj_at(WORM_TOOTH, x, y, TRUE, FALSE);
		goto default_1;
	    case PM_KILLER_TRIPE_RATION:            
		(void) mksobj_at(TRIPE_RATION, x, y, TRUE, FALSE);
		newsym(x, y);
		return (struct obj *)0;
	    case PM_KILLER_FOOD_RATION:
		(void) mksobj_at(FOOD_RATION, x, y, TRUE, FALSE);
		newsym(x, y);
		return (struct obj *)0;
	    case PM_VAMPIRE:
	    case PM_VAMPIRE_LORD:
	    case PM_VAMPIRE_MAGE:
		/* include mtmp in the mkcorpstat() call */
		num = undead_to_corpse(mndx);
		obj = mkcorpstat(CORPSE, mtmp, &mons[num], x, y, TRUE);
		obj->age -= 100;		/* this is an *OLD* corpse */
		break;
	    case PM_KOBOLD_MUMMY:
	    case PM_DWARF_MUMMY:
	    case PM_GNOME_MUMMY:
	    case PM_ORC_MUMMY:
	    case PM_ELF_MUMMY:
	    case PM_HUMAN_MUMMY:
	    case PM_MUMMY:
		case PM_TOMB_MUMMY:
	    case PM_ADOM_MUMMY:
	    case PM_MIMIC_MUMMY:
	    case PM_GIANT_MUMMY:
	    case PM_ETTIN_MUMMY:
	    case PM_TROLL_MUMMY:            
	    case PM_TROLL_ZOMBIE:            
	    case PM_KOBOLD_ZOMBIE:
	    case PM_OGRE_ZOMBIE:
	    case PM_JUJU_ZOMBI:
	    case PM_OGRE_MUMMY:
	    case PM_DWARF_ZOMBIE:
	    case PM_GNOME_ZOMBIE:
	    case PM_ORC_ZOMBIE:
	    case PM_ELF_ZOMBIE:
	    case PM_HUMAN_ZOMBIE:
	case PM_CADAVER:
	case PM_POISONOUS_WIMP:
	case PM_PRESERVED_DEAD:
	case PM_RECEIVED_DEAD_POTATO:
	case PM_EMBALMED_CORPSE:
	case PM_TOXICATED_POTATO:
	case PM_CORRUPTED_SHAMBLER:
	case PM_DRIED_CORPSE:
	case PM_DEAD_PHARAO:
	case PM_DECAYED_CORPSE:
	case PM_WAIVED_POTATO:
	case PM_DEAD_POTATO:
	    case PM_BLACK_DEATH:
	    case PM_DRAUGR_DEATHLORD:
	    case PM_TASMANIAN_ZOMBIE:
	    case PM_WALKING_CORPSE:
	case PM_HUNGRY_DEAD_POTATO:
	case PM_ROTTING_CARCASS:
	case PM_DROWNED_DEAD_ONE:
	case PM_FLESHEATER:
	case PM_PALE_SHAMBLER:
	case PM_EMBER_THRALL:
	case PM_FIERY_CARCASS:
	case PM_ZOMBIE_CAR:
	case PM_PLAGUE_BEARER:
	case PM_GREENER_ZOMBIE:
	    case PM_ZOMBIE_BADDIE:
	case PM_ZOMBIE_WARRIOR:
	    case PM_MAFIOSO:
	case PM_TALIBAN_MILITIA:
	case PM_M__TO_M___:
	    case PM_ETA_TERRORIST:
	    case PM_BLACK_ONES:
	    case PM_CORNFLAKE_ZOMBIE:
	    case PM_ZOMBIE:
	    case PM_GIANT_ZOMBIE:
	    case PM_ETTIN_ZOMBIE:
		case PM_GNOLL_GHOUL:
	    case PM_DRAUGR_ZOMBIE:
	    case PM_STEEL_ZOMBIE:
	    case PM_DRAUGR_MUMMY:
	    case PM_GREATER_MUMMY:
	    case PM_DROW_ZOMBIE:
	    case PM_DROW_MUMMY:
		num = undead_to_corpse(mndx);
		obj = mkcorpstat(CORPSE, mtmp, &mons[num], x, y, TRUE);
		obj->age -= 100;		/* this is an *OLD* corpse */
		break;
	    case PM_WIGHT:
	    case PM_FOREST_WIGHT:
	    case PM_GRAVE_WIGHT:
	    case PM_EMPEROR_WIGHT:
	    case PM_MEANIE:
	    case PM_CAR_THIEF:
	    case PM_WATER_WIGHT:
	    case PM_SUBWAY_WIGHT:
	    case PM_CRYPT_WIGHT:
	    case PM_BARROW_WIGHT:
	    case PM_TUNNEL_WIGHT:
	    case PM_TRAILER:
	    case PM_UNDEAD_BAT:
	    case PM_PETTY_TUNNEL_WIGHT:
	    case PM_CASTLE_WIGHT:
	    case PM_GRAVE_WARRIOR:
	    case PM_ULTRA_WIGHT:
	    case PM_GHOUL:
	    case PM_GHAST:
	    case PM_GASTLY:
	    case PM_HAUNTER:
	    case PM_GENGAR:
	    case PM_CREEPING___:
	    case PM_FRANKENSTEIN_S_MONSTER:
	    case PM_UNDEAD_MIMIC:
	    case PM_UNDEAD_KANGAROO:
	    case PM_RIBBON_FISH:
	    case PM_SKELETON_FISH:
	    case PM_BONE_FISH:
	    case PM_CORPSE_FISH:
	    case PM_HORROR_FISH:
	    case PM_UNDEAD_OLOG_HAI_AMBUSHER:
	    case PM_UNDEAD_ANT:
	    case PM_UNDEAD_COCKATRICE:
	    case PM_SKELETAL_HOUND:
	    case PM_CORPSE_HOUND:
	    case PM_ZOMBOCAT:
	    case PM_HAUNTED_TIGER:
	    case PM_UNDEAD_MIND_FLAYER:
	    case PM_UNDEAD_NYMPH:
	    case PM_SKELETTOCROTTA:
	    case PM_UNDEAD_RAT:
	    case PM_UNDEAD_SCORPION:
	    case PM_UNDEAD_HORSE:
	    case PM_UNDEAD_UNICORN: /* too decayed to leave a working horn, so we just don't leave one at all :D --Amy */
	    case PM_UNDEAD_CENTAUR:
	    case PM_UNDEAD_KOP:
	    case PM_UNDEAD_SERGEANT:
	    case PM_UNDEAD_LIEUTENANT:
	    case PM_UNDEAD_KOMMISSIONER:
	    case PM_UNDEAD_KAPTAIN:
	    case PM_UNDEAD_KCHIEF:
	    case PM_UNDEAD_KATCHER:
	    case PM_UNDEAD_SNAKE:
	    case PM_UNDEAD_NAGA:
	    case PM_UNDEAD_NAGA_HATCHLING:
	    case PM_UNDEAD_YETI:
	    case PM_UNDEAD_ARCHEOLOGIST:
	    case PM_UNDEAD_BARBARIAN:
	    case PM_UNDEAD_BINDER:
	    case PM_UNDEAD_BLEEDER:
	    case PM_UNDEAD_BARD:
	    case PM_UNDEAD_CAVEMAN:
	    case PM_UNDEAD_CAVEWOMAN:
	    case PM_UNDEAD_CHEVALIER:
	    case PM_UNDEAD_COURIER:
	    case PM_UNDEAD_SPACEWARS_FIGHTER:
	    case PM_UNDEAD_ZYBORG:
	    case PM_UNDEAD_DEATH_EATER:
	    case PM_UNDEAD_GANGSTER:
	    case PM_UNDEAD_POKEMON:
	    case PM_UNDEAD_ELECTRIC_MAGE:
	    case PM_UNDEAD_ACID_MAGE:
	    case PM_UNDEAD_FLAME_MAGE:
	    case PM_UNDEAD_CONVICT:
	    case PM_UNDEAD_LUNATIC:
	    case PM_UNDEAD_GEEK:
	    case PM_UNDEAD_GRADUATE:
	    case PM_UNDEAD_SCIENTIST:
	    case PM_UNDEAD_ROCKER:
	    case PM_UNDEAD_HEALER:
	    case PM_UNDEAD_ICE_MAGE:
	    case PM_UNDEAD_KNIGHT:
	    case PM_UNDEAD_WARRIOR:
	    case PM_UNDEAD_MONK:
	    case PM_UNDEAD_PSION:
	    case PM_UNDEAD_NECROMANCER:
	    case PM_UNDEAD_NOBLEMAN:
	    case PM_UNDEAD_NOBLEWOMAN:
	    case PM_UNDEAD_PIRATE:
	    case PM_UNDEAD_KORSAIR:
	    case PM_UNDEAD_CRUEL_ABUSER:
	    case PM_UNDEAD_SAIYAN:
	    case PM_UNDEAD_POLITICIAN:
	    case PM_UNDEAD_LIBRARIAN:
	    case PM_UNDEAD_COOK:
	    case PM_UNDEAD_AUGURER:
	    case PM_UNDEAD_SAGE:
	    case PM_UNDEAD_GUNNER:
	    case PM_UNDEAD_OTAKU:
	    case PM_UNDEAD_ARTIST:
	    case PM_UNDEAD_GAMER:
	    case PM_UNDEAD_DOLL_MISTRESS:
	    case PM_UNDEAD_FEAT_MASTER:
	    case PM_UNDEAD_FOXHOUND_AGENT:
	    case PM_UNDEAD_INTEL_SCRIBE:
	    case PM_UNDEAD_PALADIN:
	    case PM_UNDEAD_WANDKEEPER:
	    case PM_UNDEAD_ASSASSIN:
	    case PM_UNDEAD_BULLY:
	    case PM_UNDEAD_SUPERMARKET_CASHIER:
	    case PM_UNDEAD_SLAVE_MASTER:
	    case PM_UNDEAD_MYSTIC:
	    case PM_UNDEAD_BLOODSEEKER:
	    case PM_UNDEAD_LADIESMAN:
	    case PM_UNDEAD_JESTER:
	    case PM_UNDEAD_PICKPOCKET:
	    case PM_UNDEAD_MAHOU_SHOUJO:
	    case PM_UNDEAD_DIVER:
	    case PM_UNDEAD_GLADIATOR:
	    case PM_UNDEAD_GOFF:
	    case PM_UNDEAD_AMAZON:
	    case PM_UNDEAD_ALTMER:
	    case PM_UNDEAD_BOSMER:
	    case PM_UNDEAD_DUNMER:
	    case PM_UNDEAD_THALMOR:
	    case PM_UNDEAD_ORDINATOR:
	    case PM_UNDEAD_PRIEST:
	    case PM_UNDEAD_PRIESTESS:
	    case PM_UNDEAD_RANGER:
	    case PM_UNDEAD_ELPH:
	    case PM_UNDEAD_ROGUE:
	    case PM_UNDEAD_SAMURAI:
	    case PM_UNDEAD_TOURIST:
	    case PM_UNDEAD_DRUNK:
	    case PM_UNDEAD_FIREFIGHTER:
	    case PM_UNDEAD_LOCKSMITH:
	    case PM_UNDEAD_MUSICIAN:
	    case PM_UNDEAD_NINJA:
	    case PM_UNDEAD_OFFICER:
	    case PM_UNDEAD_UNDERTAKER:
	    case PM_UNDEAD_ZOOKEEPER:
	    case PM_UNDEAD_TRANSVESTITE:
	    case PM_UNDEAD_TOPMODEL:
	    case PM_UNDEAD_ACTIVISTOR:
	    case PM_NON_UNDEAD_SLAYER:
	    case PM_UNDEAD_VALKYRIE:
	    case PM_UNDEAD_YEOMAN:
	    case PM_UNDEAD_PADAWAN:
	    case PM_UNDEAD_JEDI:
	    case PM_UNDEAD_WIZARD:
	    case PM_UNDEAD_MOLD:
	    case PM_FUNGOID_MOLD:
	    case PM_UNDEAD_FUNGUS:
	    case PM_MOLDY_FUNGUS:
	    case PM_MUTATED_UNDEAD_POTATO:
		obj = mkcorpstat(CORPSE, (struct monst *)0, &mons[mndx], x, y, TRUE);
		obj->age -= 100;                /* this is an *OLD* corpse */
		break;
	    case PM_MEDUSA: {
			struct monst *mtmp2;


			/* KMH -- the legend of Medusa and Pegasus */
			/* Only when Medusa leaves a corpse */
			mtmp2 = makemon(&mons[PM_PEGASUS], x, y, 0);
			if (mtmp2) {
				You("%s something spring forth from the corpse of %s.",
						Blind ? "sense" : "see", mon_nam(mtmp));
				mtmp2->mpeaceful = 1;
				mtmp2->mtame = 0;
			}
			goto default_1;
		}
	    case PM_NIGHTMARE:
			pline("All that remains is her horn...");
			obj = oname(mksobj(UNICORN_HORN, TRUE, FALSE),
					artiname(ART_NIGHTHORN));
			goto initspecial;
	    case PM_BEHOLDER:
			pline("All that remains is a single eye...");
			obj = oname(mksobj(EYEBALL, TRUE, FALSE),
					artiname(ART_EYE_OF_THE_BEHOLDER));
			goto initspecial;
	    case PM_VECNA:
			pline("All that remains is a hand...");
			obj = oname(mksobj(SEVERED_HAND, TRUE, FALSE),
					artiname(ART_HAND_OF_VECNA));
		initspecial:
			obj->quan = 1;
			curse(obj);
			place_object(obj, x, y);
			stackobj(obj);
			newsym(x, y);
			return obj;
		break;
	    case PM_IRON_GOLEM:
		num = d(2,6);
		while (num--)
			obj = mksobj_at(IRON_CHAIN, x, y, TRUE, FALSE);
		mtmp->mnamelth = 0;
		break;
	    case PM_GLASS_GOLEM:
		num = d(2,4);   /* very low chance of creating all glass gems */
		while (num--)
			obj = mksobj_at((LAST_GEM + rnd(9)), x, y, TRUE, FALSE);
		mtmp->mnamelth = 0;
		break;
	    case PM_RUBY_GOLEM:
		/* [DS] Mik's original Lethe fobbed off the player with coloured
		 * glass even for the higher golems. We'll play fair here - if
		 * you can kill one of these guys, you deserve the gems. */
		num = d(2,4);
		while (num--)
			obj = mksobj_at(RUBY, x, y, TRUE, FALSE);
		mtmp->mnamelth = 0;
		break;
	    case PM_DIAMOND_GOLEM:
		num = d(2,4);   
		while (num--)
			obj = mksobj_at(DIAMOND, x, y, TRUE, FALSE);
		mtmp->mnamelth = 0;
		break;
	    case PM_SAPPHIRE_GOLEM:
		num = d(2,4);
		while (num--)
			obj = mksobj_at(SAPPHIRE, x, y, TRUE, FALSE);
		mtmp->mnamelth = 0;
		break;
	    case PM_STEEL_GOLEM:
		num = d(2,6);
		/* [DS] Add steel chains (or handcuffs!) for steel golems? */
		while (num--)
			obj = mksobj_at(IRON_CHAIN, x, y, TRUE, FALSE);
		mtmp->mnamelth = 0;
		break;
	    case PM_CRYSTAL_GOLEM:
		/* [DS] Generate gemstones of various hues */
		num = d(2,4);
		{
		    int gemspan = LAST_GEM - bases[GEM_CLASS] + 1;
		    while (num--)
			obj = mksobj_at(bases[GEM_CLASS] + rn2(gemspan), x, y,
		    			TRUE, FALSE);
		    mtmp->mnamelth = 0;
		}
		break;
	    case PM_CLAY_GOLEM:
		obj = mksobj_at(ROCK, x, y, FALSE, FALSE);

		if(!rn2(8)) {
			obj->spe = rne(2);
			if (rn2(2)) obj->blessed = rn2(2);
			 else	blessorcurse(obj, 3);
		} else if(!rn2(10)) {
			if (rn2(10)) curse(obj);
			 else	blessorcurse(obj, 3);
			obj->spe = -rne(2);
		} else	blessorcurse(obj, 10);

		obj->quan = (long)(rn2(20) + 50);
		obj->owt = weight(obj);
		mtmp->mnamelth = 0;
		break;
	    case PM_STONE_GOLEM:
		obj = mkcorpstat(STATUE, (struct monst *)0,
			mdat, x, y, FALSE);
		break;
	    case PM_WOOD_GOLEM:
		num = d(2,4);
		while(num--)
			obj = mksobj_at(QUARTERSTAFF, x, y, TRUE, FALSE);
		mtmp->mnamelth = 0;
		break;
	    case PM_LEATHER_GOLEM:
		num = d(2,4);
		while(num--)
			obj = mksobj_at(LEATHER_ARMOR, x, y, TRUE, FALSE);
		mtmp->mnamelth = 0;
		break;
	    case PM_BUG:
	    	if (!rn2(6)) {
			mksobj_at(PACK_OF_FLOPPIES, x, y, TRUE, FALSE);
		}
		break;
	    case PM_HEISENBUG:
	    	if (!rn2(3)) {
			mksobj_at(PACK_OF_FLOPPIES, x, y, TRUE, FALSE);
		}
		break;
	    case PM_PDP___:	/* PDP-11 */
	    case PM_PDP__:	/* PDP-9 */
	    case PM_VAX:
	    case PM_CRAY:
	    	num = rn2(10) + 5;
		while (num--) {
			mksobj_at(!rn2(3) ? DIODE : (!rn2(2) ? TRANSISTOR : IC), x, y, TRUE, FALSE);
		}
		break;
	    case PM_ALGOLIAN_SUNTIGER:
		if (!rn2(75)) {
 			mksobj_at(TOOTH_OF_AN_ALGOLIAN_SUNTIGER,x, y, TRUE, FALSE);
		}
		goto default_1;
		case PM_WAX_GOLEM:
		num = d(2,4);
		while (num--)
			obj = mksobj_at(WAX_CANDLE, x, y, TRUE, FALSE);
		mtmp->mnamelth = 0;
		break;
	    case PM_PLASTIC_GOLEM:
		/* KMH -- Credit cards are #ifdef TOURIST */
		num = d(2,2);
		while (num--)
			obj = mksobj_at(
#ifdef TOURIST
				CREDIT_CARD,
#else
				FAKE_AMULET_OF_YENDOR,
#endif
					x, y, TRUE, FALSE);
		mtmp->mnamelth = 0;
		break;
	    case PM_GOLD_GOLEM:
	    case PM_GOLD_SLIME:
		/* Good luck gives more coins */
		obj = mkgold((long)(200 - rnl(101)), x, y);
		mtmp->mnamelth = 0;
		break;
	    case PM_INVULNERABLE_GOLEM:
		/* Good luck gives more coins */
		obj = mkgold((long)(rnz(1000) + 2000 - rnl(1001)), x, y);
		mtmp->mnamelth = 0;
		break;

	    case PM_PAPER_GOLEM:
		num = rnd(4);
		while (num--)
			obj = mksobj_at(SCR_BLANK_PAPER, x, y, TRUE, FALSE);
		mtmp->mnamelth = 0;
		break;
	    default_1:
	    default: /* worm that walks has a chance to get a corpse even from G_NOCORPSE monsters --Amy */
		if ((mvitals[mndx].mvflags & G_NOCORPSE) && (!Race_if(PM_WORM_THAT_WALKS) || !polyok(mdat) || rn2(5) ) )
		    return (struct obj *)0;
		else	/* preserve the unique traits of some creatures */
		    obj = mkcorpstat(CORPSE, KEEPTRAITS(mtmp) ? mtmp : 0,
				     mdat, x, y, TRUE);
		break;
	}

	if (!obj) return (struct obj *)0; /* really fixing the damn bug with the "bug" and similar monster types! --Amy */

	/* All special cases should precede the G_NOCORPSE check */

	/* if polymorph or undead turning has killed this monster,
	   prevent the same attack beam from hitting its corpse */
	if (flags.bypasses) bypass_obj(obj);

	if (mtmp->mnamelth)
	    obj = oname(obj, NAME(mtmp));

	/* Avoid "It was hidden under a green mold corpse!" 
	 *  during Blind combat. An unseen monster referred to as "it"
	 *  could be killed and leave a corpse.  If a hider then hid
	 *  underneath it, you could be told the corpse type of a
	 *  monster that you never knew was there without this.
	 *  The code in hitmu() substitutes the word "something"
	 *  if the corpses obj->dknown is 0.
	 */
	if (Blind && !sensemon(mtmp)) obj->dknown = 0;

#ifdef INVISIBLE_OBJECTS
	/* Invisible monster ==> invisible corpse */
	obj->oinvis = mtmp->perminvis;
#endif

	stackobj(obj);
	newsym(x, y);
	return obj;
}

#endif /* OVLB */
#ifdef OVL1

#if 0
/* part of the original warning code which was replaced in 3.3.1 */
STATIC_OVL void
warn_effects()
{
	register struct monst *mtmp;    
	int num_mon;
	int warned_of;

	num_mon = 0;
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (!canseemon(mtmp) && !(mtmp->mpeaceful) && !(mtmp->mtame)) {
			if (Warning && (!spec_ability(uwep,SPFX_WARN) || !spec_dbon(uwep,mtmp,1))) num_mon++;
			else if (Undead_warning && is_undead(mtmp->data)) num_mon++;
			else if (spec_ability(uwep,SPFX_WARN) && spec_dbon(uwep,mtmp,1)) num_mon++;
	    }
	}
	if (num_mon == 0) return;
	/* num_mon is the # of monsters which could be warned against...
	pick one at random... */
	warned_of = rn2(num_mon)+1;
	mtmp = fmon;
	num_mon = warned_of;
	do {
		if (!canseemon(mtmp) && !(mtmp->mpeaceful) && !(mtmp->mtame)) {
			if (Warning && (!spec_ability(uwep,SPFX_WARN) || !spec_dbon(uwep,mtmp,1))) num_mon--;
			else if (Undead_warning && is_undead(mtmp->data)) num_mon--;
			else if (spec_ability(uwep,SPFX_WARN) && spec_dbon(uwep,mtmp,1)) num_mon--;
		}
		if (num_mon > 0) mtmp = mtmp->nmon;
	} while (num_mon > 0);
	display_monster(mtmp->mx,mtmp->my,mtmp,1,0);
	lastwarnlev = warnlevel;
	lastwarntime = moves;
/*	warnlevel = 0;*/
	return;
}
#endif /* 0 */

/* check mtmp and water/lava for compatibility, 0 (survived), 1 (died) */
int
minliquid(mtmp)
register struct monst *mtmp;
{
    boolean inpool, inlava, infountain;

    inpool = is_pool(mtmp->mx,mtmp->my) &&
	     !is_flyer(mtmp->data) && !is_floater(mtmp->data);
    inlava = is_lava(mtmp->mx,mtmp->my) &&
	     !is_flyer(mtmp->data) && !is_floater(mtmp->data);
    infountain = IS_FOUNTAIN(levl[mtmp->mx][mtmp->my].typ);

#ifdef STEED
	/* Flying and levitation keeps our steed out of the liquid */
	/* (but not water-walking or swimming) */
	if (mtmp == u.usteed && (Flying || Levitation))
		return (0);
#endif

    /* Gremlin multiplying won't go on forever since the hit points
     * keep going down, and when it gets to 1 hit point the clone
     * function will fail.
     */
    if (mtmp->data == &mons[PM_GREMLIN] && (inpool || infountain) && !rn2(5)) { /* lowered chance --Amy */
	if (split_mon(mtmp, (struct monst *)0))
	    dryup(mtmp->mx, mtmp->my, FALSE);
	if (inpool) water_damage(mtmp->minvent, FALSE, FALSE);
	return (0);
    } else if (mtmp->data == &mons[PM_IRON_GOLEM] && inpool && !rn2(5)) {
	int dam = d(2,6);
	if (cansee(mtmp->mx,mtmp->my))
	    pline("%s rusts.", Monnam(mtmp));
	mtmp->mhp -= dam;
	if (mtmp->mhpmax > dam) mtmp->mhpmax -= dam;
	if (mtmp->mhp < 1) {
	    mondead(mtmp);
	    if (mtmp->mhp < 1) return (1);
	}
	water_damage(mtmp->minvent, FALSE, FALSE);
	return (0);
    }

    if (inlava) {
	/*
	 * Lava effects much as water effects. Lava likers are able to
	 * protect their stuff. Fire resistant monsters can only protect
	 * themselves  --ALI
	 */
	if (!is_clinger(mtmp->data) && !likes_lava(mtmp->data)) {
	    if (!resists_fire(mtmp)) {
		if (cansee(mtmp->mx,mtmp->my))
		    pline("%s %s.", Monnam(mtmp),
			  mtmp->data == &mons[PM_WATER_ELEMENTAL] ?
			  "boils away" : "burns to a crisp");
		if (!rn2(20)) mondead(mtmp); /* give them a chance to survive --Amy */
		else (void) rloc(mtmp, FALSE); /* unrealistic, I know, but I want the game to be as hard as possible. */
		/* And monsters dying instantly is not my idea of hard. After all, you can instakill Croesus that way:
		 * freeze the moat, wait for him to step on it and burn the ice away, which is way too easy! */
	    }
	    else {
		if (--mtmp->mhp < 1) {
		    if (cansee(mtmp->mx,mtmp->my))
			pline("%s surrenders to the fire.", Monnam(mtmp));
			if (!rn2(20)) mondead(mtmp);
			else (void) rloc(mtmp, FALSE);
		}
		else if (cansee(mtmp->mx,mtmp->my))
		    pline("%s burns slightly.", Monnam(mtmp));
	    }
	    if (mtmp->mhp > 0) {
		(void) fire_damage(mtmp->minvent, FALSE, FALSE,
						mtmp->mx, mtmp->my);
		(void) rloc(mtmp, FALSE);
		return 0;
	    }
	    return (1);
	}
    } else if (inpool) {
	/* Most monsters drown in pools.  flooreffects() will take care of
	 * water damage to dead monsters' inventory, but survivors need to
	 * be handled here.  Swimmers are able to protect their stuff...
	 */
	if (!is_clinger(mtmp->data)
	    && !is_swimmer(mtmp->data) && !amphibious(mtmp->data)) {
	    if (cansee(mtmp->mx,mtmp->my)) {
		    pline("%s drowns.", Monnam(mtmp));
	    }
	    if (u.ustuck && u.uswallow && u.ustuck == mtmp) {
	    /* This can happen after a purple worm plucks you off a
		flying steed while you are over water. */
		pline("%s sinks as water rushes in and flushes you out.",
			Monnam(mtmp));
	    }
		if (!rn2(20)) mondead(mtmp); /* see above for the reason why monsters survive here --Amy */
		else (void) rloc(mtmp, FALSE);
	    if (mtmp->mhp > 0) {
		(void) rloc(mtmp, FALSE);
		water_damage(mtmp->minvent, FALSE, FALSE);
		return 0;
	    }
	    return (1);
	}
    } /*else {*/
	/* but eels have a difficult time outside */
	/*if (mtmp->data->mlet == S_EEL && !Is_waterlevel(&u.uz)) {
	    if(mtmp->mhp > 1) mtmp->mhp--;
	    monflee(mtmp, 2, FALSE, FALSE);
	}*/

	/* commented out - would just make the game easier, and we don't want that --Amy */
    /*}*/
    return (0);
}

int
mcalcmove(mon)
struct monst *mon;
{
    int mmove = mon->data->mmove;

    /* Note: MSLOW's `+ 1' prevents slowed speed 1 getting reduced to 0;
     *	     MFAST's `+ 2' prevents hasted speed 1 from becoming a no-op;
     *	     both adjustments have negligible effect on higher speeds.
     */
    if (mon->mspeed == MSLOW)
	mmove = (2 * mmove + 1) / 3;
    else if (mon->mspeed == MFAST)
	mmove = (4 * mmove + 2) / 3;

#ifdef STEED
    if (mon == u.usteed) {
	if (u.ugallop && flags.mv) {
	    /* average movement is 1.50 times normal */
	    mmove = ((rn2(2) ? 4 : 5) * mmove) / 3;
	}
    }
#endif

    return mmove;
}

/* actions that happen once per ``turn'', regardless of each
   individual monster's metabolism; some of these might need to
   be reclassified to occur more in proportion with movement rate */
void
mcalcdistress()
{
    struct monst *mtmp;

    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	if (DEADMONSTER(mtmp)) continue;

	/* must check non-moving monsters once/turn in case
	 * they managed to end up in liquid */
	if (mtmp->data->mmove == 0) {
	    if (vision_full_recalc) vision_recalc(0);
	    if (minliquid(mtmp)) continue;
	}

	/* regenerate hit points */
	mon_regen(mtmp, FALSE);

	/* possibly polymorph shapechangers and lycanthropes */
	if (mtmp->cham && !rn2(6))
	    (void) mon_spec_poly(mtmp, (struct permonst *)0, 0L, FALSE,
		    cansee(mtmp->mx,mtmp->my) && flags.verbose, FALSE, FALSE);
	were_change(mtmp);

	/* gradually time out temporary problems */
	if (mtmp->mblinded && !--mtmp->mblinded)
	    mtmp->mcansee = 1;
	if (mtmp->mfrozen && !--mtmp->mfrozen)
	    mtmp->mcanmove = 1;
	if (mtmp->mfleetim && !--mtmp->mfleetim)
	    mtmp->mflee = 0;

	/* FIXME: mtmp->mlstmv ought to be updated here */
    }
}

int
movemon()
{
    register struct monst *mtmp, *nmtmp;
    register boolean somebody_can_move = FALSE;

#if 0
    /* part of the original warning code which was replaced in 3.3.1 */
    warnlevel = 0;
#endif

    /*
    Some of you may remember the former assertion here that
    because of deaths and other actions, a simple one-pass
    algorithm wasn't possible for movemon.  Deaths are no longer
    removed to the separate list fdmon; they are simply left in
    the chain with hit points <= 0, to be cleaned up at the end
    of the pass.

    The only other actions which cause monsters to be removed from
    the chain are level migrations and losedogs().  I believe losedogs()
    is a cleanup routine not associated with monster movements, and
    monsters can only affect level migrations on themselves, not others
    (hence the fetching of nmon before moving the monster).  Currently,
    monsters can jump into traps, read cursed scrolls of teleportation,
    and drink cursed potions of raise level to change levels.  These are
    all reflexive at this point.  Should one monster be able to level
    teleport another, this scheme would have problems.
    */

    for(mtmp = fmon; mtmp; mtmp = nmtmp) {
	nmtmp = mtmp->nmon;

	/* Find a monster that we have not treated yet.	 */
	if(DEADMONSTER(mtmp))
	    continue;
	if(mtmp->movement < NORMAL_SPEED)
	    continue;

	mtmp->movement -= NORMAL_SPEED;
	if (mtmp->movement >= NORMAL_SPEED)
	    somebody_can_move = TRUE;

	if (vision_full_recalc) vision_recalc(0);	/* vision! */

	if (minliquid(mtmp)) continue;

	if (is_hider(mtmp->data)) {
	    /* unwatched mimics and piercers may hide again  [MRS] */
	    if(restrap(mtmp))   continue;
	    if(mtmp->m_ap_type == M_AP_FURNITURE ||
				mtmp->m_ap_type == M_AP_OBJECT)
		    continue;
	    if(mtmp->mundetected) continue;
	}

	/* continue if the monster died fighting */
	if (Conflict && !mtmp->iswiz && mtmp->mcansee) {
	    /* Note:
	     *  Conflict does not take effect in the first round.
	     *  Therefore, A monster when stepping into the area will
	     *  get to swing at you.
	     *
	     *  The call to fightm() must be _last_.  The monster might
	     *  have died if it returns 1.
	     */
	    if (couldsee(mtmp->mx,mtmp->my) &&
		(distu(mtmp->mx,mtmp->my) <= /*BOLT_LIM*BOLT_LIM*/ 4) &&
							fightm(mtmp))
		continue;	/* mon might have died */
	}
	if(dochugw(mtmp))		/* otherwise just move the monster */
	    continue;
    }
#if 0
    /* part of the original warning code which was replaced in 3.3.1 */
    if(warnlevel > 0)
	warn_effects();
#endif
    if (any_light_source())
	vision_full_recalc = 1;	/* in case a mon moved with a light source */
    dmonsfree();	/* remove all dead monsters */

    /* a monster may have levteleported player -dlc */
    if (u.utotype) {
	deferred_goto();
	/* changed levels, so these monsters are dormant */
	somebody_can_move = FALSE;
    }

    return somebody_can_move;
}

#endif /* OVL1 */
#ifdef OVLB

#define mstoning(obj)	(ofood(obj) && \
					(touch_petrifies(&mons[(obj)->corpsenm]) || \
					(obj)->corpsenm == PM_MEDUSA))

/*
 * Maybe eat a metallic object (not just gold).
 * Return value: 0 => nothing happened, 1 => monster ate something,
 * 2 => monster died (it must have grown into a genocided form, but
 * that can't happen at present because nothing which eats objects
 * has young and old forms).
 */
int
meatmetal(mtmp)
	register struct monst *mtmp;
{
	register struct obj *otmp;
	struct permonst *ptr;
	int poly, grow, heal, mstone;

	/* If a pet, eating is handled separately, in dog.c */
	if (mtmp->mtame) return 0;

	/* Eats topmost metal object if it is there */
	for (otmp = level.objects[mtmp->mx][mtmp->my];
						otmp; otmp = otmp->nexthere) {
	    if (mtmp->data == &mons[PM_RUST_MONSTER] && !is_rustprone(otmp))
		continue;
	    if (is_metallic(otmp) && !obj_resists(otmp, 5, 95) &&
		touch_artifact(otmp,mtmp)) {
		if (mtmp->data == &mons[PM_RUST_MONSTER] && otmp->oerodeproof) {
		    if (cansee(mtmp->mx,mtmp->my) && flags.verbose) {
			pline("%s eats %s!",
				Monnam(mtmp),
				distant_name(otmp,doname));
		    }
		    /* The object's rustproofing is gone now */
		    otmp->oerodeproof = 0;
		    mtmp->mstun = 1;
		    if (canseemon(mtmp) && flags.verbose) {
			pline("%s spits %s out in disgust!",
			      Monnam(mtmp), distant_name(otmp,doname));
		    }
		/* KMH -- Don't eat indigestible/choking objects */
		} else if (otmp->otyp != AMULET_OF_STRANGULATION &&
				otmp->otyp != RIN_SLOW_DIGESTION) {
		    if (cansee(mtmp->mx,mtmp->my) && flags.verbose)
			pline("%s eats %s!", Monnam(mtmp),
				distant_name(otmp,doname));
		    else if (flags.soundok && flags.verbose)
			You_hear("a crunching sound.");
		    mtmp->meating = otmp->owt/2 + 1;
		    /* Heal up to the object's weight in hp */
		    if (mtmp->mhp < mtmp->mhpmax) {
			mtmp->mhp += objects[otmp->otyp].oc_weight;
			if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
		    }
		    if(otmp == uball) {
			unpunish();
			delobj(otmp);
		    } else if (otmp == uchain) {
			unpunish();	/* frees uchain */
		    } else {
			poly = polyfodder(otmp);
			grow = mlevelgain(otmp);
			heal = mhealup(otmp);
			mstone = mstoning(otmp);
			delobj(otmp);
			ptr = mtmp->data;
			if (poly) {
			    if (mon_spec_poly(mtmp,
				    (struct permonst *)0, 0L, FALSE,
				    cansee(mtmp->mx,mtmp->my) && flags.verbose,
				    FALSE, FALSE))
				ptr = mtmp->data;
			} else if (grow) {
			    ptr = grow_up(mtmp, (struct monst *)0);
			} else if (mstone && !rn2(4) ) {
			    if (poly_when_stoned(ptr)) {
				mon_to_stone(mtmp);
				ptr = mtmp->data;
			    } else if (!resists_ston(mtmp)) {
				if (canseemon(mtmp))
				    pline("%s turns to stone!", Monnam(mtmp));
				monstone(mtmp);
				ptr = (struct permonst *)0;
			    }
			} else if (heal) {
			    mtmp->mhp = mtmp->mhpmax;
			}
			if (!ptr) return 2;		 /* it died */
		    }
		    /* Left behind a pile? */
		    if (rnd(25) < 3)
			(void) mksobj_at(ROCK, mtmp->mx, mtmp->my, TRUE, FALSE);
		    newsym(mtmp->mx, mtmp->my);
		    return 1;
		}
	    }
	}
	return 0;
}

/*
 * Maybe eat a lithic object.
 * Return value: 0 => nothing happened, 1 => monster ate something,
 * 2 => monster died (it must have grown into a genocided form, but
 * that can't happen at present because nothing which eats objects
 * has young and old forms).
 */
int
meatlithic(mtmp)
	register struct monst *mtmp;
{
	register struct obj *otmp;
	struct permonst *ptr;
	int poly, grow, heal, mstone;

	/* If a pet, eating is handled separately, in dog.c */
	if (mtmp->mtame) return 0;

	/* Eats topmost lithic object if it is there */
	for (otmp = level.objects[mtmp->mx][mtmp->my];
						otmp; otmp = otmp->nexthere) { 

	    if (is_lithic(otmp) && !obj_resists(otmp, 5, 95) &&
		touch_artifact(otmp,mtmp)) {
			if (otmp->otyp != AMULET_OF_STRANGULATION &&
				otmp->otyp != RIN_SLOW_DIGESTION) {
		    if (cansee(mtmp->mx,mtmp->my) && flags.verbose)
			pline("%s eats %s!", Monnam(mtmp),
				distant_name(otmp,doname));
		    else if (flags.soundok && flags.verbose)
			You_hear("a grating sound.");
		    mtmp->meating = otmp->owt/2 + 1;
		    /* Heal up to the object's weight in hp */
		    if (mtmp->mhp < mtmp->mhpmax) {
			mtmp->mhp += objects[otmp->otyp].oc_weight;
			if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
		    }
		    if(otmp == uball) {
			unpunish();
			delobj(otmp);
		    } else if (otmp == uchain) {
			unpunish();	/* frees uchain */
		    } else {
			poly = polyfodder(otmp);
			grow = mlevelgain(otmp);
			heal = mhealup(otmp);
			mstone = mstoning(otmp);
			delobj(otmp);
			ptr = mtmp->data;
			if (poly) {
			    if (mon_spec_poly(mtmp,
				    (struct permonst *)0, 0L, FALSE,
				    cansee(mtmp->mx,mtmp->my) && flags.verbose,
				    FALSE, FALSE))
				ptr = mtmp->data;
			} else if (grow) {
			    ptr = grow_up(mtmp, (struct monst *)0);
			} else if (mstone && !rn2(4) ) {
			    if (poly_when_stoned(ptr)) {
				mon_to_stone(mtmp);
				ptr = mtmp->data;
			    } else if (!resists_ston(mtmp)) {
				if (canseemon(mtmp))
				    pline("%s turns to stone!", Monnam(mtmp));
				monstone(mtmp);
				ptr = (struct permonst *)0;
			    }
			} else if (heal) {
			    mtmp->mhp = mtmp->mhpmax;
			}
			if (!ptr) return 2;		 /* it died */
		    }
		    newsym(mtmp->mx, mtmp->my);
		    return 1;
		}
	    }
	}
	return 0;
}

void
meatcorpse(mtmp)
	register struct monst *mtmp;
{
	register struct obj *otmp, *otmpB;
 
	/* If a pet, eating is handled separately, in dog.c */
	if (mtmp->mtame) return;
  
	/* Eats topmost corpse if it is there */
	for (otmp = level.objects[mtmp->mx][mtmp->my];
						    otmp; otmp = otmp->nexthere)
	       if (otmp->otyp == CORPSE &&
		  otmp->age+50 <= monstermoves) {
		    if (cansee(mtmp->mx,mtmp->my) && flags.verbose)
			pline("%s eats %s!", Monnam(mtmp),
				distant_name(otmp,doname));
		    else if (flags.soundok && flags.verbose)
			You(Hallucination ? "hear an alien's noises!" : "hear an awful gobbling noise!");
		    mtmp->meating = 2;
		    delobj(otmp);

			if (mtmp->data == &mons[PM_CORPULENT_DOG] || mtmp->data == &mons[PM_THICK_POTATO] || mtmp->data == &mons[PM_BLACK_MUZZLE] || mtmp->data == &mons[PM_CORPSE_SPITTER] || mtmp->data == &mons[PM_MUZZLE_FIEND] || mtmp->data == &mons[PM_MAW_FIEND] || mtmp->data == &mons[PM_ROCKET_MUZZLE]) {

	otmpB = mksobj(ROCKET, TRUE, FALSE);
	otmpB->quan = 1;
	otmpB->owt = weight(otmpB);
	(void) mpickobj(mtmp, otmpB);

			}

			if (mtmp->data == &mons[PM_SPIT_DEMON]) {

	otmpB = mksobj(ROCK, TRUE, FALSE);
	otmpB->quan = 5;
	otmpB->owt = weight(otmpB);
	(void) mpickobj(mtmp, otmpB);

			}



		    break; /* only eat one at a time... */
		  }
      newsym(mtmp->mx, mtmp->my);
}
 
int
meatobj(mtmp)		/* for gelatinous cubes */
	register struct monst *mtmp;
{
	register struct obj *otmp, *otmp2;
	struct permonst *ptr;
	int poly, grow, heal, count = 0, ecount = 0;
	char buf[BUFSZ];

	buf[0] = '\0';
	/* If a pet, eating is handled separately, in dog.c */
	if (mtmp->mtame) return 0;

	/* Eats organic objects, including cloth and wood, if there */
	/* Engulfs others, except huge rocks and metal attached to player */
	for (otmp = level.objects[mtmp->mx][mtmp->my]; otmp; otmp = otmp2) {
	    otmp2 = otmp->nexthere;
	    if (is_organic(otmp) && !obj_resists(otmp, 5, 95) &&
		    touch_artifact(otmp,mtmp)) {
		if (otmp->otyp == CORPSE && touch_petrifies(&mons[otmp->corpsenm]) &&
			!resists_ston(mtmp))
		    continue;
		if (otmp->otyp == AMULET_OF_STRANGULATION ||
				otmp->otyp == RIN_SLOW_DIGESTION)
		    continue;
		++count;
		if (cansee(mtmp->mx,mtmp->my) && flags.verbose)
		    pline("%s eats %s!", Monnam(mtmp),
			    distant_name(otmp, doname));
		else if (flags.soundok && flags.verbose)
		    You_hear("a slurping sound.");
		/* Heal up to the object's weight in hp */
		if (mtmp->mhp < mtmp->mhpmax) {
		    mtmp->mhp += objects[otmp->otyp].oc_weight;
		    if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
		}
		if (otmp->otyp == MEDICAL_KIT)
		    delete_contents(otmp);
		if (Has_contents(otmp)) {
		    register struct obj *otmp3;
		    /* contents of eaten containers become engulfed; this
		       is arbitrary, but otherwise g.cubes are too powerful */
		    while ((otmp3 = otmp->cobj) != 0) {
			obj_extract_self(otmp3);
			if (otmp->otyp == ICE_BOX && otmp3->otyp == CORPSE) {
			    otmp3->age = monstermoves - otmp3->age;
			    start_corpse_timeout(otmp3);
			}
			(void) mpickobj(mtmp, otmp3);
		    }
		}
		poly = polyfodder(otmp);
		grow = mlevelgain(otmp);
		heal = mhealup(otmp);
		delobj(otmp);		/* munch */
		ptr = mtmp->data;
		if (poly) {
		    if (mon_spec_poly(mtmp, (struct permonst *)0, 0L, FALSE,
			    cansee(mtmp->mx,mtmp->my) && flags.verbose,
			    FALSE, FALSE))
			ptr = mtmp->data;
		} else if (grow) {
		    ptr = grow_up(mtmp, (struct monst *)0);
		} else if (heal) {
		    mtmp->mhp = mtmp->mhpmax;
		}
		/* in case it polymorphed or died */
		if (ptr != &mons[PM_GELATINOUS_CUBE])
		    return !ptr ? 2 : 1;
	    } else if (otmp->oclass != ROCK_CLASS &&
				    otmp != uball && otmp != uchain) {
		++ecount;
		if (ecount == 1) {
			Sprintf(buf, "%s engulfs %s.", Monnam(mtmp),
			    distant_name(otmp,doname));
		} else if (ecount == 2)
			Sprintf(buf, "%s engulfs several objects.", Monnam(mtmp));
		obj_extract_self(otmp);
		(void) mpickobj(mtmp, otmp);	/* slurp */
	    }
	    /* Engulf & devour is instant, so don't set meating */
	    if (mtmp->minvis) newsym(mtmp->mx, mtmp->my);
	}
	if (ecount > 0) {
	    if (cansee(mtmp->mx, mtmp->my) && flags.verbose && buf[0])
		pline("%s", buf);
	    else if (flags.soundok && flags.verbose)
	    	You_hear("%s slurping sound%s.",
			ecount == 1 ? "a" : "several",
			ecount == 1 ? "" : "s");
	}
	return ((count > 0) || (ecount > 0)) ? 1 : 0;
}

void
mpickgold(mtmp)
	register struct monst *mtmp;
{
    register struct obj *gold;
    int mat_idx;

    if ((gold = g_at(mtmp->mx, mtmp->my)) != 0) {
	mat_idx = objects[gold->otyp].oc_material;
#ifndef GOLDOBJ
	mtmp->mgold += gold->quan;
	delobj(gold);
#else
        obj_extract_self(gold);
        add_to_minv(mtmp, gold);
#endif
	if (cansee(mtmp->mx, mtmp->my) ) {
	    if (flags.verbose && !mtmp->isgd)
		pline("%s picks up some %s.", Monnam(mtmp),
			mat_idx == GOLD ? "gold" : "money");
	    newsym(mtmp->mx, mtmp->my);
	}
    }
}
#endif /* OVLB */
#ifdef OVL2

boolean
mpickstuff(mtmp, str)
	register struct monst *mtmp;
	register const char *str;
{
	register struct obj *otmp, *otmp2;

/*	prevent shopkeepers from leaving the door of their shop */
	if(mtmp->isshk && inhishop(mtmp)) return FALSE;

	for(otmp = level.objects[mtmp->mx][mtmp->my]; otmp; otmp = otmp2) {
	    otmp2 = otmp->nexthere;
/*	Nymphs take everything.  Most monsters don't pick up corpses. */
	    if (!str ? searches_for_item(mtmp,otmp) :
		  !!(index(str, otmp->oclass))) {
		if (otmp->otyp == CORPSE && mtmp->data->mlet != S_NYMPH &&
			/* let a handful of corpse types thru to can_carry() */
			!touch_petrifies(&mons[otmp->corpsenm]) &&
			otmp->corpsenm != PM_LIZARD &&
			otmp->corpsenm != PM_CAVE_LIZARD &&
			otmp->corpsenm != PM_CHAOS_LIZARD &&
			otmp->corpsenm != PM_LIZARD_EEL &&
			otmp->corpsenm != PM_LIZARD_MAN &&
			otmp->corpsenm != PM_LIZARD_KING &&
			otmp->corpsenm != PM_EEL_LIZARD &&
			otmp->corpsenm != PM_ANTI_STONE_LIZARD &&
			otmp->corpsenm != PM_HUGE_LIZARD &&
			otmp->corpsenm != PM_ROCK_LIZARD &&
			otmp->corpsenm != PM_NIGHT_LIZARD &&
			otmp->corpsenm != PM_SAND_TIDE &&
			otmp->corpsenm != PM_FBI_AGENT &&
			otmp->corpsenm != PM_OWN_SMOKE &&
			otmp->corpsenm != PM_GRANDPA &&
			otmp->corpsenm != PM_HIDDEN_LIZARD &&
			otmp->corpsenm != PM_DEFORMED_LIZARD &&
			otmp->corpsenm != PM_CLINGING_LIZARD &&
			otmp->corpsenm != PM_MIMIC_LIZARD &&
			otmp->corpsenm != PM_KARMIC_LIZARD &&
			otmp->corpsenm != PM_MONSTER_LIZARD &&
			otmp->corpsenm != PM_ICE_LIZARD &&
			otmp->corpsenm != PM_FIRE_LIZARD &&
			otmp->corpsenm != PM_LIGHTNING_LIZARD &&
			otmp->corpsenm != PM_GIANT_LIZARD &&
			!acidic(&mons[otmp->corpsenm])) continue;
		if (!touch_artifact(otmp,mtmp)) continue;
		if (!can_carry(mtmp,otmp)) continue;
		if (is_pool(mtmp->mx,mtmp->my)) continue;
#ifdef INVISIBLE_OBJECTS
		if (otmp->oinvis && !perceives(mtmp->data)) continue;
#endif
		if (cansee(mtmp->mx,mtmp->my) && flags.verbose)
			pline("%s picks up %s.", Monnam(mtmp),
			      (distu(mtmp->mx, mtmp->my) <= 5) ?
				doname(otmp) : distant_name(otmp, doname));
		obj_extract_self(otmp);
		/* unblock point after extract, before pickup */
		if (otmp->otyp == BOULDER)
		    unblock_point(otmp->ox,otmp->oy);	/* vision */
		(void) mpickobj(mtmp, otmp);	/* may merge and free otmp */
		m_dowear(mtmp, FALSE);
		newsym(mtmp->mx, mtmp->my);
		return TRUE;			/* pick only one object */
	    }
	}
	return FALSE;
}

#endif /* OVL2 */
#ifdef OVL0

int
curr_mon_load(mtmp)
register struct monst *mtmp;
{
	register int curload = 0;
	register struct obj *obj;

	for(obj = mtmp->minvent; obj; obj = obj->nobj) {
		if(obj->otyp != BOULDER || !throws_rocks(mtmp->data))
			curload += obj->owt;
	}

	return curload;
}

int
max_mon_load(mtmp)
register struct monst *mtmp;
{
	register long maxload;

	/* Base monster carrying capacity is equal to human maximum
	 * carrying capacity, or half human maximum if not strong.
	 * (for a polymorphed player, the value used would be the
	 * non-polymorphed carrying capacity instead of max/half max).
	 * This is then modified by the ratio between the monster weights
	 * and human weights.  Corpseless monsters are given a capacity
	 * proportional to their size instead of weight.
	 */
	if (!mtmp->data->cwt)
		maxload = (MAX_CARR_CAP * (long)mtmp->data->msize) / MZ_HUMAN;
	else if (!strongmonst(mtmp->data)
		|| (strongmonst(mtmp->data) && (mtmp->data->cwt > WT_HUMAN)))
		maxload = (MAX_CARR_CAP * (long)mtmp->data->cwt) / WT_HUMAN;
	else	maxload = MAX_CARR_CAP; /*strong monsters w/cwt <= WT_HUMAN*/

	if (!strongmonst(mtmp->data)) maxload /= 2;

	if (maxload < 200) maxload = 200;

	return (int) maxload;
}

/* for restricting monsters' object-pickup */
boolean
can_carry(mtmp,otmp)
struct monst *mtmp;
struct obj *otmp;
{
	int otyp = otmp->otyp, newload = otmp->owt;
	struct permonst *mdat = mtmp->data;

	if (notake(mdat)) return FALSE;		/* can't carry anything */

	if (otyp == CORPSE && touch_petrifies(&mons[otmp->corpsenm]) &&
		!(mtmp->misc_worn_check & W_ARMG) && !resists_ston(mtmp))
	    return FALSE;
	if (otyp == CORPSE && is_rider(&mons[otmp->corpsenm]))
	    return FALSE;
	if (objects[otyp].oc_material == SILVER && hates_silver(mdat) &&
		(otyp != BELL_OF_OPENING || !is_covetous(mdat)))
	    return FALSE;

#ifdef STEED
	/* Steeds don't pick up stuff (to avoid shop abuse) */
	if (mtmp == u.usteed) return (FALSE);
#endif
	if (mtmp->isshk) return(TRUE); /* no limit */
	if (mtmp->mpeaceful && !mtmp->mtame) return(FALSE);
	/* otherwise players might find themselves obligated to violate
	 * their alignment if the monster takes something they need
	 */

	/* special--boulder throwers carry unlimited amounts of boulders */
	if (throws_rocks(mdat) && otyp == BOULDER)
		return(TRUE);

	/* nymphs deal in stolen merchandise, but not boulders or statues */
	if (mdat->mlet == S_NYMPH)
		return (boolean)(otmp->oclass != ROCK_CLASS);

	if (curr_mon_load(mtmp) + newload > max_mon_load(mtmp)) return FALSE;

	/* if the monster hates silver,  don't pick it up */
	if (objects[otmp->otyp].oc_material == SILVER && hates_silver(mtmp->data)) 
		return(FALSE);

	if(curr_mon_load(mtmp) + newload > max_mon_load(mtmp)) return(FALSE);

	return(TRUE);
}

/* return number of acceptable neighbour positions */
int
mfndpos(mon, poss, info, flag)
	register struct monst *mon;
	coord *poss;	/* coord poss[9] */
	long *info;	/* long info[9] */
	long flag;
{
	struct permonst *mdat = mon->data;
	register xchar x,y,nx,ny;
	register int cnt = 0;
	register uchar ntyp;
	uchar nowtyp;
	boolean wantpool,poolok,wantlava,lavaok,nodiag;
	boolean rockok = FALSE, treeok = FALSE, thrudoor;
	int maxx, maxy;

	x = mon->mx;
	y = mon->my;
	nowtyp = levl[x][y].typ;

	nodiag = (mdat == &mons[PM_GRID_BUG] || mdat == &mons[PM_GRID_XORN]);
	wantpool = mdat->mlet == S_EEL || mdat->mlet == S_FLYFISH || mdat == &mons[PM_HUMAN_WEREPIRANHA] || mdat == &mons[PM_HUMAN_WEREEEL] || mdat == &mons[PM_HUMAN_WEREKRAKEN] || mdat == &mons[PM_HUMAN_WEREFLYFISH] || mdat == &mons[PM_CONCORDE__] || mdat == &mons[PM_SWIMMER_TROLL] || mdat == &mons[PM_MISTER_SUBMARINE] || mdat == &mons[PM_WATER_TURRET] || mdat == &mons[PM_AQUA_TURRET] || mdat == &mons[PM_DIVER_TROLL] || mdat == &mons[PM_PUNT] || mdat == &mons[PM_LUXURY_YACHT] || mdat == &mons[PM_SUBMARINE_GOBLIN] ;
	poolok = (is_flyer(mdat) || is_clinger(mdat) ||
		 (is_swimmer(mdat) && !wantpool)) && !(mdat->mlet == S_FLYFISH || mdat == &mons[PM_HUMAN_WEREFLYFISH] || mdat == &mons[PM_CONCORDE__]);
	wantlava = (mdat->mlet == S_FLYFISH || mdat == &mons[PM_HUMAN_WEREFLYFISH] || mdat == &mons[PM_CONCORDE__]);
	lavaok = is_flyer(mdat) || is_clinger(mdat) || (likes_lava(mdat) && !wantlava);
	thrudoor = ((flag & (ALLOW_WALL|BUSTDOOR)) != 0L);
	treeok = is_flyer(mdat); /* flying monsters, but not flying players, can pass over trees --Amy */
	if (flag & ALLOW_DIG) {
	    struct obj *mw_tmp;

	    /* need to be specific about what can currently be dug */
	    if (!needspick(mdat)) {
		rockok = treeok = TRUE;
	    } else if ((mw_tmp = MON_WEP(mon)) && mw_tmp->cursed &&
		       mon->weapon_check == NO_WEAPON_WANTED) {
		rockok = is_pick(mw_tmp);
	    } else {
		rockok = (m_carrying(mon, PICK_AXE) ||
			  (m_carrying(mon, DWARVISH_MATTOCK) &&
			   !which_armor(mon, W_ARMS)));
		treeok = (m_carrying(mon, AXE) ||
			  (m_carrying(mon, BATTLE_AXE) &&
			   !which_armor(mon, W_ARMS)));
	    }
	    thrudoor |= rockok || treeok;
	}
	if (is_flyer(mdat)) treeok = TRUE; /* fail safe */

nexttry:	/* eels prefer the water, but if there is no water nearby,
		   they will crawl over land */
	if(mon->mconf) {
		flag |= ALLOW_ALL;
		flag &= ~NOTONL;
	}
	if(!mon->mcansee)
		flag |= ALLOW_SSM;
	maxx = min(x+1,COLNO-1);
	maxy = min(y+1,ROWNO-1);
	for(nx = max(1,x-1); nx <= maxx; nx++)
	  for(ny = max(0,y-1); ny <= maxy; ny++) {
	    if(nx == x && ny == y) continue;
	    if(IS_ROCK(ntyp = levl[nx][ny].typ) &&
	       !((flag & ALLOW_WALL) && may_passwall(nx,ny)) &&
	       !((IS_TREE(ntyp) ? treeok : rockok) && may_dig(nx,ny))) continue;
	    /* KMH -- Added iron bars */
	    if (ntyp == IRONBARS && !(flag & ALLOW_BARS)) continue;
	    /* ALI -- Artifact doors (no passage unless open/openable) */
	    if (IS_DOOR(ntyp) && rn2(200) ) 	/* let's allow monsters to bypass locked doors sometimes. --Amy */
		if (artifact_door(nx, ny) ?
		    levl[nx][ny].doormask & D_CLOSED && !(flag & OPENDOOR)
		      || levl[nx][ny].doormask & D_LOCKED :
		    !amorphous(mdat) &&
	       ((levl[nx][ny].doormask & D_CLOSED && !(flag & OPENDOOR)) ||
		(levl[nx][ny].doormask & D_LOCKED && !(flag & UNLOCKDOOR))) &&
	       !thrudoor) continue;
	    if(nx != x && ny != y && (nodiag ||
#ifdef REINCARNATION
	       ((IS_DOOR(nowtyp) &&
		 ((levl[x][y].doormask & ~D_BROKEN) || Is_rogue_level(&u.uz))) ||
		(IS_DOOR(ntyp) &&
		 ((levl[nx][ny].doormask & ~D_BROKEN) || Is_rogue_level(&u.uz))))
#else
	       ((IS_DOOR(nowtyp) && (levl[x][y].doormask & ~D_BROKEN)) ||
		(IS_DOOR(ntyp) && (levl[nx][ny].doormask & ~D_BROKEN)))
#endif
	       ))
		continue;
	    /*if((is_pool(nx,ny) == wantpool || is_lava(nx,ny) == wantlava || poolok) &&
	       ( lavaok || wantlava || !is_lava(nx,ny))) {*/
	    if((is_pool(nx,ny) == wantpool || (is_lava(nx,ny) && wantlava) || poolok) &&
	       (lavaok || wantlava || !is_lava(nx,ny))) {
		int dispx, dispy;
		boolean monseeu = (mon->mcansee && (!Invis || perceives(mdat)));
		boolean checkobj = OBJ_AT(nx,ny);

		/* Displacement also displaces the Elbereth/scare monster,
		 * as long as you are visible.
		 */
		if(Displaced && monseeu && (mon->mux==nx) && (mon->muy==ny)) {
		    dispx = u.ux;
		    dispy = u.uy;
		} else {
		    dispx = nx;
		    dispy = ny;
		}

		info[cnt] = 0;
		if ((checkobj || Displaced) && onscary(dispx, dispy, mon)) {
		    if(!(flag & ALLOW_SSM) && !rn2(3)) continue;
		    info[cnt] |= ALLOW_SSM;
		}
		if((nx == u.ux && ny == u.uy) ||
		   (nx == mon->mux && ny == mon->muy)) {
			if (nx == u.ux && ny == u.uy) {
				/* If it's right next to you, it found you,
				 * displaced or no.  We must set mux and muy
				 * right now, so when we return we can tell
				 * that the ALLOW_U means to attack _you_ and
				 * not the image.
				 */
				mon->mux = u.ux;
				mon->muy = u.uy;
			}
			if(!(flag & ALLOW_U)) continue;
			info[cnt] |= ALLOW_U;
		} else {
			if(MON_AT(nx, ny)) {
				struct monst *mtmp2 = m_at(nx, ny);
				long mmflag = flag | mm_aggression(mon, mtmp2);

				if (!(mmflag & ALLOW_M)) continue;
				info[cnt] |= ALLOW_M;
				if (mtmp2->mtame) {
					if (!(mmflag & ALLOW_TM)) continue;
					info[cnt] |= ALLOW_TM;
				}
			}
			/* Note: ALLOW_SANCT only prevents movement, not */
			/* attack, into a temple. */
			if(level.flags.has_temple &&
			   *in_rooms(nx, ny, TEMPLE) &&
			   !*in_rooms(x, y, TEMPLE) &&
			   in_your_sanctuary((struct monst *)0, nx, ny)) {
				if(!(flag & ALLOW_SANCT) && !rn2(5) ) continue;
				info[cnt] |= ALLOW_SANCT;
			}
		}
		if(checkobj && sobj_at(CLOVE_OF_GARLIC, nx, ny)) {
			if(flag & NOGARLIC) continue;
			info[cnt] |= NOGARLIC;
		}
		if(checkobj && sobj_at(BOULDER, nx, ny)) {
			if(!(flag & ALLOW_ROCK)) continue;
			info[cnt] |= ALLOW_ROCK;
		}
		if (monseeu && onlineu(nx,ny)) {
			if(flag & NOTONL) continue;
			info[cnt] |= NOTONL;
		}
		if (nx != x && ny != y && bad_rock(mon, x, ny)
			    && bad_rock(mon, nx, y)
			    && (bigmonst(mdat) || (curr_mon_load(mon) > 5000)))
			continue;
		/* The monster avoids a particular type of trap if it's familiar
		 * with the trap type.  Pets get ALLOW_TRAPS and checking is
		 * done in dogmove.c.  In either case, "harmless" traps are
		 * neither avoided nor marked in info[].
		 */
		{ register struct trap *ttmp = t_at(nx, ny);
		    if(ttmp) {
			if(ttmp->ttyp >= TRAPNUM || ttmp->ttyp == 0)  {
impossible("A monster looked at a very strange trap of type %d.", ttmp->ttyp);
			    continue;
			}
			if ((ttmp->ttyp != RUST_TRAP
					|| mdat == &mons[PM_FLAMING_SPHERE]
					|| mdat == &mons[PM_IRON_GOLEM])
				&& ttmp->ttyp != STATUE_TRAP
				&& ttmp->ttyp != SHIT_TRAP
				&& ttmp->ttyp != ANIMATION_TRAP
				&& ttmp->ttyp != RMB_LOSS_TRAP
				&& ttmp->ttyp != SUPERSCROLLER_TRAP
				&& ttmp->ttyp != ACTIVE_SUPERSCROLLER_TRAP
				&& ttmp->ttyp != SPEED_TRAP
				&& ttmp->ttyp != MENU_TRAP
				&& ttmp->ttyp != SWARM_TRAP
				&& ttmp->ttyp != AUTOMATIC_SWITCHER
				&& ttmp->ttyp != HEEL_TRAP
				&& ttmp->ttyp != VULN_TRAP
				&& ttmp->ttyp != DISPLAY_TRAP
				&& ttmp->ttyp != SPELL_LOSS_TRAP
				&& ttmp->ttyp != YELLOW_SPELL_TRAP
				&& ttmp->ttyp != AUTO_DESTRUCT_TRAP
				&& ttmp->ttyp != MEMORY_TRAP
				&& ttmp->ttyp != INVENTORY_TRAP
				&& ttmp->ttyp != BLACK_NG_WALL_TRAP
				&& ttmp->ttyp != UNKNOWN_TRAP
				&& ttmp->ttyp != TRAP_PERCENTS
				&& ((ttmp->ttyp != PIT
				    && ttmp->ttyp != SPIKED_PIT
				    && ttmp->ttyp != TRAPDOOR
				    && ttmp->ttyp != HOLE)
				      || (!is_flyer(mdat)
				    && !is_floater(mdat)
				    && !is_clinger(mdat))
				      || In_sokoban(&u.uz))
				&& (ttmp->ttyp != SLP_GAS_TRAP ||
				    (!resists_sleep(mon) && !breathless(mdat)) )
				&& (ttmp->ttyp != POISON_GAS_TRAP ||
				    (!resists_poison(mon) && !breathless(mdat)) )
				&& (ttmp->ttyp != SLOW_GAS_TRAP ||
				    !breathless(mdat) )
				&& (ttmp->ttyp != BEAR_TRAP ||
				    (mdat->msize > MZ_SMALL &&
				     !amorphous(mdat) && !is_flyer(mdat)))
				&& (ttmp->ttyp != FIRE_TRAP ||
				    !resists_fire(mon))
				&& (ttmp->ttyp != SHOCK_TRAP ||
				    !resists_elec(mon))
				&& (ttmp->ttyp != ICE_TRAP ||
				    !resists_cold(mon))
				&& (ttmp->ttyp != SQKY_BOARD || !is_flyer(mdat))
				&& (ttmp->ttyp != ACID_POOL || (!is_flyer(mdat) && !is_floater(mdat) && !resists_acid(mon)) )
				&& (ttmp->ttyp != WATER_POOL || (!is_flyer(mdat) && !is_floater(mdat) && !is_swimmer(mdat) && !amphibious(mdat) && !breathless(mdat)) )
				&& (ttmp->ttyp != WEB || (!amorphous(mdat) &&
				    !webmaker(mdat)))
			) {
			    if (!(flag & ALLOW_TRAPS)) {
				if (mon->mtrapseen & (1L << (ttmp->ttyp - 1)))
				    continue;
			    }
			    info[cnt] |= ALLOW_TRAPS;
			}
		    }
		}
		poss[cnt].x = nx;
		poss[cnt].y = ny;
		cnt++;
	    }
	}

	/* The following code assumes that all "wantlava" monsters are also "wantpool". --Amy */
	if(!cnt && wantlava && !is_lava(x,y) && !is_pool(x,y) ) {
		wantlava = FALSE;
		wantpool = FALSE;
		goto nexttry;
	}
	/* The reverse is obviously not true; this check had to be changed to make sure lava-loving monsters don't leave their pools of lava... */
	if(!cnt && wantpool && !wantlava && !is_pool(x,y)) {
		wantpool = FALSE;
		goto nexttry;
	}
	return(cnt);
}

#endif /* OVL0 */
#ifdef OVL1

/* Monster against monster special attacks; for the specified monster
   combinations, this allows one monster to attack another adjacent one
   in the absence of Conflict.  There is no provision for targetting
   other monsters; just hand to hand fighting when they happen to be
   next to each other. */
STATIC_OVL long
mm_aggression(magr, mdef)
struct monst *magr,	/* monster that is currently deciding where to move */
	     *mdef;	/* another monster which is next to it */
{

	if (Race_if(PM_ALBAE)) return 0L; /* if you're an albae, everything hates you more than anything else --Amy */
	if (Role_if(PM_CRUEL_ABUSER) && Qstatf(killed_nemesis) ) return 0L; /* or if you killed the abuser nemesis */
	if (Aggravate_monster && rn2(5) ) return 0L; /* aggravate monster makes monsters much more likely to target you and ignore everything else */

	/* supposedly purple worms are attracted to shrieking because they
	   like to eat shriekers, so attack the latter when feasible */
	if (magr->data == &mons[PM_PURPLE_WORM] &&
		mdef->data == &mons[PM_SHRIEKER])
	    return ALLOW_M|ALLOW_TM;

	/* elves vs. orcs */
	if(magr->data->mflags2 & M2_ELF && mdef->data->mflags2 & M2_ORC)
		return ALLOW_M|ALLOW_TM;
	/* and vice versa */
	if(mdef->data->mflags2 & M2_ELF && magr->data->mflags2 & M2_ORC)
		return ALLOW_M|ALLOW_TM;

	/* angels vs. demons */
	if(magr->data->mlet==S_ANGEL && mdef->data->mflags2 & M2_DEMON)
		return ALLOW_M|ALLOW_TM;
	/* and vice versa */
	if(mdef->data->mlet==S_ANGEL && magr->data->mflags2 & M2_DEMON)
		return ALLOW_M|ALLOW_TM;

	/* woodchucks vs. The Oracle */
	if(magr->data == &mons[PM_WOODCHUCK] && mdef->data == &mons[PM_ORACLE])
		return ALLOW_M|ALLOW_TM;

	/* aurors vs. death eaters */
	if(magr->data == &mons[PM_AUROR] && mdef->data == &mons[PM_FELLOW_DEATH_EATER])
		return ALLOW_M|ALLOW_TM;
	if(mdef->data == &mons[PM_AUROR] && magr->data == &mons[PM_FELLOW_DEATH_EATER])
		return ALLOW_M|ALLOW_TM;

#ifdef JEDI
	/* Stormtroopers vs. Padawans */
	if(magr->data == &mons[PM_STORMTROOPER] && mdef->data == &mons[PM_PADAWAN])
		return ALLOW_M|ALLOW_TM;
	/* and vice versa */
	if(mdef->data == &mons[PM_STORMTROOPER] && magr->data == &mons[PM_PADAWAN])
		return ALLOW_M|ALLOW_TM;
	/* Stormtroopers vs. Jedi */
	if(magr->data == &mons[PM_STORMTROOPER] && mdef->data == &mons[PM_JEDI])
		return ALLOW_M|ALLOW_TM;
	/* and vice versa */
	if(mdef->data == &mons[PM_STORMTROOPER] && magr->data == &mons[PM_JEDI])
		return ALLOW_M|ALLOW_TM;
	/* Stormtroopers vs. The Jedi Master */
	//if(magr->data == &mons[PM_STORMTROOPER] && mdef->data == &mons[PM_THE_JEDI_MASTER])
		//return ALLOW_M|ALLOW_TM;
	/* and vice versa */
	//if(mdef->data == &mons[PM_STORMTROOPER] && magr->data == &mons[PM_THE_JEDI_MASTER])
		//return ALLOW_M|ALLOW_TM;
	/* Jedi vs. Lord Sidious */
	if(magr->data == &mons[PM_LORD_SIDIOUS] && mdef->data == &mons[PM_JEDI])
		return ALLOW_M|ALLOW_TM;
	/* and vice versa */
	if(mdef->data == &mons[PM_LORD_SIDIOUS] && magr->data == &mons[PM_JEDI])
		return ALLOW_M|ALLOW_TM;
#endif

	return 0L;
}

boolean
monnear(mon, x, y)
register struct monst *mon;
register int x,y;
/* Is the square close enough for the monster to move or attack into? */
{
	register int distance = dist2(mon->mx, mon->my, x, y);
	if (distance==2 && ( mon->data==&mons[PM_GRID_BUG] || mon->data==&mons[PM_GRID_XORN]) ) return 0;
	return((boolean)(distance < 3));
}

/* really free dead monsters */
void
dmonsfree()
{
    struct monst **mtmp;
    int count = 0;

    for (mtmp = &fmon; *mtmp;) {
	if ((*mtmp)->mhp <= 0) {
	    struct monst *freetmp = *mtmp;
	    *mtmp = (*mtmp)->nmon;
	    dealloc_monst(freetmp);
	    count++;
	} else
	    mtmp = &(*mtmp)->nmon;
    }

    if (count != iflags.purge_monsters)
	impossible("dmonsfree: %d removed doesn't match %d pending",
		   count, iflags.purge_monsters);
    iflags.purge_monsters = 0;
}

#endif /* OVL1 */
#ifdef OVLB

/* called when monster is moved to larger structure */
void
replmon(mtmp, mtmp2)
register struct monst *mtmp, *mtmp2;
{
    struct obj *otmp;
    long unpolytime; /* WAC */

    /* transfer the monster's inventory */
    for (otmp = mtmp2->minvent; otmp; otmp = otmp->nobj) {
#ifdef DEBUG
	if (otmp->where != OBJ_MINVENT || otmp->ocarry != mtmp)
	    panic("replmon: minvent inconsistency");
#endif
	otmp->ocarry = mtmp2;
    }
    mtmp->minvent = 0;

    /* remove the old monster from the map and from `fmon' list */
    relmon(mtmp);

    /* finish adding its replacement */
#ifdef STEED
    if (mtmp == u.usteed) ; else	/* don't place steed onto the map */
#endif
    place_monster(mtmp2, mtmp2->mx, mtmp2->my);
    if (mtmp2->wormno)	    /* update level.monsters[wseg->wx][wseg->wy] */
	place_wsegs(mtmp2); /* locations to mtmp2 not mtmp. */
    if (emits_light(mtmp2->data)) {
	/* since this is so rare, we don't have any `mon_move_light_source' */
	new_light_source(mtmp2->mx, mtmp2->my,
			 emits_light(mtmp2->data),
			 LS_MONSTER, (genericptr_t)mtmp2);
	/* here we rely on the fact that `mtmp' hasn't actually been deleted */
	del_light_source(LS_MONSTER, (genericptr_t)mtmp);
    }
    /* If poly'ed,  move polytimer along */
    if (unpolytime = stop_timer(UNPOLY_MON, (genericptr_t) mtmp)) {
        (void) start_timer(unpolytime, TIMER_MONSTER, UNPOLY_MON,
                        (genericptr_t) mtmp2);
    }
    mtmp2->nmon = fmon;
    fmon = mtmp2;
    if (u.ustuck == mtmp) setustuck(mtmp2);
#ifdef STEED
    if (u.usteed == mtmp) u.usteed = mtmp2;
#endif
    if (mtmp2->isshk) replshk(mtmp,mtmp2);

    /* discard the old monster */
    dealloc_monst(mtmp);
}

/* release mon from display and monster list */
void
relmon(mon)
register struct monst *mon;
{
	register struct monst *mtmp;

	if (fmon == (struct monst *)0)  panic ("relmon: no fmon available.");

	remove_monster(mon->mx, mon->my);

	if(mon == fmon) fmon = fmon->nmon;
	else {
		for(mtmp = fmon; mtmp && mtmp->nmon != mon; mtmp = mtmp->nmon) ;
		if(mtmp)    mtmp->nmon = mon->nmon;
		else	    panic("relmon: mon not in list.");
	}
}

/* remove effects of mtmp from other data structures */
STATIC_OVL void
m_detach(mtmp, mptr)
struct monst *mtmp;
struct permonst *mptr;	/* reflects mtmp->data _prior_ to mtmp's death */
{
 	mon_stop_timers(mtmp);
	if (mtmp->mleashed) m_unleash(mtmp, FALSE);
	    /* to prevent an infinite relobj-flooreffects-hmon-killed loop */
	mtmp->mtrapped = 0;
	mtmp->mhp = 0; /* simplify some tests: force mhp to 0 */
	relobj(mtmp, 0, FALSE);
	remove_monster(mtmp->mx, mtmp->my);
	if (emits_light(mptr))
	    del_light_source(LS_MONSTER, (genericptr_t)mtmp);
	newsym(mtmp->mx,mtmp->my);
	unstuck(mtmp);
	fill_pit(mtmp->mx, mtmp->my);

	if(mtmp->isshk) shkgone(mtmp);
	if(mtmp->wormno) wormgone(mtmp);
	iflags.purge_monsters++;
}

/* find the worn amulet of life saving which will save a monster */
struct obj *
mlifesaver(mon)
struct monst *mon;
{
	/*if (!nonliving(mon->data)) {*/
	    struct obj *otmp = which_armor(mon, W_AMUL);

	    if (otmp && otmp->otyp == AMULET_OF_LIFE_SAVING)
		return otmp;
	/*}*/
	return (struct obj *)0;
}

STATIC_OVL void
lifesaved_monster(mtmp)
struct monst *mtmp;
{
	int visible;
	struct obj *lifesave = mlifesaver(mtmp);

	if (lifesave) {
		/* not canseemon; amulets are on the head, so you don't want */
		/* to show this for a long worm with only a tail visible. */
		/* Nor do you check invisibility, because glowing and disinte- */
		/* grating amulets are always visible. */
		/* [ALI] Always treat swallower as visible for consistency */
		/* with unpoly_monster(). */
		visible = u.uswallow && u.ustuck == mtmp ||
			cansee(mtmp->mx, mtmp->my);
		if (visible) {
			pline("But wait...");
			pline("%s medallion begins to glow!",
				s_suffix(Monnam(mtmp)));
			makeknown(AMULET_OF_LIFE_SAVING);
			if (attacktype(mtmp->data, AT_EXPL)
			    || attacktype(mtmp->data, AT_BOOM))
				pline("%s reconstitutes!", Monnam(mtmp));
			else
				pline("%s looks much better!", Monnam(mtmp));
			pline_The("medallion crumbles to dust!");
		}
		m_useup(mtmp, lifesave);
		mtmp->mcanmove = 1;
		mtmp->mfrozen = 0;
		if (mtmp->mtame && !mtmp->isminion) {
			wary_dog(mtmp, FALSE);
		}
		if (mtmp->mhpmax <= 0) mtmp->mhpmax = 10;
		mtmp->mhp = mtmp->mhpmax;
		if (mvitals[monsndx(mtmp->data)].mvflags & G_GENOD) {
			if (visible)
			    pline("Unfortunately %s is still genocided...",
				mon_nam(mtmp));
		} else
			return;
	}
	mtmp->mhp = 0;
}

/* WAC -- undo polymorph */
static void
unpoly_monster(mtmp)
struct monst *mtmp;
{
	int visible;
	char buf[BUFSZ];

	sprintf(buf, Monnam(mtmp));

	/* If there is a timer == monster was poly'ed */
	if (stop_timer(UNPOLY_MON, (genericptr_t) mtmp)) {
	    /* [ALI] Always treat swallower as visible so that the message
	     * indicating that the monster hasn't died comes _before_ any
	     * message about breaking out of the "new" monster.
	     */
	    visible = u.uswallow && u.ustuck == mtmp || cansee(mtmp->mx,mtmp->my);
	    mtmp->mhp = mtmp->mhpmax;
	    if (visible)
		pline("But wait...");
	    if ((newcham(mtmp, &mons[mtmp->oldmonnm], FALSE, visible)) && rn2(25))
		mtmp->mhp = mtmp->mhpmax/2; /* 4% risk of the unpolymorph killing the monster. --Amy */
	    else {
		if (visible)
		    pline("%s shudders!", Monnam(mtmp));
		mtmp->mhp = 0;
	    }
	}
}

void
mondead(mtmp)
register struct monst *mtmp;
{
	struct permonst *mptr;
	int tmp;
	int copcnt; /* Angry shopkeepers and guards will send in the police and sephirah! --Amy */

	/* WAC just in case caller forgot to...*/
	if (mtmp->mhp) mtmp->mhp = -1;

	if (mtmp->isshk && !strncmpi(shkname(mtmp), "Izchak", 6) ) { /* Thou must not kill Izchak or allow him to die. --Amy */

		adjalign(-500);
		change_luck(-10);
		You_feel("the air around you grow charged...");
		pline("Suddenly, you realize that %s has noticed you...", u_gname());
		/* Throw everything we have at the player */
		u.ugangr++; u.ugangr++; u.ugangr++; u.ugangr++; u.ugangr++; u.ugangr++; u.ugangr++; u.ugangr++; u.ugangr++;
		prayer_done();
		god_zaps_you(u.ualign.type);
	}

	if(mtmp->isgd) {
		/* if we're going to abort the death, it *must* be before
		 * the m_detach or there will be relmon problems later */
		if(!grddead(mtmp)) return;
	}

	mptr = mtmp->data;

	/* WAC First check that monster can unpoly */
	unpoly_monster(mtmp);
	if (mtmp->mhp > 0) return;

	lifesaved_monster(mtmp);
	if (mtmp->mhp > 0) return;

#ifdef STEED
	/* Player is thrown from his steed when it dies */
	if (mtmp == u.usteed)
		dismount_steed(DISMOUNT_GENERIC);
#endif

	mptr = mtmp->data;		/* save this for m_detach() */
	/* restore chameleon, lycanthropes to true form at death */
	if (mtmp->cham)
	    set_mon_data(mtmp, &mons[cham_to_pm[mtmp->cham]], -1);
	else if (mtmp->data == &mons[PM_WEREJACKAL])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WEREJACKAL], -1);
	else if (mtmp->data == &mons[PM_WEREWOLF])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WEREWOLF], -1);
	else if (mtmp->data == &mons[PM_WERERAT])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WERERAT], -1);
	else if (mtmp->data == &mons[PM_WEREPANTHER])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WEREPANTHER], -1);
	else if (mtmp->data == &mons[PM_WERETIGER])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WERETIGER], -1);
	else if (mtmp->data == &mons[PM_WERESNAKE])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WERESNAKE], -1);
	else if (mtmp->data == &mons[PM_WERESPIDER])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WERESPIDER], -1);
	else if (mtmp->data == &mons[PM_WEREGIANT])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WEREGIANT], -1);
	else if (mtmp->data == &mons[PM_WERELICHEN])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WERELICHEN], -1);
	else if (mtmp->data == &mons[PM_WEREVORTEX])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WEREVORTEX], -1);
	else if (mtmp->data == &mons[PM_WERECOW])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WERECOW], -1);
	else if (mtmp->data == &mons[PM_WEREBEAR])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WEREBEAR], -1);
	else if (mtmp->data == &mons[PM_WEREPIERCER])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WEREPIERCER], -1);
	else if (mtmp->data == &mons[PM_WEREPENETRATOR])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WEREPENETRATOR], -1);
	else if (mtmp->data == &mons[PM_WERESMASHER])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WERESMASHER], -1);
	else if (mtmp->data == &mons[PM_WERENYMPH])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WERENYMPH], -1);
	else if (mtmp->data == &mons[PM_WEREMIMIC])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WEREMIMIC], -1);
	else if (mtmp->data == &mons[PM_WERECOCKATRICE])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WERECOCKATRICE], -1);
	else if (mtmp->data == &mons[PM_WERESOLDIERANT])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WERESOLDIERANT], -1);
	else if (mtmp->data == &mons[PM_WEREMINDFLAYER])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WEREMINDFLAYER], -1);
	else if (mtmp->data == &mons[PM_WEREGHOST])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WEREGHOST], -1);
	else if (mtmp->data == &mons[PM_WEREPIRANHA])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WEREPIRANHA], -1);
	else if (mtmp->data == &mons[PM_WEREEEL])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WEREEEL], -1);
	else if (mtmp->data == &mons[PM_WEREFLYFISH])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WEREFLYFISH], -1);
	else if (mtmp->data == &mons[PM_WEREKRAKEN])
	    set_mon_data(mtmp, &mons[PM_HUMAN_WEREKRAKEN], -1);

	/* if MAXMONNO monsters of a given type have died, and it
	 * can be done, extinguish that monster.
	 *
	 * mvitals[].died does double duty as total number of dead monsters
	 * and as experience factor for the player killing more monsters.
	 * this means that a dragon dying by other means reduces the
	 * experience the player gets for killing a dragon directly; this
	 * is probably not too bad, since the player likely finagled the
	 * first dead dragon via ring of conflict or pets, and extinguishing
	 * based on only player kills probably opens more avenues of abuse
	 * for rings of conflict and such.
	 */
	/* KMH -- Yes, keep spell monsters in the count */
	tmp = monsndx(mtmp->data);

	if (tmp == PM_BLACKY) {
		if (wizard) pline("Blacky died!");
		BlackNgWalls = 0L;
	}

	if (mvitals[tmp].died < 255) mvitals[tmp].died++;

	/* if it's a (possibly polymorphed) quest leader, mark him as dead */
	if (mtmp->m_id == quest_status.leader_m_id)
	    quest_status.leader_is_dead = TRUE;
#ifdef MAIL
	/* if the mail daemon dies, no more mail delivery.  -3. */
	if (tmp == PM_MAIL_DAEMON) mvitals[tmp].mvflags |= G_GENOD;
#endif

#ifdef KOPS
	if (mtmp->data->mlet == S_KOP) {
	    /* Dead Kops may come back. */
	    switch(rnd(10)) {
		case 1:	     /* returns near the stairs */
			(void) makemon(mtmp->data,xdnstair,ydnstair,NO_MM_FLAGS);
			break;
		case 2:	     /* randomly */
			(void) makemon(mtmp->data,0,0,NO_MM_FLAGS);
			break;
		case 3:	     /* returns near the stairs */
			(void) makemon(mkclass(S_KOP,0),xdnstair,ydnstair,NO_MM_FLAGS);
			break;
		case 4:	     /* randomly */
			(void) makemon(mkclass(S_KOP,0),0,0,NO_MM_FLAGS);
			break;
		default:
			break;
	    }
	}
#endif
	if(mtmp->iswiz) wizdead();
	if(mtmp->data->msound == MS_NEMESIS && mtmp->data->mlet != S_NEMESE) nemdead();

	if(tmp == PM_ANASTASIA_STEELE) { /* very bad! */

		change_luck(-10);
		u.ualign.sins += 20; 
		adjalign(-200);
		u.ugangr++; u.ugangr++; u.ugangr++; u.ugangr++; u.ugangr++;
		prayer_done();

	}

	if(tmp == PM_SHOPKEEPER || tmp == PM_BLACK_MARKETEER || tmp == PM_GUARD) /* punishment */ {

		pline("The twit quickly called the kops, and it seems they're out to get you!");
		copcnt = rnd(monster_difficulty() ) + 1;

	      while(--copcnt >= 0) {
			(void) makemon(mkclass(S_KOP,0), u.ux, u.uy, MM_ANGRY);
		} /* while */

	}

#ifdef RECORD_ACHIEVE
        if(mtmp->data == &mons[PM_MEDUSA])
            achieve.killed_medusa = 1;
        if(mtmp->data == &mons[PM_NIGHTMARE])
            achieve.killed_nightmare = 1;
        if(mtmp->data == &mons[PM_VECNA])
            achieve.killed_vecna = 1;
        if(mtmp->data == &mons[PM_BEHOLDER])
            achieve.killed_beholder = 1;
        if(mtmp->data == &mons[PM_RUGGO_THE_GNOME_KING])
            achieve.killed_ruggo = 1;
        if(mtmp->data == &mons[PM_KROO_THE_KOBOLD_KING])
            achieve.killed_kroo = 1;
        if(mtmp->data == &mons[PM_GRUND_THE_ORC_KING])
            achieve.killed_grund = 1;
        if(mtmp->data == &mons[PM_THE_LARGEST_GIANT])
            achieve.killed_largestgiant = 1;
        if(mtmp->data == &mons[PM_SHELOB])
            achieve.killed_shelob = 1;
        if(mtmp->data == &mons[PM_GIRTAB])
            achieve.killed_girtab = 1;
        if(mtmp->data == &mons[PM_APHRODITE])
            achieve.killed_aphrodite = 1;
        if(mtmp->data == &mons[PM_DOCTOR_FRANKENSTEIN])
            achieve.killed_frankenstein = 1;
        if(mtmp->data == &mons[PM_CROESUS])
            achieve.killed_croesus = 1;
        if(mtmp->data == &mons[PM_FATHER_DAGON])
            achieve.killed_dagon = 1;
        if(mtmp->data == &mons[PM_MOTHER_HYDRA])
            achieve.killed_hydra = 1;
#ifdef LIVELOGFILE
		livelog_achieve_update();
#endif
#endif

	if(memory_is_invisible(mtmp->mx, mtmp->my))
	    unmap_object(mtmp->mx, mtmp->my);
	m_detach(mtmp, mptr);
}

/* TRUE if corpse might be dropped, magr may die if mon was swallowed */
boolean
corpse_chance(mon, magr, was_swallowed)
struct monst *mon;
struct monst *magr;			/* killer, if swallowed */
boolean was_swallowed;			/* digestion */
{
	struct permonst *mdat = mon->data;
	int i, tmp;

	if (mdat == &mons[PM_VLAD_THE_IMPALER] || mdat->mlet == S_LICH) {
	    if (cansee(mon->mx, mon->my) && !was_swallowed)
		pline("%s body crumbles into dust.", s_suffix(Monnam(mon)));
	    /* KMH -- make_corpse() handles Vecna */
	    return (mdat == &mons[PM_VECNA]);
	}

	/* Gas spores always explode upon death */
	for(i = 0; i < NATTK; i++) {
	    if (mdat->mattk[i].aatyp == AT_BOOM) {
	    	if (mdat->mattk[i].damn)
	    	    tmp = d((int)mdat->mattk[i].damn,
	    	    		(int)mdat->mattk[i].damd);
	    	else if(mdat->mattk[i].damd)
	    	    tmp = d((int)mdat->mlevel+1, (int)mdat->mattk[i].damd);
	    	else tmp = 0;
		if (was_swallowed && magr) {
		    if (magr == &youmonst) {
			There("is an explosion in your %s!",
			      body_part(STOMACH));
			Sprintf(killer_buf, "%s explosion",
				s_suffix(mdat->mname));
			if (Half_physical_damage && rn2(2) ) tmp = (tmp+1) / 2;
			losehp(tmp, killer_buf, KILLED_BY_AN);
		    } else {
			if (flags.soundok) You_hear("an explosion.");
			magr->mhp -= tmp;
			if (magr->mhp < 1) mondied(magr);
			if (magr->mhp < 1) { /* maybe lifesaved */
			    if (canspotmon(magr))
				pline("%s rips open!", Monnam(magr));
			} else if (canseemon(magr))
			    pline("%s seems to have indigestion.",
				  Monnam(magr));
		    }

		    return FALSE;
		}

	    	Sprintf(killer_buf, "%s explosion", s_suffix(mdat->mname));
	    	killer = killer_buf;
	    	killer_format = KILLED_BY_AN;
	    	explode(mon->mx, mon->my, -1, tmp, MON_EXPLODE, EXPL_NOXIOUS); 
	    	return (FALSE);
	    }
  	}

	/* Cthulhu Deliquesces... */
	if (mdat == &mons[PM_CTHULHU]) {
	    if (cansee(mon->mx, mon->my))
		pline("%s body deliquesces into a cloud of noxious gas!",
			s_suffix(Monnam(mon)));
	    else
		You_hear("hissing and bubbling!");
	    /* ...into a stinking cloud... */
	    (void) create_cthulhu_death_cloud(mon->mx, mon->my, 3, 8);
	    return (FALSE);
	}

	/* must duplicate this below check in xkilled() since it results in
	 * creating no objects as well as no corpse
	 */
	if (LEVEL_SPECIFIC_NOCORPSE(mdat))
		return FALSE;

	if (bigmonst(mdat) || mdat == &mons[PM_LIZARD] || mdat == &mons[PM_CAVE_LIZARD] || mdat == &mons[PM_CHAOS_LIZARD] || mdat == &mons[PM_LIZARD_EEL] || mdat == &mons[PM_EEL_LIZARD] || mdat == &mons[PM_ROCK_LIZARD] || mdat == &mons[PM_NIGHT_LIZARD] || mdat == &mons[PM_LIZARD_MAN] || mdat == &mons[PM_LIZARD_KING] || mdat == &mons[PM_ANTI_STONE_LIZARD]  || mdat == &mons[PM_GIANT_LIZARD] || mdat == &mons[PM_HIDDEN_LIZARD] || mdat == &mons[PM_DEFORMED_LIZARD] || mdat == &mons[PM_MIMIC_LIZARD] || mdat == &mons[PM_CLINGING_LIZARD] || mdat == &mons[PM_HUGE_LIZARD] || mdat == &mons[PM_KARMIC_LIZARD] || mdat == &mons[PM_SAND_TIDE] || mdat == &mons[PM_MONSTER_LIZARD] || mdat == &mons[PM_FBI_AGENT] || mdat == &mons[PM_OWN_SMOKE] || mdat == &mons[PM_GRANDPA] || mdat == &mons[PM_FIRE_LIZARD] || mdat == &mons[PM_LIGHTNING_LIZARD] || mdat == &mons[PM_ICE_LIZARD] || mdat == &mons[PM_KATNISS]
		   || is_golem(mdat)
		   || is_mplayer(mdat)
		   || is_umplayer(mdat)
		   || is_rider(mdat))
		return TRUE;
	return (boolean) (!rn2((int)
		(2 + ((int)(mdat->geno & G_FREQ)<2) + verysmall(mdat))));
}

/* drop (perhaps) a cadaver and remove monster */
void
mondied(mdef)
register struct monst *mdef;
{
	mondead(mdef);
	if (mdef->mhp > 0) return;	/* lifesaved */

	if (corpse_chance(mdef, (struct monst *)0, FALSE) &&
	    (accessible(mdef->mx, mdef->my) || is_pool(mdef->mx, mdef->my)))
		(void) make_corpse(mdef);
}

/* monster disappears, not dies */
void
mongone(mdef)
register struct monst *mdef;
{
	mdef->mhp = 0;	/* can skip some inventory bookkeeping */
#ifdef STEED
	/* Player is thrown from his steed when it disappears */
	if (mdef == u.usteed)
		dismount_steed(DISMOUNT_GENERIC);
#endif

	/* drop special items like the Amulet so that a dismissed Kop or nurse
	   can't remove them from the game */
	mdrop_special_objs(mdef);
	/* release rest of monster's inventory--it is removed from game */
	discard_minvent(mdef);
#ifndef GOLDOBJ
	mdef->mgold = 0L;
#endif
	m_detach(mdef, mdef->data);
}

/* drop a statue or rock and remove monster */
void
monstone(mdef)
register struct monst *mdef;
{
	struct obj *otmp, *obj, *oldminvent;
	xchar x = mdef->mx, y = mdef->my;
	boolean wasinside = FALSE;

	/* we have to make the statue before calling mondead, to be able to
	 * put inventory in it, and we have to check for lifesaving before
	 * making the statue....
	 */
	lifesaved_monster(mdef);
	if (mdef->mhp > 0) return;

	mdef->mtrapped = 0;	/* (see m_detach) */

	if ((int)mdef->data->msize > MZ_TINY ||
		    !rn2(2 + ((int) (mdef->data->geno & G_FREQ) > 2))) {
		oldminvent = 0;
		/* some objects may end up outside the statue */
		while ((obj = mdef->minvent) != 0) {
		    obj_extract_self(obj);
		    if (obj->owornmask)
			update_mon_intrinsics(mdef, obj, FALSE, TRUE);
		    obj_no_longer_held(obj);
		    if (obj->owornmask & W_WEP)
			setmnotwielded(mdef,obj);
		    obj->owornmask = 0L;
		    if (obj->otyp == BOULDER ||
#if 0				/* monsters don't carry statues */
     (obj->otyp == STATUE && mons[obj->corpsenm].msize >= mdef->data->msize) ||
#endif
				obj_resists(obj, 0, 0)) {
			if (flooreffects(obj, x, y, "fall")) continue;
			place_object(obj, x, y);
		    } else {
			if (obj->lamplit) end_burn(obj, TRUE);
			obj->nobj = oldminvent;
			oldminvent = obj;
		    }
		}
		/* defer statue creation until after inventory removal
		   so that saved monster traits won't retain any stale
		   item-conferred attributes */
		otmp = mkcorpstat(STATUE, KEEPTRAITS(mdef) ? mdef : 0,
				  mdef->data, x, y, FALSE);
		if (mdef->mnamelth) otmp = oname(otmp, NAME(mdef));
		while ((obj = oldminvent) != 0) {
		    oldminvent = obj->nobj;

		/* reduce amount of musable items the player can use --Amy */
		/* item stealers usually won't delete stuff, since their stuff might actually be your original stuff! */
			if (is_musable(obj) && rn2(2) && !dmgtype(mdef->data, AD_SEDU) && !dmgtype(mdef->data, AD_SITM) && (!dmgtype(mdef->data, AD_SSEX) || rn2(3) ) && (!dmgtype(mdef->data, AD_SGLD) || rn2(4) ) ) delobj(obj);
		    else (void) add_to_container(otmp, obj);
		}
#ifndef GOLDOBJ
		if (mdef->mgold) {
			struct obj *au;
			au = mksobj(GOLD_PIECE, FALSE, FALSE);
			au->quan = mdef->mgold;
			au->owt = weight(au);
			(void) add_to_container(otmp, au);
			mdef->mgold = 0;
		}
#endif
		/* Archeologists should not break unique statues */
		if (mdef->data->geno & G_UNIQ)
			otmp->spe = 1;
		otmp->owt = weight(otmp);
	} else
		otmp = mksobj_at(ROCK, x, y, TRUE, FALSE);

	stackobj(otmp);
	/* mondead() already does this, but we must do it before the newsym */
	if(memory_is_invisible(x, y))
	    unmap_object(x, y);
	if (cansee(x, y)) newsym(x,y);
	/* We don't currently trap the hero in the statue in this case but we could */
	if (u.uswallow && u.ustuck == mdef) wasinside = TRUE;
	(void) stop_timer(UNPOLY_MON, (genericptr_t) mdef);
	mondead(mdef);
	if (wasinside) {
		if (is_animal(mdef->data))
			You("%s through an opening in the new %s.",
				locomotion(youmonst.data, "jump"),
				xname(otmp));
	}
}

/* another monster has killed the monster mdef */
void
monkilled(mdef, fltxt, how)
register struct monst *mdef;
const char *fltxt;
int how;
{
	boolean be_sad = FALSE;		/* true if unseen pet is killed */

	if ((mdef->wormno ? worm_known(mdef) : cansee(mdef->mx, mdef->my))
		&& fltxt)
	    pline("%s is %s%s%s!", Monnam(mdef),
			nonliving(mdef->data) ? "destroyed" : "killed",
		    *fltxt ? " by the " : "",
		    fltxt
		 );
	else
	    be_sad = (mdef->mtame != 0 && !mdef->isspell);

	/* no corpses if digested or disintegrated */
	if(how == AD_DGST || how == -AD_RBRE)
	    mondead(mdef);
	else
	    mondied(mdef);

	if (be_sad && mdef->mhp <= 0)
	    You(Hallucination ? "are feeling totally down for a moment, then it passes." : (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR)) ? "hang the jib for a moment, then it passes." : "have a sad feeling for a moment, then it passes.");
}


/* WAC -- another monster has killed the monster mdef and you get exp. */
void
mon_xkilled(mdef, fltxt, how)
	register struct monst *mdef;
	const char *fltxt;
	int how;
{
	boolean be_sad = FALSE;         /* true if unseen pet is killed */

	if ((mdef->wormno ? worm_known(mdef) : cansee(mdef->mx, mdef->my))
			&& fltxt)
		pline("%s is %s%s%s!", Monnam(mdef),
				nonliving(mdef->data) ? "destroyed" : "killed",
				*fltxt ? " by the " : "",
				fltxt);
	else
		be_sad = (mdef->mtame != 0 && !mdef->isspell); 

	/* no corpses if digested or disintegrated */
	if(how == AD_DGST || how == -AD_RBRE)
		xkilled(mdef,2);
	else
		xkilled(mdef,0);

	if (be_sad && mdef->mhp <= 0)
	    You(Hallucination ? "are feeling totally down for a moment, then it passes." : (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR)) ? "hang the jib for a moment, then it passes." : "have a sad feeling for a moment, then it passes.");
}


void
unstuck(mtmp)
register struct monst *mtmp;
{
	if(u.ustuck == mtmp) {
		if(u.uswallow){
			u.ux = mtmp->mx;
			u.uy = mtmp->my;
			u.uswallow = 0;
			u.uswldtim = 0;
			if (Punished) placebc();
			vision_full_recalc = 1;
			docrt();
		}
		setustuck(0);
	}
}

void
killed(mtmp)
register struct monst *mtmp;
{
	xkilled(mtmp, 1);
}

/* the player has killed the monster mtmp */
void
xkilled(mtmp, dest)
	register struct monst *mtmp;
/*
 * Dest=1, normal; dest=0, don't print message; dest=2, don't drop corpse
 * either; dest=3, message but no corpse
 */
	int	dest;
{
	register int tmp, x = mtmp->mx, y = mtmp->my;
	register struct permonst *mdat;
	int mndx;
	register struct obj *otmp;
	register struct trap *t;
	boolean redisp = FALSE;
	boolean wasinside = u.uswallow && (u.ustuck == mtmp);


	/* KMH, conduct */
	u.uconduct.killer++;

	if (Role_if(PM_BLOODSEEKER)) healup(mtmp->m_lev, 0, FALSE, FALSE); /* special ability called "Stygwyr's Thirst" */

	if (dest & 1) {
	    const char *verb = nonliving(mtmp->data) ? "destroy" : "kill";

	    if (!wasinside && !canspotmon(mtmp))
		You("%s it!", verb);
	    else {
		You("%s %s!", verb,
		    !mtmp->mtame ? mon_nam(mtmp) :
			x_monnam(mtmp,
				 mtmp->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 "poor",
				 mtmp->mnamelth ? SUPPRESS_SADDLE : 0,
				 FALSE));
	    }
	}

	if (mtmp->mtrapped && (t = t_at(x, y)) != 0 &&
		(t->ttyp == PIT || t->ttyp == SPIKED_PIT) &&
		sobj_at(BOULDER, x, y))
	    dest |= 2;     /*
			    * Prevent corpses/treasure being created "on top"
			    * of the boulder that is about to fall in. This is
			    * out of order, but cannot be helped unless this
			    * whole routine is rearranged.
			    */

	/* your pet knows who just killed it...watch out */
	if (mtmp->mtame && !mtmp->isminion) EDOG(mtmp)->killed_by_u = 1;

	/* dispose of monster and make cadaver */
	if(stoned) monstone(mtmp);
	else mondead(mtmp);

	if (mtmp->mhp > 0) { /* monster cheated death */
		/* Cannot put the non-visible lifesaving message in
		 * lifesaved_monster()/unpoly_monster() since the message
		 * appears only when you kill it (as opposed to visible
		 * lifesaving which always appears).
		 */
		stoned = FALSE;
		if ((!u.uswallow || u.ustuck != mtmp) && !cansee(x, y))
		    pline("Maybe not...");
		return;
	}

	mdat = mtmp->data; /* note: mondead can change mtmp->data */
	mndx = monsndx(mdat);

	if (stoned) {
		stoned = FALSE;
		goto cleanup;
	}

	if((dest & 2) || LEVEL_SPECIFIC_NOCORPSE(mdat))
		goto cleanup;

#ifdef MAIL
	if(mdat == &mons[PM_MAIL_DAEMON]) {
		stackobj(mksobj_at(SCR_MAIL, x, y, FALSE, FALSE));
		redisp = TRUE;
	}
#endif
	if((!accessible(x, y) && !is_pool(x, y)) ||
	   (x == u.ux && y == u.uy)) {
	    /* might be mimic in wall or corpse in lava or on player's spot */
	    redisp = TRUE;
	    if(wasinside) spoteffects(TRUE);
	} else if(x != u.ux || y != u.uy) {
		/* might be here after swallowed */

		/* Throw a bone to vampiric and ghast players who cannot unstone themselves easily. --Amy */
		if ((mdat == &mons[PM_LIZARD] || mdat == &mons[PM_CAVE_LIZARD] || mdat == &mons[PM_CHAOS_LIZARD] || mdat == &mons[PM_HUGE_LIZARD] || mdat == &mons[PM_SAND_TIDE] || mdat == &mons[PM_FIRE_LIZARD] || mdat == &mons[PM_ROCK_LIZARD] || mdat == &mons[PM_NIGHT_LIZARD] || mdat == &mons[PM_FBI_AGENT] || mdat == &mons[PM_OWN_SMOKE] || mdat == &mons[PM_GRANDPA] || mdat == &mons[PM_LIGHTNING_LIZARD] || mdat == &mons[PM_KARMIC_LIZARD] || mdat == &mons[PM_MONSTER_LIZARD] || mdat == &mons[PM_ICE_LIZARD] || mdat == &mons[PM_LIZARD_EEL] || mdat == &mons[PM_HIDDEN_LIZARD] || mdat == &mons[PM_DEFORMED_LIZARD] || mdat == &mons[PM_MIMIC_LIZARD] || mdat == &mons[PM_CLINGING_LIZARD] || mdat == &mons[PM_LIZARD_MAN]  || mdat == &mons[PM_LIZARD_KING] || mdat == &mons[PM_GIANT_LIZARD] || mdat == &mons[PM_EEL_LIZARD] || mdat == &mons[PM_ANTI_STONE_LIZARD]) && !rn2(5)) otmp = mksobj_at(POT_ACID, x, y, TRUE, FALSE);
		/* of course the acid potions are useful for other races too, if they run out of lizard corpses */

		if ((mdat == &mons[PM_SQUIRREL] || mdat == &mons[PM_IGUANA] || mdat == &mons[PM_HELPFUL_SQUIRREL] || mdat == &mons[PM_BIG_IGUANA]) && !rn2(20)) otmp = mksobj_at(CARROT, x, y, TRUE, FALSE);
		if ((mdat == &mons[PM_SQUIRREL] || mdat == &mons[PM_IGUANA] || mdat == &mons[PM_HELPFUL_SQUIRREL] || mdat == &mons[PM_BIG_IGUANA]) && !rn2(20)) otmp = mksobj_at(BANANA, x, y, TRUE, FALSE);
		if ((mdat == &mons[PM_SQUIRREL] || mdat == &mons[PM_IGUANA] || mdat == &mons[PM_HELPFUL_SQUIRREL] || mdat == &mons[PM_BIG_IGUANA]) && !rn2(20)) otmp = mksobj_at(MELON, x, y, TRUE, FALSE);
		if ((mdat == &mons[PM_SQUIRREL] || mdat == &mons[PM_IGUANA] || mdat == &mons[PM_HELPFUL_SQUIRREL] || mdat == &mons[PM_BIG_IGUANA]) && !rn2(20)) otmp = mksobj_at(PEAR, x, y, TRUE, FALSE);
		if ((mdat == &mons[PM_SQUIRREL] || mdat == &mons[PM_IGUANA] || mdat == &mons[PM_HELPFUL_SQUIRREL] || mdat == &mons[PM_BIG_IGUANA]) && !rn2(50)) otmp = mksobj_at(ASIAN_PEAR, x, y, TRUE, FALSE);

		if ((mdat == &mons[PM_GECKO] || mdat == &mons[PM_GIANT_GECKO]) && !rn2(40)) otmp = mksobj_at(EUCALYPTUS_LEAF, x, y, TRUE, FALSE);

		if ((mdat == &mons[PM_RHAUMBUSUN] || mdat == &mons[PM_BIG_RHAUMBUSUN]) && !rn2(20)) otmp = mksobj_at(SCR_FIRE, x, y, TRUE, FALSE);

		if ((mdat == &mons[PM_SALAMANDER] || mdat == &mons[PM_FROST_SALAMANDER] || mdat == &mons[PM_KOMODO_DRAGON] || mdat == &mons[PM_PETTY_KOMODO_DRAGON]) && !rn2(20)) otmp = mksobj_at(CREAM_PIE, x, y, TRUE, FALSE);
		if ((mdat == &mons[PM_SALAMANDER] || mdat == &mons[PM_FROST_SALAMANDER] || mdat == &mons[PM_KOMODO_DRAGON] || mdat == &mons[PM_PETTY_KOMODO_DRAGON]) && !rn2(20)) otmp = mksobj_at(APPLE, x, y, TRUE, FALSE);
		if ((mdat == &mons[PM_SALAMANDER] || mdat == &mons[PM_FROST_SALAMANDER] || mdat == &mons[PM_KOMODO_DRAGON] || mdat == &mons[PM_PETTY_KOMODO_DRAGON]) && !rn2(20)) otmp = mksobj_at(ORANGE, x, y, TRUE, FALSE);
		if ((mdat == &mons[PM_SALAMANDER] || mdat == &mons[PM_FROST_SALAMANDER] || mdat == &mons[PM_KOMODO_DRAGON] || mdat == &mons[PM_PETTY_KOMODO_DRAGON]) && !rn2(20)) otmp = mksobj_at(LEMON, x, y, TRUE, FALSE);


		if (!rn2(100) && Race_if(PM_ANGBANDER) ) otmp = mksobj_at(SCR_TELEPORTATION, x, y, TRUE, FALSE);
		if (!rn2(100) && Race_if(PM_ANGBANDER) ) otmp = mksobj_at(SCR_IDENTIFY, x, y, TRUE, FALSE);

		if (!rn2(500)) otmp = mksobj_at(SCR_CURE, x, y, TRUE, FALSE);
		if (!rn2(100)) otmp = mksobj_at(SCR_MANA, x, y, TRUE, FALSE);
		if (!rn2(100)) otmp = mksobj_at(SCR_STANDARD_ID, x, y, TRUE, FALSE);
		if (!rn2(40)) otmp = mksobj_at(SCR_HEALING, x, y, TRUE, FALSE);

		if (!rn2( (Race_if(PM_DROW) ? 50 : Race_if(PM_DOPPELGANGER) ? 75 : 16) ) /*&& !(mvitals[mndx].mvflags & G_NOCORPSE) && !(nohands(mdat))*/
	/* lowered overall chance, but see below for a chance to get extra items --Amy
	 * Drow and especially Doppelgangers are super-powerful anyway, so I decided to nerf them a bit. */
#ifdef KOPS
					/*&& mdat->mlet != S_KOP*/
#endif
							) { /* allow death drops for every monster type --Amy */
			int typ;

			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			if (!rn2(32)) {
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE); /* small chance to get even more stuff --Amy */
			}
			if (!rn2(64)) {
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE); /* small chance to get even more stuff --Amy */
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			}
			if (!rn2(128)) {
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE); /* small chance to get even more stuff --Amy */
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			}
			if (!rn2(256)) {
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE); /* small chance to get even more stuff --Amy */
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			}
			if (!rn2(512)) {
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE); /* small chance to get even more stuff --Amy */
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			}
			if (!rn2(1024)) {
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE); /* small chance to get even more stuff --Amy */
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			}
			if (!rn2(2048)) {
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE); /* small chance to get even more stuff --Amy */
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			}
			if (!rn2(4096)) {
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE); /* small chance to get even more stuff --Amy */
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			}
			if (!rn2(8192)) {
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE); /* small chance to get even more stuff --Amy */
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			}
			if (!rn2(16384)) {
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE); /* small chance to get even more stuff --Amy */
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			otmp = mkobj_at(RANDOM_CLASS, x, y, TRUE);
			}
			/* Don't create large objects from small monsters */
			typ = otmp->otyp;
			/*if (mdat->msize < MZ_HUMAN && typ != FOOD_RATION 
			    && typ != LEASH
			    && typ != FIGURINE
			    && (otmp->owt > 3 ||
				objects[typ].oc_big oc_bimanual/oc_bulky ||
				is_spear(otmp) || is_pole(otmp) ||
				typ == MORNING_STAR)) {
			    delobj(otmp);
			} else */redisp = TRUE; /*just always create the object --Amy*/
		}
		/* Whether or not it always makes a corpse is, in theory,
		 * different from whether or not the corpse is "special";
		 * if we want both, we have to specify it explicitly.
		 */
		if (corpse_chance(mtmp, (struct monst *)0, FALSE))
			(void) make_corpse(mtmp);
	}
	/*if(redisp)*/ newsym(x,y);
cleanup:
	/* punish bad behaviour */

	if ( Role_if(PM_LADIESMAN) && !flags.female && mtmp->female && humanoid(mtmp->data) ) {

	    You(Hallucination ? "feel very bad for killing your future wife." : "feel very bad for killing a defenseless woman.");
	    adjalign(-25);
		u.ualign.sins += 1; 
		change_luck(-1);

	}

	/* Batman may not kill women, but you can be a female batman and it would be unfair to only punish males. --Amy */
	if ( Race_if(PM_BATMAN) && ((!flags.female && mtmp->female) || (flags.female && !mtmp->female)) && humanoid(mtmp->data)) {

	    Hallucination ? You("feel very bad for killing your future %s.", flags.female ? "husband" : "wife") : You("feel very bad for killing a defenseless %s.", flags.female ? "man" : "woman");
	    adjalign(-25);
		u.ualign.sins += 1; 
		change_luck(-1);

	}

	if(is_human(mdat) && (!always_hostile(mdat) && mtmp->malign <= 0) &&
	   (mndx < PM_ARCHEOLOGIST || mndx > PM_WIZARD) &&
	   u.ualign.type != A_CHAOTIC) {

		if (always_peaceful(mdat)) { /* being penalized for killing maia, imperials etc. was just stupid. --Amy */
		HTelepat &= ~INTRINSIC;
		change_luck( u.ualign.type == A_LAWFUL ? -2 : -1); /* lower penalty for neutrals --Amy */
		You(Hallucination ? "killed someone you weren't supposed to - whoops!" : "murderer!");

		if (Role_if(PM_PALADIN)) { /* more severe murderer penalties */
			u.ualign.sins += 5; 
			change_luck(-1);
			adjalign(-50);
		}

		if(u.ualign.type == A_LAWFUL) u.ualign.sins += 3; /*fall through*/
		u.ualign.sins += 2; 
		if (Blind && !Blind_telepat)
		    see_monsters(); /* Can't sense monsters any more. */
		}
		adjalign(u.ualign.type == A_LAWFUL ? -2 : -1);

	}
	if((mtmp->mpeaceful && !rn2(2)) || mtmp->mtame)	change_luck(-1);
	if (is_unicorn(mdat) &&
				sgn(u.ualign.type) == sgn(mdat->maligntyp)) {
		change_luck(-5);
		You_feel("guilty...");
	}

	if (Role_if(PM_ACTIVISTOR) && mdat == &mons[PM_TOPMODEL]) { /* very bad idea! --Amy */
		You_feel("guilty for killing an innocent girl.");
		change_luck(-5);
		u.ualign.sins += 10; 
		adjalign(-50);
		u.ugangr++; u.ugangr++; u.ugangr++;
		prayer_done();

	}
	/* give experience points */
	tmp = experience(mtmp, (int)mvitals[mndx].died + 1);
	more_experienced(tmp, 0);
	newexplevel();		/* will decide if you go up */

	/* adjust alignment points */
	if (mtmp->m_id == quest_status.leader_m_id) {		/* REAL BAD! */
	    adjalign(-(u.ualign.record+(int)ALIGNLIM/2));
	    pline("That was %sa bad idea...",
	    		u.uevent.qcompleted ? "probably " : "");
	} else if (mdat->msound == MS_NEMESIS)	/* Real good! */
	    adjalign((int)(ALIGNLIM/4));
	else if (mdat->msound == MS_GUARDIAN) {	/* Bad */
	    adjalign(-(int)(ALIGNLIM/8));
	    if (!Hallucination) pline("That was probably a bad idea...");
	    else pline("Whoopsie-daisy!");
	} else if (mtmp->ispriest) {
		adjalign((p_coaligned(mtmp)) ? -2 : 2);
		/* cancel divine protection for killing your priest */
		if (p_coaligned(mtmp)) u.ublessed = 0;
		if (mdat->maligntyp == A_NONE)
			adjalign((int)(ALIGNLIM / 4));		/* BIG bonus */
	} else if (mtmp->mtame) {
		adjalign(-50);	/* bad!! */
		/* your god is mighty displeased... */
		if (!Hallucination) {(Role_if(PM_PIRATE) || Role_if(PM_KORSAIR)) ? pline("Batten down the hatches!") : You_hear("the rumble of distant thunder...");}
		else You_hear("the studio audience applaud!");
	} else if (mtmp->mpeaceful) {
		adjalign(-15);
		if (!Hallucination) pline("The gods will probably not appreciate this...");
		else pline("Whoopsie-daisy!");
	}

	/* malign was already adjusted for u.ualign.type and randomization */
	adjalign(mtmp->malign);

#ifdef LIVELOG_BONES_KILLER 
	livelog_bones_killed(mtmp); 
#endif 
}

/* changes the monster into a stone monster of the same type */
/* this should only be called when poly_when_stoned() is true */
void
mon_to_stone(mtmp)
    register struct monst *mtmp;
{
    boolean polymorphed = mtmp->oldmonnm != monsndx(mtmp->data);

    if(mtmp->data->mlet == S_GOLEM) {
	/* it's a golem, and not a stone golem */
	if(canseemon(mtmp))
	    pline("%s solidifies...", Monnam(mtmp));
	if (newcham(mtmp, &mons[PM_STONE_GOLEM], FALSE, FALSE)) {
	    if (!polymorphed)
		mtmp->oldmonnm = PM_STONE_GOLEM;    /* Change is permanent */
	    if(canseemon(mtmp))
		pline("Now it's %s.", an(mtmp->data->mname));
	} else {
	    if(canseemon(mtmp))
		pline("... and returns to normal.");
	}
    } else
	impossible("Can't polystone %s!", a_monnam(mtmp));
}

void
mnexto(mtmp)	/* Make monster mtmp next to you (if possible) */
	struct monst *mtmp;
{
	coord mm;

#ifdef STEED
	if (mtmp == u.usteed) {
		/* Keep your steed in sync with you instead */
		mtmp->mx = u.ux;
		mtmp->my = u.uy;
		return;
	}
#endif

	if(!enexto(&mm, u.ux, u.uy, mtmp->data)) return;
	rloc_to(mtmp, mm.x, mm.y);
	return;
}

/* mnearto()
 * Put monster near (or at) location if possible.
 * Returns:
 *	1 - if a monster was moved from x, y to put mtmp at x, y.
 *	0 - in most cases.
 */
boolean
mnearto(mtmp,x,y,move_other)
register struct monst *mtmp;
xchar x, y;
boolean move_other;	/* make sure mtmp gets to x, y! so move m_at(x, y) */
{
	struct monst *othermon = (struct monst *)0;
	xchar newx, newy;
	coord mm;

	if ((mtmp->mx == x) && (mtmp->my == y)) return(FALSE);

	if (move_other && (othermon = m_at(x, y))) {
		if (othermon->wormno)
			remove_worm(othermon);
		else
			remove_monster(x, y);
	}

	newx = x;
	newy = y;

	if (!goodpos(newx, newy, mtmp, 0)) {
		/* actually we have real problems if enexto ever fails.
		 * migrating_mons that need to be placed will cause
		 * no end of trouble.
		 */
		if (!enexto(&mm, newx, newy, mtmp->data)) return(FALSE);
		newx = mm.x; newy = mm.y;
	}

	rloc_to(mtmp, newx, newy);

	if (move_other && othermon) {
	    othermon->mx = othermon->my = 0;
	    (void) mnearto(othermon, x, y, FALSE);
	    if ((othermon->mx != x) || (othermon->my != y))
		return(TRUE);
	}

	return(FALSE);
}


static const char *poiseff[] = {

	" feel weaker", "r brain is on fire",
	"r judgement is impaired", "r muscles won't obey you",
	" feel very sick", " break out in hives"
};

void
poisontell(typ)

	int	typ;
{
	pline("You%s.", poiseff[typ]);
}

void
poisoned(string, typ, pname, fatal)
const char *string, *pname;
int  typ, fatal;
{
	int i, plural, kprefix = KILLED_BY_AN;
	boolean thrown_weapon = (fatal < 0);

	if (thrown_weapon) fatal = -fatal;
	if(strcmp(string, "blast") && !thrown_weapon) {
	    /* 'blast' has already given a 'poison gas' message */
	    /* so have "poison arrow", "poison dart", etc... */
	    plural = (string[strlen(string) - 1] == 's')? 1 : 0;
	    /* avoid "The" Orcus's sting was poisoned... */
	    pline("%s%s %s poisoned!", isupper((int)*string) ? "" : "The ",
			string, plural ? "were" : "was");
	}

	if(Poison_resistance && rn2(20) ) {
		if(!strcmp(string, "blast")) shieldeff(u.ux, u.uy);
		pline_The("poison doesn't seem to affect you.");

		if(!rn2(20)) {
		/* Check that a stat change was made */
		if (adjattrib(typ, -1, 1))
		    pline("You%s!", poiseff[typ]); }
		return;
	}
	/* suppress killer prefix if it already has one */
	if ((i = name_to_mon(pname)) >= LOW_PM && mons[i].geno & G_UNIQ) {
	    kprefix = KILLED_BY;
	    if (!type_is_pname(&mons[i])) pname = the(pname);
	} else if (!strncmpi(pname, "the ", 4) ||
	    !strncmpi(pname, "an ", 3) ||
	    !strncmpi(pname, "a ", 2)) {
	    /*[ does this need a plural check too? ]*/
	    kprefix = KILLED_BY;
	}
	i = rn2(fatal + 20*thrown_weapon);
	if(i == 0 && !Poison_resistance && typ != A_CHA && !rn2(100)) {
		if (Invulnerable || (Stoned_chiller && Stoned))
		   pline("You are unharmed!");
		else {
		u.uhp = -1;
		pline_The("poison was deadly...");
		}
	} else if(i <= 5) {
		/* Check that a stat change was made */
		if (adjattrib(typ, thrown_weapon ? -1 : -rn1(3,3), 1))
		    pline("You%s!", poiseff[typ]);

		/* still does damage --Amy */
		i = thrown_weapon ? rnd(6) : rn1(10,6);
		if(Half_physical_damage && rn2(2) ) i = (i+1) / 2;
		losehp(i, pname, kprefix);

	} else {
		i = thrown_weapon ? rnd(6) : rn1(10,6);
		if(Half_physical_damage && rn2(2) ) i = (i+1) / 2;
		losehp(i, pname, kprefix);
	}
	if(u.uhp < 1) {
		killer_format = kprefix;
		killer = pname;
		/* "Poisoned by a poisoned ___" is redundant */
		done(strstri(pname, "poison") ? DIED : POISONING);
	}
	(void) encumber_msg();
}

/* monster responds to player action; not the same as a passive attack */
/* assumes reason for response has been tested, and response _must_ be made */
void
m_respond(mtmp)
register struct monst *mtmp;
{
    if(mtmp->data->msound == MS_SHRIEK) {
	if(flags.soundok) {
	    pline("%s shrieks.", Monnam(mtmp));
	    stop_occupation();
	}
	/* [Tom] took out the weird purple worm thing and lowered prob from 10 */        
	if (!rn2(8)) {
/*          if (!rn2(13))
		(void) makemon(&mons[PM_PURPLE_WORM], 0, 0, NO_MM_FLAGS);
	    else  */
		(void) makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS);

	}
	aggravate();
    }
    if(mtmp->data == &mons[PM_MEDUSA]) {
	register int i;
	for(i = 0; i < NATTK; i++)
	     if(mtmp->data->mattk[i].aatyp == AT_GAZE) {
		 (void) gazemu(mtmp, &mtmp->data->mattk[i]);
		 break;
	     }
    }
}

#endif /* OVLB */
#ifdef OVL2

void
setmangry(mtmp)
register struct monst *mtmp;
{
	mtmp->mstrategy &= ~STRAT_WAITMASK;
#ifdef BLACKMARKET
	/* Even if the black marketeer is already angry he may not have called
	 * for his assistants if he or his staff have not been assaulted yet.
	 */
	if (Is_blackmarket(&u.uz) && !mtmp->mpeaceful && mtmp->isshk)
	    blkmar_guards(mtmp);
#endif /* BLACKMARKET */
	if(!mtmp->mpeaceful) return;
	if(mtmp->mtame) return;
	mtmp->mpeaceful = 0;
	if(mtmp->ispriest) {
		if(p_coaligned(mtmp)) adjalign(-50); /* very bad */
		else adjalign(2);
	} else {
		adjalign(-5);		/* attacking peaceful monsters is bad */
	}

	if (mtmp->isshk && !strncmpi(shkname(mtmp), "Izchak", 6) ) {

		adjalign(-50);

	}

	if (Role_if(PM_PALADIN)) adjalign(-20);
	if (Role_if(PM_PALADIN) && mtmp->isshk && !strncmpi(shkname(mtmp), "Izchak", 6) ) {

	/* Attacking Izchak is grounds for immediate disintegration. */

		adjalign(-200);

		You_feel("the air around you grow charged...");
		pline("Suddenly, you realize that %s has noticed you...", u_gname());
		/* Throw everything we have at the player */
		god_zaps_you(u.ualign.type);

	}

	if (couldsee(mtmp->mx, mtmp->my)) {
		if (humanoid(mtmp->data) || mtmp->isshk || mtmp->isgd)
		    pline("%s gets angry!", Monnam(mtmp));
		else if (flags.verbose && flags.soundok) growl(mtmp);
	}

#ifdef BLACKMARKET
	/* Don't misbehave in the Black Market or else... */
	if (Is_blackmarket(&u.uz)) {
	    if (mtmp->isshk)
		blkmar_guards(mtmp);
	    else if (NAME(mtmp) && *NAME(mtmp)) {
		/* non-tame named monsters are presumably
		 * black marketeer's assistants */
		struct monst *shkp;
		shkp = shop_keeper(inside_shop(mtmp->mx, mtmp->my));
		if (shkp)  wakeup(shkp);
	    }
	}
#endif /* BLACKMARKET */

	/* attacking your own quest leader will anger his or her guardians */
	if (!flags.mon_moving &&	/* should always be the case here */
		mtmp->data == &mons[quest_info(MS_LEADER)]) {
	    struct monst *mon;
	    struct permonst *q_guardian = &mons[quest_info(MS_GUARDIAN)];
	    int got_mad = 0;

	    /* guardians will sense this attack even if they can't see it */
	    for (mon = fmon; mon; mon = mon->nmon)
		if (!DEADMONSTER(mon) && mon->data == q_guardian && mon->mpeaceful) {
		    mon->mpeaceful = 0;
		    if (canseemon(mon)) ++got_mad;
		}
	    if (got_mad && !Hallucination)
		pline_The("%s appear%s to be angry too...",
		      got_mad == 1 ? q_guardian->mname :
				    makeplural(q_guardian->mname),
		      got_mad == 1 ? "s" : "");
	}
}

void
wakeup(mtmp)
register struct monst *mtmp;
{
	mtmp->msleeping = 0;
	mtmp->meating = 0;	/* assume there's no salvagable food left */
	setmangry(mtmp);
	if(mtmp->m_ap_type) seemimic(mtmp);
	else if (flags.forcefight && !flags.mon_moving && mtmp->mundetected) {
	    mtmp->mundetected = 0;
	    newsym(mtmp->mx, mtmp->my);
	}
}

/* Wake up nearby monsters. */
void
wake_nearby()
{
	register struct monst *mtmp;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (!DEADMONSTER(mtmp) && distu(mtmp->mx,mtmp->my) < level_difficulty()*20) {
		mtmp->msleeping = 0;
		if (mtmp->mtame && !mtmp->isminion)
		    EDOG(mtmp)->whistletime = moves;
	    }
	}
}

/* Wake up monsters near some particular location. */
void
wake_nearto(x, y, distance)
register int x, y, distance;
{
	register struct monst *mtmp;

	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (!DEADMONSTER(mtmp) && mtmp->msleeping && (distance == 0 ||
				 dist2(mtmp->mx, mtmp->my, x, y) < distance))
		mtmp->msleeping = 0;
	}
}

/* NOTE: we must check for mimicry before calling this routine */
void
seemimic(mtmp)
register struct monst *mtmp;
{

	/* can't uncover certain types of monsters --Amy */
	if (mtmp->data == &mons[PM_DEFORMED_FISH] || mtmp->data == &mons[PM_KEYSTONE_INSPECTOR] || mtmp->data == &mons[PM_INSPECTOR_SERGEANT] || mtmp->data == &mons[PM_DEFORMED_LIZARD] || mtmp->data == &mons[PM_MAIDENHACK_HORROR] || mtmp->data == &mons[PM_YASD_HORROR] || mtmp->data == &mons[PM_GRATING_CHICKEN] || mtmp->data == &mons[PM_INSPECTOR_LIEUTENANT] || mtmp->data == &mons[PM_INSPECTOR_KAPTAIN] || mtmp->data == &mons[PM_INSPECTOR_KOMMISSIONER] || mtmp->data == &mons[PM_INSPECTOR_KCHIEF] || mtmp->data == &mons[PM_INSPECTOR_KATCHER] || mtmp->data == &mons[PM_OLOG_HAI_PERMAMIMIC] || mtmp->data == &mons[PM_VESTY] || mtmp->data == &mons[PM_CAMO_RUBBER] || mtmp->data == &mons[PM_WIPER_RUBBER] || mtmp->data == &mons[PM_DRONING_UFO_PART] || mtmp->data == &mons[PM_GNOSIS_SEPHIRAH] || mtmp->data == &mons[PM_HIDDEN_MOLD] || mtmp->data == &mons[PM_SECLUDED_MOLD] || mtmp->data == &mons[PM_HIDDEN_FUNGUS] || mtmp->data == &mons[PM_SECLUDED_FUNGUS] || mtmp->data == &mons[PM_HALLUCINATION_IMAGE] || mtmp->data == &mons[PM_MARSUPILAMI] || mtmp->data == &mons[PM_MAELSTROM] ) return;

	unsigned old_app = mtmp->mappearance;
	uchar old_ap_type = mtmp->m_ap_type;

	mtmp->m_ap_type = M_AP_NOTHING;
	mtmp->mappearance = 0;

	/*
	 *  Discovered mimics don't block light.
	 */
	if (((old_ap_type == M_AP_FURNITURE &&
	      (old_app == S_hcdoor || old_app == S_vcdoor)) ||
	     (old_ap_type == M_AP_OBJECT && old_app == BOULDER)) &&
	    !does_block(mtmp->mx, mtmp->my, &levl[mtmp->mx][mtmp->my]))
	    unblock_point(mtmp->mx, mtmp->my);

	newsym(mtmp->mx,mtmp->my);
}

/* force all chameleons to become normal */
void
rescham()
{
	register struct monst *mtmp;
	int mcham;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (DEADMONSTER(mtmp)) continue;
		if (mtmp->data == &mons[PM_DEFORMED_FISH] || mtmp->data == &mons[PM_KEYSTONE_INSPECTOR] || mtmp->data == &mons[PM_INSPECTOR_SERGEANT] || mtmp->data == &mons[PM_MAIDENHACK_HORROR] || mtmp->data == &mons[PM_YASD_HORROR] || mtmp->data == &mons[PM_DEFORMED_LIZARD] || mtmp->data == &mons[PM_GRATING_CHICKEN] || mtmp->data == &mons[PM_INSPECTOR_LIEUTENANT] || mtmp->data == &mons[PM_INSPECTOR_KAPTAIN] || mtmp->data == &mons[PM_INSPECTOR_KOMMISSIONER] || mtmp->data == &mons[PM_INSPECTOR_KCHIEF] || mtmp->data == &mons[PM_INSPECTOR_KATCHER] || mtmp->data == &mons[PM_OLOG_HAI_PERMAMIMIC] || mtmp->data == &mons[PM_VESTY] || mtmp->data == &mons[PM_CAMO_RUBBER] || mtmp->data == &mons[PM_WIPER_RUBBER] || mtmp->data == &mons[PM_DRONING_UFO_PART] || mtmp->data == &mons[PM_GNOSIS_SEPHIRAH] || mtmp->data == &mons[PM_HIDDEN_MOLD] || mtmp->data == &mons[PM_SECLUDED_MOLD] || mtmp->data == &mons[PM_HIDDEN_FUNGUS] || mtmp->data == &mons[PM_SECLUDED_FUNGUS] || mtmp->data == &mons[PM_HALLUCINATION_IMAGE]|| mtmp->data == &mons[PM_MARSUPILAMI]  || mtmp->data == &mons[PM_MAELSTROM]  ) continue;
		mcham = (int) mtmp->cham;
		if (mcham) {
			mtmp->cham = CHAM_ORDINARY;
			(void) newcham(mtmp, &mons[cham_to_pm[mcham]], FALSE,
				canseemon(mtmp));
		}
		if(is_were(mtmp->data) && mtmp->data->mlet != S_HUMAN)
			new_were(mtmp);
		if(mtmp->m_ap_type && cansee(mtmp->mx, mtmp->my)) {
			seemimic(mtmp);
			/* we pretend that the mimic doesn't */
			/* know that it has been unmasked.   */
			mtmp->msleeping = 1;
		}
	}
}

/* Let the chameleons change again -dgk */
void
restartcham()
{
	register struct monst *mtmp;

	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (DEADMONSTER(mtmp)) continue;
		mtmp->cham = pm_to_cham(monsndx(mtmp->data));
		if((mtmp->data->mlet == S_MIMIC || mtmp->data == &mons[PM_XEROC] || mtmp->data == &mons[PM_NETZAH_SEPHIRAH] || mtmp->data == &mons[PM_MIMIC_MUMMY] || mtmp->data == &mons[PM_OLOG_HAI_MIMIC] || mtmp->data == &mons[PM_CAMO_FISH] || mtmp->data == &mons[PM_FLYING_CAMO_FISH] || mtmp->data == &mons[PM_HUMAN_WEREMIMIC] || mtmp->data == &mons[PM_FATA_MORGANA] || mtmp->data == &mons[PM_CURSED_SPIRIT] || mtmp->data == &mons[PM_DEVILISH_SPIRIT] || mtmp->data == &mons[PM_ALIENATED_UFO_PART] || mtmp->data == &mons[PM_VAMPIRE_SHADOWCLOAK] || mtmp->data == &mons[PM_MULTI_HUED_NAGA] || mtmp->data == &mons[PM_KEYSTONE_WARDER] || mtmp->data == &mons[PM_WARDER_SERGEANT] || mtmp->data == &mons[PM_WARDER_LIEUTENANT] || mtmp->data == &mons[PM_MIMIC_CHICKEN] || mtmp->data == &mons[PM_NETHACKFOUR_HORROR] || mtmp->data == &mons[PM_DEVTEAM_HORROR] || mtmp->data == &mons[PM_MIMIC_LIZARD] || mtmp->data == &mons[PM_WARDER_KAPTAIN] || mtmp->data == &mons[PM_WARDER_KOMMISSIONER] || mtmp->data == &mons[PM_WARDER_KCHIEF] || mtmp->data == &mons[PM_WARDER_KATCHER] || mtmp->data == &mons[PM_MIMIC_VORTEX] || mtmp->data == &mons[PM_VOLTORB] || mtmp->data == &mons[PM_OSCILLATOR] || mtmp->data == &mons[PM_ELECTRODE] || mtmp->data == &mons[PM_HEHEHE_HE_GUY] || mtmp->data == &mons[PM_DISGUISED_SOLDIER_ANT] || mtmp->data == &mons[PM_MIMICRY_RUBBER] || mtmp->data == &mons[PM_AMORPHOUS_FISH]) && mtmp->msleeping &&
				cansee(mtmp->mx, mtmp->my)) {
			set_mimic_sym(mtmp);
			newsym(mtmp->mx,mtmp->my);
		}
	}
}

/* called when restoring a monster from a saved level; protection
   against shape-changing might be different now than it was at the
   time the level was saved. */
void
restore_cham(mon)
struct monst *mon;
{
	int mcham;

	if (Protection_from_shape_changers) {
	    mcham = (int) mon->cham;
	    if (mcham) {
		mon->cham = CHAM_ORDINARY;
		(void) newcham(mon, &mons[cham_to_pm[mcham]], FALSE, FALSE);
	    } else if (is_were(mon->data) && !is_human(mon->data)) {
		new_were(mon);
	    }
	} else if (mon->cham == CHAM_ORDINARY) {
	    mon->cham = pm_to_cham(monsndx(mon->data));
	}
}

/* unwatched hiders may hide again; if so, a 1 is returned.  */
STATIC_OVL boolean
restrap(mtmp)
register struct monst *mtmp;
{
	if(mtmp->cham || mtmp->mcan || mtmp->m_ap_type ||
	   cansee(mtmp->mx, mtmp->my) || rn2(3) || (mtmp == u.ustuck) ||
	   (sensemon(mtmp) && distu(mtmp->mx, mtmp->my) <= 2))
		return(FALSE);

	if(mtmp->data->mlet == S_MIMIC || mtmp->data == &mons[PM_XEROC] || mtmp->data == &mons[PM_NETZAH_SEPHIRAH] || mtmp->data == &mons[PM_MIMIC_MUMMY] || mtmp->data == &mons[PM_OLOG_HAI_MIMIC] || mtmp->data == &mons[PM_CAMO_FISH] || mtmp->data == &mons[PM_FLYING_CAMO_FISH] || mtmp->data == &mons[PM_HUMAN_WEREMIMIC] || mtmp->data == &mons[PM_FATA_MORGANA] || mtmp->data == &mons[PM_CURSED_SPIRIT] || mtmp->data == &mons[PM_DEVILISH_SPIRIT] || mtmp->data == &mons[PM_ALIENATED_UFO_PART] || mtmp->data == &mons[PM_VAMPIRE_SHADOWCLOAK] || mtmp->data == &mons[PM_MULTI_HUED_NAGA] || mtmp->data == &mons[PM_KEYSTONE_WARDER] || mtmp->data == &mons[PM_WARDER_SERGEANT] || mtmp->data == &mons[PM_WARDER_LIEUTENANT] || mtmp->data == &mons[PM_NETHACKFOUR_HORROR] || mtmp->data == &mons[PM_DEVTEAM_HORROR] || mtmp->data == &mons[PM_MIMIC_CHICKEN] || mtmp->data == &mons[PM_MIMIC_LIZARD] || mtmp->data == &mons[PM_WARDER_KAPTAIN] || mtmp->data == &mons[PM_WARDER_KOMMISSIONER] || mtmp->data == &mons[PM_WARDER_KCHIEF] || mtmp->data == &mons[PM_WARDER_KATCHER] || mtmp->data == &mons[PM_MIMIC_VORTEX] || mtmp->data == &mons[PM_VOLTORB] || mtmp->data == &mons[PM_OSCILLATOR] || mtmp->data == &mons[PM_HEHEHE_HE_GUY] || mtmp->data == &mons[PM_ELECTRODE] || mtmp->data == &mons[PM_DISGUISED_SOLDIER_ANT] || mtmp->data == &mons[PM_MIMICRY_RUBBER] || mtmp->data == &mons[PM_AMORPHOUS_FISH]) {
		set_mimic_sym(mtmp);
		return(TRUE);
	} else /* allow piercers and suchlike to hide on all terrain types --Amy */
	    /*if(levl[mtmp->mx][mtmp->my].typ == ROOM)*/  {
		mtmp->mundetected = 1;
		return(TRUE);
	    }

	return(FALSE);
}

short *animal_list = 0;		/* list of PM values for animal monsters */
int animal_list_count;

void
mon_animal_list(construct)
boolean construct;
{
	if (construct) {
	    short animal_temp[SPECIAL_PM];
	    int i, n;

	 /* if (animal_list) impossible("animal_list already exists"); */

	    for (n = 0, i = LOW_PM; i < SPECIAL_PM; i++)
		if (is_animal(&mons[i])) animal_temp[n++] = i;
	 /* if (n == 0) animal_temp[n++] = NON_PM; */

	    animal_list = (short *)alloc(n * sizeof *animal_list);
	    (void) memcpy((genericptr_t)animal_list,
			  (genericptr_t)animal_temp,
			  n * sizeof *animal_list);
	    animal_list_count = n;
	} else {	/* release */
	    if (animal_list) free((genericptr_t)animal_list), animal_list = 0;
	    animal_list_count = 0;
	}
}

STATIC_OVL int
pick_animal()
{
	if (!animal_list) mon_animal_list(TRUE);

	return animal_list[rn2(animal_list_count)];
}

STATIC_OVL int
select_newcham_form(mon)
struct monst *mon;
{
	int mndx = NON_PM;

	switch (mon->cham) {
	    case CHAM_SANDESTIN:
	    case CHAM_CHAOS_SHAPECHANGER:
	    case CHAM_DOPPLEZON:
		if (!rn2(7)) mndx = pick_nasty();
		break;
	    case CHAM_DOPPELGANGER:
	    case CHAM_MISSINGNO:
	    case CHAM_TRANSFORMER:
		if (!rn2(20)) mndx = pick_nasty();
		else if (!rn2(7)) mndx = rn1(PM_WIZARD - PM_ARCHEOLOGIST + 1,
					    PM_ARCHEOLOGIST);
		break;
	    case CHAM_CHAMELEON:
		if (!rn2(7)) mndx = pick_animal();
		break;
	    case CHAM_GIANT_CHAMELEON:
		if (!rn2(7)) mndx = pick_animal();
		break;
	    case CHAM_ORDINARY:
	      {
		struct obj *m_armr = which_armor(mon, W_ARM);

		if (m_armr && Is_dragon_scales(m_armr))
		    mndx = Dragon_scales_to_pm(m_armr) - mons;
		else if (m_armr && Is_dragon_mail(m_armr))
		    mndx = Dragon_mail_to_pm(m_armr) - mons;
	      }
		break;
	}
#ifdef WIZARD
	/* For debugging only: allow control of polymorphed monster; not saved */
	if (wizard && iflags.mon_polycontrol) {
		char pprompt[BUFSZ], buf[BUFSZ];
		int tries = 0;
		do {
			Sprintf(pprompt,
				"Change %s into what kind of monster? [type the name]",
				mon_nam(mon));
			getlin(pprompt,buf);
			mndx = name_to_mon(buf);
			if (mndx < LOW_PM)
				You("cannot polymorph %s into that.", mon_nam(mon));
			else break;
		} while(++tries < 5);
		if (tries==5) pline(thats_enough_tries);
	}
#endif /*WIZARD*/
	if (mndx == NON_PM) mndx = rn1(SPECIAL_PM - LOW_PM, LOW_PM);
	return mndx;
}


/* make a chameleon look like a new monster; returns 1 if it actually changed */
/* [ALI] Special case: Don't print a message if hero can neither spot the
 * original _or_ the new monster (avoids "It turns into it!").
 */
int
newcham(mtmp, mdat, polyspot, msg)
struct monst *mtmp;
struct permonst *mdat;
boolean polyspot;	/* change is the result of wand or spell of polymorph */
boolean msg;
{
	int mhp, hpn, hpd;
	int mndx, tryct;
	int couldsee = canseemon(mtmp);
	struct permonst *olddata = mtmp->data;
	char oldname[BUFSZ];
	boolean alt_mesg = FALSE;	/* Avoid "<rank> turns into a <rank>" */

	if (msg) {
	    /* like Monnam() but never mention saddle */
	    Strcpy(oldname, x_monnam(mtmp, ARTICLE_THE, (char *)0,
				     SUPPRESS_SADDLE, FALSE));
	    oldname[0] = highc(oldname[0]);
	}

	/* mdat = 0 -> caller wants a random monster shape */
	tryct = 0;
	if(mdat == 0) {
	    while (++tryct <= 100) {
		mndx = select_newcham_form(mtmp);
		mdat = &mons[mndx];
		if ((mvitals[mndx].mvflags & G_GENOD) != 0 ||
			is_placeholder(mdat)) continue;
		/* polyok rules out all M2_PNAME and M2_WERE's;
		   select_newcham_form might deliberately pick a player
		   character type, so we can't arbitrarily rule out all
		   human forms any more */
		if (is_mplayer(mdat) || is_umplayer(mdat) || (!is_human(mdat) && monpolyok(mdat)))
		    break;
	    }
	    if (tryct > 100) return 0;	/* Should never happen */
	} else if (mvitals[monsndx(mdat)].mvflags & G_GENOD)
	    return(0);	/* passed in mdat is genocided */

	if(is_male(mdat)) {
		if(mtmp->female) mtmp->female = FALSE;
	} else if (is_female(mdat)) {
		if(!mtmp->female) mtmp->female = TRUE;
	} else if (!is_neuter(mdat)) {
		if(!rn2(10)) mtmp->female = !mtmp->female;
	}

	if (In_endgame(&u.uz) && is_mplayer(olddata)) {
		/* mplayers start out as "Foo the Bar", but some of the
		 * titles are inappropriate when polymorphed, particularly
		 * into the opposite sex.  players don't use ranks when
		 * polymorphed, so dropping the rank for mplayers seems
		 * reasonable.
		 */
		char *p = index(NAME(mtmp), ' ');
		if (p) {
			*p = '\0';
			mtmp->mnamelth = p - NAME(mtmp) + 1;
		}
	}

	if(mdat == mtmp->data) return(0);	/* still the same monster */

	/* [ALI] Detect transforming between player monsters with the
	 * same rank title to avoid badly formed messages.
	 * Similarly for were creatures transforming to their alt. form.
	 */
	if (msg && is_mplayer(olddata) && is_mplayer(mdat)) {
	    const struct Role *role;
	    int i, oldmndx;

	    mndx = monsndx(mdat);
	    oldmndx = monsndx(olddata);
	    for (role = roles; role->name.m; role++) {
		if (role->femalenum == NON_PM)
		    continue;
		if ((mndx == role->femalenum && oldmndx == role->malenum) ||
			(mndx == role->malenum && oldmndx == role->femalenum)) {
		    /* Find the rank */
		    for (i = xlev_to_rank(mtmp->m_lev); i >= 0; i--)
			if (role->rank[i].m) {
			    /* Only need alternate message if no female form */
			    alt_mesg = !role->rank[i].f;
			    break;
			}
		}
	    }
	} else if (msg && is_were(olddata) &&
		monsndx(mdat) == counter_were(monsndx(olddata)))
	    alt_mesg = TRUE;

	/* WAC - At this point,  the transformation is going to happen */
	/* Reset values, remove worm tails, change levels...etc. */

	if(mtmp->wormno) {			/* throw tail away */
		wormgone(mtmp);
		place_monster(mtmp, mtmp->mx, mtmp->my);
	}

	hpn = mtmp->mhp;
	hpd = (mtmp->m_lev < 50) ? ((int)mtmp->m_lev)*8 : mdat->mlevel;
	if(!hpd) hpd = 4;

	mtmp->m_lev = adj_lev(mdat);		/* new monster level */

	mhp = (mtmp->m_lev < 50) ? ((int)mtmp->m_lev)*8 : mdat->mlevel;
	if(!mhp) mhp = 4;

	/* new hp: same fraction of max as before */
#ifndef LINT
	mtmp->mhp = (int)(((long)hpn*(long)mhp)/(long)hpd);
#endif
	if(mtmp->mhp < 0) mtmp->mhp = hpn;	/* overflow */
/* Unlikely but not impossible; a 1HD creature with 1HP that changes into a
   0HD creature will require this statement */
	if (!mtmp->mhp) mtmp->mhp = 1;

/* and the same for maximum hit points */
	hpn = mtmp->mhpmax;
#ifndef LINT
	mtmp->mhpmax = (int)(((long)hpn*(long)mhp)/(long)hpd);
#endif
	if(mtmp->mhpmax < 0) mtmp->mhpmax = hpn;	/* overflow */
	if (!mtmp->mhpmax) mtmp->mhpmax = 1;

	/* take on the new form... */
	set_mon_data(mtmp, mdat, 0);

	if (emits_light(olddata) != emits_light(mtmp->data)) {
	    /* used to give light, now doesn't, or vice versa,
	       or light's range has changed */
	    if (emits_light(olddata))
		del_light_source(LS_MONSTER, (genericptr_t)mtmp);
	    if (emits_light(mtmp->data))
		new_light_source(mtmp->mx, mtmp->my, emits_light(mtmp->data),
				 LS_MONSTER, (genericptr_t)mtmp);
	}
	if (!mtmp->perminvis || pm_invisible(olddata))
	    mtmp->perminvis = pm_invisible(mdat);
	mtmp->minvis = mtmp->invis_blkd ? 0 : mtmp->perminvis;
	if (!(hides_under(mdat) && OBJ_AT(mtmp->mx, mtmp->my)) &&
			!(mdat->mlet == S_EEL && is_pool(mtmp->mx, mtmp->my)))
		mtmp->mundetected = 0;
#ifdef STEED
	if (u.usteed) {
	    if (touch_petrifies(u.usteed->data) &&
	    		!Stone_resistance && rnl(3)) {
	    	char buf[BUFSZ];

	    	pline("You touch %s.", mon_nam(u.usteed));
	    	Sprintf(buf, "riding %s", an(u.usteed->data->mname));
	    	instapetrify(buf);
 	    }
	    if (!can_ride(u.usteed)) dismount_steed(DISMOUNT_POLY);
	}
#endif

#ifndef DCC30_BUG
	if (mdat == &mons[PM_LONG_WORM] && (mtmp->wormno = get_wormno()) != 0) {
#else
	/* DICE 3.0 doesn't like assigning and comparing mtmp->wormno in the
	 * same expression.
	 */
	if (mdat == &mons[PM_LONG_WORM] &&
		(mtmp->wormno = get_wormno(), mtmp->wormno != 0)) {
#endif
	    /* we can now create worms with tails - 11/91 */
	    initworm(mtmp, rn2(5));
	    if (count_wsegs(mtmp))
		place_worm_tail_randomly(mtmp, mtmp->mx, mtmp->my);
	}

	newsym(mtmp->mx,mtmp->my);

	if (msg && (u.uswallow && mtmp == u.ustuck || canspotmon(mtmp))) {
	    if (alt_mesg && is_mplayer(mdat))
		pline("%s is suddenly very %s!", oldname,
			mtmp->female ? "feminine" : "masculine");
	    else if (alt_mesg)
		pline("%s changes into a %s!", oldname,
			is_human(mdat) ? "human" : mdat->mname + 4);
	    else {
	    uchar save_mnamelth = mtmp->mnamelth;
	    mtmp->mnamelth = 0;
	    pline("%s turns into %s!", oldname,
		  mdat == &mons[PM_GREEN_SLIME] ? "slime" :
		  x_monnam(mtmp, ARTICLE_A, (char*)0, SUPPRESS_SADDLE, FALSE));
	    mtmp->mnamelth = save_mnamelth;
	    }
	} else if (msg && couldsee)
	    /* No message if we only sensed the monster previously */
	    pline("%s suddenly disappears!", oldname);

	/* [ALI] In Slash'EM, this must come _after_ "<mon> turns into <mon>"
	 * since it's possible to get both messages.
	 */
	if (u.ustuck == mtmp) {
		if(u.uswallow) {
			if(!attacktype(mdat,AT_ENGL)) {
				/* Does mdat care? */
				if (!noncorporeal(mdat) && !amorphous(mdat) &&
				    !is_whirly(mdat) &&
				    (mdat != &mons[PM_YELLOW_LIGHT])) {
					You("break out of %s%s!", mon_nam(mtmp),
					    (is_animal(mdat)?
					    "'s stomach" : ""));
					mtmp->mhp = 1;  /* almost dead */
				}
				expels(mtmp, olddata, FALSE);
			} else {
				/* update swallow glyphs for new monster */
				swallowed(0);
			}
		} else if (!sticks(mdat) && !sticks(youmonst.data))
			unstuck(mtmp);
	}

	possibly_unwield(mtmp, polyspot);	/* might lose use of weapon */
	mon_break_armor(mtmp, polyspot);
	if (!(mtmp->misc_worn_check & W_ARMG))
	    mselftouch(mtmp, "No longer petrify-resistant, ",
			!flags.mon_moving);
	m_dowear(mtmp, FALSE);

	/* This ought to re-test can_carry() on each item in the inventory
	 * rather than just checking ex-giants & boulders, but that'd be
	 * pretty expensive to perform.  If implemented, then perhaps
	 * minvent should be sorted in order to drop heaviest items first.
	 */
	/* former giants can't continue carrying boulders */
	if (mtmp->minvent && !throws_rocks(mdat)) {
	    register struct obj *otmp, *otmp2;

	    for (otmp = mtmp->minvent; otmp; otmp = otmp2) {
		otmp2 = otmp->nobj;
		if (otmp->otyp == BOULDER) {
		    /* this keeps otmp from being polymorphed in the
		       same zap that the monster that held it is polymorphed */
		    if (polyspot) bypass_obj(otmp);
		    obj_extract_self(otmp);
		    /* probably ought to give some "drop" message here */
		    if (flooreffects(otmp, mtmp->mx, mtmp->my, "")) continue;
		    place_object(otmp, mtmp->mx, mtmp->my);
		}
	    }
	}

	return(1);
}

/* sometimes an egg will be special */
#define BREEDER_EGG (!rn2(77))

/*
 * Determine if the given monster number can be hatched from an egg.
 * Return the monster number to use as the egg's corpsenm.  Return
 * NON_PM if the given monster can't be hatched.
 */
int
can_be_hatched(mnum)
int mnum;
{
    /* ranger quest nemesis has the oviparous bit set, making it
       be possible to wish for eggs of that unique monster; turn
       such into ordinary eggs rather than forbidding them outright */
    if (mnum == PM_SCORPIUS) mnum = PM_SCORPION;

    mnum = little_to_big(mnum);
    /*
     * Queen bees lay killer bee eggs (usually), but killer bees don't
     * grow into queen bees.  Ditto for [winged-]gargoyles.
     */
    if (mnum == PM_KILLER_BEE || mnum == PM_GARGOYLE ||
	    (lays_eggs(&mons[mnum]) && (BREEDER_EGG ||
		(mnum != PM_QUEEN_BEE && mnum != PM_WINGED_GARGOYLE))))
	return mnum;
    return NON_PM;
}

/* type of egg laid by #sit; usually matches parent */
int
egg_type_from_parent(mnum, force_ordinary)
int mnum;	/* parent monster; caller must handle lays_eggs() check */
boolean force_ordinary;
{
    if (force_ordinary || !BREEDER_EGG) {
	if (mnum == PM_QUEEN_BEE) mnum = PM_KILLER_BEE;
	else if (mnum == PM_WINGED_GARGOYLE) mnum = PM_GARGOYLE;
    }
    return mnum;
}

/* decide whether an egg of the indicated monster type is viable; */
/* also used to determine whether an egg or tin can be created... */
boolean
dead_species(m_idx, egg)
int m_idx;
boolean egg;
{
	/*
	 * For monsters with both baby and adult forms, genociding either
	 * form kills all eggs of that monster.  Monsters with more than
	 * two forms (small->large->giant mimics) are more or less ignored;
	 * fortunately, none of them have eggs.  Species extinction due to
	 * overpopulation does not kill eggs.
	 */
	return (boolean)
		(m_idx >= LOW_PM &&
		 ((mvitals[m_idx].mvflags & G_GENOD) != 0 ||
		  (egg &&
		   (mvitals[big_to_little(m_idx)].mvflags & G_GENOD) != 0)));
}

/* kill off any eggs of genocided monsters */
STATIC_OVL void
kill_eggs(obj_list)
struct obj *obj_list;
{
	struct obj *otmp;

	for (otmp = obj_list; otmp; otmp = otmp->nobj)
	    if (otmp->otyp == EGG) {
		if (dead_species(otmp->corpsenm, TRUE)) {
		    /*
		     * It seems we could also just catch this when
		     * it attempted to hatch, so we wouldn't have to
		     * search all of the objlists.. or stop all
		     * hatch timers based on a corpsenm.
		     */
		    kill_egg(otmp);
		}
#if 0	/* not used */
	    } else if (otmp->otyp == TIN) {
		if (dead_species(otmp->corpsenm, FALSE))
		    otmp->corpsenm = NON_PM;	/* empty tin */
	    } else if (otmp->otyp == CORPSE) {
		if (dead_species(otmp->corpsenm, FALSE))
		    ;		/* not yet implemented... */
#endif
	    } else if (Has_contents(otmp)) {
		kill_eggs(otmp->cobj);
	    }
}

/* kill all members of genocided species */
void
kill_genocided_monsters()
{
	struct monst *mtmp, *mtmp2;
	boolean kill_cham[CHAM_MAX_INDX+1];
	int mndx;

	kill_cham[CHAM_ORDINARY] = FALSE;	/* (this is mndx==0) */
	for (mndx = 1; mndx <= CHAM_MAX_INDX; mndx++)
	  kill_cham[mndx] = (mvitals[cham_to_pm[mndx]].mvflags & G_GENOD) != 0;
	/*
	 * Called during genocide, and again upon level change.  The latter
	 * catches up with any migrating monsters as they finally arrive at
	 * their intended destinations, so possessions get deposited there.
	 *
	 * Chameleon handling:
	 *	1) if chameleons have been genocided, destroy them
	 *	   regardless of current form;
	 *	2) otherwise, force every chameleon which is imitating
	 *	   any genocided species to take on a new form.
	 */
	for (mtmp = fmon; mtmp; mtmp = mtmp2) {
	    mtmp2 = mtmp->nmon;
	    if (DEADMONSTER(mtmp)) continue;
	    mndx = monsndx(mtmp->data);
	    if ((mvitals[mndx].mvflags & G_GENOD) || kill_cham[mtmp->cham]) {
		if (mtmp->cham && !kill_cham[mtmp->cham])
		    /* [ALI] Chameleons are not normally subject to
		     * system shock, but genocide is a special case.
		     */
		    (void) mon_spec_poly(mtmp, (struct permonst *)0, 0L,
			    FALSE, FALSE, TRUE, TRUE);
		else
		    mondead(mtmp);
	    }
	    if (mtmp->minvent) kill_eggs(mtmp->minvent);
	}

	kill_eggs(invent);
	kill_eggs(fobj);
	kill_eggs(level.buriedobjlist);
}



#endif /* OVL2 */
#ifdef OVLB

void
golemeffects(mon, damtype, dam)
register struct monst *mon;
int damtype, dam;
{
	int heal=0, slow=0;

    if (mon->data == &mons[PM_FLESH_GOLEM]) {
	if (damtype == AD_ELEC) heal = dam / 6;
	else if (damtype == AD_FIRE || damtype == AD_COLD) slow = 1;
    } else if (mon->data == &mons[PM_IRON_GOLEM]) {
	if (damtype == AD_ELEC) slow = 1;
	else if (damtype == AD_FIRE) heal = dam;
    } else {
	return;
    }
    if (slow) {
	if (mon->mspeed != MSLOW)
	    mon_adjust_speed(mon, -1, (struct obj *)0);
    }
    if (heal) {
	if (mon->mhp < mon->mhpmax) {
	    mon->mhp += dam;
	    if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
	    if (cansee(mon->mx, mon->my))
		pline("%s seems healthier.", Monnam(mon));
	}
    }
}

boolean
angry_guards(silent)
register boolean silent;
{
	register struct monst *mtmp;
	register int ct = 0, nct = 0, sct = 0, slct = 0;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (DEADMONSTER(mtmp)) continue;
		if((mtmp->data == &mons[PM_WATCHMAN] || mtmp->data == &mons[PM_WATCH_LIEUTENANT] ||
			       mtmp->data == &mons[PM_WATCH_CAPTAIN] || mtmp->data == &mons[PM_WATCH_LEADER])
					&& mtmp->mpeaceful) {
			ct++;
			if(cansee(mtmp->mx, mtmp->my) && mtmp->mcanmove) {
				if (distu(mtmp->mx, mtmp->my) == 2) nct++;
				else sct++;
			}
			if (mtmp->msleeping || mtmp->mfrozen) {
				slct++;
				mtmp->msleeping = mtmp->mfrozen = 0;
			}
			mtmp->mpeaceful = 0;
		}
	}
	if(ct) {
	    if(!silent) { /* do we want pline msgs? */
		if(slct) pline_The("guard%s wake%s up!",
				 slct > 1 ? "s" : "", slct == 1 ? "s" : "");
		if(nct || sct) {
			if(nct) pline_The("guard%s get%s angry!",
				nct == 1 ? "" : "s", nct == 1 ? "s" : "");
			else if(!Blind)
				You("see %sangry guard%s approaching!",
				  sct == 1 ? "an " : "", sct > 1 ? "s" : "");
		} else if(flags.soundok)
			You_hear("the shrill sound of a guard's whistle.");
	    }
	    return(TRUE);
	}
	return(FALSE);
}

void
pacify_guards()
{
	register struct monst *mtmp;

	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if (mtmp->data == &mons[PM_WATCHMAN] || mtmp->data == &mons[PM_WATCH_LIEUTENANT] ||
		mtmp->data == &mons[PM_WATCH_CAPTAIN] || mtmp->data == &mons[PM_WATCH_LEADER])
	    mtmp->mpeaceful = 1;
	}
}

boolean
damage_mon(mon,amount,type)
struct monst* mon;
int amount;
int type;
{
	mon->mhp -= amount;
	return (mon->mhp < 1);
}

void
mimic_hit_msg(mtmp, otyp)
struct monst *mtmp;
short otyp;
{
	short ap = mtmp->mappearance;

	switch(mtmp->m_ap_type) {
	    case M_AP_NOTHING:			
	    case M_AP_FURNITURE:
	    case M_AP_MONSTER:
		break;
	    case M_AP_OBJECT:
		if (otyp == SPE_HEALING || otyp == SPE_EXTRA_HEALING || otyp == SPE_FULL_HEALING) {
		    pline("%s seems a more vivid %s than before.",
				The(simple_typename(ap)),
				c_obj_colors[objects[ap].oc_color]);
		}
		break;
	}
}
#endif /* OVLB */

/*mon.c*/
