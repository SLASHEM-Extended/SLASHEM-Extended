/*	SCCS Id: @(#)steal.c	3.4	2003/12/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

STATIC_PTR int stealarm(void);

#ifdef OVLB
STATIC_DCL const char *equipname(struct obj *);
STATIC_DCL void mdrop_obj(struct monst *,struct obj *,BOOLEAN_P);

STATIC_OVL const char *
equipname(otmp)
register struct obj *otmp;
{
	return (
		(otmp == uwep) ? "weapon" :
		(otmp == uswapwep) ? "secondary weapon" :
		(otmp == uarmu) ? "shirt" :
		(otmp == uarmf) ? "boots" :
		(otmp == uarms) ? "shield" :
		(otmp == uarmg) ? "gloves" :
		(otmp == uarmc) ? cloak_simple_name(otmp) :
		(otmp == uleft) ? "left ring" :
		(otmp == uright) ? "right ring" :
		(otmp == uamul) ? "amulet" :
		(otmp == uimplant) ? "implant" :
		(otmp == uarmh) ? "helmet" : "armor");
}

#ifndef GOLDOBJ
long		/* actually returns something that fits in an int */
somegold()
{
#ifdef LINT	/* long conv. ok */
	return(0L);
#else
	return (long)( (u.ugold < 100) ? u.ugold :
		(u.ugold > 10000) ? rnd(10000) : rnd((int) u.ugold) );
#endif
}

void
stealgold(mtmp)
register struct monst *mtmp;
{
	register struct obj *gold = g_at(u.ux, u.uy);
	register long tmp;

	if (gold && !(gold == uchain) && !(gold == uball) && ( !u.ugold || gold->quan > u.ugold || !rn2(5))) {
	    mtmp->mgold += gold->quan;
	    delobj(gold);
	    newsym(u.ux, u.uy);
	    pline("%s quickly snatches some gold from between your %s!",
		    Monnam(mtmp), makeplural(body_part(FOOT)));
	    if(!u.ugold || !rn2(5)) {
		if (!tele_restrict(mtmp)) (void) rloc(mtmp, FALSE);
		/* do not set mtmp->mavenge here; gold on the floor is fair game */
		monflee(mtmp, rnd(10), FALSE, FALSE);
	    }
	} else if(u.ugold) {
	    u.ugold -= (tmp = somegold());
	    Your("purse feels lighter.");
	    if ( !(metallivorous(mtmp->data) || mtmp->egotype_metallivore) && rn2(20) )
		mtmp->mgold += tmp;
/* Gold bugs are metallivores, so they're supposed to EAT the gold they steal. --Amy */
	if (!tele_restrict(mtmp) && !rn2(5)) (void) rloc(mtmp, FALSE);
	    mtmp->mavenge = 1;
	    monflee(mtmp, rnd(10), FALSE, FALSE);
	    flags.botl = 1;
	}
}

#else /* !GOLDOBJ */

long		/* actually returns something that fits in an int */
somegold(umoney)
long umoney;
{
#ifdef LINT	/* long conv. ok */
	return(0L);
#else
	return (long)( (umoney < 100) ? umoney :
		(umoney > 10000) ? rnd(10000) : rnd((int) umoney) );
#endif
}

/*
Find the first (and hopefully only) gold object in a chain.
Used when leprechaun (or you as leprechaun) looks for
someone else's gold.  Returns a pointer so the gold may
be seized without further searching.
May search containers too.
Deals in gold only, as leprechauns don't care for lesser coins.
*/
struct obj *
findgold(chain)
register struct obj *chain;
{
        while (chain && chain->otyp != GOLD_PIECE) chain = chain->nobj;
        return chain;
}

/* 
Steal gold coins only.  Leprechauns don't care for lesser coins.
*/
void
stealgold(mtmp)
register struct monst *mtmp;
{
	register struct obj *fgold = g_at(u.ux, u.uy);
	register struct obj *ygold;
	register long tmp;

        /* skip lesser coins on the floor */        
        while (fgold && fgold->otyp != GOLD_PIECE) fgold = fgold->nexthere; 

        /* Do you have real gold? */
        ygold = findgold(invent);

	if (fgold && ( !ygold || fgold->quan > ygold->quan || !rn2(5))) {
            obj_extract_self(fgold);
	    add_to_minv(mtmp, fgold);
	    newsym(u.ux, u.uy);
	    pline("%s quickly snatches some gold from between your %s!",
		    Monnam(mtmp), makeplural(body_part(FOOT)));
	    if(!ygold || !rn2(5)) {
		if (!tele_restrict(mtmp)) (void) rloc(mtmp, FALSE);
		monflee(mtmp, rnd(10), FALSE, FALSE);
	    }
	} else if(ygold) {
            const int gold_price = objects[GOLD_PIECE].oc_cost;
	    tmp = (somegold(money_cnt(invent)) + gold_price - 1) / gold_price;
	    tmp = min(tmp, ygold->quan);
            if (tmp < ygold->quan) ygold = splitobj(ygold, tmp);
            freeinv(ygold);
            add_to_minv(mtmp, ygold);
	    Your("purse feels lighter.");
	    if (!tele_restrict(mtmp) && !rn2(5)) (void) rloc(mtmp, FALSE);
	    monflee(mtmp, rnd(10), FALSE, FALSE);
	    flags.botl = 1;
	}
}
#endif /* GOLDOBJ */

