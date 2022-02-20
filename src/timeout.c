/*	SCCS Id: @(#)timeout.c	3.4	2002/12/17	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "lev.h"	/* for checking save modes */

STATIC_DCL void stoned_dialogue(void);
STATIC_DCL void phasing_dialogue(void);
STATIC_DCL void vomiting_dialogue(void);
STATIC_DCL void choke_dialogue(void);
STATIC_DCL void slime_dialogue(void);
STATIC_DCL void slime_dialogue(void);
STATIC_DCL void slip_or_trip(void);
STATIC_DCL void see_lamp_flicker(struct obj *, const char *);
STATIC_DCL void lantern_message(struct obj *);
STATIC_DCL void accelerate_timer(SHORT_P, void *, long);
STATIC_DCL void cleanup_burn(void *,long);

#ifdef OVLB

/* He is being petrified - dialogue by inmet!tower */
static NEARDATA const char * const stoned_texts[] = {
	"You are slowing down.",		/* 7 */
	"You are feeling a dangerous process in your body.",		/* 6 */
	"You are struggling to keep your movements.",		/* 5 */
	"Your limbs are stiffening.",		/* 4 */
	"Your limbs have turned to stone.",	/* 3 */
	"You have turned to stone.",		/* 2 */
	"You are a statue."			/* 1 */
};

STATIC_OVL void
stoned_dialogue()
{
	register long i = (Stoned & TIMEOUT);

	if (i > 0L && i <= SIZE(stoned_texts))
		pline("%s", stoned_texts[SIZE(stoned_texts) - i]);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	if (i == 7L)
		HFast = 0L;
	if (i == 1L) {
		nomul(-3, "getting stoned", FALSE);
		nomovemsg = 0;
	}
	exercise(A_DEX, FALSE);
}

STATIC_OVL void
phasing_dialogue()
{
    if (Phasing == 15) {
        if (!FunnyHallu) {
            Your("body is beginning to feel more solid.");
        } else {
            You_feel("more distant from the spirit world.");
        }
        stop_occupation();
    } else if (Phasing == 1) {
        if (!FunnyHallu) {
            Your("body is solid again.");
        } else {
            You_feel("totally separated from the spirit world.");
        }
        stop_occupation();
    }
}

/* He is getting sicker and sicker prior to vomiting */
static NEARDATA const char * const vomiting_texts[] = {
	"are feeling mildly nauseated.",	/* 14 */
	"feel slightly confused.",		/* 11 */
	"can't seem to think straight.",	/* 8 */
	"feel incredibly sick.",		/* 5 */
	"suddenly vomit!"			/* 2 */
};

STATIC_OVL void
vomiting_dialogue()
{
	register long i = (Vomiting & TIMEOUT) / 3L;

	if ((((Vomiting & TIMEOUT) % 3L) == 2) && (i >= 0)
	    && (i < SIZE(vomiting_texts)))
		You("%s", vomiting_texts[SIZE(vomiting_texts) - i - 1]);

	switch ((int) i) {
	case 0:
		vomit();
		morehungry(20);
		break;
	case 2:
		make_stunned(HStun + d(2,4), FALSE);
		/* fall through */
	case 3:
		make_confused(HConfusion + d(2,4), FALSE);
		break;
	}
	exercise(A_CON, FALSE);
}

static NEARDATA const char * const choke_texts[] = {
	"You find it hard to breathe.",
	"You find it hard to breathe.",
	"You're gasping for air.",
	"You're gasping for air.",
	"You can no longer breathe.",
	"You can no longer breathe.",
	"You're turning %s.",
	"You're turning %s.",
	"You're about to suffocate.",
	"You suffocate."
};

static NEARDATA const char * const choke_texts2[] = {
	"Your %s is becoming constricted.",
	"Your %s is becoming constricted.",
	"Your blood is having trouble reaching your brain.",
	"Your blood is having trouble reaching your brain.",
	"The pressure on your %s increases.",
	"The pressure on your %s increases.",
	"Your consciousness is fading.",
	"Your consciousness is fading.",
	"You're about to suffocate.",
	"You suffocate."
};

STATIC_OVL void
choke_dialogue()
{
	register long i = (Strangled & TIMEOUT);

	if(i > 0 && i <= SIZE(choke_texts)) {
	    if (Breathless || !rn2(50))
		pline(choke_texts2[SIZE(choke_texts2) - i], body_part(NECK));
	    else {
		const char *str = choke_texts[SIZE(choke_texts)-i];
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		if (index(str, '%')) {
		    pline(str, hcolor(NH_BLUE));
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */ }
		else
		    pline("%s", str);
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    }
	}
	exercise(A_STR, FALSE);
}

static NEARDATA const char * const slime_texts[] = {
	"You are turning a little %s.",           /* 5 */
	"Your limbs are getting oozy.",              /* 4 */
	"Your skin begins to peel away.",            /* 3 */
	"You are turning into %s.",       /* 2 */
	"You have become %s."             /* 1 */
};

STATIC_OVL void
slime_dialogue()
{
	register long i = (Slimed & TIMEOUT) / 20L;

	if (((Slimed & TIMEOUT) % 2L) && i >= 0L
		&& i < SIZE(slime_texts)) {
	    const char *str = slime_texts[SIZE(slime_texts) - i - 1L];

	    if (index(str, '%')) {
		if (i == 4L) {	/* "you are turning green" */
		    if (!Blind)	/* [what if you're already green?] */
			pline(str, hcolor(NH_GREEN));
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		} else {
		    pline(str, an(Hallucination ? rndmonnam() : "green slime"));
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

			/* make it more obvious for the player how much time they have left --Amy */
			if (i == 0L && Slimed > 1) pline("You have %ld turns to live.", Slimed);
		}
	    } else
		pline("%s", str);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	}
	if (i == 3L) {	/* limbs becoming oozy */
	    HFast = 0L;	/* lose intrinsic speed */
	    stop_occupation();
	    if (multi > 0) nomul(0, 0, FALSE);
	}
	exercise(A_DEX, FALSE);
}

void
burn_away_slime()
{
	if (Slimed) {
	    pline_The("slime that covers you is burned away!");
	    Slimed = 0L;
	    flags.botl = 1;
	}

	if (Frozen) { /* Fire also cures freezing. --Amy */
		pline_The("heat melts the ice surrounding you!");
		make_frozen(0L, FALSE);
		flags.botl = 1;
	}

	return;
}


#endif /* OVLB */
#ifdef OVL0

