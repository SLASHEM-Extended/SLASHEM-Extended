/*	SCCS Id: @(#)do.c	3.4	2003/12/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* Contains code for 'd', 'D' (drop), '>', '<' (up, down) */

#include "hack.h"
#include "lev.h"
#include "quest.h"
#include "qtext.h"

# ifdef OVLB
STATIC_DCL void trycall(struct obj *);
# endif /* OVLB */
STATIC_DCL void dosinkring(struct obj *);
STATIC_DCL void dotoiletamulet(struct obj *);

STATIC_PTR int drop(struct obj *);
STATIC_PTR int wipeoff(void);
STATIC_PTR void lose_comp_objects(void);

#ifdef OVL0
STATIC_DCL int menu_drop(int);
#endif
#ifdef OVL2
STATIC_DCL int currentlevel_rewrite(void);
STATIC_DCL void final_level(void);
/* static boolean badspot(XCHAR_P,XCHAR_P); */
#endif

#ifdef OVLB

static NEARDATA const char drop_types[] =
	{ ALLOW_COUNT, COIN_CLASS, ALL_CLASSES, 0 };

/* 'd' command: drop one inventory item */
int
dodrop()
{

	if (NoDropProblem || u.uprops[DROP_BUG].extrinsic || have_dropbugstone() || (uamul && uamul->oartifact == ART_ARABELLA_S_SWOONING_BEAUTY) ) {

		pline("For some reason, you cannot drop items!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;

	}

#ifndef GOLDOBJ
	int result, i = (invent || u.ugold) ? 0 : (SIZE(drop_types) - 1);
#else
	int result, i = (invent) ? 0 : (SIZE(drop_types) - 1);
#endif

	if (*u.ushops) sellobj_state(SELL_DELIBERATE);
	result = drop(getobj(&drop_types[i], "drop"));
	if (*u.ushops) sellobj_state(SELL_NORMAL);
	reset_occupations();

	return result;
}

#endif /* OVLB */
#ifdef OVL0

STATIC_PTR void
lose_comp_objects()
{
	struct obj *otmp,*on;
	int x;

	x = 0;
	for (otmp=invent;otmp;) {
		on = otmp->nobj;
		if (otmp->otyp == RIN_TIMELY_BACKUP 
		||  otmp->otyp == SCR_ROOT_PASSWORD_DETECTION
		||  otmp->otyp == AMULET_OF_DATA_STORAGE
		||  otmp->otyp == WAN_BUGGING ) {
			pline("%s%s vanish%s!",(x++) ? "" : "All of a sudden, ",doname(otmp),(otmp->quan > 1) ? "" : "es");
			setnotworn(otmp);
			freeinv(otmp);
			obfree(otmp, (struct obj *) 0);
		}
		otmp = on;
	}
}



/* Called when a boulder is dropped, thrown, or pushed.  If it ends up
 * in a pool, it either fills the pool up or sinks away.  In either case,
 * it's gone for good...  If the destination is not a pool, returns FALSE.
 */
boolean
boulder_hits_pool(otmp, rx, ry, pushing)
struct obj *otmp;
register int rx, ry;
boolean pushing;
{
	if (!otmp || otmp->otyp != BOULDER)
	    impossible("Not a boulder?");
	else if (!Is_waterlevel(&u.uz) && (is_pool(rx,ry) || is_lava(rx,ry))) {
	    boolean lava = is_lava(rx,ry), fills_up;
	    const char *what = waterbody_name(rx,ry);
	    schar ltyp = levl[rx][ry].typ;
	    int chance = rn2(10);		/* water: 90%; lava: 10% */
	    fills_up = lava ? chance == 0 : chance != 0;

	    if (fills_up) {
		struct trap *ttmp = t_at(rx, ry);

		if (ltyp == DRAWBRIDGE_UP) {
		    levl[rx][ry].drawbridgemask &= ~DB_UNDER; /* clear lava */
		    levl[rx][ry].drawbridgemask |= DB_FLOOR;
		} else
		    levl[rx][ry].typ = ROOM;

		if (ttmp) (void) delfloortrap(ttmp);
		bury_objs(rx, ry);
		
		newsym(rx,ry);
		if (pushing) {
		    You("push %s into the %s.", the(xname(otmp)), what);
		    if (flags.verbose && !Blind)
			pline("Now you can cross it!");
		    /* no splashing in this case */
		}
	    }
	    if (!fills_up || !pushing) {	/* splashing occurs */
		if (!u.uinwater) {
		    if (pushing ? !Blind : cansee(rx,ry)) {
			There("is a large splash as %s %s the %s.",
			      the(xname(otmp)), fills_up? "fills":"falls into",
			      what);
		    } else if (flags.soundok)
			You_hear("a%s splash.", lava ? " sizzling" : "");
		    wake_nearto(rx, ry, 40);
		}

		if (fills_up && u.uinwater && distu(rx,ry) == 0) {
		    u.uinwater = 0;
		    docrt();
		    vision_full_recalc = 1;
		    You("find yourself on dry land again!");
		} else if (lava && distu(rx,ry) <= 2) {
		    You("are hit by molten lava%c",
			Fire_resistance ? '.' : '!');
			burn_away_slime();
		    if (Slimed) {
			pline("The slime is burned off!");
			Slimed =0;
		    }
		    losehp(d((Fire_resistance ? 1 : 3), 6),
			   "molten lava", KILLED_BY);
		} else if (!fills_up && flags.verbose &&
			   (pushing ? !Blind : cansee(rx,ry)))
		    pline("It sinks without a trace!");
	    }

	    /* boulder is now gone */
	    if (pushing) delobj(otmp);
	    else obfree(otmp, (struct obj *)0);
	    return TRUE;
	}
	return FALSE;
}

/* Used for objects which sometimes do special things when dropped; must be
 * called with the object not in any chain.  Returns TRUE if the object goes
 * away.
 */
boolean
flooreffects(obj,x,y,verb)
struct obj *obj;
int x,y;
const char *verb;
{
	struct trap *t;
	struct monst *mtmp;

	if (obj->where != OBJ_FREE)
	    panic("flooreffects: obj not free, %d", obj->where);

	/* make sure things like water_damage() have no pointers to follow */
	obj->nobj = obj->nexthere = (struct obj *)0;

	if (obj->otyp == BOULDER && boulder_hits_pool(obj, x, y, FALSE))
		return TRUE;
	else if (obj->otyp == BOULDER && (t = t_at(x,y)) != 0 &&
		 (t->ttyp == PIT || t->ttyp == SPIKED_PIT || t->ttyp == GIANT_CHASM || t->ttyp == SHIT_PIT || 
			t->ttyp == MANA_PIT || t->ttyp == ANOXIC_PIT || t->ttyp == ACID_PIT || t->ttyp == SHAFT_TRAP || t->ttyp == CURRENT_SHAFT
			|| t->ttyp == TRAPDOOR || t->ttyp == HOLE)) {
		if (((mtmp = m_at(x, y)) && mtmp->mtrapped) ||
			(u.utrap && u.ux == x && u.uy == y)) {
		    if (*verb)
			pline_The("boulder %s into the pit%s.",
				vtense((const char *)0, verb),
				(mtmp) ? "" : " with you");
		    if (mtmp) {
			if (!passes_walls(mtmp->data) && (!mtmp->egotype_wallwalk) &&
				!throws_rocks(mtmp->data)) {
				int dieroll = rnd(20);
			    if (hmon(mtmp, obj, 3, dieroll) && !is_whirly(mtmp->data))
				return FALSE;	/* still alive */
			}
			mtmp->mtrapped = 0;
		    } else {
			if (!Passes_walls && !throws_rocks(youmonst.data)) {
			    losehp(rnd(15), "squished under a boulder",
				   NO_KILLER_PREFIX);
			    return FALSE;	/* player remains trapped */
			} else u.utrap = 0;
		    }
		}
		if (*verb) {
			if (Blind) {
				if ((x == u.ux) && (y == u.uy))
					You_hear("a CRASH! beneath you.");
				else
					You_hear("the boulder %s.", verb);
			} else if (cansee(x, y)) {
				pline_The("boulder %s%s.",
				    t->tseen ? "" : "triggers and ",
				    t->ttyp == TRAPDOOR ? "plugs a trap door" :
				    t->ttyp == SHAFT_TRAP ? "plugs a shaft" :
				    t->ttyp == CURRENT_SHAFT ? "plugs a shaft" :
				    t->ttyp == HOLE ? "plugs a hole" :
				    "fills a pit");
			}
		}
		if (t && !(t->ttyp == GIANT_CHASM && !In_sokoban(&u.uz)) ) deltrap(t);
		obfree(obj, (struct obj *)0);
		bury_objs(x, y);
		newsym(x,y);
		return TRUE;
	} else if (is_lava(x, y)) {
		return fire_damage(obj, FALSE, FALSE, x, y);
	} else if (is_waterypool(x, y) || is_watertunnel(x, y)) {
		/* Reasonably bulky objects (arbitrary) splash when dropped.
		 * If you're floating above the water even small things make noise.
		 * Stuff dropped near fountains always misses */
		if ((Blind || (Levitation || Flying)) && flags.soundok &&
		    ((x == u.ux) && (y == u.uy))) {
		    if (!Underwater) {
			if (weight(obj) > 9) {
				pline("Splash!");
		        } else if (Levitation || Flying) {
				pline("Plop!");
		        }
		    }
		    map_background(x, y, 0);
		    newsym(x, y);
		}
		water_damage(obj, FALSE, FALSE);
	} else if (u.ux == x && u.uy == y &&
		(!u.utrap || u.utraptype != TT_PIT) &&
		(t = t_at(x,y)) != 0 && t->tseen &&
			(t->ttyp == PIT || t->ttyp == SPIKED_PIT || t->ttyp == GIANT_CHASM
			|| t->ttyp == SHIT_PIT || t->ttyp == MANA_PIT || t->ttyp == ANOXIC_PIT || t->ttyp == ACID_PIT)) {
		/* you escaped a pit and are standing on the precipice */
		if (Blind && flags.soundok)
			You_hear("%s %s downwards.",
				The(xname(obj)), otense(obj, "tumble"));
		else
			pline("%s %s into %s pit.",
				The(xname(obj)), otense(obj, "tumble"),
				the_your[t->madeby_u]);
	}
	if (is_lightsaber(obj) && obj->lamplit) {
		if (cansee(x, y)) You("see %s deactivate.", an(xname(obj)));
		lightsaber_deactivate(obj, TRUE);
	}
	return FALSE;
}


#endif /* OVL0 */
#ifdef OVLB

void
doaltarobj(obj)  /* obj is an object dropped on an altar */
	register struct obj *obj;
{
	/* "Removed altars disappearing while using them. This made little sense and made the game needlessly more difficult." In Soviet Russia, people want to be able to indefinitely determine BUC status of objects as soon as they find a single altar. They also really love sacfesting, which they can do for 25 hours each day without getting tired. Heck, they could probably do pudding farming for 25 hours each day, combined with sacfesting if there's an altar on their farm level, and not get bored either! But I'm assuming that Slash'EM Extended is played by real players who don't want their senses to get dulled from doing some repetitive thing for hours on end. Altars disappearing is there to limit the potential abuse and prevent endless sacfesting. Yes, you aren't supposed to get a guaranteed Magicbane on your first altar. You should be grateful if you do get it since it's freaking powerful compared to non-artifacts, and so you have to work for it, possibly using up many altars, or maybe you'll never get it if you're unlucky - that's the life! --Amy */

	/* high altars don't tell you the BUC of an item, partly as extra insurance against altar vanishing
	 * but also to make sure that you can't indefinitely identify BUC on the astral plane --Amy */
	if (Is_astralevel(&u.uz) || Is_sanctum(&u.uz)) return;

	if (Race_if(PM_MACTHEIST)) return;

	if (practicantterror && !u.pract_altartest) {
		u.pract_altartestamount++;
		if (u.pract_altartestamount >= 10) {
			pline("%s booms: 'Well, now your memory simply belongs to me. Why do you try to test your equipment on an altar anyway?'", noroelaname());
			forget(1 + rn2(5));
			u.pract_altartest = TRUE;
		}
	}

	if (u.uprops[DESECRATION].extrinsic || Desecration || have_nonsacredstone() ) {

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if (!rn2(5)) badeffect();
		if (!rn2(3)) (void) makemon(mkclass(S_GHOST,0), u.ux, u.uy, MM_ANGRY);
		if (!rn2(5)) (void) makemon(mkclass(S_MIMIC,0), u.ux, u.uy, MM_ANGRY);
		if (!rn2(10)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY);
		if (!rn2(10)) (void) makemon(mkclass(S_XAN,0), u.ux, u.uy, MM_ANGRY);
		if (!rn2(10)) (void) makemon(mkclass(S_LIGHT,0), u.ux, u.uy, MM_ANGRY);
		if (!rn2(10)) (void) makemon(mkclass(S_ELEMENTAL,0), u.ux, u.uy, MM_ANGRY);
		if (!rn2(10)) (void) makemon(mkclass(S_MUMMY,0), u.ux, u.uy, MM_ANGRY);
		if (!rn2(10)) (void) makemon(mkclass(S_ZOMBIE,0), u.ux, u.uy, MM_ANGRY);
		if (!rn2(10)) (void) makemon(mkclass(S_GOLEM,0), u.ux, u.uy, MM_ANGRY);
		if (!rn2(10)) (void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ANGRY);
		if (!rn2(10)) (void) makemon(mkclass(S_GRUE,0), u.ux, u.uy, MM_ANGRY);
		if (!rn2(10)) (void) makemon(mkclass(S_RUBMONST,0), u.ux, u.uy, MM_ANGRY);
		if (!rn2(15)) (void) makemon(mkclass(S_NYMPH,0), u.ux, u.uy, MM_ANGRY);
		if (!rn2(15)) (void) makemon(mkclass(S_RUSTMONST,0), u.ux, u.uy, MM_ANGRY);
		if (!rn2(50)) (void) makemon(mkclass(S_LICH,0), u.ux, u.uy, MM_ANGRY);

		u.aggravation = 0;

	}

	if (issoviet && !rn2(50)) {
		pline("Tip bloka l'da reshayet nakazat' vas. Khar Khar Khar Khar Khar Khar Khar!");
		badeffect();
	}

	if (!rn2(findpriest(temple_occupied(u.urooms)) ? 500 : 100) && !obj->bknown && !(obj && obj->otyp == POT_WATER) && !issoviet && (!Is_astralevel(&u.uz)) ) {
		levl[u.ux][u.uy].typ = ROOM;
		pline_The("altar suddenly vanishes!"); /* Yes, we're preventing altar abuse here, or trying to, at least. --Amy */
		newsym(u.ux,u.uy);
		return;
	} else if (!rn2(findpriest(temple_occupied(u.urooms)) ? 500 : 100) && isfriday && !obj->bknown && !(obj && obj->otyp == POT_WATER) && !issoviet && (!Is_astralevel(&u.uz)) ) {
		levl[u.ux][u.uy].typ = ROOM;
		pline_The("altar suddenly vanishes!"); /* Yes, we're preventing altar abuse here, or trying to, at least. --Amy */
		newsym(u.ux,u.uy);
		return;
	} else if (!rn2(10) && (!Is_astralevel(&u.uz)) && (u.uprops[DESECRATION].extrinsic || Desecration || have_nonsacredstone() ) ) {
		levl[u.ux][u.uy].typ = ROOM;
		pline_The("altar suddenly vanishes!");
		newsym(u.ux,u.uy);
		return;
	}

	/* Amy edit: But if the item dropped on the altar is a potion of water, the player is probably trying to do a water
	 * prayer. In this case, don't remove the altar. Also, it was stupid that if you knew the BUC of the object, there
	 * still was a chance of the altar disappearing. So I fixed those things at last. */

	if (Blind)
		return;

	/* KMH, conduct */
	u.uconduct.gnostic++;
	if (Race_if(PM_MAGYAR)) {
		You_feel("bad about breaking the atheist conduct.");
		badeffect();
	}

	/* evil patch idea by aosdict: Moloch's altars can occasionally curse the item. */

	if (!obj->cursed && !rn2(isfriday ? 100 : 500) && !obj->bknown && levl[u.ux][u.uy].altarmask == AM_NONE) curse(obj);
	if (!rn2(5) && !obj->bknown && (u.uprops[DESECRATION].extrinsic || Desecration || have_nonsacredstone()) ) curse(obj);

	if ((obj->blessed || obj->cursed) && obj->oclass != COIN_CLASS) {
		There("is %s flash as %s %s the altar.",
			an(hcolor(obj->blessed ? NH_AMBER : NH_BLACK)),
			doname(obj), otense(obj, "hit"));
		if (!Hallucination && !(LeftInventoryBug || u.uprops[LEFT_INVENTORY].extrinsic || have_leftinventorystone()) ) {
			/* prevent skill farming if the player drops individual rocks or stuff --Amy */
			if (!obj->bknown && !objects[obj->otyp].oc_merge)
				use_skill(P_SPIRITUALITY,3);
			obj->bknown = 1;
			/* the more items you test, the more likely you'll recognize BUC on future items --Amy */
			if (u.bucskill < 2 || !rn2(u.bucskill)) {
				u.bucskill++;
				if (u.bucskill > 250) u.bucskill = 250;
			}
		}
	} else {
		pline("%s %s on the altar.", Doname2(obj),
			otense(obj, "land"));
		if (!obj->bknown && !objects[obj->otyp].oc_merge && !(LeftInventoryBug || u.uprops[LEFT_INVENTORY].extrinsic || have_leftinventorystone()) )
			use_skill(P_SPIRITUALITY,3);
		obj->bknown = 1;
		if (u.bucskill < 2 || !rn2(u.bucskill)) {
			u.bucskill++;
			if (u.bucskill > 250) u.bucskill = 250;
		}
	}
}

STATIC_OVL
void
trycall(obj)
register struct obj *obj;
{
	if(!objects[obj->otyp].oc_name_known &&
	   !objects[obj->otyp].oc_uname)
	   docall(obj);
}

STATIC_OVL
void
dosinkring(obj)  /* obj is a ring being dropped over a kitchen sink */
register struct obj *obj;
{
	register struct obj *otmp,*otmp2;
	register boolean ideed = TRUE;

	You("drop %s down the drain.", doname(obj));
	obj->in_use = TRUE;	/* block free identification via interrupt */
	u.cnd_sinkamount++;

	if (!rn2(isfriday ? 3 : 10)) { /* we don't want them to be endless; ring is now lost without a message */
		pline_The("pipes break!  Water spurts out!");
		level.flags.nsinks--;
		levl[u.ux][u.uy].typ = FOUNTAIN;
		level.flags.nfountains++;
		newsym(u.ux,u.uy);
		useup(obj);
		return;
	}

	if (isfriday && !rn2(10)) goto fridaydone;

	switch(obj->otyp) {	/* effects that can be noticed without eyes */
	    case RIN_SEARCHING:
		You("thought your %s got lost in the sink, but there it is!",
			xname(obj));
		goto giveback;
/* KMH, balance patch -- now an amulet */
		/* re-enabled by Amy */
	    case RIN_DRAIN_RESISTANCE:
		pline("The sink looks weaker for a moment, but it passes.");
		pline_The("ring refuses to go down the drain!"); /* drain resistance, geddit? --Amy */
		goto giveback;
	    case RIN_SLOW_DIGESTION:
		pline_The("ring is regurgitated!");
giveback:
		obj->in_use = FALSE;
		dropx(obj);
		trycall(obj);
		return;
	    case RIN_LEVITATION:
		pline_The("sink quivers upward for a moment.");
		break;
	    case RIN_POISON_RESISTANCE:
		You("smell rotten %s.", makeplural(fruitname(FALSE)));
		break;
	    case RIN_AGGRAVATE_MONSTER:
		pline("Several flies buzz angrily around the sink.");
		break;
	    case RIN_SHOCK_RESISTANCE:
		pline("Static electricity surrounds the sink.");
		break;
	    case RIN_CONFLICT:
		You_hear("loud noises coming from the drain.");
		break;
	    case RIN_SLEEPING:		/* ALI */
		You_hear("loud snores coming from the drain.");
		break;
	    case RIN_SUSTAIN_ABILITY:	/* KMH */
		pline_The("water flow seems fixed.");
		break;
	    case RIN_GAIN_STRENGTH:
		pline_The("water flow seems %ser now.",
			(obj->spe<0) ? "weak" : "strong");
		break;
	    case RIN_GAIN_CONSTITUTION:
		pline_The("water flow seems %ser now.",
			(obj->spe<0) ? "less" : "great");
		break;
	    case RIN_GAIN_INTELLIGENCE:
	    case RIN_GAIN_WISDOM:
		pline("The water flow seems %ser now.",
			(obj->spe<0) ? "dull" : "quick");
		break;
	    case RIN_GAIN_DEXTERITY:
		pline("The water flow seems %ser now.",
			(obj->spe<0) ? "slow" : "fast");
		break;
	    case RIN_INCREASE_ACCURACY:	/* KMH */
		pline_The("water flow %s the drain.",
			(obj->spe<0) ? "misses" : "hits");
		break;
	    case RIN_INCREASE_DAMAGE:
		pline_The("water's force seems %ser now.",
			(obj->spe<0) ? "small" : "great");
		break;
	    case RIN_HUNGER:
		ideed = FALSE;
		for(otmp = level.objects[u.ux][u.uy]; otmp; otmp = otmp2) {
		    otmp2 = otmp->nexthere;
		    if (otmp != uball && otmp != uchain &&
			    !obj_resists(otmp, 1, 99)) {
			if (!Blind) {
			    pline("Suddenly, %s %s from the sink!",
				  doname(otmp), otense(otmp, "vanish"));
			    ideed = TRUE;
			}
			delobj(otmp);
		    }
		}
		break;
	    case MEAT_RING:
		/* Not the same as aggravate monster; besides, it's obvious. */
		pline("Several flies buzz around the sink.");
		break;
	    default:
		ideed = FALSE;
		break;
	}
	if(!Blind && !ideed && obj->otyp != RIN_HUNGER) {
	    ideed = TRUE;
	    switch(obj->otyp) {		/* effects that need eyes */
		case RIN_ADORNMENT:
		    pline_The("faucets flash brightly for a moment.");
		    break;
		case RIN_REGENERATION:
		    pline_The("sink looks as good as new.");
		    break;
		case RIN_PRACTICE:
		    pline_The("water flow seems %ser now.",
			(obj->cursed) ? "hard" : "easi");
		    break;
		case RIN_HEAVY_ATTACK:
		    pline_The("water's power seems %ser now.",
			(obj->spe<0) ? "small" : "great");
		    break;
		case RIN_CONFUSION_RESISTANCE:
		    pline_The("water flow seems straighter now.");
		    break;
		case RIN_RESTRATION:
		    pline_The("drain stops vibrating for a moment.");
		    break;
		case RIN_LOOTCUT:
		    pline("No water is dropping down the sink anymore.");
		    break;
		case RIN_FORM_SHIFTING:
		    pline("The sink is rapidly changing shape.");
		    break;
		case RIN_LAGGING:
		    pline("The water flow seems to be delayed!");
		    break;
		case RIN_BLESSCURSING:
		    pline("The sink first flashes %s, then %s.", NH_SILVER, NH_BLACK);
		    break;
		case RIN_ILLITERACY:
		    pline("The sink seems to be breaking apart at the seams.");
		    break;
		case RIN_STAT_DECREASE:
		    pline("The water stream seems to die of hunger.");
		    break;
		case RIN_SANITY_TIMEOUT:
		    pline("The water flow exposes quite a lot of muck in the sink.");
		    break;

		case RIN_WIMPINESS:
		    pline("Somehow the sink seems powerless.");
		    break;
		case RIN_USING_HAZARD:
		    pline("Suddenly you dread using the sink.");
		    break;
		case RIN_EXERTION_LOSS:
		    pline("The water flow just doesn't seem to get any speed!");
		    break;
		case RIN_PETCAT:
		    pline("The water is flowing in completely random directions!");
		    break;
		case RIN_POSSESSION_IDENTIFICATION:
		    if (evilfriday) {
				char qbuf[QBUFSZ];
				char possid = 0;
				sprintf(qbuf, "Do you want your possessions identified? DYWYPI?");
				possid = yn_function(qbuf, ynqchars, 'y');
				if (possid != 'n') {
					u.youaredead = 1;
					pline("Okay. Game over. If you wanted to go on playing, better hope you have an amulet of life saving because otherwise we're done here.");
					killer_format = KILLED_BY;
					killer = "wanting their possessions identified";
					done(DIED);
					u.youaredead = 0;
				}
				break;
		    }
		    pline("Do you want your possessions identified? DYWYPI? [ynq] (n) _");
		    break;
		case RIN_DAYSHIFT:
		    pline(night() ? "Suddenly the sink seems very dark." : "The sink is shining brightly.");
		    break;
		case RIN_DECONSTRUCTION:
		    pline("A screw is knocked loose from the sink.");
		    break;

	      case RIN_SICKNESS_RESISTANCE:
		    pline("The sink looks clean and neat for a moment.");
		    break;
	      case RIN_JUMPING:
		    pline("The sink jumps up and down!");
		    break;
	      case RIN_MAGIC_CONTROL:
		    pline("The water flow seems controlled!");
		    break;
	      case RIN_ILLNESS:
		    pline("The sink is overgrown with fungus.");
		    break;
	      case RIN_DISARMING:
		    pline("The water flow pauses for a moment.");
		    break;
	      case RIN_ALACRITY:
		    pline("The water flow becomes extremely fast for a moment!");
		    break;
	      case RIN_NASTINESS:
		    pline("The water looks extremely disgusting for a moment.");
		    break;
	      case RIN_BAD_EFFECT:
		    pline("The water looks really icky for a moment.");
		    break;
	      case RIN_SUPERSCROLLING:
		    pline("The water flow seems to be stuck in an infinite loop.");
		    break;
	      case RIN_ANTI_DROP:
		    pline("No water is flowing down the sink anymore.");
		    break;
	      case RIN_ENSNARING:
		    pline("The sink actively stops the water flow in its tracks!");
		    break;
	      case RIN_DIARRHEA:
		    pline("The sink water turns into pee... ewww...");
		    break;
	      case RIN_DISENGRAVING:
		    pline("Some writing seems to vanish from the water flow!");
		    break;
		case RIN_NO_SKILL:
		    pline("The water flow seems to stop completely!");
		    break;
		case RIN_LOW_STATS:
		    pline("The water flow slows down to a crawl.");
		    break;
		case RIN_FAILED_TRAINING:
		    pline("The water doesn't seem to find its way downward any longer!");
		    break;
		case RIN_FAILED_EXERCISE:
		    pline("The water flows toward the drain but just can't seem to reach it!");
		    break;
		case RIN_AUTOCURSING:
		    pline("The sink seems to be cursed!");
		    break;
		case RIN_TIME_SPENDING:
		    pline("The sink seems to age rapidly!");
		    break;
		case RIN_FAST_METABOLISM:
		    pline("The water vanishes down the drain incredibly quickly!");
		    break;

	      case RIN_NUMBNESS:
		    pline("The water flow stutters for a moment.");
		    break;
	      case RIN_CURSE:
		    pline("A malignant aura seems to surround the sink...");
		    break;
	      case RIN_DIMNESS:
		    pline("The sink's color appears to be less saturated.");
		    break;

	      case RIN_DOOM:
		    pline("You behold the unspeakable visage of the sink!");
		    break;
	      case RIN_ELEMENTS:
		    pline("The sink emits a three-colored flash!");
		    break;
	      case RIN_LIGHT:
		    pline("The sink shines brightly for a few seconds!");
		    break;
	      case RIN_MAGIC_RESISTANCE:
		    pline("The sink is momentarily surrounded by an anti-magic shell.");
		    break;
	      case RIN_MATERIAL_STABILITY:
		    pline("The sink seems to be reintegrating.");
		    break;
	      case RIN_RANDOM_EFFECTS:
	      case RIN_SPECIAL_EFFECTS:
		    pline("You are not sure if there is anything happening to the sink at all.");
		    break;
	      case RIN_MIND_SHIELDING:
		    pline("A %s aura surrounds the sink.", NH_PURPLE);
		    break;

	      case RIN_BLOOD_LOSS:
		    pline("The sink looks like it's crying...");
		    break;
	      case RIN_INTRINSIC_LOSS:
		    pline("Some parts of the sink seem missing...");
		    break;
	      case RIN_TRAP_REVEALING:
		    You("realize more details of the sink's architecture!");
		    break;
	      case RIN_HALLUCINATION:
		    pline("The sink flashes in wonderful rainbow colors!");
		    break;
		case RIN_INVISIBILITY:
		    You("don't see anything happen to the sink.");
		    break;
	    case RIN_TIMELY_BACKUP:
		pline("The sink seems undestroyable.");
		break;
		case RIN_FREE_ACTION:
		    You("see the ring slide right down the drain!");
		    break;
		case RIN_IMMUNITY_TO_DRAWBRIDGES:
		    pline("A portcullis crashes onto the sink, but the sink is unharmed!");
		    break;
		case RIN_DISCOUNT_ACTION:
		    pline("Waterdrops are quickly running down the drain.");
		    break;
		case RIN_DIMINISHED_BLEEDING:
		    pline("The waterdrops seem to be slowing down.");
		    break;
		case RIN_SEE_INVISIBLE:
		    You("see some air in the sink.");
		    break;
		case RIN_LEECH:
		    pline("The sink drains energy from the ring!");
		    break;
		case RIN_DANGER:
		    pline("Terrible things are happening to the sink...");
		    break;
		case RIN_INFRAVISION:
		    pline("The sink seems to glow %s for a moment!", NH_RED);
		    break;
		case RIN_STEALTH:
		pline_The("sink seems to blend into the floor for a moment.");
		    break;
		case RIN_FIRE_RESISTANCE:
		pline_The("hot water faucet flashes brightly for a moment.");
		    break;
		case RIN_COLD_RESISTANCE:
		pline_The("cold water faucet flashes brightly for a moment.");
		    break;
		case RIN_FEAR_RESISTANCE:
		pline_The("water seems to flow more steadily.");
		    break;
		case RIN_PROTECTION_FROM_SHAPE_CHAN:
		    pline_The("sink looks nothing like a fountain.");
		    break;
		case RIN_PROTECTION:
		    pline_The("sink glows %s for a moment.",
			    hcolor((obj->spe<0) ? NH_BLACK : NH_SILVER));
		    break;
		case RIN_WARNING:
		    pline_The("sink glows %s for a moment.", hcolor(NH_WHITE));
		    break;
		case RIN_MOOD:
			pline_The("sink looks groovy.");
			break;
		case RIN_TELEPORTATION:
		    pline_The("sink momentarily vanishes.");
		    break;
		case RIN_TELEPORT_CONTROL:
	    pline_The("sink looks like it is being beamed aboard somewhere.");
		    break;
		case RIN_POLYMORPH:
		    pline_The("sink momentarily looks like a fountain.");
		    break;
		case RIN_POLYMORPH_CONTROL:
	pline_The("sink momentarily looks like a regularly erupting geyser.");
		    break;
		case RIN_MEMORY:
		    pline_The("sink looks exactly as you remember it.");
		    break;
	    }
	}
fridaydone:

	if(ideed)
	    trycall(obj);
	else
	    You_hear("the ring bouncing down the drainpipe.");
	/* Amy edit: identifying rings is already very hard, so I'll allow rings to get regurgitated every other time. Of
	 * course this is a no-go in Soviet Russia (like every other change made by the antichrist that is Amy), because
	 * using unorthodox ways to identify items? No way! Communism dictates that there is exactly one way to identify
	 * your items, doing anything out of the ordinary invokes the wrath of the Kreml! */
	if (!rn2(issoviet ? 20 : 2)) {
		pline_The("sink backs up, leaving %s.", doname(obj));
		obj->in_use = FALSE;
		dropx(obj);
	} else
		useup(obj);
}

/* Dropping amulets down a toilet can help identify them, and most of the time you get the amulet back.
 * But the amulet can be cursed or rusted in the process. If it doesn't come back, monsters are spawned. --Amy */
STATIC_OVL
void
dotoiletamulet(obj)  /* obj is an amulet being dropped over a toilet */
register struct obj *obj;
{
	register struct obj *otmp;
	register boolean getitback = rn2(4);

	/* you can't drop the Amulet of Yendor anyway, but in case this function is somehow called with it... */
	if (obj->otyp == AMULET_OF_YENDOR || obj->otyp == FAKE_AMULET_OF_YENDOR) return;

	u.cnd_toiletamount++;

	You("drop %s down the drain.", doname(obj));
	obj->in_use = TRUE;	/* block free identification via interrupt */

	if (!rn2(isfriday ? 3 : 10)) { /* we don't want them to be endless; amulet is now lost without a message */
		pline_The("pipes break!  Water spurts out!");
		level.flags.nsinks--;
		levl[u.ux][u.uy].typ = FOUNTAIN;
		level.flags.nfountains++;
		newsym(u.ux,u.uy);
		useup(obj);
		return;
	}

	if (isfriday && !rn2(10)) goto fridaydone2;

	/* I allow you to "observe" this even if you're blind --Amy */
	switch(obj->otyp) {
	case AMULET_OF_CHANGE:
		pline("Suddenly the toilet seems to be made for the opposite gender!");
		break;
	case AMULET_OF_PEACE:
		pline("It seems the toilet has found inner peace.");
		break;
	case AMULET_OF_POLYMORPH:
		pline_The("toilet seems to be a fountain for a moment.");
		break;
	case AMULET_OF_DRAIN_RESISTANCE:
		pline_The("toilet water no longer flows down the drain!");
		break;
	case AMULET_OF_ESP:
		You("think you saw a rat in the toilet!");
		break;
	case AMULET_OF_UNDEAD_WARNING:
		pline_The("toilet seems to be afraid of the walking dead!");
		break;
	case AMULET_OF_OWN_RACE_WARNING:
		pline_The("toilet seems to be afraid of other toilets!");
		break;
	case AMULET_OF_POISON_WARNING:
		pline_The("toilet seems to be afraid of venomous pee!");
		break;
	case AMULET_OF_COVETOUS_WARNING:
		pline_The("toilet seems to be afraid of being used without a warning!");
		break;
	case AMULET_OF_FLYING:
		pline_The("toilet soars to the %s, then lands again.", ceiling(u.ux, u.uy));
		break;
	case AMULET_OF_LIFE_SAVING:
		pline_The("toilet gains an extra life!");
		break;
	case AMULET_OF_MAGICAL_BREATHING:
		pline_The("toilet water flows down the drain without requiring the flushing to be operated!");
		break;
	case AMULET_OF_REFLECTION:
		pline_The("toilet water seems to come back out of the drain!");
		break;
	case AMULET_OF_RESTFUL_SLEEP:
		pline_The("toilet doesn't feel like operating.");
		break;
	case AMULET_OF_BLINDNESS:
		You("can't see whether something is happening to the toilet!");
		break;
	case AMULET_OF_STRANGULATION:
		pline_The("toilet seems to scream in agony silently.");
		break;
	case AMULET_OF_PREMATURE_DEATH:
		pline_The("toilet suddenly seems to fall apart.");
		break;
	case AMULET_OF_UNCHANGING:
		pline_The("toilet seems indestructible.");
		break;
	case AMULET_VERSUS_POISON:
		pline_The("dirty toilet water turns clear.");
		break;
	case AMULET_OF_ANTI_TELEPORTATION:
		pline_The("toilet seems to be anchored to the spot.");
		break;
	case AMULET_VERSUS_STONE:
		pline_The("toilet doesn't turn to stone.");
		break;
	case AMULET_OF_TIME:
		pline_The("toilet suddenly displays the current time: %d:%d", getlt()->tm_hour, getlt()->tm_min);
		break;
	case AMULET_OF_DEPETRIFY:
		pline_The("toilet is definitely made of porcelain. Not mineral.");
		break;
	case AMULET_OF_PET_VIEW:
		pline("It seems as if one of your pets was being pulled down the toilet!");
		break;
	case AMULET_OF_MAGIC_RESISTANCE:
		pline_The("toilet is surrounded by a magical shield!");
		break;
	case AMULET_OF_SICKNESS_RESISTANCE:
		pline_The("toilet no longer smells of urine and crap.");
		break;
	case AMULET_OF_SWIMMING:
		pline("Some garbage floats up to the toilet water's surface.");
		break;
	case AMULET_OF_DISINTEGRATION_RESIS:
		pline_The("toilet seems very firm.");
		break;
	case AMULET_OF_ACID_RESISTANCE:
		pline_The("toilet seems resistant to cleaning vinegar solutions.");
		break;
	case AMULET_OF_REGENERATION:
		pline_The("toilet seems to be self-repairing.");
		break;
	case AMULET_OF_CONFLICT:
		pline_The("toilet drones loudly!");
		break;
	case AMULET_OF_FUMBLING:
		You_feel("that your stream of pee missed the toilet!");
		break;
	case AMULET_OF_SECOND_CHANCE:
		pline_The("toilet breaks apart and reintegrates!");
		break;
	case AMULET_OF_DATA_STORAGE:
		pline_The("toilet seems absolutely safe.");
		break;
	case AMULET_OF_WATERWALKING:
		pline_The("toilet paper swims on the water without getting wet!");
		break;
	case AMULET_OF_HUNGER:
		pline_The("toilet flushes very quickly!");
		break;
	case AMULET_OF_POWER:
		pline_The("flushing seems to create much more water than usual!");
		break;
	case AMULET_OF_LEECH:
		pline_The("toilet seems to be drawing energy from the surroundings.");
		break;
	case AMULET_OF_DANGER:
		You("suddenly develop a fear of toilets!");
		break;
	case AMULET_OF_INSOMNIA:
		pline_The("toilet seems ready to operate forever.");
		break;
	case AMULET_OF_MENTAL_STABILITY:
		pline_The("toilet seems to be operating steadily.");
		break;
	case AMULET_OF_CONTAMINATION_RESIST:
		pline_The("toilet water cleans instantly!");
		break;
	case AMULET_OF_SCENT:
		pline_The("toilet suddenly smells like roses!");
		break;
	case AMULET_OF_THE_RNG:
	case AMULET_OF_INFINITY:
		pline("Something strange is happening to the toilet.");
		break;
	case AMULET_OF_SPEED:
		pline_The("toilet flow seems faster.");
		break;
	case AMULET_OF_POLYMORPH_CONTROL:
		pline_The("toilet seems to be a regularly erupting geyser for a moment.");
		break;
	case AMULET_OF_TELEPORT_CONTROL:
		pline_The("toilet seems to be beamed aboard somewhere.");
		break;
	case AMULET_OF_PRISM:
		pline_The("toilet sparkles in rainbow colors!");
		break;
	case AMULET_OF_WARP_DIMENSION:
		pline_The("toilet suddenly looks like something straight out of an acid trip!");
		break;
	case AMULET_OF_D_TYPE_EQUIPMENT:
		pline_The("toilet water looks like lava for a moment!");
		break;
	case AMULET_VERSUS_DEATH_SPELL:
		pline_The("toilet no longer fears death.");
		break;
	case AMULET_OF_MAGIC_CONTROL:
		pline_The("button seems very easy to control.");
		break;
	case AMULET_OF_QUICK_ATTACK:
		pline_The("toilet flushes twice.");
		break;
	case AMULET_OF_QUADRUPLE_ATTACK:
		pline_The("toilet flushes four times in quick succession.");
		break;
	case AMULET_OF_NECK_BRACE:
		pline("A sword tries to smash the upper part of the toilet, but somehow misses wildly!");
		break;
	case AMULET_OF_CLIMBING:
		pline_The("toilet seems to be moving slowly but steadily towards the ceiling!");
		break;
	case AMULET_OF_TECHNICALITY:
		pline_The("toilet momentarily looks like a space station.");
		break;
	case PENDANT:
	case NECKLACE:
		pline_The("toilet seems uselessly adorned.");
		break;
	case AMULET_VERSUS_CURSES:
		pline("A malignant aura surrounds the toilet for a moment, but is repelled by a magical shield.");
		break;
	case AMULET_OF_RMB_LOSS:
		pline_The("toilet no longer seems there... until you feel for it. Why can't you see it?");
		break;
	case AMULET_OF_ITEM_TELEPORTATION:
		pline_The("dust particles and dirt inside the toilet suddenly change their positions!");
		break;
	case AMULET_OF_EXPLOSION:
		pline_The("toilet seems to be engulfed in a massive explosion!");
		break;
	case AMULET_OF_WRONG_SEEING:
		You("suddenly see a fat log of shit inside the toilet!");
		break;
	case AMULET_OF_WEAK_MAGIC:
		pline_The("water flow is suddenly too weak to flush away anything.");
		break;
	case AMULET_OF_DIRECTIONAL_SWAP:
		pline_The("water flows up instead of down!");
		break;
	case AMULET_OF_SUDDEN_CURSE:
		pline_The("toilet is surrounded by a malignant aura.");
		break;
	case AMULET_OF_ANTI_EXPERIENCE:
		pline_The("toilet seems to be impossible to improve.");
		break;
	case AMULET_OF_STONE:
		pline_The("toilet seems to be made of stone.");
		break;
	case AMULET_OF_MAP_AMNESIA:
		You("don't remember whether there was a toilet at all.");
		if (FunnyHallu) You("also forgot that you have to take a crap, and shit your trousers by mistake.");
		break;
	case AMULET_OF_DEPRESSION:
		You("feel like you lost an important part of yourself.");
		break;
	case AMULET_OF_SCREWY_INTERFACE:
		pline_The("amulet seems to hang in mid-air, even though you heard it slide down the drain!");
		break;
	case AMULET_OF_BONES:
		pline_The("toilet looks like a gravestone for a moment.");
		break;
	case AMULET_OF_SPELL_FORGETTING:
		pline_The("toilet seems very non-magical.");
		break;
	case AMULET_OF_ITEM_INSECURITY:
		pline_The("toilet looks like a castle for a moment.");
		break;
	case AMULET_OF_STOPPED_EVOLUTION:
		pline_The("toilet seems to stop generating water");
		break;
	case AMULET_OF_INITIALIZATION_FAILU:
		pline_The("toilet flusing button momentarily vanishes");
		break;
	case AMULET_OF_REAL_SCREWERY:
		pline_The("toilet suddenly seems shitty!");
		break;
	case AMULET_OF_ENEMY_SAVING:
		pline_The("toilet water's amulet seems to glow...");
		break;
	case AMULET_OF_INCREASED_FREQUENCY:
		pline_The("toilet flushing sound can be heard several times at once!");
		break;
	case AMULET_OF_SPELL_METAL:
		pline_The("porcelain toilet looks metallic for a moment.");
		break;
	case AMULET_OF_TECHOUT:
		pline_The("toilet looks primitive.");
		break;
	case AMULET_OF_BAD_PART:
		pline("Suddenly all sorts of random garbage appear in the toilet!");
		break;
	case AMULET_OF_EVIL_VARIANT:
		You("see a chunk of silver get corroded by the toilet water!"); /* evilvariant makes silver corrodeable */
		break;

	case AMULET_OF_HOSTILITY:
		pline_The("toilet suddenly threatens to attack you!");
		break;
	case AMULET_OF_SANITY_TREBLE:
		pline_The("toilet seems to be full of dirt and shit!");
		break;
	case AMULET_OF_EVIL_CRAFTING:
		You("suddenly seem to see %s taking a crap!", rndplrmonnamefemale());
		break;
	case AMULET_OF_EDIBILITY:
		pline_The("toilet looks delicious! You wonder whether you can eat it.");
		if (FunnyHallu) pline("In fact, it seems to have turned into an edible bra! Mmmmmmmmmmmmm... candy!");
		break;
	case AMULET_OF_WAKING:
		pline("Suddenly, a very loud flushing sound seems to jolt you back to your senses.");
		break;
	case AMULET_OF_TRASH:
		pline("There's a ton of useless garbage in the toilet. Better flush it quickly.");
		break;
	case AMULET_OF_UNDRESSING:
		You("are suddenly overcome with an urge to pull down your pants.");
		break;
	case AMULET_OF_STARLIGHT:
		pline("A searing ray of light shines brightly at the toilet!");
		break;
	case AMULET_OF_VULNERABILITY:
		pline_The("very air seems to erode the toilet.");
		break;
	default:
		pline("Apparently, nothing happens.");
		break;
	}

fridaydone2:

	trycall(obj);

	if (!obj->oerodeproof && is_rustprone(obj) && !hard_to_destruct(obj) && (!obj->oartifact || !rn2(4)) && obj->oeroded == MAX_ERODE) {
		pline("%s rusted away completely!", doname(obj));
		useup(obj);
		return;
	} else if (getitback) {
		pline_The("toilet flushes, and %s reappears!", doname(obj));
		obj->in_use = FALSE;
		if (!rn2(3)) curse(obj);
		if (!rn2(3)) (void) get_wet(obj, FALSE);

		dropx(obj);
	} else {
		useup(obj);
		int aggroamount = rnd(6);
		if (isfriday) aggroamount *= 2;
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
		while (aggroamount) {
			makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY);
			aggroamount--;
			if (aggroamount < 0) aggroamount = 0;
		}
		u.aggravation = 0;
		pline("Something comes out of the toilet!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	}

}


#endif /* OVLB */
#ifdef OVL0

/* some common tests when trying to drop or throw items */
boolean
canletgo(obj,word)
register struct obj *obj;
register const char *word;
{
	if(obj->owornmask & (W_ARMOR | W_RING | W_AMUL | W_IMPLANT | W_TOOL)){
		if (*word)
			Norep("You cannot %s %s you are wearing.",word,
				something);
		return(FALSE);
	}
	/* KMH, balance patch -- removed stone of rotting */        
	if ( (obj->otyp == LOADSTONE || obj->otyp == MANASTONE || obj->otyp == SLEEPSTONE || obj->otyp == LOADBOULDER || obj->otyp == STARLIGHTSTONE || obj->otyp == STONE_OF_MAGIC_RESISTANCE || obj->otyp == LUCKSTONE || obj->otyp == HEALTHSTONE || is_nastygraystone(obj)) && obj->cursed) {
		/* getobj() kludge sets corpsenm to user's specified count
		   when refusing to split a stack of cursed loadstones */
		if (*word) {
			/* getobj() ignores a count for throwing since that is
			   implicitly forced to be 1; replicate its kludge... */
			if (!strcmp(word, "throw") && obj->quan > 1L)
			    obj->corpsenm = 1;
			pline("For some reason, you cannot %s%s the stone%s!",
			      word, obj->corpsenm ? " any of" : "",
			      plur(obj->quan));
			if (FunnyHallu) pline("Your fault for picking it up, you damn idiot!"); /* YANI by Yasdorian */
		}
		obj->corpsenm = 0;		/* reset */
		obj->bknown = 1;
		return(FALSE);
	}
	if (obj->otyp == LEATHER_LEASH && obj->leashmon != 0) {
		if (*word)
			pline_The("leash is tied around your %s.",
					body_part(HAND));
		return(FALSE);
	}
	if (obj->otyp == INKA_LEASH && obj->leashmon != 0) {
		if (*word)
			pline_The("leash is tied around your %s.",
					body_part(HAND));
		return(FALSE);
	}
	if (obj->otyp == AMULET_OF_YENDOR || obj->otyp == FAKE_AMULET_OF_YENDOR) {
		if (*word)
			pline("You can't %s such a powerful item.", word);
		return(FALSE);
	}
	if (obj->owornmask & W_SADDLE) {
		if (*word)
			You("cannot %s %s you are sitting on.", word,
				something);
		return (FALSE);
	}
	return(TRUE);
}

STATIC_PTR
int
drop(obj)
register struct obj *obj;
{
	if(!obj) return(0);
	if(!canletgo(obj,"drop"))
		return(0);
	if(obj == uwep) {
		if(welded(uwep)) {
			weldmsg(obj);
			return(0);
		}
		setuwep((struct obj *)0, FALSE, TRUE);
	}
	if (obj == uswapwep) {
		setuswapwep((struct obj *)0, FALSE);
	}
	if(obj == uquiver) {
		setuqwep((struct obj *)0);
	}

	if (u.uswallow) {
		/* barrier between you and the floor */
		if(flags.verbose)
		{
			char buf[BUFSZ];

			/* doname can call s_suffix, reusing its buffer */
			strcpy(buf, s_suffix(mon_nam(u.ustuck)));
			You("drop %s into %s %s.", doname(obj), buf,
				mbodypart(u.ustuck, STOMACH));
		}
	} else {
	    if((obj->oclass == RING_CLASS || obj->otyp == MEAT_RING) &&
			IS_SINK(levl[u.ux][u.uy].typ)) {
		dosinkring(obj);
		if (issoviet && !rn2(10)) pline("Eto zanimayet ochered' potomu, chto sovetskiy khochet, chtoby igra byla der'mo.");
		return(issoviet ? 1 : 0);
	    }
	    if((obj->oclass == AMULET_CLASS) && IS_TOILET(levl[u.ux][u.uy].typ)) {
		dotoiletamulet(obj);
		if (issoviet && !rn2(10)) pline("Eto zanimayet ochered' potomu, chto sovetskiy khochet, chtoby igra byla der'mo.");
		return(issoviet ? 1 : 0);
	    }
	    if (!can_reach_floor()) {
		if (u.uprops[DROPCURSES_EFFECT].extrinsic || Dropcurses || have_dropcursestone() || (uleft && uleft->oartifact == ART_ARABELLA_S_RADAR) || (uright && uright->oartifact == ART_ARABELLA_S_RADAR) ) {
			curse(obj);
		}

		if(flags.verbose) You("drop %s.", doname(obj));
#ifndef GOLDOBJ
		if (obj->oclass != COIN_CLASS || obj == invent) freeinv(obj);
#else
		/* Ensure update when we drop gold objects */
		if (obj->oclass == COIN_CLASS) flags.botl = 1;

		freeinv(obj);
#endif
		hitfloor(obj);
		if (issoviet && !rn2(10)) pline("Eto zanimayet ochered' potomu, chto sovetskiy khochet, chtoby igra byla der'mo.");
		return(issoviet ? 1 : 0);
	    }
	    if (!IS_ALTAR(levl[u.ux][u.uy].typ) && flags.verbose)
		You("drop %s.", doname(obj));
	}

	if (u.uprops[DROPCURSES_EFFECT].extrinsic || Dropcurses || have_dropcursestone() || (uleft && uleft->oartifact == ART_ARABELLA_S_RADAR) || (uright && uright->oartifact == ART_ARABELLA_S_RADAR) ) {
		curse(obj);
	}

	dropx(obj);
	if (issoviet && !rn2(10)) pline("Eto zanimayet ochered' potomu, chto sovetskiy khochet, chtoby igra byla der'mo.");
	return(issoviet ? 1 : 0);
	/* Dropping things should no longer consume a turn now. --Amy */
	/* In Soviet Russia, picking up and dropping things requires time because everything's supposed to be shit. --Amy */

}

/* Called in several places - may produce output */
/* eg ship_object() and dropy() -> sellobj() both produce output */
void
dropx(obj)
register struct obj *obj;
{
#ifndef GOLDOBJ
	if (obj->oclass != COIN_CLASS || obj == invent) freeinv(obj);
#else
        /* Ensure update when we drop gold objects */
        if (obj->oclass == COIN_CLASS) flags.botl = 1;
        freeinv(obj);
#endif
	if (!u.uswallow) {
	    if (ship_object(obj, u.ux, u.uy, FALSE)) return;
	    if (IS_ALTAR(levl[u.ux][u.uy].typ))
		doaltarobj(obj); /* set bknown */
	}
	dropy(obj);
}

void
dropy(obj)
register struct obj *obj;
{
	if (obj == uwep) setuwep((struct obj *)0, FALSE, TRUE);
	if (obj == uquiver) setuqwep((struct obj *)0);
	if (obj == uswapwep) setuswapwep((struct obj *)0, FALSE);

	if ((!u.uswallow) && flooreffects(obj,u.ux,u.uy,"drop")) return;
	/* uswallow check done by GAN 01/29/87 */
	if(u.uswallow ) {
	    boolean could_petrify = FALSE;
	    boolean could_poly = FALSE;
	    boolean could_slime = FALSE;
	    boolean could_grow = FALSE;
	    boolean could_heal = FALSE;

	    if (obj != uball) {		/* mon doesn't pick up ball */
		if (obj->otyp == CORPSE) {
		    could_petrify = touch_petrifies(&mons[obj->corpsenm]);
		    could_poly = polyfodder(obj);
		    could_slime = slime_on_touch(&mons[obj->corpsenm]);
		    could_grow = (obj->corpsenm == PM_WRAITH);
		    could_heal = (obj->corpsenm == PM_NURSE);
		}
		if (obj->otyp == EGG) {
		    could_petrify = (touch_petrifies(&mons[obj->corpsenm]) && obj->corpsenm != PM_PLAYERMON);
		}
		(void) mpickobj(u.ustuck,obj,FALSE);
		if (is_animal(u.ustuck->data)) {
		    if (could_poly || could_slime) {
			(void) newcham(u.ustuck,
				       could_poly ? (struct permonst *)0 :
				       &mons[PM_GREEN_SLIME],
				       FALSE, could_slime);
			delobj(obj);	/* corpse is digested */
		    } else if (could_petrify && !rn2(4)) {
			minstapetrify(u.ustuck, TRUE);
			/* Don't leave a cockatrice corpse in a statue */
			if (!u.uswallow) delobj(obj);
		    } else if (could_grow) {
			(void) grow_up(u.ustuck, (struct monst *)0);
			delobj(obj);	/* corpse is digested */
		    } else if (could_heal) {
			u.ustuck->mhp = u.ustuck->mhpmax;
			delobj(obj);	/* corpse is digested */
		    }
		}
	    }
	} else  {
	    place_object(obj, u.ux, u.uy);
	    if (obj == uball)
		drop_ball(u.ux,u.uy);
	    else
		sellobj(obj, u.ux, u.uy);
	    stackobj(obj);
	    if(Blind && Levitation)
		map_object(obj, 0);
	    newsym(u.ux,u.uy);	/* remap location under self */
	}
}

/* things that must change when not held; recurse into containers.
   Called for both player and monsters */
void
obj_no_longer_held(obj)
struct obj *obj;
{
	if (!obj) {
	    return;
	} else if ((Is_container(obj) || obj->otyp == STATUE) && obj->cobj) {
	    struct obj *contents;
	    for(contents=obj->cobj; contents; contents=contents->nobj)
		obj_no_longer_held(contents);
	}
	switch(obj->otyp) {
	case CRYSKNIFE:
	    /* KMH -- Fixed crysknives have only 10% chance of reverting */
	    /* only changes when not held by player or monster */
	    if ((!obj->oerodeproof || !rn2(10)) /*&& !obj->oartifact*/ && !stack_too_big(obj)) {
		obj->otyp = WORM_TOOTH;
		obj->oerodeproof = 0;
	    }
	    break;
	}
}

/* 'D' command: drop several things */
int
doddrop()
{

	if (NoDropProblem || u.uprops[DROP_BUG].extrinsic || have_dropbugstone() || (uamul && uamul->oartifact == ART_ARABELLA_S_SWOONING_BEAUTY) ) {

		pline("For some reason, you cannot drop items!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;

	}

	int result = 0;

	add_valid_menu_class(0); /* clear any classes already there */
	if (*u.ushops) sellobj_state(SELL_DELIBERATE);
	if ((flags.menu_style != MENU_TRADITIONAL && !InventoryDoesNotGo) ||
		(result = ggetobj("drop", drop, 0, FALSE, (unsigned *)0)) < -1)
	    result = menu_drop(result);
	if (*u.ushops) sellobj_state(SELL_NORMAL);
	reset_occupations();

	return result;
}

/* Drop things from the hero's inventory, using a menu. */
STATIC_OVL int
menu_drop(retry)
int retry;
{
    int n, i, n_dropped = 0;
    long cnt;
    struct obj *otmp, *otmp2;
#ifndef GOLDOBJ
    struct obj *u_gold = 0;
#endif
    menu_item *pick_list;
    boolean all_categories = TRUE;
    boolean drop_everything = FALSE;

#ifndef GOLDOBJ
    if (u.ugold) {
	/* Hack: gold is not in the inventory, so make a gold object
	   and put it at the head of the inventory list. */
	u_gold = mkgoldobj(u.ugold);	/* removes from u.ugold */
	u_gold->in_use = TRUE;
	u.ugold = u_gold->quan;		/* put the gold back */
	assigninvlet(u_gold);		/* might end up as NOINVSYM */
	u_gold->nobj = invent;
	invent = u_gold;
    }
#endif
    if (retry) {
	all_categories = (retry == -2);
    } else if (flags.menu_style == MENU_FULL && !InventoryDoesNotGo) {
	all_categories = FALSE;
	n = query_category("Drop what type of items?",
			invent,
			UNPAID_TYPES | NOTFULLYIDED | ALL_TYPES | CHOOSE_ALL |
			BUC_BLESSED | BUC_CURSED | BUC_UNCURSED | BUC_UNKNOWN,
			&pick_list, PICK_ANY);
	if (!n) goto drop_done;
	for (i = 0; i < n; i++) {
	    if (pick_list[i].item.a_int == ALL_TYPES_SELECTED)
		all_categories = TRUE;
	    else if (pick_list[i].item.a_int == 'A')
		drop_everything = TRUE;
	    else
		add_valid_menu_class(pick_list[i].item.a_int);
	}
	free((void *) pick_list);
    } else if (flags.menu_style == MENU_COMBINATION && !InventoryDoesNotGo) {
	unsigned ggoresults = 0;
	all_categories = FALSE;
	/* Gather valid classes via traditional NetHack method */
	i = ggetobj("drop", drop, 0, TRUE, &ggoresults);
	if (i == -2) all_categories = TRUE;
	if (ggoresults & ALL_FINISHED) {
		n_dropped = i;
		goto drop_done;
	}
    }

    if (drop_everything) {
	for(otmp = invent; otmp; otmp = otmp2) {
	    otmp2 = otmp->nobj;
	    n_dropped += drop(otmp);
	}
    } else if (!(InitializationFail || u.uprops[INITIALIZATION_FAIL].extrinsic || have_initializationstone())) {
	/* should coordinate with perm invent, maybe not show worn items */
	n = query_objlist("What would you like to drop?", invent,
			USE_INVLET|INVORDER_SORT, &pick_list,
			PICK_ANY, all_categories ? allow_all : allow_category);
	if (n > 0) {
	    for (i = 0; i < n; i++) {
		otmp = pick_list[i].item.a_obj;
		cnt = pick_list[i].count;
		if (cnt < otmp->quan) {
		    if (welded(otmp)) {
			;	/* don't split */
		    } else if ( (otmp->otyp == LOADSTONE || otmp->otyp == LUCKSTONE || otmp->otyp == HEALTHSTONE || otmp->otyp == MANASTONE || otmp->otyp == SLEEPSTONE || otmp->otyp == LOADBOULDER || otmp->otyp == STARLIGHTSTONE || otmp->otyp == STONE_OF_MAGIC_RESISTANCE || is_nastygraystone(otmp) ) && otmp->cursed) {
			/* same kludge as getobj(), for canletgo()'s use */
			otmp->corpsenm = (int) cnt;	/* don't split */
		    } else {
#ifndef GOLDOBJ
			if (otmp->oclass == COIN_CLASS)
			    (void) splitobj(otmp, otmp->quan - cnt);
			else
#endif
			    otmp = splitobj(otmp, cnt);
		    }
		}
		n_dropped += drop(otmp);
	    }
	    free((void *) pick_list);
	}
    }

 drop_done:
#ifndef GOLDOBJ
    if (u_gold && invent && invent->oclass == COIN_CLASS) {
	/* didn't drop [all of] it */
	u_gold = invent;
	invent = u_gold->nobj;
	u_gold->in_use = FALSE;
	dealloc_obj(u_gold);
	update_inventory();
    }
#endif
    return n_dropped;
}

#endif /* OVL0 */
#ifdef OVL2

/* on a ladder, used in goto_level */
static NEARDATA boolean at_ladder = FALSE;

int
dodown()
{
	struct trap *trap = 0;
	boolean stairs_down = ((u.ux == xdnstair && u.uy == ydnstair) ||
		    (u.ux == sstairs.sx && u.uy == sstairs.sy && !sstairs.up)),
		ladder_down = (u.ux == xdnladder && u.uy == ydnladder);

	if (NoStaircase && u.uhave.amulet && !u.freeplaymode && (stairs_down || ladder_down) ) {

		pline(FunnyHallu ? "An anomalous energy field prevents you from taking the stairs!" : "The staircase is temporarily blocked! Try again later!");
		return(0);

	}

	if (!(powerfulimplants()) && uimplant && uimplant->oartifact == ART_JANA_S_MAKE_UP_PUTTY && (stairs_down || ladder_down) && !rn2(100)) {
		u.youaredead = 1;
		pline("NETHACK caused a Kernel error at address 0001:A9EE.");
		killer_format = KILLED_BY;
		killer = "Jana's make-up putty";
		done(DIED);
		u.youaredead = 0;
	}

	if (u.stairscumslowing && (stairs_down || ladder_down) ) {
		pline("This stair is currently blocked and will reopen in %d turn%s.", u.stairscumslowing, u.stairscumslowing > 1 ? "s" : "");
		return(0);
	}

	if (stairs_down && u.ux == sstairs.sx && u.uy == sstairs.sy && at_dgn_entrance("Gamma Caves") && !u.sewerplantcomplete) {
		pline("The Gamma Caves cannot be entered as long as you didn't make it to the bottom of the Sewer Plant yet.");
		return(0);
	}

	if (Role_if(PM_GNOME) && on_level(&mineend_level,&u.uz)) {
		pline("The staircase is filled with tons of rubble and debris.");
		pline("Poor Ruggo!");
		return (0);
	}

	if (u.usteed && !u.usteed->mcanmove) {
		pline("%s won't move!", Monnam(u.usteed));
		return(0);
	} else if (u.usteed && u.usteed->meating) {
		pline("%s is still eating.", Monnam(u.usteed));
		return(0);
	} else

	if (Levitation && !Race_if(PM_LEVITATOR) ) { /* levitators are hard enough already --Amy */
	    if ((HLevitation & I_SPECIAL) || (ELevitation & W_ARTI)) {
		/* end controlled levitation */
		if (ELevitation & W_ARTI) {
		    struct obj *obj;

		    for(obj = invent; obj; obj = obj->nobj) {
			if (obj->oartifact &&
					artifact_has_invprop(obj,LEVITATION)) {
			    if (obj->age < monstermoves)
				obj->age = monstermoves + rnz(100);
			    else
				obj->age += rnz(100);
			}
		    }
		}
		if (float_down(I_SPECIAL|TIMEOUT, W_ARTI))
		    return (1);   /* came down, so moved */
	    }
	    floating_above(stairs_down ? "stairs" : ladder_down ?
			   "ladder" : surface(u.ux, u.uy));

		if ( (stairs_down || ladder_down) && yn("You can force yourself down, but this will hurt and drain you. Do it?") == 'y') {
			pline("You drain your life-force to squeeze down the stairs.");
		    losexp("levitating down a narrow set of stairs", TRUE, FALSE);
		    losexp("levitating down a narrow set of stairs", TRUE, FALSE);
		}
		else {return(0);} /* didn't move */
	}
	if (!stairs_down && !ladder_down) {

		/* Amy edit: just because you can't see the trap doesn't mean it's not there; let the player use it! */

		if (!(trap = t_at(u.ux,u.uy)) ||
			(trap->ttyp != TRAPDOOR && trap->ttyp != SHAFT_TRAP && trap->ttyp != CURRENT_SHAFT && trap->ttyp != HOLE)
			|| !Can_fall_thru(&u.uz) ) {

			/* allow the > key to go down into a pit. But only if it really is one! --Amy */
			if ((trap = t_at(u.ux,u.uy)) && (trap->ttyp == PIT || trap->ttyp == SHIT_PIT || trap->ttyp == MANA_PIT || trap->ttyp == GIANT_CHASM || trap->ttyp == SPIKED_PIT || trap->ttyp == ANOXIC_PIT || trap->ttyp == ACID_PIT) && !u.utrap) {
				You("carefully ease yourself into the %spit.", (trap->ttyp == SPIKED_PIT) ? "spiked " : "");
                         /* if you're fumbling or clumsy, you slip */ 
				if ((Fumbling || rn2(ACURR(A_DEX) - 2) == 0) && !is_clinger(youmonst.data)) {     
					You("slip while trying to enter the %spit!", (trap->ttyp == SPIKED_PIT) ? "spiked " : "");
					dotrap(trap, FORCEBUNGLE);
					exercise(A_DEX, FALSE);
				} else {
					u.utrap = rn1(6,2); /* default pit time */
					u.utraptype = TT_PIT;
					vision_full_recalc = 1;
				}
				return 1;
			} else if (flags.autodig && !flags.nopick && uwep && is_pick(uwep)) {
				if (!touch_artifact(uwep, &youmonst)) return(0);
				return use_pick_axe2(uwep);
			} else {
				You_cant("go down here.");
				return(0);
			}
		}
	}
	if(u.ustuck) {
		You("are %s, and cannot go down.",
			!u.uswallow ? "being held" : is_animal(u.ustuck->data) ?
			"swallowed" : "engulfed");
		return(1);
	}
	if (on_level(&valley_level, &u.uz) && !u.uevent.gehennom_entered) {
		You("are standing at the gate to Gehennom.");
		pline("Unspeakable cruelty and harm lurk down there.");
		if (yn("Are you sure you want to enter?") != 'y')
			return(0);
		else pline("So be it.");
		u.uevent.gehennom_entered = 1;	/* don't ask again */

		/* Amy edit: For Junethack, simply being banished to Gehennom should not be enough for the trophy.
		 * Even though I disagree with most of Nethack Fourk's changes, the one where you need to enter Gehennom
		 * through the front entrance is actually good, since it's unlikely to be gotten by scummy behavior with
		 * low-level throwaway characters. So from now on the trophy reads "entered Gehennom via front entrance". */

#ifdef RECORD_ACHIEVE

		if (!achieve.enter_gehennom) {

			if (uarmc && itemhasappearance(uarmc, APP_TEAM_SPLAT_CLOAK)) pline("TROPHY GET!");
			if (RngeTeamSplat) pline("TROPHY GET!");
			if (Race_if(PM_INHERITOR)) giftartifact();
			if (Race_if(PM_HERALD)) heraldgift();

			if (uarmc && uarmc->oartifact == ART_JUNETHACK______WINNER) {
				u.uhpmax += 10;
				u.uenmax += 10;
				if (Upolyd) u.mhmax += 10;
				pline("Well done! Your maximum health and mana were increased to make sure you'll get even more trophies! Go for it!");
			}

		}

            achieve.enter_gehennom = 1;
#ifdef LIVELOGFILE
	livelog_achieve_update();
	livelog_report_trophy("entered Gehennom");
#endif
#endif

	}

	if(!next_to_u()) {
		You("are held back by your pet!");
		return(0);
	}

	if (trap)
	    You("%s %s.", locomotion(youmonst.data, "jump"),
		trap->ttyp == HOLE ? "down the hole" : "through the trap door");

	if (trap && Is_stronghold(&u.uz)) {
		goto_hell(FALSE, TRUE);
	} else {
		at_ladder = (boolean) (levl[u.ux][u.uy].typ == LADDER);
		next_level(!trap);
		at_ladder = FALSE;
	}
	return(1);
}

int
doup()
{
	if (u.freeplaymode && on_level(&u.uz, &astral_level) && IS_ALTAR(levl[u.ux][u.uy].typ)) {
		You("ascend back to the dungeon.");
		u.freeplaytransit = TRUE;
		if (u.uhave.amulet) { /* no longer need the amulet, now that you've won */
			struct obj *otmpi, *otmpii;
			if (invent) {
				for (otmpi = invent; otmpi; otmpi = otmpii) {
				      otmpii = otmpi->nobj;
					if (otmpi->otyp == AMULET_OF_YENDOR) {							
						if (otmpi->owornmask) {
							setnotworn(otmpi);
						}
						dropx(otmpi);
					}
				}
			}
		}
		goto_level(&medusa_level, TRUE, FALSE, FALSE);

		register int newlevX = 1;
		d_level newlevelX;
		get_level(&newlevelX, newlevX);
		goto_level(&newlevelX, TRUE, FALSE, FALSE);
		u.freeplaytransit = FALSE;
		return(0);
	}

	if( (u.ux != xupstair || u.uy != yupstair)
	     && (!xupladder || u.ux != xupladder || u.uy != yupladder)
	     && (!sstairs.sx || u.ux != sstairs.sx || u.uy != sstairs.sy
			|| !sstairs.up)
	  ) {
		You_cant("go up here.");
		return(0);
	}

	if (NoStaircase && (!u.uhave.amulet || u.freeplaymode)) {

		pline(FunnyHallu ? "An anomalous energy field prevents you from taking the stairs!" : "The staircase is temporarily blocked! Try again later!");
		return(0);

	}

	if (!(powerfulimplants()) && uimplant && uimplant->oartifact == ART_JANA_S_MAKE_UP_PUTTY && !rn2(100)) {
		u.youaredead = 1;
		pline("NETHACK caused a Kernel error at address 0001:A9EE.");
		killer_format = KILLED_BY;
		killer = "Jana's make-up putty";
		done(DIED);
		u.youaredead = 0;
	}

	if (u.stairscumslowing && (!u.uhave.amulet || u.freeplaymode)) {
		pline("This stair is currently blocked and will reopen in %d turn%s.", u.stairscumslowing, u.stairscumslowing > 1 ? "s" : "");
		return(0);
	}

	if ((iszapem && !(u.zapemescape)) && u.ux == sstairs.sx && u.uy == sstairs.sy && In_spacebase(&u.uz) && (dunlev(&u.uz) == 1) && !u.sewerplantcomplete) {
		pline("Since you've not finished the Sewer Plant yet, you cannot leave the Space Base.");
		return(0);
	}

	if (u.ux == sstairs.sx && u.uy == sstairs.sy && at_dgn_entrance("Mainframe") && !u.gammacavescomplete) {
		pline("The Mainframe cannot be entered as long as you didn't make it to the bottom of the Gamma Caves yet.");
		return(0);
	}

	if (u.usteed && !u.usteed->mcanmove) {
		pline("%s won't move!", Monnam(u.usteed));
		return(0);
	} else if (u.usteed && u.usteed->meating) {
		pline("%s is still eating.", Monnam(u.usteed));
		return(0);
	} else
	if(u.ustuck) {
		You("are %s, and cannot go up.",
			!u.uswallow ? "being held" : is_animal(u.ustuck->data) ?
			"swallowed" : "engulfed");
		return(1);
	}
	if(near_capacity() > SLT_ENCUMBER) {
		/* No levitation check; inv_weight() already allows for it */
		Your("load is too heavy to climb the %s.",
			levl[u.ux][u.uy].typ == STAIRS ? "stairs" : "ladder");
		return(1);
	}
	if(ledger_no(&u.uz) == 1) {

		if (u.freeplaymode && !u.freeplayplanes) {
			pline("Sorry. In order to re-visit the planes, you need to go to Moloch's Sanctum first.");
			return 0;
		}

		if (u.uhave.amulet && !u.amuletcompletelyimbued) {
			/* You were such a n00b and ignored all the messages telling you about the Yendorian Tower. */
			com_pager(197);
			return 0;
		}

		if (iflags.debug_fuzzer) return 0;

		if (!u.freeplaymode && yn("Beware, there will be no return! Still climb?") != 'y')
			return(0);
	}
	if(!next_to_u()) {
		You("are held back by your pet!");
		return(0);
	}
	at_ladder = (boolean) (levl[u.ux][u.uy].typ == LADDER);
	prev_level(TRUE);
	at_ladder = FALSE;

	/* Make it VERY CLEAR to the player that the amulet needs to be imbued. --Amy
	 * Otherwise, those poor saps will go ahead and climb back up 100 floors in the regular dungeon,
	 * only to discover that they cannot ascend, and then they will have to go all the way back with the amulet.
	 * Of course, if you're still stupid enough to actually do that despite the numerous messages, you will get
	 * a special scornful message calling you out on failing reading comprehension (see above) :P */
	if (u.uhave.amulet && !(In_yendorian(&u.uz)) && !(In_forging(&u.uz)) && !(In_ordered(&u.uz)) && !(In_deadground(&u.uz)) && !u.amuletcompletelyimbued) com_pager(196);

	return(1);
}

d_level save_dlevel = {0, 0};

/* check that we can write out the current level */
STATIC_OVL int
currentlevel_rewrite()
{
	register int fd;
	char whynot[BUFSZ];

	/* since level change might be a bit slow, flush any buffered screen
	 *  output (like "you fall through a trap door") */
	mark_synch();

	fd = create_levelfile(ledger_no(&u.uz), whynot);
	if (fd < 0) {
		/*
		 * This is not quite impossible: e.g., we may have
		 * exceeded our quota. If that is the case then we
		 * cannot leave this level, and cannot save either.
		 * Another possibility is that the directory was not
		 * writable.
		 */
		pline("%s", whynot);
		return -1;
	}

#ifdef MFLOPPY
	if (!savelev(fd, ledger_no(&u.uz), COUNT_SAVE)) {
		(void) close(fd);
		delete_levelfile(ledger_no(&u.uz));
		pline("Slash'EM is out of disk space for making levels!");
		You("can save, quit, or continue playing.");
		return -1;
	}
#endif
	return fd;
}

#ifdef INSURANCE
void
save_currentstate()
{
	int fd;

	if (u.hangupcheat < 2) u.hangupcheat = 0;

	if (flags.ins_chkpt) {
		/* write out just-attained level, with pets and everything */
		fd = currentlevel_rewrite();
		if(fd < 0) return;
		bufon(fd);
		savelev(fd,ledger_no(&u.uz), WRITE_SAVE);
		bclose(fd);
	}

	/* write out non-level state */
	savestateinlock();
}
#endif

/*
static boolean
badspot(x, y)
register xchar x, y;
{
	return((levl[x][y].typ != ROOM && levl[x][y].typ != AIR &&
			 levl[x][y].typ != CORR) || MON_AT(x, y));
}
*/

d_level new_dlevel = {0, 0};

void
goto_level(newlevel, at_stairs, falling, portal)
d_level *newlevel;
boolean at_stairs, falling, portal;
{
	int fd, l_idx;
	xchar new_ledger;
	boolean cant_go_back,
		up = (depth(newlevel) < depth(&u.uz)),
		newdungeon = (u.uz.dnum != newlevel->dnum),
		was_in_W_tower = In_W_tower(u.ux, u.uy, &u.uz),
		familiar = FALSE;
	boolean new = FALSE;	/* made a new level? */
	struct monst *mtmp;
	char whynot[BUFSZ];
	coord dd;
	int cx, cy;

	/* for special spawn routines... --Amy */
	int randsp;
	int randmnst;
	struct permonst *randmonstforspawn;
	int monstercolor;
	int randmnsx;
	int i;

	int angbandvar = 0;
	int angbandtraps = 0;
	int angbandx, angbandy, rtrap;

	if (dunlev(newlevel) > dunlevs_in_dungeon(newlevel))
		newlevel->dlevel = dunlevs_in_dungeon(newlevel);
	if (newdungeon && In_endgame(newlevel) && !u.freeplaymode) { /* 1st Endgame Level !!! */
		if (u.uhave.amulet)
		    assign_level(newlevel, &earth_level);
		else return;
	}

	if (!In_endgame(&u.uz) && In_endgame(newlevel) && u.freeplaymode && u.freeplayplanes) {
		assign_level(newlevel, &earth_level);
	}

	new_ledger = ledger_no(newlevel);
	if (new_ledger <= 0)
		done(ESCAPED);	/* in fact < 0 is impossible */

	assign_level(&new_dlevel, newlevel);

	/* If you have the amulet and are trying to get out of Gehennom, going
	 * up a set of stairs sometimes does some very strange things!
	 * Biased against law and towards chaos, but not nearly as strongly
	 * as it used to be (prior to 3.2.0).
	 * Odds:	    old				    new
	 *	"up"    L      N      C		"up"    L      N      C
	 *	 +1   75.0   75.0   75.0	 +1   75.0   75.0   75.0
	 *	  0    0.0   12.5   25.0	  0    6.25   8.33  12.5
	 *	 -1    8.33   4.17   0.0	 -1    6.25   8.33  12.5
	 *	 -2    8.33   4.17   0.0	 -2    6.25   8.33   0.0
	 *	 -3    8.33   4.17   0.0	 -3    6.25   0.0    0.0
	 * [Tom] I removed this... it's indescribably annoying.         
	 * comment by Amy: Yes, it definitely is. That's why I don't re-enable it...
	 * except in evilvariant mode, because that one is deliberately designed to screw you over :P */

	if ( ((Inhell && evilfriday && u.uhave.amulet && !u.freeplaymode) || (MysteriousForceActive || u.uprops[MYSTERIOUS_FORCE_EFFECT].extrinsic || have_forcestone())) && up && !newdungeon && !portal && (dunlev(&u.uz) < dunlevs_in_dungeon(&u.uz)-3)) {
		if (!rn2(4)) {
			int odds = 3 + (int)u.ualign.type;          /* 2..4 */

			/* Let's be ULTRA EVIL(TM) and sometimes push the player almost all the way back down!!! */
			if (evilfriday && !rn2(20) && dunlevs_in_dungeon(&u.uz) > (dunlev(&u.uz) + 3) ) {
				odds = (dunlevs_in_dungeon(&u.uz) - (dunlev(&u.uz) + 3) );
				if (u.ualign.type == A_CHAOTIC) odds /= 2;
				else if (u.ualign.type == A_NEUTRAL) {
					odds *= 2;
					odds /= 3;
				}
				if (odds < (3 + (int)u.ualign.type) ) odds = (3 + (int)u.ualign.type);
			}

			int diff = odds <= 1 ? 0 : rn2(odds);       /* paranoia */

			if (diff != 0) {
			assign_rnd_level(newlevel, &u.uz, diff);
			/* if inside the tower, stay inside */
			if (was_in_W_tower &&
			    !On_W_tower_level(newlevel)) diff = 0;
		}
		if (diff == 0)
			assign_level(newlevel, &u.uz);

		new_ledger = ledger_no(newlevel);

		pline("A dirty mysterious dirt force full of dirt momentarily surrounds you...");
		u.cnd_mysteriousforcecount++;
		if (on_level(newlevel, &u.uz)) {
			(void) safe_teleds(FALSE);
			(void) next_to_u();
			return;
		} else
			at_stairs = at_ladder = FALSE;
		}
	}

	/* Prevent the player from going past the first quest level unless
	 * (s)he has been given the go-ahead by the leader.
	 */
	/*if (on_level(&u.uz, &qstart_level) && !newdungeon && !ok_to_quest()) {
	*	pline("A mysterious force prevents you from descending.");
	*	return;
	}*/
	/* commented out, hopefully preventing you from making the game unwinnable by mistake --Amy */

	if (on_level(newlevel, &u.uz)) return;		/* this can happen */

	fd = currentlevel_rewrite();
	if (fd < 0) return;

	if (falling) /* assuming this is only trap door or hole */
	    impact_drop((struct obj *)0, u.ux, u.uy, newlevel->dlevel);

	check_special_room(TRUE);		/* probably was a trap door */
	if (Punished) unplacebc();
	u.utrap = 0;				/* needed in level_tele */
	fill_pit(u.ux, u.uy);
	setustuck(0);				/* idem */
	u.uinwater = 0;
	u.uundetected = 0;	/* not hidden, even if means are available */
	keepdogs(FALSE);
	if (u.uswallow)				/* idem */
		u.uswldtim = u.uswallow = 0;
	/*
	 *  We no longer see anything on the level.  Make sure that this
	 *  follows u.uswallow set to null since uswallow overrides all
	 *  normal vision.
	 */
	vision_recalc(2);

	/*
	 * Save the level we're leaving.  If we're entering the endgame,
	 * we can get rid of all existing levels because they cannot be
	 * reached any more.  We still need to use savelev()'s cleanup
	 * for the level being left, to recover dynamic memory in use and
	 * to avoid dangling timers and light sources.
	 */
	cant_go_back = /*(newdungeon && In_endgame(newlevel))*/FALSE;
/*	if (!cant_go_back) {*/
	    update_mlstmv();	/* current monsters are becoming inactive */
	    bufon(fd);		/* use buffered output */
/*	}*/
	savelev(fd, ledger_no(&u.uz),
		cant_go_back ? FREE_SAVE : (WRITE_SAVE | FREE_SAVE));
	bclose(fd);
	if (/*cant_go_back*/0) { /* Amy edit: freeplay mode means we need to keep the levels around */
	    /* discard unreachable levels; keep #0 */
	    for (l_idx = maxledgerno(); l_idx > 0; --l_idx)
		delete_levelfile(l_idx);
	}

#ifdef REINCARNATION
	if (Is_rogue_level(newlevel) || Is_rogue_level(&u.uz) || isrougelike)
		assign_rogue_graphics(isrougelike || Is_rogue_level(newlevel));
#endif
#ifdef USE_TILES
	substitute_tiles(newlevel);
#endif
	assign_level(&u.uz0, &u.uz);
	assign_level(&u.uz, newlevel);
	assign_level(&u.utolev, newlevel);
	u.utotype = 0;
	if (dunlev_reached(&u.uz) < dunlev(&u.uz))
		dunlev_reached(&u.uz) = dunlev(&u.uz);
	reset_rndmonst(NON_PM);   /* u.uz change affects monster generation */

	/* set default level change destination areas */
	/* the special level code may override these */
	(void) memset((void *) &updest, 0, sizeof updest);
	(void) memset((void *) &dndest, 0, sizeof dndest);

	if (!(level_info[new_ledger].flags & LFILE_EXISTS)) {
		/* entering this level for first time; make it now */
		if (level_info[new_ledger].flags & (FORGOTTEN|VISITED)) {
		    impossible("goto_level: returning to discarded level?");
		    level_info[new_ledger].flags &= ~(FORGOTTEN|VISITED);
		}
		mklev();
		new = TRUE;	/* made the level */

		if (Race_if(PM_ELONA_SNAIL)) { /* cleaner bastard */
			s_level *sptr;
			if ( (sptr = Is_special(&u.uz)) != 0 && sptr->flags.town) {
				(void) makemon(&mons[PM_CLEANER], 0, 0, MM_ANGRY);
				if (!rn2(3)) { /* some towns, like Palmia or Arcbelc, have two cleaners */
					(void) makemon(&mons[PM_CLEANER], 0, 0, MM_ANGRY);
					if (!rn2(5)) { /* Port Kapul even has three of the bastards! */
						(void) makemon(&mons[PM_CLEANER], 0, 0, MM_ANGRY);
					}
				}
			}
		}

		if (In_illusorycastle(&u.uz) && (dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz)) ) { /* glass golem */
			(void) makemon(&mons[PM_MOTHERFUCKER_GLASS_GOLEM], 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
		}

		if (In_angmar(&u.uz) && (dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz)) ) { /* witch king */
			(void) makemon(&mons[PM_THE_WITCH_KING_OF_ANGMAR], 0, 0, NO_MM_FLAGS);
		}

		if (In_swimmingpool(&u.uz) && (dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz)) ) { /* jewelry and stuff */
			pline("Your %s tingles and you smell the presence of treasure hidden inside the water tunnels.", body_part(NOSE));

#ifdef RECORD_ACHIEVE

			if (!achieveX.swimmingpool_cleared) {

				achieveX.swimmingpool_cleared = TRUE;
				if (uarmc && itemhasappearance(uarmc, APP_TEAM_SPLAT_CLOAK)) pline("TROPHY GET!");
				if (RngeTeamSplat) pline("TROPHY GET!");
				if (Race_if(PM_INHERITOR)) giftartifact();
				if (Race_if(PM_HERALD)) heraldgift();

				if (uarmc && uarmc->oartifact == ART_JUNETHACK______WINNER) {
					u.uhpmax += 10;
					u.uenmax += 10;
					if (Upolyd) u.mhmax += 10;
					pline("Well done! Your maximum health and mana were increased to make sure you'll get even more trophies! Go for it!");
				}
			}

#ifdef LIVELOGFILE
			livelog_achieve_update();
			livelog_report_trophy("reached the bottom of the Swimming Pools");
#endif
#endif

		}

		if (In_mainframe(&u.uz) && (dunlev(&u.uz) == 1) ) {
			(void) makemon(&mons[PM_BOFH], 0, 0, NO_MM_FLAGS);
		}

		if (In_hellbathroom(&u.uz) && (dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz)) ) {
			(void) makemon(&mons[PM_EROGENOUS_KATIA], 0, 0, NO_MM_FLAGS);
		}

		if (u.gottenbones) { /* evil patch idea by jonadab - spawn monsters if a bones file loads */

			u.cnd_bonescount++;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			while (rn2(10 + (monster_difficulty() / 3)) && (enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) ) {
				(void) makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS);
				if (wizard) pline("made bones monster");
				}

			u.gottenbones = 0;

			u.aggravation = 0;

		/* Actually, this is supposed to spawn more monsters the longer the deceased player has been dead already,
		   but coding that to properly work would be hell... --Amy */

		}

		/* some help for beginning characters because not having a container is just sucky --Amy */
		if (depth(&u.uz) >= 1 && depth(&u.uz) <= 5 && !issoviet && !rn2(5)) {
			angbandx = rn1(COLNO-3,2);
			angbandy = rn2(ROWNO);
			(void) mksobj_at(CHEST, angbandx, angbandy, TRUE, TRUE, FALSE);
		}

		/* bossrusher race: spawn a boss at a random location whenever you enter a new level --Amy */
		if (isbossrusher) {
			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			{
				int attempts = 0;
				register struct permonst *ptrZ;
newboss:
				do {

					ptrZ = rndmonst();
					attempts++;
					if (!rn2(2000)) reset_rndmonst(NON_PM);

				} while ( (!ptrZ || (ptrZ && !(ptrZ->geno & G_UNIQ))) && attempts < 50000);

				if (ptrZ && ptrZ->geno & G_UNIQ) {
					(void) makemon(ptrZ, 0, 0, MM_ANGRY);
				}
				else if (rn2(50)) {
					attempts = 0;
					goto newboss;
				}

			}

			u.aggravation = 0;

		}

		if (uarmc && uarmc->oartifact == ART_T_O_M_E) {

		      cx = rn2(COLNO);
		      cy = rn2(ROWNO);
			register int cnt = rnd(10);

			if (rn2(2)) {

				register int randmonstforFF = rnd(68);
				if (randmonstforFF == 35) randmonstforFF = 53;

				if (Aggravate_monster) {
					u.aggravation = 1;
					reset_rndmonst(NON_PM);
				}

				while(cnt--)
				    (void) makemon(mkclass(randmonstforFF,0), u.ux, u.uy, NO_MM_FLAGS);
				pline("A voice echoes:");
				verbalize("Oh, please help me! A horrible %s stole my sword! I'm nothing without it.", monexplain[randmonstforFF]);

				u.aggravation = 0;

			} else {

				if (Aggravate_monster) {
					u.aggravation = 1;
					reset_rndmonst(NON_PM);
				}

				randmonstforspawn = rndmonst();

				while(cnt--)
				    (void) makemon(randmonstforspawn, u.ux, u.uy, NO_MM_FLAGS);
				pline("A voice echoes:");
				verbalize("Leave me alone, stupid %s", randmonstforspawn->mname);

				u.aggravation = 0;

			}

		}

		if (Role_if(PM_ELEMENTALIST) && In_quest(&u.uz)) {
			register int tryct;
			register int booktype;
newelembook:
			tryct = 0;
			booktype = 0;
			while ( ((objects[booktype].oc_skill && objects[booktype].oc_skill != P_ELEMENTAL_SPELL) || !objects[booktype].oc_skill) && tryct < 10000) {
 				booktype = SPE_FORCE_BOLT + rn2(SPE_PSYBEAM - SPE_FORCE_BOLT + 1);
				tryct++;
			}

			if (objects[booktype].oc_skill && objects[booktype].oc_skill == P_ELEMENTAL_SPELL) {

rerollelemloc:
				angbandx = rn1(COLNO-3,2);
				angbandy = rn2(ROWNO);

				if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ == ROOM || levl[angbandx][angbandy].typ == CORR) && !(t_at(angbandx, angbandy)) ) {

					(void) mksobj_at(booktype, angbandx, angbandy, TRUE, TRUE, FALSE);
				} else if (rn2(1000)) goto rerollelemloc;

			}
			if (rn2(5)) goto newelembook;

		}

		if (Role_if(PM_OCCULT_MASTER) && In_quest(&u.uz)) {
			register int tryct;
			register int booktype;
newoccbook:
			tryct = 0;
			booktype = 0;
			while ( ((objects[booktype].oc_skill && objects[booktype].oc_skill != P_OCCULT_SPELL) || !objects[booktype].oc_skill) && tryct < 10000) {
 				booktype = SPE_FORCE_BOLT + rn2(SPE_PSYBEAM - SPE_FORCE_BOLT + 1);
				tryct++;
			}
			if (objects[booktype].oc_skill && objects[booktype].oc_skill == P_OCCULT_SPELL) {

rerolloccloc:
				angbandx = rn1(COLNO-3,2);
				angbandy = rn2(ROWNO);

				if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ == ROOM || levl[angbandx][angbandy].typ == CORR) && !(t_at(angbandx, angbandy)) ) {

					(void) mksobj_at(booktype, angbandx, angbandy, TRUE, TRUE, FALSE);
				} else if (rn2(1000)) goto rerolloccloc;

			}
			if (rn2(5)) goto newoccbook;

		}

		if (Role_if(PM_CHAOS_SORCEROR) && In_quest(&u.uz)) {
			register int tryct;
			register int booktype;
newchabook:
			tryct = 0;
			booktype = 0;
			while ( ((objects[booktype].oc_skill && objects[booktype].oc_skill != P_CHAOS_SPELL) || !objects[booktype].oc_skill) && tryct < 10000) {
 				booktype = SPE_FORCE_BOLT + rn2(SPE_PSYBEAM - SPE_FORCE_BOLT + 1);
				tryct++;
			}
			if (objects[booktype].oc_skill && objects[booktype].oc_skill == P_CHAOS_SPELL) {

rerollchaloc:
				angbandx = rn1(COLNO-3,2);
				angbandy = rn2(ROWNO);

				if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ == ROOM || levl[angbandx][angbandy].typ == CORR) && !(t_at(angbandx, angbandy)) ) {

					(void) mksobj_at(booktype, angbandx, angbandy, TRUE, TRUE, FALSE);
				} else if (rn2(1000)) goto rerollchaloc;

			}
			if (rn2(5)) goto newchabook;

		}

		if (isangbander || RngeAngband || (uarmc && itemhasappearance(uarmc, APP_ANGBAND_CLOAK)) ) { /* level feelings --Amy */

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			if (!rn2( (Luck*5) + 200)) {
				pline("This place seems like murder!");

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_GREMLIN,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_NYMPH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_VORTEX,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(3)) (void) makemon(mkclass(S_LIGHT,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(3)) (void) makemon(mkclass(S_LIGHT,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_ANGEL,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(3)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(4)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(5)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_ELEMENTAL,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_LICH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_LICH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_UMBER,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_UMBER,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_VAMPIRE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(3)) (void) makemon(mkclass(S_VAMPIRE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_DEMON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_DEMON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(10)) (void) makemon(mkclass(S_NEMESE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(50)) (void) makemon(mkclass(S_ARCHFIEND,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_GRUE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_RUBMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_GREMLIN,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_NYMPH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_VORTEX,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(3)) (void) makemon(mkclass(S_LIGHT,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(3)) (void) makemon(mkclass(S_LIGHT,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_ANGEL,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(3)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(4)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(5)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_ELEMENTAL,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_LICH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_LICH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_UMBER,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_UMBER,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_VAMPIRE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(3)) (void) makemon(mkclass(S_VAMPIRE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_DEMON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_DEMON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(10)) (void) makemon(mkclass(S_NEMESE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(50)) (void) makemon(mkclass(S_ARCHFIEND,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_GRUE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_RUBMONST,0), 0, 0, NO_MM_FLAGS);

				for (angbandtraps = 0; angbandtraps < 1000; angbandtraps++) {

					angbandx = rn1(COLNO-3,2);
					angbandy = rn2(ROWNO);

					if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ > DBWALL) && !(t_at(angbandx, angbandy)) ) {

					rtrap = randomtrap();

					(void) maketrap(angbandx, angbandy, rtrap, 100, TRUE);
					}
				}

			}

			else if (!rn2(Luck + 16)) {
				pline("You have a terrible feeling about this level.");

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_GREMLIN,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_NYMPH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_VORTEX,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(3)) (void) makemon(mkclass(S_LIGHT,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(3)) (void) makemon(mkclass(S_LIGHT,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_ANGEL,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(3)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(4)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(5)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_ELEMENTAL,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_LICH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_LICH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_UMBER,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_UMBER,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_VAMPIRE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(3)) (void) makemon(mkclass(S_VAMPIRE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_DEMON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_DEMON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(10)) (void) makemon(mkclass(S_NEMESE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(50)) (void) makemon(mkclass(S_ARCHFIEND,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_GRUE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && rn2(2)) (void) makemon(mkclass(S_RUBMONST,0), 0, 0, NO_MM_FLAGS);

				for (angbandtraps = 0; angbandtraps < 500; angbandtraps++) {

					angbandx = rn1(COLNO-3,2);
					angbandy = rn2(ROWNO);

					if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ > DBWALL) && !(t_at(angbandx, angbandy)) ) {

					rtrap = randomtrap();

					(void) maketrap(angbandx, angbandy, rtrap, 100, TRUE);
					}
				}

			}

			else if (!rn2(Luck + 15)) {
				pline("You have a bad feeling about this level.");

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_GREMLIN,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_NYMPH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_VORTEX,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(2)) (void) makemon(mkclass(S_LIGHT,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(2)) (void) makemon(mkclass(S_LIGHT,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(4)) (void) makemon(mkclass(S_ANGEL,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(2)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(4)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(5)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_ELEMENTAL,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(5)) (void) makemon(mkclass(S_LICH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(5)) (void) makemon(mkclass(S_LICH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_UMBER,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_UMBER,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(4)) (void) makemon(mkclass(S_VAMPIRE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(4)) (void) makemon(mkclass(S_VAMPIRE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(2)) (void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(2)) (void) makemon(mkclass(S_DEMON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_DEMON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(100)) (void) makemon(mkclass(S_NEMESE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(2)) (void) makemon(mkclass(S_GRUE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(2)) (void) makemon(mkclass(S_RUBMONST,0), 0, 0, NO_MM_FLAGS);

				for (angbandtraps = 0; angbandtraps < 100; angbandtraps++) {

					angbandx = rn1(COLNO-3,2);
					angbandy = rn2(ROWNO);

					if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ > DBWALL) && !(t_at(angbandx, angbandy)) ) {

					rtrap = randomtrap();

					(void) maketrap(angbandx, angbandy, rtrap, 100, TRUE);
					}
				}

			}

			else if (rnd(Luck + 15) < 4) {
				pline("You have an uneasy feeling about this level.");

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(4)) (void) makemon(mkclass(S_GREMLIN,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(4)) (void) makemon(mkclass(S_NYMPH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(4)) (void) makemon(mkclass(S_VORTEX,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_LIGHT,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_LIGHT,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(6)) (void) makemon(mkclass(S_ANGEL,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(4)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(5)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(6)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(4)) (void) makemon(mkclass(S_ELEMENTAL,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(7)) (void) makemon(mkclass(S_LICH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(7)) (void) makemon(mkclass(S_LICH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(4)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(4)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(4)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(4)) (void) makemon(mkclass(S_UMBER,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(4)) (void) makemon(mkclass(S_UMBER,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(5)) (void) makemon(mkclass(S_VAMPIRE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(5)) (void) makemon(mkclass(S_VAMPIRE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_DEMON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(4)) (void) makemon(mkclass(S_DEMON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_GRUE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(3)) (void) makemon(mkclass(S_RUBMONST,0), 0, 0, NO_MM_FLAGS);

				for (angbandtraps = 0; angbandtraps < 50; angbandtraps++) {

					angbandx = rn1(COLNO-3,2);
					angbandy = rn2(ROWNO);

					if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ > DBWALL) && !(t_at(angbandx, angbandy)) ) {

					rtrap = randomtrap();

					(void) maketrap(angbandx, angbandy, rtrap, 100, TRUE);
					}
				}

			}

			else if (rnd(Luck + 15) < 5) {
				pline("You have a faint uneasy feeling about this level.");

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(6)) (void) makemon(mkclass(S_GREMLIN,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(6)) (void) makemon(mkclass(S_NYMPH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(6)) (void) makemon(mkclass(S_VORTEX,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(5)) (void) makemon(mkclass(S_LIGHT,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(5)) (void) makemon(mkclass(S_LIGHT,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(12)) (void) makemon(mkclass(S_ANGEL,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(5)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(7)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(9)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(12)) (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(6)) (void) makemon(mkclass(S_ELEMENTAL,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(15)) (void) makemon(mkclass(S_LICH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(20)) (void) makemon(mkclass(S_LICH,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(7)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(9)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(11)) (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(7)) (void) makemon(mkclass(S_UMBER,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(9)) (void) makemon(mkclass(S_UMBER,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(8)) (void) makemon(mkclass(S_VAMPIRE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(10)) (void) makemon(mkclass(S_VAMPIRE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(5)) (void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(5)) (void) makemon(mkclass(S_DEMON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(8)) (void) makemon(mkclass(S_DEMON,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(5)) (void) makemon(mkclass(S_GRUE,0), 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) angbandvar++;

		 		if (!angbandvar && !rn2(5)) (void) makemon(mkclass(S_RUBMONST,0), 0, 0, NO_MM_FLAGS);

				for (angbandtraps = 0; angbandtraps < 20; angbandtraps++) {

					angbandx = rn1(COLNO-3,2);
					angbandy = rn2(ROWNO);

					if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ > DBWALL) && !(t_at(angbandx, angbandy)) ) {

					rtrap = randomtrap();

					(void) maketrap(angbandx, angbandy, rtrap, 100, TRUE);
					}
				}

			}

			else if (rnd(Luck + 15) < 6) {
				pline("This level looks reasonably safe.");
			}

			else if (rnd(Luck + 15) < 10) {
				pline("This level can't be all that bad...");

				for (angbandtraps = 0; angbandtraps < 20; angbandtraps++) {

					angbandx = rn1(COLNO-3,2);
					angbandy = rn2(ROWNO);

					if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ == ROOM || levl[angbandx][angbandy].typ == CORR) && !(t_at(angbandx, angbandy)) ) {

					    (void) mkobj_at(RANDOM_CLASS, angbandx, angbandy, FALSE, FALSE);

					}
				}

			}

			else if (rnd(Luck + 15) < 15) {
				pline("You have a good feeling about this level.");

				for (angbandtraps = 0; angbandtraps < 50; angbandtraps++) {

					angbandx = rn1(COLNO-3,2);
					angbandy = rn2(ROWNO);

					if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ == ROOM || levl[angbandx][angbandy].typ == CORR) && !(t_at(angbandx, angbandy)) ) {

					    (void) mkobj_at(RANDOM_CLASS, angbandx, angbandy, FALSE, FALSE);
						if (!rn2(50)) (void) mkobj_at(SCROLL_CLASS, angbandx, angbandy, FALSE, FALSE);
						if (!rn2(50)) (void) mkobj_at(POTION_CLASS, angbandx, angbandy, FALSE, FALSE);
						if (!rn2(100)) (void) mkobj_at(WAND_CLASS, angbandx, angbandy, FALSE, FALSE);
						if (!rn2(50)) (void) mkobj_at(SPBOOK_CLASS, angbandx, angbandy, FALSE, FALSE);
					}
				}

				angbandx = rn1(COLNO-3,2);
				angbandy = rn2(ROWNO);

				if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ == ROOM || levl[angbandx][angbandy].typ == CORR) && !(t_at(angbandx, angbandy)) ) {

				    (void) mksobj_at(usefulitem(), angbandx, angbandy, TRUE, TRUE, FALSE);
				}

			}

			else if (rnd(Luck + 15) < 24) {
				pline("You have a very good feeling about this level.");

				for (angbandtraps = 0; angbandtraps < 100; angbandtraps++) {

					angbandx = rn1(COLNO-3,2);
					angbandy = rn2(ROWNO);

					if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ == ROOM || levl[angbandx][angbandy].typ == CORR) && !(t_at(angbandx, angbandy)) ) {

					    (void) mkobj_at(RANDOM_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(SCROLL_CLASS, angbandx, angbandy, FALSE, FALSE);
						if (!rn2(50)) (void) mkobj_at(POTION_CLASS, angbandx, angbandy, FALSE, FALSE);
						if (!rn2(100)) (void) mkobj_at(WAND_CLASS, angbandx, angbandy, FALSE, FALSE);
						if (!rn2(100)) (void) mkobj_at(WEAPON_CLASS, angbandx, angbandy, FALSE, FALSE);
						if (!rn2(100)) (void) mkobj_at(ARMOR_CLASS, angbandx, angbandy, FALSE, FALSE);
						if (!rn2(50)) (void) mkobj_at(SPBOOK_CLASS, angbandx, angbandy, FALSE, FALSE);
					}
				}

				for (angbandtraps = 0; angbandtraps < 3; angbandtraps++) {

					angbandx = rn1(COLNO-3,2);
					angbandy = rn2(ROWNO);

					if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ == ROOM || levl[angbandx][angbandy].typ == CORR) && !(t_at(angbandx, angbandy)) ) {
	
					    (void) mksobj_at(usefulitem(), angbandx, angbandy, TRUE, TRUE, FALSE);
					}
				}

			}

			else if (rnd(Luck + 200) < 205) {
				pline("You have a superb feeling about this level.");

				for (angbandtraps = 0; angbandtraps < 250; angbandtraps++) {

					angbandx = rn1(COLNO-3,2);
					angbandy = rn2(ROWNO);

					if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ == ROOM || levl[angbandx][angbandy].typ == CORR) && !(t_at(angbandx, angbandy)) ) {

					    (void) mkobj_at(RANDOM_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(SCROLL_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(POTION_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(100)) (void) mkobj_at(WAND_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(100)) (void) mkobj_at(WEAPON_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(100)) (void) mkobj_at(ARMOR_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(SPBOOK_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(RING_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(150)) (void) mkobj_at(AMULET_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(TOOL_CLASS, angbandx, angbandy, TRUE, FALSE);

						if (!rn2(1000)) (void) maketrap(angbandx, angbandy, AUTOMATIC_SWITCHER, 0, FALSE);

					}
				}

				for (angbandtraps = 0; angbandtraps < 10; angbandtraps++) {

					angbandx = rn1(COLNO-3,2);
					angbandy = rn2(ROWNO);

					if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ == ROOM || levl[angbandx][angbandy].typ == CORR) && !(t_at(angbandx, angbandy)) ) {
	
					    (void) mksobj_at(usefulitem(), angbandx, angbandy, TRUE, TRUE, FALSE);
					}
				}
			}
			else {
				pline("You have an extremely superb feeling about this level.");

				for (angbandtraps = 0; angbandtraps < 250; angbandtraps++) {

					angbandx = rn1(COLNO-3,2);
					angbandy = rn2(ROWNO);

					if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ == ROOM || levl[angbandx][angbandy].typ == CORR) && !(t_at(angbandx, angbandy)) ) {

					    (void) mkobj_at(RANDOM_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(SCROLL_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(POTION_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(100)) (void) mkobj_at(WAND_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(100)) (void) mkobj_at(WEAPON_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(100)) (void) mkobj_at(ARMOR_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(SPBOOK_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(RING_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(150)) (void) mkobj_at(AMULET_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(TOOL_CLASS, angbandx, angbandy, TRUE, FALSE);
					    (void) mkobj_at(RANDOM_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(SCROLL_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(POTION_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(100)) (void) mkobj_at(WAND_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(100)) (void) mkobj_at(WEAPON_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(100)) (void) mkobj_at(ARMOR_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(SPBOOK_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(RING_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(150)) (void) mkobj_at(AMULET_CLASS, angbandx, angbandy, TRUE, FALSE);
						if (!rn2(50)) (void) mkobj_at(TOOL_CLASS, angbandx, angbandy, TRUE, FALSE);
					      if (!rn2(5)) (void) mkobj_at(RANDOM_CLASS, angbandx, angbandy, TRUE, FALSE);
					      if (!rn2(10)) (void) mkobj_at(RANDOM_CLASS, angbandx, angbandy, TRUE, FALSE);
					      if (!rn2(25)) (void) mkobj_at(RANDOM_CLASS, angbandx, angbandy, TRUE, FALSE);
					      if (!rn2(50)) (void) mkobj_at(RANDOM_CLASS, angbandx, angbandy, TRUE, FALSE);
					      if (!rn2(100)) (void) mkobj_at(RANDOM_CLASS, angbandx, angbandy, TRUE, FALSE);

						if (!rn2(500)) (void) maketrap(angbandx, angbandy, AUTOMATIC_SWITCHER, 0, FALSE);

					}
				}

				for (angbandtraps = 0; angbandtraps < 30; angbandtraps++) {

					angbandx = rn1(COLNO-3,2);
					angbandy = rn2(ROWNO);

					if (angbandx && angbandy && isok(angbandx, angbandy) && (levl[angbandx][angbandy].typ == ROOM || levl[angbandx][angbandy].typ == CORR) && !(t_at(angbandx, angbandy)) ) {
	
					    (void) mksobj_at(usefulitem(), angbandx, angbandy, TRUE, TRUE, FALSE);
					}
				}
			}
			u.aggravation = 0;
		}

		/* very rarely, spawn a group of typed monsters --Amy */

		if (!rn2(iswarper ? 160 : 8000)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			randmnst = (rn2(187) + 1);
			randmnsx = (rn2(100) + 1);

				if (wizard || !rn2(10)) pline(FunnyHallu ? "Crash bugs probably abound here, the dungeon is likely to collapse soon..." : "The air around here seems charged with tension!");

			for (i = 0; i < randsp; i++) {
			/* This function will fill the map with a random amount of monsters of one class. --Amy */

			if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

			if (randmnst < 6)
		 	    (void) makemon(mkclass(S_ANT,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 9)
		 	    (void) makemon(mkclass(S_BLOB,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 11)
		 	    (void) makemon(mkclass(S_COCKATRICE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 15)
		 	    (void) makemon(mkclass(S_DOG,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 18)
		 	    (void) makemon(mkclass(S_EYE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 22)
		 	    (void) makemon(mkclass(S_FELINE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 24)
		 	    (void) makemon(mkclass(S_GREMLIN,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 29)
		 	    (void) makemon(mkclass(S_HUMANOID,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 33)
		 	    (void) makemon(mkclass(S_IMP,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 36)
		 	    (void) makemon(mkclass(S_JELLY,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 41)
		 	    (void) makemon(mkclass(S_KOBOLD,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 44)
		 	    (void) makemon(mkclass(S_LEPRECHAUN,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 47)
		 	    (void) makemon(mkclass(S_MIMIC,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 50)
		 	    (void) makemon(mkclass(S_NYMPH,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 54)
		 	    (void) makemon(mkclass(S_ORC,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 55)
		 	    (void) makemon(mkclass(S_PIERCER,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 58)
		 	    (void) makemon(mkclass(S_QUADRUPED,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 62)
		 	    (void) makemon(mkclass(S_RODENT,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 65)
		 	    (void) makemon(mkclass(S_SPIDER,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 66)
		 	    (void) makemon(mkclass(S_TRAPPER,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 69)
		 	    (void) makemon(mkclass(S_UNICORN,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 71)
		 	    (void) makemon(mkclass(S_VORTEX,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 73)
		 	    (void) makemon(mkclass(S_WORM,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 75)
		 	    (void) makemon(mkclass(S_XAN,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 76)
		 	    (void) makemon(mkclass(S_LIGHT,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 77)
		 	    (void) makemon(mkclass(S_ZOUTHERN,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 78)
		 	    (void) makemon(mkclass(S_ANGEL,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 81)
		 	    (void) makemon(mkclass(S_BAT,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 83)
		 	    (void) makemon(mkclass(S_CENTAUR,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 86)
		 	    (void) makemon(mkclass(S_DRAGON,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 89)
		 	    (void) makemon(mkclass(S_ELEMENTAL,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 94)
		 	    (void) makemon(mkclass(S_FUNGUS,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 99)
		 	    (void) makemon(mkclass(S_GNOME,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 102)
		 	    (void) makemon(mkclass(S_GIANT,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 103)
		 	    (void) makemon(mkclass(S_JABBERWOCK,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 104)
		 	    (void) makemon(mkclass(S_KOP,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 105)
		 	    (void) makemon(mkclass(S_LICH,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 108)
		 	    (void) makemon(mkclass(S_MUMMY,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 110)
		 	    (void) makemon(mkclass(S_NAGA,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 113)
		 	    (void) makemon(mkclass(S_OGRE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 115)
		 	    (void) makemon(mkclass(S_PUDDING,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 116)
		 	    (void) makemon(mkclass(S_QUANTMECH,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 118)
		 	    (void) makemon(mkclass(S_RUSTMONST,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 121)
		 	    (void) makemon(mkclass(S_SNAKE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 123)
		 	    (void) makemon(mkclass(S_TROLL,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 124)
		 	    (void) makemon(mkclass(S_UMBER,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 125)
		 	    (void) makemon(mkclass(S_VAMPIRE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 127)
		 	    (void) makemon(mkclass(S_WRAITH,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 128)
		 	    (void) makemon(mkclass(S_XORN,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 130)
		 	    (void) makemon(mkclass(S_YETI,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 135)
		 	    (void) makemon(mkclass(S_ZOMBIE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 145)
		 	    (void) makemon(mkclass(S_HUMAN,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 147)
		 	    (void) makemon(mkclass(S_GHOST,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 149)
		 	    (void) makemon(mkclass(S_GOLEM,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 152)
		 	    (void) makemon(mkclass(S_DEMON,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 155)
		 	    (void) makemon(mkclass(S_EEL,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 160)
		 	    (void) makemon(mkclass(S_LIZARD,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 162)
		 	    (void) makemon(mkclass(S_BAD_FOOD,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 165)
		 	    (void) makemon(mkclass(S_BAD_COINS,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 166) {
				if (randmnsx < 96)
		 	    (void) makemon(mkclass(S_HUMAN,0), 0, 0, NO_MM_FLAGS);
				else
		 	    (void) makemon(mkclass(S_NEMESE,0), 0, 0, NO_MM_FLAGS);
				}
			else if (randmnst < 171)
		 	    (void) makemon(mkclass(S_GRUE,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 176)
		 	    (void) makemon(mkclass(S_WALLMONST,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 180)
		 	    (void) makemon(mkclass(S_RUBMONST,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 181) {
				if (randmnsx < 99)
		 	    (void) makemon(mkclass(S_HUMAN,0), 0, 0, NO_MM_FLAGS);
				else
		 	    (void) makemon(mkclass(S_ARCHFIEND,0), 0, 0, NO_MM_FLAGS);
				}
			else if (randmnst < 186)
		 	    (void) makemon(mkclass(S_TURRET,0), 0, 0, NO_MM_FLAGS);
			else if (randmnst < 187)
		 	    (void) makemon(mkclass(S_FLYFISH,0), 0, 0, NO_MM_FLAGS);
			else
		 	    (void) makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS);

			}
		}

		if (!rn2(iswarper ? 160 : 8000)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			randmnst = (rn2(187) + 1);
			randmnsx = (rn2(100) + 1);
		      cx = rn2(COLNO);
		      cy = rn2(ROWNO);

				if (wizard || !rn2(10)) pline(FunnyHallu ? "Crash bugs probably abound here, the dungeon is likely to collapse soon..." : "The air around here seems charged with tension!");

			for (i = 0; i < randsp; i++) {
			/* This function will fill the map with a random amount of monsters of one class. --Amy */

			if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

			if (randmnst < 6)
		 	    (void) makemon(mkclass(S_ANT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 9)
		 	    (void) makemon(mkclass(S_BLOB,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 11)
		 	    (void) makemon(mkclass(S_COCKATRICE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 15)
		 	    (void) makemon(mkclass(S_DOG,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 18)
		 	    (void) makemon(mkclass(S_EYE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 22)
		 	    (void) makemon(mkclass(S_FELINE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 24)
		 	    (void) makemon(mkclass(S_GREMLIN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 29)
		 	    (void) makemon(mkclass(S_HUMANOID,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 33)
		 	    (void) makemon(mkclass(S_IMP,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 36)
		 	    (void) makemon(mkclass(S_JELLY,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 41)
		 	    (void) makemon(mkclass(S_KOBOLD,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 44)
		 	    (void) makemon(mkclass(S_LEPRECHAUN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 47)
		 	    (void) makemon(mkclass(S_MIMIC,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 50)
		 	    (void) makemon(mkclass(S_NYMPH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 54)
		 	    (void) makemon(mkclass(S_ORC,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 55)
		 	    (void) makemon(mkclass(S_PIERCER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 58)
		 	    (void) makemon(mkclass(S_QUADRUPED,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 62)
		 	    (void) makemon(mkclass(S_RODENT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 65)
		 	    (void) makemon(mkclass(S_SPIDER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 66)
		 	    (void) makemon(mkclass(S_TRAPPER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 69)
		 	    (void) makemon(mkclass(S_UNICORN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 71)
		 	    (void) makemon(mkclass(S_VORTEX,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 73)
		 	    (void) makemon(mkclass(S_WORM,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 75)
		 	    (void) makemon(mkclass(S_XAN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 76)
		 	    (void) makemon(mkclass(S_LIGHT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 77)
		 	    (void) makemon(mkclass(S_ZOUTHERN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 78)
		 	    (void) makemon(mkclass(S_ANGEL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 81)
		 	    (void) makemon(mkclass(S_BAT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 83)
		 	    (void) makemon(mkclass(S_CENTAUR,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 86)
		 	    (void) makemon(mkclass(S_DRAGON,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 89)
		 	    (void) makemon(mkclass(S_ELEMENTAL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 94)
		 	    (void) makemon(mkclass(S_FUNGUS,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 99)
		 	    (void) makemon(mkclass(S_GNOME,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 102)
		 	    (void) makemon(mkclass(S_GIANT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 103)
		 	    (void) makemon(mkclass(S_JABBERWOCK,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 104)
		 	    (void) makemon(mkclass(S_KOP,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 105)
		 	    (void) makemon(mkclass(S_LICH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 108)
		 	    (void) makemon(mkclass(S_MUMMY,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 110)
		 	    (void) makemon(mkclass(S_NAGA,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 113)
		 	    (void) makemon(mkclass(S_OGRE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 115)
		 	    (void) makemon(mkclass(S_PUDDING,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 116)
		 	    (void) makemon(mkclass(S_QUANTMECH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 118)
		 	    (void) makemon(mkclass(S_RUSTMONST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 121)
		 	    (void) makemon(mkclass(S_SNAKE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 123)
		 	    (void) makemon(mkclass(S_TROLL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 124)
		 	    (void) makemon(mkclass(S_UMBER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 125)
		 	    (void) makemon(mkclass(S_VAMPIRE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 127)
		 	    (void) makemon(mkclass(S_WRAITH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 128)
		 	    (void) makemon(mkclass(S_XORN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 130)
		 	    (void) makemon(mkclass(S_YETI,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 135)
		 	    (void) makemon(mkclass(S_ZOMBIE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 145)
		 	    (void) makemon(mkclass(S_HUMAN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 147)
		 	    (void) makemon(mkclass(S_GHOST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 149)
		 	    (void) makemon(mkclass(S_GOLEM,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 152)
		 	    (void) makemon(mkclass(S_DEMON,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 155)
		 	    (void) makemon(mkclass(S_EEL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 160)
		 	    (void) makemon(mkclass(S_LIZARD,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 162)
		 	    (void) makemon(mkclass(S_BAD_FOOD,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 165)
		 	    (void) makemon(mkclass(S_BAD_COINS,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 166) {
				if (randmnsx < 96)
		 	    (void) makemon(mkclass(S_HUMAN,0), cx, cy, MM_ADJACENTOK);
				else
		 	    (void) makemon(mkclass(S_NEMESE,0), cx, cy, MM_ADJACENTOK);
				}
			else if (randmnst < 171)
		 	    (void) makemon(mkclass(S_GRUE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 176)
		 	    (void) makemon(mkclass(S_WALLMONST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 180)
		 	    (void) makemon(mkclass(S_RUBMONST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 181) {
				if (randmnsx < 99)
		 	    (void) makemon(mkclass(S_HUMAN,0), cx, cy, MM_ADJACENTOK);
				else
		 	    (void) makemon(mkclass(S_ARCHFIEND,0), cx, cy, MM_ADJACENTOK);
				}
			else if (randmnst < 186)
		 	    (void) makemon(mkclass(S_TURRET,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 187)
		 	    (void) makemon(mkclass(S_FLYFISH,0), cx, cy, MM_ADJACENTOK);
			else
		 	    (void) makemon((struct permonst *)0, cx, cy, MM_ADJACENTOK);

			}
		}

		if (!rn2(ishaxor ? (1500 - (Luck*50)) : (3000 - (Luck*50)) ) ) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			if (randsp > 1) randsp = rnd(randsp);

			if (wizard || !rn2(10)) pline(FunnyHallu ? "The RNG exceptionally seems to be on your side..." : "You feel that there's lots of treasure to be found here!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				if (timebasedlowerchance()) (void) makemon(&mons[PM_ITEM_MASTER], 0, 0, NO_MM_FLAGS);
			}

		}

		if (!rn2(ishaxor ? (50000 - (Luck*1000)) : (100000 - (Luck*1000)) ) ) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			if (randsp > 1) randsp = rnd(randsp);

			if (wizard || !rn2(10)) pline(FunnyHallu ? "The RNG whispers to you: 'Today's your lucky day!'" : "You feel that there's lots of good stuff to be found here!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				if (timebasedlowerchance()) (void) makemon(&mons[PM_GOOD_ITEM_MASTER], 0, 0, NO_MM_FLAGS);
			}

		}

		if (!rn2(ishaxor ? (25000 + (Luck*1000)) : (50000 + (Luck*1000)) ) ) {

			randsp = (rn3(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			if (randsp > 1) randsp = rnd(randsp);

			if (wizard || !rn2(10)) pline(FunnyHallu ? "The RNG's voice booms out: 'You were fated to die on this level. DIE!'" : "You feel that the monsters are exceptionally well-armed here!");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(&mons[PM_BAD_ITEM_MASTER], 0, 0, NO_MM_FLAGS);

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

			 	(void) makemon((struct permonst *)0, 0, 0, MM_ADJACENTOK);

				if (!rn2(2)) {

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				      (void) makemon((struct permonst *)0, 0, 0, MM_ADJACENTOK);

				}

			}

		}

		if (!rn2(iswarper ? 160 : 8000)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			randmonstforspawn = rndmonst();

			if (wizard || !rn2(10)) pline(FunnyHallu ? "Very unstable architecture here, it seems..." : "It seems there might be lots of monsters around here...");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(randmonstforspawn, 0, 0, NO_MM_FLAGS);
			}

		}

		if (Role_if(PM_GANG_SCHOLAR) && !rn2(5)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			switch (rnd(47)) {
				case 1:
				case 2: monstercolor = 38; break; /* MS_FART_QUIET */
				case 3:
				case 4: monstercolor = 39; break; /* MS_FART_NORMAL */
				case 5:
				case 6: monstercolor = 40; break; /* MS_FART_LOUD */
				case 7: monstercolor = 333; break; /* MS_STENCH */
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				case 16:
				case 17:
				case 18:
				case 19:
				case 20:
				case 21:
				case 22:
				case 23:
				case 24:
				case 25:
				case 26:
				case 27: monstercolor = 322; break; /* M5_SPACEWARS */
				case 28:
				case 29:
				case 30:
				case 31:
				case 32:
				case 33:
				case 34:
				case 35:
				case 36:
				case 37:
				case 38:
				case 39:
				case 40:
				case 41:
				case 42:
				case 43:
				case 44:
				case 45:
				case 46:
				case 47: monstercolor = 323; break; /* M5_JOKE */
			}

			for (i = 0; i < randsp; i++) {
				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(monstercolor), 0, 0, NO_MM_FLAGS);
			}

		}

		if (!rn2(iswarper ? 160 : 8000)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			monstercolor = rnd(15);
			do { monstercolor = rnd(15); } while (monstercolor == CLR_BLUE);

			if (wizard || !rn2(10)) pline(FunnyHallu ? "Uh... wow, what a strong color flash of rainbows!" : "You feel that a certain color might be prominent around here...");

			for (i = 0; i < randsp; i++) {
				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(colormon(monstercolor), 0, 0, NO_MM_FLAGS);
			}

		}

		if (!rn2(iswarper ? 160 : 8000)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			randmonstforspawn = rndmonst();
		      cx = rn2(COLNO);
		      cy = rn2(ROWNO);

			if (wizard || !rn2(10)) pline(FunnyHallu ? "Very unstable architecture here, it seems..." : "It seems there might be lots of monsters around here...");

			for (i = 0; i < randsp; i++) {

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(randmonstforspawn, cx, cy, MM_ADJACENTOK);
			}

		}

		if (Role_if(PM_GANG_SCHOLAR) && !rn2(5)) {
			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
		      cx = rn2(COLNO);
		      cy = rn2(ROWNO);
			switch (rnd(47)) {
				case 1:
				case 2: monstercolor = 38; break; /* MS_FART_QUIET */
				case 3:
				case 4: monstercolor = 39; break; /* MS_FART_NORMAL */
				case 5:
				case 6: monstercolor = 40; break; /* MS_FART_LOUD */
				case 7: monstercolor = 333; break; /* MS_STENCH */
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				case 16:
				case 17:
				case 18:
				case 19:
				case 20:
				case 21:
				case 22:
				case 23:
				case 24:
				case 25:
				case 26:
				case 27: monstercolor = 322; break; /* M5_SPACEWARS */
				case 28:
				case 29:
				case 30:
				case 31:
				case 32:
				case 33:
				case 34:
				case 35:
				case 36:
				case 37:
				case 38:
				case 39:
				case 40:
				case 41:
				case 42:
				case 43:
				case 44:
				case 45:
				case 46:
				case 47: monstercolor = 323; break; /* M5_JOKE */
			}

			for (i = 0; i < randsp; i++) {
				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(monstercolor), cx, cy, MM_ADJACENTOK);
			}

		}

		if (!rn2(iswarper ? 160 : 8000)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			monstercolor = rnd(15);
			do { monstercolor = rnd(15); } while (monstercolor == CLR_BLUE);
		      cx = rn2(COLNO);
		      cy = rn2(ROWNO);

			if (wizard || !rn2(10)) pline(FunnyHallu ? "Uh... wow, what a strong color flash of rainbows!" : "You feel that a certain color might be prominent around here...");

			for (i = 0; i < randsp; i++) {
				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(colormon(monstercolor), cx, cy, MM_ADJACENTOK);
			}

		}

		if (!rn2(iswarper ? 160 : 8000)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			monstercolor = rnd(376);

			if (wizard || !rn2(10)) pline(FunnyHallu ? "Err... is someone here? Hello-o, please show yourself!" : "Seems like someone made their home on this dungeon level.");

			for (i = 0; i < randsp; i++) {
				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(monstercolor), 0, 0, NO_MM_FLAGS);
			}

		}

		if (!rn2(iswarper ? 160 : 8000)) {

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			monstercolor = rnd(376);

		      cx = rn2(COLNO);
		      cy = rn2(ROWNO);

			if (wizard || !rn2(10)) pline(FunnyHallu ? "Err... is someone here? Hello-o, please show yourself!" : "Seems like someone made their home on this dungeon level.");

			for (i = 0; i < randsp; i++) {
				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(monstercolor), cx, cy, MM_ADJACENTOK);
			}

		}

		if (In_netherrealm(&u.uz)) {

		switch (rnd(4)) {

			case 1:

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			randmnst = (rn2(187) + 1);
			randmnsx = (rn2(100) + 1);
			if (rn2(2)) {
			      cx = rn2(COLNO);
			      cy = rn2(ROWNO);
			} else {
			      cx = 0;
			      cy = 0;
			}

			for (i = 0; i < randsp; i++) {
			/* This function will fill the map with a random amount of monsters of one class. --Amy */

			if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

			if (randmnst < 6)
		 	    (void) makemon(mkclass(S_ANT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 9)
		 	    (void) makemon(mkclass(S_BLOB,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 11)
		 	    (void) makemon(mkclass(S_COCKATRICE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 15)
		 	    (void) makemon(mkclass(S_DOG,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 18)
		 	    (void) makemon(mkclass(S_EYE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 22)
		 	    (void) makemon(mkclass(S_FELINE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 24)
		 	    (void) makemon(mkclass(S_GREMLIN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 29)
		 	    (void) makemon(mkclass(S_HUMANOID,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 33)
		 	    (void) makemon(mkclass(S_IMP,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 36)
		 	    (void) makemon(mkclass(S_JELLY,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 41)
		 	    (void) makemon(mkclass(S_KOBOLD,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 44)
		 	    (void) makemon(mkclass(S_LEPRECHAUN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 47)
		 	    (void) makemon(mkclass(S_MIMIC,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 50)
		 	    (void) makemon(mkclass(S_NYMPH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 54)
		 	    (void) makemon(mkclass(S_ORC,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 55)
		 	    (void) makemon(mkclass(S_PIERCER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 58)
		 	    (void) makemon(mkclass(S_QUADRUPED,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 62)
		 	    (void) makemon(mkclass(S_RODENT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 65)
		 	    (void) makemon(mkclass(S_SPIDER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 66)
		 	    (void) makemon(mkclass(S_TRAPPER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 69)
		 	    (void) makemon(mkclass(S_UNICORN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 71)
		 	    (void) makemon(mkclass(S_VORTEX,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 73)
		 	    (void) makemon(mkclass(S_WORM,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 75)
		 	    (void) makemon(mkclass(S_XAN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 76)
		 	    (void) makemon(mkclass(S_LIGHT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 77)
		 	    (void) makemon(mkclass(S_ZOUTHERN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 78)
		 	    (void) makemon(mkclass(S_ANGEL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 81)
		 	    (void) makemon(mkclass(S_BAT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 83)
		 	    (void) makemon(mkclass(S_CENTAUR,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 86)
		 	    (void) makemon(mkclass(S_DRAGON,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 89)
		 	    (void) makemon(mkclass(S_ELEMENTAL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 94)
		 	    (void) makemon(mkclass(S_FUNGUS,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 99)
		 	    (void) makemon(mkclass(S_GNOME,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 102)
		 	    (void) makemon(mkclass(S_GIANT,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 103)
		 	    (void) makemon(mkclass(S_JABBERWOCK,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 104)
		 	    (void) makemon(mkclass(S_KOP,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 105)
		 	    (void) makemon(mkclass(S_LICH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 108)
		 	    (void) makemon(mkclass(S_MUMMY,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 110)
		 	    (void) makemon(mkclass(S_NAGA,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 113)
		 	    (void) makemon(mkclass(S_OGRE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 115)
		 	    (void) makemon(mkclass(S_PUDDING,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 116)
		 	    (void) makemon(mkclass(S_QUANTMECH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 118)
		 	    (void) makemon(mkclass(S_RUSTMONST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 121)
		 	    (void) makemon(mkclass(S_SNAKE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 123)
		 	    (void) makemon(mkclass(S_TROLL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 124)
		 	    (void) makemon(mkclass(S_UMBER,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 125)
		 	    (void) makemon(mkclass(S_VAMPIRE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 127)
		 	    (void) makemon(mkclass(S_WRAITH,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 128)
		 	    (void) makemon(mkclass(S_XORN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 130)
		 	    (void) makemon(mkclass(S_YETI,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 135)
		 	    (void) makemon(mkclass(S_ZOMBIE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 145)
		 	    (void) makemon(mkclass(S_HUMAN,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 147)
		 	    (void) makemon(mkclass(S_GHOST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 149)
		 	    (void) makemon(mkclass(S_GOLEM,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 152)
		 	    (void) makemon(mkclass(S_DEMON,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 155)
		 	    (void) makemon(mkclass(S_EEL,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 160)
		 	    (void) makemon(mkclass(S_LIZARD,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 162)
		 	    (void) makemon(mkclass(S_BAD_FOOD,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 165)
		 	    (void) makemon(mkclass(S_BAD_COINS,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 166) {
				if (randmnsx < 96)
		 	    (void) makemon(mkclass(S_HUMAN,0), cx, cy, MM_ADJACENTOK);
				else
		 	    (void) makemon(mkclass(S_NEMESE,0), cx, cy, MM_ADJACENTOK);
				}
			else if (randmnst < 171)
		 	    (void) makemon(mkclass(S_GRUE,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 176)
		 	    (void) makemon(mkclass(S_WALLMONST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 180)
		 	    (void) makemon(mkclass(S_RUBMONST,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 181) {
				if (randmnsx < 99)
		 	    (void) makemon(mkclass(S_HUMAN,0), cx, cy, MM_ADJACENTOK);
				else
		 	    (void) makemon(mkclass(S_ARCHFIEND,0), cx, cy, MM_ADJACENTOK);
				}
			else if (randmnst < 186)
		 	    (void) makemon(mkclass(S_TURRET,0), cx, cy, MM_ADJACENTOK);
			else if (randmnst < 187)
		 	    (void) makemon(mkclass(S_FLYFISH,0), cx, cy, MM_ADJACENTOK);
			else
		 	    (void) makemon((struct permonst *)0, cx, cy, MM_ADJACENTOK);

			}

			break;

			case 2:

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			randmonstforspawn = rndmonst();
			if (rn2(2)) {
			      cx = rn2(COLNO);
			      cy = rn2(ROWNO);
			} else {
			      cx = 0;
			      cy = 0;
			}

			for (i = 0; i < randsp; i++) {

				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(randmonstforspawn, cx, cy, MM_ADJACENTOK);
			}

			break;

			case 3:

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			monstercolor = rnd(15);
			do { monstercolor = rnd(15); } while (monstercolor == CLR_BLUE);
			if (rn2(2)) {
			      cx = rn2(COLNO);
			      cy = rn2(ROWNO);
			} else {
			      cx = 0;
			      cy = 0;
			}

			for (i = 0; i < randsp; i++) {
				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(colormon(monstercolor), cx, cy, MM_ADJACENTOK);
			}

			break;

			case 4:

			randsp = (rn2(14) + 2);
			if (!rn2(10)) randsp *= 2;
			if (!rn2(100)) randsp *= 3;
			if (!rn2(1000)) randsp *= 5;
			if (!rn2(10000)) randsp *= 10;
			monstercolor = rnd(376);
			if (rn2(2)) {
			      cx = rn2(COLNO);
			      cy = rn2(ROWNO);
			} else {
			      cx = 0;
			      cy = 0;
			}

			for (i = 0; i < randsp; i++) {
				if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

				(void) makemon(specialtensmon(monstercolor), cx, cy, MM_ADJACENTOK);
			}

			break;

		} /* switch */

		} /* in nether realm */

		/* some levels are populated with a few undead player monsters --Amy */
		if (on_level(&u.uz, &earth_level))
			create_umplayers(rn1(2, 1), TRUE);
		if (on_level(&u.uz, &water_level))
			create_umplayers(rn1(2, 1), TRUE);
		if (on_level(&u.uz, &air_level))
			create_umplayers(rn1(2, 1), TRUE);
		if (on_level(&u.uz, &fire_level))
			create_umplayers(rn1(2, 1), TRUE);
		if (on_level(&u.uz, &sanctum_level))
			create_umplayers(rn1(2, 1), FALSE);
		if (on_level(&u.uz, &valley_level))
			create_umplayers(rn1(6, 5), FALSE); /* valley of the dead has more undead --Amy */

		if (ishaxor && on_level(&u.uz, &earth_level))
			create_umplayers(rn1(2, 1), TRUE);
		if (ishaxor && on_level(&u.uz, &water_level))
			create_umplayers(rn1(2, 1), TRUE);
		if (ishaxor && on_level(&u.uz, &air_level))
			create_umplayers(rn1(2, 1), TRUE);
		if (ishaxor && on_level(&u.uz, &fire_level))
			create_umplayers(rn1(2, 1), TRUE);
		if (ishaxor && on_level(&u.uz, &sanctum_level))
			create_umplayers(rn1(2, 1), FALSE);
		if (ishaxor && on_level(&u.uz, &valley_level))
			create_umplayers(rn1(6, 5), FALSE); /* valley of the dead has more undead --Amy */

	} else {
		/* returning to previously visited level; reload it */
		fd = open_levelfile(new_ledger, whynot);
		if (fd < 0) {
			pline("%s", whynot);
			pline("Probably someone removed it.");
			/*killer = whynot;
			done(TRICKED);*/
			/* we'll reach here if running in wizard mode */
			error("Cannot continue this game.");
		}
		minit();	/* ZEROCOMP */
		getlev(fd, hackpid, new_ledger, FALSE);
		(void) close(fd);
	}

#ifdef ALLEG_FX
        if (iflags.usealleg) nh_fade_out(portal);
#endif

	/* do this prior to level-change pline messages */
	vision_reset();		/* clear old level's line-of-sight */
	vision_full_recalc = 0;	/* don't let that reenable vision yet */
	flush_screen(-1);	/* ensure all map flushes are postponed */

	if (portal && !In_endgame(&u.uz)) {
	    /* find the portal on the new level */
	    register struct trap *ttrap;

	    for (ttrap = ftrap; ttrap; ttrap = ttrap->ntrap)
            /* find the portal with the right destination level (thanks bhaak --Amy) */
            if (ttrap->ttyp == MAGIC_PORTAL &&
                u.uz0.dnum == ttrap->dst.dnum &&
                u.uz0.dlevel == ttrap->dst.dlevel) break;

	    if (!ttrap) panic("goto_level: no corresponding portal!");
	    seetrap(ttrap);
	    u_on_newpos(ttrap->tx, ttrap->ty);
	} else if (at_stairs && !In_endgame(&u.uz)) {
	    if (up) {
		if (at_ladder) {
		    u_on_newpos(xdnladder, ydnladder);
		} else {
		    if (newdungeon) {
			if (Is_stronghold(&u.uz)) {
			    register xchar x, y;
			    int attempts = 0;

			    do {
#ifdef BIGSLEX
				x = (COLNO - 25 - rnd(5));
				y = rn1(ROWNO - 11, 12);
#else
				x = (COLNO - 2 - rnd(5));
				y = rn1(ROWNO - 4, 3);
#endif
				attempts++;
			    } while ((occupied(x, y) || IS_STWALL(levl[x][y].typ) || IS_WATERTUNNEL(levl[x][y].typ) || !goodpos(x, y, &youmonst, 0)) && attempts < 999999);
			    u_on_newpos(x, y);
			} else u_on_sstairs();
		    } else u_on_dnstairs();
		}
		/* Remove bug which crashes with levitation/punishment  KAA */
		if (Punished && !Levitation) {
			pline("With great effort you climb the %s.",
				at_ladder ? "ladder" : "stairs");
		} else if (at_ladder)
		    You("climb up the ladder.");
	    } else {	/* down */
		if (at_ladder) {
		    u_on_newpos(xupladder, yupladder);
		} else {
		    if (newdungeon) u_on_sstairs();
		    else u_on_upstairs();
		}
		if (u.dz && Flying)
		    You("fly down along the %s.",
			at_ladder ? "ladder" : "stairs");
		else if (u.dz &&
		    (near_capacity() > UNENCUMBERED || (Punished &&
		    ((uwep != uball) || PlayerCannotUseSkills || ((P_SKILL(P_FLAIL) < P_BASIC))
            || !Role_if(PM_CONVICT)))
		     || Fumbling || (Confusion && !Conf_resist && !rn2(20) && !Race_if(PM_ADDICT)) || (Stunned && !Stun_resist && !rn2(5) && !Race_if(PM_TUMBLRER) && !Race_if(PM_REDDITOR)) )) {
		    You("fall down the %s.", at_ladder ? "ladder" : "stairs");

		    if (!rn2(Role_if(PM_COURIER) ? 1000 : uarmh ? 50 : 10) && has_head(youmonst.data) && !Role_if(PM_COURIER) ) { /* evil patch idea by jonadab: amnesia */

			if (rn2(50)) {
				adjattrib(rn2(2) ? A_INT : A_WIS, -rno(3), FALSE, TRUE);
				if (!rn2(50)) adjattrib(rn2(2) ? A_INT : A_WIS, -rno(2), FALSE, TRUE);
			} else {
				You_feel("dizzy!");
				forget(1 + rn2(5));
			}
		    }
		    if (!rn2(15)) { /* evil patch idea by jonadab */
			    set_wounded_legs(LEFT_SIDE, HWounded_legs + rnd(50));
			    set_wounded_legs(RIGHT_SIDE, HWounded_legs + rnd(50));
			    losehp(rnd(20), "breaking their legs", KILLED_BY);
			    pline("You crash into the ground very painfully, breaking your legs.");
		    }

		    if (Punished) {
			drag_down();
			if (carried(uball)) {
			    if (uswapwep == uball)
				setuswapwep((struct obj *)0, FALSE);
			    if (uquiver == uball)
				setuqwep((struct obj *)0);
			    if (uwep == uball)
				setuwep((struct obj *)0, FALSE, TRUE);
			    freeinv(uball);
			}
		    }
		    /* falling off steed has its own losehp() call */
		    if (u.usteed) {
			if (!mayfalloffsteed())
				dismount_steed(DISMOUNT_FELL);
		    } else
			losehp(rnd(3), "falling downstairs", KILLED_BY);
		    selftouch("Falling, you");
		} else if (u.dz && at_ladder)
		    You("climb down the ladder.");
	    }
	} else {	/* trap door or level_tele or In_endgame */
	    if (was_in_W_tower && On_W_tower_level(&u.uz))
		/* Stay inside the Wizard's tower when feasible.	*/
		/* Note: up vs down doesn't really matter in this case. */
		place_lregion(dndest.nlx, dndest.nly,
				dndest.nhx, dndest.nhy,
				0,0, 0,0, LR_DOWNTELE, (d_level *) 0);
	    else if (up)
		place_lregion(updest.lx, updest.ly,
				updest.hx, updest.hy,
				updest.nlx, updest.nly,
				updest.nhx, updest.nhy,
				LR_UPTELE, (d_level *) 0);
	    else
		place_lregion(dndest.lx, dndest.ly,
				dndest.hx, dndest.hy,
				dndest.nlx, dndest.nly,
				dndest.nhx, dndest.nhy,
				LR_DOWNTELE, (d_level *) 0);
	    if (falling) {
		if (Punished) ballfall();
		selftouch("Falling, you");
	    }
	}

	if (Punished) placebc();
	obj_delivery();		/* before killing geno'd monsters' eggs */
	losedogs();
	kill_genocided_monsters();  /* for those wiped out while in limbo */
	/*
	 * Expire all timers that have gone off while away.  Must be
	 * after migrating monsters and objects are delivered
	 * (losedogs and obj_delivery).
	 */
	run_timers();

	initrack();

	if ((mtmp = m_at(u.ux, u.uy)) != 0 && mtmp != u.usteed) {
	    /* There's a monster at your target destination; it might be one
	       which accompanied you--see mon_arrive(dogmove.c)--or perhaps
	       it was already here.  Randomly move you to an adjacent spot
	       or else the monster to any nearby location.  Prior to 3.3.0
	       the latter was done unconditionally. */
	    coord cc;

	    if (!rn2(2) &&
		    enexto(&cc, u.ux, u.uy, youmonst.data) &&
		    distu(cc.x, cc.y) <= 2)
		u_on_newpos(cc.x, cc.y);	/*[maybe give message here?]*/
	    else
		mnexto(mtmp);

	    if ((mtmp = m_at(u.ux, u.uy)) != 0) {
		impossible("mnexto failed (do.c)?");
		(void) rloc(mtmp, FALSE);
	    }
	}

	/* initial movement of bubbles just before vision_recalc */
	if (Is_waterlevel(&u.uz))
		movebubbles();

	if (level_info[new_ledger].flags & FORGOTTEN) {
	    forget_map(ALL_MAP);	/* forget the map */
	    forget_traps();		/* forget all traps too */
	    familiar = TRUE;
	    level_info[new_ledger].flags &= ~FORGOTTEN;
	}

	/* Reset the screen. */
	vision_reset();		/* reset the blockages */
	docrt();		/* does a full vision recalc */
	flush_screen(-1);

#ifdef ALLEG_FX
        if (iflags.usealleg) nh_fade_in(portal);
#endif

	/*
	 *  Move all plines beyond the screen reset.
	 */

	/* give room entrance message, if any */
	check_special_room(FALSE);

	/* Check whether we just entered Gehennom. */
	if (!In_hell(&u.uz0) && Inhell) {
	    if (Is_valley(&u.uz)) {
		You("arrive at the Valley of the Dead...");
		pline_The("odor of burnt flesh and decay pervades the air.");
#ifdef MICRO
		display_nhwindow(WIN_MESSAGE, FALSE);
#endif
		You_hear("groans and moans everywhere.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Eto konets vas, potomu chto net nikakogo sposoba dlya vas, chtoby vyzhit' beskonechnyye armii vysokogo urovnya nezhit'yu." : "Waeiaer-elauanar-wuuuuuoh.");
	    } else pline("It is hot here.  You smell smoke...");

	}

	/* or whether we left Gehennom - not just Vlad's tower! --Amy */
	if (In_hell(&u.uz0) && !Inhell) 
		pline_The("heat and smoke are gone.");

	if (familiar) {
	    static const char * const fam_msgs[4] = {
		"You have a sense of deja vu.",
		"You feel like you've been here before.",
		"This place %s familiar...",
		0	/* no message */
	    };
	    static const char * const halu_fam_msgs[4] = {
		"Whoa!  Everything %s different.",
		"You are surrounded by twisty little passages, all alike.",
		"Gee, this %s like uncle Conan's place...",
		0	/* no message */
	    };
	    const char *mesg;
	    char buf[BUFSZ];
	    int which = rn2(4);

	    if (FunnyHallu)
		mesg = halu_fam_msgs[which];
	    else
		mesg = fam_msgs[which];
	    if (mesg && index(mesg, '%')) {
		sprintf(buf, mesg, !Blind ? "looks" : "seems");
		mesg = buf;
	    }
	    if (mesg) pline("%s", mesg);
	}

	if (SatanEffect || u.uprops[SATAN_EFFECT].extrinsic || have_satanstone()) {
		nomul(-2, "paralyzed by Satan", TRUE);
	}

#ifdef REINCARNATION
	if (new && Is_rogue_level(&u.uz))
	    You("enter what seems to be an older, more primitive world.");
#endif
	/* Final confrontation */
	if (In_endgame(&u.uz) && newdungeon && u.uhave.amulet && !u.freeplaymode)
		resurrect();

	/* the message from your quest leader */
	if (!In_quest(&u.uz0) && at_dgn_entrance("The Quest") &&
		!(u.uevent.qexpelled || u.uevent.qcompleted || quest_status.leader_is_dead)) {

		/* come on, this shouldn't be the same for every role! --Amy */

		if (u.uevent.qcalled) {
			/*com_pager(Role_if(PM_ROGUE) ? 4 : 3)*/qt_pager(QT_NEXTQLEVEL);
		} else {
			/*com_pager(2)*/qt_pager(QT_FIRSTQLEVEL);
			u.uevent.qcalled = TRUE;
		}
	}

	/* once Croesus is dead, his alarm doesn't work any more */
	if (Is_knox(&u.uz) && (new || !mvitals[PM_CROESUS].died)) {
		You("penetrated a high security area!");
		pline("An alarm sounds!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Kopy posle vas, i potomu chto eto Sovetskaya Rossiya, oni ne budut prosto arestovat' vas. Vmesto etogo oni sobirayutsya postavit' vas v kontslager'. Poveselis'." : "Wueueueueueue! Wueueueueueue! Wueueueueueue! Wueueueueueue! Wueueueueueue!");
		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
		    if (!DEADMONSTER(mtmp) && mtmp->msleeping) mtmp->msleeping = 0;
	}

	if (on_level(&u.uz, &astral_level))
	    final_level();


	else
	    onquest();
	assign_level(&u.uz0, &u.uz); /* reset u.uz0 */

#ifdef INSURANCE
	save_currentstate();
#endif

	/* assume this will always return TRUE when changing level */
	(void) in_out_region(u.ux, u.uy);
	(void) pickup(1);

	if (DisconnectedStairs || u.uprops[DISCONNECTED_STAIRS].extrinsic || (uleft && uleft->oartifact == ART_LIKE_A_REAL_SERVER) || (uright && uright->oartifact == ART_LIKE_A_REAL_SERVER) || have_disconnectstone()) {
	      (void) safe_teleds(FALSE);
		pline("The entrance was disconnected!");
	}

#ifdef WHEREIS_FILE
	touch_whereis();
#endif

}

STATIC_OVL void
final_level()
{
	struct monst *mtmp;
	struct obj *otmp;
	coord mm;
	int i;

	if (u.freeplaymode) {
		pline("Welcome back to the Astral Plane. You have already ascended, so you no longer need the Amulet of Yendor. In order to go back to the regular dungeon, simply press < while standing on one of the high altars.");
		return;
	}

	/* reset monster hostility relative to player */
	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
	    if (!DEADMONSTER(mtmp)) reset_hostility(mtmp);

	/* create some player-monsters */
	create_mplayers(rn1(4, 3), TRUE);
	create_umplayers(rn1(2, 1), TRUE);
	if (!rn2(5)) create_mplayers(rn1(4, 3), TRUE);
	if (!rn2(5)) create_umplayers(rn1(2, 1), TRUE);

	if (ishaxor) create_mplayers(rn1(4, 3), TRUE);
	if (ishaxor) create_umplayers(rn1(2, 1), TRUE);
	if (ishaxor && !rn2(5)) create_mplayers(rn1(4, 3), TRUE);
	if (ishaxor && !rn2(5)) create_umplayers(rn1(2, 1), TRUE);

	/* create a guardian angel next to player, if worthy */
	if (Conflict) {
	    pline(
	     "A voice booms: \"Thy desire for conflict shall be fulfilled!\"");
	    for (i = rnd(4); i > 0; --i) {
		mm.x = u.ux;
		mm.y = u.uy;
		if(enexto(&mm, mm.x, mm.y, &mons[PM_ANGEL]))
		    (void) mk_roamer(&mons[PM_ANGEL], u.ualign.type,
				     mm.x, mm.y, FALSE);
	    }
	} else if (u.ualign.record > 8) {	/* fervent */
	    if (FunnyHallu) pline("You hear Amy say \"Bundlebundlebundle!\""); /* thanks FlamingGuacamole */
	    else pline("A voice whispers: \"Thou hast been worthy of me!\"");
	    mm.x = u.ux;
	    mm.y = u.uy;
	    if (enexto(&mm, mm.x, mm.y, &mons[PM_ANGEL])) {
		if ((mtmp = mk_roamer(&mons[PM_ANGEL], u.ualign.type,
				      mm.x, mm.y, TRUE)) != 0) {
		    if (!Blind)
			pline("An angel appears near you.");
		    else
			You_feel("the presence of a friendly angel near you.");
		    /* guardian angel -- the one case mtame doesn't
		     * imply an edog structure, so we don't want to
		     * call tamedog().
		     */
		    mtmp->mtame = 10;
		    /* make him strong enough vs. endgame foes */
		    mtmp->m_lev = rn1(8,15);
		    mtmp->mhp = mtmp->mhpmax =
					d((int)mtmp->m_lev,10) + 30 + rnd(30);
		    if ((otmp = select_hwep(mtmp)) == 0) {
			otmp = mksobj(SILVER_SABER, FALSE, FALSE, FALSE);
			if (otmp) {
				if (mpickobj(mtmp, otmp, TRUE))
				    panic("merged weapon?");
			}
		    }
		    if (otmp) {
			    bless(otmp);
			    if (otmp->spe < 4) otmp->spe += rnd(4);
			    if ((otmp = which_armor(mtmp, W_ARMS)) == 0 ||
				    otmp->otyp != SHIELD_OF_REFLECTION) {
				(void) mongets(mtmp, AMULET_OF_REFLECTION);
				m_dowear(mtmp, TRUE);
			    }
		    }
		}
	    }
	}
}

static char *dfr_pre_msg = 0,	/* pline() before level change */
	    *dfr_post_msg = 0;	/* pline() after level change */

static boolean portaldeferring = FALSE;

/* change levels at the end of this turn, after monsters finish moving */
void
schedule_goto(tolev, at_stairs, falling, portal_flag, pre_msg, post_msg)
d_level *tolev;
boolean at_stairs, falling;
int portal_flag;
const char *pre_msg, *post_msg;
{
	int typmask = 0100;		/* non-zero triggers `deferred_goto' */

	/* destination flags (`goto_level' args) */
	if (at_stairs)	 typmask |= 1;
	if (falling)	 typmask |= 2;
	if (portal_flag) typmask |= 4;
	if (portal_flag < 0) typmask |= 0200;	/* flag for portal removal */
	u.utotype = typmask;
	/* destination level */
	assign_level(&u.utolev, tolev);

	if (pre_msg)
	    dfr_pre_msg = strcpy((char *)alloc(strlen(pre_msg) + 1), pre_msg);
	if (post_msg)
	    dfr_post_msg = strcpy((char *)alloc(strlen(post_msg)+1), post_msg);

	if (portal_flag && !program_state.gameover && (!rn2(isfriday ? 20 : 50) || StairsProblem || u.uprops[STAIRSTRAP].extrinsic || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || have_stairstrapstone()) ) portaldeferring = TRUE; 

}

/* handle something like portal ejection */
void
deferred_goto()
{
	if (!on_level(&u.uz, &u.utolev)) {
	    d_level dest;
	    int typmask = u.utotype; /* save it; goto_level zeroes u.utotype */

	    assign_level(&dest, &u.utolev);
	    if (dfr_pre_msg) pline("%s", dfr_pre_msg);
	    goto_level(&dest, !!(typmask&1), !!(typmask&2), !!(typmask&4));
	    if (typmask & 0200) {	/* remove portal */
		struct trap *t = t_at(u.ux, u.uy);

		if (t) {
		    deltrap(t);
		    newsym(u.ux, u.uy);
		}
	    }
	    if (dfr_post_msg) pline("%s", dfr_post_msg);
	}
	u.utotype = 0;		/* our caller keys off of this */
	if (dfr_pre_msg)
	    free((void *)dfr_pre_msg),  dfr_pre_msg = 0;
	if (dfr_post_msg)
	    free((void *)dfr_post_msg),  dfr_post_msg = 0;

	if (portaldeferring == TRUE && !program_state.gameover) {

		pline(FunnyHallu ? "Things open up on the flipside!" : "The portal radiates strange energy, and monsters appear from nowhere!");
		pushplayer(TRUE);
		(void)nasty((struct monst *)0);
		u.stairscumslowing += rn1(5,5);
		portaldeferring = FALSE;
	}
}

#endif /* OVL2 */
#ifdef OVL3

/*
 * Return TRUE if we created a monster for the corpse.  If successful, the
 * corpse is gone.
 */
boolean
revive_corpse(corpse, moldy)
struct obj *corpse;
boolean moldy;
{
    struct monst *mtmp, *mcarry;
    boolean is_uwep, chewed;
    xchar where;
    char *cname, cname_buf[BUFSZ];
    struct obj *container = (struct obj *)0;
    int container_where = 0;
    
    where = corpse->where;
    is_uwep = corpse == uwep;
    cname = eos(strcpy(cname_buf, "bite-covered "));
    strcpy(cname, corpse_xname(corpse, TRUE));
    mcarry = (where == OBJ_MINVENT) ? corpse->ocarry : 0;

    if (where == OBJ_CONTAINED) {
    	struct monst *mtmp2 = (struct monst *)0;
	container = corpse->ocontainer;
    	mtmp2 = get_container_location(container, &container_where, (int *)0);
	/* container_where is the outermost container's location even if nested */
	if (container_where == OBJ_MINVENT && mtmp2) mcarry = mtmp2;
    }
    mtmp = revive(corpse);      /* corpse is gone if successful && quan == 1 */

    if (mtmp) {
	/*
	 * [ALI] Override revive's HP calculation. The HP that a mold starts
	 * with do not depend on the HP of the monster whose corpse it grew on.
	 */
	if (moldy)
	    mtmp->mhp = mtmp->mhpmax;

	/* if you killed the elder priest, he should always revive with the covetous egotype --Amy */
	if (mtmp->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_]) {
		mtmp->isegotype = 1;
		mtmp->egotype_covetous = 1;
	}

	/* if you endlessly farm riders, your sanity will increase and cause Bad Stuff(TM)... --Amy */
	if (is_rider(mtmp->data) || is_deadlysin(mtmp->data)) {
		if ((long)mvitals[mtmp->mnum].died > 49) increasesanity( ((long)mvitals[mtmp->mnum].died - 45) * 100);
	}

	if (mtmp->data == &mons[PM_MANOCTATOR]) {
		(void) makemon(specialtensmon(333), 0, 0, NO_MM_FLAGS); /* MS_STENCH */
		You("sense the bittersweet odor of perfume in the air...");
		if (Role_if(PM_HUSSY)) pline("It feels quite soothing.");
	}

	chewed = !moldy && (mtmp->mhp < mtmp->mhpmax);
	if (chewed) cname = cname_buf;	/* include "bite-covered" prefix */
	switch (where) {
	    case OBJ_INVENT:
		if (is_uwep) {
		    if (moldy) {
			Your("weapon goes moldy.");
			pline("%s writhes out of your grasp!", Monnam(mtmp));
		    }
		    else
		    pline_The("%s writhes out of your grasp!", cname);
		}
		else
		    You_feel("squirming in your backpack!");
		break;

	    case OBJ_FLOOR:
		if (cansee(mtmp->mx, mtmp->my)) {
		    if (moldy)
			pline("%s grows on a moldy corpse!",
			  Amonnam(mtmp));
		    else
		    pline("%s rises from the dead!", chewed ?
			  Adjmonnam(mtmp, "bite-covered") : Monnam(mtmp));
		}
		break;

	    case OBJ_MINVENT:		/* probably a nymph's */
		if (cansee(mtmp->mx, mtmp->my)) {
		    if (canseemon(mcarry))
			pline("Startled, %s drops %s as it %s!",
			      mon_nam(mcarry), moldy ? "a corpse" : an(cname),
			      moldy ? "goes moldy" : "revives");
		    else
			pline("%s suddenly appears!", chewed ?
			      Adjmonnam(mtmp, "bite-covered") : Monnam(mtmp));
		}
		break;
	   case OBJ_CONTAINED:
	   	if (container_where == OBJ_MINVENT && cansee(mtmp->mx, mtmp->my) &&
		    mcarry && canseemon(mcarry) && container) {
		        char sackname[BUFSZ];
		        sprintf(sackname, "%s %s", s_suffix(mon_nam(mcarry)),
				xname(container)); 
	   		pline("%s writhes out of %s!", Amonnam(mtmp), sackname);
	   	} else if (container_where == OBJ_INVENT && container) {
		        char sackname[BUFSZ];
		        strcpy(sackname, an(xname(container)));
	   		pline("%s %ss out of %s in your pack!",
	   			Blind ? Something : Amonnam(mtmp),
				locomotion(mtmp->data,"writhes"),
	   			sackname);
	   	} else if (container_where == OBJ_FLOOR && container &&
		            cansee(mtmp->mx, mtmp->my)) {
		        char sackname[BUFSZ];
		        strcpy(sackname, an(xname(container)));
			pline("%s escapes from %s!", Amonnam(mtmp), sackname);
		}
		break;
	    default:
		/* we should be able to handle the other cases... */
		impossible("revive_corpse: lost corpse @ %d", where);
		break;
	}
	return TRUE;
    }
    return FALSE;
}

/* Revive the corpse via a timeout. */
/*ARGSUSED*/
void
revive_mon(arg, timeout)
void * arg;
long timeout;
{
#if defined(MAC_MPW)
# pragma unused ( timeout )
#endif
    struct obj *body = (struct obj *) arg;

    /* if we succeed, the corpse is gone, otherwise, rot it away */
    if (!revive_corpse(body, FALSE)) {
	if (is_rider(&mons[body->corpsenm]))
	    You_feel("less hassled.");
	if (is_deadlysin(&mons[body->corpsenm]))
	    You_feel("less sinful.");
	(void) start_timer(250L - (monstermoves-body->age),
					TIMER_OBJECT, ROT_CORPSE, arg);
    }
}

/* Revive the corpse as a mold via a timeout. */
/*ARGSUSED*/
void
moldy_corpse(arg, timeout)
void * arg;
long timeout;
{
    int pmtype, oldtyp, oldquan;
    struct obj *body = (struct obj *) arg;

    /* Turn the corpse into a mold corpse if molds are available */
    oldtyp = body->corpsenm;

    /* Weight towards non-motile fungi.
     */
    if (rn2(20) || issoviet) pmtype = pm_mkclass(S_FUNGUS, 0);
    else if (rn2(2)) pmtype = pm_mkclass(S_JELLY, 0); /*jellies, blobs and puddings should be possible --Amy*/
    else if (rn2(2)) pmtype = pm_mkclass(S_BLOB, 0);
    else pmtype = pm_mkclass(S_PUDDING, 0);

	/* "Corpses no longer mold into blobs/jellies/puddings" - In Soviet Russia, people dislike variety. They will happily revert all of Slash'EM Extended's changes that made the game more varied, because they apparently have the "resist boredom" intrinsic. --Amy */

    if ((pmtype != -1) && (!is_nonmoving(&mons[pmtype]) ) ) 
	{
	    if (rn2(20) || issoviet) pmtype = pm_mkclass(S_FUNGUS, 0);
	    else if (rn2(2)) pmtype = pm_mkclass(S_JELLY, 0); /*jellies, blobs and puddings should be possible --Amy*/
	    else if (rn2(2)) pmtype = pm_mkclass(S_BLOB, 0);
	    else pmtype = pm_mkclass(S_PUDDING, 0);
	}
    /* [ALI] Molds don't grow in adverse conditions.  If it ever
     * becomes possible for molds to grow in containers we should
     * check for iceboxes here as well.
     */
    if ((body->where == OBJ_FLOOR || body->where==OBJ_BURIED) &&
      (is_waterypool(body->ox, body->oy) || is_watertunnel(body->ox, body->oy) || is_lava(body->ox, body->oy) ||
      is_ice(body->ox, body->oy)))
	pmtype = -1;

    if (pmtype != -1) {
	/* We don't want special case revivals */
	if (cant_create(&pmtype, TRUE) || (body->oxlth &&
				(body->oattached == OATTACHED_MONST)))
	    pmtype = -1; /* cantcreate might have changed it so change it back */
    	else {
	    	body->corpsenm = pmtype;

		/* oeaten isn't used for hp calc here, and zeroing it 
		 * prevents eaten_stat() from worrying when you've eaten more
		 * from the corpse than the newly grown mold's nutrition
		 * value.
		 */
		body->oeaten = 0;

		/* [ALI] If we allow revive_corpse() to get rid of revived
		 * corpses from hero's inventory then we run into problems
		 * with unpaid corpses.
		 */
		if (body->where == OBJ_INVENT)
		    body->quan++;
		oldquan = body->quan;
	    	if (revive_corpse(body, TRUE)) {
		    if (oldquan != 1) {		/* Corpse still valid */
			body->corpsenm = oldtyp;
			if (body->where == OBJ_INVENT) {
			    useup(body);
			    oldquan--;
			}
		    }
		    if (oldquan == 1)
			body = (struct obj *)0;	/* Corpse gone */
		}
    	}
    }
    
    /* If revive_corpse succeeds, it handles the reviving corpse.
     * If there was more than one corpse, or the revive failed,
     * set the remaining corpse(s) to rot away normally.
     * Revive_corpse handles genocides
     */
    if (body) {
    	body->corpsenm = oldtyp; /* Fixup corpse after (attempted) revival */
	body->owt = weight(body);
	(void) start_timer(250L - (monstermoves-peek_at_iced_corpse_age(body)),
					TIMER_OBJECT, ROT_CORPSE, arg);
    }
}

int
donull()
{
	return(1);	/* Do nothing, but let other things happen */
}

#endif /* OVL3 */
#ifdef OVLB

STATIC_PTR int
wipeoff()
{
	if(u.ucreamed < 4)	u.ucreamed = 0;
	else			u.ucreamed -= 4;
	if (Blinded < 4)	Blinded = 0;
	else			Blinded -= 4;
	if (!Blinded) {
		pline("You've got the glop off.");
		u.cnd_wipecount++;
		u.ucreamed = 0;
		Blinded = 1;
		make_blinded(0L,TRUE);
		return(0);
	} else if (!u.ucreamed) {
		Your("%s feels clean now.", body_part(FACE));
		u.cnd_wipecount++;
		return(0);
	}
	return(1);		/* still busy */
}

int
dowipe()
{

	if (MenuIsBugged) {
	pline("The wipe command is currently unavailable!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

	if(u.ucreamed)  {
		static NEARDATA char buf[39];

		sprintf(buf, "wiping off your %s", body_part(FACE));
		set_occupation(wipeoff, buf, 0);
		/* Not totally correct; what if they change back after now
		 * but before they're finished wiping?
		 */
		return(1);
	}
	Your("%s is already clean.", body_part(FACE));
	return(1);
}

void
set_wounded_legs(side, timex)
register long side;
register int timex;
{
	/* KMH -- STEED
	 * If you are riding, your steed gets the wounded legs instead.
	 * You still call this function, but don't lose hp.
	 * Caller is also responsible for adjusting messages.
	 */

	if(!Wounded_legs) {
		ATEMP(A_DEX)--;
		flags.botl = 1;
	}

	/* KMH, intrinsics patch */

	/*if(!Wounded_legs || (HWounded_legs & TIMEOUT))*/
	/* This was interacting incorrectly with items that give wounded legs extrinsically, probably because the
	 * original devteam never expected someone to actually make such items... --Amy */

	HWounded_legs = timex;
	EWounded_legs = side;
	(void)encumber_msg();
}

void
heal_legs()
{
	if(Wounded_legs) {
		if (ATEMP(A_DEX) < 0) {
			ATEMP(A_DEX)++;
			flags.botl = 1;
		}

		if (!u.usteed)
		{
			/* KMH, intrinsics patch */
			if((EWounded_legs & BOTH_SIDES) == BOTH_SIDES) {
			Your("%s feel somewhat better.",
				makeplural(body_part(LEG)));
		} else {
			Your("%s feels somewhat better.",
				body_part(LEG));
		}
		}
		HWounded_legs = EWounded_legs = 0;
	}
	(void)encumber_msg();
}

#endif /* OVLB */

/*do.c*/
