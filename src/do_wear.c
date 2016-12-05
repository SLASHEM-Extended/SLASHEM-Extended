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

static NEARDATA const char see_yourself[] = "see yourself";
static NEARDATA const char unknown_type[] = "Unknown type of %s (%d)";
static NEARDATA const char c_armor[]  = "armor",
			   c_suit[]   = "suit",
#ifdef TOURIST
			   c_shirt[]  = "shirt",
#endif
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
	WORN_HELMET, WORN_AMUL, WORN_ARMOR,
#ifdef TOURIST
	WORN_SHIRT,
#endif
	WORN_BOOTS, W_SWAPWEP, W_QUIVER, 0L };

STATIC_DCL void FDECL(on_msg, (struct obj *));
STATIC_DCL void FDECL(on_msgdel, (struct obj *));
STATIC_DCL void FDECL(Ring_off_or_gone, (struct obj *, BOOLEAN_P));
STATIC_PTR int FDECL(select_off, (struct obj *));
STATIC_DCL struct obj *NDECL(do_takeoff);
STATIC_PTR int NDECL(take_off);
STATIC_DCL int FDECL(menu_remarm, (int));
STATIC_DCL void FDECL(already_wearing, (const char*));
STATIC_DCL void FDECL(already_wearing2, (const char*, const char*));

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
		Sprintf(how, " around your %s", body_part(HEAD));
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
		Sprintf(how, " around your %s", body_part(HEAD));
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
#ifdef JEDI
	case PLASTEEL_BOOTS:
#endif
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
	case HIGH_STILETTOS:
	case BOOTS_OF_TOTAL_STABILITY:
	case BOOTS_OF_DISPLACEMENT:
	case BOOTS_OF_SWIMMING:
	case ANTI_CURSE_BOOTS:
	case SKY_HIGH_HEELS:
	case PREHISTORIC_BOOTS:
	case SYNTHETIC_SANDALS:
		break;
	case BOOTS_OF_MOLASSES:
		pline(Hallucination ? "Icy legs, how cool!" : "These boots feel a little cold...");
		break;
	case ZIPPER_BOOTS:
		pline(Hallucination ? "Lovely fleecy caressing feelings stream into your legs!" : "While putting on this pair of boots, their zippers try to scratch your legs!");
		losehp(rnd(20), "foolishly putting on a zipper boot", KILLED_BY);
		break;
	case HIPPIE_HEELS:
		if (flags.female) pline("You're dressed like a '69 whore!");
		else pline("You're dressed like a frickin' hardrocker!");
	    if (!uarmf->cursed) {curse(uarmf); pline("The highly erotic boots weld themselves to your feet!");}
		break;

	case AUTODESTRUCT_DE_VICE_BOOTS:
	case SPEEDBUG_BOOTS:
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
	case STAIRWELL_STOMPING_BOOTS:
	case UNFAIR_STILETTOS:
		if (!uarmf->cursed) curse(uarmf);
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
	default: impossible(unknown_type, c_boots, uarmf->otyp);
    }

    if (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "velcro boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "lipuchki sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "cirt chizilmasin") ) ) {
	      if (!uarmf->cursed) {
			curse(uarmf);
			pline("The velcro boots constrict your %s, and you can't take them off again!", makeplural(body_part(FOOT)) );
		}

    }

    if (uarmf && !Role_if(PM_BINDER) && uarmf->oartifact == ART_BINDER_CRASH) {
		if (!(u.monstertimefinish % 20)) { /* 1 in 20 games */
			u.temprecursion = 0;
			u.temprecursiontime = 0;
			u.oldrecursionrole = -1;
			u.oldrecursionrace = -1;
			flags.initrole = 11;
			urole = roles[11];

			pline("You turn into a Binder!");
		} else {
			curse(uarmf);
			uarmf->hvycurse = 1;
			u.ugangr += 3;
			pline("You hear a frightening crash in the distance...");
		}
    }

    if (uarmf && uarmf->oartifact == ART_MEPHISTO_S_BROGUES) {
		if (!uarmf->cursed) {
			curse(uarmf);
			pline("A demonic aura envelopes your boots.");
		}
    }

    if (uarmf && uarmf->oartifact == ART_ELLA_S_BLOODLUST) {
		if (!uarmf->cursed) {
			curse(uarmf);
			if (!strncmpi(plname, "Ella", 4)) pline("You finally got your bloodthirsty footwear back! They immediately make sure they won't get separated from you.");
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

    if (uarmf && uarmf->oartifact == ART_LILAC_BEAUTY) {
		pline("Your incredibly beautiful footwear speaks:");
		verbalize("%s, you're not our owner. We allow you to wear us anyway, but if you ever dare to take us off, we will scratch your shins to the bone and rip your unprotected skin to shreds. Keep wearing us if you want to avoid a bloodbath!", plname);
    }

    if (uarmf && uarmf->oartifact == ART_I_M_A_BITCH__DEAL_WITH_IT && uarmf->spe < 0) {
		uarmf->spe = 0;
    }

    if (uarmf && uarmf->oartifact == ART_MANUELA_S_TORTURE_HEELS) {
		uarmf->hvycurse = 1; /* already autocursed above */
		pline("Manuela put a terrible curse on this beautiful pair of red block-heeled combat boots.");
    }

    if (uarmf && uarmf->oartifact == ART_CURSING_ANOMALY) {
		if (!uarmf->cursed) {
			curse(uarmf);
			pline("Your boots feel deathly cold. But seriously, you did expect them to autocurse, right?");
		}
    }


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
		pline(Hallucination ? "You get the feeling that something soft just slid along the full length of your legs!" : "While taking off this pair of boots, you scratch open your legs at their zippers!");
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
	case HIGH_BOOTS:
	case CRYSTAL_BOOTS:
	case JUMPING_BOOTS:
	case KICKING_BOOTS:
	case FIRE_BOOTS:
	case AUTODESTRUCT_DE_VICE_BOOTS:
	case SPEEDBUG_BOOTS:
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
	case FREEZING_BOOTS:
	case UNFAIR_STILETTOS:
	case SKY_HIGH_HEELS:
	case PREHISTORIC_BOOTS:
	case SYNTHETIC_SANDALS:
#ifdef JEDI
	case PLASTEEL_BOOTS:
#endif
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
	case FILLER_CLOAK:
	case CLOAK_OF_GROUNDING:
	case CLOAK_OF_QUENCHING:
	case CLOAK_OF_CONFUSION:
	case MANACLOAK:
	case MISSING_CLOAK:
	case TROLL_HIDE:
	case PLASTEEL_CLOAK:
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
	case DISPLACER_BEAST_HIDE:
	case THE_NEMEAN_LION_HIDE:
	case CLOAK_OF_SPRAY:
	case CLOAK_OF_FLAME:
	case CLOAK_OF_INSULATION:
	case CLOAK_OF_MATADOR:
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
	case CLOAK_OF_MAP_AMNESIA:
	case CLOAK_OF_TRANSFORMATION:

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
		if (Poison_resistance)
			pline(Hallucination ? "Very tight, like a latex shirt!" : "This cloak feels a little itchy.");
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
		} else if (!Antimagic && rn2(50) > 12) {
		    if (Hallucination) {
			You("have an out of body experience.");
		    } else if (!rnd(50)) {
			killer_format = KILLED_BY_AN;
			killer = "cloak of death";
			done(DIED);
		    } else {
			u.uhpmax -= d(4, 4);
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
	case HEAVY_STATUS_CLOAK:
	case CLOAK_OF_LUCK_NEGATION:
	case YELLOW_SPELL_CLOAK:
	case VULNERABILITY_CLOAK:
	case CLOAK_OF_INVENTORYLESSNESS:
	case CLOAK_OF_RESPAWNING:
	case NASTY_CLOAK:

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
			if (uarmc) polycloak = poly_obj(uarmc, STRANGE_OBJECT);
			if (polycloak && is_hazy(polycloak)) {
				stop_timer(UNPOLY_OBJ, (genericptr_t) polycloak);
				polycloak->oldtyp = STRANGE_OBJECT;
			}
			if (uarmc) (void) Cloak_off();
			You_feel("a little %s.", Hallucination ? "normal" : "strange");
			if (!Unchanging) polyself(FALSE);
			return 0;
			break;
		}
	case CLOAK_OF_WATER_SQUIRTING:
		pline("A gush of water squirts all over your body!");
		water_damage(invent, FALSE, FALSE);
		break;
	case CLOAK_OF_PARALYSIS:
		pline("You're paralyzed!");
		if (Free_action) nomul(-rnd(10), "putting on a cloak of paralysis");
		else nomul(-rnd(20), "putting on a cloak of paralysis");
		break;
	case CLOAK_OF_SICKNESS:
		if (Sick_resistance || !rn2(10) ) { /* small chance to not get infected even if not resistant --Amy */
			You_feel("a slight illness.");
		} else {
			make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
				"cloak of sickness", TRUE, SICK_NONVOMITABLE);
		}

		break;
	case CLOAK_OF_SLIMING:
		if (!Slimed && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) {
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "cloak of sliming";
		}
		break;
	case CLOAK_OF_STARVING:
		if (u.uhunger > 0) u.uhunger = -1;
		else u.uhunger -= rnd(1000);
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
		if (!Stoned && !Stone_resistance && !(poly_when_stoned(youmonst.data) &&
				 polymon(PM_STONE_GOLEM)) ) {
			pline("You start turning to stone!");
			Stoned = 7;
			delayed_killer = "cloak of stone";
		}
		break;
	case CLOAK_OF_LYCANTHROPY:
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREWOLF;
			You_feel("feverish.");
		}
		break;
	case CLOAK_OF_UNLIGHT:
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
		adjattrib(A_STR, -1, FALSE);
		adjattrib(A_DEX, -1, FALSE);
		adjattrib(A_CON, -1, FALSE);
		adjattrib(A_INT, -1, FALSE);
		adjattrib(A_WIS, -1, FALSE);
		adjattrib(A_CHA, -1, FALSE);
		break;
	case CLOAK_OF_TOTTER:
		pline("You completely lose your sense of direction.");
		u.totter = 1;
		break;
	case CLOAK_OF_DRAIN_LIFE:
		losexp("a cloak of drain life", FALSE, TRUE);
		break;

	/* Alchemy smock gives poison _and_ acid resistance */