void
nh_timeout()
{
	register struct prop *upp;
/*
	char c;
 */
	int sleeptime;
	int m_idx;
	int baseluck = (flags.moonphase == FULL_MOON) ? 1 : 0;
	int nastytrapdur;
	int blackngdur;
	struct obj *otmpi, *otmpii;

	if (flags.friday13) baseluck -= 1;

	/* get extra uses out of the (now limited) #youpoly command after a while --Amy */
	if (Race_if(PM_WARPER) && !rn2(10000)) u.youpolyamount++;
	if (Race_if(PM_DOPPELGANGER) && !rn2(5000)) u.youpolyamount++;
	if (Role_if(PM_SHAPESHIFTER) && !rn2(5000)) u.youpolyamount++;
	if (Race_if(PM_HEMI_DOPPELGANGER) && !rn2(4000)) u.youpolyamount++;
	if (Race_if(PM_DEATHMOLD) && !rn2(3000)) u.youpolyamount++;
	if (Race_if(PM_UNGENOMOLD) && !rn2(2000)) u.youpolyamount++;
	if (Race_if(PM_MOULD) && !rn2(1000)) u.youpolyamount++;

	if (MCReduction && MCReduction % 5000 == 0) pline("Your magic cancellation seems to work a bit better again.");

	if (u.negativeprotection && !rn2(2500)) u.negativeprotection--;
	if (u.negativeprotection < 0) u.negativeprotection = 0; /* fail safe */
	if (u.tremblingamount && !rn2(1000)) u.tremblingamount--;
	if (u.tremblingamount < 0) u.tremblingamount = 0; /* fail safe */

	if (u.inasuppression) {
		u.inasuppression--;
		if (u.inasuppression < 0) u.inasuppression = 0; /* fail safe */
	}

	if (u.badfcursed) {
		u.badfcursed--;
		if (u.badfcursed < 0) u.badfcursed = 0; /* fail safe */
		if (!u.badfcursed) You("are no longer doing a journey on the path to nowhere.");
	}
	if (u.badfdoomed) {
		u.badfdoomed--;
		if (u.badfdoomed < 0) u.badfdoomed = 0; /* fail safe */
		if (!u.badfdoomed) You("are on the way back from the journey on the path to nowhere.");
	}

	if (u.pract_smokingtimer) {
		u.pract_smokingtimer--;
		if (u.pract_smokingtimer < 0) u.pract_smokingtimer = 0; /* fail safe */
	}

	if (SimeoutBug || u.uprops[SIMEOUT_BUG].extrinsic || have_simeoutstone()) {
		if (!rn2(2500)) {
			u.usanity += (YouGetLotsOfSanity ? rnd(20) : 1);
			if (flags.showsanity) flags.botl = 1;
		}
	} else if (u.usanity && !isevilvariant && !rn2(isfriday ? 2500 : 1000)) {
		u.usanity--;

		/* mineral helps against sanity --Amy */
		if (uwep && objects[uwep->otyp].oc_material == MT_MINERAL) {
			u.usanity--;
		}
		if (u.twoweap && uswapwep && objects[uswapwep->otyp].oc_material == MT_MINERAL) {
			u.usanity--;
		}
		if (uarm && objects[uarm->otyp].oc_material == MT_MINERAL) {
			u.usanity--;
		}
		if (uarmc && objects[uarmc->otyp].oc_material == MT_MINERAL) {
			u.usanity--;
		}
		if (uarmh && objects[uarmh->otyp].oc_material == MT_MINERAL) {
			u.usanity--;
		}
		if (uarms && objects[uarms->otyp].oc_material == MT_MINERAL) {
			u.usanity--;
		}
		if (uarmg && objects[uarmg->otyp].oc_material == MT_MINERAL) {
			u.usanity--;
		}
		if (uarmf && objects[uarmf->otyp].oc_material == MT_MINERAL) {
			u.usanity--;
		}
		if (uarmu && objects[uarmu->otyp].oc_material == MT_MINERAL) {
			u.usanity--;
		}
		if (uamul && objects[uamul->otyp].oc_material == MT_MINERAL) {
			u.usanity--;
		}
		if (uimplant && objects[uimplant->otyp].oc_material == MT_MINERAL) {
			u.usanity--;
		}
		if (uleft && objects[uleft->otyp].oc_material == MT_MINERAL) {
			u.usanity--;
		}
		if (uright && objects[uright->otyp].oc_material == MT_MINERAL) {
			u.usanity--;
		}
		if (ublindf && objects[ublindf->otyp].oc_material == MT_MINERAL) {
			u.usanity--;
		}
		if (u.usanity < 0) u.usanity = 0;

		if (flags.showsanity) flags.botl = 1;
	}
	if (u.usanity < 0) u.usanity = 0; /* fail safe */
	if (u.chokhmahdamage && !rn2(10000) && !(uarmc && uarmc->otyp == CLOAK_OF_ESCALATION) ) u.chokhmahdamage--;
	if (u.chokhmahdamage < 0) u.chokhmahdamage = 0; /* fail safe */

	if (u.ragnaroktimer > 0) {
		u.ragnaroktimer--;
		if (u.ragnaroktimer < 1) {
			u.ragnaroktimer = 3; /* FILTHY HANGUP CHEATER */
			pline("UH-OH! The end of the world is happening!");
			ragnarok(TRUE);
			if (evilfriday) evilragnarok(TRUE,level_difficulty());
			u.ragnaroktimer = 0; /* okay so you didn't hang up */
		}
	}

	if (PlayerBleeds && !(u.uinvulnerable)) {
		int bleedingdamage = 1;
		if (PlayerBleeds > 4) bleedingdamage = rnd(PlayerBleeds / 5);
		losehp(bleedingdamage, (PlayerBleeds > 100) ? "a hemorrhage" : (PlayerBleeds > 50) ? "profuse bleedout" : "bleedout", KILLED_BY);
		if (!rn2(10)) pline((PlayerBleeds > 100) ? "You're squirting blood everywhere!" : (PlayerBleeds > 50) ? "You're bleeding severely!" : "You're bleeding!");
		if (bleedingdamage > 1) {

			/* bad luck makes your wounds heal more slowly --Amy */
			int bleedreductionchance = 100;
			if (Luck < 0) bleedreductionchance += (Luck * 5); /* because "Luck" is negative! */
			if (isfriday && bleedreductionchance > 1) bleedreductionchance /= 2;

			/* being a bleeder (or hemophage = racial bleeder) means you have hemophilia... */
			if ((Role_if(PM_BLEEDER) || Race_if(PM_HEMOPHAGE)) && rn2(3)) bleedreductionchance = 0;

			if (RngeHemophilia && rn2(3)) bleedreductionchance = 0;

			if (bleedreductionchance > rn2(100)) {

				PlayerBleeds -= (bleedingdamage - 1);
				if (StrongDiminishedBleeding) {
					PlayerBleeds /= 2;
				} else if (DiminishedBleeding) {
					if (!rn2(2) && PlayerBleeds > 1) PlayerBleeds -= rnd(PlayerBleeds / 2);
				}
			}
		}
		if (!PlayerBleeds) pline("Your bleeding stops.");

		if (PlayerBleeds < 0) PlayerBleeds = 0; /* fail safe */
	}

	if (u.echolocationspell) {
		u.echolocationspell--;
		if (!u.echolocationspell) You("no longer have echolocation.");
		if (u.echolocationspell < 0) u.echolocationspell = 0; /* fail safe */
	}

	if (u.gaugetimer) {
		u.gaugetimer--;
		if (!u.gaugetimer) Your("gauge is full again.");
		if (u.gaugetimer < 0) u.gaugetimer = 0; /* fail safe */
	}

	if (u.battertimer) {
		u.battertimer--;
		if (!u.battertimer) Your("battering ram is ready again.");
		if (u.battertimer < 0) u.battertimer = 0; /* fail safe */
	}

	if (u.garbagetrucktime) {
		u.garbagetrucktime--;
		if (u.garbagetrucktime < 0) u.garbagetrucktime = 0; /* fail safe */
		if (!u.garbagetrucktime) {
			u.garbagecleaned = 0;
			adjalign(50);
			pline_The("garbage truck arrived, and allowed you to empty your trash bin. Well done!");
		}
	}

	if (u.contingencyturns) {
		u.contingencyturns--;
		if (!u.contingencyturns) pline("The effect of contingency ends.");
		if (u.contingencyturns < 0) u.contingencyturns = 0; /* fail safe */
	}

	if (u.breathenhancetimer) {
		u.breathenhancetimer--;
		if (!u.breathenhancetimer) pline("Your breath is normal again.");
		if (u.breathenhancetimer < 0) u.breathenhancetimer = 0; /* fail safe */
	}

	if (u.bodyfluideffect) {
		u.bodyfluideffect--;
		if (!u.bodyfluideffect) pline("Your acidic skin disappears.");
		if (u.bodyfluideffect < 0) u.bodyfluideffect = 0; /* fail safe */
	}

	if (u.antitelespelltimeout) {
		u.antitelespelltimeout--;
		if (!u.antitelespelltimeout) pline("The anti-teleportation field dissipates.");
		if (u.antitelespelltimeout < 0) u.antitelespelltimeout = 0; /* fail safe */
	}

	if (u.persiantimer) {
		u.persiantimer--;
		if (u.persiantimer < 0) u.persiantimer = 0; /* fail safe */
	}

	if (u.horsehopturns) {
		u.horsehopturns--;
		if (!u.horsehopturns) pline("You can no longer horse hop.");
		if (u.horsehopturns < 0) u.horsehopturns = 0; /* fail safe */
	}

	if (u.ragnarokspelltimeout) {
		u.ragnarokspelltimeout--;
		if (u.ragnarokspelltimeout < 0) u.ragnarokspelltimeout = 0; /* fail safe */
	}

	if (u.sokosolveboulder) {
		u.sokosolveboulder--;
		if (!u.sokosolveboulder && issokosolver) pline("You're capable of using #monster to create a boulder.");
		if (u.sokosolveboulder < 0) u.sokosolveboulder = 0; /* fail safe */
	}

	if (u.walscholarpass) {
		u.walscholarpass--;
		if (!u.walscholarpass) pline("You can no longer pass through grave walls.");
		if (u.walscholarpass < 0) u.walscholarpass = 0; /* fail safe */
	}

	if (u.cellargravate) {
		u.cellargravate--;
		if (u.cellargravate < 0) u.cellargravate = 0; /* fail safe */
	}

	if (u.sokosolveuntrap) {
		u.sokosolveuntrap--;
		if (!u.sokosolveuntrap && issokosolver) pline("You're capable of using #monster to remove adjacent traps.");
		if (u.sokosolveuntrap < 0) u.sokosolveuntrap = 0; /* fail safe */
	}

	if (u.inertia && rn2(10) && (!Race_if(PM_SPIRIT) || rn2(2)) ) {
		u.inertia--;
		if (!u.inertia) You_feel("less slow.");
		if (u.inertia < 0) u.inertia = 0; /* fail safe */
	}

	if (u.duriworking) {
		u.duriworking--;
		if (u.duriworking < 0) u.duriworking = 0; /* fail safe */
		if (!u.duriworking) pline("Duri should be finished with your artifact by now.");
	}

	if (u.powerfailure) {
		u.powerfailure--;
		if (u.powerfailure < 0) u.powerfailure = 0; /* fail safe */
		if (!u.powerfailure) pline("Your power comes back online.");
	}

	if (u.demagogueabilitytimer) {
		u.demagogueabilitytimer--;
		if (u.demagogueabilitytimer < 0) u.demagogueabilitytimer = 0; /* fail safe */
		if (!u.demagogueabilitytimer && isdemagogue) pline("You're capable of using #monster to temporarily change your role.");
	}

	if (u.hussyperfume) {
		u.hussyperfume--;
		if (u.hussyperfume < 0) u.hussyperfume = 0; /* fail safe */
		if (!u.hussyperfume) pline("You are capable of spreading the perfume again.");
	}

	if (u.irahapoison) {
		u.irahapoison--;
		if (u.irahapoison < 0) u.irahapoison = 0; /* fail safe */
		if (!u.irahapoison) pline("You are capable of poisoning your weapon again.");
	}

	if (u.acutraining) {
		u.acutraining--;
		if (u.acutraining < 0) u.acutraining = 0; /* fail safe */
		if (!u.acutraining) pline("Your extra skill training ends.");
	}

	if (u.egglayingtimeout) {
		u.egglayingtimeout--;
		if (u.egglayingtimeout < 0) u.egglayingtimeout = 0; /* fail safe */
		if (!u.egglayingtimeout) pline("You are capable of laying eggs again.");
	}

	if (u.snaildigging) {
		u.snaildigging--;
		if (u.snaildigging < 0) u.snaildigging = 0; /* fail safe */
		if (!u.snaildigging) pline("You are capable of using your digging ability again.");
	}

	if (u.dragonpolymorphtime) {
		u.dragonpolymorphtime--;
		if (u.dragonpolymorphtime < 0) u.dragonpolymorphtime = 0; /* fail safe */
		if (!u.dragonpolymorphtime) pline("You are capable of polymorphing into a dragon again.");
	}

	if (u.werepolymorphtime) {
		u.werepolymorphtime--;
		if (u.werepolymorphtime < 0) u.werepolymorphtime = 0; /* fail safe */
		if (!u.werepolymorphtime) pline("You are capable of polymorphing into a werecreature again.");
	}

	if ((Glib > 1) && uarmh && uarmh->oartifact == ART_TARI_FEFALAS) {
		Glib = 1;
	}

	if (u.tunnelized) {
		u.tunnelized--;
		if (u.tunnelized< 0) u.tunnelized = 0; /* fail safe */
		if (!u.tunnelized) pline("You get the tunneling dirt off your clothes.");
	}

	if (u.temprecursiontime) {

		if (u.temprecursiontime < 0) u.temprecursiontime = 1; /* fail safe */

		u.temprecursiontime--;
		if (!u.temprecursiontime) {
			u.temprecursion = 0;
			if (u.oldrecursionrole != -1) {
				urole = roles[u.oldrecursionrole];
				flags.initrole = u.oldrecursionrole;
			}
			if (u.oldrecursionrace != -1) {
				urace = races[u.oldrecursionrace];
				flags.initrace = u.oldrecursionrace;
			}
			u.oldrecursionrole = -1;
			u.oldrecursionrace = -1;
			pline("You appear to be a %s %s again.", urace.noun, (flags.female && urole.name.f) ? urole.name.f : urole.name.m);
			init_uasmon();

		}
	}

	if (u.demagoguerecursiontime) {

		if (u.demagoguerecursiontime < 0) u.demagoguerecursiontime = 1; /* fail safe */
		if (In_endgame(&u.uz)) u.demagoguerecursiontime = 1; /* can't use it to ascend as something else --Amy */

		u.demagoguerecursiontime--;
		if (!u.demagoguerecursiontime) {
			u.demagoguerecursion = 0;
			if (u.oldrecursionrole != -1) {
				urole = roles[u.oldrecursionrole];
				flags.initrole = u.oldrecursionrole;
			}
			u.oldrecursionrole = -1;
			pline("You appear to be a %s %s again.", urace.noun, (flags.female && urole.name.f) ? urole.name.f : urole.name.m);
			init_uasmon();
		}

	}

	if (u.roxannemode && u.roxannetimer) {
		u.roxannetimer--;
		if (u.roxannetimer < 0) u.roxannetimer = 0; /* fail safe */
		if (u.roxannetimer == 0) {
			struct attack* attkptr;
			struct permonst* poisonroxanne = &mons[PM_ROXANNE];

			attkptr = &poisonroxanne->mattk[4];
			attkptr->aatyp = AT_NONE;
			attkptr->adtyp = AD_PHYS;
			attkptr->damn = 0;
			attkptr->damd = 0;

			attkptr = &poisonroxanne->mattk[5];
			attkptr->aatyp = AT_NONE;
			attkptr->adtyp = AD_PHYS;
			attkptr->damn = 0;
			attkptr->damd = 0;

			pline("Roxanne's poison enchantment ran out of fuel.");

		}
	}

	if (u.fumbleduration) u.fumbleduration--;
	if (u.fumbleduration < 0) u.fumbleduration = 0; /* fail safe */
	if (u.antimagicshell) {
		u.antimagicshell--;
		if (!u.antimagicshell) pline("You no longer produce an anti-magic shell.");
	}
	if (u.antimagicshell < 0) u.antimagicshell = 0; /* fail safe */

	if (u.soviettemporary) {
		u.soviettemporary--;
	}
	if (u.soviettemporary < 0) u.soviettemporary = 0; /* fail safe */

	if (u.evilvartemporary) {
		u.evilvartemporary--;
	}
	if (u.evilvartemporary < 0) u.evilvartemporary = 0; /* fail safe */

	if (u.legscratching > 1 && !FemtrapActiveJeanetta && !Role_if(PM_BLEEDER) && !Race_if(PM_HEMOPHAGE) && !BloodLossProblem && !have_bloodlossstone() && !u.uprops[BLOOD_LOSS].extrinsic && !rn2(1000)) u.legscratching--; /* always time out once per 1000 turns --Amy */

	if (!rn2(1000) && (Role_if(PM_ACTIVISTOR) || Race_if(PM_PEACEMAKER) ) && ( !( uarmu && (uarmu->otyp == RUFFLED_SHIRT || uarmu->otyp == VICTORIAN_UNDERWEAR)) || !rn2(10)) ) {
		You_hear("maniacal laughter!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Kha-kha-kha-kha-kha-KDZH KDZH, tip bloka l'da smeyetsya yego tortsa, potomu chto vy teryayete vse vashi vstroyennyye funktsii!" : "Hoehoehoehoe!");
	    attrcurse();
	}

	if (!rn2(1000) && u.uprops[INTRINSIC_LOSS].extrinsic && ( !( uarmu && (uarmu->otyp == RUFFLED_SHIRT || uarmu->otyp == VICTORIAN_UNDERWEAR)) || !rn2(10)) ) {
		You_hear("maniacal laughter!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Kha-kha-kha-kha-kha-KDZH KDZH, tip bloka l'da smeyetsya yego tortsa, potomu chto vy teryayete vse vashi vstroyennyye funktsii!" : "Hoehoehoehoe!");
	    attrcurse();
	}

	if (!rn2(1000) && (uimplant && uimplant->oartifact == ART_LAUGHING_AT_MIDNIGHT) && ( !( uarmu && (uarmu->otyp == RUFFLED_SHIRT || uarmu->otyp == VICTORIAN_UNDERWEAR)) || !rn2(10)) ) {
		You_hear("maniacal laughter!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Kha-kha-kha-kha-kha-KDZH KDZH, tip bloka l'da smeyetsya yego tortsa, potomu chto vy teryayete vse vashi vstroyennyye funktsii!" : "Hoehoehoehoe!");
	    attrcurse();
	}

	if (!rn2(1000) && IntrinsicLossProblem && ( !( uarmu && (uarmu->otyp == RUFFLED_SHIRT || uarmu->otyp == VICTORIAN_UNDERWEAR)) || !rn2(10)) ) {
		You_hear("maniacal laughter!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Kha-kha-kha-kha-kha-KDZH KDZH, tip bloka l'da smeyetsya yego tortsa, potomu chto vy teryayete vse vashi vstroyennyye funktsii!" : "Hoehoehoehoe!");
	    attrcurse();
	}

	if (!rn2(1000) && have_intrinsiclossstone() && ( !( uarmu && (uarmu->otyp == RUFFLED_SHIRT || uarmu->otyp == VICTORIAN_UNDERWEAR)) || !rn2(10)) ) {
		You_hear("maniacal laughter!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Kha-kha-kha-kha-kha-KDZH KDZH, tip bloka l'da smeyetsya yego tortsa, potomu chto vy teryayete vse vashi vstroyennyye funktsii!" : "Hoehoehoehoe!");
	    attrcurse();
	}

	if (u.burrowed) {
		u.burrowed--;
		if (u.burrowed < 0) u.burrowed = 0;
		if (!u.utrap || !u.utraptype || (u.utraptype != TT_INFLOOR)) u.burrowed = 0;
		if (!(u.burrowed)) pline("Your extra armor class from burrowing has timed out.");
	}

	if (u.magicshield) {
		u.magicshield--;
		if (u.magicshield < 0) u.magicshield = 0;
		if (!(u.magicshield)) pline("Your magic shield dissipates.");
	}

	if (u.thornspell) {
		u.thornspell--;
		if (u.thornspell < 0) u.thornspell = 0;
		if (!(u.thornspell)) pline("Your thorny skin shatters.");
	}

	if (u.enchantspell) {
		u.enchantspell--;
		if (u.enchantspell < 0) u.enchantspell = 0;
		if (!(u.enchantspell)) pline("Your weapon is no longer enchanted.");
	}

	if (u.stasistime) {
		u.stasistime--;
		if (u.stasistime < 0) u.stasistime = 0;
	}

	if (u.berserktime) {
		u.berserktime--;
		if (u.berserktime < 0) u.berserktime = 0;
		if (!(u.berserktime)) {
			make_confused(HConfusion + rnd(50), FALSE);
			set_itimeout(&HeavyConfusion, HConfusion);
			make_stunned(HStun + rnd(50), FALSE);
			set_itimeout(&HeavyStunned, HStun);
			pline("You stagger and the world spins as your berserk rage fades.");
		}
	}

	if (u.hanguppenalty > 0) {

		u.hanguppenalty--;

	}

	if (uarmf && itemhasappearance(uarmf, APP_EXPLOSIVE_BOOTS) && !rn2(10000) ) {
	      useup(uarmf);
		pline("KAABLAMM!!! Your explosive boots suddenly detonate!");
		explode(u.ux, u.uy, ZT_SPELL(ZT_FIRE), rnz(u.ulevel * 5), 0, EXPL_FIERY);
		losehp(rnz(u.ulevel * 5), "exploding TNT boots", KILLED_BY);
	    set_wounded_legs(LEFT_SIDE, HWounded_legs + 1000);
	    set_wounded_legs(RIGHT_SIDE, HWounded_legs + 1000);
	}

	if (uamul && uamul->oartifact == ART_DYNAMITUS && !rn2(2000) ) {
		pline("KAABLAMM!!! Your amulet causes an explosion!");
		explode(u.ux, u.uy, ZT_SPELL(ZT_FIRE), rnd(u.ulevel), 0, EXPL_FIERY);
		losehp(rnd(u.ulevel), "dynamite amulet explosion", KILLED_BY_AN);
	}

	if (uarmf && itemhasappearance(uarmf, APP_CASTLEVANIA_BOOTS) && !rn2(1000) && !(Race_if(PM_PLAYER_NIBELUNG) && rn2(5)) ) {
		pline("You hear a dark orchestral melody, and all the lights go out...");
		litroomlite(FALSE);
	}

	if (RngeCastlevania && !rn2(1000) && !(Race_if(PM_PLAYER_NIBELUNG) && rn2(5)) ) {
		pline("You hear a dark orchestral melody, and all the lights go out...");
		litroomlite(FALSE);
	}

	if (uarmh && (uarmh->oartifact == ART_DARK_NADIR) && !rn2(200) && !(Race_if(PM_PLAYER_NIBELUNG) && rn2(5)) ) {
		pline("Darkness surrounds you.");
		litroomlite(FALSE);
	}

	if (uarmh && (uarmh->oartifact == ART_RUTH_S_DARK_FORCE) && !rn2(200) && !(Race_if(PM_PLAYER_NIBELUNG) && rn2(5)) ) {
		pline("Darkness surrounds you.");
		litroomlite(FALSE);
	}

	if (uarmh && (uarmh->oartifact == ART_NADJA_S_DARKNESS_GENERATOR) && !rn2(200) && !(Race_if(PM_PLAYER_NIBELUNG) && rn2(5)) ) {
		pline("Darkness surrounds you.");
		litroomlite(FALSE);
	}

	if (Race_if(PM_WEAPON_TRAPPER)) { /* they know about the existence of traps --Amy */

	    struct trap *t;

	    for (t = ftrap; t != 0; t = t->ntrap) {
		if (t && !rn2(1000) && !t->tseen && (t->trapdiff < rnd(150)) && !t->hiddentrap) {
			t->tseen = 1;
			u.cnd_traprevealcount++;
			map_trap(t, TRUE);
		}
	    }

	}

	if (uarmh && itemhasappearance(uarmh, APP_RADIO_HELMET) ) {

	    struct trap *t;

	    for (t = ftrap; t != 0; t = t->ntrap) {
		if (t && !rn2(10000) && !t->tseen && (t->trapdiff < rnd(150)) && !t->hiddentrap) {
			t->tseen = 1;
			u.cnd_traprevealcount++;
			map_trap(t, TRUE);
		}
	    }

	}

	if (uarmh && uarmh->oartifact == ART_FUNCTIONAL_RADIO) {

	    struct trap *t;

	    for (t = ftrap; t != 0; t = t->ntrap) {
		if (t && !rn2(10000) && !t->tseen && (t->trapdiff < rnd(150)) && !t->hiddentrap) {
			t->tseen = 1;
			u.cnd_traprevealcount++;
			map_trap(t, TRUE);
		}
	    }

	}

	if (uarmh && uarmh->oartifact == ART____DOT__ALIEN_RADIO) {

	    struct trap *t;

	    for (t = ftrap; t != 0; t = t->ntrap) {
		if (t && !rn2(10000) && !t->tseen && (t->trapdiff < rnd(150)) && !t->hiddentrap) {
			t->tseen = 1;
			u.cnd_traprevealcount++;
			map_trap(t, TRUE);
		}
	    }

	}

	if (RngeRadioBroadcasts) {

	    struct trap *t;

	    for (t = ftrap; t != 0; t = t->ntrap) {
		if (t && !rn2(10000) && !t->tseen && (t->trapdiff < rnd(150)) && !t->hiddentrap) {
			t->tseen = 1;
			u.cnd_traprevealcount++;
			map_trap(t, TRUE);
		}
	    }

	}

	if (uarmh && uarmh->oartifact == ART_TARI_FEFALAS) {

	    struct trap *t;

	    for (t = ftrap; t != 0; t = t->ntrap) {
		if (t && !rn2(10000) && !t->tseen && (t->trapdiff < rnd(150)) && !t->hiddentrap) {
			t->tseen = 1;
			u.cnd_traprevealcount++;
			map_trap(t, TRUE);
		}
	    }

	}

	if (u.umoved && uarmf && uarmf->oartifact == ART_OUU_EECH && !rn2(100) && multi >= 0) {
		You("slip with your stiletto heels and crash into the floor. Ouch!");
		nomul(-(rnd(10) ), "prone from wiping out with their stilettos", TRUE);
		nomovemsg = "Perhaps you should... I dunno, don some shoes with which it's actually possible to walk?";

		if (!rn2(uarmh ? 50 : 10) && has_head(youmonst.data) && !Role_if(PM_COURIER) ) {

			if (rn2(50)) {
				adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
				if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
			} else {
				You_feel("dizzy!");
				forget(1 + rn2(5));
			}
		}

	}

	/* if you wear high heels without having the skill at all, bad stuff can happen --Amy */
	if (u.umoved && PlayerInHighHeels && (P_MAX_SKILL(P_HIGH_HEELS) == P_ISRESTRICTED)) {

		if (PlayerInStilettoHeels && !rn2(100) && multi >= 0) {
			You("slip with your stiletto heels and crash into the floor. Ouch!");
			nomul(-(rnd(10) ), "prone from wiping out with their stilettos", TRUE);
			nomovemsg = "You get back up and curse at your stiletto heels for making you wipe out.";

		    if (!rn2(uarmh ? 50 : 10) && has_head(youmonst.data) && !Role_if(PM_COURIER) ) {

			if (rn2(50)) {
				adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
				if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
			} else {
				You_feel("dizzy!");
				forget(1 + rn2(5));
			}
		    }

		}
		if (PlayerInConeHeels && !rn2(500) && !(uarmf && itemhasappearance(uarmf, APP_FEELGOOD_HEELS) ) ) {
			/* This is the one that will make players 'female dog' at me. Because it's evil. --Amy */

			register struct obj *otmpi, *otmpii;
			register int numdroppeditems = 0;

		    for (otmpi = invent; otmpi; otmpi = otmpii) {

		      otmpii = otmpi->nobj;

			if (!rn2(50) && !stack_too_big(otmpi) ) {

				if (otmpi->owornmask & W_ARMOR) continue;
				else if (otmpi->owornmask & W_AMUL) {
					if (otmpi->cursed) continue;
					else Amulet_off();
				} else if (otmpi->owornmask & W_IMPLANT) {
					if (otmpi->cursed) continue;
					else Implant_off();
				} else if (otmpi->owornmask & W_RING) {
					if (otmpi->cursed) continue;
					Ring_gone(otmpi);
				} else if (otmpi->owornmask & W_TOOL) continue;
				else if (otmpi->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
				    if (otmpi == uwep)
					uwepgone();
				    if (otmpi == uswapwep)
					uswapwepgone();
				    if (otmpi == uquiver)
					uqwepgone();
				}

				if (otmpi->owornmask & (W_BALL|W_CHAIN)) continue;
				else if (otmpi->owornmask) {
				/* catchall */
				    setnotworn(otmpi);
				}

				dropx(otmpi);
				numdroppeditems++;
			}
		    }
		    pline("Uh-oh, you trip with your cone heels%s", numdroppeditems ? " and some of your possessions fall out of your knapsack!" : "! But you barely manage to avoid dropping your possessions.");
		    if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		    if (numdroppeditems) scatter(u.ux,u.uy,10,VIS_EFFECTS|MAY_HIT|MAY_DESTROY|MAY_FRACTURE,0);

		}
		if (PlayerInBlockHeels && !rn2(250)) {
			pline("Being unused to wearing block heels, you painfully sprain your ankle.");
			set_wounded_legs(rn2(2) ? LEFT_SIDE : RIGHT_SIDE, HWounded_legs + rnd(20));

		}
		if (PlayerInWedgeHeels && !rn2(50) && multi >= 0) {
			You("trip over your own wedge heels.");
			nomul(-2, "stumbling with wedge heels", TRUE);
			nomovemsg = "";
		}

	}

	if (u.umoved && FemtrapActiveHenrietta && !rn2(1000)) {

				if (!(t_at(u.ux, u.uy)) ) {
					register struct trap *shittrap;
					shittrap = maketrap(u.ux, u.uy, SHIT_TRAP, 0, FALSE);
					if (shittrap && !(shittrap->hiddentrap)) {
						shittrap->tseen = 1;
					}
					if (shittrap) {
						pline("Oh no, someone opened the zippers of your boots again, causing you to slip...");
						dotrap(shittrap, RECURSIVETRAP);
					}
				}

			    slip_or_trip();

			    if (!rn2(uarmh ? 5000 : 1000) && has_head(youmonst.data) && !Role_if(PM_COURIER) ) {

				if (rn2(50)) {
					adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
					if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
				} else {
					You_feel("dizzy!");
					forget(1 + rn2(5));
				}
			    }

			    if (uarmf && uarmf->oartifact == ART_SUCH_A_SIGHER) {
					nomul(-10, "cleaning the dog shit from their shoes", TRUE);
					nomovemsg = "You finally cleaned the dog shit from your treaded soles.";
			    } else {
					nomul(-2, "fumbling", TRUE);
					nomovemsg = "";
			    }
			    /* The more you are carrying the more likely you
			     * are to make noise when you fumble.  Adjustments
			     * to this number must be thoroughly play tested.
			     */
			    if ((inv_weight() > -500)) {
				You("make a lot of noise!");
				wake_nearby();
			    }

	}

	if (u.umoved && (uarmf && itemhasappearance(uarmf, APP_IRREGULAR_BOOTS) ) && !rn2(100) && !(uarmf && !rn2(10) && itemhasappearance(uarmf, APP_BLUE_SNEAKERS) ) && (!(uarmf && uarmf->oartifact == ART_ELEVECULT) || !rn2(4)) && ((rnd(7) > P_SKILL(P_HIGH_HEELS)) || (PlayerCannotUseSkills) ) ) {
			    slip_or_trip();

			    if (!rn2(uarmh ? 5000 : 1000) && has_head(youmonst.data) && !Role_if(PM_COURIER) ) {

				if (rn2(50)) {
					adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
					if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
				} else {
					You_feel("dizzy!");
					forget(1 + rn2(5));
				}
			    }

			    nomul(-2, "fumbling", TRUE);
			    nomovemsg = "";
			    /* The more you are carrying the more likely you
			     * are to make noise when you fumble.  Adjustments
			     * to this number must be thoroughly play tested.
			     */
			    if ((inv_weight() > -500)) {
				You("make a lot of noise!");
				wake_nearby();
			    }

	}

	if (u.umoved && RngeIrregularity && !rn2(100) && !(uarmf && !rn2(10) && itemhasappearance(uarmf, APP_BLUE_SNEAKERS) ) && (!(uarmf && uarmf->oartifact == ART_ELEVECULT) || !rn2(4)) && ((rnd(7) > P_SKILL(P_HIGH_HEELS)) || (PlayerCannotUseSkills) ) ) {
			    slip_or_trip();

			    if (!rn2(uarmh ? 5000 : 1000) && has_head(youmonst.data) && !Role_if(PM_COURIER) ) {

				if (rn2(50)) {
					adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
					if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
				} else {
					You_feel("dizzy!");
					forget(1 + rn2(5));
				}
			    }

			    nomul(-2, "fumbling", TRUE);
			    nomovemsg = "";
			    /* The more you are carrying the more likely you
			     * are to make noise when you fumble.  Adjustments
			     * to this number must be thoroughly play tested.
			     */
			    if ((inv_weight() > -500)) {
				You("make a lot of noise!");
				wake_nearby();
			    }

	}

	if (u.umoved && (uarmh && uarmh->oartifact == ART_ELESSAR_ELENDIL) && !rn2(100) && !(uarmf && !rn2(10) && itemhasappearance(uarmf, APP_BLUE_SNEAKERS) ) ) {
			    slip_or_trip();

			    if (!rn2(uarmh ? 5000 : 1000) && has_head(youmonst.data) && !Role_if(PM_COURIER) ) {

				if (rn2(50)) {
					adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
					if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
				} else {
					You_feel("dizzy!");
					forget(1 + rn2(5));
				}
			    }

			    nomul(-2, "fumbling", TRUE);
			    nomovemsg = "";
			    /* The more you are carrying the more likely you
			     * are to make noise when you fumble.  Adjustments
			     * to this number must be thoroughly play tested.
			     */
			    if ((inv_weight() > -500)) {
				You("make a lot of noise!");
				wake_nearby();
			    }

	}

	if (u.umoved && evilfriday && !rn2(20) && (rnd(10) > ACURR(A_DEX)) && !(uarmf && !rn2(10) && itemhasappearance(uarmf, APP_BLUE_SNEAKERS) ) ) {
			    slip_or_trip();

			    if (!rn2(uarmh ? 5000 : 1000) && has_head(youmonst.data) && !Role_if(PM_COURIER) ) {

				if (rn2(50)) {
					adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
					if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
				} else {
					You_feel("dizzy!");
					forget(1 + rn2(5));
				}
			    }

			    nomul(-2, "fumbling", TRUE);
			    nomovemsg = "";
			    /* The more you are carrying the more likely you
			     * are to make noise when you fumble.  Adjustments
			     * to this number must be thoroughly play tested.
			     */
			    if ((inv_weight() > -500)) {
				You("make a lot of noise!");
				wake_nearby();
			    }

	}

	if (u.umoved && (uarmf && uarmf->oartifact == ART_UNEVEN_STILTS) && !rn2(100) && !(uarmf && !rn2(10) && itemhasappearance(uarmf, APP_BLUE_SNEAKERS) ) && (!(uarmf && uarmf->oartifact == ART_ELEVECULT) || !rn2(4)) && ((rnd(7) > P_SKILL(P_HIGH_HEELS)) || (PlayerCannotUseSkills) ) ) {
			    slip_or_trip();

			    if (!rn2(uarmh ? 5000 : 1000) && has_head(youmonst.data) && !Role_if(PM_COURIER) ) {

				if (rn2(50)) {
					adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
					if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
				} else {
					You_feel("dizzy!");
					forget(1 + rn2(5));
				}
			    }

			    nomul(-2, "fumbling", TRUE);
			    nomovemsg = "";
			    /* The more you are carrying the more likely you
			     * are to make noise when you fumble.  Adjustments
			     * to this number must be thoroughly play tested.
			     */
			    if ((inv_weight() > -500)) {
				You("make a lot of noise!");
				wake_nearby();
			    }

	}

	if (!u.persiantimer && uarmf && itemhasappearance(uarmf, APP_PERSIAN_BOOTS) && !rn2(1000) ) {

		pline("Your persian boots demand a sacrifice for allowing you to wear them.");
		pline("You allow them to scratch over the full length of your shins with their zippers.");
		if (Upolyd) {
			losehp( (u.mhmax / 2) + 2, "persian zipper boots", KILLED_BY);
			u.persiantimer = (u.mhmax / 2) + 2;
		}
		else {
			losehp( (u.uhpmax / 2) + 2, "persian zipper boots", KILLED_BY);
			u.persiantimer = (u.uhpmax / 2) + 2;
		}
		pline("Your %s are covered with deep wounds and you lose lots of %s!", makeplural(body_part(LEG)), body_part(BLOOD) );
		if (!rn2(5)) {
			u.uhpmax++;
			if (Upolyd) u.mhmax++;
			pline("But then the persian boots gently enclose your sweet %s again, and you feel much better.", makeplural(body_part(FOOT)));
		}

	}

	if (uarmf && itemhasappearance(uarmf, APP_VELCRO_BOOTS) && !rn2(1000) ) {
	    set_wounded_legs(LEFT_SIDE, HWounded_legs + rnz(50) );
	    set_wounded_legs(RIGHT_SIDE, HWounded_legs + rnz(50) );
		pline("Your velcro boots decide to scratch up and down your shins with their lash, opening terrible wounds.");
		losehp( rnz(u.ulevel + 2), "bloodthirsty velcro boots", KILLED_BY);

	}

	if (!rn2(200) && u.uprops[BAD_EFFECTS].extrinsic) badeffect();

	if (!rn2(200) && BadEffectProblem) badeffect();

	if (!rn2(200) && uwep && uwep->oartifact == ART_RANDOMISATOR) badeffect();

	if (!rn2(200) && u.twoweap && uswapwep && uswapwep->oartifact == ART_RANDOMISATOR) badeffect();

	if (!rn2(200) && uwep && uwep->oartifact == ART_MASTER_BALL) badeffect();

	if (!rn2(200) && u.twoweap && uswapwep && uswapwep->oartifact == ART_MASTER_BALL) badeffect();

	if (!rn2(200) && uarmu && uarmu->oartifact == ART_TRAP_DUNGEON_OF_SHAMBHALA) badeffect();

	if (!rn2(200) && have_badeffectstone() ) badeffect();

	if (!rn2(200) && uwep && uwep->oartifact == ART_WHAW_WHAW) badeffect();

	if (!rn2(200) && u.twoweap && uswapwep && uswapwep->oartifact == ART_WHAW_WHAW) badeffect();

	if (!rn2(200) && uarmf && uarmf->oartifact == ART_ELENA_S_CHALLENGE ) badeffect();

	if (!rn2(100) && u.uprops[RANDOM_RUMORS].extrinsic) {
		const char *line;
		char buflin[BUFSZ];
		if (rn2(2)) line = getrumor(-1, buflin, TRUE);
		else line = getrumor(0, buflin, TRUE);
		if (!*line) line = "Slash'EM rumors file closed for renovation.";
		pline("%s", line);
	}

	if (!rn2(100) && RngeRadioBroadcasts) {
		const char *line;
		char buflin[BUFSZ];
		if (rn2(2)) line = getrumor(-1, buflin, TRUE);
		else line = getrumor(0, buflin, TRUE);
		if (!*line) line = "Slash'EM rumors file closed for renovation.";
		pline("%s", line);
	}

	if (!rn2(100) && ublindf && ublindf->otyp == RADIOGLASSES) {
		const char *line;
		char buflin[BUFSZ];
		if (rn2(2)) line = getrumor(-1, buflin, TRUE);
		else line = getrumor(0, buflin, TRUE);
		if (!*line) line = "Slash'EM rumors file closed for renovation.";
		pline("%s", line);
	}

	if (!rn2(100) && uarmh && uarmh->oartifact == ART_TARI_FEFALAS) {
		const char *line;
		char buflin[BUFSZ];
		if (rn2(2)) line = getrumor(-1, buflin, TRUE);
		else line = getrumor(0, buflin, TRUE);
		if (!*line) line = "Slash'EM rumors file closed for renovation.";
		pline("%s", line);
	}

	if (!rn2(200) && u.uprops[AUTOMATIC_TRAP_CREATION].extrinsic) {

		makerandomtrap(FALSE);

	}

	if (!rn2(200) && TrapCreationProblem) {

		makerandomtrap(FALSE);

	}

	if (!rn2(200) && uarmu && uarmu->oartifact == ART_TIE_DYE_SHIRT_OF_SHAMBHALA) {

		makerandomtrap(FALSE);

	}

	if (!rn2(200) && uarmu && uarmu->oartifact == ART_TRAP_DUNGEON_OF_SHAMBHALA) {

		makerandomtrap(FALSE);

	}

	if (!rn2(200) && have_trapcreationstone() ) {

		makerandomtrap(FALSE);

	}

	if (!rn2(200) && uleft && uleft->oartifact == ART_HENRIETTA_S_MAGICAL_AID ) {

		makerandomtrap(FALSE);

	}

	if (!rn2(200) && uright && uright->oartifact == ART_HENRIETTA_S_MAGICAL_AID ) {

		makerandomtrap(FALSE);

	}

	if (!rn2(2000) && u.uprops[AUTOMATIC_TRAP_CREATION].extrinsic) {

		makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE);
		makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE);

	}

	if (!rn2(2000) && TrapCreationProblem) {

		makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE);
		makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE);

	}

	if (!rn2(2000) && uarmu && uarmu->oartifact == ART_TIE_DYE_SHIRT_OF_SHAMBHALA) {

		makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE);
		makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE);

	}

	if (!rn2(2000) && uarmu && uarmu->oartifact == ART_TRAP_DUNGEON_OF_SHAMBHALA) {

		makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE);
		makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE);

	}

	if (!rn2(2000) && have_trapcreationstone() ) {

		makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE);
		makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE);

	}

	if (!rn2(2000) && uleft && uleft->oartifact == ART_HENRIETTA_S_MAGICAL_AID ) {

		makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE);
		makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE);

	}

	if (!rn2(2000) && uright && uright->oartifact == ART_HENRIETTA_S_MAGICAL_AID ) {

		makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE);
		makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE); makerandomtrap(FALSE);

	}

	if (!rn2(1000) && u.uprops[SENTIENT_HIGH_HEELS].extrinsic) {

			switch (rnd(26)) {

				case 1:

				pline("Your footwear suddenly slams against your shins! It hurts!");
				losehp(rnd(10)+ rnd(monster_difficulty() + 1),"sentient high heel to the shins",KILLED_BY_AN);
				break;

				case 2:

				pline("Your footwear suddenly slams against your shins! You stagger...");
				make_stunned(HStun + rnd(10) + rnd(monster_difficulty() + 1), FALSE);
				losehp(1,"self-willed footwear to the shins",KILLED_BY_AN);
				break;

				case 3:

				pline("Your footwear suddenly slams against your %ss, opening your arteries and squirting %s everywhere!", body_part(HAND), body_part(BLOOD));
			      incr_itimeout(&Glib, rnd(10) + rnd(monster_difficulty() + 1));
				break;

				case 4:

				pline("Your footwear suddenly hits your %ss!", body_part(HAND) );
			      incr_itimeout(&Glib, 2);
				losehp(rnd(10)+ rnd(monster_difficulty() + 1),"sentient high heel to the hands",KILLED_BY_AN);
				break;

				case 5:

				pline("Your footwear suddenly slams against your shins!");
				losehp(rnd(10)+ rnd(monster_difficulty() + 1),"animated lady's shoe to the shins",KILLED_BY_AN);

				if (multi >= 0 && !rn2(2)) {
				    if (flags.female) {
					pline("It hurts a bit, but not too badly." );
				    } else if (Free_action) {
					pline("It hurts like hell, but you bear it like a man.");            
				    } else {
					pline("It hurts like hell! You pass out from the intense pain.");            
					nomovemsg = "You finally manage to get up again.";
					{
						int paralysistime = monster_difficulty() + 1;
						paralysistime /= 2;
						if (paralysistime <= 0) paralysistime = 1;
						if (paralysistime > 1) paralysistime = rnd(paralysistime);
						if (paralysistime > 10) {
							while (rn2(5) && (paralysistime > 10)) {
								paralysistime--;
							}
						}

						nomul(-(paralysistime), "knocked out by their own sentient footwear", TRUE);
						exercise(A_DEX, FALSE);
					}
				    }
				}

				break;

				case 6:

				pline("Your high-heeled footwear suddenly scratches up and down your %ss!", body_part(LEG) );

				losehp(rnd(10)+ rnd( (monster_difficulty() * 2) + 1),"sexy piece of sentient footwear",KILLED_BY_AN);

				if (u.legscratching <= 5)
			    	    pline("It stings a little.");
				else if (u.legscratching <= 10)
			    	    pline("It hurts quite a bit as some of your skin is scraped off!");
				else if (u.legscratching <= 20)
				    pline("Blood drips from your %s as the heel scratches over your open wounds!", body_part(LEG));
				else if (u.legscratching <= 40)
				    pline("You can feel the heel scratching on your shin bone! It hurts and bleeds a lot!");
				else
				    pline("You watch in shock as your blood is squirting everywhere, all the while feeling the razor-sharp high heel mercilessly opening your %ss!", body_part(LEG));

				losehp(u.legscratching, "terrible leg scratches", KILLED_BY);
				u.legscratching++;
				register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
				  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";
			    set_wounded_legs(side, HWounded_legs + rnd(60-ACURR(A_DEX)));
			    exercise(A_STR, FALSE);
			    exercise(A_DEX, FALSE);

				break;

				case 7:

				pline("Your stiletto heel suddenly kicks one of your sensitive body parts!" );

				losehp(rnd(10)+ rnd( (monster_difficulty() * 4) + 1),"sentient stiletto footwear",KILLED_BY_AN);
				if (!rn2(250)) pushplayer(TRUE);

				break;

				case 8:

				pline("Your %s is suddenly hit painfully by your own high-heeled shoe!", body_part(HEAD) );

				losehp(rnd(12)+ rnd( monster_difficulty() + 1),"sentient heeled shoe to the head",KILLED_BY_AN);

				break;

				case 9:

				pline("Klock! Your sexy high heel suddenly slams on your %s, producing a beautiful sound.", body_part(HEAD) );

				losehp(rnd(20)+ rnd( monster_difficulty() + 1),"self-willed high heel to the head",KILLED_BY_AN);

				break;

				case 10:

				pline("Suddenly, your footwear squeezes and stings your skin, and you notice their soles are covered with spikes!" );

				losehp(rnd(10)+ rnd( monster_difficulty() + 1),"sentient footwear with spikes",KILLED_BY_AN);
				    if (!rn2(6))
					poisoned("spikes", A_STR, "poisoned sentient footwear", 8);

				break;

				case 11:

				pline("Your high-heeled footwear painfully thunders on your %s!", body_part(HEAD) );

				losehp(rnd(4)+ rnd( monster_difficulty() + 1),"sentient high heel to the head",KILLED_BY_AN);

				break;

				case 12:

				pline("Your female footwear unyieldingly bonks your %s!", body_part(HEAD) );

				losehp(rnd(10)+ rnd( monster_difficulty() + 1),"sentient female footwear to the head",KILLED_BY_AN);

				break;

				case 13:

				pline("Your %s is hit hard by your own piece of footwear!", body_part(HEAD) );

				losehp(rnd(12)+ rnd( monster_difficulty() + 1),"female winter footwear to the head",KILLED_BY_AN);
				if (Upolyd) u.mhmax--; /* lose one hit point */
				else u.uhpmax--; /* lose one hit point */
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				if (u.mh > u.mhmax) u.mh = u.mhmax;

				break;

				case 14:

				pline("Your unyielding lady's shoe painfully lands on your %s!", body_part(HEAD) );

				losehp(rnd(15)+ rnd( monster_difficulty() + 1),"self-willed lady's shoe to the head",KILLED_BY_AN);

					if (multi >= 0 && !rn2(2)) {
					    if (Free_action) {
						pline("You struggle to stay on your %s.", makeplural(body_part(FOOT)));
					    } else {
						pline("You're knocked out and helplessly drop to the floor.");
						nomovemsg = 0;	/* default: "you can move again" */
						nomul(-rnd(5), "knocked out by a hard piece of female footwear", TRUE);
						exercise(A_DEX, FALSE);
						    }
						}

				break;

				case 15:

					if (Role_if(PM_COURIER)) pline("Your high-heeled footwear harmlessly scratches you.");
					else {pline("Your high-heeled footwear scratches your %s!", body_part(HEAD));

						if (!uarmh || uarmh->otyp != DUNCE_CAP) {

					    /* No such thing as mindless players... */
					    if (ABASE(A_INT) <= ATTRMIN(A_INT)) {
						int lifesaved = 0;
						struct obj *wore_amulet = uamul;
			
						while(1) {
						    /* avoid looping on "die(y/n)?" */
						    if (lifesaved && (discover || wizard)) {
							if (wore_amulet && !uamul) {
							    /* used up AMULET_OF_LIFE_SAVING; still
							       subject to dying from brainlessness */
							    wore_amulet = 0;
							} else if (wizard) {
							    /* explicitly chose not to die;
							       arbitrarily boost intelligence */
							    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
							    You_feel("like a scarecrow.");
							    break;
							}
						    }

							u.youaredead = 1;
							u.youarereallydead = 1;
						    if (lifesaved)
							pline("Unfortunately your brain is still gone.");
						    else
							Your("last thought fades away.");
						    killer = "brainlessness";
						    killer_format = KILLED_BY;
						    done(DIED);
						    if (wizard) {
							    u.youaredead = 0;
							    u.youarereallydead = 0;
						    }
						    lifesaved++;
						}
					    }
					}
					/* adjattrib gives dunce cap message when appropriate */
					if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
					else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE, TRUE);
					if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
					if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
					exercise(A_WIS, FALSE);

						}

				break;

				case 16:
					pline("Your block-heeled combat boot hits your %s with the massive heel!", body_part(HEAD));
					losehp(rnd(12) + rnd( monster_difficulty() + 1),"sentient block heel to the head",KILLED_BY);
					pline("You're seeing little asterisks everywhere.");
					make_confused(HConfusion + rnd(10) + rnd( monster_difficulty() + 1), FALSE);

				break;

				case 17:
					pline("Your very erotic inka boot hits you, and you have an orgasm.");
					if (practicantterror) {
						pline("%s rings out: 'Wanking off is not permitted in my laboratory, but you know that. 200 zorkmids.'", noroelaname());
						fineforpracticant(200, 0, 0);
					}
					badeffect();
					losehp(rnd(4) + rnd( monster_difficulty() + 1),"sentient inka boot to the head",KILLED_BY_AN);

				break;

				case 18:
					pline("AAAAAHHHHH! Your %s screams as your steel-capped sandal hits with the massive, unyielding metal heel!", body_part(HEAD));
					make_stunned(HStun + rnd(100) + rnd( (monster_difficulty() * 10) + 1), FALSE);
						if (!rn2(10)) {
							pline("You're knocked out and helplessly drop to the floor.");
							nomovemsg = 0;	/* default: "you can move again" */
							if (StrongFree_action) nomul(-rnd(2), "knocked out by a sentient steel-capped sandal", TRUE);
							else if (Free_action) nomul(-rnd(4), "knocked out by a sentient steel-capped sandal", TRUE);
							else nomul(-rnd(8), "knocked out by a sentient steel-capped sandal", TRUE);
						}
					losehp(rnd(20)+ rnd( (monster_difficulty() * 5) + 1),"sentient steel-capped sandal to the head",KILLED_BY_AN);

				break;

				case 19:
					pline("Your %s is bashed by your very fleecy block-heeled sandal!", body_part(HEAD));
					make_dimmed(HDimmed + rnd(50) + rnd( (monster_difficulty() * 2) + 1), TRUE);
					losehp(rnd(12) + rnd( (monster_difficulty() * 2) + 1),"a sentient block-heeled sandal to the head",KILLED_BY);

				break;

				case 20:
					pline("Ouch! Your soft lady shoe hits you, and despite being incredibly soft, hits your optical nerve!");
					make_blinded(Blinded + rnd(30) + rnd( (monster_difficulty() * 3) + 1),FALSE);
					losehp(rnd(6) + rnd( (monster_difficulty() * 2) + 1),"a sentient lady shoe to the head",KILLED_BY);

				break;

				case 21:
					pline("Ulch - your block-heeled boot hits your body, and you find out that the soles are immersed with dog shit! Apparently you stepped into a heap of shit without even noticing?");

					    register struct obj *objX, *objX2;
					    for (objX = invent; objX; objX = objX2) {
					      objX2 = objX->nobj;
						if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
						if (objX && !rn2(100)) wither_dmg(objX, xname(objX), 3, TRUE, &youmonst);

					    }
					losehp(rnd(5) + rnd( monster_difficulty() + 1),"a sentient dogshit boot",KILLED_BY);

				break;

				case 22:
					pline("Wow, your beautiful prostitute shoe slams on your %s with the massive heel! It's irresistible!", body_part(HEAD));
					losehp(rnd(10) + rnd( monster_difficulty() + 1),"a sentient prostitute shoe to the head",KILLED_BY);
					badeffect();

				break;

				case 23:
				{
					int randomkick = rnd(15);

					if (randomkick == 1) {
						pline("Your hippie heel steps on your %s with the plateau heels!", body_part(HAND));
						incr_itimeout(&Glib, 20 + monster_difficulty()); /* painfully jamming your fingers */
						losehp(rnd(10) + rnd( monster_difficulty() + 1),"being jammed by a plateau heel",KILLED_BY);
					}
					if (randomkick == 2) {
						pline("Your red whore boot jams your %ss!", body_part(TOE));

						register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
						const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";
				    		set_wounded_legs(side, HWounded_legs + rnd(60-ACURR(A_DEX)));
				    		exercise(A_STR, FALSE);
				    		exercise(A_DEX, FALSE);
						losehp(rnd(10) + rnd( monster_difficulty() + 1),"being jammed by a red whore boot",KILLED_BY);
					}

					if (randomkick == 3) {
						pline("Your prostitute shoe kicks right into your nuts!");
						losehp(rnd(10) + rnd( monster_difficulty() + 1),"being kicked in the nuts by a prostitute shoe",KILLED_BY);
						if (!flags.female) losehp(rnd(10) + rnd( monster_difficulty() + 1),"being kicked in the nuts by a prostitute shoe",KILLED_BY);
						else pline("But you don't actually have nuts. How the hell could this attack even affect you?");

					}

					if (randomkick == 4) {
						pline("Your erotic lady boot painfully stomps your body!");
						u.uhp -= 1;
						u.uhpmax -= 1;
						u.uen -= 1;
						u.uenmax -= 1;
						losehp(rnd(15) + rnd( (monster_difficulty() * 3) + 1),"being stomped by erotic lady boots",KILLED_BY);
					}

					if (randomkick == 5) {
						pline("Your incredibly erotic female shoe kicks you and looks so lovely that you fall in love with her, and are unable to fight back.");
						nomovemsg = "You finally decide to stop admiring the sexy leather boots.";
						nomul(-rnd(5), "mesmerized by a pair of sexy leather boots", TRUE);
						losehp(rnd(10) + rnd( monster_difficulty() + 1),"being stomped by erotic female shoes",KILLED_BY);

					}

					if (randomkick == 6) {
						pline("You're being kicked by your incredibly high-heeled platform boot. Think of the sweet red leather your sputa will flow down.");
						morehungry(1000);
					      make_vomiting(Vomiting+20, TRUE);
						if (Sick && Sick < 100)
						 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
						losehp(rnd(10) + rnd( monster_difficulty() + 1),"being kicked by incredibly high-heeled platform boots",KILLED_BY);
					}

					if (randomkick == 7) {
						pline("You decide to close your %ss for a while, thinking about your sexy red overknees and their lovely block heels while they kick you very painfully.", body_part(EYE));
						make_blinded(Blinded + (monster_difficulty() * 5), FALSE);
						losehp(rnd(10) + rnd( monster_difficulty() + 1),"being kicked by sexy red overknees",KILLED_BY);
					}

					if (randomkick == 8) {
						pline("Wow... your wonderful high heels are soooooo mesmerizing and beautiful while they kick you...");
						make_hallucinated(HHallucination + (monster_difficulty() * 5),FALSE,0L);
						losehp(rnd(10) + rnd( monster_difficulty() + 1),"being kicked by wonderful high heels",KILLED_BY);
					}

					if (randomkick == 9) {
						pline("You wonder where your plateau heels come from. As they kick you, your %s spins in bewilderment.", body_part(HEAD));
						make_confused(HConfusion + (monster_difficulty() * 5), FALSE);
						losehp(rnd(10) + rnd( monster_difficulty() + 1),"being kicked by plateau heels",KILLED_BY);
					}

					if (randomkick == 10) {
						pline("Argh! You got hit by your fleecy red combat boot, but the massive heel strained a vital muscle!");
						make_stunned(HStun + (monster_difficulty() * 5),FALSE);
						losehp(rnd(10) + rnd( monster_difficulty() + 1),"being kicked by massive heels",KILLED_BY);
					}

					if (randomkick == 11) {
						if (!rn2(25)) {
							pline("Fuck! You were hit by your own high heel, which was contaminated with spores! Why didn't you clean them before you put them on?");
							make_sick(rn1(25,25), "contaminated high heel", TRUE, SICK_VOMITABLE);
							losehp(rnd(10) + rnd( monster_difficulty() + 1),"being kicked by unclean high heels",KILLED_BY);
						}
						else {
							pline("You get kicked by your female boots, which are somehow rubbed with venom! How unfair! It certainly wasn't you who impregnated them, so they must have been like that already when you got them...");
							poisoned("block heel", A_STR, "poisoned block heel", 8);
							losehp(rnd(10) + rnd( monster_difficulty() + 1),"being kicked by venomous high heels",KILLED_BY);
						}
					}

					if (randomkick == 12) {
						pline("Ow, your slutty boots are stomping you repeatedly!");
						make_numbed(HNumbed + (monster_difficulty() * 5), FALSE);
						losehp(rnd(10) + rnd( monster_difficulty() + 1),"being stomped by slutty boots",KILLED_BY);
					}

					if (randomkick == 13) {
						pline("You are getting the creeps as your incredibly high heel kicks you.");
						make_frozen(HFrozen + (monster_difficulty() * 5),FALSE);
						losehp(rnd(10) + rnd( monster_difficulty() + 1),"being kicked by incredibly high heels",KILLED_BY);
						}

					if (randomkick == 14) {
						pline("Your block-heeled lady boot kicks you, and burns your skin!");
						make_burned(HBurned + (monster_difficulty() * 5),FALSE);
						losehp(rnd(10) + rnd( monster_difficulty() + 1),"being kicked by block-heeled lady boots",KILLED_BY);
					}

					if (randomkick == 15) {
						pline("You shudder in fear as your violent high-heeled plateau boot performs lethal kick attacks on you.");
						make_feared(HFeared + (monster_difficulty() * 5),FALSE);
						losehp(rnd(10) + rnd( monster_difficulty() + 1),"being kicked by a violent high-heeled plateau boot",KILLED_BY);
					}

				}
				break;

				case 24:
					pline("Your fleecy lady boot suddenly scratches over your %s with the lovely block heel!", body_part(LEG));
					playerbleed(monster_difficulty());
					losehp(rnd(10) + rnd( monster_difficulty() + 1),"a sentient sharp-edged lady boot",KILLED_BY);

				break;

				case 25:
					pline("Your razor-sharp stiletto sandal scratches very %sy wounds on your %s!", body_part(BLOOD), body_part(LEG));
					playerbleed(monster_difficulty());
					losehp(rnd(15) + rnd( (monster_difficulty() * 2) + 1),"a sentient sharp-edged stiletto sandal",KILLED_BY);

				break;

				case 26:
					pline("Your italian high heel scratches your %s very painfully!!", body_part(LEG));
					playerbleed(monster_difficulty());
					badeffect();
					losehp(rnd(20) + rnd( (monster_difficulty() * 3) + 1),"a sentient sharp-edged italian heel",KILLED_BY);

				break;

				default:
					impossible("nonexistant sentient high heels effect used");
				break;

			}

	}

	if (!rn2(250) && u.uprops[REPEATING_VULNERABILITY].extrinsic) {

		deacrandomintrinsic(rnz(200));
	}

	if (!rn2(250) && AutomaticVulnerabilitiy) {

		deacrandomintrinsic(rnz(200));
	}

	if (!rn2(250) && have_vulnerabilitystone() ) {

		deacrandomintrinsic(rnz(200));
	}

	if (u.uprops[TELEPORTING_ITEMS].extrinsic) {

		if (invent) {
		    for (otmpi = invent; otmpi; otmpi = otmpii) {
		      otmpii = otmpi->nobj;

			if (!rn2(10000) && (otmpi->otyp != AMULET_OF_ITEM_TELEPORTATION) && !(otmpi->oartifact == ART_SCHWUEU) && (otmpi->otyp != ITEM_TELEPORTING_STONE) && !(otmpi->oartifact == ART_SARAH_S_GRANNY_WEAR) && !(objects[otmpi->otyp].oc_material == MT_BONE && rn2(10)) && !stack_too_big(otmpi) ) {

				if (otmpi->owornmask & W_ARMOR) {
				    if (otmpi == uskin) {
					skinback(TRUE);		/* uarm = uskin; uskin = 0; */
				    }
				    if (otmpi == uarm) (void) Armor_off();
				    else if (otmpi == uarmc) (void) Cloak_off();
				    else if (otmpi == uarmf) (void) Boots_off();
				    else if (otmpi == uarmg) (void) Gloves_off();
				    else if (otmpi == uarmh) (void) Helmet_off();
				    else if (otmpi == uarms) (void) Shield_off();
				    else if (otmpi == uarmu) (void) Shirt_off();
				    /* catchall -- should never happen */
				    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
				} else if (otmpi ->owornmask & W_AMUL) {
				    Amulet_off();
				} else if (otmpi ->owornmask & W_IMPLANT) {
				    Implant_off();
				} else if (otmpi ->owornmask & W_RING) {
				    Ring_gone(otmpi);
				} else if (otmpi ->owornmask & W_TOOL) {
				    Blindf_off(otmpi);
				} else if (otmpi ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
				    if (otmpi == uwep)
					uwepgone();
				    if (otmpi == uswapwep)
					uswapwepgone();
				    if (otmpi == uquiver)
					uqwepgone();
				}

				if (otmpi->owornmask & (W_BALL|W_CHAIN)) {
				    unpunish();
				} else if (otmpi->owornmask) {
				/* catchall */
				    setnotworn(otmpi);
				}

				dropx(otmpi);
			      if (otmpi && otmpi->where == OBJ_FLOOR) rloco(otmpi);
				u.cnd_itemportcount++;
			}
		    }
		}

	}

	if (TeleportingItems) {

		if (invent) {
		    for (otmpi = invent; otmpi; otmpi = otmpii) {
		      otmpii = otmpi->nobj;

			if (!rn2(10000) && (otmpi->otyp != AMULET_OF_ITEM_TELEPORTATION) && !(otmpi->oartifact == ART_SCHWUEU) && (otmpi->otyp != ITEM_TELEPORTING_STONE) && !(otmpi->oartifact == ART_SARAH_S_GRANNY_WEAR) && !(objects[otmpi->otyp].oc_material == MT_BONE && rn2(10)) && !stack_too_big(otmpi) ) {

				if (otmpi->owornmask & W_ARMOR) {
				    if (otmpi == uskin) {
					skinback(TRUE);		/* uarm = uskin; uskin = 0; */
				    }
				    if (otmpi == uarm) (void) Armor_off();
				    else if (otmpi == uarmc) (void) Cloak_off();
				    else if (otmpi == uarmf) (void) Boots_off();
				    else if (otmpi == uarmg) (void) Gloves_off();
				    else if (otmpi == uarmh) (void) Helmet_off();
				    else if (otmpi == uarms) (void) Shield_off();
				    else if (otmpi == uarmu) (void) Shirt_off();
				    /* catchall -- should never happen */
				    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
				} else if (otmpi ->owornmask & W_AMUL) {
				    Amulet_off();
				} else if (otmpi ->owornmask & W_IMPLANT) {
				    Implant_off();
				} else if (otmpi ->owornmask & W_RING) {
				    Ring_gone(otmpi);
				} else if (otmpi ->owornmask & W_TOOL) {
				    Blindf_off(otmpi);
				} else if (otmpi ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
				    if (otmpi == uwep)
					uwepgone();
				    if (otmpi == uswapwep)
					uswapwepgone();
				    if (otmpi == uquiver)
					uqwepgone();
				}

				if (otmpi->owornmask & (W_BALL|W_CHAIN)) {
				    unpunish();
				} else if (otmpi->owornmask) {
				/* catchall */
				    setnotworn(otmpi);
				}

				dropx(otmpi);
			      if (otmpi && otmpi->where == OBJ_FLOOR) rloco(otmpi);
				u.cnd_itemportcount++;
			}
		    }
		}

	}

	if (have_itemportstone() ) {

		if (invent) {
		    for (otmpi = invent; otmpi; otmpi = otmpii) {
		      otmpii = otmpi->nobj;

			if (!rn2(10000) && (otmpi->otyp != AMULET_OF_ITEM_TELEPORTATION) && !(otmpi->oartifact == ART_SCHWUEU) && (otmpi->otyp != ITEM_TELEPORTING_STONE) && !(otmpi->oartifact == ART_SARAH_S_GRANNY_WEAR) && !(objects[otmpi->otyp].oc_material == MT_BONE && rn2(10)) && !stack_too_big(otmpi) ) {

				if (otmpi->owornmask & W_ARMOR) {
				    if (otmpi == uskin) {
					skinback(TRUE);		/* uarm = uskin; uskin = 0; */
				    }
				    if (otmpi == uarm) (void) Armor_off();
				    else if (otmpi == uarmc) (void) Cloak_off();
				    else if (otmpi == uarmf) (void) Boots_off();
				    else if (otmpi == uarmg) (void) Gloves_off();
				    else if (otmpi == uarmh) (void) Helmet_off();
				    else if (otmpi == uarms) (void) Shield_off();
				    else if (otmpi == uarmu) (void) Shirt_off();
				    /* catchall -- should never happen */
				    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
				} else if (otmpi ->owornmask & W_AMUL) {
				    Amulet_off();
				} else if (otmpi ->owornmask & W_IMPLANT) {
				    Implant_off();
				} else if (otmpi ->owornmask & W_RING) {
				    Ring_gone(otmpi);
				} else if (otmpi ->owornmask & W_TOOL) {
				    Blindf_off(otmpi);
				} else if (otmpi ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
				    if (otmpi == uwep)
					uwepgone();
				    if (otmpi == uswapwep)
					uswapwepgone();
				    if (otmpi == uquiver)
					uqwepgone();
				}

				if (otmpi->owornmask & (W_BALL|W_CHAIN)) {
				    unpunish();
				} else if (otmpi->owornmask) {
				/* catchall */
				    setnotworn(otmpi);
				}

				dropx(otmpi);
			      if (otmpi && otmpi->where == OBJ_FLOOR) rloco(otmpi);
				u.cnd_itemportcount++;
			}
		    }
		}

	}

	if (Race_if(PM_PLAYER_DYNAMO)) {

		if (invent) {
		    for (otmpi = invent; otmpi; otmpi = otmpii) {
		      otmpii = otmpi->nobj;

			if (!rn2(10000) && (otmpi->otyp != AMULET_OF_ITEM_TELEPORTATION) && !(otmpi->oartifact == ART_SCHWUEU) && (otmpi->otyp != ITEM_TELEPORTING_STONE) && !(otmpi->oartifact == ART_SARAH_S_GRANNY_WEAR) && !(objects[otmpi->otyp].oc_material == MT_BONE && rn2(10)) && !stack_too_big(otmpi) ) {

				if (otmpi->owornmask & W_ARMOR) {
				    if (otmpi == uskin) {
					skinback(TRUE);		/* uarm = uskin; uskin = 0; */
				    }
				    if (otmpi == uarm) (void) Armor_off();
				    else if (otmpi == uarmc) (void) Cloak_off();
				    else if (otmpi == uarmf) (void) Boots_off();
				    else if (otmpi == uarmg) (void) Gloves_off();
				    else if (otmpi == uarmh) (void) Helmet_off();
				    else if (otmpi == uarms) (void) Shield_off();
				    else if (otmpi == uarmu) (void) Shirt_off();
				    /* catchall -- should never happen */
				    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
				} else if (otmpi ->owornmask & W_AMUL) {
				    Amulet_off();
				} else if (otmpi ->owornmask & W_IMPLANT) {
				    Implant_off();
				} else if (otmpi ->owornmask & W_RING) {
				    Ring_gone(otmpi);
				} else if (otmpi ->owornmask & W_TOOL) {
				    Blindf_off(otmpi);
				} else if (otmpi ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
				    if (otmpi == uwep)
					uwepgone();
				    if (otmpi == uswapwep)
					uswapwepgone();
				    if (otmpi == uquiver)
					uqwepgone();
				}

				if (otmpi->owornmask & (W_BALL|W_CHAIN)) {
				    unpunish();
				} else if (otmpi->owornmask) {
				/* catchall */
				    setnotworn(otmpi);
				}

				dropx(otmpi);
			      if (otmpi && otmpi->where == OBJ_FLOOR) rloco(otmpi);
				u.cnd_itemportcount++;
			}
		    }
		}

	}

	if (uarmf && uarmf->oartifact == ART_SARAH_S_GRANNY_WEAR) {

		if (invent) {
		    for (otmpi = invent; otmpi; otmpi = otmpii) {
		      otmpii = otmpi->nobj;

			if (!rn2(10000) && (otmpi->otyp != AMULET_OF_ITEM_TELEPORTATION) && !(otmpi->oartifact == ART_SCHWUEU) && (otmpi->otyp != ITEM_TELEPORTING_STONE) && !(otmpi->oartifact == ART_SARAH_S_GRANNY_WEAR) && !(objects[otmpi->otyp].oc_material == MT_BONE && rn2(10)) && !stack_too_big(otmpi) ) {

				if (otmpi->owornmask & W_ARMOR) {
				    if (otmpi == uskin) {
					skinback(TRUE);		/* uarm = uskin; uskin = 0; */
				    }
				    if (otmpi == uarm) (void) Armor_off();
				    else if (otmpi == uarmc) (void) Cloak_off();
				    else if (otmpi == uarmf) (void) Boots_off();
				    else if (otmpi == uarmg) (void) Gloves_off();
				    else if (otmpi == uarmh) (void) Helmet_off();
				    else if (otmpi == uarms) (void) Shield_off();
				    else if (otmpi == uarmu) (void) Shirt_off();
				    /* catchall -- should never happen */
				    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
				} else if (otmpi ->owornmask & W_AMUL) {
				    Amulet_off();
				} else if (otmpi ->owornmask & W_IMPLANT) {
				    Implant_off();
				} else if (otmpi ->owornmask & W_RING) {
				    Ring_gone(otmpi);
				} else if (otmpi ->owornmask & W_TOOL) {
				    Blindf_off(otmpi);
				} else if (otmpi ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
				    if (otmpi == uwep)
					uwepgone();
				    if (otmpi == uswapwep)
					uswapwepgone();
				    if (otmpi == uquiver)
					uqwepgone();
				}

				if (otmpi->owornmask & (W_BALL|W_CHAIN)) {
				    unpunish();
				} else if (otmpi->owornmask) {
				/* catchall */
				    setnotworn(otmpi);
				}

				dropx(otmpi);
			      if (otmpi && otmpi->where == OBJ_FLOOR) rloco(otmpi);
				u.cnd_itemportcount++;
			}
		    }
		}

	}

	if (uamul && uamul->oartifact == ART_SCHWUEU ) {

		if (invent) {
		    for (otmpi = invent; otmpi; otmpi = otmpii) {
		      otmpii = otmpi->nobj;

			if (!rn2(10000) && (otmpi->otyp != AMULET_OF_ITEM_TELEPORTATION) && !(otmpi->oartifact == ART_SCHWUEU) && (otmpi->otyp != ITEM_TELEPORTING_STONE) && !(otmpi->oartifact == ART_SARAH_S_GRANNY_WEAR) && !(objects[otmpi->otyp].oc_material == MT_BONE && rn2(10)) && !stack_too_big(otmpi) ) {

				if (otmpi->owornmask & W_ARMOR) {
				    if (otmpi == uskin) {
					skinback(TRUE);		/* uarm = uskin; uskin = 0; */
				    }
				    if (otmpi == uarm) (void) Armor_off();
				    else if (otmpi == uarmc) (void) Cloak_off();
				    else if (otmpi == uarmf) (void) Boots_off();
				    else if (otmpi == uarmg) (void) Gloves_off();
				    else if (otmpi == uarmh) (void) Helmet_off();
				    else if (otmpi == uarms) (void) Shield_off();
				    else if (otmpi == uarmu) (void) Shirt_off();
				    /* catchall -- should never happen */
				    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
				} else if (otmpi ->owornmask & W_AMUL) {
				    Amulet_off();
				} else if (otmpi ->owornmask & W_IMPLANT) {
				    Implant_off();
				} else if (otmpi ->owornmask & W_RING) {
				    Ring_gone(otmpi);
				} else if (otmpi ->owornmask & W_TOOL) {
				    Blindf_off(otmpi);
				} else if (otmpi ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
				    if (otmpi == uwep)
					uwepgone();
				    if (otmpi == uswapwep)
					uswapwepgone();
				    if (otmpi == uquiver)
					uqwepgone();
				}

				if (otmpi->owornmask & (W_BALL|W_CHAIN)) {
				    unpunish();
				} else if (otmpi->owornmask) {
				/* catchall */
				    setnotworn(otmpi);
				}

				dropx(otmpi);
			      if (otmpi && otmpi->where == OBJ_FLOOR) rloco(otmpi);
				u.cnd_itemportcount++;
			}
		    }
		}

	}

	if (u.uprops[TRAP_REVEALING].extrinsic) {

	    struct trap *t;

	    for (t = ftrap; t != 0; t = t->ntrap) {
		if (t && !rn2(1000) && !t->tseen && (t->trapdiff < rnd(150)) && !t->hiddentrap) {
			t->tseen = 1;
			u.cnd_traprevealcount++;
			map_trap(t, TRUE);
		}
	    }

	}

	if (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == TRAP_REVEALING) ) {

	    struct trap *t;

	    for (t = ftrap; t != 0; t = t->ntrap) {
		if (t && !rn2(1000) && !t->tseen && (t->trapdiff < rnd(150)) && !t->hiddentrap) {
			t->tseen = 1;
			u.cnd_traprevealcount++;
			map_trap(t, TRUE);
		}
	    }

	}

	if (uarmg && uarmg->oartifact == ART_LINE_IN_THE_SAND) {

	    struct trap *t;

	    for (t = ftrap; t != 0; t = t->ntrap) {
		if (t && !rn2(1000) && !t->tseen && (t->trapdiff < rnd(150)) && !t->hiddentrap) {
			t->tseen = 1;
			u.cnd_traprevealcount++;
			map_trap(t, TRUE);
		}
	    }

	}

	if (uwep && uwep->oartifact == ART_MASAMUNE) {

	    struct trap *t;

	    for (t = ftrap; t != 0; t = t->ntrap) {
		if (t && !rn2(1000) && !t->tseen && (t->trapdiff < rnd(150)) && !t->hiddentrap) {
			t->tseen = 1;
			u.cnd_traprevealcount++;
			map_trap(t, TRUE);
		}
	    }

	}

	if (!rn2(1000) && Race_if(PM_WEAPON_TRAPPER)) { /* Harder than hard race that gets random nasty trap effects. --Amy */

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (!rn2(100) && uarmf && uarmf->oartifact == ART_PHANTO_S_RETARDEDNESS) {
		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		/* no warning message */

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (!rn2(5000) && Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (!rn2(1000) && u.uprops[NASTINESS_EFFECTS].extrinsic) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (!rn2(1000) && uimplant && uimplant->oartifact == ART_ETHERATORGARDEN) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (!rn2(1000) && uarmf && uarmf->oartifact == ART_NASTIST) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (!rn2(1000) && uarm && uarm->oartifact == ART_DON_SUICUNE_DOES_NOT_APPRO) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (!rn2(1000) && uwep && uwep->oartifact == ART_ARABELLA_S_MELEE_POWER) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (!rn2(1000) && u.twoweap && uswapwep && uswapwep->oartifact == ART_ARABELLA_S_MELEE_POWER) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (!rn2(1000) && uamul && uamul->oartifact == ART_ONE_MOMENT_IN_TIME) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (!rn2(1000) && uarm && uarm->oartifact == ART_DON_SUICUNE_USED_SELFDESTR) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (!rn2(1000) && NastinessProblem) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (!rn2(1000) && uarmg && uarmg->oartifact == ART_AA_S_CRASHING_TRAGEDY) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (!rn2(1000) && uamul && uamul->oartifact == ART_SATAN_S_FINAL_TRICK) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (!rn2(1000) && have_nastystone() ) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (!rn2(1000) && uarmf && uarmf->oartifact == ART_ELENA_S_CHALLENGE ) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

	}

	if (uarmc && itemhasappearance(uarmc, APP_MANTLE_OF_COAT) && !rn2(5000) ) {

		randomnastytrapeffect(200, 1000);

	}

	/* Max alignment record moved from align.h, so we can make it into a dynamic function --Amy */

	if (!AlignmentProblem && !u.uprops[ALIGNMENT_FAILURE].extrinsic && !have_alignmentstone() && !(uimplant && uimplant->oartifact == ART_SINFUL_REPENTER) && !rn2(Race_if(PM_UNALIGNMENT_THING) ? 50 : 200) && ((u.alignlim < 20) ? (TRUE) : (rnd(u.alignlim) < 20) ) )
		u.alignlim++;

	if ( (AlignmentProblem || u.uprops[ALIGNMENT_FAILURE].extrinsic || have_alignmentstone() || (uimplant && uimplant->oartifact == ART_SINFUL_REPENTER) ) && !rn2(Race_if(PM_UNALIGNMENT_THING) ? 50 : 200) ) {
		u.alignlim--;
		if(u.ualign.record > u.alignlim)
			u.ualign.record = u.alignlim;
	}

	/* Nymph race randomly gets punished --Amy */
	if (!rn2(2000) && Race_if(PM_NYMPH)) {
			punishx();
	}

	if (have_sleepstone() && !rn2(200)) {
	    fall_asleep(-rnd(10), TRUE);
		pline("You fall asleep.");
	}

	/* special bleeder handling --Amy */
	if (!rn2(500) && Role_if(PM_BLEEDER)) {
		You("are losing blood!");
		losehp(rnz(u.legscratching), "bleeding out", KILLED_BY);
	}
	if (!rn2(500) && Race_if(PM_HEMOPHAGE)) {
		You("are losing blood!");
		losehp(rnz(u.legscratching), "bleeding out", KILLED_BY);
	}
	if (!rn2(500) && u.uprops[BLOOD_LOSS].extrinsic) {
		You("are losing blood!");
		losehp(rnz(u.legscratching), "bleeding out", KILLED_BY);
	}
	if (!rn2(500) && BloodLossProblem) {
		You("are losing blood!");
		losehp(rnz(u.legscratching), "bleeding out", KILLED_BY);
	}
	if (!rn2(500) && have_bloodlossstone() ) {
		You("are losing blood!");
		losehp(rnz(u.legscratching), "bleeding out", KILLED_BY);
	}
	if (!rn2(500) && uarmf && uarmf->oartifact == ART_AMYBSOD_S_VAMPIRIC_SNEAKER) {
		You("are losing blood!");
		losehp(rnz(u.legscratching), "bleeding out", KILLED_BY);
	}
	if (!rn2(500) && uarmf && uarmf->oartifact == ART_AMYBSOD_S_NEW_FOOTWEAR) {
		You("are losing blood!");
		losehp(rnz(u.legscratching), "bleeding out", KILLED_BY);
	}
	if (!rn2(500) && uarmc && uarmc->oartifact == ART_TERRIFYING_LOSS) {
		You("are losing blood!");
		losehp(rnz(u.legscratching), "bleeding out", KILLED_BY);
	}
	if (!rn2(500) && uwep && uwep->oartifact == ART_SCALPEL_OF_THE_BLOODLETTER) {
		You("are losing blood!");
		losehp(rnz(u.legscratching), "bleeding out", KILLED_BY);
	}
	if (!rn2(500) && u.twoweap && uswapwep && uswapwep->oartifact == ART_SCALPEL_OF_THE_BLOODLETTER) {
		You("are losing blood!");
		losehp(rnz(u.legscratching), "bleeding out", KILLED_BY);
	}

	if (uarmc && itemhasappearance(uarmc, APP_DEADLY_CLOAK) && !rn2(1000) ) {
		pline("Your deadly cloak saps your life!");
		losehp(rnd(u.ulevel), "a deadly cloak", KILLED_BY);
	}
	if (RngeOccasionalDamage && !rn2(1000) ) {
		pline("Your life is sapped!");
		losehp(rnd(u.ulevel), "occasional damage", KILLED_BY);
	}

	if (uarmc && itemhasappearance(uarmc, APP_JARRING_CLOAK) && !rn2(1000) ) {
		wake_nearby();
		aggravate();
		pline("Your cloak emits a grating, annoying sound.");
	}

	if (RngeNoise && !rn2(1000)) {
		wake_nearby();
		aggravate();
		pline("You emit a grating, annoying sound.");
	}

	if (uarmf && uarmf->oartifact == ART_PORCELAIN_ELEPHANT && !rn2(100) ) {
		wake_nearby();
		aggravate();
		pline("Rattle/clink! Everyone in your vicinity woke up due to the loud sound.");
	}

	if ( (WeaknessProblem || u.uprops[WEAKNESS_PROBLEM].extrinsic || (uarmc && uarmc->oartifact == ART_FEMMY_FATALE) || (uarmf && uarmf->oartifact == ART_WEAK_FROM_HUNGER) || have_weaknessstone() ) && u.uhunger < 201) {

		if (!rn2(20)) {

			u.uhpmax -= 3;
			if (u.uhp > u.uhpmax) u.uhp -= 3;
			if (u.uhp < 1) {
				u.youaredead = 1;
				pline("You pass away due to being too weak.");
				killer_format = KILLED_BY_AN;
				killer = "fatal weakness";
				done(DIED);
				u.youaredead = 0;
			}

		} else {

			u.uhp -= 3;
			if (u.uhp < 1) {
				u.youaredead = 1;
				pline("You pass away due to being too weak.");
				killer_format = KILLED_BY_AN;
				killer = "fatal weakness";
				done(DIED);
				u.youaredead = 0;
			}

		}

	}

	if (uwep && uwep->oartifact == ART_TENSA_ZANGETSU) {
		u.uhp--;
		if (!rn2(2)) u.uhp--;
		if (u.uhp < 10) pline("Tensa Zangetsu causes you to feel weaker!");
		if (u.uhp < 3) pline("In fact, you feel almost faint...");
		if (u.uhp < 1) {
			u.youaredead = 1;
			pline("Tensa Zangetsu sapped your last bit of life and you die. Goodbye.");
			killer_format = KILLED_BY;
			killer = "wielding Tensa Zangetsu for too long";
			done(DIED);
			u.youaredead = 0;
		}
	}

	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_TENSA_ZANGETSU) {
		u.uhp--;
		if (!rn2(2)) u.uhp--;
		if (u.uhp < 10) pline("Tensa Zangetsu causes you to feel weaker!");
		if (u.uhp < 3) pline("In fact, you feel almost faint...");
		if (u.uhp < 1) {
			u.youaredead = 1;
			pline("Tensa Zangetsu sapped your last bit of life and you die. Goodbye.");
			killer_format = KILLED_BY;
			killer = "wielding Tensa Zangetsu for too long";
			done(DIED);
			u.youaredead = 0;
		}
	}

	if (uwep && uwep->oartifact == ART_ZANKAI_HUNG_ZE_TUNG_DO_HAI) {
		u.uhp--;
		if (!rn2(2)) u.uhp--;
		if (u.uhp < 10) pline("Die tumin! Die tumin all.");
		if (u.uhp < 3) pline("Bi sheng, pliya.");
		if (u.uhp < 1) {
			u.youaredead = 1;
			pline("Tschingtsching-tschingesswey!");
			killer_format = KILLED_BY;
			killer = "wielding a life-sapping artifact for too long";
			done(DIED);
			u.youaredead = 0;
		}
	}

	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ZANKAI_HUNG_ZE_TUNG_DO_HAI) {
		u.uhp--;
		if (!rn2(2)) u.uhp--;
		if (u.uhp < 10) pline("Die tumin! Die tumin all.");
		if (u.uhp < 3) pline("Bi sheng, pliya.");
		if (u.uhp < 1) {
			u.youaredead = 1;
			pline("Tschingtsching-tschingesswey!");
			killer_format = KILLED_BY;
			killer = "wielding a life-sapping artifact for too long";
			done(DIED);
			u.youaredead = 0;
		}
	}

	if (!rn2(2500) && Role_if(PM_BLEEDER)) {
		You("are losing lots of blood!");
		u.uhp -= 1;
		u.uhpmax -= 1;
		u.uen -= 1;
		u.uenmax -= 1;
		losehp(rnz(u.legscratching), "severe bleedout", KILLED_BY);
	}
	if (!rn2(2500) && Race_if(PM_HEMOPHAGE)) {
		You("are losing lots of blood!");
		u.uhp -= 1;
		u.uhpmax -= 1;
		u.uen -= 1;
		u.uenmax -= 1;
		losehp(rnz(u.legscratching), "severe bleedout", KILLED_BY);
	}

	if (!rn2(2500) && u.uprops[BLOOD_LOSS].extrinsic) {
		You("are losing lots of blood!");
		u.uhp -= 1;
		u.uhpmax -= 1;
		u.uen -= 1;
		u.uenmax -= 1;
		losehp(rnz(u.legscratching), "severe bleedout", KILLED_BY);
	}
	if (!rn2(2500) && BloodLossProblem) {
		You("are losing lots of blood!");
		u.uhp -= 1;
		u.uhpmax -= 1;
		u.uen -= 1;
		u.uenmax -= 1;
		losehp(rnz(u.legscratching), "severe bleedout", KILLED_BY);
	}
	if (!rn2(2500) && uwep && uwep->oartifact == ART_SCALPEL_OF_THE_BLOODLETTER) {
		You("are losing lots of blood!");
		u.uhp -= 1;
		u.uhpmax -= 1;
		u.uen -= 1;
		u.uenmax -= 1;
		losehp(rnz(u.legscratching), "severe bleedout", KILLED_BY);
	}
	if (!rn2(2500) && u.twoweap && uswapwep && uswapwep->oartifact == ART_SCALPEL_OF_THE_BLOODLETTER) {
		You("are losing lots of blood!");
		u.uhp -= 1;
		u.uhpmax -= 1;
		u.uen -= 1;
		u.uenmax -= 1;
		losehp(rnz(u.legscratching), "severe bleedout", KILLED_BY);
	}
	if (!rn2(2500) && have_bloodlossstone() ) {
		You("are losing lots of blood!");
		u.uhp -= 1;
		u.uhpmax -= 1;
		u.uen -= 1;
		u.uenmax -= 1;
		losehp(rnz(u.legscratching), "severe bleedout", KILLED_BY);
	}
	if (!rn2(2500) && uarmf && uarmf->oartifact == ART_AMYBSOD_S_VAMPIRIC_SNEAKER) {
		You("are losing lots of blood!");
		u.uhp -= 1;
		u.uhpmax -= 1;
		u.uen -= 1;
		u.uenmax -= 1;
		losehp(rnz(u.legscratching), "severe bleedout", KILLED_BY);
	}
	if (!rn2(2500) && uarmf && uarmf->oartifact == ART_AMYBSOD_S_NEW_FOOTWEAR) {
		You("are losing lots of blood!");
		u.uhp -= 1;
		u.uhpmax -= 1;
		u.uen -= 1;
		u.uenmax -= 1;
		losehp(rnz(u.legscratching), "severe bleedout", KILLED_BY);
	}
	if (!rn2(2500) && uarmc && uarmc->oartifact == ART_TERRIFYING_LOSS) {
		You("are losing lots of blood!");
		u.uhp -= 1;
		u.uhpmax -= 1;
		u.uen -= 1;
		u.uenmax -= 1;
		losehp(rnz(u.legscratching), "severe bleedout", KILLED_BY);
	}

	/* stone of magic resistance is teh uber, and means that if you wish for MR you'll never pick something else.
	 * So I decided to be mean (like always :P) and make it so that the stone curses itself after a while. --Amy */
	if (!rn2(2000)) {

		register struct obj *otmpX;
	
		for(otmpX = invent; otmpX; otmpX = otmpX->nobj) {
			if(otmpX->otyp == STONE_OF_MAGIC_RESISTANCE) {
				if (otmpX->blessed) unbless(otmpX);
				else curse(otmpX);
			}
		}

	}

	if (!rn2(7500) && Role_if(PM_BLEEDER)) {
		pline("Your scratching wounds are bleeding %s worse than before!", rn2(2) ? "even" : "much");
		u.legscratching++;
	}
	if (!rn2(7500) && Race_if(PM_HEMOPHAGE)) {
		pline("Your scratching wounds are bleeding %s worse than before!", rn2(2) ? "even" : "much");
		u.legscratching++;
	}

	if (!rn2(7500) && u.uprops[BLOOD_LOSS].extrinsic) {
		pline("Your scratching wounds are bleeding %s worse than before!", rn2(2) ? "even" : "much");
		u.legscratching++;
	}
	if (!rn2(7500) && BloodLossProblem) {
		pline("Your scratching wounds are bleeding %s worse than before!", rn2(2) ? "even" : "much");
		u.legscratching++;
	}
	if (!rn2(7500) && uwep && uwep->oartifact == ART_SCALPEL_OF_THE_BLOODLETTER) {
		pline("Your scratching wounds are bleeding %s worse than before!", rn2(2) ? "even" : "much");
		u.legscratching++;
	}
	if (!rn2(7500) && u.twoweap && uswapwep && uswapwep->oartifact == ART_SCALPEL_OF_THE_BLOODLETTER) {
		pline("Your scratching wounds are bleeding %s worse than before!", rn2(2) ? "even" : "much");
		u.legscratching++;
	}
	if (!rn2(7500) && have_bloodlossstone() ) {
		pline("Your scratching wounds are bleeding %s worse than before!", rn2(2) ? "even" : "much");
		u.legscratching++;
	}
	if (!rn2(7500) && uarmf && uarmf->oartifact == ART_AMYBSOD_S_VAMPIRIC_SNEAKER ) {
		pline("Your scratching wounds are bleeding %s worse than before!", rn2(2) ? "even" : "much");
		u.legscratching++;
	}
	if (!rn2(7500) && uarmf && uarmf->oartifact == ART_AMYBSOD_S_NEW_FOOTWEAR ) {
		pline("Your scratching wounds are bleeding %s worse than before!", rn2(2) ? "even" : "much");
		u.legscratching++;
	}
	if (!rn2(7500) && uarmc && uarmc->oartifact == ART_TERRIFYING_LOSS ) {
		pline("Your scratching wounds are bleeding %s worse than before!", rn2(2) ? "even" : "much");
		u.legscratching++;
	}

	if (u.uluck != baseluck && !rn2(((u.uhave.amulet && !u.freeplaymode && u.amuletcompletelyimbued) || u.ugangr) ? 300 : 600) ) {
	/* Cursed luckstones stop bad luck from timing out; blessed luckstones
	 * stop good luck from timing out; normal luckstones stop both;
	 * neither is stopped if you don't have a luckstone.
	 * Luck is based at 0 usually, +1 if a full moon and -1 on Friday 13th
	 */
	    register int time_luck = stone_luck(FALSE);
	    boolean nostone = !carrying(LUCKSTONE) && !stone_luck(TRUE);

	    if(u.uluck > baseluck && (nostone || time_luck < 0 || !rn2(10) )) /* now luck will also time out if you do have a luckstone; it just times out more slowly --Amy */
		u.uluck--;
	    else if(u.uluck < baseluck && (!isfriday || !rn2(2)) && (nostone || time_luck > 0 || !rn2(10) ))
		u.uluck++;
	}

	/* WAC -- check for timeout of specials */
	tech_timeout();

    if(Phasing) phasing_dialogue();
	if(u.uinvulnerable) return; /* things past this point could kill you */
	if(Stoned) stoned_dialogue();
	if(Slimed) slime_dialogue();
	if(Vomiting) vomiting_dialogue();
	if(Strangled) choke_dialogue();
	if (Sick && (moves % 7 == 0) ) {
		pline(Role_if(PM_PIRATE) ? "Ye still feel poxy." : Role_if(PM_KORSAIR) ? "Ye still feel poxy." : (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ? "Ye still feel poxy." : "You still feel deathly sick.");
		if (Sickopathy) pline("You have %ld turns to live.", Sick);
	}
	if(u.mtimedone && !--u.mtimedone) {
		if (!Race_if(PM_UNGENOMOLD) && u.polyformed) rehumanize();
		/* Amy evilness: if you're polymorphed into something very fast, you may occasionally be forced out of your
		 * polymorph form even with unchanging, because otherwise it could get real unbalanced.
		 * Also, if you do stay in the form, you only get 100 more turns so you can't get an extremely long polymorph
		 * duration via unchanging if it's a form that normally doesn't last that long */
		else if ( (Unchanging || (uarmc && uarmc->oartifact == ART_PERMANENTITIS) || (uarmc && uarmc->oartifact == ART_SHAPETAKE_NUMBER_FIVE) && ((youmonst.data->mmove < 18) || (rnd(youmonst.data->mmove) < 18) ) ) || Race_if(PM_UNGENOMOLD) )
			u.mtimedone = 100;
		else
			rehumanize();
	}
	if (u.mtimedone < 0) u.mtimedone = rnd(5); /* fail safe --Amy */
	if(u.ucreamed) u.ucreamed--;

	/* Dissipate spell-based protection. */
	if (u.usptime) {
	    if (--u.usptime == 0 && u.uspellprot) {
		u.usptime = u.uspmtime;
		u.uspellprot--;
		find_ac();
		if (!Blind)
		    Norep("The %s haze around you %s.", hcolor(NH_GOLDEN),
			  u.uspellprot ? "becomes less dense" : "disappears");
	    }
	}

	if (u.ugallop) {
	    if (--u.ugallop == 0L && u.usteed)
	    	pline("%s stops galloping.", Monnam(u.usteed));
	}

	for(upp = u.uprops; upp < u.uprops+SIZE(u.uprops); upp++)
	    if((upp->intrinsic & TIMEOUT) && !(--upp->intrinsic & TIMEOUT)) {
		switch(upp - u.uprops){
		case STONED:
			u.youaredead = 1;
			if (delayed_killer && !killer) {
				killer = delayed_killer;
				delayed_killer = 0;
			}
			if (!killer) {
				/* leaving killer_format would make it
				   "petrified by petrification" */
				killer_format = NO_KILLER_PREFIX;
				killer = "killed by petrification";
			}
			done(STONING);
			u.youaredead = 0;
			break;
		case SLIMED:
			if (Polymorph_control) {
				greenslimetransformation();
				pline("You permanently became a green slime.");
				if (Upolyd) rehumanize();
				break;
			}
			u.youaredead = 1;

			if (delayed_killer && !killer) {
				killer = delayed_killer;
				delayed_killer = 0;
			}
			if (!killer) {
				killer_format = NO_KILLER_PREFIX;
				killer = "turned into green slime";
			}
			done(TURNED_SLIME);
			u.youaredead = 0;
			break;
		case VOMITING:
			make_vomiting(0L, TRUE);
			break;
		case MC_REDUCTION:
			pline("Your magic cancellation seems to work correctly again.");
			break;

		 case FEMTRAP_FEMMY:

			pline("The dungeon no longer seems feminine.");

		 break;

		 case FEMTRAP_MADELEINE:

			pline("Your shins are no longer super sensitive to being kicked by women.");

		 break;

		 case FEMTRAP_MARLENA:

			pline("The dungeon is less green now.");

		 break;

		 case FEMTRAP_KRISTIN:

			pline("The women got bored of the multitude of high heels.");

		 break;

		 case FEMTRAP_ANNA:

			pline("There's an announcement saying that the hussies no longer consider you a target. Phew.");

		 break;

		 case FEMTRAP_RUEA:

			pline("The women gave up their attempts to convert you.");

		 break;

		 case FEMTRAP_DORA:

			pline("There's a loud flapping noise indicating that the evil shoe-hating birds are leaving the dungeon.");

		 break;

		 case FEMTRAP_MARIKE:

			pline("Aww, apparently you no longer want to listen to squeaking farting noises. But why? You didn't even come yet...");

		 break;

		 case FEMTRAP_JETTE:

			pline("The power of feminism seems to be decreasing. Don't feel safe just yet, though.");

		 break;

		 case FEMTRAP_INA:

			pline("Since you didn't vomit for a week, your anorexia condition disappears. But here's a newsflash: you could have instantly healed the condition by simply dying (don't try that at home).");

		 break;

		 case FEMTRAP_SING:

			pline("You got enough and decide to end the evil woman's slavery reign by bashing her face with a platform boot that has cow dung on the soles. After striking her down, you flee from your prison and don't look back.");

		 break;

		 case FEMTRAP_VICTORIA:

			pline("You finished playing with the karate women.");

		 break;

		 case FEMTRAP_MELISSA:

			pline("It dawns on you that being seduced by women isn't a good thing, because they might steal your stuff.");

		 break;

		case FEMTRAP_ANNEMARIE:
			pline("The women stopped their athletic exercises.");
			break;
		case FEMTRAP_JIL:
			pline("Ah, good, the girls decided to put their sweaty socks in the washing machine at last. About time, too.");
			break;
		case FEMTRAP_JANA:
			pline("All the grave walls are set up now. But somehow you feel that they behave normally now.");
			break;
		case FEMTRAP_KATRIN:
			pline("It seems that no more chewing gum traps are being set up, although you still need to be wary of the existing ones.");
			break;
		case FEMTRAP_GUDRUN:
			pline("You hear Gudrun announce that the challenge has ended, but maybe she's still around somewhere? Gotta see if you can catch her!");
			break;
		case FEMTRAP_ELLA:
			pline("Ella announces that her challenge is over. Quick, see if she's still trying to escape, you could attack her from behind!");
			break;
		case FEMTRAP_MANUELA:
			pline("Apparently you said something that the women consider offensive, and their block-heeled combat boots are retreating as a result.");
			break;
		case FEMTRAP_JENNIFER:
			pline("You've realized that no more silent farters are coming.");
			break;
		case FEMTRAP_PATRICIA:
			pline("Aww, you're no longer the thick girl with the sexy steel-capped sandals...");
			break;
		case FEMTRAP_ANTJE:
			pline("Careful, the storm is coming! The sheep ain't got no coil anymore!");
			break;
		case FEMTRAP_ANTJE_X:
			pline("Maybe, it dawns on you, it's better to use the toilet at your own home after all...");
			break;
		case FEMTRAP_KERSTIN:
			pline("Finally those pesky farmer girls are leaving, they were really getting on your nerves.");
			break;
		case FEMTRAP_LAURA:
			pline("You got really sick of fighting female clothing all the time, hopefully there'll be some actual monsters for a while now...");
			break;
		case FEMTRAP_LARISSA:
			pline("Blegh, the asshole tricked you! It seems that you stepped into dog shit for nothing at all, and now you can waste all your free time cleaning your shoes again! If you get that guy into your fingers...");
			break;
		case FEMTRAP_NORA:
			pline("Man, now you're REALLY hungry. You feel like eating a bear whole to fill the huge hole in your %s.", body_part(STOMACH));
			break;
		case FEMTRAP_NATALIA:
			if (flags.female) pline("Oh good, your menstruational period seems to have come to an end. Hopefully it won't make a return.");
			else pline("The russian girl is done punishing you with her heel sandals.");
			break;
		case FEMTRAP_SUSANNE:
			pline("Well, the powerful women haven't killed you yet, so they decide to spawn no longer.");
			break;
		case FEMTRAP_LISA:
			pline("Bah, underwear is actually boring, why on earth did you waste your time with that?");
			break;
		case FEMTRAP_BRIDGHITTE:
			pline("Now be careful, for even though you might have been lucky when you crossed the bridge, you might slip up on the way back...");
			break;
		case FEMTRAP_JULIA:
			pline("That special shield the women had, it was so turbo annoying. Good thing it's no longer active now.");
			break;
		case FEMTRAP_NICOLE:
			pline("Aww, you'd have loved to figure out what that dark aura surrounding the women is, but it's gone...");
			break;
		case FEMTRAP_RITA:
			pline("That bullshit effect where monsters could trigger feminism trap effects and cause you to be affected by them, it has ended. Took way too long. You're really angry at the sadistic programmer bitch who designed it like that.");
			break;

		 case FEMTRAP_ANITA:

			pline("You survived the dangerous high heels... for now.");

		 break;

		 case FEMTRAP_HENRIETTA:

			pline("At last, you wised up and decided to switch to a pair of boots without zippers.");

		 break;

		 case FEMTRAP_VERENA:

			pline("Now you're so annoyed that you slap that annoying blonde girl right in the face, knocking her out. Maybe now she'll leave you alone, she already caused enough annoyance anyway.");

		 break;

		 case FEMTRAP_ARABELLA:

			pline("You're relieved to have gotten over the nastiness. Beware, there may still be some traps around.");

		 break;

		 case FEMTRAP_SARAH:

			pline("Apparently the farting gas is depleted.");

		 break;

		 case FEMTRAP_CLAUDIA:

			pline("Your sexy butt cheek wood confusion ends.");

		 break;

		 case FEMTRAP_LUDGERA:

			pline("At last the disgusting toilet noises ceased.");

		 break;

		 case FEMTRAP_KATI:

			pline("You vow to never clean a girl's shoes again.");

		 break;

		 case FEMTRAP_ANASTASIA:

			pline("You come back to your senses and realize that stepping into a heap of shit is, well, shit.");

		 break;

		 case FEMTRAP_JESSICA:

			pline("You no longer have super tender butt cheeks, or a super tender body for that matter.");

		 break;

		 case FEMTRAP_SOLVEJG:

			pline("At last, your voice is normal again, although you're also considerably less sexy than you used to be.");

		 break;

		 case FEMTRAP_WENDY:

			pline("You survived the wrath of the women, and feel safe from them now.");

		 break;

		 case FEMTRAP_KATHARINA:

			pline("The female dungeon inhabitants decide to make their butts Geneva-compliant again.");

		 break;

		 case FEMTRAP_ELENA:

			pline("Your lust for pretty women fades. Aww...");

		 break;

		 case FEMTRAP_THAI:

			pline("You no longer need to constantly go to the toilet, and also your physique seems normal now.");

		 break;

		 case FEMTRAP_ELIF:

			pline("The lovely ghostly girl named Elif disappears, telling you that she enjoyed the time she spent with you.");

		 break;

		 case FEMTRAP_NADJA:

			pline("The women are no longer angry at you, and you may hit them again. Don't get it wrong though - I didn't say that you *should*, just that you're no longer hit with retribution if you do - they will still fight back!");

		 break;

		 case FEMTRAP_SANDRA:

			pline("Somehow, you're still alive. Your legs are now safe from any imaginary winter stilettos.");

		 break;

		 case FEMTRAP_NATALJE:

			pline("After dancing for a long time, you are exhausted. And thus, at last you may rest again. But your sexiness and all the other benefits fade.");

		 break;

		 case FEMTRAP_JEANETTA:

			pline("Jeanetta is done collecting your skin shreds. Better don't visit her trophy hall unless you got a strong stomach.");

		 break;

		 case FEMTRAP_YVONNE:

			pline("There are no more toilets being built specifically for you.");

		 break;

		 case FEMTRAP_MAURAH:

			pline("Your butt is no longer as sexy as before, and will no longer fart unless you tell it to do so.");

		 break;

		 case FEMTRAP_MELTEM:

			pline("The girls exhausted their farting gas.");

		 break;

		 case FEMTRAP_NELLY:

			pline("Good thing you survived the dangerous female hug attacks.");

		 break;

		 case FEMTRAP_EVELINE:

			pline("Apparently the women stopped running.");

		 break;

		 case FEMTRAP_KARIN:

			pline("Your nuts no longer have to fear getting kicked by female knees constantly.");

		 break;

		 case FEMTRAP_JUEN:

			pline("You finally took a trick from your emergency bag of tricks to heal your almost broken shins.");

		 break;

		 case FEMTRAP_KRISTINA:

			pline("Finally the damn cigarettes got extinguished.");

		 break;

		 case FEMTRAP_LOU:

			pline("You swear that you'll kill the next asshole who brushes your cloak with their dirty shoes.");

		 break;

		 case FEMTRAP_ALMUT:

			pline("Apparently you managed to save yourself from the bloodthirsty girl turn shoes.");

		 break;

		 case FEMTRAP_JULIETTA:

			pline("It seems that Julietta finally got bored and decides to annoy other people instead of you.");

		 break;

		case SICK:

			if (Role_if(PM_COOK) && rn2(2)) {
			u.usick_type = 0;
			pline("You survived the poisoning!");
			break;
			}
			u.youaredead = 1;

			You("die from your illness.");
			killer_format = KILLED_BY_AN;
			killer = u.usick_cause;
			if ((m_idx = name_to_mon(killer)) >= LOW_PM) {
			    if (type_is_pname(&mons[m_idx])) {
				killer_format = KILLED_BY;
			    } else if (mons[m_idx].geno & G_UNIQ) {
				killer = the(killer);
				strcpy(u.usick_cause, killer);
				killer_format = KILLED_BY;
			    }
			}
			u.usick_type = 0;
			done(POISONING);
			u.youaredead = 0;
			break;
		case FAST:
			if (!Very_fast)
				You_feel("yourself slowing down%s.",
							Fast ? " a bit" : "");
			break;
		case FIRE_RES:
			if (!Fire_resistance)
				You_feel("a little warmer.");
			break;
		case FEAR_RES:
			if (!Fear_resistance)
				You_feel("less resistant to fear.");
			break;
		case COLD_RES:
			if (!Cold_resistance)
				You_feel("a little cooler.");
			break;
		case SLEEP_RES:
			if (!Sleep_resistance)
				You_feel("a little sleepy.");
			break;
		case KEEN_MEMORY:
			if (!Keen_memory)
				You_feel("less able to memorize things.");
			break;
		case SIGHT_BONUS:
			if (!Sight_bonus)
				You("notice your surroundings darkening a bit.");
			vision_full_recalc = 1;
			break;
		case SHOCK_RES:
			if (!Shock_resistance)
				You_feel("a little static cling.");
			break;
		case ANTIMAGIC:
			if (!Antimagic)
				You_feel("vulnerable to magic.");
			break;
		case DRAIN_RES:
			if (!Drain_resistance)
				You_feel("less resistant to drain life.");
			break;
		case MAGICAL_BREATHING:
			if (!Amphibious)
				You("need to breathe again.");
			break;
		case SWIMMING:
			if (!Swimming)
				pline("Your water wings disappear.");
			break;
		case AGGRAVATE_MONSTER:
			if (!Aggravate_monster)
				You_feel("less aggravated.");
			break;
		case POISON_RES:
			if (!Poison_resistance)
				You_feel("a little less healthy.");
			break;
		case DISINT_RES:
			if (!Disint_resistance)
				You_feel("a little less firm.");
			break;
		case ACID_RES:
			if (!Acid_resistance)
				You_feel("more vulnerable to acid.");
			break;
		case STONE_RES:
			if (!Stone_resistance)
				You_feel("a little less limber.");
			break;
		case HALF_PHDAM:
			if (!Half_physical_damage)
				You_feel("less resistant to damage.");
			break;
		case ASTRAL_VISION:
			if (!Astral_vision)
				You_feel("unable to keep using your x-ray vision.");
			break;
		case BLIND_RES:
			if (!Blind_resistance)
				You_feel("less resistant to blindness.");
			break;
		case HALLUC_RES:
			if (!Halluc_resistance)
				You_feel("less resistant to hallucinations.");
			break;
		case HUNGER:
			if (!Hunger)
				You_feel("less hungry.");
			break;
		case HALF_SPDAM:
			if (!Half_spell_damage)
				You_feel("less resistant to spells.");
			break;
		case SICK_RES:
			if (!Sick_resistance)
				You_feel("worried about getting sick.");
			break;
		case DISPLACED:
			if (!Displaced)
				You_feel("more exposed.");
			break;
		case TELEPORT:
			if (!Teleportation)
				You_feel("a little less jumpy.");
			break;
		case TELEPORT_CONTROL:
			if (!Teleport_control)
				You_feel("a little less in control of yourself.");
			break;
		case POLYMORPH:
			if (!Polymorph)
				You_feel("a little less unstable.");
			break;
		case POLYMORPH_CONTROL:
			if (!Polymorph_control)
				You_feel("a little less in control of your form.");
			break;
		case TELEPAT:
			if (!HTelepat)
				You_feel("a little less mentally acute.");
			break;
		case SEARCHING:
			if (!HSearching)
				pline("You are no longer searching for things.");
			break;
		case INFRAVISION:
			if (!Infravision)
				pline("Your %s are no longer sensitive to infrared radiation.", makeplural(body_part(EYE)));
			break;
		case WARNING:
			if (!HWarning)
				pline("Your radar goes out.");
			break;
		case REGENERATION:
			if (!HRegeneration)
				pline("You stop regenerating rapidly.");
			break;
		case CONFLICT:
			if (!HConflict)
				pline("You stop generating conflict.");
			break;
		case PREMDEATH:
			if (!HPrem_death)
				pline("You no longer have to fear that you might die prematurely.");
			break;
		case MAP_AMNESIA:
			if (!HMap_amnesia)
				pline("Your memory seems to be coming back.");
			break;
		case STEALTH:
			if (!HStealth)
				pline("Your movements are a little less silent.");
			break;
		case FREE_ACTION:
			if (!Free_action)
				You_feel("a little stiffer.");
			break;
		case PASSES_WALLS:
			if (!Passes_walls)
				You_feel("a little more solid.");
			break;
		case INVULNERABLE:
			if (!Invulnerable)
				You("are no longer invulnerable.");
			break;
		case GLIB:
			if (!IsGlib)
				pline("Your %s are no longer slippery.", makeplural(body_part(HAND)) );
			break;
		case REFLECTING:
			if (!Blind)
				pline("The shimmering globe around you flickers and vanishes.");
			else
				pline("You don't feel very smooth anymore.");
			break;
		case CONFUSION:
			HConfusion = 1; /* So make_confused works properly */
			make_confused(0L, TRUE);
			stop_occupation();
			break;
		case STUNNED:
			HStun = 1;
			make_stunned(0L, TRUE);
			stop_occupation();
			break;
		case NUMBED:
			HNumbed = 1;
			make_numbed(0L, TRUE);
			stop_occupation();
			break;
		case FEARED:
			HFeared = 1;
			make_feared(0L, TRUE);
			stop_occupation();
			break;
		case FROZEN:
			HFrozen = 1;
			make_frozen(0L, TRUE);
			stop_occupation();
			break;
		case BURNED:
			HBurned = 1;
			make_burned(0L, TRUE);
			stop_occupation();
			break;
		case DIMMED:
			HDimmed = 1;
			make_dimmed(0L, TRUE);
			stop_occupation();
			break;
		case BLINDED:
			Blinded = 1;
			make_blinded(0L, TRUE);
			stop_occupation();
			break;
		case INVIS:
			newsym(u.ux,u.uy);
			if (!Invis && !BInvis && !Blind) {
			    You(!See_invisible ?
				    "are no longer invisible." :
				    "can no longer see through yourself.");
			    stop_occupation();
			}
			break;
		case SEE_INVIS:
			set_mimic_blocking(); /* do special mimic handling */
			see_monsters();		/* make invis mons appear */
			newsym(u.ux,u.uy);	/* make self appear */
			stop_occupation();
			break;
		case WOUNDED_LEGS:
			heal_legs();
			stop_occupation();
			break;
		case HALLUC:
			HHallucination = 1;
			(void) make_hallucinated(0L, TRUE, 0L);
			stop_occupation();
			break;
		case SLEEPING:
			if (unconscious() || Sleep_resistance)
				HSleeping += rnd(1000);
			else if (Sleeping) {
				if (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ) pline("Ye take a caulk.");
				else You("fall asleep.");
				sleeptime = rnd(20);
				fall_asleep(-sleeptime, TRUE);
				HSleeping += sleeptime + rnd(1000);
			}
			break;
		case LEVITATION:
			(void) float_down(I_SPECIAL|TIMEOUT, 0L);
			break;
		case FLYING:
			if (!Flying)
				You_feel("less airborne.");
			break;
		case CONF_RES:
			if (!Conf_resist)
				pline("You no longer resist confusion.");
			break;
		case CONT_RES:
			if (!Cont_resist)
				pline("You no longer resist contamination.");
			break;
		case DISCOUNT_ACTION:
			if (!Discount_action)
				pline("You no longer resist paralysis.");
			break;
		case FULL_NUTRIENT:
			if (!Full_nutrient)
				pline("Your nutrition consumption speeds up again.");
			break;
		case TECHNICALITY:
			if (!Technicality)
				pline("Your techniques are weaker again.");
			break;
		case SCENT_VIEW:
			if (!ScentView)
				pline("Your %s returns to normal.", body_part(NOSE));
			break;
		case DIMINISHED_BLEEDING:
			if (!DiminishedBleeding)
				pline("Your %s coagulation factor is no longer active.", body_part(BLOOD));
			break;
		case CONTROL_MAGIC:
			if (!ControlMagic)
				pline("You're no longer controlling your magic.");
			break;
		case EXP_BOOST:
			if (!ExpBoost)
				pline("Your experience shrine effect has ended.");
			break;
		case PLAYERBLEEDING:
			if (!PlayerBleeds)
				pline("Your bleeding stops.");
			stop_occupation();
			flags.botl = TRUE;
			break;
		case BLACK_NG_WALLS:

			pline(FunnyHallu ? "Rien ne va plus... You seem to remember this slogan being printed on all official Pokemon games' box covers. It's like 'Rien ne va plus' is the official Pokemon slogan!" : "Rien ne va plus...");
			/* Of course it's actually the Roulette slogan. --Amy */

			if (Upolyd) losehp(u.mhmax, "failing to defeat Blacky in time", KILLED_BY);
			losehp(u.uhpmax, "failing to defeat Blacky in time", KILLED_BY);
			break;
		case STRANGLED:
			u.youaredead = 1;
			killer_format = KILLED_BY;
			killer = (u.uburied) ? "suffocation" : "strangulation";
			done(DIED);
			u.youaredead = 0;
			break;
		case FUMBLING:
			/* call this only when a move took place.  */
			/* otherwise handle fumbling msgs locally. */
			if (u.umoved && !Levitation && !(uarmf && !rn2(10) && itemhasappearance(uarmf, APP_BLUE_SNEAKERS)) && (!(uarmf && uarmf->oartifact == ART_ELEVECULT) || !rn2(4)) && (!PlayerInHighHeels || (PlayerCannotUseSkills) || (rnd(7) > P_SKILL(P_HIGH_HEELS) ) ) ) {
			    slip_or_trip();

			/* based on the evil patch idea by jonadab: stupidity or amnesia from falling on your head --Amy */

			    if (!rn2(uarmh ? 5000 : 1000) && has_head(youmonst.data) && !Role_if(PM_COURIER) ) {

				if (rn2(50)) {
					adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
					if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
				} else {
					You_feel("dizzy!");
					forget(1 + rn2(5));
				}
			    }

			    nomul(-2, "fumbling", TRUE);
			    nomovemsg = "";
			    /* The more you are carrying the more likely you
			     * are to make noise when you fumble.  Adjustments
			     * to this number must be thoroughly play tested.
			     */
			    if ((inv_weight() > -500)) {
				You("make a lot of noise!");
				wake_nearby();
			    }
			}
			/* from outside means slippery ice; don't reset
			   counter if that's the only fumble reason */
			if (!u.fumbleduration) HFumbling &= ~FROMOUTSIDE;
			if (Fumbling)
			    HFumbling += rnd(20);
			break;
		case DETECT_MONSTERS:
			see_monsters();
			break;

		case TIME_STOPPED:
			pline("Time is no longer stopped.");
			break;

		case DEAC_FIRE_RES:
			pline("You are no longer prevented from having fire resistance.");
			break;
		case DEAC_COLD_RES:
			pline("You are no longer prevented from having cold resistance.");
			break;
		case DEAC_SLEEP_RES:
			pline("You are no longer prevented from having sleep resistance.");
			break;
		case DEAC_DISINT_RES:
			pline("You are no longer prevented from having disintegration resistance.");
			break;
		case DEAC_SHOCK_RES:
			pline("You are no longer prevented from having shock resistance.");
			break;
		case DEAC_POISON_RES:
			pline("You are no longer prevented from having poison resistance.");
			break;
		case DEAC_DRAIN_RES:
			pline("You are no longer prevented from having drain resistance.");
			break;
		case DEAC_SICK_RES:
			pline("You are no longer prevented from having sickness resistance.");
			break;
		case DEAC_ANTIMAGIC:
			pline("You are no longer prevented from having magic resistance.");
			break;
		case DEAC_ACID_RES:
			pline("You are no longer prevented from having acid resistance.");
			break;
		case DEAC_STONE_RES:
			pline("You are no longer prevented from having petrification resistance.");
			break;
		case DEAC_FEAR_RES:
			pline("You are no longer prevented from having fear resistance.");
			break;
		case DEAC_SEE_INVIS:
			pline("You are no longer prevented from having see invisible.");
			break;
		case DEAC_TELEPAT:
			pline("You are no longer prevented from having telepathy.");
			break;
		case DEAC_WARNING:
			pline("You are no longer prevented from having warning.");
			break;
		case DEAC_SEARCHING:
			pline("You are no longer prevented from having automatic searching.");
			break;
		case DEAC_CLAIRVOYANT:
			pline("You are no longer prevented from having clairvoyance.");
			break;
		case DEAC_INFRAVISION:
			pline("You are no longer prevented from having infravision.");
			break;
		case DEAC_DETECT_MONSTERS:
			pline("You are no longer prevented from having detect monsters.");
			break;
		case DEAC_INVIS:
			pline("You are no longer prevented from having invisibility.");
			break;
		case DEAC_DISPLACED:
			pline("You are no longer prevented from having displacement.");
			break;
		case DEAC_STEALTH:
			pline("You are no longer prevented from having stealth.");
			break;
		case DEAC_JUMPING:
			pline("You are no longer prevented from having jumping.");
			break;
		case DEAC_TELEPORT_CONTROL:
			pline("You are no longer prevented from having teleport control.");
			break;
		case DEAC_FLYING:
			pline("You are no longer prevented from having flying.");
			break;
		case DEAC_MAGICAL_BREATHING:
			pline("You are no longer prevented from having magical breathing.");
			break;
		case DEAC_PASSES_WALLS:
			pline("You are no longer prevented from having phasing.");
			break;
		case DEAC_SLOW_DIGESTION:
			pline("You are no longer prevented from having slow digestion.");
			break;
		case DEAC_HALF_SPDAM:
			pline("You are no longer prevented from having half spell damage.");
			break;
		case DEAC_HALF_PHDAM:
			pline("You are no longer prevented from having half physical damage.");
			break;
		case DEAC_REGENERATION:
			pline("You are no longer prevented from having regeneration.");
			break;
		case DEAC_ENERGY_REGENERATION:
			pline("You are no longer prevented from having energy regeneration.");
			break;
		case DEAC_POLYMORPH_CONTROL:
			pline("You are no longer prevented from having polymorph control.");
			break;
		case DEAC_FAST:
			pline("You are no longer prevented from having speed.");
			break;
		case DEAC_REFLECTING:
			pline("You are no longer prevented from having reflection.");
			break;
		case DEAC_KEEN_MEMORY:
			pline("You are no longer prevented from having keen memory.");
			break;
		case DEAC_SIGHT_BONUS:
			pline("You are no longer prevented from having extra sight.");
			break;
		case DEAC_VERSUS_CURSES:
			pline("You are no longer prevented from having curse resistance.");
			break;
		case DEAC_STUN_RES:
			pline("You are no longer prevented from having stun resistance.");
			break;
		case DEAC_CONF_RES:
			pline("You are no longer prevented from having confusion resistance.");
			break;
		case DEAC_PSI_RES:
			pline("You are no longer prevented from having psi resistance.");
			break;
		case DEAC_DOUBLE_ATTACK:
			pline("You are no longer prevented from having double attacks.");
			break;
		case DEAC_QUAD_ATTACK:
			pline("You are no longer prevented from having quad attacks.");
			break;
		case DEAC_THE_FORCE:
			pline("You are no longer prevented from using the force like a real jedi.");
			break;
		case DEAC_FREE_ACTION:
			pline("You are no longer prevented from having free action.");
			break;
		case DEAC_HALLU_PARTY:
			pline("You are no longer prevented from hallu partying.");
			break;
		case DEAC_DRUNKEN_BOXING:
			pline("You are no longer prevented from drunken boxing.");
			break;
		case DEAC_STUNNOPATHY:
			pline("You are no longer prevented from having stunnopathy.");
			break;
		case DEAC_NUMBOPATHY:
			pline("You are no longer prevented from having numbopathy.");
			break;
		case DEAC_DIMMOPATHY:
			pline("You are no longer prevented from having dimmopathy.");
			break;
		case DEAC_FREEZOPATHY:
			pline("You are no longer prevented from having freezopathy.");
			break;
		case DEAC_STONED_CHILLER:
			pline("You are no longer prevented from being a stoned chiller.");
			break;
		case DEAC_CORROSIVITY:
			pline("You are no longer prevented from having corrosivity.");
			break;
		case DEAC_FEAR_FACTOR:
			pline("You are no longer prevented from having an increased fear factor.");
			break;
		case DEAC_BURNOPATHY:
			pline("You are no longer prevented from having burnopathy.");
			break;
		case DEAC_SICKOPATHY:
			pline("You are no longer prevented from having sickopathy.");
			break;
		case DEAC_WONDERLEGS:
			pline("You are no longer prevented from having wonderlegs.");
			break;
		case DEAC_GLIB_COMBAT:
			pline("You are no longer prevented from having glib combat.");
			break;
		case DEAC_MANALEECH:
			pline("You are no longer prevented from having manaleech.");
			break;
		case DEAC_PEACEVISION:
			pline("You are no longer prevented from having peacevision.");
			break;
		case DEAC_CONT_RES:
			pline("You are no longer prevented from having contamination resistance.");
			break;
		case DEAC_DISCOUNT_ACTION:
			pline("You are no longer prevented from having discount action.");
			break;
		case DEAC_FULL_NUTRIENT:
			pline("You are no longer prevented from having full nutrients.");
			break;
		case DEAC_TECHNICALITY:
			pline("You are no longer prevented from having technicality.");
			break;
		case DEAC_SCENT_VIEW:
			pline("You are no longer prevented from having scent view.");
			break;
		case DEAC_DIMINISHED_BLEEDING:
			pline("You are no longer prevented from having diminished bleeding.");
			break;
		case DEAC_CONTROL_MAGIC:
			pline("You are no longer prevented from having control magic.");
			break;
		case DEAC_EXP_BOOST:
			pline("You are no longer prevented from having EXP boost.");
			break;
		case DEAC_ASTRAL_VISION:
			pline("You are no longer prevented from having astral vision.");
			break;
		case DEAC_BLIND_RES:
			pline("You are no longer prevented from having blindness resistance.");
			break;
		case DEAC_HALLUC_RES:
			pline("You are no longer prevented from having hallucination resistance.");
			break;

		}
	}

	run_timers();
}

