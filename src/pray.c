/*	SCCS Id: @(#)pray.c	3.4	2003/03/23	*/
/* Copyright (c) Benson I. Margulies, Mike Stephenson, Steve Linhart, 1989. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "epri.h"
#include "qtext.h"

/*STATIC_PTR int prayer_done(void);*/
STATIC_DCL struct obj *worst_cursed_item(void);
STATIC_DCL int in_trouble(void);
STATIC_DCL void fix_worst_trouble(int);
/*STATIC_DCL void angrygods(ALIGNTYP_P);*/
STATIC_DCL void at_your_feet(const char *);
STATIC_DCL void gcrownu(void);
STATIC_DCL void pleased(ALIGNTYP_P);
/*STATIC_DCL void godvoice(ALIGNTYP_P,const char*);*/
/*STATIC_DCL void god_zaps_you(ALIGNTYP_P);*/
STATIC_DCL void god_summons_minions(ALIGNTYP_P);
STATIC_DCL void fry_by_god(ALIGNTYP_P);
STATIC_DCL void gods_angry(ALIGNTYP_P);
STATIC_DCL void gods_upset(ALIGNTYP_P);
STATIC_DCL void consume_offering(struct obj *);
STATIC_DCL boolean water_prayer(BOOLEAN_P);
STATIC_DCL boolean blocked_boulder(int,int);
static void lawful_god_gives_angel(void);
static void god_gives_pet(ALIGNTYP_P);
static int offer_oracle(struct monst *, struct obj *);
static void god_gives_benefit(ALIGNTYP_P);
STATIC_DCL boolean canofferownrace(void);

/* simplify a few tests */
#define Cursed_obj(obj,typ) ((obj) && (obj)->otyp == (typ) && (obj)->cursed)

/*
 * Logic behind deities and altars and such:
 * + prayers are made to your god if not on an altar, and to the altar's god
 *   if you are on an altar
 * + If possible, your god answers all prayers, which is why bad things happen
 *   if you try to pray on another god's altar
 * + sacrifices work basically the same way, but the other god may decide to
 *   accept your allegiance, after which they are your god.  If rejected,
 *   your god takes over with your punishment.
 * + if you're in Gehennom, all messages come from Moloch
 */

/*
 *	Moloch, who dwells in Gehennom, is the "renegade" cruel god
 *	responsible for the theft of the Amulet from Marduk, the Creator.
 *	Moloch is unaligned.
 */
static const char	*Moloch = "Moloch";
static const char	*ScholarMoloch = "_Anna";

static const char *godvoices[] = {
    "booms out",
    "thunders",
    "rings out",
    "booms",
};

/* values calculated when prayer starts, and used when completed */
static aligntyp p_aligntyp;
static int p_trouble;
static int p_type; /* (-1)-3: (-1)=really naughty, 3=really good */

#define PIOUS 20
#define DEVOUT 14
#define FERVENT 9
#define STRIDENT 4

/*
 * The actual trouble priority is determined by the order of the
 * checks performed in in_trouble() rather than by these numeric
 * values, so keep that code and these values synchronized in
 * order to have the values be meaningful.
 */

#define TROUBLE_CURSED_SYMBIOTE	15
#define TROUBLE_STONED			14
#define TROUBLE_SLIMED			13
#define TROUBLE_STRANGLED		12
#define TROUBLE_LAVA			11
#define TROUBLE_SICK			10
#define TROUBLE_STARVING		 9
#define TROUBLE_HIT			 8
#define TROUBLE_LYCANTHROPE		 7
#define TROUBLE_LOADSTONE		 6
#define TROUBLE_COLLAPSING		 5
#define TROUBLE_STUCK_IN_WALL		 4
#define TROUBLE_CURSED_LEVITATION	 3
#define TROUBLE_UNUSEABLE_HANDS		 2
#define TROUBLE_CURSED_BLINDFOLD	 1

#define TROUBLE_PUNISHED		(-1)
#define TROUBLE_FUMBLING		(-2)
#define TROUBLE_CURSED_ITEMS		(-3)
#define TROUBLE_SADDLE			(-4)
#define TROUBLE_BLIND			(-5)
#define TROUBLE_POISONED		(-6)
#define TROUBLE_WOUNDED_LEGS		(-7)
#define TROUBLE_HUNGRY			(-8)
#define TROUBLE_STUNNED			(-9)
#define TROUBLE_CONFUSED		(-10)
#define TROUBLE_HALLUCINATION		(-11)
#define TROUBLE_NUMBED		(-12)
#define TROUBLE_FROZEN		(-13)
#define TROUBLE_BURNED		(-14)
#define TROUBLE_FEARED		(-15)
#define TROUBLE_DIMMED		(-16)
#define TROUBLE_BLEEDING		(-17)
#define TROUBLE_LOW_ENERGY		(-18)

/* We could force rehumanize of polyselfed people, but we can't tell
   unintentional shape changes from the other kind. Oh well.
   3.4.2: make an exception if polymorphed into a form which lacks
   hands; that's a case where the ramifications override this doubt.
 */

/* Return 0 if nothing particular seems wrong, positive numbers for
   serious trouble, and negative numbers for comparative annoyances. This
   returns the worst problem. There may be others, and the gods may fix
   more than one.

This could get as bizarre as noting surrounding opponents, (or hostile dogs),
but that's really hard.
 */

#define ugod_is_angry() (u.ualign.record < 0)
#define on_altar()	IS_ALTAR(levl[u.ux][u.uy].typ)
#define on_shrine()	((levl[u.ux][u.uy].altarmask & AM_SHRINE) != 0)
#define a_align(x,y)	((aligntyp)Amask2align(levl[x][y].altarmask & AM_MASK))

STATIC_OVL int
in_trouble()
{
	struct obj *otmp;
	int i, j, count=0;

/* Borrowed from eat.c */

#define SATIATED	0
#define NOT_HUNGRY	1
#define HUNGRY		2
#define WEAK		3
#define FAINTING	4
#define FAINTED		5
#define STARVED		6

	/*
	 * major troubles
	 */
	if(Stoned) return(TROUBLE_STONED);
	if(Slimed) return(TROUBLE_SLIMED);
	if(Strangled) return(TROUBLE_STRANGLED);
	if(u.utrap && u.utraptype == TT_LAVA) return(TROUBLE_LAVA);
	if(Sick) return(TROUBLE_SICK);
	if(u.uhs >= WEAK) return(TROUBLE_STARVING);
	if (Upolyd ? (u.mh <= 5 || u.mh*7 <= u.mhmax) :
		(u.uhp <= 5 || u.uhp*7 <= u.uhpmax)) return(TROUBLE_HIT);
	if(u.ulycn >= LOW_PM && !Race_if(PM_HUMAN_WEREWOLF) && !Role_if(PM_LUNATIC) && !Race_if(PM_AK_THIEF_IS_DEAD_) )
		return(TROUBLE_LYCANTHROPE);

	if(near_capacity() >= HVY_ENCUMBER && have_loadstone() )
		return(TROUBLE_LOADSTONE);

	if(near_capacity() >= HVY_ENCUMBER && have_loadboulder() )
		return(TROUBLE_LOADSTONE);

	if(near_capacity() >= HVY_ENCUMBER && have_starlightstone() )
		return(TROUBLE_LOADSTONE);

	if(near_capacity() >= EXT_ENCUMBER && AMAX(A_STR)-ABASE(A_STR) > 3)
		return(TROUBLE_COLLAPSING);


	for (i= -1; i<=1; i++) for(j= -1; j<=1; j++) {
		if (!i && !j) continue; /* thanks Patric Mueller for this fix. It is implemented in vanilla but not in SLASH'EM for some reason, so I'll add it here. --Amy */
		if (!isok(u.ux+i, u.uy+j) || IS_ROCK(levl[u.ux+i][u.uy+j].typ)
		    || (blocked_boulder(i,j) && !throws_rocks(youmonst.data)))
			count++;
	}
	if (count /*=*/>= 8 && !Passes_walls) /* bugfix, no longer necessary (see above) but I'll keep it anyway --Amy */
		return(TROUBLE_STUCK_IN_WALL);

	if (Cursed_obj(uarmf, LEVITATION_BOOTS) ||
		stuck_ring(uleft, RIN_LEVITATION) ||
		stuck_ring(uright, RIN_LEVITATION))
		return(TROUBLE_CURSED_LEVITATION);
	if (!Race_if(PM_UNGENOMOLD) && ((nohands(youmonst.data) && !Race_if(PM_TRANSFORMER)) || !freehand()) ) {
	    /* for bag/box access [cf use_container()]...
	       make sure it's a case that we know how to handle;
	       otherwise "fix all troubles" would get stuck in a loop */
	    if (welded(uwep) && !(uimplant && uimplant->oartifact == ART_HO_YOO_YOYO) ) return TROUBLE_UNUSEABLE_HANDS;
	    if (Upolyd && !Race_if(PM_TRANSFORMER) && nohands(youmonst.data) && !(uimplant && uimplant->oartifact == ART_HO_YOO_YOYO) && (!Unchanging ||
		    ((otmp = unchanger()) != 0 && otmp->cursed)))
		return TROUBLE_UNUSEABLE_HANDS;
	}
	if(Blindfolded && ublindf->cursed) return(TROUBLE_CURSED_BLINDFOLD);

	if (uinsymbiosis && u.usymbiote.cursed) return(TROUBLE_CURSED_SYMBIOTE);

	/*
	 * minor troubles
	 */
	if(Punished) return(TROUBLE_PUNISHED);
	if (Cursed_obj(uarmg, GAUNTLETS_OF_FUMBLING) ||
		Cursed_obj(uarmg, CLOAK_OF_FUMBLING) ||
		Cursed_obj(uarmg, AMULET_OF_FUMBLING) ||
		Cursed_obj(uarmf, FUMBLE_BOOTS))
	    return TROUBLE_FUMBLING;
	if (worst_cursed_item()) return TROUBLE_CURSED_ITEMS;
	if (u.usteed) {	/* can't voluntarily dismount from a cursed saddle */
	    otmp = which_armor(u.usteed, W_SADDLE);
	    if (Cursed_obj(otmp, LEATHER_SADDLE)) return TROUBLE_SADDLE;
	    if (Cursed_obj(otmp, INKA_SADDLE)) return TROUBLE_SADDLE;
	}

	if (Blinded > 1 && haseyes(youmonst.data)) return(TROUBLE_BLIND);
	for(i=0; i<A_MAX; i++)
	    if(ABASE(i) < AMAX(i)) return(TROUBLE_POISONED);
	if(Wounded_legs && !u.usteed) return (TROUBLE_WOUNDED_LEGS);
	if(u.uhs >= HUNGRY) return(TROUBLE_HUNGRY);
	if(HStun) return (TROUBLE_STUNNED);
	if(HConfusion) return (TROUBLE_CONFUSED);
	if(Hallucination) return(TROUBLE_HALLUCINATION);
	if(HNumbed) return (TROUBLE_NUMBED);
	if(HFrozen) return (TROUBLE_FROZEN);
	if(HBurned) return (TROUBLE_BURNED);
	if(HDimmed) return (TROUBLE_DIMMED);
	if(PlayerBleeds) return (TROUBLE_BLEEDING);
	if(HFeared) return (TROUBLE_FEARED);
        if((u.uen <= 5 || u.uen*7 <= u.uenmax) && (u.uen < u.uenmax)) 
        	return(TROUBLE_LOW_ENERGY);
	return(0);
}

STATIC_OVL boolean
canofferownrace()
{
	if (RngeCannibalism) return TRUE;

	switch (Race_switch) {

		case PM_ALIEN:
		case PM_CURSER:
		case PM_GASTLY:
		case PM_GIGANT:
		case PM_RODNEYAN:
		case PM_INSECTOID:
		case PM_WEAPON_TRAPPER:
		case PM_KOBOLT:
		case PM_MOULD:
		case PM_HUMANLIKE_DRAGON:
		case PM_HUMANLIKE_NAGA:
		case PM_MISSINGNO:
		case PM_WORM_THAT_WALKS:
		case PM_OGRO:
		case PM_DEATHMOLD:
		case PM_AQUATIC_MONSTER:
		case PM_TROLLOR:
		case PM_SHOE:
		case PM_PLAYER_SALAMANDER:
		case PM_VORTEX:
		case PM_METAL:
		case PM_SHELL:
		case PM_CORTEX:
		case PM_GAVIL:
		case PM_PLAYER_ANDROID:
		case PM_RETICULAN:
		case PM_OUTSIDER:
		case PM_HUMANOID_DEVIL:
		case PM_MUMMY:
		case PM_LICH_WARRIOR:
		case PM_UNGENOMOLD:
		case PM_PLAYER_ZRUTY:
		case PM_PLAYER_GOLEM:
		case PM_PLAYER_MUSHROOM:
		case PM_PLAYER_ASURA:
		case PM_PIERCER:
		case PM_PLAYER_HULK:
		case PM_PLAYER_JABBERWOCK:
		case PM_WARPER:
		case PM_LEVITATOR:
		case PM_PHANTOM_GHOST:
		case PM_UNALIGNMENT_THING:
		case PM_ARMED_COCKATRICE:
		case PM_ELEMENTAL:
		case PM_WEAPON_BUG:
		case PM_HUMANOID_LEPRECHAUN:
		case PM_NYMPH:
		case PM_TURTLE:
		case PM_LOWER_ENT:
		case PM_SPRIGGAN:
		case PM_JELLY:
		case PM_WEAPON_CUBE:
		case PM_WEAPON_IMP:
		case PM_HUMANOID_DRYAD:
		case PM_PLAYER_SLIME:
		case PM_BORG:
		case PM_AUREAL:
		case PM_MAZKE:
		case PM_ELONA_SNAIL:
		case PM_ANCIPITAL:
		case PM_PLAYER_DOLGSMAN:
		case PM_YEEK:
		case PM_PLAYER_GREMLIN:
		case PM_FAWN:
		case PM_CHIROPTERAN:
		case PM_YUKI_PLAYA:
		case PM_INKA:
		case PM_OCTOPODE:
		case PM_WEAPON_XORN:
		case PM_WISP:
		case PM_WEAPONIZED_DINOSAUR:
		case PM_SATRE:
			return TRUE;
	}

	return FALSE;

}

/* select an item for TROUBLE_CURSED_ITEMS */
STATIC_OVL struct obj *
worst_cursed_item()
{
    register struct obj *otmp;

    /* if strained or worse, check for loadstone first */
    if (near_capacity() >= HVY_ENCUMBER) {
	for (otmp = invent; otmp; otmp = otmp->nobj)
	    if (Cursed_obj(otmp, LOADBOULDER)) return otmp;
	    if (Cursed_obj(otmp, STARLIGHTSTONE)) return otmp;
	    if (Cursed_obj(otmp, LOADSTONE)) return otmp;
    }
    /* weapon takes precedence if it is interfering
       with taking off a ring or putting on a shield */
    if (welded(uwep) && (uright || bimanual(uwep))) {	/* weapon */
	otmp = uwep;
    /* gloves come next, due to rings */
    } else if (uarmg && uarmg->cursed) {		/* gloves */
	otmp = uarmg;
    /* then shield due to two handed weapons and spells */
    } else if (uarms && uarms->cursed) {		/* shield */
	otmp = uarms;
    /* then cloak due to body armor */
    } else if (uarmc && uarmc->cursed) {		/* cloak */
	otmp = uarmc;
    } else if (uarm && uarm->cursed) {			/* suit */
	otmp = uarm;
    } else if (uarmh && uarmh->cursed) {		/* helmet */
	otmp = uarmh;
    } else if (uarmf && uarmf->cursed) {		/* boots */
	otmp = uarmf;
    } else if (uarmu && uarmu->cursed) {		/* shirt */
	otmp = uarmu;
    } else if (uamul && uamul->cursed) {		/* amulet */
	otmp = uamul;
    } else if (uimplant && uimplant->cursed) {		/* implant */
	otmp = uimplant;
    } else if (uleft && uleft->cursed) {		/* left ring */
	otmp = uleft;
    } else if (uright && uright->cursed) {		/* right ring */
	otmp = uright;
    } else if (ublindf && ublindf->cursed) {		/* eyewear */
	otmp = ublindf;	/* must be non-blinding lenses */
    /* if weapon wasn't handled above, do it now */
    } else if (welded(uwep)) {				/* weapon */
	otmp = uwep;
    /* active secondary weapon even though it isn't welded */
    } else if (uswapwep && uswapwep->cursed && u.twoweap) {
	otmp = uswapwep;
    /* all worn items ought to be handled by now */
    } else {
	for (otmp = invent; otmp; otmp = otmp->nobj) {
	    if (!otmp->cursed) continue;
	    if (otmp->otyp == LOADSTONE || otmp->otyp == LOADBOULDER || otmp->otyp == STARLIGHTSTONE || otmp->otyp==HEALTHSTONE ||
		    confers_luck(otmp))
		break;
	}
    }
    return otmp;
}

