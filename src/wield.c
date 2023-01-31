/*	SCCS Id: @(#)wield.c	3.4	2003/01/29	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

/* KMH -- Differences between the three weapon slots.
 *
 * The main weapon (uwep):
 * 1.  Is filled by the (w)ield command.
 * 2.  Can be filled with any type of item.
 * 3.  May be carried in one or both hands.
 * 4.  Is used as the melee weapon and as the launcher for
 *     ammunition.
 * 5.  Only conveys intrinsics when it is a weapon, weapon-tool,
 *     or artifact.
 * 6.  Certain cursed items will weld to the hand and cannot be
 *     unwielded or dropped.  See erodeable_wep() and will_weld()
 *     below for the list of which items apply.
 *
 * The secondary weapon (uswapwep):
 * 1.  Is filled by the e(x)change command, which swaps this slot
 *     with the main weapon.  If the "pushweapon" option is set,
 *     the (w)ield command will also store the old weapon in the
 *     secondary slot.
 * 2.  Can be field with anything that will fit in the main weapon
 *     slot; that is, any type of item.
 * 3.  Is usually NOT considered to be carried in the hands.
 *     That would force too many checks among the main weapon,
 *     second weapon, shield, gloves, and rings; and it would
 *     further be complicated by bimanual weapons.  A special
 *     exception is made for two-weapon combat.
 * 4.  Is used as the second weapon for two-weapon combat, and as
 *     a convenience to swap with the main weapon.
 * 5.  Never conveys intrinsics.
 * 6.  Cursed items never weld (see #3 for reasons), but they also
 *     prevent two-weapon combat.
 *
 * The quiver (uquiver):
 * 1.  Is filled by the (Q)uiver command.
 * 2.  Can be filled with any type of item.
 * 3.  Is considered to be carried in a special part of the pack.
 * 4.  Is used as the item to throw with the (f)ire command.
 *     This is a convenience over the normal (t)hrow command.
 * 5.  Never conveys intrinsics.
 * 6.  Cursed items never weld; their effect is handled by the normal
 *     throwing code.
 *
 * No item may be in more than one of these slots.
 */

/*STATIC_DCL int ready_weapon(struct obj *, BOOLEAN_P);*/
static const char all_count[] = { ALLOW_COUNT, ALL_CLASSES, 0 };
static const char allnoncount[] = { ALL_CLASSES, 0 };

/* used by will_weld() */
/* probably should be renamed */
#define erodeable_wep(optr)	((optr)->oclass == WEAPON_CLASS \
				|| is_weptool(optr) \
				|| (optr)->oclass == BALL_CLASS \
				|| (optr)->oclass == GEM_CLASS \
				|| (optr)->oclass == VENOM_CLASS \
				|| (optr)->oclass == CHAIN_CLASS)

/* used by welded(), and also while wielding */
#define will_weld(optr)		((optr)->cursed \
				&& (erodeable_wep(optr) \
				   || (optr)->otyp == TIN_OPENER || (optr)->otyp == BUDO_NO_SASU))


/*** Functions that place a given item in a slot ***/
/* Proper usage includes:
 * 1.  Initializing the slot during character generation or a
 *     restore.
 * 2.  Setting the slot due to a player's actions.
 * 3.  If one of the objects in the slot are split off, these
 *     functions can be used to put the remainder back in the slot.
 * 4.  Putting an item that was thrown and returned back into the slot.
 * 5.  Emptying the slot, by passing a null object.  NEVER pass
 *     zeroobj!
 *
 * If the item is being moved from another slot, it is the caller's
 * responsibility to handle that.  It's also the caller's responsibility
 * to print the appropriate messages.
 *
 * MRKR: It now takes an extra flag put_away which is true if the 
 *       unwielded weapon is being put back into the inventory 
 *       (rather than dropped, destroyed, etc)
 */
void
setuwep(obj, put_away, cancurseshit)
register struct obj *obj;
boolean put_away;
boolean cancurseshit; /* otherwise, saving and loading would trigger it every time! --Amy */
{
	struct obj *olduwep = uwep;

	if (obj == uwep) return; /* necessary to not set unweapon */
	/* This message isn't printed in the caller because it happens
	 * *whenever* Sunsword is unwielded, from whatever cause.
	 */
	setworn(obj, W_WEP);
	if (uwep == obj && olduwep && (olduwep->oartifact == ART_SUNSWORD || olduwep->oartifact == ART_SUNSCREEN || olduwep->oartifact == ART_SUNTINOPENER || olduwep->oartifact == ART_SUNRUBBERHOSE) &&
		olduwep->lamplit) {
	    end_burn(olduwep, FALSE);
	    if (!Blind) pline("%s glowing.", Tobjnam(olduwep, "stop"));
	}
	if (uwep && obj && olduwep && olduwep->oartifact == ART_WIRE_OF_LUCK) set_moreluck();

	/* Note: Explicitly wielding a pick-axe will not give a "bashing"
	 * message.  Wielding one via 'a'pplying it will.
	 * 3.2.2:  Wielding arbitrary objects will give bashing message too.
	 */
	if (obj) {
		unweapon = (obj->oclass == WEAPON_CLASS) ?
				is_launcher(obj) || is_ammo(obj) ||
				is_missile(obj) || (is_pole(obj) && !u.usteed
				) : !is_weptool(obj);
	} else
		unweapon = TRUE;	/* for "bare hands" message */

	if (uwep && uwep->oartifact == ART_WIRE_OF_LUCK) set_moreluck();

	if (!cancurseshit) goto cursingdone;

	if (uwep && objects[uwep->otyp].oc_skill == P_TRIDENT && Race_if(PM_NEMESIS) && !uwep->nemtrident && uwep->spe < 1) {
		uwep->nemtrident = 1;
		uwep->spe += rne(2);
		pline_The("trident glows in your %s for a moment.", body_part(HAND));
		if (uwep->spe > 120) uwep->spe = 120; /* fail safe */
	}

	if (uwep && uwep->oartifact == ART_WHY_ALWAYS_CONUNDRUM && objects[uwep->otyp].oc_material == MT_CONUNDRUM) {
		objects[uwep->otyp].oc_material = rn2(LASTMATERIAL + 1);
		Your("weapon's material morphs to a different one!");
	}

	if (uwep && uwep->otyp == HONOR_KATANA && !uwep->cursed) {
		curse(uwep);
		Your("katana welds itself to your %s!", body_part(HAND));
	}

	if (uwep && uwep->oartifact == ART_RIDGET_PHASTO) {
		curse(uwep);
		uwep->hvycurse = uwep->prmcurse = uwep->stckcurse = TRUE;
	}

	if (uwep && uwep->oartifact == ART_HOL_ON_MAN && !uwep->cursed) {
		curse(uwep);
	}

	if (uwep && uwep->oartifact == ART_ALASSEA_TELEMNAR && !uwep->hvycurse) {
		curse(uwep);
		uwep->hvycurse = 1;
		pline("A terrible black aura surrounds your sickle...");
	}

	if (uwep && uwep->oartifact == ART_SEXCALIBUR && !uwep->hvycurse) {
		curse(uwep);
		uwep->hvycurse = 1;
		pline("A terrible black aura surrounds your whip...");
	}

	if (uwep && uwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON && !uwep->cursed) {
		curse(uwep);
	}
	if (uwep && uwep->oartifact == ART_WAR_S_SWORD && !uwep->cursed) {
		curse(uwep);
		pline("Whoops, the weapon cursed itself.");
	}
	if (uwep && uwep->oartifact == ART_GONDOLIN_S_HIDDEN_PASSAGE && !uwep->cursed) {
		curse(uwep);
		pline("Whoops, the weapon cursed itself.");
	}
	if (uwep && uwep->oartifact == ART_GODAWFUL_ENCHANTMENT && !uwep->cursed) {
		curse(uwep);
		pline("Whoops, the weapon cursed itself.");
	}
	if (uwep && uwep->oartifact == ART_BARDICHE_ASSALT && !uwep->cursed) {
		curse(uwep);
		pline("Whoops, the weapon cursed itself.");
	}
	if (uwep && uwep->oartifact == ART_RASSCHEN_TAAK && !uwep->cursed) {
		curse(uwep);
	}
	if (uwep && uwep->oartifact == ART_YOU_RE_STUCCO && !uwep->cursed) {
		curse(uwep);
		pline("Great. You're stucco now.");
	}

	if (uwep && uwep->oartifact == ART_CERULEAN_SMASH && !Role_if(PM_HUSSY) && !uwep->hvycurse) {
		curse(uwep);
		uwep->hvycurse = 1;
		if (uwep->spe > -10) uwep->spe = -10;
	}

	if (uwep && uwep->oartifact == ART_SANDRA_S_EVIL_MINDDRILL) {
		if (uwep->spe > -20) uwep->spe--;
	}

	if (uwep && uwep->oartifact == ART_HENRIETTA_S_MISTAKE && !uwep->hvycurse) {
		curse(uwep);
		uwep->hvycurse = 1;
		if (!strncmpi(plname, "Henrietta", 10) || !strncmpi(plalias, "Henrietta", 10)) pline("Dear Henrietta, the dogs produced plenty of heaps of shit specially for you to step into! Please make sure you don't miss any of them.");
		else pline("Apparently you want to repeat Henrietta's mistake. Oh well. Good luck avoiding all the heaps of shit now!");
	}

	if (uwep && uwep->oartifact == ART_BLADE_OF_GOTHMOG && !uwep->hvycurse) {
		curse(uwep);
		uwep->hvycurse = 1;
		pline("A terrible black aura surrounds your sword...");
	}

	if (uwep && uwep->oartifact == ART_ARABELLA_S_MELEE_POWER) {
	    (void) makemon(&mons[PM_GUNNHILD_S_GENERAL_STORE], 0, 0, NO_MM_FLAGS);
	}

	if (uwep && uwep->oartifact == ART_ARABELLA_S_WARDING_HOE) {
		curse(uwep);
		uwep->hvycurse = uwep->prmcurse = uwep->evilcurse = 1;
	}

	if (uwep && uwep->oartifact == ART_ATOMIC_MISSING) {
		curse(uwep);
		pline("The ballista becomes cursed as you wield it.");
	}

	if (uwep && uwep->oartifact == ART_XIUHCOATL) {
		curse(uwep);
		pline("The atlatl becomes cursed as you wield it.");
	}

	if (uwep && uwep->oartifact == ART_FADED_USELESSNESS) {
		curse(uwep);
		pline("In order to prevent it from falling off, your knife curses itself.");
	}

	if (uwep && uwep->oartifact == ART_THRANDUIL_LOSSEHELIN && !uwep->hvycurse) {
		curse(uwep);
		uwep->hvycurse = 1;
		pline("A terrible black aura surrounds your weapon...");
	}

	if (uwep && uwep->oartifact == ART_MANUELA_S_PRACTICANT_TERRO && !uwep->cursed) {
		curse(uwep);
		pline("The riding crop welds itself to your %s and forces you to terrorize innocent practicants!", body_part(HAND));
	}

	if (uwep && uwep->oartifact == ART_LUISA_S_CHARMING_BEAUTY && !uwep->cursed) {
		curse(uwep);
		pline("Luisa's beautiful high-heeled lady boot refuses to be put away. Use it wisely!");
	}

	if (uwep && uwep->oartifact == ART_AND_IT_KEEPS_ON_MOVING && !uwep->cursed) {
		curse(uwep);
		pline("Your weapon is cursed now, and you will keep on moving.");
	}

	if (uwep && uwep->oartifact == ART_AMY_S_FIRST_GIRLFRIEND && !uwep->cursed) {
		curse(uwep);
		pline("You let out a deep sigh as the beautiful, soft girl shoe welds itself to your %s.", body_part(HAND));
	}

	if (uwep && uwep->oartifact == ART_BANG_BANG && uwep->spe < 2) uwep->spe = 2;

	if (uwep && uwep->oartifact == ART_DEADLY_GAMBLING && !rn2(100) ) {
		u.youaredead = 1;
		pline("BANG! You die.");
		killer_format = KILLED_BY;
		killer = "deadly gambling";
		done(DIED);
		u.youaredead = 0;
	}

	if (uwep && uwep->oartifact == ART_GUN_CONTROL_LAWS && !uwep->cursed) {
		curse(uwep);
		pline("Oh no! Morgoth curses your gun as you wield it!");
	}
	
	if (uwep && uwep->oartifact == ART_OVERHEATER && !uwep->cursed) {
		curse(uwep);
		pline("Your weapon becomes cursed!");
	}

	if (uwep && uwep->oartifact == ART_KINGS_RANSOM_FOR_YOU) {
		curse(uwep);
		uwep->hvycurse = uwep->prmcurse = uwep->evilcurse = 1;
		pline("You realize that you've made a horrible mistake.");
	}

	if (uwep && (AutocursingEquipment || u.uprops[AUTOCURSE_EQUIP].extrinsic || have_autocursestone())) curse(uwep);
	
	if (uwep && uwep->spe > -10 && (TrashingBugEffect || u.uprops[TRASHING_EFFECT].extrinsic || have_trashstone())) uwep->spe--;

cursingdone:

	/* MRKR: Handle any special effects of unwielding a weapon */
	if (olduwep && olduwep != uwep)
	    unwield(olduwep, put_away);

	update_inventory();
}

