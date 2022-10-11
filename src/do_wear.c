/*	SCCS Id: @(#)do_wear.c	3.4	2003/11/14	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifndef OVLB

STATIC_DCL long takeoff_mask, taking_off;

#else /* OVLB */

STATIC_OVL NEARDATA long takeoff_mask = 0L;
static NEARDATA long taking_off = 0L;

static NEARDATA int todelay;
static boolean cancelled_don = FALSE;

#define KEEN 		10000	/* memory increase reading the book */
#define MAX_KNOW 	70000	/* Absolute Max timeout */
#define spellid(spell)          spl_book[spell].sp_id
#define incrnknow(spell)        spl_book[spell].sp_know = ((spl_book[spell].sp_know < 1) ? KEEN \
				 : ((spl_book[spell].sp_know + KEEN) > MAX_KNOW) ? MAX_KNOW \
				 : spl_book[spell].sp_know + KEEN)
#define boostknow(spell,boost)  spl_book[spell].sp_know = ((spl_book[spell].sp_know + boost > MAX_KNOW) ? MAX_KNOW \
				 : spl_book[spell].sp_know + boost)

static NEARDATA const char see_yourself[] = "see yourself";
static NEARDATA const char unknown_type[] = "Unknown type of %s (%d)";
static NEARDATA const char unknown_type_long[] = "Unknown type of %s (%ld)";
static NEARDATA const char c_armor[]  = "armor",
			   c_suit[]   = "suit",
			   c_shirt[]  = "shirt",
			   c_cloak[]  = "cloak",
			   c_gloves[] = "gloves",
			   c_boots[]  = "boots",
			   c_helmet[] = "helmet",
			   c_shield[] = "shield",
			   c_weapon[] = "weapon",
			   c_sword[]  = "sword",
			   c_axe[]    = "axe",
			   c_that_[]  = "that";

static NEARDATA const long takeoff_order[] = { WORN_BLINDF, W_WEP,
	WORN_SHIELD, WORN_GLOVES, LEFT_RING, RIGHT_RING, WORN_CLOAK,
	WORN_HELMET, WORN_AMUL, WORN_IMPLANT, WORN_ARMOR,
	WORN_SHIRT,
	WORN_BOOTS, W_SWAPWEP, W_QUIVER, 0L };

STATIC_DCL void on_msg(struct obj *);
STATIC_DCL void on_msgdel(struct obj *);
STATIC_DCL void Ring_off_or_gone(struct obj *, BOOLEAN_P);
STATIC_PTR int select_off(struct obj *);
STATIC_DCL struct obj *do_takeoff(void);
STATIC_PTR int take_off(void);
STATIC_DCL int menu_remarm(int);
STATIC_DCL void already_wearing(const char*);
STATIC_DCL void already_wearing2(const char*, const char*);

void
off_msg(otmp)
register struct obj *otmp;
{
	if (!otmp) return;

	if(flags.verbose)
	    You("were wearing %s.", doname(otmp));
}

/* for items that involve no delay */
STATIC_OVL void
on_msg(otmp)
register struct obj *otmp;
{
	if (!otmp) return;

	if (flags.verbose) {
	    char how[BUFSZ];

	    how[0] = '\0';
	    if (otmp->otyp == TOWEL)
		sprintf(how, " around your %s", body_part(HEAD));
	    You("are now wearing %s%s.",
		obj_is_pname(otmp) ? the(xname(otmp)) : an(xname(otmp)),
		how);
	}
}

STATIC_OVL void
on_msgdel(otmp)
register struct obj *otmp;
{
	if (!otmp) return;

	if (flags.verbose) {
	    char how[BUFSZ];

	    how[0] = '\0';
	    if (otmp->otyp == TOWEL)
		sprintf(how, " around your %s", body_part(HEAD));
	    You("are putting on %s%s.",
		obj_is_pname(otmp) ? the(xname(otmp)) : an(xname(otmp)),
		how);

	    if ((otmp->otyp == BLACK_DRAGON_SCALE_MAIL) || (otmp->otyp == BLUE_DRAGON_SCALE_MAIL) || (otmp->otyp == BRASS_DRAGON_SCALE_MAIL) || (otmp->otyp == BLOOD_DRAGON_SCALE_MAIL) ) pline("How kinky."); /* idea by FIQ */
	}
}

/*
 * The Type_on() functions should be called *after* setworn().
 * The Type_off() functions call setworn() themselves.
 */

int
Boots_on()
{
    long oldprop; 
    if (!uarmf) return 0; 
    oldprop = u.uprops[objects[uarmf->otyp].oc_oprop].extrinsic & ~WORN_BOOTS;

    switch(uarmf->otyp) {
	case PLASTEEL_BOOTS:
	case LOW_BOOTS:
	case IRON_SHOES:
	case HIGH_BOOTS:
	case CRYSTAL_BOOTS:
	case GNOMISH_BOOTS:
	case WEDGE_SANDALS:
	case FEMININE_PUMPS:
	case DANCING_SHOES:
	case SWEET_MOCASSINS:
	case SOFT_SNEAKERS:
	case LEATHER_PEEP_TOES:
	case COMBAT_STILETTOS:
	case ITALIAN_HEELS:
	case LADY_BOOTS:
	case STILETTO_SANDALS:
	case JUMPING_BOOTS:
	case FLYING_BOOTS:
	case KICKING_BOOTS:
	case FIRE_BOOTS:
	case BOOTS_OF_ACID_RESISTANCE:
	case RUBBER_BOOTS:
	case LEATHER_SHOES:
	case SNEAKERS:
	case BOOTS_OF_PLUGSUIT:
	case FIELD_BOOTS:
	case UGG_BOOTS:
	case BOOTS_OF_SAFEGUARD:
	case BOOTS_OF_FREEDOM:
	case FREEZING_BOOTS:
	case PASSTHROUGH_BOOTS:
	case HIGH_STILETTOS:
	case BOOTS_OF_TOTAL_STABILITY:
	case BOOTS_OF_DISPLACEMENT:
	case BOOTS_OF_SWIMMING:
	case ANTI_CURSE_BOOTS:
	case BOOTS_OF_SHOCK_RESISTANCE:
	case BOOTS_OF_INVERSION:
	case SKY_HIGH_HEELS:
	case PREHISTORIC_BOOTS:
	case SYNTHETIC_SANDALS:
		break;
	case BOOTS_OF_MOLASSES:
		pline(FunnyHallu ? "Icy legs, how cool!" : "These boots feel a little cold...");
		break;
	case ZIPPER_BOOTS:
		pline(FunnyHallu ? "Lovely fleecy caressing feelings stream into your legs!" : "While putting on this pair of boots, their zippers try to scratch your legs!");
		losehp(rnd(20), "foolishly putting on a zipper boot", KILLED_BY);
		break;
	case HIPPIE_HEELS:
		if (flags.female) pline("You're dressed like a '69 whore!");
		else pline("You're dressed like a frickin' hardrocker!");
	    if (!uarmf->cursed) {curse(uarmf); pline("The highly erotic boots weld themselves to your feet!");}
		break;

	case AUTODESTRUCT_DE_VICE_BOOTS:
	case SPEEDBUG_BOOTS:
	case DISCONNECTED_BOOTS:
	case BOSS_BOOTS:
	case SENTIENT_HIGH_HEELED_SHOES:
	case BOOTS_OF_FAINTING:
	case DIFFICULT_BOOTS:
	case BOOTS_OF_WEAKNESS:
	case GRIDBUG_CONDUCT_BOOTS:
	case DISENCHANTING_BOOTS:
	case LIMITATION_BOOTS:
	case PET_STOMPING_PLATFORM_BOOTS:
	case ASS_KICKER_BOOTS:
	case DEMENTIA_BOOTS:

	case COMPETITION_BOOTS:
	case QUASIMODULAR_BOOTS:
	case SINFUL_HEELS:
	case BLOODSUCKING_SHOES:
	case COVETED_BOOTS:
	case LIGHTLESS_BOOTS:
	case KILLER_HEELS:
	case CHECKER_BOOTS:
	case ELVIS_SHOES:
	case AIRSTEP_BOOTS:
	case BOOTS_OF_INTERRUPTION:
	case HIGH_HEELED_SKIERS:
	case HIGH_SCORING_HEELS:
	case REPEATABLE_BOOTS:
	case TRON_BOOTS:
	case KILLER_SPAWN_BOOTS:
	case RED_SPELL_HEELS:
	case DESTRUCTIVE_HEELS:

	case VIOLET_BEAUTY_HEELS:
	case LONG_POINTY_HEELS:
	case SELF_WILLED_HEELS:
	case SOIL_CLINGING_BOOTS:
	case PERMANENTLY_BLACK_SHOES:
	case AUTOSCOOTER_HEELS:
	case FORCEFUL_BOOTS:
	case BUM_BUM_BOOTS:
	case SADISTIC_BOOTS:
	case FLUCKERING_BOOTS:
	case SPRAP_BOOTS:
	case FAILED_ATTEMPT_BOOTS:
	case BRIGHT_CYAN_BEAUTIES:
	case SIA_BOOTS:
	case PLATINUM_SPELL_BOOTS:
	case INVIS_WALKERS:
	case NOISY_BOOTS:
	case MORK_BOOTS:

	case STAIRWELL_STOMPING_BOOTS:
	case UNFAIR_STILETTOS:
	case DUMMY_BOOTS_A:
	case DUMMY_BOOTS_B:
	case DUMMY_BOOTS_C:
	case DUMMY_BOOTS_D:
	case DUMMY_BOOTS_E:
	case DUMMY_BOOTS_F:
	case DUMMY_BOOTS_G:
	case DUMMY_BOOTS_H:
	case DUMMY_BOOTS_I:
	case DUMMY_BOOTS_J:
	case DUMMY_BOOTS_K:
	case DUMMY_BOOTS_L:
	case DUMMY_BOOTS_M:
	case DUMMY_BOOTS_N:
	case DUMMY_BOOTS_O:
	case DUMMY_BOOTS_P:
	case DUMMY_BOOTS_Q:
	case DUMMY_BOOTS_R:
	case DUMMY_BOOTS_S:
	case DUMMY_BOOTS_T:
	case DUMMY_BOOTS_U:
	case DUMMY_BOOTS_V:
	case DUMMY_BOOTS_W:
	case DUMMY_BOOTS_X:
	case DUMMY_BOOTS_Y:
	case DUMMY_BOOTS_Z:
	case DUMMY_BOOTS_AA:
	case DUMMY_BOOTS_AB:
	case DUMMY_BOOTS_AC:
	case DUMMY_BOOTS_AD:
	case DUMMY_BOOTS_AE:
	case DUMMY_BOOTS_AF:
	case DUMMY_BOOTS_AG:
	case DUMMY_BOOTS_AH:
	case DUMMY_BOOTS_AI:
	case DUMMY_BOOTS_AJ:
	case DUMMY_BOOTS_AK:
	case DUMMY_BOOTS_AL:
	case DUMMY_BOOTS_AM:
	case DUMMY_BOOTS_AN:
	case DUMMY_BOOTS_AO:
	case DUMMY_BOOTS_AP:
	case DUMMY_BOOTS_AQ:
	case DUMMY_BOOTS_AR:
	case DUMMY_BOOTS_AS:
	case DUMMY_BOOTS_AT:
	case DUMMY_BOOTS_AU:
	case DUMMY_BOOTS_AV:
	case DUMMY_BOOTS_AW:
	case DUMMY_BOOTS_AX:
		if (!uarmf->cursed) curse(uarmf);
		break;

	case FEMMY_STILETTO_BOOTS:
	case MADELEINE_PLATEAU_BOOTS:
	case MARLENA_HIKING_BOOTS:
	case ANASTASIA_DANCING_SHOES:
	case JESSICA_LADY_SHOES:
	case SOLVEJG_MOCASSINS:
	case WENDY_LEATHER_PUMPS:
	case KATHARINA_PLATFORM_BOOTS:
	case ELENA_COMBAT_BOOTS:
	case THAI_COMBAT_BOOTS:
	case ELIF_SNEAKERS:
	case NADJA_BUCKLED_LADY_SHOES:
	case SANDRA_COMBAT_BOOTS:
	case NATALJE_BLOCK_HEEL_SANDALS:
	case JEANETTA_GIRL_BOOTS:
	case YVONNE_GIRL_SNEAKERS:
	case MAURAH_HUGGING_BOOTS:
	case MELTEM_COMBAT_BOOTS:
	case SARAH_HUGGING_BOOTS:
	case CLAUDIA_WOODEN_SANDALS:
	case LUDGERA_HIKING_BOOTS:
	case KATI_GIRL_BOOTS:
	case NELLY_LADY_PUMPS:
	case EVELINE_WEDGE_SANDALS:
	case KARIN_LADY_SANDALS:
	case JUEN_PEEP_TOES:
	case KRISTINA_PLATFORM_SNEAKERS:
	case LOU_SNEAKERS:
	case ALMUT_SNEAKERS:
	case JULIETTA_PEEP_TOES:
	case ARABELLA_HUGGING_BOOTS:
	case KRISTIN_COMBAT_BOOTS:
	case ANNA_HUGGING_BOOTS:
	case RUEA_COMBAT_BOOTS:
	case DORA_COMBAT_BOOTS:
	case MARIKE_SNEAKERS:
	case JETTE_COMBAT_BOOTS:
	case INA_HUGGING_BOOTS:
	case SING_PLATFORM_BOOTS:
	case VICTORIA_COMBAT_BOOTS:
	case MELISSA_WEDGE_BOOTS:
	case ANITA_LADY_PUMPS:
	case HENRIETTA_COMBAT_BOOTS:
	case VERENA_STILETTO_SANDALS:
	case ANNEMARIE_PLATFORM_SANDALS:
	case JIL_VELCRO_SHOES:
	case JANA_SNEAKERS:
	case KATRIN_COMBAT_BOOTS:
	case GUDRUN_WEDGE_SANDALS:
	case ELLA_WEDGE_SANDALS:
	case MANUELA_COMBAT_BOOTS:
	case JENNIFER_COMBAT_BOOTS:
	case PATRICIA_STEEL_CAPPED_SANDALS:
	case ANTJE_SNEAKERS:
	case ANTJE_COMBAT_BOOTS:
	case KERSTIN_COMBAT_BOOTS:
	case KERSTIN_WOODEN_SANDALS:
	case KERSTIN_SNEAKERS:
	case LAURA_WINTER_BOOTS:
	case LARISSA_PLATFORM_BOOTS:
	case NORA_COMBAT_BOOTS:
	case NATALIA_WEDGE_SANDALS:
	case SUSANNE_COMBAT_BOOTS:
	case LISA_COMBAT_BOOTS:
	case BRIDGHITTE_SKI_HEELS:
	case JULIA_COMBAT_BOOTS:
	case NICOLE_COMBAT_BOOTS:
	case RITA_STILETTOS:
	case JANINA_LADY_PUMPS:
	case ROSA_GIRL_SHOES:
	case KSENIA_PLATFORM_SANDALS:
	case LYDIA_LADY_PUMPS:
	case CONNY_COMBAT_BOOTS:
	case KATIA_COMBAT_BOOTS:
	case MARIYA_COMBAT_BOOTS:
	case ELISE_HIPPIE_HEELS:
	case RONJA_COMBAT_BOOTS:
	case ARIANE_COMBAT_BOOTS:
	case JOHANNA_COMBAT_BOOTS:
	case INGE_COMBAT_BOOTS:

		if (!uarmf->cursed) {
			pline("Oh whoops, your footwear welds itself to your %s. This might be the result of a curse.%s", makeplural(body_part(FOOT)), FunnyHallu ? " (Thank you, Captain Obvious.)" : "" );
			curse(uarmf);
		}		
		break;

	case WATER_WALKING_BOOTS:
		if (u.uinwater) spoteffects(TRUE);
		break;
	case SPEED_BOOTS:
		/* Speed boots are still better than intrinsic speed, */
		/* though not better than potion speed */
		if (!oldprop && !(HFast & TIMEOUT)) {
			makeknown(uarmf->otyp);
			You_feel("yourself speed up%s.",
				(oldprop || HFast) ? " a bit more" : "");
		}
		break;
	case ELVEN_BOOTS:
		if (!oldprop && !HStealth && !BStealth) {
			makeknown(uarmf->otyp);
			You("walk very quietly.");
		}
		break;
	case FUMBLE_BOOTS:
	case ATSUZOKO_BOOTS:
	case ROLLER_BLADE:
		if (!oldprop && !(HFumbling & ~TIMEOUT))
			incr_itimeout(&HFumbling, rnd(20));
		break;
	case MULTI_SHOES:
		if (!oldprop && !(HFumbling & ~TIMEOUT))
			incr_itimeout(&HFumbling, rnd(20));
	      if (!uarmf->cursed) {
			curse(uarmf);
			pline("You get the feeling that these shoes don't want to be taken off...");
		}
		break;
	case CARRYING_BOOTS:
		vision_full_recalc = 1;
		break;
	case STOMPING_BOOTS:
		EAggravate_monster |= WORN_BOOTS;
		break;
	case LEVITATION_BOOTS:
		if (!oldprop && !HLevitation) {
			flags.botl = 1;
			makeknown(uarmf->otyp);
			float_up();
			spoteffects(FALSE);
		}
		break;
	default: impossible(unknown_type_long, c_boots, uarmf->otyp);
    }

    if (uarmf && itemhasappearance(uarmf, APP_VELCRO_BOOTS) ) {
	      if (!uarmf->cursed) {
			curse(uarmf);
			pline("The velcro boots constrict your %s, and you can't take them off again!", makeplural(body_part(FOOT)) );
		}

    }

    if (uarmf && !Role_if(PM_BINDER) && uarmf->oartifact == ART_BINDER_CRASH) {
		if (!(u.monstertimefinish % 20)) { /* 1 in 20 games */
			bindertransformation();
			pline("You turn into a Binder!");
		} else {
			curse(uarmf);
			uarmf->hvycurse = 1;
			u.ugangr += 3;
			pline("You hear a frightening crash in the distance...");
		}
    }

    if (uarmf && uarmf->oartifact == ART_PRACTICLASSY && !flags.hybridbeacher && !(quest_status.killed_nemesis)) {
		flags.hybridbeacher = TRUE;
		flags.hybridization++;
		pline("%s thunders: 'Alright %s you little practicant maggot, get ready for another hard day of work! You're late by 5 minutes but that's your loss! If you do your work properly this time you'll not get any problems with me, but if you step out of line you'll pay zorkmids for each of your offenses!'", noroelaname(), playeraliasname);

    }

    if (uarmf && uarmf->oartifact == ART_MEPHISTO_S_BROGUES) {
		if (!uarmf->cursed) {
			curse(uarmf);
			pline("A demonic aura envelopes your boots.");
		}
    }

    if (uarmf && uarmf->oartifact == ART_HERSAY_PRICE) {
		if (!uarmf->hvycurse) {
			curse(uarmf);
			uarmf->hvycurse = TRUE;
			pline("Now you have to pay the price for using her abilities.");
		}
    }
    if (uarmf && uarmf->oartifact == ART_DARK_BALL_OF_LIGHT) {
		if (!uarmf->hvycurse) {
			curse(uarmf);
			uarmf->hvycurse = TRUE;
			pline("These dark boots won't come off easily.");
		}
		if (!uarmf->stckcurse) {
			uarmf->stckcurse = TRUE;
		}
    }

    if (uarmf && uarmf->oartifact == ART_PRADA_S_DEVIL_WEAR) {
		if (!uarmf->cursed) {
			curse(uarmf);
			pline("The devils curse your boots.");
		}
    }

    if (uarmf && uarmf->oartifact == ART_RITA_S_TENDER_STILETTOS) {
		if (!uarmf->cursed) curse(uarmf);
		if (uarmf->spe > -10) uarmf->spe = -10;
    }

    if (uarmf && uarmf->oartifact == ART_GIGGLING) {
		if (uarmf->spe > -10) uarmf->spe = -10;
    }

    if (uarmf && uarmf->oartifact == ART_PHANTO_S_RETARDEDNESS) {
		if (!uarmf->cursed) curse(uarmf);
		uarmf->hvycurse = uarmf->prmcurse = uarmf->stckcurse = TRUE;
		pline("Now you look like a total retard and can no longer take off these boots.");
    }

    if (uarmf && uarmf->oartifact == ART_RNG_S_SEXINESS && uarmf->spe == 0) {
		if (!rn2(2)) uarmf->spe = rnd(5);
		else uarmf->spe = -(rnd(5));
    }

    if (uarmf && uarmf->oartifact == ART_RNG_S_BEAUTY && uarmf->spe == 0) {
		if (!rn2(2)) uarmf->spe = rnd(7);
		else uarmf->spe = -(rnd(7));
    }

    if (uarmf && uarmf->oartifact == ART_ELENA_S_CHALLENGE) {
		if (!uarmf->cursed) {
			curse(uarmf);
			pline("You decided to enact Elena's Challenge, which means that all men need to endure being kicked by you in an attempt to 'conquer' you. In order to prevent you from simply ending the challenge prematurely, your block-heeled combat boots just welded themselves to your %s.", makeplural(body_part(FOOT)));
		}
    }

    if (uarmf && uarmf->oartifact == ART_ELLA_S_BLOODLUST) {
		if (!uarmf->cursed) {
			curse(uarmf);
			if (!strncmpi(plname, "Ella", 5) || !strncmpi(plalias, "Ella", 5)) pline("You finally got your bloodthirsty footwear back! They immediately make sure they won't get separated from you.");
			else pline("You decided to pretend to be Ella? Now prove it!");
		}
    }

    if (uarmf && uarmf->oartifact == ART_SOLVEJG_S_STINKING_SLIPPER) {
		if (!uarmf->cursed) {
			curse(uarmf);
			uarmf->hvycurse = 1;
			pline("As you put on these erotic mocassins, you immediately get all wet and horny. They're sooooooo cute!!! And they also emit a beguiling (but vile) stench that completely prevents you from resisting their beauty. Thankfully they are enchanted (or cursed?) in a way that makes it very difficult to take them off again.");
		}
    }

    if (uarmf && uarmf->oartifact == ART_ZERDROY_GUNNING) {

		if (!tech_known(T_CREATE_AMMO)) {
		    	learntech(T_CREATE_AMMO, FROMOUTSIDE, 1);
		    	pline("Suddenly, you know how to use the create ammo technique!");
		}

    }

    if (uarmf && uarmf->oartifact == ART_ANASTASIA_S_UNEXPECTED_ABI) {
		int extraabilities = 0;
		if (!uarmf->cursed) curse(uarmf);

		if (!tech_known(T_ATTIRE_CHARM)) {
		    	learntech(T_ATTIRE_CHARM, FROMOUTSIDE, 1);
		    	pline("Suddenly, you know how to use the attire charm technique!");
			extraabilities++;
		}

		if (!tech_known(T_WORLD_FALL)) {
		    	learntech(T_WORLD_FALL, FROMOUTSIDE, 1);
		    	pline("Suddenly, you know how to use the world fall technique!");
			extraabilities++;
		}

		if (extraabilities > 1) extraabilities = 9;
		else if (extraabilities) extraabilities = 3;

		while (extraabilities > 0) {
			extraabilities--;
			deacrandomintrinsic(1000000);
		}

    }

    if (uarmf && uarmf->oartifact == ART_KYLIE_LUM_S_SNAKESKIN_BOOT) {
		u.kyliemode = 1;
		if (Role_if(PM_FAILED_EXISTENCE)) {
			pline("BAD MOVE!!! A failed existence pretending to be Kylie draws the wrath of the gods.");
			NastinessProblem |= FROMOUTSIDE;
		}
    }

    if (uarmf && uarmf->oartifact == ART_LILAC_BEAUTY) {
		pline("Your incredibly beautiful footwear speaks:");
		verbalize("%s, you're not our owner. We allow you to wear us anyway, but if you ever dare to take us off, we will scratch your shins to the bone and rip your unprotected skin to shreds. Keep wearing us if you want to avoid a bloodbath!", playeraliasname);
    }

    if (uarmf && uarmf->oartifact == ART_I_M_A_BITCH__DEAL_WITH_IT && uarmf->spe < 0) {
		uarmf->spe = 0;
    }

    if (uarmf && uarmf->oartifact == ART_MANUELA_S_TORTURE_HEELS) {
		uarmf->hvycurse = 1; /* already autocursed above */
		pline("Manuela put a terrible curse on this beautiful pair of red block-heeled combat boots.");
    }

    if (uarmf && uarmf->oartifact == ART_MANUELA_S_UNKNOWN_HEELS) {
		curse(uarmf);
		uarmf->hvycurse = 1;
		pline("Manuela put a terrible curse on this beautiful pair of high heels.");
    }

    if (uarmf && uarmf->oartifact == ART_CURSING_ANOMALY) {
		if (!uarmf->cursed) {
			curse(uarmf);
			pline("Your boots feel deathly cold. But seriously, you did expect them to autocurse, right?");
		}
    }

    if (uarmf && uarmf->oartifact == ART_AMY_LOVES_AUTOCURSING_ITEM) {
		if (!uarmf->cursed) {
			curse(uarmf);
			pline("Your boots feel deathly cold. But seriously, you did expect them to autocurse, right?");
		}
    }

    if (uarmf && uarmf->oartifact == ART_HENRIETTA_S_DOGSHIT_BOOTS) {
		if (!uarmf->cursed) {
			curse(uarmf);
			pline("Congratulations, you can now walk around in a pair of boots that won't come off, and whose previous owner fully stepped into a heap of dog shit.");
		}
    }

    if (uarmf && uarmf->oartifact == ART_RATCH_CLOSURE_SCRATCHING && uarmf->spe < 1) uarmf->spe++;

	if (uarmf && (AutocursingEquipment || u.uprops[AUTOCURSE_EQUIP].extrinsic || have_autocursestone())) curse(uarmf);

	if (uarmf && uarmf->spe > -10 && (TrashingBugEffect || u.uprops[TRASHING_EFFECT].extrinsic || have_trashstone())) uarmf->spe--;

    return 0;
}

int
Boots_off()
{
    if (uarmf && uarmf->oartifact == ART_LILAC_BEAUTY) {
		pline("You angered your beautiful lilac pumps!");
		pline("In a terrible blood rush, they scratch up and down your legs again and again. Your blood squirts everywhere while the tender, but very sharp-edged heel is destroying every shred of your skin it can reach.");
		losexp("being scratched to death by an angry pair of lilac pumps", TRUE, FALSE);
		pline("Frustrated, the lovely leather pumps settle down.");
    }

    int otyp = uarmf->otyp;
    long oldprop = u.uprops[objects[otyp].oc_oprop].extrinsic & ~WORN_BOOTS;

    takeoff_mask &= ~W_ARMF;
	/* For levitation, float_down() returns if Levitation, so we
	 * must do a setworn() _before_ the levitation case.
	 */
    setworn((struct obj *)0, W_ARMF);
    switch (otyp) {
	case SPEED_BOOTS:
		if (!Very_fast && !cancelled_don) {
			makeknown(otyp);
			You_feel("yourself slow down%s.",
				Fast ? " a bit" : "");
		}
		break;
	case WATER_WALKING_BOOTS:
		if (is_pool(u.ux,u.uy) && !Levitation && !Flying &&
		    !is_clinger(youmonst.data) && !cancelled_don) {
			makeknown(otyp);
			/* make boots known in case you survive the drowning */
			spoteffects(TRUE);
		}
		break;
	case ELVEN_BOOTS:
		if (!oldprop && !HStealth && !BStealth && !cancelled_don) {
			makeknown(otyp);
			You("sure are noisy.");
		}
		break;
	case FUMBLE_BOOTS:
	case ATSUZOKO_BOOTS:
	case MULTI_SHOES:
	case ROLLER_BLADE:
		if (!oldprop && !u.fumbleduration && !(HFumbling & ~TIMEOUT))
			HFumbling = EFumbling = 0;
		break;
	case STOMPING_BOOTS:
		EAggravate_monster &= ~WORN_BOOTS;
		break;
	case CARRYING_BOOTS:
		vision_full_recalc = 1;
		break;
	case FLYING_BOOTS:
		(void) float_down(0L, 0L);
		break;
	case LEVITATION_BOOTS:
		if (!oldprop && !HLevitation && !cancelled_don) {
			flags.botl = 1;
			(void) float_down(0L, 0L);
			makeknown(otyp);
		}
		break;
	case ZIPPER_BOOTS:
		pline(FunnyHallu ? "You get the feeling that something soft just slid along the full length of your legs!" : "While taking off this pair of boots, you scratch open your legs at their zippers!");
		losehp(rnd(20), "foolishly taking off a zipper boot", KILLED_BY);
		    set_wounded_legs(LEFT_SIDE, HWounded_legs + rn1(35, 41));
		    set_wounded_legs(RIGHT_SIDE, HWounded_legs + rn1(35, 41));
		break;
	case LOW_BOOTS:
	case IRON_SHOES:
	case GNOMISH_BOOTS:
	case BOOTS_OF_MOLASSES:
	case WEDGE_SANDALS:
	case FEMININE_PUMPS:
	case DANCING_SHOES:
	case SWEET_MOCASSINS:
	case SOFT_SNEAKERS:
	case LEATHER_PEEP_TOES:
	case COMBAT_STILETTOS:
	case ITALIAN_HEELS:
	case LADY_BOOTS:
	case STILETTO_SANDALS:
	case HIGH_BOOTS:
	case CRYSTAL_BOOTS:
	case JUMPING_BOOTS:
	case KICKING_BOOTS:
	case FIRE_BOOTS:
	case AUTODESTRUCT_DE_VICE_BOOTS:
	case SPEEDBUG_BOOTS:
	case DUMMY_BOOTS_A:
	case DUMMY_BOOTS_B:
	case DUMMY_BOOTS_C:
	case DUMMY_BOOTS_D:
	case DUMMY_BOOTS_E:
	case DUMMY_BOOTS_F:
	case DUMMY_BOOTS_G:
	case DUMMY_BOOTS_H:
	case DUMMY_BOOTS_I:
	case DUMMY_BOOTS_J:
	case DUMMY_BOOTS_K:
	case DUMMY_BOOTS_L:
	case DUMMY_BOOTS_M:
	case DUMMY_BOOTS_N:
	case DUMMY_BOOTS_O:
	case DUMMY_BOOTS_P:
	case DUMMY_BOOTS_Q:
	case DUMMY_BOOTS_R:
	case DUMMY_BOOTS_S:
	case DUMMY_BOOTS_T:
	case DUMMY_BOOTS_U:
	case DUMMY_BOOTS_V:
	case DUMMY_BOOTS_W:
	case DUMMY_BOOTS_X:
	case DUMMY_BOOTS_Y:
	case DUMMY_BOOTS_Z:
	case DUMMY_BOOTS_AA:
	case DUMMY_BOOTS_AB:
	case DUMMY_BOOTS_AC:
	case DUMMY_BOOTS_AD:
	case DUMMY_BOOTS_AE:
	case DUMMY_BOOTS_AF:
	case DUMMY_BOOTS_AG:
	case DUMMY_BOOTS_AH:
	case DUMMY_BOOTS_AI:
	case DUMMY_BOOTS_AJ:
	case DUMMY_BOOTS_AK:
	case DUMMY_BOOTS_AL:
	case DUMMY_BOOTS_AM:
	case DUMMY_BOOTS_AN:
	case DUMMY_BOOTS_AO:
	case DUMMY_BOOTS_AP:
	case DUMMY_BOOTS_AQ:
	case DUMMY_BOOTS_AR:
	case DUMMY_BOOTS_AS:
	case DUMMY_BOOTS_AT:
	case DUMMY_BOOTS_AU:
	case DUMMY_BOOTS_AV:
	case DUMMY_BOOTS_AW:
	case DUMMY_BOOTS_AX:
	case DISCONNECTED_BOOTS:
	case BOSS_BOOTS:
	case SENTIENT_HIGH_HEELED_SHOES:
	case BOOTS_OF_FAINTING:
	case DIFFICULT_BOOTS:
	case BOOTS_OF_WEAKNESS:
	case GRIDBUG_CONDUCT_BOOTS:
	case DISENCHANTING_BOOTS:
	case LIMITATION_BOOTS:
	case PET_STOMPING_PLATFORM_BOOTS:
	case ASS_KICKER_BOOTS:
	case DEMENTIA_BOOTS:

	case COMPETITION_BOOTS:
	case QUASIMODULAR_BOOTS:
	case SINFUL_HEELS:
	case BLOODSUCKING_SHOES:
	case COVETED_BOOTS:
	case LIGHTLESS_BOOTS:
	case KILLER_HEELS:
	case CHECKER_BOOTS:
	case ELVIS_SHOES:
	case AIRSTEP_BOOTS:
	case BOOTS_OF_INTERRUPTION:
	case HIGH_HEELED_SKIERS:
	case HIGH_SCORING_HEELS:
	case REPEATABLE_BOOTS:
	case TRON_BOOTS:
	case KILLER_SPAWN_BOOTS:
	case RED_SPELL_HEELS:
	case DESTRUCTIVE_HEELS:

	case VIOLET_BEAUTY_HEELS:
	case LONG_POINTY_HEELS:
	case SELF_WILLED_HEELS:
	case SOIL_CLINGING_BOOTS:
	case PERMANENTLY_BLACK_SHOES:
	case AUTOSCOOTER_HEELS:
	case FORCEFUL_BOOTS:
	case BUM_BUM_BOOTS:
	case SADISTIC_BOOTS:
	case FLUCKERING_BOOTS:
	case SPRAP_BOOTS:
	case FAILED_ATTEMPT_BOOTS:
	case BRIGHT_CYAN_BEAUTIES:
	case SIA_BOOTS:
	case PLATINUM_SPELL_BOOTS:
	case INVIS_WALKERS:
	case NOISY_BOOTS:
	case MORK_BOOTS:

	case STAIRWELL_STOMPING_BOOTS:
	case BOOTS_OF_ACID_RESISTANCE:
	case RUBBER_BOOTS:
	case LEATHER_SHOES:
	case SNEAKERS:
	case BOOTS_OF_PLUGSUIT:
	case FIELD_BOOTS:
	case UGG_BOOTS:
	case BOOTS_OF_SAFEGUARD:
	case BOOTS_OF_FREEDOM:
	case HIGH_STILETTOS:
	case BOOTS_OF_TOTAL_STABILITY:
	case BOOTS_OF_DISPLACEMENT:
	case BOOTS_OF_SWIMMING:
	case ANTI_CURSE_BOOTS:
	case BOOTS_OF_SHOCK_RESISTANCE:
	case BOOTS_OF_INVERSION:
	case FREEZING_BOOTS:
	case PASSTHROUGH_BOOTS:
	case UNFAIR_STILETTOS:
	case SKY_HIGH_HEELS:
	case PREHISTORIC_BOOTS:
	case SYNTHETIC_SANDALS:
	case PLASTEEL_BOOTS:

	case FEMMY_STILETTO_BOOTS:
	case MADELEINE_PLATEAU_BOOTS:
	case MARLENA_HIKING_BOOTS:
	case ANASTASIA_DANCING_SHOES:
	case JESSICA_LADY_SHOES:
	case SOLVEJG_MOCASSINS:
	case WENDY_LEATHER_PUMPS:
	case KATHARINA_PLATFORM_BOOTS:
	case ELENA_COMBAT_BOOTS:
	case THAI_COMBAT_BOOTS:
	case ELIF_SNEAKERS:
	case NADJA_BUCKLED_LADY_SHOES:
	case SANDRA_COMBAT_BOOTS:
	case NATALJE_BLOCK_HEEL_SANDALS:
	case JEANETTA_GIRL_BOOTS:
	case YVONNE_GIRL_SNEAKERS:
	case MAURAH_HUGGING_BOOTS:
	case MELTEM_COMBAT_BOOTS:
	case SARAH_HUGGING_BOOTS:
	case CLAUDIA_WOODEN_SANDALS:
	case LUDGERA_HIKING_BOOTS:
	case KATI_GIRL_BOOTS:
	case NELLY_LADY_PUMPS:
	case EVELINE_WEDGE_SANDALS:
	case KARIN_LADY_SANDALS:
	case JUEN_PEEP_TOES:
	case KRISTINA_PLATFORM_SNEAKERS:
	case LOU_SNEAKERS:
	case ALMUT_SNEAKERS:
	case JULIETTA_PEEP_TOES:
	case ARABELLA_HUGGING_BOOTS:
	case KRISTIN_COMBAT_BOOTS:
	case ANNA_HUGGING_BOOTS:
	case RUEA_COMBAT_BOOTS:
	case DORA_COMBAT_BOOTS:
	case MARIKE_SNEAKERS:
	case JETTE_COMBAT_BOOTS:
	case INA_HUGGING_BOOTS:
	case SING_PLATFORM_BOOTS:
	case VICTORIA_COMBAT_BOOTS:
	case MELISSA_WEDGE_BOOTS:
	case ANITA_LADY_PUMPS:
	case HENRIETTA_COMBAT_BOOTS:
	case VERENA_STILETTO_SANDALS:
	case ANNEMARIE_PLATFORM_SANDALS:
	case JIL_VELCRO_SHOES:
	case JANA_SNEAKERS:
	case KATRIN_COMBAT_BOOTS:
	case GUDRUN_WEDGE_SANDALS:
	case ELLA_WEDGE_SANDALS:
	case MANUELA_COMBAT_BOOTS:
	case JENNIFER_COMBAT_BOOTS:
	case PATRICIA_STEEL_CAPPED_SANDALS:
	case ANTJE_SNEAKERS:
	case ANTJE_COMBAT_BOOTS:
	case KERSTIN_COMBAT_BOOTS:
	case KERSTIN_WOODEN_SANDALS:
	case KERSTIN_SNEAKERS:
	case LAURA_WINTER_BOOTS:
	case LARISSA_PLATFORM_BOOTS:
	case NORA_COMBAT_BOOTS:
	case NATALIA_WEDGE_SANDALS:
	case SUSANNE_COMBAT_BOOTS:
	case LISA_COMBAT_BOOTS:
	case BRIDGHITTE_SKI_HEELS:
	case JULIA_COMBAT_BOOTS:
	case NICOLE_COMBAT_BOOTS:
	case RITA_STILETTOS:
	case JANINA_LADY_PUMPS:
	case ROSA_GIRL_SHOES:
	case KSENIA_PLATFORM_SANDALS:
	case LYDIA_LADY_PUMPS:
	case CONNY_COMBAT_BOOTS:
	case KATIA_COMBAT_BOOTS:
	case MARIYA_COMBAT_BOOTS:
	case ELISE_HIPPIE_HEELS:
	case RONJA_COMBAT_BOOTS:
	case ARIANE_COMBAT_BOOTS:
	case JOHANNA_COMBAT_BOOTS:
	case INGE_COMBAT_BOOTS:
		break;
	case HIPPIE_HEELS:

		pline("You decide you had enough of those hallucination-inducing boots.");
		break;

	default: impossible(unknown_type, c_boots, otyp);
    }

    cancelled_don = FALSE;
    return 0;
}

