/*	SCCS Id: @(#)dothrow.c	3.4	2003/12/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* Contains code for 't' (throw) */

#include "hack.h"
#include "edog.h"

STATIC_DCL int throw_obj(struct obj *, int, int);
STATIC_DCL void autoquiver(void);
STATIC_DCL int gem_accept(struct monst *, struct obj *);
STATIC_DCL void tmiss(struct obj *, struct monst *);
STATIC_DCL int throw_gold(struct obj *);
STATIC_DCL void check_shop_obj(struct obj *,XCHAR_P,XCHAR_P,BOOLEAN_P);
STATIC_DCL void breakobj(struct obj *,XCHAR_P,XCHAR_P,BOOLEAN_P,BOOLEAN_P);
STATIC_DCL void breakmsg(struct obj *,BOOLEAN_P);
STATIC_DCL boolean toss_up(struct obj *, BOOLEAN_P);
STATIC_DCL boolean throwing_weapon(struct obj *);
STATIC_DCL void sho_obj_return_to_u(struct obj *obj);
STATIC_DCL boolean mhurtle_step(void *,int,int);
static void autoquiver(void);	/* KMH -- automatically fill quiver */


static NEARDATA const char toss_objs[] =
	{ ALLOW_COUNT, COIN_CLASS, ALL_CLASSES, WEAPON_CLASS, 0 };
/* different default choices when wielding a sling (gold must be included) */
static NEARDATA const char bullets[] =
	{ ALLOW_COUNT, COIN_CLASS, ALL_CLASSES, GEM_CLASS, 0 };

struct obj *thrownobj = 0;	/* tracks an object until it lands */

extern boolean notonhead;	/* for long worms */

#define THROW_UWEP 	1
#define THROW_USWAPWEP 	2

/* Split this object off from its slot */

struct obj *
splitoneoff(pobj)
struct obj **pobj;
{
    struct obj *obj = *pobj;
    struct obj *otmp = (struct obj *)0;
    if (obj == uquiver) {
	if (obj->quan > 1L)
	    setuqwep(otmp = splitobj(obj, 1L));
	else
	    setuqwep((struct obj *)0);
    } else if (obj == uswapwep) {
	if (obj->quan > 1L)
	    setuswapwep(otmp = splitobj(obj, 1L), FALSE);
	else
	    setuswapwep((struct obj *)0, FALSE);
    } else if (obj == uwep) {
	if (obj->quan > 1L)
	    setworn(otmp = splitobj(obj, 1L), W_WEP);
	    /* not setuwep; do not change unweapon */
	else {
	    setuwep((struct obj *)0, FALSE, TRUE);
	    if (uwep) return (struct obj *)0; /* unwielded, died, rewielded */
	}
    } else if (obj->quan > 1L)
	otmp = splitobj(obj, 1L);
    *pobj = otmp;
    return obj;
}