void
swaptech()
{
	register struct obj *otmp;
	struct obj *oldswapwep = uswapwep;

	if (CannotSelectItemsInPrompts) return;
swapweaponchoice:
	otmp = getobj(allnoncount, "put into your swap weapon slot");
	if (!otmp) {
		if (yn("Really exit with no object selected?") == 'y')
			pline("You just wasted the opportunity to put something into your swap weapon slot.");
		else goto swapweaponchoice;
		pline("Nothing selected.");
		return;
	}
	if (otmp->owornmask) {
		You("can't set an item as your secondary weapon that's already equipped in a different slot!");
		return;
	}

	if (oldswapwep)
	    unwield(oldswapwep, TRUE);

	setworn(otmp, W_SWAPWEP);
	update_inventory();

	if (PlayerCannotUseSkills || (P_SKILL(P_TWO_WEAPON_COMBAT) < P_MASTER) ) {

		if (uswapwep && uswapwep->oartifact == ART_ALASSEA_TELEMNAR && !uswapwep->hvycurse) {
			curse(uswapwep);
			uswapwep->hvycurse = 1;
			pline("A terrible black aura surrounds your sickle...");
		}

		if (uswapwep && uswapwep->otyp == HONOR_KATANA && !uswapwep->cursed) {
			curse(uswapwep);
			Your("katana welds itself to your other %s!", body_part(HAND));
		}

		if (uswapwep && uswapwep->oartifact == ART_RIDGET_PHASTO) {
			curse(uswapwep);
			uswapwep->hvycurse = uswapwep->prmcurse = uswapwep->stckcurse = TRUE;
		}

		if (uswapwep && uswapwep->oartifact == ART_HOL_ON_MAN && !uswapwep->cursed) {
			curse(uswapwep);
		}

		if (uswapwep && uswapwep->oartifact == ART_SEXCALIBUR && !uswapwep->hvycurse) {
			curse(uswapwep);
			uswapwep->hvycurse = 1;
			pline("A terrible black aura surrounds your whip...");
		}

		if (uswapwep && uswapwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON && !uswapwep->cursed) {
			curse(uswapwep);
		}
		if (uswapwep && uswapwep->oartifact == ART_WAR_S_SWORD && !uswapwep->cursed) {
			curse(uswapwep);
			pline("Whoops, the weapon cursed itself.");
		}
		if (uswapwep && uswapwep->oartifact == ART_GONDOLIN_S_HIDDEN_PASSAGE && !uswapwep->cursed) {
			curse(uswapwep);
			pline("Whoops, the weapon cursed itself.");
		}
		if (uswapwep && uswapwep->oartifact == ART_GODAWFUL_ENCHANTMENT && !uswapwep->cursed) {
			curse(uswapwep);
			pline("Whoops, the weapon cursed itself.");
		}
		if (uswapwep && uswapwep->oartifact == ART_BARDICHE_ASSALT && !uswapwep->cursed) {
			curse(uswapwep);
			pline("Whoops, the weapon cursed itself.");
		}
		if (uswapwep && uswapwep->oartifact == ART_RASSCHEN_TAAK && !uswapwep->cursed) {
			curse(uswapwep);
		}
		if (uswapwep && uswapwep->oartifact == ART_YOU_RE_STUCCO && !uswapwep->cursed) {
			curse(uswapwep);
			pline("Great. You're stucco now.");
		}

		if (uswapwep && uswapwep->oartifact == ART_CERULEAN_SMASH && !Role_if(PM_HUSSY) && !uswapwep->hvycurse) {
			curse(uswapwep);
			uswapwep->hvycurse = 1;
			if (uswapwep->spe > -10) uswapwep->spe = -10;
		}

		if (uswapwep && uswapwep->oartifact == ART_SANDRA_S_EVIL_MINDDRILL) {
			if (uswapwep->spe > -20) uswapwep->spe--;
		}

		if (uswapwep && uswapwep->oartifact == ART_HENRIETTA_S_MISTAKE && !uswapwep->hvycurse) {
			curse(uswapwep);
			uswapwep->hvycurse = 1;
			if (!strncmpi(plname, "Henrietta", 10) || !strncmpi(plalias, "Henrietta", 10)) pline("Dear Henrietta, the dogs produced plenty of heaps of shit specially for you to step into! Please make sure you don't miss any of them.");
			else pline("Apparently you want to repeat Henrietta's mistake. Oh well. Good luck avoiding all the heaps of shit now!");
		}

		if (uswapwep && uswapwep->oartifact == ART_BLADE_OF_GOTHMOG && !uswapwep->hvycurse) {
			curse(uswapwep);
			uswapwep->hvycurse = 1;
			pline("A terrible black aura surrounds your sword...");
		}

		if (uswapwep && uswapwep->oartifact == ART_ARABELLA_S_MELEE_POWER) {
		    (void) makemon(&mons[PM_GUNNHILD_S_GENERAL_STORE], 0, 0, NO_MM_FLAGS);
		}

		if (uswapwep && uswapwep->oartifact == ART_ARABELLA_S_WARDING_HOE) {
			curse(uswapwep);
			uswapwep->hvycurse = uswapwep->prmcurse = uswapwep->evilcurse = 1;
		}

		if (uswapwep && uswapwep->oartifact == ART_ATOMIC_MISSING) {
			curse(uswapwep);
			pline("The ballista becomes cursed as you wield it.");
		}

		if (uswapwep && uswapwep->oartifact == ART_XIUHCOATL) {
			curse(uswapwep);
			pline("The atlatl becomes cursed as you wield it.");
		}

		if (uswapwep && uswapwep->oartifact == ART_FADED_USELESSNESS) {
			curse(uswapwep);
			pline("In order to prevent it from falling off, your knife curses itself.");
		}

		if (uswapwep && uswapwep->oartifact == ART_THRANDUIL_LOSSEHELIN && !uswapwep->hvycurse) {
			curse(uswapwep);
			uswapwep->hvycurse = 1;
			pline("A terrible black aura surrounds your weapon...");
		}

		if (uswapwep && uswapwep->oartifact == ART_MANUELA_S_PRACTICANT_TERRO && !uswapwep->cursed) {
			curse(uswapwep);
			pline("The riding crop glows with an evil aura and forces you to terrorize innocent practicants!");
		}

		if (uswapwep && uswapwep->oartifact == ART_LUISA_S_CHARMING_BEAUTY && !uswapwep->cursed) {
			curse(uswapwep);
			pline("Luisa's beautiful high-heeled lady boot refuses to be put away. Use it wisely!");
		}

		if (uswapwep && uswapwep->oartifact == ART_AND_IT_KEEPS_ON_MOVING && !uswapwep->cursed) {
			curse(uswapwep);
			pline("Your weapon is cursed now, and you will keep on moving.");
		}

		if (uswapwep && uswapwep->oartifact == ART_AMY_S_FIRST_GIRLFRIEND && !uswapwep->cursed) {
			curse(uswapwep);
			pline("You let out a deep sigh as the beautiful, soft girl shoe becomes cursed.");
		}

		if (uswapwep && uswapwep->oartifact == ART_BANG_BANG && uswapwep->spe < 2) uswapwep->spe = 2;

		if (uswapwep && uswapwep->oartifact == ART_DEADLY_GAMBLING && !rn2(100) ) {
			u.youaredead = 1;
			pline("BANG! You die.");
			killer_format = KILLED_BY;
			killer = "deadly gambling";
			done(DIED);
			u.youaredead = 0;
		}

		if (uswapwep && uswapwep->oartifact == ART_GUN_CONTROL_LAWS && !uswapwep->cursed) {
			curse(uswapwep);
			pline("Oh no! Morgoth curses your gun as you wield it!");
		}
	
		if (uswapwep && uswapwep->oartifact == ART_OVERHEATER && !uswapwep->cursed) {
			curse(uswapwep);
			pline("Your weapon becomes cursed!");
		}

		if (uswapwep && uswapwep->oartifact == ART_KINGS_RANSOM_FOR_YOU) {
			curse(uswapwep);
			uswapwep->hvycurse = uswapwep->prmcurse = uswapwep->evilcurse = 1;
			pline("You realize that you've made a horrible mistake.");
		}

		if (uswapwep && (AutocursingEquipment || u.uprops[AUTOCURSE_EQUIP].extrinsic || have_autocursestone())) curse(uswapwep);
	
		if (uswapwep && uswapwep->spe > -10 && (TrashingBugEffect || u.uprops[TRASHING_EFFECT].extrinsic || have_trashstone())) uswapwep->spe--;

	}

}

