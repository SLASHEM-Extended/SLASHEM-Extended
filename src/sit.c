/*	SCCS Id: @(#)sit.c	3.4	2002/09/21	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"

/* categories whose names don't come from OBJ_NAME(objects[type]) */
#define PN_POLEARMS		(-1)
#define PN_SABER		(-2)
#define PN_HAMMER		(-3)
#define PN_WHIP			(-4)
#define PN_PADDLE		(-5)
#define PN_FIREARMS		(-6)
#define PN_ATTACK_SPELL		(-7)
#define PN_HEALING_SPELL	(-8)
#define PN_DIVINATION_SPELL	(-9)
#define PN_ENCHANTMENT_SPELL	(-10)
#define PN_PROTECTION_SPELL	(-11)
#define PN_BODY_SPELL		(-12)
#define PN_MATTER_SPELL		(-13)
#define PN_BARE_HANDED		(-14)
#define PN_HIGH_HEELS		(-15)
#define PN_MARTIAL_ARTS		(-16)
#define PN_RIDING		(-17)
#define PN_TWO_WEAPONS		(-18)
#ifdef LIGHTSABERS
#define PN_LIGHTSABER		(-19)
#endif

static const char all_count[] = { ALLOW_COUNT, ALL_CLASSES, 0 };

#ifndef OVLB

STATIC_DCL NEARDATA const short skill_names_indices[];
STATIC_DCL NEARDATA const char *odd_skill_names[];

#else	/* OVLB */

/* KMH, balance patch -- updated */
STATIC_OVL NEARDATA const short skill_names_indices[P_NUM_SKILLS] = {
	0,                DAGGER,         KNIFE,        AXE,
	PICK_AXE,         SHORT_SWORD,    BROADSWORD,   LONG_SWORD,
	TWO_HANDED_SWORD, SCIMITAR,       PN_SABER,     CLUB,
	PN_PADDLE,        MACE,           MORNING_STAR,   FLAIL,
	PN_HAMMER,        QUARTERSTAFF,   PN_POLEARMS,  SPEAR,
	JAVELIN,          TRIDENT,        LANCE,        BOW,
	SLING,            PN_FIREARMS,    CROSSBOW,       DART,
	SHURIKEN,         BOOMERANG,      PN_WHIP,      UNICORN_HORN,
#ifdef LIGHTSABERS
	PN_LIGHTSABER,
#endif
	PN_ATTACK_SPELL,     PN_HEALING_SPELL,
	PN_DIVINATION_SPELL, PN_ENCHANTMENT_SPELL,
	PN_PROTECTION_SPELL,            PN_BODY_SPELL,
	PN_MATTER_SPELL,
	PN_BARE_HANDED, 	PN_HIGH_HEELS,	PN_MARTIAL_ARTS, 
	PN_TWO_WEAPONS,
#ifdef STEED
	PN_RIDING,
#endif
};


STATIC_OVL NEARDATA const char * const odd_skill_names[] = {
    "no skill",
    "polearms",
    "saber",
    "hammer",
    "whip",
    "paddle",
    "firearms",
    "attack spells",
    "healing spells",
    "divination spells",
    "enchantment spells",
    "protection spells",
    "body spells",
    "matter spells",
    "bare-handed combat",
    "high heels",
    "martial arts",
    "riding",
    "two-handed combat",
#ifdef LIGHTSABERS
    "lightsaber"
#endif
};

#endif	/* OVLB */

#define P_NAME(type) (skill_names_indices[type] > 0 ? \
		      OBJ_NAME(objects[skill_names_indices[type]]) : \
			odd_skill_names[-skill_names_indices[type]])

void
take_gold()
{
#ifndef GOLDOBJ
	if (u.ugold <= 0)  {
		You_feel("a strange sensation.");
	} else {
		You("notice you have no gold!");
		u.ugold = 0;
		flags.botl = 1;
	}
#else
        struct obj *otmp, *nobj;
	int lost_money = 0;
	for (otmp = invent; otmp; otmp = nobj) {
		nobj = otmp->nobj;
		if (otmp->oclass == COIN_CLASS) {
			lost_money = 1;
			delobj(otmp);
		}
	}
	if (!lost_money)  {
		You_feel("a strange sensation.");
	} else {
		You("notice you have no money!");
		flags.botl = 1;
	}
#endif
}