/* Throw the selected object, asking for direction */
STATIC_OVL int
throw_obj(obj, shotlimit, thrown)
register struct obj *obj;
int shotlimit;
int thrown;
{
	struct obj *otmp;
	struct obj *launcher;
	int multishot = (Race_if(PM_CLOCKWORK_AUTOMATON) && !Upolyd) ? rnd(3) : Race_if(PM_MANSTER) ? rnd(2) : Race_if(PM_MONGUNG) ? rnd(2) : Race_if(PM_HAXOR) ? rno(2) : 1;
	boolean fullmultishot = FALSE; /* depends on missile weapons skill --Amy */
	boolean reallyfullmultishot = FALSE;
	int angeramount; /* for blade anger technique */

	if (RngeMultishot) multishot++;
	if (Race_if(PM_SPARD)) multishot += rnd(4);
	if (uarmf && uarmf->oartifact == ART_ZERO_SUIT) multishot += 1;
	if (uarmh && uarmh->oartifact == ART_VIRUS_ATTACK) multishot += 1;
	if (uarmh && uarmh->oartifact == ART_SURFACE_TO_AIR_SITE) multishot += 1;
	if (uwep && uwep->oartifact == ART_LASER_PALADIN) multishot += 1;
	if (uarmg && uarmg->oartifact == ART_WHINY_MARY) multishot += rnd(5);
	if (uwep && uwep->oartifact == ART_GUNS_IN_MY_HEAD) multishot += 1;

	if (Double_attack || (uwep && uwep->oartifact == ART_MELISSA_S_PEACEBRINGER && !u.twoweap) || (uwep && uwep->oartifact == ART_HELICOPTER_CHAIN && !u.twoweap) || (uwep && uwep->oartifact == ART_CRUSHING_IMPACT && !u.twoweap) ) multishot += rn2(multishot + 1);
	if (Quad_attack) multishot += rn2(multishot * 3 + 1);

	if ((long)multishot > obj->quan && (long)multishot > 1) multishot = (int)obj->quan;

	    if ((shotlimit > 0) && (multishot > shotlimit)) multishot = shotlimit;

	schar skill;
	long wep_mask;
	boolean twoweap;

	multi = 0;		/* reset; it's been used up */
	
	if (thrown == 1 && uwep && ammo_and_launcher(obj, uwep)) 
		launcher = uwep;
	else if (thrown == 2 && uswapwep && ammo_and_launcher(obj, uswapwep))
		launcher = uswapwep;
	else if (thrown == 666) { /* inbuilt pistol boots */
		if (uarmf && itemhasappearance(uarmf, APP_PISTOL_BOOTS) ) launcher = uarmf;
	}
	else launcher = (struct obj *)0;

	/* ask "in what direction?" */
#ifndef GOLDOBJ
	if (!getdir((char *)0)) {
		if (obj->oclass == COIN_CLASS) {
		    u.ugold += obj->quan;
		    flags.botl = 1;
		    dealloc_obj(obj);
		}
		return(0);
	}

	if(obj->oclass == COIN_CLASS) return(throw_gold(obj));
#else
	if (!getdir((char *)0)) {
	    /* obj might need to be merged back into the singular gold object */
	    freeinv(obj);
	    addinv(obj);
	    return(0);
	}

        /*
	  Throwing money is usually for getting rid of it when
          a leprechaun approaches, or for bribing an oncoming 
          angry monster.  So throw the whole object.

          If the money is in quiver, throw one coin at a time,
          possibly using a sling.
        */
	if(obj->oclass == COIN_CLASS && obj != uquiver) return(throw_gold(obj));
#endif

	if(!canletgo(obj,"throw"))
		return(0);

	if(obj == uwep && welded(obj)) {
		weldmsg(obj);
		return(1);
	}

	if (obj->oartifact == ART_MJOLLNIR && obj != uwep) {
	    pline("%s must be wielded before it can be thrown.",
		The(xname(obj)));
		return(0);
	}
	if (obj->oartifact == ART_OTHER_MJOLLNIR && obj != uwep) {
	    pline("%s must be wielded before it can be thrown.",
		The(xname(obj)));
		return(0);
	}
	/* Since it's almost impossible to get 25 strength in slex, valkyries can simply throw Mjollnir at all times --Amy */
	if ((obj->oartifact == ART_MJOLLNIR && !Role_if(PM_VALKYRIE) && !Role_if(PM_VANILLA_VALK) && ACURR(A_STR) < STR19(25))
	   || (obj->otyp == BOULDER && !throws_rocks(youmonst.data) && !(uarmg && uarmg->oartifact == ART_MOUNTAIN_FISTS) )) {
		pline("It's too heavy.");
		return(1);
	}
	if ((obj->oartifact == ART_OTHER_MJOLLNIR && !Role_if(PM_VALKYRIE) && !Role_if(PM_VANILLA_VALK) && ACURR(A_STR) < STR19(25))
	   || (obj->otyp == BOULDER && !throws_rocks(youmonst.data) && !(uarmg && uarmg->oartifact == ART_MOUNTAIN_FISTS) )) {
		pline("It's too heavy.");
		return(1);
	}
	if(!u.dx && !u.dy && !u.dz) {
		You("cannot throw an object at yourself.");
		return(0);
	}
	u_wipe_engr(2);
	if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && (obj->otyp == CORPSE &&
		    touch_petrifies(&mons[obj->corpsenm]))) {
		You("throw the %s corpse with your bare %s.",
		    mons[obj->corpsenm].mname, body_part(HAND));
		sprintf(killer_buf, "throwing a petrifying corpse");
		instapetrify(killer_buf);
	}
	if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && (obj->otyp == PETRIFYIUM_BAR)) {
		You("throw the bar with your bare %s.", body_part(HAND));
		sprintf(killer_buf, "a thrown petrifyium bar");
		instapetrify(killer_buf);
	}
	if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && (obj->otyp == PETRIFYIUM_BRA)) {
		You("throw the bra with your bare %s.", body_part(HAND));
		sprintf(killer_buf, "a thrown petrifyium bra");
		instapetrify(killer_buf);
	}
	if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && (obj->otyp == EGG &&
		    touch_petrifies(&mons[obj->corpsenm]) && obj->corpsenm != PM_PLAYERMON)) {
		You("throw the %s egg with your bare %s.",
		    mons[obj->corpsenm].mname, body_part(HAND));
		sprintf(killer_buf, "throwing a petrifying egg");
		instapetrify(killer_buf);
	}
	if (welded(obj)) {
		weldmsg(obj);
		return 1;
	}

	/* Multishot calculations
	 */
	skill = objects[obj->otyp].oc_skill;
	if (( (ammo_and_launcher(obj, uwep) && !(uwep && uwep->otyp == LASERXBOW && !uwep->lamplit) && !(uwep && uwep->otyp == KLIUSLING && !uwep->lamplit) ) || skill == P_DAGGER || skill == P_KNIFE || skill == P_BOOMERANG || skill == -P_BOOMERANG || skill == P_VENOM || skill == -P_VENOM ||
			skill == -P_DART || skill == -P_SHURIKEN || skill == P_SPEAR || skill == P_JAVELIN) &&
		!( (Confusion && !Conf_resist) || (Stunned && !Stun_resist) )) {
	    /* Bonus if the player is proficient in this weapon... */

		if (!(PlayerCannotUseSkills)) {

	    switch (P_SKILL(weapon_type(obj))) {
	    default:	break; /* No bonus */
	    case P_SKILLED:	multishot++; break;
	    case P_EXPERT:	multishot += 2; break;
	    case P_MASTER:	multishot += 3; break; /* this might be implemented --Amy */
	    case P_GRAND_MASTER:	multishot += 4; break;
	    case P_SUPREME_MASTER:	multishot += 5; break;
	    }

		}
	    
	    /* ...or is using a good weapon... */
	    /* Elven Craftsmanship makes for light, quick bows */
	    if (obj->otyp == ELVEN_ARROW && !obj->cursed && !rn2(3)) multishot++;
	    if (launcher && launcher->otyp == ELVEN_BOW &&
	      !launcher->cursed && !rn2(3))
		multishot++;

	    if (launcher && launcher->oartifact == ART_MULTISHOTTEMSO && launcher->lamplit && !(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_DJEM_SO)) {
			default:	break; /* No bonus */
			case P_BASIC:	multishot++; break;
			case P_SKILLED:	multishot += 2; break;
			case P_EXPERT:	multishot += 3; break;
			case P_MASTER:	multishot += 4; break;
			case P_GRAND_MASTER:	multishot += 5; break;
			case P_SUPREME_MASTER:	multishot += 6; break;
		}
	    }

	    if (launcher && launcher->oartifact == ART_CANNONDANCER && !(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_FIREARM)) {
			default:	break; /* No bonus */
			case P_BASIC:	multishot += 1; break;
			case P_SKILLED:	multishot += rnd(2); break;
			case P_EXPERT:	multishot += rnd(3); break;
			case P_MASTER:	multishot += rnd(4); break;
			case P_GRAND_MASTER:	multishot += rnd(5); break;
			case P_SUPREME_MASTER:	multishot += rnd(6); break;
		}
	    }

	    if (launcher && launcher->oartifact == ART_EXPERIMENTAL_MIRV) {
		multishot += 7;
		fullmultishot = TRUE;
		reallyfullmultishot = TRUE;
	    }

	    if (launcher && launcher->otyp == WILDHILD_BOW && obj->otyp == ODOR_SHOT) multishot++;
	    if (launcher && launcher->otyp == COMPOST_BOW && obj->otyp == FORBIDDEN_ARROW) multishot++;

	    if (launcher && launcher->oartifact == ART_TEAM_FORTRESS_GL && obj->otyp == FRAG_GRENADE) multishot += 5;
	    if (launcher && launcher->oartifact == ART_TEAM_FORTRESS_GL && obj->otyp == GAS_GRENADE) multishot += 5;

	    if (launcher && launcher->oartifact == ART_MEASURE_SKILLING) multishot++;
	    if (launcher && launcher->oartifact == ART_SKILLED_THROUGHLOAD) multishot++;
	    if (launcher && launcher->oartifact == ART_LINCOLN_S_REPEATER) multishot++;
	    if (launcher && launcher->oartifact == ART_AR_ARMALYTE) multishot++;
	    if (launcher && launcher->oartifact == ART_LOUD_SHITTER) multishot += rn1(2, 2);
	    if (launcher && launcher->oartifact == ART_CLACKINDRA) multishot += 3;
	    if (launcher && launcher->oartifact == ART_UZ_I) multishot += rnd(3);
	    if (launcher && launcher->oartifact == ART_HEAVY_CROSSBOW_OF_ETERNITY) multishot += 2;
	    if (launcher && obj && obj->oartifact == ART_FUCK_THE_SERVER) multishot += 2;
	    if (launcher && obj && obj->oartifact == ART_WARRIOR_WOMAN) multishot += rnd(2);
	    if (launcher && obj && obj->oartifact == ART_BLOCKEL_EM) multishot += rnd(3);
	    if (launcher && obj && obj->oartifact == ART_EXFLING) multishot += 2;

	    if (uarmh && uarmh->oartifact == ART_NIAMH_PENH && launcher && objects[launcher->otyp].oc_skill == P_FIREARM) multishot++;

	    if (uarmf && uarmf->oartifact == ART_FRENCHYPOSS && uarmf->blessed && launcher && objects[launcher->otyp].oc_skill == P_FIREARM) multishot++;

	    if (uarms && uarms->oartifact == ART_MISSING_LETTER_D && launcher && objects[launcher->otyp].oc_skill == P_SLING) multishot += 2;
	    if (uleft && uleft->oartifact == ART_ACTUAL_CAVE_DWELLING && launcher && objects[launcher->otyp].oc_skill == P_SLING) multishot++;
	    if (uright && uright->oartifact == ART_ACTUAL_CAVE_DWELLING && launcher && objects[launcher->otyp].oc_skill == P_SLING) multishot++;

	    if (uarms && uarms->oartifact == ART_OFFBOW && launcher && objects[launcher->otyp].oc_skill == P_BOW) multishot += 1;

	    if (launcher && launcher->oartifact == ART_NOCK_GUN && (launcher->invoketimer <= monstermoves) ) {
		int artitimeout = rnz(2000);
		if (!rn2(5)) artitimeout = rnz(20000); /* squeaking does not help here, as it's not an actual invoke --Amy */
		multishot += 6;
		launcher->invoketimer = (monstermoves + artitimeout);
	    }

	    if (obj && obj->oartifact == ART_WIWIU_) multishot += rnd(3);
	    if (obj && obj->oartifact == ART_GOLDEN_SPIRIT_GEL) multishot += 3;
	    if (obj && obj->oartifact == ART_LEAD_SYRINGE) multishot += 2;
	    if (obj && obj->otyp == RAPID_DART) multishot += 2;
	    if (obj && obj->otyp == NINJA_STAR) multishot += 3;
	    if (obj && obj->otyp == FLAMETHROWER) multishot += 4;
	    if (obj && obj->oartifact == ART_POEPOEPOEPOEOEU_) multishot += 4;
	    if (obj && obj->oartifact == ART_MRLS) multishot += 1;
	    if (obj && obj->oartifact == ART_FRIGHT_PRAWN) multishot += 1;
	    if (obj && obj->oartifact == ART_WASHINGTON_S_CAPPER) multishot += 1;
	    if (uarmh && uarmh->oartifact == ART_GIVE_THE_BONUS_STUFF) multishot += 1;

	    if (obj && obj->oartifact == ART_CAPAUER && !PlayerCannotUseSkills) {
		if (P_MAX_SKILL(P_SHURIKEN) == P_SUPREME_MASTER) {
			switch (P_SKILL(P_SHURIKEN)) {
				case P_GRAND_MASTER: multishot += 1; break;
				case P_MASTER: multishot += 2; break;
				case P_EXPERT: multishot += 3; break;
			}
		} else if (P_MAX_SKILL(P_SHURIKEN) == P_GRAND_MASTER) {
			switch (P_SKILL(P_SHURIKEN)) {
				case P_MASTER: multishot += 1; break;
				case P_EXPERT: multishot += 2; break;
			}
		} else if (P_MAX_SKILL(P_SHURIKEN) == P_MASTER) {
			switch (P_SKILL(P_SHURIKEN)) {
				case P_EXPERT: multishot += 1; break;
			}
		}

	    }

	    if (uarm && uarm->oartifact == ART_WOOD_HUNTERING && !(launcher && objects[launcher->otyp].oc_skill == P_FIREARM)) multishot += rnd(2);

	    if (Race_if(PM_AZTPOK) && launcher && objects[launcher->otyp].oc_skill == P_FIREARM) multishot += rnd(2);
	    if (Race_if(PM_TURMENE) && launcher && objects[launcher->otyp].oc_skill == P_FIREARM) multishot += rnd(3);
	    if (uamul && uamul->oartifact == ART_BOMBER_DELIVERY && launcher && objects[launcher->otyp].oc_skill == P_FIREARM) multishot++;

	    if (uamul && uamul->oartifact == ART_PURPLE_ARBALEST && launcher && objects[launcher->otyp].oc_skill == P_CROSSBOW) multishot++;
	    if (uamul && uamul->oartifact == ART_PURPLE_ARBALEST) multishot++;

	    if (uimplant && uimplant->oartifact == ART_BIUUU_ && launcher && objects[launcher->otyp].oc_skill == P_CROSSBOW) multishot++;
	    if (powerfulimplants() && uimplant && uimplant->oartifact == ART_BIUUU_ && launcher && objects[launcher->otyp].oc_skill == P_BOW) multishot++;
	    if (uarmh && uarmh->oartifact == ART_HELM_OF_THE_ARCANE_ARCHER && launcher && objects[launcher->otyp].oc_skill == P_BOW) multishot++;

	    if (launcher && objects[launcher->otyp].oc_skill == P_FIREARM && !(PlayerCannotUseSkills) && P_SKILL(P_SQUEAKING) >= P_MASTER && P_SKILL(P_GUN_CONTROL) >= P_MASTER && Upolyd) {
			multishot++;
			if (P_SKILL(P_SQUEAKING) >= P_GRAND_MASTER && P_SKILL(P_GUN_CONTROL) >= P_GRAND_MASTER) multishot++;
			if (P_SKILL(P_SQUEAKING) >= P_SUPREME_MASTER && P_SKILL(P_GUN_CONTROL) >= P_SUPREME_MASTER) multishot++;
	    }

	    if (launcher && launcher->otyp == CATAPULT && (!obj || obj->otyp != ROCK)) multishot += rnd(5);
	    if (launcher && launcher->otyp == CATAPULT && obj && obj->otyp == ROCK) multishot += rno(5);
	    if (launcher && launcher->oartifact == ART_DRIVE_BY && u.usteed) {
		multishot += 2;
		if (u.ugallop) multishot++;
	    }
	    if (launcher && launcher->oartifact == ART_BROWNING) multishot += rnd(5);

	    if ((uwep && uwep->oartifact == ART_SPEERTHROW) && (skill == P_SPEAR) ) {
			multishot += 2;
			if (Race_if(PM_AZTPOK) || Race_if(PM_MAYMES)) multishot++;
	    }

	    if (uwep && uwep->oartifact == ART_POWCHARGE) multishot++;

	    if (launcher && (launcher->otyp == RIFLE || launcher->otyp == SNIPER_RIFLE || launcher->otyp == HUNTING_RIFLE || launcher->otyp == PROCESS_CARD) && uarmc && itemhasappearance(uarmc, APP_RIFLING_POWER_CLOAK) ) multishot += rnd(2);

	    if (launcher && launcher->otyp == HYDRA_BOW) multishot += 2;
	    if (launcher && launcher->otyp == DEMON_CROSSBOW && launcher->altmode == WP_MODE_AUTO) multishot += 4;
	    if (launcher && launcher->otyp == WILDHILD_BOW) multishot += 2;

	    if (launcher && launcher->oartifact == ART_STREAMSHOOTER) multishot += 1;

	    if (launcher && launcher->oartifact == ART_SNIPEGIANT) multishot += 1;
	    if (launcher && launcher->oartifact == ART_DOLE__EM_ALL_OUT) multishot += 2;

	    if (launcher && launcher->oartifact == ART_MAXIMUM_LAUNCH_POWER) multishot += rnd(2);

	    if (launcher && launcher->oartifact == ART_WINSETT_S_BIG_DADDY) multishot += rnd(2);

	    if (launcher && launcher->oartifact == ART_TUNA_CANNON) multishot += 1;

	    if (launcher && launcher->oartifact == ART_FOEOEOEOEOEOEOE) multishot += rnd(3);

	    if (!PlayerCannotUseSkills && skill == P_SLING && ((uarm && uarm->oartifact == ART_LU_NONNAME) || Role_if(PM_HEDDERJEDI)) ) {
		if (u.kliuskill >= 20) multishot++;
		if (u.kliuskill >= 160) multishot++;
		if (u.kliuskill >= 540) multishot++;
		if (u.kliuskill >= 1280) multishot++;
		if (u.kliuskill >= 2500) multishot++;
		if (u.kliuskill >= 4320) multishot++;
	    }

	    if (Role_if(PM_TOSSER) && obj && objects[obj->otyp].oc_skill == P_JAVELIN) multishot += 1;
	    if (Role_if(PM_MILL_SWALLOWER) && obj && (objects[obj->otyp].oc_skill == P_CROSSBOW || objects[obj->otyp].oc_skill == -P_CROSSBOW)) multishot += 1;

	    if (uarmg && uarmg->oartifact == ART_PEEPING_GROOVE && launcher && (launcher->otyp == SHOTGUN || launcher->otyp == PAPER_SHOTGUN || launcher->otyp == SAWED_OFF_SHOTGUN || launcher->otyp == AUTO_SHOTGUN)) multishot += rnd(7);

	    if (uarmc && uarmc->oartifact == ART_PALEOLITHIC_ELBOW_CONTRACT && launcher && objects[launcher->otyp].oc_skill == P_BOW) multishot += 5;

	    if (uamul && uamul->oartifact == ART_WILDWEST_LAW && launcher && objects[launcher->otyp].oc_skill == P_FIREARM) multishot += 1;

	    /* 1/3 of object enchantment */
	    if (launcher && launcher->spe > 1)
		multishot += (long) rounddiv(launcher->spe,3);
	    
	    /* ...or is using a special weapon for their role... */
	    switch (Role_switch) {
	    case PM_RANGER:
		multishot++;
		break;
	    case PM_ROGUE:
		if (skill == P_DAGGER) multishot++;
		break;
	    case PM_RINGSEEKER:
		if (skill == P_SLING) multishot++;
		break;
	    case PM_ROCKER:
		if (skill == P_SLING) {multishot++;

		if (!(PlayerCannotUseSkills)) {

		if (P_SKILL(weapon_type(obj)) >= P_SKILLED) multishot++;
		if (P_SKILL(weapon_type(obj)) >= P_EXPERT) multishot++;
		if (P_SKILL(weapon_type(obj)) >= P_MASTER) multishot++;
		if (P_SKILL(weapon_type(obj)) >= P_GRAND_MASTER) multishot++;
		if (P_SKILL(weapon_type(obj)) >= P_SUPREME_MASTER) multishot++;

		}
		}
		break;
	    case PM_ELPH: /* elf role --Amy */
		multishot++;
		if (obj->otyp == ELVEN_ARROW && launcher &&
				launcher->otyp == ELVEN_BOW) multishot++;
		break;
	    case PM_TWELPH: /* dark elf role --Amy */
		multishot++;
		if (obj->otyp == DARK_ELVEN_ARROW && launcher &&
				launcher->otyp == DARK_ELVEN_BOW) multishot++;
		break;
	    case PM_SAMURAI:
		if (obj->otyp == YA && launcher && launcher->otyp == YUMI) multishot++;
		if (obj->otyp == FAR_EAST_ARROW && launcher && launcher->otyp == YUMI) multishot++;
		break;
	    default:
		break;	/* No bonus */
	    }
	    /* ...or using their race's special bow */
	    switch (Race_switch) {
	    case PM_ELF:
	    case PM_PLAYER_MYRKALFR:
		if (obj->otyp == ELVEN_ARROW && launcher &&
				launcher->otyp == ELVEN_BOW) multishot++;
		break;
	    case PM_PLAYABLE_NEANDERTHAL:
		if (skill == P_SLING) multishot++;
		break;
	    case PM_INKA:
		if (launcher && launcher->otyp == INKA_SLING && ammo_and_launcher(obj, launcher) ) multishot += 2;
		break;
	    case PM_ORC:
		if (obj->otyp == ORCISH_ARROW && uwep &&
				uwep->otyp == ORCISH_BOW) multishot++;
		break;
	    case PM_HOBBIT:	/* slings are retarded weapons according to Jacob Black, so a little bonus is in order... --Amy */
		if (skill == P_SLING) multishot++;
		break;
	    default:
		break;	/* No bonus */
	    }

		if (!(PlayerCannotUseSkills)) {

			switch (P_SKILL(P_MISSILE_WEAPONS)) {
				/* These fallthroughs are intentional --Amy */
				case P_SUPREME_MASTER:
					if ((skill == P_DAGGER || skill == P_KNIFE || skill == P_SPEAR ) && !rn2(5)) multishot++;
					if (skill == P_JAVELIN && !rn2(3)) multishot++;
					if ((skill == P_DART || skill == P_SHURIKEN || skill == P_BOOMERANG || skill == -P_DART || skill == -P_SHURIKEN || skill == -P_BOOMERANG || skill == P_VENOM || skill == -P_VENOM ) && !rn2(4)) multishot++;
				case P_GRAND_MASTER:
					if ((skill == P_DAGGER || skill == P_KNIFE || skill == P_SPEAR ) && !rn2(5)) multishot++;
					if (skill == P_JAVELIN && !rn2(3)) multishot++;
					if ((skill == P_DART || skill == P_SHURIKEN || skill == P_BOOMERANG || skill == -P_DART || skill == -P_SHURIKEN || skill == -P_BOOMERANG || skill == P_VENOM || skill == -P_VENOM ) && !rn2(4)) multishot++;
				case P_MASTER:
					if ((skill == P_DAGGER || skill == P_KNIFE || skill == P_SPEAR ) && !rn2(5)) multishot++;
					if (skill == P_JAVELIN && !rn2(3)) multishot++;
					if ((skill == P_DART || skill == P_SHURIKEN || skill == P_BOOMERANG || skill == -P_DART || skill == -P_SHURIKEN || skill == -P_BOOMERANG || skill == P_VENOM || skill == -P_VENOM ) && !rn2(4)) multishot++;
			    	case P_EXPERT:
					if ((skill == P_DAGGER || skill == P_KNIFE || skill == P_SPEAR ) && !rn2(5)) multishot++;
					if (skill == P_JAVELIN && !rn2(3)) multishot++;
					if ((skill == P_DART || skill == P_SHURIKEN || skill == P_BOOMERANG || skill == -P_DART || skill == -P_SHURIKEN || skill == -P_BOOMERANG || skill == P_VENOM || skill == -P_VENOM ) && !rn2(4)) multishot++;
				case P_SKILLED:
					if ((skill == P_DAGGER || skill == P_KNIFE || skill == P_SPEAR ) && !rn2(5)) multishot++;
					if (skill == P_JAVELIN && !rn2(3)) multishot++;
					if ((skill == P_DART || skill == P_SHURIKEN || skill == P_BOOMERANG || skill == -P_DART || skill == -P_SHURIKEN || skill == -P_BOOMERANG || skill == P_VENOM || skill == -P_VENOM ) && !rn2(4)) multishot++;
				case P_BASIC:
					if ((skill == P_DAGGER || skill == P_KNIFE || skill == P_SPEAR ) && !rn2(5)) multishot++;
					if (skill == P_JAVELIN && !rn2(3)) multishot++;
					if ((skill == P_DART || skill == P_SHURIKEN || skill == P_BOOMERANG || skill == -P_DART || skill == -P_SHURIKEN || skill == -P_BOOMERANG || skill == P_VENOM || skill == -P_VENOM ) && !rn2(4)) multishot++;
				default: break;
			}
		}

	if ((long)multishot > obj->quan) multishot = (int)obj->quan;

	    if (multishot > 0)
	    	multishot = rnd(multishot); /* Some randomness */
	    else multishot = 1;
	       
	    /* Tech: Flurry */
	    if ( (objects[obj->otyp].oc_skill == -P_BOW || objects[obj->otyp].oc_skill == -P_CROSSBOW || objects[obj->otyp].oc_skill == -P_SLING) && tech_inuse(T_FLURRY)) {
		multishot += 1; multishot += rnd(multishot); /* Let'em rip! Extra bonus added by Amy. */

		/* more than usual == volley */
		if (((shotlimit <= 0) || (shotlimit >= multishot)) && 
			(obj->quan >= multishot))
		    You("let fly a volley of %s!", xname(obj));
	    }

	    if ( (objects[obj->otyp].oc_skill == -P_DAGGER || objects[obj->otyp].oc_skill == P_DAGGER || objects[obj->otyp].oc_skill == -P_DART || objects[obj->otyp].oc_skill == P_DART || objects[obj->otyp].oc_skill == -P_SHURIKEN || objects[obj->otyp].oc_skill == P_SHURIKEN || objects[obj->otyp].oc_skill == -P_BOOMERANG || objects[obj->otyp].oc_skill == P_BOOMERANG || objects[obj->otyp].oc_skill == -P_VENOM || objects[obj->otyp].oc_skill == P_VENOM || objects[obj->otyp].oc_skill == -P_KNIFE || objects[obj->otyp].oc_skill == P_KNIFE || objects[obj->otyp].oc_skill == -P_SPEAR || objects[obj->otyp].oc_skill == P_SPEAR || objects[obj->otyp].oc_skill == -P_JAVELIN || objects[obj->otyp].oc_skill == P_JAVELIN) && uarmf && uarmf->oartifact == ART_H__S_GANGSTER_KICKS) {
		multishot += 1;
	    }

	    if ( (objects[obj->otyp].oc_skill == -P_DAGGER || objects[obj->otyp].oc_skill == P_DAGGER || objects[obj->otyp].oc_skill == -P_DART || objects[obj->otyp].oc_skill == P_DART || objects[obj->otyp].oc_skill == -P_SHURIKEN || objects[obj->otyp].oc_skill == P_SHURIKEN || objects[obj->otyp].oc_skill == -P_BOOMERANG || objects[obj->otyp].oc_skill == P_BOOMERANG || objects[obj->otyp].oc_skill == -P_VENOM || objects[obj->otyp].oc_skill == P_VENOM || objects[obj->otyp].oc_skill == -P_KNIFE || objects[obj->otyp].oc_skill == P_KNIFE || objects[obj->otyp].oc_skill == -P_SPEAR || objects[obj->otyp].oc_skill == P_SPEAR || objects[obj->otyp].oc_skill == -P_JAVELIN || objects[obj->otyp].oc_skill == P_JAVELIN) && tech_inuse(T_DOUBLE_THROWNAGE)) {
		multishot += 1; multishot += rnd(multishot); /* Let'em rip! Extra bonus added by Amy. */

		/* more than usual == volley */
		if (((shotlimit <= 0) || (shotlimit >= multishot)) && 
			(obj->quan >= multishot))
		    You("throw a hail of %s!", xname(obj));
	    }

	    if (launcher && launcher->oartifact == ART_ARROW_RAIN && (launcher->invoketimer <= monstermoves) ) {
		int artitimeout = rnz(2000);
		if (!rn2(5)) artitimeout = rnz(20000); /* squeaking does not help here, as it's not an actual invoke --Amy */
		multishot += 8;
		fullmultishot = TRUE;
		reallyfullmultishot = TRUE;
		launcher->invoketimer = (monstermoves + artitimeout);
	    }

	    if (launcher && launcher->otyp == PISTOL_PAIR) multishot *= 2;

	    /* Shotlimit controls your rate of fire */
	    if ((shotlimit > 0) && (multishot > shotlimit)) multishot = shotlimit;

	    if (uarm && uarm->oartifact == ART_POWASPEL) {
		multishot -= rnd(2);
		if (multishot < 1) multishot = 1;
	    }

	    if (launcher && launcher->oartifact == ART_DESERT_EAGLE) {
		multishot--;
		if (multishot < 1) multishot = 1;
	    }
	    if (launcher && launcher->oartifact == ART_LEONE_M__GUAGE_SUPER) {
		multishot -= 2;
		if (multishot < 1) multishot = 1;
	    }
	    if (launcher && launcher->otyp == BLADE_BOW) {
		multishot -= 2;
		if (multishot < 1) multishot = 1;
	    }
	    if (obj && obj->otyp == HEAVY_SPEAR) {
		multishot -= 2;
		if (multishot < 1) multishot = 1;
	    }
	    if (obj && obj->otyp == SUPERHEAVY_SPEAR) {
		multishot -= 3;
		if (multishot < 1) multishot = 1;
	    }
	    if (launcher && launcher->oartifact == ART_MOSIN_NAGANT) {
		multishot -= 3;
		if (multishot < 1) multishot = 1;
	    }		
	    if (launcher && launcher->oartifact == ART_BOW_OF_SKADI) {
		multishot -= rnd(2);
		if (multishot < 1) multishot = 1;
	    }
	    if (launcher && launcher->otyp == SHOVEL) {
		multishot--;
		if (multishot < 1) multishot = 1;
	    }
	    if (launcher && launcher->oartifact == ART_OZYZEVPDWTVP) {
		multishot--;
		if (multishot < 1) multishot = 1;
	    }
	    if (uarmc && uarmc->oartifact == ART_OLD_PERSON_TALK) {
		multishot /= 2;
		if (multishot < 1) multishot = 1;
	    }
	    if (uarms && uarms->otyp == COMPLETE_BLOCKAGE_SHIELD) {
		multishot /= 2;
		multishot--;
		if (multishot < 1) multishot = 1;
	    }

	    /* Rate of fire is intrinsic to the weapon - cannot be user selected
	     * except via altmode
	     * Only for valid launchers 
	     * (currently oc_rof conflicts with wsdam)
	     */
	    if (launcher && is_launcher(launcher))
	    {
		if (objects[(launcher->otyp)].oc_rof && launcher->oartifact != ART_EXPERIMENTAL_MIRV) {
		    multishot += (objects[(launcher->otyp)].oc_rof - 1);
		    if (launcher->oartifact == ART_PRECISION_STRIKE) multishot -= 2;
		}
		if (launcher->altmode == WP_MODE_SINGLE)
		  /* weapons switchable b/w full/semi auto */
		    multishot = 1;
		else if (launcher->altmode == WP_MODE_BURST)
		    multishot = ((multishot > 3) ? (multishot / 3) : 1);
		/* else it is auto == no change */

		if (objects[(launcher->otyp)].oc_rof) {
			if (launcher->altmode == WP_MODE_AUTO && obj->quan < objects[(launcher->otyp)].oc_rof) {
				pline("You do not have enough ammo to fire that weapon in full-auto mode!");
				return(0);
			} else if (launcher->altmode == WP_MODE_BURST && obj->quan < (objects[(launcher->otyp)].oc_rof / 3) ) {
				pline("You do not have enough ammo to fire that weapon in burst-fire mode!");
				return(0);
			}
		}
	    }

	    if ((long)multishot > obj->quan) multishot = (int)obj->quan;
	}

	if (multishot < 1) multishot = 1;

	if (objects[obj->otyp].oc_skill == -P_CROSSBOW && launcher && launcher->otyp != DEMON_CROSSBOW && multishot > 1) multishot = rnd(multishot);

	if (launcher && launcher->oartifact == ART_DOLORES__WINNING_STRAT) {
		if (multishot > 2) multishot = 2;
	}

	if (launcher && launcher->oartifact == ART_GAT_FROM_HELL) {
		int drainhp = multishot;
		while (drainhp > 0) {
			drainhp--;
			if (u.uhpmax > 1) {
				u.uhpmax--;
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			} else {
				u.youaredead = 1;
				pline("Unfortunately you didn't have enough health to power this dangerous weapon.");
				killer_format = KILLED_BY;
				killer = "firing the gat from hell";
				done(DIED);
				u.youaredead = 0;
			}
		}
		pline("%d points of health have been drained!", multishot);
		flags.botl = TRUE;
	}

	if (launcher && launcher->oartifact == ART_UPGRADED_LEMURE) {
		int verisiertnumber = multishot * 100;
		u.uprops[VERISIERTEFFECT].intrinsic += verisiertnumber;
	}

	if (launcher && launcher->oartifact == ART_POST_OFFICE_COURSE) {
		int postofficenumber = multishot;
		adjalign(-postofficenumber);
		u.alignlim -= postofficenumber;
	}

	if (launcher && (launcher->otyp == SUBMACHINE_GUN || launcher->otyp == LEAD_UNLOADER || launcher->otyp == ASSAULT_RIFLE || launcher->otyp == STORM_RIFLE || launcher->otyp == KALASHNIKOV || launcher->otyp == AUTO_SHOTGUN || launcher->otyp == MILITARY_RIFLE) && launcher->altmode == WP_MODE_AUTO && !bulletator_allowed(1)) {
		if (launcher->otyp == SUBMACHINE_GUN) {
			u.bulletatorwantedlevel += 1;
			u.bulletatortimer += 100;
			if (P_MAX_SKILL(P_FIREARM) == P_ISRESTRICTED && P_MAX_SKILL(P_GUN_CONTROL) == P_ISRESTRICTED) u.bulletatortimer += 300;
		}
		if (launcher->otyp == LEAD_UNLOADER) {
			u.bulletatorwantedlevel += 1;
			u.bulletatortimer += 100;
			if (P_MAX_SKILL(P_FIREARM) == P_ISRESTRICTED && P_MAX_SKILL(P_GUN_CONTROL) == P_ISRESTRICTED) u.bulletatortimer += 300;
		}
		if (launcher->otyp == ASSAULT_RIFLE) {
			u.bulletatorwantedlevel += 3;
			u.bulletatortimer += 300;
			if (P_MAX_SKILL(P_FIREARM) == P_ISRESTRICTED && P_MAX_SKILL(P_GUN_CONTROL) == P_ISRESTRICTED) u.bulletatortimer += 1000;
		}
		if (launcher->otyp == STORM_RIFLE) {
			u.bulletatorwantedlevel += 3;
			u.bulletatortimer += 300;
			if (P_MAX_SKILL(P_FIREARM) == P_ISRESTRICTED && P_MAX_SKILL(P_GUN_CONTROL) == P_ISRESTRICTED) u.bulletatortimer += 1000;
		}
		if (launcher->otyp == AUTO_SHOTGUN) {
			u.bulletatorwantedlevel += 1;
			u.bulletatortimer += 100;
			if (P_MAX_SKILL(P_FIREARM) == P_ISRESTRICTED && P_MAX_SKILL(P_GUN_CONTROL) == P_ISRESTRICTED) u.bulletatortimer += 1000;
		}
		if (launcher->otyp == KALASHNIKOV) {
			u.bulletatorwantedlevel += 4;
			u.bulletatortimer += 400;
			if (P_MAX_SKILL(P_FIREARM) == P_ISRESTRICTED && P_MAX_SKILL(P_GUN_CONTROL) == P_ISRESTRICTED) u.bulletatortimer += 2000;
		}
		if (launcher->otyp == MILITARY_RIFLE) {
			u.bulletatorwantedlevel += 9;
			u.bulletatortimer += 850;
			if (P_MAX_SKILL(P_FIREARM) == P_ISRESTRICTED && P_MAX_SKILL(P_GUN_CONTROL) == P_ISRESTRICTED) u.bulletatortimer += 5000;
		}
		if (!u.bulletatorgun) {
			pline("You're not allowed to use automatic firearms. Bulletators have been alerted.");
			u.bulletatorgun = TRUE;
		}
	}
	if (launcher && (launcher->otyp == ARM_BLASTER || launcher->otyp == HEAVY_MACHINE_GUN || launcher->otyp == BFG ) && !bulletator_allowed(1)) {
		if (launcher->otyp == ARM_BLASTER) {
			u.bulletatorwantedlevel += 5;
			u.bulletatortimer += 500;
			if (P_MAX_SKILL(P_FIREARM) == P_ISRESTRICTED && P_MAX_SKILL(P_GUN_CONTROL) == P_ISRESTRICTED) u.bulletatortimer += 1500;
		}
		if (launcher->otyp == HEAVY_MACHINE_GUN) {
			u.bulletatorwantedlevel += 10;
			u.bulletatortimer += 1000;
			if (P_MAX_SKILL(P_FIREARM) == P_ISRESTRICTED && P_MAX_SKILL(P_GUN_CONTROL) == P_ISRESTRICTED) u.bulletatortimer += 5000;
		}
		if (launcher->otyp == BFG) {
			u.bulletatorwantedlevel += 10;
			u.bulletatortimer += 1000;
			if (P_MAX_SKILL(P_FIREARM) == P_ISRESTRICTED && P_MAX_SKILL(P_GUN_CONTROL) == P_ISRESTRICTED) u.bulletatortimer += 5000;
		}
		if (!u.bulletatorgun) {
			pline("You're not allowed to use automatic firearms. Bulletators have been alerted.");
			u.bulletatorgun = TRUE;
		}
	}
	if (launcher && launcher->otyp == DEMON_CROSSBOW && launcher->altmode == WP_MODE_AUTO && !bulletator_allowed(3)) {
		if (!rn2(2)) {
			u.bulletatorwantedlevel += 1;
			u.bulletatortimer += 100;
		}
		if (!u.bulletatorxbow) {
			pline("You're not allowed to use the demon crossbow. Bulletators have been alerted.");
			u.bulletatorxbow = TRUE;
		}
	}
	if (launcher && launcher->otyp == HYDRA_BOW && !bulletator_allowed(2)) {
		if (!rn2(5)) {
			u.bulletatorwantedlevel += 1;
			u.bulletatortimer += 100;
		}
		if (!u.bulletatorbow) {
			pline("You're not allowed to use the hydra bow. Bulletators have been alerted.");
			u.bulletatorbow = TRUE;
		}
	}

	if (launcher && launcher->otyp == CATAPULT && !bulletator_allowed(4)) {
		if (!rn2(2)) {
			u.bulletatorwantedlevel += 1;
			u.bulletatortimer += 100;
		}
		if (!u.bulletatorsling) {
			pline("You're not allowed to use the catapult. Bulletators have been alerted.");
			u.bulletatorsling = TRUE;
		}
	}

	if (obj && obj->otyp == FLAMETHROWER && !bulletator_allowed(5)) {
		if (!rn2(3)) {
			u.bulletatorwantedlevel += 1;
			u.bulletatortimer += 100;
		}
		if (!u.bulletatorjavelin) {
			pline("You're not allowed to use the flamethrower. Bulletators have been alerted.");
			u.bulletatorjavelin = TRUE;
		}
	}

	if (launcher && launcher->otyp == DEMON_CROSSBOW && !rn2(100)) {
		int attempts = 0;
		register struct permonst *ptrZ;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

newbossO:
		do {
			ptrZ = rndmonst();
			attempts++;
			if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
			if (!rn2(2000)) reset_rndmonst(NON_PM);

		} while ( (!ptrZ || (ptrZ && !is_demon(ptrZ))) && attempts < 50000);

		if (ptrZ && is_demon(ptrZ)) {
			(void) makemon(ptrZ, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			pline("A demon suddenly appears from nowhere!");
		} else if (rn2(50)) {
			attempts = 0;
			goto newbossO;
		}

		u.mondiffhack = 0;
		u.aggravation = 0;
	}

	if (launcher && launcher->otyp == DEMON_CROSSBOW && !rn2(100)) {
		int tryct = 0;
		int x, y;
		boolean canbeinawall = FALSE;
		if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

		for (tryct = 0; tryct < 2000; tryct++) {
			x = rn1(COLNO-3,2);
			y = rn2(ROWNO);

			if (isok(x, y) && ((levl[x][y].typ > DBWALL) || canbeinawall) && !(t_at(x, y)) ) {
				(void) maketrap(x, y, rndtrap(), 100, FALSE);
				break;
				}
		}

	}

	if (launcher && launcher->otyp == DEMON_CROSSBOW && !rn2(100)) {
		badeffect();
		pline_The("demon crossbow malfunctioned!");
		return(1);
	}

	/* nerf multishot --Amy */

	if (!reallyfullmultishot) fullmultishot = 0;
	if (tech_inuse(T_FLURRY) && rn2(3)) fullmultishot = 1;
	if (tech_inuse(T_DOUBLE_THROWNAGE) && rn2(3)) fullmultishot = 1;

	if (!(PlayerCannotUseSkills)) {

	    switch (P_SKILL(P_MISSILE_WEAPONS)) {
	    default:	break;
	    case P_SKILLED: if (!rn2(8)) fullmultishot = 1; break;
	    case P_EXPERT: if (!rn2(6)) fullmultishot = 1; break;
	    case P_MASTER: if (!rn2(4)) fullmultishot = 1; break;
	    case P_GRAND_MASTER: if (!rn2(2)) fullmultishot = 1; break;
	    case P_SUPREME_MASTER: fullmultishot = 1; break;
	    }
	}

	if ((multishot > 3) && !fullmultishot && !(launcher && launcher->oartifact == ART_STREAMSHOOTER) && !(objects[obj->otyp].oc_skill == -P_FIREARM) && !(objects[obj->otyp].oc_skill == P_FIREARM) ) multishot = 2 + rno(multishot - 2);

	if (multishot > 1 && isfriday && !rn2(3)) multishot = rnd(multishot);

	if (obj && obj->otyp == JUMPING_FLAMER) multishot = 1;
	if (launcher && launcher->oartifact == ART_TSCHUEUU) multishot = 1;

	m_shot.s = (ammo_and_launcher(obj, uwep) && !(uwep && uwep->otyp == LASERXBOW && !uwep->lamplit) && !(uwep && uwep->otyp == KLIUSLING && !uwep->lamplit) ) ? TRUE : FALSE;
	/* give a message if shooting more than one, or if player
	   attempted to specify a count */
	if (multishot > 1 || shotlimit > 0) {
	    /* "You shoot N arrows." or "You throw N daggers." */
	    You("%s %d %s.",
		m_shot.s ? "shoot" : "throw",
		multishot,	/* (might be 1 if player gave shotlimit) */
		(multishot == 1) ? singular(obj, xname) :  xname(obj));
	}

	if (obj && obj->oartifact == ART_POWERED_BY_HUNGER) {
		morehungry(multishot * 50);
	}

	if (obj && uwep && ammo_and_launcher(obj,uwep) && uwep->oartifact == ART_BUG_BAZOOKA) {
		(void) makemon(mkclass(S_XAN,0), 0, 0, MM_ANGRY);
		(void) makemon(mkclass(S_ANT,0), 0, 0, MM_ANGRY);
	}

	if (launcher && launcher->oartifact == ART_TSCHUEUU) youmonst.movement += 6;

	if (launcher && launcher->oartifact == ART_NOZZLE_CHANGE) {
		if (launcher->altmode == WP_MODE_AUTO) launcher->altmode = WP_MODE_SINGLE;
		else launcher->altmode = WP_MODE_AUTO;

	}

	if (launcher && launcher->oartifact == ART_SHENA_S_PANTY && !rn2(100)) {
		TimeStopped += rn1(2,2);
		pline((Role_if(PM_SAMURAI) || Role_if(PM_NINJA)) ? "Jikan ga teishi shimashita." : "Time has stopped.");
	}

	if (launcher && launcher->oartifact == ART_SCENTFUL_PANTY && !rn2(100)) {
		TimeStopped += rn1(2,2);
		pline((Role_if(PM_SAMURAI) || Role_if(PM_NINJA)) ? "Jikan ga teishi shimashita." : "Time has stopped.");
	}

	if (launcher && launcher->oartifact == ART_SPEW_THE_BOW) {
		buzz(10, rnd(4), u.ux, u.uy, u.dx, u.dy);
	}

	if (launcher && obj && obj->oartifact == ART_SUFFOCATION_BREATH && u.ualign.record > 0) {
		buzz(16, 10, u.ux, u.uy, u.dx, u.dy); /* poison gas */
		increasesincounter(1);
		u.alignlim--;
		adjalign(-1);
	}

	if (launcher && obj && obj->oartifact == ART_PAY_FOR_THE_TOILET) {
		FemaleTrapAntjeX += 2000;
	}

	if (tech_inuse(T_GREEN_MISSILE) && obj && obj->oclass == VENOM_CLASS) {
		int melteestrength = 1;
		if (techlevX(get_tech_no(T_GREEN_MISSILE)) > 9) melteestrength += (techlevX(get_tech_no(T_GREEN_MISSILE)) / 10);
		buzz(16, melteestrength, u.ux, u.uy, u.dx, u.dy);
	}

	if (obj && obj->oartifact == ART_ACTUAL_FLAME && u.uen >= 5) {
		u.uen -= 5;
		flags.botl = TRUE;
		buzz(1, 1, u.ux, u.uy, u.dx, u.dy); /* 1, not 11, or it'll explode! */
	}

	if (tech_inuse(T_BLADE_ANGER) && (objects[obj->otyp].oc_skill == -P_SHURIKEN || objects[obj->otyp].oc_skill == P_SHURIKEN ) ) {
		struct obj *pseudo;
		pseudo = mksobj(SPE_BLADE_ANGER, FALSE, 2, FALSE);
		if (!pseudo) goto bladeangerdone;
		if (pseudo->otyp == GOLD_PIECE) pseudo->otyp = SPE_BLADE_ANGER; /* minimalist fix */
		pseudo->blessed = pseudo->cursed = 0;
		pseudo->quan = 20L;			/* do not let useup get it */
		pseudo->spe = obj->spe;
		angeramount = multishot;

		/* we have to clone the entire remaining function because of eternal bugs...
		 * defining pseudo in the function leads to obfree throwing an error if you didn't use blade anger,
		 * and pseudo isn't used in any other way (yet), so let's do it like this --Amy */

		wep_mask = obj->owornmask;
		m_shot.o = obj->otyp;
		m_shot.n = multishot;
		for (m_shot.i = 1; m_shot.i <= m_shot.n; m_shot.i++) {
		    twoweap = u.twoweap;
		    /* split this object off from its slot if necessary */
		    if (obj->quan > 1L) {
			otmp = splitobj(obj, 1L);
		    } else {
			otmp = obj;
			if (otmp->owornmask)
			    remove_worn_item(otmp, FALSE);
		    }
		    freeinv(otmp);
		    throwit(otmp, wep_mask, twoweap, thrown);
		}
		m_shot.n = m_shot.i = 0;
		m_shot.o = STRANGE_OBJECT;
		m_shot.s = FALSE;

		/* now do blade anger damage (it usually has a higher range than the thrown shuriken) */
		if (u.uen < 10) pline("Not enough mana for blade anger.");
		while (pseudo && angeramount >= 1) {
			if (u.uen >= 10) {
				u.uen -= 10;
				flags.botl = TRUE;
				weffects(pseudo);
			}
			angeramount--;
		}
		if (pseudo) obfree(pseudo, (struct obj *)0);	/* now, get rid of it */
		return 1;

	}

	if ((tech_inuse(T_BEAMSWORD) || (obj && obj->oartifact == ART_LINK_S_MASTER_SWORD)) && is_lightsaber(obj) && obj->lamplit ) {
		if (obj && obj->oartifact == ART_LINK_S_MASTER_SWORD) u.linkmasterswordhack = 1;
		struct obj *pseudo;
		pseudo = mksobj(SPE_BEAMSWORD, FALSE, 2, FALSE);
		if (!pseudo) goto bladeangerdone;
		if (pseudo->otyp == GOLD_PIECE) pseudo->otyp = SPE_BEAMSWORD; /* minimalist fix */
		pseudo->blessed = pseudo->cursed = 0;
		pseudo->quan = 20L;			/* do not let useup get it */
		pseudo->spe = obj->spe;

		/* we have to clone the entire remaining function because of eternal bugs...
		 * defining pseudo in the function leads to obfree throwing an error if you didn't use blade anger,
		 * and pseudo isn't used in any other way (yet), so let's do it like this --Amy */

		wep_mask = obj->owornmask;
		m_shot.o = obj->otyp;
		m_shot.n = multishot;
		for (m_shot.i = 1; m_shot.i <= m_shot.n; m_shot.i++) {
		    twoweap = u.twoweap;
		    /* split this object off from its slot if necessary */
		    if (obj->quan > 1L) {
			otmp = splitobj(obj, 1L);
		    } else {
			otmp = obj;
			if (otmp->owornmask)
			    remove_worn_item(otmp, FALSE);
		    }
		    freeinv(otmp);
		    throwit(otmp, wep_mask, twoweap, thrown);
		}
		m_shot.n = m_shot.i = 0;
		m_shot.o = STRANGE_OBJECT;
		m_shot.s = FALSE;

		/* now do beamsword damage (it usually has a higher range than the thrown lightsaber) */
		weffects(pseudo);

		if (pseudo) obfree(pseudo, (struct obj *)0);	/* now, get rid of it */
		return 1;

	}

bladeangerdone:

	wep_mask = obj->owornmask;
	m_shot.o = obj->otyp;
	m_shot.n = multishot;
	for (m_shot.i = 1; m_shot.i <= m_shot.n; m_shot.i++) {

		int savechance = 0;

		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_MISSILE_WEAPONS)) {

			    case P_BASIC:		savechance = 1; break;
			    case P_SKILLED:	savechance = 2; break;
			    case P_EXPERT:	savechance = 3; break;
			    case P_MASTER:	savechance = 4; break;
			    case P_GRAND_MASTER:savechance = 5; break;
			    case P_SUPREME_MASTER:savechance = 6; break;
			    default: savechance += 0; break;
			}
		}

	    if (!obj) { /* uh-oh */
		You("suddenly ran out of ammo for your ranged attack!");
		m_shot.n = m_shot.i = 0;
		m_shot.o = STRANGE_OBJECT;
		m_shot.s = FALSE;
		return 1;
	    }

		/* melee weapons dull when used repeatedly, so firing a launcher should do the same --Amy */
		if (uwep && ammo_and_launcher(obj, uwep) && weaponwilldull(uwep) && (rnd(7) > savechance) && !issoviet) {
			if (uwep->greased) {
				uwep->greased--;
				pline("Your weapon loses its grease.");
			} else {
				uwep->spe--;
				pline("Your weapon dulls.");
				u.cnd_weapondull++;
			}
		}
		if (uswapwep && ammo_and_launcher(obj, uswapwep) && weaponwilldull(uswapwep) && (rnd(7) > savechance) && !issoviet) {
			if (uswapwep->greased) {
				uswapwep->greased--;
				pline("Your weapon loses its grease.");
			} else {
				uswapwep->spe--;
				pline("Your weapon dulls.");
				u.cnd_weapondull++;
			}
		}

		if (uwep && uwep->oartifact == ART_LOUD_SHITTER && ammo_and_launcher(obj, uwep)) {
			wake_nearby();
		}
		if (uswapwep && uswapwep->oartifact == ART_LOUD_SHITTER && ammo_and_launcher(obj, uswapwep)) {
			wake_nearby();
		}

	    twoweap = u.twoweap;
	    /* split this object off from its slot if necessary */
	    if (obj->quan > 1L) {
		otmp = splitobj(obj, 1L);
	    } else {
		otmp = obj;
		if (otmp->owornmask)
		    remove_worn_item(otmp, FALSE);
	    }
	    freeinv(otmp);
	    throwit(otmp, wep_mask, twoweap, thrown);
	    if (autismweaponcheck(ART_PSG) || autismweaponcheck(ART_BORKED_PARA)) {
		pushplayer(FALSE);
	    }
	}
	m_shot.n = m_shot.i = 0;
	m_shot.o = STRANGE_OBJECT;
	m_shot.s = FALSE;

	return 1;

}


