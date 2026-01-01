/*	SCCS Id: @(#)dokick.c	3.4	2003/12/04	*/
/* Copyright (c) Izchak Miller, Mike Stephenson, Steve Linhart, 1989. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "eshk.h"

#define is_bigfoot(x)	((x) == &mons[PM_SASQUATCH])
#define martial()	(martial_bonus_specialmode() || is_bigfoot(youmonst.data) || \
		(uarmf && uarmf->otyp == KICKING_BOOTS) || (uarmf && uarmf->otyp == COMBAT_WEDGES) || (uarmf && uarmf->otyp == STOMPING_BOOTS) )

static NEARDATA struct rm *maploc;
static NEARDATA const char *gate_str;

extern boolean notonhead;	/* for long worms */

STATIC_DCL void kickdmg(struct monst *, BOOLEAN_P);
STATIC_DCL int kick_object(XCHAR_P, XCHAR_P);
STATIC_DCL char *kickstr(char *);
STATIC_DCL void otransit_msg(struct obj *, BOOLEAN_P, long);
STATIC_DCL void drop_to(coord *,SCHAR_P);

static NEARDATA struct obj *kickobj;

static const char kick_passes_thru[] = "kick passes harmlessly through";

STATIC_OVL void
kickdmg(mon, clumsy)
register struct monst *mon;
register boolean clumsy;
{
	register int mdx, mdy;
	/* calculate base kick damage: this uses STR, DEX and CON; if using the buhurt martial arts style and wearing power armor, the multiplier is bigger --Amy */
	int dmg = ( ACURRSTR + ACURR(A_DEX) + ACURR(A_CON) ) / 15;
	if (u.martialstyle == MARTIALSTYLE_BUHURT && uarm && is_power_armor(uarm)) {
		dmg = ( ACURRSTR + ACURR(A_DEX) + ACURR(A_CON) ) / 10;
	}
	int kick_skill = P_NONE;
	int blessed_foot_damage = 0;
	boolean trapkilled = FALSE;

	if (uarmf && uarmf->oartifact == ART_LARISSA_S_ANGER) dmg += 5;
	if (uarmf && uarmf->oartifact == ART_FIGHTBOOST) dmg += 5;
	if (uarmf && uarmf->oartifact == ART_JUEN_S_WEAKNESS) dmg += 5;
	if (uarmf && uarmf->oartifact == ART_JUMP_KICK_ACTION) dmg += 5;
	if (uarmf && uarmf->oartifact == ART_SHIN_KICKING_GAME) dmg += 5;
	if (uarmf && uarmf->oartifact == ART_AIRSHIP_DANCING) dmg += 2;
	if (uarmf && uarmf->oartifact == ART_WILD_SEX_GAME) dmg += 2;
	if (uarmf && uarmf->oartifact == ART_LITTLE_BITCH_IS_RUCTIOUS) dmg += 3;
	if (uarmf && uarmf->oartifact == ART_NE_PROSTO_KRASIVO) dmg += 3;
	if (uarmf && uarmf->oartifact == ART_SILVESTERBLAM) dmg += 3;
	if (uarmf && uarmf->oartifact == ART_FULL_PROGRAM) dmg += 1;
	if (uarmf && uarmf->oartifact == ART_AS_STRONG_AS_BOOTS) dmg += 5;
	if (uarmf && uarmf->oartifact == ART_LEXI_S_POWERKICK) dmg += 5;
	if (uarmf && uarmf->oartifact == ART_ARTHUR_S_HIGH_HEELED_PLATF) dmg += 2;
	if (uarmf && uarmf->oartifact == ART_KATHARINA_S_LOVELINESS) dmg += 10;
	if (uarmf && uarmf->oartifact == ART_FIGHT_WITH_PLATFORM_INA) dmg += 10;
	if (uarmf && uarmf->oartifact == ART_EXCITING_SPFLOTCH) dmg += 2;
	if (uarmf && itemhasappearance(uarmf, APP_SUPERHARD_SANDALS)) dmg += 5;
	if (uwep && uwep->oartifact == ART_BLU_TOE) dmg += 5;
	if (flags.female && uwep && uwep->oartifact == ART_KICKSIN_GIRL) dmg += 5;
	if (uarmf && uarmf->oartifact == ART_FIND_THE_COMBAT_STANCE) dmg += 10;
	if (uarmf && uarmf->oartifact == ART_THICK_PLATFORM_CRAZE) dmg += 5;
	if (uwep && uwep->oartifact == ART_EVA_S_COLORCHANGE) dmg += 3;
	if (uwep && uwep->oartifact == ART_FEETSERN) dmg += 5;
	if (uarm && uarm->oartifact == ART_JEANETTA_S_REVENGE) dmg += 5;
	if (uarmu && uarmu->oartifact == ART_FRIEDERIKE_S_BUNDLING) dmg += 5;
	if (uarmg && uarmg->oartifact == ART_NALI_THE_BNALI) dmg += rn1(8,8);
	if (uarmf && uarmf->otyp == SHOCK_ISOLATED_HEELS) {
		dmg += 2;
		if (verysmall(mon->data)) dmg += 5;
	}

	if (u.ulevel >= 30) dmg += rnd(4);
	else if (u.ulevel >= 24) dmg += rnd(3);
	else if (u.ulevel >= 16) dmg += rnd(2);
	else if (u.ulevel >= 8) dmg += 1;

	if (u.martialstyle == MARTIALSTYLE_TAEKWONDO) {
		dmg += (1 + (GushLevel / 3));
	}

	if (uwep && uwep->oartifact == ART_SYSETTE_S_THIEVINGNESS) {
		dmg += 7;
		if (PlayerInBlockHeels) dmg += 8;
	}

	if (uarmf && uarmf->oartifact == ART_SIGRUN_S_WEDDING_SHOES) dmg += rnz(5); /* a rare instance where the player gets rnz-based damage bonuses --Amy */

	if (uarmf && uarmf->oartifact == ART_FANNY_S_BROOK_THAT_YOU_MIS) dmg += (flags.female ? 7 : 2);

	if (uarmf && uarmf->oartifact == ART_ARVOGENIA_S_BIKER_HEELS && u.usteed) dmg += 5;

	if (Race_if(PM_TAYIIN)) dmg += 2;

	if (uarmf && uarmf->oartifact == ART_SCRATCH_THE_SHRED && mon->mhp < (mon->mhpmax / 10) ) {
		You("scratch the last shred away.");
		dmg += 10000;
	}

	if (uarmf && itemhasappearance(uarmf, APP_ALLIGATOR_PUMPS) && !rn2(1000) && !(mon->data->geno & G_UNIQ) && !bigmonst(mon->data) ) {
		pline("*gulp*");
		dmg += 10000;
	}

	if (uarmf && uarmf->oartifact == ART_SAY_THE_WRONG_LINE && humanoid(mon->data) && !(mon->female) ) {
		dmg += 2;
		if (!rn2(100)) {
			pline("Oh no, you said the wrong line, namely the one that allows %s to pee into your boots!", mon_nam(mon));
			curse(uarmf);
			if (uarmf->spe > -20) uarmf->spe--;
			pline("Due to %s's evil penis, your boots are defiled now...", mon_nam(mon));
			return;
		}
	}

	if (uarmf && uarmf->otyp == KICKING_BOOTS)
	    dmg += 5;
	if (uarmf && uarmf->otyp == COMBAT_WEDGES)
	    dmg += 5;
	if (uarmf && uarmf->otyp == STOMPING_BOOTS)
	    dmg += 7;
	if (RngeKicking) dmg += 5;

	if (!(PlayerCannotUseSkills)) {

		if (uarmf && (uarmf->otyp == KICKING_BOOTS) && P_SKILL(P_MARTIAL_ARTS) == P_SKILLED) dmg += 2;
		if (uarmf && (uarmf->otyp == KICKING_BOOTS) && P_SKILL(P_MARTIAL_ARTS) == P_EXPERT) dmg += 3;
		if (uarmf && (uarmf->otyp == KICKING_BOOTS) && P_SKILL(P_MARTIAL_ARTS) == P_MASTER) dmg += 5;
		if (uarmf && (uarmf->otyp == KICKING_BOOTS) && P_SKILL(P_MARTIAL_ARTS) == P_GRAND_MASTER) dmg += 6;
		if (uarmf && (uarmf->otyp == KICKING_BOOTS) && P_SKILL(P_MARTIAL_ARTS) == P_SUPREME_MASTER) dmg += 8;

		if (uarmf && (uarmf->otyp == COMBAT_WEDGES) && P_SKILL(P_MARTIAL_ARTS) == P_SKILLED) dmg += 2;
		if (uarmf && (uarmf->otyp == COMBAT_WEDGES) && P_SKILL(P_MARTIAL_ARTS) == P_EXPERT) dmg += 3;
		if (uarmf && (uarmf->otyp == COMBAT_WEDGES) && P_SKILL(P_MARTIAL_ARTS) == P_MASTER) dmg += 5;
		if (uarmf && (uarmf->otyp == COMBAT_WEDGES) && P_SKILL(P_MARTIAL_ARTS) == P_GRAND_MASTER) dmg += 6;
		if (uarmf && (uarmf->otyp == COMBAT_WEDGES) && P_SKILL(P_MARTIAL_ARTS) == P_SUPREME_MASTER) dmg += 8;

		if (uarmf && (uarmf->otyp == STOMPING_BOOTS) && P_SKILL(P_MARTIAL_ARTS) == P_SKILLED) dmg += 2;
		if (uarmf && (uarmf->otyp == STOMPING_BOOTS) && P_SKILL(P_MARTIAL_ARTS) == P_EXPERT) dmg += 4;
		if (uarmf && (uarmf->otyp == STOMPING_BOOTS) && P_SKILL(P_MARTIAL_ARTS) == P_MASTER) dmg += 6;
		if (uarmf && (uarmf->otyp == STOMPING_BOOTS) && P_SKILL(P_MARTIAL_ARTS) == P_GRAND_MASTER) dmg += 8;
		if (uarmf && (uarmf->otyp == STOMPING_BOOTS) && P_SKILL(P_MARTIAL_ARTS) == P_SUPREME_MASTER) dmg += 10;

	}

	if (uarmf && uarmf->otyp == WEDGE_SANDALS && Race_if(PM_NAVI) )
	    dmg += 5;

	if (!(PlayerCannotUseSkills)) {

		if (uarmf && (uarmf->otyp == WEDGE_SANDALS) && Race_if(PM_NAVI) && P_SKILL(P_MARTIAL_ARTS) == P_SKILLED) dmg += 1;
		if (uarmf && (uarmf->otyp == WEDGE_SANDALS) && Race_if(PM_NAVI) && P_SKILL(P_MARTIAL_ARTS) == P_EXPERT) dmg += 2;
		if (uarmf && (uarmf->otyp == WEDGE_SANDALS) && Race_if(PM_NAVI) && P_SKILL(P_MARTIAL_ARTS) == P_MASTER) dmg += 3;
		if (uarmf && (uarmf->otyp == WEDGE_SANDALS) && Race_if(PM_NAVI) && P_SKILL(P_MARTIAL_ARTS) == P_GRAND_MASTER) dmg += 4;
		if (uarmf && (uarmf->otyp == WEDGE_SANDALS) && Race_if(PM_NAVI) && P_SKILL(P_MARTIAL_ARTS) == P_SUPREME_MASTER) dmg += 5;

	}

	if (uarmf && uarmf->otyp == WEDGE_SANDALS)
	    dmg += 2;

	if (!(PlayerCannotUseSkills)) {

		if (uarmf && (uarmf->otyp == WEDGE_SANDALS) && P_SKILL(P_MARTIAL_ARTS) == P_SKILLED && rn2(2)) dmg += 1;
		if (uarmf && (uarmf->otyp == WEDGE_SANDALS) && P_SKILL(P_MARTIAL_ARTS) == P_EXPERT) dmg += 1;
		if (uarmf && (uarmf->otyp == WEDGE_SANDALS) && P_SKILL(P_MARTIAL_ARTS) == P_MASTER) dmg += 2;
		if (uarmf && (uarmf->otyp == WEDGE_SANDALS) && P_SKILL(P_MARTIAL_ARTS) == P_GRAND_MASTER) dmg += 3;
		if (uarmf && (uarmf->otyp == WEDGE_SANDALS) && P_SKILL(P_MARTIAL_ARTS) == P_SUPREME_MASTER) dmg += 4;

	}

	if (!PlayerCannotUseSkills && PlayerInPyramidalHeels) {
		if (u.pyramidalskill >= 20) dmg += 4;
		if (u.pyramidalskill >= 160) dmg += 4;
		if (u.pyramidalskill >= 540) dmg += 4;
		if (u.pyramidalskill >= 1280) dmg += 4;
		if (u.pyramidalskill >= 2500) dmg += 4;
		if (u.pyramidalskill >= 4320) dmg += 4;
	}

	if (uarmf && uarmf->otyp == STAIRWELL_STOMPING_BOOTS)
	    dmg += 2;

	if (!(PlayerCannotUseSkills)) {

		if (uarmf && (uarmf->otyp == STAIRWELL_STOMPING_BOOTS) && P_SKILL(P_MARTIAL_ARTS) == P_SKILLED) dmg += 1;
		if (uarmf && (uarmf->otyp == STAIRWELL_STOMPING_BOOTS) && P_SKILL(P_MARTIAL_ARTS) == P_EXPERT) dmg += 2;
		if (uarmf && (uarmf->otyp == STAIRWELL_STOMPING_BOOTS) && P_SKILL(P_MARTIAL_ARTS) == P_MASTER) dmg += 3;
		if (uarmf && (uarmf->otyp == STAIRWELL_STOMPING_BOOTS) && P_SKILL(P_MARTIAL_ARTS) == P_GRAND_MASTER) dmg += 4;
		if (uarmf && (uarmf->otyp == STAIRWELL_STOMPING_BOOTS) && P_SKILL(P_MARTIAL_ARTS) == P_SUPREME_MASTER) dmg += 5;

	}

	if (uarmf && uarmf->otyp == STILETTO_SANDALS)
	    dmg += 2;

	if (!(PlayerCannotUseSkills)) {

		if (uarmf && (uarmf->otyp == STILETTO_SANDALS) && P_SKILL(P_MARTIAL_ARTS) == P_SKILLED) dmg += 1;
		if (uarmf && (uarmf->otyp == STILETTO_SANDALS) && P_SKILL(P_MARTIAL_ARTS) == P_EXPERT) dmg += 2;
		if (uarmf && (uarmf->otyp == STILETTO_SANDALS) && P_SKILL(P_MARTIAL_ARTS) == P_MASTER) dmg += 4;
		if (uarmf && (uarmf->otyp == STILETTO_SANDALS) && P_SKILL(P_MARTIAL_ARTS) == P_GRAND_MASTER) dmg += 5;
		if (uarmf && (uarmf->otyp == STILETTO_SANDALS) && P_SKILL(P_MARTIAL_ARTS) == P_SUPREME_MASTER) dmg += 7;

	}

	if (uarmf && uarmf->otyp == COMBAT_STILETTOS)
	    dmg += 3;

	if (!(PlayerCannotUseSkills)) {

		if (uarmf && (uarmf->otyp == COMBAT_STILETTOS) && P_SKILL(P_MARTIAL_ARTS) == P_SKILLED) dmg += 2;
		if (uarmf && (uarmf->otyp == COMBAT_STILETTOS) && P_SKILL(P_MARTIAL_ARTS) == P_EXPERT) dmg += 4;
		if (uarmf && (uarmf->otyp == COMBAT_STILETTOS) && P_SKILL(P_MARTIAL_ARTS) == P_MASTER) dmg += 6;
		if (uarmf && (uarmf->otyp == COMBAT_STILETTOS) && P_SKILL(P_MARTIAL_ARTS) == P_GRAND_MASTER) dmg += 8;
		if (uarmf && (uarmf->otyp == COMBAT_STILETTOS) && P_SKILL(P_MARTIAL_ARTS) == P_SUPREME_MASTER) dmg += 10;

	}

	if (uarmf && uarmf->otyp == LEATHER_PEEP_TOES)
	    dmg += 2;

	if (!(PlayerCannotUseSkills)) {

		if (uarmf && (uarmf->otyp == LEATHER_PEEP_TOES) && P_SKILL(P_MARTIAL_ARTS) == P_SKILLED) dmg += 1;
		if (uarmf && (uarmf->otyp == LEATHER_PEEP_TOES) && P_SKILL(P_MARTIAL_ARTS) == P_EXPERT) dmg += 3;
		if (uarmf && (uarmf->otyp == LEATHER_PEEP_TOES) && P_SKILL(P_MARTIAL_ARTS) == P_MASTER) dmg += 5;
		if (uarmf && (uarmf->otyp == LEATHER_PEEP_TOES) && P_SKILL(P_MARTIAL_ARTS) == P_GRAND_MASTER) dmg += 7;
		if (uarmf && (uarmf->otyp == LEATHER_PEEP_TOES) && P_SKILL(P_MARTIAL_ARTS) == P_SUPREME_MASTER) dmg += 9;

	}

	if (uarmf && uarmf->otyp == HIPPIE_HEELS)
	    dmg += 4;

	if (!(PlayerCannotUseSkills)) {

		if (uarmf && (uarmf->otyp == HIPPIE_HEELS) && P_SKILL(P_MARTIAL_ARTS) == P_SKILLED) dmg += 3;
		if (uarmf && (uarmf->otyp == HIPPIE_HEELS) && P_SKILL(P_MARTIAL_ARTS) == P_EXPERT) dmg += 5;
		if (uarmf && (uarmf->otyp == HIPPIE_HEELS) && P_SKILL(P_MARTIAL_ARTS) == P_MASTER) dmg += 8;
		if (uarmf && (uarmf->otyp == HIPPIE_HEELS) && P_SKILL(P_MARTIAL_ARTS) == P_GRAND_MASTER) dmg += 10;
		if (uarmf && (uarmf->otyp == HIPPIE_HEELS) && P_SKILL(P_MARTIAL_ARTS) == P_SUPREME_MASTER) dmg += 13;

	}

	if (!PlayerCannotUseSkills && uarmf && uarmf->oartifact == ART_MEASURER_TIP) {
		switch (P_SKILL(P_KNIFE)) {

			case P_BASIC:		dmg +=  rnd(3); break;
			case P_SKILLED:	dmg +=  rnd(6); break;
			case P_EXPERT:	dmg +=  rnd(9); break;
			case P_MASTER:	dmg +=  rnd(12); break;
			case P_GRAND_MASTER:	dmg +=  rnd(15); break;
			case P_SUPREME_MASTER:	dmg +=  rnd(18); break;
			default: dmg += 0; break;

		}

	}

	if ((!(PlayerCannotUseSkills)) && PlayerInHighHeels) {

		switch (P_SKILL(P_HIGH_HEELS)) {

			case P_BASIC:		dmg +=  1; break;
			case P_SKILLED:	dmg +=  2; break;
			case P_EXPERT:	dmg +=  3; break;
			case P_MASTER:	dmg +=  4; break;
			case P_GRAND_MASTER:	dmg +=  5; break;
			case P_SUPREME_MASTER:	dmg +=  6; break;
			default: dmg += 0; break;

		}

	}

	if (uarmf && uarmf->oartifact == ART_ANASTASIA_S_PLAYTIME && !(mon->mflee)) {
		if (!resist(mon, TOOL_CLASS, 0, NOTELL)) {
			monflee(mon, rn1(10,10), FALSE, FALSE, FALSE);
			pline("%s turns to flee!", Monnam(mon));
		}
	}

	if (uarmf && uarmf->oartifact == ART_SLAM_INTO && humanoid(mon->data) && !is_neuter(mon->data) && !(mon->female)) {
		dmg += 8;
		if (!resist(mon, TOOL_CLASS, 0, NOTELL)) {
			pline("That kick really knocked the wind out of %s.", mon_nam(mon));
			mon_adjust_speed(mon, -1, (struct obj *)0);
			mon->mstun = TRUE;
			monflee(mon, rnd(10), FALSE, FALSE, FALSE);
			mon->healblock += rnd(10);
			if (!rn2(10)) cancelmonsterlite(mon);
		}
	}

	if (uarmf && uarmf->oartifact == ART_SUNA_S_CONTROVERSY_MATTER && uarmf->oeroded2 >= 1) {
		if (!resist(mon, ARMOR_CLASS, 0, NOTELL)) {
			mon_adjust_speed(mon, -1, (struct obj *)0 );
			m_dowear(mon, FALSE); /* might want speed boots */
			if (!rn2(10)) cancelmonsterlite(mon);
		}
	}

	if (uarmf && uarmf->oartifact == ART_JUEN_S_ROLLOVER && mon->mcanmove && !rn2(10)) {
		if (!resist(mon, ARMOR_CLASS, 0, NOTELL)) {
			mon->mcanmove = 0;
			mon->mfrozen = rnd(12);
			mon->mstrategy &= ~STRAT_WAITFORU;
			pline("You roll over %s with your roller blades!", mon_nam(mon));
		}
	}

	if (uarmf && uarmf->oartifact == ART_SINGHETA && mon->mcanmove) {
		if (!resist(mon, ARMOR_CLASS, 0, NOTELL)) {
			mon->mcanmove = 0;
			mon->mfrozen = 2;
			mon->mstrategy &= ~STRAT_WAITFORU;
			pline("%s can't fight back!", Monnam(mon));
		}
	}

	if (uarmf && uarmf->otyp == EGGKICK_SHOES && humanoid(mon->data) && !(mon->female) && !is_neuter(mon->data) ) {
		dmg += 5;
	}

	if (Race_if(PM_DUTHOL) && PlayerInBlockHeels) dmg += 2;

	if (uwep && uwep->oartifact == ART_PEEPLUE) dmg += 2;

	if (uwep && uwep->oartifact == ART_ON_CLOUD_SEVEN) dmg += 2;

	if (uarmf && uarmf->oartifact == ART_EROTICLAMP && u.ustuck && !u.uswallow && !sticks(youmonst.data)) dmg += 2;

	if (uarmf && uarmf->otyp == FEMININE_PUMPS && uarmf->spe >= 1 && !rn2(2))
		dmg += uarmf->spe;

	if (!(PlayerCannotUseSkills)) {

	if (uarmf && uarmf->otyp == FEMININE_PUMPS && uarmf->spe >= 1 && !rn2(3) && P_SKILL(P_MARTIAL_ARTS) >= P_SKILLED) dmg += uarmf->spe;
	if (uarmf && uarmf->otyp == FEMININE_PUMPS && uarmf->spe >= 1 && !rn2(5) && P_SKILL(P_MARTIAL_ARTS) >= P_EXPERT) dmg += uarmf->spe;
	if (uarmf && uarmf->otyp == FEMININE_PUMPS && uarmf->spe >= 1 && !rn2(7) && P_SKILL(P_MARTIAL_ARTS) >= P_MASTER) dmg += uarmf->spe;
	if (uarmf && uarmf->otyp == FEMININE_PUMPS && uarmf->spe >= 1 && !rn2(10) && P_SKILL(P_MARTIAL_ARTS) >= P_GRAND_MASTER) dmg += uarmf->spe;
	if (uarmf && uarmf->otyp == FEMININE_PUMPS && uarmf->spe >= 1 && !rn2(12) && P_SKILL(P_MARTIAL_ARTS) >= P_SUPREME_MASTER) dmg += uarmf->spe;

	}

	if (uarmf && !rn2(3) && itemhasappearance(uarmf, APP_PLOF_HEELS) ) {
		pline("*plof*");
		dmg += rnd(15);
	}

	if (uarmf && itemhasappearance(uarmf, APP_BAYONET_HEELS) && !PlayerCannotUseSkills && P_SKILL(P_SHORT_SWORD) >= P_BASIC) {
		switch (P_SKILL(P_SHORT_SWORD)) {
			case P_BASIC: dmg += 1; break;
			case P_SKILLED: dmg += 2; break;
			case P_EXPERT: dmg += 3; break;
			case P_MASTER: dmg += 4; break;
			case P_GRAND_MASTER: dmg += 5; break;
			case P_SUPREME_MASTER: dmg += 6; break;
		}
	}

	if (uwep && uwep->oartifact == ART_BARANA_S_MISS) dmg += 10;
	if (uarmf && itemhasappearance(uarmf, APP_BATTLE_BOOTS)) dmg += 5;
	if (uarmf && itemhasappearance(uarmf, APP_PLATFORM_SNEAKERS)) dmg += 2;
	if (uarmf && uarmf->oartifact == ART_EVELINE_S_LOVELIES) dmg += 5;
	if (uarmf && uarmf->oartifact == ART_MANDY_S_ROUGH_BEAUTY) dmg += 10;
	if (uarmf && uarmf->oartifact == ART_KYLIE_LUM_S_SNAKESKIN_BOOT) dmg += 10;
	if (uarmc && uarmc->oartifact == ART_CONNY_S_COMBAT_COAT) dmg += 5;
	if (uarmh && uarmh->oartifact == ART_CLAUDIA_S_SEXY_SCENT) dmg += 10;
	if (uarmh && uarmh->oartifact == ART_HILMA_S_AGE) dmg += rnd(10);

	if (uarmf && itemhasappearance(uarmf, APP_STEEL_TOED_BOOTS) ) dmg++;

	if (uarmf && uarmf->oartifact == ART_HEADCRUNCH && has_head(mon->data) ) dmg += 10;

	if (PlayerInStilettoHeels && uarmf && uarmf->oartifact == ART_VORPAL_HEELS && !rn2(20)) {
		if (has_head(mon->data) && !noncorporeal(mon->data) && !amorphous(mon->data) ) {
			if (mon->data->geno & G_UNIQ) {
				dmg += (GushLevel * 2);
				You("critically kick %s!", mon_nam(mon));
			} else {
				Your("stiletto heels cut off %s's %s!", mon_nam(mon), mbodypart(mon, HEAD));
				dmg += 10000;
			}
		}
	}

	if (uarmf && uarmf->oartifact == ART_ELIANE_S_COMBAT_SNEAKERS && !(mon->data->geno & G_UNIQ) && !rn2(20)) {
		Your("sneakers instantly bludgeon %s.", mon_nam(mon));
		dmg += 10000; /* instant death */
	}

	if (uarmf && itemhasappearance(uarmf, APP_VELCRO_SANDALS)) dmg += rnd(10);

	if (uarmf && uarmf->oartifact == ART_SOFT_KARATE_KICK && !P_RESTRICTED(P_MARTIAL_ARTS)) {
		switch (P_SKILL(P_MARTIAL_ARTS)) {
			case P_UNSKILLED: dmg += 8; break;
			case P_BASIC: dmg += 6; break;
			case P_SKILLED: dmg += 4; break;
			case P_EXPERT: dmg += 2; break;
		}
	}

	if (uarmf && uarmf->oartifact == ART_HUGGING__GROPING_AND_STROK) dmg += 5;
	if (uarmf && uarmf->oartifact == ART_ELENETTES) dmg += 2;
	if (uarmh && uarmh->oartifact == ART_STROKING_COMBAT) dmg += 2;

	if (uarmf && itemhasappearance(uarmf, APP_WEAPON_LIGHT_BOOTS)) {
		dmg += u.ulevel;
		if (uarmf && uarmf->oartifact == ART_SHARP_EDGED_AND_DANGEROUS) {

			if (!u.ualign.sins || (u.ualign.sins && !rn2(u.ualign.sins))) {
				increasesincounter(1);
				u.alignlim--;
				pline("Using such a dangerous pair of boots without permission is very sinful.");
			}
		} else {
			increasesincounter(1);
			u.alignlim--;
			pline("Using such a dangerous pair of boots without permission is very sinful.");
		}
	}

	if (uarmc && uarmc->oartifact == ART_INA_S_SORROW && u.uhunger < 0) dmg += 10;

	if (uarmf && uarmf->oartifact == ART_MADELEINE_S_GIRL_FOOTSTEPS) adjalign(1);

	/* quarterback is expert at kicking --Amy */
	if (Role_if(PM_QUARTERBACK) && dmg > 0) dmg += rnd(dmg);

	if (uarmf && itemhasappearance(uarmf, APP_YELLOW_SNEAKERS)) dmg *= 2;

	if (uarmf && itemhasappearance(uarmf, APP_CALF_LEATHER_SANDALS)) clumsy = FALSE;
	if (u.martialstyle == MARTIALSTYLE_KUNGFU && !rn2(3)) clumsy = FALSE;
	if (u.martialstyle == MARTIALSTYLE_KARATE) clumsy = FALSE;

	if (uarmf && uarmf->oartifact == ART_MAILIE_S_CHALLENGE) clumsy = FALSE;
	if (uwep && uwep->oartifact == ART_INSECTMASHER) clumsy = FALSE;
	if (uwep && uwep->oartifact == ART_BLU_TOE) clumsy = FALSE;
	if (uarmf && uarmf->oartifact == ART_FRONT_TARGET) clumsy = FALSE;
	if (uarmf && uarmf->oartifact == ART_ELENETTES) clumsy = FALSE;
	if (uarmc && uarmc->oartifact == ART_MARC_S_MANAFILL && !flags.female) clumsy = FALSE;
	if (uarmf && uarmf->oartifact == ART_EVIL_HAIRTEAR) clumsy = FALSE;
	if (uarmf && uarmf->oartifact == ART_JOHN_S_REDBLOCK) clumsy = FALSE;

	/* excessive wt affects dex, so it affects dmg */
	if (clumsy) dmg /= 2;

	/* kicking a dragon or an elephant will not harm it */
	if (thick_skinned(mon->data) && dmg && !(uarmf && uarmf->oartifact == ART_HUGGING__GROPING_AND_STROK) ) dmg = 1;
	if ((uarmf && itemhasappearance(uarmf, APP_CALF_LEATHER_SANDALS)) && dmg) dmg = 1;

	/* high heels are the elder priest's kryptonite; he's thick-skinned so this must come after the above line */
	if (PlayerInHighHeels && (mon->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_])) {

		dmg += 50;
		pline("Your high heels severely wound the elder priest's tentacles!");

	}

	if (mon->data == &mons[PM_PLOF_ANJE]) {
		dmg += rnd(10);
		pline("*plof*");
	}

	if (uarmf && uarmf->oartifact == ART_SEXY_PUMPS_OF_RAGNAROK) {

		ragnarok(FALSE);
		if (evilfriday && u.ulevel > 1) evilragnarok(FALSE,u.ulevel);
	}

	if (u.femcombostrike && PlayerInConeHeels) {
		dmg += u.femcombostrike;
		pline("You scratch %s for %d extra damage!", mon_nam(mon), u.femcombostrike);
	}
	if (tech_inuse(T_FEMALE_COMBO) && PlayerInConeHeels) {
		u.femcombostrike += 2;
		u.femcomboactive = TRUE;
	}

	if (u.martialstyle == MARTIALSTYLE_KINAMUTAY && !mon->female && humanoid(mon->data)) {
		int kinachance = 0;
		if (!PlayerCannotUseSkills) {
			switch (P_SKILL(P_SQUEAKING)) {
				case P_BASIC: kinachance = 1; break;
				case P_SKILLED: kinachance = 2; break;
				case P_EXPERT: kinachance = 3; break;
				case P_MASTER: kinachance = 4; break;
				case P_GRAND_MASTER: kinachance = 5; break;
				case P_SUPREME_MASTER: kinachance = 6; break;
			}
		}

		if (kinachance > rn2(20)) {
			You("fully kick %s in the nuts!", mon_nam(mon));

			dmg += rnd(10);
			mon->mcanmove = 0;
			if (mon->mfrozen < 110) mon->mfrozen += rn1(10,5);
			mon->mstrategy &= ~STRAT_WAITFORU;

			adjalign(-20);
			if (!rn2(50)) {
				increasesincounter(1);
				u.alignlim--;
				pline_The("gods are probably not very pleased about you fighting dirty, though.");
			}
		}
	}

	if (uarmh && uarmh->oartifact == ART_NYPHERISBANE && (mon->data->mlet == S_SNAKE || mon->data->mlet == S_NAGA) ) {
		You("totally stomp that stupid snake.");
		dmg += 100;
	}

	if (uarmf && uarmf->oartifact == ART_LENG_S_KRYPTONITE && (mon->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_])) {

		dmg += 50;
		pline("SPLAT! The elder priest's tentacles are crushed underneath your very lovely high heels!");
		/* unfortunately he has enough HP that he can still survive even though he already took 50 due to the heels */

	}

	if (PlayerInHighHeels && attacktype(mon->data, AT_TENT)) {
		dmg += 2;
		if (uarmf && uarmf->spe > 0) dmg += uarmf->spe;

		if (mon->data != &mons[PM_DNETHACK_ELDER_PRIEST_TM_]) pline("Your high heels hurt %s's tentacles!", mon_nam(mon));

	}

	/* attacking a shade is useless */
	if ( (is_shade(mon->data) || mon->egotype_shader) && !(uwep && uwep->oartifact == ART_AP_) && !(uarmf && (objects[uarmf->otyp].oc_material == MT_SILVER || objects[uarmf->otyp].oc_material == MT_ARCANIUM)) )
	    dmg = 0;

	if ((is_undead(mon->data) || is_demon(mon->data)) && uarmf &&
		uarmf->blessed)
	    blessed_foot_damage = 1;

	if ( (is_shade(mon->data) || mon->egotype_shader) && !(uwep && uwep->oartifact == ART_AP_) && !(uarmf && (objects[uarmf->otyp].oc_material == MT_SILVER || objects[uarmf->otyp].oc_material == MT_ARCANIUM)) && !blessed_foot_damage) {
	    pline_The("%s.", kick_passes_thru);
	    /* doesn't exercise skill or abuse alignment or frighten pet,
	       and shades have no passive counterattack */
	    return;
	}

	if(mon->m_ap_type) seemimic(mon);

	check_caitiff(mon);

	/* squeeze some guilt feelings... */
	if(mon->mtame) {
	    abuse_dog(mon);
	    if (mon->mtame)
		monflee(mon, (dmg ? rnd(dmg) : 1), FALSE, FALSE, FALSE);
	    else
		mon->mflee = 0;
	}

	u.cnd_kickmonstercount++;

	if (dmg > 0) {
		/* convert potential damage to actual damage */
		dmg = rnd(dmg);
		if (martial()) {

		    int dexdamage = ACURR(A_DEX) / 3;
		    if (dexdamage < 0) dexdamage = 0;

		    if (dmg > /*1*/0) kick_skill = P_MARTIAL_ARTS; /* fix by Amy */
		    dmg += rn3(dexdamage + 1);

			/* artifact hit effect (hammerfeet) implemented by stealing code from dnethack --Amy */
			if(uarmf && uarmf->oartifact) {
				if (artifact_hit(&youmonst, mon, uarmf, &dmg, rnd(20)) ) {
					if(mon->mhp <= 0) /* artifact killed monster */
						return;
				}
			}

		    dmg += weapon_dam_bonus((struct obj *)0, TRUE);
			/* marital arts skill is supposed to improve this!!! --Amy */
		    if ((P_SKILL(P_MARTIAL_ARTS) > 0) && !rn2(2)) dmg += (rnd(P_SKILL(P_MARTIAL_ARTS)) * rno(2));
		} else {

			if(uarmf && uarmf->oartifact) {
				if (artifact_hit(&youmonst, mon, uarmf, &dmg, rnd(20)) ) {
					if(mon->mhp <= 0) /* artifact killed monster */
						return;
				}
			}

			if (dmg > 0) kick_skill = P_MARTIAL_ARTS; /* so that acu can unlock it if he wants to */
		}
		/* a good kick exercises your dex */
		exercise(A_DEX, TRUE);
	}
	if (blessed_foot_damage) dmg += rnd(4);
	if (uarmf) dmg += uarmf->spe;

	if (increase_damage_bonus_value() > 1) dmg += rnd(increase_damage_bonus_value());
	else dmg += increase_damage_bonus_value();	/* add ring(s) of increase damage */

	dmg += (Drunken_boxing && Confusion);
	dmg += (StrongDrunken_boxing && Confusion);
	if (RngeBloodlust) dmg += 1;
	if (uarms && uarms->oartifact == ART_TEH_BASH_R) dmg += 2;

	if (Race_if(PM_RODNEYAN)) dmg += (1 + (GushLevel / 3) );

	if (Race_if(PM_PLAYER_SKELETON) && dmg > 1) {
		dmg /= 5;
		if (dmg < 1) dmg = 1;
	}

	if (u.martialstyle == MARTIALSTYLE_BOXING) {
		dmg /= 2;
		if (dmg < 1) dmg = 1;
	}

	if (uarmf && uarmf->oartifact == ART_ELIANE_S_SHIN_SMASH) dmg *= 2;
	if (u.martialstyle == MARTIALSTYLE_KUNGFU) {
		dmg *= 11;
		dmg /= 10;
	}

	/*if (dmg > 127) dmg = 127;*/ /* sanity check... but not actually needed --Amy */

	if (uarmf && uarmf->oartifact == ART_ELENA_S_CHALLENGE && !is_neuter(mon->data) && !(mon->female) && humanoid(mon->data) && !rn2(100) && !resist(mon, TOOL_CLASS, 0, NOTELL) ) {
		pline("%s gets all submissive from being kicked by your beautiful high-heeled combat boots!", Monnam(mon));
		(void) tamedog(mon, (struct obj *) 0, FALSE);
		return;
	}

	if (uarmf && uarmf->oartifact == ART_BARBED_HOOK_ZIPPER && !mon->mfrenzied && !rn2(100) && !resist(mon, TOOL_CLASS, 0, NOTELL) ) {
		pline("%s seems calmer.", Monnam(mon));
		mon->mpeaceful = TRUE;
		return;
	}

	if (dmg > 0) {
		mon->mhp -= dmg;

		/* wuot why isn't this here, the monster took damage so there should be a message ffs! --Amy */
		if (mon->mhp > 0) {
			monster_pain(mon);
			wounds_message(mon);
		}

#ifdef SHOW_DMG
		showdmg(dmg);
#endif
	}
	if (mon->mhp > 0 && (martial() || (uarmf && itemhasappearance(uarmf, APP_BUFFALO_BOOTS)) || (uarmf && itemhasappearance(uarmf, APP_WHITE_BUFFALO_BOOTS)) ) && !bigmonst(mon->data) && !rn2(u.martialstyle == MARTIALSTYLE_TAEKWONDO ? 2 : 3) &&
	    mon->mcanmove && mon != u.ustuck && !mon->mtrapped) {
		/* see if the monster has a place to move into */
		mdx = mon->mx + u.dx;
		mdy = mon->my + u.dy;
		if(goodpos(mdx, mdy, mon, 0)) {
			pline("%s reels from the blow.", Monnam(mon));
			if (m_in_out_region(mon, mdx, mdy)) {
			    remove_monster(mon->mx, mon->my);
			    newsym(mon->mx, mon->my);
			    place_monster(mon, mdx, mdy);
			    newsym(mon->mx, mon->my);
			    set_apparxy(mon);
			    if (mintrap(mon) == 2) trapkilled = TRUE;
			}
		}
	} else if (mon->mhp > 0 && mon != u.ustuck && !mon->mtrapped && rn2(10) && uarmf && itemhasappearance(uarmf, APP_DOCTOR_CLAW_BOOTS) ) {

		switch (rnd(4)) {
			case 1:
				verbalize("Aaa-err!"); break;
			case 2:
				verbalize("Waa-aii!"); break;
			case 3:
				verbalize("Haa-waii!"); break;
			case 4:
				verbalize("Kamehameha!"); break;
		}

		/* see if the monster has a place to move into */
		mdx = mon->mx + u.dx;
		mdy = mon->my + u.dy;
		if(goodpos(mdx, mdy, mon, 0)) {
			pline("%s reels from the blow.", Monnam(mon));
			if (m_in_out_region(mon, mdx, mdy)) {
			    remove_monster(mon->mx, mon->my);
			    newsym(mon->mx, mon->my);
			    place_monster(mon, mdx, mdy);
			    newsym(mon->mx, mon->my);
			    set_apparxy(mon);
			    if (mintrap(mon) == 2) trapkilled = TRUE;
			}
		}
	}

	if (uarmf && uarmf->oartifact == ART_RONJA_S_FEMALE_PUSHING && !rn2(10)) {
		/* see if the monster has a place to move into */
		mdx = mon->mx + u.dx;
		mdy = mon->my + u.dy;
		if(goodpos(mdx, mdy, mon, 0)) {
			pline("%s is pushed back by your fleecy block heels.", Monnam(mon));
			if (m_in_out_region(mon, mdx, mdy)) {
			    remove_monster(mon->mx, mon->my);
			    newsym(mon->mx, mon->my);
			    place_monster(mon, mdx, mdy);
			    newsym(mon->mx, mon->my);
			    set_apparxy(mon);
			    if (mintrap(mon) == 2) trapkilled = TRUE;
			}
		}

	}

	if (uarmf && (itemhasappearance(uarmf, APP_PLATFORM_BOOTS) || itemhasappearance(uarmf, APP_PLATEAU_BOOTS)) && !rn2(3) ) {
		if (!mon->mstun) 	{
			if (rn2(3)) pline("%s is stunned by your strong kick!", Monnam(mon));
			else if (rn2(10)) pline("You stomp %s's %s!", mon_nam(mon), makeplural(mbodypart(mon,TOE)) );
			else pline("You stomp %s's %s with your lovely boots!", mon_nam(mon), makeplural(mbodypart(mon,TOE)) );
		}
		mon->mstun = TRUE;
	}

	if (uarmf && uarmf->oartifact == ART_FINGERMASH && MON_WEP(mon) ) {
		register struct obj *monwepon;
		monwepon = MON_WEP(mon);
		if (monwepon) {
			if (!monwepon->cursed) {
				curse(monwepon);
				pline("%s's weapon becomes cursed!", Monnam(mon));
			}
			if (monwepon->spe > 0) {
				monwepon->spe = 0;
				pline("%s's weapon loses all of its enchantment!", Monnam(mon));
			}
		}

	}

	if (!rn2(10) && MON_WEP(mon) && uarmf && itemhasappearance(uarmf, APP_GENTLE_SNEAKERS) ) {
		register struct obj *monwepon;
		monwepon = MON_WEP(mon);
		if (monwepon) {
			obj_extract_self(monwepon);
			possibly_unwield(mon, FALSE);
			setmnotwielded(mon, monwepon);

		}

		if (monwepon && monwepon->mstartinventB && !(monwepon->oartifact) && !(monwepon->fakeartifact && timebasedlowerchance()) && (!rn2(4) || (rn2(100) < u.equipmentremovechance) || !timebasedlowerchance() ) ) {
			You("vaporize %s %s!", s_suffix(mon_nam(mon)), xname(monwepon));
			delobj(monwepon);
		} else if (monwepon && monwepon->mstartinventC && !(monwepon->oartifact) && !(monwepon->fakeartifact && !rn2(10)) && rn2(10) ) {
			You("vaporize %s %s!", s_suffix(mon_nam(mon)), xname(monwepon));
			delobj(monwepon);
		} else if (monwepon && monwepon->mstartinventE && !(monwepon->oartifact) && !(monwepon->fakeartifact && !rn2(20)) && rn2(20) ) {
			You("vaporize %s %s!", s_suffix(mon_nam(mon)), xname(monwepon));
			delobj(monwepon);
		} else if (monwepon && monwepon->mstartinventD && !(monwepon->oartifact) && !(monwepon->fakeartifact && !rn2(4)) && rn2(4) ) {
			You("vaporize %s %s!", s_suffix(mon_nam(mon)), xname(monwepon));
			delobj(monwepon);
		} else if (monwepon && monwepon->mstartinventX) {
			You("vaporize %s %s!", s_suffix(mon_nam(mon)), xname(monwepon));
			delobj(monwepon);
		} else {
			You("knock %s %s to the %s!", s_suffix(mon_nam(mon)), xname(monwepon), surface(u.ux, u.uy));
			if (monwepon->otyp == CRYSKNIFE && (!monwepon->oerodeproof || !rn2(10))) {
				monwepon->otyp = WORM_TOOTH;
				monwepon->owt = weight(monwepon);
				monwepon->oerodeproof = 0;
			}
			place_object(monwepon, u.ux, u.uy);
			stackobj(monwepon);
		}

	}

	if (RngeToestomping && !rn2(5) && !mon->mstun) {
		pline("You stomp %s's %s!", mon_nam(mon), makeplural(mbodypart(mon,TOE)) );
		mon->mstun = TRUE;

	}

	if (RngeCoquetry && !rn2(5) && !mon->mconf) {
		pline("Your coquetry demoralizes %s!", mon_nam(mon) );
		mon->mconf = TRUE;
	}

    if (uarmf && itemhasappearance(uarmf, APP_VELCRO_BOOTS) ) {
		if (!mon->mstun) 	{
			pline("Your velcro boots joyously scratch along %s's %s, drawing lots of %s!", mon_nam(mon), makeplural(mbodypart(mon,LEG)), mbodypart(mon,BLOOD) );
		}
		mon->mstun = TRUE;

	}

	if (uarmf && uarmf->oartifact == ART_CHLOE_S_EXAGGERATION && mon->mhpmax > 1) {
		pline("Your massive ball heels totally stomp %s.", mon_nam(mon));
		mon->mhpmax--;
		if (mon->mhp > mon->mhpmax) mon->mhp--;
	}

	if (uarmf && uarmf->oartifact == ART_STEFANJE_S_PROBLEM && mon->mhpmax > 1) {
		pline("Your 'Stefanje' sandals stomp %s with their lovely heels.", mon_nam(mon));
		mon->mhpmax--;
		if (mon->mhp > mon->mhpmax) mon->mhp--;
	}

	if (uarmf && uarmf->oartifact == ART_LOVELY_GIRL_PLATEAUS) {

		if (!mon->mstun || !mon->mconf) {
			pline("Wham! Your lovely plateau soles fully kick %s in the ass.", mon_nam(mon) );
			mon->mstun = mon->mconf = TRUE;
		}

	}

	if (uarmf && uarmf->oartifact == ART_TANJA_S_MARTIAL_PROWESS && mon->mcanmove && !PlayerCannotUseSkills) {
		int parlyzchance = 0;

		switch (P_SKILL(P_MARTIAL_ARTS)) {
			case P_BASIC: parlyzchance = 1; break;
			case P_SKILLED: parlyzchance = 2; break;
			case P_EXPERT: parlyzchance = 3; break;
			case P_MASTER: parlyzchance = 4; break;
			case P_GRAND_MASTER: parlyzchance = 5; break;
			case P_SUPREME_MASTER: parlyzchance = 6; break;
		}

		if (parlyzchance > rn2(100)) {
			mon->mcanmove = 0;
			mon->mfrozen = parlyzchance + 1;
			mon->mstrategy &= ~STRAT_WAITFORU;
			You("kick %s so powerfully that %s can't fight back for a bit.", mon_nam(mon), mhe(mon));
		}

	}

	if ((uarmf && itemhasappearance(uarmf, APP_VELCRO_SANDALS)) && !rn2(3) && mon->mcanmove) {
		pline("Your velcro lashes severely hurt %s.", mon_nam(mon) );
		mon->mstun = TRUE;
		mon->mcanmove = 0;
		mon->mfrozen = rnd(10);
		mon->mstrategy &= ~STRAT_WAITFORU;

	}

	if (uarmf && uarmf->oartifact == ART_KRISTIN_S_INNER_FEEL && !rn2(10) && mon->mcanmove) {
		if (!resist(mon, RING_CLASS, 0, NOTELL)) {
			mon->mcanmove = 0;
			mon->mfrozen = rn1(4,4);
			mon->mstrategy &= ~STRAT_WAITFORU;
			pline("Your block heels clamp %s's %s!", mon_nam(mon), makeplural(mbodypart(mon, TOE)) );
		}
	}

	if (uarmf && uarmf->otyp == EGGKICK_SHOES && mon->mcanmove && humanoid(mon->data) && !(mon->female) && !is_neuter(mon->data) && !rn2(20) ) {
		pline("Score! %s's nuts got squeezed very painfully by your kick!", Monnam(mon));
		mon->mcanmove = 0;
		mon->mfrozen = rnd(10);
		mon->mstrategy &= ~STRAT_WAITFORU;
	}

	if (uarmf && uarmf->oartifact == ART_FINAL_CHALLENGE && !rn2(5) && !(mon->female) && !is_neuter(mon->data) && mon->mcanmove) {
		pline("Your sexy block heels fully kick %s in the nuts.", mon_nam(mon));
		mon->mcanmove = 0;
		mon->mfrozen = rnd(5);
		mon->mstrategy &= ~STRAT_WAITFORU;

	}

	if (uarmh && uarmh->oartifact == ART_CLAUDIA_S_SEXY_SCENT && !rn2(4)) {
		pline("%s is paralyzed by your powerful kick!", Monnam(mon));
		mon->mcanmove = 0;
		mon->mfrozen = rnd(10);
		mon->mstrategy &= ~STRAT_WAITFORU;

	}

	if (uarmf && uarmf->oartifact == ART_DO_NOT_PEE && !rn2(5) && !(mon->female) && !is_neuter(mon->data) && !uarmf->oeroded && !uarmf->oeroded2) {
		pline("%s is paralyzed by your powerful kick!", Monnam(mon));
		mon->mcanmove = 0;
		if (!mon->mfrozen) mon->mfrozen = rnd(10);
		else if (mon->mfrozen < 127) mon->mfrozen++;
		mon->mstrategy &= ~STRAT_WAITFORU;
	}

	if (uarmf && uarmf->oartifact == ART_AS_STRONG_AS_BOOTS && !rn2(10) && mon->mcanmove) {
		pline("%s is paralyzed by your powerful kick!", Monnam(mon));
		mon->mcanmove = 0;
		mon->mfrozen = rnd(5);
		mon->mstrategy &= ~STRAT_WAITFORU;
	}

	if (uarmf && itemhasappearance(uarmf, APP_SISTER_SHOES)) {
		mon->bleedout += rnd(5);
		pline("Your very pretty block heels scratch %sy wounds on %s's %s!", mbodypart(mon, BLOOD), mon_nam(mon), makeplural(mbodypart(mon, LEG)) );
	}

	if (uarmh && uarmh->oartifact == ART_HILMA_S_AGE) {
		mon->bleedout += rn1(5,5);
		pline("You violently scratch %s's %s!", mon_nam(mon), mbodypart(mon, LEG));
	}

	if (uarmf && uarmf->oartifact == ART_KARIN_S_SHARP_EDGE) {
		mon->bleedout += rnd(GushLevel);
		pline("Your razor-sharp cone heels scratch very %sy wounds on %s's %s!", mbodypart(mon, BLOOD), mon_nam(mon), makeplural(mbodypart(mon, LEG)) );
	}

	if (uarm && uarm->oartifact == ART_TIMONA_S_INNER_BICKER) {
		mon->bleedout += rnd(5);
		pline("Your very pretty block heels scratch %sy wounds on %s's %s!", mbodypart(mon, BLOOD), mon_nam(mon), makeplural(mbodypart(mon, LEG)) );
	}

	if (uarmf && uarmf->oartifact == ART_ABRASIVE_RUPIKA) {
		mon->bleedout += rnd(6);
		pline("Your rough soles abrade some %s from %s's %s!", mbodypart(mon, BODY_SKIN), mon_nam(mon), makeplural(mbodypart(mon, LEG)) );
	}

	if (uarmf && uarmf->oartifact == ART_KRISTIN_S_CHEAP_EDGE) {
		mon->bleedout += rnd(4);
		pline("Your very pretty block heels scratch %sy wounds on %s's %s!", mbodypart(mon, BLOOD), mon_nam(mon), makeplural(mbodypart(mon, LEG)) );
	}

	if (uwep && uwep->oartifact == ART_TONA_S_GAMES && (PlayerInConeHeels || PlayerInStilettoHeels) ) {
		mon->bleedout += rnd(10);
		pline("Your razor-sharp high heels scratch up and down %s's %s, drawing %s!", mon_nam(mon), makeplural(mbodypart(mon, LEG)), mbodypart(mon, BLOOD) );
	}

	if (uarmf && uarmf->oartifact == ART_BLOOD_BRAND) {
		mon->bleedout += rnd(8);
		pline("Your razor-sharp high heels scratch up and down %s's %s, drawing %s!", mon_nam(mon), makeplural(mbodypart(mon, LEG)), mbodypart(mon, BLOOD) );
	}

	if (uarmf && itemhasappearance(uarmf, APP_BLADED_DISKS)) {
		mon->bleedout += rnd(5);
		pline("Your metal blades slit %s, drawing a lot of %s!", mon_nam(mon), mbodypart(mon, BLOOD) );
	}
	if (uarmf && uarmf->oartifact == ART_HEME_FLOW) {
		mon->bleedout += rnd(10);
	}

	if (uarmf && uarmf->oartifact == ART_DORA_S_SCRATCHY_HEELS) {
		mon->bleedout += rnd(10);
		pline("Your very pretty block heels scratch %sy wounds on %s's %s!", mbodypart(mon, BLOOD), mon_nam(mon), makeplural(mbodypart(mon, LEG)) );
	}

	if (uarmf && uarmf->oartifact == ART_FULL_PROGRAM) {
		mon->bleedout += 3;
	}

	if (uarmf && uarmf->oartifact == ART_SHARP_EDGED_AND_DANGEROUS) {
		mon->bleedout += 10;
	}

	if (uarmf && uarmf->oartifact == ART_SCRATCHE_HUSSY) {
		mon->bleedout += rnd(10);
		pline("Your very pretty block heels scratch %sy wounds on %s's %s!", mbodypart(mon, BLOOD), mon_nam(mon), makeplural(mbodypart(mon, LEG)) );
	}

	if (uarmc && uarmc->oartifact == ART_CONNY_S_COMBAT_COAT && !rn2(10) && mon->mcanmove ) {

		pline("You place an incredibly strong kick at %s's body, who staggers and can't fight back for now.", mon_nam(mon) );
		mon->mstun = TRUE;
		mon->mcanmove = 0;
		mon->mfrozen = rnd(10);
		mon->mstrategy &= ~STRAT_WAITFORU;

	}

	if (FemtrapActiveNatalje) {
		pline("Your sexy high heels stomp %s's %s.", mon_nam(mon), makeplural(mbodypart(mon, TOE)) );
		mon->mcanmove = 0;
		mon->mfrozen += 1;
		mon->mstrategy &= ~STRAT_WAITFORU;

	}

	if (uarmf && uarmf->oartifact == ART_ELIANE_S_SHIN_SMASH && mon->mcanmove) {

		pline("You smash %s's shins with a powerful kick.", mon_nam(mon) );
		mon->mcanmove = 0;
		mon->mfrozen = rnd(10);
		mon->mstrategy &= ~STRAT_WAITFORU;

	}

	if (uarmf && uarmf->oartifact == ART_KYLIE_LUM_S_SNAKESKIN_BOOT && !rn2(4) && mon->mcanmove ) {

		pline("Your very elegant snakeskin boots stop %s in its tracks.", mon_nam(mon) );
		if (FunnyHallu) pline("You wonder if the PETA activists saw that.");
		mon->mcanmove = 0;
		mon->mfrozen = rnd(10);
		mon->mstrategy &= ~STRAT_WAITFORU;

	}

	if (PlayerInStilettoHeels && (humanoid(mon->data) || is_animal(mon->data)) && !(mon->female) && !is_neuter(mon->data) && tech_inuse(T_ASIAN_KICK)) {

		pline("As you drive the stiletto heel up against %s's nuts, his %s gets a shocked expression. You kicked his nuts in such a way that they're forced out of their original position, and several internal blood vessels are lacerated in the process. He falls over unconscious and most certainly won't get up for a long while!", mon_nam(mon), mbodypart(mon, FACE) );
		mon->mcanmove = 0;
		mon->mfrozen += rnz(100);
		mon->mstrategy &= ~STRAT_WAITFORU;

		stopsingletechnique(T_ASIAN_KICK);

	}

	if (PlayerInStilettoHeels && has_head(mon->data) && !(mon->data->geno & G_UNIQ) && !mon->mcanmove && tech_inuse(T_SKULL_CRUSH)) {

		stopsingletechnique(T_SKULL_CRUSH);
		mon->mhp = -1; /* instadeath */
		Your("stiletto heels penetrate %s's skull, splitting it in two.", mon_nam(mon));

	}

	if (PlayerInConeHeels && tech_inuse(T_LEGSCRATCH_ATTACK)) {

		pline("Your tender cone heels scratch up and down %s's %s!", mon_nam(mon), makeplural(mbodypart(mon,LEG)) );
		if (resist(mon, TOOL_CLASS, 0, NOTELL)) {
			pline("But %s shows no indication that your attack did anything...", mon_nam(mon) );
		} else if (mon->permspeed != MSLOW) {
			mon_adjust_speed(mon, -1, (struct obj *)0);
			pline("Your heel scrapes off a bit of %s from %s's %s.", mbodypart(mon, BODY_SKIN), mon_nam(mon), makeplural(mbodypart(mon,LEG)) );

		} else {
			pline("Your heel just doesn't stop scratching over %s's %s, and %s is squirting everywhere!", mon_nam(mon), makeplural(mbodypart(mon,LEG)), mbodypart(mon, BLOOD) );
			mon->mhp -= dmg;

		}

	}

	if (PlayerInWedgeHeels && tech_inuse(T_GROUND_STOMP) && !(mon->mcanmove)) {

		pline("Your wedge heels stomp %s's defenseless body.", mon_nam(mon) );
		mon->mhp -= rnd(10);
		mon->mfrozen += 2;
		mon->mcanmove = 0;
		mon->mstrategy &= ~STRAT_WAITFORU;

	}

	if (uarmf && uarmf->oartifact == ART_SOLVEJG_S_SQUEEZE_IN && !rn2(3) && !mon->msleeping) {
		if (!resist(mon, TOOL_CLASS, 0, NOTELL)) {
			sleep_monst(mon, rnd(10), -1);
			if (mon->msleeping || !(mon->mcanmove)) {
				pline("%s has to grapple with the pain from your girl kick and can't fight back!", Monnam(mon));
			}
			slept_monst(mon);

		}

	}

	if (PlayerInBlockHeels && tech_inuse(T_ATHLETIC_COMBAT) && !mon->msleeping) {

		pline("Your block heels aim a high kick at %s.", mon_nam(mon) );

		if (!resist(mon, TOOL_CLASS, 0, NOTELL)) {

			sleep_monst(mon, rnd(10), -1);
			if (resists_sleep(mon) && !rn2(10) && mon->mcanmove) {
				mon->mcanmove = 0;
				mon->mfrozen = rnd(10);
			}
			if (mon->msleeping || !(mon->mcanmove))
				pline("%s is amazed by your athletic combat and can't fight back!", Monnam(mon));
			slept_monst(mon);

		}

	}

	if (uarmf && itemhasappearance(uarmf, APP_PORCELAIN_SANDALS)) {
		uarmf->spe--;
		if (uarmf->spe < -10) {
			pline("Suddenly, your porcelain sandals break into a thousand fragments.");
			useup(uarmf);
			nomul(0, 0, FALSE);
			return;
		}
	}

	if (uarmf && uarmf->oartifact == ART_WUMSHIN && rn2(4) && mon->mcanmove) {

		pline("Wumm! You kick %s in the shins with your bum bum boots.", mon_nam(mon));
		mon->mcanmove = 0;
		mon->mfrozen = 1;
		mon->mstrategy &= ~STRAT_WAITFORU;

	}

	if (uarmf && uarmf->oartifact == ART_VERA_S_FREEZER) {
		pline("Your very lovely female 'Vera' sneakers clamp %s's %s!", mon_nam(mon), makeplural(mbodypart(mon,TOE)) );
		if (!resist(mon, RING_CLASS, 0, NOTELL)) {
			mon_adjust_speed(mon, -1, (struct obj *)0 );
			m_dowear(mon, FALSE); /* might want speed boots */
		}
	}

	if (uarmf && uarmf->oartifact == ART_VERA_S_ICECUBE_SMASH && (!resists_cold(mon) || player_will_pierce_resistance()) ) {
		pline("Your very lovely female 'Vera' sneakers clamp %s's %s!", mon_nam(mon), makeplural(mbodypart(mon,TOE)) );
		if (!resist(mon, RING_CLASS, 0, NOTELL)) {
			mon_adjust_speed(mon, -1, (struct obj *)0 );
			m_dowear(mon, FALSE); /* might want speed boots */
		}
	}

	if (uarmf && uarmf->oartifact == ART_SHIT_KICKERS) {
		if (!resist(mon, WEAPON_CLASS, 0, NOTELL) && !mon->mconf) {
			mon->mconf = TRUE;
			pline("You kick the shit out of %s!", mon_nam(mon));
		}
		if (!resist(mon, WEAPON_CLASS, 0, NOTELL) && !mon->mstun) {
			mon->mstun = TRUE;
			pline("You absolutely kick %s in the ass!", mon_nam(mon));
		}
	}

	if (uarmf && uarmf->oartifact == ART_KATRIN_S_PARALYSIS && !sticks(mon->data)) {
	    setustuck(mon); /* it's now stuck to you */
	    if (rn2(2)) pline("You hold %s in place by stepping on its %s.", mon_nam(mon), makeplural(mbodypart(mon,TOE)));
	    else pline("The chewing gum clinging to your dancing shoe soles sticks to %s's %s!", mon_nam(mon), makeplural(mbodypart(mon,FOOT)));
		/* supposed to give both messages to be more logical, but players dislike message spam... --Amy */
	}

	if (mon->mhp > 0 && !rn2(100) && uarmf && uarmf->oartifact == ART_SHIN_KICK_OF_LOVE) {
		if (!mon->mpeaceful && !mon->mfrenzied && !resist(mon, RING_CLASS, 0, NOTELL)) {
			mon->mpeaceful = 1;
			pline("%s loves your beautiful sandals and no longer wants to harm you!", Monnam(mon) );
		}

	}

	(void) passive(mon, TRUE, mon->mhp > 0, AT_KICK, FALSE);
	if (mon->mhp <= 0 && !trapkilled) killed(mon);

	/* may bring up a dialog, so put this after all messages */
	if (kick_skill != P_NONE) {	/* exercise proficiency */
	    use_skill(kick_skill, 1);
	}

	if (PlayerInPyramidalHeels && !rn2((flags.female) ? 16 : 150) ) {
		u.pyramidalskill++;
		if (uamul && uamul->oartifact == ART_GREETINGS_FROM_EVI) u.pyramidalskill += 2;
		if (uimplant && uimplant->oartifact == ART_TORSTEN_S_FEMININENESS && !flags.female) u.pyramidalskill += 4;
		if (u.pyramidalskill == 20) You("are now more skilled in type 7: pyramidal heels.");
		if (u.pyramidalskill == 160) You("are now more skilled in type 7: pyramidal heels.");
		if (u.pyramidalskill == 540) You("are now more skilled in type 7: pyramidal heels.");
		if (u.pyramidalskill == 1280) You("are now more skilled in type 7: pyramidal heels.");
		if (u.pyramidalskill == 2500) You("are now more skilled in type 7: pyramidal heels.");
		if (u.pyramidalskill == 4320) You("are now most skilled in type 7: pyramidal heels.");
	}
	if (PlayerInBallHeels && !rn2((flags.female) ? 15 : 100) ) {
		u.ballskill++;
		if (uamul && uamul->oartifact == ART_GREETINGS_FROM_EVI) u.ballskill += 2;
		if (uimplant && uimplant->oartifact == ART_TORSTEN_S_FEMININENESS && !flags.female) u.ballskill += 4;
		if (u.ballskill == 20) You("are now more skilled in type 6: ball heels.");
		if (u.ballskill == 160) You("are now more skilled in type 6: ball heels.");
		if (u.ballskill == 540) You("are now more skilled in type 6: ball heels.");
		if (u.ballskill == 1280) You("are now more skilled in type 6: ball heels.");
		if (u.ballskill == 2500) You("are now more skilled in type 6: ball heels.");
		if (u.ballskill == 4320) You("are now most skilled in type 6: ball heels.");
	}
	if (PlayerInColumnarHeels && !rn2((flags.female) ? 100 : 12) ) {
		u.columnarskill++;
		if (uamul && uamul->oartifact == ART_GREETINGS_FROM_EVI) u.columnarskill += 2;
		if (uimplant && uimplant->oartifact == ART_TORSTEN_S_FEMININENESS && !flags.female) u.columnarskill += 4;
		if (u.columnarskill == 20) You("are now more skilled in type 5: columnar heels.");
		if (u.columnarskill == 160) You("are now more skilled in type 5: columnar heels.");
		if (u.columnarskill == 540) You("are now more skilled in type 5: columnar heels.");
		if (u.columnarskill == 1280) You("are now more skilled in type 5: columnar heels.");
		if (u.columnarskill == 2500) You("are now more skilled in type 5: columnar heels.");
		if (u.columnarskill == 4320) You("are now most skilled in type 5: columnar heels.");
	}
	if (PlayerInStilettoHeels && !rn2((flags.female || Role_if(PM_TRANSVESTITE) || Role_if(PM_TRANSSYLVANIAN)) ? 10 : 50) ) use_skill(P_STILETTO_HEELS, 1);
	if (PlayerInConeHeels && !rn2((flags.female || Role_if(PM_TRANSVESTITE) || Role_if(PM_TRANSSYLVANIAN)) ? 8 : 30) ) use_skill(P_CONE_HEELS, 1);
	if (PlayerInBlockHeels && !rn2(5) ) use_skill(P_BLOCK_HEELS, 1);
	if (PlayerInWedgeHeels && !rn2(5) ) use_skill(P_WEDGE_HEELS, 1);

	if (uarmf && uarmf->oartifact == ART_MEASURER_TIP && (dmg > 0)) {
		use_skill(P_KNIFE, 1);
	}

	if (uarmf && itemhasappearance(uarmf, APP_BAYONET_HEELS) && (dmg > 0) ) {
		use_skill(P_SHORT_SWORD, 1);
	}
}

