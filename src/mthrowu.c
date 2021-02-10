/*	SCCS Id: @(#)mthrowu.c	3.4	2003/05/09	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

STATIC_DCL int drop_throw(struct monst *, struct obj *,BOOLEAN_P,int,int);

#define URETREATING(x,y) (distmin(u.ux,u.uy,x,y) > distmin(u.ux0,u.uy0,x,y))

#define POLE_LIM 8	/* How far monsters can use pole-weapons */

#ifndef OVLB

const char *breathwep[];

#else /* OVLB */

/*
 * Keep consistent with breath weapons in zap.c, and AD_* in monattk.h.
 */
NEARDATA const char *breathwep[] = {
				"fragments",
				"fire",
				"frost",
				"sleep gas",
				"a disintegration blast",
				"lightning",
				"poison gas",
				"acid",
				"light",
				"a psionic blast"
};

NEARDATA const char *hallubreathwep[] = {"fragments", "fire", "frost", "sleep gas", "a disintegration blast", "lightning", "poison gas", "acid", "light", "strange breath #9", "sizzle", "nexus", "slaying", "vomit", "nausea", "repetition", "nether", "chaos", "confusion", "smoke", "--More-- You have died. DYWYPI?", "darkness", "sound", "gravity", "vibration", "penetration", "spitballs", "fart gas", "stinking gas", "slow gas", "rainbows", "air", "balloons", "nitrogen", "chloroform", "prussic acid", "ozone", "spill", "litter", "garbage", "trash", "heat", "cold", "ice", "water", "earth", "hell", "sky", "astral", "stars", "asterisks", "exclamation marks!!!", "feathers", "springs", "fog", "dew", "snow", "drugs", "rock'n'roll", "smog", "sludge", "waste", "temperature", "humidity", "vortices", "clouds", "a psionic blast", "cotton candy", "butterflies",

};