int
dothrow()
{
	register struct obj *obj;
	int oldmulti = multi, result, shotlimit;
	char *oldsave_cm = save_cm;
	struct trap *trap;

	/*
	 * Since some characters shoot multiple missiles at one time,
	 * allow user to specify a count prefix for 'f' or or select 
	 * a number of items in the item selection for 't' to limit
	 * number of items thrown (to avoid possibly hitting something
	 * behind target after killing it, or perhaps to conserve ammo).
	 *
	 * Nethack 3.3.0 uses prefixes for all - should this revert to that?
	 *
	 * Prior to 3.3.0, command ``3t'' meant ``t(shoot) t(shoot) t(shoot)''
	 * and took 3 turns.  Now it means ``t(shoot at most 3 missiles)''.
	 */

	if (trap = t_at(u.ux, u.uy)) {
		if (trap->ttyp == VIVISECTION_TRAP) {
			You("are in vivisection, and therefore unable to use a ranged attack!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return 0;
		}
	}

	if (u.rangedreload) {
		You("have to reload, which is gonna take %d more turns!", u.rangedreload);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}

	if (u.twoweap && uarms) {
		You("are way too busy with your two weapons and shield.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}

	if (notake(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
	    You("are physically incapable of throwing anything.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		if (yn("But you can try to throw anyway. Okay?") == 'y') {
			if (rn2(3) && !polyskillchance()) {
		 		morehungry(10);
				pline("The darn thing doesn't seem to fly very far.");
				if (!rn2(20)) badeffect();
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
				return 1;
			}
		}
		else {return(0);}
	}

	if(check_capacity((char *)0)) return(0);
	obj = getobj(uslinging() ? bullets : toss_objs, "throw");
	/* it is also possible to throw food */
	/* (or jewels, or iron balls... ) */

	if (!obj) return(0);

#ifdef MAIL
	if (obj->otyp == SCR_MAIL) {
		You("try to cheat, but it fails.");
		if (Is_airlevel(&u.uz)) pline("Stop being a wiseguy and play the game properly already! :-P");
		return(0);
	}
#endif

	if (EpviProblemActive && obj && (objects[obj->otyp].oc_minlvl > (u.ulevel + u.xtralevelmult - 1) ) ) {
		verbalize("I cannot use that yet.");
		return(0);
	}

	/* kludge to work around parse()'s pre-decrement of 'multi' */
	shotlimit = (multi || save_cm) ? multi + 1 : 0;

        result = throw_obj(obj, shotlimit, THROW_UWEP);
        
	/*
	 * [ALI] Bug fix: Temporary paralysis (eg., from hurtle) cancels
	 * any count for the throw command.
	 */
	if (multi >= 0)
	    multi = oldmulti;
        save_cm = oldsave_cm;
        return (result);
}


/* KMH -- automatically fill quiver */
/* Suggested by Jeffrey Bay <jbay@convex.hp.com> */
static void
autoquiver()
{
	struct obj *otmp, *oammo = 0, *omissile = 0, *omisc = 0, *altammo = 0;

	if (uquiver)
	    return;

	/* Scan through the inventory */
	for (otmp = invent; otmp; otmp = otmp->nobj) {
	    if (otmp->owornmask || otmp->oartifact || !otmp->dknown) {
		;	/* Skip it */
	    } else if (otmp->otyp == ROCK ||
			/* seen rocks or known flint or known glass */
			(objects[otmp->otyp].oc_name_known &&
			 otmp->otyp == FLINT) ||
			(objects[otmp->otyp].oc_name_known &&
			 otmp->oclass == GEM_CLASS &&
			 objects[otmp->otyp].oc_material == MT_GLASS)) {
		if (uslinging())
		    oammo = otmp;
		else if (ammo_and_launcher(otmp, uswapwep))
		    altammo = otmp;
		else if (!omisc)
		    omisc = otmp;
	    } else if (otmp->oclass == GEM_CLASS) {
		;	/* skip non-rock gems--they're ammo but
			   player has to select them explicitly */
	    } else if (is_ammo(otmp)) {
		if (ammo_and_launcher(otmp, uwep))
		    /* Ammo matched with launcher (bow and arrow, crossbow and bolt) */
		    oammo = otmp;
		else if (ammo_and_launcher(otmp, uswapwep))
		    altammo = otmp;
		else
		    /* Mismatched ammo (no better than an ordinary weapon) */
		    omisc = otmp;
	    } else if (is_missile(otmp)) {
		/* Missile (dart, shuriken, etc.) */
		omissile = otmp;
	    } else if (otmp->oclass == WEAPON_CLASS && throwing_weapon(otmp)) {
		/* Ordinary weapon */
		if (objects[otmp->otyp].oc_skill == P_DAGGER
			&& !omissile) 
		    omissile = otmp;
		else
		    omisc = otmp;
	    }
	}

	/* Pick the best choice */
	if (oammo)
	    setuqwep(oammo);
	else if (omissile)
	    setuqwep(omissile);
	else if (altammo)
	    setuqwep(altammo);
	else if (omisc)
	    setuqwep(omisc);

	return;
}

int
dofire()
{
	int result, shotlimit;
	struct trap *trap;

	if (trap = t_at(u.ux, u.uy)) {
		if (trap->ttyp == VIVISECTION_TRAP) {
			You("are in vivisection, and therefore unable to use a ranged attack!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return 0;
		}
	}

	if (u.twoweap && uarms) {
		You("are way too busy with your two weapons and shield.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}

	if (u.rangedreload) {
		You("have to reload, which is gonna take %d more turns!", u.rangedreload);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}

	if (notake(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
	    You("are physically incapable of doing that.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		if (yn("But you can try to fire anyway. Okay?") == 'y') {
			if (rn2(3) && !polyskillchance()) {
	 		morehungry(10);
			pline("The darn thing doesn't seem to fly very far.");
			if (!rn2(20)) badeffect();
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return 1;
			}
		}
		else {return(0);}
	}

#ifdef MAIL
	if (uquiver && uquiver->otyp == SCR_MAIL) {
		You("try to cheat, but it fails.");
		if (Is_airlevel(&u.uz)) pline("Stop being a wiseguy and play the game properly already! :-P");
		return(0);
	}
#endif

	if (EpviProblemActive && uquiver && (objects[uquiver->otyp].oc_minlvl > (u.ulevel + u.xtralevelmult - 1) ) ) {
		verbalize("I cannot use that yet.");
		return(0);
	}

	if(check_capacity((char *)0)) return(0);
	if (!uquiver) {
		if (!flags.autoquiver) {
			/* Don't automatically fill the quiver */
			You("have no ammunition readied!");
			if (iflags.quiver_fired)
			  dowieldquiver(); /* quiver_fired */
			if (!uquiver)
			  return(dothrow());
		}
		else { /* quiver_fired */
		autoquiver();
		if (!uquiver) {
			You("have nothing appropriate for your quiver!");
			return(dothrow());
		} else {
			You("fill your quiver:");
			prinv((char *)0, uquiver, 0L);
		}
		} /* quiver_fired */
	}

	/*
	 * Since some characters shoot multiple missiles at one time,
	 * allow user to specify a count prefix for 'f' or 't' to limit
	 * number of items thrown (to avoid possibly hitting something
	 * behind target after killing it, or perhaps to conserve ammo).
	 *
	 * The number specified can never increase the number of missiles.
	 * Using ``5f'' when the shooting skill (plus RNG) dictates launch
	 * of 3 projectiles will result in 3 being shot, not 5.
	 */
	/* kludge to work around parse()'s pre-decrement of `multi' */
	shotlimit = (multi || save_cm) ? multi + 1 : 0;
	multi = 0;		/* reset; it's been used up */

	if (u.twoweap) {
		if (!can_twoweapon()) untwoweapon();
		else if (ammo_and_launcher(uquiver,uwep) 
		    && ammo_and_launcher(uquiver, uswapwep)){
			result = throw_obj(uquiver, shotlimit, THROW_UWEP);
			if ((result == 1) && uquiver) 
			    result += throw_obj(uquiver, shotlimit, THROW_USWAPWEP);
			if (result > 1) result--;
			return(result);
		}
	}
	result = (throw_obj(uquiver, shotlimit, THROW_UWEP));
	
	return result;
}


/*
 * Object hits floor at hero's feet.  Called from drop() and throwit().
 */
void
hitfloor(obj)
register struct obj *obj;
{
	if (IS_SOFT(levl[u.ux][u.uy].typ) || u.uinwater) {
		dropy(obj);
		return;
	}
	if (IS_ALTAR(levl[u.ux][u.uy].typ))
		doaltarobj(obj);
	else
		pline("%s hit%s the %s.", Doname2(obj),
		      (obj->quan == 1L) ? "s" : "", surface(u.ux,u.uy));
	/* waaaaaaaaaaay too big a penalty to lose ALL potions unconditionally --Amy */
	if (!rn2(obj->oerodeproof ? 100 : 20)) {
		if (hero_breaks(obj, u.ux, u.uy, TRUE)) return;
	}
	if (ship_object(obj, u.ux, u.uy, FALSE)) return;
	dropy(obj);
	if (!u.uswallow) container_impact_dmg(obj);
}

/*
 * Walk a path from src_cc to dest_cc, calling a proc for each location
 * except the starting one.  If the proc returns FALSE, stop walking
 * and return FALSE.  If stopped early, dest_cc will be the location
 * before the failed callback.
 */
boolean
walk_path(src_cc, dest_cc, check_proc, arg)
    coord *src_cc;
    coord *dest_cc;
    boolean (*check_proc)(void *, int, int);
    void * arg;
{
    int x, y, dx, dy, x_change, y_change, err, i, prev_x, prev_y;
    boolean keep_going = TRUE;

    /* Use Bresenham's Line Algorithm to walk from src to dest */
    dx = dest_cc->x - src_cc->x;
    dy = dest_cc->y - src_cc->y;
    prev_x = x = src_cc->x;
    prev_y = y = src_cc->y;

    if (dx < 0) {
	x_change = -1;
	dx = -dx;
    } else
	x_change = 1;
    if (dy < 0) {
	y_change = -1;
	dy = -dy;
    } else
	y_change = 1;

    i = err = 0;
    if (dx < dy) {
	while (i++ < dy) {
	    prev_x = x;
	    prev_y = y;
	    y += y_change;
	    err += dx;
	    if (err >= dy) {
		x += x_change;
		err -= dy;
	    }
	/* check for early exit condition */
	if (!(keep_going = (*check_proc)(arg, x, y)))
	    break;
	}
    } else {
	while (i++ < dx) {
	    prev_x = x;
	    prev_y = y;
	    x += x_change;
	    err += dy;
	    if (err >= dx) {
		y += y_change;
		err -= dx;
	    }
	/* check for early exit condition */
	if (!(keep_going = (*check_proc)(arg, x, y)))
	    break;
	}
    }

    if (keep_going)
	return TRUE;	/* successful */

    dest_cc->x = prev_x;
    dest_cc->y = prev_y;
    return FALSE;
}

/*
 * Single step for the hero flying through the air from jumping, flying,
 * etc.  Called from hurtle() and jump() via walk_path().  We expect the
 * argument to be a pointer to an integer -- the range -- which is
 * used in the calculation of points off if we hit something.
 *
 * Bumping into monsters won't cause damage but will wake them and make
 * them angry.  Auto-pickup isn't done, since you don't have control over
 * your movements at the time.
 *
 * Possible additions/changes:
 *	o really attack monster if we hit one
 *	o set stunned if we hit a wall or door
 *	o reset nomul when we stop
 *	o creepy feeling if pass through monster (if ever implemented...)
 *	o bounce off walls
 *	o let jumps go over boulders
 */
boolean
hurtle_step(arg, x, y)
    void * arg;
    int x, y;
{
    int ox, oy, *range = (int *)arg;
    struct obj *obj;
    struct monst *mon;
    boolean may_pass = TRUE;
    struct trap *ttmp;
    
    if (!isok(x,y)) {
	You_feel("the spirits holding you back.");
	return FALSE;
    } else if (!in_out_region(x, y)) {
	return FALSE;
    } else if (*range == 0) {
	return FALSE;			/* previous step wants to stop now */
    }

    if (!Passes_walls || !(may_pass = may_passwall(x, y))) {
	if (IS_ROCKWFL(levl[x][y].typ) || closed_door(x,y)) {
	    const char *s;

	    pline("Ouch!");
	    if (IS_TREE(levl[x][y].typ))
		s = "bumping into a tree";
	    else if (IS_ROCKWFL(levl[x][y].typ))
		s = "bumping into a wall";
	    else
		s = "bumping into a door";
	    losehp(rnd(2+*range), s, KILLED_BY);
	    return FALSE;
	}
	if (levl[x][y].typ == IRONBARS) {
	    You("crash into some iron bars.  Ouch!");
	    losehp(rnd(2+*range), "crashing into iron bars", KILLED_BY);
	    return FALSE;
	}
	if ((obj = sobj_at(BOULDER,x,y)) != 0) {
	    You("bump into a %s.  Ouch!", xname(obj));
	    losehp(rnd(2+*range), "bumping into a boulder", KILLED_BY);
	    return FALSE;
	}
	if (!may_pass) {
	    /* did we hit a no-dig non-wall position? */
	    You("smack into something!");
	    losehp(rnd(2+*range), "touching the edge of the universe", KILLED_BY);
	    return FALSE;
	}
	if ((u.ux - x) && (u.uy - y) &&
		bad_rock(&youmonst,u.ux,y) && bad_rock(&youmonst,x,u.uy)) {
	    boolean too_much = (invent && (inv_weight() + weight_cap() > 5000));
	    /* Move at a diagonal. */
	    if (bigmonst(youmonst.data) || too_much) {
		You("%sget forcefully wedged into a crevice.",
			too_much ? "and all your belongings " : "");
		losehp(rnd(2+*range), "wedging into a narrow crevice", KILLED_BY);
		return FALSE;
	    }
	}
    }

    if ((mon = m_at(x, y)) != 0) {
	You("bump into %s.", a_monnam(mon));
	wakeup(mon);
	return FALSE;
    }
    if ((u.ux - x) && (u.uy - y) &&
	bad_rock(&youmonst, u.ux, y) && bad_rock(&youmonst, x, u.uy)) {
	/* Move at a diagonal. */
	if (In_sokoban(&u.uz)) {
	    You("come to an abrupt halt!");
	    return FALSE;
	}
    }

    ox = u.ux;
    oy = u.uy;
    u.ux = x;
    u.uy = y;
    newsym(ox, oy);		/* update old position */
    vision_recalc(1);		/* update for new position */
    flush_screen(1);
    /* FIXME:
     * Each trap should really trigger on the recoil if
     * it would trigger during normal movement. However,
     * not all the possible side-effects of this are
     * tested [as of 3.4.0] so we trigger those that
     * we have tested, and offer a message for the
     * ones that we have not yet tested.
     */
    if ((ttmp = t_at(x, y)) != 0) {
    	if (ttmp->ttyp == MAGIC_PORTAL) {
    		dotrap(ttmp,0);
    		return FALSE;
	} else if (ttmp->ttyp == FIRE_TRAP) {
    		dotrap(ttmp,0);
	} else if ((ttmp->ttyp == PIT || ttmp->ttyp == SPIKED_PIT || ttmp->ttyp == GIANT_CHASM || ttmp->ttyp == SHIT_PIT || ttmp->ttyp == MANA_PIT || ttmp->ttyp == ANOXIC_PIT || ttmp->ttyp == HYPOXIC_PIT || ttmp->ttyp == ACID_PIT || ttmp->ttyp == SHAFT_TRAP || ttmp->ttyp == CURRENT_SHAFT ||
		    ttmp->ttyp == HOLE || ttmp->ttyp == TRAPDOOR) &&
		   In_sokoban(&u.uz)) {
		/* Air currents overcome the recoil */
    		dotrap(ttmp,0);
		*range = 0;
		return TRUE;
    	} else {
		if (ttmp->tseen)
		    You("pass right over %s %s.",
		    	(ttmp->ttyp == ARROW_TRAP) ? "an" : "a",
		    	defsyms[trap_to_defsym(ttmp->ttyp)].explanation);
    	}
    }
    if (--*range < 0)		/* make sure our range never goes negative */
	*range = 0;
    if (*range != 0)
	delay_output();
    return TRUE;
}

STATIC_OVL boolean
mhurtle_step(arg, x, y)
    void * arg;
    int x, y;
{
	struct monst *mon = (struct monst *)arg;

	/* TODO: Treat walls, doors, iron bars, pools, lava, etc. specially
	 * rather than just stopping before.
	 */
	if (goodpos(x, y, mon, 0) && m_in_out_region(mon, x, y)) {
	    remove_monster(mon->mx, mon->my);
	    newsym(mon->mx, mon->my);
	    place_monster(mon, x, y);
	    newsym(mon->mx, mon->my);
	    set_apparxy(mon);
	    (void) mintrap(mon);
	    return TRUE;
	}
	return FALSE;
}

/*
 * The player moves through the air for a few squares as a result of
 * throwing or kicking something.
 *
 * dx and dy should be the direction of the hurtle, not of the original
 * kick or throw and be only.
 */
void
hurtle(dx, dy, range, verbose)
    int dx, dy, range;
    boolean verbose;
{
    coord uc, cc;

    /* The chain is stretched vertically, so you shouldn't be able to move
     * very far diagonally.  The premise that you should be able to move one
     * spot leads to calculations that allow you to only move one spot away
     * from the ball, if you are levitating over the ball, or one spot
     * towards the ball, if you are at the end of the chain.  Rather than
     * bother with all of that, assume that there is no slack in the chain
     * for diagonal movement, give the player a message and return.
     */
    if(Punished && !carried(uball)) {
	You_feel("a tug from the iron ball.");
	nomul(0, 0, FALSE);
	return;
    } else if (u.utrap) {
	You("are anchored by the %s.",
	    u.utraptype == TT_WEB ? "web" : u.utraptype == TT_GLUE ? "glue" : u.utraptype == TT_LAVA ? "lava" :
		u.utraptype == TT_INFLOOR ? surface(u.ux,u.uy) : "trap");
	nomul(0, 0, FALSE);
	return;
    }

    /* make sure dx and dy are [-1,0,1] */
    dx = sgn(dx);
    dy = sgn(dy);

    if(!range || (!dx && !dy) || u.ustuck) return; /* paranoia */

    nomul(-range, "moving through the air", TRUE);
    nomovemsg = 0;
    if (verbose)
	You("%s in the opposite direction.", range > 1 ? "hurtle" : "float");
    /* if we're in the midst of shooting multiple projectiles, stop */
    if (m_shot.i < m_shot.n) {
	/* last message before hurtling was "you shoot N arrows" */
	You("stop %sing after the first %s.",
	    m_shot.s ? "shoot" : "throw", m_shot.s ? "shot" : "toss");
	m_shot.n = m_shot.i;	/* make current shot be the last */
    }

    /* This used to give sokoban penalties but you can't actually bypass anything so the penalty is removed --Amy */
    /* Soviet Russia comment is in apply.c */

	if (issoviet && In_sokoban(&u.uz)) {
		change_luck(-1);
		pline("Teper' vy teryayete ochko udachi KHAR KHAR. Eto deystviye ne pomoglo vam reshit' golovolomki, no my takiye elitnyye.");
		if (evilfriday) u.ugangr++;
	}

    uc.x = u.ux;
    uc.y = u.uy;
    /* this setting of cc is only correct if dx and dy are [-1,0,1] only */
    cc.x = u.ux + (dx * range);
    cc.y = u.uy + (dy * range);
    (void) walk_path(&uc, &cc, hurtle_step, (void *)&range);
    teleds(cc.x, cc.y, FALSE);
}

/* Move a monster through the air for a few squares.
 */
void
mhurtle(mon, dx, dy, range)
	struct monst *mon;
	int dx, dy, range;
{
    coord mc, cc;

	/* At the very least, debilitate the monster */
	mon->movement = 0;
	mon->mstun = 1;

	/* Is the monster stuck or too heavy to push?
	 * (very large monsters have too much inertia, even floaters and flyers)
	 */
	if (mon->data->msize >= MZ_HUGE || mon == u.ustuck || mon->mtrapped)
	    return;

    /* Make sure dx and dy are [-1,0,1] */
    dx = sgn(dx);
    dy = sgn(dy);
    if(!range || (!dx && !dy)) return; /* paranoia */

	/* Send the monster along the path */
	mc.x = mon->mx;
	mc.y = mon->my;
	cc.x = mon->mx + (dx * range);
	cc.y = mon->my + (dy * range);
	(void) walk_path(&mc, &cc, mhurtle_step, (void *)mon);
	return;
}

STATIC_OVL void
check_shop_obj(obj, x, y, broken)
register struct obj *obj;
register xchar x, y;
register boolean broken;
{
	struct monst *shkp = shop_keeper(*u.ushops);

	if(!shkp) return;

	if(broken) {
		if (obj->unpaid) {
		    (void)stolen_value(obj, u.ux, u.uy,
				       (boolean)shkp->mpeaceful, FALSE, TRUE);
		    subfrombill(obj, shkp);
		}
		obj->no_charge = 1;
		return;
	}

	if (!costly_spot(x, y) || *in_rooms(x, y, SHOPBASE) != *u.ushops) {
		/* thrown out of a shop or into a different shop */
		if (obj->unpaid) {
		    (void)stolen_value(obj, u.ux, u.uy,
				       (boolean)shkp->mpeaceful, FALSE, FALSE);
		    subfrombill(obj, shkp);
		}
	} else {
		if (costly_spot(u.ux, u.uy) && costly_spot(x, y)) {
		    if(obj->unpaid) subfrombill(obj, shkp);
		    else if(!(x == shkp->mx && y == shkp->my))
			    sellobj(obj, x, y);
		}
	}
}

/*
 * Hero tosses an object upwards with appropriate consequences.
 *
 * Returns FALSE if the object is gone.
 */
STATIC_OVL boolean
toss_up(obj, hitsroof)
struct obj *obj;
boolean hitsroof;
{
    const char *almost;
    /* note: obj->quan == 1 */

    if (hitsroof) {
	if (breaktest(obj)) {
		pline("%s hits the %s.", Doname2(obj), ceiling(u.ux, u.uy));
		breakmsg(obj, !Blind);
		if (issegfaulter && obj->otyp == SEGFAULT_VENOM && !rn2(5) ) { /* segfault panic! */
			u.segfaultpanic = TRUE;
		} else if (obj->oartifact == ART_DO_NOT_THROW_ME) { /* uh-oh... you really messed up big time there. */
			u.segfaultpanic = TRUE;
		}
		breakobj(obj, u.ux, u.uy, TRUE, TRUE);
		return FALSE;
	}
	almost = "";
    } else {
	almost = " almost";
    }
    pline("%s%s hits the %s, then falls back on top of your %s.",
	  Doname2(obj), almost, ceiling(u.ux,u.uy), body_part(HEAD));

    /* object now hits you */

    if (obj->oclass == POTION_CLASS) {
	potionhit(&youmonst, obj, TRUE);
    } else if (breaktest(obj)) {
	int otyp = obj->otyp, ocorpsenm = obj->corpsenm;
	int blindinc;

	/* need to check for blindness result prior to destroying obj */
	blindinc = (otyp == CREAM_PIE || otyp == BLINDING_VENOM) &&
		   /* AT_WEAP is ok here even if attack type was AT_SPIT */
		   can_blnd(&youmonst, &youmonst, AT_WEAP, obj) ? rnd(25) : 0;

	breakmsg(obj, !Blind);
	breakobj(obj, u.ux, u.uy, TRUE, TRUE);
	obj = 0;	/* it's now gone */
	switch (otyp) {
	case PETRIFYIUM_BAR:
		if (!uarmh && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
		    !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM)))
		goto petrify;
	case PETRIFYIUM_BRA:
		if (!uarmh && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
		    !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM)))
		goto petrify;
	case EGG:
		if (touch_petrifies(&mons[ocorpsenm]) && ocorpsenm != PM_PLAYERMON &&
		    !uarmh && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
		    !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM)))
		goto petrify;
	case CREAM_PIE:
	case BLINDING_VENOM:
		pline("You've got it all over your %s!", body_part(FACE));
		if (blindinc) {
		    if (otyp == BLINDING_VENOM && !Blind)
			pline("It blinds you!");
		    u.ucreamed += blindinc;
		    make_blinded(Blinded + (long)blindinc, FALSE);
		    if (!Blind) Your("%s", vision_clears);
		}
		break;
	default:
		break;
	}
	return FALSE;
    } else {		/* neither potion nor other breaking object */
	boolean less_damage = uarmh && is_hardmaterial(uarmh), artimsg = FALSE;
	int dmg = dmgval(obj, &youmonst);

	if (obj->oartifact)
	    /* need a fake die roll here; rn1(18,2) avoids 1 and 20 */
	    artimsg = artifact_hit((struct monst *)0, &youmonst,
				   obj, &dmg, rn1(18,2));

	if (!dmg) {	/* probably wasn't a weapon; base damage on weight */
	    dmg = (int) obj->owt / 100;
	    if (dmg < 1) dmg = 1;
	    else if (dmg > 6) dmg = 6;
	    if (is_shade(youmonst.data) && objects[obj->otyp].oc_material != MT_SILVER && objects[obj->otyp].oc_material != MT_ARCANIUM)
		dmg = 0;
	}
	if (dmg > 1 && less_damage) dmg = 1;
	if (dmg > 0) {
		if (increase_damage_bonus_value() > 1) dmg += rnd(increase_damage_bonus_value());
		else dmg += increase_damage_bonus_value();
	}
	if (dmg > 0 && uarmh && uarmh->oartifact == ART_REMOTE_GAMBLE) dmg += 2;
	if (dmg > 0 && uarm && uarm->oartifact == ART_MOTHERFUCKER_TROPHY) dmg += 5;
	if (dmg > 0 && u.tiksrvzllatdown) dmg += 1;

	if (dmg > 0 && (uarmg && itemhasappearance(uarmg, APP_UNCANNY_GLOVES))) dmg += 1;
	if (dmg > 0 && (uarmg && itemhasappearance(uarmg, APP_SLAYING_GLOVES))) dmg += 1;

	if (dmg > 0 && uarmc && uarmc->oartifact == ART_INA_S_SORROW && u.uhunger < 0) dmg += 3;
	if (dmg > 0 && uwep && uwep->oartifact == ART_SPAMBAIT_FIRE) dmg += 2;
	if (dmg > 0 && uwep && uwep->oartifact == ART_GARY_S_RIVALRY) dmg += 2;
	if (dmg > 0 && uarmf && uarmf->oartifact == ART_KATI_S_IRRESISTIBLE_STILET) dmg += 2;
	if (dmg > 0 && uarmf && uarmf->oartifact == ART_STREET_ROCKZ) dmg += 2;
	if (dmg > 0 && uwep && uwep->oartifact == ART_THOR_S_STRIKE && ACURR(A_STR) >= STR19(25)) dmg += 5;
	if (dmg > 0 && uarmh && uarmh->oartifact == ART_IRON_HELM_OF_GORLIM) dmg += 10;
	if (dmg > 0 && uarmc && uarmc->oartifact == ART_SELVERFEND) dmg += 1;
	if (dmg > 0 && uarm && uarm->oartifact == ART_DESTRUCTO_S_COAT) dmg += 4;
	if (dmg > 0 && uarm && uarm->oartifact == ART_TIMONA_S_INNER_BICKER) dmg += 1;
	if (dmg > 0 && uamul && uamul->oartifact == ART_PLAYING_QUAKE) dmg += 3;
	if (dmg > 0 && uwep && uwep->oartifact == ART_DARKGOD_S_MINUSES) dmg -= 6;
	if (dmg > 0 && u.twoweap && uswapwep && uswapwep->oartifact == ART_DARKGOD_S_MINUSES) dmg -= 6;
	if (dmg > 0 && uarmh && uarmh->oartifact == ART_SUDUNSEL) dmg += 2;
	if (dmg > 0 && uwep && uwep->oartifact == ART_KLOCKING_NOISE) dmg += 2;
	if (dmg > 0 && ublindf && ublindf->oartifact == ART_MEANINGFUL_CHALLENGE) dmg += 1;
	if (dmg > 0 && uarm && uarm->otyp == DARK_DRAGON_SCALES) dmg += 1;
	if (dmg > 0 && uarmg && uarmg->oartifact == ART_FLOEMMELFLOEMMELFLOEMMELFL) dmg += 1;
	if (dmg > 0 && uarm && uarm->otyp == DARK_DRAGON_SCALE_MAIL) dmg += 1;
	if (dmg > 0 && uarms && uarms->otyp == DARK_DRAGON_SCALE_SHIELD) dmg += 1;
	if (dmg > 0 && uarmg && uarmg->oartifact == ART_YES_TO_RANGED_COMBAT) dmg += rnd(6);
	if (dmg > 0 && uleft && uleft->oartifact == ART_BLIND_PILOT) dmg += 10;
	if (dmg > 0 && uright && uright->oartifact == ART_BLIND_PILOT) dmg += 10;
	if (dmg > 0 && uleft && uleft->oartifact == ART_SHL_THEME) dmg += 2;
	if (dmg > 0 && uright && uright->oartifact == ART_SHL_THEME) dmg += 2;
	if (dmg > 0 && uamul && uamul->oartifact == ART_NOW_YOU_HAVE_LOST) dmg += 10;
	if (dmg > 0 && Role_if(PM_ARCHEOLOGIST) && uamul && uamul->oartifact == ART_ARCHEOLOGIST_SONG) dmg += 2;
	if (dmg > 0 && uarmg && uarmg->oartifact == ART_MADELINE_S_STUPID_GIRL) dmg += 3;
	if (dmg > 0 && ublindf && ublindf->oartifact == ART_EYEHANDER) dmg += 5;
	if (dmg > 0) dmg += (Drunken_boxing && Confusion);
	if (dmg > 0) dmg += (StrongDrunken_boxing && Confusion);
	if (RngeBloodlust && dmg > 0) dmg++;
	if (dmg > 0 && u.boosttimer) dmg += 2;
	if (dmg > 0 && uarms && uarms->oartifact == ART_TEH_BASH_R) dmg += 2;
	if (dmg > 0 && uarmc && uarmc->oartifact == ART_DUFFDUFFDUFF) dmg += 3;
	if (dmg > 0 && uarmg && uarmg->oartifact == ART_RAAAAAAAARRRRRRGH) dmg += 5;
	if (dmg > 0 && uarmg && uarmg->oartifact == ART_SI_OH_WEE) dmg += 2;
	if (dmg > 0 && powerfulimplants() && uimplant && uimplant->oartifact == ART_RHEA_S_MISSING_EYESIGHT) dmg += rnd(5);
	if (dmg > 0 && powerfulimplants() && uimplant && uimplant->oartifact == ART_SOME_LITTLE_AID) dmg += 1;
	if (dmg > 0 && Race_if(PM_VIKING)) dmg += 1;
	if (dmg > 0 && Race_if(PM_SERB)) dmg += 1;
	if (dmg > 0 && Race_if(PM_RUSMOT)) dmg += 2;
	if (dmg > 0 && uarmg && uarmg->oartifact == ART_MAJOR_PRESENCE) dmg += 2;
	if (dmg > 0 && uarmf && uarmf->oartifact == ART_SNAILHUNT) dmg += 1;
	if (dmg > 0 && uarmf && uarmf->oartifact == ART_MAY_BRITT_S_ADULTHOOD) dmg += 1;
	if (dmg > 0 && uarmf && uarmf->oartifact == ART_CRASHING_YOUR_SISTER_S_WED) dmg += 2;
	if (dmg > 0 && uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 0) dmg += 1;
	if (dmg > 0 && uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 49) dmg += 1;
	if (dmg > 0 && uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 99) dmg += 1;
	if (dmg > 0 && uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 149) dmg += 1;
	if (dmg > 0 && uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 199) dmg += 1;
	if (dmg > 0 && uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 249) dmg += 1;
	if (dmg > 0 && bmwride(ART_KERSTIN_S_COWBOY_BOOST)) dmg += 2;
	if (dmg > 0 && Role_if(PM_OTAKU) && uarmc && itemhasappearance(uarmc, APP_FOURCHAN_CLOAK)) dmg += 1;
	if (dmg > 0 && Race_if(PM_RODNEYAN)) dmg += (1 + (GushLevel / 3) );
	if (dmg > 0 && uarmf && uarmf->oartifact == ART_PROPERTY_GRUMBLE) dmg += 8;
	if (dmg > 0 && uarmh && uarmh->oartifact == ART_HABIBA_S_MATRONAGE) dmg += 2;
	if (dmg > 0 && uarmg && uarmg->oartifact == ART_UNKNOWINGNESS_AS_A_WEAPON && !(objects[uarmg->otyp].oc_name_known)) dmg += 5;
	if (dmg > 0 && uwep && uwep->oartifact == ART_BUCK_SHOT && !uwep->bknown) dmg += 2;
	if (dmg > 0 && uwep && uwep->oartifact == ART_FALCO_S_ORB) dmg += 1;
	if (dmg > 0 && uarmg && uarmg->oartifact == ART_PLUS_TO_DAM) dmg += 2;
	if (dmg > 0 && uarm && uarm->oartifact == ART_YOU_ARE_UGLY) dmg += 1;
	if (dmg > 0 && uarm && uarm->oartifact == ART_THERE_GOES_SHE_TO) dmg += 4;
	if (dmg > 0 && uarms && uarms->oartifact == ART_RONDITSCH) dmg += 1;
	if (dmg > 0 && uwep && uwep->oartifact == ART_NOOBY_BONUS_STYLE && !bimanual(uwep)) dmg += 2;
	if (dmg > 0 && powerfulimplants() && uimplant && uimplant->oartifact == ART_NIOBE_S_ANGER) dmg += 2;
	if (dmg > 0 && powerfulimplants() && uimplant && uimplant->oartifact == ART_I_M_GONNA_CRUSH_YA_) dmg += 4;
	if (dmg > 0 && uarms && uarms->oartifact == ART_UNUSUAL_ENCH) dmg += 1;
	if (dmg > 0 && bmwride(ART_ZIN_BA)) dmg += 1;
	if (dmg > 0 && uarm && uarm->oartifact == ART_I_AM_YOUR_FALL) dmg += 2;
	if (dmg > 0 && uarm && uarm->oartifact == ART_ETH_ITH) dmg += 3;
	if (dmg > 0 && uleft && uleft->oartifact == ART_RING_OF_THROR) dmg += 2;
	if (dmg > 0 && uright && uright->oartifact == ART_RING_OF_THROR) dmg += 2;
	if (dmg > 0 && uleft && uleft->oartifact == ART_KRATSCHEM_HARD) dmg += 2;
	if (dmg > 0 && uright && uright->oartifact == ART_KRATSCHEM_HARD) dmg += 2;
	if (dmg > 0 && uleft && uleft->oartifact == ART_WILDFIST) dmg += 4;
	if (dmg > 0 && uright && uright->oartifact == ART_WILDFIST) dmg += 4;
	if (dmg > 0 && uarm && uarm->oartifact == ART_EITHER_INTELLIGENT_OR_FAIR) dmg += 2;
	if (dmg > 0 && uarmg && uarmg->oartifact == ART_DOCHGOGRAP) dmg += 2;

	if (dmg > 0 && uwep && uwep->oartifact == ART_AK_____) {
		if (!PlayerCannotUseSkills) {
			if (P_SKILL(P_FIREARM) < P_BASIC) dmg += 3;
			else if (P_SKILL(P_FIREARM) == P_BASIC) dmg += 2;
			else if (P_SKILL(P_FIREARM) == P_SKILLED) dmg += 1;
		}
		if (P_SKILL(P_FIREARM) == P_MASTER) dmg -= 2;
		if (P_SKILL(P_FIREARM) == P_GRAND_MASTER) dmg -= 4;
		if (P_SKILL(P_FIREARM) == P_SUPREME_MASTER) dmg -= 6;
	}
	if (dmg > 0 && u.twoweap && uswapwep && uswapwep->oartifact == ART_AK_____) {
		if (P_SKILL(P_FIREARM) == P_MASTER) dmg -= 2;
		if (P_SKILL(P_FIREARM) == P_GRAND_MASTER) dmg -= 4;
		if (P_SKILL(P_FIREARM) == P_SUPREME_MASTER) dmg -= 6;
	}

	if (Race_if(PM_ITAQUE)) dmg -= 1;
	if (uwep && uwep->oartifact == ART_RIP_STRATEGY) dmg -= 5;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_RIP_STRATEGY) dmg -= 5;
	if (uwep && uwep->oartifact == ART_KLOBB) dmg -= 6;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_KLOBB) dmg -= 6;
	if (uwep && uwep->oartifact == ART_EXCALIPOOR) dmg -= 9;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_EXCALIPOOR) dmg -= 9;
	if (uwep && uwep->oartifact == ART_VLADSBANE) dmg -= 5;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_VLADSBANE) dmg -= 5;
	if (uwep && uwep->oartifact == ART_CHARGING_MADE_EASY) dmg -= 5;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_CHARGING_MADE_EASY) dmg -= 5;
	if (uwep && uwep->oartifact == ART_BLACK_MARK) dmg -= 1;
	if (uarm && uarm->oartifact == ART_POWASPEL) dmg -= 3;
	if (uarmf && uarmf->oartifact == ART_MY_DEAR_SLAVE) dmg -= 1;

	if (dmg < 0) dmg = 0;	/* beware negative rings of increase damage */
	if (Half_physical_damage && (rn2(2) || (uwep && uwep->oartifact == ART_SOOTHE_)) ) dmg = (dmg + 1) / 2;
	if (StrongHalf_physical_damage && (rn2(2) || (uwep && uwep->oartifact == ART_SOOTHE_)) ) dmg = (dmg + 1) / 2;

	if (uarmh) {
	    if (less_damage && dmg < (Upolyd ? u.mh : u.uhp)) {
		if (!artimsg)
		    pline("Fortunately, you are wearing a hard helmet.");
	    } else if (flags.verbose &&
		    !(obj->otyp == CORPSE && touch_petrifies(&mons[obj->corpsenm])))
		Your("%s does not protect you.", xname(uarmh));
	} else if (obj->otyp == CORPSE && touch_petrifies(&mons[obj->corpsenm])) {
	    if ((!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
		    !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
		petrify:
		/* killer_format = KILLED_BY;
		killer = "elementary physics";	
		You("turn to stone.");
		if (obj) dropy(obj);	
		done(STONING); */
		if (!Stoned) {
			if (Hallucination && rn2(10)) pline("Good thing you are already stoned.");
			else {
				You("start turning to stone.");
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				u.cnd_stoningcount++;
				delayed_killer = "elementary physics";
			}
		}
		return obj ? TRUE : FALSE;
	    }
	}
	hitfloor(obj);
	losehp(dmg, "falling object", KILLED_BY_AN);
    }
    return TRUE;
}