void
kick_monster(x, y)
register xchar x, y;
{
	register boolean clumsy = FALSE;
	register struct monst *mon = m_at(x, y);
	register int i, j, canhitmon, objenchant;

	bhitpos.x = x;
	bhitpos.y = y;
	if (!attack_checks(mon, TRUE)) return;
	setmangry(mon);

	/* Kick attacks by kicking monsters are normal attacks, not special.
	 * This is almost always worthless, since you can either take one turn
	 * and do all your kicks, or else take one turn and attack the monster
	 * normally, getting all your attacks _including_ all your kicks.
	 * If you have >1 kick attack, you get all of them.
	 */
	if (Upolyd && attacktype(youmonst.data, AT_KICK)) {
	    struct attack *uattk;
	    int sum;
	    schar tmp = find_roll_to_hit(mon);
	    schar roll = 0;

	    for (i = 0; i < NATTK; i++) {
		/* first of two kicks might have provoked counterattack
		   that has incapacitated the hero (ie, floating eye) */
		if (multi < 0) break;

		uattk = &youmonst.data->mattk[i];
		/* we only care about kicking attacks here */
		if (uattk->aatyp != AT_KICK) continue;

		if ( (is_shade(mon->data) || mon->egotype_shader) && !(uwep && uwep->oartifact == ART_AP_) && !(uarmf && (objects[uarmf->otyp].oc_material == MT_SILVER || objects[uarmf->otyp].oc_material == MT_ARCANIUM)) && (!uarmf || !uarmf->blessed)) {
		    /* doesn't matter whether it would have hit or missed,
		       and shades have no passive counterattack */
		    Your("%s %s.", kick_passes_thru, mon_nam(mon));
		    break;	/* skip any additional kicks */
		} else if (tmp > (roll = rnd(20))) {
		    You("kick %s.", mon_nam(mon));
		    wakeup(mon);
		    if (u.martialstyle == MARTIALSTYLE_KARATE) {
			if (!rn2(10)) aggravate();
			wake_nearby();
		    }
		    sum = damageum(mon, uattk);
		    (void)passive(mon, (boolean)(sum > 0), (sum != 2), AT_KICK, FALSE);
		    if (sum == 2)
			break;		/* Defender died */
		} else {
		    missum(mon, tmp, roll, uattk);
		    (void)passive(mon, 0, 1, AT_KICK, FALSE);
		}
	    }
	    return;
	}

	if(Levitation && !rn2(3) && verysmall(mon->data) && (!mon->egotype_flying) &&
	   !is_flyer(mon->data)) {
		pline(FunnyHallu ? "Your legs seem to have a mind of their own!" : "Floating in the air, you miss wildly!");
		exercise(A_DEX, FALSE);
		(void) passive(mon, FALSE, 1, AT_KICK, FALSE);
		return;
	}

	/*STEPHEN WHITE'S NEW CODE */
	canhitmon = 0;
	if (need_one(mon))    canhitmon += 1; 
	if (need_two(mon))    canhitmon += 2;         
	if (need_three(mon))  canhitmon += 3; 
	if (need_four(mon))   canhitmon += 4;         
	if (uarmf && uarmf->oartifact == ART_KILLCAP) canhitmon = 0;
	if (uwep && uwep->oartifact == ART_AP_) canhitmon = 0;

	if (Role_if(PM_MONK) && !Upolyd) {
		if (!uwep && !uarm && !uarms) objenchant = GushLevel / 4;
		else if (!uwep) objenchant = GushLevel / 12;
		else objenchant = 0;
		if (objenchant < 0) objenchant = 0;
		if (uarmf && (uarmf->spe > 0)) objenchant += uarmf->spe;
	} else if (uarmf)
		objenchant = uarmf->spe;
	else objenchant = 0;

	if ((objenchant < canhitmon) && ((canhitmon > 0) || (uarmf && uarmf->oartifact == ART_BUGBOOTS_OF_ERRORNESS) ) && !Upolyd && (issoviet || !rn2(isfriday ? 2 : 3) ) ) {
		if (!issoviet) Your("attack doesn't seem to harm %s.",
			mon_nam(mon));
		else pline("Etot monstr ne mozhet byt' povrezhden, potomu chto Sovetskiy khochet nesmotrya vas.");
		(void) passive(mon, FALSE, 1, TRUE, FALSE);
		return;
	}

	if (mon->egotype_flickerer) {
		pline("%s flickers and is impervious to melee and missile attacks!", Monnam(mon));
		if (!rn2(30)) pline("Reminder: you must use something else in order to damage this monster!");
		else if (!rn2(30)) pline("You read that right: your attacks are doing no damage at all!");
		else if (!rn2(30)) pline("Hint: try offensive wands or spells.");

		(void) passive(mon, FALSE, 1, TRUE, FALSE);
		return;
	}

	if (mon->data == &mons[PM_DTTN_ERROR] && rn2(10)) {

		pline("%s absorbed the attack and took no damage!", Monnam(mon));
		(void) passive(mon, FALSE, 1, TRUE, FALSE);
		return;
	}

	if (mon->data == &mons[PM_LITTLE_POISON_IVY] || mon->data == &mons[PM_IWLWIFI_FIRMWARE_BLOB] || mon->data == &mons[PM_SUPERDEEP_TYPE] || mon->data == &mons[PM_AGULA] || mon->data == &mons[PM_FLUIDATOR_IVE] || mon->data == &mons[PM_CRITICALLY_INJURED_PERCENTS] || mon->data == &mons[PM_AMBER_FEMMY] || mon->data == &mons[PM_MISTER_GRIBBS] || mon->data == &mons[PM_IMMUNITY_VIRUS] || mon->data == &mons[PM_UNGENOCIDABLE_VAMPSHIFTER] || mon->data == &mons[PM_TERRIFYING_POISON_IVY] || mon->data == &mons[PM_GIRL_WITH_THE_MOST_BEAUTIFUL_SHOES_IN_THE_WORLD] || mon->data == &mons[PM_IMMOVABLE_OBSTACLE] || mon->data == &mons[PM_INVINCIBLE_HAEN] || mon->data == &mons[PM_CHAREY] || mon->data == &mons[PM_INVENTOR_OF_THE_SISTER_COMBAT_BOOTS] || mon->data == &mons[PM_SWEET_ASIAN_POISON_IVY] || mon->data == &mons[PM_FIRST_DUNVEGAN] || mon->data == &mons[PM_ARABELLA_SHOE] || mon->data == &mons[PM_ANASTASIA_SHOE] || mon->data == &mons[PM_HENRIETTA_SHOE] || mon->data == &mons[PM_KATRIN_SHOE] || mon->data == &mons[PM_JANA_SHOE] || mon->data == &mons[PM_PERCENTI_HAS_LOST___] || mon->data == &mons[PM_PERCENTI_IS_IMMUNE_TO_THE_ATTACK_]) {

		pline("%s is IMMUNE to the attack!", Monnam(mon));
		if (FunnyHallu) You("curse at Konami for designing it like that.");
		if (!rn2(30)) pline("Reminder: you must use something else in order to damage this monster!");
		else if (!rn2(30)) pline("You read that right: your attacks are doing no damage at all!");
		else if (!rn2(30)) pline("Hint: try offensive wands or spells.");

		(void) passive(mon, FALSE, 1, TRUE, FALSE);
		return;
	}

	i = -inv_weight();
	j = weight_cap();

	if (uarmf && itemhasappearance(uarmf, APP_COMBAT_BOOTS) ) i += 6000;

	if (uarmf && uarmf->oartifact == ART_KYLIE_LUM_S_SNAKESKIN_BOOT) i += 6000;

	if((i < (j*3)/10) && !(uarmf && uarmf->oartifact == ART_MAILIE_S_CHALLENGE) && !(uwep && uwep->oartifact == ART_INSECTMASHER) && !(uwep && uwep->oartifact == ART_BLU_TOE) && !(uarmf && uarmf->oartifact == ART_FRONT_TARGET) && !(uarmf && uarmf->oartifact == ART_ELENETTES) && !(uarmc && uarmc->oartifact == ART_MARC_S_MANAFILL && !flags.female) && u.martialstyle != MARTIALSTYLE_KARATE && !(uarmf && uarmf->oartifact == ART_JOHN_S_REDBLOCK) && !(uarmf && uarmf->oartifact == ART_EVIL_HAIRTEAR) && !(uarmf && itemhasappearance(uarmf, APP_CALF_LEATHER_SANDALS)) ) {
		if((!rn2((i < j/10) ? 2 : (i < j/5) ? 3 : 4)) || (isfriday && !rn2(5))) {
			if(martial() && !rn2(isfriday ? 10 : 2)) goto doit;
			Your("clumsy kick does no damage.");
			(void) passive(mon, FALSE, 1, AT_KICK, FALSE);
			return;
		}
		if(i < j/10) clumsy = TRUE;
		else if(!rn2((i < j/5) ? 2 : 3)) clumsy = TRUE;
	}

	if (Fumbling) clumsy = TRUE;

	else if (uarm && objects[uarm->otyp].oc_bulky && ACURR(A_DEX) < rnd(25))
		clumsy = TRUE;

	if (uarmf && itemhasappearance(uarmf, APP_COMBAT_BOOTS) ) clumsy = FALSE;

	if (uarmf && uarmf->oartifact == ART_MAILIE_S_CHALLENGE) clumsy = FALSE;
	if (uwep && uwep->oartifact == ART_INSECTMASHER) clumsy = FALSE;
	if (uarmf && uarmf->oartifact == ART_FRONT_TARGET) clumsy = FALSE;
	if (uarmf && uarmf->oartifact == ART_ELENETTES) clumsy = FALSE;
	if (uarmc && uarmc->oartifact == ART_MARC_S_MANAFILL && !flags.female) clumsy = FALSE;
	if (uarmf && uarmf->oartifact == ART_EVIL_HAIRTEAR) clumsy = FALSE;
	if (uarmf && uarmf->oartifact == ART_JOHN_S_REDBLOCK) clumsy = FALSE;
	if (uwep && uwep->oartifact == ART_BLU_TOE) clumsy = FALSE;

	if (uarmf && itemhasappearance(uarmf, APP_CALF_LEATHER_SANDALS)) clumsy = FALSE;
	if (u.martialstyle == MARTIALSTYLE_KUNGFU && !rn2(3)) clumsy = FALSE;
	if (u.martialstyle == MARTIALSTYLE_KARATE) clumsy = FALSE;

	if (uarmf && uarmf->oartifact == ART_KYLIE_LUM_S_SNAKESKIN_BOOT) clumsy = FALSE;
	if (uarmf && uarmf->oartifact == ART_JUMP_KICK_ACTION) clumsy = FALSE;

	if (isfriday && !rn2(10)) clumsy = TRUE;
	if (uwep && uwep->oartifact == ART_BARANA_S_MISS && !rn2(10)) clumsy = TRUE;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_BARANA_S_MISS && !rn2(10)) clumsy = TRUE;