#if 0
	case ALCHEMY_SMOCK:
		EAcid_resistance |= WORN_CLOAK;
  		break;
#endif
	default: impossible(unknown_type, c_cloak, uarmc->otyp);
    }

	if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "straitjacket cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "smiritel'naya rubashka plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "tor kamzul plash") ) ) {
		if (!uarmc->hvycurse) {
			curse(uarmc);
			uarmc->hvycurse = 1;
			pline("An aura of evil darkness surrounds your cloak as you put it on!");
		}
	}

	if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "ignorant cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "nevezhestvennyye plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "johil plash") ) ) {
		if (!uarmc->cursed) {
			curse(uarmc);
		}
		You_feel("ignorant.");
	}

	if ( (Role_if(PM_GEEK) || Role_if(PM_GRADUATE)) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "geek cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "komp'yutershchik plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "qani plash") ) ) {
		int i;
		for (i = 0; i < MAXSPELL; i++)  {
			if (spellid(i) == SPE_ALTER_REALITY) break;
			else if (spellid(i) == NO_SPELL) {
				spl_book[i].sp_id = SPE_ALTER_REALITY;
				spl_book[i].sp_lev = objects[SPE_ALTER_REALITY].oc_level;
				incrnknow(i);
				pline("You gain the power of Eru Illuvator!");
				break;
			}
		}
	}

	if (uarmc && !(uarmc->cursed) && uarmc->oartifact == ART_INA_S_LAB_COAT) {
		pline("Bad for you - you just cursed yourself with Ina's anorexia. :-(");
		curse(uarmc);
	}

	if (uarmc && !(uarmc->cursed) && uarmc->oartifact == ART_BROKEN_WINGS) {
		pline("A terrible black glow surrounds your nonfunctional wings and roots you to the ground.");
		curse(uarmc);
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

	if (uarmc && uarmc->oartifact == ART_VEIL_OF_LATONA) {
		if (!uarmc->hvycurse) {
			curse(uarmc);
			uarmc->hvycurse = 1;
			pline("An evil curse goes off as you put on the veil.");
		}
	}

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
	case TROLL_HIDE:
	case POISONOUS_CLOAK:
	case CLOAK_OF_DEATH:
	case OILSKIN_CLOAK:
	case ROBE:
	case ALCHEMY_SMOCK:
	case LEO_NEMAEUS_HIDE:
	case PLASTEEL_CLOAK:
	case LEATHER_CLOAK:
	case CLOAK_OF_WARMTH:
	case CLOAK_OF_GROUNDING:
	case CLOAK_OF_QUENCHING:
	case CLOAK_OF_LEECH:
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
	case ANTI_DISQUIET_CLOAK:
	case HUGGING_GOWN:
	case COCLOAK:
	case CLOAK_OF_HEALTH:
	case CLOAK_OF_DISCOVERY:
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
	case HIDE:
	case DISPLACER_BEAST_HIDE:
	case THE_NEMEAN_LION_HIDE:
	case CLOAK_OF_SPRAY:
	case CLOAK_OF_FLAME:
	case CLOAK_OF_INSULATION:
	case CLOAK_OF_MATADOR:
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
    switch(uarmh->otyp) {
	case FEDORA:
		set_moreluck();
		flags.botl = 1;
		break;
#ifdef JEDI
	case PLASTEEL_HELM:
#endif
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

	case HELM_OF_STEEL:
	case SEDGE_HAT:
	case SKULLCAP:
	case RANDOMIZED_HELMET:
	case CRYSTAL_HELM:
	case DENTED_POT:
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
	case HELM_OF_TELEPORTATION:
	case HELM_OF_TELEPORT_CONTROL:
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
		adj_abon(uarmh, uarmh->spe);
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
	case MUTING_HELM:
	case ULCH_HELMET:
	case DIMMER_HELMET:
	case HELM_OF_STARVATION:
	case QUAFFER_HELMET:
	case INCORRECTLY_ADJUSTED_HELMET:
	case CAPTCHA_HELM:
	case HELM_OF_BAD_ALIGNMENT:
	case SOUNDPROOF_HELMET:
	case OUT_OF_MEMORY_HELMET:
	case UNWANTED_HELMET:
		if (!uarmh->cursed) curse(uarmh);
		break;
	default: impossible(unknown_type, c_helmet, uarmh->otyp);
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
		litroomlite(FALSE);
    }

    if (uarmh && uarmh->oartifact == ART_FORMFILLER) {
		if (!uarmh->cursed) curse(uarmh);
		uarmh->hvycurse = 1;
		/* base item is a "nasty" item, and by definition those give no messages upon cursing themselves --Amy */
    }

    return 0;
}