/* return true for weapon meant to be thrown; excludes ammo */
STATIC_OVL boolean
throwing_weapon(obj)
struct obj *obj;
{
	return (is_missile(obj) || is_spear(obj) ||
		/* daggers and knife (excludes scalpel) */
		(is_blade(obj) && !is_sword(obj) &&
		 (objects[obj->otyp].oc_dir & PIERCE)) ||
		/* special cases [might want to add AXE] */
		obj->otyp == WAR_HAMMER || obj->otyp == AKLYS || obj->otyp == BLOW_AKLYS);
}

/* the currently thrown object is returning to you (not for boomerangs) */
STATIC_OVL void
sho_obj_return_to_u(obj)
struct obj *obj;
{
    /* might already be our location (bounced off a wall) */
    if (bhitpos.x != u.ux || bhitpos.y != u.uy) {
	int x = bhitpos.x - u.dx, y = bhitpos.y - u.dy;

	tmp_at(DISP_FLASH, obj_to_glyph(obj));
	while(x != u.ux || y != u.uy) {
	    tmp_at(x, y);
	    delay_output();
	    x -= u.dx; y -= u.dy;
	}
	tmp_at(DISP_END, 0);
    }
}

void
throwit(obj, wep_mask, twoweap, thrown)
struct obj *obj;
long wep_mask;	/* used to re-equip returning boomerang */
boolean twoweap; /* used to restore twoweapon mode if wielded weapon returns */
int thrown;
{
	register struct monst *mon;
	register int range, urange;
	struct obj *launcher = (struct obj*) 0;
	boolean impaired = ( (Confusion && !Conf_resist) || (Stunned && !Stun_resist) || Blind ||
			   Hallucination || Fumbling || Frozen || Burned || Dimmed || Numbed || Feared);

	if (thrown == 1) launcher = uwep;
	else if (thrown == 2) launcher = uswapwep;
	else if (thrown == 666) { /* inbuilt pistol boots */
		if (uarmf && itemhasappearance(uarmf, APP_PISTOL_BOOTS) ) launcher = uarmf;
	}

	boolean misfire = FALSE;
	if (obj->cursed && !(uarmg && uarmg->otyp == GAUNTLETS_OF_TRUE_AIM) && !(uarmg && itemhasappearance(uarmg, APP_SUREFIRE_GLOVES)) ) {
		if (!rn2(7)) misfire = TRUE;
	}
	if (obj->otyp == FLIMSY_DART) {
		if (!rn2(7)) misfire = TRUE;
	}
	if (ACURR(A_DEX) == 1) {
		if (!rn2(7)) misfire = TRUE;
	}
	if (is_grassland(u.ux, u.uy) && !(uarms && uarms->oartifact == ART_FLORENSE_S_GEBLOOMEL) && !(uarm && uarm->oartifact == ART_OBERTURT) && !(uarm && uarm->oartifact == ART_DORL_TSCH) && !(uamul && uamul->oartifact == ART_CONTRO_MOVE) && !(uarmf && itemhasappearance(uarmf, APP_GARDEN_SLIPPERS)) ) {
		if (!rn2(7)) misfire = TRUE;
	}
	if (obj->greased) {
		if (!rn2(7)) misfire = TRUE;
	}
	if (Race_if(PM_PLAYER_SKELETON) && !rn2(3)) {
		if (!rn2(7)) misfire = TRUE;
	}
	if (autismweaponcheck(ART_FOEOEOEOEOEOEOE)) {
		if (!rn2(7)) misfire = TRUE;
	}
	if (uarmg && itemhasappearance(uarmg, APP_CLUMSY_GLOVES) ) {
		if (!rn2(7)) misfire = TRUE;
	}
	if ((obj->oartifact == ART_COMPLETELY_OFF) || (obj->oartifact == ART_FLUSCH) || (obj->oartifact == ART_STREW_ANYWHERE) || u.uprops[PROJECTILES_MISFIRE].extrinsic || ProjectilesMisfire || have_misfirestone() ) misfire = TRUE;

	/* KMH -- Handle Plague here */
	if (launcher && (launcher->oartifact == ART_PLAGUE || launcher->oartifact == ART_BOW_OF_VINDERRE || launcher->oartifact == ART_SHAKING_BOW || launcher->oartifact == ART_BIBLICAL_PLAGUE || launcher->oartifact == ART_BOW_OF_HERCULES) &&
			ammo_and_launcher(obj, launcher) && is_poisonable(obj))
		obj->opoisoned = 1;

	if (launcher && launcher->oartifact == ART_NOZZLE_CHANGE && ammo_and_launcher(obj, launcher))
		obj->opoisoned = 1; /* even if it cannot be poisoned, this is not a bug --Amy */

	if (uarmf && uarmf->oartifact == ART_DOUBTLY_POISON && is_poisonable(obj)) {
		obj->opoisoned = obj->superpoison = 1;
	}

	if (obj && obj->oartifact == ART_TROPICAL_WOOD_SELECTION) obj->opoisoned = 1;
	if (obj && obj->oartifact == ART_DART_OF_THE_ASSASSIN) obj->opoisoned = 1;

	if (launcher && obj && ammo_and_launcher(obj, launcher) && obj->otyp == POISON_BOLT) obj->opoisoned = 1;
	if (launcher && obj && ammo_and_launcher(obj, launcher) && obj->otyp == CHROME_PELLET) obj->opoisoned = 1;

	obj->was_thrown = 1;
	if (misfire && (u.dx || u.dy) ) {
	    boolean slipok = TRUE;
	    if (ammo_and_launcher(obj, launcher))
		pline("%s!", Tobjnam(obj, "misfire"));
	    else {
		/* only slip if it's greased or meant to be thrown */
		/* Amy edit: well, the item is cursed, so it should malfunction no matter what. See monsters throwing
		 * cursed potions, which also causes them to slip. */
		/*if (obj->greased || throwing_weapon(obj))*/
		    /* BUG: this message is grammatically incorrect if obj has
		       a plural name; greased gloves or boots for instance. */
		    pline("%s as you throw it!", Tobjnam(obj, "slip"));
		/*else slipok = FALSE;*/
	    }
	    if (slipok) {
		u.dx = rn2(3)-1;
		u.dy = rn2(3)-1;
		if (!u.dx && !u.dy) u.dz = 1;
		impaired = TRUE;
	    }
	}

	if ((u.dx || u.dy || (u.dz < 1)) &&
	    calc_capacity((int)obj->owt) > SLT_ENCUMBER &&
	    (Upolyd ? (u.mh < 5 && u.mh != u.mhmax)
	     : (u.uhp < 10 && u.uhp != u.uhpmax)) &&
	    obj->owt > (unsigned)((Upolyd ? u.mh : u.uhp) * 2) &&
	    !Is_airlevel(&u.uz)) {
	    You("have so little stamina, %s drops from your grasp.",
		the(xname(obj)));
	    exercise(A_CON, FALSE);
	    u.dx = u.dy = 0;
	    u.dz = 1;
	}

	thrownobj = obj;

	if (launcher && launcher->oartifact == ART_LEONE_M__GUAGE_SUPER) {
		nomul(-2, "suffering from M3 recoil", TRUE);
	}
	if (launcher && launcher->oartifact == ART_MOSIN_NAGANT) {
		nomul(-3, "reloading the Mosin-Nagant", TRUE);
	}
	if (obj && obj->otyp == JUMPING_FLAMER) {
		if (u.rangedreload < 3) u.rangedreload = 3;
	}

	if(u.uswallow) {
		mon = u.ustuck;
		bhitpos.x = mon->mx;
		bhitpos.y = mon->my;
	} else if(u.dz) {
	    if (u.dz < 0 && (Role_if(PM_VALKYRIE) || Role_if(PM_VANILLA_VALK)) &&
		    (obj->oartifact == ART_MJOLLNIR || obj->oartifact == ART_OTHER_MJOLLNIR) && !impaired) {
		pline("%s the %s and returns to your hand!",
		      Tobjnam(obj, "hit"), ceiling(u.ux,u.uy));
		obj = addinv(obj);
		(void) encumber_msg();
		setuwep(obj, TRUE, TRUE);
		u.twoweap = twoweap;
		return;
	    }
	    if (u.dz < 0 && (Role_if(PM_JEDI) || Role_if(PM_SHADOW_JEDI) || Role_if(PM_HEDDERJEDI) || !rn2(2)) &&
		    is_lightsaber(obj) && obj->lamplit && !impaired &&
			!(PlayerCannotUseSkills) && rn2(2) &&
		    P_SKILL(weapon_type(obj)) >= P_SKILLED) {
		pline("%s the %s and returns to your hand!",
		      Tobjnam(obj, "hit"), ceiling(u.ux,u.uy));
		obj = addinv(obj);
		(void) encumber_msg();
		setuwep(obj, TRUE, TRUE);
		u.twoweap = twoweap;
		return;
	    }
	    /* [ALI]
	     * Grenades are armed but are then processed by toss_up/hitfloor
	     * as normal.
	     *
	     * Bullets just disappear with no message.
	     *
	     * Rockets hit the ceiling/floor and explode.
	     */
	    else if (is_grenade(obj)) {
		arm_bomb(obj, TRUE);
		You("yell 'Fire in the hole!'");
	    } else if (obj->oclass == VENOM_CLASS) {
			check_shop_obj(obj, u.ux, u.uy, TRUE);
			obfree(obj, (struct obj *)0);
			return;
	    } else if (is_bullet(obj) && ammo_and_launcher(obj, launcher)) {
		if (!Is_airlevel(&u.uz) && !Is_waterlevel(&u.uz) && !Underwater
			&& (objects[obj->otyp].oc_dir & EXPLOSION)) {
		    pline("%s hit%s the %s and explodes in a ball of fire!",
			    Doname2(obj), (obj->quan == 1L) ? "s" : "",
			    u.dz < 0 ? ceiling(u.ux, u.uy) : surface(u.ux, u.uy));
		    explode(u.ux, u.uy, ZT_SPELL(ZT_FIRE), d(3, 8),
			    WEAPON_CLASS, EXPL_FIERY);

			if (obj && obj->otyp == MINI_NUKE) fatman_explosion(u.ux, u.uy, obj);
		}
		check_shop_obj(obj, u.ux, u.uy, TRUE);
		u.cnd_gunpowderused++; /* even if bulletreuse or lead bullets allows them to be used again --Amy */
		if (((!(tech_inuse(T_BULLETREUSE)) || rn2(3)) && !(Race_if(PM_VIETIS) && !rn2(3)) && !(uarmh && uarmh->oartifact == ART_NIAMH_PENH && !rn2(3)) && !(obj->oartifact == ART_REUSEME && rn2(4)) && !(uarm && uarm->oartifact == ART_MG_RATTERING && (objects[obj->otyp].w_ammotyp == WP_BULLET_MG) ) && !(obj->oartifact == ART_EVERYTHING_GREENS_SO_GREEN) && !(obj->oartifact == ART_COMBAT_PELLET && rn2(10)) && !(objects[obj->otyp].oc_material == MT_LEAD && !rn2(2))) || (uarmf && uarmf->oartifact == ART_ANACONDA_HEELS) ) {
			obfree(obj, (struct obj *)0);
			return;
		}
	    }
	    if (u.dz < 0 && !Is_airlevel(&u.uz) &&
		    !Underwater && !is_crystalwater(u.ux, u.uy) && !Is_waterlevel(&u.uz)) {
		(void) toss_up(obj, rn2(5));
	    } else {
		hitfloor(obj);
	    }
	    thrownobj = (struct obj*)0;
	    return;

	} else if( (obj->otyp == BOOMERANG || obj->otyp == ALU_BOOMERANG || obj->otyp == MITHRIL_BOOMERANG || obj->otyp == SILVER_CHAKRAM || obj->otyp == BATARANG || obj->otyp == DARK_BATARANG) && !Underwater) {
		if(Is_airlevel(&u.uz) || Levitation)
		    hurtle(-u.dx, -u.dy, 1, TRUE);
		mon = boomhit(u.dx, u.dy);
		if(mon == &youmonst) {		/* the thing was caught */
			exercise(A_DEX, TRUE);
			obj = addinv(obj);
			(void) encumber_msg();
			if (wep_mask && !(obj->owornmask & wep_mask)) {
			    setworn(obj, wep_mask);
			    u.twoweap = twoweap;
			}
			thrownobj = (struct obj*)0;
			return;
		}
	} else {
		/* note to self by Amy: urange is not the range of the thrown thing - range is!!! */
		urange = (int)(ACURRSTR)/2;

		/* balls are easy to throw or at least roll */
		/* also, this insures the maximum range of a ball is greater
		 * than 1, so the effects from throwing attached balls are
		 * actually possible
		 */
		if (obj->oclass == BALL_CLASS)
			range = urange - (int)(obj->owt/600); /* thanks to the ball's weight being 2400 now */
		else
			range = urange - (int)(obj->owt/40);
		if (obj == uball) {
			if (u.ustuck) range = 1;
			else if (range >= 5) range = 5;
		}
		if (range < 1) range = 1;

		/* KMH, balance patch -- new macros */
		if (is_ammo(obj)) {
		    if ( (ammo_and_launcher(obj, launcher) && !(launcher && launcher->otyp == LASERXBOW && !launcher->lamplit) && !(launcher && launcher->otyp == KLIUSLING && !launcher->lamplit) ) ) {
			if (is_launcher(launcher) && 
					objects[(launcher->otyp)].oc_range) 
				range = objects[(launcher->otyp)].oc_range;
		    else
			range++;
		    } else if (obj->oclass != GEM_CLASS)
			range /= 2;
		}
		if (thrown == 666) range = 15;

		if (uarmh && uarmh->oartifact == ART_VIRUS_ATTACK) range += 2;
		if (launcher && ammo_and_launcher(obj, launcher) && launcher->otyp == SNIPESLING && obj) range += 5;
		if (launcher && ammo_and_launcher(obj, launcher) && launcher->otyp == BLUE_BOW && obj) range += 1;
		if (launcher && ammo_and_launcher(obj, launcher) && launcher->oartifact == ART_TSCHUEUU && obj) range += 10;
		if (launcher && (ammo_and_launcher(obj, launcher) && !(launcher && launcher->otyp == LASERXBOW && !launcher->lamplit) ) && obj && obj->otyp == ETHER_BOLT) range += 2;
		if (obj && obj->oartifact == ART_LONG_RANGE_BALLISTICS) range += 5;
		if (launcher && ammo_and_launcher(obj, launcher) && obj->oartifact == ART_SNIPESNIPESNIPE) range += 5;
		if (obj && obj->oartifact == ART_WAY_TOO_LONG) range += 10;
		if (launcher && ammo_and_launcher(obj, launcher) && obj->oartifact == ART_SAY__CHESS_) range += 2;
		if (launcher && ammo_and_launcher(obj, launcher) && obj->oartifact == ART_SIDE_GATE) range += 20;
		if (launcher && ammo_and_launcher(obj, launcher) && obj->oartifact == ART_ASA_LEHIBE) range += 8;
		if (launcher && ammo_and_launcher(obj, launcher) && obj->oartifact == ART_MADBALL) range += 6;
		if (launcher && ammo_and_launcher(obj, launcher) && obj->oartifact == ART_SNIPOBLOW) range += 12;
		if (launcher && ammo_and_launcher(obj, launcher) && obj->oartifact == ART_WASGORAE) range += 5;
		if (obj && obj->oartifact == ART_PEWWWWWWW) range += 15;
		if (obj && obj->oartifact == ART_WIUNEW) range += 3;
		if (obj && obj->oartifact == ART_KLUEUEUEU) range += 5;
		if (obj && obj->oartifact == ART_GEHENNA_MODE && ( (Inhell && !Race_if(PM_HERETIC) ) || flags.gehenna ) ) range += 4;
		if (uwep && uwep->oartifact == ART_GEHENNA_MODE && ( (Inhell && !Race_if(PM_HERETIC) ) || flags.gehenna ) ) range += 3;
		if (uarmf && uarmf->oartifact == ART_JONACE_S_TALLNESS) range += 1;

		/* range egotype by Amy: if the ammo has it, firing that ammo gives a bonus; if the launcher has it, then
		 * all ammos fired from that particular launcher get a bonus; these can stack */
		if (obj && obj->enchantment == WEAPEGO_RANGE1) range += 2;
		if (obj && obj->enchantment == WEAPEGO_RANGE2) range += 5;
		if (obj && obj->enchantment == WEAPEGO_RANGE3) range += 8;
		if (obj && obj->enchantment == WEAPEGO_RANGE4) range += 12;
		if (launcher && ammo_and_launcher(obj, launcher) && launcher->enchantment == WEAPEGO_RANGE1) range += 2;
		if (launcher && ammo_and_launcher(obj, launcher) && launcher->enchantment == WEAPEGO_RANGE2) range += 5;
		if (launcher && ammo_and_launcher(obj, launcher) && launcher->enchantment == WEAPEGO_RANGE3) range += 8;
		if (launcher && ammo_and_launcher(obj, launcher) && launcher->enchantment == WEAPEGO_RANGE4) range += 12;

		if (!PlayerCannotUseSkills && launcher && ammo_and_launcher(obj, launcher) && launcher->otyp == KLIUSLING && launcher->lamplit) {
			if (u.kliuskill >= 20) range++;
			if (u.kliuskill >= 160) range++;
			if (u.kliuskill >= 540) range++;
			if (u.kliuskill >= 1280) range++;
			if (u.kliuskill >= 2500) range++;
			if (u.kliuskill >= 4320) range++;
		}

		if (!PlayerCannotUseSkills && obj && obj->oartifact == ART_WEDIFORCE) {
			switch (P_SKILL(P_WEDI)) {
				case P_BASIC:	range += 1; break;
				case P_SKILLED:	range += 2; break;
				case P_EXPERT:	range += 3; break;
				case P_MASTER:	range += 4; break;
				case P_GRAND_MASTER:	range += 5; break;
				case P_SUPREME_MASTER:	range += 6; break;
				default: break;
			}
		}

		if (Race_if(PM_ENGCHIP) && launcher && objects[launcher->otyp].oc_skill == P_BOW) range += 2;
		if (Race_if(PM_ENGCHIP) && launcher && objects[launcher->otyp].oc_skill == P_CROSSBOW) range += 2;
		if (Race_if(PM_KORONST) && launcher && objects[launcher->otyp].oc_skill == P_SLING) range += 2;

		if (uarmg && uarmg->oartifact == ART_BEEEEEEEANPOLE && launcher && objects[launcher->otyp].oc_skill == P_BOW) range += 5;
		if (uwep && uwep->oartifact == ART_SNIPER_CROSSHAIR && launcher && objects[launcher->otyp].oc_skill == P_CROSSBOW) range += 30;
		if ((uarmc && itemhasappearance(uarmc, APP_CYANISM_CLOAK)) && launcher && objects[launcher->otyp].oc_skill == P_SLING) range += 3;
		if (launcher && launcher->oartifact == ART_ITALY_SI_ES) range += 4;
		if (launcher && launcher->oartifact == ART_MISS_LAUNCHER) range += 2;
		if (launcher && launcher->oartifact == ART_ACTUALLY_USABLE_GL) range += 6;
		if (obj && obj->oartifact == ART_MEGATON_LOAD) range += 10;
		if (obj && obj->oartifact == ART_A_MILE_AND_A_HALF) range += 25;
		if (obj && obj->oartifact == ART_LONG_MILE) range += 5;
		if (obj && obj->oartifact == ART_FLAI_AWEI) range += 10;
		if (obj && obj->oartifact == ART_NINJINGY) range += 2;
		if (obj && obj->oartifact == ART_FTS) range += 10;
		if (obj && obj->oartifact == ART_WASHINGTON_S_CAPPER) range += 5;
		if (uarmu && uarmu->oartifact == ART_NOW_YOU_MADE_HER_SAD) range += 2;

		if (obj && obj->oartifact == ART_RACER_PROJECTILE) range *= 2;

		if (launcher && launcher->oartifact == ART_ENEMY_DEAD_AT_CLOSE_RANGE) range--;

		if (Race_if(PM_GERTEUT) && range > 5) range = 5;
		if (Race_if(PM_PERVERT) && range > 2) range = 2;
		if (launcher && launcher->otyp == SHOVEL && range > 4) range = 4;
		if (launcher && launcher->oartifact == ART_OZYZEVPDWTVP) range -= 2;

		if (range < 1) range = 1; /* fail safe */

		if (Is_airlevel(&u.uz) || Levitation) {
		    /* action, reaction... */
		    urange -= range;
		    if(urange < 1) urange = 1;
		    range -= urange;
		    if(range < 1) range = 1;
		}

		if (obj->otyp == BOULDER)
		    range = 20;		/* you must be giant */
		else if (obj->oartifact == ART_MJOLLNIR || obj->oartifact == ART_OTHER_MJOLLNIR)
		    range = (range + 1) / 2;	/* it's heavy */
		else if (obj == uball && u.utrap && u.utraptype == TT_INFLOOR)
		    range = 1;

		if (Underwater) range = 1;

		if (Race_if(PM_GERTEUT) && range > 5) range = 5;
		if (Race_if(PM_PERVERT) && range > 2) range = 2;

		if (range < 1) range = 1; /* fail safe */

		mon = bhit(u.dx,u.dy,range,THROWN_WEAPON,
			   (int (*)(MONST_P,OBJ_P))0,
			   (int (*)(OBJ_P,OBJ_P))0,
			   &obj, TRUE);

		/* have to do this after bhit() so u.ux & u.uy are correct */
		if(Is_airlevel(&u.uz) || Levitation)
		    hurtle(-u.dx, -u.dy, urange, TRUE);

		if (!obj) {
		    thrownobj = (struct obj *)0;
		    return;
		}
	}

	if(mon) {
		boolean obj_gone;

		if (mon->isshk &&
		    obj->where == OBJ_MINVENT && obj->ocarry == mon) {
		    thrownobj = (struct obj*)0;
		    return;		/* alert shk caught it */
		}
		(void) snuff_candle(obj);
		notonhead = (bhitpos.x != mon->mx || bhitpos.y != mon->my);
		obj_gone = thitmonst(mon, obj, thrown, FALSE);
		/* Monster may have been tamed; this frees old mon */
		mon = m_at(bhitpos.x, bhitpos.y);

		/* [perhaps this should be moved into thitmonst or hmon] */
		if (mon && mon->isshk &&
			(!inside_shop(u.ux, u.uy) ||
			 !index(in_rooms(mon->mx, mon->my, SHOPBASE), *u.ushops)))
		    hot_pursuit(mon);

		if (obj_gone) return;
	}

	/* Handle grenades or rockets */
	if (is_grenade(obj)) {
	    arm_bomb(obj, TRUE);
	    You("yell 'Fire in the hole!'");
	} else if (ammo_and_launcher(obj, launcher) &&
		(objects[obj->otyp].oc_dir & EXPLOSION)) {
	    if (cansee(bhitpos.x,bhitpos.y)) 
		pline("%s explodes in a ball of fire!", Doname2(obj));
	    else You_hear("an explosion");
	    explode(bhitpos.x, bhitpos.y, ZT_SPELL(ZT_FIRE),
		    d(3,8), WEAPON_CLASS, EXPL_FIERY);

		if (obj && obj->otyp == MINI_NUKE) fatman_explosion(bhitpos.x, bhitpos.y, obj);
	}

	if (obj->oclass == VENOM_CLASS) {
		check_shop_obj(obj, bhitpos.x,bhitpos.y, TRUE);
		obfree(obj, (struct obj *)0);
		return;
	}

	if (is_bullet(obj) && (ammo_and_launcher(obj, launcher) && !is_grenade(obj))) {
		check_shop_obj(obj, bhitpos.x,bhitpos.y, TRUE);
		u.cnd_gunpowderused++; /* even if bulletreuse or lead bullets allows them to be used again --Amy */
		if (((!(tech_inuse(T_BULLETREUSE)) || rn2(3)) && !(Race_if(PM_VIETIS) && !rn2(3)) && !(uarmh && uarmh->oartifact == ART_NIAMH_PENH && !rn2(3)) && !(obj->oartifact == ART_REUSEME && rn2(4)) && !(uarm && uarm->oartifact == ART_MG_RATTERING && (objects[obj->otyp].w_ammotyp == WP_BULLET_MG) ) && !(obj->oartifact == ART_EVERYTHING_GREENS_SO_GREEN) && !(obj->oartifact == ART_COMBAT_PELLET && rn2(10)) && !(objects[obj->otyp].oc_material == MT_LEAD && !rn2(2))) || (uarmf && uarmf->oartifact == ART_ANACONDA_HEELS) ) {
			obfree(obj, (struct obj *)0);
			return;
		}
	}

	int djemsochance = 0;

	if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_DJEM_SO)) {

			case P_BASIC:	djemsochance =  1; break;
			case P_SKILLED:	djemsochance =  2; break;
			case P_EXPERT:	djemsochance =  4; break;
			case P_MASTER:	djemsochance =  6; break;
			case P_GRAND_MASTER:	djemsochance =  8; break;
			case P_SUPREME_MASTER:	djemsochance =  9; break;
			default: djemsochance = 0; break;
		}
	}

	/* boomerang can come back, idea from dnethack, implementation by Amy (chance is less than 100%) */
	int boomerangchance = 20;

	if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_BOOMERANG)) {

			case P_BASIC:	boomerangchance = 30; break;
			case P_SKILLED:	boomerangchance = 35; break;
			case P_EXPERT:	boomerangchance = 40; break;
			case P_MASTER:	boomerangchance = 50; break;
			case P_GRAND_MASTER:	boomerangchance = 60; break;
			case P_SUPREME_MASTER:	boomerangchance = 70; break;
			default: boomerangchance = 20; break;
		}

		switch (P_SKILL(P_DJEM_SO)) {

			case P_BASIC:	boomerangchance +=  2; break;
			case P_SKILLED:	boomerangchance +=  4; break;
			case P_EXPERT:	boomerangchance +=  6; break;
			case P_MASTER:	boomerangchance +=  8; break;
			case P_GRAND_MASTER:	boomerangchance +=  10; break;
			case P_SUPREME_MASTER:	boomerangchance +=  12; break;
			default: break;
		}

		if (boomerangchance > 90) boomerangchance = 90; /* shouldn't happen */

		if (Race_if(PM_BATMAN)) { /* his batarang usually comes back --Amy */
			boomerangchance += ((100 - boomerangchance) / 2);
		}
	}

	if (u.uswallow) {
		/* ball is not picked up by monster */
		if (obj != uball) (void) mpickobj(u.ustuck,obj,FALSE);
	} else {
		/* the code following might become part of dropy() */
		if (
			(obj->oartifact == ART_MJOLLNIR && Role_if(PM_VALKYRIE) && rn2(100)) ||
			(obj->oartifact == ART_OTHER_MJOLLNIR && Role_if(PM_VALKYRIE) && rn2(100)) ||
			(obj->oartifact == ART_MJOLLNIR && Role_if(PM_VANILLA_VALK) && rn2(100)) ||
			(obj->oartifact == ART_OTHER_MJOLLNIR && Role_if(PM_VANILLA_VALK) && rn2(100)) ||

			(is_lightsaber(obj) && (obj->lamplit || Role_if(PM_SHADOW_JEDI)) && (rn2(2) || (djemsochance >= rn2(11)) ) &&
			( ( (Role_if(PM_JEDI) || Role_if(PM_SHADOW_JEDI) || Role_if(PM_HEDDERJEDI)) && P_SKILL(weapon_type(obj)) > P_SKILLED) || (!rn2(2) && P_SKILL(weapon_type(obj)) > P_SKILLED) || (djemsochance >= rn2(11)) ) ) ||

			((objects[obj->otyp].oc_skill == P_BOOMERANG || objects[obj->otyp].oc_skill == -P_BOOMERANG) &&
			((boomerangchance > rn2(100)) || (obj->oartifact && !rn2(3)) ) )

			) {

			boolean boomerfix = (objects[obj->otyp].oc_skill == P_BOOMERANG || objects[obj->otyp].oc_skill == -P_BOOMERANG);

		    /* we must be wearing Gauntlets of Power to get here */
		    /* or a Jedi with a lightsaber or a thrown boomerang */
		    if ( (Role_if(PM_JEDI) || Role_if(PM_SHADOW_JEDI) || Role_if(PM_HEDDERJEDI)) && u.uen < 5){
			You("don't have enough force to call %s. You need at least 5 points of mana!", the(xname(obj)));
		    } else {
		      if (Role_if(PM_JEDI) || Role_if(PM_SHADOW_JEDI) || Role_if(PM_HEDDERJEDI))
			u.uen -= 5;
			if (!boomerfix) sho_obj_return_to_u(obj);	    /* display its flight */

			/* djem so just trains so damn slowly... so here's an improvement --Amy */
			if (is_lightsaber(obj)) {
				use_skill(P_DJEM_SO, 1);
				if (obj->otyp == PINK_LIGHTSWORD || obj->otyp == PINK_DOUBLE_LIGHTSWORD) use_skill(P_DJEM_SO, 1);
				if (obj->oartifact == ART_ROSH_TRAINOR) use_skill(P_DJEM_SO, 1);
			}

		    if (!impaired && rn2(100)) {
			pline("%s to your hand!", Tobjnam(obj, "return"));
			if (is_lightsaber(obj)) mightbooststat(A_DEX);
			obj = addinv(obj);
			(void) encumber_msg();
			if (!boomerfix) {
				setuwep(obj, TRUE, TRUE);
				u.twoweap = twoweap;
			} else {
				setuqwep(obj);
			}
			if(cansee(bhitpos.x, bhitpos.y))
			    newsym(bhitpos.x,bhitpos.y);
		    } else {
			int dmg = rn2(2);
			if (!dmg) {
			    pline(Blind ? "%s lands %s your %s." :
					"%s back to you, landing %s your %s.",
				  Blind ? Something : Tobjnam(obj, "return"),
				  Levitation ? "beneath" : "at",
				  makeplural(body_part(FOOT)));
			} else {
			    dmg += rnd(3);
			    pline(Blind ? "%s your %s!" :
					"%s back toward you, hitting your %s!",
				  Tobjnam(obj, Blind ? "hit" : "fly"),
				  body_part(ARM));
			    (void) artifact_hit((struct monst *)0,
						&youmonst, obj, &dmg, 0);
			    losehp(dmg, xname(obj),
				obj_is_pname(obj) ? KILLED_BY : KILLED_BY_AN);
			}
			if (ship_object(obj, u.ux, u.uy, FALSE)) {
		    	    thrownobj = (struct obj*)0;
			    return;
			}
			dropy(obj);
		    }
		    thrownobj = (struct obj*)0;
		    return;
		    }
		}

		if (!IS_SOFT(levl[bhitpos.x][bhitpos.y].typ) &&
			breaktest(obj)) {
		    tmp_at(DISP_FLASH, obj_to_glyph(obj));
		    tmp_at(bhitpos.x, bhitpos.y);
		    delay_output();
		    tmp_at(DISP_END, 0);
		    breakmsg(obj, cansee(bhitpos.x, bhitpos.y));
			if (issegfaulter && obj->otyp == SEGFAULT_VENOM && !rn2(5)) { /* segfault panic! */
				u.segfaultpanic = TRUE;
			} else if (obj->oartifact == ART_DO_NOT_THROW_ME) { /* uh-oh... you really messed up big time there. */
				u.segfaultpanic = TRUE;
			}
		    breakobj(obj, bhitpos.x, bhitpos.y, TRUE, TRUE);
		    return;
		}
		if(flooreffects(obj,bhitpos.x,bhitpos.y,"fall")) return;
		obj_no_longer_held(obj);
		if (mon && mon->isshk && is_pick(obj)) {
		    if (cansee(bhitpos.x, bhitpos.y))
			pline("%s snatches up %s.",
			      Monnam(mon), the(xname(obj)));
		    if(*u.ushops)
			check_shop_obj(obj, bhitpos.x, bhitpos.y, FALSE);
		    (void) mpickobj(mon, obj, FALSE);	/* may merge and free obj */
		    thrownobj = (struct obj*)0;
		    return;
		}
		(void) snuff_candle(obj);
		if (!mon && ship_object(obj, bhitpos.x, bhitpos.y, FALSE)) {
		    thrownobj = (struct obj*)0;
		    return;
		}
		thrownobj = (struct obj*)0;
		place_object(obj, bhitpos.x, bhitpos.y);
		if(*u.ushops && obj != uball)
		    check_shop_obj(obj, bhitpos.x, bhitpos.y, FALSE);

		stackobj(obj);
		if (obj == uball)
		    drop_ball(bhitpos.x, bhitpos.y);
		if (cansee(bhitpos.x, bhitpos.y))
		    newsym(bhitpos.x,bhitpos.y);
		if (obj_sheds_light(obj))
		    vision_full_recalc = 1;
		if (!IS_SOFT(levl[bhitpos.x][bhitpos.y].typ))
		    container_impact_dmg(obj);
	}
}