doit:
	if (uarmf && uarmf->oartifact == ART_MOST_EROTIC_AIR_CURRENT_NO && !rn2(5)) {
		You("try to kick %s, but your pointy shoe tip just barely misses %s %s.", mon_nam(mon), mhis(mon), mbodypart(mon, LEG));
		if (FunnyHallu) pline("This results in very erotic air current noises.");
		mon->mconf = TRUE;
		return;
	}

	You("kick %s.", mon_nam(mon));
	wakeup(mon);
	if(!rn2(clumsy ? 3 : 4) && (u.martialstyle != MARTIALSTYLE_KUNGFU || rn2(4)) && (clumsy || !bigmonst(mon->data)) &&
	   mon->mcansee && !mon->mtrapped && !(uarmf && uarmf->oartifact == ART_CHLOE_S_EXAGGERATION) && !thick_skinned(mon->data) &&
	   mon->data->mlet != S_EEL && u.martialstyle != MARTIALSTYLE_KARATE && haseyes(mon->data) && mon->mcanmove &&
	   !mon->mstun && !mon->mconf && !mon->msleeping &&
	   mon->data->mmove >= 12) {
		if(!nohands(mon->data) && !rn2(martial() ? 5 : 3)) {
		    pline("%s blocks your %skick.", Monnam(mon),
				clumsy ? "clumsy " : "");
		    (void) passive(mon, FALSE, 1, AT_KICK, FALSE);
		    return;
		} else {
		    mnexto(mon);
		    if(mon->mx != x || mon->my != y) {
			if(memory_is_invisible(x, y)) {
			    unmap_object(x, y);
			    newsym(x, y);
			}
			pline("%s %s, %s evading your %skick.", Monnam(mon),
				(can_teleport(mon->data) ? "teleports" :
				 is_floater(mon->data) ? "floats" :
				 is_flyer(mon->data) ? "swoops" :
				 (nolimbs(mon->data) || slithy(mon->data)) ?
					"slides" : "jumps"),
				clumsy ? "easily" : "nimbly",
				clumsy ? "clumsy " : "");
			(void) passive(mon, FALSE, 1, AT_KICK, FALSE);
			return;
		    }
		}
	}

	if (uarmf && itemhasappearance(uarmf, APP_A_HEELS) && !rn2(10)) {
		pline("%s simply evades your clunky stiletto heels!", Monnam(mon));
		return;
	}

	kickdmg(mon, clumsy);

	if (uarmf && uarmf->oartifact == ART_JANA_S_MUD_OBSESSION && !rn2(20)) {
		buzz(27, 6, u.ux,u.uy,u.dx,u.dy); /* 27 = acid blast */
	}

	if (uarmf && uarmf->otyp == SLEEP_INDUCTION_BOOTS && !rn2(10))
		buzz(23, 4, u.ux,u.uy,u.dx,u.dy); /* 23 = sleep ray */

	return;
}