STATIC_OVL void
fix_worst_trouble(trouble)
register int trouble;
{
	int i;
	struct obj *otmp = 0;
	const char *what = (const char *)0;
	static NEARDATA const char leftglow[] = "left ring softly glows",
				   rightglow[] = "right ring softly glows";

	switch (trouble) {
	    case TROUBLE_CURSED_SYMBIOTE:
		    Your(FunnyHallu ? "body feels like you were having an orgasm." : "symbiote feels better.");
		    uncursesymbiote(FALSE);
		    break;
	    case TROUBLE_STONED:
		    You_feel("more limber.");
		    Stoned = 0;
		    flags.botl = 1;
		    delayed_killer = 0;
		    break;
	    case TROUBLE_SLIMED:
		    FunnyHallu ? pline("The rancid goo is gone! Yay!") : pline_The("slime disappears.");
		    Slimed = 0;
		    flags.botl = 1;
		    delayed_killer = 0;
		    break;
	    case TROUBLE_STRANGLED:
		    if (uamul && uamul->otyp == AMULET_OF_STRANGULATION) {
			Your("amulet vanishes!");
			useup(uamul);
		    }
		    You("can breathe again.");
		    Strangled = 0;
		    flags.botl = 1;
		    break;
	    case TROUBLE_LAVA:
		    You("are back on solid ground.");
		    /* teleport should always succeed, but if not,
		     * just untrap them.
		     */
		    if(!safe_teleds(FALSE))
			u.utrap = 0;
		    break;
	    case TROUBLE_STARVING:
			if (!u.weakcheat) losestr(-1, TRUE); /* otherwise this could be exploited until you have 25 str --Amy */
			else u.weakcheat--;
		    /* fall into... */
	    case TROUBLE_HUNGRY:
		    Your("%s feels content.", body_part(STOMACH));
		    init_uhunger ();
		    flags.botl = 1;
		    break;
	    case TROUBLE_SICK:
		    You_feel("better.");
		    make_sick(0L, (char *) 0, FALSE, SICK_ALL);
		    break;
	    case TROUBLE_HIT:
		    /* "fix all troubles" will keep trying if hero has
		       5 or less hit points, so make sure they're always
		       boosted to be more than that */
		    You_feel("much better.");
		    if (Upolyd) {
			u.mhmax += rnd(5);
			if (u.mhmax <= 5) u.mhmax = 5+1;
			u.mh = u.mhmax;
		    }
		    if (u.uhpmax < u.ulevel * 5 + 11) u.uhpmax += rnd(5);
		    if (u.uhpmax <= 5) u.uhpmax = 5+1;
		    u.uhp = u.uhpmax;
		    flags.botl = 1;
		    break;
	    case TROUBLE_COLLAPSING:
		    ABASE(A_STR) = AMAX(A_STR);
		    flags.botl = 1;
		    break;
	    case TROUBLE_STUCK_IN_WALL:
		    Your("surroundings change.");
		    /* no control, but works on no-teleport levels */
		    (void) safe_teleds(FALSE);
		    break;
	    case TROUBLE_CURSED_LEVITATION:
		    if (Cursed_obj(uarmf, LEVITATION_BOOTS)) {
			otmp = uarmf;
		    } else if ((otmp = stuck_ring(uleft,RIN_LEVITATION)) !=0) {
			if (otmp == uleft) what = leftglow;
		    } else if ((otmp = stuck_ring(uright,RIN_LEVITATION))!=0) {
			if (otmp == uright) what = rightglow;
		    }
		    goto decurse;
	    case TROUBLE_UNUSEABLE_HANDS:
		    if (welded(uwep)) {
			otmp = uwep;
			goto decurse;
		    }
		    if (Upolyd && nohands(youmonst.data)) {
			if (!Unchanging) {
			    Your("shape becomes uncertain.");
			    rehumanize();  /* "You return to {normal} form." */
			} else if ((otmp = unchanger()) != 0 && otmp->cursed) {
			    /* otmp is an amulet of unchanging */
			    goto decurse;
			}
		    }
		    if (nohands(youmonst.data) || !freehand())
			impossible("fix_worst_trouble: couldn't cure hands.");
		    break;
	    case TROUBLE_CURSED_BLINDFOLD:
		    otmp = ublindf;
		    goto decurse;
	    case TROUBLE_LYCANTHROPE:
		    you_unwere(TRUE);
		    break;
	/*
	 */
	    case TROUBLE_PUNISHED:
		    Your("chain disappears.");
		    unpunish();
		    break;
	    case TROUBLE_FUMBLING:
		    if (Cursed_obj(uarmg, GAUNTLETS_OF_FUMBLING))
			otmp = uarmg;
		    else if (Cursed_obj(uarmf, FUMBLE_BOOTS))
			otmp = uarmf;
		    else if (Cursed_obj(uarmc, CLOAK_OF_FUMBLING))
			otmp = uarmc;
		    else if (Cursed_obj(uamul, AMULET_OF_FUMBLING))
			otmp = uamul;
		    goto decurse;
		    /*NOTREACHED*/
		    break;
	    case TROUBLE_CURSED_ITEMS:
	    case TROUBLE_LOADSTONE:
		    otmp = worst_cursed_item();
		    if (otmp == uright) what = rightglow;
		    else if (otmp == uleft) what = leftglow;
decurse:
		    if (!otmp) {
			impossible("fix_worst_trouble: nothing to uncurse.");
			return;
		    }
		    if (!stack_too_big(otmp)) uncurse(otmp, TRUE);
		    if (!Blind) {
			Your("%s %s.", what ? what :
				(const char *)aobjnam(otmp, "softly glow"),
			     hcolor(NH_AMBER));
			otmp->bknown = TRUE;
		    }
		    update_inventory();
		    break;
	    case TROUBLE_POISONED:
		    if (FunnyHallu)
			pline("There's a tiger in your tank.");
		    else
			You_feel("in good health again.");
		    for(i=0; i<A_MAX; i++) {
			if(ABASE(i) < AMAX(i)) {
				ABASE(i) = AMAX(i);
				flags.botl = 1;
			}
		    }
		    (void) encumber_msg();
		    break;
	    case TROUBLE_BLIND:
	    	    {
	    	    	int num_eyes = eyecount(youmonst.data);
		    const char *eye = body_part(EYE);

			Your("%s feel%s better.",
			 (num_eyes == 1) ? eye : makeplural(eye),
			     (num_eyes == 1) ? "s" : "");
		    u.ucreamed = 0;
			make_blinded(0L,FALSE);
			break;
		    }
	    case TROUBLE_WOUNDED_LEGS:
		    heal_legs();
		    break;
	    case TROUBLE_STUNNED:
		    make_stunned(0L,TRUE);
		    break;
	    case TROUBLE_CONFUSED:
		    make_confused(0L,TRUE);
		    break;
	    case TROUBLE_HALLUCINATION:
		    pline ("Looks like you are back in Kansas.");
		    (void) make_hallucinated(0L,FALSE,0L);
		    break;
	    case TROUBLE_NUMBED:
		    make_numbed(0L,TRUE);
		    break;
	    case TROUBLE_FROZEN:
		    make_frozen(0L,TRUE);
		    break;
	    case TROUBLE_BURNED:
		    make_burned(0L,TRUE);
		    break;
	    case TROUBLE_DIMMED:
		    make_dimmed(0L,TRUE);
		    break;
	    case TROUBLE_BLEEDING:
		    PlayerBleeds = 0;
		    Your("bleeding stops!");
		    break;
	    case TROUBLE_FEARED:
		    make_feared(0L,TRUE);
		    break;
	    case TROUBLE_LOW_ENERGY:
		    You_feel("revitalised.");
		    u.uen = u.uenmax;
		    flags.botl = 1;
		    break;
	    case TROUBLE_SADDLE:
		    otmp = which_armor(u.usteed, W_SADDLE);
		    uncurse(otmp, TRUE);
		    if (!Blind) {
			pline("%s %s %s.",
			      s_suffix(upstart(y_monnam(u.usteed))),
			      aobjnam(otmp, "softly glow"),
			      hcolor(NH_AMBER));
			otmp->bknown = TRUE;
		    }
		    break;
	}
}

/* "I am sometimes shocked by...  the nuns who never take a bath without
 * wearing a bathrobe all the time.  When asked why, since no man can see them,
 * they reply 'Oh, but you forget the good God'.  Apparently they conceive of
 * the Deity as a Peeping Tom, whose omnipotence enables Him to see through
 * bathroom walls, but who is foiled by bathrobes." --Bertrand Russell, 1943
 * Divine wrath, dungeon walls, and armor follow the same principle.
 */
void
god_zaps_you(resp_god)
aligntyp resp_god;
{
	if (u.uswallow) {
	    pline("Suddenly a bolt of lightning comes down at you from the heavens!");
	    pline("It strikes %s!", mon_nam(u.ustuck));
	    if (!resists_elec(u.ustuck)) {
		pline("%s fries to a crisp!", Monnam(u.ustuck));
		/* Yup, you get experience.  It takes guts to successfully
		 * pull off this trick on your god, anyway.
		 */
		xkilled(u.ustuck, 0);
	    } else pline("%s seems unaffected.", Monnam(u.ustuck));
	} else {
	    pline("Suddenly, a bolt of lightning strikes you!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Eto konets ty oblazhalsya cheloveka. Bog lichno reshil, chto on ne lyubit tebya i teper' ty mertv. Mudak." : "DUEUEUEUEUEUE-DLUELUELUELUELUELUELUE!");
	    if (Reflecting) {
		shieldeff(u.ux, u.uy);
		if (Blind) pline("For some reason you're unaffected.");
		else
		    (void) ureflects("%s reflects from your %s.", "It");
	    } else if (Shock_resistance) {
		shieldeff(u.ux, u.uy);
		pline("It seems not to affect you.");
	    } else fry_by_god(resp_god);
	}

	pline("%s is not deterred...", align_gname(resp_god));
	if (u.uswallow) {
	    pline("A wide-angle disintegration beam aimed at you hits %s!",
			mon_nam(u.ustuck));
	    if (!resists_disint(u.ustuck)) {
		pline("%s fries to a crisp!", Monnam(u.ustuck));
		xkilled(u.ustuck, 2); /* no corpse */
	    } else
		pline("%s seems unaffected.", Monnam(u.ustuck));
	} else {
	    pline("A wide-angle disintegration beam hits you!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' UMIRAT' NAKONETS ty glupyy der'movoy chelovek! Vy ne dolzhny sushchestvovat' bol'she, potomu chto vy dazhe ne znayete, kak igrat' slesh ikh!" : "Dwoaing-doaing...");

	    /* disintegrate shield and body armor before disintegrating
	     * the impudent mortal, like black dragon breath -3.
	     */
	    if (uarms && !(EReflecting & W_ARMS) &&
	    		!(EDisint_resistance & W_ARMS))
		(void) destroy_arm(uarms);
	    if (uarmc && !(EReflecting & W_ARMC) &&
	    		!(EDisint_resistance & W_ARMC))
		(void) destroy_arm(uarmc);
	    if (uarm && !(EReflecting & W_ARM) &&
	    		!(EDisint_resistance & W_ARM) && !uarmc)
		(void) destroy_arm(uarm);
	    if (uarmu && !uarm && !uarmc && !(EReflecting & W_ARMU) &&
	    		!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
	    if (!Disint_resistance)
		fry_by_god(resp_god);
	    else {
		You("bask in its %s glow for a minute...", NH_BLACK);
		godvoice(resp_god, rn2(2) ? "You have further angered me!" : "I believe it not!");
	    }
	    if (Is_astralevel(&u.uz) || Is_sanctum(&u.uz)) {
		/* one more try for high altars */
		verbalize("Thou cannot escape my wrath, mortal!");
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);                
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		verbalize("Annihilate %s, my servants!", uhim());
	    } else {
		verbalize("Thou cannot escape my wrath, mortal!");
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		verbalize("Destroy %s, my servants!", uhim());
	    }
	}
}

/* A weaker form of god_zaps_you, this just summons some minions without the instadeath bolts and beams. --Amy */
STATIC_OVL void
god_summons_minions(resp_god)
aligntyp resp_god;
{
	    if (Is_astralevel(&u.uz) || Is_sanctum(&u.uz)) {
		/* one more try for high altars */
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);                
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		verbalize("Go forth, my minions, slay this mortal scum for me!");
	    } else {
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		summon_minion(resp_god, FALSE);
		verbalize("Go forth, my minions, slay this mortal scum for me!");
	    }
}
STATIC_OVL void
fry_by_god(resp_god) /* Amy note: this instakills you even if you have many HP, unlike other disintegration sources */
aligntyp resp_god;
{
	char killerbuf[64];

	u.youaredead = 1;
	You("fry to a crisp.");
	killer_format = KILLED_BY;
	sprintf(killerbuf, "the wrath of a deity (%s)", align_gname(resp_god));
	killer = killerbuf;
	done(DIED);
	u.youaredead = 0;
}

void
angrygods(resp_god)
aligntyp resp_god;
{
	register int	maxanger;

	int copcnt; /* Angry gods will send in the police and sephirah! --Amy */
	u.cnd_kopsummonamount++;
	copcnt = rnd(monster_difficulty() ) + 1;
	if (rn2(5)) copcnt = (copcnt / (rnd(4) + 1)) + 1;

	if (resp_god == A_NEUTRAL) copcnt += 1;
	if (resp_god == A_CHAOTIC) copcnt += (1 + rnd(2));

	if (rn2(5)) copcnt /= 2;
	if (!rn2(5)) copcnt /= 2; /* don't make too many */
	if (!rn2(10)) copcnt /= 3;
	if (copcnt < 1) copcnt = 1;

	if (uarmh && itemhasappearance(uarmh, APP_ANTI_GOVERNMENT_HELMET) ) {
		copcnt = (copcnt / 2) + 1;
	}

	if (RngeAntiGovernment) {
		copcnt = (copcnt / 2) + 1;
	}

	if (Aggravate_monster) {
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
	}

      while(--copcnt >= 0) {
		(void) makemon(mkclass(S_KOP,0), u.ux, u.uy, MM_ANGRY);

		if (!rn2(200)) {

			int koptryct = 0;
			int kox, koy;

			for (koptryct = 0; koptryct < 2000; koptryct++) {
				kox = rn1(COLNO-3,2);
				koy = rn2(ROWNO);

				if (kox && koy && isok(kox, koy) && (levl[kox][koy].typ > DBWALL) && !(t_at(kox, koy)) ) {
					(void) maketrap(kox, koy, KOP_CUBE, 0);
					break;
					}
			}
		}

	} /* while */

	u.aggravation = 0;

	if( (Inhell && !Race_if(PM_HERETIC) ) || flags.gehenna) resp_god = A_NONE;
	u.ublessed = 0;

	/* changed from tmp = u.ugangr + abs (u.uluck) -- rph */
	/* added test for alignment diff -dlc */
	if(resp_god != u.ualign.type) /* negative alignment record shouldn't protect you but make things worse! --Amy */
	    maxanger = (u.ualign.record > 0 ? u.ualign.record/2 : -u.ualign.record) + (Luck > 0 ? -Luck/3 : -Luck);
	else
	    maxanger =  3*u.ugangr +
		((Luck > 0 || u.ualign.record >= STRIDENT) ? -Luck/3 : -Luck);

	/* chaotic gods are harder to anger, but if they are angered, they'll smite you much harder --Amy */
	if (resp_god == A_NEUTRAL) maxanger += 1;
	if (resp_god == A_CHAOTIC) maxanger += 2;

	/* positive alignment record should be useful instead of bad --Amy */
	if (rn2(2) && u.ualign.record >= 5) maxanger -= rnd(u.ualign.record / 5);
	if (maxanger < 1) maxanger = 1; /* possible if bad align & good luck */
	/*else if (maxanger > 15) maxanger = 15;*/ /* be reasonable */ /* edit by Amy - no longer needed, see below */

	switch (rn2(maxanger)) {
	    case 0:
	    case 1:	You_feel("that %s is %s.", align_gname(resp_god),
			    FunnyHallu ? "bummed" : "displeased");
			break;
	    case 2:
	    case 3:
			godvoice(resp_god,(char *)0);
			pline("\"Thou %s, %s.\"",
			    (ugod_is_angry() && resp_god == u.ualign.type)
				? "hast strayed from the path" :
						"art arrogant",
			      youmonst.data->mlet == S_HUMAN ? "mortal" : "creature");
			verbalize("Thou must relearn thy lessons!");
			(void) adjattrib(A_WIS, -3, FALSE, TRUE);
			losexp((char *)0, TRUE, FALSE); /* divine wrath ignores mundane stuff like drain resistance --Amy */
			break;
	    case 6:	if (!Punished) {
			    gods_angry(resp_god);
			    punish((struct obj *)0);
			    break;
			} /* else fall thru */
	    case 4:
	    case 5:	gods_angry(resp_god);
			if (!Blind && !Antimagic)
			    pline("%s glow surrounds you.",
				  An(hcolor(NH_BLACK)));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			rndcurse();
			break;
	    case 7:
	    case 8:	godvoice(resp_god,(char *)0);
			verbalize("Thou durst %s me?",
				  (on_altar() &&
				   (a_align(u.ux,u.uy) != resp_god)) ?
				  "scorn":"call upon");
			pline("\"Then die, %s!\"",
			      youmonst.data->mlet == S_HUMAN ? "mortal" : "creature");
			summon_minion(resp_god, FALSE);
			break;
	    case 9:
	    case 10:	/* new possible outcomes if your god is extraordinarily angry --Amy */
			    godvoice(resp_god,(char *)0);
				verbalize("Thou dost need penance!");
			    punishx();
			    punishx();
			    break;
	    case 11:	
	    case 12:	
			gods_angry(resp_god);
			god_summons_minions(resp_god);
			break;
	    case 13:	
			godvoice(resp_god,(char *)0);
			verbalize("Apres moi, le deluge!");
			if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
				water_damage(invent, FALSE, FALSE);
				if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
			}
			if (Burned) make_burned(0L, TRUE);
			break;
	    case 14:	
			gods_angry(resp_god);
			verbalize("Sayeth goodbye to thine belongings!");
			withering_damage(invent, FALSE, FALSE);
			break;
	    case 15:	
			gods_angry(resp_god);
			verbalize("I curse thee for thine offense!");
		    attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse();
			break;

	    default:	gods_angry(resp_god);
			if (!rn2(evilfriday ? 3 : 5)) god_zaps_you(resp_god); /* lower instadeath chance --Amy */
			else god_summons_minions(resp_god);
			break;
	}
