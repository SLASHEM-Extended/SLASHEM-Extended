/*	SCCS Id: @(#)makemon.c	3.4	2003/09/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "epri.h"
#include "emin.h"
#include "edog.h"
#include "quest.h"
#ifdef REINCARNATION
#include <ctype.h>
#endif

STATIC_VAR NEARDATA struct monst zeromonst;

/* this assumes that a human quest leader or nemesis is an archetype
   of the corresponding role; that isn't so for some roles (tourist
   for instance) but is for the priests and monks we use it for... */
#define quest_mon_represents_role(mptr,role_pm) \
		(mptr->mlet == S_HUMAN && Role_if(role_pm) && \
		  (mptr->msound == MS_LEADER || mptr->msound == MS_NEMESIS))

#ifdef OVL0
STATIC_DCL boolean FDECL(uncommon, (int));
STATIC_DCL int FDECL(align_shift, (struct permonst *));
#endif /* OVL0 */
STATIC_DCL boolean FDECL(wrong_elem_type, (struct permonst *));
STATIC_DCL void FDECL(m_initgrp,(struct monst *,int,int,int));
STATIC_DCL void FDECL(m_initthrow,(struct monst *,int,int));
STATIC_DCL void FDECL(m_initweap,(struct monst *));
STATIC_DCL void FDECL(m_initxtraitem,(struct monst *));
STATIC_DCL void FDECL(m_initweap_normal,(struct monst *));
#ifdef OVL1
STATIC_DCL void FDECL(m_initinv,(struct monst *));
#endif /* OVL1 */

extern const int monstr[];

#define m_initsgrp(mtmp, x, y)	m_initgrp(mtmp, x, y, 3)
#define m_initlgrp(mtmp, x, y)	m_initgrp(mtmp, x, y, 10)
#define m_initvlgrp(mtmp, x, y)  m_initgrp(mtmp, x, y, 20)
#define m_initxlgrp(mtmp, x, y)  m_initgrp(mtmp, x, y, 40)
#define m_initxxlgrp(mtmp, x, y)  m_initgrp(mtmp, x, y, 100)
#define toostrong(monindx, lev) (monstr[monindx] > lev)
#define tooweak(monindx, lev)	(monstr[monindx] < lev)
#define Qstats(x)	(quest_status.x)

#ifdef OVLB
boolean
is_home_elemental(ptr)
register struct permonst *ptr;
{
	if (ptr->mlet == S_ELEMENTAL)
	    switch (monsndx(ptr)) {
		case PM_AIR_ELEMENTAL: return Is_airlevel(&u.uz);
		case PM_FIRE_ELEMENTAL: return Is_firelevel(&u.uz);
		case PM_EARTH_ELEMENTAL: return Is_earthlevel(&u.uz);
		case PM_WATER_ELEMENTAL: return Is_waterlevel(&u.uz);
		case PM_ASTRAL_ELEMENTAL: return Is_astralevel(&u.uz);
	    }
	return FALSE;
}

/*
 * Return true if the given monster cannot exist on this elemental level.
 */
STATIC_OVL boolean
wrong_elem_type(ptr)
    register struct permonst *ptr;
{
    if (ptr->mlet == S_ELEMENTAL) {
	return((boolean)(!is_home_elemental(ptr)));
    } else if (Is_earthlevel(&u.uz)) {
	/* no restrictions? */
    } else if (Is_waterlevel(&u.uz)) {
	/* just monsters that can swim */
	if(!is_swimmer(ptr)) return TRUE;
    } else if (Is_firelevel(&u.uz)) {
	if (!pm_resistance(ptr,MR_FIRE)) return TRUE;
    } else if (Is_airlevel(&u.uz)) {
	if(!(is_flyer(ptr) && ptr->mlet != S_TRAPPER) && !is_floater(ptr)
	   && !amorphous(ptr) && !noncorporeal(ptr) && !is_whirly(ptr))
	    return TRUE;
    }
    return FALSE;
}

STATIC_OVL void
m_initgrp(mtmp, x, y, n)	/* make a group just like mtmp */
register struct monst *mtmp;
register int x, y, n;
{
	coord mm;
	register int cnt = rnd(n);
	int kindred = rn2(20) ? 0 : 1;
	if (!(u.monstertimefinish % 137) ) kindred = rn2(5) ? 0 : 1;
	if (!(u.monstertimefinish % 837) ) kindred = rn2(2) ? 0 : 1;
	if (!(u.monstertimefinish % 4337) ) kindred = rn2(20) ? 1 : 0;

	if (mtmp->data->geno & G_PLATOON) kindred = 1;
	if (mtmp->data->geno & G_UNIQ) kindred = 1; /* uniques are created with others of their kin, instead of clones of themselves */
	struct monst *mon;
#if defined(__GNUC__) && (defined(HPUX) || defined(DGUX))
	/* There is an unresolved problem with several people finding that
	 * the game hangs eating CPU; if interrupted and restored, the level
	 * will be filled with monsters.  Of those reports giving system type,
	 * there were two DG/UX and two HP-UX, all using gcc as the compiler.
	 * hcroft@hpopb1.cern.ch, using gcc 2.6.3 on HP-UX, says that the
	 * problem went away for him and another reporter-to-newsgroup
	 * after adding this debugging code.  This has almost got to be a
	 * compiler bug, but until somebody tracks it down and gets it fixed,
	 * might as well go with the "but it went away when I tried to find
	 * it" code.
	 */
	int cnttmp,cntdiv;

	cnttmp = cnt;
# ifdef DEBUG
	pline("init group call x=%d,y=%d,n=%d,cnt=%d.", x, y, n, cnt);
# endif
	cntdiv = ((u.ulevel < 3) ? 4 : (u.ulevel < 5) ? 2 : 1);
#endif
	/* Tuning: cut down on swarming at low character levels [mrs] */
	if ((/*u.ulevel*/level_difficulty() < 5) && rn2(5) ) cnt /= 2;
#if defined(__GNUC__) && (defined(HPUX) || defined(DGUX))
	if (cnt != (cnttmp/cntdiv)) {
		pline("cnt=%d using %d, cnttmp=%d, cntdiv=%d", cnt,
			(u.ulevel < 3) ? 4 : (u.ulevel < 5) ? 2 : 1,
			cnttmp, cntdiv);
	}
#endif
	if(!cnt) cnt++;
#if defined(__GNUC__) && (defined(HPUX) || defined(DGUX))
	if (cnt < 0) cnt = 1;
	if (cnt > 10) cnt = 10;
#endif

	mm.x = x;
	mm.y = y;
	while(cnt--) {
		if (peace_minded(mtmp->data)) continue;
		/* Don't create groups of peaceful monsters since they'll get
		 * in our way.  If the monster has a percentage chance so some
		 * are peaceful and some are not, the result will just be a
		 * smaller group.
		 */
		if (enexto(&mm, mm.x, mm.y, mtmp->data)) {
		    if (!kindred) mon = makemon(mtmp->data, mm.x, mm.y, NO_MM_FLAGS);
		    else mon = makemon(mkclass(mtmp->data->mlet,0), mm.x, mm.y, NO_MM_FLAGS);
			if (mon) {
		    mon->mpeaceful = FALSE;
		    mon->mavenge = 0;
		    set_malign(mon);
			}
		    /* Undo the second peace_minded() check in makemon(); if the
		     * monster turned out to be peaceful the first time we
		     * didn't create it at all; we don't want a second check.
		     */
		}
	}
}

STATIC_OVL
void
m_initthrow(mtmp,otyp,oquan)
struct monst *mtmp;
int otyp,oquan;
{
	register struct obj *otmp;

	otmp = mksobj(otyp, TRUE, FALSE);
	if (!otmp) return;
	otmp->quan = (long) rn1(oquan, 3);
	if (!issoviet) {
		if (!rn2(20)) otmp->quan += rnd(otmp->quan);
		if (!rn2(1000)) otmp->quan += rnz(otmp->quan + 3);
		if (!rn2(1000)) otmp->quan += rnz( rnd( (otmp->quan * 2) + 3) );
	}
	otmp->owt = weight(otmp);
	if (otyp == ORCISH_ARROW) otmp->opoisoned = TRUE;
	if (otmp->oclass == WEAPON_CLASS) otmp->mstartinventB = 1;
	(void) mpickobj(mtmp, otmp, TRUE);
}

#endif /* OVLB */
#ifdef OVL2

STATIC_OVL void
m_initweap_normal(mtmp)
register struct monst *mtmp;
{
	register struct permonst *ptr = mtmp->data;
	int bias;

	bias = (is_lord(ptr) * 2) + (is_prince(ptr) * 5) + (extra_nasty(ptr) * 3) + strongmonst(ptr);
	switch(rnd(25 - (3 * bias))) {
	    case 1:
		if(strongmonst(ptr)) (void) mongets(mtmp, rn2(20) ? BATTLE_AXE : TRIDENT);
		else if (rn2(2)) m_initthrow(mtmp, rn2(200) ? DART : SPIKE, 20);
		else (void) mongets(mtmp, KNIFE);
		break;
	    case 2:
		if(strongmonst(ptr))
		    (void) mongets(mtmp, rn2(20) ? TWO_HANDED_SWORD : QUARTERSTAFF);
		else if (rn2(2)) {
		    (void) mongets(mtmp, rn2(20) ? CROSSBOW : !rn2(3) ? POWER_CROSSBOW : !rn2(4) ? (rn2(2) ? PILE_BUNKER : HELO_CROSSBOW) : rn2(50) ? DROVEN_CROSSBOW : DEMON_CROSSBOW);
		    m_initthrow(mtmp, rn2(20) ? CROSSBOW_BOLT : rn2(5) ? DROVEN_BOLT : KOKKEN, 30);
		}
		else {
		    (void) mongets(mtmp, rn2(500) ? SLING : CATAPULT);
		    m_initthrow(mtmp, rn2(100) ? ROCK : FLINT, 10);
		}
		break;
	    case 3:
		if (rn2(20)) {
			(void) mongets(mtmp, rn2(1000) ? BOW : HYDRA_BOW);
			if (rn2(250)) m_initthrow(mtmp, ARROW, 25);
			else switch (rnd(5)) {
				case 1:
					m_initthrow(mtmp, SILVER_ARROW, 25);
					break;
				case 2:
					m_initthrow(mtmp, GOLDEN_ARROW, 25);
					break;
				case 3:
					m_initthrow(mtmp, ANCIENT_ARROW, 25);
					break;
				case 4:
					m_initthrow(mtmp, DROVEN_ARROW, 25);
					break;
				case 5:
					m_initthrow(mtmp, YA, 25);
					break;
			}
		}
		else if (rn2(200)) {
			(void) mongets(mtmp, rn2(20) ? PISTOL : RIFLE);
			m_initthrow(mtmp, rn2(2000) ? BULLET : ANTIMATTER_BULLET, 25);
		}
		else switch (rnd(100)) {
			case 1:
				(void) mongets(mtmp, HEAVY_MACHINE_GUN);
				m_initthrow(mtmp, rn2(2000) ? BULLET : ANTIMATTER_BULLET, 50);
				m_initthrow(mtmp, rn2(2000) ? BULLET : ANTIMATTER_BULLET, 50);
				break;
			case 2:
				(void) mongets(mtmp, AUTO_SHOTGUN);
				m_initthrow(mtmp, SHOTGUN_SHELL, 50);
				break;
			case 3:
				(void) mongets(mtmp, ARM_BLASTER);
				m_initthrow(mtmp, rn2(20) ? BLASTER_BOLT : HEAVY_BLASTER_BOLT, 50);
				break;
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				(void) mongets(mtmp, HAND_BLASTER);
				m_initthrow(mtmp, rn2(20) ? BLASTER_BOLT : HEAVY_BLASTER_BOLT, 40);
				break;
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				(void) mongets(mtmp, CUTTING_LASER);
				m_initthrow(mtmp, LASER_BEAM, 50);
				break;
			case 16:
			case 17:
			case 18:
				(void) mongets(mtmp, RAYGUN);
				m_initthrow(mtmp, LASER_BEAM, 50);
				break;
			case 19:
				(void) mongets(mtmp, ROCKET_LAUNCHER);
				m_initthrow(mtmp, ROCKET, 5);
				break;
			case 20:
				(void) mongets(mtmp, GRENADE_LAUNCHER);
				m_initthrow(mtmp, FRAG_GRENADE, 15);
				break;
			case 21:
				(void) mongets(mtmp, GRENADE_LAUNCHER);
				m_initthrow(mtmp, GAS_GRENADE, 15);
				break;
			case 22:
				(void) mongets(mtmp, SAWED_OFF_SHOTGUN);
				m_initthrow(mtmp, SHOTGUN_SHELL, 25);
				break;
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
			case 28:
			case 29:
			case 30:
			case 31:
			case 32:
			case 33:
			case 34:
			case 35:
			case 36:
				(void) mongets(mtmp, SHOTGUN);
				m_initthrow(mtmp, SHOTGUN_SHELL, 25);
				break;
			case 37:
			case 38:
			case 39:
			case 40:
			case 41:
			case 42:
			case 43:
			case 44:
			case 45:
			case 46:
			case 47:
			case 48:
			case 49:
			case 50:
				(void) mongets(mtmp, ASSAULT_RIFLE);
				m_initthrow(mtmp, rn2(2000) ? BULLET : ANTIMATTER_BULLET, 50);
				m_initthrow(mtmp, rn2(2000) ? BULLET : ANTIMATTER_BULLET, 25);
				break;
			case 51:
			case 52:
			case 53:
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 64:
			case 65:
			case 66:
			case 67:
			case 68:
			case 69:
			case 70:
			case 71:
			case 72:
			case 73:
			case 74:
			case 75:
				(void) mongets(mtmp, SNIPER_RIFLE);
				m_initthrow(mtmp, rn2(2000) ? BULLET : ANTIMATTER_BULLET, 50);
				break;
			case 76:
			case 77:
			case 78:
			case 79:
			case 80:
			case 81:
			case 82:
			case 83:
			case 84:
			case 85:
			case 86:
			case 87:
			case 88:
			case 89:
			case 90:
			case 91:
			case 92:
			case 93:
			case 94:
			case 95:
			case 96:
			case 97:
			case 98:
			case 99:
			case 100:
				(void) mongets(mtmp, SUBMACHINE_GUN);
				m_initthrow(mtmp, rn2(2000) ? BULLET : ANTIMATTER_BULLET, 50);
				break;
		}

		break;
	    case 4:
		if(strongmonst(ptr)) (void) mongets(mtmp, rn2(20) ? LONG_SWORD : BROADSWORD);
		else if (rn2(2)) m_initthrow(mtmp, DAGGER, 3);
		else m_initthrow(mtmp, KNIFE, 1);
		break;
	    case 5:
		if(strongmonst(ptr))
		    (void) mongets(mtmp, rn2(20) ? LUCERN_HAMMER : BEC_DE_CORBIN);
		else if (rn2(2)) (void) mongets(mtmp, AKLYS);
		else (void) mongets(mtmp, BULLWHIP);
		break;
	    /* [Tom] added some more */
	    case 6:
		if(strongmonst(ptr))
		    (void) mongets(mtmp, rn2(20) ? AXE : WAR_HAMMER);
		else if (rn2(2)) (void) mongets(mtmp, SHORT_SWORD);
		else (void) mongets(mtmp, FLAIL);
		break;
	    case 7:
		if(strongmonst(ptr))
		    (void) mongets(mtmp, rn2(20) ? MACE : BASEBALL_BAT);
		else if (rn2(2)) (void) mongets(mtmp, CLUB);
		else (void) mongets(mtmp, RUBBER_HOSE);
		break;
	    case 8:
		(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1) );
		if (!rn2(20)) (void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1) );
		if(strongmonst(ptr)) (void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1) );
		break;
	    case 9:
		if (!rn2(50)) (void) mongets(mtmp, PICK_AXE);
		if (!rn2(250)) (void) mongets(mtmp, DWARVISH_MATTOCK);
	    default:
		break;
	}

	if (needspick(ptr) && !rn2(40) ) (void) mongets(mtmp, PICK_AXE);

	return;
}

STATIC_OVL void
m_initxtraitem(mtmp)
register struct monst *mtmp;
{
	register struct permonst *ptr = mtmp->data;

	if (!rn2(250) && timebasedlowerchance() && (rn2(100) > u.usefulitemchance) ) (void) mongets(mtmp, SCR_STANDARD_ID);

	if (!rn2(60) && ((rn2(100) > u.usefulitemchance) || (rn2(100) > u.usefulitemchance) ) ) (void) mongets(mtmp, SCR_HEALING);

	if (!rn2(ishaxor ? 600 : 1200)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
	if (!rn2(ishaxor ? 600 : 1200)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
	if (!rn2(ishaxor ? 600 : 1200)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));

	if (attacktype(ptr, AT_WEAP) && !rn2(ishaxor ? 1000 : 2000)) {
		struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
		if (otmpX) {
			otmpX->spe = 0;
			otmpX->quan = 1;
			otmpX->owt = weight(otmpX);
			otmpX->corpsenm = egg_type_from_parent(PM_COCKATRICE, FALSE);
			attach_egg_hatch_timeout(otmpX);
			kill_egg(otmpX); /* make sure they're stale --Amy */
			(void) mpickobj(mtmp,otmpX, TRUE);
		}
	}

	if (attacktype(ptr, AT_WEAP) && !rn2(ishaxor ? 250 : 500)) {

		struct obj *otmpY = mksobj(CORPSE,TRUE,FALSE);

		if (otmpY) {
			otmpY->spe = 0;
			otmpY->quan = 1;
			otmpY->owt = /*weight(otmpY)*/30;
			otmpY->corpsenm = PM_COCKATRICE;
			start_corpse_timeout(otmpY); /* gotta make sure they time out after a while! --Amy */
			(void) mpickobj(mtmp,otmpY, TRUE);
		}

		struct obj *otmpZ = mksobj(rnd_class(PLASTEEL_GLOVES,GAUNTLETS_OF_DEXTERITY), TRUE, FALSE);
		if (otmpZ) {
			otmpZ->quan = 1;
			otmpZ->owt = weight(otmpZ);
			(void) mpickobj(mtmp, otmpZ, TRUE);
		}

	}

}

STATIC_OVL void
m_initweap(mtmp)
register struct monst *mtmp;
{
	register struct permonst *ptr = mtmp->data;
	register int mm = monsndx(ptr);
	struct obj *otmp;

#ifdef REINCARNATION
	if (Is_rogue_level(&u.uz) && rn2(2) ) return;
#endif
/*
 *	first a few special cases:
 *
 *		giants get a boulder to throw sometimes.
 *		ettins get clubs
 *		kobolds get darts to throw
 *		centaurs get some sort of bow & arrows or bolts
 *		soldiers get all sorts of things.
 *		kops get clubs & cream pies.
 */

	switch (ptr->mlet) {
	    case S_GIANT:
		if (rn2(2)) (void)mongets(mtmp, (mm != PM_ETTIN) ?
				    BOULDER : CLUB);

		if (ptr == &mons[PM_OPERATION]) (void)mongets(mtmp, FEMININE_PUMPS);
		if (ptr == &mons[PM_SUNBEAM]) (void)mongets(mtmp, WAN_SOLAR_BEAM);
		if (ptr == &mons[PM_CLUB_RACQUET]) (void)mongets(mtmp, MACE);
		if (ptr == &mons[PM_BLOOD_IMPACTER]) (void)mongets(mtmp, FLAIL);
		if (ptr == &mons[PM_IMPACTER]) (void)mongets(mtmp, FLANGED_MACE);
		if (ptr == &mons[PM_DEATH_IMPACTER]) (void)mongets(mtmp, AKLYS);
		if (ptr == &mons[PM_TURBO_CLUB_RACQUET]) (void)mongets(mtmp, METAL_CLUB);
		if (ptr == &mons[PM_GIANT_STONE_CRUSHER]) (void)mongets(mtmp, MALLET);
		if (ptr == &mons[PM_MAULOTAUR]) (void)mongets(mtmp, MALLET);

		if (ptr == &mons[PM_CONCRETE_GIANT]) (void)mongets(mtmp, BOULDER);

		if (ptr == &mons[PM_ELECTRIC_GIANT]) (void)mongets(mtmp, WAN_LIGHTNING);

		if (ptr == &mons[PM_FATHIEN_ELITE]) { (void)mongets(mtmp, ELVEN_BOW); (void)mongets(mtmp, ELVEN_DAGGER);
			 m_initthrow(mtmp, ELVEN_ARROW, 50);
		}

		if (ptr == &mons[PM_FATHIEN_BLADEWITCH]) { (void)mongets(mtmp, ELVEN_BOW); (void)mongets(mtmp, WEDGE_SANDALS); (void)mongets(mtmp, ELVEN_SHORT_SWORD); m_initthrow(mtmp, ELVEN_ARROW, 50);
		}

		if (ptr == &mons[PM_GRASS_GIANT]) { (void)mongets(mtmp, SLING); m_initthrow(mtmp, ROCK, 50);
		}

		if (ptr == &mons[PM_EARTH_GIANT]) { (void)mongets(mtmp, WAN_DIGGING); (void)mongets(mtmp, BOULDER); (void)mongets(mtmp, BOULDER); (void)mongets(mtmp, SLING); m_initthrow(mtmp, FLINT, 10);
		}

		if (ptr == &mons[PM_POISON_GIANT]) { (void)mongets(mtmp, ORCISH_BOW); m_initthrow(mtmp, ORCISH_ARROW, 50);
		}

		if (ptr == &mons[PM_GLASS_GIANT]) { (void)mongets(mtmp, SLING); m_initthrow(mtmp, FLINT, 35);
		}

		break;

	    case S_ANT:

		if (ptr == &mons[PM_DUKE_NUKEM_PLAYER]) { (void) mongets(mtmp, PISTOL);
			 m_initthrow(mtmp, BULLET, 25);

		}
		if (ptr == &mons[PM_DOOM_PLAYER]) {
			 (void) mongets(mtmp, HAND_BLASTER);
			m_initthrow(mtmp, rn2(20) ? BLASTER_BOLT : HEAVY_BLASTER_BOLT, 40);

		}

		break;

	    case S_FELINE:

		if (ptr == &mons[PM_PANTHER_WOMAN]) (void)mongets(mtmp, BULLWHIP);
		if (ptr == &mons[PM_ARMED_PANTHER]) m_initthrow(mtmp, ORCISH_DAGGER, 2);

		if (ptr == &mons[PM_HUNTRESS]) (void)mongets(mtmp, BULLWHIP);
		if (ptr == &mons[PM_SLINGER]) m_initthrow(mtmp, ORCISH_DAGGER, 3);
		if (ptr == &mons[PM_SPEAR_THROWING_CAT]) m_initthrow(mtmp, SPEAR, 3);
		if (ptr == &mons[PM_BOMB_THROWING_CAT]) m_initthrow(mtmp, FRAG_GRENADE, 3);
		if (ptr == &mons[PM_SABER_CAT]) (void)mongets(mtmp, RAPIER);
		if (ptr == &mons[PM_NIGHT_TIGER]) (void)mongets(mtmp, JAVELIN);
		if (ptr == &mons[PM_NIGHT_SLINGER]) { (void)mongets(mtmp, SLING); m_initthrow(mtmp, ROCK, 25);}
		if (ptr == &mons[PM_POISON_SLINGER]) m_initthrow(mtmp, GAS_GRENADE, 3);
		if (ptr == &mons[PM_INCORRECT_CAT]) (void)mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1) );
		if (ptr == &mons[PM_ANNOYING_CAT]) (void)mongets(mtmp, AMULET_OF_LIFE_SAVING );
		if (ptr == &mons[PM_HYPER_SABLE_CAT]) (void)mongets(mtmp, GOLDEN_SABER );
		if (ptr == &mons[PM_HELL_SLINGER]) { (void)mongets(mtmp, SLING); m_initthrow(mtmp, FLINT, 25);}

		if (ptr == &mons[PM_GUN_CAT]) { (void)mongets(mtmp, PISTOL); m_initthrow(mtmp, BULLET, 25);}

		if (ptr == &mons[PM_AUTOHIT_SLINGER]) { (void)mongets(mtmp, SLING); m_initthrow(mtmp, FLINT, 35);}
		if (ptr == &mons[PM_JAVELIN_CHUCKER]) m_initthrow(mtmp, JAVELIN, 3);
		if (ptr == &mons[PM_SPIRIT_THROWER]) m_initthrow(mtmp, SPIRIT_THROWER, 3);

		break;

	    case S_WALLMONST:

		if (ptr == &mons[PM_KALASHNIKOV_WALL] || ptr == &mons[PM_PETTY_KALASHNIKOV_WALL]) {
		  	(void) mongets(mtmp, HEAVY_MACHINE_GUN);
		  	m_initthrow(mtmp, BULLET, 30);
		  	m_initthrow(mtmp, BULLET, 30);
		}

		break;

	    case S_RUBMONST:

		if (ptr == &mons[PM_LIGHTNINGROD]) {
		  	(void) mongets(mtmp, POT_SPEED);
		  	(void) mongets(mtmp, WAN_CREATE_HORDE);
		}

		break;

	    case S_HUMAN:

		if (ptr == &mons[PM_TRIBUTE]) {

			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));

		}

		if (ptr == &mons[PM_TRIBUTE_MASTER]) {

			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
			(void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));
			(void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));
			(void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));
			(void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));
			(void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));
		}

		if (ptr == &mons[PM_BLOODY_LAWYER]) (void) mongets(mtmp, SCR_BAD_EFFECT);

		if (ptr == &mons[PM_HEFFER]) (void) mongets(mtmp, MALLET);

		if (ptr == &mons[PM_KARATE_SISTER]) (void) mongets(mtmp, COMBAT_STILETTOS);

		if (ptr == &mons[PM_WOODSMAN]) (void) mongets(mtmp, AXE);

		if (ptr == &mons[PM_BATTLE_MAIDEN]) {(void) mongets(mtmp, LEATHER_PEEP_TOES); (void) mongets(mtmp, KATANA); (void) mongets(mtmp, CROSSBOW); m_initthrow(mtmp, CROSSBOW_BOLT, 25);
		}

		if (ptr == &mons[PM_FEMALE_SAMURAI]) {(void) mongets(mtmp, WEDGE_SANDALS); (void) mongets(mtmp, KATANA); (void) mongets(mtmp, YUMI); m_initthrow(mtmp, YA, 25);
		}

		if (ptr == &mons[PM_FRYBONE]) {(void) mongets(mtmp, DAGGER); (void) mongets(mtmp, SMALL_SHIELD);}

		if (ptr == &mons[PM_GEMINI_RICKETS]) {(void) mongets(mtmp, ATHAME); (void) mongets(mtmp, LARGE_SHIELD); (void) mongets(mtmp, SPLINT_MAIL);}

		if (ptr == &mons[PM_CAR_DRIVING_SUPERTHIEF]) {(void) mongets(mtmp, SCR_WARPING); (void) mongets(mtmp, SCR_WARPING);}

		if (ptr == &mons[PM_SUPERJEDI] || ptr == &mons[PM_DIVISION_JEDI] || ptr == &mons[PM_CRITICALLY_INJURED_JEDI]) (void) mongets(mtmp, RED_LIGHTSABER);

		if (ptr == &mons[PM_OH_GOD]) (void) mongets(mtmp, WAN_SUMMON_UNDEAD);
		if (ptr == &mons[PM_IZUMO]) { (void) mongets(mtmp, KATANA); m_initthrow(mtmp, DART, 25);
		}

		if (ptr == &mons[PM_DARK_RANGER] || ptr == &mons[PM_DARK_ARCHER] || ptr == &mons[PM_VILE_ARCHER] || ptr == &mons[PM_FIRE_ARCHER] || ptr == &mons[PM_BLACK_ARCHER] || ptr == &mons[PM_FLESH_ARCHER] || ptr == &mons[PM_SAND_ARCHER] || ptr == &mons[PM_JUNGLE_ARCHER] || ptr == &mons[PM_SERRATED_ARCHER] || ptr == &mons[PM_PALE_ARCHER] || ptr == &mons[PM_WATER_ARCHER]) { (void) mongets(mtmp, BOW); m_initthrow(mtmp, ARROW, 25); }

		if (ptr == &mons[PM_RANGER_CHIEFTAIN]) { (void) mongets(mtmp, BOW); m_initthrow(mtmp, ARROW, 50); }
		if (ptr == &mons[PM_CORSAIR_OF_UMBAR]) { (void) mongets(mtmp, BOW); m_initthrow(mtmp, ARROW, 50); }
		if (ptr == &mons[PM_NOVICE_RANGER]) { (void) mongets(mtmp, BOW); m_initthrow(mtmp, ARROW, 10); }
		if (ptr == &mons[PM_SEMI_NOVICE_RANGER]) { (void) mongets(mtmp, BOW); m_initthrow(mtmp, ARROW, 10); }

		if (ptr == &mons[PM_ROGUE_HIRELING] || ptr == &mons[PM_ROGUE_SCOUT] || ptr == &mons[PM_WIG_SHEEP] || ptr == &mons[PM_ROGUE_CAPTAIN]) { (void) mongets(mtmp, BOW); m_initthrow(mtmp, ARROW, 35); }

		if (ptr == &mons[PM_HOSTILE_GIRLFRIEND] || ptr == &mons[PM_PAS_OP]) { (void) mongets(mtmp, CROSSBOW); m_initthrow(mtmp, CROSSBOW_BOLT, 35); }

		if (ptr == &mons[PM_VERA_S_SERVANT] || ptr == &mons[PM_LADY_FREEZE]) { (void) mongets(mtmp, ELVEN_BOW); m_initthrow(mtmp, ELVEN_ARROW, 35); }

		if (ptr == &mons[PM_DEVIOUS_GIRL] || ptr == &mons[PM_HELL_FIRE_ROGUE]) { (void) mongets(mtmp, GRENADE_LAUNCHER); m_initthrow(mtmp, FRAG_GRENADE, 10); }

		if (ptr == &mons[PM_NINJA_LORD]) { (void) mongets(mtmp, KATANA); m_initthrow(mtmp, SHURIKEN, 30); }

		if (ptr == &mons[PM_VIOLET_BLADEWITCH]) { (void) mongets(mtmp, RUNESWORD); (void) mongets(mtmp, DROVEN_BOW); m_initthrow(mtmp, DROVEN_ARROW, 30); (void) mongets(mtmp, DANCING_SHOES); }

		if (ptr == &mons[PM_WEAK_GUARD] || ptr == &mons[PM_POSSESSED_GUARD]) { (void) mongets(mtmp, VOULGE); (void) mongets(mtmp, SCALE_MAIL); }

		if (ptr == &mons[PM_PSYCHO_GUARD]) { (void) mongets(mtmp, GUISARME); (void) mongets(mtmp, CHAIN_MAIL); }
		if (ptr == &mons[PM_EXTREME_PSYCHO_GUARD]) { (void) mongets(mtmp, GUISARME); (void) mongets(mtmp, CHAIN_MAIL); }

		if (ptr == &mons[PM_BAD_GUARD] || ptr == &mons[PM_DARK_LORD_GUARD]) { (void) mongets(mtmp, BILL_GUISARME); (void) mongets(mtmp, SPLINT_MAIL); }

		if (ptr == &mons[PM_DARK_SPEARWOMAN]) (void) mongets(mtmp, SPEAR);

		if (ptr == &mons[PM_DREAD_WITCH]) (void) mongets(mtmp, SCR_SUMMON_UNDEAD);

		if (ptr == &mons[PM_DIRE_WITCH]) {(void) mongets(mtmp, SCR_CREATE_MONSTER); (void) mongets(mtmp, WAN_COLD);}

		if (ptr == &mons[PM_SHAOLIN_PRIESTESS]) {(void) mongets(mtmp, ELECTRIC_SWORD); (void) mongets(mtmp, ROBE_OF_POWER); (void) mongets(mtmp, SWEET_MOCASSINS);}

		if (ptr == &mons[PM_TWISTED_SYLPH]) {(void) mongets(mtmp, WAN_CREATE_MONSTER); (void) mongets(mtmp, POT_FULL_HEALING); (void) mongets(mtmp, WAN_FIREBALL); (void) mongets(mtmp, WAN_BAD_EFFECT);}

		if (ptr == &mons[PM_DARK_RONIN] || ptr == &mons[PM_FEMALE_RONIN]) {(void) mongets(mtmp, KATANA); (void) mongets(mtmp, LARGE_SHIELD); (void) mongets(mtmp, YUMI); m_initthrow(mtmp, YA, 30);}

		if (ptr == &mons[PM_ELVEN_ARCHER]) {(void) mongets(mtmp, ELVEN_BOW); m_initthrow(mtmp, ELVEN_ARROW, 25);}
		if (ptr == &mons[PM_HIGH_ELVEN_RANGER]) {(void) mongets(mtmp, ELVEN_BOW); m_initthrow(mtmp, ELVEN_ARROW, 50); m_initthrow(mtmp, ELVEN_ARROW, 50);}

		if (ptr == &mons[PM_BAAL_SPY] || ptr == &mons[PM_HOSTILE_POW]) (void) mongets(mtmp, TWO_HANDED_SWORD);

		if (ptr == &mons[PM_TORTURED_TRAITOR] || ptr == &mons[PM_TRANSLATOR]) (void) mongets(mtmp, TSURUGI);

		if (ptr == &mons[PM_VILE_LANCER] || ptr == &mons[PM_BAD_LANCE_CARRIER] || ptr == &mons[PM_DARK_LANCER] || ptr == &mons[PM_SAND_LANCER] || ptr == &mons[PM_BLACK_LANCER] || ptr == &mons[PM_FLESH_LANCER] || ptr == &mons[PM_FIRE_LANCER] || ptr == &mons[PM_SERRATED_LANCE_CARRIER] || ptr == &mons[PM_PALE_PHALANX] || ptr == &mons[PM_JUNGLE_PHALANX] || ptr == &mons[PM_WATER_PHALANX] || ptr == &mons[PM_CHARRED_PHALANX] || ptr == &mons[PM_NINJA_FIGHTER] || ptr == &mons[PM_MARTIAL_ARTS_ASSASSIN]) (void) mongets(mtmp, LANCE);

		if (ptr == &mons[PM_BLACK_MAGE] || ptr == &mons[PM_COUNSELOR] || ptr == &mons[PM_MAGISTRATE] || ptr == &mons[PM_GREEN_GUNNER] || ptr == &mons[PM_ADVOCATE] || ptr == &mons[PM_CABBALIST] || ptr == &mons[PM_DREADJUDGE] || ptr == &mons[PM_JOY_SPIKE_MAGE] || ptr == &mons[PM_JOY_PRISON_MAGE] || ptr == &mons[PM_BEAR_CLAN_MAGE]) (void) mongets(mtmp, QUARTERSTAFF);

		if (ptr == &mons[PM_GREEN_GUNNER]) { (void) mongets(mtmp, PISTOL); m_initthrow(mtmp, BULLET, 25); }
		if (ptr == &mons[PM_BEAR_CLAN_MAGE]) { (void) mongets(mtmp, SHOTGUN); m_initthrow(mtmp, SHOTGUN_SHELL, 20); }

		if (ptr == &mons[PM_SERRATED_ROGUE]) (void) mongets(mtmp, JAGGED_STAR);

		if (ptr == &mons[PM_ARABELLA_THE_MONEY_THIEF]) {

			mtmp->mgold = u.bankcashamount;
			u.bankcashamount = 0;
			u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));

		}

		if (ptr == &mons[PM_DARK_JEDI]) (void) mongets(mtmp, RED_LIGHTSABER);

		if (ptr == &mons[PM_STORECLERK]) (void) mongets(mtmp, POT_PARALYSIS);
		if (ptr == &mons[PM_FUNNY_GIRL]) (void) mongets(mtmp, POT_BLINDNESS);
		if (ptr == &mons[PM_FUNNY_LADY]) {(void) mongets(mtmp, POT_BLINDNESS); (void) mongets(mtmp, POT_BLINDNESS); }
		if (ptr == &mons[PM_JOYOUS_GIRL]) (void) mongets(mtmp, POT_BLINDNESS);
		if (ptr == &mons[PM_JOYOUS_LADY]) {(void) mongets(mtmp, POT_BLINDNESS); (void) mongets(mtmp, POT_BLINDNESS); }
		if (ptr == &mons[PM_ELF_BOXER]) (void) mongets(mtmp, POT_CONFUSION);
		if (ptr == &mons[PM_ELF_CHAMPION]) {(void) mongets(mtmp, POT_CONFUSION); (void) mongets(mtmp, POT_CONFUSION); (void) mongets(mtmp, POT_CONFUSION);}
		if (ptr == &mons[PM_LECTURER]) {(void) mongets(mtmp, POT_SLEEPING); (void) mongets(mtmp, POT_SLEEPING); (void) mongets(mtmp, POT_SLEEPING);}

		if(is_mercenary(ptr) || mm == PM_SHOPKEEPER || mm == PM_PUNISHER
#ifdef YEOMAN
				|| mm == PM_CHIEF_YEOMAN_WARDER || mm == PM_YEOMAN_WARDER
#endif
#ifdef JEDI
				|| mm == PM_JEDI || mm == PM_PADAWAN || mm == PM_STORMTROOPER
#endif
				|| mm == PM_ENEMY_TROOPER || mm == PM_INFANTRYMAN || mm == PM_CUNTGUN_TROOPER
				|| mm == PM_GI_TROOPER || mm == PM_HEAVY_WEAPON_DUDE || mm == PM_RIFLEMAN
				|| mm == PM_SNIPER || mm == PM_RIOT_BREAKER || mm == PM_TANK_BREAKER
				|| mm == PM_EVASIVE_ENEMY_TROOPER || mm == PM_EVASIVE_INFANTRYMAN || mm == PM_EVASIVE_CUNTGUN_TROOPER
				|| mm == PM_EVASIVE_GI_TROOPER || mm == PM_EVASIVE_HEAVY_WEAPON_DUDE || mm == PM_EVASIVE_RIFLEMAN
				|| mm == PM_EVASIVE_SNIPER || mm == PM_EVASIVE_RIOT_BREAKER || mm == PM_EVASIVE_TANK_BREAKER
		) {

		    int w1 = 0, w2 = 0;
		    switch (mm) {
			case PM_ILLUSIONARY_SOLDIER:

				(void) mongets(mtmp, LEATHER_GLOVES);
				{
					struct obj *otmpS = mksobj(CORPSE,TRUE,FALSE);

					if (otmpS) {
						otmpS->spe = 0;
						otmpS->quan = 1;
						otmpS->owt = 30;
						otmpS->corpsenm = PM_COCKATRICE;
						start_corpse_timeout(otmpS); /* gotta make sure they time out after a while! --Amy */
						(void) mpickobj(mtmp,otmpS, TRUE);
					}
				}
				/* fall through */

			case PM_SOLDIER:
#ifdef FIREARMS
			  w1 = rn2(2) ? RIFLE : SUBMACHINE_GUN;
		  	  m_initthrow(mtmp, BULLET, 25);
		  	  m_initthrow(mtmp, BULLET, 25);
			  w2 = rn2(2) ? KNIFE : DAGGER;
			  if (rn2(2)) (void) mongets(mtmp, FRAG_GRENADE);
			  break;
#endif
			case PM_WATCHMAN:
			case PM_ANGRY_WATCHMAN:
			  if (!rn2(3)) {
			      w1 = rn1(BEC_DE_CORBIN - PARTISAN + 1, PARTISAN);
			      w2 = rn2(2) ? DAGGER : KNIFE;
			  } else w1 = rn2(2) ? SPEAR : SHORT_SWORD;
			  break;
			case PM_WATCH_CAPTAIN:
			case PM_ANGRY_WATCH_CAPTAIN:
			  w1 = rn2(2) ? LONG_SWORD : SILVER_SABER;
			  break;
			case PM_ANGRY_WATCH_LIEUTENANT:
			case PM_WATCH_LIEUTENANT:
			  w1 = rn2(2) ? LONG_SWORD : SILVER_SABER;
			  break;
			case PM_ANGRY_WATCH_LEADER:
			case PM_WATCH_LEADER:
			  w1 = rn2(2) ? LONG_SWORD : SILVER_SABER;
			  break;
			case PM_LIEUTENANT:
#ifdef FIREARMS
			  if (rn2(2)) {
			  	w1 = HEAVY_MACHINE_GUN;
			  	m_initthrow(mtmp, BULLET, 50);
			  	m_initthrow(mtmp, BULLET, 50);
			  	m_initthrow(mtmp, BULLET, 50);
			  } else {
			  	w1 = SUBMACHINE_GUN;
			  	m_initthrow(mtmp, BULLET, 30);
			  	m_initthrow(mtmp, BULLET, 30);
			  }
			  w2 = rn2(2) ? KNIFE : DAGGER;
			  if (!rn2(4)) {
			  	(void) mongets(mtmp, FRAG_GRENADE);
			  	(void) mongets(mtmp, FRAG_GRENADE);
			  } else if (!rn2(4)) {
			  	(void) mongets(mtmp, GAS_GRENADE);
			  	(void) mongets(mtmp, GAS_GRENADE);
			  }
			  break;
#endif
			case PM_SERGEANT:
#ifdef FIREARMS
			  if (rn2(2)) {
			  	w1 = AUTO_SHOTGUN;
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 10);
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 10);
			  } else {
			  	w1 = ASSAULT_RIFLE;
			  	m_initthrow(mtmp, BULLET, 30);
			  	m_initthrow(mtmp, BULLET, 30);
			  }
			  w2= rn2(2) ? DAGGER : KNIFE;
			  if (!rn2(5)) {
			  	m_initthrow(mtmp, FRAG_GRENADE, 5);
				if (!rn2(5)) (void) mongets(mtmp, GRENADE_LAUNCHER);
			  } else if (!rn2(5)) {
			  	m_initthrow(mtmp, GAS_GRENADE, 5);
				if (!rn2(5)) (void) mongets(mtmp, GRENADE_LAUNCHER);
			  }
			  break;
#endif
#ifdef YEOMAN
			case PM_YEOMAN_WARDER:
#endif
			  w1 = rn2(2) ? FLAIL : MACE;
			  break;
#ifdef CONVICT
			case PM_PRISON_GUARD:
#endif /* CONVICT */
			case PM_CAPTAIN:
			case PM_GENERAL:
#ifdef FIREARMS
			  if (rn2(2)) {
			  	w1 = AUTO_SHOTGUN;
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  } else if (rn2(2)) {
			  	w1 = HEAVY_MACHINE_GUN;
			  	m_initthrow(mtmp, BULLET, 60);
			  	m_initthrow(mtmp, BULLET, 60);
			  	m_initthrow(mtmp, BULLET, 60);
			  } else {
			  	w1 = ASSAULT_RIFLE;
			  	m_initthrow(mtmp, BULLET, 60);
			  	m_initthrow(mtmp, BULLET, 60);
			  }
			  if (rn2(2)) {
				  w2 = ROCKET_LAUNCHER;
			  	  m_initthrow(mtmp, ROCKET, 5);
			  } else if (!rn2(4)) {
				  (void) mongets(mtmp, GRENADE_LAUNCHER);			  
			  	  m_initthrow(mtmp, 
			  	  	(rn2(2) ? FRAG_GRENADE : GAS_GRENADE), 
			  	  	5);
			  } else {
				  w2 = rn2(2) ? SILVER_SABER : DAGGER;
			  }
			  break;
#endif
#ifdef YEOMAN
			case PM_CHIEF_YEOMAN_WARDER:
#endif
			  w1 = rn2(2) ? BROADSWORD : LONG_SWORD;
			  break;
#ifdef JEDI
			case PM_PADAWAN:
			case PM_JEDI:
			  switch(rnd(3)){
			    case 1: mongets(mtmp, RED_LIGHTSABER); break;
			    case 2: mongets(mtmp, BLUE_LIGHTSABER); break;
			    case 3: mongets(mtmp, GREEN_LIGHTSABER); break;
			    default: break;
			  }
			  break;
			case PM_THE_JEDI_MASTER:
			  mongets(mtmp, BLUE_LIGHTSABER);
			  mongets(mtmp, SPEED_BOOTS);
			  mongets(mtmp, AMULET_OF_REFLECTION);
			  break;
			case PM_STORMTROOPER:
#ifdef FIREARMS
			  if (rn2(2)) {
			  	w1 = HEAVY_MACHINE_GUN;
			  	m_initthrow(mtmp, BULLET, 50);
			  } else {
			  	w1 = SUBMACHINE_GUN;
			  	m_initthrow(mtmp, BULLET, 30);
			  }
			  mongets(mtmp, PLASTEEL_ARMOR);
			  mongets(mtmp, PLASTEEL_GLOVES);
			  mongets(mtmp, PLASTEEL_BOOTS);
			  mongets(mtmp, PLASTEEL_HELM);
			  break;
#endif // FIREARMS
#endif // JEDI

#ifdef FIREARMS
			case PM_HEAVY_WEAPON_DUDE:
			case PM_EVASIVE_HEAVY_WEAPON_DUDE:
			  	w1 = HEAVY_MACHINE_GUN;
			  	m_initthrow(mtmp, BULLET, 50);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_INFANTRYMAN:
			case PM_EVASIVE_INFANTRYMAN:
			  	w1 = SUBMACHINE_GUN;
			  	m_initthrow(mtmp, BULLET, 50);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_ENEMY_TROOPER:
			case PM_EVASIVE_ENEMY_TROOPER:
			  	w1 = PISTOL;
			  	m_initthrow(mtmp, BULLET, 50);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_CUNTGUN_TROOPER:
			case PM_EVASIVE_CUNTGUN_TROOPER:
			  	w1 = RIFLE;
			  	m_initthrow(mtmp, BULLET, 50);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_GI_TROOPER:
			case PM_EVASIVE_GI_TROOPER:
			  	w1 = ASSAULT_RIFLE;
			  	m_initthrow(mtmp, BULLET, 50);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_SNIPER:
			case PM_EVASIVE_SNIPER:
			  	w1 = SNIPER_RIFLE;
			  	m_initthrow(mtmp, BULLET, 50);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_GERMAN_SNIPER:
			  	w1 = SNIPER_RIFLE;
			  	m_initthrow(mtmp, BULLET, 50);
			  	m_initthrow(mtmp, BULLET, 50);
			  mongets(mtmp, PLATE_MAIL);
			  break;

			case PM_RIFLEMAN:
			case PM_EVASIVE_RIFLEMAN:
			  	w1 = SHOTGUN;
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_RIOT_BREAKER:
			case PM_EVASIVE_RIOT_BREAKER:
			  	w1 = AUTO_SHOTGUN;
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_TANK_BREAKER:
			case PM_EVASIVE_TANK_BREAKER:
			  	w1 = ROCKET_LAUNCHER;
			  	m_initthrow(mtmp, ROCKET, 7);
			  mongets(mtmp, CHAIN_MAIL);
			  break;
#endif
			case PM_SHOPKEEPER:
			case PM_PUNISHER:
#ifdef FIREARMS
			  (void) mongets(mtmp, rn2(50) ? SHOTGUN : SAWED_OFF_SHOTGUN);
			  m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  m_initthrow(mtmp, SHOTGUN_SHELL, 20);

			/* evil patch idea by jonadab: 5% chance for shopkeepers to be generated invisible */
			  if (!rn2(20)) (void) mongets(mtmp, CLOAK_OF_INVISIBILITY);

#endif
			  /* Fallthrough */
			default:
			  if (!rn2(4)) w1 = DAGGER;
			  if (!rn2(7)) w2 = SPEAR;
			  break;
		    }
		    if (w1) (void)mongets(mtmp, w1);
		    if (!w2 && w1 != DAGGER && !rn2(4)) w2 = KNIFE;
		    if (w2) (void)mongets(mtmp, w2);
		} else if (is_elf(ptr)) {

		    if (mm == PM_DROW) {
			(void) mongets(mtmp, DARK_ELVEN_MITHRIL_COAT);
			(void) mongets(mtmp, DARK_ELVEN_SHORT_SWORD);
			(void) mongets(mtmp, DARK_ELVEN_BOW);
			m_initthrow(mtmp, DARK_ELVEN_ARROW, 25);
		    } else if (mm == PM_DROW_SHARPSHOOTER) {
			(void) mongets(mtmp, DARK_ELVEN_MITHRIL_COAT);
			(void) mongets(mtmp, DARK_ELVEN_SHORT_SWORD);
			(void) mongets(mtmp, DROVEN_CROSSBOW);
			m_initthrow(mtmp, DROVEN_BOLT, 25);
		    } else if (mm == PM_DROW_SNIPER) {
			(void) mongets(mtmp, DARK_ELVEN_MITHRIL_COAT);
			(void) mongets(mtmp, DARK_ELVEN_SHORT_SWORD);
			(void) mongets(mtmp, DROVEN_BOW);
			m_initthrow(mtmp, DROVEN_ARROW, 25);
		    } else {
		    if (rn2(2))
			(void) mongets(mtmp,
				   !rn2(4) ? ELVEN_MITHRIL_COAT : ELVEN_CLOAK);
		      if (!rn2(3)) (void)mongets(mtmp, ELVEN_LEATHER_HELM);
		    else if (!rn2(4)) (void)mongets(mtmp, ELVEN_BOOTS);
		      if (!rn2(3)) (void)mongets(mtmp, ELVEN_DAGGER);
		    switch (rn2(3)) {
			case 0:
			    if (!rn2(4)) (void)mongets(mtmp, ELVEN_SHIELD);
			    if (rn2(3)) (void)mongets(mtmp, ELVEN_SHORT_SWORD);
			    (void)mongets(mtmp, ELVEN_BOW);
			    m_initthrow(mtmp, ELVEN_ARROW, 25);
			    break;
			case 1:
			    (void)mongets(mtmp, ELVEN_BROADSWORD);
			    if (rn2(2)) (void)mongets(mtmp, ELVEN_SHIELD);
			    break;
			case 2:
			    if (rn2(2)) {
				(void)mongets(mtmp, ELVEN_SPEAR);
				(void)mongets(mtmp, ELVEN_SHIELD);
			    }
			    break;
		    }
		    if (mm == PM_ELVENKING) {
			if (rn2(3) || (in_mklev && Is_earthlevel(&u.uz)))
			    (void)mongets(mtmp, PICK_AXE);
			if (!rn2(50)) (void)mongets(mtmp, CRYSTAL_BALL);
		    }
		    } /* normal elves */                
		} else /* enemy characters! */ /* They will always get stuff now. --Amy */
		 if ((mm >= PM_ARCHEOLOGIST && mm <= PM_WIZARD /*&& rn2(4)*/) || (mm >= PM_UNDEAD_ARCHEOLOGIST && mm <= PM_UNDEAD_WIZARD) ) { 

			if (!rn2(ishaxor ? 20 : 40) ) (void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
			if (!rn2(ishaxor ? 20 : 40) ) (void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));
			if (!rn2(ishaxor ? 40 : 80) ) (void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));
			if (!rn2(ishaxor ? 80 : 160) ) (void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));
			if (!rn2(ishaxor ? 160 : 320) ) (void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));
			if (!rn2(ishaxor ? 320 : 640) ) (void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));

		  switch (mm) {
		   case PM_ARCHEOLOGIST:
		   case PM_UNDEAD_ARCHEOLOGIST:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));

		     (void)mongets(mtmp, BULLWHIP);
		     (void)mongets(mtmp, LEATHER_JACKET);
		     (void)mongets(mtmp, FEDORA);
		     if (rn2(2)) (void)mongets(mtmp, PICK_AXE);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_ZOOKEEPER:
		   case PM_UNDEAD_ZOOKEEPER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));

		     (void)mongets(mtmp, BULLWHIP);
		   break;

		   case PM_ARTIST:
		   case PM_UNDEAD_ARTIST:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, MALLET);
		     (void)mongets(mtmp, FEDORA);
		     (void)mongets(mtmp, LAB_COAT);
		     if (!rn2(4)) (void)mongets(mtmp, GAUNTLETS_OF_DEXTERITY);
		   break;

		   case PM_GAMER:
		   case PM_UNDEAD_GAMER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, T_SHIRT);

		   break;

		   case PM_BARBARIAN:
		   case PM_UNDEAD_BARBARIAN:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));

		     (void)mongets(mtmp, BATTLE_AXE);
		     if (!rn2(2)) (void)mongets(mtmp, TWO_HANDED_SWORD);
		     (void)mongets(mtmp, RING_MAIL);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_BINDER:
		   case PM_UNDEAD_BINDER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, VOULGE);
		     (void)mongets(mtmp, KNIFE);
		     (void)mongets(mtmp, LEATHER_CLOAK);
		   break;

		   case PM_BLEEDER:
		   case PM_UNDEAD_BLEEDER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, KNIFE);
		     (void)mongets(mtmp, LAB_COAT);
		   break;

		   case PM_BARD: case PM_UNDEAD_BARD:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, LEATHER_CLOAK);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_CAVEMAN: case PM_CAVEWOMAN:
		   case PM_UNDEAD_CAVEMAN: case PM_UNDEAD_CAVEWOMAN:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, CLUB);
		     if (rn2(3)) {
			(void)mongets(mtmp, BOW);
			 m_initthrow(mtmp, ARROW, 25);
		     }
		     (void)mongets(mtmp, LEATHER_ARMOR);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;
		   case PM_DOPPELGANGER:
		     (void)mongets(mtmp, SILVER_DAGGER);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_HEMI_DOPPELGANGER:

			(void)mongets(mtmp, WAN_POLYMORPH);
		   break;

		   case PM_GEEK:
		   case PM_UNDEAD_GEEK:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, SILVER_DAGGER);
		   break;
		   case PM_ELF:
		     /* gets taken care of later... */
		     /*(void)mongets(mtmp, ELVEN_SHORT_SWORD);
		     if (rn2(3)) {
			(void)mongets(mtmp, ELVEN_BOW);
			 m_initthrow(mtmp, ELVEN_ARROW, 25);
		     }
		     if (rn2(3)) (void)mongets(mtmp, ELVEN_CLOAK);
		     else (void)mongets(mtmp, ELVEN_MITHRIL_COAT);*/
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;
		   case PM_FLAME_MAGE:
		   case PM_UNDEAD_FLAME_MAGE:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, QUARTERSTAFF);
		     (void)mongets(mtmp, STUDDED_LEATHER_ARMOR);
		     (void)mongets(mtmp, WAN_FIRE);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_FIREFIGHTER:
		   case PM_UNDEAD_FIREFIGHTER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, AXE);
		     (void)mongets(mtmp, WAN_FIRE);
		   break;

		   case PM_ACID_MAGE:
		   case PM_UNDEAD_ACID_MAGE:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, QUARTERSTAFF);
		     (void)mongets(mtmp, STUDDED_LEATHER_ARMOR);
		     (void)mongets(mtmp, WAN_ACID);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_UNDEAD_ROCKER: /* needs to be filled */
		   case PM_ROCKER: /* needs to be filled */
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));

			(void)mongets(mtmp, SLING);
			 m_initthrow(mtmp, FLINT, 25);
			(void)mongets(mtmp, PICK_AXE);

			struct obj *otmp = mksobj(EGG,TRUE,FALSE);
			if (otmp) {
				otmp->spe = 0;
				otmp->quan = 1;
				otmp->owt = weight(otmp);
				otmp->corpsenm = egg_type_from_parent(PM_COCKATRICE, FALSE);
				attach_egg_hatch_timeout(otmp);
				kill_egg(otmp); /* make sure they're stale --Amy */
				mpickobj(mtmp,otmp, TRUE);
			}

		   break;

		   case PM_CONVICT:
		   case PM_UNDEAD_CONVICT:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, HEAVY_IRON_BALL);
		     (void)mongets(mtmp, STRIPED_SHIRT);
		     (void)mongets(mtmp, ROCK);
		     (void)mongets(mtmp, ROCK);
		     (void)mongets(mtmp, ROCK);
		     (void)mongets(mtmp, ROCK);
		     (void)mongets(mtmp, ROCK);

			break;

		   case PM_MURDERER:
		   case PM_UNDEAD_MURDERER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, HEAVY_IRON_BALL);
		     (void)mongets(mtmp, STRIPED_SHIRT);
		     (void)mongets(mtmp, ROCK);
		     (void)mongets(mtmp, ROCK);
		     (void)mongets(mtmp, ROCK);
		     (void)mongets(mtmp, ROCK);
		     (void)mongets(mtmp, ROCK);

		   break;

		   case PM_SPACEWARS_FIGHTER:
		   case PM_UNDEAD_SPACEWARS_FIGHTER:
		   case PM_CAMPERSTRIKER:
		   case PM_UNDEAD_CAMPERSTRIKER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, GREEN_LIGHTSABER);
		     (void)mongets(mtmp, DAGGER);
		     (void)mongets(mtmp, PLASTEEL_ARMOR);
		     if (!rn2(3)) (void)mongets(mtmp, WAN_SOLAR_BEAM);

		   break;

		   case PM_COURIER:
		   case PM_UNDEAD_COURIER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, PISTOL);
			 m_initthrow(mtmp, BULLET, 25);
		     (void)mongets(mtmp, KNIFE);

		   break;

		   case PM_ZYBORG:
		   case PM_UNDEAD_ZYBORG:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, PLATE_MAIL);
		     (void)mongets(mtmp, GREEN_LIGHTSABER);
		     (void)mongets(mtmp, WAN_SOLAR_BEAM);

		   break;

		   case PM_LUNATIC:
		   case PM_UNDEAD_LUNATIC:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, STEEL_SHIELD);
		     (void)mongets(mtmp, STEEL_WHIP);

		   break;

		   case PM_GANGSTER:
		   case PM_UNDEAD_GANGSTER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, SUBMACHINE_GUN);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);
		     (void)mongets(mtmp, LEATHER_JACKET);

		   break;

		   case PM_GUNNER:
		   case PM_UNDEAD_GUNNER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, ASSAULT_RIFLE);
		     if (!rn2(5)) (void)mongets(mtmp, SNIPER_RIFLE);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);
		     (void)mongets(mtmp, LEATHER_JACKET);

		   break;

		   case PM_DOOM_MARINE:
		   case PM_UNDEAD_DOOM_MARINE:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, ASSAULT_RIFLE);
			 m_initthrow(mtmp, BULLET, 50);
		     (void)mongets(mtmp, ARM_BLASTER);
			 m_initthrow(mtmp, BLASTER_BOLT, 50);
		     (void)mongets(mtmp, CHAIN_MAIL);
		     (void)mongets(mtmp, HELMET);
		     (void)mongets(mtmp, LEATHER_GLOVES);
		     (void)mongets(mtmp, IRON_SHOES);
			if (!rn2(100)) {
		     (void)mongets(mtmp, BFG);
			 m_initthrow(mtmp, BFG_AMMO, 400);
			}

		   break;

		   case PM_WANDKEEPER:
		   case PM_UNDEAD_WANDKEEPER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, LAB_COAT);
		     (void)mongets(mtmp, KNIFE);
		     (void)mongets(mtmp, WAN_MAGIC_MISSILE);
		     if (!rn2(5)) (void)mongets(mtmp, WAN_FIRE);
		     if (!rn2(5)) (void)mongets(mtmp, WAN_STRIKING);
		     if (!rn2(5)) (void)mongets(mtmp, WAN_COLD);
		     if (!rn2(5)) (void)mongets(mtmp, WAN_LIGHTNING);
		     if (!rn2(5)) (void)mongets(mtmp, WAN_ACID);
		     if (!rn2(5)) (void)mongets(mtmp, WAN_SLEEP);
		     if (!rn2(25)) (void)mongets(mtmp, WAN_DEATH);
		     if (!rn2(10)) (void)mongets(mtmp, WAN_FIREBALL);
		     if (!rn2(5)) (void)mongets(mtmp, WAN_SOLAR_BEAM);
		     if (!rn2(5)) (void)mongets(mtmp, WAN_CREATE_MONSTER);
		     if (!rn2(5)) (void)mongets(mtmp, WAN_BAD_EFFECT);
		     if (!rn2(5)) (void)mongets(mtmp, WAN_FULL_HEALING);
		     if (!rn2(10)) (void)mongets(mtmp, WAN_DRAINING);
		     if (!rn2(50)) (void)mongets(mtmp, WAN_CANCELLATION);
		     if (!rn2(25)) (void)mongets(mtmp, WAN_POLYMORPH);
		     if (!rn2(5)) (void)mongets(mtmp, WAN_TELEPORTATION);
		     if (!rn2(100)) (void)mongets(mtmp, WAN_BANISHMENT);
		     if (!rn2(50)) (void)mongets(mtmp, WAN_CREATE_HORDE);
		     if (!rn2(50)) (void)mongets(mtmp, WAN_CLONE_MONSTER);
		     if (!rn2(5)) (void)mongets(mtmp, WAN_DIGGING);
		     if (!rn2(15)) (void)mongets(mtmp, WAN_TRAP_CREATION);

		   break;

		   case PM_DEATH_EATER:
		   case PM_UNDEAD_DEATH_EATER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, QUARTERSTAFF);
		     (void)mongets(mtmp, SHOTGUN);
			 m_initthrow(mtmp, SHOTGUN_SHELL, 10);
		     (void)mongets(mtmp, CLOAK_OF_REFLECTION);
		     if (!rn2(10)) (void) mongets(mtmp, WAN_DEATH);
		     if (!rn2(10)) (void) mongets(mtmp, AMULET_OF_LIFE_SAVING);

		   break;

		   case PM_POKEMON:
		   case PM_UNDEAD_POKEMON:
		   case PM_MUSICIAN:
		   case PM_UNDEAD_MUSICIAN:
		   case PM_GRADUATE:
		   case PM_UNDEAD_GRADUATE:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		   break;

		   case PM_WARRIOR:
		   case PM_UNDEAD_WARRIOR:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, MACE);
		     (void)mongets(mtmp, CROSSBOW);
			 m_initthrow(mtmp, CROSSBOW_BOLT, 50);
		     (void)mongets(mtmp, LARGE_SHIELD);

			(void) mongets(mtmp, rnd_class(PLATE_MAIL,YELLOW_DRAGON_SCALES));

		   break;

		   case PM_NOBLEMAN:
		   case PM_UNDEAD_NOBLEMAN:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, RAPIER);
		     (void)mongets(mtmp, RUFFLED_SHIRT);
		     (void)mongets(mtmp, LEATHER_GLOVES);
		     (void)mongets(mtmp, LEATHER_CLOAK);

		   break;

		   case PM_NOBLEWOMAN:
		   case PM_UNDEAD_NOBLEWOMAN:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, RAPIER);
		     (void)mongets(mtmp, VICTORIAN_UNDERWEAR);
		     (void)mongets(mtmp, LEATHER_GLOVES);
		     (void)mongets(mtmp, LEATHER_CLOAK);

		   break;

		   case PM_DRUNK:
		   case PM_UNDEAD_DRUNK:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, RUBBER_HOSE);

		   break;

		   case PM_PIRATE:
		   case PM_UNDEAD_PIRATE:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, SCIMITAR);
		     (void)mongets(mtmp, PISTOL);
		     (void)mongets(mtmp, KNIFE);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);
		     (void)mongets(mtmp, LEATHER_JACKET);
		     (void)mongets(mtmp, HIGH_BOOTS);

		   break;

		   case PM_OFFICER:
		   case PM_UNDEAD_OFFICER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, CLUB);
		     (void)mongets(mtmp, PISTOL);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);
		     (void)mongets(mtmp, LEATHER_ARMOR);
		     (void)mongets(mtmp, rn2(2) ? WAN_STRIKING : WAN_MAGIC_MISSILE);

		   break;

		   case PM_GLADIATOR:
		   case PM_UNDEAD_GLADIATOR:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, SILVER_SPEAR);
		     (void)mongets(mtmp, SHIELD_OF_REFLECTION);

		   break;

		   case PM_GOFF:
		   case PM_UNDEAD_GOFF:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, QUARTERSTAFF);
		     (void)mongets(mtmp, HIPPIE_HEELS);
		     (void)mongets(mtmp, PISTOL);
			 m_initthrow(mtmp, BULLET, 50);
		     (void)mongets(mtmp, POT_ACID);
		     (void)mongets(mtmp, POT_ACID);

		   break;

		   case PM_AMAZON:
		   case PM_UNDEAD_AMAZON:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, JAVELIN);
		     (void)mongets(mtmp, LEATHER_ARMOR);
		     (void)mongets(mtmp, BOW);
			 m_initthrow(mtmp, ARROW, 50);

		   break;

		   case PM_DRUID:
		   case PM_UNDEAD_DRUID:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, CLUB);
		     (void)mongets(mtmp, LEATHER_ARMOR);
		     (void)mongets(mtmp, BOW);
			 m_initthrow(mtmp, ARROW, 50);

		   break;

		   case PM_ALTMER:
		   case PM_UNDEAD_ALTMER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, QUARTERSTAFF);
		     (void)mongets(mtmp, WAN_FIRE);

		   break;

		   case PM_BOSMER:
		   case PM_UNDEAD_BOSMER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, ELVEN_DAGGER);
		     (void)mongets(mtmp, ELVEN_BOW);
			 m_initthrow(mtmp, ELVEN_ARROW, 50);

		   break;

		   case PM_DUNMER:
		   case PM_UNDEAD_DUNMER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, DARK_ELVEN_DAGGER);
		     (void)mongets(mtmp, DARK_ELVEN_BOW);
			 m_initthrow(mtmp, DARK_ELVEN_ARROW, 50);

		   break;

		   case PM_THALMOR:
		   case PM_UNDEAD_THALMOR:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, ELVEN_BROADSWORD);
		     (void)mongets(mtmp, ELVEN_MITHRIL_COAT);
		     (void)mongets(mtmp, DARK_ELVEN_BOW);
			 m_initthrow(mtmp, DARK_ELVEN_ARROW, 50);

		   break;

		   case PM_ORDINATOR:
		   case PM_UNDEAD_ORDINATOR:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, TSURUGI);
		     (void)mongets(mtmp, RUNESWORD);
		     (void)mongets(mtmp, DARK_ELVEN_MITHRIL_COAT);

		   break;

		   case PM_KORSAIR:
		   case PM_UNDEAD_KORSAIR:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, DAGGER);
		     (void)mongets(mtmp, SCIMITAR);

		   break;

		   case PM_DIVER:
		   case PM_UNDEAD_DIVER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, KNIFE);
		     (void)mongets(mtmp, TRIDENT);
		     (void)mongets(mtmp, SPEAR);
		     (void)mongets(mtmp, OILSKIN_CLOAK);

		   break;

		   case PM_CHEVALIER:
		   case PM_UNDEAD_CHEVALIER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, TRIDENT);
		     (void)mongets(mtmp, ROBE);
		     (void)mongets(mtmp, SMALL_SHIELD);

		   break;

		   case PM_TRANSVESTITE:
		   case PM_UNDEAD_TRANSVESTITE:
		   case PM_TRANSSYLVANIAN:
		   case PM_UNDEAD_TRANSSYLVANIAN:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

			(void) mongets(mtmp, rnd_class(WEDGED_LITTLE_GIRL_SANDAL,SPIKED_BATTLE_BOOT));
			(void) mongets(mtmp, rnd_class(WEDGE_SANDALS,COMBAT_STILETTOS));

		   break;

		   case PM_TOPMODEL:
		   case PM_UNDEAD_TOPMODEL:
		   case PM_FAILED_EXISTENCE:
		   case PM_UNDEAD_FAILED_EXISTENCE:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, KNIFE);
		     (void)mongets(mtmp, RIFLE);
			 m_initthrow(mtmp, BULLET, 50);
			(void) mongets(mtmp, rnd_class(WEDGE_SANDALS,COMBAT_STILETTOS));

		   break;

		   case PM_ACTIVISTOR:
		   case PM_UNDEAD_ACTIVISTOR:

			if (moves < 2) mtmp->female = 1;

			if (mtmp->female) (void)mongets(mtmp, VICTORIAN_UNDERWEAR);
			else (void)mongets(mtmp, RUFFLED_SHIRT);

		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, FLY_SWATTER);

		   break;

		   case PM_UNBELIEVER:
		   case PM_UNDEAD_UNBELIEVER:

			(void)mongets(mtmp, BLACK_AESTIVALIS);
			(void)mongets(mtmp, CHAIN_MAIL);

		   break;

		   case PM_ELECTRIC_MAGE:
		   case PM_UNDEAD_ELECTRIC_MAGE:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, QUARTERSTAFF);
		     (void)mongets(mtmp, STUDDED_LEATHER_ARMOR);
		     (void)mongets(mtmp, WAN_LIGHTNING);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_POISON_MAGE:
		   case PM_UNDEAD_POISON_MAGE:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, SILVER_KHAKKHARA);
		     (void)mongets(mtmp, POISONOUS_CLOAK);
		     (void)mongets(mtmp, WAN_POISON);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_HEALER:
		   case PM_UNDEAD_HEALER:
		   case PM_SCIENTIST:
		   case PM_UNDEAD_SCIENTIST:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, SCALPEL);
		     (void)mongets(mtmp, LEATHER_GLOVES);
		     (void)mongets(mtmp, WAN_HEALING);
		     (void)mongets(mtmp, WAN_SLEEP);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 20);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 20));
#endif
		   break;
		   case PM_ICE_MAGE:
		   case PM_UNDEAD_ICE_MAGE:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, QUARTERSTAFF);
		     (void)mongets(mtmp, STUDDED_LEATHER_ARMOR);
		     (void)mongets(mtmp, WAN_COLD);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;
#ifdef YEOMAN
		   case PM_YEOMAN:
#endif
		   case PM_UNDEAD_YEOMAN:
		   case PM_KNIGHT:
		   case PM_UNDEAD_KNIGHT:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, LONG_SWORD);
		     (void)mongets(mtmp, PLATE_MAIL);
		     (void)mongets(mtmp, LARGE_SHIELD);
		     (void)mongets(mtmp, HELMET);
		     (void)mongets(mtmp, LEATHER_GLOVES);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		     break;

		   case PM_PALADIN:
		   case PM_UNDEAD_PALADIN:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, LONG_SWORD);
		     (void)mongets(mtmp, PLATE_MAIL);
		     (void)mongets(mtmp, LARGE_SHIELD);
		     (void)mongets(mtmp, HELMET);
		     (void)mongets(mtmp, LEATHER_GLOVES);
		     (void)mongets(mtmp, LOW_BOOTS);
		     break;

		   case PM_MONK:
		   case PM_UNDEAD_MONK:
		   case PM_SAIYAN:
		   case PM_UNDEAD_SAIYAN:
		   case PM_PSION:
		   case PM_UNDEAD_PSION:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));


#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 5);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 5));
#endif
		   break;
		   case PM_NECROMANCER:
		   case PM_UNDEAD_NECROMANCER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));

		     (void)mongets(mtmp, ATHAME);
		     (void)mongets(mtmp, SCR_SUMMON_UNDEAD);
		     if (!rn2(10)) (void)mongets(mtmp, WAN_SUMMON_UNDEAD);
		     if (!rn2(4)) (void)mongets(mtmp, PICK_AXE);
		     (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     (void) mongets(mtmp, rnd_defensive_item(mtmp));
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_JESTER:
		   case PM_UNDEAD_JESTER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, HAWAIIAN_SHIRT);
		     (void)mongets(mtmp, DUNCE_CAP);
		     (void)mongets(mtmp, LEATHER_GLOVES);
		     (void)mongets(mtmp, DAGGER);
		     (void)mongets(mtmp, RUBBER_HOSE);

			break;

		   case PM_FEAT_MASTER:
		   case PM_UNDEAD_FEAT_MASTER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, FLAIL);
		     (void)mongets(mtmp, ORCISH_HELM);
		     (void)mongets(mtmp, LEATHER_JACKET);
		     (void)mongets(mtmp, POT_HEALING);

			break;

		   case PM_FOXHOUND_AGENT:
		   case PM_UNDEAD_FOXHOUND_AGENT:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, FEDORA);
		     (void)mongets(mtmp, ELVEN_CLOAK);
		     (void)mongets(mtmp, LEATHER_GLOVES);
		     (void)mongets(mtmp, KNIFE);
		     if (!rn2(6)) (void)mongets(mtmp, KICKING_BOOTS);

			break;

		   case PM_PRIEST:
		   case PM_PRIESTESS:
		   case PM_UNDEAD_PRIEST:
		   case PM_UNDEAD_PRIESTESS:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, MACE);
		     (void)mongets(mtmp, rn1(ROBE_OF_WEAKNESS - ROBE + 1, ROBE));
		     (void)mongets(mtmp, SMALL_SHIELD);
		     if (!rn2(4)) {
			int v,vials;
			vials = rn2(4)+1;
			for (v=0;v<vials;v++) {
			  otmp = mksobj(POT_WATER, FALSE, FALSE);
			  if (otmp) {
				  bless(otmp);
				  mpickobj(mtmp, otmp, TRUE);
			  }
			}
		     }
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;
		   case PM_ROGUE:
		   case PM_UNDEAD_ROGUE:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, SHORT_SWORD);
		     (void)mongets(mtmp, LEATHER_ARMOR);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 25);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 25));
#endif
		   break;

		   case PM_ASSASSIN:
		   case PM_UNDEAD_ASSASSIN:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, DAGGER);
		     (void)mongets(mtmp, BOW);
			    m_initthrow(mtmp, ARROW, 25);
			    m_initthrow(mtmp, ARROW, 25);
		     (void)mongets(mtmp, LEATHER_ARMOR);
		     (void)mongets(mtmp, LEATHER_CLOAK);
		   break;

		   case PM_LOCKSMITH:
		   case PM_UNDEAD_LOCKSMITH:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		   break;

		   case PM_DQ_SLIME:
		   case PM_UNDEAD_DQ_SLIME:
		   case PM_ERDRICK:
		   case PM_UNDEAD_ERDRICK:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, BATTLE_STAFF);
		     (void)mongets(mtmp, LEATHER_ARMOR);
		     (void)mongets(mtmp, HELMET);

		   break;

		   case PM_BULLY:
		   case PM_UNDEAD_BULLY:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, KNIFE);
		     (void)mongets(mtmp, CLUB);
		     (void)mongets(mtmp, DUNCE_CAP);
		     (void)mongets(mtmp, T_SHIRT);

		   break;

		   case PM_PICKPOCKET:
		   case PM_UNDEAD_PICKPOCKET:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, KNIFE);
		     (void)mongets(mtmp, DAGGER);
		     (void)mongets(mtmp, LEATHER_GLOVES);
		     (void)mongets(mtmp, CLOAK_OF_DISPLACEMENT);

		   break;

		   case PM_RANGER:
		   case PM_ELPH:
		   case PM_UNDEAD_RANGER:
		   case PM_UNDEAD_ELPH:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, ELVEN_DAGGER);
		     (void)mongets(mtmp, ELVEN_BOW);
			    m_initthrow(mtmp, ELVEN_ARROW, 25);

		   break;

		   case PM_TWELPH:
		   case PM_UNDEAD_TWELPH:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, DARK_ELVEN_DAGGER);
		     (void)mongets(mtmp, DARK_ELVEN_BOW);
			    m_initthrow(mtmp, DARK_ELVEN_ARROW, 25);

		   break;

		   case PM_GOLDMINER:
		   case PM_UNDEAD_GOLDMINER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, PICK_AXE);
		     (void)mongets(mtmp, CROSSBOW);
			    m_initthrow(mtmp, CROSSBOW_BOLT, 25);

		   break;

		   case PM_MIDGET:
		   case PM_UNDEAD_MIDGET:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, DWARVISH_MATTOCK);
		     (void)mongets(mtmp, DWARVISH_IRON_HELM);
		     (void)mongets(mtmp, DWARVISH_MITHRIL_COAT);

		   break;

		   case PM_RINGSEEKER:
		   case PM_UNDEAD_RINGSEEKER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, SLING);
		     (void)mongets(mtmp, LEATHER_ARMOR);
			    m_initthrow(mtmp, ROCK, 25);
			    m_initthrow(mtmp, ROCK, 25);

		   break;

		   case PM_SHAPESHIFTER:
		   case PM_UNDEAD_SHAPESHIFTER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, WAN_POLYMORPH);

		   break;

		   case PM_NINJA:
		   case PM_UNDEAD_NINJA:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, SHORT_SWORD);
		     (void)mongets(mtmp, POT_INVISIBILITY);
			m_initthrow(mtmp, SHURIKEN, 25);
		   break;

		   case PM_SAMURAI:
		   case PM_UNDEAD_SAMURAI:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, KATANA);
		     if (rn2(2)) (void)mongets(mtmp, SHORT_SWORD);
		     if (rn2(3)) {
			(void)mongets(mtmp, YUMI);
			m_initthrow(mtmp, YA, 25);
		     }
		     (void)mongets(mtmp, SPLINT_MAIL);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_OTAKU:
		   case PM_UNDEAD_OTAKU:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, KATANA);
		     (void)mongets(mtmp, T_SHIRT);
		   break;

		   case PM_LIBRARIAN:
		   case PM_UNDEAD_LIBRARIAN:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, ROBE);

		   break;

		   case PM_FIGHTER:
		   case PM_UNDEAD_FIGHTER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, SHORT_SWORD);
		     (void)mongets(mtmp, SAILOR_BLOUSE);
		     (void)mongets(mtmp, WAN_POLYMORPH);

		   break;

		   case PM_FENCER:
		   case PM_UNDEAD_FENCER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, SHORT_SWORD);
		     (void)mongets(mtmp, DAGGER);
		     (void)mongets(mtmp, LEATHER_ARMOR);

		   break;

		   case PM_MEDIUM:
		   case PM_UNDEAD_MEDIUM:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, SILVER_DAGGER);
		     (void)mongets(mtmp, YUMI);
		     (void)mongets(mtmp, ROBE);
		     m_initthrow(mtmp, YA, 50);

		   break;

		   case PM_JUSTICE_KEEPER:
		   case PM_UNDEAD_JUSTICE_KEEPER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, MACE);
		     (void)mongets(mtmp, SPLINT_MAIL);

		   break;

		   case PM_STAND_USER:
		   case PM_UNDEAD_STAND_USER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		   break;

		   case PM_SEXYMATE:
		   case PM_UNDEAD_SEXYMATE:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, FEMININE_PUMPS);
		     (void)mongets(mtmp, SEXY_LEATHER_PUMP);

		   break;

		   case PM_LADIESMAN:
		   case PM_UNDEAD_LADIESMAN:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, KNIFE);
		     (void)mongets(mtmp, RUFFLED_SHIRT);
		     (void)mongets(mtmp, FEDORA);

		   break;

		   case PM_SUPERMARKET_CASHIER:
		   case PM_UNDEAD_SUPERMARKET_CASHIER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, LEATHER_JACKET);
		     (void)mongets(mtmp, KNIFE);
		     (void)mongets(mtmp, LOW_BOOTS);

		   break;

		   case PM_BLOODSEEKER:
		   case PM_UNDEAD_BLOODSEEKER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, SHORT_SWORD);
		     (void)mongets(mtmp, WAN_FIRE);
		     (void)mongets(mtmp, COMBAT_STILETTOS);

		   break;

		   case PM_SLAVE_MASTER:
		   case PM_UNDEAD_SLAVE_MASTER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		   break;

		   case PM_MYSTIC:
		   case PM_UNDEAD_MYSTIC:
			(void) mongets(mtmp, rnd_offensive_item(mtmp));
			(void) mongets(mtmp, rnd_offensive_item(mtmp));
			(void) mongets(mtmp, rnd_defensive_item(mtmp));
			(void) mongets(mtmp, rnd_defensive_item(mtmp));
			(void) mongets(mtmp, rnd_misc_item(mtmp));
			(void) mongets(mtmp, rnd_misc_item(mtmp));
			(void) mongets(mtmp, rnd_offensive_item_new(mtmp));
			(void) mongets(mtmp, rnd_offensive_item_new(mtmp));
			(void) mongets(mtmp, rnd_defensive_item_new(mtmp));
			(void) mongets(mtmp, rnd_defensive_item_new(mtmp));
			(void) mongets(mtmp, rnd_misc_item_new(mtmp));
			(void) mongets(mtmp, rnd_misc_item_new(mtmp));

		   break;

		   case PM_COOK:
		   case PM_UNDEAD_COOK:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, ROBE);
		     (void)mongets(mtmp, KNIFE);

		   break;

#ifdef TOURIST
		   case PM_TOURIST:
		   case PM_UNDEAD_TOURIST:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     m_initthrow(mtmp, DART, 20);
		     (void)mongets(mtmp, HAWAIIAN_SHIRT);
		     if (rn2(2)) (void)mongets(mtmp, EXPENSIVE_CAMERA);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 20);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 20));
#endif
		   break;
#endif
		   case PM_UNDEAD_SLAYER:
		   case PM_NON_UNDEAD_SLAYER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, SILVER_SPEAR);
		     (void)mongets(mtmp, CHAIN_MAIL);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_AUGURER:
		   case PM_UNDEAD_AUGURER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, LEATHER_ARMOR);

		   break;

		   case PM_POLITICIAN:
		   case PM_UNDEAD_POLITICIAN:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, LEATHER_JACKET);
		     (void)mongets(mtmp, T_SHIRT);
		     (void)mongets(mtmp, LOW_BOOTS);

		   break;

		   case PM_INTEL_SCRIBE:
		   case PM_UNDEAD_INTEL_SCRIBE:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, DAGGER);
		     if (!rn2(3)) (void)mongets(mtmp, CLOAK_OF_DISPLACEMENT);
		   break;

		   case PM_MAHOU_SHOUJO:
		   case PM_UNDEAD_MAHOU_SHOUJO:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, WAN_STRIKING);
		     if (!rn2(10)) (void)mongets(mtmp, WAN_BANISHMENT);
		     if (!rn2(2)) (void)mongets(mtmp, POT_PARALYSIS);
		     if (!rn2(2)) (void)mongets(mtmp, POT_CYANIDE);
		   break;

		   case PM_DOLL_MISTRESS:
		   case PM_UNDEAD_DOLL_MISTRESS:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, WORM_TOOTH);
		     (void)mongets(mtmp, DWARVISH_CLOAK);
		     (void)mongets(mtmp, VICTORIAN_UNDERWEAR);
		   break;

		   case PM_CRUEL_ABUSER:
		   case PM_UNDEAD_CRUEL_ABUSER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, BULLWHIP);
		     (void)mongets(mtmp, KNOUT);
		     (void)mongets(mtmp, IRON_CHAIN);
		     (void)mongets(mtmp, VICTORIAN_UNDERWEAR);
		     (void)mongets(mtmp, LEATHER_JACKET);
		     if (!rn2(6)) (void)mongets(mtmp, KICKING_BOOTS);
		   break;

		   case PM_UNDERTAKER:
		   case PM_UNDEAD_UNDERTAKER:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, SCALPEL);
		     (void)mongets(mtmp, VICTORIAN_UNDERWEAR);
		   break;

		   case PM_VALKYRIE:
		   case PM_UNDEAD_VALKYRIE:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));

		     (void)mongets(mtmp, LONG_SWORD);
		     if (!rn2(3)) m_initthrow(mtmp, DAGGER, 4);
		     (void)mongets(mtmp, SMALL_SHIELD);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_SAGE:
		   case PM_UNDEAD_SAGE:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, MACE);
		     (void)mongets(mtmp, ROBE);
		     (void)mongets(mtmp, POT_HEALING);
		     (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		   break;

		   case PM_WIZARD:
		   case PM_UNDEAD_WIZARD:
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(50)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, ATHAME);
		     (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;
		   default:
		   break;
		  } 
		  if ((int) mtmp->m_lev > rn2(120))
		     (void) mongets(mtmp, rnd_offensive_item(mtmp));
		  if (!rn2(200))
		     (void) mongets(mtmp, rnd_offensive_item(mtmp));
		  if ((int) mtmp->m_lev > rn2(120))
		     (void) mongets(mtmp, rnd_defensive_item(mtmp));
		  if (!rn2(200))
		     (void) mongets(mtmp, rnd_defensive_item(mtmp));
		  if ((int) mtmp->m_lev > rn2(120))
		     (void) mongets(mtmp, rnd_misc_item(mtmp));
		  if (!rn2(200))
		     (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(1000)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(1000)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(1000)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(1000)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(1000)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(1000)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		 } /* end of other characters */
	       /*break;*/
		else if (ptr->msound == MS_PRIEST ||
			quest_mon_represents_role(ptr,PM_PRIEST)) {

		    otmp = mksobj(MACE, FALSE, FALSE);
		    if(otmp) {
			otmp->spe = rnd(3);
			if(!rn2(2)) curse(otmp);
			(void) mpickobj(mtmp, otmp, TRUE);
		    }

		} else if (mm == PM_PIRATE_BROTHER){
			(void)mongets(mtmp, SCIMITAR);
			(void)mongets(mtmp, LEATHER_ARMOR);
			(void)mongets(mtmp, HIGH_BOOTS);

		} else if (mm == PM_SIZZLE){
		    otmp = mksobj(LONG_SWORD, FALSE, FALSE);
		    if (otmp) {
			int spe2;
			otmp = oname(otmp, artiname(ART_VORPAL_BLADE));
			bless(otmp);
			otmp->oerodeproof = TRUE;
			spe2 = rn2(4);
			otmp->spe = max(otmp->spe, spe2);
			(void) mpickobj(mtmp, otmp, TRUE);
		    }

		} else if (mm == PM_KATNISS){

			(void)mongets(mtmp, ELVEN_SPEAR);

		} else if (mm == PM_ARABELLA){

			(void)mongets(mtmp, HUGGING_BOOT);
			(void)mongets(mtmp, SPEED_BOOTS);
			(void)mongets(mtmp, WAN_BANISHMENT);

		} else if (mm == PM_ANASTASIA){

			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);
			(void)mongets(mtmp, DANCING_SHOES);

		} else if (mm == PM_HENRIETTA){

			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			(void)mongets(mtmp, PLASTEEL_BOOTS);

		} else if (mm == PM_KATRIN){

			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			(void)mongets(mtmp, FEMININE_PUMPS);

		} else if (mm == PM_JANA){

			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);
			(void)mongets(mtmp, ELVEN_BOOTS);

		} else if (mm == PM_THE_CHAMPION_OF_POMPEJI){

			(void)mongets(mtmp, STYGIAN_PIKE);
			(void)mongets(mtmp, STONE_DRAGON_SCALE_MAIL);

		} else if (mm == PM_DEATH_METAL_FREAK){
			if (rn2(2)) (void)mongets(mtmp, rn2(2) ? FIRE_HORN : FROST_HORN);
			(void)mongets(mtmp, SCR_LOCKOUT);
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));

		} else if (mm == PM_FREAKY_GIRL){
			if (rn2(2)) (void)mongets(mtmp, rn2(2) ? FIRE_HORN : FROST_HORN);
			(void)mongets(mtmp, SCR_LOCKOUT);
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));

		} else if (mm == PM_ACTIVIST){

			if (mtmp->female) (void)mongets(mtmp, VICTORIAN_UNDERWEAR);
			else (void)mongets(mtmp, RUFFLED_SHIRT);
			(void)mongets(mtmp, rnd_class(SCR_LAVA,SCR_LOCKOUT));

		} else if (mm == PM_HELEN_WANNABE){

			if (mtmp->female) (void)mongets(mtmp, VICTORIAN_UNDERWEAR);
			else (void)mongets(mtmp, RUFFLED_SHIRT);
			(void)mongets(mtmp, rnd_class(SCR_LAVA,SCR_LOCKOUT));

		} else if (mm == PM_ACTIVIST_LEADER || mm == PM_HELEN_THE_ACTIVIST_LEADER){

			if (mtmp->female) (void)mongets(mtmp, VICTORIAN_UNDERWEAR);
			else (void)mongets(mtmp, RUFFLED_SHIRT);
			(void)mongets(mtmp, rnd_class(SCR_LAVA,SCR_LOCKOUT));
			(void)mongets(mtmp, rnd_class(SCR_LAVA,SCR_LOCKOUT));
			(void)mongets(mtmp, rnd_class(SCR_LAVA,SCR_LOCKOUT));
			(void)mongets(mtmp, rnd_class(SCR_LAVA,SCR_LOCKOUT));
			(void)mongets(mtmp, rnd_class(SCR_LAVA,SCR_LOCKOUT));
			(void)mongets(mtmp, rnd_class(SCR_LAVA,SCR_LOCKOUT));
			(void)mongets(mtmp, rnd_class(SCR_LAVA,SCR_LOCKOUT));
			(void)mongets(mtmp, rnd_class(SCR_LAVA,SCR_LOCKOUT));
			(void)mongets(mtmp, rnd_class(SCR_LAVA,SCR_LOCKOUT));
			(void)mongets(mtmp, rnd_class(SCR_LAVA,SCR_LOCKOUT));
			(void)mongets(mtmp, rnd_class(SCR_LAVA,SCR_LOCKOUT));

		} else if (mm == PM_DEATH_METAL_DRUMMER){
			if (rn2(2)) (void)mongets(mtmp, rn2(2) ? FIRE_HORN : FROST_HORN);
			(void)mongets(mtmp, SCR_LOCKOUT);
		     (void)mongets(mtmp, SUBMACHINE_GUN);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));

		} else if (mm == PM_WILD_WOMAN){
			if (rn2(2)) (void)mongets(mtmp, rn2(2) ? FIRE_HORN : FROST_HORN);
			(void)mongets(mtmp, SCR_LOCKOUT);
		     (void)mongets(mtmp, SUBMACHINE_GUN);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));

		} else if (mm == PM_DEATH_METAL_ORCHESTRA_LEADER){
			if (rn2(2)) (void)mongets(mtmp, rn2(2) ? FIRE_HORN : FROST_HORN);
			(void)mongets(mtmp, SCR_LOCKOUT);
			(void)mongets(mtmp, SCR_LOCKOUT);
			(void)mongets(mtmp, SCR_LOCKOUT);
			(void)mongets(mtmp, SCR_LOCKOUT);
			(void)mongets(mtmp, SCR_LOCKOUT);
		     (void)mongets(mtmp, HEAVY_MACHINE_GUN);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
			(void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));
			(void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));

		} else if (mm == PM_DRACO_THE_SHARPSHOOTER){
		     (void)mongets(mtmp, SNIPER_RIFLE);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
			(void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));
			(void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));

		} else if (mm == PM_GRANDPA_TANK_BREAKER){
		     (void)mongets(mtmp, ROCKET_LAUNCHER);
			 m_initthrow(mtmp, ROCKET, 10);
		     (void)mongets(mtmp, REALLY_HEAVY_IRON_BALL);
			(void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
			(void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));
			(void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));

		} else if (mm == PM_DANCING_GUY){
			(void)mongets(mtmp, DANCING_SHOES);

		} else if (mm == PM_DANCING_GIRL){
			(void)mongets(mtmp, DANCING_SHOES);

		} else if (mm == PM_SEXY_GIRL){
			(void)mongets(mtmp, DANCING_SHOES);

		} else if (mm == PM_DANCING_KING){
			(void)mongets(mtmp, LACQUERED_DANCING_SHOE);
			(void)mongets(mtmp, DANCING_SHOES);

		} else if (mm == PM_DANCING_QUEEN){
			(void)mongets(mtmp, LACQUERED_DANCING_SHOE);
			(void)mongets(mtmp, DANCING_SHOES);

		} else if (mm == PM_SEXY_LADY){
			(void)mongets(mtmp, LACQUERED_DANCING_SHOE);
			(void)mongets(mtmp, DANCING_SHOES);

		} else if (mm == PM_ASIAN_GIRL){
			(void)mongets(mtmp, HUGGING_BOOT);

		} else if (mm == PM_CHINESE_GIRL){
			(void)mongets(mtmp, HUGGING_BOOT);

		} else if (mm == PM_JAPANESE_GIRL){
			(void)mongets(mtmp, WOODEN_GETA);

		} else if (mm == PM_ANIME_GIRL){
			(void)mongets(mtmp, ATSUZOKO_BOOTS);

		} else if (mm == PM_KOREAN_GIRL){
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

		} else if (mm == PM_VIETNAMESE_GIRL){
			(void)mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);

		} else if (mm == PM_TURKISH_GIRL){
			(void)mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);

		} else if (mm == PM_PERSIAN_GIRL){
			(void)mongets(mtmp, SEXY_LEATHER_PUMP);

		} else if (mm == PM_TENDER_ASIAN_GIRL){
			(void)mongets(mtmp, HUGGING_BOOT);

		} else if (mm == PM_ESTRELLA_GIRL){
			(void)mongets(mtmp, SWEET_MOCASSINS);

		} else if (mm == PM_SWEET_COQUETTE){
			(void)mongets(mtmp, SWEET_MOCASSINS);

		} else if (mm == PM_DARK_GIRL){
			(void)mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);

		} else if (mm == PM_DARK_WOMAN){
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			(void)mongets(mtmp, DANCING_SHOES);

		} else if (mm == PM_DARK_LADY){
			(void)mongets(mtmp, LACQUERED_DANCING_SHOE);
			(void)mongets(mtmp, LEATHER_PEEP_TOES);

		} else if (mm == PM_SEXY_CHILD_MODEL){
			(void)mongets(mtmp, HIGH_STILETTOS);

		} else if (mm == PM_SEXY_MODEL){
			(void)mongets(mtmp, HIGH_STILETTOS);
			(void)mongets(mtmp, SEXY_LEATHER_PUMP);

		} else if (mm == PM_SKATER_GIRL){
			(void)mongets(mtmp, ROLLER_BLADE);

		} else if (mm == PM_SKATER_WOMAN){
			(void)mongets(mtmp, ROLLER_BLADE);
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);

		} else if (mm == PM_MYSTERIOUS_GIRL){
			(void)mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);

		} else if (mm == PM_MYSTERIOUS_WOMAN){
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			(void)mongets(mtmp, DANCING_SHOES);

		} else if (mm == PM_MYSTERIOUS_LADY){
			(void)mongets(mtmp, LACQUERED_DANCING_SHOE);
			(void)mongets(mtmp, LEATHER_PEEP_TOES);

		} else if (mm == PM_REDGUARD_GIRL){
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);
			(void)mongets(mtmp, AMULET_OF_LIFE_SAVING);

		} else if (mm == PM_REDGUARD_WOMAN){
			(void)mongets(mtmp, HUGGING_BOOT);
			(void)mongets(mtmp, WEDGE_SANDALS);
			(void)mongets(mtmp, AMULET_OF_LIFE_SAVING);

		} else if (mm == PM_REDGUARD_LADY){
			(void)mongets(mtmp, SEXY_LEATHER_PUMP);
			(void)mongets(mtmp, FEMININE_PUMPS);
			(void)mongets(mtmp, AMULET_OF_LIFE_SAVING);

		} else if (mm == PM_ODOROUS_GIRL){
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);
			(void)mongets(mtmp, AMULET_OF_LIFE_SAVING);

		} else if (mm == PM_ODOROUS_WOMAN){
			(void)mongets(mtmp, HUGGING_BOOT);
			(void)mongets(mtmp, WEDGE_SANDALS);
			(void)mongets(mtmp, AMULET_OF_LIFE_SAVING);

		} else if (mm == PM_ODOROUS_LADY){
			(void)mongets(mtmp, SEXY_LEATHER_PUMP);
			(void)mongets(mtmp, FEMININE_PUMPS);
			(void)mongets(mtmp, AMULET_OF_LIFE_SAVING);

		} else if (mm == PM_THIEVING_GIRL){
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);

		} else if (mm == PM_THIEVING_WOMAN){
			(void)mongets(mtmp, LACQUERED_DANCING_SHOE);
			(void)mongets(mtmp, SOFT_SNEAKERS);

		} else if (mm == PM_THIEVING_LADY){
			(void)mongets(mtmp, SPIKED_BATTLE_BOOT);
			(void)mongets(mtmp, COMBAT_STILETTOS);

		} else if (mm == PM_CRAPPY_BRAT){
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);

		} else if (mm == PM_SPOILED_PRINCESS){
			(void)mongets(mtmp, LACQUERED_DANCING_SHOE);
			(void)mongets(mtmp, SOFT_SNEAKERS);

		} else if (mm == PM_HAUGHTY_BITCH){
			(void)mongets(mtmp, SPIKED_BATTLE_BOOT);
			(void)mongets(mtmp, COMBAT_STILETTOS);

		} else if (mm == PM_KICKBOXING_GIRL){
			(void)mongets(mtmp, SOFT_SNEAKERS);
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);

		} else if (mm == PM_KICKBOXING_LADY){
			(void)mongets(mtmp, SOFT_SNEAKERS);
			(void)mongets(mtmp, WOODEN_GETA);

		} else if (mm == PM_FORSAKEN_GIRL){
			(void)mongets(mtmp, SOFT_SNEAKERS);
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);

		} else if (mm == PM_FORSAKEN_LADY){
			(void)mongets(mtmp, SOFT_SNEAKERS);
			(void)mongets(mtmp, WOODEN_GETA);

		} else if (mm == PM_ELONA_GIRL){
			(void)mongets(mtmp, SOFT_SNEAKERS);
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);

		} else if (mm == PM_ELONA_LADY){
			(void)mongets(mtmp, SOFT_SNEAKERS);
			(void)mongets(mtmp, WOODEN_GETA);

		} else if (mm == PM_ESTRELLARINA){
			(void)mongets(mtmp, SWEET_MOCASSINS);

		} else if (mm == PM_EROTICITY_QUEEN){
			(void)mongets(mtmp, SWEET_MOCASSINS);

		} else if (mm == PM_PROSTITUTE){
			(void)mongets(mtmp, BULLWHIP);
			(void)mongets(mtmp, HIPPIE_HEELS);

		} else if (mm == PM_STRIPPER_GIRL){
			(void)mongets(mtmp, BULLWHIP);
			(void)mongets(mtmp, HIPPIE_HEELS);
			(void)mongets(mtmp, SUBMACHINE_GUN);
		  	m_initthrow(mtmp, BULLET, 50);

		} else if (mm == PM_STRIPPER_LADY){
			(void)mongets(mtmp, KNIFE);
			(void)mongets(mtmp, HIPPIE_HEELS);
			(void)mongets(mtmp, ASSAULT_RIFLE);
		  	m_initthrow(mtmp, BULLET, 50);

		} else if (mm == PM_THICK_PROSTITUTE){
			(void)mongets(mtmp, BULLWHIP);
			(void)mongets(mtmp, HIPPIE_HEELS);

		} else if (mm == PM_BLACK_PROSTITUTE){
			(void)mongets(mtmp, FLY_SWATTER);
			(void)mongets(mtmp, HIPPIE_HEELS);
			(void)mongets(mtmp, VICTORIAN_UNDERWEAR);

		} else if (mm == PM_LEOPARD_DRESS_PROSTITUTE){
			(void)mongets(mtmp, FLY_SWATTER);
			(void)mongets(mtmp, HIPPIE_HEELS);
			(void)mongets(mtmp, VICTORIAN_UNDERWEAR);

		} else if (mm == PM_SADISTIC_PROSTITUTE){
			(void)mongets(mtmp, FLY_SWATTER);
			(void)mongets(mtmp, HIPPIE_HEELS);
			(void)mongets(mtmp, VICTORIAN_UNDERWEAR);

		} else if (mm == PM_GREEN_PROSTITUTE){
			(void)mongets(mtmp, KNIFE);
			(void)mongets(mtmp, HIPPIE_HEELS);
			(void)mongets(mtmp, ROBE);

		} else if (mm == PM_VIOLET_PROSTITUTE){
			(void)mongets(mtmp, KNIFE);
			(void)mongets(mtmp, HIPPIE_HEELS);
			(void)mongets(mtmp, ROBE);

		} else if (mm == PM_STRING_TANGA_PROSTITUTE){
			(void)mongets(mtmp, KNIFE);
			(void)mongets(mtmp, HIPPIE_HEELS);
			(void)mongets(mtmp, ROBE);

		} else if (mm == PM_CIGARETTE_SMOKING_PROSTITUTE){
			(void)mongets(mtmp, KNIFE);
			(void)mongets(mtmp, HIPPIE_HEELS);
			(void)mongets(mtmp, ROBE);

		} else if (mm == PM_SECRETARY){
			(void)mongets(mtmp, KNIFE);
			(void)mongets(mtmp, PISTOL);
		  	m_initthrow(mtmp, BULLET, 50);
			(void)mongets(mtmp, FEMININE_PUMPS);
			(void)mongets(mtmp, VICTORIAN_UNDERWEAR);

		} else if (mm == PM_STUDENT_ASSISTANT){
			(void)mongets(mtmp, HIPPIE_HEELS);

		} else if (mm == PM_ANTISOCIAL_WOMAN){
			(void)mongets(mtmp, HIPPIE_HEELS);

		} else if (mm == PM_STUDENT_LEADER){
			(void)mongets(mtmp, HIPPIE_HEELS);
			(void)mongets(mtmp, SCR_CREATE_MONSTER);
			(void)mongets(mtmp, SCR_CREATE_MONSTER);
			(void)mongets(mtmp, SCR_CREATE_MONSTER);
			(void)mongets(mtmp, WAN_CREATE_HORDE);
			(void)mongets(mtmp, WAN_SUMMON_UNDEAD);

		} else if (mm == PM_HIPPIE_LADY){
			(void)mongets(mtmp, HIPPIE_HEELS);

		} else if (mm == PM_SPUTA_FLOWING_WOMAN){
			(void)mongets(mtmp, HIPPIE_HEELS);

		} else if (mm == PM_HIPPIE_GUY){
			(void)mongets(mtmp, HIPPIE_HEELS);

		} else if (mm == PM_SWEET_WOMAN){
			(void)mongets(mtmp, SWEET_MOCASSINS);
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

		} else if (mm == PM_SEXY_WOMAN){
			(void)mongets(mtmp, SWEET_MOCASSINS);
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

		} else if (mm == PM_PETTY_SWEET_WOMAN){
			(void)mongets(mtmp, SWEET_MOCASSINS);
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

		} else if (mm == PM_LITTLE_GIRL){
			(void)mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);

		} else if (mm == PM_LITTLE_BOY){
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);

		} else if (mm == PM_FARTING_GIRL){
			(void)mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);

		} else if (mm == PM_FARTING_BOY){
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);

		} else if (mm == PM_ASIAN_WOMAN){
			(void)mongets(mtmp, HUGGING_BOOT);
			(void)mongets(mtmp, LEATHER_PEEP_TOES);

		} else if (mm == PM_SEXY_CHINESE_WOMAN){
			(void)mongets(mtmp, HUGGING_BOOT);
			(void)mongets(mtmp, LEATHER_PEEP_TOES);

		} else if (mm == PM_JAPANESE_WOMAN){
			(void)mongets(mtmp, WOODEN_GETA);
			(void)mongets(mtmp, WEDGE_SANDALS);

		} else if (mm == PM_ANIME_WOMAN){
			(void)mongets(mtmp, ATSUZOKO_BOOTS);
			(void)mongets(mtmp, WOODEN_GETA);

		} else if (mm == PM_MANGA_WOMAN){
			(void)mongets(mtmp, WOODEN_GETA);

		} else if (mm == PM_KOREAN_WOMAN){
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			(void)mongets(mtmp, HIPPIE_HEELS);

		} else if (mm == PM_VIETNAMESE_WOMAN){
			(void)mongets(mtmp, SEXY_LEATHER_PUMP);
			(void)mongets(mtmp, FEMININE_PUMPS);

		} else if (mm == PM_TURKISH_WOMAN){
			(void)mongets(mtmp, SPIKED_BATTLE_BOOT);
			(void)mongets(mtmp, COMBAT_STILETTOS);

		} else if (mm == PM_PERSIAN_WOMAN){
			(void)mongets(mtmp, SEXY_LEATHER_PUMP);
			(void)mongets(mtmp, SWEET_MOCASSINS);

		} else if (mm == PM_PRETTY_ASIAN_WOMAN){
			(void)mongets(mtmp, HUGGING_BOOT);
			(void)mongets(mtmp, LEATHER_PEEP_TOES);

		} else if (mm == PM_ASIAN_LADY){
			(void)mongets(mtmp, WOODEN_GETA);
			(void)mongets(mtmp, LEATHER_PEEP_TOES);

		} else if (mm == PM_CHINESE_LADY){
			(void)mongets(mtmp, HUGGING_BOOT);
			(void)mongets(mtmp, LEATHER_PEEP_TOES);

		} else if (mm == PM_JAPANESE_LADY){
			(void)mongets(mtmp, WOODEN_GETA);
			(void)mongets(mtmp, SOFT_SNEAKERS);

		} else if (mm == PM_ANIME_LADY){
			(void)mongets(mtmp, ATSUZOKO_BOOTS);
			(void)mongets(mtmp, WOODEN_GETA);

		} else if (mm == PM_MANGA_LADY){
			(void)mongets(mtmp, WOODEN_GETA);

		} else if (mm == PM_KOREAN_LADY){
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			(void)mongets(mtmp, COMBAT_STILETTOS);

		} else if (mm == PM_VIETNAMESE_LADY){
			(void)mongets(mtmp, SEXY_LEATHER_PUMP);
			(void)mongets(mtmp, SOFT_SNEAKERS);

		} else if (mm == PM_TURKISH_LADY){
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			(void)mongets(mtmp, COMBAT_STILETTOS);

		} else if (mm == PM_PERSIAN_LADY){
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			(void)mongets(mtmp, FEMININE_PUMPS);

		} else if (mm == PM_SEXY_ASIAN_LADY){
			(void)mongets(mtmp, WOODEN_GETA);
			(void)mongets(mtmp, LEATHER_PEEP_TOES);

		} else if (mm == PM_YOUNG_ADULT_GIRL){
			(void)mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);
			(void)mongets(mtmp, WEDGE_SANDALS);

		} else if (mm == PM_YOUNG_ADULT_BOY){
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);
			(void)mongets(mtmp, WEDGE_SANDALS);

		} else if (mm == PM_YOUNG_WOMAN){
			(void)mongets(mtmp, HIGH_HEELED_SANDAL);
			(void)mongets(mtmp, FEMININE_PUMPS);

		} else if (mm == PM_YOUNG_MAN){
			(void)mongets(mtmp, HIGH_HEELED_SANDAL);
			(void)mongets(mtmp, WEDGE_SANDALS);

		} else if (mm == PM_FARTING_WOMAN){
			(void)mongets(mtmp, HIGH_HEELED_SANDAL);
			(void)mongets(mtmp, FEMININE_PUMPS);

		} else if (mm == PM_FARTING_MAN){
			(void)mongets(mtmp, HIGH_HEELED_SANDAL);
			(void)mongets(mtmp, WEDGE_SANDALS);

		} else if (mm == PM_ADULT_LADY){
			(void)mongets(mtmp, SEXY_LEATHER_PUMP);
			(void)mongets(mtmp, FEMININE_PUMPS);

		} else if (mm == PM_ADULT_GENTLEMAN){
			(void)mongets(mtmp, SPIKED_BATTLE_BOOT);
			(void)mongets(mtmp, COMBAT_STILETTOS);

		} else if (mm == PM_OLD_GRANNY){
			(void)mongets(mtmp, SEXY_LEATHER_PUMP);
			(void)mongets(mtmp, FEMININE_PUMPS);

		} else if (mm == PM_OLD_GRANDPA){
			(void)mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
			(void)mongets(mtmp, COMBAT_STILETTOS);

		} else if (mm == PM_FARTING_QUEEN){
			(void)mongets(mtmp, SEXY_LEATHER_PUMP);
			(void)mongets(mtmp, FEMININE_PUMPS);

		} else if (mm == PM_FARTING_KING){
			(void)mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
			(void)mongets(mtmp, COMBAT_STILETTOS);

		} else if (mm == PM_CHARMING_SCRATCHER){
			(void)mongets(mtmp, FEMININE_PUMPS);

		} else if (mm == PM_NICE_OLD_LADY){
			(void)mongets(mtmp, FEMININE_PUMPS);

		} else if (mm == PM_KUNG_FU_GIRL){
			(void)mongets(mtmp, WEDGE_SANDALS);

		} else if (mm == PM_NINJA_GIRL){
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

		} else if (mm == PM_YOGA_GIRL){
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

		} else if (mm == PM_NINJA_BOY){
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

		} else if (mm == PM_TAEKWONDO_GIRL){
			(void)mongets(mtmp, WOODEN_GETA);
			(void)mongets(mtmp, LEATHER_PEEP_TOES);

		} else if (mm == PM_TAI_CHI_GIRL){
			(void)mongets(mtmp, WOODEN_GETA);
			(void)mongets(mtmp, LEATHER_PEEP_TOES);

		} else if (mm == PM_BLACKBELT){
			(void)mongets(mtmp, SPIKED_BATTLE_BOOT);
			(void)mongets(mtmp, COMBAT_STILETTOS);

		} else if (mm == PM_KUNG_FU_GUY){
			(void)mongets(mtmp, WEDGE_SANDALS);

		} else if (mm == PM_FLYING_GIRL){
			(void)mongets(mtmp, COMBAT_STILETTOS);

		} else if (mm == PM_FLYING_GUY){
			(void)mongets(mtmp, COMBAT_STILETTOS);

		} else if (mm == PM_DRAGONBALL_KID){

			(void) mongets(mtmp, rnd_class(PLASTEEL_BOOTS,LEVITATION_BOOTS));

		} else if (mm == PM_KAMEHAMEHA_FIGHTER){

			(void) mongets(mtmp, rnd_class(PLASTEEL_BOOTS,LEVITATION_BOOTS));

			if (!rn2(5)) mongets(mtmp, rnd_offensive_item_new(mtmp));

		} else if (mm == PM_SUPER_SAIYAN){

			mongets(mtmp, rnd_offensive_item_new(mtmp));
			(void) mongets(mtmp, rnd_class(PLASTEEL_BOOTS,LEVITATION_BOOTS));

		} else if (mm == PM_MAYOR_CUMMERBUND){

			int spe2;
		    otmp = mksobj(SCIMITAR, FALSE, FALSE);
		    if (otmp) {
			curse(otmp);
			otmp->oerodeproof = TRUE;
			spe2 = d(1,3);
			otmp->spe = max(otmp->spe, spe2);
			(void) mpickobj(mtmp, otmp, TRUE);
		    }
			
		    otmp = mksobj(LEATHER_JACKET, FALSE, FALSE);
		    if (otmp) {
			otmp->oerodeproof = TRUE;
			spe2 = d(2,3);
			otmp->spe = max(otmp->spe, spe2);
			(void) mpickobj(mtmp, otmp, TRUE);
		    }

		    otmp = mksobj(SMALL_SHIELD, FALSE, FALSE);
		    if (otmp) {
			otmp->oerodeproof = TRUE;
			spe2 = d(1,3);
			otmp->spe = max(otmp->spe, spe2);
			(void) mpickobj(mtmp, otmp, TRUE);
		    }

			(void)mongets(mtmp, LEATHER_CLOAK);
			(void)mongets(mtmp, HIGH_BOOTS);
			(void)mongets(mtmp, LEATHER_GLOVES);
		}

#ifdef CONVICT
		else if (mm == PM_MINER) {

		    (void)mongets(mtmp, PICK_AXE);
		    otmp = mksobj(BRASS_LANTERN, TRUE, FALSE);
			if (otmp) (void) mpickobj(mtmp, otmp, TRUE);
	            /*begin_burn(otmp, FALSE);*/
		}
#endif /* CONVICT */

		break;

		case S_DOG:

			if(mm == PM_SPIT_DEMON){ (void)mongets(mtmp, SLING);
		  	  m_initthrow(mtmp, ROCK, 5);
			}

			if(mm == PM_CORPULENT_DOG || mm == PM_THICK_POTATO){ (void)mongets(mtmp, ROCKET_LAUNCHER);
		  	  m_initthrow(mtmp, ROCKET, 1);
			}
			if(mm == PM_CORPSE_SPITTER || mm == PM_BLACK_MUZZLE){ (void)mongets(mtmp, ROCKET_LAUNCHER);
		  	  m_initthrow(mtmp, ROCKET, 2);
			}
			if(mm == PM_MUZZLE_FIEND || mm == PM_MAW_FIEND){ (void)mongets(mtmp, ROCKET_LAUNCHER);
		  	  m_initthrow(mtmp, ROCKET, 3);
			}
			if(mm == PM_ROCKET_MUZZLE){ (void)mongets(mtmp, ROCKET_LAUNCHER);
		  	  m_initthrow(mtmp, ROCKET, 4);
			}

		break;

		case S_GHOST:
		if(mm == PM_BLACKBEARD_S_GHOST){

			int spe2;
		    otmp = mksobj(SCIMITAR, FALSE, FALSE);
		    if (otmp) {
			curse(otmp);
			otmp->oerodeproof = TRUE;
			otmp->oeroded = 1;
			spe2 = d(2,3);
			otmp->spe = max(otmp->spe, spe2);
			(void) mpickobj(mtmp, otmp, TRUE);
		    }

		    /* MRKR: Dwarves in the Mines sometimes carry torches */

		    if (In_mines(&u.uz)) {
		      if (!rn2(4)) {	
			otmp = mksobj(TORCH, TRUE, FALSE);
			if (otmp) {
				otmp->quan = 1;
				(void) mpickobj(mtmp, otmp, TRUE);
			}

			/* If this spot is unlit, light the torch */

			/*if (!levl[mtmp->mx][mtmp->my].lit) {
			  begin_burn(otmp, FALSE);
			}*/
			/* was causing weird error messages, so let's just disable it for now --Amy */
		      
		      }
		    }
		} else if (mm == PM_IMELDA_S_GHOST) {

		    (void)mongets(mtmp, LEATHER_PEEP_TOES);
		} 

		if (mm == PM_POLICE) {
				(void)mongets(mtmp, RUBBER_HOSE);
				m_initthrow(mtmp, CREAM_PIE, 2);
		}

		break;

	    case S_ANGEL:
		{

			/* preventing low level angels from generating with powerful stuff that players can claim --Amy */
			if (mtmp->m_lev < rnd(100)) break;

		    int spe2;

		    /* create minion stuff; can't use mongets */
		    otmp = mksobj(LONG_SWORD, FALSE, FALSE);

		    if (otmp) {

			/* maybe make it special */
			if (!rn2(20) || is_lord(ptr))
				otmp = oname(otmp, artiname(rn2(2) ? ART_DEMONBANE : ART_SUNSWORD));
			bless(otmp);
			otmp->oerodeproof = TRUE;
			spe2 = rn2(4);
			otmp->spe = max(otmp->spe, spe2);
			(void) mpickobj(mtmp, otmp, TRUE);

			otmp = mksobj(!rn2(4) || is_lord(ptr) ?
				  SHIELD_OF_REFLECTION : LARGE_SHIELD,
				  FALSE, FALSE);
			otmp->cursed = otmp->hvycurse = otmp->prmcurse = FALSE;
			otmp->oerodeproof = TRUE;
			otmp->spe = 0;
			(void) mpickobj(mtmp, otmp, TRUE);

		    }
		}
		break;

	    case S_GOLEM:
		{
			if (mm == PM_ANIMATED_WEDGE_SANDAL) (void)mongets(mtmp, WEDGE_SANDALS);
			else if (mm == PM_ANIMATED_SYNTHETIC_SANDAL) (void)mongets(mtmp, SYNTHETIC_SANDALS);
			else if (mm == PM_ANIMATED_SEXY_LEATHER_PUMP) (void)mongets(mtmp, FEMININE_PUMPS);
			else if (mm == PM_ANIMATED_UNFAIR_STILETTO) (void)mongets(mtmp, UNFAIR_STILETTOS);
			else if (mm == PM_ANIMATED_SKY_HIGH_HEEL) (void)mongets(mtmp, SKY_HIGH_HEELS);
			else if (mm == PM_ANIMATED_BEAUTIFUL_FUNNEL_HEELED_PUMP) (void)mongets(mtmp, FEMININE_PUMPS);
			else if (mm == PM_ANIMATED_LEATHER_PEEP_TOE) (void)mongets(mtmp, LEATHER_PEEP_TOES);
			else if (mm == PM_ANIMATED_COMBAT_STILETTO) (void)mongets(mtmp, COMBAT_STILETTOS);
			else if (mm == PM_ANIMATED_ATSUZOKO_BOOT) (void)mongets(mtmp, ATSUZOKO_BOOTS);
			else if (mm == PM_ANIMATED_STILETTO_SANDAL) (void)mongets(mtmp, HIGH_STILETTOS);
			else if (mm == PM_ANIMATED_ROLLER_BLADE) (void)mongets(mtmp, ROLLER_BLADE);
			else if (mm == PM_ANIMATED_PROSTITUTE_SHOE) (void)mongets(mtmp, HIPPIE_HEELS);

			if (mm == PM_ATHENA_GUARDIAN) (void)mongets(mtmp, HEAVY_HAMMER);
			if (mm == PM_ATHENA_GIANT) (void)mongets(mtmp, MALLET);

			if (mm == PM_CLOCK_GOLEM) (void)mongets(mtmp, WAN_TIME);

			if (mm == PM_FLYING_SCIMITAR) (void)mongets(mtmp, SCIMITAR);
			if (mm == PM_WHIRLING_HATCHET) (void)mongets(mtmp, AXE);
			if (mm == PM_CURSED_SWORD) (void)mongets(mtmp, ELVEN_SHORT_SWORD);
			if (mm == PM_ANIMATED_DAGGER) (void)mongets(mtmp, rn2(20) ? DAGGER : SILVER_DAGGER);
			if (mm == PM_SWORD_FAMILIAR) (void)mongets(mtmp, SHORT_SWORD);
			if (mm == PM_HAMMER_OF_THE_UNDERWORLD) (void)mongets(mtmp, MALLET);
			if (mm == PM_HYPERTYPE) (void)mongets(mtmp, WAN_TRAP_CREATION);
			if (mm == PM_LEVEL____ENEMY) (void)mongets(mtmp, SCR_TRAP_CREATION);
			if (mm == PM_SECURITY_BOT) {(void)mongets(mtmp, ASSAULT_RIFLE);
		  	  m_initthrow(mtmp, BULLET, 25);
			}

		}
		break;

	    case S_IMP:

		switch (mm) {
			case PM_LEGION_DEVIL_GRUNT:



#ifdef FIREARMS
			  (void) mongets(mtmp, rn2(2) ? RIFLE : SUBMACHINE_GUN);
		  	  m_initthrow(mtmp, BULLET, 25);
		  	  m_initthrow(mtmp, BULLET, 25);
			  (void) mongets(mtmp, rn2(2) ? KNIFE : DAGGER);
			  if (rn2(2)) (void) mongets(mtmp, FRAG_GRENADE);
			  break;
#endif
			case PM_LEGION_DEVIL_SOLDIER:



#ifdef FIREARMS
			  if (rn2(2)) {
			  	(void) mongets(mtmp, HEAVY_MACHINE_GUN);
			  	m_initthrow(mtmp, BULLET, 50);
			  	m_initthrow(mtmp, BULLET, 50);
			  	m_initthrow(mtmp, BULLET, 50);
			  } else {
			  	(void) mongets(mtmp, SUBMACHINE_GUN);
			  	m_initthrow(mtmp, BULLET, 30);
			  	m_initthrow(mtmp, BULLET, 30);
			  }
			  (void) mongets(mtmp, rn2(2) ? KNIFE : DAGGER);
			  if (!rn2(4)) {
			  	(void) mongets(mtmp, FRAG_GRENADE);
			  	(void) mongets(mtmp, FRAG_GRENADE);
			  } else if (!rn2(4)) {
			  	(void) mongets(mtmp, GAS_GRENADE);
			  	(void) mongets(mtmp, GAS_GRENADE);
			  }
			  break;
#endif
			case PM_LEGION_DEVIL_SERGEANT:



#ifdef FIREARMS
			  if (rn2(2)) {
			  	(void) mongets(mtmp, AUTO_SHOTGUN);
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 10);
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 10);
			  } else {
			  	(void) mongets(mtmp, ASSAULT_RIFLE);
			  	m_initthrow(mtmp, BULLET, 30);
			  	m_initthrow(mtmp, BULLET, 30);
			  }
			  (void) mongets(mtmp, rn2(2) ? DAGGER : KNIFE);
			  if (!rn2(5)) {
			  	m_initthrow(mtmp, FRAG_GRENADE, 5);
			  	if (!rn2(5)) (void) mongets(mtmp, GRENADE_LAUNCHER);
			  } else if (!rn2(5)) {
			  	m_initthrow(mtmp, GAS_GRENADE, 5);
			  	if (!rn2(5)) (void) mongets(mtmp, GRENADE_LAUNCHER);
			  }
			  break;
#endif
			case PM_LEGION_DEVIL_CAPTAIN:
			case PM_LEGION_DEVIL_GENERAL:

#ifdef FIREARMS
			  if (rn2(2)) {
			  	(void) mongets(mtmp, AUTO_SHOTGUN);
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  } else if (rn2(2)) {
			  	(void) mongets(mtmp, HEAVY_MACHINE_GUN);
			  	m_initthrow(mtmp, BULLET, 60);
			  	m_initthrow(mtmp, BULLET, 60);
			  	m_initthrow(mtmp, BULLET, 60);
			  } else {
			  	(void) mongets(mtmp, ASSAULT_RIFLE);
			  	m_initthrow(mtmp, BULLET, 60);
			  	m_initthrow(mtmp, BULLET, 60);
			  }
			  if (rn2(2)) {
				  (void) mongets(mtmp, ROCKET_LAUNCHER);
			  	  m_initthrow(mtmp, ROCKET, 5);
			  } else if (!rn2(4)) {
				  (void) mongets(mtmp, GRENADE_LAUNCHER);			  
			  	  m_initthrow(mtmp, 
			  	  	(rn2(2) ? FRAG_GRENADE : GAS_GRENADE), 
			  	  	5);
			  } else {
				  (void) mongets(mtmp, rn2(2) ? SILVER_SABER : DAGGER);
			  }
			  break;
#endif
		    default:
			/*m_initweap_normal(mtmp);*/
			break;
		}	
		break;

	    case S_QUADRUPED:

			if(ptr == &mons[PM_SANTA_CLAUS]) (void) mongets(mtmp, MACE);
			if(ptr == &mons[PM_HELL_BULL]) (void) mongets(mtmp, LUCERN_HAMMER);

			if(ptr == &mons[PM_CHAINED_CLAN]) (void) mongets(mtmp, ROTATING_CHAIN);
			if(ptr == &mons[PM_TUNDRA_CLAN]) (void) mongets(mtmp, SCR_ICE);
			if(ptr == &mons[PM_BURNING_CLAN]) (void) mongets(mtmp, POT_FIRE);
			if(ptr == &mons[PM_CIRCLE_CLAN]) (void) mongets(mtmp, SCR_TELEPORTATION);
			if(ptr == &mons[PM_SMOKE_CLAN]) (void) mongets(mtmp, SCR_FIRE);

			if(ptr == &mons[PM_FOREST_CLAN]) (void) mongets(mtmp, SCR_GROWTH);
			if(ptr == &mons[PM_MARSH_CLAN]) (void) mongets(mtmp, SCR_FLOOD);
			if(ptr == &mons[PM_ASH_CLAN]) (void) mongets(mtmp, FRAG_GRENADE);
			if(ptr == &mons[PM_TOMB_CLAN]) (void) mongets(mtmp, SCR_SUMMON_UNDEAD);
			if(ptr == &mons[PM_LATTICE_CLAN]) (void) mongets(mtmp, SCR_BARRHING);
			if(ptr == &mons[PM_HELL_CLAN]) (void) mongets(mtmp, SCR_LAVA);

			if(ptr == &mons[PM_LAIR_TOUGHGUY]) (void) mongets(mtmp, BASEBALL_BAT);
			if(ptr == &mons[PM_HARD_HITTING_DUDE]) (void) mongets(mtmp, HEAVY_HAMMER);

		break;

	    case S_TRAPPER:

			if(ptr == &mons[PM_SMART_TRAPPER]) (void) mongets(mtmp, SCR_TRAP_CREATION);
			if(ptr == &mons[PM_TRAPPER_KING]) (void) mongets(mtmp, WAN_TRAP_CREATION);

			if(ptr == &mons[PM_GRAY_SNIPER]) {
				(void) mongets(mtmp, SLING);
				(void) mongets(mtmp, rnd_class(RIGHT_MOUSE_BUTTON_STONE,NASTY_STONE));
			}

			if(ptr == &mons[PM_NASTYSTONE_SNIPER]) {
				(void) mongets(mtmp, SLING);
				(void) mongets(mtmp, rnd_class(RIGHT_MOUSE_BUTTON_STONE,NASTY_STONE));
				(void) mongets(mtmp, rnd_class(RIGHT_MOUSE_BUTTON_STONE,NASTY_STONE));
			}

			if(ptr == &mons[PM_EVIL_PATCH_IDEA_IMPLEMENTATOR]) {
				(void) mongets(mtmp, SLING);
				(void) mongets(mtmp, rnd_class(RIGHT_MOUSE_BUTTON_STONE,NASTY_STONE));
				(void) mongets(mtmp, rnd_class(RIGHT_MOUSE_BUTTON_STONE,NASTY_STONE));
				(void) mongets(mtmp, rnd_class(RIGHT_MOUSE_BUTTON_STONE,NASTY_STONE));
			}

		break;

	    case S_GNOME:

			if(ptr == &mons[PM_RADIOACTIVE_GNOLL]) (void) mongets(mtmp, POT_RADIUM);

		switch (mm) {
		    case PM_GNOLL:
			if(!rn2(3)) (void) mongets(mtmp, ORCISH_HELM);
			if(!rn2(3)) (void) mongets(mtmp, STUDDED_LEATHER_ARMOR);
			if(!rn2(3)) (void) mongets(mtmp, ORCISH_SHIELD);
			if(!rn2(4)) (void) mongets(mtmp, SPEAR);
			break;

		    case PM_GNOLL_WARRIOR:
			if(!rn2(2)) (void) mongets(mtmp, ORCISH_HELM);

			if (!rn2(20))
			    (void) mongets(mtmp, ORANGE_DRAGON_SCALE_MAIL);
			else if (rn2(3))
			    (void) mongets(mtmp, SCALE_MAIL);
			else
			    (void) mongets(mtmp, SPLINT_MAIL);

			if(!rn2(2)) (void) mongets(mtmp, ORCISH_SHIELD);
		 	if(!rn2(3)) (void) mongets(mtmp, KATANA);
			break;

		    case PM_GNOLL_CHIEFTAIN:
			(void) mongets(mtmp, ORCISH_HELM);

			if (!rn2(10))
			    (void) mongets(mtmp, BLUE_DRAGON_SCALE_MAIL);
			else
			    (void) mongets(mtmp, CRYSTAL_PLATE_MAIL);

			(void) mongets(mtmp, ORCISH_SHIELD);
			(void) mongets(mtmp, KATANA);
			(void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
			break;

		    case PM_GNOLL_SHAMAN:
			if (!rn2(10))
			    (void) mongets(mtmp, SILVER_DRAGON_SCALE_MAIL);
			else if (rn2(5))
			    (void) mongets(mtmp, CRYSTAL_PLATE_MAIL);
			else
			    (void) mongets(mtmp, RED_DRAGON_SCALE_MAIL);

			(void) mongets(mtmp, ATHAME);
			m_initthrow(mtmp, SHURIKEN, 40);
			(void) mongets(mtmp, rnd_offensive_item(mtmp));
			(void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
			break;

		    default:
			/*m_initweap_normal(mtmp);*/
			break;
		}	
		break;
	    case S_HUMANOID:

		if (ptr == &mons[PM_KLINGON_SOLDIER]) {

			  (void) mongets(mtmp, SUBMACHINE_GUN);
		  	  m_initthrow(mtmp, BULLET, 25);
		  	  m_initthrow(mtmp, BULLET, 25);
			  (void) mongets(mtmp, DAGGER);
			  (void) mongets(mtmp, FRAG_GRENADE);

		}

		if (ptr == &mons[PM_DREY_ELF]) { (void)mongets(mtmp, ELVEN_BOW); (void)mongets(mtmp, ELVEN_DAGGER);
			 m_initthrow(mtmp, ELVEN_ARROW, 25);
		}

		if (ptr == &mons[PM_DEATHWALKER_KNIGHT]) {

		     (void)mongets(mtmp, LONG_SWORD);
		     (void)mongets(mtmp, PLATE_MAIL);
		     (void)mongets(mtmp, LARGE_SHIELD);
		     (void)mongets(mtmp, HELMET);
		     (void)mongets(mtmp, LEATHER_GLOVES);
		}

		if (ptr == &mons[PM_DEATHWALKER_BATTLEMASTER]) {

		     (void)mongets(mtmp, ELECTRIC_SWORD);
		     (void)mongets(mtmp, DWARVISH_MITHRIL_COAT);
		     (void)mongets(mtmp, STEEL_SHIELD);
		     (void)mongets(mtmp, HELM_OF_STEEL);
		     (void)mongets(mtmp, GAUNTLETS_OF_STEEL);
		}

		if (ptr == &mons[PM_SKULK_BLADE]) {

		     (void)mongets(mtmp, SCIMITAR);
		     (void)mongets(mtmp, SMALL_SHIELD);
		     (void)mongets(mtmp, POT_HEALING);
		}

		if (ptr == &mons[PM_SOULLESS_CUTTER]) (void)mongets(mtmp, AXE);

		if (ptr == &mons[PM_HALFLING_SLINGER]) {(void)mongets(mtmp, SLING); m_initthrow(mtmp, ROCK, 15); }

		if (ptr == &mons[PM_GREAT_BAR_SLAYER]) (void)mongets(mtmp, PARTISAN);
		if (ptr == &mons[PM_RELIGIOUS_POTATO]) (void)mongets(mtmp, VOULGE);
		if (ptr == &mons[PM_FANATICIAN]) (void)mongets(mtmp, HALBERD);
		if (ptr == &mons[PM_BUTLER]) (void)mongets(mtmp, LUCERN_HAMMER);

		if (ptr == &mons[PM_FETISH] || ptr == &mons[PM_FLAYER] || ptr == &mons[PM_SOUL_KILLER] || ptr == &mons[PM_PYGMY_WARRIOR] || ptr == &mons[PM_STYGIAN_DOLL] || ptr == &mons[PM_VOODOO_FETISH] || ptr == &mons[PM_GIANT_PYGMY]) (void)mongets(mtmp, KNIFE);

		if (ptr == &mons[PM_FETISH_BLOWDART] || ptr == &mons[PM_FLAYER_BLOWDART] || ptr == &mons[PM_SOUL_KILLER_BLOWDART] || ptr == &mons[PM_PYGMY_WARRIOR_BLOWDART] || ptr == &mons[PM_STYGIAN_DOLL_BLOWDART] || ptr == &mons[PM_VOODOO_FETISH_BLOWDART] || ptr == &mons[PM_GIANT_PYGMY_BLOWDART]) m_initthrow(mtmp, DART, 25);

		if (ptr == &mons[PM_POLICEMAN]) {

				(void)mongets(mtmp, RUBBER_HOSE);
				m_initthrow(mtmp, CREAM_PIE, 2);
		}

		if (is_dwarf(ptr)) {
		    if (rn2(7)) (void)mongets(mtmp, DWARVISH_CLOAK);
		    if (rn2(7)) (void)mongets(mtmp, IRON_SHOES);
		    if (!rn2(4)) {
			(void)mongets(mtmp, DWARVISH_SHORT_SWORD);
			/* note: you can't use a mattock with a shield */
			if (rn2(2)) (void)mongets(mtmp, DWARVISH_MATTOCK);
			else {
				(void)mongets(mtmp, AXE);
				(void)mongets(mtmp, DWARVISH_ROUNDSHIELD);
			}
			(void)mongets(mtmp, DWARVISH_IRON_HELM);
			if (!rn2(4))
			    (void)mongets(mtmp, DWARVISH_MITHRIL_COAT);
		    } else if (rn2(2)) {
			(void)mongets(mtmp, !rn2(3) ? PICK_AXE : DAGGER);
		    }
		} else if(mm == PM_GITHYANKI_PIRATE){
			(void)mongets(mtmp, TWO_HANDED_SWORD);
			(void)mongets(mtmp, BRONZE_PLATE_MAIL);
			(void)mongets(mtmp, LEATHER_GLOVES);
			(void)mongets(mtmp, HIGH_BOOTS);
		} else if (is_hobbit(ptr)) {
			    switch (rn2(6)) {
			        case 0:
				    (void)mongets(mtmp, DAGGER);
				    break;
				case 1:
				    (void)mongets(mtmp, ELVEN_DAGGER);
				    break;
				case 2:
			            (void)mongets(mtmp, rn2(500) ? SLING : CATAPULT);
			            /* WAC give them some rocks to throw */
			            m_initthrow(mtmp, ROCK, rn2(200) ? 2 : 50); 
					if (!rn2(40)) m_initthrow(mtmp, ROCK, 20);
						break;
			        case 3:
			        case 4:
			        case 5:
					break;
			      }
				/* WAC add 50% chance of leather */
			    if (!rn2(10)) (void)mongets(mtmp, ELVEN_MITHRIL_COAT);
				else if (!rn2(2)) (void)mongets(mtmp, LEATHER_ARMOR);
				if (!rn2(10)) (void)mongets(mtmp, DWARVISH_CLOAK);
		} else switch(mm) {
          /* Mind flayers get robes */
          case PM_MIND_FLAYER:
            if (!rn2(2)) (void)mongets(mtmp, ROBE);

            break;
          case PM_GITHYANKI_PIRATE:
            (void)mongets(mtmp, SCIMITAR);

            break;
          case PM_ANUBITE:
            (void)mongets(mtmp, TRIDENT);
            (void)mongets(mtmp, RING_MAIL);

            break;
          case PM_MASTER_MIND_FLAYER:

            if (!rn2(10)) (void)mongets(mtmp, ROBE_OF_PROTECTION);
			else if (!rn2(10)) (void)mongets(mtmp, ROBE_OF_POWER);
			else (void)mongets(mtmp, ROBE);
            break;
		  case PM_GNOLL:
			if (!rn2(2)) switch (rn2(3)) {
				case 0: (void)mongets(mtmp, BARDICHE); break;
				case 1: (void)mongets(mtmp, VOULGE); break;
				case 2: (void)mongets(mtmp, HALBERD); break;
			}
			if (!rn2(2)) (void)mongets(mtmp, LEATHER_ARMOR);
			break;
          default:
	          break;                     
		}
		break;
	    case S_KOP:		/* create Keystone Kops with cream pies to
				 * throw. As suggested by KAA.	   [MRS]
				 */
		if (!rn2(4)) m_initthrow(mtmp, CREAM_PIE, 2);
		if (!rn2(3)) (void)mongets(mtmp,(rn2(2)) ? CLUB : RUBBER_HOSE);
		if (!rn2(500)) (void)mongets(mtmp, nastymusableitem() );

		break;
	    case S_ORC:

		if (ptr == &mons[PM_DEPRAVED_ORC]) (void) mongets(mtmp, WAN_CANCELLATION);

                /* All orcs will get at least an orcish dagger*/
		if(rn2(2)) (void)mongets(mtmp, ORCISH_HELM);
		switch (mm != PM_ORC_CAPTAIN ? mm :
			rn2(2) ? PM_MORDOR_ORC : PM_URUK_HAI) {
		    case PM_MORDOR_ORC:
			if(!rn2(3)) (void)mongets(mtmp, SCIMITAR);
                            else if (rn2(2)) (void)mongets(mtmp, ORCISH_DAGGER);
			if(!rn2(3)) (void)mongets(mtmp, ORCISH_SHIELD);
			if(!rn2(3)) (void)mongets(mtmp, KNIFE);
                        /* WAC add possible orcish spear */
                        if (!rn2(4)) m_initthrow(mtmp, ORCISH_SPEAR, 1);
			if(!rn2(3)) (void)mongets(mtmp, KNIFE);
			if(!rn2(3)) (void)mongets(mtmp, ORCISH_CHAIN_MAIL);
			break;
		    case PM_URUK_HAI:
			if(!rn2(3)) (void)mongets(mtmp, ORCISH_CLOAK);
			if(!rn2(3)) (void)mongets(mtmp, ORCISH_SHORT_SWORD);
                            else if (rn2(2)) (void)mongets(mtmp, ORCISH_DAGGER);
			if(!rn2(3)) (void)mongets(mtmp, IRON_SHOES);
			if(!rn2(3)) {
			    (void)mongets(mtmp, ORCISH_BOW);
			    m_initthrow(mtmp, ORCISH_ARROW, 25);
			}
			if(!rn2(3)) (void)mongets(mtmp, URUK_HAI_SHIELD);
			break;
                    case PM_GOBLIN:
                        if(!rn2(3)) (void)mongets(mtmp, ORCISH_SHORT_SWORD);
                            else if (rn2(2)) (void)mongets(mtmp, ORCISH_DAGGER);
			break;
                    case PM_GOBLIN_COP:
				(void)mongets(mtmp, RUBBER_HOSE);
				m_initthrow(mtmp, CREAM_PIE, 2);
			break;
                    case PM_SUPERCOP:
				(void)mongets(mtmp, RUBBER_HOSE);
				m_initthrow(mtmp, CREAM_PIE, 2);
			break;
                    case PM_HOBGOBLIN:
                        if(!rn2(3)) (void)mongets(mtmp, MORNING_STAR);
                            else if(!rn2(3)) (void)mongets(mtmp, ORCISH_SHORT_SWORD);
                            else if (rn2(2)) (void)mongets(mtmp, ORCISH_DAGGER);
			break;
		    default:
			if (mm != PM_ORC_SHAMAN && rn2(2)) 
/*                          (void)mongets(mtmp, (mm == PM_GOBLIN || rn2(2) == 0)*/
                          (void)mongets(mtmp, (rn2(2) == 0)
						   ? ORCISH_DAGGER : SCIMITAR);
		}
		break;
	    case S_OGRE:

		if(ptr == &mons[PM_WINE_VILLAGE_OGRE]) (void) mongets(mtmp, POT_CONFUSION);

		if(ptr == &mons[PM_HUGE_OGRE_THIEF]) { (void) mongets(mtmp, MIRROR); (void) mongets(mtmp, MIRROR); }

		if (!rn2(mm == PM_OGRE_KING ? 3 : mm == PM_OGRE_LORD ? 6 : 12))
		    (void) mongets(mtmp, BATTLE_AXE);
		else
		    (void) mongets(mtmp, CLUB);
		break;
	    case S_KOBOLD:

			if(ptr == &mons[PM_KOBOLD_ARCHER] || ptr == &mons[PM_ENRAGED_SHAMAN] || ptr == &mons[PM_DEMENTED_SHAMAN] || ptr == &mons[PM_DERANGED_SHAMAN] || ptr == &mons[PM_DEPRAVED_SHAMAN] || ptr == &mons[PM_TELEPORTED_SHAMAN]) { (void) mongets(mtmp, BOW); m_initthrow(mtmp, ARROW, 20);}

			if (ptr == &mons[PM_KOBOLD_GUNMAN]) {
				(void) mongets(mtmp, FLINTLOCK);
		  		m_initthrow(mtmp, BULLET, 20);
			}

			if(ptr == &mons[PM_KOBOLD_DIGGER]) (void) mongets(mtmp, WAN_DIGGING);
			if(ptr == &mons[PM_KOBOLD_ALCHEMIST]) (void) mongets(mtmp, POT_CYANIDE);
			if(ptr == &mons[PM_INSANE_KOBOLD]) (void) mongets(mtmp, WAN_STONING);
			if(ptr == &mons[PM_DISINTEGRATION_KOBOLD]) m_initthrow(mtmp, DART_OF_DISINTEGRATION, 20);
			if(ptr == &mons[PM_DRILLING_KOBOLD]) m_initthrow(mtmp, DART_OF_DISINTEGRATION, 30);
			if(ptr == &mons[PM_DISINTEGRATION_MASTER]) m_initthrow(mtmp, DART_OF_DISINTEGRATION, 50);

                /* WAC gets orcish 1:4, otherwise darts
                        (used to be darts 1:4)
                   gets orcish short sword 1:4, otherwise orcish dagger */
                if (!rn2(4)) m_initthrow(mtmp, ORCISH_SPEAR, 1);
                   else if (rn2(2)) m_initthrow(mtmp, rn2(500) ? DART : DART_OF_DISINTEGRATION, 20);
                if (!rn2(4)) mongets(mtmp, ORCISH_SHORT_SWORD);
                   else if (rn2(2)) mongets(mtmp, ORCISH_DAGGER);
		break;

	    case S_CENTAUR:

		if (rn2(2)) {
		    if(ptr == &mons[PM_FOREST_CENTAUR]) {
			(void)mongets(mtmp, BOW);
			m_initthrow(mtmp, ARROW, 25);
		    } else {
			(void)mongets(mtmp, CROSSBOW);
			m_initthrow(mtmp, CROSSBOW_BOLT, 25);
		    }
		}
		break;
	    case S_UMBER:

		if(ptr == &mons[PM_HEADLESS_BRUTE]) { (void) mongets(mtmp, AXE); (void) mongets(mtmp, SMALL_SHIELD); }
		if(ptr == &mons[PM_DECAPITATED_DEFILER]) { (void) mongets(mtmp, AXE); (void) mongets(mtmp, SMALL_SHIELD); }

		if(mm == PM_SHOTGUN_HORROR || mm == PM_SHOTGUN_TERROR) { (void)mongets(mtmp, CROSSBOW);
		m_initthrow(mtmp, CROSSBOW_BOLT, 25);	}
		break;
	    case S_WRAITH:
		(void)mongets(mtmp, KNIFE);
		(void)mongets(mtmp, LONG_SWORD);

		if(mm == PM_GRAVE_WARRIOR) { (void)mongets(mtmp, PICK_AXE);
			(void)mongets(mtmp, WAN_DIGGING);
		}
		break;
	    case S_ZOMBIE:

		if(ptr == &mons[PM_SKELETON_ARCHER] || ptr == &mons[PM_ARCHER] || ptr == &mons[PM_NECRO_ARCHER] || ptr == &mons[PM_CORPSE_ARCHER] || ptr == &mons[PM_HORROR_ARCHER] || ptr == &mons[PM_PELLET_ARCHER] || ptr == &mons[PM_ECM_ARCHER] || ptr == &mons[PM_SCORCHED_ARCHER] || ptr == &mons[PM_ELECTRO_ARCHER] || ptr == &mons[PM_BURNING_ARCHER] || ptr == &mons[PM_BONE_ARCHER] || ptr == &mons[PM_RETURNED_ARCHER] || ptr == &mons[PM_ICE_ARCHER] || ptr == &mons[PM_FLAME_ARCHER] || ptr == &mons[PM_POISONOUS_ARCHER] || ptr == &mons[PM_GATLING_ARCHER] || ptr == &mons[PM_CRYO_ARCHER] || ptr == &mons[PM_SCREAM_ARCHER] || ptr == &mons[PM_STATIC_ARCHER] ) { (void) mongets(mtmp, BOW); m_initthrow(mtmp, ARROW, 25);
		}

		if(ptr == &mons[PM_UNDEAD_TEMPLAR]) { (void) mongets(mtmp, LONG_SWORD); (void) mongets(mtmp, LARGE_SHIELD); }

		if(ptr == &mons[PM_PALE_ASIAN_LADY]) (void) mongets(mtmp, AXE);

		if(ptr == &mons[PM_PIT_ARCHER]) { (void) mongets(mtmp, BOW); m_initthrow(mtmp, ORCISH_ARROW, 25); }
		if(ptr == &mons[PM_DUNGEON_ARCHER]) { (void) mongets(mtmp, BOW); m_initthrow(mtmp, ELVEN_ARROW, 25); }
		if(ptr == &mons[PM_SCORCHING_ARCHER]) { (void) mongets(mtmp, BOW); m_initthrow(mtmp, DARK_ELVEN_ARROW, 25); }

		if(ptr == &mons[PM_FULL_REFUGE] || ptr == &mons[PM_CLEAVED_ZOMBIE]) (void) mongets(mtmp, AXE);

		if (mm == PM_ZOMBIE_PROSTITUTE){
			(void)mongets(mtmp, BULLWHIP);
			(void)mongets(mtmp, HIPPIE_HEELS);
		}

		if(ptr == &mons[PM_REFUGE_UHLERT]) (void) mongets(mtmp, BATTLE_AXE);

		if(ptr == &mons[PM_SICKLE_SKELETON] || ptr == &mons[PM_RAGE_SCYTHE] || ptr == &mons[PM_SHADOW_REAPER]) (void) mongets(mtmp, FAUCHARD);

		if(ptr == &mons[PM_POLICE_OFFICER]) (void) mongets(mtmp, SCR_FIRE);

		if(mm == PM_SKELETAL_PIRATE){
		    otmp = rn2(2) ? mksobj(SCIMITAR, FALSE, FALSE) : mksobj(KNIFE, FALSE, FALSE);
		    curse(otmp);
			otmp->oeroded = 1;
		    (void) mpickobj(mtmp, otmp, TRUE);
			
		    otmp = rn2(2) ? mksobj(HIGH_BOOTS, FALSE, FALSE) : mksobj(LEATHER_JACKET, FALSE, FALSE);
		    curse(otmp);
			otmp->oeroded2 = 1;
		    (void) mpickobj(mtmp, otmp, TRUE);
			break;
		}
		if (!rn2(4)) (void)mongets(mtmp, LEATHER_ARMOR);
		if (!rn2(4))
			(void)mongets(mtmp, (rn2(3) ? KNIFE : SHORT_SWORD));
		break;
	    case S_LIZARD:
		if (mm == PM_SALAMANDER)
			(void)mongets(mtmp, (rn2(7) ? SPEAR : rn2(3) ?
					     TRIDENT : STILETTO));
		break;
	    case S_TROLL:

		if(ptr == &mons[PM_OGRE_TROLL]) (void) mongets(mtmp, IRON_CHAIN);
		if(ptr == &mons[PM_WAR_TROLL]) (void) mongets(mtmp, IRON_CHAIN);
		if(ptr == &mons[PM_TROLL_GIANT]) (void) mongets(mtmp, ROTATING_CHAIN);
		if(ptr == &mons[PM_OLOG_HAI_BERSERKER]) (void) mongets(mtmp, SCR_ENRAGE);

		if(ptr == &mons[PM_ANNOYING_TROLL]) (void) mongets(mtmp, RANSEUR);
		if(ptr == &mons[PM_MEAN_TROLL]) (void) mongets(mtmp, PARTISAN);

		if (!rn2(2)) switch (rn2(4)) {
		    case 0: (void)mongets(mtmp, RANSEUR); break;
		    case 1: (void)mongets(mtmp, PARTISAN); break;
		    case 2: (void)mongets(mtmp, GLAIVE); break;
		    case 3: (void)mongets(mtmp, SPETUM); break;
		}
		break;
	    case S_DEMON:

			if(ptr == &mons[PM_BLOODTHIRSTER]) (void) mongets(mtmp, BATTLE_AXE);

			if(ptr == &mons[PM_CYBERDEMON]) {
			      (void)mongets(mtmp, ROCKET_LAUNCHER);
				m_initthrow(mtmp, ROCKET, 20);
			}

			if(ptr == &mons[PM_OREMORJ__THE_CYBERDEMON_LORD]) {
			      (void)mongets(mtmp, ROCKET_LAUNCHER);
			      (void)mongets(mtmp, ROCKET_LAUNCHER);
			      (void)mongets(mtmp, ROCKET_LAUNCHER);
				m_initthrow(mtmp, ROCKET, 20);
				m_initthrow(mtmp, ROCKET, 20);
				m_initthrow(mtmp, ROCKET, 20);
			}

			if (ptr == &mons[PM_AURORA_BOMBER]) {
				(void) mongets(mtmp, ROCKET_LAUNCHER);
			  	m_initthrow(mtmp, ROCKET, 3);
			}

			if (ptr == &mons[PM_STONE_DEVIL]) {
			  	(void) mongets(mtmp, BOULDER);
			  	(void) mongets(mtmp, BOULDER);
			  	(void) mongets(mtmp, BOULDER);
			  	(void) mongets(mtmp, BOULDER);

			}

			if (ptr == &mons[PM_STONE_DEMON]) { /* evil patch idea by jonadab */

				if (rn2(2)) (void) mongets(mtmp, TSURUGI);

				else {

					struct obj *otmpY = mksobj(CORPSE,TRUE,FALSE);

					if (otmpY) {

						otmpY->spe = 0;
						otmpY->quan = 1;
						otmpY->owt = 30;
						otmpY->corpsenm = PM_COCKATRICE;
						start_corpse_timeout(otmpY); /* gotta make sure they time out after a while! --Amy */
						(void) mpickobj(mtmp,otmpY, TRUE);

					}

				}

			}

		switch (mm) {
			case PM_DAMNED_PIRATE:
				otmp = mksobj(SCIMITAR, FALSE, FALSE);
				if (otmp) {
					curse(otmp);
					(void) mpickobj(mtmp, otmp, TRUE);
				}
				
				otmp = mksobj(LEATHER_ARMOR, FALSE, FALSE);
				if (otmp) {
					curse(otmp);
					otmp->oeroded = 1;
					(void) mpickobj(mtmp, otmp, TRUE);
				}
			break;
		    case PM_BALROG:
			(void)mongets(mtmp, BULLWHIP);
			(void)mongets(mtmp, BROADSWORD);
			break;
		    case PM_ORCUS:
			(void)mongets(mtmp, WAN_DEATH); /* the Wand of Orcus */
			break;
		    case PM_HORNED_DEVIL:
			(void)mongets(mtmp, rn2(4) ? TRIDENT : BULLWHIP);
			break;
		    case PM_DRUG_DEALER:
			(void)mongets(mtmp, POT_RADIUM);
			break;
		    case PM_DISPATER:
			(void)mongets(mtmp, WAN_STRIKING);
			break;
		    case PM_YEENOGHU:
			(void)mongets(mtmp, FLAIL);
			break;
		}
		/* prevent djinnis and mail daemons from leaving objects when
		 * they vanish
		 */
		if (!is_demon(ptr) && mm != PM_DAMNED_PIRATE) break;
		/* fall thru */
/*
 *	Now the general case, Some chance of getting some type
 *	of weapon for "normal" monsters.  Certain special types
 *	of monsters will get a bonus chance or different selections.
 */
	    default:
	      /*m_initweap_normal(mtmp);*/
	      break;
	}

	if (!rn2(3)) m_initweap_normal(mtmp);

/*    if ((int) mtmp->m_lev > rn2(120)) */        

	if (!rn2(ishaxor ? 20 : 40) ) (void) mongets(mtmp, rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1));
	if (!rn2(ishaxor ? 50 : 100) ) (void) mongets(mtmp, rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS));

      if ((int) mtmp->m_lev > rn2(500))
		(void) mongets(mtmp, rnd_offensive_item(mtmp));
	if (!rn2(1200)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));

}

#endif /* OVL2 */
#ifdef OVL1

#ifdef GOLDOBJ
/*
 *   Makes up money for monster's inventory.
 *   This will change with silver & copper coins
 */
void 
mkmonmoney(mtmp, amount)
struct monst *mtmp;
long amount;
{
    struct obj *gold = mksobj(GOLD_PIECE, FALSE, FALSE);
    if (gold) {
	    gold->quan = amount;
	    add_to_minv(mtmp, gold);
    }
}
#endif

STATIC_OVL void
m_initinv(mtmp)
register struct	monst	*mtmp;
{
	register int cnt;
	register struct obj *otmp;
	register struct permonst *ptr = mtmp->data;
/* 	char *opera_cloak = "opera cloak";*/
	int i;

#ifdef REINCARNATION
	if (Is_rogue_level(&u.uz) && rn2(2) ) return;
#endif

/*
 *	Soldiers get armour & rations - armour approximates their ac.
 *	Nymphs may get mirror or potion of object detection.
 */

	/* Sometimes, monsters might be generated with eggs. --Amy */

	if (!rn2(lays_eggs(mtmp->data) ? 500 : 1000) ) {
		struct obj *otmpB = mksobj(EGG,TRUE,FALSE);
		if (otmpB) {
			otmpB->spe = 0;
			if (!rn2(200)) otmpB->spe = 2;
			otmpB->quan = 1;
			while (!rn2(5)) otmpB->quan += 1;
			otmpB->owt = weight(otmpB);
			otmpB->corpsenm = egg_type_from_parent(mtmp->mnum, FALSE);
			attach_egg_hatch_timeout(otmpB);
			mpickobj(mtmp,otmpB, TRUE);
		}
	}

	if (!rn2(5000)) {
		struct obj *otmpC = mksobj(EGG,TRUE,FALSE);
		if (otmpC) {
			otmpC->spe = 0;
			if (!rn2(200)) otmpC->spe = 2;
			otmpC->quan = 1;
			while (!rn2(25)) otmpC->quan += 1;
			otmpC->owt = weight(otmpC);
			attach_egg_hatch_timeout(otmpC);
			mpickobj(mtmp,otmpC, TRUE);
		}
	}

	switch(ptr->mlet) {
	    case S_HUMAN:
		if (monsndx(ptr) == PM_FORD_PERFECT || monsndx(ptr) == PM_ARTHUR_DENT) {
			mongets(mtmp,PEANUT_BAG);
		} 
		if (monsndx(ptr) == PM_FORD_PERFECT) {
			mongets(mtmp,TOWEL);
		}
		if (monsndx(ptr) == PM_TOTAL_WOUNDING_KNIGHT) {
			mongets(mtmp,SCR_WOUNDS);
			if (!rn2(2)) mongets(mtmp,SCR_WOUNDS);
			if (!rn2(3)) mongets(mtmp,SCR_WOUNDS);
			if (!rn2(5)) mongets(mtmp,SCR_WOUNDS);
			if (!rn2(7)) mongets(mtmp,SCR_WOUNDS);
		}
		if (ptr == &mons[PM_MAUD_AGENT] || ptr == &mons[PM_DUAM_XNAHT_AGENT]) (void) mongets(mtmp, POT_AMNESIA);

		if (ptr == &mons[PM_DELUGE_AGENT] || ptr == &mons[PM_FLOOD_SURFER] || ptr == &mons[PM_WATER_GIRL] || ptr == &mons[PM_WATER_LADY]) (void) mongets(mtmp, SCR_FLOOD);
		if (ptr == &mons[PM_LAVA_LOVER] || ptr == &mons[PM_CRISPY_BURNER] || ptr == &mons[PM_FIRE_GIRL] || ptr == &mons[PM_FIERY_LADY]) (void) mongets(mtmp, SCR_LAVA);
		if (ptr == &mons[PM_TREE_HUGGER] || ptr == &mons[PM_ECOFRIENDLY_RESEARCHER] || ptr == &mons[PM_NATURE_GIRL] || ptr == &mons[PM_NATURE_LADY]) (void) mongets(mtmp, SCR_GROWTH);
		if (ptr == &mons[PM_NORTHERNER] || ptr == &mons[PM_COLD_PERSON] || ptr == &mons[PM_ICE_GIRL] || ptr == &mons[PM_ICE_LADY]) (void) mongets(mtmp, SCR_ICE);
		if (ptr == &mons[PM_FORMER_PRISONER] || ptr == &mons[PM_JOBLESS_WARDEN] || ptr == &mons[PM_FEMALE_GUARD] || ptr == &mons[PM_FEMALE_YEOMAN]) (void) mongets(mtmp, SCR_BARRHING);
		if (ptr == &mons[PM_PARANOID_SCHIZO] || ptr == &mons[PM_SUPERSTITIOUS_PERSON] || ptr == &mons[PM_MENTAL_GIRL] || ptr == &mons[PM_MENTAL_LADY]) (void) mongets(mtmp, SCR_LOCKOUT);
		if (ptr == &mons[PM_ACID_TRIPPER] || ptr == &mons[PM_DRUG_ABUSER] || ptr == &mons[PM_DRUGGED_GIRL] || ptr == &mons[PM_CRACKWHORE]) (void) mongets(mtmp, POT_HALLUCINATION);
		if (ptr == &mons[PM_SNOWBALL_THROWER] || ptr == &mons[PM_ICE_REGENT] || ptr == &mons[PM_SNOW_GIRL] || ptr == &mons[PM_SNOW_QUEEN]) (void) mongets(mtmp, POT_ICE);
		if (ptr == &mons[PM_FRIGHTENING_CREEP] || ptr == &mons[PM_TEETHLESS_CRONE] || ptr == &mons[PM_DEFORMED_GIRL] || ptr == &mons[PM_FEMALE_HULL]) (void) mongets(mtmp, POT_FEAR);
		if (ptr == &mons[PM_FIREBREATHER] || ptr == &mons[PM_FLAME_PUNISHER] || ptr == &mons[PM_DRAGON_GIRL] || ptr == &mons[PM_DRAGON_LADY]) (void) mongets(mtmp, POT_FIRE);
		if (ptr == &mons[PM_ELECTRO_SHOCKER] || ptr == &mons[PM_TAZER] || ptr == &mons[PM_SADISTIC_GIRL] || ptr == &mons[PM_SADISTIC_LADY]) (void) mongets(mtmp, POT_STUNNING);
		if (ptr == &mons[PM_SQUEEZER] || ptr == &mons[PM_JAMCLAMPER] || ptr == &mons[PM_BDSM_GIRL] || ptr == &mons[PM_BDSM_LADY]) (void) mongets(mtmp, POT_NUMBNESS);
		if (ptr == &mons[PM_FIENDISH_WARLOCK] || ptr == &mons[PM_MAGICAL_GIRL]) (void) mongets(mtmp, SCR_BAD_EFFECT);
		if (ptr == &mons[PM_OCCULT_WARLOCK] || ptr == &mons[PM_MAGICAL_LADY]) (void) mongets(mtmp, WAN_BAD_EFFECT);

		if (ptr == &mons[PM_GIRLY_GIRL]) (void) mongets(mtmp, SCR_GIRLINESS);
		if (ptr == &mons[PM_FEMALE_GIRL]) (void) mongets(mtmp, WAN_SUMMON_SEXY_GIRL);

		if (ptr == &mons[PM_DIVINE_SUMMONER]) (void) mongets(mtmp, SCR_SUMMON_ELM);
		if (ptr == &mons[PM_CELESTIAL_SUMMONER]) (void) mongets(mtmp, WAN_SUMMON_ELM);

		if (ptr == &mons[PM_ABALLINO]) (void) mongets(mtmp, WAN_TRAP_CREATION);
		if (ptr == &mons[PM_GENERAL_ZAROFF]) (void) mongets(mtmp, WAN_TRAP_CREATION);
		if (ptr == &mons[PM_JACK_THE_RIPPER]) (void) mongets(mtmp, WAN_TRAP_CREATION);
		if (ptr == &mons[PM_GENERAL_HEATHERSTONE]) (void) mongets(mtmp, WAN_TRAP_CREATION);

		if (ptr == &mons[PM_FOOTPAD]) (void) mongets(mtmp, SCR_TRAP_CREATION);
		if (ptr == &mons[PM_THIEF]) (void) mongets(mtmp, SCR_TRAP_CREATION);

		if (ptr == &mons[PM_STONER]) (void) mongets(mtmp, SCR_STONING);
		if (ptr == &mons[PM_DESTROYER]) (void) mongets(mtmp, SCR_DESTROY_ARMOR);
		if (ptr == &mons[PM_REAL_PUNISHER]) (void) mongets(mtmp, SCR_PUNISHMENT);
		if (ptr == &mons[PM_HEAVY_STONER]) (void) mongets(mtmp, WAN_STONING);
		if (ptr == &mons[PM_MASTER_PUNISHER]) (void) mongets(mtmp, WAN_PUNISHMENT);
		if (ptr == &mons[PM_MAUD_BADASS]) (void) mongets(mtmp, WAN_AMNESIA);

		if (ptr == &mons[PM_NOVICE_ARCHER] || ptr == &mons[PM_APPRENTICE_ARCHER]) {
			(void) mongets(mtmp, BOW);
			m_initthrow(mtmp, ARROW, 25);
		}

		if (ptr == &mons[PM_SHARPSHOOTER]) {
			(void) mongets(mtmp, BOW);
			m_initthrow(mtmp, ARROW, 25);
			m_initthrow(mtmp, ARROW, 25);
			m_initthrow(mtmp, ARROW, 25);
		}

		if (ptr == &mons[PM_SLINGSHOOTER]) {
			(void) mongets(mtmp, SLING);
			m_initthrow(mtmp, ROCK, 25);
			m_initthrow(mtmp, ROCK, 25);
		}

		if (monsndx(ptr) == PM_ENCLAVE_SOLDIER || monsndx(ptr) == PM_EVASIVE_ENCLAVE_SOLDIER) {
			mongets(mtmp, HAND_BLASTER);
			mongets(mtmp, FULL_PLATE_MAIL);
			m_initthrow(mtmp, rn2(20) ? BLASTER_BOLT : HEAVY_BLASTER_BOLT, 40);
		}

		if (monsndx(ptr) == PM_ENCLAVE_OFFICER || monsndx(ptr) == PM_EVASIVE_ENCLAVE_OFFICER) {
			mongets(mtmp, ARM_BLASTER);
			mongets(mtmp, LEATHER_ARMOR);
			m_initthrow(mtmp, rn2(20) ? BLASTER_BOLT : HEAVY_BLASTER_BOLT, 50);
		}

		if (monsndx(ptr) == PM_ENCLAVE_STORMTROOPER || monsndx(ptr) == PM_EVASIVE_ENCLAVE_STORMTROOPER) {
			mongets(mtmp, CUTTING_LASER);
			mongets(mtmp, FULL_PLATE_MAIL);
			m_initthrow(mtmp, LASER_BEAM, 50);
		}

		if (monsndx(ptr) == PM_ENCLAVE_ELITE_SOLDIER || monsndx(ptr) == PM_EVASIVE_ENCLAVE_ELITE_SOLDIER) {
			mongets(mtmp, RAYGUN);
			mongets(mtmp, FULL_PLATE_MAIL);
			m_initthrow(mtmp, LASER_BEAM, 50);
		}

		if (monsndx(ptr) == PM_ATOMIC_SOLDIER || monsndx(ptr) == PM_EVASIVE_ATOMIC_SOLDIER) {
			mongets(mtmp, BFG);
			mongets(mtmp, COMMANDER_SUIT);
			m_initthrow(mtmp, BFG_AMMO, 50);
			m_initthrow(mtmp, BFG_AMMO, 50);
			m_initthrow(mtmp, BFG_AMMO, 50);
			m_initthrow(mtmp, BFG_AMMO, 50);
			m_initthrow(mtmp, BFG_AMMO, 50);
			m_initthrow(mtmp, BFG_AMMO, 50);
			m_initthrow(mtmp, BFG_AMMO, 50);
			m_initthrow(mtmp, BFG_AMMO, 50);
		}

		if (ptr == &mons[PM_GUDRUN_S_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, WEDGE_SANDALS); }
		if (ptr == &mons[PM_ELLA_S_WEDGE_SANDAL]) { (void) mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL); (void) mongets(mtmp, WEDGE_SANDALS); }
		if (ptr == &mons[PM_MELISSA_S_WEDGE_HEELED_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }
		if (ptr == &mons[PM_MANUELA_S_WHORE_BOOT]) { (void) mongets(mtmp, SPIKED_BATTLE_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }

		if(is_mercenary(ptr)
#ifdef YEOMAN
				|| ptr == &mons[PM_CHIEF_YEOMAN_WARDER]
				|| ptr == &mons[PM_YEOMAN_WARDER]
#endif
		) {
		    register int mac;

		    switch(monsndx(ptr)) {
			case PM_GUARD: mac = -1; break;
#ifdef CONVICT
			case PM_PRISON_GUARD: mac = -2; break;
#endif /* CONVICT */
			case PM_SOLDIER: mac = 3; break;
			case PM_ILLUSIONARY_SOLDIER: mac = 3; break;
			case PM_SERGEANT: mac = 0; break;
			case PM_LIEUTENANT: mac = -2; break;
			case PM_CAPTAIN: mac = -3; break;
			case PM_GENERAL: mac = -3; break;
#ifdef YEOMAN
			case PM_YEOMAN_WARDER:
#endif
			case PM_WATCHMAN: mac = 3; break;
			case PM_ANGRY_WATCHMAN: mac = 3; break;
#ifdef YEOMAN
			case PM_CHIEF_YEOMAN_WARDER:
				mongets(mtmp, TALLOW_CANDLE);
#endif
			case PM_WATCH_CAPTAIN: mac = -2; break;
			case PM_WATCH_LIEUTENANT: mac = -2; break;
			case PM_WATCH_LEADER: mac = -2; break;
			case PM_ANGRY_WATCH_CAPTAIN: mac = -2; break;
			case PM_ANGRY_WATCH_LIEUTENANT: mac = -2; break;
			case PM_ANGRY_WATCH_LEADER: mac = -2; break;
			default: impossible("odd mercenary %d?", monsndx(ptr));
				mac = 0;
				break;
		    }

		    if (mac < -1 && rn2(5))
			mac += 7 + mongets(mtmp, (rn2(5)) ?
					   PLATE_MAIL : CRYSTAL_PLATE_MAIL);
		    else if (mac < 3 && rn2(5))
			mac += 6 + mongets(mtmp, (rn2(3)) ?
					   SPLINT_MAIL : BANDED_MAIL);
		    else if (rn2(5))
			mac += 3 + mongets(mtmp, (rn2(3)) ?
					   RING_MAIL : STUDDED_LEATHER_ARMOR);
		    else
			mac += 2 + mongets(mtmp, LEATHER_ARMOR);

		    if (mac < 10 && rn2(3))
			mac += 1 + mongets(mtmp, HELMET);
		    else if (mac < 10 && rn2(2))
			mac += 1 + mongets(mtmp, DENTED_POT);
		    if (mac < 10 && rn2(3))
			mac += 1 + mongets(mtmp, SMALL_SHIELD);
		    else if (mac < 10 && rn2(2))
			mac += 2 + mongets(mtmp, LARGE_SHIELD);
		    if (mac < 10 && rn2(3))
			mac += 1 + mongets(mtmp, LOW_BOOTS);
		    else if (mac < 10 && rn2(2))
			mac += 2 + mongets(mtmp, HIGH_BOOTS);
		    if (mac < 10 && rn2(3))
			mac += 1 + mongets(mtmp, LEATHER_GLOVES);
		    else if (mac < 10 && rn2(2))
			mac += 1 + mongets(mtmp, LEATHER_CLOAK);

		    if(ptr != &mons[PM_GUARD] &&
#ifdef CONVICT
			ptr != &mons[PM_PRISON_GUARD] &&
#endif /* CONVICT */
			ptr != &mons[PM_WATCHMAN] &&
			ptr != &mons[PM_ANGRY_WATCHMAN] &&
			ptr != &mons[PM_ANGRY_WATCH_LIEUTENANT] &&
			ptr != &mons[PM_ANGRY_WATCH_LEADER] &&
			ptr != &mons[PM_WATCH_LEADER] &&
			ptr != &mons[PM_ANGRY_WATCH_CAPTAIN] &&
			ptr != &mons[PM_WATCH_LIEUTENANT] &&
			ptr != &mons[PM_WATCH_CAPTAIN]) {
			if (!rn2(15)) (void) mongets(mtmp, K_RATION);
			if (!rn2(12)) (void) mongets(mtmp, C_RATION);
			if (ptr != &mons[PM_SOLDIER] && !rn2(3))
				(void) mongets(mtmp, BUGLE);
		    } else
			   if (ptr == &mons[PM_WATCHMAN] && rn2(3))
				(void) mongets(mtmp, TIN_WHISTLE);
		} else if (ptr == &mons[PM_TEACHER] && (!rn2(5)) ) {
			/* They're going to need it :-) */
			mongets(mtmp,rn2(3) ? RING_MAIL : PLATE_MAIL);
		} else if (ptr == &mons[PM_PRINCIPAL]) {
		    			/* They're going to need it :-) */
			mongets(mtmp,rn2(3) ? RING_MAIL : PLATE_MAIL);
		  
		}  else if (ptr == &mons[PM_SHOPKEEPER]) {
		    (void) mongets(mtmp,SKELETON_KEY);
		    /* STEPHEN WHITE'S NEW CODE                
		     *
		     * "Were here to pump *clap* YOU up!"  -Hans and Frans
		     *                                      Saterday Night Live
		     */
#ifndef FIREARMS
		    switch (rn2(4)) {
		    /* MAJOR fall through ... */
		    case 0: (void) mongets(mtmp, WAN_MAGIC_MISSILE);
		    case 1: (void) mongets(mtmp, POT_EXTRA_HEALING);
		    case 2: (void) mongets(mtmp, POT_HEALING);
		    case 3: (void) mongets(mtmp, WAN_STRIKING);
		    }
#endif
		    switch (rnd(4)) {
			/* MAJOR fall through ... */
			case 1: (void) mongets(mtmp,POT_HEALING);
			case 2: (void) mongets(mtmp,POT_EXTRA_HEALING);
			case 3: (void) mongets(mtmp,SCR_TELEPORTATION);
			case 4: (void) mongets(mtmp,WAN_TELEPORTATION);
			default:
				break;
		    }
		} else if (ptr->msound == MS_PRIEST || mtmp->data == &mons[PM_UNALIGNED_PRIEST] ||
			quest_mon_represents_role(ptr,PM_PRIEST)) {

		    /* evil patch idea by jonadab: 5% chance for aligned priests to be generated invisible */

		    (void) mongets(mtmp,
			    !rn2(20) ? CLOAK_OF_INVISIBILITY : rn2(7) ? rn1(ROBE_OF_WEAKNESS - ROBE + 1, ROBE) :
					     rn2(3) ? CLOAK_OF_PROTECTION :
						 CLOAK_OF_MAGIC_RESISTANCE);
		    (void) mongets(mtmp, SMALL_SHIELD);
#ifndef GOLDOBJ
		    mtmp->mgold = (long)rn1(10,20);
#else
		    mkmonmoney(mtmp,(long)rn1(10,20));
#endif
		} else if (quest_mon_represents_role(ptr,PM_MONK)) {
		    (void) mongets(mtmp, rn2(11) ? ROBE :
					     CLOAK_OF_MAGIC_RESISTANCE);
		}
		if (mtmp->data == &mons[PM_WIZARD_OF_YENDOR]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (ishaxor && mtmp->data == &mons[PM_WIZARD_OF_YENDOR]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);

		break;

	    case S_PIERCER:

		if (mtmp->data == &mons[PM_JIGEN_DAISUKE]) { (void) mongets(mtmp, ROCKET_LAUNCHER); (void) mongets(mtmp, ROCKET_LAUNCHER); m_initthrow(mtmp, ROCKET, 10); m_initthrow(mtmp, ROCKET, 10); m_initthrow(mtmp, ROCKET, 10); }
		if (mtmp->data == &mons[PM_JUEN_S_LEATHER_PEEP_TOE]) (void) mongets(mtmp, LEATHER_PEEP_TOES);

		break;

	    case S_GOLEM:
		if (mtmp->data == &mons[PM_ROBO_KY]) { (void) mongets(mtmp, ROCKET_LAUNCHER); m_initthrow(mtmp, ROCKET, 10); }
		if (mtmp->data == &mons[PM_TGWTG]) { (void) mongets(mtmp, SUBMACHINE_GUN); m_initthrow(mtmp, BULLET, 50); m_initthrow(mtmp, BULLET, 50); }

		break;

	    case S_FELINE:
		if (mtmp->data == &mons[PM_CUTCAT]) {
			 (void) mongets(mtmp, CUTTING_LASER);
			 m_initthrow(mtmp, LASER_BEAM, 50);
		}

		break;

	    case S_IMP:

		if (mtmp->data == &mons[PM_LEGION_DEVIL_BUZZER]) {
			 (void) mongets(mtmp, HAND_BLASTER);
			m_initthrow(mtmp, rn2(20) ? BLASTER_BOLT : HEAVY_BLASTER_BOLT, 40);
		}
		if (mtmp->data == &mons[PM_LEGION_DEVIL_ZAPPER]) {
			 (void) mongets(mtmp, ARM_BLASTER);
			m_initthrow(mtmp, rn2(20) ? BLASTER_BOLT : HEAVY_BLASTER_BOLT, 50);
		}
		if (mtmp->data == &mons[PM_LEGION_DEVIL_CUTTER]) {
			 (void) mongets(mtmp, CUTTING_LASER);
			m_initthrow(mtmp, LASER_BEAM, 50);
		}
		if (mtmp->data == &mons[PM_LEGION_DEVIL_RAYMAN]) {
			 (void) mongets(mtmp, RAYGUN);
			m_initthrow(mtmp, LASER_BEAM, 50);
		}

		if (mtmp->data == &mons[PM_ARROGANCE]) (void) mongets(mtmp, WAN_TRAP_CREATION);
		if (mtmp->data == &mons[PM_METAL_EMBATTLED_GIGANTIC_ULTIMATE_MIKI_IMPALER]) { (void) mongets(mtmp, ROCKET_LAUNCHER); m_initthrow(mtmp, ROCKET, 7);}
		if (mtmp->data == &mons[PM_MACROSS_VALKARIE]) { (void) mongets(mtmp, ROCKET_LAUNCHER); m_initthrow(mtmp, ROCKET, 10);}
		if (mtmp->data == &mons[PM_THE_WING_ZERO]) { (void) mongets(mtmp, ROCKET_LAUNCHER); m_initthrow(mtmp, ROCKET, 20);}

		if (mtmp->data == &mons[PM_ARCH_VILE]) (void) mongets(mtmp, WAN_CREATE_HORDE);
		if (mtmp->data == &mons[PM_ANJA_S_SNEAKER]) (void) mongets(mtmp, SOFT_SNEAKERS);
		if (mtmp->data == &mons[PM_EVA_S_MOCASSIN]) { (void) mongets(mtmp, SWEET_MOCASSINS); (void) mongets(mtmp, SCR_TRAP_CREATION); }
		if (mtmp->data == &mons[PM_KLARA_S_HIGH_HEELED_SANDAL]) { (void) mongets(mtmp, HIGH_HEELED_SANDAL); (void) mongets(mtmp, WEDGE_SANDALS); (void) mongets(mtmp, SCR_TRAP_CREATION); (void) mongets(mtmp, POT_SPEED); }
		if (mtmp->data == &mons[PM_LARISSA_S_GIRL_BOOT]) (void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);
		if (mtmp->data == &mons[PM_SANDRA_S_HIGH_HEELED_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, COMBAT_STILETTOS); (void) mongets(mtmp, SCR_TRAP_CREATION); }
		if (mtmp->data == &mons[PM_NADJA_S_EROTIC_LADY_SHOE]) { (void) mongets(mtmp, LACQUERED_DANCING_SHOE); (void) mongets(mtmp, FEMININE_PUMPS); (void) mongets(mtmp, SCR_TRAP_CREATION); }

		break;

	    case S_BAT:

		if (mtmp->data == &mons[PM_ARCEUS]) (void) mongets(mtmp, WAN_CHROMATIC_BEAM);
		if (mtmp->data == &mons[PM_KARIN_S_FLAT_SANDAL]) (void) mongets(mtmp, WEDGE_SANDALS);
		if (mtmp->data == &mons[PM_JASIEEN_S_WEDGE_SANDAL]) { (void) mongets(mtmp, WEDGE_SANDALS); (void) mongets(mtmp, SCR_TRAP_CREATION); (void) mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL); }
		if (mtmp->data == &mons[PM_RITA_S_SWEET_STILETTO]) { (void) mongets(mtmp, FEMININE_PUMPS); (void) mongets(mtmp, SEXY_LEATHER_PUMP); }
		if (mtmp->data == &mons[PM_PATRICIA_S_STEEL_CAPPED_SANDAL]) { (void) mongets(mtmp, HIGH_HEELED_SANDAL); }
		if (mtmp->data == &mons[PM_KERSTIN_S_LOVELY_SNEAKER]) { (void) mongets(mtmp, SOFT_SNEAKERS); (void) mongets(mtmp, SOFT_GIRL_SNEAKER); }
		if (mtmp->data == &mons[PM_MELTEM_S_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }
		if (mtmp->data == &mons[PM_KERSTIN_S_LOVELY_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }
		if (mtmp->data == &mons[PM_KARIN_S_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }
		if (mtmp->data == &mons[PM_KERSTIN_S_WOODEN_SANDAL]) { (void) mongets(mtmp, WEDGE_SANDALS); (void) mongets(mtmp, WOODEN_GETA); }

		break;

	    case S_LIZARD:

		if (mtmp->data == &mons[PM_ALMUTH_S_CUDDLY_SOFT_SNEAKER]) { (void) mongets(mtmp, SOFT_SNEAKERS); (void) mongets(mtmp, SOFT_GIRL_SNEAKER); }
		if (mtmp->data == &mons[PM_YVONNE_S_SOFT_SNEAKER]) { (void) mongets(mtmp, SOFT_SNEAKERS); (void) mongets(mtmp, SOFT_GIRL_SNEAKER); }
		if (mtmp->data == &mons[PM_JENNIFER_S_LOVELY_SNEAKER]) { (void) mongets(mtmp, SOFT_SNEAKERS); (void) mongets(mtmp, SOFT_GIRL_SNEAKER); }
		if (mtmp->data == &mons[PM_JENNIFER_S_SOFT_COMBAT_BOOT]) { (void) mongets(mtmp, HIPPIE_HEELS); (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); }
		if (mtmp->data == &mons[PM_MIRIAM_S_CONE_HEEL_BOOT]) (void) mongets(mtmp, COMBAT_STILETTOS);

		break;

	    case S_COCKATRICE:

		if (mtmp->data == &mons[PM_MADELEINE_S_PLATEAU_BOOT]) (void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);
		if (mtmp->data == &mons[PM_MADELEINE_S_CALF_LEATHER_SANDAL]) (void) mongets(mtmp, WEDGE_SANDALS);

		break;

	    case S_RUBMONST:

		if (mtmp->data == &mons[PM_JULCHEN_S_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }
		if (mtmp->data == &mons[PM_JOHANETTA_S_POINTY_GIRL_SHOE]) (void) mongets(mtmp, DANCING_SHOES);
		if (mtmp->data == &mons[PM_THE_BLACK_LADY]) (void) mongets(mtmp, SCR_TRAP_CREATION);
		break;

	    case S_BAD_COINS:

		if (mtmp->data == &mons[PM_SABRINA_S_BLOCK_HEELED_SANDAL]) { (void) mongets(mtmp, HIGH_HEELED_SANDAL); (void) mongets(mtmp, WEDGE_SANDALS); }
		if (mtmp->data == &mons[PM_SUNALI_S_HIKING_BOOT]) (void) mongets(mtmp, HUGGING_BOOT);
		if (mtmp->data == &mons[PM_DORA_S_STINKING_LEATHER_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }
		if (mtmp->data == &mons[PM_CARINA_S_PROFILED_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }
		if (mtmp->data == &mons[PM_VERENA_S_WORN_OUT_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }
		if (mtmp->data == &mons[PM_VERENA_S_STILETTO_SANDAL]) { (void) mongets(mtmp, HIGH_HEELED_SANDAL); (void) mongets(mtmp, COMBAT_STILETTOS); }
		if (mtmp->data == &mons[PM_DORA_S_TENDER_STILETTO_SANDAL]) { (void) mongets(mtmp, HIGH_HEELED_SANDAL); (void) mongets(mtmp, COMBAT_STILETTOS); }
		if (mtmp->data == &mons[PM_NADINE_S_HIGH_HEELED_SANDAL]) { (void) mongets(mtmp, HIGH_HEELED_SANDAL); (void) mongets(mtmp, WEDGE_SANDALS); }

		break;

	    case S_MIMIC:

		if (mtmp->data == &mons[PM_SIMONE_S_THICK_HUGGING_BOOT]) (void) mongets(mtmp, HUGGING_BOOT);

		if (mtmp->data == &mons[PM_SONJA_S_TORN_SNEAKER]) (void) mongets(mtmp, SOFT_SNEAKERS);

		if (mtmp->data == &mons[PM_POTION_MIMIC]) (void) mongets(mtmp, POT_FULL_HEALING);
		if (mtmp->data == &mons[PM_SCROLL_MIMIC]) (void) mongets(mtmp, SCR_TELEPORTATION);
		if (mtmp->data == &mons[PM_RING_MIMIC]) (void) mongets(mtmp, RIN_TIMELY_BACKUP);
		if (mtmp->data == &mons[PM_POTION_PERMAMIMIC]) (void) mongets(mtmp, POT_FULL_HEALING);
		if (mtmp->data == &mons[PM_SCROLL_PERMAMIMIC]) (void) mongets(mtmp, SCR_TELEPORTATION);
		if (mtmp->data == &mons[PM_RING_PERMAMIMIC]) (void) mongets(mtmp, RIN_TIMELY_BACKUP);
		if (mtmp->data == &mons[PM_CLOAKER]) (void) mongets(mtmp, ELVEN_CLOAK);
		if (mtmp->data == &mons[PM_PERMACLOAKER]) (void) mongets(mtmp, ELVEN_CLOAK);

		/* jonadab wants these things to spawn mumak; I'll just let them spawn random stuff with a scroll. --Amy */
		if (mtmp->data == &mons[PM_LARGE_SPAWN_MIMIC]) (void) mongets(mtmp, SCR_CREATE_MONSTER);
		if (mtmp->data == &mons[PM_GIANT_SPAWN_MIMIC]) (void) mongets(mtmp, SCR_CREATE_MONSTER);
		if (mtmp->data == &mons[PM_LARGE_SPAWN_PERMAMIMIC]) (void) mongets(mtmp, SCR_CREATE_MONSTER);
		if (mtmp->data == &mons[PM_GIANT_SPAWN_PERMAMIMIC]) (void) mongets(mtmp, SCR_CREATE_MONSTER);

		break;

	    case S_WORM:

		if (mtmp->data == &mons[PM_SAND_MAGGOT]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 1;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_SAND_MAGGOT, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_FOREST_MAGGOT]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 1;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_FOREST_MAGGOT, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_DARK_MAGGOT]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 1;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_DARK_MAGGOT, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_UNDEAD_WORM]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 1;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_UNDEAD_WORM, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_ROCK_WORM]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 2;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_ROCK_WORM, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_STEEL_MAGGOT]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 2;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_STEEL_MAGGOT, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_DEVOURER]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 3;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_DEVOURER, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_GIANT_ERROR]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 3;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(rndmonnum(), FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_PETTY_GIANT_ERROR]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 3;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(rndmonnum(), FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_GIANT_LAMPREY]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 4;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_GIANT_LAMPREY, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_CAVE_WORM]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 4;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_CAVE_WORM, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_CAVE_ERROR]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 4;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_CAVE_ERROR, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_WORLD_KILLER]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 5;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_WORLD_KILLER, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_SPEEDSTER_MAGGOT]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 7;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_SPEEDSTER_MAGGOT, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_NYMPHOMANIAC]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 7;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_NYMPHOMANIAC, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_BIRD_SPAWNER]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 3;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_ASSERTION_FAILURE, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}
		if (mtmp->data == &mons[PM_GLOAM_SPAWNER]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 3;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_GREENER_SPIRIT, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

		if (mtmp->data == &mons[PM_BREEDER]) (void) mongets(mtmp, EGG);

		if (mtmp->data == &mons[PM_YASAMAN_S_LEATHER_BOOT]) (void) mongets(mtmp, HIPPIE_HEELS);

		if (mtmp->data == &mons[PM_FLESH_SPAWNER] || mtmp->data == &mons[PM_GIANT_DOG]) { (void) mongets(mtmp, EGG); (void) mongets(mtmp, EGG); (void) mongets(mtmp, EGG);
		}
		if (mtmp->data == &mons[PM_STYGIAN_HAG]) { (void) mongets(mtmp, EGG); (void) mongets(mtmp, EGG); (void) mongets(mtmp, EGG); (void) mongets(mtmp, EGG);
		}
		if (mtmp->data == &mons[PM_GROTESQUE]) { (void) mongets(mtmp, EGG); (void) mongets(mtmp, EGG); (void) mongets(mtmp, EGG); (void) mongets(mtmp, EGG); (void) mongets(mtmp, EGG);
		}

		if (mtmp->data == &mons[PM_CLOCK_WOLF]) { (void) mongets(mtmp, EGG); (void) mongets(mtmp, EGG); (void) mongets(mtmp, EGG); (void) mongets(mtmp, EGG); (void) mongets(mtmp, EGG); (void) mongets(mtmp, EGG); (void) mongets(mtmp, EGG);
		}

		if (mtmp->data == &mons[PM_SALLY_S_VELCRO_SNEAKER]) (void) mongets(mtmp, SOFT_SNEAKERS);

		break;

	    case S_BAD_FOOD:

		if (mtmp->data == &mons[PM_HARROWBARK_WALKER]) m_initthrow(mtmp, JAVELIN, 5);
		if (mtmp->data == &mons[PM_HARROWBARK_WANDLE]) m_initthrow(mtmp, SPEAR, 5);

		if (mtmp->data == &mons[PM_GUARDIAN_O__THE_FOREST]) m_initthrow(mtmp, SPIRIT_THROWER, 5);
		if (mtmp->data == &mons[PM_BASTARD_OF_THE_FOREST]) m_initthrow(mtmp, TORPEDO, 5);

		if (mtmp->data == &mons[PM_HARROWBARK_FIEND]) m_initthrow(mtmp, SPIRIT_THROWER, 5);
		if (mtmp->data == &mons[PM_HARROWBARK_BOO]) m_initthrow(mtmp, ELVEN_SPEAR, 5);

		if (mtmp->data == &mons[PM_QUIVER_SLOT]) { (void) mongets(mtmp, BOW); m_initthrow(mtmp, ARROW, 25);}

		if (mtmp->data == &mons[PM_MACHIEVALLIAN_MOSS]) (void) mongets(mtmp, SCR_TRAP_CREATION);

		if (mtmp->data == &mons[PM_ROSY_S_LEATHER_PUMP]) { (void) mongets(mtmp, FEMININE_PUMPS); (void) mongets(mtmp, SEXY_LEATHER_PUMP); }

		break;

	    case S_NAGA:
		if (mtmp->data == &mons[PM_TU_TARA_SHIELDSNAPPER]) { (void) mongets(mtmp, BOW); m_initthrow(mtmp, ARROW, 25);}
		if (mtmp->data == &mons[PM_TU_TARA_EDGECUTTER]) (void) mongets(mtmp, BEC_DE_CORBIN);
		if (mtmp->data == &mons[PM_TU_TARA_WARMONGER]) (void) mongets(mtmp, KATANA);

		if (mtmp->data == &mons[PM_SNAKE_WOMAN]) (void) mongets(mtmp, SCALPEL);

		if (mtmp->data == &mons[PM_HYDRA_SNAKE]) {
		  	(void) mongets(mtmp, HYDRA_BOW);
		  	m_initthrow(mtmp, GOLDEN_ARROW, 25);
		}

		break;

	    case S_LIGHT:
		if (mtmp->data == &mons[PM_FIERY_EFFULGENCE]) m_initthrow(mtmp, FRAG_GRENADE, 5);

		if (mtmp->data == &mons[PM_DENISE_S_GIRL_SHOE]) (void) mongets(mtmp, SOFT_SNEAKERS);

		break;
	    case S_GNOME:

		if (mtmp->data == &mons[PM_GNOME_BADASS]) (void) mongets(mtmp, WAN_REMOVE_RESISTANCE);
		if (mtmp->data == &mons[PM_GNOMISH_SLIME_MAGE]) (void) mongets(mtmp, POT_SLIME);
		if (mtmp->data == &mons[PM_NASTY_GRIFFIN]) (void) mongets(mtmp, SCR_MEGALOAD);
		if (mtmp->data == &mons[PM_GNOME_ROGUE_LEADER]) (void) mongets(mtmp, SCR_TRAP_CREATION);
		if (mtmp->data == &mons[PM_NAUTILUS_STEERSMAN]) (void) mongets(mtmp, SCR_TRAP_CREATION);
		if (mtmp->data == &mons[PM_NAUTILUS_NEER_DO_WELL]) (void) mongets(mtmp, SCR_TRAP_CREATION);

		if (mtmp->data == &mons[PM_GNOMISH_GUNMAN]) {
		  	(void) mongets(mtmp, FLINTLOCK);
		  	m_initthrow(mtmp, BULLET, 20);
		}

		if (!rn2(5)) { (void)mongets(mtmp, GNOMISH_HELM); }
		if (!rn2(10)) { (void)mongets(mtmp, GNOMISH_BOOTS); }
		if (!rn2(15)) { (void)mongets(mtmp, GNOMISH_SUIT); }
		break;
	    case S_NYMPH:
	      if(mtmp->data == &mons[PM_SATYR]){
	        if(!rn2(2)) (void) mongets(mtmp, POT_BOOZE);
	        (void) mongets(mtmp, WOODEN_FLUTE);
		}

		if (ptr == &mons[PM_GUN_CHICK]) {
			(void) mongets(mtmp, FLINTLOCK);
	  		m_initthrow(mtmp, BULLET, 20);
		}
		if (ptr == &mons[PM_SUBMACHINE_NYMPH]) {
			(void) mongets(mtmp, SUBMACHINE_GUN);
	  		m_initthrow(mtmp, BULLET, 50);
		}
		if (ptr == &mons[PM_SEXY_HEXY]) {
			(void) mongets(mtmp, CUTTING_LASER);
	  		m_initthrow(mtmp, LASER_BEAM, 50);
		}
		if (ptr == &mons[PM_SNIPER_BUNDLE_HEAD]) {
			(void) mongets(mtmp, SNIPER_RIFLE);
	  		m_initthrow(mtmp, BULLET, 50);
		}
		if (ptr == &mons[PM_TEASING_NYMPH]) {
	  		m_initthrow(mtmp, SPIKE, 50);
		}

		if (mtmp->data == &mons[PM_NATALIYA_S_WEDGE_SANDAL]) { (void) mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL); (void) mongets(mtmp, WEDGE_SANDALS); }

		if (mtmp->data == &mons[PM_APHRODITE]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_FILTHY_GUTTERSNIPE]) (void) mongets(mtmp, WAN_BAD_LUCK);
		if (mtmp->data == &mons[PM_INCONTINENT_NYMPH]) (void) mongets(mtmp, POT_URINE);
		if (mtmp->data == &mons[PM_FREAKING_DRYAD]) (void) mongets(mtmp, SCR_GROWTH);
		if (mtmp->data == &mons[PM_DIGGING_NYMPH]) (void) mongets(mtmp, WAN_DIGGING);
		if (mtmp->data == &mons[PM_PHASER_GIANT_OREAD]) (void) mongets(mtmp, BOULDER);
		if (mtmp->data == &mons[PM_OBNOXIOUS_NAIAD]) (void) mongets(mtmp, SCR_FLOOD);
		if (ishaxor && mtmp->data == &mons[PM_APHRODITE]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if(!rn2(2)) (void) mongets(mtmp, MIRROR);
		if(!rn2(/*2*/10)) (void) mongets(mtmp, POT_OBJECT_DETECTION);

		/* evil patch idea by jonadab: levelwarping nymphs. To avoid annoying segfaults I'll have them do it via item. --Amy */
		if(!rn2(200)) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (ptr == &mons[PM_NYMPH_QUEEN]) {
			struct obj *otmp = mksobj(SKELETON_KEY,TRUE,FALSE);
			if (otmp) {
				otmp = oname(otmp,artiname(ART_KEY_OF_ACCESS));
				mpickobj(mtmp,otmp, TRUE);
			}
		}
		break;

	    case S_QUADRUPED:

		if (ptr == &mons[PM_RAY_BEAR]) {
			(void) mongets(mtmp, RAYGUN);
	  		m_initthrow(mtmp, LASER_BEAM, 50);
		}

		if (mtmp->data == &mons[PM_PURE_CHAOS_HORDE]) (void) mongets(mtmp, SCR_CHAOS_TERRAIN);

		if (mtmp->data == &mons[PM_SOPHIE_S_COMBAT_BOOT]) (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		if (mtmp->data == &mons[PM_BEA_S_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }
		if (mtmp->data == &mons[PM_THAI_S_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }
		if (mtmp->data == &mons[PM_CORINA_S_STILETTO_BOOT]) (void) mongets(mtmp, COMBAT_STILETTOS);

			if(ptr == &mons[PM_EXIT]) (void) mongets(mtmp, !rn2(3) ? SCR_WARPING : !rn2(2) ? SCR_ROOT_PASSWORD_DETECTION : SCR_TELEPORTATION);

		if (mtmp->data == &mons[PM_SIEGE_BEAST] || mtmp->data == &mons[PM_TANKED_ELEPHANT]) {
			(void) makemon(&mons[PM_IMP], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			(void) makemon(&mons[PM_IMP], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			if (!rn2(3)) (void) makemon(&mons[PM_IMP], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			if (!rn2(5)) (void) makemon(&mons[PM_IMP], mtmp->mx, mtmp->my, MM_ADJACENTOK);
		}

		if (mtmp->data == &mons[PM_CRUSH_BEAST] || mtmp->data == &mons[PM_DIFFICULT_ELEPHANT]) {
			(void) makemon(&mons[PM_KOBOLD], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			(void) makemon(&mons[PM_KOBOLD], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			if (!rn2(3)) (void) makemon(&mons[PM_KOBOLD], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			if (!rn2(5)) (void) makemon(&mons[PM_KOBOLD], mtmp->mx, mtmp->my, MM_ADJACENTOK);
		}

		if (mtmp->data == &mons[PM_BLOOD_BRINGER]) {
			(void) makemon(&mons[PM_GOBLIN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			(void) makemon(&mons[PM_GOBLIN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			if (!rn2(3)) (void) makemon(&mons[PM_GOBLIN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			if (!rn2(5)) (void) makemon(&mons[PM_GOBLIN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
		}

		if (mtmp->data == &mons[PM_GORE_BEARER] || mtmp->data == &mons[PM_GIANT_MILL_AUTO]) {
			(void) makemon(&mons[PM_DEMON_CHEATER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			(void) makemon(&mons[PM_DEMON_CHEATER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			if (!rn2(3)) (void) makemon(&mons[PM_DEMON_CHEATER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			if (!rn2(5)) (void) makemon(&mons[PM_DEMON_CHEATER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
		}

		if (mtmp->data == &mons[PM_DEMON_STEED]) {
			(void) makemon(&mons[PM_DEMON_SPRITE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			(void) makemon(&mons[PM_DEMON_SPRITE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			if (!rn2(3)) (void) makemon(&mons[PM_DEMON_SPRITE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			if (!rn2(5)) (void) makemon(&mons[PM_DEMON_SPRITE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
		}

		break;

	    case S_ANT:

		if (mtmp->data == &mons[PM_GREAT_PARALYSATOR_FLY]) (void) mongets(mtmp, WAN_PARALYSIS);
		if (mtmp->data == &mons[PM_TEAM_NASTYTRAP_MEMBER]) (void) mongets(mtmp, SCR_NASTINESS);

		if (mtmp->data == &mons[PM_JELLY_BEE]) (void) mongets(mtmp, LUMP_OF_ROYAL_JELLY);
		if (mtmp->data == &mons[PM_CROWNED_QUEEN_BEE]) (void) mongets(mtmp, LUMP_OF_ROYAL_JELLY);
		if (mtmp->data == &mons[PM_GIANT_JELLY_BEE]) (void) mongets(mtmp, LUMP_OF_ROYAL_JELLY);

		if (mtmp->data == &mons[PM_MARIKE_S_WORN_OUT_SNEAKER]) (void) mongets(mtmp, SOFT_SNEAKERS);
		if (mtmp->data == &mons[PM_JEANETTA_S_LITTLE_GIRL_BOOT]) (void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);

		break;

	    case S_CENTAUR:

		if (ptr == &mons[PM_HYDRA_CENTAUR]) {
		  	(void) mongets(mtmp, HYDRA_BOW);
		  	m_initthrow(mtmp, GOLDEN_ARROW, 25);
		}

		break;

	    case S_DOG:

		if (mtmp->data == &mons[PM_CRAPDOG]) (void) mongets(mtmp, SCR_BULLSHIT);
		if (mtmp->data == &mons[PM_ELEMENTAL_GIRL]) (void) mongets(mtmp, SOFT_SNEAKERS);
		if (mtmp->data == &mons[PM_SUE_LYN_S_THICK_WINTER_BOOT]) { (void) mongets(mtmp, HUGGING_BOOT); (void) mongets(mtmp, SCR_TRAP_CREATION); }

		if (mtmp->data == &mons[PM_BOW_WOLF]) {
		  	(void) mongets(mtmp, BOW);
		  	m_initthrow(mtmp, ANCIENT_ARROW, 25);
		}

		break;

	    case S_GHOST:

		if (mtmp->data == &mons[PM_CACKLING_SPIRIT]) (void) mongets(mtmp, WAN_TRAP_CREATION);
		if (mtmp->data == &mons[PM_HAUNTED_SPIRIT]) (void) mongets(mtmp, WAN_CURSE_ITEMS);
		if (mtmp->data == &mons[PM_SUMMONING_SPIRIT]) (void) mongets(mtmp, SCR_SUMMON_GHOST);
		if (mtmp->data == &mons[PM_HOMING_THUNDER_CALLER]) (void) mongets(mtmp, WAN_DRAIN_MANA);
		if (mtmp->data == &mons[PM_ELENA_S_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }
		if (mtmp->data == &mons[PM_KATHARINA_S_LOVELY_GIRL_BOOT]) { (void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS); (void) mongets(mtmp, LEATHER_PEEP_TOES); }

		break;

	    case S_FUNGUS:

		if (mtmp->data == &mons[PM_PLEASING_FUNGUS]) (void) mongets(mtmp, SCR_IMMOBILITY);
		if (mtmp->data == &mons[PM_PLEASING_WARP_FUNGUS]) (void) mongets(mtmp, WAN_IMMOBILITY);
		if (mtmp->data == &mons[PM_PLEASING_PATCH]) (void) mongets(mtmp, SCR_EGOISM);
		if (mtmp->data == &mons[PM_PLEASING_WARP_PATCH]) (void) mongets(mtmp, WAN_EGOISM);

		if (mtmp->data == &mons[PM_NATALJE_S_PLATEAU_SANDAL]) { (void) mongets(mtmp, HIGH_HEELED_SANDAL); (void) mongets(mtmp, SCR_TRAP_CREATION); (void) mongets(mtmp, WEDGE_SANDALS); }

		break;

	    case S_KOP:

		if (ptr == &mons[PM_NAZI_ROCKETEER]) {
			 (void) mongets(mtmp, ROCKET_LAUNCHER);
			 m_initthrow(mtmp, ROCKET, 25);
		}

		if (ptr == &mons[PM_ARMED_KOP_KOMMISSIONER]) {
			(void) mongets(mtmp, FLINTLOCK);
	  		m_initthrow(mtmp, BULLET, 20);
		}

		if (ptr == &mons[PM_ANTIMATTER_KOMMISSIONER]) {
			(void) mongets(mtmp, FLINTLOCK);
	  		m_initthrow(mtmp, ANTIMATTER_BULLET, 20);
		}

		if (ptr == &mons[PM_ANTIMATTER_KCHIEF]) {
			(void) mongets(mtmp, SNIPER_RIFLE);
	  		m_initthrow(mtmp, ANTIMATTER_BULLET, 50);
		}

		if (ptr == &mons[PM_ANTIMATTER_KATCHER]) {
			(void) mongets(mtmp, SUBMACHINE_GUN);
	  		m_initthrow(mtmp, ANTIMATTER_BULLET, 50);
		}

		if (ptr == &mons[PM_ANTIMATTER_KRIMINOLOGIST]) {
			(void) mongets(mtmp, ASSAULT_RIFLE);
	  		m_initthrow(mtmp, ANTIMATTER_BULLET, 50);
	  		m_initthrow(mtmp, ANTIMATTER_BULLET, 50);
		}

		if (ptr == &mons[PM_ARMED_KOP_KCHIEF]) {
			(void) mongets(mtmp, SHOTGUN);
	  		m_initthrow(mtmp, SHOTGUN_SHELL, 25);
		}

		if (ptr == &mons[PM_ARMED_KOP_KATCHER]) {
			(void) mongets(mtmp, HAND_BLASTER);
			m_initthrow(mtmp, rn2(20) ? BLASTER_BOLT : HEAVY_BLASTER_BOLT, 40);
		}

		if (ptr == &mons[PM_ARMED_KOP_KRIMINOLOGIST]) {
			(void) mongets(mtmp, RAYGUN);
			m_initthrow(mtmp, LASER_BEAM, 50);
		}

		if (ptr == &mons[PM_ROADBLOCK_KOP]) {
			m_initthrow(mtmp, CALTROP, 50);
		}

		if (ptr == &mons[PM_BARRIER_KOP]) {
			m_initthrow(mtmp, SPIKE, 50);
		}

		if (ptr == &mons[PM_TRAFFIC_KOP]) {
			m_initthrow(mtmp, NEEDLE, 50);
		}

		if (ptr == &mons[PM_ANTITANK_KOP]) (void) mongets(mtmp, nastymusableitem() );

		if (ptr == &mons[PM_NETHER_KOP]) (void) mongets(mtmp, WAN_NETHER_BEAM);
		if (ptr == &mons[PM_AURORA_KOP]) (void) mongets(mtmp, WAN_AURORA_BEAM);
		if (ptr == &mons[PM_INFERNAL_KOP]) (void) mongets(mtmp, WAN_INFERNO);
		if (ptr == &mons[PM_ICY_KOP]) (void) mongets(mtmp, WAN_ICE_BEAM);
		if (ptr == &mons[PM_THUNDEROUS_KOP]) (void) mongets(mtmp, WAN_THUNDER);
		if (ptr == &mons[PM_SPILL_KOP]) (void) mongets(mtmp, WAN_SLUDGE);
		if (ptr == &mons[PM_TOXIC_KOP]) (void) mongets(mtmp, WAN_TOXIC);
		if (ptr == &mons[PM_GAS_KOP]) (void) mongets(mtmp, WAN_CHLOROFORM);
		if (ptr == &mons[PM_GRAVITY_KOP]) (void) mongets(mtmp, WAN_GRAVITY_BEAM);
		if (ptr == &mons[PM_BUBBLING_KOP]) (void) mongets(mtmp, WAN_BUBBLEBEAM);
		if (ptr == &mons[PM_DREAMING_KOP]) (void) mongets(mtmp, WAN_DREAM_EATER);
		if (ptr == &mons[PM_NIGHTLY_KOP]) (void) mongets(mtmp, WAN_GOOD_NIGHT);
		if (ptr == &mons[PM_HYPER_KOP]) (void) mongets(mtmp, WAN_HYPER_BEAM);

		break;

	    case S_TRAPPER:

		if (ptr == &mons[PM_DEMOLITIONS_EXPERT]) {
			m_initthrow(mtmp, FRAG_GRENADE, 15);
			(void) mongets(mtmp, SCR_TRAP_CREATION);
		}

		if (mtmp->data == &mons[PM_FIRE_METROID]) (void) mongets(mtmp, SCR_FLOODING);
		if(ptr == &mons[PM_CARNIVOROUS_BAG]) (void) mongets(mtmp, BAG_OF_TRICKS);
		if(ptr == &mons[PM_TRAPPER_CART]) (void) mongets(mtmp, SCR_TRAP_CREATION);
		if (monsndx(ptr) == PM_JANE_S_HUGGING_BOOT) { (void) mongets(mtmp, HUGGING_BOOT); (void) mongets(mtmp, SCR_TELEPORTATION); }

		if (ptr == &mons[PM_LASER_METROID]) {
			(void) mongets(mtmp, HAND_BLASTER);
			m_initthrow(mtmp, rn2(20) ? BLASTER_BOLT : HEAVY_BLASTER_BOLT, 40);
		}

		if (ptr == &mons[PM_ZEBETITE_METROID]) {
			(void) mongets(mtmp, CUTTING_LASER);
			m_initthrow(mtmp, LASER_BEAM, 50);
		}

		if (ptr == &mons[PM_LIGHTSCREEN_METROID]) {
			(void) mongets(mtmp, RAYGUN);
			m_initthrow(mtmp, LASER_BEAM, 50);
		}

		if (ptr == &mons[PM_SUPREME_METROID]) {
			(void) mongets(mtmp, ARM_BLASTER);
			m_initthrow(mtmp, rn2(20) ? BLASTER_BOLT : HEAVY_BLASTER_BOLT, 50);
		}

		break;

	    case S_JABBERWOCK:

		if (mtmp->data == &mons[PM_FUMBLING_MONSTER]) (void) mongets(mtmp, WAN_FUMBLING);
		if (mtmp->data == &mons[PM_GLIB_MONSTER]) (void) mongets(mtmp, WAN_FINGER_BENDING);
		if (mtmp->data == &mons[PM_GREATER_STUN_MONSTER]) (void) mongets(mtmp, WAN_STUN_MONSTER);

		break;

	    case S_UMBER:

		if (mtmp->data == &mons[PM_GLUTTONOUS_HULK]) (void) mongets(mtmp, WAN_STARVATION);

		if (ptr == &mons[PM_CUTTING_HORROR] || ptr == &mons[PM_CUTTING_TERROR]) {
			(void) mongets(mtmp, CUTTING_LASER);
			m_initthrow(mtmp, LASER_BEAM, 50);
		}

		break;

	    case S_GREMLIN:

		if (mtmp->data == &mons[PM_GREMGARGOYLE]) (void) mongets(mtmp, POT_CANCELLATION);
		if (mtmp->data == &mons[PM_ANITA_S_SEXY_LEATHER_PUMP]) { (void) mongets(mtmp, SEXY_LEATHER_PUMP); (void) mongets(mtmp, FEMININE_PUMPS); }

		break;

	    case S_BLOB:

		if (ptr == &mons[PM_ONANOCTER__A]) {
			 (void) mongets(mtmp, PISTOL);
			 m_initthrow(mtmp, BULLET, 25);
		}

		if (ptr == &mons[PM_ONANOCTER__B]) {
			 (void) mongets(mtmp, DAGGER);
			 (void) mongets(mtmp, RIFLE);
			 m_initthrow(mtmp, BULLET, 25);
		}

		if (ptr == &mons[PM_ONANOCTER__C]) {
			 (void) mongets(mtmp, SHORT_SWORD);
			 (void) mongets(mtmp, SUBMACHINE_GUN);
			 m_initthrow(mtmp, BULLET, 50);
		}

		if (ptr == &mons[PM_ONANOCTER__D]) {
			 (void) mongets(mtmp, BROADSWORD);
			 (void) mongets(mtmp, SHOTGUN);
			 m_initthrow(mtmp, SHOTGUN_SHELL, 20);
		}

		if (ptr == &mons[PM_ONANOCTER__E]) {
			 (void) mongets(mtmp, JAGGED_STAR);
			 (void) mongets(mtmp, ASSAULT_RIFLE);
			 m_initthrow(mtmp, BULLET, 75);
		}

		if (ptr == &mons[PM_ONANOCTER__F]) {
			 (void) mongets(mtmp, STYGIAN_PIKE);
			 (void) mongets(mtmp, AUTO_SHOTGUN);
			 m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			 (void) mongets(mtmp, SNIPER_RIFLE);
			 m_initthrow(mtmp, BULLET, 50);
		}

		if (ptr == &mons[PM_ONANOCTER__G]) {
			 (void) mongets(mtmp, MALLET);
			 (void) mongets(mtmp, OBSID);
			 (void) mongets(mtmp, HEAVY_MACHINE_GUN);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);
			 (void) mongets(mtmp, ROCKET_LAUNCHER);
			 m_initthrow(mtmp, ROCKET, 3);
		}

		if (ptr == &mons[PM_ONANOCTER__H]) {
			 (void) mongets(mtmp, DROVEN_SPEAR);
			 (void) mongets(mtmp, TWO_HANDED_FLAIL);
			 (void) mongets(mtmp, BFG);
			 m_initthrow(mtmp, BFG_AMMO, 50);
			 m_initthrow(mtmp, BFG_AMMO, 50);
			 m_initthrow(mtmp, BFG_AMMO, 50);
			 m_initthrow(mtmp, BFG_AMMO, 50);
			 m_initthrow(mtmp, BFG_AMMO, 50);
			 m_initthrow(mtmp, BFG_AMMO, 50);
			 m_initthrow(mtmp, BFG_AMMO, 50);
			 m_initthrow(mtmp, BFG_AMMO, 50);
			 (void) mongets(mtmp, CUTTING_LASER);
			 m_initthrow(mtmp, LASER_BEAM, 50);
		}

		break;

	    case S_RUSTMONST:

		if (mtmp->data == &mons[PM_MASTER_DISINTEGRATOR]) (void) mongets(mtmp, WAN_DISINTEGRATION);
		if (mtmp->data == &mons[PM_DISINTEGRATOR_LORD]) (void) mongets(mtmp, WAN_DISINTEGRATION_BEAM);
		if (mtmp->data == &mons[PM_GREAT_ACID_MONSTER]) (void) mongets(mtmp, WAN_CORROSION);
		if (mtmp->data == &mons[PM_TANJA_S_SOFT_SNEAKER]) { (void) mongets(mtmp, SOFT_SNEAKERS); (void) mongets(mtmp, SOFT_GIRL_SNEAKER); }

		if (mtmp->data == &mons[PM_BLASTER_DISINTEGRATOR]) {
			 (void) mongets(mtmp, ARM_BLASTER);
			m_initthrow(mtmp, rn2(20) ? BLASTER_BOLT : HEAVY_BLASTER_BOLT, 50);
		}

		if (mtmp->data == &mons[PM_METAL_EATER]) {
			 (void) mongets(mtmp, CATAPULT);
			m_initthrow(mtmp, ROCK, 50);
			m_initthrow(mtmp, ROCK, 50);
		}

		if (mtmp->data == &mons[PM_SILVER_SUCKER]) {
			 (void) mongets(mtmp, PILE_BUNKER);
			m_initthrow(mtmp, KOKKEN, 50);
		}

		if (mtmp->data == &mons[PM_NEEDLE_MONSTER]) {
			m_initthrow(mtmp, NEEDLE, 50);
			m_initthrow(mtmp, NEEDLE, 50);
		}

		if (mtmp->data == &mons[PM_SPIKER]) {
			m_initthrow(mtmp, SPIKE, 50);
			m_initthrow(mtmp, SPIKE, 50);
		}

		break;

          case S_RODENT:
		if (mtmp->data == &mons[PM_THE_RAT_KING]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (ishaxor && mtmp->data == &mons[PM_THE_RAT_KING]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);

		if (mtmp->data == &mons[PM_QUILL_RAT]) m_initthrow(mtmp, DART, 3);
		if (mtmp->data == &mons[PM_PRICK_RAT]) m_initthrow(mtmp, DART, 3);

		if (mtmp->data == &mons[PM_SPIKE_FIEND]) m_initthrow(mtmp, DART, 5);
		if (mtmp->data == &mons[PM_THORN_BEAST]) m_initthrow(mtmp, DART, 7);

		if (mtmp->data == &mons[PM_RAZOR_SPINE]) m_initthrow(mtmp, DART, 10);
		if (mtmp->data == &mons[PM_BLADE_BACK]) m_initthrow(mtmp, DART, 10);

		if (mtmp->data == &mons[PM_DESERT_PRICK]) m_initthrow(mtmp, DART, 12);

		if (mtmp->data == &mons[PM_JUNGLE_URCHIN]) m_initthrow(mtmp, DART, 15);
		if (mtmp->data == &mons[PM_SHRUB_THORN]) m_initthrow(mtmp, DART, 15);

		if (mtmp->data == &mons[PM_THORN_RAT]) m_initthrow(mtmp, DART, 18);
		if (mtmp->data == &mons[PM_SNIPER_FIEND]) m_initthrow(mtmp, DART, 20);
		if (mtmp->data == &mons[PM_PRICK_MONSTER]) m_initthrow(mtmp, DART, 20);
		if (mtmp->data == &mons[PM_BIMMELING_RAT]) m_initthrow(mtmp, DART, 25);

		if (mtmp->data == &mons[PM_SCAVENGER]) (void) mongets(mtmp, DANCING_SHOES);
		if (mtmp->data == &mons[PM_ANASTASIA_S_DANCING_SHOE]) (void) mongets(mtmp, DANCING_SHOES);
		if (mtmp->data == &mons[PM_ANASTASIA_S_SNEAKER]) (void) mongets(mtmp, SOFT_SNEAKERS);
		if (mtmp->data == &mons[PM_ANASTASIA_S_HIGH_HEEL]) (void) mongets(mtmp, FEMININE_PUMPS);
		if (mtmp->data == &mons[PM_PLAGUE_EATER]) (void) mongets(mtmp, SWEET_MOCASSINS);
		if (mtmp->data == &mons[PM_SOLVEJG_S_MOCASSIN]) (void) mongets(mtmp, SWEET_MOCASSINS);
		if (mtmp->data == &mons[PM_JESSICA_S_MOCASSIN]) (void) mongets(mtmp, SWEET_MOCASSINS);
		if (mtmp->data == &mons[PM_SHADOW_BEAST]) (void) mongets(mtmp, WEDGE_SANDALS);
		if (mtmp->data == &mons[PM_RUEA_S_LADY_SANDAL]) (void) mongets(mtmp, WEDGE_SANDALS);
		if (mtmp->data == &mons[PM_BONE_GASHER]) (void) mongets(mtmp, HIPPIE_HEELS);
		if (mtmp->data == &mons[PM_LUDGERA_S_HIKING_BOOT]) (void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);
		if (mtmp->data == &mons[PM_MAILIE_S_THICK_GIRL_BOOT]) (void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);

		if (mtmp->data == &mons[PM_MAGDALENA_S_STINKING_COMBAT_BOOT]) { (void) mongets(mtmp, HIPPIE_HEELS);  (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); }
		if (mtmp->data == &mons[PM_ELIF_S_BLOCK_HEELED_COMBAT_BOOT]) { (void) mongets(mtmp, HIPPIE_HEELS);  (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); }
		if (mtmp->data == &mons[PM_RUEA_S_STILETTO_BOOT]) { (void) mongets(mtmp, SPIKED_BATTLE_BOOT); (void) mongets(mtmp, SCR_TRAP_CREATION);  (void) mongets(mtmp, COMBAT_STILETTOS); }
		if (mtmp->data == &mons[PM_JESSICA_S_STILETTO_BOOT]) { (void) mongets(mtmp, SPIKED_BATTLE_BOOT);  (void) mongets(mtmp, COMBAT_STILETTOS); (void) mongets(mtmp, WAN_TRAP_CREATION);}

		if (mtmp->data == &mons[PM_KATRIN_S_DANCING_SHOE]) { (void) mongets(mtmp, SCR_TRAP_CREATION);  (void) mongets(mtmp, LACQUERED_DANCING_SHOE); (void) mongets(mtmp, DANCING_SHOES); }
		if (mtmp->data == &mons[PM_ELIF_S_SNEAKER]) { (void) mongets(mtmp, SOFT_GIRL_SNEAKER); (void) mongets(mtmp, SOFT_SNEAKERS); }
		if (mtmp->data == &mons[PM_KATRIN_S_HIGH_HEELED_PUMP]) { (void) mongets(mtmp, FEMININE_PUMPS); (void) mongets(mtmp, SEXY_LEATHER_PUMP); }
		if (mtmp->data == &mons[PM_WENDY_S_SEXY_LEATHER_PUMP]) { (void) mongets(mtmp, FEMININE_PUMPS); (void) mongets(mtmp, SEXY_LEATHER_PUMP); }

		if (ptr == &mons[PM_MUTATED_BATTLE_BUNNY]) (void) mongets(mtmp, rn2(10) ? POT_MUTATION : WAN_MUTATION);

		break;

	    case S_UNICORN:

		if (mtmp->data == &mons[PM_RONJA_S_BLOCK_HEELED_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }

		break;

          case S_SNAKE:
		if (mtmp->data == &mons[PM_CLOUD_CATTLE]) (void) mongets(mtmp, SCR_CLOUDS);

		if (mtmp->data == &mons[PM_ARMED_SNAKE]) { (void) mongets(mtmp, ORCISH_BOW); m_initthrow(mtmp, ARROW, 25);}

		if (mtmp->data == &mons[PM_SNAKE_ARCHER]) { (void) mongets(mtmp, BOW); m_initthrow(mtmp, ARROW, 25);}
		if (mtmp->data == &mons[PM_VIPER_ARCHER]) { (void) mongets(mtmp, BOW); m_initthrow(mtmp, ORCISH_ARROW, 25);}
		if (mtmp->data == &mons[PM_SNAKE_SNIPER]) { (void) mongets(mtmp, SNIPER_RIFLE); m_initthrow(mtmp, BULLET, 25);}
		if (mtmp->data == &mons[PM_CLOCK_FOREST_ARCHER]) { (void) mongets(mtmp, DARK_ELVEN_BOW); m_initthrow(mtmp, ORCISH_ARROW, 25);}
		if (mtmp->data == &mons[PM_WIND_BOWMAN]) { (void) mongets(mtmp, DARK_ELVEN_BOW); m_initthrow(mtmp, DARK_ELVEN_ARROW, 25);}

		if (mtmp->data == &mons[PM_SNAKE_ELITE_SNIPER]) {
			 (void) mongets(mtmp, HELO_CROSSBOW);
			m_initthrow(mtmp, CROSSBOW_BOLT, 50);
		}

		if (mtmp->data == &mons[PM_RHEA_S_LADY_PUMP]) { (void) mongets(mtmp, FEMININE_PUMPS); (void) mongets(mtmp, SEXY_LEATHER_PUMP); (void) mongets(mtmp, SCR_TRAP_CREATION); }

		break;
	    case S_GIANT:
		if (ptr == &mons[PM_MINOTAUR]) {
		    if (!rn2(3) || (in_mklev && Is_earthlevel(&u.uz)))
			(void) mongets(mtmp, WAN_DIGGING);
		} else if (ptr == &mons[PM_CHEATING_MINOTAUR]) { /* evil patch idea by jonadab */
			if (!rn2(3)) (void) mongets(mtmp, WAN_LIGHTNING);
			else if (!rn2(2)) (void) mongets(mtmp, AMULET_OF_LIFE_SAVING);
			else (void) mongets(mtmp, AMULET_OF_REFLECTION);
		} else if (is_giant(ptr)) {
		    for (cnt = rn2((int)(mtmp->m_lev / 2)); cnt; cnt--) {

			if (!rn2(3)) { /* greatly lowered chance --Amy */
			otmp = mksobj(rnd_class(DILITHIUM_CRYSTAL,LUCKSTONE-1),
				      FALSE, FALSE);
			if (otmp) {
				otmp->quan = rnd(3); /* lower amount */
				otmp->owt = weight(otmp);
				if(!rn2(8)) {
					otmp->spe = rne(2);
					if (rn2(2)) otmp->blessed = rn2(2);
					 else	blessorcurse(otmp, 3);
				} else if(!rn2(10)) {
					if (rn2(10)) curse(otmp);
					 else	blessorcurse(otmp, 3);
					otmp->spe = -rne(2);
				} else	blessorcurse(otmp, 10);
				(void) mpickobj(mtmp, otmp, TRUE);
				}
			}

		    }
		}

		if (ptr == &mons[PM_SABINE_S_BLOCK_HEELED_SANDAL]) (void)mongets(mtmp, HIGH_HEELED_SANDAL);

		break;

	    case S_TURRET:

		if (!rn2(25)) (void)mongets(mtmp, POT_INVISIBILITY);

		if (ptr == &mons[PM_CRUDE_TURRET]) m_initthrow(mtmp, ORCISH_DAGGER, 3);
		if (ptr == &mons[PM_DAGGER_TURRET]) m_initthrow(mtmp, DAGGER, 3);
		if (ptr == &mons[PM_DROVEN_TURRET]) m_initthrow(mtmp, DARK_ELVEN_DAGGER, 3);
		if (ptr == &mons[PM_THRUSTING_TURRET]) (void)mongets(mtmp, LUCERN_HAMMER);
		if (ptr == &mons[PM_SCYTHING_TURRET]) (void)mongets(mtmp, BEC_DE_CORBIN);
		if (ptr == &mons[PM_SPEAR_TURRET]) m_initthrow(mtmp, SPEAR, 3);

		if (ptr == &mons[PM_ARROW_TURRET]) {
		  	(void) mongets(mtmp, ORCISH_BOW);
		  	m_initthrow(mtmp, ORCISH_ARROW, 25);
		}
		if (ptr == &mons[PM_LARGE_ARROW_TURRET]) {
		  	(void) mongets(mtmp, BOW);
		  	m_initthrow(mtmp, ARROW, 25);
		}
		if (ptr == &mons[PM_ELVEN_TURRET]) {
		  	(void) mongets(mtmp, ELVEN_BOW);
		  	m_initthrow(mtmp, ELVEN_ARROW, 25);
		}
		if (ptr == &mons[PM_DARK_ELVEN_TURRET]) {
		  	(void) mongets(mtmp, DARK_ELVEN_BOW);
		  	m_initthrow(mtmp, DARK_ELVEN_ARROW, 25);
		}
		if (ptr == &mons[PM_SAMU_TURRET]) {
		  	(void) mongets(mtmp, YUMI);
		  	m_initthrow(mtmp, YA, 25);
		}
		if (ptr == &mons[PM_PREHISTORIC_TURRET]) {
		  	(void) mongets(mtmp, SLING);
		  	m_initthrow(mtmp, ROCK, 50);
		}
		if (ptr == &mons[PM_NEOLITHIC_TURRET]) {
		  	(void) mongets(mtmp, SLING);
		  	m_initthrow(mtmp, FLINT, 25);
		}
		if (ptr == &mons[PM_HARDLOAD_TURRET]) {
		  	(void) mongets(mtmp, SLING);
		  	m_initthrow(mtmp, LOADSTONE, 5);
		}
		if (ptr == &mons[PM_GIGALOAD_TURRET]) {
		  	(void) mongets(mtmp, SLING);
		  	m_initthrow(mtmp, LOADBOULDER, 5);
		}
		if (ptr == &mons[PM_GUN_TURRET]) {
		  	(void) mongets(mtmp, PISTOL);
		  	m_initthrow(mtmp, BULLET, 25);
		}
		if (ptr == &mons[PM_SMG_TURRET]) {
		  	(void) mongets(mtmp, SUBMACHINE_GUN);
		  	m_initthrow(mtmp, BULLET, 50);
		}
		if (ptr == &mons[PM_GATLING_TURRET]) {
		  	(void) mongets(mtmp, HEAVY_MACHINE_GUN);
		  	m_initthrow(mtmp, BULLET, 50);
		  	m_initthrow(mtmp, BULLET, 50);
		}
		if (ptr == &mons[PM_CUNTGUN_TURRET]) {
		  	(void) mongets(mtmp, RIFLE);
		  	m_initthrow(mtmp, BULLET, 25);
		}
		if (ptr == &mons[PM_AUTOMATIC_TURRET]) {
		  	(void) mongets(mtmp, ASSAULT_RIFLE);
		  	m_initthrow(mtmp, BULLET, 50);
		}
		if (ptr == &mons[PM_SNIPER_TURRET]) {
		  	(void) mongets(mtmp, SNIPER_RIFLE);
		  	m_initthrow(mtmp, BULLET, 25);
		}
		if (ptr == &mons[PM_PELLET_TURRET]) {
		  	(void) mongets(mtmp, SHOTGUN);
		  	m_initthrow(mtmp, SHOTGUN_SHELL, 20);
		}
		if (ptr == &mons[PM_CITYKILLER_TURRET]) {
		  	(void) mongets(mtmp, AUTO_SHOTGUN);
		  	m_initthrow(mtmp, SHOTGUN_SHELL, 20);
		}
		if (ptr == &mons[PM_ROCKET_TURRET]) {
		  	(void) mongets(mtmp, ROCKET_LAUNCHER);
		  	m_initthrow(mtmp, ROCKET, 10);
		}
		if (ptr == &mons[PM_GL_TURRET] || ptr == &mons[PM_DEATH_SENTRY]) {
		  	(void) mongets(mtmp, GRENADE_LAUNCHER);
		  	m_initthrow(mtmp, FRAG_GRENADE, 10);
		}
		if (ptr == &mons[PM_SHOOTER_TURRET]) {
		  	(void) mongets(mtmp, CROSSBOW);
		  	m_initthrow(mtmp, CROSSBOW_BOLT, 35);
		}
		if (ptr == &mons[PM_OBSIDIAN_TURRET]) {
		  	(void) mongets(mtmp, DROVEN_CROSSBOW);
		  	m_initthrow(mtmp, DROVEN_BOLT, 35);
		}
		if (ptr == &mons[PM_GLASS_TURRET]) {
		  	(void) mongets(mtmp, DROVEN_BOW);
		  	m_initthrow(mtmp, DROVEN_ARROW, 25);
		}
		if (ptr == &mons[PM_ANCIENT_TURRET]) {
		  	(void) mongets(mtmp, BOW);
		  	m_initthrow(mtmp, ANCIENT_ARROW, 25);
		}
		if (ptr == &mons[PM_HYDRA_TURRET]) {
		  	(void) mongets(mtmp, HYDRA_BOW);
		  	m_initthrow(mtmp, GOLDEN_ARROW, 25);
		}
		if (ptr == &mons[PM_LASER_TURRET]) {
		  	(void) mongets(mtmp, HAND_BLASTER);
		  	m_initthrow(mtmp, rn2(20) ? BLASTER_BOLT : HEAVY_BLASTER_BOLT, 40);
		}
		if (ptr == &mons[PM_BLASTER_TURRET]) {
		  	(void) mongets(mtmp, ARM_BLASTER);
		  	m_initthrow(mtmp, rn2(20) ? BLASTER_BOLT : HEAVY_BLASTER_BOLT, 50);
		}
		if (ptr == &mons[PM_CUTTING_TURRET]) {
		  	(void) mongets(mtmp, CUTTING_LASER);
		  	m_initthrow(mtmp, LASER_BEAM, 50);
		}
		if (ptr == &mons[PM_RAY_TURRET]) {
		  	(void) mongets(mtmp, RAYGUN);
		  	m_initthrow(mtmp, LASER_BEAM, 50);
		}
		if (ptr == &mons[PM_ATOMIC_TURRET]) {
		  	(void) mongets(mtmp, BFG);
		  	m_initthrow(mtmp, BFG_AMMO, 50);
		  	m_initthrow(mtmp, BFG_AMMO, 50);
		  	m_initthrow(mtmp, BFG_AMMO, 50);
		  	m_initthrow(mtmp, BFG_AMMO, 50);
		  	m_initthrow(mtmp, BFG_AMMO, 50);
		  	m_initthrow(mtmp, BFG_AMMO, 50);
		  	m_initthrow(mtmp, BFG_AMMO, 50);
		  	m_initthrow(mtmp, BFG_AMMO, 50);
		}

		if (ptr == &mons[PM_DART_TURRET]) m_initthrow(mtmp, DART, 20);
		if (ptr == &mons[PM_NINJA_TURRET]) m_initthrow(mtmp, SHURIKEN, 50);

		break;

	    case S_EEL:

		if (ptr == &mons[PM_FLINTFISH]) {
			(void) mongets(mtmp, FLINTLOCK);
	  		m_initthrow(mtmp, BULLET, 20);
		}

		if (ptr == &mons[PM_CUTTERFISH]) {
			(void) mongets(mtmp, CUTTING_LASER);
	  		m_initthrow(mtmp, LASER_BEAM, 50);
		}

		if (ptr == &mons[PM_ANNOYING_FISH]) {
	  		m_initthrow(mtmp, CALTROP, 50);
		}

		if (ptr == &mons[PM_CATFISH]) {
			(void) mongets(mtmp, CATAPULT);
	  		m_initthrow(mtmp, ROCK, 50);
	  		m_initthrow(mtmp, ROCK, 50);
	  		m_initthrow(mtmp, ROCK, 50);
		}

		if (ptr == &mons[PM_VERY_ANNOYING_FISH]) {
	  		m_initthrow(mtmp, SPIKE, 50);
	  		m_initthrow(mtmp, SPIKE, 50);
		}

		if (ptr == &mons[PM_EXTREMELY_ANNOYING_FISH]) {
	  		m_initthrow(mtmp, NEEDLE, 50);
	  		m_initthrow(mtmp, NEEDLE, 50);
	  		m_initthrow(mtmp, NEEDLE, 50);
	  		m_initthrow(mtmp, NEEDLE, 50);
		}

		if (ptr == &mons[PM_GREEK_FISH]) {
			(void) mongets(mtmp, HELO_CROSSBOW);
	  		m_initthrow(mtmp, CROSSBOW_BOLT, 50);
		}

		if (ptr == &mons[PM_SOLDIER_STINKER]) 

			{
			  (void) mongets(mtmp, SUBMACHINE_GUN);
		  	  m_initthrow(mtmp, BULLET, 25);
		  	  m_initthrow(mtmp, BULLET, 25);
			  (void) mongets(mtmp, DAGGER);
			  (void) mongets(mtmp, FRAG_GRENADE);
			}

		if (ptr == &mons[PM_BATTLE_TANK]) 

			{
			  (void) mongets(mtmp, HEAVY_MACHINE_GUN);
		  	  m_initthrow(mtmp, BULLET, 25);
		  	  m_initthrow(mtmp, BULLET, 25);
		  	  m_initthrow(mtmp, BULLET, 25);
		  	  m_initthrow(mtmp, BULLET, 25);
			  (void) mongets(mtmp, JAGGED_STAR);
			}

		if (ptr == &mons[PM_LIVING_CANNON]) 

			{
			  (void) mongets(mtmp, ROCKET_LAUNCHER);
		  	  m_initthrow(mtmp, ROCKET, 5);
			  (void) mongets(mtmp, MALLET);
			}

		break;

	    case S_WRAITH:
	    if (!rn2(2)) (void)mongets(mtmp, ROBE);

		if (ptr == &mons[PM_SOPHIA_S_HIGH_HEELED_LADY_S_SHOE]) { (void)mongets(mtmp, COMBAT_STILETTOS); (void)mongets(mtmp, SPIKED_BATTLE_BOOT); }

		if (ptr == &mons[PM_NAZGUL]) {
			otmp = mksobj(RIN_INVISIBILITY, FALSE, FALSE);
			if (otmp) {
				curse(otmp);
				(void) mpickobj(mtmp, otmp, TRUE);
			}
		}

		if (ptr == &mons[PM_BLEACH_GHOST]) m_initthrow(mtmp, CALTROP, 50);

		break;

	    case S_ORC:

		if (ptr == &mons[PM_DESU_SENTAI]) {
			(void) mongets(mtmp, FLINTLOCK);
	  		m_initthrow(mtmp, BULLET, 20);
		}

		if (ptr == &mons[PM_GOBLIN_DART_THROWER]) {
	  		m_initthrow(mtmp, DART, 35);
		}

		if (ptr == &mons[PM_GOBLIN_GRENADIER]) {
	  		m_initthrow(mtmp, FRAG_GRENADE, 10);
		}

		if (ptr == &mons[PM_SMUT_ORC]) {
			if (!rn2(3)) (void) mongets(mtmp, WAN_SUMMON_SEXY_GIRL);
			else if (!rn2(2)) (void) mongets(mtmp, SCR_GIRLINESS);
			else 	(void) maketrap(mtmp->mx, mtmp->my, rn2(2) ? FART_TRAP : HEEL_TRAP, 0);
		}
		break;
	    case S_FLYFISH:

		if (mtmp->data == &mons[PM_ANNE_S_SNEAKER]) { (void) mongets(mtmp, SOFT_GIRL_SNEAKER); (void) mongets(mtmp, SOFT_SNEAKERS); }

		if (ptr == &mons[PM_FLYING_HYDRA]) {
			(void) mongets(mtmp, HYDRA_BOW);
			m_initthrow(mtmp, ANCIENT_ARROW, 50);
			m_initthrow(mtmp, GOLDEN_ARROW, 50);
		}

		break;

	    case S_LICH:

		if (mtmp->data == &mons[PM_SUESCHEN_S_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }

		if (ptr == &mons[PM_ELUSIVE_LICH]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (ishaxor && ptr == &mons[PM_ELUSIVE_LICH]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (ptr == &mons[PM_ORE_LICH]) (void) mongets(mtmp, WAN_SUMMON_UNDEAD);

		if (ptr == &mons[PM_CUTLICH]) {
			(void) mongets(mtmp, CUTTING_LASER);
			m_initthrow(mtmp, LASER_BEAM, 50);
		}

		if (ptr == &mons[PM_BANG_LICH]) {
			(void) mongets(mtmp, SHOTGUN);
			m_initthrow(mtmp, SHOTGUN_SHELL, 30);
		}

		if (ptr == &mons[PM_FLINT_LICH]) {
			(void) mongets(mtmp, FLINTLOCK);
			m_initthrow(mtmp, BULLET, 25);
		}

		if (ptr == &mons[PM_ANTI_UNDEAD_LICH]) {
			(void) mongets(mtmp, DEMON_CROSSBOW);
			m_initthrow(mtmp, KOKKEN, 50);
		}

		if (ptr == &mons[PM_HELLISH_LICH]) {
			(void) mongets(mtmp, DEMON_CROSSBOW);
			m_initthrow(mtmp, CROSSBOW_BOLT, 50);
		}

		if (ptr == &mons[PM_PSCHI_LICH]) {
			(void) mongets(mtmp, PILE_BUNKER);
			m_initthrow(mtmp, CROSSBOW_BOLT, 50);
		}

		if (ptr == &mons[PM_SNIPER_LICH]) {
			(void) mongets(mtmp, HELO_CROSSBOW);
			m_initthrow(mtmp, DROVEN_BOLT, 50);
		}

		if (ptr == &mons[PM_CAT_LICH]) {
			(void) mongets(mtmp, CATAPULT);
			m_initthrow(mtmp, ROCK, 50);
			m_initthrow(mtmp, ROCK, 50);
		}

		if (ptr == &mons[PM_HYDRA_LICH]) {
			(void) mongets(mtmp, HYDRA_BOW);
			m_initthrow(mtmp, ANCIENT_ARROW, 50);
			m_initthrow(mtmp, GOLDEN_ARROW, 50);
		}

		if (ptr == &mons[PM_GUN_LICH]) {
			(void) mongets(mtmp, PISTOL);
			m_initthrow(mtmp, BULLET, 25);
			m_initthrow(mtmp, BULLET, 25);
		}

		if (ptr == &mons[PM_EGYPTIAN_PHARAO]) {
			(void) mongets(mtmp, ARM_BLASTER);
			m_initthrow(mtmp, rn2(20) ? BLASTER_BOLT : HEAVY_BLASTER_BOLT, 50);
		}

		if (ptr == &mons[PM_MASTER_LICH] && !rn2(13))
			(void)mongets(mtmp, (rn2(7) ? ATHAME : WAN_NOTHING));
		else if (ptr == &mons[PM_ARCH_LICH] && !rn2(3)) {
			otmp = mksobj(rn2(3) ? ATHAME : QUARTERSTAFF,
				      TRUE, rn2(13) ? FALSE : TRUE);
			if (otmp) {
				if (otmp->spe < 2) otmp->spe = rnd(3);
				if (!rn2(4)) otmp->oerodeproof = 1;
				(void) mpickobj(mtmp, otmp, TRUE);
			}
		}
		else if (ptr == &mons[PM_PHARAO]) {
		  	(void) mongets(mtmp, (rn2(5) ? ASSAULT_RIFLE : HEAVY_MACHINE_GUN));
		  	m_initthrow(mtmp, BULLET, 60);
		  	m_initthrow(mtmp, BULLET, 60);
		}
		else if (ptr == &mons[PM_ANCIENT_PHARAO]) {
		  	(void) mongets(mtmp, HEAVY_MACHINE_GUN);
		  	m_initthrow(mtmp, BULLET, 60);
		  	m_initthrow(mtmp, BULLET, 60);
		}
		break;

	    case S_OGRE:

		if (monsndx(ptr) == PM_HIDEOUS_OGRE_MAGE) (void) mongets(mtmp, SCR_TRAP_CREATION);
		if (monsndx(ptr) == PM_GROTESQUE_OGRE_SHAMAN) (void) mongets(mtmp, SCR_TRAP_CREATION);

		if (ptr == &mons[PM_CUTTER_OGRE]) {
			(void) mongets(mtmp, CUTTING_LASER);
			m_initthrow(mtmp, LASER_BEAM, 50);
		}
		break;

	    case S_MUMMY:
		if (rn2(7)) (void)mongets(mtmp, MUMMY_WRAPPING);
		break;
	    case S_QUANTMECH:
		if (monsndx(ptr) == PM_QUANTUM_MECHANIC && !rn2(20)) {
			otmp = mksobj(LARGE_BOX, FALSE, FALSE);
			if (otmp) {
				otmp->spe = 1; /* flag for special box */
				otmp->owt = weight(otmp);
				(void) mpickobj(mtmp, otmp, TRUE);
			}
		}

		if (monsndx(ptr) == PM_INCINERATOR) m_initthrow(mtmp, FRAG_GRENADE, 12);
		if (monsndx(ptr) == PM_STEAM_POWERED_PISTOL_SENTRY) { (void) mongets(mtmp, PISTOL); m_initthrow(mtmp, BULLET, 50); }
		if (monsndx(ptr) == PM_STEAM_POWERED_RIFLE_SENTRY) { (void) mongets(mtmp, RIFLE); m_initthrow(mtmp, BULLET, 50); }
		if (monsndx(ptr) == PM_STEAM_POWERED_SHOTGUN_SENTRY) { (void) mongets(mtmp, SHOTGUN); m_initthrow(mtmp, SHOTGUN_SHELL, 50); }
		if (monsndx(ptr) == PM_STEAM_POWERED_ROCKET_SENTRY) { (void) mongets(mtmp, ROCKET_LAUNCHER); m_initthrow(mtmp, ROCKET, 50); }
		if (monsndx(ptr) == PM_STEAM_POWERED_MISSILE_SENTRY) { (void) mongets(mtmp, ROCKET_LAUNCHER); m_initthrow(mtmp, ROCKET, 50); m_initthrow(mtmp, FRAG_GRENADE, 50); }
		if (monsndx(ptr) == PM_NAZI_PANTHER_TANK) { (void) mongets(mtmp, HEAVY_MACHINE_GUN); (void) mongets(mtmp, ROCKET_LAUNCHER); m_initthrow(mtmp, ROCKET, 50); m_initthrow(mtmp, BULLET, 50); m_initthrow(mtmp, BULLET, 50); m_initthrow(mtmp, BULLET, 50); m_initthrow(mtmp, BULLET, 50); }
		if (monsndx(ptr) == PM_GATLING_AUTOMATA) { (void) mongets(mtmp, HEAVY_MACHINE_GUN); m_initthrow(mtmp, BULLET, 50); m_initthrow(mtmp, BULLET, 50); m_initthrow(mtmp, BULLET, 50); m_initthrow(mtmp, BULLET, 50); }
		if (monsndx(ptr) == PM_MOBILE_REPLICATING_MACHINE_GUN_AUTOMATA_TYPE_IX) { (void) mongets(mtmp, HEAVY_MACHINE_GUN); m_initthrow(mtmp, BULLET, 50); m_initthrow(mtmp, BULLET, 50); m_initthrow(mtmp, BULLET, 50); m_initthrow(mtmp, BULLET, 50); }
		if (monsndx(ptr) == PM_BOMB_CARRYING_ORNITHOPTER_AUTOMATON) m_initthrow(mtmp, FRAG_GRENADE, 20);

	      if (monsndx(ptr) == PM_CLOCKWORK_AUTOMATON) {
	        (void) mongets(mtmp, rn2(2)? BROADSWORD : SCIMITAR);
	        (void) mongets(mtmp, BRONZE_PLATE_MAIL);
	      }
		if (monsndx(ptr) == PM_DOCTOR_FRANKENSTEIN) {
			(void)mongets(mtmp, LAB_COAT);
			(void)mongets(mtmp, WAN_POLYMORPH);
			(void)mongets(mtmp, SPE_POLYMORPH);
		}

		if (ptr == &mons[PM_ACTUAL_MONSTER_GENERATOR]) {
			(void) mongets(mtmp, SCR_SUMMON_BOSS);
			if (!rn2(3)) (void) mongets(mtmp, SCR_SUMMON_BOSS);
			if (!rn2(7)) {
				(void) mongets(mtmp, SCR_SUMMON_BOSS);
				(void) mongets(mtmp, SCR_SUMMON_BOSS);
			}
			if (!rn2(35)) {
				(void) mongets(mtmp, SCR_SUMMON_BOSS);
				(void) mongets(mtmp, SCR_SUMMON_BOSS);
				(void) mongets(mtmp, SCR_SUMMON_BOSS);
			}
			if (!rn2(335)) {
				(void) mongets(mtmp, SCR_SUMMON_BOSS);
				(void) mongets(mtmp, SCR_SUMMON_BOSS);
				(void) mongets(mtmp, SCR_SUMMON_BOSS);
				(void) mongets(mtmp, SCR_SUMMON_BOSS);
				(void) mongets(mtmp, SCR_SUMMON_BOSS);
			}
			(void) mongets(mtmp, WAN_CREATE_HORDE);
			(void) mongets(mtmp, WAN_SUMMON_UNDEAD);
		}

 		break;

		break;
	    case S_LEPRECHAUN:
#ifndef GOLDOBJ
		mtmp->mgold = (long) d(level_difficulty(), 30);
#else
		mkmonmoney(mtmp, (long) d(level_difficulty(), 30));
#endif
		if (monsndx(ptr) == PM_LUPIN_THE_THIRD) (void) mongets(mtmp, WAN_TRAP_CREATION);
		if (monsndx(ptr) == PM_GRETA_S_HUGGING_BOOT) { (void) mongets(mtmp, HUGGING_BOOT); (void) mongets(mtmp, SCR_TRAP_CREATION); }

		if (monsndx(ptr) == PM_CORPSE_SHREDDER) m_initthrow(mtmp, FRAG_GRENADE, 3);
		if (monsndx(ptr) == PM_CORNUCHULA) m_initthrow(mtmp, FRAG_GRENADE, 4);
		if (monsndx(ptr) == PM_SKI_JIR) m_initthrow(mtmp, FRAG_GRENADE, 5);
		if (monsndx(ptr) == PM_CODE_EDITOR) m_initthrow(mtmp, FRAG_GRENADE, 10);
		if (monsndx(ptr) == PM_MUNCHKIN) { (void) mongets(mtmp, ROCKET_LAUNCHER); m_initthrow(mtmp, ROCKET, 10); }

		break;
       case S_ZRUTY: /* case S_ZOUTHERN: */
	      if (ptr == &mons[PM_BANNIK] && !rn2(5)) (void) mongets(mtmp, TOWEL);
	      if (ptr == &mons[PM_LESHY]) (void) mongets(mtmp, APPLE);

		if (ptr == &mons[PM_MUTATED_UNDEAD_POTATO]) (void) mongets(mtmp, rn2(10) ? POT_MUTATION : WAN_MUTATION);
	      if (ptr == &mons[PM_FRANZI_S_WEDGE_SANDAL]) { mongets(mtmp, WEDGE_SANDALS); mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL); }

	      break;

          case S_VORTEX: 

	      if (ptr == &mons[PM_EVELINE_S_SWEET_WEDGE_SANDAL]) { mongets(mtmp, WEDGE_SANDALS); mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL); }
		if (monsndx(ptr) == PM_NICHOLAS_D__WOLFWOOD) { (void) mongets(mtmp, ROCKET_LAUNCHER); m_initthrow(mtmp, ROCKET, 10); }

	    break;

	    case S_ELEMENTAL:        
  /*            if(ptr == &mons[PM_WATER_WEIRD]){
			otmp = mksobj(WAN_WISHING,TRUE,FALSE);
			if (otmp) {
				otmp->spe=3;
				otmp->blessed=0;
				mpickobj(mtmp, otmp, TRUE);
			}
		}*/

		if (ptr == &mons[PM_HEAVY_GIRL]) (void) mongets(mtmp, COMBAT_STILETTOS);
		if (monsndx(ptr) == PM_ICELANDIC_GUN_CHICK) { (void) mongets(mtmp, PISTOL); m_initthrow(mtmp, BULLET, 30); }

		if (ptr == &mons[PM_SUPER_ELEMENTAL]) (void) mongets(mtmp, SCR_ELEMENTALISM);

		if (ptr == &mons[PM_IMBA_WATER_ELEMENTAL]) (void) mongets(mtmp, WAN_TIDAL_WAVE);

		if (ptr == &mons[PM_ULTRA_ELEMENTAL]) {
			(void) mongets(mtmp, SCR_ELEMENTALISM);
			(void) mongets(mtmp, SCR_ELEMENTALISM);
		}

		/* evil patch idea by jonadab; he wants them to turn the tile a player is on into lava
               I'll just give them an appropriate scroll instead. --Amy */
		if (ptr == &mons[PM_THEMATIC_FIRE_ELEMENTAL]) (void) mongets(mtmp, SCR_LAVA);

		if (ptr == &mons[PM_THEMATIC_ASTRAL_ELEMENTAL]) (void) mongets(mtmp, rn2(10) ? SCR_BAD_EFFECT : WAN_BAD_EFFECT);

		break;	
	    case S_VAMPIRE:
		/* [Lethe] Star and fire vampires don't get this stuff */
		if (ptr == &mons[PM_STAR_VAMPIRE] || 
				ptr == &mons[PM_FIRE_VAMPIRE])
		    break;
	    	/* Get opera cloak */
/*	    	otmp = readobjnam(opera_cloak);
		if (otmp && otmp != &zeroobj) mpickobj(mtmp, otmp, TRUE);*/
		for (i = STRANGE_OBJECT; i < NUM_OBJECTS; i++) {
			register const char *zn;
			if ((zn = OBJ_DESCR(objects[i])) && !strcmpi(zn, "opera cloak")) {
				if (!OBJ_NAME(objects[i])) i = STRANGE_OBJECT;
				break;
			}
			if ((zn = OBJ_DESCR(objects[i])) && !strcmpi(zn, "nakidka")) {
				if (!OBJ_NAME(objects[i])) i = STRANGE_OBJECT;
				break;
			}
			if ((zn = OBJ_DESCR(objects[i])) && !strcmpi(zn, "operasi plash")) {
				if (!OBJ_NAME(objects[i])) i = STRANGE_OBJECT;
				break;
			}
		}
		if (i != NUM_OBJECTS) (void)mongets(mtmp, i);
		if (rn2(2)) {
		    if ((int) mtmp->m_lev > rn2(30))
			(void)mongets(mtmp, POT_VAMPIRE_BLOOD);
		    else
			(void)mongets(mtmp, POT_BLOOD);
		}

		if(ptr == &mons[PM_SING_S_PLATEAU_BOOT]) (void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);

		if (ptr == &mons[PM_ANTI_VAMPIRE]) {
			(void) mongets(mtmp, DEMON_CROSSBOW);
			m_initthrow(mtmp, KOKKEN, 50);
		}

		if (ptr == &mons[PM_GOD_OF_STARCRAFT_UNIVERSE]) {
		(void)mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (!rn2(3)) (void)mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (!rn2(7)) (void)mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (!rn2(11)) (void)mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (!rn2(15)) (void)mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		}

		break;
	    case S_DEMON:
		/* moved here from m_initweap() because these don't
		   have AT_WEAP so m_initweap() is not called for them */
		if (ptr == &mons[PM_ICE_DEVIL] && !rn2(4)) {
			(void)mongets(mtmp, SPEAR);
		/* [DS] Cthulhu isn't fully integrated yet, and he won't be
		 *      until Moloch's Sanctum is rearranged */
		} else if (ptr == &mons[PM_CTHULHU]) {
			/*(void)mongets(mtmp, AMULET_OF_YENDOR);*/ /* the high priest already has it, and we're not gonna scrap him --Amy */
			(void)mongets(mtmp, WAN_DEATH);
			(void)mongets(mtmp, POT_FULL_HEALING);
		} else if (ptr == &mons[PM_ASMODEUS]) {
			(void)mongets(mtmp, WAN_COLD);
			(void)mongets(mtmp, WAN_FIRE);
		}

		if(ptr == &mons[PM_CHARGING_KINDNESS] || ptr == &mons[PM_ARMORED_BUNDLE_GIRL]) (void) mongets(mtmp, COMBAT_STILETTOS);
		if (mtmp->data == &mons[PM_CONNY_S_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }
		if (mtmp->data == &mons[PM_BENGE_BARBAROIS]) { (void) mongets(mtmp, SCR_TRAP_CREATION); (void) mongets(mtmp, WAN_TRAP_CREATION); }

		if(ptr == &mons[PM_FRIEDERIKE_S_SOFT_DANCING_SHOE]) (void) mongets(mtmp, DANCING_SHOES);

		if(ptr == &mons[PM_GREATER_NALFESHNEE]) (void) mongets(mtmp, WAN_CREATE_MONSTER);
		if(ptr == &mons[PM_MASTER_NALFESHNEE]) (void) mongets(mtmp, WAN_CREATE_HORDE);

		if(ptr == &mons[PM_SUPER_DEMON]) (void) mongets(mtmp, SCR_DEMONOLOGY);
		if(ptr == &mons[PM_SLOPPY_SLOTH]) (void) mongets(mtmp, WAN_INERTIA);

		if(ptr == &mons[PM_DEADLY_SIN]) (void) mongets(mtmp, SCR_SIN);
		if(ptr == &mons[PM_ICON_OF_SIN]) (void) mongets(mtmp, WAN_SIN);

		if (ptr == &mons[PM_INA_S_HUGGING_BOOT]) (void)mongets(mtmp, HUGGING_BOOT);

		if(ptr == &mons[PM_ULTRA_DEMON]) {
		 (void) mongets(mtmp, SCR_DEMONOLOGY);
		 (void) mongets(mtmp, SCR_DEMONOLOGY);
		}

		if (mtmp->data == &mons[PM_BAEL]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_ZUGGTMOY]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_BAPHOMET]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_DAUGHTER_LILITH]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_MAMMON]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_XINIVRAE]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_MALCANTHET]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_MOTHER_LILITH]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_BELIAL]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_CRONE_LILITH]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_LYNKHAB]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_LEVIATHAN]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_MEPHISTOPHELES]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_BAALPHEGOR]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_GRAZ_ZT]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_PALE_NIGHT]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_VERIER]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_JUIBLEX]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_JUBILEX]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_YEENOGHU]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_ORCUS]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_GERYON]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_DISPATER]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_BAALZEBUB]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_ASMODEUS]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_DEMOGORGON]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_CTHULHU]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);

	if (ishaxor) {
		if (mtmp->data == &mons[PM_BAEL]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_ZUGGTMOY]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_BAPHOMET]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_DAUGHTER_LILITH]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_MAMMON]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_XINIVRAE]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_MALCANTHET]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_MOTHER_LILITH]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_BELIAL]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_CRONE_LILITH]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_LYNKHAB]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_LEVIATHAN]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_MEPHISTOPHELES]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_BAALPHEGOR]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_GRAZ_ZT]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_PALE_NIGHT]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_VERIER]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_JUIBLEX]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_JUBILEX]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_YEENOGHU]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_ORCUS]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_GERYON]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_DISPATER]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_BAALZEBUB]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_ASMODEUS]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_DEMOGORGON]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_CTHULHU]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
	}

		break;
	    case S_ANGEL:
		if (monsndx(ptr) == PM_DOUGLAS_ADAMS) {
			(void) mongets(mtmp,HITCHHIKER_S_GUIDE_TO_THE_GALA);
		}

		if (monsndx(ptr) == PM_MARTIAN_MISSILE_WALKER) {
			(void) mongets(mtmp,ROCKET_LAUNCHER);
			 m_initthrow(mtmp, ROCKET, 10);
		}

		if (monsndx(ptr) == PM_SELENITE) {
			(void) mongets(mtmp, FLINTLOCK);
			 m_initthrow(mtmp, BULLET, 20);
		}

		if (mtmp->data == &mons[PM_VICTORIA_S_LADY_PUMP]) { (void) mongets(mtmp, SEXY_LEATHER_PUMP); (void) mongets(mtmp, FEMININE_PUMPS); }
		if (mtmp->data == &mons[PM_VICTORIA_S_FLEECY_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }
		if (mtmp->data == &mons[PM_VICTORIA_S_HIGH_HEELED_SANDAL]) { (void) mongets(mtmp, HIGH_HEELED_SANDAL); (void) mongets(mtmp, WEDGE_SANDALS); }
		break;

	    case S_XORN:
		if (monsndx(ptr) == PM_MARVIN) {
			int i;
			int t[3] = {DIODE, TRANSISTOR, IC};
			for (i=0;i<3;i++) {
				otmp = mksobj(t[i],FALSE,FALSE);
				if (otmp) {
					otmp->quan = rn2(7) + 1;
					otmp->owt = weight(otmp);
					mpickobj(mtmp,otmp, TRUE);
				}
			}
		}
		break;
 
	    case S_XAN:
		if (ptr == &mons[PM_RANDOM_MONSTER_GENERATOR]) (void) mongets(mtmp, SCR_GROUP_SUMMONING);
		if (ptr == &mons[PM_KATIA_S_LEATHER_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }

		if (monsndx(ptr) == PM_DEEP_THOUGHT) {
			otmp = mksobj(IC,FALSE,FALSE);
			if (otmp) {
				otmp->quan = 42;
	 			otmp->owt = weight(otmp);
				mpickobj(mtmp,otmp, TRUE);
			}
 		}
 		break;

	    case S_WALLMONST:

		if (ptr == &mons[PM_DILATIONAL_BLACK_HOLE]) (void) mongets(mtmp, SCR_ANTIMATTER);

 		break;

	    case S_YETI:

		if (ptr == &mons[PM_KATI_S_WEDGE_HEELED_PUMP]) { (void) mongets(mtmp, FEMININE_PUMPS); (void) mongets(mtmp, SEXY_LEATHER_PUMP); }
		if (ptr == &mons[PM_KATI_S_THICK_GIRL_BOOT]) { (void) mongets(mtmp, HIPPIE_HEELS); (void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS); }
		if (ptr == &mons[PM_KATI_S_STRIPED_SNEAKER]) { (void) mongets(mtmp, SOFT_SNEAKERS); (void) mongets(mtmp, SOFT_GIRL_SNEAKER); }

		if (monsndx(ptr) == PM_ZAPHOD_BREEBLEBROX) {
			otmp = mksobj(POT_PAN_GALACTIC_GARGLE_BLASTE,FALSE,FALSE);
			if (otmp) {
				otmp->quan = d(2,4);
				otmp->owt = weight(otmp);
				mpickobj(mtmp,otmp, TRUE);
			}
		}

		if (ptr == &mons[PM_CHAOS_MONKEY]) { /* idea by deepy */

			(void) mongets(mtmp, POT_POLYMORPH);
			(void) mongets(mtmp, RIN_TIMELY_BACKUP);
			(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
			(void) mongets(mtmp, POT_STUNNING);
			(void) mongets(mtmp, POT_FULL_HEALING);
			(void) mongets(mtmp, SCR_CREATE_MONSTER);
			(void) mongets(mtmp, SCR_FLOOD);
			(void) mongets(mtmp, SCR_EARTH);
			(void) mongets(mtmp, SCR_WARPING);
			(void) mongets(mtmp, SCR_BAD_EFFECT);
			(void) mongets(mtmp, SCR_LAVA);
			(void) mongets(mtmp, SCR_BARRHING);
			(void) mongets(mtmp, SCR_LOCKOUT);
			(void) mongets(mtmp, SCR_TRAP_CREATION);
			(void) mongets(mtmp, WAN_SUMMON_UNDEAD);
			(void) mongets(mtmp, WAN_CREATE_HORDE);
			(void) mongets(mtmp, WAN_DIGGING);

		}

 		break;

	    case S_HUMANOID:

		if (ptr == &mons[PM_DWARVEN_MESSENGER]) (void) mongets(mtmp, SCR_SUMMON_BOSS);
		if (ptr == &mons[PM_LEGENDARY_HOBBIT_ROGUE]) (void) mongets(mtmp, SCR_TRAP_CREATION);
		if (ptr == &mons[PM_CZECH_GIRL]) (void) mongets(mtmp, HIPPIE_HEELS);
		if (ptr == &mons[PM_VIETNAMESE_BEAUTY]) (void) mongets(mtmp, HIGH_STILETTOS);
		if (ptr == &mons[PM_SWEET_SISTER]) (void) mongets(mtmp, SOFT_SNEAKERS);
		if (ptr == &mons[PM_LOVELY_GIRL]) {
			(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			(void) mongets(mtmp, HIGH_STILETTOS);
		}

 		break;

	    case S_JELLY:

		if (ptr == &mons[PM_ENEMY_CUBE_JELLY]) (void) mongets(mtmp, SCR_SUMMON_BOSS);
		if (ptr == &mons[PM_GREATER_ENEMY_CUBE_JELLY]) (void) mongets(mtmp, SCR_SUMMON_BOSS);
		if (mtmp->data == &mons[PM_NORA_S_BEAUTIFUL_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }

		if (mtmp->data == &mons[PM_BLAKE_STONE_JELLY]) {
			 (void) mongets(mtmp, ARM_BLASTER);
			m_initthrow(mtmp, rn2(20) ? BLASTER_BOLT : HEAVY_BLASTER_BOLT, 50);
		}

 		break;

	    case S_EYE:
		if (ptr == &mons[PM_OAK_SAGE]) (void) mongets(mtmp, POT_FULL_HEALING);
		if (ptr == &mons[PM_VISOR]) (void) mongets(mtmp, WAN_MAKE_VISIBLE);

 		break;
	    case S_GRUE:
		if (ptr == &mons[PM_ROCKET_MINE] || ptr == &mons[PM_BOUNCING_MINE]) {
			(void) mongets(mtmp, ROCKET_LAUNCHER);
			m_initthrow(mtmp, ROCKET, 10);
		}

		if (mtmp->data == &mons[PM_ANNEMARIE_S_GIRL_SHOE]) { (void) mongets(mtmp, LACQUERED_DANCING_SHOE); (void) mongets(mtmp, WEDGE_SANDALS); }

 		break;
	    case S_SPIDER:
		if (ptr == &mons[PM_CAMPER_ASSHOLE]) { (void)mongets(mtmp, SNIPER_RIFLE);
			 m_initthrow(mtmp, BULLET, 10);

		}

		if (ptr == &mons[PM_NONEXISTANT_COP]) {
			(void)mongets(mtmp, RUBBER_HOSE);
			m_initthrow(mtmp, CREAM_PIE, 2);
		}

		if (mtmp->data == &mons[PM_SARAH_S_HUGGING_BOOT]) { (void) mongets(mtmp, HUGGING_BOOT); (void) mongets(mtmp, DANCING_SHOES); }

 		break;

	    case S_DRAGON:
		if (ptr == &mons[PM_DRAGON_LORD]) {
			struct obj *otmp;
			otmp = mksobj(GRAY_DRAGON_SCALE_MAIL,TRUE,FALSE);
			if (otmp) {
				otmp = oname(otmp,artiname(ART_SCALES_OF_THE_DRAGON_LORD));
				if (otmp) mpickobj(mtmp,otmp, TRUE);
			}
		}

		if (ptr == &mons[PM_RICHEST_MAN_OF_THE_WORLD]) {
		    mtmp->mgold += (long)rn1(20, 200);
		}

		if (ptr == &mons[PM_WICKED_WOODS]) (void) mongets(mtmp, SCR_GROWTH);
		if (ptr == &mons[PM_ARCTIS_INHABITANT]) (void) mongets(mtmp, SCR_ICE);
		if (ptr == &mons[PM_FALLING_BALL_DRAGON]) (void) mongets(mtmp, HEAVY_IRON_BALL);
		if (ptr == &mons[PM_TELEPHONING_BALL_DRAGON]) (void) mongets(mtmp, HEAVY_IRON_BALL);
		if (ptr == &mons[PM_RAIN_CLOUD_DRAGON]) (void) mongets(mtmp, SCR_FLOOD);
		if (ptr == &mons[PM_WATER_USING_DRAGON]) (void) mongets(mtmp, SCR_FLOOD);
		if (ptr == &mons[PM_RAIN_GUESSING_DRAGON]) (void) mongets(mtmp, SCR_FLOOD);
		if (ptr == &mons[PM_GREEN_ANIMATION_DRAGON]) (void) mongets(mtmp, SCR_CREATE_MONSTER);
		if (ptr == &mons[PM_ANIMAL_ANTICS]) (void) mongets(mtmp, WAN_CREATE_MONSTER);
		if (ptr == &mons[PM_TRAP_INSTALLING_DRAGON]) (void) mongets(mtmp, SCR_TRAP_CREATION);
		if (ptr == &mons[PM_ARBITRATOR]) (void) mongets(mtmp, TIN_WHISTLE);

		if (ptr == &mons[PM_VANESSA_S_THICK_LADY_SHOE]) (void) mongets(mtmp, HIPPIE_HEELS);
		if (ptr == &mons[PM_KSENIA_S_PLATEAU_SANDAL]) { (void) mongets(mtmp, HIGH_HEELED_SANDAL); (void) mongets(mtmp, WEDGE_SANDALS); }
		if (ptr == &mons[PM_MAURAH_S_HUGGING_BOOT]) { (void) mongets(mtmp, HUGGING_BOOT); (void) mongets(mtmp, COMBAT_STILETTOS); }
		if (mtmp->data == &mons[PM_ARIANE_S_FLEECY_COMBAT_BOOT]) { (void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT); (void) mongets(mtmp, HIPPIE_HEELS); }
		if (ptr == &mons[PM_NELLY_S_ELEGANT_PUMP]) { (void) mongets(mtmp, SEXY_LEATHER_PUMP); (void) mongets(mtmp, FEMININE_PUMPS); }
		if (ptr == &mons[PM_MAURAH_S_SWEETHEART_PUMP]) { (void) mongets(mtmp, SEXY_LEATHER_PUMP); (void) mongets(mtmp, FEMININE_PUMPS); }

		if (ptr == &mons[PM_LAVA_DRAGON]) { (void) mongets(mtmp, SCR_LAVA); (void) mongets(mtmp, SCR_LAVA); (void) mongets(mtmp, SCR_LAVA);
		}
		/* according to jonadab the lava dragon should turn floor tiles into lava with its breath, but
		   since adding new breath types is difficult, let's just give them a few scrolls of lava. --Amy */

		if (ptr == &mons[PM_CHINESE_WOMAN]) {(void) mongets(mtmp, WOODEN_GETA);
		(void) mongets(mtmp, WEDGE_SANDALS);
		}

		if (ptr == &mons[PM_BARROWGRIM_SOLDIER]) { (void)mongets(mtmp, BOW); (void)mongets(mtmp, DAGGER);
			 m_initthrow(mtmp, ARROW, 10);

		}

		if (ptr == &mons[PM_DEMONIC_BLADEWITCH]) { (void)mongets(mtmp, BOW); (void)mongets(mtmp, WEDGE_SANDALS); (void)mongets(mtmp, SHORT_SWORD); m_initthrow(mtmp, ARROW, 50);
		}

		if (ptr == &mons[PM_MARTIAN_INHABITANT]) { (void)mongets(mtmp, SLING); (void)mongets(mtmp, LOADSTONE);

		}

		if (ptr == &mons[PM_ROMULANIAN_SOLDIER]) {

			  (void) mongets(mtmp, SUBMACHINE_GUN);
		  	  m_initthrow(mtmp, BULLET, 25);
		  	  m_initthrow(mtmp, BULLET, 25);
			  (void) mongets(mtmp, DAGGER);
			  (void) mongets(mtmp, FRAG_GRENADE);

		}

		if (ptr == &mons[PM_BUFFALO_SOLDIER]) {

			  (void) mongets(mtmp, ASSAULT_RIFLE);
		  	  m_initthrow(mtmp, BULLET, 25);
		  	  m_initthrow(mtmp, BULLET, 25);
			  (void) mongets(mtmp, DAGGER);
			  (void) mongets(mtmp, FRAG_GRENADE);

		}

		break;

	    default:
		break;
	}

	if ((int) mtmp->m_lev > rn2(400))
		(void) mongets(mtmp, rnd_defensive_item(mtmp));
	if ((int) mtmp->m_lev > rn2(400))
		(void) mongets(mtmp, rnd_misc_item(mtmp));
	if ((int) mtmp->m_lev > rn2(1000))
		(void) mongets(mtmp, rnd_offensive_item(mtmp));
	if (!rn2(1200)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
	if (!rn2(1200)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
	if (!rn2(3000)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));

	/* inventory initialization for bosses --Amy */

	if (ptr == &mons[PM_CRISTI]) {
		(void) mongets(mtmp, QUARTERSTAFF);
		(void) mongets(mtmp, WAN_GAIN_LEVEL);
	}

	if (ptr == &mons[PM_ROBAT]) {
		(void) mongets(mtmp, ASSAULT_RIFLE);
		(void) mongets(mtmp, KNIFE);
		m_initthrow(mtmp, BULLET, 50);
		m_initthrow(mtmp, BULLET, 50);

	}

	if (ptr == &mons[PM_TOBI]) {
		(void) mongets(mtmp, SILVER_MACE);
		(void) mongets(mtmp, ROBE);
		(void) mongets(mtmp, SCR_SUMMON_ELM);

	}

	if (ptr == &mons[PM_FLO_OOO]) {
		(void) mongets(mtmp, ELECTRIC_SWORD);
		m_initthrow(mtmp, DART, 50);
		(void) mongets(mtmp, GAUNTLETS_OF_TYPING);
		(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		(void) mongets(mtmp, WAN_BUGGING);

	}

	if (ptr == &mons[PM_CONSE]) {
		(void) mongets(mtmp, PISTOL);
		(void) mongets(mtmp, IRON_SHOES);
		m_initthrow(mtmp, BULLET, 50);

	}

	if (ptr == &mons[PM_PETE]) {
		(void) mongets(mtmp, PISTOL);
		m_initthrow(mtmp, BULLET, 50);
		(void) mongets(mtmp, AMULET_OF_LIFE_SAVING);
		(void) mongets(mtmp, STRIPED_SHIRT);

	}

	if (ptr == &mons[PM_ALEX]) {
		(void) mongets(mtmp, WAN_FEAR);
		(void) mongets(mtmp, WAN_TELEPORTATION);
		(void) mongets(mtmp, SCR_GROUP_SUMMONING);

	}

	if (ptr == &mons[PM_YVONNE]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_MAGDALENA]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, POT_FIRE);
		(void) mongets(mtmp, RING_MAIL);
		(void) mongets(mtmp, COMBAT_STILETTOS);

	}

	if (ptr == &mons[PM_EVELINE]) {
		(void) mongets(mtmp, WEDGE_SANDALS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, VICTORIAN_UNDERWEAR);
		(void) mongets(mtmp, WAN_TIME);
		(void) mongets(mtmp, WAN_INERTIA);

	}

	if (ptr == &mons[PM_BEA]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, POT_URINE);

	}

	if (ptr == &mons[PM_MAILIE]) {
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SCR_RELOCATION);

	}

	if (ptr == &mons[PM_JULCHEN]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, POT_HEALING);

	}

	if (ptr == &mons[PM_SABRINA]) {
		(void) mongets(mtmp, WEDGE_SANDALS);
		(void) mongets(mtmp, MACE);
		m_initthrow(mtmp, CREAM_PIE, 5);

	}

	if (ptr == &mons[PM_SABINE]) {
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, WEDGE_SANDALS);
		(void) mongets(mtmp, CLOAK_OF_MAGIC_RESISTANCE);
		(void) mongets(mtmp, SCR_IMMOBILITY);

	}

	if (ptr == &mons[PM_EGAS]) {
		(void) mongets(mtmp, SUBMACHINE_GUN);
		(void) mongets(mtmp, SCR_MESSAGE);
		m_initthrow(mtmp, BULLET, 25);

	}

	if (ptr == &mons[PM_HANNES]) {
		(void) mongets(mtmp, PISTOL);
		(void) mongets(mtmp, LEATHER_ARMOR);
		(void) mongets(mtmp, HELMET);
		(void) mongets(mtmp, WAN_STUN_MONSTER);
		(void) mongets(mtmp, IRON_SHOES);
		m_initthrow(mtmp, BULLET, 50);

	}

	if (ptr == &mons[PM_KATZOU]) {
		(void) mongets(mtmp, POT_AMNESIA);
		(void) mongets(mtmp, POT_CANCELLATION);
		(void) mongets(mtmp, POT_CYANIDE);
		(void) mongets(mtmp, POT_RADIUM);
		(void) mongets(mtmp, POT_SLIME);
		(void) mongets(mtmp, POT_FULL_HEALING);
		(void) mongets(mtmp, POT_INVISIBILITY);
		(void) mongets(mtmp, WAN_POISON);

	}

	if (ptr == &mons[PM_BASCHDI_DEATHSTRIKE]) {
		(void) mongets(mtmp, POT_INVISIBILITY);
		(void) mongets(mtmp, ROCKET_LAUNCHER);
		(void) mongets(mtmp, POT_RADIUM);
		(void) mongets(mtmp, PLATE_MAIL);
		m_initthrow(mtmp, ROCKET, 5);

	}

	if (ptr == &mons[PM_MARKUS]) {
		(void) mongets(mtmp, CLUB);
		(void) mongets(mtmp, T_SHIRT);

	}

	if (ptr == &mons[PM_MARTIN]) {
		(void) mongets(mtmp, GAS_GRENADE);
		(void) mongets(mtmp, RIFLE);
		(void) mongets(mtmp, LOADSTONE);
		(void) mongets(mtmp, SCR_BAD_EFFECT);
		(void) mongets(mtmp, HELM_OF_OPPOSITE_ALIGNMENT);
		(void) mongets(mtmp, VULNERABILITY_CLOAK);
		(void) mongets(mtmp, CLAY_CHAIN);
		(void) mongets(mtmp, SCR_RUMOR);
		(void) mongets(mtmp, SCR_RUMOR);
		m_initthrow(mtmp, CROSSBOW_BOLT, 15);

	}

	if (ptr == &mons[PM_CORINA]) {
		(void) mongets(mtmp, SPIKED_BATTLE_BOOT);
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, STUDDED_LEATHER_ARMOR);
		(void) mongets(mtmp, SCR_CREATE_VICTIM);
		(void) mongets(mtmp, WAN_FINGER_BENDING);

	}

	if (ptr == &mons[PM_ELIF]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, VICTORIAN_UNDERWEAR);

	}

	if (ptr == &mons[PM_SUNALI]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SCR_SIN);
		(void) mongets(mtmp, LEATHER_PEEP_TOES);
		m_initthrow(mtmp, GAS_GRENADE, 5);

	}

	if (ptr == &mons[PM_RHEA]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, WAN_FINGER_BENDING);
		(void) mongets(mtmp, SCR_CREATE_TRAP);
		m_initthrow(mtmp, DART, 50);
		m_initthrow(mtmp, DART, 50);

	}

	if (ptr == &mons[PM_JESSICA]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, SCR_FLOODING);
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, BAG_OF_TRICKS);
		(void) mongets(mtmp, LEATHER_CLOAK);

	}

	if (ptr == &mons[PM_SOLVEJG]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, WAN_EGOISM);
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);

	}

	if (ptr == &mons[PM_THAI]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, SCR_DESTROY_WEAPON);
		(void) mongets(mtmp, SCR_SIN);

	}

	if (ptr == &mons[PM_NADJA]) {
		(void) mongets(mtmp, SCR_TRAP_CREATION);
		(void) mongets(mtmp, SCR_TELE_LEVEL);
		(void) mongets(mtmp, LACQUERED_DANCING_SHOE);
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, SCR_ENRAGE);
		(void) mongets(mtmp, WAN_FINGER_BENDING);

	}

	if (ptr == &mons[PM_JANNIK]) {
		(void) mongets(mtmp, SUBMACHINE_GUN);
		(void) mongets(mtmp, BULLWHIP);
		(void) mongets(mtmp, SCR_RELOCATION);
		(void) mongets(mtmp, SCR_RELOCATION);
		m_initthrow(mtmp, BULLET, 25);

	}

	if (ptr == &mons[PM_URBALDI]) {
		(void) mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
		(void) mongets(mtmp, WEDGE_SANDALS);
		(void) mongets(mtmp, SNIPER_RIFLE);
		m_initthrow(mtmp, BULLET, 10);

	}

	if (ptr == &mons[PM_H_]) {
		(void) mongets(mtmp, ATHAME);
		(void) mongets(mtmp, WAN_DRAIN_MANA);
		(void) mongets(mtmp, RIFLE);
		(void) mongets(mtmp, SCR_DESTROY_ARMOR);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		m_initthrow(mtmp, BULLET, 50);

	}

	if (ptr == &mons[PM_PAUL]) {
		(void) mongets(mtmp, KNIFE);
		(void) mongets(mtmp, BULLWHIP);
		(void) mongets(mtmp, LEATHER_JACKET);

	}

	if (ptr == &mons[PM_DAVID]) {
		(void) mongets(mtmp, ASSAULT_RIFLE);
		(void) mongets(mtmp, VOULGE);
		(void) mongets(mtmp, LEATHER_JACKET);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_BARRHING);
		(void) mongets(mtmp, SCR_GROUP_SUMMONING);
		(void) mongets(mtmp, SCR_POWER_HEALING);
		m_initthrow(mtmp, BULLET, 50);

	}

	if (ptr == &mons[PM_RITA]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, WAN_TRAP_CREATION);
		(void) mongets(mtmp, WAN_CREATE_HORDE);
		(void) mongets(mtmp, SCR_ANTIMATTER);

	}

	if (ptr == &mons[PM_KATI]) {
		(void) mongets(mtmp, FEMININE_PUMPS);

	}

	if (ptr == &mons[PM_JOHANETTA]) {
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, SILVER_DRAGON_SCALES);
		(void) mongets(mtmp, DANCING_SHOES);

	}

	if (ptr == &mons[PM_KLARA]) {
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, WAN_BAD_LUCK);
		(void) mongets(mtmp, WAN_TIDAL_WAVE);

	}

	if (ptr == &mons[PM_SANDRA]) {
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, SPIKED_BATTLE_BOOT);
		(void) mongets(mtmp, SCALE_MAIL);
		(void) mongets(mtmp, LEATHER_CLOAK);
		(void) mongets(mtmp, WAN_DISINTEGRATION_BEAM);

	}

	if (ptr == &mons[PM_MELTEM]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, HIPPIE_HEELS);

	}

	if (ptr == &mons[PM_LARISSA]) {
		(void) mongets(mtmp, SCR_TELEPORTATION);
		(void) mongets(mtmp, SCR_TELEPORTATION);
		(void) mongets(mtmp, SCR_CREATE_TRAP);
		(void) mongets(mtmp, WAN_ACID);
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);

	}

	if (ptr == &mons[PM_SUESCHEN]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, SHOTGUN);
		(void) mongets(mtmp, SCR_SUMMON_ELM);
		m_initthrow(mtmp, SHOTGUN_SHELL, 10);

	}

	if (ptr == &mons[PM_DOCTOR_MAEX]) {
		(void) mongets(mtmp, POT_CYANIDE);
		(void) mongets(mtmp, POT_CYANIDE);
		(void) mongets(mtmp, POT_CYANIDE);
		(void) mongets(mtmp, POT_RADIUM);
		(void) mongets(mtmp, POT_RADIUM);
		(void) mongets(mtmp, POT_RADIUM);
		(void) mongets(mtmp, ORCISH_BOW);
		(void) mongets(mtmp, WAN_POISON);
		m_initthrow(mtmp, ORCISH_ARROW, 50);
		m_initthrow(mtmp, ORCISH_ARROW, 50);
		m_initthrow(mtmp, ORCISH_ARROW, 50);
		m_initthrow(mtmp, ORCISH_ARROW, 50);

	}

	if (ptr == &mons[PM_MARC_THE_SPACEWARS_FIGHTER]) {
		(void) mongets(mtmp, DAGGER);
		(void) mongets(mtmp, LEATHER_ARMOR);
		(void) mongets(mtmp, WAN_FIREBALL);
		(void) mongets(mtmp, WAN_SLEEP);

	}

	if (ptr == &mons[PM_ARNE]) {
		(void) mongets(mtmp, POT_SPEED);
		(void) mongets(mtmp, IRON_CHAIN);
		(void) mongets(mtmp, LOW_BOOTS);
		(void) mongets(mtmp, LEATHER_JACKET);
		(void) mongets(mtmp, SCR_DESTROY_WEAPON);

	}

	if (ptr == &mons[PM_HAILBUSH]) {
		(void) mongets(mtmp, SCR_GROWTH);
		(void) mongets(mtmp, SCR_GROWTH);
		(void) mongets(mtmp, SCR_GROWTH);
		(void) mongets(mtmp, SCR_GROWTH);
		(void) mongets(mtmp, SCR_GROWTH);
		(void) mongets(mtmp, CLOAK_OF_REFLECTION);

	}

	if (ptr == &mons[PM_ROMANN]) {
		(void) mongets(mtmp, MALLET);
		(void) mongets(mtmp, SCR_CREATE_VICTIM);

	}

	if (ptr == &mons[PM_SIEGFRIED]) {
		(void) mongets(mtmp, STEEL_WHIP);
		(void) mongets(mtmp, HEAVY_MACHINE_GUN);
		m_initthrow(mtmp, BULLET, 50);

	}

	if (ptr == &mons[PM_ROY]) {
		(void) mongets(mtmp, STEEL_WHIP);
		(void) mongets(mtmp, HEAVY_MACHINE_GUN);
		m_initthrow(mtmp, BULLET, 50);

	}

	if (ptr == &mons[PM_ANTONIA]) {
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, FLY_SWATTER);
		(void) mongets(mtmp, LAB_COAT);

	}

	if (ptr == &mons[PM_CHASKA]) {
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, LEATHER_JACKET);
		(void) mongets(mtmp, POT_SPEED);

	}

	if (ptr == &mons[PM_LUDGERA]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, SCR_MEGALOAD);

	}

	if (ptr == &mons[PM_LAURA]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SCR_FLOOD);
		(void) mongets(mtmp, SCR_FLOODING);
		(void) mongets(mtmp, SCR_ICE);
		(void) mongets(mtmp, POT_NUMBNESS);
		(void) mongets(mtmp, POT_STUNNING);
		(void) mongets(mtmp, WAN_CORROSION);
		(void) mongets(mtmp, SILVER_DRAGON_SCALE_MAIL);
		(void) mongets(mtmp, TEMPEST_HORN);

	}

	if (ptr == &mons[PM_EVA]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, SILVER_SPEAR);
		(void) mongets(mtmp, JAVELIN);
		(void) mongets(mtmp, SCR_TRAP_CREATION);
		m_initthrow(mtmp, SPEAR, 10);

	}

	if (ptr == &mons[PM_MAURAH]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, WAN_POISON);
		(void) mongets(mtmp, SCR_BAD_EFFECT);
		(void) mongets(mtmp, SCR_SUMMON_GHOST);
		m_initthrow(mtmp, GAS_GRENADE, 5);

	}

	if (ptr == &mons[PM_SOPHIE]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_MARIAN]) {
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, WAN_POISON);

	}

	if (ptr == &mons[PM_ARABELLA_THE_TUFTED_ASIAN_GIRL]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, WAN_BANISHMENT);
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, SCR_WARPING);

	}

	if (ptr == &mons[PM_ANASTASIA_THE_SWEET_BLONDE]) {
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);

	}

	if (ptr == &mons[PM_HENRIETTA_THE_BURLY_WOMAN]) {
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, SCR_TRAP_CREATION);
		(void) mongets(mtmp, SCR_TRAP_CREATION);

	}

	if (ptr == &mons[PM_KATRIN_THE_VIOLET_BEAUTY]) {
		(void) mongets(mtmp, POT_PARALYSIS);
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, SCR_CREATE_TRAP);
		(void) mongets(mtmp, SCR_MEGALOAD);
		(void) mongets(mtmp, WAN_IMMOBILITY);

	}

	if (ptr == &mons[PM_JANA_THE_SOFT_WENCH]) {
		(void) mongets(mtmp, POT_URINE);
		(void) mongets(mtmp, SCR_BARRHING);
		(void) mongets(mtmp, SCR_RELOCATION);
		(void) mongets(mtmp, SCR_RELOCATION);
		(void) mongets(mtmp, SCR_RELOCATION);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, WAN_SIN);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_JIL]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_PATRICIA]) {
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, HIPPIE_HEELS);

	}

	if (ptr == &mons[PM_KERSTIN]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, BLUE_DRAGON_SCALE_MAIL);
		(void) mongets(mtmp, SCR_POWER_HEALING);

	}

	if (ptr == &mons[PM_SALLY]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, DAGGER);

	}

	if (ptr == &mons[PM_HANH]) {
		(void) mongets(mtmp, WEDGE_SANDALS);
		(void) mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
		(void) mongets(mtmp, POT_FULL_HEALING);

	}

	if (ptr == &mons[PM_ANTJE]) {
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, SCR_BULLSHIT);
		(void) mongets(mtmp, SCR_CREATE_VICTIM);

	}

	if (ptr == &mons[PM_JENNIFER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

	}

	if (ptr == &mons[PM_ELKE]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, POT_HEALING);
		(void) mongets(mtmp, POT_HEALING);

	}

	if (ptr == &mons[PM_KARIN]) {
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, WAN_DRAINING);
		(void) mongets(mtmp, WAN_STUN_MONSTER);
		(void) mongets(mtmp, SCR_TRAP_CREATION);

	}

	if (ptr == &mons[PM_NICOLE]) {
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, ELVEN_BOW);
		(void) mongets(mtmp, TEMPEST_HORN);
		m_initthrow(mtmp, ELVEN_ARROW, 50);

	}

	if (ptr == &mons[PM_BETTINA]) {
		(void) mongets(mtmp, SPEED_BOOTS);
		(void) mongets(mtmp, ATHAME);
		(void) mongets(mtmp, T_SHIRT);

	}

	if (ptr == &mons[PM_INA]) {
		(void) mongets(mtmp, WAN_STARVATION);
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, WAN_EXTRA_HEALING);

	}

	if (ptr == &mons[PM_HEIKE]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SPEED_BOOTS);
		(void) mongets(mtmp, PISTOL);
		(void) mongets(mtmp, SCR_SIN);
		(void) mongets(mtmp, SCR_DESTROY_WEAPON);
		m_initthrow(mtmp, BULLET, 20);

	}

	if (ptr == &mons[PM_DORA]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, BULLWHIP);
		(void) mongets(mtmp, POT_GAIN_LEVEL);
		(void) mongets(mtmp, POT_GAIN_LEVEL);
		(void) mongets(mtmp, POT_HEALING);
		(void) mongets(mtmp, SCR_CREATE_TRAP);

	}

	if (ptr == &mons[PM_MAITE]) {
		(void) mongets(mtmp, POT_CYANIDE);
		(void) mongets(mtmp, WAN_INERTIA);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, HIPPIE_HEELS);

	}

	if (ptr == &mons[PM_RUTH]) {
		(void) mongets(mtmp, POT_RADIUM);
		(void) mongets(mtmp, POT_RADIUM);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);

	}

	if (ptr == &mons[PM_VERENA]) {
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, SCR_TELEPORTATION);
		(void) mongets(mtmp, SCR_POWER_HEALING);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);

	}

	if (ptr == &mons[PM_LOU]) {
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, WAN_POISON);
		(void) mongets(mtmp, POT_EXTRA_HEALING);
		(void) mongets(mtmp, POT_EXTRA_HEALING);
		(void) mongets(mtmp, POT_EXTRA_HEALING);

	}

	if (ptr == &mons[PM_DANIELLE]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, WAN_CANCELLATION);
		(void) mongets(mtmp, WAN_TIME);
		(void) mongets(mtmp, RIFLE);
		m_initthrow(mtmp, BULLET, 50);

	}

	if (ptr == &mons[PM_AMANDINE]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, SCR_MESSAGE);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_LILY]) {
		(void) mongets(mtmp, ORCISH_DAGGER);

	}

	if (ptr == &mons[PM_LEONIE]) {
		(void) mongets(mtmp, SHORT_SWORD);

	}

	if (ptr == &mons[PM_MIRA]) {
		(void) mongets(mtmp, POT_HEALING);
		(void) mongets(mtmp, POT_HEALING);
		(void) mongets(mtmp, POT_HEALING);
		(void) mongets(mtmp, POT_HEALING);
		(void) mongets(mtmp, POT_HEALING);
		(void) mongets(mtmp, POT_EXTRA_HEALING);
		(void) mongets(mtmp, POT_EXTRA_HEALING);
		(void) mongets(mtmp, POT_EXTRA_HEALING);
		(void) mongets(mtmp, POT_FULL_HEALING);
		(void) mongets(mtmp, WAN_INCREASE_MAX_HITPOINTS);
	}

	if (ptr == &mons[PM_SOFIA]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, SCR_EGOISM);

	}

	if (ptr == &mons[PM_CHRISTIANE]) {
		(void) mongets(mtmp, SCR_BAD_EFFECT);
		(void) mongets(mtmp, WAN_BAD_EFFECT);
		(void) mongets(mtmp, SCR_TELEPORTATION);

	}

	if (ptr == &mons[PM_ANN_KATHRIN]) {
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, LACQUERED_DANCING_SHOE);
		(void) mongets(mtmp, SCR_TELEPORTATION);

	}

	if (ptr == &mons[PM_NJUSHA]) {
		(void) mongets(mtmp, WOODEN_GETA);
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, WAN_CONFUSION);

	}

	if (ptr == &mons[PM_DESIREE]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, KNIFE);
		(void) mongets(mtmp, POT_HEALING);
		(void) mongets(mtmp, WAN_CREATE_MONSTER);

	}

	if (ptr == &mons[PM_CELIA]) {
		(void) mongets(mtmp, SCR_BULLSHIT);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, WAN_POISON);

	}

	if (ptr == &mons[PM_ELIANE]) {
		(void) mongets(mtmp, WAN_PARALYSIS);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, POT_PARALYSIS);
		(void) mongets(mtmp, POT_PARALYSIS);
		(void) mongets(mtmp, POT_PARALYSIS);
		(void) mongets(mtmp, SCR_POWER_HEALING);
		(void) mongets(mtmp, SCR_RELOCATION);
	}

	if (ptr == &mons[PM_FENJA]) {
		(void) mongets(mtmp, WAN_DISINTEGRATION);
		(void) mongets(mtmp, WAN_STONING);
		(void) mongets(mtmp, WAN_STUN_MONSTER);
		(void) mongets(mtmp, WAN_FULL_HEALING);
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, WOODEN_GETA);
		(void) mongets(mtmp, HEAVY_MACHINE_GUN);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		m_initthrow(mtmp, BULLET, 50);
		m_initthrow(mtmp, BULLET, 50);
		m_initthrow(mtmp, BULLET, 50);
		m_initthrow(mtmp, BULLET, 50);
	}

	if (ptr == &mons[PM_SILVANA]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SCALPEL);
		(void) mongets(mtmp, SCR_EARTH);
		(void) mongets(mtmp, SCR_FIRE);
	}

	if (ptr == &mons[PM_MANDARINA]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, CRYSKNIFE);
		(void) mongets(mtmp, STUDDED_LEATHER_ARMOR);
		(void) mongets(mtmp, SCR_HEALING);
		(void) mongets(mtmp, SCR_HEALING);
		(void) mongets(mtmp, SCR_HEALING);
		(void) mongets(mtmp, SCR_HEALING);
		(void) mongets(mtmp, SCR_HEALING);

	}

	if (ptr == &mons[PM_ARIANE]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, WAN_REDUCE_MAX_HITPOINTS);
		(void) mongets(mtmp, WAN_STRIKING);
		(void) mongets(mtmp, WAN_CHROMATIC_BEAM);

	}

	if (ptr == &mons[PM_CARINA]) {
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, POT_PARALYSIS);
		(void) mongets(mtmp, LEATHER_JACKET);

	}

	if (ptr == &mons[PM_DENISE]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);

	}

	if (ptr == &mons[PM_KIRA]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, WAN_HEALING);

	}

	if (ptr == &mons[PM_NADINE]) {
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, WEDGE_SANDALS);
		(void) mongets(mtmp, SILVER_DRAGON_SCALES);
		(void) mongets(mtmp, WAN_ACID);
		(void) mongets(mtmp, WAN_SUMMON_ELM);
		(void) mongets(mtmp, SCR_TELEPORTATION);

	}

	if (ptr == &mons[PM_RONJA]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, WAN_STUN_MONSTER);
		(void) mongets(mtmp, POT_EXTRA_HEALING);
		(void) mongets(mtmp, LEATHER_JACKET);
		(void) mongets(mtmp, CLOAK_OF_MAGIC_RESISTANCE);

	}

	if (ptr == &mons[PM_ELISABETH]) {
		(void) mongets(mtmp, SCR_FLOOD);
		(void) mongets(mtmp, SCR_FLOODING);

	}

	if (ptr == &mons[PM_CONNY]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, SCR_BAD_EFFECT);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, SCR_CREATE_TRAP);
		m_initthrow(mtmp, DART, 50);

	}

	if (ptr == &mons[PM_LISA]) {
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, WAN_LIGHTNING);
		(void) mongets(mtmp, SCR_ENRAGE);
		(void) mongets(mtmp, POT_BLINDNESS);

	}

	if (ptr == &mons[PM_ANJA]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);

	}

	if (ptr == &mons[PM_YASAMAN]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, LEATHER_CLOAK);
		(void) mongets(mtmp, SCR_SUMMON_GHOST);

	}

	if (ptr == &mons[PM_ALMUTH]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, WAN_CORROSION);

	}

	if (ptr == &mons[PM_KSENIA]) {
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, SCR_HEALING);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, POT_FEAR);
		(void) mongets(mtmp, POT_SPEED);
		(void) mongets(mtmp, WAN_TELE_LEVEL);
		(void) mongets(mtmp, WAN_CHROMATIC_BEAM);

	}

	if (ptr == &mons[PM_MIRIAM]) {
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, POT_SLEEPING);
		(void) mongets(mtmp, POT_STUNNING);

	}

	if (ptr == &mons[PM_ELENA]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, WEDGE_SANDALS);
		(void) mongets(mtmp, POT_CONFUSION);
		(void) mongets(mtmp, POT_HALLUCINATION);
		(void) mongets(mtmp, SCR_CHAOS_TERRAIN);
		(void) mongets(mtmp, WAN_FULL_HEALING);
		(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
	}

	if (ptr == &mons[PM_KATHARINA]) {
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, POT_CANCELLATION);
		(void) mongets(mtmp, POT_AMNESIA);
		(void) mongets(mtmp, SCR_SUMMON_UNDEAD);
		(void) mongets(mtmp, WAN_FULL_HEALING);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
	}

	if (ptr == &mons[PM_HELEN]) {
		(void) mongets(mtmp, WAN_BAD_EFFECT);
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, SCR_RELOCATION);
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		m_initthrow(mtmp, DART, 30);
		m_initthrow(mtmp, SHURIKEN, 5);

	}

	if (ptr == &mons[PM_LUCIA]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, WAR_HAMMER);

	}

	if (ptr == &mons[PM_VICTORIA]) {
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, SCR_PUNISHMENT);
		(void) mongets(mtmp, SCR_SIN);
		(void) mongets(mtmp, WAN_REMOVE_RESISTANCE);

	}

	if (ptr == &mons[PM_SIMONE]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, WAN_LYCANTHROPY);
		(void) mongets(mtmp, POT_FIRE);
		(void) mongets(mtmp, POT_ICE);
		(void) mongets(mtmp, POT_FEAR);
		(void) mongets(mtmp, SCR_DESTROY_WEAPON);
		(void) mongets(mtmp, DANCING_SHOES);

	}

	if (ptr == &mons[PM_NATALIYA]) {
		(void) mongets(mtmp, WEDGE_SANDALS);
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, POT_BLINDNESS);
		(void) mongets(mtmp, SCR_CLOUDS);
	}

	if (ptr == &mons[PM_KRISTIN]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SCR_GROUP_SUMMONING);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, WAN_REMOVE_RESISTANCE);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SCR_CREATE_VICTIM);
		(void) mongets(mtmp, SCR_CREATE_VICTIM);
		(void) mongets(mtmp, HIPPIE_HEELS);

	}

	if (ptr == &mons[PM_NELLY]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, WAN_GAIN_LEVEL);

	}

	if (ptr == &mons[PM_REJDA]) {
		(void) mongets(mtmp, ASSAULT_RIFLE);
		(void) mongets(mtmp, POT_SPEED);
		(void) mongets(mtmp, WAN_DRAIN_MANA);
		(void) mongets(mtmp, POT_INVISIBILITY);
		m_initthrow(mtmp, BULLET, 50);
		m_initthrow(mtmp, BULLET, 50);

	}

	if (ptr == &mons[PM_VERA]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, WAN_COLD);
		(void) mongets(mtmp, POT_ICE);
		(void) mongets(mtmp, SCR_ICE);
		(void) mongets(mtmp, POT_ICE);
		(void) mongets(mtmp, SCR_ICE);
		(void) mongets(mtmp, POT_ICE);
		(void) mongets(mtmp, SCR_ICE);
		(void) mongets(mtmp, WHITE_DRAGON_SCALES);
		(void) mongets(mtmp, CLOAK_OF_WARMTH);
		(void) mongets(mtmp, ICE_SHIELD);
		(void) mongets(mtmp, WAN_INCREASE_MAX_HITPOINTS);
	}

	if (ptr == &mons[PM_NORA]) {
		(void) mongets(mtmp, POT_SPEED);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, POT_GAIN_LEVEL);
		(void) mongets(mtmp, POT_MUTATION);
		(void) mongets(mtmp, WAN_CREATE_HORDE);
	}

	if (ptr == &mons[PM_JASIEEN]) {
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, AMULET_OF_LIFE_SAVING);
		(void) mongets(mtmp, CLOAK_OF_REFLECTION);
		(void) mongets(mtmp, BLACK_DRAGON_SCALE_MAIL);
		(void) mongets(mtmp, HELM_OF_STEEL);
		(void) mongets(mtmp, GAUNTLETS_OF_STEEL);
		(void) mongets(mtmp, T_SHIRT);
		(void) mongets(mtmp, VENOM_SHIELD);
	}

	if (ptr == &mons[PM_MARIKE]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, POT_URINE);
		(void) mongets(mtmp, WAN_SLIMING);
	}

	if (ptr == &mons[PM_MERLE]) {
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_MARIANNE]) {
		(void) mongets(mtmp, SCR_WOUNDS);

	}

	if (ptr == &mons[PM_SINA]) {
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, CLUB);
		(void) mongets(mtmp, POT_ACID);

	}

	if (ptr == &mons[PM_DOROTHEA]) {
		(void) mongets(mtmp, RUBBER_HOSE);

	}

	if (ptr == &mons[PM_TINANYA]) {
		(void) mongets(mtmp, AMULET_OF_LIFE_SAVING);
		(void) mongets(mtmp, LEATHER_PEEP_TOES);
		(void) mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
		(void) mongets(mtmp, SPEAR);
	}

	if (ptr == &mons[PM_ALINE]) {
		(void) mongets(mtmp, T_SHIRT);
		(void) mongets(mtmp, RING_MAIL);
		(void) mongets(mtmp, MACE);

	}

	if (ptr == &mons[PM_MICHAELA]) {
		(void) mongets(mtmp, LARGE_SHIELD);
		(void) mongets(mtmp, SCIMITAR);
		(void) mongets(mtmp, UNICORN_HORN);

	}

	if (ptr == &mons[PM_MARE]) {
		(void) mongets(mtmp, TORPEDO);
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, WAN_SPEED_MONSTER);
		(void) mongets(mtmp, LEATHER_ARMOR);
		(void) mongets(mtmp, POT_STUNNING);
		(void) mongets(mtmp, SCR_SUMMON_GHOST);
		m_initthrow(mtmp, SPIRIT_THROWER, 5);
	}

	if (ptr == &mons[PM_NOEMI]) {
		(void) mongets(mtmp, POT_URINE);
		(void) mongets(mtmp, SCR_ICE);
		(void) mongets(mtmp, WAN_AMNESIA);
		(void) mongets(mtmp, VICTORIAN_UNDERWEAR);

	}

	if (ptr == &mons[PM_GIULIA]) {
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, LACQUERED_DANCING_SHOE);
		(void) mongets(mtmp, POT_FIRE);
		(void) mongets(mtmp, WAN_CREATE_HORDE);
	}

	if (ptr == &mons[PM_LEA]) {
		(void) mongets(mtmp, SCR_TELEPORTATION);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, KNIFE);

	}

	if (ptr == &mons[PM_SARAH]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, SCR_BAD_EFFECT);

	}

	if (ptr == &mons[PM_SARINA]) {
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SCR_ICE);
		(void) mongets(mtmp, POT_HEALING);
	}

	if (ptr == &mons[PM_ALEXIA]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);	
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);	
		(void) mongets(mtmp, WAN_BAD_EFFECT);	
		(void) mongets(mtmp, WAN_FINGER_BENDING);	

	}

	if (ptr == &mons[PM_ISIS]) {
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);
		(void) mongets(mtmp, POT_FULL_HEALING);
		(void) mongets(mtmp, SCR_WARPING);
		(void) mongets(mtmp, WAN_DIGGING);
	}

	if (ptr == &mons[PM_ILSE]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, CLOAK_OF_PROTECTION);
		(void) mongets(mtmp, SCR_HEALING);
	}

	if (ptr == &mons[PM_MELANIE]) {
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, SCR_WOUNDS);
		(void) mongets(mtmp, SPIKED_BATTLE_BOOT);
	}

	if (ptr == &mons[PM_LARA]) {
		(void) mongets(mtmp, SCR_BAD_EFFECT);
		(void) mongets(mtmp, POT_SPEED);

	}

	if (ptr == &mons[PM_JANINA]) {
		(void) mongets(mtmp, SPIKED_BATTLE_BOOT);
		(void) mongets(mtmp, COMBAT_STILETTOS);

	}

	if (ptr == &mons[PM_MICHA]) {
		(void) mongets(mtmp, RUBBER_HOSE);
		m_initthrow(mtmp, CREAM_PIE, 4);

	}

	if (ptr == &mons[PM_FRANZI]) {
		(void) mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		(void) mongets(mtmp, SCR_FLOOD);
		(void) mongets(mtmp, SCR_RUMOR);
	}

	if (ptr == &mons[PM_AMELIE]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, SCR_CREATE_TRAP);

	}

	if (ptr == &mons[PM_ANNIKA]) {
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, POT_URINE);
		(void) mongets(mtmp, SCR_GROWTH);
	}

	if (ptr == &mons[PM_BARBARA]) {
		(void) mongets(mtmp, WOODEN_GETA);
		(void) mongets(mtmp, WEDGE_SANDALS);
		(void) mongets(mtmp, WAN_LIGHTNING);
		(void) mongets(mtmp, POT_NUMBNESS);
		(void) mongets(mtmp, SCR_TELEPORTATION);
	}

	if (ptr == &mons[PM_ELSA]) {
		(void) mongets(mtmp, POT_CANCELLATION);

	}

	if (ptr == &mons[PM_ISABEL]) {
		(void) mongets(mtmp, POT_URINE);

	}

	if (ptr == &mons[PM_THUY_HAN]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SCR_BAD_EFFECT);

	}

	if (ptr == &mons[PM_CHARLOTTE]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, RIFLE);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		m_initthrow(mtmp, BULLET, 20);
	}

	if (ptr == &mons[PM_FRIEDERIKE]) {
		(void) mongets(mtmp, POT_ICE);
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SCR_SUMMON_BOSS);
		(void) mongets(mtmp, SCR_SUMMON_ELM);
		(void) mongets(mtmp, POT_AMNESIA);
	}

	if (ptr == &mons[PM_SOPHIA]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, SCR_DESTROY_ARMOR);
		(void) mongets(mtmp, SCR_IMMOBILITY);
		(void) mongets(mtmp, SCR_DESTROY_ARMOR);
		(void) mongets(mtmp, POT_FEAR);
	}

	if (ptr == &mons[PM_SUE_LYN]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, WAN_BAD_EFFECT);
		(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
		(void) mongets(mtmp, WAN_GAIN_LEVEL);
		(void) mongets(mtmp, WAN_TIDAL_WAVE);
	}

	if (ptr == &mons[PM_JUEN]) {
		(void) mongets(mtmp, LEATHER_PEEP_TOES);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, CLOAK_OF_DEATH);
		(void) mongets(mtmp, POT_VAMPIRE_BLOOD);
		(void) mongets(mtmp, SCR_PUNISHMENT);
	}

	if (ptr == &mons[PM_RUEA]) {
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);
		(void) mongets(mtmp, SCR_BULLSHIT);
		(void) mongets(mtmp, WAN_REMOVE_RESISTANCE);
	}

	if (ptr == &mons[PM_MARIYA]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, SCR_BAD_EFFECT);
		(void) mongets(mtmp, WAN_AMNESIA);
		(void) mongets(mtmp, WAN_SIN);
		(void) mongets(mtmp, POT_CONFUSION);
	}

	if (ptr == &mons[PM_WENDY_O__KOOPA]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, POT_NUMBNESS);
		(void) mongets(mtmp, SCR_TRAP_CREATION);
	}

	if (ptr == &mons[PM_KATIA]) {
		(void) mongets(mtmp, POT_GAIN_LEVEL);
		(void) mongets(mtmp, WAN_GAIN_LEVEL);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, SCR_BULLSHIT);
		(void) mongets(mtmp, SCR_MEGALOAD);
		(void) mongets(mtmp, POT_URINE);
	}

	if (ptr == &mons[PM_TANJA]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, POT_STUNNING);
		(void) mongets(mtmp, POT_CONFUSION);
	}

	if (ptr == &mons[PM_VANESSA]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, FLAIL);
		(void) mongets(mtmp, SCR_BARRHING);
		(void) mongets(mtmp, SCR_EGOISM);
		(void) mongets(mtmp, WAN_POISON);

	}

	if (ptr == &mons[PM_ANNE]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, WAN_SLIMING);
		(void) mongets(mtmp, SCR_GROUP_SUMMONING);
		(void) mongets(mtmp, SCR_DESTROY_WEAPON);

	}

	if (ptr == &mons[PM_LENA]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, WAN_STRIKING);
		(void) mongets(mtmp, T_SHIRT);

	}

	if (ptr == &mons[PM_JEANETTA]) {
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);
		(void) mongets(mtmp, WAN_SLOW_MONSTER);
		(void) mongets(mtmp, WAN_STUN_MONSTER);
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, SCR_CREATE_TRAP);
	}

	if (ptr == &mons[PM_GUDRUN]) {
		(void) mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, RIFLE);
		(void) mongets(mtmp, POT_ICE);
		(void) mongets(mtmp, SCR_TELEPORTATION);
		(void) mongets(mtmp, SCR_SUMMON_GHOST);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		m_initthrow(mtmp, BULLET, 50);
	}

	if (ptr == &mons[PM_MELISSA]) {
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, SCR_SUMMON_ELM);
		(void) mongets(mtmp, LEATHER_ARMOR);
		(void) mongets(mtmp, LEATHER_CLOAK);
		(void) mongets(mtmp, WAN_STRIKING);
		(void) mongets(mtmp, TEMPEST_HORN);
	}

	if (ptr == &mons[PM_ELLA]) {
		(void) mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
		(void) mongets(mtmp, WEDGE_SANDALS);
		(void) mongets(mtmp, WAN_BAD_EFFECT);
		(void) mongets(mtmp, PLASTEEL_HELM);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, WAN_SUMMON_UNDEAD);
		(void) mongets(mtmp, WAN_TIME);
		(void) mongets(mtmp, SCR_PUNISHMENT);
		(void) mongets(mtmp, SCR_POWER_HEALING);
		(void) mongets(mtmp, POT_AMNESIA);
	}

	if (ptr == &mons[PM_MADELEINE]) {
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);
		(void) mongets(mtmp, WEDGE_SANDALS);
		(void) mongets(mtmp, WAN_FINGER_BENDING);
		(void) mongets(mtmp, SCR_BULLSHIT);
	}

	if (ptr == &mons[PM_ANITA]) {
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, SCR_WOUNDS);
		(void) mongets(mtmp, SCR_WOUNDS);
		(void) mongets(mtmp, SCR_WOUNDS);
		(void) mongets(mtmp, SCR_WOUNDS);
		(void) mongets(mtmp, SCR_WOUNDS);
		(void) mongets(mtmp, SCR_ENRAGE);
		(void) mongets(mtmp, SCR_GROUP_SUMMONING);
	}

	if (ptr == &mons[PM_NINA]) {
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, STILETTO);
		(void) mongets(mtmp, WAN_STUN_MONSTER);
		(void) mongets(mtmp, SCR_SUMMON_BOSS);
		(void) mongets(mtmp, SCR_CREATE_VICTIM);

	}

	if (ptr == &mons[PM_NATASCHA]) {
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);
		(void) mongets(mtmp, LEATHER_GLOVES);
		(void) mongets(mtmp, SHIELD_OF_REFLECTION);
	}

	if (ptr == &mons[PM_MANUELA]) {
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, SPIKED_BATTLE_BOOT);
		(void) mongets(mtmp, WAN_CREATE_HORDE);
		(void) mongets(mtmp, DEEP_DRAGON_SCALE_MAIL);
		(void) mongets(mtmp, LAB_COAT);
		(void) mongets(mtmp, PLASTEEL_GLOVES);
		(void) mongets(mtmp, BUGLE);
		(void) mongets(mtmp, POT_POLYMORPH);
		(void) mongets(mtmp, WAN_PUNISHMENT);
		(void) mongets(mtmp, SCR_LAVA);
		(void) mongets(mtmp, SCR_STONING);
		(void) mongets(mtmp, SCR_ANTIMATTER);
		(void) mongets(mtmp, WAN_DISINTEGRATION);
		(void) mongets(mtmp, WAN_POISON);
	}

	if (ptr == &mons[PM_TILLA]) {
		(void) mongets(mtmp, GLAIVE);
		(void) mongets(mtmp, POT_BLINDNESS);
		(void) mongets(mtmp, POT_BLINDNESS);
		(void) mongets(mtmp, POT_BLINDNESS);
		m_initthrow(mtmp, CREAM_PIE, 5);
	}

	if (ptr == &mons[PM_GRETA]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, WAN_MAKE_VISIBLE);
		(void) mongets(mtmp, SCR_DESTROY_ARMOR);
		(void) mongets(mtmp, SCR_DESTROY_WEAPON);
		(void) mongets(mtmp, POT_RADIUM);
		(void) mongets(mtmp, SCR_SUMMON_GHOST);
	}

	if (ptr == &mons[PM_JANE]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, POT_FIRE);
		(void) mongets(mtmp, WAN_EGOISM);
		(void) mongets(mtmp, SCR_TRAP_CREATION);
		(void) mongets(mtmp, SCR_CREATE_TRAP);
		(void) mongets(mtmp, WAN_CREATE_MONSTER);
	}

	if (ptr == &mons[PM_NATALJE]) {
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, BAG_OF_TRICKS);
		(void) mongets(mtmp, POT_GAIN_LEVEL);
		(void) mongets(mtmp, POT_CYANIDE);
		(void) mongets(mtmp, WAN_CURSE_ITEMS);
		(void) mongets(mtmp, WAN_POLYMORPH);
		(void) mongets(mtmp, SCR_POWER_HEALING);
		(void) mongets(mtmp, SCR_ENRAGE);
		(void) mongets(mtmp, SCR_RELOCATION);
	}

	if (ptr == &mons[PM_LITTLE_MARIE]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SPEED_BOOTS);
		(void) mongets(mtmp, WAN_CREATE_HORDE);
		(void) mongets(mtmp, WAN_SUMMON_UNDEAD);
		(void) mongets(mtmp, SCR_WARPING);
		(void) mongets(mtmp, SCR_IMMOBILITY);
		(void) mongets(mtmp, SCR_SIN);
		(void) mongets(mtmp, SCR_GROUP_SUMMONING);
		(void) mongets(mtmp, POT_HALLUCINATION);
		(void) mongets(mtmp, SUBMACHINE_GUN);
		(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
		m_initthrow(mtmp, BULLET, 50);
		m_initthrow(mtmp, BULLET, 50);
	}

	if (ptr == &mons[PM_TONJA]) {
		(void) mongets(mtmp, SPIKED_BATTLE_BOOT);
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, WAN_DEATH);

	}

	if (ptr == &mons[PM_ANNEMARIE]) {
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, LEATHER_PEEP_TOES);
		(void) mongets(mtmp, ROCKET_LAUNCHER);
		(void) mongets(mtmp, POT_FEAR);
		(void) mongets(mtmp, SCR_HEALING);
		(void) mongets(mtmp, SCR_CHAOS_TERRAIN);
		(void) mongets(mtmp, WAN_FIREBALL);
		(void) mongets(mtmp, WAN_SUMMON_ELM);
		m_initthrow(mtmp, ROCKET, 5);
	}

	if (ptr == &mons[PM_CARO]) {
		(void) mongets(mtmp, WAN_FIRE);
		(void) mongets(mtmp, LEATHER_JACKET);

	}

	if (ptr == &mons[PM_BIANCA]) {
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, LEATHER_PEEP_TOES);
		(void) mongets(mtmp, SCR_EGOISM);
		(void) mongets(mtmp, WAN_EGOISM);
		(void) mongets(mtmp, WAN_SOLAR_BEAM);
		(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
		(void) mongets(mtmp, ORANGE_DRAGON_SCALES);
	}

	if (ptr == &mons[PM_CARMEN]) {
		(void) mongets(mtmp, SPEED_BOOTS);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, WAN_EXTRA_HEALING);
	}

	if (ptr == &mons[PM_HILDA]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, WAN_INERTIA);
		(void) mongets(mtmp, SCR_SUMMON_BOSS);

	}

	if (ptr == &mons[PM_JOHANNA]) {
		(void) mongets(mtmp, POT_CONFUSION);

	}

	if (ptr == &mons[PM_JULIETTA]) {
		(void) mongets(mtmp, LACQUERED_DANCING_SHOE);
		(void) mongets(mtmp, POT_AMNESIA);
		(void) mongets(mtmp, WAN_AMNESIA);
		(void) mongets(mtmp, WAN_DISINTEGRATION_BEAM);
		(void) mongets(mtmp, SCR_AMNESIA);
		(void) mongets(mtmp, SCR_WARPING);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
	}

	if (ptr == &mons[PM_LINDA]) {
		(void) mongets(mtmp, WAN_CREATE_HORDE);

	}

	if (ptr == &mons[PM_PETRA]) {
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, WAN_PARALYSIS);
		(void) mongets(mtmp, WAN_STUN_MONSTER);

	}

	if (ptr == &mons[PM_SONJA]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, SCR_SUMMON_BOSS);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, SCR_SUMMON_UNDEAD);
		(void) mongets(mtmp, LEATHER_JACKET);
		(void) mongets(mtmp, POT_POLYMORPH);
		(void) mongets(mtmp, SCR_MESSAGE);
	}

	if (ptr == &mons[PM_STELLA]) {
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, WAN_CREATE_MONSTER);
		(void) mongets(mtmp, POT_BLINDNESS);

	}

	if (ptr == &mons[PM_TERESA]) {
		(void) mongets(mtmp, POT_NUMBNESS);
		(void) mongets(mtmp, POT_NUMBNESS);
		(void) mongets(mtmp, POT_NUMBNESS);
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, LANCE);

	}

	if (ptr == &mons[PM_ROSY]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, POT_SPEED);
		(void) mongets(mtmp, SCR_CLOUDS);
		(void) mongets(mtmp, CREAM_PIE);
		(void) mongets(mtmp, POT_BLINDNESS);
	}

	if (ptr == &mons[PM_SING]) {
		(void) mongets(mtmp, POT_VAMPIRE_BLOOD);
		(void) mongets(mtmp, POT_VAMPIRE_BLOOD);
		(void) mongets(mtmp, POT_VAMPIRE_BLOOD);
		(void) mongets(mtmp, WOODEN_GETA);
		(void) mongets(mtmp, WEDGE_SANDALS);
		(void) mongets(mtmp, POT_PARALYSIS);
		(void) mongets(mtmp, POT_CONFUSION);
		(void) mongets(mtmp, ROBE);
		(void) mongets(mtmp, SCR_ENRAGE);
	}

	if (ptr == &mons[PM_SIMONE]) {

		mtmp->fartbonus = 2;

	}

	if (ptr == &mons[PM_RUEA]) {

		mtmp->crapbonus = 5;

	}

	if (ptr == &mons[PM_SARAH]) {

		mtmp->fartbonus = 4;

	}

	if (ptr == &mons[PM_SUNALI]) {

		mtmp->crapbonus = 5;

	}

	if (ptr == &mons[PM_KATI]) {

		mtmp->crapbonus = 2;
		mtmp->fartbonus = 9;

	}

	if (ptr == &mons[PM_THAI]) {

		mtmp->crapbonus = 3;

	}

	if (ptr == &mons[PM_ELIF]) {

		mtmp->fartbonus = 7;

	}

	if (ptr == &mons[PM_SOLVEJG]) {

		mtmp->fartbonus = 1;

	}

	if (ptr == &mons[PM_ANTJE]) {

		mtmp->crapbonus = 1;

	}

	if (ptr == &mons[PM_MAURAH]) {

		mtmp->crapbonus = 6;
		mtmp->fartbonus = 3;

	}

	if (ptr == &mons[PM_KLARA]) {

		mtmp->crapbonus = 10;

	}

	if (ptr == &mons[PM_KERSTIN]) {

		mtmp->crapbonus = 2;
		mtmp->fartbonus = 3;

	}

	if (ptr == &mons[PM_EVELINE]) {

		mtmp->crapbonus = 2;

	}

	if (ptr == &mons[PM_INA]) {

		mtmp->crapbonus = 3;
		mtmp->fartbonus = 6;

	}

	if (ptr == &mons[PM_KARIN]) {

		mtmp->crapbonus = 8;

	}

	if (ptr == &mons[PM_SUE_LYN]) {

		mtmp->crapbonus = 4;
		mtmp->fartbonus = 9;

	}

	if (ptr == &mons[PM_MARIKE]) {

		mtmp->fartbonus = 9;

	}

	if (ptr == &mons[PM_KATIA]) {

		mtmp->crapbonus = 20;

	}

	if (ptr == &mons[PM_LUDGERA]) {

		mtmp->crapbonus = 1;

	}

	if (ptr == &mons[PM_MARIYA]) {

		mtmp->fartbonus = 8;

	}

	if (ptr == &mons[PM_SANDRA]) {

		mtmp->fartbonus = 5;

	}

	if (ptr == &mons[PM_MELTEM]) {

		mtmp->fartbonus = 4;

	}

	if (ptr == &mons[PM_TENDER_ASIAN_GIRL]) {

		mtmp->fartbonus = 7;

	}

	if (ptr == &mons[PM_CHINESE_GIRL]) {

		mtmp->fartbonus = 7;

	}

	if (ptr == &mons[PM_KOREAN_GIRL]) {

		mtmp->fartbonus = 8;

	}

	if (ptr == &mons[PM_TURKISH_GIRL]) {

		mtmp->crapbonus = 4;

	}

	if (ptr == &mons[PM_YOGA_GIRL]) {

		mtmp->fartbonus = 3;

	}

	if (ptr == &mons[PM_BEAUTIFUL_GIRL]) {

		mtmp->fartbonus = 5;

	}

	if (ptr == &mons[PM_PRETTY_ASIAN_WOMAN]) {

		mtmp->fartbonus = 7;

	}

	if (ptr == &mons[PM_SEXY_CHINESE_WOMAN]) {

		mtmp->fartbonus = 7;

	}

	if (ptr == &mons[PM_KOREAN_WOMAN]) {

		mtmp->fartbonus = 6;

	}

	if (ptr == &mons[PM_TURKISH_WOMAN]) {

		mtmp->crapbonus = 3;

	}

	if (ptr == &mons[PM_WONDERFUL_LADY]) {

		mtmp->fartbonus = 6;

	}

	if (ptr == &mons[PM_SEXY_ASIAN_LADY]) {

		mtmp->fartbonus = 5;

	}

	if (ptr == &mons[PM_CHINESE_LADY]) {

		mtmp->fartbonus = 5;

	}

	if (ptr == &mons[PM_KOREAN_LADY]) {

		mtmp->fartbonus = 5;

	}

	if (ptr == &mons[PM_TURKISH_LADY]) {

		mtmp->crapbonus = 2;

	}

	if (ptr == &mons[PM_WATER_GIRL]) {

		mtmp->fartbonus = 6;

	}

	if (ptr == &mons[PM_NATURE_GIRL]) {

		mtmp->fartbonus = 7;

	}

	if (ptr == &mons[PM_DEFORMED_GIRL]) {

		mtmp->crapbonus = 10;

	}

	if (ptr == &mons[PM_MAGICAL_GIRL]) {

		mtmp->fartbonus = 5;

	}

	if (ptr == &mons[PM_WATER_LADY]) {

		mtmp->fartbonus = 6;

	}

	if (ptr == &mons[PM_NATURE_LADY]) {

		mtmp->fartbonus = 8;

	}

	if (ptr == &mons[PM_ICE_LADY]) {

		mtmp->fartbonus = 2;

	}

	if (ptr == &mons[PM_FEMALE_HULL]) {

		mtmp->crapbonus = 15;

	}

	if (ptr == &mons[PM_SADISTIC_LADY]) {

		mtmp->fartbonus = 5;

	}

	if (ptr == &mons[PM_BDSM_LADY]) {

		mtmp->fartbonus = 3;

	}

	if (ptr == &mons[PM_MAGICAL_LADY]) {

		mtmp->fartbonus = 7;

	}

	if (ptr == &mons[PM_FREAKY_GIRL]) {

		mtmp->fartbonus = 5;

	}

	if (ptr == &mons[PM_SLUT]) {

		mtmp->fartbonus = 4;
		mtmp->crapbonus = 6;

	}

	if (ptr == &mons[PM_WENCH]) {

		mtmp->fartbonus = 8;
		mtmp->crapbonus = 9;

	}

	if (ptr == &mons[PM_WILD_WOMAN]) {

		mtmp->fartbonus = 5;
		mtmp->crapbonus = 2;

	}

	if (ptr == &mons[PM_WENDY_O__KOOPA]) {

		mtmp->crapbonus = 5;
		mtmp->fartbonus = 7;

	}

	if (ptr == &mons[PM_ELENA]) {

		mtmp->crapbonus = 5;
		mtmp->fartbonus = 9;

	}

	if (ptr == &mons[PM_KATHARINA]) {

		mtmp->crapbonus = 6;
		mtmp->fartbonus = 9;

	}

	if (ptr == &mons[PM_JESSICA]) {

		mtmp->fartbonus = 9;

	}

	if (ptr == &mons[PM_JENNIFER]) {

		mtmp->crapbonus = 1;

	}

	if (ptr == &mons[PM_YVONNE]) {

		mtmp->crapbonus = 2;

	}

	if (ptr == &mons[PM_MIRIAM]) {

		mtmp->fartbonus = 2;

	}

	if (ptr == &mons[PM_LISA]) {

		mtmp->crapbonus = 2;

	}

	if (ptr == &mons[PM_BIRGIT]) {

		mtmp->fartbonus = 5;

	}

	if (ptr == &mons[PM_INGE]) {

		mtmp->fartbonus = 8;

	}

	if (ptr == &mons[PM_MISS_SCHRACK]) {

		mtmp->fartbonus = 7;

	}

	if (ptr == &mons[PM_EXTRA_FLEECY_BUNDLE]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SOFT_SNEAKERS);
	}

	if (ptr == &mons[PM_EMMELIE]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, T_SHIRT);
	}

	if (ptr == &mons[PM_LUISA]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, BULLWHIP);
	}

	if (ptr == &mons[PM_SHY_LAURA]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, LEATHER_CLOAK);
		(void) mongets(mtmp, LEATHER_JACKET);
	}

	if (ptr == &mons[PM_ANNA]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, POT_NUMBNESS);
	}

	if (ptr == &mons[PM_LEXI]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		m_initthrow(mtmp, CREAM_PIE, 4);
	}

	if (ptr == &mons[PM_SOFT_SARAH]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SOFT_SNEAKERS);
	}

	if (ptr == &mons[PM_MAREIKE]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, SCR_FIRE);
	}

	if (ptr == &mons[PM_REBECCA]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, WAN_FEAR);
	}

	if (ptr == &mons[PM_ROUGH_TERESA]) {
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, POT_HALLUCINATION);
	}

	if (ptr == &mons[PM_JANINE]) {
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, POT_BLINDNESS);
		(void) mongets(mtmp, SCR_SUMMON_ELM);
	}

	if (ptr == &mons[PM_BITCHY_LARA]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, POT_STUNNING);
		(void) mongets(mtmp, SCR_MEGALOAD);
	}

	if (ptr == &mons[PM_MARLEEN]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, POT_PARALYSIS);
		(void) mongets(mtmp, WAN_STUN_MONSTER);
	}

	if (ptr == &mons[PM_NONEROTIC_IRINA]) {
		(void) mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
		(void) mongets(mtmp, WEDGE_SANDALS);
		(void) mongets(mtmp, WAN_BANISHMENT);
		(void) mongets(mtmp, SCR_RELOCATION);
	}

	if (ptr == &mons[PM_BUNDLY_ANN]) {
		(void) mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, WAN_STRIKING);
		(void) mongets(mtmp, STEEL_WHIP);
	}

	if (ptr == &mons[PM_LISELOTTE]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, POT_CONFUSION);
	}

	if (ptr == &mons[PM_MAY_BRITT]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, SCR_WOUNDS);
		(void) mongets(mtmp, SCR_EGOISM);
		if (!rn2(20)) mongets(mtmp, WAN_EGOISM);
		(void) mongets(mtmp, POT_CONFUSION);
	}

	if (ptr == &mons[PM_LILLY]) {
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, SCR_WARPING);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
	}

	if (ptr == &mons[PM_MIRIAM_THE_SPIRIT_GIRL]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, POT_NUMBNESS);
		(void) mongets(mtmp, SCR_GROUP_SUMMONING);
	}

	if (ptr == &mons[PM_THE_SWEET_HIGH_HEEL_LOVING_ASIAN_GIRL]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, WAN_FINGER_BENDING);
		(void) mongets(mtmp, WAN_STUN_MONSTER);
		if (!rn2(10)) (void) mongets(mtmp, WAN_SIN);
	}

	if (ptr == &mons[PM_EMMA]) {
		(void) mongets(mtmp, PLATE_MAIL);
	}

	if (ptr == &mons[PM_ALIDA]) {
		(void) mongets(mtmp, CLOAK_OF_MAGIC_RESISTANCE);
	}

	if (ptr == &mons[PM_JOSEFINE]) {
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, DANCING_SHOES);
		if (!rn2(10)) (void) mongets(mtmp, WAN_EGOISM);
		if (!rn2(3)) (void) mongets(mtmp, SCR_EGOISM);
	}

	if (ptr == &mons[PM_VILEA]) {
		(void) mongets(mtmp, HUGGING_BOOT);
	}

	if (ptr == &mons[PM_VILEA_S_SISTER]) {
		(void) mongets(mtmp, HUGGING_BOOT);
	}

	if (ptr == &mons[PM_HANNAH]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, POT_BLINDNESS);
	}

	if (ptr == &mons[PM_AMELJE]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, FEMININE_PUMPS);
	}

	if (ptr == &mons[PM_NON_PRETTY_MELANIE]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SWEET_MOCASSINS);
	}

	if (ptr == &mons[PM_THE_SWEET_REDGUARD_GIRL]) {
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, POT_STUNNING);
		(void) mongets(mtmp, POT_RADIUM);
	}

	if (ptr == &mons[PM_THE_HEELED_TOPMODEL]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, SCR_RUMOR);
	}

	if (ptr == &mons[PM_THE_HUGGING_TOPMODEL]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, DANCING_SHOES);
		(void) mongets(mtmp, SCR_MESSAGE);
	}

	if (ptr == &mons[PM_MARIE]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SOFT_SNEAKERS);
	}

	if (ptr == &mons[PM_THE_FAIRY]) {
		(void) mongets(mtmp, RING_MAIL);
		(void) mongets(mtmp, LEATHER_CLOAK);
	}

	if (ptr == &mons[PM_FANNY]) {
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, WEDGE_SANDALS);
		(void) mongets(mtmp, WAN_STUN_MONSTER);
	}

	if (ptr == &mons[PM_THE_BASTARD_BROTHER_OF_SHY_LAURA]) {
		(void) mongets(mtmp, STYGIAN_PIKE);
	}

	if (ptr == &mons[PM_APE_HEAD]) {
		(void) mongets(mtmp, POT_CYANIDE);
		(void) mongets(mtmp, POT_URINE);
		(void) mongets(mtmp, SCR_BULLSHIT);
	}

	if (ptr == &mons[PM_MALADJUSTED_LEON]) {
		(void) mongets(mtmp, WAN_MAGIC_MISSILE);
	}

	if (ptr == &mons[PM_MAX_THE_BADASS]) {
		(void) mongets(mtmp, BASEBALL_BAT);
	}

	if (ptr == &mons[PM_ANTON]) {
		(void) mongets(mtmp, METAL_CLUB);
		(void) mongets(mtmp, SLING);
		m_initthrow(mtmp, CAPTCHA_STONE, 1);
	}

	if (ptr == &mons[PM_JONAS_THE_SCHIZO_AUTIST]) {
		m_initthrow(mtmp, DART, 20);
	}

	if (ptr == &mons[PM_LARS_THE_SCHIZOPHRENIC]) {
		m_initthrow(mtmp, SHURIKEN, 1);
	}

	if (ptr == &mons[PM_MELVIN]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SCR_BULLSHIT);
	}

	if (ptr == &mons[PM_THE_BASTARD_BROTHER_OF_LILLY]) {
		(void) mongets(mtmp, SCR_CHAOS_TERRAIN);
	}

	if (ptr == &mons[PM_THE_DISGUSTING_FRIEND_OF_LILLY_S_BROTHER]) {
		(void) mongets(mtmp, POT_RADIUM);
	}

	if (ptr == &mons[PM_THE_DISGUSTING_SMOKER_FRIEND_OF_MARIE]) {
		(void) mongets(mtmp, POT_CYANIDE);
		(void) mongets(mtmp, WAN_DISINTEGRATION);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		(void) mongets(mtmp, SHIELD_OF_REFLECTION);
		(void) mongets(mtmp, RUBBER_HOSE);
	}

	if (ptr == &mons[PM_SCHALOTTE]) {
		(void) mongets(mtmp, WAN_CONFUSION);
	}

	if (ptr == &mons[PM_ROXY_GRETA]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);
		(void) mongets(mtmp, POT_CONFUSION);
		(void) mongets(mtmp, POT_STUNNING);
	}

	if (ptr == &mons[PM_BUNDLE_NADJA]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, POT_PARALYSIS);
		(void) mongets(mtmp, VICTORIAN_UNDERWEAR);
	}

	if (ptr == &mons[PM_STRU_BES]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, CLOAK_OF_REFLECTION);
		(void) mongets(mtmp, BAG_OF_TRICKS);
		(void) mongets(mtmp, FIRE_HELMET);
		(void) mongets(mtmp, OILSKIN_GLOVES);
		(void) mongets(mtmp, ICE_SHIELD);
		(void) mongets(mtmp, SCR_ANTIMATTER);
		(void) mongets(mtmp, WAN_INERTIA);
		(void) mongets(mtmp, BOOTS_OF_ACID_RESISTANCE);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, AMULET_OF_LIFE_SAVING);
		(void) mongets(mtmp, WAN_BAD_EFFECT);
		(void) mongets(mtmp, WAN_GAIN_LEVEL);
		(void) mongets(mtmp, WAN_INCREASE_MAX_HITPOINTS);
		(void) mongets(mtmp, WAN_REDUCE_MAX_HITPOINTS);
		(void) mongets(mtmp, WAN_CURSE_ITEMS);
	}

	if (ptr == &mons[PM_YANNICK]) {
		(void) mongets(mtmp, HEAVY_HAMMER);
		(void) mongets(mtmp, SCR_CLOUDS);
	}

	if (ptr == &mons[PM_FELIX]) {
		(void) mongets(mtmp, CATAPULT);
		m_initthrow(mtmp, FLINT, 50);
	}

	if (ptr == &mons[PM_STEFAN]) {
		(void) mongets(mtmp, POT_HEALING);
		(void) mongets(mtmp, STRIPED_SHIRT);
	}

	if (ptr == &mons[PM_FREDDIE]) {
		(void) mongets(mtmp, POT_STUNNING);
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
	}

	if (ptr == &mons[PM_MICHAEL]) {
		(void) mongets(mtmp, BROADSWORD);
		(void) mongets(mtmp, PLATE_MAIL);
		(void) mongets(mtmp, POT_SLEEPING);
	}

	if (ptr == &mons[PM_HANNO]) {
		(void) mongets(mtmp, KATANA);
		(void) mongets(mtmp, SPLINT_MAIL);
	}

	if (ptr == &mons[PM_ROBIN]) {
		(void) mongets(mtmp, ELVEN_CLOAK);
		(void) mongets(mtmp, POT_INVISIBILITY);
		(void) mongets(mtmp, ELVEN_BOW);
		m_initthrow(mtmp, ELVEN_ARROW, 50);
		m_initthrow(mtmp, ELVEN_ARROW, 50);
	}

	if (ptr == &mons[PM_JNR]) {
		(void) mongets(mtmp, STYGIAN_PIKE);
		(void) mongets(mtmp, BRONZE_PLATE_MAIL);
		(void) mongets(mtmp, SCR_HEALING);
		(void) mongets(mtmp, POT_EXTRA_HEALING);
	}

	if (ptr == &mons[PM_LARS]) {
		(void) mongets(mtmp, BULLWHIP);
		(void) mongets(mtmp, WAN_MAGIC_MISSILE);
	}

	if (ptr == &mons[PM_TOMMY]) {
		(void) mongets(mtmp, FLY_SWATTER);
		(void) mongets(mtmp, ROBE_OF_WEAKNESS);
		(void) mongets(mtmp, GNOMISH_BOOTS);
		(void) mongets(mtmp, POT_MUTATION);
	}

	if (ptr == &mons[PM_LUKAS]) {
		(void) mongets(mtmp, WAN_GAIN_LEVEL);
		(void) mongets(mtmp, OBSID);
		(void) mongets(mtmp, PLASTEEL_ARMOR);
		(void) mongets(mtmp, GREEN_LIGHTSABER);
		(void) mongets(mtmp, AMULET_OF_LIFE_SAVING);
		(void) mongets(mtmp, WAN_FULL_HEALING);
		(void) mongets(mtmp, WAN_BAD_EFFECT);
	}

	if (ptr == &mons[PM_LUKAS_S_BODYGUARD]) {
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SNIPER_RIFLE);
		(void) mongets(mtmp, POT_FEAR);
		(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
		(void) mongets(mtmp, CLOAK_OF_MAGIC_RESISTANCE);
		m_initthrow(mtmp, BULLET, 50);
		m_initthrow(mtmp, BULLET, 50);
	}

	if (ptr == &mons[PM_SELINGER]) {
		(void) mongets(mtmp, HUGGING_BOOT);
		(void) mongets(mtmp, SPEED_BOOTS);
		(void) mongets(mtmp, POT_SPEED);
		(void) mongets(mtmp, SCR_TELEPORTATION);
		(void) mongets(mtmp, SCR_RELOCATION);
		(void) mongets(mtmp, SCR_TELEPORTATION);
		(void) mongets(mtmp, SCR_TELEPORTATION);
		(void) mongets(mtmp, WAN_TELEPORTATION);
		(void) mongets(mtmp, WAN_DIGGING);
	}

	if (ptr == &mons[PM_BAENNI]) {
		(void) mongets(mtmp, SCR_BULLSHIT);
		(void) mongets(mtmp, POT_URINE);
		(void) mongets(mtmp, STRIPED_SHIRT);
		(void) mongets(mtmp, POT_SLIME);
	}

	if (ptr == &mons[PM_WALTER]) {
		(void) mongets(mtmp, RUBBER_HOSE);
		(void) mongets(mtmp, PISTOL);
		(void) mongets(mtmp, WAN_STUN_MONSTER);
		(void) mongets(mtmp, CHAIN_MAIL);
		m_initthrow(mtmp, BULLET, 20);
		m_initthrow(mtmp, CREAM_PIE, 4);
	}

	if (ptr == &mons[PM_KLAUS]) {
		(void) mongets(mtmp, RUBBER_HOSE);
		(void) mongets(mtmp, PISTOL);
		(void) mongets(mtmp, WAN_STUN_MONSTER);
		(void) mongets(mtmp, CHAIN_MAIL);
		m_initthrow(mtmp, BULLET, 20);
		m_initthrow(mtmp, CREAM_PIE, 4);
	}

	if (ptr == &mons[PM_LAURENS]) {
		(void) mongets(mtmp, SCR_TELEPORTATION);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		m_initthrow(mtmp, DART, 20);
	}

	if (ptr == &mons[PM_JORIN]) {
		(void) mongets(mtmp, CROSSBOW);
		(void) mongets(mtmp, BANDED_MAIL);
		(void) mongets(mtmp, WAN_TELEPORTATION);
		(void) mongets(mtmp, SCR_FIRE);
		m_initthrow(mtmp, CROSSBOW_BOLT, 50);
	}

	if (ptr == &mons[PM_ANDREJ]) {
		(void) mongets(mtmp, DAGGER);
		(void) mongets(mtmp, SLING);
		m_initthrow(mtmp, ROCK, 10);
	}

	if (ptr == &mons[PM_ANSELM]) {
		(void) mongets(mtmp, SCR_GROWTH);
		(void) mongets(mtmp, POT_HEALING);
		(void) mongets(mtmp, POT_EXTRA_HEALING);
		(void) mongets(mtmp, BOW);
		(void) mongets(mtmp, CLUB);
		(void) mongets(mtmp, LEATHER_ARMOR);
		m_initthrow(mtmp, ORCISH_ARROW, 5);
	}

	if (ptr == &mons[PM_ARAM]) {
		(void) mongets(mtmp, WAN_SLEEP);
		(void) mongets(mtmp, LONG_SWORD);
		(void) mongets(mtmp, ORCISH_CLOAK);
	}

	if (ptr == &mons[PM_BORIS]) {
		(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
		(void) mongets(mtmp, RUNESWORD);
		(void) mongets(mtmp, WAN_COLD);
		(void) mongets(mtmp, WAN_ACID);
	}

	if (ptr == &mons[PM_BURKHARD]) {
		(void) mongets(mtmp, DROVEN_GREATSWORD);
		(void) mongets(mtmp, DWARVISH_MITHRIL_COAT);
		(void) mongets(mtmp, WAN_HEALING);
		m_initthrow(mtmp, FRAG_GRENADE, 10);
	}

	if (ptr == &mons[PM_NILS]) {
		(void) mongets(mtmp, DWARVISH_SPEAR);
		(void) mongets(mtmp, HELMET);
		m_initthrow(mtmp, ORCISH_SPEAR, 5);
	}

	if (ptr == &mons[PM_OZAN]) {
		(void) mongets(mtmp, SILVER_SABER);
		(void) mongets(mtmp, SLING);
		m_initthrow(mtmp, OPAL, 5);
	}

	if (ptr == &mons[PM_THORWALD]) {
		(void) mongets(mtmp, SHORT_SWORD);
		(void) mongets(mtmp, LEATHER_ARMOR);
		(void) mongets(mtmp, BOW);
		(void) mongets(mtmp, SCR_WOUNDS);
		m_initthrow(mtmp, ARROW, 1);
	}

	if (ptr == &mons[PM_HENDRIC]) {
		(void) mongets(mtmp, POT_SPEED);
		(void) mongets(mtmp, AXE);
		(void) mongets(mtmp, SHOTGUN);
		m_initthrow(mtmp, SHOTGUN_SHELL, 8);
	}

	if (ptr == &mons[PM_NINO]) {
		(void) mongets(mtmp, WAN_TELEPORTATION);
		(void) mongets(mtmp, BILL_GUISARME);
		(void) mongets(mtmp, POT_CONFUSION);
		(void) mongets(mtmp, SCR_LAVA);
		(void) mongets(mtmp, SCR_GROUP_SUMMONING);
	}

	if (ptr == &mons[PM_KASTORTRANSPORT]) {
		(void) mongets(mtmp, SCR_CHAOS_TERRAIN);
		(void) mongets(mtmp, SCR_CHAOS_TERRAIN);
		(void) mongets(mtmp, SCR_CHAOS_TERRAIN);
		(void) mongets(mtmp, SCR_TELEPORTATION);
		(void) mongets(mtmp, SCR_FLOODING);
		(void) mongets(mtmp, WAN_STRIKING);
		(void) mongets(mtmp, RING_MAIL);
	}

	if (ptr == &mons[PM_LARRY]) {
		(void) mongets(mtmp, WAN_MAGIC_MISSILE);
	}

	if (ptr == &mons[PM_MORTON]) {
		(void) mongets(mtmp, POT_STUNNING);
		(void) mongets(mtmp, BROADSWORD);
		(void) mongets(mtmp, WAN_MAGIC_MISSILE);
	}

	if (ptr == &mons[PM_IGGY]) {
		(void) mongets(mtmp, WAN_MAGIC_MISSILE);
		(void) mongets(mtmp, WAN_TELEPORTATION);
	}

	if (ptr == &mons[PM_LEMMY]) {
		(void) mongets(mtmp, SLING);
		(void) mongets(mtmp, SCR_ICE);
		m_initthrow(mtmp, EPIDOTE, 3);
		m_initthrow(mtmp, CYAN_STONE, 3);
	}

	if (ptr == &mons[PM_LUDWIG]) {
		(void) mongets(mtmp, WAN_MAGIC_MISSILE);
		(void) mongets(mtmp, POT_STUNNING);
		(void) mongets(mtmp, POT_CONFUSION);
		(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
		(void) mongets(mtmp, SCR_WARPING);
	}

	if (ptr == &mons[PM_DANIEL]) {
		(void) mongets(mtmp, MALLET);
		(void) mongets(mtmp, AUTO_SHOTGUN);
		(void) mongets(mtmp, GRAY_DRAGON_SCALE_MAIL);
		(void) mongets(mtmp, WAN_DISINTEGRATION);
		m_initthrow(mtmp, SHOTGUN_SHELL, 25);
	}

	if (ptr == &mons[PM_LEN_KIND]) {
		(void) mongets(mtmp, WAN_HASTE_MONSTER);
		(void) mongets(mtmp, POT_FULL_HEALING);
		(void) mongets(mtmp, BULLWHIP);
		(void) mongets(mtmp, ORCISH_CHAIN_MAIL);
	}

	if (ptr == &mons[PM_ILJA]) {
		(void) mongets(mtmp, POT_SLIME);
		(void) mongets(mtmp, SCR_EARTH);
		(void) mongets(mtmp, WAN_TELE_LEVEL);
		(void) mongets(mtmp, WAN_DRAINING);
		(void) mongets(mtmp, WAN_TIDAL_WAVE);
		(void) mongets(mtmp, BENT_SABLE);
		(void) mongets(mtmp, GNOMISH_SUIT);
		(void) mongets(mtmp, ORCISH_HELM);
	}

	if (ptr == &mons[PM_TILL]) {
		(void) mongets(mtmp, BEC_DE_CORBIN);
		(void) mongets(mtmp, BRONZE_PLATE_MAIL);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
	}

	if (ptr == &mons[PM_TOMAS]) {
		(void) mongets(mtmp, POT_GAIN_LEVEL);
		(void) mongets(mtmp, URUK_HAI_SHIELD);
		(void) mongets(mtmp, CATAPULT);
		m_initthrow(mtmp, FLINT, 40);
	}

	if (ptr == &mons[PM_NICO]) {
		(void) mongets(mtmp, GREAT_DAGGER);
		(void) mongets(mtmp, BAG_OF_TRICKS);
		(void) mongets(mtmp, SCR_DESTROY_ARMOR);
		(void) mongets(mtmp, SCR_DESTROY_ARMOR);
		(void) mongets(mtmp, WAN_BAD_EFFECT);
		(void) mongets(mtmp, WAN_CURSE_ITEMS);
		(void) mongets(mtmp, WAN_REMOVE_RESISTANCE);
		m_initthrow(mtmp, KNIFE, 20);
	}

	if (ptr == &mons[PM_TILLMANN]) {
		(void) mongets(mtmp, TSURUGI);
		(void) mongets(mtmp, FORCE_PIKE);
		(void) mongets(mtmp, YELLOW_DRAGON_SCALES);
		(void) mongets(mtmp, VENOM_SHIELD);
		(void) mongets(mtmp, AMULET_OF_LIFE_SAVING);
		(void) mongets(mtmp, POT_ICE);
		(void) mongets(mtmp, SCR_EGOISM);
		(void) mongets(mtmp, SCR_BAD_EFFECT);
		(void) mongets(mtmp, WAN_GAIN_LEVEL);
		(void) mongets(mtmp, WAN_CREATE_HORDE);
	}

	if (ptr == &mons[PM_WALKER]) {
		(void) mongets(mtmp, WAN_BAD_LUCK);
		(void) mongets(mtmp, WAN_CORROSION);
		(void) mongets(mtmp, WAN_LYCANTHROPY);
		(void) mongets(mtmp, SCR_SUMMON_BOSS);
		(void) mongets(mtmp, DARK_ELVEN_SHORT_SWORD);
		(void) mongets(mtmp, PLASTEEL_BOOTS);
	}

	if (ptr == &mons[PM_NIKOLEI]) {
		(void) mongets(mtmp, POT_SPEED);
		(void) mongets(mtmp, WAN_HASTE_MONSTER);
		(void) mongets(mtmp, TRIDENT);
		(void) mongets(mtmp, SLING);
		m_initthrow(mtmp, SALT_CHUNK, 10);
	}

	if (ptr == &mons[PM_JONAS]) {
		(void) mongets(mtmp, WAN_SLIMING);
	}

	if (ptr == &mons[PM_IWAN]) {
		(void) mongets(mtmp, KNIFE);
		(void) mongets(mtmp, CRYSKNIFE);
		(void) mongets(mtmp, RIFLE);
		(void) mongets(mtmp, SCR_SUMMON_BOSS);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		m_initthrow(mtmp, BULLET, 10);
	}

	if (ptr == &mons[PM_RUBINHO]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, RIFLE);
		(void) mongets(mtmp, CHAIN_MAIL);
		(void) mongets(mtmp, WAN_FIRE);
		(void) mongets(mtmp, POT_FIRE);
		(void) mongets(mtmp, SCR_HEALING);
		(void) mongets(mtmp, POT_EXTRA_HEALING);
		m_initthrow(mtmp, BULLET, 50);
		m_initthrow(mtmp, BULLET, 50);
		m_initthrow(mtmp, SHURIKEN, 50);
	}

	if (ptr == &mons[PM_THE_COFFIN_NAIL]) {
		(void) mongets(mtmp, WAN_SUMMON_UNDEAD);
		(void) mongets(mtmp, WAN_SUMMON_UNDEAD);
		(void) mongets(mtmp, WAN_SUMMON_UNDEAD);
		(void) mongets(mtmp, PICK_AXE);
		(void) mongets(mtmp, SCR_SUMMON_GHOST);
		(void) mongets(mtmp, SCR_SUMMON_GHOST);
		(void) mongets(mtmp, SMALL_SHIELD);
	}

	if (ptr == &mons[PM_EVILLAN]) {
		(void) mongets(mtmp, WAN_CREATE_MONSTER);
		(void) mongets(mtmp, SILVER_LONG_SWORD);
		(void) mongets(mtmp, CATAPULT);
		(void) mongets(mtmp, SILVER_DRAGON_SCALE_MAIL);
		m_initthrow(mtmp, CIRMOCLINE, 7);
		m_initthrow(mtmp, SILVER_SPEAR, 5);
	}

	if (ptr == &mons[PM_THILO]) {
		(void) mongets(mtmp, KNIFE);
		(void) mongets(mtmp, KNIFE);
	}

	if (ptr == &mons[PM_MAURUS]) {
		(void) mongets(mtmp, SCR_BAD_EFFECT);
		(void) mongets(mtmp, SCR_SUMMON_UNDEAD);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
	}

	if (ptr == &mons[PM_DOMINIK]) {
		(void) mongets(mtmp, POT_STUNNING);
		(void) mongets(mtmp, WAN_REDUCE_MAX_HITPOINTS);
		(void) mongets(mtmp, BATTLE_STAFF);
		(void) mongets(mtmp, ELVEN_CLOAK);
		(void) mongets(mtmp, WAN_LYCANTHROPY);
		(void) mongets(mtmp, WAN_STUN_MONSTER);
	}

	if (ptr == &mons[PM_ALBERT]) {
		(void) mongets(mtmp, PISTOL);
		(void) mongets(mtmp, KNIFE);
		(void) mongets(mtmp, SCR_HEALING);
		(void) mongets(mtmp, PLASTEEL_ARMOR);
		m_initthrow(mtmp, BULLET, 50);
	}

	if (ptr == &mons[PM_DON_FINWE]) {
		(void) mongets(mtmp, ELVEN_BOW);
		(void) mongets(mtmp, ELVEN_MITHRIL_COAT);
		(void) mongets(mtmp, ELVEN_BROADSWORD);
		(void) mongets(mtmp, ELVEN_LEATHER_HELM);
		(void) mongets(mtmp, ELVEN_BOOTS);
		(void) mongets(mtmp, WAN_EXTRA_HEALING);
		m_initthrow(mtmp, ELVEN_DAGGER, 5);
		m_initthrow(mtmp, ELVEN_SPEAR, 5);
		m_initthrow(mtmp, ELVEN_ARROW, 50);
	}

	if (ptr == &mons[PM_GERRIT]) {
		(void) mongets(mtmp, SPEAR);
	}

	if (ptr == &mons[PM_JENS]) {
		(void) mongets(mtmp, PISTOL);
		(void) mongets(mtmp, SCR_WARPING);
		m_initthrow(mtmp, SILVER_BULLET, 50);
	}

	if (ptr == &mons[PM_LEON]) {
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, LEATHER_ARMOR);
		(void) mongets(mtmp, BATTLE_AXE);
		(void) mongets(mtmp, SCR_GROWTH);
		(void) mongets(mtmp, SCR_GROWTH);
	}

	if (ptr == &mons[PM_MARIUS]) {
		(void) mongets(mtmp, SLING);
		m_initthrow(mtmp, ROCK, 5);
	}

	if (ptr == &mons[PM_SIGURD]) {
		(void) mongets(mtmp, WAN_CORROSION);
		(void) mongets(mtmp, POT_AMNESIA);
		(void) mongets(mtmp, CLOAK_OF_REFLECTION);
		(void) mongets(mtmp, LONG_SWORD);
	}

	if (ptr == &mons[PM_WILHELM]) {
		(void) mongets(mtmp, KATANA);
		(void) mongets(mtmp, YUMI);
		m_initthrow(mtmp, YA, 50);
	}

	if (ptr == &mons[PM_ANDREAS]) {
		(void) mongets(mtmp, WAR_HAMMER);
		(void) mongets(mtmp, PLATE_MAIL);
		(void) mongets(mtmp, SCR_TELEPORTATION);
	}

	if (ptr == &mons[PM_TYPE_OF_ICE_BLOCK]) {
		(void) mongets(mtmp, POT_ICE);
		(void) mongets(mtmp, POT_ICE);
		(void) mongets(mtmp, POT_ICE);
		(void) mongets(mtmp, POT_ICE);
		(void) mongets(mtmp, POT_ICE);
		(void) mongets(mtmp, SCR_ICE);
		(void) mongets(mtmp, SCR_ICE);
		(void) mongets(mtmp, SCR_ICE);
		(void) mongets(mtmp, SCR_FLOOD);
		(void) mongets(mtmp, WAN_COLD);
		(void) mongets(mtmp, WHITE_DRAGON_SCALE_MAIL);
		(void) mongets(mtmp, HELM_OF_STEEL);
		(void) mongets(mtmp, SCR_BAD_EFFECT);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
	}

	if (ptr == &mons[PM_SVEN]) {
		(void) mongets(mtmp, WAN_SOLAR_BEAM);
		(void) mongets(mtmp, SCR_DESTROY_ARMOR);
		(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
		(void) mongets(mtmp, HEAVY_MACHINE_GUN);
		(void) mongets(mtmp, TSURUGI);
		(void) mongets(mtmp, GRAY_DRAGON_SCALE_MAIL);
		(void) mongets(mtmp, POT_FULL_HEALING);
		(void) mongets(mtmp, WAN_FULL_HEALING);
		(void) mongets(mtmp, SCR_SUMMON_BOSS);
		(void) mongets(mtmp, SCR_SUMMON_BOSS);
		(void) mongets(mtmp, SCR_SUMMON_BOSS);
		(void) mongets(mtmp, WAN_CREATE_HORDE);
		(void) mongets(mtmp, WAN_CREATE_HORDE);
		m_initthrow(mtmp, SILVER_BULLET, 50);
		m_initthrow(mtmp, SILVER_BULLET, 50);
		m_initthrow(mtmp, SILVER_BULLET, 50);
		m_initthrow(mtmp, SILVER_BULLET, 50);
	}

	if (ptr == &mons[PM_DR__DELILAH]) {
		(void) mongets(mtmp, POT_ACID);
		(void) mongets(mtmp, POT_ACID);
		(void) mongets(mtmp, WAN_ACID);
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, QUARTERSTAFF);
	}

	if (ptr == &mons[PM_JUTTA_FLAX]) {
		(void) mongets(mtmp, SCR_BAD_EFFECT);
		(void) mongets(mtmp, SCR_SIN);
		(void) mongets(mtmp, SCR_TRAP_CREATION);
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, LACQUERED_DANCING_SHOE);
		(void) mongets(mtmp, POT_CONFUSION);
		(void) mongets(mtmp, POT_STUNNING);
	}

	if (ptr == &mons[PM_ANNIE]) {
		(void) mongets(mtmp, WAN_REMOVE_RESISTANCE);
		(void) mongets(mtmp, BULLWHIP);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
	}

	if (ptr == &mons[PM_BIRGIT]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, HIPPIE_HEELS);
		(void) mongets(mtmp, WAN_STRIKING);
		(void) mongets(mtmp, LEATHER_ARMOR);
		(void) mongets(mtmp, LEATHER_CLOAK);
		(void) mongets(mtmp, BUGLE);
		(void) mongets(mtmp, SCR_RELOCATION);
	}

	if (ptr == &mons[PM_INGE]) {
		(void) mongets(mtmp, SCR_LAVA);
		(void) mongets(mtmp, SCR_BARRHING);
		(void) mongets(mtmp, SCR_CHAOS_TERRAIN);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
		(void) mongets(mtmp, HIPPIE_HEELS);
	}

	if (ptr == &mons[PM_ALICE]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, BULLWHIP);
		(void) mongets(mtmp, RING_MAIL);
		(void) mongets(mtmp, HELMET);
		(void) mongets(mtmp, WAN_DRAIN_MANA);
		(void) mongets(mtmp, POT_EXTRA_HEALING);
		(void) mongets(mtmp, SILVER_SPEAR);
	}

	if (ptr == &mons[PM_KELK]) {
		(void) mongets(mtmp, POT_ACID);
		(void) mongets(mtmp, FLY_SWATTER);
		(void) mongets(mtmp, POISONOUS_CLOAK);
		(void) mongets(mtmp, GREEN_DRAGON_SCALES);
		(void) mongets(mtmp, BOOTS_OF_ACID_RESISTANCE);
		(void) mongets(mtmp, POT_RADIUM);
	}

	if (ptr == &mons[PM_KUN_KWEI]) {
		(void) mongets(mtmp, WAN_FEAR);
	}

	if (ptr == &mons[PM_MARENKA]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, STEEL_WHIP);
		(void) mongets(mtmp, SHIELD_OF_REFLECTION);
		(void) mongets(mtmp, WAN_CREATE_MONSTER);
		(void) mongets(mtmp, WAN_STONING);
		m_initthrow(mtmp, SILVER_SPEAR, 3);
	}

	if (ptr == &mons[PM_GRIDIN]) {
		(void) mongets(mtmp, WAN_REDUCE_MAX_HITPOINTS);
		(void) mongets(mtmp, POT_STUNNING);
		(void) mongets(mtmp, RUBBER_HOSE);
	}

	if (ptr == &mons[PM_WURIE]) {
		(void) mongets(mtmp, SCR_SUMMON_BOSS);
	}

	if (ptr == &mons[PM_MISS_BEING_MOOSE]) {
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, SWEET_MOCASSINS);
	}

	if (ptr == &mons[PM_JENNI]) {
		(void) mongets(mtmp, WAN_CURSE_ITEMS);
		(void) mongets(mtmp, STUDDED_LEATHER_ARMOR);
	}

	if (ptr == &mons[PM_IRIS_FLAX]) {
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, PISTOL);
		(void) mongets(mtmp, BOW);
		(void) mongets(mtmp, SILVER_DRAGON_SCALES);
		(void) mongets(mtmp, SCR_BAD_EFFECT);
		m_initthrow(mtmp, SILVER_SPEAR, 10);
		m_initthrow(mtmp, SILVER_BULLET, 50);
		m_initthrow(mtmp, SILVER_DAGGER, 10);
		m_initthrow(mtmp, SILVER_ARROW, 50);
	}

	if (ptr == &mons[PM_DR__PAPPE]) {
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
	}

	if (ptr == &mons[PM_MERRY_LUIS]) {
		(void) mongets(mtmp, WAN_MAGIC_MISSILE);
	}

	if (ptr == &mons[PM_MISS_SCHRACK]) {
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, SPIKED_BATTLE_BOOT);
		(void) mongets(mtmp, LANCE);
		(void) mongets(mtmp, POT_FIRE);
		(void) mongets(mtmp, POT_PARALYSIS);
		(void) mongets(mtmp, SCR_ICE);
		(void) mongets(mtmp, WAN_HEALING);
		m_initthrow(mtmp, GAS_GRENADE, 5);
	}

	if (ptr == &mons[PM_CAROLA]) {
		(void) mongets(mtmp, WAN_PARALYSIS);
	}

	if (ptr == &mons[PM_MISS_YOUNGDRAGON]) {
		(void) mongets(mtmp, WAN_CURSE_ITEMS);
		(void) mongets(mtmp, WAN_BAD_EFFECT);
		(void) mongets(mtmp, GRAY_DRAGON_SCALE_MAIL);
		(void) mongets(mtmp, ORCISH_BOW);
		m_initthrow(mtmp, DARK_ELVEN_ARROW, 50);
	}

	if (ptr == &mons[PM_KLAF]) {
		(void) mongets(mtmp, SCR_TELEPORTATION);
	}

	if (ptr == &mons[PM_MARIA]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, COMBAT_STILETTOS);
		(void) mongets(mtmp, POT_ACID);
		(void) mongets(mtmp, WAN_ACID);
	}

	if (ptr == &mons[PM_MISS_HOUSE_WORKER]) {
		(void) mongets(mtmp, SCR_BAD_EFFECT);
	}

	if (ptr == &mons[PM_GRAHW]) {
		(void) mongets(mtmp, UNICORN_HORN);
		m_initthrow(mtmp, DART, 20);
	}

	if (ptr == &mons[PM_ROSWITHA]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, WAN_HEALING);
		(void) mongets(mtmp, WAN_EXTRA_HEALING);
		(void) mongets(mtmp, WAN_FULL_HEALING);
		(void) mongets(mtmp, WAN_CONFUSION);
		(void) mongets(mtmp, SCR_SUMMON_ELM);
		(void) mongets(mtmp, POT_CONFUSION);
	}

	if (ptr == &mons[PM_RAFSCHAR]) {
		(void) mongets(mtmp, WAN_CREATE_MONSTER);
		(void) mongets(mtmp, T_SHIRT);
	}

	if (ptr == &mons[PM_MISS_NICOLAS]) {
		(void) mongets(mtmp, POT_BLINDNESS);
		(void) mongets(mtmp, POT_ICE);
		(void) mongets(mtmp, POT_FEAR);
	}

	if (ptr == &mons[PM_LILJANA]) {
		(void) mongets(mtmp, WAN_COLD);
		(void) mongets(mtmp, WAN_LIGHTNING);
		(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, SWEET_MOCASSINS);
	}

	if (ptr == &mons[PM_RENDTBAR]) {
		(void) mongets(mtmp, POT_RADIUM);
		(void) mongets(mtmp, POT_RADIUM);
		(void) mongets(mtmp, POT_RADIUM);
	}

	if (ptr == &mons[PM_MISS_DARKMOON]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);
		(void) mongets(mtmp, SCR_FLOOD);
		(void) mongets(mtmp, ELVEN_CLOAK);
		(void) mongets(mtmp, LEATHER_JACKET);
	}

	if (ptr == &mons[PM_MISS_TODAY]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);
	}

	if (ptr == &mons[PM_MISS_MUD_DIRT]) {
		(void) mongets(mtmp, SCR_CHAOS_TERRAIN);
	}

	if (ptr == &mons[PM_SCHMIDTS]) {
		(void) mongets(mtmp, POT_FIRE);
		(void) mongets(mtmp, SCR_LAVA);
		(void) mongets(mtmp, SWEET_MOCASSINS);
	}

	if (ptr == &mons[PM_BUNDLE_TREE]) {
		(void) mongets(mtmp, SCR_GROWTH);
		(void) mongets(mtmp, SCR_GROWTH);
		(void) mongets(mtmp, SCR_GROWTH);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
	}

	if (ptr == &mons[PM_MISS_LOCKING_STABLE]) {
		(void) mongets(mtmp, POT_POLYMORPH);
		(void) mongets(mtmp, WAN_STRIKING);
		(void) mongets(mtmp, STEEL_WHIP);
	}

	if (ptr == &mons[PM_MISTER_BLACKMAN]) {
		(void) mongets(mtmp, BUGLE);
		(void) mongets(mtmp, BLACK_DRAGON_SCALE_MAIL);
		(void) mongets(mtmp, ORCISH_SHORT_SWORD);
		(void) mongets(mtmp, ORCISH_HELM);
		(void) mongets(mtmp, ORCISH_CHAIN_MAIL);
	}

	if (ptr == &mons[PM_MISTER_GREENLY]) {
		(void) mongets(mtmp, RUBBER_HOSE);
	}

	if (ptr == &mons[PM_DRIVING_IMPULSE]) {
		(void) mongets(mtmp, POT_SPEED);
		(void) mongets(mtmp, SCR_BAD_EFFECT);
	}

	if (ptr == &mons[PM_MOVING_TWITCH]) {
		(void) mongets(mtmp, MALLET);
		(void) mongets(mtmp, LEATHER_JACKET);
		(void) mongets(mtmp, POT_EXTRA_HEALING);
	}

	if (ptr == &mons[PM_MARIHUANA]) {
		(void) mongets(mtmp, POT_BLINDNESS);
		(void) mongets(mtmp, KNOUT);
		(void) mongets(mtmp, SCR_IMMOBILITY);
		(void) mongets(mtmp, POT_CYANIDE);
		(void) mongets(mtmp, WAN_SUMMON_UNDEAD);
		(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
		m_initthrow(mtmp, CREAM_PIE, 4);
	}

	if (ptr == &mons[PM_LEUTKAK]) {
		(void) mongets(mtmp, POT_SLIME);
		(void) mongets(mtmp, POISONOUS_CLOAK);
	}

	if (ptr == &mons[PM_HANS]) {
		(void) mongets(mtmp, LACQUERED_DANCING_SHOE);
		(void) mongets(mtmp, LEATHER_JACKET);
		(void) mongets(mtmp, SLING);
		(void) mongets(mtmp, SCR_GROUP_SUMMONING);
		m_initthrow(mtmp, INVENTORY_LOSS_STONE, 1);
		m_initthrow(mtmp, DROPBUG_STONE, 1);
		m_initthrow(mtmp, BAD_EFFECT_STONE, 1);
	}

	if (ptr == &mons[PM_GLOECKLE]) {
		(void) mongets(mtmp, BUGLE);
		(void) mongets(mtmp, POT_AMNESIA);
	}

	if (ptr == &mons[PM_MISTER_ADMONISHER]) {
		(void) mongets(mtmp, WAN_CANCELLATION);
	}

	if (ptr == &mons[PM_MISTER_HEAD]) {
		(void) mongets(mtmp, POT_SPEED);
		(void) mongets(mtmp, BASEBALL_BAT);
	}

	if (ptr == &mons[PM_DR__RAMOV]) {
		(void) mongets(mtmp, TWO_HANDED_SWORD);
		(void) mongets(mtmp, PLATE_MAIL);
		(void) mongets(mtmp, SCR_PUNISHMENT);
		(void) mongets(mtmp, POT_STUNNING);
		(void) mongets(mtmp, WAN_FINGER_BENDING);
	}

	if (ptr == &mons[PM_LEFT_ASS]) {
		(void) mongets(mtmp, WAN_DRAINING);
		(void) mongets(mtmp, SCR_MEGALOAD);
		(void) mongets(mtmp, WAN_STARVATION);
		(void) mongets(mtmp, WAN_REDUCE_MAX_HITPOINTS);
		(void) mongets(mtmp, GOLD_CHAIN);
	}

	if (ptr == &mons[PM_MISTER_STRIP_MINE]) {
		(void) mongets(mtmp, WAN_DIGGING);
		(void) mongets(mtmp, DWARVISH_MATTOCK);
		(void) mongets(mtmp, CLOAK_OF_MAGIC_RESISTANCE);
	}

	if (ptr == &mons[PM_PEDALAS]) {
		(void) mongets(mtmp, SCR_EARTH);
		(void) mongets(mtmp, SCR_EARTH);
		(void) mongets(mtmp, SCR_EARTH);
		(void) mongets(mtmp, WAN_STRIKING);
	}

	if (ptr == &mons[PM_GROSS_SANTANA]) {
		(void) mongets(mtmp, BUGLE);
		(void) mongets(mtmp, POT_PARALYSIS);
	}

	if (ptr == &mons[PM_DAD_MANGER]) {
		(void) mongets(mtmp, MACE);
		(void) mongets(mtmp, WAN_HEALING);
	}

	if (ptr == &mons[PM_DESERT_DWARF]) {
		(void) mongets(mtmp, FLAIL);
		(void) mongets(mtmp, WAN_STRIKING);
		(void) mongets(mtmp, LEATHER_JACKET);
	}

	if (ptr == &mons[PM_HAXXOR]) {
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, ELECTRIC_SWORD);
	}

	if (ptr == &mons[PM_AL_CAPONE]) {
		(void) mongets(mtmp, PISTOL);
		(void) mongets(mtmp, LEATHER_JACKET);
		(void) mongets(mtmp, ELVEN_CLOAK);
		(void) mongets(mtmp, WAN_TELEPORTATION);
		m_initthrow(mtmp, BULLET, 50);
	}

	if (ptr == &mons[PM_RUNNER]) {
		(void) mongets(mtmp, SCR_CHAOS_TERRAIN);
	}

	if (ptr == &mons[PM_GUNTER]) {
		(void) mongets(mtmp, POT_ACID);
		(void) mongets(mtmp, POT_RADIUM);
		(void) mongets(mtmp, POT_CYANIDE);
		(void) mongets(mtmp, POT_HALLUCINATION);
		(void) mongets(mtmp, POT_HALLUCINATION);
		(void) mongets(mtmp, LEATHER_GLOVES);
	}

	if (ptr == &mons[PM_MISTER_ALSO_MATTE]) {
		(void) mongets(mtmp, WAN_FIREBALL);
		(void) mongets(mtmp, LAB_COAT);
	}

	if (ptr == &mons[PM_MISTER_GARBAGE_HOUSES]) {
		(void) mongets(mtmp, SLING);
		(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
		m_initthrow(mtmp, CAPTCHA_STONE, 1);
		m_initthrow(mtmp, QUIZ_STONE, 1);
	}

	if (ptr == &mons[PM_HARRO]) {
		(void) mongets(mtmp, SCR_BULLSHIT);
		(void) mongets(mtmp, MUMMY_WRAPPING);
		(void) mongets(mtmp, IRON_CHAIN);
		(void) mongets(mtmp, WAN_SIN);
		(void) mongets(mtmp, HEAVY_IRON_BALL);
	}

	if (ptr == &mons[PM_BAERAN]) {
		(void) mongets(mtmp, RUBBER_HOSE);
		(void) mongets(mtmp, PISTOL);
		(void) mongets(mtmp, WAN_STUN_MONSTER);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		m_initthrow(mtmp, CREAM_PIE, 4);
		m_initthrow(mtmp, BULLET, 20);
	}

	if (ptr == &mons[PM_DICKMOUTH]) {
		(void) mongets(mtmp, POT_PARALYSIS);
	}

	if (ptr == &mons[PM_RAINI]) {
		(void) mongets(mtmp, WAR_HAMMER);
		(void) mongets(mtmp, CHAIN_MAIL);
		(void) mongets(mtmp, POT_ICE);
		(void) mongets(mtmp, POT_FIRE);
	}

	if (ptr == &mons[PM_MISTER_BE_SILENT_KERT]) {
		(void) mongets(mtmp, WAN_CREATE_MONSTER);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, SCR_LOCKOUT);
		(void) mongets(mtmp, WAN_SUMMON_ELM);
		(void) mongets(mtmp, SCR_SUMMON_ELM);
		(void) mongets(mtmp, POT_GAIN_LEVEL);
		(void) mongets(mtmp, RIN_TIMELY_BACKUP);
		(void) mongets(mtmp, CLOAK_OF_INVISIBILITY);
		(void) mongets(mtmp, ELVEN_MITHRIL_COAT);
		(void) mongets(mtmp, ELYSIUM_SCOURGE);
	}

	if (ptr == &mons[PM_CONSTI]) {
		(void) mongets(mtmp, POT_SLEEPING);
	}

	if (ptr == &mons[PM_SOUND_IMPACTS]) {
		(void) mongets(mtmp, BUGLE);
		(void) mongets(mtmp, MACE);
		m_initthrow(mtmp, DART, 50);
		m_initthrow(mtmp, DART, 50);
	}

	if (ptr == &mons[PM_SCHRUMPEL]) {
		(void) mongets(mtmp, WAN_CLONE_MONSTER);
	}

	if (ptr == &mons[PM_FAHRTUM]) {
		(void) mongets(mtmp, WAN_FIRE);
		(void) mongets(mtmp, WAN_COLD);
		(void) mongets(mtmp, CLOAK_OF_REFLECTION);
		(void) mongets(mtmp, SCR_HEALING);
		(void) mongets(mtmp, POT_POLYMORPH);
	}

	if (ptr == &mons[PM_RIVERMAN]) {
		(void) mongets(mtmp, KNOUT);
		(void) mongets(mtmp, POT_SPEED);
	}

	if (ptr == &mons[PM_MISTER_INTERNET]) {
		(void) mongets(mtmp, SCR_TELEPORTATION);
		(void) mongets(mtmp, SCR_WARPING);
		(void) mongets(mtmp, SCR_CREATE_MONSTER);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
	}

	if (ptr == &mons[PM_KOVVELKOV]) {
		(void) mongets(mtmp, CONCRETE_NUNCHIAKU);
		(void) mongets(mtmp, HEAVY_CONCRETE_BALL);
		(void) mongets(mtmp, CLOAK_OF_MAGIC_RESISTANCE);
		(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
	}

	if (ptr == &mons[PM_BAENDRE]) {
		(void) mongets(mtmp, WAN_POLYMORPH);
	}

	if (ptr == &mons[PM_MISTER_SIR]) {
		(void) mongets(mtmp, PISTOL);
		(void) mongets(mtmp, SCR_ENRAGE);
		(void) mongets(mtmp, PLASTEEL_CLOAK);
		(void) mongets(mtmp, WAN_SLOW_MONSTER);
		(void) mongets(mtmp, POT_EXTRA_HEALING);
		m_initthrow(mtmp, BULLET, 20);
	}

	if (ptr == &mons[PM_TELPAN]) {
		(void) mongets(mtmp, WAN_BANISHMENT);
	}

	if (ptr == &mons[PM_MISTER_ANCIENT_REGIME]) {
		(void) mongets(mtmp, POT_MUTATION);
	}

	if (ptr == &mons[PM_HIRSCHAH]) {
		(void) mongets(mtmp, BULLWHIP);
	}

	if (ptr == &mons[PM_OTTO]) {
		(void) mongets(mtmp, POT_SPEED);
		(void) mongets(mtmp, WAN_FEAR);
		(void) mongets(mtmp, IRON_CHAIN);
	}

	if (ptr == &mons[PM_HAIRBEARD]) {
		(void) mongets(mtmp, SCR_BAD_EFFECT);
		(void) mongets(mtmp, WAN_FIRE);
	}

	if (ptr == &mons[PM_WEITZER]) {
		(void) mongets(mtmp, SCR_GROWTH);
	}

	if (ptr == &mons[PM_SUPER_OLD_FRITZ]) {
		(void) mongets(mtmp, POT_SLEEPING);
		(void) mongets(mtmp, WAN_SLEEP);
		(void) mongets(mtmp, CLAY_CHAIN);
	}

	if (ptr == &mons[PM_FIVE_MAN]) {
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		(void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		(void) mongets(mtmp, GAUNTLETS_OF_TYPING);
	}

	if (ptr == &mons[PM_MISTER_WHO]) {
		(void) mongets(mtmp, POT_CONFUSION);
	}

	if (ptr == &mons[PM_FLAT_MAN]) {
		(void) mongets(mtmp, KATANA);
		(void) mongets(mtmp, SCR_SUMMON_BOSS);
	}

	if (ptr == &mons[PM_PRINCIPAL_NATURAL_GAS]) {
		(void) mongets(mtmp, DROVEN_BOW);
		(void) mongets(mtmp, MALLET);
		(void) mongets(mtmp, FULL_PLATE_MAIL);
		(void) mongets(mtmp, AMULET_OF_DATA_STORAGE);
		(void) mongets(mtmp, CLOAK_OF_MAGIC_RESISTANCE);
		(void) mongets(mtmp, HELM_OF_STEEL);
		(void) mongets(mtmp, PLASTEEL_GLOVES);
		(void) mongets(mtmp, STEEL_SHIELD);
		(void) mongets(mtmp, SCR_SUMMON_GHOST);
		(void) mongets(mtmp, SCR_RELOCATION);
		(void) mongets(mtmp, SCR_EGOISM);
		(void) mongets(mtmp, SPEED_BOOTS);
		(void) mongets(mtmp, WAN_DISINTEGRATION);
		(void) mongets(mtmp, WAN_TIME);
		(void) mongets(mtmp, WAN_CREATE_HORDE);
		m_initthrow(mtmp, DROVEN_ARROW, 50);
		m_initthrow(mtmp, DROVEN_ARROW, 50);
		m_initthrow(mtmp, DROVEN_ARROW, 50);
		m_initthrow(mtmp, DROVEN_ARROW, 50);
	}

	if (ptr == &mons[PM_THE_EXTRA_FLEECY_BUNDLE_HER_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_EMMELIE_S_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_LUISA_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_LEXI_S_WONDERFULLY_SOFT_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_REBECCA_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_ROUGH_TERESA_S_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_JANINE_S_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_BITCHY_LARA_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_MARLEEN_S_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_MARLEEN_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_NONEROTIC_IRINA_S_WEDGE_SANDAL]) {
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	if (ptr == &mons[PM_BUNDLY_ANN_S_SOFT_SANDAL]) {
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	if (ptr == &mons[PM_LISELOTTE_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_LISELOTTE_S_SOFT_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_MAY_BRITT_S_FLUFFY_SANDAL]) {
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	if (ptr == &mons[PM_ROXY_GRETA_S_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_THE_HIGH_HEEL_LOVING_ASIAN_GIRL_HER_HEELS]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);

	}

	if (ptr == &mons[PM_ALIDA_S_COLORFUL_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_JOSEFINE_S_SUPER_SWEET_VELCRO_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_VILEA_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_AMELJE_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_AMELJE_S_SANDAL]) {
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	if (ptr == &mons[PM_MELANIE_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_THE_HUGGING_TOPMODEL_HER_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_FANNY_S_VELCRO_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_FANNY_S_BRAND_NEW_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_FANNY_S_BUCKLED_SANDAL]) {
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	if (ptr == &mons[PM_BUNDLE_NADJA_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_MAGDALENA_S_DANCING_SHOE]) {
		(void) mongets(mtmp, DANCING_SHOES);

	}

	if (ptr == &mons[PM_MAILIE_S_STILETTO]) {
		(void) mongets(mtmp, HIGH_STILETTOS);
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);

	}

	if (ptr == &mons[PM_MAILIE_S_BEGUILING_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_JULCHEN_S_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_SABRINA_S_PLATEAU_BOOT]) {
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);

	}

	if (ptr == &mons[PM_SOLVEJG_S_OTHER_MOCASSIN]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);

	}

	if (ptr == &mons[PM_THAI_S_HIGH_HEELED_LADY_SHOE]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);

	}

	if (ptr == &mons[PM_NADJA_S_THICK_BOOT]) {
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);

	}

	if (ptr == &mons[PM_RITA_S_SEXY_SANDAL]) {
		(void) mongets(mtmp, HIGH_STILETTOS);
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);

	}

	if (ptr == &mons[PM_KATI_S_HIGH_HEELED_SANDAL]) {
		(void) mongets(mtmp, HIGH_STILETTOS);
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);

	}

	if (ptr == &mons[PM_JOHANETTA_S_ROUGH_SOLE_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_JOHANETTA_S_DANCING_SHOE]) {
		(void) mongets(mtmp, DANCING_SHOES);

	}

	if (ptr == &mons[PM_KLARA_S_POINTY_COMBAT_BOOT]) {
		(void) mongets(mtmp, SPIKED_BATTLE_BOOT);
		(void) mongets(mtmp, COMBAT_STILETTOS);

	}

	if (ptr == &mons[PM_SANDRA_S_PLATEAU_BOOT]) {
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);

	}

	if (ptr == &mons[PM_LARISSA_S_SOFT_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_LARISSA_S_BLOCK_HEELED_BOOT]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

	}

	if (ptr == &mons[PM_SUESCHEN_S_FEMININE_SANDAL]) {
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	if (ptr == &mons[PM_ANTONIA_S_LEATHER_PUMP]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);

	}

	if (ptr == &mons[PM_CHASKA_S_LEATHER_PUMP]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);

	}

	if (ptr == &mons[PM_LUDGERA_S_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_LAURA_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_LAURA_S_CUDDLY_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_MARIAN_S_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_MARIAN_S_PERSIAN_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_JIL_S_VELCRO_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_PATRICIA_S_COMBAT_BOOT]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

	}

	if (ptr == &mons[PM_PATRICIA_S_WORN_OUT_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_HANH_S_LITTLE_GIRL_SANDAL]) {
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	if (ptr == &mons[PM_HANH_S_BLOCK_HEELED_SANDAL]) {
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);

	}

	if (ptr == &mons[PM_HANH_S_SEXY_PUMP]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);

	}

	if (ptr == &mons[PM_HANH_S_TRANSLUCENT_SANDAL]) {
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	if (ptr == &mons[PM_HANH_S_BALLERINA]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);

	}

	if (ptr == &mons[PM_ANTJE_S_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_ANTJE_S_BLOCK_HEELED_BOOT]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

	}

	if (ptr == &mons[PM_ELKE_S_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_NICOLE_S_BLOCK_HEELED_COMBAT_BOOT]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

	}

	if (ptr == &mons[PM_NICOLE_S_LOVELY_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_BETTINA_S_COMBAT_BOOT]) {
		(void) mongets(mtmp, SPIKED_BATTLE_BOOT);

	}

	if (ptr == &mons[PM_HEIKE_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_DORA_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_DORA_S_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_RUTH_S_BLOCK_HEELED_LADY_BOOT]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

	}

	if (ptr == &mons[PM_RUTH_S_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_LOU_S_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_DANIELLE_S_HIGH_HEELED_SANDAL]) {
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, HIGH_STILETTOS);

	}

	if (ptr == &mons[PM_DANIELLE_S_EROTIC_LADY_SHOE]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);

	}

	if (ptr == &mons[PM_SOFIA_S_LEATHER_PUMP]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);

	}

	if (ptr == &mons[PM_ANN_KATHRIN_S_GIRL_SHOE]) {
		(void) mongets(mtmp, WOODEN_GETA);

	}

	if (ptr == &mons[PM_NJUSHA_S_SWEET_SANDAL]) {
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	if (ptr == &mons[PM_ELIANE_S_SHIN_SMASHING_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_LISA_S_YELLOW_STILETTO]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, HIGH_STILETTOS);

	}

	if (ptr == &mons[PM_LISA_S_PURPLE_STILETTO]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, HIGH_STILETTOS);

	}

	if (ptr == &mons[PM_LISA_S_HIGH_HEELED_SANDAL]) {
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, HIGH_STILETTOS);

	}

	if (ptr == &mons[PM_LISA_S_GIRL_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_LISA_S_BLOCK_HEELED_COMBAT_BOOT]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

	}

	if (ptr == &mons[PM_ARABELLA_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_ARABELLA_S_SOFT_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_ARABELLA_S_HIGH_HEELED_LADY_SHOE]) {
		(void) mongets(mtmp, LACQUERED_DANCING_SHOE);

	}

	if (ptr == &mons[PM_HENRIETTA_S_THICK_BLOCK_HEELED_BOOT]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

	}

	if (ptr == &mons[PM_KATRIN_S_COMBAT_BOOT]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

	}

	if (ptr == &mons[PM_JANA_S_SOFT_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_YASAMAN_S_LADY_PUMP]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);

	}

	if (ptr == &mons[PM_ALMUTH_S_BLOCK_HEELED_BOOT]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

	}

	if (ptr == &mons[PM_KSENIA_S_STILETTO_BOOT]) {
		(void) mongets(mtmp, SPIKED_BATTLE_BOOT);
		(void) mongets(mtmp, COMBAT_STILETTOS);

	}

	if (ptr == &mons[PM_ELENA_S_PLATEAU_BOOT]) {
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);

	}

	if (ptr == &mons[PM_ELENA_S_PLATFORM_SANDAL]) {
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	if (ptr == &mons[PM_INGE_S_COMBAT_BOOT]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

	}

	if (ptr == &mons[PM_HELEN_S_LADY_PUMP]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);

	}

	if (ptr == &mons[PM_KRISTIN_S_BLOCK_HEELED_COMBAT_BOOT]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

	}

	if (ptr == &mons[PM_KRISTIN_S_SUPER_HIGH_LADY_SHOE]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);

	}

	if (ptr == &mons[PM_KRISTIN_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_KRISTIN_S_SOFT_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_KRISTIN_S_BLOCK_HEELED_SANDAL]) {
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);

	}

	if (ptr == &mons[PM_KRISTIN_S_LACQUERED_NOBLE_SANDAL]) {
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	if (ptr == &mons[PM_KRISTIN_S_PLATEAU_BOOT]) {
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);

	}

	if (ptr == &mons[PM_VERA_S_ICE_COLD_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_NORA_S_LOVELY_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_GIULIA_S_DANCING_SHOE]) {
		(void) mongets(mtmp, DANCING_SHOES);

	}

	if (ptr == &mons[PM_SARAH_S_SOFT_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_SARAH_S_MOCASSIN]) {
		(void) mongets(mtmp, SWEET_MOCASSINS);

	}

	if (ptr == &mons[PM_IRIS_S_SILVER_PUMP]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);

	}

	if (ptr == &mons[PM_ALEXIA_S_SOFT_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_BARBARA_S_LOVELY_SANDAL]) {
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	if (ptr == &mons[PM_ROSWITHA_S_LACQUERED_PUMP]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);

	}

	if (ptr == &mons[PM_CHARLOTTE_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_FRIEDERIKE_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_SOPHIA_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_SUE_LYN_S_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_JUEN_S_BLOCK_HEELED_COMBAT_BOOT]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

	}

	if (ptr == &mons[PM_JUEN_S_PLATEAU_BOOT]) {
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);

	}

	if (ptr == &mons[PM_JUEN_S_FRIGHTENING_BATHING_SANDAL]) {
		(void) mongets(mtmp, WOODEN_GETA);
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	if (ptr == &mons[PM_JUEN_S_WEDGE_SANDAL]) {
		(void) mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	if (ptr == &mons[PM_JUEN_S_WOODEN_SANDAL]) {
		(void) mongets(mtmp, WOODEN_GETA);
		(void) mongets(mtmp, HIGH_STILETTOS);

	}

	if (ptr == &mons[PM_JUEN_S_SEXY_LEATHER_PUMP]) {
		(void) mongets(mtmp, FEMININE_PUMPS);
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);

	}

	if (ptr == &mons[PM_RUEA_S_PLATEAU_BOOT]) {
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);

	}

	if (ptr == &mons[PM_RUEA_S_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);
		(void) mongets(mtmp, SOFT_GIRL_SNEAKER);

	}

	if (ptr == &mons[PM_RUEA_S_BLOCK_HEELED_COMBAT_BOOT]) {
		(void) mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);

	}

	if (ptr == &mons[PM_RUEA_S_NOBLE_SANDAL]) {
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, HIGH_STILETTOS);

	}

	if (ptr == &mons[PM_KATIA_S_LOVELY_SNEAKER]) {
		(void) mongets(mtmp, SOFT_SNEAKERS);

	}

	if (ptr == &mons[PM_GUDRUN_S_WEDGE_SANDAL]) {
		(void) mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	if (ptr == &mons[PM_NATASCHA_S_PLATEAU_BOOT]) {
		(void) mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);

	}

	if (ptr == &mons[PM_JANE_S_DANCING_SHOE]) {
		(void) mongets(mtmp, DANCING_SHOES);

	}

	if (ptr == &mons[PM_LITTLE_MARIE_S_HUGGING_BOOT]) {
		(void) mongets(mtmp, HUGGING_BOOT);

	}

	if (ptr == &mons[PM_BIANCA_S_HIGH_HEELED_SANDAL]) {
		(void) mongets(mtmp, HIGH_HEELED_SANDAL);
		(void) mongets(mtmp, HIGH_STILETTOS);

	}

	if (ptr == &mons[PM_HILDA_S_LADY_PUMP]) {
		(void) mongets(mtmp, SEXY_LEATHER_PUMP);
		(void) mongets(mtmp, FEMININE_PUMPS);

	}

	if (ptr == &mons[PM_SING_S_JAPANESE_GETA]) {
		(void) mongets(mtmp, WOODEN_GETA);
		(void) mongets(mtmp, WEDGE_SANDALS);

	}

	/* "evil" patch idea: 50% of the time, quest leader gets a kyrt shirt that can be enchanted like elven armor.
	 * I guess the evil part about it is that you'll have to kill your leader if you want it. --Amy */
	if (!rn2(2) && ptr->msound == MS_LEADER) (void) mongets(mtmp, KYRT_SHIRT);

	/* ordinary soldiers rarely have access to magic (or gold :-) */
	if (ptr == &mons[PM_SOLDIER] && rn2(15)) return;


#ifndef GOLDOBJ
	if (likes_gold(ptr) && !mtmp->mgold && !rn2(5))
		mtmp->mgold =
		      (long) d(level_difficulty(), mtmp->minvent ? 5 : 10);
#else
	if (likes_gold(ptr) && !findgold(mtmp->minvent) && !rn2(5))
		mkmonmoney(mtmp, (long) d(level_difficulty(), mtmp->minvent ? 5 : 10));
#endif
}

/* Note: for long worms, always call cutworm (cutworm calls clone_mon) */
struct monst *
clone_mon(mon, x, y)
struct monst *mon;
xchar x, y;	/* clone's preferred location or 0 (near mon) */
{
	coord mm;
	struct monst *m2;

	if (mon->data->mlet == S_TROVE) /* way too much potential for abuse --Amy */
	    return (struct monst *)0;

	/* may be too weak or have been extinguished for population control */
	if (mon->mhp <= 1 || ((mvitals[monsndx(mon->data)].mvflags & G_EXTINCT) && !(mon->data == &mons[PM_JUBILEX]) && !(mon->data == &mons[PM_SCHRUMPEL]) ) )
	    return (struct monst *)0;

	if (x == 0) {
	    mm.x = mon->mx;
	    mm.y = mon->my;
	    if (!enexto(&mm, mm.x, mm.y, mon->data) || MON_AT(mm.x, mm.y))
		return (struct monst *)0;
	} else if (!isok(x, y)) {
	    return (struct monst *)0;	/* paranoia */
	} else {
	    mm.x = x;
	    mm.y = y;
	    if (MON_AT(mm.x, mm.y)) {
		if (!enexto(&mm, mm.x, mm.y, mon->data) || MON_AT(mm.x, mm.y))
		    return (struct monst *)0;
	    }
	}
	m2 = newmonst(0);
	*m2 = *mon;			/* copy condition of old monster */
	m2->nmon = fmon;
	fmon = m2;
	m2->m_id = flags.ident++;
	if (!m2->m_id) m2->m_id = flags.ident++;	/* ident overflowed */
	m2->mx = mm.x;
	m2->my = mm.y;

	m2->minvent = (struct obj *) 0; /* objects don't clone */
	m2->mleashed = FALSE;
#ifndef GOLDOBJ
	m2->mgold = 0L;
#endif
	/* Max HP the same, but current HP halved for both.  The caller
	 * might want to override this by halving the max HP also.
	 * When current HP is odd, the original keeps the extra point.
	 */
	m2->mhpmax = mon->mhpmax;
	m2->mhp = mon->mhp / 2;
	mon->mhp -= m2->mhp;

	/* Same for the power */
	m2->m_enmax = mon->m_enmax;
	m2->m_en = mon->m_en /= 2;

	/* since shopkeepers and guards will only be cloned if they've been
	 * polymorphed away from their original forms, the clone doesn't have
	 * room for the extra information.  we also don't want two shopkeepers
	 * around for the same shop.
	 */
	if (mon->isshk) m2->isshk = FALSE;
	if (mon->isgd) m2->isgd = FALSE;
	if (mon->ispriest) m2->ispriest = FALSE;
	if (mon->isgyp) m2->isgyp = FALSE;
	m2->mxlth = 0;
	place_monster(m2, m2->mx, m2->my);
	if (emits_light(m2->data))
	    new_light_source(m2->mx, m2->my, emits_light(m2->data),
			     LS_MONSTER, (genericptr_t)m2);
	if (m2->mnamelth) {
	    m2->mnamelth = 0; /* or it won't get allocated */
	    m2 = christen_monst(m2, NAME(mon));
	} else if (mon->isshk) {
	    m2 = christen_monst(m2, shkname(mon));
	}

	/* not all clones caused by player are tame or peaceful */
	if (!flags.mon_moving) {
	    if (mon->mtame)
		m2->mtame = rn2(max(2 + u.uluck, 2)) ? mon->mtame : 0;
	    else if (mon->mpeaceful)
		m2->mpeaceful = rn2(max(2 + u.uluck, 2)) ? 1 : 0;
	}

	newsym(m2->mx,m2->my);	/* display the new monster */
	if (m2->mtame) {
	    struct monst *m3;

	    if (mon->isminion) {
		m3 = newmonst(sizeof(struct epri) + mon->mnamelth);
		*m3 = *m2;
		m3->mxlth = sizeof(struct epri);
		if (m2->mnamelth) Strcpy(NAME(m3), NAME(m2));
		*(EPRI(m3)) = *(EPRI(mon));
		replmon(m2, m3);
		m2 = m3;
	    } else {
		/* because m2 is a copy of mon it is tame but not init'ed.
		 * however, tamedog will not re-tame a tame dog, so m2
		 * must be made non-tame to get initialized properly.
		 */
		m2->mtame = 0;
		if ((m3 = tamedog(m2, (struct obj *)0, FALSE)) != 0) {
		    m2 = m3;
		    *(EDOG(m2)) = *(EDOG(mon));
		}
	    }
	}
	set_malign(m2);

	return m2;
}

/*
 * Propagate a species
 *
 * Once a certain number of monsters are created, don't create any more
 * at random (i.e. make them extinct).  The previous (3.2) behavior was
 * to do this when a certain number had _died_, which didn't make
 * much sense.
 *
 * Returns FALSE propagation unsuccessful
 *         TRUE  propagation successful
 */
boolean
propagate(mndx, tally, ghostly)
int mndx;
boolean tally;
boolean ghostly;
{
	boolean result;
	uchar lim = mbirth_limit(mndx);
	boolean gone = (mvitals[mndx].mvflags & G_GONE); /* genocided or extinct */

	result = (((int) mvitals[mndx].born < lim) && !gone) ? TRUE : FALSE;

	/* if it's unique, don't ever make it again */
	if (mons[mndx].geno & G_UNIQ) mvitals[mndx].mvflags |= G_EXTINCT;

	if (mvitals[mndx].born < 255 && tally && (!ghostly || (ghostly && result)))
		 mvitals[mndx].born++;
	if ((int) mvitals[mndx].born >= lim && !(mons[mndx].geno & G_NOGEN) &&
		!(mvitals[mndx].mvflags & G_EXTINCT)) {
#if defined(DEBUG) && defined(WIZARD)
		if (wizard) pline("Automatically extinguished %s.",
					makeplural(mons[mndx].mname));
#endif
		mvitals[mndx].mvflags |= G_EXTINCT;
		reset_rndmonst(mndx);
	}
	return result;
}

/*
 * called with [x,y] = coordinates;
 *	[0,0] means anyplace
 *	[u.ux,u.uy] means: near player (if !in_mklev)
 *
 *	In case we make a monster group, only return the one at [x,y].
 */
struct monst *
makemon(ptr, x, y, mmflags)
register struct permonst *ptr;
register int	x, y;
register int	mmflags;
{
	register struct monst *mtmp;
	int mndx, mcham, ct, mitem, xlth, senserchance;
	boolean anymon = (!ptr);
	boolean byyou = (x == u.ux && y == u.uy);
	boolean allow_minvent = ((((mmflags & NO_MINVENT) == 0) || ptr == &mons[PM_HUGE_OGRE_THIEF]) && ptr != &mons[PM_HOLE_MASTER] && ptr != &mons[PM_TRAP_MASTER] && ptr != &mons[PM_BOULDER_MASTER] && ptr != &mons[PM_ITEM_MASTER] && ptr != &mons[PM_GOOD_ITEM_MASTER] && ptr != &mons[PM_BAD_ITEM_MASTER]);
	boolean allow_special = ((mmflags & MM_NOSPECIALS) == 0);
	boolean countbirth = ((mmflags & MM_NOCOUNTBIRTH) == 0);
	unsigned gpflags = (mmflags & MM_IGNOREWATER) ? MM_IGNOREWATER : 0;
	int randsp;
	int i;
	coord cc;

	/* if caller wants random location, do it here */
	if(x == 0 && y == 0) {
		int tryct = 0;	/* careful with bigrooms */
		struct monst fakemon;

		fakemon.data = ptr;	/* set up for goodpos */
		do {
			x = rn1(COLNO-3,2);
			y = rn2(ROWNO);
		} while(!goodpos(x, y, ptr ? &fakemon : (struct monst *)0, gpflags) ||
			(!in_mklev && tryct++ < 50 && cansee(x, y)));

	} else if (byyou && !in_mklev) {
		coord bypos;

		if(enexto_core(&bypos, u.ux, u.uy, ptr, gpflags)) {
			x = bypos.x;
			y = bypos.y;
		} else
			return((struct monst *)0);
	}

	/* Does monster already exist at the position? */
	if(MON_AT(x, y)) {
		if ((mmflags & MM_ADJACENTOK) != 0) {
			coord bypos;
			if(enexto_core(&bypos, x, y, ptr, gpflags)) {
				x = bypos.x;
				y = bypos.y;
			} else
				return((struct monst *) 0);
		} else 
			return((struct monst *) 0);
	}

	if(ptr){
		mndx = monsndx(ptr);
		/* if you are to make a specific monster and it has
		   already been genocided, return */
		if (mvitals[mndx].mvflags & G_GENOD) return((struct monst *) 0);
#if defined(WIZARD) && defined(DEBUG)
		if (wizard && (mvitals[mndx].mvflags & G_EXTINCT))
		    pline("Explicitly creating extinct monster %s.",
			mons[mndx].mname);
#endif
	} else {
		/* make a random (common) monster that can survive here.
		 * (the special levels ask for random monsters at specific
		 * positions, causing mass drowning on the medusa level,
		 * for instance.)
		 */
		int tryct = 0;	/* maybe there are no good choices */
		struct monst fakemon;
		do {
			if(!(ptr = rndmonst())) {
#ifdef DEBUG
			    pline("Warning: no monster.");
#endif
			    return((struct monst *) 0);	/* no more monsters! */
			}
			fakemon.data = ptr;	/* set up for goodpos */
		} while(!goodpos(x, y, &fakemon, gpflags) && tryct++ < 50);
		mndx = monsndx(ptr);
	}

	(void) propagate(mndx, countbirth, FALSE);
	xlth = ptr->pxlth;
	if (mmflags & MM_EDOG) xlth += sizeof(struct edog);
	else if (mmflags & MM_EMIN) xlth += sizeof(struct emin);
	mtmp = newmonst(xlth);
	*mtmp = zeromonst;		/* clear all entries in structure */
	(void)memset((genericptr_t)mtmp->mextra, 0, xlth);
	mtmp->nmon = fmon;
	fmon = mtmp;
	mtmp->m_id = flags.ident++;
	if (!mtmp->m_id) mtmp->m_id = flags.ident++;	/* ident overflowed */
	set_mon_data(mtmp, ptr, 0);
	if (mtmp->data->msound == MS_LEADER)
	    quest_status.leader_m_id = mtmp->m_id;
	mtmp->mxlth = xlth;
	mtmp->mnum = mndx;

	mtmp->m_lev = adj_lev(ptr);
	
	/* WAC set oldmonnm */
	mtmp->oldmonnm = monsndx(ptr);
	
	if (ptr >= &mons[PM_ARCHEOLOGIST] && ptr <= &mons[PM_WIZARD]) {
	   /* enemy characters are of varying level */
	   int base_you, base_lev;
	   base_you = (u.ulevel / 2)+1;
	   base_lev = level_difficulty()+1;
	   if (base_you < 1) base_you = 1;
	   if (base_lev < 1) base_lev = 1;
	   mtmp->m_lev = (1 + rn2(base_you) + rn2(base_lev) / 2)+1;
	}

	if (ptr == &mons[PM_BLACKY]) {
	   /* blacky too --Amy */
	   int base_you, base_lev;
	   base_you = (u.ulevel / 2)+1;
	   base_lev = level_difficulty()+1;
	   if (base_you < 1) base_you = 1;
	   if (base_lev < 1) base_lev = 1;
	   mtmp->m_lev = (1 + rn2(base_you) + rn2(base_lev) / 2)+1;
	}

	/* randomly make higher-level monsters for doppelgangers since the game is easy enough for them already --Amy */
	if (mtmp->m_lev && (mtmp->m_lev < 50) && !rn2(10) && Race_if(PM_DOPPELGANGER)) {

		mtmp->m_lev += rnd(u.ulevel);
		if (mtmp->m_lev > 49) mtmp->m_lev = 49;
	}

	if (mtmp->m_lev && (mtmp->m_lev < 50) && !rn2(10) && Role_if(PM_SHAPESHIFTER)) {

		mtmp->m_lev += rnd(u.ulevel);
		if (mtmp->m_lev > 49) mtmp->m_lev = 49;
	}

	if (LevelTrapEffect || u.uprops[LEVELBUG].extrinsic || have_levelstone()) {

		mtmp->m_lev += rnd(mvitals[mtmp->mnum].born + 1);
		if (mtmp->m_lev > 49) mtmp->m_lev = 49;

	}

	/* Set HP, HPmax */	
	if (is_golem(ptr)) {
	    mtmp->mhpmax = mtmp->mhp = golemhp(mndx);

	    if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "celtic helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "kel'tskaya shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "seltik dubulg'a") ) ) {
		    mtmp->mhpmax *= 3;
		    mtmp->mhpmax /= 2;
		    mtmp->mhp *= 3;
		    mtmp->mhp /= 2;

	    }

	} else if (mndx == PM_CRITICALLY_INJURED_THIEF || mndx == PM_CRITICALLY_INJURED_JEDI) { 
		/* can be killed in a single hit --Amy */
	    mtmp->mhpmax = mtmp->mhp = 1;
	} else if (is_rider(ptr) || is_deadlysin(ptr) ) {
	    /* We want low HP, but a high mlevel so they can attack well */
		mtmp->mhpmax = mtmp->mhp = d(10,8) + 20 + ptr->mlevel;
	} /*else if (ptr->mlevel > 49) {*/	/* from now on we'll just let their hp be calculated normally --Amy */
	    /* "special" fixed hp monster
	     * the hit points are encoded in the mlevel in a somewhat strange
	     * way to fit in the 50..127 positive range of a signed character
	     * above the 1..49 that indicate "normal" monster levels */
	    /*mtmp->mhpmax = mtmp->mhp = 5*(ptr->mlevel - 6);*/ /*hp increase --Amy */
	    /*mtmp->m_lev = mtmp->mhp / 4;*/	/* approximation */
	/*}*/ else if (ptr->mlet == S_DRAGON && mndx >= PM_GRAY_DRAGON) {
	    /* adult dragons */
	    mtmp->mhpmax = mtmp->mhp = (int) (In_endgame(&u.uz) ?
		(8 * mtmp->m_lev) : (4 * mtmp->m_lev + d((int)mtmp->m_lev, 4)));
	} else if (!mtmp->m_lev) {
	    mtmp->mhpmax = mtmp->mhp = rnd(4);
	} else {
	    mtmp->mhpmax = mtmp->mhp = d((int)mtmp->m_lev, 8);
	    
	    if (is_home_elemental(ptr))
		mtmp->mhpmax = (mtmp->mhp *= 3);
	    else mtmp->mhpmax = mtmp->mhp = 
		d((int)mtmp->m_lev, 8) + (mtmp->m_lev*rnd(2));
	}

	if (ptr->mlevel > 49) { /* so they still get extra HP --Amy */

		mtmp->mhpmax += ( (ptr->mlevel - 49) * 5);
		mtmp->mhp = mtmp->mhpmax;

	}

	if (ptr == &mons[PM__S_SECRET_CAR] || ptr == &mons[PM_OAK_SAGE] || ptr == &mons[PM__S_____NIX] || ptr == &mons[PM_TANK_WARRIOR] || ptr == &mons[PM__S_RAT_MAN] || ptr == &mons[PM__S_UFO] ) { /* supposed to be extra annoying --Amy */
		mtmp->mhpmax *= 5;
		mtmp->mhp *= 5;

	}

	if (ptr->geno & G_UNIQ) {
		if (rn2(5)) mtmp->mhpmax += rnd(mtmp->mhpmax);
		else if (rn2(5)) mtmp->mhpmax += rnd(mtmp->mhpmax * 2);
		else if (rn2(5)) mtmp->mhpmax += rnd(mtmp->mhpmax * 3);
		else mtmp->mhpmax += rnd(mtmp->mhpmax * 4);
		if (!rn2(10)) {
			mtmp->mhpmax /= 2;
			if (mtmp->mhpmax < 2) mtmp->mhpmax = 2;
		}
		mtmp->mhp = mtmp->mhpmax;
	}

	if (Role_if(PM_CAMPERSTRIKER) && (ptr->geno & G_UNIQ) ) {
		if (rn2(5)) mtmp->mhpmax += rnd(mtmp->mhpmax);
		else if (rn2(5)) mtmp->mhpmax += rnd(mtmp->mhpmax * 2);
		else if (rn2(5)) mtmp->mhpmax += rnd(mtmp->mhpmax * 3);
		else mtmp->mhpmax += rnd(mtmp->mhpmax * 4);
		if (!rn2(10)) {
			mtmp->mhpmax /= 2;
			if (mtmp->mhpmax < 2) mtmp->mhpmax = 2;
		}
		mtmp->mhp = mtmp->mhpmax;
	}

	if (ptr == &mons[PM_DARK_GOKU] || ptr == &mons[PM_FRIEZA]) { /* credits go to Bug Sniper for this idea --Amy */
		mtmp->mhpmax += 9000;
		mtmp->mhp += 9000;
	}

	/* Assign power */
	/*if (mindless(ptr)) {
	    mtmp->m_enmax = mtmp->m_en = 0;
	} else {*/
	    /* This is actually quite similar to hit dice,  
	     * but with more randomness 
	     * Mindless monsters may do some spellcasting now, mainly for sephirahs --Amy
	     */
	    mtmp->m_enmax = mtmp->m_en =
		d((int)mtmp->m_lev * 2, 4) + (mtmp->m_lev*rnd(2));
		if (mindless(ptr)) {mtmp->m_enmax = mtmp->m_enmax / 2; mtmp->m_en = mtmp->m_en / 2; }
	/*}*/
	if (is_female(ptr)) mtmp->female = TRUE;
	else if (is_male(ptr)) mtmp->female = FALSE;
	else mtmp->female = rn2(2);	/* ignored for neuters */

	if (In_sokoban(&u.uz) && !mindless(ptr))  /* know about traps here */
	    mtmp->mtrapseen = (1L << (PIT - 1)) | (1L << (HOLE - 1));
	if (ptr->msound == MS_LEADER)		/* leader knows about portal */
	    mtmp->mtrapseen |= (1L << (MAGIC_PORTAL-1));

	place_monster(mtmp, x, y);
	mtmp->mcansee = mtmp->mcanmove = TRUE;
	mtmp->mpeaceful = (mmflags & MM_ANGRY) ? FALSE : peace_minded(ptr);
	mtmp->mtraitor  = FALSE;

	/* Everything that can hide under an object will now do so. --Amy */
      if(x && y && !issoviet && allow_special && (hides_under(ptr) || !rn2(100) ) ) { /* low chance of getting an object even if nonhiding, too */

	  if (rn2(3) && (rn2(100) > u.concealitemchance))
		(void) mkobj_at(0, x, y, TRUE);
	  else
		(void) mkobj_at(COIN_CLASS, x, y, TRUE);
	}

		/* and even lower chance to get extra objects */
	if (!rn2(200) && allow_special) {
	  (void) mkobj_at(0, x, y, TRUE);
	  if (!rn2(3)) (void) mkobj_at(0, x, y, TRUE);
	}
	if (!rn2(400) && allow_special) {
	  (void) mkobj_at(0, x, y, TRUE);
	  if (!rn2(3)) (void) mkobj_at(0, x, y, TRUE);
	  if (!rn2(9)) (void) mkobj_at(0, x, y, TRUE);
	}
	if (!rn2(800) && allow_special) {
	  (void) mkobj_at(0, x, y, TRUE);
	  if (!rn2(3)) (void) mkobj_at(0, x, y, TRUE);
	  if (!rn2(9)) (void) mkobj_at(0, x, y, TRUE);
	  if (!rn2(27)) (void) mkobj_at(0, x, y, TRUE);
	}
	if (!rn2(1600) && allow_special) {
	  (void) mkobj_at(0, x, y, TRUE);
	  if (!rn2(3)) (void) mkobj_at(0, x, y, TRUE);
	  if (!rn2(9)) (void) mkobj_at(0, x, y, TRUE);
	  if (!rn2(27)) (void) mkobj_at(0, x, y, TRUE);
	  if (!rn2(81)) (void) mkobj_at(0, x, y, TRUE);
	}

	/* maybe generate a sleeping monster */

	if (!rn2(Aggravate_monster ? 500 : 100) && !u.uhave.amulet) mtmp->msleeping = 1;

	/* or an invisible one, based on an evil patch idea by jonadab */

	if (!rn2(250)) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}

	if (ptr == &mons[PM_SCROLLER_MASTER] && x && y && isok(x, y) && !(t_at(x, y)) )
		(void) maketrap(x, y, ACTIVE_SUPERSCROLLER_TRAP, 0);

	if (ptr == &mons[PM_BOULDER_MASTER] && x && y && isok(x, y) && !(t_at(x, y)) )
		(void) mksobj_at(BOULDER, x, y, TRUE, FALSE);

	if (ptr == &mons[PM_GOOD_ITEM_MASTER] && x && y && isok(x, y) )
		(void) mksobj_at(usefulitem(), x, y, TRUE, FALSE);

	if (ptr == &mons[PM_BAD_ITEM_MASTER] && x && y && isok(x, y) )
		(void) mksobj_at(nastymusableitem(), x, y, TRUE, FALSE);

	if (ptr == &mons[PM_HOLE_MASTER] && x && y && isok(x, y) && !(t_at(x, y)) )
		(void) maketrap(x, y, HOLE, 10);

	if (ptr == &mons[PM_TRAP_MASTER] && x && y && isok(x, y) && !(t_at(x, y)) )
		(void) maketrap(x, y, randomtrap(), 100 );

	/* maybe make a random trap underneath the monster, higher chance for drow to make it harder for them --Amy */

	if (!rn2( (Race_if(PM_DROW) ? 100 : 500) ) && allow_special && x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y))  ) {
		int rtrap;

		rtrap = randomtrap();

		(void) maketrap(x, y, rtrap, 100);

	}

	if (!rn2(100) && (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "eldritch cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sverkh'yestestvennyy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "aql bovar qilmaydigan plash") ))) {
		mtmp->isegotype = 1;
		mtmp->egotype_abomination = 1;
	}

	if (((!rn2(isxrace ? 30 : 100) ) || (!(u.monstertimefinish % 337) && !rn2(isxrace ? 10 : 40) ) || (!(u.monstertimefinish % 3217) && !rn2(isxrace ? 4 : 15) ) ) || always_egotype(mtmp->data) ) {

		mtmp->isegotype = 1;
		switch (rnd(144)) {
			case 1:
			case 2:
			case 3: mtmp->egotype_thief = 1; break;
			case 4: mtmp->egotype_wallwalk = 1; break;
			case 5: mtmp->egotype_disenchant = 1; break;
			case 6:
			case 7: mtmp->egotype_rust = 1; break;
			case 8: 
			case 9: mtmp->egotype_corrosion = 1; break;
			case 10: 
			case 11: mtmp->egotype_decay = 1; break;
			case 12: mtmp->egotype_wither = 1; break;
			case 13: 
			case 14: 
			case 15: mtmp->egotype_grab = 1; break;
			case 16: 
			case 17: mtmp->egotype_flying = 1; break;
			case 18: 
			case 19: mtmp->egotype_hide = 1; break;
			case 20: 
			case 21: 
			case 22: mtmp->egotype_regeneration = 1; break;
			case 23: 
			case 24: 
			case 25: mtmp->egotype_undead = 1; break;
			case 26: mtmp->egotype_domestic = 1; break;
			case 27: mtmp->egotype_covetous = 1; break;
			case 28: 
			case 29: mtmp->egotype_avoider = 1; break;
			case 30: mtmp->egotype_petty = 1; break;
			case 31: mtmp->egotype_pokemon = 1; break;
			case 32: mtmp->egotype_slows = 1; break;
			case 33: mtmp->egotype_vampire = 1; break;
			case 34: mtmp->egotype_teleportself = 1; break;
			case 35: mtmp->egotype_teleportyou = 1; break;
			case 36: 
			case 37: mtmp->egotype_wrap = 1; break;
			case 38: mtmp->egotype_disease = 1; break;
			case 39: mtmp->egotype_slime = 1; break;
			case 40: 
			case 41: 
			case 42: 
			case 43: mtmp->egotype_engrave = 1; break;
			case 44: 
			case 45: mtmp->egotype_dark = 1; break;
			case 46: mtmp->egotype_luck = 1; break;
			case 47: 
			case 48: 
			case 49: mtmp->egotype_push = 1; break;
			case 50: mtmp->egotype_arcane = 1; break;
			case 51: mtmp->egotype_clerical = 1; break;
			case 52: 
			case 53: mtmp->egotype_armorer = 1; break;
			case 54: mtmp->egotype_tank = 1; break;
			case 55: 
			case 56: mtmp->egotype_speedster = 1; break;
			case 57: mtmp->egotype_racer = 1; break;
			case 58: mtmp->egotype_randomizer = 1; break;
			case 59: mtmp->egotype_blaster = 1; break;
			case 60: mtmp->egotype_multiplicator = 1; break;
			case 61: mtmp->egotype_gator = 1; break;
			case 62: mtmp->egotype_reflecting = 1; break;
			case 63: mtmp->egotype_hugger = 1; break;
			case 64: mtmp->egotype_mimic = 1; break;
			case 65: mtmp->egotype_permamimic = 1; break;
			case 66:
			case 67: mtmp->egotype_poisoner = 1; break;
			case 68: mtmp->egotype_elementalist = 1; break;
			case 69: mtmp->egotype_resistor = 1; break;
			case 70:
			case 71: mtmp->egotype_acidspiller = 1; break;
			case 72:
			case 73: mtmp->egotype_watcher = 1; break;
			case 74: mtmp->egotype_metallivore = 1; break;
			case 75: mtmp->egotype_lithivore = 1; break;
			case 76: mtmp->egotype_organivore = 1; break;
			case 77: mtmp->egotype_breather = 1; break;
			case 78: mtmp->egotype_beamer = 1; break;
			case 79:
			case 80: mtmp->egotype_troll = 1; break;
			case 81:
			case 82:
			case 83:
			case 84:
			case 85:
			case 86: mtmp->egotype_faker = 1; break;
			case 87:
			case 88:
			case 89:
			case 90: mtmp->egotype_farter = 1; break;
			case 91: mtmp->egotype_timer = 1; break;
			case 92: mtmp->egotype_thirster = 1; break;
			case 93: mtmp->egotype_watersplasher = 1; break;
			case 94: mtmp->egotype_cancellator = 1; break;
			case 95: mtmp->egotype_banisher = 1; break;
			case 96: mtmp->egotype_shredder = 1; break;
			case 97: mtmp->egotype_abductor = 1; break;
			case 98:
			case 99: mtmp->egotype_incrementor = 1; break;
			case 100: mtmp->egotype_mirrorimage = 1; break;
			case 101:
			case 102: mtmp->egotype_curser = 1; break;
			case 103: mtmp->egotype_horner = 1; break;
			case 104: mtmp->egotype_lasher = 1; break;
			case 105: mtmp->egotype_cullen = 1; break;
			case 106:
			case 107:
			case 108: mtmp->egotype_webber = 1; break;
			case 109: mtmp->egotype_itemporter = 1; break;
			case 110: mtmp->egotype_schizo = 1; break;
			case 111: mtmp->egotype_nexus = 1; break;
			case 112: mtmp->egotype_sounder = 1; break;
			case 113: mtmp->egotype_gravitator = 1; break;
			case 114: mtmp->egotype_inert = 1; break;
			case 115:
			case 116: mtmp->egotype_antimage = 1; break;
			case 117: mtmp->egotype_plasmon = 1; break;
			case 118:
			case 119:
			case 120: mtmp->egotype_weaponizer = 1; break;
			case 121: mtmp->egotype_engulfer = 1; break;
			case 122: mtmp->egotype_bomber = 1; break;
			case 123:
			case 124: mtmp->egotype_exploder = 1; break;
			case 125: mtmp->egotype_unskillor = 1; break;
			case 126: mtmp->egotype_blinker = 1; break;
			case 127: mtmp->egotype_psychic = 1; break;
			case 128: mtmp->egotype_abomination = 1; break;
			case 129: mtmp->egotype_gazer = 1; break;
			case 130: mtmp->egotype_seducer = 1; break;
			case 131: mtmp->egotype_flickerer = 1; break;
			case 132:
			case 133: mtmp->egotype_hitter = 1; break;
			case 134: mtmp->egotype_piercer = 1; break;
			case 135: mtmp->egotype_petshielder = 1; break;
			case 136: mtmp->egotype_displacer = 1; break;
			case 137: mtmp->egotype_lifesaver = 1; break;
			case 138: mtmp->egotype_venomizer = 1; break;
			case 139: mtmp->egotype_nastinator = 1; break;
			case 140: mtmp->egotype_baddie = 1; break;
			case 141: mtmp->egotype_dreameater = 1; break;
			case 142: mtmp->egotype_sludgepuddle = 1; break;
			case 143: mtmp->egotype_vulnerator = 1; break;
			case 144: mtmp->egotype_marysue = 1; break;

		}

		while (!rn2(iswarper ? 4 : 10)) {
		switch (rnd(144)) {
			case 1:
			case 2:
			case 3: mtmp->egotype_thief = 1; break;
			case 4: mtmp->egotype_wallwalk = 1; break;
			case 5: mtmp->egotype_disenchant = 1; break;
			case 6:
			case 7: mtmp->egotype_rust = 1; break;
			case 8: 
			case 9: mtmp->egotype_corrosion = 1; break;
			case 10: 
			case 11: mtmp->egotype_decay = 1; break;
			case 12: mtmp->egotype_wither = 1; break;
			case 13: 
			case 14: 
			case 15: mtmp->egotype_grab = 1; break;
			case 16: 
			case 17: mtmp->egotype_flying = 1; break;
			case 18: 
			case 19: mtmp->egotype_hide = 1; break;
			case 20: 
			case 21: 
			case 22: mtmp->egotype_regeneration = 1; break;
			case 23: 
			case 24: 
			case 25: mtmp->egotype_undead = 1; break;
			case 26: mtmp->egotype_domestic = 1; break;
			case 27: mtmp->egotype_covetous = 1; break;
			case 28: 
			case 29: mtmp->egotype_avoider = 1; break;
			case 30: mtmp->egotype_petty = 1; break;
			case 31: mtmp->egotype_pokemon = 1; break;
			case 32: mtmp->egotype_slows = 1; break;
			case 33: mtmp->egotype_vampire = 1; break;
			case 34: mtmp->egotype_teleportself = 1; break;
			case 35: mtmp->egotype_teleportyou = 1; break;
			case 36: 
			case 37: mtmp->egotype_wrap = 1; break;
			case 38: mtmp->egotype_disease = 1; break;
			case 39: mtmp->egotype_slime = 1; break;
			case 40: 
			case 41: 
			case 42: 
			case 43: mtmp->egotype_engrave = 1; break;
			case 44: 
			case 45: mtmp->egotype_dark = 1; break;
			case 46: mtmp->egotype_luck = 1; break;
			case 47: 
			case 48: 
			case 49: mtmp->egotype_push = 1; break;
			case 50: mtmp->egotype_arcane = 1; break;
			case 51: mtmp->egotype_clerical = 1; break;
			case 52: 
			case 53: mtmp->egotype_armorer = 1; break;
			case 54: mtmp->egotype_tank = 1; break;
			case 55: 
			case 56: mtmp->egotype_speedster = 1; break;
			case 57: mtmp->egotype_racer = 1; break;
			case 58: mtmp->egotype_randomizer = 1; break;
			case 59: mtmp->egotype_blaster = 1; break;
			case 60: mtmp->egotype_multiplicator = 1; break;
			case 61: mtmp->egotype_gator = 1; break;
			case 62: mtmp->egotype_reflecting = 1; break;
			case 63: mtmp->egotype_hugger = 1; break;
			case 64: mtmp->egotype_mimic = 1; break;
			case 65: mtmp->egotype_permamimic = 1; break;
			case 66:
			case 67: mtmp->egotype_poisoner = 1; break;
			case 68: mtmp->egotype_elementalist = 1; break;
			case 69: mtmp->egotype_resistor = 1; break;
			case 70:
			case 71: mtmp->egotype_acidspiller = 1; break;
			case 72:
			case 73: mtmp->egotype_watcher = 1; break;
			case 74: mtmp->egotype_metallivore = 1; break;
			case 75: mtmp->egotype_lithivore = 1; break;
			case 76: mtmp->egotype_organivore = 1; break;
			case 77: mtmp->egotype_breather = 1; break;
			case 78: mtmp->egotype_beamer = 1; break;
			case 79:
			case 80: mtmp->egotype_troll = 1; break;
			case 81:
			case 82:
			case 83:
			case 84:
			case 85:
			case 86: mtmp->egotype_faker = 1; break;
			case 87:
			case 88:
			case 89:
			case 90: mtmp->egotype_farter = 1; break;
			case 91: mtmp->egotype_timer = 1; break;
			case 92: mtmp->egotype_thirster = 1; break;
			case 93: mtmp->egotype_watersplasher = 1; break;
			case 94: mtmp->egotype_cancellator = 1; break;
			case 95: mtmp->egotype_banisher = 1; break;
			case 96: mtmp->egotype_shredder = 1; break;
			case 97: mtmp->egotype_abductor = 1; break;
			case 98:
			case 99: mtmp->egotype_incrementor = 1; break;
			case 100: mtmp->egotype_mirrorimage = 1; break;
			case 101:
			case 102: mtmp->egotype_curser = 1; break;
			case 103: mtmp->egotype_horner = 1; break;
			case 104: mtmp->egotype_lasher = 1; break;
			case 105: mtmp->egotype_cullen = 1; break;
			case 106:
			case 107:
			case 108: mtmp->egotype_webber = 1; break;
			case 109: mtmp->egotype_itemporter = 1; break;
			case 110: mtmp->egotype_schizo = 1; break;
			case 111: mtmp->egotype_nexus = 1; break;
			case 112: mtmp->egotype_sounder = 1; break;
			case 113: mtmp->egotype_gravitator = 1; break;
			case 114: mtmp->egotype_inert = 1; break;
			case 115:
			case 116: mtmp->egotype_antimage = 1; break;
			case 117: mtmp->egotype_plasmon = 1; break;
			case 118:
			case 119:
			case 120: mtmp->egotype_weaponizer = 1; break;
			case 121: mtmp->egotype_engulfer = 1; break;
			case 122: mtmp->egotype_bomber = 1; break;
			case 123:
			case 124: mtmp->egotype_exploder = 1; break;
			case 125: mtmp->egotype_unskillor = 1; break;
			case 126: mtmp->egotype_blinker = 1; break;
			case 127: mtmp->egotype_psychic = 1; break;
			case 128: mtmp->egotype_abomination = 1; break;
			case 129: mtmp->egotype_gazer = 1; break;
			case 130: mtmp->egotype_seducer = 1; break;
			case 131: mtmp->egotype_flickerer = 1; break;
			case 132:
			case 133: mtmp->egotype_hitter = 1; break;
			case 134: mtmp->egotype_piercer = 1; break;
			case 135: mtmp->egotype_petshielder = 1; break;
			case 136: mtmp->egotype_displacer = 1; break;
			case 137: mtmp->egotype_lifesaver = 1; break;
			case 138: mtmp->egotype_venomizer = 1; break;
			case 139: mtmp->egotype_nastinator = 1; break;
			case 140: mtmp->egotype_baddie = 1; break;
			case 141: mtmp->egotype_dreameater = 1; break;
			case 142: mtmp->egotype_sludgepuddle = 1; break;
			case 143: mtmp->egotype_vulnerator = 1; break;
			case 144: mtmp->egotype_marysue = 1; break;
		}
		}

	}

	if (ptr->mlet == S_FUNGUS && ( (!rn2(isxrace ? 20 : 66) ) || (!(u.monstertimefinish % 337) && !rn2(isxrace ? 6 : 24) ) || (!(u.monstertimefinish % 3217) && !rn2(isxrace ? 3 : 10) ) ) ) {

		mtmp->isegotype = 1;
		switch (rnd(144)) {
			case 1:
			case 2:
			case 3: mtmp->egotype_thief = 1; break;
			case 4: mtmp->egotype_wallwalk = 1; break;
			case 5: mtmp->egotype_disenchant = 1; break;
			case 6:
			case 7: mtmp->egotype_rust = 1; break;
			case 8: 
			case 9: mtmp->egotype_corrosion = 1; break;
			case 10: 
			case 11: mtmp->egotype_decay = 1; break;
			case 12: mtmp->egotype_wither = 1; break;
			case 13: 
			case 14: 
			case 15: mtmp->egotype_grab = 1; break;
			case 16: 
			case 17: mtmp->egotype_flying = 1; break;
			case 18: 
			case 19: mtmp->egotype_hide = 1; break;
			case 20: 
			case 21: 
			case 22: mtmp->egotype_regeneration = 1; break;
			case 23: 
			case 24: 
			case 25: mtmp->egotype_undead = 1; break;
			case 26: mtmp->egotype_domestic = 1; break;
			case 27: mtmp->egotype_covetous = 1; break;
			case 28: 
			case 29: mtmp->egotype_avoider = 1; break;
			case 30: mtmp->egotype_petty = 1; break;
			case 31: mtmp->egotype_pokemon = 1; break;
			case 32: mtmp->egotype_slows = 1; break;
			case 33: mtmp->egotype_vampire = 1; break;
			case 34: mtmp->egotype_teleportself = 1; break;
			case 35: mtmp->egotype_teleportyou = 1; break;
			case 36: 
			case 37: mtmp->egotype_wrap = 1; break;
			case 38: mtmp->egotype_disease = 1; break;
			case 39: mtmp->egotype_slime = 1; break;
			case 40: 
			case 41: 
			case 42: 
			case 43: mtmp->egotype_engrave = 1; break;
			case 44: 
			case 45: mtmp->egotype_dark = 1; break;
			case 46: mtmp->egotype_luck = 1; break;
			case 47: 
			case 48: 
			case 49: mtmp->egotype_push = 1; break;
			case 50: mtmp->egotype_arcane = 1; break;
			case 51: mtmp->egotype_clerical = 1; break;
			case 52: 
			case 53: mtmp->egotype_armorer = 1; break;
			case 54: mtmp->egotype_tank = 1; break;
			case 55: 
			case 56: mtmp->egotype_speedster = 1; break;
			case 57: mtmp->egotype_racer = 1; break;
			case 58: mtmp->egotype_randomizer = 1; break;
			case 59: mtmp->egotype_blaster = 1; break;
			case 60: mtmp->egotype_multiplicator = 1; break;
			case 61: mtmp->egotype_gator = 1; break;
			case 62: mtmp->egotype_reflecting = 1; break;
			case 63: mtmp->egotype_hugger = 1; break;
			case 64: mtmp->egotype_mimic = 1; break;
			case 65: mtmp->egotype_permamimic = 1; break;
			case 66:
			case 67: mtmp->egotype_poisoner = 1; break;
			case 68: mtmp->egotype_elementalist = 1; break;
			case 69: mtmp->egotype_resistor = 1; break;
			case 70:
			case 71: mtmp->egotype_acidspiller = 1; break;
			case 72:
			case 73: mtmp->egotype_watcher = 1; break;
			case 74: mtmp->egotype_metallivore = 1; break;
			case 75: mtmp->egotype_lithivore = 1; break;
			case 76: mtmp->egotype_organivore = 1; break;
			case 77: mtmp->egotype_breather = 1; break;
			case 78: mtmp->egotype_beamer = 1; break;
			case 79:
			case 80: mtmp->egotype_troll = 1; break;
			case 81:
			case 82:
			case 83:
			case 84:
			case 85:
			case 86: mtmp->egotype_faker = 1; break;
			case 87:
			case 88:
			case 89:
			case 90: mtmp->egotype_farter = 1; break;
			case 91: mtmp->egotype_timer = 1; break;
			case 92: mtmp->egotype_thirster = 1; break;
			case 93: mtmp->egotype_watersplasher = 1; break;
			case 94: mtmp->egotype_cancellator = 1; break;
			case 95: mtmp->egotype_banisher = 1; break;
			case 96: mtmp->egotype_shredder = 1; break;
			case 97: mtmp->egotype_abductor = 1; break;
			case 98:
			case 99: mtmp->egotype_incrementor = 1; break;
			case 100: mtmp->egotype_mirrorimage = 1; break;
			case 101:
			case 102: mtmp->egotype_curser = 1; break;
			case 103: mtmp->egotype_horner = 1; break;
			case 104: mtmp->egotype_lasher = 1; break;
			case 105: mtmp->egotype_cullen = 1; break;
			case 106:
			case 107:
			case 108: mtmp->egotype_webber = 1; break;
			case 109: mtmp->egotype_itemporter = 1; break;
			case 110: mtmp->egotype_schizo = 1; break;
			case 111: mtmp->egotype_nexus = 1; break;
			case 112: mtmp->egotype_sounder = 1; break;
			case 113: mtmp->egotype_gravitator = 1; break;
			case 114: mtmp->egotype_inert = 1; break;
			case 115:
			case 116: mtmp->egotype_antimage = 1; break;
			case 117: mtmp->egotype_plasmon = 1; break;
			case 118:
			case 119:
			case 120: mtmp->egotype_weaponizer = 1; break;
			case 121: mtmp->egotype_engulfer = 1; break;
			case 122: mtmp->egotype_bomber = 1; break;
			case 123:
			case 124: mtmp->egotype_exploder = 1; break;
			case 125: mtmp->egotype_unskillor = 1; break;
			case 126: mtmp->egotype_blinker = 1; break;
			case 127: mtmp->egotype_psychic = 1; break;
			case 128: mtmp->egotype_abomination = 1; break;
			case 129: mtmp->egotype_gazer = 1; break;
			case 130: mtmp->egotype_seducer = 1; break;
			case 131: mtmp->egotype_flickerer = 1; break;
			case 132:
			case 133: mtmp->egotype_hitter = 1; break;
			case 134: mtmp->egotype_piercer = 1; break;
			case 135: mtmp->egotype_petshielder = 1; break;
			case 136: mtmp->egotype_displacer = 1; break;
			case 137: mtmp->egotype_lifesaver = 1; break;
			case 138: mtmp->egotype_venomizer = 1; break;
			case 139: mtmp->egotype_nastinator = 1; break;
			case 140: mtmp->egotype_baddie = 1; break;
			case 141: mtmp->egotype_dreameater = 1; break;
			case 142: mtmp->egotype_sludgepuddle = 1; break;
			case 143: mtmp->egotype_vulnerator = 1; break;
			case 144: mtmp->egotype_marysue = 1; break;
		}

		while (!rn2(iswarper ? 4 : 10)) {
		switch (rnd(144)) {
			case 1:
			case 2:
			case 3: mtmp->egotype_thief = 1; break;
			case 4: mtmp->egotype_wallwalk = 1; break;
			case 5: mtmp->egotype_disenchant = 1; break;
			case 6:
			case 7: mtmp->egotype_rust = 1; break;
			case 8: 
			case 9: mtmp->egotype_corrosion = 1; break;
			case 10: 
			case 11: mtmp->egotype_decay = 1; break;
			case 12: mtmp->egotype_wither = 1; break;
			case 13: 
			case 14: 
			case 15: mtmp->egotype_grab = 1; break;
			case 16: 
			case 17: mtmp->egotype_flying = 1; break;
			case 18: 
			case 19: mtmp->egotype_hide = 1; break;
			case 20: 
			case 21: 
			case 22: mtmp->egotype_regeneration = 1; break;
			case 23: 
			case 24: 
			case 25: mtmp->egotype_undead = 1; break;
			case 26: mtmp->egotype_domestic = 1; break;
			case 27: mtmp->egotype_covetous = 1; break;
			case 28: 
			case 29: mtmp->egotype_avoider = 1; break;
			case 30: mtmp->egotype_petty = 1; break;
			case 31: mtmp->egotype_pokemon = 1; break;
			case 32: mtmp->egotype_slows = 1; break;
			case 33: mtmp->egotype_vampire = 1; break;
			case 34: mtmp->egotype_teleportself = 1; break;
			case 35: mtmp->egotype_teleportyou = 1; break;
			case 36: 
			case 37: mtmp->egotype_wrap = 1; break;
			case 38: mtmp->egotype_disease = 1; break;
			case 39: mtmp->egotype_slime = 1; break;
			case 40: 
			case 41: 
			case 42: 
			case 43: mtmp->egotype_engrave = 1; break;
			case 44: 
			case 45: mtmp->egotype_dark = 1; break;
			case 46: mtmp->egotype_luck = 1; break;
			case 47: 
			case 48: 
			case 49: mtmp->egotype_push = 1; break;
			case 50: mtmp->egotype_arcane = 1; break;
			case 51: mtmp->egotype_clerical = 1; break;
			case 52: 
			case 53: mtmp->egotype_armorer = 1; break;
			case 54: mtmp->egotype_tank = 1; break;
			case 55: 
			case 56: mtmp->egotype_speedster = 1; break;
			case 57: mtmp->egotype_racer = 1; break;
			case 58: mtmp->egotype_randomizer = 1; break;
			case 59: mtmp->egotype_blaster = 1; break;
			case 60: mtmp->egotype_multiplicator = 1; break;
			case 61: mtmp->egotype_gator = 1; break;
			case 62: mtmp->egotype_reflecting = 1; break;
			case 63: mtmp->egotype_hugger = 1; break;
			case 64: mtmp->egotype_mimic = 1; break;
			case 65: mtmp->egotype_permamimic = 1; break;
			case 66:
			case 67: mtmp->egotype_poisoner = 1; break;
			case 68: mtmp->egotype_elementalist = 1; break;
			case 69: mtmp->egotype_resistor = 1; break;
			case 70:
			case 71: mtmp->egotype_acidspiller = 1; break;
			case 72:
			case 73: mtmp->egotype_watcher = 1; break;
			case 74: mtmp->egotype_metallivore = 1; break;
			case 75: mtmp->egotype_lithivore = 1; break;
			case 76: mtmp->egotype_organivore = 1; break;
			case 77: mtmp->egotype_breather = 1; break;
			case 78: mtmp->egotype_beamer = 1; break;
			case 79:
			case 80: mtmp->egotype_troll = 1; break;
			case 81:
			case 82:
			case 83:
			case 84:
			case 85:
			case 86: mtmp->egotype_faker = 1; break;
			case 87:
			case 88:
			case 89:
			case 90: mtmp->egotype_farter = 1; break;
			case 91: mtmp->egotype_timer = 1; break;
			case 92: mtmp->egotype_thirster = 1; break;
			case 93: mtmp->egotype_watersplasher = 1; break;
			case 94: mtmp->egotype_cancellator = 1; break;
			case 95: mtmp->egotype_banisher = 1; break;
			case 96: mtmp->egotype_shredder = 1; break;
			case 97: mtmp->egotype_abductor = 1; break;
			case 98:
			case 99: mtmp->egotype_incrementor = 1; break;
			case 100: mtmp->egotype_mirrorimage = 1; break;
			case 101:
			case 102: mtmp->egotype_curser = 1; break;
			case 103: mtmp->egotype_horner = 1; break;
			case 104: mtmp->egotype_lasher = 1; break;
			case 105: mtmp->egotype_cullen = 1; break;
			case 106:
			case 107:
			case 108: mtmp->egotype_webber = 1; break;
			case 109: mtmp->egotype_itemporter = 1; break;
			case 110: mtmp->egotype_schizo = 1; break;
			case 111: mtmp->egotype_nexus = 1; break;
			case 112: mtmp->egotype_sounder = 1; break;
			case 113: mtmp->egotype_gravitator = 1; break;
			case 114: mtmp->egotype_inert = 1; break;
			case 115:
			case 116: mtmp->egotype_antimage = 1; break;
			case 117: mtmp->egotype_plasmon = 1; break;
			case 118:
			case 119:
			case 120: mtmp->egotype_weaponizer = 1; break;
			case 121: mtmp->egotype_engulfer = 1; break;
			case 122: mtmp->egotype_bomber = 1; break;
			case 123:
			case 124: mtmp->egotype_exploder = 1; break;
			case 125: mtmp->egotype_unskillor = 1; break;
			case 126: mtmp->egotype_blinker = 1; break;
			case 127: mtmp->egotype_psychic = 1; break;
			case 128: mtmp->egotype_abomination = 1; break;
			case 129: mtmp->egotype_gazer = 1; break;
			case 130: mtmp->egotype_seducer = 1; break;
			case 131: mtmp->egotype_flickerer = 1; break;
			case 132:
			case 133: mtmp->egotype_hitter = 1; break;
			case 134: mtmp->egotype_piercer = 1; break;
			case 135: mtmp->egotype_petshielder = 1; break;
			case 136: mtmp->egotype_displacer = 1; break;
			case 137: mtmp->egotype_lifesaver = 1; break;
			case 138: mtmp->egotype_venomizer = 1; break;
			case 139: mtmp->egotype_nastinator = 1; break;
			case 140: mtmp->egotype_baddie = 1; break;
			case 141: mtmp->egotype_dreameater = 1; break;
			case 142: mtmp->egotype_sludgepuddle = 1; break;
			case 143: mtmp->egotype_vulnerator = 1; break;
			case 144: mtmp->egotype_marysue = 1; break;
		}
		}

	}

	if (AlwaysEgotypeMonsters || u.uprops[ALWAYS_EGOTYPES].extrinsic || have_egostone()) {

		mtmp->isegotype = 1;
		switch (rnd(144)) {
			case 1:
			case 2:
			case 3: mtmp->egotype_thief = 1; break;
			case 4: mtmp->egotype_wallwalk = 1; break;
			case 5: mtmp->egotype_disenchant = 1; break;
			case 6:
			case 7: mtmp->egotype_rust = 1; break;
			case 8: 
			case 9: mtmp->egotype_corrosion = 1; break;
			case 10: 
			case 11: mtmp->egotype_decay = 1; break;
			case 12: mtmp->egotype_wither = 1; break;
			case 13: 
			case 14: 
			case 15: mtmp->egotype_grab = 1; break;
			case 16: 
			case 17: mtmp->egotype_flying = 1; break;
			case 18: 
			case 19: mtmp->egotype_hide = 1; break;
			case 20: 
			case 21: 
			case 22: mtmp->egotype_regeneration = 1; break;
			case 23: 
			case 24: 
			case 25: mtmp->egotype_undead = 1; break;
			case 26: mtmp->egotype_domestic = 1; break;
			case 27: mtmp->egotype_covetous = 1; break;
			case 28: 
			case 29: mtmp->egotype_avoider = 1; break;
			case 30: mtmp->egotype_petty = 1; break;
			case 31: mtmp->egotype_pokemon = 1; break;
			case 32: mtmp->egotype_slows = 1; break;
			case 33: mtmp->egotype_vampire = 1; break;
			case 34: mtmp->egotype_teleportself = 1; break;
			case 35: mtmp->egotype_teleportyou = 1; break;
			case 36: 
			case 37: mtmp->egotype_wrap = 1; break;
			case 38: mtmp->egotype_disease = 1; break;
			case 39: mtmp->egotype_slime = 1; break;
			case 40: 
			case 41: 
			case 42: 
			case 43: mtmp->egotype_engrave = 1; break;
			case 44: 
			case 45: mtmp->egotype_dark = 1; break;
			case 46: mtmp->egotype_luck = 1; break;
			case 47: 
			case 48: 
			case 49: mtmp->egotype_push = 1; break;
			case 50: mtmp->egotype_arcane = 1; break;
			case 51: mtmp->egotype_clerical = 1; break;
			case 52: 
			case 53: mtmp->egotype_armorer = 1; break;
			case 54: mtmp->egotype_tank = 1; break;
			case 55: 
			case 56: mtmp->egotype_speedster = 1; break;
			case 57: mtmp->egotype_racer = 1; break;
			case 58: mtmp->egotype_randomizer = 1; break;
			case 59: mtmp->egotype_blaster = 1; break;
			case 60: mtmp->egotype_multiplicator = 1; break;
			case 61: mtmp->egotype_gator = 1; break;
			case 62: mtmp->egotype_reflecting = 1; break;
			case 63: mtmp->egotype_hugger = 1; break;
			case 64: mtmp->egotype_mimic = 1; break;
			case 65: mtmp->egotype_permamimic = 1; break;
			case 66:
			case 67: mtmp->egotype_poisoner = 1; break;
			case 68: mtmp->egotype_elementalist = 1; break;
			case 69: mtmp->egotype_resistor = 1; break;
			case 70:
			case 71: mtmp->egotype_acidspiller = 1; break;
			case 72:
			case 73: mtmp->egotype_watcher = 1; break;
			case 74: mtmp->egotype_metallivore = 1; break;
			case 75: mtmp->egotype_lithivore = 1; break;
			case 76: mtmp->egotype_organivore = 1; break;
			case 77: mtmp->egotype_breather = 1; break;
			case 78: mtmp->egotype_beamer = 1; break;
			case 79:
			case 80: mtmp->egotype_troll = 1; break;
			case 81:
			case 82:
			case 83:
			case 84:
			case 85:
			case 86: mtmp->egotype_faker = 1; break;
			case 87:
			case 88:
			case 89:
			case 90: mtmp->egotype_farter = 1; break;
			case 91: mtmp->egotype_timer = 1; break;
			case 92: mtmp->egotype_thirster = 1; break;
			case 93: mtmp->egotype_watersplasher = 1; break;
			case 94: mtmp->egotype_cancellator = 1; break;
			case 95: mtmp->egotype_banisher = 1; break;
			case 96: mtmp->egotype_shredder = 1; break;
			case 97: mtmp->egotype_abductor = 1; break;
			case 98:
			case 99: mtmp->egotype_incrementor = 1; break;
			case 100: mtmp->egotype_mirrorimage = 1; break;
			case 101:
			case 102: mtmp->egotype_curser = 1; break;
			case 103: mtmp->egotype_horner = 1; break;
			case 104: mtmp->egotype_lasher = 1; break;
			case 105: mtmp->egotype_cullen = 1; break;
			case 106:
			case 107:
			case 108: mtmp->egotype_webber = 1; break;
			case 109: mtmp->egotype_itemporter = 1; break;
			case 110: mtmp->egotype_schizo = 1; break;
			case 111: mtmp->egotype_nexus = 1; break;
			case 112: mtmp->egotype_sounder = 1; break;
			case 113: mtmp->egotype_gravitator = 1; break;
			case 114: mtmp->egotype_inert = 1; break;
			case 115:
			case 116: mtmp->egotype_antimage = 1; break;
			case 117: mtmp->egotype_plasmon = 1; break;
			case 118:
			case 119:
			case 120: mtmp->egotype_weaponizer = 1; break;
			case 121: mtmp->egotype_engulfer = 1; break;
			case 122: mtmp->egotype_bomber = 1; break;
			case 123:
			case 124: mtmp->egotype_exploder = 1; break;
			case 125: mtmp->egotype_unskillor = 1; break;
			case 126: mtmp->egotype_blinker = 1; break;
			case 127: mtmp->egotype_psychic = 1; break;
			case 128: mtmp->egotype_abomination = 1; break;
			case 129: mtmp->egotype_gazer = 1; break;
			case 130: mtmp->egotype_seducer = 1; break;
			case 131: mtmp->egotype_flickerer = 1; break;
			case 132:
			case 133: mtmp->egotype_hitter = 1; break;
			case 134: mtmp->egotype_piercer = 1; break;
			case 135: mtmp->egotype_petshielder = 1; break;
			case 136: mtmp->egotype_displacer = 1; break;
			case 137: mtmp->egotype_lifesaver = 1; break;
			case 138: mtmp->egotype_venomizer = 1; break;
			case 139: mtmp->egotype_nastinator = 1; break;
			case 140: mtmp->egotype_baddie = 1; break;
			case 141: mtmp->egotype_dreameater = 1; break;
			case 142: mtmp->egotype_sludgepuddle = 1; break;
			case 143: mtmp->egotype_vulnerator = 1; break;
			case 144: mtmp->egotype_marysue = 1; break;
		}

		while (!rn2(iswarper ? 4 : 10)) {
		switch (rnd(144)) {
			case 1:
			case 2:
			case 3: mtmp->egotype_thief = 1; break;
			case 4: mtmp->egotype_wallwalk = 1; break;
			case 5: mtmp->egotype_disenchant = 1; break;
			case 6:
			case 7: mtmp->egotype_rust = 1; break;
			case 8: 
			case 9: mtmp->egotype_corrosion = 1; break;
			case 10: 
			case 11: mtmp->egotype_decay = 1; break;
			case 12: mtmp->egotype_wither = 1; break;
			case 13: 
			case 14: 
			case 15: mtmp->egotype_grab = 1; break;
			case 16: 
			case 17: mtmp->egotype_flying = 1; break;
			case 18: 
			case 19: mtmp->egotype_hide = 1; break;
			case 20: 
			case 21: 
			case 22: mtmp->egotype_regeneration = 1; break;
			case 23: 
			case 24: 
			case 25: mtmp->egotype_undead = 1; break;
			case 26: mtmp->egotype_domestic = 1; break;
			case 27: mtmp->egotype_covetous = 1; break;
			case 28: 
			case 29: mtmp->egotype_avoider = 1; break;
			case 30: mtmp->egotype_petty = 1; break;
			case 31: mtmp->egotype_pokemon = 1; break;
			case 32: mtmp->egotype_slows = 1; break;
			case 33: mtmp->egotype_vampire = 1; break;
			case 34: mtmp->egotype_teleportself = 1; break;
			case 35: mtmp->egotype_teleportyou = 1; break;
			case 36: 
			case 37: mtmp->egotype_wrap = 1; break;
			case 38: mtmp->egotype_disease = 1; break;
			case 39: mtmp->egotype_slime = 1; break;
			case 40: 
			case 41: 
			case 42: 
			case 43: mtmp->egotype_engrave = 1; break;
			case 44: 
			case 45: mtmp->egotype_dark = 1; break;
			case 46: mtmp->egotype_luck = 1; break;
			case 47: 
			case 48: 
			case 49: mtmp->egotype_push = 1; break;
			case 50: mtmp->egotype_arcane = 1; break;
			case 51: mtmp->egotype_clerical = 1; break;
			case 52: 
			case 53: mtmp->egotype_armorer = 1; break;
			case 54: mtmp->egotype_tank = 1; break;
			case 55: 
			case 56: mtmp->egotype_speedster = 1; break;
			case 57: mtmp->egotype_racer = 1; break;
			case 58: mtmp->egotype_randomizer = 1; break;
			case 59: mtmp->egotype_blaster = 1; break;
			case 60: mtmp->egotype_multiplicator = 1; break;
			case 61: mtmp->egotype_gator = 1; break;
			case 62: mtmp->egotype_reflecting = 1; break;
			case 63: mtmp->egotype_hugger = 1; break;
			case 64: mtmp->egotype_mimic = 1; break;
			case 65: mtmp->egotype_permamimic = 1; break;
			case 66:
			case 67: mtmp->egotype_poisoner = 1; break;
			case 68: mtmp->egotype_elementalist = 1; break;
			case 69: mtmp->egotype_resistor = 1; break;
			case 70:
			case 71: mtmp->egotype_acidspiller = 1; break;
			case 72:
			case 73: mtmp->egotype_watcher = 1; break;
			case 74: mtmp->egotype_metallivore = 1; break;
			case 75: mtmp->egotype_lithivore = 1; break;
			case 76: mtmp->egotype_organivore = 1; break;
			case 77: mtmp->egotype_breather = 1; break;
			case 78: mtmp->egotype_beamer = 1; break;
			case 79:
			case 80: mtmp->egotype_troll = 1; break;
			case 81:
			case 82:
			case 83:
			case 84:
			case 85:
			case 86: mtmp->egotype_faker = 1; break;
			case 87:
			case 88:
			case 89:
			case 90: mtmp->egotype_farter = 1; break;
			case 91: mtmp->egotype_timer = 1; break;
			case 92: mtmp->egotype_thirster = 1; break;
			case 93: mtmp->egotype_watersplasher = 1; break;
			case 94: mtmp->egotype_cancellator = 1; break;
			case 95: mtmp->egotype_banisher = 1; break;
			case 96: mtmp->egotype_shredder = 1; break;
			case 97: mtmp->egotype_abductor = 1; break;
			case 98:
			case 99: mtmp->egotype_incrementor = 1; break;
			case 100: mtmp->egotype_mirrorimage = 1; break;
			case 101:
			case 102: mtmp->egotype_curser = 1; break;
			case 103: mtmp->egotype_horner = 1; break;
			case 104: mtmp->egotype_lasher = 1; break;
			case 105: mtmp->egotype_cullen = 1; break;
			case 106:
			case 107:
			case 108: mtmp->egotype_webber = 1; break;
			case 109: mtmp->egotype_itemporter = 1; break;
			case 110: mtmp->egotype_schizo = 1; break;
			case 111: mtmp->egotype_nexus = 1; break;
			case 112: mtmp->egotype_sounder = 1; break;
			case 113: mtmp->egotype_gravitator = 1; break;
			case 114: mtmp->egotype_inert = 1; break;
			case 115:
			case 116: mtmp->egotype_antimage = 1; break;
			case 117: mtmp->egotype_plasmon = 1; break;
			case 118:
			case 119:
			case 120: mtmp->egotype_weaponizer = 1; break;
			case 121: mtmp->egotype_engulfer = 1; break;
			case 122: mtmp->egotype_bomber = 1; break;
			case 123:
			case 124: mtmp->egotype_exploder = 1; break;
			case 125: mtmp->egotype_unskillor = 1; break;
			case 126: mtmp->egotype_blinker = 1; break;
			case 127: mtmp->egotype_psychic = 1; break;
			case 128: mtmp->egotype_abomination = 1; break;
			case 129: mtmp->egotype_gazer = 1; break;
			case 130: mtmp->egotype_seducer = 1; break;
			case 131: mtmp->egotype_flickerer = 1; break;
			case 132:
			case 133: mtmp->egotype_hitter = 1; break;
			case 134: mtmp->egotype_piercer = 1; break;
			case 135: mtmp->egotype_petshielder = 1; break;
			case 136: mtmp->egotype_displacer = 1; break;
			case 137: mtmp->egotype_lifesaver = 1; break;
			case 138: mtmp->egotype_venomizer = 1; break;
			case 139: mtmp->egotype_nastinator = 1; break;
			case 140: mtmp->egotype_baddie = 1; break;
			case 141: mtmp->egotype_dreameater = 1; break;
			case 142: mtmp->egotype_sludgepuddle = 1; break;
			case 143: mtmp->egotype_vulnerator = 1; break;
			case 144: mtmp->egotype_marysue = 1; break;
		}
		}

	}

	if(/*in_mklev && */hides_under(ptr) && OBJ_AT(x, y))
	  mtmp->mundetected = TRUE;

	switch(ptr->mlet) {
		case S_MIMIC:
			set_mimic_sym(mtmp);
			break;
		case S_TROLL:
			if (mndx == PM_OLOG_HAI_MIMIC) set_mimic_sym(mtmp);
			if (mndx == PM_OLOG_HAI_PERMAMIMIC) set_mimic_sym(mtmp);
			if (mndx == PM_TROLL_PERMAMIMIC_MUMMY) set_mimic_sym(mtmp);
			if (mndx == PM_NETHERWORLD_TROLL) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}

			break;
		case S_GREMLIN:
			if (mndx == PM_DEFORMED_GALLTRIT) set_mimic_sym(mtmp);

			break;
		case S_GIANT:
			if (mndx == PM_EVIL_PATCH_MINOTAUR) set_mimic_sym(mtmp);
			if (mndx == PM_MINOTAUR_MIMIC) set_mimic_sym(mtmp);

			break;
		case S_LICH:
			if (mndx == PM_MIMIC_LICH) set_mimic_sym(mtmp);
			if (mndx == PM_DEMILICH_PERMAMIMIC) set_mimic_sym(mtmp);

			break;
		case S_IMP:
			if (mndx == PM_COCOON_IMP) set_mimic_sym(mtmp);
			if (mndx == PM_HIDDEN_RUTTERKIN) set_mimic_sym(mtmp);

			break;
		case S_LIZARD:
			if (mndx == PM_MIMIC_LIZARD) set_mimic_sym(mtmp);
			if (mndx == PM_DEFORMED_LIZARD) set_mimic_sym(mtmp);

			break;

		case S_UMBER:

			if (mndx == PM_MAIDENHACK_HORROR) set_mimic_sym(mtmp);
			if (mndx == PM_YASD_HORROR) set_mimic_sym(mtmp);
			if (mndx == PM_NETHACKFOUR_HORROR) set_mimic_sym(mtmp);
			if (mndx == PM_DEVTEAM_HORROR) set_mimic_sym(mtmp);
			if (mndx == PM_UMBER_MIMIC) set_mimic_sym(mtmp);

			break;

		case S_BLOB:

			if (mndx == PM_ONANOCTER__G) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}

			break;

		case S_TRAPPER:

			if (mndx == PM_DECEPTIVE_SPRINGY) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}

			break;

		case S_RUSTMONST:

			if (mndx == PM_SECRET_DISENCHANTER) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_CAMO_RUST_MONSTER) set_mimic_sym(mtmp);

			break;

		case S_RODENT:

			if (mndx == PM_SOCK_MOLE) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}

			break;

		case S_DOG:

			if (mndx == PM_CLEAR_HOUND) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_ETHEREAL_HOUND) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_REAL_CLEAR_HOUND) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_REAL_ETHEREAL_HOUND) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}

			break;

		case S_BAD_FOOD:

			if (mndx == PM_CLEAR_MUSHROOM_PATCH) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_CORPSER) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mtmp->data == &mons[PM_ROTTEN_FOOD_RATION]) set_mimic_sym(mtmp);

			if (mndx == PM_POISON_CREEPER) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_PLAGUE_POPPY) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_CARRION_VINE) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_CYCLE_OF_LIFE) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_SOLAR_CREEPER) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_VINES) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}

			break;

		case S_WORM:

			if (mndx == PM_CLEAR_WORM_MASS) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}

			break;

		case S_YETI:

			if (mndx == PM_NIGHT_MARAUDER) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_SPIRIT_BABOON) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}

			break;

		case S_KOP:
			if (mtmp->data == &mons[PM_NETZAH_SEPHIRAH]) set_mimic_sym(mtmp);
			if (mtmp->data == &mons[PM_GNOSIS_SEPHIRAH]) set_mimic_sym(mtmp);

			if (mndx == PM_KEYSTONE_POLICEMAN) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_POLICEMAN_SERGEANT) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_POLICEMAN_LIEUTENANT) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_POLICEMAN_KAPTAIN) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_POLICEMAN_KOMMISSIONER) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_POLICEMAN_KCHIEF) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_POLICEMAN_KATCHER) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_POLICEMAN_KRIMINOLOGIST) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}

			if (mndx == PM_KEYSTONE_WARDER) set_mimic_sym(mtmp);
			if (mndx == PM_WARDER_SERGEANT) set_mimic_sym(mtmp);
			if (mndx == PM_WARDER_LIEUTENANT) set_mimic_sym(mtmp);
			if (mndx == PM_WARDER_KAPTAIN) set_mimic_sym(mtmp);
			if (mndx == PM_WARDER_KOMMISSIONER) set_mimic_sym(mtmp);
			if (mndx == PM_WARDER_KCHIEF) set_mimic_sym(mtmp);
			if (mndx == PM_WARDER_KATCHER) set_mimic_sym(mtmp);
			if (mndx == PM_WARDER_KRIMINOLOGIST) set_mimic_sym(mtmp);

			if (mndx == PM_KEYSTONE_INSPECTOR) set_mimic_sym(mtmp);
			if (mndx == PM_INSPECTOR_SERGEANT) set_mimic_sym(mtmp);
			if (mndx == PM_INSPECTOR_LIEUTENANT) set_mimic_sym(mtmp);
			if (mndx == PM_INSPECTOR_KAPTAIN) set_mimic_sym(mtmp);
			if (mndx == PM_INSPECTOR_KOMMISSIONER) set_mimic_sym(mtmp);
			if (mndx == PM_INSPECTOR_KCHIEF) set_mimic_sym(mtmp);
			if (mndx == PM_INSPECTOR_KATCHER) set_mimic_sym(mtmp);
			if (mndx == PM_INSPECTOR_KRIMINOLOGIST) set_mimic_sym(mtmp);

			if (mndx == PM_HUMAN_WEREMIMIC) set_mimic_sym(mtmp);
			if (mndx == PM_HUMAN_WEREPERMAMIMIC) set_mimic_sym(mtmp);

			break;
		case S_XORN:
			if (mtmp->data == &mons[PM_XEROC]) set_mimic_sym(mtmp);
			if (mtmp->data == &mons[PM_PORTER_XORN_PERMAMIMIC]) set_mimic_sym(mtmp);
			break;
		case S_BAD_COINS:
			if (mtmp->data == &mons[PM_FATA_MORGANA]) set_mimic_sym(mtmp);
			break;
		case S_ZOMBIE:
			if (mtmp->data == &mons[PM_ALIENATED_UFO_PART]) set_mimic_sym(mtmp);
			if (mndx == PM_UNDEFINED_UFO_PART) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_DRONING_UFO_PART) set_mimic_sym(mtmp);
			break;
		case S_NAGA:
			if (mtmp->data == &mons[PM_MULTI_HUED_NAGA]) set_mimic_sym(mtmp);
			break;

		case S_FELINE:
			if (rn2(2) && mndx == PM_AIR_DEATH) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_DEFORMED_CAT) set_mimic_sym(mtmp);
			if (mndx == PM_LINUS_THE_CAT) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE; mtmp->minvisreal = TRUE;}
			break;

		case S_KOBOLD:

			if (mndx == PM_UNIDENTIFIED_KOBOLD_SHAMAN) set_mimic_sym(mtmp);

			if (mtmp->data == &mons[PM_FALLEN_SHAMAN]) {
				(void) makemon(&mons[PM_FALLEN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_FALLEN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_FALLEN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_FALLEN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_PLEASED_SHAMAN]) {
				(void) makemon(&mons[PM_PLEASURE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_PLEASURE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_PLEASURE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_PLEASURE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_CARVER_SHAMAN]) {
				(void) makemon(&mons[PM_CARVER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_CARVER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_CARVER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_CARVER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_BUTCHER_SHAMAN]) {
				(void) makemon(&mons[PM_BUTCHER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_BUTCHER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_BUTCHER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_BUTCHER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_DEVILKIN_SHAMAN]) {
				(void) makemon(&mons[PM_DEVILKIN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_DEVILKIN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_DEVILKIN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_DEVILKIN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_DARK_SHAMAN]) {
				(void) makemon(&mons[PM_DARK_CREATURE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_DARK_CREATURE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_DARK_CREATURE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_DARK_CREATURE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_WARPED_SHAMAN]) {
				(void) makemon(&mons[PM_WARPED_FALLEN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_WARPED_FALLEN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_WARPED_FALLEN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_WARPED_FALLEN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_MOVED_SHAMAN]) {
				(void) makemon(&mons[PM_MOVED_ONE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_MOVED_ONE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_MOVED_ONE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_MOVED_ONE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_TURNED_SHAMAN]) {
				(void) makemon(&mons[PM_TURNED_ONE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_TURNED_ONE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_TURNED_ONE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_TURNED_ONE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_ROTATED_SHAMAN]) {
				(void) makemon(&mons[PM_ROTATED_ONE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_ROTATED_ONE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_ROTATED_ONE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_ROTATED_ONE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_FEARSOME_BUTCHER_SHAMAN]) {
				(void) makemon(&mons[PM_FEARSOME_BUTCHER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_FEARSOME_BUTCHER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_FEARSOME_BUTCHER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_FEARSOME_BUTCHER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_DIABOLIC_SHAMAN]) {
				(void) makemon(&mons[PM_DIABOLIC_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_DIABOLIC_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_DIABOLIC_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_DIABOLIC_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_HIDDEN_SHAMAN]) {
			    mtmp->perminvis = TRUE;
			    mtmp->minvis = TRUE;
				(void) makemon(&mons[PM_HIDDEN_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_HIDDEN_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_HIDDEN_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_HIDDEN_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_DESERT_KOBOLD_SHAMAN]) {
				(void) makemon(&mons[PM_DESERT_KOBOLD], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_DESERT_KOBOLD], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_DESERT_KOBOLD], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_DESERT_KOBOLD], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_ARCANE_BUTCHER_SHAMAN]) {
				(void) makemon(&mons[PM_ARCANE_BUTCHER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_ARCANE_BUTCHER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_ARCANE_BUTCHER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_ARCANE_BUTCHER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_JUNGLE_WARRIOR_SHAMAN]) {
				(void) makemon(&mons[PM_JUNGLE_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_JUNGLE_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_JUNGLE_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_JUNGLE_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}


			if (mtmp->data == &mons[PM_HIDDEN_POTATO]) {
			    mtmp->perminvis = TRUE;
			    mtmp->minvis = TRUE;

			}

			break;

		case S_HUMANOID:

			if (mtmp->data == &mons[PM_OVERSEER]) {
				(void) makemon(&mons[PM_MINION], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_MINION], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_MINION], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_MINION], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_LASHER]) {
				(void) makemon(&mons[PM_SLAYER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_SLAYER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_SLAYER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_SLAYER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_OVERLORD]) {
				(void) makemon(&mons[PM_ICE_BOAR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_ICE_BOAR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_ICE_BOAR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_ICE_BOAR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_BLOOD_BOSS]) {
				(void) makemon(&mons[PM_FIRE_BOAR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_FIRE_BOAR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_FIRE_BOAR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_FIRE_BOAR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_HELLWHIP]) {
				(void) makemon(&mons[PM_HELL_SPAWN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_HELL_SPAWN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_HELL_SPAWN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_HELL_SPAWN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_SEXTON]) {
				(void) makemon(&mons[PM_ZAKARUMITE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_ZAKARUMITE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_ZAKARUMITE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_ZAKARUMITE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_CHURCH_PREACHER]) {
				(void) makemon(&mons[PM_CHURCH_NOVICE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_CHURCH_NOVICE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_ROLLER_COASTER]) {
				(void) makemon(&mons[PM_GREAT_BAR_SLAYER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_GREAT_BAR_SLAYER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_GREAT_BAR_SLAYER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_GREAT_BAR_SLAYER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_CANTOR]) {
				(void) makemon(&mons[PM_FAITHFUL], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_FAITHFUL], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_FAITHFUL], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_FAITHFUL], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_HEIROPHANT]) {
				(void) makemon(&mons[PM_ZEALOT], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_ZEALOT], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_ZEALOT], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_ZEALOT], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_POPE]) {
				(void) makemon(&mons[PM_FANATICIAN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_FANATICIAN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_FANATICIAN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_FANATICIAN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_BAPTIST]) {
				(void) makemon(&mons[PM_BUTLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_BUTLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_BUTLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_BUTLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_HOLY_ROLLER_COASTER]) {
				(void) makemon(&mons[PM_BUTLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_BUTLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_BUTLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_BUTLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_BUTLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_EGYPTIAN_POPE]) {
				(void) makemon(&mons[PM_BUTLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_BUTLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_BUTLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_BUTLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_BUTLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_BUTLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_FETISH_SHAMAN]) {
				(void) makemon(&mons[PM_FETISH], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_FETISH_BLOWDART], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_FETISH], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_FETISH_BLOWDART], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_PYGMY_WARRIOR_SHAMAN]) {
				(void) makemon(&mons[PM_PYGMY_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_PYGMY_WARRIOR_BLOWDART], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_PYGMY_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_PYGMY_WARRIOR_BLOWDART], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_FLAYER_SHAMAN]) {
				(void) makemon(&mons[PM_FLAYER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_FLAYER_BLOWDART], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_FLAYER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_FLAYER_BLOWDART], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_SOUL_KILLER_SHAMAN]) {
				(void) makemon(&mons[PM_SOUL_KILLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_SOUL_KILLER_BLOWDART], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_SOUL_KILLER], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_SOUL_KILLER_BLOWDART], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_STYGIAN_DOLL_SHAMAN]) {
				(void) makemon(&mons[PM_STYGIAN_DOLL], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_STYGIAN_DOLL_BLOWDART], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_STYGIAN_DOLL], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_STYGIAN_DOLL_BLOWDART], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_VOODOO_FETISH_SHAMAN]) {
				(void) makemon(&mons[PM_VOODOO_FETISH], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_VOODOO_FETISH_BLOWDART], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_VOODOO_FETISH], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_VOODOO_FETISH_BLOWDART], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_GIANT_PYGMY_SHAMAN]) {
				(void) makemon(&mons[PM_GIANT_PYGMY], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_GIANT_PYGMY_BLOWDART], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_GIANT_PYGMY], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_GIANT_PYGMY_BLOWDART], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_WOLF_MAN_PACKLEADER]) {
				(void) makemon(&mons[PM_GRAY_WOLF], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_GRAY_WOLF], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_GRAY_WOLF], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_GRAY_WOLF], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_BEAR_MAN_PACKLEADER]) {
				(void) makemon(&mons[PM_BROWN_BEAR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_BROWN_BEAR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_BROWN_BEAR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_BROWN_BEAR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_DWARVEN_BULL_MAN_PACKLEADER]) {
				(void) makemon(&mons[PM_BULL], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_BULL], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_BULL], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_BULL], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mndx == PM_CAMO_DWARF) set_mimic_sym(mtmp);
			if (mndx == PM_DWARF_KING_PERMAMIMIC) set_mimic_sym(mtmp);

			break;

		case S_MUMMY:
			if (mtmp->data == &mons[PM_MIMIC_MUMMY]) set_mimic_sym(mtmp);
			if (mndx == PM_TROLL_PERMAMIMIC_MUMMY) set_mimic_sym(mtmp);

			if (mtmp->data == &mons[PM_REVIVAL_SPINNER]) (void) makemon(&mons[PM_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);

			if (mtmp->data == &mons[PM_UPPER_SPINNER]) {
			(void) makemon(&mons[PM_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			(void) makemon(&mons[PM_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_HOLLOW_ONE] || mtmp->data == &mons[PM_EMPTY_CREATURE] || mtmp->data == &mons[PM_OLD_CORPSE]) {
				(void) makemon(&mons[PM_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_GUARDIAN]) {
				(void) makemon(&mons[PM_RETURNED_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_RETURNED_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_RETURNED_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_RETURNED_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_GIANT_NUT_MOTHER]) {
				(void) makemon(&mons[PM_ICE_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_ICE_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_ICE_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_ICE_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_UNRAVELER] || mtmp->data == &mons[PM_DISAFFECTOR]) {
				(void) makemon(&mons[PM_BONE_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_BONE_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_BONE_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_BONE_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_BAAL_SUBJECT_MUMMY]) {
				(void) makemon(&mons[PM_BAAL_COLD_MAGE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_DEATH_MAGE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_BAAL_COLD_MAGE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_DEATH_MAGE], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}
			if (mtmp->data == &mons[PM_HORADRIM_ANCIENT] || mtmp->data == &mons[PM_HORADRIM_ANCESTOR]) {
				(void) makemon(&mons[PM_BURNING_DEAD_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_BURNING_DEAD_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_BURNING_DEAD_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_BURNING_DEAD_POTATO], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_CLOCK_OLD_HORADRIM]) {
				(void) makemon(&mons[PM_BLIGHTING_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_BLIGHTING_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_BLIGHTING_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_BLIGHTING_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}
			if (mtmp->data == &mons[PM_DECOMPOSER]) {
				(void) makemon(&mons[PM_BLIGHTING_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_BLIGHTING_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_BLIGHTING_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_BLIGHTING_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_BLIGHTING_SKELETON], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}
			if (mtmp->data == &mons[PM_PALEOLITHIC_MUMMY]) {
				(void) makemon(&mons[PM_ANIMATED_ICE_CUBES], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_ANIMATED_ICE_CUBES], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_ANIMATED_ICE_CUBES], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_ANIMATED_ICE_CUBES], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}
			if (mtmp->data == &mons[PM_DISSOLVED_UNDEAD_POTATO]) {
				(void) makemon(&mons[PM_ANIMATED_ICE_CUBES], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_ANIMATED_ICE_CUBES], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_ANIMATED_ICE_CUBES], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_ANIMATED_ICE_CUBES], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_ANIMATED_ICE_CUBES], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			break;
		case S_RUBMONST:
			if (mtmp->data == &mons[PM_MIMICRY_RUBBER]) set_mimic_sym(mtmp);
			if (mtmp->data == &mons[PM_UNKNOWN_ESCALATION_RUBBER]) set_mimic_sym(mtmp);
			if (mtmp->data == &mons[PM_DSCHIIM_RUBBER]) set_mimic_sym(mtmp);
			if (mndx == PM_CAMO_RUBBER) set_mimic_sym(mtmp);
			if (mndx == PM_WIPER_RUBBER) set_mimic_sym(mtmp);
			if (mndx == PM_PETTY_WIPER_RUBBER) set_mimic_sym(mtmp);

			if (mndx == PM_CLEAR_ICKY_THING) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}

			break;
		case S_GHOST:
			if (mtmp->data == &mons[PM_CURSED_SPIRIT]) set_mimic_sym(mtmp);
			if (mtmp->data == &mons[PM_DEVILISH_SPIRIT]) set_mimic_sym(mtmp);

			if (mndx == PM_SHADOW_SHAPE) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE; mtmp->minvisreal = TRUE;}
			if (mndx == PM_TOP_SECRET_AGENT) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE; mtmp->minvisreal = TRUE;}

			break;
		case S_QUADRUPED:
			if (mtmp->data == &mons[PM_BLENDING_TITANOTHERE]) set_mimic_sym(mtmp);
			break;
		case S_VORTEX:
			if (mtmp->data == &mons[PM_MIMIC_VORTEX]) set_mimic_sym(mtmp);
			if (mndx == PM_MAELSTROM) set_mimic_sym(mtmp);

			if (mndx == PM_LLOIGOR) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			break;
		case S_ANT:
			if (mtmp->data == &mons[PM_DISGUISED_SOLDIER_ANT]) set_mimic_sym(mtmp);
			if (mtmp->data == &mons[PM_MIMIC_ANT]) set_mimic_sym(mtmp);
			break;
		case S_HUMAN:
			if (is_pool(x, y) && (mndx == PM_HUMAN_WEREPIRANHA || mndx == PM_HUMAN_WEREEEL || mndx == PM_HUMAN_WEREKRAKEN) )
			    mtmp->mundetected = TRUE;
			if ( (is_pool(x, y) || is_lava(x, y) ) && (mndx == PM_HUMAN_WEREFLYFISH) )
			    mtmp->mundetected = TRUE;
			if (mtmp->data == &mons[PM_OSCILLATOR]) set_mimic_sym(mtmp);

			if (mndx == PM_KURAST_BOO) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_STALKER_GIRL) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_THE_INVISIBLE_MAN) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE; mtmp->minvisreal = TRUE;}

			break;
		case S_EYE:
			if (mtmp->data == &mons[PM_VOLTORB]) set_mimic_sym(mtmp);
			if (mtmp->data == &mons[PM_ELECTRODE]) set_mimic_sym(mtmp);
			break;
		case S_COCKATRICE:
			if (mtmp->data == &mons[PM_MIMIC_CHICKEN]) set_mimic_sym(mtmp);
			if (mtmp->data == &mons[PM_GRATING_CHICKEN]) set_mimic_sym(mtmp);
			break;
		case S_SPIDER:
		case S_SNAKE:

			if (mtmp->data == &mons[PM_UPGRADED_SECRET_CAR]) set_mimic_sym(mtmp);
			if (mtmp->data == &mons[PM_EXPENSIVE_SECRET_CAR]) set_mimic_sym(mtmp);
			if (mtmp->data == &mons[PM_RUNNING_COBRA]) set_mimic_sym(mtmp);

			if (mndx == PM_CLEAR_CENTIPEDE) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}

			/* if(in_mklev)
			    if(x && y)
				(void) mkobj_at(0, x, y, TRUE);
			if(in_mklev && hides_under(ptr) && OBJ_AT(x, y))
			    mtmp->mundetected = TRUE;*/
			break;
		case S_LIGHT:
		case S_ELEMENTAL:
			if (mndx == PM_STALKER || mndx == PM_FORCE_STALKER || mndx == PM_STONE_STALKER || mndx == PM_THE_HIDDEN || mndx == PM_INVISIBLE_BADGUY || mndx == PM_UNSEEN_POTATO || mndx == PM_CAMOUFLAGED_WATCHER || mndx == PM_UNSEEN_SERVANT || mndx == PM_HIDDEN_TRACKER || mndx == PM_SILENT_KILLER || mndx == PM_ILLUSION_WEAVER || mndx == PM_PAIN_MASTER || mndx == PM_BLACK_LIGHT || mndx == PM_CHEATING_BLACK_LIGHT || mndx == PM_INVISIBLE_SPIRIT || mndx == PM_BLACK_LASER || mndx == PM_POLTERGEIST) {
			    mtmp->perminvis = TRUE;
			    mtmp->minvis = TRUE;
			}

			if (mndx == PM_HALLUCINATION_IMAGE) set_mimic_sym(mtmp);
			if (mndx == PM_CHEATING_BLACK_LIGHT) set_mimic_sym(mtmp);

			break;
		case S_FUNGUS:
			if (mndx == PM_INVISIBLE_MOLD || mndx == PM_INVISIBLE_FUNGUS || mndx == PM_INVISIBLE_PATCH || mndx == PM_INVISIBLE_FORCE_FUNGUS || mndx == PM_INVISIBLE_FORCE_PATCH || mndx == PM_INVISIBLE_WARP_FUNGUS || mndx == PM_INVISIBLE_WARP_PATCH || mndx == PM_INVISIBLE_STALK || mndx == PM_INVISIBLE_SPORE || mndx == PM_INVISIBLE_MUSHROOM || mndx == PM_INVISIBLE_GROWTH || mndx == PM_INVISIBLE_COLONY ) {
			    mtmp->perminvis = TRUE;
			    mtmp->minvis = TRUE;
			}
			if (mndx == PM_FALSE_MOLD || mndx == PM_DECEIVING_MOLD || mndx == PM_FALSE_FUNGUS || mndx == PM_DECEIVING_FUNGUS || mndx == PM_FALSE_PATCH || mndx == PM_DECEIVING_PATCH || mndx == PM_FALSE_FORCE_FUNGUS || mndx == PM_DECEIVING_FORCE_FUNGUS || mndx == PM_FALSE_FORCE_PATCH || mndx == PM_DECEIVING_FORCE_PATCH || mndx == PM_FALSE_WARP_FUNGUS || mndx == PM_DECEIVING_WARP_FUNGUS || mndx == PM_FALSE_WARP_PATCH || mndx == PM_DECEIVING_WARP_PATCH || mndx == PM_FALSE_STALK || mndx == PM_DECEIVING_STALK || mndx == PM_FALSE_SPORE || mndx == PM_DECEIVING_SPORE || mndx == PM_FALSE_MUSHROOM || mndx == PM_DECEIVING_MUSHROOM || mndx == PM_FALSE_GROWTH || mndx == PM_DECEIVING_GROWTH || mndx == PM_FALSE_COLONY || mndx == PM_DECEIVING_COLONY) mtmp->mpeaceful = TRUE;
			if (mndx == PM_HIDDEN_MOLD) set_mimic_sym(mtmp);
			if (mndx == PM_SECLUDED_MOLD) set_mimic_sym(mtmp);
			if (mndx == PM_HIDDEN_FUNGUS) set_mimic_sym(mtmp);
			if (mndx == PM_SECLUDED_FUNGUS) set_mimic_sym(mtmp);
			if (mndx == PM_HIDDEN_PATCH) set_mimic_sym(mtmp);
			if (mndx == PM_SECLUDED_PATCH) set_mimic_sym(mtmp);
			if (mndx == PM_HIDDEN_FORCE_FUNGUS) set_mimic_sym(mtmp);
			if (mndx == PM_SECLUDED_FORCE_FUNGUS) set_mimic_sym(mtmp);
			if (mndx == PM_HIDDEN_FORCE_PATCH) set_mimic_sym(mtmp);
			if (mndx == PM_SECLUDED_FORCE_PATCH) set_mimic_sym(mtmp);
			if (mndx == PM_HIDDEN_WARP_FUNGUS) set_mimic_sym(mtmp);
			if (mndx == PM_SECLUDED_WARP_FUNGUS) set_mimic_sym(mtmp);
			if (mndx == PM_HIDDEN_WARP_PATCH) set_mimic_sym(mtmp);
			if (mndx == PM_SECLUDED_WARP_PATCH) set_mimic_sym(mtmp);
			if (mndx == PM_HIDDEN_STALK) set_mimic_sym(mtmp);
			if (mndx == PM_SECLUDED_STALK) set_mimic_sym(mtmp);
			if (mndx == PM_HIDDEN_SPORE) set_mimic_sym(mtmp);
			if (mndx == PM_SECLUDED_SPORE) set_mimic_sym(mtmp);
			if (mndx == PM_HIDDEN_GROWTH) set_mimic_sym(mtmp);
			if (mndx == PM_SECLUDED_GROWTH) set_mimic_sym(mtmp);
			if (mndx == PM_HIDDEN_MUSHROOM) set_mimic_sym(mtmp);
			if (mndx == PM_SECLUDED_MUSHROOM) set_mimic_sym(mtmp);
			if (mndx == PM_HIDDEN_COLONY) set_mimic_sym(mtmp);
			if (mndx == PM_SECLUDED_COLONY) set_mimic_sym(mtmp);
			break;
		case S_EEL:
			if (is_pool(x, y))
			    mtmp->mundetected = TRUE;
			if (mtmp->data == &mons[PM_CAMO_FISH] || mtmp->data == &mons[PM_DEFORMED_FISH] || mtmp->data == &mons[PM_AMORPHOUS_FISH]) set_mimic_sym(mtmp);

			if (mndx == PM_FLOUNDER || mndx == PM_FLAT_FISH) {
			    mtmp->perminvis = TRUE;
			    mtmp->minvis = TRUE;
			}

			break;
		case S_FLYFISH:
			if (is_pool(x, y) || is_lava(x, y) )
			    mtmp->mundetected = TRUE;
			if (mtmp->data == &mons[PM_FLYING_CAMO_FISH]) set_mimic_sym(mtmp);
			break;
		case S_PUDDING:
			if (ptr == &mons[PM_PHANTOM]) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			break;
		case S_ZOUTHERN:
			if (mndx == PM_MARSUPILAMI) set_mimic_sym(mtmp);
			break;
		case S_LEPRECHAUN:

			if (mtmp->data == &mons[PM_HEHEHE_HE_GUY]) set_mimic_sym(mtmp);
			if (mndx == PM_VESTY) set_mimic_sym(mtmp);
			if (ptr == &mons[PM_MALICIOUS_LEPRECHAUN]) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (ptr == &mons[PM_DEATH_LEPRECHAUN]) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}

      if (mndx == PM_POOKA){
	        if(!rn2(5)) (void) mongets(mtmp, CARROT);
#ifdef TOURIST
		    if (!rn2(8)) (void) mongets(mtmp, HAWAIIAN_SHIRT); 
#endif
		    if (!rn2(4)) (void)mongets(mtmp, ELVEN_LEATHER_HELM);
		    switch (rn2(3)) {
		case 0:
			    if (!rn2(4)) (void)mongets(mtmp, ELVEN_SHIELD);
			    if (rn2(2)) (void)mongets(mtmp, ELVEN_SHORT_SWORD);
         case 1:
			    (void)mongets(mtmp, ELVEN_DAGGER);
			    break;
		case 2:
			    if (rn2(2)) {
				(void)mongets(mtmp, ELVEN_SPEAR);
				(void)mongets(mtmp, ELVEN_SHIELD);
			    }
			    break;
		    }
      }
         if (ptr == &mons[PM_POOKA] && !rn2(3)){
			    mtmp->perminvis = TRUE;
			    mtmp->minvis = TRUE;
         }
			mtmp->msleeping = 1;
			break;
		case S_JABBERWOCK:

		if (mndx == PM_PUTRID_DEFILER || mndx == PM_WRETCHED_DEFILER || mndx == PM_FETID_DEFILER || mndx == PM_RANCID_DEFILER || mndx == PM_MOLDY_DEFILER) { mtmp->mflee = 1; mtmp->mfleetim = 120;}

		if (mtmp->data == &mons[PM_PUTRID_DEFILER]) { 
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 3;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_PAIN_WORM, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}
		if (mtmp->data == &mons[PM_WRETCHED_DEFILER]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 4;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_TORMENT_WORM, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}
		if (mtmp->data == &mons[PM_FETID_DEFILER]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 5;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_AGONY_WORM, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}
		if (mtmp->data == &mons[PM_RANCID_DEFILER]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 6;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_ECSTASY_WORM, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}
		if (mtmp->data == &mons[PM_MOLDY_DEFILER]) {
			struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
			if (otmpX) {
				otmpX->spe = 0;
				otmpX->quan = 7;
				otmpX->owt = weight(otmpX);
				otmpX->corpsenm = egg_type_from_parent(PM_ANGUISH_WORM, FALSE);
				attach_egg_hatch_timeout(otmpX);
				mpickobj(mtmp,otmpX, TRUE);
			}
		}

	        if (rn2(2) && rn2(5) && !u.uhave.amulet) mtmp->msleeping = 1;

			break; /* fall through removed by Amy */

		case S_GRUE:

	      if (mndx == PM__S_____NIX) mtmp->msleeping = 1;
			if (mndx == PM_IT) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE; mtmp->minvisreal = TRUE;}
			if (mndx == PM_INVISIBLE_HORROR) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE; mtmp->minvisreal = TRUE;}

			break;

		case S_NYMPH:
	      if (mndx == PM_SATYR)
	        if (rn2(2))
	          (void) mongets(mtmp, DAGGER);

			if (rn2(5) && !u.uhave.amulet) mtmp->msleeping = 1;

			if (mndx == PM_PIXIE || mndx == PM_PIXIE_ARCHER) {        
  			    mtmp->perminvis = TRUE;
  			    mtmp->minvis = TRUE;
			}

			if (mtmp->data == &mons[PM_MIMIC_NYMPH]) set_mimic_sym(mtmp);

			break;
		case S_DRAGON:
			if (mndx == PM_KLIEAU_MANTICORE) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mtmp->data == &mons[PM_ARTILLERY_DRAGON]) set_mimic_sym(mtmp);
			break;
		case S_ORC:

			if (mndx == PM_DECEPTIVE_ORC) set_mimic_sym(mtmp);

			if (mtmp->data == &mons[PM_ALIEN_FROM_MARS]) {
				(void) makemon(&mons[PM_MAN_FROM_MARS], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_MAN_FROM_MARS], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_MAN_FROM_MARS], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_MAN_FROM_MARS], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (mtmp->data == &mons[PM_RUNT_SHAMAN]) {
				(void) makemon(&mons[PM_RUNT], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				(void) makemon(&mons[PM_RUNT], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(3)) (void) makemon(&mons[PM_RUNT], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if (!rn2(5)) (void) makemon(&mons[PM_RUNT], mtmp->mx, mtmp->my, MM_ADJACENTOK);
			}

			if (Race_if(PM_ELF)) mtmp->mpeaceful = FALSE;
			break;
		case S_UNICORN:

			if (mndx == PM_THESTRAL) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mndx == PM_DISGUISED_UNDEAD_UNICORN) set_mimic_sym(mtmp);

			if (is_unicorn(ptr) &&
					sgn(u.ualign.type) == sgn(ptr->maligntyp))
				mtmp->mpeaceful = TRUE;
			break;
		case S_BAT:

			if (mndx == PM_INVISIBLE_CC) {mtmp->minvis = TRUE; mtmp->perminvis = TRUE;}
			if (mtmp->data == &mons[PM_UNIDENTIFIED_RAVEN]) set_mimic_sym(mtmp);

			if (mndx == PM_RITA_S_SWEET_STILETTO) {
				(void) makemon(mkclass(S_SPIDER,0), mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_ANGRY);
				(void) makemon(mkclass(S_SPIDER,0), mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_ANGRY);
				(void) makemon(mkclass(S_SPIDER,0), mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_ANGRY);
				(void) makemon(mkclass(S_SPIDER,0), mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_ANGRY);
				(void) makemon(mkclass(S_SPIDER,0), mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_ANGRY);
				(void) makemon(mkclass(S_SPIDER,0), mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_ANGRY);
				(void) makemon(mkclass(S_SPIDER,0), mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_ANGRY);
				(void) makemon(mkclass(S_SPIDER,0), mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_ANGRY);
			}

			if ((Race_if(PM_IMPERIAL) || (Inhell && !Race_if(PM_HERETIC) ) || flags.gehenna) && is_bat(ptr))
			    mon_adjust_speed(mtmp, 2, (struct obj *)0);
			break;
		case S_VAMPIRE:
			/* [DS] Star vampires are invisible until they feed */
			if (mndx == PM_STAR_VAMPIRE) {
			    mtmp->perminvis = TRUE;
			    mtmp->minvis = TRUE;
			}

			if (mndx == PM_ORIENTAL_VAMPIRE) {mtmp->perminvis = TRUE; mtmp->minvis = TRUE; }

			if (mtmp->data == &mons[PM_VAMPIRE_SHADOWCLOAK]) set_mimic_sym(mtmp);
			break;
	}

	/* If you're a unicorn, everything will be fast because they need to be able to match your ultra-high speed. --Amy */
	if (Race_if(PM_PLAYER_UNICORN)) mon_adjust_speed(mtmp, 2, (struct obj *)0);

	if (mtmp->egotype_mimic || mtmp->egotype_permamimic) set_mimic_sym(mtmp);

	if (standardmimic(ptr) || permamimic(ptr)) set_mimic_sym(mtmp);


	if (ptr->mlet == u.speedymonster) mon_adjust_speed(mtmp, 2, (struct obj *)0);
	if (!rn2(20) && (ptr->mlet == u.musemonster) ) {
		switch (rnd(3)) {
			case 1:
				if (rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
				else (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
				break;
			case 2:
				if (rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
				else (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
				break;
			case 3:
				if (rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
				else (void) mongets(mtmp, rnd_misc_item_new(mtmp));
				break;
		}

	}

	if ((ct = emits_light(mtmp->data)) > 0)
		new_light_source(mtmp->mx, mtmp->my, ct,
				 LS_MONSTER, (genericptr_t)mtmp);

	mitem = 0;	/* extra inventory item for this monster */

	if ((mcham = pm_to_cham(mndx)) != CHAM_ORDINARY) {
		/* If you're protected with a ring, don't create
		 * any shape-changing chameleons -dgk
		 */
		if (Protection_from_shape_changers)
			mtmp->cham = CHAM_ORDINARY;
		else {
			mtmp->cham = mcham;
			(void) mon_spec_poly(mtmp, rndmonst(), 0L, FALSE, FALSE, FALSE, FALSE);
		}
	} else if (mndx == PM_WIZARD_OF_YENDOR) {
		mtmp->iswiz = TRUE;
		flags.no_of_wizards++;
		if (flags.no_of_wizards == 1 && Is_earthlevel(&u.uz))
			mitem = SPE_DIG;
		if (Race_if(PM_RODNEYAN)) mtmp->mpeaceful = 1;

	} else if (mndx == PM_DJINNI) {
		flags.djinni_count++;
	} else if (mndx == PM_DAO) {
		flags.dao_count++;
	} else if (mndx == PM_EFREETI) {
		flags.efreeti_count++;
	} else if (mndx == PM_MARID) {
		flags.marid_count++;
	} else if (mndx == PM_GHOST) {
		flags.ghost_count++;
		if (!(mmflags & MM_NONAME))
			mtmp = christen_monst(mtmp, rndghostname());

	/* undead player monsters don't have titles, so I'm assigning names to them instead --Amy */
	} else if (mndx >= PM_UNDEAD_ARCHEOLOGIST && mndx <= PM_UNDEAD_WIZARD && !(mmflags & MM_NONAME) ) {

		mtmp = christen_monst(mtmp, mtmp->female ? rndplrmonnamefemale() : rndplrmonname() );

	} else if (mndx == PM_NIGHTMARE) {
		struct obj *otmp;

		otmp = oname(mksobj(SKELETON_KEY, TRUE, FALSE),
				artiname(ART_KEY_OF_LAW));
		if (otmp) {
			otmp->blessed = otmp->cursed = otmp->hvycurse = otmp->prmcurse = 0;
			mpickobj(mtmp, otmp, TRUE);
		}
	} else if (mndx == PM_BEHOLDER) {
		struct obj *otmp;

		otmp = oname(mksobj(SKELETON_KEY, TRUE, FALSE),
				artiname(ART_KEY_OF_NEUTRALITY));
		if (otmp) {
			otmp->blessed = otmp->cursed = otmp->hvycurse = otmp->prmcurse = 0;
			mpickobj(mtmp, otmp, TRUE);
		}
	} else if (mndx == PM_VECNA) {
		struct obj *otmp;

		otmp = oname(mksobj(SKELETON_KEY, TRUE, FALSE),
				artiname(ART_KEY_OF_CHAOS));
		if (otmp) {
			otmp->blessed = otmp->cursed = otmp->hvycurse = otmp->prmcurse = 0;
			mpickobj(mtmp, otmp, TRUE);
		}
	} else if (mndx == PM_GYPSY) {
		/* KMH -- Gypsies are randomly generated; initialize them here */
		gypsy_init(mtmp);
	} else if (mndx == PM_VLAD_THE_IMPALER) {
		mitem = CANDELABRUM_OF_INVOCATION;
	} else if (mndx == PM_CROESUS) {
		mitem = TWO_HANDED_SWORD;
	} else if (ptr->msound == MS_NEMESIS && ptr->mlet != S_NEMESE && mndx != PM_TRUE_MISSINGNO && mndx != PM_ETHEREAL_MISSINGNO && mndx != PM_STARLIT_SKY && mndx != PM_MISNAMED_STARLIT_SKY && mndx != PM_WRONG_NAMED_STARLIT_SKY && mndx != PM_ERRONEOUS_STARLIT_SKY && mndx != PM_DARK_STARLIT_SKY && mndx != PM_BLACK_STARLIT_SKY && mndx != PM_RED_STARLIT_SKY && mndx != PM_BROWN_STARLIT_SKY && mndx != PM_GREEN_STARLIT_SKY && mndx != PM_PURPLE_STARLIT_SKY && mndx != PM_YELLOW_STARLIT_SKY && mndx != PM_ORANGE_STARLIT_SKY && mndx != PM_CYAN_STARLIT_SKY && mndx != PM_VIOLET_STARLIT_SKY) {
		mitem = BELL_OF_OPENING; /* prevent new random nemesis monsters from dropping the bell --Amy */
	} else if (mndx == PM_PESTILENCE) {
		mitem = POT_SICKNESS;
	} 



	if (mitem && allow_minvent) (void) mongets(mtmp, mitem);

	if(in_mklev) {
		if(((is_ndemon(ptr)) ||
		    (mndx == PM_WUMPUS) ||
		    (mndx == PM_LONG_WORM) ||
		    (mndx == PM_GIANT_EEL)) && !u.uhave.amulet && rn2(5))
			mtmp->msleeping = 1;
	} else {
		if(byyou) {
			newsym(mtmp->mx,mtmp->my);
			set_apparxy(mtmp);
		}
	}
	if(is_dprince(ptr) && ptr->msound == MS_BRIBE) {
	    mtmp->mpeaceful = mtmp->minvis = mtmp->perminvis = 1;
	    mtmp->mavenge = 0;
	    if (uwep && uwep->oartifact == ART_EXCALIBUR)
		mtmp->mpeaceful = mtmp->mtame = FALSE;
	}

	if (u.uprops[GHOST_WORLD].extrinsic || GhostWorld || have_ghostlystone() ) {
		mtmp->minvis = mtmp->perminvis = mtmp->minvisreal = 1;
	}

	if (mtmp->data->mlet == u.alwaysinvisible) mtmp->minvis = mtmp->perminvis = 1;
	if (mtmp->data->mlet == u.alwayshidden) mtmp->minvis = mtmp->perminvis = mtmp->minvisreal = 1;

	if (!rn2(!(u.monstertimefinish % 13334) ? 3 : !(u.monstertimefinish % 1334) ? 10 : !(u.monstertimefinish % 134) ? 30 : 100) && mtmp->minvis) mtmp->minvisreal = TRUE;

	if (!rn2(!(u.monstertimefinish % 13239) ? 3 : !(u.monstertimefinish % 1329) ? 10 : !(u.monstertimefinish % 129) ? 30 : 100) ) mtmp->noegodesc = TRUE;

	if (!rn2(!(u.monstertimefinish % 13239) ? 3 : !(u.monstertimefinish % 1329) ? 10 : !(u.monstertimefinish % 129) ? 30 : 100) ) mtmp->noegodisplay = TRUE;

	if (issoviet && rn2(3)) {
		mtmp->noegodesc = TRUE;
		if (!rn2(2)) mtmp->noegodisplay = TRUE;
	}

#ifndef DCC30_BUG
	if (mndx == PM_LONG_WORM && (mtmp->wormno = get_wormno()) != 0)
#else
	/* DICE 3.0 doesn't like assigning and comparing mtmp->wormno in the
	 * same expression.
	 */
	if (mndx == PM_LONG_WORM &&
		(mtmp->wormno = get_wormno(), mtmp->wormno != 0))
#endif
	{
	    /* we can now create worms with tails - 11/91 */
	    initworm(mtmp, rn2(5));
	    if (count_wsegs(mtmp)) place_worm_tail_randomly(mtmp, x, y);
	}
	set_malign(mtmp);		/* having finished peaceful changes */
	if(anymon || (ptr->geno & G_UNIQ) || !rn2((ptr->geno & G_VLGROUP) ? 500 : (ptr->geno & G_LGROUP) ? 200 : (ptr->geno & G_RGROUP) ? 100 : (ptr->geno & G_SGROUP) ? 50 : 5) ) { /* everything that spawns in groups can spawn in bigger groups --Amy */
	    if ((ptr->geno & G_SGROUP) && allow_special && rn2(2)) {
		if (!rn2(5000))  m_initxxlgrp(mtmp, mtmp->mx, mtmp->my);
		else if(!rn2(800))  m_initxlgrp(mtmp, mtmp->mx, mtmp->my);
		else if(!rn2(160))  m_initvlgrp(mtmp, mtmp->mx, mtmp->my);
		else if(!rn2(30))  m_initlgrp(mtmp, mtmp->mx, mtmp->my);
		else if (rn2(10))    m_initsgrp(mtmp, mtmp->mx, mtmp->my);
	    } else if (ptr->geno & G_LGROUP && allow_special && rn2(3) ) {
		if (!rn2(1000))  m_initxxlgrp(mtmp, mtmp->mx, mtmp->my);
		else if(!rn2(200))  m_initxlgrp(mtmp, mtmp->mx, mtmp->my);
		else if(!rn2(40))  m_initvlgrp(mtmp, mtmp->mx, mtmp->my);
		else if(rn2(3))  m_initlgrp(mtmp, mtmp->mx, mtmp->my);
		else if (rn2(10))	    m_initsgrp(mtmp, mtmp->mx, mtmp->my);
	    } else if(ptr->geno & G_VLGROUP && allow_special && rn2(5) ) {
		if (!rn2(200))  m_initxxlgrp(mtmp, mtmp->mx, mtmp->my);
		else if(!rn2(20))  m_initxlgrp(mtmp, mtmp->mx, mtmp->my);
		else if(rn2(3))  m_initvlgrp(mtmp, mtmp->mx, mtmp->my);
		else if(rn2(3))  m_initlgrp(mtmp, mtmp->mx, mtmp->my);
		else if (rn2(10))        m_initsgrp(mtmp, mtmp->mx, mtmp->my);
	    } else if(ptr->geno & G_RGROUP && allow_special && rn2(5) ) {
		if (!rn2(100))  m_initxxlgrp(mtmp, mtmp->mx, mtmp->my);
		else if(!rn2(10))  m_initxlgrp(mtmp, mtmp->mx, mtmp->my);
		else if(!rn2(5))  m_initvlgrp(mtmp, mtmp->mx, mtmp->my);
		else if(!rn2(5))  m_initlgrp(mtmp, mtmp->mx, mtmp->my);
		else if (rn2(10))        m_initsgrp(mtmp, mtmp->mx, mtmp->my);
	    }
		/* allow other monsters to spawn in groups too --Amy */
	    else if (!rn2(500) && allow_special && mndx != PM_SHOPKEEPER && mndx != PM_BLACK_MARKETEER && mndx != PM_ALIGNED_PRIEST && mndx != PM_HIGH_PRIEST && mndx != PM_GUARD && mndx != quest_info(MS_NEMESIS) /*&& !(ptr->geno & G_UNIQ)*/ ) {
		if (!rn2(500))  m_initxxlgrp(mtmp, mtmp->mx, mtmp->my);
		else if(!rn2(100))  m_initxlgrp(mtmp, mtmp->mx, mtmp->my);
		else if (!rn2(20)) m_initvlgrp(mtmp, mtmp->mx, mtmp->my);
		else if (!rn2(4)) m_initlgrp(mtmp, mtmp->mx, mtmp->my);
		else m_initsgrp(mtmp, mtmp->mx, mtmp->my);
	    }
	}

	if (allow_minvent) {
	    if(is_armed(ptr))
		m_initweap(mtmp);	/* equip with weapons / armor */
	    m_initinv(mtmp);  /* add on a few special items incl. more armor */
		m_initxtraitem(mtmp);

		/* for an elemental, monsters get musable items more often --Amy */
		if (Race_if(PM_ELEMENTAL) && mtmp->data->mmove && (rnd(mtmp->data->mmove) > 5) && !rn2(25)) {

			switch (rnd(10)) {
	
				case 1:
				case 2:
				case 3:
				case 4:
				case 5: 
					if (rn2(20)) mongets(mtmp, rnd_offensive_item(mtmp));
					else mongets(mtmp, rnd_offensive_item_new(mtmp)); break;
				case 6:
				case 7:
				case 8:
					if (rn2(20)) mongets(mtmp, rnd_defensive_item(mtmp));
					else mongets(mtmp, rnd_defensive_item_new(mtmp)); break;
				case 9:
				case 10:
					if (rn2(20)) mongets(mtmp, rnd_misc_item(mtmp));
					else mongets(mtmp, rnd_misc_item_new(mtmp)); break;
			}
		}

	    m_dowear(mtmp, TRUE);
	} else {
	    if (mtmp->minvent) discard_minvent(mtmp);
	    mtmp->minvent = (struct obj *)0;    /* caller expects this */
	    mtmp->minvent = (struct obj *)0;    /* caller expects this */
		if (allow_special) m_initxtraitem(mtmp);
	}
	if ((ptr->mflags3 & M3_WAITMASK) && !(mmflags & MM_NOWAIT)) {
		if (ptr->mflags3 & M3_WAITFORU)
			mtmp->mstrategy |= STRAT_WAITFORU;
		if (ptr->mflags3 & M3_CLOSE)
			mtmp->mstrategy |= STRAT_CLOSE;
	}

	if (mndx == PM_UNFORTUNATE_VICTIM && in_mklev ) { /* These are supposed to spawn already dead. --Amy */
			monkilled(mtmp, "", AD_PHYS);
			return((struct monst *)0);
	} 

	if (mndx == PM_SCROLLER_MASTER || mndx == PM_BOULDER_MASTER || mndx == PM_ITEM_MASTER || mndx == PM_GOOD_ITEM_MASTER || mndx == PM_BAD_ITEM_MASTER || mndx == PM_HOLE_MASTER || mndx == PM_TRAP_MASTER) {
		monkilled(mtmp, "", AD_PHYS); /* leave no trace of this monster --Amy */
		return((struct monst *)0);
	}

	if (mndx == PM_SHOCKING_SPHERE && Role_if(PM_ACID_MAGE) && Is_nemesis(&u.uz) ) {
			(void) mon_spec_polyX(mtmp,  &mons[PM_LIGHTNING_PROOF_WALL], 0L, FALSE, FALSE, FALSE, FALSE);
	} 

	/* Kop characters sometimes receive pets --Amy */
	if (mtmp->mpeaceful && Race_if(PM_KOP) && !rn2(10) && sgn(u.ualign.type) == sgn(mtmp->data->maligntyp) && (mtmp->data->mr < rnd(100) ) ) {
		(void) tamedog(mtmp, (struct obj *)0, FALSE);
		return((struct monst *)0);
	}

	if (!rn2(50) && (mtmp->data->mcolor == CLR_GREEN || mtmp->data->mcolor == CLR_BRIGHT_GREEN) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "grass cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "plashch trava") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "o't plash") ) ) {
		(void) tamedog(mtmp, (struct obj *)0, FALSE);
		return((struct monst *)0);
	}

	if (!rn2(20) && is_pokemon(mtmp->data) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "poke mongo cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sovat' mongo plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "soktudun mongo plash") )) {
		(void) tamedog(mtmp, (struct obj *)0, FALSE);
		return((struct monst *)0);
	}

	if (mtmp->mpeaceful && is_animal(mtmp->data) && uarm && uarm->oartifact == ART_BEASTMASTER_S_DUSTER && !rn2(100)) {
		(void) tamedog(mtmp, (struct obj *)0, FALSE);
		return((struct monst *)0);
	}

	if (mndx == PM_SIZZLE || mndx == PM_KATNISS) {
		(void) tamedog(mtmp, (struct obj *)0, FALSE);
		return((struct monst *)0);
	}

	if (!in_mklev)
	    newsym(mtmp->mx,mtmp->my);	/* make sure the mon shows up */

	if (Race_if(PM_SENSER)) { /* can sometimes sense a monster that spawns --Amy */
		if (u.ulevel < 5) senserchance = 3;
		else if (u.ulevel < 10) senserchance = 4;
		else if (u.ulevel < 15) senserchance = 5;
		else if (u.ulevel < 20) senserchance = 6;
		else if (u.ulevel < 25) senserchance = 7;
		else if (u.ulevel < 30) senserchance = 8;
		else senserchance = 9;

		if (rnd(10) < senserchance ) pline("You sense the arrival of %s.",noit_mon_nam(mtmp) );
	}

	/* immunizer needs a disadvantage; I'm randomly reducing their alignment --Amy */
	if (Race_if(PM_IMMUNIZER) && !rn2(3) ) adjalign(-1);

	if (!rn2(5000) && allow_special) { /* very rarely create some monsters with the same letter --Amy */
		randsp = (rn2(24) + 2);
		if (!rn2(3)) randsp *= 2;
		if (!rn2(7)) randsp *= 3;
		if (!rn2(20)) randsp *= 5;
		if (!rn2(100)) randsp *= 10;

		for (i = 0; i < randsp; i++) {
			if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;
			(void) makemon(mkclass(ptr->mlet,0), mtmp->mx, mtmp->my, MM_ADJACENTOK);
		}
	}

	return(mtmp);
}

int
mbirth_limit(mndx)
int mndx;
{
	/* assert(MAXMONNO < 255); */
	return (mndx == PM_NAZGUL ? 9 : mndx == PM_ERINYS ? 3 : MAXMONNO); 
}

/* used for wand/scroll/spell of create monster */
/* returns TRUE iff you know monsters have been created */

boolean
create_critters(cnt, mptr)
int cnt;
struct permonst *mptr;		/* usually null; used for confused reading */
{
	coord c;
	int x, y;
	struct monst *mon;
	boolean known = FALSE;
#ifdef WIZARD
	boolean ask = wizard;
#endif

	while (cnt--) {
#ifdef WIZARD
	    if (ask) {
		if (create_particular()) {
		    known = TRUE;
		    continue;
		}
		else ask = FALSE;	/* ESC will shut off prompting */
	    }
#endif
	    x = u.ux,  y = u.uy;
	    /* if in water, try to encourage an aquatic monster
	       by finding and then specifying another wet location */
	    if (!mptr && u.uinwater && enexto(&c, x, y, &mons[PM_GIANT_EEL]))
		x = c.x,  y = c.y;

	    mon = makemon(mptr, x, y, NO_MM_FLAGS);
	    if (mon && canspotmon(mon)) known = TRUE;
	}
	return known;
}

#endif /* OVL1 */
#ifdef OVL0

STATIC_OVL boolean
uncommon(mndx)
int mndx;
{
	if (mons[mndx].geno & (G_NOGEN/* | G_UNIQ*/)) return TRUE;
	if ((mons[mndx].geno & (G_UNIQ)) && rn2(20) && !Role_if(PM_TRANSSYLVANIAN) ) return TRUE;
	if (mvitals[mndx].mvflags & G_GONE) return TRUE;

	/* In Soviet Russia, uncommon entities are more common because "harharhar har!" --Amy */

	if (uncommon2(&mons[mndx]) && issoviet ? !rn2(3) : rn2(2) && !Race_if(PM_RODNEYAN) ) return TRUE;
	if (uncommon3(&mons[mndx]) && rn2(issoviet ? 2 : 3) && !Race_if(PM_RODNEYAN) ) return TRUE;
	if (uncommon5(&mons[mndx]) && rn2(issoviet ? 3 : 5) && !Race_if(PM_RODNEYAN) ) return TRUE;
	if (uncommon7(&mons[mndx]) && rn2(issoviet ? 4 : 7) && !Race_if(PM_RODNEYAN) ) return TRUE;
	if (uncommon10(&mons[mndx]) && rn2(issoviet ? 5 : 10) && !Race_if(PM_RODNEYAN) ) return TRUE;

	if (monstr[mndx] >= 9 && monstr[mndx] < 14 && !rn2(10)) return TRUE;
	if (monstr[mndx] >= 14 && monstr[mndx] < 19 && !rn2(5)) return TRUE;
	if (monstr[mndx] >= 19 && monstr[mndx] < 24 && (rnd(10) > 3) ) return TRUE;
	if (monstr[mndx] >= 24 && monstr[mndx] < 28 && (rnd(10) > 4) ) return TRUE;
	if (monstr[mndx] >= 28 && monstr[mndx] < 33 && !rn2(2)) return TRUE;
	if (monstr[mndx] >= 33 && monstr[mndx] < 37 && (rnd(10) > 6) ) return TRUE;
	if (monstr[mndx] >= 37 && monstr[mndx] < 42 && (rnd(10) > 7) ) return TRUE;
	if (monstr[mndx] >= 42 && monstr[mndx] < 46 && rn2(5)) return TRUE;
	if (monstr[mndx] >= 46 && monstr[mndx] < 50 && rn2(10)) return TRUE;
	if (monstr[mndx] >= 50 && monstr[mndx] < 60 && rn2(20)) return TRUE;
	if (monstr[mndx] >= 60 && monstr[mndx] < 70 && rn2(50)) return TRUE;
	if (monstr[mndx] >= 70 && rn2(100)) return TRUE;

	/*if (Inhell)
		return(mons[mndx].maligntyp > A_NEUTRAL);
	else
		return((mons[mndx].geno & G_HELL) != 0);*/
	return FALSE;
}

/*
 *	shift the probability of a monster's generation by
 *	comparing the dungeon alignment and monster alignment.
 *	return an integer in the range of 0-5.
 */
STATIC_OVL int
align_shift(ptr)
register struct permonst *ptr;
{
    static NEARDATA long oldmoves = 0L;	/* != 1, starting value of moves */
    static NEARDATA s_level *lev;
    register int alshift;
	aligntyp alignment;

	alignment = ptr->maligntyp;
	alignment = (alignment > 0) ? A_LAWFUL :
		(alignment < 0) ? A_CHAOTIC :
		A_NEUTRAL;

    if(oldmoves != moves) {
	lev = Is_special(&u.uz);
	oldmoves = moves;
    }
    switch((lev) ? lev->flags.align : dungeons[u.uz.dnum].flags.align) {
    default:	/* just in case */
    case AM_NONE:	alshift = 0;
			break;
    case AM_LAWFUL:	alshift = (ptr->maligntyp+20)/(2*ALIGNWEIGHT);
	if (alignment == A_LAWFUL && u.ualign.type == A_CHAOTIC) alshift++;
			break;
    case AM_NEUTRAL:	alshift = (20 - abs(ptr->maligntyp))/ALIGNWEIGHT;
		if (alignment == A_NEUTRAL && u.ualign.type == A_NEUTRAL) alshift++;
			break;
    case AM_CHAOTIC:	alshift = (-(ptr->maligntyp-20))/(2*ALIGNWEIGHT);
		if (alignment == A_CHAOTIC && u.ualign.type == A_LAWFUL) alshift++;
			break;
    }

	/* player's alignment should have a bigger effect --Amy */
	if (alignment == A_NEUTRAL && u.ualign.type == A_NEUTRAL) alshift++;
	if (alignment == A_CHAOTIC && u.ualign.type == A_LAWFUL) alshift++;
	if (alignment == A_LAWFUL && u.ualign.type == A_CHAOTIC) alshift++;

	/* make the game harder for player liches */
	if (Race_if(PM_LICH_WARRIOR) && ptr->mlevel > 9) alshift++;
	if (Race_if(PM_LICH_WARRIOR) && ptr->mlevel > 19) alshift++;
	if (Race_if(PM_LICH_WARRIOR) && ptr->mlevel > 29) alshift++;
	if (Race_if(PM_LICH_WARRIOR) && ptr->mlevel > 39) alshift += 2;

	/* and also for rodney players */

	if (Race_if(PM_RODNEYAN) && ptr->mlevel > 9) alshift++;
	if (Race_if(PM_RODNEYAN) && ptr->mlevel > 19) alshift += 2;
	if (Race_if(PM_RODNEYAN) && ptr->mlevel > 29) alshift += 4;
	if (Race_if(PM_RODNEYAN) && ptr->mlevel > 39) alshift += 10;
	if (Race_if(PM_RODNEYAN) && ptr->mlevel > 49) alshift += 25;

    return alshift;
}

static NEARDATA struct {
	int choice_count;
	/*char*/int mchoices[SPECIAL_PM];	/* value range is 0..127, Amy edit: way too few! */
} rndmonst_state = { -1, {0} };

/* select a random monster type */
struct permonst *
rndmonst()
{
	register struct permonst *ptr;
	register int mndx, ct;

	int randmnst;
	int randmnsx;

	if(u.ukinghill){ /* You have pirate quest artifact in open inventory */
		if(rnd(100)>98){
			if(In_endgame(&u.uz)) return &mons[PM_GITHYANKI_PIRATE];
			else if(Inhell) return &mons[PM_DAMNED_PIRATE];
			else return &mons[PM_SKELETAL_PIRATE];
		}
	}
	
	if (!rn2(100)) { /* all monster "teams" have at least some chance of being selected --Amy */
		randmnst = (rn2(187) + 1);
		randmnsx = (rn2(100) + 1);

		if (randmnst < 6)
	 	    return(mkclass(S_ANT,0));
		else if (randmnst < 9)
		    return(mkclass(S_BLOB,0));
			else if (randmnst < 11)
		    return(mkclass(S_COCKATRICE,0));
			else if (randmnst < 15)
		    return(mkclass(S_DOG,0));
			else if (randmnst < 18)
		    return(mkclass(S_EYE,0));
			else if (randmnst < 22)
		    return(mkclass(S_FELINE,0));
			else if (randmnst < 24)
		    return(mkclass(S_GREMLIN,0));
			else if (randmnst < 29)
		    return(mkclass(S_HUMANOID,0));
			else if (randmnst < 33)
		    return(mkclass(S_IMP,0));
			else if (randmnst < 36)
		    return(mkclass(S_JELLY,0));
			else if (randmnst < 41)
		    return(mkclass(S_KOBOLD,0));
			else if (randmnst < 44)
		    return(mkclass(S_LEPRECHAUN,0));
			else if (randmnst < 47)
		    return(mkclass(S_MIMIC,0));
			else if (randmnst < 50)
		    return(mkclass(S_NYMPH,0));
			else if (randmnst < 54)
		    return(mkclass(S_ORC,0));
			else if (randmnst < 55)
		    return(mkclass(S_PIERCER,0));
			else if (randmnst < 58)
		    return(mkclass(S_QUADRUPED,0));
			else if (randmnst < 62)
		    return(mkclass(S_RODENT,0));
			else if (randmnst < 65)
		    return(mkclass(S_SPIDER,0));
			else if (randmnst < 66)
		    return(mkclass(S_TRAPPER,0));
			else if (randmnst < 69)
		    return(mkclass(S_UNICORN,0));
			else if (randmnst < 71)
		    return(mkclass(S_VORTEX,0));
			else if (randmnst < 73)
		    return(mkclass(S_WORM,0));
			else if (randmnst < 75)
		    return(mkclass(S_XAN,0));
			else if (randmnst < 76)
		    return(mkclass(S_LIGHT,0));
			else if (randmnst < 77)
		    return(mkclass(S_ZOUTHERN,0));
			else if (randmnst < 78)
		    return(mkclass(S_ANGEL,0));
			else if (randmnst < 81)
		    return(mkclass(S_BAT,0));
			else if (randmnst < 83)
		    return(mkclass(S_CENTAUR,0));
			else if (randmnst < 86)
		    return(mkclass(S_DRAGON,0));
			else if (randmnst < 89)
		    return(mkclass(S_ELEMENTAL,0));
			else if (randmnst < 94)
		    return(mkclass(S_FUNGUS,0));
			else if (randmnst < 99)
		    return(mkclass(S_GNOME,0));
			else if (randmnst < 102)
		    return(mkclass(S_GIANT,0));
			else if (randmnst < 103)
		    return(mkclass(S_JABBERWOCK,0));
			else if (randmnst < 104)
		    return(mkclass(S_KOP,0));
			else if (randmnst < 105)
		    return(mkclass(S_LICH,0));
			else if (randmnst < 108)
		    return(mkclass(S_MUMMY,0));
			else if (randmnst < 110)
		    return(mkclass(S_NAGA,0));
			else if (randmnst < 113)
		    return(mkclass(S_OGRE,0));
			else if (randmnst < 115)
		    return(mkclass(S_PUDDING,0));
			else if (randmnst < 116)
		    return(mkclass(S_QUANTMECH,0));
			else if (randmnst < 118)
		    return(mkclass(S_RUSTMONST,0));
			else if (randmnst < 121)
		    return(mkclass(S_SNAKE,0));
			else if (randmnst < 123)
		    return(mkclass(S_TROLL,0));
			else if (randmnst < 124)
		    return(mkclass(S_UMBER,0));
			else if (randmnst < 125)
		    return(mkclass(S_VAMPIRE,0));
			else if (randmnst < 127)
		    return(mkclass(S_WRAITH,0));
			else if (randmnst < 128)
		    return(mkclass(S_XORN,0));
			else if (randmnst < 130)
		    return(mkclass(S_YETI,0));
			else if (randmnst < 135)
		    return(mkclass(S_ZOMBIE,0));
			else if (randmnst < 145)
		    return(mkclass(S_HUMAN,0));
			else if (randmnst < 147)
		    return(mkclass(S_GHOST,0));
			else if (randmnst < 149)
		    return(mkclass(S_GOLEM,0));
			else if (randmnst < 152)
		    return(mkclass(S_DEMON,0));
			else if (randmnst < 155)
		    return(mkclass(S_EEL,0));
			else if (randmnst < 160)
		    return(mkclass(S_LIZARD,0));
			else if (randmnst < 162)
		    return(mkclass(S_BAD_FOOD,0));
			else if (randmnst < 165)
		    return(mkclass(S_BAD_COINS,0));
			else if (randmnst < 166) {
				if (randmnsx < 96)
		 	    return(mkclass(S_HUMAN,0));
				else
		 	    return(mkclass(S_NEMESE,0));
				}
			else if (randmnst < 171)
		    return(mkclass(S_GRUE,0));
			else if (randmnst < 176)
		    return(mkclass(S_WALLMONST,0));
			else if (randmnst < 180)
		    return(mkclass(S_RUBMONST,0));
			else if (randmnst < 181) {
				if (randmnsx < 99)
		    return(mkclass(S_HUMAN,0));
				else
		    return(mkclass(S_ARCHFIEND,0));
				}
			else if (randmnst < 186)
		    return(mkclass(S_TURRET,0));
			else if (randmnst < 187)
		    return(mkclass(S_FLYFISH,0));

			/* otherwise, don't return */
	}

/* [Tom] this was locking up priest quest... who knows why? */
/* fixed it! no 'W' class monsters with corpses! oops! */
/*    if(u.uz.dnum == quest_dnum && (ptr = qt_montype())) return(ptr); */
/*	if(u.uz.dnum == quest_dnum) {
		if ((ptr = qt_montype())) {
			return(ptr);
		}
	}*/

	/* KMH -- February 2 is Groundhog Day! */
	if (Is_oracle_level(&u.uz) && (!!flags.groundhogday ^ !rn2(20)))
		return (&mons[PM_WOODCHUCK]);

      if (u.uz.dnum == quest_dnum && (!rn2(7)) && (ptr = qt_montype()) != 0)
	    return ptr; 

	if (rndmonst_state.choice_count < 0) {	/* need to recalculate */
	    int zlevel, minmlev, maxmlev;
	    boolean elemlevel;
#ifdef REINCARNATION
	    boolean upper;
#endif
	    rndmonst_state.choice_count = 0;
	    /* look for first common monster */
	    for (mndx = LOW_PM; mndx < SPECIAL_PM; mndx++) {
		if (!uncommon(mndx)) break;
		rndmonst_state.mchoices[mndx] = 0;
	    }		
	    if (mndx == SPECIAL_PM) {
		/* evidently they've all been exterminated */
#ifdef DEBUG
		pline("rndmonst: no common mons!");
#endif
		return (struct permonst *)0;
	    } /* else `mndx' now ready for use below */
	    zlevel = level_difficulty();
	    /* determine the level of the weakest monster to make. */
	    minmlev = /*zlevel / 6*/0;
	    /* determine the level of the strongest monster to make. */
	    maxmlev = (zlevel + u.ulevel + 1)>>1;
	    if (!rn2(100)) maxmlev *= 2;
	    if (!rn2(1000)) maxmlev *= 4;
	    if (!rn2(10000)) maxmlev = 127;
		if (maxmlev > 127) maxmlev = 127; /* maxmlev is an int, but better safe than sorry. --Amy */
	    if (issoviet && maxmlev > 2) minmlev = (maxmlev / 2);
		/* In Soviet Russia, things can never be difficult enough. Don't bother the player with weak stuff like newts,
		 * when we could spawn all kinds of barbazus, chthonians and great wyrms of the elements in Gehennom. --Amy */
#ifdef REINCARNATION
	    upper = Is_rogue_level(&u.uz);
#endif
	    elemlevel = In_endgame(&u.uz) && !Is_astralevel(&u.uz);

/*
 *	Find out how many monsters exist in the range we have selected.
 */
	     
loopback:
	    /* (`mndx' initialized above) */
	    for ( ; mndx < SPECIAL_PM; mndx++) {
		ptr = &mons[mndx];
		rndmonst_state.mchoices[mndx] = 0;
		if (tooweak(mndx, minmlev) || (rn2(20) ? toostrong(mndx, maxmlev) : ptr->mlevel > maxmlev ) )
		    continue;
#ifdef REINCARNATION
		if (upper && !isupper((int)def_monsyms[(int)(ptr->mlet)]) && rn2(5) ) continue;
#endif
		if (elemlevel && wrong_elem_type(ptr) && rn2(20) ) continue;
		if (uncommon(mndx)) continue;
		/*if (Inhell && (ptr->geno & G_NOHELL)) continue;*/
		ct = (int)(ptr->geno & G_FREQ);

		/* mystics have different generation frequencies. I decided that it depends on their gender, too. --Amy */

		if (ct > 0 && Role_if(PM_MYSTIC) && flags.female) ct = 1;

		if (ct && (ct + align_shift(ptr)) > 0) ct += align_shift(ptr);

		if (ct > 0 && Role_if(PM_MYSTIC) && !flags.female) ct = 1;

		/* evil patch - one monster class is always generated with a higher frequency (even for mystics) --Amy */
		if (ct > 0 && (ptr->mlet == u.frequentmonster)) ct += u.freqmonsterbonus;
		if (ct > 0 && (ptr->mlet == urole.enemy1sym)) ct += 5;
		if (ct > 0 && (ptr->mlet == urole.enemy2sym)) ct += 2;

		if (ct > 0 && (ptr->mcolor == u.frequentcolor)) ct += u.freqcolorbonus;

		if (ct > 0 && (mndx == u.frequentspecies)) ct += u.freqspeciesbonus;
		if (ct > 0 && (mndx == u.frequentspecies2)) ct += u.freqspeciesbonus2;
		if (ct > 0 && (mndx == u.frequentspecies3)) ct += u.freqspeciesbonus3;
		if (ct > 0 && (mndx == u.frequentspecies4)) ct += u.freqspeciesbonus4;
		if (ct > 0 && (mndx == u.frequentspecies5)) ct += u.freqspeciesbonus5;
		if (ct > 0 && (mndx == u.frequentspecies6)) ct += u.freqspeciesbonus6;
		if (ct > 0 && (mndx == u.frequentspecies7)) ct += u.freqspeciesbonus7;
		if (ct > 0 && (mndx == u.frequentspecies8)) ct += u.freqspeciesbonus8;
		if (ct > 0 && (mndx == u.frequentspecies9)) ct += u.freqspeciesbonus9;
		if (ct > 0 && (mndx == u.frequentspecies10)) ct += u.freqspeciesbonus10;
		if (ct > 0 && (mndx == u.frequentspecies11)) ct += u.freqspeciesbonus;
		if (ct > 0 && (mndx == u.frequentspecies12)) ct += u.freqspeciesbonus2;
		if (ct > 0 && (mndx == u.frequentspecies13)) ct += u.freqspeciesbonus3;
		if (ct > 0 && (mndx == u.frequentspecies14)) ct += u.freqspeciesbonus4;
		if (ct > 0 && (mndx == u.frequentspecies15)) ct += u.freqspeciesbonus5;
		if (ct > 0 && (mndx == u.frequentspecies16)) ct += u.freqspeciesbonus6;
		if (ct > 0 && (mndx == u.frequentspecies17)) ct += u.freqspeciesbonus7;
		if (ct > 0 && (mndx == u.frequentspecies18)) ct += u.freqspeciesbonus8;
		if (ct > 0 && (mndx == u.frequentspecies19)) ct += u.freqspeciesbonus9;
		if (ct > 0 && (mndx == u.frequentspecies20)) ct += u.freqspeciesbonus10;

		if (ct > 0 && (urole.enemy1num != NON_PM) && (mndx == urole.enemy1num)) ct += 125;
		if (ct > 0 && (urole.enemy2num != NON_PM) && (mndx == urole.enemy2num)) ct += 25;

		if (ct > 0 && (mndx == u.nospawnspecies)) ct = 0;
		if (ct > 0 && (mndx == u.nospawnspecies2)) ct = 0;
		if (ct > 0 && (mndx == u.nospawnspecies3)) ct = 0;
		if (ct > 0 && (mndx == u.nospawnspecies4)) ct = 0;
		if (ct > 0 && (mndx == u.nospawnspecies5)) ct = 0;
		if (ct > 0 && (mndx == u.nospawnspecies6)) ct = 0;
		if (ct > 0 && (mndx == u.nospawnspecies7)) ct = 0;
		if (ct > 0 && (mndx == u.nospawnspecies8)) ct = 0;
		if (ct > 0 && (mndx == u.nospawnspecies9)) ct = 0;
		if (ct > 0 && (mndx == u.nospawnspecies10)) ct = 0;

		/*if (ct < 0 || ct > 127)
		    panic("rndmonst: bad count [#%d: %d]", mndx, ct);*/
		if (ct < 0) {pline("rndmonst: bad count [#%d: %d]", mndx, ct); ct = 0;}
		if (ct > 10000) {pline("rndmonst: bad count [#%d: %d]", mndx, ct); ct = 10000;} /* arbitrary --Amy */
		rndmonst_state.choice_count += ct;
		rndmonst_state.mchoices[mndx] = ct;
	    }
/*
 *	    Possible modification:  if choice_count is "too low",
 *	    expand minmlev..maxmlev range and try again.
 */
	} /* choice_count+mchoices[] recalc */

	if (rndmonst_state.choice_count <= 0) {
	    /* maybe no common mons left, or all are too weak or too strong */
#ifdef DEBUG
	    Norep("rndmonst: choice_count=%d", rndmonst_state.choice_count);
#endif
	    return (struct permonst *)0;
	}

/*
 *	Now, select a monster at random.
 */
	ct = rnd(rndmonst_state.choice_count);
	for (mndx = LOW_PM; mndx < SPECIAL_PM; mndx++)
	    if ((ct -= (int)rndmonst_state.mchoices[mndx]) <= 0) break;

	/*if (mndx == SPECIAL_PM || uncommon(mndx)) {*/	/* shouldn't happen */
		/* but Amy edit: possible due to new spawn routine that involves the uncommon function */
	    /*impossible("rndmonst: bad `mndx' [#%d]", mndx);
	    return (struct permonst *)0;
	}*/

	return &mons[mndx];

/*
 * [Tom] Your rival adventurers are a special case:
 *       - They have varying levels, and thus can always appear
 *       - But they need to be sorta rare or else they're a large
 *         percentage of the dungeon inhabitants.
 */

	/* Sorry, but this code is not working, so I'm commenting it out. --Amy */
/*	      if ((monsndx(ptr-1) >= PM_ARCHEOLOGIST) &&
		  (monsndx(ptr-1) <= PM_WIZARD)
		   && (rn2(4))) goto loopback;
*/

}

/* called when you change level (experience or dungeon depth) or when
   monster species can no longer be created (genocide or extinction) */
void
reset_rndmonst(mndx)
int mndx;	/* particular species that can no longer be created */
{
	/* cached selection info is out of date */
	if (mndx == NON_PM) {
	    rndmonst_state.choice_count = -1;	/* full recalc needed */
	} else if (mndx < SPECIAL_PM) {
	    rndmonst_state.choice_count -= rndmonst_state.mchoices[mndx];
	    rndmonst_state.mchoices[mndx] = 0;
	} /* note: safe to ignore extinction of unique monsters */
}

#endif /* OVL0 */
#ifdef OVL1

/*	The routine below is used to make one of the multiple types
 *	of a given monster class.  The second parameter specifies a
 *	special casing bit mask to allow the normal genesis
 *	masks to be deactivated.  Returns 0 if no monsters
 *	in that class can be made.
 */

struct permonst *
mkclass(class,spc)
char	class;
int	spc;
{
	register int    first;
	int tryct = 0;

newtry:
	first = pm_mkclass(class,spc);

	if ((first == -1) && tryct < 1000) {
		tryct++;
		goto newtry;
	}

	if ((mons[first].mlet != class) && tryct < 1000) {
		tryct++;
		goto newtry;
	}
	
	if (first == -1) return((struct permonst *) 0);

	return(&mons[first]);
}

/* Called by mkclass() - returns the pm of the monster 
 * Returns -1 (PM_PLAYERMON) if can't find monster of the class
 *
 * spc may have G_UNIQ and/or G_NOGEN set to allow monsters of
 * this type (otherwise they will be ignored). It may also have
 * MKC_ULIMIT set to place an upper limit on the difficulty of
 * the monster returned.
 */
int
pm_mkclass(class,spc)
char    class;
int     spc;
{
	register int	first, last, num = 0;
	int maxmlev, mask = (G_NOGEN | G_UNIQ) & ~spc;
	if (!rn2(20) || Role_if(PM_TRANSSYLVANIAN) ) mask = (G_NOGEN) & ~spc;

	int uncommontwo = 0;
	int uncommonthree = 0;
	int uncommonfive = 0;
	int uncommonseven = 0;
	int uncommonten = 0;

	if (!issoviet) {
		uncommontwo = rn2(2) ? 1 : 0;
		uncommonthree = rn2(3) ? 1 : 0;
		uncommonfive = rn2(5) ? 1 : 0;
		uncommonseven = rn2(7) ? 1 : 0;
		uncommonten = rn2(10) ? 1 : 0;
	} else {
		uncommontwo = !rn2(3) ? 1 : 0;
		uncommonthree = rn2(2) ? 1 : 0;
		uncommonfive = rn2(3) ? 1 : 0;
		uncommonseven = rn2(4) ? 1 : 0;
		uncommonten = rn2(5) ? 1 : 0;
	}

	int uncommonnewten = !rn2(10) ? 1 : 0;
	int uncommonnewfifteen = !rn2(5) ? 1 : 0;
	int uncommonnewtwenty = (rnd(10) > 3) ? 1 : 0;
	int uncommonnewtwentyfive = (rnd(10) > 4) ? 1 : 0;
	int uncommonnewthirty = !rn2(2) ? 1 : 0;
	int uncommonnewthirtyfive = (rnd(10) > 6) ? 1 : 0;
	int uncommonnewforty = (rnd(10) > 7) ? 1 : 0;
	int uncommonnewfortyfive = rn2(5) ? 1 : 0;
	int uncommonnewfifty = rn2(10) ? 1 : 0;
	int uncommonnewsixty = rn2(20) ? 1 : 0;
	int uncommonnewseventy = rn2(50) ? 1 : 0;

	int bonuslevel;
	boolean calctype;

	/*maxmlev = level_difficulty() >> 1;*/ /* what the heck? does that divide the actual result by 2?! --Amy */
	  maxmlev = monster_difficulty();
	    if (!rn2(100)) maxmlev *= 2;
	    if (!rn2(1000)) maxmlev *= 4;
	    if (!rn2(5)) maxmlev += rnz(2);
	    if (!rn2(20)) maxmlev += rnz(3);
	    if (!rn2(100)) maxmlev += rnz(4);
	    if (!rn2(10000)) maxmlev = 127;
		if (maxmlev > 127) maxmlev = 127; /* very important! The game might otherwise crash or become unstable! */
	if(class < 1 || class >= MAXMCLASSES) {
	    impossible("mkclass called with bad class!");
	    return(-1);
	}

	/*pline("max spawn level %d",maxmlev);*/

/*	Assumption #1:	monsters of a given class are contiguous in the
 *			mons[] array.
 */
	for (first = LOW_PM; first < SPECIAL_PM; first++)
	    if (mons[first].mlet == class) break;
	if (first == SPECIAL_PM) return (-1);

	if (rn2(5)) calctype = 1;
	else calctype = 0;

	for (last = first;
		last < SPECIAL_PM && mons[last].mlet == class; last++)
	    if (!(mvitals[last].mvflags & G_GONE) && !(mons[last].geno & mask)
					&& !is_placeholder(&mons[last])
					&& !(uncommontwo && uncommon2(&mons[last]) && !Race_if(PM_RODNEYAN) )
					&& !(uncommonthree && uncommon3(&mons[last]) && !Race_if(PM_RODNEYAN) )
					&& !(uncommonfive && uncommon5(&mons[last]) && !Race_if(PM_RODNEYAN) )
					&& !(uncommonseven && uncommon7(&mons[last]) && !Race_if(PM_RODNEYAN) )
					&& !(uncommonten && uncommon10(&mons[last]) && !Race_if(PM_RODNEYAN) )
					&& !(uncommonnewten && monstr[last] >= 10 && monstr[last] < 15 )
					&& !(uncommonnewfifteen && monstr[last] >= 15 && monstr[last] < 20 )
					&& !(uncommonnewtwenty && monstr[last] >= 20 && monstr[last] < 25 )
					&& !(uncommonnewtwentyfive && monstr[last] >= 25 && monstr[last] < 30 )
					&& !(uncommonnewthirty && monstr[last] >= 30 && monstr[last] < 35 )
					&& !(uncommonnewthirtyfive && monstr[last] >= 35 && monstr[last] < 40 )
					&& !(uncommonnewforty && monstr[last] >= 40 && monstr[last] < 45 )
					&& !(uncommonnewfortyfive && monstr[last] >= 45 && monstr[last] < 50 )
					&& !(uncommonnewfifty && monstr[last] >= 50 && monstr[last] < 60 )
					&& !(uncommonnewsixty && monstr[last] >= 60 && monstr[last] < 70 )
					&& !(uncommonnewseventy && monstr[last] >= 70 )
					&& (last != u.nospawnspecies) && (last != u.nospawnspecies2) && (last != u.nospawnspecies3) && (last != u.nospawnspecies4) && (last != u.nospawnspecies5) && (last != u.nospawnspecies6) && (last != u.nospawnspecies7) && (last != u.nospawnspecies8) && (last != u.nospawnspecies9) && (last != u.nospawnspecies10)

				) {
		/* consider it */
		/*if(spc & MKC_ULIMIT && toostrong(last, 4 * maxmlev)) break;*/

		bonuslevel = 0;
		if (!rn2(5)) bonuslevel += rnd(5);
		if (!rn2(20)) bonuslevel += rnd(7);
		if (!rn2(100)) bonuslevel += rnd(10);
		if (!rn2(500)) bonuslevel += rnd(15);
		if (!rn2(2500)) bonuslevel += rnd(20);
		if (!rn2(20000)) bonuslevel += rnd(30);
		if (!rn2(50000)) bonuslevel += rnd(50);
		if (!rn2(100000)) bonuslevel += rnd(100);

		if(num && (calctype ? toostrong(last, (maxmlev + bonuslevel) ) : mons[last].mlevel > (maxmlev + bonuslevel) ) &&
		   monstr[last] != monstr[last-1]) break;

		num += mons[last].geno & G_FREQ;
		if (last == u.frequentspecies) num += u.freqspeciesbonus;
		if (last == u.frequentspecies2) num += u.freqspeciesbonus2;
		if (last == u.frequentspecies3) num += u.freqspeciesbonus3;
		if (last == u.frequentspecies4) num += u.freqspeciesbonus4;
		if (last == u.frequentspecies5) num += u.freqspeciesbonus5;
		if (last == u.frequentspecies6) num += u.freqspeciesbonus6;
		if (last == u.frequentspecies7) num += u.freqspeciesbonus7;
		if (last == u.frequentspecies8) num += u.freqspeciesbonus8;
		if (last == u.frequentspecies9) num += u.freqspeciesbonus9;
		if (last == u.frequentspecies10) num += u.freqspeciesbonus10;
		if (last == u.frequentspecies11) num += u.freqspeciesbonus;
		if (last == u.frequentspecies12) num += u.freqspeciesbonus2;
		if (last == u.frequentspecies13) num += u.freqspeciesbonus3;
		if (last == u.frequentspecies14) num += u.freqspeciesbonus4;
		if (last == u.frequentspecies15) num += u.freqspeciesbonus5;
		if (last == u.frequentspecies16) num += u.freqspeciesbonus6;
		if (last == u.frequentspecies17) num += u.freqspeciesbonus7;
		if (last == u.frequentspecies18) num += u.freqspeciesbonus8;
		if (last == u.frequentspecies19) num += u.freqspeciesbonus9;
		if (last == u.frequentspecies20) num += u.freqspeciesbonus10;
		if (mons[last].mcolor == u.frequentcolor) num += u.freqcolorbonus;

		if ((urole.enemy1num != NON_PM) && (last == urole.enemy1num)) num += 125;
		if ((urole.enemy2num != NON_PM) && (last == urole.enemy2num)) num += 25;

	    }

	if(!num) return(-1);

/*	Assumption #2:	monsters of a given class are presented in ascending
 *			order of strength.
 */
	for(num = rnd(num); num > 0; first++)
	    if (!(mvitals[first].mvflags & G_GONE) && !(mons[first].geno & mask)
					&& !is_placeholder(&mons[first])
					&& !(uncommontwo && uncommon2(&mons[first]) && !Race_if(PM_RODNEYAN) )
					&& !(uncommonthree && uncommon3(&mons[first]) && !Race_if(PM_RODNEYAN) )
					&& !(uncommonfive && uncommon5(&mons[first]) && !Race_if(PM_RODNEYAN) )
					&& !(uncommonseven && uncommon7(&mons[first]) && !Race_if(PM_RODNEYAN) )
					&& !(uncommonten && uncommon10(&mons[first]) && !Race_if(PM_RODNEYAN) )
					&& !(uncommonnewten && monstr[first] >= 10 && monstr[first] < 15 )
					&& !(uncommonnewfifteen && monstr[first] >= 15 && monstr[first] < 20 )
					&& !(uncommonnewtwenty && monstr[first] >= 20 && monstr[first] < 25 )
					&& !(uncommonnewtwentyfive && monstr[first] >= 25 && monstr[first] < 30 )
					&& !(uncommonnewthirty && monstr[first] >= 30 && monstr[first] < 35 )
					&& !(uncommonnewthirtyfive && monstr[first] >= 35 && monstr[first] < 40 )
					&& !(uncommonnewforty && monstr[first] >= 40 && monstr[first] < 45 )
					&& !(uncommonnewfortyfive && monstr[first] >= 45 && monstr[first] < 50 )
					&& !(uncommonnewfifty && monstr[first] >= 50 && monstr[first] < 60 )
					&& !(uncommonnewsixty && monstr[first] >= 60 && monstr[first] < 70 )
					&& !(uncommonnewseventy && monstr[first] >= 70 )
					&& (first != u.nospawnspecies) && (first != u.nospawnspecies2) && (first != u.nospawnspecies3) && (first != u.nospawnspecies4) && (first != u.nospawnspecies5) && (first != u.nospawnspecies6) && (first != u.nospawnspecies7) && (first != u.nospawnspecies8) && (first != u.nospawnspecies9) && (first != u.nospawnspecies10)
				) {
		/* skew towards lower value monsters at lower exp. levels */
		num -= mons[first].geno & G_FREQ;
		if (first == u.frequentspecies) num -= u.freqspeciesbonus;
		if (first == u.frequentspecies2) num -= u.freqspeciesbonus2;
		if (first == u.frequentspecies3) num -= u.freqspeciesbonus3;
		if (first == u.frequentspecies4) num -= u.freqspeciesbonus4;
		if (first == u.frequentspecies5) num -= u.freqspeciesbonus5;
		if (first == u.frequentspecies6) num -= u.freqspeciesbonus6;
		if (first == u.frequentspecies7) num -= u.freqspeciesbonus7;
		if (first == u.frequentspecies8) num -= u.freqspeciesbonus8;
		if (first == u.frequentspecies9) num -= u.freqspeciesbonus9;
		if (first == u.frequentspecies10) num -= u.freqspeciesbonus10;
		if (first == u.frequentspecies11) num -= u.freqspeciesbonus;
		if (first == u.frequentspecies12) num -= u.freqspeciesbonus2;
		if (first == u.frequentspecies13) num -= u.freqspeciesbonus3;
		if (first == u.frequentspecies14) num -= u.freqspeciesbonus4;
		if (first == u.frequentspecies15) num -= u.freqspeciesbonus5;
		if (first == u.frequentspecies16) num -= u.freqspeciesbonus6;
		if (first == u.frequentspecies17) num -= u.freqspeciesbonus7;
		if (first == u.frequentspecies18) num -= u.freqspeciesbonus8;
		if (first == u.frequentspecies19) num -= u.freqspeciesbonus9;
		if (first == u.frequentspecies20) num -= u.freqspeciesbonus10;
		if (mons[first].mcolor == u.frequentcolor) num -= u.freqcolorbonus;
		
		if ((urole.enemy1num != NON_PM) && (first == urole.enemy1num)) num -= 125;
		if ((urole.enemy2num != NON_PM) && (first == urole.enemy2num)) num -= 25;

		/* or not, because seriously... what the heck??? --Amy */
		/* if (num && adj_lev(&mons[first]) > (u.ulevel*2)) { */
		    /* but not when multiple monsters are same level */
		    /* if (mons[first].mlevel != mons[first+1].mlevel)
			num--;
		} */
	    }
	first--; /* correct an off-by-one error */

	return(first);
}

int
adj_lev(ptr)	/* adjust strength of monsters based on u.uz and u.ulevel */
register struct permonst *ptr;
{
	int	tmp, tmp2;

	if (ptr == &mons[PM_WIZARD_OF_YENDOR]) {
		/* does not depend on other strengths, but does get stronger
		 * every time he is killed
		 */
		tmp = ptr->mlevel + mvitals[PM_WIZARD_OF_YENDOR].died;
		if (tmp > 49) tmp = 49;
		return tmp;
	}

	if((tmp = ptr->mlevel) > 49) return(50); /* "special" demons/devils */
	tmp2 = (level_difficulty() - tmp);
	if(tmp2 < 0) tmp--;		/* if mlevel > u.uz decrement tmp */
	else tmp += (tmp2 / 5);		/* else increment 1 per five diff */

	tmp2 = (u.ulevel - ptr->mlevel);	/* adjust vs. the player */
	if(tmp2 > 0) tmp += (tmp2 / 4);		/* level as well */

	tmp2 = (3 * ((int) ptr->mlevel))/ 2;	/* crude upper limit */

	/* adjustments by Amy */
	/* jonadab wants monsters to spawn with up to 8x their base level, but that's really excessive. */
	if (!rn2(2) && (tmp2 < 6) && (level_difficulty() > 9)) { tmp++; tmp2++;}
	if (!rn2(2) && (tmp2 < 6) && (level_difficulty() > 19)) { tmp++; tmp2++;}
	if (!rn2(3) && (tmp2 < 11) && (level_difficulty() > 19)) { tmp++; tmp2++;}
	if (!rn2(2) && (tmp2 < 8) && (level_difficulty() > 29)) { tmp++; tmp2++;}
	if (!rn2(3) && (tmp2 < 16) && (level_difficulty() > 29)) { tmp++; tmp2++;}
	if (!rn2(2) && (tmp2 < 10) && (level_difficulty() > 39)) { tmp++; tmp2++;}
	if (!rn2(3) && (tmp2 < 21) && (level_difficulty() > 39)) { tmp++; tmp2++;}
	if (!rn2(2) && (tmp2 < 12) && (level_difficulty() > 49)) { tmp++; tmp2++;}
	if (!rn2(3) && (tmp2 < 24) && (level_difficulty() > 49)) { tmp++; tmp2++;}
	if (!rn2(2) && (tmp2 < 14) && (level_difficulty() > 74)) { tmp++; tmp2++;}
	if (!rn2(3) && (tmp2 < 27) && (level_difficulty() > 74)) { tmp++; tmp2++;}
	if (!rn2(2) && (tmp2 < 16) && (level_difficulty() > 99)) { tmp++; tmp2++;}
	if (!rn2(3) && (tmp2 < 30) && (level_difficulty() > 99)) { tmp++; tmp2++;}

	/* Another adjustment for rodneyans, because they're too strong. Junethack 2016 showed how much too strong, so let's
	 * give higher levels to monsters which will hopefully enable them to sometimes survive the rodneyan's stronger
	 * melee attacks! Although I'm fairly sure the rodneyan is still too powerful even with this change. --Amy */
	if (Race_if(PM_RODNEYAN)) {

		if (!rn2(2) && (tmp2 < 6) && (level_difficulty() > 9)) { tmp++; tmp2++;}
		if (!rn2(2) && (tmp2 < 6) && (level_difficulty() > 19)) { tmp++; tmp2++;}
		if (!rn2(3) && (tmp2 < 11) && (level_difficulty() > 19)) { tmp++; tmp2++;}
		if (!rn2(2) && (tmp2 < 8) && (level_difficulty() > 29)) { tmp++; tmp2++;}
		if (!rn2(3) && (tmp2 < 16) && (level_difficulty() > 29)) { tmp++; tmp2++;}
		if (!rn2(2) && (tmp2 < 10) && (level_difficulty() > 39)) { tmp++; tmp2++;}
		if (!rn2(3) && (tmp2 < 21) && (level_difficulty() > 39)) { tmp++; tmp2++;}
		if (!rn2(2) && (tmp2 < 12) && (level_difficulty() > 49)) { tmp++; tmp2++;}
		if (!rn2(3) && (tmp2 < 24) && (level_difficulty() > 49)) { tmp++; tmp2++;}
		if (!rn2(2) && (tmp2 < 14) && (level_difficulty() > 74)) { tmp++; tmp2++;}
		if (!rn2(3) && (tmp2 < 27) && (level_difficulty() > 74)) { tmp++; tmp2++;}
		if (!rn2(2) && (tmp2 < 16) && (level_difficulty() > 99)) { tmp++; tmp2++;}
		if (!rn2(3) && (tmp2 < 30) && (level_difficulty() > 99)) { tmp++; tmp2++;}
		if (!rn2(10)) {
			tmp += rnd(10);
			tmp2 += rnd(10);
		}

	}

	if (tmp2 > 49) tmp2 = 49;		/* hard upper limit */
	return((tmp > tmp2) ? tmp2 : (tmp > 0 ? tmp : 0)); /* 0 lower limit */
}

#endif /* OVL1 */
#ifdef OVLB

struct permonst *
grow_up(mtmp, victim)	/* `mtmp' might "grow up" into a bigger version */
struct monst *mtmp, *victim;
{
	int oldtype, newtype, max_increase, cur_increase,
	    lev_limit, hp_threshold;
	struct permonst *ptr = mtmp->data;

	/* monster died after killing enemy but before calling this function */
	/* currently possible if killing a gas spore */
	if (mtmp->mhp <= 0)
	    return ((struct permonst *)0);

	if (mtmp->oldmonnm != monsndx(ptr))
	    return ptr;		/* No effect if polymorphed */

	/* note:  none of the monsters with special hit point calculations
	   have both little and big forms */
	oldtype = monsndx(ptr);
	newtype = little_to_big(oldtype);
	if (newtype == PM_PRIEST && mtmp->female) newtype = PM_PRIESTESS;

	/* growth limits differ depending on method of advancement */
	if (victim) {		/* killed a monster */
	    /*
	     * The HP threshold is the maximum number of hit points for the
	     * current level; once exceeded, a level will be gained.
	     * Possible bug: if somehow the hit points are already higher
	     * than that, monster will gain a level without any increase in HP.
	     */
	    hp_threshold = mtmp->m_lev * 8;		/* normal limit */
	    if (!mtmp->m_lev)
		hp_threshold = 4;
	    else if (is_golem(ptr))	/* strange creatures */
		hp_threshold = ((mtmp->mhpmax / 10) + 1) * 10 - 1;
	    else if (is_home_elemental(ptr))
		hp_threshold *= 3;
	    lev_limit = /*3 * (int)ptr->mlevel / 2*/50;	/* same as adj_lev() */
	    /* If they can grow up, be sure the level is high enough for that */
	    if (oldtype != newtype && mons[newtype].mlevel > lev_limit)
		lev_limit = (int)mons[newtype].mlevel;
	    /* number of hit points to gain; unlike for the player, we put
	       the limit at the bottom of the next level rather than the top */
	    max_increase = rnd((int)victim->m_lev + 1);
	    if (mtmp->mhpmax + max_increase > hp_threshold + 1)
		max_increase = max((hp_threshold + 1) - mtmp->mhpmax, 0);
		if (max_increase > 1) max_increase = rnd(max_increase); /* nerf by Amy, since it was going way too fast */
		if (mtmp->m_lev > 14) {
		if (rnd(mtmp->m_lev) > 14) max_increase = 0;
		if (rnd(mtmp->m_lev) > 24) max_increase = 0;
		if (rnd(mtmp->m_lev) > 34) max_increase = 0;
		if (rnd(mtmp->m_lev) > 39) max_increase = 0;
		if (rnd(mtmp->m_lev) > 43) max_increase = 0;
		if (rnd(mtmp->m_lev) > 46) max_increase = 0;
		if (rnd(mtmp->m_lev) > 48) max_increase = 0;
		}
	    cur_increase = (max_increase > 1) ? rn2(max_increase) : 0;

	} else {
	    /* a gain level potion or wraith corpse; always go up a level
	       unless already at maximum (49 is hard upper limit except
	       for demon lords, who start at 50 and can't go any higher) */
	    max_increase = cur_increase = rnd(8) + rnd(2);
	    hp_threshold = 0;	/* smaller than `mhpmax + max_increase' */
	    lev_limit = 50;		/* recalc below */
	}

	mtmp->mhpmax += max_increase;
	mtmp->mhp += cur_increase;

	mtmp->m_enmax += max_increase;
	mtmp->m_en += cur_increase;

	if (ishaxor) {
	mtmp->mhpmax += max_increase;
	mtmp->mhp += cur_increase;

	mtmp->m_enmax += max_increase;
	mtmp->m_en += cur_increase;
	}
	
	if (mtmp->mhpmax <= hp_threshold)
	    return ptr;		/* doesn't gain a level */

	/* Allow to grow up even if grown up form would normally be
	 * out of range */
	if (lev_limit < mons[newtype].mlevel)
	    lev_limit = mons[newtype].mlevel;

	if (is_mplayer(ptr)) lev_limit = 30;    /* same as player */
	else if (lev_limit < 5) lev_limit = 5;  /* arbitrary */
	else if (lev_limit > 49) lev_limit = (ptr->mlevel > 49 ? 50 : 49);

	if ((int)++mtmp->m_lev >= mons[newtype].mlevel && newtype != oldtype) {
	    ptr = &mons[newtype];

		/* stupid sensemon function! Screw it, evolving messages will always be displayed now. --Amy */

		/*if (sensemon(mtmp)) {*/
			pline("What? %s is evolving!", mon_nam(mtmp) );
			pline("%s evolved into %s!", mon_nam(mtmp), an(ptr->mname) );
		/*}*/

	    if (mvitals[newtype].mvflags & G_GENOD) {	/* allow G_EXTINCT */
		if (sensemon(mtmp))
		    pline("As %s grows up into %s, %s %s!", mon_nam(mtmp),
			an(ptr->mname), mhe(mtmp),
			nonliving(ptr) ? "expires" : "dies");
		set_mon_data(mtmp, ptr, -1);	/* keep mvitals[] accurate */
		mondied(mtmp);
		return (struct permonst *)0;
	    }
	    set_mon_data(mtmp, ptr, 1);		/* preserve intrinsics */
	    newsym(mtmp->mx, mtmp->my);		/* color may change */
	    lev_limit = (int)mtmp->m_lev;	/* never undo increment */
	}
	/* sanity checks */
	if ((int)mtmp->m_lev > lev_limit) {
	    mtmp->m_lev--;	/* undo increment */
	    /* HP might have been allowed to grow when it shouldn't */
	    if (mtmp->mhpmax == hp_threshold + 1) mtmp->mhpmax--;
	}

	/* Dark Goku is supposed to have over 9000 HP (idea by Bug Sniper). Don't make him lose it from quaffing gain level. --Amy */
	if (mtmp->mhpmax > 5000*8) mtmp->mhpmax = 5000*8;	  /* absolute limit */
	if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;

	if (mtmp->m_enmax > 5000*8) mtmp->m_enmax = 5000*8;     /* absolute limit */
	if (mtmp->m_en > mtmp->m_enmax) mtmp->m_en = mtmp->m_enmax;

	if (mtmp->oldmonnm != monsndx(ptr)) mtmp->oldmonnm = monsndx(ptr);
	return ptr;
}

#endif /* OVLB */
#ifdef OVL1

int
mongets(mtmp, otyp)
register struct monst *mtmp;
register int otyp;
{
	register struct obj *otmp;
	int spe;

	if (!otyp) return 0;
	otmp = mksobj(otyp, TRUE, rn2(50) ? FALSE : TRUE);
	if (otmp) {
	    if (mtmp->data->mlet == S_DEMON) {
		/* demons never get blessed objects */
		if (otmp->blessed) curse(otmp);
	    } else if(is_lminion(mtmp)) {
		/* lawful minions don't get cursed, bad, or rusting objects */
		otmp->cursed = otmp->hvycurse = otmp->prmcurse = FALSE;
		if(otmp->spe < 0) otmp->spe = 0;
		otmp->oerodeproof = TRUE;
	    } else if(In_endgame(&u.uz) && is_mplayer(mtmp->data) && is_sword(otmp)) {
		otmp->spe = (3 + rn2(3));
	    }
/*
 *      This seems to be covered under mkobj.c ...
 *
 *          * STEPHEN WHITE'S NEW CODE *
 *
 *          if ((otmp->otyp == ORCISH_DAGGER      && !rn2(8))  ||
 *              (otmp->otyp == ORCISH_SPEAR       && !rn2(10)) ||
 *              (otmp->otyp == ORCISH_SHORT_SWORD && !rn2(12)))
 *                      otmp->opoisoned = TRUE;
 *
 *           * It could be alread immune to rust ... *
 *          if (!otmp->oerodeproof && !is_rustprone(otmp) &&
 *              ((otmp->otyp >= SPEAR && otmp->otyp <= BULLWHIP) ||
 *               (otmp->otyp >= ELVEN_LEATHER_HELM &&
 *                otmp->otyp <= LEVITATION_BOOTS))) {
 *                   if (!rn2(10-otmp->spe)) otmp->oerodeproof = TRUE;
 *              else if (!rn2(10+otmp->spe)) otmp->oeroded = rn2(3);
 *          }
 */

	    if(otmp->otyp == CANDELABRUM_OF_INVOCATION) {
		otmp->spe = 0;
		otmp->age = 0L;
		otmp->lamplit = FALSE;
		otmp->blessed = otmp->cursed = otmp->hvycurse = otmp->prmcurse = FALSE;
	    } else if (otmp->otyp == BELL_OF_OPENING) {
		otmp->blessed = otmp->cursed = otmp->hvycurse = otmp->prmcurse = FALSE;
	    } else if (otmp->otyp == SPE_BOOK_OF_THE_DEAD) {
		otmp->blessed = FALSE;
		otmp->cursed = TRUE;
	    }

	    /* leaders don't tolerate inferior quality battle gear */
	    if (is_prince(mtmp->data)) {
		if (otmp->oclass == WEAPON_CLASS && otmp->spe < 1)
		    otmp->spe = 1;
		else if (otmp->oclass == ARMOR_CLASS && otmp->spe < 0)
		    otmp->spe = 0;
	    }

	    spe = otmp->spe;
	    if (is_musable(otmp)) otmp->mstartinvent = 1;
	    if (otmp->oclass == WEAPON_CLASS || otmp->oclass == ARMOR_CLASS) otmp->mstartinventB = 1;
	    (void) mpickobj(mtmp, otmp, TRUE);	/* might free otmp */
	    return(spe);
	} else return(0);
}

#endif /* OVL1 */
#ifdef OVLB

int
golemhp(type)
int type;
{
	switch(type) {
		case PM_KARAKASA: return 30;
		case PM_ANIMATED_LIMB: return 30;
		case PM_DARK_FEMALE_ANDROID: return 30;
		case PM_LIVING_IRON_CHAIN: return 30;
		case PM_BLACK_CHAMELEON: return 35;
		case PM_UGLY_STICK: return 35;
		case PM_PAPER_GOLEM: return 36;
		case PM_GREY_CYBORG: return 36;
		case PM_STRAW_GOLEM: return 40;
		case PM_GARGOYLE: return 46;
		case PM_FLYING_SCIMITAR: return 50;
		case PM_CLOTH_GOLEM: return 50;
		case PM_ANIMATED_TORCH: return 50;
		case PM_PLUSH_BEAR_GOLEM: return 50;
		case PM_INTERCEPTOR_DOLL: return 50;

		case PM_FLAME_ATRONACH: return 50;
		case PM_FROST_ATRONACH: return 100;
		case PM_STORM_ATRONACH: return 150;
		case PM_VENOM_ATRONACH: return 200;

		case PM_CURSED_SWORD: return 100;
		case PM_ANIMATED_DAGGER: return 100;
		case PM_SWORD_FAMILIAR: return 150;
		case PM_ROPE_GOLEM: return 60;
		case PM_TIN_SOLDIER: return 60;
		case PM_CLAY_SOLDIER: return 60;
		case PM_LEATHER_GOLEM: return 80;
		case PM_GOLD_GOLEM: return 80;
		case PM_BAMBOO_GOLEM: return 80;
		case PM_EEL_GOLEM: return 80;
		case PM_STONE_SOLDIER: return 80;
		case PM_BRONZE_GOLEM: return 80;
		case PM_BANANA_PEEL_GOLEM: return 80;
		case PM_WOOD_GOLEM: return 100;
		case PM_WOODEN_GOLEM: return 100;
		case PM_SCARECROW: return 100;
		case PM_PETTY_SCARECROW: return 100;
		case PM_WOODEN_FIGURINE: return 110;
		case PM_BARNACLE_COVERED_GOLEM: return 120;
		case PM_CLANKING_CHAINS: return 120;
		case PM_SOOTBALL: return 140;
		case PM_PETTY_SOOTBALL: return 140;
		case PM_SPOOKY_PORTRAIT: return 150;
		case PM_HAUNTED_TEAPOT: return 200;
		case PM_ANIMATED_HOURGLASS: return 225;
		case PM_GRINDER: return 300;
		case PM_MINI_ROBO_KY: return 300;
		case PM_MORBID_MIRROR: return 310;
		case PM_STAINED_GLASS_GOLEM: return 100;
		case PM_LIVING_LECTURN: return 100;
		case PM_ANIMATED_WOODEN_STATUE: return 72;
		case PM_SOW_STUPID_GUY: return 72;
		case PM_TOWER_STATUE: return 72;
		case PM_SUPER_WOODEN_STATUE: return 360; /* quite unbalanced; players who can't handle them should run away */
		case PM_VANISHING_POINT: return 72;
		case PM_LEVEL____ENEMY: return 72;
		case PM_GEODUDE: return 100;
		case PM_RUBBER_GOLEM: return 150;
		case PM_BRASS_GOLEM: return 170;
		case PM_ALLOY_GOLEM: return 180;
		case PM_MIRROR_GOLEM: return 210;
		case PM_TITANIUM_GOLEM: return 230;
		case PM_BRONZE_COLOSSUS: return 270;
		case PM_SILVER_GOLEM: return 290;
		case PM_BANPEI_KUN: return 290;
		case PM_PLATINUM_GOLEM: return 310;
		case PM_ANIMATED_ARMOR: return 100;
		case PM_ANIMATED_BRONZE_STATUE: return 103;
		case PM_MAIN_SCHOOL_GRADUATE: return 103;
		case PM_AKKAD: return 103;
		case PM_DARK_SAILOR: return 103;
		case PM_HYPERTYPE: return 103;
		case PM_ANIMATED_WEDGE_SANDAL: return 100;
		case PM_ANIMATED_IRON_STATUE: return 137;
		case PM_BEST_TEACHER_EVER: return 137;
		case PM_TGWTG: return 137;
		case PM_THE_FOREVER_TRAIN: return 137;
		case PM_AMUSING_THIRTEEN_GRADER: return 137;
		case PM_SECURITY_BOT: return 137;
		case PM_ANIMATED_MARBLE_STATUE: return 199;
		case PM_SCHOOL_DIRECTOR: return 199;
		case PM_ULTRA_STATUE: return 199;

		case PM_DEATH_SWORD: return 250;
		case PM_CLOCK_GOLEM: return 250;
		case PM_WHIRLING_HATCHET: return 250;
		case PM_HELLBLADE: return 320;
		case PM_BLADE_BARRIER: return 550;
		case PM_BLADE_OF_CHAOS: return 550;
		case PM_CELESTIAL_CLARION: return 400;
		case PM_CANNON_GOLEM: return 400;
		case PM_MISHAPEN_GOLEM: return 400;
		case PM_FROST_GOLEM: return 400;

		case PM_ANIMATED_COPPER_STATUE: return 237;
		case PM_GREAT_PROJECTION_STATUE: return 237;
		case PM_LEAD_STATUE: return 237;
		case PM_ANIMATED_SILVER_STATUE: return 273;
		case PM_FLYER_STATUE: return 273;
		case PM_ANIMAYION_STATUE: return 273;
		case PM_ANIMATED_GOLDEN_STATUE: return 324;
		case PM_STEELHAMMER: return 324;
		case PM_THUNDER_HAMMER: return 324;
		case PM_ANIMATED_PLATINUM_STATUE: return 388;
		case PM_SUPERCASH_ROAD_STATUE: return 388;
		case PM_THE_ULTIMATE_TANK: return 388;

		case PM_ATHENA_BLOCKER: return 1000;
		case PM_ATHENA_BASHER: return 300;
		case PM_ATHENA_CHESSMAN: return 400;
		case PM_ATHENA_GUARDIAN: return 500;
		case PM_ATHENA_PROTECTOR: return 500;
		case PM_ATHENA_GIANT: return 750;

		case PM_SPACE_TRANSPORT: return 199;
		case PM_WEATWIND: return 199;
		case PM_FLESH_GOLEM: return 120;
		case PM_ANIMATED_SYNTHETIC_SANDAL: return 120;
		case PM_BRAIN_GOLEM: return 120;
		case PM_PETTY_BRAIN_GOLEM: return 120;
		case PM_SPELL_GOLEM: return 130;
		case PM_GROVE_GUARDIAN: return 170;
		case PM_STATUE_GARGOYLE: return 140;
		case PM_STATUE_GOLEM: return 140;
		case PM_CLAY_GOLEM: return 150;
		case PM_SEMBLANCE: return 150;
		case PM_SANDMAN: return 150;
		case PM_TONE_GOLEM: return 150;
		case PM_BLOOD_GOLEM: return 200;
		case PM_HAUNTED_HELMET: return 200;
		case PM_KNOBBLE_STICK: return 200;
		case PM_GUTS_GOLEM: return 200;
		case PM_SMOKE_GOLEM: return 200;
		case PM_CLAY_STATUE: return 150;
		case PM_TREASURY_GOLEM: return 160;
		case PM_ANIMATED_HUGGING_BOOT: return 160;
		case PM_SNOW_GOLEM: return 160;
		case PM_STONE_GOLEM: return 180;
		case PM_STONE_STATUE: return 180;
		case PM_GLASS_GOLEM: return 140;
		case PM_SERVANT_OF_THE_UNKNOWN_GOD: return 140;
		case PM_HEAD_OF_THE_UNKNOWN_GOD: return 140;
		case PM_BODY_OF_THE_UNKNOWN_GOD: return 140;
		case PM_LEGS_OF_THE_UNKNOWN_GOD: return 140;
		case PM_EYE_OF_THE_UNKNOWN_GOD: return 100;
		case PM_SCOURGE_OF_THE_UNKNOWN_GOD: return 180;
		case PM_AQUATIC_GOLEM: return 190;
		case PM_BONE_GOLEM: return 200;
		case PM_ANIMATED_BLOCK_HEELED_COMBAT_BOOT: return 200;
		case PM_ANIMATED_IRREGULAR_HEEL: return 233;
		case PM_SAURON_THE_IMPRISONED: return 200;
		case PM_ANIMATED_LEATHER_PEEP_TOE: return 250;
		case PM_ANIMATED_ROLLER_BLADE: return 260;
		case PM_IRONMAN: return 240;
		case PM_IRON_GOLEM: return 240;
		case PM_TIN_MAN: return 240;
		case PM_THE_BORSHIN: return 240;
		case PM_IRON_MAN: return 240;
		case PM_ARGENTUM_GOLEM: return 240;
		case PM_ICE_GOLEM: return 240;
		case PM_ICE_STATUE: return 240;
		case PM_MIST_GOLEM: return 240;
		case PM_GRAVELER: return 250;
		case PM_THUY_HAN: return 250;
		case PM_RETRIEVER: return 250;
		case PM_ANIMATED_BEAUTIFUL_SANDAL: return 250;
		case PM_TELERETRIEVER: return 250;
		case PM_RUBY_GOLEM: return 250;
		case PM_FIRE_GOLEM: return 250;
		case PM_LAVA_GOLEM: return 250;
		case PM_AUTO_ROLLER: return 250;
		case PM_BURNING_BRUTE: return 250;
		case PM_PUKELMAN: return 250;
		case PM_COLBRAN: return 270;
		case PM_MITHRIL_GOLEM: return 300;
		case PM_TILL: return 300;
		case PM_ARSENAL: return 300;
		case PM_ANIMATED_ATSUZOKO_BOOT: return 320;
		case PM_EOG_GOLEM: return 340;
		case PM_SILENT_WATCHER: return 350;
		case PM_ANIMATED_STILETTO_SANDAL: return 350;
		case PM_COLOSSUS: return 360;
		case PM_DROLEM: return 440;
		case PM_JNR: return 400;
		case PM_COAL_GOLEM: return 400;
		case PM_COPPER_GOLEM: return 400;
		case PM_LEAD_GOLEM: return 400;
		case PM_BLADE_OF_SLAUGHTER: return 450;
		case PM_DIFFICULT_LEAD_GOLEM: return 400;
		case PM_ANIMATED_PROSTITUTE_SHOE: return 400;
		case PM_JUGGERNAUT_OF_KHORNE: return 450;
		case PM_BURNING_MONSTER: return 250;
		case PM_ANIMATED_SEXY_LEATHER_PUMP: return 250;
		case PM_ANIMATED_SKY_HIGH_HEEL: return 300;
		case PM_ANIMATED_UNFAIR_STILETTO: return 350;
		case PM_ANIMATED_BEAUTIFUL_FUNNEL_HEELED_PUMP: return 350;
		case PM_DIAMOND_GOLEM: return 270;
		case PM_SAPPHIRE_GOLEM: return 280;
		case PM_STEEL_GOLEM: return 290;
		case PM_CRYSTAL_GOLEM: return 300;
		case PM_CRYSTAL_STATUE: return 300;
		case PM_CRYSTAL_ICE_GOLEM: return 300;
		case PM_ANIMATED_COMBAT_STILETTO: return 300;
		case PM_ETERNIUM_GOLEM: return 350;
		case PM_THE_DISGUSTING_SMOKER_FRIEND_OF_MARIE: return 350;
		case PM_EBONY_GOLEM: return 400;
		case PM_CENTER_OF_ALL: return 400;
		case PM_TIME_GOLEM: return 400;
		case PM_LION_SHAPED_GOLEM: return 450;
		case PM_DAZZLING_GOLEM: return 450;
		case PM_FINNISH_EXPLORER: return 400;
		case PM_ARCTIC_WONDERER: return 400;
		case PM_FRANKENSTEIN_S_MONSTER: return 400;
		case PM_M_LING__MONTGOMERY_S_ATTENDANT: return 400;
		case PM_PINOCCHIO: return 400;
		case PM_GOLEM: return 400;
		case PM_BLOODY_BEAUTIES: return 500;
		case PM_KASTORTRANSPORT: return 500;
		case PM_ANIMATED_WINTER_STILETTO: return 500;
		case PM_ROBO_KY: return 750;
		case PM_IRIS_S_SILVER_PUMP: return 750;
		case PM_INVULNERABLE_GOLEM: return 2000;
		case PM_HAMMER_OF_THE_UNDERWORLD: return 1000;
		case PM_WAX_GOLEM: return 40;
		case PM_WRAP_GOLEM: return 40;
		case PM_TIN_GOLEM: return 40;
		case PM_RESIN_GOLEM: return 40;
		case PM_PLASTIC_GOLEM: return 60;
		default: {impossible("undefined golem %d?",type); return (mons[type].mlevel * 10);} /* failsafe so undefined golems don't start with zero hp! --Amy */
	}
}

#endif /* OVLB */
#ifdef OVL1

/*
 *	Alignment vs. yours determines monster's attitude to you.
 *	( some "animal" types are co-aligned, but also hungry )
 */

boolean
peace_minded(ptr)
register struct permonst *ptr;
{

	if (Race_if(PM_RODNEYAN) && monsndx(ptr) == PM_RODNEY_S_SISTER) return TRUE;

	if (Race_if(PM_ALBAE) || Race_if(PM_RODNEYAN) || issoviet || Role_if(PM_MURDERER) || Role_if(PM_FAILED_EXISTENCE) ) return FALSE; /* albae are hated by all other races --Amy */
	if (Role_if(PM_CRUEL_ABUSER) && Qstats(killed_nemesis) ) return FALSE; /* you murderer! */
	if (uarmf && uarmf->oartifact == ART_HERMES__UNFAIRNESS) return FALSE;
	if (ptr->msound == MS_NEMESIS)	return FALSE;

	if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "politician cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "politik plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "siyosatchi plash") ) ) return FALSE;

	aligntyp mal = ptr->maligntyp, ual = u.ualign.type;

	if (always_peaceful(ptr)) return TRUE;

	if (!rn2(Aggravate_monster ? 500 : 100)) return TRUE; /* low chance of getting random peaceful monsters --Amy */

	/* chaotic alignment is too easy and lawful is too hard. Make it easier for neutral and especially lawful by having monsters generate peaceful more often. --Amy */
	if ( (sgn(u.ualign.type) == sgn(ptr->maligntyp) ) && !rn2(20) && !Role_if(PM_CONVICT) && u.ualign.type == A_LAWFUL) return TRUE;
	if ( (sgn(u.ualign.type) == sgn(ptr->maligntyp) ) && !rn2(50) && !Role_if(PM_CONVICT) && u.ualign.type == A_NEUTRAL) return TRUE;

	if (is_pokemon(ptr) && rn2(5) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "poke mongo cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sovat' mongo plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "soktudun mongo plash") ) ) return TRUE;

	if (ptr->mlet == S_KOP && Race_if(PM_KOP) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_ANGEL && Race_if(PM_HUMANOID_ANGEL) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_DEMON && Race_if(PM_HUMANOID_DEVIL) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (is_demon(ptr) && Race_if(PM_HUMANOID_DEVIL) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;

	if (ptr->mlet == S_KOBOLD && Race_if(PM_KOBOLT) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_OGRE && Race_if(PM_OGRO) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_GIANT && Race_if(PM_GIGANT) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_TROLL && Race_if(PM_TROLLOR) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_ANT && Race_if(PM_INSECTOID) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE; /* Go Team Ant! --Amy */
	if (ptr->mlet == S_SNAKE && Race_if(PM_SNAKEMAN) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_SPIDER && Race_if(PM_SPIDERMAN) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_LIZARD && Race_if(PM_ARGONIAN) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_NYMPH && Race_if(PM_NYMPH) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_NYMPH && Role_if(PM_DRUID) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_BAD_FOOD && Role_if(PM_DRUID) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_LEPRECHAUN && Race_if(PM_HUMANOID_LEPRECHAUN) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_HUMANOID && Race_if(PM_ILLITHID) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_VORTEX && Race_if(PM_VORTEX) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_VORTEX && Race_if(PM_CORTEX) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_BAT && Race_if(PM_BATMAN) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_HUMAN && Race_if(PM_VEELA) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_CENTAUR && Race_if(PM_HUMANOID_CENTAUR) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_DRAGON && Race_if(PM_HUMANLIKE_DRAGON) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (ptr->mlet == S_NAGA && Race_if(PM_HUMANLIKE_NAGA) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;

	if (is_mercenary(ptr) && Role_if(PM_DOOM_MARINE) && rn2(100) ) return TRUE;

	if (ptr->mlet == S_EEL && Race_if(PM_AQUATIC_MONSTER) && !Role_if(PM_CONVICT) && rn2(50)) return TRUE;
	if (ptr->mlet == S_FLYFISH && Race_if(PM_AQUATIC_MONSTER) && !Role_if(PM_CONVICT) && rn2(50)) return TRUE;
	if (ptr->mlet == S_RUSTMONST && Race_if(PM_AQUATIC_MONSTER) && !Role_if(PM_CONVICT) && rn2(50)) return TRUE;
	if ((is_swimmer(ptr) || (ptr)->mflags1 & M1_AMPHIBIOUS) && Race_if(PM_AQUATIC_MONSTER) && !Role_if(PM_CONVICT) && rn2(2)) return TRUE;

	if (ptr->mlet == S_ANGEL && (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "angelic cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "angel'skoye plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "farishtalarning plash") ) ) && rn2(100)) return TRUE;
	if (ptr->mlet == S_DEMON && (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "demonic cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "demonicheskaya plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "jinlarning plash") ) ) && rn2(2)) return TRUE;
	if (ptr->mlet == S_DEMON && (uarmg && uarmg->oartifact == ART_IRIS_S_PRECIOUS_METAL) && rn2(10)) return TRUE;

	if (!always_hostile(ptr) && Race_if(PM_ANGBANDER) && !Role_if(PM_CONVICT) && rn2(2)) return TRUE;

	if (telepathic(ptr) && Race_if(PM_LEVITATOR) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;

	if (is_undead(ptr) && !mindless(ptr) && Race_if(PM_HUMAN_WRAITH) && !Role_if(PM_CONVICT) && rn2(100)) return TRUE;
	if (is_undead(ptr) && mindless(ptr) && Race_if(PM_HUMAN_WRAITH) && !Role_if(PM_CONVICT) && rn2(4)) return TRUE;
	if (is_animal(ptr) && rn2(10) && uarm && uarm->oartifact == ART_BEASTMASTER_S_DUSTER) return TRUE;

	if (always_hostile(ptr)) return FALSE;
	if (ptr->msound == MS_LEADER || ptr->msound == MS_GUARDIAN)
		return TRUE;

	if (ptr->mlet == S_ANGEL && Race_if(PM_HUMANOID_DEVIL)) return FALSE;
	if (ptr->mlet == S_DEMON && Race_if(PM_HUMANOID_ANGEL)) return FALSE;
	if (is_demon(ptr) && Race_if(PM_HUMANOID_ANGEL)) return FALSE;

	if (is_elf(ptr) && is_elf(youmonst.data)) {
		/* Light and dark elves are always hostile to each other.
		 * Suggested by Dr. Eva R. Myers.
		 */
		 if (ual > A_NEUTRAL && mal < A_NEUTRAL ||
		   ual < A_NEUTRAL && mal > A_NEUTRAL)
			return FALSE;
	}

	if (race_peaceful(ptr)) return TRUE;
	if (race_hostile(ptr)) return FALSE;

	/* the monster is hostile if its alignment is different from the
	 * player's */
	if (sgn(mal) != sgn(ual)) return FALSE;

	/* Negative monster hostile to player with Amulet. */
	if (mal < A_NEUTRAL && u.uhave.amulet) return FALSE;

	/* minions are hostile to players that have strayed at all */
	/* and they can also be hostile to players with good alignment --Amy */
	if (is_minion(ptr) && u.ualign.record < 0 && !issoviet) return FALSE; /*return((boolean)(u.ualign.record >= 0));*/ 

	/* Last case:  a chance of a co-aligned monster being
	 * hostile.  This chance is greater if the player has strayed
	 * (u.ualign.record negative) or the monster is not strongly aligned.
	 * Edit by Amy - much higher chance of the monster being hostile.
	 */
	/*return((boolean)(!!rn2(16 + (u.ualign.record < -15 ? -15 : u.ualign.record)) &&
		!!rn2(2 + abs(mal))));*/

	/* "Coaligned minions/mons shouldn't be hostile often. Reverted the chances of coaligned minions and creatures being hostile back to their original values. - makemon.c" In Soviet Russia, people got annoyed about the fact they still occasionally had to battle archons as a lawful character. They thought that this was making the game too hard, even though the vanilla behavior basically gives lawful characters a free pass to most non-predetermined angel encounters, which IMHO borders on the other extreme: too easy! What's the use of peaceful monsters, anyway? They just serve to get in your way and reduce the amount of monsters that you can fight. --Amy */

	if (u.ualign.record < 0 && rn2(100) && !issoviet) return FALSE;
	if (u.ualign.record == 0 && rn2(50) && !issoviet) return FALSE;
	if (u.ualign.record >= 1 && rnd(u.ualign.record) < (5 + rnd(20)) && rn2(33) && !issoviet ) return FALSE;
	if (!rn2(5 + abs(mal))) return FALSE;
	else return TRUE;
}

/* Set malign to have the proper effect on player alignment if monster is
 * killed.  Negative numbers mean it's bad to kill this monster; positive
 * numbers mean it's good.  Since there are more hostile monsters than
 * peaceful monsters, the penalty for killing a peaceful monster should be
 * greater than the bonus for killing a hostile monster to maintain balance.
 * Rules:
 *   it's bad to kill peaceful monsters, potentially worse to kill always-
 *	peaceful monsters
 *   it's never bad to kill a hostile monster, although it may not be good
 */
void
set_malign(mtmp)
struct monst *mtmp;
{
	schar mal = mtmp->data->maligntyp;
	boolean coaligned;

	if (mtmp->ispriest || mtmp->isminion) {
		/* some monsters have individual alignments; check them */
		if (mtmp->ispriest)
			mal = EPRI(mtmp)->shralign;
		else if (mtmp->isminion)
			mal = EMIN(mtmp)->min_align;
		/* unless alignment is none, set mal to -5,0,5 */
		/* (see align.h for valid aligntyp values)     */
		if(mal != A_NONE)
			mal *= 5;
		/* make priests of Moloch hostile */
		if (mal == A_NONE) mtmp->mpeaceful = 0;
	}


	coaligned = (sgn(mal) == sgn(u.ualign.type));
	if (mtmp->data->msound == MS_LEADER) {
		mtmp->malign = -20;
	} else if (mal == A_NONE) {
		if (mtmp->mpeaceful)
			mtmp->malign = 0;
		else
			mtmp->malign = 20;	/* really hostile */
	} else if (always_peaceful(mtmp->data)) {
		int absmal = abs(mal);
		if (mtmp->mpeaceful)
			mtmp->malign = -3*max(5,absmal);
		else
			mtmp->malign = 3*max(5,absmal); /* renegade */
	} else if (always_hostile(mtmp->data)) {
		int absmal = abs(mal);
		if (coaligned)
			mtmp->malign = 0;
		else
			mtmp->malign = max(5,absmal);
	} else if (coaligned) {
		int absmal = abs(mal);
		if (mtmp->mpeaceful)
			mtmp->malign = -3*max(3,absmal);
		else	/* renegade */
			mtmp->malign = max(3,absmal);
	} else	/* not coaligned and therefore hostile */
		mtmp->malign = abs(mal);
	if (mtmp->malign >= 1 && rn2(5))  mtmp->malign /= 3; /*rebuilding alignment should be harder --Amy*/

	if (mtmp->malign >= 1 && Role_if(PM_POLITICIAN)) mtmp->malign = 0; /* politicians don't like getting their hands dirty */
}

#endif /* OVL1 */
#ifdef OVLB

static NEARDATA char syms[] = {
	MAXOCLASSES, MAXOCLASSES+1, RING_CLASS, WAND_CLASS, WEAPON_CLASS,
	FOOD_CLASS, COIN_CLASS, SCROLL_CLASS, POTION_CLASS, ARMOR_CLASS,
	AMULET_CLASS, TOOL_CLASS, ROCK_CLASS, GEM_CLASS, SPBOOK_CLASS,
	S_MIMIC_DEF, S_MIMIC_DEF, S_MIMIC_DEF,
};

void
set_mimic_sym(mtmp)		/* KAA, modified by ERS */
register struct monst *mtmp;
{
	int typ, roomno, rt;
	unsigned appear, ap_type;
	int s_sym;
	struct obj *otmp;
	int mx, my;

	if (!mtmp) return;
	mx = mtmp->mx; my = mtmp->my;
	typ = levl[mx][my].typ;
					/* only valid for INSIDE of room */
	roomno = levl[mx][my].roomno - ROOMOFFSET;
	if (roomno >= 0)
		rt = rooms[roomno].rtype;
#ifdef SPECIALIZATION
	else if (IS_ROOM(typ))
		rt = OROOM,  roomno = 0;
#endif
	else	rt = 0;	/* roomno < 0 case for GCC_WARN */

	if (OBJ_AT(mx, my)) {
		ap_type = M_AP_OBJECT;
		appear = level.objects[mx][my]->otyp;
	} else if (IS_DOOR(typ) || IS_WALL(typ) ||
		   typ == SDOOR || typ == SCORR) {
		ap_type = M_AP_FURNITURE;
		/*
		 *  If there is a wall to the left that connects to this
		 *  location, then the mimic mimics a horizontal closed door.
		 *  This does not allow doors to be in corners of rooms.
		 */
		if (mx != 0 &&
			(levl[mx-1][my].typ == HWALL    ||
			 levl[mx-1][my].typ == TLCORNER ||
			 levl[mx-1][my].typ == TRWALL   ||
			 levl[mx-1][my].typ == BLCORNER ||
			 levl[mx-1][my].typ == TDWALL   ||
			 levl[mx-1][my].typ == CROSSWALL||
			 levl[mx-1][my].typ == TUWALL    ))
		    appear = S_hcdoor;
		else
		    appear = S_vcdoor;

		if(!mtmp->minvis || See_invisible)
		    block_point(mx,my);	/* vision */
	} else if (level.flags.is_maze_lev && rn2(2)) {
		ap_type = M_AP_OBJECT;
		appear = BOULDER;
	} else if (roomno < 0) {
		ap_type = M_AP_OBJECT;

		if (!rn2(5)) {

			ap_type = M_AP_FURNITURE;

			switch (rnd(19)) {

				case 1:		appear = S_stone; break;
				case 2:		appear = S_bars; break;
				case 3:		appear = S_tree; break;
				case 4:		appear = S_room; break;
				case 5:		appear = S_darkroom; break;
				case 6:		appear = S_corr; break;
				case 7:		appear = S_litcorr; break;
				case 8:		appear = S_altar; break;
				case 9:		appear = S_grave; break;
				case 10:		appear = S_throne; break;
				case 11:		appear = S_sink; break;
				case 12:		appear = S_fountain; break;
				case 13:		appear = S_toilet; break;
				case 14:		appear = S_pool; break;
				case 15:		appear = S_ice; break;
				case 16:		appear = S_lava; break;
				case 17:		appear = S_air; break;
				case 18:		appear = S_cloud; break;
				case 19:		appear = S_water; break;
				default:		appear = S_stone; break;

			}

		}

		else if (rn2(10)) {

		s_sym = syms[rn2((int)sizeof(syms))];
		if (s_sym >= MAXOCLASSES) {
			ap_type = M_AP_FURNITURE;
			appear = s_sym == MAXOCLASSES ? S_upstair : S_dnstair;
		} else if (s_sym == COIN_CLASS) {
			ap_type = M_AP_OBJECT;
			appear = GOLD_PIECE;
		} else {
			ap_type = M_AP_OBJECT;
			if (s_sym == S_MIMIC_DEF) {
				appear = STRANGE_OBJECT;
			} else {
				otmp = mkobj( (char) s_sym, FALSE );
				if (otmp) {
					appear = otmp->otyp;
					/* make sure container contents are free'ed */
					if (Has_contents(otmp))
						delete_contents(otmp);
					obfree(otmp, (struct obj *) 0);
				} else appear = STRANGE_OBJECT;
			}
		}

		}
		else
		appear = BOULDER;
		if(!mtmp->minvis || See_invisible)
		    block_point(mx,my);	/* vision */
	} else if (rt == ZOO || rt == VAULT) {
		ap_type = M_AP_OBJECT;
		appear = GOLD_PIECE;
	} else if (rt == DELPHI) {
		if (rn2(2)) {
			ap_type = M_AP_OBJECT;
			appear = BOULDER;
		} else {
			ap_type = M_AP_FURNITURE;
			appear = S_fountain;
		}
	} else if (rt == TEMPLE) {
		ap_type = M_AP_FURNITURE;
		appear = S_altar;
	/*
	 * We won't bother with beehives, morgues, barracks, throne rooms
	 * since they shouldn't contain too many mimics anyway...
	 */
	} else if (rt >= SHOPBASE) {
		s_sym = get_shop_item(rt - SHOPBASE);
		if (s_sym < 0) {
			ap_type = M_AP_OBJECT;
			appear = -s_sym;
		} else {
			if (s_sym == RANDOM_CLASS)
				s_sym = syms[rn2((int)sizeof(syms)-2) + 2];
			goto assign_sym;
		}
	} else {
		s_sym = syms[rn2((int)sizeof(syms))];
assign_sym:

		if (!rn2(5)) {

			ap_type = M_AP_FURNITURE;

			switch (rnd(19)) {

				case 1:		appear = S_stone; break;
				case 2:		appear = S_bars; break;
				case 3:		appear = S_tree; break;
				case 4:		appear = S_room; break;
				case 5:		appear = S_darkroom; break;
				case 6:		appear = S_corr; break;
				case 7:		appear = S_litcorr; break;
				case 8:		appear = S_altar; break;
				case 9:		appear = S_grave; break;
				case 10:		appear = S_throne; break;
				case 11:		appear = S_sink; break;
				case 12:		appear = S_fountain; break;
				case 13:		appear = S_toilet; break;
				case 14:		appear = S_pool; break;
				case 15:		appear = S_ice; break;
				case 16:		appear = S_lava; break;
				case 17:		appear = S_air; break;
				case 18:		appear = S_cloud; break;
				case 19:		appear = S_water; break;
				default:		appear = S_stone; break;

			}

		}

		else if (s_sym >= MAXOCLASSES) {
			ap_type = M_AP_FURNITURE;
			appear = s_sym == MAXOCLASSES ? S_upstair : S_dnstair;
		} else if (s_sym == COIN_CLASS) {
			ap_type = M_AP_OBJECT;
			appear = GOLD_PIECE;
		} else {
			ap_type = M_AP_OBJECT;
			if (s_sym == S_MIMIC_DEF) {
				appear = STRANGE_OBJECT;
			} else {
				otmp = mkobj( (char) s_sym, FALSE );
				if (otmp) {
					appear = otmp->otyp;
					/* make sure container contents are free'ed */
					if (Has_contents(otmp))
						delete_contents(otmp);
					obfree(otmp, (struct obj *) 0);
				} else appear = STRANGE_OBJECT;
			}
		}
	}

	if (mtmp->data == &mons[PM_CLOAKER] || mtmp->data == &mons[PM_PERMACLOAKER] || mtmp->data == &mons[PM_CLOAK_MIMIC] || mtmp->data == &mons[PM_CLOAK_PERMAMIMIC]) {
		s_sym = ARMOR_CLASS;
		ap_type = M_AP_OBJECT;
		appear = rnd_class(MUMMY_WRAPPING, CLOAK_OF_DISPLACEMENT);
	}

	if (mtmp->data == &mons[PM_POTION_MIMIC] || mtmp->data == &mons[PM_POTION_PERMAMIMIC]) {
		s_sym = POTION_CLASS;
		ap_type = M_AP_OBJECT;
		appear = rnd_class(POT_BOOZE, POT_PAN_GALACTIC_GARGLE_BLASTE);
	}

	if (mtmp->data == &mons[PM_RUNESTONE_MIMIC] || mtmp->data == &mons[PM_RUNESTONE_PERMAMIMIC]) {
		s_sym = GEM_CLASS;
		ap_type = M_AP_OBJECT;
		appear = rnd_class(DILITHIUM_CRYSTAL, JADE);
	}

	if (mtmp->data == &mons[PM_SCROLL_MIMIC] || mtmp->data == &mons[PM_SCROLL_PERMAMIMIC] || mtmp->data == &mons[PM_MAP_MIMIC] || mtmp->data == &mons[PM_MAP_PERMAMIMIC]) {
		s_sym = SCROLL_CLASS;
		ap_type = M_AP_OBJECT;
		appear = rnd_class(SCR_CREATE_MONSTER, SCR_GIRLINESS);
	}

	if (mtmp->data == &mons[PM_STAFF_MIMIC] || mtmp->data == &mons[PM_STAFF_PERMAMIMIC]) {
		s_sym = SCROLL_CLASS;
		ap_type = M_AP_OBJECT;
		appear = QUARTERSTAFF;
	}

	if (mtmp->data == &mons[PM_MAGIC_BOOK_MIMIC] || mtmp->data == &mons[PM_MAGIC_BOOK_PERMAMIMIC] || mtmp->data == &mons[PM_PRAYER_BOOK_MIMIC] || mtmp->data == &mons[PM_PRAYER_BOOK_PERMAMIMIC] || mtmp->data == &mons[PM_SONG_BOOK_MIMIC] || mtmp->data == &mons[PM_SONG_BOOK_PERMAMIMIC]) {
		s_sym = SPBOOK_CLASS;
		ap_type = M_AP_OBJECT;
		appear = rnd_class(SPE_FORCE_BOLT, SPE_PSYBEAM);
	}

	if (mtmp->data == &mons[PM_ROD_MIMIC] || mtmp->data == &mons[PM_ROD_PERMAMIMIC]) {
		s_sym = SCROLL_CLASS;
		ap_type = M_AP_OBJECT;
		appear = STAR_ROD;
	}

	if (mtmp->data == &mons[PM_WAND_MIMIC] || mtmp->data == &mons[PM_WAND_PERMAMIMIC]) {
		s_sym = SCROLL_CLASS;
		ap_type = M_AP_OBJECT;
		appear = rnd_class(WAN_LIGHT, WAN_PSYBEAM);
	}

	if (mtmp->data == &mons[PM_RING_MIMIC] || mtmp->data == &mons[PM_RING_PERMAMIMIC]) {
		s_sym = RING_CLASS;
		ap_type = M_AP_OBJECT;
		appear = rnd_class(RIN_ADORNMENT, RIN_TELEPORT_CONTROL);
	}

	if (mtmp->data == &mons[PM_CHEST_MIMIC] || mtmp->data == &mons[PM_CHEST_PERMAMIMIC]) {
		s_sym = TOOL_CLASS;
		ap_type = M_AP_OBJECT;
		appear = CHEST;
	}

	if (mtmp->data == &mons[PM_DOOR_MIMIC] || mtmp->data == &mons[PM_DOOR_PERMAMIMIC] || mtmp->data == &mons[PM_SMALL_DOOR_MIMIC] || mtmp->data == &mons[PM_SMALL_DOOR_PERMAMIMIC]) {
		s_sym = MAXOCLASSES;
		ap_type = M_AP_FURNITURE;
		appear = S_hcdoor;
	}

	if (mtmp->data == &mons[PM_DEMONIC_DOOR] || mtmp->data == &mons[PM_SATANIC_DOOR]) {
		s_sym = MAXOCLASSES;
		ap_type = M_AP_FURNITURE;
		appear = S_hcdoor;
	}



	mtmp->m_ap_type = ap_type;
	mtmp->mappearance = appear;
}

/* release a monster from a bag of tricks */
void
bagotricks(bag)
struct obj *bag;
{
    if (!bag || bag->otyp != BAG_OF_TRICKS) {
	impossible("bad bag o' tricks");
    } else if (bag->spe < 1) {
	pline(nothing_happens);
    } else {
	boolean gotone = FALSE;
	int cnt = 1;

	consume_obj_charge(bag, TRUE);

	if (!rn2(23)) cnt += rn1(7, 1);
	if (bag && bag->oartifact == ART_VERY_TRICKY_INDEED) cnt *= 2;

	while (cnt-- > 0) {
	    if (makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS))
		gotone = TRUE;
	}
	if (gotone) makeknown(BAG_OF_TRICKS);
    }
}

#endif /* OVLB */

/*makemon.c*/