/*
 *  Return TRUE if caught (the gold taken care of), FALSE otherwise.
 *  The gold object is *not* attached to the fobj chain!
 */
boolean
ghitm(mtmp, gold)
register struct monst *mtmp;
register struct obj *gold;
{
	boolean msg_given = FALSE;
	boolean willdelete = FALSE;

	if(!likes_gold(mtmp->data) && !mtmp->isshk && !mtmp->ispriest
			&& !is_mercenary(mtmp->data)) {
		wakeup(mtmp);
	} else if (!mtmp->mcanmove) {
		/* too light to do real damage */
		if (canseemon(mtmp)) {
		    pline_The("%s harmlessly %s %s.", xname(gold),
			      otense(gold, "hit"), mon_nam(mtmp));
		    msg_given = TRUE;
		}
	} else {
#ifdef GOLDOBJ
                long value = gold->quan * objects[gold->otyp].oc_cost;
#endif
		mtmp->msleeping = 0;
		mtmp->meating = 0;
		if(!rn2(4)) setmangry(mtmp); /* not always pleasing */

		/* greedy monsters catch gold */
		if (cansee(mtmp->mx, mtmp->my))
		    pline("%s catches the gold.", Monnam(mtmp));
#ifndef GOLDOBJ
		if (mtmp->isshk && rn2(2)) mtmp->mgold += gold->quan;
		else willdelete = TRUE;
#endif
		if (mtmp->isshk) {
			long robbed = ESHK(mtmp)->robbed;

			if (robbed) {
#ifndef GOLDOBJ
				robbed -= gold->quan;
#else
				robbed -= value;
#endif
				if (robbed < 0) robbed = 0;
				pline_The("amount %scovers %s recent losses.",
				      !robbed ? "" : "partially ",
				      mhis(mtmp));
				ESHK(mtmp)->robbed = robbed;
				if(!robbed)
					make_happy_shk(mtmp, FALSE);
			} else {
				if(mtmp->mpeaceful && rn2(2) ) { /* randomly cheat the player --Amy */
#ifndef GOLDOBJ
				    ESHK(mtmp)->credit += gold->quan;
#else
				    ESHK(mtmp)->credit += value;
#endif
				    You("have %ld %s in credit.",
					ESHK(mtmp)->credit,
					currency(ESHK(mtmp)->credit));
				} else if (mtmp->mpeaceful) {
					verbalize("I'll take that. Hahaha!");
				} else verbalize("Thanks, scum!");
			}
		} else if (mtmp->ispriest) {
			if (mtmp->mpeaceful)
			    verbalize("Thank you for your contribution.");
			else verbalize("Thanks, scum!");
		} else if (is_mercenary(mtmp->data)) {
		    long goldreqd = 0L;

		    if (rn2(3)) {
			if (mtmp->data == &mons[PM_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_TEUTON_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_FRANKISH_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_VIDEO_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_BRITISH_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_AMERICAN_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_COPPER_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_ARAB_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_ASIAN_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_MEDIEVAL_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_PAD_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_GAUCHE_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_SEAFARING_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_BYZANTINE_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_CELTIC_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_VANILLA_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_VIKING_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_VANGUARD_TROOPER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_INFANTRY_RECRUIT])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_LEGIONARY_GRUNT])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_MILITANT_TROOPER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_RIFLEMAN_PRIVATE])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_FOOTMAN_CONSCRIPT])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_GUARDSMAN_TRAINEE])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_FUSILIER_ROOKIE])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_SPEARMAN_RECRUIT])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_ARCHER_CADET])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_KUNG_FU_JESTER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_SWAMP_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_JAVA_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_IBERIAN_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_ROHIRRIM_SOLDIER])
			   goldreqd = 100L;
			else if (mtmp->data == &mons[PM_EXTRATERRESTRIAL_SERGEANT])
			   goldreqd = 250L;
			else if (mtmp->data == &mons[PM_MINOAN_SERGEANT])
			   goldreqd = 250L;
			else if (mtmp->data == &mons[PM_SHADOW_COMMANDO])
			   goldreqd = 250L;
			else if (mtmp->data == &mons[PM_ASS_RIMMING_RAMMER])
			   goldreqd = 250L;
			else if (mtmp->data == &mons[PM_HUN_SERGEANT])
			   goldreqd = 250L;
			else if (mtmp->data == &mons[PM_MONGOL_SERGEANT])
			   goldreqd = 250L;
			else if (mtmp->data == &mons[PM_ORANGE_SERGEANT])
			   goldreqd = 250L;
			else if (mtmp->data == &mons[PM_TWOWEAP_SERGEANT])
			   goldreqd = 250L;
			else if (mtmp->data == &mons[PM_PERSIAN_SERGEANT])
			   goldreqd = 250L;
			else if (mtmp->data == &mons[PM_SERGEANT])
			   goldreqd = 250L;
			else if (mtmp->data == &mons[PM_LIEUTENANT])
			   goldreqd = 500L;
			else if (mtmp->data == &mons[PM_YAMATO_LIEUTENANT])
			   goldreqd = 500L;
			else if (mtmp->data == &mons[PM_CARTHAGE_LIEUTENANT])
			   goldreqd = 500L;
			else if (mtmp->data == &mons[PM_ROMAN_LIEUTENANT])
			   goldreqd = 500L;
			else if (mtmp->data == &mons[PM_ELITE_SENTINEL])
			   goldreqd = 500L;
			else if (mtmp->data == &mons[PM_CAPTAIN])
			   goldreqd = 750L;
			else if (mtmp->data == &mons[PM_IRONCLAD_WARRIOR])
			   goldreqd = 750L;
			else if (mtmp->data == &mons[PM_GOTHIC_CAPTAIN])
			   goldreqd = 750L;
			else if (mtmp->data == &mons[PM_URBAN_CAMO_CAPTAIN])
			   goldreqd = 750L;
			else if (mtmp->data == &mons[PM_GENERAL])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_PRISON_GUARD])
			   goldreqd = 200L;
			else if (mtmp->data == &mons[PM_STORMBREAKER_INFANTRY])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_VENOMOUS_OPERATIVE])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_CHAOS_REAVER])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_NOVA_SHOCK_TROOPER])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_LUNAR_WARDEN])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_DREADNOUGHT_MARINE])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_ARCTIC_BLADESTORM])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_RADIANT_GUARDIAN])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_EMBERSTRIKE_GUNNER])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_THUNDERCLAP_TROOPER])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_CELESTIAL_PARAGON])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_SOULFIRE_SENTINEL])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_SPECTRAL_ENFORCER])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_ABYSSAL_LEGIONNAIRE])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_CYBERNETIC_VANGUARD])
			   goldreqd = 1000L;
			else if (mtmp->data == &mons[PM_ASTRAL_BLADEMASTER])
			   goldreqd = 1000L;

			if (goldreqd && !mtmp->mfrenzied) {
#ifndef GOLDOBJ
			   if (gold->quan > goldreqd +
				(u.ugold + u.ulevel*rn2(5))/ACURR(A_CHA))
#else
			   if (value > goldreqd +
				(money_cnt(invent) + u.ulevel*rn2(5))/ACURR(A_CHA))
#endif
			    mtmp->mpeaceful = TRUE;

/* Amy: politicians have a chance of taming soldiers with a bribe; the tamedog code handles the actual success rate */

			    if (uarmc && uarmc->oartifact == ART_FIELD_MARS_RESOUNDS) {
				(void) tamedog(mtmp, (struct obj *) 0, TRUE); /* guaranteed because powerful artifact --Amy */
			    } else if (Role_if(PM_POLITICIAN)) {
				(void) tamedog(mtmp, (struct obj *) 0, FALSE);
			    }
			}
		     }
		     if (mtmp->mpeaceful)
			    verbalize("That should do.  Now beat it!");
		     else verbalize("That's not enough, coward!");
		}