#ifdef NOARTIFACTWISH
	u.usacrifice = 0;
#endif
	u.ublesscnt += rnz(ishaxor ? 150 : 300);
	return;
}

/* helper to print "str appears at your feet", or appropriate */
static void
at_your_feet(str)
	const char *str;
{
	if (Blind) str = Something;
	if (u.uswallow) {
	    /* barrier between you and the floor */
	    pline("%s %s into %s %s.", str, vtense(str, "drop"),
		  s_suffix(mon_nam(u.ustuck)), mbodypart(u.ustuck, STOMACH));
	} else {
	    pline("%s %s %s your %s!", str,
		  Blind ? "lands" : vtense(str, "appear"),
		  Levitation ? "beneath" : "at",
		  makeplural(body_part(FOOT)));
	}
}

STATIC_OVL void
gcrownu()
{
    struct obj *obj;
    boolean already_exists, in_hand;
    short class_gift;
    int sp_no;
#define ok_wep(o) ((o) && ((o)->oclass == WEAPON_CLASS || is_weptool(o)))

    HSee_invisible |= FROMOUTSIDE;
    HFire_resistance |= FROMOUTSIDE;
    HCold_resistance |= FROMOUTSIDE;
    HShock_resistance |= FROMOUTSIDE;
    HSleep_resistance |= FROMOUTSIDE;
    HPoison_resistance |= FROMOUTSIDE;
    u.weapon_slots += 5;
    godvoice(u.ualign.type, (char *)0);

    obj = ok_wep(uwep) ? uwep : 0;
    already_exists = in_hand = FALSE;	/* lint suppression */
	if( Role_if(PM_PIRATE) ){
		u.uevent.uhand_of_elbereth = 2; /* Alignment of P King is treated as neutral */
		in_hand = (uwep && uwep->oartifact == ART_REAVER);
		already_exists = exist_artifact(SCIMITAR, artiname(ART_REAVER));
		verbalize("Hurrah for our Pirate King!");
	}
	else {
    switch (u.ualign.type) {
    case A_LAWFUL:
	u.uevent.uhand_of_elbereth = 1;
	verbalize("I crown thee...  The Hand of Elbereth!");
	break;
    case A_NEUTRAL:
	u.uevent.uhand_of_elbereth = 2;
	in_hand = (uwep && uwep->oartifact == ART_VORPAL_BLADE);
	already_exists = exist_artifact(LONG_SWORD, artiname(ART_VORPAL_BLADE));
	verbalize("Thou shalt be my Envoy of Balance!");
	break;
    case A_CHAOTIC:
	u.uevent.uhand_of_elbereth = 3;
	in_hand = (uwep && uwep->oartifact == ART_STORMBRINGER);
	already_exists = exist_artifact(RUNESWORD, artiname(ART_STORMBRINGER));
	verbalize("Thou art chosen to %s for My Glory!",
		  already_exists && !in_hand ? "take lives" : "steal souls");
	break;
    }
	}

    if (rn2(3) && (!issoviet || !rn2(3))) { 
	/* It's boring if the crowning gift is always the same, because it means the player is likely to use the same
	 * artifacts each game. Yes, random crowning gifts may suck, but this is SLASH'EM Extended, where you're never 
	 * supposed to be guaranteed an awesome item! Randomness and variety ftw! --Amy */
	/* In Soviet Russia, communism dictates that everyone gets the Stormbringer because the type of ice block said so. */

		obj = mk_artifact((struct obj *)0, a_align(u.ux,u.uy), TRUE);
		if (obj) {
		    if (obj->spe < 3) obj->spe = 3;
		    if (!rn2(2)) obj->spe += rnd(3);
		    if (!rn2(10)) obj->spe += rnd(7);
		    if (obj->cursed) uncurse(obj, TRUE);
		    if (!obj->blessed) bless(obj);
		    obj->oerodeproof = TRUE;
		    dropy(obj);
		    at_your_feet("An object");
		    godvoice(u.ualign.type, "Use my gift wisely!");
#ifdef LIVELOGFILE
			char	 reportbuf[BUFSZ];
			sprintf (reportbuf, "received %s by %s", ONAME(obj), u_gname());
			livelog_report_trophy(reportbuf);
#endif
		    u.ugifts++;
		    u.ublesscnt = rnz(300 + (50 * nartifact_exist() ));
			if (ishaxor) u.ublesscnt /= 2;
		    exercise(A_WIS, TRUE);
		    /* make sure we can use this weapon */
			if (P_MAX_SKILL(get_obj_skill(obj, TRUE)) == P_ISRESTRICTED) {
			    unrestrict_weapon_skill(get_obj_skill(obj, TRUE));
			} else if (P_MAX_SKILL(get_obj_skill(obj, TRUE)) == P_UNSKILLED) {
				unrestrict_weapon_skill(get_obj_skill(obj, TRUE));
				P_MAX_SKILL(get_obj_skill(obj, TRUE)) = P_BASIC;
			} else if (rn2(2) && P_MAX_SKILL(get_obj_skill(obj, TRUE)) == P_BASIC) {
				P_MAX_SKILL(get_obj_skill(obj, TRUE)) = P_SKILLED;
			} else if (!rn2(4) && P_MAX_SKILL(get_obj_skill(obj, TRUE)) == P_SKILLED) {
				P_MAX_SKILL(get_obj_skill(obj, TRUE)) = P_EXPERT;
			} else if (!rn2(10) && P_MAX_SKILL(get_obj_skill(obj, TRUE)) == P_EXPERT) {
				P_MAX_SKILL(get_obj_skill(obj, TRUE)) = P_MASTER;
			} else if (!rn2(100) && P_MAX_SKILL(get_obj_skill(obj, TRUE)) == P_MASTER) {
				P_MAX_SKILL(get_obj_skill(obj, TRUE)) = P_GRAND_MASTER;
			} else if (!rn2(200) && P_MAX_SKILL(get_obj_skill(obj, TRUE)) == P_GRAND_MASTER) {
				P_MAX_SKILL(get_obj_skill(obj, TRUE)) = P_SUPREME_MASTER;
			}

		    discover_artifact(obj->oartifact);
		    return;
		}
		pline("But the RNG, being the supreme deity, decided that your coronation/culmination does not actually grant you a gift...");
	    return;
    }

    class_gift = STRANGE_OBJECT;
    /* 3.3.[01] had this in the A_NEUTRAL case below,
       preventing chaotic wizards from receiving a spellbook */
    if (Role_if(PM_WIZARD) &&
	    (!uwep || (uwep->oartifact != ART_VORPAL_BLADE &&
		       uwep->oartifact != ART_STORMBRINGER)) &&
	    !carrying(SPE_FINGER_OF_DEATH)) {
	class_gift = SPE_FINGER_OF_DEATH;
 make_splbk:
	obj = mksobj(class_gift, TRUE, FALSE);
	if (obj) {
		bless(obj);
		obj->bknown = TRUE;
		at_your_feet("A spellbook");
		dropy(obj);
		u.ugifts++;
	}
	/* when getting a new book for known spell, enhance
	   currently wielded weapon rather than the book */
	for (sp_no = 0; sp_no < MAXSPELL; sp_no++)
	    if (spl_book[sp_no].sp_id == class_gift) {
		if (ok_wep(uwep)) obj = uwep;	/* to be blessed,&c */
		break;
	    }
    } else if (Role_if(PM_MONK) &&
	    (!uwep || !uwep->oartifact) &&
	    !carrying(SPE_RESTORE_ABILITY)) {
	/* monks rarely wield a weapon */
	class_gift = SPE_RESTORE_ABILITY;
	goto make_splbk;
    }

	if( Role_if(PM_PIRATE) ){
		if (class_gift != STRANGE_OBJECT) {
			;		/* already got bonus above for some reason */
		} else if (in_hand) {
			Your("%s rings with the sound of waves!", xname(obj));
			obj->dknown = TRUE;
		} else if (!already_exists) {
			obj = mksobj(SCIMITAR, FALSE, FALSE);
			if (obj) {
				obj = oname(obj, artiname(ART_REAVER));
				obj->spe = 1;
				at_your_feet("A sword");
				dropy(obj);
#ifdef LIVELOGFILE
				char	 reportbuf[BUFSZ];
				sprintf (reportbuf, "received %s by %s", ONAME(obj), u_gname());
				livelog_report_trophy(reportbuf);
#endif
				u.ugifts++;
			}
		}
		/* acquire Reaver's skill regardless of weapon or gift, 
			although pirates are already good at using scimitars */
		if (P_MAX_SKILL(P_SCIMITAR) == P_ISRESTRICTED) {
		    unrestrict_weapon_skill(P_SCIMITAR);
		} else if (P_MAX_SKILL(P_SCIMITAR) == P_UNSKILLED) {
			unrestrict_weapon_skill(P_SCIMITAR);
			P_MAX_SKILL(P_SCIMITAR) = P_BASIC;
		} else if (rn2(2) && P_MAX_SKILL(P_SCIMITAR) == P_BASIC) {
			P_MAX_SKILL(P_SCIMITAR) = P_SKILLED;
		} else if (!rn2(4) && P_MAX_SKILL(P_SCIMITAR) == P_SKILLED) {
			P_MAX_SKILL(P_SCIMITAR) = P_EXPERT;
		} else if (!rn2(10) && P_MAX_SKILL(P_SCIMITAR) == P_EXPERT) {
			P_MAX_SKILL(P_SCIMITAR) = P_MASTER;
		} else if (!rn2(100) && P_MAX_SKILL(P_SCIMITAR) == P_MASTER) {
			P_MAX_SKILL(P_SCIMITAR) = P_GRAND_MASTER;
		} else if (!rn2(200) && P_MAX_SKILL(P_SCIMITAR) == P_GRAND_MASTER) {
			P_MAX_SKILL(P_SCIMITAR) = P_SUPREME_MASTER;
		}

		if (obj && obj->oartifact == ART_REAVER)
			discover_artifact(ART_REAVER);
	}
	else {
    switch (u.ualign.type) {
    case A_LAWFUL:
	if (class_gift != STRANGE_OBJECT) {
	    ;		/* already got bonus above */
	} else if (obj && obj->otyp == LONG_SWORD && !obj->oartifact) {
	    if (!Blind) Your("sword shines brightly for a moment.");
	    obj = oname(obj, artiname(ART_EXCALIBUR));
	    if (obj && obj->oartifact == ART_EXCALIBUR) u.ugifts++;
	}
	/* acquire Excalibur's skill regardless of weapon or gift */

	if (P_MAX_SKILL(P_LONG_SWORD) == P_ISRESTRICTED) {
	    unrestrict_weapon_skill(P_LONG_SWORD);
	} else if (P_MAX_SKILL(P_LONG_SWORD) == P_UNSKILLED) {
		unrestrict_weapon_skill(P_LONG_SWORD);
		P_MAX_SKILL(P_LONG_SWORD) = P_BASIC;
	} else if (rn2(2) && P_MAX_SKILL(P_LONG_SWORD) == P_BASIC) {
		P_MAX_SKILL(P_LONG_SWORD) = P_SKILLED;
	} else if (!rn2(4) && P_MAX_SKILL(P_LONG_SWORD) == P_SKILLED) {
		P_MAX_SKILL(P_LONG_SWORD) = P_EXPERT;
	} else if (!rn2(10) && P_MAX_SKILL(P_LONG_SWORD) == P_EXPERT) {
		P_MAX_SKILL(P_LONG_SWORD) = P_MASTER;
	} else if (!rn2(100) && P_MAX_SKILL(P_LONG_SWORD) == P_MASTER) {
		P_MAX_SKILL(P_LONG_SWORD) = P_GRAND_MASTER;
	} else if (!rn2(200) && P_MAX_SKILL(P_LONG_SWORD) == P_GRAND_MASTER) {
		P_MAX_SKILL(P_LONG_SWORD) = P_SUPREME_MASTER;
	}

	if (obj && obj->oartifact == ART_EXCALIBUR)
	    discover_artifact(ART_EXCALIBUR);
	break;
    case A_NEUTRAL:
	if (class_gift != STRANGE_OBJECT) {
	    ;		/* already got bonus above */
	} else if (in_hand) {
	    Your("%s goes snicker-snack!", xname(obj));
	    obj->dknown = TRUE;
	} else if (!already_exists) {
	    obj = mksobj(LONG_SWORD, FALSE, FALSE);
	    if (obj) {
		    obj = oname(obj, artiname(ART_VORPAL_BLADE));
		    obj->spe = 1;
		    at_your_feet("A sword");
		    dropy(obj);
#ifdef LIVELOGFILE
			char	 reportbuf[BUFSZ];
			sprintf (reportbuf, "received %s by %s", ONAME(obj), u_gname());
			livelog_report_trophy(reportbuf);
#endif
		    u.ugifts++;
	    }
	}
	/* acquire Vorpal Blade's skill regardless of weapon or gift */

	if (P_MAX_SKILL(P_LONG_SWORD) == P_ISRESTRICTED) {
	    unrestrict_weapon_skill(P_LONG_SWORD);
	} else if (P_MAX_SKILL(P_LONG_SWORD) == P_UNSKILLED) {
		unrestrict_weapon_skill(P_LONG_SWORD);
		P_MAX_SKILL(P_LONG_SWORD) = P_BASIC;
	} else if (rn2(2) && P_MAX_SKILL(P_LONG_SWORD) == P_BASIC) {
		P_MAX_SKILL(P_LONG_SWORD) = P_SKILLED;
	} else if (!rn2(4) && P_MAX_SKILL(P_LONG_SWORD) == P_SKILLED) {
		P_MAX_SKILL(P_LONG_SWORD) = P_EXPERT;
	} else if (!rn2(10) && P_MAX_SKILL(P_LONG_SWORD) == P_EXPERT) {
		P_MAX_SKILL(P_LONG_SWORD) = P_MASTER;
	} else if (!rn2(100) && P_MAX_SKILL(P_LONG_SWORD) == P_MASTER) {
		P_MAX_SKILL(P_LONG_SWORD) = P_GRAND_MASTER;
	} else if (!rn2(200) && P_MAX_SKILL(P_LONG_SWORD) == P_GRAND_MASTER) {
		P_MAX_SKILL(P_LONG_SWORD) = P_SUPREME_MASTER;
	}

	if (obj && obj->oartifact == ART_VORPAL_BLADE)
	    discover_artifact(ART_VORPAL_BLADE);
	break;
    case A_CHAOTIC:
      {
	char swordbuf[BUFSZ];

	sprintf(swordbuf, "%s sword", hcolor(NH_BLACK));
	if (class_gift != STRANGE_OBJECT) {
	    ;		/* already got bonus above */
	} else if (in_hand) {
	    Your("%s hums ominously!", swordbuf);
	    obj->dknown = TRUE;
	} else if (!already_exists) {
	    obj = mksobj(RUNESWORD, FALSE, FALSE);
	    if (obj) {
		    obj = oname(obj, artiname(ART_STORMBRINGER));
		    at_your_feet(An(swordbuf));
		    obj->spe = 1;
		    dropy(obj);
#ifdef LIVELOGFILE
			char	 reportbuf[BUFSZ];
			sprintf (reportbuf, "received %s by %s", ONAME(obj), u_gname());
			livelog_report_trophy(reportbuf);
#endif
		    u.ugifts++;
	    }
	}
	/* acquire Stormbringer's skill regardless of weapon or gift */

	if (P_MAX_SKILL(P_BROAD_SWORD) == P_ISRESTRICTED) {
	    unrestrict_weapon_skill(P_BROAD_SWORD);
	} else if (P_MAX_SKILL(P_BROAD_SWORD) == P_UNSKILLED) {
		unrestrict_weapon_skill(P_BROAD_SWORD);
		P_MAX_SKILL(P_BROAD_SWORD) = P_BASIC;
	} else if (rn2(2) && P_MAX_SKILL(P_BROAD_SWORD) == P_BASIC) {
		P_MAX_SKILL(P_BROAD_SWORD) = P_SKILLED;
	} else if (!rn2(4) && P_MAX_SKILL(P_BROAD_SWORD) == P_SKILLED) {
		P_MAX_SKILL(P_BROAD_SWORD) = P_EXPERT;
	} else if (!rn2(10) && P_MAX_SKILL(P_BROAD_SWORD) == P_EXPERT) {
		P_MAX_SKILL(P_BROAD_SWORD) = P_MASTER;
	} else if (!rn2(100) && P_MAX_SKILL(P_BROAD_SWORD) == P_MASTER) {
		P_MAX_SKILL(P_BROAD_SWORD) = P_GRAND_MASTER;
	} else if (!rn2(200) && P_MAX_SKILL(P_BROAD_SWORD) == P_GRAND_MASTER) {
		P_MAX_SKILL(P_BROAD_SWORD) = P_SUPREME_MASTER;
	}

	if (obj && obj->oartifact == ART_STORMBRINGER)
	    discover_artifact(ART_STORMBRINGER);
	break;
      }
    default:
	obj = 0;	/* lint */
	break;
    }
	}

    /* enhance weapon regardless of alignment or artifact status */
    if (ok_wep(obj)) {
	bless(obj);
	obj->oeroded = obj->oeroded2 = 0;
	obj->oerodeproof = TRUE;
	obj->bknown = obj->rknown = TRUE;
	/* STEPHEN WHITE'S NEW CODE */
	if (u.ualign.type == A_LAWFUL) {
	    if (obj->spe < 3) obj->spe = 3;
	    else if (obj->spe > 2) obj->spe += 1;
	} else if (obj->spe < 1) obj->spe = 1;
	/* acquire skill in this weapon */

	if (P_MAX_SKILL(get_obj_skill(obj, TRUE)) == P_ISRESTRICTED) {
	    unrestrict_weapon_skill(get_obj_skill(obj, TRUE));
	} else if (P_MAX_SKILL(get_obj_skill(obj, TRUE)) == P_UNSKILLED) {
		unrestrict_weapon_skill(get_obj_skill(obj, TRUE));
		P_MAX_SKILL(get_obj_skill(obj, TRUE)) = P_BASIC;
	} else if (rn2(2) && P_MAX_SKILL(get_obj_skill(obj, TRUE)) == P_BASIC) {
		P_MAX_SKILL(get_obj_skill(obj, TRUE)) = P_SKILLED;
	} else if (!rn2(4) && P_MAX_SKILL(get_obj_skill(obj, TRUE)) == P_SKILLED) {
		P_MAX_SKILL(get_obj_skill(obj, TRUE)) = P_EXPERT;
	} else if (!rn2(10) && P_MAX_SKILL(get_obj_skill(obj, TRUE)) == P_EXPERT) {
		P_MAX_SKILL(get_obj_skill(obj, TRUE)) = P_MASTER;
	} else if (!rn2(100) && P_MAX_SKILL(get_obj_skill(obj, TRUE)) == P_MASTER) {
		P_MAX_SKILL(get_obj_skill(obj, TRUE)) = P_GRAND_MASTER;
	} else if (!rn2(200) && P_MAX_SKILL(get_obj_skill(obj, TRUE)) == P_GRAND_MASTER) {
		P_MAX_SKILL(get_obj_skill(obj, TRUE)) = P_SUPREME_MASTER;
	}

    } else if (class_gift == STRANGE_OBJECT) {
	/* opportunity knocked, but there was nobody home... */
	You_feel("unworthy.");
    }
    update_inventory();
    return;
}