#endif /* OVL0 */
#ifdef OVL1

void
fall_asleep(how_long, wakeup_msg)
int how_long;
boolean wakeup_msg;
{
	stop_occupation();
	nomul(how_long, "sleeping", TRUE);

	if (evilfriday) {
		pline("Ouch! You crash into the %s.", surface(u.ux,u.uy));
		losehp(rn1(3,3), "slapping to the floor", KILLED_BY);
	}

	/* generally don't notice sounds while sleeping */
	if (wakeup_msg && multi == how_long) {
	    /* caller can follow with a direct call to Hear_again() if
	       there's a need to override this when wakeup_msg is true */
	    flags.soundok = 0;
	    afternmv = Hear_again;	/* this won't give any messages */
	}
	/* early wakeup from combat won't be possible until next monster turn */
	u.usleep = monstermoves;
	nomovemsg = wakeup_msg ? "You wake up." : You_can_move_again;
}


/* WAC polymorph an object
 * Unlike monsters,  this function is called after the polymorph
 */
void
set_obj_poly(obj, old)
struct obj *obj, *old;
{
	/* Same unpolytime (500,500) as for player */
	if (is_hazy(old))
	    obj->oldtyp = old->oldtyp;
	else
	    obj->oldtyp = old->otyp;
	if (obj->oldtyp == obj->otyp)
	    obj->oldtyp = STRANGE_OBJECT;
	else
	    (void) start_timer(/*rn1(500,500)*/rnz(1000), TIMER_OBJECT,
			UNPOLY_OBJ, (void *) obj);
	return;
}

