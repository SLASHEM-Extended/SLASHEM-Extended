/*	SCCS Id: @(#)timeout.c	3.4	2002/12/17	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "lev.h"	/* for checking save modes */

STATIC_DCL void NDECL(stoned_dialogue);
#ifdef CONVICT
STATIC_DCL void NDECL(phasing_dialogue);
#endif /* CONVICT */
STATIC_DCL void NDECL(vomiting_dialogue);
STATIC_DCL void NDECL(choke_dialogue);
STATIC_DCL void NDECL(slime_dialogue);
STATIC_DCL void NDECL(slime_dialogue);
STATIC_DCL void NDECL(slip_or_trip);
STATIC_DCL void FDECL(see_lamp_flicker, (struct obj *, const char *));
STATIC_DCL void FDECL(lantern_message, (struct obj *));
STATIC_DCL void FDECL(accelerate_timer, (SHORT_P, genericptr_t, long));
STATIC_DCL void FDECL(cleanup_burn, (genericptr_t,long));

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
		pline(stoned_texts[SIZE(stoned_texts) - i]);
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	if (i == 7L)
		HFast = 0L;
	if (i == 1L) {
		nomul(-3, "getting stoned");
		nomovemsg = 0;
	}
	exercise(A_DEX, FALSE);
}

#ifdef CONVICT
STATIC_OVL void
phasing_dialogue()
{
    if (Phasing == 15) {
        if (!Hallucination) {
            Your("body is beginning to feel more solid.");
        } else {
            You_feel("more distant from the spirit world.");
        }
        stop_occupation();
    } else if (Phasing == 1) {
        if (!Hallucination) {
            Your("body is solid again.");
        } else {
            You_feel("totally separated from the spirit world.");
        }
        stop_occupation();
    }
}
#endif /* CONVICT */


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
		You(vomiting_texts[SIZE(vomiting_texts) - i - 1]);

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
	"You're gasping for air.",
	"You can no longer breathe.",
	"You're turning %s.",
	"You suffocate."
};