int
dosit()
{

	register struct obj *otmp;

	if (MenuBug || u.uprops[MENU_LOST].extrinsic || have_menubugstone()) {
	pline("The sit command is currently unavailable!");
	display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

	static const char sit_message[] = "sit on the %s.";
	register struct trap *trap;
	register int typ = levl[u.ux][u.uy].typ;


#ifdef STEED
	if (u.usteed) {
	    You("are already sitting on %s.", mon_nam(u.usteed));
	    return (0);
	}
#endif

	if(!can_reach_floor())	{
	    if (Levitation)
		You("tumble in place.");
	    else
		You("are sitting on air.");
	    return 0;
	} else if (is_pool(u.ux, u.uy) && !Underwater) {  /* water walking */
	    goto in_water;
	}

	if(OBJ_AT(u.ux, u.uy)) {
	    register struct obj *obj;

	    obj = level.objects[u.ux][u.uy];
	    You("sit on %s.", the(xname(obj)));
	    if (!(Is_box(obj) || objects[obj->otyp].oc_material == CLOTH))
		pline("It's not very comfortable...");

	} else if ((trap = t_at(u.ux, u.uy)) != 0 ||
		   (u.utrap && (u.utraptype >= TT_LAVA))) {

	    if (u.utrap) {
		exercise(A_WIS, FALSE);	/* you're getting stuck longer */
		if(u.utraptype == TT_BEARTRAP) {
		    You_cant("sit down with your %s in the bear trap.", body_part(FOOT));
		    u.utrap++;
	        } else if(u.utraptype == TT_PIT) {
		    if(trap->ttyp == SPIKED_PIT) {
			You("sit down on a spike.  Ouch!");
			losehp(1, "sitting on an iron spike", KILLED_BY);
			exercise(A_STR, FALSE);
		    } else
			You("sit down in the pit.");
		    u.utrap += rn2(5);
		} else if(u.utraptype == TT_WEB) {
		    You("sit in the spider web and get entangled further!");
		    u.utrap += rn1(10, 5);
		} else if(u.utraptype == TT_GLUE) {
		    You("immerse yourself with glue!");
		    u.utrap += rn1(10, 5);
		} else if(u.utraptype == TT_LAVA) {
		    /* Must have fire resistance or they'd be dead already */
		    You("sit in the lava!");
		    u.utrap += rnd(4);
		    losehp(d(2,10), "sitting in lava", KILLED_BY);
		} else if(u.utraptype == TT_INFLOOR) {
		    You_cant("maneuver to sit!");
		    u.utrap++;
		}
	    } else {
	        You("sit down.");
		dotrap(trap, 0);
	    }
	} else if(Underwater || Is_waterlevel(&u.uz)) {
	    if (Is_waterlevel(&u.uz))
		There("are no cushions floating nearby.");
	    else
		You("sit down on the muddy bottom.");
	} else if(is_pool(u.ux, u.uy)) {
 in_water:
	    You("sit in the water.");
	    if (!rn2(10) && uarm)
		(void) rust_dmg(uarm, "armor", 1, TRUE, &youmonst);
	    else if (!rn2(10) && uarmf && uarmf->otyp != WATER_WALKING_BOOTS)
		(void) rust_dmg(uarm, "armor", 1, TRUE, &youmonst);
#ifdef SINKS
	} else if(IS_SINK(typ)) {

	    You(sit_message, defsyms[S_sink].explanation);
	    Your("%s gets wet.", humanoid(youmonst.data) ? "rump" : "underside");
	} else if(IS_TOILET(typ)) {
	    You(sit_message, defsyms[S_toilet].explanation);
	    if ((!Sick) && (u.uhs > 0)) You("don't have to go...");
	    else {
			if (Role_if(PM_BARBARIAN) || Role_if(PM_CAVEMAN)) You("miss...");
			else You("grunt.");
			/* Based on real life experience (urgh) this doesn't always instantly cure sickness. --Amy */
			if (Sick && !rn2(3) ) make_sick(0L, (char *)0, TRUE, SICK_VOMITABLE);
			else if (Sick && !rn2(10) ) make_sick(0L, (char *)0, TRUE, SICK_ALL);
			if (u.uhs == 0) morehungry(rn2(400)+200);
	    }
#endif
	} else if(IS_ALTAR(typ)) {

	    You(sit_message, defsyms[S_altar].explanation);
	    altar_wrath(u.ux, u.uy);

	} else if(IS_GRAVE(typ)) {

	    You(sit_message, defsyms[S_grave].explanation);

	} else if(typ == STAIRS) {

	    You(sit_message, "stairs");

	} else if(typ == LADDER) {

	    You(sit_message, "ladder");

	} else if (is_lava(u.ux, u.uy)) {

	    /* must be WWalking */
	    You(sit_message, "lava");
	    burn_away_slime();
	    if (likes_lava(youmonst.data) || (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "hot boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "goryachiye botinki") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "issiq chizilmasin") ) ) || (uamul && uamul->otyp == AMULET_OF_D_TYPE_EQUIPMENT) || (uarm && uarm->oartifact == ART_LAURA_CROFT_S_BATTLEWEAR) ) {
		pline_The("lava feels warm.");
		return 1;
	    }
	    pline_The("lava burns you!");
	    if (Slimed) {
	       pline("The slime is burned away!");
	       Slimed = 0;
	    }
	    losehp(d((Fire_resistance ? 2 : 10), 10),
		   "sitting on lava", KILLED_BY);

	} else if (is_ice(u.ux, u.uy)) {

	    You(sit_message, defsyms[S_ice].explanation);
	    if (!Cold_resistance) pline_The("ice feels cold.");

	} else if (typ == DRAWBRIDGE_DOWN) {

	    You(sit_message, "drawbridge");

	} else if(IS_THRONE(typ)) {

	    You(sit_message, defsyms[S_throne].explanation);
	    if (rnd(6) > 4)  {
		switch (rnd(20))  {
		    case 1:
			(void) adjattrib(rn2(A_MAX), -rn1(4,3), FALSE);
			losehp(rnd(10), "cursed throne", KILLED_BY_AN);
			break;
		    case 2:
			(void) adjattrib(rn2(A_MAX), 1, FALSE);
			break;
		    case 3:
			pline("A%s electric shock shoots through your body!",
			      (Shock_resistance) ? "n" : " massive");
			losehp(Shock_resistance ? rnd(6) : rnd(30),
			       "electric chair", KILLED_BY_AN);
			exercise(A_CON, FALSE);
			break;
		    case 4:
			You_feel("much, much better!");
			if (Upolyd) {
			    if (u.mh >= (u.mhmax - 5))  u.mhmax += 4;
			    u.mh = u.mhmax;
			}
			if(u.uhp >= (u.uhpmax - 5))  u.uhpmax += 4;
			u.uhp = u.uhpmax;
			make_blinded(0L,TRUE);
			make_sick(0L, (char *) 0, FALSE, SICK_ALL);
			heal_legs();
			flags.botl = 1;
			break;
		    case 5:
			take_gold();
			break;
		    case 6:
/* ------------===========STEPHEN WHITE'S NEW CODE============------------ */                                                
			if(u.uluck < 7) {
			    You_feel("your luck is changing.");
			    change_luck(5);
			} else	    makewish();
			break;
		    case 7:
			{
			register int cnt = rnd(10);

			pline("A voice echoes:");
			verbalize("Thy audience hath been summoned, %s!",
				  flags.female ? "Dame" : "Sire");
			while(cnt--)
			    (void) makemon(courtmon(), u.ux, u.uy, NO_MM_FLAGS);
			break;
			}
		    case 8:
			pline("A voice echoes:");
			verbalize("By thy Imperious order, %s...",
				  flags.female ? "Dame" : "Sire");
			do_genocide(5);	/* REALLY|ONTHRONE, see do_genocide() */
			break;
		    case 9:
			pline("A voice echoes:");
	verbalize("A curse upon thee for sitting upon this most holy throne!");
			if (Luck > 0)  {
			    make_blinded(Blinded + rn1(100,250),TRUE);
			} else	    rndcurse();
			break;
		    case 10:
			if (Luck < 0 || (HSee_invisible & INTRINSIC))  {
				if (level.flags.nommap) {
					pline(
					"A terrible drone fills your head!");
					make_confused(HConfusion + rnd(30),
									FALSE);
				} else {
					pline("An image forms in your mind.");
					do_mapping();
				}
			} else  {
				Your("vision becomes clear.");
				HSee_invisible |= FROMOUTSIDE;
				newsym(u.ux, u.uy);
			}
			break;
		    case 11:
			if (Luck < 0)  {
			    You_feel("threatened.");
			    aggravate();
			} else  {

			    You_feel("a wrenching sensation.");
			    tele();		/* teleport him */
			}
			break;
		    case 12:
			You("are granted an insight!");
			if (invent) {
			    /* rn2(5) agrees w/seffects() */
			    identify_pack(rn2(5), 0);
			}
			break;
		    case 13:
			Your("mind turns into a pretzel!");
			make_confused(HConfusion + rn1(7,16),FALSE);
			break;
		    case 14:
			You("are granted some new skills!"); /* new effect that unrestricts skills --Amy */

			int acquiredskill;
			acquiredskill = 0;

			while (acquiredskill == 0) { /* ask the player what they want --Amy */

			if (P_RESTRICTED(P_DAGGER) && yn("Do you want to learn the dagger skill?")=='y') {
				    unrestrict_weapon_skill(P_DAGGER);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_KNIFE) && yn("Do you want to learn the knife skill?")=='y') {
				    unrestrict_weapon_skill(P_KNIFE);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_AXE) && yn("Do you want to learn the axe skill?")=='y') {
				    unrestrict_weapon_skill(P_AXE);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_PICK_AXE) && yn("Do you want to learn the pick-axe skill?")=='y') {
				    unrestrict_weapon_skill(P_PICK_AXE);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_SHORT_SWORD) && yn("Do you want to learn the short sword skill?")=='y') {
				    unrestrict_weapon_skill(P_SHORT_SWORD);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_BROAD_SWORD) && yn("Do you want to learn the broad sword skill?")=='y') {
				    unrestrict_weapon_skill(P_BROAD_SWORD);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_LONG_SWORD) && yn("Do you want to learn the long sword skill?")=='y') {
				    unrestrict_weapon_skill(P_LONG_SWORD);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_TWO_HANDED_SWORD) && yn("Do you want to learn the two-handed sword skill?")=='y') {
				    unrestrict_weapon_skill(P_TWO_HANDED_SWORD);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_SCIMITAR) && yn("Do you want to learn the scimitar skill?")=='y') {
				    unrestrict_weapon_skill(P_SCIMITAR);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_SABER) && yn("Do you want to learn the saber skill?")=='y') {
				    unrestrict_weapon_skill(P_SABER);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_CLUB) && yn("Do you want to learn the club skill?")=='y') {
				    unrestrict_weapon_skill(P_CLUB);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_PADDLE) && yn("Do you want to learn the paddle skill?")=='y') {
				    unrestrict_weapon_skill(P_PADDLE);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_MACE) && yn("Do you want to learn the mace skill?")=='y') {
				    unrestrict_weapon_skill(P_MACE);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_MORNING_STAR) && yn("Do you want to learn the morning star skill?")=='y') {
				    unrestrict_weapon_skill(P_MORNING_STAR);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_FLAIL) && yn("Do you want to learn the flail skill?")=='y') {
				    unrestrict_weapon_skill(P_FLAIL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_HAMMER) && yn("Do you want to learn the hammer skill?")=='y') {
				    unrestrict_weapon_skill(P_HAMMER);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_QUARTERSTAFF) && yn("Do you want to learn the quarterstaff skill?")=='y') {
				    unrestrict_weapon_skill(P_QUARTERSTAFF);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_POLEARMS) && yn("Do you want to learn the polearms skill?")=='y') {
				    unrestrict_weapon_skill(P_POLEARMS);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_SPEAR) && yn("Do you want to learn the spear skill?")=='y') {
				    unrestrict_weapon_skill(P_SPEAR);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_JAVELIN) && yn("Do you want to learn the javelin skill?")=='y') {
				    unrestrict_weapon_skill(P_JAVELIN);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_TRIDENT) && yn("Do you want to learn the trident skill?")=='y') {
				    unrestrict_weapon_skill(P_TRIDENT);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_LANCE) && yn("Do you want to learn the lance skill?")=='y') {
				    unrestrict_weapon_skill(P_LANCE);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_BOW) && yn("Do you want to learn the bow skill?")=='y') {
				    unrestrict_weapon_skill(P_BOW);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_SLING) && yn("Do you want to learn the sling skill?")=='y') {
				    unrestrict_weapon_skill(P_SLING);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_FIREARM) && yn("Do you want to learn the firearms skill?")=='y') {
				    unrestrict_weapon_skill(P_FIREARM);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_CROSSBOW) && yn("Do you want to learn the crossbow skill?")=='y') {
				    unrestrict_weapon_skill(P_CROSSBOW);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_DART) && yn("Do you want to learn the dart skill?")=='y') {
				    unrestrict_weapon_skill(P_DART);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_SHURIKEN) && yn("Do you want to learn the shuriken skill?")=='y') {
				    unrestrict_weapon_skill(P_SHURIKEN);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_BOOMERANG) && yn("Do you want to learn the boomerang skill?")=='y') {
				    unrestrict_weapon_skill(P_BOOMERANG);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_WHIP) && yn("Do you want to learn the whip skill?")=='y') {
				    unrestrict_weapon_skill(P_WHIP);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_UNICORN_HORN) && yn("Do you want to learn the unicorn horn skill?")=='y') {
				    unrestrict_weapon_skill(P_UNICORN_HORN);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_LIGHTSABER) && yn("Do you want to learn the lightsaber skill?")=='y') {
				    unrestrict_weapon_skill(P_LIGHTSABER);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_ATTACK_SPELL) && yn("Do you want to learn the attack spell skill?")=='y') {
				    unrestrict_weapon_skill(P_ATTACK_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_HEALING_SPELL) && yn("Do you want to learn the healing spell skill?")=='y') {
				    unrestrict_weapon_skill(P_HEALING_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_DIVINATION_SPELL) && yn("Do you want to learn the divination spell skill?")=='y') {
				    unrestrict_weapon_skill(P_DIVINATION_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_ENCHANTMENT_SPELL) && yn("Do you want to learn the enchantment spell skill?")=='y') {
				    unrestrict_weapon_skill(P_ENCHANTMENT_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_PROTECTION_SPELL) && yn("Do you want to learn the protection spell skill?")=='y') {
				    unrestrict_weapon_skill(P_PROTECTION_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_BODY_SPELL) && yn("Do you want to learn the body spell skill?")=='y') {
				    unrestrict_weapon_skill(P_BODY_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_MATTER_SPELL) && yn("Do you want to learn the matter spell skill?")=='y') {
				    unrestrict_weapon_skill(P_MATTER_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_RIDING) && yn("Do you want to learn the riding skill?")=='y') {
				    unrestrict_weapon_skill(P_RIDING);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_HIGH_HEELS) && yn("Do you want to learn the high heels skill?")=='y') {
				    unrestrict_weapon_skill(P_HIGH_HEELS);	acquiredskill = 1; }
			else if (yn("Do you want to learn no new skill at all?")=='y') {
				    acquiredskill = 1; }
			}
			pline("Check out what you got!");

			break;
		    case 15:
			pline("A voice echoes:");
			verbalize("Thou be cursed!");
			attrcurse();
			break;
		    case 16:
			pline("A voice echoes:");
			verbalize("Thou shall be punished!");
			punishx();
			break;
		    case 17:
			You_feel("like someone has touched your forehead...");

			int skillimprove = rnd(P_NUM_SKILLS);

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
				break;
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
				break;
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

			break;
		    case 18:
			{register int cnt = rnd(10);
			struct permonst *randmonstforspawn = rndmonst();

			while(cnt--)
			    (void) makemon(randmonstforspawn, u.ux, u.uy, NO_MM_FLAGS);
			pline("A voice echoes:");
			verbalize("Leave me alone, stupid %s", randmonstforspawn->mname);
			break;
			}
		    case 19:
			{register int cnt = rnd(10);
			int randmonstforspawn = rnd(68);
			if (randmonstforspawn == 35) randmonstforspawn = 53;

			while(cnt--)
			    (void) makemon(mkclass(randmonstforspawn,0), u.ux, u.uy, NO_MM_FLAGS);
			pline("A voice echoes:");
			verbalize("Oh, please help me! A horrible %s stole my sword! I'm nothing without it.", monexplain[randmonstforspawn]);
			break;
			}
		    case 20:
			{
			pline("You may fully identify an object!");

			otmp = getobj(all_count, "secure identify");

			if (!otmp) {
				pline("A feeling of loss comes over you.");
				break;
			}
			if (otmp) {
				makeknown(otmp->otyp);
				if (otmp->oartifact) discover_artifact((int)otmp->oartifact);
				otmp->known = otmp->dknown = otmp->bknown = otmp->rknown = 1;
				if (otmp->otyp == EGG && otmp->corpsenm != NON_PM)
				learn_egg_type(otmp->corpsenm);
				prinv((char *)0, otmp, 0L);
			}
			}
			break;

		    default:	impossible("throne effect");
				break;
		}
	    } else {
		if (is_prince(youmonst.data))
		    You_feel("very comfortable here.");
		else
		    You_feel("somehow out of place...");
	    }

	    if (!rn2(6) && IS_THRONE(levl[u.ux][u.uy].typ)) {
		/* may have teleported */
		levl[u.ux][u.uy].typ = ROOM;
		pline_The("throne vanishes in a puff of logic.");
		newsym(u.ux,u.uy);
	    }

	} else if (lays_eggs(youmonst.data)) {
		struct obj *uegg;

		if (!flags.female) {
			pline(Hallucination ? "You try to lay an egg, but instead you... okay let's not go there." : "Males can't lay eggs!");
			return 0;
		}

		if (u.uhunger < (int)objects[EGG].oc_nutrition) {
			You("don't have enough energy to lay an egg.");
			return 0;
		}

		uegg = mksobj(EGG, FALSE, FALSE);
		if (uegg) {
			uegg->spe = 1;
			uegg->quan = 1;
			uegg->owt = weight(uegg);
			uegg->corpsenm = egg_type_from_parent(u.umonnum, FALSE);
			uegg->known = uegg->dknown = 1;
			attach_egg_hatch_timeout(uegg);
			You("lay an egg.");
			dropy(uegg);
			stackobj(uegg);
			morehungry((int)objects[EGG].oc_nutrition);
		}
	} else if (u.uswallow)
		There("are no seats in here!");
	else
		pline("Having fun sitting on the %s?", surface(u.ux,u.uy));
	return(1);
}