int
Helmet_off()
{
    takeoff_mask &= ~W_ARMH;

    switch(uarmh->otyp) {
	case FEDORA:
	    setworn((struct obj *)0, W_ARMH);
	    set_moreluck();
	    flags.botl = 1;
	    return 0;
#ifdef JEDI
	case PLASTEEL_HELM:
#endif
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

	case SEDGE_HAT:
	case SKULLCAP:
	case RANDOMIZED_HELMET:
	case CRYSTAL_HELM:
	case HELM_OF_STEEL:
	case DENTED_POT:
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
	case INCORRECTLY_ADJUSTED_HELMET:
	case CAPTCHA_HELM:
	case HELM_OF_BAD_ALIGNMENT:
	case SOUNDPROOF_HELMET:
	case OUT_OF_MEMORY_HELMET:
	/* KMH, balance patch -- removed */ /* but re-inserted by Amy */
	case FIRE_HELMET:
	case HELM_OF_SPEED:
	case HELM_OF_TELEPORTATION:
	case HELM_OF_TELEPORT_CONTROL:
	case HELMET_OF_UNDEAD_WARNING:
	case HELM_OF_DISCOVERY:
	case UNWANTED_HELMET:
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
	    if (!cancelled_don) adj_abon(uarmh, -uarmh->spe);
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

	    break;

	default: impossible(unknown_type, c_helmet, uarmh->otyp);
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

    switch(uarmg->otyp) {
#ifdef JEDI
	case PLASTEEL_GLOVES:
#endif
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
	case GAUNTLETS_OF_STEALING:
	case GAUNTLETS_OF_MISFIRING:
	case EVIL_GLOVES:
		if (!uarmg->cursed) curse(uarmg);
		break;

	case GAUNTLETS_OF_GOOD_FORTUNE:
	    set_moreluck();
	    flags.botl = 1;
		break;

	case GAUNTLETS_OF_SWIMMING:
		if (u.uinwater) {
		   pline(Hallucination ? "Suddenly, you're floating! Whee!" : "Hey! You can swim!");
		   spoteffects(TRUE);
		}
		break;
	case GAUNTLETS_OF_FUMBLING:
		if (!oldprop && !(HFumbling & ~TIMEOUT))
			incr_itimeout(&HFumbling, rnd(20));
		break;
	case GAUNTLETS_OF_POWER:
		/*makeknown(uarmg->otyp);*/
		/*flags.botl = 1;*/ /* taken care of in attrib.c */
		adj_abon(uarmg, uarmg->spe);
		break;
	case GAUNTLETS_OF_DEXTERITY:
		adj_abon(uarmg, uarmg->spe);
		break;
	default: impossible(unknown_type, c_gloves, uarmg->otyp);
    }

    if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "spanish gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "ispanskiy perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "ispaniya qo'lqop") ) ) {
	      if (!uarmg->cursed) {
			curse(uarmg);
			pline("Whoops - your %s are squeezed by these gloves!", makeplural(body_part(HAND)) );
		}

    }

    if (uarmg && uarmg->oartifact == ART_AFK_MEANS_ASS_FUCKER) {
	      if (!uarmg->cursed) {
			curse(uarmg);
			pline("afk (ass-fucker)");
		}
    }

    if (uarmg && uarmg->oartifact == ART_FLICTIONLESS_COMBAT) {
	      u.uprops[ITEMCURSING].intrinsic |= FROMOUTSIDE;
		if (!tech_known(T_BLESSING)) {
		    	learntech(T_BLESSING, FROMOUTSIDE, 1);
		    	pline("Suddenly, you know how to use the blessing technique!");
		} else You_feel("that these gloves would have taught you how to use the blessing technique if you didn't already knew it.");
    }

    if (uarmg && uarmg->oartifact == ART_ARABELLA_S_BANK_OF_CROSSRO) {
		if (!uarmg->cursed) curse(uarmg);
		uarmg->hvycurse = 1;
		/* She lures you in with her unsuspecting bank, but then you realize that this bank just deletes all of
		 * your money... and since "banishmentitis" isn't a thing, you gain levelteleportitis instead. --Amy */
    }

    return 0;
}

int
Gloves_off()
{
    long oldprop =
	u.uprops[objects[uarmg->otyp].oc_oprop].extrinsic & ~WORN_GLOVES;

    takeoff_mask &= ~W_ARMG;

    switch(uarmg->otyp) {
#ifdef JEDI
	case PLASTEEL_GLOVES:
#endif
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
	case GAUNTLETS_OF_STEALING:
	case GAUNTLETS_OF_MISFIRING:
	case GAUNTLETS_OF_PANIC:
	case GAUNTLETS_OF_THE_FORCE:
	case GAUNTLETS_OF_SAFEGUARD:
	case GAUNTLETS_OF_PLUGSUIT:
	case COMMANDER_GLOVES:
	case FIELD_GLOVES:
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
	    /*makeknown(uarmg->otyp);*/
	    /*flags.botl = 1;*/ /* taken care of in attrib.c */
	    if (!cancelled_don) adj_abon(uarmg, -uarmg->spe);
	    break;
	case GAUNTLETS_OF_DEXTERITY:
	    if (!cancelled_don) adj_abon(uarmg, -uarmg->spe);
	    break;
	default: impossible(unknown_type, c_gloves, uarmg->otyp);
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
	Strcpy(kbuf, an(killer_cxname(uwep, TRUE)));
	instapetrify(kbuf);
	uwepgone();  /* life-saved still doesn't allow touching cockatrice */
    }

    /* KMH -- ...or your secondary weapon when you're wielding it */
    if (u.twoweap && uswapwep && uswapwep->otyp == CORPSE &&
	touch_petrifies(&mons[uswapwep->corpsenm])) {
	char kbuf[BUFSZ];

	You("wield the %s in your bare %s.",
	    corpse_xname(uswapwep, TRUE), body_part(HAND));

	Strcpy(kbuf, an(killer_cxname(uswapwep, TRUE)));
	instapetrify(kbuf);
	uswapwepgone();	/* lifesaved still doesn't allow touching cockatrice */
    }

    return 0;
}