/* hero is hit by something other than a monster */
int
thitu(tlev, dam, obj, name)
int tlev, dam;
struct obj *obj;
const char *name;	/* if null, then format `obj' */
{
	const char *onm, *knm;
	boolean is_acid;
	boolean is_tailspike;
	boolean is_egg;
	boolean is_polearm;
	boolean is_thrown_weapon;
	boolean is_bulletammo;
	int kprefix = KILLED_BY_AN;
	char onmbuf[BUFSZ], knmbuf[BUFSZ];

	int shieldblockrate = 0;
	int saberblockrate = 0;
	int enchrequired = 0;

	int extrachance = 1;

	if (u.twoweap && uwep && uswapwep && tech_inuse(T_WEAPON_BLOCKER)) {
		shieldblockrate = 25;
	}

	if (uarms) {

		switch (uarms->otyp) {

		case SMALL_SHIELD:
			shieldblockrate = 10;
			break;
		case PAPER_SHIELD:
		case DIFFICULT_SHIELD:
			shieldblockrate = 40;
			break;
		case ICKY_SHIELD:
			shieldblockrate = 0;
			break;
		case HEAVY_SHIELD:
			shieldblockrate = 10;
			break;
		case BARRIER_SHIELD:
			shieldblockrate = 30;
			break;
		case TROLL_SHIELD:
		case MAGICAL_SHIELD:
		case SPECIAL_SHIELD:
		case WEAPON_SIGN:
			shieldblockrate = 20;
			break;
		case TARRIER:
			shieldblockrate = 25;
			break;
		case SHIELD_OF_PEACE:
			shieldblockrate = 20;
			break;
		case ELVEN_SHIELD:
			shieldblockrate = 20;
			if (Race_if(PM_ELF) || Race_if(PM_PLAYER_MYRKALFR) || Race_if(PM_DROW) || Role_if(PM_ELPH) || Role_if(PM_TWELPH)) shieldblockrate += 5;
			break;
		case URUK_HAI_SHIELD:
			shieldblockrate = 22;
			if (Race_if(PM_ORC)) shieldblockrate += 5;
			break;
		case ORCISH_SHIELD:
		case ORCISH_GUARD_SHIELD:
			shieldblockrate = 18;
			if (Race_if(PM_ORC)) shieldblockrate += 5;
			break;
		case DWARVISH_ROUNDSHIELD:
			shieldblockrate = 24;
			if (Race_if(PM_DWARF) || Role_if(PM_MIDGET) ) shieldblockrate += 5;
			break;
		case LARGE_SHIELD:
		case SHIELD:
			shieldblockrate = 25;
			break;
		case STEEL_SHIELD:
			shieldblockrate = 30;
			break;
		case METEORIC_STEEL_SHIELD:
			shieldblockrate = 32;
			break;
		case CRYSTAL_SHIELD:
		case RAPIRAPI:
		case HIDE_SHIELD:
			shieldblockrate = 35;
			break;
		case SHIELD_OF_REFLECTION:
		case SILVER_SHIELD:
		case ANCIENT_SHIELD:
		case MIRROR_SHIELD:
			shieldblockrate = 25;
			break;
		case FLAME_SHIELD:
			shieldblockrate = 30;
			break;
		case ICE_SHIELD:
			shieldblockrate = 30;
			break;
		case LIGHTNING_SHIELD:
			shieldblockrate = 30;
			break;
		case VENOM_SHIELD:
		case CHROME_SHIELD:
		case ANTISHADOW_SHIELD:
			shieldblockrate = 30;
			break;
		case SHIELD_OF_LIGHT:
			shieldblockrate = 30;
			break;
		case SHIELD_OF_MOBILITY:
			shieldblockrate = 30;
			break;
		case GRAY_DRAGON_SCALE_SHIELD:
		case SILVER_DRAGON_SCALE_SHIELD:
		case MERCURIAL_DRAGON_SCALE_SHIELD:
		case SHIMMERING_DRAGON_SCALE_SHIELD:
		case DEEP_DRAGON_SCALE_SHIELD:
		case RED_DRAGON_SCALE_SHIELD:
		case WHITE_DRAGON_SCALE_SHIELD:
		case ORANGE_DRAGON_SCALE_SHIELD:
		case BLACK_DRAGON_SCALE_SHIELD:
		case BLUE_DRAGON_SCALE_SHIELD:
		case COPPER_DRAGON_SCALE_SHIELD:
		case PLATINUM_DRAGON_SCALE_SHIELD:
		case BRASS_DRAGON_SCALE_SHIELD:
		case AMETHYST_DRAGON_SCALE_SHIELD:
		case PURPLE_DRAGON_SCALE_SHIELD:
		case DIAMOND_DRAGON_SCALE_SHIELD:
		case EMERALD_DRAGON_SCALE_SHIELD:
		case SAPPHIRE_DRAGON_SCALE_SHIELD:
		case RUBY_DRAGON_SCALE_SHIELD:
		case GREEN_DRAGON_SCALE_SHIELD:
		case GOLDEN_DRAGON_SCALE_SHIELD:
		case FEMINISM_DRAGON_SCALE_SHIELD:
		case CANCEL_DRAGON_SCALE_SHIELD:
		case NEGATIVE_DRAGON_SCALE_SHIELD:
		case CORONA_DRAGON_SCALE_SHIELD:
		case CONTRO_DRAGON_SCALE_SHIELD:
		case HEROIC_DRAGON_SCALE_SHIELD:
		case STONE_DRAGON_SCALE_SHIELD:
		case CYAN_DRAGON_SCALE_SHIELD:
		case PSYCHIC_DRAGON_SCALE_SHIELD:
		case RAINBOW_DRAGON_SCALE_SHIELD:
		case BLOOD_DRAGON_SCALE_SHIELD:
		case PLAIN_DRAGON_SCALE_SHIELD:
		case SKY_DRAGON_SCALE_SHIELD:
		case WATER_DRAGON_SCALE_SHIELD:
		case MAGIC_DRAGON_SCALE_SHIELD:
		case YELLOW_DRAGON_SCALE_SHIELD:

			shieldblockrate = 23;
			break;

		case EVIL_DRAGON_SCALE_SHIELD:

			shieldblockrate = 33;
			break;

		default: impossible("Unknown type of shield (%ld)", uarms->otyp);

		}

		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_SHIELD)) {
				case P_BASIC: shieldblockrate += 2; break;
				case P_SKILLED: shieldblockrate += 4; break;
				case P_EXPERT: shieldblockrate += 6; break;
				case P_MASTER: shieldblockrate += 8; break;
				case P_GRAND_MASTER: shieldblockrate += 10; break;
				case P_SUPREME_MASTER: shieldblockrate += 12; break;
			}

		}

		if (uarms->oartifact == ART_LURTZ_S_WALL) shieldblockrate += 20;
		if (uarm && uarm->oartifact == ART_MOEBIUS_ARMOR) shieldblockrate += 10;
		if (uarms->oartifact == ART_I_M_GETTING_HUNGRY) shieldblockrate += 20;
		if (uarms->oartifact == ART_WHANG_CLINK_CLONK) shieldblockrate += 10;
		if (uarms->oartifact == ART_LOOK_HOW_IT_BLOCKS) shieldblockrate += 20;
		if (uarms->oartifact == ART_BLOCKING_EXTREME) shieldblockrate += 10;
		if (uarms->oartifact == ART_CUTTING_THROUGH) shieldblockrate += 5;
		if (uwep && uwep->oartifact == ART_VEST_REPLACEMENT) shieldblockrate += 10;
		if (Race_if(PM_MACTHEIST)) shieldblockrate += 10;

		if (u.holyshield) shieldblockrate += (3 + spell_damage_bonus(SPE_HOLY_SHIELD));

		if (uarms->spe > 0) shieldblockrate += (uarms->spe * 2);

		if (uarms->cursed) shieldblockrate /= 2;
		if (uarms->blessed) shieldblockrate += 5;

		if (uarms->spe < 0) shieldblockrate += (uarms->spe * 2);

		if (uarm && uarm->oartifact == ART_WOODSTOCK) shieldblockrate += 5;
		if (Numbed) shieldblockrate -= 10;

		if (tlev > 10) shieldblockrate -= (rn2(tlev - 9));

		if (!PlayerCannotUseSkills) {
			switch (P_SKILL(P_SHIEN)) {
				case P_BASIC: shieldblockrate += 1; break;
				case P_SKILLED: shieldblockrate += 2; break;
				case P_EXPERT: shieldblockrate += 3; break;
				case P_MASTER: shieldblockrate += 4; break;
				case P_GRAND_MASTER: shieldblockrate += 5; break;
				case P_SUPREME_MASTER: shieldblockrate += 6; break;
			}
		}

		if (Conflict && shieldblockrate > 0) {
			shieldblockrate *= 2;
			shieldblockrate /= 3;
		}
		if (StrongConflict && shieldblockrate > 0) {
			shieldblockrate *= 2;
			shieldblockrate /= 3;
		}

		if (Role_if(PM_DANCER)) shieldblockrate /= 2;

		if (shieldblockrate < 0) shieldblockrate = 0;

		/* If you're berserk, you cannot block at all. We will still show your actual chance to block in enlightenment,
		   so this line should not be copied over to cmd.c --Amy */
		if (u.berserktime) shieldblockrate = 0;

	}

	if (!name) {
	    if (!obj) panic("thitu: name & obj both null?");
	    name = strcpy(onmbuf,
			 (obj->quan > 1L) ? doname(obj) : mshot_xname(obj));
	    knm = strcpy(knmbuf, killer_xname(obj));
	    kprefix = KILLED_BY;  /* killer_name supplies "an" if warranted */
	} else {
	    knm = name;
	    /* [perhaps ought to check for plural here to] */
	    if (!strncmpi(name, "the ", 4) ||
		    !strncmpi(name, "an ", 3) ||
		    !strncmpi(name, "a ", 2)) kprefix = KILLED_BY;
	}
	onm = (obj && obj_is_pname(obj)) ? the(name) :
			    (obj && obj->quan > 1L) ? name : an(name);
	is_acid = (obj && obj->otyp == ACID_VENOM);
	is_tailspike = (obj && obj->otyp == TAIL_SPIKES);
	is_egg = (obj && obj->otyp == EGG);
	is_polearm = (obj && (objects[obj->otyp].oc_skill == P_POLEARMS || objects[obj->otyp].oc_skill == P_LANCE || objects[obj->otyp].oc_skill == P_GRINDER || obj->otyp == AKLYS || obj->otyp == BLOW_AKLYS || obj->otyp == REACH_TRIDENT || obj->otyp == SPINED_BALL || obj->otyp == CHAIN_AND_SICKLE));
	is_thrown_weapon = (obj && (objects[obj->otyp].oc_skill == P_DART || objects[obj->otyp].oc_skill == P_SHURIKEN) );
	is_bulletammo = (obj && obj->otyp >= BULLET && obj->otyp <= GAS_GRENADE);

	if (is_bulletammo) extrachance = 1;
	else if (is_acid || is_tailspike || is_egg || is_polearm || (obj && obj->oclass == VENOM_CLASS) ) extrachance = 10;
	else if (is_thrown_weapon) extrachance = 3;
	else extrachance = 2;

	if (uwep && is_lightsaber(uwep) && uwep->lamplit) {
		saberblockrate = 5;
		if (!PlayerCannotUseSkills) {
			switch (P_SKILL(P_SHIEN)) {

				case P_BASIC:	saberblockrate +=  10; break;
				case P_SKILLED:	saberblockrate +=  20; break;
				case P_EXPERT:	saberblockrate +=  30; break;
				case P_MASTER:	saberblockrate +=  40; break;
				case P_GRAND_MASTER:	saberblockrate +=  50; break;
				case P_SUPREME_MASTER:	saberblockrate +=  60; break;
				default: saberblockrate += 0; break;
			}

		}
		if (P_SKILL(weapon_type(uwep)) >= P_SKILLED && !(PlayerCannotUseSkills) ) {
			saberblockrate += 30;
			if (Role_if(PM_JEDI)) {
				saberblockrate += ((100 - saberblockrate) / 2);
			}
			if (Race_if(PM_BORG)) {
				saberblockrate += ((100 - saberblockrate) / 5);
			}
		}

		if (tlev > 10) saberblockrate -= (rn3(tlev - 9));

	}

	if((u.uac + tlev <= rnd(20)) && (!rn2(StrongConflict ? 5 : Conflict ? 4 : 3))) {
		if(Blind || !flags.verbose) pline("It misses.");
		else You("are almost hit by %s.", onm);
		return(0);
	} else if ( (u.uac < 0) && (!rn2(StrongConflict ? 4 : Conflict ? 3 : 2)) && !rn2(extrachance) && (rnd(100) < (-(u.uac))) )    {
		/* more negative AC means a higher chance to deflect projectiles with armor --Amy */
		if(Blind || !flags.verbose) pline("Your armor deflects a projectile.");
		else You("deflect %s with your armor.", onm);

		u.ubodyarmorturns++;
		if (u.ubodyarmorturns >= 5) {
			u.ubodyarmorturns = 0;
			use_skill(P_BODY_ARMOR, 1);
		}

		return(0);

	} else if (!rn2(extrachance) && (rnd(100) < shieldblockrate) ) {

			if (u.twoweap && uwep && uswapwep && tech_inuse(T_WEAPON_BLOCKER)) {

				Your("weapons block a projectile.");
				if (evilfriday && multi >= 0) nomul(-2, "blocking with both weapons", TRUE);
				use_skill(P_TWO_WEAPON_COMBAT, 1);

			} else {

				/* a good shield allows you to block projectiles --Amy */
				if(Blind || !flags.verbose) pline("You block a projectile with your shield.");
				else You("block %s with your shield.", onm);
				use_skill(P_SHIELD, 1);
				if (uarms && uarms->oartifact == ART_SHIENSIDE) use_skill(P_SHIEN, 1);

				u.ubodyarmorturns++;
				if (u.ubodyarmorturns >= 5) {
					u.ubodyarmorturns = 0;
					use_skill(P_BODY_ARMOR, 1);
				}

			}

			return(0);

	} else if (uwep && is_lightsaber(uwep) && (3 > rnd(extrachance)) && uwep->lamplit && (saberblockrate > rn2(100))) {

		/* dodge missiles, even when blind; see "A new hope" for blindness reference */
		You("dodge %s with %s.", onm, yname(uwep));
		use_skill(P_SHIEN, rnd(4)); /* would take forever to train otherwise --Amy */

		if (tech_inuse(T_ABSORBER_SHIELD) && uwep && is_lightsaber(uwep) && uwep->lamplit) {
			pline("Energy surges into the lightsaber as the projectile is blocked.");
			uwep->age += 25;
		}

		return(0);

	} else if (powerfulimplants() && (!rn2(extrachance) || !rn2(extrachance) || !rn2(extrachance)) && uimplant && uimplant->oartifact == ART_GYMNASTIC_LOVE && !rn2(3)) {

			if(Blind || !flags.verbose) You("skillfully evade a projectile.");
			else You("skillfully evade %s.", onm);
			return(0);

	} else if (Race_if(PM_CUPID) && !rn2(5)) {

			if(Blind || !flags.verbose) You("sidestep a projectile.");
			else You("sidestep %s.", onm);
			return(0);

	} else if (uwep && uwep->oartifact == ART_SYLVIE_S_INVENTION && rn2(3)) {

			if(Blind || !flags.verbose) You("skillfully evade a projectile.");
			else You("skillfully evade %s.", onm);
			return(0);

	} else if (tech_inuse(T_FORCE_FIELD) && rn2(4)) {

			if(Blind || !flags.verbose) pline("Your force field causes a projectile to miss you.");
			else pline("Your force field causes %s to miss you.", onm);
			return(0);

	} else if (Race_if(PM_PLAYER_ATLANTEAN) && rn2(2)) {

			if(Blind || !flags.verbose) pline("Your force field causes a projectile to miss you.");
			else pline("Your force field causes %s to miss you.", onm);
			return(0);

	} else if (Race_if(PM_PLAYER_DYNAMO) && !rn2(3)) {

			if(Blind || !flags.verbose) pline("You absorb a projectile.");
			else pline("You absorb %s.", onm);
			healup(rnd(u.ulevel), 0, FALSE, FALSE);
			return(0);

	} else if (!rn2(extrachance) && rnd(30) < (2 + (GushLevel / 2) ) ) {

			/* depending on your character level, you may be able to dodge --Amy */
			if(Blind || !flags.verbose) pline("You dodge a projectile.");
			else You("dodge %s.", onm);
			return(0);

	} else {
		if(Blind || !flags.verbose) You("are hit!");
		else You("are hit by %s%s", onm, exclam(dam));

		if (obj && obj->otyp == YITH_TENTACLE) {
			increasesanity(rnz(monster_difficulty() + 1));
		}
		if (obj && obj->otyp == NASTYPOLE && !rn2(10)) {
			badeffect();
		}

		if (obj && obj->otyp == PETRIFYIUM_BAR) {
			    if ((!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
				!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
				if (!Stoned) {
					if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
					else {
						Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
						u.cnd_stoningcount++;
						pline("You start turning to stone!");
					}
				}
				sprintf(killer_buf, "petrifyium bar");
				delayed_killer = killer_buf;
		
			    }
		}

		if (obj && obj->otyp == DISINTEGRATION_BAR) {

			if ((!Disint_resistance || !rn2(StrongDisint_resistance ? 1000 : 100) || (evilfriday && (uarms || uarmc || uarm || uarmu)) ) && !rn2(10)) {
				You_feel("like you're falling apart!");
	
				if (uarms) {
				    /* destroy shield; other possessions are safe */
				    if (!(EDisint_resistance & W_ARMS)) (void) destroy_arm(uarms);
				} else if (uarmc) {
				    /* destroy cloak; other possessions are safe */
				    if (!(EDisint_resistance & W_ARMC)) (void) destroy_arm(uarmc);
				} else if (uarm) {
				    /* destroy suit */
				    if (!(EDisint_resistance & W_ARM)) (void) destroy_arm(uarm);
				} else if (uarmu) {
				    /* destroy shirt */
				    if (!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
				} else {
					if (u.uhpmax > 20) {
						u.uhpmax -= rnd(20);
						if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
						losehp(rnz(100 + level_difficulty()), "click click click click click you died", KILLED_BY);

					} else {
						u.youaredead = 1;
						done(DIED);
						u.youaredead = 0;
					}
				}
	
			}

		}

		if (obj && objects[obj->otyp].oc_material == MT_SILVER && (hates_silver(youmonst.data) || (uwep && uwep->oartifact == ART_PORKMAN_S_BALLS_OF_STEEL) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_PORKMAN_S_BALLS_OF_STEEL) ) ) {
			dam += rnd(20);
			pline_The("silver sears your flesh!");
			exercise(A_CON, FALSE);
		}
		if (obj && objects[obj->otyp].oc_material == MT_COPPER && hates_copper(youmonst.data)) {
			dam += 20;
			pline_The("copper decomposes you!");
			exercise(A_CON, FALSE);
		}
		if (obj && objects[obj->otyp].oc_material == MT_PLATINUM && (hates_platinum(youmonst.data) || u.contamination >= 1000) ) {
			dam += 20;
			pline_The("platinum smashes you!");
			exercise(A_CON, FALSE);
		}
		if (obj && obj->cursed && (hates_cursed(youmonst.data) || youmonst.data->mlet == S_ANGEL || Race_if(PM_HUMANOID_ANGEL))) {
			dam += 4;
			if (obj->hvycurse) dam += 4;
			if (obj->prmcurse) dam += 7;
			if (obj->bbrcurse) dam += 15;
			if (obj->evilcurse) dam += 15;
			if (obj->morgcurse) dam += 15;
			pline("An unholy aura blasts you!");
			exercise(A_CON, FALSE);
		}
		if (obj && objects[obj->otyp].oc_material == MT_VIVA && hates_viva(youmonst.data)) {
			dam += 20;
			pline_The("irradiation severely hurts you!");
			exercise(A_CON, FALSE);
		}
		if (obj && objects[obj->otyp].oc_material == MT_INKA) {
			dam += 5;
			pline_The("inka string hurts you!");
			exercise(A_CON, FALSE);
		}
		if (obj && obj->otyp == ODOR_SHOT) {
			dam += rnd(10);
			pline("You inhale the horrific odor!");
			if (tlev < 1) increasesanity(rnz(5));
			else increasesanity(rnz(tlev * 5));
			exercise(A_CON, FALSE);
		}

		if (obj && objects[obj->otyp].oc_skill == P_POLEARMS && (u.usteed || youmonst.data->mlet == S_CENTAUR || youmonst.data->mlet == S_UNICORN) || (!Upolyd && Race_if(PM_PLAYER_UNICORN)) || (!Upolyd && Race_if(PM_HUMANOID_CENTAUR)) || (!Upolyd && Race_if(PM_THUNDERLORD)) ) {
			dam += rnd(10);
			if (u.usteed && !rn2(25)) {
				if (!mayfalloffsteed()) {
					pline("The polearm lifts you out of your saddle!");
					dismount_steed(DISMOUNT_FELL);
				}

			}
		}

		if (uarmf && uarmf->oartifact == ART_STAR_SOLES) enchrequired = 1;
		if (Race_if(PM_PLAYER_SKELETON)) enchrequired = 2;
		if (uarmf && uarmf->oartifact == ART_PHANTO_S_RETARDEDNESS) enchrequired = 4;

		if (is_acid && Acid_resistance && (StrongAcid_resistance || rn2(10)) ) {
			pline("It doesn't seem to hurt you.");
			if (Stoned) fix_petrification();
		} else if ((enchrequired > 0) && rn2(3) && obj && obj->spe < enchrequired) {
			pline("The attack doesn't seem to harm you.");
		}
		else {
			if (is_acid) {pline("It burns!");
				if (tlev > 0) dam += rnd(tlev);
				if (Stoned) fix_petrification();
				}
			else if (is_tailspike && (tlev > 0) ) dam += rnd(tlev * 2);
			else if (is_polearm && !rn2(2) && (tlev > 10) ) dam += rnd(tlev - 10);
			else if (!is_bulletammo && (tlev > 10) && !rn2(3)) dam += rnd(tlev - 10);

			if (Half_physical_damage && rn2(2) ) dam = (dam+1) / 2;
			if (StrongHalf_physical_damage && rn2(2) ) dam = (dam+1) / 2;

			if (dam && u.uac < /*-1*/0) { /* AC protects against this damage now, at least a bit --Amy */

				int tempval;

				int effectiveac = (-(u.uac));
				if (issoviet) {
					effectiveac -= 20;
					if (effectiveac < 1) effectiveac = 1;
				}
				if (effectiveac > (issoviet ? 100 : 120)) {
					if (issoviet) effectiveac -= rn3(effectiveac - 99);
					else effectiveac -= rn3(effectiveac - 119);
				}
				if (effectiveac > (issoviet ? 60 : 80)) {
					if (issoviet) effectiveac -= rn3(effectiveac - 59);
					else effectiveac -= rn3(effectiveac - 79);
				}
				if (effectiveac > (issoviet ? 20 : 40)) {
					if (issoviet) effectiveac -= rn2(effectiveac - 19);
					else effectiveac -= rn2(effectiveac - 39);
				}

				tempval = rnd((effectiveac / (issoviet ? 5 : 4)) + 1);
				if (tempval < 1) tempval = 1;
				if (tempval > (issoviet ? 20 : 50)) tempval = (issoviet ? 20 : 50); /* max limit increased --Amy */

				if (issoviet) {
					dam -= tempval;
					if (dam < 1) dam = 1;
				}

				if (dam > 1 && tempval > 0) {
					dam *= (100 - rnd(tempval));
					dam++;
					dam /= 100;
					if (dam < 1) dam = 1;
				}

			}

			if (dam >= 2 && GushLevel > rnd(100)) dam = (dam+1) / 2;

			losehp(dam, knm, kprefix);
			exercise(A_STR, FALSE);
		}

		if (obj && (objects[obj->otyp].oc_skill == P_SHURIKEN || objects[obj->otyp].oc_skill == -P_SHURIKEN) && dam > 0) {
			You("get a cut.");
			playerbleed(dam);
		}

		/* evil patch: antimatter bullets will damage the player's inventory --Amy */
		if (obj && obj->otyp == ANTIMATTER_BULLET) {
			antimatter_damage(invent, FALSE, FALSE);
		}

		/* evil patch: darts of disintegration can disintegrate the player
		 * only have a 10% chance of actually doing so, because otherwise it would be really unbalanced --Amy */
		if (obj && obj->otyp == DART_OF_DISINTEGRATION) {
			if ((!Disint_resistance || !rn2(StrongDisint_resistance ? 1000 : 100) || (evilfriday && (uarms || uarmc || uarm || uarmu)) ) && !rn2(10)) {
				You_feel("like you're falling apart!");
	
				if (uarms) {
				    /* destroy shield; other possessions are safe */
				    if (!(EDisint_resistance & W_ARMS)) (void) destroy_arm(uarms);
				} else if (uarmc) {
				    /* destroy cloak; other possessions are safe */
				    if (!(EDisint_resistance & W_ARMC)) (void) destroy_arm(uarmc);
				} else if (uarm) {
				    /* destroy suit */
				    if (!(EDisint_resistance & W_ARM)) (void) destroy_arm(uarm);
				} else if (uarmu) {
				    /* destroy shirt */
				    if (!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
				} else {
					if (u.uhpmax > 20) {
						u.uhpmax -= rnd(20);
						if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
						losehp(rnz(100 + level_difficulty()), "click click click click click you died", KILLED_BY);

					} else {
						u.youaredead = 1;
						done(DIED);
						u.youaredead = 0;
					}
				}
	
			}
		}

		if (obj && objects[obj->otyp].oc_skill == P_GRINDER) {
			You("are grinded!");
			losehp(rnd(10), "grinding", KILLED_BY);
		}

		return(1);
	}
}

/* Be sure this corresponds with what happens to player-thrown objects in
 * dothrow.c (for consistency). --KAA
 * Returns 0 if object still exists (not destroyed).
 */

STATIC_OVL int
drop_throw(mon, obj, ohit, x, y)
register struct monst *mon;
register struct obj *obj;
boolean ohit;
int x,y;
{
	struct obj *mwep = (struct obj *) 0;
	
	int retvalu = 1;
	int create, chance;
	struct monst *mtmp;
	struct trap *t;

	if (mon) mwep = MON_WEP(mon);

	if (issegfaulter) { /* used to cause segfault panics but that's just malicious... cause fake segfault instead --Amy */
		if (obj->oclass == VENOM_CLASS) {
			if (obj->otyp == SEGFAULT_VENOM && !rn2(5) ) {
				u.segfaultpanic = TRUE;
			}
		}
	}

	if ((obj->otyp == CREAM_PIE || obj->oclass == VENOM_CLASS ||
/* WAC added Spoon throw code */
                    (obj->oartifact == ART_HOUCHOU) ||
		    /* WAC -- assume monsters don't throw without 
		    	using the right propellor */
                    (is_bullet(obj) && !(objects[obj->otyp].oc_material == MT_LEAD && !rn2(2))) ||
		    (ohit && obj->otyp == EGG)))
		create = 0;
	else if (ohit && (is_multigen(obj) || obj->otyp == ROCK)) {

		/* copying over the dothrow.c code, because it makes no sense for blessed +10 ammo to break 2 out of 3 times --Amy */
	    chance = greatest_erosionX(obj) - obj->spe;
	    chance -= rnd(2);

	    if (chance > 1) {
		if (chance == 3) chance = 2;
		else if (chance == 4) chance = 3;
		else if (chance == 5) chance = 3;
		else if (chance > 5) chance /= 2;
		create = !rn2(chance);
	    } else {
		chance = 3 + obj->spe - greatest_erosionX(obj);
		if (chance > 3) chance = 2 + rno(chance - 2);
		if (chance < 1) chance = 1; /* fail safe */
		if (Race_if(PM_MONGUNG)) chance *= 2;

		create = rn2(chance);
		}

	    if (obj->blessed && !rnl(6))
		create = 1;
	    if (!obj->blessed && !obj->cursed && !rn2(3) && !rnl(6))
		create = 1;

		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_MISSILE_WEAPONS)) {
				default: break;
				case P_BASIC: if (rn2(10) < 1) create = 1; break;
				case P_SKILLED: if (rn2(10) < 2) create = 1; break;
				case P_EXPERT: if (rn2(10) < 3) create = 1; break;
				case P_MASTER: if (rn2(10) < 4) create = 1; break;
				case P_GRAND_MASTER: if (rn2(10) < 5) create = 1; break;
				case P_SUPREME_MASTER: if (rn2(10) < 6) create = 1; break;
			}
		}

		if (!(PlayerCannotUseSkills)) {

	    if (objects[obj->otyp].oc_skill == -P_BOW && (P_SKILL(P_BOW) >= P_BASIC) && rn2(P_SKILL(P_BOW)) )
		create = 1;
	    if (objects[obj->otyp].oc_skill == -P_CROSSBOW && (P_SKILL(P_CROSSBOW) >= P_BASIC) && rn2(P_SKILL(P_CROSSBOW)) )
		create = 1;
	    if (objects[obj->otyp].oc_skill == -P_SLING && (P_SKILL(P_SLING) >= P_BASIC) && rn2(P_SKILL(P_SLING)) )
		create = 1;
	    if (objects[obj->otyp].oc_skill == -P_DART && (P_SKILL(P_DART) >= P_BASIC) && rn2(P_SKILL(P_DART)) )
		create = 1;
	    if (objects[obj->otyp].oc_skill == -P_SHURIKEN && (P_SKILL(P_SHURIKEN) >= P_BASIC) && rn2(P_SKILL(P_SHURIKEN)) )
		create = 1;

		}

	    if (objects[obj->otyp].oc_skill == -P_BOW && uarm && uarm->oartifact == ART_WOODSTOCK && !create && !rn2(2))
		create = 1;
	    if (objects[obj->otyp].oc_material == MT_MINERAL && uarm && uarm->oartifact == ART_QUARRY && !create && !rn2(2))
		create = 1;
	    if (uarmc && uarmc->oartifact == ART_ARABELLA_S_WEAPON_STORAGE && !create && !rn2(2))
		create = 1;
	    if (Race_if(PM_MACTHEIST) && objects[obj->otyp].oc_skill == P_SLING && !create && !rn2(2))
		create = 1;
	    if (Race_if(PM_MACTHEIST) && objects[obj->otyp].oc_skill == -P_SLING && !create && !rn2(2))
		create = 1;

	    if (objects[obj->otyp].oc_material == MT_LEAD && !create && !rn2(4)) create = 1;
	    if (obj->otyp == DART_OF_DISINTEGRATION && rn2(10)) create = 0;

	} else create = 1;

	if (obj->mstartinventB && obj->otyp != ROCKET && !is_grenade(obj) && !(obj->oartifact) && !(obj->fakeartifact && timebasedlowerchance() && rn2(4) ) && (!rn2(4) || (rn2(100) < u.equipmentremovechance) || !timebasedlowerchance() ) ) create = 0;
	if (obj->mstartinventC && obj->otyp != ROCKET && !is_grenade(obj) && !(obj->oartifact) && !(obj->fakeartifact && !rn2(10)) && rn2(10)) create = 0;

	/* Detonate rockets */
	if (is_grenade(obj)) {

		if (rn2(5)) verbalize("Fire in the hole!");
		else if (rn2(3)) verbalize("Fire in the ass hole!");
		else if (rn2(2)) verbalize("Fire in your ass hole %s!", playeraliasname);
		else verbalize("Here is a grenade four you!"); /* sic */

		if (!ohit) {
			create = 1; /* Don't destroy */
			arm_bomb(obj, FALSE);
		} else {
			grenade_explode(obj, bhitpos.x, bhitpos.y, FALSE, 0);
			obj = (struct obj *)0;
		}
	} else if (objects[obj->otyp].oc_dir & EXPLOSION) {
	    	if (cansee(bhitpos.x,bhitpos.y)) 
	    		pline("%s explodes in a ball of fire!", Doname2(obj));
	    	else You_hear("an explosion");
		explode(bhitpos.x, bhitpos.y, -ZT_SPELL(ZT_FIRE), d(3,8),
		    WEAPON_CLASS, EXPL_FIERY);
	}

	/* D: Detonate crossbow bolts from Hellfire if they hit */
	if (ohit && mwep && mwep->oartifact == ART_HELLFIRE
		  && is_ammo(obj) && ammo_and_launcher(obj, mwep)) {
	  
		if (cansee(bhitpos.x,bhitpos.y)) 
			pline("%s explodes in a ball of fire!", Doname2(obj));
		else 
			You_hear("an explosion");

		explode(bhitpos.x, bhitpos.y, -ZT_SPELL(ZT_FIRE),
		    d(2,6), WEAPON_CLASS, EXPL_FIERY);

		/* D: Exploding bolts will be destroyed */
		create = 0;
	}

	if (create && !((mtmp = m_at(x, y)) && (mtmp->mtrapped) &&
			(t = t_at(x, y)) && ((t->ttyp == PIT) || (t->ttyp == SHIT_PIT) || (t->ttyp == MANA_PIT) || (t->ttyp == ANOXIC_PIT) || (t->ttyp == GIANT_CHASM) || (t->ttyp == ACID_PIT) ||
			(t->ttyp == SPIKED_PIT)))) {
		int objgone = 0;
		obj->mstartinventB = 0;
		obj->mstartinventC = 0;

		if (down_gate(x, y) != -1)
			objgone = ship_object(obj, x, y, FALSE);
		if (!objgone) {
			if (!flooreffects(obj,x,y,"fall")) { /* don't double-dip on damage */
			    place_object(obj, x, y);

			    if (!mtmp && x == u.ux && y == u.uy)
				mtmp = &youmonst;
			    if (mtmp && ohit)
				passive_obj(mtmp, obj, (struct attack *)0);

			    /* evil patch idea: monsters shooting nasty gray stones cause them to end up in your pack --Amy */
			    if (obj && x == u.ux && y == u.uy && is_nastygraystone(obj)) {
			      pline("%s lands in your knapsack!", Doname2(obj));
				(void) pickup_object(obj, obj->quan, TRUE, TRUE);
			    } else if (obj && x == u.ux && y == u.uy && is_feminismstone(obj)) {
			      pline("%s stays in your inventory, and you get a bad feeling about it.", Doname2(obj));
				(void) pickup_object(obj, obj->quan, TRUE, TRUE);
			    } else stackobj(obj);
			    retvalu = 0;

			}
		}
	} else if (obj) obfree(obj, (struct obj*) 0);

	return retvalu;
}