void
skillcaploss()
{

	int skilltoreduce = rnd(P_NUM_SKILLS);
	int tryct;
	int i = 0;

	if (P_MAX_SKILL(skilltoreduce) == P_BASIC) {
		P_MAX_SKILL(skilltoreduce) = P_UNSKILLED;
		pline("You lose all knowledge of the %s skill!", P_NAME(skilltoreduce));
	} else if (!rn2(2) && P_MAX_SKILL(skilltoreduce) == P_SKILLED) {
		P_MAX_SKILL(skilltoreduce) = P_BASIC;
		pline("You lose some knowledge of the %s skill!", P_NAME(skilltoreduce));
	} else if (!rn2(4) && P_MAX_SKILL(skilltoreduce) == P_EXPERT) {
		P_MAX_SKILL(skilltoreduce) = P_SKILLED;
		pline("You lose some knowledge of the %s skill!", P_NAME(skilltoreduce));
	} else if (!rn2(10) && P_MAX_SKILL(skilltoreduce) == P_MASTER) {
		P_MAX_SKILL(skilltoreduce) = P_EXPERT;
		pline("You lose some knowledge of the %s skill!", P_NAME(skilltoreduce));
	} else if (!rn2(100) && P_MAX_SKILL(skilltoreduce) == P_GRAND_MASTER) {
		P_MAX_SKILL(skilltoreduce) = P_MASTER;
		pline("You lose some knowledge of the %s skill!", P_NAME(skilltoreduce));
	} else if (!rn2(200) && P_MAX_SKILL(skilltoreduce) == P_SUPREME_MASTER) {
		P_MAX_SKILL(skilltoreduce) = P_GRAND_MASTER;
		pline("You lose some knowledge of the %s skill!", P_NAME(skilltoreduce));
	}

	tryct = 400;

	while (u.skills_advanced && tryct && (P_SKILL(skilltoreduce) > P_MAX_SKILL(skilltoreduce)) ) {
		lose_weapon_skill(1);
		i++;
		tryct--;
	}

	while (i) {
		u.weapon_slots++;  /* because every skill up costs one slot --Amy */
		i--;
	}

	/* still higher than the cap? that probably means you started with some knowledge of the skill... */
	if (P_SKILL(skilltoreduce) > P_MAX_SKILL(skilltoreduce)) {
		P_SKILL(skilltoreduce) = P_MAX_SKILL(skilltoreduce);
		u.weapon_slots++;
	}

	return;

}