int
Cloak_on()
{
    long oldprop;
	if (!uarmc) return 0;
	oldprop = u.uprops[objects[uarmc->otyp].oc_oprop].extrinsic & ~WORN_CLOAK;

	if (uarmc && uarmc->oartifact == ART_DSCHLSCHLSCHLSCHLSCH) {
		u.uprops[DEAC_ANTIMAGIC].intrinsic += 1;
	}

	if (uarmf && uarmf->otyp == BOOTS_OF_SHOCK_RESISTANCE && uarmc && is_metallic(uarmc)) {
		You("receive a little shock.");
		losehp(rnd(2), "little electric shock", KILLED_BY_AN);
	}

    switch(uarmc->otyp) {
	case ELVEN_CLOAK:
	case CLOAK_OF_PROTECTION:
	case CLOAK_OF_DISPLACEMENT:
		makeknown(uarmc->otyp);
		break;
	case ORCISH_CLOAK:
	case DROVEN_CLOAK:
	case DWARVISH_CLOAK:
	case CLOAK_OF_MAGIC_RESISTANCE:
	case CLOAK_OF_REFLECTION:
	case ROBE:
	case ALCHEMY_SMOCK:
	case LEO_NEMAEUS_HIDE:
	case LEATHER_CLOAK:
	case CLOAK_OF_WARMTH:
	case CLOAK_OF_LEECH:
	case CLOAK_OF_COAGULATION:
	case CLOAK_OF_SCENT:
	case FILLER_CLOAK:
	case CLOAK_OF_GROUNDING:
	case CLOAK_OF_QUENCHING:
	case CLOAK_OF_CONFUSION:
	case MANACLOAK:
	case CLOAK_OF_INVERSION:
	case CLOAK_OF_WINCING:
	case MISSING_CLOAK:
	case TROLL_HIDE:
	case PLASTEEL_CLOAK:
	case LORICATED_CLOAK:
	/* KMH, balance patch -- removed */
	/* but re-inserted by Amy */
	case CLOAK_OF_DRAIN_RESISTANCE:              
	case AYANAMI_WRAPPING:
	case RUBBER_APRON:
	case KITCHEN_APRON:
	case FRILLED_APRON:
	case SUPER_MANTLE:
	case WINGS_OF_ANGEL:
	case DUMMY_WINGS:
	case FUR:
	case HIDE:
	case CLOAK_OF_PEACE:
	case CLOAK_OF_DIMNESS:
	case CLOAK_OF_CONTAMINATION_RESISTA:
	case DISPLACER_BEAST_HIDE:
	case THE_NEMEAN_LION_HIDE:
	case CLOAK_OF_SPRAY:
	case CLOAK_OF_FLAME:
	case CLOAK_OF_INSULATION:
	case CLOAK_OF_MATADOR:
	case CLOAK_OF_PAIN_SENSE:
	case SPECIAL_CLOAK:
	case PLAIN_CLOAK:
	case ARCHAIC_CLOAK:
	case CLOAK_OF_AWAKENING:
	case CLOAK_OF_STABILITY:
	case ANTI_DISQUIET_CLOAK:
	case HUGGING_GOWN:
	case COCLOAK:
	case CLOAK_OF_HEALTH:
	case CLOAK_OF_DISCOVERY:
	case CLOAK_OF_BLINDNESS_RESISTANCE:
	case CLOAK_OF_HALLUCINATION_RESISTA:
	case BIONIC_CLOAK:
	case CLOAK_OF_PORTATION:
	case CLOAK_OF_CONTROL:
	case CLOAK_OF_SHIFTING:
	case FLOATCLOAK:
	case CLOAK_OF_PRESCIENCE:
	case SENSOR_CLOAK:
	case CLOAK_OF_SPEED:
	case VAULT_CLOAK:
	case CLOAK_OF_SPELL_RESISTANCE:
	case CLOAK_OF_PHYSICAL_RESISTANCE:
	case OPERATION_CLOAK:
	case BESTEST_CLOAK:
	case CLOAK_OF_FREEDOM:
	case BIKINI:
	case CLOAK_OF_PERMANENCE:
	case CLOAK_OF_SLOW_DIGESTION:
	case CLOAK_OF_INFRAVISION:
	case CLOAK_OF_BANISHING_FEAR:
	case CLOAK_OF_MEMORY:
	case CLOAK_OF_THE_FORCE:
	case CLOAK_OF_SEEING:
	case CLOAK_OF_CURSE_CATCHING:
	case LION_CLOAK:
	case TIGER_CLOAK:
	case CLOAK_OF_PRACTICE:
	case CLOAK_OF_ELEMENTALISM:
	case PSIONIC_CLOAK:
	case SECOND_SKIN:
	case CLOAK_OF_FLIGHT:
	case CLOAK_OF_MAGIC_CONTROL:
	case CLOAK_OF_EXPERIENCE:
	case CLOAK_OF_MAP_AMNESIA:
	case CLOAK_OF_TRANSFORMATION:
	case CLOAK_OF_DISCOUNT_ACTION:
	case CLOAK_OF_TECHNICALITY:
	case CLOAK_OF_FULL_NUTRITION:

	case CLOAK_OF_AGGRAVATION:
	case CLOAK_OF_MAGICAL_BREATHING:
	case CLOAK_OF_STAT_LOCK:
	case WING_CLOAK:
	case CLOAK_OF_PREMATURE_DEATH:
	case ANTIDEATH_CLOAK:
		break;
	case CLOAK_OF_FUMBLING:
		if (!oldprop && !(HFumbling & ~TIMEOUT))
			incr_itimeout(&HFumbling, rnd(20));
		break;
	/* KMH, balance patch -- lab coat gives poison _and_ acid resistance */
	case LAB_COAT:
		EAcid_resistance |= WORN_CLOAK;
		break;
	case MUMMY_WRAPPING:
		/* Note: it's already being worn, so we have to cheat here. */
		if ((HInvis || EInvis || pm_invisible(youmonst.data)) && !Blind) {
		    newsym(u.ux,u.uy);
		    You("can %s!",
			See_invisible ? "no longer see through yourself"
			: see_yourself);
		}
		break;
	case CLOAK_OF_INVISIBILITY:
		/* since cloak of invisibility was worn, we know mummy wrapping
		   wasn't, so no need to check `oldprop' against blocked */
		if (!oldprop && !HInvis && !Blind) {
		    makeknown(uarmc->otyp);
		    newsym(u.ux,u.uy);
		    pline("Suddenly you can%s yourself.",
			See_invisible ? " see through" : "not see");
		}
		break;
	case POISONOUS_CLOAK:
		if (Poison_resistance && (StrongPoison_resistance || rn2(10)) )
			pline(FunnyHallu ? "Very tight, like a latex shirt!" : "This cloak feels a little itchy.");
		else {
		    makeknown(uarmc->otyp);
		    poisoned("cloak",A_STR,"poisonous cloak",3);
		}
		break;

	case CLOAK_OF_DEATH:

		pline("Oh no, this is a cloak of death!");
		    makeknown(uarmc->otyp);
		if (nonliving(youmonst.data) || is_demon(youmonst.data) || Death_resistance) {
		    You("seem no deader than before.");
		} else if ((!Antimagic || !rn2(StrongAntimagic ? 20 : 5)) && rn2(50) > 12) {
		    if (Hallucination) {
			You("have an out of body experience.");
		    } else if (!rnd(50) && !Antimagic) {
			u.youaredead = 1;
			killer_format = KILLED_BY_AN;
			killer = "cloak of death";
			done(DIED);
			u.youaredead = 0;
		    } else {
			u.uhpmax -= d(4, 4);
			if (u.uhpmax < 1) u.uhpmax = 1;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			You_feel("a loss of life.");
			losehp(d(4, 4),"cloak of death",KILLED_BY_AN);
		    }
		} else {
		    pline("Lucky for you, it didn't work!");
		}

		break;

	case CLOAK_OF_UNSPELLING:
	case ANTI_CASTER_CLOAK:
	case ADOM_CLOAK:
	case SPAWN_CLOAK:
	case EGOIST_CLOAK:
	case CLOAK_OF_TIME:
	case CHATBOX_CLOAK:
	case HERETIC_CLOAK:
	case EERIE_CLOAK:
	case CLOAK_OF_NAKEDNESS:

	case BROWN_SHIT_CLOAK:
	case ANTICALL_CLOAK:
	case YELLOW_WING:
	case ELESDE_CLOAK:
	case CLOAK_OF_GRAVATION:
	case CLOAK_OF_NONFOLLOWING:
	case SPELL_RETAIN_CLOAK:
	case CLOAK_OF_FAST_DECAY:

	case WHITE_SPELL_CLOAK:
	case GREYOUT_CLOAK:
	case TRUMP_COAT:
	case CLOAK_OF_SUDDEN_ATTACK:
	case CLOAK_OF_BAD_TRAPPING:
	case GRAYOUT_CLOAK:
	case PSEUDO_TELEPORTER_CLOAK:
	case CLOCKLOAK:
	case NOKEDEX_CLOAK:
	case NAYLIGHT_CLOAK:
	case BATTERY_CLOAK:
	case CLOAK_OF_WRONG_ANNOUNCEMENT:
	case STORMY_CLOAK:
	case GIMP_CLOAK:
	case SNARENET_CLOAK:
	case PINK_SPELL_CLOAK:
	case EVENCORE_CLOAK:
	case UNDERLAYER_CLOAK:
	case CYAN_SPELL_CLOAK:
	case ELONGATION_CLOAK:
	case UNFAIR_ATTACK_CLOAK:
	case CLOAK_OF_BAD_PART:

	case HEAVY_STATUS_CLOAK:
	case CLOAK_OF_LUCK_NEGATION:
	case YELLOW_SPELL_CLOAK:
	case VULNERABILITY_CLOAK:
	case CLOAK_OF_INVENTORYLESSNESS:
	case CLOAK_OF_RESPAWNING:
	case NASTY_CLOAK:
	case DUMMY_CLOAK_A:
	case DUMMY_CLOAK_B:
	case DUMMY_CLOAK_C:
	case DUMMY_CLOAK_D:
	case DUMMY_CLOAK_E:
	case DUMMY_CLOAK_F:
	case DUMMY_CLOAK_G:
	case DUMMY_CLOAK_H:
	case DUMMY_CLOAK_I:
	case DUMMY_CLOAK_J:
	case DUMMY_CLOAK_K:
	case DUMMY_CLOAK_L:
	case DUMMY_CLOAK_M:
	case DUMMY_CLOAK_N:
	case DUMMY_CLOAK_O:
	case DUMMY_CLOAK_P:
	case DUMMY_CLOAK_Q:
	case DUMMY_CLOAK_R:
	case DUMMY_CLOAK_S:
	case DUMMY_CLOAK_T:
	case DUMMY_CLOAK_U:
	case DUMMY_CLOAK_V:
	case DUMMY_CLOAK_W:
	case DUMMY_CLOAK_X:
	case DUMMY_CLOAK_Y:
	case DUMMY_CLOAK_Z:
	case DUMMY_CLOAK_AA:
	case DUMMY_CLOAK_AB:
	case DUMMY_CLOAK_AC:
	case DUMMY_CLOAK_AD:
	case DUMMY_CLOAK_AE:
	case DUMMY_CLOAK_AF:
	case DUMMY_CLOAK_AG:
	case DUMMY_CLOAK_AH:
	case DUMMY_CLOAK_AI:
	case DUMMY_CLOAK_AJ:
	case DUMMY_CLOAK_AK:
	case DUMMY_CLOAK_AL:
	case DUMMY_CLOAK_AM:
	case DUMMY_CLOAK_AN:
	case DUMMY_CLOAK_AO:
	case DUMMY_CLOAK_AP:
	case DUMMY_CLOAK_AQ:
	case DUMMY_CLOAK_AR:
	case DUMMY_CLOAK_AS:
	case DUMMY_CLOAK_AT:
	case DUMMY_CLOAK_AU:
	case DUMMY_CLOAK_AV:
	case DUMMY_CLOAK_AW:
	case DUMMY_CLOAK_AX:

		if (!uarmc->cursed) curse(uarmc);
		break;

	case CLOAK_OF_CONFLICT:

		if (!uarmc->cursed) {
			curse(uarmc);
			pline("Your cloak is surrounded by a black aura.");
		}
		break;

	case OILSKIN_CLOAK:
		pline("%s very tightly.", Tobjnam(uarmc, "fit"));
		break;

	case LETHE_CLOAK:
		forget(ALL_SPELLS|ALL_MAP);
		You_feel("dizzy!");
		break;
	case CLOAK_OF_POLYMORPH:
		{
			register struct obj *polycloak;
			if (uarmc) polycloak = poly_obj(uarmc, STRANGE_OBJECT, FALSE);
			if (polycloak && is_hazy(polycloak)) {
				stop_timer(UNPOLY_OBJ, (void *) polycloak);
				polycloak->oldtyp = STRANGE_OBJECT;
			}
			if (uarmc) (void) Cloak_off();
			You_feel("a little %s.", FunnyHallu ? "normal" : "strange");
			if (!Unchanging) polyself(FALSE);
			return 0;
			break;
		}
	case CLOAK_OF_WATER_SQUIRTING:
		pline("A gush of water squirts all over your body!");
		if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
			water_damage(invent, FALSE, FALSE);
		}
		break;
	case CLOAK_OF_PARALYSIS:
		pline("You're paralyzed!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
		if (isstunfish) {
			if (StrongFree_action) nomul(-rnz(5), "putting on a cloak of paralysis", TRUE);
			else if (Free_action) nomul(-rnz(10), "putting on a cloak of paralysis", TRUE);
			else nomul(-rnz(20), "putting on a cloak of paralysis", TRUE);

		} else {
			if (StrongFree_action) nomul(-rnd(5), "putting on a cloak of paralysis", TRUE);
			else if (Free_action) nomul(-rnd(10), "putting on a cloak of paralysis", TRUE);
			else nomul(-rnd(20), "putting on a cloak of paralysis", TRUE);
		}
		break;
	case CLOAK_OF_SICKNESS:
		if (IntSick_resistance || (ExtSick_resistance && rn2(20)) || !rn2(10) ) { /* small chance to not get infected even if not resistant --Amy */
			You_feel("a slight illness.");
		} else {
			make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
				"cloak of sickness", TRUE, SICK_NONVOMITABLE);
		}

		break;
	case CLOAK_OF_SLIMING:
		if (!Slimed && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) {
			You("don't feel very well.");
			make_slimed(100);
			killer_format = KILLED_BY_AN;
			delayed_killer = "slimed by a cloak of sliming";
		}
		break;
	case CLOAK_OF_STARVING:
		if (u.uhunger > 0) u.uhunger = -1;
		else u.uhunger -= rnd(StrongFull_nutrient ? 250 : Full_nutrient ? 500 : 1000);
		break;
	case CLOAK_OF_CURSE:
		attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse();
		break;
	case CLOAK_OF_DISENCHANTMENT:
		{
			struct obj *otmpE;
		      for (otmpE = invent; otmpE; otmpE = otmpE->nobj) {
				if (otmpE && !rn2(10)) (void) drain_item(otmpE);
			}
			pline("Your equipment seems less effective.");
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}
		break;
	case CLOAK_OF_OUTRIGHT_EVILNESS:
		{
			register struct obj *objX;

			for(objX = invent; objX ; objX = objX->nobj)
				if (!rn2(5) && !stack_too_big(objX))	curse(objX);
		}
		break;
	case CLOAK_OF_STONE:
		if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(poly_when_stoned(youmonst.data) &&
				 polymon(PM_STONE_GOLEM)) ) {
			pline("You start turning to stone!");
			if (Hallucination && rn2(10)) pline("But you are already stoned.");
			else {
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				u.cnd_stoningcount++;
				delayed_killer = "cloak of stone";
			}
		}
		break;
	case CLOAK_OF_LYCANTHROPY:
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREWOLF;
			You_feel("feverish.");
			u.cnd_lycanthropecount++;
		}
		break;
	case CLOAK_OF_UNLIGHT:
		if (Race_if(PM_PLAYER_NIBELUNG) && rn2(5)) break;
		pline("Darkness surrounds you.");
		litroomlite(FALSE);
		break;
	case CLOAK_OF_ESCALATION:
		u.chokhmahdamage += rnd(5);
		break;
	case CLOAK_OF_MAGICAL_DRAINAGE:
		drain_en(rnd(u.ulevel) + 1 + rnd(monster_difficulty() + 1));
		break;
	case CLOAK_OF_ANGRINESS:
		u.ugangr++;
	      You("get the feeling that %s is angry...", u_gname());
		break;
	case CLOAK_OF_CANCELLATION:
	      (void) cancel_monst(&youmonst, (struct obj *)0, FALSE, TRUE, FALSE);
		break;
	case CLOAK_OF_TURN_LOSS:
		if (!uarmc->hvycurse) {
			curse(uarmc);
			uarmc->hvycurse = 1;
			pline("Bad idea!");
		}
		if (moves < 10000000) { /* fail safe just to make sure you can't overflow the number --Amy */
			moves += 1000;
			monstermoves += 1000;
		}
		break;
	case CLOAK_OF_ATTRIBUTE_LOSS:
		adjattrib(A_STR, -1, FALSE, TRUE);
		adjattrib(A_DEX, -1, FALSE, TRUE);
		adjattrib(A_CON, -1, FALSE, TRUE);
		adjattrib(A_INT, -1, FALSE, TRUE);
		adjattrib(A_WIS, -1, FALSE, TRUE);
		adjattrib(A_CHA, -1, FALSE, TRUE);
		break;
	case CLOAK_OF_TOTTER:
		pline("You completely lose your sense of direction.");
		u.totter = 1;
		break;
	case CLOAK_OF_DRAIN_LIFE:
		if (rn2(25) && moves < 1000 && u.ulevel < 2) break; /* can still start the game dead or get a "poison pill" when wear-testing a random cloak at XL1 but it's greatly reduced --Amy */
		losexp("a cloak of drain life", FALSE, TRUE);
		break;

	/* Alchemy smock gives poison _and_ acid resistance */
#if 0
	case ALCHEMY_SMOCK:
		EAcid_resistance |= WORN_CLOAK;
  		break;
#endif
	default: impossible(unknown_type_long, c_cloak, uarmc->otyp);
    }

	if (uarmc && itemhasappearance(uarmc, APP_STRAITJACKET_CLOAK) ) {
		if (!uarmc->hvycurse) {
			curse(uarmc);
			uarmc->hvycurse = 1;
			pline("An aura of evil darkness surrounds your cloak as you put it on!");
		}
	}

	if (uarmc && itemhasappearance(uarmc, APP_IGNORANT_CLOAK) ) {
		if (!uarmc->cursed) {
			curse(uarmc);
		}
		You_feel("ignorant.");
	}

	if ( (Role_if(PM_GEEK) || Role_if(PM_GRADUATE) || Role_if(PM_CRACKER) || Role_if(PM_SOFTWARE_ENGINEER)) && uarmc && itemhasappearance(uarmc, APP_GEEK_CLOAK) ) {
		int i;
		for (i = 0; i < MAXSPELL; i++)  {
			if (spellid(i) == SPE_ALTER_REALITY) break;
			else if (spellid(i) == NO_SPELL) {
				spl_book[i].sp_id = SPE_ALTER_REALITY;
				spl_book[i].sp_lev = objects[SPE_ALTER_REALITY].oc_level;
				spl_book[i].sp_memorize = TRUE;
				incrnknow(i);
				pline("You gain the power of Eru Illuvator!");

				if (u.emynluincomplete) boostknow(i, 1000);

				if (!PlayerCannotUseSkills && P_SKILL(P_MEMORIZATION) >= P_BASIC) {

					char nervbuf[QBUFSZ];
					char thisisannoying = 0;

					sprintf(nervbuf, "Do you want to use the memorization skill to get even more power?");
					thisisannoying = yn_function(nervbuf, ynqchars, 'y');
					if (thisisannoying != 'n') {

						int memoboost = 0;
						switch (P_SKILL(P_MEMORIZATION)) {
							case P_BASIC: memoboost = 2; break;
							case P_SKILLED: memoboost = 4; break;
							case P_EXPERT: memoboost = 6; break;
							case P_MASTER: memoboost = 8; break;
							case P_GRAND_MASTER: memoboost = 10; break;
							case P_SUPREME_MASTER: memoboost = 12; break;
						}
					    	boostknow(i, memoboost * 1000);
						spl_book[i].sp_memorize = TRUE;
					} else spl_book[i].sp_memorize = FALSE;

				}

				break;
			}
		}
	}

	if (uarmc && !(uarmc->cursed) && uarmc->oartifact == ART_INA_S_LAB_COAT) {
		pline("Bad for you - you just cursed yourself with Ina's anorexia. :-(");
		curse(uarmc);
	}

	if (uarmc && !(uarmc->cursed) && uarmc->oartifact == ART_INA_S_OVERCOAT) {
		pline("Bad for you - you just cursed yourself with Ina's anorexia. :-(");
		curse(uarmc);
	}

	if (uarmc && !(uarmc->cursed) && uarmc->oartifact == ART_BROKEN_WINGS) {
		pline("A terrible black glow surrounds your nonfunctional wings and roots you to the ground.");
		curse(uarmc);
	}

	if (uarmc && !(uarmc->cursed) && uarmc->oartifact == ART_GROUNDBUMMER) {
		pline("A terrible black glow surrounds your cloak and roots you to the ground.");
		curse(uarmc);
	}

	if (uarmc && !(uarmc->cursed) && uarmc->oartifact == ART_ARABELLA_S_SEXY_GIRL_BUTT) {
		curse(uarmc);
	}

	if (uarmc && uarmc->oartifact == ART_FILTHY_MORTALS_WILL_DIE) {
		u.negativeprotection += 10;
		if (evilfriday && u.ublessed > 0) {
			u.ublessed -= 10;
			if (u.ublessed < 0) u.ublessed = 0;
		}
		pline("You become more vulnerable, and realize that putting this cloak on was a very bad idea.");
	}

	if (uarmc && uarmc->oartifact == ART_RNG_S_EMBRACE && uarmc->spe == 0) {
		if (!rn2(2)) uarmc->spe = rnd(5);
		else uarmc->spe = -(rnd(5));
	}

	if (uarmc && uarmc->oartifact == ART_RITA_S_DECEPTIVE_MANTLE) {
		if (!uarmc->cursed) curse(uarmc);
		if (uarmc->spe > -10) uarmc->spe = -10;
		/* No message. Quite the deceptive little girl, Rita giggles while her victims happily run around not knowing
		 * that the item they just equipped became cursed -10. And she especially wants to see your face when you
		 * open your inventory 1000 turns later and discover what happened! :-P --Amy
		 * To clarify: Rita does not actually WEAR this thing.	
		 * She made it to lure in unsuspecting victims of course! */
	}

	if (uarmc && uarmc->oartifact == ART_RITA_S_LOVELY_OVERGARMENT) {
		if (!uarmc->cursed) curse(uarmc);
		if (uarmc->spe > -10) uarmc->spe = -10;
		/* No message. Quite the deceptive little girl, Rita giggles while her victims happily run around not knowing
		 * that the item they just equipped became cursed -10. And she especially wants to see your face when you
		 * open your inventory 1000 turns later and discover what happened! :-P --Amy
		 * To clarify: Rita does not actually WEAR this thing.	
		 * She made it to lure in unsuspecting victims of course! */
	}

	if (uarmc && uarmc->oartifact == ART_ARABELLA_S_LIGHTNINGROD) {
		if (!uarmc->cursed) curse(uarmc);
		uarmc->hvycurse = uarmc->prmcurse = 1;
		/* Arabella's bait items never give any message. --Amy */
	}

	if (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) {
		if (!uarmc->cursed) curse(uarmc);
		uarmc->hvycurse = 1;
	}

    if (uarmc && uarmc->oartifact == ART_JEDI_MIND_POWER) {
		if (!tech_known(T_TELEKINESIS)) {
			if (yn_function("This cloak allows you to learn telekinesis, but it comes at a price. Do it?", ynchars, 'n') == 'y') {
			    	learntech(T_TELEKINESIS, FROMOUTSIDE, 1);
			    	pline("Suddenly, you know how to use the telekinesis technique!");

				int disabling = 3;
				while (disabling > 0) {
					disabling--;
					deacrandomintrinsic(1000000);

				}

			}
		}
    }


	if (uarmc && uarmc->oartifact == ART_HAHAHA_HA_HAHAHAHAHA) {
		RespawnProblem |= FROMOUTSIDE;
		pline("Hahaha hahahahaha, hahahahaha hahahahahahaha, HEHEHEHEHEHEHEE-HEE!!!"); /* Bwarharharharhar! */
	}

	if (uarmc && uarmc->oartifact == ART_LEA_S_SCHOOL_UNIFORM) {
		if (!uarmc->cursed) curse(uarmc);
		if (!uarmc->hvycurse) uarmc->hvycurse = 1;
		/* For Lea's cloaks, no message is given either; see above. */
	}

	if (uarmc && uarmc->oartifact == ART_LEA_S_SPOKESWOMAN_UNIFORM) {
		if (!uarmc->cursed) curse(uarmc);
		if (!uarmc->hvycurse) uarmc->hvycurse = 1;
	}

	if (uarmc && uarmc->oartifact == ART_ARABELLA_S_WAND_BOOSTER) {
		if (!uarmc->cursed) curse(uarmc);
		if (!uarmc->hvycurse) uarmc->hvycurse = 1;
	}

	if (uarmc && uarmc->oartifact == ART_VEIL_OF_LATONA) {
		if (!uarmc->hvycurse) {
			curse(uarmc);
			uarmc->hvycurse = 1;
			pline("An evil curse goes off as you put on the veil.");
		}
	}

	if (uarmc && uarmc->oartifact == ART_CONNY_S_COMBAT_COAT) {
		if (!uarmc->hvycurse) {
			curse(uarmc);
			uarmc->hvycurse = 1;
			pline("Your cloak is heavily cursed, and you hear Conny's voice wishing you good luck.");
		}
	}

	if (uarmc && uarmc->oartifact == ART_VEIL_OF_MINISTRY) {
		if (!uarmc->hvycurse) {
			curse(uarmc);
			uarmc->hvycurse = 1;
			pline("An evil curse goes off as you put on the veil.");
		}
	}

	if (uarmc && (AutocursingEquipment || u.uprops[AUTOCURSE_EQUIP].extrinsic || have_autocursestone())) curse(uarmc);

	if (uarmc && uarmc->spe > -10 && (TrashingBugEffect || u.uprops[TRASHING_EFFECT].extrinsic || have_trashstone())) uarmc->spe--;

    return 0;
}