/* steal armor after you finish taking it off */
unsigned int stealoid;		/* object to be stolen */
unsigned int stealmid;		/* monster doing the stealing */

STATIC_PTR int
stealarm()
{
	register struct monst *mtmp;
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
	    if(otmp->o_id == stealoid) {
		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		    if(mtmp->m_id == stealmid) {
			if(DEADMONSTER(mtmp)) impossible("stealarm(): dead monster stealing"); 
			if(!dmgtype(mtmp->data, AD_SITM)) /* polymorphed */
			    goto botm;
			if(otmp->unpaid)
			    subfrombill(otmp, shop_keeper(*u.ushops));
			freeinv(otmp);
			if (evilfriday) pline("Something seems missing...");
			else pline("%s steals %s!", Monnam(mtmp), doname(otmp));

			if (StealDegrading || u.uprops[STEAL_DEGRADING].extrinsic || have_stealdegradestone()) {
				if (!stack_too_big(otmp)) curse(otmp);
				if (!stack_too_big(otmp) && otmp->spe > -20) otmp->spe--;
			}

			(void) mpickobj(mtmp,otmp,FALSE);	/* may free otmp */
			/* Implies seduction, "you gladly hand over ..."
			   so we don't set mavenge bit here. */
			monflee(mtmp, rnd(10), FALSE, FALSE);
			if (!tele_restrict(mtmp) && !rn2(5)) (void) rloc(mtmp, FALSE);
		        break;
		    }
		}
		break;
	    }
	}
botm:   stealoid = 0;
	return 0;
}

/* An object you're wearing has been taken off by a monster (theft or
   seduction).  Also used if a worn item gets transformed (stone to flesh). */
void
remove_worn_item(obj, unchain_ball)
struct obj *obj;
boolean unchain_ball;	/* whether to unpunish or just unwield */
{
	if (donning(obj))
	    cancel_don();
	if (!obj->owornmask)
	    return;

	if (obj->owornmask & W_ARMOR) {
	    if (obj == uskin) {
		impossible("Removing embedded scales?");
		skinback(TRUE);		/* uarm = uskin; uskin = 0; */
	    }
	    if (obj == uarm) (void) Armor_off();
	    else if (obj == uarmc) (void) Cloak_off();
	    else if (obj == uarmf) (void) Boots_off();
	    else if (obj == uarmg) (void) Gloves_off();
	    else if (obj == uarmh) (void) Helmet_off();
	    else if (obj == uarms) (void) Shield_off();
	    else if (obj == uarmu) (void) Shirt_off();
	    /* catchall -- should never happen */
	    else setworn((struct obj *)0, obj->owornmask & W_ARMOR);
	} else if (obj->owornmask & W_AMUL) {
	    Amulet_off();
	} else if (obj->owornmask & W_IMPLANT) {
	    Implant_off();
	} else if (obj->owornmask & W_RING) {
	    Ring_gone(obj);
	} else if (obj->owornmask & W_TOOL) {
	    Blindf_off(obj);
	} else if (obj->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
	    if (obj == uwep)
		uwepgone();
	    if (obj == uswapwep)
		uswapwepgone();
	    if (obj == uquiver)
		uqwepgone();
	}

	if (obj->owornmask & (W_BALL|W_CHAIN)) {
	    if (unchain_ball) unpunish();
	} else if (obj->owornmask) {
	/* catchall */
	    setnotworn(obj);
	}
}

/* Returns 1 when something was stolen (or at least, when N should flee now)
 * Returns -1 if the monster died in the attempt
 * Avoid stealing the object stealoid
 */