void
rndcurse()			/* curse a few inventory items at random! */
{
	int	nobj = 0;
	int	cnt, onum;
	struct	obj	*otmp;
	static const char mal_aura[] = "feel a malignant aura surround %s.";

	if (uwep && (uwep->oartifact == ART_MAGICBANE) && rn2(20)) {
	    You(mal_aura, "the magic-absorbing blade");
	    return;
	}

	if (Versus_curses && rn2(20)) { /* curse resistance, by Chris_ANG */
		pline("A malignant aura surrounds you but is absorbed by your magical shield!");
	    return;
	}

	else if(u.ukinghill && rn2(20)){
	    You(mal_aura, "the cursed treasure chest");
		otmp = 0;
		for(otmp = invent; otmp; otmp=otmp->nobj)
			if(otmp->oartifact == ART_TREASURY_OF_PROTEUS)
				break;
		if(!otmp) pline("Treasury not actually in inventory??");
		else if(otmp->blessed)
			unbless(otmp);
		else
			curse(otmp);
	    update_inventory();		
		return;
	}
	if(Antimagic) {
	    shieldeff(u.ux, u.uy);
	    You(mal_aura, "you");
	}

	for (otmp = invent; otmp; otmp = otmp->nobj) {
#ifdef GOLDOBJ
	    /* gold isn't subject to being cursed or blessed */
	    if (otmp->oclass == COIN_CLASS) continue;
#endif
	    nobj++;
	}
	if (nobj) {
	    for (cnt = rnd(6/((!!Antimagic) + (!!Half_spell_damage) + 1));
		 cnt > 0; cnt--)  {
		onum = rnd(nobj);
		for (otmp = invent; otmp; otmp = otmp->nobj) {
#ifdef GOLDOBJ
		    /* as above */
		    if (otmp->oclass == COIN_CLASS) continue;
#endif
		    if (--onum == 0) break;	/* found the target */
		}
		/* the !otmp case should never happen; picking an already
		   cursed item happens--avoid "resists" message in that case
		   Amy edit: only prime cursed items can't be cursed further
		   edit again: now they can, since we have even more severe curses */
		if (!otmp) continue;	/* next target */

		if(otmp->oartifact && spec_ability(otmp, SPFX_INTEL) &&
		   rn2(10) < 8) {
		    pline("%s!", Tobjnam(otmp, "resist"));
		    continue;
		}

		if (!stack_too_big(otmp)) {
		if(otmp->blessed)
			unbless(otmp);
		else
			curse(otmp);
		}
	    }
	    update_inventory();
	}

#ifdef STEED
	/* treat steed's saddle as extended part of hero's inventory */
	if (u.usteed && !rn2(50) &&
		(otmp = which_armor(u.usteed, W_SADDLE)) != 0) {
	    if (otmp->blessed)
		unbless(otmp);
	    else
		curse(otmp);
	    if (!Blind) {
		pline("%s %s %s.",
		      s_suffix(upstart(y_monnam(u.usteed))),
		      aobjnam(otmp, "glow"),
		      hcolor(otmp->cursed ? NH_BLACK : (const char *)"brown"));
		otmp->bknown = TRUE;
	    }
	}
#endif	/*STEED*/
}