#ifndef GOLDOBJ
		dealloc_obj(gold);
#else
		add_to_minv(mtmp, gold);
#endif
		return TRUE;
	}

	if (!msg_given) miss(xname(gold), mtmp);
	return FALSE;
}

/* container is kicked, dropped, thrown or otherwise impacted by player.
 * Assumes container is on floor.  Checks contents for possible damage. */
void
container_impact_dmg(obj)
struct obj *obj;
{
	struct monst *shkp;
	struct obj *otmp, *otmp2;
	long loss = 0L;
	boolean costly, insider;
	xchar x = obj->ox, y = obj->oy;

	/* only consider normal containers */
	if (!Is_container(obj) || Is_mbag(obj)) return;

	costly = ((shkp = shop_keeper(*in_rooms(x, y, SHOPBASE))) &&
		  costly_spot(x, y));
	insider = (*u.ushops && inside_shop(u.ux, u.uy) &&
		   *in_rooms(x, y, SHOPBASE) == *u.ushops);

	for (otmp = obj->cobj; otmp; otmp = otmp2) {
	    const char *result = (char *)0;

	    otmp2 = otmp->nobj;

	    /* Amy edit: way too harsh to constantly break all those potions. it's also dumb that this function is called
	     * both by kicking the container (you have to be stupid enough to do that on purpose) and also by dropping it
	     * down a staircase or while levitating (can happen by accident)... I'd like to make it so that kicking it has
	     * way more of a chance that the items break, but oh well */

	    if ((objects[otmp->otyp].oc_material == MT_GLASS || objects[otmp->otyp].oc_material == MT_OBSIDIAN || is_vitric(otmp)) &&
		otmp->oclass != GEM_CLASS && !rn2(otmp->oerodeproof ? 100 : 20) && !obj_resists(otmp, 33, 100) && !stack_too_big(otmp)) {
		result = "shatter";
	    } else if (otmp->otyp == EGG && !rn2(otmp->oerodeproof ? 100 : 20) && !stack_too_big(otmp)) {
		result = "cracking";
	    } else if (evilfriday && !rn2(otmp->oerodeproof ? 10 : 2) && !obj_resists(otmp, 50, 100) && !stack_too_big(otmp)) {
		result = "breaking sound, followed by evil laughter";
	    }
	    if (result) {
		if (otmp->otyp == MIRROR) change_luck(-2);

		/* eggs laid by you.  penalty is -1 per egg, max 5,
		 * but it's always exactly 1 that breaks */
		if (otmp->otyp == EGG && otmp->spe && otmp->corpsenm >= LOW_PM)
		    change_luck(-1);
		You_hear("a muffled %s.", result);
		if (costly)
		    loss += stolen_value(otmp, x, y,
					 (boolean)shkp->mpeaceful, TRUE, TRUE);
		if (otmp->quan > 1L)
		    useup(otmp);
		else {
		    obj_extract_self(otmp);
		    obfree(otmp, (struct obj *) 0);
		}
	    }
	}
	if (costly && loss) {
	    if (!insider) {
		You("caused %ld %s worth of damage!", loss, currency(loss));
		make_angry_shk(shkp, x, y);
	    } else {
		You("owe %s %ld %s for objects destroyed.",
		    mon_nam(shkp), loss, currency(loss));
	    }
	}
}