int
steal(mtmp, objnambuf, powersteal, instasteal)
struct monst *mtmp;
char *objnambuf;
boolean powersteal; /* AD_SEDU - more likely to succeed */
boolean instasteal; /* offlevel item or starting as matrayser - guaranteed to succeed, don't give messages */
{
	struct obj *otmp;
	int tmp, could_petrify, named = 0, armordelay;
	boolean monkey_business; /* true iff an animal is doing the thievery */
	int do_charm = is_neuter(mtmp->data) || flags.female == mtmp->female;

	int invisstealroll = 0;
	int bonestealroll = 0;

	int amountofstealings = 1;
	int nobjtempvar = 0;
	if (inv_cnt() > 50) {
		nobjtempvar = inv_cnt();
		while (nobjtempvar > 50) {
			amountofstealings++;
			nobjtempvar -= 40;
		}
	}
stealagain:

	invisstealroll = 0;
	if (powersteal) invisstealroll = rnd(40);
	if (instasteal) invisstealroll = 100;

	bonestealroll = 0;
	if (!rn2(10)) bonestealroll = 1;
	if (instasteal) bonestealroll = 1;

	char buf[BUFSZ];

	if (objnambuf) *objnambuf = '\0';
	/* the following is true if successful on first of two attacks. */
	/*if(!monnear(mtmp, u.ux, u.uy)) return(0);*/

	/* food being eaten might already be used up but will not have
	   been removed from inventory yet; we don't want to steal that,
	   so this will cause it to be removed now */
	if (occupation) (void) maybe_finished_meal(FALSE);

	if (!invent || (inv_cnt() == 1 && uskin)) {
nothing_to_steal:
	    /* Not even a thousand men in armor can strip a naked man. */
	    if(Blind && !instasteal)
	      pline("Somebody tries to rob you, but finds nothing to steal.");
	    else if (!instasteal)
	      pline("%s tries to rob you, but there is nothing to steal!", Monnam(mtmp));
	    return(1);  /* let thief flee */
	}

	if (!rn2(10) && !instasteal && uarmf && itemhasappearance(uarmf, APP_NOBLE_SANDALS) ) {
		pline("%s tries to steal something from you, but you notice it and scratch %s %s with your noble sandals!", Monnam(mtmp), mhis(mtmp), makeplural(mbodypart(mtmp, LEG)) );
		return(1);  /* let thief flee */

	}

	monkey_business = (is_animal(mtmp->data) && !instasteal);
	if (monkey_business) {
	    ;	/* skip ring special cases */
	} else if (Adornment & LEFT_RING) {
	    otmp = uleft;
	    goto gotobj;
	} else if (Adornment & RIGHT_RING) {
	    otmp = uright;
	    goto gotobj;
	}

	tmp = 0;
	for(otmp = invent; otmp; otmp = otmp->nobj)
	    if ((!uarm || otmp != uarmc) && otmp != uskin
				&& ((!otmp->oinvis || perceives(mtmp->data) || (invisstealroll > 10) ) && (objects[otmp->otyp].oc_material != MT_BONE || bonestealroll) && !(otmp->stckcurse && !instasteal) && (!otmp->oinvisreal || (invisstealroll > 36)) )
				)
		tmp += ((otmp->owornmask &
			(W_ARMOR | W_RING | W_AMUL | W_IMPLANT | W_TOOL)) ? 5 : 1);
	if (!tmp) goto nothing_to_steal;
	tmp = rn2(tmp);
	for(otmp = invent; otmp; otmp = otmp->nobj)
	    if ((!uarm || otmp != uarmc) && otmp != uskin
				&& ((!otmp->oinvis || perceives(mtmp->data) || (invisstealroll > 10)) && (objects[otmp->otyp].oc_material != MT_BONE || bonestealroll) && !(otmp->stckcurse && !instasteal) && (!otmp->oinvisreal || (invisstealroll > 36)) )
			)
		if((tmp -= ((otmp->owornmask &
			(W_ARMOR | W_RING | W_AMUL | W_IMPLANT | W_TOOL)) ? 5 : 1)) < 0)
			break;
	if(!otmp) {
		impossible("Steal fails!");
		return(0);
	}
	/* can't steal gloves while wielding - so steal the wielded item. */
	if (otmp == uarmg && !instasteal && uwep && uwep->stckcurse) {
		if (amountofstealings > 1) {
			amountofstealings--;
			goto stealagain;
		} else return(0);
	}
	if (otmp == uarm && !instasteal && uarmc && uarmc->stckcurse) {
		if (amountofstealings > 1) {
			amountofstealings--;
			goto stealagain;
		} else return(0);
	}
	if (otmp == uarmu && !instasteal && uarmc && uarmc->stckcurse) {
		if (amountofstealings > 1) {
			amountofstealings--;
			goto stealagain;
		} else return(0);
	}
	if (otmp == uarmu && !instasteal && uarm && uarm->stckcurse) {
		if (amountofstealings > 1) {
			amountofstealings--;
			goto stealagain;
		} else return(0);
	}

	if (otmp == uarmg && uwep && !(uwep->stckcurse && !instasteal))
	    otmp = uwep;

	/* can't steal armor while wearing cloak - so steal the cloak. */
	else if(otmp == uarm && uarmc && !(uarmc->stckcurse && !instasteal)) otmp = uarmc;
	else if(otmp == uarmu && uarmc && !(uarmc->stckcurse && !instasteal)) otmp = uarmc;
	else if(otmp == uarmu && uarm && !(uarm->stckcurse && !instasteal)) otmp = uarm;


gotobj:
	if (stack_too_big(otmp) && !instasteal && !issoviet) {

		pline("%s tries to steal your %s, but you quickly protect them!", !canspotmon(mtmp) ? "It" : Monnam(mtmp), doname(otmp));
		/* can't say "you may steal them anyway", because the stack resisted,
		 * which means the stealing attempt just failed unconditionally --Amy */

		if (amountofstealings > 1) {
			amountofstealings--;
			goto stealagain;
		} else return(0);
	}

	/* artifacts resist stealing because I'm nice --Amy */
	if (rn2(10) && !instasteal && (!powersteal || !rn2(2)) && (otmp->oartifact || (otmp->fakeartifact && !rn2(3))) && !issoviet) {

		pline("%s tries to steal your %s, but you quickly protect it!", !canspotmon(mtmp) ? "It" : Monnam(mtmp), doname(otmp));

		getlin ("Do you want to allow it to be stolen anyway? [yes/no]",buf);
		(void) lcase (buf);
		if (strcmp (buf, "yes")) {

			if (amountofstealings > 1) {
				amountofstealings--;
				goto stealagain;
			} else return(0);

		}

	}

	/* In Soviet Russia, you don't need no saving throw, since you're probably so naughty you'd jump at any chance to
	 * have "fun" with a woman. Even if said "fun" means losing all that you have. And of course, Russian women are also
	 * the absolute temptresses who will MAKE you undress whether you want it or not. --Amy */

	if ( ((rnd(50) < ACURR(A_CHA)) || (rnd(50) < ACURR(A_CHA)) || (rnd(50) < ACURR(A_CHA)) ) && !instasteal && !powersteal && !issoviet && (otmp->owornmask & (W_ARMOR | W_RING | W_AMUL | W_IMPLANT | W_TOOL))) {
		if (otmp->cursed) {
			otmp->bknown = 1;
			pline("%s tries to take off your %s, which appears to be cursed.", !canspotmon(mtmp) ? "It" : Monnam(mtmp), equipname(otmp)); 

			getlin ("Allow it to be taken? [yes/no]",buf);
			(void) lcase (buf);
			if (strcmp (buf, "yes")) {

				if (amountofstealings > 1) {
					amountofstealings--;
					goto stealagain;
				} else return(0);

			}
		} else {
			pline("%s tries to take off your %s, but you resist!", !canspotmon(mtmp) ? "It" : Monnam(mtmp), equipname(otmp));

			getlin ("Do you want to allow it to be stolen anyway? [yes/no]",buf);
			(void) lcase (buf);
			if (strcmp (buf, "yes")) {

				if (amountofstealings > 1) {
					amountofstealings--;
					goto stealagain;
				} else return(0);

			}
		}
	}

	if(otmp->o_id == stealoid) {
		if (amountofstealings > 1) {
			amountofstealings--;
			goto stealagain;
		} else return(0);
	}

	/* I took the liberty of making saddles less likely to be stolen, because riding sucks enough as it is. --Amy */
	if (rn2(5) && !issoviet && otmp == usaddle) {
		if (amountofstealings > 1) {
			amountofstealings--;
			goto stealagain;
		} else return(0);
	}
	if (otmp == usaddle) dismount_steed(DISMOUNT_FELL);

	/* animals can't overcome curse stickiness nor unlock chains */
	if (monkey_business && !instasteal) {
	    boolean ostuck;
	    /* is the player prevented from voluntarily giving up this item?
	       (ignores loadstones; the !can_carry() check will catch those) */
	    if (otmp == uball)
		ostuck = TRUE;	/* effectively worn; curse is implicit */
	    else if (otmp == uquiver || (otmp == uswapwep && !u.twoweap))
		ostuck = FALSE;	/* not really worn; curse doesn't matter */
	    else
		ostuck = (otmp->cursed && otmp->owornmask);

	    if ((ostuck || !can_carry(mtmp, otmp)) && rn2(5)) {
		static const char * const how[] = { "steal","snatch","grab","take" };
 cant_take:
		pline("%s tries to %s your %s but gives up.",
		      Monnam(mtmp), how[rn2(SIZE(how))],
		      (otmp->owornmask & W_ARMOR) ? equipname(otmp) :
		       cxname(otmp));
		/* the fewer items you have, the less likely the thief
		   is going to stick around to try again (0) instead of
		   running away (1) */

		if (amountofstealings > 1) {
			amountofstealings--;
			goto stealagain;
		} else return !rn2(inv_cnt() / 5 + 2);
		
	    }
	}

	if (otmp->otyp == LEATHER_LEASH && otmp->leashmon) {
	    if (monkey_business && otmp->cursed) goto cant_take;
	    o_unleash(otmp);
	}
	if (otmp->otyp == INKA_LEASH && otmp->leashmon) {
	    if (monkey_business && otmp->cursed) goto cant_take;
	    o_unleash(otmp);
	}

	/* you're going to notice the theft... */
	stop_occupation();

	if (otmp->owornmask && instasteal) {
		setnotworn(otmp); /* catchall */
	}

	if((otmp->owornmask & (W_ARMOR | W_RING | W_AMUL | W_IMPLANT | W_TOOL))){
		switch(otmp->oclass) {
		case TOOL_CLASS:
		case AMULET_CLASS:
		case IMPLANT_CLASS:
		case RING_CLASS:
		case FOOD_CLASS: /* meat ring */
		    remove_worn_item(otmp, TRUE);
		    break;
		case ARMOR_CLASS:
		    armordelay = objects[otmp->otyp].oc_delay;
		    /* Stop putting on armor which has been stolen. */
		    if (donning(otmp) || is_animal(mtmp->data)) {
		      remove_worn_item(otmp, TRUE);
			break;
		    } else if (monkey_business) {
			/* animals usually don't have enough patience
			   to take off items which require extra time */
			if (armordelay >= 1 && rn2(10)) goto cant_take;
		      remove_worn_item(otmp, TRUE);
			break;
		    } else {
			int curssv = otmp->cursed;
			int slowly;
			boolean seen = canspotmon(mtmp);
			char pronoun[4];

			if (!seen) {
			    strcpy(pronoun, mhe(mtmp));
			    pronoun[0] = highc(pronoun[0]);
			}
			otmp->cursed = 0;
			/* can't charm you without first waking you */
			if (multi < 0 && is_fainted()) unmul((char *)0);
			slowly = (armordelay >= 1 || multi < 0);
			if (do_charm) {
			    char action[15];
			    if (curssv)
				sprintf(action, "let %s take",
					mhis(mtmp));
			    else
				strcpy(action, slowly ?
					"start removing" : "hand over");
			    pline("%s charms you.  You gladly %s your %s.",
				  !seen ? pronoun : Monnam(mtmp), action,
				  equipname(otmp));
			}
			else
			    {pline("%s seduces you and %s off your %s.",
				  !seen ? pronoun : Adjmonnam(mtmp,
				  mtmp->female ? "beautiful" : "handsome"),
				  curssv ? "helps you to take" :
				  slowly ? "you start taking" : "you take",
				  equipname(otmp));
				if (mtmp->female) (void) adjattrib(A_CHA, -1, FALSE, TRUE);
				if (!mtmp->female) (void) adjattrib(A_WIS, -1, FALSE, TRUE);
				}
			named++;
			/* the following is to set multi for later on */
			nomul(-armordelay, "being seduced into taking off their clothes", TRUE);
			nomovemsg = 0;
			remove_worn_item(otmp, TRUE);
			otmp->cursed = curssv;
			if(multi < 0){
				/*
				multi = 0;
				nomovemsg = 0;
				afternmv = 0;
				*/
				stealoid = otmp->o_id;
				stealmid = mtmp->m_id;
				afternmv = stealarm;

				if (amountofstealings > 1) {
					amountofstealings--;
					goto stealagain;
				} else return(0);

			}
		    }
		    break;
		default:
		    impossible("Tried to steal a strange worn thing. [%d]",
			       otmp->oclass);
		}
	}
	else if (otmp->owornmask)
	    remove_worn_item(otmp, TRUE);

	/* do this before removing it from inventory */
	if (objnambuf) strcpy(objnambuf, yname(otmp));
	/* set mavenge bit so knights won't suffer an
	 * alignment penalty during retaliation;
	 */
	mtmp->mavenge = 1;

	freeinv(otmp);
	if (evilfriday && !instasteal) pline("Something seems missing...");
	else if (!instasteal) pline("%s stole %s.", named ? "It" : Monnam(mtmp), doname(otmp));
	u.cnd_itemstealamount++;

	/* evil patch idea by jonadab - levelporting stealers
         he wants them to always levelport if they manage to steal an artifact...
	   however, if it's an artifact key (those meant for Vlad's Tower), they won't (obvious rule patch) --Amy */
	if (!rn2(1000) || (otmp->oartifact && !(otmp->otyp == SKELETON_KEY)) ) (void) mongets(mtmp, rn2(5) ? SCR_ROOT_PASSWORD_DETECTION : SCR_WARPING);

	could_petrify = (otmp->otyp == CORPSE &&
			 touch_petrifies(&mons[otmp->corpsenm]));

	if (StealDegrading || u.uprops[STEAL_DEGRADING].extrinsic || have_stealdegradestone()) {
		if (!stack_too_big(otmp)) curse(otmp);
		if (!stack_too_big(otmp) && otmp->spe > -20) otmp->spe--;
	}

	(void) mpickobj(mtmp,otmp,FALSE);	/* may free otmp */
	if (could_petrify && !instasteal && !(mtmp->misc_worn_check & W_ARMG) && !rn2(4)) {
	    minstapetrify(mtmp, TRUE);
	    return -1;
	}

	if (amountofstealings > 1) {
		amountofstealings--;
		goto stealagain;
	} else return((multi < 0) ? 0 : 1);
	
}