int
Cloak_off()
{
    int otyp = uarmc->otyp;
    long oldprop = u.uprops[objects[otyp].oc_oprop].extrinsic & ~WORN_CLOAK;

    takeoff_mask &= ~W_ARMC;
	/* For mummy wrapping, taking it off first resets `Invisible'. */
    setworn((struct obj *)0, W_ARMC);
    switch (otyp) {
	case ELVEN_CLOAK:
	case ORCISH_CLOAK:
	case DROVEN_CLOAK:
	case DWARVISH_CLOAK:
	case CLOAK_OF_PROTECTION:
	case CLOAK_OF_MAGIC_RESISTANCE:
	case CLOAK_OF_REFLECTION:
	case CLOAK_OF_DISPLACEMENT:
	case CLOAK_OF_CONFUSION:
	case MANACLOAK:
	case CLOAK_OF_INVERSION:
	case CLOAK_OF_WINCING:
	case TROLL_HIDE:
	case POISONOUS_CLOAK:
	case CLOAK_OF_DEATH:
	case OILSKIN_CLOAK:
	case ROBE:
	case ALCHEMY_SMOCK:
	case LEO_NEMAEUS_HIDE:
	case PLASTEEL_CLOAK:
	case LORICATED_CLOAK:
	case LEATHER_CLOAK:
	case CLOAK_OF_WARMTH:
	case CLOAK_OF_GROUNDING:
	case CLOAK_OF_QUENCHING:
	case CLOAK_OF_LEECH:
	case CLOAK_OF_COAGULATION:
	case CLOAK_OF_SCENT:
	case FILLER_CLOAK:

	case LETHE_CLOAK:
	case CLOAK_OF_MAP_AMNESIA:
	case CLOAK_OF_POLYMORPH:
	case CLOAK_OF_TRANSFORMATION:
	case CLOAK_OF_WATER_SQUIRTING:
	case CLOAK_OF_PARALYSIS:
	case CLOAK_OF_SICKNESS:
	case CLOAK_OF_SLIMING:
	case CLOAK_OF_STARVING:
	case CLOAK_OF_CURSE:
	case CLOAK_OF_DISENCHANTMENT:
	case CLOAK_OF_OUTRIGHT_EVILNESS:
	case CLOAK_OF_STONE:
	case CLOAK_OF_LYCANTHROPY:
	case CLOAK_OF_UNLIGHT:
	case CLOAK_OF_ESCALATION:
	case CLOAK_OF_MAGICAL_DRAINAGE:
	case CLOAK_OF_ANGRINESS:
	case CLOAK_OF_CANCELLATION:
	case CLOAK_OF_TURN_LOSS:
	case CLOAK_OF_ATTRIBUTE_LOSS:
	case CLOAK_OF_TOTTER:
	case CLOAK_OF_DRAIN_LIFE:
	case CLOAK_OF_AWAKENING:
	case CLOAK_OF_STABILITY:
	case DUMMY_CLOAK_A:
	case DUMMY_CLOAK_B:
	case DUMMY_CLOAK_C:
	case DUMMY_CLOAK_D:
	case DUMMY_CLOAK_E:
	case DUMMY_CLOAK_F:
	case DUMMY_CLOAK_G:
	case DUMMY_CLOAK_H:
	case DUMMY_CLOAK_I:
	case DUMMY_CLOAK_J:
	case DUMMY_CLOAK_K:
	case DUMMY_CLOAK_L:
	case DUMMY_CLOAK_M:
	case DUMMY_CLOAK_N:
	case DUMMY_CLOAK_O:
	case DUMMY_CLOAK_P:
	case DUMMY_CLOAK_Q:
	case DUMMY_CLOAK_R:
	case DUMMY_CLOAK_S:
	case DUMMY_CLOAK_T:
	case DUMMY_CLOAK_U:
	case DUMMY_CLOAK_V:
	case DUMMY_CLOAK_W:
	case DUMMY_CLOAK_X:
	case DUMMY_CLOAK_Y:
	case DUMMY_CLOAK_Z:
	case DUMMY_CLOAK_AA:
	case DUMMY_CLOAK_AB:
	case DUMMY_CLOAK_AC:
	case DUMMY_CLOAK_AD:
	case DUMMY_CLOAK_AE:
	case DUMMY_CLOAK_AF:
	case DUMMY_CLOAK_AG:
	case DUMMY_CLOAK_AH:
	case DUMMY_CLOAK_AI:
	case DUMMY_CLOAK_AJ:
	case DUMMY_CLOAK_AK:
	case DUMMY_CLOAK_AL:
	case DUMMY_CLOAK_AM:
	case DUMMY_CLOAK_AN:
	case DUMMY_CLOAK_AO:
	case DUMMY_CLOAK_AP:
	case DUMMY_CLOAK_AQ:
	case DUMMY_CLOAK_AR:
	case DUMMY_CLOAK_AS:
	case DUMMY_CLOAK_AT:
	case DUMMY_CLOAK_AU:
	case DUMMY_CLOAK_AV:
	case DUMMY_CLOAK_AW:
	case DUMMY_CLOAK_AX:
	case ANTI_DISQUIET_CLOAK:
	case HUGGING_GOWN:
	case COCLOAK:
	case CLOAK_OF_HEALTH:
	case CLOAK_OF_DISCOVERY:
	case CLOAK_OF_BLINDNESS_RESISTANCE:
	case CLOAK_OF_HALLUCINATION_RESISTA:
	case BIONIC_CLOAK:
	case CLOAK_OF_PORTATION:
	case CLOAK_OF_CONTROL:
	case CLOAK_OF_SHIFTING:
	case FLOATCLOAK:
	case CLOAK_OF_PRESCIENCE:
	case SENSOR_CLOAK:
	case CLOAK_OF_SPEED:
	case VAULT_CLOAK:
	case CLOAK_OF_SPELL_RESISTANCE:
	case CLOAK_OF_PHYSICAL_RESISTANCE:
	case OPERATION_CLOAK:
	case BESTEST_CLOAK:
	case CLOAK_OF_FREEDOM:
	case BIKINI:
	case CLOAK_OF_PERMANENCE:
	case CLOAK_OF_SLOW_DIGESTION:
	case CLOAK_OF_INFRAVISION:
	case CLOAK_OF_BANISHING_FEAR:
	case CLOAK_OF_MEMORY:
	case CLOAK_OF_THE_FORCE:
	case CLOAK_OF_SEEING:
	case CLOAK_OF_CURSE_CATCHING:
	case LION_CLOAK:
	case TIGER_CLOAK:
	case CLOAK_OF_PRACTICE:
	case CLOAK_OF_ELEMENTALISM:
	case PSIONIC_CLOAK:
	case SECOND_SKIN:
	case CLOAK_OF_FLIGHT:
	case CLOAK_OF_DISCOUNT_ACTION:
	case CLOAK_OF_TECHNICALITY:
	case CLOAK_OF_FULL_NUTRITION:

	case CLOAK_OF_AGGRAVATION:
	case CLOAK_OF_CONFLICT:
	case CLOAK_OF_MAGICAL_BREATHING:
	case CLOAK_OF_STAT_LOCK:
	case WING_CLOAK:
	case CLOAK_OF_PREMATURE_DEATH:
	case ANTIDEATH_CLOAK:
	case SPECIAL_CLOAK:
	case PLAIN_CLOAK:
	case ARCHAIC_CLOAK:

	case CLOAK_OF_UNSPELLING:
	case ANTI_CASTER_CLOAK:
	case HEAVY_STATUS_CLOAK:
	case CLOAK_OF_LUCK_NEGATION:
	case YELLOW_SPELL_CLOAK:
	case VULNERABILITY_CLOAK:
	case CLOAK_OF_INVENTORYLESSNESS:
	case CLOAK_OF_RESPAWNING:
	case ADOM_CLOAK:
	case SPAWN_CLOAK:
	case EGOIST_CLOAK:
	case CLOAK_OF_TIME:
	case CHATBOX_CLOAK:
	case HERETIC_CLOAK:
	case EERIE_CLOAK:
	case CLOAK_OF_NAKEDNESS:

	case BROWN_SHIT_CLOAK:
	case ANTICALL_CLOAK:
	case YELLOW_WING:
	case ELESDE_CLOAK:
	case CLOAK_OF_GRAVATION:
	case CLOAK_OF_NONFOLLOWING:
	case SPELL_RETAIN_CLOAK:
	case CLOAK_OF_FAST_DECAY:

	case WHITE_SPELL_CLOAK:
	case GREYOUT_CLOAK:
	case TRUMP_COAT:
	case CLOAK_OF_SUDDEN_ATTACK:
	case CLOAK_OF_BAD_TRAPPING:
	case GRAYOUT_CLOAK:
	case PSEUDO_TELEPORTER_CLOAK:
	case CLOCKLOAK:
	case NOKEDEX_CLOAK:
	case NAYLIGHT_CLOAK:
	case BATTERY_CLOAK:
	case CLOAK_OF_WRONG_ANNOUNCEMENT:
	case STORMY_CLOAK:
	case GIMP_CLOAK:
	case SNARENET_CLOAK:
	case PINK_SPELL_CLOAK:
	case EVENCORE_CLOAK:
	case UNDERLAYER_CLOAK:
	case CYAN_SPELL_CLOAK:
	case ELONGATION_CLOAK:
	case UNFAIR_ATTACK_CLOAK:
	case CLOAK_OF_BAD_PART:

	/* KMH, balance patch -- removed */
	/* but re-inserted by Amy */
	case CLOAK_OF_DRAIN_RESISTANCE:
	case MISSING_CLOAK:
	case AYANAMI_WRAPPING:
	case RUBBER_APRON:
	case KITCHEN_APRON:
	case FRILLED_APRON:
	case SUPER_MANTLE:
	case WINGS_OF_ANGEL:
	case DUMMY_WINGS:
	case FUR:
	case CLOAK_OF_MAGIC_CONTROL:
	case CLOAK_OF_EXPERIENCE:
	case HIDE:
	case CLOAK_OF_PEACE:
	case CLOAK_OF_DIMNESS:
	case CLOAK_OF_CONTAMINATION_RESISTA:
	case DISPLACER_BEAST_HIDE:
	case THE_NEMEAN_LION_HIDE:
	case CLOAK_OF_SPRAY:
	case CLOAK_OF_FLAME:
	case CLOAK_OF_INSULATION:
	case CLOAK_OF_MATADOR:
	case CLOAK_OF_PAIN_SENSE:
	case NASTY_CLOAK:
		break;
	case CLOAK_OF_FUMBLING:
	    if (!oldprop && !u.fumbleduration && !(HFumbling & ~TIMEOUT))
		HFumbling = EFumbling = 0;
	    break;
	/* KMH, balance patch -- lab coat gives poison _and_ acid resistance */
	case LAB_COAT:
		EAcid_resistance &= ~WORN_CLOAK;
		break;
	case MUMMY_WRAPPING:
		if (Invis && !Blind) {
		    newsym(u.ux,u.uy);
		    You("can %s.",
			See_invisible ? "see through yourself"
			: "no longer see yourself");
		}
		break;
	case CLOAK_OF_INVISIBILITY:
		if (!oldprop && !HInvis && !Blind) {
		    makeknown(CLOAK_OF_INVISIBILITY);
		    newsym(u.ux,u.uy);
		    pline("Suddenly you can %s.",
			See_invisible ? "no longer see through yourself"
			: see_yourself);
		}
		break;
#if 0
	/* Alchemy smock gives poison _and_ acid resistance */
	case ALCHEMY_SMOCK:
		EAcid_resistance &= ~WORN_CLOAK;
  		break;
#endif
	default: impossible(unknown_type, c_cloak, otyp);
    }
    return 0;
}

int
Helmet_on()
{
    if (!uarmh) return 0; 

	if (uarmf && uarmf->otyp == BOOTS_OF_SHOCK_RESISTANCE && uarmh && is_metallic(uarmh)) {
		You("receive a little shock.");
		losehp(rnd(2), "little electric shock", KILLED_BY_AN);
	}

    switch(uarmh->otyp) {
	case FEDORA:
		set_moreluck();
		flags.botl = 1;
		break;
	case PLASTEEL_HELM:
	case HELMET:

	case NURSE_CAP:
	case KATYUSHA:
	case BUNNY_EAR:
	case DRAGON_HORNED_HEADPIECE:
	case STRAW_HAT:
	case SPEEDWAGON_S_HAT:
	case MECHA_IRAZU:
	case SCHOOL_CAP:
	case CROWN:
	case ANTENNA:
	case CHAIN_COIF:
	case COLOR_CONE:
	case FIELD_HELM:
	case HELM_OF_SAFEGUARD:
	case HELM_OF_UNDERWATER_ACTION:
	case HELM_OF_JAMMING:
	case HELM_OF_DECONTAMINATION:
	case WINCING_GRIMACE:
	case HELM_VERSUS_DEATH:
	case HELM_OF_BEGINNER_S_LUCK:
	case HELM_OF_FULL_NUTRITION:

	case HELM_OF_STEEL:
	case SEDGE_HAT:
	case SKULLCAP:
	case RANDOMIZED_HELMET:
	case CRYSTAL_HELM:
	case DENTED_POT:
	case BASINET:
	case ELVEN_LEATHER_HELM:
	case ELVEN_HELM:
	case WAR_HAT:
	case HIGH_ELVEN_HELM:
	case GNOMISH_HELM:
	case DWARVISH_IRON_HELM:
	case DROVEN_HELM:
	case ORCISH_HELM:
	/* KMH, balance patch -- removed */ /* but re-inserted by Amy */
	case FIRE_HELMET:
	case HELM_OF_SPEED:
	case SKILL_CAP:
	case HELM_OF_TELEPORTATION:
	case HELM_OF_TELEPORT_CONTROL:
	case HELM_OF_OPAQUE_THOUGHTS:
	case SCENTY_HELMET:
	case OILSKIN_COIF:
	case HELM_OF_ANTI_MAGIC:
	case HELMET_OF_UNDEAD_WARNING:
	case HELM_OF_TELEPATHY:
	case HELM_OF_DISCOVERY:
	case HELM_OF_DRAIN_RESISTANCE:
	case HELM_OF_FEAR:
	case BOOGEYMAN_HELMET:
	case HELM_OF_HUNGER:
	case HELM_OF_TRUE_SIGHT:
	case HELM_OF_WARNING:
	case HELM_OF_DETOXIFICATION:
	case TINFOIL_HELMET:
	case PARANOIA_HELMET:
	case WONDER_HELMET:
	case POINTED_HELMET:
	case BOG_STANDARD_HELMET:
		break;

	case BOBBLE_HAT:
		You_feel("%s.", (ACURR(A_INT) <= 6) ? "like sitting in a corner" : "giddy");
		curse(uarmh);
		break;

	case HELM_OF_NO_DIGESTION:
		if (!uarmh->prmcurse) {
			pline("%s, and is blasted by a terrible black aura!", Tobjnam(uarmh, "vibrate"));
		    curse(uarmh);
			uarmh->hvycurse = uarmh->prmcurse = 1;
		}
		break;
	case FLACK_HELMET:
	case MINING_HELM:
		vision_full_recalc = 1;
		break;
	case HELM_OF_SENSORY_DEPRIVATION:
		if (!uarmh->cursed) {
		    if (Blind)
			pline("%s for a moment.", Tobjnam(uarmh, "vibrate"));
		    else
			pline("%s %s for a moment.",
			      Tobjnam(uarmh, "glow"), hcolor(NH_BLACK));
		    curse(uarmh);
		}
		break;
	case HELM_OF_BRILLIANCE:
		makeknown(uarmh->otyp);
		break;
	case CORNUTHAUM:
		/* people think marked wizards know what they're talking
		 * about, but it takes trained arrogance to pull it off,
		 * and the actual enchantment of the hat is irrelevant.
		 */
		ABON(A_CHA) += (Role_if(PM_WIZARD) ? 1 : -1);
		flags.botl = 1;
		makeknown(uarmh->otyp);
		break;
	case HELM_OF_OPPOSITE_ALIGNMENT:
	case HELM_OF_LAWFUL:
	case HELM_OF_NEUTRAL:
	case HELM_OF_CHAOTIC:

		if (Race_if(PM_CHIQUAI) && rn2(5)) { /* do nothing, not even autocurse */
			break;
		}
		if (Race_if(PM_MACTHEIST) && rn2(4)) {
			break;
		}
		if (Race_if(PM_GERTEUT) && rn2(4)) {
			break;
		}

		if (uarmh->otyp == HELM_OF_OPPOSITE_ALIGNMENT) {
			if (u.ualign.type == A_NEUTRAL)
			    u.ualign.type = rn2(2) ? A_CHAOTIC : A_LAWFUL;
			else u.ualign.type = -(u.ualign.type);
			u.ublessed = 0; /* lose your god's protection */
		} else if (uarmh->otyp == HELM_OF_LAWFUL) {
			if (u.ualign.type != A_LAWFUL) {
				u.ualign.type = A_LAWFUL;
				u.ublessed = 0;
			}
		} else if (uarmh->otyp == HELM_OF_NEUTRAL) {
			if (u.ualign.type != A_NEUTRAL) {
				u.ualign.type = A_NEUTRAL;
				u.ublessed = 0;
			}
		} else if (uarmh->otyp == HELM_OF_CHAOTIC) {
			if (u.ualign.type != A_CHAOTIC) {
				u.ualign.type = A_CHAOTIC;
				u.ublessed = 0;
			}
		}
	     /* makeknown(uarmh->otyp);   -- moved below, after xname() */
		/*FALLTHRU*/
	case DUNCE_CAP:
		if (!uarmh->cursed) {
		    if (Blind)
			pline("%s for a moment.", Tobjnam(uarmh, "vibrate"));
		    else
			pline("%s %s for a moment.",
			      Tobjnam(uarmh, "glow"), hcolor(NH_BLACK));
		    curse(uarmh);
		}
		flags.botl = 1;		/* reveal new alignment or INT & WIS */
		if (Hallucination) {
		    pline("My brain hurts!"); /* Monty Python's Flying Circus */
		} else if (uarmh->otyp == DUNCE_CAP) {
		    You_feel("%s.",	/* track INT change; ignore WIS */
		  ACURR(A_INT) <= (ABASE(A_INT) + ABON(A_INT) + ATEMP(A_INT)) ?
			     "like sitting in a corner" : "giddy");
		} else {
		    Your("mind oscillates briefly.");
		    /*makeknown(HELM_OF_OPPOSITE_ALIGNMENT);*/ /* taken out since there's several of them now --Amy */
		}
		break;
	case HELM_OF_STORMS:
	case HELM_OF_DETECT_MONSTERS:
		if (!uarmh->cursed) {
		    if (Blind)
			pline("%s for a moment.", Tobjnam(uarmh, "vibrate"));
		    else
			pline("%s %s for a moment.",
			      Tobjnam(uarmh, "glow"), hcolor(NH_BLACK));
		    curse(uarmh);
		}
		break;

	case HELM_OF_OBSCURED_DISPLAY:
	case HELM_OF_LOSE_IDENTIFICATION:
	case HELM_OF_THIRST:
	case HELM_OF_AMNESIA:
	case BLACKY_HELMET:
	case ANTI_DRINKER_HELMET:
	case WHISPERING_HELMET:
	case CYPHER_HELM:
	case ANGER_HELM:
	case BIGSCRIPT_HELM:
	case QUIZ_HELM:
	case DIZZY_HELMET:
	case RARE_HELMET:
	case MUTING_HELM:
	case ULCH_HELMET:
	case DIMMER_HELMET:
	case HELM_OF_STARVATION:
	case QUAFFER_HELMET:
	case SOUND_EFFECT_HELMET:
	case INCORRECTLY_ADJUSTED_HELMET:
	case CAPTCHA_HELM:
	case HELM_OF_BAD_ALIGNMENT:
	case SOUNDPROOF_HELMET:
	case OUT_OF_MEMORY_HELMET:
	case UNWANTED_HELMET:

	case QUAVERSAL_HELMET:
	case HELM_OF_SHUFFLING:
	case GOLDSPELL_HELMET:
	case AIRHEAD_CAP:
	case FALLOUT_HELMET:
	case IDENTIFY_CURD_HELMET:
	case BAEAEAEP_SPY_HELMET:
	case CRUNCHER_HELMET:
	case DISTORTED_GRIMACE:
	case ELM_ET:
	case SANEMAKER_HELMET:

	case MOMHAT:
	case CARTRIDGE_OF_HAVING_A_HORROR:
	case BORDERLESS_HELMET:
	case HELMET_OF_ANTI_SEARCHING:
	case HELM_OF_COUNTER_ROTATION:
	case DELIGHT_HELMET:
	case MESSAGE_FILTER_HELMET:
	case FLICKER_VISOR:
	case SCRIPTED_HELMET:
	case EMPTY_LINE_HELMET:
	case GREEN_SPELL_HELMET:
	case INFOLESS_HELMET:
	case BLUE_SPELL_HELMET:
	case MORE_HELMET:
	case BUGXPLORE_HELMET:
	case YAWNING_VISOR:
	
	case DUMMY_HELMET_A:
	case DUMMY_HELMET_B:
	case DUMMY_HELMET_C:
	case DUMMY_HELMET_D:
	case DUMMY_HELMET_E:
	case DUMMY_HELMET_F:
	case DUMMY_HELMET_G:
	case DUMMY_HELMET_H:
	case DUMMY_HELMET_I:
	case DUMMY_HELMET_J:
	case DUMMY_HELMET_K:
	case DUMMY_HELMET_L:
	case DUMMY_HELMET_M:
	case DUMMY_HELMET_N:
	case DUMMY_HELMET_O:
	case DUMMY_HELMET_P:
	case DUMMY_HELMET_Q:
	case DUMMY_HELMET_R:
	case DUMMY_HELMET_S:
	case DUMMY_HELMET_T:
	case DUMMY_HELMET_U:
	case DUMMY_HELMET_V:
	case DUMMY_HELMET_W:
	case DUMMY_HELMET_X:
	case DUMMY_HELMET_Y:
	case DUMMY_HELMET_Z:
	case DUMMY_HELMET_AA:
	case DUMMY_HELMET_AB:
	case DUMMY_HELMET_AC:
	case DUMMY_HELMET_AD:
	case DUMMY_HELMET_AE:
	case DUMMY_HELMET_AF:
	case DUMMY_HELMET_AG:
		if (!uarmh->cursed) curse(uarmh);
		break;
	default: impossible(unknown_type_long, c_helmet, uarmh->otyp);
    }

    if (uarmh && uarmh->oartifact == ART_NYPHERISBANE) {
		if (!uarmh->cursed) {
			curse(uarmh);
			pline("Whoops, the gas mask curses itself.");
		}
    }

    if (uarmh && uarmh->oartifact == ART_DARK_NADIR) {
		if (!uarmh->cursed) {
		    if (Blind)
			pline("%s for a moment.", Tobjnam(uarmh, "vibrate"));
		    else
			pline("%s %s for a moment.",
			      Tobjnam(uarmh, "glow"), hcolor(NH_BLACK));
			curse(uarmh);
		}
		if (!(Race_if(PM_PLAYER_NIBELUNG) && rn2(5)) ) litroomlite(FALSE);
    }

    if (uarmh && uarmh->oartifact == ART_RUTH_S_DARK_FORCE) {
		if (!uarmh->cursed) {
		    if (Blind)
			pline("%s for a moment.", Tobjnam(uarmh, "vibrate"));
		    else
			pline("%s %s for a moment.",
			      Tobjnam(uarmh, "glow"), hcolor(NH_BLACK));
			curse(uarmh);
		}
		if (!(Race_if(PM_PLAYER_NIBELUNG) && rn2(5)) ) litroomlite(FALSE);
    }

    if (uarmh && uarmh->oartifact == ART_NADJA_S_DARKNESS_GENERATOR) {
		if (!uarmh->cursed) {
		    if (Blind)
			pline("%s for a moment.", Tobjnam(uarmh, "vibrate"));
		    else
			pline("%s %s for a moment.",
			      Tobjnam(uarmh, "glow"), hcolor(NH_BLACK));
			curse(uarmh);
		}
		if (!(Race_if(PM_PLAYER_NIBELUNG) && rn2(5)) ) litroomlite(FALSE);
    }

    if (uarmh && uarmh->oartifact == ART_FORMFILLER) {
		if (!uarmh->cursed) curse(uarmh);
		uarmh->hvycurse = 1;
		/* base item is a "nasty" item, and by definition those give no messages upon cursing themselves --Amy */
    }

    if (uarmh && uarmh->oartifact == ART_WOLF_KING) {
		if (!uarmh->cursed) curse(uarmh);
		uarmh->hvycurse = 1;
		pline("You howl at the moon like a wolf while your helmet is surrounded by a terrible black aura.");
    }

    if (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) {
		if (!uarmh->cursed) curse(uarmh);
		uarmh->hvycurse = 1;
		uarmh->prmcurse = 1;
		pline("The Crown prime curses itself as you put it on. You knew that would happen, right?");
    }

    if (uarmh && uarmh->oartifact == ART_RNG_S_GRIMACE && uarmh->spe == 0) {
		if (!rn2(2)) uarmh->spe = rnd(5);
		else uarmh->spe = -(rnd(5));
    }

	if (uarmh && uarmh->oartifact == ART_SOARUP && uarmh->spe == 0) {
		uarmh->spe = 1;
	}

	if (uarmh && itemhasappearance(uarmh, APP_LESS_HELMET)) {
		if (uarmh->spe > 0) uarmh->spe--;
	}

    if (uarmh && uarmh->oartifact == ART_DEEP_INSANITY) {
		if (!uarmh->cursed) curse(uarmh); /* don't tell the poor sap about their horrible fate */
    }

    if (uarmh && uarmh->oartifact == ART_FLAT_INSANITY) {
		if (!uarmh->cursed) curse(uarmh); /* don't tell the poor sap about their horrible fate */
    }

	if (uarmh && (AutocursingEquipment || u.uprops[AUTOCURSE_EQUIP].extrinsic || have_autocursestone())) curse(uarmh);

	if (uarmh && uarmh->spe > -10 && (TrashingBugEffect || u.uprops[TRASHING_EFFECT].extrinsic || have_trashstone())) uarmh->spe--;

    return 0;
}

int
Helmet_off()
{
    if (uarmh && uarmh->oartifact == ART_JAMILA_S_BELIEF) {
		u.ualign.sins++;
		u.alignlim--;
		adjalign(-200);
		pline("Taking off your headgear is a terribly bad idea, and the gods will certainly not be pleased!");
		if (HardcoreAlienMode) {
			u.ualign.sins += 4;
			u.alignlim -= 4;
			adjalign(-1000);
		}
    }

    takeoff_mask &= ~W_ARMH;

    switch(uarmh->otyp) {
	case FEDORA:
	    setworn((struct obj *)0, W_ARMH);
	    set_moreluck();
	    flags.botl = 1;
	    return 0;
	case PLASTEEL_HELM:
	case HELMET:

	case NURSE_CAP:
	case KATYUSHA:
	case BUNNY_EAR:
	case DRAGON_HORNED_HEADPIECE:
	case STRAW_HAT:
	case SPEEDWAGON_S_HAT:
	case MECHA_IRAZU:
	case SCHOOL_CAP:
	case CROWN:
	case ANTENNA:
	case CHAIN_COIF:
	case COLOR_CONE:
	case FIELD_HELM:
	case HELM_OF_SAFEGUARD:
	case HELM_OF_UNDERWATER_ACTION:
	case HELM_OF_JAMMING:
	case WONDER_HELMET:
	case POINTED_HELMET:
	case BOG_STANDARD_HELMET:
	case HELM_OF_DECONTAMINATION:
	case WINCING_GRIMACE:
	case HELM_VERSUS_DEATH:
	case HELM_OF_BEGINNER_S_LUCK:
	case HELM_OF_FULL_NUTRITION:

	case SEDGE_HAT:
	case SKULLCAP:
	case RANDOMIZED_HELMET:
	case CRYSTAL_HELM:
	case HELM_OF_STEEL:
	case DENTED_POT:
	case BASINET:
	case ELVEN_LEATHER_HELM:
	case ELVEN_HELM:
	case WAR_HAT:
	case HIGH_ELVEN_HELM:
	case GNOMISH_HELM:
	case DWARVISH_IRON_HELM:
	case DROVEN_HELM:
	case ORCISH_HELM:
	case HELM_OF_DRAIN_RESISTANCE:
	case HELM_OF_FEAR:
	case BOOGEYMAN_HELMET:
	case HELM_OF_TRUE_SIGHT:
	case HELM_OF_WARNING:
	case HELM_OF_DETOXIFICATION:
	case HELM_OF_NO_DIGESTION:
	case TINFOIL_HELMET:
	case PARANOIA_HELMET:
	case HELM_OF_HUNGER:
	case HELM_OF_STORMS:
	case HELM_OF_DETECT_MONSTERS:
	case HELM_OF_OBSCURED_DISPLAY:
	case RARE_HELMET:
	case HELM_OF_LOSE_IDENTIFICATION:
	case HELM_OF_THIRST:
	case HELM_OF_AMNESIA:
	case HELM_OF_SENSORY_DEPRIVATION:
	case BLACKY_HELMET:
	case ANTI_DRINKER_HELMET:
	case WHISPERING_HELMET:
	case CYPHER_HELM:
	case ANGER_HELM:
	case BIGSCRIPT_HELM:
	case QUIZ_HELM:
	case DIZZY_HELMET:
	case MUTING_HELM:
	case ULCH_HELMET:
	case DIMMER_HELMET:
	case HELM_OF_STARVATION:
	case QUAFFER_HELMET:
	case SOUND_EFFECT_HELMET:

	case MOMHAT:
	case CARTRIDGE_OF_HAVING_A_HORROR:
	case BORDERLESS_HELMET:
	case HELMET_OF_ANTI_SEARCHING:
	case HELM_OF_COUNTER_ROTATION:
	case DELIGHT_HELMET:
	case MESSAGE_FILTER_HELMET:
	case FLICKER_VISOR:
	case SCRIPTED_HELMET:
	case EMPTY_LINE_HELMET:
	case GREEN_SPELL_HELMET:
	case INFOLESS_HELMET:
	case BLUE_SPELL_HELMET:
	case BUGXPLORE_HELMET:
	case YAWNING_VISOR:
	case MORE_HELMET:

	case INCORRECTLY_ADJUSTED_HELMET:
	case CAPTCHA_HELM:
	case HELM_OF_BAD_ALIGNMENT:
	case SOUNDPROOF_HELMET:
	case OUT_OF_MEMORY_HELMET:
	case DUMMY_HELMET_A:
	case DUMMY_HELMET_B:
	case DUMMY_HELMET_C:
	case DUMMY_HELMET_D:
	case DUMMY_HELMET_E:
	case DUMMY_HELMET_F:
	case DUMMY_HELMET_G:
	case DUMMY_HELMET_H:
	case DUMMY_HELMET_I:
	case DUMMY_HELMET_J:
	case DUMMY_HELMET_K:
	case DUMMY_HELMET_L:
	case DUMMY_HELMET_M:
	case DUMMY_HELMET_N:
	case DUMMY_HELMET_O:
	case DUMMY_HELMET_P:
	case DUMMY_HELMET_Q:
	case DUMMY_HELMET_R:
	case DUMMY_HELMET_S:
	case DUMMY_HELMET_T:
	case DUMMY_HELMET_U:
	case DUMMY_HELMET_V:
	case DUMMY_HELMET_W:
	case DUMMY_HELMET_X:
	case DUMMY_HELMET_Y:
	case DUMMY_HELMET_Z:
	case DUMMY_HELMET_AA:
	case DUMMY_HELMET_AB:
	case DUMMY_HELMET_AC:
	case DUMMY_HELMET_AD:
	case DUMMY_HELMET_AE:
	case DUMMY_HELMET_AF:
	case DUMMY_HELMET_AG:
	/* KMH, balance patch -- removed */ /* but re-inserted by Amy */
	case FIRE_HELMET:
	case HELM_OF_SPEED:
	case SKILL_CAP:
	case BOBBLE_HAT:
	case HELM_OF_TELEPORTATION:
	case HELM_OF_TELEPORT_CONTROL:
	case HELM_OF_OPAQUE_THOUGHTS:
	case SCENTY_HELMET:
	case OILSKIN_COIF:
	case HELM_OF_ANTI_MAGIC:
	case HELMET_OF_UNDEAD_WARNING:
	case HELM_OF_DISCOVERY:
	case UNWANTED_HELMET:

	case QUAVERSAL_HELMET:
	case HELM_OF_SHUFFLING:
	case GOLDSPELL_HELMET:
	case AIRHEAD_CAP:
	case FALLOUT_HELMET:
	case IDENTIFY_CURD_HELMET:
	case BAEAEAEP_SPY_HELMET:
	case CRUNCHER_HELMET:
	case DISTORTED_GRIMACE:
	case ELM_ET:
	case SANEMAKER_HELMET:

	    break;
	case FLACK_HELMET:
	case MINING_HELM:
		vision_full_recalc = 1;
		break;
	case DUNCE_CAP:
	    flags.botl = 1;
	    break;
	case CORNUTHAUM:
	    if (!cancelled_don) {
		ABON(A_CHA) += (Role_if(PM_WIZARD) ? -1 : 1);
		flags.botl = 1;
	    }
	    break;
	case HELM_OF_TELEPATHY:
	    /* need to update ability before calling see_monsters() */
	    setworn((struct obj *)0, W_ARMH);
	    see_monsters();
	    return 0;
	case HELM_OF_BRILLIANCE:
		makeknown(uarmh->otyp);
	    break;
	case HELM_OF_OPPOSITE_ALIGNMENT:
	    u.ualign.type = u.ualignbase[A_CURRENT];
	    u.ublessed = 0; /* lose the other god's protection */
	    flags.botl = 1;
	    break;
	case HELM_OF_LAWFUL:
	case HELM_OF_NEUTRAL:
	case HELM_OF_CHAOTIC:
	    if (u.ualign.type != u.ualignbase[A_CURRENT]) {
		    u.ublessed = 0;
		    flags.botl = 1;
	    }
	    u.ualign.type = u.ualignbase[A_CURRENT];
	    flags.botl = 1;

	    break;

	default: impossible(unknown_type_long, c_helmet, uarmh->otyp);
    }
    setworn((struct obj *)0, W_ARMH);
    cancelled_don = FALSE;
    return 0;
}