/* an object may hit a monster; various factors adjust the chance of hitting */
int
omon_adj(mon, obj, mon_notices)
struct monst *mon;
struct obj *obj;
boolean mon_notices;
{
	int tmp = 0;

	/* size of target affects the chance of hitting */
	tmp += (mon->data->msize - MZ_MEDIUM);		/* -2..+5 */
	/* sleeping target is more likely to be hit */
	if (mon->msleeping) {
	    tmp += 2;
	    if (mon_notices) mon->msleeping = 0;
	}
	/* ditto for immobilized target */
	if (!mon->mcanmove || !mon->data->mmove) {
	    tmp += 4;
	    if (mon_notices && mon->data->mmove && !rn2(mon->masleep ? 3 : 10)) {
		mon->mcanmove = 1;
		mon->masleep = 0;
		mon->mfrozen = 0;
	    }
	}
	/* some objects are more likely to hit than others */
	switch (obj->otyp) {
	case HEAVY_IRON_BALL:
	    if (obj != uball) tmp += 2;
	    break;
	case BOULDER:
	    tmp += 6;
	    if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_BOULDER_THROWING)) {
			default: break;
			case P_BASIC: tmp += 2; break;
			case P_SKILLED: tmp += 4; break;
			case P_EXPERT: tmp += 6; break;
			case P_MASTER: tmp += 8; break;
			case P_GRAND_MASTER: tmp += 10; break;
			case P_SUPREME_MASTER: tmp += 12; break;

		}
	    }
	    break;
	default:
	    if (obj->oclass == WEAPON_CLASS || obj->oclass == BALL_CLASS || obj->oclass == CHAIN_CLASS || obj->oclass == VENOM_CLASS || is_weptool(obj) || obj->oclass == GEM_CLASS)
		tmp += hitval(obj, mon);
	    break;
	}
	return tmp;
}

/* thrown object misses target monster */
STATIC_OVL void
tmiss(obj, mon)
struct obj *obj;
struct monst *mon;
{
    const char *missile = mshot_xname(obj);

    /* If the target can't be seen or doesn't look like a valid target,
       avoid "the arrow misses it," or worse, "the arrows misses the mimic."
       An attentive player will still notice that this is different from
       an arrow just landing short of any target (no message in that case),
       so will realize that there is a valid target here anyway. */
    if (!canseemon(mon) || (mon->m_ap_type && mon->m_ap_type != M_AP_MONSTER))
	pline("%s %s.", The(missile), otense(obj, "miss"));
    else
	miss(missile, mon);
    if (!rn2(3)) wakeup(mon);

    if ( (autismweaponcheck(ART_CANNONDANCER)) && multi >= 0) {
		if (isstunfish) nomul(-(rnz(10)), "having missed the beat", TRUE);
		else nomul(-(rn1(5,5)), "having missed the beat", TRUE);

    }

    return;
}

#define quest_arti_hits_leader(obj,mon)	\
  (obj->oartifact && is_quest_artifact(obj) && (mon->data->msound == MS_LEADER))

/*
 * Object thrown by player arrives at monster's location.
 * Return 1 if obj has disappeared or otherwise been taken care of,
 * 0 if caller must take care of it.
 */