/* timer callback routine: undo polymorph on an object */
void
unpoly_obj(arg, timeout)
	void * arg;
	long timeout;
{
	struct obj *obj, *otmp, *otmp2;
	int oldobj, depthin;
	boolean silent = (timeout != monstermoves),     /* unpoly'ed while away */
		explodes;

	obj = (struct obj *) arg;
	if (!is_hazy(obj)) return;
	oldobj = obj->oldtyp;

	if (carried(obj) && !silent) /* silent == TRUE is a strange case... */
		pline("Suddenly, your %s!", aobjnam(obj, "transmute"));

	(void) stop_timer(UNPOLY_OBJ, (void *) obj);

	obj = poly_obj(obj, oldobj, FALSE);

	if (obj->otyp == WAN_CANCELLATION || Is_mbag(obj)) {
	    otmp = obj;
	    depthin = 0;
	    explodes = FALSE;

	    while (otmp->where == OBJ_CONTAINED) {
		otmp = otmp->ocontainer;
		if (otmp->otyp == BAG_OF_HOLDING || otmp->otyp == ICE_BOX_OF_HOLDING || otmp->otyp == CHEST_OF_HOLDING) {
		    explodes = mbag_explodes(obj, depthin);
		    break;
		}
		depthin++;
	    }

	    if (explodes) {
		otmp2 = otmp;
		while (otmp2->where == OBJ_CONTAINED) {
		    otmp2 = otmp2->ocontainer;

		    if (otmp2->otyp == BAG_OF_HOLDING || otmp2->otyp == ICE_BOX_OF_HOLDING || otmp2->otyp == CHEST_OF_HOLDING) 
			otmp = otmp2;
		}
		destroy_mbag(otmp, silent);
	    }
	}	
	return;
}
#endif /* OVL1 */
#ifdef OVL0