void
attrcurse()			/* remove a random INTRINSIC ability */
{
	switch(rnd(191)) {
	case 1 : 
	case 2 : 
	case 3 : 
	case 4 : 
	case 5 : 
	case 6 : 
	case 7 : 
	case 8 : 
	case 9 : 
	case 10 : if (HFire_resistance & INTRINSIC) {
			HFire_resistance &= ~INTRINSIC;
			You_feel("warmer.");
		}
		if (HFire_resistance & TIMEOUT) {
			HFire_resistance &= ~TIMEOUT;
			You_feel("warmer.");
		}
		break;
	case 11 : 
	case 12 : 
	case 13 : 
	case 14 : 
	case 15 : 
	case 16 : if (HTeleportation & INTRINSIC) {
			HTeleportation &= ~INTRINSIC;
			You_feel("less jumpy.");
		}
		if (HTeleportation & TIMEOUT) {
			HTeleportation &= ~TIMEOUT;
			You_feel("less jumpy.");
		}
		break;
	case 17 : 
	case 18 : 
	case 19 : 
	case 20 : 
	case 21 : 
	case 22 : 
	case 23 : 
	case 24 : 
	case 25 : 
	case 26 : if (HPoison_resistance & INTRINSIC) {
			HPoison_resistance &= ~INTRINSIC;
			You_feel("a little sick!");
		}
		if (HPoison_resistance & TIMEOUT) {
			HPoison_resistance &= ~TIMEOUT;
			You_feel("a little sick!");
		}
		break;
	case 27 : 
	case 28 : 
	case 29 : 
	case 30 : 
	case 31 : 
	case 32 : 
	case 33 : 
	case 34 : 
	case 35 : 
	case 36 : if (HTelepat & INTRINSIC) {
			HTelepat &= ~INTRINSIC;
			if (Blind && !Blind_telepat)
			    see_monsters();	/* Can't sense mons anymore! */
			Your("senses fail!");
		}
		if (HTelepat & TIMEOUT) {
			HTelepat &= ~TIMEOUT;
			if (Blind && !Blind_telepat)
			    see_monsters();	/* Can't sense mons anymore! */
			Your("senses fail!");
		}
		break;
	case 37 : 
	case 38 : 
	case 39 : 
	case 40 : 
	case 41 : 
	case 42 : 
	case 43 : 
	case 44 : 
	case 45 : 
	case 46 : if (HCold_resistance & INTRINSIC) {
			HCold_resistance &= ~INTRINSIC;
			You_feel("cooler.");
		}
		if (HCold_resistance & TIMEOUT) {
			HCold_resistance &= ~TIMEOUT;
			You_feel("cooler.");
		}
		break;
	case 47 : 
	case 48 : 
	case 49 : 
	case 50 : 
	case 51 : 
	case 52 : 
	case 53 : 
	case 54 : 
	case 55 : 
	case 56 : if (HInvis & INTRINSIC) {
			HInvis &= ~INTRINSIC;
			You_feel("paranoid.");
		}
		if (HInvis & TIMEOUT) {
			HInvis &= ~TIMEOUT;
			You_feel("paranoid.");
		}
		break;
	case 57 : 
	case 58 : 
	case 59 : 
	case 60 : 
	case 61 : 
	case 62 : 
	case 63 : 
	case 64 : 
	case 65 : 
	case 66 : if (HSee_invisible & INTRINSIC) {
			HSee_invisible &= ~INTRINSIC;
			You("%s!", Hallucination ? "tawt you taw a puttie tat"
						: "thought you saw something");
		}
		if (HSee_invisible & TIMEOUT) {
			HSee_invisible &= ~TIMEOUT;
			You("%s!", Hallucination ? "tawt you taw a puttie tat"
						: "thought you saw something");
		}
		break;
	case 67 : 
	case 68 : 
	case 69 : 
	case 70 : 
	case 71 : 
	case 72 : 
	case 73 : 
	case 74 : 
	case 75 : 
	case 76 : if (HFast & INTRINSIC) {
			HFast &= ~INTRINSIC;
			You_feel("slower.");
		}
		if (HFast & TIMEOUT) {
			HFast &= ~TIMEOUT;
			You_feel("slower.");
		}
		break;
	case 77 : 
	case 78 : 
	case 79 : 
	case 80 : 
	case 81 : 
	case 82 : 
	case 83 : 
	case 84 : 
	case 85 : 
	case 86 : if (HStealth & INTRINSIC) {
			HStealth &= ~INTRINSIC;
			You_feel("clumsy.");
		}
		if (HStealth & TIMEOUT) {
			HStealth &= ~TIMEOUT;
			You_feel("clumsy.");
		}
		break;
	case 87: if (HProtection & INTRINSIC) {
			HProtection &= ~INTRINSIC;
			You_feel("vulnerable.");
		}
		if (HProtection & TIMEOUT) {
			HProtection &= ~TIMEOUT;
			You_feel("vulnerable.");
		}
		break;
	case 88 : 
	case 89 : 
	case 90 : 
	case 91 : 
	case 92: if (HAggravate_monster & INTRINSIC) {
			HAggravate_monster &= ~INTRINSIC;
			You_feel("less attractive.");
		}
		if (HAggravate_monster & TIMEOUT) {
			HAggravate_monster &= ~TIMEOUT;
			You_feel("less attractive.");
		}
		break;
	case 93 : 
	case 94 : 
	case 95 : 
	case 96 : 
	case 97 : 
	case 98 : 
	case 99 : 
	case 100 : 
	case 101 : 
	case 102: if (HSleep_resistance & INTRINSIC) {
			HSleep_resistance &= ~INTRINSIC;
			You_feel("tired all of a sudden.");
		}
		if (HSleep_resistance & TIMEOUT) {
			HSleep_resistance &= ~TIMEOUT;
			You_feel("tired all of a sudden.");
		}
		break;
	case 103 : 
	case 104 : 
	case 105 : 
	case 106 : 
	case 107 : 
	case 108 : 
	case 109 : 
	case 110 : 
	case 111 : 
	case 112: if (HDisint_resistance & INTRINSIC) {
			HDisint_resistance &= ~INTRINSIC;
			You_feel("like you're going to break apart.");
		}
		if (HDisint_resistance & TIMEOUT) {
			HDisint_resistance &= ~TIMEOUT;
			You_feel("like you're going to break apart.");
		}
		break;
	case 113 : 
	case 114 : 
	case 115 : 
	case 116 : 
	case 117 : 
	case 118 : 
	case 119 : 
	case 120 : 
	case 121 : 
	case 122: if (HShock_resistance & INTRINSIC) {
			HShock_resistance &= ~INTRINSIC;
			You_feel("like someone has zapped you.");
		}
		if (HShock_resistance & TIMEOUT) {
			HShock_resistance &= ~TIMEOUT;
			You_feel("like someone has zapped you.");
		}
		break;
	case 123: if (HDrain_resistance & INTRINSIC) {
			HDrain_resistance &= ~INTRINSIC;
			You_feel("like someone is sucking out your life-force.");
		}
		if (HDrain_resistance & TIMEOUT) {
			HDrain_resistance &= ~TIMEOUT;
			You_feel("like someone is sucking out your life-force.");
		}
		break;
	case 124: if (HSick_resistance & INTRINSIC) {
			HSick_resistance &= ~INTRINSIC;
			You_feel("no longer immune to diseases!");
		}
		if (HSick_resistance & TIMEOUT) {
			HSick_resistance &= ~TIMEOUT;
			You_feel("no longer immune to diseases!");
		}
		break;
	case 125 : 
	case 126 : 
	case 127: if (HWarning & INTRINSIC) {
			HWarning &= ~INTRINSIC;
			You_feel("that your radar has just stopped working!");
		}
		if (HWarning & TIMEOUT) {
			HWarning &= ~TIMEOUT;
			You_feel("that your radar has just stopped working!");
		}
		break;
	case 128 : 
	case 129 : 
	case 130 : 
	case 131 : 
	case 132 : 
	case 133 : 
	case 134: if (HSearching & INTRINSIC) {
			HSearching &= ~INTRINSIC;
			You_feel("unable to find something you lost!");
		}
		if (HSearching & TIMEOUT) {
			HSearching &= ~TIMEOUT;
			You_feel("unable to find something you lost!");
		}
		break;
	case 135: if (HClairvoyant & INTRINSIC) {
			HClairvoyant &= ~INTRINSIC;
			You_feel("a loss of mental capabilities!");
		}
		if (HClairvoyant & TIMEOUT) {
			HClairvoyant &= ~TIMEOUT;
			You_feel("a loss of mental capabilities!");
		}
		break;
	case 136: if (HInfravision & INTRINSIC) {
			HInfravision &= ~INTRINSIC;
			You_feel("shrouded in darkness.");
		}
		if (HInfravision & TIMEOUT) {
			HInfravision &= ~TIMEOUT;
			You_feel("shrouded in darkness.");
		}
		break;
	case 137: if (HDetect_monsters & INTRINSIC) {
			HDetect_monsters &= ~INTRINSIC;
			You_feel("that you can no longer sense monsters.");
		}
		if (HDetect_monsters & TIMEOUT) {
			HDetect_monsters &= ~TIMEOUT;
			You_feel("that you can no longer sense monsters.");
		}
		break;
	case 138: if (HJumping & INTRINSIC) {
			HJumping &= ~INTRINSIC;
			You_feel("your legs shrinking.");
		}
		if (HJumping & TIMEOUT) {
			HJumping &= ~TIMEOUT;
			You_feel("your legs shrinking.");
		}
		break;
	case 139 : 
	case 140 : 
	case 141 : 
	case 142 : 
	case 143 : 
	case 144 : 
	case 145 : 
	case 146 : 
	case 147 : 
	case 148: if (HTeleport_control & INTRINSIC) {
			HTeleport_control &= ~INTRINSIC;
			You_feel("unable to control where you're going.");
		}
		if (HTeleport_control & TIMEOUT) {
			HTeleport_control &= ~TIMEOUT;
			You_feel("unable to control where you're going.");
		}
		break;
	case 149: if (HMagical_breathing & INTRINSIC) {
			HMagical_breathing &= ~INTRINSIC;
			You_feel("you suddenly need to breathe!");
		}
		if (HMagical_breathing & TIMEOUT) {
			HMagical_breathing &= ~TIMEOUT;
			You_feel("you suddenly need to breathe!");
		}
		break;
	case 150: if (HRegeneration & INTRINSIC) {
			HRegeneration &= ~INTRINSIC;
			You_feel("your wounds are healing slower!");
		}
		if (HRegeneration & TIMEOUT) {
			HRegeneration &= ~TIMEOUT;
			You_feel("your wounds are healing slower!");
		}
		break;
	case 151: if (HEnergy_regeneration & INTRINSIC) {
			HEnergy_regeneration &= ~INTRINSIC;
			You_feel("a loss of mystic power!");
		}
		if (HEnergy_regeneration & TIMEOUT) {
			HEnergy_regeneration &= ~TIMEOUT;
			You_feel("a loss of mystic power!");
		}
		break;
	case 152: if (HPolymorph & INTRINSIC) {
			HPolymorph &= ~INTRINSIC;
			You_feel("unable to change form!");
		}
		if (HPolymorph & TIMEOUT) {
			HPolymorph &= ~TIMEOUT;
			You_feel("unable to change form!");
		}
		break;
	case 153: if (HPolymorph_control & INTRINSIC) {
			HPolymorph_control &= ~INTRINSIC;
			You_feel("less control over your own body.");
		}
		if (HPolymorph_control & TIMEOUT) {
			HPolymorph_control &= ~TIMEOUT;
			You_feel("less control over your own body.");
		}
		break;
	case 154 : 
	case 155 : 
	case 156 : 
	case 157: if (HAcid_resistance & INTRINSIC) {
			HAcid_resistance &= ~INTRINSIC;
			You_feel("worried about corrosion!");
		}
		if (HAcid_resistance & TIMEOUT) {
			HAcid_resistance &= ~TIMEOUT;
			You_feel("worried about corrosion!");
		}
		break;
	case 158: if (HFumbling & INTRINSIC) {
			HFumbling &= ~INTRINSIC;
			You_feel("less clumsy.");
		}
		if (HFumbling & TIMEOUT) {
			HFumbling &= ~TIMEOUT;
			You_feel("less clumsy.");
		}
		break;
	case 159: if (HSleeping & INTRINSIC) {
			HSleeping &= ~INTRINSIC;
			You_feel("like you just had some coffee.");
		}
		if (HSleeping & TIMEOUT) {
			HSleeping &= ~TIMEOUT;
			You_feel("like you just had some coffee.");
		}
		break;
	case 160: if (HHunger & INTRINSIC) {
			HHunger &= ~INTRINSIC;
			You_feel("like you just ate a chunk of meat.");
		}
		if (HHunger & TIMEOUT) {
			HHunger &= ~TIMEOUT;
			You_feel("like you just ate a chunk of meat.");
		}
		break;
	case 161: if (HConflict & INTRINSIC) {
			HConflict &= ~INTRINSIC;
			You_feel("more acceptable.");
		}
		if (HConflict & TIMEOUT) {
			HConflict &= ~TIMEOUT;
			You_feel("more acceptable.");
		}
		break;
	case 162: if (HSlow_digestion & INTRINSIC) {
			HSlow_digestion &= ~INTRINSIC;
			You_feel("like you're burning calories faster.");
		}
		if (HSlow_digestion & TIMEOUT) {
			HSlow_digestion &= ~TIMEOUT;
			You_feel("like you're burning calories faster.");
		}
		break;
	case 163: if (HFlying & INTRINSIC) {
			HFlying &= ~INTRINSIC;
			You_feel("like you just lost your wings!");
		}
		if (HFlying & TIMEOUT) {
			HFlying &= ~TIMEOUT;
			You_feel("like you just lost your wings!");
		}
		break;
	case 164: if (HPasses_walls & INTRINSIC) {
			HPasses_walls &= ~INTRINSIC;
			You_feel("less ethereal!");
		}
		if (HPasses_walls & TIMEOUT) {
			HPasses_walls &= ~TIMEOUT;
			You_feel("less ethereal!");
		}
		break;
	case 165: if (HAntimagic & INTRINSIC) {
			HAntimagic &= ~INTRINSIC;
			You_feel("less protected from magic!");
		}
		if (HAntimagic & TIMEOUT) {
			HAntimagic &= ~TIMEOUT;
			You_feel("less protected from magic!");
		}
		break;
	case 166: if (HReflecting & INTRINSIC) {
			HReflecting &= ~INTRINSIC;
			You_feel("less reflexive!");
		}
		if (HReflecting & TIMEOUT) {
			HReflecting &= ~TIMEOUT;
			You_feel("less reflexive!");
		}
		break;
	case 167: if (Blinded & INTRINSIC) {
			Blinded &= ~INTRINSIC;
			You_feel("visually clear!");
		}
		if (Blinded & TIMEOUT) {
			Blinded &= ~TIMEOUT;
			You_feel("visually clear!");
		}
		break;
	case 168: if (Glib & INTRINSIC) {
			Glib &= ~INTRINSIC;
			You_feel("heavy-handed!");
		}
		if (Glib & TIMEOUT) {
			Glib &= ~TIMEOUT;
			You_feel("heavy-handed!");
		}
		break;
	case 169: if (HSwimming & INTRINSIC) {
			HSwimming &= ~INTRINSIC;
			You_feel("less aquatic!");
		}
		if (HSwimming & TIMEOUT) {
			HSwimming &= ~TIMEOUT;
			You_feel("less aquatic!");
		}
		break;
	case 170: if (HNumbed & INTRINSIC) {
			HNumbed &= ~INTRINSIC;
			You_feel("your body parts relax.");
		}
		if (HNumbed & TIMEOUT) {
			HNumbed &= ~TIMEOUT;
			You_feel("your body parts relax.");
		}
		break;
	case 171: if (HFree_action & INTRINSIC) {
			HFree_action &= ~INTRINSIC;
			You_feel("a loss of freedom!");
		}
		if (HFree_action & TIMEOUT) {
			HFree_action &= ~TIMEOUT;
			You_feel("a loss of freedom!");
		}
		break;
	case 172: if (HFeared & INTRINSIC) {
			HFeared &= ~INTRINSIC;
			You_feel("less afraid.");
		}
		if (HFeared & TIMEOUT) {
			HFeared &= ~TIMEOUT;
			You_feel("less afraid.");
		}
		break;
	case 173: if (HFear_resistance & INTRINSIC) {
			HFear_resistance &= ~INTRINSIC;
			You_feel("a little anxious!");
		}
		if (HFear_resistance & TIMEOUT) {
			HFear_resistance &= ~TIMEOUT;
			You_feel("a little anxious!");
		}
		break;
	case 174: if (u.uhitincxtra != 0) {
			u.uhitinc -= u.uhitincxtra;
			u.uhitincxtra = 0;
			You_feel("your to-hit rating changing!");
		}
		break;
	case 175: if (u.udamincxtra != 0) {
			u.udaminc -= u.udamincxtra;
			u.udamincxtra = 0;
			You_feel("your damage rating changing!");
		}
		break;
	case 176: if (HKeen_memory & INTRINSIC) {
			HKeen_memory &= ~INTRINSIC;
			You_feel("a case of selective amnesia...");
		}
		if (HKeen_memory & TIMEOUT) {
			HKeen_memory &= ~TIMEOUT;
			You_feel("a case of selective amnesia...");
		}
		break;
	case 177: if (HVersus_curses & INTRINSIC) {
			HVersus_curses &= ~INTRINSIC;
			You_feel("cursed!");
		}
		if (HVersus_curses & TIMEOUT) {
			HVersus_curses &= ~TIMEOUT;
			You_feel("cursed!");
		}
		break;
	case 178: if (HStun_resist & INTRINSIC) {
			HStun_resist &= ~INTRINSIC;
			You_feel("a little stunned!");
		}
		if (HStun_resist & TIMEOUT) {
			HStun_resist &= ~TIMEOUT;
			You_feel("a little stunned!");
		}
		break;
	case 179: if (HConf_resist & INTRINSIC) {
			HConf_resist &= ~INTRINSIC;
			You_feel("a little confused!");
		}
		if (HConf_resist & TIMEOUT) {
			HConf_resist &= ~TIMEOUT;
			You_feel("a little confused!");
		}
		break;
	case 180: if (HDouble_attack & INTRINSIC) {
			HDouble_attack &= ~INTRINSIC;
			You_feel("your attacks becoming slower!");
		}
		if (HDouble_attack & TIMEOUT) {
			HDouble_attack &= ~TIMEOUT;
			You_feel("your attacks becoming slower!");
		}
		break;
	case 181: if (HQuad_attack & INTRINSIC) {
			HQuad_attack &= ~INTRINSIC;
			You_feel("your attacks becoming a lot slower!");
		}
		if (HQuad_attack & TIMEOUT) {
			HQuad_attack &= ~TIMEOUT;
			You_feel("your attacks becoming a lot slower!");
		}
		break;
	case 182: if (HExtra_wpn_practice & INTRINSIC) {
			HExtra_wpn_practice &= ~INTRINSIC;
			You_feel("less able to learn new stuff!");
		}
		if (HExtra_wpn_practice & TIMEOUT) {
			HExtra_wpn_practice &= ~TIMEOUT;
			You_feel("less able to learn new stuff!");
		}
		break;
	case 183: if (HDeath_resistance & INTRINSIC) {
			HDeath_resistance &= ~INTRINSIC;
			You_feel("a little dead!");
		}
		if (HDeath_resistance & TIMEOUT) {
			HDeath_resistance &= ~TIMEOUT;
			You_feel("a little dead!");
		}
		break;
	case 184: if (HDisplaced & INTRINSIC) {
			HDisplaced &= ~INTRINSIC;
			You_feel("a little exposed!");
		}
		if (HDisplaced & TIMEOUT) {
			HDisplaced &= ~TIMEOUT;
			You_feel("a little exposed!");
		}
		break;
	case 185: if (HPsi_resist & INTRINSIC) {
			HPsi_resist &= ~INTRINSIC;
			You_feel("empty-minded!");
		}
		if (HPsi_resist & TIMEOUT) {
			HPsi_resist &= ~TIMEOUT;
			You_feel("empty-minded!");
		}
		break;
	case 186: if (HSight_bonus & INTRINSIC) {
			HSight_bonus &= ~INTRINSIC;
			You_feel("less perceptive!");
		}
		if (HSight_bonus & TIMEOUT) {
			HSight_bonus &= ~TIMEOUT;
			You_feel("less perceptive!");
		}
		break;
	case 187:
	case 188: if (HManaleech & INTRINSIC) {
			HManaleech &= ~INTRINSIC;
			You_feel("less magically attuned!");
		}
		if (HManaleech & TIMEOUT) {
			HManaleech &= ~TIMEOUT;
			You_feel("less magically attuned!");
		}
		break;
	case 189: if (HMap_amnesia & INTRINSIC) {
			HMap_amnesia &= ~INTRINSIC;
			You_feel("less forgetful!");
		}
		if (HMap_amnesia & TIMEOUT) {
			HMap_amnesia &= ~TIMEOUT;
			You_feel("less forgetful!");
		}
		break;
	default: break;
	}
}

/*sit.c*/