#endif /* OVLB */
#ifdef OVL1

/* Returns 1 if otmp is free'd, 0 otherwise. */
int
mpickobj(mtmp,otmp,creation)
register struct monst *mtmp;
register struct obj *otmp;
boolean creation;
{
    int freed_otmp;

#ifndef GOLDOBJ
    if (otmp->oclass == COIN_CLASS) {
	mtmp->mgold += otmp->quan;
	obfree(otmp, (struct obj *)0);
	freed_otmp = 1;
    } else {
#endif
    boolean snuff_otmp = FALSE;
    /* don't want hidden light source inside the monster; assumes that
       engulfers won't have external inventories; whirly monsters cause
       the light to be extinguished rather than letting it shine thru */
    if (otmp->lamplit &&  /* hack to avoid function calls for most objs */
      	obj_sheds_light(otmp) &&
	attacktype(mtmp->data, AT_ENGL)) {
	/* this is probably a burning object that you dropped or threw */
	if (u.uswallow && mtmp == u.ustuck && !Blind)
	    pline("%s out.", Tobjnam(otmp, "go"));
	snuff_otmp = TRUE;
    }
    /* Must do carrying effects on object prior to add_to_minv() */
    carry_obj_effects(mtmp, otmp);
    /* add_to_minv() might free otmp [if merged with something else],
       so we have to call it after doing the object checks */
    freed_otmp = add_to_minv(mtmp, otmp);
    /* and we had to defer this until object is in mtmp's inventory */
    if (snuff_otmp) snuff_light_source(mtmp->mx, mtmp->my);
#ifndef GOLDOBJ
    }
#endif
	if (creation) m_dowear(mtmp, TRUE); /* might want to wear whatever armor they stole --Amy */
	else m_dowear(mtmp, FALSE);

    return freed_otmp;
}