#endif /* OVLB */
#ifdef OVL1

/* an object launched by someone/thing other than player attacks a monster;
   return 1 if the object has stopped moving (hit or its range used up) */
int
ohitmon(mon, mtmp, otmp, range, verbose)
struct monst *mon;  /* monster thrower (if applicable) */
struct monst *mtmp;	/* accidental target */
struct obj *otmp;	/* missile; might be destroyed by drop_throw */
int range;		/* how much farther will object travel if it misses */
			/* Use -1 to signify to keep going even after hit, */
			/* unless its gone (used for rolling_boulder_traps) */
boolean verbose;  /* give message(s) even when you can't see what happened */
{
	int damage, tmp;
	boolean vis, ismimic;
	int objgone = 1;
	register struct obj *blocker = (struct obj *)0;
	int shieldblockrate = 0;

	ismimic = mtmp->m_ap_type && mtmp->m_ap_type != M_AP_MONSTER;
	vis = cansee(bhitpos.x, bhitpos.y);

	tmp = 5 + find_mac(mtmp) + omon_adj(mtmp, otmp, FALSE);

	/* Amy edit: if a pet is the target and the monster is high-level, add to-hit to make sure it can actually hit */
	if (mtmp->mtame && mon) {
		int armordifferential = 0;
		if (mon->m_lev > 0) armordifferential += mon->m_lev;
		if (mtmp->m_lev > mon->m_lev) armordifferential -= (mtmp->m_lev - mon->m_lev);
		if (armordifferential < 0) armordifferential = 0; /* fail safe */
		tmp += armordifferential;
		if (otmp && otmp->oclass == VENOM_CLASS) tmp += 10;
	}
	if (verysmall(mtmp->data) && !rn2(8)) {
	    if (!ismimic) {
		if (vis) pline("%s avoids a projectile.", Monnam(mtmp));
	    }
	    if (!range) { /* Last position; object drops */
		(void) drop_throw(mon, otmp, 0, mtmp->mx, mtmp->my);
		return 1;
	    }
	} else if (rathersmall(mtmp->data) && !verysmall(mtmp->data) && !rn2(20)) {
	    if (!ismimic) {
		if (vis) pline("%s avoids a projectile.", Monnam(mtmp));
	    }
	    if (!range) { /* Last position; object drops */
		(void) drop_throw(mon, otmp, 0, mtmp->mx, mtmp->my);
		return 1;
	    }
	} else if (hugemonst(mtmp->data) && !rn2(8)) {
	    if (!ismimic) {
		if (vis) pline("%s shrugs off a projectile.", Monnam(mtmp));
	    }
	    if (!range) { /* Last position; object drops */
		(void) drop_throw(mon, otmp, 0, mtmp->mx, mtmp->my);
		return 1;
	    }
	} else if (bigmonst(mtmp->data) && !hugemonst(mtmp->data) && !rn2(15)) {
	    if (!ismimic) {
		if (vis) pline("%s shrugs off a projectile.", Monnam(mtmp));
	    }
	    if (!range) { /* Last position; object drops */
		(void) drop_throw(mon, otmp, 0, mtmp->mx, mtmp->my);
		return 1;
	    }
	} else if (amorphous(mtmp->data) && !rn2(10)) {
	    if (!ismimic) {
		if (vis) pline("%s's amorphous body skillfully dodges a projectile.", Monnam(mtmp));
	    }
	    if (!range) { /* Last position; object drops */
		(void) drop_throw(mon, otmp, 0, mtmp->mx, mtmp->my);
		return 1;
	    }
	} else if (noncorporeal(mtmp->data) && !rn2(2)) {
	    if (!ismimic) {
		if (vis) pline("%s avoids a projectile due to being noncorporeal.", Monnam(mtmp));
	    }
	    if (!range) { /* Last position; object drops */
		(void) drop_throw(mon, otmp, 0, mtmp->mx, mtmp->my);
		return 1;
	    }
	} else if (unsolid(mtmp->data) && !rn2(8)) {
	    if (!ismimic) {
		if (vis) pline("%s's unsolid body lets a projectile pass through harmlessly.", Monnam(mtmp));
	    }
	    if (!range) { /* Last position; object drops */
		(void) drop_throw(mon, otmp, 0, mtmp->mx, mtmp->my);
		return 1;
	    }
	} else if (mtmp->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_] || mtmp->data == &mons[PM_ATHLEANNIE] || mtmp->data == &mons[PM_MR__CONCLUSIO] || mtmp->data == &mons[PM_YOUR_GAME_ENDS_NOW]|| mtmp->data == &mons[PM_ELITE_GENDAME] || mtmp->data == &mons[PM_LILAC_FEMMY] || mtmp->data == &mons[PM_GREEN]) { /* will never be hit by monsters' ranged attacks */
	    if (!ismimic) {
		pline("%s swats a projectile away.", Monnam(mtmp));
	    }
	    if (!range) { /* Last position; object drops */
		(void) drop_throw(mon, otmp, 0, mtmp->mx, mtmp->my);
		return 1;
	    }
	} else if (blocker = (which_armor(mtmp, W_ARMS))) {

		switch (blocker->otyp) {

			case SMALL_SHIELD:
				shieldblockrate = 20;
				break;
			case PAPER_SHIELD:
			case DIFFICULT_SHIELD:
				shieldblockrate = 50;
				break;
			case ICKY_SHIELD:
				shieldblockrate = 10;
				break;
			case HEAVY_SHIELD:
				shieldblockrate = 20;
				break;
			case BARRIER_SHIELD:
				shieldblockrate = 40;
				break;
			case TROLL_SHIELD:
			case MAGICAL_SHIELD:
			case SPECIAL_SHIELD:
			case WEAPON_SIGN:
				shieldblockrate = 30;
				break;
			case TARRIER:
				shieldblockrate = 35;
				break;
			case SHIELD_OF_PEACE:
				shieldblockrate = 30;
				break;
			case ELVEN_SHIELD:
				shieldblockrate = 30;
				if (is_elf(mtmp->data)) shieldblockrate += 5;
				break;
			case URUK_HAI_SHIELD:
				shieldblockrate = 32;
				if (is_orc(mtmp->data)) shieldblockrate += 5;
				break;
			case ORCISH_SHIELD:
			case ORCISH_GUARD_SHIELD:
				shieldblockrate = 28;
				if (is_orc(mtmp->data)) shieldblockrate += 5;
				break;
			case DWARVISH_ROUNDSHIELD:
				shieldblockrate = 34;
				if (is_dwarf(mtmp->data)) shieldblockrate += 5;
				break;
			case LARGE_SHIELD:
			case SHIELD:
				shieldblockrate = 35;
				break;
			case STEEL_SHIELD:
				shieldblockrate = 40;
				break;
			case METEORIC_STEEL_SHIELD:
				shieldblockrate = 42;
				break;
			case CRYSTAL_SHIELD:
			case RAPIRAPI:
			case HIDE_SHIELD:
				shieldblockrate = 45;
				break;
			case SHIELD_OF_REFLECTION:
			case ANCIENT_SHIELD:
			case SILVER_SHIELD:
			case MIRROR_SHIELD:
				shieldblockrate = 35;
				break;
			case FLAME_SHIELD:
				shieldblockrate = 40;
				break;
			case ICE_SHIELD:
				shieldblockrate = 40;
				break;
			case LIGHTNING_SHIELD:
				shieldblockrate = 40;
				break;
			case VENOM_SHIELD:
			case CHROME_SHIELD:
			case ANTISHADOW_SHIELD:
				shieldblockrate = 40;
				break;
			case SHIELD_OF_LIGHT:
				shieldblockrate = 40;
				break;
			case SHIELD_OF_MOBILITY:
				shieldblockrate = 40;
				break;

			case GRAY_DRAGON_SCALE_SHIELD:
			case SILVER_DRAGON_SCALE_SHIELD:
			case MERCURIAL_DRAGON_SCALE_SHIELD:
			case SHIMMERING_DRAGON_SCALE_SHIELD:
			case DEEP_DRAGON_SCALE_SHIELD:
			case RED_DRAGON_SCALE_SHIELD:
			case WHITE_DRAGON_SCALE_SHIELD:
			case ORANGE_DRAGON_SCALE_SHIELD:
			case BLACK_DRAGON_SCALE_SHIELD:
			case BLUE_DRAGON_SCALE_SHIELD:
			case COPPER_DRAGON_SCALE_SHIELD:
			case PLATINUM_DRAGON_SCALE_SHIELD:
			case BRASS_DRAGON_SCALE_SHIELD:
			case AMETHYST_DRAGON_SCALE_SHIELD:
			case PURPLE_DRAGON_SCALE_SHIELD:
			case DIAMOND_DRAGON_SCALE_SHIELD:
			case EMERALD_DRAGON_SCALE_SHIELD:
			case SAPPHIRE_DRAGON_SCALE_SHIELD:
			case RUBY_DRAGON_SCALE_SHIELD:
			case GREEN_DRAGON_SCALE_SHIELD:
			case GOLDEN_DRAGON_SCALE_SHIELD:
			case FEMINISM_DRAGON_SCALE_SHIELD:
			case CANCEL_DRAGON_SCALE_SHIELD:
			case NEGATIVE_DRAGON_SCALE_SHIELD:
			case CORONA_DRAGON_SCALE_SHIELD:
			case CONTRO_DRAGON_SCALE_SHIELD:
			case HEROIC_DRAGON_SCALE_SHIELD:
			case STONE_DRAGON_SCALE_SHIELD:
			case CYAN_DRAGON_SCALE_SHIELD:
			case PSYCHIC_DRAGON_SCALE_SHIELD:
			case RAINBOW_DRAGON_SCALE_SHIELD:
			case BLOOD_DRAGON_SCALE_SHIELD:
			case PLAIN_DRAGON_SCALE_SHIELD:
			case SKY_DRAGON_SCALE_SHIELD:
			case WATER_DRAGON_SCALE_SHIELD:
			case MAGIC_DRAGON_SCALE_SHIELD:
			case YELLOW_DRAGON_SCALE_SHIELD:

				shieldblockrate = 33;
				break;

			case EVIL_DRAGON_SCALE_SHIELD:

				shieldblockrate = 43;
				break;

			default: impossible("Unknown type of shield (%ld)", blocker->otyp);

		}

		if (shieldblockrate && (blocker->spe > 0)) shieldblockrate += (blocker->spe * 2);
		if (blocker->blessed) shieldblockrate += 5;

		if (rnd(100) < shieldblockrate) {
			    if (!ismimic) {
				pline("%s's shield blocks a projectile.", Monnam(mtmp));
			    }
			    if (!range) { /* Last position; object drops */
				(void) drop_throw(mon, otmp, 0, mtmp->mx, mtmp->my);
				return 1;
			    }
		}
		else goto blockingdone;
	} else