int
ready_weapon(wep, put_away)
struct obj *wep;
boolean put_away;
{
	/* Separated function so swapping works easily */
	int res = 0;

	if (!wep) {
	    /* No weapon */
	    if (uwep) {
		You("are empty %s.", body_part(HANDED));
		setuwep((struct obj *) 0, put_away, TRUE);
		/* You can just drop your weapon and pick it back up in zero turns, so unwielding something should not
		 * take time either.
		 * Except in Soviet Russia of course, where dropping and picking up items isn't free, because nothing done
		 * by the player is allowed to be free in communism. --Amy */
		if (issoviet) {
			res++;
			pline("Teper' vy s pustymi rukami. Eto stoit povorot, tak kak tip bloka l'da KHE-KHE nenavidit vas.");
		}
	    } else
		You("are already empty %s.", body_part(HANDED));
	} else if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && wep->otyp == CORPSE
				&& touch_petrifies(&mons[wep->corpsenm])) {
	    /* Prevent wielding cockatrice when not wearing gloves --KAA */
	    char kbuf[BUFSZ];

	    You("wield the %s corpse in your bare %s.",
		mons[wep->corpsenm].mname, makeplural(body_part(HAND)));
	    sprintf(kbuf, "%s corpse", an(mons[wep->corpsenm].mname));
	    instapetrify(kbuf);
	} else if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && wep->otyp == EGG && wep->corpsenm != PM_PLAYERMON
				&& touch_petrifies(&mons[wep->corpsenm])) {
	    /* Prevent wielding cockatrice when not wearing gloves --KAA */
	    char kbuf[BUFSZ];

	    You("wield the %s egg in your bare %s.",
		mons[wep->corpsenm].mname, makeplural(body_part(HAND)));
	    sprintf(kbuf, "%s egg", an(mons[wep->corpsenm].mname));
	    instapetrify(kbuf);
	} else if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && wep->otyp == PETRIFYIUM_BAR) {
	    /* Prevent wielding cockatrice when not wearing gloves --KAA */
	    char kbuf[BUFSZ];

	    You("wield the petrifyium bar in your bare %s.", makeplural(body_part(HAND)));
	    sprintf(kbuf, "petrifyium bar");
	    instapetrify(kbuf);
	} else if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && wep->otyp == PETRIFYIUM_BRA) {
	    /* Prevent wielding cockatrice when not wearing gloves --KAA */
	    char kbuf[BUFSZ];

	    You("wield the petrifyium bra in your bare %s.", body_part(HAND));
	    sprintf(kbuf, "petrifyium bra");
	    instapetrify(kbuf);
	} else if (uarms && (uarms->otyp != GRIM_SHIELD && !Race_if(PM_THRALL) && !Race_if(PM_URGOTH) && !Race_if(PM_ZAUR) && !Race_if(PM_WYLVAN)) && bimanual(wep))
	    You("cannot wield a two-handed %s while wearing a shield.",
		is_sword(wep) ? "sword" :
		    wep->otyp == BATTLE_AXE ? "axe" : "weapon");

	else if (bimanual(wep) && Race_if(PM_LICH_WARRIOR) && !Upolyd )
	    pline("As a lich, you cannot wield a two-handed weapon.");
	/* Yes I know, a lich barbarian will start out wielding that axe. --Amy */

	else if (bimanual(wep) && Race_if(PM_HAND) && !Upolyd )
	    pline("As a hand, you cannot wield a two-handed weapon.");

	else if (!is_launcher(wep) && Race_if(PM_ELONA_SNAIL) && !Upolyd )
	    pline("As a snail, you can only wield launchers.");
	/* in Elona, launchers have their separate slot, but not so here --Amy */

	else if (wep->oartifact && !touch_artifact(wep, &youmonst)) {
	    res++;	/* takes a turn even though it doesn't get wielded */
	} else if (tech_inuse(T_EVISCERATE)) {
		/* WAC - if you have 'L' has claws out and wields weapon,
		 * can't retract claws
		 */
		You("can't retract your claws!");
	} else {
	    /* Weapon WILL be wielded after this point */
	    res++;
	    if (will_weld(wep)) {
		const char *tmp = xname(wep), *thestr = "The ";
		if (strncmp(tmp, thestr, 4) && !strncmp(The(tmp),thestr,4))
		    tmp = thestr;
		else tmp = "";
		pline("%s%s %s to your %s!", tmp, aobjnam(wep, "weld"),
			(wep->quan == 1L) ? "itself" : "themselves", /* a3 */
			bimanual(wep) ?
				(const char *)makeplural(body_part(HAND))
				: body_part(HAND));
		wep->bknown = TRUE;
	    } else {
		/* The message must be printed before setuwep (since
		 * you might die and be revived from changing weapons),
		 * and the message must be before the death message and
		 * Lifesaved rewielding.  Yet we want the message to
		 * say "weapon in hand", thus this kludge.
		 */
		long dummy = wep->owornmask;
		wep->owornmask |= W_WEP;
		if (Race_if(PM_ELEMENTAL) && wep && weapon_type(wep) != P_NONE) wep->known = TRUE;	/* elementals will recognize enchantment --Amy */
		prinv((char *)0, wep, 0L);
		wep->owornmask = dummy;
	    }
	    setuwep(wep, put_away, TRUE);

	    /* KMH -- Talking artifacts are finally implemented */
	    arti_speak(wep);

	    if ((wep->oartifact == ART_SUNSWORD || wep->oartifact == ART_SUNSCREEN || wep->oartifact == ART_SUNTINOPENER || wep->oartifact == ART_SUNRUBBERHOSE) && !wep->lamplit) {
		begin_burn(wep, FALSE);
		if (!Blind)
		    pline("%s to glow brilliantly!", Tobjnam(wep, "begin"));
	    }

	    if (wep->oartifact == ART_WIRE_OF_LUCK) set_moreluck();

#if 0
	    /* we'll get back to this someday, but it's not balanced yet */
	    if (Race_if(PM_ELF) && !wep->oartifact &&
			    objects[wep->otyp].oc_material == MT_IRON) {
		/* Elves are averse to wielding cold iron */
		You("have an uneasy feeling about wielding cold iron.");
		change_luck(-1);
	    }
#endif

	    if (wep->unpaid) {
		struct monst *this_shkp;

		if ((this_shkp = shop_keeper(inside_shop(u.ux, u.uy))) !=
		    (struct monst *)0) {
		    pline("%s says \"You be careful with my %s!\"",
			  shkname(this_shkp),
			  xname(wep));
		}
	    }
	}
	return(res);
}