#endif /* OVL1 */
#ifdef OVLB

void
stealamulet(mtmp)
struct monst *mtmp;
{
    struct obj *otmp = (struct obj *)0;
    int real=0, fake=0;

	int choiceamount = 0;
	int totalamount = 0;
	boolean somethingtosteal = FALSE;

	int stealtype = isevilvariant ? 3 : rnd(2);

	/* select the artifact to steal */
	/* Amy edit: completely overhauled the function; in evilvariant mode it always tries to steal both a macguffin
	 * and an artifact now, otherwise it either steals one or the other. If it can't find anything in non-evilvariant
	 * mode, it may try to find something in the other category so the attack isn't wasted */

tryagain:
	if (stealtype == 1 || stealtype == 3) {
		if(u.uhave.amulet) {
			real = AMULET_OF_YENDOR;
			fake = FAKE_AMULET_OF_YENDOR;
			choiceamount++;
		}
		if(u.uhave.bell && !rn2(choiceamount + 1)) {
			real = BELL_OF_OPENING;
			fake = BELL;
			choiceamount++;
		}
		if(u.uhave.book && !rn2(choiceamount + 1)) {
			real = SPE_BOOK_OF_THE_DEAD;
			fake = 0;
			choiceamount++;
		}
		if(u.uhave.menorah && !rn2(choiceamount + 1)) {
			real = CANDELABRUM_OF_INVOCATION;
			fake = 0;
			choiceamount++;
		}

		if (!otmp) {
		/* If we get here, real and fake have been set up. */
			for(otmp = invent; otmp; otmp = otmp->nobj)
				if(otmp->otyp == real || (otmp->otyp == fake && !mtmp->iswiz))
				break;
		}

		if (otmp) { /* we have something to snatch */
			if (otmp->owornmask)
				remove_worn_item(otmp, TRUE);
			freeinv(otmp);
		/* mpickobj wont merge otmp because none of the above things
		   to steal are mergable */

			if (StealDegrading || u.uprops[STEAL_DEGRADING].extrinsic || have_stealdegradestone()) {
				if (!stack_too_big(otmp)) curse(otmp);
				if (!stack_too_big(otmp) && otmp->spe > -20) otmp->spe--;
			}

			somethingtosteal = TRUE;
			(void) mpickobj(mtmp,otmp,FALSE);	/* may merge and free otmp */
			if (evilfriday) pline("Something seems missing...");
			else pline("%s stole %s!", Monnam(mtmp), doname(otmp));
			u.cnd_itemstealamount++;
			if (can_teleport(mtmp->data) && !tele_restrict(mtmp))
			(void) rloc(mtmp, FALSE);
		}
		if (!somethingtosteal && stealtype == 1 && rn2(50)) {
			stealtype = 2;
			goto tryagain;
		}

	}