STATIC_OVL void
pleased(g_align)
	aligntyp g_align;
{
	/* don't use p_trouble, worst trouble may get fixed while praying */
	int trouble = in_trouble();	/* what's your worst difficulty? */
	int pat_on_head = 0, kick_on_butt;

	if (Race_if(PM_NEMESIS) && !rn2(4)) { /* aww, you got unlucky... */
		You_feel("that %s is noncommittal.", align_gname(g_align));
		goto setprayertimeout;
	}

	You_feel("that %s is %s.", align_gname(g_align),
	    u.ualign.record >= DEVOUT ?
	    FunnyHallu ? "pleased as punch" : "well-pleased" :
	    u.ualign.record >= STRIDENT ?
	    FunnyHallu ? "ticklish" : "pleased" :
	    FunnyHallu ? "full" : "satisfied");

	/* not your deity */
	if (on_altar() && p_aligntyp != u.ualign.type) {
		adjalign(-5);
		return;
	} else if (u.ualign.record < 2 && trouble <= 0) adjalign(1);

	/* depending on your luck & align level, the god you prayed to will:
	   - fix your worst problem if it's major.
	   - fix all your major problems.
	   - fix your worst problem if it's minor.
	   - fix all of your problems.
	   - do you a gratuitous favor.

	   if you make it to the the last category, you roll randomly again
	   to see what they do for you.

	   If your luck is at least 0, then you are guaranteed rescued
	   from your worst major problem. */

	if (!trouble && u.ualign.record >= DEVOUT) {
	    /* if hero was in trouble, but got better, no special favor */
	    if (p_trouble == 0) pat_on_head = 1;
	} else {
	    int action = rn1(Luck + (on_altar() ? 3 + on_shrine() : 2), 1);

	    if (!on_altar()) action = min(action, 3);
	    if (u.ualign.record < STRIDENT)
		action = (u.ualign.record > 0 || !rnl(2)) ? 1 : 0;
	    /* pleased Lawful gods often send you a helpful angel if you're
	       getting the crap beat out of you */
	    if ((u.uhp < 5 || (u.uhp*7 < u.uhpmax)) &&
		 u.ualign.type == A_LAWFUL && !rn2(4)) lawful_god_gives_angel(); /* lower chance --Amy */

	/* To prevent infinite loops and also to make prayer less of a cure-all, only try up to 5 times. --Amy */

	    switch(min(action,5)) {
	    case 5: pat_on_head = 1;
	    case 4: /*do fix_worst_trouble(trouble);
		    while ((trouble = in_trouble()) != 0);*/
			if ((trouble = in_trouble()) != 0) fix_worst_trouble(trouble);
			if ((trouble = in_trouble()) != 0) fix_worst_trouble(trouble);
			if ((trouble = in_trouble()) != 0) fix_worst_trouble(trouble);
			if ((trouble = in_trouble()) != 0) fix_worst_trouble(trouble);
			if ((trouble = in_trouble()) != 0) fix_worst_trouble(trouble);
		    break;

	    case 3: fix_worst_trouble(trouble);
	    case 2: /*while ((trouble = in_trouble()) > 0)
		    fix_worst_trouble(trouble);*/
			if ((trouble = in_trouble()) > 0) fix_worst_trouble(trouble);
			if ((trouble = in_trouble()) > 0) fix_worst_trouble(trouble);
			if ((trouble = in_trouble()) > 0) fix_worst_trouble(trouble);
			if ((trouble = in_trouble()) > 0) fix_worst_trouble(trouble);
			if ((trouble = in_trouble()) > 0) fix_worst_trouble(trouble);
		    break;

	    case 1: if (trouble > 0) fix_worst_trouble(trouble);
	    case 0: break; /* your god blows you off, too bad */
	    }
	}

    /* note: can't get pat_on_head unless all troubles have just been
       fixed or there were no troubles to begin with; hallucination
       won't be in effect so special handling for it is superfluous */

	/* lawful gods are nicer, chaotic ones less so --Amy */
    if(pat_on_head && !rn2((g_align == A_CHAOTIC) ? 5 : (g_align == A_NEUTRAL) ? 4 : 3) )
	switch(rn2((Luck + 6)>>1)) {
	case 0:	break;
	case 1:
	    if (uwep && !stack_too_big(uwep) && (welded(uwep) || uwep->oclass == WEAPON_CLASS ||
			 is_weptool(uwep))) {
		char repair_buf[BUFSZ];

		*repair_buf = '\0';
		if (uwep->oeroded || uwep->oeroded2)
		    sprintf(repair_buf, " and %s now as good as new",
			    otense(uwep, "are"));

		if (uwep->cursed) {
		    uncurse(uwep, TRUE);
		    uwep->bknown = TRUE;
		    if (!Blind)
			Your("%s %s%s.", aobjnam(uwep, "softly glow"),
			     hcolor(NH_AMBER), repair_buf);
		    else You_feel("the power of %s over your %s.",
			u_gname(), xname(uwep));
		    *repair_buf = '\0';
		} else if (!uwep->blessed) {
		    bless(uwep);
		    uwep->bknown = TRUE;
		    if (!Blind)
			Your("%s with %s aura%s.",
			     aobjnam(uwep, "softly glow"),
			     an(hcolor(NH_LIGHT_BLUE)), repair_buf);
		    else You_feel("the blessing of %s over your %s.",
			u_gname(), xname(uwep));
		    *repair_buf = '\0';
		}

		/* fix any rust/burn/rot damage, but don't protect
		   against future damage */
		if (uwep->oeroded || uwep->oeroded2) {
		    uwep->oeroded = uwep->oeroded2 = 0;
		    /* only give this message if we didn't just bless
		       or uncurse (which has already given a message) */
		    if (*repair_buf)
			Your("%s as good as new!",
			     aobjnam(uwep, Blind ? "feel" : "look"));
		}
		update_inventory();
	    }
	    break;
	case 3:
	    /* takes 2 hints to get the music to enter the stronghold */
	    if (!u.uevent.uopened_dbridge) {
		if (u.uevent.uheard_tune < 1) {
		    godvoice(g_align,(char *)0);
		    verbalize("Hark, %s!",
			  youmonst.data->mlet == S_HUMAN ? "mortal" : "creature");
		    verbalize(
			"To enter the castle, thou must play the right tune!");
		    u.uevent.uheard_tune++;
		    break;
		} else if (u.uevent.uheard_tune < 2) {
		    You_hear("a divine music...");
		    pline("It sounds like:  \"%s\".", tune);
		    u.uevent.uheard_tune++;
		    break;
		}
	    }
	    /* Otherwise, falls into next case */
	case 2:
	    if (!Blind)
		You("are surrounded by %s glow.", an(hcolor(NH_GOLDEN)));
	    /* if any levels have been lost (and not yet regained),
	       treat this effect like blessed full healing */
	    if (u.ulevel < u.ulevelmax) {
		u.ulevelmax -= 1;	/* see potion.c */
		pluslvl(FALSE);
	    } else {
		u.uhpmax += 5;
		if (Upolyd) u.mhmax += 5;
	    }
	    u.uhp = u.uhpmax;
	    if (Upolyd) u.mh = u.mhmax;
	    ABASE(A_STR) = AMAX(A_STR);
	    if (u.uhunger < 1500) init_uhunger();
	    if (u.uluck < 0) change_luck(1); /* used to be set to 0, but now you'll have to work for it --Amy */
	    make_blinded(0L,TRUE);
	    flags.botl = 1;
	    break;
	case 4: {
	    register struct obj *otmp;
	    int any = 0;

	    if (Blind)
		You_feel("the power of %s.", u_gname());
	    else You("are surrounded by %s aura.",
		     an(hcolor(NH_LIGHT_BLUE)));
	    for(otmp=invent; otmp; otmp=otmp->nobj) {
		if (otmp->cursed && !stack_too_big(otmp) ) {
		    uncurse(otmp, TRUE);
		    if (!Blind) {
			Your("%s %s.", aobjnam(otmp, "softly glow"),
			     hcolor(NH_AMBER));
			otmp->bknown = TRUE;
			++any;
		    }
		}
	    }
	    if (any) update_inventory();
	    break;
	}
	case 7:
	case 8:
	case 9:		/* KMH -- can occur during full moons */
	    if (u.ualign.record >= PIOUS && !u.uevent.uhand_of_elbereth) {
		gcrownu();
		break;
	    } /* else FALLTHRU */
	case 6:	{
	    struct obj *otmp;
	    int sp_no, trycnt = u.ulevel + 1;

	    at_your_feet("An object");
	    /* not yet known spells given preference over already known ones */
	    /* Also, try to grant a spell for which there is a skill slot */
	    otmp = mkobj(SPBOOK_CLASS, TRUE);
	    while (--trycnt > 0) {
		if (!otmp) break;
		if (otmp->otyp != SPE_BLANK_PAPER) {
		    for (sp_no = 0; sp_no < MAXSPELL; sp_no++)
			if (spl_book[sp_no].sp_id == otmp->otyp) break;
		    if (sp_no == MAXSPELL &&
			!P_RESTRICTED(spell_skilltype(otmp->otyp)))
			break;	/* usable, but not yet known */
		} else {
		    if (!objects[SPE_BLANK_PAPER].oc_name_known ||
			    carrying(MAGIC_MARKER)) break;
		}
		otmp->otyp = rnd_class(bases[SPBOOK_CLASS], SPE_BLANK_PAPER);
	    }
	    bless(otmp);
	    place_object(otmp, u.ux, u.uy);
	    break;
	}
	case 5: {
	    const char *msg="\"and thus I grant thee the gift of %s!\"";
	    godvoice(u.ualign.type, "Thou hast pleased me with thy progress,");
	    if (!(HTelepat & INTRINSIC))  {
		HTelepat |= FROMOUTSIDE;
		pline(msg, "Telepathy");
		if (Blind) see_monsters();
	    } else if (!(HFast & INTRINSIC))  {
		HFast |= FROMOUTSIDE;
		pline(msg, "Speed");
	    } else if (!(HStealth & INTRINSIC))  {
		HStealth |= FROMOUTSIDE;
		pline(msg, "Stealth");
	    } else {
		if (!(HProtection & INTRINSIC))  {
		    HProtection |= FROMOUTSIDE;
		    if (!u.ublessed)  u.ublessed = rn1(3, 2);
		} else if (!rn2(u.ublessed)) u.ublessed++; /* make it much harder to get to -127AC --Amy */
		pline(msg, "my protection");
	    }
	    verbalize("Use it wisely in my name!");
	    break;
	}
	default:	impossible("Confused deity!");
	    break;
	}

setprayertimeout:
	if (!((uarmc && itemhasappearance(uarmc, APP_STORM_COAT)) && !rn2(2))) u.ublesscnt = rnz(ishaxor ? 175 : 350);
	kick_on_butt = (u.uevent.udemigod && !u.freeplaymode && u.amuletcompletelyimbued) ? 1 : 0;
	if (u.uevent.uhand_of_elbereth) kick_on_butt++;
	if (kick_on_butt) u.ublesscnt += kick_on_butt * rnz(ishaxor ? 500 : 1000);
	if (uimplant && uimplant->oartifact == ART_CORONATION_CULMINATION) u.ublesscnt += rnz(ishaxor ? 500 : 1000);

	/* evil patch idea by jonadab: permaconverting yourself grants increased prayer timeout */
	if (u.ualignbase[A_CURRENT] != u.ualignbase[A_ORIGINAL]) u.ublesscnt += rnz(ishaxor ? 500 : 1000);

	return;
}