/*
 * Cleanup a hazy object if timer stopped.
 */
/*ARGSUSED*/
static void
cleanup_unpoly(arg, timeout)
    void * arg;
    long timeout;
{
#if defined(MAC_MPW) || defined(__MWERKS__)
# pragma unused(timeout)
#endif
    struct obj *obj = (struct obj *)arg;
    obj->oldtyp = STRANGE_OBJECT;
#ifdef WIZARD
    if (wizard && obj->where == OBJ_INVENT)
	update_inventory();
#endif
}

#endif /* OVL0 */
#ifdef OVL1

/* WAC polymorph a monster
 * returns 0 if no change, 1 if polymorphed and -1 if died.
 * This handles system shock for monsters so DON'T do system shock elsewhere
 * when polymorphing.
 * (except in unpolymorph code,  which is a special case)
 */
int
mon_poly(mtmp, your_fault, change_fmt)
struct monst *mtmp;
boolean your_fault;
const char *change_fmt;
{
	if (change_fmt && canseemon(mtmp)) pline(change_fmt, Monnam(mtmp));
	return mon_spec_poly(mtmp, (struct permonst *)0, 0L,
		FALSE, canseemon(mtmp), TRUE, your_fault);
}


/* WAC Muscle function - for more control over polying
 * returns 0 if no change, 1 if polymorphed and -1 if died.
 * cancels/sets up timers if polymorph is successful
 * lets receiver handle failures
 */