int
Shield_on()
{
/*
    switch (uarms->otyp) {
	case SMALL_SHIELD:
	case ELVEN_SHIELD:
	case URUK_HAI_SHIELD:
	case ORCISH_SHIELD:
	case DWARVISH_ROUNDSHIELD:
	case LARGE_SHIELD:
	case STEEL_SHIELD:
	case CRYSTAL_SHIELD:
	case SHIELD_OF_REFLECTION:
	case FLAME_SHIELD:
	case ORCISH_GUARD_SHIELD:
	case SHIELD:
	case SILVER_SHIELD:
	case MIRROR_SHIELD:
	case RAPIRAPI:
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
	case STONE_DRAGON_SCALE_SHIELD:
	case CYAN_DRAGON_SCALE_SHIELD:
	case PSYCHIC_DRAGON_SCALE_SHIELD:
	case RAINBOW_DRAGON_SCALE_SHIELD:
	case BLOOD_DRAGON_SCALE_SHIELD:
	case PLAIN_DRAGON_SCALE_SHIELD:
	case SKY_DRAGON_SCALE_SHIELD:
	case WATER_DRAGON_SCALE_SHIELD:
	case YELLOW_DRAGON_SCALE_SHIELD:
		break;
	default: impossible(unknown_type, c_shield, uarms->otyp);
    }
*/

    if (uarms && uarms->oartifact == ART_SHATTERED_DREAMS) {
		if (!uarms->cursed) {
			curse(uarms);
			pline("Your dreams are shattered as the shield becomes cursed.");
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
    if (uarms && uarms->oartifact == ART_GOLDEN_DAWN) {
		if (!uarms->cursed) {
			curse(uarms);
		}
    }
    if (uarms && uarms->oartifact == ART_REAL_PSYCHOS_WEAR_PURPLE) {
		if (!uarms->cursed) {
			curse(uarms);
		}
    }

    return 0;
}

int
Shield_off()
{
    takeoff_mask &= ~W_ARMS;
/*
    switch (uarms->otyp) {
	case SMALL_SHIELD:
	case ELVEN_SHIELD:
	case URUK_HAI_SHIELD:
	case ORCISH_SHIELD:
	case DWARVISH_ROUNDSHIELD:
	case LARGE_SHIELD:
	case STEEL_SHIELD:
	case CRYSTAL_SHIELD:
	case SHIELD_OF_REFLECTION:
	case FLAME_SHIELD:
	case ORCISH_GUARD_SHIELD:
	case SHIELD:
	case SILVER_SHIELD:
	case MIRROR_SHIELD:
	case RAPIRAPI:
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
	case STONE_DRAGON_SCALE_SHIELD:
	case CYAN_DRAGON_SCALE_SHIELD:
	case PSYCHIC_DRAGON_SCALE_SHIELD:
	case RAINBOW_DRAGON_SCALE_SHIELD:
	case BLOOD_DRAGON_SCALE_SHIELD:
	case PLAIN_DRAGON_SCALE_SHIELD:
	case SKY_DRAGON_SCALE_SHIELD:
	case WATER_DRAGON_SCALE_SHIELD:
	case YELLOW_DRAGON_SCALE_SHIELD:
		break;
	default: impossible(unknown_type, c_shield, uarms->otyp);
    }
*/
    setworn((struct obj *)0, W_ARMS);
    return 0;
}

#ifdef TOURIST
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

	if (uarmu->oartifact == ART_TIE_DYE_SHIRT_OF_SHAMBHALA) {

		if (uarmu->spe < 10) uarmu->spe = 10;

	}

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
#endif	/*TOURIST*/

/* This must be done in worn.c, because one of the possible intrinsics conferred
 * is fire resistance, and we have to immediately set HFire_resistance in worn.c
 * since worn.c will check it before returning.
 */
int
Armor_on()
{
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

	if (uarm && !(uarm->cursed) && uarm->oartifact == ART_SUPERESCAPE_MAIL) {
		pline("BEEEEEEEP! Your armor is cursed!");
		curse(uarm);
	}

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

	case AMULET_OF_RMB_LOSS:
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
		pline("It constricts your throat!");
		Strangled = 6;
		break;

	case AMULET_OF_STONE:
		makeknown(AMULET_OF_STONE);
		if (!Stoned && !Stone_resistance && !(poly_when_stoned(youmonst.data) &&
				 polymon(PM_STONE_GOLEM)) ) {
			pline("You start turning to stone!");
			Stoned = 7;
			stop_occupation();
			delayed_killer = "amulet of stone";
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
				nomul(-sleeptime, "taking a refreshing nap");
				u.usleep = 1;
				nomovemsg = "You wake up from your refreshing nap.";
		HSleeping = rnd(1000);
			}                
		} else HSleeping = rnd(1000);
		break;
	case AMULET_OF_DATA_STORAGE:
		You_feel("full of knowledge.");
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

}

void
Amulet_off()
{
    int otyp = uamul->otyp;
    long oldprop = u.uprops[objects[otyp].oc_oprop].extrinsic & ~WORN_AMUL;
    takeoff_mask &= ~W_AMUL;

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
Ring_on(obj)
register struct obj *obj;
{
    long oldprop = u.uprops[objects[obj->otyp].oc_oprop].extrinsic;
    int old_attrib, which;

    if (obj == uwep) setuwep((struct obj *) 0, TRUE);
    if (obj == uswapwep) setuswapwep((struct obj *) 0, TRUE);
    if (obj == uquiver) setuqwep((struct obj *) 0);

    /* only mask out W_RING when we don't have both
       left and right rings of the same type */
    if ((oldprop & W_RING) != W_RING) oldprop &= ~W_RING;

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
	case RIN_FAST_METABOLISM:
	case RIN_DANGER:

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
#ifdef INVISIBLE_OBJECTS
		see_objects();
#endif

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
    }

    if (obj->oartifact == ART_RING_OF_WOE) {
		curse(obj);
		obj->hvycurse = obj->prmcurse = 1;
		pline("A terrible aura of darkness and eternal damnation surrounds your ring.");
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
#ifdef INVISIBLE_OBJECTS                
		    see_objects();
#endif
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
	    setuwep((struct obj *) 0, TRUE);
	if (otmp == uswapwep)
	    setuswapwep((struct obj *) 0, TRUE);
	if (otmp == uquiver)
	    setuqwep((struct obj *) 0);
	setworn(otmp, W_TOOL);
	on_msg(otmp);

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
		if (otmp->otyp != LENSES)
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
#ifdef TOURIST
	if (uarmu) (void) Shirt_on();
#endif
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
#ifdef TOURIST
    else if (otmp == uarmu)
	result = (afternmv == Shirt_on || afternmv == Shirt_off ||
		  what == WORN_SHIRT);
#endif
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
static NEARDATA const char accessories[] = {RING_CLASS, AMULET_CLASS, TOOL_CLASS, FOOD_CLASS, 0};

/* the 'T' command */
int
dotakeoff()
{
	register struct obj *otmp = (struct obj *)0;
	int armorpieces = 0;

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
#ifdef TOURIST
	} else if (uarmu) {
		armorpieces++;
		otmp = uarmu;
#endif
	}
	if (!armorpieces) {
	     /* assert( GRAY_DRAGON_SCALES > YELLOW_DRAGON_SCALE_MAIL ); */
		if (uskin)
		    pline_The("%s merged with your skin!",
			      uskin->otyp >= GRAY_DRAGON_SCALES ?
				"dragon scales are" : "dragon scale mail is");
		else
		    pline("Not wearing any armor.%s", (iflags.cmdassist && 
				(uleft || uright || uamul || ublindf)) ?
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
	if (otmp == uskin || ((otmp == uarm) && uarmc)
#ifdef TOURIST
			  || ((otmp == uarmu) && (uarmc || uarm))
#endif
		) {
	    You_cant("take that off.");
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
	MOREACC(ublindf);

	if(!Accessories) {
		pline("Not wearing any accessories.%s", (iflags.cmdassist &&
			    (uarm || uarmc ||
#ifdef TOURIST
			     uarmu ||
#endif
			     uarms || uarmh || uarmg || uarmf)) ?
		      "  Use 'T' command to take off armor." : "");
		return(0);
	}
	if (Accessories > 0) otmp = getobj(accessories, "remove");
	if(!otmp) return(0);
	if(!(otmp->owornmask & (W_RING | W_AMUL | W_TOOL))) {
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

	if(cursed(otmp)) return(0);
	if(delay) {
		nomul(delay, "disrobing");
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
#ifdef TOURIST
	    is_shirt(otmp) ? c_shirt :
#endif
	    is_suit(otmp) ? c_suit : 0;

	if (!is_cloak(otmp) && !is_boots(otmp) && Race_if(PM_ELONA_SNAIL) && !Upolyd) {
		if (noisy) pline("As a snail, the only types of armor you can wear are cloaks and boots.");
		return 0;
	}

	if (!is_suit(otmp) && !is_boots(otmp) && Race_if(PM_WEAPONIZED_DINOSAUR) && !Upolyd) {
		if (noisy) pline("As a dinosaur, the only types of armor you can wear are suits and boots.");
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

    if (which && cantweararm(youmonst.data) && !Race_if(PM_TRANSFORMER)  &&
	    /* same exception for cloaks as used in m_dowear() */
	    (which != c_cloak || youmonst.data->msize != MZ_SMALL) &&
	    (racial_exception(&youmonst, otmp) < 1)) {
	if (noisy) { pline_The("%s will not fit on your body.", which);

		if (yn("Try to put it on anyway?") == 'y') {
			if (rn2(2)) { 	u.ublesscnt += rnz(5);
			pline("Feeling uncomfortable, you decide to stop trying.");
		    return 0;}
			}

		else {return(0);}

		}


    } else if (otmp->owornmask & W_ARMOR) {
	if (noisy) already_wearing(c_that_);
	return 0;
    }

    if (welded(uwep) && bimanual(uwep) &&
	    (is_suit(otmp)
#ifdef TOURIST
			|| is_shirt(otmp)
#endif
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

#ifdef JEDI
	/* mind flayers cannot wear helmets at all --Amy */
	} else if (Upolyd && (is_mind_flayer(youmonst.data))) {
		if (noisy)
			pline_The("%s won't fit over your tentacles.", xname(otmp));
		err++;
#endif
	} else
	    *mask = W_ARMH;
    } else if (is_shield(otmp)) {
	if (uarms) {
	    if (noisy) already_wearing(an(c_shield));
	    err++;
	} else if (uwep && bimanual(uwep)) {
	    if (noisy) 
		You("cannot wear a shield while wielding a two-handed %s.",
		    is_sword(uwep) ? c_sword :
		    (uwep->otyp == BATTLE_AXE) ? c_axe : (uwep->otyp == DWARVISH_BATTLE_AXE) ? c_axe : c_weapon);
	    err++;
	} else if (u.twoweap) {
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
	} else if (welded(uwep)) {
	    if (noisy) You("cannot wear gloves over your %s.",
			   is_sword(uwep) ? c_sword : c_weapon);
	    err++;
	} else
	    *mask = W_ARMG;
#ifdef TOURIST
    } else if (is_shirt(otmp)) {
	if (uarm || uarmc || uarmu) {
	    if (uarmu) {
		if (noisy) already_wearing(an(c_shirt));
	    } else {
		if (noisy) You_cant("wear that over your %s.",
			           (uarm && !uarmc) ? c_armor : cloak_simple_name(uarmc));
	    }
	    err++;
	} else
	    *mask = W_ARMU;
#endif
    } else if (is_cloak(otmp)) {
	if (uarmc) {
	    if (noisy) already_wearing(an(cloak_simple_name(uarmc)));
	    err++;
	} else
	    *mask = W_ARMC;
    } else if (is_suit(otmp)) {
	if (uarmc) {
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

	/* cantweararm checks for suits of armor */
	/* verysmall or nohands checks for shields, gloves, etc... */
	if (!Race_if(PM_TRANSFORMER) && !Race_if(PM_HUMAN_WRAITH) && (verysmall(youmonst.data) || nohands(youmonst.data))) {
		pline("Don't even bother. Your current form can't realistically wear armor!");

		if (yn("But you may try to wear something anyway. Do it?") == 'y') {
			if (rn2(3)) { 		make_stunned(HStun + rnd(40),FALSE);
			pline("Damn! You just stagger around aimlessly!");
		    return 1;}
		}
		else {return(0);}

	}

	if (Race_if(PM_HUMAN_WRAITH) && (u.uhpmax < 2 || u.uhp < 2) ) {pline("You don't have enough health to wear armor!");
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
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
		setuwep((struct obj *)0, TRUE);
	if (otmp == uswapwep)
		setuswapwep((struct obj *) 0, TRUE);
	if (otmp == uquiver)
		setuqwep((struct obj *) 0);
	setworn(otmp, mask);
	delay = -objects[otmp->otyp].oc_delay;
	if(delay){
		nomul(delay, "dressing up");
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
#ifdef TOURIST
		if (is_shirt(otmp)) (void) Shirt_on();
#endif
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

	if(uleft && uright && uamul && ublindf) {
		Your("%s%s are full, and you're already wearing an amulet and %s.",
			humanoid(youmonst.data) ? "ring-" : "",
			makeplural(body_part(FINGER)),
			ublindf->otyp==LENSES ? "some lenses" : ublindf->otyp==CONDOME ? "a condome" : "a blindfold");
		return(0);
	}
	otmp = getobj(accessories, "put on");
	if(!otmp) return(0);
	if(otmp->owornmask & (W_RING | W_AMUL | W_TOOL)) {
		already_wearing(c_that_);
		return(0);
	}
	if(welded(otmp)) {
		weldmsg(otmp);
		return(0);
	}
	if(otmp == uwep)
		setuwep((struct obj *)0, TRUE);
	if(otmp == uswapwep)
		setuswapwep((struct obj *) 0, TRUE);
	if(otmp == uquiver)
		setuqwep((struct obj *) 0);
	if(otmp->oclass == RING_CLASS || otmp->otyp == MEAT_RING) {
		if(nolimbs(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
			You("cannot make the ring stick to your body.");
			return(0);
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

			Sprintf(qbuf, "Which %s%s, Right or Left?",
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
		if (uarmg && !FingerlessGloves && uarmg->cursed) {
			uarmg->bknown = TRUE;
		    You("cannot remove your gloves to put on the ring.");
			return(0);
		}
		if (welded(uwep) && bimanual(uwep)) {
			/* welded will set bknown */
	    You("cannot free your weapon hands to put on the ring.");
			return(0);
		}
		if (welded(uwep) && mask==RIGHT_RING) {
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
	} else {	/* it's a blindfold, towel, or lenses */
		if (ublindf) {
			if (ublindf->otyp == TOWEL)
				Your("%s is already covered by a towel.",
					body_part(FACE));
			else if (ublindf->otyp == CONDOME)
					already_wearing("a condome");
			else if (ublindf->otyp == BLINDFOLD) {
				if (otmp->otyp == LENSES)
					already_wearing2("lenses", "a blindfold");
				else
					already_wearing("a blindfold");
			} else if (ublindf->otyp == LENSES) {
				if (otmp->otyp == BLINDFOLD)
					already_wearing2("a blindfold", "some lenses");
				else
					already_wearing("some lenses");
			} else
				already_wearing(something); /* ??? */
			return(0);
		}
		if (otmp->otyp != BLINDFOLD && otmp->otyp != TOWEL && otmp->otyp != LENSES && otmp->otyp != CONDOME) {
			You_cant("wear that!");
			return(0);
		}
		if (otmp->oartifact && !touch_artifact(otmp, &youmonst))
		    return 1;
#ifdef JEDI
		if (uarmh && (uarmh->otyp == PLASTEEL_HELM || uarmh->otyp == HELM_OF_STORMS || uarmh->otyp == HELM_OF_DETECT_MONSTERS) ){
			pline("The %s covers your whole face. You need to remove it first.", xname(uarmh));
			display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return 1;
		}
#endif
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
#define UAC_MIN (-128)
#define UAC_LIM 127

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
#ifdef TOURIST
	if(uarmu) uac -= ARM_BONUS(uarmu);
#endif
	if(uleft && uleft->otyp == RIN_PROTECTION) uac -= uleft->spe;
	if(uright && uright->otyp == RIN_PROTECTION) uac -= uright->spe;
	if (HProtection & INTRINSIC) uac -= u.ublessed;
	uac -= u.uspellprot;

/* STEPHEN WHITE'S NEW CODE */
	/* Dexterity now affects AC */
	if (ACURR(A_DEX) < 4) uac += 3;
	else if (ACURR(A_DEX) < 6) uac += 2;
	else if (ACURR(A_DEX) < 8) uac += 1;
	else if (ACURR(A_DEX) < 14) uac -= 0;
	else if (ACURR(A_DEX) < 21) uac -= ACURR(A_DEX)-14;
	else if (ACURR(A_DEX) < 22) uac -= 6;
	else if (ACURR(A_DEX) < 24) uac -= 7;
	else uac -= 8;

	if (Role_if(PM_MONK) && !uwep && (!uarm ||
		uarm->otyp==ROBE ||
		uarm->otyp==ROBE_OF_POWER ||
		uarm->otyp==ROBE_OF_WEAKNESS ||
		uarm->otyp==ROBE_OF_MAGIC_RESISTANCE ||
		uarm->otyp==ROBE_OF_PERMANENCE ||
		uarm->otyp==ROBE_OF_SPELL_POWER ||
		uarm->otyp==ROBE_OF_FAST_CASTING ||
		uarm->otyp==ROBE_OF_ENERGY_SUCTION ||
		uarm->otyp==ROBE_OF_RANDOMNESS ||
		uarm->otyp==ROBE_OF_DEFENSE ||
		uarm->otyp==ROBE_OF_SPECIALTY ||
		uarm->otyp==ROBE_OF_NASTINESS ||
		uarm->otyp==ROBE_OF_PSIONICS ||
		uarm->otyp==ROBE_OF_PROTECTION) && !uarms) {
/*WAC cap off the Monk's ac bonus to -11 */
            if (u.ulevel > 18) uac -= 11;
            else uac -= (u.ulevel / 2) + 2;
	}
	if (Race_if(PM_DOPPELGANGER) && !uarm) uac -= (u.ulevel / 4) + 1;
	if (Role_if(PM_SHAPESHIFTER) && !uarm) uac -= (u.ulevel / 4) + 1;
	if (Race_if(PM_HAXOR)) uac -= 2;
	if ((Race_if(PM_HUMAN_WEREWOLF) || Race_if(PM_AK_THIEF_IS_DEAD_) || Role_if(PM_LUNATIC)) && !uarm) uac -= (u.ulevel / 4) + 1;

	if (Race_if(PM_HUMAN_WRAITH)) uac -= u.ulevel;
	if (Race_if(PM_TURTLE)) uac -= u.ulevel;
	if (Race_if(PM_LOWER_ENT)) uac -= u.ulevel;

	/* Harlow - make sure it doesn't wrap around ;) */
	uac = (uac < UAC_MIN ? UAC_MIN : (uac > UAC_LIM ? UAC_LIM : uac));
	
	/*Corsets suck*/
	if(uarmu && uarmu->otyp == VICTORIAN_UNDERWEAR){
		uac += 2; //flat penalty. Something in the code "corrects" ac values >10, this is a kludge.
	}

	if (u.menoraget) uac -= 1;
	if (u.bookofthedeadget) uac -= 1;
	if (u.silverbellget) uac -= 1;
	if (u.chaoskeyget && u.neutralkeyget && u.lawfulkeyget) uac -= 1;
	if (u.medusaremoved) uac -= 1;
	if (u.luckstoneget && u.sokobanfinished) uac -= 1;

	/* bonus for wearing racial armor */

	if ((Race_if(PM_DWARF) || Role_if(PM_MIDGET)) && uarm && uarm->otyp == DWARVISH_MITHRIL_COAT) uac -= 1;
	if ((Race_if(PM_DROW) || Role_if(PM_TWELPH)) && uarm && uarm->otyp == DARK_ELVEN_MITHRIL_COAT) uac -= 1;
	if ((Race_if(PM_ELF) || Role_if(PM_ELPH)) && uarm && uarm->otyp == ELVEN_MITHRIL_COAT) uac -= 1;
	if ((Race_if(PM_ELF) || Role_if(PM_ELPH)) && uarm && uarm->otyp == ELVEN_TOGA) uac -= 1;
	if ((Race_if(PM_GNOME) || Role_if(PM_GOLDMINER)) && uarm && uarm->otyp == GNOMISH_SUIT) uac -= 2;
	if (Race_if(PM_ORC) && uarm && (uarm->otyp == ORCISH_CHAIN_MAIL || uarm->otyp == ORCISH_RING_MAIL) ) uac -= 1;

	if (Race_if(PM_ORC) && uarmc && uarmc->otyp == ORCISH_CLOAK ) uac -= 1;
	if ((Race_if(PM_DWARF) || Role_if(PM_MIDGET)) && uarmc && uarmc->otyp == DWARVISH_CLOAK ) uac -= 1;
	if ((Race_if(PM_ELF) || Role_if(PM_ELPH)) && uarmc && uarmc->otyp == ELVEN_CLOAK) uac -= 1;
	if ((Race_if(PM_ELF) || Role_if(PM_ELPH)) && uarmg && uarmg->otyp == ELVEN_GAUNTLETS) uac -= 1;

	if ((Race_if(PM_ELF) || Role_if(PM_ELPH)) && uarmh && uarmh->otyp == ELVEN_LEATHER_HELM) uac -= 1;
	if ((Race_if(PM_ELF) || Role_if(PM_ELPH)) && uarmh && uarmh->otyp == ELVEN_HELM) uac -= 1;
	if ((Race_if(PM_ELF) || Role_if(PM_ELPH)) && uarmh && uarmh->otyp == HIGH_ELVEN_HELM) uac -= 1;
	if ((Race_if(PM_GNOME) || Role_if(PM_GOLDMINER)) && uarmh && uarmh->otyp == GNOMISH_HELM) uac -= 2;
	if (Race_if(PM_ORC) && uarmh && uarmh->otyp == ORCISH_HELM) uac -= 1;
	if ((Race_if(PM_DWARF) || Role_if(PM_MIDGET)) && uarmh && uarmh->otyp == DWARVISH_IRON_HELM) uac -= 1;

	if ((Race_if(PM_ELF) || Role_if(PM_ELPH)) && uarms && uarms->otyp == ELVEN_SHIELD) uac -= 1;
	if (Race_if(PM_ORC) && uarms && (uarms->otyp == ORCISH_SHIELD || uarms->otyp == ORCISH_GUARD_SHIELD || uarms->otyp == URUK_HAI_SHIELD) ) uac -= 1;
	if ((Race_if(PM_DWARF) || Role_if(PM_MIDGET)) && uarms && uarms->otyp == DWARVISH_ROUNDSHIELD) uac -= 1;

	if ((Race_if(PM_GNOME) || Role_if(PM_GOLDMINER)) && uarmf && uarmf->otyp == GNOMISH_BOOTS) uac -= 2;

	if (u.artifactprotection) uac -= 2;
	if (have_mothrelay() ) uac -= 2;

	if (PlayerInHighHeels && !(AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || have_unskilledstone())) { /* extra AC --Amy */

		switch (P_SKILL(P_HIGH_HEELS)) {
			case P_BASIC: uac -= 1; break;
			case P_SKILLED: uac -= 2; break;
			case P_EXPERT: uac -= 3; break;
			case P_MASTER: uac -= 4; break;
			case P_GRAND_MASTER: uac -= 5; break;
			case P_SUPREME_MASTER: uac -= 7; break;

		}

	}

	if (uarms && !(AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || have_unskilledstone())) {
		switch (P_SKILL(P_SHIELD)) {
			case P_BASIC: uac -= 1; break;
			case P_SKILLED: uac -= 2; break;
			case P_EXPERT: uac -= 3; break;
			case P_MASTER: uac -= 4; break;
			case P_GRAND_MASTER: uac -= 5; break;
			case P_SUPREME_MASTER: uac -= 6; break;

		}

	}

	if (!(AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || have_unskilledstone())) {
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

	}

	if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "mantle of coat") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "mantiya pal'to") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "ko'ylagi mantiya")) ) {
		uac -= 5;
	}

	if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "dnethack cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "podzemeliy i vnezemnyye plashch vzlomat'") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "hamzindon va dunyo bo'lmagan doirasi so'yish plash") )) uac += 5;

	if (uarm && uarm->oartifact == ART_PROTECTION_WITH_A_PRICE) uac -= 5;
	if (uarm && uarm->oartifact == ART_GRANDMASTER_S_ROBE) uac -= 5;
	if (uarm && uarm->oartifact == ART_ARMOR_OF_EREBOR) uac -= 10;
	if (uarmh && uarmh->oartifact == ART_COW_ENCHANTMENT) uac -= 9;
	if (uarmh && uarmh->oartifact == ART_CROWN_OF_THE_SAINT_KING) uac -= 5;
	if (uarmh && uarmh->oartifact == ART_HELM_OF_THE_DARK_LORD) uac -= 5;
	if (uarmg && uarmg->oartifact == ART_MOLASS_TANK) uac -= 10;
	if (uarms && uarms->oartifact == ART_AEAEAEAEAEGIS) uac -= 10;
	if (uarms && uarms->oartifact == ART_WHANG_CLINK_CLONK) uac -= 5;
	if (uarmf && uarmf->oartifact == ART_I_M_A_BITCH__DEAL_WITH_IT) uac -= 5;
	if (uleft && uleft->oartifact == ART_CRYLOCK) uac -= 10;
	if (uright && uright->oartifact == ART_CRYLOCK) uac -= 10;
	if (uwep && uwep->oartifact == ART_ACTA_METALLURGICA_VOL___) uac -= 5;

	if (u.negativeprotection) uac += u.negativeprotection;

	if (tech_inuse(T_IRON_SKIN)) uac -= u.ulevel;

	if (u.tunnelized) uac += 20;

	if (u.burrowed) {
		uac -= 20;
		if (!(AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || have_unskilledstone())) {
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

	if (u.berserktime) {
		int difference = (-(uac - 10));
		difference = difference / 5;
		if (difference > 0) uac = 10 - difference;
		else uac = 10;
		
	}

	if (u.uprops[NAKEDNESS].extrinsic || Nakedness || have_nakedstone() ) uac = 10;

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
	if (u.twoweap && otmp) {
		otherwep = is_sword(otmp) ? c_sword :
		    makesingular(oclass_names[(int)otmp->oclass]);
		Your("%s %sslips from your %s.",
			otherwep,
			xfl ? "also " : "",
			makeplural(body_part(HAND)));
		setuswapwep((struct obj *)0, FALSE);
		xfl++;
		if ( (otmp->otyp != LOADSTONE && otmp->otyp != HEALTHSTONE && otmp->otyp != LUCKSTONE && otmp->otyp != MANASTONE && otmp->otyp != SLEEPSTONE && otmp->otyp != LOADBOULDER && otmp->otyp != STARLIGHTSTONE && otmp->otyp != STONE_OF_MAGIC_RESISTANCE && !is_nastygraystone(otmp) ) || !otmp->cursed)
			dropx(otmp);
	}
	otmp = uwep;
	if (otmp && !welded(otmp)) {
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
		setuwep((struct obj *)0, FALSE);
		if ( (otmp->otyp != LOADSTONE && otmp->otyp != HEALTHSTONE && otmp->otyp != LUCKSTONE && otmp->otyp != MANASTONE && otmp->otyp != SLEEPSTONE && otmp->otyp != LOADBOULDER && otmp->otyp != STARLIGHTSTONE && otmp->otyp != STONE_OF_MAGIC_RESISTANCE && !is_nastygraystone(otmp) ) || !otmp->cursed)
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
#ifdef TOURIST
	if (!otmph)
	    otmph = (victim == &youmonst) ? uarmu : which_armor(victim, W_ARMU);
#endif
	
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

	/* special ring checks */
	if (otmp == uright || otmp == uleft) {
	    if (nolimbs(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
		pline_The("ring is stuck.");
		return 0;
	    }
	    why = 0;	/* the item which prevents ring removal */
	    if (welded(uwep) && (otmp == uright || bimanual(uwep))) {
		Sprintf(buf, "free a weapon %s", body_part(HAND));
		why = uwep;
	    } else if (uarmg && !FingerlessGloves && uarmg->cursed) {
		Sprintf(buf, "take off your %s", c_gloves);
		why = uarmg;
	    }
	    if (why) {
		You("cannot %s to remove the ring.", buf);
		why->bknown = TRUE;
		return 0;
	    }
	}
	/* special glove checks */
	if (otmp == uarmg) {
	    if (welded(uwep)) {
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
#ifdef TOURIST
			|| otmp == uarmu
#endif
		) {
	    why = 0;	/* the item which prevents disrobing */
	    if (uarmc && uarmc->cursed) {
		Sprintf(buf, "remove your %s", cloak_simple_name(uarmc));
		why = uarmc;
#ifdef TOURIST
	    } else if (otmp == uarmu && uarm && uarm->cursed) {
		Sprintf(buf, "remove your %s", c_suit);
		why = uarm;
#endif
	    } else if (welded(uwep) && bimanual(uwep)) {
		Sprintf(buf, "release your %s",
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
#ifdef TOURIST
	else if(otmp == uarmu) takeoff_mask |= WORN_SHIRT;
#endif
	else if(otmp == uleft) takeoff_mask |= LEFT_RING;
	else if(otmp == uright) takeoff_mask |= RIGHT_RING;
	else if(otmp == uamul) takeoff_mask |= WORN_AMUL;
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
	    setuwep((struct obj *) 0, TRUE);
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
#ifdef TOURIST
	} else if (taking_off == WORN_SHIRT) {
	  otmp = uarmu;
	  if (!cursed(otmp)) (void) Shirt_off();
#endif
	} else if (taking_off == WORN_AMUL) {
	  otmp = uamul;
	  if(!cursed(otmp)) Amulet_off();
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
#ifdef TOURIST
	} else if (taking_off == WORN_SHIRT) {
	  otmp = uarmu;
	  /* add the time to take off and put back on armor and/or cloak */
	  if (uarm)  todelay += 2 * objects[uarm->otyp].oc_delay;
	  if (uarmc) todelay += 2 * objects[uarmc->otyp].oc_delay + 1;
#endif
	} else if (taking_off == WORN_AMUL) {
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

    if (taking_off || takeoff_mask) {
	You("continue %s.", disrobing);
	set_occupation(take_off, disrobing, 0);
	return 0;
    } else if (!uwep && !uswapwep && !uquiver && !uamul && !ublindf &&
		!uleft && !uright && !wearing_armor()) {
	You("are not wearing anything.");
	return 0;
    }

    add_valid_menu_class(0); /* reset */
    if (flags.menu_style != MENU_TRADITIONAL ||
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
    } else if (flags.menu_style == MENU_FULL) {
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
	free((genericptr_t) pick_list);
    } else if (flags.menu_style == MENU_COMBINATION) {
	all_worn_categories = FALSE;
	if (ggetobj("take off", select_off, 0, TRUE, (unsigned *)0) == -2)
	    all_worn_categories = TRUE;
    }

    n = query_objlist("What do you want to take off?", invent,
			SIGNAL_NOMENU|USE_INVLET|INVORDER_SORT,
			&pick_list, PICK_ANY,
			all_worn_categories ? is_worn : is_worn_by_type);
    if (n > 0) {
	for (i = 0; i < n; i++)
	    (void) select_off(pick_list[i].item.a_obj);
	free((genericptr_t) pick_list);
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
#define DESTROY_ARM(o) ((otmp = (o)) != 0 && \
			(!atmp || atmp == otmp) && \
			(!obj_resists(otmp, 0, 90)))

	if (DESTROY_ARM(uarmc)) {
		if (donning(otmp)) cancel_don();
		Your("%s crumbles and turns to dust!",
		     cloak_simple_name(uarmc));
		(void) Cloak_off();
		useup(otmp);
	} else if (DESTROY_ARM(uarm)) {
		if (donning(otmp)) cancel_don();
		Your("armor turns to dust and falls to the %s!",
			surface(u.ux,u.uy));
		(void) Armor_gone();
		useup(otmp);
#ifdef TOURIST
	} else if (DESTROY_ARM(uarmu)) {
		if (donning(otmp)) cancel_don();
		Your("shirt crumbles into tiny threads and falls apart!");
		(void) Shirt_off();
		useup(otmp);
#endif
	} else if (DESTROY_ARM(uarmh)) {
		if (donning(otmp)) cancel_don();
		Your("helmet turns to dust and is blown away!");
		(void) Helmet_off();
		useup(otmp);
	} else if (DESTROY_ARM(uarmg)) {
		if (donning(otmp)) cancel_don();
		Your("gloves vanish!");
		(void) Gloves_off();
		useup(otmp);
		selftouch("You");
	} else if (DESTROY_ARM(uarmf)) {
		if (donning(otmp)) cancel_don();
		Your("boots disintegrate!");
		(void) Boots_off();
		useup(otmp);
	} else if (DESTROY_ARM(uarms)) {
		if (donning(otmp)) cancel_don();
		Your("shield crumbles away!");
		(void) Shield_off();
		useup(otmp);
	} else {
		return 0;		/* could not destroy anything */
	}

#undef DESTROY_ARM
	stop_occupation();
	return(1);
}

void
adj_abon(otmp, delta)
register struct obj *otmp;
register schar delta;
{
	if (uarmg && uarmg == otmp && otmp->otyp == GAUNTLETS_OF_DEXTERITY) {
		if (delta) {
			makeknown(uarmg->otyp);
			ABON(A_DEX) += (delta);
		}
		flags.botl = 1;
	}
	if (uarmg && uarmg == otmp && otmp->otyp == GAUNTLETS_OF_POWER) {
		if (delta) {
			makeknown(uarmg->otyp);
			ABON(A_STR) += (delta);
		}
		flags.botl = 1;
	}
	if (uarmh && uarmh == otmp && otmp->otyp == HELM_OF_BRILLIANCE) {
		if (delta) {
			makeknown(uarmh->otyp);
			ABON(A_INT) += (delta);
			ABON(A_WIS) += (delta);
		}
		flags.botl = 1;
	}
}

#endif /* OVLB */

/*do_wear.c*/