/* either blesses or curses water on the altar,
 * returns true if it found any water here.
 */
STATIC_OVL boolean
water_prayer(bless_water)
    boolean bless_water;
{
    register struct obj* otmp;
    register long changed = 0;
    boolean other = FALSE, bc_known = !(Blind || Hallucination);

    /* Praying on a coaligned altar will completely decontaminate you if it was safe to pray --Amy */
    if (bless_water && u.contamination) decontaminate(u.contamination);

    for(otmp = level.objects[u.ux][u.uy]; otmp; otmp = otmp->nexthere) {
	/* turn water into (un)holy water */
	if (otmp->otyp == POT_WATER &&
		(bless_water ? !otmp->blessed : !otmp->cursed)) {
	    otmp->blessed = bless_water;
	    otmp->cursed = !bless_water;
	    otmp->bknown = bc_known;
	    changed += otmp->quan;
	} else if(otmp->oclass == POTION_CLASS)
	    other = TRUE;
    }
    if(!Blind && changed) {
	pline("%s potion%s on the altar glow%s %s for a moment.",
	      ((other && changed > 1L) ? "Some of the" :
					(other ? "One of the" : "The")),
	      ((other || changed > 1L) ? "s" : ""), (changed > 1L ? "" : "s"),
	      (bless_water ? hcolor(NH_LIGHT_BLUE) : hcolor(NH_BLACK)));
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

    return((boolean)(changed > 0L));
}

void
godvoice(g_align, words)
    aligntyp g_align;
    const char *words;
{
    const char *quot = "";
    if(words)
	quot = "\"";
    else
	words = "";

    pline_The("voice of %s %s: %s%s%s", align_gname(g_align),
	  godvoices[rn2(SIZE(godvoices))], quot, words, quot);
}

STATIC_OVL void
gods_angry(g_align)
    aligntyp g_align;
{
    godvoice(g_align, "Thou hast angered me.");
}

/* The g_align god is upset with you. */
STATIC_OVL void
gods_upset(g_align)
	aligntyp g_align;
{
	if(g_align == u.ualign.type) u.ugangr++;
	else if(u.ugangr) u.ugangr--;
	angrygods(g_align);
}

static NEARDATA const char sacrifice_types[] = { FOOD_CLASS, AMULET_CLASS, 0 };
static NEARDATA const char ext_sacrifice_types[] = { ALLOW_FLOOROBJ,
	FOOD_CLASS, AMULET_CLASS, 0 };

STATIC_OVL void
consume_offering(otmp)
register struct obj *otmp;
{
    if (FunnyHallu)
	switch (rn2(3)) {
	    case 0:
		Your("sacrifice sprouts wings and a propeller and roars away!");
		break;
	    case 1:
		Your("sacrifice puffs up, swelling bigger and bigger, and pops!");
		break;
	    case 2:
		Your("sacrifice collapses into a cloud of dancing particles and fades away!");
		break;
	}
    else if (Blind && u.ualign.type == A_LAWFUL)
	Your("sacrifice disappears!");
    else Your("sacrifice is consumed in a %s!",
	      u.ualign.type == A_LAWFUL ? "flash of light" : "burst of flame");

    if (evilfriday) {
	/* It's no mistake that this happens even while hallucinating, because the things you saw while hallucinating
	 * weren't real, there still was the usual flash of light or burst of flame and you just perceived it as something
	 * different. So yeah, you'll still be affected --Amy */

	if (u.ualign.type == A_LAWFUL) {
		if (!Blind) pline("Everything suddenly goes dark.");
		make_blinded(Blinded + rnz(250), FALSE);
	} else {
		(void) burnarmor(&youmonst);
		if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) destroy_item(SCROLL_CLASS, AD_FIRE);
		if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) destroy_item(SPBOOK_CLASS, AD_FIRE);
		if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) destroy_item(POTION_CLASS, AD_FIRE);
		if (!Fire_resistance) {
			pline_The("fire burns you!");
			losehp(rnd(2), "the burst of flame from offering a sacrifice", KILLED_BY);
		}

	}

    }

    if (carried(otmp)) useup(otmp);
    else useupf(otmp, 1L);
    exercise(A_WIS, TRUE);
	use_skill(P_SPIRITUALITY, Role_if(PM_PRIEST) ? 5 : 1);
}

void
god_gives_pet(alignment)
aligntyp alignment;
{
/*
    register struct monst *mtmp2;
    register struct permonst *pm;
 */
    int mnum;
    int mon;

    switch ((int)alignment) {
	case A_LAWFUL:
	    mnum = lawful_minion(u.ulevel);
	    break;
	case A_NEUTRAL:
	    mnum = neutral_minion(u.ulevel);
	    break;
	case A_CHAOTIC:
	case A_NONE:
	    mnum = chaotic_minion(u.ulevel);
	    break;
	default:
	    impossible("unaligned player?");
	    mnum = ndemon(A_NONE);
	    break;
    }
    mon = make_pet_minion(mnum,alignment);
    if (mon) {
	switch ((int)alignment) {
	   case A_LAWFUL:
		pline("%s", Blind ? "You feel the presence of goodness." :
		 "There is a puff of white fog!");
	   break;
	   case A_NEUTRAL:
		pline("%s", Blind ? "You hear the earth rumble..." :
		 "A cloud of gray smoke gathers around you!");
	   break;
	   case A_CHAOTIC:
	   case A_NONE:
		pline("%s", Blind ? "You hear an evil chuckle!" :
		 "A miasma of stinking vapors coalesces around you!");
	   break;
	}
	if (PlayerHearsSoundEffects) pline(issoviet ? "Vy, veroyatno, blizok k smerti v lyubom sluchaye. Kak tol'ko vy natknulis' v eto novoye domashneye zhivotnoye, v to vremya kak zaputalsya, i glup, kak vy, veroyatno, vy budete delat' eto, igra zakonchitsya v vashem porazhenii." : "Chaeaeaeaet!");
	godvoice(u.ualign.type, "My minion shall serve thee!");
	return;
    }
}

static void
lawful_god_gives_angel()
{
/*
    register struct monst *mtmp2;
    register struct permonst *pm;
*/
    int mnum;
    int mon;

    mnum = lawful_minion(u.ulevel);
    mon = make_pet_minion(mnum,A_LAWFUL);
    pline("%s", Blind ? "You feel the presence of goodness." :
	 "There is a puff of white fog!");
	if (PlayerHearsSoundEffects) pline(issoviet ? "Vy, veroyatno, blizok k smerti v lyubom sluchaye. Kak tol'ko vy natknulis' v eto novoye domashneye zhivotnoye, v to vremya kak zaputalsya, i glup, kak vy, veroyatno, vy budete delat' eto, igra zakonchitsya v vashem porazhenii." : "Chaeaeaeaet!");
    if (u.uhp > (u.uhpmax / 10)) godvoice(u.ualign.type, "My minion shall serve thee!");
    else godvoice(u.ualign.type, "My minion shall save thee!");
}


/* KMH -- offerings to Oracle */
static int
offer_oracle (mtmp, otmp)
	struct monst *mtmp;
	struct obj *otmp;
{
	/* The Oracle doesn't care about the age or species of the corpse,
	 * except for the dreaded woodchuck.
	 */
	boolean woodchuck = (otmp->corpsenm == PM_WOODCHUCK);


	/* Make sure it's a corpse */
    if (otmp->otyp != CORPSE) {
		pline("%s", nothing_happens);
		if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
			pline("Oh wait, actually something bad happens...");
			badeffect();
		}
		return (1);
	}

	/* you're handling this corpse, even if it was killed upon the altar */
	feel_cockatrice(otmp, TRUE);

	pline("%s looks at you %sfully.", Monnam(mtmp), woodchuck ? "scorn" : "thought");
	outrumor(woodchuck ? -1 : 0, FALSE, FALSE);
	consume_offering(otmp);
	u.cnd_offercount++;
	adjalign(-5);	/* Your god is jealous */
	if (woodchuck) {
		/* You are annoying the Oracle */
		change_luck(-1);
		exercise(A_WIS, FALSE);
		exercise(A_WIS, FALSE);	/* outrumor() exercised */
		exercise(A_WIS, FALSE);	/* consume_offering() exercised */
	}
	return (1);
}