blockingdone:	
	if (tmp < rnd(20)) {
	    if (!ismimic) {
		if (vis) miss(distant_name(otmp, mshot_xname), mtmp);
		else if (verbose) pline("It is missed.");
	    }
	    if (!range) { /* Last position; object drops */
		(void) drop_throw(mon, otmp, 0, mtmp->mx, mtmp->my);
		return 1;
	    }
	} else if (otmp->oclass == POTION_CLASS) {
	    if (ismimic) seemimic(mtmp);
	    mtmp->msleeping = 0;
	    if (vis) otmp->dknown = 1;
	    potionhit(mtmp, otmp, FALSE);
	    if (mon && !DEADMONSTER(mon) && !DEADMONSTER(mtmp) &&
		    mtmp->movement >= NORMAL_SPEED && rn2(4)) {
		/* retaliate */
		mtmp->movement -= NORMAL_SPEED;
		mattackm(mtmp, mon);
	    }
	    return 1;
	} else {
	    damage = dmgval(otmp, mtmp);
	    if (mtmp->mtame && mon) {
		if (mon->m_lev >= 5) damage += ((mon->m_lev - 4) / 2);
		if (otmp->otyp == TAIL_SPIKES) damage += rnd((mon->m_lev * 2) + 30);
		if (otmp->otyp == DART_OF_DISINTEGRATION) damage += rnd(50);
		if (otmp->otyp == DISINTEGRATION_BAR) damage += rnd(50);
		if (otmp->otyp == NASTYPOLE) damage += rnd(10);
		if (otmp->otyp == PETRIFYIUM_BAR && !rn2(4)) damage += rnd(200);
		if (otmp && objects[otmp->otyp].oc_skill == P_GRINDER) damage + rnd(10);
	    }
            if (otmp->otyp == SPOON) {
            pline("The spoon flashes brightly as it hits %s.",
                   the(mon_nam(mtmp)));
            }

	    if (otmp->otyp == ACID_VENOM && resists_acid(mtmp))
		damage = 0;
	    if (ismimic) seemimic(mtmp);
	    mtmp->msleeping = 0;
	    if (vis) hit(distant_name(otmp,mshot_xname), mtmp, exclam(damage));
	    else if (verbose) pline("%s is hit%s", Monnam(mtmp), exclam(damage));

	    if (otmp->opoisoned) {
		if (resists_poison(mtmp)) {
		    if (vis) pline_The("poison doesn't seem to affect %s.",
				   mon_nam(mtmp));
		} else {
		    if (rn2(150)) {
			damage += rnd(mtmp->mtame ? 15 : 6);
		    } else {
			if (vis) pline_The("poison was deadly...");
			damage = mtmp->mhp;
		    }
		}
	    }
	    if (objects[otmp->otyp].oc_material == MT_SILVER &&
		    hates_silver(mtmp->data)) {
		if (vis) pline_The("silver sears %s flesh!",
				s_suffix(mon_nam(mtmp)));
		else if (verbose) pline("Its flesh is seared!");
	    }
	    if (objects[otmp->otyp].oc_material == MT_VIVA && hates_viva(mtmp->data)) {
		if (verbose) pline("It is irradiated!");
	    }
	    if (objects[otmp->otyp].oc_material == MT_COPPER && hates_copper(mtmp->data)) {
		if (verbose) pline("It is decomposed!");
	    }
	    if (objects[otmp->otyp].oc_material == MT_PLATINUM && hates_platinum(mtmp->data)) {
		if (verbose) pline("It is smashed!");
	    }
	    if (otmp->cursed && hates_cursed(mtmp->data)) {
		if (verbose) pline("It is blasted by darkness!");
	    }
	    if (objects[otmp->otyp].oc_material == MT_INKA && hates_inka(mtmp->data)) {
		if (verbose) pline("It is hurt!");
	    }
	    if (otmp->otyp == ODOR_SHOT && hates_odor(mtmp->data)) {
		if (verbose) pline("It is beguiled!");
	    }
	    if (otmp->otyp == ACID_VENOM && cansee(mtmp->mx,mtmp->my)) {
		if (resists_acid(mtmp)) {
		    if (vis || verbose)
			pline("%s is unaffected.", Monnam(mtmp));
		    damage = 0;
		} else {
		    if (vis) pline_The("acid burns %s!", mon_nam(mtmp));
		    else if (verbose) pline("It is burned!");
		}
	    }
	    mtmp->mhp -= damage;
	    if (mtmp->mhp < 1) {
		if (vis || verbose)
		    pline("%s is %s!", Monnam(mtmp),
			(nonliving(mtmp->data) || !canspotmon(mtmp))
			? "destroyed" : "killed");
		/* don't blame hero for unknown rolling boulder trap */
		if (!flags.mon_moving &&
		    (otmp->otyp != BOULDER || range >= 0 || !otmp->otrapped))
		    xkilled(mtmp,0);
		else mondied(mtmp);
	    }

	    if (can_blnd((struct monst*)0, mtmp,
		    (uchar)(otmp->otyp == BLINDING_VENOM ? AT_SPIT : AT_WEAP),
		    otmp)) {
		if (vis && mtmp->mcansee)
		    pline("%s is blinded by %s.", Monnam(mtmp), the(xname(otmp)));
		mtmp->mcansee = 0;
		tmp = (int)mtmp->mblinded + rnd(25) + 20;
		if (tmp > 127) tmp = 127;
		mtmp->mblinded = tmp;
	    }

	    if (mon && !DEADMONSTER(mon) && !DEADMONSTER(mtmp) &&
		    mtmp->movement >= NORMAL_SPEED && rn2(4)) {
		/* retaliate */
		mtmp->movement -= NORMAL_SPEED;
		mattackm(mtmp, mon);
	    }

	    objgone = drop_throw(mon, otmp, 1, bhitpos.x, bhitpos.y);
	    if (!objgone && range == -1) {  /* special case */
		    obj_extract_self(otmp); /* free it for motion again */
		    return 0;
	    }
	    return 1;
	}
	return 0;
}