STATIC_OVL int
kick_object(x, y)
xchar x, y;
{
	int range;
	register struct monst *mon, *shkp;
	struct trap *trap;
	char bhitroom;
	boolean costly, isgold, slide = FALSE;

	/* if a pile, the "top" object gets kicked */
	kickobj = level.objects[x][y];

	/* kickobj should always be set due to conditions of call */
	if(!kickobj || kickobj->otyp == BOULDER
			|| kickobj == uball || kickobj == uchain)
		return(0);

	if ((trap = t_at(x,y)) != 0 &&
			(((trap->ttyp == PIT || trap->ttyp == SHIT_PIT || trap->ttyp == MANA_PIT || trap->ttyp == ANOXIC_PIT || trap->ttyp == HYPOXIC_PIT || trap->ttyp == ACID_PIT ||
			   trap->ttyp == SPIKED_PIT || trap->ttyp == GIANT_CHASM) && !Passes_walls) ||
			 trap->ttyp == WEB)) {
		if (!trap->tseen && !trap->hiddentrap) find_trap(trap);
		You_cant("kick %s that's in a %s!", something,
			 FunnyHallu ? "tizzy" :
			 (trap->ttyp == WEB) ? "web" : "pit");
		return 1;
	}

	if(Fumbling && !rn2(3)) {
		Your("clumsy kick missed.");
		return(1);
	}

	if(kickobj->otyp == PETRIFYIUM_BAR && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && (!uarmf || (uarmf && itemhasappearance(uarmf, APP_STRAP_ON_STILETTOS))) ) {
		pline("Kicking a petrifying weapon is a bad idea.");
	    if (!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
		
		if (!Stoned) {

			if (Hallucination && rn2(10)) pline("Good thing you are already stoned.");
			else {
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				flags.botl = 1;
				u.cnd_stoningcount++;
				delayed_killer = "kicking a petrifyium bar";
			}
		}

	    }
	}

	if(kickobj->otyp == PETRIFYIUM_BRA && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && (!uarmf || (uarmf && itemhasappearance(uarmf, APP_STRAP_ON_STILETTOS))) ) {
		pline("Kicking a petrifying bra is a bad idea.");
	    if (!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
		
		if (!Stoned) {

			if (Hallucination && rn2(10)) pline("Good thing you are already stoned.");
			else {
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				flags.botl = 1;
				u.cnd_stoningcount++;
				delayed_killer = "kicking a petrifyium bra";
			}
		}

	    }
	}

	if(kickobj->otyp == CORPSE && touch_petrifies(&mons[kickobj->corpsenm])
			&& (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && (!uarmf || (uarmf && itemhasappearance(uarmf, APP_STRAP_ON_STILETTOS))) ) {
	    static char kbuf[BUFSZ];

	    You("kick the %s with your bare %s.",
		corpse_xname(kickobj, TRUE), makeplural(body_part(FOOT)));
	    if (!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
		
		/*You("turn to stone...");
		killer_format = KILLED_BY;
		sprintf(kbuf, "kicking a monster corpse (%s) without boots",
			an(killer_cxname(kickobj, TRUE)));
		killer = kbuf;
		done(STONING);	*/
		if (!Stoned) {

			if (Hallucination && rn2(10)) pline("Good thing you are already stoned.");
			else {
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				flags.botl = 1;
				u.cnd_stoningcount++;
				delayed_killer = killer_cxname(kickobj,TRUE);
			}
		}

	    }
	}

	/* range < 2 means the object will not move.	*/
	/* maybe dexterity should also figure here.     */
	/* MAR - if there are multiple objects, range is calculated */
	/* from a single object (not entire stack!) */
	
	if (kickobj->quan > 1L && (kickobj->oclass != COIN_CLASS)) /*MAR*/
		range = (int)((ACURRSTR)/2 - kickobj->owt/kickobj->quan/40);
	else range = (int)((ACURRSTR)/2 - kickobj->owt/40);

	if(martial()) range += rnd(3);

	if (is_waterypool(x, y) || is_watertunnel(x,y)) {
	    /* you're in the water too; significantly reduce range */
	    range = range / 3 + 1;	/* {1,2}=>1, {3,4,5}=>2, {6,7,8}=>3 */
	} else {
	    if (is_ice(x, y)) range += rnd(3),  slide = TRUE;
	    if (kickobj->greased) range += rnd(3),  slide = TRUE;
	}

	/* Mjollnir is magically too heavy to kick */
	if(kickobj->oartifact == ART_MJOLLNIR) range = 1;
	if(kickobj->oartifact == ART_OTHER_MJOLLNIR) range = 1;
	if (kickobj->dynamitekaboom) range = 1;
	if (kickobj->mstartinventX) range = 1;

	/* see if the object has a place to move into */
	if(!ZAP_POS(levl[x+u.dx][y+u.dy].typ) || closed_door(x+u.dx, y+u.dy))
		range = 1;

	costly = ((shkp = shop_keeper(*in_rooms(x, y, SHOPBASE))) &&
				    costly_spot(x, y));
	isgold = (kickobj->oclass == COIN_CLASS);

	if (IS_ROCK(levl[x][y].typ) || closed_door(x, y)) {
	    if ((!martial() && rn2(20) > ACURR(A_DEX)) ||
		    IS_ROCK(levl[u.ux][u.uy].typ) || closed_door(u.ux, u.uy)) {
		if (Blind)
		    pline("It doesn't come loose.");
		else
		    pline("%s %sn't come loose.",
			  The(distant_name(kickobj, xname)),
			  otense(kickobj, "do"));
		return (!rn2(3) || martial());
	    }
	    if (Blind)
		pline("It comes loose.");
	    else
		pline("%s %s loose.",
		      The(distant_name(kickobj, xname)),
		      otense(kickobj, "come"));
	    if (kickobj && is_metallic(kickobj)) wake_nearby(); /* metallic objects make noise --Amy */
	    obj_extract_self(kickobj);
	    newsym(x, y);
	    if (costly && (!costly_spot(u.ux, u.uy) ||
		    !index(u.urooms, *in_rooms(x, y, SHOPBASE))))
		addtobill(kickobj, FALSE, FALSE, FALSE);
	    if (!flooreffects(kickobj, u.ux, u.uy, "fall")) {
		place_object(kickobj, u.ux, u.uy);
		stackobj(kickobj);
		newsym(u.ux, u.uy);
	    }
	    return 1;
	}

	/* a box gets a chance of breaking open here */
	if(Is_box(kickobj)) {
		boolean otrp = kickobj->otrapped;
		wake_nearby(); /* box always makes noise, presumably because of the metallic lock */
		if(range < 2) pline("THUD!");

		container_impact_dmg(kickobj);

		if (kickobj->olocked) {
		    if (!rn2(5) || (martial() && !rn2(2))) {
			You("break open the lock!");
			u.cnd_kicklockcount++;
			kickobj->olocked = 0;
			kickobj->obroken = 1;
			if (otrp) (void) chest_trap(kickobj, LEG, FALSE);
			return(1);
		    }
		} else {
		    if (!rn2(3) || (martial() && !rn2(2))) {
			pline_The("lid slams open, then falls shut.");
			if (otrp) (void) chest_trap(kickobj, LEG, FALSE);
			return(1);
		    }
		}
		if(range < 2) return(1);
		/* else let it fall through to the next cases... */
	}

	if (kickobj && is_metallic(kickobj)) wake_nearby(); /* metallic objects make noise --Amy */

	/* fragile objects should not be kicked */
	if (hero_breaks(kickobj, kickobj->ox, kickobj->oy, FALSE)) return 1;

	/* too heavy to move.  range is calculated as potential distance from
	 * player, so range == 2 means the object may move up to one square
	 * from its current position
	 */
	if(range < 2 || (isgold && kickobj->quan > 300L)) {
	    if(!Is_box(kickobj)) {
			pline("Thump!");
			if (kickobj->oartifact == ART_DONALD_THUMP_) badeffect();
	    }
	    return(!rn2(3) || martial());
	}

	if (uarmf && uarmf->oartifact == ART_NEANDERTHAL_SOCCER_CLUB) {
		exercise(A_STR, TRUE);
		exercise(A_DEX, TRUE);
	}

	if (kickobj->quan > 1L && !isgold) kickobj = splitobj(kickobj, 1L);

	if (slide && !Blind)
	    pline("Whee!  %s %s across the %s.", Doname2(kickobj),
		  otense(kickobj, "slide"), surface(x,y));

	obj_extract_self(kickobj);
	(void) snuff_candle(kickobj);
	newsym(x, y);
	mon = bhit(u.dx, u.dy, range, KICKED_WEAPON,
		   (int (*)(MONST_P,OBJ_P))0,
		   (int (*)(OBJ_P,OBJ_P))0,
		   &kickobj, TRUE);
	if (!kickobj)
	    return 1;		/* object broken (and charged for if costly) */ 
	if(mon) {
	    if (mon->isshk &&
		    kickobj->where == OBJ_MINVENT && kickobj->ocarry == mon)
		return 1;	/* alert shk caught it */
	    notonhead = (mon->mx != bhitpos.x || mon->my != bhitpos.y);
	    if(isgold ? ghitm(mon, kickobj) :   /* caught? */
		thitmonst(mon, kickobj, 3, FALSE))        /* hit && used up? */
		return(1);
	}

	/* the object might have fallen down a hole */
	if (kickobj->where == OBJ_MIGRATING) {
	    if (costly) {
		if(isgold)
		    costly_gold(x, y, kickobj->quan);
		else (void)stolen_value(kickobj, x, y,
					(boolean)shkp->mpeaceful, FALSE, FALSE);
	    }
	    return 1;
	}

	bhitroom = *in_rooms(bhitpos.x, bhitpos.y, SHOPBASE);
	if (costly && (!costly_spot(bhitpos.x, bhitpos.y) ||
			*in_rooms(x, y, SHOPBASE) != bhitroom)) {
	    if(isgold)
		costly_gold(x, y, kickobj->quan);
	    else (void)stolen_value(kickobj, x, y,
				    (boolean)shkp->mpeaceful, FALSE, FALSE);
	}

	if(flooreffects(kickobj,bhitpos.x,bhitpos.y,"fall")) return(1);
	place_object(kickobj, bhitpos.x, bhitpos.y);
	stackobj(kickobj);
	newsym(kickobj->ox, kickobj->oy);
	return(1);
}

STATIC_OVL char *
kickstr(buf)
char *buf;
{
	const char *what;

	if (kickobj) what = distant_name(kickobj,doname);
	else if (IS_DOOR(maploc->typ)) what = "a door";
	else if (IS_TREE(maploc->typ)) what = "a tree";
	else if (IS_GRAVEWALL(maploc->typ)) what = "a grave wall";
	else if (IS_TUNNELWALL(maploc->typ)) what = "a tunnel";
	else if (IS_FARMLAND(maploc->typ)) what = "a farmland";
	else if (IS_MOUNTAIN(maploc->typ)) what = "a mountain";
	else if (IS_WATERTUNNEL(maploc->typ)) what = "a water tunnel";
	else if (IS_STWALL(maploc->typ)) what = "a wall";
	else if (IS_ROCK(maploc->typ)) what = "a rock";
	else if (IS_THRONE(maploc->typ)) what = "a throne";
	else if (IS_FOUNTAIN(maploc->typ)) what = "a fountain";
	else if (IS_GRAVE(maploc->typ)) what = "a headstone";
	else if (IS_SINK(maploc->typ)) what = "a sink";
        else if (IS_TOILET(maploc->typ)) what = "a toilet";        
	else if (IS_ALTAR(maploc->typ)) what = "an altar";
	else if (IS_DRAWBRIDGE(maploc->typ)) what = "a drawbridge";
	else if (maploc->typ == STAIRS) what = "the stairs";
	else if (maploc->typ == LADDER) what = "a ladder";
	else if (maploc->typ == IRONBARS) what = "an iron bar";
	else what = "something weird";
	return strcat(strcpy(buf, "kicking "), what);
}

int
dokick()
{
	int x, y;
	int avrg_attrib;
	register struct monst *mtmp;
	boolean no_kick = FALSE;
	char buf[BUFSZ];
	register struct obj *quivtmp;
	struct trap *trap;

	if (trap = t_at(u.ux, u.uy)) {
		if (trap->ttyp == VIVISECTION_TRAP) {
			You("are in vivisection, and therefore unable to kick!");
			return 0;
		}
	}

	if (!Race_if(PM_TRANSFORMER) && (nolimbs(youmonst.data) || slithy(youmonst.data))) {
		You("have no legs to kick with.");
		if (yn("Try a full-body tackle instead? Warning: this can fail and damage your lower body.") == 'y') {
			if (rn2(3) && !polyskillchance()) {
				set_wounded_legs(LEFT_SIDE, HWounded_legs + rnd(60-ACURR(A_DEX)));
				pline("Argh! That didn't work!");
				if (!rn2(20)) badeffect();
				return 1;
			}
		}
		else {no_kick = TRUE;}
	} else if (verysmall(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
		You("are too small to do any kicking.");
		if (yn("Try it anyway? Warning: this can fail and damage your lower body.") == 'y') {
			if (rn2(3) && !polyskillchance()) {
				set_wounded_legs(LEFT_SIDE, HWounded_legs + rnd(60-ACURR(A_DEX)));
				pline("You hurt your muscles!");
				if (!rn2(20)) badeffect();
			 	return 1;
			}
		}
		else {no_kick = TRUE;}
	}
	/* end checks that let you try anyway */

	if (u.usteed) {
		if (yn_function("Kick your steed?", ynchars, 'n') == 'y') {
		    You("kick %s.", mon_nam(u.usteed));
		    kick_steed();
		    return 1;
		}/* else {
		    return 0;
		}*/
	/* your legs are not fucking welded to the horse, so you should be able to kick monsters too!!! --Amy */
	} else if (Wounded_legs) {
		/* note: jump() has similar code */
		Your("%s is in no shape for kicking.", body_part(LEG));
		no_kick = TRUE;
	} else if (near_capacity() > MOD_ENCUMBER) { /* used to be that stressed prevented kicking; changed to strained by Amy */
		Your("load is too heavy to balance yourself for a kick.");
		no_kick = TRUE;
	} else if (youmonst.data->mlet == S_LIZARD) {
		Your("legs cannot kick effectively.");
		no_kick = TRUE;
	} else if (u.uinwater && !rn2(2)) {
		Your("slow motion kick doesn't hit anything.");
		no_kick = TRUE;
	} else if (u.utrap) {
		switch (u.utraptype) {
		    case TT_PIT:
			pline("There's not enough room to kick down here.");
			break;
		    case TT_WEB:
		    case TT_GLUE:
		    case TT_BEARTRAP:
			You_cant("move your %s!", body_part(LEG));
			break;
		    default:
			break;
		}
		no_kick = TRUE;
	}

	if (no_kick) {
		/* ignore direction typed before player notices kick failed */
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);	/* --More-- */
		return 0;
	}

	if(!getdir((char *)0)) return(0);
	if(!u.dx && !u.dy) return(0);

	x = u.ux + u.dx;
	y = u.uy + u.dy;

	if(!isok(x, y)) { /* gotta fix that unneccessary segfault for once and for all! --Amy */

	pline(FunnyHallu ? "You get a great rebound effect!" : "Your kick hits an invisible barrier.");
	return(1);
	}

	/* KMH -- Kicking boots always succeed */
	if (uarmf && ((uarmf->otyp == KICKING_BOOTS) || (uarmf->otyp == COMBAT_WEDGES) || (uarmf->otyp == STOMPING_BOOTS)) )
	    avrg_attrib = 99;
	else
	    avrg_attrib = (ACURRSTR+ACURR(A_DEX)+ACURR(A_CON))/3;

	if(u.uswallow) {
		switch(rn2(3)) {
		case 0:  You_cant("move your %s!", body_part(LEG));
			 break;
		case 1:  if (is_animal(u.ustuck->data)) {
				pline("%s burps loudly.", Monnam(u.ustuck));
				break;
			 }
		default: Your("feeble kick has no effect."); break;
		}
		return(1);
	}
	if (Levitation) {
		int xx, yy;

		xx = u.ux - u.dx;
		yy = u.uy - u.dy;
		/* doors can be opened while levitating, so they must be
		 * reachable for bracing purposes
		 * Possible extension: allow bracing against stuff on the side?
		 */
		if (isok(xx,yy) && !IS_ROCK(levl[xx][yy].typ) &&
			!IS_DOOR(levl[xx][yy].typ) &&
			(!Is_airlevel(&u.uz) || !OBJ_AT(xx,yy))) {
		    You("have nothing to brace yourself against.");
		    return(0);
		}
	}

	if (uarmf && itemhasappearance(uarmf, APP_PISTOL_BOOTS) && uquiver && (objects[uquiver->otyp].w_ammotyp == WP_BULLET_PISTOL) ) {
		if (uquiver->quan > 1L) {
			quivtmp = splitobj(uquiver, 1L);
		} else {
			quivtmp = uquiver;
			if (quivtmp->owornmask)
			    remove_worn_item(quivtmp, FALSE);
		}
		freeinv(quivtmp);
		if (quivtmp && uarmf && uarmf->oartifact == ART_ANACONDA_HEELS) quivtmp->spe += 12;
		throwit(quivtmp, 0L, FALSE, 666);
	}

	/* make noise only if the boots are metallic --Amy */
	if (uarmf && is_metallic(uarmf)) wake_nearby();
	u_wipe_engr(2);

	maploc = &levl[x][y];

	/* The next five tests should stay in    */
	/* their present order: monsters, pools, */
	/* objects, non-doors, doors.		 */

	if(MON_AT(x, y)) {
		struct permonst *mdat;

		mtmp = m_at(x, y);
		mdat = mtmp->data;
		if (!mtmp->mpeaceful || !canspotmon(mtmp))
		    flags.forcefight = TRUE; /* attack even if invisible */
		kick_monster(x, y);
		flags.forcefight = FALSE;
		/* see comment in attack_checks() */
		if (!DEADMONSTER(mtmp) &&
		    !canspotmon(mtmp) &&
		    /* check x and y; a monster that evades your kick by
		       jumping to an unseen square doesn't leave an I behind */
		    mtmp->mx == x && mtmp->my == y &&
		    !memory_is_invisible(x, y) && !(monstersoundtype(mtmp) == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember) &&
		    !(u.uswallow && mtmp == u.ustuck))
			map_invisible(x, y);
		if((Is_airlevel(&u.uz) || Levitation) && flags.move) {
		    int range;

		    range = ((int)youmonst.data->cwt + (weight_cap() + inv_weight()));
		    if (range < 1) range = 1; /* divide by zero avoidance */
		    range = (3*(int)mdat->cwt) / range;

		    if(range < 1) range = 1;
		    hurtle(-u.dx, -u.dy, range, TRUE);
		}
		return(1);
	}
	if (memory_is_invisible(x, y)) {
		unmap_object(x, y);
		newsym(x, y);
	}
	if (is_waterypool(x, y) ^ !!u.uinwater) {
		/* objects normally can't be removed from water by kicking */
		You("splash some water around.");
		return 1;
	}

	kickobj = (struct obj *)0;
	if (OBJ_AT(x, y) &&
	    (!Levitation || Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)
	     || sobj_at(BOULDER,x,y))) {
		if(kick_object(x, y)) {
		    if(Is_airlevel(&u.uz))
			hurtle(-u.dx, -u.dy, 1, TRUE); /* assume it's light */
		    return(1);
		}
		goto ouch;
	}

	if(!IS_DOOR(maploc->typ)) {
		if(maploc->typ == SDOOR) {
		    if(!Levitation && ((rn2(25) < avrg_attrib ) || !rn2(5) ) ) {
			cvt_sdoor_to_door(maploc);	/* ->typ = DOOR */
			pline("Crash!  %s a secret door!",
			      /* don't "kick open" when it's locked
				 unless it also happens to be trapped */
			(maploc->doormask & (D_LOCKED|D_TRAPPED)) == D_LOCKED ?
			      "Your kick uncovers" : "You kick open");
			wake_nearby();
			exercise(A_DEX, TRUE);
			if(maploc->doormask & D_TRAPPED) {
			    maploc->doormask = D_NODOOR;
			    b_trapped("door", FOOT);
			} else if (maploc->doormask != D_NODOOR &&
				   !(maploc->doormask & D_LOCKED))
			    maploc->doormask = D_ISOPEN;
			if (Blind)
			    feel_location(x,y);	/* we know it's gone */
			else
			    newsym(x,y);
			if (maploc->doormask == D_ISOPEN ||
			    maploc->doormask == D_NODOOR)
			    unblock_point(x,y);	/* vision */
			return(1);
		    } else goto ouch;
		}
		if(maploc->typ == SCORR) {
		    if(!Levitation && ((rn2(25) < avrg_attrib ) || !rn2(5) ) ) {
			pline("Crash!  You kick open a secret passage!");
			wake_nearby();
			exercise(A_DEX, TRUE);
			maploc->typ = CORR;
			if (Blind)
			    feel_location(x,y);	/* we know it's gone */
			else
			    newsym(x,y);
			unblock_point(x,y);	/* vision */
			return(1);
		    } else goto ouch;
		}
		if(IS_THRONE(maploc->typ)) {
		    register int i;
		    if(Levitation) goto dumb;
		    if((Luck < 0 || maploc->doormask) && !rn2(3)) {
			maploc->typ = ROOM;
			maploc->doormask = 0; /* don't leave loose ends.. */
			(void) mkgold((long)rnd(200), x, y);
			if (Blind)
			    pline("CRASH!  You destroy it.");
			else {
			    pline("CRASH!  You destroy the throne.");
			    newsym(x, y);
			}
			wake_nearby();
			exercise(A_DEX, TRUE);
			return(1);
		    } else if(Luck > 0 && !rn2(3) && !maploc->looted) {
			(void) mkgold((long) rn1(201, 300), x, y);
			i = Luck + 1;
			if(i > 6) i = 6;
			while(i--)
			    (void) mksobj_at(rnd_class(DILITHIUM_CRYSTAL,
					LUCKSTONE-1), x, y, TRUE, TRUE, FALSE);

			if (Blind)
			    You("kick %s loose!", something);
			else {
			    You("kick loose some ornamental coins and gems!");
			    newsym(x, y);
			}
			/* prevent endless milking */
			maploc->looted = T_LOOTED;
			return(1);
		    } else if (!rn2(4)) {
			if(dunlev(&u.uz) < dunlevs_in_dungeon(&u.uz)) {
			    fall_through(FALSE);
			    return(1);
			} else goto ouch;
		    }
		    goto ouch;
		}
		if(IS_ALTAR(maploc->typ)) {
		    if(Levitation) goto dumb;
		    You("kick %s.",(Blind ? something : "the altar"));
		    if(!rn2(3)) goto ouch;
		    altar_wrath(x, y);
		    exercise(A_DEX, TRUE);
		    return(1);
		}
		if(IS_FOUNTAIN(maploc->typ)) {
		    if(Levitation) goto dumb;
		    You("kick %s.",(Blind ? something : "the fountain"));
		    if(!rn2(3)) goto ouch;
		    /* make metal boots rust */
		    if(uarmf && rn2(3))
			if (!rust_dmg(uarmf, "metal boots", 1, FALSE, &youmonst)) {
				Your("boots get wet.");
				/* could cause short-lived fumbling here */
			}
		    exercise(A_DEX, TRUE);
		    return(1);
		}
		if(IS_GRAVE(maploc->typ) || maploc->typ == IRONBARS)
		    goto ouch;
		if(IS_TREE(maploc->typ)) {
		    struct obj *treefruit;
		    /* nothing, fruit or trouble? 75:23.5:1.5% */

		    if (uarmf && uarmf->oartifact == ART_CLIMATE_PROTECTION_IS_ERRI) {
			pline("You kick the tree, causing it to crumble.");
			levl[x][y].typ = CORR; /* it is intentional that this does not call the tree squad --Amy */
			newsym(x,y);
			return(1);
		    }

		    if (rn2(3)) {
			if ( !rn2(6))
			    You_hear("a low buzzing."); /* a warning */
		    if (uarmf && itemhasappearance(uarmf, APP_JUNGLE_BOOTS) ) {
			pline("You kick the tree. Nothing happens.");
			return(1);
		    } else if (RngeJungleAction) {
			pline("You kick the tree. Nothing happens.");
			return(1);
		    } else
			goto ouch;
		    }
		    if (rn2(15) && (maploc->looted & TREE_LOOTED) ) {
				pline("The tree's branches are swinging, but there's no fruit on it.");
				return(1);
		    } else if (rn2(15) && !(maploc->looted & TREE_LOOTED) ) {

			if ((!issoviet && rn2(3)) || (isfriday && !rn2(2)) ) {
				/* nerf by Amy, it was way too easy to accumulate tons of fruits. */
				pline("The tree's branches are swinging, but apparently it doesn't bear any fruits.");
				maploc->looted |= TREE_LOOTED;
				return(1);
			} else {
				treefruit = rnd_treefruit_at(x, y);
				if (!treefruit) {
					pline("The tree's branches are swinging.");
					return(1);
				}
				long nfruit = 8L-rnl(7), nfall;
				if (rn2(3)) nfruit /= 2; /* With the high amount of trees in the game, this is necessary --Amy */
				if (!rn2(4)) nfruit /= 3;
				if (nfruit < 1) nfruit = 1;
				short frtype = treefruit->otyp;
				treefruit->quan = nfruit;
				treefruit->owt = weight(treefruit);
				if (is_plural(treefruit))
				    pline("Some %s fall from the tree!", xname(treefruit));
				else
				    pline("%s falls from the tree!", An(xname(treefruit)));
				nfall = scatter(x,y,2,MAY_HIT,treefruit);
				if (nfall != nfruit) {
				    /* scatter left some in the tree, but treefruit
				     * may not refer to the correct object */
				    treefruit = mksobj(frtype, TRUE, FALSE, FALSE);
				    if (treefruit) {
					    treefruit->quan = nfruit-nfall;
					    treefruit->owt = weight(treefruit);
					    pline("%ld %s got caught in the branches.",
						nfruit-nfall, xname(treefruit));
					    dealloc_obj(treefruit);
				    }
				}
				exercise(A_DEX, TRUE);
				exercise(A_WIS, TRUE);	/* discovered a new food source! */
				newsym(x, y);
				maploc->looted |= TREE_LOOTED;
				return(1);
			}
		    } else if (!(maploc->looted & TREE_SWARM)) {
		    	int cnt = rnl(4) + 2;
			int made = 0;
		    	coord mm;
		    	mm.x = x; mm.y = y;

			if (!rn2(20)) {

				while (cnt--) {
					if (enexto(&mm, mm.x, mm.y, &mons[PM_WOOD_NYMPH]) &&
					makemon(mkclass(S_NYMPH,0), mm.x, mm.y, MM_ANGRY))
					made++;
				}
				wake_nearby(); /* make sure they're awake --Amy */

			} else {

				while (cnt--) {
					if (enexto(&mm, mm.x, mm.y, &mons[PM_KILLER_BEE]) &&
					makemon(beehivemon(), mm.x, mm.y, MM_ANGRY))
					made++;
				}
			}

			if ( made )
			    pline("You've attracted the tree's former occupants!");
			else
			    You("smell stale honey.");
			maploc->looted |= TREE_SWARM;
			return(1);
		    }
		    if (uarmf && itemhasappearance(uarmf, APP_JUNGLE_BOOTS) ) {
			pline("You kick the tree. Nothing happens.");
			return(1);
		    } else if (RngeJungleAction) {
			pline("You kick the tree. Nothing happens.");
			return(1);
		    } else
		    goto ouch;
		}
		if(IS_TOILET(maploc->typ)) {
		   if(Levitation) goto dumb;
		   pline("Klunk!");
		   if (!rn2(5)) wake_nearby();
		   if (!rn2(4)) breaktoilet(x,y);
		   return(1);
		}
		if(IS_GRAVE(maploc->typ)) {
		   goto ouch;
		}
		if(IS_SINK(maploc->typ)) {
			int gend = poly_gender();
			int washerndx = (gend == 1 || (gend == 2 && rn2(2))) ?
					PM_INCUBUS : PM_SUCCUBUS;
			boolean specwasher = rn2(2) ? 0 : 1;
			boolean specpudding = rn2(2) ? 0 : 1;

			if(Levitation) goto dumb;

			u.cnd_sinkamount++;

			if (!rn2(5)) { /* punish the sucker who keeps kicking sinks :P --Amy */
				pline_The("pipes break!  Water spurts out!");
				level.flags.nsinks--;
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				newsym(x,y);
				return(1);
			} else if (rn2(5)) {
				if(flags.soundok)
					pline("Klunk!  The pipes vibrate noisily.");
				else pline("Klunk!");
				if (!rn2(5)) wake_nearby();
				exercise(A_DEX, TRUE);
				return(1);
			} else if(!(maploc->looted & S_LPUDDING) && !rn2(3) && !((specpudding == 1) && (mvitals[PM_BLACK_PUDDING].mvflags & G_GONE))) {
				if (Blind)
					You_hear("a gushing sound.");
				else
					pline("A %s ooze gushes up from the drain!", specpudding == 1 ? hcolor(NH_BLACK) : "strange");
				(void) makemon((specpudding == 1) ? &mons[PM_BLACK_PUDDING] : mkclass(S_PUDDING, 0), x, y, NO_MM_FLAGS);
				exercise(A_DEX, TRUE);
				newsym(x,y);
				maploc->looted |= S_LPUDDING;
				return(1);
			} else if(!(maploc->looted & S_LDWASHER) && !rn2(3) && !((specwasher == 1) && (mvitals[washerndx].mvflags & G_GONE))) {
				/* can't resist... */
				pline("%s returns!", (Blind ? Something : "The dish washer"));
				if (makemon((specwasher == 1) ? &mons[washerndx] : specialtensmon(226), x, y, NO_MM_FLAGS)) newsym(x,y); /* AD_SSEX */
				maploc->looted |= S_LDWASHER;
				exercise(A_DEX, TRUE);
				return(1);
			} else if(!rn2(3)) {
				pline("Flupp!  %s.", (Blind ? "You hear a sloshing sound" : "Muddy waste pops up from the drain"));
				if(!(maploc->looted & S_LRING)) { /* once per sink */

					if (rn2(5)) { /* nerf nerf NEEEEEERF! :D --Amy */
						pline("But it contains nothing useful.");
						maploc->looted |= S_LRING;
					} else {

						if (!Blind)
							You("see a ring shining in its midst.");
						(void) mkobj_at(RING_CLASS, x, y, TRUE, FALSE);
						newsym(x, y);
						exercise(A_DEX, TRUE);
						exercise(A_WIS, TRUE);	/* a discovery! */
						maploc->looted |= S_LRING;
					}
				}
				return(1);
			}
			goto ouch;
		}
		if (maploc->typ == STAIRS || maploc->typ == WATERTUNNEL || maploc->typ == LADDER ||
						    IS_STWALL(maploc->typ)) {
		    if(!IS_STWALL(maploc->typ) && maploc->ladder == LA_DOWN)
			goto dumb;
ouch:

			if (uamul && uamul->oartifact == ART_WALT_VERSUS_ANNA && !rn2(3) && IS_STWALL(maploc->typ) && !(levl[x][y].wall_info & W_NONDIGGABLE)) {
				pline("Crash! Your kick razed the wall!");
				wake_nearby();
				maploc->typ = CORR;
				newsym(x,y);
				return(1);
			}

			if (Role_if(PM_QUARTERBACK)) { /* expert at kicking and thus doesn't take damage, from PRIME --Amy */
				pline("Thump - you kicked an obstacle.");

				if (Blind) feel_location(x,y); /* we know we hit it */
				if (is_drawbridge_wall(x,y) >= 0) {
					pline_The("drawbridge is unaffected.");
					/* update maploc to refer to the drawbridge */
					(void) find_drawbridge(&x,&y);
					maploc = &levl[x][y];
				}
				if(Is_airlevel(&u.uz) || Levitation)
					hurtle(-u.dx, -u.dy, rn1(2,4), TRUE); /* assume it's heavy */
				return(1);
			}

		    pline(FunnyHallu ? "You stumped your little toe - ouch!" : "Ouch!  That hurts!");
		    exercise(A_DEX, FALSE);
		    exercise(A_STR, FALSE);
		    if (Blind) feel_location(x,y); /* we know we hit it */
		    if (is_drawbridge_wall(x,y) >= 0) {
			pline_The("drawbridge is unaffected.");
			/* update maploc to refer to the drawbridge */
			(void) find_drawbridge(&x,&y);
			maploc = &levl[x][y];
		    }
		    if(!rn2(u.martialstyle == MARTIALSTYLE_KARATE ? 20 : 3)) set_wounded_legs(RIGHT_SIDE, HWounded_legs + 5 + rnd(5));
		    losehp(rnd( (u.martialstyle == MARTIALSTYLE_KARATE) ? 1 : (ACURR(A_CON) > 15) ? 3 : 5), kickstr(buf),
			KILLED_BY);
		    if(Is_airlevel(&u.uz) || Levitation)
			hurtle(-u.dx, -u.dy, rn1(2,4), TRUE); /* assume it's heavy */
		    return(1);
		}
		goto dumb;
	}

	if(maploc->doormask == D_ISOPEN ||
	   maploc->doormask == D_BROKEN ||
	   maploc->doormask == D_NODOOR) {
dumb:
		exercise(A_DEX, FALSE);
		if (martial() || (uarmf && itemhasappearance(uarmf, APP_PISTOL_BOOTS)) || ACURR(A_DEX) >= 16 || rn2(3)) {
			You("kick at empty space.");
			if (Blind) feel_location(x,y);
		} else {
			pline(FunnyHallu ? "Argh... this feels like overdoing sports!" : "Dumb move!  You strain a muscle.");
			exercise(A_STR, FALSE);
			set_wounded_legs(RIGHT_SIDE, HWounded_legs + 5 + rnd(5));
		}
		if ((Is_airlevel(&u.uz) || Levitation) && rn2(2)) {
		    hurtle(-u.dx, -u.dy, 1, TRUE);
		    return 1;		/* you moved, so use up a turn */
		}
		return(1); /* what the HELL??? you kicked at thin air, of course you used up a turn!!! --Amy */
	}

	/* not enough leverage to kick open doors while levitating */
	if(Levitation) goto ouch;

	/* Ali - artifact doors */
	if (artifact_door(x, y)) goto ouch;

	exercise(A_DEX, TRUE);
	/* door is known to be CLOSED or LOCKED */
	if ((rnl(35) < avrg_attrib + (!martial() ? 0 : ACURR(A_DEX))) || (uwep && uwep->oartifact == ART_MORVAK_S_GRINDSTONE) || (uball && uball->oartifact == ART_MORVAK_S_GRINDSTONE) ) {
		boolean shopdoor = *in_rooms(x, y, SHOPBASE) ? TRUE : FALSE;
		/* break the door */
		u.cnd_kicklockcount++;
		wake_nearby();
		if(maploc->doormask & D_TRAPPED) {
		    if (flags.verbose) You("kick the door.");
		    exercise(A_STR, FALSE);
		    maploc->doormask = D_NODOOR;
		    b_trapped("door", FOOT);
		} else if(ACURR(A_STR) > 18 && !rn2(5) && !shopdoor) {
		    pline("As you kick the door, it shatters to pieces!");
		    exercise(A_STR, TRUE);
		    maploc->doormask = D_NODOOR;
		} else {
		    pline("As you kick the door, it crashes open!");
		    exercise(A_STR, TRUE);
		    maploc->doormask = D_BROKEN;
		}
		if (Blind)
		    feel_location(x,y);		/* we know we broke it */
		else
		    newsym(x,y);
		unblock_point(x,y);		/* vision */
		if (shopdoor) {
		    add_damage(x, y, 400L);
		    pay_for_damage("break", FALSE);
		}
		if (in_town(x, y))
		  for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		    if (DEADMONSTER(mtmp)) continue;
		    if((mtmp->data == &mons[PM_WATCHMAN] || mtmp->data == &mons[PM_WATCH_LIEUTENANT] ||
			mtmp->data == &mons[PM_WATCH_CAPTAIN] || mtmp->data == &mons[PM_WATCH_LEADER]) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mpeaceful) {
			if (canspotmon(mtmp))
			    pline("%s yells:", Amonnam(mtmp));
			else
			    You_hear("someone yell:");
			verbalize("Halt, thief!  You're under arrest!");
			(void) angry_guards(FALSE);
			break;
		    }
		  }
	} else {
	    if (Blind) feel_location(x,y);	/* we know we hit it */
	    exercise(A_STR, TRUE);
	    pline("WHAMMM!!!");
	    wake_nearby();
	    if (in_town(x, y))
		for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		    if (DEADMONSTER(mtmp)) continue;
		    if ((mtmp->data == &mons[PM_WATCHMAN] || mtmp->data == &mons[PM_WATCH_LIEUTENANT] ||
				mtmp->data == &mons[PM_WATCH_CAPTAIN] || mtmp->data == &mons[PM_WATCH_LEADER]) &&
			    mtmp->mpeaceful && couldsee(mtmp->mx, mtmp->my)) {
			if (canspotmon(mtmp))
			    pline("%s yells:", Amonnam(mtmp));
			else
			    You_hear("someone yell:");
			if(levl[x][y].looted & D_WARNED) {
			    verbalize("Halt, vandal!  You're under arrest!");
			    (void) angry_guards(FALSE);
			} else {
			    int i;
			    verbalize("Hey, stop damaging that door!");
			    /* [ALI] Since marking a door as warned will have
			     * the side effect of trapping the door, it must be
			     * included in the doors[] array in order that trap
			     * detection will find it.
			     */
			    for(i = doorindex - 1; i >= 0; i--)
				if (x == doors[i].x && y == doors[i].y)
				    break;
			    if (i < 0)
				i = add_door(x, y, (struct mkroom *)0);
			    if (i >= 0)
				levl[x][y].looted |= D_WARNED;
			}
			break;
		    }
		}
	}
	return(1);
}