int
dosacrifice()
{
    register struct obj *otmp;
    int value = 0;
    int pm;
    aligntyp altaralign = a_align(u.ux,u.uy);
    struct monst *orac = NULL;

    /* KMH -- offerings to Oracle */
    if (Is_oracle_level(&u.uz) && !u.uswallow) {
	for(orac = fmon; orac; orac = orac->nmon)
	    if (orac->data == &mons[PM_ORACLE]) break;
	if (!orac || distu(orac->mx, orac->my) > 2 || !mon_visible(orac)) {
	    pline(FunnyHallu ? "You hold your hands up in the air. Nobody seems to notice though." : "I see no one next to you to take a sacrifice.");
	    return 0;
	}
    }
    if (!orac && (!on_altar() || u.uswallow)) {
	You("are not standing on an altar.");
	return 0;
    }

    if (Race_if(PM_PLAYER_ANDROID)) {
	You("do not give offerings to the god of the future.");
	return 0;
    }

    if (In_endgame(&u.uz)) {
	if (!(otmp = getobj(sacrifice_types, "sacrifice"))) return 0;
    } else {
	if (!(otmp = getobj(ext_sacrifice_types, "sacrifice"))) return 0;
    }

    /* KMH -- offerings to Oracle */
    if (orac) return (offer_oracle(orac, otmp));

    /*
      Was based on nutritional value and aging behavior (< 50 moves).
      Sacrificing a food ration got you max luck instantly, making the
      gods as easy to please as an angry dog!

      Now only accepts corpses, based on the game's evaluation of their
      toughness.  Human and pet sacrifice, as well as sacrificing unicorns
      of your alignment, is strongly discouraged.
     */

#define MAXVALUE 24 /* Highest corpse value (besides Wiz) */

    /* sacrificing the eye and/or hand of Vecna is a special case */
    if (otmp->oartifact == ART_EYE_OF_THE_BEHOLDER ||
	    otmp->oartifact == ART_HAND_OF_VECNA) {
	You("offer this evil thing to %s...", a_gname());
	value = MAXVALUE; /* holy crap! */
	u.uconduct.gnostic++;	/* KMH, ethics */
	if (Race_if(PM_MAGYAR)) {
		You_feel("bad about breaking the atheist conduct.");
		badeffect();
	}
    }

    if (otmp->otyp == CORPSE) {
	register struct permonst *ptr = &mons[otmp->corpsenm];
	struct monst *mtmp;
	extern const int monstr[];

	if (Role_if(PM_UNBELIEVER) ) {

		pline("You don't believe in gods! You cannot offer corpses!");
		return 0;
	}

	/* KMH, conduct */
	u.uconduct.gnostic++;
	if (Race_if(PM_MAGYAR)) {
		You_feel("bad about breaking the atheist conduct.");
		badeffect();
	}

	/* you're handling this corpse, even if it was killed upon the altar */
	feel_cockatrice(otmp, TRUE);

	if (otmp->corpsenm == PM_ACID_BLOB
		|| (monstermoves <= peek_at_iced_corpse_age(otmp) + 50)) {
	    value = monstr[otmp->corpsenm] + 1;
	}
	    if (otmp->oeaten)
		value = eaten_stat(value, otmp);

	/* fix for new races since they're MH_HUMAN but not actually supposed to count as same race --Amy */

	if (your_race(ptr) && !canofferownrace()) {
	    if (is_demon(youmonst.data) || Race_if(PM_HUMAN_WEREWOLF) || Race_if(PM_AK_THIEF_IS_DEAD_) || Role_if(PM_LUNATIC)) {
		You("find the idea very satisfying.");
		exercise(A_WIS, TRUE);
	    } else if (u.ualign.type != A_CHAOTIC) {
		    pline("You'll regret this infamous offense!");
		    exercise(A_WIS, FALSE);
	    }

	    if (altaralign != A_CHAOTIC && altaralign != A_NONE) {
		/* curse the lawful/neutral altar */
		int race = mrace2race(ptr->mflags2);
		if (race != ROLE_NONE)
		    pline_The("altar is stained with %s blood.",
			    races[race].adj);
		else
		    impossible("Bad monster race?");
		if(!Is_astralevel(&u.uz))
		    levl[u.ux][u.uy].altarmask = AM_CHAOTIC;
		angry_priest();
	    } else {
		struct monst *dmon;
		const char *demonless_msg;
		register struct obj *octmp;

		/* Human sacrifice on a chaotic or unaligned altar */
		/* is equivalent to demon summoning */
		if (altaralign == A_CHAOTIC && u.ualign.type != A_CHAOTIC) {
		    pline(
		     "The blood floods the altar, which vanishes in %s cloud!",
			  an(hcolor(NH_BLACK)));
		    levl[u.ux][u.uy].typ = ROOM;
		    levl[u.ux][u.uy].altarmask = 0;
		    newsym(u.ux, u.uy);
		    angry_priest();
		    demonless_msg = "cloud dissipates";
		} else {
		    /* either you're chaotic or altar is Moloch's or both */
		    pline_The("blood covers the altar, and a dark cloud forms!");
		    change_luck(altaralign == A_NONE ? -2 : 2);
		    demonless_msg = "blood coagulates";
		}
		if ((pm = (!rn2(20) ? dprince(altaralign) : dlord(altaralign) ) ) != NON_PM &&
		    (dmon = makemon(&mons[pm], u.ux, u.uy, NO_MM_FLAGS))) {
		    /* here to be seen */
		    dmon->minvis = FALSE;
		    if (dmon->minvisreal) dmon->minvis = TRUE;
		    You("have summoned %s!", a_monnam(dmon));
		    if (sgn(u.ualign.type) == sgn(dmon->data->maligntyp)) {
			dmon->mpeaceful = (rn2(2) ? TRUE : FALSE); /* making things a bit more dangerous for chaotics --Amy */
			if (dmon && dmon->mpeaceful && !rn2(500) ) dmon = tamedog(dmon, (struct obj *) 0, TRUE);
			} else {
			switch (rn2(14)) {
			  case 0:
			  case 5:
			  case 6:
			  case 7:
			  case 8:
			  case 9:
			       pline("He is furious!");
			       dmon->mpeaceful = FALSE;
			  break;
			  case 1:
			       pline("Angered at your summons, he curses you!");
			       /* but not angry enough to whup yer ass */
			       for(octmp = invent; octmp ; octmp = octmp->nobj)
				 if (!rn2(6) && !stack_too_big(octmp) ) curse(octmp);
			       break;
			  case 10:
			       pline("Angered at your summons, he curses you!");
			       dmon->mpeaceful = FALSE;
			       for(octmp = invent; octmp ; octmp = octmp->nobj)
				 if (!rn2(6) && !stack_too_big(octmp) ) curse(octmp);
			       break;
			  case 2: 
			  case 3:
			       You("are terrified, and unable to move.");
			       nomul(-3, "being terrified of a demon", TRUE);
			       nomovemsg = 0;
				 if (dmon && !rn2(500) ) dmon = tamedog(dmon, (struct obj *) 0, TRUE);
			       break;                
			  case 11:
			       dmon->mpeaceful = FALSE;
			       You("are terrified, and unable to move.");
			       nomul(-3, "being terrified of a hostile demon", TRUE);
			       nomovemsg = 0;
			       break;                
			  case 4:
			       pline("Amused, he grants you a boon!");
				 if (!rn2(4)) makewish(evilfriday ? FALSE : TRUE);
				 else othergreateffect();
				 if (dmon && !rn2(200) ) dmon = tamedog(dmon, (struct obj *) 0, TRUE);
			  case 13:
				 pline("He forces you to wear an artifact!");
				 bad_artifact();
				 break;
			  case 14:
				 pline("He forces you to wear an artifact!");
			       dmon->mpeaceful = FALSE;
				 bad_artifact();
				 break;
			  break;
		       }
		    }
		} else pline_The("%s.", demonless_msg);
	    }

	    if (u.ualign.type != A_CHAOTIC) {
		adjalign(-25);
		u.ugangr += 3;
		(void) adjattrib(A_WIS, -1, TRUE, TRUE);
		if (!Inhell) angrygods(u.ualign.type);
		change_luck(-5);
	    } else adjalign(5);
	    if (carried(otmp)) useup(otmp);
	    else useupf(otmp, 1L);

		/* create Dirge from player's longsword here if possible */
		if (u.ualign.type == A_CHAOTIC && Role_if(PM_KNIGHT) && !u.ugangr && u.ualign.record > 0 && uwep && uwep->otyp == LONG_SWORD && !uwep->oartifact && !exist_artifact(LONG_SWORD, artiname(ART_DIRGE))) {

			uwep = oname(uwep, artiname(ART_DIRGE));
			if (uwep) { /* you can never be safe... --Amy */
				curse(uwep);
				if (uwep->spe < 10) uwep->spe++;
				uwep->oeroded = uwep->oeroded2 = 0;
				uwep->oerodeproof = TRUE;
				discover_artifact(ART_DIRGE);
				exercise(A_WIS,TRUE);
				pline("Your sword slithers in your hand and seems to change!");
#ifdef LIVELOGFILE
				livelog_report_trophy("had Dirge gifted to them by the grace of a chaotic deity");
#endif
			}
		}

	    return(1);
	} else if (otmp->oxlth && otmp->oattached == OATTACHED_MONST
		    && ((mtmp = get_mtraits(otmp, FALSE)) != (struct monst *)0)
		    && mtmp->mtame) {
	    /* mtmp is a temporary pointer to a tame monster's attributes,
	     * not a real monster */
	    pline("So this is how you repay loyalty?");
	    adjalign(-25);
	    value = -1;
	    HAggravate_monster |= FROMOUTSIDE;
	} else if (is_undead(ptr)) { /* Not demons--no demon corpses */
	    if (u.ualign.type != A_CHAOTIC)
		value += 1;
	} else if (is_unicorn(ptr)) {
	    int unicalign = sgn(ptr->maligntyp);

	    /* If same as altar, always a very bad action. */
	    if (unicalign == altaralign) {
		pline("Such an action is an insult to %s!",
		      (unicalign == A_CHAOTIC)
		      ? "chaos" : unicalign ? "law" : "balance");
		(void) adjattrib(A_WIS, -1, TRUE, TRUE);
		value = -5;
	    } else if (u.ualign.type == altaralign) {
		/* If different from altar, and altar is same as yours, */
		/* it's a very good action */
		if (u.ualign.record < u.alignlim)
		    You_feel("appropriately %s.", align_str(u.ualign.type));
		else You_feel("you are thoroughly on the right path.");
		adjalign(5);
		u.alignlim += 1;
#ifdef NOARTIFACTWISH
		u.usacrifice += 5;
#endif
		value += 3;
	    } else
		/* If sacrificing unicorn of your alignment to altar not of */
		/* your alignment, your god gets angry and it's a conversion */
		if (unicalign == u.ualign.type) {
		    u.ualign.record = -1;
		    value = 1;
		} else value += 3;
	}
    } /* corpse */

    if (otmp->otyp == AMULET_OF_YENDOR) {
	if (u.freeplaymode) {
	    pline("You've ascended already. No need to offer the amulet again.");
	    return 1;
	} else if (!Is_astralevel(&u.uz)) {
	    if (FunnyHallu)
		    You_feel("homesick.");
	    else
		    You_feel("an urge to return to the surface.");
	    return 1;
	} else {
	    /* The final Test.	Did you win? */
	    if(uamul == otmp) Amulet_off();
	    u.uevent.ascended = 1;
	    if(carried(otmp)) useup(otmp); /* well, it's gone now */
	    else useupf(otmp, 1L);
	    You("offer the Amulet of Yendor to %s...", a_gname());
	    if (u.ualign.type != altaralign) {
		/* And the opposing team picks you up and
		   carries you off on their shoulders */
		adjalign(-99);
		qt_pager(QT_DISGRACE);
		pline("%s accepts your gift, and gains dominion over %s...",
		      a_gname(), u_gname());
		pline("%s is enraged...", u_gname());
		pline("Fortunately, %s permits you to live...", a_gname());
		pline("A cloud of %s smoke surrounds you...",
		      hcolor((const char *)"orange"));
		done(ESCAPED);
	    } else { /* super big win */
		adjalign(10);

#ifdef RECORD_ACHIEVE

		if (!achieve.ascended) {

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

                achieve.ascended = 1;
			qt_pager(QT_ASCENSION);
#ifdef LIVELOGFILE
		livelog_achieve_update();
#endif
#endif

		if (!issoviet) {
			pline("An invisible choir sings, and you are bathed in radiance...");
			godvoice(altaralign, "Congratulations, mortal!");
			display_nhwindow(WIN_MESSAGE, FALSE);
			verbalize("In return for thy service, I grant thee the gift of Immortality!");

			if (Role_if(PM_QUARTERBACK)) You("punt the Amulet of Yendor onto the high altar. You ascend!");
			else You("ascend to the status of Demigod%s...", flags.female ? "dess" : "");
		} else {
			pline("An invisible Red Army choir sings, and you are bathed in Chernobyl radiation...");
			godvoice(altaralign, "Haraso, tovari.");
			display_nhwindow(WIN_MESSAGE, FALSE);
			verbalize("In return for thy service, I grant thee a dacha by the Black Sea!");

			/* mocking message inspired by my roommate --Amy */
			pline("Yesli by vy podumali, chto budet voznagrazhdeniye, vy porezayete sebya! Eto igra-ublyudok, KHAR KHAR KHAR KHAR KHAR KHAR KHAR!!!");

			if (Role_if(PM_QUARTERBACK)) You("punt the Amulet of Yendor onto the high altar. You ascend!");
			else You("ascend to the status of Demigod%s...", flags.female ? "dess" : "");

		}
		done(ASCENDED);
	    }
	}
    } /* real Amulet */

    if (otmp->otyp == FAKE_AMULET_OF_YENDOR) {
	    if (flags.soundok)
		You_hear("a nearby thunderclap.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Davay! Predlozheniye amulet na nevernom altar'! Vy poluchite tysyachu bonusnykh ochkov, yesli vy delayete!" : "Daaaaaaaaaau dai dai.");
	    if (!otmp->known) {
		You("realize you have made a %s.",
		    FunnyHallu ? "boo-boo" : "mistake");
		otmp->known = TRUE;
		change_luck(-1);
		return 1;
	    } else {
		/* don't you dare try to fool the gods */
		change_luck(-3);
		adjalign(-100);
		u.ugangr += 3;
#ifdef NOARTIFACTWISH
		u.usacrifice = 0;
#endif
		value = -3;
	    }
    } /* fake Amulet */

    if (value == 0) {
	pline("%s", nothing_happens);
	if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
		pline("Oh wait, actually something bad happens...");
		badeffect();
	}
	return (1);
    }

    if (altaralign != u.ualign.type &&
	(Is_astralevel(&u.uz) || Is_sanctum(&u.uz))) {
	/*
	 * REAL BAD NEWS!!! High altars cannot be converted.  Even an attempt
	 * gets the god who owns it truely pissed off.
	 */
	You_feel("the air around you grow charged...");
	pline("Suddenly, you realize that %s has noticed you...", a_gname());
	godvoice(altaralign, "So, mortal!  You dare desecrate my High Temple!");
	/* Throw everything we have at the player */
	god_zaps_you(altaralign);
    } else if (value < 0) { /* I don't think the gods are gonna like this... */
	gods_upset(altaralign);
    } else {
	int saved_anger = u.ugangr;
	int saved_cnt = u.ublesscnt;
	int saved_luck = u.uluck;

	/* Sacrificing at an altar of a different alignment */
	if (u.ualign.type != altaralign) {
	    /* Is this a conversion ? */
	    /* An unaligned altar in Gehennom will always elicit rejection. */
	    if (ugod_is_angry() || (altaralign == A_NONE && ( (Inhell && !Race_if(PM_HERETIC) ) || flags.gehenna ) )) {
		if(u.ualignbase[A_CURRENT] == u.ualignbase[A_ORIGINAL] && !(Race_if(PM_CHIQUAI) && rn2(5)) &&
		   altaralign != A_NONE) {
		    You("have a strong feeling that %s is angry...", u_gname());
		    consume_offering(otmp);
		    u.cnd_offercount++;
		    pline("%s accepts your allegiance.", a_gname());

		    /* The player wears a helm of opposite alignment? */
		    if (uarmh && uarmh->otyp == HELM_OF_OPPOSITE_ALIGNMENT)
			u.ualignbase[A_CURRENT] = altaralign;
		    else
			u.ualign.type = u.ualignbase[A_CURRENT] = altaralign;
		    u.ublessed = 0;
		    flags.botl = 1;

		    You("have a sudden sense of a new direction.");
		    /* Beware, Conversion is costly */
		    change_luck(-3);
		    u.ublesscnt += (ishaxor ? 150 : 300);
		    adjalign((int)(u.ualignbase[A_ORIGINAL] * (u.alignlim / 2)));
		} else {
		    u.ugangr += 3;
		    adjalign(-25);
#ifdef NOARTIFACTWISH
		    u.usacrifice = 0;
#endif
		    pline("%s rejects your sacrifice!", a_gname());
		    godvoice(altaralign, "Suffer, infidel!");
		    change_luck(-5);
		    (void) adjattrib(A_WIS, -2, TRUE, TRUE);
		    if (!Inhell) angrygods(u.ualign.type);
			else 	angrygods(altaralign);
		}
		return(1);
	    } else {
		consume_offering(otmp);
		u.cnd_offercount++;
		You("sense a conflict between %s and %s.",
		    u_gname(), a_gname());
		if (((rn2(2) && rn2(8 + u.ulevel) > 5) || (Race_if(PM_EGYMID) && rn2(2) && rn2(8 + u.ulevel) > 5)) && !(Race_if(PM_CHIQUAI) && rn2(3)) ) {
		    /* lower chance of conversion --Amy */
		    struct monst *pri;
		    You_feel("the power of %s increase.", u_gname());
		    u.cnd_altarconvertamount++;
		    if (!rn2(2) || rnl(u.ulevel) > 3) { /* higher chance of conversion creating minions --Amy */
		    	/* KMH -- Only a chance of this happening */
				You_feel("%s is very angry at you!", a_gname());                    
				summon_minion(altaralign, FALSE);
				summon_minion(altaralign, FALSE);
				if (rn2(u.ulevel)) summon_minion(altaralign, FALSE); /* summon more --Amy */
				if (rn2(u.ulevel)) summon_minion(altaralign, FALSE);
				if (!rn2(2)) summon_minion(altaralign, FALSE);
				if (!rn2(4)) summon_minion(altaralign, FALSE);
				if (!rn2(8)) summon_minion(altaralign, FALSE);
				if (!rn2(16)) summon_minion(altaralign, FALSE);
				if (!rn2(32)) summon_minion(altaralign, FALSE);
				if (!rn2(64)) summon_minion(altaralign, FALSE);
				if (!rn2(128)) summon_minion(altaralign, FALSE);
				if (!rn2(256)) summon_minion(altaralign, FALSE);
				if (!rn2(512)) summon_minion(altaralign, FALSE);
		    } else {
			    pline("%s's voice penetrates your mind from the distance: 'Oh it's just %s, I don't care, I'm on the phone'.", a_gname(), playeraliasname); /* thanks Lorskel, it's about time that I actually implement this --Amy */
		    }
		    if (altaralign == A_NONE && uarmf && uarmf->oartifact == ART_VARIANTISH_DESIGN) {
				(void) makemon(mkclass(S_ORC,0), u.ux, u.uy, MM_ANGRY);
				(void) makemon(mkclass(S_ORC,0), u.ux, u.uy, MM_ANGRY);
				(void) makemon(mkclass(S_ORC,0), u.ux, u.uy, MM_ANGRY);
				(void) makemon(mkclass(S_ORC,0), u.ux, u.uy, MM_ANGRY);
				(void) makemon(mkclass(S_ORC,0), u.ux, u.uy, MM_ANGRY);
				pline("Orctown woes!");
		    }

		    u.ublesscnt = 0;  /* WAC You deserve this ... */
		    exercise(A_WIS, TRUE);
#ifdef NOARTIFACTWISH
		    u.usacrifice += 5;
#endif
		    change_luck(1);
		    /* Yes, this is supposed to be &=, not |= */
		    levl[u.ux][u.uy].altarmask &= AM_SHRINE;
		    /* the following accommodates stupid compilers */
		    levl[u.ux][u.uy].altarmask =
			levl[u.ux][u.uy].altarmask | (Align2amask(u.ualign.type));
		    if (!Blind)
			pline_The("altar glows %s.",
			      hcolor(
			      u.ualign.type == A_LAWFUL ? NH_WHITE :
			      u.ualign.type ? NH_BLACK : (const char *)"gray"));

		    if (rnl(u.ulevel) > 6 && u.ualign.record > 0 &&
		       rnd(u.ualign.record) > (3*u.alignlim)/4) {
			summon_minion(altaralign, TRUE);
			if (rn2(u.ulevel)) summon_minion(altaralign, TRUE); /* summon more --Amy */
			if (!rn2(3)) summon_minion(altaralign, TRUE);
			if (!rn2(9)) summon_minion(altaralign, TRUE);
			if (!rn2(27)) summon_minion(altaralign, TRUE);
			if (!rn2(81)) summon_minion(altaralign, TRUE);
			if (!rn2(243)) summon_minion(altaralign, TRUE);
			}

		    /* anger priest; test handles bones files */
		    if((pri = findpriest(temple_occupied(u.urooms))) &&
		       !p_coaligned(pri))
			angry_priest();
		} else {
		    pline("Unluckily, you feel the power of %s decrease.",
			  u_gname());
		    change_luck(-1);
#ifdef NOARTIFACTWISH
		    u.usacrifice = 0;
#endif
		    exercise(A_WIS, FALSE);
		    if (rnl(u.ulevel) > 6 && u.ualign.record > 0 &&
		       rnd(u.ualign.record) > (7*u.alignlim)/8) {
			summon_minion(altaralign, TRUE);
			if (rn2(u.ulevel)) summon_minion(altaralign, TRUE); /* summon more --Amy */
			if (!rn2(3)) summon_minion(altaralign, TRUE);
			if (!rn2(9)) summon_minion(altaralign, TRUE);
			if (!rn2(27)) summon_minion(altaralign, TRUE);
			if (!rn2(81)) summon_minion(altaralign, TRUE);
			if (!rn2(243)) summon_minion(altaralign, TRUE);
			}

		}
		return(1);
	    }
	}

	consume_offering(otmp);
	u.cnd_offercount++;

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

	if (!rn2(findpriest(temple_occupied(u.urooms)) ? 500 : 100) && !issoviet && (!Is_astralevel(&u.uz)) ) {
		levl[u.ux][u.uy].typ = ROOM;
		pline_The("altar suddenly vanishes!"); /* hopefully this will teach those altar-campers! --Amy */
		newsym(u.ux,u.uy);
	} else if (!rn2(10) && (!Is_astralevel(&u.uz)) && (u.uprops[DESECRATION].extrinsic || Desecration || have_nonsacredstone() ) ) {
		levl[u.ux][u.uy].typ = ROOM;
		pline_The("altar suddenly vanishes!");
		newsym(u.ux,u.uy);
	} else if (!rn2(findpriest(temple_occupied(u.urooms)) ? 5000 : 100) && !issoviet && (!Is_astralevel(&u.uz))) {
		levl[u.ux][u.uy].altarmask = !rn2(4) ? Align2amask(A_CHAOTIC) : !rn2(3) ? Align2amask(A_NEUTRAL) : !rn2(2) ? Align2amask(A_LAWFUL) : Align2amask(A_NONE);
		pline("The altar rotates and seems to be changing.");
	}

	if (Role_if(PM_POLITICIAN) && !rn2(5)) {

	u.ugold += rnz(100);
	pline(FunnyHallu ? "Bribe money! Yay!" : "Your budget is extended!");

	}

	/* Amy edit: curing sanity is way too difficult! so here's one way of reducing it a bit */
	reducesanity(rnd(20));

	/* OK, you get brownie points. */
	if(u.ugangr) {

		/* In the Evil Variant, gods are much harder to mollify if they're very angry. There's no upper limit,
		 * meaning that extremely angry gods might require sacrifices whose levels are higher than the
		 * maximum possible monster level, making them impossible to mollify. Yes, this is intentional --Amy */
		if (evilfriday && value > 0) {
			value -= u.ugangr;
			if (value < 0) value = 0;
		}

	    u.ugangr -=
		((value * (u.ualign.type == A_CHAOTIC ? rnd(2) : rnd(3) )) / MAXVALUE); /* harder --Amy */
	    if(u.ugangr < 0) u.ugangr = 0;
	    if(u.ugangr != saved_anger) {
		if (u.ugangr) {
		    pline("%s seems %s.", u_gname(),
			  FunnyHallu ? "groovy" : "slightly mollified");

		    if ((int)u.uluck < 0) change_luck(1);
		} else {
		    pline("%s seems %s.", u_gname(), FunnyHallu ?
			  "cosmic (not a new fact)" : "mollified");

		    if ((int)u.uluck < 0) change_luck(1); /* used to be set to 0, gotta make it harder to get your luck back --Amy */
		}
	    } else { /* not satisfied yet */
		if (FunnyHallu)
		    pline_The("gods seem tall.");
		else You("have a feeling of inadequacy.");
	    }
	} else if(ugod_is_angry()) {
	    if(value > MAXVALUE) value = MAXVALUE;
	    if(value > -u.ualign.record) value = -u.ualign.record;
	    adjalign(value);
	    You_feel("partially absolved.");
	} else if (u.ublesscnt > 0) {
	    u.ublesscnt -=
		((value * (u.ualign.type == A_CHAOTIC ? 500 : 300)) / MAXVALUE);
	    if(u.ublesscnt < 0) u.ublesscnt = 0;
	    if(u.ublesscnt != saved_cnt) {
		if (u.ublesscnt) {
		    if (FunnyHallu)
			You("realize that the gods are not like you and I.");
		    else
			You("have a hopeful feeling.");
		    if ((int)u.uluck < 0) change_luck(1);
		} else {
		    if (FunnyHallu)
			pline("Overall, there is a smell of fried onions.");
		    else
			You("have a feeling of reconciliation.");
		    if ((int)u.uluck < 0) /*u.uluck = 0;*/change_luck(1);
		}
	    }
	/* neutrals get gifts most often, chaotics are most unlikely to get them --Amy */
	} else if (!rn2((altaralign == A_CHAOTIC) ? (value > 24 ? 3 : value > 10 ? 4 : 5) : (altaralign == A_LAWFUL) ? (value > 24 ? 2 : value > 10 ? 3 : 4) : (value > 24 ? 1 : value > 10 ? 2 : 3) )) {
	    int nartifacts = nartifact_exist();

	    /* you were already in pretty good standing */
	    /* The player can gain an artifact */
	    /* The chance goes down as the number of artifacts goes up */
	    if ((u.ulevel > 2 || !issoviet) && u.uluck >= 0 &&
		!rn2(Role_if(PM_GANG_SCHOLAR) ? (2 + (u.ugifts) + (nartifacts)) : (10 + (5 * u.ugifts) + (nartifacts)))) { /* modified chance --Amy */
		otmp = mk_artifact((struct obj *)0, a_align(u.ux,u.uy), TRUE);
		if (otmp) {
		    if (otmp->spe < 0) otmp->spe = 0;
		    if (otmp->cursed) uncurse(otmp, TRUE);
		    otmp->oerodeproof = TRUE;
		    dropy(otmp);
		    at_your_feet("An object");
		    godvoice(u.ualign.type, "Use my gift wisely!");
#ifdef LIVELOGFILE
			char	 reportbuf[BUFSZ];
			sprintf (reportbuf, "received %s by %s", ONAME(otmp), u_gname());
			livelog_report_trophy(reportbuf);
#endif
		    u.ugifts++;
		    u.ublesscnt = rnz(300 + (50 * nartifacts));
			if (ishaxor) u.ublesscnt /= 2;
		    exercise(A_WIS, TRUE);
		    /* make sure we can use this weapon */

			if (P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_ISRESTRICTED) {
			    unrestrict_weapon_skill(get_obj_skill(otmp, TRUE));
			} else if (P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_UNSKILLED) {
				unrestrict_weapon_skill(get_obj_skill(otmp, TRUE));
				P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_BASIC;
			} else if (rn2(2) && P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_BASIC) {
				P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_SKILLED;
			} else if (!rn2(4) && P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_SKILLED) {
				P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_EXPERT;
			} else if (!rn2(10) && P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_EXPERT) {
				P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_MASTER;
			} else if (!rn2(100) && P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_MASTER) {
				P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_GRAND_MASTER;
			} else if (!rn2(200) && P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_GRAND_MASTER) {
				P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_SUPREME_MASTER;
			}

		    discover_artifact(otmp->oartifact);
		    return(1);
		} else {
		    pline("A spellbook appears at your %s!",
				    makeplural(body_part(FOOT)));
		    bless(mkobj_at(SPBOOK_CLASS,
				    u.ux, u.uy, TRUE));
#ifdef NOARTIFACTWISH
				u.usacrifice = 0;
#endif
				return(1);
		}
	    } else if (!rnl(30 + u.ulevel)) {
			/* no artifact, but maybe a helpful pet? */
			/* WAC is now some generic benefit (includes pets) */
			god_gives_benefit(altaralign);
#ifdef NOARTIFACTWISH
		    u.usacrifice = 0;
#endif
		    return(1);
	    }

	    if (!rn2(3)) { change_luck((value * LUCKMAX) / (MAXVALUE * 2));
	    if ((int)u.uluck < 0) /*u.uluck = 0;*/change_luck(1);
	    if (u.uluck != saved_luck) {
		if (Blind)
		    You("think %s brushed your %s.",something, body_part(FOOT));
		else You(FunnyHallu ?
		    "see crabgrass at your %s.  A funny thing in a dungeon." :
		    "glimpse a four-leaf clover at your %s.",
		    makeplural(body_part(FOOT)));
			}
	    }
	}
    }
    return(1);
}