int
mon_spec_poly(mtmp, type, when, polyspot, transform_msg, system_shock,
	your_fault)
struct monst *mtmp;
struct permonst *type;
long when;
boolean polyspot;
boolean transform_msg;
boolean system_shock;
boolean your_fault;
{
	int i;
	int wasrider = 0;

	/* Due to changes in muse.c, riders can now polyself via items. This would allow players to get rid of them easily,
	 * by throwing a cockatrice egg at the polymorphed monster. Or get a tame rider by taming the polymorph result,
	 * although the newly added M3_TRAITOR and M3_NOTAME flags should make that less useful. Anyway, in order to prevent
	 * abuse, we'll now spawn clones of the riders if they polymorph. Yes, this will also happen if you didn't actually
	 * try to get rid of the riders permanently; after all, this is the Astral Plane, and you should not linger around
	 * anyway but work towards your goal, which is called ascension! --Amy */
	if (mtmp->data == &mons[PM_DEATH]) wasrider = 1;
	if (mtmp->data == &mons[PM_PESTILENCE]) wasrider = 2;
	if (mtmp->data == &mons[PM_FAMINE]) wasrider = 3;
	if (mtmp->data == &mons[PM_FRUSTRATION]) wasrider = 4;

	i = newcham(mtmp, type, polyspot, transform_msg);
	if (system_shock && (!i || !rn2(25))) {
	    /* Uhoh.  !i == newcham wasn't able to make the polymorph...*/
	    if (transform_msg) pline("%s shudders.", Monnam(mtmp));
	    if (i) mtmp->mhp -= rnd(30);
	    if (!rn2(200)) mtmp->mhp = 0; /* chance that the monster doesn't survive the polymorph --Amy */
	    if (!i || (mtmp->mhp <= 0)) {
		if (your_fault) xkilled(mtmp, 3);
		else mondead(mtmp);
		i = -1;
	    }
	}
	if (i > 0) {

		if (wasrider == 1) makemon(&mons[PM_DEATH], 0, 0, NO_MM_FLAGS);
		if (wasrider == 2) makemon(&mons[PM_PESTILENCE], 0, 0, NO_MM_FLAGS);
		if (wasrider == 3) makemon(&mons[PM_FAMINE], 0, 0, NO_MM_FLAGS);
		if (wasrider == 4) makemon(&mons[PM_FRUSTRATION], 0, 0, NO_MM_FLAGS);

	    /* Stop any old timers.   */
	    (void) stop_timer(UNPOLY_MON, (void *) mtmp);
	    /* Lengthen unpolytime - was 500,500  for player */
	    (void) start_timer(when ? when : /*rn1(1000, 1000)*/rnz(2000), TIMER_MONSTER,
		    UNPOLY_MON, (void *) mtmp);
	}
	return i;
}

int
mon_spec_polyX(mtmp, type, when, polyspot, transform_msg, system_shock,
	your_fault)
struct monst *mtmp;
struct permonst *type;
long when;
boolean polyspot;
boolean transform_msg;
boolean system_shock;
boolean your_fault;
{
	int i;

	i = newcham(mtmp, type, polyspot, transform_msg);
	if (system_shock && (!i || !rn2(25))) {
	    /* Uhoh.  !i == newcham wasn't able to make the polymorph...*/
	    if (transform_msg) pline("%s shudders.", Monnam(mtmp));
	    if (i) mtmp->mhp -= rnd(30);
	    if (!rn2(200)) mtmp->mhp = 0; /* chance that the monster doesn't survive the polymorph --Amy */
	    if (!i || (mtmp->mhp <= 0)) {
		if (your_fault) xkilled(mtmp, 3);
		else mondead(mtmp);
		i = -1;
	    }
	}
	if (i > 0) {
	    /* Stop any old timers.   */
	    (void) stop_timer(UNPOLY_MON, (void *) mtmp);
	    /* Lengthen unpolytime - was 500,500  for player */
	    (void) start_timer(when ? when : /*rn1(1000, 1000)*/999999, TIMER_MONSTER,
		    UNPOLY_MON, (void *) mtmp);
	}
	return i;
}


/* timer callback routine: undo polymorph on a monster */
void
unpoly_mon(arg, timeout)
	void * arg;
	long timeout;
{
	struct monst *mtmp;
	int oldmon;
	char oldname[BUFSZ];  /* DON'T use char * since this will change! */
	boolean silent = (timeout != monstermoves);     /* unpoly'ed while away */

	mtmp = (struct monst *) arg;
	oldmon = mtmp->oldmonnm;

	strcpy(oldname, Monnam(mtmp));

	(void) stop_timer(UNPOLY_MON, (void *) mtmp);

	if (!newcham(mtmp, &mons[oldmon], FALSE, (canseemon(mtmp) && !silent))) {
	    /* Wasn't able to unpolymorph */
	    if (canseemon(mtmp) && !silent) pline("%s shudders.", oldname);
	    mondead(mtmp);
	    return;
	}

	/* Check if current form is genocided */
	if (mvitals[oldmon].mvflags & G_GENOD) {
	    mtmp->mhp = 0;
	    if (canseemon(mtmp) && !silent) pline("%s shudders.", oldname);
	    /*  Since only player can read scrolls of genocide... */
	    xkilled(mtmp, 3);
	    return;
	}

#if 0
	if (canseemon(mtmp)) pline ("%s changes into %s!", 
		oldname, an(mtmp->data->mname));
#endif
	return;
}

/* Attach an explosion timeout to a given explosive device */
void
attach_bomb_blow_timeout(bomb, fuse, yours)
struct obj *bomb;
int fuse;
boolean yours;
{
	long expiretime;	

	if (bomb->cursed && !bomb->dynamitekaboom && !rn2(2)) return; /* doesn't arm if not armed */

	/* Now if you play with other people's property... */
	if (yours && (!carried(bomb) && costly_spot(bomb->ox, bomb->oy) &&
		!bomb->no_charge || bomb->unpaid)) {
	    verbalize("You play with it, you pay for it!");
	    bill_dummy_object(bomb);
	}

	expiretime = stop_timer(BOMB_BLOW, (void *) bomb);
	if (expiretime > 0L) fuse = fuse - (expiretime - monstermoves);
	bomb->yours = yours;
	bomb->oarmed = TRUE;

	(void) start_timer((long)fuse, TIMER_OBJECT, BOMB_BLOW, (void *)bomb);
}

/* timer callback routine: detonate the explosives */
void
bomb_blow(arg, timeout)
void * arg;
long timeout;
{
	struct obj *bomb;
	xchar x,y;
	boolean silent, underwater;
	struct monst *mtmp = (struct monst *)0;

	bomb = (struct obj *) arg;

	silent = (timeout != monstermoves);     /* exploded while away */

	if (get_obj_location(bomb, &x, &y, BURIED_TOO | CONTAINED_TOO)) {
		switch(bomb->where) {		
		    case OBJ_MINVENT:
		    	mtmp = bomb->ocarry;
			if (bomb == MON_WEP(mtmp)) {
			    bomb->owornmask &= ~W_WEP;
			    MON_NOWEP(mtmp);
			}
			if (!silent) {
			    if (canseemon(mtmp))
				You("see %s engulfed in an explosion!", mon_nam(mtmp));
			}
		    	mtmp->mhp -= d(2,5);
			if(mtmp->mhp < 1) {
				if(!bomb->yours) 
					monkilled(mtmp, 
						  (silent ? "" : "explosion"),
						  AD_PHYS);
				else xkilled(mtmp, !silent);
			}
			break;
		    case OBJ_INVENT:
		    	/* This shouldn't be silent! */
			pline("Something explodes inside your knapsack!");
			if (bomb == uwep) {
			    uwepgone();
			    stop_occupation();
			} else if (bomb == uswapwep) {
			    uswapwepgone();
			    stop_occupation();
			} else if (bomb == uquiver) {
			    uqwepgone();
			    stop_occupation();
			}
		    	losehp(d(2,5), "carrying live explosives", KILLED_BY);
		    	break;
		    case OBJ_FLOOR:
			underwater = is_waterypool(x, y) || is_watertunnel(x,y);
			if (!silent) {
			    if (x == u.ux && y == u.uy) {
				if (underwater && (Flying || Levitation))
				    pline_The("water boils beneath you.");
				else if (underwater && (Wwalking || Race_if(PM_KORONST)))
				    pline_The("water erupts around you.");
				else pline("A bomb explodes under your %s!",
				  makeplural(body_part(FOOT)));
			    } else if (cansee(x, y))
				You(underwater ?
				    "see a plume of water shoot up." :
				    "see a bomb explode.");
			}
			if (underwater && (Flying || Levitation || Wwalking || Race_if(PM_KORONST))) {
			    if ((Wwalking || Race_if(PM_KORONST)) && x == u.ux && y == u.uy) {
				struct trap trap;
				trap.ntrap = NULL;
				trap.tx = x;
				trap.ty = y;
				trap.launch.x = -1;
				trap.launch.y = -1;
				trap.ttyp = RUST_TRAP;
				trap.tseen = 0;
				trap.once = 0;
				trap.madeby_u = 0;
				trap.dst.dnum = -1;
				trap.dst.dlevel = -1;
				dotrap(&trap, 0);
			    }
			    goto free_bomb;
			}
		    	break;
		    default:	/* Buried, contained, etc. */
			if (!silent)
			    You_hear("a muffled explosion.");
			goto free_bomb;
			break;
		}
		grenade_explode(bomb, x, y, bomb->yours, silent ? 2 : 0);
		return;
	} /* Migrating grenades "blow up in midair" */

free_bomb:
	obj_extract_self(bomb);
	obfree(bomb, (struct obj *)0);
}

/* Attach an egg hatch timeout to the given egg. */
void
attach_egg_hatch_timeout(egg)
struct obj *egg;
{
	int i;

	/* stop previous timer, if any */
	(void) stop_timer(HATCH_EGG, (void *) egg);

	/*
	 * Decide if and when to hatch the egg.  The old hatch_it() code tried
	 * once a turn from age 151 to 200 (inclusive), hatching if it rolled
	 * a number x, 1<=x<=age, where x>150.  This yields a chance of
	 * hatching > 99.9993%.  Mimic that here.
	 */
	for (i = (MAX_EGG_HATCH_TIME-50)+1; i <= MAX_EGG_HATCH_TIME; i++)
	    if (rnd(i) > 150) {
		/* egg will hatch */
		(void) start_timer((long)i, TIMER_OBJECT,
						HATCH_EGG, (void *)egg);
		break;
	    }
}

/* prevent an egg from ever hatching */
void
kill_egg(egg)
struct obj *egg;
{
	/* stop previous timer, if any */
	(void) stop_timer(HATCH_EGG, (void *) egg);
}

/* timer callback routine: hatch the given egg */
void
hatch_egg(arg, timeout)
void * arg;
long timeout;
{
	struct obj *egg;
	struct monst *mon, *mon2;
	coord cc;
	xchar x, y;
	boolean yours, silent, knows_egg = FALSE;
	boolean cansee_hatchspot = FALSE;
	int i, mnum, hatchcount = 0;

	egg = (struct obj *) arg;
	/* sterilized while waiting */
	if (egg->corpsenm == NON_PM) return;
	if (u.sterilized) return;

	mon = mon2 = (struct monst *)0;
	mnum = big_to_little(egg->corpsenm);
	/* The identity of one's father is learned, not innate */
	yours = (egg->spe || (!flags.female && carried(egg) && !rn2(2)) || (((rnd(30 - ACURR(A_CHA))) < 4) && carried(egg)) );
	silent = (timeout != monstermoves);	/* hatched while away */

	/* only can hatch when in INVENT, FLOOR, MINVENT */
	if (get_obj_location(egg, &x, &y, 0)) {
	    hatchcount = rnd((int)egg->quan);
	    cansee_hatchspot = cansee(x, y) && !silent;
	    if (!(mons[mnum].geno & G_UNIQ) &&
		   !(mvitals[mnum].mvflags & (G_GENOD | G_EXTINCT))) {
		for (i = hatchcount; i > 0; i--) {
		    if (!enexto(&cc, x, y, &mons[mnum]) ||
			 !(mon = makemon(&mons[mnum], cc.x, cc.y, NO_MINVENT)))
			break;

			if (mon) u.cnd_eggcount++;

		    /* tame if your own egg hatches while you're on the
		       same dungeon level, or any dragon egg which hatches
		       while it's in your inventory */
		    if ((yours && !silent) ||
			(carried(egg) && mon->data->mlet == S_DRAGON)) {
			if ((mon2 = tamedog(mon, (struct obj *)0, FALSE)) != 0) {
			    mon = mon2;
			    if (carried(egg) && mon->data->mlet != S_DRAGON)
				mon->mtame = 20;
			}
		    }
		    if (mvitals[mnum].mvflags & G_EXTINCT)
			break;	/* just made last one */
		    mon2 = mon;	/* in case makemon() fails on 2nd egg */
		}
		if (!mon) mon = mon2;
		hatchcount -= i;
		egg->quan -= (long)hatchcount;
	    }
	}
#if 0
	/*
	 * We could possibly hatch while migrating, but the code isn't
	 * set up for it...
	 */
	else if (obj->where == OBJ_MIGRATING) {
	    /*
	    We can do several things.  The first ones that come to
	    mind are:

	    + Create the hatched monster then place it on the migrating
	      mons list.  This is tough because all makemon() is made
	      to place the monster as well.    Makemon() also doesn't
	      lend itself well to splitting off a "not yet placed"
	      subroutine.

	    + Mark the egg as hatched, then place the monster when we
	      place the migrating objects.

	    + Or just kill any egg which gets sent to another level.
	      Falling is the usual reason such transportation occurs.
	    */
	    cansee_hatchspot = FALSE;
	    mon = ???
	    }
#endif

	if (mon) {
	    char monnambuf[BUFSZ], carriedby[BUFSZ];
	    boolean siblings = (hatchcount > 1), redraw = FALSE;

	    if (cansee_hatchspot) {
		sprintf(monnambuf, "%s%s",
			siblings ? "some " : "",
			siblings ?
			makeplural(m_monnam(mon)) : an(m_monnam(mon)));
		/* we don't learn the egg type here because learning
		   an egg type requires either seeing the egg hatch
		   or being familiar with the egg already,
		   as well as being able to see the resulting
		   monster, checked below
		*/
	    }
	    switch (egg->where) {
		case OBJ_INVENT:
		    knows_egg = TRUE; /* true even if you are blind */
		    if (!cansee_hatchspot)
			You_feel("%s %s from your pack!", something,
			    locomotion(mon->data, "drop"));
		    else
			You("see %s %s out of your pack!",
			    monnambuf, locomotion(mon->data, "drop"));
		    if (yours) {
			pline("%s cries sound like \"%s%s\"",
			    siblings ? "Their" : "Its",
			    flags.female ? "mommy" : "daddy",
			    egg->spe ? "." : "?");
		    } else if (mon->data->mlet == S_DRAGON) {
			verbalize("Gleep!");		/* Mything eggs :-) */
		    }
		    break;

		case OBJ_FLOOR:
		    if (cansee_hatchspot) {
			knows_egg = TRUE;
			You("see %s hatch.", monnambuf);
			redraw = TRUE;	/* update egg's map location */
		    }
		    break;

		case OBJ_MINVENT:
		    if (cansee_hatchspot) {
			/* egg carring monster might be invisible */
			if (canseemon(egg->ocarry)) {
			    sprintf(carriedby, "%s pack",
				     s_suffix(a_monnam(egg->ocarry)));
			    knows_egg = TRUE;
			}
			else if (is_waterypool(mon->mx, mon->my))
			    strcpy(carriedby, "empty water");
			else
			    strcpy(carriedby, "thin air");
			You("see %s %s out of %s!", monnambuf,
			    locomotion(mon->data, "drop"), carriedby);
		    }
		    break;
#if 0
		case OBJ_MIGRATING:
		    break;
#endif
		default:
		    impossible("egg hatched where? (%d)", (int)egg->where);
		    break;
	    }

	    if (cansee_hatchspot && knows_egg)
		learn_egg_type(mnum);

	    if (egg->quan > 0) {
		/* still some eggs left */
		attach_egg_hatch_timeout(egg);
		if (egg->timed) {
		    /* replace ordinary egg timeout with a short one */
		    (void) stop_timer(HATCH_EGG, (void *)egg);
		    (void) start_timer((long)rnd(12), TIMER_OBJECT,
					HATCH_EGG, (void *)egg);
		}
	    } else if (carried(egg)) {
		useup(egg);
	    } else {
		/* free egg here because we use it above */
		obj_extract_self(egg);
		obfree(egg, (struct obj *)0);
	    }
	    if (redraw) newsym(x, y);
	}
}

/* Learn to recognize eggs of the given type. */
void
learn_egg_type(mnum)
int mnum;
{
	/* baby monsters hatch from grown-up eggs */
	mnum = little_to_big(mnum);
	mvitals[mnum].mvflags |= MV_KNOWS_EGG;
	/* we might have just learned about other eggs being carried */
	update_inventory();
}

/* Attach a fig_transform timeout to the given figurine. */
void
attach_fig_transform_timeout(figurine)
struct obj *figurine;
{
	int i;

	/* stop previous timer, if any */
	(void) stop_timer(FIG_TRANSFORM, (void *) figurine);

	/*
	 * Decide when to transform the figurine.
	 */
	i = rnd(9000) + 200;
	/* figurine will transform */
	(void) start_timer((long)i, TIMER_OBJECT,
				FIG_TRANSFORM, (void *)figurine);
}

/* give a fumble message */
STATIC_OVL void
slip_or_trip()
{
	struct obj *otmp = vobj_at(u.ux, u.uy);
	const char *what, *pronoun;
	char buf[BUFSZ];
	boolean on_foot = TRUE;
	if (u.usteed) on_foot = FALSE;

	u.cnd_fumbled++;

	if (otmp && on_foot && !u.uinwater && is_waterypool(u.ux, u.uy)) otmp = 0;

	if (otmp && on_foot) {		/* trip over something in particular */
	    /*
		If there is only one item, it will have just been named
		during the move, so refer to by via pronoun; otherwise,
		if the top item has been or can be seen, refer to it by
		name; if not, look for rocks to trip over; trip over
		anonymous "something" if there aren't any rocks.
	     */
	    pronoun = otmp->quan == 1L ? "it" : FunnyHallu ? "they" : "them";
	    what = !otmp->nexthere ? pronoun :
		  (otmp->dknown || !Blind) ? doname(otmp) :
		  ((otmp = sobj_at(ROCK, u.ux, u.uy)) == 0 ? something :
		  (otmp->quan == 1L ? "a rock" : "some rocks"));
	    if (FunnyHallu) {
		what = strcpy(buf, what);
		buf[0] = highc(buf[0]);
		pline("Egads!  %s bite%s your %s!",
			what, (!otmp || otmp->quan == 1L) ? "s" : "",
			body_part(FOOT));
	    } else {
		You("trip over %s.", what);
	    }
	} else if (rn2(3) && is_ice(u.ux, u.uy)) {
	    pline("%s %s%s on the ice.", u.usteed ? upstart(x_monnam(u.usteed,
				u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				(char *)0, SUPPRESS_SADDLE, FALSE)) :
		"You", rn2(2) ? "slip" : "slide", on_foot ? "" : "s");
	} else {
	    if (on_foot) {
		switch (rn2(4)) {
		  case 1:
			You("trip over your own %s.", FunnyHallu ?
				"elbow" : makeplural(body_part(FOOT)));
			break;
		  case 2:
			You("slip %s.", FunnyHallu ?
				"on a banana peel" : "and nearly fall");
			break;
		  case 3:
			You("flounder.");
			break;
		  default:
			You("stumble.");
			break;
		}
	    }
	    else if (!mayfalloffsteed()) {
		switch (rn2(4)) {
		  case 1:
			Your("%s slip out of the stirrups.", makeplural(body_part(FOOT)));
			break;
		  case 2:
			You("let go of the reins.");
			break;
		  case 3:
			You("bang into the saddle-horn.");
			break;
		  default:
			You("slide to one side of the saddle.");
			break;
		}
		dismount_steed(DISMOUNT_FELL);
	    }
	}
}

/* Print a lamp flicker message with tailer. */
STATIC_OVL void
see_lamp_flicker(obj, tailer)
struct obj *obj;
const char *tailer;
{
	switch (obj->where) {
	    case OBJ_INVENT:
	    case OBJ_MINVENT:
		pline("%s flickers%s.", Yname2(obj), tailer);
		break;
	    case OBJ_FLOOR:
		You("see %s flicker%s.", an(xname(obj)), tailer);
		break;
	}
}

/* Print a dimming message for brass lanterns. */
STATIC_OVL void
lantern_message(obj)
struct obj *obj;
{
	/* from adventure */
	switch (obj->where) {
	    case OBJ_INVENT:
		Your("lantern is getting dim.");
		if (FunnyHallu)
		    pline("Batteries have not been invented yet.");
		break;
	    case OBJ_FLOOR:
		You("see a lantern getting dim.");
		break;
	    case OBJ_MINVENT:
		pline("%s lantern is getting dim.",
		    s_suffix(Monnam(obj->ocarry)));
		break;
	}
}

/*
 * Timeout callback for for objects that are burning. E.g. lamps, candles.
 * See begin_burn() for meanings of obj->age and obj->spe.
 */