STATIC_OVL void
drop_to(cc, loc)
coord *cc;
schar loc;
{
	/* cover all the MIGR_xxx choices generated by down_gate() */
	switch (loc) {
	 case MIGR_RANDOM:	/* trap door or hole */
		    if (Is_stronghold(&u.uz)) {
			cc->x = valley_level.dnum;
			cc->y = valley_level.dlevel;
			break;
		    } else if (In_endgame(&u.uz) || Is_botlevel(&u.uz)) {
			cc->y = cc->x = 0;
			break;
		    } /* else fall to the next cases */
	 case MIGR_STAIRS_UP:
	 case MIGR_LADDER_UP:
		    cc->x = u.uz.dnum;
		    cc->y = u.uz.dlevel + 1;
		    break;
	 case MIGR_SSTAIRS:
		    cc->x = sstairs.tolev.dnum;
		    cc->y = sstairs.tolev.dlevel;
		    break;
	 default:
	 case MIGR_NOWHERE:
		    /* y==0 means "nowhere", in which case x doesn't matter */
		    cc->y = cc->x = 0;
		    break;
	}
}

void
impact_drop(missile, x, y, dlev)
struct obj *missile;
xchar x, y, dlev;
{
	schar toloc;
	register struct obj *obj, *obj2;
	register struct monst *shkp;
	long oct, dct, price, debit, robbed;
	boolean angry, costly, isrock;
	coord cc;

	if(!OBJ_AT(x, y)) return;

	toloc = down_gate(x, y);
	drop_to(&cc, toloc);
	if (!cc.y) return;

	if (dlev) {
		/* send objects next to player falling through trap door.
		 * checked in obj_delivery().
		 */
		toloc = MIGR_NEAR_PLAYER;
		cc.y = dlev;
	}

	costly = costly_spot(x, y);
	price = debit = robbed = 0L;
	angry = FALSE;
	shkp = (struct monst *) 0;
	/* if 'costly', we must keep a record of ESHK(shkp) before
	 * it undergoes changes through the calls to stolen_value.
	 * the angry bit must be reset, if needed, in this fn, since
	 * stolen_value is called under the 'silent' flag to avoid
	 * unsavory pline repetitions.
	 */
	if(costly) {
	    if ((shkp = shop_keeper(*in_rooms(x, y, SHOPBASE))) != 0) {
		debit	= ESHK(shkp)->debit;
		robbed	= ESHK(shkp)->robbed;
		angry	= !shkp->mpeaceful;
	    }
	}

	isrock = (missile && missile->otyp == ROCK);
	oct = dct = 0L;
	for(obj = level.objects[x][y]; obj; obj = obj2) {
		obj2 = obj->nexthere;
		if(obj == missile) continue;
		/* number of objects in the pile */
		oct += obj->quan;
		if(obj == uball || obj == uchain) continue;
		/* boulders can fall too, but rarely & never due to rocks */
		if((isrock && obj->otyp == BOULDER) ||
		   rn2(obj->otyp == BOULDER ? 30 : 3)) continue;
		obj_extract_self(obj);

		if(costly) {
		    price += stolen_value(obj, x, y,
				(costly_spot(u.ux, u.uy) &&
				 index(u.urooms, *in_rooms(x, y, SHOPBASE))),
				TRUE, FALSE);
		    /* set obj->no_charge to 0 */
		    if (Has_contents(obj))
			picked_container(obj);	/* does the right thing */
		    if (obj->oclass != COIN_CLASS)
			obj->no_charge = 0;
		}

		add_to_migration(obj);
		obj->ox = cc.x;
		obj->oy = cc.y;
		obj->owornmask = (long)toloc;

		/* number of fallen objects */
		dct += obj->quan;
	}

	if (dct && cansee(x, y)) {      /* at least one object fell */
	    const char *what = (dct == 1L ? "object falls" : "objects fall");

	    if (missile)
		pline("From the impact, %sother %s.",
		      dct == oct ? "the " : dct == 1L ? "an" : "", what);
	    else if (oct == dct)
		pline("%s adjacent %s %s.",
		      dct == 1L ? "The" : "All the", what, gate_str);
	    else
		pline("%s adjacent %s %s.",
		      dct == 1L ? "One of the" : "Some of the",
		      dct == 1L ? "objects falls" : what, gate_str);
	}

	if(costly && shkp && price) {
		if(ESHK(shkp)->robbed > robbed) {
		    You("removed %ld %s worth of goods!", price, currency(price));
		    if(cansee(shkp->mx, shkp->my)) {
			if(ESHK(shkp)->customer[0] == 0)
			    (void) strncpy(ESHK(shkp)->customer, plname, PL_NSIZ);
			if(angry)
			    pline("%s is infuriated!", Monnam(shkp));
			else pline("\"%s, you are a thief!\"", plname);
		    } else  You_hear("a scream, \"Thief!\"");
		    hot_pursuit(shkp);
		    (void) angry_guards(FALSE);
		    return;
		}
		if(ESHK(shkp)->debit > debit) {
		    long amt = (ESHK(shkp)->debit - debit);
		    You("owe %s %ld %s for goods lost.",
			Monnam(shkp),
			amt, currency(amt));
		}
	}

}