int
thitmonst(mon, obj, thrown, polearming)
register struct monst *mon;
register struct obj   *obj;
int thrown;
boolean polearming;
{
	register int	tmp; /* Base chance to hit */
	register int	disttmp; /* distance modifier */
	struct obj *launcher;
	register struct obj *blocker = (struct obj *)0;
	boolean doubleshot = FALSE;	/* are you dual-wielding and firing with the secondary weapon as well? */

	int skillpierce; /* with high skill, you can hit monsters that would normally have dodged --Amy */

	int otyp = obj->otyp;
	boolean guaranteed_hit = (u.uswallow && mon == u.ustuck && rn2(3));
	int dieroll = rnd(20);

	boolean stupidrock = 0;
	if (obj->otyp == ROCK) stupidrock = 1;

	boolean bulletate = 0;
	if (objects[obj->otyp].oc_skill == P_FIREARM || objects[obj->otyp].oc_skill == -P_FIREARM) bulletate = 1;

	boolean pieks = 0;
	if (objects[obj->otyp].oc_skill == P_POLEARMS) pieks = 1;
	if (objects[obj->otyp].oc_skill == P_LANCE) pieks = 1;
	if (obj->otyp == GRAPPLING_HOOK) pieks = 1;
	if (obj->otyp == LAJATANG) pieks = 1;
	if (obj->otyp == JACK_KNIFE) pieks = 1;
	boolean stopevading = 0;
	if (obj->oartifact == ART_STOP_EVADING_ME) stopevading = 1;

	register int shieldblockrate = 0;

	/* Differences from melee weapons:
	 *
	 * Dex still gives a bonus, but strength does not.
	 * Polymorphed players lacking attacks may still throw.
	 * There's a base -1 to hit.
	 * No bonuses for fleeing or stunned targets (they don't dodge
	 *    melee blows as readily, but dodging arrows is hard anyway).
	 * Not affected by traps, etc.
	 * Certain items which don't in themselves do damage ignore tmp.
	 * Distance and monster size affect chance to hit.
	 */
	/* KMH -- added ring of increase accuracy */

	if (thrown == 1) launcher = uwep;
	else if (thrown == 2) launcher = uswapwep;
	else if (thrown == 666) { /* inbuilt pistol boots */
		if (uarmf && itemhasappearance(uarmf, APP_PISTOL_BOOTS) ) launcher = uarmf;
	}
	else launcher = (struct obj *)0;

	if (u.twoweap && uswapwep && launcher && (uswapwep == launcher)) doubleshot = TRUE;

	boolean gunused = 0;
	if (launcher && ammo_and_launcher(obj, launcher) && objects[launcher->otyp].oc_skill == P_FIREARM) gunused = 1;

	tmp = -1 + ( (!rn2(3) && Luck > 0) ? rnd(Luck) : Luck) + find_mac(mon) + ((increase_accuracy_bonus_value() > 1) ? rnd(increase_accuracy_bonus_value()) : increase_accuracy_bonus_value()) +
			(!rn2(3) ? (maybe_polyd(rnd(youmonst.data->mlevel + 1), rnd(GushLevel))) : (maybe_polyd(youmonst.data->mlevel + 1, GushLevel)) );

	/* early-game bonuses to make starting characters not suck too badly --Amy */
	if (u.ulevel < 6) tmp += 1;
	if (u.ulevel < 4) tmp += 1; /* because abon() somehow doesn't get factored in */
	if (u.ulevel < 2) tmp += 1;
	if (u.ulevel < 5 && rn2(2)) tmp += 1;
	if (u.ulevel < 3 && rn2(2)) tmp += 1;

	if (GushLevel > 5) tmp += 1;
	if (GushLevel > 9) tmp += 1;
	if (GushLevel > 12) tmp += 1;
	if (GushLevel > 15) tmp += 1;
	if (GushLevel > 19) tmp += 1;
	if (GushLevel > 23) tmp += 1;
	if (GushLevel > 26) tmp += 1;
	if (GushLevel > 29) tmp += 1;

	if (!issoviet && !rn2(3)) tmp += rno(GushLevel);

	if (uarmh && uarmh->oartifact == ART_REMOTE_GAMBLE) tmp += 2;
	if (uarm && uarm->oartifact == ART_MOTHERFUCKER_TROPHY) tmp += 5;
	if (u.tiksrvzllatdown) tmp += 5;

	if (Race_if(PM_GERTEUT)) tmp += 5;

	if (obj && obj->otyp == LASER_FLYAXE && obj->lamplit) tmp += 5;
	if (obj && obj->otyp == DISKOS) tmp += 5;
	if (obj && obj->otyp == TOMAHAWK) tmp += 5;
	if (obj && obj->oartifact == ART_LONG_RANGE_BALLISTICS) tmp += 10;

	if (uarmh && uarmh->oartifact == ART_B_A_L_L_A_S && gunused) tmp += 3;

	if (uarmf && uarmf->oartifact == ART_WE_ARE__TRANNIES && gunused) tmp += rnd(10);

	if (uarmg && itemhasappearance(uarmg, APP_UNCANNY_GLOVES)) tmp += 1;
	if (uarmg && itemhasappearance(uarmg, APP_SLAYING_GLOVES)) tmp += 1;
	if (uarmg && itemhasappearance(uarmg, APP_SUREFIRE_GLOVES)) tmp += 2;
	if (uarmg && uarmg->otyp == GAUNTLETS_OF_TRUE_AIM) {
		tmp += 2;
		if (uarmg->spe > 0) tmp += uarmg->spe;
	}

	if (uarmh && uarmh->oartifact == ART_IRON_HELM_OF_GORLIM) tmp += 10;
	if (uarmc && uarmc->oartifact == ART_FEND_FOR_YOURSELF) tmp += 7;
	if (uamul && uamul->oartifact == ART_PLAYING_QUAKE) tmp += 5;
	if (uwep && uwep->oartifact == ART_DARKGOD_S_MINUSES) tmp -= 6;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_DARKGOD_S_MINUSES) tmp -= 6;
	if (uarmh && uarmh->oartifact == ART_SUDUNSEL) tmp += 2;
	if (uarmc && uarmc->oartifact == ART_SELVERFEND) tmp += 1;
	if (uarm && uarm->otyp == DARK_DRAGON_SCALES) tmp += 1;
	if (uarm && uarm->otyp == DARK_DRAGON_SCALE_MAIL) tmp += 1;
	if (uarms && uarms->otyp == DARK_DRAGON_SCALE_SHIELD) tmp += 1;
	if (uarmg && uarmg->oartifact == ART_FLOEMMELFLOEMMELFLOEMMELFL) tmp += 1;
	if (uarmf && uarmf->oartifact == ART_MELISSA_S_BEAUTY) tmp += 5;
	if (uarmf && uarmf->oartifact == ART_MAY_BRITT_S_ADULTHOOD) tmp -= 2;
	if (uwep && uwep->oartifact == ART_WILD_HEAVY_SWINGS) tmp -= 10;
	if (uwep && uwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON) tmp += 1;
	if (uarmc && uarmc->oartifact == ART_ENEMIES_SHALL_LAUGH_TOO) tmp += 10;
	if (uimplant && uimplant->oartifact == ART_ACTUAL_PRECISION) tmp += 5;
	if (uarm && uarm->oartifact == ART_ARMOR_PIERCE) tmp += 50;
	if (Flying && u.usteed && uwep && uwep->oartifact == ART_AIRCRAFT_MISSILE) tmp += 5;
	if (uimplant && uimplant->oartifact == ART_RHEA_S_MISSING_EYESIGHT) tmp -= rnd(20);
	if (uwep && uwep->oartifact == ART_SIGIX_BROADSWORD) tmp -= 5;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_SIGIX_BROADSWORD) tmp -= 5;
	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_ACTUAL_PRECISION) tmp += 5;
	if (uleft && uleft->oartifact == ART_BLIND_PILOT) tmp -= 10;
	if (uright && uright->oartifact == ART_BLIND_PILOT) tmp -= 10;
	if (uleft && uleft->oartifact == ART_SHL_THEME) tmp += 2;
	if (uright && uright->oartifact == ART_SHL_THEME) tmp += 2;
	if (uarmf && uarmf->oartifact == ART_JONACE_S_TALLNESS) tmp += 5;
	if (uleft && uleft->oartifact == ART_ACTUAL_CAVE_DWELLING) tmp += 2;
	if (uright && uright->oartifact == ART_ACTUAL_CAVE_DWELLING) tmp += 2;
	if (uleft && uleft->oartifact == ART_HEXTROSE) tmp += 6;
	if (uarm && uarm->oartifact == ART_PURPLE_SOCKET) tmp += 5;
	if (uright && uright->oartifact == ART_HEXTROSE) tmp += 6;
	if (Role_if(PM_ARCHEOLOGIST) && uamul && uamul->oartifact == ART_ARCHEOLOGIST_SONG) tmp += 2;
	if (u.boosttimer) tmp += 5;
	if (ublindf && ublindf->oartifact == ART_EYEHANDER) tmp += 5;
	if (uarmg && uarmg->oartifact == ART_SOFT_TO_THE_TOUCH) tmp += 5;
	if (ublindf && ublindf->oartifact == ART_MEANINGFUL_CHALLENGE) tmp += 2;
	if (uwep && uwep->oartifact == ART_JUSTICE_FOR_GARLIC) tmp += 5;
	if (uwep && uwep->oartifact == ART_ATOMIC_MISSING) tmp -= 20;
	if (uarmg && uarmg->oartifact == ART_SI_OH_WEE) tmp += 2;
	if (uimplant && uimplant->oartifact == ART_SOME_LITTLE_AID) tmp += 1;
	if (uwep && uwep->oartifact == ART_RIP_STRATEGY) tmp -= 5;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_RIP_STRATEGY) tmp -= 5;
	if (uarmf && uarmf->oartifact == ART_CRASHING_YOUR_SISTER_S_WED) tmp -= 5;
	if (uarmf && uarmf->oartifact == ART_CAT_ROCKZ) tmp += 5;
	if (uamul && uamul->oartifact == ART_ISRA_S_ANGLE) tmp += 10;
	if (Race_if(PM_SERB)) tmp += 1;
	if (uarmg && uarmg->oartifact == ART_MAJOR_PRESENCE) tmp += 2;
	if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 0) tmp += 1;
	if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 49) tmp += 1;
	if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 99) tmp += 1;
	if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 149) tmp += 1;
	if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 199) tmp += 1;
	if (uwep && uwep->oartifact == ART_SINSWORD && u.ualign.record < 249) tmp += 1;
	if (StrongBlind_resistance) tmp += rn1(5, 5);
	if (uarmh && uarmh->oartifact == ART_WAITING_FOR_MELEE) tmp -= 2;
	if (bmwride(ART_KERSTIN_S_COWBOY_BOOST)) tmp += 2;
	if (uwep && uwep->oartifact == ART_VLADSBANE) tmp -= 5;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_VLADSBANE) tmp -= 5;
	if (uwep && uwep->oartifact == ART_CHARGING_MADE_EASY) tmp -= 5;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_CHARGING_MADE_EASY) tmp -= 5;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_DUAL_MASTERY) tmp += 5;
	if (uwep && uwep->oartifact == ART_SPINESHOOTER) tmp += 5;
	if (uarmf && uarmf->oartifact == ART_PROPERTY_GRUMBLE) tmp -= 5;
	if (uarmg && uarmg->oartifact == ART_UNKNOWINGNESS_AS_A_WEAPON && !(objects[uarmg->otyp].oc_name_known)) tmp += 5;
	if (uwep && uwep->oartifact == ART_FALCO_S_ORB) tmp += 1;
	if (uwep && uwep->oartifact == ART_VERY_SPECIFICNESS) tmp += 1;
	if (uarmg && uarmg->oartifact == ART_PLUS_TO_HIT) tmp += 5;
	if (uarmh && uarmh->oartifact == ART_BE_THE_LITE) tmp += 1;
	if (uarms && uarms->oartifact == ART_RONDITSCH) tmp += 1;
	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_NIOBE_S_ANGER) tmp += 4;
	if (obj && objects[obj->otyp].oc_material == MT_ADAMANTIUM) tmp += 2;
	if (uimplant && uimplant->oartifact == ART_I_M_GONNA_CRUSH_YA_) tmp += 4;
	if (bmwride(ART_ZIN_BA)) tmp += 4;
	if (obj && obj->oartifact == ART_BLOHIT) tmp += 10;
	if (obj && obj->oartifact == ART_KLUEUEUEU) tmp += 1000;
	if (uarm && uarm->oartifact == ART_I_AM_YOUR_FALL) tmp += 10;
	if (uarmg && uarmg->oartifact == ART_GET_THE_OLD_VALUES_BACK) tmp += 3;
	if (uleft && uleft->oartifact == ART_CERBERUS_BAND) tmp += 3;
	if (uright && uright->oartifact == ART_CERBERUS_BAND) tmp += 3;
	if (uleft && uleft->oartifact == ART_CHERRYTAPPER) tmp += 10;
	if (uright && uright->oartifact == ART_CHERRYTAPPER) tmp += 10;
	if (uarmc && uarmc->oartifact == ART_ISHITA_S_OVERWHELMING) tmp += 5;
	if (uarmg && uarmg->oartifact == ART_DOCHGOGRAP) tmp += 4;
	if (u.ulevel >= 8) tmp++;
	if (u.ulevel >= 16) tmp++;
	if (u.ulevel >= 24) tmp++;
	if (u.ulevel >= 30) tmp++;

	if (uleft && uleft->oartifact == ART_KRATSCHEM_HARD) tmp += 3;
	if (uright && uright->oartifact == ART_KRATSCHEM_HARD) tmp += 3;

	if (PlayerInWedgeHeels && !PlayerCannotUseSkills) {
		switch (P_SKILL(P_WEDGE_HEELS)) {
			default: break;
			case P_BASIC: tmp += 1; break;
			case P_SKILLED: tmp += 2; break;
			case P_EXPERT: tmp += 3; break;
			case P_MASTER: tmp += 4; break;
			case P_GRAND_MASTER: tmp += 5; break;
			case P_SUPREME_MASTER: tmp += 6; break;
		}
	}

	if (uwep && uwep->oartifact == ART_AK_____) {
		if (!PlayerCannotUseSkills) {
			if (P_SKILL(P_FIREARM) < P_BASIC) tmp += 6;
			else if (P_SKILL(P_FIREARM) == P_BASIC) tmp += 4;
			else if (P_SKILL(P_FIREARM) == P_SKILLED) tmp += 2;
		}
		if (P_SKILL(P_FIREARM) == P_MASTER) tmp -= 2;
		if (P_SKILL(P_FIREARM) == P_GRAND_MASTER) tmp -= 4;
		if (P_SKILL(P_FIREARM) == P_SUPREME_MASTER) tmp -= 6;
	}
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_AK_____) {
		if (P_SKILL(P_FIREARM) == P_MASTER) tmp -= 2;
		if (P_SKILL(P_FIREARM) == P_GRAND_MASTER) tmp -= 4;
		if (P_SKILL(P_FIREARM) == P_SUPREME_MASTER) tmp -= 6;
	}

	if (u.martialstyle == MARTIALSTYLE_KARATE) tmp -= 5;
	if (obj && obj->oartifact == ART_WASHINGTON_S_CAPPER) tmp -= 5;
	if (uwep && uwep->oartifact == ART_LONGLOSS) tmp -= rnd(10);
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_LONGLOSS) tmp -= rnd(10);
	if (uwep && uwep->oartifact == ART_ENEMY_DEAD_AT_CLOSE_RANGE) tmp -= 10;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ENEMY_DEAD_AT_CLOSE_RANGE) tmp -= 10;
	if (uwep && uwep->oartifact == ART_ITALY_SI_ES) tmp -= rnd(25);
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ITALY_SI_ES) tmp -= rnd(25);
	if (uarmc && uarmc->oartifact == ART_OLD_PERSON_TALK) tmp -= 5;
	if (uwep && uwep->oartifact == ART_BLACK_MARK) tmp -= 1;
	if (uwep && uwep->oartifact == ART_LOUD_SHITTER) tmp -= 7;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_LOUD_SHITTER) tmp -= 7;
	if (uwep && uwep->oartifact == ART_CHINESE_MODEL) tmp -= 5;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_CHINESE_MODEL) tmp -= 5;
	if (uwep && uwep->oartifact == ART_XUANLONG) tmp -= 5;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_XUANLONG) tmp -= 5;
	if (uarm && uarm->oartifact == ART_POWASPEL) tmp -= 3;
	if (uwep && uwep->oartifact == ART_UZ_I) tmp -= rnd(10);
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_UZ_I) tmp -= rnd(10);
	if (uwep && uwep->oartifact == ART_AR_ARMALYTE) tmp -= 2;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_AR_ARMALYTE) tmp -= 2;
	if (u.martialstyle == MARTIALSTYLE_MUAYTHAI) tmp -= 5;
	if (u.martialstyle == MARTIALSTYLE_BOXING) tmp -= 5;

	if (uarm && uarm->oartifact == ART_DAMMIT_PICK_UP) tmp -= 5;

	if (Role_if(PM_OTAKU) && uarmc && itemhasappearance(uarmc, APP_FOURCHAN_CLOAK)) tmp += 1;

	if (is_grassland(u.ux, u.uy) && !(uamul && uamul->oartifact == ART_CONTRO_MOVE) && !(uarm && uarm->oartifact == ART_OBERTURT) && !(uarms && uarms->oartifact == ART_FLORENSE_S_GEBLOOMEL) && !(uarm && uarm->oartifact == ART_DORL_TSCH) && !(uarmf && itemhasappearance(uarmf, APP_GARDEN_SLIPPERS))) tmp -= rnd(5);

	if (ACURR(A_DEX) < 2) tmp -= 5;
	else if (ACURR(A_DEX) < 3) tmp -= 4;
	else if (ACURR(A_DEX) < 4) tmp -= 3;
	else if (ACURR(A_DEX) < 6) tmp -= 2;
	else if (ACURR(A_DEX) < 8) tmp -= 1;
	else if (ACURR(A_DEX) >= 12) tmp += (ACURR(A_DEX) - 11);

	if (tech_inuse(T_STEADY_HAND)) tmp += 5;

	if (uarmc && uarmc->oartifact == ART_ROKKO_CHAN_S_SUIT) tmp += 5;

	if (!issoviet && !rn2(20 - (GushLevel / 2) )) tmp += rnd(GushLevel);

	if (Race_if(PM_ENGCHIP) && objects[obj->otyp].oc_skill == P_BOW) tmp -= 5;
	if (Race_if(PM_ENGCHIP) && objects[obj->otyp].oc_skill == -P_BOW) tmp -= 5;
	if (Race_if(PM_ENGCHIP) && objects[obj->otyp].oc_skill == P_CROSSBOW) tmp -= 5;
	if (Race_if(PM_ENGCHIP) && objects[obj->otyp].oc_skill == -P_BOW) tmp -= 5;
	if (uwep && uwep->oartifact == ART_KLOBB) tmp -= 6;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_KLOBB) tmp -= 6;
	if (uwep && uwep->oartifact == ART_EXCALIPOOR) tmp -= 9;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_EXCALIPOOR) tmp -= 9;

	if (Race_if(PM_VIETIS) && objects[obj->otyp].oc_skill != -P_FIREARM && objects[obj->otyp].oc_skill != P_FIREARM) tmp -= rnd(10);

	if (Race_if(PM_BOVER)) {
		if (uarm && is_metallic(uarm)) tmp -= rnd(3);
		if (uarmu && is_metallic(uarmu)) tmp -= rnd(3);
		if (uarmc && is_metallic(uarmc)) tmp -= rnd(3);
		if (uarms && is_metallic(uarms)) tmp -= rnd(3);
		if (uarmh && is_metallic(uarmh)) tmp -= rnd(3);
		if (uarmg && is_metallic(uarmg)) tmp -= rnd(3);
		if (uarmf && is_metallic(uarmf)) tmp -= rnd(3);
	}

	/* shooters that have innate to-hit penalty for the player go here; if you're dual-wielding two launchers
	 * that use the same type of ammo, and the secondary one has such a penalty, it's supposed to be much higher
	 * than if it was the primary launcher, for balance purposes, since otherwise dual-wielding assault rifles
	 * would be far better in every way than a heavy MG! --Amy
	 * annoyingly, nozzle change (artifact lead unloader) has already changed its fire mode at this point... */

inaccurateguns:
	if (launcher && launcher->otyp == HYDRA_BOW) tmp -= rnd(8);
	if (launcher && launcher->otyp == WILDHILD_BOW && !Role_if(PM_HUSSY)) tmp -= rnd(10);
	if (launcher && launcher->otyp == CATAPULT) {
		tmp -= rnd(10);
		if (!rn2(2)) tmp -= rnd(10);
	}
	if (launcher && launcher->otyp == SUBMACHINE_GUN && launcher->altmode == WP_MODE_AUTO) tmp -= rnd(6);
	if (launcher && launcher->otyp == LEAD_UNLOADER && launcher->altmode == WP_MODE_AUTO) tmp -= rnd(6);
	if (launcher && launcher->oartifact == ART_NOZZLE_CHANGE && launcher->altmode == WP_MODE_SINGLE) tmp -= rnd(6);
	if (launcher && launcher->otyp == AUTO_SHOTGUN && launcher->altmode == WP_MODE_AUTO) tmp -= rnd(8);
	if (launcher && launcher->otyp == POWER_CROSSBOW) tmp -= rnd(8);
	if (launcher && launcher->otyp == PILE_BUNKER) tmp -= rnd(4);
	if (launcher && launcher->otyp == ASSAULT_RIFLE && (launcher->altmode == WP_MODE_AUTO || (launcher->altmode == WP_MODE_BURST && !rn2(3)) ) ) {
		tmp -= rnd(8);
		if (!rn2(3)) tmp -= rnd(5);
	}
	if (launcher && launcher->otyp == STORM_RIFLE && (launcher->altmode == WP_MODE_AUTO || (launcher->altmode == WP_MODE_BURST && !rn2(3)) ) ) {
		tmp -= rnd(8);
		if (!rn2(3)) tmp -= rnd(5);
	}
	if (launcher && launcher->otyp == ARM_BLASTER) {
		tmp -= rnd(10);
		if (!rn2(2)) tmp -= rnd(8);
	}
	if (launcher && launcher->otyp == DEMON_CROSSBOW && launcher->altmode == WP_MODE_AUTO) {
		tmp -= rnd(20);
		if (!rn2(2)) tmp -= rnd(6);
	}
	if (launcher && launcher->otyp == KALASHNIKOV && (launcher->altmode == WP_MODE_AUTO || (launcher->altmode == WP_MODE_BURST && !rn2(3)) ) ) {
		tmp -= rnd(9);
		if (!rn2(2)) tmp -= rnd(5);
	}
	if (launcher && launcher->otyp == HEAVY_MACHINE_GUN) {
		tmp -= rnd(20);
		if (!rn2(2)) tmp -= rnd(10);
		if (!rn2(3)) tmp -= rnd(10);
	}
	if (launcher && launcher->otyp == MILITARY_RIFLE && (launcher->altmode == WP_MODE_AUTO || (launcher->altmode == WP_MODE_BURST && !rn2(3)) ) ) {
		tmp -= rnd(18);
		if (!rn2(2)) tmp -= rnd(9);
	}
	if (launcher && launcher->otyp == PISTOL_PAIR && P_RESTRICTED(P_TWO_WEAPON_COMBAT)) {
		tmp -= rnd(20);
	}

	if (launcher && launcher->oartifact == ART_FN_M____PARA) tmp -= rnd(15);
	if (launcher && launcher->oartifact == ART_CITYKILLER_COMBAT_SHOTGUN) tmp -= rnd(10);
	if (launcher && launcher->oartifact == ART_COLONEL_BASTARD_S_LASER_PI) tmp -= rnd(5);

	if (doubleshot) {
		if (!rn2(2)) doubleshot = FALSE;
		goto inaccurateguns;
	}

	if (Race_if(PM_SWIKNI)) {
		if (obj) {
			if (obj->oeroded) tmp -= ((obj->oeroded) * 2);
			if (obj->oeroded2) tmp -= ((obj->oeroded2) * 2);
		}

	}

	if (tech_inuse(T_UNARMED_FOCUS)) tmp -= rnd(20);
	if (u.martialstyle == MARTIALSTYLE_KUNGFU) tmp -= rnd(20);

	/* quarterback is highly skilled at shooting small round objects --Amy */
	if (Role_if(PM_QUARTERBACK) && objects[obj->otyp].oc_skill == -P_SLING) tmp += rn1(5, 5);
	if (Role_if(PM_QUARTERBACK) && objects[obj->otyp].oc_skill == P_SLING) tmp += rn1(5, 5);

	/* let's just add that bonus anyway. --Amy */
	if(mon->mstun) tmp += 2;
	if(mon->mflee) tmp += 2;
	if(mon->msleeping) tmp += 2;
	if(!mon->mcanmove) tmp += 4;

	/* Missile weapon skill: without it, you generally have a lower to-hit; with it, you eventually gain positive bonuses --Amy */
	tmp -= rn2(5);
	skillpierce = 0;
	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_MISSILE_WEAPONS)) {
			default: break;
			case P_BASIC: tmp += rnd(2); skillpierce += 1; break;
			case P_SKILLED: tmp += rnd(3); skillpierce += 2; break;
			case P_EXPERT: tmp += rnd(5); skillpierce += 3; break;
			case P_MASTER: tmp += rnd(6); skillpierce += 4; break;
			case P_GRAND_MASTER: tmp += rnd(8); skillpierce += 5; break;
			case P_SUPREME_MASTER: tmp += rnd(10); skillpierce += 6; break;
		}

		if (obj && objects[obj->otyp].oc_skill == -P_FIREARM) {
			switch (P_SKILL(P_GUN_CONTROL)) {
				default: break;
				case P_BASIC: tmp += rnd(2); skillpierce += 1; break;
				case P_SKILLED: tmp += rnd(4); skillpierce += 2; break;
				case P_EXPERT: tmp += rnd(6); skillpierce += 3; break;
				case P_MASTER: tmp += rnd(8); skillpierce += 4; break;
				case P_GRAND_MASTER: tmp += rnd(10); skillpierce += 5; break;
				case P_SUPREME_MASTER: tmp += rnd(12); skillpierce += 6; break;
			}
		}

		switch (P_SKILL(P_DJEM_SO)) {
			default: break;
			case P_BASIC: tmp += 1; break;
			case P_SKILLED: tmp += rnd(2); break;
			case P_EXPERT: tmp += rnd(3); break;
			case P_MASTER: tmp += rnd(4); break;
			case P_GRAND_MASTER: tmp += rnd(5); break;
			case P_SUPREME_MASTER: tmp += rnd(6); break;
		}

		/* shien and djem so are both "form V" so they boost each other --Amy
		 * reduced monster evasiveness if you've enhanced both */
		if (obj && is_lightsaber(obj) && (obj->lamplit || Role_if(PM_SHADOW_JEDI)) ) {
			if (P_SKILL(P_SHIEN) >= P_BASIC && P_SKILL(P_DJEM_SO) >= P_BASIC) skillpierce++;
			if (P_SKILL(P_SHIEN) >= P_SKILLED && P_SKILL(P_DJEM_SO) >= P_SKILLED) skillpierce++;
			if (P_SKILL(P_SHIEN) >= P_EXPERT && P_SKILL(P_DJEM_SO) >= P_EXPERT) skillpierce++;
			if (P_SKILL(P_SHIEN) >= P_MASTER && P_SKILL(P_DJEM_SO) >= P_MASTER) skillpierce++;
			if (P_SKILL(P_SHIEN) >= P_GRAND_MASTER && P_SKILL(P_DJEM_SO) >= P_GRAND_MASTER) skillpierce++;
			if (P_SKILL(P_SHIEN) >= P_SUPREME_MASTER && P_SKILL(P_DJEM_SO) >= P_SUPREME_MASTER) skillpierce++;
		}

		/* polearms and such should get to-hit bonuses from general combat as well, because I want it :D --Amy */
		if (pieks) {

			switch (P_SKILL(P_GENERAL_COMBAT)) {
				default: break;
				case P_BASIC: tmp += 1; skillpierce += 1; break;
				case P_SKILLED: tmp += rnd(2); skillpierce += 2; break;
				case P_EXPERT: tmp += rnd(3); skillpierce += 3; break;
				case P_MASTER: tmp += rnd(4); skillpierce += 4; break;
				case P_GRAND_MASTER: tmp += rnd(5); skillpierce += 5; break;
				case P_SUPREME_MASTER: tmp += rnd(6); skillpierce += 6; break;
			}

		}

	}

	if (uwep && uwep->oartifact == ART_ULTRA_ANNOYANCE && pieks) skillpierce += 5;

	if (stopevading) skillpierce += rnd(5);

	if (Race_if(PM_FRO) && objects[obj->otyp].oc_skill == P_AXE) {
		tmp += 5;
	}
	
	if (Numbed) {
		if (tmp > 1) {
			tmp *= 9;
			tmp /= 10;
		}
		tmp -= 2;
	}

	if (u.tremblingamount) tmp -= rnd(u.tremblingamount);

	if (!rn2(20)) tmp -= 20; /* catastrophic failure on a "natural 20", similar to D&D --Amy */
	if (Race_if(PM_INHERITOR) && !rn2(100)) tmp -= 20;

	if (Role_if(PM_FAILED_EXISTENCE) && rn2(2)) tmp = -100; /* 50% chance of automiss --Amy */
	if (uarmc && uarmc->oartifact == ART_ARTIFICIAL_FAKE_DIFFICULTY && !rn2(6)) tmp = -100;

	if (obj && obj->oartifact == ART_EVASION_BREAK) goto evasionchancedone;

	/* certain monsters are capable of deflecting projectiles --Amy */

	if (!pieks && !(uarm && uarm->oartifact == ART_ARMOR_PIERCE) && !(gunused && rn2(3)) && !(rn2(13) < skillpierce ) ) {

		if (verysmall(mon->data) && !rn2(4)) {
			tmp = -100;
			pline("%s avoids the projectile!", Monnam(mon));
		}
		if (rathersmall(mon->data) && !(verysmall(mon->data)) && !rn2(10)) {
			tmp = -100;
			pline("%s avoids the projectile!", Monnam(mon));
		}
		if (hugemonst(mon->data) && !rn2(2) && (mon->m_lev > rnd(GushLevel) ) ) {
			tmp = -100;
			pline("%s shrugs off the projectile!", Monnam(mon));
		}
		if (bigmonst(mon->data) && !(hugemonst(mon->data)) && !rn2(5) && (mon->m_lev > rnd(GushLevel) ) ) {
			tmp = -100;
			pline("%s shrugs off the projectile!", Monnam(mon));
		}

	}

	/* certain traits also allow monsters to avoid getting hit */

	if (amorphous(mon->data) && !(uarm && uarm->oartifact == ART_ARMOR_PIERCE) && !rn2(5) && tmp > -50 && !(rn2(20) < skillpierce ) ) {
		tmp = -100;
		pline("%s's amorphous body skillfully dodges the projectile!", Monnam(mon));
	}
	if (noncorporeal(mon->data) && !(uarm && uarm->oartifact == ART_ARMOR_PIERCE) && rn2(3) && tmp > -50 && !(rn2(40) < skillpierce ) ) {
		tmp = -100;
		pline("%s easily avoids the projectile due to being noncorporeal!", Monnam(mon));
	}
	if (unsolid(mon->data) && !(uarm && uarm->oartifact == ART_ARMOR_PIERCE) && !rn2(4) && tmp > -50 && !(rn2(20) < skillpierce ) ) {
		tmp = -100;
		pline("%s's unsolid body lets the projectile pass through harmlessly!", Monnam(mon));
	}