static NEARDATA const char * const choke_texts2[] = {
	"Your %s is becoming constricted.",
	"Your blood is having trouble reaching your brain.",
	"The pressure on your %s increases.",
	"Your consciousness is fading.",
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
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		if (index(str, '%')) {
		    pline(str, hcolor(NH_BLUE));
			display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */ }
		else
		    pline(str);
			display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
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
			display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		} else
		    pline(str, an(Hallucination ? rndmonnam() : "green slime"));
			display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    } else
		pline(str);
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	}
	if (i == 3L) {	/* limbs becoming oozy */
	    HFast = 0L;	/* lose intrinsic speed */
	    stop_occupation();
	    if (multi > 0) nomul(0, 0);
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
	if (Race_if(PM_WARPER) && moves % 10000 == 0) u.youpolyamount++;
	if (Race_if(PM_DOPPELGANGER) && moves % 5000 == 0) u.youpolyamount++;
	if (Role_if(PM_SHAPESHIFTER) && moves % 5000 == 0) u.youpolyamount++;
	if (Race_if(PM_HEMI_DOPPELGANGER) && moves % 4000 == 0) u.youpolyamount++;
	if (Race_if(PM_DEATHMOLD) && moves % 3000 == 0) u.youpolyamount++;
	if (Race_if(PM_UNGENOMOLD) && moves % 2000 == 0) u.youpolyamount++;
	if (Race_if(PM_MOULD) && moves % 1000 == 0) u.youpolyamount++;

	if (MCReduction && MCReduction % 5000 == 0) pline("Your magic cancellation seems to work a bit better again.");

	if (u.negativeprotection && !rn2(2500)) u.negativeprotection--;
	if (u.chokhmahdamage && !rn2(10000)) u.chokhmahdamage--;

	if (u.inertia && rn2(10)) {
		u.inertia--;
		if (!u.inertia) pline("You feel less slow.");
	}

	if (u.fumbleduration) u.fumbleduration--;
	if (u.antimagicshell) {
		u.antimagicshell--;
		if (!u.antimagicshell) pline("You no longer produce an anti-magic shell.");
	}

	if (u.legscratching > 1 && !Role_if(PM_BLEEDER) && !Race_if(PM_HEMOPHAGE) && !BloodLossProblem && !have_bloodlossstone() && !u.uprops[BLOOD_LOSS].extrinsic && moves % 1000 == 0) u.legscratching--; /* always time out once per 1000 turns --Amy */

	if (!rn2(1000) && (Role_if(PM_ACTIVISTOR) || Race_if(PM_PEACEMAKER) ) && ( !( uarmu && (uarmu->otyp == RUFFLED_SHIRT || uarmu->otyp == VICTORIAN_UNDERWEAR)) || !rn2(10)) ) {
		You_hear("maniacal laughter!");
	    attrcurse();
	}

	if (!rn2(1000) && u.uprops[INTRINSIC_LOSS].extrinsic && ( !( uarmu && (uarmu->otyp == RUFFLED_SHIRT || uarmu->otyp == VICTORIAN_UNDERWEAR)) || !rn2(10)) ) {
		You_hear("maniacal laughter!");
	    attrcurse();
	}

	if (!rn2(1000) && IntrinsicLossProblem && ( !( uarmu && (uarmu->otyp == RUFFLED_SHIRT || uarmu->otyp == VICTORIAN_UNDERWEAR)) || !rn2(10)) ) {
		You_hear("maniacal laughter!");
	    attrcurse();
	}

	if (!rn2(1000) && have_intrinsiclossstone() && ( !( uarmu && (uarmu->otyp == RUFFLED_SHIRT || uarmu->otyp == VICTORIAN_UNDERWEAR)) || !rn2(10)) ) {
		You_hear("maniacal laughter!");
	    attrcurse();
	}

	if (u.hanguppenalty > 0) {

		u.hanguppenalty--;

	}

	if (Race_if(PM_WEAPON_TRAPPER)) { /* they know about the existence of traps --Amy */

	    struct trap *t;

	    for (t = ftrap; t != 0; t = t->ntrap) {
		if (!rn2(1000) && !t->tseen && !t->hiddentrap) {
			t->tseen = 1;
			map_trap(t, TRUE);
		}
	    }

	}

	if (!rn2(200) && u.uprops[BAD_EFFECTS].extrinsic) badeffect();

	if (!rn2(200) && BadEffectProblem) badeffect();

	if (!rn2(200) && have_badeffectstone() ) badeffect();

	if (!rn2(100) && u.uprops[RANDOM_RUMORS].extrinsic) {
		const char *line;
		char buflin[BUFSZ];
		if (rn2(2)) line = getrumor(-1, buflin, TRUE);
		else line = getrumor(0, buflin, TRUE);
		if (!*line) line = "Slash'EM rumors file closed for renovation.";
		pline("%s", line);
	}

	if (!rn2(200) && u.uprops[AUTOMATIC_TRAP_CREATION].extrinsic) {

		makerandomtrap();

	}

	if (!rn2(200) && TrapCreationProblem) {

		makerandomtrap();

	}

	if (!rn2(200) && have_trapcreationstone() ) {

		makerandomtrap();

	}

	if (!rn2(2000) && u.uprops[AUTOMATIC_TRAP_CREATION].extrinsic) {

		makerandomtrap(); makerandomtrap(); makerandomtrap(); makerandomtrap();
		makerandomtrap(); makerandomtrap(); makerandomtrap(); makerandomtrap();

	}

	if (!rn2(2000) && TrapCreationProblem) {

		makerandomtrap(); makerandomtrap(); makerandomtrap(); makerandomtrap();
		makerandomtrap(); makerandomtrap(); makerandomtrap(); makerandomtrap();

	}

	if (!rn2(2000) && have_trapcreationstone() ) {

		makerandomtrap(); makerandomtrap(); makerandomtrap(); makerandomtrap();
		makerandomtrap(); makerandomtrap(); makerandomtrap(); makerandomtrap();

	}

	if (!rn2(1000) && u.uprops[SENTIENT_HIGH_HEELS].extrinsic) {

			switch (rnd(15)) {

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
					nomul(-(rnd(monster_difficulty() + 1)), "knocked out by their own sentient footwear" );
					exercise(A_DEX, FALSE);
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
				if (!rn2(250)) pushplayer();

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
						nomul(-rnd(5), "knocked out by a hard piece of female footwear");
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
							} else {
							    /* explicitly chose not to die;
							       arbitrarily boost intelligence */
							    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
							    You_feel("like a scarecrow.");
							    break;
							}
						    }

						    if (lifesaved)
							pline("Unfortunately your brain is still gone.");
						    else
							Your("last thought fades away.");
						    killer = "brainlessness";
						    killer_format = KILLED_BY;
						    done(DIED);
						    lifesaved++;
						}
					    }
					}
					/* adjattrib gives dunce cap message when appropriate */
					if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE);
					else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE);
					if (!rn2(3)) forget_levels(5);	/* lose memory of 25% of levels */
					if (!rn2(10)) forget_objects(5);	/* lose memory of 25% of objects */
					exercise(A_WIS, FALSE);

						}

				break;

			}

	}

	if (!rn2(250) && u.uprops[REPEATING_VULNERABILITY].extrinsic) {

		switch (rnd(117)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				u.uprops[DEAC_FIRE_RES].intrinsic += rnz(200);
				pline("You are prevented from having fire resistance!");
				break;
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				u.uprops[DEAC_COLD_RES].intrinsic += rnz(200);
				pline("You are prevented from having cold resistance!");
				break;
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				u.uprops[DEAC_SLEEP_RES].intrinsic += rnz(200);
				pline("You are prevented from having sleep resistance!");
				break;
			case 16:
			case 17:
				u.uprops[DEAC_DISINT_RES].intrinsic += rnz(200);
				pline("You are prevented from having disintegration resistance!");
				break;
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
				u.uprops[DEAC_SHOCK_RES].intrinsic += rnz(200);
				pline("You are prevented from having shock resistance!");
				break;
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
				u.uprops[DEAC_POISON_RES].intrinsic += rnz(200);
				pline("You are prevented from having poison resistance!");
				break;
			case 28:
			case 29:
			case 30:
				u.uprops[DEAC_DRAIN_RES].intrinsic += rnz(200);
				pline("You are prevented from having drain resistance!");
				break;
			case 31:
			case 32:
				u.uprops[DEAC_SICK_RES].intrinsic += rnz(200);
				pline("You are prevented from having sickness resistance!");
				break;
			case 33:
			case 34:
				u.uprops[DEAC_ANTIMAGIC].intrinsic += rnz(200);
				pline("You are prevented from having magic resistance!");
				break;
			case 35:
			case 36:
			case 37:
			case 38:
				u.uprops[DEAC_ACID_RES].intrinsic += rnz(200);
				pline("You are prevented from having acid resistance!");
				break;
			case 39:
			case 40:
				u.uprops[DEAC_STONE_RES].intrinsic += rnz(200);
				pline("You are prevented from having petrification resistance!");
				break;
			case 41:
				u.uprops[DEAC_FEAR_RES].intrinsic += rnz(200);
				pline("You are prevented from having fear resistance!");
				break;
			case 42:
			case 43:
			case 44:
				u.uprops[DEAC_SEE_INVIS].intrinsic += rnz(200);
				pline("You are prevented from having see invisible!");
				break;
			case 45:
			case 46:
			case 47:
				u.uprops[DEAC_TELEPAT].intrinsic += rnz(200);
				pline("You are prevented from having telepathy!");
				break;
			case 48:
			case 49:
			case 50:
				u.uprops[DEAC_WARNING].intrinsic += rnz(200);
				pline("You are prevented from having warning!");
				break;
			case 51:
			case 52:
			case 53:
				u.uprops[DEAC_SEARCHING].intrinsic += rnz(200);
				pline("You are prevented from having automatic searching!");
				break;
			case 54:
				u.uprops[DEAC_CLAIRVOYANT].intrinsic += rnz(200);
				pline("You are prevented from having clairvoyance!");
				break;
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
				u.uprops[DEAC_INFRAVISION].intrinsic += rnz(200);
				pline("You are prevented from having infravision!");
				break;
			case 60:
				u.uprops[DEAC_DETECT_MONSTERS].intrinsic += rnz(200);
				pline("You are prevented from having detect monsters!");
				break;
			case 61:
			case 62:
			case 63:
				u.uprops[DEAC_INVIS].intrinsic += rnz(200);
				pline("You are prevented from having invisibility!");
				break;
			case 64:
				u.uprops[DEAC_DISPLACED].intrinsic += rnz(200);
				pline("You are prevented from having displacement!");
				break;
			case 65:
			case 66:
			case 67:
				u.uprops[DEAC_STEALTH].intrinsic += rnz(200);
				pline("You are prevented from having stealth!");
				break;
			case 68:
				u.uprops[DEAC_JUMPING].intrinsic += rnz(200);
				pline("You are prevented from having jumping!");
				break;
			case 69:
			case 70:
			case 71:
				u.uprops[DEAC_TELEPORT_CONTROL].intrinsic += rnz(200);
				pline("You are prevented from having teleport control!");
				break;
			case 72:
				u.uprops[DEAC_FLYING].intrinsic += rnz(200);
				pline("You are prevented from having flying!");
				break;
			case 73:
				u.uprops[DEAC_MAGICAL_BREATHING].intrinsic += rnz(200);
				pline("You are prevented from having magical breathing!");
				break;
			case 74:
				u.uprops[DEAC_PASSES_WALLS].intrinsic += rnz(200);
				pline("You are prevented from having phasing!");
				break;
			case 75:
			case 76:
				u.uprops[DEAC_SLOW_DIGESTION].intrinsic += rnz(200);
				pline("You are prevented from having slow digestion!");
				break;
			case 77:
				u.uprops[DEAC_HALF_SPDAM].intrinsic += rnz(200);
				pline("You are prevented from having half spell damage!");
				break;
			case 78:
				u.uprops[DEAC_HALF_PHDAM].intrinsic += rnz(200);
				pline("You are prevented from having half physical damage!");
				break;
			case 79:
			case 80:
			case 81:
			case 82:
			case 83:
				u.uprops[DEAC_REGENERATION].intrinsic += rnz(200);
				pline("You are prevented from having regeneration!");
				break;
			case 84:
			case 85:
				u.uprops[DEAC_ENERGY_REGENERATION].intrinsic += rnz(200);
				pline("You are prevented from having mana regeneration!");
				break;
			case 86:
			case 87:
			case 88:
				u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += rnz(200);
				pline("You are prevented from having polymorph control!");
				break;
			case 89:
			case 90:
			case 91:
			case 92:
			case 93:
				u.uprops[DEAC_FAST].intrinsic += rnz(200);
				pline("You are prevented from having speed!");
				break;
			case 94:
			case 95:
			case 96:
				u.uprops[DEAC_REFLECTING].intrinsic += rnz(200);
				pline("You are prevented from having reflection!");
				break;
			case 97:
			case 98:
			case 99:
				u.uprops[DEAC_FREE_ACTION].intrinsic += rnz(200);
				pline("You are prevented from having free action!");
				break;
			case 100:
				u.uprops[DEAC_HALLU_PARTY].intrinsic += rnz(200);
				pline("You are prevented from hallu partying!");
				break;
			case 101:
				u.uprops[DEAC_DRUNKEN_BOXING].intrinsic += rnz(200);
				pline("You are prevented from drunken boxing!");
				break;
			case 102:
				u.uprops[DEAC_STUNNOPATHY].intrinsic += rnz(200);
				pline("You are prevented from having stunnopathy!");
				break;
			case 103:
				u.uprops[DEAC_NUMBOPATHY].intrinsic += rnz(200);
				pline("You are prevented from having numbopathy!");
				break;
			case 104:
				u.uprops[DEAC_FREEZOPATHY].intrinsic += rnz(200);
				pline("You are prevented from having freezopathy!");
				break;
			case 105:
				u.uprops[DEAC_STONED_CHILLER].intrinsic += rnz(200);
				pline("You are prevented from being a stoned chiller!");
				break;
			case 106:
				u.uprops[DEAC_CORROSIVITY].intrinsic += rnz(200);
				pline("You are prevented from having corrosivity!");
				break;
			case 107:
				u.uprops[DEAC_FEAR_FACTOR].intrinsic += rnz(200);
				pline("You are prevented from having an increased fear factor!");
				break;
			case 108:
				u.uprops[DEAC_BURNOPATHY].intrinsic += rnz(200);
				pline("You are prevented from having burnopathy!");
				break;
			case 109:
				u.uprops[DEAC_SICKOPATHY].intrinsic += rnz(200);
				pline("You are prevented from having sickopathy!");
				break;
			case 110:
				u.uprops[DEAC_KEEN_MEMORY].intrinsic += rnz(200);
				pline("You are prevented from having keen memory!");
				break;
			case 111:
				u.uprops[DEAC_THE_FORCE].intrinsic += rnz(200);
				pline("You are prevented from using the force like a real jedi!");
				break;
			case 112:
				u.uprops[DEAC_SIGHT_BONUS].intrinsic += rnz(200);
				pline("You are prevented from having extra sight!");
				break;
			case 113:
				u.uprops[DEAC_VERSUS_CURSES].intrinsic += rnz(200);
				pline("You are prevented from having curse resistance!");
				break;
			case 114:
				u.uprops[DEAC_STUN_RES].intrinsic += rnz(200);
				pline("You are prevented from having stun resistance!");
				break;
			case 115:
				u.uprops[DEAC_CONF_RES].intrinsic += rnz(200);
				pline("You are prevented from having confusion resistance!");
				break;
			case 116:
				u.uprops[DEAC_DOUBLE_ATTACK].intrinsic += rnz(200);
				pline("You are prevented from having double attacks!");
				break;
			case 117:
				u.uprops[DEAC_QUAD_ATTACK].intrinsic += rnz(200);
				pline("You are prevented from having quad attacks!");
				break;
			}
	}

	if (!rn2(250) && AutomaticVulnerabilitiy) {

		switch (rnd(117)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				u.uprops[DEAC_FIRE_RES].intrinsic += rnz(200);
				pline("You are prevented from having fire resistance!");
				break;
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				u.uprops[DEAC_COLD_RES].intrinsic += rnz(200);
				pline("You are prevented from having cold resistance!");
				break;
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				u.uprops[DEAC_SLEEP_RES].intrinsic += rnz(200);
				pline("You are prevented from having sleep resistance!");
				break;
			case 16:
			case 17:
				u.uprops[DEAC_DISINT_RES].intrinsic += rnz(200);
				pline("You are prevented from having disintegration resistance!");
				break;
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
				u.uprops[DEAC_SHOCK_RES].intrinsic += rnz(200);
				pline("You are prevented from having shock resistance!");
				break;
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
				u.uprops[DEAC_POISON_RES].intrinsic += rnz(200);
				pline("You are prevented from having poison resistance!");
				break;
			case 28:
			case 29:
			case 30:
				u.uprops[DEAC_DRAIN_RES].intrinsic += rnz(200);
				pline("You are prevented from having drain resistance!");
				break;
			case 31:
			case 32:
				u.uprops[DEAC_SICK_RES].intrinsic += rnz(200);
				pline("You are prevented from having sickness resistance!");
				break;
			case 33:
			case 34:
				u.uprops[DEAC_ANTIMAGIC].intrinsic += rnz(200);
				pline("You are prevented from having magic resistance!");
				break;
			case 35:
			case 36:
			case 37:
			case 38:
				u.uprops[DEAC_ACID_RES].intrinsic += rnz(200);
				pline("You are prevented from having acid resistance!");
				break;
			case 39:
			case 40:
				u.uprops[DEAC_STONE_RES].intrinsic += rnz(200);
				pline("You are prevented from having petrification resistance!");
				break;
			case 41:
				u.uprops[DEAC_FEAR_RES].intrinsic += rnz(200);
				pline("You are prevented from having fear resistance!");
				break;
			case 42:
			case 43:
			case 44:
				u.uprops[DEAC_SEE_INVIS].intrinsic += rnz(200);
				pline("You are prevented from having see invisible!");
				break;
			case 45:
			case 46:
			case 47:
				u.uprops[DEAC_TELEPAT].intrinsic += rnz(200);
				pline("You are prevented from having telepathy!");
				break;
			case 48:
			case 49:
			case 50:
				u.uprops[DEAC_WARNING].intrinsic += rnz(200);
				pline("You are prevented from having warning!");
				break;
			case 51:
			case 52:
			case 53:
				u.uprops[DEAC_SEARCHING].intrinsic += rnz(200);
				pline("You are prevented from having automatic searching!");
				break;
			case 54:
				u.uprops[DEAC_CLAIRVOYANT].intrinsic += rnz(200);
				pline("You are prevented from having clairvoyance!");
				break;
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
				u.uprops[DEAC_INFRAVISION].intrinsic += rnz(200);
				pline("You are prevented from having infravision!");
				break;
			case 60:
				u.uprops[DEAC_DETECT_MONSTERS].intrinsic += rnz(200);
				pline("You are prevented from having detect monsters!");
				break;
			case 61:
			case 62:
			case 63:
				u.uprops[DEAC_INVIS].intrinsic += rnz(200);
				pline("You are prevented from having invisibility!");
				break;
			case 64:
				u.uprops[DEAC_DISPLACED].intrinsic += rnz(200);
				pline("You are prevented from having displacement!");
				break;
			case 65:
			case 66:
			case 67:
				u.uprops[DEAC_STEALTH].intrinsic += rnz(200);
				pline("You are prevented from having stealth!");
				break;
			case 68:
				u.uprops[DEAC_JUMPING].intrinsic += rnz(200);
				pline("You are prevented from having jumping!");
				break;
			case 69:
			case 70:
			case 71:
				u.uprops[DEAC_TELEPORT_CONTROL].intrinsic += rnz(200);
				pline("You are prevented from having teleport control!");
				break;
			case 72:
				u.uprops[DEAC_FLYING].intrinsic += rnz(200);
				pline("You are prevented from having flying!");
				break;
			case 73:
				u.uprops[DEAC_MAGICAL_BREATHING].intrinsic += rnz(200);
				pline("You are prevented from having magical breathing!");
				break;
			case 74:
				u.uprops[DEAC_PASSES_WALLS].intrinsic += rnz(200);
				pline("You are prevented from having phasing!");
				break;
			case 75:
			case 76:
				u.uprops[DEAC_SLOW_DIGESTION].intrinsic += rnz(200);
				pline("You are prevented from having slow digestion!");
				break;
			case 77:
				u.uprops[DEAC_HALF_SPDAM].intrinsic += rnz(200);
				pline("You are prevented from having half spell damage!");
				break;
			case 78:
				u.uprops[DEAC_HALF_PHDAM].intrinsic += rnz(200);
				pline("You are prevented from having half physical damage!");
				break;
			case 79:
			case 80:
			case 81:
			case 82:
			case 83:
				u.uprops[DEAC_REGENERATION].intrinsic += rnz(200);
				pline("You are prevented from having regeneration!");
				break;
			case 84:
			case 85:
				u.uprops[DEAC_ENERGY_REGENERATION].intrinsic += rnz(200);
				pline("You are prevented from having mana regeneration!");
				break;
			case 86:
			case 87:
			case 88:
				u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += rnz(200);
				pline("You are prevented from having polymorph control!");
				break;
			case 89:
			case 90:
			case 91:
			case 92:
			case 93:
				u.uprops[DEAC_FAST].intrinsic += rnz(200);
				pline("You are prevented from having speed!");
				break;
			case 94:
			case 95:
			case 96:
				u.uprops[DEAC_REFLECTING].intrinsic += rnz(200);
				pline("You are prevented from having reflection!");
				break;
			case 97:
			case 98:
			case 99:
				u.uprops[DEAC_FREE_ACTION].intrinsic += rnz(200);
				pline("You are prevented from having free action!");
				break;
			case 100:
				u.uprops[DEAC_HALLU_PARTY].intrinsic += rnz(200);
				pline("You are prevented from hallu partying!");
				break;
			case 101:
				u.uprops[DEAC_DRUNKEN_BOXING].intrinsic += rnz(200);
				pline("You are prevented from drunken boxing!");
				break;
			case 102:
				u.uprops[DEAC_STUNNOPATHY].intrinsic += rnz(200);
				pline("You are prevented from having stunnopathy!");
				break;
			case 103:
				u.uprops[DEAC_NUMBOPATHY].intrinsic += rnz(200);
				pline("You are prevented from having numbopathy!");
				break;
			case 104:
				u.uprops[DEAC_FREEZOPATHY].intrinsic += rnz(200);
				pline("You are prevented from having freezopathy!");
				break;
			case 105:
				u.uprops[DEAC_STONED_CHILLER].intrinsic += rnz(200);
				pline("You are prevented from being a stoned chiller!");
				break;
			case 106:
				u.uprops[DEAC_CORROSIVITY].intrinsic += rnz(200);
				pline("You are prevented from having corrosivity!");
				break;
			case 107:
				u.uprops[DEAC_FEAR_FACTOR].intrinsic += rnz(200);
				pline("You are prevented from having an increased fear factor!");
				break;
			case 108:
				u.uprops[DEAC_BURNOPATHY].intrinsic += rnz(200);
				pline("You are prevented from having burnopathy!");
				break;
			case 109:
				u.uprops[DEAC_SICKOPATHY].intrinsic += rnz(200);
				pline("You are prevented from having sickopathy!");
				break;
			case 110:
				u.uprops[DEAC_KEEN_MEMORY].intrinsic += rnz(200);
				pline("You are prevented from having keen memory!");
				break;
			case 111:
				u.uprops[DEAC_THE_FORCE].intrinsic += rnz(200);
				pline("You are prevented from using the force like a real jedi!");
				break;
			case 112:
				u.uprops[DEAC_SIGHT_BONUS].intrinsic += rnz(200);
				pline("You are prevented from having extra sight!");
				break;
			case 113:
				u.uprops[DEAC_VERSUS_CURSES].intrinsic += rnz(200);
				pline("You are prevented from having curse resistance!");
				break;
			case 114:
				u.uprops[DEAC_STUN_RES].intrinsic += rnz(200);
				pline("You are prevented from having stun resistance!");
				break;
			case 115:
				u.uprops[DEAC_CONF_RES].intrinsic += rnz(200);
				pline("You are prevented from having confusion resistance!");
				break;
			case 116:
				u.uprops[DEAC_DOUBLE_ATTACK].intrinsic += rnz(200);
				pline("You are prevented from having double attacks!");
				break;
			case 117:
				u.uprops[DEAC_QUAD_ATTACK].intrinsic += rnz(200);
				pline("You are prevented from having quad attacks!");
				break;
			}
	}

	if (!rn2(250) && have_vulnerabilitystone() ) {

		switch (rnd(117)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				u.uprops[DEAC_FIRE_RES].intrinsic += rnz(200);
				pline("You are prevented from having fire resistance!");
				break;
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				u.uprops[DEAC_COLD_RES].intrinsic += rnz(200);
				pline("You are prevented from having cold resistance!");
				break;
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				u.uprops[DEAC_SLEEP_RES].intrinsic += rnz(200);
				pline("You are prevented from having sleep resistance!");
				break;
			case 16:
			case 17:
				u.uprops[DEAC_DISINT_RES].intrinsic += rnz(200);
				pline("You are prevented from having disintegration resistance!");
				break;
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
				u.uprops[DEAC_SHOCK_RES].intrinsic += rnz(200);
				pline("You are prevented from having shock resistance!");
				break;
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
				u.uprops[DEAC_POISON_RES].intrinsic += rnz(200);
				pline("You are prevented from having poison resistance!");
				break;
			case 28:
			case 29:
			case 30:
				u.uprops[DEAC_DRAIN_RES].intrinsic += rnz(200);
				pline("You are prevented from having drain resistance!");
				break;
			case 31:
			case 32:
				u.uprops[DEAC_SICK_RES].intrinsic += rnz(200);
				pline("You are prevented from having sickness resistance!");
				break;
			case 33:
			case 34:
				u.uprops[DEAC_ANTIMAGIC].intrinsic += rnz(200);
				pline("You are prevented from having magic resistance!");
				break;
			case 35:
			case 36:
			case 37:
			case 38:
				u.uprops[DEAC_ACID_RES].intrinsic += rnz(200);
				pline("You are prevented from having acid resistance!");
				break;
			case 39:
			case 40:
				u.uprops[DEAC_STONE_RES].intrinsic += rnz(200);
				pline("You are prevented from having petrification resistance!");
				break;
			case 41:
				u.uprops[DEAC_FEAR_RES].intrinsic += rnz(200);
				pline("You are prevented from having fear resistance!");
				break;
			case 42:
			case 43:
			case 44:
				u.uprops[DEAC_SEE_INVIS].intrinsic += rnz(200);
				pline("You are prevented from having see invisible!");
				break;
			case 45:
			case 46:
			case 47:
				u.uprops[DEAC_TELEPAT].intrinsic += rnz(200);
				pline("You are prevented from having telepathy!");
				break;
			case 48:
			case 49:
			case 50:
				u.uprops[DEAC_WARNING].intrinsic += rnz(200);
				pline("You are prevented from having warning!");
				break;
			case 51:
			case 52:
			case 53:
				u.uprops[DEAC_SEARCHING].intrinsic += rnz(200);
				pline("You are prevented from having automatic searching!");
				break;
			case 54:
				u.uprops[DEAC_CLAIRVOYANT].intrinsic += rnz(200);
				pline("You are prevented from having clairvoyance!");
				break;
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
				u.uprops[DEAC_INFRAVISION].intrinsic += rnz(200);
				pline("You are prevented from having infravision!");
				break;
			case 60:
				u.uprops[DEAC_DETECT_MONSTERS].intrinsic += rnz(200);
				pline("You are prevented from having detect monsters!");
				break;
			case 61:
			case 62:
			case 63:
				u.uprops[DEAC_INVIS].intrinsic += rnz(200);
				pline("You are prevented from having invisibility!");
				break;
			case 64:
				u.uprops[DEAC_DISPLACED].intrinsic += rnz(200);
				pline("You are prevented from having displacement!");
				break;
			case 65:
			case 66:
			case 67:
				u.uprops[DEAC_STEALTH].intrinsic += rnz(200);
				pline("You are prevented from having stealth!");
				break;
			case 68:
				u.uprops[DEAC_JUMPING].intrinsic += rnz(200);
				pline("You are prevented from having jumping!");
				break;
			case 69:
			case 70:
			case 71:
				u.uprops[DEAC_TELEPORT_CONTROL].intrinsic += rnz(200);
				pline("You are prevented from having teleport control!");
				break;
			case 72:
				u.uprops[DEAC_FLYING].intrinsic += rnz(200);
				pline("You are prevented from having flying!");
				break;
			case 73:
				u.uprops[DEAC_MAGICAL_BREATHING].intrinsic += rnz(200);
				pline("You are prevented from having magical breathing!");
				break;
			case 74:
				u.uprops[DEAC_PASSES_WALLS].intrinsic += rnz(200);
				pline("You are prevented from having phasing!");
				break;
			case 75:
			case 76:
				u.uprops[DEAC_SLOW_DIGESTION].intrinsic += rnz(200);
				pline("You are prevented from having slow digestion!");
				break;
			case 77:
				u.uprops[DEAC_HALF_SPDAM].intrinsic += rnz(200);
				pline("You are prevented from having half spell damage!");
				break;
			case 78:
				u.uprops[DEAC_HALF_PHDAM].intrinsic += rnz(200);
				pline("You are prevented from having half physical damage!");
				break;
			case 79:
			case 80:
			case 81:
			case 82:
			case 83:
				u.uprops[DEAC_REGENERATION].intrinsic += rnz(200);
				pline("You are prevented from having regeneration!");
				break;
			case 84:
			case 85:
				u.uprops[DEAC_ENERGY_REGENERATION].intrinsic += rnz(200);
				pline("You are prevented from having mana regeneration!");
				break;
			case 86:
			case 87:
			case 88:
				u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += rnz(200);
				pline("You are prevented from having polymorph control!");
				break;
			case 89:
			case 90:
			case 91:
			case 92:
			case 93:
				u.uprops[DEAC_FAST].intrinsic += rnz(200);
				pline("You are prevented from having speed!");
				break;
			case 94:
			case 95:
			case 96:
				u.uprops[DEAC_REFLECTING].intrinsic += rnz(200);
				pline("You are prevented from having reflection!");
				break;
			case 97:
			case 98:
			case 99:
				u.uprops[DEAC_FREE_ACTION].intrinsic += rnz(200);
				pline("You are prevented from having free action!");
				break;
			case 100:
				u.uprops[DEAC_HALLU_PARTY].intrinsic += rnz(200);
				pline("You are prevented from hallu partying!");
				break;
			case 101:
				u.uprops[DEAC_DRUNKEN_BOXING].intrinsic += rnz(200);
				pline("You are prevented from drunken boxing!");
				break;
			case 102:
				u.uprops[DEAC_STUNNOPATHY].intrinsic += rnz(200);
				pline("You are prevented from having stunnopathy!");
				break;
			case 103:
				u.uprops[DEAC_NUMBOPATHY].intrinsic += rnz(200);
				pline("You are prevented from having numbopathy!");
				break;
			case 104:
				u.uprops[DEAC_FREEZOPATHY].intrinsic += rnz(200);
				pline("You are prevented from having freezopathy!");
				break;
			case 105:
				u.uprops[DEAC_STONED_CHILLER].intrinsic += rnz(200);
				pline("You are prevented from being a stoned chiller!");
				break;
			case 106:
				u.uprops[DEAC_CORROSIVITY].intrinsic += rnz(200);
				pline("You are prevented from having corrosivity!");
				break;
			case 107:
				u.uprops[DEAC_FEAR_FACTOR].intrinsic += rnz(200);
				pline("You are prevented from having an increased fear factor!");
				break;
			case 108:
				u.uprops[DEAC_BURNOPATHY].intrinsic += rnz(200);
				pline("You are prevented from having burnopathy!");
				break;
			case 109:
				u.uprops[DEAC_SICKOPATHY].intrinsic += rnz(200);
				pline("You are prevented from having sickopathy!");
				break;
			case 110:
				u.uprops[DEAC_KEEN_MEMORY].intrinsic += rnz(200);
				pline("You are prevented from having keen memory!");
				break;
			case 111:
				u.uprops[DEAC_THE_FORCE].intrinsic += rnz(200);
				pline("You are prevented from using the force like a real jedi!");
				break;
			case 112:
				u.uprops[DEAC_SIGHT_BONUS].intrinsic += rnz(200);
				pline("You are prevented from having extra sight!");
				break;
			case 113:
				u.uprops[DEAC_VERSUS_CURSES].intrinsic += rnz(200);
				pline("You are prevented from having curse resistance!");
				break;
			case 114:
				u.uprops[DEAC_STUN_RES].intrinsic += rnz(200);
				pline("You are prevented from having stun resistance!");
				break;
			case 115:
				u.uprops[DEAC_CONF_RES].intrinsic += rnz(200);
				pline("You are prevented from having confusion resistance!");
				break;
			case 116:
				u.uprops[DEAC_DOUBLE_ATTACK].intrinsic += rnz(200);
				pline("You are prevented from having double attacks!");
				break;
			case 117:
				u.uprops[DEAC_QUAD_ATTACK].intrinsic += rnz(200);
				pline("You are prevented from having quad attacks!");
				break;
			}
	}

	if (u.uprops[TELEPORTING_ITEMS].extrinsic) {

		if (invent) {
		    for (otmpi = invent; otmpi; otmpi = otmpii) {
		      otmpii = otmpi->nobj;

			if (!rn2(10000) && (otmpi->otyp != AMULET_OF_ITEM_TELEPORTATION) && (otmpi->otyp != ITEM_TELEPORTING_STONE) && !stack_too_big(otmpi) ) {

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
#ifdef TOURIST
				    else if (otmpi == uarmu) (void) Shirt_off();
#endif
				    /* catchall -- should never happen */
				    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
				} else if (otmpi ->owornmask & W_AMUL) {
				    Amulet_off();
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
			      if (otmpi->where == OBJ_FLOOR) rloco(otmpi);
			}
		    }
		}

	}

	if (TeleportingItems) {

		if (invent) {
		    for (otmpi = invent; otmpi; otmpi = otmpii) {
		      otmpii = otmpi->nobj;

			if (!rn2(10000) && (otmpi->otyp != AMULET_OF_ITEM_TELEPORTATION) && (otmpi->otyp != ITEM_TELEPORTING_STONE) && !stack_too_big(otmpi) ) {

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
#ifdef TOURIST
				    else if (otmpi == uarmu) (void) Shirt_off();
#endif
				    /* catchall -- should never happen */
				    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
				} else if (otmpi ->owornmask & W_AMUL) {
				    Amulet_off();
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
			      if (otmpi->where == OBJ_FLOOR) rloco(otmpi);
			}
		    }
		}

	}

	if (have_itemportstone() ) {

		if (invent) {
		    for (otmpi = invent; otmpi; otmpi = otmpii) {
		      otmpii = otmpi->nobj;

			if (!rn2(10000) && (otmpi->otyp != AMULET_OF_ITEM_TELEPORTATION) && (otmpi->otyp != ITEM_TELEPORTING_STONE) && !stack_too_big(otmpi) ) {

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
#ifdef TOURIST
				    else if (otmpi == uarmu) (void) Shirt_off();
#endif
				    /* catchall -- should never happen */
				    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
				} else if (otmpi ->owornmask & W_AMUL) {
				    Amulet_off();
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
			      if (otmpi->where == OBJ_FLOOR) rloco(otmpi);
			}
		    }
		}

	}

	if (u.uprops[TRAP_REVEALING].extrinsic) {

	    struct trap *t;

	    for (t = ftrap; t != 0; t = t->ntrap) {
		if (!rn2(1000) && !t->tseen && !t->hiddentrap) {
			t->tseen = 1;
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

		switch (rnd(67)) {

			case 1: RMBLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 2: NoDropProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 3: DSTWProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 4: StatusTrapProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); 
				if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
				if (HStun) set_itimeout(&HeavyStunned, HStun);
				if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
				if (HFeared) set_itimeout(&HeavyFeared, HFeared);
				if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
				if (HBurned) set_itimeout(&HeavyBurned, HBurned);
				if (Blinded) set_itimeout(&HeavyBlind, Blinded);
				if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);
				break;
			case 5: Superscroller += rnz(nastytrapdur * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) * (monster_difficulty() + 1)); 
				(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
				break;
			case 6: MenuBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 7: FreeHandLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 8: Unidentify += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 9: Thirst += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 10: LuckLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 11: ShadesOfGrey += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 12: FaintActive += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 13: Itemcursing += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 14: DifficultyIncreased += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 15: Deafness += rnz(nastytrapdur * (monster_difficulty() + 1)); flags.soundok = 0; break;
			case 16: CasterProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 17: WeaknessProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 18: RotThirteen += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 19: BishopGridbug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 20: UninformationProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 21: StairsProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 22: AlignmentProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 23: ConfusionProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 24: SpeedBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 25: DisplayLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 26: SpellLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 27: YellowSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 28: AutoDestruct += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 29: MemoryLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 30: InventoryLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 31: {

				if (BlackNgWalls) break;

				BlackNgWalls = (blackngdur - (monster_difficulty() * 3));
				(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
				break;
			}
			case 32: IntrinsicLossProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 33: BloodLossProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 34: BadEffectProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 35: TrapCreationProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 36: AutomaticVulnerabilitiy += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 37: TeleportingItems += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 38: NastinessProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 39: CaptchaProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 40: FarlookProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 41: RespawnProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;

			case 42: RecurringAmnesia += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 43: BigscriptEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 44: {
				BankTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1));
				if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));
				u.bankcashamount += u.ugold;
				u.ugold = 0;

				break;
			}
			case 45: MapTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 46: TechTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 47: RecurringDisenchant += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 48: verisiertEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 49: ChaosTerrain += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 50: Muteness += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 51: EngravingDoesntWork += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 52: MagicDeviceEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 53: BookTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 54: LevelTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 55: QuizTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 56: FastMetabolismEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 57: NoReturnEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 58: AlwaysEgotypeMonsters += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 59: TimeGoesByFaster += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 60: FoodIsAlwaysRotten += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 61: AllSkillsUnskilled += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 62: AllStatsAreLower += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 63: PlayerCannotTrainSkills += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 64: PlayerCannotExerciseStats += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 65: TurnLimitation += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 66: WeakSight += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 67: RandomMessages += rnz(nastytrapdur * (monster_difficulty() + 1)); break;

		}

	}

	if (!rn2(1000) && u.uprops[NASTINESS_EFFECTS].extrinsic) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		switch (rnd(67)) {

			case 1: RMBLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 2: NoDropProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 3: DSTWProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 4: StatusTrapProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); 
				if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
				if (HStun) set_itimeout(&HeavyStunned, HStun);
				if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
				if (HFeared) set_itimeout(&HeavyFeared, HFeared);
				if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
				if (HBurned) set_itimeout(&HeavyBurned, HBurned);
				if (Blinded) set_itimeout(&HeavyBlind, Blinded);
				if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);
				break;
			case 5: Superscroller += rnz(nastytrapdur * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) * (monster_difficulty() + 1)); 
				(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
				break;
			case 6: MenuBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 7: FreeHandLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 8: Unidentify += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 9: Thirst += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 10: LuckLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 11: ShadesOfGrey += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 12: FaintActive += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 13: Itemcursing += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 14: DifficultyIncreased += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 15: Deafness += rnz(nastytrapdur * (monster_difficulty() + 1)); flags.soundok = 0; break;
			case 16: CasterProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 17: WeaknessProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 18: RotThirteen += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 19: BishopGridbug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 20: UninformationProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 21: StairsProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 22: AlignmentProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 23: ConfusionProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 24: SpeedBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 25: DisplayLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 26: SpellLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 27: YellowSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 28: AutoDestruct += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 29: MemoryLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 30: InventoryLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 31: {

				if (BlackNgWalls) break;

				BlackNgWalls = (blackngdur - (monster_difficulty() * 3));
				(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
				break;
			}
			case 32: IntrinsicLossProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 33: BloodLossProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 34: BadEffectProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 35: TrapCreationProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 36: AutomaticVulnerabilitiy += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 37: TeleportingItems += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 38: NastinessProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 39: CaptchaProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 40: FarlookProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 41: RespawnProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 42: RecurringAmnesia += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 43: BigscriptEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 44: {
				BankTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1));
				if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));
				u.bankcashamount += u.ugold;
				u.ugold = 0;

				break;
			}
			case 45: MapTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 46: TechTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 47: RecurringDisenchant += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 48: verisiertEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 49: ChaosTerrain += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 50: Muteness += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 51: EngravingDoesntWork += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 52: MagicDeviceEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 53: BookTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 54: LevelTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 55: QuizTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 56: FastMetabolismEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 57: NoReturnEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 58: AlwaysEgotypeMonsters += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 59: TimeGoesByFaster += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 60: FoodIsAlwaysRotten += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 61: AllSkillsUnskilled += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 62: AllStatsAreLower += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 63: PlayerCannotTrainSkills += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 64: PlayerCannotExerciseStats += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 65: TurnLimitation += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 66: WeakSight += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 67: RandomMessages += rnz(nastytrapdur * (monster_difficulty() + 1)); break;

		}

	}

	if (!rn2(1000) && NastinessProblem) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		switch (rnd(67)) {

			case 1: RMBLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 2: NoDropProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 3: DSTWProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 4: StatusTrapProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); 
				if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
				if (HStun) set_itimeout(&HeavyStunned, HStun);
				if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
				if (HFeared) set_itimeout(&HeavyFeared, HFeared);
				if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
				if (HBurned) set_itimeout(&HeavyBurned, HBurned);
				if (Blinded) set_itimeout(&HeavyBlind, Blinded);
				if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);
				break;
			case 5: Superscroller += rnz(nastytrapdur * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) * (monster_difficulty() + 1)); 
				(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
				break;
			case 6: MenuBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 7: FreeHandLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 8: Unidentify += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 9: Thirst += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 10: LuckLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 11: ShadesOfGrey += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 12: FaintActive += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 13: Itemcursing += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 14: DifficultyIncreased += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 15: Deafness += rnz(nastytrapdur * (monster_difficulty() + 1)); flags.soundok = 0; break;
			case 16: CasterProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 17: WeaknessProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 18: RotThirteen += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 19: BishopGridbug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 20: UninformationProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 21: StairsProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 22: AlignmentProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 23: ConfusionProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 24: SpeedBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 25: DisplayLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 26: SpellLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 27: YellowSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 28: AutoDestruct += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 29: MemoryLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 30: InventoryLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 31: {

				if (BlackNgWalls) break;

				BlackNgWalls = (blackngdur - (monster_difficulty() * 3));
				(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
				break;
			}
			case 32: IntrinsicLossProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 33: BloodLossProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 34: BadEffectProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 35: TrapCreationProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 36: AutomaticVulnerabilitiy += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 37: TeleportingItems += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 38: NastinessProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 39: CaptchaProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 40: FarlookProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 41: RespawnProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 42: RecurringAmnesia += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 43: BigscriptEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 44: {
				BankTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1));
				if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));
				u.bankcashamount += u.ugold;
				u.ugold = 0;

				break;
			}
			case 45: MapTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 46: TechTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 47: RecurringDisenchant += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 48: verisiertEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 49: ChaosTerrain += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 50: Muteness += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 51: EngravingDoesntWork += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 52: MagicDeviceEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 53: BookTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 54: LevelTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 55: QuizTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 56: FastMetabolismEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 57: NoReturnEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 58: AlwaysEgotypeMonsters += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 59: TimeGoesByFaster += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 60: FoodIsAlwaysRotten += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 61: AllSkillsUnskilled += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 62: AllStatsAreLower += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 63: PlayerCannotTrainSkills += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 64: PlayerCannotExerciseStats += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 65: TurnLimitation += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 66: WeakSight += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 67: RandomMessages += rnz(nastytrapdur * (monster_difficulty() + 1)); break;

		}

	}

	if (!rn2(1000) && have_nastystone() ) {

		nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		switch (rnd(67)) {

			case 1: RMBLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 2: NoDropProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 3: DSTWProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 4: StatusTrapProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); 
				if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
				if (HStun) set_itimeout(&HeavyStunned, HStun);
				if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
				if (HFeared) set_itimeout(&HeavyFeared, HFeared);
				if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
				if (HBurned) set_itimeout(&HeavyBurned, HBurned);
				if (Blinded) set_itimeout(&HeavyBlind, Blinded);
				if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);
				break;
			case 5: Superscroller += rnz(nastytrapdur * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) * (monster_difficulty() + 1)); 
				(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
				break;
			case 6: MenuBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 7: FreeHandLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 8: Unidentify += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 9: Thirst += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 10: LuckLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 11: ShadesOfGrey += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 12: FaintActive += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 13: Itemcursing += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 14: DifficultyIncreased += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 15: Deafness += rnz(nastytrapdur * (monster_difficulty() + 1)); flags.soundok = 0; break;
			case 16: CasterProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 17: WeaknessProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 18: RotThirteen += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 19: BishopGridbug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 20: UninformationProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 21: StairsProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 22: AlignmentProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 23: ConfusionProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 24: SpeedBug += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 25: DisplayLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 26: SpellLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 27: YellowSpells += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 28: AutoDestruct += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 29: MemoryLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 30: InventoryLoss += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 31: {

				if (BlackNgWalls) break;

				BlackNgWalls = (blackngdur - (monster_difficulty() * 3));
				(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
				break;
			}
			case 32: IntrinsicLossProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 33: BloodLossProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 34: BadEffectProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 35: TrapCreationProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 36: AutomaticVulnerabilitiy += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 37: TeleportingItems += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 38: NastinessProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 39: CaptchaProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 40: FarlookProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 41: RespawnProblem += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 42: RecurringAmnesia += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 43: BigscriptEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 44: {
				BankTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1));
				if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));
				u.bankcashamount += u.ugold;
				u.ugold = 0;

				break;
			}
			case 45: MapTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 46: TechTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 47: RecurringDisenchant += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 48: verisiertEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 49: ChaosTerrain += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 50: Muteness += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 51: EngravingDoesntWork += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 52: MagicDeviceEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 53: BookTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 54: LevelTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 55: QuizTrapEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 56: FastMetabolismEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 57: NoReturnEffect += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 58: AlwaysEgotypeMonsters += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 59: TimeGoesByFaster += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 60: FoodIsAlwaysRotten += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 61: AllSkillsUnskilled += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 62: AllStatsAreLower += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 63: PlayerCannotTrainSkills += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 64: PlayerCannotExerciseStats += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 65: TurnLimitation += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 66: WeakSight += rnz(nastytrapdur * (monster_difficulty() + 1)); break;
			case 67: RandomMessages += rnz(nastytrapdur * (monster_difficulty() + 1)); break;

		}

	}

	/* Max alignment record moved from align.h, so we can make it into a dynamic function --Amy */

	if (!AlignmentProblem && !u.uprops[ALIGNMENT_FAILURE].extrinsic && !have_alignmentstone() && !rn2(Race_if(PM_UNALIGNMENT_THING) ? 50 : 200) && ((u.alignlim < 20) ? (TRUE) : (rnd(u.alignlim) < 20) ) )
		u.alignlim++;

	if ( (AlignmentProblem || u.uprops[ALIGNMENT_FAILURE].extrinsic || have_alignmentstone() ) && !rn2(Race_if(PM_UNALIGNMENT_THING) ? 50 : 200) ) {
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

	if ( (WeaknessProblem || u.uprops[WEAKNESS_PROBLEM].extrinsic || have_weaknessstone() ) && u.uhunger < 201) {

		if (!rn2(20)) {

			u.uhpmax -= 3;
			if (u.uhp > u.uhpmax) u.uhp -= 3;
			if (u.uhp < 1) {
				pline("You pass away due to being too weak.");
				killer_format = KILLED_BY_AN;
				killer = "fatal weakness";
				done(DIED);
			}

		} else {

			u.uhp -= 3;
			if (u.uhp < 1) {
				pline("You pass away due to being too weak.");
				killer_format = KILLED_BY_AN;
				killer = "fatal weakness";
				done(DIED);
			}

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

	if (!rn2(2500) && have_bloodlossstone() ) {
		You("are losing lots of blood!");
		u.uhp -= 1;
		u.uhpmax -= 1;
		u.uen -= 1;
		u.uenmax -= 1;
		losehp(rnz(u.legscratching), "severe bleedout", KILLED_BY);
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
	if (!rn2(7500) && have_bloodlossstone() ) {
		pline("Your scratching wounds are bleeding %s worse than before!", rn2(2) ? "even" : "much");
		u.legscratching++;
	}

	if (u.uluck != baseluck &&
		moves % (u.uhave.amulet || u.ugangr ? 300 : 600) == 0) {
	/* Cursed luckstones stop bad luck from timing out; blessed luckstones
	 * stop good luck from timing out; normal luckstones stop both;
	 * neither is stopped if you don't have a luckstone.
	 * Luck is based at 0 usually, +1 if a full moon and -1 on Friday 13th
	 */
	    register int time_luck = stone_luck(FALSE);
	    boolean nostone = !carrying(LUCKSTONE) && !stone_luck(TRUE);

	    if(u.uluck > baseluck && (nostone || time_luck < 0 || !rn2(10) )) /* now luck will also time out if you do have a luckstone; it just times out more slowly --Amy */
		u.uluck--;
	    else if(u.uluck < baseluck && (nostone || time_luck > 0 || !rn2(10) ))
		u.uluck++;
	}

	/* WAC -- check for timeout of specials */
	tech_timeout();

#ifdef CONVICT
    if(Phasing) phasing_dialogue();
#endif /* CONVICT */
	if(u.uinvulnerable) return; /* things past this point could kill you */
	if(Stoned) stoned_dialogue();
	if(Slimed) slime_dialogue();
	if(Vomiting) vomiting_dialogue();
	if(Strangled) choke_dialogue();
	if(u.mtimedone && !--u.mtimedone) {
		if (Unchanging || Race_if(PM_UNGENOMOLD) )
			u.mtimedone = rnd(100*youmonst.data->mlevel + 1);
		else
			rehumanize();
	}
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

#ifdef STEED
	if (u.ugallop) {
	    if (--u.ugallop == 0L && u.usteed)
	    	pline("%s stops galloping.", Monnam(u.usteed));
	}
#endif

	for(upp = u.uprops; upp < u.uprops+SIZE(u.uprops); upp++)
	    if((upp->intrinsic & TIMEOUT) && !(--upp->intrinsic & TIMEOUT)) {
		switch(upp - u.uprops){
		case STONED:
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
			break;
		case SLIMED:
			if (delayed_killer && !killer) {
				killer = delayed_killer;
				delayed_killer = 0;
			}
			if (!killer) {
				killer_format = NO_KILLER_PREFIX;
				killer = "turned into green slime";
			}
			done(TURNED_SLIME);
			break;
		case VOMITING:
			make_vomiting(0L, TRUE);
			break;
		case MC_REDUCTION:
			pline("Your magic cancellation seems to work correctly again.");
			break;
		case SICK:

			if (Role_if(PM_COOK) && rn2(2)) {
			u.usick_type = 0;
			pline("You survived the poisoning!");
			break;
			}

			You("die from your illness.");
			killer_format = KILLED_BY_AN;
			killer = u.usick_cause;
			if ((m_idx = name_to_mon(killer)) >= LOW_PM) {
			    if (type_is_pname(&mons[m_idx])) {
				killer_format = KILLED_BY;
			    } else if (mons[m_idx].geno & G_UNIQ) {
				killer = the(killer);
				Strcpy(u.usick_cause, killer);
				killer_format = KILLED_BY;
			    }
			}
			u.usick_type = 0;
			done(POISONING);
			break;
		case FAST:
			if (!Very_fast)
				You_feel("yourself slowing down%s.",
							Fast ? " a bit" : "");
			break;
		case FIRE_RES:
			if (!Fire_resistance)
				You("feel a little warmer.");
			break;
		case FEAR_RES:
			if (!Fear_resistance)
				You("feel less resistant to fear.");
			break;
		case COLD_RES:
			if (!Cold_resistance)
				You("feel a little cooler.");
			break;
		case SLEEP_RES:
			if (!Sleep_resistance)
				You("feel a little sleepy.");
			break;
		case KEEN_MEMORY:
			if (!Keen_memory)
				You("feel less able to memorize things.");
			break;
		case SIGHT_BONUS:
			if (!Sight_bonus)
				You("notice your surroundings darkening a bit.");
			vision_full_recalc = 1;
			break;
		case SHOCK_RES:
			if (!Shock_resistance)
				You("feel a little static cling.");
			break;
		case AGGRAVATE_MONSTER:
			if (!Aggravate_monster)
				You("feel less aggravated.");
			break;
		case POISON_RES:
			if (!Poison_resistance)
				You("feel a little less healthy.");
			break;
		case DISINT_RES:
			if (!Disint_resistance)
				You("feel a little less firm.");
			break;
		case ACID_RES:
			if (!Acid_resistance)
				You("feel more vulnerable to acid.");
			break;
		case STONE_RES:
			if (!Stone_resistance)
				You("feel a little less limber.");
			break;
		case HALF_PHDAM:
			if (!Half_physical_damage)
				You("feel less resistant to damage.");
			break;
		case HALF_SPDAM:
			if (!Half_spell_damage)
				You("feel less resistant to spells.");
			break;
		case SICK_RES:
			if (!Sick_resistance)
				You("feel worried about getting sick.");
			break;
		case DISPLACED:
			if (!Displaced)
				You("feel more exposed.");
			break;
		case TELEPORT:
			if (!Teleportation)
				You("feel a little less jumpy.");
			break;
		case TELEPORT_CONTROL:
			if (!Teleport_control)
				You("feel a little less in control of yourself.");
			break;
		case TELEPAT:
			if (!HTelepat)
				You("feel a little less mentally acute.");
			break;
		case FREE_ACTION:
			if (!Free_action)
				You("feel a little stiffer.");
			break;
		case PASSES_WALLS:
			if (!Passes_walls)
				You("feel a little more solid.");
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
				if (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR)) pline("Ye take a caulk.");
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
				pline("You feel less airborne.");
			break;
		case BLACK_NG_WALLS:

			pline(Hallucination ? "Rien ne va plus... You seem to remember this slogan being printed on all official Pokemon games' box covers. It's like 'Rien ne va plus' is the official Pokemon slogan!" : "Rien ne va plus...");
			/* Of course it's actually the Roulette slogan. --Amy */

			if (Upolyd) losehp(u.mhmax, "failing to defeat Blacky in time", KILLED_BY);
			losehp(u.uhpmax, "failing to defeat Blacky in time", KILLED_BY);
			break;
		case STRANGLED:
			killer_format = KILLED_BY;
			killer = (u.uburied) ? "suffocation" : "strangulation";
			done(DIED);
			break;
		case FUMBLING:
			/* call this only when a move took place.  */
			/* otherwise handle fumbling msgs locally. */
			if (u.umoved && !Levitation) {
			    slip_or_trip();

			/* based on the evil patch idea by jonadab: stupidity or amnesia from falling on your head --Amy */

			    if (!rn2(1000) && has_head(youmonst.data) && !Role_if(PM_COURIER) ) {

				if (rn2(50)) {
					adjattrib(rn2(2) ? A_INT : A_WIS, -rnd(5), FALSE);
				} else {
					You_feel("dizzy!");
					forget(1 + rn2(5));
				}
			    }

			    nomul(-2, "fumbling");
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
	nomul(how_long, "sleeping");
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


#ifdef UNPOLYPILE
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
			UNPOLY_OBJ, (genericptr_t) obj);
	return;
}

/* timer callback routine: undo polymorph on an object */
void
unpoly_obj(arg, timeout)
	genericptr_t arg;
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

	(void) stop_timer(UNPOLY_OBJ, (genericptr_t) obj);

	obj = poly_obj(obj, oldobj);

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
#endif /* UNPOLYPILE */
#endif /* OVL1 */
#ifdef OVL0

#ifdef UNPOLYPILE
/*
 * Cleanup a hazy object if timer stopped.
 */
/*ARGSUSED*/
static void
cleanup_unpoly(arg, timeout)
    genericptr_t arg;
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
#endif /* UNPOLYPILE */

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
	    (void) stop_timer(UNPOLY_MON, (genericptr_t) mtmp);
	    /* Lengthen unpolytime - was 500,500  for player */
	    (void) start_timer(when ? when : /*rn1(1000, 1000)*/rnz(2000), TIMER_MONSTER,
		    UNPOLY_MON, (genericptr_t) mtmp);
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
	    (void) stop_timer(UNPOLY_MON, (genericptr_t) mtmp);
	    /* Lengthen unpolytime - was 500,500  for player */
	    (void) start_timer(when ? when : /*rn1(1000, 1000)*/999999, TIMER_MONSTER,
		    UNPOLY_MON, (genericptr_t) mtmp);
	}
	return i;
}


/* timer callback routine: undo polymorph on a monster */
void
unpoly_mon(arg, timeout)
	genericptr_t arg;
	long timeout;
{
	struct monst *mtmp;
	int oldmon;
	char oldname[BUFSZ];  /* DON'T use char * since this will change! */
	boolean silent = (timeout != monstermoves);     /* unpoly'ed while away */

	mtmp = (struct monst *) arg;
	oldmon = mtmp->oldmonnm;

	strcpy(oldname, Monnam(mtmp));

	(void) stop_timer(UNPOLY_MON, (genericptr_t) mtmp);

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

#ifdef FIREARMS
/* Attach an explosion timeout to a given explosive device */
void
attach_bomb_blow_timeout(bomb, fuse, yours)
struct obj *bomb;
int fuse;
boolean yours;
{
	long expiretime;	

	if (bomb->cursed && !rn2(2)) return; /* doesn't arm if not armed */

	/* Now if you play with other people's property... */
	if (yours && (!carried(bomb) && costly_spot(bomb->ox, bomb->oy) &&
		!bomb->no_charge || bomb->unpaid)) {
	    verbalize("You play with it, you pay for it!");
	    bill_dummy_object(bomb);
	}

	expiretime = stop_timer(BOMB_BLOW, (genericptr_t) bomb);
	if (expiretime > 0L) fuse = fuse - (expiretime - monstermoves);
	bomb->yours = yours;
	bomb->oarmed = TRUE;

	(void) start_timer((long)fuse, TIMER_OBJECT, BOMB_BLOW, (genericptr_t)bomb);
}

/* timer callback routine: detonate the explosives */
void
bomb_blow(arg, timeout)
genericptr_t arg;
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
			underwater = is_pool(x, y);
			if (!silent) {
			    if (x == u.ux && y == u.uy) {
				if (underwater && (Flying || Levitation))
				    pline_The("water boils beneath you.");
				else if (underwater && Wwalking)
				    pline_The("water erupts around you.");
				else pline("A bomb explodes under your %s!",
				  makeplural(body_part(FOOT)));
			    } else if (cansee(x, y))
				You(underwater ?
				    "see a plume of water shoot up." :
				    "see a bomb explode.");
			}
			if (underwater && (Flying || Levitation || Wwalking)) {
			    if (Wwalking && x == u.ux && y == u.uy) {
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
#endif

/* Attach an egg hatch timeout to the given egg. */
void
attach_egg_hatch_timeout(egg)
struct obj *egg;
{
	int i;

	/* stop previous timer, if any */
	(void) stop_timer(HATCH_EGG, (genericptr_t) egg);

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
						HATCH_EGG, (genericptr_t)egg);
		break;
	    }
}

/* prevent an egg from ever hatching */
void
kill_egg(egg)
struct obj *egg;
{
	/* stop previous timer, if any */
	(void) stop_timer(HATCH_EGG, (genericptr_t) egg);
}

/* timer callback routine: hatch the given egg */
void
hatch_egg(arg, timeout)
genericptr_t arg;
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

	mon = mon2 = (struct monst *)0;
	mnum = big_to_little(egg->corpsenm);
	/* The identity of one's father is learned, not innate */
	yours = (egg->spe || (!flags.female && carried(egg) && !rn2(2)));
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
		Sprintf(monnambuf, "%s%s",
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
			    Sprintf(carriedby, "%s pack",
				     s_suffix(a_monnam(egg->ocarry)));
			    knows_egg = TRUE;
			}
			else if (is_pool(mon->mx, mon->my))
			    Strcpy(carriedby, "empty water");
			else
			    Strcpy(carriedby, "thin air");
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
		    (void) stop_timer(HATCH_EGG, (genericptr_t)egg);
		    (void) start_timer((long)rnd(12), TIMER_OBJECT,
					HATCH_EGG, (genericptr_t)egg);
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
	(void) stop_timer(FIG_TRANSFORM, (genericptr_t) figurine);

	/*
	 * Decide when to transform the figurine.
	 */
	i = rnd(9000) + 200;
	/* figurine will transform */
	(void) start_timer((long)i, TIMER_OBJECT,
				FIG_TRANSFORM, (genericptr_t)figurine);
}

/* give a fumble message */
STATIC_OVL void
slip_or_trip()
{
	struct obj *otmp = vobj_at(u.ux, u.uy);
	const char *what, *pronoun;
	char buf[BUFSZ];
	boolean on_foot = TRUE;
#ifdef STEED
	if (u.usteed) on_foot = FALSE;
#endif

	if (otmp && on_foot && !u.uinwater && is_pool(u.ux, u.uy)) otmp = 0;

	if (otmp && on_foot) {		/* trip over something in particular */
	    /*
		If there is only one item, it will have just been named
		during the move, so refer to by via pronoun; otherwise,
		if the top item has been or can be seen, refer to it by
		name; if not, look for rocks to trip over; trip over
		anonymous "something" if there aren't any rocks.
	     */
	    pronoun = otmp->quan == 1L ? "it" : Hallucination ? "they" : "them";
	    what = !otmp->nexthere ? pronoun :
		  (otmp->dknown || !Blind) ? doname(otmp) :
		  ((otmp = sobj_at(ROCK, u.ux, u.uy)) == 0 ? something :
		  (otmp->quan == 1L ? "a rock" : "some rocks"));
	    if (Hallucination) {
		what = strcpy(buf, what);
		buf[0] = highc(buf[0]);
		pline("Egads!  %s bite%s your %s!",
			what, (!otmp || otmp->quan == 1L) ? "s" : "",
			body_part(FOOT));
	    } else {
		You("trip over %s.", what);
	    }
	} else if (rn2(3) && is_ice(u.ux, u.uy)) {
	    pline("%s %s%s on the ice.",
#ifdef STEED
		u.usteed ? upstart(x_monnam(u.usteed,
				u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				(char *)0, SUPPRESS_SADDLE, FALSE)) :
#endif
		"You", rn2(2) ? "slip" : "slide", on_foot ? "" : "s");
	} else {
	    if (on_foot) {
		switch (rn2(4)) {
		  case 1:
			You("trip over your own %s.", Hallucination ?
				"elbow" : makeplural(body_part(FOOT)));
			break;
		  case 2:
			You("slip %s.", Hallucination ?
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
#ifdef STEED
	    else {
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
#endif
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
		if (Hallucination)
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
genericptr_t arg;
long timeout;
{
	struct obj *obj = (struct obj *) arg;
	boolean canseeit, many, menorah, need_newsym;
	xchar x, y;
	char whose[BUFSZ];

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
#ifdef FIREARMS
		} else if (obj->otyp == STICK_OF_DYNAMITE) {
			bomb_blow((genericptr_t) obj, timeout);
			return;
#endif
		}

	    } else {
		obj->age -= how_long;
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
	    case NATURAL_CANDLE:
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
				pline(Hallucination ?
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

#ifdef LIGHTSABERS
	    case RED_DOUBLE_LIGHTSABER:
	    	if (obj->altmode && obj->cursed && !rn2(25)) {
		    obj->altmode = FALSE;
		    pline("%s %s reverts to single blade mode!",
			    whose, xname(obj));
	    	}
	    case GREEN_LIGHTSABER: 
#ifdef D_SABER
	    case BLUE_LIGHTSABER:
#if 0
	    case VIOLET_LIGHTSABER:
	    case WHITE_LIGHTSABER:
	    case YELLOW_LIGHTSABER:
#endif
#endif
	    case RED_LIGHTSABER:
	    case LASER_SWATTER:
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
#endif

#ifdef FIREARMS
	    case STICK_OF_DYNAMITE:
		end_burn(obj, FALSE);
		bomb_blow((genericptr_t) obj, timeout);
		return;
#endif
	    default:
		impossible("burn_object: unexpeced obj %s", xname(obj));
		break;
	}
	if (need_newsym) newsym(x, y);
}

#ifdef LIGHTSABERS
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
	if (obj->otyp == RED_DOUBLE_LIGHTSABER) obj->altmode = FALSE;
	if ((obj == uwep) || (u.twoweap && obj != uswapwep)) unweapon = TRUE;
	end_burn(obj, timer_attached);
}
#endif

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
#ifdef LIGHTSABERS
	    case RED_DOUBLE_LIGHTSABER:
	    	if (obj->altmode && obj->age > 1) 
		    obj->age--; /* Double power usage */
	    case RED_LIGHTSABER:
	    case LASER_SWATTER:
#ifdef D_SABER
	    case BLUE_LIGHTSABER:
#if 0
	    case VIOLET_LIGHTSABER:
	    case WHITE_LIGHTSABER:
	    case YELLOW_LIGHTSABER:
#endif
#endif
	    case GREEN_LIGHTSABER:
	    	turns = 1;
    	    	radius = 1;
#ifdef JEDI
		if (obj->oartifact == ART_LIGHTSABER_PROTOTYPE){
			do_timer = FALSE;
			obj->lamplit = 1;
		}
#endif
		break;
#endif
	    case POT_OIL:
		turns = obj->age;
		radius = 1;	/* very dim light */
		break;
#ifdef FIREARMS
	    case STICK_OF_DYNAMITE:
		turns = obj->age;
		radius = 1;     /* very dim light */
		break;
#endif

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
	    case NATURAL_CANDLE:
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
					BURN_OBJECT, (genericptr_t)obj)) {
		obj->lamplit = 1;
		obj->age -= turns;
		if (carried(obj) && !already_lit)
		    update_inventory();
	    } else {
		obj->lamplit = 0;
	    }
	} else {
	    if (carried(obj) && !already_lit)
		update_inventory();
	}

	if (obj->lamplit && !already_lit) {
	    xchar x, y;

	    if (get_obj_location(obj, &x, &y, CONTAINED_TOO|BURIED_TOO))
		new_light_source(x, y, radius, LS_OBJECT, (genericptr_t) obj);
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
	    impossible("end_burn: obj %s not lit", xname(obj));
	    return;
	}

	if (obj->otyp == MAGIC_LAMP || obj->otyp == MAGIC_CANDLE ||
#ifdef JEDI
		obj->oartifact == ART_LIGHTSABER_PROTOTYPE ||
#endif
		artifact_light(obj))
	    timer_attached = FALSE;

	if (!timer_attached) {
	    /* [DS] Cleanup explicitly, since timer cleanup won't happen */
	    del_light_source(LS_OBJECT, (genericptr_t)obj);
	    obj->lamplit = 0;
	    if (obj->where == OBJ_INVENT)
		update_inventory();
	} else if (!stop_timer(BURN_OBJECT, (genericptr_t) obj))
	    impossible("end_burn: obj %s not timed!", xname(obj));
}

#endif /* OVL1 */
#ifdef OVL0

/*
 * Cleanup a burning object if timer stopped.
 */
static void
cleanup_burn(arg, expire_time)
    genericptr_t arg;
    long expire_time;
{
    struct obj *obj = (struct obj *)arg;
    if (!obj->lamplit) {
	impossible("cleanup_burn: obj %s not lit", xname(obj));
	return;
    }

    del_light_source(LS_OBJECT, arg);

    /* restore unused time */
    obj->age += expire_time - monstermoves;

    obj->lamplit = 0;

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
    impossible("burn_faster: obj %s not lit", xname(obj));
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
	    nomul(-3, "hiding from thunderstorm");
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
 *							genericptr_t arg)
 *		Start a timer of kind 'kind' that will expire at time
 *		monstermoves+'timeout'.  Call the function at 'func_index'
 *		in the timeout table using argument 'arg'.  Return TRUE if
 *		a timer was started.  This places the timer on a list ordered
 *		"sooner" to "later".  If an object, increment the object's
 *		timer count.
 *
 *	long stop_timer(short func_index, genericptr_t arg)
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
STATIC_DCL const char *FDECL(kind_name, (SHORT_P));
STATIC_DCL void FDECL(print_queue, (winid, timer_element *));
#endif
STATIC_DCL void FDECL(insert_timer, (timer_element *));
STATIC_DCL timer_element *FDECL(remove_timer, (timer_element **, SHORT_P,
								genericptr_t));
STATIC_DCL void FDECL(write_timer, (int, timer_element *));
STATIC_DCL boolean FDECL(mon_is_local, (struct monst *));
STATIC_DCL boolean FDECL(timer_is_local, (timer_element *));
STATIC_DCL int FDECL(maybe_write_timer, (int, int, BOOLEAN_P));
static void FDECL(write_timer, (int, timer_element *)); /* Damn typedef write_timer is in the middle */

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
#ifdef FIREARMS
    TTAB(bomb_blow,     (timeout_proc)0,	"bomb_blow"),
#endif
#ifdef UNPOLYPILE
    TTAB(unpoly_obj,    cleanup_unpoly,		"unpoly_obj"),
#endif
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
	    Sprintf(buf, " %4ld   %4ld  %-6s %s(%s)",
		curr->timeout, curr->tid, kind_name(curr->kind),
		timeout_funcs[curr->func_index].name,
		fmt_ptr((genericptr_t)curr->arg, arg_address));
#else
	    Sprintf(buf, " %4ld   %4ld  %-6s #%d(%s)",
		curr->timeout, curr->tid, kind_name(curr->kind),
		curr->func_index,
		fmt_ptr((genericptr_t)curr->arg, arg_address));
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

    Sprintf(buf, "Current time = %ld.", monstermoves);
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
		      fmt_ptr((genericptr_t)obj, obj_address), curr->tid);
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

	if (curr->kind == TIMER_OBJECT) ((struct obj *)(curr->arg))->timed--;
	(*timeout_funcs[curr->func_index].f)(curr->arg, curr->timeout);
	free((genericptr_t) curr);
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
genericptr_t arg;
{
    timer_element *gnu;

    if (func_index < 0 || func_index >= NUM_TIME_FUNCS)
	panic("start_timer");

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
genericptr_t arg;
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
	free((genericptr_t) doomed);
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
	if (curr->kind == TIMER_OBJECT && curr->arg == (genericptr_t)src) {
	    curr->arg = (genericptr_t) dest;
	    dest->timed++;
	    count++;
	}
    if (count != src->timed)
	panic("obj_move_timers");
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
	if (curr->kind == TIMER_OBJECT && curr->arg == (genericptr_t)src) {
	    (void) start_timer(curr->timeout-monstermoves, TIMER_OBJECT,
					curr->func_index, (genericptr_t)dest);
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
	if (curr->kind == TIMER_OBJECT && curr->arg == (genericptr_t)obj) {
	    if (prev)
		prev->next = curr->next;
	    else
		timer_base = curr->next;
	    if (timeout_funcs[curr->func_index].cleanup)
		(*timeout_funcs[curr->func_index].cleanup)(curr->arg,
			curr->timeout);
	    free((genericptr_t) curr);
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
	if (curr->kind == TIMER_MONSTER && curr->arg == (genericptr_t)mon) {
	    if (prev)
		prev->next = curr->next;
	    else
		timer_base = curr->next;
	    if (timeout_funcs[curr->func_index].cleanup)
		(*timeout_funcs[curr->func_index].cleanup)(curr->arg,
			curr->timeout);
	    free((genericptr_t) curr);
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
genericptr_t arg;
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
    genericptr_t arg_save;

    switch (timer->kind) {
	case TIMER_GLOBAL:
	case TIMER_LEVEL:
	    /* assume no pointers in arg */
	    bwrite(fd, (genericptr_t) timer, sizeof(timer_element));
	    break;

	case TIMER_OBJECT:
	    if (timer->needs_fixup)
		bwrite(fd, (genericptr_t)timer, sizeof(timer_element));
	    else {
		/* replace object pointer with id */
		arg_save = timer->arg;
		timer->arg = (genericptr_t)((struct obj *)timer->arg)->o_id;
		timer->needs_fixup = 1;
		bwrite(fd, (genericptr_t)timer, sizeof(timer_element));
		timer->arg = arg_save;
		timer->needs_fixup = 0;
	    }
	    break;

	case TIMER_MONSTER:
	    if (timer->needs_fixup)
		bwrite(fd, (genericptr_t)timer, sizeof(timer_element));
	    else {
		/* replace monster pointer with id */
		arg_save = timer->arg;
		timer->arg = (genericptr_t)((struct monst *)timer->arg)->m_id;
		timer->needs_fixup = 1;
		bwrite(fd, (genericptr_t)timer, sizeof(timer_element));
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
genericptr_t arg;
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
	free((genericptr_t) timer);
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
    switch (obj->where) {
	case OBJ_INVENT:
	case OBJ_MIGRATING:	return FALSE;
	case OBJ_FLOOR:
	case OBJ_BURIED:	return TRUE;
	case OBJ_CONTAINED:	return obj_is_local(obj->ocontainer);
	case OBJ_MINVENT:	return mon_is_local(obj->ocarry);
    }
    panic("obj_is_local: %s, %d", cxname(obj), obj->where); /* improvement by Patric Mueller */
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
	    bwrite(fd, (genericptr_t) &timer_id, sizeof(timer_id));

	count = maybe_write_timer(fd, range, FALSE);
	bwrite(fd, (genericptr_t) &count, sizeof count);
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
		free((genericptr_t) curr);
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
	mread(fd, (genericptr_t) &timer_id, sizeof timer_id);

    /* restore elements */
    mread(fd, (genericptr_t) &count, sizeof count);
    while (count-- > 0) {
	curr = (timer_element *) alloc(sizeof(timer_element));
	mread(fd, (genericptr_t) curr, sizeof(timer_element));
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
		    if (!lookup_id_mapping((unsigned)curr->arg, &nid))
			panic("relink_timers 1");
		} else
		    nid = (unsigned) curr->arg;
		curr->arg = (genericptr_t) find_oid(nid);
		if (!curr->arg) impossible("cant find o_id %d", nid);
		curr->needs_fixup = 0;
	    } else if (curr->kind == TIMER_MONSTER) {
/*                panic("relink_timers: no monster timer implemented");*/
                /* WAC attempt to relink monster timers based on above
                 * and light source code
                 */
		if (ghostly) {
		    if (!lookup_id_mapping((unsigned)curr->arg, &nid))
                        panic("relink_timers 1b");
		} else
		    nid = (unsigned) curr->arg;
                curr->arg = (genericptr_t) find_mid(nid, FM_EVERYWHERE);
		if (!curr->arg) panic("cant find m_id %d", nid);
		curr->needs_fixup = 0;
	    } else
		panic("relink_timers 2");
	}
    }
}

#endif /* OVL0 */

/*timeout.c*/