void
m_throw(mon, x, y, dx, dy, range, obj)
	register struct monst *mon;
	register int x,y,dx,dy,range;		/* direction and range */
	register struct obj *obj;
{
	register struct monst *mtmp;
	struct obj *singleobj, *mwep;
	char sym = obj->oclass;
	int hitu, blindinc = 0;

	bhitpos.x = x;
	bhitpos.y = y;

	if (obj->quan == 1L) {
	    /*
	     * Remove object from minvent.  This cannot be done later on;
	     * what if the player dies before then, leaving the monster
	     * with 0 daggers?  (This caused the infamous 2^32-1 orcish
	     * dagger bug).
	     *
	     * VENOM is not in minvent - it should already be OBJ_FREE.
	     * The extract below does nothing.
	     */

	    /* not possibly_unwield, which checks the object's */
	    /* location, not its existence */
	    if (MON_WEP(mon) == obj) {
		    setmnotwielded(mon,obj);
		    MON_NOWEP(mon);
	    }
	    obj_extract_self(obj);
	    singleobj = obj;
	    obj = (struct obj *) 0;
	} else {
	    singleobj = splitobj(obj, 1L);
	    obj_extract_self(singleobj);
	}

	singleobj->owornmask = 0; /* threw one of multiple weapons in hand? */

	if (mon) mwep = MON_WEP(mon);
	else mwep = (struct obj *) 0;
	
	/* D: Special launcher effects */
	if (mwep && is_ammo(singleobj) && ammo_and_launcher(singleobj, mwep)) {
	    if (mwep->oartifact == ART_PLAGUE && is_poisonable(singleobj))
			singleobj->opoisoned = 1;

	    /* D: Hellfire is handled in drop_throw */
	}

	if (mwep && singleobj && ammo_and_launcher(singleobj, mwep) && is_ammo(singleobj) && singleobj->otyp == POISON_BOLT) {
		singleobj->opoisoned = 1;

	}

	if (singleobj->cursed && (dx || dy) && !rn2(7)) {
	    if(canseemon(mon) && flags.verbose) {
		if(is_ammo(singleobj))
		    pline("%s misfires!", Monnam(mon));
		else
		    pline("%s as %s throws it!",
			  Tobjnam(singleobj, "slip"), mon_nam(mon));
	    }
	    dx = rn2(3)-1;
	    dy = rn2(3)-1;
	    /* check validity of new direction */
	    if (!dx && !dy) {
		(void) drop_throw(mon, singleobj, 0, bhitpos.x, bhitpos.y);
		return;
	    }
	}

	/* pre-check for doors, walls and boundaries.
	  Also need to pre-check for bars regardless of direction;
	  the random chance for small objects hitting bars is
	  skipped when reaching them at point blank range */
	if (!isok(bhitpos.x+dx,bhitpos.y+dy)
	    || (IS_ROCK(levl[bhitpos.x+dx][bhitpos.y+dy].typ) && !IS_FARMLAND(levl[bhitpos.x+dx][bhitpos.y+dy].typ) )
	    || IS_WATERTUNNEL(levl[bhitpos.x+dx][bhitpos.y+dy].typ)
	    || closed_door(bhitpos.x+dx, bhitpos.y+dy)
	    || (levl[bhitpos.x + dx][bhitpos.y + dy].typ == IRONBARS &&
	        hits_bars(&singleobj, bhitpos.x, bhitpos.y, 0, 0))) {

		if (!isok(bhitpos.x+dx,bhitpos.y+dy) || !((u.ux == bhitpos.x+dx) && (u.uy == bhitpos.y+dy)) ) {
		    (void) drop_throw(mon, singleobj, 0, bhitpos.x, bhitpos.y);
		    return;
		}
	}

	/* Note: drop_throw may destroy singleobj.  Since obj must be destroyed
	 * early to avoid the dagger bug, anyone who modifies this code should
	 * be careful not to use either one after it's been freed.
	 */
	if (sym) tmp_at(DISP_FLASH, obj_to_glyph(singleobj));
	while(range-- > 0) { /* Actually the loop is always exited by break */
		bhitpos.x += dx;
		bhitpos.y += dy;

		if (!singleobj) { /* destroyed by hits_bars, bug discovered by amateurhour --Amy */
			pline("Some thin air brushes you!");
			break;
		}

		if ((mtmp = m_at(bhitpos.x, bhitpos.y)) != 0) {
		    if (ohitmon(mon, mtmp, singleobj, range, TRUE))
			break;
		} else if (bhitpos.x == u.ux && bhitpos.y == u.uy) {
		    if (multi) nomul(0, 0, FALSE);

		    if (singleobj->oclass == GEM_CLASS &&
			    singleobj->otyp <= LAST_GEM+9 /* 9 glass colors */
			    && is_unicorn(youmonst.data)) {
			if (singleobj->otyp > LAST_GEM) {
			    You("catch the %s.", xname(singleobj));
			    You("are not interested in %s junk.",
				s_suffix(mon_nam(mon)));
			    makeknown(singleobj->otyp);
			    dropy(singleobj);
			} else {
			    You("accept %s gift in the spirit in which it was intended.",
				s_suffix(mon_nam(mon)));
			    (void)hold_another_object(singleobj,
				"You catch, but drop, %s.", xname(singleobj),
				"You catch:");
			}
			break;
		    }
		    if (singleobj->oclass == POTION_CLASS) {
			if (!Blind) singleobj->dknown = 1;
			potionhit(&youmonst, singleobj, FALSE);
			break;
		    }
		    switch(singleobj->otyp) {
			int dam, hitv;
			case EGG:
			    if (!touch_petrifies(&mons[singleobj->corpsenm])) {
				impossible("monster throwing egg type %d",
					singleobj->corpsenm);
				hitu = 0;
				break;
			    }
			    /* fall through */
			case CREAM_PIE:
			case BLINDING_VENOM:
			    hitu = thitu(8 + (mon->m_lev / 2), 0, singleobj, (char *)0);
			    break;
			default:
			    dam = dmgval(singleobj, &youmonst);
			    if (singleobj->otyp == BOULDER && !rn2(2)) dam += (2 * (mon->m_lev));

			    if (singleobj->otyp == BOULDER && (mon->data == &mons[PM_BOULDER_FART] || mon->data == &mons[PM_FIRM_BOULDER_FART])) {
				pline("%s produces %s farting noises with %s %s butt.", Monnam(mon), !rn2(2) ? "loud" : "disgusting", mhis(mon), mon->female ? "sexy" : "ugly");
				u.cnd_fartingcount++;
				if (Role_if(PM_BUTT_LOVER) && !rn2(20)) buttlovertrigger();
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				if (!extralongsqueak()) badeffect();
			    }

			    hitv = 3 - distmin(u.ux,u.uy, mon->mx,mon->my);
			    if (hitv < -4) hitv = -4;
			    if (is_elf(mon->data) &&
				objects[singleobj->otyp].oc_skill == P_BOW) {
				hitv++;
				if (MON_WEP(mon) &&
				    MON_WEP(mon)->otyp == ELVEN_BOW)
				    hitv++;
				if(singleobj->otyp == ELVEN_ARROW) dam++;
			    }
			    if (bigmonst(youmonst.data)) hitv++;
				/* high-level monsters sometimes deal extra damage --Amy */
				if (mon->m_lev >= 10 && !rn2(2)) {
					hitv += 8 + singleobj->spe + ((mon->m_lev - 9) / 2);
				} else {
					hitv += 8 + singleobj->spe;
				}
			    if (dam < 1) dam = 1;
			    if (mon && mon->data == &mons[PM_UTIMA_DESTROYER_OF_XEREN]) dam += rnd(100);
			    hitu = thitu(hitv, dam, singleobj, (char *)0);
		    }
		    if (hitu && singleobj->opoisoned) {
			char onmbuf[BUFSZ], knmbuf[BUFSZ];

			strcpy(onmbuf, xname(singleobj));
			strcpy(knmbuf, killer_xname(singleobj));
			poisoned(onmbuf, A_STR, knmbuf, -10);
		    }
		    if(hitu &&
		       can_blnd((struct monst*)0, &youmonst,
				(uchar)(singleobj->otyp == BLINDING_VENOM ?
					AT_SPIT : AT_WEAP), singleobj)) {
			blindinc = rnd(25);
			if(singleobj->otyp == CREAM_PIE) {
			    if(!Blind) pline("Yecch!  You've been creamed.");
			    else pline("There's %s sticky all over your %s.",
				       something,
				       body_part(FACE));
			} else if(singleobj->otyp == BLINDING_VENOM) {
			    int num_eyes = eyecount(youmonst.data);
			    /* venom in the eyes */
			    if(!Blind) pline_The("venom blinds you.");
			    else Your("%s sting%s.",
				      (num_eyes == 1) ? body_part(EYE) :
						makeplural(body_part(EYE)),
				      (num_eyes == 1) ? "s" : "");
			}
		    }
		    if (hitu && singleobj->otyp == FAERIE_FLOSS_RHING) {
			    losexp("a sweet ring of faerie floss", TRUE, FALSE);
		    } /* This ignores level-drain resistance (not a bug). --Amy */

		    if (hitu && singleobj->otyp == COLLUSION_KNIFE && !(Race_if(PM_PLAYER_NIBELUNG) && rn2(5))) {
				pline("Collusion!");
				litroomlite(FALSE);
		    }
		    if (hitu && singleobj->otyp == DARKNESS_CLUB&& !(Race_if(PM_PLAYER_NIBELUNG) && rn2(5))) {
				pline("Collusion!");
				litroomlite(FALSE);
		    }
		    if (hitu && singleobj->otyp == YITH_TENTACLE) {
				increasesanity(rnz(monster_difficulty() + 1));
		    }
		    if (hitu && singleobj->otyp == NASTYPOLE && !rn2(10)) {
				badeffect();
		    }
		    if (hitu && singleobj->otyp == DISINTEGRATION_BAR) {
			if ((!Disint_resistance || !rn2(StrongDisint_resistance ? 1000 : 100) || (evilfriday && (uarms || uarmc || uarm || uarmu)) ) && !rn2(10)) {
				You_feel("like you're falling apart!");
	
				if (uarms) {
				    /* destroy shield; other possessions are safe */
				    if (!(EDisint_resistance & W_ARMS)) (void) destroy_arm(uarms);
				} else if (uarmc) {
				    /* destroy cloak; other possessions are safe */
				    if (!(EDisint_resistance & W_ARMC)) (void) destroy_arm(uarmc);
				} else if (uarm) {
				    /* destroy suit */
				    if (!(EDisint_resistance & W_ARM)) (void) destroy_arm(uarm);
				} else if (uarmu) {
				    /* destroy shirt */
				    if (!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
				} else {
					if (u.uhpmax > 20) {
						u.uhpmax -= rnd(20);
						if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
						losehp(rnz(100 + level_difficulty()), "click click click click click you died", KILLED_BY);

					} else {
						u.youaredead = 1;
						done(DIED);
						u.youaredead = 0;
					}
				}
	
			}
		    }
		    if (hitu && singleobj->otyp == PETRIFYIUM_BAR) {
			    if ((!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
				!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
				if (!Stoned) {
					if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
					else {
						Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
						u.cnd_stoningcount++;
						pline("You start turning to stone!");
					}
				}
				sprintf(killer_buf, "petrifyium bar");
				delayed_killer = killer_buf;
		
			    }
		    }

		    if (hitu && singleobj->otyp == EGG) {
			if ((!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
				if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
				else if (Stoned) pline("You are already stoned.");
				else {
					You("start turning to stone!");
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					delayed_killer = "thrown petrifying egg";
				}
			}
		    }
		    stop_occupation();
		    if (hitu || !range) {
                        (void) drop_throw(mon, singleobj, hitu, u.ux, u.uy);
			break;
		    }
		}
		if (!range	/* reached end of path */
			/* missile hits edge of screen */
			|| !isok(bhitpos.x+dx,bhitpos.y+dy)
			/* missile hits the wall */
			|| (IS_ROCK(levl[bhitpos.x+dx][bhitpos.y+dy].typ) && !IS_FARMLAND(levl[bhitpos.x+dx][bhitpos.y+dy].typ))
			/* missile hit closed door */
			|| closed_door(bhitpos.x+dx, bhitpos.y+dy)
			/* missile might hit bars */
			|| (levl[bhitpos.x+dx][bhitpos.y+dy].typ == IRONBARS &&
		        hits_bars(&singleobj, bhitpos.x, bhitpos.y, !rn2(5), 0))
			/* Thrown objects "sink" */
			|| IS_SINK(levl[bhitpos.x][bhitpos.y].typ)
			|| IS_WATERTUNNEL(levl[bhitpos.x][bhitpos.y].typ)
								) {

			if (!range || IS_SINK(levl[bhitpos.x][bhitpos.y].typ) || IS_WATERTUNNEL(levl[bhitpos.x][bhitpos.y].typ) || !isok(bhitpos.x+dx,bhitpos.y+dy) || !((u.ux == bhitpos.x+dx) && (u.uy == bhitpos.y+dy)) ) {
			    if (singleobj) /* hits_bars might have destroyed it */
				(void) drop_throw(mon, singleobj, 0, bhitpos.x, bhitpos.y);
			    break;
			}

		}
		tmp_at(bhitpos.x, bhitpos.y);
		delay_output();
	}
	tmp_at(bhitpos.x, bhitpos.y);
	delay_output();
	tmp_at(DISP_END, 0);

	if (blindinc) {
		u.ucreamed += blindinc;
		make_blinded(Blinded + (long)blindinc, FALSE);
		if (!Blind) Your("%s", vision_clears);
	}
}

#endif /* OVL1 */
#ifdef OVLB

/* Remove an item from the monster's inventory and destroy it. */
void
m_useup(mon, obj)
struct monst *mon;
struct obj *obj;
{
	if (obj->quan > 1L) {
		obj->quan--;
		obj->owt = weight(obj);
	} else {
		obj_extract_self(obj);
		possibly_unwield(mon, FALSE);
		if (obj->owornmask) {
		    mon->misc_worn_check &= ~(obj->owornmask);
		    update_mon_intrinsics(mon, obj, FALSE, FALSE);
		}
		obfree(obj, (struct obj*) 0);
	}
}

#endif /* OVLB */
#ifdef OVL1

/* monster attempts ranged weapon attack against player */
void
thrwmu(mtmp)
struct monst *mtmp;
{
	struct obj *otmp, *mwep;
	xchar x, y;
	schar skill;
	int multishot;
	const char *onm;
	int chance;

	int polelimit = POLE_LIM;

	/* Rearranged beginning so monsters can use polearms not in a line */
	if (mtmp->weapon_check == NEED_WEAPON || !MON_WEP(mtmp)) {
	    mtmp->weapon_check = NEED_RANGED_WEAPON;
	    /* mon_wield_item resets weapon_check as appropriate */
	    if(mon_wield_item(mtmp) != 0) return;
	}

	/* Pick a weapon */
	otmp = select_rwep(mtmp, FALSE);
	if (!otmp) return;

	if ((MON_WEP(mtmp) == otmp) && is_applypole(otmp)) {
	    int dam, hitv;

		if (otmp->otyp == NOOB_POLLAX || otmp->otyp == GREAT_POLLAX) polelimit += 5;
		if (otmp->otyp == YITH_TENTACLE) polelimit += 2;
		if (otmp->otyp == POLE_LANTERN) polelimit += 10;
		if (otmp->otyp == NASTYPOLE) polelimit += 8;
		if (otmp->oartifact == ART_ETHER_PENETRATOR) polelimit += 5;
		if (otmp->oartifact == ART_FUURKER) polelimit += 6;
		if (otmp->otyp == WOODEN_BAR) polelimit += 7;
		if (otmp->oartifact == ART_OVERLONG_STICK) polelimit += 12;
		/* monsters cheat and ignore the increased minimum range :P */

	    if (dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) > polelimit ||
		    !couldsee(mtmp->mx, mtmp->my))
		return;	/* Out of range, or intervening wall */

	    if (canseemon(mtmp)) {
		onm = xname(otmp);
		pline("%s thrusts %s.", Monnam(mtmp),
		      obj_is_pname(otmp) ? the(onm) : an(onm));
	    }

	    dam = dmgval(otmp, &youmonst);
	    hitv = 3 - distmin(u.ux,u.uy, mtmp->mx,mtmp->my);
	    if (hitv < -4) hitv = -4;
	    if (bigmonst(youmonst.data)) hitv++;
	    hitv += 8 + otmp->spe;
	    if (dam < 1) dam = 1;

	    (void) thitu(hitv, dam, otmp, (char *)0);
	    stop_occupation();
	    return;
	}

	x = mtmp->mx;
	y = mtmp->my;
	/* If you are coming toward the monster, the monster
	 * should try to soften you up with missiles.  If you are
	 * going away, you are probably hurt or running.  Give
	 * chase, but if you are getting too far away, throw.
	 */
	/* WAC Catch this since rn2(0) is illegal */
	chance = (BOLT_LIM - distmin(x,y,mtmp->mux,mtmp->muy) > 0) ?
		BOLT_LIM - distmin(x,y,mtmp->mux,mtmp->muy) : 1;
	if (!lined_upB(mtmp) || (URETREATING(x,y) && rn2(chance)))
	    return;

	skill = objects[otmp->otyp].oc_skill;
	mwep = MON_WEP(mtmp);		/* wielded weapon */

	if (!(elongation_monster(mtmp->data) || ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone()) && mwep && ammo_and_launcher(otmp, mwep) && objects[mwep->otyp].oc_range &&
		dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) >
		objects[mwep->otyp].oc_range * objects[mwep->otyp].oc_range) 
		return; /* Out of range */

	/* monsters were throwing darts way across the map, that is, distances of 70+ squares.
	 * This was obviously not intended; they should just be able to fire sniper rifles at their actual range. --Amy */
	else if (!(elongation_monster(mtmp->data) || ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone()) && !(mwep && ammo_and_launcher(otmp, mwep) && objects[mwep->otyp].oc_range) && dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) > ((BOLT_LIM + strongmonst(mtmp->data) ) * (BOLT_LIM + strongmonst(mtmp->data) )) ) return;

	/* Multishot calculations */
	multishot = 1;
	if (((mwep && ammo_and_launcher(otmp, mwep)) || skill == P_DAGGER || skill == P_KNIFE || skill == P_BOOMERANG || skill == -P_BOOMERANG ||
		skill == -P_DART || skill == -P_SHURIKEN || skill == P_SPEAR || skill == P_JAVELIN) && !mtmp->mconf) {
	    /* Assumes lords are skilled, princes are expert */
	    if (is_prince(mtmp->data)) multishot += 2;
	    else if (is_lord(mtmp->data)) multishot++;

		/* strong, nasty or high-level monsters can also shoot more --Amy */
		if (mtmp->m_lev >= 10 && strongmonst(mtmp->data) && !rn2(3)) multishot++;
		if (mtmp->m_lev >= 10 && strongmonst(mtmp->data) && !rn2(9)) multishot++;
		if (mtmp->m_lev >= 10 && strongmonst(mtmp->data) && !rn2(27)) multishot++;

		if (mtmp->m_lev >= 10 && extra_nasty(mtmp->data) && !rn2(2)) multishot++;
		if (mtmp->m_lev >= 10 && extra_nasty(mtmp->data) && !rn2(4)) multishot++;
		if (mtmp->m_lev >= 10 && extra_nasty(mtmp->data) && !rn2(8)) multishot++;

		if (mtmp->m_lev >= 10 && mtmp->m_lev < 20) multishot += 1;
		if (mtmp->m_lev >= 20 && mtmp->m_lev < 30) multishot += rnd(2);
		if (mtmp->m_lev >= 30 && mtmp->m_lev < 40) multishot += rnd(3);
		if (mtmp->m_lev >= 40 && mtmp->m_lev < 50) multishot += rnd(4);
		if (mtmp->m_lev >= 50 && mtmp->m_lev < 60) multishot += rnd(5);
		if (mtmp->m_lev >= 60 && mtmp->m_lev < 70) multishot += rnd(6);
		if (mtmp->m_lev >= 70 && mtmp->m_lev < 80) multishot += rnd(7);
		if (mtmp->m_lev >= 80 && mtmp->m_lev < 90) multishot += rnd(8);
		if (mtmp->m_lev >= 90 && mtmp->m_lev < 100) multishot += rnd(9);
		if (mtmp->m_lev >= 100) multishot += rnd(10);

	    /*  Elven Craftsmanship makes for light,  quick bows */
	    if (otmp->otyp == ELVEN_ARROW && !otmp->cursed)
		multishot++;
	    if (mwep && mwep->otyp == ELVEN_BOW && !mwep->cursed) multishot++;

	    if (mwep && mwep->otyp == WILDHILD_BOW && otmp->otyp == ODOR_SHOT) multishot++;
	    if (mwep && mwep->otyp == COMPOST_BOW && otmp->otyp == FORBIDDEN_ARROW) multishot++;

	    if (mwep && mwep->otyp == CATAPULT) multishot += rnd(5);

	    if (mwep && mwep->otyp == HYDRA_BOW) multishot += 2;
	    if (mwep && mwep->otyp == DEMON_CROSSBOW) multishot += 4;
	    if (mwep && mwep->otyp == WILDHILD_BOW) multishot += 2;

	    /* 1/3 of object enchantment */
	    if (mwep && mwep->spe > 1)
		multishot += rounddiv(mwep->spe, 3);
	    /* Some randomness */
	    if (multishot > 1)
		multishot = rnd(multishot);
	    if (mwep && objects[mwep->otyp].oc_rof && is_launcher(mwep))
		multishot += objects[mwep->otyp].oc_rof;

	    switch (monsndx(mtmp->data)) {
	    case PM_SPARD:
	    case PM_IBERIAN_SOLDIER:
		    multishot += 3;
		    break;
	    case PM_BLUE_ARCHER:
		    multishot += 2;
		    break;
	    case PM_RANGER:
	    case PM_ROCKER:
	    case PM_GATLING_ARCHER:
		    multishot++;
		    break;
	    case PM_PELLET_ARCHER:
	    case PM_ECM_ARCHER:
	    case PM_SHOTGUN_HORROR:
	    case PM_SHOTGUN_TERROR:
	    case PM_KOBOLD_PEPPERMASTER:
		    multishot++;
		    multishot++;
		    break;
	    case PM_BRA_GIANT:
		    multishot += 5;
		    break;
	    case PM_ELPH:
		    multishot++;
		    if (otmp->otyp == ELVEN_ARROW && mwep && mwep->otyp == ELVEN_BOW) multishot++;
		    break;
	    case PM_ROGUE:
		    if (skill == P_DAGGER) multishot++;
		    break;
	    case PM_TOSSER:
		    if (skill == P_JAVELIN) multishot++;
		    break;
	    case PM_NINJA:
	    case PM_NINJA_GAIDEN:
	    case PM_SAMURAI:
		    if (otmp->otyp == YA && mwep &&
			mwep->otyp == YUMI) multishot++;
		    break;
	    default:
		break;
	    }
	    /* racial bonus */
	    if ((is_elf(mtmp->data) &&
		    otmp->otyp == ELVEN_ARROW &&
		    mwep && mwep->otyp == ELVEN_BOW) ||
		(is_orc(mtmp->data) &&
		    otmp->otyp == ORCISH_ARROW &&
		    mwep && mwep->otyp == ORCISH_BOW))
		multishot++;

		/* weaker monsters shouldn't spam you with thousands of arrows --Amy */
		if (!rn2(2) && !strongmonst(mtmp->data) && !extra_nasty(mtmp->data) && !(mtmp->data->geno & G_UNIQ) && multishot > 1) multishot -= rnd(multishot / 2);

	    if ((long)multishot > otmp->quan) multishot = (int)otmp->quan;
	    if (multishot < 1) multishot = 1;
	    /* else multishot = rnd(multishot); */
	}

	if (canseemon(mtmp)) {
	    char onmbuf[BUFSZ];

	    if (multishot > 1) {
		/* "N arrows"; multishot > 1 implies otmp->quan > 1, so
		   xname()'s result will already be pluralized */
		sprintf(onmbuf, "%d %s", multishot, xname(otmp));
		onm = onmbuf;
	    } else {
		/* "an arrow" */
		onm = singular(otmp, xname);
		onm = obj_is_pname(otmp) ? the(onm) : an(onm);
	    }
	    m_shot.s = (mwep && ammo_and_launcher(otmp,mwep)) ? TRUE : FALSE;
	    pline("%s %s %s!", Monnam(mtmp),
		  m_shot.s ? is_bullet(otmp) ? "fires" : "shoots" : "throws",
		  onm);
	    m_shot.o = otmp->otyp;
	} else {
	    if (flags.soundok && !issoviet) You_hear("a flinging sound."); /* at least tell the player that something's happening --Amy */
	    m_shot.o = STRANGE_OBJECT;	/* don't give multishot feedback */
	}

	m_shot.n = multishot;
	for (m_shot.i = 1; m_shot.i <= m_shot.n; m_shot.i++)
	    m_throw(mtmp, mtmp->mx, mtmp->my, sgn(tbx), sgn(tby),
		    distmin(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy), otmp);
	m_shot.n = m_shot.i = 0;
	m_shot.o = STRANGE_OBJECT;
	m_shot.s = FALSE;

	nomul(0, 0, FALSE);
}