evasionchancedone:

	if (FemtrapActiveNatalia && !flags.female && spawnswithhammersandal(mon->data) && rn2(4)) {
		tmp = -100;
		pline("The projectile just passes through %s!", mon_nam(mon));
	}

	if (mon->data == &mons[PM_XXXXXXXXXXXXXXXXXXXX]) {
		if (tmp > -100) tmp = -100;
		pline("The projectile just passes through %s!", mon_nam(mon));
	}

	if (mon->data == &mons[PM_IDE_BY__]) {
		if (tmp > -100) tmp = -100;
		pline("The projectile just passes through %s!", mon_nam(mon));
	}

	/* the elder priest uses cheats */
	if (swatting_monster(mon->data) && rn2(15) && tmp > -50) {
		tmp = -100;
		pline("%s swats the projectile away!", Monnam(mon));
	}

	if (monstersoundtype(mon) == MS_BULLETATOR && bulletate && rn2(4)) {
		tmp = -100;
		pline("%s absorbs the projectile!", Monnam(mon));
	}

	if (FemtrapActiveAnnemarie && humanoid(mon->data) && mon->female && rn2(15) && tmp > -50) {
		tmp = -100;
		pline("%s swats the projectile away!", Monnam(mon));
	}

	if (ecm_monster(mon->data) && rn2(15) && tmp > -50) {
		tmp = -100;
		pline("%s uses an ECM system to divert the projectile!", Monnam(mon));
	}

	if (stupidrock && !(uarm && uarm->oartifact == ART_ARMOR_PIERCE) && (tmp > -50) && !(rn2(25) < skillpierce ) ) {
		if (verysmall(mon->data) && !rn2(4)) {
			tmp = -100;
			pline("%s avoids the projectile!", Monnam(mon));
		}
		if (rathersmall(mon->data) && !(verysmall(mon->data)) && !rn2(10)) {
			tmp = -100;
			pline("%s avoids the projectile!", Monnam(mon));
		}
		if (hugemonst(mon->data) && !rn2(2) && (mon->m_lev > rnd(GushLevel) ) ) {
			tmp = -100;
			pline("%s shrugs off the projectile!", Monnam(mon));
		}
		if (bigmonst(mon->data) && !(hugemonst(mon->data)) && !rn2(5) && (mon->m_lev > rnd(GushLevel) ) ) {
			tmp = -100;
			pline("%s shrugs off the projectile!", Monnam(mon));
		}
		if (amorphous(mon->data) && !rn2(5) && tmp > -50) {
			tmp = -100;
			pline("%s's amorphous body skillfully dodges the projectile!", Monnam(mon));
		}
		if (noncorporeal(mon->data) && rn2(3) && tmp > -50) {
			tmp = -100;
			pline("%s easily avoids the projectile due to being noncorporeal!", Monnam(mon));
		}
		if (unsolid(mon->data) && !rn2(4) && tmp > -50) {
			tmp = -100;
			pline("%s's unsolid body lets the projectile pass through harmlessly!", Monnam(mon));
		}

	}

	if (MON_WEP(mon)) { /* shien monster lightsaber form */
		struct obj *monweapon;
		monweapon = MON_WEP(mon);
		if (monweapon) {
			if (is_lightsaber(monweapon) && tmp > -50 && monweapon->lamplit && (!rn2(2) || ((mon->data->geno & G_UNIQ) && !rn2(2)) ) ) {
				tmp = -100;
				pline("%s's lightsaber blocks the projectile!", Monnam(mon));

			}
		}
	}

	if (blocker = (which_armor(mon, W_ARMS))) {

		shieldblockrate = shield_block_rate(blocker);
		shieldblockrate += 10; /* monsters can simply block better --Amy */

		if (blocker->otyp == ELVEN_SHIELD && is_elf(mon->data)) shieldblockrate += 5;
		if (blocker->otyp == URUK_HAI_SHIELD && is_orc(mon->data)) shieldblockrate += 5;
		if (blocker->otyp == ORCISH_SHIELD && is_orc(mon->data)) shieldblockrate += 5;
		if (blocker->otyp == ORCISH_GUARD_SHIELD && is_orc(mon->data)) shieldblockrate += 5;
		if (blocker->otyp == DWARVISH_ROUNDSHIELD && is_dwarf(mon->data)) shieldblockrate += 5;

		if (shieldblockrate && (blocker->spe > 0)) shieldblockrate += (blocker->spe * 2);
		if (blocker->blessed) shieldblockrate += 5;

		if (stupidrock && shieldblockrate) shieldblockrate *= 2;

		if (uarm && (uarm->oartifact == ART_ARMOR_PIERCE) && (shieldblockrate > 1)) {
			shieldblockrate /= 10;
			if (shieldblockrate < 1) shieldblockrate = 1;
		}

		if (blocker->otyp == BROKEN_SHIELD) shieldblockrate = 0;

		if ((rnd(100) < shieldblockrate) && tmp > -50) {
			tmp = -100;
			pline("%s's shield deflects your projectile!", Monnam(mon));
		}
	}

	if (Race_if(PM_ENGCHIP) && !rn2(20) && objects[obj->otyp].oc_skill == P_BOW) tmp = -100;
	if (Race_if(PM_ENGCHIP) && !rn2(20) && objects[obj->otyp].oc_skill == -P_BOW) tmp = -100;
	if (Race_if(PM_ENGCHIP) && !rn2(20) && objects[obj->otyp].oc_skill == P_CROSSBOW) tmp = -100;
	if (Race_if(PM_ENGCHIP) && !rn2(20) && objects[obj->otyp].oc_skill == -P_BOW) tmp = -100;

	/* Modify to-hit depending on distance; but keep it sane.
	 * Polearms get a distance penalty even when wielded; it's
	 * hard to hit at a distance.
	 */

	/* Amy edit: ranged combat was way too powerful, I decided to greatly reduce to-hit at long distances.
	 * Firearms are an exception for two reasons: one, their ammo always breaks; two, some of them (especially rifles)
	 * are actually meant to be used at range and those that are not (e.g. shotguns) have limited range anyway.
	 * Crossbows are a bit of a special case: they're potentially the most damaging shooter for which the ammos
	 * can be re-used, but most types also need a pretty high enchantment value to get any serious multishot,
	 * and they're also kind of meant to be used at long range, so their to-hit will receive a lesser nerf. */

	if (obj && obj->oartifact == ART_FTS) tmp += distmin(u.ux, u.uy, mon->mx, mon->my);

	if ( !(launcher && ammo_and_launcher(obj, launcher) && objects[launcher->otyp].oc_skill == P_FIREARM) && (!(launcher  && ammo_and_launcher(obj, launcher) && objects[launcher->otyp].oc_skill == P_CROSSBOW && !rn2(2)) ) ) {

		disttmp = 3 - distmin(u.ux, u.uy, mon->mx, mon->my);
		if(disttmp < -4) disttmp = -4;

		if (uarmg && uarmg->oartifact == ART_SNIPE_EVERYTHING && disttmp < 0) disttmp = 0;

		tmp += disttmp;

		if ((distmin(u.ux, u.uy, mon->mx, mon->my) > 3) && !(uarmg && uarmg->oartifact == ART_SNIPE_EVERYTHING) ) {

			switch (distmin(u.ux, u.uy, mon->mx, mon->my)) {

				case 4:
					tmp -= rn2(2);
					break;
				case 5:
					tmp -= rn2(4);
					break;
				case 6:
					tmp -= rn2(6);
					break;
				case 7:
					tmp -= rn2(9);
					break;
				case 8:
					tmp -= rn2(12);
					break;
				case 9:
					tmp -= rn2(16);
					break;
				case 10:
				default:
					tmp -= rn2(20);
					break;

			}

		}

	}

	/* gloves are a hinderance to proper use of bows */
	if (uarmg && launcher && objects[launcher->otyp].oc_skill == P_BOW) {
	    switch (uarmg->otyp) {
	    case GAUNTLETS_OF_POWER:    /* metal */
		tmp -= 2;
		break;
	    case GAUNTLETS_OF_FUMBLING:
		tmp -= 3;
		break;
	    case GAUNTLETS_OF_MISFIRING:
		tmp -= 5;
		break;
	    case GAUNTLETS_OF_PANIC:
		tmp -= 1;
		break;
	    case GAUNTLETS_OF_FREE_ACTION:
	    case ELVEN_GAUNTLETS:
	    case FEMALE_GLOVES:
		tmp += 1;
		break;
	    case GAUNTLETS_OF_TRUE_AIM:
		tmp += 2;
		break;
	    case REGULAR_GLOVES:
	    case GAUNTLETS_OF_SWIMMING:            
	    case GAUNTLETS_OF_DEXTERITY:
		break;
	    default: /* why do we need this impossible message anyway? --Amy */
		/*impossible("Unknown type of gloves (%d)", uarmg->otyp);*/
		break;
	    }
	}

	if (obj && launcher && ammo_and_launcher(obj, launcher) && launcher->otyp == LASERXBOW && launcher->lamplit && launcher->altmode) tmp += 5;

/*	with a lot of luggage, your agility diminishes */
	if (near_capacity()) tmp -= rnd(near_capacity() * 5);
	if (u.utrap) tmp -= 5;

	if (uarmg && itemhasappearance(uarmg, APP_CLUMSY_GLOVES)) tmp -= 3;

	if (Race_if(PM_PLAYER_SKELETON)) tmp -= rnd(u.ulevel); /* lesser nerf than melee, since you also misfire */

	tmp += omon_adj(mon, obj, TRUE);
	if (is_orc(mon->data) && maybe_polyd(is_elf(youmonst.data),
			Race_if(PM_ELF)))
	    tmp++;
	if (guaranteed_hit) {
	    tmp += 1000; /* Guaranteed hit */
	}

	/* sleeping unicorns don't catch gems */
	if (obj->oclass == GEM_CLASS && is_unicorn(mon->data) && !mon->msleeping) {
	    if (mon->mtame) {
		pline("%s catches and drops %s.", Monnam(mon), the(xname(obj)));
		return 0;
	    } else {
		pline("%s catches %s.", Monnam(mon), the(xname(obj)));
		return gem_accept(mon, obj);
	    }
	}

	/* don't make game unwinnable if naive player throws artifact
	   at leader.... */
	if (quest_arti_hits_leader(obj, mon)) {
	    /* not wakeup(), which angers non-tame monsters */
	    mon->msleeping = 0;
	    mon->mstrategy &= ~STRAT_WAITMASK;

	    if (mon->mcanmove) {
		pline("%s catches %s.", Monnam(mon), the(xname(obj)));
		if (mon->mpeaceful) {
		    boolean next2u = monnear(mon, u.ux, u.uy);

		    finish_quest(obj);	/* acknowledge quest completion */
		    pline("%s %s %s back to you.", Monnam(mon),
			  (next2u ? "hands" : "tosses"), the(xname(obj)));
		    if (!next2u) sho_obj_return_to_u(obj);
		    obj = addinv(obj);	/* back into your inventory */
		    (void) encumber_msg();
		} else {
		    /* angry leader caught it and isn't returning it */
		    (void) mpickobj(mon, obj, FALSE);
		}
		return 1;		/* caller doesn't need to place it */
	    }
	    return(0);
	}

	if (!polearming && befriend_with_obj(mon->data, obj)) goto befriended;

	if (obj->oclass == WEAPON_CLASS || obj->oclass == BALL_CLASS || obj->oclass == CHAIN_CLASS || obj->oclass == VENOM_CLASS || is_weptool(obj) || obj->oartifact == ART_KLUEUEUEU || obj->oartifact == ART_GURKNOOK ||
		obj->oclass == GEM_CLASS) {
	    if (is_ammo(obj)) {
		if (!ammo_and_launcher(obj, launcher)) {
		    tmp -= 4;
		} else {
		    if (!InvertedState) {
			tmp += launcher->spe - greatest_erosionX(launcher);
		    } else {
			tmp -= abs(launcher->spe);
			tmp -= greatest_erosionX(launcher);
		    }
		    tmp += weapon_hit_bonus(launcher);
		    if (launcher->oartifact) tmp += spec_abon(launcher, mon);
		    /*
		     * Elves and Samurais are highly trained w/bows,
		     * especially their own special types of bow.
		     * Polymorphing won't make you a bow expert.
		     */
		    if ((Race_if(PM_ELF) || Race_if(PM_PLAYER_MYRKALFR) || Race_if(PM_DROW) ||
		    		Role_if(PM_SAMURAI) || Role_if(PM_ELPH) || Role_if(PM_TWELPH)) &&
				(!Upolyd || your_race(youmonst.data)) &&
				objects[launcher->otyp].oc_skill == P_BOW) {
			tmp++;
			if ((Race_if(PM_ELF) || Race_if(PM_PLAYER_MYRKALFR)) && launcher->otyp == ELVEN_BOW)
			tmp++;
			/*else */if (Race_if(PM_DROW) && launcher->otyp == DARK_ELVEN_BOW)
			    tmp++;
			/*else */if (Role_if(PM_ELPH) && launcher->otyp == ELVEN_BOW)
			tmp++;
			/*else */if (Role_if(PM_TWELPH) && launcher->otyp == DARK_ELVEN_BOW)
			tmp++;
			/*else */if (Role_if(PM_SAMURAI) && launcher->otyp == YUMI)
			    tmp++;
		    }
		}
	    } else {
		if (otyp == BOOMERANG || otyp == ALU_BOOMERANG || otyp == MITHRIL_BOOMERANG || otyp == SILVER_CHAKRAM || otyp == BATARANG || otyp == DARK_BATARANG)		/* arbitrary */
		    tmp += 4;
		else if (throwing_weapon(obj))	/* meant to be thrown */
		    tmp += 2;
		else				/* not meant to be thrown */
		    tmp -= 2;
		/* we know we're dealing with a weapon or weptool handled
		   by WEAPON_SKILLS once ammo objects have been excluded */
		tmp += weapon_hit_bonus(obj);
	    }
#ifdef DEBUG
	    pline("(%i/20)", tmp);
#endif

	    if (tmp >= dieroll) {
		ranged_thorns(mon);
		if (hmon(mon,obj,thrown?thrown:3,dieroll)) {  /* mon still alive */
		    (void) cutworm(mon, bhitpos.x, bhitpos.y, obj);
		}
		exercise(A_DEX, TRUE);

		if (obj && obj->otyp == LASER_POLE) {
			u.uvaapadturns++;
			if (u.uvaapadturns >= 4) {
				u.uvaapadturns = 0;
				use_skill(P_VAAPAD, 1);
			}
		}

		if (obj && obj->otyp == LASER_CHAIN && !bimanual(obj) && !uarms && !u.twoweap) {
			u.umakashiturns++;
			if (u.umakashiturns >= 4) {
				u.umakashiturns = 0;
				use_skill(P_MAKASHI, 1);
			}
		}

		/* Detonate bolts shot by Hellfire */
		if (ammo_and_launcher(obj, launcher) &&
			(launcher->oartifact == ART_HELLFIRE || launcher->oartifact == ART_LONG_FIREMISSILE || launcher->oartifact == ART_EVERCONSUMING_HELLFIRE || obj->oartifact == ART_BAKUHATSU_SEI_MISAIRU || obj->oartifact == ART_POMME_GRENADE || launcher->oartifact == ART_UNIDENTIFIED_HELLCAST || launcher->oartifact == ART_SEVENTH_SCRIPTURE)) {
		    if (cansee(bhitpos.x,bhitpos.y)) 
			pline("%s explodes in a ball of fire!", Doname2(obj));
		    else You_hear("an explosion");
		    explode(bhitpos.x, bhitpos.y, ZT_SPELL(ZT_FIRE),
			    d(2,6), WEAPON_CLASS, EXPL_FIERY);
		}

		/* projectiles other than magic stones
		 * sometimes disappear when thrown
		 * WAC - Spoon always disappears after doing damage
		 */
		if (((objects[otyp].oc_skill < P_NONE && 
			objects[otyp].oc_skill > -P_BOOMERANG) ||
			( objects[otyp].oc_skill == P_DAGGER && ((obj->spe < 1) || (obj->spe > 0 && (!rn2(obj->spe + 1) || !rn2(obj->spe + 1))) ) && (!obj->oartifact || !rn2(1000)) ) ||
			( objects[otyp].oc_skill == P_KNIFE && ((obj->spe < 1) || (obj->spe > 0 && (!rn2(obj->spe + 1) || !rn2(obj->spe + 1))) ) && (!obj->oartifact || !rn2(1000)) ) ||
			( objects[otyp].oc_skill == P_SPEAR && ((obj->spe < 1) || (obj->spe > 0 && (!rn2(obj->spe + 1) || !rn2(obj->spe + 1) || !rn2(obj->spe + 1))) ) && (!obj->oartifact || !rn2(1000)) ) ||
			( objects[otyp].oc_skill == P_JAVELIN && ((obj->spe < 1) || (obj->spe > 0 && !rn2(obj->spe + 1)) ) && (!obj->oartifact || !rn2(1000)) ) ||

/* low chance for daggers, knives and spears to disappear --Amy */
			(obj->oclass == GEM_CLASS && 
			(!objects[otyp].oc_magic || !rn2(5) )))	/* also low chance for loadstones etc. to disappear */
			|| (obj->oartifact == ART_HOUCHOU)
			|| (obj->oartifact == ART_VORPALITY_TEMPTATION && u.vorpalitytempt >= 100)
			/* WAC catch Hellfire */
			|| (launcher && (launcher->oartifact == ART_HELLFIRE || launcher->oartifact == ART_LONG_FIREMISSILE || launcher->oartifact == ART_EVERCONSUMING_HELLFIRE || obj->oartifact == ART_BAKUHATSU_SEI_MISAIRU || obj->oartifact == ART_POMME_GRENADE || launcher->oartifact == ART_UNIDENTIFIED_HELLCAST || launcher->oartifact == ART_SEVENTH_SCRIPTURE)
			&& is_ammo(obj) && ammo_and_launcher(obj, launcher))
		   ) {
		    /* we were breaking 2/3 of everything unconditionally.
		     * we still don't want anything to survive unconditionally,
		     * but we need ammo to stay around longer on average.
		     */
		    int broken, chance;
		    chance = greatest_erosionX(obj) - obj->spe;	/* base chance increased --Amy */
		    chance -= rnd(2);
		    if (chance > 1) {
			if (chance == 3) chance = 2;
			else if (chance == 4) chance = 3;
			else if (chance == 5) chance = 3;
			else if (chance > 5) chance /= 2;
			broken = rn2(chance);
		    } else {/* continue to survive longer with better enchantment --Amy */
			chance = 3 + obj->spe - greatest_erosionX(obj);
			if (chance > 3) chance = 2 + rno(chance - 2);
			if (chance < 2) chance = 2; /* fail safe */
			if (Race_if(PM_MONGUNG)) chance *= 2;
			broken = !rn2(chance);
		    }
		    if ( objects[otyp].oc_skill == P_DAGGER )
			broken = !rn2(Race_if(PM_MONGUNG) ? 80 : 40);
		    if ( objects[otyp].oc_skill == P_SPEAR )
			broken = !rn2(Race_if(PM_MONGUNG) ? 150 : 75);
		    if ( objects[otyp].oc_skill == P_KNIFE )
			broken = !rn2(Race_if(PM_MONGUNG) ? 160 : 80);
		    if ( objects[otyp].oc_skill == P_JAVELIN )
			broken = !rn2(Race_if(PM_MONGUNG) ? 2400 : 1200);
		    if (obj->blessed && !rnl(6))
			broken = 0;
			/* also save uncursed ones sometimes --Amy */
		    if (!obj->blessed && !obj->cursed && !rn2(3) && !rnl(6))
			broken = 0;

			if (!(PlayerCannotUseSkills)) {
				switch (P_SKILL(P_MISSILE_WEAPONS)) {
					default: break;
					case P_BASIC: if (rn2(10) < 1) broken = 0; break;
					case P_SKILLED: if (rn2(10) < 2) broken = 0; break;
					case P_EXPERT: if (rn2(10) < 3) broken = 0; break;
					case P_MASTER: if (rn2(10) < 4) broken = 0; break;
					case P_GRAND_MASTER: if (rn2(10) < 5) broken = 0; break;
					case P_SUPREME_MASTER: if (rn2(10) < 6) broken = 0; break;
				}
			}

			/* allow skill to save ammo --Amy */

			if (!(PlayerCannotUseSkills)) {

		    if (objects[otyp].oc_skill == -P_BOW && (P_SKILL(P_BOW) >= P_BASIC) && rn2(P_SKILL(P_BOW)) )
			broken = 0;
		    if (objects[otyp].oc_skill == -P_CROSSBOW && (P_SKILL(P_CROSSBOW) >= P_BASIC) && rn2(P_SKILL(P_CROSSBOW)) )
			broken = 0;
		    if (objects[otyp].oc_skill == -P_SLING && (P_SKILL(P_SLING) >= P_BASIC) && rn2(P_SKILL(P_SLING)) )
			broken = 0;
		    if (objects[otyp].oc_skill == -P_DART && (P_SKILL(P_DART) >= P_BASIC) && rn2(P_SKILL(P_DART)) )
			broken = 0;
		    if (objects[otyp].oc_skill == -P_SHURIKEN && (P_SKILL(P_SHURIKEN) >= P_BASIC) && rn2(P_SKILL(P_SHURIKEN)) )
			broken = 0;
		    if (objects[otyp].oc_skill == P_DAGGER && (P_SKILL(P_DAGGER) >= P_BASIC) && rn2(P_SKILL(P_DAGGER)) )
			broken = 0;
		    if (objects[otyp].oc_skill == P_SPEAR && (P_SKILL(P_SPEAR) >= P_BASIC) && rn2(P_SKILL(P_SPEAR)) )
			broken = 0;
		    if (objects[otyp].oc_skill == P_JAVELIN && (P_SKILL(P_JAVELIN) >= P_BASIC) && rn2(P_SKILL(P_JAVELIN)) )
			broken = 0;
		    if (objects[otyp].oc_skill == P_KNIFE && (P_SKILL(P_KNIFE) >= P_BASIC) && rn2(P_SKILL(P_KNIFE)) )
			broken = 0;

			}

		    if (objects[otyp].oc_skill == -P_BOW && uarm && uarm->oartifact == ART_WOODSTOCK && broken && !rn2(2))
			broken = 0;
		    if (objects[otyp].oc_skill == -P_SLING && uarms && uarms->oartifact == ART_MISSING_LETTER_D && broken && rn2(4))
			broken = 0;
		    if (objects[otyp].oc_material == MT_MINERAL && uarm && uarm->oartifact == ART_QUARRY && broken && !rn2(2))
			broken = 0;
		    if (uarmc && uarmc->oartifact == ART_ARABELLA_S_WEAPON_STORAGE && broken && !rn2(2))
			broken = 0;
		    if (Race_if(PM_MACTHEIST) && objects[otyp].oc_skill == P_SLING && broken && !rn2(2))
			broken = 0;
		    if (Race_if(PM_MACTHEIST) && objects[otyp].oc_skill == -P_SLING && broken && !rn2(2))
			broken = 0;
		    if (autismringcheck(ART_ACTUAL_CAVE_DWELLING) && objects[otyp].oc_skill == P_SLING && broken && rn2(4))
			broken = 0; /* doesn't stack if you wear two, this is by design --Amy */
		    if (autismringcheck(ART_ACTUAL_CAVE_DWELLING) && objects[otyp].oc_skill == -P_SLING && broken && rn2(4))
			broken = 0;

		    if (obj->oartifact == ART_USE_A_LOT) {
			if (rn2(10)) broken = 0;
		    }
		    if (obj->oartifact == ART_WIUNEW) {
			if (rn2(10)) broken = 0;
		    }
		    if (obj->oartifact == ART_BE_CONSERVED) {
			if (rn2(10)) broken = 0;
		    }
		    if (obj->oartifact == ART_ACCURACITY) {
			if (rn2(10)) broken = 0;
		    }
		    if (obj->oartifact == ART_SLOW_DIFFUSION) {
			if (rn2(5)) broken = 0;
		    }
		    if (obj->oartifact == ART_LAST_LIKE_FOREVER) {
			if (rn2(25)) broken = 0;
		    }

		    if (objects[otyp].oc_material == MT_BAMBOO && broken && !rn2(4)) broken = 0;

		    if (objects[otyp].oc_material == MT_LEAD && broken && !rn2(4)) broken = 0;
		    if (otyp == DART_OF_DISINTEGRATION && rn2(10) ) broken = 1;

			/* Due to segfaults and stuff when trying to make this work in other functions, I'm just deciding that
			 * any thoroughly eroded stuff you throw will generally be destroyed much more often. --Amy */
		    if ((obj->oeroded == MAX_ERODE || obj->oeroded2 == MAX_ERODE) && !rn2(25) && !hard_to_destruct(obj)) broken = 1;

			/* artifact that sets the mulching rate to exactly 10%, regardless of other calculations */
		    if (obj->oartifact == ART_ACTUALLY_THE_MATTER_COMES_) {
			if (!rn2(10)) broken = 1;
			else broken = 0;
		    }

		    /* mulch nastytrap: ammo breaks unconditionally no matter what --Amy */
		    if (MulchBug || u.uprops[MULCH_BUG].extrinsic || have_mulchstone() || autismweaponcheck(ART_PIN_EM_ONCE) ) broken = 1;

		    if (obj && obj->oartifact == ART_HOUCHOU) broken = 1;
		    if (obj && obj->oartifact == ART_VORPALITY_TEMPTATION && u.vorpalitytempt >= 100) broken = 1;

		    if (broken) {
			if (*u.ushops)
			    check_shop_obj(obj, bhitpos.x,bhitpos.y, TRUE);
			/*
			 * Thrown grenades and explosive ammo used with the
			 * relevant launcher explode rather than simply
			 * breaking.
			 */
			if ((thrown == 1 || thrown == 2) && is_grenade(obj)) {
			    grenade_explode(obj, bhitpos.x, bhitpos.y, TRUE, 0);
			} else if (ammo_and_launcher(obj, launcher) && (objects[obj->otyp].oc_dir & EXPLOSION)) {
				if (cansee(bhitpos.x,bhitpos.y)) 
					pline("%s explodes in a ball of fire!", Doname2(obj));
				else You_hear("an explosion"); /* Amy note: we do not add an exclamation mark here on purpose */

				explode(bhitpos.x, bhitpos.y, ZT_SPELL(ZT_FIRE), d(3,8), WEAPON_CLASS, EXPL_FIERY);
				u.cnd_gunpowderused++;

				if (obj && obj->otyp == MINI_NUKE) fatman_explosion(bhitpos.x, bhitpos.y, obj);

				obfree(obj, (struct obj *)0);
			} else {
				obfree(obj, (struct obj *)0);
				u.cnd_ammomulched++; /* known problem: bullets can also run this code --Amy */
				if (RngeMulchingDisplay || (uarm && uarm->oartifact == ART_SEE_THE_MULCH_STATE) ) Your("projectile has mulched.");
			}
			return 1;
		    }
		}
		/* ceramic stuff dulls in melee, but we want ceramic missiles to dull too --Amy */
		if (obj && objects[obj->otyp].oc_material == MT_CERAMIC && !rn2(10) && obj->spe > -10) {
			obj->spe--;
		}

		if (obj && obj->oartifact == ART_CHA_SHATTER && !rn2(3) && obj->spe > -20) {
			obj->spe--;
		}

		passive_obj(mon, obj, (struct attack *)0);
	    } else {
		tmiss(obj, mon);
	    }

	} else if (otyp == HEAVY_IRON_BALL) {
	    exercise(A_STR, TRUE);
	    if (tmp >= dieroll) {
		int was_swallowed = guaranteed_hit;

		exercise(A_DEX, TRUE);
		ranged_thorns(mon);
		if (!hmon(mon,obj,thrown?thrown:3,dieroll)) {         /* mon killed */
		    if (was_swallowed && !u.uswallow && obj == uball)
			return 1;	/* already did placebc() */
		}
	    } else {
		tmiss(obj, mon);
	    }

	} else if (otyp == BOULDER) {
	    exercise(A_STR, TRUE);
	    if (tmp >= dieroll) {
		exercise(A_DEX, TRUE);
		ranged_thorns(mon);
		(void) hmon(mon, obj, thrown ? thrown : 3, dieroll);

		if (obj) {
			int bouldersplinterchance = 10;
			if (obj->oartifact) bouldersplinterchance *= 10;
			if (obj->cursed) bouldersplinterchance /= 2;
			if (obj->blessed) bouldersplinterchance *= 3;

			if (!PlayerCannotUseSkills) {
				switch (P_SKILL(P_BOULDER_THROWING)) {
					default: break;
					case P_BASIC: bouldersplinterchance *= 6; bouldersplinterchance /= 5; break;
					case P_SKILLED: bouldersplinterchance *= 7; bouldersplinterchance /= 5; break;
					case P_EXPERT: bouldersplinterchance *= 8; bouldersplinterchance /= 5; break;
					case P_MASTER: bouldersplinterchance *= 9; bouldersplinterchance /= 5; break;
					case P_GRAND_MASTER: bouldersplinterchance *= 10; bouldersplinterchance /= 5; break;
					case P_SUPREME_MASTER: bouldersplinterchance *= 11; bouldersplinterchance /= 5; break;
				}
			}

			if (!rn2(bouldersplinterchance)) {
				pline_The("boulder shatters into fragments!");
				fracture_rock(obj);
			}
		}

	    } else {
		tmiss(obj, mon);
	    }

	} else if ((otyp == EGG || otyp == CREAM_PIE) &&
		(guaranteed_hit || ACURR(A_DEX) > rnd(25) || (tmp >= rnd(20) && !PlayersRaysAreInaccurate) )) { /* F this stupidity. Sorry. --Amy */

		ranged_thorns(mon);
		(void) hmon(mon, obj, thrown?thrown:3,dieroll);
		if (issegfaulter && otyp == SEGFAULT_VENOM && !rn2(5)) { /* segfault panic! */
			u.segfaultpanic = TRUE;
		} else if (obj->oartifact == ART_DO_NOT_THROW_ME) { /* uh-oh... you really messed up big time there. */
			u.segfaultpanic = TRUE;
		}

	    return 1;	/* hmon used it up */

	} else if (obj->oclass == POTION_CLASS &&
		(guaranteed_hit || ACURR(A_DEX) > rnd(25) || (tmp >= rnd(20) && !PlayersRaysAreInaccurate) )) { /* The damn things missed way too often. */
	    ranged_thorns(mon);
	    potionhit(mon, obj, TRUE);
	    return 1;

	} else if (!polearming && (befriend_with_obj(mon->data, obj) || (obj->oclass == FOOD_CLASS && mon->egotype_domestic) || (otyp == KELP_FROND && mon->egotype_petty) || (mon->mtame && dogfood(mon, obj) <= ACCFOOD)) ) {
befriended:

		if (tamedog(mon, obj, FALSE)) return 1; /* obj is gone */
		else {
			/* not tmiss(), which angers non-tame monsters */
			miss(xname(obj), mon);
			mon->msleeping = 0;
			mon->mstrategy &= ~STRAT_WAITMASK;
		}
	} else if (guaranteed_hit) {
	    /* this assumes that guaranteed_hit is due to swallowing */
	    wakeup(mon);
	    ranged_thorns(mon);
	    if (obj->otyp == CORPSE && touch_petrifies(&mons[obj->corpsenm]) && !rn2(4)) {
		if (is_animal(u.ustuck->data)) {
			minstapetrify(u.ustuck, TRUE);
			/* Don't leave a cockatrice corpse available in a statue */
			if (!u.uswallow) {
				delobj(obj);
				return 1;
			}
	    	}
	    }
	    pline("%s into %s %s.",
		Tobjnam(obj, "vanish"), s_suffix(mon_nam(mon)),
		is_animal(u.ustuck->data) ? "entrails" : "currents");
	} else {
	    tmiss(obj, mon);
	}
	return 0;
}