void
setuqwep(obj)
register struct obj *obj;
{
	setworn(obj, W_QUIVER);
	update_inventory();
}

void
setuswapwep(obj, put_away)
register struct obj *obj;
boolean put_away;
{
	struct obj *oldswapwep = uswapwep;
	setworn(obj, W_SWAPWEP);

	if (oldswapwep && oldswapwep != uswapwep)
	    unwield(oldswapwep, put_away);
	update_inventory();
}


/*** Commands to change particular slot(s) ***/

static NEARDATA const char wield_objs[] =
	{ ALL_CLASSES, ALLOW_NONE, WEAPON_CLASS, TOOL_CLASS, BALL_CLASS, CHAIN_CLASS, VENOM_CLASS, 0 };
static NEARDATA const char ready_objs[] =
	{ ALL_CLASSES, ALLOW_NONE, WEAPON_CLASS, VENOM_CLASS, 0 };
static NEARDATA const char bullets[] =	/* (note: different from dothrow.c) */
	{ ALL_CLASSES, ALLOW_NONE, GEM_CLASS, WEAPON_CLASS, VENOM_CLASS, 0 };

int
dowield()
{
	register struct obj *wep, *oldwep;
	int result;

	/* May we attempt this? */
	multi = 0;
	if (cantwield(youmonst.data) && !Race_if(PM_TRANSFORMER) && !Race_if(PM_HUMAN_WRAITH) ) {
		pline("Don't be ridiculous! Your current form cannot realistically wield a weapon!");

		if (yn("Try anyway?") == 'y') {
			if (rn2(3) && !polyskillchance()) {
		 		make_confused(HConfusion + rnd(40),FALSE);
				pline("Uhh... that didn't seem to work.");
				if (!rn2(20)) badeffect();
				return 1;
			}
		}
		else {return(0);}

	}

	if (Race_if(PM_HUMAN_WRAITH) && (u.uhpmax < 2 || u.uhp < 2) ) {pline("You don't have enough health to wield weapons!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}

	if (Race_if(PM_HUMAN_WRAITH)) {

		if (yn("Wielding a weapon as a wraith will permanently damage your health. Do it anyway?") == 'y') {

			u.uhp -= 1;
			u.uhpmax -= 1;
			if (Upolyd) {
				u.mh -= 1;
			u.mhmax -= 1;
			}

		}
		else return 0;

	}

	/* Prompt for a new weapon */
	if (!(wep = getobj(wield_objs, "wield")))
		/* Cancelled */
		return (0);
	else if (wep == uwep) {
	    You("are already wielding that!");
	    if (is_weptool(wep)) unweapon = FALSE;	/* [see setuwep()] */
		return (0);
	} else if (welded(uwep)) {
		weldmsg(uwep);
		/* previously interrupted armor removal mustn't be resumed */
		reset_remarm();
		return (0);
	}

	/* Handle no object, or object in other slot */
	if (wep == &zeroobj)
		wep = (struct obj *) 0;
	else if (wep == uswapwep && !Race_if(PM_HUMAN_WRAITH) && !cantwield(youmonst.data) )
		return (doswapweapon());
	else if (wep == uswapwep && Race_if(PM_HUMAN_WRAITH) ) {

		pline("You now have no secondary weapon readied.");
		setuswapwep((struct obj *) 0, FALSE);
	}
	else if (wep == uquiver)
		setuqwep((struct obj *) 0);
	else if (wep->owornmask & (W_ARMOR | W_RING | W_AMUL | W_IMPLANT | W_TOOL
			| W_SADDLE
			)) {
		You("cannot wield that!");
		return (0);
	}

	/* Set your new primary weapon */
	oldwep = uwep;
	result = ready_weapon(wep, TRUE);
	if (flags.pushweapon && oldwep && uwep != oldwep)
		setuswapwep(oldwep, TRUE);
	untwoweapon();
	if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void)doredraw();

	return (result);
}

int
doswapweapon()
{
	register struct obj *oldwep, *oldswap;
	int result = 0;


	/* May we attempt this? */
	multi = 0;
	if (cantwield(youmonst.data) && !Race_if(PM_TRANSFORMER) && !Race_if(PM_HUMAN_WRAITH) ) {

		pline("Don't be ridiculous! Your current form cannot realistically wield a weapon!");

		if (yn("Try anyway?") == 'y') {
			if (rn2(3) && !polyskillchance()) {
		 		make_confused(HConfusion + rnd(40),FALSE);
				pline("Uhh... that didn't seem to work.");
				if (!rn2(20)) badeffect();
				return 1;
			}
		}
		else {return(0);}
	}

	if (Race_if(PM_HUMAN_WRAITH) && (u.uhpmax < 2 || u.uhp < 2) ) {pline("You don't have enough health to wield weapons!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}

	if (Race_if(PM_HUMAN_WRAITH)) {

		if (yn("Wielding a weapon as a wraith will permanently damage your health. Do it anyway?") == 'y') {

			u.uhp -= 1;
			u.uhpmax -= 1;
			if (Upolyd) {
				u.mh -= 1;
			u.mhmax -= 1;
			}

		}
		else return 0;

	}

	if (welded(uwep)) {
		weldmsg(uwep);
		return (0);
	}

	/* Unwield your current secondary weapon */
	oldwep = uwep;
	oldswap = uswapwep;
	if (uswapwep)
		unwield(uswapwep, FALSE);
	u.twoweap = 0;
	setuswapwep((struct obj *) 0, FALSE);

	/* Set your new primary weapon */
	result = ready_weapon(oldswap, TRUE);

	/* Set your new secondary weapon */
	if (uwep == oldwep)
		/* Wield failed for some reason */
		setuswapwep(oldswap, FALSE);
	else {
		setuswapwep(oldwep, FALSE);
		if (uswapwep)
			prinv((char *)0, uswapwep, 0L);
		else
			You("have no secondary weapon readied.");
	}

	if (u.twoweap && !can_twoweapon())
		untwoweapon();
	if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void)doredraw();

	return (result);
}

int
dowieldquiver()
{
	register struct obj *newquiver;
	const char *quivee_types = (uslinging() ||
		  (uswapwep && objects[uswapwep->otyp].oc_skill == P_SLING)) ?
				 bullets : ready_objs;

	/* Since the quiver isn't in your hands, don't check cantwield(), */
	/* will_weld(), touch_petrifies(), etc. */
	multi = 0;

	/* Slash'EM has used Q for quiver since it started */
	/* Because 'Q' used to be quit... */
	if (flags.suppress_alert < FEATURE_NOTICE_VER(0,0,0))
		pline("Note: Please use #quit if you wish to exit the game.");

	/* Prompt for a new quiver */
	if (!(newquiver = getobj(quivee_types, "ready")))
		/* Cancelled */
		return (0);

	/* Handle no object, or object in other slot */
	/* Any type is okay, since we give no intrinsics anyways */
	if (newquiver == &zeroobj) {
		/* Explicitly nothing */
		if (uquiver) {
			You("now have no ammunition readied.");
			setuqwep(newquiver = (struct obj *) 0);
		} else {
			You("already have no ammunition readied!");
			return(0);
		}
	} else if (newquiver == uquiver) {
		pline("That ammunition is already readied!");
		return(0);
	} else if (newquiver == uwep) {
		/* Prevent accidentally readying the main weapon */
		pline("%s already being used as a weapon!",
		      !is_plural(uwep) ? "That is" : "They are");
		return(0);
	} else if (newquiver->owornmask & (W_ARMOR | W_RING | W_AMUL | W_IMPLANT | W_TOOL
			| W_SADDLE
			)) {
		You("cannot ready that!");
		return (0);
	} else {
		long dummy;


		/* Check if it's the secondary weapon */
		if (newquiver == uswapwep) {
			setuswapwep((struct obj *) 0, TRUE);
			untwoweapon();
		}

		/* Okay to put in quiver; print it */
		dummy = newquiver->owornmask;
		newquiver->owornmask |= W_QUIVER;
		prinv((char *)0, newquiver, 0L);
		newquiver->owornmask = dummy;
	}

	/* Finally, place it in the quiver */
	setuqwep(newquiver);
	/* Take no time since this is a convenience slot */
	return (0);
}

/* used for #rub and for applying pick-axe, whip, grappling hook, or polearm */
/* (moved from apply.c) */
boolean
wield_tool(obj, verb)
struct obj *obj;
const char *verb;	/* "rub",&c */
{
    const char *what;
    boolean more_than_1;

    if (obj == uwep) return TRUE;   /* nothing to do if already wielding it */

    if (!verb) verb = "wield";
    what = xname(obj);
    more_than_1 = (obj->quan > 1L ||
		   strstri(what, "pair of ") != 0 ||
		   strstri(what, "s of ") != 0);

    if (obj->owornmask & (W_ARMOR|W_RING|W_AMUL|W_IMPLANT|W_TOOL)) {
	char yourbuf[BUFSZ];

	You_cant("%s %s %s while wearing %s.",
		 verb, shk_your(yourbuf, obj), what,
		 more_than_1 ? "them" : "it");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return FALSE;
    }
    if (welded(uwep)) {
	if (flags.verbose) {
	    const char *hand = body_part(HAND);

	    if (bimanual(uwep)) hand = makeplural(hand);
	    if (strstri(what, "pair of ") != 0) more_than_1 = FALSE;
	    pline(
	     "Since your weapon is welded to your %s, you cannot %s %s %s.",
		  hand, verb, more_than_1 ? "those" : "that", xname(obj));
	} else {
	    You_cant("do that.");
	}
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return FALSE;
    }
    if (cantwield(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
	You_cant("hold %s strongly enough.", more_than_1 ? "them" : "it");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return FALSE;
    }
    /* check shield */

	if (bimanual(obj) && Race_if(PM_LICH_WARRIOR) && !Upolyd ) {
	    pline("As a lich, you cannot wield a two-handed weapon.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return FALSE;
	}

	if (bimanual(obj) && Race_if(PM_HAND) && !Upolyd ) {
	    pline("As a lich, you cannot wield a two-handed weapon.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return FALSE;
	}

	if (!is_launcher(obj) && Race_if(PM_ELONA_SNAIL) && !Upolyd ) {
	    pline("As a snail, you can only wield launchers.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return FALSE;
	}

    if (uarms && (uarms->otyp != GRIM_SHIELD && !Race_if(PM_THRALL) && !Race_if(PM_URGOTH) && !Race_if(PM_ZAUR) && !Race_if(PM_WYLVAN)) && bimanual(obj)) {
	You("cannot %s a two-handed %s while wearing a shield.",
	    verb, (obj->oclass == WEAPON_CLASS) ? "weapon" : "tool");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return FALSE;
    }

	if (Race_if(PM_HUMAN_WRAITH) && (u.uhpmax < 2 || u.uhp < 2) ) {pline("You don't have enough health to wield tools!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return FALSE;
	}

	if (Race_if(PM_HUMAN_WRAITH)) {

		if (yn("Wielding a tool as a wraith will permanently damage your health. Do it anyway?") == 'y') {

			u.uhp -= 1;
			u.uhpmax -= 1;
			if (Upolyd) {
				u.mh -= 1;
			u.mhmax -= 1;
			}

		}
		else return FALSE;

	}

    if (uquiver == obj) setuqwep((struct obj *)0);
    if (uswapwep == obj) {
	(void) doswapweapon();
	/* doswapweapon might fail */
	if (uswapwep == obj) return FALSE;
    } else {
	You("now wield %s.", doname(obj));
	setuwep(obj, TRUE, TRUE);
    }
    if (uwep != obj) return FALSE;	/* rewielded old object after dying */
    /* applying weapon or tool that gets wielded ends two-weapon combat */
    if (u.twoweap)
	untwoweapon();
    if (obj->oclass != WEAPON_CLASS && !is_weptool(obj))
	unweapon = TRUE;
    return TRUE;
}

/* WAC
 * For the purposes of SLASH'EM, artifacts should be wieldable in either hand
 */
int
can_twoweapon()
{
	char buf[BUFSZ];
	const char *what;
	boolean disallowed_by_race;
	boolean disallowed_by_role;
	struct obj *otmp;

#define NOT_WEAPON(obj) (obj && !is_weptool(obj) && obj->oclass != WEAPON_CLASS && obj->oclass != BALL_CLASS && obj->oclass != GEM_CLASS && obj->oclass != CHAIN_CLASS && obj->oclass != VENOM_CLASS)
	if (!could_twoweap(youmonst.data) && !(uactivesymbiosis && mons[u.usymbiote.mnum].mattk[1].aatyp == AT_WEAP ) && (uwep || uswapwep)) {
	    what = uwep && uswapwep ? "two weapons" : "more than one weapon";
	    if (cantwield(youmonst.data) && !Race_if(PM_TRANSFORMER) )
		pline("Don't be ridiculous! Your current form has enough trouble wielding ONE weapon!");
	    else if (Upolyd)
		You_cant("use %s in your current form.", what);
	    else {
		disallowed_by_role = P_MAX_SKILL(P_TWO_WEAPON_COMBAT) < P_BASIC;
		disallowed_by_race = youmonst.data->mattk[1].aatyp != AT_WEAP;
		*buf = '\0';
		if (!disallowed_by_role)
		    strcpy(buf, disallowed_by_race ? urace.noun : urace.adj);
		if (disallowed_by_role || !disallowed_by_race) {
		    if (!disallowed_by_role)
			strcat(buf, " ");
		    strcat(buf, (flags.female && urole.name.f) ?
			    urole.name.f : urole.name.m);
		}
		pline("%s aren't able to use %s at once.",
			makeplural(upstart(buf)), what);
	    }
	} else if (cantwield(youmonst.data) && !Race_if(PM_TRANSFORMER) )
	    pline("Don't be ridiculous! Your current form has enough trouble wielding ONE weapon!");
	else if (youmonst.data->mattk[1].aatyp != AT_WEAP &&
		youmonst.data->mattk[1].aatyp != AT_CLAW &&
		!(uactivesymbiosis && mons[u.usymbiote.mnum].mattk[1].aatyp == AT_WEAP ) ) {
	    if (Upolyd)
		You_cant("fight with two %s in your current form.",
			makeplural(body_part(HAND)));
	    else
		pline("%s aren't able to fight two-handed.",
			upstart(makeplural(urace.noun)));
	} else if ((NOT_WEAPON(uwep) && issoviet) || NOT_WEAPON(uswapwep)) {
	    otmp = NOT_WEAPON(uwep) ? uwep : uswapwep;
	    pline("%s %s.", Yname2(otmp),
		is_plural(otmp) ? "aren't weapons" : "isn't a weapon");
	    if (issoviet && NOT_WEAPON(uwep)) pline("Zamedlit'! Pochemu ty voobshche igrayesh' v etu duratskuyu igru?");
	} else if ((uwep && bimanual(uwep)) || (uswapwep && bimanual(uswapwep))) {
	    otmp = (uwep && bimanual(uwep)) ? uwep : uswapwep;
	    pline("%s isn't one-handed.", Yname2(otmp));
	} else if (uarms && uarms->otyp != GRIM_SHIELD && !Race_if(PM_THRALL) && !Race_if(PM_URGOTH) && !Race_if(PM_ZAUR) && !Race_if(PM_WYLVAN)) {
	    if (uwep || uswapwep)
		what = uwep && uswapwep ?  "use two weapons" :
		    "use more than one weapon";
	    else {
		sprintf(buf, "fight with two %s", makeplural(body_part(HAND)));
		what = buf;
	    }
	    You_cant("%s while wearing a shield.", what);
	}
        /* WAC:  TODO: cannot wield conflicting alignment artifacts*/
#if 0
  	else if (uswapwep->oartifact && ...)
	    pline("%s resists being held second to another weapon!",
		    Yname2(uswapwep));
#endif
	else if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && 
		(uswapwep && uswapwep->otyp == CORPSE &&                   
                (touch_petrifies(&mons[uswapwep->corpsenm])))) {
	    char kbuf[BUFSZ];

	    You("wield the %s corpse with your bare %s.",
		    mons[uswapwep->corpsenm].mname, body_part(HAND));
	    sprintf(kbuf, "%s corpse", an(mons[uswapwep->corpsenm].mname));
	    instapetrify(kbuf);
        } 	else if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && 
		(uswapwep && uswapwep->otyp == EGG && uswapwep->corpsenm != PM_PLAYERMON &&
                (touch_petrifies(&mons[uswapwep->corpsenm])))) {
	    char kbuf[BUFSZ];

	    You("wield the %s egg with your bare %s.",
		    mons[uswapwep->corpsenm].mname, body_part(HAND));
	    sprintf(kbuf, "%s corpse", an(mons[uswapwep->corpsenm].mname));
	    instapetrify(kbuf);
        } 	else if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && 
		(uswapwep && uswapwep->otyp == PETRIFYIUM_BAR)) {
	    char kbuf[BUFSZ];

	    You("wield the petrifyium bar with your bare %s.", body_part(HAND));
	    sprintf(kbuf, "petrifyium bar");
	    instapetrify(kbuf);
        } 	else if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && 
		(uswapwep && uswapwep->otyp == PETRIFYIUM_BRA)) {
	    char kbuf[BUFSZ];

	    You("wield the petrifyium bra with your bare %s.", body_part(HAND));
	    sprintf(kbuf, "petrifyium bra");
	    instapetrify(kbuf);
        } else if (uswapwep && (IsGlib || (uswapwep->cursed && (PlayerCannotUseSkills || (P_SKILL(P_TWO_WEAPON_COMBAT) < P_EXPERT) ) ) )) {
	    if (!IsGlib)
		uswapwep->bknown = TRUE;
	    drop_uswapwep();
	} else
	    return (TRUE); /* Passes all the checks */
	
	/* Otherwise */
	return (FALSE);
}

void
drop_uswapwep()
{
	char str[BUFSZ];
	struct obj *obj = uswapwep;

	/* Avoid trashing makeplural's static buffer */
	strcpy(str, makeplural(body_part(HAND)));
	Your("%s from your %s!",  aobjnam(obj, "slip"), str);
	setuswapwep((struct obj *) 0, FALSE);
	dropx(obj);
}

int
dotwoweapon()
{
	/* You can always toggle it off */
	if (u.twoweap) {
		if (uwep)
		    You("switch to your primary weapon.");
		else if (uswapwep) {
		    You("are empty %s.", body_part(HANDED));
		    unweapon = TRUE;
		} else
		    You("switch to your right %s.", body_part(HAND));
		if (uswapwep)
		    unwield(uswapwep, TRUE);
		u.twoweap = 0;
		update_inventory();
		return (0);
	}

	/* May we use two weapons? */
	if (can_twoweapon()) {
		/* Success! */
		if (uwep && uswapwep)
		    You("begin two-weapon combat.");
		else if (uwep || uswapwep) {
		    You("begin fighting with a weapon and your %s %s.",
			    uwep ? "left" : "right", body_part(HAND));
		    unweapon = FALSE;
		} else if (Upolyd)
		    You("begin fighting with two %s.",
			    makeplural(body_part(HAND)));
		else
		    You("begin two-handed combat.");
		u.twoweap = 1;
		update_inventory();
		return (rnd(20) > ACURR(A_DEX));
	}
	return (0);
}

/*** Functions to empty a given slot ***/
/* These should be used only when the item can't be put back in
 * the slot by life saving.  Proper usage includes:
 * 1.  The item has been eaten, stolen, burned away, or rotted away.
 * 2.  Making an item disappear for a bones pile.
 */
void
uwepgone()
{
	if (uwep) {
		if (artifact_light(uwep) && uwep->lamplit) {
		    end_burn(uwep, FALSE);
		    if (!Blind) pline("%s glowing.", Tobjnam(uwep, "stop"));
		}
		unwield(uwep, FALSE);
		setworn((struct obj *)0, W_WEP);
		unweapon = TRUE;
		update_inventory();
		if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void)doredraw();
	}
}

void
uswapwepgone()
{
	if (uswapwep) {
		setworn((struct obj *)0, W_SWAPWEP);
		update_inventory();
	}
}

void
uqwepgone()
{
	if (uquiver) {
		setworn((struct obj *)0, W_QUIVER);
		update_inventory();
	}
}

void
untwoweapon()
{
	if (u.twoweap) {
		if (uwep && uswapwep)
		    You("can no longer use two weapons at once.");
		else if (cantwield(youmonst.data))
		    You("can no longer control which %s to fight with.",
			    body_part(HAND));
		else
		    You("can no longer use two %s to fight.",
			    makeplural(body_part(HAND)));
		if (uswapwep)
		    unwield(uswapwep, TRUE);
		u.twoweap = FALSE;
		update_inventory();
	}
	return;
}

/* Maybe rust object, or corrode it if acid damage is called for */
void
erode_obj(target, acid_dmg, fade_scrolls)
struct obj *target;		/* object (e.g. weapon or armor) to erode */
boolean acid_dmg;
boolean fade_scrolls;
{
	int erosion;
	struct monst *victim;
	boolean vismon;
	boolean visobj;

	if (!target)
	    return;
	victim = carried(target) ? &youmonst :
	    mcarried(target) ? target->ocarry : (struct monst *)0;
	vismon = victim && (victim != &youmonst) && canseemon(victim);
	visobj = !victim && cansee(bhitpos.x, bhitpos.y); /* assume thrown */

	erosion = acid_dmg ? target->oeroded2 : target->oeroded;

	if (stack_too_big(target)) return;

	if (target->oartifact && rn2(4)) return;

	if (itemhasappearance(target, APP_BRAND_NEW_GLOVES) && rn2(4) ) return;

	if (itemhasappearance(target, APP_IMAGINARY_HEELS) ) return;

	if (target->oartifact == ART_RATCH_CLOSURE_SCRATCHING && rn2(4) ) return;

	if (itemhasappearance(target, APP_WITHERED_CLOAK) ) return;

	if (uarmf && !rn2(2) && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) return;

	if (target->greased && (!issoviet || !rn2(2)) ) {
	    grease_protect(target,(char *)0,victim);
	} else if (target->oclass == SCROLL_CLASS) {
	    if(fade_scrolls && target->otyp != SCR_BLANK_PAPER && !target->oartifact && target->otyp != SCR_HEALING && target->otyp != SCR_EXTRA_HEALING && target->otyp != SCR_STANDARD_ID && target->otyp != SCR_HEAL_OTHER && target->otyp != SCR_MANA && target->otyp != SCR_GREATER_MANA_RESTORATION && target->otyp != SCR_CURE && target->otyp != SCR_PHASE_DOOR
#ifdef MAIL
	    && target->otyp != SCR_MAIL
#endif
					)
	    {
		if (!Blind) {
		    if (victim == &youmonst)
			Your("%s.", aobjnam(target, "fade"));
		    else if (vismon)
			pline("%s's %s.", Monnam(victim),
			      aobjnam(target, "fade"));
		    else if (visobj)
			pline_The("%s.", aobjnam(target, "fade"));
		}
		target->otyp = SCR_BLANK_PAPER;
		target->spe = 0;
	    }
	} else if (target->oerodeproof || (Race_if(PM_CHIQUAI) && rn2(4)) ||
		(acid_dmg ? !is_corrodeable(target) : !is_rustprone(target))) {
	    if (flags.verbose || !(target->oerodeproof && target->rknown)) {
		if (victim == &youmonst)
		    Your("%s not affected.", aobjnam(target, "are"));
		else if (vismon)
		    pline("%s's %s not affected.", Monnam(victim),
			aobjnam(target, "are"));
		/* no message if not carried */
	    }
	    if (target->oerodeproof) target->rknown = TRUE;
	} else if (erosion < MAX_ERODE) {
	    if (victim == &youmonst) {
		Your("%s%s!", aobjnam(target, acid_dmg ? "corrode" : "rust"),
		    erosion+1 == MAX_ERODE ? " completely" :
		    erosion ? " further" : "");
		if (issoviet && target->greased) pline("Sovetskiy khochet vash detal' byt' povrezhden, nesmotrya na smazku, potomu chto on takoy mudak!");
	    } else if (vismon)
		pline("%s's %s%s!", Monnam(victim),
		    aobjnam(target, acid_dmg ? "corrode" : "rust"),
		    erosion+1 == MAX_ERODE ? " completely" :
		    erosion ? " further" : "");
	    else if (visobj)
		pline_The("%s%s!",
		    aobjnam(target, acid_dmg ? "corrode" : "rust"),
		    erosion+1 == MAX_ERODE ? " completely" :
		    erosion ? " further" : "");
	    if (acid_dmg)
		target->oeroded2++;
	    else
		target->oeroded++;
	} else /*if (!target->oartifact)*/ {

		    if (victim == &youmonst && !hard_to_destruct(target) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) ) {
			pline("One of your items got vaporized!"),
			remove_worn_item(target, FALSE);
			if (target == uball) unpunish();
			useupall(target);
			uwepgone();
			update_inventory();
		    }

			/* the following code always crashes the game, unfortunately... --Amy */
			/*else {

			lethe_damage(target, TRUE, FALSE);*/

			/*pline("An item got vaporized!"),
					delobj(target);
					update_inventory();*/


/*	    if (flags.verbose) {
		if (victim == &youmonst)
		    Your("%s completely %s.",
			aobjnam(target, Blind ? "feel" : "look"),
			acid_dmg ? "corroded" : "rusty");
		else if (vismon)
		    pline("%s's %s completely %s.", Monnam(victim),
			aobjnam(target, "look"),
			acid_dmg ? "corroded" : "rusty");
		else if (visobj)
		    pline_The("%s completely %s.",
			aobjnam(target, "look"),
			acid_dmg ? "corroded" : "rusty");
	    } */

	}
}

void
wither_obj(target, acid_dmg, fade_scrolls)
struct obj *target;
boolean acid_dmg;
boolean fade_scrolls;
{
	int erosion;
	struct monst *victim;
	boolean vismon;
	boolean visobj;

	if (!target)
	    return;
	victim = carried(target) ? &youmonst :
	    mcarried(target) ? target->ocarry : (struct monst *)0;
	vismon = victim && (victim != &youmonst) && canseemon(victim);
	visobj = !victim && cansee(bhitpos.x, bhitpos.y); /* assume thrown */

	erosion = (rn2(2) ? target->oeroded2 : target->oeroded);

	if (stack_too_big(target)) return;

	if (target->oartifact && rn2(4)) return;

	if (itemhasappearance(target, APP_BRAND_NEW_GLOVES) && rn2(4) ) return;

	if (itemhasappearance(target, APP_IMAGINARY_HEELS) ) return;

	if (target->oartifact == ART_RATCH_CLOSURE_SCRATCHING && rn2(4) ) return;

	if (itemhasappearance(target, APP_WITHERED_CLOAK) ) return;

	if (uarmf && !rn2(2) && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) return;

	if (target->greased && (!issoviet || !rn2(2)) ) {
	    grease_protect(target,(char *)0,victim);
	} else if (target->oclass == SCROLL_CLASS) {
	    if(fade_scrolls && target->otyp != SCR_BLANK_PAPER && !target->oartifact && target->otyp != SCR_HEALING && target->otyp != SCR_EXTRA_HEALING && target->otyp != SCR_STANDARD_ID && target->otyp != SCR_HEAL_OTHER && target->otyp != SCR_MANA && target->otyp != SCR_GREATER_MANA_RESTORATION && target->otyp != SCR_CURE && target->otyp != SCR_PHASE_DOOR
#ifdef MAIL
	    && target->otyp != SCR_MAIL
#endif
					)
	    {
		if (!Blind) {
		    if (victim == &youmonst)
			Your("%s.", aobjnam(target, "fade"));
		    else if (vismon)
			pline("%s's %s.", Monnam(victim),
			      aobjnam(target, "fade"));
		    else if (visobj)
			pline_The("%s.", aobjnam(target, "fade"));
		}
		target->otyp = SCR_BLANK_PAPER;
		target->spe = 0;
	    }
	} else if (is_unwitherable(target) || (Race_if(PM_CHIQUAI) && rn2(4)) ) {
	    if (flags.verbose || !(target->oerodeproof && target->rknown)) {
		if (victim == &youmonst)
		    Your("%s not affected.", aobjnam(target, "are"));
		else if (vismon)
		    pline("%s's %s not affected.", Monnam(victim),
			aobjnam(target, "are"));
		/* no message if not carried */
	    }
	    if (target->oerodeproof) target->rknown = TRUE;
	} else if (erosion < MAX_ERODE) {
	    if (victim == &youmonst) {
		Your("%s%s!", aobjnam(target, acid_dmg ? "corrode" : "rust"),
		    erosion+1 == MAX_ERODE ? " completely" :
		    erosion ? " further" : "");
		if (issoviet && target->greased) pline("Sovetskiy khochet vash detal' byt' povrezhden, nesmotrya na smazku, potomu chto on takoy mudak!");
	    } else if (vismon)
		pline("%s's %s%s!", Monnam(victim),
		    aobjnam(target, acid_dmg ? "corrode" : "rust"),
		    erosion+1 == MAX_ERODE ? " completely" :
		    erosion ? " further" : "");
	    else if (visobj)
		pline_The("%s%s!",
		    aobjnam(target, acid_dmg ? "corrode" : "rust"),
		    erosion+1 == MAX_ERODE ? " completely" :
		    erosion ? " further" : "");
	    if (acid_dmg)
		target->oeroded2++;
	    else
		target->oeroded++;
	} else {

		    if (victim == &youmonst && !hard_to_destruct(target) && (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) ) {
			pline("One of your items got vaporized!"),
			remove_worn_item(target, FALSE);
			if (target == uball) unpunish();
			useupall(target);
			uwepgone();
			update_inventory();
		    }

	}
}

int
chwepon(otmp, amount)
register struct obj *otmp;
register int amount;
{
	const char *color = hcolor((amount < 0) ? NH_BLACK : NH_BLUE);
	const char *xtime;
	int otyp = STRANGE_OBJECT;

	if(!uwep || (uwep->oclass != WEAPON_CLASS && uwep->oclass != BALL_CLASS && uwep->oclass != GEM_CLASS && uwep->oclass != CHAIN_CLASS && uwep->oclass != VENOM_CLASS && !is_weptool(uwep))) {
		char buf[BUFSZ];

		sprintf(buf, "Your %s %s.", makeplural(body_part(HAND)),
			(amount >= 0) ? "twitch" : "itch");
		strange_feeling(otmp, buf);
		exercise(A_DEX, (boolean) (amount >= 0));
		return(0);
	}

	if (otmp && otmp->oclass == SCROLL_CLASS) otyp = otmp->otyp;

	if(uwep->otyp == WORM_TOOTH && amount >= 0) {
		uwep->otyp = CRYSKNIFE;
		uwep->oerodeproof = 0;
		Your("weapon seems sharper now.");

		if ((uwep->morgcurse || uwep->evilcurse || uwep->bbrcurse) && !rn2(100) ) {
			uwep->prmcurse = uwep->hvycurse = uwep->cursed = uwep->morgcurse = uwep->evilcurse = uwep->bbrcurse = uwep->stckcurse = 0;
		}
		else if (uwep->prmcurse && !(uwep->morgcurse || uwep->evilcurse || uwep->bbrcurse) && !rn2(10) ) {
			uwep->prmcurse = uwep->hvycurse = uwep->cursed = uwep->morgcurse = uwep->evilcurse = uwep->bbrcurse = uwep->stckcurse = 0;
		}
		else if (!(uwep->prmcurse) && !(uwep->morgcurse || uwep->evilcurse || uwep->bbrcurse) && uwep->hvycurse && !rn2(3) ) {
			uwep->prmcurse = uwep->hvycurse = uwep->cursed = uwep->morgcurse = uwep->evilcurse = uwep->bbrcurse = uwep->stckcurse = 0;
		}
		else if (!(uwep->prmcurse) && !(uwep->hvycurse) && !(uwep->morgcurse || uwep->evilcurse || uwep->bbrcurse) ) uwep->prmcurse = uwep->hvycurse = uwep->cursed = uwep->morgcurse = uwep->evilcurse = uwep->bbrcurse = uwep->stckcurse = 0;

		if (otyp != STRANGE_OBJECT) makeknown(otyp);
		return(1);
	}

	if(uwep->otyp == CRYSKNIFE && amount < 0) {
		if (!uwep->oartifact) uwep->otyp = WORM_TOOTH;
		uwep->oerodeproof = 0;
		Your("weapon seems duller now.");
		if (otyp != STRANGE_OBJECT && otmp->bknown) makeknown(otyp);
		return(1);
	}

	if (amount < 0 && uwep->oartifact && restrict_name(uwep, ONAME(uwep))) {
	    if (!Blind)
		Your("%s %s.", aobjnam(uwep, "faintly glow"), color);
	    return(1);
	}
	/* there is a (soft) upper and lower limit to uwep->spe */
	if(((uwep->spe > (is_droven_weapon(uwep) ? 10 : is_elven_weapon(uwep) ? 8 : 5) && amount >= 0) || (uwep->spe < -5 && amount < 0)) && rn2(3) && !rn2(3) ) {

		if (uwep->oartifact) {
			uwep->spe = 0;
		    Your("%s %s for a while and then %s.",
			 aobjnam(uwep, "violently glow"), color,
			 otense(uwep, "fade"));
			return(1);
		}
	    if (!Blind)
	    Your("%s %s for a while and then %s.",
		 aobjnam(uwep, "violently glow"), color,
		 otense(uwep, "evaporate"));
	    else
		Your("%s.", aobjnam(uwep, "evaporate"));

	    useupall(uwep);	/* let all of them disappear */
	    return(1);
	}
	if (!Blind) {
	    xtime = (amount*amount == 1) ? "moment" : "while";
	    Your("%s %s for a %s.",
		 aobjnam(uwep, amount == 0 ? "violently glow" : "glow"),
		 color, xtime);
	    if (otyp != STRANGE_OBJECT && uwep->known &&
		    (amount > 0 || (amount < 0 && otmp->bknown)))
		makeknown(otyp);
	}
	uwep->spe += amount;
	if (uwep && uwep->oartifact == ART_BOARDED_SHELF && amount > 0 && uwep->spe < 22) uwep->spe += 4;
	if (Race_if(PM_SPARD) && amount > 0) uwep->spe++;
	if(amount > 0) {

		if (uwep && objects[(uwep)->otyp].oc_material == MT_CELESTIUM) {
			if (!uwep->cursed) bless(uwep);
			else uncurse(uwep, FALSE);
		}

		if ((uwep->morgcurse || uwep->evilcurse || uwep->bbrcurse) && !rn2(100) ) {
			uwep->prmcurse = uwep->hvycurse = uwep->cursed = uwep->morgcurse = uwep->evilcurse = uwep->bbrcurse = uwep->stckcurse = 0;
		}
		else if (uwep->prmcurse && !(uwep->morgcurse || uwep->evilcurse || uwep->bbrcurse) && !rn2(10) ) {
			uwep->prmcurse = uwep->hvycurse = uwep->cursed = uwep->morgcurse = uwep->evilcurse = uwep->bbrcurse = uwep->stckcurse = 0;
		}
		else if (!(uwep->prmcurse) && !(uwep->morgcurse || uwep->evilcurse || uwep->bbrcurse) && uwep->hvycurse && !rn2(3) ) {
			uwep->prmcurse = uwep->hvycurse = uwep->cursed = uwep->morgcurse = uwep->evilcurse = uwep->bbrcurse = uwep->stckcurse = 0;
		}
		else if (!(uwep->prmcurse) && !(uwep->hvycurse) && !(uwep->morgcurse || uwep->evilcurse || uwep->bbrcurse) ) uwep->prmcurse = uwep->hvycurse = uwep->cursed = uwep->morgcurse = uwep->evilcurse = uwep->bbrcurse = uwep->stckcurse = 0;

	}

	/*
	 * Enchantment, which normally improves a weapon, has an
	 * addition adverse reaction on Magicbane whose effects are
	 * spe dependent.  Give an obscure clue here.
	 */
	if (uwep->oartifact == ART_MAGICBANE && uwep->spe >= 0) {
		Your("right %s %sches!",
			body_part(HAND),
			(((amount > 1) && (uwep->spe > 1)) ? "flin" : "it"));
	}

	/* an elven magic clue, cookie@keebler */
	/* elven weapons vibrate warningly when enchanted beyond a limit */
	if ((uwep->spe > (is_droven_weapon(uwep) ? 10 : is_elven_weapon(uwep) ? 8 : 5) )
		&& (is_elven_weapon(uwep) || uwep->oartifact || !rn2(7)))
	    Your("%s unexpectedly.",
		aobjnam(uwep, "suddenly vibrate"));

	return(1);
}

int
chwepon_other(otmp, amount)
register struct obj *otmp;
register int amount;
{
	const char *color = hcolor((amount < 0) ? NH_BLACK : NH_BLUE);
	const char *xtime;

	if(!otmp || (otmp->oclass != WEAPON_CLASS && otmp->oclass != BALL_CLASS && otmp->oclass != GEM_CLASS && otmp->oclass != CHAIN_CLASS && otmp->oclass != VENOM_CLASS && !is_weptool(otmp))) {
		char buf[BUFSZ];

		sprintf(buf, "Your %s %s.", makeplural(body_part(HAND)),
			(amount >= 0) ? "twitch" : "itch");
		exercise(A_DEX, (boolean) (amount >= 0));
		return(0);
	}

	if(otmp->otyp == WORM_TOOTH && amount >= 0) {
		otmp->otyp = CRYSKNIFE;
		otmp->oerodeproof = 0;
		Your("weapon seems sharper now.");

		if ((otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(100) ) {
			otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;
		}
		else if (otmp->prmcurse && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(10) ) {
			otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;
		}
		else if (!(otmp->prmcurse) && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && otmp->hvycurse && !rn2(3) ) {
			otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;
		}
		else if (!(otmp->prmcurse) && !(otmp->hvycurse) && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) ) otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;

		return(1);
	}

	if(otmp->otyp == CRYSKNIFE && amount < 0) {
		if (!otmp->oartifact) otmp->otyp = WORM_TOOTH;
		otmp->oerodeproof = 0;
		Your("weapon seems duller now.");
		return(1);
	}

	if (amount < 0 && otmp->oartifact && restrict_name(otmp, ONAME(otmp))) {
	    if (!Blind)
		Your("%s %s.", aobjnam(otmp, "faintly glow"), color);
	    return(1);
	}
	/* there is a (soft) upper and lower limit to otmp->spe */
	if(((otmp->spe > (is_droven_weapon(otmp) ? 10 : is_elven_weapon(otmp) ? 8 : 5) && amount >= 0) || (otmp->spe < -5 && amount < 0)) && rn2(3) && !rn2(3) ) {

		if (otmp->oartifact) {
			otmp->spe = 0;
		    Your("%s %s for a while and then %s.",
			 aobjnam(otmp, "violently glow"), color,
			 otense(otmp, "fade"));
			return(1);
		}
	    if (!Blind)
	    Your("%s %s for a while and then %s.",
		 aobjnam(otmp, "violently glow"), color,
		 otense(otmp, "evaporate"));
	    else
		Your("%s.", aobjnam(otmp, "evaporate"));

	    useupall(otmp);	/* let all of them disappear */
	    return(1);
	}
	if (!Blind) {
	    xtime = (amount*amount == 1) ? "moment" : "while";
	    Your("%s %s for a %s.",
		 aobjnam(otmp, amount == 0 ? "violently glow" : "glow"),
		 color, xtime);
	}
	otmp->spe += amount;
	if (otmp && otmp->oartifact == ART_BOARDED_SHELF && amount > 0 && otmp->spe < 22) otmp->spe += 4;
	if (Race_if(PM_SPARD) && amount > 0) otmp->spe++;
	if(amount > 0) {

		if (otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM) {
			if (!otmp->cursed) bless(otmp);
			else uncurse(otmp, FALSE);
		}

		if ((otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(100) ) {
			otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;
		}
		else if (otmp->prmcurse && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(10) ) {
			otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;
		}
		else if (!(otmp->prmcurse) && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && otmp->hvycurse && !rn2(3) ) {
			otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;
		}
		else if (!(otmp->prmcurse) && !(otmp->hvycurse) && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) ) otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;

	}

	/*
	 * Enchantment, which normally improves a weapon, has an
	 * addition adverse reaction on Magicbane whose effects are
	 * spe dependent.  Give an obscure clue here.
	 */
	if (otmp->oartifact == ART_MAGICBANE && otmp->spe >= 0) {
		Your("right %s %sches!",
			body_part(HAND),
			(((amount > 1) && (otmp->spe > 1)) ? "flin" : "it"));
	}

	/* an elven magic clue, cookie@keebler */
	/* elven weapons vibrate warningly when enchanted beyond a limit */
	if ((otmp->spe > (is_droven_weapon(otmp) ? 10 : is_elven_weapon(otmp) ? 8 : 5) )
		&& (is_elven_weapon(otmp) || otmp->oartifact || !rn2(7)))
	    Your("%s unexpectedly.",
		aobjnam(otmp, "suddenly vibrate"));

	return(1);
}

int
welded(obj)
register struct obj *obj;
{
	if (obj && obj == uwep && will_weld(obj)) {
		obj->bknown = TRUE;
		return 1;
	}
	return 0;
}

void
weldmsg(obj)
register struct obj *obj;
{
	long savewornmask;

	savewornmask = obj->owornmask;
	Your("%s %s welded to your %s!",
		xname(obj), otense(obj, "are"),
		bimanual(obj) ? (const char *)makeplural(body_part(HAND))
				: body_part(HAND));
	obj->owornmask = savewornmask;
}

void
unwield(obj, put_away)
register struct obj *obj;
boolean put_away;
{
    /* MRKR: Extinguish torches when they are put away */
    if (put_away && obj->otyp == TORCH && obj->lamplit) {
	You("extinguish %s before putting it away.", yname(obj));
	end_burn(obj, TRUE);
    }
    else if (put_away && is_lightsaber(obj) && obj->lamplit &&
	!artifact_light(obj)){
	    char yourbuf[BUFSZ];
	    You("deactivate %s %s before putting it away.", shk_your(yourbuf, obj), xname(obj));
	    end_burn(obj, TRUE);
    }

	if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void)doredraw();
}

/*wield.c*/