#endif /* OVL1 */
#ifdef OVLB

int
spitmu(mtmp, mattk)		/* monster spits substance at you */
register struct monst *mtmp;
register struct attack *mattk;
{
	register struct obj *otmp;

	if(mtmp->mcan) {

	    if(flags.soundok)
		pline("A dry rattle comes from %s throat.",
		                      s_suffix(mon_nam(mtmp)));
	    return 0;
	}
	if(lined_up(mtmp)) {
		if (issegfaulter && rn2(10)) {
			otmp = mksobj(SEGFAULT_VENOM, TRUE, FALSE, FALSE);
		} else switch (mattk->adtyp) {
		    case AD_BLND:
		    case AD_DRST:
			otmp = mksobj(BLINDING_VENOM, TRUE, FALSE, FALSE);
			break;
		    case AD_DRLI:
			otmp = mksobj(FAERIE_FLOSS_RHING, TRUE, FALSE, FALSE);
			break;
		    case AD_NAST:
			otmp = mksobj(SEGFAULT_VENOM, TRUE, FALSE, FALSE);
			break;
		    case AD_TCKL:
			otmp = mksobj(TAIL_SPIKES, TRUE, FALSE, FALSE);
			break;
		    default:
			pline("bad attack type in spitmu");
				/* fall through */
		    case AD_ACID:
			otmp = mksobj(ACID_VENOM, TRUE, FALSE, FALSE);
			break;
		}
		if (!otmp) return 0;

		if(!rn2(BOLT_LIM-distmin(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy))) {
		    if (canseemon(mtmp))
			pline("%s spits venom!", Monnam(mtmp));
		    else if (flags.soundok && !issoviet) You_hear("a spitting sound.");
		    m_throw(mtmp, mtmp->mx, mtmp->my, sgn(tbx), sgn(tby),
			distmin(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy), otmp);
		    nomul(0, 0, FALSE);
		    return 0;
		}
	}
	return 0;
}