	if (stealtype == 2 || stealtype == 3) {

		/* steal any artifact. Based on the reaction of the 3.6.0 devteam on tungtn's complaint about AD_SAMU behavior --Amy */
		choiceamount = 0;
		for(otmp = invent; otmp; otmp = otmp->nobj) {
			if(otmp->oartifact && !rn2(totalamount + 1) ) {
				choiceamount++;
				totalamount++;
			}
		}
		for(otmp = invent; otmp; otmp = otmp->nobj) {
			if(otmp->oartifact) {
				choiceamount--;
				if (choiceamount <= 0) break; /* steal this one */
			}
		}
		if (!otmp) return;

		if (otmp) { /* we have something to snatch */
			if (otmp->owornmask)
				remove_worn_item(otmp, TRUE);
			freeinv(otmp);
		/* mpickobj wont merge otmp because none of the above things
		   to steal are mergable */

			if (StealDegrading || u.uprops[STEAL_DEGRADING].extrinsic || have_stealdegradestone()) {
				if (!stack_too_big(otmp)) curse(otmp);
				if (!stack_too_big(otmp) && otmp->spe > -20) otmp->spe--;
			}

			somethingtosteal = TRUE;
			(void) mpickobj(mtmp,otmp,FALSE);	/* may merge and free otmp */
			if (evilfriday) pline("Something seems missing...");
			else pline("%s stole %s!", Monnam(mtmp), doname(otmp));
			u.cnd_itemstealamount++;
			if (can_teleport(mtmp->data) && !tele_restrict(mtmp))
			(void) rloc(mtmp, FALSE);
		}

		if (!somethingtosteal && stealtype == 2 && rn2(50)) {
			stealtype = 1;
			goto tryagain;
		}

	}
}