void
burn_object(arg, timeout)
void * arg;
long timeout;
{
	struct obj *obj = (struct obj *) arg;
	boolean canseeit, many, menorah, need_newsym;
	xchar x, y;
	char whose[BUFSZ];
	int lightsaberchance = 0;
	int lightsaberchance2 = 0;

	menorah = obj->otyp == CANDELABRUM_OF_INVOCATION;
	many = menorah ? obj->spe > 1 : obj->quan > 1L;

	/* timeout while away */
	if (timeout != monstermoves) {
	    long how_long = monstermoves - timeout;

	    if (how_long >= obj->age) {
		obj->age = 0;
		end_burn(obj, FALSE);

		if (menorah) {
		    obj->spe = 0;	/* no more candles */
		} else if (Is_candle(obj) || obj->otyp == POT_OIL) {
		    /* get rid of candles and burning oil potions */
		    obj_extract_self(obj);
		    obfree(obj, (struct obj *)0);
		    obj = (struct obj *) 0;
		} else if (obj->otyp == STICK_OF_DYNAMITE) {
			bomb_blow((void *) obj, timeout);
			return;
		}

	    } else {

		lightsaberchance = 0;
		lightsaberchance2 = 0;
		boolean willdrainenergy = TRUE;

		if (uarms && uarms->oartifact == ART_THERMO_NUCLEAR_CHAMBER && rn2(3)) willdrainenergy = FALSE;
		if (uarms && uarms->oartifact == ART_SUPER_ENERGY_LINES && rn2(2)) willdrainenergy = FALSE;
		if (obj->otyp == HEAVY_LASER_BALL && rn2(3)) willdrainenergy = FALSE;
		if (obj->otyp == LASER_TIN_OPENER && rn2(2)) willdrainenergy = FALSE;

		if (tech_inuse(T_PIRATE_BROTHERING) && uwep && is_lightsaber(uwep) && uswapwep && weapon_type(uswapwep) == P_SCIMITAR && u.twoweap) {
			willdrainenergy = FALSE; /* do nothing */
		}

		if (!PlayerCannotUseSkills && tech_inuse(T_ENERGY_CONSERVATION) && obj && is_lightsaber(obj)) {
			switch (P_SKILL(P_MAKASHI)) {
				case P_BASIC: lightsaberchance = 1; break;
				case P_SKILLED: lightsaberchance = 3; break;
				case P_EXPERT: lightsaberchance = 4; break;
				case P_MASTER: lightsaberchance = 5; break;
				case P_GRAND_MASTER: lightsaberchance = 7; break;
				case P_SUPREME_MASTER: lightsaberchance = 8; break;
			}
		}
		if (!PlayerCannotUseSkills && obj && obj->oartifact == ART_SERIALSABER) {
			switch (P_SKILL(P_MAKASHI)) {
				case P_BASIC: lightsaberchance2 = 1; break;
				case P_SKILLED: lightsaberchance2 = 3; break;
				case P_EXPERT: lightsaberchance2 = 4; break;
				case P_MASTER: lightsaberchance2 = 5; break;
				case P_GRAND_MASTER: lightsaberchance2 = 7; break;
				case P_SUPREME_MASTER: lightsaberchance2 = 8; break;
			}
		}
		if (willdrainenergy && (rnd(10) > lightsaberchance) && (rnd(10) > lightsaberchance2)) obj->age -= how_long;

		begin_burn(obj, TRUE);
	    }
	    return;
	}

	/* only interested in INVENT, FLOOR, and MINVENT */
	if (get_obj_location(obj, &x, &y, 0)) {
	    canseeit = !Blind && cansee(x, y);
	    /* set up `whose[]' to be "Your" or "Fred's" or "The goblin's" */
	    (void) Shk_Your(whose, obj);
	} else {
	    canseeit = FALSE;
	}
	need_newsym = FALSE;

	/* obj->age is the age remaining at this point.  */
	switch (obj->otyp) {
	    case POT_OIL:
		    /* this should only be called when we run out */
		    if (canseeit) {
			switch (obj->where) {
			    case OBJ_INVENT:
			    case OBJ_MINVENT:
				pline("%s potion of oil has burnt away.",
				    whose);
				break;
			    case OBJ_FLOOR:
				You("see a burning potion of oil go out.");
				need_newsym = TRUE;
				break;
			}
		    }
		    end_burn(obj, FALSE);	/* turn off light source */
		    obj_extract_self(obj);
		    obfree(obj, (struct obj *)0);
		    obj = (struct obj *) 0;
		    break;
		    
	    case TORCH:
	    case BRASS_LANTERN:
	    case OIL_LAMP:
		switch((int)obj->age) {
		    case 150:
		    case 100:
		    case 50:
			if (canseeit) {
			    if (obj->otyp == BRASS_LANTERN)
				lantern_message(obj);
			    else
				see_lamp_flicker(obj,
				    obj->age == 50L ? " considerably" : "");
			}
			break;

		    case 25:
			if (canseeit) {
			    if (obj->otyp == BRASS_LANTERN)
				lantern_message(obj);
			    else {
				switch (obj->where) {
				    case OBJ_INVENT:
				    case OBJ_MINVENT:
					pline("%s %s seems about to go out.",
					    whose, xname(obj));
					break;
				    case OBJ_FLOOR:
					You("see %s about to go out.",
					    an(xname(obj)));
					break;
				}
			    }
			}
			break;

		    case 0:
			/* even if blind you'll know if holding it */
			if (canseeit || obj->where == OBJ_INVENT) {
			    switch (obj->where) {
				case OBJ_INVENT:
				case OBJ_MINVENT:
				    if (obj->otyp == BRASS_LANTERN)
					pline("%s lantern has run out of power.",
					    whose);
				    else
					pline("%s %s has gone out.",
					    whose, xname(obj));
				    break;
				case OBJ_FLOOR:
				    if (obj->otyp == BRASS_LANTERN)
					You("see a lantern run out of power.");
				    else
					You("see %s go out.",
					    an(xname(obj)));
				    break;
			    }
			}
			
			/* MRKR: Burnt out torches are considered worthless */
			
			if (obj->otyp == TORCH) {
			  if (obj->unpaid && costly_spot(u.ux, u.uy)) {
			    const char *ithem = obj->quan > 1L ? "them" : "it";
			    verbalize("You burn %s, you bought %s!", ithem, ithem);
			    bill_dummy_object(obj);
			  }
			}
			end_burn(obj, FALSE);
			break;

		    default:
			/*
			 * Someone added fuel to the lamp while it was
			 * lit.  Just fall through and let begin burn
			 * handle the new age.
			 */
			break;
		}

		if (obj->age)
		    begin_burn(obj, TRUE);

		break;

	    case CANDELABRUM_OF_INVOCATION:
	    case TALLOW_CANDLE:
	    case WAX_CANDLE:
	    case JAPAN_WAX_CANDLE:
	    case OIL_CANDLE:
	    case UNAFFECTED_CANDLE:
	    case SPECIFIC_CANDLE:
	    case __CANDLE:
	    case NATURAL_CANDLE:
	    case UNSPECIFIED_CANDLE:
	    case GENERAL_CANDLE:
		switch (obj->age) {
		    case 75:
			if (canseeit)
			    switch (obj->where) {
				case OBJ_INVENT:
				case OBJ_MINVENT:
				    pline("%s %scandle%s getting short.",
					whose,
					menorah ? "candelabrum's " : "",
					many ? "s are" : " is");
				    break;
				case OBJ_FLOOR:
				    You("see %scandle%s getting short.",
					    menorah ? "a candelabrum's " :
						many ? "some " : "a ",
					    many ? "s" : "");
				    break;
			    }
			break;

		    case 15:
			if (canseeit)
			    switch (obj->where) {
				case OBJ_INVENT:
				case OBJ_MINVENT:
				    pline(
					"%s %scandle%s flame%s flicker%s low!",
					    whose,
					    menorah ? "candelabrum's " : "",
					    many ? "s'" : "'s",
					    many ? "s" : "",
					    many ? "" : "s");
				    break;
				case OBJ_FLOOR:
				    You("see %scandle%s flame%s flicker low!",
					    menorah ? "a candelabrum's " :
						many ? "some " : "a ",
					    many ? "s'" : "'s",
					    many ? "s" : "");
				    break;
			    }
			break;

		    case 0:
			/* we know even if blind and in our inventory */
			if (canseeit || obj->where == OBJ_INVENT) {
			    if (menorah) {
				switch (obj->where) {
				    case OBJ_INVENT:
				    case OBJ_MINVENT:
					pline("%s candelabrum's flame%s.",
					    whose,
					    many ? "s die" : " dies");
					break;
				    case OBJ_FLOOR:
					You("see a candelabrum's flame%s die.",
						many ? "s" : "");
					break;
				}
			    } else {
				switch (obj->where) {
				    case OBJ_INVENT:
				    case OBJ_MINVENT:
					pline("%s %s %s consumed!",
					    whose,
					    xname(obj),
					    many ? "are" : "is");
					break;
				    case OBJ_FLOOR:
					/*
					You see some wax candles consumed!
					You see a wax candle consumed!
					*/
					You("see %s%s consumed!",
					    many ? "some " : "",
					    many ? xname(obj):an(xname(obj)));
					need_newsym = TRUE;
					break;
				}

				/* post message */
				pline(FunnyHallu ?
					(many ? "They shriek!" :
						"It shrieks!") :
					Blind ? "" :
					    (many ? "Their flames die." :
						    "Its flame dies."));
			    }
			}
			end_burn(obj, FALSE);

			if (menorah) {
			    obj->spe = 0;
			} else {
			    obj_extract_self(obj);
			    obfree(obj, (struct obj *)0);
			    obj = (struct obj *) 0;
			}
			break;

		    default:
			/*
			 * Someone added fuel (candles) to the menorah while
			 * it was lit.  Just fall through and let begin burn
			 * handle the new age.
			 */
			break;
		}

		if (obj && obj->age)
		    begin_burn(obj, TRUE);

		break;

	    case RED_DOUBLE_LIGHTSABER:
	    case CYAN_DOUBLE_LIGHTSABER:
	    case WHITE_DOUBLE_LIGHTSABER:
	    case LASER_POLE:
	    case LASERDENT:
	    case LASERXBOW:
	    case SITH_STAFF:
	    	if (obj->altmode && obj->cursed && !rn2(25)) {
		    obj->altmode = FALSE;
		    pline("%s %s reverts to single blade mode!",
			    whose, xname(obj));
	    	}
	    case GREEN_LIGHTSABER: 
	    case ORANGE_LIGHTSABER: 
	    case BLACK_LIGHTSABER: 
	    case BLUE_LIGHTSABER:
	    case LASERFIST:
	    case LASER_TIN_OPENER:
	    case LASER_SWORD:
	    case BEAMSWORD:
	    case HEAVY_LASER_BALL:
	    case LASER_CHAIN:
	    case MYSTERY_LIGHTSABER:
	    case VIOLET_LIGHTSABER:
	    case WHITE_LIGHTSABER:
	    case YELLOW_LIGHTSABER:
	    case RED_LIGHTSABER:
	    case LASER_SWATTER:
	    case NANO_HAMMER:
	    case LIGHTWHIP:
	    case KLIUSLING:
	    case STARWARS_MACE:
	    case ELECTRIC_CIGARETTE:
	        /* Callback is checked every 5 turns - 
	        	lightsaber automatically deactivates if not wielded */
	        if ((obj->cursed && !rn2(50)) ||
	            (obj->where == OBJ_FLOOR) || 
		    (obj->where == OBJ_MINVENT && 
		    	(!MON_WEP(obj->ocarry) || MON_WEP(obj->ocarry) != obj)) ||
		    (obj->where == OBJ_INVENT &&
		    	((!uwep || uwep != obj) &&
		    	 (!u.twoweap || !uswapwep || obj != uswapwep))))
	            lightsaber_deactivate(obj, FALSE);
		switch (obj->age) {			
		    case 100:
			/* Single warning time */
			if (canseeit) {
			    switch (obj->where) {
				case OBJ_INVENT:
				case OBJ_MINVENT:
				    pline("%s %s dims!",whose, xname(obj));
				    break;
				case OBJ_FLOOR:
				    You("see %s dim!", an(xname(obj)));
				    break;
			    }
			} else {
			    You("hear the hum of %s change!", an(xname(obj)));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Bystro! Zapusk i pust' eto pokonchim monstra proch' s yego svetovym mechom, prezhde chem eto slishkom pozdno!" : "Dmmmmmm-dmmmmmmmmm!");
			}
			break;
		    case 0:
			lightsaber_deactivate(obj, FALSE);
			break;

		    default:
			/*
			 * Someone added fuel to the lightsaber while it was
			 * lit.  Just fall through and let begin burn
			 * handle the new age.
			 */
			break;
		}
		if (obj && obj->age && obj->lamplit) /* might be deactivated */
		    begin_burn(obj, TRUE);
		break;

	    case STICK_OF_DYNAMITE:
		end_burn(obj, FALSE);
		bomb_blow((void *) obj, timeout);
		return;
	    default:
		impossible("burn_object: unexpeced obj %s", xname(obj));
		break;
	}
	if (need_newsym) newsym(x, y);
}

/* lightsabers deactivate when they hit the ground/not wielded */
/* assumes caller checks for correct conditions */
void
lightsaber_deactivate (obj, timer_attached)
	struct obj *obj;
	boolean timer_attached;
{
	xchar x,y;
	char whose[BUFSZ];

	(void) Shk_Your(whose, obj);
		
	if (get_obj_location(obj, &x, &y, 0)) {
	    if (cansee(x, y)) {
		switch (obj->where) {
			case OBJ_INVENT:
			case OBJ_MINVENT:
			    pline("%s %s deactivates.",whose, xname(obj));
			    break;
			case OBJ_FLOOR:
			    You("see %s deactivate.", an(xname(obj)));
			    break;
		}
	    } else {
		You("hear a lightsaber deactivate.");
	    }
	}
	if (obj->otyp == RED_DOUBLE_LIGHTSABER || obj->otyp == CYAN_DOUBLE_LIGHTSABER || obj->otyp == LASERDENT || obj->otyp == LASERXBOW || obj->otyp == LASER_POLE || obj->otyp == SITH_STAFF || obj->otyp == WHITE_DOUBLE_LIGHTSABER) obj->altmode = FALSE;
	if ((obj == uwep) || (u.twoweap && obj != uswapwep)) unweapon = TRUE;
	end_burn(obj, timer_attached);
}

/*
 * Start a burn timeout on the given object. If not "already lit" then
 * create a light source for the vision system.  There had better not
 * be a burn already running on the object.
 *
 * Magic lamps stay lit as long as there's a genie inside, so don't start
 * a timer.
 *
 * Burn rules:
 *      torches
 *		age = # of turns of fuel left
 *		spe = <weapon plus of torch, not used here>
 *
 *	potions of oil, lamps & candles:
 *		age = # of turns of fuel left
 *		spe = <unused>
 *
 *	magic lamps:
 *		age = <unused>
 *		spe = 0 not lightable, 1 lightable forever
 *
 *	candelabrum:
 *		age = # of turns of fuel left
 *		spe = # of candles
 *
 * Once the burn begins, the age will be set to the amount of fuel
 * remaining _once_the_burn_finishes_.  If the burn is terminated
 * early then fuel is added back.
 *
 * This use of age differs from the use of age for corpses and eggs.
 * For the latter items, age is when the object was created, so we
 * know when it becomes "bad".
 *
 * This is a "silent" routine - it should not print anything out.
 */
void
begin_burn(obj, already_lit)
	struct obj *obj;
	boolean already_lit;
{
	int radius = 3;
	long turns = 0;
	boolean do_timer = TRUE;
	int lightsaberchance = 0;
	int lightsaberchance2 = 0;

	if (obj->age == 0 && obj->otyp != MAGIC_LAMP &&
		obj->otyp != MAGIC_CANDLE && !artifact_light(obj))
	    return;

	switch (obj->otyp) {
	    case MAGIC_LAMP:
	    case MAGIC_CANDLE:
		obj->lamplit = 1;
		do_timer = FALSE;
		if (obj->otyp == MAGIC_CANDLE) obj->age = 300L;
		break;
	    case LASER_POLE:
		lightsaberchance = 0;
		lightsaberchance2 = 0;
		obj->altmode = TRUE;
	    	if (obj->altmode && obj->age > 1) {

			boolean willdrainenergy = TRUE;

			if (uarms && uarms->oartifact == ART_THERMO_NUCLEAR_CHAMBER && rn2(3)) willdrainenergy = FALSE;
			if (uarms && uarms->oartifact == ART_SUPER_ENERGY_LINES && rn2(2)) willdrainenergy = FALSE;
			if (obj->otyp == HEAVY_LASER_BALL && rn2(3)) willdrainenergy = FALSE;
			if (obj->otyp == LASER_TIN_OPENER && rn2(2)) willdrainenergy = FALSE;

			if (!PlayerCannotUseSkills && tech_inuse(T_ENERGY_CONSERVATION)) {
				switch (P_SKILL(P_MAKASHI)) {
					case P_BASIC: lightsaberchance = 1; break;
					case P_SKILLED: lightsaberchance = 3; break;
					case P_EXPERT: lightsaberchance = 4; break;
					case P_MASTER: lightsaberchance = 5; break;
					case P_GRAND_MASTER: lightsaberchance = 7; break;
					case P_SUPREME_MASTER: lightsaberchance = 8; break;
				}
			}
			if (!PlayerCannotUseSkills && obj && obj->oartifact == ART_SERIALSABER) {
				switch (P_SKILL(P_MAKASHI)) {
					case P_BASIC: lightsaberchance2 = 1; break;
					case P_SKILLED: lightsaberchance2 = 3; break;
					case P_EXPERT: lightsaberchance2 = 4; break;
					case P_MASTER: lightsaberchance2 = 5; break;
					case P_GRAND_MASTER: lightsaberchance2 = 7; break;
					case P_SUPREME_MASTER: lightsaberchance2 = 8; break;
				}
			}
			if (willdrainenergy && (rnd(10) > lightsaberchance) && (rnd(10) > lightsaberchance2)) obj->age--; /* Double power usage */

		}
	    	turns = 1;
    	    	radius = 1;
		if (obj->oartifact == ART_LIGHTSABER_PROTOTYPE){
			do_timer = FALSE;
			obj->lamplit = 1;
		}
		break;
	    case RED_DOUBLE_LIGHTSABER:
	    case CYAN_DOUBLE_LIGHTSABER:
	    case LASERDENT:
	    case LASERXBOW:
	    case SITH_STAFF:
	    case WHITE_DOUBLE_LIGHTSABER:

	    {

		boolean willdrainenergy = TRUE;

		if (uarms && uarms->oartifact == ART_THERMO_NUCLEAR_CHAMBER && rn2(3)) willdrainenergy = FALSE;
		if (uarms && uarms->oartifact == ART_SUPER_ENERGY_LINES && rn2(2)) willdrainenergy = FALSE;
		if (obj->otyp == HEAVY_LASER_BALL && rn2(3)) willdrainenergy = FALSE;
		if (obj->otyp == LASER_TIN_OPENER && rn2(2)) willdrainenergy = FALSE;

		lightsaberchance = 0;
	    	if (obj->altmode && obj->age > 1) {
			if (!PlayerCannotUseSkills && tech_inuse(T_ENERGY_CONSERVATION)) {
				switch (P_SKILL(P_MAKASHI)) {
					case P_BASIC: lightsaberchance = 1; break;
					case P_SKILLED: lightsaberchance = 3; break;
					case P_EXPERT: lightsaberchance = 4; break;
					case P_MASTER: lightsaberchance = 5; break;
					case P_GRAND_MASTER: lightsaberchance = 7; break;
					case P_SUPREME_MASTER: lightsaberchance = 8; break;
				}
			}
			if (!PlayerCannotUseSkills && obj && obj->oartifact == ART_SERIALSABER) {
				switch (P_SKILL(P_MAKASHI)) {
					case P_BASIC: lightsaberchance2 = 1; break;
					case P_SKILLED: lightsaberchance2 = 3; break;
					case P_EXPERT: lightsaberchance2 = 4; break;
					case P_MASTER: lightsaberchance2 = 5; break;
					case P_GRAND_MASTER: lightsaberchance2 = 7; break;
					case P_SUPREME_MASTER: lightsaberchance2 = 8; break;
				}
			}
			if (willdrainenergy && (rnd(10) > lightsaberchance) && (rnd(10) > lightsaberchance2)) obj->age--; /* Double power usage */
		}
	    }
	    case RED_LIGHTSABER:
	    case LASER_SWATTER:
	    case LASER_SWORD:
	    case BEAMSWORD:
	    case NANO_HAMMER:
	    case KLIUSLING:
	    case STARWARS_MACE:
	    case LIGHTWHIP:
	    case ELECTRIC_CIGARETTE:
	    case BLUE_LIGHTSABER:
	    case LASERFIST:
	    case LASER_TIN_OPENER:
	    case HEAVY_LASER_BALL:
	    case LASER_CHAIN:
	    case MYSTERY_LIGHTSABER:
	    case VIOLET_LIGHTSABER:
	    case WHITE_LIGHTSABER:
	    case YELLOW_LIGHTSABER:
	    case GREEN_LIGHTSABER:
	    case ORANGE_LIGHTSABER:
	    case BLACK_LIGHTSABER:
	    	turns = 1;
    	    	radius = 1;
		if (obj->oartifact == ART_LIGHTSABER_PROTOTYPE){
			do_timer = FALSE;
			obj->lamplit = 1;
		}
		break;
	    case POT_OIL:
		turns = obj->age;
		radius = 1;	/* very dim light */
		break;
	    case STICK_OF_DYNAMITE:
		turns = obj->age;
		radius = 1;     /* very dim light */
		break;

	    case BRASS_LANTERN:
	    case OIL_LAMP:
	    case TORCH:
		/* magic times are 150, 100, 50, 25, and 0 */
		if (obj->age > 150L)
		    turns = obj->age - 150L;
		else if (obj->age > 100L)
		    turns = obj->age - 100L;
		else if (obj->age > 50L)
		    turns = obj->age - 50L;
		else if (obj->age > 25L)
		    turns = obj->age - 25L;
		else
		    turns = obj->age;
		break;

	    case CANDELABRUM_OF_INVOCATION:
	    case TALLOW_CANDLE:
	    case WAX_CANDLE:
	    case JAPAN_WAX_CANDLE:
	    case OIL_CANDLE:
	    case UNAFFECTED_CANDLE:
	    case SPECIFIC_CANDLE:
	    case __CANDLE:
	    case NATURAL_CANDLE:
	    case UNSPECIFIED_CANDLE:
	    case GENERAL_CANDLE:
		/* magic times are 75, 15, and 0 */
		if (obj->age > 75L)
		    turns = obj->age - 75L;
		else if (obj->age > 15L)
		    turns = obj->age - 15L;
		else
		    turns = obj->age;
		radius = candle_light_range(obj);
		break;

	    default:
                /* [ALI] Support artifact light sources */
		if (obj->oartifact && artifact_light(obj)) {
		    obj->lamplit = 1;
		    do_timer = FALSE;
		    radius = 2;
		}
		else {
		    impossible("begin burn: unexpected %s", xname(obj));
		    turns = obj->age;
		}
		break;
	}

	if (do_timer) {
	    if (start_timer(turns, TIMER_OBJECT,
					BURN_OBJECT, (void *)obj)) {
		obj->lamplit = 1;
		if (obj->otyp == LASER_POLE) obj->altmode = 1;
		lightsaberchance = 0;

		boolean willdrainenergy = TRUE;

		if (uarms && uarms->oartifact == ART_THERMO_NUCLEAR_CHAMBER && rn2(3)) willdrainenergy = FALSE;
		if (uarms && uarms->oartifact == ART_SUPER_ENERGY_LINES && rn2(2)) willdrainenergy = FALSE;
		if (obj->otyp == HEAVY_LASER_BALL && rn2(3)) willdrainenergy = FALSE;
		if (obj->otyp == LASER_TIN_OPENER && rn2(2)) willdrainenergy = FALSE;
		if (tech_inuse(T_PIRATE_BROTHERING) && uwep && is_lightsaber(uwep) && uswapwep && weapon_type(uswapwep) == P_SCIMITAR && u.twoweap) {
			willdrainenergy = FALSE;
		}

		if (!PlayerCannotUseSkills && tech_inuse(T_ENERGY_CONSERVATION) && obj && is_lightsaber(obj)) {
			switch (P_SKILL(P_MAKASHI)) {
				case P_BASIC: lightsaberchance = 1; break;
				case P_SKILLED: lightsaberchance = 3; break;
				case P_EXPERT: lightsaberchance = 4; break;
				case P_MASTER: lightsaberchance = 5; break;
				case P_GRAND_MASTER: lightsaberchance = 7; break;
				case P_SUPREME_MASTER: lightsaberchance = 8; break;
			}
		}
		if (!PlayerCannotUseSkills && obj && obj->oartifact == ART_SERIALSABER) {
			switch (P_SKILL(P_MAKASHI)) {
				case P_BASIC: lightsaberchance2 = 1; break;
				case P_SKILLED: lightsaberchance2 = 3; break;
				case P_EXPERT: lightsaberchance2 = 4; break;
				case P_MASTER: lightsaberchance2 = 5; break;
				case P_GRAND_MASTER: lightsaberchance2 = 7; break;
				case P_SUPREME_MASTER: lightsaberchance2 = 8; break;
			}
		}

		if (willdrainenergy && (rnd(10) > lightsaberchance) && (rnd(10) > lightsaberchance2)) obj->age -= turns;
		if (carried(obj) && !already_lit)
		    update_inventory();
	    } else {
		obj->lamplit = 0;
		/* double lightsaber should have its second blade turned off too! --Amy */
		if (obj->otyp == RED_DOUBLE_LIGHTSABER || obj->otyp == CYAN_DOUBLE_LIGHTSABER || obj->otyp == LASERDENT || obj->otyp == LASERXBOW || obj->otyp == LASER_POLE || obj->otyp == SITH_STAFF || obj->otyp == WHITE_DOUBLE_LIGHTSABER) {
			obj->altmode = FALSE;
		}
	    }
	} else {
	    if (carried(obj) && !already_lit)
		update_inventory();
	}

	if (obj->lamplit && !already_lit) {
	    xchar x, y;

	    if (get_obj_location(obj, &x, &y, CONTAINED_TOO|BURIED_TOO))
		new_light_source(x, y, radius, LS_OBJECT, (void *) obj);
	    else
		if (wizard) impossible("begin_burn: can't get obj position");
	}
}

/*
 * Stop a burn timeout on the given object if timer attached.  Darken
 * light source.
 */