#endif /* OVLB */
#ifdef OVL1

int
breamu(mtmp, mattk)			/* monster breathes at you (ranged) */
	register struct monst *mtmp;
	register struct attack  *mattk;
{
	/* if new breath types are added, change AD_ACID to max type */
	int typ = (mattk->adtyp == AD_RBRE) ? rnd(AD_SPC2) : mattk->adtyp ;

	if (typ < AD_MAGM || typ > AD_SPC2) typ = rnd(AD_SPC2); /* for shambling horrors etc. --Amy */

	if(lined_up(mtmp)) {

	    if(mtmp->mcan) {
		if(flags.soundok) {
		    if(canseemon(mtmp))
			pline("%s coughs.", Monnam(mtmp));
		    else
			You_hear("a cough.");
		}
		return(0);
	    }
	    if(!mtmp->mspec_used && rn2(3)) {

		if((typ >= AD_MAGM) && (typ <= AD_SPC2)) {

			/* angband has the infamous "it breathes" deaths --Amy */

			if (isangbander && rn2(2) && (rnd( (int)mattk->damn * 10) > u.uhp )) {

			u.youaredead = 1;

			pline("It breathes"); /* The following --More-- prompt is forced and shall not be disabled --Amy */
			display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline("You have died.");

			killer_format = KILLED_BY_AN;
			killer = "fatal breath attack";
			done(DIED);

			u.youaredead = 0;

			return(0);
			}

			if (isangbander && rn2(2) && (rnd( (int)mattk->damd * 10) > u.uhp )) {

			u.youaredead = 1;

			pline("It breathes");
			display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			pline("You have died.");

			killer_format = KILLED_BY_AN;
			killer = "fatal breath attack";
			done(DIED);

			u.youaredead = 0;

			return(0);
			}

		/* In Soviet Russia, no player may ever know anything. Whether it's a dragon breathing at you,
		 * a gnome trying to shoot you with his crossbow, or a cobra spitting venom... "if you can't see it,
		 * it doesn't exist". Or rather, "if there was no message about it in SLASH'EM then there may not be one
		 * here either". Yes, it's unbelievable, but that's what they are thinking. --Amy */

		    if(canseemon(mtmp))
			pline("%s breathes %s!", Monnam(mtmp),
			      FunnyHallu ? hallubreathwep[rn2(SIZE(hallubreathwep))] : breathwep[typ-1]);
		    else if (flags.soundok && !issoviet) {
				if (isangbander) pline("It breathes.");
				else You_hear("an exhaling sound.");
			}
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		    buzz((int) (-20 - (typ-1)), (rn2(2) ? (int)mattk->damn : (int)mattk->damd ),
			 mtmp->mx, mtmp->my, sgn(tbx), sgn(tby));
		    nomul(0, 0, FALSE);
		    /* breath runs out sometimes. Also, give monster some
		     * cunning; don't breath if the player fell asleep.
		     */
		    if(!rn2(3))
			mtmp->mspec_used = 10+rn2(20);
		    if(typ == AD_SLEE && !Sleep_resistance)
			mtmp->mspec_used += rnd(20);
		} else pline("Breath weapon %d used", typ-1);
	    }
	}
	return(1);
}


/* WAC for doorbusting ONLY (at this point in time) No checks */
boolean
breamspot(mtmp, mattk, ax, ay)
register struct monst *mtmp;
register struct attack  *mattk;
xchar ax, ay;
{
	/* if new breath types are added, change AD_ACID to max type */
	int typ = (mattk->adtyp == AD_RBRE) ? rnd(AD_SPC2) : mattk->adtyp ;


	if((typ >= AD_MAGM) && (typ <= AD_SPC2)) {
		if(canseemon(mtmp))
			pline("%s breathes %s!", Monnam(mtmp),
				FunnyHallu ? hallubreathwep[rn2(SIZE(hallubreathwep))] : breathwep[typ-1]);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		/* Do the door first - monster is ON TOP so call direct */
		zap_over_floor(mtmp->mx, mtmp->my, (int) (-20 - (typ-1)), NULL);
		buzz((int) (-20 - (typ-1)), (rn2(2) ? (int)mattk->damn : (int)mattk->damd ), 
				mtmp->mx, mtmp->my, ax, ay);
		nomul(0, 0, FALSE);
		/* breath runs out sometimes. */
		if(!rn2(3))
			mtmp->mspec_used = 10+rn2(20);
	} else impossible("Breath weapon %d used", typ-1);
	return(TRUE);
}