#endif /* OVLB */
#ifdef OVL0

/* drop one object taken from a (possibly dead) monster's inventory */
STATIC_OVL void
mdrop_obj(mon, obj, verbosely)
struct monst *mon;
struct obj *obj;
boolean verbosely;
{
    int omx = mon->mx, omy = mon->my;

    if (obj->owornmask) {
	/* perform worn item handling if the monster is still alive */
	if (mon->mhp > 0) {
	    mon->misc_worn_check &= ~obj->owornmask;
	    update_mon_intrinsics(mon, obj, FALSE, TRUE);
	 /* obj_no_longer_held(obj); -- done by place_object */
	    if (obj->owornmask & W_WEP) setmnotwielded(mon, obj);
	/* don't charge for an owned saddle on dead steed */
	} else if (mon->mtame && (obj->owornmask & W_SADDLE) && 
		!obj->unpaid && costly_spot(omx, omy)) {
	    obj->no_charge = 1;
	}
	obj->owornmask = 0L;
    }
    obj->mstartinvent = 0;
    obj->mstartinventB = 0;
    obj->mstartinventC = 0;
    obj->mstartinventD = 0;
    if (verbosely && cansee(omx, omy))
	pline("%s drops %s.", Monnam(mon), distant_name(obj, doname));
    if (!flooreffects(obj, omx, omy, "fall")) {
	place_object(obj, omx, omy);
	stackobj(obj);
    }
}

/* some monsters bypass the normal rules for moving between levels or
   even leaving the game entirely; when that happens, prevent them from
   taking the Amulet or invocation tools with them */
void
mdrop_special_objs(mon)
struct monst *mon;
{
    struct obj *obj, *otmp;

    for (obj = mon->minvent; obj; obj = otmp) {
	otmp = obj->nobj;
	/* the Amulet, invocation tools, and Rider corpses resist even when
	   artifacts and ordinary objects are given 0% resistance chance */
	if (obj_resists(obj, 0, 0)) {
	    obj_extract_self(obj);
	    mdrop_obj(mon, obj, FALSE);
	}
    }
}