STATIC_OVL int
gem_accept(mon, obj)
register struct monst *mon;
register struct obj *obj;
{
	char buf[BUFSZ];
	boolean is_buddy = sgn(mon->data->maligntyp) == sgn(u.ualign.type);
	if (mon->data == &mons[PM_MOLOCH_ALIGNED_UNICORN]) is_buddy = FALSE;
	boolean is_gem = objects[obj->otyp].oc_material == MT_GEMSTONE;
	int ret = 0;
	static NEARDATA const char nogood[] = " is not interested in your junk.";
	static NEARDATA const char acceptgift[] = " accepts your gift.";
	static NEARDATA const char maybeluck[] = " hesitatingly";
	static NEARDATA const char noluck[] = " graciously";
	static NEARDATA const char addluck[] = " gratefully";

	strcpy(buf,Monnam(mon));
	if (!mon->mfrenzied) {
		mon->mpeaceful = 1;
		mon->mavenge = 0;
	}

	/* Luck boosts used to be way too high. Thus I lowered them. --Amy */

	/* object properly identified */
	if(obj->dknown && objects[obj->otyp].oc_name_known) {
		if(is_gem) {
			if(is_buddy) {
				strcat(buf,addluck);
				if (!rn2(isfriday ? 10 : issoviet ? 2 : 3)) change_luck(issoviet ? 4 : rnd(4) );
				if (issoviet) pline("Vy, bezuslovno, povezlo, potomu chto eta igra yavlyayetsya nesbalansirovannym, kak chert!");
			} else {
				strcat(buf,maybeluck);
				if (!rn2(isfriday ? 10 : issoviet ? 3 : 5)) change_luck(issoviet ? 2 : rnd(2) );
				if (issoviet) pline("Kto-to reshil, chto vy dolzhny poluchit' tonny udachi besplatno!");
			}
		} else {
			strcat(buf,nogood);
			goto nopick;
		}
	/* making guesses */
	} else if(obj->onamelth || objects[obj->otyp].oc_uname) {
		if(is_gem) {
			if(is_buddy) {
				strcat(buf,addluck);
				if (!rn2(isfriday ? 10 : issoviet ? 2 : 3)) change_luck(issoviet ? 2 : rnd(2) );
				if (issoviet) pline("Vy, bezuslovno, povezlo, potomu chto eta igra yavlyayetsya nesbalansirovannym, kak chert!");
			} else {
				strcat(buf,maybeluck);
				if (!rn2(isfriday ? 10 : issoviet ? 3 : 5)) change_luck(issoviet ? 2 : rnd(2) );
				if (issoviet) pline("Kto-to reshil, chto vy dolzhny poluchit' tonny udachi besplatno!");
			}
		} else {
			strcat(buf,nogood);
			goto nopick;
		}
	/* value completely unknown to @ */
	} else {
		if(is_gem) {
			if(is_buddy) {
				strcat(buf,addluck);
				if (!rn2(isfriday ? 10 : issoviet ? 2 : 3)) change_luck(1);
			} else {
				strcat(buf,maybeluck);
				if (!rn2(isfriday ? 10 : issoviet ? 3 : 5)) change_luck(1);
			}
		} else {
			strcat(buf,noluck);
		}
	}
	strcat(buf,acceptgift);
	if(*u.ushops) check_shop_obj(obj, mon->mx, mon->my, TRUE);
	if (!issoviet && rn2(2)) (void) mpickobj(mon, obj, FALSE);	/* may merge and free obj */
	else 	obfree(obj, (struct obj *)0); /* definitely frees obj, nerf by Amy */
	ret = 1;

nopick:
	if(!Blind) pline("%s", buf);
	if (!tele_restrict(mon)) (void) rloc(mon, FALSE);
	return(ret);
}

/*
 * Comments about the restructuring of the old breaks() routine.
 *
 * There are now three distinct phases to object breaking:
 *     breaktest() - which makes the check/decision about whether the
 *                   object is going to break.
 *     breakmsg()  - which outputs a message about the breakage,
 *                   appropriate for that particular object. Should
 *                   only be called after a positve breaktest().
 *                   on the object and, if it going to be called,
 *                   it must be called before calling breakobj().
 *                   Calling breakmsg() is optional.
 *     breakobj()  - which actually does the breakage and the side-effects
 *                   of breaking that particular object. This should
 *                   only be called after a positive breaktest() on the
 *                   object.
 *
 * Each of the above routines is currently static to this source module.
 * There are two routines callable from outside this source module which
 * perform the routines above in the correct sequence.
 *
 *   hero_breaks() - called when an object is to be broken as a result
 *                   of something that the hero has done. (throwing it,
 *                   kicking it, etc.)
 *   breaks()      - called when an object is to be broken for some
 *                   reason other than the hero doing something to it.
 */

/*
 * The hero causes breakage of an object (throwing, dropping it, etc.)
 * Return 0 if the object didn't break, 1 if the object broke.
 */
int
hero_breaks(obj, x, y, from_invent)
struct obj *obj;
xchar x, y;		/* object location (ox, oy may not be right) */
boolean from_invent;	/* thrown or dropped by player; maybe on shop bill */
{
	boolean in_view = !Blind;
	if (!breaktest(obj)) return 0;
	breakmsg(obj, in_view);
	breakobj(obj, x, y, TRUE, from_invent);
	return 1;
}

/*
 * The object is going to break for a reason other than the hero doing
 * something to it.
 * Return 0 if the object doesn't break, 1 if the object broke.
 */
int
breaks(obj, x, y)
struct obj *obj;
xchar x, y;		/* object location (ox, oy may not be right) */
{
	boolean in_view = Blind ? FALSE : cansee(x, y);

	if (!breaktest(obj)) return 0;
	breakmsg(obj, in_view);
	breakobj(obj, x, y, FALSE, FALSE);
	return 1;
}

/*
 * Unconditionally break an object. Assumes all resistance checks
 * and break messages have been delivered prior to getting here.
 */
STATIC_OVL void
breakobj(obj, x, y, hero_caused, from_invent)
struct obj *obj;
xchar x, y;		/* object location (ox, oy may not be right) */
boolean hero_caused;	/* is this the hero's fault? */
boolean from_invent;
{
	if (itemhasappearance(obj, APP_POTION_RESERVATROL) && isok(x, y)) {
		(void) create_gas_cloud(x, y, 3+bcsign(obj), 8+4*bcsign(obj));
		You("smell chemicals.");
	}
	if (itemhasappearance(obj, APP_POTION_NITROGLYCERIN) && isok(x, y)) {
		struct obj *dynamite;
		dynamite = mksobj_at(STICK_OF_DYNAMITE, x, y, TRUE, FALSE, FALSE);
		if (dynamite) {
			if (dynamite->otyp != STICK_OF_DYNAMITE) delobj(dynamite);
			else {
				dynamite->dynamitekaboom = 1;
				dynamite->quan = 1;
				dynamite->owt = weight(dynamite);
				attach_bomb_blow_timeout(dynamite, 0, 0);
				run_timers();
			}
		}

	}

	if (obj->oartifact == ART_BUNDLEWING && isok(x, y)) { /* sleep rays in all directions --Amy */
		buzz(13, 6, x, y, -1, 0);
		buzz(13, 6, x, y, 1, 0);
		buzz(13, 6, x, y, -1, 1);
		buzz(13, 6, x, y, 1, 1);
		buzz(13, 6, x, y, 0, 1);
		buzz(13, 6, x, y, -1, -1);
		buzz(13, 6, x, y, 1, -1);
		buzz(13, 6, x, y, 0, -1);
	}

	int am;
	if (IS_ALTAR(levl[x][y].typ))
	    am = levl[x][y].altarmask & AM_MASK;
	else
	    am = AM_NONE;
	switch (obj->oclass == POTION_CLASS ? POT_WATER : obj->otyp) {
		case MIRROR:
			if (hero_caused)
			    change_luck(-2);
			break;
		case POT_WATER:		/* really, all potions */
			if (obj->otyp == POT_OIL && obj->lamplit) {
			    splatter_burning_oil(x,y);
			} else if ((obj->otyp == POT_VAMPIRE_BLOOD ||
				   obj->otyp == POT_BLOOD) &&
				   am != AM_CHAOTIC &&
				   am != AM_NONE) {
			    /* ALI: If blood is spilt on a lawful or
			     * neutral altar the effect is similar to
			     * human sacrifice. There's no effect on
			     * chaotic or unaligned altars since it is
			     * not sufficient to summon a demon.
			     */
			    if (hero_caused) {
				/* Regardless of your race/alignment etc.
				 * Lawful and neutral gods really _dont_
				 * like vampire or (presumed) human blood
				 * on their altars.
				 */
				pline("You'll regret this infamous offense!");
				exercise(A_WIS, FALSE);
			    }
			    /* curse the lawful/neutral altar */
			    pline_The("altar is stained with blood.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vy nechisto yeretikom, pravoslavnaya tserkov' progonit vas!" : "Klatsch.");
			    if (!Is_astralevel(&u.uz))
				levl[x][y].altarmask = AM_CHAOTIC;
			    angry_priest();
			} else if (distu(x,y) <= 2) {
			    if (!breathless(youmonst.data) || haseyes(youmonst.data)) {
				if (obj->otyp != POT_WATER) {
					if (!breathless(youmonst.data))
			    		     /* [what about "familiar odor" when known?] */
					    You("smell a peculiar odor...");
					else {
					    int numeyes = eyecount(youmonst.data);
					    Your("%s water%s.",
						 (numeyes == 1) ? body_part(EYE) :
							makeplural(body_part(EYE)),
						 (numeyes == 1) ? "s" : "");
					}
				}
				potionbreathe(obj);
			    }
			}
			/* monster breathing isn't handled... [yet?] */
			break;
		case EGG:
			/* breaking your own eggs is bad luck */
			if (hero_caused && obj->spe && obj->corpsenm >= LOW_PM)
			    change_luck((schar) -min(obj->quan, 5L));
			break;
	}
	if (hero_caused) {
	    if (from_invent) {
		if (*u.ushops)
			check_shop_obj(obj, x, y, TRUE);
	    } else if (!obj->no_charge && costly_spot(x, y)) {
		/* it is assumed that the obj is a floor-object */
		char *o_shop = in_rooms(x, y, SHOPBASE);
		struct monst *shkp = shop_keeper(*o_shop);

		if (shkp) {		/* (implies *o_shop != '\0') */
		    static NEARDATA long lastmovetime = 0L;
		    static NEARDATA boolean peaceful_shk = FALSE;
		    /*  We want to base shk actions on her peacefulness
			at start of this turn, so that "simultaneous"
			multiple breakage isn't drastically worse than
			single breakage.  (ought to be done via ESHK)  */
		    if (moves != lastmovetime)
			peaceful_shk = shkp->mpeaceful;
		    if (stolen_value(obj, x, y, peaceful_shk, FALSE, TRUE) > 0L &&
			(*o_shop != u.ushops[0] || !inside_shop(u.ux, u.uy)) &&
			moves != lastmovetime) make_angry_shk(shkp, x, y);
		    lastmovetime = moves;
		}
	    }
	}
	if (obj == uball) unpunish();
	if (obj == uchain) unpunish();
	delobj(obj);
}

/*
 * Check to see if obj is going to break, but don't actually break it.
 * Return 0 if the object isn't going to break, 1 if it is.
 * Amy edit: artifacts no longer completely immune, muahahahaha!
 * also, if the item is vitric, artifact versions should have a considerable chance of breaking too
 */
boolean
breaktest(obj)
struct obj *obj;
{
	if (is_vitric(obj)) {
		if (obj_resists(obj, 1, 33)) return 0;
	} else {
		if (obj_resists(obj, 1, 99)) return 0;
	}
	if ((objects[obj->otyp].oc_material == MT_GLASS || objects[obj->otyp].oc_material == MT_OBSIDIAN || is_vitric(obj)) && (!obj->oartifact || is_vitric(obj) || !rn2(10)) &&
		obj->oclass != GEM_CLASS)
	    return 1;
	switch (obj->oclass == POTION_CLASS ? POT_WATER : obj->otyp) {
		case EXPENSIVE_CAMERA:
		case POT_WATER:		/* really, all potions */
		case EGG:
		case CREAM_PIE:
		case MELON:
		case ACID_VENOM:
		case TAIL_SPIKES:
		case PHYSIO_VENOM:
		case ICE_BLOCK:
		case BLINDING_VENOM:
		case SEGFAULT_VENOM:
		case FAERIE_FLOSS_RHING:
			return 1;
		default:
			return 0;
	}
}

STATIC_OVL void
breakmsg(obj, in_view)
struct obj *obj;
boolean in_view;
{
	const char *to_pieces;

	to_pieces = "";
	switch (obj->oclass == POTION_CLASS ? POT_WATER : obj->otyp) {
		default: /* glass or crystal wand */
		    if (obj->oclass != WAND_CLASS)
			/*impossible("breaking odd object?");*/
			pline(FunnyHallu ? "Shards, lots of shards!" : "An odd object broke!"); /*quartz rings for example*/
		case CRYSTAL_PLATE_MAIL:
		case LENSES:
		case MIRROR:
		case CRYSTAL_BALL:
		case ORB_OF_CHARGING:
		case ORB_OF_ENCHANTMENT:
		case ORB_OF_DESTRUCTION:
		case EXPENSIVE_CAMERA:
			to_pieces = " into a thousand pieces";
			/*FALLTHRU*/
		case POT_WATER:		/* really, all potions */
			if (!in_view)
			    You_hear("%s shatter!", something);
			else
			    pline("%s shatter%s%s!", Doname2(obj),
				(obj->quan==1) ? "s" : "", to_pieces);
			break;
		case EGG:
		case MELON:
			pline("Splat!");
			break;
		case CREAM_PIE:
			if (in_view) pline("What a mess!");
			break;
		case ACID_VENOM:
		case TAIL_SPIKES:
		case PHYSIO_VENOM:
		case ICE_BLOCK:
		case BLINDING_VENOM:
		case SEGFAULT_VENOM:
		case FAERIE_FLOSS_RHING:
			pline("Splash!");
			break;
	}
}

STATIC_OVL int
throw_gold(obj)
struct obj *obj;
{
	int range, odx, ody;
#ifndef GOLDOBJ
	long zorks = obj->quan;
#endif
	register struct monst *mon;

	if(!u.dx && !u.dy && !u.dz) {
#ifndef GOLDOBJ
		u.ugold += obj->quan;
		flags.botl = 1;
		dealloc_obj(obj);
#endif
		You("cannot throw gold at yourself.");
		return(0);
	}
#ifdef GOLDOBJ
        freeinv(obj);
#endif
	if(u.uswallow) {
		pline(is_animal(u.ustuck->data) ?
			"%s in the %s's entrails." : "%s into %s.",
#ifndef GOLDOBJ
			"The gold disappears", mon_nam(u.ustuck));
		u.ustuck->mgold += zorks;
		dealloc_obj(obj);
#else
			"The money disappears", mon_nam(u.ustuck));
		add_to_minv(u.ustuck, obj);
#endif
		return(1);
	}

	if(u.dz) {
		if (u.dz < 0 && !Is_airlevel(&u.uz) &&
					!Underwater && !Is_waterlevel(&u.uz)) {
	pline_The("gold hits the %s, then falls back on top of your %s.",
		    ceiling(u.ux,u.uy), body_part(HEAD));
		    /* some self damage? */
		    if(uarmh) pline("Fortunately, you are wearing a helmet!");
		}
		bhitpos.x = u.ux;
		bhitpos.y = u.uy;
	} else {
		/* consistent with range for normal objects */
		range = (int)((ACURRSTR)/2 - obj->owt/40);

		/* see if the gold has a place to move into */
		odx = u.ux + u.dx;
		ody = u.uy + u.dy;
		if(!ZAP_POS(levl[odx][ody].typ) || closed_door(odx, ody)) {
			bhitpos.x = u.ux;
			bhitpos.y = u.uy;
		} else {
			mon = bhit(u.dx, u.dy, range, THROWN_WEAPON,
				   (int (*)(MONST_P,OBJ_P))0,
				   (int (*)(OBJ_P,OBJ_P))0,
				   &obj, FALSE);
			if (!obj)
			    return 1;
			if(mon) {
			    if (ghitm(mon, obj))	/* was it caught? */
				return 1;
			} else {
			    if(ship_object(obj, bhitpos.x, bhitpos.y, FALSE))
				return 1;
			}
		}
	}

	if(flooreffects(obj,bhitpos.x,bhitpos.y,"fall")) return(1);
	if(u.dz > 0)
		pline_The("gold hits the %s.", surface(bhitpos.x,bhitpos.y));
	place_object(obj,bhitpos.x,bhitpos.y);
	if(*u.ushops) sellobj(obj, bhitpos.x, bhitpos.y);
	stackobj(obj);
	newsym(bhitpos.x,bhitpos.y);
	return(1);
}

/*dothrow.c*/