int
Gloves_on()
{
    long oldprop; 
    if (!uarmg) return 0; 
    oldprop = u.uprops[objects[uarmg->otyp].oc_oprop].extrinsic & ~WORN_GLOVES;

	if (uarmf && uarmf->otyp == BOOTS_OF_SHOCK_RESISTANCE && uarmg && is_metallic(uarmg)) {
		You("receive a little shock.");
		losehp(rnd(2), "little electric shock", KILLED_BY_AN);
	}

    switch(uarmg->otyp) {
	case PLASTEEL_GLOVES:
	case ROCKET_GAUNTLETS:
	case LEATHER_GLOVES:
	case GAUNTLETS_OF_STEEL:
	case GAUNTLETS_OF_TYPING:
	case GAUNTLETS_OF_SLOWING:
	case GAUNTLETS_OF_REFLECTION:
	case ORIHALCYON_GAUNTLETS:
	case GAUNTLETS_OF_PANIC:
	case GAUNTLETS_OF_THE_FORCE:
	case GAUNTLETS_OF_SAFEGUARD:
	case GAUNTLETS_OF_PLUGSUIT:
	case COMMANDER_GLOVES:
	case ROGUES_GLOVES:
	case FIELD_GLOVES:
	case GAUNTLETS:
	case ELVEN_GAUNTLETS:
	case UNKNOWN_GAUNTLETS:
	case GAUNTLETS_OF_FREE_ACTION:
	case GAUNTLETS_OF_LEECH:
	case SILVER_GAUNTLETS:
	case GAUNTLETS_OF_FAST_CASTING:
	case GAUNTLETS_OF_NO_FLICTION:
	case ARCANE_GAUNTLETS:
	case PLACEHOLDER_GLOVES:
	case PROTECTIVE_GLOVES:
		break;
	case OILSKIN_GLOVES:
		if (!uarmg->cursed) {
		    if (Blind)
			pline("%s for a moment.", Tobjnam(uarmg, "vibrate"));
		    else
			pline("%s %s for a moment.",
			      Tobjnam(uarmg, "glow"), hcolor(NH_BLACK));
		    curse(uarmg);
		}
		break;
	case MENU_NOSE_GLOVES:
	case UNWIELDY_GLOVES:
	case CONFUSING_GLOVES:
	case UNDROPPABLE_GLOVES:
	case GAUNTLETS_OF_MISSING_INFORMATI:
	case GAUNTLETS_OF_TRAP_CREATION:
	case SADO_MASO_GLOVES:
	case BANKING_GLOVES:
	case DIFFICULT_GLOVES:
	case CHAOS_GLOVES:
	case LEVELING_GLOVES:
	case GAUNTLETS_OF_STEALING:
	case GAUNTLETS_OF_MISFIRING:
	case EVIL_GLOVES:

	case SCALER_MITTENS:
	case GLOVES_OF_ENERGY_DRAINING:
	case MARY_SUE_GLOVES:
	case GAUNTLETS_OF_BAD_CASTING:
	case METER_GAUNTLETS:
	case WEIGHTING_GAUNTLETS:
	case BLACK_SPELL_GAUNTLETS:
	case HEAVY_GRABBING_GLOVES:
	case GAUNTLETS_OF_REVERSE_ENCHANTME:
	case FUCKUP_MELEE_GAUNTLETS:

	case ORANGE_SPELL_GLOVES:
	case MOJIBAKE_GLOVES:
	case DIXPLOSION_GLOVES:
	case LEFT_APPENDAGE_GLOVES:
	case POKELIE_GLOVES:
	case ALLPICKING_GLOVES:
	case SILVER_SPELL_GLOVES:
	case SPELLDRAIN_GLOVES:

	case DUMMY_GLOVES_A:
	case DUMMY_GLOVES_B:
	case DUMMY_GLOVES_C:
	case DUMMY_GLOVES_D:
	case DUMMY_GLOVES_E:
	case DUMMY_GLOVES_F:
	case DUMMY_GLOVES_G:
	case DUMMY_GLOVES_H:
	case DUMMY_GLOVES_I:
	case DUMMY_GLOVES_J:
	case DUMMY_GLOVES_K:
	case DUMMY_GLOVES_L:
	case DUMMY_GLOVES_M:
	case DUMMY_GLOVES_N:
	case DUMMY_GLOVES_O:
	case DUMMY_GLOVES_P:
	case DUMMY_GLOVES_Q:
	case DUMMY_GLOVES_R:
	case DUMMY_GLOVES_S:
	case DUMMY_GLOVES_T:
	case DUMMY_GLOVES_U:
	case DUMMY_GLOVES_V:
	case DUMMY_GLOVES_W:
	case DUMMY_GLOVES_X:
	case DUMMY_GLOVES_Y:
	case DUMMY_GLOVES_Z:
	case DUMMY_GLOVES_AA:
	case DUMMY_GLOVES_AB:
	case DUMMY_GLOVES_AC:
	case DUMMY_GLOVES_AD:
	case DUMMY_GLOVES_AE:
	case DUMMY_GLOVES_AF:
	case DUMMY_GLOVES_AG:
	case DUMMY_GLOVES_AH:
	case DUMMY_GLOVES_AI:
		if (!uarmg->cursed) curse(uarmg);
		break;

	case GAUNTLETS_OF_GOOD_FORTUNE:
	    set_moreluck();
	    flags.botl = 1;
		break;

	case GAUNTLETS_OF_SWIMMING:
		if (u.uinwater) {
		   pline(FunnyHallu ? "Suddenly, you're floating! Whee!" : "Hey! You can swim!");
		   spoteffects(TRUE);
		}
		break;
	case GAUNTLETS_OF_FUMBLING:
		if (!oldprop && !(HFumbling & ~TIMEOUT))
			incr_itimeout(&HFumbling, rnd(20));
		break;
	case GAUNTLETS_OF_POWER:
		makeknown(uarmg->otyp);
		break;
	case GAUNTLETS_OF_DEXTERITY:
		makeknown(uarmg->otyp);
		break;
	default: impossible(unknown_type_long, c_gloves, uarmg->otyp);
    }

    if (uarmg && uarmg->oartifact == ART_JONADAB_S_METAL_GUARD && (objects[uarmg->otyp].oc_material != MT_IRON)) {
		pline_The("pair of gloves turns into iron gauntlets!");
		objects[uarmg->otyp].oc_material = MT_IRON;
    }

    if (uarmg && itemhasappearance(uarmg, APP_SPANISH_GLOVES) ) {
	      if (!uarmg->cursed) {
			curse(uarmg);
			pline("Whoops - your %s are squeezed by these gloves!", makeplural(body_part(HAND)) );
		}

    }

	if (uarmg && itemhasappearance(uarmg, APP_INVERSE_GLOVES)) {

		if (uarmg->spe) uarmg->spe = -uarmg->spe;
		if (uarmg->spe < 0) {
			curse(uarmg);
			uarmg->hvycurse = 1;
		}

	}

	if (uarmg && itemhasappearance(uarmg, APP_GAMEBLE_GLOVES)) {

		if (rn2(2)) {
			pline("Your gloves feel warm.");
			bless(uarmg);
			if (uarmg->spe < 20) uarmg->spe++;
		} else {
			pline("Your gloves feel cold.");
			curse(uarmg);
			if (uarmg->spe > -20) uarmg->spe--;
		}

	}

    if (uarmg && uarmg->oartifact == ART_AFK_MEANS_ASS_FUCKER) {
	      if (!uarmg->cursed) {
			curse(uarmg);
			pline("afk (ass-fucker)");
		}
    }

    if (uarmg && uarmg->oartifact == ART_SPREAD_YOUR_LEGS_WIDE) {
	      if (!uarmg->cursed) {
			curse(uarmg);
			pline("Christian Grey made sure that you cannot easily take off these gloves again!");
		}
    }

    if (uarmg && uarmg->oartifact == ART_BURN_OR_NO) {
		if(!(HBurnopathy & FROMOUTSIDE)) {
			pline("You feel a soothing fire inside of you!");
			HBurnopathy |= FROMOUTSIDE;
		}

    }

    if (uarmg && uarmg->oartifact == ART_FREEZE_OR_YES) {
		if(!(HFreezopathy & FROMOUTSIDE)) {
			pline("You feel a tasty ice-cream cone inside of you!");
			HFreezopathy |= FROMOUTSIDE;
		}

    }

    if (uarmg && uarmg->oartifact == ART_NUMB_OR_MAYBE) {
		int randomize = rnd(100);
		if (randomize < 51) {
			if(!(HNumbopathy & FROMOUTSIDE)) {
				pline("You feel a beneficial numbness inside of you!");
				HNumbopathy |= FROMOUTSIDE;
			}
		} else if (randomize < 100) {
			pline("Maybe that wasn't the best idea.");
			badeffect();
		} else {
			pline("Uh-oh... your spells became yellow.");
			YellowSpells |= FROMOUTSIDE;
		}

    }

    if (uarmg && uarmg->oartifact == ART_OH_LOOK_AT_THAT) {
		if (P_MAX_SKILL(P_PETKEEPING) == P_ISRESTRICTED) {
			unrestrict_weapon_skill(P_PETKEEPING);
			P_MAX_SKILL(P_PETKEEPING) = P_EXPERT;
			pline("You gain knowledge of the petkeeping skill, but at a price...");
			curse(uarmg);
			uarmg->hvycurse = uarmg->prmcurse = 1;
		}

    }

    if (uarmg && uarmg->oartifact == ART_WOULD_YOU_RAIGHT_THAT) {
		if (P_MAX_SKILL(P_SEARCHING) == P_ISRESTRICTED) {
			unrestrict_weapon_skill(P_SEARCHING);
			P_MAX_SKILL(P_SEARCHING) = P_EXPERT;
			pline("You gain knowledge of the searching skill, but at a price...");
			curse(uarmg);
			uarmg->hvycurse = uarmg->prmcurse = 1;
		}

    }

    if (uarmg && uarmg->oartifact == ART_DEAD_SLAM_THE_TIME_SHUT) {
		if (P_MAX_SKILL(P_DEVICES) == P_ISRESTRICTED) {
			unrestrict_weapon_skill(P_DEVICES);
			P_MAX_SKILL(P_DEVICES) = P_EXPERT;
			pline("You gain knowledge of the devices skill, but at a price...");
			curse(uarmg);
			uarmg->hvycurse = uarmg->prmcurse = 1;
		}

    }

    if (uarmg && uarmg->oartifact == ART_RNG_S_JOY && uarmg->spe == 0) {
		if (!rn2(2)) uarmg->spe = rnd(5);
		else uarmg->spe = -(rnd(5));
    }

    if (uarmg && uarmg->oartifact == ART_FLICTIONLESS_COMBAT) {
	      u.uprops[ITEMCURSING].intrinsic |= FROMOUTSIDE;
		if (!tech_known(T_BLESSING)) {
		    	learntech(T_BLESSING, FROMOUTSIDE, 1);
		    	pline("Suddenly, you know how to use the blessing technique!");
		} else You_feel("that these gloves would have taught you how to use the blessing technique if you didn't already knew it.");
    }

    if (uarmg && uarmg->oartifact == ART_DIMVISION) {
	      u.uprops[WEAKSIGHT].intrinsic |= FROMOUTSIDE;
		if (!tech_known(T_EDDY_WIND)) {
		    	learntech(T_EDDY_WIND, FROMOUTSIDE, 1);
		    	pline("Suddenly, you know how to use the eddy wind technique!");
		} else You_feel("that these gloves would have taught you how to use the eddy wind technique if you didn't already knew it.");
    }

    if (uarmg && uarmg->oartifact == ART_ARABELLA_S_BANK_OF_CROSSRO) {
		if (!uarmg->cursed) curse(uarmg);
		uarmg->hvycurse = 1;
		/* She lures you in with her unsuspecting bank, but then you realize that this bank just deletes all of
		 * your money... and since "banishmentitis" isn't a thing, you gain levelteleportitis instead. --Amy */
    }

    if (uarmg && uarmg->oartifact == ART_ARABELLA_S_GREAT_BANISHER) {
		if (!uarmg->cursed) curse(uarmg);
		uarmg->hvycurse = 1;
    }

	if (uarmg && (AutocursingEquipment || u.uprops[AUTOCURSE_EQUIP].extrinsic || have_autocursestone())) curse(uarmg);

	if (uarmg && uarmg->spe > -10 && (TrashingBugEffect || u.uprops[TRASHING_EFFECT].extrinsic || have_trashstone())) uarmg->spe--;

    return 0;
}

int
Gloves_off()
{
    long oldprop =
	u.uprops[objects[uarmg->otyp].oc_oprop].extrinsic & ~WORN_GLOVES;

    takeoff_mask &= ~W_ARMG;

    switch(uarmg->otyp) {
	case PLASTEEL_GLOVES:
	case ROCKET_GAUNTLETS:
	case LEATHER_GLOVES:
	case OILSKIN_GLOVES:
	case GAUNTLETS_OF_STEEL:
	case GAUNTLETS_OF_TYPING:
	case GAUNTLETS_OF_REFLECTION:
	case ORIHALCYON_GAUNTLETS:
	case GAUNTLETS_OF_SLOWING:
	case MENU_NOSE_GLOVES:
	case UNWIELDY_GLOVES:
	case CONFUSING_GLOVES:
	case UNDROPPABLE_GLOVES:
	case GAUNTLETS_OF_MISSING_INFORMATI:
	case GAUNTLETS_OF_TRAP_CREATION:
	case SADO_MASO_GLOVES:
	case BANKING_GLOVES:
	case DIFFICULT_GLOVES:
	case CHAOS_GLOVES:
	case LEVELING_GLOVES:

	case SCALER_MITTENS:
	case GLOVES_OF_ENERGY_DRAINING:
	case MARY_SUE_GLOVES:
	case GAUNTLETS_OF_BAD_CASTING:
	case METER_GAUNTLETS:
	case WEIGHTING_GAUNTLETS:
	case BLACK_SPELL_GAUNTLETS:
	case HEAVY_GRABBING_GLOVES:
	case GAUNTLETS_OF_REVERSE_ENCHANTME:
	case FUCKUP_MELEE_GAUNTLETS:

	case ORANGE_SPELL_GLOVES:
	case MOJIBAKE_GLOVES:
	case DIXPLOSION_GLOVES:
	case LEFT_APPENDAGE_GLOVES:
	case POKELIE_GLOVES:
	case ALLPICKING_GLOVES:
	case SILVER_SPELL_GLOVES:
	case SPELLDRAIN_GLOVES:

	case GAUNTLETS_OF_STEALING:
	case GAUNTLETS_OF_MISFIRING:
	case GAUNTLETS_OF_PANIC:
	case GAUNTLETS_OF_THE_FORCE:
	case GAUNTLETS_OF_SAFEGUARD:
	case GAUNTLETS_OF_PLUGSUIT:
	case ROGUES_GLOVES:
	case COMMANDER_GLOVES:
	case FIELD_GLOVES:
	case DUMMY_GLOVES_A:
	case DUMMY_GLOVES_B:
	case DUMMY_GLOVES_C:
	case DUMMY_GLOVES_D:
	case DUMMY_GLOVES_E:
	case DUMMY_GLOVES_F:
	case DUMMY_GLOVES_G:
	case DUMMY_GLOVES_H:
	case DUMMY_GLOVES_I:
	case DUMMY_GLOVES_J:
	case DUMMY_GLOVES_K:
	case DUMMY_GLOVES_L:
	case DUMMY_GLOVES_M:
	case DUMMY_GLOVES_N:
	case DUMMY_GLOVES_O:
	case DUMMY_GLOVES_P:
	case DUMMY_GLOVES_Q:
	case DUMMY_GLOVES_R:
	case DUMMY_GLOVES_S:
	case DUMMY_GLOVES_T:
	case DUMMY_GLOVES_U:
	case DUMMY_GLOVES_V:
	case DUMMY_GLOVES_W:
	case DUMMY_GLOVES_X:
	case DUMMY_GLOVES_Y:
	case DUMMY_GLOVES_Z:
	case DUMMY_GLOVES_AA:
	case DUMMY_GLOVES_AB:
	case DUMMY_GLOVES_AC:
	case DUMMY_GLOVES_AD:
	case DUMMY_GLOVES_AE:
	case DUMMY_GLOVES_AF:
	case DUMMY_GLOVES_AG:
	case DUMMY_GLOVES_AH:
	case DUMMY_GLOVES_AI:
	case GAUNTLETS:
	case ELVEN_GAUNTLETS:
	case UNKNOWN_GAUNTLETS:
	case GAUNTLETS_OF_FREE_ACTION:
	case GAUNTLETS_OF_LEECH:
	case SILVER_GAUNTLETS:
	case GAUNTLETS_OF_FAST_CASTING:
	case GAUNTLETS_OF_NO_FLICTION:
	case EVIL_GLOVES:
	case ARCANE_GAUNTLETS:
	case PLACEHOLDER_GLOVES:
	case PROTECTIVE_GLOVES:
	    break;
	case GAUNTLETS_OF_GOOD_FORTUNE:
	    setworn((struct obj *)0, W_ARMG);
	    set_moreluck();
	    flags.botl = 1;
	    return 0;

	case GAUNTLETS_OF_SWIMMING:
	    if (u.uinwater) {
	       You("begin to thrash about!");
	       spoteffects(TRUE);
	    }
	    break;
	case GAUNTLETS_OF_FUMBLING:
	    if (!oldprop && !u.fumbleduration && !(HFumbling & ~TIMEOUT))
		HFumbling = EFumbling = 0;
	    break;
	case GAUNTLETS_OF_POWER:
		makeknown(uarmg->otyp);
	    break;
	case GAUNTLETS_OF_DEXTERITY:
		makeknown(uarmg->otyp);
	    break;
	default: impossible(unknown_type_long, c_gloves, uarmg->otyp);
    }
    setworn((struct obj *)0, W_ARMG);
    cancelled_don = FALSE;
    (void) encumber_msg();		/* immediate feedback for GoP */

    /* Prevent wielding cockatrice when not wearing gloves */
    if (uwep && uwep->otyp == CORPSE &&
		touch_petrifies(&mons[uwep->corpsenm])) {
	char kbuf[BUFSZ];

	You("wield the %s in your bare %s.",
	    corpse_xname(uwep, TRUE), makeplural(body_part(HAND)));
	strcpy(kbuf, an(killer_cxname(uwep, TRUE)));
	instapetrify(kbuf);
	uwepgone();  /* life-saved still doesn't allow touching cockatrice */
    }

    if (uwep && uwep->otyp == PETRIFYIUM_BAR) {
	char kbuf[BUFSZ];

	You("wield the bar in your bare %s.", makeplural(body_part(HAND)));
	strcpy(kbuf, "petrifyium bar");
	instapetrify(kbuf);
	uwepgone();  /* life-saved still doesn't allow touching cockatrice */

    }

    if (uwep && uwep->otyp == PETRIFYIUM_BRA) {
	char kbuf[BUFSZ];

	You("wield the bra in your bare %s.", body_part(HAND));
	strcpy(kbuf, "petrifyium bra");
	instapetrify(kbuf);
	uwepgone();  /* life-saved still doesn't allow touching cockatrice */

    }

    /* KMH -- ...or your secondary weapon when you're wielding it */
    if (u.twoweap && uswapwep && uswapwep->otyp == CORPSE &&
	touch_petrifies(&mons[uswapwep->corpsenm])) {
	char kbuf[BUFSZ];

	You("wield the %s in your bare %s.",
	    corpse_xname(uswapwep, TRUE), body_part(HAND));

	strcpy(kbuf, an(killer_cxname(uswapwep, TRUE)));
	instapetrify(kbuf);
	uswapwepgone();	/* lifesaved still doesn't allow touching cockatrice */
    }

    return 0;
}

int
Shield_on()
{
/*

	if (uarmf && uarmf->otyp == BOOTS_OF_SHOCK_RESISTANCE && uarms && is_metallic(uarms)) {
		You("receive a little shock.");
		losehp(rnd(2), "little electric shock", KILLED_BY_AN);
	}

    switch (uarms->otyp) {
	case SMALL_SHIELD:
	case BULL_SHIELD:
	case ONE_EATING_SIGN:
	case ORGANOSHIELD:
	case ELVEN_SHIELD:
	case URUK_HAI_SHIELD:
	case ORCISH_SHIELD:
	case DWARVISH_ROUNDSHIELD:
	case LARGE_SHIELD:
	case STEEL_SHIELD:
	case METEORIC_STEEL_SHIELD:
	case CRYSTAL_SHIELD:
	case GRIM_SHIELD:
	case SHIELD_OF_REFLECTION:
	case FLAME_SHIELD:
	case ORCISH_GUARD_SHIELD:
	case SHIELD:
	case SILVER_SHIELD:
	case ANCIENT_SHIELD:
	case MIRROR_SHIELD:
	case RAPIRAPI:
	case HIDE_SHIELD:
	case PAPER_SHIELD:
	case ICKY_SHIELD:
	case HEAVY_SHIELD:
	case BARRIER_SHIELD:
	case WEAPON_SIGN:
	case TROLL_SHIELD:
	case TARRIER:
	case CHROME_SHIELD:
	case INVERSION_SHIELD:
	case ANTISHADOW_SHIELD:
	case DIFFICULT_SHIELD:
	case TEZ_SHIELD:
	case SPECIAL_SHIELD:
	case MAGICAL_SHIELD:
	case SHIELD_OF_PEACE:
	case ICE_SHIELD:
	case LIGHTNING_SHIELD:
	case VENOM_SHIELD:
	case SHIELD_OF_LIGHT:
	case SHIELD_OF_MOBILITY:
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
	case DARK_DRAGON_SCALE_SHIELD:
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
	case EVIL_DRAGON_SCALE_SHIELD:
	case YELLOW_DRAGON_SCALE_SHIELD:
		break;
	default: impossible(unknown_type, c_shield, uarms->otyp);
    }
*/

	if (uarms && uarms->otyp == EVIL_DRAGON_SCALE_SHIELD) curse(uarms);
	if (uarms && uarms->otyp == DIFFICULT_SHIELD) curse(uarms);
	if (uarms && uarms->otyp == TEZ_SHIELD) curse(uarms);

    if (uarms && uarms->oartifact == ART_SHATTERED_DREAMS) {
		if (!uarms->cursed) {
			curse(uarms);
			pline("Your dreams are shattered as the shield becomes cursed.");
		}
    }
    if (uarms && uarms->oartifact == ART_NO_FUTURE_BUT_AGONY) {
		if (!uarms->cursed) {
			curse(uarms);
			pline("You feel that there is nothing but agony waiting for you, and as a proof, your shield just became cursed.");
		}
    }
    if (uarms && uarms->oartifact == ART_BURNING_DISK) {
		if (!uarms->cursed) {
			curse(uarms);
			pline("Searing flames of evilness surround your shield.");
		}

    }
    if (uarms && uarms->oartifact == ART_TYPE_OF_ICE_BLOCK_HATES_YO) {
		if (!uarms->cursed) {
			curse(uarms);
			pline("'HAEHAEHAEHAEHAEHAEHAE!!!', the type of ice block says and curses your shield.");
		}

    }
    if (uarms && uarms->oartifact == ART_NUMBED_CAN_T_DO) {
		if (!uarms->cursed) {
			curse(uarms);
			pline("A numbing aura surrounds your shield and welds it to your body!");
		}

    }
    if (uarms && uarms->oartifact == ART_VENOMAT) {
		if (!uarms->cursed) {
			curse(uarms);
			pline("Looks like you're going to wear this venomous shield for a while...");
		}

    }

    if (uarms && uarms->oartifact == ART_SYSTEMATIC_CHAOS) {
		if (!uarms->cursed) {
			curse(uarms);
		}
    }
    if (uarms && uarms->oartifact == ART_BONUS_HOLD) {
		if (!uarms->cursed) {
			curse(uarms);
		}
    }
    if (uarms && uarms->oartifact == ART_GOLDEN_DAWN) {
		if (!uarms->cursed) {
			curse(uarms);
		}
    }
    if (uarms && uarms->oartifact == ART_DOLORES__VIRGINITY) {
		if (!uarms->cursed) {
			curse(uarms);
		}
    }
    if (uarms && uarms->oartifact == ART_CCC_CCC_CCCCCCC) {
		if (!uarms->cursed) {
			curse(uarms);
		}
    }
    if (uarms && uarms->oartifact == ART_FIVE_STAR_PARTY) {
		if (!uarms->cursed) {
			curse(uarms);
		}
    }
    if (uarms && uarms->oartifact == ART_GREXIT_IS_NEAR) {
		if (!uarms->cursed) {
			curse(uarms);
		}
    }
    if (uarms && uarms->oartifact == ART_REAL_PSYCHOS_WEAR_PURPLE) {
		if (!uarms->cursed) {
			curse(uarms);
		}
    }
    if (uarms && uarms->oartifact == ART_REAL_MEN_WEAR_PSYCHOS) {
		if (!uarms->cursed) {
			curse(uarms);
		}
    }

    if (uarms && uarms->oartifact == ART_RNG_S_SAFEGUARD && uarms->spe == 0) {
		if (!rn2(2)) uarms->spe = rnd(7);
		else uarms->spe = -(rnd(7));
    }

	if (uarms && (AutocursingEquipment || u.uprops[AUTOCURSE_EQUIP].extrinsic || have_autocursestone())) curse(uarms);

	if (uarms && uarms->spe > -10 && (TrashingBugEffect || u.uprops[TRASHING_EFFECT].extrinsic || have_trashstone())) uarms->spe--;

    return 0;
}

int
Shield_off()
{
    takeoff_mask &= ~W_ARMS;
/*
    switch (uarms->otyp) {
	case SMALL_SHIELD:
	case BULL_SHIELD:
	case ONE_EATING_SIGN:
	case ORGANOSHIELD:
	case ELVEN_SHIELD:
	case URUK_HAI_SHIELD:
	case ORCISH_SHIELD:
	case DWARVISH_ROUNDSHIELD:
	case LARGE_SHIELD:
	case STEEL_SHIELD:
	case METEORIC_STEEL_SHIELD:
	case CRYSTAL_SHIELD:
	case GRIM_SHIELD:
	case SHIELD_OF_REFLECTION:
	case FLAME_SHIELD:
	case ORCISH_GUARD_SHIELD:
	case SHIELD:
	case SILVER_SHIELD:
	case ANCIENT_SHIELD:
	case MIRROR_SHIELD:
	case RAPIRAPI:
	case HIDE_SHIELD:
	case PAPER_SHIELD:
	case ICKY_SHIELD:
	case HEAVY_SHIELD:
	case BARRIER_SHIELD:
	case WEAPON_SIGN:
	case TROLL_SHIELD:
	case CHROME_SHIELD:
	case INVERSION_SHIELD:
	case ANTISHADOW_SHIELD:
	case TARRIER:
	case DIFFICULT_SHIELD:
	case TEZ_SHIELD:
	case SPECIAL_SHIELD:
	case MAGICAL_SHIELD:
	case SHIELD_OF_PEACE:
	case ICE_SHIELD:
	case LIGHTNING_SHIELD:
	case VENOM_SHIELD:
	case SHIELD_OF_LIGHT:
	case SHIELD_OF_MOBILITY:
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
	case DARK_DRAGON_SCALE_SHIELD:
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
	case EVIL_DRAGON_SCALE_SHIELD:
	case YELLOW_DRAGON_SCALE_SHIELD:
		break;
	default: impossible(unknown_type, c_shield, uarms->otyp);
    }
*/
    setworn((struct obj *)0, W_ARMS);
    return 0;
}

int
Shirt_on()
{
/*
    switch (uarmu->otyp) {
	case HAWAIIAN_SHIRT:
	case T_SHIRT:
		break;
	default: impossible(unknown_type, c_shirt, uarmu->otyp);
    }
*/

	if (uarmf && uarmf->otyp == BOOTS_OF_SHOCK_RESISTANCE && uarmu && is_metallic(uarmu)) {
		You("receive a little shock.");
		losehp(rnd(2), "little electric shock", KILLED_BY_AN);
	}

	/* Cursed underwear/shirt may lifesave a player. It is therefore a good idea to curse them. --Amy
	   Nobles and activistors will be able to voluntarily make them cursed. */

	if(uarmu->otyp == VICTORIAN_UNDERWEAR){

		if ((Role_if(PM_NOBLEMAN) || Role_if(PM_NOBLEWOMAN) || Role_if(PM_ACTIVISTOR) || Race_if(PM_PEACEMAKER) ) && !uarmu->cursed ) {if (yn_function("Do you want your sexy underwear to stick?", ynchars, 'n') == 'y') { curse(uarmu);
			pline("Your fleecy underwear gently welds itself to your curved body!");
			}
		}

		pline("The %s shapes your figure, but it isn't very practical to fight in.",
				OBJ_NAME(objects[uarmu->otyp]));
	}

	if(uarmu->otyp == RUFFLED_SHIRT){

		if ((Role_if(PM_NOBLEMAN) || Role_if(PM_NOBLEWOMAN) || Role_if(PM_ACTIVISTOR) || Race_if(PM_PEACEMAKER) ) && !uarmu->cursed ) {if (yn_function("Do you want your noble shirt to stick?", ynchars, 'n') == 'y') { curse(uarmu);
			pline("Your shirt tickles comfortably on your smooth skin!");
			}
		}

	}

	if (uarmu->otyp == PETRIFYIUM_BRA && (!Stone_resistance || (!IntStone_resistance && !rn2(20))) && !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM)) ) {
		if (!Stoned) {
			if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
			else {
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				u.cnd_stoningcount++;
				pline("You start turning to stone!");
			}
		}
		sprintf(killer_buf, "wearing a petrifyium bra");
		delayed_killer = killer_buf;

	}

	if(uarmu->otyp == BAD_SHIRT) curse(uarmu);

	if (uarmu->oartifact == ART_TIE_DYE_SHIRT_OF_SHAMBHALA) {

		if (uarmu->spe < 10) uarmu->spe = 10;

	}

	if (uarmu->oartifact == ART_TRAP_DUNGEON_OF_SHAMBHALA) {

		if (uarmu->spe < 10) uarmu->spe = 10;

	}

	if (uarmu && (AutocursingEquipment || u.uprops[AUTOCURSE_EQUIP].extrinsic || have_autocursestone())) curse(uarmu);

	if (uarmu && uarmu->spe > -10 && (TrashingBugEffect || u.uprops[TRASHING_EFFECT].extrinsic || have_trashstone())) uarmu->spe--;

    return 0;
}

int
Shirt_off()
{
    takeoff_mask &= ~W_ARMU;
/*
    switch (uarmu->otyp) {
	case HAWAIIAN_SHIRT:
	case T_SHIRT:
		break;
	default: impossible(unknown_type, c_shirt, uarmu->otyp);
    }
*/
    setworn((struct obj *)0, W_ARMU);
    return 0;
}

/* This must be done in worn.c, because one of the possible intrinsics conferred
 * is fire resistance, and we have to immediately set HFire_resistance in worn.c
 * since worn.c will check it before returning.
 */