/* determine prayer results in advance; also used for enlightenment */
boolean
can_pray(praying)
boolean praying;	/* false means no messages should be given */
{
    int alignment;
	int maedhrosbonus = (uarm && uarm->oartifact == ART_MAEDHROS_SARALONDE) ? 250 : 0;

    p_aligntyp = on_altar() ? a_align(u.ux,u.uy) : ( (Race_if(PM_HERETIC) || (Confusion && !rn2(StrongConf_resist ? 10000 : Conf_resist ? 1000 : 100) ) ) ? (!rn2(3) ? A_CHAOTIC : !rn2(2) ? A_NEUTRAL : A_LAWFUL ) : u.ualign.type);
    p_trouble = in_trouble();

    if (Race_if(PM_PLAYER_ANDROID) && praying) { /* dnethack anachronononononononononononaut */
	pline("There is but one god in the future, and to it you do not pray.");
	if (FunnyHallu) pline("(The game is your dad, so if it tells you that you don't pray to this god, you really don't pray to it.)");
	return FALSE;

    }

    if (is_demon(youmonst.data) && (p_aligntyp != A_CHAOTIC) && !Race_if(PM_MAZKE) && !Race_if(PM_BORG) && !Race_if(PM_AUREAL) ) {
	if (praying)
	    pline_The("very idea of praying to a %s god is repugnant to you.",
		  p_aligntyp ? "lawful" : "neutral");
	return FALSE;
    }

    if (!strncmpi(align_gname(p_aligntyp), "No God", 6) ) {
	if (praying)
		pline("You do not believe in gods, so praying is impossible!");
	return FALSE;
    }

    if (praying)
	You("begin praying to %s.", align_gname(p_aligntyp));

    if (u.ualign.type && u.ualign.type == -p_aligntyp)
	alignment = -u.ualign.record;		/* Opposite alignment altar */
    else if (u.ualign.type != p_aligntyp)
	alignment = u.ualign.record / 2;	/* Different alignment altar */
    else alignment = u.ualign.record;

	/* chaotic gods allow you to pray more often, lawfuls allow it least often --Amy */
    if ((p_trouble > 0) ? (u.ublesscnt > ((u.ualign.type == A_CHAOTIC) ? (300 + maedhrosbonus) : (u.ualign.type == A_NEUTRAL) ? (250 + maedhrosbonus) : (200 + maedhrosbonus) ) ) : /* big trouble */
	(p_trouble < 0) ? (u.ublesscnt > ((u.ualign.type == A_CHAOTIC) ? (200 + maedhrosbonus) : (u.ualign.type == A_NEUTRAL) ? (125 + maedhrosbonus) : (100 + maedhrosbonus) ) ) : /* minor difficulties */
	(u.ublesscnt > 0))			/* not in trouble */
	p_type = 0;		/* too soon... */
    else if ((int)Luck < (HardcoreAlienMode ? rn2(2) : Race_if(PM_SPARD) ? -1 : 0) || u.ugangr || alignment < (HardcoreAlienMode ? rn2(20) : Race_if(PM_SPARD) ? -10 : 0) )
	p_type = 1;		/* too naughty... */
    else /* alignment >= 0 */ {
	if(on_altar() && u.ualign.type != p_aligntyp)
	    p_type = 2;
	else if (Race_if(PM_HERETIC) && u.ualign.type != p_aligntyp)
	    p_type = 2;
	else
	    p_type = 3;
    }

	/* Lawful characters polymorphed into an undead (or of vampiric or ghast race) have a higher chance
	   of their prayers actually working, because we don't want lawful vampires to be unplayable. --Amy */

    if (is_undead(youmonst.data) && !Inhell &&
	( (p_aligntyp == A_LAWFUL && !rn2(2)) || (p_aligntyp == A_NEUTRAL && !rn2(10))))
	p_type = -1;
    /* Note:  when !praying, the random factor for neutrals makes the
       return value a non-deterministic approximation for enlightenment.
       This case should be uncommon enough to live with... */

    return !praying ? (boolean)(p_type == 3 && !Inhell) : TRUE;
}

int
dopray()
{
    /* Confirm accidental slips of Alt-P */

    if ((Role_if(PM_GANG_SCHOLAR) || (uarmh && uarmh->oartifact == ART_SALADIN_S_DESERT_FOX)) && u.scholarturns < moves) {

	u.scholarturns = moves + 1000;

	if (can_pray(FALSE) && !u.ugangr) godvoice(u.ualign.type, "Ah yes, my scholar, pray to me and I will lend thee my helping hand!");
	else {
		if (u.ugangr) godvoice(u.ualign.type, "Thou hast angered me before. I expect thee to make a valuable sacrifice to me first before thou mayst pray again.");
		if (u.ublesscnt > 0) godvoice(u.ualign.type, "Thou mayst not pray yet!");
		if ((int)Luck < (HardcoreAlienMode ? 1 : Race_if(PM_SPARD) ? -1 : 0) ) godvoice(u.ualign.type, "Thou art unlucky! I cannot help thee now!");
		if (u.ualign.record < (HardcoreAlienMode ? 20 : Race_if(PM_SPARD) ? -10 : 0) ) godvoice(u.ualign.type, "Thou must not pray to me, for thou hath sinned!");
		if (Inhell || flags.gehenna) godvoice(u.ualign.type, "My scholar, I cannot help thee in the Under World!");
	}

    } else if (uimplant && uimplant->oartifact == ART_HO_YOO_YOYO) {
		if (can_pray(FALSE) && !u.ugangr) pline("You can safely pray.");
		else pline("You can not safely pray.");
    }

    if (flags.prayconfirm)
	if (yn("Are you sure you want to pray?") == 'n')
	    return 0;

    if (Race_if(PM_MAGYAR)) {
	adjalign(-10);
	change_luck(-1);
	pline("Praying as a magyar is a risky action because you're supposed to be an atheist!");
    }

    u.uconduct.gnostic++;
	if (Race_if(PM_MAGYAR)) {
		You_feel("bad about breaking the atheist conduct.");
		badeffect();
	}
    u.uconduct.praydone++;
    /* Praying implies that the hero is conscious and since we have
       no deafness attribute this implies that all verbalized messages
       can be heard.  So, in case the player has used the 'O' command
       to toggle this accessible flag off, force it to be on. */
    if (!(YouAreDeaf)) flags.soundok = 1; /* Amy edit: we do have a deafness attribute... :D */

    if (IS_TOILET(levl[u.ux][u.uy].typ)) {
	pline("You pray to the Porcelain God.");

	if (!rn2(10)) {levl[u.ux][u.uy].typ = ROOM;
	pline_The("toilet suddenly vanishes!"); /* otherwise you could just cure food poisoning indefinitely --Amy */
	newsym(u.ux,u.uy); return(1);}

	if (!Sick && !HConfusion && !HStun) {
	    pline("He ignores your pleas.");
	    return(1);
	}
	pline("He smiles upon you.");
	if (Sick) make_sick(0L, (char *)0, TRUE, SICK_ALL);
	if (HConfusion) make_confused(0L, TRUE);
	if (HStun) make_stunned(0L, TRUE);
	use_skill(P_SPIRITUALITY, Role_if(PM_PRIEST) ? 5 : 1);
	return(1);
    }

    /* set up p_type and p_alignment */
    if (!can_pray(TRUE)) return 0;

#ifdef WIZARD
    if (wizard && p_type >= 0) {
	if (yn("Force the gods to be pleased?") == 'y') {
	    u.ublesscnt = 0;
	    if (u.uluck < 0) u.uluck = 0;
	    if (u.ualign.record <= 0) u.ualign.record = 1;
	    u.ugangr = 0;
	    if(p_type < 2) p_type = 3;
	}
    }
#endif

    if (tech_inuse(T_PRAYING_SUCCESS)) {
	    u.ublesscnt = 0;
	    if (u.uluck < 0) u.uluck = 0;
	    if (u.ualign.record <= 0) u.ualign.record = 1;
	    u.ugangr = 0;
	    if(p_type < 2) p_type = 3;
    }

    nomul(-3, "praying unsuccessfully", TRUE); /* note by Amy: if successfully, you're invulnerable... */
    nomovemsg = "You finish your prayer.";
    afternmv = prayer_done;

	/* if you've been true to your god you can't die while you pray */
    if(p_type == 3 && !Inhell) {
	if (!Blind) You("are surrounded by a shimmering light.");
	u.uinvulnerable = TRUE;
	use_skill(P_SPIRITUALITY, Role_if(PM_PRIEST) ? rnd(100) : rnd(20) );
    }
    return(1);
}

/*STATIC_PTR */int
prayer_done()		/* M. Stephenson (1.0.3b) */
{
    aligntyp alignment = p_aligntyp;

    u.uinvulnerable = FALSE;
    if(p_type == -1) {
	godvoice(alignment,
		 alignment == A_LAWFUL ?
		 "Vile creature, thou durst call upon me?" :
		 "Walk no more, perversion of nature!");
	You_feel("like you are falling apart.");
	if (Upolyd && !Race_if(PM_UNGENOMOLD) ) {
	    /* KMH, balance patch -- Gods have mastery over unchanging */
	rehumanize();
	}
	/* ALI, Racially undead (ie., vampires) get the same penalties */
	else if (rn2(20) > ACURR(A_CON))
	    u.uhp = (u.uhp/2) + 1;
	losehp(rnd(20), "residual undead turning effect", KILLED_BY_AN);
	exercise(A_CON, FALSE);
	return(1);
    }
    if ( (Inhell && !Race_if(PM_HERETIC) ) || flags.gehenna ) {
	pline("Since you are in Gehennom, %s won't help you.",
	      align_gname(alignment));
	/* haltingly aligned is least likely to anger */
	if (u.ualign.record <= 0 || rnl(u.ualign.record) || rn2(2) )
	    angrygods(u.ualign.type);
	return(0);
    }

	/* Imperial heretics can't pray --Amy */
    if (Race_if(PM_IMPERIAL) ) {
	pline("%s won't help a filthy imperial heretic like you.",
	      align_gname(alignment));
	godvoice(u.ualign.type, "Join the Stormcloaks or die, worthless human being!");
      angrygods(u.ualign.type);
	return(0);
    }

	if (uarmc && !rn2(10) && itemhasappearance(uarmc, APP_GODLESS_CLOAK) ) {

	angrygods(u.ualign.type);
	return(0);
	}

	if (RngeAngryGods && !rn2(10)) {
		angrygods(u.ualign.type);
		return(0);
	}

	/* praying as a unicorn can just randomly fail, because the race is too powerful --Amy */
	if (Race_if(PM_PLAYER_UNICORN) && !on_altar() && !rn2(2) ) { /* it's safe if you're on an altar */
		angrygods(u.ualign.type);
		return(0);
	}

    if (p_type == 0) {
	if(on_altar() && u.ualign.type != alignment)
	    (void) water_prayer(FALSE);
	if (!((uarmc && itemhasappearance(uarmc, APP_STORM_COAT)) && !rn2(2))) u.ublesscnt += rnz(ishaxor ? 125 : 250);
	change_luck(-rnd(3)); /* used to always be -3 --Amy */
	gods_upset(u.ualign.type);
    } else if(p_type == 1) {
	if(on_altar() && u.ualign.type != alignment)
	    (void) water_prayer(FALSE);
	angrygods(u.ualign.type);	/* naughty */
    } else if(p_type == 2) {
	if(water_prayer(FALSE)) {
	    /* attempted water prayer on a non-coaligned altar */
	    if (!((uarmc && itemhasappearance(uarmc, APP_STORM_COAT)) && !rn2(2))) u.ublesscnt += rnz(ishaxor ? 125 : 250);
	    change_luck(-3);
	    gods_upset(u.ualign.type);
	} else pleased(alignment);
    } else {
	/* coaligned */
	if(on_altar())
	    (void) water_prayer(TRUE);
	pleased(alignment); /* nice */
    }
    return(1);
}