void
end_burn(obj, timer_attached)
	struct obj *obj;
	boolean timer_attached;
{
	if (!obj->lamplit) {
		/* This shit was causing "heisensegfaults" (phantom crash bugs) on the server.
		 * Apparently, even with obj defined, xname doesn't always work correctly. Trim the message then. --Amy */
	    impossible("end_burn: object not lit");
	    return;
	}

	if (obj->otyp == MAGIC_LAMP || obj->otyp == MAGIC_CANDLE || obj->oartifact == ART_LIGHTSABER_PROTOTYPE || artifact_light(obj))
	    timer_attached = FALSE;

	if (!timer_attached) {
	    /* [DS] Cleanup explicitly, since timer cleanup won't happen */
	    del_light_source(LS_OBJECT, (void *)obj);
	    obj->lamplit = 0;
	    /* caller doesn't always make sure that double lightsabers are turned off properly; do so here */
	    if (obj->otyp == RED_DOUBLE_LIGHTSABER || obj->otyp == CYAN_DOUBLE_LIGHTSABER || obj->otyp == LASERDENT || obj->otyp == LASERXBOW || obj->otyp == LASER_POLE || obj->otyp == SITH_STAFF || obj->otyp == WHITE_DOUBLE_LIGHTSABER) {
			obj->altmode = FALSE;
		}
	    if (obj->where == OBJ_INVENT)
		update_inventory();
	} else if (!stop_timer(BURN_OBJECT, (void *) obj))
	    impossible("end_burn: object not timed!");
}

#endif /* OVL1 */
#ifdef OVL0

/*
 * Cleanup a burning object if timer stopped.
 */
static void
cleanup_burn(arg, expire_time)
    void * arg;
    long expire_time;
{
    struct obj *obj = (struct obj *)arg;
    if (!obj->lamplit) {
	impossible("cleanup_burn: obj not lit");
	return;
    }

    del_light_source(LS_OBJECT, arg);

    /* restore unused time */
    obj->age += expire_time - monstermoves;

    obj->lamplit = 0;
    if (obj->otyp == RED_DOUBLE_LIGHTSABER || obj->otyp == CYAN_DOUBLE_LIGHTSABER || obj->otyp == LASERDENT || obj->otyp == LASERXBOW || obj->otyp == LASER_POLE || obj->otyp == SITH_STAFF || obj->otyp == WHITE_DOUBLE_LIGHTSABER) {
	obj->altmode = FALSE;
    }

    if (obj->where == OBJ_INVENT)
	update_inventory();
}

#endif /* OVL0 */
#ifdef OVL1

/* 
 * MRKR: Use up some fuel quickly, eg: when hitting a monster with 
 *       a torch.
 */

void 
burn_faster(obj, adj) 
struct obj *obj;
long adj;
{

  if (!obj->lamplit) {
    impossible("burn_faster: obj not lit");
    return;
  }

  accelerate_timer(BURN_OBJECT, obj, adj);
}

void
do_storms()
{
    int nstrike;
    register int x, y;
    int dirx, diry;
    int count;

    /* no lightning if not the air level or too often, even then */
    if(!Is_airlevel(&u.uz) || rn2(8))
	return;

    /* the number of strikes is 8-log2(nstrike) */
    for(nstrike = rnd(64); nstrike <= 64; nstrike *= 2) {
	count = 0;
	do {
	    x = rnd(COLNO-1);
	    y = rn2(ROWNO);
	} while (++count < 100 && levl[x][y].typ != CLOUD);

	if(count < 100) {
	    dirx = rn2(3) - 1;
	    diry = rn2(3) - 1;
	    if(dirx != 0 || diry != 0)
		buzz(-15, /* "monster" LIGHTNING spell */
		     8, x, y, dirx, diry);
	}
    }

    if(levl[u.ux][u.uy].typ == CLOUD) {
	/* inside a cloud during a thunder storm is deafening */
	pline("Kaboom!!!  Boom!!  Boom!!");
	if(!u.uinvulnerable) {
	    stop_occupation();
	    nomul(-3, "hiding from thunderstorm", TRUE);
	    nomovemsg = 0;
	}
    } else
	You_hear("a rumbling noise.");
}
#endif /* OVL1 */


#ifdef OVL0
/* ------------------------------------------------------------------------- */
/*
 * Generic Timeout Functions.
 *
 * Interface:
 *
 * General:
 *	boolean start_timer(long timeout,short kind,short func_index,
 *							void * arg)
 *		Start a timer of kind 'kind' that will expire at time
 *		monstermoves+'timeout'.  Call the function at 'func_index'
 *		in the timeout table using argument 'arg'.  Return TRUE if
 *		a timer was started.  This places the timer on a list ordered
 *		"sooner" to "later".  If an object, increment the object's
 *		timer count.
 *
 *	long stop_timer(short func_index, void * arg)
 *		Stop a timer specified by the (func_index, arg) pair.  This
 *		assumes that such a pair is unique.  Return the time the
 *		timer would have gone off.  If no timer is found, return 0.
 *		If an object, decrement the object's timer count.
 *
 *	void run_timers(void)
 *		Call timers that have timed out.
 *
 *
 * Save/Restore:
 *	void save_timers(int fd, int mode, int range)
 *		Save all timers of range 'range'.  Range is either global
 *		or local.  Global timers follow game play, local timers
 *		are saved with a level.  Object and monster timers are
 *		saved using their respective id's instead of pointers.
 *
 *	void restore_timers(int fd, int range, boolean ghostly, long adjust)
 *		Restore timers of range 'range'.  If from a ghost pile,
 *		adjust the timeout by 'adjust'.  The object and monster
 *		ids are not restored until later.
 *
 *	void relink_timers(boolean ghostly)
 *		Relink all object and monster timers that had been saved
 *		using their object's or monster's id number.
 *
 * Object Specific:
 *	void obj_move_timers(struct obj *src, struct obj *dest)
 *		Reassign all timers from src to dest.
 *
 *	void obj_split_timers(struct obj *src, struct obj *dest)
 *		Duplicate all timers assigned to src and attach them to dest.
 *
 *	void obj_stop_timers(struct obj *obj)
 *		Stop all timers attached to obj.
 *
 * Monster Specific:
 *	void mon_stop_timers(struct monst *mon)
 *		Stop all timers attached to mon.
 */

#ifdef WIZARD
STATIC_DCL const char *kind_name(SHORT_P);
STATIC_DCL void print_queue(winid, timer_element *);
#endif
STATIC_DCL void insert_timer(timer_element *);
STATIC_DCL timer_element *remove_timer(timer_element **, SHORT_P, void *);
STATIC_DCL void write_timer(int, timer_element *);
STATIC_DCL boolean mon_is_local(struct monst *);
STATIC_DCL boolean timer_is_local(timer_element *);
STATIC_DCL int maybe_write_timer(int, int, BOOLEAN_P);
static void write_timer(int, timer_element *); /* Damn typedef write_timer is in the middle */

/* ordered timer list */
static timer_element *timer_base;		/* "active" */
static unsigned long timer_id = 1;

/* If defined, then include names when printing out the timer queue */
#define VERBOSE_TIMER

typedef struct {
    timeout_proc f, cleanup;
#ifdef VERBOSE_TIMER
    const char *name;
# define TTAB(a, b, c) {a,b,c}
#else
# define TTAB(a, b, c) {a,b}
#endif
} ttable;

/* table of timeout functions */
static const ttable timeout_funcs[NUM_TIME_FUNCS] = {
    TTAB(rot_organic,	(timeout_proc)0,	"rot_organic"),
    TTAB(rot_corpse,	(timeout_proc)0,	"rot_corpse"),
    TTAB(moldy_corpse,  (timeout_proc)0,	"moldy_corpse"),
    TTAB(revive_mon,	(timeout_proc)0,	"revive_mon"),
    TTAB(burn_object,	cleanup_burn,		"burn_object"),
    TTAB(hatch_egg,	(timeout_proc)0,	"hatch_egg"),
    TTAB(fig_transform, (timeout_proc)0,	"fig_transform"),
    TTAB(unpoly_mon,    (timeout_proc)0,	"unpoly_mon"),
    TTAB(bomb_blow,     (timeout_proc)0,	"bomb_blow"),
    TTAB(unpoly_obj,    cleanup_unpoly,		"unpoly_obj"),
};
#undef TTAB


#if defined(WIZARD)

STATIC_OVL const char *
kind_name(kind)
    short kind;
{
    switch (kind) {
	case TIMER_LEVEL: return "level";
	case TIMER_GLOBAL: return "global";
	case TIMER_OBJECT: return "object";
	case TIMER_MONSTER: return "monster";
    }
    return "unknown";
}

STATIC_OVL void
print_queue(win, base)
    winid win;
    timer_element *base;
{
    timer_element *curr;
    char buf[BUFSZ], arg_address[20];

    if (!base) {
	putstr(win, 0, "<empty>");
    } else {
	putstr(win, 0, "timeout  id   kind   call");
	for (curr = base; curr; curr = curr->next) {
#ifdef VERBOSE_TIMER
	    sprintf(buf, " %4ld   %4ld  %-6s %s(%s)",
		curr->timeout, curr->tid, kind_name(curr->kind),
		timeout_funcs[curr->func_index].name,
		fmt_ptr((void *)curr->arg, arg_address));
#else
	    sprintf(buf, " %4ld   %4ld  %-6s #%d(%s)",
		curr->timeout, curr->tid, kind_name(curr->kind),
		curr->func_index,
		fmt_ptr((void *)curr->arg, arg_address));
#endif
	    putstr(win, 0, buf);
	}
    }
}

int
wiz_timeout_queue()
{
    winid win;
    char buf[BUFSZ];

    win = create_nhwindow(NHW_MENU);	/* corner text window */
    if (win == WIN_ERR) return 0;

    sprintf(buf, "Current time = %ld.", monstermoves);
    putstr(win, 0, buf);
    putstr(win, 0, "");
    putstr(win, 0, "Active timeout queue:");
    putstr(win, 0, "");
    print_queue(win, timer_base);

    display_nhwindow(win, FALSE);
    destroy_nhwindow(win);

    return 0;
}

void
cryogenics()
{
	timer_element *curr;
	int number;

	for (curr = timer_base; curr; curr = curr->next) {

		if (curr && curr->kind == TIMER_OBJECT && (curr->func_index == ROT_CORPSE || curr->func_index == MOLDY_CORPSE || curr->func_index == REVIVE_MON || curr->func_index == HATCH_EGG) ) {
			number++;
			curr->timeout += rnd(50);
		}
	}

	if (number) pline("Corpses and eggs have been cryogenized.");
	else pline("There's nothing that can be cryogenized.");

}

void
timer_sanity_check()
{
    timer_element *curr;
    char obj_address[20];

    /* this should be much more complete */
    for (curr = timer_base; curr; curr = curr->next)
	if (curr->kind == TIMER_OBJECT) {
	    struct obj *obj = (struct obj *) curr->arg;
	    if (obj->timed == 0) {
		pline("timer sanity: untimed obj %s, timer %ld",
		      fmt_ptr((void *)obj, obj_address), curr->tid);
	    }
	}
}

#endif /* WIZARD */


/*
 * Pick off timeout elements from the global queue and call their functions.
 * Do this until their time is less than or equal to the move count.
 */
void
run_timers()
{
    timer_element *curr;

    /*
     * Always use the first element.  Elements may be added or deleted at
     * any time.  The list is ordered, we are done when the first element
     * is in the future.
     */
    while (timer_base && timer_base->timeout <= monstermoves) {
	curr = timer_base;
	timer_base = curr->next;

	if (curr->kind == TIMER_OBJECT) {

		if (!((struct obj *)curr->arg)) {
			impossible("ERROR! run_timers %s - object does not exist", timeout_funcs[curr->func_index].name);

			free((void *) curr);
			return;
		}

		((struct obj *)(curr->arg))->timed--;
	}

	(*timeout_funcs[curr->func_index].f)(curr->arg, curr->timeout);
	free((void *) curr);
    }
}


/*
 * Start a timer.  Return TRUE if successful.
 */
boolean
start_timer(when, kind, func_index, arg)
long when;
short kind;
short func_index;
void * arg;
{
    timer_element *gnu;

    if (func_index < 0 || func_index >= NUM_TIME_FUNCS) {
	panic("start_timer");
	}

    gnu = (timer_element *) alloc(sizeof(timer_element));
    gnu->next = 0;
    gnu->tid = timer_id++;
    gnu->timeout = monstermoves + when;
    gnu->kind = kind;
    gnu->needs_fixup = 0;
    gnu->func_index = func_index;
    gnu->arg = arg;
    insert_timer(gnu);

    if (kind == TIMER_OBJECT)	/* increment object's timed count */
	((struct obj *)arg)->timed++;

    /* should check for duplicates and fail if any */
    return TRUE;
}


/*
 * Remove the timer from the current list and free it up.  Return the time
 * it would have gone off, 0 if not found.
 */
long
stop_timer(func_index, arg)
short func_index;
void * arg;
{
    timer_element *doomed;
    long timeout;

    doomed = remove_timer(&timer_base, func_index, arg);

    if (doomed) {
	timeout = doomed->timeout;
	if (doomed->kind == TIMER_OBJECT)
	    ((struct obj *)arg)->timed--;
	if (timeout_funcs[doomed->func_index].cleanup)
	    (*timeout_funcs[doomed->func_index].cleanup)(arg, timeout);
	free((void *) doomed);
	return timeout;
    }
    return 0;
}


/*
 * Move all object timers from src to dest, leaving src untimed.
 */
void
obj_move_timers(src, dest)
    struct obj *src, *dest;
{
    int count;
    timer_element *curr;

    for (count = 0, curr = timer_base; curr; curr = curr->next)
	if (curr->kind == TIMER_OBJECT && curr->arg == (void *)src) {
	    curr->arg = (void *) dest;
	    dest->timed++;
	    count++;
	}
    if (count != src->timed) {
	panic("obj_move_timers");
	}
    src->timed = 0;
}


/*
 * Find all object timers and duplicate them for the new object "dest".
 */
void
obj_split_timers(src, dest)
    struct obj *src, *dest;
{
    timer_element *curr, *next_timer=0;

    for (curr = timer_base; curr; curr = next_timer) {
	next_timer = curr->next;	/* things may be inserted */
	if (curr->kind == TIMER_OBJECT && curr->arg == (void *)src) {
	    (void) start_timer(curr->timeout-monstermoves, TIMER_OBJECT,
					curr->func_index, (void *)dest);
	}
    }
}


/*
 * Stop all timers attached to this object.  We can get away with this because
 * all object pointers are unique.
 */
void
obj_stop_timers(obj)
    struct obj *obj;
{
    timer_element *curr, *prev, *next_timer=0;

    for (prev = 0, curr = timer_base; curr; curr = next_timer) {
	next_timer = curr->next;
	if (curr->kind == TIMER_OBJECT && curr->arg == (void *)obj) {
	    if (prev)
		prev->next = curr->next;
	    else
		timer_base = curr->next;
	    if (timeout_funcs[curr->func_index].cleanup)
		(*timeout_funcs[curr->func_index].cleanup)(curr->arg,
			curr->timeout);
	    free((void *) curr);
	} else {
	    prev = curr;
	}
    }
    obj->timed = 0;
}

/*
 * Stop all timers attached to this monster.  We can get away with this because
 * all monster pointers are unique.
 */
void
mon_stop_timers(mon)
    struct monst *mon;
{
    timer_element *curr, *prev, *next_timer=0;

    for (prev = 0, curr = timer_base; curr; curr = next_timer) {
	next_timer = curr->next;
	if (curr->kind == TIMER_MONSTER && curr->arg == (void *)mon) {
	    if (prev)
		prev->next = curr->next;
	    else
		timer_base = curr->next;
	    if (timeout_funcs[curr->func_index].cleanup)
		(*timeout_funcs[curr->func_index].cleanup)(curr->arg,
			curr->timeout);
	    free((void *) curr);
	} else {
	    prev = curr;
	}
    }
}


/* Insert timer into the global queue */
STATIC_OVL void
insert_timer(gnu)
    timer_element *gnu;
{
    timer_element *curr, *prev;

    for (prev = 0, curr = timer_base; curr; prev = curr, curr = curr->next)
	if (curr->timeout >= gnu->timeout) break;

    gnu->next = curr;
    if (prev)
	prev->next = gnu;
    else
	timer_base = gnu;
}


STATIC_OVL timer_element *
remove_timer(base, func_index, arg)
timer_element **base;
short func_index;
void * arg;
{
    timer_element *prev, *curr;

    for (prev = 0, curr = *base; curr; prev = curr, curr = curr->next)
	if (curr->func_index == func_index && curr->arg == arg) break;

    if (curr) {
	if (prev)
	    prev->next = curr->next;
	else
	    *base = curr->next;
    }

    return curr;
}

STATIC_OVL void
write_timer(fd, timer)
    int fd;
    timer_element *timer;
{
    void * arg_save;

    switch (timer->kind) {
	case TIMER_GLOBAL:
	case TIMER_LEVEL:
	    /* assume no pointers in arg */
	    bwrite(fd, (void *) timer, sizeof(timer_element));
	    break;

	case TIMER_OBJECT:
	    if (timer->needs_fixup)
		bwrite(fd, (void *)timer, sizeof(timer_element));
	    else {
		/* replace object pointer with id */
		arg_save = timer->arg;
		if (!((struct obj *)timer->arg)) {
			impossible("ERROR! write_timer %s - object does not exist", timeout_funcs[timer->func_index].name);

			/* the dreaded "segfault panic" bug... apparently we need to write the timer anyway,
			 * because if we don't, we produce a corrupted savegame --Amy
			 * set the timeout to occur on the next turn to ensure the bugged timer is removed ASAP */
			timer->timeout = (moves + 1);
			bwrite(fd, (void *)timer, sizeof(timer_element));
			break;
		}

		timer->arg = (void *)((struct obj *)timer->arg)->o_id;
		timer->needs_fixup = 1;
		bwrite(fd, (void *)timer, sizeof(timer_element));
		timer->arg = arg_save;
		timer->needs_fixup = 0;
	    }
	    break;

	case TIMER_MONSTER:
	    if (timer->needs_fixup)
		bwrite(fd, (void *)timer, sizeof(timer_element));
	    else {
		/* replace monster pointer with id */
		arg_save = timer->arg;
		timer->arg = (void *)((struct monst *)timer->arg)->m_id;
		timer->needs_fixup = 1;
		bwrite(fd, (void *)timer, sizeof(timer_element));
		timer->arg = arg_save;
		timer->needs_fixup = 0;
	    }
	    break;

	default:
	    panic("write_timer");
	    break;
    }
}

/*
 * MRKR: Run one particular timer faster for a number of steps
 *       Needed for burn_faster above.
 */

STATIC_OVL void
accelerate_timer(func_index, arg, adj) 
short func_index;
void * arg;
long adj;
{ 
    timer_element *timer;

    /* This will effect the ordering, so we remove it from the list */
    /* and add it back in afterwards (if warranted) */

    timer = remove_timer(&timer_base, func_index, arg);    

    for (; adj > 0; adj--) {
      timer->timeout--;

      if (timer->timeout <= monstermoves) {
	if (timer->kind == TIMER_OBJECT) ((struct obj *)arg)->timed--;
	(*timeout_funcs[func_index].f)(arg, timer->timeout);
	free((void *) timer);
	break;
      }
    }

    if (adj == 0)
      insert_timer(timer);
}

/*
 * Return TRUE if the object will stay on the level when the level is
 * saved.
 */
boolean
obj_is_local(obj)
    struct obj *obj;
{
	if (!obj) {
		impossible("obj_is_local: object does not exist");
		return FALSE;
	}

    switch (obj->where) {
	case OBJ_INVENT:
	case OBJ_MIGRATING:	return FALSE;
	case OBJ_FLOOR:
	case OBJ_BURIED:	return TRUE;
	case OBJ_CONTAINED:	return obj_is_local(obj->ocontainer);
	case OBJ_MINVENT:	return mon_is_local(obj->ocarry);
    }
	impossible("obj is local panic - CRITICAL BUG, YOU SHOULD SAVE THE GAME RIGHT AWAY");
	/*pline("obj location %d", obj->where);*/
	/*if (issegfaulter) && obj) pline("obj in question: %s", cxname(obj));*/
	/* the above line was sometimes causing a segfault in xname2() even if obj existed... */

      /*if (issegfaulter && strncmpi(plname, "BSOD2", 5)) panic("obj_is_local: %s, %d", cxname(obj), obj->where); */ /* improvement by Patric Mueller */

	/* This is the "segfault panic", "hallucination bug" and "timed object bug" that has been plaguing SLEX for a while.
	 * Due to some weirdness, the actual panic message almost never displays, making it next to impossible
	 * to diagnose the bug - maybe because obj is undefined and the cxname() function thus causes a segfault?
	 * I've removed the panic now, and write_timer is changed to try to salvage the bugged timer by making it time out
	 * on the next turn. Wizmode testing with the segfaulter race shows that it does in fact work!
	 * The segfaulter race should also no longer cause segfault panics on purpose. If for any reason we need to
	 * do crash tests, the above line can be re-enabled to make it crash again. This has the advantage
	 * of allowing me to test the game's behavior in the case of a segfault, similar to a missingno, except that the
	 * latter usually leaves a recoverable savegame file. Producing segfaults on purpose is otherwise actually
	 * not all that easy! --Amy */

	/* The BSOD2 check is an ugly kludge that shall serve as a way for me to run tests on the hardfought server --Amy */

	/* update: The root of that bug was in objnam.c, hallucination would not check whether a hallucinated tool
	 * is free of contents. The eggs or whatever was in those containers would get initialized with timers,
	 * then the container holding them would disappear, but the egg timer wasn't deleted, leading to crashes.
	 * It has now been fixed, but just in case there are any other obj_is_local errors, the above fixes are preserved. */

    return FALSE;
}


/*
 * Return TRUE if the given monster will stay on the level when the
 * level is saved.
 */
STATIC_OVL boolean
mon_is_local(mon)
struct monst *mon;
{
    struct monst *curr;

    for (curr = migrating_mons; curr; curr = curr->nmon)
	if (curr == mon) return FALSE;
    /* `mydogs' is used during level changes, never saved and restored */
    for (curr = mydogs; curr; curr = curr->nmon)
	if (curr == mon) return FALSE;
    return TRUE;
}


/*
 * Return TRUE if the timer is attached to something that will stay on the
 * level when the level is saved.
 */
STATIC_OVL boolean
timer_is_local(timer)
    timer_element *timer;
{
    switch (timer->kind) {
	case TIMER_LEVEL:	return TRUE;
	case TIMER_GLOBAL:	return FALSE;
	case TIMER_OBJECT:	return obj_is_local((struct obj *)timer->arg);
	case TIMER_MONSTER:	return mon_is_local((struct monst *)timer->arg);
    }
    panic("timer_is_local");
    return FALSE;
}


/*
 * Part of the save routine.  Count up the number of timers that would
 * be written.  If write_it is true, actually write the timer.
 */
STATIC_OVL int
maybe_write_timer(fd, range, write_it)
    int fd, range;
    boolean write_it;
{
    int count = 0;
    timer_element *curr;

    for (curr = timer_base; curr; curr = curr->next) {
	if (range == RANGE_GLOBAL) {
	    /* global timers */

	    if (!timer_is_local(curr)) {
		count++;
		if (write_it) write_timer(fd, curr);
	    }

	} else {
	    /* local timers */

	    if (timer_is_local(curr)) {
		count++;
		if (write_it) write_timer(fd, curr);
	    }

	}
    }

    return count;
}


/*
 * Save part of the timer list.  The parameter 'range' specifies either
 * global or level timers to save.  The timer ID is saved with the global
 * timers.
 *
 * Global range:
 *		+ timeouts that follow the hero (global)
 *		+ timeouts that follow obj & monst that are migrating
 *
 * Level range:
 *		+ timeouts that are level specific (e.g. storms)
 *		+ timeouts that stay with the level (obj & monst)
 */
void
save_timers(fd, mode, range)
    int fd, mode, range;
{
    timer_element *curr, *prev, *next_timer=0;
    int count;

    if (perform_bwrite(mode)) {
	if (range == RANGE_GLOBAL)
	    bwrite(fd, (void *) &timer_id, sizeof(timer_id));

	count = maybe_write_timer(fd, range, FALSE);
	bwrite(fd, (void *) &count, sizeof count);
	(void) maybe_write_timer(fd, range, TRUE);
    }

    if (release_data(mode)) {
	for (prev = 0, curr = timer_base; curr; curr = next_timer) {
	    next_timer = curr->next;	/* in case curr is removed */

	    if ( !(!!(range == RANGE_LEVEL) ^ !!timer_is_local(curr)) ) {
		if (prev)
		    prev->next = curr->next;
		else
		    timer_base = curr->next;
		free((void *) curr);
		/* prev stays the same */
	    } else {
		prev = curr;
	    }
	}
    }
}


/*
 * Pull in the structures from disk, but don't recalculate the object and
 * monster pointers.
 */
void
restore_timers(fd, range, ghostly, adjust)
    int fd, range;
    boolean ghostly;	/* restoring from a ghost level */
    long adjust;	/* how much to adjust timeout */
{
    int count;
    timer_element *curr;

    if (range == RANGE_GLOBAL)
	mread(fd, (void *) &timer_id, sizeof timer_id);

    /* restore elements */
    mread(fd, (void *) &count, sizeof count);
    while (count-- > 0) {
	curr = (timer_element *) alloc(sizeof(timer_element));
	mread(fd, (void *) curr, sizeof(timer_element));
	if (ghostly)
	    curr->timeout += adjust;
	insert_timer(curr);
    }
}


/* reset all timers that are marked for reseting */
void
relink_timers(ghostly)
    boolean ghostly;
{
    timer_element *curr;
    unsigned nid;

    for (curr = timer_base; curr; curr = curr->next) {
	if (curr->needs_fixup) {
	    if (curr->kind == TIMER_OBJECT) {
		if (ghostly) {
		    if (!lookup_id_mapping((unsigned)curr->arg, &nid)) {
			panic("relink_timers 1");
			}
		} else
		    nid = (unsigned) curr->arg;
		curr->arg = (void *) find_oid(nid);
		if (!curr->arg) impossible("cant find o_id %d", nid);
		curr->needs_fixup = 0;
	    } else if (curr->kind == TIMER_MONSTER) {
/*                panic("relink_timers: no monster timer implemented");*/
                /* WAC attempt to relink monster timers based on above
                 * and light source code
                 */
		if (ghostly) {
		    if (!lookup_id_mapping((unsigned)curr->arg, &nid)) {
                        panic("relink_timers 1b");
			}
		} else
		    nid = (unsigned) curr->arg;
                curr->arg = (void *) find_mid(nid, FM_EVERYWHERE);
		if (!curr->arg) panic("cant find m_id %d", nid);
		curr->needs_fixup = 0;
	    } else {
		panic("relink_timers 2");
		}
	}
    }
}

#endif /* OVL0 */

/*timeout.c*/