int
Armor_on()
{
	if (uarmf && uarmf->otyp == BOOTS_OF_SHOCK_RESISTANCE && uarm && is_metallic(uarm)) {
		You("receive a little shock.");
		losehp(rnd(2), "little electric shock", KILLED_BY_AN);
	}

	/* KMH -- certain armor is obvious when worn */
	switch (uarm->otyp) {
		case ROBE_OF_PROTECTION:
		case ROBE_OF_DEFENSE:
		case ROBE_OF_WEAKNESS:
			makeknown(uarm->otyp);
			break;
	}
	if (uarm && uarm->otyp == ROBE_OF_NASTINESS) {
		if (!uarm->cursed) curse(uarm);
	}
	if (uarm && uarm->otyp == EVIL_DRAGON_SCALE_MAIL) {
		if (!uarm->cursed) curse(uarm);
	}
	if (uarm && uarm->otyp == EVIL_DRAGON_SCALES) {
		if (!uarm->cursed) curse(uarm);
	}
	if (uarm && uarm->otyp == EVIL_PLATE_MAIL) {
		if (!uarm->cursed) curse(uarm);
	}
	if (uarm && uarm->otyp == ROBE_OF_DESTINY) {
		if (!uarm->cursed) curse(uarm);
	}
	if (uarm && uarm->otyp == EVIL_LEATHER_ARMOR) {
		if (!uarm->cursed) curse(uarm);
	}

	if (uarm && uarm->oartifact == ART_NOW_FOR_THE_DISCHARGE) {

		if (!tech_known(T_SIGIL_DISCHARGE)) {
		      u.uprops[DISCHARGE_BUG].intrinsic |= FROMOUTSIDE;
			learntech(T_SIGIL_DISCHARGE, FROMOUTSIDE, 1);
		    	pline("Suddenly, you know how to use the sigil of discharge technique!");
		}

	}
	if (uarm && uarm->oartifact == ART_ORBCHARGE) {

		if (!tech_known(T_BUGGARD)) {
			u.uprops[DEAC_DETECT_MONSTERS].intrinsic += 1000000;
			learntech(T_BUGGARD, FROMOUTSIDE, 1);
		    	pline("Suddenly, you know how to use the buggard technique!");
			pline("Your detect monsters has been deactivated for a very long time.");
		}

	}

	if (uarm && !(uarm->cursed) && uarm->oartifact == ART_SUPERESCAPE_MAIL) {
		pline("BEEEEEEEP! Your armor is cursed!");
		curse(uarm);
	}

	if (uarm && !(uarm->cursed) && uarm->oartifact == ART_ABSOLUTE_MONSTER_MAIL) {
		pline("BEEEEEEEP! Your armor is cursed!");
		curse(uarm);
	}

	if (uarm && !(uarm->cursed) && uarm->oartifact == ART_FLEEING_MINE_MAIL) {
		pline("BEEEEEEEP! Your armor is cursed!");
		curse(uarm);
	}

	if (uarm && !(uarm->cursed) && uarm->oartifact == ART_SHRINK_S_AID) {
		pline("Well done! Your armor is now cursed.");
		curse(uarm);
	}

	if (uarm && !(uarm->hvycurse) && uarm->oartifact == ART_WATER_SHYNESS) {
		pline("Your armor is surrounded by an evil black aura.");
		curse(uarm);
		uarm->hvycurse = 1;
	}
	if (uarm && !(uarm->hvycurse) && uarm->oartifact == ART_PLANTOPLIM) {
		pline("Your armor is surrounded by an evil black aura.");
		curse(uarm);
		uarm->hvycurse = 1;
	}
	if (uarm && !(uarm->hvycurse) && uarm->oartifact == ART_WRONG_TURN) {

		int mntmp;

		curse(uarm);
		uarm->hvycurse = 1;

		do {
			mntmp = rn2(NUMMONS);
		} while(( (notake(&mons[mntmp]) && rn2(4) ) || !polyok(&mons[mntmp]) || (!(is_undead(&mons[mntmp])) ) || ((mons[mntmp].mmove == 1) && rn2(4) ) || ((mons[mntmp].mmove == 2) && rn2(3) ) || ((mons[mntmp].mmove == 3) && rn2(2) ) || ((mons[mntmp].mmove == 4) && !rn2(3) ) || ( (mons[mntmp].mlevel < 10) && ((mons[mntmp].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[mntmp]) && rn2(2) ) || ( is_nonmoving(&mons[mntmp]) && rn2(5) ) || ( is_eel(&mons[mntmp]) && rn2(5) ) || ( is_nonmoving(&mons[mntmp]) && rn2(20) ) || (is_jonadabmonster(&mons[mntmp]) && rn2(20)) || ( uncommon2(&mons[mntmp]) && !rn2(4) ) || ( uncommon3(&mons[mntmp]) && !rn2(3) ) || ( uncommon5(&mons[mntmp]) && !rn2(2) ) || ( uncommon7(&mons[mntmp]) && rn2(3) ) || ( uncommon10(&mons[mntmp]) && rn2(5) ) || ( is_eel(&mons[mntmp]) && rn2(20) ) ) );

		u.wormpolymorph = mntmp;
		polyself(FALSE);
	}

	if (uarm && uarm->oartifact == ART_RNG_S_FUN && uarm->spe == 0) {
		if (!rn2(2)) uarm->spe = rnd(5);
		else uarm->spe = -(rnd(5));
	}

	if (uarm && uarm->oartifact == ART_RNG_S_PRIDE && uarm->spe == 0) {
		if (!rn2(2)) uarm->spe = rnd(5);
		else uarm->spe = -(rnd(5));
	}

	if (uarm && uarm->oartifact == ART_RNG_S_COMPLETION && uarm->spe == 0) {
		if (!rn2(2)) uarm->spe = rnd(7);
		else uarm->spe = -(rnd(7));
	}

	if (uarm && uarm->oartifact == ART_SOFT_GIRL && !flags.female) {
		pline("You turn into a soft girl!");
		change_sex();
	}

	if (uarm && uarm->oartifact == ART_INCREDIBLY_SEXY_SQUEAKING && uarm->spe > -5) {
		uarm->spe -= 5;

		int attempts = 0;
		register struct permonst *ptrZ;
		register struct monst *bossmon;
sexysqueaking:
		do {

			ptrZ = rndmonst();
			attempts++;
			if (!rn2(2000)) reset_rndmonst(NON_PM);

		} while ( (!ptrZ || (ptrZ && !(ptrZ->msound == MS_FART_NORMAL))) && attempts < 50000);

		if (ptrZ && ptrZ->msound == MS_FART_NORMAL) {
			bossmon = makemon(ptrZ, u.ux, u.uy, NO_MM_FLAGS);
		}
		else if (rn2(50)) {
			attempts = 0;
			goto sexysqueaking;
		}

		if (bossmon) {
			tamedog(bossmon, (struct obj *) 0, TRUE);
			pline("Someone sexy is waiting for you nearby...");
		}

	}

	if (uarm && uarm->oartifact == ART_ARMOR_OF_ISILDUR && uarm->spe < 1) uarm->spe = rnd(10);
	if (uarm && uarm->oartifact == ART_COCKBANGER_ARMOR && uarm->spe < 1) uarm->spe = rnd(10);

	if (uarm && (AutocursingEquipment || u.uprops[AUTOCURSE_EQUIP].extrinsic || have_autocursestone())) curse(uarm);

	if (uarm && uarm->spe > -10 && (TrashingBugEffect || u.uprops[TRASHING_EFFECT].extrinsic || have_trashstone())) uarm->spe--;

    return 0;
}

int
Armor_off()
{
    takeoff_mask &= ~W_ARM;
    setworn((struct obj *)0, W_ARM);
    cancelled_don = FALSE;
    return 0;
}

/* The gone functions differ from the off functions in that if you die from
 * taking it off and have life saving, you still die.
 */
int
Armor_gone()
{
    takeoff_mask &= ~W_ARM;
    setnotworn(uarm);
    cancelled_don = FALSE;
    return 0;
}

void
Amulet_on()
{
    long oldprop;
	if (!uamul) return;
	oldprop = u.uprops[objects[uamul->otyp].oc_oprop].extrinsic & ~WORN_AMUL;

	if (uarmf && uarmf->otyp == BOOTS_OF_SHOCK_RESISTANCE && uamul && is_metallic(uamul)) {
		You("receive a little shock.");
		losehp(rnd(2), "little electric shock", KILLED_BY_AN);
	}

    if (uamul && uamul->oartifact == ART_TSCHOECK_KLOECK) { /* needs to be done here because it disintegrates when worn */
		if (u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic < 2000000) {
			u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1000000;
			pline("Your polymorph control has been deactivated for a very long time.");
		}
    }

    if (uamul && uamul->oartifact == ART_RATSCH_WATSCH) { /* needs to be done here in case it disintegrates when worn */
		if (u.uprops[DEAC_TELEPORT_CONTROL].intrinsic < 2000000) {
			u.uprops[DEAC_TELEPORT_CONTROL].intrinsic += 1000000;
			pline("Your teleport control has been deactivated for a very long time.");
		}
    }

    if (uamul && uamul->oartifact == ART_DIKKIN_S_DRAGON_TEETH && uarm) {
		(void) Armor_gone();
		pline("Your armor falls off.");
    }

    if (uamul && uamul->oartifact == ART_FORMTAKER) { /* ditto */
		HPolymorph |= FROMOUTSIDE;
		pline("Your form feels unstable!");
    }

    if (uamul && uamul->oartifact == ART_AUTOMATICALLY_METAL && !is_metallic(uamul)) {
		pline_The("amulet automatically becomes metal.");
		objects[uamul->otyp].oc_material = MT_METAL;
    }

    switch(uamul->otyp) {
	case AMULET_OF_ESP:
#if 0	/* OBSOLETE */
		if(uamul->oartifact == ART_MEDALLION_OF_SHIFTERS) rescham();
#endif
	case AMULET_OF_LIFE_SAVING:
	case AMULET_VERSUS_POISON:
	case AMULET_OF_DRAIN_RESISTANCE:
	case AMULET_OF_REFLECTION:
	case AMULET_OF_DEPETRIFY:
	case AMULET_OF_MAGICAL_BREATHING:
	/* KMH, balance patch -- removed
	case AMULET_OF_REGENERATION:
	case AMULET_OF_CONFLICT:*/
	case FAKE_AMULET_OF_YENDOR:
	case AMULET_OF_SECOND_CHANCE:
		break;
	case AMULET_OF_UNDEAD_WARNING:
		break;

	case AMULET_OF_TIME:
		pline("The time is: %d:%d", getlt()->tm_hour, getlt()->tm_min);
		break;

	case AMULET_OF_RMB_LOSS:
	case AMULET_OF_TRAP_WARPING:
	case AMULET_OF_HI_LEVEL_CASTING:
	case AMULET_OF_EXPLOSION:
	case AMULET_OF_ANTI_TELEPORTATION:
	case AMULET_OF_WRONG_SEEING:
	case AMULET_OF_WEAK_MAGIC:
	case AMULET_OF_DIRECTIONAL_SWAP:
	case AMULET_OF_SUDDEN_CURSE:
	case AMULET_OF_ANTI_EXPERIENCE:
	case AMULET_OF_ITEM_TELEPORTATION:
	case AMULET_OF_PREMATURE_DEATH:
	case AMULET_OF_DANGER:
	case AMULET_OF_SCREWY_INTERFACE:
	case AMULET_OF_BONES:
	case AMULET_OF_SPELL_FORGETTING:
	case AMULET_OF_VULNERABILITY:

	case AMULET_OF_ITEM_INSECURITY:
	case AMULET_OF_STOPPED_EVOLUTION:
	case AMULET_OF_INITIALIZATION_FAILU:
	case AMULET_OF_REAL_SCREWERY:
	case AMULET_OF_ENEMY_SAVING:
	case AMULET_OF_INCREASED_FREQUENCY:
	case AMULET_OF_SPELL_METAL:
	case AMULET_OF_TECHOUT:
	case AMULET_OF_BAD_PART:
	case AMULET_OF_EVIL_VARIANT:

	case AMULET_OF_HOSTILITY:
	case AMULET_OF_SANITY_TREBLE:
	case AMULET_OF_EVIL_CRAFTING:
	case AMULET_OF_EDIBILITY:
	case AMULET_OF_WAKING:
	case AMULET_OF_TRASH:
	case AMULET_OF_UNDRESSING:
	case AMULET_OF_STARLIGHT:

		if (uamul && !uamul->cursed) curse(uamul);

		break;

	case AMULET_OF_UNCHANGING:
		if (Slimed) {
		    Slimed = 0;
		    flags.botl = 1;
		}
		break;
	case AMULET_OF_FUMBLING:
		if (!oldprop && !(HFumbling & ~TIMEOUT))
			incr_itimeout(&HFumbling, rnd(20));
		break;
	case AMULET_OF_CHANGE:
	    {
		int orig_sex = poly_gender();

		if (Unchanging) break;
		change_sex();

		if (practicantterror) {
			pline("%s thunders: 'That type of surgery is dangerous! You've practiced it without adhering to standard safety precautions, meaning there's now a fine of 20000 zorkmids and I'll also collect any money you gain from now on to teach you a lesson!'", noroelaname());
			fineforpracticant(20000, 0, 0);
			BankTrapEffect |= FROMOUTSIDE;
		}

		/* Don't use same message as polymorph */
		if (orig_sex != poly_gender()) {
		    makeknown(AMULET_OF_CHANGE);
		    You("are suddenly very %s!", flags.female ? "feminine"
			: "masculine");
		    flags.botl = 1;
		} else
		    /* already polymorphed into single-gender monster; only
		       changed the character's base sex */
		    You("don't feel like yourself.");
		pline_The("amulet disintegrates!");
		if (orig_sex == poly_gender() && uamul->dknown &&
			!objects[AMULET_OF_CHANGE].oc_name_known &&
			!objects[AMULET_OF_CHANGE].oc_uname)
		    docall(uamul);
		useup(uamul);
		break;
	    }
	/* KMH, balance patch -- removed */
	/* but re-inserted by Amy */
	case AMULET_OF_POLYMORPH:
		makeknown(AMULET_OF_POLYMORPH);
		You_feel("rather strange.");
		polyself(FALSE);
		flags.botl = 1;
		pline("The amulet disintegrates!");
		useup(uamul);
		break;
	case AMULET_OF_STRANGULATION:
		makeknown(AMULET_OF_STRANGULATION);

		if (uarmh && uarmh->oartifact == ART_VACUUM_CLEANER_DEATH) {
			u.youaredead = 1;
			pline("You suffocate."); /* magical breathing doesn't help --Amy */
			killer = "amulet of strangulation";
			killer_format = KILLED_BY;
			done(CHOKING);
			u.youaredead = 0;
		}

		pline("It constricts your throat!");
		Strangled = 11;
		break;

	case AMULET_OF_STONE:
		makeknown(AMULET_OF_STONE);
		if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(poly_when_stoned(youmonst.data) &&
				 polymon(PM_STONE_GOLEM)) ) {
			pline("You start turning to stone!");
			if (Hallucination && rn2(10)) pline("But you are already stoned.");
			else {
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				u.cnd_stoningcount++;
				stop_occupation();
				delayed_killer = "amulet of stone";
			}
		}
		break;
	case AMULET_OF_DEPRESSION:
		/* don't always give a message, and a vague one if at all --Amy */
		if (!rn2(3)) You_feel("down...");
		if (u.uluck > 0) u.uluck = -u.uluck;
		if (u.ualign.record > 0) u.ualign.record = -u.ualign.record;

		break;


	case AMULET_OF_RESTFUL_SLEEP:
		if Race_if(PM_KOBOLT) break;
		if(uamul->blessed) {
			char buf[BUFSZ];
			int sleeptime;
  
			makeknown(AMULET_OF_RESTFUL_SLEEP);
			do {
			getlin("How many moves do you wish to sleep for? [1-500]", buf);
				    sleeptime = (!*buf || *buf=='\033') ? 0 : atoi(buf);
				} while (sleeptime < 1 || sleeptime > 500);
			if (sleeptime > 0) {
				You("sit down and fall asleep.");
				nomul(-sleeptime, "taking a refreshing nap", FALSE);
				u.usleep = 1;
				nomovemsg = "You wake up from your refreshing nap.";
		HSleeping = rnd(1000);
			}                
		} else HSleeping = rnd(1000);
		break;
	case AMULET_OF_DATA_STORAGE:
		You_feel("full of knowledge.");
		break;
	case AMULET_OF_CLIMBING:
		You_feel("ready for climbing.");
		makeknown(AMULET_OF_CLIMBING);
		break;
	case AMULET_OF_GUARDING:
		makeknown(AMULET_OF_GUARDING);
		break;
	case AMULET_OF_YENDOR:
		break;
	/* KMH, balance patch -- added */
	case AMULET_VERSUS_STONE:
		/*uunstone();*/
		/* unbalanced. You could do it indefinitely. --Amy */
		break;
	case AMULET_OF_FLYING:
		if (!(EFlying & ~W_AMUL) && !is_flyer(youmonst.data)) {
			You_feel("like flying!");
			if (!Levitation)
				float_up();
			makeknown(AMULET_OF_FLYING);
		}
		break;
    }

    if (uamul && uamul->oartifact == ART___TH_NAZGUL) {
		curse(uamul);
		uamul->hvycurse = 1;
		pline("Mortal creatures cannot master such a powerful amulet, and are therefore afflicted by a dark, evil curse!");
    }

    if (uamul && uamul->oartifact == ART_WALT_VERSUS_ANNA) {
		curse(uamul);
		pline("Now you have to fight on Walt's side, and to make sure you cannot run off, the amulet cursed itself!");
    }

    if (uamul && uamul->oartifact == ART_NAZGUL_S_REVENGE) {
		curse(uamul);
		uamul->hvycurse = 1;
		pline("Mortal creatures cannot master such a powerful amulet, and are therefore afflicted by a dark, evil curse!");
    }

    if (uamul && uamul->oartifact == ART_LOW_ZERO_NUMBER) {
		if (!uamul->cursed) curse(uamul);
    }

    if (uamul && uamul->oartifact == ART_ARABELLA_S_PRECIOUS_GADGET) {
		if (!uamul->cursed) curse(uamul);
    }

	if (uamul && (AutocursingEquipment || u.uprops[AUTOCURSE_EQUIP].extrinsic || have_autocursestone())) curse(uamul);

}

void
Amulet_off()
{
    int otyp = uamul->otyp;
    long oldprop = u.uprops[objects[otyp].oc_oprop].extrinsic & ~WORN_AMUL;
    takeoff_mask &= ~W_AMUL;

	if (uamul && !(Race_if(PM_INKA)) && objects[(uamul)->otyp].oc_material == MT_INKA) {
		pline("The inka amulet tries to resist being taken off, and severely strangulates you before you can finally slip it from your %s!", body_part(NECK));
		if (Upolyd) losehp( (u.mhmax / 2) + 2, "inka amulet", KILLED_BY_AN);
		else losehp( (u.uhpmax / 2) + 2, "inka amulet", KILLED_BY_AN);
	}

    switch(uamul->otyp) {
	case AMULET_OF_ESP:
		/* need to update ability before calling see_monsters() */
#if 0	/* OBSOLETE */
		if(uamul->oartifact == ART_MEDALLION_OF_SHIFTERS) restartcham();
#endif
		setworn((struct obj *)0, W_AMUL);
		see_monsters();
		return;
	case AMULET_OF_LIFE_SAVING:
	case AMULET_VERSUS_POISON:
	case AMULET_OF_DRAIN_RESISTANCE:
	/* KMH, balance patch -- removed
	case AMULET_OF_REGENERATION:
	case AMULET_OF_CONFLICT:*/
	/* KMH, balance patch -- added */
	case AMULET_VERSUS_STONE:
	case AMULET_OF_REFLECTION:
	case AMULET_OF_SECOND_CHANCE:
	case AMULET_OF_DEPETRIFY:
	case AMULET_OF_CHANGE:
	case AMULET_OF_UNCHANGING:
	case FAKE_AMULET_OF_YENDOR:
		break;
	case AMULET_OF_UNDEAD_WARNING:
		break;

	case AMULET_OF_FUMBLING:
		if (!oldprop && !u.fumbleduration && !(HFumbling & ~TIMEOUT))
			HFumbling = EFumbling = 0;
		break;
	case AMULET_OF_MAGICAL_BREATHING:
		if (Underwater) {
		    /* HMagical_breathing must be set off
			before calling drown() */
		    setworn((struct obj *)0, W_AMUL);
		    if (!breathless(youmonst.data) && !amphibious(youmonst.data)
						&& !Swimming) {
			You("suddenly inhale an unhealthy amount of water!");
		    	(void) drown();
		    }
		    return;
		}
		break;
	case AMULET_OF_STRANGULATION:
		if (Strangled) {
			You("can breathe more easily!");
			Strangled = 0;
		}
		break;
	case AMULET_OF_RESTFUL_SLEEP:
		setworn((struct obj *)0, W_AMUL);
		if (!ESleeping && !(HSleeping & INTRINSIC) && !Race_if(PM_KOBOLT))
			HSleeping = 0;
		return;
	case AMULET_OF_DATA_STORAGE:
		You_feel("intellectually poor.");
		break;
	/* KMH, balance patch -- added */
	case AMULET_OF_FLYING:
		setworn((struct obj *)0, W_AMUL);
		(void) float_down(0L, 0L);
		return;
	case AMULET_OF_YENDOR:
		break;
    }
    setworn((struct obj *)0, W_AMUL);
    return;
}

void
Implant_on()

{

    long oldprop;
    int bucidchance;
    int typeidvalue;
	if (!uimplant) return;
	oldprop = u.uprops[objects[uimplant->otyp].oc_oprop].extrinsic & ~WORN_IMPLANT;

	if (uarmf && uarmf->otyp == BOOTS_OF_SHOCK_RESISTANCE && uimplant && is_metallic(uimplant)) {
		You("receive a little shock.");
		losehp(rnd(2), "little electric shock", KILLED_BY_AN);
	}

    switch(uimplant->otyp) {

    }

    if (uimplant && uimplant->oartifact == ART_HENRIETTA_S_TENACIOUSNESS) {
		if (!uimplant->cursed) curse(uimplant);
    }

    if (uimplant && uimplant->oartifact == ART_CORONATION_CULMINATION) {
	if (!u.coronationculmination) {

		You("are crowned!");
		u.coronationculmination = TRUE;
		u.weapon_slots++;

		struct obj *durifact;

		boolean havegifts = u.ugifts;
		if (!havegifts) u.ugifts++;

		durifact = mk_artifact((struct obj *)0, !rn2(3) ? A_CHAOTIC : rn2(2) ? A_NEUTRAL : A_LAWFUL, TRUE);
		if (durifact) {

			int duriskill = get_obj_skill(durifact, TRUE);

			if (P_MAX_SKILL(duriskill) == P_ISRESTRICTED) {
			    unrestrict_weapon_skill(duriskill);
			} else if (P_MAX_SKILL(duriskill) == P_UNSKILLED) {
				unrestrict_weapon_skill(duriskill);
				P_MAX_SKILL(duriskill) = P_BASIC;
			} else if (rn2(2) && P_MAX_SKILL(duriskill) == P_BASIC) {
				P_MAX_SKILL(duriskill) = P_SKILLED;
			} else if (!rn2(4) && P_MAX_SKILL(duriskill) == P_SKILLED) {
				P_MAX_SKILL(duriskill) = P_EXPERT;
			} else if (!rn2(10) && P_MAX_SKILL(duriskill) == P_EXPERT) {
				P_MAX_SKILL(duriskill) = P_MASTER;
			} else if (!rn2(100) && P_MAX_SKILL(duriskill) == P_MASTER) {
				P_MAX_SKILL(duriskill) = P_GRAND_MASTER;
			} else if (!rn2(200) && P_MAX_SKILL(duriskill) == P_GRAND_MASTER) {
				P_MAX_SKILL(duriskill) = P_SUPREME_MASTER;
			}

			if (Race_if(PM_RUSMOT)) {
				if (P_MAX_SKILL(duriskill) == P_ISRESTRICTED) {
				    unrestrict_weapon_skill(duriskill);
				} else if (P_MAX_SKILL(duriskill) == P_UNSKILLED) {
					unrestrict_weapon_skill(duriskill);
					P_MAX_SKILL(duriskill) = P_BASIC;
				} else if (rn2(2) && P_MAX_SKILL(duriskill) == P_BASIC) {
					P_MAX_SKILL(duriskill) = P_SKILLED;
				} else if (!rn2(4) && P_MAX_SKILL(duriskill) == P_SKILLED) {
					P_MAX_SKILL(duriskill) = P_EXPERT;
				} else if (!rn2(10) && P_MAX_SKILL(duriskill) == P_EXPERT) {
					P_MAX_SKILL(duriskill) = P_MASTER;
				} else if (!rn2(100) && P_MAX_SKILL(duriskill) == P_MASTER) {
					P_MAX_SKILL(duriskill) = P_GRAND_MASTER;
				} else if (!rn2(200) && P_MAX_SKILL(duriskill) == P_GRAND_MASTER) {
					P_MAX_SKILL(duriskill) = P_SUPREME_MASTER;
				}
			}

			dropy(durifact);
			discover_artifact(durifact->oartifact);
			pline("An object appeared at your %s!", makeplural(body_part(FOOT)));
		}
		if (!havegifts) u.ugifts--;

	}
    }

    if (uimplant && uimplant->oartifact == ART_RNG_S_EXTRAVAGANZA && uimplant->spe == 0) {
		if (!rn2(2)) uimplant->spe = rnd(9);
		else uimplant->spe = -(rnd(9));
    }

    if (uimplant && uimplant->oartifact == ART_SLEX_WANTS_YOU_TO_DIE_A_PA) {
		u.uhpmax++;
		flags.botl = TRUE;

		getnastytrapintrinsic();
    }

	if (uimplant && (AutocursingEquipment || u.uprops[AUTOCURSE_EQUIP].extrinsic || have_autocursestone())) curse(uimplant);

    if (!rn2(3)) curse(uimplant);

    if (uimplant->otyp >= IMPLANT_OF_QUICKENING && uimplant->otyp <= IMPLANT_OF_BLITZEN) curse(uimplant);
    if (uimplant->otyp >= IMPLANT_OF_BUTCHERY && uimplant->otyp <= IMPLANT_OF_FREEDOM) curse(uimplant);
    if (uimplant->otyp >= IMPLANT_OF_TOTAL_NONSENSE && uimplant->otyp <= IMPLANT_OF_GALVANIZATION) curse(uimplant);
    if (uimplant->otyp >= IMPLANT_OF_YOUR_MOMMA && uimplant->otyp <= IMPLANT_OF_ENFORCING) curse(uimplant);

    if (!PlayerCannotUseSkills) {
	bucidchance = 0;
	switch (P_SKILL(P_IMPLANTS)) {
		default: break;
		case P_SKILLED: bucidchance = 1; break;
		case P_EXPERT: bucidchance = 3; break;
		case P_MASTER: bucidchance = 5; break;
		case P_GRAND_MASTER: bucidchance = 7; break;
		case P_SUPREME_MASTER: bucidchance = 10; break;
	}
	if (rnd(10) <= bucidchance) uimplant->bknown = TRUE;

	typeidvalue = (uimplant->shirtmessage / 1000);
	if (bucidchance >= 1) {
		if (((typeidvalue % 10) + 1) <= bucidchance) {
			makeknown(uimplant->otyp);
		}
	}

    }

	/* all implants with an enchantment will autoidentify it upon wearing, even if you can't benefit from it --Amy */
    if (uimplant && objects[uimplant->otyp].oc_charged) {
	uimplant->known = TRUE;
    }


}

void
Implant_off()

{

    int otyp = uimplant->otyp;
    long oldprop = u.uprops[objects[otyp].oc_oprop].extrinsic & ~WORN_IMPLANT;
    takeoff_mask &= ~W_IMPLANT;

    setworn((struct obj *)0, W_IMPLANT);
    return;

}

void
Ring_on(obj)
register struct obj *obj;
{
    long oldprop = u.uprops[objects[obj->otyp].oc_oprop].extrinsic;
    int old_attrib, which;

    if (obj == uwep) setuwep((struct obj *) 0, TRUE, TRUE);
    if (obj == uswapwep) setuswapwep((struct obj *) 0, TRUE);
    if (obj == uquiver) setuqwep((struct obj *) 0);

    /* only mask out W_RING when we don't have both
       left and right rings of the same type */
    if ((oldprop & W_RING) != W_RING) oldprop &= ~W_RING;

	if (uarmf && uarmf->otyp == BOOTS_OF_SHOCK_RESISTANCE && obj && is_metallic(obj)) {
		You("receive a little shock.");
		losehp(rnd(2), "little electric shock", KILLED_BY_AN);
	}

    switch(obj->otyp){
	case RIN_TELEPORTATION:
	case RIN_REGENERATION:
	case RIN_SEARCHING:
	case RIN_STEALTH:
	case RIN_HUNGER:
	case RIN_AGGRAVATE_MONSTER:
	case RIN_POISON_RESISTANCE:
	case RIN_FIRE_RESISTANCE:
	case RIN_COLD_RESISTANCE:
	case RIN_FEAR_RESISTANCE:
	case RIN_SHOCK_RESISTANCE:
	case RIN_CONFLICT:
	case RIN_TELEPORT_CONTROL:
	case RIN_POLYMORPH:
	case RIN_POLYMORPH_CONTROL:
	/* KMH, balance patch -- now an amulet
	case RIN_DRAIN_RESISTANCE: */
	/* KMH -- added */
	case RIN_MOOD:
	case RIN_FREE_ACTION:                
	case RIN_SLOW_DIGESTION:
	case RIN_SUSTAIN_ABILITY:
	case MEAT_RING:
		break;
	case RIN_LIGHT:
		vision_full_recalc = 1;
		break;
	case RIN_HALLUCINATION:
	case RIN_CURSE:
	case RIN_DOOM:

		if (!obj->cursed) {
		    if (Blind)
			pline("%s for a moment.", Tobjnam(obj, "vibrate"));
		    else
			pline("%s %s for a moment.",
			      Tobjnam(obj, "glow"), hcolor(NH_BLACK));
		    curse(obj);
		}

		break;

	case RIN_INTRINSIC_LOSS:
	case RIN_BLOOD_LOSS:
	case RIN_NASTINESS:
	case RIN_BAD_EFFECT:
	case RIN_SUPERSCROLLING:
	case RIN_ANTI_DROP:
	case RIN_ENSNARING:
	case RIN_DIARRHEA:
	case RIN_DISENGRAVING:
	case RIN_NO_SKILL:
	case RIN_LOW_STATS:
	case RIN_FAILED_TRAINING:
	case RIN_FAILED_EXERCISE:
	case RIN_AUTOCURSING:
	case RIN_TIME_SPENDING:
	case RIN_FAST_METABOLISM:
	case RIN_DANGER:
	case RIN_INCESSANT_FARTING:
	case RIN_LAMITY:
	case RIN_WALLFARTING:

	case RIN_LOOTCUT:
	case RIN_FORM_SHIFTING:
	case RIN_LAGGING:
	case RIN_BLESSCURSING:
	case RIN_ILLITERACY:
	case RIN_STAT_DECREASE:
	case RIN_SANITY_TIMEOUT:

	case RIN_WIMPINESS:
	case RIN_USING_HAZARD:
	case RIN_EXERTION_LOSS:
	case RIN_PETCAT:
	case RIN_POSSESSION_IDENTIFICATION:
	case RIN_DAYSHIFT:
	case RIN_DECONSTRUCTION:

		if (!obj->cursed) curse(obj);

		break;

	case RIN_ILLNESS:

		make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20), "a ring of illness", TRUE, SICK_NONVOMITABLE);

		if (!obj->cursed) curse(obj);

		break;

	case RIN_SLEEPING:        
		if Race_if(PM_KOBOLT) break;
		HSleeping = rnd(1000);
		break;
#if 0
	case RIN_INDIGESTION:
		incr_itimeout(&HIndigestion, rnd(20));
		break;
#endif
	case RIN_WARNING:
		see_monsters();
		break;
	case RIN_SEE_INVISIBLE:
		/* can now see invisible monsters */
		set_mimic_blocking(); /* do special mimic handling */
		see_monsters();
		see_objects();

		if (Invis && !oldprop && !HSee_invisible &&
				!perceives(youmonst.data) && !Blind) {
		    newsym(u.ux,u.uy);
		    pline("Suddenly you are transparent, but there!");
		    makeknown(RIN_SEE_INVISIBLE);
		}
		break;
	case RIN_INFRAVISION:
		see_monsters();

		break;
	case RIN_TIMELY_BACKUP:
		You_feel("absolutely safe.");
		break;
	case RIN_INVISIBILITY:
		if (!oldprop && !HInvis && !BInvis && !Blind) {
		    makeknown(RIN_INVISIBILITY);
		    newsym(u.ux,u.uy);
		    self_invis_message();
		}
		break;
	case RIN_LEVITATION:
		if (!oldprop && !HLevitation) {
		    flags.botl = 1;
		    float_up();
		    makeknown(RIN_LEVITATION);
		    spoteffects(FALSE);	/* for sinks */
		}
		break;
	case RIN_GAIN_STRENGTH:
		which = A_STR;
		goto adjust_attrib;
	case RIN_GAIN_CONSTITUTION:
		which = A_CON;
		goto adjust_attrib;
	case RIN_ADORNMENT:
		which = A_CHA;
 adjust_attrib:
		old_attrib = ACURR(which);
		ABON(which) += obj->spe;
		if (ACURR(which) != old_attrib ||
			(objects[obj->otyp].oc_name_known &&
			    old_attrib != 25 && old_attrib != 3)) {
		    flags.botl = 1;
		    makeknown(obj->otyp);
		    obj->known = 1;
		    update_inventory();
		}
		break;
	case RIN_GAIN_INTELLIGENCE:
		ABON(A_INT) += obj->spe;
		flags.botl = 1;
		if (obj->spe || objects[RIN_GAIN_INTELLIGENCE].oc_name_known) {
			makeknown(RIN_GAIN_INTELLIGENCE);
			obj->known = TRUE;
		}
		break;
	case RIN_GAIN_WISDOM:
		ABON(A_WIS) += obj->spe;
		flags.botl = 1;
		if (obj->spe || objects[RIN_GAIN_WISDOM].oc_name_known) {
			makeknown(RIN_GAIN_WISDOM);
			obj->known = TRUE;
		}
		break;
	case RIN_GAIN_DEXTERITY:
		ABON(A_DEX) += obj->spe;
		flags.botl = 1;
		if (obj->spe || objects[RIN_GAIN_DEXTERITY].oc_name_known) {
			makeknown(RIN_GAIN_DEXTERITY);
			obj->known = TRUE;
		}
		break;
	case RIN_INCREASE_ACCURACY:	/* KMH */
		u.uhitinc += obj->spe;
		break;
	case RIN_INCREASE_DAMAGE:
		u.udaminc += obj->spe;
		break;
	case RIN_HEAVY_ATTACK:
		u.uhitinc += obj->spe;
		u.udaminc += obj->spe;
		break;
	case RIN_PROTECTION_FROM_SHAPE_CHAN:
		rescham();
		break;
	case RIN_PROTECTION:
		if (obj->spe || objects[RIN_PROTECTION].oc_name_known) {
		    flags.botl = 1;
		    makeknown(RIN_PROTECTION);
		    obj->known = 1;
		    update_inventory();
		}
		break;
	case RIN_THREE_POINT_SEVEN_PROTECTI:
		if (obj->spe || objects[RIN_THREE_POINT_SEVEN_PROTECTI].oc_name_known) {
		    flags.botl = 1;
		    makeknown(RIN_THREE_POINT_SEVEN_PROTECTI);
		    obj->known = 1;
		    update_inventory();
		}
		break;
    }

    if (obj->oartifact == ART_RING_OF_WOE) {
		curse(obj);
		obj->hvycurse = obj->prmcurse = 1;
		pline("A terrible aura of darkness and eternal damnation surrounds your ring.");
    }

    if (obj->oartifact == ART_VERSION_CONTROL) {
		if (!obj->cursed) {
			curse(obj);
			pline("BEEEEP! The ring becomes cursed.");
		}

		if (flags.hybridevilvariant) {
			pline("Currently you are playing in evilvariant mode.");
			if (yn("Turn off evilvariant mode?") == 'y') {
				flags.hybridevilvariant = FALSE;
				flags.hybridization--;
				pline("You no longer have the evilvariant hybrid race.");
			}
		} else {
			pline("Currently you aren't playing in evilvariant mode.");
			if (yn("Turn on evilvariant mode?") == 'y') {
				flags.hybridevilvariant = TRUE;
				flags.hybridization++;
				pline("You now have the evilvariant hybrid race.");
			}
		}

		if (flags.hybridsoviet) {
			pline("Currently you are playing in soviet mode.");
			if (yn("Turn off soviet mode?") == 'y') {
				flags.hybridsoviet = FALSE;
				flags.hybridization--;
				pline("You no longer have the soviet hybrid race.");
			}
		} else {
			pline("Currently you aren't playing in soviet mode.");
			if (yn("Turn on soviet mode?") == 'y') {
				flags.hybridsoviet = TRUE;
				flags.hybridization++;
				pline("You now have the soviet hybrid race.");
			}
		}

    }

    if (obj->oartifact == ART_FIRE_NIGHT) {
		if (!tech_known(T_BEAUTY_CHARM) && u.ugold >= 10000) {
		    	learntech(T_BEAUTY_CHARM, FROMOUTSIDE, 1);
			u.ugold -= 10000;
		    	pline("You learn the beauty charm technique for 10000 zorkmids!");
		}

    }

    if (obj->oartifact == ART_ABSOLUTE_AUTOCURSE) {

		register struct obj *crsobj;
		pline("Your entire inventory becomes cursed.");
		for(crsobj = invent; crsobj ; crsobj = crsobj->nobj)
		if (!stack_too_big(crsobj))	curse(crsobj);
    }

    if (obj->oartifact == ART_ARABELLA_S_RESIST_COLD) {

		register struct obj *crsobj;
		pline("You hear a high-pitched sound followed by a short, slightly lower-pitched one...");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Eto ochen' plokho dlya vas, i ochen' smeshno dlya tipa bloka l'da. Ay privet privet privet!" : "Dueueue-due!");
		for(crsobj = invent; crsobj ; crsobj = crsobj->nobj)
		if (!stack_too_big(crsobj))	curse(crsobj);
    }

    if (obj->oartifact == ART_EAT_MORE_KITTENS) {
		if (!obj->cursed) {
			curse(obj);
			pline("You become like the antichrist, and in order to prevent you from clearing your name, the ring curses itself.");
		}
    }

    if (obj->oartifact == ART_CRYLOCK) {
		curse(obj);
		obj->hvycurse = 1;
		pline("Black tears drop from the ring...");
    }

    if (obj->oartifact == ART_ARABELLA_S_RADAR) {
		curse(obj);
		obj->hvycurse = 1;
    }

	if (obj && (AutocursingEquipment || u.uprops[AUTOCURSE_EQUIP].extrinsic || have_autocursestone())) curse(obj);

	if (obj && obj->spe > -10 && (TrashingBugEffect || u.uprops[TRASHING_EFFECT].extrinsic || have_trashstone())) obj->spe--;

}