/* NOTE: ship_object assumes otmp was FREED from fobj or invent.
 * <x,y> is the point of drop.  otmp is _not_ an <x,y> resident:
 * otmp is either a kicked, dropped, or thrown object.
 */
boolean
ship_object(otmp, x, y, shop_floor_obj)
xchar  x, y;
struct obj *otmp;
boolean shop_floor_obj;
{
	schar toloc;
	xchar ox, oy;
	coord cc;
	struct obj *obj;
	struct trap *t;
	boolean nodrop, unpaid, container, impact = FALSE;
	long n = 0L;

	if (!otmp) return(FALSE);
	if ((toloc = down_gate(x, y)) == MIGR_NOWHERE) return(FALSE);
	drop_to(&cc, toloc);
	if (!cc.y) return(FALSE);

	/* objects other than attached iron ball always fall down ladder,
	   but have a chance of staying otherwise */
	nodrop = (otmp == uball) || (otmp == uchain) ||
		(toloc != MIGR_LADDER_UP && rn2(3));

	container = Has_contents(otmp);
	unpaid = (otmp->unpaid || (container && count_unpaid(otmp->cobj)));

	if(OBJ_AT(x, y)) {
	    for(obj = level.objects[x][y]; obj; obj = obj->nexthere)
		if(obj != otmp) n += obj->quan;
	    if(n) impact = TRUE;
	}
	/* boulders never fall through trap doors, but they might knock
	   other things down before plugging the hole */
	if (otmp->otyp == BOULDER &&
		((t = t_at(x, y)) != 0) &&
		(t->ttyp == TRAPDOOR || t->ttyp == SHAFT_TRAP || t->ttyp == CURRENT_SHAFT || t->ttyp == HOLE)) {
	    if (impact) impact_drop(otmp, x, y, 0);
	    return FALSE;		/* let caller finish the drop */
	}

	if (cansee(x, y)) otransit_msg(otmp, nodrop, n);

	if (nodrop) {
	    if (impact) impact_drop(otmp, x, y, 0);
	    return(FALSE);
	}

	if(unpaid || shop_floor_obj) {
	    if(unpaid) {
		subfrombill(otmp, shop_keeper(*u.ushops));
		(void)stolen_value(otmp, u.ux, u.uy, TRUE, FALSE, FALSE);
	    } else {
		ox = otmp->ox;
		oy = otmp->oy;
		(void)stolen_value(otmp, ox, oy,
			  (costly_spot(u.ux, u.uy) &&
			      index(u.urooms, *in_rooms(ox, oy, SHOPBASE))),
			  FALSE, FALSE);
	    }
	    /* set otmp->no_charge to 0 */
	    if(container)
		picked_container(otmp); /* happens to do the right thing */
	    if(otmp->oclass != COIN_CLASS)
		otmp->no_charge = 0;
	}

	if (otmp == uwep) setuwep((struct obj *)0, FALSE, TRUE);
	if (otmp == uswapwep) setuswapwep((struct obj *)0, FALSE);
	if (otmp == uquiver) setuqwep((struct obj *)0);

	/* some things break rather than ship
	 * Amy edit: but it shouldn't happen all the damn time!! */
	if (!rn2(otmp->oerodeproof ? 100 : 20) && breaktest(otmp)) {
	    const char *result;

	    if (objects[otmp->otyp].oc_material == MT_GLASS || objects[otmp->otyp].oc_material == MT_OBSIDIAN || is_vitric(otmp) || otmp->otyp == EXPENSIVE_CAMERA )
	    {
		if (otmp->otyp == MIRROR)
		    change_luck(-2);
		result = "crash";
	    } else {
		/* penalty for breaking eggs laid by you */
		if (otmp->otyp == EGG && otmp->spe && otmp->corpsenm >= LOW_PM)
		    change_luck((schar) -min(otmp->quan, 5L));
		result = "splat";
	    }
	    You_hear("a muffled %s.",result);
	    obj_extract_self(otmp);
	    obfree(otmp, (struct obj *) 0);
	    return TRUE;
	}

	add_to_migration(otmp);
	otmp->ox = cc.x;
	otmp->oy = cc.y;
	otmp->owornmask = (long)toloc;
	/* boulder from rolling boulder trap, no longer part of the trap */
	if (otmp->otyp == BOULDER) otmp->otrapped = 0;

	if(impact) {
	    /* the objs impacted may be in a shop other than
	     * the one in which the hero is located.  another
	     * check for a shk is made in impact_drop.  it is, e.g.,
	     * possible to kick/throw an object belonging to one
	     * shop into another shop through a gap in the wall,
	     * and cause objects belonging to the other shop to
	     * fall down a trap door--thereby getting two shopkeepers
	     * angry at the hero in one shot.
	     */
	    impact_drop(otmp, x, y, 0);
	    newsym(x,y);
	}
	return(TRUE);
}

void
obj_delivery()
{
	register struct obj *otmp, *otmp2;
	register int nx, ny;
	long where;

	for (otmp = migrating_objs; otmp; otmp = otmp2) {
	    otmp2 = otmp->nobj;
	    if (otmp->ox != u.uz.dnum || otmp->oy != u.uz.dlevel) continue;

	    obj_extract_self(otmp);
	    where = otmp->owornmask;		/* destination code */
	    otmp->owornmask = 0L;

	    switch ((int)where) {
	     case MIGR_STAIRS_UP:   nx = xupstair,  ny = yupstair;
				break;
	     case MIGR_LADDER_UP:   nx = xupladder,  ny = yupladder;
				break;
	     case MIGR_SSTAIRS:	    nx = sstairs.sx,  ny = sstairs.sy;
				break;
	     case MIGR_NEAR_PLAYER: nx = u.ux,  ny = u.uy;
				break;
	     default:
	     case MIGR_RANDOM:	    nx = ny = 0;
				break;
	    }
	    if (nx > 0) {
		place_object(otmp, nx, ny);
		stackobj(otmp);
		(void)scatter(nx, ny, rnd(2), 0, otmp);
	    } else {		/* random location */
		/* set dummy coordinates because there's no
		   current position for rloco() to update */
		otmp->ox = otmp->oy = 0;
		rloco(otmp);
	    }
	}
}

STATIC_OVL void
otransit_msg(otmp, nodrop, num)
register struct obj *otmp;
register boolean nodrop;
long num;
{
	char obuf[BUFSZ];

	sprintf(obuf, "%s%s",
		 (otmp->otyp == CORPSE &&
			type_is_pname(&mons[otmp->corpsenm])) ? "" : "The ",
		 cxname(otmp));

	if(num) { /* means: other objects are impacted */
	    sprintf(eos(obuf), " %s %s object%s",
		    otense(otmp, "hit"),
		    num == 1L ? "another" : "other",
		    num > 1L ? "s" : "");
	    if(nodrop)
		sprintf(eos(obuf), ".");
	    else
		sprintf(eos(obuf), " and %s %s.",
			otense(otmp, "fall"), gate_str);
	    pline("%s", obuf);
	} else if(!nodrop)
	    pline("%s %s %s.", obuf, otense(otmp, "fall"), gate_str);
}

/* migration destination for objects which fall down to next level */
schar
down_gate(x, y)
xchar x, y;
{
	struct trap *ttmp;

	gate_str = 0;
	/* this matches the player restriction in goto_level() */
	if (on_level(&u.uz, &qstart_level) && !ok_to_quest())
	    return MIGR_NOWHERE;

	if ((xdnstair == x && ydnstair == y) ||
		(sstairs.sx == x && sstairs.sy == y && !sstairs.up)) {
	    gate_str = "down the stairs";
	    return (xdnstair == x && ydnstair == y) ?
		    MIGR_STAIRS_UP : MIGR_SSTAIRS;
	}
	if (xdnladder == x && ydnladder == y) {
	    gate_str = "down the ladder";
	    return MIGR_LADDER_UP;
	}

	if (((ttmp = t_at(x, y)) != 0 && ttmp->tseen) &&
		(ttmp->ttyp == TRAPDOOR || ttmp->ttyp == SHAFT_TRAP || ttmp->ttyp == CURRENT_SHAFT || ttmp->ttyp == HOLE)) {
	    gate_str = (ttmp->ttyp == TRAPDOOR) ?
		    "through the trap door" : (ttmp->ttyp == CURRENT_SHAFT) ? "through the shaft" :
			(ttmp->ttyp == SHAFT_TRAP) ? "through the shaft" : "through the hole";
	    return MIGR_RANDOM;
	}
	return MIGR_NOWHERE;
}

/*dokick.c*/