int
doturn()
{	
	/* WAC doturn is now a technique */
	/* Try to use turn undead spell if you don't know the tech. */
/*	if (!Role_if(PM_PRIEST) && !Role_if(PM_KNIGHT) && !Role_if(PM_UNDEAD_SLAYER)) {*/
	if (!tech_known(T_TURN_UNDEAD)) {
		if (objects[SPE_TURN_UNDEAD].oc_name_known) {
		    register int sp_no;
		    for (sp_no = 0; sp_no < MAXSPELL &&
			 spl_book[sp_no].sp_id != NO_SPELL &&
			 spl_book[sp_no].sp_id != SPE_TURN_UNDEAD; sp_no++);

		    if (sp_no < MAXSPELL &&
			spl_book[sp_no].sp_id == SPE_TURN_UNDEAD)
			    return spelleffects(sp_no, TRUE);
		}

		You("don't know how to turn undead!");
		return(0);
	}
	if (!wizard) {
		pline("In order to turn undead, use #technique or cast the turn undead spell.");
		return(0);
	}
	return(turn_undead());
}

int
turn_undead()
{
	struct monst *mtmp, *mtmp2;
	int once, range, xlev;

	u.uconduct.gnostic++;
	if (Race_if(PM_MAGYAR)) {
		You_feel("bad about breaking the atheist conduct.");
		badeffect();
	}

	int alignmentlimit;

	if ((u.ualign.type != A_CHAOTIC &&
		    (is_demon(youmonst.data) || is_undead(youmonst.data))) ||
				u.ugangr > 6 /* "Die, mortal!" */) {

		pline("For some reason, %s seems to ignore you.", u_gname());
		aggravate();
		exercise(A_WIS, FALSE);
		return(0);
	}

	if ( (Inhell && !Race_if(PM_HERETIC) && !(uwep && uwep->oartifact == ART_ORTHODOX_MANIFEST) ) || flags.gehenna) {
	    pline("Since you are in Gehennom, %s won't help you.", u_gname());
	    aggravate();
	    return(0);
	}

	if (u.ualign.record < -20) {
		pline("For some reason, %s seems to ignore you.", u_gname());
		aggravate();
		exercise(A_WIS, FALSE);
		adjalign(-rnd(20));
		return(0);

	}

	alignmentlimit = (u.ualign.record < -10) ? 10 : (u.ualign.record < -15) ? 15 : (u.ualign.record + 15);

	pline("Calling upon %s, you chant an arcane formula.", u_gname());
	exercise(A_WIS, TRUE);

	/* note: does not perform unturn_dead() on victims' inventories */
	range = BOLT_LIM + (u.ulevel / 5);	/* 5 to 11 */
	range *= range;
	once = 0;
	for(mtmp = fmon; mtmp; mtmp = mtmp2) {
	    mtmp2 = mtmp->nmon;

	    if (DEADMONSTER(mtmp)) continue;
	    if (!cansee(mtmp->mx,mtmp->my) ||
		distu(mtmp->mx,mtmp->my) > range) continue;

	    if (distu(mtmp->mx,mtmp->my) > rnd(150)) continue;

	    if (distu(mtmp->mx,mtmp->my) > alignmentlimit) continue;

	    if (!mtmp->mpeaceful && (is_undead(mtmp->data) || mtmp->egotype_undead ||
		   (is_demon(mtmp->data) && (u.ulevel > (MAXULEV/2))))) {

		    mtmp->msleeping = 0;
		    if (Confusion && !Conf_resist) {
			if (!once++)
			    pline("Unfortunately, your voice falters.");
			mtmp->mflee = 0;
			mtmp->mfrozen = 0;
			mtmp->masleep = 0;
			mtmp->mcanmove = 1;
		    } else if (!resist(mtmp, '\0', 0, TELL)) {
			xlev = 6;
			switch (mtmp->data->mlet) {
			    /* this is intentional, lichs are tougher
			       than zombies. */
			    /* ToDo - catch vampire bats */
			case S_LICH:    xlev += 2;  /*FALLTHRU*/
			case S_GHOST:   xlev += 2;  /*FALLTHRU*/
			case S_VAMPIRE: xlev += 2;  /*FALLTHRU*/
			case S_WRAITH:  xlev += 2;  /*FALLTHRU*/
			case S_MUMMY:   xlev += 2;  /*FALLTHRU*/
			case S_ZOMBIE:
			    if ((u.ulevel >= xlev) && (rnd(30) >= xlev) &&
				    !resist(mtmp, '\0', 0, NOTELL)) {
				if (u.ualign.type == A_CHAOTIC && !mtmp->mfrenzied) {
				    mtmp->mpeaceful = 1;
				    set_malign(mtmp);
				} else { /* damn them */
				    killed(mtmp);

					/* train spirituality... it will be harder for chaotics, because they can't damn the monster,
					 * but oh well; chaotic is already the strongest alignment so this is okay --Amy */
					use_skill(P_SPIRITUALITY, Role_if(PM_PRIEST) ? 5 : 1);

				}
				break;
			    } /* else flee */
			    /*FALLTHRU*/
			default:
			    monflee(mtmp, rnd(10), FALSE, TRUE);
			    break;
			}
		    }
	    }
	}
	nomul(-(2 + rnd(3)), "trying to turn undead monsters", TRUE); /* used to always be -2 which felt a little overpowered --Amy */
	adjalign(-rnd(20)); /* a little cost for using the ability... but if you kill a bunch by using it, you'll probably gain more alignment than it costs --Amy */
	nomovemsg = 0;
	return(1);
}

int
turn_allmonsters()
{
	struct monst *mtmp, *mtmp2;

	for(mtmp = fmon; mtmp; mtmp = mtmp2) {
	    mtmp2 = mtmp->nmon;

	    if (DEADMONSTER(mtmp)) continue;
	    if (distu(mtmp->mx,mtmp->my) > 3) continue;

	    if (!mtmp->mpeaceful) {

		    mtmp->msleeping = 0;
			monflee(mtmp, rnd(10), FALSE, TRUE);
	    }
	}
	return(1);
}

const char *
a_gname()
{
    return(a_gname_at(u.ux, u.uy));
}

const char *
a_gname_at(x,y)     /* returns the name of an altar's deity */
xchar x, y;
{
    if(!IS_ALTAR(levl[x][y].typ)) return((char *)0);

    return align_gname(a_align(x,y));
}

const char *
u_gname()  /* returns the name of the player's deity */
{
    return align_gname(u.ualign.type);
}

const char *
align_gname(alignment)
aligntyp alignment;
{
    const char *gnam;
    int which;

	if (Hallucination) { /* come on, hallucination should always affect god's names. --Amy */

	    which = randrole();
	    switch (rn2(3)) {
	     case 0:	gnam = roles[which].lgod; break;
	     case 1:	gnam = roles[which].ngod; break;
	     case 2:	gnam = roles[which].cgod; break;
	     default:	gnam = 0; break;		/* lint suppression */
	    }
	    if (!gnam) gnam = Role_if(PM_GANG_SCHOLAR) ? ScholarMoloch : Role_if(PM_WALSCHOLAR) ? ScholarMoloch : Moloch;
	    if (*gnam == '_') ++gnam;
	    return gnam;
	}

	if (isheretic || Role_if(PM_FAILED_EXISTENCE) || Role_if(PM_TRANSSYLVANIAN) || Role_if(PM_PRIEST) || Role_if(PM_MYSTIC) || Role_if(PM_SHAPESHIFTER) || Role_if(PM_GUNNER) || Role_if(PM_WILD_TALENT) || Role_if(PM_ERDRICK) || Role_if(PM_ANACHRONIST) ) {

    switch (alignment) {
     case A_NONE:	gnam = Role_if(PM_GANG_SCHOLAR) ? ScholarMoloch : Role_if(PM_WALSCHOLAR) ? ScholarMoloch : Moloch; break;
     case A_LAWFUL:	gnam = u.hereticlgod; break;
     case A_NEUTRAL:	gnam = u.hereticngod; break;
     case A_CHAOTIC:	gnam = u.hereticcgod; break;
     default:		impossible("unknown alignment.");
			gnam = "someone"; break;
	    }

	} else {

    switch (alignment) {
     case A_NONE:	gnam = Role_if(PM_GANG_SCHOLAR) ? ScholarMoloch : Role_if(PM_WALSCHOLAR) ? ScholarMoloch : Moloch; break;
     case A_LAWFUL:	gnam = urole.lgod; break;
     case A_NEUTRAL:	gnam = urole.ngod; break;
     case A_CHAOTIC:	gnam = urole.cgod; break;
     default:		impossible("unknown alignment.");
			gnam = "someone"; break;
	    }

	}

    if (*gnam == '_') ++gnam;
    return gnam;
}

/* hallucination handling for priest/minion names: select a random god
   iff character is hallucinating */
const char *
halu_gname(alignment)
aligntyp alignment;
{
    const char *gnam;
    int which;

    if (!Hallucination) return align_gname(alignment);

    which = randrole();
    switch (rn2(3)) {
     case 0:	gnam = roles[which].lgod; break;
     case 1:	gnam = roles[which].ngod; break;
     case 2:	gnam = roles[which].cgod; break;
     default:	gnam = 0; break;		/* lint suppression */
    }
    if (!gnam) gnam = Role_if(PM_GANG_SCHOLAR) ? ScholarMoloch : Role_if(PM_WALSCHOLAR) ? ScholarMoloch : Moloch;
    if (*gnam == '_') ++gnam;
    return gnam;
}

/* deity's title */
const char *
align_gtitle(alignment)
aligntyp alignment;
{
    const char *gnam, *result = "god";


	if (isheretic || Role_if(PM_TRANSSYLVANIAN) || Role_if(PM_FAILED_EXISTENCE) || Role_if(PM_PRIEST) || Role_if(PM_MYSTIC) || Role_if(PM_SHAPESHIFTER) || Role_if(PM_GUNNER) || Role_if(PM_WILD_TALENT) || Role_if(PM_ERDRICK) || Role_if(PM_ANACHRONIST) ) {

    switch (alignment) {
     case A_LAWFUL:	gnam = u.hereticlgod; break;
     case A_NEUTRAL:	gnam = u.hereticngod; break;
     case A_CHAOTIC:	gnam = u.hereticcgod; break;
     default:		gnam = 0; break;
	    }

	} else {

    switch (alignment) {
     case A_LAWFUL:	gnam = urole.lgod; break;
     case A_NEUTRAL:	gnam = urole.ngod; break;
     case A_CHAOTIC:	gnam = urole.cgod; break;
     default:		gnam = 0; break;
	    }

	}

    /*switch (alignment) {
     case A_LAWFUL:	gnam = urole.lgod; break;
     case A_NEUTRAL:	gnam = urole.ngod; break;
     case A_CHAOTIC:	gnam = urole.cgod; break;
     default:		gnam = 0; break;
    }*/
    if (gnam && *gnam == '_') result = "goddess";
    return result;
}

void
altar_wrath(x, y)
register int x, y;
{
    aligntyp altaralign = a_align(x,y);

    if(!strcmp(align_gname(altaralign), u_gname())) {
	godvoice(altaralign, "How darest thou desecrate my altar!");
	(void) adjattrib(A_WIS, -1, FALSE, TRUE);
    } else {
	pline("A voice (could it be %s?) whispers:",
	      align_gname(altaralign));
	verbalize("Thou shalt pay, infidel!");
	change_luck(-1);
    }
}

/* assumes isok() at one space away, but not necessarily at two */
STATIC_OVL boolean
blocked_boulder(dx,dy)
int dx,dy;
{
    register struct obj *otmp;
    long count = 0L;

    for(otmp = level.objects[u.ux+dx][u.uy+dy]; otmp; otmp = otmp->nexthere) {
	if(otmp->otyp == BOULDER)
	    count += otmp->quan;
    }

    switch(count) {
	case 0: return FALSE; /* no boulders--not blocked */
	case 1: break; /* possibly blocked depending on if it's pushable */
	default: return TRUE; /* >1 boulder--blocked after they push the top
	    one; don't force them to push it first to find out */
    }

    if (!isok(u.ux+2*dx, u.uy+2*dy))
	return TRUE;
    if (IS_ROCK(levl[u.ux+2*dx][u.uy+2*dy].typ))
	return TRUE;
    if (sobj_at(BOULDER, u.ux+2*dx, u.uy+2*dy))
	return TRUE;

    return FALSE;
}

/* Give away something */
void
god_gives_benefit(alignment)
aligntyp alignment;
{
	register struct obj *otmp;
	const char *what = (const char *)0;
	
	if (!rnl(30 + u.ulevel)) god_gives_pet(alignment);
	else {
		switch (rnl(4)) {
				/* increase weapon bonus */
			case 0: /* randomly charge an object */
			case 1: /* randomly identify items in the backpack */
			case 2: /* do magic mapping */
			case 3: /* give some food */
			case 4: /* randomly bless items */
		    /* weapon takes precedence if it interferes
		       with taking off a ring or shield */

		    if (uwep && !uwep->blessed) /* weapon */
			    otmp = uwep;
		    else if (uswapwep && !uswapwep->blessed) /* secondary weapon */
			    otmp = uswapwep;
		    /* gloves come next, due to rings */
		    else if (uarmg && !uarmg->blessed)    /* gloves */
			    otmp = uarmg;
		    /* then shield due to two handed weapons and spells */
		    else if (uarms && !uarms->blessed)    /* shield */
			    otmp = uarms;
		    /* then cloak due to body armor */
		    else if (uarmc && !uarmc->blessed)    /* cloak */
			    otmp = uarmc;
		    else if (uarm && !uarm->blessed)      /* armor */
			    otmp = uarm;
		    else if (uarmh && !uarmh->blessed)    /* helmet */
			    otmp = uarmh;
		    else if (uarmf && !uarmf->blessed)    /* boots */
			    otmp = uarmf;
		    else if (uarmu && !uarmu->blessed)    /* shirt */
			    otmp = uarmu;
		    /* (perhaps amulet should take precedence over rings?) */
		    else if (uleft && !uleft->blessed)
			    otmp = uleft;
		    else if (uright && !uright->blessed)
			    otmp = uright;
		    else if (uamul && !uamul->blessed) /* amulet */
			    otmp = uamul;
		    else if (uimplant && !uimplant->blessed) /* implant */
			    otmp = uimplant;
		    else {
			    for(otmp=invent; otmp; otmp=otmp->nobj)
				if (!otmp->blessed)
					break;
			    return; /* Nothing to do! */
		    }
		    if (!stack_too_big(otmp)) bless(otmp);
		    otmp->bknown = TRUE;
		    if (!Blind)
			    Your("%s %s.",
				 what ? what :
				 (const char *)aobjnam (otmp, "softly glow"),
				 hcolor(NH_AMBER));
			break;
		}
	}
}
/*pray.c*/