STATIC_OVL void
Ring_off_or_gone(obj,gone)
register struct obj *obj;
boolean gone;
{
    long mask = (obj->owornmask & W_RING);
    int old_attrib, which;

    takeoff_mask &= ~mask;
    if(!(u.uprops[objects[obj->otyp].oc_oprop].extrinsic & mask))
	impossible("Strange... I didn't know you had that ring.");
    if(gone) setnotworn(obj);
    else setworn((struct obj *)0, obj->owornmask);

    switch(obj->otyp) {
	case RIN_TELEPORTATION:
	case RIN_REGENERATION:
	case RIN_SEARCHING:
	case RIN_STEALTH:
	case RIN_HUNGER:
	case RIN_AGGRAVATE_MONSTER:
	case RIN_POISON_RESISTANCE:
	case RIN_FIRE_RESISTANCE:
	case RIN_COLD_RESISTANCE:
	case RIN_FEAR_RESISTANCE:
	case RIN_SHOCK_RESISTANCE:
	case RIN_CONFLICT:
	case RIN_TELEPORT_CONTROL:
	case RIN_POLYMORPH:
	case RIN_POLYMORPH_CONTROL:
	/* KMH, balance patch -- now an amulet
	case RIN_DRAIN_RESISTANCE: */
	/* KMH, balance patch -- added */
	case RIN_MOOD:
	case RIN_FREE_ACTION:                
	case RIN_SLOW_DIGESTION:
	case RIN_SUSTAIN_ABILITY:
	case MEAT_RING:
		break;
	case RIN_LIGHT:
		vision_full_recalc = 1;
		break;
	case RIN_ILLNESS:
		if (Sick) You("are no longer ill.");
		if (Slimed) {
		    pline_The("slime disappears!");
		    Slimed = 0;
		 /* flags.botl = 1; -- healup() handles this */
		}
		healup(0, 0, TRUE, FALSE);
		break;
	case RIN_SLEEPING:
		if (!ESleeping && !(HSleeping & INTRINSIC) && !Race_if(PM_KOBOLT))
			HSleeping = 0;
		break;
#if 0
	case RIN_INDIGESTION:
		if (!EIndigestion)
			HIndigestion = 0;
		break;
#endif
	case RIN_TIMELY_BACKUP:
		You_feel("unsafe.");
		break;
	case RIN_WARNING:
		see_monsters();
		break;
	case RIN_SEE_INVISIBLE:
		/* Make invisible monsters go away */
		if (!See_invisible) {
		    set_mimic_blocking(); /* do special mimic handling */
		    see_monsters();
		    see_objects();
		}

		if (Invisible && !Blind) {
		    newsym(u.ux,u.uy);
		    pline("Suddenly you cannot see yourself.");
		    makeknown(RIN_SEE_INVISIBLE);
		}
		break;
	case RIN_INFRAVISION:
		see_monsters();

		break;
	case RIN_INVISIBILITY:
		if (!Invis && !BInvis && !Blind) {
		    newsym(u.ux,u.uy);
		    Your("body seems to unfade%s.",
			 See_invisible ? " completely" : "..");
		    makeknown(RIN_INVISIBILITY);
		}
		break;
	case RIN_LEVITATION:
		flags.botl = 1;
		(void) float_down(0L, 0L);
		if (!Levitation) makeknown(RIN_LEVITATION);
		break;
	case RIN_GAIN_STRENGTH:
		which = A_STR;
		goto adjust_attrib;
	case RIN_GAIN_INTELLIGENCE:
		which = A_INT;
		goto adjust_attrib;
	case RIN_GAIN_WISDOM:
		which = A_WIS;
		goto adjust_attrib;
	case RIN_GAIN_DEXTERITY:
		which = A_DEX;
		goto adjust_attrib;
	case RIN_GAIN_CONSTITUTION:
		which = A_CON;
		goto adjust_attrib;
	case RIN_ADORNMENT:
		which = A_CHA;
 adjust_attrib:
		old_attrib = ACURR(which);
		ABON(which) -= obj->spe;
		if (ACURR(which) != old_attrib) {
		    flags.botl = 1;
		    makeknown(obj->otyp);
		    obj->known = 1;
		    update_inventory();
		}
		break;
	case RIN_INCREASE_ACCURACY:	/* KMH */
		u.uhitinc -= obj->spe;
		break;
	case RIN_INCREASE_DAMAGE:
		u.udaminc -= obj->spe;
		break;
	case RIN_HEAVY_ATTACK:
		u.uhitinc -= obj->spe;
		u.udaminc -= obj->spe;
		break;
	case RIN_PROTECTION:
		/* might have forgotten it due to amnesia */
		if (obj->spe) {
		    flags.botl = 1;
		    makeknown(RIN_PROTECTION);
		    obj->known = 1;
		    update_inventory();
		}
	case RIN_THREE_POINT_SEVEN_PROTECTI:
		/* might have forgotten it due to amnesia */
		if (obj->spe) {
		    flags.botl = 1;
		    makeknown(RIN_THREE_POINT_SEVEN_PROTECTI);
		    obj->known = 1;
		    update_inventory();
		}
	case RIN_PROTECTION_FROM_SHAPE_CHAN:
		/* If you're no longer protected, let the chameleons
		 * change shape again -dgk
		 */
		restartcham();
		break;
    }
}

void
Ring_off(obj)
struct obj *obj;
{
	Ring_off_or_gone(obj,FALSE);
}

void
Ring_gone(obj)
struct obj *obj;
{
	Ring_off_or_gone(obj,TRUE);
}

void
Blindf_on(otmp)
register struct obj *otmp;
{
	long already_blind = Blind, changed = FALSE;

	if (otmp == uwep)
	    setuwep((struct obj *) 0, TRUE, TRUE);
	if (otmp == uswapwep)
	    setuswapwep((struct obj *) 0, TRUE);
	if (otmp == uquiver)
	    setuqwep((struct obj *) 0);
	setworn(otmp, W_TOOL);
	on_msg(otmp);

	if (uarmf && uarmf->otyp == BOOTS_OF_SHOCK_RESISTANCE && otmp && is_metallic(otmp)) {
		You("receive a little shock.");
		losehp(rnd(2), "little electric shock", KILLED_BY_AN);
	}

	if (otmp && otmp->oartifact == ART_BLINDFOLD_OF_MISPELLING) {
		if (!otmp->cursed) {
			curse(otmp);
			pline("Yur bldnflt crrrss tsefl!!!!!111");
		}
	}

	if (otmp && otmp->oartifact == ART_ANSWER_IS___) {
		badeffect();
	}

	if (Blind && !already_blind) {
	    changed = TRUE;
	    if (flags.verbose) You_cant("see any more.");
	    /* set ball&chain variables before the hero goes blind */
	    if (Punished) set_bc(0);
	} else if (already_blind && !Blind) {
	    changed = TRUE;
	    /* "You are now wearing the Eyes of the Overworld." */
	    You("can see!");
	}
	if (changed) {
	    /* blindness has just been toggled */
	    if (Blind_telepat || Infravision) see_monsters();
	    vision_full_recalc = 1;	/* recalc vision limits */
	    flags.botl = 1;
	}

	if (otmp && (AutocursingEquipment || u.uprops[AUTOCURSE_EQUIP].extrinsic || have_autocursestone())) curse(otmp);

	if (otmp && otmp->spe > -10 && (TrashingBugEffect || u.uprops[TRASHING_EFFECT].extrinsic || have_trashstone())) otmp->spe--;

}

void
Blindf_off(otmp)
register struct obj *otmp;
{
	boolean was_blind = Blind, changed = FALSE;

	takeoff_mask &= ~W_TOOL;
	setworn((struct obj *)0, otmp->owornmask);
	off_msg(otmp);

	if (Blind) {
	    if (was_blind) {
		/* "still cannot see" makes no sense when removing lenses
		   since they can't have been the cause of your blindness */
		if (otmp->otyp != LENSES && otmp->otyp != RADIOGLASSES && otmp->otyp != BOSS_VISOR)
		    You("still cannot see.");
	    } else {
		changed = TRUE;	/* !was_blind */
		/* "You were wearing the Eyes of the Overworld." */
		You_cant("see anything now!");
		/* set ball&chain variables before the hero goes blind */
		if (Punished) set_bc(0);
	    }
	} else if (was_blind) {
	    changed = TRUE;	/* !Blind */
	    You("can see again.");
	}
	if (changed) {
	    /* blindness has just been toggled */
	    if (Blind_telepat || Infravision) see_monsters();
	    vision_full_recalc = 1;	/* recalc vision limits */
	    flags.botl = 1;
	}
}


/* called in main to set intrinsics of worn start-up items */
void
set_wear()
{
	if (uarmu) (void) Shirt_on();
	if (uarm)  (void) Armor_on();
	if (uarmc) (void) Cloak_on();
	if (uarmf) (void) Boots_on();
	if (uarmg) (void) Gloves_on();
	if (uarmh) (void) Helmet_on();
	if (uarms) (void) Shield_on();
}

/* check whether the target object is currently being put on (or taken off) */
boolean
donning(otmp)		/* also checks for doffing */
register struct obj *otmp;
{
 /* long what = (occupation == take_off) ? taking_off : 0L; */
    long what = taking_off;	/* if nonzero, occupation is implied */
    boolean result = FALSE;

    if (otmp == uarm)
	result = (afternmv == Armor_on || afternmv == Armor_off ||
		  what == WORN_ARMOR);
    else if (otmp == uarmu)
	result = (afternmv == Shirt_on || afternmv == Shirt_off ||
		  what == WORN_SHIRT);
    else if (otmp == uarmc)
	result = (afternmv == Cloak_on || afternmv == Cloak_off ||
		  what == WORN_CLOAK);
    else if (otmp == uarmf)
	result = (afternmv == Boots_on || afternmv == Boots_off ||
		  what == WORN_BOOTS);
    else if (otmp == uarmh)
	result = (afternmv == Helmet_on || afternmv == Helmet_off ||
		  what == WORN_HELMET);
    else if (otmp == uarmg)
	result = (afternmv == Gloves_on || afternmv == Gloves_off ||
		  what == WORN_GLOVES);
    else if (otmp == uarms)
	result = (afternmv == Shield_on || afternmv == Shield_off ||
		  what == WORN_SHIELD);

    return result;
}

void
cancel_don()
{
	/* the piece of armor we were donning/doffing has vanished, so stop
	 * wasting time on it (and don't dereference it when donning would
	 * otherwise finish)
	 */
	cancelled_don = (afternmv == Boots_on || afternmv == Helmet_on ||
			 afternmv == Gloves_on || afternmv == Armor_on);
	afternmv = 0;
	nomovemsg = (char *)0;
	multi = 0;
	todelay = 0;
	taking_off = 0L;
}

static NEARDATA const char clothes[] = {ARMOR_CLASS, 0};
static NEARDATA const char accessories[] = {RING_CLASS, AMULET_CLASS, IMPLANT_CLASS, TOOL_CLASS, FOOD_CLASS, 0};

/* the 'T' command */
int
dotakeoff()
{
	register struct obj *otmp = (struct obj *)0;
	int armorpieces = 0;

	if (HardcoreAlienMode) {
		int i, j, bd = 1;
		struct monst *mtmp;
		for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) {
				if (canspotmon(mtmp)) {
					pline("Not now! They're looking!");
					if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
					return 0;
				} else {
					pline("Someone watched you change clothes...");
					if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
					u.ugangr++;
					adjalign(-50);
					change_luck(-1);
					prayer_done();
					return 1;

				}
			}
		}

	}

	boolean updowninversion = 0;
	if (uarmc && itemhasappearance(uarmc, APP_UP_DOWN_CLOAK)) updowninversion = 1;

#define MOREARM(x) if (x) { armorpieces++; otmp = x; }
	MOREARM(uarmh);
	MOREARM(uarms);
	MOREARM(uarmg);
	MOREARM(uarmf);
	if (uarmc) {
		armorpieces++;
		otmp = uarmc;
	} else if (uarm) {
		armorpieces++;
		otmp = uarm;
	} else if (uarmu) {
		armorpieces++;
		otmp = uarmu;
	}
	if (!armorpieces) {
	     /* assert( GRAY_DRAGON_SCALES > YELLOW_DRAGON_SCALE_MAIL ); */
		if (uskin)
		    pline_The("%s merged with your skin!",
			      uskin->otyp >= GRAY_DRAGON_SCALES ?
				"dragon scales are" : "dragon scale mail is");
		else
		    pline("Not wearing any armor.%s", (iflags.cmdassist && 
				(uleft || uright || uamul || uimplant || ublindf)) ?
			  "  Use 'R' command to remove accessories." : "");
		return 0;
	}
	if (armorpieces > 0)
		otmp = getobj(clothes, "take off");
	if (otmp == 0) return(0);
	if (!(otmp->owornmask & W_ARMOR)) {
		You("are not wearing that.");
		return(0);
	}
	/* note: the `uskin' case shouldn't be able to happen here; dragons
	   can't wear any armor so will end up with `armorpieces == 0' above */
	if (otmp == uskin || ((otmp == uarm) && uarmc && !updowninversion) || ((otmp == uarmc) && uarm && updowninversion) 
			  || ((otmp == uarmu) && (uarmc || uarm))
		) {
	    You_cant("take that off.%s", (otmp == uskin) ? "" : " It appears that the item is covered by another piece of equipment.");
	    return 0;
	}

	reset_remarm();		/* clear takeoff_mask and taking_off */
	(void) select_off(otmp);
	if (!takeoff_mask) return 0;
	reset_remarm();		/* armoroff() doesn't use takeoff_mask */

	(void) armoroff(otmp);
	return(1);
}

/* the 'R' command */
int
doremring()
{
	register struct obj *otmp = 0;
	int Accessories = 0;

#define MOREACC(x) if (x) { Accessories++; otmp = x; }
	MOREACC(uleft);
	MOREACC(uright);
	MOREACC(uamul);
	MOREACC(uimplant);
	MOREACC(ublindf);

	if(!Accessories) {
		pline("Not wearing any accessories.%s", (iflags.cmdassist &&
			    (uarm || uarmc ||
			     uarmu ||
			     uarms || uarmh || uarmg || uarmf)) ?
		      "  Use 'T' command to take off armor." : "");
		return(0);
	}
	if (Accessories > 0) otmp = getobj(accessories, "remove");
	if(!otmp) return(0);
	if(!(otmp->owornmask & (W_RING | W_AMUL | W_IMPLANT | W_TOOL))) {
		You("are not wearing that.");
		return(0);
	}

	reset_remarm();		/* clear takeoff_mask and taking_off */
	(void) select_off(otmp);
	if (!takeoff_mask) return 0;
	reset_remarm();		/* not used by Ring_/Amulet_/Blindf_off() */

	if (otmp == uright || otmp == uleft) {
		/* Sometimes we want to give the off_msg before removing and
		 * sometimes after; for instance, "you were wearing a moonstone
		 * ring (on right hand)" is desired but "you were wearing a
		 * square amulet (being worn)" is not because of the redundant
		 * "being worn".
		 */
		off_msg(otmp);
		Ring_off(otmp);
	} else if (otmp == uamul) {
		Amulet_off();
		off_msg(otmp);
	} else if (otmp == uimplant) {
		Implant_off();
		off_msg(otmp);
	} else if (otmp == ublindf) {
		Blindf_off(otmp);	/* does its own off_msg */
	} else {
		impossible("removing strange accessory?");
	}
	return(1);
}

/* Check if something worn is cursed _and_ unremovable. */
int
cursed(otmp)
register struct obj *otmp;
{
	/* Curses, like chickens, come home to roost. */
	if((otmp == uwep) ? welded(otmp) : (int)otmp->cursed) {
		Hallucination ? pline("You can't. It's cursed.") : You("can't.  %s cursed.",
			(is_boots(otmp) || is_gloves(otmp) || otmp->quan > 1L)
			? "They are" : "It is");
		otmp->bknown = TRUE;
		return(1);
	}
	return(0);
}

int
armoroff(otmp)
register struct obj *otmp;
{
	register int delay = -objects[otmp->otyp].oc_delay;
	if (HardcoreAlienMode) delay--;
	
	if(cursed(otmp)) return(0);
	if(delay) {
		nomul(delay, "disrobing", TRUE);
		if (is_helmet(otmp)) {
			nomovemsg = "You finish taking off your helmet.";
			afternmv = Helmet_off;
		     }
		else if (is_gloves(otmp)) {
			nomovemsg = "You finish taking off your gloves.";
			afternmv = Gloves_off;
		     }
		else if (is_boots(otmp)) {
			nomovemsg = "You finish taking off your boots.";
			afternmv = Boots_off;
		     }
		else if (otmp->otyp == VICTORIAN_UNDERWEAR) {
			nomovemsg = "You finish taking off your impractical underwear.";
			afternmv = Shirt_off;
		     }
		else if (is_shirt(otmp)) {
			nomovemsg = "You finish taking off your shirt.";
			afternmv = Shirt_off;
		     }
		else if (is_cloak(otmp)) {
			nomovemsg = "You finish taking off your cloak.";
			afternmv = Cloak_off;
		     }
		else if (is_shield(otmp)) {
			nomovemsg = "You finish taking off your shield.";
			afternmv = Shield_off;
		     }
		else {
			nomovemsg = "You finish taking off your suit.";
			afternmv = Armor_off;
		}
	} else {
		/* Be warned!  We want off_msg after removing the item to
		 * avoid "You were wearing ____ (being worn)."  However, an
		 * item which grants fire resistance might cause some trouble
		 * if removed in Hell and lifesaving puts it back on; in this
		 * case the message will be printed at the wrong time (after
		 * the messages saying you died and were lifesaved).  Luckily,
		 * no cloak, shield, or fast-removable armor grants fire
		 * resistance, so we can safely do the off_msg afterwards.
		 * Rings do grant fire resistance, but for rings we want the
		 * off_msg before removal anyway so there's no problem.  Take
		 * care in adding armors granting fire resistance; this code
		 * might need modification.
		 * 3.2 (actually 3.1 even): this comment is obsolete since
		 * fire resistance is not needed for Gehennom.
		 */
		if(is_cloak(otmp))
			(void) Cloak_off();
		else if(is_shield(otmp))
			(void) Shield_off();
		else if(is_boots(otmp))
			(void) Boots_off();
		else if(is_shirt(otmp))
			(void) Shirt_off();
		else if(is_gloves(otmp)) /* good fortune */
			(void) Gloves_off();
		else if (is_helmet(otmp))
			/* [Finn E. Theodorsen] For fedoras */
			(void) Helmet_off();
		else setworn((struct obj *)0, otmp->owornmask & W_ARMOR);
		off_msg(otmp);
	}
	takeoff_mask = taking_off = 0L;
	return(1);
}

STATIC_OVL void
already_wearing(cc)
const char *cc;
{
	You("are already wearing %s%c", cc, (cc == c_that_) ? '!' : '.');
}

STATIC_OVL void
already_wearing2(cc1, cc2)
const char *cc1, *cc2;
{
	You_cant("wear %s because you're wearing %s there already.", cc1, cc2);
}

/*
 * canwearobj checks to see whether the player can wear a piece of armor
 *
 * inputs: otmp (the piece of armor)
 *         noisy (if TRUE give error messages, otherwise be quiet about it)
 * output: mask (otmp's armor type)
 */