/* release the objects the creature is carrying */
void
relobj(mtmp,show,is_pet)
register struct monst *mtmp;
register int show;
boolean is_pet;		/* If true, pet should keep wielded/worn items */
{
	register struct obj *otmp;
	register int omx = mtmp->mx, omy = mtmp->my;
	struct obj *keepobj = 0;
	struct obj *wep = MON_WEP(mtmp);
	boolean item1 = FALSE, item2 = FALSE;

	if (!is_pet || mindless(mtmp->data) || is_animal(mtmp->data))
		item1 = item2 = TRUE;
	if (!tunnels(mtmp->data) || !needspick(mtmp->data))
		item1 = TRUE;

	while ((otmp = mtmp->minvent) != 0) {
		obj_extract_self(otmp);
		/* special case: pick-axe and unicorn horn are non-worn */
		/* items that we also want pets to keep 1 of */
		/* (It is a coincidence that these can also be wielded.) */
		if (otmp->owornmask || otmp == wep || otmp->mstartinvent || otmp->petmarked || otmp->mstartinventB || otmp->mstartinventC || otmp->mstartinventD ||
		    ((!item1 && otmp->otyp == PICK_AXE) ||
		     (!item2 && otmp->otyp == UNICORN_HORN && !otmp->cursed))) {
			if (is_pet && !evades_destruction(otmp)) { /* dont drop worn/wielded item */
				if (otmp->otyp == PICK_AXE)
					item1 = TRUE;
				if (otmp->otyp == UNICORN_HORN && !otmp->cursed)
					item2 = TRUE;
				otmp->nobj = keepobj;
				keepobj = otmp;
				continue;
			}
		}

		/* reduce amount of musable items the player can use --Amy */
		/* item stealers usually won't delete stuff, since their stuff might actually be your original stuff! */
		if (is_musable(otmp) && !(is_grenade(otmp) && otmp->oarmed) && otmp->mstartinvent && !(otmp->oartifact) && !(otmp->fakeartifact && timebasedlowerchance()) && (!rn2(3) || (rn2(100) < u.musableremovechance) || (rn2(4) && (otmp->otyp == POT_BLOOD || otmp->otyp == POT_VAMPIRE_BLOOD) ) || LootcutBug || u.uprops[LOOTCUT_BUG].extrinsic || have_lootcutstone() || !timebasedlowerchance() ) && !(mtmp->data == &mons[PM_GOOD_ITEM_MASTER]) && !(mtmp->data == &mons[PM_BAD_ITEM_MASTER]) && !is_pet ) {
			delobj(otmp);
		} else if (otmp->mstartinventB && !(is_grenade(otmp) && otmp->oarmed) && !(otmp->oartifact) && !(otmp->fakeartifact && timebasedlowerchance()) && (!rn2(4) || (rn2(100) < u.equipmentremovechance) || !timebasedlowerchance() ) && !(mtmp->data == &mons[PM_GOOD_ITEM_MASTER]) && !(mtmp->data == &mons[PM_BAD_ITEM_MASTER]) && !is_pet ) {
			delobj(otmp);
		} else if (otmp->mstartinventC && !(is_grenade(otmp) && otmp->oarmed) && !(otmp->oartifact) && !(otmp->fakeartifact && !rn2(10)) && rn2(10) && !(mtmp->data == &mons[PM_GOOD_ITEM_MASTER]) && !(mtmp->data == &mons[PM_BAD_ITEM_MASTER]) && !is_pet ) {
			delobj(otmp);
		} else if (otmp->mstartinventD && !(is_grenade(otmp) && otmp->oarmed) && !(otmp->oartifact) && !(otmp->fakeartifact && !rn2(4)) && rn2(4) && !(mtmp->data == &mons[PM_GOOD_ITEM_MASTER]) && !(mtmp->data == &mons[PM_BAD_ITEM_MASTER]) && !is_pet ) {
			delobj(otmp);
		} else mdrop_obj(mtmp, otmp, is_pet && flags.verbose);
	}

	/* put kept objects back */
	while ((otmp = keepobj) != (struct obj *)0) {
	    keepobj = otmp->nobj;
	    (void) add_to_minv(mtmp, otmp);
	}
#ifndef GOLDOBJ
	if (mtmp->mgold) {
		register long g = mtmp->mgold;
		(void) mkgold(g, omx, omy);
		if (is_pet && cansee(omx, omy) && flags.verbose)
			pline("%s drops %ld gold piece%s.", Monnam(mtmp),
				g, plur(g));
		mtmp->mgold = 0L;
	}
#endif
	
	if (show & cansee(omx, omy))
		newsym(omx, omy);
}

#endif /* OVL0 */

/*steal.c*/