boolean
linedup(ax, ay, bx, by, special)
register xchar ax, ay, bx, by;
boolean special; /* for monsters that can shoot from infinite distance --Amy */
{
	int dx, dy;
	tbx = ax - bx;	/* These two values are set for use */
	tby = ay - by;	/* after successful return.	    */

	/* sometimes displacement makes a monster think that you're at its
	   own location; prevent it from throwing and zapping in that case */
	if (!tbx && !tby) return FALSE;

    if ((!tbx || !tby || abs(tbx) == abs(tby)) /* straight line or diagonal */
        && distmin(tbx, tby, 0, 0) < ((special || ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone()) ? 100 : EnglandMode ? 10 : BOLT_LIM) ) {
        if ((ax == u.ux && ay == u.uy) ? (boolean) couldsee(bx, by) : clear_path(ax, ay, bx, by))
            return TRUE;
        /* don't have line of sight, but might still be lined up
           if that lack of sight is due solely to boulders */
        dx = sgn(ax - bx), dy = sgn(ay - by);
        do {
            /* <bx,by> is guaranteed to eventually converge with <ax,ay> */
            bx += dx, by += dy;
            if ((IS_ROCK(levl[bx][by].typ) && !IS_FARMLAND(levl[bx][by].typ)) || IS_WATERTUNNEL(levl[bx][by].typ) || closed_door(bx, by))
                return FALSE;
        } while (bx != ax || by != ay);
        /* reached target position without encountering obstacle */
            return TRUE;
    }

	/*if((!tbx || !tby || abs(tbx) == abs(tby))*/ /* straight line or diagonal */
	/*   && distmin(tbx, tby, 0, 0) < ((ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone()) ? 100 : EnglandMode ? 10 : BOLT_LIM)) {
	    if(ax == u.ux && ay == u.uy) return((boolean)(couldsee(bx,by)));
	    else if(clear_path(ax,ay,bx,by)) return TRUE;
	}*/
	return FALSE;
}

boolean
linedupB(ax, ay, bx, by) /* without the distance check --Amy */
register xchar ax, ay, bx, by;
{
	int dx, dy;
	tbx = ax - bx;	/* These two values are set for use */
	tby = ay - by;	/* after successful return.	    */

	/* sometimes displacement makes a monster think that you're at its
	   own location; prevent it from throwing and zapping in that case */
	if (!tbx && !tby) return FALSE;

    if ((!tbx || !tby || abs(tbx) == abs(tby)) /* straight line or diagonal */
        ) {
        if ((ax == u.ux && ay == u.uy) ? (boolean) couldsee(bx, by) : clear_path(ax, ay, bx, by))
            return TRUE;
        /* don't have line of sight, but might still be lined up
           if that lack of sight is due solely to boulders */
        dx = sgn(ax - bx), dy = sgn(ay - by);
        do {
            /* <bx,by> is guaranteed to eventually converge with <ax,ay> */
            bx += dx, by += dy;
            if ((IS_ROCK(levl[bx][by].typ) && !IS_FARMLAND(levl[bx][by].typ)) || IS_WATERTUNNEL(levl[bx][by].typ) || closed_door(bx, by))
                return FALSE;
        } while (bx != ax || by != ay);
        /* reached target position without encountering obstacle */
            return TRUE;
    }

	/*if((!tbx || !tby || abs(tbx) == abs(tby)))*/ /* straight line or diagonal */
	/*{
	    if(ax == u.ux && ay == u.uy) return((boolean)(couldsee(bx,by)));
	    else if(clear_path(ax,ay,bx,by)) return TRUE;
	}*/
	return FALSE;
}

boolean
lined_up(mtmp)		/* is mtmp in position to use ranged attack? */
	register struct monst *mtmp;
{
	return(linedup(mtmp->mux,mtmp->muy,mtmp->mx,mtmp->my, (elongation_monster(mtmp->data)) ));
}

boolean
lined_upB(mtmp)		/* is mtmp in position to use ranged attack? */
	register struct monst *mtmp;
{
	return(linedupB(mtmp->mux,mtmp->muy,mtmp->mx,mtmp->my));
}

#endif /* OVL1 */
#ifdef OVL0

/* Check if a monster is carrying a particular item.
 */
struct obj *
m_carrying(mtmp, type)
struct monst *mtmp;
int type;
{
	register struct obj *otmp;

	for(otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		if(otmp->otyp == type)
			return(otmp);
	return((struct obj *) 0);
}

/* TRUE iff thrown/kicked/rolled object doesn't pass through iron bars */
boolean
hits_bars(obj_p, x, y, always_hit, whodidit)
struct obj **obj_p;	/* *obj_p will be set to NULL if object breaks */
int x, y;
int always_hit;	/* caller can force a hit for items which would fit through */
int whodidit;	/* 1==hero, 0=other, -1==just check whether it'll pass thru */
{
    struct obj *otmp = *obj_p;
    int obj_type = otmp->otyp;
    boolean hits = always_hit;

    if (!hits)
	switch (otmp->oclass) {
	case WEAPON_CLASS:
	    {
		int oskill = objects[obj_type].oc_skill;

		hits = (oskill != -P_BOW  && oskill != -P_CROSSBOW &&
			oskill != -P_DART && oskill != -P_SHURIKEN &&
			(oskill != -P_FIREARM || obj_type == ROCKET) &&
			oskill != P_SPEAR && oskill != P_JAVELIN &&
			oskill != P_KNIFE);	/* but not dagger */
		break;
	    }
	case ARMOR_CLASS:
		hits = (objects[obj_type].oc_armcat != ARM_GLOVES);
		break;
	case TOOL_CLASS:
		hits = (obj_type != SKELETON_KEY &&
			obj_type != SECRET_KEY &&
			obj_type != LOCK_PICK &&
			obj_type != HAIRCLIP &&
			obj_type != CREDIT_CARD &&
			obj_type != DATA_CHIP &&
			obj_type != TALLOW_CANDLE &&
			obj_type != WAX_CANDLE &&
			obj_type != JAPAN_WAX_CANDLE &&
			obj_type != OIL_CANDLE &&
			obj_type != UNAFFECTED_CANDLE &&
			obj_type != SPECIFIC_CANDLE &&
			obj_type != __CANDLE &&
			obj_type != NATURAL_CANDLE &&
			obj_type != UNSPECIFIED_CANDLE &&
			obj_type != GENERAL_CANDLE &&
			obj_type != LENSES &&
			obj_type != RADIOGLASSES &&
			obj_type != BOSS_VISOR &&
			obj_type != TIN_WHISTLE &&
			obj_type != GRASS_WHISTLE &&
			obj_type != MAGIC_WHISTLE);
		break;
	case ROCK_CLASS:	/* includes boulder */
		if (obj_type != STATUE ||
			mons[otmp->corpsenm].msize > MZ_TINY) hits = TRUE;
		break;
	case FOOD_CLASS:
		if (obj_type == CORPSE &&
			mons[otmp->corpsenm].msize > MZ_TINY) hits = TRUE;
		else
		    hits = (obj_type == MEAT_STICK ||
			    obj_type == HUGE_CHUNK_OF_MEAT);
		break;
	case SPBOOK_CLASS:
	case WAND_CLASS:
	case BALL_CLASS:
	case CHAIN_CLASS:
		hits = TRUE;
		break;
	default:
		break;
	}

    if (hits && whodidit != -1) {
	if (whodidit ? hero_breaks(otmp, x, y, FALSE) : breaks(otmp, x, y))
	    *obj_p = otmp = 0;		/* object is now gone */
	    /* breakage makes its own noises */
	else if (obj_type == BOULDER || obj_type == HEAVY_IRON_BALL)
	    pline("Whang!");
	else if (otmp->oclass == COIN_CLASS ||
		objects[obj_type].oc_material == MT_GOLD ||
		objects[obj_type].oc_material == MT_SILVER)
	    pline("Clink!");
	else
	    pline("Clonk!");
    }

    return hits;
}

/* Find a target for a ranged attack. From dnethack (thanks Chris_ANG). Here in SLEX, it's meant to be specifically for
 * hostile monsters attacking your pets at range. */
struct monst *
mfind_target(mtmp, force_linedup)
struct monst *mtmp;
boolean force_linedup;
{
	int dir, origdir = -1;
	int x, y, dx, dy, tbx, tby;

	int i;

	struct monst *mat, *mret = (struct monst *)0, *oldmret = (struct monst *)0;
	struct monst *mtmp2;

	if (mtmp->mpeaceful || mtmp->mtame) return 0;

	struct obj *mrwep = select_rwep(mtmp, TRUE); /* may use polearm even when far from the player --Amy */

	for (mtmp2 = fmon; mtmp2; mtmp2 = mtmp2->nmon) {
		if(mtmp == mtmp2) continue;
		if (mtmp2 && u.usteed && mtmp2 == u.usteed) continue; /* steeds are fragile enough already... --Amy */
		if (mtmp2->mtame && (mlined_up(mtmp, mtmp2, FALSE) || attacktype(mtmp->data, AT_GAZE) || attacktype(mtmp->data, AT_WEAP)) &&
		((attacktype(mtmp->data, AT_GAZE) && !mtmp->mcan)
		|| (attacktype(mtmp->data, AT_MAGC) && !mtmp->mcan && distmin(mtmp2->mx,mtmp2->my,mtmp->mx,mtmp->my) < BOLT_LIM)
		|| (attacktype(mtmp->data, AT_WEAP) && mrwep && distmin(mtmp2->mx,mtmp2->my,mtmp->mx,mtmp->my) < BOLT_LIM)
		|| (attacktype(mtmp->data, AT_BREA) && !mtmp->mcan && distmin(mtmp2->mx,mtmp2->my,mtmp->mx,mtmp->my) < BOLT_LIM)
		|| (attacktype(mtmp->data, AT_BEAM) && distmin(mtmp2->mx,mtmp2->my,mtmp->mx,mtmp->my) < BOLT_LIM)
		|| (attacktype(mtmp->data, AT_SPIT) && !mtmp->mcan && distmin(mtmp2->mx,mtmp2->my,mtmp->mx,mtmp->my) < BOLT_LIM)) )
		{
			if (!mret) {
				if ((!rn2((mtmp2->m_lev > 40) ? 3 : (mtmp2->m_lev > 30) ? 5 : (mtmp2->m_lev > 20) ? 10 : 20) && !(u.usteed && mtmp2 == u.usteed && !rn2(10)) && (!rn2(5) || mtmp2->mcanmove) && (!rn2(5) || (mtmp2->mhpmax > 5 && mtmp2->mhp > (mtmp2->mhpmax / 5) )) && (mtmp2->m_lev > rn2(6)) && ((mtmp->m_lev - mtmp2->m_lev) < (2 + rn2(5)) ) ) || attacktype(mtmp2->data, AT_EXPL) || mtmp->mfrenzied) mret = mtmp2;
			}
		}
	}

	if (mret != (struct monst *)0) {

		if(!mlined_up(mtmp, mret, FALSE) && !attacktype(mtmp->data, AT_GAZE) && !attacktype(mtmp->data, AT_WEAP)) {
			tbx = tby = 0;
		} else return mret;
	}

	/* Nothing lined up? */
	tbx = tby = 0;
	return (struct monst *)0;
}

boolean
mlined_up(mtmp, mdef, breath)	/* From dnethack: is mtmp in position to use ranged attack? */
	register struct monst *mtmp;
	register struct monst *mdef;
	register boolean breath;
{
	struct monst *mat;

	boolean lined_up = linedup(mdef->mx,mdef->my,mtmp->mx,mtmp->my, FALSE);

	int dx = sgn(mdef->mx - mtmp->mx),
	    dy = sgn(mdef->my - mtmp->my);

	int x = mtmp->mx, y = mtmp->my;

	int i = 10; /* arbitrary */

        /* No special checks if confused - can't tell friend from foe */
	if (!lined_up || mtmp->mconf || !mtmp->mtame) return lined_up;

        /* Check for friendlies in the line of fire. */
	for (; !breath || i > 0; --i)
	{
	    x += dx;
	    y += dy;
	    if (!isok(x, y)) break;
		
            if (x == u.ux && y == u.uy) 
	        return FALSE;

	    mat = m_at(x, y);
	    if (mat)
	    {
	        if (!breath && mat == mdef) return lined_up;

		/* Don't hit friendlies - since this is used by hostile monsters, other hostile monsters == friendly */
		if (!mat->mtame && !mat->mpeaceful) return FALSE;
	    }
	}

	return lined_up;
}


#endif /* OVL0 */

/*mthrowu.c*/