int
canwearobj(otmp,mask,noisy)
struct obj *otmp;
long *mask;
boolean noisy;
{
    int err = 0;
    const char *which;

    which = is_cloak(otmp) ? c_cloak :
	    is_shirt(otmp) ? c_shirt :
	    is_suit(otmp) ? c_suit : 0;

	if (Race_if(PM_OCTOPODE) && (is_boots(otmp) || is_gloves(otmp) || is_shield(otmp) || is_cloak(otmp) || is_helmet(otmp) || is_suit(otmp) || is_shirt(otmp))) {
		if (noisy) pline("Octopodes are completely unable to wear any armor pieces.");
		return 0;
	}

	if (Race_if(PM_ETHEREALOID) && !Upolyd && (is_boots(otmp) || is_gloves(otmp) || is_shield(otmp) || is_cloak(otmp) || is_helmet(otmp) || is_suit(otmp) || is_shirt(otmp))) {
		if (noisy) pline("Too bad! As an etherealoid, you cannot wear armor.");
		/* but can still polymorph, put on something, and then poly back and still be wearing the item --Amy */
		return 0;
	}

	if (Race_if(PM_INCORPOREALOID) && !Upolyd && (is_boots(otmp) || is_gloves(otmp) || is_shield(otmp) || is_cloak(otmp) || is_helmet(otmp) || is_suit(otmp) || is_shirt(otmp))) {
		if (noisy) pline("Too bad! As an incorporealoid, you cannot wear armor.");
		/* and since you cannot polymorph, well... --Amy */
		return 0;
	}

	if (!is_cloak(otmp) && !is_boots(otmp) && !(otmp->oartifact == ART_ELONA_S_SNAIL_TRAIL) && Race_if(PM_ELONA_SNAIL) && !Upolyd) {
		if (noisy) pline("As a snail, the only types of armor you can wear are cloaks and boots.");
		return 0;
	}

	if (!is_suit(otmp) && !is_boots(otmp) && Race_if(PM_WEAPONIZED_DINOSAUR) && !Upolyd) {
		if (noisy) pline("As a dinosaur, the only types of armor you can wear are suits and boots.");
		return 0;
	}

	if (is_suit(otmp) && uamul && uamul->oartifact == ART_DIKKIN_S_DRAGON_TEETH) {
		if (noisy) pline("Your wings prevent you from wearing body armor.");
		return 0;
	}

	if (is_boots(otmp) && Race_if(PM_FAWN) && !Upolyd) {
		if (noisy) pline("Fawns cannot wear boots!");
		return 0;
	}
	if (is_boots(otmp) && Race_if(PM_SATRE) && !Upolyd) {
		if (noisy) pline("Satres cannot wear boots!");
		return 0;
	}
	if (is_cloak(otmp) && Race_if(PM_SATRE) && !Upolyd) {
		if (noisy) pline("Satres cannot wear cloaks!");
		return 0;
	}
	if (is_shirt(otmp) && Race_if(PM_SATRE) && !Upolyd) {
		if (noisy) pline("Satres cannot wear shirts!");
		return 0;
	}
	if (is_suit(otmp) && Race_if(PM_SATRE) && !Upolyd) {
		if (noisy) pline("Satres cannot wear body armor!");
		return 0;
	}
	if (is_suit(otmp) && Race_if(PM_WEAPON_XORN) && !Upolyd) {
		if (noisy) pline("Xorns are too big to wear body armor!");
		return 0;
	}

	if (is_boots(otmp) && noisy && Race_if(PM_ELONA_SNAIL)) {
		pline("Snails are completely unable to move if they wear boots that aren't high-heeled. If you want to wear these, you should be sure that they are high-heeled because otherwise you're screwed!");
		if (yn("Are you sure you want to put them on?") != 'y') return 0;
	}

	if (is_boots(otmp) && noisy && Race_if(PM_WEAPONIZED_DINOSAUR)) {
		if (yn("As a dinosaur, wearing non-high-heeled boots slows you down to half speed, so you should be sure that the pair you're trying to put on is high-heeled. Do you want to wear them?") != 'y') return 0;
	}

	if (is_boots(otmp) && noisy && Race_if(PM_DUFFLEPUD)) {
		if (yn("The uncommon size of your dufflepud feet means that wearing boots of any kind will be awkward, causing you to move at half speed. Really wear them?") != 'y') return 0;
	}

    if (which && (cantweararm(youmonst.data) || (Race_if(PM_CHIROPTERAN) && !Upolyd) || (Race_if(PM_PLAYER_MUSHROOM) && !Upolyd) ) && !Race_if(PM_TRANSFORMER)  && (otmp->otyp != OSFA_CHAIN_MAIL) &&
	    /* same exception for cloaks as used in m_dowear() */
	    (which != c_cloak || youmonst.data->msize != MZ_SMALL) &&
	    (racial_exception(&youmonst, otmp) < 1)) {
	if (noisy) { pline_The("%s will not fit on your body.", which);

		if (yn("Try to put it on anyway?") == 'y') {
			if (rn2(2) && !polyskillchance()) {
			 	u.ublesscnt += rnz(5);
				pline("Feeling uncomfortable, you decide to stop trying.");
				TimerunBug += 1; /* ugh, ugly hack. But nomul() doesn't want to work... --Amy */
				if (!rn2(20)) badeffect();
				return 0;
				}
			}

		else {return(0);}

		}


    } else if (otmp->owornmask & W_ARMOR) {
	if (noisy) already_wearing(c_that_);
	return 0;
    }

    if (welded(uwep) && bimanual(uwep) &&
	    (is_suit(otmp)
			|| is_shirt(otmp)
	    )) {
	if (noisy)
	    You("cannot do that while holding your %s.",
		is_sword(uwep) ? c_sword : c_weapon);
	return 0;
    }

    if (is_helmet(otmp)) {
	if (uarmh) {
	    if (noisy) already_wearing(an(c_helmet));
	    err++;
	} else if (Upolyd && has_horns(youmonst.data) && !Race_if(PM_TRANSFORMER) && !is_flimsy(otmp)) {
	    /* (flimsy exception matches polyself handling) */
	    if (noisy)
		pline_The("%s won't fit over your horn%s.",
			  c_helmet, plur(num_horns(youmonst.data)));
	    err++;
	} else if (Role_if(PM_COURIER)) {
	    if (noisy)
		pline("You have no head!");
	    err++;
	/* Actually, you got shot in the head by Benny, so you aren't technically without a head. But for the sake of it,
	   let's pretend the courier actually doesn't have one. By the way, Benny is the "some random guy" quest nemesis
	   in the courier quest, since all Fallout New Vegas NPCs are undistinguishable clones of each other. --Amy */

	} else if (Race_if(PM_ILLITHID) || Race_if(PM_ANCIPITAL)) {
	    if (noisy)
		pline("Your tentacles prevent that action!");
	    err++;

	/* mind flayers cannot wear helmets at all --Amy */
	} else if (Upolyd && (is_mind_flayer(youmonst.data))) {
		if (noisy)
			pline_The("%s won't fit over your tentacles.", xname(otmp));
		err++;
	} else
	    *mask = W_ARMH;
    } else if (is_shield(otmp)) {
	if (uarms) {
	    if (noisy) already_wearing(an(c_shield));
	    err++;
	} else if (uwep && bimanual(uwep) && otmp->otyp != GRIM_SHIELD) {
	    if (noisy) 
		You("cannot wear a shield while wielding a two-handed %s.",
		    is_sword(uwep) ? c_sword :
		    (uwep->otyp == BATTLE_AXE) ? c_axe : (uwep->otyp == DWARVISH_BATTLE_AXE) ? c_axe : c_weapon);
	    err++;
	} else if (u.twoweap && otmp->otyp != GRIM_SHIELD) {
	    if (noisy) {
		if (uwep && uswapwep)
		    You("cannot wear a shield while wielding two weapons.");
		else
		    You("cannot wear a shield while fighting with two %s.",
			    makeplural(body_part(HAND)));
	    }
	    err++;
	} else
	    *mask = W_ARMS;
    } else if (is_boots(otmp)) {
	if (uarmf) {
	    if (noisy) already_wearing(c_boots);
	    err++;
	} else if (Upolyd && slithy(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
	    if (noisy) You("have no feet...");	/* not body_part(FOOT) */
	    err++;
	} else if (Upolyd && youmonst.data->mlet == S_CENTAUR) {
	    /* break_armor() pushes boots off for centaurs,
	       so don't let dowear() put them back on... */
	    if (noisy) pline("You have too many hooves to wear %s.",
			     c_boots);	/* makeplural(body_part(FOOT)) yields
					   "rear hooves" which sounds odd */
	    err++;
	} else if (u.utrap && (u.utraptype == TT_BEARTRAP ||
				u.utraptype == TT_INFLOOR)) {
	    if (u.utraptype == TT_BEARTRAP) {
		if (noisy) Your("%s is trapped!", body_part(FOOT));
	    } else {
		if (noisy) Your("%s are stuck in the %s!",
				makeplural(body_part(FOOT)),
				surface(u.ux, u.uy));
	    }
	    err++;
	} else
	    *mask = W_ARMF;
    } else if (is_gloves(otmp)) {
	if (uarmg) {
	    if (noisy) already_wearing(c_gloves);
	    err++;
	} else if (welded(uwep) && issoviet) {
	    if (noisy) You("cannot wear gloves over your %s.",
			   is_sword(uwep) ? c_sword : c_weapon);
	    err++;
	} else
	    *mask = W_ARMG;
    } else if (is_shirt(otmp)) {
	if (uarm || uarmc || uarmu) {
	    if (uarmu) {
		if (noisy) already_wearing(an(c_shirt));
		err++;
	    } else if (issoviet) {
		if (noisy) You_cant("wear that over your %s.",
			           (uarm && !uarmc) ? c_armor : cloak_simple_name(uarmc));
		err++;
	    } else *mask = W_ARMU;
	} else
	    *mask = W_ARMU;
    } else if (is_cloak(otmp)) {
	if (uarmc) {
	    if (noisy) already_wearing(an(cloak_simple_name(uarmc)));
	    err++;
	} else
	    *mask = W_ARMC;
    } else if (is_suit(otmp)) {
	if (uarmc && issoviet) {
	    if (noisy) You("cannot wear armor over a %s.", cloak_simple_name(uarmc));
	    err++;
	} else if (uarm) {
	    if (noisy) already_wearing("some armor");
	    err++;
	} else
	    *mask = W_ARM;
    } else {
	/* getobj can't do this after setting its allow_all flag; that
	   happens if you have armor for slots that are covered up or
	   extra armor for slots that are filled */
	if (noisy) silly_thing("wear", otmp);
	err++;
    }
/* Unnecessary since now only weapons and special items like pick-axes get
 * welded to your hand, not armor
    if (welded(otmp)) {
	if (!err++) {
	    if (noisy) weldmsg(otmp);
	}
    }
 */
    return !err;
}

/* the 'W' command */
int
dowear()
{
	struct obj *otmp;
	int delay;
	long mask = 0;

	if (HardcoreAlienMode) {
		int i, j, bd = 1;
		struct monst *mtmp;
		for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) {
				if (canspotmon(mtmp)) {
					pline("Not now! They're looking!");
					if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
					return 0;
				} else {
					pline("Someone watched you change clothes...");
					if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
					u.ugangr++;
					adjalign(-50);
					change_luck(-1);
					prayer_done();
					return 1;

				}
			}
		}

	}

	/* cantweararm checks for suits of armor */
	/* verysmall or nohands checks for shields, gloves, etc... */
	if (!Race_if(PM_TRANSFORMER) && !Race_if(PM_HUMAN_WRAITH) && (verysmall(youmonst.data) || nohands(youmonst.data))) {
		pline("Don't even bother. Your current form can't realistically wear armor!");

		if (yn("But you may try to wear something anyway. Do it?") == 'y') {
			if (rn2(3) && !polyskillchance()) {
		 		make_stunned(HStun + rnd(40),FALSE);
				pline("Damn! You just stagger around aimlessly!");
				if (!rn2(20)) badeffect();
				return 1;
			}
		}
		else {return(0);}

	}

	if (Race_if(PM_HUMAN_WRAITH) && (u.uhpmax < 2 || u.uhp < 2) ) {pline("You don't have enough health to wear armor!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}

	if (Race_if(PM_HUMAN_WRAITH)) {

		if (yn("Putting on armor as a wraith will permanently damage your health. Do it anyway?") == 'y') {

			u.uhp -= 1;
			u.uhpmax -= 1;
			if (Upolyd) {
				u.mh -= 1;
			u.mhmax -= 1;
			}

		}
		else return 0;

	}

	otmp = getobj(clothes, "wear");
	if(!otmp) return(0);

	if (!canwearobj(otmp,&mask,TRUE)) return(0);

	if (otmp->oartifact && !touch_artifact(otmp, &youmonst))
	    return 1;	/* costs a turn even though it didn't get worn */

	if (otmp->otyp == HELM_OF_OPPOSITE_ALIGNMENT &&
			qstart_level.dnum == u.uz.dnum) {	/* in quest */
		if (u.ualignbase[A_CURRENT] == u.ualignbase[A_ORIGINAL])
			You("narrowly avoid losing all chance at your goal.");
		else	/* converted */
			You("are suddenly overcome with shame and change your mind.");
		u.ublessed = 0; /* lose your god's protection */
		makeknown(otmp->otyp);
		flags.botl = 1;
		return 1;
	}

	otmp->known = TRUE;
	if(otmp == uwep)
		setuwep((struct obj *)0, TRUE, TRUE);
	if (otmp == uswapwep)
		setuswapwep((struct obj *) 0, TRUE);
	if (otmp == uquiver)
		setuqwep((struct obj *) 0);
	setworn(otmp, mask);
	delay = -objects[otmp->otyp].oc_delay;
	if (HardcoreAlienMode) delay--;
	if(delay){
		nomul(delay, "dressing up", TRUE);
		if(is_boots(otmp)) afternmv = Boots_on;
		if(is_helmet(otmp)) afternmv = Helmet_on;
		if(is_gloves(otmp)) afternmv = Gloves_on;
		if(otmp == uarm) afternmv = Armor_on;
		if(is_cloak(otmp)) afternmv = Cloak_on;
		if (is_shield(otmp)) afternmv = Shield_on;
		if (is_shirt(otmp)) afternmv = Shirt_on;
		nomovemsg = "You finish your dressing maneuver.";
		on_msgdel(otmp); /* the game is supposed to tell you what exactly you are wearing! --Amy */
	} else {
		if(is_cloak(otmp)) (void) Cloak_on();
		if(is_boots(otmp)) (void) Boots_on();
		if(is_gloves(otmp)) (void) Gloves_on();
		if(otmp == uarm) (void) Armor_on();
		if (is_shield(otmp)) (void) Shield_on();
		if (is_shirt(otmp)) (void) Shirt_on();
		if (is_helmet(otmp)) (void) Helmet_on();	/* fedoras */
		on_msg(otmp);
	}
	takeoff_mask = taking_off = 0L;
	return(1);
}

int
doputon()
{
	register struct obj *otmp;
	long mask = 0L;

	otmp = getobj(accessories, "put on");
	if(!otmp) return(0);
	if(otmp->owornmask & (W_RING | W_AMUL | W_IMPLANT | W_TOOL)) {
		already_wearing(c_that_);
		return(0);
	}
	if(welded(otmp)) {
		weldmsg(otmp);
		return(0);
	}
	if(otmp == uwep)
		setuwep((struct obj *)0, TRUE, TRUE);
	if(otmp == uswapwep)
		setuswapwep((struct obj *) 0, TRUE);
	if(otmp == uquiver)
		setuqwep((struct obj *) 0);
	if(otmp->oclass == RING_CLASS || otmp->otyp == MEAT_RING) {
		if(nolimbs(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
			You("cannot make the ring stick to your body.");

			if (yn("Try to put it on anyway?") == 'y') {
				if (rn2(3) && !polyskillchance()) {
					make_dimmed(HDimmed + rnd(40),FALSE);
					pline("The only thing that happens is that you got dimmed...");
					if (!rn2(20)) badeffect();
					return(1);
				}
			} else return(0);
		}
		if(uleft && uright){
			There("are no more %s%s to fill.",
				humanoid(youmonst.data) ? "ring-" : "",
				makeplural(body_part(FINGER)));
			return(0);
		}
		if(uleft) mask = RIGHT_RING;
		else if(uright) mask = LEFT_RING;
		else do {
			char qbuf[QBUFSZ];
			char answer;

			sprintf(qbuf, "Which %s%s, Right or Left?",
				humanoid(youmonst.data) ? "ring-" : "",
				body_part(FINGER));
			if(!(answer = yn_function(qbuf, "rl", '\0')))
				return(0);
			switch(answer){
			case 'l':
			case 'L':
				mask = LEFT_RING;
				break;
			case 'r':
			case 'R':
				mask = RIGHT_RING;
				break;
			}
		} while(!mask);
		if (uarmg && !FingerlessGloves && uarmg->cursed && issoviet) {
			uarmg->bknown = TRUE;
		    You("cannot remove your gloves to put on the ring.");
			return(0);
		}
		if (welded(uwep) && bimanual(uwep) && issoviet) {
			/* welded will set bknown */
	    You("cannot free your weapon hands to put on the ring.");
			return(0);
		}
		if (welded(uwep) && mask==RIGHT_RING && issoviet) {
			/* welded will set bknown */
	    You("cannot free your weapon hand to put on the ring.");
			return(0);
		}
		if (otmp->oartifact && !touch_artifact(otmp, &youmonst))
		    return 1; /* costs a turn even though it didn't get worn */
		setworn(otmp, mask);
		Ring_on(otmp);
	} else if (otmp->oclass == AMULET_CLASS) {
		if(uamul) {
			already_wearing("an amulet");
			return(0);
		}
		if (otmp->oartifact && !touch_artifact(otmp, &youmonst))
		    return 1;
		setworn(otmp, W_AMUL);
		if (otmp->otyp == AMULET_OF_CHANGE) {
			Amulet_on();
			/* Don't do a prinv() since the amulet is now gone */
			return(1);
		}
		Amulet_on();
	} else if (otmp->oclass == IMPLANT_CLASS) {
		if(uimplant) {
			already_wearing("an implant");
			return(0);
		}
		if (otmp->oartifact && !touch_artifact(otmp, &youmonst))
		    return 1;
		setworn(otmp, W_IMPLANT);
		Implant_on();
	} else {	/* it's a blindfold, towel, or lenses */
		if (ublindf) {
			if (ublindf->otyp == TOWEL)
				Your("%s is already covered by a towel.",
					body_part(FACE));
			else if (ublindf->otyp == CONDOME)
					already_wearing("a condome");
			else if (ublindf->otyp == SOFT_CHASTITY_BELT)
					already_wearing("a condome");
			else if (ublindf->otyp == CLIMBING_SET)
					already_wearing("a climbing set");
			else if (ublindf->otyp == BLINDFOLD || ublindf->otyp == EYECLOSER || ublindf->otyp == DRAGON_EYEPATCH) {
				if (otmp->otyp == LENSES || otmp->otyp == RADIOGLASSES || otmp->otyp == BOSS_VISOR)
					already_wearing2("lenses", "a blindfold");
				else
					already_wearing("a blindfold");
			} else if (ublindf->otyp == LENSES || ublindf->otyp == RADIOGLASSES || ublindf->otyp == BOSS_VISOR) {
				if (otmp->otyp == BLINDFOLD || otmp->otyp == EYECLOSER || otmp->otyp == DRAGON_EYEPATCH)
					already_wearing2("a blindfold", "some lenses");
				else
					already_wearing("some lenses");
			} else
				already_wearing(something); /* ??? */
			return(0);
		}
		if (!(is_blindfold_slot(otmp))) {
			You_cant("wear that!");
			return(0);
		}
		if (otmp->oartifact && !touch_artifact(otmp, &youmonst))
		    return 1;
		if (uarmh && (uarmh->otyp == PLASTEEL_HELM || uarmh->otyp == HELM_OF_STORMS || uarmh->otyp == HELM_OF_DETECT_MONSTERS) ){
			pline("The %s covers your whole face. You need to remove it first.", xname(uarmh));
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return 1;
		}
		Blindf_on(otmp);
		return(1);
	}
	if (is_worn(otmp))
	    prinv((char *)0, otmp, 0L);
	return(1);
}

#endif /* OVLB */

#ifdef OVL0

/* Limits of uac (conveniently equal to the limits of an schar ;) */
#define UAC_MIN /*(-128)*/(-9999999)
#define UAC_LIM /*127*/9999999
/* Amy edit: why the hell is that an schar, I decided that limitations are stupid and should be removed */

int
armorbonusvalue(obj)
struct obj *obj;
{
	int armoringvalue = 0;

	if (!obj) return 0;

	armoringvalue = objects[(obj)->otyp].a_ac;

	if (!InvertedState) {
		armoringvalue += ( (obj)->spe - min((int)greatest_erosionX(obj),objects[(obj)->otyp].a_ac) );
	} else {
		armoringvalue -= abs(obj->spe);
		armoringvalue -= min((int)greatest_erosionX(obj),objects[(obj)->otyp].a_ac);
	}

	return armoringvalue;

}

void
find_ac()
{
	int uac = mons[u.umonnum].ac;

	if(uarm) uac -= ARM_BONUS(uarm);
	if(uarmc) uac -= ARM_BONUS(uarmc);
	if(uarmh) uac -= ARM_BONUS(uarmh);
	if(uarmf) uac -= ARM_BONUS(uarmf);
	if(uarms) uac -= ARM_BONUS(uarms);
	if(uarmg) uac -= ARM_BONUS(uarmg);
	if(uarmu) uac -= ARM_BONUS(uarmu);

	/* implants are mainly meant for those who lack hands --Amy */
	if(uimplant) uac -= ( (powerfulimplants() || ARM_BONUS(uimplant) < 1) ? ARM_BONUS_IMPLANT(uimplant) : (ARM_BONUS_IMPLANT(uimplant) / 2));

	if (uimplant && powerfulimplants() && uimplant->oartifact == ART_DEINE_MUDDA && uimplant->spe > 0) uac -= (uimplant->spe * 4);

	/* are you restricted? if yes, the implant may be actively bad for you */
	if (uimplant && P_RESTRICTED(P_IMPLANTS) && !powerfulimplants()) uac += 2;

	if(uleft && uleft->otyp == RIN_PROTECTION) uac -= uleft->spe;
	if(uright && uright->otyp == RIN_PROTECTION) uac -= uright->spe;
	if(uleft && uleft->otyp == RIN_THREE_POINT_SEVEN_PROTECTI) uac -= uleft->spe;
	if(uright && uright->otyp == RIN_THREE_POINT_SEVEN_PROTECTI) uac -= uright->spe;
	if (HProtection & INTRINSIC) uac -= u.ublessed;
	uac -= u.uspellprot;

/* STEPHEN WHITE'S NEW CODE */
	/* Dexterity now affects AC */
	if (ACURR(A_DEX) < 4) uac += 3;
	else if (ACURR(A_DEX) < 6) uac += 2;
	else if (ACURR(A_DEX) < 8) uac += 1;
	else if (ACURR(A_DEX) < 11) uac -= 0;
	else if (ACURR(A_DEX) < 13) uac -= 1;
	else if (ACURR(A_DEX) < 14) uac -= 2;
	else if (ACURR(A_DEX) < 16) uac -= 3;
	else if (ACURR(A_DEX) < 17) uac -= 4;
	else if (ACURR(A_DEX) < 19) uac -= 5;
	else if (ACURR(A_DEX) < 20) uac -= 6;
	else if (ACURR(A_DEX) < 21) uac -= 7;
	else if (ACURR(A_DEX) < 22) uac -= 8;
	else if (ACURR(A_DEX) < 24) uac -= 9;
	else uac -= 10;

	if (Role_if(PM_MONK) && !uwep && (!uarm ||
		(uarm->otyp >= ROBE && uarm->otyp <= ROBE_OF_WEAKNESS)) && !uarms) {
/*WAC cap off the Monk's ac bonus to -11 */
            if (u.ulevel > 18) uac -= 11;
            else uac -= (u.ulevel / 2) + 2;
	}
	if (Race_if(PM_DOPPELGANGER) && !uarm) uac -= (u.ulevel / 4) + 1;
	if (Role_if(PM_SHAPESHIFTER) && !uarm) uac -= (u.ulevel / 4) + 1;
	if (Race_if(PM_HAXOR)) uac -= 2;
	if ((Race_if(PM_HUMAN_WEREWOLF) || Race_if(PM_AK_THIEF_IS_DEAD_) || Role_if(PM_LUNATIC)) && !uarm) uac -= (u.ulevel / 4) + 1;

	if (Race_if(PM_HUMAN_WRAITH)) uac -= u.ulevel;
	if (Race_if(PM_ETHEREALOID)) uac -= u.ulevel;
	if (Race_if(PM_INCORPOREALOID)) uac -= u.ulevel;
	if (Race_if(PM_TURTLE)) uac -= u.ulevel;
	if (Race_if(PM_LOWER_ENT)) uac -= u.ulevel;
	if (Race_if(PM_KUTAR)) uac -= (u.ulevel / 3);

	/* Harlow - make sure it doesn't wrap around ;) */
	uac = (uac < UAC_MIN ? UAC_MIN : (uac > UAC_LIM ? UAC_LIM : uac));
	
	/*Corsets suck*/
	if(uarmu && uarmu->otyp == VICTORIAN_UNDERWEAR){
		uac += 2; //flat penalty. Something in the code "corrects" ac values >10, this is a kludge.
	}

	if (u.menoraget && u.bookofthedeadget && u.silverbellget) uac -= 1;
	if (u.chaoskeyget && u.neutralkeyget && u.lawfulkeyget) uac -= 1;
	if (u.medusaremoved && u.luckstoneget && u.sokobanfinished) uac -= 1;

	/* bonus for wearing racial armor */

	if ((Race_if(PM_DWARF) || Role_if(PM_MIDGET)) && uarm && uarm->otyp == DWARVISH_MITHRIL_COAT) uac -= 1;
	if ((Race_if(PM_DROW) || Role_if(PM_TWELPH)) && uarm && uarm->otyp == DARK_ELVEN_MITHRIL_COAT) uac -= 1;
	if ((Race_if(PM_ELF) || Race_if(PM_PLAYER_MYRKALFR) || Role_if(PM_ELPH)) && uarm && uarm->otyp == ELVEN_MITHRIL_COAT) uac -= 1;
	if ((Race_if(PM_ELF) || Race_if(PM_PLAYER_MYRKALFR) || Role_if(PM_ELPH)) && uarm && uarm->otyp == ELVEN_TOGA) uac -= 1;
	if ((Race_if(PM_GNOME) || Role_if(PM_GOLDMINER)) && uarm && uarm->otyp == GNOMISH_SUIT) uac -= 2;
	if (Race_if(PM_ORC) && uarm && (uarm->otyp == ORCISH_CHAIN_MAIL || uarm->otyp == ORCISH_RING_MAIL) ) uac -= 1;

	if (Race_if(PM_ORC) && uarmc && uarmc->otyp == ORCISH_CLOAK ) uac -= 1;
	if ((Race_if(PM_DWARF) || Role_if(PM_MIDGET)) && uarmc && uarmc->otyp == DWARVISH_CLOAK ) uac -= 1;
	if ((Race_if(PM_ELF) || Race_if(PM_PLAYER_MYRKALFR) || Role_if(PM_ELPH)) && uarmc && uarmc->otyp == ELVEN_CLOAK) uac -= 1;
	if ((Race_if(PM_ELF) || Race_if(PM_PLAYER_MYRKALFR) || Role_if(PM_ELPH)) && uarmg && uarmg->otyp == ELVEN_GAUNTLETS) uac -= 1;

	if (Race_if(PM_INKA) && uarmg && objects[(uarmg)->otyp].oc_material == MT_INKA) uac -= 6;
	if (Race_if(PM_INKA) && uarmc && objects[(uarmc)->otyp].oc_material == MT_INKA) uac -= 6;
	if (Race_if(PM_INKA) && uarmu && objects[(uarmu)->otyp].oc_material == MT_INKA) uac -= 6;
	if (Race_if(PM_INKA) && uarm && objects[(uarm)->otyp].oc_material == MT_INKA) uac -= 6;
	if (Race_if(PM_INKA) && uarms && objects[(uarms)->otyp].oc_material == MT_INKA) uac -= 6;
	if (Race_if(PM_INKA) && uarmh && objects[(uarmh)->otyp].oc_material == MT_INKA) uac -= 6;
	if (Race_if(PM_INKA) && uarmf && objects[(uarmf)->otyp].oc_material == MT_INKA) uac -= 6;
	if (Race_if(PM_INKA) && uleft && objects[(uleft)->otyp].oc_material == MT_INKA) uac -= 6;
	if (Race_if(PM_INKA) && uright && objects[(uright)->otyp].oc_material == MT_INKA) uac -= 6;
	if (Race_if(PM_INKA) && uamul && objects[(uamul)->otyp].oc_material == MT_INKA) uac -= 6;
	if (Race_if(PM_INKA) && uimplant && objects[(uimplant)->otyp].oc_material == MT_INKA) uac -= 6;

	if ((Race_if(PM_ELF) || Race_if(PM_PLAYER_MYRKALFR) || Role_if(PM_ELPH)) && uarmh && uarmh->otyp == ELVEN_LEATHER_HELM) uac -= 1;
	if ((Race_if(PM_ELF) || Race_if(PM_PLAYER_MYRKALFR) || Role_if(PM_ELPH)) && uarmh && uarmh->otyp == ELVEN_HELM) uac -= 1;
	if ((Race_if(PM_ELF) || Race_if(PM_PLAYER_MYRKALFR) || Role_if(PM_ELPH)) && uarmh && uarmh->otyp == HIGH_ELVEN_HELM) uac -= 1;
	if ((Race_if(PM_GNOME) || Role_if(PM_GOLDMINER)) && uarmh && uarmh->otyp == GNOMISH_HELM) uac -= 2;
	if (Race_if(PM_ORC) && uarmh && uarmh->otyp == ORCISH_HELM) uac -= 1;
	if ((Race_if(PM_DWARF) || Role_if(PM_MIDGET)) && uarmh && uarmh->otyp == DWARVISH_IRON_HELM) uac -= 1;

	if ((Race_if(PM_ELF) || Race_if(PM_PLAYER_MYRKALFR) || Role_if(PM_ELPH)) && uarms && uarms->otyp == ELVEN_SHIELD) uac -= 1;
	if (Race_if(PM_ORC) && uarms && (uarms->otyp == ORCISH_SHIELD || uarms->otyp == ORCISH_GUARD_SHIELD || uarms->otyp == URUK_HAI_SHIELD) ) uac -= 1;
	if ((Race_if(PM_DWARF) || Role_if(PM_MIDGET)) && uarms && uarms->otyp == DWARVISH_ROUNDSHIELD) uac -= 1;

	if ((Race_if(PM_GNOME) || Role_if(PM_GOLDMINER)) && uarmf && uarmf->otyp == GNOMISH_BOOTS) uac -= 2;

	if (Race_if(PM_MAYMES) && uarmc && uarmc->otyp == CLOAK_OF_PROTECTION) uac -= 2;
	if (Race_if(PM_MAYMES) && uarm && uarm->otyp == ROBE_OF_PROTECTION) uac -= 2;
	if (Race_if(PM_MAYMES) && uleft && uleft->otyp == RIN_PROTECTION) uac -= 2;
	if (Race_if(PM_MAYMES) && uright && uright->otyp == RIN_PROTECTION) uac -= 2;
	if (Race_if(PM_MAYMES) && uleft && uleft->otyp == RIN_THREE_POINT_SEVEN_PROTECTI) uac -= 2;
	if (Race_if(PM_MAYMES) && uright && uright->otyp == RIN_THREE_POINT_SEVEN_PROTECTI) uac -= 2;
	if (Race_if(PM_FRO)) uac -= 2;

	if (Race_if(PM_BOVER)) {
		if (uarm && is_metallic(uarm)) uac += 2;
		if (uarmu && is_metallic(uarmu)) uac += 2;
		if (uarmc && is_metallic(uarmc)) uac += 2;
		if (uarmf && is_metallic(uarmf)) uac += 2;
		if (uarmg && is_metallic(uarmg)) uac += 2;
		if (uarmh && is_metallic(uarmh)) uac += 2;
		if (uarms && is_metallic(uarms)) uac += 2;

		if (uarm && is_lithic(uarm)) uac -= 1;
		if (uarmu && is_lithic(uarmu)) uac -= 1;
		if (uarmc && is_lithic(uarmc)) uac -= 1;
		if (uarmf && is_lithic(uarmf)) uac -= 1;
		if (uarmg && is_lithic(uarmg)) uac -= 1;
		if (uarmh && is_lithic(uarmh)) uac -= 1;
		if (uarms && is_lithic(uarms)) uac -= 1;
	}

	if (uarmc && itemhasappearance(uarmc, APP_SLOWING_GOWN)) uac -= 3;

	if (u.artifactprotection) uac -= 2;
	if (have_mothrelay() ) uac -= 2;

	if (!PlayerCannotUseSkills && !uarms && !u.twoweap) {

		switch (P_SKILL(P_MAKASHI)) {
			case P_BASIC: uac -= 1; break;
			case P_SKILLED: uac -= 2; break;
			case P_EXPERT: uac -= 3; break;
			case P_MASTER: uac -= 4; break;
			case P_GRAND_MASTER: uac -= 5; break;
			case P_SUPREME_MASTER: uac -= 6; break;

		}

	}

	if (!PlayerCannotUseSkills && uwep && uarm && is_lightsaber(uwep) && (uwep->lamplit || Role_if(PM_SHADOW_JEDI)) && (uarm->otyp >= ROBE && uarm->otyp <= ROBE_OF_WEAKNESS)) {

		switch (P_SKILL(P_SORESU)) {
			case P_BASIC: uac -= 1; break;
			case P_SKILLED: uac -= 2; break;
			case P_EXPERT: uac -= 3; break;
			case P_MASTER: uac -= 4; break;
			case P_GRAND_MASTER: uac -= 5; break;
			case P_SUPREME_MASTER: uac -= 6; break;

		}

	}

	if (PlayerInHighHeels && !(PlayerCannotUseSkills)) { /* extra AC --Amy */

		switch (P_SKILL(P_HIGH_HEELS)) {
			case P_BASIC: uac -= 1; break;
			case P_SKILLED: uac -= 2; break;
			case P_EXPERT: uac -= 3; break;
			case P_MASTER: uac -= 4; break;
			case P_GRAND_MASTER: uac -= 5; break;
			case P_SUPREME_MASTER: uac -= 7; break;

		}

	}

	if (PlayerInSexyFlats && !(PlayerCannotUseSkills)) { /* extra AC --Amy */

		switch (P_SKILL(P_SEXY_FLATS)) {
			case P_BASIC: uac -= 1; break;
			case P_SKILLED: uac -= 2; break;
			case P_EXPERT: uac -= 3; break;
			case P_MASTER: uac -= 4; break;
			case P_GRAND_MASTER: uac -= 5; break;
			case P_SUPREME_MASTER: uac -= 7; break;

		}

	}

	/* shield skill only works for AC if you're not using the shield with a two-hander or dual-wielding --Amy */
	if (uarms && !(uwep && bimanual(uwep)) && !u.twoweap && !(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_SHIELD)) {
			case P_BASIC: uac -= 1; break;
			case P_SKILLED: uac -= 3; break;
			case P_EXPERT: uac -= 5; break;
			case P_MASTER: uac -= 6; break;
			case P_GRAND_MASTER: uac -= 8; break;
			case P_SUPREME_MASTER: uac -= 10; break;

		}

	}

	if (uimplant && !(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_IMPLANTS)) {
			case P_BASIC: uac -= 1; break;
			case P_SKILLED: uac -= 2; break;
			case P_EXPERT: uac -= 3; break;
			case P_MASTER: uac -= 4; break;
			case P_GRAND_MASTER: uac -= 5; break;
			case P_SUPREME_MASTER: uac -= 6; break;

		}

		/* Implants are meant to be used by races without hands, or players who polymorph into forms without hands.
		 * They get a bunch of additional benefits from a worn implant --Amy */
		if (powerfulimplants()) {
			uac -= 5;
			switch (P_SKILL(P_IMPLANTS)) {
				case P_BASIC: uac -= 1; break;
				case P_SKILLED: uac -= 2; break;
				case P_EXPERT: uac -= 3; break;
				case P_MASTER: uac -= 4; break;
				case P_GRAND_MASTER: uac -= 5; break;
				case P_SUPREME_MASTER: uac -= 6; break;

			}
			if (!uarms) uac--;
			if (!uarm) uac--;
			if (!uarmc) uac--;
			if (!uarmu) uac--;
			if (!uarmh) uac--;
			if (!uarmg) uac--;
			if (!uarmf) uac--;

		}

	}

	if (!(PlayerCannotUseSkills)) {
		int armoramount = 0;
		if (uarms) armoramount++;
		if (uarm) armoramount++;
		if (uarmc) armoramount++;
		if (uarmu) armoramount++;
		if (uarmh) armoramount++;
		if (uarmg) armoramount++;
		if (uarmf) armoramount++;
		switch (P_SKILL(P_BODY_ARMOR)) {
			case P_BASIC: uac -= (armoramount / 4); break;
			case P_SKILLED: uac -= (armoramount / 2); break;
			case P_EXPERT: uac -= (armoramount * 3 / 4); break;
			case P_MASTER: uac -= armoramount; break;
			case P_GRAND_MASTER: uac -= (armoramount * 5 / 4); break;
			case P_SUPREME_MASTER: uac -= (armoramount * 6 / 4); break;

		}

		/* add another little bonus regardless of how much or little stuff you wear --Amy */
		switch (P_SKILL(P_BODY_ARMOR)) {
			case P_SKILLED: uac -= 1; break;
			case P_EXPERT: uac -= 1; break;
			case P_MASTER: uac -= 2; break;
			case P_GRAND_MASTER: uac -= 3; break;
			case P_SUPREME_MASTER: uac -= 4; break;

		}

	}

	if (uarmc && itemhasappearance(uarmc, APP_MANTLE_OF_COAT)) {
		uac -= 5;
	}

	if (uarmc && itemhasappearance(uarmc, APP_DNETHACK_CLOAK)) uac += 5;
	if (Race_if(PM_INHERITOR)) uac += 5;
	if (RngeDnethack) uac += 5;
	if (uarmf && uarmf->oartifact == ART_DORA_S_SCRATCHY_HEELS) uac += 5;
	if (uarms && uarms->oartifact == ART_THERMO_NUCLEAR_CHAMBER) uac += 10;
	if (uarms && uarms->oartifact == ART_SUPER_ENERGY_LINES) uac += 10;

	if (uarm && uarm->oartifact == ART_PROTECTION_WITH_A_PRICE) uac -= 5;
	if (uarm && uarm->oartifact == ART_GRANDMASTER_S_ROBE) uac -= 5;
	if (uarm && uarm->oartifact == ART_ARMOR_OF_EREBOR) uac -= 10;
	if (uarmh && uarmh->oartifact == ART_COW_ENCHANTMENT) uac -= 9;
	if (uarmh && uarmh->oartifact == ART_CROWN_OF_THE_SAINT_KING) uac -= 5;
	if (uarmh && uarmh->oartifact == ART_CASQUESPIRE_TRANSLATE) uac -= 5;
	if (uarmh && uarmh->oartifact == ART_HARD_HAT_AREA) uac -= 5;
	if (uarmh && uarmh->oartifact == ART_STEELSKULL_PROTECTOR) uac -= 3;
	if (uarmh && uarmh->oartifact == ART_HELM_OF_THE_DARK_LORD) uac -= 5;
	if (uarmh && uarmh->oartifact == ART_METEORIC_AC) uac -= 15;
	if (uarmg && uarmg->oartifact == ART_MOLASS_TANK) uac -= 10;
	if (uarmg && uarmg->oartifact == ART_TANKS_A_LOT) uac -= 5;
	if (uarms && uarms->oartifact == ART_AEAEAEAEAEGIS) uac -= 10;
	if (uarms && uarms->oartifact == ART_WHANG_CLINK_CLONK) uac -= 5;
	if (uarmf && uarmf->oartifact == ART_I_M_A_BITCH__DEAL_WITH_IT) uac -= 5;
	if (uarmf && uarmf->oartifact == ART_MELISSA_S_BEAUTY) uac -= 5;
	if (uleft && uleft->oartifact == ART_CRYLOCK) uac -= 10;
	if (uright && uright->oartifact == ART_CRYLOCK) uac -= 10;
	if (uleft && uleft->oartifact == ART_SCRAWNY_PIPSQUEAK) uac -= 5;
	if (uright && uright->oartifact == ART_SCRAWNY_PIPSQUEAK) uac -= 5;
	if (uwep && uwep->oartifact == ART_ACTA_METALLURGICA_VOL___) uac -= 5;
	if (uarm && uarm->oartifact == ART_MAEDHROS_SARALONDE) uac -= 5;
	if (uarm && uarm->oartifact == ART_QUARRY) uac -= 5;
	if (uarm && uarm->oartifact == ART_SHRINK_S_AID) uac -= 7;
	if (uarm && uarm->oartifact == ART_SOFT_GIRL) uac -= 5;
	if (uarm && uarm->oartifact == ART_NOPPED_SUIT) uac -= 3;
	if (uarmc && uarmc->oartifact == ART_FIREBURN_COLDSHATTER) uac -= 5;
	if (uarmc && uarmc->oartifact == ART_ACIDSHOCK_CASTLECRUSHER) uac -= 5;
	if (uarmc && uarmc->oartifact == ART_YAUI_GAUI_FURS) uac -= 5;
	if (uarmc && uarmc->oartifact == ART_PHANTOM_OF_THE_OPERA) uac -= 5;
	if (uarm && uarm->oartifact == ART_SILKS_OF_THE_VICTOR) uac -= 5;
	if (uarmc && uarmc->oartifact == ART_SPACEWASTE) uac -= 3;
	if (uarmh && uarmh->oartifact == ART_NOSED_BUG) uac -= 7;
	if (uarmf && uarmf->oartifact == ART_PORCELAIN_ELEPHANT) uac -= 5;
	if (uarmf && uarmf->oartifact == ART_DAMPENER) uac -= 5;
	if (uarmf && uarmf->oartifact == ART_ROCKZ_ARMY) uac -= 10;
	if (uarmc && uarmc->oartifact == ART_SEXY_STROKING_UNITS) uac -= 5;
	if (uarm && uarm->oartifact == ART_ANASTASIA_S_SOFT_CLOTHES) uac -= 10;
	if (uarm && uarm->oartifact == ART_ROCKET_IMPULSE) uac -= 10;
	if (uarm && uarm->oartifact == ART_THA_WALL) uac -= 9;
	if (uarmc && uarmc->oartifact == ART_LAURA_S_SWIMSUIT) uac += 5;
	if (uwep && uwep->oartifact == ART_ELOPLUS_STAT) uac -= 1;
	if (uarm && uarm->oartifact == ART_BLUEFORM) uac -= 2;
	if (uarms && uarms->oartifact == ART_CUTTING_THROUGH) uac -= 5;
	if (Role_if(PM_ARCHEOLOGIST) && uamul && uamul->oartifact == ART_ARCHEOLOGIST_SONG) uac -= 2;
	if (uarmh && uarmh->oartifact == ART_DUE_DUE_DUE_DUE_BRMMMMMMM) uac -= 2;
	if (uarmc && uarmc->oartifact == ART_CAN_T_TOUCH_THIS) uac -= 10;
	if (uarmg && uarmg->oartifact == ART_RAAAAAAAARRRRRRGH) uac -= 5;
	if (uarmg && uarmg->oartifact == ART_STOUT_IMMURRING) uac -= 10;
	if (uamul && uamul->oartifact == ART_WOUUU) uac -= 5;
	if (uarmc && uarmc->oartifact == ART_HIGH_KING_OF_SKIRIM) uac -= 5;
	if (uarmg && uarmg->oartifact == ART_MARY_INSCRIPTION) uac -= 5;
	if (uarm && uarm->oartifact == ART_REQUIRED_POWER_PLANT_GEAR) uac -= 5;
	if (uarm && uarm->oartifact == ART_STABLE_EXOSKELETON) uac -= 10;
	if (HardcoreAlienMode) uac -= 1;
	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_HENRIETTA_S_TENACIOUSNESS) uac -= 10;
	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_LAUGHING_AT_MIDNIGHT) uac -= 5;
	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_ARABELLA_S_SEXY_CHARM) uac -= 20;
	if (Role_if(PM_OTAKU) && uarmc && itemhasappearance(uarmc, APP_FOURCHAN_CLOAK)) uac -= 1;
	if (uarmf && uarmf->oartifact == ART_KATI_S_IRRESISTIBLE_STILET) uac -= 2;
	if (uarmf && uarmf->oartifact == ART_EXCITING_SPFLOTCH) uac -= 2;
	if (uarmc && uarmc->oartifact == ART_FOOKING_TANK) uac -= 10;
	if (uarmg && uarmg->oartifact == ART_AA_S_CRASHING_TRAGEDY) uac -= 5;
	if (uarmf && uarmf->oartifact == ART_INERT_GREAVES) uac -= 4;
	if (uarmf && uarmf->oartifact == ART_UNFELLABLE_TREE && u.burrowed) uac -= 20;
	if (Race_if(PM_DUTHOL) && PlayerInBlockHeels) uac -= 5;
	if (Race_if(PM_HYPOTHERMIC) && uarmc) uac -= 3;
	if (uarm && uarm->oartifact == ART_UBERGAGE) uac -= 4;
	if (uarm && uarm->oartifact == ART_PEOPLE_COAT) uac -= 5;
	if (uarms && uarms->oartifact == ART_NORSE_MITHRIL) uac -= 5;
	if (uarmu && uarmu->oartifact == ART_SWEET_VICTORIA) uac -= 2;
	if (uarmh && uarmh->oartifact == ART_FOOTBALL_MASK) uac -= 2;
	if (uarmf && uarmf->oartifact == ART_ARTHUR_S_HIGH_HEELED_PLATF) uac -= 2;
	if (uwep && uwep->oartifact == ART_SIGIX_BROADSWORD) uac -= 20;
	if (uarm && uarm->oartifact == ART_MOEBIUS_ARMOR) uac -= 10;
	if (bmwride(ART_PANZER_TANK)) uac -= 10;
	if (FemtrapActivePatricia) uac -= 3;
	if (uarm && uarm->oartifact == ART_ELMHERE && multi < 0) uac -= 5;
	if (bmwride(ART_KERSTIN_S_COWBOY_BOOST)) uac -= 5;

	if (uamul && uamul->oartifact == ART_MOSH_PIT_SCRAMBLE) {
		if ((!uarm || is_metallic(uarm)) && (!uarmc || is_metallic(uarmc)) && (!uarmu || is_metallic(uarmu)) && (!uarms || is_metallic(uarms)) && (!uarmg || is_metallic(uarmg)) && (!uarmf || is_metallic(uarmf)) && (!uarmh || is_metallic(uarmh)) ) {
			if (uarm && is_metallic(uarm)) uac -= 2;
			if (uarmu && is_metallic(uarmu)) uac -= 2;
			if (uarmc && is_metallic(uarmc)) uac -= 2;
			if (uarmh && is_metallic(uarmh)) uac -= 2;
			if (uarmf && is_metallic(uarmf)) uac -= 2;
			if (uarms && is_metallic(uarms)) uac -= 2;
			if (uarmg && is_metallic(uarmg)) uac -= 2;
		}
	}

	/* make it easier for player to finish shoe-cleaning occupation --Amy */
	if (u.singtrapocc || u.katitrapocc) uac -= 20;

	if (Numbed) uac += 5;

	if (uarms && uarms->oartifact == ART_ARMOR_CLASS_WALL) {
		uac -= 5;
		if (uarms && !(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_SHIELD)) {
				case P_BASIC: uac -= 1; break;
				case P_SKILLED: uac -= 2; break;
				case P_EXPERT: uac -= 3; break;
				case P_MASTER: uac -= 4; break;
				case P_GRAND_MASTER: uac -= 5; break;
				case P_SUPREME_MASTER: uac -= 6; break;
			}
		}
	}

	if (uwep && uwep->oartifact == ART_VEST_REPLACEMENT) {
		uac -= 5;
		if (uarms) uac -= 5;
	}

	if (Race_if(PM_SWIKNI)) {
		if (uarmc) {
			uac += (uarmc->oeroded * 2);
			uac += (uarmc->oeroded2 * 2);
		}
		if (uarm) {
			uac += (uarm->oeroded * 2);
			uac += (uarm->oeroded2 * 2);
		}
		if (uarmu) {
			uac += (uarmu->oeroded * 2);
			uac += (uarmu->oeroded2 * 2);
		}
		if (uarms) {
			uac += (uarms->oeroded * 2);
			uac += (uarms->oeroded2 * 2);
		}
		if (uarmh) {
			uac += (uarmh->oeroded * 2);
			uac += (uarmh->oeroded2 * 2);
		}
		if (uarmf) {
			uac += (uarmf->oeroded * 2);
			uac += (uarmf->oeroded2 * 2);
		}
		if (uarmg) {
			uac += (uarmg->oeroded * 2);
			uac += (uarmg->oeroded2 * 2);
		}
	}

	if (uarmc && objects[(uarmc)->otyp].oc_material == MT_VIVA) {
		uac -= (uarmc->oeroded * 2);
		uac -= (uarmc->oeroded2 * 2);
	}
	if (uarm && objects[(uarm)->otyp].oc_material == MT_VIVA) {
		uac -= (uarm->oeroded * 2);
		uac -= (uarm->oeroded2 * 2);
	}
	if (uarmu && objects[(uarmu)->otyp].oc_material == MT_VIVA) {
		uac -= (uarmu->oeroded * 2);
		uac -= (uarmu->oeroded2 * 2);
	}
	if (uarms && objects[(uarms)->otyp].oc_material == MT_VIVA) {
		uac -= (uarms->oeroded * 2);
		uac -= (uarms->oeroded2 * 2);
	}
	if (uarmh && objects[(uarmh)->otyp].oc_material == MT_VIVA) {
		uac -= (uarmh->oeroded * 2);
		uac -= (uarmh->oeroded2 * 2);
	}
	if (uarmf && objects[(uarmf)->otyp].oc_material == MT_VIVA) {
		uac -= (uarmf->oeroded * 2);
		uac -= (uarmf->oeroded2 * 2);
	}
	if (uarmg && objects[(uarmg)->otyp].oc_material == MT_VIVA) {
		uac -= (uarmg->oeroded * 2);
		uac -= (uarmg->oeroded2 * 2);
	}
	if (uamul && objects[(uamul)->otyp].oc_material == MT_VIVA) {
		uac -= 3;
		uac -= (uamul->oeroded * 2);
		uac -= (uamul->oeroded2 * 2);
	}
	if (uimplant && objects[(uimplant)->otyp].oc_material == MT_VIVA) {
		uac -= 3;
		uac -= (uimplant->oeroded * 2);
		uac -= (uimplant->oeroded2 * 2);
	}
	if (uleft && objects[(uleft)->otyp].oc_material == MT_VIVA) {
		uac -= 2;
		uac -= (uleft->oeroded);
		uac -= (uleft->oeroded2);
	}
	if (uright && objects[(uright)->otyp].oc_material == MT_VIVA) {
		uac -= 2;
		uac -= (uright->oeroded);
		uac -= (uright->oeroded2);
	}
	if (ublindf && objects[(ublindf)->otyp].oc_material == MT_VIVA) {
		uac -= 4;
		uac -= (ublindf->oeroded * 3);
		uac -= (ublindf->oeroded2 * 3);
	}
	if (uarmc && objects[(uarmc)->otyp].oc_material == MT_CERAMIC) {
		uac -= 2;
	}
	if (uarm && objects[(uarm)->otyp].oc_material == MT_CERAMIC) {
		uac -= 2;
	}
	if (uarmu && objects[(uarmu)->otyp].oc_material == MT_CERAMIC) {
		uac -= 2;
	}
	if (uarms && objects[(uarms)->otyp].oc_material == MT_CERAMIC) {
		uac -= 2;
	}
	if (uarmh && objects[(uarmh)->otyp].oc_material == MT_CERAMIC) {
		uac -= 2;
	}
	if (uarmf && objects[(uarmf)->otyp].oc_material == MT_CERAMIC) {
		uac -= 2;
	}
	if (uarmg && objects[(uarmg)->otyp].oc_material == MT_CERAMIC) {
		uac -= 2;
	}
	if (uarmc && objects[(uarmc)->otyp].oc_material == MT_METEOSTEEL) {
		uac -= 1;
	}
	if (uarm && objects[(uarm)->otyp].oc_material == MT_METEOSTEEL) {
		uac -= 1;
	}
	if (uarmu && objects[(uarmu)->otyp].oc_material == MT_METEOSTEEL) {
		uac -= 1;
	}
	if (uarms && objects[(uarms)->otyp].oc_material == MT_METEOSTEEL) {
		uac -= 1;
	}
	if (uarmh && objects[(uarmh)->otyp].oc_material == MT_METEOSTEEL) {
		uac -= 1;
	}
	if (uarmf && objects[(uarmf)->otyp].oc_material == MT_METEOSTEEL) {
		uac -= 1;
	}
	if (uarmg && objects[(uarmg)->otyp].oc_material == MT_METEOSTEEL) {
		uac -= 1;
	}

	if (u.negativeprotection) uac += u.negativeprotection;

	if (tech_inuse(T_IRON_SKIN)) uac -= u.ulevel;

	if (!flags.female && uarmu && uarmu->oartifact == ART_GIANT_SWINGING_PENIS) uac -= 10;

	if (u.tunnelized) uac += 20;

	if (is_wagon(u.ux, u.uy)) uac -= 5;

	if (u.burrowed) {
		uac -= 20;
		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_ELEMENTAL_SPELL)) {
				case P_BASIC: uac -= 2; break;
				case P_SKILLED: uac -= 4; break;
				case P_EXPERT: uac -= 6; break;
				case P_MASTER: uac -= 8; break;
				case P_GRAND_MASTER: uac -= 10; break;
				case P_SUPREME_MASTER: uac -= 12; break;

			}

		}
	}

	/* Harlow - make sure it doesn't wrap around ;) */
	uac = (uac < UAC_MIN ? UAC_MIN : (uac > UAC_LIM ? UAC_LIM : uac));

	if (Race_if(PM_ITAQUE)) {
		int difference = (-(uac - 10));
		difference = difference / 10;
		if (difference > 0) uac -= difference;

	}

	if (Race_if(PM_BABYLONIAN)) {
		int difference = (-(uac - 10));
		if (difference > 0) uac -= difference;

	}

	if (uarmg && uarmg->oartifact == ART_EGASSO_S_GIBBERISH) {
		int difference = (-(uac - 10));
		difference = difference / 10;
		if (difference > 0) uac -= difference;

	}

	if (u.berserktime) {
		int difference = (-(uac - 10));
		difference = difference / 5;
		if (difference > 0) uac = 10 - difference;
		
	}

	if (Race_if(PM_SERB)) {
		int difference = (-(uac - 10));
		difference *= 4;
		difference /= 5;
		if (difference > 0) uac = 10 - difference;
	}

	if (Race_if(PM_RUSMOT)) {
		int difference = (-(uac - 10));
		difference *= 4;
		difference /= 5;
		if (difference > 0) uac = 10 - difference;
	}

	if (Dimmed) {
		int difference = (-(uac - 10));
		difference = difference / 2;
		if (difference > 0) uac = 10 - difference;
		
	}

	if (Role_if(PM_DANCER)) { /* glass cannon */
		int difference = (-(uac - 10));
		difference = difference / 2;
		if (difference > 0) uac = 10 - difference;
		
	}

	if (Race_if(PM_TAYIIN)) {
		int difference = (-(uac - 10));
		difference *= 9;
		difference /= 10;
		if (difference > 0) uac = 10 - difference;
	}

	/* heavy two-handed weapons should have a disadvantage to make up for the fact that they deal great damage and
	 * don't suffer from low to-hit like dual-wielded one-handed weapons do. These are basically all two-handers that
	 * are meant to be used in melee, so e.g. bows are unaffected but also polearms because those are already balanced
	 * by the fact that their damage is only mediocre and they're not effective without riding.
	 * Additionally, quarterstaff, grinder and unicorn horn are exempt because they don't get big damage bonuses and
	 * shouldn't be made completely pointless just because I'm trying to balance the heavy hitters.
	 * And double lightsabers are exempt because it's already enough of a hassle to manage power for them. --Amy */
	if (uwep && is_heavyweapon(uwep)) {

		int heavyreduction = 80;

		if (!PlayerCannotUseSkills) {
			switch (P_SKILL(P_TWO_HANDED_WEAPON)) {
				case P_BASIC: heavyreduction = 83; break;
				case P_SKILLED: heavyreduction = 86; break;
				case P_EXPERT: heavyreduction = 89; break;
				case P_MASTER: heavyreduction = 92; break;
				case P_GRAND_MASTER: heavyreduction = 95; break;
				case P_SUPREME_MASTER: heavyreduction = 98; break;
				default: heavyreduction = 80; break;
			}
		}

		int difference = (-(uac - 10));
		difference *= heavyreduction;
		difference /= 100;
		if (difference > 0) uac = 10 - difference;

	}

	if (bmwride(ART_BECHT_S_SPEEDMOBILE)) {
		int difference = (-(uac - 10));
		difference *= 4;
		difference /= 5;
		if (difference > 0) uac = 10 - difference;
	}

	if (Race_if(PM_PLAYER_ASURA)) {

		int worncount = 0;
		if (uarm) worncount++;
		if (uarmc) worncount++;
		if (uarmu) worncount++;
		if (uarms) worncount++;
		if (uarmh) worncount++;
		if (uarmf) worncount++;
		if (uarmg) worncount++;

		if (worncount > 0) {
			int difference = (-(uac - 10));
			difference = difference * (20 - worncount);
			difference /= 20;
			if (difference > 0) uac = 10 - difference;

		}

	}

	if (uarm && uarm->oartifact == ART_IMPRACTICAL_COMBAT_WEAR) {
		int difference = (-(uac - 10));
		difference = difference / 2;
		if (difference > 0) uac = 10 - difference;
		
	}

	if (u.uprops[NAKEDNESS].extrinsic || (uarmg && uarmg->oartifact == ART_SPREAD_YOUR_LEGS_WIDE) || (flags.female && uarmu && uarmu->oartifact == ART_GIANT_SWINGING_PENIS) || Nakedness || have_nakedstone() ) uac = 10;

	if(uac != u.uac){
		u.uac = uac;
		flags.botl = 1;
	}

}

#endif /* OVL0 */
#ifdef OVLB

void
glibr()
{
	register struct obj *otmp;
	int xfl = 0;
	boolean leftfall, rightfall;
	const char *otherwep = 0;

	leftfall = (uleft && !uleft->cursed &&
		    (!uwep || !welded(uwep) || !bimanual(uwep)));
	rightfall = (uright && !uright->cursed && (!welded(uwep)));
	if (!uarmg && (leftfall || rightfall) && !nolimbs(youmonst.data)) {
		/* changed so cursed rings don't fall off, GAN 10/30/86 */
		Your("%s off your %s.",
			(leftfall && rightfall) ? "rings slip" : "ring slips",
			(leftfall && rightfall) ? makeplural(body_part(FINGER)) :
			body_part(FINGER));
		xfl++;
		if (leftfall) {
			otmp = uleft;
			Ring_off(uleft);
			dropx(otmp);
		}
		if (rightfall) {
			otmp = uright;
			Ring_off(uright);
			dropx(otmp);
		}
	}

	otmp = uswapwep;
	if (u.twoweap && otmp && (otmp->otyp != BRASS_KNUCKLES) && !(otmp->oartifact == ART_GREEN_ASSISTANT) && (otmp->otyp != SUPER_KNUCKLES) && (otmp->otyp != ULTRA_KNUCKLES) && (otmp->otyp != ELITE_KNUCKLES) ) {
		otherwep = is_sword(otmp) ? c_sword :
		    makesingular(oclass_names[(int)otmp->oclass]);
		Your("%s %sslips from your %s.",
			otherwep,
			xfl ? "also " : "",
			makeplural(body_part(HAND)));
		setuswapwep((struct obj *)0, FALSE);
		xfl++;
		if (( (otmp->otyp != LOADSTONE && otmp->otyp != HEALTHSTONE && otmp->otyp != LUCKSTONE && otmp->otyp != MANASTONE && otmp->otyp != SLEEPSTONE && otmp->otyp != LOADBOULDER && otmp->otyp != STARLIGHTSTONE && otmp->otyp != STONE_OF_MAGIC_RESISTANCE && !is_nastygraystone(otmp) && !is_feminismstone(otmp) ) || !otmp->cursed) && !(otmp->otyp == LUCKSTONE && isevilvariant && !otmp->cursed && !otmp->blessed && Luck < 0))
			dropx(otmp);
	}
	otmp = uwep;
	if (otmp && !welded(otmp) && (otmp->otyp != BRASS_KNUCKLES) && !(otmp->oartifact == ART_GREEN_ASSISTANT) && (otmp->otyp != SUPER_KNUCKLES) && (otmp->otyp != ULTRA_KNUCKLES) && (otmp->otyp != ELITE_KNUCKLES) ) {
		const char *thiswep;

		/* nice wording if both weapons are the same type */
		thiswep = is_sword(otmp) ? c_sword :
		    makesingular(oclass_names[(int)otmp->oclass]);
		if (otherwep && strcmp(thiswep, otherwep)) otherwep = 0;

		/* changed so cursed weapons don't fall, GAN 10/30/86 */
		Your("%s%s %sslips from your %s.",
			otherwep ? "other " : "", thiswep,
			xfl ? "also " : "",
			makeplural(body_part(HAND)));
		setuwep((struct obj *)0, FALSE, TRUE);
		if (( (otmp->otyp != LOADSTONE && otmp->otyp != HEALTHSTONE && otmp->otyp != LUCKSTONE && otmp->otyp != MANASTONE && otmp->otyp != SLEEPSTONE && otmp->otyp != LOADBOULDER && otmp->otyp != STARLIGHTSTONE && otmp->otyp != STONE_OF_MAGIC_RESISTANCE && !is_nastygraystone(otmp) && !is_feminismstone(otmp) ) || !otmp->cursed) && !(otmp->otyp == LUCKSTONE && isevilvariant && !otmp->cursed && !otmp->blessed && Luck < 0))
			dropx(otmp);
	}
}

struct obj *
some_armor(victim)
struct monst *victim;
{
	register struct obj *otmph, *otmp;

	otmph = (victim == &youmonst) ? uarmc : which_armor(victim, W_ARMC);
	if (!otmph)
	    otmph = (victim == &youmonst) ? uarm : which_armor(victim, W_ARM);
	if (!otmph)
	    otmph = (victim == &youmonst) ? uarmu : which_armor(victim, W_ARMU);
	
	otmp = (victim == &youmonst) ? uarmh : which_armor(victim, W_ARMH);
	if(otmp && (!otmph || !rn2(4))) otmph = otmp;
	otmp = (victim == &youmonst) ? uarmg : which_armor(victim, W_ARMG);
	if(otmp && (!otmph || !rn2(4))) otmph = otmp;
	otmp = (victim == &youmonst) ? uarmf : which_armor(victim, W_ARMF);
	if(otmp && (!otmph || !rn2(4))) otmph = otmp;
	otmp = (victim == &youmonst) ? uarms : which_armor(victim, W_ARMS);
	if(otmp && (!otmph || !rn2(4))) otmph = otmp;
	return(otmph);
}

/* erode some arbitrary armor worn by the victim */
void
erode_armor(victim, acid_dmg)
struct monst *victim;
boolean acid_dmg;
{
	struct obj *otmph = some_armor(victim);

	if (otmph && (otmph != uarmf)) {
	    erode_obj(otmph, acid_dmg, FALSE);
	    if (carried(otmph)) update_inventory();
	}
}

/* used for praying to check and fix levitation trouble */
struct obj *
stuck_ring(ring, otyp)
struct obj *ring;
int otyp;
{
    if (ring != uleft && ring != uright) {
	impossible("stuck_ring: neither left nor right?");
	return (struct obj *)0;
    }

    if (ring && ring->otyp == otyp) {
	/* reasons ring can't be removed match those checked by select_off();
	   limbless case has extra checks because ordinarily it's temporary */
	if (nolimbs(youmonst.data) &&
		uamul && uamul->otyp == AMULET_OF_UNCHANGING && uamul->cursed)
	    return uamul;
	if (welded(uwep) && (ring == uright || bimanual(uwep))) return uwep;
	if (uarmg && uarmg->cursed) return uarmg;
	if (ring->cursed) return ring;
    }
    /* either no ring or not right type or nothing prevents its removal */
    return (struct obj *)0;
}

/* also for praying; find worn item that confers "Unchanging" attribute */
struct obj *
unchanger()
{
    if (uamul && uamul->otyp == AMULET_OF_UNCHANGING) return uamul;
    return 0;
}

/* occupation callback for 'A' */
STATIC_PTR
int
select_off(otmp)
register struct obj *otmp;
{
	struct obj *why;
	char buf[BUFSZ];

	if (!otmp) return 0;
	*buf = '\0';			/* lint suppresion */

	boolean updowninversion = 0;
	if (uarmc && itemhasappearance(uarmc, APP_UP_DOWN_CLOAK)) updowninversion = 1;

	if (uarmf && uarmf->oartifact == ART_END_OF_LEWDNESS) {
		if ((otmp == uarm) || (otmp == uarmu) || (otmp == uarmh) || (otmp == uarmf) || (otmp == uarmg) || (otmp == uarms) || (otmp == uarmc)) {
			pline("Due to your shoes, you cannot take off any armor pieces!");
			return 0;
		}
	}

	/* implant check */
	if (otmp == uimplant) {

		if (uarm && uarm->oartifact == ART_PLANTOPLIM) {
			pline("It seems that the implant is stuck.");
			return 0;
		}

		if (PlayerCannotUseSkills || P_SKILL(P_IMPLANTS) <= P_UNSKILLED) {
			pline("Due to your lack of skill, you cannot remove implants at all. Train the skill first.");
			return 0;
		}

		if (P_SKILL(P_IMPLANTS) == P_BASIC) {
			if (!otmp->blessed || u.uhpmax < 11) {
				pline("Your implants skill is too low to remove this implant. Dipping it in holy water may allow you to remove it though.");
				return 0;
			} else {
				pline("Removing that implant is complicated, you'll take damage to your maximum health and may suffer additional bad effects.");
				getlin ("Really try to remove it? [yes/no]",buf);
				(void) lcase (buf);
				if (!(strcmp (buf, "yes"))) {
					pline("As you try to rip the implant out of your body, you are severely hurt.");
					u.uhpmax -= rnd(10);
					if (u.uhpmax < 1) u.uhpmax = 1;
					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
					losehp(rnd(25), "trying to remove an implant", KILLED_BY);
					badeffect();
				} else return 0;
			}
		}

		if (P_SKILL(P_IMPLANTS) == P_SKILLED) {
			if (!otmp->blessed) {
				pline("Removing that implant is complicated, you'll take damage to your maximum health and may suffer additional bad effects.");
				getlin ("Really try to remove it? [yes/no]",buf);
				(void) lcase (buf);
				if (!(strcmp (buf, "yes"))) {
					pline("As you try to rip the implant out of your body, you are severely hurt.");
					u.uhpmax -= rnd(10);
					if (u.uhpmax < 1) u.uhpmax = 1;
					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
					losehp(rnd(25), "trying to remove an implant", KILLED_BY);
					badeffect();
				} else return 0;
			}
		}

	}

	/* special ring checks */
	if (otmp == uright || otmp == uleft) {
	    if (uarmf && uarmf->oartifact == ART_END_OF_LEWDNESS) {
		pline_The("ring is stuck.");
		return 0;
	    }

	    if (nolimbs(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
		pline_The("ring is stuck.");
		return 0;
	    }
	    if (objects[(otmp)->otyp].oc_material == MT_INKA && !(Race_if(PM_INKA))  && !(youmonst.data->msound == MS_FART_QUIET || youmonst.data->msound == MS_FART_NORMAL || youmonst.data->msound == MS_FART_LOUD) ) {
		pline("Inka rings cannot be taken off. You need to chat to a farting monster to have it removed.");
		return 0;
	    }
	    why = 0;	/* the item which prevents ring removal */
	    if (welded(uwep) && !(!PlayerCannotUseSkills && P_SKILL(P_IMPLANTS) >= P_EXPERT) && (otmp == uright || bimanual(uwep))) {
		sprintf(buf, "free a weapon %s", body_part(HAND));
		why = uwep;
	    } else if (uarmg && !(!PlayerCannotUseSkills && P_SKILL(P_IMPLANTS) >= P_EXPERT) && !FingerlessGloves && uarmg->cursed) {
		sprintf(buf, "take off your %s", c_gloves);
		why = uarmg;
	    }
	    if (why) {
		You("cannot %s to remove the ring.", buf);
		why->bknown = TRUE;
		return 0;
	    }
	}
	/* special shield checks */
	if (otmp == uarms) {
	    if (welded(uwep) && !(!PlayerCannotUseSkills && P_SKILL(P_IMPLANTS) >= P_EXPERT) && bimanual(uwep)) {
		You("cannot slip the shield from your shoulder over your welded weapon.");
		uwep->bknown = TRUE;
		return 0;
	    }
	}
	/* special glove checks */
	if (otmp == uarmg) {
	    if (welded(uwep) && !(!PlayerCannotUseSkills && P_SKILL(P_IMPLANTS) >= P_SKILLED) ) {
		You("are unable to take off your %s while wielding that %s.",
		    c_gloves, is_sword(uwep) ? c_sword : c_weapon);
		uwep->bknown = TRUE;
		return 0;
	    } else if (IsGlib) {
		You_cant("take off the slippery %s with your slippery %s.",
			 c_gloves, makeplural(body_part(FINGER)));
		return 0;
	    }
	}
	/* special boot checks */
	if (otmp == uarmf) {
	    if (u.utrap && u.utraptype == TT_BEARTRAP) {
		pline_The("bear trap prevents you from pulling your %s out.",
			  body_part(FOOT));
		return 0;
	    } else if (u.utrap && u.utraptype == TT_INFLOOR) {
		You("are stuck in the %s, and cannot pull your %s out.",
		    surface(u.ux, u.uy), makeplural(body_part(FOOT)));
		return 0;
	    }
	}
	/* special suit and shirt checks */
	if (otmp == uarm
			|| otmp == uarmu
		) {
	    why = 0;	/* the item which prevents disrobing */
	    if (uarmc && !(!PlayerCannotUseSkills && P_SKILL(P_IMPLANTS) >= P_MASTER) && uarmc->cursed && (!updowninversion || otmp == uarmu) ) {
		sprintf(buf, "remove your %s", cloak_simple_name(uarmc));
		why = uarmc;
	    } else if (otmp == uarmu && !(!PlayerCannotUseSkills && P_SKILL(P_IMPLANTS) >= P_GRAND_MASTER) && uarm && uarm->cursed ) {
		sprintf(buf, "remove your %s", c_suit);
		why = uarm;
	    } else if (welded(uwep) && !(!PlayerCannotUseSkills && P_SKILL(P_IMPLANTS) >= P_EXPERT) && bimanual(uwep)) {
		sprintf(buf, "release your %s",
			is_sword(uwep) ? c_sword :
			(uwep->otyp == BATTLE_AXE) ? c_axe : (uwep->otyp == DWARVISH_BATTLE_AXE) ? c_axe : c_weapon);
		why = uwep;
	    }
	    if (why) {
		You("cannot %s to take off %s.", buf, the(xname(otmp)));
		why->bknown = TRUE;
		return 0;
	    }
	}
	if (updowninversion && !(!PlayerCannotUseSkills && P_SKILL(P_IMPLANTS) >= P_MASTER) && (otmp == uarmc) && uarm && uarm->cursed) {
		You("cannot remove your suit to take off that up-down cloak.");
		if (uarm) uarm->bknown = TRUE;
		return 0;
	}

	/* basic curse check */
	if (otmp == uquiver || (otmp == uswapwep && !u.twoweap)) {
	    ;	/* some items can be removed even when cursed */
	} else {
	    /* otherwise, this is fundamental */
	    if (cursed(otmp)) return 0;
	}

	if(otmp == uarm) takeoff_mask |= WORN_ARMOR;
	else if(otmp == uarmc) takeoff_mask |= WORN_CLOAK;
	else if(otmp == uarmf) takeoff_mask |= WORN_BOOTS;
	else if(otmp == uarmg) takeoff_mask |= WORN_GLOVES;
	else if(otmp == uarmh) takeoff_mask |= WORN_HELMET;
	else if(otmp == uarms) takeoff_mask |= WORN_SHIELD;
	else if(otmp == uarmu) takeoff_mask |= WORN_SHIRT;
	else if(otmp == uleft) takeoff_mask |= LEFT_RING;
	else if(otmp == uright) takeoff_mask |= RIGHT_RING;
	else if(otmp == uamul) takeoff_mask |= WORN_AMUL;
	else if(otmp == uimplant) takeoff_mask |= WORN_IMPLANT;
	else if(otmp == ublindf) takeoff_mask |= WORN_BLINDF;
	else if(otmp == uwep) takeoff_mask |= W_WEP;
	else if(otmp == uswapwep) takeoff_mask |= W_SWAPWEP;
	else if(otmp == uquiver) takeoff_mask |= W_QUIVER;

	else impossible("select_off: %s???", doname(otmp));

	return(0);
}

STATIC_OVL struct obj *
do_takeoff()
{
	register struct obj *otmp = (struct obj *)0;

	if (taking_off == W_WEP) {
	  if(!cursed(uwep)) {
	    setuwep((struct obj *) 0, TRUE, TRUE);
	    You("are empty %s.", body_part(HANDED));
	    u.twoweap = FALSE;
	  }
	} else if (taking_off == W_SWAPWEP) {
	  setuswapwep((struct obj *) 0, TRUE);
	  You("no longer have a second weapon readied.");
	  u.twoweap = FALSE;
	} else if (taking_off == W_QUIVER) {
	  setuqwep((struct obj *) 0);
	  You("no longer have ammunition readied.");
	} else if (taking_off == WORN_ARMOR) {
	  otmp = uarm;
	  if(!cursed(otmp)) (void) Armor_off();
	} else if (taking_off == WORN_CLOAK) {
	  otmp = uarmc;
	  if(!cursed(otmp)) (void) Cloak_off();
	} else if (taking_off == WORN_BOOTS) {
	  otmp = uarmf;
	  if(!cursed(otmp)) (void) Boots_off();
	} else if (taking_off == WORN_GLOVES) {
	  otmp = uarmg;
	  if(!cursed(otmp)) (void) Gloves_off();
	} else if (taking_off == WORN_HELMET) {
	  otmp = uarmh;
	  if(!cursed(otmp)) (void) Helmet_off();
	} else if (taking_off == WORN_SHIELD) {
	  otmp = uarms;
	  if(!cursed(otmp)) (void) Shield_off();
	} else if (taking_off == WORN_SHIRT) {
	  otmp = uarmu;
	  if (!cursed(otmp)) (void) Shirt_off();
	} else if (taking_off == WORN_AMUL) {
	  otmp = uamul;
	  if(!cursed(otmp)) Amulet_off();
	} else if (taking_off == WORN_IMPLANT) {
	  otmp = uimplant;
	  if(!cursed(otmp)) Implant_off();
	} else if (taking_off == LEFT_RING) {
	  otmp = uleft;
	  if(!cursed(otmp)) Ring_off(uleft);
	} else if (taking_off == RIGHT_RING) {
	  otmp = uright;
	  if(!cursed(otmp)) Ring_off(uright);
	} else if (taking_off == WORN_BLINDF) {
	  if (!cursed(ublindf)) Blindf_off(ublindf);
	} else impossible("do_takeoff: taking off %lx", taking_off);

	return(otmp);
}

static const char *disrobing = "";

STATIC_PTR
int
take_off()
{
	register int i;
	register struct obj *otmp;

	if (taking_off) {
	    if (todelay > 0) {
		todelay--;
		return(1);	/* still busy */
	    } else {
		if ((otmp = do_takeoff())) off_msg(otmp);
	    }
	    takeoff_mask &= ~taking_off;
	    taking_off = 0L;
	}

	for(i = 0; takeoff_order[i]; i++)
	    if(takeoff_mask & takeoff_order[i]) {
		taking_off = takeoff_order[i];
		break;
	    }

	otmp = (struct obj *) 0;
	todelay = 0;

	if (taking_off == 0L) {
	  You("finish %s.", disrobing);
	  return 0;
	} else if (taking_off == W_WEP) {
	  todelay = 1;
	} else if (taking_off == W_SWAPWEP) {
	  todelay = 1;
	} else if (taking_off == W_QUIVER) {
	  todelay = 1;
	} else if (taking_off == WORN_ARMOR) {
	  otmp = uarm;
	  /* If a cloak is being worn, add the time to take it off and put
	   * it back on again.  Kludge alert! since that time is 0 for all
	   * known cloaks, add 1 so that it actually matters...
	   */
	  if (uarmc) todelay += 2 * objects[uarmc->otyp].oc_delay + 1;
	} else if (taking_off == WORN_CLOAK) {
	  otmp = uarmc;
	} else if (taking_off == WORN_BOOTS) {
	  otmp = uarmf;
	} else if (taking_off == WORN_GLOVES) {
	  otmp = uarmg;
	} else if (taking_off == WORN_HELMET) {
	  otmp = uarmh;
	} else if (taking_off == WORN_SHIELD) {
	  otmp = uarms;
	} else if (taking_off == WORN_SHIRT) {
	  otmp = uarmu;
	  /* add the time to take off and put back on armor and/or cloak */
	  if (uarm)  todelay += 2 * objects[uarm->otyp].oc_delay;
	  if (uarmc) todelay += 2 * objects[uarmc->otyp].oc_delay + 1;
	} else if (taking_off == WORN_AMUL) {
	  todelay = 1;
	} else if (taking_off == WORN_IMPLANT) {
	  todelay = 1;
	} else if (taking_off == LEFT_RING) {
	  todelay = 1;
	} else if (taking_off == RIGHT_RING) {
	  todelay = 1;
	} else if (taking_off == WORN_BLINDF) {
	  todelay = 2;
	} else {
	  impossible("take_off: taking off %lx", taking_off);
	  return 0;	/* force done */
	}

	if (otmp) todelay += objects[otmp->otyp].oc_delay;

	/* Since setting the occupation now starts the counter next move, that
	 * would always produce a delay 1 too big per item unless we subtract
	 * 1 here to account for it.
	 */
	if (todelay > 0) todelay--;

	set_occupation(take_off, disrobing, 0);
	return(1);		/* get busy */
}

/* clear saved context to avoid inappropriate resumption of interrupted 'A' */
void
reset_remarm()
{
	taking_off = takeoff_mask = 0L;
	disrobing = nul;
}

/* the 'A' command -- remove multiple worn items */
int
doddoremarm()
{
    int result = 0;

	if (HardcoreAlienMode) {
		int i, j, bd = 1;
		struct monst *mtmp;
		for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) {
				if (canspotmon(mtmp)) {
					pline("Not now! They're looking!");
					if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
					return 0;
				} else {
					pline("Someone watched you change clothes...");
					if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
					u.ugangr++;
					adjalign(-50);
					change_luck(-1);
					prayer_done();
					return 1;

				}
			}
		}

	}

    if (taking_off || takeoff_mask) {
	You("continue %s.", disrobing);
	set_occupation(take_off, disrobing, 0);
	return 0;
    } else if (!uwep && !uswapwep && !uquiver && !uamul && !uimplant && !ublindf &&
		!uleft && !uright && !wearing_armor()) {
	You("are not wearing anything.");
	return 0;
    }

    add_valid_menu_class(0); /* reset */
    if ((flags.menu_style != MENU_TRADITIONAL && !InventoryDoesNotGo) ||
	    (result = ggetobj("take off", select_off, 0, FALSE, (unsigned *)0)) < -1)
	result = menu_remarm(result);

    if (takeoff_mask) {
	/* default activity for armor and/or accessories,
	   possibly combined with weapons */
	disrobing = "disrobing";
	/* specific activity when handling weapons only */
	if (!(takeoff_mask & ~(W_WEP|W_SWAPWEP|W_QUIVER)))
	    disrobing = "disarming";
	(void) take_off();
    }
    /* The time to perform the command is already completely accounted for
     * in take_off(); if we return 1, that would add an extra turn to each
     * disrobe.
     */
    return 0;
}

STATIC_OVL int
menu_remarm(retry)
int retry;
{
    int n, i = 0;
    menu_item *pick_list;
    boolean all_worn_categories = TRUE;

    if (retry) {
	all_worn_categories = (retry == -2);
    } else if (flags.menu_style == MENU_FULL && !InventoryDoesNotGo) {
	all_worn_categories = FALSE;
	n = query_category("What type of things do you want to take off?",
			   invent, WORN_TYPES|ALL_TYPES, &pick_list, PICK_ANY);
	if (!n) return 0;
	for (i = 0; i < n; i++) {
	    if (pick_list[i].item.a_int == ALL_TYPES_SELECTED)
		all_worn_categories = TRUE;
	    else
		add_valid_menu_class(pick_list[i].item.a_int);
	}
	free((void *) pick_list);
    } else if (flags.menu_style == MENU_COMBINATION && !InventoryDoesNotGo) {
	all_worn_categories = FALSE;
	if (ggetobj("take off", select_off, 0, TRUE, (unsigned *)0) == -2)
	    all_worn_categories = TRUE;
    }

	if (CannotSelectItemsInPrompts) return 0;

    n = query_objlist("What do you want to take off?", invent,
			SIGNAL_NOMENU|USE_INVLET|INVORDER_SORT,
			&pick_list, PICK_ANY,
			all_worn_categories ? is_worn : is_worn_by_type);
    if (n > 0) {
	for (i = 0; i < n; i++)
	    (void) select_off(pick_list[i].item.a_obj);
	free((void *) pick_list);
    } else if (n < 0 && flags.menu_style != MENU_COMBINATION) {
	There("is nothing else you can remove or unwield.");
    }
    return 0;
}

/* hit by destroy armor scroll/black dragon breath/monster spell */
int
destroy_arm(atmp)
register struct obj *atmp;
{
	register struct obj *otmp;

	/* Amy edit: erosionproof gear should have a saving throw */
	/* In Soviet Russia, no item may ever be safe from destruction unless it was already resistant in vanilla.
	 * After all, the government uses disintegration beams to destroy the gear of people with 'unwanted' opinions,
	 * and those bastards shouldn't be able to take countermeasures :-P */

#define DESTROY_ARM(o) ((otmp = (o)) != 0 && \
			(!atmp || atmp == otmp) && (!otmp->oerodeproof || issoviet || !rn2(3)) && \
			(!obj_resists(otmp, 0, 90)))

	if (DESTROY_ARM(uarmc)) {
		if (donning(otmp)) cancel_don();
		Your("%s crumbles and turns to dust!",
		     cloak_simple_name(uarmc));
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vasha bronya ne yavlyayetsya bezopasnym. Luchshe pokhoronit' svoi plany voskhozhdeniya srazu." : "KRRRRRRRRRTSCH!");
		(void) Cloak_off();
		useup(otmp);
	} else if (DESTROY_ARM(uarm)) {
		if (donning(otmp)) cancel_don();
		Your("armor turns to dust and falls to the %s!",
			surface(u.ux,u.uy));
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vasha bronya ne yavlyayetsya bezopasnym. Luchshe pokhoronit' svoi plany voskhozhdeniya srazu." : "KRRRRRRRRRTSCH!");
		(void) Armor_gone();
		useup(otmp);
	} else if (DESTROY_ARM(uarmu)) {
		if (donning(otmp)) cancel_don();
		Your("shirt crumbles into tiny threads and falls apart!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vasha bronya ne yavlyayetsya bezopasnym. Luchshe pokhoronit' svoi plany voskhozhdeniya srazu." : "KRRRRRRRRRTSCH!");
		(void) Shirt_off();
		useup(otmp);
	} else if (DESTROY_ARM(uarmh)) {
		if (donning(otmp)) cancel_don();
		Your("helmet turns to dust and is blown away!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vasha bronya ne yavlyayetsya bezopasnym. Luchshe pokhoronit' svoi plany voskhozhdeniya srazu." : "KRRRRRRRRRTSCH!");
		(void) Helmet_off();
		useup(otmp);
	} else if (DESTROY_ARM(uarmg)) {
		if (donning(otmp)) cancel_don();
		Your("gloves vanish!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vasha bronya ne yavlyayetsya bezopasnym. Luchshe pokhoronit' svoi plany voskhozhdeniya srazu." : "KRRRRRRRRRTSCH!");
		(void) Gloves_off();
		useup(otmp);
		selftouch("You");
	} else if (DESTROY_ARM(uarmf)) {
		if (donning(otmp)) cancel_don();
		Your("boots disintegrate!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vasha bronya ne yavlyayetsya bezopasnym. Luchshe pokhoronit' svoi plany voskhozhdeniya srazu." : "KRRRRRRRRRTSCH!");
		(void) Boots_off();
		useup(otmp);
	} else if (DESTROY_ARM(uarms)) {
		if (donning(otmp)) cancel_don();
		Your("shield crumbles away!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vasha bronya ne yavlyayetsya bezopasnym. Luchshe pokhoronit' svoi plany voskhozhdeniya srazu." : "KRRRRRRRRRTSCH!");
		(void) Shield_off();
		useup(otmp);
	} else {
		return 0;		/* could not destroy anything */
	}

#undef DESTROY_ARM
	stop_occupation();
	return(1);
}

/* adj_abon was here. I removed it. All items that used it are handled in attrib.c now, which does the same thing,
 * with the difference that it's much more elegant and we don't have to put adj_abon calls all over the source :-) --Amy */

#endif /* OVLB */

/*do_wear.c*/
